#!/usr/bin/env bash
# Paket p: Werkzeug blueprint_cpp (mit Engine-Objekten) für den Worktree bauen und chk dagegen linken.
set -euo pipefail
S=/home/claude/kernengine.de/koordination; W=${W:-/home/claude/w-p}
export TMPDIR=$S/shared-tmp KERN_JOBS=2 HOST_CXX=clang++-18 HOST_CC=clang-18
$W/tools/blueprint_cpp.sh >/dev/null 2>&1 || true
d=$(ls -td $TMPDIR/kern-blueprint-cpp/*/ | head -1)
objs=$(ls $d/obj/*.o | grep -v "/blueprint_cpp.cpp.o")
clang++-18 -std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I$W/kern/include -I$W/kern/tests -I$W/kern/third_party/box3d/include $S/p/chk.cpp $objs -lm -o $S/p/chk
echo "gebaut: $d"
