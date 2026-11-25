#!/usr/bin/env bash
# Builds and runs SpawnCity's native unit tests (host compiler, no GL, sanitizers on).
#   kern/games/spawncity/tests/run_tests.sh [name-filter | --name <whole name>...]
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
# ... and its Blueprints as C++ classes, as the game links them: the tests run the rules as the game does
# (and call those of the folder "Program" as their classes, level_blueprints.h, as the game does). The tests
# call the Blueprints of the program's files as their classes too, wherever they stand: what they prove binds
# the Blueprint (until the program runs from the level, these still stand in the folder "Source").
called_by_tests=(
  terrain_cpp
  tiles_cpp
  wanted_cpp
  combat_rules_cpp
  equipment_cpp
  gps_live_cpp
  nostr_cpp
  settings_cpp
  perf_cpp
  look_cpp
  road_graph_cpp
  people_cpp
  world_traffic_cpp
  controls_cpp
  touch_play_cpp
)
"$kern/../tools/blueprint_cpp.sh" "$game/level/spawncity.klevel" "$game/src/level_blueprints.cpp" "$game/src/level_blueprints.h" Program "${called_by_tests[@]}"
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
  for f in "$dir"/*.cpp; do case "$(basename "$f")" in main.cpp|level_data.cpp) continue ;; esac; grep -q '#include "game.h"' "$f" || base_sources+=("$f"); done
  # (the level's text as it stands: what the old program reads of the level is the level the tests run)
  cp -p "$game/src/level_data.cpp" "$dir/level_data.cpp" && base_sources+=("$dir/level_data.cpp")
  # (a file of the old program the engine as it stands does not compile is not in it: a test that needs it does not link)
  printf '%s\n' "${base_sources[@]}" | CXX_="$cxx" PRELUDE="$out/base/prelude.h" xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c '
    src="$1"; ns="$2"; shift 2
    o="${src%.cpp}.o"
    { [ -f "$o" ] && [ ! "$src" -nt "$o" ]; } || [ -f "$o.failed" ] || "$CXX_" "$@" -D"sc=$ns" -include "$PRELUDE" -w -c "$src" -o "$o" 2> "$o.log" || touch "$o.failed"' _ {} "$ns" "${flags[@]}"
  built=()
  for f in "${base_sources[@]}"; do [ -f "${f%.cpp}.o" ] && built+=("${f%.cpp}.o"); done
  lib="$out/base/$ns.a"
  rm -f "$lib" && ar rcs "$lib" "${built[@]}"
  base_libs+=("$lib")
done
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
"$cxx" "${link[@]}" "${cpp_objects[@]}" "${objects[@]}" "${base_libs[@]}" -lm -o "$out/spawncity_tests"
# SC_TEST_BUILD_ONLY=1 (the CI: one job builds, the jobs of the tests take its objects - SC_TEST_OUT - and only link): stop here.
[ "${SC_TEST_BUILD_ONLY:-}" = 1 ] && { echo "built $out/spawncity_tests"; exit 0; }
# SC_TEST_SHARD=k/n (the CI: n jobs): this job runs the k-th of every n tests, in both passes; the steps after the
# tests - the game's own sources, the rules' Blueprint as C++ - the first job alone.
whole=1
case "${SC_TEST_SHARD:-}" in "" | 1/*) ;; *) whole=0 ;; esac
# SC_TEST_PASS=1 or 2 (the CI: a job for each): only the first pass (against the engine's stand-in) or only the second
# (against the engine itself); the steps after the tests go with the first. Unset: both, one after the other.
pass="${SC_TEST_PASS:-}"
case "$pass" in "" | 1 | 2) ;; *) echo "FAIL SC_TEST_PASS=$pass (1 or 2)"; exit 1 ;; esac
[ "$pass" = 2 ] && whole=0
# (every test runs, whatever an earlier one gave - see test_main.cpp; the run's result is that of all of it)
# (up to KERN_JOBS tests at once, else as many as there are cores - test_main.cpp; each in its own process, its lines as one piece in the order of the tests)
rc=0
[ "$pass" = 2 ] || "$out/spawncity_tests" "$@" || rc=1
# The converted functions once more against the engine itself: the source and its Blueprint (interpreter,
# class) call the real functions of the engine's nodes, each side with objects of its own, set up alike
# for every call - the description of every node they call has to fit the real function (rule_blueprint.h).
if [ $# -eq 0 ] && [ "$pass" != 1 ]; then SC_ENGINE=real "$out/spawncity_tests" converted_ || rc=1; fi
# The differential tests in the form "two builds" (tools/source_diff.py; tests/lowered/builds/<file>/): each side is the
# whole program in its state - the old tree (the game's sources as they stood at the test's base: these sources with
# old/ put over them and the files named in gone taken away) and the sources as they stand -, the engine and the level
# as they stand. The same test is compiled into each; the old program writes what each compared function gave, the
# new compares. Run with the tests (no filter, or one its name - lowered_<file>_cpp - holds), in the first pass.
builds_rc=0
for drv in "$here"/lowered/builds/*/builds_*.cpp; do
  [ -f "$drv" ] || continue
  bdir="$(dirname "$drv")" stem="$(basename "$bdir")" name="lowered_$(basename "$bdir" | sed 's/[^A-Za-z0-9_]/_/g')_cpp"
  want=0; [ $# -eq 0 ] && want=1
  for a in "$@"; do case "$name" in *"$a"*) want=1 ;; esac; done
  [ "$want" = 1 ] && [ "$pass" != 2 ] && [ "$whole" = 1 ] || continue
  bout="$out/builds/$stem"; tree="$bout/tree/src"
  rm -rf "$bout/tree" && mkdir -p "$bout/tree"
  # (beside it what stands beside src; the tests as folders of links - a header of the tests that names ../src/ is then
  # the old tree's: the test of the old program is compiled from there)
  for e in "$game"/*; do case "$(basename "$e")" in src) ;; tests) cp -rs "$e" "$bout/tree/tests" ;; *) ln -s "$e" "$bout/tree/$(basename "$e")" ;; esac; done
  cp -r "$game/src" "$tree"
  (cd "$bdir/old" && find . -type f) | while IFS= read -r f; do mkdir -p "$(dirname "$tree/$f")"; cp "$bdir/old/$f" "$tree/$f"; done
  while IFS= read -r f; do [ -n "$f" ] && rm -f "$tree/$f"; done < "$bdir/gone"
  # (the objects: the engine's as the tests have them; the game's - every source but main.cpp, game.h's too - of each tree.
  # An old source that is the new one, with no header of the trees other, is the new one's object)
  same_headers=1
  if [ -n "$(cd "$bdir/old" && find . -name '*.h' -o -name '*.hpp')" ] || [ -s "$bdir/gone" ]; then same_headers=0; fi
  okey="$({ printf '%s ' "$key"; find "$tree" -name '*.h' -o -name '*.hpp' | sort | xargs cat; } | sha256sum | cut -c1-12)"
  side_objects() {  # tree flag... -> the object of every source of the tree (compiled where it is not there)
    local t="$1"; shift
    for f in "$t"/*.cpp; do
      [ "$(basename "$f")" = main.cpp ] && continue
      if [ "$t" = "$game/src" ] || { [ "$same_headers" = 1 ] && cmp -s "$f" "$game/src/$(basename "$f")"; }; then
        object_of "$game/src/$(basename "$f")"
      else
        echo "$out/obj/builds-$stem-$(basename "$f")-$okey-$(sha256sum "$f" | cut -c1-12).o"
      fi
    done
  }
  oflags=(); for f in "${flags[@]}"; do [ "$f" = "-I$game/src" ] && oflags+=("-I$tree") || oflags+=("$f"); done
  engine=(); for f in "${cpp_objects[@]}"; do case "$f" in *_kern_src_*|*_kern_tests_gl_stub*|*_tests_test_main.cpp-*) engine+=("$f") ;; esac; done
  for side in new old; do
    if [ "$side" = new ]; then t="$game/src" d="$drv" sf=("${flags[@]}"); else t="$tree" d="$bout/tree/tests/${drv#"$here"/}" sf=("${oflags[@]}"); fi
    mapfile -t objs < <(side_objects "$t")
    k=0; jobs_src=()
    for f in "$t"/*.cpp; do [ "$(basename "$f")" = main.cpp ] && continue; [ -f "${objs[$k]}" ] || jobs_src+=("$f|${objs[$k]}"); k=$((k + 1)); done
    if [ "${#jobs_src[@]}" -gt 0 ]; then
      printf '%s\n' "${jobs_src[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c 'src="${1%%|*}" o="${1#*|}"; shift; "$cxx" "$@" -c "$src" -o "$o"' _ {} "${sf[@]}"
    fi
    dobj="$bout/driver-$side.o"
    "$cxx" "${sf[@]}" -c "$d" -o "$dobj"
    "$cxx" "${link[@]}" "${engine[@]}" "${objs[@]}" "$dobj" "${objects[@]}" -lm -o "$bout/$side"
  done
  rm -f "$bout/record"
  echo "two builds: $name (base $(sed -n 's#^// base: ##p' "$drv"))"
  SC_LOWERED_BUILDS="write:$bout/record" "$bout/old" --name "$name" || builds_rc=1
  SC_LOWERED_BUILDS="check:$bout/record" "$bout/new" --name "$name" || builds_rc=1
done
[ "$builds_rc" = 0 ] || rc=1
# The game's own sources - what includes game.h and is not linked into the tests - compile: a
# rule's header that changed is caught here, not first by the game's build.
if [ $# -eq 0 ] && [ "$whole" = 1 ]; then
  rest=()
  for f in "$game"/src/*.cpp; do if [ "$(basename "$f")" = main.cpp ] || grep -q '#include "game.h"' "$f"; then rest+=("$f"); fi; done
  printf '%s\n' "${rest[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} "$cxx" -std=c++20 -fsyntax-only -fno-exceptions -fno-rtti -Wall -Wextra -Wno-unused-parameter \
    -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src" {}
  echo "ok   the game's own sources compile (${#rest[@]} files)"
fi
# The rules' Blueprint as its C++ text, compiled as it is by the host compiler (nothing looked up
# at run time), gives what the interpreter gives: every call, value for value.
if [ $# -eq 0 ] && [ "$whole" = 1 ]; then
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
