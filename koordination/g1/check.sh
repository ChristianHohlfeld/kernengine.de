#!/usr/bin/env bash
# check.sh <tag> <substring> <src...>: --check with a trace of the Wait named
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
tag=$1; sub=$2; shift 2
cd /home/claude/w-g1/kern/games/spawncity || exit 1
FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy3.lock nice python3 $S/g1/trace.py "$sub" --check sc -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $S/g1/$tag.log 2>&1
echo "exit $?" >> $S/g1/$tag.log
