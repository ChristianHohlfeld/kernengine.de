// The game's calls into the Blueprint of the actor "Guided" (see guided.h): no rule lives here.
#include "guided.h"

#include "blueprint_call.h"

namespace sc::guided {

namespace {
using kern::logic::Value;
rules::Blueprint g_rules;

// The Blueprint's struct Missile, field by field in its order.
Value to_value(const Missile& m) {
  return Value::structure({Value::vector(m.p), Value::vector(m.v), Value::vector(m.goal), Value::number(m.born), Value::number(m.cruise_z), Value::boolean(m.motor),
                           Value::boolean(m.pedestrian), Value::number(m.dodge.seconds), Value::boolean(m.dodge.lost), Value::vector({m.dodge_last.x, m.dodge_last.y, 0}),
                           Value::boolean(m.tracking)});
}
bool to_missile(const Value& v, Missile& m) {
  if (v.fields() < 11) return false;
  m.p = v.field(0)->as_vector();
  m.v = v.field(1)->as_vector();
  m.goal = v.field(2)->as_vector();
  m.born = static_cast<f32>(v.field(3)->as_number());
  m.cruise_z = static_cast<f32>(v.field(4)->as_number());
  m.motor = v.field(5)->b;
  m.pedestrian = v.field(6)->b;
  m.dodge.seconds = static_cast<f32>(v.field(7)->as_number());
  m.dodge.lost = v.field(8)->b;
  const Vec3 last = v.field(9)->as_vector();
  m.dodge_last = {last.x, last.y};
  m.tracking = v.field(10)->b;
  return true;
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

f32 speed() { return static_cast<f32>(g_rules.variable("Speed")); }
f32 turn_g() { return static_cast<f32>(g_rules.variable("TurnG")); }
f32 blast_radius() { return static_cast<f32>(g_rules.variable("BlastRadius")); }
f32 damage() { return static_cast<f32>(g_rules.variable("Damage")); }
f32 cooldown() { return static_cast<f32>(g_rules.variable("Cooldown")); }
f32 pick_px() { return static_cast<f32>(g_rules.variable("PickPx")); }
f32 range() { return static_cast<f32>(g_rules.variable("Range")); }
f32 ignite() { return static_cast<f32>(g_rules.variable("Ignite")); }

Dodge update_dodge(Dodge state, Vec2 previous, Vec2 current, f32 dt, bool pedestrian) {
  std::vector<Value> r;
  if (!g_rules.call("UpdateDodge", {Value::number(state.seconds), Value::boolean(state.lost), Value::vector({previous.x, previous.y, 0}), Value::vector({current.x, current.y, 0}),
                                    Value::number(dt), Value::boolean(pedestrian)}, r) || r.size() < 2)
    return state;
  return {static_cast<f32>(r[0].as_number()), r[1].b};
}

f32 pick_score(Vec3 eye, Vec3 forward, f32 fov, f32 height_px, Vec3 target) {
  return static_cast<f32>(g_rules.number("PickScore", {Value::vector(eye), Value::vector(forward), Value::number(fov), Value::number(height_px), Value::vector(target)}));
}

Missile launch(Vec3 start, Vec3 target, f32 now, bool pedestrian) {
  Missile m;
  std::vector<Value> r;
  if (g_rules.call("Launch", {Value::vector(start), Value::vector(target), Value::number(now), Value::boolean(pedestrian)}, r) && !r.empty()) to_missile(r[0], m);
  return m;
}

void track(Missile& m, Vec3 at, f32 dt) {
  std::vector<Value> r;
  if (g_rules.call("Track", {to_value(m), Value::vector(at), Value::number(dt)}, r) && !r.empty()) to_missile(r[0], m);
}

Result step(Missile& m, f32 now, f32 dt, f32 ground) {
  std::vector<Value> r;
  if (!g_rules.call("Step", {to_value(m), Value::number(now), Value::number(dt), Value::number(ground)}, r) || r.size() < 2 || !to_missile(r[0], m)) return Result::flying;
  return static_cast<Result>(r[1].i);
}

}  // namespace sc::guided
