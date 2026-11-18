#!/usr/bin/env bash
# The engine's objects for the tests of this package (as run_tests.sh builds them: sanitizers on); the lock is taken per batch of files.
set -u
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
K=/home/claude/w-g1/kern; G=$K/games/spawncity; O=$S/g1/obj
mkdir -p $O
batch=$O/batch.sh; n=0
flush() { [ -s $batch ] && flock $S/heavy3.lock bash $batch; : > $batch; n=0; }
: > $batch
add() { echo "$1" >> $batch; n=$((n+1)); [ $n -ge ${2:-10} ] && flush; }
for c in $K/third_party/box3d/src/*.c; do o=$O/b_$(basename $c .c).o; [ -f $o ] || add "nice cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I$K/third_party/box3d/include -I$K/third_party/box3d/src -c $c -o $o" 60; done
for c in $K/third_party/musl_math/src/*.c; do o=$O/m_$(basename $c .c).o; [ -f $o ] || add "nice cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $o" 400; done
flush
flags="-std=c++20 -O1 -g0 -fsanitize=address,undefined -fno-sanitize-recover=undefined -ffp-contract=off -fno-exceptions -fno-rtti -w -I$K/include -I$K/third_party/box3d/include -I$G/src -I$K/tests -I$G/tests"
srcs=()
for d in core logic game geo physics gfx scene audio net; do for f in $(find $K/src/$d -name '*.cpp' | sort); do srcs+=($f); done; done
srcs+=($K/tests/gl_stub.cpp)
for f in "${srcs[@]}"; do o=$O/e_$(echo ${f#$K/} | sed 's|/|_|g').o; if [ ! -f $o ] || [ $f -nt $o ]; then add "echo cc $f; nice c++ $flags -c $f -o $o.tmp && mv $o.tmp $o" 8; fi; done
flush
echo built
