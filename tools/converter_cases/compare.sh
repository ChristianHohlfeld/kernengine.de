#!/usr/bin/env bash
# The comparison tests a run of the converter wrote, built with the engine and run (tools/converter_cases/run.sh, the CI's
# probe):
#   tools/converter_cases/compare.sh <project> <level> <tests folder> <out folder> [<source> ...]
# <tests folder>: the converter's --test (test_<x>.cpp: each Blueprint's comparison with its source, as C++ computes it).
# The tests and the program's sources are built under UBSan (-fsanitize=undefined): what C++ leaves undefined in a call
# of the source says so in the log ("runtime error:" - tools/source_numbers.py counts it); the engine is built as always.
# Built as a game's tests are: the project embedded (kern/tools/embed_project.py), its Blueprints as classes
# (tools/blueprint_cpp.sh --project, the header level_blueprints.h beside them), the engine (kern/src without studio and
# teacher), kern/tools/test_main.cpp, and the program's own <source>s (what the tests' sources call of other files). INCLUDES:
# more -I switches (the program's folders). Each test and each source is compiled on its own: one that does not compile is
# said and left out (a source whose functions are then missing: linked without them, said). What the tests print goes to
# <out folder>/compare.log (the program: tests.bin - no name of a header, the folder is on the include path); the status is the build's - the tests' own verdict is read from the log (tools/source_numbers.py).
# The engine's objects stand in the folder of this run (kern/tools/run_dir.sh - nothing is kept from one run to the next),
# keyed by their source, the switches and every header of the engine.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
kern="$root/kern"
project="$1" level="$2" tests="$3" o="$4"; shift 4
. "$kern/tools/run_dir.sh"
out="$KERN_RUN_DIR/kern-cases-obj"
cxx="${CXX:-${CLANGXX:-clang++}}"
cc="${CC:-$(dirname "$(command -v "$cxx")")/$(basename "$cxx" | sed 's/clang++/clang/; s/g++/gcc/')}"
mkdir -p "$out/box3d" "$out/musl" "$out/obj" "$o/obj"
read -r -a more <<< "${INCLUDES:-}"
. "$kern/third_party/musl_math/flags.sh"; read -r -a mathflags <<< "$(kern_math_flags "$cxx")"
flags=(-std=c++20 -O1 -ffp-contract=off "${mathflags[@]}" -fno-exceptions -fno-rtti -w -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$kern/tests")
jobs=() objects=()
for c in "$kern"/third_party/box3d/src/*.c; do
  x="$out/box3d/$(basename "$c" .c).o"; objects+=("$x")
  [ -f "$x" ] || jobs+=("$cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I$kern/third_party/box3d/include -I$kern/third_party/box3d/src -c $c -o $x")
done
for c in "$kern"/third_party/musl_math/src/*.c "$kern"/src/math/*.c; do
  x="$out/musl/$(basename "$c" .c).o"; objects+=("$x")
  [ -f "$x" ] || jobs+=("$cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $x")
done
headers="$(find "$kern/include" "$kern/src" -name '*.h' -o -name '*.inc' | sort | xargs cat | sha256sum | cut -c1-12)"
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
# (with the header of the classes the program calls - level_blueprints.h, the converter's --classes - where there are such
# classes; else without it, and the header empty: a source of the program that includes it compiles, and what it would call of
# such a class is not there - said by the link below)
if ! "$root/tools/blueprint_cpp.sh" --project "$project" "$level" "$o/level_blueprints.cpp" "$o/level_blueprints.h" > "$o/classes.log" 2>&1; then
  echo "the header of the classes the program calls is empty: $(grep -v '^\s*$' "$o/classes.log" | tail -3 | tr '\n' ' ' | cut -c1-400)"
  "$root/tools/blueprint_cpp.sh" --project "$project" "$level" "$o/level_blueprints.cpp" > /dev/null
  printf '#pragma once\n// (no Blueprint the program calls as its class in this project - tools/converter_cases/compare.sh)\n' > "$o/level_blueprints.h"
fi
# (each class of the header binds in the source of its glue - <out>_kern_blueprint_<id>.cpp beside level_blueprints.cpp, which
# blueprints::bind calls: compiled with it, or the link misses blueprints::bind_kern_blueprint_<id>)
glue=()
for g in "$o"/level_blueprints_kern_blueprint_*.cpp; do [ -f "$g" ] && glue+=("$g"); done
own=("$o/project.cpp" "$o/level_blueprints.cpp" "${glue[@]}" "$tests"/test_*.cpp "$@")
export cxx o
ubsan=(-fsanitize=undefined -fno-sanitize=vptr)
# (no comparison without it: a compiler whose UBSan does not link - clang without compiler-rt - stops here, with the reason)
printf 'int main(int c, char**) { return c << 1; }\n' > "$o/ubsan.cpp"
"$cxx" "${ubsan[@]}" "$o/ubsan.cpp" -o "$o/ubsan.bin" 2> "$o/ubsan.log" || { echo "compare.sh: $cxx builds nothing under UBSan (-fsanitize=undefined): $(head -c 400 "$o/ubsan.log")" >&2; exit 1; }
printf '%s\n' "${own[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
  s="$1"; shift; x="$o/obj/$(basename "$s" .cpp).o"
  "$cxx" "$@" -c "$s" -o "$x" 2> "$x.log" || { rm -f "$x"; echo "does not compile: $s: $(grep -m1 "error:" "$x.log" | cut -c1-300)"; }' _ {} "${flags[@]}" "${ubsan[@]}" -I"$o" "${more[@]}"
built=()
for s in "${own[@]}"; do x="$o/obj/$(basename "$s" .cpp).o"; [ -f "$x" ] && built+=("$x"); done
if ! "$cxx" "${ubsan[@]}" "${built[@]}" "${objects[@]}" -lm -o "$o/tests.bin" 2> "$o/link.log"; then
  echo "linked without what is missing ($(grep -c 'undefined reference' "$o/link.log") references - $o/link.log): a test that calls it ends there"
  "$cxx" "${ubsan[@]}" "${built[@]}" "${objects[@]}" -lm -Wl,--unresolved-symbols=ignore-all -o "$o/tests.bin"
fi
"$o/tests.bin" > "$o/compare.log" 2>&1 || true
