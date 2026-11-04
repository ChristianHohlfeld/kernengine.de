// Equipment: jetpack (player_walk_mode_v4.mjs), flight logbook (flight_logbook.mjs), procedural music.
#include <string>
#include <vector>

#include "../src/equipment.h"
#include "test_util.h"

using namespace sc;

SC_TEST(equipment_jetpack) {
  Jetpack j;
  SC_CHECK(j.update(0.1f, 0, false, 0, 0, false) == 0 && j.fuel == 1, "on the ground: nothing");
  SC_CHECK(j.ignite(1), "ignition in the air");
  // Simulate a vertical hop: thrust while jump is held.
  f32 z = 1, vz = 1, t = 0;
  for (int i = 0; i < 120; ++i) {  // 2 s
    const f32 acc = j.update(1.0f / 60, t, true, vz, z, true);
    vz += (acc - Jetpack::kGravity) / 60;
    z += vz / 60;
    t += 1.0f / 60;
  }
  SC_CHECK(z > 10 && vz <= Jetpack::kMaxClimb + 0.5f, "climbs over a house at most 8 m/s (z %.1f, vz %.1f)", z, vz);
  SC_CHECK(j.fuel < 0.75f && j.fuel > 0.4f, "2 s burn uses fuel (%.2f)", j.fuel);
  // Let go, fall: the cushion fires before touchdown.
  bool cushion = false;
  for (int i = 0; i < 600 && z > 0; ++i) {
    const f32 acc = j.update(1.0f / 60, t, true, vz, z, false);
    if (acc >= Jetpack::kAcc) cushion = true;
    vz += (acc - Jetpack::kGravity) / 60;
    z += vz / 60;
    t += 1.0f / 60;
  }
  SC_CHECK(cushion && vz > -Jetpack::kFallSafe, "the cushion catches the fall (%.1f m/s)", vz);
  const f32 f0 = j.fuel;
  for (int i = 0; i < 240; ++i) j.update(1.0f / 60, t + i / 60.0f, false, 0, 0, false);
  SC_CHECK(j.fuel > f0 + 0.8f || j.fuel == 1, "refills on the ground (%.2f -> %.2f)", f0, j.fuel);
  Jetpack empty;
  empty.fuel = 0.01f;
  SC_CHECK(!empty.ignite(0), "no ignition without fuel");
}

SC_TEST(equipment_flight_log) {
  FlightLog log;
  FlightSample s;
  s.volts = 16.4f;
  log.observe(s);
  SC_CHECK(!log.flying() && log.flights().empty(), "disarmed: no flight");
  s.armed = true;
  for (int i = 0; i <= 100; ++i) {
    s.t = i * 0.1f;
    s.pos = {i * 0.5f, 0, 2};
    s.vel = {5, 0, 0};
    s.agl = 2;
    s.volts = 16.4f - i * 0.01f;
    log.observe(s);
  }
  s.armed = false;
  log.observe(s);
  SC_CHECK(log.flights().size() == 1, "one flight");
  const Flight& f = log.flights()[0];
  SC_CHECK(std::fabs(f.duration - 10) < 0.15f && std::fabs(f.distance - 50) < 0.6f && std::fabs(f.max_speed - 5) < 1e-3f, "10 s, 50 m, 5 m/s (%.2f %.2f %.2f)", f.duration, f.distance, f.max_speed);
  SC_CHECK(f.path.size() >= 20 && f.path.size() <= 22 && f.reason == "FC_DISARM", "path every 0.5 s (%zu)", f.path.size());
  SC_CHECK(f.volts_min < f.volts_start, "battery sag");
  log.network("peer-join", 3);
  const std::string j = log.json();
  SC_CHECK(j.find("\"flights\":[{\"id\":1") != std::string::npos && j.find("peer-join") != std::string::npos, "json");
}

SC_TEST(equipment_music) {
  MusicSynth m;
  m.init(48000);
  std::vector<f32> buf(2 * 4800);
  m.select(MusicSynth::kMenu, 1);
  f32 peak = 0;
  for (int i = 0; i < 40; ++i) { std::fill(buf.begin(), buf.end(), 0.0f); m.render_add(buf.data(), 4800); peak = std::fmax(peak, m.peak); }
  SC_CHECK(peak > 0.05f && peak < 1.2f, "the title music plays (peak %.3f)", peak);
  m.select(2, 1);
  for (int i = 0; i < 20; ++i) m.render_add(buf.data(), 4800);
  SC_CHECK(m.programme() == 2, "the radio station takes over (%d)", m.programme());
  m.select(-2, 0);
  for (int i = 0; i < 20; ++i) { std::fill(buf.begin(), buf.end(), 0.0f); m.render_add(buf.data(), 4800); }
  SC_CHECK(m.peak == 0, "radio off: silence");
  SC_CHECK(MusicSynth::station_count() == 5, "five stations");
}
