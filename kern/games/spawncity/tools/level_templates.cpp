// The drone as the level's data: a template (kern/docs/runtime_actors.md) the game spawns (spiel.cpp: SpawnCity::start).
// Writes it as the level file has actors (kern::game::actor_to_json), one JSON array. tools/level_templates.sh builds
// this, runs it and puts the actor into level/spawncity.klevel. (Until 26.11.2026 also the player with its head, the
// jet, the car, the bus and the parachute: out of the small game - BLUEPRINTS.md, "Herausgenommen am 26.11.".)
//
// Its body and colliders are the ones of its rigid spec (Drone::rigid_spec); the level file keeps numbers to 1e-6 (7
// digits for inertia).
#include <cstdio>
#include <string>

#include "drone.h"
#include "kern/core/json.h"
#include "kern/game/level.h"

using namespace sc;
using namespace kern::game;

namespace {

Actor vehicle(const char* name, const RigidSpec& spec) {
  Actor a;
  a.name = name;
  a.folder = "Vehicles";
  a.is_template = true;
  BodyComponent b;
  b.motion = Motion::dynamic;
  b.mass = spec.mass.mass;
  b.center_of_mass = spec.mass.center;
  for (int i = 0; i < 6; ++i) b.inertia[i] = spec.mass.inertia[i];
  b.linear_damping = spec.linear_damping;
  b.angular_damping = spec.angular_damping;
  b.bullet = spec.bullet;
  b.allow_sleep = false;
  a.body = b;
  for (const ColliderSpec& c : spec.colliders) {
    ColliderComponent cc;
    switch (c.kind) {
      case kern::physics::ShapeKind::box: cc.shape = ColliderShape::box; break;
      case kern::physics::ShapeKind::sphere: cc.shape = ColliderShape::sphere; break;
      case kern::physics::ShapeKind::capsule: cc.shape = ColliderShape::capsule; break;
      default: cc.shape = ColliderShape::hull; break;
    }
    cc.center = c.center;
    cc.rotation = quat_to_euler(c.rotation);
    cc.size = c.size;
    cc.radius = c.radius;
    cc.length = c.length;
    cc.points = c.points;
    cc.friction = c.friction;
    cc.restitution = c.restitution;
    cc.density = 100;
    a.colliders.push_back(cc);
  }
  return a;
}

}  // namespace

int main() {
  Level l;
  // The drone: its body only (the game draws it) - a template the game spawns.
  l.add(vehicle("Drone", Drone::rigid_spec()));
  kern::json::Value out = kern::json::Value::array();
  for (const Actor& a : l.actors) out.push(actor_to_json(a));
  std::printf("%s\n", kern::json::stringify(out, 1).c_str());
  return 0;
}
