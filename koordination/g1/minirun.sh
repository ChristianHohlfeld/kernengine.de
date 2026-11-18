#!/usr/bin/env bash
# minirun.sh: the converted tests of $S/g1/mini (after miniconv.sh), built without sanitizers and run - the engine's
# sources they need compiled here (objects kept in mini/eobj), everything under the lock once.
set -u
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
W=/home/claude/w-g1; kern=$W/kern; game=$kern/games/spawncity; M=$S/g1/mini; B=$M/build; E=$M/eobj
mkdir -p $B $E
python3 $game/tools/embed_level.py $M/l.klevel $B/level_data.cpp
$W/tools/blueprint_cpp.sh $M/l.klevel $B/level_blueprints.cpp
flags=(-std=c++20 -O1 -g0 -ffp-contract=off -fno-exceptions -fno-rtti -w -I$kern/include -I$kern/third_party/box3d/include -I$game/src -I$kern/tests -I$game/tests)
objs=()
cc1() { local f=$1 o=$2; shift 2; if [ ! -f $o ] || [ $f -nt $o ]; then echo "cc $f"; nice c++ "${flags[@]}" "$@" -c $f -o $o.tmp && mv $o.tmp $o; fi; objs+=($o); }
for f in ${ENGINE_SRCS:-}; do cc1 $f $E/$(echo ${f#$kern/} | sed 's|/|_|g').o; done
cc1 $game/src/rules.cpp $E/g_rules.o
( cd $game/src && true )
for f in $B/level_data.cpp $B/level_blueprints.cpp $M/main.cpp; do o=$B/$(basename $f .cpp).o; rm -f $o; cc1 $f $o -I$M/src; done
for t in $M/tests/converted/test_*.cpp; do o=$B/$(basename $t .cpp).o; rm -f $o; cc1 $t $o -I$M/src; done
for c in $kern/third_party/musl_math/src/*.c; do o=$E/m_$(basename $c .c).o; [ -f $o ] || nice cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $o; objs+=($o); done
C=/tmp/kern-blueprint-cpp/24b8fb023e5ae18f/obj
for o in $C/*.o; do case $(basename $o) in blueprint_cpp.cpp.o|*.c.o) ;; *) objs+=($o) ;; esac; done
nice c++ "${objs[@]}" $S/g1/obj/b_*.o -lm -o $B/mini_tests 2> $B/link.log || { grep -o "undefined reference to \`[^']*'" $B/link.log | sed 's/(.*//' | sort | uniq -c | sort -rn | head -40; exit 1; }
$B/mini_tests "${1:-}"
