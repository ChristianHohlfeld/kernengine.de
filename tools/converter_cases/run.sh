#!/usr/bin/env bash
# Small cases of the converter (tools/cpp_to_blueprint.py), each a form of C++ the game has, and what its Blueprint
# text must say. Run: tools/converter_cases/run.sh (needs clang++ like the converter; KEEP=1 keeps the folder).
#   pad.cpp   a void function that writes through a reference to another object's state (`PadState& p = holder().pad;
#             p.lx = lx;` - main.cpp's sc_gamepad): a step, never KERN_PURE; a function that only changes its own
#             values (doubled) stays a value. Its test: holder() - the getter of the state's one object - is there the
#             object the call is compared on (kern_state_now); the test compiles and compares the functions.
#   bytes.cpp an export that hands a view of the host's bytes on (`sum_of({data, length})` - main.cpp's sc_net_push,
#             sc_dem_data): those entries, copied (Copy Bytes) - written, not waiting on "a view (Span) of a part of a list".
#   table.cpp a constant table of structs (`constexpr Spec kTable[]` of weapons.h) named from a namespace that has a
#             Spec of its own: its type is the one where it is declared (else its texts are read as another struct's Bools).
#   getter.cpp a function of another state (Other::look) reads the holder's state through its getter: no call of it is
#             compared on Holder's object - it waits, with the place and the reason, and its test does not call it on a
#             missing object (the test writer refuses the same where it writes a test: guard).
#   nodekind.cpp an enum of the engine's logic (kern::logic::NodeKind) in the program's source: in its test the real
#             enum (an alias), not a copy - else the field of a real NodeDef is another type (register_node of game.cpp).
# Each case is a program of its own, its Blueprints read into a level of their own, as the game's are: the engine's
# reader refuses what is wrong (a function read as a value that changes something).
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
here="$root/tools/converter_cases"
top="$(mktemp -d)"
[ -n "${KEEP:-}" ] && echo "($top)" >&2 || trap 'rm -rf "$top"' EXIT
fail=0
convert() {   # convert <case>: the case's program, converted with its test, into a level of its own
  out="$top/$1"
  mkdir -p "$out/src" "$out/texts" "$out/pt/converted"
  cp "$here/$1.h" "$here/$1.cpp" "$out/src/"
  # (the tests' own header and what it includes of the game: ../src from the tests' folder, as in the game)
  for h in "$root"/kern/games/spawncity/src/*.h; do [ -e "$out/src/$(basename "$h")" ] || cp "$h" "$out/src/"; done
  cp "$root"/kern/games/spawncity/tests/*.h "$out/pt/"
  echo '{"name":"Cases","nextId":1,"actors":[]}' > "$out/cases.klevel"
  (cd "$out" && FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert sc -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
     --level "$out/cases.klevel" --test "$out/pt/converted" --texts "$out/texts" -- "src/$1.cpp" > "$out/convert.log" 2>&1) \
    || { grep -v '^round' "$out/convert.log" | tail -20; echo "FAIL $1: the converter"; fail=1; return; }
  if grep -q "the Blueprints were not read" "$out/convert.log" || [ "$(grep -c '"logic"' "$out/cases.klevel")" = 0 ]; then
    echo "FAIL $1: the level reads its Blueprints"; grep -B2 -A2 "not read\|is read as a value" "$out/convert.log" | head; fail=1
  else echo "ok   $1: the level reads its Blueprints"; fi
}
check() {   # check <case> <what> yes|no <pattern in its Blueprint texts>
  local c=$1 what=$2 want=$3 pattern=$4 found=0
  grep -qE -- "$pattern" "$top/$c"/texts/*.cpp && found=1
  if { [ "$want" = yes ] && [ $found = 1 ]; } || { [ "$want" = no ] && [ $found = 0 ]; }; then echo "ok   $c: $what"; else echo "FAIL $c: $what"; fail=1; fi
}
tested() {    # tested <case> <what> <pattern in its tests>
  if grep -qE -- "$3" "$top/$1"/pt/converted/*.cpp 2>/dev/null; then echo "ok   $1: $2"; else echo "FAIL $1: $2"; fail=1; fi
}

convert pad
check pad "take_pad is written" yes '\bvoid take_pad\('
check pad "take_pad (writes through a reference) is a step, not KERN_PURE" no 'KERN_PURE[^(]*\btake_pad\('
check pad "doubled (only its own values) is a value: KERN_PURE" yes 'KERN_PURE doubledResult doubled\('
logged() {    # logged <case> <what> <pattern in its log>
  if grep -qE -- "$3" "$top/$1/convert.log"; then echo "ok   $1: $2"; else echo "FAIL $1: $2"; fail=1; fi
}
logged pad "the log says why take_pad is a step" '^ +take_pad \([0-9]+\): written \(a step: it changes p \(a reference it declares\)\)'
logged pad "the log says doubled is a value" '^ +doubled \([0-9]+\): written$'
tested pad "take_pad's test calls it on the object compared on" 'kern_state_now = s.get\(\), .*take_pad\('
tested pad "holder() in the test is that object" 'static_cast<Holder\*>\(kern_state_now\)\)\.pad'

convert getter
logged getter "Other::look waits with its reason, in the converter" '^ +Other::look \([0-9]+\): the state of the Blueprint Holder'
if grep -qs "look(" "$top/getter"/pt/converted/*.cpp; then echo "FAIL getter: no test calls Other::look"; fail=1; else echo "ok   getter: no test calls Other::look"; fi

convert nodekind
logged nodekind "is_event is written" '^ +is_event \([0-9]+\): written$'
tested nodekind "its test has the engine's NodeKind itself" 'using NodeKind = :: kern :: logic :: NodeKind;'

convert bytes
check bytes "sc_take_bytes is written, the view a copy of the host's bytes" yes 'list_copy_bytes<"u8", "u8">\(part, 0, data, data_at, length\)'

convert table
check table "width_and_glock is written, the table as weapons' Specs" yes 'weapons_Spec\{"MP", 55\.0, 180\.0\}'

if [ $fail != 0 ]; then
  for c in pad getter nodekind bytes table; do echo "--- $c"; grep -v '^round' "$top/$c/convert.log" | tail -8; done
  exit 1
fi
