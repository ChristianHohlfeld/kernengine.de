// A rule actor of the level as the tests run it (see blueprint_call.h for how the game calls one).
#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "../src/rules.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/logic.h"

namespace sc {

/// The Blueprint of an actor of the level, read once for all the tests (null: the level has none of
/// that name, and `error` says so).
inline const kern::logic::Graph* level_blueprint(std::string_view actor, std::string* error = nullptr) {
  static std::map<std::string, std::pair<std::unique_ptr<kern::logic::Graph>, std::string>, std::less<>> known;
  auto at = known.find(actor);
  if (at == known.end()) {
    auto held = std::make_unique<kern::logic::Graph>();
    std::string why;
    if (!rules::actor_graph(actor, *held, &why)) held.reset();
    at = known.emplace(std::string(actor), std::make_pair(std::move(held), std::move(why))).first;
  }
  if (!at->second.first && error) *error = at->second.second;
  return at->second.first.get();
}

/// A rule actor of the level running in a logic runtime of its own, bound to the C++ that calls it.
struct RuleBlueprint {
  struct Host : kern::logic::Host {
    std::string errors;
    kern::f64 time() const override { return 0; }
    void print(const std::string&, bool) override {}
    void error(const std::string& text) override { errors += text + "\n"; }
    kern::u32 random() override { return 4; }
    /// The engine of a test: the stand-in, which answers by the outputs' forms and keeps every call.
    kern::logic::EngineStandIn stand_in;
    bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override { return stand_in.engine(function, in, out); }
  } host;
  kern::logic::Runtime rt{host};
  kern::logic::Instance* inst = nullptr;
  std::string problems;
  void (*bind)(kern::logic::Instance*) = nullptr;
  RuleBlueprint(const char* actor, void (*binder)(kern::logic::Instance*), bool native = true) : bind(binder) {
    std::string error;
    const kern::logic::Graph* held = level_blueprint(actor, &error);
    if (!held) { problems = error; return; }
    const kern::logic::Graph& g = *held;
    // The libraries a Blueprint uses are the Blueprints of the level's other actors, by their names:
    // with them it is one graph, and the rules that follow calls judge that one.
    rt.blueprints = [](std::string_view name) { return level_blueprint(name); };
    kern::logic::Graph whole;
    if (!kern::logic::link(g, rt.blueprints, whole, &error)) { problems = error; return; }
    for (const kern::logic::Issue& i : kern::logic::check(whole)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
    rt.native = native;
    inst = rt.add(g, 2, actor);
    rt.begin();
    bind(inst);
  }
  ~RuleBlueprint() { bind(nullptr); }
};

}  // namespace sc
