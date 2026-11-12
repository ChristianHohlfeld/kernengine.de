// The game's calls into the Blueprint of the actor "SpawnGuard" (see spawn_guard.h): no rule lives here.
#include "spawn_guard.h"

#include "blueprint_call.h"

namespace sc::spawn {

namespace {
using kern::logic::Value;
rules::Blueprint g_rules;

// The Blueprint's structs, field by field in their order.
Value to_value(const Viewer& v) {
  return Value::structure({Value::vector(v.eye), Value::vector(v.forward), Value::number(v.half_angle), Value::number(v.far), Value::boolean(v.local)});
}
Value to_value(const Footprint& f) {
  return Value::structure({Value::vector({f.center.x, f.center.y, 0}), Value::number(f.yaw), Value::vector({f.half.x, f.half.y, 0})});
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

f32 default_far() { return static_cast<f32>(g_rules.variable("DefaultFar")); }

f32 cone_half_angle(f32 vertical_fov, f32 aspect) { return static_cast<f32>(g_rules.number("ConeHalfAngle", {Value::number(vertical_fov), Value::number(aspect)})); }

bool seen_by(const Viewer& v, Vec3 point, f32 far_entry, f32 height) {
  return g_rules.flag("SeenBy", {to_value(v), Value::vector(point), Value::number(far_entry), Value::number(height)});
}

bool can_spawn(const std::vector<Viewer>& viewers, Vec3 point, f32 far_entry, f32 height) {
  std::vector<Value> list;
  list.reserve(viewers.size());
  for (const Viewer& v : viewers) list.push_back(to_value(v));
  return g_rules.flag("CanSpawn", {Value::of(std::move(list)), Value::vector(point), Value::number(far_entry), Value::number(height)});
}

bool footprints_overlap(const Footprint& a, const Footprint& b, f32 margin) { return g_rules.flag("FootprintsOverlap", {to_value(a), to_value(b), Value::number(margin)}); }

}  // namespace sc::spawn
