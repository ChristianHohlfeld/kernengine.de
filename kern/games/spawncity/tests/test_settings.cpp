// The settings table: SpawnCity's defaults and ranges, the "set" parser. The table is the level's Blueprint
// Settings; the settings themselves are the program's (settings.cpp), bound to the actor "Settings" of the level's
// world as the game binds them (bind_settings(world, table)) - what they do there is the program's Blueprint
// SpawnCity, compared with this source bit for bit (tests/converted/test_SpawnCity__settings_cpp.cpp).
#include <cmath>
#include <string>

#include "../src/blueprint_call.h"
#include "../src/settings.h"
#include "level_blueprints.h"
#include "rules_world.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

KERN_TEST(settings_defaults_and_ranges) {
  {
    kern::test::BlueprintRun bp("Settings", nullptr, true, {"settings_cpp"});
    KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Settings Blueprint compiles and runs: %s", bp.problems.c_str());
    KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
    KERN_CHECK(kern::logic::bind_blueprint<blueprints::settings_cpp>(bp.rt), "settings_cpp runs as its class");
  }
  sc::test::RulesWorld level;
  KERN_CHECK(level.ok, "the level's world starts");
  if (!level.ok) return;
  SettingsTable table;
  KERN_CHECK(bind_settings(level.world(), table), "its table has one row per setting");
  Settings s(table);
  auto get = [&](Opt o) { return s.get(o); };
  auto on = [&](Opt o) { return s.on(o); };
  KERN_CHECK(get(Opt::mouse_sensitivity) == 100 && !on(Opt::mouse_invert_y), "mouse 100 %%, not inverted");
  KERN_CHECK(get(Opt::drone_max_speed_kmh) == 90 && std::fabs(get(Opt::drone_hover_agl) - 1.2f) < 1e-6f, "drone 90 km/h, hover 1.2 m");
  KERN_CHECK(get(Opt::car_max_kmh) == 115 && get(Opt::car_reverse_kmh) == 32, "car 115 / 32 km/h");
  KERN_CHECK(get(Opt::cam_fpv_tilt) == -15 && get(Opt::cam_fov) == 105, "camera -15 deg, 105 deg");
  KERN_CHECK(get(Opt::audio_drone) == 50 && get(Opt::audio_vehicle) == 58 && get(Opt::music_volume) == 32, "audio mix v5");
  KERN_CHECK(on(Opt::world_jetpack) && on(Opt::world_friendly_fire) && !on(Opt::world_terrain) && !on(Opt::world_flak) && on(Opt::world_guided), "world options: jetpack, friendly fire, guided on; terrain, flak off");
  const auto v = s.version();
  KERN_CHECK(s.set(table, "mouse.sensitivity", "999") && get(Opt::mouse_sensitivity) == 300, "clamped to 300");
  KERN_CHECK(s.set(table, "mouse.sensitivity", "47") && get(Opt::mouse_sensitivity) == 45, "snapped to the 5 %% step (%g)", static_cast<double>(get(Opt::mouse_sensitivity)));
  KERN_CHECK(s.version() > v, "version counts changes");
  KERN_CHECK(s.set(table, "world.people", "false") && !on(Opt::world_people), "toggle off");
  KERN_CHECK(!s.set(table, "no.such", "1") && !s.set(table, "car.maxKmh", "fast"), "unknown key / no number");
  s.reset_section(table, "maus");
  KERN_CHECK(get(Opt::mouse_sensitivity) == 100 && !on(Opt::world_people), "section reset keeps the others");
  const std::string schema = s.schema_json(table);
  KERN_CHECK(schema.find("\"key\":\"world.jetpack\"") != std::string::npos && schema.front() == '[' && schema.back() == ']', "schema lists every row");
}
