// The settings table: SpawnCity's defaults and ranges, the "set" parser.
#include <string>

#include "../src/settings.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

SC_TEST(settings_defaults_and_ranges) {
  RuleBlueprint bp("Settings", [](kern::logic::Instance* i) { bind_settings(i); });
  SC_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Settings Blueprint compiles and runs: %s", bp.problems.c_str());
  SC_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  SC_CHECK(bind_settings(bp.inst), "its table has one row per setting");
  Settings s;
  SC_CHECK(s.get(Opt::mouse_sensitivity) == 100 && !s.on(Opt::mouse_invert_y), "mouse 100 %%, not inverted");
  SC_CHECK(s.get(Opt::drone_max_speed_kmh) == 90 && std::fabs(s.get(Opt::drone_hover_agl) - 1.2f) < 1e-6f, "drone 90 km/h, hover 1.2 m");
  SC_CHECK(s.get(Opt::car_max_kmh) == 115 && s.get(Opt::car_reverse_kmh) == 32, "car 115 / 32 km/h");
  SC_CHECK(s.get(Opt::cam_fpv_tilt) == -15 && s.get(Opt::cam_fov) == 105, "camera -15 deg, 105 deg");
  SC_CHECK(s.get(Opt::audio_drone) == 50 && s.get(Opt::audio_vehicle) == 58 && s.get(Opt::music_volume) == 32, "audio mix v5");
  SC_CHECK(s.on(Opt::world_jetpack) && s.on(Opt::world_friendly_fire) && !s.on(Opt::world_terrain) && !s.on(Opt::world_flak) && s.on(Opt::world_guided), "world options: jetpack, friendly fire, guided on; terrain, flak off");
  const u32 v = s.version();
  SC_CHECK(s.set("mouse.sensitivity", "999") && s.get(Opt::mouse_sensitivity) == 300, "clamped to 300");
  SC_CHECK(s.set("mouse.sensitivity", "47") && s.get(Opt::mouse_sensitivity) == 45, "snapped to the 5 %% step (%g)", s.get(Opt::mouse_sensitivity));
  SC_CHECK(s.version() > v, "version counts changes");
  SC_CHECK(s.set("world.people", "false") && !s.on(Opt::world_people), "toggle off");
  SC_CHECK(!s.set("no.such", "1") && !s.set("car.maxKmh", "fast"), "unknown key / no number");
  s.reset_section("maus");
  SC_CHECK(s.get(Opt::mouse_sensitivity) == 100 && !s.on(Opt::world_people), "section reset keeps the others");
  const std::string schema = s.schema_json();
  SC_CHECK(schema.find("\"key\":\"world.jetpack\"") != std::string::npos && schema.front() == '[' && schema.back() == ']', "schema lists every row");
}
