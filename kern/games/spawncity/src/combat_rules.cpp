// Combat and event rules (see combat_rules.h).
#include "combat_rules.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace sc::combat {

// ---------------------------------------------------------------- military_response.mjs frame()
MilEvent MilitaryClock::update(f32 now, i32 stars, f32 heat, bool enabled) {
  if (!enabled) { active = false; since5 = alert_at = -1; return MilEvent::none; }
  if (stars >= 5) { if (since5 < 0) since5 = now; }
  else since5 = -1;
  if (!active && alert_at < 0 && stars >= 5 && ((heat >= kTriggerHeat && now - since5 > kTriggerHoldS) || heat >= kTriggerExtremeHeat)) {
    alert_at = now;
    return MilEvent::alert;
  }
  if (alert_at >= 0 && !active && stars < 5) { alert_at = -1; return MilEvent::cancelled; }
  if (alert_at >= 0 && !active && now - alert_at > kAlertS) {
    alert_at = -1;
    active = true;
    next_reinforce = now + kFirstReinforceS;
    return MilEvent::start;
  }
  if (active && stars <= kEndStars) { active = false; return MilEvent::stop; }
  return MilEvent::none;
}

// ---------------------------------------------------------------- flak_battery.mjs
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
  return cell_rand(seed, 0) <= kFlakChance;
}

f32 flak_burst_damage(f32 d) { return d > kFlakBurstR ? 0.0f : std::round(14 * (1 - d / kFlakBurstR) + 2); }

// ---------------------------------------------------------------- pedestrian_agents.mjs
f32 agent_random(u32 seed) { return static_cast<f32>((seed * 2246822519u) >> 8) * (1.0f / 16777216.0f); }

Temper temper_of(u32 seed) {
  const f32 r = agent_random(seed);
  return r < 0.06f ? Temper::aggressive : r < 0.36f ? Temper::runner : Temper::fearful;
}

Arms arms_of(u32 seed) {
  if (temper_of(seed) != Temper::aggressive) return Arms::fists;
  u32 h = (static_cast<u32>(static_cast<f64>(agent_random(seed)) * 4294967296.0) ^ 0x9e3779b9u) * 2654435761u;
  h ^= h >> 15;
  const f32 r = static_cast<f32>(h % 10000) / 10000;
  return r < 0.03f ? Arms::gun : r < 0.2f ? Arms::knife : Arms::fists;
}

// ---------------------------------------------------------------- zombie_nights.mjs frame()
ZombieEvent ZombieRounds::update(f32 now, bool night, i32 alive) {
  if (night && !active) { active = true; round = 0; next_round(now); return ZombieEvent::night; }
  if (!night && active) { active = false; to_spawn = 0; return ZombieEvent::dawn; }
  if (!active) return ZombieEvent::none;
  if (to_spawn > 0 && now >= next_spawn && alive < kZombieCap) return ZombieEvent::spawn;
  if (to_spawn <= 0 && alive == 0) {
    if (break_until < 0) { break_until = now + 9; return ZombieEvent::round_done; }
    if (now > break_until) { next_round(now); return ZombieEvent::round; }
  }
  return ZombieEvent::none;
}

f32 ZombieRounds::speed(f32 r_fast, f32 r_speed) const {
  const bool fast = round >= 3 && r_fast < std::fmin(0.55f, 0.12f * static_cast<f32>(round - 2));
  return fast ? 3 + r_speed * 1.1f : 0.85f + r_speed * 0.7f + static_cast<f32>(round) * 0.06f;
}

// ---------------------------------------------------------------- sandbox_game.mjs
const EnemySpec& enemy_spec(EnemyType t) {
  // TYPES: hp in hits (every hit costs 1), speed m/s, range m, shot interval, burst, damage, accuracy
  static const EnemySpec kSpecs[] = {
      {"COP", 3, 3.6f, 24, 0.9f, 1, 5, 0.62f, 250, 1.0f, {0.18f, 0.42f, 1.0f}, {0.16f, 0.9f, 1.0f}},
      {"SWAT", 7, 2.9f, 32, 1.7f, 3, 4, 0.5f, 450, 1.08f, {0.11f, 0.13f, 0.19f}, {1.0f, 0.82f, 0.25f}},
      {"HEAVY", 14, 2.1f, 48, 3.6f, 1, 0, 0.0f, 900, 1.22f, {0.48f, 0.08f, 0.19f}, {1.0f, 0.7f, 0.28f}},
  };
  return kSpecs[static_cast<u8>(t)];
}

bool choose_enemy(i32 stars, f32 r, i32 heavies, EnemyType& out) {
  if (stars >= 4 && r < 0.22f && heavies < 2) { out = EnemyType::heavy; return true; }
  out = stars >= 2 && r < 0.5f ? EnemyType::swat : EnemyType::cop;
  return true;
}

i32 Chaos::award(i32 points, f32 now) {
  combo = now < combo_until ? std::min(8, combo + 1) : 1;
  combo_until = now + kComboS;
  flow = std::fmin(1.0f, flow + 0.18f);
  const i32 gained = static_cast<i32>(std::round(static_cast<f32>(points * combo)));
  score += gained;
  return gained;
}

void Chaos::tick(f32 dt, f32 now) {
  flow = std::fmax(0.0f, flow - dt * 0.06f);
  if (now > combo_until && combo > 1) combo = 1;
}

// ---------------------------------------------------------------- gameplay_fun_logic.mjs
namespace {
constexpr Contract kContractList[kContracts] = {
    {"hot-escape", "HOT ESCAPE", "2 STERNE ERREICHEN, SICHT BRECHEN, ENTKOMMEN", 1, 800},
    {"drone-hunter", "DRONE HUNTER", "2 POLIZEIDROHNEN ZERSTOEREN", 2, 950},
    {"emp-chain", "EMP CHAIN", "3 DROHNEN MIT EMP AUSSCHALTEN", 3, 850},
    {"vehicle-chaos", "VEHICLE CHAOS", "3 AUTOS ODER BUSSE ZERSTOEREN", 3, 700},
    {"untouchable", "UNTOUCHABLE", "MIT 2+ STERNEN OHNE SCHADEN ENTKOMMEN", 1, 1200},
};
i32 rounded(f32 v) { return std::max(0, static_cast<i32>(std::round(v / 10)) * 10); }
}  // namespace

const Contract& contract(i32 index) { return kContractList[((index % kContracts) + kContracts) % kContracts]; }

f32 Fun::multiplier(f32 now) const {
  const f32 heat = static_cast<f32>(std::clamp(stars, 0, 5)), c = static_cast<f32>(std::clamp(combo, 0, 20));
  const f32 base = 1 + heat * 0.25f + std::floor(c / 2) * 0.2f, od = now < overdrive_until ? 1.75f : 1.0f;
  return std::fmin(4.5f, base * od);
}

void Fun::clock(f32 now, i32 st) {
  stars = std::clamp(st, 0, 5);
  max_stars = std::max(max_stars, stars);
  if (combo > 0 && now >= combo_expires) { combo = 0; combo_expires = -1e9f; }
}

void Fun::progress(FunEvent e, i32 affected, std::string& out) {
  if (contract_complete) return;
  const Contract& c = contract(contract_index);
  const std::string id = c.id;
  const i32 peak = std::max(stars, max_stars);
  i32 delta = 0;
  if (id == "hot-escape") delta = e == FunEvent::escape && peak >= 2;
  else if (id == "drone-hunter") delta = e == FunEvent::police_kill;
  else if (id == "emp-chain") delta = e == FunEvent::emp ? affected : 0;
  else if (id == "vehicle-chaos") delta = e == FunEvent::world_kill_car || e == FunEvent::world_kill_bus;
  else if (id == "untouchable") delta = e == FunEvent::escape && peak >= 2 && run_damage == 0;
  if (delta <= 0) return;
  contract_progress = std::min(c.target, contract_progress + delta);
  if (contract_progress >= c.target) {
    contract_complete = true;
    ++contracts_completed;
    score += c.reward;
    high_score = std::max(high_score, score);
    out = std::string("AUFTRAG ") + c.name + " +" + std::to_string(c.reward);
  }
}

std::string Fun::skill(FunEvent e, f32 now, f32 base, const char* label, i32 momentum_add, i32 affected) {
  ++combo;
  best_combo = std::max(best_combo, combo);
  combo_expires = now + kFunComboS;
  momentum += std::max(0, momentum_add);
  i32 surge = 0;
  std::string out;
  if (momentum >= kMomentumTarget) {
    momentum -= kMomentumTarget;
    overdrive_until = now + kOverdriveS;
    surge = 250 + std::min(250, combo * 25);
  }
  const f32 m = multiplier(now);
  const i32 a = rounded(base * m + static_cast<f32>(surge));
  at_risk += a;
  last_award = a;
  last_label = label;
  out = std::string(label) + " +" + std::to_string(a) + (surge ? " OVERDRIVE" : "");
  progress(e, affected, out);
  return out;
}

std::string Fun::apply(FunEvent e, f32 now, i32 st, f32 amount) {
  clock(now, st);
  switch (e) {
    case FunEvent::world_kill_person: return skill(e, now, 90, "TARGET DOWN", 1);
    case FunEvent::world_kill_car: return skill(e, now, 150, "CAR WRECKED", 1);
    case FunEvent::world_kill_bus: return skill(e, now, 260, "BUS WRECKED", 1);
    case FunEvent::world_kill_player: return skill(e, now, 340, "HIT CONFIRMED", 1);
    case FunEvent::police_kill: return skill(e, now, 430, "POLICE DRONE DOWN", 2);
    case FunEvent::emp: {
      const i32 n = static_cast<i32>(std::floor(amount));
      if (n <= 0) return "";
      static char label[24];
      std::snprintf(label, sizeof label, "EMP x%d", n);
      return skill(e, now, 120.0f * static_cast<f32>(n), label, n >= 2 ? 2 : 1, n);
    }
    case FunEvent::escape: {
      const i32 peak = std::max({max_stars, stars, st});
      const i32 payout = rounded(static_cast<f32>(at_risk + peak * 220 + std::max(0, combo - 1) * 35));
      std::string out;
      stars = peak;
      progress(e, 0, out);
      score += payout;
      at_risk = 0;
      high_score = std::max(high_score, score);
      combo = 0;
      combo_expires = -1e9f;
      momentum = std::min(momentum, kMomentumTarget - 1);
      max_stars = stars = 0;
      run_damage = 0;
      mercy = std::max(0, mercy - 1);
      last_award = payout;
      last_label = "ESCAPED";
      return "ENTKOMMEN +" + std::to_string(payout) + (out.empty() ? "" : " · " + out);
    }
    case FunEvent::damage: {
      const i32 broken = combo;
      run_damage += std::fmax(0.0f, amount);
      combo = 0;
      combo_expires = -1e9f;
      momentum = std::max(0, momentum - 1);
      return broken > 1 ? "COMBO WEG" : "";
    }
    case FunEvent::drone_destroyed: {
      const i32 lost = rounded(static_cast<f32>(at_risk) * 0.2f);
      at_risk = std::max(0, at_risk - lost);
      combo = momentum = 0;
      combo_expires = -1e9f;
      mercy = std::min(2, mercy + 1);
      last_label = "DRONE LOST";
      return "DROHNE VERLOREN -" + std::to_string(lost);
    }
    case FunEvent::player_death: {
      const i32 insured = rounded(static_cast<f32>(at_risk) * 0.5f);
      score += insured;
      at_risk = 0;
      high_score = std::max(high_score, score);
      combo = momentum = 0;
      combo_expires = -1e9f;
      mercy = 2;
      last_label = "BUSTED";
      return "BUSTED · versichert +" + std::to_string(insured);
    }
    case FunEvent::cycle_contract:
      contract_index = (contract_index + 1) % kContracts;
      contract_progress = 0;
      contract_complete = false;
      return std::string("AUFTRAG: ") + contract(contract_index).name;
    case FunEvent::tick: return "";
  }
  return "";
}

// ---------------------------------------------------------------- race_track.mjs
namespace {
constexpr f32 S = kCourseScale;
const Gate kGateList[kGates] = {
    {"START / FINISH", {-2.0f * S, 0.0f * S, 1.15f}, 1.90f, 1.55f, true},
    {"GATE 2", {-5.0f * S, -1.7f * S, 1.55f}, 1.75f, 1.45f, false},
    {"GATE 3", {-7.0f * S, 1.25f * S, 1.05f}, 1.65f, 1.35f, false},
    {"HIGH GATE", {-4.2f * S, 4.45f * S, 1.85f}, 1.70f, 1.45f, false},
    {"GATE 5", {0.0f * S, 5.55f * S, 1.20f}, 1.75f, 1.40f, false},
    {"GATE 6", {4.0f * S, 3.10f * S, 1.65f}, 1.65f, 1.35f, false},
    {"LOW GATE", {4.55f * S, -1.10f * S, 1.00f}, 1.60f, 1.25f, false},
    {"GATE 8", {1.55f * S, -3.85f * S, 1.45f}, 1.70f, 1.40f, false},
};
}  // namespace

const Gate& gate(i32 i) { return kGateList[((i % kGates) + kGates) % kGates]; }

void gate_axes(i32 i, Vec2& normal, Vec2& right) {
  const Vec3 a = gate(i).center, b = gate(i + 1).center;
  Vec2 n{b.x - a.x, b.y - a.y};
  const f32 l = std::fmax(1e-6f, std::hypot(n.x, n.y));
  normal = n * (1.0f / l);
  right = {-normal.y, normal.x};
}

void Race::reset() { *this = Race{}; }

bool Race::crossed(i32 gi, Vec3 a, Vec3 b) {
  const Gate& g = gate(gi);
  Vec2 n, r;
  gate_axes(gi, n, r);
  const f32 da = (a.x - g.center.x) * n.x + (a.y - g.center.y) * n.y, db = (b.x - g.center.x) * n.x + (b.y - g.center.y) * n.y;
  if (!(da < 0 && db >= 0)) return false;
  const f32 den = da - db;
  if (std::fabs(den) < 1e-8f) return false;
  const f32 t = da / den;
  const Vec3 hit = a + (b - a) * t;
  const f32 u = (hit.x - g.center.x) * r.x + (hit.y - g.center.y) * r.y, v = hit.z - g.center.z;
  return std::fabs(u) <= g.width * 0.5f && std::fabs(v) <= g.height * 0.5f;
}

i32 Race::update(Vec3 p, f32 t) {
  if (!have_prev || finished || t - last_pass < 0.2f || !crossed(next, prev, p)) { prev = p; have_prev = true; return -1; }
  last_pass = t;
  const i32 passed = next;
  if (!started) { started = true; lap = 1; race_start = lap_start = t; next = 1; }
  else if (passed == 0) {
    last_lap = t - lap_start;
    if (best_lap < 0 || last_lap < best_lap) best_lap = last_lap;
    if (lap >= kRaceLaps) { finished = true; finish_time = t - race_start; next = 0; }
    else { ++lap; lap_start = t; next = 1; }
  } else next = passed == kGates - 1 ? 0 : passed + 1;
  prev = p;
  return passed;
}

std::string race_time(f32 s) {
  if (!(s >= 0)) return "-";
  const int m = static_cast<int>(s / 60);
  char b[32];
  std::snprintf(b, sizeof b, "%02d:%06.3f", m, s - static_cast<f32>(m) * 60);
  return b;
}

}  // namespace sc::combat
