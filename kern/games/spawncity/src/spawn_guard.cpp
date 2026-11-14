// The game's calls into the Blueprint of the actor "SpawnGuard" (see spawn_guard.h): no rule lives here.
#include "spawn_guard.h"

#include "blueprint_call.h"

namespace sc::spawn {

namespace {
rules::Blueprint g_rules{"SpawnGuard"};

/// The Blueprint's struct Viewer, as it takes it.
struct Looker {
  Vec3 eye, forward;
  f64 half_angle = 0, far = 0;
  bool local = false;
};
Looker looker_of(const Viewer& v) { return {v.eye, v.forward, rules::decimal(v.half_angle), rules::decimal(v.far), v.local}; }
/// The Blueprint's struct Footprint, as it takes it.
struct Print {
  Vec3 center;
  f64 yaw = 0;
  Vec3 half;
};
Print print_of(const Footprint& f) { return {Vec3{f.center.x, f.center.y, 0}, rules::decimal(f.yaw), Vec3{f.half.x, f.half.y, 0}}; }
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

f32 default_far() { return static_cast<f32>(g_rules.get<f64>("DefaultFar")); }

f32 cone_half_angle(f32 vertical_fov, f32 aspect) { return static_cast<f32>(g_rules.number("ConeHalfAngle", rules::decimal(vertical_fov), rules::decimal(aspect))); }

bool seen_by(const Viewer& v, Vec3 point, f32 far_entry, f32 height) {
  return g_rules.flag("SeenBy", looker_of(v), point, rules::decimal(far_entry), rules::decimal(height));
}

bool can_spawn(const std::vector<Viewer>& viewers, Vec3 point, f32 far_entry, f32 height) {
  std::vector<Looker> lookers;
  for (const Viewer& v : viewers) lookers.push_back(looker_of(v));
  return g_rules.flag("CanSpawn", lookers, point, rules::decimal(far_entry), rules::decimal(height));
}

bool footprints_overlap(const Footprint& a, const Footprint& b, f32 margin) { return g_rules.flag("FootprintsOverlap", print_of(a), print_of(b), rules::decimal(margin)); }

}  // namespace sc::spawn
