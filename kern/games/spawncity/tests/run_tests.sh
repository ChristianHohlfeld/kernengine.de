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
# Kern's engine-side pieces the models use (physics, geometry builders, the renderer headless on
# Kern's GL stub), then the game's simulation sources (no GameWorld) and the tests.
sources=()
for d in core physics geo gfx scene; do while IFS= read -r f; do sources+=("$f"); done < <(find "$kern/src/$d" -name '*.cpp' | sort); done
sources+=("$kern/tests/gl_stub.cpp")
for f in drone car jet mvt tiles city_stream wanted; do [ -f "$game/src/$f.cpp" ] && sources+=("$game/src/$f.cpp"); done
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
