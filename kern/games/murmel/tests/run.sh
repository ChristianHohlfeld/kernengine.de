#!/usr/bin/env bash
# Murmel nativ: baut die Engine (ohne Studio und Lehrer, Objekte zwischengespeichert in $MURMEL_OBJ), bettet das
# Projekt ein (kern/tools/embed_project.py), baut den Test des Spiels und spielt ihn (tests/murmel_test.cpp).
#
#   kern/games/murmel/tests/run.sh [--spur]        (CXX / CC: die Compiler, Vorgabe c++ / cc)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
out="${MURMEL_OBJ:-${TMPDIR:-/tmp}/murmel-obj}"
cxx="${CXX:-c++}" cc="${CC:-cc}"
mkdir -p "$out/box3d" "$out/musl" "$out/obj" "$out/gen"
jobs=()
for c in "$kern"/third_party/box3d/src/*.c; do
  o="$out/box3d/$(basename "$c" .c).o"
  [ -f "$o" ] || jobs+=("$cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I$kern/third_party/box3d/include -I$kern/third_party/box3d/src -c $c -o $o")
done
for c in "$kern"/third_party/musl_math/src/*.c; do
  o="$out/musl/$(basename "$c" .c).o"
  [ -f "$o" ] || jobs+=("$cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $o")
done
flags=(-std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -I"$kern/include" -I"$kern/third_party/box3d/include")
# (ein Objekt je Quelle, unter dem Hash von Quelle, Flags und allen Headern der Engine: ein geänderter Header baut neu)
headers="$(find "$kern/include" -name '*.h' | sort | xargs cat | sha256sum | cut -c1-12)"
engine=()
while IFS= read -r f; do
  o="$out/obj/$(echo "$f" | sed "s|$kern/||; s|/|_|g")-$( { echo "$headers ${flags[*]} $cxx"; cat "$f"; } | sha256sum | cut -c1-12).o"
  engine+=("$o")
  [ -f "$o" ] || jobs+=("$cxx ${flags[*]} -c $f -o $o")
done < <(find "$kern/src" -name '*.cpp' -not -path '*/studio/*' -not -path '*/lehrer/*' | sort; echo "$kern/tests/gl_stub.cpp")
if [ ${#jobs[@]} -gt 0 ]; then
  echo "murmel: ${#jobs[@]} Objekte der Engine zu bauen"
  printf '%s\n' "${jobs[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} sh -c '{}'
fi
python3 "$kern/tools/embed_project.py" "$game/murmel.kproject" "$out/gen/murmel_project.cpp"
# Die erwarteten Werte des Laufs (tests/erwartet.txt: Name Wert je Zeile).
defs=()
while read -r name wert; do [ -n "$name" ] && defs+=("-DMURMEL_$name=$wert"); done < "$here/erwartet.txt"
"$cxx" "${flags[@]}" -I"$game/src" -I"$kern/tests" "${defs[@]}" "$here/murmel_test.cpp" "$game/src/murmel.cpp" "$out/gen/murmel_project.cpp" \
  "${engine[@]}" "$out"/box3d/*.o "$out"/musl/*.o -lm -o "$out/murmel_test"
"$out/murmel_test" "$@"
