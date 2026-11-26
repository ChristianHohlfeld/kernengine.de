#!/usr/bin/env bash
# The native export: a Kern level - or a game's project with the sources of its App - as a standalone HTML game
# whose Blueprints are compiled C++ (kern/docs/app.md 7.).
#
#   tools/export_game.sh <level.klevel> <out.html> [title]
#   tools/export_game.sh <game.kproject> <out.html> [flags / sources of the App...]
#
# A project: kern/build.sh --app --game --project <it> --classes builds the module with the App's sources (its
# kern_main hands the engine its App), tools/export_app.mjs writes the page - the module, kern.mjs, audio.mjs and
# app.mjs inline, titled with the project's name. No path to the engine, no name of a game.
#
# Kern Studio's "Package Game" puts the level next to the engine module the editor runs; its
# Blueprints are then compiled by the browser when the game starts (the JIT). This script builds
# an engine module for the level instead:
#   1. tools/blueprint_cpp.sh writes every Blueprint of the level as its C++ class
#      (kern/docs/blueprint_cpp.md) - each node a direct function call;
#   2. kern/build.sh --studio compiles the engine with those classes, fully optimised;
#   3. tools/export_game.mjs writes the page (the same page "Package Game" writes).
# In the game every Blueprint runs as its class (the tier "native C++"); nothing is compiled or
# looked up when it starts. The level stays the source: export again after changing it.
#
# Needs what kern/build.sh needs (wasi-sdk: WASI_SDK), a host C++ compiler for the Blueprint
# tool, and node.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
level="${1:?usage: tools/export_game.sh <level.klevel> <out.html> [title] | <game.kproject> <out.html> [sources...]}"
out="${2:?usage: tools/export_game.sh <level.klevel> <out.html> [title] | <game.kproject> <out.html> [sources...]}"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
if [ "${level##*.}" = kproject ]; then
  shift 2
  "$root/kern/build.sh" --app --game ${KERN_COMMERCIAL:+--commercial} --project "$level" --classes "$work/game.wasm" "$@"
  node "$root/tools/export_app.mjs" "$level" "$work/game.wasm" "$out"
  exit 0
fi
"$root/tools/blueprint_cpp.sh" "$level" "$work/level_blueprints.cpp"
"$root/kern/build.sh" --studio ${KERN_COMMERCIAL:+--commercial} "$work/game.wasm" "$work/level_blueprints.cpp"
node "$root/tools/export_game.mjs" "$level" "$work/game.wasm" "$out" "${3:-}"
