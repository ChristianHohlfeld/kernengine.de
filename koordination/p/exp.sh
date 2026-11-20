#!/usr/bin/env bash
# Paket p: Versuch – Header-Kopie mit einer Änderung ($1: python-Ersetzung), Messwerkzeug damit bauen und messen.
set -euo pipefail
S=/home/claude/kernengine.de/koordination; W=/home/claude/w-p
d=$S/p/exp; rm -rf $d; mkdir -p $d/o; cp -r $W/kern/include $d/inc
python3 -c "$1" $d/inc
B=$W/kern/games/spawncity/tests/bench
F="-std=c++20 -O3 -flto=thin -DNDEBUG -ffp-contract=off -fno-exceptions -fno-rtti -w -I$d/inc -I$S/p/lvl"
flock $S/heavy2.lock bash -c "for f in $B/*.cpp; do clang++-18 $F -c \$f -o $d/o/\$(basename \$f).o || exit 1; done; for f in $W/kern/third_party/musl_math/src/*.c; do clang-18 -std=c17 -O3 -flto=thin -ffp-contract=off -fno-math-errno -w -c \$f -o $d/o/m_\$(basename \$f).o; done; clang++-18 $F $d/o/*.o -lm -o $d/bench"
flock $S/heavy2.lock flock $S/heavy3.lock $d/bench 15 | awk '{printf "%-18s %s %s\n", $1, $(NF-1), $NF}'
