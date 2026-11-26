#!/usr/bin/env bash
# Murmel nativ: baut die Engine (ohne Studio und Lehrer, Objekte zwischengespeichert in $MURMEL_OBJ), bettet das
# Projekt ein (kern/tools/embed_project.py), baut den Test des Spiels und spielt ihn (tests/murmel_test.cpp).
#
#   kern/games/murmel/tests/run.sh [--spur]        (CXX / CC: die Compiler, Vorgabe c++ / cc)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
source "$here/engine.sh"
python3 "$kern/tools/embed_project.py" "$game/murmel.kproject" "$out/gen/murmel_project.cpp"
# Die erwarteten Werte des Laufs (tests/erwartet.txt: Name Wert je Zeile).
defs=()
while read -r name wert; do [ -n "$name" ] && defs+=("-DMURMEL_$name=$wert"); done < "$here/erwartet.txt"
# Zweimal gebaut und gespielt: die Blueprints der Levels im Interpreter, dann als ihre C++-Klassen (tools/blueprint_cpp.sh,
# wie ein Spiel sie einkompiliert) - derselbe Lauf, dieselben erwarteten Werte.
classes=()
for level in "$game"/levels/*.klevel; do
  c="$out/gen/$(basename "$level" .klevel)_blueprints.cpp"
  "$kern/../tools/blueprint_cpp.sh" "$level" "$c"
  classes+=("$c")
done
"$cxx" "${flags[@]}" -I"$game/src" -I"$kern/tests" "${defs[@]}" -DMURMEL_STUFE='"interpreter"' "$here/murmel_test.cpp" "$game/src/murmel.cpp" "$out/gen/murmel_project.cpp" \
  "${engine[@]}" "$out"/box3d/*.o "$out"/musl/*.o -lm -o "$out/murmel_test"
"$cxx" "${flags[@]}" -I"$game/src" -I"$kern/tests" "${defs[@]}" -DMURMEL_STUFE='"native C++"' "$here/murmel_test.cpp" "$game/src/murmel.cpp" "$out/gen/murmel_project.cpp" \
  "${classes[@]}" "${engine[@]}" "$out"/box3d/*.o "$out"/musl/*.o -lm -o "$out/murmel_test_klassen"
echo "== die Blueprints im Interpreter"
"$out/murmel_test" "$@"
echo "== die Blueprints als ihre C++-Klassen"
"$out/murmel_test_klassen" "$@"
