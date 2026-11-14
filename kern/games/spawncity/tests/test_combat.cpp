// Combat and event rules with SpawnCity's numbers (combat_rules.h).
#include "../src/combat_rules.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;
using namespace sc::combat;

namespace {
/// The five rule actors of the level, bound for a test.
struct CombatBlueprints {
  sc::RuleBlueprint military{"Military", [](kern::logic::Instance* i) { sc::combat::bind_military(i); }};
  sc::RuleBlueprint zombies{"Zombies", [](kern::logic::Instance* i) { sc::combat::bind_zombies(i); }};
  sc::RuleBlueprint sandbox{"Sandbox", [](kern::logic::Instance* i) { sc::combat::bind_sandbox(i); }};
  sc::RuleBlueprint fun{"Fun", [](kern::logic::Instance* i) { sc::combat::bind_fun(i); }};
  sc::RuleBlueprint race{"Race", [](kern::logic::Instance* i) { sc::combat::bind_race(i); }};
};
}  // namespace

SC_TEST(combat_military_escalation) {
  CombatBlueprints rules;
  for (const sc::RuleBlueprint* bp : {&rules.military, &rules.zombies, &rules.sandbox, &rules.fun, &rules.race}) {
    SC_CHECK(bp->inst && bp->problems.empty() && !bp->inst->disabled(), "a combat rule Blueprint of the level compiles and runs: %s", bp->problems.c_str());
    SC_CHECK(bp->inst && std::string(bp->inst->tier()) == "native C++", "it runs as its C++ class");
  }
  SC_CHECK(sc::combat::bind_military(rules.military.inst) && sc::combat::bind_zombies(rules.zombies.inst) && sc::combat::bind_sandbox(rules.sandbox.inst) &&
               sc::combat::bind_fun(rules.fun.inst) && sc::combat::bind_race(rules.race.inst),
           "the game reads their tables");
  MilitaryClock m;
  SC_CHECK(m.update(0, 5, 40) == MilEvent::none, "five stars just reached: no army yet");
  SC_CHECK(m.update(39, 5, 40) == MilEvent::none, "held 39 s: still none");
  SC_CHECK(m.update(41, 5, 40) == MilEvent::alert, "held over 40 s at heat 36+: alert");
  SC_CHECK(m.update(50, 4, 40) == MilEvent::cancelled, "lost the fifth star: alert called off");
  MilitaryClock x;
  SC_CHECK(x.update(0, 5, 60) == MilEvent::alert, "extreme heat 55+: alert at once");
  SC_CHECK(x.update(10, 5, 60) == MilEvent::none, "alert runs 15 s");
  SC_CHECK(x.update(15.5f, 5, 60) == MilEvent::start && x.active, "after the alert the army takes over");
  SC_CHECK(x.update(20, 4, 30) == MilEvent::none, "four stars: they stay");
  SC_CHECK(x.update(21, 3, 20) == MilEvent::stop && !x.active, "three stars: they withdraw");
  SC_CHECK(heli_hit_chance(0, 0) == 0.32f && heli_hit_chance(400, 10) == 0.05f, "heli MG chance clamp");
  SC_CHECK(std::fabs(soldier_hit_chance(27, 0) - 0.14f) < 1e-5f, "rifle chance .24 - d/270");
}

SC_TEST(combat_flak_cells) {
  CombatBlueprints rules;
  u32 seed = 0, seed2 = 0;
  int guns = 0;
  for (int x = -20; x < 20; ++x)
    for (int y = -20; y < 20; ++y) guns += flak_cell(x, y, seed) ? 1 : 0;
  SC_CHECK(guns > 1600 * 0.25f && guns < 1600 * 0.4f, "about 32 %% of the cells have a gun (%d / 1600)", guns);
  const bool a = flak_cell(3, -7, seed), b = flak_cell(3, -7, seed2);
  SC_CHECK(a == b && seed == seed2, "placement is deterministic per cell");
  SC_CHECK(flak_burst_damage(0) == 16 && flak_burst_damage(7.5f) == 0 && flak_burst_damage(3.5f) == 9, "airburst 14 (1 - d/7) + 2");
}

SC_TEST(combat_tempers) {
  CombatBlueprints rules;
  int aggressive = 0, runner = 0, knife = 0, gun = 0;
  for (u32 i = 1; i <= 20000; ++i) {
    const u32 seed = i * 2654435761u;
    const Temper t = temper_of(seed);
    aggressive += t == Temper::aggressive;
    runner += t == Temper::runner;
    const Arms a = arms_of(seed);
    knife += a == Arms::knife;
    gun += a == Arms::gun;
  }
  SC_CHECK(aggressive > 900 && aggressive < 1500, "about 6 %% aggressive (%d)", aggressive);
  SC_CHECK(runner > 5300 && runner < 6700, "about 30 %% runners (%d)", runner);
  SC_CHECK(knife > 0 && knife < aggressive / 3 && gun < knife, "few knives, fewer guns (%d / %d)", knife, gun);
  SC_CHECK(gangster_hit_chance(30, 6) == 0.08f, "street pistol at range: often a miss");
}

SC_TEST(combat_zombie_rounds) {
  CombatBlueprints rules;
  ZombieRounds z;
  SC_CHECK(z.update(0, false, 0) == ZombieEvent::none && !z.active, "day: nothing");
  SC_CHECK(z.update(1, true, 0) == ZombieEvent::night && z.round == 1 && z.to_spawn == 10, "night: round 1 with 6 + 4 zombies");
  SC_CHECK(z.update(2, true, 0) == ZombieEvent::none, "first spawn after 1.8 s");
  SC_CHECK(z.update(3, true, 0) == ZombieEvent::spawn, "spawn due");
  for (int i = 0; i < 10; ++i) z.spawned(3);
  SC_CHECK(z.update(4, true, 3) == ZombieEvent::none, "zombies still up");
  SC_CHECK(z.update(5, true, 0) == ZombieEvent::round_done, "all down: round over");
  SC_CHECK(z.update(15, true, 0) == ZombieEvent::round && z.round == 2 && z.to_spawn == 14, "9 s break, then round 2");
  SC_CHECK(z.hp() == 124, "hp 100 + 12 per round");
  SC_CHECK(z.update(16, false, 0) == ZombieEvent::dawn && !z.active, "dawn ends the night");
}

SC_TEST(combat_fun_bank) {
  CombatBlueprints rules;
  Fun f;
  std::string l = f.apply(FunEvent::police_kill, 1, 2);
  SC_CHECK(f.at_risk == 650 && f.score == 0, "drone down at 2 stars: 430 x 1.5 = 650 at risk (%d)", f.at_risk);
  f.apply(FunEvent::world_kill_car, 2, 2);
  SC_CHECK(f.at_risk == 650 + 260, "car, combo 2: x1.7 = 260 (%d)", f.at_risk);
  l = f.apply(FunEvent::escape, 30, 0);
  SC_CHECK(f.score == 1350 + 800 && f.at_risk == 0, "escape banks risk + 2 x 220 + HOT ESCAPE (combo lapsed) (%d)", f.score);
  SC_CHECK(f.contract_complete, "hot escape done");
  f.apply(FunEvent::cycle_contract, 31, 0);
  f.apply(FunEvent::world_kill_bus, 32, 0);
  f.apply(FunEvent::player_death, 33, 0);
  SC_CHECK(f.at_risk == 0 && f.score == 2150 + 370 && f.mercy == 2, "death insures half (%d)", f.score);
  SC_CHECK(std::fabs(f.police_damage_scale() - 0.76f) < 1e-6f, "mercy lowers police damage");
}

SC_TEST(combat_chaos_combo) {
  CombatBlueprints rules;
  Chaos c;
  c.award(250, 0);
  c.award(250, 1);
  c.award(250, 2);
  SC_CHECK(c.score == 250 + 500 + 750 && c.combo == 3, "combo within 3.2 s (%d)", c.score);
  c.award(100, 10);
  SC_CHECK(c.combo == 1, "combo lapses");
  EnemyType t = choose_enemy(4, 0.1f, 0);
  SC_CHECK(t == EnemyType::heavy, "four stars: a heavy now and then");
  t = choose_enemy(1, 0.3f, 0);
  SC_CHECK(t == EnemyType::cop, "one star: cops");
}

SC_TEST(combat_race_gates) {
  CombatBlueprints rules;
  Race r;
  const Gate& g0 = gate(0);
  Vec2 n, right;
  gate_axes(0, n, right);
  auto through = [&](i32 i, f32 t) {
    Vec2 nn, rr;
    gate_axes(i, nn, rr);
    const Vec3 c = gate(i).center;
    r.update(c - Vec3{nn.x, nn.y, 0} * 0.5f, t);
    return r.update(c + Vec3{nn.x, nn.y, 0} * 0.5f, t + 0.01f);
  };
  SC_CHECK(std::fabs(g0.center.x + 4.5f) < 1e-5f, "course scaled 2.25");
  // Missing the gate (beside it) counts nothing.
  r.update(g0.center + Vec3{right.x, right.y, 0} * 3.0f - Vec3{n.x, n.y, 0}, 0);
  SC_CHECK(r.update(g0.center + Vec3{right.x, right.y, 0} * 3.0f + Vec3{n.x, n.y, 0}, 0.1f) < 0, "beside the gate: no pass");
  f32 t = 1;
  for (int lap = 0; lap < 3; ++lap)
    for (int i = 0; i < kGates; ++i) { SC_CHECK(through(i, t) == i, "gate %d lap %d", i, lap); t += 2; }
  SC_CHECK(through(0, t) == 0 && r.finished && r.lap == 3, "three laps finish");
  SC_CHECK(std::fabs(r.best_lap - 16) < 0.05f, "lap time 16 s (%.2f)", r.best_lap);
  SC_CHECK(race_time(75.5f) == "01:15.500", "time format");
}
