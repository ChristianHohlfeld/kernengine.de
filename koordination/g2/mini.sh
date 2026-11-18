#!/usr/bin/env bash
# mini.sh: the converter on the small program in g2/mini (fast check of a general rule)
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
G=/home/claude/w-g2/kern/games/spawncity
D=$S/g2/mini; rm -rf $D/tests $D/texts; mkdir -p $D/tests/converted $S/shared-tmp
cd $D
git -C /home/claude/w-g2 show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp $G/tests/*.h $D/tests/
cp $G/src/*.h src/
K=/home/claude/w-g2/kern
FUNCTIONS=1 KERN_JOBS=1 TEST_ERRORS=1 TEST_KEEP=$D/keep TMPDIR=$S/shared-tmp nice python3 ${TRACE:-/home/claude/w-g2/tools/cpp_to_blueprint.py} ${TRACE:+"$RX"} --convert sc --level $D/l.klevel --test $D/tests/converted --texts $D/texts -I$K/include -I$K/third_party/box3d/include -Isrc -- src/${MINI:-a}.cpp > $D/out.log 2>&1
echo "exit $?"; cat $D/out.log | cut -c1-900
