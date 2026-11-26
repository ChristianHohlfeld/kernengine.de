#!/usr/bin/env bash
# Murmel nativ: der Spieltest (tests/murmel_test.cpp) als natives Programm der Engine (kern/tools/build_native.sh,
# kern/docs/app.md 7.) - zweimal gebaut und gespielt: die Blueprints der Levels im Interpreter, dann als ihre
# C++-Klassen (--classes, wie ein Spiel sie einkompiliert) - derselbe Lauf, dieselben erwarteten Werte.
#
#   kern/games/murmel/tests/run.sh [--spur]        (CXX / CC: die Compiler; KERN_NATIVE_CACHE: die Objekte der Engine)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
out="${TMPDIR:-/tmp}/murmel-test"
# Die erwarteten Werte des Laufs (tests/erwartet.txt: Name Wert je Zeile).
defs=()
while read -r name wert; do [ -n "$name" ] && defs+=("-DMURMEL_$name=$wert"); done < "$here/erwartet.txt"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" -o "$out/murmel_test" \
  -I"$game/src" "${defs[@]}" -DMURMEL_STUFE='"interpreter"' "$here/murmel_test.cpp" "$game/src/murmel.cpp"
"$kern/tools/build_native.sh" --project "$game/murmel.kproject" --classes -o "$out/murmel_test_klassen" \
  -I"$game/src" "${defs[@]}" -DMURMEL_STUFE='"native C++"' "$here/murmel_test.cpp" "$game/src/murmel.cpp"
echo "== die Blueprints im Interpreter"
"$out/murmel_test" "$@"
echo "== die Blueprints als ihre C++-Klassen"
"$out/murmel_test_klassen" "$@"
