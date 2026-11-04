// The settings table: SpawnCity's defaults and ranges, the "set" parser, the stick curve.
#include <string>

#include "../src/settings.h"
#include "test_util.h"

using namespace sc;

SC_TEST(settings_defaults_and_ranges) {
  Settings s;
  SC_CHECK(s.get(Opt::mouse_sensitivity) == 100 && !s.on(Opt::mouse_invert_y), "mouse 100 %%, not inverted");
  SC_CHECK(s.get(Opt::drone_max_speed_kmh) == 90 && std::fabs(s.get(Opt::drone_hover_agl) - 1.2f) < 1e-6f, "drone 90 km/h, hover 1.2 m");
  SC_CHECK(s.get(Opt::car_max_kmh) == 115 && s.get(Opt::car_reverse_kmh) == 32, "car 115 / 32 km/h");
  SC_CHECK(s.get(Opt::cam_fpv_tilt) == -15 && s.get(Opt::cam_fov) == 105, "camera -15 deg, 105 deg");
  SC_CHECK(s.get(Opt::audio_drone) == 50 && s.get(Opt::audio_vehicle) == 58 && s.get(Opt::music_volume) == 32, "audio mix v5");
  SC_CHECK(s.on(Opt::world_jetpack) && !s.on(Opt::world_friendly_fire), "world options: jetpack on, friendly fire off");
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

SC_TEST(settings_stick_curve) {
  SC_CHECK(fineness_axis(1, 10) == 1 && fineness_axis(-1, 10) == -1, "full deflection stays full");
  SC_CHECK(std::fabs(fineness_axis(0.5f, 1) - 0.5f) < 1e-6f, "level 1 is linear");
  const f32 e = 0.70f * 0.38f;  // level 10
  SC_CHECK(std::fabs(fineness_axis(0.5f, 10) - (0.5f * (1 - e) + 0.125f * e)) < 1e-6f, "phoneAxis at level 10");
}
