#!/usr/bin/env bash
# Die Blueprints der Levels von Murmel sind ihre Texte (kern/docs/blueprint_cpp.md): blueprints/<level>.<actor>.cpp
# ist der Blueprint des Akteurs <actor> im Level levels/<level>.klevel ("Level": der Level-Graph). Ohne Argument
# schreibt das Skript die Texte in die Levels (tools/blueprint_cpp.sh --apply); --check prüft, dass jedes Level genau
# die Blueprints seiner Texte hält (in einer Kopie angewendet, das Level danach unverändert).
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
root="$(cd "$game/../../.." && pwd)"
check=0
[ "${1:-}" = "--check" ] && check=1
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
status=0
for level in "$game"/levels/*.klevel; do
  name="$(basename "$level" .klevel)"
  args=()
  for t in "$game"/blueprints/"$name".*.cpp; do
    [ -e "$t" ] || continue
    actor="$(basename "$t" .cpp)"; actor="${actor#"$name".}"
    args+=("$actor" "$t")
  done
  [ ${#args[@]} -gt 0 ] || continue
  target="$level"
  if [ "$check" = 1 ]; then cp "$level" "$tmp/$name.klevel"; target="$tmp/$name.klevel"; fi
  "$root/tools/blueprint_cpp.sh" --apply "$target" "${args[@]}"
  if [ "$check" = 1 ] && ! python3 -c 'import json,sys; sys.exit(json.load(open(sys.argv[1])) != json.load(open(sys.argv[2])))' "$level" "$target"; then
    echo "blueprints.sh: levels/$name.klevel hält nicht die Blueprints seiner Texte (kern/games/murmel/tests/blueprints.sh schreibt sie)" >&2
    status=1
  fi
done
exit $status
