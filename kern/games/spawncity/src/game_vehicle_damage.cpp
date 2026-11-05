// SpawnCity on Kern: what cars and buses take, 1:1 after the original - civilian cars and buses
// (walk_world_experience_hotfix.mjs TARGET_HP / TARGET_DAMAGE, critical_damage_logic.mjs PROFILES,
// gameplay_final_runtime_v2.mjs dispatchExplosion) and police cruisers (police_ground_units.mjs
// damageCar / onExplosion / crashCheck). Hooks: damage_car, explosion, the shot handlers.
#include <algorithm>
#include <cmath>

#include "game.h"

namespace sc {

namespace {
bool is_bus(const TrafficCar& c) { return c.car->spec().radius > 0.4f; }
}  // namespace

f32 Game::car_max_hp(const TrafficCar& c) const {
  // police CAR_HP 300; TARGET_HP car 180, bus 320
  return c.police ? 300.0f : is_bus(c) ? 320.0f : 180.0f;
}

void Game::car_bullet(i32 i, bool by_player) {
  // a round that hits a car: police CAR_HIT 22, TARGET_DAMAGE car 30 / bus 26; the sniper's .50 takes
  // SNIPER_CAR_FRACTION .34 of a civilian's HP, a cruiser takes it as four hits (sniperShotAt)
  if (i < 0 || i >= static_cast<i32>(cars_.size())) return;
  const TrafficCar& c = cars_[i];
  const bool sniper = mode_ == Mode::foot && foot_weapon_ == weapons::Id::sniper;
  f32 dmg = c.police ? 22.0f : is_bus(c) ? 26.0f : 30.0f;
  if (sniper) dmg = c.police ? 4 * 22.0f : std::fmax(1.0f, std::round(car_max_hp(c) * 0.34f));
  damage_car(i, dmg, by_player);
}

void Game::blast_cars(Vec3 at, f32 radius, f32 damage, bool by_player) {
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    if (!c.placed || c.wreck) continue;
    const Vec3 p = matrix_of(c.actor).translation();
    if (c.police) {
      // police_ground_units.mjs onExplosion: r 2..60, maxDamage >= 20, linear over r + 2.5 m, x 2.4
      const f32 r = clampf(radius, 2, 60), md = std::fmax(20.0f, damage), dd = std::hypot(p.x - at.x, p.y - at.y);
      if (dd < r + 2.5f) damage_car(static_cast<i32>(i), md * 2.4f * (1 - dd / (r + 2.5f)), by_player);
      continue;
    }
    // dispatchExplosion: radius 2..18; f = smoothstep falloff x exposure (buildings .18); a car takes
    // ceil(3 f) hits, a bus ceil(4 f), at most 5 (each TARGET_DAMAGE)
    const f32 r = clampf(radius, 2, 18), d = kern::length(p - at);
    if (d >= r) continue;
    const f32 f = weapons::blast_factor(d, r, !line_of_sight(at, p + Vec3{0, 0, 0.8f}));
    const f32 hits = std::fmin(5.0f, std::ceil(f * (is_bus(c) ? 4.0f : 3.0f)));
    for (i32 k = 0; k < static_cast<i32>(hits); ++k) damage_car(static_cast<i32>(i), is_bus(c) ? 26.0f : 30.0f, by_player);  // hit by hit (a critical car speeds up)
  }
}

}  // namespace sc
