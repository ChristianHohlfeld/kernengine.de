#!/usr/bin/env bash
# Murmel nativ: der Spieltest (tests/murmel_test.cpp) als natives Programm der Engine (kern/tools/build_native.sh,
# kern/docs/app.md 7.) - zweimal gebaut und gespielt: die Blueprints der Levels im Interpreter, dann als ihre
# C++-Klassen (--classes, wie ein Spiel sie einkompiliert) - derselbe Lauf, dieselben erwarteten Werte, und beide Läufe
# Zeile für Zeile gleich (--ergebnis).
#
#   kern/games/murmel/tests/run.sh [--spur]        (CXX / CC: die Compiler; die Engine im Ordner dieses Laufs, kern/tools/run_dir.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
. "$kern/tools/run_dir.sh"   # (die Engine einmal für beide Programme dieses Laufs - in keinem anderen)
out="${TMPDIR:-/tmp}/murmel-test"
# Die erwarteten Werte des Laufs (tests/erwartet.txt: Name Wert je Zeile).
defs=()
while read -r name wert; do [ -n "$name" ] && defs+=("-DMURMEL_$name=$wert"); done < "$here/erwartet.txt"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" -o "$out/murmel_test" \
  -I"$game/src" "${defs[@]}" -DMURMEL_STUFE='"interpreter"' "$here/murmel_test.cpp" "$game/src/murmel.cpp"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" --classes -o "$out/murmel_test_klassen" \
  -I"$game/src" "${defs[@]}" -DMURMEL_STUFE='"native C++"' "$here/murmel_test.cpp" "$game/src/murmel.cpp"
mkdir -p "$out"
echo "== die Blueprints im Interpreter"
"$out/murmel_test" --ergebnis "$out/interpreter.txt" "$@"
echo "== die Blueprints als ihre C++-Klassen"
"$out/murmel_test_klassen" --ergebnis "$out/klassen.txt" "$@"
# die Wege, ein Ergebnis: derselbe Lauf, Zeile für Zeile (die Spur, Frames, Punkte, Töne, Widgets). Die App als
# Programm-Blueprint (dritter Weg) wartet, bis die Sprache die Zahl-Typen der Quelle hat (f32: gewonnen_seit_).
if ! cmp -s "$out/interpreter.txt" "$out/klassen.txt"; then
  echo "run.sh: der Lauf mit den Klassen ist ein anderer als der im Interpreter:"
  diff "$out/interpreter.txt" "$out/klassen.txt" | head -20
  exit 1
fi
echo "run.sh: Interpreter und Klassen, ein Ergebnis ($(wc -l < "$out/interpreter.txt") Zeilen)"
