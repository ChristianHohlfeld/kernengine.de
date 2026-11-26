// LIVE GPS (gps_live_mode.mjs): weak fixes and jumps are rejected, the track glides, activity from speed.
// The filter is the level's Blueprint gps_live_cpp (its struct and the functions on it: gps_live), called as
// its class - as the game calls it (level_blueprints.h), with the game's loop guard.
#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

KERN_TEST(start_gps_live_filter) {
  kern::test::BlueprintRun gps_rules("gps_live_cpp", nullptr, true, {"gps_live"});
  gps_rules.rt.budget = rules::kLoopBudget;
  KERN_CHECK(gps_rules.problems.empty() && kern::logic::bind_blueprint<blueprints::gps_live_cpp>(gps_rules.rt) && kern::logic::bind_blueprint<blueprints::gps_live>(gps_rules.rt),
           "gps_live_cpp and gps_live run as their classes: %s", gps_rules.problems.c_str());
  auto& filter = kern::logic::blueprint<blueprints::gps_live_cpp>();
  auto& track = kern::logic::blueprint<blueprints::gps_live>();
  blueprints::gps_live::GpsLive g;
  KERN_CHECK(!filter.GpsLive_fix(g, 0, 0, 0, 5).Result, "off: no fixes");
  filter.GpsLive_start(g, 0);
  KERN_CHECK(!filter.GpsLive_fix(g, 0, 0, 0, 80).Result && g.rejected == 1, "accuracy over 65 m is rejected");
  KERN_CHECK(filter.GpsLive_fix(g, 0, 0, 0, 5).Result && track.GpsLive_has_fix(g).Result, "first fix");
  // Jogging north at 3 m/s, a fix every second.
  f64 t = 0;
  for (int i = 1; i <= 20; ++i) {
    for (int k = 0; k < 10; ++k) filter.GpsLive_frame(g, t + k * 0.1, 0.1);
    t = i;
    filter.GpsLive_fix(g, t, 0, 3.0 * i, 5, 3.0, 0);
  }
  for (int k = 0; k < 60; ++k) filter.GpsLive_frame(g, t + 0.01 * k, 0.1);
  const Vec3 shown = track.GpsLive_shown(g).Result;
  KERN_CHECK(std::fabs(shown.y - 60) < 4 && std::fabs(shown.x) < 0.5f, "follows the track (%.1f %.1f)", shown.x, shown.y);
  KERN_CHECK(filter.GpsLive_activity(g).Result == blueprints::gps_live::GpsLive_Activity::run, "3 m/s is running (%.2f m/s)", static_cast<f64>(track.GpsLive_speed(g).Result));
  KERN_CHECK(std::fabs(track.GpsLive_heading(g).Result) < 0.1, "heading north");
  const f64 distance = track.GpsLive_distance(g).Result;
  KERN_CHECK(distance > 50 && distance < 70, "distance %.1f m", distance);
  KERN_CHECK(!filter.GpsLive_fix(g, t + 1, 500, 60, 20).Result, "a 500 m jump in a second is rejected");
  // A long gap (screen off) starts cleanly at the new place.
  KERN_CHECK(filter.GpsLive_fix(g, t + 30, 200, 300, 5).Result, "after a gap");
  filter.GpsLive_frame(g, t + 30, 0.1);
  const Vec3 again = track.GpsLive_shown(g).Result;
  KERN_CHECK(std::fabs(again.x - 200) < 1 && std::fabs(again.y - 300) < 1, "re-acquired at the new place");
}
