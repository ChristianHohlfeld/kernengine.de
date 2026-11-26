#!/usr/bin/env bash
# The tools of the engine know no game: no game's namespace, macro or name in the converter
# (tools/cpp_to_blueprint.py), the writer of the classes (tools/blueprint_cpp.cpp, .sh), these cases, the engine's
# tools (kern/tools) and the frame of the tests (kern/include/kern/test). A game hands the converter its namespace
# (--convert <ns>); what a test needs of the game is the embedded project (kern/test/blueprint.h).
#   tools/converter_cases/names.sh        (a case of tools/converter_cases/run.sh; the CI's job converter-cases)
set -uo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
# (the games of this repository by their names; the patterns stand split, so that this file says none of them)
games=("spawn""city" "mur""mel")
pattern="(^|[^A-Za-z0-9_])s""c::|\\bS""C_[A-Z]"
for g in "${games[@]}"; do pattern+="|$g"; done
places=("$root/tools/cpp_to_blueprint.py" "$root/tools/blueprint_cpp.cpp" "$root/tools/blueprint_cpp.sh" "$root/tools/converter_cases" "$root/kern/tools" "$root/kern/include/kern/test")
found="$(grep -rniE "$pattern" --exclude='*.pyc' "${places[@]}" | sed "s|$root/||" | head -20)"
if [ -n "$found" ]; then
  echo "FAIL names: the tools name a game - a game's namespace, macro or name stands in:"
  echo "$found" | cut -c1-200 | sed 's/^/  /'
  exit 1
fi
echo "ok   names: no game's name in the converter, the class writer, the cases, kern/tools and kern/include/kern/test"
