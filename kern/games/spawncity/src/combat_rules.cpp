// Combat and event rules: the Blueprints of the level's actors "Military", "Zombies", "Sandbox",
// "Fun" and "Race" compute them (see combat_rules.h). Here: the calls, the numbers and tables as
// the game reads them, and the hashes.
#include "combat_rules.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "blueprint_call.h"

namespace sc::combat {

// The Blueprints' variables (read when a Blueprint is bound).
f32 kTriggerHeat = 0;
f32 kTriggerHoldS = 0;
f32 kTriggerExtremeHeat = 0;
f32 kAlertS = 0;
f32 kReinforceS = 0;
f32 kFirstReinforceS = 0;
i32 kEndStars = 0;
i32 kSquad = 0;
i32 kMaxSoldiers = 0;
f32 kSoldierHp = 0;
f32 kMilHit = 0;
f32 kHeliHp = 0;
f32 kTankHp = 0;
f32 kHeliDmg = 0;
f32 kHeliDmgDrone = 0;
f32 kSoldierDmg = 0;
f32 kSoldierDmgDrone = 0;
f32 kShellDmg = 0;
f32 kSoldierReactS = 0;
f32 kBurstWindowS = 0;
i32 kSoldierBursts = 0;
f32 kFlakCell = 0;
f32 kFlakChance = 0;
f32 kFlakRange = 0;
f32 kFlakMinAgl = 0;
f32 kFlakMuzzle = 0;
f32 kFlakRps = 0;
f32 kFlakOn = 0;
f32 kFlakOff = 0;
f32 kFlakSpread = 0;
f32 kFlakBurstR = 0;
f32 kFlakHp = 0;
f32 kJetSpeed = 0;
f32 kSoundMps = 0;
f32 kJetAltMin = 0;
f32 kJetAltMax = 0;
f32 kJetSpan = 0;
f32 kAmbientMinS = 0;
f32 kAmbientMaxS = 0;
f32 kStrikeCooldownS = 0;
f32 kBombRadius = 0;
f32 kBombDamage = 0;
f32 kStickLen = 0;
i32 kStick = 0;
f32 kPunchDmg = 0;
f32 kStabDmg = 0;
f32 kGangGunDmg = 0;
i32 kZombieCap = 0;
f32 kZombieHit = 0;
f32 kZombieMelee = 0;
f32 kZombieMeleeS = 0;
f32 kZombieSpawnMin = 0;
f32 kZombieSpawnMax = 0;
i32 kZombieHitPts = 0;
i32 kZombieKillPts = 0;
f32 kEnemySpawnS = 0;
f32 kComboS = 0;
f32 kFunComboS = 0;
f32 kOverdriveS = 0;
i32 kMomentumTarget = 0;
i32 kContracts = 0;
i32 kRaceLaps = 0;
i32 kGates = 0;
f32 kCourseScale = 0;

namespace {
using kern::logic::Value;
rules::Blueprint g_military{"Military"}, g_zombies{"Zombies"}, g_sandbox{"Sandbox"}, g_fun{"Fun"}, g_race{"Race"};
Value N(f32 x) { return rules::number(x); }
Value I(i64 x) { return Value::integer(x); }
Value B(bool x) { return Value::boolean(x); }
f32 f(const Value& v) { return static_cast<f32>(v.as_number()); }
i32 whole(const Value& v) { return static_cast<i32>(v.i); }
f32 num(const rules::Blueprint& b, const char* function, const std::vector<Value>& args) { return static_cast<f32>(b.number(function, args)); }
Vec3 vec(const Value& v) { return {static_cast<f32>(v.v.x), static_cast<f32>(v.v.y), static_cast<f32>(v.v.z)}; }

// The tables as the Blueprints have them (the rows' texts are kept here: the rows point into them).
struct EnemyText { std::string name; };
std::vector<EnemyText> g_enemy_texts;
std::vector<EnemySpec> g_enemies;
struct ContractText { std::string id, name, hint; };
std::vector<ContractText> g_contract_texts;
std::vector<Contract> g_contracts;
std::vector<std::string> g_gate_names;
std::vector<Gate> g_gates;
const EnemySpec kNoEnemy = {"", 1, 0, 0, 1, 1, 0, 0, 0, 1, {}, {}};
const Contract kNoContract = {"", "", "", 1, 0};
const Gate kNoGate = {"", {}, 1, 1, false};
}  // namespace

// ---------------------------------------------------------------- "Military"
bool bind_military(kern::logic::Instance* blueprint) {
  g_military.running = blueprint;
  kTriggerHeat = kTriggerHoldS = kTriggerExtremeHeat = kAlertS = kReinforceS = kFirstReinforceS = kEndStars = kSquad = kMaxSoldiers = kSoldierHp = kMilHit = kHeliHp = kTankHp = kHeliDmg = kHeliDmgDrone = kSoldierDmg = kSoldierDmgDrone = kShellDmg = kSoldierReactS = kBurstWindowS = kSoldierBursts = kFlakCell = kFlakChance = kFlakRange = kFlakMinAgl = kFlakMuzzle = kFlakRps = kFlakOn = kFlakOff = kFlakSpread = kFlakBurstR = kFlakHp = kJetSpeed = kSoundMps = kJetAltMin = kJetAltMax = kJetSpan = kAmbientMinS = kAmbientMaxS = kStrikeCooldownS = kBombRadius = kBombDamage = kStickLen = kStick = kPunchDmg = kStabDmg = kGangGunDmg = 0;
  if (!blueprint) return false;
  kTriggerHeat = static_cast<f32>(g_military.variable("TriggerHeat"));
  kTriggerHoldS = static_cast<f32>(g_military.variable("TriggerHoldS"));
  kTriggerExtremeHeat = static_cast<f32>(g_military.variable("TriggerExtremeHeat"));
  kAlertS = static_cast<f32>(g_military.variable("AlertS"));
  kReinforceS = static_cast<f32>(g_military.variable("ReinforceS"));
  kFirstReinforceS = static_cast<f32>(g_military.variable("FirstReinforceS"));
  kEndStars = static_cast<i32>(g_military.variable("EndStars"));
  kSquad = static_cast<i32>(g_military.variable("Squad"));
  kMaxSoldiers = static_cast<i32>(g_military.variable("MaxSoldiers"));
  kSoldierHp = static_cast<f32>(g_military.variable("SoldierHp"));
  kMilHit = static_cast<f32>(g_military.variable("MilHit"));
  kHeliHp = static_cast<f32>(g_military.variable("HeliHp"));
  kTankHp = static_cast<f32>(g_military.variable("TankHp"));
  kHeliDmg = static_cast<f32>(g_military.variable("HeliDmg"));
  kHeliDmgDrone = static_cast<f32>(g_military.variable("HeliDmgDrone"));
  kSoldierDmg = static_cast<f32>(g_military.variable("SoldierDmg"));
  kSoldierDmgDrone = static_cast<f32>(g_military.variable("SoldierDmgDrone"));
  kShellDmg = static_cast<f32>(g_military.variable("ShellDmg"));
  kSoldierReactS = static_cast<f32>(g_military.variable("SoldierReactS"));
  kBurstWindowS = static_cast<f32>(g_military.variable("BurstWindowS"));
  kSoldierBursts = static_cast<i32>(g_military.variable("SoldierBursts"));
  kFlakCell = static_cast<f32>(g_military.variable("FlakCell"));
  kFlakChance = static_cast<f32>(g_military.variable("FlakChance"));
  kFlakRange = static_cast<f32>(g_military.variable("FlakRange"));
  kFlakMinAgl = static_cast<f32>(g_military.variable("FlakMinAgl"));
  kFlakMuzzle = static_cast<f32>(g_military.variable("FlakMuzzle"));
  kFlakRps = static_cast<f32>(g_military.variable("FlakRps"));
  kFlakOn = static_cast<f32>(g_military.variable("FlakOn"));
  kFlakOff = static_cast<f32>(g_military.variable("FlakOff"));
  kFlakSpread = static_cast<f32>(g_military.variable("FlakSpread"));
  kFlakBurstR = static_cast<f32>(g_military.variable("FlakBurstR"));
  kFlakHp = static_cast<f32>(g_military.variable("FlakHp"));
  kJetSpeed = static_cast<f32>(g_military.variable("JetSpeed"));
  kSoundMps = static_cast<f32>(g_military.variable("SoundMps"));
  kJetAltMin = static_cast<f32>(g_military.variable("JetAltMin"));
  kJetAltMax = static_cast<f32>(g_military.variable("JetAltMax"));
  kJetSpan = static_cast<f32>(g_military.variable("JetSpan"));
  kAmbientMinS = static_cast<f32>(g_military.variable("AmbientMinS"));
  kAmbientMaxS = static_cast<f32>(g_military.variable("AmbientMaxS"));
  kStrikeCooldownS = static_cast<f32>(g_military.variable("StrikeCooldownS"));
  kBombRadius = static_cast<f32>(g_military.variable("BombRadius"));
  kBombDamage = static_cast<f32>(g_military.variable("BombDamage"));
  kStickLen = static_cast<f32>(g_military.variable("StickLen"));
  kStick = static_cast<i32>(g_military.variable("Stick"));
  kPunchDmg = static_cast<f32>(g_military.variable("PunchDmg"));
  kStabDmg = static_cast<f32>(g_military.variable("StabDmg"));
  kGangGunDmg = static_cast<f32>(g_military.variable("GangGunDmg"));
  return true;
}

MilEvent MilitaryClock::update(f32 now, i32 stars, f32 heat, bool enabled) {
  std::vector<Value> r;
  if (!g_military.call("Update", {Value::structure({B(active), N(since5), N(alert_at), N(next_reinforce)}), N(now), I(stars), N(heat), B(enabled)}, r) || r.size() < 2 || r[0].fields() < 4)
    return MilEvent::none;
  active = r[0].field(0)->b;
  since5 = f(*r[0].field(1));
  alert_at = f(*r[0].field(2));
  next_reinforce = f(*r[0].field(3));
  return static_cast<MilEvent>(r[1].i);
}
f32 heli_hit_chance(f32 d, f32 speed) {
  f64 chance = 0;
  g_military.call("HeliHitChance", std::tie(chance), rules::decimal(d), rules::decimal(speed));
  return static_cast<f32>(chance);
}
f32 soldier_hit_chance(f32 d, f32 speed) {
  f64 chance = 0;
  g_military.call("SoldierHitChance", std::tie(chance), rules::decimal(d), rules::decimal(speed));
  return static_cast<f32>(chance);
}
f32 tank_aim_error(f32 d, f32 speed) {
  f64 metres = 0;
  g_military.call("TankAimError", std::tie(metres), rules::decimal(d), rules::decimal(speed));
  return static_cast<f32>(metres);
}
f32 gangster_hit_chance(f32 d, f32 speed) {
  f64 chance = 0;
  g_military.call("GangsterHitChance", std::tie(chance), rules::decimal(d), rules::decimal(speed));
  return static_cast<f32>(chance);
}
f32 strike_launch_time(f32 now) {
  f64 time = 0;
  g_military.call("StrikeLaunchTime", std::tie(time), rules::decimal(now));
  return static_cast<f32>(time);
}
f32 flak_burst_damage(f32 d) {
  f64 damage = 0;
  g_military.call("FlakBurstDamage", std::tie(damage), rules::decimal(d));
  return static_cast<f32>(damage);
}

// flak_battery.mjs hash() and rand(): a cell's own numbers
u32 hash_text(const std::string& s) {
  u32 h = 2166136261u;
  for (unsigned char c : s) { h ^= c; h *= 16777619u; }
  return h;
}

f32 cell_rand(u32 seed, u32 i) {
  u32 x = seed ^ ((i + 1) * 0x9e3779b1u);
  x ^= x >> 16;
  x *= 0x7feb352du;
  x ^= x >> 15;
  x *= 0x846ca68bu;
  x ^= x >> 16;
  return static_cast<f32>(static_cast<f64>(x) / 4294967296.0);
}

bool flak_cell(i64 cx, i64 cy, u32& seed) {
  char key[64];
  std::snprintf(key, sizeof key, "flak:l%lld:%lld", static_cast<long long>(cx), static_cast<long long>(cy));
  seed = hash_text(key);
  bool gun = false;
  g_military.call("FlakHasGun", std::tie(gun), rules::decimal(cell_rand(seed, 0)));
  return gun;
}

// pedestrian_agents.mjs agentRandom: a person's own number
f32 agent_random(u32 seed) { return static_cast<f32>((seed * 2246822519u) >> 8) * (1.0f / 16777216.0f); }

Temper temper_of(u32 seed) {
  Temper temper = Temper::fearful;
  g_military.call("TemperOf", std::tie(temper), rules::decimal(agent_random(seed)));
  return temper;
}

Arms arms_of(u32 seed) {
  // (his second number, from the first)
  u32 h = (static_cast<u32>(static_cast<f64>(agent_random(seed)) * 4294967296.0) ^ 0x9e3779b9u) * 2654435761u;
  h ^= h >> 15;
  Arms arms = Arms::fists;
  g_military.call("ArmsOf", std::tie(arms), temper_of(seed), static_cast<f64>(h % 10000) / 10000.0);
  return arms;
}

// ---------------------------------------------------------------- "Zombies"
bool bind_zombies(kern::logic::Instance* blueprint) {
  g_zombies.running = blueprint;
  kZombieCap = kZombieHit = kZombieMelee = kZombieMeleeS = kZombieSpawnMin = kZombieSpawnMax = kZombieHitPts = kZombieKillPts = 0;
  if (!blueprint) return false;
  kZombieCap = static_cast<i32>(g_zombies.variable("ZombieCap"));
  kZombieHit = static_cast<f32>(g_zombies.variable("ZombieHit"));
  kZombieMelee = static_cast<f32>(g_zombies.variable("ZombieMelee"));
  kZombieMeleeS = static_cast<f32>(g_zombies.variable("ZombieMeleeS"));
  kZombieSpawnMin = static_cast<f32>(g_zombies.variable("ZombieSpawnMin"));
  kZombieSpawnMax = static_cast<f32>(g_zombies.variable("ZombieSpawnMax"));
  kZombieHitPts = static_cast<i32>(g_zombies.variable("ZombieHitPts"));
  kZombieKillPts = static_cast<i32>(g_zombies.variable("ZombieKillPts"));
  return true;
}

namespace {
Value to_value(const ZombieRounds& z) { return Value::structure({B(z.active), I(z.round), I(z.to_spawn), I(z.points), N(z.next_spawn), N(z.break_until)}); }
void take(const Value& v, ZombieRounds& z) {
  if (v.fields() < 6) return;
  z.active = v.field(0)->b;
  z.round = whole(*v.field(1));
  z.to_spawn = whole(*v.field(2));
  z.points = whole(*v.field(3));
  z.next_spawn = f(*v.field(4));
  z.break_until = f(*v.field(5));
}
}  // namespace

ZombieEvent ZombieRounds::update(f32 now, bool night, i32 alive) {
  std::vector<Value> r;
  if (!g_zombies.call("Update", {to_value(*this), N(now), B(night), I(alive)}, r) || r.size() < 2) return ZombieEvent::none;
  take(r[0], *this);
  return static_cast<ZombieEvent>(r[1].i);
}
void ZombieRounds::spawned(f32 now) {
  std::vector<Value> r;
  if (g_zombies.call("Spawned", {to_value(*this), N(now)}, r) && !r.empty()) take(r[0], *this);
}
void ZombieRounds::next_round(f32 now) {
  std::vector<Value> r;
  if (g_zombies.call("NextRound", {to_value(*this), N(now)}, r) && !r.empty()) take(r[0], *this);
}
f32 ZombieRounds::hp() const {
  f64 hp = 0;
  g_zombies.call("Hp", std::tie(hp), round);
  return static_cast<f32>(hp);
}
f32 ZombieRounds::speed(f32 r_fast, f32 r_speed) const {
  f64 mps = 0;
  g_zombies.call("Speed", std::tie(mps), round, rules::decimal(r_fast), rules::decimal(r_speed));
  return static_cast<f32>(mps);
}

// ---------------------------------------------------------------- "Sandbox"
bool bind_sandbox(kern::logic::Instance* blueprint) {
  g_sandbox.running = blueprint;
  g_enemy_texts.clear();
  g_enemies.clear();
  kEnemySpawnS = kComboS = 0;
  if (!blueprint) return false;
  kEnemySpawnS = static_cast<f32>(g_sandbox.variable("EnemySpawnS"));
  kComboS = static_cast<f32>(g_sandbox.variable("ComboS"));
  const Value table = blueprint->get("Enemies");
  if (table.size() < 3) return false;  // (a row for each EnemyType)
  g_enemy_texts.reserve(table.size());
  for (std::size_t i = 0; i < table.size(); ++i) {
    const Value row = table.item(i);
    if (row.fields() < 12) { g_enemy_texts.clear(); g_enemies.clear(); return false; }
    g_enemy_texts.push_back({row.field(0)->as_text()});
    g_enemies.push_back({g_enemy_texts.back().name.c_str(), f(*row.field(1)), f(*row.field(2)), f(*row.field(3)), f(*row.field(4)), whole(*row.field(5)), f(*row.field(6)), f(*row.field(7)),
                         whole(*row.field(8)), f(*row.field(9)), vec(*row.field(10)), vec(*row.field(11))});
  }
  return true;
}

const EnemySpec& enemy_spec(EnemyType t) { return static_cast<std::size_t>(t) < g_enemies.size() ? g_enemies[static_cast<std::size_t>(t)] : kNoEnemy; }

bool choose_enemy(i32 stars, f32 r, i32 heavies, EnemyType& out) {
  std::vector<Value> v;
  if (!g_sandbox.call("ChooseEnemy", {I(stars), N(r), I(heavies)}, v) || v.empty()) return false;
  out = static_cast<EnemyType>(v[0].i);
  return true;
}
f32 enemy_hit_chance(const EnemySpec& s, f32 d, f32 moving) {
  f64 chance = 0;
  g_sandbox.call("EnemyHitChance", std::tie(chance), rules::decimal(s.acc), rules::decimal(d), rules::decimal(moving));
  return static_cast<f32>(chance);
}
i32 enemy_budget(i32 stars) {
  i32 enemies = 0;
  g_sandbox.call("Budget", std::tie(enemies), stars);
  return enemies;
}

namespace {
Value to_value(const Chaos& c) { return Value::structure({I(c.score), I(c.combo), N(c.combo_until), N(c.flow)}); }
void take(const Value& v, Chaos& c) {
  if (v.fields() < 4) return;
  c.score = whole(*v.field(0));
  c.combo = whole(*v.field(1));
  c.combo_until = f(*v.field(2));
  c.flow = f(*v.field(3));
}
}  // namespace

i32 Chaos::award(i32 points, f32 now) {
  std::vector<Value> r;
  if (!g_sandbox.call("Award", {to_value(*this), I(points), N(now)}, r) || r.size() < 2) return 0;
  take(r[0], *this);
  return whole(r[1]);
}
void Chaos::tick(f32 dt, f32 now) {
  std::vector<Value> r;
  if (g_sandbox.call("Tick", {to_value(*this), N(dt), N(now)}, r) && !r.empty()) take(r[0], *this);
}

// ---------------------------------------------------------------- "Fun"
bool bind_fun(kern::logic::Instance* blueprint) {
  g_fun.running = blueprint;
  g_contract_texts.clear();
  g_contracts.clear();
  kContracts = 0;
  kFunComboS = kOverdriveS = kMomentumTarget = 0;
  if (!blueprint) return false;
  kFunComboS = static_cast<f32>(g_fun.variable("FunComboS"));
  kOverdriveS = static_cast<f32>(g_fun.variable("OverdriveS"));
  kMomentumTarget = static_cast<i32>(g_fun.variable("MomentumTarget"));
  const Value table = blueprint->get("Contracts");
  if (table.size() == 0) return false;
  g_contract_texts.reserve(table.size());
  for (std::size_t i = 0; i < table.size(); ++i) {
    const Value row = table.item(i);
    if (row.fields() < 5) { g_contract_texts.clear(); g_contracts.clear(); return false; }
    g_contract_texts.push_back({row.field(0)->as_text(), row.field(1)->as_text(), row.field(2)->as_text()});
    const ContractText& t = g_contract_texts.back();
    g_contracts.push_back({t.id.c_str(), t.name.c_str(), t.hint.c_str(), whole(*row.field(3)), whole(*row.field(4))});
  }
  kContracts = static_cast<i32>(g_contracts.size());
  return true;
}

const Contract& contract(i32 index) { return kContracts > 0 ? g_contracts[static_cast<std::size_t>(((index % kContracts) + kContracts) % kContracts)] : kNoContract; }

namespace {
Value to_value(const Fun& s) {
  return Value::structure({I(s.score), I(s.at_risk), I(s.high_score), I(s.stars), I(s.max_stars), I(s.combo), I(s.best_combo), I(s.momentum), N(s.combo_expires), N(s.overdrive_until),
                           I(s.contract_index), I(s.contract_progress), I(s.contracts_completed), I(s.mercy), I(s.last_award), N(s.run_damage), B(s.contract_complete),
                           Value::text(s.last_label)});
}
void take(const Value& v, Fun& s) {
  if (v.fields() < 18) return;
  s.score = whole(*v.field(0));
  s.at_risk = whole(*v.field(1));
  s.high_score = whole(*v.field(2));
  s.stars = whole(*v.field(3));
  s.max_stars = whole(*v.field(4));
  s.combo = whole(*v.field(5));
  s.best_combo = whole(*v.field(6));
  s.momentum = whole(*v.field(7));
  s.combo_expires = f(*v.field(8));
  s.overdrive_until = f(*v.field(9));
  s.contract_index = whole(*v.field(10));
  s.contract_progress = whole(*v.field(11));
  s.contracts_completed = whole(*v.field(12));
  s.mercy = whole(*v.field(13));
  s.last_award = whole(*v.field(14));
  s.run_damage = f(*v.field(15));
  s.contract_complete = v.field(16)->b;
  s.last_label = v.field(17)->as_text();
}
}  // namespace

std::string Fun::apply(FunEvent e, f32 now, i32 stars_now, f32 amount) {
  std::vector<Value> r;
  if (!g_fun.call("Apply", {to_value(*this), rules::of_enum(e), N(now), I(stars_now), N(amount)}, r) || r.size() < 2) return {};
  take(r[0], *this);
  return r[1].as_text();
}
f32 Fun::multiplier(f32 now) const { return num(g_fun, "Multiplier", {to_value(*this), N(now)}); }
f32 Fun::police_damage_scale() const {
  f64 scale = 0;
  g_fun.call("PoliceDamageScale", std::tie(scale), mercy);
  return static_cast<f32>(scale);
}

// ---------------------------------------------------------------- "Race"
bool bind_race(kern::logic::Instance* blueprint) {
  g_race.running = blueprint;
  g_gate_names.clear();
  g_gates.clear();
  kGates = 0;
  kRaceLaps = kCourseScale = 0;
  if (!blueprint) return false;
  kRaceLaps = static_cast<i32>(g_race.variable("RaceLaps"));
  kCourseScale = static_cast<f32>(g_race.variable("CourseScale"));
  const std::size_t n = blueprint->get("Gates").size();
  if (n < 2) return false;
  g_gate_names.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {  // (each gate as it stands in the world: the Blueprint's GateAt)
    std::vector<Value> r;
    if (!g_race.call("GateAt", {I(static_cast<i64>(i))}, r) || r.empty() || r[0].fields() < 5) { g_gate_names.clear(); g_gates.clear(); return false; }
    g_gate_names.push_back(r[0].field(0)->as_text());
    g_gates.push_back({g_gate_names.back().c_str(), vec(*r[0].field(1)), f(*r[0].field(2)), f(*r[0].field(3)), r[0].field(4)->b});
  }
  kGates = static_cast<i32>(g_gates.size());
  return true;
}

const Gate& gate(i32 i) { return kGates > 0 ? g_gates[static_cast<std::size_t>(((i % kGates) + kGates) % kGates)] : kNoGate; }

void gate_axes(i32 i, Vec2& normal, Vec2& right) {
  std::vector<Value> r;
  if (!g_race.call("GateAxes", {I(i)}, r) || r.size() < 2) { normal = {1, 0}; right = {0, 1}; return; }
  normal = {static_cast<f32>(r[0].v.x), static_cast<f32>(r[0].v.y)};
  right = {static_cast<f32>(r[1].v.x), static_cast<f32>(r[1].v.y)};
}

void Race::reset() { *this = Race{}; }

bool Race::crossed(i32 gi, Vec3 a, Vec3 b) {
  bool crossed = false;
  g_race.call("Crossed", std::tie(crossed), gi, a, b);
  return crossed;
}

i32 Race::update(Vec3 p, f32 t) {
  std::vector<Value> r;
  if (!g_race.call("Update", {Value::structure({B(started), B(finished), I(lap), I(next), N(race_start), N(lap_start), N(finish_time), N(last_lap), N(best_lap), N(last_pass), B(have_prev),
                                                Value::vector(prev)}),
                              Value::vector(p), N(t)},
                   r) ||
      r.size() < 2 || r[0].fields() < 12)
    return -1;
  const Value& v = r[0];
  started = v.field(0)->b;
  finished = v.field(1)->b;
  lap = whole(*v.field(2));
  next = whole(*v.field(3));
  race_start = f(*v.field(4));
  lap_start = f(*v.field(5));
  finish_time = f(*v.field(6));
  last_lap = f(*v.field(7));
  best_lap = f(*v.field(8));
  last_pass = f(*v.field(9));
  have_prev = v.field(10)->b;
  prev = vec(*v.field(11));
  return whole(r[1]);
}

// (how a time is written: "01:15.500")
std::string race_time(f32 s) {
  if (!(s >= 0)) return "-";
  const int m = static_cast<int>(s / 60);
  char b[32];
  std::snprintf(b, sizeof b, "%02d:%06.3f", m, s - static_cast<f32>(m) * 60);
  return b;
}

}  // namespace sc::combat
