// The entry: where the game's fixed step runs in the level's logic (BLUEPRINTS.md, "5 - Umschalten").
// The game registers a type of node that an object is (game.cpp register_node: FixedStepNode, a kern::logic::NodeType,
// "spawncity.fixed_step", event logic.tick) and build_level adds one node of it to the level's graph of the world. Runtime::fire
// runs a tick's handlers instance by instance - the level's graph first, then the actors in their order - and within one by
// node id: so fixed_step runs first in every tick. It stays so while the level file has no graph of its own and no actor
// before Game handles logic.tick: this test holds that (also when Game's Blueprint lives in the program's logic runtime,
// not in the world - the node stays in the world's level graph).
#include <string>

#include "kern/game/level.h"
#include "kern/logic/logic.h"
#include "rules.h"
#include "test_util.h"

SC_TEST(entry_fixed_step_first) {
  kern::game::Level l;
  std::string error;
  SC_CHECK(kern::game::from_text(sc::rules::level_text(), l, &error), "the level file reads: %s", error.c_str());
  // (the level's own graph is the game's: build_level puts the one node there - the file has none)
  SC_CHECK(l.logic.empty(), "the level file has no graph of its own (build_level adds the fixed step's node)");
  const kern::logic::Library& lib = kern::logic::library();
  std::size_t game = l.actors.size(), before = 0;
  for (std::size_t i = 0; i < l.actors.size(); ++i)
    if (l.actors[i].name == "Game") { game = i; break; }
  SC_CHECK(game < l.actors.size(), "the level has the Blueprint Game");
  std::string ticking;
  for (std::size_t i = 0; i < game && i < l.actors.size(); ++i) {
    const kern::game::Actor& a = l.actors[i];
    if (!a.logic) continue;
    for (const kern::logic::Node& n : a.logic->nodes) {
      const kern::logic::NodeDef* d = lib.find(n.type);
      if (d && d->event == "logic.tick") {
        ++before;
        ticking = a.name + " (" + n.type + ")";
      }
    }
  }
  SC_CHECK(before == 0, "no actor before Game handles logic.tick (%zu, e.g. %s): its Event Tick runs where the level's node runs today",
           before, ticking.c_str());
}
