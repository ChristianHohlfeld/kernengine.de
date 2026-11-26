#!/usr/bin/env bash
# Murmel für den Browser: die Engine als ein WebAssembly-Modul, das Projekt eingebettet und seine Blueprints - die App
# Murmel und die Bibliothek murmel_cpp des Programms, die Blueprints seiner Levels - als C++-Klassen (kern/build.sh
# --project --classes, kern/docs/app.md 7.), neben der Seite web/index.html (kern/web/app.mjs ist ihr Host). Die
# einzige Quelle des Spiels ist sein Einstieg src/main.cpp; die App schreibt der Konverter aus src/murmel.cpp in das
# Projekt (tests/convert.sh --write).
#
#   kern/games/murmel/build.sh            (WASI_SDK: wie kern/build.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kern="$(cd "$here/../.." && pwd)"
"$kern/build.sh" --app --game --project "$here/murmel.kproject" --classes "$here/web/murmel.wasm" "$here/src/main.cpp"
