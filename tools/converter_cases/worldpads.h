#pragma once
#include "kern/game/runtime.h"
#include "kern/logic/actor_blueprint.h"
namespace cases::worldpads {
/// The Blueprint of one of the level's actors as a game names it (a class of its own made of the engine's).
struct Rules : kern::logic::ActorBlueprint {
  using ActorBlueprint::ActorBlueprint;
};
/// A value of the program whose method calls the Blueprint of the world's actor.
struct Axis {
  double v = 0;
  double step(double x);
};
/// The input maths of a game, in a file of its own: free functions that call the Blueprint of the actor Lamps of the world
/// the program's App runs (worldapp.cpp binds it: bind(world)). Functions of the Blueprint of the program's one object -
/// the App -, in the part of this file.
bool bind(kern::game::GameWorld& world);
double radius();
double twice(double x);
void both(double x, double& a, double& b);
/// ... and one that only calls those.
double wide(double x);
}
