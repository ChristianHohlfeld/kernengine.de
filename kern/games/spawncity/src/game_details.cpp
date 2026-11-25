// SpawnCity on Kern: combat details on top of the town and the weapons (combat_detail.h):
//  * critical damage (critical_damage_logic.mjs): a bus burns 3.1 s instead of a car's 2.45 s, a
//    burning vehicle hit again goes up sooner (0.62 / 0.72 s), a police drone at 34 % smokes and
//    blows after 2.2 s (0.56 s when hit again), the player's drone at 28 % after 2.05 s,
//  * bullet momentum (ballistics.mjs): a round that stops in a Box3D body hands it p = m * v at
//    the hit point - 9 mm 2.9 N*s, 5.56 mm 3.8 N*s, .50 BMG 41 N*s; no knockback multipliers,
//  * the rule ballot (rules_vote_engine.mjs) run locally by the `ballot` command; carrying the
//    votes between players is the network's.
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "game.h"

namespace sc {

namespace {
constexpr f32 kCarHp = 300, kPdHp = 100, kDroneHp = 100;  // police_ground_units.mjs, wanted_police_drones.mjs, player_vitals_logic.mjs
bool is_bus(const TrafficCar& c) { return c.car->spec().radius > 0.4f; }

// The words of a command from `at` on, as std::istream's >> reads them (libstdc++, the C locale): spaces
// are skipped; a word goes up to the next space; a whole number is decimal, with a sign - none there: 0,
// beyond its type: the type's end - and once one read has failed (`good`), none after it reads anything
// or changes its value (also where only spaces are left).
constexpr const char* kSpaces = " \t\n\v\f\r";
bool read_word(const std::string& s, std::size_t& at, bool& good, std::string& w) {
  if (!good) return false;
  const std::size_t from = s.find_first_not_of(kSpaces, at);
  if (from == std::string::npos) {
    good = false;
    return false;
  }
  const std::size_t to = s.find_first_of(kSpaces, from);
  w = s.substr(from, to == std::string::npos ? std::string::npos : to - from);
  at = to == std::string::npos ? s.size() : to;
  return true;
}
// (where the next thing to read begins - npos: only spaces are left)
std::size_t next_at(const std::string& s, std::size_t at) { return s.find_first_not_of(kSpaces, at); }
// (the number that begins at `from`: strtoll reads the characters >> reads there - a sign, digits - and
// where they end; false: there is none)
bool number_at(const std::string& s, std::size_t from, std::size_t& at, long long& x) {
  char* end = nullptr;
  x = std::strtoll(s.c_str() + from, &end, 10);
  if (end == s.c_str() + from) return false;
  at = static_cast<std::size_t>(end - s.c_str());
  return true;
}
bool read_int(const std::string& s, std::size_t& at, bool& good, int& v) {
  if (!good) return false;
  const std::size_t from = next_at(s, at);
  long long x = 0;
  if (from == std::string::npos) {
    good = false;  // (nothing left: v stays)
  } else if (!number_at(s, from, at, x)) {
    v = 0;
    good = false;
  } else if (x > INT_MAX || x < INT_MIN) {
    v = x > INT_MAX ? INT_MAX : INT_MIN;
    good = false;
  } else {
    v = static_cast<int>(x);
  }
  return good;
}
bool read_u32(const std::string& s, std::size_t& at, bool& good, u32& v) {
  if (!good) return false;
  const std::size_t from = next_at(s, at);
  long long x = 0;
  if (from == std::string::npos) {
    good = false;
    return false;
  }
  if (!number_at(s, from, at, x)) {
    v = 0;
    good = false;
    return false;
  }
  // (unsigned: a - in front gives the two's complement of the number; beyond 2^32 - 1 it is the end)
  const bool minus = x < 0;
  const unsigned long long m = minus ? 0ull - static_cast<unsigned long long>(x) : static_cast<unsigned long long>(x);
  if (m > UINT_MAX) {
    v = UINT_MAX;
    good = false;
    return false;
  }
  v = minus ? 0u - static_cast<u32>(m) : static_cast<u32>(m);
  return true;
}
}  // namespace

void Game::update_details(f32 dt) {
  (void)dt;
  if (!world_placed_) return;
  const f32 now = world_->sim_time();
  // ---- burning cars and buses
  if (car_last_hp_.size() != cars_.size()) { car_last_hp_.assign(cars_.size(), kCarHp); car_burn_seen_.assign(cars_.size(), -1); }
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (!c.placed || c.wreck) { car_last_hp_[i] = c.hp; car_burn_seen_[i] = -1; continue; }
    const detail::Critical kind = is_bus(c) ? detail::Critical::bus : detail::Critical::car;
    if (c.burn_until >= 0) {
      if (car_burn_seen_[i] < 0) {
        // It just turned critical (game_town.cpp starts the car's 2.45 s): a bus takes longer.
        car_burn_seen_[i] = now;
        if (kind == detail::Critical::bus) c.burn_until = std::fmax(c.burn_until, detail::detonation_at(kind, now));
      } else if (c.hp < car_last_hp_[i] - 1e-3f) {
        c.burn_until = detail::accelerate(kind, c.burn_until, now);  // hit again while burning
      }
    } else car_burn_seen_[i] = -1;
    car_last_hp_[i] = c.hp;
  }
  // ---- police drones: smoking at 34 %, then they come down in a blast
  for (i32 i = 0; i < 5; ++i) {
    PoliceDrone& d = pdrones_[i];
    if (!d.active) { pd_crit_[i].reset(); continue; }
    if (pd_crit_[i].update(detail::Critical::police_drone, d.hp, kPdHp, now)) {
      explosion(d.pos, 3, 40, true);
      wanted::report(wanted::Crime::police_drone, true);
      d.active = false;
      d.destroyed_until = now + 9;
      ++crit_blasts_;
    }
  }
  // ---- the player's drone: 28 %, 2.05 s
  if (mode_ == Mode::drone && drone_out_) {
    const bool was = drone_crit_.armed();
    if (drone_crit_.update(detail::Critical::drone, drone_hp_, kDroneHp, now)) {
      ++crit_blasts_;
      damage_player_vehicle(drone_hp_ + 1);
    } else if (!was && drone_crit_.armed()) {
      toast_ = "DROHNE KRITISCH";
      toast_age_ = 0;
    }
  } else drone_crit_.reset();
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
    if (s.shooter == soldier_ && foot_weapon_ == weapons::Id::sniper) round = detail::Round::fifty_bmg;
    else if (s.shooter != soldier_ && mode_ == Mode::drone) round = detail::Round::five_five_six;
    world_->physics().apply_linear_impulse(h.body, detail::bullet_impulse(dir, round), h.point);
    ++bullet_pushes_;
  }
}

void Game::details_hud(std::string& s) {
  i32 smoking = 0;
  for (const detail::CriticalClock& c : pd_crit_) smoking += c.armed() ? 1 : 0;
  char b[200];
  std::snprintf(b, sizeof b, ",\"details\":{\"droneCritical\":%s,\"pdSmoking\":%d,\"critBlasts\":%u,\"bulletPushes\":%u,\"ballot\":\"%s\"}",
                drone_crit_.armed() ? "true" : "false", smoking, crit_blasts_, bullet_pushes_, mp::ballot_name(ballot_.status).c_str());
  s += b;
}

bool Game::details_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (cmd == "guided") {
    // Tests: one press of the GUIDED weapon (the drone must be out).
    if (mode_ != Mode::drone) { out = "not drone"; return true; }
    drone_weapon_ = 2;
    const bool ok = guided_press();
    out = ok ? std::string("launched ") + guided_lock_ : guided_marked_ ? std::string("marked ") + guided_mark_.label : "denied";
    return true;
  }
  if (starts("hurt ")) {
    // Tests: damage the vehicle the player is in (drone: critical below 28).
    damage_player_vehicle(static_cast<f32>(std::atof(cmd.c_str() + 5)));
    char b[64];
    std::snprintf(b, sizeof b, "drone %.0f car %.0f", drone_hp_, mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].hp : -1.0f);
    out = b;
    return true;
  }
  if (starts("ballot")) {
    // ballot new KEY 0|1 HOST M1 M2 ... | ballot vote PEER 0|1 M1 M2 ... | ballot
    std::size_t at = 6;  // (the words after "ballot", read as std::istream reads them)
    bool good = true;
    std::string op;
    read_word(cmd, at, good, op);
    // (a ballot of its own, beside a session's - the same rules: mp_rules.h; KEY is only its label)
    std::vector<u8> members;
    const f64 now_ms = static_cast<f64>(world_->sim_time()) * 1000;
    if (op == "new") {
      std::string key;
      int value = 0;
      u32 host = 0, m = 0;
      read_word(cmd, at, good, key);
      read_int(cmd, at, good, value);
      read_u32(cmd, at, good, host);
      while (read_u32(cmd, at, good, m)) members.push_back(static_cast<u8>(m));
      mp::create_ballot(ballot_, key.empty() ? 0 : ++ballot_serial_, 0, value != 0, static_cast<u8>(host), members, now_ms);
    } else if (op == "vote") {
      u32 peer = 0, m = 0;
      int yes = 0;
      read_u32(cmd, at, good, peer);
      read_int(cmd, at, good, yes);
      while (read_u32(cmd, at, good, m)) members.push_back(static_cast<u8>(m));
      mp::Ballot::Status did = ballot_.status;
      mp::cast_vote(ballot_, static_cast<u8>(peer), yes != 0, members, now_ms, did);
      out = mp::ballot_name(did);
      return true;
    }
    out = mp::ballot_name(ballot_.status);
    return true;
  }
  return false;
}

}  // namespace sc

namespace sc {

namespace {
// "sc:<kind><variant>" for every sound of the bank (built once).
const std::string& sfx_id(blueprints::combat_audio::combat_audio_Kind k, u32 v) {
  static std::vector<std::string> ids;
  if (ids.empty())
    for (u8 i = 0; i < static_cast<u8>(blueprints::combat_audio::combat_audio_Kind::count); ++i)
      for (u32 n = 0; n < 4; ++n) ids.push_back(std::string("sc:") + std::string(kern::logic::blueprint<blueprints::combat_audio_cpp>().kind_name(static_cast<blueprints::combat_audio::combat_audio_Kind>(i)).Result.view()) + std::to_string(n));
  return ids[static_cast<std::size_t>(k) * 4 + v];
}
}  // namespace

void Game::combat_audio_init() {
  if (combat_audio_ready_ || !bank_) return;
  // Rendered once (combat_audio_bank.mjs prebakes its bank at load too), then handed to Kern's
  // SoundBank as decoded PCM, so play_sound("sc:pistol2", ...) works like any imported sound.
  for (u8 i = 0; i < static_cast<u8>(blueprints::combat_audio::combat_audio_Kind::count); ++i) {
    const auto k = static_cast<blueprints::combat_audio::combat_audio_Kind>(i);
    for (u32 v = 0; v < blueprints::combat_audio_cpp::kVariants[i]; ++v) {
      // (the Blueprint's samples, 64 bit, made once into the 32-bit PCM the engine's bank takes - its boundary)
      std::vector<f32> pcm;
      for (const f64 x : kern::logic::blueprint<blueprints::combat_audio_cpp>().render(k, v, blueprints::combat_audio_cpp::kSampleRate).Result) pcm.push_back(static_cast<f32>(x));
      const std::string& id = sfx_id(k, v);
      bank_->request(id);
      bank_->set_pcm(id, pcm.data(), static_cast<u32>(pcm.size()), 1, blueprints::combat_audio_cpp::kSampleRate);
    }
  }
  combat_audio_ready_ = true;
}

const char* Game::combat_sfx(blueprints::combat_audio::combat_audio_Kind k) {
  // playCombatAudio: the variants take turns.
  const u8 i = static_cast<u8>(k);
  const u32 v = sfx_cursor_[i]++ % blueprints::combat_audio_cpp::kVariants[i];
  return sfx_id(k, v).c_str();
}

}  // namespace sc
