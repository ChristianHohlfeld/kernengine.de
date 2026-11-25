// Performance and look: the frame-time meter, the automatic quality ladder (perf), camera shake,
// the FPV stabilizer, speed streaks, the day clock and the light helpers (look). Both are the level's
// Blueprints perf_cpp and look_cpp, called as their classes - as the program calls them (level_blueprints.h).
// The start levels and the ladder's table (perf.h kDesktopStart, kMobileStart, kMobileMax, kTier) stand in no
// class yet: perf_governor_start_levels checks them on the C++ until the level carries the whole header.
#include "../src/blueprint_call.h"
#include "../src/perf.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"

#include <cstdlib>
#include <string>

using namespace sc;

namespace {
using Pf = blueprints::perf_cpp;
using Lk = blueprints::look_cpp;
/// A Blueprint of the level running as its class in a logic of its own.
template <class T>
struct ClassBlueprint {
  RuleBlueprint rules;
  bool bound = false;
  explicit ClassBlueprint(const char* actor) : rules(actor, nullptr, true) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
/// Runs the governor at a steady frame rate for `seconds`; returns how many levels it changed.
int run(Pf& p, Pf::perf_Governor& g, f32 fps, f32 seconds) {
  int changes = 0;
  for (f32 t = 0; t < seconds; t += 1 / fps) changes += p.Governor_update(g, 1 / fps).Result ? 1 : 0;
  return changes;
}
/// The same on the C++ governor (perf.h), for the start levels that stand in no class yet.
int run(perf::Governor& g, f32 fps, f32 seconds) {
  int changes = 0;
  for (f32 t = 0; t < seconds; t += 1 / fps) changes += g.update(1 / fps) ? 1 : 0;
  return changes;
}
}  // namespace

SC_TEST(perf_meter_laps) {
  ClassBlueprint<Pf> b("perf_cpp");
  SC_CHECK(b.bound, "perf_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Pf& p = *b;
  using Slot = Pf::perf_Slot;
  Pf::perf_Meter m;
  for (int i = 0; i < 200; ++i) {
    const f64 t0 = i * 20.0;
    p.Meter_begin_frame(m, t0);
    p.Meter_lap(m, Slot::logic, t0 + 1);
    p.Meter_lap(m, Slot::physics, t0 + 4);
    p.Meter_lap(m, Slot::stream, t0 + 4.5);
    p.Meter_lap(m, Slot::logic, t0 + 5.5);
    p.Meter_lap(m, Slot::draw, t0 + 7);
    p.Meter_end_frame(m, t0 + 7, 3, -1);
  }
  const f64 logic = p.Meter_ms(m, Slot::logic).Result;
  SC_CHECK(std::fabs(logic - 2) < 0.01f, "logic 2 ms (two laps), got %.3f", logic);
  SC_CHECK(std::fabs(p.Meter_ms(m, Slot::physics).Result - 3) < 0.01f && std::fabs(p.Meter_ms(m, Slot::stream).Result - 0.5f) < 0.01f, "physics 3, stream 0.5");
  SC_CHECK(std::fabs(p.Meter_update_ms(m).Result - 7) < 0.01f && std::fabs(p.Meter_render_ms(m).Result - 3) < 0.01f && p.Meter_gpu_ms(m).Result < 0, "update 7, render 3, no gpu timer");
}

SC_TEST(perf_governor_steps_down_and_up) {
  ClassBlueprint<Pf> b("perf_cpp");
  SC_CHECK(b.bound, "perf_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Pf& p = *b;
  Pf::perf_Governor g;
  auto tier = [&] { return static_cast<int>(p.Governor_tier(g).Result); };
  p.Governor_reset(g, Pf::kTiers - 1, Pf::kTiers - 1);
  SC_CHECK(run(p, g, 60, 20) == 0 && tier() == 4, "60 fps keeps the top level");
  // 40 fps: after warm-up / settle + 2 s below 50, one level down; then each settles before the next.
  p.Governor_reset(g, 4, 4);
  run(p, g, 40, static_cast<f32>(Pf::kWarmup + Pf::kDownHold - 0.5f));
  SC_CHECK(tier() == 4, "no decision inside warm-up + hold (tier %d)", tier());
  run(p, g, 40, 1.5f);
  SC_CHECK(tier() == 3, "one level down after holding below 50 fps (tier %d)", tier());
  run(p, g, 40, 30);
  SC_CHECK(tier() == 0, "keeps stepping to the floor at 40 fps (tier %d)", tier());
  // Reserve again: up one level after 8 s at 57+, but the level that failed stays blocked.
  const f64 blocked = p.Governor_blocked(g, 1).Result;
  SC_CHECK(blocked > 0, "a level that was too heavy is blocked for a while (%.1f s)", blocked);
  run(p, g, 60, 25);
  SC_CHECK(tier() >= 1 && tier() < 4, "climbs again with reserve, a level at a time (tier %d)", tier());
  run(p, g, 60, 400);
  SC_CHECK(tier() == 4, "climbs back once the blocks run out (tier %d)", tier());
  // A ceiling (perf_governor_start_levels: the phones'), a fixed level.
  p.Governor_reset(g, 2, 3);
  run(p, g, 60, 200);
  SC_CHECK(tier() == 3, "it stops at its ceiling (tier %d)", tier());
  p.Governor_set_fixed(g, 1);
  run(p, g, 20, 30);
  SC_CHECK(tier() == 1 && !p.Governor_automatic(g).Result, "a fixed level is not changed");
  // Single long frames (a hitch) do not drop a level.
  p.Governor_reset(g, 4, 4);
  run(p, g, 60, 5);
  for (int i = 0; i < 40; ++i) { p.Governor_update(g, 1.0f / 60); if (i % 10 == 0) p.Governor_update(g, 0.1f); }
  SC_CHECK(tier() == 4, "hitches alone keep the level (tier %d, %.1f fps)", tier(), static_cast<f64>(p.Governor_fps(g).Result));
}

// Runs on the C++ (perf.h) until the level carries the whole header: kDesktopStart, kMobileStart, kMobileMax and the
// ladder kTier stand in no class.
SC_TEST(perf_governor_start_levels) {
  perf::Governor g;
  g.reset(perf::kDesktopStart, perf::kTiers - 1);
  SC_CHECK(run(g, 60, 20) == 0 && g.tier() == 4, "60 fps keeps the top level");
  g.reset(perf::kMobileStart, perf::kMobileMax);
  run(g, 60, 200);
  SC_CHECK(g.tier() == perf::kMobileMax, "phones stop at their ceiling (tier %d)", g.tier());
  // Tiers fall monotonically in cost.
  bool mono = true;
  for (int i = 1; i < perf::kTiers; ++i)
    mono = mono && perf::kTier[i].people >= perf::kTier[i - 1].people && perf::kTier[i].shadow_m >= perf::kTier[i - 1].shadow_m &&
           perf::kTier[i].render_scale >= perf::kTier[i - 1].render_scale && perf::kTier[i].lights >= perf::kTier[i - 1].lights;
  SC_CHECK(mono && perf::kTier[4].people == 80 && perf::kTier[2].people == 44 && perf::kTier[4].shadow_m == 110 && perf::kTier[2].shadow_m == 70,
           "ladder from SpawnCity's desktop (80 / 110 m) and phone (44 / 70 m) values");
}

SC_TEST(look_camera_shake) {
  ClassBlueprint<Lk> b("look_cpp");
  SC_CHECK(b.bound, "look_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Lk& lk = *b;
  Lk::look_Shake s;
  f64 cap = 0;
  const f64 smg = lk.weapon_trauma(cap, true, false, false).Result;
  SC_CHECK(std::fabs(smg - 0.035f) < 1e-6f && std::fabs(cap - 0.42f) < 1e-6f, "full auto 0.035 capped at 0.42");
  for (int i = 0; i < 40; ++i) lk.Shake_add(s, smg, cap);
  SC_CHECK(std::fabs(s.trauma - 0.42f) < 1e-5f, "sustained fire builds a steady rumble (%.3f)", static_cast<f64>(s.trauma));
  Vec3 a, o;
  SC_CHECK(lk.Shake_sample(s, a, o, 1.0f / 60, 1.234, 1).Result && std::fabs(a.x) <= Lk::kMaxAngle && std::fabs(o.x) <= Lk::kMaxOffset, "bounded offsets");
  for (int i = 0; i < 60; ++i) lk.Shake_sample(s, a, o, 1.0f / 60, i / 60.0, 1);
  SC_CHECK(s.trauma == 0, "decays at 1.35 / s");
  SC_CHECK(lk.blast_trauma(5, 9.5f).Result > 0.5f && lk.blast_trauma(200, 9.5f).Result == 0, "near blast punches, far one nothing");
  SC_CHECK(lk.impact_trauma(2).Result == 0 && lk.impact_trauma(30).Result == 0.7f, "impacts from 2.5 m/s, capped 0.7");
  Lk::Camera c;
  c.eye = {0, 0, 2};
  c.target = {10, 0, 2};
  c.up = {0, 0, 1};
  lk.shake_camera(c, {0.05f, 0, 0}, {0.1f, 0, 0});
  SC_CHECK(c.target.z > 2.0f && std::fabs(c.eye.y + 0.1f) < 1e-4f, "pitch tilts the view up, offset moves right (-y when looking +x)");
}

SC_TEST(look_fpv_stabilizer) {
  ClassBlueprint<Lk> b("look_cpp");
  SC_CHECK(b.bound, "look_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Lk& lk = *b;
  Lk::look_Stabilizer st;
  Lk::Camera c;
  c.eye = {0, 0, 10};
  c.target = {4, 0, 10};
  c.up = {0, 0, 1};
  lk.Stabilizer_apply(st, c, 1.0f / 60);
  // Frame jitter of 2 cm: the shown eye moves only a part of it.
  Lk::Camera j = c;
  j.eye = {0, 0.02f, 10};
  j.target = {4, 0.02f, 10};
  lk.Stabilizer_apply(st, j, 1.0f / 240);
  SC_CHECK(j.eye.y > 0 && j.eye.y < 0.02f, "jitter filtered (%.4f)", static_cast<f64>(j.eye.y));
  Lk::Camera far = c;
  far.eye = {5, 0, 10};
  far.target = {9, 0, 10};
  lk.Stabilizer_apply(st, far, 1.0f / 60);
  SC_CHECK(far.eye.x == 5, "a jump beyond 0.35 m teleports");
}

SC_TEST(look_speed_lines) {
  ClassBlueprint<Lk> b("look_cpp");
  SC_CHECK(b.bound, "look_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Lk& lk = *b;
  Lk::look_SpeedLines l;
  Vec3 p{0, 0, 0};
  for (int i = 0; i < 120; ++i) lk.SpeedLines_update(l, p += Vec3{5.0f / 60, 0, 0}, 1.0f / 60);
  SC_CHECK(l.k == 0, "5 m/s: no streaks");
  for (int i = 0; i < 240; ++i) lk.SpeedLines_update(l, p += Vec3{45.0f / 60, 0, 0}, 1.0f / 60);
  SC_CHECK(l.k > 0.95f, "45 m/s: full streaks (%.2f)", static_cast<f64>(l.k));
  bool inside = true;
  for (const auto& s : l.s) inside = inside && s.r >= 0.15f && s.r <= 1.15f;
  SC_CHECK(inside, "streaks respawn at the centre");
  Lk::look_SpeedLines t;
  lk.SpeedLines_update(t, {0, 0, 0}, 0.1f);
  lk.SpeedLines_update(t, {200, 0, 0}, 0.1f);
  SC_CHECK(t.k == 0 && t.speed == 0, "a teleport is no speed");
}

SC_TEST(look_day_clock) {
  ClassBlueprint<Lk> b("look_cpp");
  SC_CHECK(b.bound, "look_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Lk& lk = *b;
  Lk::Lighting l;
  Lk::SkyState sky;
  const f64 noon = lk.day_lighting(l, sky, 0.5f, 210, 0.00085f, 1).Result;
  SC_CHECK(noon == 0 && l.sun_direction.z > 0.5f, "noon: day, sun high");
  const f64 midnight = lk.day_lighting(l, sky, 0.0f, 210, 0.00085f, 1).Result;
  SC_CHECK(midnight == 1 && sky.night == 1 && l.exposure > 1.3f, "midnight: night");
  f64 ph = -1;
  auto parse = [&](const char* s) { return static_cast<bool>(lk.parse_time(ph, std::string(s)).Result); };
  SC_CHECK(parse("21:30") && std::fabs(ph - 21.5f / 24) < 1e-5f, "21:30");
  SC_CHECK(parse("0.75") && ph == 0.75f && parse("6") && std::fabs(ph - 0.25f) < 1e-6f, "phase and hours");
  SC_CHECK(!parse("abc") && !parse("25:00"), "rejects nonsense");
  SC_CHECK(lk.follow_phase(0.5f, 0.5003f).Result == 0.5f, "tiny drift ignored");
  SC_CHECK(std::fabs(lk.follow_phase(0.5f, 0.506f).Result - 0.503f) < 1e-5f, "small drift eased by half");
  SC_CHECK(std::fabs(lk.follow_phase(0.98f, 0.02f).Result - 0.02f) < 1e-5f, "big drift jumps across midnight");
}

SC_TEST(look_lights) {
  ClassBlueprint<Lk> b("look_cpp");
  SC_CHECK(b.bound, "look_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Lk& lk = *b;
  // A SpawnCity street lamp (46, 26 m) lights the street 5.75 m below like three.js does.
  const f64 k = lk.Kern_intensity(46, 26, 5.75f).Result;
  const f32 d2 = 5.75f * 5.75f, w = 1 - d2 / (26.0f * 26), att = w * w / (1 + d2 / (26.0f * 26));
  SC_CHECK(std::fabs(k * 0.12f * att - 46 / (kPi * d2)) < 1e-4f, "equal light at the reference distance (%.2f)", k);
  SC_CHECK(lk.Kern_intensity(10, 5, 6).Result == 0, "beyond its range: nothing");
  const Vec3 c = lk.hex_linear(0xffffff).Result;
  SC_CHECK(std::fabs(c.x - 1) < 1e-5f && lk.hex_linear(0x808080).Result.x < 0.25f, "sRGB to linear");
  int red = 0, blue = 0;
  for (int i = 0; i < 660; ++i) { const int f = static_cast<int>(lk.police_flash(i / 1000.0f).Result); red += f == 1; blue += f == 2; }
  SC_CHECK(std::abs(red - 220) <= 2 && std::abs(blue - 220) <= 2, "light bar: two red and two blue slots of 110 ms per 660 ms (%d / %d)", red, blue);
  SC_CHECK(lk.drone_flash(0, 0).Result == 1 && lk.drone_flash(0.4f, 0).Result == 2, "drone flasher red then blue");
  SC_CHECK(lk.lamp_glow(0.1f).Result == 0 && lk.lamp_glow(1).Result == 1, "lamps from dusk");
}
