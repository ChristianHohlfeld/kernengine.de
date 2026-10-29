// Game rules with SpawnCity's numbers: wanted level, police response, escape.
#include "../src/wanted.h"
#include "test_util.h"

using namespace sc;

SC_TEST(rules_wanted_levels) {
  SC_CHECK(wanted::stars_for_heat(3.9f) == 0 && wanted::stars_for_heat(4) == 1 && wanted::stars_for_heat(30) == 5, "thresholds 4 9 15 22 30");
  SC_CHECK(wanted::police_count(1) == 1 && wanted::police_count(3) == 3 && wanted::police_count(5) == 3, "units min(3, ceil(.7 stars))");
  SC_CHECK(wanted::detection_radius(2) == 66, "detection 52 + 7 stars");
  SC_CHECK(wanted::police_damage(1) == 2 && wanted::police_damage(3) == 3 && wanted::police_damage(5) == 4, "damage 2 2 3 3 4");
  SC_CHECK(wanted::arrival_delay_ms(5) == 9500, "arrival max(9000, 16000 - 1300 stars)");
  SC_CHECK(std::fabs(wanted::hit_chance(1, 0, 0) - 0.3f) < 1e-6f && wanted::hit_chance(5, 100, 20) == 0.1f, "hit chance clamp");
  SC_CHECK(wanted::emp_impulse(0) == 126 && wanted::emp_impulse(31) == 0, "EMP impulse");
}

SC_TEST(rules_wanted_escape) {
  wanted::Tracker t;
  t.report(wanted::Crime::carjacking, false);
  SC_CHECK(t.stars() == 0, "nobody saw it");
  for (int i = 0; i < 4; ++i) t.report(wanted::Crime::carjacking, true);
  SC_CHECK(t.stars() == 1, "four carjackings seen = one star (%d)", t.stars());
  t.update(4.0f, false);
  SC_CHECK(t.stars() == 1, "still searched after 4 s");
  t.update(1.2f, false);
  SC_CHECK(t.stars() == 0, "escaped after 5.1 s out of sight");
}
