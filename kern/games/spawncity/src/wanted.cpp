// The game's calls into the Blueprint of the actor "Wanted" (see wanted.h): no rule lives here.
#include "wanted.h"

#include "blueprint_call.h"

namespace sc::wanted {

namespace {
kern::logic::Instance* g_blueprint = nullptr;

f64 call(const char* function, std::initializer_list<f64> args) { return g_blueprint ? g_blueprint->call_number(function, args) : 0; }
f64 variable(const char* name) { return g_blueprint ? g_blueprint->get(name).as_number() : 0; }
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_blueprint = blueprint; }
kern::logic::Instance* blueprint() { return g_blueprint; }

i32 max_stars() { return static_cast<i32>(variable("MaxStars")); }
f32 emp_range() { return static_cast<f32>(variable("EmpRange")); }
f32 emp_cooldown_ms() { return static_cast<f32>(variable("EmpCooldownMs")); }
f32 max_fire_range() { return static_cast<f32>(variable("MaxFireRange")); }

i32 severity(Crime c) { return static_cast<i32>(call("Severity", {static_cast<f64>(c)})); }
i32 stars_for_heat(f32 heat) { return static_cast<i32>(call("StarsForHeat", {rules::decimal(heat)})); }
i32 police_count(i32 stars) { return static_cast<i32>(call("PoliceCount", {static_cast<f64>(stars)})); }
f32 detection_radius(i32 stars) { return static_cast<f32>(call("DetectionRadius", {static_cast<f64>(stars)})); }
f32 escape_ms(i32 stars) { return static_cast<f32>(call("EscapeMs", {static_cast<f64>(stars)})); }
f32 police_damage(i32 stars) { return static_cast<f32>(call("PoliceDamage", {static_cast<f64>(stars)})); }
f32 spawn_radius(i32 index) { return static_cast<f32>(call("SpawnRadius", {static_cast<f64>(index)})); }
f32 arrival_delay_ms(i32 stars) { return static_cast<f32>(call("ArrivalDelayMs", {static_cast<f64>(stars)})); }
f32 engage_delay_ms(i32 stars) { return static_cast<f32>(call("EngageDelayMs", {static_cast<f64>(stars)})); }
f32 wave_break_ms(i32 stars) { return static_cast<f32>(call("WaveBreakMs", {static_cast<f64>(stars)})); }
f32 fire_range(i32 stars) { return static_cast<f32>(call("FireRange", {static_cast<f64>(stars)})); }
f32 fire_interval_ms(i32 stars) { return static_cast<f32>(call("FireIntervalMs", {static_cast<f64>(stars)})); }
f32 hit_chance(i32 stars, f32 distance_m, f32 player_speed) { return static_cast<f32>(call("HitChance", {static_cast<f64>(stars), rules::decimal(distance_m), rules::decimal(player_speed)})); }
f32 emp_impulse(f32 distance_m) { return static_cast<f32>(call("EmpImpulse", {rules::decimal(distance_m)})); }

void Tracker::report(Crime c, bool witnessed) { call("Report", {static_cast<f64>(c), witnessed ? 1.0 : 0.0}); }
void Tracker::update(f32 dt, bool seen) { call("Update", {rules::decimal(dt), seen ? 1.0 : 0.0}); }
void Tracker::clear() { call("Clear", {}); }
f32 Tracker::hidden_ms() const { return static_cast<f32>(variable("HiddenMs")); }
f32 Tracker::heat() const { return static_cast<f32>(variable("Heat")); }
i32 Tracker::stars() const { return static_cast<i32>(call("Stars", {})); }

}  // namespace sc::wanted
