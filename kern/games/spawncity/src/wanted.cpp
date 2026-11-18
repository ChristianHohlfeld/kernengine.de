// The game's calls into the Blueprint of the actor "Wanted" (see wanted.h): no rule lives here.
#include "wanted.h"

#include "blueprint_call.h"

namespace sc::wanted {

namespace {
rules::Blueprint g_rules{"Wanted"};
}  // namespace

bool bind(kern::logic::Runtime& logic) { return g_rules.bind(logic); }

i32 max_stars() { return g_rules.get<i32>("MaxStars"); }
f32 emp_range() { return static_cast<f32>(g_rules.get<f64>("EmpRange")); }
f32 emp_cooldown_ms() { return static_cast<f32>(g_rules.get<f64>("EmpCooldownMs")); }
f32 max_fire_range() { return static_cast<f32>(g_rules.get<f64>("MaxFireRange")); }

i32 severity(Crime c) {
  i32 r = 0;
  g_rules.call("Severity", std::tie(r), static_cast<i32>(c));
  return r;
}
i32 stars_for_heat(f32 heat) {
  i32 r = 0;
  g_rules.call("StarsForHeat", std::tie(r), rules::decimal(heat));
  return r;
}
i32 police_count(i32 stars) {
  i32 r = 0;
  g_rules.call("PoliceCount", std::tie(r), stars);
  return r;
}
f32 detection_radius(i32 stars) { return static_cast<f32>(g_rules.number("DetectionRadius", stars)); }
f32 escape_ms(i32 stars) { return static_cast<f32>(g_rules.number("EscapeMs", stars)); }
f32 police_damage(i32 stars) { return static_cast<f32>(g_rules.number("PoliceDamage", stars)); }
f32 spawn_radius(i32 index) { return static_cast<f32>(g_rules.number("SpawnRadius", index)); }
f32 arrival_delay_ms(i32 stars) { return static_cast<f32>(g_rules.number("ArrivalDelayMs", stars)); }
f32 engage_delay_ms(i32 stars) { return static_cast<f32>(g_rules.number("EngageDelayMs", stars)); }
f32 wave_break_ms(i32 stars) { return static_cast<f32>(g_rules.number("WaveBreakMs", stars)); }
f32 fire_range(i32 stars) { return static_cast<f32>(g_rules.number("FireRange", stars)); }
f32 fire_interval_ms(i32 stars) { return static_cast<f32>(g_rules.number("FireIntervalMs", stars)); }
f32 hit_chance(i32 stars, f32 distance_m, f32 player_speed) { return static_cast<f32>(g_rules.number("HitChance", stars, rules::decimal(distance_m), rules::decimal(player_speed))); }
f32 emp_impulse(f32 distance_m) { return static_cast<f32>(g_rules.number("EmpImpulse", rules::decimal(distance_m))); }

void report(Crime c, bool witnessed) { g_rules.call("Report", std::tie(), static_cast<i32>(c), witnessed); }
void update(f32 dt, bool seen) { g_rules.call("Update", std::tie(), rules::decimal(dt), seen); }
void clear() { g_rules.call("Clear", std::tie()); }
f32 hidden_ms() { return static_cast<f32>(g_rules.get<f64>("HiddenMs")); }
f32 heat() { return static_cast<f32>(g_rules.get<f64>("Heat")); }
i32 stars() {
  i32 r = 0;
  g_rules.call("Stars", std::tie(r));
  return r;
}

}  // namespace sc::wanted
