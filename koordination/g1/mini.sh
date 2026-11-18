#!/usr/bin/env bash
# mini.sh <substring> <name...> : --check of small files in $S/g1/mini/src
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
sub=$1; shift
files=(); for n in "$@"; do files+=(src/$n.cpp); done
cd $S/g1/mini && FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp python3 $S/g1/trace.py "$sub" --check sc -I/home/claude/w-g1/kern/include -I/home/claude/w-g1/kern/third_party/box3d/include -Isrc -- "${files[@]}" 2>&1
