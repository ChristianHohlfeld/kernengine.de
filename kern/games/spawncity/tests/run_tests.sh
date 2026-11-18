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
while IFS= read -r f; do sources+=("$f"); done < <(find "$here" -name 'test_*.cpp' | sort)
flags=(-std=c++20 "${build[@]}" -ffp-contract=off -fno-exceptions -fno-rtti
       -Wall -Wextra -Wno-unused-parameter -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" -I"$kern/tests" -I"$out/base")
# The old programs the differential tests of tests/lowered compare with (tools/source_diff.py): for every commit a
# test names as its base (`// base: <commit>`), the game's sources as they stood there - built as the game's are,
# with the engine as it stands, in a namespace of their own (sc_base_<commit>), and kept in a library the tests
# take from what they use. The old program registers nothing at static initialization (the jobs are the
# program's that stands).
base_libs=()
for b in $(cat "$here"/lowered/test_*.cpp 2>/dev/null | sed -n 's|^// base: \([0-9a-f]\{40\}\)$|\1|p' | sort -u); do
  ns="sc_base_${b:0:12}" dir="$out/base/sc_base_${b:0:12}"
  if [ ! -d "$dir" ]; then
    rm -rf "$dir.part" && mkdir -p "$dir.part"
    git -C "$game" archive "$b" -- src | tar -x -C "$dir.part" --strip-components=1
    mv "$dir.part" "$dir"
  fi
  printf '#pragma once\n#include "kern/core/jobs.h"\n#undef KERN_JOB\n#define KERN_JOB(kind, fn) static_assert(true)\n' > "$out/base/prelude.h"
  base_sources=()
  for f in "$dir"/*.cpp; do [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f" || base_sources+=("$f"); done
  # (a file of the old program the engine as it stands does not compile is not in it: a test that needs it does not link)
  printf '%s\n' "${base_sources[@]}" | CXX_="$cxx" PRELUDE="$out/base/prelude.h" xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
    src="$1"; ns="$2"; shift 2
    o="${src%.cpp}.o"
    [ -f "$o" ] || [ -f "$o.failed" ] || "$CXX_" "$@" -D"sc=$ns" -include "$PRELUDE" -w -c "$src" -o "$o" 2> "$o.log" || touch "$o.failed"' _ {} "$ns" "${flags[@]}"
  built=()
  for f in "${base_sources[@]}"; do [ -f "${f%.cpp}.o" ] && built+=("${f%.cpp}.o"); done
  lib="$out/base/$ns.a"
  rm -f "$lib" && ar rcs "$lib" "${built[@]}"
  base_libs+=("$lib")
done
# Header changes rebuild what includes them (the cache key includes every header's content): a header
# of the engine or the game everything, a header of the tests only the tests.
key="$({ printf '%s ' "${flags[@]}"; find "$kern/include" "$game" -path "$here" -prune -o \( -name '*.h' -o -name '*.hpp' \) -print | sort | xargs cat; } | sha256sum | cut -c1-12)"
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
"$cxx" "${link[@]}" "${cpp_objects[@]}" "${objects[@]}" "${base_libs[@]}" -lm -o "$out/spawncity_tests"
"$out/spawncity_tests" "$@"
# The converted functions once more against the engine itself: the source and its Blueprint (interpreter,
# class) call the real functions of the engine's nodes, each side with objects of its own, set up alike
# for every call - the description of every node they call has to fit the real function (rule_blueprint.h).
if [ $# -eq 0 ]; then SC_ENGINE=real "$out/spawncity_tests" converted_; fi
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
