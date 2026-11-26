// Equipment: jetpack (player_walk_mode_v4.mjs), flight logbook (flight_logbook.mjs), procedural music.
// They are the level's Blueprint equipment_cpp (its structs: equipment), called as its class - as the program
// calls it (level_blueprints.h), with the game's loop guard; the jetpack's rules are the Blueprint Jetpack.
#include <memory>
#include <string>
#include <vector>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Eq = blueprints::equipment_cpp;
/// equipment_cpp running as its class, beside the level's rule actors it calls (`with`).
struct EquipmentBlueprint {
  kern::test::BlueprintRun rules;
  bool bound = false;
  explicit EquipmentBlueprint(const char* with = nullptr) : rules(with, nullptr, true, {"equipment_cpp"}) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Eq>(rules.rt);
  }
  Eq& operator*() { return kern::logic::blueprint<Eq>(); }
};
}  // namespace

KERN_TEST(equipment_jetpack) {
  EquipmentBlueprint e("Jetpack");
  kern::test::BlueprintRun& bp = e.rules;
  KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Jetpack Blueprint compiles and runs: %s", bp.problems.c_str());
  KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  KERN_CHECK(e.bound, "equipment_cpp runs as its class");
  if (!e.bound) return;
  Eq& eq = *e;
  KERN_CHECK(eq.Jetpack_bind().Result, "the game finds the Jetpack's Blueprint");
  const f64 gravity = eq.Jetpack_gravity().Result, max_climb = eq.Jetpack_max_climb().Result, acc_full = eq.Jetpack_acc().Result, fall_safe = eq.Jetpack_fall_safe().Result;
  Eq::Jetpack j;
  KERN_CHECK(eq.Jetpack_update(j, 0.1, 0, false, 0, 0, false).Result == 0 && j.fuel == 1, "on the ground: nothing");
  KERN_CHECK(eq.Jetpack_ignite(j, 1).Result, "ignition in the air");
  // Simulate a vertical hop: thrust while jump is held.
  f64 z = 1, vz = 1, t = 0;
  for (int i = 0; i < 120; ++i) {  // 2 s
    const f64 acc = eq.Jetpack_update(j, 1.0 / 60, t, true, vz, z, true).Result;
    vz += (acc - gravity) / 60;
    z += vz / 60;
    t += 1.0 / 60;
  }
  KERN_CHECK(z > 10 && vz <= max_climb + 0.5, "climbs over a house at most 8 m/s (z %.1f, vz %.1f)", z, vz);
  KERN_CHECK(j.fuel < 0.75 && j.fuel > 0.4, "2 s burn uses fuel (%.2f)", static_cast<f64>(j.fuel));
  // Let go, fall: the cushion fires before touchdown.
  bool cushion = false;
  for (int i = 0; i < 600 && z > 0; ++i) {
    const f64 acc = eq.Jetpack_update(j, 1.0 / 60, t, true, vz, z, false).Result;
    if (acc >= acc_full) cushion = true;
    vz += (acc - gravity) / 60;
    z += vz / 60;
    t += 1.0 / 60;
  }
  KERN_CHECK(cushion && vz > -fall_safe, "the cushion catches the fall (%.1f m/s)", vz);
  const f64 f0 = j.fuel;
  for (int i = 0; i < 240; ++i) eq.Jetpack_update(j, 1.0 / 60, t + i / 60.0, false, 0, 0, false);
  KERN_CHECK(j.fuel > f0 + 0.8 || j.fuel == 1, "refills on the ground (%.2f -> %.2f)", f0, static_cast<f64>(j.fuel));
  Eq::Jetpack empty;
  empty.fuel = 0.01;
  KERN_CHECK(!eq.Jetpack_ignite(empty, 0).Result, "no ignition without fuel");
}

KERN_TEST(equipment_flight_log) {
  EquipmentBlueprint e;
  KERN_CHECK(e.bound, "equipment_cpp runs as its class: %s", e.rules.problems.c_str());
  if (!e.bound) return;
  Eq& eq = *e;
  Eq::FlightLog log;
  Eq::FlightSample s;
  s.volts = 16.4;
  eq.FlightLog_observe(log, s);
  KERN_CHECK(!eq.FlightLog_flying(log).Result && log.flights_.empty(), "disarmed: no flight");
  s.armed = true;
  for (int i = 0; i <= 100; ++i) {
    s.t = i * 0.1;
    s.pos = {i * 0.5f, 0, 2};
    s.vel = {5, 0, 0};
    s.agl = 2;
    s.volts = 16.4 - i * 0.01;
    eq.FlightLog_observe(log, s);
  }
  s.armed = false;
  eq.FlightLog_observe(log, s);
  KERN_CHECK(log.flights_.size() == 1, "one flight");
  if (log.flights_.size() != 1) return;
  const Eq::Flight& f = log.flights_[0];
  KERN_CHECK(std::fabs(f.duration - 10) < 0.15 && std::fabs(f.distance - 50) < 0.6 && std::fabs(f.max_speed - 5) < 1e-3, "10 s, 50 m, 5 m/s (%.2f %.2f %.2f)",
           static_cast<f64>(f.duration), static_cast<f64>(f.distance), static_cast<f64>(f.max_speed));
  KERN_CHECK(f.path.size() >= 20 && f.path.size() <= 22 && f.reason.str() == "FC_DISARM", "path every 0.5 s (%zu)", f.path.size());
  KERN_CHECK(f.volts_min < f.volts_start, "battery sag");
  eq.FlightLog_network(log, kern::logic::Text("peer-join"), 3);
  const std::string j = eq.FlightLog_json(log).Result.str();
  KERN_CHECK(j.find("\"flights\":[{\"id\":1") != std::string::npos && j.find("peer-join") != std::string::npos, "json");
}

KERN_TEST(equipment_music) {
  EquipmentBlueprint e;
  KERN_CHECK(e.bound, "equipment_cpp runs as its class: %s", e.rules.problems.c_str());
  if (!e.bound) return;
  Eq& eq = *e;
  auto m = std::make_unique<Eq::MusicSynth>();   // (its echo: 24000 numbers)
  eq.MusicSynth_init(*m, 48000);
  Eq::List<Eq::Float> buf(std::vector<f64>(2 * 4800));
  eq.MusicSynth_select(*m, Eq::kMenu, 1);
  f64 peak = 0;
  for (int i = 0; i < 40; ++i) { std::fill(buf.begin(), buf.end(), 0.0); eq.MusicSynth_render_add(*m, buf, 4800); peak = std::fmax(peak, m->peak); }
  KERN_CHECK(peak > 0.05 && peak < 1.2, "the title music plays (peak %.3f)", peak);
  eq.MusicSynth_select(*m, 2, 1);
  for (int i = 0; i < 20; ++i) eq.MusicSynth_render_add(*m, buf, 4800);
  KERN_CHECK(eq.MusicSynth_programme(*m).Result == 2, "the radio station takes over (%d)", static_cast<int>(eq.MusicSynth_programme(*m).Result));
  eq.MusicSynth_select(*m, -2, 0);
  for (int i = 0; i < 20; ++i) { std::fill(buf.begin(), buf.end(), 0.0); eq.MusicSynth_render_add(*m, buf, 4800); }
  KERN_CHECK(m->peak == 0, "radio off: silence");
  KERN_CHECK(eq.MusicSynth_station_count().Result == 5, "five stations");
}
