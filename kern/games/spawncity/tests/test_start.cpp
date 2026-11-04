// LIVE GPS (gps_live_mode.mjs): weak fixes and jumps are rejected, the track glides, activity from speed.
#include "../src/gps_live.h"
#include "test_util.h"

using namespace sc;

SC_TEST(start_gps_live_filter) {
  GpsLive g;
  SC_CHECK(!g.fix(0, 0, 0, 5), "off: no fixes");
  g.start(0);
  SC_CHECK(!g.fix(0, 0, 0, 80) && g.rejected == 1, "accuracy over 65 m is rejected");
  SC_CHECK(g.fix(0, 0, 0, 5) && g.has_fix(), "first fix");
  // Jogging north at 3 m/s, a fix every second.
  f64 t = 0;
  for (int i = 1; i <= 20; ++i) {
    for (int k = 0; k < 10; ++k) g.frame(t + k * 0.1, 0.1f);
    t = i;
    g.fix(t, 0, 3.0f * i, 5, 3.0f, 0);
  }
  for (int k = 0; k < 60; ++k) g.frame(t + 0.01 * k, 0.1f);
  SC_CHECK(std::fabs(g.shown().y - 60) < 4 && std::fabs(g.shown().x) < 0.5f, "follows the track (%.1f %.1f)", g.shown().x, g.shown().y);
  SC_CHECK(g.activity() == GpsLive::Activity::run, "3 m/s is running (%.2f m/s)", g.speed());
  SC_CHECK(std::fabs(g.heading()) < 0.1f, "heading north");
  SC_CHECK(g.distance() > 50 && g.distance() < 70, "distance %.1f m", g.distance());
  SC_CHECK(!g.fix(t + 1, 500, 60, 20), "a 500 m jump in a second is rejected");
  // A long gap (screen off) starts cleanly at the new place.
  SC_CHECK(g.fix(t + 30, 200, 300, 5), "after a gap");
  g.frame(t + 30, 0.1f);
  SC_CHECK(std::fabs(g.shown().x - 200) < 1 && std::fabs(g.shown().y - 300) < 1, "re-acquired at the new place");
}
