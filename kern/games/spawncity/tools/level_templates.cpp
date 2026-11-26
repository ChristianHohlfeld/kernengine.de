// The actors SpawnCity's code built into its level at every start (until 26.11.2026: Game::build_level), as the level's
// data: the player with its head, and the vehicles as templates (kern/docs/runtime_actors.md) - Game spawns them
// (game.cpp: Game::start). Writes them as the level file has actors (kern::game::actor_to_json), one JSON array.
// tools/level_templates.sh builds this, runs it and puts the actors into level/spawncity.klevel.
//
// The vehicles' bodies and colliders are the ones of their rigid specs (Drone::rigid_spec, Jet::rigid_spec,
// Car::rigid_spec); the level file keeps numbers to 1e-6 (7 digits for inertia).
#include <cstdio>
#include <string>

#include "car.h"
#include "drone.h"
#include "jet.h"
#include "kern/core/json.h"
#include "kern/game/level.h"

using namespace sc;
using namespace kern::game;

namespace {

constexpr f32 kPlayerHealth = 100 * 1.18f;   // player_vitals_runtime.mjs: +18 % tougher

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
  // The player: Kern's soldier with SpawnCity's on-foot values (walk 5.5, sprint 8.1, jump 3.6 m/s, eye 1.68 m, 90
  // degrees horizontal field of view), starting with the pistol - where the player starts (its spawn point).
  Actor s;
  s.name = "Player";
  s.transform.position = {0, 0, 0.725f};
  MeshComponent m;
  m.shape = Shape::capsule;
  m.radius = 0.35f;
  m.height = 1.45f;
  m.material.color = {0.32f, 0.42f, 0.3f};
  s.mesh = m;
  BodyComponent b;
  b.motion = Motion::dynamic;
  b.mass = 80;
  b.allow_sleep = false;
  b.lock_angular[0] = b.lock_angular[1] = true;  // stands upright while not possessed (flying the drone)
  s.body = b;
  ControllerComponent c;
  c.mode = ControlMode::soldier;
  c.camera = CameraMode::first_person;
  c.max_speed = 5.5f;
  c.boost_speed = 8.1f;
  c.jump_speed = 3.6f;
  c.eye_height = 1.68f;
  c.camera_fov = 59;  // 90 degrees across a 16:9 view (camera_pitch_contract.mjs)
  s.controller = c;
  s.weapon = weapon_preset(WeaponKind::pistol);
  s.weapon->spread = s.weapon->aim_spread = s.weapon->recoil = 0;  // the original's rounds: no cone (select_foot_weapon)
  HealthComponent h;
  h.max_health = kPlayerHealth;
  h.regen_delay = 0;  // player_vitals_runtime.mjs: no regeneration (only a RESET or a respawn heals)
  h.respawn = true;
  s.health = h;
  const ActorId sid = l.add(s).id;
  Actor head;
  head.name = "Head";
  head.parent = sid;
  head.transform.position = {0, 0, 0.83f};
  MeshComponent hm;
  hm.shape = Shape::sphere;
  hm.radius = 0.17f;
  head.mesh = hm;
  head.tags.push_back("head");
  l.add(head);
  // The vehicles: bodies only (the game draws them) - templates Game spawns (the police cruisers are Cars).
  l.add(vehicle("Drone", Drone::rigid_spec()));
  l.add(vehicle("Jet", Jet::rigid_spec()));
  l.add(vehicle("Car", Car::rigid_spec(CarSpec::car())));
  l.add(vehicle("Bus", Car::rigid_spec(CarSpec::bus())));
  {
    // Parachute (pilot + canopy): an upright capsule body carried by the canopy's drag.
    RigidSpec rs;
    ColliderSpec cap;
    cap.kind = kern::physics::ShapeKind::capsule;
    cap.radius = 0.3f;
    cap.length = 1.1f;
    rs.colliders.push_back(cap);
    rs.mass.mass = 90;
    box_inertia(90, {0.6f, 0.6f, 1.7f}, rs.mass);
    Actor a = vehicle("Parachute", rs);
    a.body->lock_angular[0] = a.body->lock_angular[1] = a.body->lock_angular[2] = true;
    l.add(a);
  }
  kern::json::Value out = kern::json::Value::array();
  for (const Actor& a : l.actors) out.push(actor_to_json(a));
  std::printf("%s\n", kern::json::stringify(out, 1).c_str());
  return 0;
}
