#!/usr/bin/env bash
# Kleinst-Harness für Paket d: Logik + Kern als Objekte (je Quelle neu, wenn sie oder ein Header neuer ist), dazu harness.cpp.
set -euo pipefail
root=/home/claude/w-d; d=/home/claude/kernengine.de/koordination/d; obj=$d/obj; mkdir -p $obj
newest_h=$(find $root/kern/include -name '*.h' -newer $obj/.stamp 2>/dev/null | head -1 || true)
srcs=("$root/kern/src/game/logic_node_shapes.cpp"); while IFS= read -r f; do srcs+=("$f"); done < <(find $root/kern/src/logic $root/kern/src/core -name '*.cpp' | sort)
for f in "${srcs[@]}"; do o=$obj/$(basename $f).o
  if [ ! -f $o ] || [ $f -nt $o ] || [ -n "$newest_h" ]; then echo "cc $(basename $f)"; g++ -std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I$root/kern/include -I$root/kern/third_party/box3d/include -c $f -o $o; fi
done
for f in $root/kern/third_party/musl_math/src/*.c; do o=$obj/$(basename $f).o; [ -f $o ] || gcc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $f -o $o; done
touch $obj/.stamp
g++ -std=c++20 -O1 -g -ffp-contract=off -fno-exceptions -fno-rtti -w -I$root/kern/include -I$root/kern/tests -I$d -I$root/kern/third_party/box3d/include $d/${H:-harness}.cpp $obj/*.o -lm -o $d/${H:-harness}
