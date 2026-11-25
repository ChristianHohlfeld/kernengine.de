// Combat and event rules with SpawnCity's numbers. The rules are the level's rule actors (Military, Zombies,
// Sandbox, Fun, Race); what the program keeps of them is the Blueprint combat_rules_cpp (its structs:
// combat_rules), called as its class - as the program calls it (level_blueprints.h), with the game's loop guard.
#include <string>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

namespace {
using Cr = blueprints::combat_rules_cpp;
/// The five rule actors of the level and combat_rules_cpp beside them, as their classes.
struct CombatBlueprints {
  RuleBlueprint rules{nullptr, nullptr, true, {"Military", "Zombies", "Sandbox", "Fun", "Race", "combat_rules_cpp"}};
  bool bound = false;
  CombatBlueprints() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Cr>(rules.rt);
  }
  Cr& operator*() { return kern::logic::blueprint<Cr>(); }
};
}  // namespace

SC_TEST(combat_military_escalation) {
  CombatBlueprints rules;
  for (const char* name : {"Military", "Zombies", "Sandbox", "Fun", "Race"}) {
    const kern::logic::Instance* bp = rules.rules.find(name);
    SC_CHECK(bp && !bp->disabled(), "the combat rule Blueprint %s of the level compiles and runs: %s", name, rules.rules.problems.c_str());
    SC_CHECK(bp && std::string(bp->tier()) == "native C++", "it runs as its C++ class");
  }
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class");
  if (!rules.bound) return;
  Cr& cr = *rules;
  using E = Cr::combat_MilEvent;
  Cr::combat_MilitaryClock m;
  SC_CHECK(cr.MilitaryClock_update(m, 0, 5, 40, true).Result == E::none, "five stars just reached: no army yet");
  SC_CHECK(cr.MilitaryClock_update(m, 39, 5, 40, true).Result == E::none, "held 39 s: still none");
  SC_CHECK(cr.MilitaryClock_update(m, 41, 5, 40, true).Result == E::alert, "held over 40 s at heat 36+: alert");
  SC_CHECK(cr.MilitaryClock_update(m, 50, 4, 40, true).Result == E::cancelled, "lost the fifth star: alert called off");
  Cr::combat_MilitaryClock x;
  SC_CHECK(cr.MilitaryClock_update(x, 0, 5, 60, true).Result == E::alert, "extreme heat 55+: alert at once");
  SC_CHECK(cr.MilitaryClock_update(x, 10, 5, 60, true).Result == E::none, "alert runs 15 s");
  SC_CHECK(cr.MilitaryClock_update(x, 15.5, 5, 60, true).Result == E::start && x.active, "after the alert the army takes over");
  SC_CHECK(cr.MilitaryClock_update(x, 20, 4, 30, true).Result == E::none, "four stars: they stay");
  SC_CHECK(cr.MilitaryClock_update(x, 21, 3, 20, true).Result == E::stop && !x.active, "three stars: they withdraw");
  SC_CHECK(cr.heli_hit_chance(0, 0).Result == 0.32f && cr.heli_hit_chance(400, 10).Result == 0.05f, "heli MG chance clamp");
  SC_CHECK(std::fabs(cr.soldier_hit_chance(27, 0).Result - 0.14f) < 1e-5f, "rifle chance .24 - d/270");
}

SC_TEST(combat_flak_cells) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  Cr::U32 seed = 0, seed2 = 0;
  int guns = 0;
  for (int x = -20; x < 20; ++x)
    for (int y = -20; y < 20; ++y) guns += cr.flak_cell(seed, Cr::Int64{x}, Cr::Int64{y}).Result ? 1 : 0;
  SC_CHECK(guns > 1600 * 0.25f && guns < 1600 * 0.4f, "about 32 %% of the cells have a gun (%d / 1600)", guns);
  const bool a = cr.flak_cell(seed, Cr::Int64{3}, Cr::Int64{-7}).Result, b = cr.flak_cell(seed2, Cr::Int64{3}, Cr::Int64{-7}).Result;
  SC_CHECK(a == b && seed == seed2, "placement is deterministic per cell");
  SC_CHECK(cr.flak_burst_damage(0).Result == 16 && cr.flak_burst_damage(7.5).Result == 0 && cr.flak_burst_damage(3.5).Result == 9, "airburst 14 (1 - d/7) + 2");
}

SC_TEST(combat_tempers) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  int aggressive = 0, runner = 0, knife = 0, gun = 0;
  for (u32 i = 1; i <= 20000; ++i) {
    const u32 seed = i * 2654435761u;
    const Cr::combat_Temper t = cr.temper_of(seed).Result;
    aggressive += t == Cr::combat_Temper::aggressive;
    runner += t == Cr::combat_Temper::runner;
    const Cr::combat_Arms a = cr.arms_of(seed).Result;
    knife += a == Cr::combat_Arms::knife;
    gun += a == Cr::combat_Arms::gun;
  }
  SC_CHECK(aggressive > 900 && aggressive < 1500, "about 6 %% aggressive (%d)", aggressive);
  SC_CHECK(runner > 5300 && runner < 6700, "about 30 %% runners (%d)", runner);
  SC_CHECK(knife > 0 && knife < aggressive / 3 && gun < knife, "few knives, fewer guns (%d / %d)", knife, gun);
  SC_CHECK(cr.gangster_hit_chance(30, 6).Result == 0.08f, "street pistol at range: often a miss");
}

SC_TEST(combat_zombie_rounds) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  using E = Cr::combat_ZombieEvent;
  Cr::combat_ZombieRounds z;
  SC_CHECK(cr.ZombieRounds_update(z, 0, false, 0).Result == E::none && !z.active, "day: nothing");
  SC_CHECK(cr.ZombieRounds_update(z, 1, true, 0).Result == E::night && z.round == 1 && z.to_spawn == 10, "night: round 1 with 6 + 4 zombies");
  SC_CHECK(cr.ZombieRounds_update(z, 2, true, 0).Result == E::none, "first spawn after 1.8 s");
  SC_CHECK(cr.ZombieRounds_update(z, 3, true, 0).Result == E::spawn, "spawn due");
  for (int i = 0; i < 10; ++i) cr.ZombieRounds_spawned(z, 3);
  SC_CHECK(cr.ZombieRounds_update(z, 4, true, 3).Result == E::none, "zombies still up");
  SC_CHECK(cr.ZombieRounds_update(z, 5, true, 0).Result == E::round_done, "all down: round over");
  SC_CHECK(cr.ZombieRounds_update(z, 15, true, 0).Result == E::round_ && z.round == 2 && z.to_spawn == 14, "9 s break, then round 2");
  SC_CHECK(cr.ZombieRounds_hp(z).Result == 124, "hp 100 + 12 per round");
  SC_CHECK(cr.ZombieRounds_update(z, 16, false, 0).Result == E::dawn && !z.active, "dawn ends the night");
}

SC_TEST(combat_fun_bank) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  using E = Cr::combat_FunEvent;
  Cr::combat_Fun f;
  cr.Fun_apply(f, E::police_kill, 1, 2, 0);
  SC_CHECK(f.at_risk == 650 && f.score == 0, "drone down at 2 stars: 430 x 1.5 = 650 at risk (%d)", static_cast<int>(f.at_risk));
  cr.Fun_apply(f, E::world_kill_car, 2, 2, 0);
  SC_CHECK(f.at_risk == 650 + 260, "car, combo 2: x1.7 = 260 (%d)", static_cast<int>(f.at_risk));
  cr.Fun_apply(f, E::escape, 30, 0, 0);
  SC_CHECK(f.score == 1350 + 800 && f.at_risk == 0, "escape banks risk + 2 x 220 + HOT ESCAPE (combo lapsed) (%d)", static_cast<int>(f.score));
  SC_CHECK(f.contract_complete, "hot escape done");
  cr.Fun_apply(f, E::cycle_contract, 31, 0, 0);
  cr.Fun_apply(f, E::world_kill_bus, 32, 0, 0);
  cr.Fun_apply(f, E::player_death, 33, 0, 0);
  SC_CHECK(f.at_risk == 0 && f.score == 2150 + 370 && f.mercy == 2, "death insures half (%d)", static_cast<int>(f.score));
  SC_CHECK(std::fabs(cr.Fun_police_damage_scale(f).Result - 0.76f) < 1e-6f, "mercy lowers police damage");
}

SC_TEST(combat_chaos_combo) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  Cr::combat_Chaos c;
  cr.Chaos_award(c, 250, 0);
  cr.Chaos_award(c, 250, 1);
  cr.Chaos_award(c, 250, 2);
  SC_CHECK(c.score == 250 + 500 + 750 && c.combo == 3, "combo within 3.2 s (%d)", static_cast<int>(c.score));
  cr.Chaos_award(c, 100, 10);
  SC_CHECK(c.combo == 1, "combo lapses");
  Cr::combat_EnemyType t = cr.choose_enemy(4, 0.1, 0).Result;
  SC_CHECK(t == Cr::combat_EnemyType::heavy, "four stars: a heavy now and then");
  t = cr.choose_enemy(1, 0.3, 0).Result;
  SC_CHECK(t == Cr::combat_EnemyType::cop, "one star: cops");
}

SC_TEST(combat_race_gates) {
  CombatBlueprints rules;
  SC_CHECK(rules.bound, "combat_rules_cpp runs as its class: %s", rules.rules.problems.c_str());
  if (!rules.bound) return;
  Cr& cr = *rules;
  // The gates as they stand in the world: the Race Blueprint's GateAt, as the program reads them.
  kern::logic::Instance* race = rules.rules.find("Race");
  SC_CHECK(race != nullptr, "the Race Blueprint runs");
  if (!race) return;
  const i32 gates = static_cast<i32>(kern::logic::from_value<Cr::List<Cr::combat_GateRow>>(race->get("Gates")).size());
  auto gate = [&](i32 i) {
    std::vector<kern::logic::Value> out;
    race->call("GateAt", {kern::logic::Value::integer(i)}, out);
    return out.empty() ? Cr::combat_GateRow{} : kern::logic::from_value<Cr::combat_GateRow>(out[0]);
  };
  SC_CHECK(gates >= 2, "a course of %d gates", gates);
  Cr::combat_Race r;
  const Cr::combat_GateRow g0 = gate(0);
  Vec3 n, right;
  cr.gate_axes(n, right, 0);
  auto through = [&](i32 i, f64 t) {
    Vec3 nn, rr;
    cr.gate_axes(nn, rr, i);
    const Vec3 c = gate(i).center;
    cr.Race_update(r, c - Vec3{nn.x, nn.y, 0} * 0.5f, t);
    return static_cast<i32>(cr.Race_update(r, c + Vec3{nn.x, nn.y, 0} * 0.5f, t + 0.01).Result);
  };
  SC_CHECK(std::fabs(g0.center.x + 4.5f) < 1e-5f, "course scaled 2.25");
  // Missing the gate (beside it) counts nothing.
  cr.Race_update(r, g0.center + Vec3{right.x, right.y, 0} * 3.0f - Vec3{n.x, n.y, 0}, 0);
  SC_CHECK(cr.Race_update(r, g0.center + Vec3{right.x, right.y, 0} * 3.0f + Vec3{n.x, n.y, 0}, 0.1).Result < 0, "beside the gate: no pass");
  f64 t = 1;
  for (int lap = 0; lap < 3; ++lap)
    for (int i = 0; i < gates; ++i) { SC_CHECK(through(i, t) == i, "gate %d lap %d", i, lap); t += 2; }
  SC_CHECK(through(0, t) == 0 && r.finished && r.lap == 3, "three laps finish");
  SC_CHECK(std::fabs(r.best_lap - 16) < 0.05, "lap time 16 s (%.2f)", static_cast<f64>(r.best_lap));
  SC_CHECK(cr.race_time(75.5).Result.str() == "01:15.500", "time format");
}
