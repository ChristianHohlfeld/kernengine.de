#!/usr/bin/env bash
# The whole game as classes (BLUEPRINTS.md, "5 - Umschalten": the one step at the end): every Blueprint of the folder
# "Source" taken as one of "Program" - in a copy of the level, the level stays as it is - written by tools/blueprint_cpp.sh
# into a header per class and a source per class's glue, and every one of these files compiled by itself, as the
# game's build compiles them. Says per file whether it compiles and how much memory its compiler took, and counts the
# errors by their message.
#
#   kern/games/spawncity/tools/all_program.sh <dir>     (everything goes into <dir>; KERN_JOBS files at a time)
#
# REFRESH="src/a.cpp ..." first converts these sources into the copy of the level (tools/cpp_to_blueprint.py, as the full
# run does): the Blueprints of the engine's headers they use are written anew. LINK=1 then compiles every file and the
# engine, links them with tools/all_program_run.cpp and plays the level without a window (Begin and TICKS steps); a link
# that fails says which symbols are missing.
#
# CI: workflow_dispatch of ci.yml with probe="all_program".
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
root="$(cd "$here/../../.." && pwd)"
out="$(mkdir -p "$1" && cd "$1" && pwd)"
cxx="${CXX:-c++}"
cp "$here/level/spawncity.klevel" "$out/all.klevel"
if [ -n "${REFRESH:-}" ]; then
  (cd "$here" && mkdir -p "$out/refresh/converted" "$out/refresh/texts" && cp tests/*.h "$out/refresh/" &&
   CLANGXX="${CLANGXX:-clang++}" FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert sc --program firmware=fc,hwcontract \
     -I../../include -I../../third_party/box3d/include -Isrc --level "$out/all.klevel" --test "$out/refresh/converted" --texts "$out/refresh/texts" \
     -- $REFRESH > "$out/refresh.log" 2>&1)
  echo "refresh ($REFRESH): rc=$? - $(tail -n 3 "$out/refresh.log" | tr '\n' ' ')"
fi
python3 - "$out/all.klevel" <<'PY'
import sys
s = open(sys.argv[1]).read()
open(sys.argv[1], "w").write(s.replace('"folder": "Source"', '"folder": "Program"'))
PY
mkdir -p "$out/gen"
"$root/tools/blueprint_cpp.sh" "$out/all.klevel" "$out/gen/level_blueprints.cpp" "$out/gen/level_blueprints.h" Program || exit 1
export cxx root out
compile() {
  local f="$1" log="$out/logs/$(basename "$1" .cpp).log"
  /usr/bin/time -f "peak_kb=%M seconds=%e" "$cxx" -std=c++20 -fsyntax-only -ffp-contract=off -fno-exceptions -fno-rtti -w -fmax-errors=0 \
    -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -I"$out/gen" "$f" > "$log" 2>&1
  echo "rc=$? $(basename "$f") $(grep -o 'peak_kb=[0-9]* seconds=[0-9.]*' "$log" | tail -1) errors=$(grep -c 'error:' "$log")"
}
export -f compile
mkdir -p "$out/logs"
ls "$out"/gen/*.cpp | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c 'compile "$1"' _ {} | sort > "$out/files.txt"
echo "files: $(wc -l < "$out/files.txt"), compile: $(grep -c '^rc=0' "$out/files.txt"), do not: $(grep -vc '^rc=0' "$out/files.txt")"
echo "memory per file (KB): median $(grep -o 'peak_kb=[0-9]*' "$out/files.txt" | cut -d= -f2 | sort -n | awk '{a[NR]=$1} END {print a[int((NR+1)/2)]}'), over 2 GB: $(grep -o 'peak_kb=[0-9]*' "$out/files.txt" | cut -d= -f2 | awk '$1 > 2097152' | wc -l); the ten that take most:"
sed -E 's/^rc=[0-9]+ ([^ ]+) peak_kb=([0-9]+).*/\2 \1/' "$out/files.txt" | sort -n -r | head -10 | while read -r kb f; do
  echo "  $kb KB $f ($(grep -io 'the Blueprint of [^"]*"[^"]*"' "$out/gen/$f" | head -1))"; done
grep -v '^rc=0' "$out/files.txt" | head -50
echo "errors by message:"
cat "$out"/logs/*.log | grep 'error:' | sed -E 's/^[^ ]* error: //; s/kern_blueprint_[0-9a-f]+::/X::/g' | cut -c1-200 | sort | uniq -c | sort -rn | head -60

[ "${LINK:-}" = 1 ] || exit 0
# ---- the whole program linked with the engine, and started
k="$root/kern"; obj="$out/obj"; mkdir -p "$obj"
flags=(-std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I"$k/include" -I"$k/third_party/box3d/include" -I"$k/tests" -I"$out/gen")
for c in "$k"/third_party/box3d/src/*.c; do cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I"$k/third_party/box3d/include" -I"$k/third_party/box3d/src" -c "$c" -o "$obj/box3d_$(basename "$c" .c).o" & done; wait
for c in "$k"/third_party/musl_math/src/*.c; do cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$c" -o "$obj/musl_$(basename "$c" .c).o" & done; wait
export obj; printf '%s\n' $(find "$k/src" -name '*.cpp' | sort) "$k/tests/gl_stub.cpp" "$out"/gen/*.cpp "$here/tools/all_program_run.cpp" |
  xargs -P "${LINK_JOBS:-3}" -I{} bash -c 'o="$obj/$(echo "$0" | sed "s|/|_|g").o"; "$cxx" "$@" -c "$0" -o "$o" 2> "$o.log" || { echo "does not compile: $0 ($(grep -c error: "$o.log") errors)"; rm -f "$o"; }' {} "${flags[@]}"
if "$cxx" "$obj"/*.o -lm -o "$out/all_program_run" 2> "$out/link.log"; then
  echo "linked: $(du -h "$out/all_program_run" | cut -f1)"
  "$out/all_program_run" "$out/all.klevel" "${TICKS:-600}" > "$out/run.log" 2>&1; echo "run rc=$?"; head -n 80 "$out/run.log"
else
  echo "link fails: $(grep -c 'undefined reference' "$out/link.log") undefined references, $(grep -o "undefined reference to \`[^']*'" "$out/link.log" | sort -u | wc -l) symbols:"
  grep -o "undefined reference to \`[^']*'" "$out/link.log" | sort | uniq -c | sort -rn | head -80
  grep -v 'undefined reference' "$out/link.log" | grep -i error | head -20
fi
