// GUIDED missile, critical damage clocks, bullet momentum (guided.h,
// combat_detail.h) with SpawnCity's numbers. What the program's C++ asks of the rules is the level's Blueprints
// guided_cpp and combat_detail_cpp, called as their classes - as the program calls them (level_blueprints.h) -
// beside the rule actors Guided and CombatDetail they ask (Call on Actor).
#include "../src/blueprint_call.h"
#include "level_blueprints.h"
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
using Cd = blueprints::combat_detail_cpp;
using Gd = blueprints::guided_cpp;
/// The class T of a program file's Blueprint, running beside the rule actor it asks (`rule`).
template <class T> struct ProgramClass {
  RuleBlueprint rules;
  bool bound = false;
  ProgramClass(const char* actor, const char* rule) : rules(actor, nullptr, true, {rule}) {
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt) && kern::logic::blueprint<T>().bind().Result;
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
struct DetailClass : ProgramClass<Cd> {
  DetailClass() : ProgramClass("combat_detail_cpp", "CombatDetail") {}
};
struct GuidedClass : ProgramClass<Gd> {
  GuidedClass() : ProgramClass("guided_cpp", "Guided") {}
};
#define CLASS_BOUND(c, name) \
  SC_CHECK((c).bound, name " runs as its class, beside its rule actor: %s", (c).rules.problems.c_str()); \
  if (!(c).bound) return
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
  DetailClass dc;
  CLASS_BOUND(dc, "combat_detail_cpp");
  Cd& cd = *dc;
  SC_CHECK(std::fabs(cd.projectile_gravity().Result - 9.80665f) < 1e-6f, "the constants are its variables");
  SC_CHECK(cd.bullet_impulse({0, 0, 0}, Cd::detail_Round::nine_mm).Result.x > 2.8f, "a bullet without a direction pushes along x");
  // How long a rule takes when the game asks the Blueprint for it - the clock of one vehicle,
  // asked every frame - as the class and in the interpreter.
  // (as its class: the program's combat_detail_cpp asks CombatDetail; in the interpreter - a runtime that runs no
  // class - the C++ of combat_detail.h asks it, until the program runs from the level)
  auto class_clock_ns = [&](int rounds) {
    Cd::detail_CriticalClock c;
    const auto t0 = std::chrono::steady_clock::now();
    int fired = 0;
    for (int i = 0; i < rounds; ++i) fired += cd.CriticalClock_update(c, Cd::detail_Critical::drone, 100.0f - static_cast<f32>(std::min(i % 300, 95)), 100, static_cast<f32>(i) / 60).Result ? 1 : 0;
    const f64 ns = std::chrono::duration<f64, std::nano>(std::chrono::steady_clock::now() - t0).count() / rounds;
    return std::pair<f64, int>(ns, fired);
  };
  auto clock_ns = [](int rounds) {
    detail::CriticalClock c;
    const auto t0 = std::chrono::steady_clock::now();
    int fired = 0;
    for (int i = 0; i < rounds; ++i) fired += c.update(detail::Critical::drone, 100.0f - static_cast<f32>(std::min(i % 300, 95)), 100, static_cast<f32>(i) / 60) ? 1 : 0;
    const f64 ns = std::chrono::duration<f64, std::nano>(std::chrono::steady_clock::now() - t0).count() / rounds;
    return std::pair<f64, int>(ns, fired);
  };
  const auto native = class_clock_ns(20000);
  f64 interpreted = 0;
  int interpreted_fired = 0;
  {
    CombatDetailBlueprint slow(false);
    SC_CHECK(slow.inst && std::string(slow.inst->tier()) != "native C++", "(the same Blueprint in the interpreter)");
    const auto r = clock_ns(20000);
    interpreted = r.first;
    interpreted_fired = r.second;
  }
  detail::bind(bp.rt);
  std::printf("  CombatDetail.ClockUpdate from C++: %.0f ns as its class, %.0f ns in the interpreter (this build checks every memory access)\n", native.first, interpreted);
  SC_CHECK(native.second == interpreted_fired && native.second > 0, "both give the same (%d / %d blasts)", native.second, interpreted_fired);
  // (the bound is the claim - under a millisecond, as for the rules of Controls (test_controls.cpp): what one call takes
  // on a busy machine, in a build that checks every memory access, varies by tens of microseconds - E19)
  SC_CHECK(native.first < 1e6, "a rule asked every frame costs microseconds, not milliseconds (%.0f ns)", native.first);
}

SC_TEST(guided_cruise_dive_and_fuze) {
  GuidedClass gc;
  CLASS_BOUND(gc, "guided_cpp");
  Gd& g = *gc;
  // A target 300 m away on the ground: the missile drops, climbs to cruise height (target + 28 m),
  // flies flat at <= 34 m/s and dives in to the fuze.
  const Vec3 start{0, 0, 20}, target{300, 0, 1};
  Gd::guided_Missile m = g.launch(start, target, 0, false).Result;
  SC_CHECK(std::fabs(m.cruise_z - 29) < 1e-4f, "cruise = max(launch + 8, target + 28) = 29 (%.2f)", m.cruise_z);
  f32 t = 0, max_z = 0, max_speed = 0, dt = 1.0f / 60;
  Gd::the_guided_Result_ r = Gd::the_guided_Result_::flying;
  for (int i = 0; i < 60 * 30 && r == Gd::the_guided_Result_::flying; ++i) {
    t += dt;
    r = g.step(m, t, dt, 0).Result;
    if (t < g.ignite().Result) SC_CHECK(!m.motor, "no motor on the rail");
    if (t > 1) { max_z = std::fmax(max_z, m.p.z); max_speed = std::fmax(max_speed, kern::length(m.v)); }
  }
  SC_CHECK(r == Gd::the_guided_Result_::fuze, "proximity fuze at the target (result %d)", static_cast<int>(r));
  SC_CHECK(max_speed <= g.speed().Result + 1e-3f, "never faster than 34 m/s (%.2f)", max_speed);
  // It aims at the cruise height above the target, so it climbs on a shallow slope from 20 m.
  SC_CHECK(max_z > 22 && max_z < 30, "climbs towards the 29 m cruise height before the dive (%.1f)", max_z);
  SC_CHECK(t > 300.0f / 34.0f && t < 16, "flight time plausible for 300 m (%.1f s)", t);
}

SC_TEST(guided_turn_limit_and_dodge) {
  GuidedClass gc;
  CLASS_BOUND(gc, "guided_cpp");
  Gd& g = *gc;
  // Turning: velocity change per step never exceeds 2.2 g (x1.4 in the dive).
  Gd::guided_Missile m = g.launch({0, 0, 50}, {0, 200, 50}, 0, true).Result;
  m.born = -1;  // motor already lit
  m.v = {34, 0, 0};
  const Vec3 v0 = m.v;
  g.step(m, 0, 0.1f, 0);
  SC_CHECK(kern::length(m.v - v0) <= 9.81f * g.turn_g().Result * 0.1f + 0.5f, "turn limited (dv %.2f)", kern::length(m.v - v0));
  // A sprinting pedestrian (>= 5.8 m/s) for 0.35 s shakes the lock off; walking does not.
  Gd::guided_Dodge d;
  for (int i = 0; i < 5; ++i) d = g.update_dodge(d, {0, 0, 0}, {0.3f, 0, 0}, 0.05f, true).Result;  // 6 m/s
  SC_CHECK(!d.lost && d.seconds > 0.24f, "0.25 s of sprint: still locked");
  for (int i = 0; i < 3; ++i) d = g.update_dodge(d, {0, 0, 0}, {0.3f, 0, 0}, 0.05f, true).Result;
  SC_CHECK(d.lost, "0.4 s of sprint: lock lost");
  Gd::guided_Dodge w;
  for (int i = 0; i < 40; ++i) w = g.update_dodge(w, {0, 0, 0}, {0.1f, 0, 0}, 0.05f, true).Result;  // 2 m/s
  SC_CHECK(!w.lost && w.seconds == 0, "walking keeps the lock");
  Gd::guided_Dodge car;
  for (int i = 0; i < 40; ++i) car = g.update_dodge(car, {0, 0, 0}, {1, 0, 0}, 0.05f, false).Result;
  SC_CHECK(!car.lost, "vehicles cannot dodge by speed");
  // Tracking stops once dodged: the goal stays at the last place it knew.
  Gd::guided_Missile p = g.launch({0, 0, 30}, {50, 0, 1}, 0, true).Result;
  for (int i = 1; i <= 10; ++i) g.track(p, {50.0f + 0.3f * i, 0, 1}, 0.05f);
  SC_CHECK(!p.tracking && p.goal.x < 52, "lock lost after the sprint, goal frozen (%.1f)", p.goal.x);
}

SC_TEST(guided_pick_score) {
  GuidedClass gc;
  CLASS_BOUND(gc, "guided_cpp");
  Gd& g = *gc;
  const Vec3 eye{0, 0, 10}, fwd{1, 0, 0};
  const f32 fov = 60 * kDeg;
  SC_CHECK(g.pick_score(eye, fwd, fov, 540, {100, 0, 10}).Result == 0, "dead centre: 0 px");
  SC_CHECK(g.pick_score(eye, fwd, fov, 540, {-100, 0, 10}).Result < 0, "behind: not pickable");
  // 5 m off at 100 m: 0.05 / tan(30 deg) * 270 = 23.4 px, minus min(18, 6) = 17.4 px.
  const f32 s = g.pick_score(eye, fwd, fov, 540, {100, 5, 10}).Result;
  SC_CHECK(std::fabs(s - 17.4f) < 0.5f, "off-axis pixels with the slack (%.2f)", s);
}

SC_TEST(details_critical_clocks) {
  DetailClass dc;
  CLASS_BOUND(dc, "combat_detail_cpp");
  Cd& cd = *dc;
  using Critical = Cd::detail_Critical;
  const auto is_critical = [&](Critical k, f64 hp, f64 max_hp) { return cd.is_critical(k, hp, max_hp).Result; };
  const auto detonation_at = [&](Critical k, f64 now) { return cd.detonation_at(k, now).Result; };
  const auto accelerate = [&](Critical k, f64 at, f64 now) { return cd.accelerate(k, at, now).Result; };
  /// detail::CriticalClock: its update and armed, on the Blueprint's struct.
  struct CriticalClock {
    Cd& cd;
    Cd::detail_CriticalClock c;
    f64 at = -1;
    bool update(Critical k, f64 hp, f64 max_hp, f64 now) {
      const bool fired = cd.CriticalClock_update(c, k, hp, max_hp, now).Result;
      at = c.at;
      return fired;
    }
    bool armed() { return cd.CriticalClock_armed(c).Result; }
  };
  SC_CHECK(is_critical(Critical::car, 60, 300) && !is_critical(Critical::car, 61, 300), "car critical at 20 %%");
  SC_CHECK(is_critical(Critical::police_drone, 34, 100) && !is_critical(Critical::police_drone, 35, 100), "police drone at 34 %%");
  SC_CHECK(std::fabs(detonation_at(Critical::bus, 10) - 13.1f) < 1e-4f, "bus blows 3.1 s later");
  SC_CHECK(std::fabs(accelerate(Critical::car, 12, 11) - 11.62f) < 1e-4f, "a hit brings it to 0.62 s");
  SC_CHECK(accelerate(Critical::car, 11.2f, 11) == 11.2f, "never later than already due");
  CriticalClock c{cd, {}};
  SC_CHECK(!c.update(Critical::drone, 100, 100, 0), "healthy");
  SC_CHECK(!c.update(Critical::drone, 28, 100, 1) && c.armed(), "28 %%: countdown armed");
  SC_CHECK(std::fabs(c.at - 3.05f) < 1e-4f, "2.05 s for the drone");
  SC_CHECK(!c.update(Critical::drone, 20, 100, 1.5f) && std::fabs(c.at - 2.04f) < 1e-4f, "hit again: 0.54 s");
  SC_CHECK(c.update(Critical::drone, 20, 100, 2.1f) && !c.armed(), "blows when due, once");
  CriticalClock r{cd, {}};
  r.update(Critical::drone, 20, 100, 0);
  r.update(Critical::drone, 100, 100, 1);
  SC_CHECK(!r.armed(), "a new (healthy) drone clears the countdown");
}

SC_TEST(details_bullet_momentum) {
  DetailClass dc;
  CLASS_BOUND(dc, "combat_detail_cpp");
  Cd& cd = *dc;
  using Round = Cd::detail_Round;
  const auto momentum_ns = [&](Round r) { return cd.momentum_ns(r).Result; };
  SC_CHECK(std::fabs(momentum_ns(Round::nine_mm) - 2.88f) < 1e-3f, "9 mm 2.88 N*s");
  SC_CHECK(std::fabs(momentum_ns(Round::fifty_bmg) - 40.762f) < 1e-2f, ".50 BMG ~41 N*s");
  const Vec3 j = cd.bullet_impulse({0, 3, 0}, Round::five_five_six).Result;
  SC_CHECK(std::fabs(j.y - 3.76f) < 1e-3f && j.x == 0 && j.z == 0, "along the path, no upward bias");
}

SC_TEST(details_combat_audio_bank) {
  // The bank is the level's Blueprint combat_audio_cpp (its kinds: combat_audio), called as its class - as the game
  // calls it (level_blueprints.h), with the game's loop guard.
  RuleBlueprint audio_rules("combat_audio_cpp", nullptr, true, {"combat_audio"});
  audio_rules.rt.budget = rules::kLoopBudget;
  SC_CHECK(audio_rules.problems.empty() && kern::logic::bind_blueprint<blueprints::combat_audio_cpp>(audio_rules.rt),
           "combat_audio_cpp runs as its class: %s", audio_rules.problems.c_str());
  auto& bank = kern::logic::blueprint<blueprints::combat_audio_cpp>();
  using Kind = blueprints::combat_audio::combat_audio_Kind;
  const auto render = [&](Kind k, u32 v) { return bank.render(k, v, blueprints::combat_audio_cpp::kSampleRate).Result; };
  // Lengths as SpawnCity's prebaked bank (ceil(duration * 44100)), every sample finite and in range.
  auto near = [](std::size_t n, std::size_t want) { return n + 1 >= want && n <= want + 1; };
  SC_CHECK(near(render(Kind::shot, 0).size(), 8820), "MP round 0.2 s (%zu)", render(Kind::shot, 0).size());
  SC_CHECK(near(render(Kind::pistol, 1).size(), 26108), "pistol variant 1: 0.592 s (%zu)", render(Kind::pistol, 1).size());
  for (u8 i = 0; i < static_cast<u8>(Kind::count); ++i)
    for (u32 v = 0; v < blueprints::combat_audio_cpp::kVariants[i]; ++v) {
      // (as the game hands them to the engine's bank: each sample made into 32 bit)
      std::vector<f32> pcm;
      for (const f64 x : render(static_cast<Kind>(i), v)) pcm.push_back(static_cast<f32>(x));
      f32 peak = 0;
      bool finite = !pcm.empty();
      for (f32 x : pcm) { finite = finite && std::isfinite(x); peak = std::fmax(peak, std::fabs(x)); }
      const std::string name(bank.kind_name(static_cast<Kind>(i)).Result.view());
      SC_CHECK(finite && peak > 0.3f && peak <= 1.0f, "%s %u: finite, normalised (peak %.2f)", name.c_str(), v, peak);
    }
}
