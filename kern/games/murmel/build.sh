#!/usr/bin/env bash
# Murmel für den Browser: das Projekt eingebettet (kern/tools/embed_project.py), die App und die Engine als ein
# WebAssembly-Modul (kern/build.sh --app --game), neben der Seite web/index.html (kern/web/app.mjs ist ihr Host).
#
#   kern/games/murmel/build.sh            (WASI_SDK: wie kern/build.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kern="$(cd "$here/../.." && pwd)"
gen="${MURMEL_GEN:-$(cd "$kern/.." && pwd)/generated/murmel}"
mkdir -p "$gen"
python3 "$kern/tools/embed_project.py" "$here/murmel.kproject" "$gen/murmel_project.cpp"
"$kern/build.sh" --app --game "$here/web/murmel.wasm" -I"$here/src" "$here/src/murmel.cpp" "$here/src/main.cpp" "$gen/murmel_project.cpp"
