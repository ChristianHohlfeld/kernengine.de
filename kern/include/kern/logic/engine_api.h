// Kern Logic: the engine as nodes.
//
// What the engine's headers declare and do not define there is something only the engine can do:
// cast a ray through the physics world, draw a mesh, play a sound. Each such function is a node -
// kern::physics::World::cast_ray is engine.world.cast_ray, in a Blueprint's C++
// engine_world_cast_ray(world, origin, translation, filter, ignore):
//   - a const method and a function that only gives something is a value, everything else a step;
//   - what a function writes through a parameter is a further output;
//   - an object of the engine with a life of its own (a class: the physics world, the renderer,
//     the mixer) is a handle, a whole number the host gives - the first input of its nodes;
//   - a plain struct of the engine (Pose, BodySettings, RayHit) is a struct the Blueprint declares
//     with the same fields: the pin names it.
// The table is written from the headers by tools/engine_api.py (engine_api.gen.cpp), and so are
// the functions of a Blueprint's class (engine_nodes.gen.h).
//
// Every tier calls a function of the engine through one door: logic::Host::engine(function, in,
// out). The game binds the engine there; a test binds a stand-in (EngineStandIn).
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/logic/logic.h"

namespace kern::logic {

struct EnginePin {
  const char* id;     // the pin: the parameter's name in the engine's header ("result": what the function returns)
  PinType type;
  const char* of;     // a struct or an enum (or a list of them): its name
  const char* shape;  // the value's form, written out: n i b v t, e<values>, {fields}, [items]
};

struct EngineFunction {
  const char* type;      // the node: "engine.world.cast_ray"
  const char* cpp;       // the function: "kern::physics::World::cast_ray"
  const char* label;
  const char* category;
  const char* help;
  bool value;            // a value (it changes nothing): no exec pins
  std::vector<EnginePin> inputs, outputs;
};

/// Every function of the engine that is a node, in the order of the headers.
const std::vector<EngineFunction>& engine_functions();
/// The function of a node type, or null.
const EngineFunction* engine_function(std::string_view type);
/// The nodes of the table, in a library (Library::core has them).
void add_engine_nodes(Library& lib);

/// A stand-in for the engine, for tests: it answers every function of the table with values of
/// its outputs' forms - made from the function's name and its inputs, so the same call gets the
/// same answer in every tier - and keeps what it was asked, call by call.
struct EngineStandIn {
  std::vector<std::string> calls;  // "engine.world.pose(1, {2, 0})" ...
  bool engine(std::string_view function, const std::vector<Value>& in, std::vector<Value>& out);
};
/// A value as the stand-in writes it down: every number with all its digits.
std::string engine_shown(const Value& v);

}  // namespace kern::logic
