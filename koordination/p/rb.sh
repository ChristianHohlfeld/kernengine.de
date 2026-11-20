#!/usr/bin/env bash
# Paket p (Nachfolger 2): Messwerkzeug mit den Headern des Worktrees neu bauen (classes.h aus $CL, Vorgabe b4), Fälle messen.
#   rb.sh <runden> [fall…]   (BENCH_ONLY=1: nur bauen)
set -euo pipefail
S=/home/claude/kernengine.de/koordination; W=${W:-/home/claude/w-p}; CL=${CL:-$S/p/b4}; d=${RD:-$S/p/r}
mkdir -p $d/o; B=$W/kern/games/spawncity/tests/bench
export TMPDIR=$S/shared-tmp W CL d
build() {
  F="${XF:-} -std=c++20 -O3 -flto=thin -DNDEBUG -ffp-contract=off -fno-exceptions -fno-rtti -w -I$W/kern/include -I$CL -I${TESTS:-$S/p/conv} -I$W/kern/games/spawncity/tests/converted -I$W/kern/games/spawncity/tests -I$W/kern/third_party/box3d/include"
  for f in $B/*.cpp $W/kern/src/logic/engine_host.cpp; do
    clang++-18 $F -c $f -o $d/o/$(basename $f).o &
    [ "$(jobs -r | wc -l)" -lt 2 ] || wait -n
  done
  wait
  clang++-18 $F $d/o/*.o $CL/musl_*.o -lm -o $d/bench
}
export -f build
flock $S/heavy2.lock bash -c 'set -e; S=/home/claude/kernengine.de/koordination; B=$W/kern/games/spawncity/tests/bench; build'
[ -n "${BENCH_ONLY:-}" ] && exit 0
flock $S/heavy2.lock flock $S/heavy3.lock $d/bench "$@"
