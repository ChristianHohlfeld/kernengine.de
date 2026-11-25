#!/usr/bin/env bash
# The whole game as classes (BLUEPRINTS.md, "5 - Umschalten": the one step at the end): every Blueprint of the folder
# "Source" taken as one of "Program" - in a copy of the level, the level stays as it is - written by tools/blueprint_cpp.sh
# into a header per class and a source per class's glue, and every one of these files compiled by itself, as the
# game's build compiles them. Says per file whether it compiles and how much memory its compiler took, and counts the
# errors by their message.
#
#   kern/games/spawncity/tools/all_program.sh <dir>     (everything goes into <dir>; KERN_JOBS files at a time)
#
# CI: workflow_dispatch of ci.yml with probe="all_program".
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
root="$(cd "$here/../../.." && pwd)"
out="$(mkdir -p "$1" && cd "$1" && pwd)"
cxx="${CXX:-c++}"
python3 - "$here/level/spawncity.klevel" "$out/all.klevel" <<'PY'
import sys
s = open(sys.argv[1]).read()
open(sys.argv[2], "w").write(s.replace('"folder": "Source"', '"folder": "Program"'))
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
