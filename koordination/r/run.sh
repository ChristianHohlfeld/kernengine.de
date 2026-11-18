#!/bin/bash
# run.sh <out-dir> units...   (in kern/games/spawncity)
S=/home/claude/kernengine.de/koordination
out=$1; shift
for u in "$@"; do
  TMPDIR=$S/shared-tmp nice python3 ../../../tools/source_diff.py --base origin/main --test $out -I../../include -I../../third_party/box3d/include -Isrc -- src/$u > $S/r/log/${u}.log 2>&1
  head -1 $S/r/log/${u}.log | cut -c1-400
done
