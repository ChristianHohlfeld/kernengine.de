#!/usr/bin/env bash
# Murmel für den Browser: die App und die Engine als ein WebAssembly-Modul, das Projekt eingebettet und die
# Blueprints seiner Levels als C++-Klassen (kern/build.sh --project --classes, kern/docs/app.md 7.), neben der Seite
# web/index.html (kern/web/app.mjs ist ihr Host).
#
#   kern/games/murmel/build.sh            (WASI_SDK: wie kern/build.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kern="$(cd "$here/../.." && pwd)"
"$kern/build.sh" --app --game --project "$here/murmel.kproject" --classes "$here/web/murmel.wasm" -I"$here/src" "$here/src/murmel.cpp" "$here/src/main.cpp"
