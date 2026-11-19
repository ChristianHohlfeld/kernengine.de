#!/usr/bin/env bash
# Builds and runs SpawnCity's native unit tests (host compiler, no GL, sanitizers on).
#   kern/games/spawncity/tests/run_tests.sh [name-filter]
# CXX / CC override the compilers (default c++ / cc). Objects are cached in
# $SC_TEST_OUT (default $TMPDIR/spawncity-tests); Kern itself is not modified.
# SC_TEST_RELEASE=1 builds them as the game is built (-O2, no sanitizers; objects in
# $SC_TEST_OUT-release): for the times a test prints, e.g. `SC_TEST_RELEASE=1 run_tests.sh details_blueprint`.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
out="${SC_TEST_OUT:-${TMPDIR:-/tmp}/spawncity-tests}"
build=(-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined) link=(-fsanitize=address,undefined)
if [ "${SC_TEST_RELEASE:-}" = 1 ]; then build=(-O2) link=() out="$out-release"; fi
# The compiler: clang - the compiler that builds the game (emscripten is clang). Where C++ leaves a meaning
# open (the order of a call's arguments, [expr.call]/8), the source means what the game's compiler makes of it,
# and the tests compare against exactly that (decision E13). CXX/CC choose others; without them clang is looked
# for as the converter looks for it: CLANGXX, clang++, clang++-20 ... clang++-16 (and the clang beside it).
# No clang: the run fails - it does not fall back to another compiler.
if [ -z "${CXX:-}" ]; then
  for c in "${CLANGXX:-}" clang++ clang++-20 clang++-19 clang++-18 clang++-17 clang++-16; do
    if [ -n "$c" ] && command -v "$c" >/dev/null; then CXX="$c"; break; fi
  done
  if [ -z "${CXX:-}" ]; then echo "FAIL: no clang (CLANGXX, clang++, clang++-20 ... clang++-16) - the tests are built with the game's compiler"; exit 1; fi
fi
if [ -z "${CC:-}" ]; then
  CC="$(dirname "$(command -v "$CXX")")/$(basename "$CXX" | sed 's/clang++/clang/')"
  if ! command -v "$CC" >/dev/null; then echo "FAIL: no C compiler beside $CXX ($CC)"; exit 1; fi
fi
cc="$CC" cxx="$CXX"
echo "compilers: $("$cxx" --version | head -1) / $cc"
# (objects of each compiler apart: its name and version are part of every cache key and folder)
tool="$({ "$cxx" --version | head -1; "$cc" --version | head -1; } | sha256sum | cut -c1-8)"
mkdir -p "$out/box3d-$tool" "$out/musl-$tool" "$out/obj"
objects=()
pids=()
for c in "$kern"/third_party/box3d/src/*.c; do
  o="$out/box3d-$tool/$(basename "$c" .c).o"; objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ]; then
    "$cc" -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I"$kern/third_party/box3d/include" -I"$kern/third_party/box3d/src" -c "$c" -o "$o" & pids+=($!)
  fi
done
for c in "$kern"/third_party/musl_math/src/*.c; do
  o="$out/musl-$tool/$(basename "$c" .c).o"; objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ]; then "$cc" -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$c" -o "$o" & pids+=($!); fi
done
for p in "${pids[@]}"; do wait "$p"; done
# The level file (the project: world settings, actors, their Blueprints) as a source the game links.
python3 "$game/tools/embed_level.py" "$game/level/spawncity.klevel" "$game/src/level_data.cpp"
# ... and its Blueprints as C++ classes, as the game links them: the tests run the rules as the game does.
"$kern/../tools/blueprint_cpp.sh" "$game/level/spawncity.klevel" "$game/src/level_blueprints.cpp"
# Kern's engine-side pieces the models use (physics, geometry builders, the renderer headless on
# Kern's GL stub, Kern Logic for the rules that are Blueprints, the game layer for the Blueprints
# that ask the world - a ray, an actor), every component the game is built with (kern/build.sh --app
# --game: all of kern/src but the editor's and the teacher's - the app runtime too, whose Engine
# and Input are objects of the engine the source calls, and the C API it stands on; the tests' factory of
# the app is test_main.cpp's), then the game's simulation sources (not the Game itself) and the tests.
sources=()
for d in core physics geo gfx scene audio net logic api app game; do while IFS= read -r f; do sources+=("$f"); done < <(find "$kern/src/$d" -name '*.cpp' | sort); done
sources+=("$kern/tests/gl_stub.cpp")
# Every game source that does not need GameWorld (not main.cpp, nothing that includes game.h).
for f in "$game"/src/*.cpp; do [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f" || sources+=("$f"); done
while IFS= read -r f; do sources+=("$f"); done < <(find "$here" -name 'test_*.cpp' | sort)
flags=(-std=c++20 "${build[@]}" -ffp-contract=off -fno-exceptions -fno-rtti
       -Wall -Wextra -Wno-unused-parameter -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" -I"$kern/tests")
# Header changes rebuild what includes them (the cache key includes every header's content): a header
# of the engine or the game everything, a header of the tests only the tests.
key="$({ printf '%s ' "$tool" "${flags[@]}"; find "$kern/include" "$game" -path "$here" -prune -o \( -name '*.h' -o -name '*.hpp' \) -print | sort | xargs cat; } | sha256sum | cut -c1-12)"
test_key="$({ printf '%s ' "$key"; find "$here" -name '*.h' -o -name '*.hpp' | sort | xargs cat; } | sha256sum | cut -c1-12)"
export cxx out key test_key here
object_of() { case "$1" in "$here"/*) k="$test_key" ;; *) k="$key" ;; esac; echo "$out/obj/$(echo "$1" | sed "s|/|_|g")-$k-$(sha256sum "$1" | cut -c1-12).o"; }
export -f object_of
printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
  src="$1"; shift
  o="$(object_of "$src")"
  [ -f "$o" ] || "$cxx" "$@" -c "$src" -o "$o"' _ {} "${flags[@]}"
cpp_objects=()
for f in "${sources[@]}"; do cpp_objects+=("$(object_of "$f")"); done
"$cxx" "${link[@]}" "${cpp_objects[@]}" "${objects[@]}" -lm -o "$out/spawncity_tests"
# (every test runs, whatever an earlier one gave - see test_main.cpp; the run's result is that of all of it)
rc=0
"$out/spawncity_tests" "$@" || rc=1
# The converted functions once more against the engine itself: the source and its Blueprint (interpreter,
# class) call the real functions of the engine's nodes, each side with objects of its own, set up alike
# for every call - the description of every node they call has to fit the real function (rule_blueprint.h).
if [ $# -eq 0 ]; then SC_ENGINE=real "$out/spawncity_tests" converted_ || rc=1; fi
# The game's own sources - what includes game.h and is not linked into the tests - compile: a
# rule's header that changed is caught here, not first by the game's build.
if [ $# -eq 0 ]; then
  rest=()
  for f in "$game"/src/*.cpp; do if [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f"; then rest+=("$f"); fi; done
  printf '%s\n' "${rest[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} "$cxx" -std=c++20 -fsyntax-only -fno-exceptions -fno-rtti -Wall -Wextra -Wno-unused-parameter \
    -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" {}
  echo "ok   the game's own sources compile (${#rest[@]} files)"
fi
# The rules' Blueprint as its C++ text, compiled as it is by the host compiler (nothing looked up
# at run time), gives what the interpreter gives: every call, value for value.
if [ $# -eq 0 ]; then
  nat="$out/native"
  rm -rf "$nat" && mkdir -p "$nat"
  "$out/spawncity_tests" --emit-native "$nat"
  # (the engine, as the classes call it: Kern Logic, and the game layer for the nodes that ask the world)
  engine_objects=()
  for f in "${cpp_objects[@]}"; do case "$f" in *_kern_src_*|*_kern_tests_gl_stub*) engine_objects+=("$f") ;; esac; done
  "$cxx" "${flags[@]}" -Werror -I"$nat" "$nat/blueprint_native_main.cpp" "${engine_objects[@]}" "${objects[@]}" -lm -o "$out/wanted_native_test"
  "$out/wanted_native_test" "$nat/blueprint_native_expected.txt"
fi
exit "$rc"
