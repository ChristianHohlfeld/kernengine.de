#!/usr/bin/env bash
# Builds a native program of a game (kern/docs/app.md 7.): the engine - without the editor and the Lehrer, with the
# headless graphics kern/tests/gl_stub.cpp - plus the game's sources, linked into one program. A game's test is such
# a program; the game lists only its own sources and its project.
#
#   kern/tools/build_native.sh [--project <game.kproject>] [--classes] [--sanitize] -o <program> [flags / sources...]
#
#   --project   embeds the project and its levels (kern/game/embedded.h: kern::game::embedded_project())
#   --classes   with --project: the Blueprints of the project's levels compiled in as C++ classes
#   --sanitize  ASan and UBSan, as the engine's tests (kern/tests/run.sh)
# Arguments ending in .cpp / .cc are sources, the others compiler flags (-I, -D ...). The engine's objects are kept in
# $KERN_NATIVE_CACHE (default $TMPDIR/kern-native) under a hash of the source, the flags and all headers of the engine:
# a second game, or the same game again, only compiles what changed. CXX / CC: the compilers (default c++ / cc);
# KERN_JOBS: how many at once (default: all cores).
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kern="$(dirname "$here")"
usage="usage: kern/tools/build_native.sh [--project <game.kproject>] [--classes] [--sanitize] -o <program> [flags / sources...]"
project="" classes="" sanitize=0 out=""
while [ $# -gt 0 ]; do
  case "$1" in
    --project) project="${2:?$usage}"; shift ;;
    --classes) classes=--classes ;;
    --sanitize) sanitize=1 ;;
    -o) out="${2:?$usage}"; shift ;;
    *) break ;;
  esac
  shift
done
[ -n "$out" ] || { echo "$usage" >&2; exit 1; }
[ -z "$classes" ] || [ -n "$project" ] || { echo "build_native.sh: --classes compiles the Blueprints of a project's levels - it needs --project" >&2; exit 1; }
cxx="${CXX:-c++}" cc="${CC:-cc}" jobs="${KERN_JOBS:-$(nproc)}"
cache="${KERN_NATIVE_CACHE:-${TMPDIR:-/tmp}/kern-native}"
san=()
[ "$sanitize" = 1 ] && san=(-fsanitize=address,undefined -fno-sanitize-recover=undefined)
. "$kern/third_party/musl_math/flags.sh"   # (kern_math_flags: the engine's mathematics, never computed by the compiler)
read -r -a mathflags <<< "$(kern_math_flags "$cxx")"
flags=(-std=c++20 -O1 -ffp-contract=off "${mathflags[@]}" -fno-exceptions -fno-rtti -w "${san[@]}" -I"$kern/include" -I"$kern/third_party/box3d/include")
key="$( { echo "${flags[*]} $cxx $cc"; "$cxx" --version 2>/dev/null | head -1; } | sha256sum | cut -c1-12)"
mkdir -p "$cache/box3d-$key" "$cache/musl-$key" "$cache/obj-$key" "$cache/gen"
work=()
third=()
for c in "$kern"/third_party/box3d/src/*.c; do
  o="$cache/box3d-$key/$(basename "$c" .c).o"
  third+=("$o")
  [ -f "$o" ] || work+=("$cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w ${san[*]} -I$kern/third_party/box3d/include -I$kern/third_party/box3d/src -c $c -o $o")
done
for c in "$kern"/third_party/musl_math/src/*.c; do
  o="$cache/musl-$key/$(basename "$c" .c).o"
  third+=("$o")
  [ -f "$o" ] || work+=("$cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w ${san[*]} -c $c -o $o")
done
# (an object per source of the engine, under the hash of the source and all of the engine's headers: a changed
# header compiles everything again)
headers="$(find "$kern/include" "$kern/src" "$kern/third_party/microduck" -name '*.h' -o -name '*.inc' | sort | xargs cat | sha256sum | cut -c1-12)"
engine=()
while IFS= read -r f; do
  o="$cache/obj-$key/$(echo "$f" | sed "s|$kern/||; s|/|_|g")-$( { echo "$headers"; cat "$f"; } | sha256sum | cut -c1-12).o"
  engine+=("$o")
  [ -f "$o" ] || work+=("$cxx ${flags[*]} -c $f -o $o.tmp && mv $o.tmp $o")
done < <(find "$kern/src" -name '*.cpp' -not -path '*/studio/*' -not -path '*/lehrer/*' | sort; echo "$kern/tests/gl_stub.cpp")
if [ ${#work[@]} -gt 0 ]; then
  echo "build_native.sh: ${#work[@]} objects of the engine to compile ($jobs at once)"
  printf '%s\n' "${work[@]}" | xargs -P "$jobs" -I{} sh -c '{}'
fi
# the game: its sources and flags, and what its project brings
sources=()
extra=()
for a in "$@"; do
  case "$a" in
    *.cpp|*.cc) [ -f "$a" ] || { echo "build_native.sh: $a: no such source" >&2; exit 1; }; sources+=("$a") ;;
    *) extra+=("$a") ;;
  esac
done
if [ -n "$project" ]; then
  gen="$cache/gen/$(echo "$(cd "$(dirname "$project")" && pwd)/$(basename "$project") $classes" | sha256sum | cut -c1-12)"
  while IFS= read -r f; do sources+=("$f"); done < <("$here/project_sources.sh" "$project" "$gen" $classes)
fi
mkdir -p "$(dirname "$out")"
"$cxx" "${flags[@]}" -I"$kern/tests" "${extra[@]}" "${sources[@]}" "${engine[@]}" "${third[@]}" -lm -o "$out"
echo "build_native.sh: built $out"
