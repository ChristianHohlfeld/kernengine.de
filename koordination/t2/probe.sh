#!/usr/bin/env bash
# probe.sh <tag> <src...>: Konverter lokal über diese Quelltexte (w-t2), mit Kopie des Levels, TEST_ERRORS, TEST_KEEP
S=/home/claude/kernengine.de/koordination
tag=$1; shift
out=$S/t2/p-$tag; rm -rf $out; mkdir -p $out/tests $out/texts
cd /home/claude/w-t2/kern/games/spawncity || exit 1
cp level/spawncity.klevel $out/level.klevel

TEST_ERRORS=1 TEST_KEEP=$out/keep FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp python3 ../../../tools/cpp_to_blueprint.py --convert sc --program firmware=fc,hwcontract --level $out/level.klevel --test tests/converted --texts $out/texts \
  -I../../include -I../../third_party/box3d/include -Isrc -- "$@" > $out/out.log 2>&1
echo "rc=$?" >> $out/out.log
