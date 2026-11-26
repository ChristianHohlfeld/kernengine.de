#pragma once
#include "kern/game/runtime.h"
#include "kern/logic/logic.h"
namespace cases::worldmix {
/// One ActorBlueprint bound to a world and to a logic: a Blueprint holds the Actor of a world, or names the actor where
/// its logic runs - not both. It waits, with the reason.
bool bind_world(kern::game::GameWorld& world);
bool bind_logic(kern::logic::Runtime& logic);
double radius();
}
