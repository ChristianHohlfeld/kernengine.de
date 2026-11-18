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
cc="${CC:-cc}" cxx="${CXX:-c++}"
mkdir -p "$out/box3d" "$out/musl" "$out/obj"
objects=()
pids=()
for c in "$kern"/third_party/box3d/src/*.c; do
  o="$out/box3d/$(basename "$c" .c).o"; objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ]; then
    "$cc" -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I"$kern/third_party/box3d/include" -I"$kern/third_party/box3d/src" -c "$c" -o "$o" & pids+=($!)
  fi
done
for c in "$kern"/third_party/musl_math/src/*.c; do
  o="$out/musl/$(basename "$c" .c).o"; objects+=("$o")
  if [ ! -f "$o" ] || [ "$c" -nt "$o" ]; then "$cc" -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$c" -o "$o" & pids+=($!); fi
done
for p in "${pids[@]}"; do wait "$p"; done
# The level file (the project: world settings, actors, their Blueprints) as a source the game links.
python3 "$game/tools/embed_level.py" "$game/level/spawncity.klevel" "$game/src/level_data.cpp"
# ... and its Blueprints as C++ classes, as the game links them: the tests run the rules as the game does.
"$kern/../tools/blueprint_cpp.sh" "$game/level/spawncity.klevel" "$game/src/level_blueprints.cpp"
# Kern's engine-side pieces the models use (physics, geometry builders, the renderer headless on
# Kern's GL stub, Kern Logic for the rules that are Blueprints, the game layer for the Blueprints
# that ask the world - a ray, an actor), then the game's simulation sources (not the Game itself)
# and the tests.
sources=()
for d in core physics geo gfx scene audio net logic game; do while IFS= read -r f; do sources+=("$f"); done < <(find "$kern/src/$d" -name '*.cpp' | sort); done
sources+=("$kern/tests/gl_stub.cpp")
# Every game source that does not need GameWorld (not main.cpp, nothing that includes game.h).
for f in "$game"/src/*.cpp; do [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f" || sources+=("$f"); done
# The tests - but the differential tests of a lowering (tests/lowered): those are a program of their own,
# built with clang (below).
lowered=()
while IFS= read -r f; do case "$f" in "$here"/lowered/*) lowered+=("$f") ;; *) sources+=("$f") ;; esac; done < <(find "$here" -name 'test_*.cpp' | sort)
flags=(-std=c++20 "${build[@]}" -ffp-contract=off -fno-exceptions -fno-rtti
       -Wall -Wextra -Wno-unused-parameter -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" -I"$kern/tests")
# Header changes rebuild what includes them (the cache key includes every header's content): a header
# of the engine or the game everything, a header of the tests only the tests.
key="$({ printf '%s ' "${flags[@]}"; find "$kern/include" "$game" -path "$here" -prune -o \( -name '*.h' -o -name '*.hpp' \) -print | sort | xargs cat; } | sha256sum | cut -c1-12)"
test_key="$({ printf '%s ' "$key"; find "$here" -name '*.h' -o -name '*.hpp' | sort | xargs cat; } | sha256sum | cut -c1-12)"
export out key test_key here
# (objects of each compiler in a folder of their own: obj for cxx, obj-clang for the differential tests)
object_of() { case "$1" in "$here"/*) k="$test_key" ;; *) k="$key" ;; esac; echo "$out/obj$2/$(echo "$1" | sed "s|/|_|g")-$k-$(sha256sum "$1" | cut -c1-12).o"; }
export -f object_of
# compile_all <compiler> <folder suffix> <sources...>: every source once (cached), in parallel
compile_all() {
  local with="$1" tag="$2"; shift 2
  mkdir -p "$out/obj$tag"
  printf '%s\n' "$@" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
    with="$1"; tag="$2"; src="$3"; shift 3
    o="$(object_of "$src" "$tag")"
    [ -f "$o" ] || "$with" "$@" -c "$src" -o "$o"' _ "$with" "$tag" {} "${flags[@]}"
}
compile_all "$cxx" "" "${sources[@]}"
cpp_objects=()
for f in "${sources[@]}"; do cpp_objects+=("$(object_of "$f" "")"); done
"$cxx" "${link[@]}" "${cpp_objects[@]}" "${objects[@]}" -lm -o "$out/spawncity_tests"
# (every test runs, whatever an earlier one gave - see test_main.cpp; the run's result is that of all of it)
rc=0
"$out/spawncity_tests" "$@" || rc=1
# The differential tests of a lowering (tests/lowered: the source before and after, value for value) with
# clang - the compiler that builds the game (emscripten): where C++ leaves the order open (two arguments of a
# call that draw from the same generator, [expr.call]/8), the lowering keeps the meaning of the game's
# compiler (decision E9), and only under it can its differential test be green. With them, what they need
# of the game and of Kern, built by clang too - the same sources as above, test_main.cpp for the frame.
# No clang: the run fails (nothing is skipped).
if [ "${#lowered[@]}" -gt 0 ]; then
  # (CLANGXX, else clang++, else the newest clang++-<version> on the PATH)
  clangxx="${CLANGXX:-}"
  if [ -z "$clangxx" ]; then
    if command -v clang++ >/dev/null; then clangxx=clang++
    else clangxx="$(compgen -c clang++- | grep -E '^clang\+\+-[0-9]+$' | sort -t- -k2 -n | tail -1 || true)"; fi
  fi
  if [ -z "$clangxx" ] || ! command -v "$clangxx" >/dev/null; then echo "FAIL tests/lowered: no clang (CLANGXX, clang++, clang++-<version>) - the differential tests are built with clang"; exit 1; fi
  echo "tests/lowered: built with $clangxx"
  low_sources=()
  for f in "${sources[@]}"; do case "$f" in "$here"/test_*.cpp) [ "$f" = "$here/test_main.cpp" ] && low_sources+=("$f") ;; "$here"/converted/*) ;; *) low_sources+=("$f") ;; esac; done
  low_sources+=("${lowered[@]}")
  compile_all "$clangxx" "-clang" "${low_sources[@]}"
  low_objects=()
  for f in "${low_sources[@]}"; do low_objects+=("$(object_of "$f" "-clang")"); done
  "$clangxx" "${link[@]}" "${low_objects[@]}" "${objects[@]}" -lm -o "$out/spawncity_lowered"
  "$out/spawncity_lowered" "$@" || rc=1
fi
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
