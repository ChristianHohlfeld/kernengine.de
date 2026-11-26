#!/usr/bin/env bash
# Small cases of the converter (tools/cpp_to_blueprint.py), each a form of C++ a program has, and what its Blueprint
# text must say. Their namespace is `cases`: the converter is given it (--convert cases) and knows no game's name -
# the first case checks that the tools say none (names.sh). Run: tools/converter_cases/run.sh (needs clang++ like the converter; KEEP=1 keeps the folder).
#   pad.cpp   a void function that writes through a reference to another object's state (`PadState& p = holder().pad;
#             p.lx = lx;` - an export that takes a gamepad's state): a step, never KERN_PURE; a function that only changes its own
#             values (doubled) stays a value. Its test: holder() - the getter of the state's one object - is there the
#             object the call is compared on (kern_state_now); the test compiles and compares the functions.
#   bytes.cpp an export that hands a view of the host's bytes on (`sum_of({data, length})` - bytes from the
#             network, a file): those entries, copied (Copy Bytes) - written, not waiting on "a view (Span) of a part of a list".
#   table.cpp a constant table of structs (`constexpr Spec kTable[]`) named from a namespace that has a
#             Spec of its own: its type is the one where it is declared (else its texts are read as another struct's Bools).
#   program.cpp + door.cpp  a program with its App (a state that implements kern::App, its one object made in the entry
#             `int kern_main() { static CasesApp app; return kern::run(app); }` as kern/docs/project.md 4. writes it), an
#             export and the library the export uses, beside a file nothing of the program uses: the App, the export's
#             Blueprint and what it uses stand in the project, the door in the level - each with its reason in the log
#             (kern/docs/project.md: a level refuses an export); the project and the level both read, and their classes
#             are written from both (tools/blueprint_cpp.sh --project).
#   getter.cpp a function of another state (Other::look) reads the holder's state through its getter: no call of it is
#             compared on Holder's object - it waits, with the place and the reason, and its test does not call it on a
#             missing object (the test writer refuses the same where it writes a test: guard).
#   nodekind.cpp an enum of the engine's logic (kern::logic::NodeKind) in the program's source: in its test the real
#             enum (an alias), not a copy - else the field of a real NodeDef is another type (register_node of a program's node types).
#   compiles.py the test writer's compile check: "too many errors emitted" is no complaint of a piece (the piece
#             before it went for it).
#   placement.py where the Blueprints stand (place_blueprints, apply_to_level): what a Blueprint of the project uses is
#             the program's; a source of its own (another folder) the program uses that stands in the level stops the run;
#             --classes takes the header only (the project says which Blueprints the program calls as their classes).
#   frame.cpp the engine's Frame named as a program writes it (`using namespace kern; ... const Frame& frame`), beside
#             the class Frame inside kern::logic::Instance: the engine's (marked for Blueprints), never the class of a
#             class ("Frame not KERN_BLUEPRINT_TYPE") - written and compared.
#   makeworld.cpp std::make_unique of an object of the engine with several constructors of that count: the one whose
#             parameters take the arguments (GameWorld(Engine&) for an Engine, GameWorld(Renderer&, ...) for a Renderer).
#   nested.cpp a struct with a struct of its own (Stream::Tile) beside another Tile (mvt::Tile), named first from mvt: its
#             field `kern::Map<u64, std::unique_ptr<Tile>>` (Tile declared before it) holds Stream::Tile - a field's type is looked up inside its struct.
#   ownget.cpp an object of the engine a state holds as its own (std::unique_ptr<GameWorld>): its get() is the handle
#             - written and compared.
#   objects.cpp objects of the engine are Object<I> (kern/docs/engine_object.md), never a number: as an input, what a
#             function gives, a field of a struct, a list of pointers, unique_ptr<...>::get(), nullptr, `if (p)`, two
#             compared, a function of the engine called on one - written and compared, and no Handle of Object.
#   actors.cpp actors are Actors (kern::logic::ActorHandle: an id and its world, kern/docs/project.md 7), never a number: as
#             an input, what a function gives, a list of them, none (Actor{}), two compared (Same Actor), the id (Id of
#             Actor, the one way to an Int) - written and compared; one made of numbers waits with its reason.
#   meshextra.cpp the engine's MeshData::push_extra(const f32* values) handed only arrays of a fixed length (`const f32
#             rec[3]`): a view to read is an input by value that takes a list or an array - the reader reads the call
#             (before: "the reference input values is float_list, but rec is float_list[3]", the Blueprints not read).
#   synth.cpp a struct and an enum of the engine (kern::audio::SynthPatch, Wave) the file names with `using kern::audio::Wave;`:
#             in its test the enum stands before every `using` of it (before: `using audio::Wave;` in front of the enum
#             itself - the test went without it, its function was not compared).
#   views.cpp views of the program's own and the engine's: kern::Span<const f32> and const f32* (to read) are inputs by
#             value, handed an array of a fixed length, a list, a pointer into a list - both in one program; kern::Span<f32>
#             (to write through) is a reference input. A range-for over a view only reads: sum_of is a value.
# Each case is a program of its own, its Blueprints read into a project and a level of their own, as a game's are:
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
  echo '{"name":"Cases","nextId":1,"actors":[]}' > "$out/cases.klevel"
  echo '{"format":"kern-project","version":1,"name":"Cases","levels":["cases.klevel"],"startLevel":"cases.klevel","blueprints":[]}' > "$out/cases.kproject"
  (cd "$out" && FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert cases --folder Cases -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
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
logged() {    # logged <case> <what> <pattern in its log>
  if grep -qE -- "$3" "$top/$1/convert.log"; then echo "ok   $1: $2"; else echo "FAIL $1: $2"; fail=1; fi
}

"$here/names.sh" || fail=1

convert pad
check pad "take_pad is written" yes '\bvoid take_pad\('
check pad "take_pad (writes through a reference) is a step, not KERN_PURE" no 'KERN_PURE[^(]*\btake_pad\('
check pad "doubled (only its own values) is a value: KERN_PURE" yes 'KERN_PURE doubledResult doubled\('
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

convert ownget
logged ownget "Holder::world (unique_ptr::get()) is written" '^ +Holder::world \([0-9]+\): written$'
logged ownget "both are compared" '^2 functions of the Blueprints are compared'

convert frame
logged frame "seconds_of (const Frame&: the engine's kern::Frame) is written" '^ +seconds_of \([0-9]+\): written$'
logged frame "it is compared" '^1 functions? of the Blueprints (is|are) compared'

convert makeworld
logged makeworld "Maker::make (from the Engine) is written with GameWorld(Engine&)" '^ +Maker::make \([0-9]+\): written .*runs the engine.s engine\.game_world\.make_2;'
logged makeworld "Maker::make_drawn (from the Renderer) is written with GameWorld(Renderer&, ...)" '^ +Maker::make_drawn \([0-9]+\): written .*runs the engine.s engine\.game_world\.make;'
logged makeworld "both are compared" '^2 functions of the Blueprints are compared'
convert objects
check objects "an Object as an input" yes 'void add\(Object<kern::audio::Mixer> m = Object<kern::audio::Mixer>\{\}\)'
check objects "if (p): Is Valid Object" yes 'if \(object_valid<kern::audio::Mixer>\(m\)\)'
check objects "a list of pointers: a List of Objects" yes 'List<Object<kern::audio::Mixer>> mixers_ = List<Object<kern::audio::Mixer>>\{\}'
check objects "what a function gives, nullptr: none" yes 'Object<kern::audio::Mixer>\{\}, mixers_\[0\]'
check objects "unique_ptr<...>::get(): the Object it owns" yes 'return \{own_\};'
check objects "against nullptr: Is Valid Object" yes 'return \{object_valid<kern::audio::Mixer>\(own_\)\};'
check objects "two compared: Same Object" yes 'object_equal<kern::audio::Mixer>\(first\(\)\.Result, own\(\)\.Result\)'
check objects "a field of a struct" yes 'KERN_STRUCT\(Rig\) \{ Object<kern::audio::Mixer> mixer = Object<kern::audio::Mixer>\{\};'
check objects "a function of the engine on an Object" yes 'engine_mixer_sample_rate\(m\)'
check objects "no Object becomes a number (Handle of Object)" no 'object_handle'
logged objects "all nine are written and compared" '^9 functions of the Blueprints are compared'

convert actors
check actors "an Actor as an input" yes 'void add\(Actor a = Actor\{\}\)'
check actors "against none: Same Actor with Actor{}" yes 'logic_not\(actor_equal\(a, Actor\{\}\)\)'
check actors "a list of them: a List of Actors" yes 'List<Actor> members_ = List<Actor>\{\}'
check actors "two compared: Same Actor" yes 'actor_equal\('
check actors "its id: Id of Actor" yes 'actor_id\('
logged actors "an Actor made of numbers waits, with the reason" '^ +made_of \([0-9]+\): .*an Actor made of numbers: an actor comes from its world'
logged actors "the five of Crowd are written and compared" '^5 functions of the Blueprints are compared'

convert bytes
check bytes "take_bytes is written, the view a copy of the host's bytes" yes 'list_copy_bytes<"u8", "u8">\(part, 0, data, data_at, length\)'

convert table
check table "width_and_glock is written, the table as weapons' Specs" yes 'weapons_Spec\{"MP", 55\.0, 180\.0\}'

convert nested
check nested "Stream's field holds Stream::Tile, named from inside the struct (not mvt::Tile)" yes 'List<Stream_Tile>'
check nested "... and nowhere a list of mvt::Tile" no 'List<mvt_Tile>'
logged nested "both are compared" '^2 functions of the Blueprints are compared'

convert meshextra
check meshextra "push_extra's values is an input by value (a view to read)" yes 'void MeshData_push_extra\(MeshData& self, List<Float> values = List<Float>\{\}, Int values_at = 0\)'
logged meshextra "extra_of_the_shore (hands an array) is written" '^ +extra_of_the_shore \([0-9]+\): written'

convert views
check views "Span<const f32> is an input by value" yes 'sum_ofResult sum_of\(List<Float> values = List<Float>\{\}\)'
check views "const f32* is an input by value and its place" yes 'first_ofResult first_of\(List<Float> values = List<Float>\{\}, Int values_at = 0\)'
check views "Span<f32> (to write through) is a reference input" yes 'void fill\(List<Float>& out, Float v = 0\.0\)'
logged views "sum_of (a range-for over a view) is a value" '^ +sum_of \([0-9]+\): written$'
logged views "first_of is a value" '^ +first_of \([0-9]+\): written$'
logged views "push_extra handed an array is written" '^ +extra_from_an_array \([0-9]+\): written'
logged views "push_extra handed a list and a place in it is written" '^ +extra_from_a_list \([0-9]+\): written'
logged views "fill (a view to write through) is written" '^ +fill \([0-9]+\): written'
logged views "all eight are written and compared" '^8 functions of the Blueprints are compared'

convert synth
logged synth "bright is written" '^ +bright \([0-9]+\): written$'
logged synth "it is compared" '^1 functions? of the Blueprints (is|are) compared'

convert program door
logged program "the log says the App stands in the project, and why" '^  CasesApp: the project - it implements App - the program.s App'
logged program "the log says the export's Blueprint stands in the project, and why" '^  program_cpp: the project - it exports case_score - exports belong to the program'
logged program "the log says the library the export uses stands in the project, and why" '^  program: the project - the program.s program_cpp uses it'
logged program "the log says the door stands in the level, and why" '^  door_cpp: the level - a world.s'
stands program CasesApp project
stands program program_cpp project
stands program program project
stands program door_cpp level
stands program door level
check program "the export is written" yes 'KERN_EXPORT_AS\("case_score"'
check program "the App's update takes what the engine hands as values, in the order of C++ (Engine&, const Frame&)" yes 'void update\(Object<kern::Engine> arg1 = Object<kern::Engine>\{\}, Frame arg2 = Frame\{'
if out=$("$root/tools/blueprint_cpp.sh" --project "$top/program/cases.kproject" "$top/program/cases.klevel" "$top/program/classes.cpp" 2>&1) && grep -q 'program_cpp' "$top/program/classes.cpp" && grep -q 'door_cpp' "$top/program/classes.cpp"
then echo "ok   program: the classes are written from the project and the level ($out)"; else echo "FAIL program: the classes from the project and the level: $out"; fail=1; fi
# (the level refuses an export: read into it, the export's Blueprint is not read)
cp "$top/program/cases.klevel" "$top/program/refuses.klevel"
if out=$("$root/tools/blueprint_cpp.sh" --apply "$top/program/refuses.klevel" --with "$top/program/cases.kproject" program_cpp "$top/program/texts/program_cpp.cpp" 2>&1); then
  echo "FAIL program: a level refuses an export"; fail=1
elif echo "$out" | grep -q 'is an export (case_score) - exports belong to the program'; then echo "ok   program: a level refuses an export (with the reason)"
else echo "FAIL program: a level refuses an export, with the reason: $out"; fail=1; fi

python3 "$here/compiles.py" || fail=1
python3 "$here/placement.py" || fail=1

if [ $fail != 0 ]; then
  for c in pad getter nodekind ownget frame makeworld objects actors bytes table nested meshextra views synth program; do echo "--- $c"; grep -v '^round' "$top/$c/convert.log" | tail -8; done
  exit 1
fi
