#!/usr/bin/env bash
# The time a Blueprint's class takes against the C++ it was converted from (BLUEPRINTS.md, "Der Weg ab
# hier": every building block costs in the class what the C++ it stands for costs).
#
#   tools/blueprint_bench.sh <level.klevel> [--tests <dir>] [--check <limit>] [rounds] [case ...]
#   (--check 1.10: fails when a class takes more than 1.10 times its source in the numbers of the
#   language - bench.cpp; --tests: the tests/converted of the same conversion as the level, by
#   default the game's own)
#
# Writes the classes of the measured Blueprints (tools/blueprint_cpp.sh --classes), builds
# kern/games/spawncity/tests/bench/ in the game's release build (clang, -O3 -flto=thin -DNDEBUG
# -ffp-contract=off -fno-exceptions -fno-rtti, as kern/build.sh) and runs it: per case the time per
# call of the source and of the class and their ratio (bench.cpp). The source is measured twice: as
# the game builds it (src_*.cpp) and in the numbers of the language, as the test of tests/converted
# compiles it (src64_*.cpp: its test_<file>.cpp, f32 is double) - the class is held to the second.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
[ $# -ge 1 ] || { echo "usage: tools/blueprint_bench.sh <level.klevel> [rounds] [case ...]" >&2; exit 2; }
level="$1"; shift
tests="$root/kern/games/spawncity/tests/converted"
if [ "${1:-}" = "--tests" ]; then tests="$(cd "$2" && pwd)"; shift 2; fi
cxx="${BENCH_CXX:-clang++-18}"
if [ -n "${BENCH_DIR:-}" ]; then dir="$BENCH_DIR"; else  # (BENCH_DIR: keep the build there, to read it)
  dir="${TMPDIR:-/tmp}/kern-blueprint-bench/$$"
  trap 'rm -rf "$dir"' EXIT
fi
mkdir -p "$dir"
bench="$root/kern/games/spawncity/tests/bench"
# (the actors whose Blueprints are measured - the libraries they use come with them)
"$root/tools/blueprint_cpp.sh" --classes "$level" "$dir/classes.h" \
  qr_code_cpp nostr_cpp mvt_cpp lamps_cpp perf_cpp settings_cpp combat_audio_cpp jet_seats_cpp wire person_hit common tiles >&2
. "$root/kern/third_party/musl_math/flags.sh"   # (kern_math_flags: the engine's mathematics, never computed by the compiler)
read -r -a mathflags <<< "$(kern_math_flags "${BENCH_CXX:-clang++-18}")"
flags=(-std=c++20 -O3 -flto=thin -DNDEBUG -ffp-contract=off "${mathflags[@]}" -fno-exceptions -fno-rtti -w -I"$root/kern/include" -I"$dir"
  -I"$tests" -I"$root/kern/games/spawncity/tests/converted" -I"$root/kern/games/spawncity/tests" -I"$root/kern/third_party/box3d/include")
for f in "$bench"/*.cpp "$root/kern/src/logic/engine_host.cpp"; do  # (engine_host: what the tests' header holds of the engine's objects)
  "$cxx" "${flags[@]}" -c "$f" -o "$dir/$(basename "$f").o" &
  [ "$(jobs -r | wc -l)" -lt "${KERN_JOBS:-1}" ] || wait -n
done
for f in "$root"/kern/third_party/musl_math/src/*.c "$root"/kern/src/math/*.c; do  # (the language's own sin, cos, exp ..., as kern/build.sh)
  "${BENCH_CC:-clang-18}" -std=c17 -O3 -flto=thin -ffp-contract=off -fno-math-errno -w -c "$f" -o "$dir/musl_$(basename "$f").o" &
  [ "$(jobs -r | wc -l)" -lt "${KERN_JOBS:-1}" ] || wait -n
done
wait
"$cxx" "${flags[@]}" "$dir"/*.o -lm -o "$dir/bench"
# (the measurement takes the machine alone: run it under the locks of the other heavy work)
${BENCH_RUN:-} "$dir/bench" "$@"
