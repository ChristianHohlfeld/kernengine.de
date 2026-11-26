#!/usr/bin/env bash
# Der Konverter (tools/cpp_to_blueprint.py) über die Quelle der App von Murmel (src/murmel.cpp, src/main.cpp), dann
# der Vergleich jeder geschriebenen Funktion mit ihrer Quelle, Bit für Bit. Der Konverter schreibt in eine Kopie des
# Projekts (die App und was sie benutzt: Programm-Blueprints, kern/docs/project.md 7a.) und ein Level für die Welt,
# die Texte nach $out/texts, die Vergleichstests nach $out/pt/converted (im Namensraum murmel, mit dem Testrahmen der
# Engine: kern/test/converted.h). Die Tests laufen gegen das Projekt, das der Konverter schrieb (eingebettet, mit
# seinem Level), die Blueprints als ihre C++-Klassen (tools/blueprint_cpp.sh) und im Interpreter - zweimal: gegen den
# Stellvertreter der Engine und gegen die Engine selbst (KERN_TEST_ENGINE=real).
#
#   kern/games/murmel/tests/convert.sh        (CLANGXX: der clang++ des Konverters; CXX/CC wie tests/run.sh;
#                                              KEEP=1 behält den Ordner)
# Endet mit 1, solange eine Funktion der App wartet, nicht verglichen wird oder ihr Vergleich abweicht.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
root="$(cd "$kern/.." && pwd)"
work="$(mktemp -d)"
[ -n "${KEEP:-}" ] && echo "($work)" >&2 || trap 'rm -rf "$work"' EXIT
mkdir -p "$work/src" "$work/texts" "$work/pt/converted"
cp "$game"/src/*.h "$game"/src/*.cpp "$work/src/"
cp -r "$game/levels" "$work/levels"
echo '{"format":"kern-level","version":1,"name":"Murmel Programm","nextId":1,"actors":[]}' > "$work/levels/programm.klevel"
# (die Kopie des Projekts nennt auch das Level, in das der Konverter die Welt schreibt: die Tests finden dort ihre Blueprints)
python3 - "$game/murmel.kproject" "$work/murmel.kproject" <<'PY'
import json, sys
p = json.load(open(sys.argv[1]))
p["levels"].append("levels/programm.klevel")
json.dump(p, open(sys.argv[2], "w"), indent=2)
PY
status=0
(cd "$work" && FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert murmel -I"$kern/include" -I"$kern/third_party/box3d/include" -Isrc \
   --level "$work/levels/programm.klevel" --project "$work/murmel.kproject" --folder Programm --test "$work/pt/converted" --texts "$work/texts" \
   -- src/murmel.cpp src/main.cpp) > "$work/convert.log" 2>&1 || status=$?
grep -v '^round' "$work/convert.log" | tail -60
echo "--- Blueprints im Level: $(python3 -c 'import json,sys; print(", ".join(a["name"] for a in json.load(open(sys.argv[1]))["actors"]))' "$work/levels/programm.klevel")"
echo "--- Blueprints im Projekt: $(python3 -c 'import json,sys; print(", ".join(b["name"] for b in json.load(open(sys.argv[1])).get("blueprints", [])))' "$work/murmel.kproject")"
ls "$work/texts" 2>/dev/null | sed 's/^/--- Text: /'
[ "$status" = 0 ] || { echo "convert.sh: der Konverter endet mit $status"; exit 1; }
grep -q '^  Murmel: the project - it implements App' "$work/convert.log" || { echo "convert.sh: die App Murmel steht nicht im Projekt"; exit 1; }
# Geschrieben und verglichen: jede Funktion (die Zahlen der Zusammenfassung des Konverters).
written="$(sed -n 's/^\([0-9]*\) of \([0-9]*\) functions of the program are written as Blueprints.*/\1 \2/p' "$work/convert.log")"
compared="$(sed -n 's/^\([0-9]*\) functions\{0,1\} of the Blueprints \(is\|are\) compared.*/\1/p' "$work/convert.log")"
read -r w all <<< "${written:-0 -1}"
echo "convert.sh: $w von $all Funktionen geschrieben, ${compared:-0} verglichen"
[ "$w" = "$all" ] || { echo "convert.sh: Funktionen der App warten (siehe oben)"; exit 1; }
# (verglichen wird jede, die als Funktion geschrieben ist; eine, die nur einen Platz benennt, steht in ihren Aufrufern -
# "a name for a place - written where it is called" - und wird mit ihnen verglichen)
functions="$(grep -cE '^    [^ ]+ \([0-9]+\): written' "$work/convert.log" || true)"
[ "${compared:-0}" = "$functions" ] || { echo "convert.sh: $functions Funktionen als Funktion geschrieben, aber ${compared:-0} verglichen (Gründe: siehe oben, \"cannot compare\")"; exit 1; }

# Die Vergleichstests bauen und fahren: die Engine (tests/engine.sh), das Projekt des Konverters eingebettet, seine
# Blueprints als Klassen, der Läufer und der Allokator der Engine (kern/tools/test_main.cpp, test_memory.cpp).
source "$here/engine.sh"
python3 "$kern/tools/embed_project.py" "$work/murmel.kproject" "$work/projekt.cpp"
"$root/tools/blueprint_cpp.sh" --project "$work/murmel.kproject" "$work/levels/programm.klevel" "$work/klassen.cpp"
"$cxx" "${flags[@]}" -I"$work" -I"$kern/tests" "$work"/pt/converted/*.cpp "$kern/tools/test_main.cpp" "$kern/tools/test_memory.cpp" \
  "$work/projekt.cpp" "$work/klassen.cpp" "${engine[@]}" "$out"/box3d/*.o "$out"/musl/*.o -lm -o "$work/vergleich"
echo "== Vergleich gegen den Stellvertreter der Engine"
"$work/vergleich"
echo "== Vergleich gegen die Engine selbst"
KERN_TEST_ENGINE=real "$work/vergleich"
