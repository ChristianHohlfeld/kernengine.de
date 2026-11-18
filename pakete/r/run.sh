#!/bin/bash
# run.sh <out-dir> units...   (in kern/games/spawncity)
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
out=$1; shift
for u in "$@"; do
  TMPDIR=$S/shared-tmp nice python3 ../../../tools/source_diff.py --base origin/main --test $out -I../../include -I../../third_party/box3d/include -Isrc -- src/$u > $S/r/log/${u}.log 2>&1
  head -1 $S/r/log/${u}.log | cut -c1-400
done
