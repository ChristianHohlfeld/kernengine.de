#!/usr/bin/env bash
# Der Konverter (tools/cpp_to_blueprint.py) über die Quelle der App von Murmel (src/murmel.cpp, src/main.cpp): was er
# als Blueprint schreibt und worauf der Rest wartet. Er schreibt in eine Kopie des Projekts (die App und was sie benutzt:
# Programm-Blueprints, kern/docs/project.md 7a.) und ein Level für die Welt, die Texte nach $out/texts, die
# Vergleichstests nach $out/pt/converted. Geprüft: die App steht im Projekt (Murmel: the project - it implements App).
#
#   kern/games/murmel/tests/convert.sh        (CLANGXX: der clang++ des Konverters; KEEP=1 behält den Ordner)
# Endet mit 1, solange eine Funktion der App wartet: die App ist dann noch kein Blueprint.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
root="$(cd "$game/../../.." && pwd)"
out="$(mktemp -d)"
[ -n "${KEEP:-}" ] && echo "($out)" >&2 || trap 'rm -rf "$out"' EXIT
mkdir -p "$out/src" "$out/texts" "$out/pt/converted"
cp "$game"/src/*.h "$game"/src/*.cpp "$out/src/"
# (UMWEG: der Vergleichstest, den der Konverter schreibt, braucht den Testrahmen von SpawnCity - ../converted.h,
# SC_TEST, sc::converted -, und der seinen Teil von SpawnCitys Quelle; ein zweites Spiel hat nur diesen)
cp "$root"/kern/games/spawncity/tests/*.h "$out/pt/"
for h in "$root"/kern/games/spawncity/src/*.h; do [ -e "$out/src/$(basename "$h")" ] || cp "$h" "$out/src/"; done
echo '{"format":"kern-level","version":1,"name":"Murmel Programm","nextId":1,"actors":[]}' > "$out/program.klevel"
cp "$game/murmel.kproject" "$out/murmel.kproject"
status=0
(cd "$out" && python3 "$root/tools/cpp_to_blueprint.py" --convert murmel -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
   --level "$out/program.klevel" --project "$out/murmel.kproject" --folder Programm --test "$out/pt/converted" --texts "$out/texts" \
   -- src/murmel.cpp src/main.cpp) > "$out/convert.log" 2>&1 || status=$?
grep -v '^round' "$out/convert.log" | tail -60
echo "--- Blueprints im Level: $(python3 -c 'import json,sys; print(", ".join(a["name"] for a in json.load(open(sys.argv[1]))["actors"]))' "$out/program.klevel")"
echo "--- Blueprints im Projekt: $(python3 -c 'import json,sys; print(", ".join(b["name"] for b in json.load(open(sys.argv[1])).get("blueprints", [])))' "$out/murmel.kproject")"
ls "$out/texts" 2>/dev/null | sed 's/^/--- Text: /'
[ "$status" = 0 ] || { echo "convert.sh: der Konverter endet mit $status"; exit 1; }
if grep -qi 'wait' "$out/convert.log"; then echo "convert.sh: Funktionen der App warten (siehe oben)"; exit 1; fi
grep -q '^  Murmel: the project - it implements App' "$out/convert.log" || { echo "convert.sh: die App Murmel steht nicht im Projekt"; exit 1; }
