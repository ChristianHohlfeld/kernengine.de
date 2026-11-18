#!/usr/bin/env bash
# seeprobe.sh <tag> <src files...>: the diagnosis (see.py) on these sources, under the lock
S=/home/claude/kernengine.de/koordination
tag=$1; shift
D=$S/g2/see-$tag; rm -rf $D; mkdir -p $D/t/converted $S/shared-tmp
cd /home/claude/w-g2/kern/games/spawncity
git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp tests/*.h $D/t/
FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy2.lock nice python3 $S/g2/see.py "Game::|make_sounds" --convert sc --level $D/l.klevel --test $D/t/converted --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $D/out.log 2> $D/see.log
echo "$tag exit $? $(date +%H:%M)"
