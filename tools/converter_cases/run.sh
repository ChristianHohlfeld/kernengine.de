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
#   program.cpp + door.cpp  a program with its App (a state that implements kern::App - main.cpp's SpawnCityApp), an
#             export and the library the export uses, beside a file nothing of the program uses: the App, the export's
#             Blueprint and what it uses stand in the project, the door in the level - each with its reason in the log
#             (kern/docs/project.md: a level refuses an export); the project and the level both read, and their classes
#             are written from both (tools/blueprint_cpp.sh --project).
# Each case is a program of its own, its Blueprints read into a project and a level of their own, as the game's are:
# the engine's reader refuses what is wrong (a function read as a value that changes something, an export in a level).
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
here="$root/tools/converter_cases"
top="$(mktemp -d)"
[ -n "${KEEP:-}" ] && echo "($top)" >&2 || trap 'rm -rf "$top"' EXIT
fail=0
convert() {   # convert <case> [<file> ...]: the case's program (its files), converted with its test, into a project and a level of their own
  out="$top/$1"
  mkdir -p "$out/src" "$out/texts" "$out/pt/converted"
  local f files=()
  for f in "$@"; do cp "$here/$f.h" "$here/$f.cpp" "$out/src/"; files+=("src/$f.cpp"); done
  # (the tests' own header and what it includes of the game: ../src from the tests' folder, as in the game)
  for h in "$root"/kern/games/spawncity/src/*.h; do [ -e "$out/src/$(basename "$h")" ] || cp "$h" "$out/src/"; done
  cp "$root"/kern/games/spawncity/tests/*.h "$out/pt/"
  echo '{"name":"Cases","nextId":1,"actors":[]}' > "$out/cases.klevel"
  echo '{"format":"kern-project","version":1,"name":"Cases","levels":["cases.klevel"],"startLevel":"cases.klevel","blueprints":[]}' > "$out/cases.kproject"
  (cd "$out" && FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert sc -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
     --level "$out/cases.klevel" --project "$out/cases.kproject" --test "$out/pt/converted" --texts "$out/texts" -- "${files[@]}" > "$out/convert.log" 2>&1) \
    || { grep -v '^round' "$out/convert.log" | tail -20; echo "FAIL $1: the converter"; fail=1; return; }
  if grep -q "the Blueprints were not read" "$out/convert.log" || [ "$(cat "$out/cases.klevel" "$out/cases.kproject" | grep -c '"logic"')" = 0 ]; then
    echo "FAIL $1: the level and the project read their Blueprints"; grep -B2 -A2 "not read\|is read as a value" "$out/convert.log" | head; fail=1
  else echo "ok   $1: the level and the project read their Blueprints"; fi
}
stands() {    # stands <case> <Blueprint> level|project: it stands there, and not in the other
  local c=$1 name=$2 want=$3
  if python3 - "$top/$c" "$name" "$want" <<'PY'
import json, sys
d, name, want = sys.argv[1:]
level = {a["name"] for a in json.load(open(d + "/cases.klevel"))["actors"] if (a.get("components") or {}).get("logic")}
project = {b["name"] for b in json.load(open(d + "/cases.kproject"))["blueprints"] if b.get("logic")}
sys.exit(0 if (name in level, name in project) == (want == "level", want == "project") else 1)
PY
  then echo "ok   $c: $name stands in the $want"; else echo "FAIL $c: $name stands in the $want"; fail=1; fi
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

convert bytes
check bytes "sc_take_bytes is written, the view a copy of the host's bytes" yes 'list_copy_bytes<"u8", "u8">\(part, 0, data, data_at, length\)'

convert table
check table "width_and_glock is written, the table as weapons' Specs" yes 'weapons_Spec\{"MP", 55\.0, 180\.0\}'

convert program door
logged program "the log says the App stands in the project, and why" '^  CasesApp: the project - it implements App - the program.s App'
logged program "the log says the export's Blueprint stands in the project, and why" '^  program_cpp: the project - it exports sc_case_score - exports belong to the program'
logged program "the log says the library the export uses stands in the project, and why" '^  program: the project - the program.s program_cpp uses it'
logged program "the log says the door stands in the level, and why" '^  door_cpp: the level - a world.s'
stands program CasesApp project
stands program program_cpp project
stands program program project
stands program door_cpp level
stands program door level
check program "the export is written" yes 'KERN_EXPORT_AS\("sc_case_score"'
check program "the App's update takes what the engine hands as values, in the order of C++ (Engine&, const Frame&)" yes 'void update\(Int arg1 = 0, Frame arg2 = Frame\{'
if out=$("$root/tools/blueprint_cpp.sh" --project "$top/program/cases.kproject" "$top/program/cases.klevel" "$top/program/classes.cpp" 2>&1) && grep -q 'program_cpp' "$top/program/classes.cpp" && grep -q 'door_cpp' "$top/program/classes.cpp"
then echo "ok   program: the classes are written from the project and the level ($out)"; else echo "FAIL program: the classes from the project and the level: $out"; fail=1; fi
# (the level refuses an export: read into it, the export's Blueprint is not read)
cp "$top/program/cases.klevel" "$top/program/refuses.klevel"
if out=$("$root/tools/blueprint_cpp.sh" --apply "$top/program/refuses.klevel" --with "$top/program/cases.kproject" program_cpp "$top/program/texts/program_cpp.cpp" 2>&1); then
  echo "FAIL program: a level refuses an export"; fail=1
elif echo "$out" | grep -q 'is an export (sc_case_score) - exports belong to the program'; then echo "ok   program: a level refuses an export (with the reason)"
else echo "FAIL program: a level refuses an export, with the reason: $out"; fail=1; fi

if [ $fail != 0 ]; then
  for c in pad bytes table program; do echo "--- $c"; grep -v '^round' "$top/$c/convert.log" | tail -8; done
  exit 1
fi
