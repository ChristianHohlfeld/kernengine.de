#!/usr/bin/env bash
# dbg.sh <tag> <src...> : lokaler Konverter-Lauf mit Wait-Positionen (where.py), unter heavy2.lock
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
tag=$1; shift
D=$S/g2/dbg-$tag; rm -rf $D; mkdir -p $D $S/shared-tmp
cd /home/claude/w-g2/kern/games/spawncity || exit 1
git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel
WHERE_LOG=$D/where.log FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp flock $S/heavy2.lock nice python3 $S/g2/where.py ../../../tools/cpp_to_blueprint.py --convert sc --level $D/l.klevel --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $D/out.log 2>&1
echo "fertig rc=$? $D"
