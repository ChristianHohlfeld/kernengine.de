#!/usr/bin/env bash
# The comparison tests a run of the converter wrote, built with the engine and run (tools/converter_cases/run.sh, the CI's
# probe with source_numbers):
#   tools/converter_cases/compare.sh <project> <level> <tests folder> <out folder> [<source> ...]
# <tests folder>: the converter's --test (test_<x>.cpp and - converted with KERN_TEST_SOURCE_NUMBERS=1 - test_<x>_source.cpp).
# Built as a game's tests are: the project embedded (kern/tools/embed_project.py), its Blueprints as classes
# (tools/blueprint_cpp.sh --project, the header level_blueprints.h beside them), the engine (kern/src without studio and
# teacher), kern/tools/test_main.cpp, and the program's own <source>s (what the tests' sources call of other files). INCLUDES:
# more -I switches (the program's folders). Each test and each source is compiled on its own: one that does not compile is
# said and left out (a source whose functions are then missing: linked without them, said). What the tests print goes to
# <out folder>/compare.log; the status is the build's - the tests' own verdict is read from the log (tools/source_numbers.py).
# The engine's objects are kept in $CASES_OBJ (default $TMPDIR/kern-cases-obj), keyed by their source, the switches and every
# header of the engine.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
kern="$root/kern"
project="$1" level="$2" tests="$3" o="$4"; shift 4
out="${CASES_OBJ:-${TMPDIR:-/tmp}/kern-cases-obj}"
cxx="${CXX:-${CLANGXX:-clang++}}"
cc="${CC:-$(dirname "$(command -v "$cxx")")/$(basename "$cxx" | sed 's/clang++/clang/; s/g++/gcc/')}"
mkdir -p "$out/box3d" "$out/musl" "$out/obj" "$o/obj"
read -r -a more <<< "${INCLUDES:-}"
flags=(-std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$kern/tests")
jobs=() objects=()
for c in "$kern"/third_party/box3d/src/*.c; do
  x="$out/box3d/$(basename "$c" .c).o"; objects+=("$x")
  [ -f "$x" ] || jobs+=("$cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I$kern/third_party/box3d/include -I$kern/third_party/box3d/src -c $c -o $x")
done
for c in "$kern"/third_party/musl_math/src/*.c; do
  x="$out/musl/$(basename "$c" .c).o"; objects+=("$x")
  [ -f "$x" ] || jobs+=("$cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $x")
done
headers="$(find "$kern/include" -name '*.h' | sort | xargs cat | sha256sum | cut -c1-12)"
while IFS= read -r f; do
  x="$out/obj/$(echo "$f" | sed "s|$kern/||; s|/|_|g")-$( { echo "$headers ${flags[*]} $cxx"; cat "$f"; } | sha256sum | cut -c1-12).o"
  objects+=("$x")
  [ -f "$x" ] || jobs+=("$cxx ${flags[*]} -c $f -o $x")
done < <(find "$kern/src" -name '*.cpp' -not -path '*/studio/*' -not -path '*/lehrer/*' | sort; echo "$kern/tests/gl_stub.cpp"; echo "$kern/tools/test_memory.cpp"; echo "$kern/tools/test_main.cpp")
if [ ${#jobs[@]} -gt 0 ]; then
  echo "(the engine: ${#jobs[@]} objects to build)"
  printf '%s\n' "${jobs[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} sh -c '{}'
fi
# (a level written without format and version - the converter's cases write it so: as a level is read)
python3 -c "import json,sys; p=sys.argv[1]; d=json.load(open(p)); d.setdefault('format','kern-level'); d.setdefault('version',1); json.dump(d,open(p,'w'))" "$level"
python3 "$kern/tools/embed_project.py" "$project" "$o/project.cpp" > /dev/null
# (the header of the classes the program calls - level_blueprints.h - where its tests name it: the converter's --classes)
if grep -qs 'level_blueprints.h' "$tests"/test_*.cpp; then
  "$root/tools/blueprint_cpp.sh" --project "$project" "$level" "$o/level_blueprints.cpp" "$o/level_blueprints.h" > /dev/null
else
  "$root/tools/blueprint_cpp.sh" --project "$project" "$level" "$o/level_blueprints.cpp" > /dev/null
fi
own=("$o/project.cpp" "$o/level_blueprints.cpp" "$tests"/test_*.cpp "$@")
export cxx o
printf '%s\n' "${own[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
  s="$1"; shift; x="$o/obj/$(basename "$s" .cpp).o"
  "$cxx" "$@" -c "$s" -o "$x" 2> "$x.log" || { rm -f "$x"; echo "does not compile: $s: $(grep -m1 "error:" "$x.log" | cut -c1-300)"; }' _ {} "${flags[@]}" -I"$o" "${more[@]}"
built=()
for s in "${own[@]}"; do x="$o/obj/$(basename "$s" .cpp).o"; [ -f "$x" ] && built+=("$x"); done
if ! "$cxx" "${built[@]}" "${objects[@]}" -lm -o "$o/compare" 2> "$o/link.log"; then
  echo "linked without what is missing ($(grep -c 'undefined reference' "$o/link.log") references - $o/link.log): a test that calls it ends there"
  "$cxx" "${built[@]}" "${objects[@]}" -lm -Wl,--unresolved-symbols=ignore-all -o "$o/compare"
fi
"$o/compare" > "$o/compare.log" 2>&1 || true
