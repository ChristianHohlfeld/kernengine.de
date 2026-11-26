#pragma once
#include "kern/logic/actor_blueprint.h"
namespace cases::rules {
/// The program's way to call a Blueprint of the level (as a game has it in a header of its own): a class whose base
/// is a class of the engine (kern::logic::ActorBlueprint). The file that calls far::pole_radius() does not include this
/// header - only far_b.cpp does.
struct Blueprint : kern::logic::ActorBlueprint {
  using ActorBlueprint::ActorBlueprint;
};
int rules_count();
}
