// SpawnCity's wanted rules (see wanted.h; sim/wanted_system_logic.mjs).
#include "wanted.h"

#include <algorithm>

namespace sc::wanted {

namespace {
i32 level(i32 stars, i32 lo) { return stars < lo ? lo : (stars > kMaxStars ? kMaxStars : stars); }
constexpr i32 kThresholds[5] = {4, 9, 15, 22, 30};
}  // namespace

i32 severity(Crime c) {
  switch (c) {
    case Crime::person: return 2;
    case Crime::person_hit: return 1;
    case Crime::player: return 2;
    case Crime::car: return 1;
    case Crime::bus: return 2;
    case Crime::police_drone:
    case Crime::police_officer:
    case Crime::police_car: return 3;
    case Crime::carjacking: return 1;
    case Crime::bird: return 0;
  }
  return 0;
}

i32 stars_for_heat(f32 heat) {
  const i32 v = heat > 0 ? static_cast<i32>(std::floor(heat)) : 0;
  i32 s = 0;
  for (i32 t : kThresholds)
    if (v >= t) ++s;
  return s < kMaxStars ? s : kMaxStars;
}
i32 police_count(i32 stars) { const i32 l = level(stars, 0); return l ? std::min(3, static_cast<i32>(std::ceil(l * 0.7f))) : 0; }
f32 detection_radius(i32 stars) { const i32 l = level(stars, 0); return l ? 52.0f + static_cast<f32>(l) * 7 : 0; }
f32 escape_ms(i32 stars) { const i32 l = level(stars, 0); return l ? 4200.0f + static_cast<f32>(l) * 900 : 0; }
f32 police_damage(i32 stars) { return static_cast<f32>(std::min(4, 2 + (level(stars, 1) - 1) / 2)); }
f32 spawn_radius(i32 index) { return 58.0f + static_cast<f32>((index < 0 ? 0 : index) % 3) * 8; }
f32 arrival_delay_ms(i32 stars) { return std::fmax(9000.0f, 16000.0f - static_cast<f32>(level(stars, 1)) * 1300.0f); }
f32 engage_delay_ms(i32 stars) { return std::fmax(4200.0f, 6800.0f - static_cast<f32>(level(stars, 1)) * 450.0f); }
f32 wave_break_ms(i32 stars) { return 9500.0f - static_cast<f32>(level(stars, 1)) * 450.0f; }
f32 fire_range(i32 stars) { return std::fmin(kMaxFireRange, 28 + static_cast<f32>(level(stars, 1)) * 1.4f); }
f32 fire_interval_ms(i32 stars) { return std::fmax(3900.0f, 5100.0f - static_cast<f32>(level(stars, 1)) * 230.0f); }
f32 hit_chance(i32 stars, f32 distance_m, f32 player_speed) {
  const f32 dp = clampf((distance_m - 6) / 32, 0, 1) * 0.26f, mp = std::fmin(1.0f, std::fmax(0.0f, player_speed) / 7.2f) * 0.24f;
  return clampf(0.28f + static_cast<f32>(level(stars, 1)) * 0.02f - dp - mp, 0.1f, 0.38f);
}
f32 emp_impulse(f32 d) {
  if (!(d >= 0) || d > kEmpRange) return 0;
  const f32 t = std::fmin(1.0f, d / kEmpRange), s = t * t * (3 - 2 * t);
  return 54 + 72 * (1 - s);
}

void Tracker::report(Crime c, bool witnessed) {
  if (!witnessed) return;
  heat_ += static_cast<f32>(severity(c));
  hidden_ms_ = 0;
}

void Tracker::update(f32 dt, bool seen) {
  const i32 s = stars();
  if (s <= 0 || seen) { hidden_ms_ = 0; return; }
  hidden_ms_ += dt * 1000;
  if (hidden_ms_ >= escape_ms(s)) clear();
}

}  // namespace sc::wanted
