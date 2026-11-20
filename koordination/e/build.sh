#!/bin/bash
# build.sh <kern-tree> <out>   (interpreter only, -O2, against the tree's logic sources)
K=$1/kern; O=$2; mkdir -p $O; S=/home/claude/kernengine.de/koordination
flock $S/heavy3.lock env TMPDIR=$S/shared-tmp bash -c "
for f in logic core_nodes formula codegen notation; do clang++-18 -std=c++20 -O2 -fno-exceptions -fno-rtti -ffp-contract=off -I$K/include -c $K/src/logic/\$f.cpp -o $O/\$f.o || exit 1; done
for f in base json; do clang++-18 -std=c++20 -O2 -fno-exceptions -fno-rtti -I$K/include -c $K/src/core/\$f.cpp -o $O/\$f.o || exit 1; done
clang++-18 -std=c++20 -O2 -fno-exceptions -fno-rtti -I$K/include -c /tmp/e/m/stubs.cpp -o $O/stubs.o &&
clang++-18 -std=c++20 -O2 -fno-exceptions -fno-rtti -I$K/include -I$K/tests -c /tmp/e/b/bcall.cpp -o $O/bcall.o && clang++-18 -std=c++20 -O1 -fno-exceptions -fno-rtti -I$K/include -I$K/tests -c /tmp/e/b/cases.cpp -o $O/cases.oo &&
clang++-18 $O/*.o /tmp/e/m/mm/*.o -o $O/bcall"
