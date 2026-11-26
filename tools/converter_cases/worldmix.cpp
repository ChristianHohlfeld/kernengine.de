#include "worldmix.h"
#include "kern/logic/actor_blueprint.h"
namespace cases::worldmix {
namespace {
kern::logic::ActorBlueprint g_both{"Lamps"};
}
bool bind_world(kern::game::GameWorld& world) { return g_both.bind(world); }
bool bind_logic(kern::logic::Runtime& logic) { return g_both.bind(logic); }
double radius() { return g_both.get<double>("PoleRadius"); }
}
