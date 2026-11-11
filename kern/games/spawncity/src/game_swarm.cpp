// The mates' units in our world (zombie_nights.mjs "zombie-sync" / "zombie-hit", military_response.mjs
// "mil-snap" / "mil-hit"): every player simulates what hunts him - his zombies, the army's
// helicopters, tanks and soldiers, his police (officers on foot, cruisers, drones) and the sandbox
// enemies (COP / SWAT / HEAVY) - and sends them every 120 ms (SYNC_MS), all of them (more than one
// message carries go in numbered parts: net_session.h send_swarm). The others draw them where he
// has them and shoot them: a hit goes to the owner, who applies it to the real unit by the rule his
// own hits follow (and kills it). They chase their owner alone - here they are pictures that can be
// hit. A blast needs no message of its own: every machine runs a mate's blast (Fx::explosion) on its
// own units. A mate silent for 2.5 s takes his units with him.
#include <algorithm>
#include <cmath>

#include "combat_rules.h"
#include "game.h"

namespace sc {

namespace {
constexpr f64 kSyncMs = 120, kSilentMs = 2500, kFallenMs = 1500;
constexpr f32 kZombieHit = 34;  // zombie_nights.mjs HIT_DMG
constexpr f32 kPoliceDroneHit = 34, kCruiserHit = 22;  // wanted_police_drones.mjs POLICE_HIT_DAMAGE, police_ground_units.mjs CAR_HIT
// the zombies' and soldiers' colours (game_events.cpp / game_military.cpp)
const Vec3 kZSkin[4] = {{0.54f, 0.6f, 0.47f}, {0.49f, 0.55f, 0.44f}, {0.58f, 0.63f, 0.51f}, {0.44f, 0.49f, 0.4f}};
const Vec3 kZShirt[4] = {{0.36f, 0.35f, 0.29f}, {0.29f, 0.3f, 0.26f}, {0.42f, 0.36f, 0.29f}, {0.25f, 0.29f, 0.31f}};
const Vec3 kZPants[4] = {{0.18f, 0.17f, 0.16f}, {0.23f, 0.21f, 0.19f}, {0.15f, 0.16f, 0.18f}, {0.25f, 0.22f, 0.18f}};
const Vec3 kSoldierShirt{0.33f, 0.38f, 0.24f}, kSoldierPants{0.29f, 0.32f, 0.22f};
const Vec3 kSoldierSkins[4] = {{0.78f, 0.58f, 0.44f}, {0.66f, 0.44f, 0.32f}, {0.48f, 0.29f, 0.2f}, {0.82f, 0.64f, 0.49f}};
const Vec3 kEnemyPants{0.08f, 0.07f, 0.12f}, kEnemySkin{0.88f, 0.7f, 0.58f};  // game_events.cpp update_enemies
const Vec3 kCruiserWhite{0.93f, 0.94f, 0.96f}, kWreckBlack{0.055f, 0.05f, 0.045f};  // game_net.cpp / game.cpp draw_vehicles
constexpr f32 kRemoteHp = 1e6f;  // a mate's unit never dies here: its owner decides

bool is_person(net::SwarmKind k) { return k == net::SwarmKind::zombie || k == net::SwarmKind::soldier || k == net::SwarmKind::officer || k == net::SwarmKind::enemy; }
// the kinds whose death is shown: the unit goes on for a moment with kUnitDead
bool falls(net::SwarmKind k) { return k == net::SwarmKind::officer || k == net::SwarmKind::enemy; }
u16 drone_id(const PoliceDrone& d) { return static_cast<u16>(d.serial % 65535 + 1); }
}  // namespace

void Game::swarm_clear() {
  for (auto& [o, sw] : swarms_)
    for (RemoteUnit& u : sw.units)
      if (u.person >= 0) people_.at(u.person).state = People::State::gone;
  swarms_.clear();
  swarm_sent_.clear();
  swarm_fallen_.clear();
}

void Game::swarm_collect(std::vector<net::SwarmUnit>& out) {
  const f32 st = world_->sim_time();
  const f64 now = time_ * 1000;
  CombatState& C = combat_;
  auto next_id = [&]() { if (++swarm_serial_ == 0) ++swarm_serial_; return swarm_serial_; };
  std::vector<SwarmSent> sent;
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
  // the police: the officers who are out of their car ...
  for (PoliceUnit& u : units_) {
    if (u.car < 0) continue;
    for (PoliceUnit::Cop& c : u.cops) {
      if (c.person < 0 || c.inside) continue;
      const People::Person& p = people_.at(c.person);
      if (p.seed != c.seed || p.state == People::State::down || p.state == People::State::gone) continue;
      if (!c.id) c.id = next_id();
      const u8 flags = static_cast<u8>((p.aiming ? net::kOfficerAims : 0) | ((c.seed % 5) << net::kOfficerSkinShift));
      out.push_back({c.id, net::SwarmKind::officer, flags, p.pos, wrap_pi(p.yaw), p.v});
      sent.push_back({out.back(), c.person, p.seed});
    }
  }
  // ... every cruiser that stands in the world (driven, abandoned, burning, a wreck) but the one we
  // drive ourselves (that one is our car in our pose) ...
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    if (!c.police || !c.placed || !world_->actor_alive(c.actor) || (mode_ == Mode::car && car_index_ == static_cast<i32>(i))) continue;
    const BodyState s = state_of(c.actor);
    const Vec3 f = s.rotation.rotate({1, 0, 0});
    const bool lights = c.unit >= 0 && c.unit < static_cast<i32>(units_.size()) && units_[c.unit].state != PoliceUnit::State::abandoned &&
                        units_[c.unit].state != PoliceUnit::State::wreck && units_[c.unit].state != PoliceUnit::State::leave;  // (draw_town's light bar)
    const u8 flags = static_cast<u8>((lights ? net::kCruiserLights : 0) | (c.wreck ? net::kCruiserWreck : 0) | (c.burn_until >= 0 ? net::kCruiserBurns : 0));
    out.push_back({static_cast<u16>(i + 1), net::SwarmKind::cruiser, flags, s.position, std::atan2(f.y, f.x), c.car->steer_angle()});
  }
  // ... and the drones
  for (const PoliceDrone& d : pdrones_)
    if (d.active) out.push_back({drone_id(d), net::SwarmKind::police_drone, static_cast<u8>(d.emp ? net::kDroneEmp : 0), d.pos, wrap_pi(d.yaw), 0});
  // the sandbox enemies
  for (EnemyAi& e : C.enemies) {
    if (e.person < 0) continue;
    const People::Person& p = people_.at(e.person);
    if (p.kind != People::Kind::enemy || p.state == People::State::down || p.state == People::State::gone) continue;
    if (!e.id) e.id = next_id();
    const u8 flags = static_cast<u8>((static_cast<u8>(e.type) & net::kEnemyTypeMask) | (p.aiming ? net::kEnemyAims : 0));
    out.push_back({e.id, net::SwarmKind::enemy, flags, p.pos, wrap_pi(p.yaw), p.v});
    sent.push_back({out.back(), e.person, p.seed});
  }
  // who was in the last list, is not in this one and lies dead: fallen (the mates see him go down)
  for (const SwarmSent& was : swarm_sent_) {
    if (std::any_of(sent.begin(), sent.end(), [&](const SwarmSent& s) { return s.unit.id == was.unit.id && s.unit.kind == was.unit.kind; })) continue;
    if (was.person < 0 || was.person >= static_cast<i32>(people_.all().size())) continue;
    const People::Person& p = people_.at(was.person);
    if (p.seed != was.seed || p.state != People::State::down) continue;
    net::SwarmUnit u = was.unit;
    u.flags |= net::kUnitDead;
    u.p = p.pos;
    u.aux = 0;
    swarm_fallen_.push_back({u, now + kFallenMs});
  }
  swarm_sent_ = std::move(sent);
  std::erase_if(swarm_fallen_, [&](const SwarmFallen& f) { return now >= f.until; });
  for (const SwarmFallen& f : swarm_fallen_) out.push_back(f.unit);
}

void Game::swarm_apply_hit(const net::SwarmHit& h) {
  const f32 st = world_->sim_time();
  CombatState& C = combat_;
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
    case net::SwarmKind::officer:
      // the damage is the officer's own rule already (People::shot on the mate's machine): he goes
      // down at 0 as under our rounds - it is not our crime
      for (PoliceUnit& u : units_)
        for (PoliceUnit::Cop& c : u.cops)
          if (u.car >= 0 && c.id == h.id && c.person >= 0 && !c.inside && people_.at(c.person).seed == c.seed) people_.hurt(c.person, h.damage, {}, false, st);
      break;
    case net::SwarmKind::enemy:
      // sandbox_game.mjs: every hit costs him 1 - update_enemies counts it from the health he lost
      for (EnemyAi& e : C.enemies)
        if (e.id == h.id && e.person >= 0) people_.hurt(e.person, std::fmin(h.damage, 100.0f), {}, false, st);
      break;
    case net::SwarmKind::cruiser: {
      const i32 car = static_cast<i32>(h.id) - 1;
      if (car >= 0 && car < static_cast<i32>(cars_.size()) && cars_[car].police && cars_[car].placed) damage_car(car, h.damage, false, town::Cause::round);
      break;
    }
    case net::SwarmKind::police_drone: police_drone_mate_hit(h.id, h.damage); break;
  }
}

void Game::swarm_update(f32 dt) {
  if (!net_.active() || !world_placed_) { if (!swarms_.empty()) swarm_clear(); return; }
  const f64 now = time_ * 1000;
  const f32 st = world_->sim_time();
  // ---- ours out, every 120 ms (only with mates there)
  if (now - swarm_clock_ >= kSyncMs && !net_.remotes().empty()) {
    swarm_clock_ = now;
    std::vector<net::SwarmUnit> out;
    swarm_collect(out);
    net_.send_swarm(out);
  }
  // ---- theirs in
  for (net::Swarm& sw : net_.take_swarms()) {
    RemoteSwarm& r = swarms_[sw.owner];
    r.seen = now;
    std::vector<RemoteUnit> next;
    for (const net::SwarmUnit& u : sw.units) {
      const bool dead = falls(u.kind) && (u.flags & net::kUnitDead);
      auto it = std::find_if(r.units.begin(), r.units.end(), [&](const RemoteUnit& q) { return q.id == u.id && q.kind == u.kind; });
      RemoteUnit q;
      if (it != r.units.end()) { q = *it; it->id = 0; it->person = -1; }  // taken over
      else {
        if (dead) continue;  // went down before we ever saw him
        q.id = u.id;
        q.kind = u.kind;
        q.p = u.p;
        q.yaw = u.yaw;
        const u32 seed = (static_cast<u32>(sw.owner) << 16) | u.id;
        if (u.kind == net::SwarmKind::zombie)
          q.person = people_.add_scripted(People::Kind::zombie, u.p, u.yaw, kRemoteHp, seed, kZShirt[seed % 4], kZPants[seed % 4], kZSkin[seed % 4], 0);
        else if (u.kind == net::SwarmKind::soldier)
          q.person = people_.add_scripted(People::Kind::soldier, u.p, u.yaw, kRemoteHp, seed, kSoldierShirt, kSoldierPants, kSoldierSkins[seed % 4], static_cast<u8>(seed % 4));
        else if (u.kind == net::SwarmKind::officer) {
          // (his skin is his seed's: a seed here that gives the same one)
          q.person = people_.add_officer(u.p, u.yaw, -1, seed * 5 + ((u.flags >> net::kOfficerSkinShift) & 7) % 5);
          people_.at(q.person).hp = kRemoteHp;
          people_.at(q.person).remote = true;
        } else if (u.kind == net::SwarmKind::enemy) {
          const u8 type = std::min<u8>(u.flags & net::kEnemyTypeMask, static_cast<u8>(combat::EnemyType::heavy));
          q.person = people_.add_scripted(People::Kind::enemy, u.p, u.yaw, kRemoteHp, seed, combat::enemy_spec(static_cast<combat::EnemyType>(type)).color, kEnemyPants, kEnemySkin, type);
          people_.at(q.person).remote = true;
        }
      }
      if (dead && !q.fallen) {
        // he went down on his owner's machine: his body falls here and lies its time (People), the
        // unit keeps only his name until the owner drops it
        q.fallen = true;
        if (q.person >= 0) {
          People::Person& p = people_.at(q.person);
          p.remote = false;
          p.pos = {u.p.x, u.p.y, city_.ground_height(u.p.x, u.p.y)};
          people_.hurt(q.person, p.hp, {std::cos(p.yaw), std::sin(p.yaw), 0}, false, st);
          q.person = -1;
        }
      }
      if (u.kind == net::SwarmKind::cruiser && (u.flags & net::kCruiserWreck) && q.wreck_at < 0) q.wreck_at = st;
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
  for (const net::SwarmHit& h : net_.take_swarm_hits()) swarm_apply_hit(h);
  // ---- our rounds and blows on the mates' officers and enemies (People noted them): to their owners
  std::vector<People::RemoteHit> ours;
  people_.take_remote_hits(ours);
  for (const People::RemoteHit& h : ours)
    for (auto& [owner, sw] : swarms_)
      for (const RemoteUnit& u : sw.units)
        if (u.person == h.index) net_.send_swarm_hit(owner, u.id, u.kind, h.damage);
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
      p.aiming = u.kind == net::SwarmKind::zombie || (u.kind == net::SwarmKind::soldier && u.flags == 1) || (u.kind == net::SwarmKind::officer && (u.flags & net::kOfficerAims)) ||
                 (u.kind == net::SwarmKind::enemy && (u.flags & net::kEnemyAims));
      p.hp = kRemoteHp;
    }
    ++it;
  }
}

bool Game::swarm_person_hit(i32 person, f32 damage) {
  for (auto& [owner, sw] : swarms_)
    for (RemoteUnit& u : sw.units) {
      if (u.person != person) continue;
      if (u.kind != net::SwarmKind::zombie && u.kind != net::SwarmKind::soldier) return true;  // (an officer, an enemy: People noted the hit - swarm_update sends it)
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
      if (is_person(u.kind)) continue;
      f32 t = 0;
      f32 damage = 0;
      if (u.kind == net::SwarmKind::cruiser) {
        if (u.flags & net::kCruiserWreck) continue;
        // the car's box as handle_shots takes it (its footprint + 0.4 m), turned by its heading: the
        // first point of the round's line inside it
        const Vec3 half = CarSpec::car().half;
        const kern::physics::Pose rest = car_rest_pose(CarSpec::car(), u.p.x, u.p.y, u.yaw);
        const Quat back = Quat::axis_angle({0, 0, 1}, -u.yaw);
        const Vec3 o = back.rotate(s.from - rest.position), d = back.rotate(seg), h{half.x + 0.4f, half.y + 0.4f, 1.0f};
        f32 t0 = 0, t1 = 1;
        bool miss = false;
        const f32 os[3] = {o.x, o.y, o.z}, ds[3] = {d.x, d.y, d.z}, hs[3] = {h.x, h.y, h.z};
        for (int k = 0; k < 3 && !miss; ++k) {
          if (std::fabs(ds[k]) < 1e-6f) { miss = std::fabs(os[k]) > hs[k]; continue; }
          f32 a = (-hs[k] - os[k]) / ds[k], b = (hs[k] - os[k]) / ds[k];
          if (a > b) std::swap(a, b);
          t0 = std::fmax(t0, a);
          t1 = std::fmin(t1, b);
          miss = t0 > t1;
        }
        if (miss) continue;
        t = t0;
        damage = mode_ == Mode::foot && foot_weapon_ == weapons::Id::sniper ? 4 * kCruiserHit : kCruiserHit;  // car_bullet: the .50 counts as four hits
      } else if (u.kind == net::SwarmKind::police_drone) {
        if (u.flags & net::kDroneEmp) continue;
        t = clampf(kern::dot(u.p - s.from, seg) / l2, 0, 1);
        if (kern::length(s.from + seg * t - u.p) >= 0.55f) continue;  // handle_shots: a sphere of 0.55 m
        damage = kPoliceDroneHit;
      } else {
        if (u.flags) continue;  // already going down / a wreck
        const Vec3 c = u.kind == net::SwarmKind::heli ? u.p : u.p + Vec3{0, 0, 1.2f};
        const f32 r = u.kind == net::SwarmKind::heli ? 2.2f : 2.6f;
        t = clampf(kern::dot(c - s.from, seg) / l2, 0, 1);
        if (kern::length(s.from + seg * t - c) >= r) continue;
        damage = u.kind == net::SwarmKind::heli ? combat::kMilHit : 4;  // a tank's armour: 4 a bullet
      }
      s.to = s.from + seg * t;
      s.surface = 2;
      net_.send_swarm_hit(owner, u.id, u.kind, damage);
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
      else if (u.kind == net::SwarmKind::enemy && u.person >= 0) {
        // the stripe and visor in the type's colour, the HEAVY's tube (game_events.cpp draw_events)
        const People::Person& p = people_.at(u.person);
        const combat::EnemyType type = static_cast<combat::EnemyType>(p.role);
        A.put(CombatArt::glow_box, kern::Mat4::translate(p.pos + Vec3{0, 0, 1.4f}) * kern::Mat4::trs({}, yaw_quat(p.yaw), {0.27f, 0.45f, 0.06f}), combat::enemy_spec(type).stripe * 1.6f);
        if (type == combat::EnemyType::heavy)
          A.put(CombatArt::cylinder, kern::Mat4::translate(p.pos + Vec3{std::cos(p.yaw) * 0.1f, std::sin(p.yaw) * 0.1f, 1.62f}) * kern::Mat4::trs({}, yaw_quat(p.yaw) * Quat::axis_angle({0, 1, 0}, kPi / 2), {0.26f, 0.26f, 1.1f}),
                {0.1f, 0.1f, 0.13f});
      }
    }
}

void Game::swarm_draw_vehicles() {
  // the mates' cruisers: standing on our ground at their place and heading (as a car at rest stands)
  for (auto& [owner, sw] : swarms_)
    for (const RemoteUnit& u : sw.units) {
      if (u.kind != net::SwarmKind::cruiser) continue;
      if (!swarm_cruiser_) swarm_cruiser_ = std::make_unique<Car>(CarSpec::car());
      const kern::physics::Pose rest = car_rest_pose(swarm_cruiser_->spec(), u.p.x, u.p.y, u.yaw);
      swarm_cruiser_->show(u.aux);
      art_.car(kern::Mat4::trs(rest.position, rest.rotation, {1, 1, 1}), *swarm_cruiser_, u.flags & net::kCruiserWreck ? kWreckBlack : kCruiserWhite, false, false);
    }
}

void Game::swarm_draw_town() {
  const f32 now = world_->sim_time();
  for (auto& [owner, sw] : swarms_)
    for (const RemoteUnit& u : sw.units) {
      if (u.kind == net::SwarmKind::cruiser) {
        const kern::physics::Pose rest = car_rest_pose(CarSpec::car(), u.p.x, u.p.y, u.yaw);
        people_art_.light_bar(kern::Mat4::trs(rest.position, rest.rotation, {1, 1, 1}), u.flags & net::kCruiserLights, now);
        if ((u.flags & net::kCruiserWreck) && now - u.wreck_at < 40) people_art_.fire(rest.position + Vec3{0, 0, 0.4f}, 0.35f, now);  // (draw_town's wreck fire)
      } else if (u.kind == net::SwarmKind::police_drone) {
        // leaning into where it flies (draw_town: 0.04 rad per m/s; it is eased to its place at 10 /s)
        const Vec3 v = (u.tp - u.p) * 10.0f;
        people_art_.police_drone(u.p, u.yaw, clampf(-v.y * 0.04f, -0.3f, 0.3f), clampf(v.x * 0.04f, -0.3f, 0.3f), !(u.flags & net::kDroneEmp), now, u.id % 5);
      }
    }
}

}  // namespace sc
