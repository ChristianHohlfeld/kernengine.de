#!/usr/bin/env bash
# probe.sh <tag> <regex> <src files...>   (the converter on these sources, tracing Waits matching regex)
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
tag=$1; rx=$2; shift; shift
D=$S/g2/dbg-$tag; rm -rf $D; mkdir -p $D/t/converted $S/shared-tmp
cd /home/claude/w-g2/kern/games/spawncity
git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp tests/*.h $D/t/
FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy2.lock nice python3 $S/g2/trace.py "$rx" --convert sc --level $D/l.klevel --test $D/t/converted --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $D/out.log 2> $D/trace.log
echo "exit $?"
