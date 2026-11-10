#!/usr/bin/env bash
# The Blueprints of a Kern level as C++ classes a game links (tools/blueprint_cpp.cpp).
#
#   tools/blueprint_cpp.sh <level.klevel> <out.cpp>
#
# Builds the tool with the host compiler - once per state of its sources (Kern Logic, the node
# library and the tool itself; cached in $TMPDIR/kern-blueprint-cpp) - and runs it. HOST_CXX / HOST_CC override
# the compilers (default c++ / cc): they build a program for this machine, whatever the game is
# built for.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cxx="${HOST_CXX:-c++}" cc="${HOST_CC:-cc}"
# (the gameplay nodes as the library knows them - names, pins, settings - need nothing of the game)
sources=("$root/tools/blueprint_cpp.cpp" "$root/kern/src/game/logic_node_shapes.cpp")
while IFS= read -r f; do sources+=("$f"); done < <(find "$root/kern/src/logic" "$root/kern/src/core" -name '*.cpp' | sort)
key="$({ cat "${sources[@]}"; find "$root/kern/include" -name '*.h' | sort | xargs cat; cat "$root"/kern/third_party/musl_math/src/*.c; echo "$cxx $cc"; } | sha256sum | cut -c1-16)"
dir="${TMPDIR:-/tmp}/kern-blueprint-cpp/$key"
bin="$dir/blueprint_cpp"
if [ ! -x "$bin" ]; then
  mkdir -p "$dir/obj"
  export cxx cc dir root
  printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c \
    '"$cxx" -std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -c "$1" -o "$dir/obj/$(basename "$1").o"' _ {}
  printf '%s\n' "$root"/kern/third_party/musl_math/src/*.c | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c \
    '"$cc" -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$1" -o "$dir/obj/$(basename "$1").o"' _ {}
  "$cxx" "$dir"/obj/*.o -lm -o "$bin.tmp.$$"
  mv "$bin.tmp.$$" "$bin"
fi
"$bin" "$@"
