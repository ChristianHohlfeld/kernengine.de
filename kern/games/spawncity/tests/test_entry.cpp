// The entry: where the game's fixed step runs (BLUEPRINTS.md, "5 - Umschalten"). The game gives its world a FixedStep
// (game.cpp take_fixed_step: FixedStepper, kern/game/runtime.h): the world runs it in every fixed step after the physics
// step and before the Blueprints' Event Tick - a place of the engine, not a node that has to stand first. So the level
// file holds no graph of its own, and no node of a type the engine does not know (the old node type of the fixed step,
// a node of an engine function that is gone): this test holds that.
#include <string>

#include "kern/game/level.h"
#include "kern/logic/logic.h"
#include "rules.h"
#include "kern/test/test.h"

KERN_TEST(entry_fixed_step_of_the_world) {
  kern::game::Level l;
  std::string error;
  KERN_CHECK(kern::game::from_text(sc::rules::level_text(), l, &error), "the level file reads: %s", error.c_str());
  KERN_CHECK(l.logic.empty(), "the level file has no graph of its own (the fixed step is the world's)");
  const kern::logic::Library& lib = kern::logic::library();
  std::size_t unknown = 0;
  std::string which;
  for (const kern::game::Actor& a : l.actors) {
    if (!a.logic) continue;
    for (const kern::logic::Node& n : a.logic->nodes)
      if (!lib.find(n.type)) {
        ++unknown;
        which = a.name + " (" + n.type + ")";
      }
  }
  KERN_CHECK(unknown == 0, "every node of the level's Blueprints is of a type the engine knows (%zu are not, e.g. %s)", unknown, which.c_str());
}
