// Kern Logic: the engine as nodes.
//
// What the engine's headers declare and do not define there is something only the engine can do:
// cast a ray through the physics world, draw a mesh, play a sound. Each such function is a node -
// kern::physics::World::cast_ray is engine.world.cast_ray, in a Blueprint's C++
// engine_world_cast_ray(world, origin, translation, filter, ignore):
//   - a const method and a function that only gives something is a value, everything else a step
//     (a function handed an object of the engine to change is a step);
//   - what a function writes through a parameter is a further output, and so is what it reports
//     through a pointer that is null by default (`bool* changed = nullptr`), and what it writes
//     through a pointer to numbers with their count beside it (`void render(f32* out, Count<2> frames)`:
//     the output list `out`, its count an input - EnginePin::count; the count says in its type that it
//     counts what the pointer shows, kern::Count<Per>, Per numbers each: 2 * frames numbers -
//     EnginePin::per). The same for a pointer the engine reads (`push_incoming(from, const u8* data,
//     Count<1> bytes)`). A whole number of another type beside a pointer says nothing about its length:
//     tools/engine_api.py stops on it (no length is guessed from a name);
//   - an object of the engine with a life of its own (a class: the physics world, the renderer,
//     the mixer; a struct that cannot be copied or holds such an object) is a handle, a whole
//     number the host gives - the first input of its nodes, which are all its public methods;
//   - a plain struct of the engine (Pose, BodySettings, RayHit) is a struct the Blueprint declares
//     with the same fields: the pin names it.
// The table is written from the headers by tools/engine_api.py (engine_api.gen.cpp), and so are
// the functions of a Blueprint's class (engine_nodes.gen.h).
//
// The interpreter and the JIT call a function of the engine through one door: logic::Host::engine(
// function, in, out). The game binds the engine there (kern/logic/engine_host.h: call_engine, the real
// function of every node); a test binds a stand-in (EngineStandIn) or the engine itself. A Blueprint's
// class - the exported game - calls the function itself, typed, where its host gives it the engine's
// objects (kern/logic/engine_direct.h), and through the same door where it does not (a test with a
// stand-in). Both do the same: the tool writes both from this table.
#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "kern/logic/logic.h"

namespace kern::logic {

struct EnginePin {
  const char* id;     // the pin: the parameter's name in the engine's header ("result": what the function returns)
  PinType type;
  const char* of;     // a struct or an enum (or a list of them): its name
  const char* shape;  // the value's form, written out: n i b v t, l (Int64), h (a handle of an object made - never 0), e<values>, {fields}, [items], [N items] (N entries: `f32 m[16]`), [?item] (no entry or one: std::optional)
  /// An output list the engine writes through a pointer, with their count beside it (`void render(f32*
  /// out, Count<2> frames)`): the input that is the count - the room the call says it has ("" for any other pin).
  const char* count = "";
  /// How many numbers each one of that count is (`void render(f32* out, Count<2> frames)`: 2 - the
  /// engine writes 2 * frames numbers there).
  u32 per = 1;
};

struct EngineFunction {
  const char* type;      // the node: "engine.world.cast_ray"
  const char* cpp;       // the function: "kern::physics::World::cast_ray"
  const char* label;
  const char* category;
  const char* help;
  bool value;            // a value (it changes nothing): no exec pins
  std::vector<EnginePin> inputs, outputs;
  /// A place of the engine: the function gives a reference (not const) to something its object holds, and
  /// only looks it up (`std::vector<ShotEvent>& shots() { return shots_; }`). The node binds it - its output
  /// "result" is that place - and the nodes that name a place work on it there (kern/docs/blueprint_cpp.md,
  /// "Plätze der Engine").
  bool place = false;
  /// It reads a clock (KERN_CLOCK, kern/core/base.h: now_ms, wall_ms): what it gives changes by itself - two
  /// plays of the same program on the engine itself do not get the same from it (kern/tests/native_show.h).
  bool clock = false;
  /// A field of an object of the engine (`Mesh::visible`, `TextureDesc::width`): a place as above, but no function
  /// is called - the node binds it without asking the engine anything; reading and setting it is "<type>@get" /
  /// "<type>@set" (kern/docs/blueprint_cpp.md, "Felder eines Objekts der Engine").
  bool field = false;
};

/// Every function of the engine that is a node, in the order of the headers.
const std::vector<EngineFunction>& engine_functions();
/// The function of a node type, or null.
const EngineFunction* engine_function(std::string_view type);

/// An interface of the engine (kern/docs/blueprint_cpp.md, "Schnittstellen der Engine"): a class of the engine whose
/// functions are virtual - kern::App, kern::ShaderExtension. The engine calls an object of it; a Blueprint can be that
/// object (Graph::implements names the interface): its functions of the names of the interface's are what the engine
/// calls. Each virtual function is the node of the table that calls it (EngineVirtual::node): the Blueprint's function
/// has that node's inputs after the first (the object: the Blueprint itself) and its outputs, in their order and of
/// their types. One that is pure the Blueprint must have; one that is not it may have - else the interface's own runs.
struct EngineVirtual {
  const char* name;  // the function: "vertex"
  const char* node;  // its node: "engine.shader_extension.vertex"
  bool pure;
  /// Why the engine cannot hand this one to a Blueprint ("is handed a pointer"; "": it can) - a Blueprint has no
  /// function of its name, the interface's own runs.
  const char* waits = "";
};
struct EngineInterface {
  const char* name;  // as a Blueprint names it: "ShaderExtension", "net_Transport"
  const char* cpp;   // "kern::ShaderExtension"
  std::vector<EngineVirtual> functions;
  /// Why no Blueprint implements it in this build ("" where one can): a function of it whose pins the engine
  /// cannot hand to a Blueprint and take back (a pointer the engine writes through) - tools/engine_api.py says which.
  const char* waits = "";
};
const std::vector<EngineInterface>& engine_interfaces();
const EngineInterface* engine_interface(std::string_view name);
/// The nodes of the table, in a library (Library::core has them).
void add_engine_nodes(Library& lib);

/// A stand-in for the engine, for tests: it answers every function of the table with values of
/// its outputs' forms - made from the function's name and its inputs, so the same call gets the
/// same answer in every tier - and keeps what it was asked, call by call. An output list with a
/// count (EnginePin::count) has no more entries than that input says (times EnginePin::per).
/// A place of the engine (EngineFunction::place) is a state of the stand-in, per function and inputs: what
/// "<function>@get" gives there is what "<function>@set" set there last (at first a value of the output's
/// form, made from the call) - and neither is a call it keeps: the function was called once, when the place
/// was bound; what is read and set there after is the engine's memory, as in C++.
struct EngineStandIn {
  std::vector<std::string> calls;  // "engine.world.pose(1, {2, 0})" ...
  std::map<std::string, Value> places;  // "engine.game_world.shots(1)" -> what is there
  bool engine(std::string_view function, const std::vector<Value>& in, std::vector<Value>& out);
  /// The Callbacks a call hands it are called - three times each, with inputs drawn from the call - and what they
  /// give is written down with the call and goes into its answer. Handed to an object of the engine (the call's first
  /// input is its handle), they stay with it: every later call on that object calls them again, until it is released.
  struct Kept {
    std::string shape;  // (its form: cn(nn))
    Value callback;
  };
  std::map<i64, std::map<std::string, Kept>> kept;
};
/// A value as the stand-in writes it down: every number with all its digits.
std::string engine_shown(const Value& v);
/// A call as the stand-in writes it down - "engine.world.pose(1, {2, 0})": what a test compares between
/// two sides that call the engine (the stand-in, or the engine itself).
std::string engine_call_shown(std::string_view function, const std::vector<Value>& in);

}  // namespace kern::logic
