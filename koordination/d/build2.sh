#!/usr/bin/env bash
set -euo pipefail
root=/home/claude/w-d; d=/home/claude/kernengine.de/koordination/d; obj=$d/obj; mkdir -p $d/obj2
for f in $root/kern/src/geo/*.cpp; do o=$d/obj2/$(basename $f).o
  if [ ! -f $o ] || [ $f -nt $o ] || [ -n "$(find $root/kern/include -name '*.h' -newer $o | head -1)" ]; then echo "cc $(basename $f)"; g++ -std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I$root/kern/include -I$root/kern/third_party/box3d/include -c $f -o $o; fi
done
g++ -std=c++20 -O1 -g -ffp-contract=off -fno-exceptions -fno-rtti -w -I$root/kern/include -I$root/kern/tests -I$root/kern/third_party/box3d/include $d/harness2.cpp $d/stub_renderer.cpp -x c++ $1 -x none $obj/*.o $d/obj2/*.o -lm -o $d/harness2
