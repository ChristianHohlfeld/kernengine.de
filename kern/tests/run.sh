#!/usr/bin/env bash
# Builds and runs the Kern unit tests natively (host compiler, sanitizers on).
#   kern/tests/run.sh            (CXX / CC override the compilers, default c++ / cc)
#
# Box3D is compiled once without sanitizers (third-party C code, scalar path as
# in WebAssembly) and cached; all Kern C++ is built with ASan and UBSan.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(dirname "$here")"
out="${KERN_TEST_OUT:-${TMPDIR:-/tmp}/kern-tests}"
mkdir -p "$out/box3d"
cc="${CC:-cc}"
objects=()
pids=()
for c in "$root"/third_party/box3d/src/*.c; do
  o="$out/box3d/$(basename "$c" .c).o"
  objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ] || [ -n "$(find "$root/third_party/box3d" -name '*.h' -newer "$o" -o -name '*.inl' -newer "$o" | head -1)" ]; then
    "$cc" -std=c17 -O2 -g -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I"$root/third_party/box3d/include" -I"$root/third_party/box3d/src" -c "$c" -o "$o" &
    pids+=($!)
  fi
done
for p in "${pids[@]}"; do wait "$p"; done
# The deterministic libm (kern/core/dmath.h).
mkdir -p "$out/musl_math"
for c in "$root"/third_party/musl_math/src/*.c; do
  o="$out/musl_math/$(basename "$c" .c).o"
  objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ] || [ -n "$(find "$root/third_party/musl_math" -name '*.h' -newer "$o" | head -1)" ]; then
    "$cc" -std=c17 -O2 -g -ffp-contract=off -fno-math-errno -w -c "$c" -o "$o"
  fi
done
sources=()
while IFS= read -r f; do sources+=("$f"); done < <(find "$root/src" -name '*.cpp' | sort)
sources+=("$here/gl_stub.cpp" "$here/test_main.cpp")
# One object per source, compiled in parallel (all cores).
cxx="${CXX:-c++}"
flags=(-std=c++20 -O1 -g -ffp-contract=off -fno-exceptions -fno-rtti -fsanitize=address,undefined -fno-sanitize-recover=undefined
       -Wall -Wextra -Wno-unused-parameter -Werror -I"$root/include" -I"$root/third_party/box3d/include" -I"$here")
mkdir -p "$out/obj"
export cxx
printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c 'src="$1"; shift; o="'"$out"'/obj/$(echo "$src" | sed "s|/|_|g").o"; "$cxx" "$@" -c "$src" -o "$o"' _ {} "${flags[@]}"
cpp_objects=()
for f in "${sources[@]}"; do cpp_objects+=("$out/obj/$(echo "$f" | sed "s|/|_|g").o"); done
"$cxx" -fsanitize=address,undefined "${cpp_objects[@]}" "${objects[@]}" -lm -o "$out/kern_tests"
(cd "$here" && "$out/kern_tests" "$@")
# Blueprints compiled as C++ must behave exactly like the interpreter.
if [ $# -eq 0 ]; then
  core=()
  for f in "${cpp_objects[@]}"; do case "$f" in *_src_logic_*|*_src_core_*) core+=("$f") ;; esac; done
  # A Blueprint's C++ text (docs/blueprint_cpp.md), compiled as it is, computes what the
  # interpreter computes: every call of every case, value for value - and the tier cases
  # (logic_cases.h) play their scenarios as classes.
  nat="$out/native_gen"
  rm -rf "$nat" && mkdir -p "$nat"
  "$out/kern_tests" --emit-native "$nat"
  "$cxx" "${flags[@]}" -I"$nat" "$nat/blueprint_native_main.cpp" "${core[@]}" "$out"/musl_math/*.o -lm -o "$out/blueprint_native_test"
  "$out/blueprint_native_test" "$nat/blueprint_native_expected.txt"
  # The Blueprints of a game as classes: the levels of game_cases.h, written as level files; their
  # Blueprints as C++ by the tool a game's build uses (tools/blueprint_cpp), compiled and linked
  # with the engine. Every scenario, played in the interpreter and as those classes, is the same.
  gam="$out/game_native_gen"
  rm -rf "$gam" && mkdir -p "$gam"
  "$out/kern_tests" --emit-game-native "$gam"
  shapes="$out/obj/$(echo "$root/src/game/logic_node_shapes.cpp" | sed "s|/|_|g").o"
  "$cxx" "${flags[@]}" -w "$root/../tools/blueprint_cpp.cpp" "$shapes" "${core[@]}" "$out"/musl_math/*.o -lm -o "$out/blueprint_cpp"
  for level in "$gam"/*.klevel; do "$out/blueprint_cpp" "$level" "${level%.klevel}_blueprints.cpp"; done
  engine=()
  for f in "${cpp_objects[@]}"; do case "$f" in *test_main.cpp.o) ;; *) engine+=("$f") ;; esac; done
  "$cxx" "${flags[@]}" "$here/game_native_test.cpp" "$gam"/*_blueprints.cpp "${engine[@]}" "${objects[@]}" -lm -o "$out/game_native_test"
  "$out/game_native_test" "$gam"
file; its
  # Blueprints as C++ by the tool a game's build uses (tools/blueprint_cpp), compiled and linked
  # with the engine. The scenario, played in the interpreter and as those classes, is the same.
  gam="$out/game_native_gen"
  rm -rf "$gam" && mkdir -p "$gam"
  "$out/kern_tests" --emit-game-native "$gam"
  shapes="$out/obj/$(echo "$root/src/game/logic_node_shapes.cpp" | sed "s|/|_|g").o"
  "$cxx" "${flags[@]}" -w "$root/../tools/blueprint_cpp.cpp" "$shapes" "${core[@]}" "$out"/musl_math/*.o -lm -o "$out/blueprint_cpp"
  "$out/blueprint_cpp" "$gam/arena.klevel" "$gam/arena_blueprints.cpp"
  engine=()
  for f in "${cpp_objects[@]}"; do case "$f" in *test_main.cpp.o) ;; *) engine+=("$f") ;; esac; done
  "$cxx" "${flags[@]}" "$here/game_native_test.cpp" "$gam/arena_blueprints.cpp" "${engine[@]}" "${objects[@]}" -lm -o "$out/game_native_test"
  "$out/game_native_test" "$gam/arena.klevel"
fi
