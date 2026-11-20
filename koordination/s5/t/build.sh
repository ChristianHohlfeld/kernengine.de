set -e
root=/home/claude/w-s5
srcs=("$root/kern/src/game/logic_node_shapes.cpp" $(find "$root/kern/src/logic" "$root/kern/src/core" -name '*.cpp' | sort))
mkdir -p obj
printf '%s\n' "${srcs[@]}" t.cpp | xargs -P 2 -I{} bash -c 'o=obj/$(basename "$1").o; [ "$o" -nt "$1" ] && [ "$1" != t.cpp ] || c++ -std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I'$root'/kern/include -I'$root'/kern/third_party/box3d/include -c "$1" -o "$o"' _ {}
for c in $root/kern/third_party/musl_math/src/*.c; do o=obj/$(basename $c).o; [ -f $o ] || cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $o; done
c++ obj/*.o -lm -o t
./t
