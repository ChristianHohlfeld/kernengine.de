// The settings table: SpawnCity's defaults and ranges, the "set" parser. The table is the level's Blueprint
// Settings; the settings themselves are the Blueprint settings_cpp, called as its class - as the program
// calls it (level_blueprints.h).
#include <string>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

KERN_TEST(settings_defaults_and_ranges) {
  kern::test::BlueprintRun bp("Settings", nullptr, true, {"settings_cpp"});
  KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Settings Blueprint compiles and runs: %s", bp.problems.c_str());
  KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  bp.rt.budget = rules::kLoopBudget;
  using S = blueprints::settings_cpp;
  using Opt = S::Opt;
  KERN_CHECK(kern::logic::bind_blueprint<S>(bp.rt), "settings_cpp runs as its class");
  S& set = kern::logic::blueprint<S>();
  S::SettingsTable table;
  KERN_CHECK(set.bind_settings(table).Result, "its table has one row per setting");
  S::Settings s = set.Settings_new(table).Result;
  auto get = [&](Opt o) { return set.Settings_get(s, o).Result; };
  auto on = [&](Opt o) { return static_cast<bool>(set.Settings_on(s, o).Result); };
  KERN_CHECK(get(Opt::mouse_sensitivity) == 100 && !on(Opt::mouse_invert_y), "mouse 100 %%, not inverted");
  KERN_CHECK(get(Opt::drone_max_speed_kmh) == 90 && std::fabs(get(Opt::drone_hover_agl) - 1.2f) < 1e-6f, "drone 90 km/h, hover 1.2 m");
  KERN_CHECK(get(Opt::car_max_kmh) == 115 && get(Opt::car_reverse_kmh) == 32, "car 115 / 32 km/h");
  KERN_CHECK(get(Opt::cam_fpv_tilt) == -15 && get(Opt::cam_fov) == 105, "camera -15 deg, 105 deg");
  KERN_CHECK(get(Opt::audio_drone) == 50 && get(Opt::audio_vehicle) == 58 && get(Opt::music_volume) == 32, "audio mix v5");
  KERN_CHECK(on(Opt::world_jetpack) && on(Opt::world_friendly_fire) && !on(Opt::world_terrain) && !on(Opt::world_flak) && on(Opt::world_guided), "world options: jetpack, friendly fire, guided on; terrain, flak off");
  const auto v = set.Settings_version(s).Result;
  KERN_CHECK(set.Settings_set_2(s, table, "mouse.sensitivity", "999").Result && get(Opt::mouse_sensitivity) == 300, "clamped to 300");
  KERN_CHECK(set.Settings_set_2(s, table, "mouse.sensitivity", "47").Result && get(Opt::mouse_sensitivity) == 45, "snapped to the 5 %% step (%g)", static_cast<double>(get(Opt::mouse_sensitivity)));
  KERN_CHECK(set.Settings_version(s).Result > v, "version counts changes");
  KERN_CHECK(set.Settings_set_2(s, table, "world.people", "false").Result && !on(Opt::world_people), "toggle off");
  KERN_CHECK(!set.Settings_set_2(s, table, "no.such", "1").Result && !set.Settings_set_2(s, table, "car.maxKmh", "fast").Result, "unknown key / no number");
  set.Settings_reset_section(s, table, "maus");
  KERN_CHECK(get(Opt::mouse_sensitivity) == 100 && !on(Opt::world_people), "section reset keeps the others");
  const std::string schema(set.Settings_schema_json(s, table).Result.view());
  KERN_CHECK(schema.find("\"key\":\"world.jetpack\"") != std::string::npos && schema.front() == '[' && schema.back() == ']', "schema lists every row");
}
