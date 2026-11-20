#!/bin/bash
S=/home/claude/kernengine.de/koordination
for side in base:/home/claude/w-e-base neu:/home/claude/w-e; do
  name=${side%%:*}; tree=${side#*:}
  cd $tree && TMPDIR=$S/shared-tmp KERN_JOBS=2 BENCH_DIR=$S/e/bench/$name flock $S/heavy2.lock flock $S/heavy3.lock tools/blueprint_bench.sh $S/p/lvl/spawncity.klevel 9 > $S/e/bench/$name.txt 2>&1
  echo "$name rc=$?" >> $S/e/bench/done.txt
done
