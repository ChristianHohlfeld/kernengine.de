#!/usr/bin/env bash
# miniconv.sh <name...>: --convert with tests of src/<name>.cpp in $S/g1/mini; texts in mini/texts, tests in mini/tests/converted
S=/home/claude/kernengine.de/koordination
G=/home/claude/w-g1/kern/games/spawncity
cd $S/g1/mini || exit 1
rm -rf tests texts; mkdir -p tests/converted
cp base.klevel l.klevel; cp $G/tests/*.h tests/
files=(); for n in "$@"; do files+=(src/$n.cpp); done
FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp python3 $S/g1/trace.py "${TRACE:-zzz}" --convert sc --level l.klevel --test tests/converted --texts texts -I/home/claude/w-g1/kern/include -I/home/claude/w-g1/kern/third_party/box3d/include -Isrc -- "${files[@]}" 2>&1
