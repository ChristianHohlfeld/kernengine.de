#!/usr/bin/env bash
# Benchmark qr_code: encode der v5-URL, das alte C++ (955503b, f32-Build wie das Spiel) gegen die Klasse des
# Blueprints qr_code_cpp (aus dem Level, tools/blueprint_cpp.sh ... level_blueprints.h Program), beide -O2 wie das Spiel.
#   run.sh [<worktree>]   (Standard /home/claude/w-s5; der Header wird aus dessen Level neu geschrieben)
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"; W="${1:-/home/claude/w-s5}"; K="$W/kern"
TMPDIR="${TMPDIR:-/tmp}" "$W/tools/blueprint_cpp.sh" "$K/games/spawncity/level/spawncity.klevel" "$here/lb.cpp" "$here/level_blueprints.h" Program >/dev/null
flags=(-std=c++20 -O2 -ffp-contract=off -fno-exceptions -fno-rtti -I"$K/include" -I"$K/third_party/box3d/include")
c++ "${flags[@]}" -I"$K/games/spawncity/src" -I"$here" "$here/bench_old.cpp" "$here/old_qr.cpp" -o "$here/bench_old"
c++ "${flags[@]}" -I"$here" "$here/bench_new.cpp" -o "$here/bench_new"
for i in 1 2 3; do "$here/bench_old"; "$here/bench_new"; done
