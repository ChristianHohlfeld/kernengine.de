// A Blueprint of the project or of one of its levels as a test runs it (kern/test/converted.h compares one with the
// source it was converted from; a game's own tests run its Blueprints the same way). The Blueprints are those of the
// embedded project (kern/game/embedded.h - kern/tools/embed_project.py writes it): the program's in the project file,
// a world's in the level that holds it. The loop guard is the project's (settings "loopBudget").
#pragma once
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "kern/core/jobs.h"
#include "kern/core/json.h"
#include "kern/game/embedded.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

namespace kern::test {

/// The engine of the tests: the stand-in (it answers from the call itself), or - KERN_TEST_ENGINE=real - the engine
/// itself: every side a test compares calls the real functions (kern/logic/engine_host.h), each with objects
/// of its own, set up the same way for every call (none to begin with; a handle it does not know yet names an
/// object as C++ makes it without arguments - engine_make_default). The calls are written down as the
/// stand-in writes them, and compared the same way.
inline bool real_engine() {
  static const bool real = [] {
    const char* e = std::getenv("KERN_TEST_ENGINE");
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

}  // namespace kern::test

namespace kern::test {
/// The embedded project as a test reads it (null: it does not read - `why` says so), once for a test program.
inline const kern::game::Project* project(std::string* why = nullptr) {
  static const struct Read {
    kern::game::Project p;
    std::string why;
    bool ok;
    Read() : ok(kern::game::from_text(kern::game::embedded_project_text(), p, &why)) {}
  } read;
  if (!read.ok && why) *why = "the project: " + read.why;
  return read.ok ? &read.p : nullptr;
}
/// The loop guard of the logic a test runs Blueprints in (Runtime::budget): the project's (settings "loopBudget"),
/// as the program runs them (kern/game/program.h).
inline kern::u32 loop_budget() {
  const kern::game::Project* p = project();
  return p ? p->settings.loop_budget : kern::ProjectSettings{}.loop_budget;
}
/// The embedded level of this path (one of the project's "levels") as a document, read once by a test program (null:
/// it does not read, or the build embedded no text for it - `why` says so). A level may be large (tens of MB) and
/// a test asks for many of its Blueprints: it is read once.
inline const kern::json::Value* level_document(const std::string& path, std::string* why = nullptr) {
  static std::map<std::string, std::pair<std::unique_ptr<kern::json::Value>, std::string>, std::less<>> read;
  auto at = read.find(path);
  if (at == read.end()) {
    auto doc = std::make_unique<kern::json::Value>();
    std::string error;
    const std::string_view text = kern::game::embedded_level(path);
    if (text.empty()) {
      error = "the level " + path + ": the build embedded no text of it (kern/tools/embed_project.py)";
      doc.reset();
    } else if (!kern::json::parse(text, *doc, &error)) {
      error = "the level " + path + ": " + error;
      doc.reset();
    }
    at = read.emplace(path, std::make_pair(std::move(doc), std::move(error))).first;
  }
  if (!at->second.first && why) *why = at->second.second;
  return at->second.first.get();
}
/// The Blueprint called `name` - one of the project's (the program's) or that of an actor of that name in one of the
/// project's levels -, read from the embedded texts (false: none has one of that name, or two levels have one - `error`
/// says which). A name stands once, in the project or in a level (kern/docs/project.md 5.).
inline bool read_blueprint(std::string_view name, kern::logic::Graph& out, std::string& error) {
  std::string why;
  const kern::game::Project* p = project(&why);
  if (!p) {
    error = why;
    return false;
  }
  if (const kern::game::ProgramBlueprint* b = p->blueprint(name)) {
    out = b->logic;
    return true;
  }
  const kern::json::Value* found = nullptr;
  std::string found_in;
  for (const std::string& path : p->levels) {
    const kern::json::Value* doc = level_document(path, &why);
    if (!doc) {
      error = why;
      return false;
    }
    for (const kern::json::Value& a : (*doc)["actors"].items()) {
      if (a["name"].as_string() != name) continue;
      const kern::json::Value& graph = a["components"]["logic"];  // (an actor's Blueprint is one of its components)
      if (!graph.is_object()) break;
      if (found) {
        error = "the Blueprint \"" + std::string(name) + "\" stands in two levels of the project, " + found_in + " and " + path + ": a test names one Blueprint";
        return false;
      }
      found = &graph;
      found_in = path;
      break;
    }
  }
  if (found) return kern::logic::graph_from_json(*found, out, &error);
  std::string levels;
  for (const std::string& path : p->levels) levels += (levels.empty() ? "" : ", ") + path;
  error = "neither the project " + p->name + " nor its levels (" + (levels.empty() ? "none" : levels) + ") have a Blueprint \"" + std::string(name) + "\"";
  return false;
}
}  // namespace kern::test

namespace kern::test {

/// The Blueprint of this name (read_blueprint), read once for all the tests (null: there is none of that name, and
/// `error` says why).
inline const kern::logic::Graph* blueprint_of(std::string_view actor, std::string* error = nullptr) {
  static std::map<std::string, std::pair<std::unique_ptr<kern::logic::Graph>, std::string>, std::less<>> known;
  auto at = known.find(actor);
  if (at == known.end()) {
    auto held = std::make_unique<kern::logic::Graph>();
    std::string why;
    if (!read_blueprint(actor, *held, why)) held.reset();
    at = known.emplace(std::string(actor), std::make_pair(std::move(held), std::move(why))).first;
  }
  if (!at->second.first && error) *error = at->second.second;
  return at->second.first.get();
}

/// A logic in which no actor has a Blueprint: an ActorBlueprint bound to it answers with the zeros, as an unbound one
/// does (a test that is over binds its ActorBlueprints there - its own logic is gone with it).
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

/// A Blueprint (blueprint_of) running in a logic runtime of its own as the actor of its name, the C++ that calls it
/// bound to that - with other Blueprints beside it (`others`), each as the actor of its name: Find Actor by Name finds
/// them (the host's find_actor), and Call on Actor and Get Variable on Actor reach them as in the program. (`actor`
/// null: only those others.)
struct BlueprintRun {
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
      if (direct) return kern::logic::call_engine(objects, function, in, out);
      return test_engine(keeps ? *keeps : stand_in, keeps && keeps_objects ? *keeps_objects : objects, function, in, out);
    }
    /// The engine as the program has it (`BlueprintRun::Engine::direct`): the Blueprints' classes call the engine's
    /// functions on these objects themselves (kern/logic/engine_direct.h) - a test hands them its own (a physics
    /// world: `objects.adopt(world)` is its handle) - and the interpreter calls them the same way.
    bool direct = false;
    kern::logic::EngineObjects* engine_objects() override { return direct ? &objects : nullptr; }
    void engine_failed(std::string_view function) override {
      if (direct) error(std::string(function) + ": " + objects.error);
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
  /// How the program binds the C++ that calls it (null: nothing to bind), and what that gave (Has Blueprint,
  /// and whether its tables are as the program needs them).
  bool (*bind)(kern::logic::Runtime&) = nullptr;
  bool bound = false;
  /// The engine its Blueprints call: the test's (the stand-in, or KERN_TEST_ENGINE=real) or - `direct` - the engine
  /// itself on the objects the test gives it, as in the program (a test of the program's classes: what they do).
  enum class Engine { test, direct };
  BlueprintRun(const char* actor, bool (*binder)(kern::logic::Runtime&), bool native = true, const std::vector<std::string>& others = {},
                Engine engine = Engine::test)
      : bind(binder) {
    std::string error;
    host.direct = engine == Engine::direct;
    // The libraries a Blueprint uses are the other Blueprints of the project and its levels, by their names:
    // with them it is one graph, and the rules that follow calls judge that one.
    rt.blueprints = [](std::string_view name) { return blueprint_of(name); };
    rt.native = native;
    if (actor) {
      const kern::logic::Graph* held = blueprint_of(actor, &error);
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
      const kern::logic::Graph* g = blueprint_of(name, &error);
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
  ~BlueprintRun() {
    if (bind) bind(no_logic());
  }
};

}  // namespace kern::test
