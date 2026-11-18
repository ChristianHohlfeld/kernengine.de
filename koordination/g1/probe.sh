#!/usr/bin/env bash
# probe.sh <tag> <src...>   (run in kern/games/spawncity of the worktree)
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
tag=$1; shift
cd /home/claude/w-g1/kern/games/spawncity || exit 1
D=$S/g1/$tag; rm -rf $D; mkdir -p $D/t/converted $S/shared-tmp
git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp tests/*.h $D/t/
FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy3.lock nice python3 ../../../tools/cpp_to_blueprint.py --convert sc --level $D/l.klevel --test $D/t/converted --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $D/out.log 2>&1
echo "exit $?" >> $D/out.log
