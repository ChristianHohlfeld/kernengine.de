#!/usr/bin/env bash
# tprobe.sh <tag> <trace-substrings> <src...>: converter probe (<=3 files) with trace of the named Waits, run in kern/games/spawncity of w-g1
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
tag=$1; want=$2; shift 2
cd /home/claude/w-g1/kern/games/spawncity || exit 1
D=$S/g1/$tag; rm -rf $D; mkdir -p $D/t/converted $S/shared-tmp
git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp tests/*.h $D/t/
TRACE_DEPTH=${TRACE_DEPTH:-8} FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy3.lock nice python3 $S/g1/trace.py "$want" --convert sc --level $D/l.klevel --test $D/t/converted --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $D/out.log 2>&1
echo "exit $?" >> $D/out.log
