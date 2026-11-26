#!/usr/bin/env bash
# The C++ sources a game's project brings into its build (kern/docs/app.md 7.): the project with its levels
# embedded (kern/tools/embed_project.py: kern::game::embedded_project()), and with --classes the Blueprints of each of
# its levels as C++ classes (tools/blueprint_cpp.sh - they then run compiled, the tier "native C++").
#
#   kern/tools/project_sources.sh <game.kproject> <out-dir> [--classes]     prints the sources, one a line
#
# kern/build.sh --project and kern/tools/build_native.sh --project use it; a game does not list them itself.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project="${1:?usage: kern/tools/project_sources.sh <game.kproject> <out-dir> [--classes]}"
out="${2:?usage: kern/tools/project_sources.sh <game.kproject> <out-dir> [--classes]}"
classes="${3:-}"
[ -f "$project" ] || { echo "project_sources.sh: $project: no such project file" >&2; exit 1; }
[ -z "$classes" ] || [ "$classes" = --classes ] || { echo "project_sources.sh: $classes: unknown option (--classes)" >&2; exit 1; }
mkdir -p "$out"
name="$(basename "$project" .kproject)"
python3 "$here/embed_project.py" "$project" "$out/${name}_project.cpp" >&2
echo "$out/${name}_project.cpp"
if [ "$classes" = --classes ]; then
  dir="$(cd "$(dirname "$project")" && pwd)"
  while IFS= read -r level; do
    [ -n "$level" ] || continue
    c="$out/$(basename "$level" .klevel)_blueprints.cpp"
    "$here/../../tools/blueprint_cpp.sh" "$dir/$level" "$c" >&2
    echo "$c"
  done < <(python3 -c 'import json,sys; [print(l) for l in json.load(open(sys.argv[1])).get("levels", [])]' "$project")
fi
