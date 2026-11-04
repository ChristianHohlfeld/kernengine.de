// Performance and look: the frame-time meter, the automatic quality ladder (perf.h), camera shake,
// the FPV stabilizer, speed streaks, the day clock and the light helpers (look.h).
#include "../src/look.h"
#include "../src/perf.h"
#include "test_util.h"

#include <cstdlib>

using namespace sc;

namespace {
/// Runs the governor at a steady frame rate for `seconds`; returns how many levels it changed.
int run(perf::Governor& g, f32 fps, f32 seconds) {
  int changes = 0;
  for (f32 t = 0; t < seconds; t += 1 / fps) changes += g.update(1 / fps) ? 1 : 0;
  return changes;
}
}  // namespace

SC_TEST(perf_meter_laps) {
  perf::Meter m;
  for (int i = 0; i < 200; ++i) {
    const f64 t0 = i * 20.0;
    m.begin_frame(t0);
    m.lap(perf::Slot::logic, t0 + 1);
    m.lap(perf::Slot::physics, t0 + 4);
    m.lap(perf::Slot::stream, t0 + 4.5);
    m.lap(perf::Slot::logic, t0 + 5.5);
    m.lap(perf::Slot::draw, t0 + 7);
    m.end_frame(t0 + 7, 3, -1);
  }
  SC_CHECK(std::fabs(m.ms(perf::Slot::logic) - 2) < 0.01f, "logic 2 ms (two laps), got %.3f", m.ms(perf::Slot::logic));
  SC_CHECK(std::fabs(m.ms(perf::Slot::physics) - 3) < 0.01f && std::fabs(m.ms(perf::Slot::stream) - 0.5f) < 0.01f, "physics 3, stream 0.5");
  SC_CHECK(std::fabs(m.update_ms() - 7) < 0.01f && std::fabs(m.render_ms() - 3) < 0.01f && m.gpu_ms() < 0, "update 7, render 3, no gpu timer");
}

SC_TEST(perf_governor_steps_down_and_up) {
  perf::Governor g;
  g.reset(perf::kDesktopStart, perf::kTiers - 1);
  SC_CHECK(run(g, 60, 20) == 0 && g.tier() == 4, "60 fps keeps the top level");
  // 40 fps: after warm-up / settle + 2 s below 50, one level down; then each settles before the next.
  g.reset(4, 4);
  run(g, 40, perf::kWarmup + perf::kDownHold - 0.5f);
  SC_CHECK(g.tier() == 4, "no decision inside warm-up + hold (tier %d)", g.tier());
  run(g, 40, 1.5f);
  SC_CHECK(g.tier() == 3, "one level down after holding below 50 fps (tier %d)", g.tier());
  run(g, 40, 30);
  SC_CHECK(g.tier() == 0, "keeps stepping to the floor at 40 fps (tier %d)", g.tier());
  // Reserve again: up one level after 8 s at 57+, but the level that failed stays blocked.
  SC_CHECK(g.blocked(1) > 0, "a level that was too heavy is blocked for a while (%.1f s)", g.blocked(1));
  run(g, 60, 25);
  SC_CHECK(g.tier() >= 1 && g.tier() < 4, "climbs again with reserve, a level at a time (tier %d)", g.tier());
  run(g, 60, 400);
  SC_CHECK(g.tier() == 4, "climbs back once the blocks run out (tier %d)", g.tier());
  // A mobile ceiling, a fixed level.
  g.reset(perf::kMobileStart, perf::kMobileMax);
  run(g, 60, 200);
  SC_CHECK(g.tier() == perf::kMobileMax, "phones stop at their ceiling (tier %d)", g.tier());
  g.set_fixed(1);
  run(g, 20, 30);
  SC_CHECK(g.tier() == 1 && !g.automatic(), "a fixed level is not changed");
  // Single long frames (a hitch) do not drop a level.
  g.reset(4, 4);
  run(g, 60, 5);
  for (int i = 0; i < 40; ++i) { g.update(1.0f / 60); if (i % 10 == 0) g.update(0.1f); }
  SC_CHECK(g.tier() == 4, "hitches alone keep the level (tier %d, %.1f fps)", g.tier(), g.fps());
  // Tiers fall monotonically in cost.
  bool mono = true;
  for (int i = 1; i < perf::kTiers; ++i)
    mono = mono && perf::kTier[i].people >= perf::kTier[i - 1].people && perf::kTier[i].shadow_m >= perf::kTier[i - 1].shadow_m &&
           perf::kTier[i].render_scale >= perf::kTier[i - 1].render_scale && perf::kTier[i].lights >= perf::kTier[i - 1].lights;
  SC_CHECK(mono && perf::kTier[4].people == 80 && perf::kTier[2].people == 44 && perf::kTier[4].shadow_m == 110 && perf::kTier[2].shadow_m == 70,
           "ladder from SpawnCity's desktop (80 / 110 m) and phone (44 / 70 m) values");
}

SC_TEST(look_camera_shake) {
  look::Shake s;
  f32 cap = 0;
  const f32 smg = look::weapon_trauma(true, false, false, cap);
  SC_CHECK(std::fabs(smg - 0.035f) < 1e-6f && std::fabs(cap - 0.42f) < 1e-6f, "full auto 0.035 capped at 0.42");
  for (int i = 0; i < 40; ++i) s.add(smg, cap);
  SC_CHECK(std::fabs(s.trauma - 0.42f) < 1e-5f, "sustained fire builds a steady rumble (%.3f)", s.trauma);
  Vec3 a;
  Vec2 o;
  SC_CHECK(s.sample(1.0f / 60, 1.234, 1, a, o) && std::fabs(a.x) <= look::Shake::kMaxAngle && std::fabs(o.x) <= look::Shake::kMaxOffset, "bounded offsets");
  for (int i = 0; i < 60; ++i) s.sample(1.0f / 60, i / 60.0, 1, a, o);
  SC_CHECK(s.trauma == 0, "decays at 1.35 / s");
  SC_CHECK(look::blast_trauma(5, 9.5f) > 0.5f && look::blast_trauma(200, 9.5f) == 0, "near blast punches, far one nothing");
  SC_CHECK(look::impact_trauma(2) == 0 && look::impact_trauma(30) == 0.7f, "impacts from 2.5 m/s, capped 0.7");
  kern::Camera c;
  c.eye = {0, 0, 2};
  c.target = {10, 0, 2};
  c.up = {0, 0, 1};
  look::shake_camera(c, {0.05f, 0, 0}, {0.1f, 0});
  SC_CHECK(c.target.z > 2.0f && std::fabs(c.eye.y + 0.1f) < 1e-4f, "pitch tilts the view up, offset moves right (-y when looking +x)");
}

SC_TEST(look_fpv_stabilizer) {
  look::Stabilizer st;
  kern::Camera c;
  c.eye = {0, 0, 10};
  c.target = {4, 0, 10};
  c.up = {0, 0, 1};
  st.apply(c, 1.0f / 60);
  // Frame jitter of 2 cm: the shown eye moves only a part of it.
  kern::Camera j = c;
  j.eye = {0, 0.02f, 10};
  j.target = {4, 0.02f, 10};
  st.apply(j, 1.0f / 240);
  SC_CHECK(j.eye.y > 0 && j.eye.y < 0.02f, "jitter filtered (%.4f)", j.eye.y);
  kern::Camera far = c;
  far.eye = {5, 0, 10};
  far.target = {9, 0, 10};
  st.apply(far, 1.0f / 60);
  SC_CHECK(far.eye.x == 5, "a jump beyond 0.35 m teleports");
}

SC_TEST(look_speed_lines) {
  look::SpeedLines l;
  Vec3 p{0, 0, 0};
  for (int i = 0; i < 120; ++i) l.update(p += Vec3{5.0f / 60, 0, 0}, 1.0f / 60);
  SC_CHECK(l.k == 0, "5 m/s: no streaks");
  for (int i = 0; i < 240; ++i) l.update(p += Vec3{45.0f / 60, 0, 0}, 1.0f / 60);
  SC_CHECK(l.k > 0.95f, "45 m/s: full streaks (%.2f)", l.k);
  bool inside = true;
  for (const auto& s : l.s) inside = inside && s.r >= 0.15f && s.r <= 1.15f;
  SC_CHECK(inside, "streaks respawn at the centre");
}

SC_TEST(look_day_clock) {
  kern::Lighting l;
  kern::SkyState sky;
  const f32 noon = look::day_lighting(0.5f, 210, 0.00085f, 1, l, sky);
  SC_CHECK(noon == 0 && l.sun_direction.z > 0.5f, "noon: day, sun high");
  const f32 midnight = look::day_lighting(0.0f, 210, 0.00085f, 1, l, sky);
  SC_CHECK(midnight == 1 && sky.night == 1 && l.exposure > 1.3f, "midnight: night");
  f32 ph = -1;
  SC_CHECK(look::parse_time("21:30", ph) && std::fabs(ph - 21.5f / 24) < 1e-5f, "21:30");
  SC_CHECK(look::parse_time("0.75", ph) && ph == 0.75f && look::parse_time("6", ph) && std::fabs(ph - 0.25f) < 1e-6f, "phase and hours");
  SC_CHECK(!look::parse_time("abc", ph) && !look::parse_time("25:00", ph), "rejects nonsense");
  SC_CHECK(look::follow_phase(0.5f, 0.5003f) == 0.5f, "tiny drift ignored");
  SC_CHECK(std::fabs(look::follow_phase(0.5f, 0.506f) - 0.503f) < 1e-5f, "small drift eased by half");
  SC_CHECK(std::fabs(look::follow_phase(0.98f, 0.02f) - 0.02f) < 1e-5f, "big drift jumps across midnight");
}

SC_TEST(look_lights) {
  // A SpawnCity street lamp (46, 26 m) lights the street 5.75 m below like three.js does.
  const f32 k = look::kern_intensity(46, 26, 5.75f);
  const f32 d2 = 5.75f * 5.75f, w = 1 - d2 / (26.0f * 26), att = w * w / (1 + d2 / (26.0f * 26));
  SC_CHECK(std::fabs(k * 0.12f * att - 46 / (kPi * d2)) < 1e-4f, "equal light at the reference distance (%.2f)", k);
  SC_CHECK(look::kern_intensity(10, 5, 6) == 0, "beyond its range: nothing");
  const Vec3 c = look::hex_linear(0xffffff);
  SC_CHECK(std::fabs(c.x - 1) < 1e-5f && look::hex_linear(0x808080).x < 0.25f, "sRGB to linear");
  int red = 0, blue = 0;
  for (int i = 0; i < 660; ++i) { const int f = look::police_flash(i / 1000.0f); red += f == 1; blue += f == 2; }
  SC_CHECK(std::abs(red - 220) <= 2 && std::abs(blue - 220) <= 2, "light bar: two red and two blue slots of 110 ms per 660 ms (%d / %d)", red, blue);
  SC_CHECK(look::drone_flash(0, 0) == 1 && look::drone_flash(0.4f, 0) == 2, "drone flasher red then blue");
  SC_CHECK(look::lamp_glow(0.1f) == 0 && look::lamp_glow(1) == 1, "lamps from dusk");
}
