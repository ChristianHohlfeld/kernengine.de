// A rule actor of the level as the tests run it (see blueprint_call.h for how the game calls one).
#pragma once
#include <string>

#include "../src/rules.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/logic.h"

namespace sc {

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
    kern::logic::Graph g;
    std::string error;
    if (!rules::actor_graph(actor, g, &error)) { problems = error; return; }
    for (const kern::logic::Issue& i : kern::logic::check(g)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
    rt.native = native;
    inst = rt.add(g, 2, actor);
    rt.begin();
    bind(inst);
  }
  ~RuleBlueprint() { bind(nullptr); }
};

}  // namespace sc
