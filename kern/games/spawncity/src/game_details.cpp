// SpawnCity on Kern: combat details on top of the town and the weapons (combat_detail.h):
//  * critical damage (critical_damage_logic.mjs): a bus burns 3.1 s instead of a car's 2.45 s, a
//    burning vehicle hit again goes up sooner (0.62 / 0.72 s), a police drone at 34 % smokes and
//    blows after 2.2 s (0.56 s when hit again), the player's drone at 28 % after 2.05 s,
//  * bullet momentum (ballistics.mjs): a round that stops in a Box3D body hands it p = m * v at
//    the hit point - 9 mm 2.9 N*s, 5.56 mm 3.8 N*s, .50 BMG 41 N*s; no knockback multipliers,
//  * the rule ballot (rules_vote_engine.mjs) run locally by the `ballot` command; carrying the
//    votes between players is the network's.
#include <cmath>
#include <cstdio>
#include <sstream>

#include "game.h"

namespace sc {

namespace {
constexpr f32 kCarHp = 300, kPdHp = 100, kDroneHp = 100;  // police_ground_units.mjs, wanted_police_drones.mjs, player_vitals_logic.mjs
bool is_bus(const TrafficCar& c) { return c.car->spec().radius > 0.4f; }
}  // namespace

void Game::update_details(f32 dt) {
  (void)dt;
  if (!world_placed_) return;
  const f32 now = world_->sim_time();
  // ---- burning cars and buses
  if (details_.car_last_hp.size() != cars_.size()) { details_.car_last_hp.assign(cars_.size(), kCarHp); details_.car_burn_seen.assign(cars_.size(), -1); }
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (!c.placed || c.wreck) { details_.car_last_hp[i] = c.hp; details_.car_burn_seen[i] = -1; continue; }
    const detail::Critical kind = is_bus(c) ? detail::Critical::bus : detail::Critical::car;
    if (c.burn_until >= 0) {
      if (details_.car_burn_seen[i] < 0) {
        // It just turned critical (game_town.cpp starts the car's 2.45 s): a bus takes longer.
        details_.car_burn_seen[i] = now;
        if (kind == detail::Critical::bus) c.burn_until = std::fmax(c.burn_until, detail::detonation_at(kind, now));
      } else if (c.hp < details_.car_last_hp[i] - 1e-3f) {
        c.burn_until = detail::accelerate(kind, c.burn_until, now);  // hit again while burning
      }
    } else details_.car_burn_seen[i] = -1;
    details_.car_last_hp[i] = c.hp;
  }
  // ---- police drones: smoking at 34 %, then they come down in a blast
  for (i32 i = 0; i < 5; ++i) {
    PoliceDrone& d = police_.pdrones[i];
    if (!d.active) { details_.pd_crit[i].reset(); continue; }
    if (details_.pd_crit[i].update(detail::Critical::police_drone, d.hp, kPdHp, now)) {
      explosion(d.pos, 3, 40, true);
      wanted::report(wanted::Crime::police_drone, true);
      d.active = false;
      d.destroyed_until = now + 9;
      ++details_.crit_blasts;
    }
  }
  // ---- the player's drone: 28 %, 2.05 s
  if (mode_ == Mode::drone && vehicle_.drone_out) {
    const bool was = details_.drone_crit.armed();
    if (details_.drone_crit.update(detail::Critical::drone, vehicle_.drone_hp, kDroneHp, now)) {
      ++details_.crit_blasts;
      damage_player_vehicle(vehicle_.drone_hp + 1);
    } else if (!was && details_.drone_crit.armed()) {
      toast_ = "DROHNE KRITISCH";
      toast_age_ = 0;
    }
  } else details_.drone_crit.reset();
  // ---- bullet momentum into the bodies the rounds stopped in
  for (const kern::game::ShotEvent& s : world_->shots()) {
    if (!s.impact || s.surface != 2) continue;
    const Vec3 d = s.to - s.from;
    const f32 l = kern::length(d);
    if (l < 0.05f) continue;
    const Vec3 dir = d * (1.0f / l);
    const kern::physics::RayHit h = world_->physics().cast_ray(s.from, dir * (l + 0.2f), ~0ull, {});
    if (!h.hit || !h.body.valid()) continue;
    detail::Round round = detail::Round::nine_mm;
    if (s.shooter == soldier_ && weapons_.foot_weapon == weapons::Id::sniper) round = detail::Round::fifty_bmg;
    else if (s.shooter != soldier_ && mode_ == Mode::drone) round = detail::Round::five_five_six;
    world_->physics().apply_linear_impulse(h.body, detail::bullet_impulse(dir, round), h.point);
    ++details_.bullet_pushes;
  }
}

void Game::details_hud(std::string& s) {
  i32 smoking = 0;
  for (const detail::CriticalClock& c : details_.pd_crit) smoking += c.armed() ? 1 : 0;
  char b[200];
  std::snprintf(b, sizeof b, ",\"details\":{\"droneCritical\":%s,\"pdSmoking\":%d,\"critBlasts\":%u,\"bulletPushes\":%u,\"ballot\":\"%s\"}",
                details_.drone_crit.armed() ? "true" : "false", smoking, details_.crit_blasts, details_.bullet_pushes, mp::ballot_name(details_.ballot.status).c_str());
  s += b;
}

bool Game::details_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (cmd == "guided") {
    // Tests: one press of the GUIDED weapon (the drone must be out).
    if (mode_ != Mode::drone) { out = "not drone"; return true; }
    weapons_.drone_weapon = 2;
    const bool ok = guided_press();
    out = ok ? std::string("launched ") + guided_.lock : guided_.marked ? std::string("marked ") + guided_.mark.label : "denied";
    return true;
  }
  if (starts("hurt ")) {
    // Tests: damage the vehicle the player is in (drone: critical below 28).
    damage_player_vehicle(static_cast<f32>(std::atof(cmd.c_str() + 5)));
    char b[64];
    std::snprintf(b, sizeof b, "drone %.0f car %.0f", vehicle_.drone_hp, mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].hp : -1.0f);
    out = b;
    return true;
  }
  if (starts("ballot")) {
    // ballot new KEY 0|1 HOST M1 M2 ... | ballot vote PEER 0|1 M1 M2 ... | ballot
    std::istringstream in(cmd.substr(6));
    std::string op;
    in >> op;
    // (a ballot of its own, beside a session's - the same rules: mp_rules.h; KEY is only its label)
    std::vector<u8> members;
    const f64 now_ms = static_cast<f64>(world_->sim_time()) * 1000;
    if (op == "new") {
      std::string key;
      int value = 0;
      u32 host = 0, m = 0;
      in >> key >> value >> host;
      while (in >> m) members.push_back(static_cast<u8>(m));
      mp::create_ballot(details_.ballot, key.empty() ? 0 : ++details_.ballot_serial, 0, value != 0, static_cast<u8>(host), members, now_ms);
    } else if (op == "vote") {
      u32 peer = 0, m = 0;
      int yes = 0;
      in >> peer >> yes;
      while (in >> m) members.push_back(static_cast<u8>(m));
      mp::Ballot::Status did = details_.ballot.status;
      mp::cast_vote(details_.ballot, static_cast<u8>(peer), yes != 0, members, now_ms, &did);
      out = mp::ballot_name(did);
      return true;
    }
    out = mp::ballot_name(details_.ballot.status);
    return true;
  }
  return false;
}

}  // namespace sc

namespace sc {

namespace {
// "sc:<kind><variant>" for every sound of the bank (built once).
const std::string& sfx_id(combat_audio::Kind k, u32 v) {
  static std::vector<std::string> ids;
  if (ids.empty())
    for (u8 i = 0; i < static_cast<u8>(combat_audio::Kind::count); ++i)
      for (u32 n = 0; n < 4; ++n) ids.push_back(std::string("sc:") + combat_audio::kind_name(static_cast<combat_audio::Kind>(i)) + std::to_string(n));
  return ids[static_cast<std::size_t>(k) * 4 + v];
}
}  // namespace

void Game::combat_audio_init() {
  if (details_.combat_audio_ready || !audio_.bank) return;
  // Rendered once (combat_audio_bank.mjs prebakes its bank at load too), then handed to Kern's
  // SoundBank as decoded PCM, so play_sound("sc:pistol2", ...) works like any imported sound.
  for (u8 i = 0; i < static_cast<u8>(combat_audio::Kind::count); ++i) {
    const auto k = static_cast<combat_audio::Kind>(i);
    for (u32 v = 0; v < combat_audio::kVariants[i]; ++v) {
      const std::vector<f32> pcm = combat_audio::render(k, v);
      const std::string& id = sfx_id(k, v);
      audio_.bank->request(id);
      audio_.bank->set_pcm(id, pcm.data(), static_cast<u32>(pcm.size()), 1, combat_audio::kSampleRate);
    }
  }
  details_.combat_audio_ready = true;
}

const char* Game::combat_sfx(combat_audio::Kind k) {
  // playCombatAudio: the variants take turns.
  const u8 i = static_cast<u8>(k);
  const u32 v = details_.sfx_cursor[i]++ % combat_audio::kVariants[i];
  return sfx_id(k, v).c_str();
}

}  // namespace sc
