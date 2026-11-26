# Die Engine als Objekte für die nativen Programme von Murmel (ohne Studio und Lehrer), im Ordner dieses Laufs
# (kern/tools/run_dir.sh - kein Zwischenspeicher über den Lauf hinaus): von tests/run.sh und tests/convert.sh mit `source` genommen. Danach stehen
# bereit: $out, $cxx, $cc, flags (die Schalter der C++-Übersetzung) und engine (die Objekte der Engine, mit
# $out/box3d/*.o und $out/musl/*.o zu linken). Braucht: $kern.
. "$kern/tools/run_dir.sh"
out="$KERN_RUN_DIR/murmel-obj"
cxx="${CXX:-c++}" cc="${CC:-cc}"
mkdir -p "$out/box3d" "$out/musl" "$out/obj" "$out/gen"
jobs=()
for c in "$kern"/third_party/box3d/src/*.c; do
  o="$out/box3d/$(basename "$c" .c).o"
  [ -f "$o" ] || jobs+=("$cc -std=c17 -O2 -DBOX3D_DISABLE_SIMD -ffp-contract=off -w -I$kern/third_party/box3d/include -I$kern/third_party/box3d/src -c $c -o $o")
done
for c in "$kern"/third_party/musl_math/src/*.c "$kern"/src/math/*.c; do
  o="$out/musl/$(basename "$c" .c).o"
  [ -f "$o" ] || jobs+=("$cc -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c $c -o $o")
done
. "$kern/third_party/musl_math/flags.sh"   # (kern_math_flags: the engine's mathematics, never computed by the compiler)
read -r -a mathflags <<< "$(kern_math_flags "$cxx")"
flags=(-std=c++20 -O1 -ffp-contract=off "${mathflags[@]}" -fno-exceptions -fno-rtti -w -I"$kern/include" -I"$kern/third_party/box3d/include")
# (ein Objekt je Quelle, unter dem Hash von Quelle, Flags und allen Headern der Engine: ein geänderter Header baut neu)
headers="$(find "$kern/include" "$kern/src" -name '*.h' -o -name '*.inc' | sort | xargs cat | sha256sum | cut -c1-12)"
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
