#!/usr/bin/env bash
# The Blueprints of a Kern level as C++ classes a game links (tools/blueprint_cpp.cpp).
#
#   tools/blueprint_cpp.sh [--project <name.kproject>] <level.klevel> <out.cpp> [<out.h> [<name> ...]]
#
# With <out.h>: the classes of the project's Blueprints - the program's, those the program's own C++ calls
# directly - and of the libraries they use go into <out.h> (see blueprint_cpp.cpp); the Blueprints named after
# it too, wherever they stand.
#
# Builds the tool with the host compiler - once per run (in the folder of this run, kern/tools/run_dir.sh: nothing is
# kept from one run to the next; a script that calls it often shares it through KERN_RUN_DIR) - and runs it. HOST_CXX / HOST_CC override
# the compilers (default c++ / cc): they build a program for this machine, whatever the game is
# built for.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cxx="${HOST_CXX:-c++}" cc="${HOST_CC:-cc}"
# (the gameplay nodes as the library knows them - names, pins, settings - need nothing of the game)
sources=("$root/tools/blueprint_cpp.cpp" "$root/kern/src/game/logic_node_shapes.cpp" "$root/kern/src/game/world_export.cpp")
while IFS= read -r f; do sources+=("$f"); done < <(find "$root/kern/src/logic" "$root/kern/src/core" -name '*.cpp' | sort)
key="$({ cat "${sources[@]}"; find "$root/kern/include" -name '*.h' | sort | xargs cat; cat "$root"/kern/third_party/musl_math/src/*.c "$root"/kern/src/math/*.c; echo "$cxx $cc"; } | sha256sum | cut -c1-16)"
. "$root/kern/tools/run_dir.sh"
dir="$KERN_RUN_DIR/kern-blueprint-cpp/$key"
bin="$dir/blueprint_cpp"
if [ ! -x "$bin" ]; then
  mkdir -p "$dir/obj"
  . "$root/kern/third_party/musl_math/flags.sh"; mathflags="$(kern_math_flags "$cxx")"   # (the engine's mathematics, never computed by the compiler)
  export cxx cc dir root mathflags
  printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c \
    '"$cxx" -std=c++20 -O1 -ffp-contract=off $mathflags -fno-exceptions -fno-rtti -w -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -c "$1" -o "$dir/obj/$(basename "$1").o"' _ {}
  printf '%s\n' "$root"/kern/third_party/musl_math/src/*.c "$root"/kern/src/math/*.c | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c \
    '"$cc" -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$1" -o "$dir/obj/$(basename "$1").o"' _ {}
  "$cxx" "$dir"/obj/*.o -lm -o "$bin.tmp.$$"
  mv "$bin.tmp.$$" "$bin"
fi
"$bin" "$@"
