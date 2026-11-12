// The game's calls into the Blueprint of the actor "CombatDetail" (see combat_detail.h): no rule
// lives here.
#include "combat_detail.h"

#include "blueprint_call.h"

namespace sc::detail {

namespace {
using kern::logic::Value;
using rules::of_enum;
rules::Blueprint g_rules;
kern::logic::Instance*& g_blueprint = g_rules.running;

bool call(const char* function, const std::vector<Value>& args, std::vector<Value>& results) { return g_rules.call(function, args, results); }
f64 number(const char* function, const std::vector<Value>& args) { return g_rules.number(function, args); }
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_blueprint = blueprint; }
kern::logic::Instance* blueprint() { return g_blueprint; }

bool is_critical(Critical k, f32 hp, f32 max_hp) {
  std::vector<Value> r;
  return call("IsCritical", {of_enum(k), rules::number(hp), rules::number(max_hp)}, r) && !r.empty() && r[0].b;
}
f32 detonation_at(Critical k, f32 now) { return static_cast<f32>(number("DetonationAt", {of_enum(k), rules::number(now)})); }
f32 accelerate(Critical k, f32 current_at, f32 now) { return static_cast<f32>(number("Accelerate", {of_enum(k), rules::number(current_at), rules::number(now)})); }

bool CriticalClock::update(Critical k, f32 hp, f32 max_hp, f32 now) {
  std::vector<Value> r;
  if (!call("ClockUpdate", {of_enum(k), rules::number(hp), rules::number(max_hp), rules::number(now), rules::number(at), rules::number(last_hp)}, r) || r.size() < 3) return false;
  at = static_cast<f32>(r[1].as_number());
  last_hp = static_cast<f32>(r[2].as_number());
  return r[0].b;
}

f32 momentum_ns(Round r) { return static_cast<f32>(number("MomentumNs", {of_enum(r)})); }
Vec3 bullet_impulse(Vec3 direction, Round round) {
  std::vector<Value> r;
  return call("BulletImpulse", {Value::vector(direction), of_enum(round)}, r) && !r.empty() ? r[0].as_vector() : Vec3{};
}
f32 projectile_gravity() { return g_blueprint ? static_cast<f32>(g_blueprint->get("ProjectileGravity").as_number()) : 0; }

}  // namespace sc::detail
