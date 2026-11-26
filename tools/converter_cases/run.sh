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
#   libraries.py every header of the engine that marks a type for Blueprints has its library; one that has none stops the
#             converter before it converts anything, with the header; a text that names what the libraries do not have is
#             found with who and the line (the one check, tools/engine_api.py --check runs it on texts too).
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
#             compared, a function of the engine called on one - written and compared, and no Handle of Object; its classes
#             compile (a struct of the program with an Object field is held as that Object - before: as an Actor).
#   localobj.cpp an object of the engine made as a local (`kern::Earcut ec; ec.triangulate(...)`): the Object its make
#             gives, released where its block ends - written, compared, its class compiles (before: "an object of the
#             engine as another type (Object<kern::Earcut> where Int is wanted)").
#   actors.cpp actors are Actors (kern::logic::ActorHandle: an id and its world, kern/docs/project.md 7), never a number: as
#             an input, what a function gives, a list of them, none (Actor{}), two compared (Same Actor), the id (Id of
#             Actor, the one way to an Int) - written and compared; one made of numbers waits with its reason.
#   meshextra.cpp the engine's MeshData::push_extra(const f32* values) handed only arrays of a fixed length (`const f32
#             rec[3]`): a view to read is an input by value that takes a list or an array - the reader reads the call
#             (before: "the reference input values is float_list, but rec is float_list[3]", the Blueprints not read).
#   objects.cpp once more, from a folder of another length: the same texts and the same level, byte for byte.
#   inner.cpp an export inside the program's namespace (KERN_EXPORT: `extern "C"` there, as a game's main.cpp has them):
#             what the block declares stands in the namespace - the export is written (one tree, unit_dump).
#   patch.cpp a struct and an enum of the engine (kern::audio::SynthPatch, Wave) the file names with `using kern::audio::Wave;`:
#             in its test the enum stands before every `using` of it (before: `using audio::Wave;` in front of the enum
#             itself - the test went without it, its function was not compared).
#   chain.cpp + chain_b.cpp  a chain of calls over the two parts of a state's Blueprint (root <- step1 <- step2 <- step3 <- step4,
#             from file to file): root waits for a reason of its own, and every one that calls it waits, saying the one it calls
#             - all in the same round, whatever the length of the chain (before: one link a round, and step2/step4 waited on
#             their tests, which did not hold what they lean on). Its log says, round by round, why the round is needed.
#   look.cpp + lookapp.cpp  an App whose field starts with what is computed (`ext_[1] = {&look()}`: the engine object of
#             another state, Look) and that declares no constructor: C++'s own constructor, which makes that field, is
#             its function (LookApp::LookApp - before: every function reading ext_ waited, "declare one: `LookApp();` ... =
#             default"); Look, whose fields start with nothing computed, has no such function (nothing to make).
#   far.cpp + far_b.cpp + far_rules.cpp  a function (reach) that calls one of another file (far::pole_radius), whose pieces
#             name a class of a third file's header (rules::Blueprint : kern::logic::ActorBlueprint, far_rules.h) that the
#             caller does not include: its test holds the pieces with the headers of their files - written and compared
#             (before: "expected class name", the pieces went, and reach waited on "use of undeclared identifier 'far'").
#             far.klevel: the level's actor Lamps, whose variable the pieces read.
#   worldrules.cpp + worldmix.cpp  the program's C++ binds an ActorBlueprint to a world it runs (bind(GameWorld&), kern/docs/
#             blueprint_cpp.md "An einen Actor einer Welt gebunden"): in the Blueprint of the program's one object (Runner) a
#             variable Actor, set by bind with GameWorld actor named, which every call and read goes to; the tests bind the
#             source's ActorBlueprint to the test's world (kern::test::BlueprintRun). worldmix: one bound to a world and to a
#             logic waits, with the reason. worldrules.klevel: the actor Lamps (PoleRadius, Twice).
#   blasts.cpp a struct of a state's own (LookState::Blast) beside one of the namespace of the same short name (Blast): a list
#             of it read through a reference (`LookState& L = look_; L.blasts.size()`) is the state's - the compiler writes the
#             name in full (<namespace>::LookState::Blast), and that is the one (before: "a List<LookState_Blast> where a
#             List<Blast> is needed").
#   pieces.cpp exports that give a text and bytes by a pointer and their length (`export_text<&f>(x, out_len)`,
#             export_bytes): the test calls the source with a place for the length and compares the piece the two say -
#             written and compared (before: "the test has no values of unsigned int *").
#   running.cpp the logic the Blueprints run in as the engine names it in its own namespace (GameWorld::logic_runtime gives a
#             `logic::Runtime*`, kern::game::blueprints_running takes one): the test's stand-ins name it by its full name - written
#             and compared (before: "no type named 'Runtime' in namespace converted_...::logic", the stand-in of GameWorld went).
#   logicarg.cpp + logicarg_b.cpp  a function that takes the logic the Blueprints run in (kern::logic::Runtime&: no input, the
#             Blueprint's own) called from another file: what it takes is told the same there and here - written (before: the
#             converter stopped, "takes and gives ['I32', 'Bool'] there and ['Object<kern::logic::Runtime>', 'I32', 'Bool'] here").
#   placelib.cpp + placelib_b.cpp  a place of the engine (`auto& ev = w->events(); ev.clear();` - GameWorld::events is
#             KERN_PLACE: one node binds it, the nodes that read and clear it name that node, "bound") in a function of a
#             library, called from another file: linked into its caller (logic::link), the library's nodes have ids of the
#             caller's, and so has the node "bound" names - read, compared, its classes compile (before: "\"bound\" names no
#             node that binds a place of the engine (10)", the Blueprints not read - as a game's library with
#             RoadBuilder::chunks()).
#   views.cpp views of the program's own and the engine's: kern::Span<const f32> and const f32* (to read) are inputs by
#             value, handed an array of a fixed length, a list, a pointer into a list - both in one program; kern::Span<f32>
#             (to write through) is a reference input. A range-for over a view only reads: sum_of is a value.
#   hyperbolic.cpp std::sinh, cosh, tanh, asinh: the engine's own (kern/src/math/hyperbolic.c) - written and the same in both passes.
#   numbers_same.cpp, numbers_engine.cpp, numbers_f32.cpp, numbers_sum.cpp, numbers_i64.cpp  the second pass of the comparison (KERN_TEST_SOURCE_NUMBERS=1:
#             the source as C++ computes it): each case's tests are built and run (compare.sh, tools/source_numbers.py). numbers_same
#             (u32 that wraps, u64) is right; the other three are expected red until the number types of the language are the
#             subset's (f32 intermediate, f32 sum of 1/60 against a threshold, long long past 2^53) - one of them right is a fault of the list.
# Each case is a program of its own, its Blueprints read into a project and a level of their own, as a game's are:
# the engine's reader refuses what is wrong (a function read as a value that changes something, an export in a level).
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
here="$root/tools/converter_cases"
top="$(mktemp -d)"
[ -n "${KEEP:-}" ] && echo "($top)" >&2 || trap 'rm -rf "$top"' EXIT
. "$root/kern/tools/run_dir.sh"   # (the tool of blueprint_cpp.sh and the engine of compare.sh: built once in this run, kept in none)
fail=0
classes() {   # classes <case> <what>: the case's Blueprints as classes (tools/blueprint_cpp.sh --project), compiled
  # (of a copy of the level, written with format and version as a level is read - the case's own stays as the converter wrote it)
  python3 -c "import json,sys; d=json.load(open(sys.argv[1])); d.setdefault('format','kern-level'); d.setdefault('version',1); json.dump(d,open(sys.argv[2],'w'))" "$top/$1/cases.klevel" "$top/$1/classes.klevel"
  if "$root/tools/blueprint_cpp.sh" --project "$top/$1/cases.kproject" "$top/$1/classes.klevel" "$top/$1/classes.cpp" > "$top/$1/classes.log" 2>&1 &&
     "${CLANGXX:-clang++}" -std=c++20 -fsyntax-only -w -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" "$top/$1/classes.cpp" > "$top/$1/classes.err" 2>&1
  then echo "ok   $1: $2"; else echo "FAIL $1: $2: $(grep -m1 'error' "$top/$1/classes.log" "$top/$1/classes.err" | cut -c1-300)"; fail=1; fi
}
convert() {   # convert <case> [<file> ...]: the case's program (its files), converted with its test, into a project and a level of their own
  out="$top/${INTO:-}$1"
  mkdir -p "$out/src" "$out/texts" "$out/pt/converted"
  local f files=()
  for f in "$@"; do cp "$here/$f.h" "$here/$f.cpp" "$out/src/"; files+=("src/$f.cpp"); done
  # (a case with actors of its own in the level - Blueprints the program calls by an actor's name: <case>.klevel)
  if [ -f "$here/$1.klevel" ]; then cp "$here/$1.klevel" "$out/cases.klevel"; else echo '{"name":"Cases","nextId":1,"actors":[]}' > "$out/cases.klevel"; fi
  echo '{"format":"kern-project","version":1,"name":"Cases","levels":["cases.klevel"],"startLevel":"cases.klevel","blueprints":[]}' > "$out/cases.kproject"
  (cd "$out" && KERN_TEST_SOURCE_NUMBERS="${SOURCE_NUMBERS:-}" FUNCTIONS=1 python3 "$root/tools/cpp_to_blueprint.py" --convert cases --folder Cases -I"$root/kern/include" -I"$root/kern/third_party/box3d/include" -Isrc \
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
classes objects "the classes compile: a struct's field that is an Object is held as an Object (Held<Rig>), never as an Actor"

convert localobj
logged localobj "an object of the engine made as a local (kern::Earcut ec;) is written: an Object, never an Int" '^ +triangles \([0-9]+\): written'
check localobj "the local holds the Object its class's make gives" yes 'Object<kern::Earcut> ec'
logged localobj "it is compared" '^1 functions? of the Blueprints (is|are) compared'
classes localobj "the classes compile"

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
classes bytes "the classes compile: the glue hands the export the host's bytes as the list of its input (List<U8>)"

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

convert inner
check inner "the export inside the namespace is written" yes 'KERN_EXPORT_AS\("case_doubled"'

convert patch
logged patch "bright is written" '^ +bright \([0-9]+\): written$'
logged patch "it is compared" '^1 functions? of the Blueprints (is|are) compared'

convert chain chain_b
logged chain "step1 waits for root" '^ +Chain::step1 \([0-9]+\): it calls Chain::root\(\), which waits$'
logged chain "step2 waits for step1 (not on its test)" '^ +Chain::step2 \([0-9]+\): it calls Chain::step1\(\), which waits$'
logged chain "step3 waits for step2" '^ +Chain::step3 \([0-9]+\): it calls Chain::step2\(\), which waits$'
logged chain "step4 waits for step3 (not on its test)" '^ +Chain::step4 \([0-9]+\): it calls Chain::step3\(\), which waits$'
logged chain "free_of_it is written" '^ +Chain::free_of_it \([0-9]+\): written$'
logged chain "each round says why it is needed" '^round 2: 2 Blueprints - what Chain/chain_b_cpp, Chain/chain_cpp gave is new'
if grep -q '^round 4' "$top/chain/convert.log"; then echo "FAIL chain: all of the chain waits in one round (three rounds: the first, the chain, the one that finds nothing new)"; fail=1
else echo "ok   chain: all of the chain waits in one round (three rounds: the first, the chain, the one that finds nothing new)"; fi

convert pieces
logged pieces "case_label (export_text with out_len) is written" '^ +case_label \([0-9]+\): written$'
logged pieces "case_bytes (export_bytes with out_len) is written" '^ +case_bytes \([0-9]+\): written$'
logged pieces "all four are compared" '^4 functions of the Blueprints are compared'
tested pieces "the test hands the source a place for the length and compares the piece" 'return given_ \? std::string\(given_, given_ \+ out_len_\)'

convert logicarg logicarg_b
logged logicarg "both_ready (hands the logic on to a function of another file) is written" '^ +both_ready \([0-9]+\): written'
logged logicarg "both are compared" '^2 functions of the Blueprints are compared'

convert placelib placelib_b
logged placelib "take_events (names a place of the engine) is written" '^ +take_events \([0-9]+\): written'
logged placelib "events_twice (calls it from another file) is written" '^ +events_twice \([0-9]+\): written'
logged placelib "both are compared" '^2 functions of the Blueprints are compared'
classes placelib "the classes compile: the library's place, linked into its caller"

convert running
logged running "Runner::running (logic_runtime, blueprints_running) is written" '^ +Runner::running \([0-9]+\): written'
logged running "it is compared" '^1 functions? of the Blueprints (is|are) compared'

convert blasts
logged blasts "Viewer::blast (a list of LookState::Blast beside a Blast of the namespace) is written" '^ +Viewer::blast \([0-9]+\): written'
logged blasts "both are compared" '^2 functions of the Blueprints are compared'

convert far far_b far_rules
logged far "reach (calls far::pole_radius of another file, whose pieces need a header it does not include) is written" '^ +reach \([0-9]+\): written$'
logged far "all three are compared" '^3 functions of the Blueprints are compared'

convert worldrules worldmix
check worldrules "the Blueprint of an actor of a world (ActorBlueprint bound to a world) is a variable Actor of the program's Blueprint" yes '^  Actor worldrules_g_lamps = Actor\{\};'
check worldrules "bind(world): the Actor of the world's actor of its name (GameWorld actor named)" yes 'worldrules_g_lamps = actor_named;'
check worldrules "... of that world" yes 'actor_named = engine_game_world_actor_named\(world[_0-9]*, "Lamps"\);'
check worldrules "... and what it gives: Has Blueprint of it" yes 'return \{func_has_blueprint\(worldrules_g_lamps\)\};'
check worldrules "a call goes to that Actor (Call on Actor)" yes 'func_call_on<"Lamps_Twice">\(worldrules_g_lamps, '
check worldrules "a variable is read of it (Get Variable on Actor)" yes 'func_get_on<"PoleRadius", Float>\(worldrules_g_lamps\)'
check worldrules "no call names the actor by its name in the program's logic" no 'actor_find\("Lamps"\)'
logged worldrules "bind, radius, twice, Runner::start, Runner::pole and Runner::lit are written" '^Runner/worldrules_cpp +6 / +6 functions'
logged worldrules "one bound to a world and to a logic waits, with the reason" '^ +radius \([0-9]+\): the Blueprint of an actor \(g_both\) bound to a world and to a logic'
tested worldrules "the source's ActorBlueprint is bound to the test's world" 'worldrules::g_lamps\.bind\(bp\.source_world\(\)\);'
# (built and run: the source - its ActorBlueprint bound to the test's world - and the Blueprint, in the interpreter and as its class)
if INCLUDES="-I$top/worldrules/src" "$here/compare.sh" "$top/worldrules/cases.kproject" "$top/worldrules/cases.klevel" "$top/worldrules/pt/converted" "$top/worldrules/built" > "$top/worldrules/compare.build" 2>&1 &&
   ! grep -q "does not compile" "$top/worldrules/compare.build" && grep -qE '^ok +converted_Runner__worldrules_cpp' "$top/worldrules/built/compare.log" &&
   [ "$(grep -oE 'converted::Tally tally\("(bind|radius|twice|start|pole|lit)"\)' "$top/worldrules"/pt/converted/test_*.cpp | sort -u | wc -l)" = 6 ]
then echo "ok   worldrules: the tests compare the six, the source and the Blueprint in the interpreter and as its class"
else echo "FAIL worldrules: the tests compare the six: $(grep -m3 -E 'does not compile|FAIL|differ' "$top/worldrules/compare.build" "$top/worldrules/built/compare.log" 2>/dev/null | cut -c1-300)"; fail=1; fi

convert look lookapp
logged look "LookApp's own constructor (C++'s, not written down) is its function" '^ +LookApp::LookApp \([0-9]+\): '
if grep -q 'has no constructor of its own' "$top/look/convert.log"; then echo "FAIL look: nothing waits for a constructor written down"; fail=1; else echo "ok   look: nothing waits for a constructor written down"; fi
if grep -q '^ *Look::Look ' "$top/look/convert.log"; then echo "FAIL look: Look (nothing to make) has no constructor as a function"; fail=1; else echo "ok   look: Look (nothing to make) has no constructor as a function"; fi
logged look "Look::name is written" '^ +Look::name \([0-9]+\): written$'

# (the same case from a folder of another length: the same Blueprints, byte for byte - a declaration's identity is its own
# in the one tree of the compiler, never an address: kern/docs/blueprint_cpp.md)
INTO=a_folder_whose_name_is_much_longer_than_the_other_ones/ convert objects
if diff -r -q "$top/objects/texts" "$top/a_folder_whose_name_is_much_longer_than_the_other_ones/objects/texts" > /dev/null &&
   cmp -s "$top/objects/cases.klevel" "$top/a_folder_whose_name_is_much_longer_than_the_other_ones/objects/cases.klevel"
then echo "ok   objects: the same from a folder of another length, byte for byte"; else echo "FAIL objects: the same from a folder of another length, byte for byte"; fail=1; fi

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

# ---- the numbers of the source (kern/docs/blueprint_cpp.md "Zahlen"): the second pass of the comparison ----
# Converted with KERN_TEST_SOURCE_NUMBERS=1, a case has two tests: as always (the source with the numbers of the Blueprint
# language) and test_<x>_source.cpp (the source as C++ computes it - f32 stays f32). They are built and run (compare.sh);
# tools/source_numbers.py --right: every function written, compared in both passes, the same in both.
# numbers_same (u32 that wraps, u64): right today - and it must stay so.
right() {   # right <case>: the case's tests, built and run, say every function is written and the same in both passes
  INCLUDES="-I$top/$1/src" "$here/compare.sh" "$top/$1/cases.kproject" "$top/$1/cases.klevel" "$top/$1/pt/converted" "$top/$1/built" > "$top/$1/compare.build" 2>&1 ||
    { tail -20 "$top/$1/compare.build"; return 2; }
  if grep -q "does not compile" "$top/$1/compare.build"; then grep "does not compile" "$top/$1/compare.build"; return 2; fi
  python3 "$root/tools/source_numbers.py" "$top/$1/pt/converted" "$top/$1/built/compare.log" --right > "$top/$1/numbers.txt" &&
    grep -qE '^([0-9]+) of \1 functions of the program are written' "$top/$1/convert.log"   # (and none waits)
}
SOURCE_NUMBERS=1 convert numbers_same
logged numbers_same "both passes compare both functions" '^2 functions are compared once more with their source as C\+\+ computes it'
if right numbers_same; then echo "ok   numbers_same: written, and the same as the source in both passes (u32 that wraps, u64)"
else echo "FAIL numbers_same: written, and the same as the source in both passes"; cat "$top/numbers_same/numbers.txt" 2>/dev/null; fail=1; fi
# numbers_engine: the engine's own types (kern::Quat, physics::Pose, physics::BodyType) - in the second pass the test holds them as
# the engine has them, never a copy; right today, and it must stay so.
SOURCE_NUMBERS=1 convert numbers_engine
tested numbers_engine "the second pass holds the engine's own Quat and Pose, not copies" '^using Quat = ::kern::Quat;'
tested numbers_engine "the second pass holds the engine's own enum" 'using BodyType = :: kern :: physics :: BodyType;'
logged numbers_engine "both passes compare the three functions" '^3 functions are compared once more with their source as C\+\+ computes it'
if right numbers_engine; then echo "ok   numbers_engine: written, and the same as the source in both passes (the engine's own types)"
else echo "FAIL numbers_engine: written, and the same as the source in both passes"; cat "$top/numbers_engine/numbers.txt" 2>/dev/null; fail=1; fi
# hyperbolic: std::sinh, cosh, tanh, asinh (f64) - the engine's own functions (kern/src/math/hyperbolic.c): written
# (before: "std::sinh: the engine has no sinh yet"), and the same as the source in both passes.
SOURCE_NUMBERS=1 convert hyperbolic
check hyperbolic "std::sinh and std::cosh are the engine's" yes 'sinh\(.*cosh\('
check hyperbolic "std::asinh is the engine's" yes 'asinh\('
logged hyperbolic "all three are compared once more with their source" '^3 functions are compared once more with their source as C\+\+ computes it'
if right hyperbolic; then echo "ok   hyperbolic: written, and the same as the source in both passes (the engine's sinh, cosh, tanh, asinh)"
else echo "FAIL hyperbolic: written, and the same as the source in both passes"; cat "$top/hyperbolic/numbers.txt" 2>/dev/null; fail=1; fi
# Expected red until the number types of the language are the subset's (kern/docs/blueprint_cpp.md "Zahlen", Entwurf): what
# the Blueprint computes is not what C++ computes - each case says where. Such a case that is right is a fault of this list
# (red): the number types have come, and the case goes up to numbers_same.
#   numbers_f32  an intermediate in f32 (a*b): the Blueprint computes it in 64 bit
#   numbers_sum  an f32 sum of 1/60 against a threshold: past 1 after 60 frames in 64 bit, after 61 in f32
#   numbers_i64  a long long past 2^53 (a Blueprint's Int is exact up to 2^53: the function waits)
for c in numbers_f32 numbers_sum numbers_i64; do
  SOURCE_NUMBERS=1 convert $c
  if right $c; then echo "FAIL $c: expected red until the number types - but it is right: take it out of the list (to numbers_same)"; cat "$top/$c/numbers.txt"; fail=1
  elif [ $? = 2 ]; then echo "FAIL $c: its tests do not build"; fail=1
  else echo "ok   $c: expected red until the number types: $({ grep -m1 -P '^    \S+ \(\d+\): (?!written)' "$top/$c/convert.log" || grep -m1 -E 'differs|not compared' "$top/$c/numbers.txt"; } | sed 's/^ *//' | cut -c1-220)"; fi
done

python3 "$here/compiles.py" || fail=1
python3 "$here/libraries.py" || fail=1
python3 "$here/placement.py" || fail=1

if [ $fail != 0 ]; then
  for c in pad getter nodekind ownget frame makeworld objects actors bytes table nested meshextra views inner patch chain logicarg placelib running pieces blasts far worldrules look program; do echo "--- $c"; grep -v '^round' "$top/$c/convert.log" | tail -8; done
  exit 1
fi
