// The entry: where the drone's fixed step runs. The game takes the world's fixed steps itself (spiel.cpp SpawnCity::fly:
// GameWorld::step_once, then the drone's force model) - no node that has to stand first. So the level file holds no
// graph of its own, and no node of a type the engine does not know (the old node type of the fixed step, a node of an
// engine function that is gone): this test holds that.
#include <string>

#include "kern/game/embedded.h"
#include "kern/game/level.h"
#include "kern/game/project.h"
#include "kern/logic/logic.h"
#include "kern/test/test.h"

KERN_TEST(entry_fixed_step_of_the_world) {
  kern::game::Level l;
  std::string error;
  KERN_CHECK(kern::game::from_text(kern::game::embedded_level("spawncity.klevel"), l, &error), "the level file reads: %s", error.c_str());
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

// The drone is the level's data (tools/level_templates.cpp wrote it; kern/docs/runtime_actors.md): a template the game
// spawns, no player (the world is simulated: the drone flies in it). Its body and colliders are those of the drone's
// rigid spec, to the level file's precision (1e-6; inertia 7 digits) - a spec that changes without its template fails
// here (tools/level_templates.sh writes it anew).
#include "drone.h"

KERN_TEST(entry_vehicle_templates_are_the_rigid_specs) {
  kern::game::Level l;
  std::string error;
  KERN_CHECK(kern::game::from_text(kern::game::embedded_level("spawncity.klevel"), l, &error), "the level file reads: %s", error.c_str());
  auto find = [&](const char* name) -> const kern::game::Actor* {
    for (const kern::game::Actor& a : l.actors)
      if (a.name == name) return &a;
    return nullptr;
  };
  KERN_CHECK(!find("Player") && kern::game::default_player(l) == 0, "the level has no player (the drone flies in a simulated world)");
  auto near = [](kern::f32 a, kern::f32 b, kern::f32 rel) { return std::fabs(a - b) <= 1e-6f + rel * std::fabs(b); };
  auto same = [&](const char* name, const sc::RigidSpec& spec) {
    const kern::game::Actor* a = find(name);
    KERN_CHECK(a && a->is_template && a->parent == 0, "the level has the template %s", name);
    if (!a || !a->body) return;
    bool ok = a->body->motion == kern::game::Motion::dynamic && near(a->body->mass, spec.mass.mass, 1e-6f) && a->body->bullet == spec.bullet &&
              !a->body->allow_sleep && near(a->body->linear_damping, spec.linear_damping, 0) && near(a->body->angular_damping, spec.angular_damping, 0) &&
              a->colliders.size() == spec.colliders.size();
    for (int i = 0; i < 3; ++i) ok = ok && near(a->body->center_of_mass[i], spec.mass.center[i], 0);
    for (int i = 0; i < 6; ++i) ok = ok && near(a->body->inertia[i], spec.mass.inertia[i], 1e-6f);
    for (std::size_t k = 0; ok && k < spec.colliders.size(); ++k) {
      const kern::game::ColliderComponent& c = a->colliders[k];
      const sc::ColliderSpec& s = spec.colliders[k];
      const kern::Vec3 rot = kern::game::quat_to_euler(s.rotation);
      ok = ok && near(c.radius, s.radius, 0) && near(c.length, s.length, 0) && near(c.friction, s.friction, 0) && near(c.restitution, s.restitution, 0) &&
           c.points.size() == s.points.size();
      for (int i = 0; i < 3; ++i) ok = ok && near(c.center[i], s.center[i], 0) && near(c.size[i], s.size[i], 0) && std::fabs(c.rotation[i] - rot[i]) < 1e-4f;
      for (std::size_t p = 0; ok && p < s.points.size(); ++p)
        for (int i = 0; i < 3; ++i) ok = ok && near(c.points[p][i], s.points[p][i], 0);
    }
    KERN_CHECK(ok, "the template %s has the body and the colliders of its rigid spec", name);
  };
  same("Drone", sc::Drone::rigid_spec());
  // What the game's code set on the level at every start is the level's and the project's (tools/level_world.py): the
  // 240 Hz step the vehicles' force models run at, one Box3D sub-step, the shadow over 110 m - on a phone 70 m, the
  // device profile "mobile" (kern/docs/project.md 2.).
  KERN_CHECK(l.world.step_hz == 240 && l.world.substeps == 1 && l.world.shadow_distance == 110, "the level's world steps at 240 Hz, one sub-step, shadow 110 m");
  kern::game::Project project;
  KERN_CHECK(kern::game::from_text(kern::game::embedded_project_text(), project, &error), "the project reads: %s", error.c_str());
  const kern::DeviceProfile* mobile = project.settings.profile("mobile");
  kern::game::WorldSettings phone = l.world;
  KERN_CHECK(mobile && kern::game::overlay_world(mobile->world, phone, &error) && phone.shadow_distance == 70 && phone.step_hz == 240,
           "the profile mobile has the phone's shadow (70 m), nothing else of the world: %s", error.c_str());
  for (const char* gone : {"Jet", "Car", "Bus", "Parachute"}) KERN_CHECK(!find(gone), "the level has no template %s (out of the small game)", gone);
}
