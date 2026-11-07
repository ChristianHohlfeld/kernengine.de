// SpawnCity on Kern: events in the town - people who fight back (pedestrian_agents.mjs,
// gangsters.mjs), zombie nights (zombie_nights.mjs), the sandbox enemies and chaos score
// (sandbox_game.mjs), the gameplay contracts (gameplay_fun_logic.mjs / _runtime.mjs), the race
// track (race_track.mjs), car debris (world_car_explosion.mjs), the sniper scope state
// (sniper_scope.mjs), headshot click (headshot_feedback.mjs); plus the per-frame entry, the
// drawing, the HUD fragment and the test commands of all combat pieces.
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "game.h"
#include "weapons.h"

namespace sc {

using namespace combat;
using kern::Mat4;
using P = People::Person;

namespace {
f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 approach_angle(f32 cur, f32 target, f32 max_step) { return cur + clampf(wrap_pi(target - cur), -max_step, max_step); }
bool alive(const P& p) { return p.state != People::State::down && p.state != People::State::gone; }
// zombie_nights.mjs ZCOL
const Vec3 kZSkin[4] = {{0.54f, 0.6f, 0.47f}, {0.49f, 0.55f, 0.44f}, {0.58f, 0.63f, 0.51f}, {0.44f, 0.49f, 0.4f}};
const Vec3 kZShirt[4] = {{0.36f, 0.35f, 0.29f}, {0.29f, 0.3f, 0.26f}, {0.42f, 0.36f, 0.29f}, {0.25f, 0.29f, 0.31f}};
const Vec3 kZPants[4] = {{0.18f, 0.17f, 0.16f}, {0.23f, 0.21f, 0.19f}, {0.15f, 0.16f, 0.18f}, {0.25f, 0.22f, 0.18f}};
const char* kGateNames[] = {"START / FINISH", "GATE 2", "GATE 3", "HIGH GATE", "GATE 5", "GATE 6", "LOW GATE", "GATE 8"};
}  // namespace

// ---------------------------------------------------------------------------------------------
// people who fight back

void Game::update_fighters_back(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  const bool foot = mode_ == Mode::foot && !player_dead_;
  const Vec3 me = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  const Vec3 me_vel = world_->actor_velocity(soldier_);
  // provoke (pedestrian_agents.mjs): a hit makes the aggressive ones fight; gunfire close by makes
  // the armed aggressive ones draw
  std::vector<Vec3> shots;
  for (const kern::game::ShotEvent& s : world_->shots())
    if (s.shooter == soldier_) shots.push_back(s.from);
  for (std::size_t pi = 0; pi < people_.all().size(); ++pi) {
    P& p = people_.at(static_cast<i32>(pi));
    if (p.kind != People::Kind::citizen || !alive(p)) continue;
    const Temper t = temper_of(p.seed);
    const bool provoked = p.provoked_at > now - 0.5f;
    if (p.state != People::State::fight && t == Temper::aggressive) {
      bool fight = provoked;
      if (!fight && arms_of(p.seed) != Arms::fists)
        for (const Vec3& s : shots) fight = fight || dist2d(s, p.pos) < 12;
      if (fight) { p.state = People::State::fight; p.fight_until = now + 12; p.flee_until = 0; }
    }
    p.provoked_at = -99;
    if (p.state != People::State::fight) continue;
    if (provoked) p.fight_until = now + 12;
    const f32 dx = me.x - p.pos.x, dy = me.y - p.pos.y, d = std::hypot(dx, dy);
    const Arms arm = arms_of(p.seed);
    if (!foot || now > p.fight_until || d > (arm == Arms::gun ? 45.0f : 35.0f)) { p.state = People::State::walk; p.aiming = false; continue; }
    const f32 face = std::atan2(dy, dx);
    p.aiming = arm == Arms::gun;
    const f32 r = agent_random(p.seed);
    if (arm == Arms::gun) {
      // a gangster with a gun keeps 6..14 m and fires a cheap pistol (gangsters.mjs)
      p.yaw = approach_angle(p.yaw, face, dt * 3.4f);
      const f32 want = d > 14 ? 1.0f : d < 6 ? -1.0f : 0.0f, top = want > 0 ? 2.6f : want < 0 ? 1.5f : 0.0f;
      p.v += clampf(top - p.v, -3 * dt, 2.4f * dt);
      if (want != 0) people_.free_move(world_->physics(), p, Vec2{dx / d, dy / d} * want, p.v * dt, false);
      if (now - p.last_attack > 1.5f + r * 0.9f && d < 26 && std::fabs(wrap_pi(face - p.yaw)) < 0.3f) {
        p.last_attack = now;
        const Vec3 from = p.pos + Vec3{std::cos(p.yaw) * 0.5f, std::sin(p.yaw) * 0.5f, 1.42f};
        Vec3 to = me + Vec3{0, 0, 1.33f};
        if (rng_.unit() < gangster_hit_chance(d, std::hypot(me_vel.x, me_vel.y))) world_->damage(soldier_, kGangGunDmg, 0, false, from);
        else to += Vec3{(rng_.unit() - 0.5f) * 2.2f, (rng_.unit() - 0.5f) * 2.2f, (rng_.unit() - 0.55f) * 1.3f};
        kern::game::ShotEvent e;
        const Vec3 dir = kern::normalize(to - from);
        const RayResult h = cast(world_->physics(), from, dir, d + 14, {}, kCatWorld);
        e.from = from;
        e.to = h.hit ? h.point : from + dir * (d + 8);
        e.impact = h.hit;
        e.surface = h.hit ? 1 : 0;
        e.normal = h.normal;
        world_->shots().push_back(e);
        combat_tracer(from, e.to, {3.0f, 2.2f, 1.0f});
        world_->play_sound(combat_sfx(combat_audio::Kind::pistol), from, 0.6f, 0.92f + rng_.unit() * 0.1f, true);
        osfx("gangsters.bang", from);
        people_.frighten(from, 22, now, 1);  // other people run from gangster gunfire like from yours
        ++C.gangster_shots;
      }
    } else {
      p.yaw = approach_angle(p.yaw, face, dt * 3);
      const f32 reach = arm == Arms::knife ? 0.8f : 0.95f;
      if (d > reach) {
        p.v += clampf(std::fmin(2.8f, p.speed + 1.3f) - p.v, -3 * dt, 2.4f * dt);
        people_.free_move(world_->physics(), p, {dx / d, dy / d}, std::fmin(d - 0.9f, p.v * dt), false);
      } else {
        p.v = std::fmax(0.0f, p.v - 5 * dt);
        const f32 every = arm == Arms::knife ? 0.64f + r * 0.3f : 0.78f + r * 0.38f;
        if (now - p.last_attack > every) {
          p.last_attack = now;
          // a stab: ~16 % of the player's health; a punch of an untrained adult ~8 %
          world_->damage(soldier_, arm == Arms::knife ? kStabDmg : kPunchDmg, 0, false, p.pos + Vec3{0, 0, 1.2f});
          world_->play_sound(arm == Arms::knife ? combat_sfx(combat_audio::Kind::hit) : "builtin:impact_light", me + Vec3{0, 0, 1.2f}, 0.7f, arm == Arms::knife ? 1.3f : 0.8f, true);
          if (arm == Arms::knife) osfx("gangsters.stabSound", me);  // "arondight:pedestrian-stab" at the player
          ++(arm == Arms::knife ? C.stabs : C.punches);
        }
      }
    }
    p.phase += p.v * dt * 3.4f;
  }
}

// ---------------------------------------------------------------------------------------------
// zombie_nights.mjs

void Game::update_zombies(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  i32 up = 0;
  for (const ZombieAi& z : C.zombie_ai) up += z.person >= 0;
  const bool night = C.zombies_enabled && (night_ > 0.55f || C.zombie_force);
  const Vec3 me = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  switch (C.zombies.update(now, night, up)) {
    case ZombieEvent::night: combat_banner("DIE NACHT BRICHT HEREIN"); break;
    case ZombieEvent::round: combat_banner("RUNDE " + std::to_string(C.zombies.round)); break;
    case ZombieEvent::round_done: combat_banner("RUNDE " + std::to_string(C.zombies.round) + " UEBERSTANDEN"); break;
    case ZombieEvent::dawn:
      for (ZombieAi& z : C.zombie_ai)
        if (z.person >= 0) people_.at(z.person).state = People::State::gone;
      C.zombie_ai.clear();
      if (C.zombies.round) combat_banner("MORGENGRAUEN - " + std::to_string(C.zombies.round) + (C.zombies.round == 1 ? " RUNDE" : " RUNDEN") + " UEBERLEBT");
      C.zombies.round = 0;
      break;
    case ZombieEvent::spawn: {
      // the dead rise only where no player sees it, 34..70 m around him
      for (int k = 0; k < 14; ++k) {
        const f32 a = rng_.range(0, 2 * kPi), r = rng_.range(kZombieSpawnMin, kZombieSpawnMax);
        const Vec3 p{me.x + std::cos(a) * r, me.y + std::sin(a) * r, 0};
        const RayResult roof = cast(world_->physics(), {p.x, p.y, 60}, {0, 0, -1}, 70, {}, kCatWorld);
        if (roof.hit && roof.point.z > 0.35f) continue;
        if (!spawn_unseen(p, 1.9f)) continue;
        const u32 seed = rng_.next();
        ZombieAi z;
        z.person = people_.add_scripted(People::Kind::zombie, p, std::atan2(me.y - p.y, me.x - p.x), C.zombies.hp(), seed, kZShirt[seed % 4], kZPants[seed % 4], kZSkin[seed % 4], 0);
        z.speed = C.zombies.speed(rng_.unit(), rng_.unit());
        z.last_hp = C.zombies.hp();
        C.zombie_ai.push_back(z);
        C.zombies.spawned(now);
        break;
      }
      break;
    }
    case ZombieEvent::none: break;
  }
  const bool foot = mode_ == Mode::foot && !player_dead_;
  for (ZombieAi& z : C.zombie_ai) {
    if (z.person < 0) continue;
    P& p = people_.at(z.person);
    if (p.kind != People::Kind::zombie || !alive(p)) { z.person = -1; continue; }
    if (p.hp < z.last_hp) { C.zombies.points += kZombieHitPts; osfx("zombie.shriek", p.pos + Vec3{0, 0, 1.6f}); }  // every hit 10 points (and a shriek)
    z.last_hp = p.hp;
    const Vec3 target = foot ? me : player_position();
    // a player in a boarded-up building: to the door, break the planks, in (game_barricades.cpp)
    bool walled = false, hold = false;
    const Vec3 goal = gaps_zombie_goal(z.person, p.pos, target, now, walled, hold);
    const f32 gx = goal.x - p.pos.x, gy = goal.y - p.pos.y, gd = std::hypot(gx, gy);
    const f32 dx = target.x - p.pos.x, dy = target.y - p.pos.y, d = std::hypot(dx, dy);
    p.aiming = true;  // arms reaching forward
    if (!hold && gd > (goal.x == target.x && goal.y == target.y ? 0.9f : 0.05f)) {
      const bool ok = people_.free_move(world_->physics(), p, {gx / gd, gy / gd}, std::fmin(gd, z.speed * dt), false);
      p.v = ok ? z.speed : 0;
    } else p.v = 0;
    p.yaw = approach_angle(p.yaw, std::atan2(gy, gx), dt * 8);
    p.phase += p.v * dt * 3.4f;
    if (foot && d < 1.35f && !walled && now >= z.next_hit) {  // nobody is clawed through an intact barricade
      z.next_hit = now + kZombieMeleeS;
      z.swing = now;
      world_->damage(soldier_, kZombieMelee, 0, false, p.pos + Vec3{0, 0, 1.3f});
    }
    if (d < 34 && now - C.last_groan > 0.7f && rng_.unit() < dt * 0.4f) {
      C.last_groan = now;
      osfx("zombie_nights.groan", p.pos + Vec3{0, 0, 1.6f});  // a rasping groan, each its own (audio.mjs zombieVoice)
    }
    // far stragglers drop out where nobody sees them go (the round keeps its pressure)
    if (d > 140 && spawn_unseen(p.pos, 1.9f)) { p.state = People::State::gone; z.person = -1; ++C.zombies.to_spawn; }
  }
  std::erase_if(C.zombie_ai, [](const ZombieAi& z) { return z.person < 0; });
}

// ---------------------------------------------------------------------------------------------
// sandbox_game.mjs: enemies on foot

void Game::update_enemies(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  const i32 stars = wanted_.stars();
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && mode_ != Mode::jet;
  i32 up = 0, heavies = 0;
  for (const EnemyAi& e : C.enemies)
    if (e.person >= 0 && !e.retreat) { ++up; heavies += e.type == EnemyType::heavy; }
  if (stars > 0) {
    C.no_wanted_since = -1;
    if (have && up < std::min(kMaxEnemies, kEnemyBudget[std::min(5, stars)]) && now - C.last_spawn > kEnemySpawnS) {
      C.last_spawn = now;
      EnemyType type;
      choose_enemy(stars, rng_.unit(), heavies, type);
      for (int k = 0; k < 10; ++k) {
        const f32 a = rng_.range(0, 2 * kPi), d = rng_.range(55, 85);
        const Vec3 p{tp.x + std::cos(a) * d, tp.y + std::sin(a) * d, 0};
        const RayResult roof = cast(world_->physics(), {p.x, p.y, 60}, {0, 0, -1}, 70, {}, kCatWorld);
        if ((roof.hit && roof.point.z > 0.35f) || !spawn_unseen(p, 2.2f)) continue;
        const EnemySpec& s = enemy_spec(type);
        EnemyAi e;
        e.type = type;
        e.person = people_.add_scripted(People::Kind::enemy, p, a + kPi, 1e4f, rng_.next(), s.color, {0.08f, 0.07f, 0.12f}, {0.88f, 0.7f, 0.58f}, static_cast<u8>(type));
        e.last_hp = 1e4f;
        e.next_shot = now + rng_.range(0.9f, 1.8f);
        e.strafe = rng_.unit() < 0.5f ? -1.0f : 1.0f;
        C.enemies.push_back(e);
        break;
      }
    }
  } else {
    if (C.no_wanted_since < 0) C.no_wanted_since = now;
    if (now - C.no_wanted_since > 2.5f) for (EnemyAi& e : C.enemies) e.retreat = true;
  }
  for (EnemyAi& e : C.enemies) {
    if (e.person < 0) continue;
    P& p = people_.at(e.person);
    if (p.kind != People::Kind::enemy || !alive(p)) { e.person = -1; continue; }
    const EnemySpec& s = enemy_spec(e.type);
    if (p.hp < e.last_hp) { e.hits += 1; p.hp = e.last_hp; e.next_shot = std::fmin(e.next_shot, now + 0.35f); }  // every hit costs 1
    if (e.hits >= s.hp) { people_.blast(p.pos + Vec3{0, 0, 0.9f}, 0.25f, 1e5f, true, now); continue; }
    if (!have || e.hold) continue;
    const f32 dx = tp.x - p.pos.x, dy = tp.y - p.pos.y, d = std::hypot(dx, dy);
    if (d > 300 || (e.retreat && d > 120 && spawn_unseen(p.pos, 2))) { p.state = People::State::gone; e.person = -1; continue; }
    const f32 want = std::atan2(dy, dx);
    f32 move = 0, side = 0;
    if (e.retreat) move = -1;
    else if (d > s.range) move = 1;
    else if (d < s.range * 0.45f) move = -0.6f;
    else { side = e.strafe * 0.5f; if (rng_.unit() < dt * 0.25f) e.strafe = -e.strafe; }
    const f32 heading = move < 0 ? want + kPi : side != 0 && move == 0 ? want + kPi / 2 * sign0(side) : want;
    const f32 spd = s.speed * (move != 0 ? std::fabs(move) : std::fabs(side));
    if (spd > 0) people_.free_move(world_->physics(), p, {std::cos(heading), std::sin(heading)}, spd * dt, false);
    p.v = spd;
    p.yaw = want;
    p.aiming = !e.retreat && d < s.range * 1.15f;
    p.phase += p.v * dt * 3.4f;
    if (e.retreat || d >= s.range * 1.15f || now < e.next_shot) continue;
    const Vec3 from = p.pos + Vec3{0, 0, 1.45f * s.scale};
    if (!line_of_sight(from, tp)) { e.next_shot = now + 0.5f; continue; }
    if (e.burst_left > 0) { --e.burst_left; e.next_shot = now + 0.11f; }
    else { e.burst_left = s.burst - 1; e.next_shot = now + s.shot_s * rng_.range(0.8f, 1.25f); }
    if (e.type == EnemyType::heavy) {
      // HEAVY: a slow visible rocket (26 m/s), a real explosion on impact
      C.rockets.push_back({from, kern::normalize(tp - from) * 26.0f, kern::length(tp - from) / 26 + 0.2f});
      world_->play_sound("builtin:launcher", from, 0.9f, 0.6f, true);
      osfx("sandbox_game.sfx", from, 0, 0.9f, 0.6f);  // sfx("shot", {gain: .9, rate: .6, at: from})
      continue;
    }
    const f32 dist = kern::length(tp - from);
    Vec3 aim = tp;
    if (rng_.unit() < enemy_hit_chance(s, dist, kern::length(tv))) { hurt_player(s.dmg, s.dmg, from); world_->shake_camera(0.12f); }
    else aim += Vec3{rng_.range(-2.5f, 2.5f), rng_.range(-2.5f, 2.5f), rng_.range(-0.6f, 1.2f)};
    combat_tracer(from, aim, e.type == EnemyType::cop ? Vec3{3.0f, 2.8f, 1.9f} : Vec3{3.0f, 2.0f, 0.8f});
    world_->play_sound(combat_sfx(combat_audio::Kind::pistol), from, 0.45f, e.type == EnemyType::cop ? 1.35f : 1.05f, true);
    osfx("sandbox_game.sfx", from, 0, 0.45f, e.type == EnemyType::cop ? 1.35f : 1.05f);  // sfx("shot", ...)
  }
  std::erase_if(C.enemies, [](const EnemyAi& e) { return e.person < 0; });
  for (EnemyRocket& r : C.rockets) {
    const Vec3 prev = r.p;
    r.p += r.v * dt;
    r.life -= dt;
    const Vec3 step = r.p - prev;
    const RayResult h = cast(world_->physics(), prev, kern::normalize(step, {0, 0, -1}), kern::length(step) + 0.05f, {}, kCatWorld);
    const bool near = have && kern::length(r.p - tp) < 1.6f;
    if (r.life <= 0 || h.hit || near || r.p.z <= city_.ground_height(r.p.x, r.p.y) + 0.15f) {
      osfx("sandbox_game.sfx", h.hit ? h.point : r.p, 1, 1, 1);  // sfx("explosion", {gain: 1, at: p})
      explosion(h.hit ? h.point : r.p, 6, 90, false, BlastBy::weapon);  // the enemy rocket
      r.life = -99;
    }
  }
  std::erase_if(C.rockets, [](const EnemyRocket& r) { return r.life <= -99; });
}

// ---------------------------------------------------------------------------------------------
// scores: chaos (sandbox_game.mjs) and contracts (gameplay_fun_logic.mjs)

void Game::update_scores(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  const i32 stars = wanted_.stars();
  auto pop = [&](const std::string& t) { if (!t.empty()) { C.pop = t; C.pop_at = now; } };
  auto award = [&](i32 pts, const char* label) {
    C.chaos.award(pts, now);
    if (label) pop(C.chaos.combo > 1 ? std::string(label) + "  x" + std::to_string(C.chaos.combo) : std::string(label));
  };
  std::vector<People::Death> deaths;
  people_.take_deaths(deaths);
  for (const People::Death& d : deaths) {
    if (d.head && d.by_player) { world_->play_sound(combat_sfx(combat_audio::Kind::crack), cam_.eye, 0.32f, 1.0f, false); osfx("headshot_feedback.announceHeadshot", cam_.eye); }  // headshot_feedback.mjs: a neutral short click
    if (!d.by_player) continue;
    // "npc-death": the mates' copy of this person falls too (the same id on every machine)
    if (net_.active() && d.kind == People::Kind::citizen && d.index >= 0) {
      const People::Person& dp = people_.at(d.index);
      net::Fx f;
      f.kind = net::Fx::npc_death;
      f.p = dp.pos;
      f.dir = {static_cast<f32>(dp.seed & 0xffffu), static_cast<f32>(dp.seed >> 16), dp.yaw};
      net_.send_fx(f);
    }
    switch (d.kind) {
      case People::Kind::zombie: C.zombies.points += kZombieKillPts; rounds_zombie_down(); break;
      case People::Kind::enemy: {
        const EnemyType t = static_cast<EnemyType>(people_.at(d.index).role);
        award(enemy_spec(t).score, t == EnemyType::cop ? "TAKEDOWN!" : t == EnemyType::swat ? "SWAT DOWN!" : "HEAVY DOWN!!");
        break;
      }
      case People::Kind::soldier: award(40, nullptr); break;
      case People::Kind::citizen:
      case People::Kind::officer:
        award(40, nullptr);
        pop(C.fun.apply(FunEvent::world_kill_person, now, stars));
        break;
    }
  }
  // wrecked cars: debris and score (world_car_explosion.mjs, sandbox onWorldKill)
  if (C.car_wrecked.size() < cars_.size()) C.car_wrecked.resize(cars_.size(), 0);
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    const bool w = c.placed && c.wreck;
    if (w && !C.car_wrecked[i]) {
      const bool bus = c.car->spec().radius > 0.4f;
      if (c.hit_by_player) { award(bus ? 220 : 140, bus ? "BUS WRECKED!" : "CAR WRECKED!"); pop(C.fun.apply(bus ? FunEvent::world_kill_bus : FunEvent::world_kill_car, now, stars)); }
      const Mat4 m = matrix_of(c.actor);
      const Vec3 at = m.translation();
      const Vec3 h = c.car->spec().half;
      // nine parts: bonnet, boot, doors, four wheels, a bumper - thrown out and bouncing (0.30 / 0.76)
      for (int k = 0; k < 9; ++k) {
        Debris d;
        const f32 a = rng_.range(0, 2 * kPi), sp = rng_.range(4, 9);
        d.p = at + Vec3{std::cos(a) * h.x * 0.5f, std::sin(a) * h.y * 0.5f, 0.8f};
        d.v = {std::cos(a) * sp, std::sin(a) * sp, rng_.range(6, 11)};
        d.spin = {rng_.range(-9, 9), rng_.range(-9, 9), rng_.range(-6, 6)};
        const bool wheel = k >= 4 && k < 8;
        d.size = wheel ? Vec3{0.62f, 0.62f, 0.24f} : Vec3{rng_.range(0.6f, 1.4f), rng_.range(0.5f, 1.0f), 0.08f};
        d.color = wheel ? Vec3{0.06f, 0.06f, 0.07f} : c.color * 0.55f;
        d.born = now;
        C.debris.push_back(d);
      }
    }
    C.car_wrecked[i] = w;
  }
  // police drones: destroyed / knocked out by EMP
  i32 emp_hits = 0;
  for (int i = 0; i < 5; ++i) {
    const PoliceDrone& d = pdrones_[i];
    if (C.pd_was[i] && !d.active && d.destroyed_until > now) { award(300, "COP DRONE DOWN!"); pop(C.fun.apply(FunEvent::police_kill, now, stars)); }
    if (d.active && d.emp && !C.pd_emp[i]) ++emp_hits;
    C.pd_was[i] = d.active;
    C.pd_emp[i] = d.active && d.emp;
  }
  if (emp_hits) pop(C.fun.apply(FunEvent::emp, now, stars, static_cast<f32>(emp_hits)));
  // stars
  if (stars > C.last_stars && stars > 0) pop("WANTED " + std::string(static_cast<std::size_t>(stars), '*'));
  if (C.last_stars > 0 && stars == 0 && !player_dead_) { award(400 * C.last_stars, "ESCAPED!"); pop(C.fun.apply(FunEvent::escape, now, C.last_stars)); }
  C.last_stars = stars;
  // damage and death
  const f32 hp = world_->hud().health;
  if (C.last_health >= 0 && hp >= 0 && hp < C.last_health && mode_ == Mode::foot) C.fun.apply(FunEvent::damage, now, stars, C.last_health - hp);
  C.last_health = hp;
  if (player_dead_ && !C.was_dead) pop(C.fun.apply(FunEvent::player_death, now, stars));
  C.was_dead = player_dead_;
  C.fun.apply(FunEvent::tick, now, stars);
  C.chaos.tick(dt, now);
}

// ---------------------------------------------------------------------------------------------
// race_track.mjs

void Game::update_race() {
  CombatState& C = combat_;
  if (!C.race_on || mode_ != Mode::drone || !drone_out_ || !drone_.telemetry().armed) return;
  const f32 now = world_->sim_time();
  const i32 passed = C.race.update(matrix_of(drone_actor_).translation() - C.race_origin, now);
  if (passed < 0) return;
  if (C.race.finished) combat_banner("ZIEL · " + race_time(C.race.finish_time) + " · Bestrunde " + race_time(C.race.best_lap));
  else if (passed == 0) { C.pop = "RUNDE " + std::to_string(C.race.lap) + "/" + std::to_string(kRaceLaps); C.pop_at = now; }
  else { C.pop = kGateNames[passed]; C.pop_at = now; }
  world_->play_sound("builtin:checkpoint", cam_.eye, 0.6f, 1, false);
}

// ---------------------------------------------------------------------------------------------
// debris (world_car_explosion.mjs: bounce 0.30, friction 0.76, life 6.5 s)

void Game::update_debris(f32 dt) {
  const f32 now = world_->sim_time();
  for (Debris& d : combat_.debris) {
    d.v.z -= kG * dt;
    d.p += d.v * dt;
    d.q = qnormalize(d.q * Quat::axis_angle(kern::normalize(d.spin, {0, 0, 1}), kern::length(d.spin) * dt));
    const f32 floor = city_.ground_height(d.p.x, d.p.y) + 0.06f + d.size.z * 0.5f;
    if (d.p.z < floor) {
      d.p.z = floor;
      if (d.v.z < 0) d.v.z = -d.v.z * 0.3f;
      d.v.x *= 0.76f;
      d.v.y *= 0.76f;
      d.spin = d.spin * 0.6f;
    }
  }
  std::erase_if(combat_.debris, [&](const Debris& d) { return now - d.born > d.life; });
  if (combat_.debris.size() > 7 * 9) combat_.debris.erase(combat_.debris.begin(), combat_.debris.begin() + static_cast<std::ptrdiff_t>(combat_.debris.size() - 63));
}

// ---------------------------------------------------------------------------------------------
// per frame

void Game::update_combat(f32 dt) {
  if (!world_placed_ || dt <= 0) return;
  CombatState& C = combat_;
  // The air strike is aimed and called in game_controls.cpp (4 / J / D-pad up / STRIKE, fighter_jets.mjs);
  // the race ("race") and the next contract ("contract") are commands: spawncity.de has no keys for them.
  C.scope = mode_ == Mode::foot && foot_weapon_ == weapons::Id::sniper && world_->aim_blend() > 0.85f && !player_dead_;  // stepAds scoped
  // player rockets / grenades that fly into the helicopter, the tank or a flak gun go off there
  for (Projectile& p : projectiles_) {
    if (!p.alive || !p.by_player || p.id == weapons::Id::hand_grenade) continue;
    bool boom = false;
    for (const Heli& h : C.helis) boom = boom || (h.state != Heli::State::down && kern::length(h.p - p.pos) < 3.2f);
    for (const Tank& k : C.tanks) boom = boom || (!k.wreck && kern::length(k.p + Vec3{0, 0, 1.2f} - p.pos) < 3.4f);
    if (boom) { p.alive = false; const weapons::Spec& s = weapons::spec(p.id); explosion(p.pos, std::fmax(2.0f, s.radius_m), s.damage, true, BlastBy::weapon); }
  }
  update_military(dt);
  update_flak(dt);
  update_jets(dt);
  update_fighters_back(dt);
  update_zombies(dt);
  update_enemies(dt);
  update_scores(dt);
  update_race();
  update_debris(dt);
}

void Game::draw_events() {
  CombatArt& A = combat_art_;
  const f32 now = world_->sim_time();
  for (const Debris& d : combat_.debris) {
    const f32 fade = clampf((d.born + d.life - now) / 1.4f, 0, 1);
    A.put(CombatArt::box, Mat4::trs(d.p, d.q, d.size * (0.3f + 0.7f * fade)), d.color);
  }
  for (const EnemyRocket& r : combat_.rockets) A.put(CombatArt::glow_sphere, Mat4::translate(r.p) * Mat4::scale({0.44f, 0.44f, 0.44f}), {3.0f, 1.6f, 0.5f});
  // sandbox enemies: the stripe and visor in the type's colour (sandbox_game.mjs buildEnemy)
  for (const EnemyAi& e : combat_.enemies) {
    if (e.person < 0) continue;
    const P& p = people_.all()[static_cast<std::size_t>(e.person)];
    const EnemySpec& s = enemy_spec(e.type);
    A.put(CombatArt::glow_box, Mat4::translate(p.pos + Vec3{0, 0, 1.4f}) * Mat4::trs({}, yaw_quat(p.yaw), {0.27f, 0.45f, 0.06f}), s.stripe * 1.6f);
    if (e.type == EnemyType::heavy) A.put(CombatArt::cylinder, Mat4::translate(p.pos + Vec3{std::cos(p.yaw) * 0.1f, std::sin(p.yaw) * 0.1f, 1.62f}) * Mat4::trs({}, yaw_quat(p.yaw) * Quat::axis_angle({0, 1, 0}, kPi / 2), {0.26f, 0.26f, 1.1f}), {0.1f, 0.1f, 0.13f});
  }
  // wrecked tanks and the like burn
  for (const Tank& k : combat_.tanks)
    if (k.wreck && now - k.wreck_at < 40)
      for (int i = 0; i < 3; ++i) {
        const f32 t = now * (3.1f + static_cast<f32>(i)) + static_cast<f32>(i) * 2.1f, s = 1.4f * (0.55f + 0.25f * std::sin(t));
        A.put(CombatArt::glow_sphere, Mat4::translate(k.p + Vec3{0.3f * std::sin(t), 0.3f * std::cos(t), 2 + s * 0.6f + static_cast<f32>(i) * 0.5f}) * Mat4::scale({s, s, s * 1.4f}), {2.8f, 1.1f, 0.25f});
      }
  // headlights at night (traffic, buses, cruisers, the player's car): two warm glows at the front
  // and a soft pool of light on the road ahead
  if (night_ > 0.25f) {
    const f32 k = clampf((night_ - 0.25f) / 0.5f, 0, 1);
    for (std::size_t ci = 0; ci < cars_.size(); ++ci) {
      const TrafficCar& c = cars_[ci];
      if (!c.placed || c.wreck || !world_->actor_alive(c.actor)) continue;
      if (!c.driver && !(mode_ == Mode::car && static_cast<i32>(ci) == car_index_)) continue;  // a parked, empty car is dark
      const Mat4 m = matrix_of(c.actor);
      const Vec3 pos = m.translation();
      if (kern::length(pos - cam_.eye) > 160) continue;
      const Vec3 fwd = kern::normalize(Vec3{m.m[0], m.m[1], 0}, {1, 0, 0}), side{-fwd.y, fwd.x, 0};
      const Vec3 h = c.car->spec().half;
      for (f32 sd : {-1.0f, 1.0f})
        A.put(CombatArt::glow_sphere, Mat4::translate(pos + fwd * (h.x + 0.02f) + side * (sd * h.y * 0.7f) + Vec3{0, 0, 0.05f}) * Mat4::scale({0.16f, 0.16f, 0.12f}), Vec3{3.2f, 3.0f, 2.6f} * k);
      const Vec3 pool = pos + fwd * (h.x + 5.5f);
      const Vec3 g{pool.x, pool.y, city_.ground_height(pool.x, pool.y) + 0.06f};
      A.put(CombatArt::fade_sphere, Mat4::translate(g) * Mat4::trs({}, yaw_quat(std::atan2(fwd.y, fwd.x)), {9.0f, 4.2f, 0.05f}), Vec3{1.0f, 0.93f, 0.78f} * 1.6f, 0.2f * k);
    }
  }
  // a burnt-out car or bus: flames for 9 s, then a dark smoke column until it is cleared
  for (const TrafficCar& c : cars_) {
    if (!c.placed || !c.wreck) continue;
    const f32 age = now - c.wreck_at;
    const Vec3 at = matrix_of(c.actor).translation();
    for (int i = 0; i < 3; ++i) {
      const f32 t = now * (2.7f + static_cast<f32>(i)) + static_cast<f32>(i) * 2.3f;
      if (age < 9) {
        const f32 s = 1.1f * (0.55f + 0.25f * std::sin(t)) * clampf(1.4f - age / 9, 0.3f, 1);
        A.put(CombatArt::glow_sphere, Mat4::translate(at + Vec3{0.35f * std::sin(t), 0.35f * std::cos(t), 1.1f + s * 0.5f + static_cast<f32>(i) * 0.4f}) * Mat4::scale({s, s, s * 1.5f}), {2.8f, 1.05f, 0.22f});
      }
      const f32 rise = std::fmod(now * 0.9f + static_cast<f32>(i) * 1.3f, 4.0f), s = 0.7f + rise * 0.45f;
      A.put(CombatArt::fade_sphere, Mat4::translate(at + Vec3{0.4f * std::sin(t * 0.3f), 0.4f * std::cos(t * 0.3f), 1.6f + rise * 1.6f}) * Mat4::scale({s, s, s}), {0.05f, 0.05f, 0.055f}, 0.55f * (1 - rise / 4));
    }
  }
  // race gates (race_track.mjs: start yellow, next green, passed blue, others white)
  if (combat_.race_on)
    for (i32 i = 0; i < kGates; ++i) {
      const Gate& g = gate(i);
      Vec2 n, r;
      gate_axes(i, n, r);
      const Race& R = combat_.race;
      Vec3 col = g.start ? Vec3{1.0f, 0.78f, 0.34f} : Vec3{0.95f, 0.96f, 0.97f};
      if (R.started && !R.finished && i < R.next && R.next != 0) col = {0.35f, 0.72f, 1.0f};
      if (!R.finished && i == R.next) col = Vec3{0.43f, 0.96f, 0.66f} * 2.2f;
      A.gate(combat_.race_origin + g.center, std::atan2(r.y, r.x), g.width, g.height, col);
    }
}

void Game::draw_combat() {
  combat_art_.begin();
  if (world_placed_) { draw_military(); draw_events(); destruction_.draw(combat_art_); draw_guided_mark(); rounds_draw(); }
  combat_art_.end();
}

// ---------------------------------------------------------------------------------------------
// HUD and commands

void Game::combat_hud(std::string& s) {
  const CombatState& C = combat_;
  const f32 now = world_->sim_time();
  i32 soldiers = 0, fighters = 0;
  for (const SoldierAi& x : C.soldiers) soldiers += x.person >= 0;
  for (const P& p : people_.all()) fighters += p.state == People::State::fight;
  u32 flak_alive = 0;
  for (const FlakGun& g : C.flak) flak_alive += !g.dead;
  const Contract& k = contract(C.fun.contract_index);
  char b[768];
  std::snprintf(b, sizeof b,
                ",\"combat\":{\"mil\":\"%s\",\"helis\":%zu,\"tanks\":%zu,\"soldiers\":%d,\"flak\":%u,\"flakDestroyed\":%u,\"flakHits\":%u,\"flakRounds\":%zu,"
                "\"jets\":%zu,\"bombs\":%zu,\"strike\":%.1f,\"fighters\":%d,\"gangsterShots\":%u,\"enemies\":%zu,"
                "\"zombies\":{\"night\":%s,\"round\":%d,\"alive\":%zu,\"points\":%d},"
                "\"chaos\":{\"score\":%d,\"combo\":%d,\"flow\":%.2f},"
                "\"fun\":{\"score\":%d,\"risk\":%d,\"high\":%d,\"combo\":%d,\"mult\":%.2f,\"contract\":\"%s\",\"hint\":\"%s\",\"progress\":%d,\"target\":%d,\"done\":%s},"
                "\"scope\":%s,\"debris\":%zu,\"drone\":%.1f",
                C.military.active ? "on" : C.military.alert_at >= 0 ? "alert" : "off", C.helis.size(), C.tanks.size(), soldiers, flak_alive, C.flak_destroyed,
                C.flak_hits, C.flak_rounds.size(), C.jets.size(), C.bombs.size(), std::fmax(0.0f, kStrikeCooldownS - (now - C.last_strike)), fighters,
                C.gangster_shots, C.enemies.size(), C.zombies.active ? "true" : "false", C.zombies.round, C.zombie_ai.size(), C.zombies.points, C.chaos.score,
                C.chaos.combo, C.chaos.flow, C.fun.score, C.fun.at_risk, C.fun.high_score, C.fun.combo, C.fun.multiplier(now), k.name, k.hint,
                C.fun.contract_progress, k.target, C.fun.contract_complete ? "true" : "false", C.scope ? "true" : "false", C.debris.size(),
                std::fmax(0.0f, C.drone_ready_at - now));
  s += b;
  if (C.race_on) {
    const Race& R = C.race;
    const f32 cur = R.started ? (R.finished ? R.last_lap : now - R.lap_start) : 0;
    std::snprintf(b, sizeof b, ",\"race\":{\"lap\":%d,\"laps\":%d,\"next\":\"%s\",\"time\":\"%s\",\"best\":\"%s\",\"finished\":%s}", R.lap, kRaceLaps,
                  R.finished ? "FINISHED" : kGateNames[R.next], race_time(cur).c_str(), race_time(R.best_lap).c_str(), R.finished ? "true" : "false");
    s += b;
  }
  if (now - C.banner_at < 3.8f) s += ",\"banner\":\"" + C.banner + "\"";
  if (now - C.pop_at < 1.5f) s += ",\"pop\":\"" + C.pop + "\"";
  s += "}";
}

bool Game::combat_command(const std::string& cmd, std::string& out) {
  CombatState& C = combat_;
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  const f32 now = world_->sim_time();
  if (!world_placed_ && (starts("military") || starts("strike") || starts("zombies") || starts("race") || starts("enemy") || starts("provoke"))) { out = "not placed"; return true; }
  if (cmd == "person show" && world_placed_) {
    // Tests: a citizen 3 m ahead, facing the camera, standing (the look of the people)
    const Vec3 me = player_position();
    const Vec3 fwd = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
    const Vec3 p{me.x + fwd.x * 3.0f, me.y + fwd.y * 3.0f, city_.ground_height(me.x + fwd.x * 3.0f, me.y + fwd.y * 3.0f)};
    people_.add_scripted(People::Kind::zombie, p, std::atan2(-fwd.y, -fwd.x), 100, rng_.next(), {0.7f, 0.2f, 0.18f}, {0.15f, 0.18f, 0.3f}, {0.86f, 0.66f, 0.5f}, 0);
    out = "ok";
    return true;
  }
  if (starts("heat ")) {
    // Tests: heat to N (witnessed police kills, severity 3 each)
    const f32 n = static_cast<f32>(std::atof(cmd.c_str() + 5));
    for (int k = 0; k < 200 && wanted_.heat() < n; ++k) wanted_.report(wanted::Crime::police_officer, true);
    star_since_ = now - wanted::arrival_delay_ms(wanted_.stars()) / 1000;
    out = std::to_string(wanted_.heat());
    return true;
  }
  if (cmd == "military start") {
    // Tests: five stars at the trigger heat, then the army at once (no hold, no alert)
    for (int k = 0; k < 200 && wanted_.heat() < kTriggerHeat; ++k) wanted_.report(wanted::Crime::police_officer, true);
    C.military.active = true;
    C.military.alert_at = -1;
    C.military.next_reinforce = now + kFirstReinforceS;
    C.pending_heli = 1;
    C.pending_squad = kSquad;
    C.pending_tank = 1;
    C.pending_at = 0;
    combat_banner("MILITAER RUECKT AN");
    out = "on";
    return true;
  }
  if (cmd == "military off" || cmd == "military on") {
    C.military_enabled = cmd == "military on";
    if (!C.military_enabled) { C.military.active = false; C.helis.clear(); for (Tank& k : C.tanks) if (k.body.valid() && world_->physics().body_valid(k.body)) world_->physics().destroy_body(k.body); C.tanks.clear(); C.shells.clear(); for (SoldierAi& s : C.soldiers) if (s.person >= 0) people_.at(s.person).state = People::State::gone; C.soldiers.clear(); }
    out = C.military_enabled ? "on" : "off";
    return true;
  }
  if (cmd == "strike" || starts("strike ")) {
    Vec3 at;
    if (cmd == "strike") { Vec3 dir; const Vec3 eye = aim_ray(dir); const RayResult r = cast(world_->physics(), eye, dir, 1500, {}, kCatWorld); at = r.hit ? r.point : eye + dir * 180.0f; }
    else { const Vec3 me = player_position(); f32 x = 0, y = 0; std::sscanf(cmd.c_str() + 7, "%f %f", &x, &y); at = me + Vec3{x, y, 0}; }
    C.last_strike = cmd == "strike" ? C.last_strike : -1e9f;  // tests: no cooldown with coordinates
    out = air_strike(at) ? "ok" : "cooldown";
    return true;
  }
  if (cmd == "flyover") { launch_jets(cam_.eye.x, cam_.eye.y, false, now, 70, 0); out = "ok"; return true; }
  if (starts("zombies ")) {
    const std::string a = cmd.substr(8);
    if (a == "force") C.zombie_force = true;
    else if (a == "day") C.zombie_force = false;
    else if (a == "off" || a == "on") C.zombies_enabled = a == "on";
    out = C.zombies_enabled ? (C.zombie_force ? "force" : "on") : "off";
    return true;
  }
  if (cmd == "race") {
    C.race_on = !C.race_on;
    C.race.reset();
    // the course around where the drone (or the pilot) is: the start gate 4.5 m behind
    const Vec3 at = mode_ == Mode::drone && drone_out_ ? matrix_of(drone_actor_).translation() : player_position();
    C.race_origin = {at.x + 4.5f, at.y, city_.ground_height(at.x, at.y)};
    combat_banner(C.race_on ? "RENNSTRECKE · 3 RUNDEN" : "RENNSTRECKE AUS");
    out = C.race_on ? "on" : "off";
    return true;
  }
  if (cmd == "race fly") {
    // Tests: the drone's path through the next gate (two points either side)
    if (!C.race_on) { out = "off"; return true; }
    Vec2 n, r;
    const i32 g = C.race.next;
    gate_axes(g, n, r);
    const Vec3 c = gate(g).center;
    C.race.update(c - Vec3{n.x, n.y, 0} * 0.5f, now);
    out = std::to_string(C.race.update(c + Vec3{n.x, n.y, 0} * 0.5f, now + 0.25f)) + " lap " + std::to_string(C.race.lap);
    C.race.last_pass = now - 1;
    return true;
  }
  if (cmd == "damaged") { out = std::to_string(city_.damaged_buildings()); return true; }  // tests: buildings broken so far
  if (cmd == "rounds") {
    // tests: rounds in flight, launcher bounces so far
    char b[64];
    std::snprintf(b, sizeof b, "%d in flight, %u bounces", static_cast<int>(projectiles_.size()), bounce_fx_next_);
    out = b;
    return true;
  }
  if (cmd == "melee probe") {
    // tests: one knife blow straight through the view, now
    char b[96];
    const bool hit = melee_hit(1.9f, 1.2f, 45, true, 12);
    Vec3 dir;
    const Vec3 eye = aim_ray(dir);
    std::snprintf(b, sizeof b, "%s eye %.1f %.1f %.1f dir %.2f %.2f %.2f", hit ? "hit" : "miss", eye.x, eye.y, eye.z, dir.x, dir.y, dir.z);
    out = b;
    return true;
  }
  if (cmd == "enemy hp") {
    // tests: the last enemy's health
    if (C.enemies.empty() || C.enemies.back().person < 0) { out = "none"; return true; }
    const People::Person& q = people_.at(C.enemies.back().person);
    char b[64];
    std::snprintf(b, sizeof b, "%d hits at %.1f m", static_cast<int>(C.enemies.back().hits), dist2d(q.pos, player_position()));
    out = b;
    return true;
  }
  if (starts("enemy ")) {
    std::string t = cmd.substr(6);
    // "enemy near": tests - a cop at arm's length (1.3 m) in front, for the fists and the knife
    const bool near = t == "near";
    if (near) {
      t = "cop";
      for (EnemyAi& o : C.enemies)
        if (o.hold && o.person >= 0) { people_.at(o.person).state = People::State::gone; o.person = -1; }
    }
    const EnemyType type = t == "heavy" ? EnemyType::heavy : t == "swat" ? EnemyType::swat : EnemyType::cop;
    const Vec3 me = player_position();
    const Vec3 fwd = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
    const f32 dist = near ? 1.3f : 20.0f;
    const Vec3 p{me.x + fwd.x * dist, me.y + fwd.y * dist, near ? me.z : 0};
    const EnemySpec& s = enemy_spec(type);
    EnemyAi e;
    e.type = type;
    e.person = people_.add_scripted(People::Kind::enemy, p, 0, 1e4f, rng_.next(), s.color, {0.08f, 0.07f, 0.12f}, {0.88f, 0.7f, 0.58f}, static_cast<u8>(type));
    e.last_hp = 1e4f;
    e.next_shot = now + 1;
    e.hold = near;
    C.enemies.push_back(e);
    out = s.name;
    return true;
  }
  if (cmd == "provoke") {
    // Tests: the nearest citizen squares up (as if aggressive and punched)
    const Vec3 me = matrix_of(soldier_).translation();
    P* best = nullptr;
    f32 bd = 1e9f;
    for (std::size_t pi = 0; pi < people_.all().size(); ++pi)
      if (P& p = people_.at(static_cast<i32>(pi)); p.kind == People::Kind::citizen && alive(p) && dist2d(p.pos, me) < bd) { bd = dist2d(p.pos, me); best = &p; }
    if (!best) { out = "nobody"; return true; }
    best->state = People::State::fight;
    best->fight_until = now + 12;
    const Arms a = arms_of(best->seed);
    out = std::string(a == Arms::gun ? "gun" : a == Arms::knife ? "knife" : "fists") + " " + std::to_string(static_cast<int>(bd));
    return true;
  }
  if (cmd == "contract") { out = C.fun.apply(FunEvent::cycle_contract, now, wanted_.stars()); return true; }
  if (cmd == "combat") { std::string s = "{\"x\":0"; combat_hud(s); out = s + "}"; return true; }
  return false;
}

}  // namespace sc
