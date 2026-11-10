// Game rules with SpawnCity's numbers: wanted level, police response, escape. The rules are the
// Blueprint of the level's actor "Wanted" (level/spawncity.klevel): the tests run that Blueprint
// in Kern Logic and call it the way the game does (wanted.h).
#include "../src/rules.h"
#include "../src/wanted.h"
#include "test_util.h"

using namespace sc;

namespace {
/// The level's actor "Wanted" running in a logic runtime of its own, bound to wanted.h.
struct WantedBlueprint {
  struct Host : kern::logic::Host {
    f64 now = 0;
    std::string errors;
    f64 time() const override { return now; }
    void print(const std::string&, bool) override {}
    void error(const std::string& text) override { errors += text + "\n"; }
    kern::u32 random() override { return 4; }
  } host;
  kern::logic::Runtime rt{host};
  kern::logic::Instance* inst = nullptr;
  std::string problems;
  WantedBlueprint() {
    kern::logic::Graph g;
    std::string error;
    if (!rules::actor_graph("Wanted", g, &error)) { problems = error; return; }
    for (const kern::logic::Issue& i : kern::logic::check(g)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
    inst = rt.add(g, 1, "Wanted");
    rt.begin();
    wanted::bind(inst);
  }
  ~WantedBlueprint() { wanted::bind(nullptr); }
};
}  // namespace

SC_TEST(rules_wanted_blueprint) {
  WantedBlueprint bp;
  SC_CHECK(bp.inst && bp.problems.empty(), "the level's Wanted Blueprint compiles without errors or warnings: %s", bp.problems.c_str());
  if (!bp.inst) return;
  SC_CHECK(!bp.inst->disabled() && bp.host.errors.empty(), "it runs (%s)", bp.host.errors.c_str());
  // every rule the game calls is a function of the Blueprint
  for (const char* f : {"Severity", "StarsForHeat", "PoliceCount", "DetectionRadius", "EscapeMs", "PoliceDamage", "SpawnRadius", "ArrivalDelayMs", "EngageDelayMs",
                        "WaveBreakMs", "FireRange", "FireIntervalMs", "HitChance", "EmpImpulse", "Report", "Update", "Clear", "Stars"})
    SC_CHECK(bp.inst->has_function(f), "function %s", f);
  SC_CHECK(wanted::max_stars() == 5 && wanted::emp_range() == 30 && wanted::emp_cooldown_ms() == 18000 && wanted::max_fire_range() == 35, "the constants are its variables");
  // the whole table of the original (wanted_system_logic.mjs), star by star
  const int severity[10] = {2, 1, 2, 1, 2, 3, 3, 3, 1, 0};
  for (int c = 0; c < 10; ++c) SC_CHECK(wanted::severity(static_cast<wanted::Crime>(c)) == severity[c], "severity of crime %d", c);
  const float heats[] = {-3, 0, 3.99f, 4, 8.9f, 9, 14, 15, 21.5f, 22, 29, 30, 500};
  const int stars[] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
  for (int i = 0; i < 13; ++i) SC_CHECK(wanted::stars_for_heat(heats[i]) == stars[i], "stars for heat %.2f", heats[i]);
  const int count[] = {0, 1, 2, 3, 3, 3, 3};
  const float detect[] = {0, 59, 66, 73, 80, 87, 87}, escape[] = {0, 5100, 6000, 6900, 7800, 8700, 8700}, damage[] = {2, 2, 2, 3, 3, 4, 4};
  const float arrival[] = {14700, 14700, 13400, 12100, 10800, 9500, 9500}, engage[] = {6350, 6350, 5900, 5450, 5000, 4550, 4550};
  const float wave[] = {9050, 9050, 8600, 8150, 7700, 7250, 7250}, interval[] = {4870, 4870, 4640, 4410, 4180, 3950, 3950};
  for (int s = 0; s <= 6; ++s) {
    SC_CHECK(wanted::police_count(s) == count[s], "units at %d stars", s);
    SC_CHECK(wanted::detection_radius(s) == detect[s] && wanted::escape_ms(s) == escape[s], "detection and escape at %d stars", s);
    SC_CHECK(wanted::police_damage(s) == damage[s], "damage at %d stars", s);
    SC_CHECK(wanted::arrival_delay_ms(s) == arrival[s] && wanted::engage_delay_ms(s) == engage[s] && wanted::wave_break_ms(s) == wave[s], "delays at %d stars", s);
    SC_CHECK(wanted::fire_interval_ms(s) == interval[s], "fire interval at %d stars", s);
    const int level = s < 1 ? 1 : s > 5 ? 5 : s;
    SC_CHECK(std::fabs(wanted::fire_range(s) - std::fmin(35.0f, 28 + static_cast<float>(level) * 1.4f)) < 1e-5f, "fire range at %d stars", s);
  }
  SC_CHECK(wanted::spawn_radius(0) == 58 && wanted::spawn_radius(1) == 66 && wanted::spawn_radius(2) == 74 && wanted::spawn_radius(3) == 58 && wanted::spawn_radius(-4) == 58, "spawn rings 58 66 74");
  // the hit chance against the original's formula on a grid
  for (int s = 1; s <= 5; ++s)
    for (float d : {0.0f, 6.0f, 20.0f, 38.0f, 90.0f})
      for (float v : {-2.0f, 0.0f, 3.6f, 7.2f, 15.0f}) {
        const float dp = clampf((d - 6) / 32, 0, 1) * 0.26f, mp = std::fmin(1.0f, std::fmax(0.0f, v) / 7.2f) * 0.24f;
        const float want = clampf(0.28f + static_cast<float>(s) * 0.02f - dp - mp, 0.1f, 0.38f);
        SC_CHECK(std::fabs(wanted::hit_chance(s, d, v) - want) < 1e-6f, "hit chance %d stars, %.0f m, %.1f m/s: %f vs %f", s, d, v, wanted::hit_chance(s, d, v), want);
      }
  // the EMP's push: 126 at the player, 54 at the edge, nothing beyond or behind
  SC_CHECK(wanted::emp_impulse(0) == 126 && std::fabs(wanted::emp_impulse(30) - 54) < 1e-4f && std::fabs(wanted::emp_impulse(15) - 90) < 1e-4f, "EMP 126 .. 90 .. 54");
  SC_CHECK(wanted::emp_impulse(30.01f) == 0 && wanted::emp_impulse(-1) == 0 && wanted::emp_impulse(std::nanf("")) == 0, "EMP: nothing out of range");
  SC_CHECK(bp.host.errors.empty(), "no errors while it ran (%s)", bp.host.errors.c_str());
}

SC_TEST(rules_wanted_unbound) {
  // without the Blueprint there are no rules (the game refuses to start without it)
  SC_CHECK(wanted::blueprint() == nullptr && wanted::police_count(5) == 0 && wanted::arrival_delay_ms(1) == 0, "unbound: nothing");
  wanted::Tracker t;
  t.report(wanted::Crime::police_car, true);
  SC_CHECK(t.stars() == 0 && t.heat() == 0, "unbound: no heat");
}

SC_TEST(rules_wanted_levels) {
  WantedBlueprint bp;
  SC_CHECK(wanted::stars_for_heat(3.9f) == 0 && wanted::stars_for_heat(4) == 1 && wanted::stars_for_heat(30) == 5, "thresholds 4 9 15 22 30");
  SC_CHECK(wanted::police_count(1) == 1 && wanted::police_count(3) == 3 && wanted::police_count(5) == 3, "units min(3, ceil(.7 stars))");
  SC_CHECK(wanted::detection_radius(2) == 66, "detection 52 + 7 stars");
  SC_CHECK(wanted::police_damage(1) == 2 && wanted::police_damage(3) == 3 && wanted::police_damage(5) == 4, "damage 2 2 3 3 4");
  SC_CHECK(wanted::arrival_delay_ms(5) == 9500, "arrival max(9000, 16000 - 1300 stars)");
  SC_CHECK(std::fabs(wanted::hit_chance(1, 0, 0) - 0.3f) < 1e-6f && wanted::hit_chance(5, 100, 20) == 0.1f, "hit chance clamp");
  SC_CHECK(wanted::emp_impulse(0) == 126 && wanted::emp_impulse(31) == 0, "EMP impulse");
}

SC_TEST(rules_wanted_escape) {
  WantedBlueprint bp;
  wanted::Tracker t;
  t.report(wanted::Crime::carjacking, false);
  SC_CHECK(t.stars() == 0, "nobody saw it");
  for (int i = 0; i < 4; ++i) t.report(wanted::Crime::carjacking, true);
  SC_CHECK(t.stars() == 1, "four carjackings seen = one star (%d)", t.stars());
  t.update(4.0f, false);
  SC_CHECK(t.stars() == 1, "still searched after 4 s");
  t.update(1.2f, false);
  SC_CHECK(t.stars() == 0, "escaped after 5.1 s out of sight");
  SC_CHECK(t.heat() == 0 && t.hidden_ms() == 0, "the escape clears the heat and the clock");
  // being seen restarts the clock; a new crime does too
  for (int i = 0; i < 9; ++i) t.report(wanted::Crime::person_hit, true);
  SC_CHECK(t.stars() == 2 && t.heat() == 9, "nine hits seen = two stars");
  t.update(5.0f, false);
  SC_CHECK(std::fabs(t.hidden_ms() - 5000) < 1e-3f && t.stars() == 2, "5 s of the 6 s out of sight");
  t.update(0.1f, true);
  SC_CHECK(t.hidden_ms() == 0 && t.stars() == 2, "seen again: the clock restarts");
  t.update(5.9f, false);
  t.report(wanted::Crime::bird, true);
  SC_CHECK(t.hidden_ms() == 0 && t.heat() == 9, "a crime seen restarts the clock (a bird costs nothing)");
  t.update(5.9f, false);
  SC_CHECK(t.stars() == 2, "not yet");
  t.update(0.2f, false);
  SC_CHECK(t.stars() == 0, "gone after 6 s");
  t.update(1.0f, false);
  SC_CHECK(t.hidden_ms() == 0, "no stars: no clock");
  SC_CHECK(bp.host.errors.empty(), "no errors while it ran (%s)", bp.host.errors.c_str());
}
