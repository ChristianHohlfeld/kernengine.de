// How the native Blueprint test writes a value down: the interpreter's side (kern_tests
// --emit-native) and the compiled side (blueprint_native_test) use this one function, and the
// two texts must be the same, character by character.
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "kern/logic/engine_api.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

inline std::string native_show(const kern::logic::Value& v) {
  using kern::logic::PinType;
  char b[160];
  auto number = [&](double x, int digits) -> std::string {
    if (std::isnan(x)) return "nan";  // (whatever its sign)
    std::snprintf(b, sizeof b, "%.*g", digits, x);
    return b;
  };
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer: std::snprintf(b, sizeof b, "%lld", static_cast<long long>(v.i)); return b;
    case PinType::number: return number(v.n, 17);
    case PinType::vector: return "(" + number(v.v.x, 9) + ", " + number(v.v.y, 9) + ", " + number(v.v.z, 9) + ")";
    case PinType::text: return "\"" + v.s + "\"";
    case PinType::actor: std::snprintf(b, sizeof b, "#%llu", static_cast<unsigned long long>(v.a)); return b;
    case PinType::enumeration: std::snprintf(b, sizeof b, "~%lld", static_cast<long long>(v.i)); return b;  // (its place among its enum's values)
    case PinType::structure: {  // its fields, each as above
      std::string r = "{";
      for (std::size_t k = 0, n = v.fields(); k < n; ++k) r += (k ? ", " : "") + native_show(*v.field(k));
      return r + "}";
    }
    default: break;
  }
  if (!kern::logic::is_list(v.type)) return "?";
  std::string r = "[";  // a list: its items, each as above
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + native_show(v.item(k));
  return r + "]";
}

/// The host of the native Blueprint test: time stands still, errors are kept; its random numbers
/// are one fixed sequence (a Blueprint draws its seed from it). Its engine is the stand-in: every
/// function answers by its outputs' forms, the same in every tier.
struct NativeTestHost : kern::logic::Host {
  std::vector<std::string> errors;
  kern::logic::EngineStandIn stand_in;
  bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override { return stand_in.engine(function, in, out); }
  kern::f64 time() const override { return 0; }
  void print(const std::string&, bool) override {}
  void error(const std::string& text) override { errors.push_back(text); }
  kern::u32 state = 2463534242u;
  kern::u32 random() override {
    state = state * 1664525u + 1013904223u;
    return state;
  }
};

/// The host of a case played on the engine itself: its engine is the engine - the real function of every
/// node (kern/logic/engine_host.h), on objects of its own. `direct`: it gives a Blueprint's class these
/// objects, and the class calls the engine's functions on them itself (kern/logic/engine_direct.h); else
/// every call comes through its door. It counts the calls that came through the door and keeps what it
/// was told of calls that failed, in their order.
struct NativeEngineHost : NativeTestHost {
  kern::logic::EngineObjects objects;
  bool direct = false;
  long door = 0;
  std::string told;
  bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override {
    ++door;
    if (kern::logic::call_engine(objects, function, in, out)) return true;
    engine_failed(function);
    return false;
  }
  kern::logic::EngineObjects* engine_objects() override { return direct ? &objects : nullptr; }
  void engine_failed(std::string_view function) override { told += " !" + std::string(function) + ": " + objects.error; }
};

/// The functions `runs` of a Blueprint (no inputs), called on the engine itself: after each, a row - the run
/// with what the host was told of calls that failed, then every variable of the Blueprint (`name=value`).
/// `as_class`: the Blueprint runs as its compiled class, which calls the engine's functions itself - else in
/// the interpreter, through the host's door. `problem`: it did not run that way.
inline std::vector<std::vector<std::string>> native_rows_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, bool as_class, std::string& problem) {
  NativeEngineHost host;
  host.direct = as_class;
  kern::logic::Runtime rt(host);
  rt.jit = false;
  rt.native = as_class;
  kern::logic::Instance* inst = rt.add(graph, 0, name);
  rt.begin();
  if ((std::string(inst->tier()) == "native C++") != as_class) problem = name + " runs in the " + inst->tier();
  std::vector<std::vector<std::string>> rows;
  for (const std::string& run : runs) {
    std::vector<kern::logic::Value> results;
    host.told.clear();
    std::vector<std::string>& row = rows.emplace_back();
    row.push_back(run + (inst->call(run, {}, results) ? " ->" : " (no call) ->") + host.told);
    for (const kern::logic::Variable& v : graph.variables) row.push_back(v.name + "=" + native_show(inst->get(v.name)));
  }
  if (problem.empty() && as_class && host.door != 0) problem = "the class of " + name + " called the engine through the host's door " + std::to_string(host.door) + " times";
  if (problem.empty() && !as_class && host.door == 0) problem = "the interpreter did not call the engine through the host's door";
  return rows;
}

/// The same as one text: a line per run. The interpreter's and the class's are the same, character by character.
inline std::string native_play_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, bool as_class, std::string& problem) {
  std::string text = "== " + name + " on the engine itself\n";
  for (const std::vector<std::string>& row : native_rows_on_engine(graph, name, runs, as_class, problem)) {
    text += row[0] + " |";
    for (std::size_t k = 1; k < row.size(); ++k) text += " " + row[k];
    text += "\n";
  }
  return text;
}

/// The functions `runs` played on the engine itself three times in one process: in the interpreter through
/// the host's door, as the compiled class that calls the engine's functions itself, and through the door
/// again. What the door gives the same both times - what the host was told after a run, each variable after
/// it - the class gives the same, character by character; each difference is a line of what this returns.
/// What the door does not give the same twice depends on more than the Blueprint: on the clock, or on what
/// the engine keeps between calls (the number of the next job). It cannot be compared and is named in
/// `unrepeatable` (each once). (The class plays between the two: what only ever grows - the clock, a
/// count - and is the same both times through the door was the same for the class.)
inline std::string native_compare_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, std::string& unrepeatable, std::string& problem) {
  const auto door = native_rows_on_engine(graph, name, runs, false, problem);
  const auto itself = native_rows_on_engine(graph, name, runs, true, problem);
  const auto again = native_rows_on_engine(graph, name, runs, false, problem);
  std::string differs;
  if (door.size() != itself.size() || door.size() != again.size()) return "the three plays have not the same runs\n";
  std::vector<bool> named(door.empty() ? 0 : door[0].size(), false);
  for (std::size_t r = 0; r < door.size(); ++r) {
    if (door[r].size() != itself[r].size() || door[r].size() != again[r].size() || door[r].size() != named.size()) return "the three plays have not the same variables\n";
    for (std::size_t k = 0; k < door[r].size(); ++k) {
      if (door[r][k] != again[r][k]) {   // (named once: a variable by its name, what the host was told by the run)
        if (k == 0 || !named[k]) unrepeatable += " " + (k == 0 ? runs[r] + "(told)" : door[r][k].substr(0, door[r][k].find('=')));
        named[k] = true;
      } else if (itself[r][k] != door[r][k]) {
        differs += "after " + runs[r] + ": " + itself[r][k] + "\n     through the door: " + door[r][k] + "\n";
      }
    }
  }
  return differs;
}
