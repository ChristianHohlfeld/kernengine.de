// The mates' units in our world (zombie_nights.mjs "zombie-sync" / "zombie-hit", military_response.mjs
// "mil-snap" / "mil-hit"): every player simulates his own zombies, helicopters, tanks and soldiers
// and sends them every 120 ms (SYNC_MS); the others draw them where they are and shoot them - a hit
// goes to the owner, who applies it to the real unit (and kills it). A mate silent for 2.5 s takes
// his units with him.
#include <algorithm>
#include <cmath>

#include "combat_rules.h"
#include "game.h"

namespace sc {

namespace {
constexpr f64 kSyncMs = 120, kSilentMs = 2500;
constexpr f32 kZombieHit = 34;  // zombie_nights.mjs HIT_DMG
// the zombies' and soldiers' colours (game_events.cpp / game_military.cpp)
const Vec3 kZSkin[4] = {{0.54f, 0.6f, 0.47f}, {0.49f, 0.55f, 0.44f}, {0.58f, 0.63f, 0.51f}, {0.44f, 0.49f, 0.4f}};
const Vec3 kZShirt[4] = {{0.36f, 0.35f, 0.29f}, {0.29f, 0.3f, 0.26f}, {0.42f, 0.36f, 0.29f}, {0.25f, 0.29f, 0.31f}};
const Vec3 kZPants[4] = {{0.18f, 0.17f, 0.16f}, {0.23f, 0.21f, 0.19f}, {0.15f, 0.16f, 0.18f}, {0.25f, 0.22f, 0.18f}};
const Vec3 kSoldierShirt{0.33f, 0.38f, 0.24f}, kSoldierPants{0.29f, 0.32f, 0.22f};
const Vec3 kSoldierSkins[4] = {{0.78f, 0.58f, 0.44f}, {0.66f, 0.44f, 0.32f}, {0.48f, 0.29f, 0.2f}, {0.82f, 0.64f, 0.49f}};
constexpr f32 kRemoteHp = 1e6f;  // a mate's unit never dies here: its owner decides

}  // namespace

void Game::swarm_clear() {
  for (auto& [o, sw] : swarms_)
    for (RemoteUnit& u : sw.units)
      if (u.person >= 0) people_.at(u.person).state = People::State::gone;
  swarms_.clear();
}

void Game::swarm_update(f32 dt) {
  if (!net_.active() || !world_placed_) { if (!swarms_.empty()) swarm_clear(); return; }
  const f64 now = time_ * 1000;
  const f32 st = world_->sim_time();
  CombatState& C = combat_;
  auto next_id = [&]() { if (++swarm_serial_ == 0) ++swarm_serial_; return swarm_serial_; };
  // ---- ours out, every 120 ms (only with mates there)
  if (now - swarm_clock_ >= kSyncMs && !net_.remotes().empty()) {
    swarm_clock_ = now;
    std::vector<net::SwarmUnit> out;
    for (ZombieAi& z : C.zombie_ai) {
      if (z.person < 0) continue;
      if (!z.id) z.id = next_id();
      const People::Person& p = people_.at(z.person);
      out.push_back({z.id, net::SwarmKind::zombie, static_cast<u8>(st - z.swing < 0.6f ? 1 : 0), p.pos, wrap_pi(p.yaw), p.v});
    }
    for (Heli& h : C.helis) {
      if (h.dead) continue;
      if (!h.id) h.id = next_id();
      out.push_back({h.id, net::SwarmKind::heli, static_cast<u8>(h.state == Heli::State::down ? 1 : 0), h.p, wrap_pi(h.yaw), 0});
    }
    for (Tank& k : C.tanks) {
      if (k.dead) continue;
      if (!k.id) k.id = next_id();
      out.push_back({k.id, net::SwarmKind::tank, static_cast<u8>(k.wreck ? 1 : 0), k.p, wrap_pi(k.yaw), wrap_pi(k.turret)});
    }
    for (SoldierAi& s : C.soldiers) {
      if (s.person < 0) continue;
      if (!s.id) s.id = next_id();
      const People::Person& p = people_.at(s.person);
      if (p.state == People::State::down || p.state == People::State::gone) continue;
      const u8 anim = s.los || st - s.last_shot < 0.5f ? 1 : p.v > 3 ? 2 : p.v > 0 ? 3 : 0;
      out.push_back({s.id, net::SwarmKind::soldier, anim, p.pos, wrap_pi(p.yaw), p.v});
    }
    if (out.size() > 64) out.resize(64);
    net_.send_swarm(out);
  }
  // ---- theirs in
  for (net::Swarm& sw : net_.take_swarms()) {
    RemoteSwarm& r = swarms_[sw.owner];
    r.seen = now;
    std::vector<RemoteUnit> next;
    for (const net::SwarmUnit& u : sw.units) {
      auto it = std::find_if(r.units.begin(), r.units.end(), [&](const RemoteUnit& q) { return q.id == u.id && q.kind == u.kind; });
      RemoteUnit q;
      if (it != r.units.end()) { q = *it; it->id = 0; it->person = -1; }  // taken over
      else {
        q.id = u.id;
        q.kind = u.kind;
        q.p = u.p;
        q.yaw = u.yaw;
        const u32 seed = (static_cast<u32>(sw.owner) << 16) | u.id;
        if (u.kind == net::SwarmKind::zombie)
          q.person = people_.add_scripted(People::Kind::zombie, u.p, u.yaw, kRemoteHp, seed, kZShirt[seed % 4], kZPants[seed % 4], kZSkin[seed % 4], 0);
        else if (u.kind == net::SwarmKind::soldier)
          q.person = people_.add_scripted(People::Kind::soldier, u.p, u.yaw, kRemoteHp, seed, kSoldierShirt, kSoldierPants, kSoldierSkins[seed % 4], static_cast<u8>(seed % 4));
      }
      q.tp = u.p;
      q.tyaw = u.yaw;
      q.aux = u.aux;
      q.flags = u.flags;
      next.push_back(q);
    }
    // the ones its owner no longer has are gone (killed, retired)
    for (RemoteUnit& gone : r.units)
      if (gone.person >= 0) people_.at(gone.person).state = People::State::gone;
    r.units = std::move(next);
  }
  // ---- hits on ours from the mates
  for (const net::SwarmHit& h : net_.take_swarm_hits()) {
    switch (h.kind) {
      case net::SwarmKind::zombie:
        for (ZombieAi& z : C.zombie_ai)
          if (z.id == h.id && z.person >= 0) {
            people_.hurt(z.person, h.damage, {}, false, st);
            z.last_hp = people_.at(z.person).hp;  // the mate earns the points, not we
          }
        break;
      case net::SwarmKind::soldier:
        for (SoldierAi& s : C.soldiers)
          if (s.id == h.id && s.person >= 0) people_.hurt(s.person, h.damage, {}, false, st);
        break;
      case net::SwarmKind::heli:
        for (Heli& x : C.helis)
          if (x.id == h.id && !x.dead && x.state != Heli::State::down) {
            x.hp -= h.damage;
            if (x.hp <= 0) { x.state = Heli::State::down; x.v.z = std::fmin(x.v.z, 2.0f); world_->play_sound(combat_sfx(combat_audio::Kind::explosion), x.p, 0.8f, 0.8f, true); }
          }
        break;
      case net::SwarmKind::tank:
        for (Tank& k : C.tanks)
          if (k.id == h.id && !k.dead && !k.wreck) {
            k.hp -= h.damage;
            if (k.hp <= 0) { k.wreck = true; k.wreck_at = st; explosion(k.p + Vec3{0, 0, 1.5f}, 12, 100, false); }
          }
        break;
    }
  }
  // ---- drawn where they are (eased in, as renderRemote: 1 - exp(-10 dt)); silent mates leave
  const f32 a = 1 - std::exp(-dt * 10);
  for (auto it = swarms_.begin(); it != swarms_.end();) {
    if (now - it->second.seen > kSilentMs) {
      for (RemoteUnit& u : it->second.units) if (u.person >= 0) people_.at(u.person).state = People::State::gone;
      it = swarms_.erase(it);
      continue;
    }
    for (RemoteUnit& u : it->second.units) {
      u.p = u.p + (u.tp - u.p) * a;
      u.yaw += wrap_pi(u.tyaw - u.yaw) * a;
      u.rotor += dt * 28;
      if (u.person < 0) continue;
      People::Person& p = people_.at(u.person);
      if (p.state == People::State::gone) { u.person = -1; continue; }
      // on our ground, not his: his height field can differ (refined later, a crater) - no zombie
      // floats above the street or wades in it
      p.pos = {u.p.x, u.p.y, city_.ground_height(u.p.x, u.p.y)};
      p.yaw = u.yaw;
      p.v = u.kind == net::SwarmKind::zombie ? std::fmax(0.4f, u.aux) * (kern::length(u.tp - u.p) > 0.05f ? 1.0f : 0.0f) : u.aux;
      p.phase += p.v * dt * 3.4f;
      p.aiming = u.kind == net::SwarmKind::zombie || u.flags == 1;
      p.hp = kRemoteHp;
    }
    ++it;
  }
}

bool Game::swarm_person_hit(i32 person, f32 damage) {
  for (auto& [owner, sw] : swarms_)
    for (RemoteUnit& u : sw.units) {
      if (u.person != person) continue;
      const f32 dmg = u.kind == net::SwarmKind::zombie ? kZombieHit : combat::kMilHit;
      net_.send_swarm_hit(owner, u.id, u.kind, dmg);
      if (u.kind == net::SwarmKind::zombie) combat_.zombies.points += combat::kZombieHitPts;  // addPoints(HIT_PTS) for the shooter
      people_.at(person).hp = kRemoteHp;
      (void)damage;
      return true;
    }
  return false;
}

bool Game::swarm_shot(kern::game::ShotEvent& s) {
  const Vec3 seg = s.to - s.from;
  const f32 l2 = kern::dot(seg, seg);
  if (l2 < 1e-6f || swarms_.empty()) return false;
  for (auto& [owner, sw] : swarms_)
    for (RemoteUnit& u : sw.units) {
      if (u.kind != net::SwarmKind::heli && u.kind != net::SwarmKind::tank) continue;
      if (u.flags) continue;  // already going down / a wreck
      const Vec3 c = u.kind == net::SwarmKind::heli ? u.p : u.p + Vec3{0, 0, 1.2f};
      const f32 r = u.kind == net::SwarmKind::heli ? 2.2f : 2.6f;
      const f32 t = clampf(kern::dot(c - s.from, seg) / l2, 0, 1);
      if (kern::length(s.from + seg * t - c) >= r) continue;
      s.to = s.from + seg * t;
      s.surface = 2;
      net_.send_swarm_hit(owner, u.id, u.kind, u.kind == net::SwarmKind::heli ? combat::kMilHit : 4);  // a tank's armour: 4 a bullet
      return true;
    }
  return false;
}

void Game::swarm_draw() {
  CombatArt& A = combat_art_;
  for (auto& [owner, sw] : swarms_)
    for (const RemoteUnit& u : sw.units) {
      if (u.kind == net::SwarmKind::heli) A.heli(kern::Mat4::translate(u.p) * kern::Mat4::trs({}, yaw_quat(u.yaw), {1, 1, 1}), u.rotor, u.flags != 0);
      else if (u.kind == net::SwarmKind::tank) A.tank(kern::Mat4::translate({u.p.x, u.p.y, city_.ground_height(u.p.x, u.p.y)}) * kern::Mat4::trs({}, yaw_quat(u.yaw), {1, 1, 1}), u.aux, u.flags != 0);
    }
}

}  // namespace sc
