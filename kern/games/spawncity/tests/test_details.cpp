// GUIDED missile, critical damage clocks, bullet momentum (guided.h,
// combat_detail.h) with SpawnCity's numbers.
#include "../src/combat_audio.h"
#include "../src/combat_detail.h"
#include "../src/guided.h"
#include "../src/rules.h"
#include "rule_blueprint.h"
#include "test_util.h"

#include <algorithm>
#include <chrono>

using namespace sc;

namespace {
struct CombatDetailBlueprint : RuleBlueprint {
  explicit CombatDetailBlueprint(bool native = true) : RuleBlueprint("CombatDetail", &detail::bind, native) {}
};
struct GuidedBlueprint : RuleBlueprint {
  GuidedBlueprint() : RuleBlueprint("Guided", &guided::bind) {}
};
}  // namespace

SC_TEST(details_blueprint) {
  CombatDetailBlueprint bp;
  SC_CHECK(bp.inst && bp.problems.empty(), "the level's CombatDetail Blueprint compiles without errors or warnings: %s", bp.problems.c_str());
  if (!bp.inst) return;
  SC_CHECK(!bp.inst->disabled() && bp.host.errors.empty(), "it runs (%s)", bp.host.errors.c_str());
  // as in the game: its compiled C++ class (src/level_blueprints.cpp, written from the level by the build)
  SC_CHECK(std::string(bp.inst->tier()) == "native C++", "the CombatDetail Blueprint runs as its C++ class (it runs: %s)", bp.inst->tier());
  for (const char* f : {"IsCritical", "DetonationAt", "Accelerate", "ClockUpdate", "MomentumNs", "BulletImpulse"})
    SC_CHECK(bp.inst->has_function(f), "function %s", f);
  SC_CHECK(std::fabs(detail::projectile_gravity() - 9.80665f) < 1e-6f, "the constants are its variables");
  SC_CHECK(detail::bullet_impulse({0, 0, 0}, detail::Round::nine_mm).x > 2.8f, "a bullet without a direction pushes along x");
  // How long a rule takes when the game asks the Blueprint for it - the clock of one vehicle,
  // asked every frame - as the class and in the interpreter.
  auto clock_ns = [](int rounds) {
    detail::CriticalClock c;
    const auto t0 = std::chrono::steady_clock::now();
    int fired = 0;
    for (int i = 0; i < rounds; ++i) fired += c.update(detail::Critical::drone, 100.0f - static_cast<f32>(std::min(i % 300, 95)), 100, static_cast<f32>(i) / 60) ? 1 : 0;
    const f64 ns = std::chrono::duration<f64, std::nano>(std::chrono::steady_clock::now() - t0).count() / rounds;
    return std::pair<f64, int>(ns, fired);
  };
  const auto native = clock_ns(20000);
  f64 interpreted = 0;
  int interpreted_fired = 0;
  {
    CombatDetailBlueprint slow(false);
    SC_CHECK(slow.inst && std::string(slow.inst->tier()) != "native C++", "(the same Blueprint in the interpreter)");
    const auto r = clock_ns(20000);
    interpreted = r.first;
    interpreted_fired = r.second;
  }
  detail::bind(bp.inst);
  std::printf("  CombatDetail.ClockUpdate from C++: %.0f ns as its class, %.0f ns in the interpreter (this build checks every memory access)\n", native.first, interpreted);
  SC_CHECK(native.second == interpreted_fired && native.second > 0, "both give the same (%d / %d blasts)", native.second, interpreted_fired);
  SC_CHECK(native.first < 20000, "a rule asked every frame costs microseconds, not milliseconds (%.0f ns)", native.first);
}

SC_TEST(guided_cruise_dive_and_fuze) {
  GuidedBlueprint bp;
  // A target 300 m away on the ground: the missile drops, climbs to cruise height (target + 28 m),
  // flies flat at <= 34 m/s and dives in to the fuze.
  const Vec3 start{0, 0, 20}, target{300, 0, 1};
  guided::Missile m = guided::launch(start, target, 0, false);
  SC_CHECK(std::fabs(m.cruise_z - 29) < 1e-4f, "cruise = max(launch + 8, target + 28) = 29 (%.2f)", m.cruise_z);
  f32 t = 0, max_z = 0, max_speed = 0, dt = 1.0f / 60;
  guided::Result r = guided::Result::flying;
  for (int i = 0; i < 60 * 30 && r == guided::Result::flying; ++i) {
    t += dt;
    r = guided::step(m, t, dt, 0);
    if (t < guided::ignite()) SC_CHECK(!m.motor, "no motor on the rail");
    if (t > 1) { max_z = std::fmax(max_z, m.p.z); max_speed = std::fmax(max_speed, kern::length(m.v)); }
  }
  SC_CHECK(r == guided::Result::fuze, "proximity fuze at the target (result %d)", static_cast<int>(r));
  SC_CHECK(max_speed <= guided::speed() + 1e-3f, "never faster than 34 m/s (%.2f)", max_speed);
  // It aims at the cruise height above the target, so it climbs on a shallow slope from 20 m.
  SC_CHECK(max_z > 22 && max_z < 30, "climbs towards the 29 m cruise height before the dive (%.1f)", max_z);
  SC_CHECK(t > 300.0f / 34.0f && t < 16, "flight time plausible for 300 m (%.1f s)", t);
}

SC_TEST(guided_turn_limit_and_dodge) {
  GuidedBlueprint bp;
  // Turning: velocity change per step never exceeds 2.2 g (x1.4 in the dive).
  guided::Missile m = guided::launch({0, 0, 50}, {0, 200, 50}, 0, true);
  m.born = -1;  // motor already lit
  m.v = {34, 0, 0};
  const Vec3 v0 = m.v;
  guided::step(m, 0, 0.1f, 0);
  SC_CHECK(kern::length(m.v - v0) <= 9.81f * guided::turn_g() * 0.1f + 0.5f, "turn limited (dv %.2f)", kern::length(m.v - v0));
  // A sprinting pedestrian (>= 5.8 m/s) for 0.35 s shakes the lock off; walking does not.
  guided::Dodge d;
  for (int i = 0; i < 5; ++i) d = guided::update_dodge(d, {0, 0}, {0.3f, 0}, 0.05f, true);  // 6 m/s
  SC_CHECK(!d.lost && d.seconds > 0.24f, "0.25 s of sprint: still locked");
  for (int i = 0; i < 3; ++i) d = guided::update_dodge(d, {0, 0}, {0.3f, 0}, 0.05f, true);
  SC_CHECK(d.lost, "0.4 s of sprint: lock lost");
  guided::Dodge w;
  for (int i = 0; i < 40; ++i) w = guided::update_dodge(w, {0, 0}, {0.1f, 0}, 0.05f, true);  // 2 m/s
  SC_CHECK(!w.lost && w.seconds == 0, "walking keeps the lock");
  guided::Dodge car;
  for (int i = 0; i < 40; ++i) car = guided::update_dodge(car, {0, 0}, {1, 0}, 0.05f, false);
  SC_CHECK(!car.lost, "vehicles cannot dodge by speed");
  // Tracking stops once dodged: the goal stays at the last place it knew.
  guided::Missile p = guided::launch({0, 0, 30}, {50, 0, 1}, 0, true);
  for (int i = 1; i <= 10; ++i) guided::track(p, {50.0f + 0.3f * i, 0, 1}, 0.05f);
  SC_CHECK(!p.tracking && p.goal.x < 52, "lock lost after the sprint, goal frozen (%.1f)", p.goal.x);
}

SC_TEST(guided_pick_score) {
  GuidedBlueprint bp;
  const Vec3 eye{0, 0, 10}, fwd{1, 0, 0};
  const f32 fov = 60 * kDeg;
  SC_CHECK(guided::pick_score(eye, fwd, fov, 540, {100, 0, 10}) == 0, "dead centre: 0 px");
  SC_CHECK(guided::pick_score(eye, fwd, fov, 540, {-100, 0, 10}) < 0, "behind: not pickable");
  // 5 m off at 100 m: 0.05 / tan(30 deg) * 270 = 23.4 px, minus min(18, 6) = 17.4 px.
  const f32 s = guided::pick_score(eye, fwd, fov, 540, {100, 5, 10});
  SC_CHECK(std::fabs(s - 17.4f) < 0.5f, "off-axis pixels with the slack (%.2f)", s);
}

SC_TEST(details_critical_clocks) {
  using namespace detail;
  CombatDetailBlueprint bp;
  SC_CHECK(is_critical(Critical::car, 60, 300) && !is_critical(Critical::car, 61, 300), "car critical at 20 %%");
  SC_CHECK(is_critical(Critical::police_drone, 34, 100) && !is_critical(Critical::police_drone, 35, 100), "police drone at 34 %%");
  SC_CHECK(std::fabs(detonation_at(Critical::bus, 10) - 13.1f) < 1e-4f, "bus blows 3.1 s later");
  SC_CHECK(std::fabs(accelerate(Critical::car, 12, 11) - 11.62f) < 1e-4f, "a hit brings it to 0.62 s");
  SC_CHECK(accelerate(Critical::car, 11.2f, 11) == 11.2f, "never later than already due");
  CriticalClock c;
  SC_CHECK(!c.update(Critical::drone, 100, 100, 0), "healthy");
  SC_CHECK(!c.update(Critical::drone, 28, 100, 1) && c.armed(), "28 %%: countdown armed");
  SC_CHECK(std::fabs(c.at - 3.05f) < 1e-4f, "2.05 s for the drone");
  SC_CHECK(!c.update(Critical::drone, 20, 100, 1.5f) && std::fabs(c.at - 2.04f) < 1e-4f, "hit again: 0.54 s");
  SC_CHECK(c.update(Critical::drone, 20, 100, 2.1f) && !c.armed(), "blows when due, once");
  CriticalClock r;
  r.update(Critical::drone, 20, 100, 0);
  r.update(Critical::drone, 100, 100, 1);
  SC_CHECK(!r.armed(), "a new (healthy) drone clears the countdown");
}

SC_TEST(details_bullet_momentum) {
  using namespace detail;
  CombatDetailBlueprint bp;
  SC_CHECK(std::fabs(momentum_ns(Round::nine_mm) - 2.88f) < 1e-3f, "9 mm 2.88 N*s");
  SC_CHECK(std::fabs(momentum_ns(Round::fifty_bmg) - 40.762f) < 1e-2f, ".50 BMG ~41 N*s");
  const Vec3 j = bullet_impulse({0, 3, 0}, Round::five_five_six);
  SC_CHECK(std::fabs(j.y - 3.76f) < 1e-3f && j.x == 0 && j.z == 0, "along the path, no upward bias");
}

SC_TEST(details_combat_audio_bank) {
  using namespace combat_audio;
  // Lengths as SpawnCity's prebaked bank (ceil(duration * 44100)), every sample finite and in range.
  auto near = [](std::size_t n, std::size_t want) { return n + 1 >= want && n <= want + 1; };
  SC_CHECK(near(render(Kind::shot, 0).size(), 8820), "MP round 0.2 s (%zu)", render(Kind::shot, 0).size());
  SC_CHECK(near(render(Kind::pistol, 1).size(), 26108), "pistol variant 1: 0.592 s (%zu)", render(Kind::pistol, 1).size());
  for (u8 i = 0; i < static_cast<u8>(Kind::count); ++i)
    for (u32 v = 0; v < kVariants[i]; ++v) {
      const std::vector<f32> pcm = render(static_cast<Kind>(i), v);
      f32 peak = 0;
      bool finite = !pcm.empty();
      for (f32 x : pcm) { finite = finite && std::isfinite(x); peak = std::fmax(peak, std::fabs(x)); }
      SC_CHECK(finite && peak > 0.3f && peak <= 1.0f, "%s %u: finite, normalised (peak %.2f)", kind_name(static_cast<Kind>(i)), v, peak);
    }
}
