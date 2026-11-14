// The game's calls into the Blueprint of the actor "CombatDetail" (see combat_detail.h): no rule
// lives here.
#include "combat_detail.h"

#include "blueprint_call.h"

namespace sc::detail {

namespace {
using rules::decimal;
rules::Blueprint g_rules{"CombatDetail"};
kern::logic::Instance*& g_blueprint = g_rules.running;
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_blueprint = blueprint; }
kern::logic::Instance* blueprint() { return g_blueprint; }

bool is_critical(Critical k, f32 hp, f32 max_hp) {
  bool critical = false;
  g_rules.call("IsCritical", std::tie(critical), k, decimal(hp), decimal(max_hp));
  return critical;
}
f32 detonation_at(Critical k, f32 now) {
  f64 at = 0;
  g_rules.call("DetonationAt", std::tie(at), k, decimal(now));
  return static_cast<f32>(at);
}
f32 accelerate(Critical k, f32 current_at, f32 now) {
  f64 at = 0;
  g_rules.call("Accelerate", std::tie(at), k, decimal(current_at), decimal(now));
  return static_cast<f32>(at);
}

bool CriticalClock::update(Critical k, f32 hp, f32 max_hp, f32 now) {
  bool fires = false;
  f64 next_at = 0, next_hp = 0;
  g_rules.call("ClockUpdate", std::tie(fires, next_at, next_hp), k, decimal(hp), decimal(max_hp), decimal(now), decimal(at), decimal(last_hp));
  at = static_cast<f32>(next_at);
  last_hp = static_cast<f32>(next_hp);
  return fires;
}

f32 momentum_ns(Round r) {
  f64 ns = 0;
  g_rules.call("MomentumNs", std::tie(ns), r);
  return static_cast<f32>(ns);
}
Vec3 bullet_impulse(Vec3 direction, Round round) {
  Vec3 impulse{};
  g_rules.call("BulletImpulse", std::tie(impulse), direction, round);
  return impulse;
}
f32 projectile_gravity() { return g_blueprint ? static_cast<f32>(g_blueprint->get("ProjectileGravity").as_number()) : 0; }

}  // namespace sc::detail
