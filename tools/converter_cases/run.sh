#!/usr/bin/env bash
# Small cases of the converter (tools/cpp_to_blueprint.py), each a form of C++ the game has, and what its Blueprint
# text must say. Run: tools/converter_cases/run.sh (needs clang++ like the converter).
#   pad.cpp  a void function that writes through a reference to another object's state (`PadState& p = holder().pad;
#            p.lx = lx;` - main.cpp's sc_gamepad): a step, never KERN_PURE; a function that only changes its own
#            values (doubled) stays a value. Its test: holder() - the getter of the state's one object - is there the
#            object the call is compared on (kern_state_now), the test compiles and compares both functions.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
here="$root/tools/converter_cases"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT
mkdir -p "$out/src" "$out/texts" "$out/pt/converted"
cp "$here"/pad.h "$here"/pad.cpp "$out/src/"
# (the tests' own header and what it includes of the game: ../src from the tests' folder, as in the game)
for h in "$root"/kern/games/spawncity/src/*.h; do [ -e "$out/src/$(basename "$h")" ] || cp "$h" "$out/src/"; done
cp "$root"/kern/games/spawncity/tests/*.h "$out/pt/"
(cd "$out" && python3 "$root/tools/cpp_to_blueprint.py" --convert sc -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
   --test "$out/pt/converted" --texts "$out/texts" -- src/pad.cpp > "$out/convert.log" 2>&1) \
  || { cat "$out/convert.log"; echo "FAIL: the converter"; exit 1; }
fail=0
check() {   # check <what> <pattern that must be found> | ! <pattern that must not be>
  local what=$1 want=$2 pattern=$3
  if grep -qE -- "$pattern" "$out"/texts/*.cpp; then found=1; else found=0; fi
  if { [ "$want" = yes ] && [ $found = 1 ]; } || { [ "$want" = no ] && [ $found = 0 ]; }; then echo "ok   $what"; else echo "FAIL $what"; fail=1; fi
}
check "take_pad is written" yes '\bvoid take_pad\('
check "take_pad (writes through a reference) is a step, not KERN_PURE" no 'KERN_PURE[^(]*\btake_pad\('
check "doubled (only its own values) is a value: KERN_PURE" yes 'KERN_PURE doubledResult doubled\('
if grep -q "^2 functions of the Blueprints are compared" "$out/convert.log"; then echo "ok   both are compared by their tests"; else echo "FAIL both are compared by their tests"; grep -v '^round' "$out/convert.log" | tail -8; fail=1; fi
if grep -q "kern_state_now = s.get(), .*take_pad(" "$out"/pt/converted/*.cpp && grep -q "static_cast<Holder\*>(kern_state_now)).pad" "$out"/pt/converted/*.cpp; then
  echo "ok   holder() in the test is the object compared on"; else echo "FAIL holder() in the test is the object compared on"; fail=1; fi
[ $fail = 0 ] || { grep -h -A12 -E 'take_pad|doubled' "$out"/texts/*.cpp | head -40; exit 1; }
