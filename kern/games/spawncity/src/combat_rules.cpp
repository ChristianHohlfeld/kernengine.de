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
rules::Blueprint g_military{"Military"}, g_zombies{"Zombies"}, g_sandbox{"Sandbox"}, g_fun{"Fun"}, g_race{"Race"};

// The tables as the Blueprints have them - their rows, which keep the texts the game's rows point
// into - and the game's rows.
/// The struct EnemySpec of the Blueprint "Sandbox".
struct EnemyRow {
  std::string name;
  f64 hp = 0, speed = 0, range = 0, shot_s = 0;
  i32 burst = 0;
  f64 dmg = 0, acc = 0;
  i32 score = 0;
  f64 scale = 0;
  Vec3 color, stripe;
};
std::vector<EnemyRow> g_enemy_rows;
std::vector<EnemySpec> g_enemies;
/// The struct Contract of the Blueprint "Fun".
struct ContractRow {
  std::string id, name, hint;
  i32 target = 0, reward = 0;
};
std::vector<ContractRow> g_contract_rows;
std::vector<Contract> g_contracts;
/// The struct Gate of the Blueprint "Race".
struct GateRow {
  std::string name;
  Vec3 center;
  f64 width = 0, height = 0;
  bool start = false;
};
std::vector<GateRow> g_gate_rows;
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
  kTriggerHeat = static_cast<f32>(g_military.get<f64>("TriggerHeat"));
  kTriggerHoldS = static_cast<f32>(g_military.get<f64>("TriggerHoldS"));
  kTriggerExtremeHeat = static_cast<f32>(g_military.get<f64>("TriggerExtremeHeat"));
  kAlertS = static_cast<f32>(g_military.get<f64>("AlertS"));
  kReinforceS = static_cast<f32>(g_military.get<f64>("ReinforceS"));
  kFirstReinforceS = static_cast<f32>(g_military.get<f64>("FirstReinforceS"));
  kEndStars = static_cast<i32>(g_military.get<f64>("EndStars"));
  kSquad = static_cast<i32>(g_military.get<f64>("Squad"));
  kMaxSoldiers = static_cast<i32>(g_military.get<f64>("MaxSoldiers"));
  kSoldierHp = static_cast<f32>(g_military.get<f64>("SoldierHp"));
  kMilHit = static_cast<f32>(g_military.get<f64>("MilHit"));
  kHeliHp = static_cast<f32>(g_military.get<f64>("HeliHp"));
  kTankHp = static_cast<f32>(g_military.get<f64>("TankHp"));
  kHeliDmg = static_cast<f32>(g_military.get<f64>("HeliDmg"));
  kHeliDmgDrone = static_cast<f32>(g_military.get<f64>("HeliDmgDrone"));
  kSoldierDmg = static_cast<f32>(g_military.get<f64>("SoldierDmg"));
  kSoldierDmgDrone = static_cast<f32>(g_military.get<f64>("SoldierDmgDrone"));
  kShellDmg = static_cast<f32>(g_military.get<f64>("ShellDmg"));
  kSoldierReactS = static_cast<f32>(g_military.get<f64>("SoldierReactS"));
  kBurstWindowS = static_cast<f32>(g_military.get<f64>("BurstWindowS"));
  kSoldierBursts = static_cast<i32>(g_military.get<f64>("SoldierBursts"));
  kFlakCell = static_cast<f32>(g_military.get<f64>("FlakCell"));
  kFlakChance = static_cast<f32>(g_military.get<f64>("FlakChance"));
  kFlakRange = static_cast<f32>(g_military.get<f64>("FlakRange"));
  kFlakMinAgl = static_cast<f32>(g_military.get<f64>("FlakMinAgl"));
  kFlakMuzzle = static_cast<f32>(g_military.get<f64>("FlakMuzzle"));
  kFlakRps = static_cast<f32>(g_military.get<f64>("FlakRps"));
  kFlakOn = static_cast<f32>(g_military.get<f64>("FlakOn"));
  kFlakOff = static_cast<f32>(g_military.get<f64>("FlakOff"));
  kFlakSpread = static_cast<f32>(g_military.get<f64>("FlakSpread"));
  kFlakBurstR = static_cast<f32>(g_military.get<f64>("FlakBurstR"));
  kFlakHp = static_cast<f32>(g_military.get<f64>("FlakHp"));
  kJetSpeed = static_cast<f32>(g_military.get<f64>("JetSpeed"));
  kSoundMps = static_cast<f32>(g_military.get<f64>("SoundMps"));
  kJetAltMin = static_cast<f32>(g_military.get<f64>("JetAltMin"));
  kJetAltMax = static_cast<f32>(g_military.get<f64>("JetAltMax"));
  kJetSpan = static_cast<f32>(g_military.get<f64>("JetSpan"));
  kAmbientMinS = static_cast<f32>(g_military.get<f64>("AmbientMinS"));
  kAmbientMaxS = static_cast<f32>(g_military.get<f64>("AmbientMaxS"));
  kStrikeCooldownS = static_cast<f32>(g_military.get<f64>("StrikeCooldownS"));
  kBombRadius = static_cast<f32>(g_military.get<f64>("BombRadius"));
  kBombDamage = static_cast<f32>(g_military.get<f64>("BombDamage"));
  kStickLen = static_cast<f32>(g_military.get<f64>("StickLen"));
  kStick = static_cast<i32>(g_military.get<f64>("Stick"));
  kPunchDmg = static_cast<f32>(g_military.get<f64>("PunchDmg"));
  kStabDmg = static_cast<f32>(g_military.get<f64>("StabDmg"));
  kGangGunDmg = static_cast<f32>(g_military.get<f64>("GangGunDmg"));
  return true;
}

namespace {
/// The Clock of the Blueprint "Military", as it takes and gives it.
struct Clock {
  bool active = false;
  f64 since5 = -1, alert_at = -1, next_reinforce = 0;
};
}  // namespace

MilEvent MilitaryClock::update(f32 now, i32 stars, f32 heat, bool enabled) {
  Clock clock;
  MilEvent event = MilEvent::none;
  g_military.call("Update", std::tie(clock, event), Clock{active, rules::decimal(since5), rules::decimal(alert_at), rules::decimal(next_reinforce)}, rules::decimal(now), stars,
                  rules::decimal(heat), enabled);
  active = clock.active;
  since5 = static_cast<f32>(clock.since5);
  alert_at = static_cast<f32>(clock.alert_at);
  next_reinforce = static_cast<f32>(clock.next_reinforce);
  return event;
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
  kZombieCap = static_cast<i32>(g_zombies.get<f64>("ZombieCap"));
  kZombieHit = static_cast<f32>(g_zombies.get<f64>("ZombieHit"));
  kZombieMelee = static_cast<f32>(g_zombies.get<f64>("ZombieMelee"));
  kZombieMeleeS = static_cast<f32>(g_zombies.get<f64>("ZombieMeleeS"));
  kZombieSpawnMin = static_cast<f32>(g_zombies.get<f64>("ZombieSpawnMin"));
  kZombieSpawnMax = static_cast<f32>(g_zombies.get<f64>("ZombieSpawnMax"));
  kZombieHitPts = static_cast<i32>(g_zombies.get<f64>("ZombieHitPts"));
  kZombieKillPts = static_cast<i32>(g_zombies.get<f64>("ZombieKillPts"));
  return true;
}

namespace {
/// The Rounds of the Blueprint "Zombies", as it takes and gives them.
struct Rounds {
  bool active = false;
  i32 round = 0, to_spawn = 0, points = 0;
  f64 next_spawn = 0, break_until = -1;
};
Rounds rounds_of(const ZombieRounds& z) { return {z.active, z.round, z.to_spawn, z.points, rules::decimal(z.next_spawn), rules::decimal(z.break_until)}; }
void take(const Rounds& r, ZombieRounds& z) {
  z.active = r.active;
  z.round = r.round;
  z.to_spawn = r.to_spawn;
  z.points = r.points;
  z.next_spawn = static_cast<f32>(r.next_spawn);
  z.break_until = static_cast<f32>(r.break_until);
}
}  // namespace

ZombieEvent ZombieRounds::update(f32 now, bool night, i32 alive) {
  Rounds r;
  ZombieEvent event = ZombieEvent::none;
  g_zombies.call("Update", std::tie(r, event), rounds_of(*this), rules::decimal(now), night, alive);
  take(r, *this);
  return event;
}
void ZombieRounds::spawned(f32 now) {
  Rounds r;
  g_zombies.call("Spawned", std::tie(r), rounds_of(*this), rules::decimal(now));
  take(r, *this);
}
void ZombieRounds::next_round(f32 now) {
  Rounds r;
  g_zombies.call("NextRound", std::tie(r), rounds_of(*this), rules::decimal(now));
  take(r, *this);
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
  g_enemies.clear();
  kEnemySpawnS = static_cast<f32>(g_sandbox.get<f64>("EnemySpawnS"));
  kComboS = static_cast<f32>(g_sandbox.get<f64>("ComboS"));
  g_enemy_rows = g_sandbox.get<std::vector<EnemyRow>>("Enemies");  // (the rows stay as they are now: the specs point into their texts)
  if (g_enemy_rows.size() < 3) {  // (a row for each EnemyType)
    g_enemy_rows.clear();
    return false;
  }
  for (const EnemyRow& e : g_enemy_rows)
    g_enemies.push_back({e.name.c_str(), static_cast<f32>(e.hp), static_cast<f32>(e.speed), static_cast<f32>(e.range), static_cast<f32>(e.shot_s), e.burst, static_cast<f32>(e.dmg),
                         static_cast<f32>(e.acc), e.score, static_cast<f32>(e.scale), e.color, e.stripe});
  return true;
}

const EnemySpec& enemy_spec(EnemyType t) { return static_cast<std::size_t>(t) < g_enemies.size() ? g_enemies[static_cast<std::size_t>(t)] : kNoEnemy; }

EnemyType choose_enemy(i32 stars, f32 r, i32 heavies) {
  EnemyType type = EnemyType::cop;
  g_sandbox.call("ChooseEnemy", std::tie(type), stars, rules::decimal(r), heavies);
  return type;
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
/// The Chaos of the Blueprint "Sandbox", as it takes and gives it.
struct ChaosRun {
  i32 score = 0, combo = 1;
  f64 combo_until = -1, flow = 0;
};
ChaosRun run_of(const Chaos& c) { return {c.score, c.combo, rules::decimal(c.combo_until), rules::decimal(c.flow)}; }
void take(const ChaosRun& r, Chaos& c) {
  c.score = r.score;
  c.combo = r.combo;
  c.combo_until = static_cast<f32>(r.combo_until);
  c.flow = static_cast<f32>(r.flow);
}
}  // namespace

i32 Chaos::award(i32 points, f32 now) {
  ChaosRun r;
  i32 gained = 0;
  g_sandbox.call("Award", std::tie(r, gained), run_of(*this), points, rules::decimal(now));
  take(r, *this);
  return gained;
}
void Chaos::tick(f32 dt, f32 now) {
  ChaosRun r;
  g_sandbox.call("Tick", std::tie(r), run_of(*this), rules::decimal(dt), rules::decimal(now));
  take(r, *this);
}

// ---------------------------------------------------------------- "Fun"
bool bind_fun(kern::logic::Instance* blueprint) {
  g_fun.running = blueprint;
  g_contracts.clear();
  kContracts = 0;
  kFunComboS = static_cast<f32>(g_fun.get<f64>("FunComboS"));
  kOverdriveS = static_cast<f32>(g_fun.get<f64>("OverdriveS"));
  kMomentumTarget = static_cast<i32>(g_fun.get<f64>("MomentumTarget"));
  g_contract_rows = g_fun.get<std::vector<ContractRow>>("Contracts");  // (the rows stay as they are now: the game's rows point into their texts)
  if (g_contract_rows.empty()) return false;
  for (const ContractRow& c : g_contract_rows) g_contracts.push_back({c.id.c_str(), c.name.c_str(), c.hint.c_str(), c.target, c.reward});
  kContracts = static_cast<i32>(g_contracts.size());
  return true;
}

const Contract& contract(i32 index) { return kContracts > 0 ? g_contracts[static_cast<std::size_t>(((index % kContracts) + kContracts) % kContracts)] : kNoContract; }

namespace {
/// The Run of the Blueprint "Fun", as it takes and gives it.
struct FunRun {
  i32 score = 0, at_risk = 0, high_score = 0, stars = 0, max_stars = 0, combo = 0, best_combo = 0, momentum = 0;
  f64 combo_expires = -1e9, overdrive_until = -1e9;
  i32 contract_index = 0, contract_progress = 0, contracts_completed = 0, mercy = 0, last_award = 0;
  f64 run_damage = 0;
  bool contract_complete = false;
  std::string last_label = "READY";
};
FunRun run_of(const Fun& s) {
  return {s.score, s.at_risk, s.high_score, s.stars, s.max_stars, s.combo, s.best_combo, s.momentum, rules::decimal(s.combo_expires), rules::decimal(s.overdrive_until),
          s.contract_index, s.contract_progress, s.contracts_completed, s.mercy, s.last_award, rules::decimal(s.run_damage), s.contract_complete, s.last_label};
}
void take(const FunRun& r, Fun& s) {
  s.score = r.score;
  s.at_risk = r.at_risk;
  s.high_score = r.high_score;
  s.stars = r.stars;
  s.max_stars = r.max_stars;
  s.combo = r.combo;
  s.best_combo = r.best_combo;
  s.momentum = r.momentum;
  s.combo_expires = static_cast<f32>(r.combo_expires);
  s.overdrive_until = static_cast<f32>(r.overdrive_until);
  s.contract_index = r.contract_index;
  s.contract_progress = r.contract_progress;
  s.contracts_completed = r.contracts_completed;
  s.mercy = r.mercy;
  s.last_award = r.last_award;
  s.run_damage = static_cast<f32>(r.run_damage);
  s.contract_complete = r.contract_complete;
  s.last_label = r.last_label;
}
}  // namespace

std::string Fun::apply(FunEvent e, f32 now, i32 stars_now, f32 amount) {
  FunRun r;
  std::string line;
  g_fun.call("Apply", std::tie(r, line), run_of(*this), e, rules::decimal(now), stars_now, rules::decimal(amount));
  take(r, *this);
  return line;
}
f32 Fun::multiplier(f32 now) const { return static_cast<f32>(g_fun.number("Multiplier", run_of(*this), rules::decimal(now))); }
f32 Fun::police_damage_scale() const {
  f64 scale = 0;
  g_fun.call("PoliceDamageScale", std::tie(scale), mercy);
  return static_cast<f32>(scale);
}

// ---------------------------------------------------------------- "Race"
bool bind_race(kern::logic::Instance* blueprint) {
  g_race.running = blueprint;
  g_gate_rows.clear();
  g_gates.clear();
  kGates = 0;
  kRaceLaps = static_cast<i32>(g_race.get<f64>("RaceLaps"));
  kCourseScale = static_cast<f32>(g_race.get<f64>("CourseScale"));
  const i32 n = static_cast<i32>(g_race.get<std::vector<GateRow>>("Gates").size());
  if (n < 2) return false;
  for (i32 i = 0; i < n; ++i) {  // (each gate as it stands in the world: the Blueprint's GateAt)
    GateRow g;
    g_race.call("GateAt", std::tie(g), i);
    g_gate_rows.push_back(g);
  }
  // (the rows stay as they are now: the game's gates point into their names)
  for (const GateRow& g : g_gate_rows) g_gates.push_back({g.name.c_str(), g.center, static_cast<f32>(g.width), static_cast<f32>(g.height), g.start});
  kGates = static_cast<i32>(g_gates.size());
  return true;
}

const Gate& gate(i32 i) { return kGates > 0 ? g_gates[static_cast<std::size_t>(((i % kGates) + kGates) % kGates)] : kNoGate; }

void gate_axes(i32 i, Vec2& normal, Vec2& right) {
  Vec3 n, r;
  g_race.call("GateAxes", std::tie(n, r), i);
  normal = {n.x, n.y};
  right = {r.x, r.y};
}

void Race::reset() { *this = Race{}; }

bool Race::crossed(i32 gi, Vec3 a, Vec3 b) {
  bool crossed = false;
  g_race.call("Crossed", std::tie(crossed), gi, a, b);
  return crossed;
}

namespace {
/// The Run of the Blueprint "Race", as it takes and gives it.
struct RaceRun {
  bool started = false, finished = false;
  i32 lap = 0, next = 0;
  f64 race_start = 0, lap_start = 0, finish_time = -1, last_lap = -1, best_lap = -1, last_pass = -99;
  bool have_prev = false;
  Vec3 prev;
};
}  // namespace

i32 Race::update(Vec3 p, f32 t) {
  RaceRun r;
  i32 passed = 0;
  g_race.call("Update", std::tie(r, passed),
              RaceRun{started, finished, lap, next, rules::decimal(race_start), rules::decimal(lap_start), rules::decimal(finish_time), rules::decimal(last_lap), rules::decimal(best_lap),
                      rules::decimal(last_pass), have_prev, prev},
              p, rules::decimal(t));
  started = r.started;
  finished = r.finished;
  lap = r.lap;
  next = r.next;
  race_start = static_cast<f32>(r.race_start);
  lap_start = static_cast<f32>(r.lap_start);
  finish_time = static_cast<f32>(r.finish_time);
  last_lap = static_cast<f32>(r.last_lap);
  best_lap = static_cast<f32>(r.best_lap);
  last_pass = static_cast<f32>(r.last_pass);
  have_prev = r.have_prev;
  prev = r.prev;
  return passed;
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
