#!/usr/bin/env bash
# Builds and runs SpawnCity's native unit tests (host compiler, no GL, sanitizers on).
#   kern/games/spawncity/tests/run_tests.sh [name-filter]
# CXX / CC override the compilers (default c++ / cc). Objects are cached in
# $SC_TEST_OUT (default $TMPDIR/spawncity-tests); Kern itself is not modified.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
out="${SC_TEST_OUT:-${TMPDIR:-/tmp}/spawncity-tests}"
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
# Kern's engine-side pieces the models use (physics, geometry builders, the renderer headless on
# Kern's GL stub, Kern Logic for the rules that are Blueprints), then the game's simulation sources
# (no GameWorld) and the tests.
sources=()
for d in core physics geo gfx scene audio net logic; do while IFS= read -r f; do sources+=("$f"); done < <(find "$kern/src/$d" -name '*.cpp' | sort); done
sources+=("$kern/tests/gl_stub.cpp")
# Every game source that does not need GameWorld (not main.cpp, nothing that includes game.h).
for f in "$game"/src/*.cpp; do [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f" || sources+=("$f"); done
while IFS= read -r f; do sources+=("$f"); done < <(find "$here" -name 'test_*.cpp' | sort)
flags=(-std=c++20 -O1 -g -ffp-contract=off -fno-exceptions -fno-rtti -fsanitize=address,undefined -fno-sanitize-recover=undefined
       -Wall -Wextra -Wno-unused-parameter -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" -I"$kern/tests")
# Header changes rebuild everything (the cache key includes every header's content).
key="$({ printf '%s ' "${flags[@]}"; find "$kern/include" "$game" -name '*.h' -o -name '*.hpp' | sort | xargs cat; } | sha256sum | cut -c1-12)"
export cxx out key
printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
  src="$1"; shift
  o="$out/obj/$(echo "$src" | sed "s|/|_|g")-$key-$(sha256sum "$src" | cut -c1-12).o"
  [ -f "$o" ] || "$cxx" "$@" -c "$src" -o "$o"' _ {} "${flags[@]}"
cpp_objects=()
for f in "${sources[@]}"; do cpp_objects+=("$out/obj/$(echo "$f" | sed "s|/|_|g")-$key-$(sha256sum "$f" | cut -c1-12).o"); done
"$cxx" -fsanitize=address,undefined "${cpp_objects[@]}" "${objects[@]}" -lm -o "$out/spawncity_tests"
"$out/spawncity_tests" "$@"
# The rules' Blueprint as its C++ text, compiled as it is by the host compiler (nothing looked up
# at run time), gives what the interpreter gives: every call, value for value.
if [ $# -eq 0 ]; then
  nat="$out/native"
  rm -rf "$nat" && mkdir -p "$nat"
  "$out/spawncity_tests" --emit-native "$nat"
  logic_objects=()
  for f in "${cpp_objects[@]}"; do case "$f" in *_kern_src_logic_*|*_kern_src_core_*) logic_objects+=("$f") ;; esac; done
  "$cxx" "${flags[@]}" -Werror -I"$nat" "$nat/blueprint_native_main.cpp" "${logic_objects[@]}" "$out"/musl/*.o -lm -o "$out/wanted_native_test"
  "$out/wanted_native_test" "$nat/blueprint_native_expected.txt"
fi
