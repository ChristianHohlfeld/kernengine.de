#!/usr/bin/env bash
# Murmel nativ: der Spieltest (tests/murmel_test.cpp) als natives Programm der Engine (kern/tools/build_native.sh,
# kern/docs/app.md 7.) - dreimal gebaut und gespielt: die Blueprints des Projekts (die App Murmel, ihre Bibliothek, die
# Blueprints der Levels) im Interpreter, dann als ihre C++-Klassen (--classes, wie das Spiel sie einkompiliert), und die
# App als ihre Quelle src/murmel.cpp, nativ gebaut (MURMEL_QUELLE, ein Projekt ohne Programm-Blueprints). Jeder Lauf
# prüft dieselben erwarteten Werte (tests/erwartet.txt), und die drei Läufe sind derselbe, Zeile für Zeile (--ergebnis):
# die Quelle im C++-Teilsatz rechnet mit den Zahlen der Sprache wie ihr Blueprint (kern/docs/blueprint_cpp.md, „Zahlen“).
#
#   kern/games/murmel/tests/run.sh [--spur]        (CXX / CC: die Compiler; die Engine im Ordner dieses Laufs, kern/tools/run_dir.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
. "$kern/tools/run_dir.sh"   # (die Engine einmal für beide Programme dieses Laufs - in keinem anderen)
out="${TMPDIR:-/tmp}/murmel-test"
mkdir -p "$out"
# Die erwarteten Werte des Laufs (tests/erwartet.txt: Name Wert je Zeile).
defs=()
while read -r name wert; do [ -n "$name" ] && defs+=("-DMURMEL_$name=$wert"); done < "$here/erwartet.txt"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" -o "$out/murmel_test" \
  "${defs[@]}" -DMURMEL_STUFE='"interpreter"' "$here/murmel_test.cpp"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" --classes -o "$out/murmel_test_klassen" \
  "${defs[@]}" -DMURMEL_STUFE='"native C++"' "$here/murmel_test.cpp"
# (das Projekt ohne seine Programm-Blueprints, mit einer Kopie seiner Levels: die App ist die Quelle)
rm -rf "$out/quelle" && mkdir -p "$out/quelle" && cp -r "$game/levels" "$out/quelle/levels"
python3 - "$game/murmel.kproject" "$out/quelle/murmel.kproject" <<'PY'
import json, sys
p = json.load(open(sys.argv[1], encoding="utf-8"))
p["blueprints"] = []
json.dump(p, open(sys.argv[2], "w", encoding="utf-8"), indent=2)
PY
"$kern/tools/build_native.sh" --project "$out/quelle/murmel.kproject" -o "$out/murmel_test_quelle" \
  "${defs[@]}" -DMURMEL_STUFE='"interpreter"' -DMURMEL_QUELLE -I"$game/src" "$here/murmel_test.cpp" "$game/src/murmel.cpp"
echo "== die Blueprints im Interpreter"
"$out/murmel_test" --ergebnis "$out/interpreter.txt" "$@"
echo "== die Blueprints als ihre C++-Klassen"
"$out/murmel_test_klassen" --ergebnis "$out/klassen.txt" "$@"
echo "== die App als ihre Quelle, nativ gebaut"
"$out/murmel_test_quelle" --ergebnis "$out/quelle.txt" "$@"
# drei Wege, ein Ergebnis
for weg in klassen quelle; do
  if ! cmp -s "$out/interpreter.txt" "$out/$weg.txt"; then
    echo "run.sh: der Lauf \"$weg\" ist ein anderer als der im Interpreter:"
    diff "$out/interpreter.txt" "$out/$weg.txt" | head -20
    exit 1
  fi
done
echo "run.sh: drei Wege (Interpreter, Klassen, Quelle nativ), ein Ergebnis ($(wc -l < "$out/interpreter.txt") Zeilen)"
