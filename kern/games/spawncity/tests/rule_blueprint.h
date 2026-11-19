// A rule actor of the level as the tests run it (see blueprint_call.h for how the game calls one).
#pragma once
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../src/rules.h"
#include "kern/core/jobs.h"
#include "kern/core/json.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

namespace sc {

/// The engine of the tests: the stand-in (it answers from the call itself), or - SC_ENGINE=real - the engine
/// itself: every side a test compares calls the real functions (kern/logic/engine_host.h), each with objects
/// of its own, set up the same way for every call (none to begin with; a handle it does not know yet names an
/// object as C++ makes it without arguments - engine_make_default). The calls are written down as the
/// stand-in writes them, and compared the same way.
inline bool real_engine() {
  static const bool real = [] {
    const char* e = std::getenv("SC_ENGINE");
    return e && std::string_view(e) == "real";
  }();
  return real;
}
/// The engine of one side, as a call being compared begins with it: its objects - and what the engine holds of its
/// own beyond them, its jobs (kern/core/jobs.h): none, the next numbered 1, run where the program pumps them, so that
/// when a job runs is fixed by the calls alone. Every side begins the same (a job's number is no count of the sides
/// that ran before it).
inline void fresh_engine(kern::logic::EngineObjects& objects) {
  objects.clear();
  objects.on_missing(kern::logic::engine_make_default);
  kern::jobs::set_threads(false);
  kern::jobs::reset();
}
/// How many calls of the engine itself (real_engine) were refused since the call being compared began
/// (converted::Watch sets it to zero; a call of the source in which one was is not compared - Tally::take).
inline long g_engine_refused = 0;
inline std::string g_engine_refused_first;   // (the first call refused since the comparison began, as written down: said in the note)
/// The engine's clocks on the engine itself (real_engine): a function the table marks as one (EngineFunction::clock -
/// KERN_CLOCK, kern/core/base.h) gives what the time is when it is called, and the source is called before the
/// Blueprints. So a clock answers a Blueprint what it answered the source at the same place of the record of the
/// call being compared, where the Blueprint's call there is the source's (the same function, the same values -
/// the order of the calls is compared as for every call, Tally::take); anywhere else it is asked itself. `source`:
/// the record of the source (converted::Watch sets it, with the answers gone, as a call of the source begins).
struct Clocks {
  const kern::logic::EngineStandIn* source = nullptr;
  std::map<std::size_t, std::vector<kern::logic::Value>> given;   // (by the place of the call in the source's record)
};
inline Clocks g_clocks;
/// One call of the engine on one side: written down in `kept`, answered by the stand-in or the engine.
inline bool test_engine(kern::logic::EngineStandIn& kept, kern::logic::EngineObjects& objects, std::string_view function, const std::vector<kern::logic::Value>& in,
                        std::vector<kern::logic::Value>& out) {
  if (!real_engine()) return kept.engine(function, in, out);
  // (a place of the engine read or set - "<function>@get", "@set" - is no call of the source: not written down)
  if (function.ends_with("@get") || function.ends_with("@set")) return kern::logic::call_engine(objects, function, in, out);
  const kern::logic::EngineFunction* f = kern::logic::engine_function(function);
  if (!f) return false;
  kept.calls.push_back(kern::logic::engine_call_shown(function, in));
  const std::size_t at = kept.calls.size() - 1;
  if (f->clock && g_clocks.source && &kept != g_clocks.source) {
    const auto given = g_clocks.given.find(at);
    if (given != g_clocks.given.end() && at < g_clocks.source->calls.size() && g_clocks.source->calls[at] == kept.calls[at] && given->second.size() == out.size()) {
      out = given->second;
      return true;
    }
  }
  // A call the engine refuses (no object of its class at a handle, a list shorter than its count) gives
  // nothing: its record says so - a side on which it is refused and one on which it is not differ there.
  if (!kern::logic::call_engine(objects, function, in, out)) {
    if (g_engine_refused++ == 0) g_engine_refused_first = kept.calls.back() + (objects.error.empty() ? "" : " (" + objects.error + ")");
    kept.calls.back() += " - refused";
  } else if (f->clock && &kept == g_clocks.source) {
    g_clocks.given[at] = out;
  }
  return true;
}

}  // namespace sc

namespace sc::rules {
/// The Blueprint of the level's actor `actor`, read from the level file's text (false: no such actor, or it
/// has none - `error` says why). The game itself reads the level as a whole (Game::build_level); only the
/// tests ask for one actor's Blueprint.
inline bool actor_graph(std::string_view actor, kern::logic::Graph& out, std::string& error) {
  kern::json::Value doc;
  std::string why;
  if (!kern::json::parse(level_text(), doc, &why)) {
    error = "the level file: " + why;
    return false;
  }
  for (const kern::json::Value& a : doc["actors"].items()) {
    if (a["name"].as_string() != actor) continue;
    const kern::json::Value& graph = a["components"]["logic"];  // (an actor's Blueprint is one of its components)
    if (!graph.is_object()) break;
    return kern::logic::graph_from_json(graph, out, &error);
  }
  error = "the level has no actor \"" + std::string(actor) + "\" with a Blueprint";
  return false;
}
}  // namespace sc::rules

namespace sc {

/// The Blueprint of an actor of the level, read once for all the tests (null: the level has none of
/// that name, and `error` says so).
inline const kern::logic::Graph* level_blueprint(std::string_view actor, std::string* error = nullptr) {
  static std::map<std::string, std::pair<std::unique_ptr<kern::logic::Graph>, std::string>, std::less<>> known;
  auto at = known.find(actor);
  if (at == known.end()) {
    auto held = std::make_unique<kern::logic::Graph>();
    std::string why;
    if (!rules::actor_graph(actor, *held, why)) held.reset();
    at = known.emplace(std::string(actor), std::make_pair(std::move(held), std::move(why))).first;
  }
  if (!at->second.first && error) *error = at->second.second;
  return at->second.first.get();
}

/// A logic in which no actor has a Blueprint: rules bound to it answer with the zeros, as unbound ones do
/// (a test that is over binds its rules there - its own logic is gone with it).
inline kern::logic::Runtime& no_logic() {
  static struct None : kern::logic::Host {
    kern::f64 time() const override { return 0; }
    void print(const std::string&, bool) override {}
    void error(const std::string&) override {}
    kern::u32 random() override { return 4; }
  } host;
  static kern::logic::Runtime logic{host};
  return logic;
}

/// A rule actor of the level running in a logic runtime of its own, the C++ that calls it bound to that -
/// with the Blueprints of other actors of the level beside it (`others`), each by its name: Find Actor by
/// Name finds them (the host's find_actor), and Call on Actor and Get Variable on Actor reach them as in
/// the game. (`actor` null: only those others.)
struct RuleBlueprint {
  struct Host : kern::logic::Host {
    std::string errors;
    kern::f64 time() const override { return 0; }
    void print(const std::string&, bool) override {}
    void error(const std::string& text) override { errors += text + "\n"; }
    kern::u32 random() override { return 4; }
    /// The engine of a test: the stand-in, which answers by the outputs' forms and keeps every call - or the
    /// engine itself (real_engine), with the objects of this side.
    kern::logic::EngineStandIn stand_in;
    kern::logic::EngineObjects objects;
    /// (another that keeps the calls, where a test compares them with those of other code: else this one's -
    /// with the objects of that code)
    kern::logic::EngineStandIn* keeps = nullptr;
    kern::logic::EngineObjects* keeps_objects = nullptr;
    bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override {
      return test_engine(keeps ? *keeps : stand_in, keeps && keeps_objects ? *keeps_objects : objects, function, in, out);
    }
    /// The actors of this runtime by their names.
    std::map<std::string, kern::logic::ActorRef, std::less<>> actors;
    kern::logic::ActorRef find_actor(std::string_view name) override {
      const auto at = actors.find(name);
      return at == actors.end() ? 0 : at->second;
    }
  } host;
  kern::logic::Runtime rt{host};
  kern::logic::Instance* inst = nullptr;
  std::string problems;
  /// How the game binds the C++ that calls it (null: nothing to bind), and what that gave (Has Blueprint,
  /// and whether its tables are as the game needs them).
  bool (*bind)(kern::logic::Runtime&) = nullptr;
  bool bound = false;
  RuleBlueprint(const char* actor, bool (*binder)(kern::logic::Runtime&), bool native = true, const std::vector<std::string>& others = {}) : bind(binder) {
    std::string error;
    // The libraries a Blueprint uses are the Blueprints of the level's other actors, by their names:
    // with them it is one graph, and the rules that follow calls judge that one.
    rt.blueprints = [](std::string_view name) { return level_blueprint(name); };
    rt.native = native;
    if (actor) {
      const kern::logic::Graph* held = level_blueprint(actor, &error);
      if (!held) { problems = error; return; }
      const kern::logic::Graph& g = *held;
      kern::logic::Graph whole;
      if (!kern::logic::link(g, rt.blueprints, whole, &error)) { problems = error; return; }
      for (const kern::logic::Issue& i : kern::logic::check(whole)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
      inst = rt.add(g, 2, actor);
      host.actors.emplace(actor, 2);
    }
    kern::logic::ActorRef next = 3;
    for (const std::string& name : others) {
      if (host.actors.count(name)) continue;
      const kern::logic::Graph* g = level_blueprint(name, &error);
      kern::logic::Graph whole;
      if (!g || !kern::logic::link(*g, rt.blueprints, whole, &error)) { problems += name + ": " + error + "\n"; continue; }
      if (rt.add(*g, next, name)) host.actors.emplace(name, next);
      ++next;
    }
    rt.begin();
    if (bind) bound = bind(rt);
  }
  /// The running Blueprint of the actor of this name (null: none here).
  kern::logic::Instance* find(std::string_view name) {
    const kern::logic::ActorRef id = host.find_actor(name);
    return id ? rt.find(id) : nullptr;
  }
  ~RuleBlueprint() {
    if (bind) bind(no_logic());
  }
};

}  // namespace sc
