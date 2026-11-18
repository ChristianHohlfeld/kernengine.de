#!/usr/bin/env bash
# The final run on the test tree (/home/claude/kernengine.de), at origin/work as it is now:
#   final.sh <tag>
# 1. the full conversion of the game with its tests (writes the level and tests/converted there)
# 2. the game's whole suite: its own tests and the comparison of every converted function
# Logs: $S/final/<tag>/{when,convert.log,suite.log}; `when` says how each stage ended.
S="$(cd "$(dirname "$0")" && pwd)"
tag="$1"
out="$S/final/$tag"
main=/home/claude/kernengine.de
mkdir -p "$out"
: > "$out/when"
cd "$main" || exit 1
git fetch -q origin && git checkout -q -f --detach origin/work && git clean -q -fd kern/games/spawncity/tests/converted
echo "tree $(git log --oneline -1)" >> "$out/when"
echo "convert: start $(date +%H:%M)" >> "$out/when"
cd "$main/kern/games/spawncity" || exit 1
if ! FUNCTIONS=1 KERN_JOBS=2 TMPDIR="$S/shared-tmp" python3 ../../../tools/cpp_to_blueprint.py --convert sc --program firmware=fc,hwcontract --level level/spawncity.klevel --test tests/converted --texts "$out/texts" \
      -I../../include -I../../third_party/box3d/include -Isrc -- $(ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp\|main.cpp") > "$out/convert.log" 2>&1; then
  echo "FAILED at convert $(date +%H:%M)" >> "$out/when"; exit 1
fi
grep -E "functions of the program are written|compared with their source" "$out/convert.log" >> "$out/when"
echo "suite: start $(date +%H:%M)" >> "$out/when"
cd "$main" || exit 1
if ! KERN_JOBS=2 kern/games/spawncity/tests/run_tests.sh > "$out/suite.log" 2>&1; then
  grep -E "tests, .* checks" "$out/suite.log" | tail -1 >> "$out/when"
  echo "FAILED at suite $(date +%H:%M)" >> "$out/when"; exit 1
fi
grep -E "tests, .* checks" "$out/suite.log" | tail -1 >> "$out/when"
echo "cost lines: $(grep -c 'cost:' "$out/suite.log")" >> "$out/when"
echo "DONE $(date +%H:%M)" >> "$out/when"
