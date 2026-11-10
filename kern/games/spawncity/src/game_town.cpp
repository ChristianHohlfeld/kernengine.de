// SpawnCity on Kern: the town around the player - people, police, wanted level, damage and
// explosions (SpawnCity world_procedural_population.mjs, pedestrian_agents.mjs,
// police_ground_units.mjs, wanted_system_logic.mjs, critical_damage_logic.mjs).
#include <algorithm>
#include <cmath>

#include "game.h"
#include "mp_rules.h"
#include "weapons.h"

namespace sc {

using kern::game::ActorId;

namespace {
// police_ground_units.mjs
constexpr f32 kStopFoot = 17, kEngageMin = 6, kEngageMax = 15, kReturnM = 42, kRunMps = 3.7f;
constexpr f32 kSpawnMin = 70, kSpawnMax = 125, kDespawn = 230, kPathSeconds = 1.8f, kSightSeconds = 0.3f;
constexpr f32 kVolleyGap = 0.48f, kCopHitScale = 0.5f;
constexpr f32 kCarHp = 300, kCarHit = 22, kCarCritical = 0.20f, kCarBurn = 2.45f /* critical_damage_logic.mjs car: 20 %, 2450 ms */, kCrashDv = 7, kCrashDamage = 16;
constexpr f32 kDroneHp = 100, kDroneHit = 25, kJetHit = 22;
// wanted_police_drones.mjs
constexpr f32 kPdHp = 100, kPdHit = 34, kPdRespawn = 9, kPdSensor = 0.17f, kPdReaction = 0.75f, kPdVolley = 0.65f, kPdDamageCooldown = 1.05f;
constexpr f32 kPdFormation[5] = {0, 0.08f, -0.08f, 0.15f, -0.15f};

f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 approach_angle(f32 cur, f32 target, f32 max_step) { return cur + clampf(wrap_pi(target - cur), -max_step, max_step); }
}  // namespace

bool Game::line_of_sight(Vec3 a, Vec3 b) const {
  const Vec3 d = b - a;
  const f32 l = kern::length(d);
  if (l < 0.05f) return true;
  // The player's own body (soldier capsule or vehicle) is what is looked at, not in the way.
  const ActorId me = mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::jet ? jet_actor_ : mode_ == Mode::drone ? drone_actor_
                     : mode_ == Mode::parachute ? chute_actor_ : soldier_;
  return !cast(world_->physics(), a, d * (1.0f / l), l, body(me), kCatWorld).hit;
}

PeopleView Game::people_view() const {
  PeopleView v;
  v.physics = &world_->physics();
  v.city = &city_;
  v.focus = focus_position();
  if (net_.hosting()) v.foci = mate_foci();  // MP: the citizens live around every player (game_worldsync.cpp)
  v.player = player_position();
  if (mode_ == Mode::foot) v.player.z -= 0.73f;
  v.player_on_foot = mode_ == Mode::foot;
  v.player_alive = !player_dead_;
  v.can_spawn = [this](Vec3 p) { return gaps_can_spawn(p, 1.9f, mobile_ ? 125.0f : 150.0f); };  // spawn_visibility_guard.mjs (game_gaps.cpp)
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    if (!c.placed || !world_->actor_alive(c.actor)) continue;
    const BodyState s = state_of(c.actor);
    VehicleView vv;
    vv.position = s.position;
    vv.velocity = s.velocity;
    vv.rotation = s.rotation;
    vv.half = {c.car->spec().half.x, c.car->spec().half.y};
    vv.player = mode_ == Mode::car && static_cast<i32>(i) == car_index_;
    v.vehicles.push_back(vv);
  }
  if (drone_out_) {
    const BodyState s = state_of(drone_actor_);
    v.vehicles.push_back({s.position - Vec3{0, 0, city_.ground_height(s.position.x, s.position.y)}, s.velocity, s.rotation, {0.3f, 0.3f}, true, true});
  }
  if (jet_placed_) {
    const BodyState s = state_of(jet_actor_);
    const bool flying = jet_.telemetry().contacts == 0;
    // Body frame of the jet: +Y nose; footprint across X (wings) and Y (length).
    v.vehicles.push_back({s.position, s.velocity, s.rotation * Quat::axis_angle({0, 0, 1}, kPi / 2), {7.3f, 5.0f}, mode_ == Mode::jet, flying});
  }
  return v;
}

// ---------------------------------------------------------------------------------------------
// shots: the player's bullets against people and vehicles

void Game::handle_shots() {
  const f32 now = world_->sim_time();
  std::vector<kern::game::ShotEvent>& shots = world_->shots();
  // gameplay_final_runtime_v2.mjs sniperShotAt atArrival: what the .50 round does at the far end lands
  // when the bullet gets there (distance / SNIPER_TRACER_MPS 850 m/s); the rest acts at once
  std::vector<kern::game::ShotEvent*> work;
  std::vector<kern::game::ShotEvent> arrived;
  gaps_sniper_rounds(shots, arrived, now);
  for (kern::game::ShotEvent& s : shots)
    if (s.shooter == soldier_ && !(mode_ == Mode::foot && foot_weapon_ == weapons::Id::sniper)) work.push_back(&s);
  for (kern::game::ShotEvent& s : arrived) work.push_back(&s);
  for (kern::game::ShotEvent* sp : work) {
    kern::game::ShotEvent& s = *sp;
    // Shots are heard: people close by run (pedestrian_agents.mjs: 10 m all, to 22 m some).
    people_.frighten(s.from, 22, now, 1);
    Vec3 to = s.to;
    // Police drones: a sphere of 0.55 m around each.
    bool drone_hit = false;
    for (PoliceDrone& d : pdrones_) {
      if (!d.active) continue;
      const Vec3 seg = s.to - s.from;
      const f32 l2 = kern::dot(seg, seg);
      if (l2 < 1e-6f) continue;
      const f32 t = clampf(kern::dot(d.pos - s.from, seg) / l2, 0, 1);
      if (kern::length(s.from + seg * t - d.pos) < 0.55f) {
        s.to = s.from + seg * t;
        s.surface = 2;
        d.hp -= kPdHit;
        if (d.hp <= 0) {
          osfx("wanted_police_drones.down", d.pos);
          explosion(d.pos, 3, 40, true);
          wanted_.report(wanted::Crime::police_drone, true);
          d.active = false;
          d.destroyed_until = now + kPdRespawn;
        }
        drone_hit = true;
        break;
      }
    }
    if (drone_hit) continue;
    if (combat_shot(s)) continue;  // helicopter, tank, flak (game_military.cpp)
    const f32 damage = static_cast<f32>(world_->level().find(soldier_) && world_->level().find(soldier_)->weapon ? world_->level().find(soldier_)->weapon->damage : 30);
    i32 person = -1;
    if (hit_remotes(s.from, to, damage) || (person = people_.shot(s.from, to, damage, true, now)) >= 0 || life_shot(s.from, to, damage)) {  // life_shot: cows, birds, joggers (M3 life)
      if (person >= 0) swarm_person_hit(person, damage);  // a mate's zombie or soldier: the hit goes to him
      if (person >= 0) osfx("gameplay_polish_lite.playPersonGroan", to);  // a person hit (registerWorldPopulationHit)
      // Glock and sniper go through bodies: up to three, 0.6 m on past each, walls stop them
      // (glockShotAt / sniperShotAt pierced < 3); the end point stays where the round really ends
      if (damage >= 26) {
        const Vec3 dir = kern::normalize(s.to - s.from, {1, 0, 0});
        Vec3 from = to + dir * 0.6f;
        for (int pierced = 1; pierced < 3 && kern::dot(s.to - from, dir) > 0.5f; ++pierced) {
          Vec3 next = s.to;
          if (!(hit_remotes(from, next, damage) || people_.shot(from, next, damage, true, now) >= 0 || life_shot(from, next, damage))) break;
          from = next + dir * 0.6f;
        }
      }
      s.to = to;
      s.surface = 3;  // flesh: no decal
      s.impact = true;
      continue;
    }
    if (s.impact && s.surface != 2) world_bullet(s.to);  // the world: a window pane may crack
    if (s.surface == 2 && s.impact) {
      // A moving body: which car was it?
      for (std::size_t i = 0; i < cars_.size(); ++i) {
        const TrafficCar& c = cars_[i];
        if (!c.placed || c.wreck) continue;
        const BodyState b = state_of(c.actor);
        const Vec3 local = to_local(b.rotation, s.to - b.position);
        const Vec3 h = c.car->spec().half;
        if (std::fabs(local.x) < h.x + 0.4f && std::fabs(local.y) < h.y + 0.4f && std::fabs(local.z) < 2.0f) {
          car_bullet(static_cast<i32>(i), true);  // per kind (game_vehicle_damage.cpp)
          break;
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------------------------
// damage and explosions

void Game::damage_car(i32 i, f32 amount, bool by_player) {
  if (i < 0 || i >= static_cast<i32>(cars_.size())) return;
  TrafficCar& c = cars_[i];
  if (c.wreck || !(amount > 0)) return;
  const f32 now = world_->sim_time();
  // a cruiser the player drives takes nothing (police_ground_units.mjs damageCar: playerDriven)
  if (c.police && mode_ == Mode::car && i == car_index_) return;
  c.hp = std::fmax(0.0f, c.hp - amount);
  const bool bus = c.car->spec().radius > 0.4f;
  // critical: a cruiser at 25 % burns 2.6 s, at 0 at most 0.5 s more (police_ground_units.mjs);
  // a car / bus at 20 % 2.45 / 3.1 s, and every further hit cuts it to 0.62 / 0.72 s
  // (critical_damage_logic.mjs PROFILES, walk_world_experience_hotfix.mjs armCriticalDamage)
  if (c.hp <= car_max_hp(c) * (c.police ? 0.25f : kCarCritical)) {
    if (c.burn_until < 0) c.burn_until = now + (c.police ? 2.6f : bus ? 3.1f : kCarBurn);
    else if (!c.police || c.hp <= 0) c.burn_until = std::fmin(c.burn_until, now + (c.police ? 0.5f : bus ? 0.72f : 0.62f));
  }
  if (by_player) c.hit_by_player = true;
}

void Game::wreck_car(i32 i) {
  TrafficCar& c = cars_[i];
  if (c.wreck) return;
  c.wreck = true;
  c.hp = 0;
  c.driver = false;
  c.burn_until = -1;
  c.wreck_at = world_->sim_time();
  const Vec3 at = matrix_of(c.actor).translation() + Vec3{0, 0, 0.6f};
  // Whoever sits inside does not survive it.
  if (c.unit >= 0 && c.unit < static_cast<i32>(units_.size())) {
    PoliceUnit& u = units_[c.unit];
    for (PoliceUnit::Cop& cop : u.cops)
      if (cop.inside) { cop.inside = false; cop.seed = 0; }
    u.state = PoliceUnit::State::wreck;
  }
  const bool mine = mode_ == Mode::car && i == car_index_;
  if (mine) {
    exit_vehicle();
    world_->damage(soldier_, 80, 0, false, at);
  }
  // A burnt-out car (critical_damage_logic.mjs): explodes, jumps and stays.
  const bool by_player = c.hit_by_player;
  // a cruiser 7 m / 110 and 1450 x 5.5 up (police_ground_units.mjs wreckCar); a car or bus goes off with
  // the default blast 8 m / 100 (world_car_explosion.mjs -> dispatchExplosion)
  if (c.police) explosion(at, 7, 110, by_player);
  else explosion(at, 8, 100, by_player);
  world_->push_actor(c.actor, {0, 0, 1450 * (c.police ? 5.5f : 3.5f)}, true);
  if (by_player && c.police) wanted_.report(wanted::Crime::police_car, true);
  else if (by_player) wanted_.report(c.car->spec().radius > 0.4f ? wanted::Crime::bus : wanted::Crime::car, people_.witnessed(at));
}

void Game::damage_player_vehicle(f32 amount) {
  const f32 now = world_->sim_time();
  (void)now;
  if (mode_ == Mode::car && car_index_ >= 0) damage_car(car_index_, amount, false);
  else if (mode_ == Mode::drone && drone_out_) {
    drone_hp_ -= amount;
    if (drone_hp_ <= 0) {
      const Vec3 at = matrix_of(drone_actor_).translation();
      world_->particle_burst(at, {2.0f, 0.8f, 0.2f});
      world_->play_sound("builtin:break", at, 1, 1.2f, true);
      toast_ = "Drohne abgeschossen!";
      toast_age_ = 0;
      toggle_drone();
      drone_hp_ = kDroneHp;
      combat_.drone_ready_at = world_->sim_time() + 8;  // player_vitals_logic.mjs DRONE_REPLACEMENT_COOLDOWN_MS
      combat_.pop = combat_.fun.apply(combat::FunEvent::drone_destroyed, world_->sim_time(), wanted_.stars());  // gameplay_fun_logic.mjs
      combat_.pop_at = world_->sim_time();
    }
  } else if (mode_ == Mode::jet) {
    gaps_jet_damage(amount);  // jet_mode.mjs damageAirframe: 400 HP, thrust loss, burning, break-up (game_gaps.cpp)
  }
}

void Game::explosion(Vec3 at, f32 radius, f32 damage, bool by_player, BlastBy kind) {
  const f32 now = world_->sim_time();
  ++explosions_;
  look_blast(at, radius);  // light, camera shake (game_look.cpp)
  // dispatchExplosion -> visualBlast(position, 1): a pool of 8 in the original
  if (visual_blasts_.size() >= 8) visual_blasts_.erase(visual_blasts_.begin());
  visual_blasts_.push_back({at, now, 1});
  world_->physics().explode(at, radius, radius * 0.5f, 6.0f);
  world_->particle_burst(at, {3.0f, 1.2f, 0.3f});
  world_->particle_burst(at + Vec3{0, 0, 0.8f}, {1.1f, 1.1f, 1.1f});
  world_->play_sound(combat_sfx(combat_audio::Kind::explosion), at, 1, 1, true);
  osfx("player_vehicle_runtime_v2.playExplosion", at);  // "arondight:world-explosion"
  const f32 cam_d = kern::length(cam_.eye - at);
  world_->shake_camera(clampf(1.4f - cam_d / (radius * 3), 0, 1.2f));
  people_.blast(at, radius, damage, by_player, now);
  combat_blast(at, radius, damage, by_player);  // army, flak, rockets (game_military.cpp)
  if (by_player) blast_remotes(at, radius, damage, kind == BlastBy::missile ? 2 : kind == BlastBy::weapon ? 1 : 0);
  // The player on foot - and the pilot standing below his drone (weapons.h blast_factor: buildings
  // in between shield most of it).
  if (mode_ == Mode::foot || (mode_ == Mode::drone && !player_dead_)) {
    const Vec3 me = matrix_of(soldier_).translation();
    const f32 d = kern::length(me - at);
    // dispatchExplosion: only an amount above 2 counts (gameplay_final_runtime_v2.mjs)
    // dispatchExplosion clamps the blast to 2..18 m and 10..160 damage
    const f32 rr = clampf(radius, 2, 18), dmg = clampf(damage, 10, 160);
    // (the line of sight past his own capsule - in the drone line_of_sight leaves out the drone)
    const Vec3 dl = me - at;
    const f32 dlen = kern::length(dl);
    const bool shielded = dlen > 0.05f && cast(world_->physics(), at, dl * (1.0f / dlen), dlen, body(soldier_), kCatWorld).hit;
    const f32 amount = d < rr ? dmg * weapons::blast_factor(d, rr, shielded) : 0;
    if (amount > 2) world_->damage(soldier_, amount, 0, false, at);
  }
  blast_cars(at, radius, damage, by_player);  // hits per car / bus, cruisers linear (game_vehicle_damage.cpp)
  // the parked jet (player_vehicle_runtime_v2 airframes): grenades and rockets wreck it too, linear
  // over the radius + 5 m (its size); in the cockpit the flight's own damage applies
  if (jet_placed_ && mode_ != Mode::jet && gaps_) {
    const f32 dj = kern::length(matrix_of(jet_actor_).translation() - at), rj = radius + 5;
    if (dj < rj) gaps_jet_damage(std::fmax(20.0f, damage) * 2.5f * (1 - dj / rj));
  }
  world_blast(at, radius, damage, by_player);  // M3: craters, scorch, facade marks, animals, ragdolls
  if (kind != BlastBy::other) destruction_.blast(at, radius, damage, kind == BlastBy::missile, city_, rng_);  // weapons break buildings
  life_blast(at, radius);  // M3 life: cows, pigeons, joggers, ragdolls (game_life.cpp)
  gaps_lamps_blast(at, radius);  // street_lamps.mjs onExplosion (game_lamps.cpp)
  blast_police_drones(at, radius, damage, by_player);  // every blast reaches them (game_foot_rounds.cpp)
}

// ---------------------------------------------------------------------------------------------
// police

void Game::spawn_police_unit() {
  // A free cruiser of the three in the level.
  i32 car = -1;
  for (std::size_t i = 0; i < cars_.size(); ++i)
    if (cars_[i].police && cars_[i].unit < 0 && !cars_[i].placed) { car = static_cast<i32>(i); break; }
  if (car < 0) return;
  const Vec3 me = player_position();
  Vec3 p;
  f32 heading = 0;
  bool found = false;
  for (int attempt = 0; attempt < 12 && !found; ++attempt) {
    if (!city_.street_point(rng_, me, kSpawnMax, p, heading)) break;
    const f32 d = dist2d(p, me);
    if (d < kSpawnMin) continue;
    if (cast(world_->physics(), p + Vec3{0, 0, 3.5f}, {0, 0, -1}, 3.0f, {}).hit) continue;  // something parked there
    found = true;
  }
  if (!found) return;
  TrafficCar& c = cars_[car];
  // Facing the player: the cruiser turns towards the chase from the start.
  const f32 to_player = std::atan2(me.y - p.y, me.x - p.x);
  if (std::fabs(wrap_pi(to_player - heading)) > kPi / 2) heading = wrap_pi(heading + kPi);
  const Vec3 pos{p.x, p.y, city_.ground_height(p.x, p.y) + Car::ground_offset(c.car->spec()) + 0.05f};
  if (!world_->actor_alive(c.actor)) world_->set_alive(c.actor, true);
  const Vec3 rot{0, 0, heading / kDeg};
  world_->teleport(c.actor, pos, &rot);
  world_->set_actor_velocity(c.actor, {});
  c.car->reset();
  c.placed = true;
  c.driver = true;
  c.hp = car_max_hp(c);
  c.wreck = false;
  c.hit_by_player = false;
  c.burn_until = -1;
  c.lane = -1;
  PoliceUnit u;
  u.car = car;
  u.state = PoliceUnit::State::pursuit;
  for (int k = 0; k < 2; ++k) {
    u.cops[k].seed = rng_.next() | 1u;
    u.cops[k].inside = true;
    u.cops[k].seat = k ? -1 : 1;
  }
  u.progress_at = pos;
  u.progress_t = world_->sim_time();
  // Reuse a finished unit's slot so car.unit indices stay valid.
  i32 slot = -1;
  for (std::size_t i = 0; i < units_.size(); ++i)
    if (units_[i].car < 0) { slot = static_cast<i32>(i); break; }
  if (slot < 0) { units_.push_back(u); slot = static_cast<i32>(units_.size() - 1); }
  else units_[slot] = u;
  c.unit = slot;
  last_wave_ = world_->sim_time();
}

void Game::step_cop(PoliceUnit& u, PoliceUnit::Cop& c, f32 dt) {
  if (c.person < 0 || c.inside) return;
  People::Person& p = people_.at(c.person);
  if (p.seed != c.seed || p.state == People::State::gone) { c.person = -1; c.seed = 0; return; }
  if (p.state == People::State::down) { c.person = -1; c.seed = 0; return; }
  const f32 now = world_->sim_time();
  const i32 stars = std::max(1, wanted_.stars());
  Vec3 target = player_position();
  if (mode_ == Mode::foot) target.z -= 0.73f;
  const f32 dx = target.x - p.pos.x, dy = target.y - p.pos.y, d = std::hypot(dx, dy);
  const Vec3 eye = p.pos + Vec3{0, 0, 1.45f};
  const Vec3 aim = target + Vec3{0, 0, mode_ == Mode::foot ? 1.33f : 0.8f};
  if (now - c.los_at > kSightSeconds) {
    c.los_at = now;
    const bool was = c.los;
    c.los = !player_dead_ && d < 60 && line_of_sight(eye, aim);
    if (c.los && !was) c.next_shot = std::fmax(c.next_shot, now + 1.1f + rng_.unit() * 0.7f);
  }
  const bool back = u.state == PoliceUnit::State::back || u.state == PoliceUnit::State::leave;
  Vec2 move{};
  bool moving = false;
  if (back) {
    const Vec3 car = matrix_of(cars_[u.car].actor).translation();
    const f32 dd = dist2d(car, p.pos);
    if (dd < 1.9f) { c.inside = true; p.state = People::State::gone; c.person = -1; return; }
    move = {(car.x - p.pos.x) / dd, (car.y - p.pos.y) / dd};
    moving = true;
  } else if (!c.los || d > kEngageMax) {
    if (d > 0.1f) { move = {dx / d, dy / d}; moving = true; }
  } else if (d < kEngageMin) {
    if (d > 0.1f) { move = {-dx / d * 0.6f, -dy / d * 0.6f}; moving = true; }
  }
  if (moving) {
    const f32 sp = back ? kRunMps : (d > kEngageMax + 10 ? kRunMps : 2.4f) * std::hypot(move.x, move.y);
    const bool ok = people_.free_move(world_->physics(), p, move, sp * dt, false);
    p.v = ok ? sp : 0;
    const f32 face = c.los && !back && d < kEngageMax + 8 ? std::atan2(dy, dx) : std::atan2(move.y, move.x);
    p.yaw = approach_angle(p.yaw, face, dt * 10);
  } else {
    p.v = 0;
    p.yaw = approach_angle(p.yaw, std::atan2(dy, dx), dt * 12);
  }
  p.phase += p.v * dt * 3.4f;
  p.aiming = !back && c.los && d < kEngageMax + 12;
  // Shooting: in range and in sight; officers keep a gap between their shots (a volley gap).
  if (back || !c.los || d >= kEngageMax + 12 || player_dead_) return;
  if (now >= c.next_shot && now - last_cop_shot_ < kVolleyGap) c.next_shot = last_cop_shot_ + kVolleyGap + rng_.unit() * 0.26f;
  if (now < c.next_shot || std::fabs(wrap_pi(p.yaw - std::atan2(dy, dx))) > 0.35f) return;
  c.next_shot = now + std::fmax(2.3f, 3.5f - static_cast<f32>(stars) * 0.16f) + rng_.unit() * 1.5f;
  last_cop_shot_ = now;
  const f32 cy = std::cos(p.yaw), sy = std::sin(p.yaw);
  const Vec3 from{p.pos.x + cy * 0.45f + sy * 0.12f, p.pos.y + sy * 0.45f - cy * 0.12f, p.pos.z + 1.38f};
  const f32 player_speed = kern::length(world_->actor_velocity(mode_ == Mode::foot ? soldier_ : mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor
                                                                                       : mode_ == Mode::jet ? jet_actor_
                                                                                       : mode_ == Mode::drone ? drone_actor_
                                                                                                              : soldier_));
  const bool hit = rng_.unit() < wanted::hit_chance(stars, d, player_speed) * kCopHitScale;
  kern::game::ShotEvent s;
  s.from = from;
  s.shooter = 0;
  if (hit) {
    s.to = aim;
    s.impact = true;
    s.surface = 3;
    if (mode_ == Mode::foot) world_->damage(soldier_, std::max(2.0f, std::round(wanted::police_damage(stars) * 0.4f)), 0, false, from);
    else if (mode_ == Mode::drone) damage_player_vehicle(kDroneHit);
    else if (mode_ == Mode::jet) damage_player_vehicle(kJetHit);
    else damage_player_vehicle(kCarHit);
  } else {
    Vec3 miss = aim + Vec3{(rng_.unit() - 0.5f) * 2.4f, (rng_.unit() - 0.5f) * 2.4f, (rng_.unit() - 0.6f) * 1.4f};
    const Vec3 dir = kern::normalize(miss - from);
    const RayResult r = cast(world_->physics(), from, dir, d + 12, {});
    s.to = r.hit ? r.point : from + dir * (d + 6);
    s.impact = r.hit;
    s.normal = r.normal;
    s.surface = r.hit ? 1 : 0;
  }
  world_->shots().push_back(s);
  world_->play_sound(combat_sfx(combat_audio::Kind::pistol), from, 0.85f, 1.0f, true);
  osfx("police_ground_units.playShot", from);
}

void Game::drive_police(PoliceUnit& u, f32 dt) {
  (void)dt;
  TrafficCar& c = cars_[u.car];
  const f32 now = world_->sim_time();
  const BodyState s = state_of(c.actor);
  const Vec3 pos = s.position, fwd = s.rotation.rotate({1, 0, 0});
  const f32 vf = c.car->forward_speed(), sp = std::hypot(s.velocity.x, s.velocity.y);
  Vec3 me = player_position();
  const f32 d = dist2d(me, pos);
  const f32 player_speed = kern::length(world_->actor_velocity(mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::foot ? soldier_ : jet_actor_));
  const bool seen = !player_dead_ && d < 45 && line_of_sight(pos + Vec3{0, 0, 1.3f}, me + Vec3{0, 0, 0.6f});
  auto brake = [&]() {
    CarInput in;
    in.handbrake = true;
    in.pedal = std::fabs(vf) > 0.5f ? -sign0(vf) : 0;
    c.input = in;
  };
  auto exit_car = [&]() {
    const f32 yaw = std::atan2(fwd.y, fwd.x);
    const Vec3 left{-fwd.y, fwd.x, 0};
    for (PoliceUnit::Cop& cop : u.cops) {
      if (!cop.inside || !cop.seed) continue;
      Vec3 at = pos + fwd * 0.25f + left * (1.35f * static_cast<f32>(cop.seat));
      at.z = city_.ground_height(at.x, at.y);
      if (!line_of_sight(pos + Vec3{0, 0, 1.0f}, at + Vec3{0, 0, 1.0f})) at = pos - fwd * 2.6f;
      at.z = city_.ground_height(at.x, at.y);
      cop.person = people_.add_officer(at, yaw + static_cast<f32>(cop.seat) * kPi / 2, static_cast<i32>(&u - units_.data()), cop.seed);
      cop.inside = false;
      cop.next_shot = now + 0.7f + rng_.unit() * 0.5f;
      cop.los = false;
      cop.los_at = -1;
    }
    u.state = PoliceUnit::State::engage;
  };
  switch (u.state) {
    case PoliceUnit::State::pursuit: {
      const Vec2 pos2{pos.x, pos.y};
      const f32 herr = wrap_pi(std::atan2(u.last_target.y - pos.y, u.last_target.x - pos.x) - std::atan2(fwd.y, fwd.x));
      if (u.recover_until > now) {
        if (std::fabs(herr) < 0.45f && now - u.recover_start > 0.5f) u.recover_until = -1;
        else {
          CarInput in;
          in.pedal = -0.8f;
          in.steer = -sign0(herr == 0 ? 1 : herr);
          in.max_reverse = 5;
          c.input = in;
          return;
        }
      }
      if (sp < 1.2f && std::fabs(herr) > 1.05f && d > kStopFoot) {
        if (u.turn_since < 0) u.turn_since = now;
        if (now - u.turn_since > 0.7f) { u.recover_start = now; u.recover_until = now + 2.2f; u.turn_since = -1; return; }
      } else u.turn_since = -1;
      if (sp < 0.7f && d > kStopFoot) {
        if (u.stuck_since < 0) u.stuck_since = now;
        if (now - u.stuck_since > 2) { u.recover_start = now; u.recover_until = now + 1.6f; u.stuck_since = -1; u.path.clear(); return; }
      } else u.stuck_since = -1;
      if (now - u.progress_t > 4) {
        if (d > kStopFoot && dist2d(pos, u.progress_at) < 6) { u.recover_start = now; u.recover_until = now + 1.6f; u.path.clear(); }
        u.progress_at = pos;
        u.progress_t = now;
      }
      if (!player_driving() && d < kStopFoot && seen) { u.state = PoliceUnit::State::stopping; brake(); return; }
      if (player_driving() && d < 9 && sp < 1.5f && seen && player_speed < 2) { u.state = PoliceUnit::State::stopping; brake(); return; }
      Vec2 target{me.x, me.y};
      if (!seen || d > 40) {
        if (u.path.empty() || now - u.path_at > kPathSeconds) {
          if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
          roads_.path(pos2, target, u.path);
          u.path_at = now;
        }
        if (u.path.size() >= 2) {
          target = RoadGraph::pursuit_point(u.path, pos2, std::fmax(8.0f, sp * 1.1f));
          const Vec2 far = RoadGraph::pursuit_point(u.path, pos2, std::fmax(22.0f, sp * 2.2f));
          const f32 h1 = std::atan2(target.y - pos.y, target.x - pos.x), h2 = std::atan2(far.y - target.y, far.x - target.x);
          u.corner_speed = clampf(17 - std::fabs(wrap_pi(h2 - h1)) / (kPi / 2) * 12, 5, 17);
        }
      } else {
        u.corner_speed = 17;
      }
      u.last_target = target;
      CarInput in = ai_drive(pos, fwd, vf, {target.x, target.y, 0}, std::fmin(u.corner_speed, d > 70 ? 17.0f : d > 30 ? 13.0f : 8.0f));
      in.max_speed = 26;
      c.input = in;
      return;
    }
    case PoliceUnit::State::stopping:
      brake();
      if (sp < 1.2f) {
        if (player_driving() && d > 12) u.state = PoliceUnit::State::pursuit;
        else exit_car();
      }
      return;
    case PoliceUnit::State::engage: {
      brake();
      bool far = true;
      for (const PoliceUnit::Cop& cop : u.cops)
        if (cop.person >= 0 && dist2d(people_.at(cop.person).pos, me) <= kReturnM) far = false;
      if ((player_driving() && (player_speed > 4 || d > 20)) || far) u.state = PoliceUnit::State::back;
      return;
    }
    case PoliceUnit::State::back: {
      brake();
      bool all_in = true;
      for (const PoliceUnit::Cop& cop : u.cops)
        if (cop.seed && !cop.inside) all_in = false;
      if (all_in) u.state = wanted_.stars() > 0 ? PoliceUnit::State::pursuit : PoliceUnit::State::leave;
      return;
    }
    case PoliceUnit::State::leave: {
      if (u.left_at < 0) u.left_at = now;
      const f32 a = std::atan2(pos.y - me.y, pos.x - me.x);
      CarInput in = ai_drive(pos, fwd, vf, pos + Vec3{std::cos(a), std::sin(a), 0} * 60.0f, 12);
      c.input = in;
      return;
    }
    case PoliceUnit::State::abandoned:
    case PoliceUnit::State::wreck: brake(); return;
  }
}

void Game::spawn_police_drones() {
  const f32 now = world_->sim_time();
  const i32 stars = wanted_.stars();
  const i32 desired = wanted::police_count(stars);
  i32 active = 0;
  for (const PoliceDrone& d : pdrones_) active += d.active ? 1 : 0;
  // A wave at once; when a wave is down, the next after a break (syncPoliceWaves).
  if (active > 0) { drone_wave_live_ = true; if (active >= desired) return; }
  else if (drone_wave_live_) { drone_wave_live_ = false; next_drone_wave_ = now + wanted::wave_break_ms(stars) / 1000; }
  if (next_drone_wave_ >= 0 && now < next_drone_wave_) return;
  // They come in from behind the player's view, at his height (policeSpawnSpot).
  const Vec3 anchor = last_contact_ > now - wanted::escape_ms(stars) / 1000 ? last_known_ : player_position();
  const f32 forward = std::atan2(cam_.target.y - cam_.eye.y, cam_.target.x - cam_.eye.x);
  for (u32 i = 0; i < 5 && active < desired; ++i) {
    PoliceDrone& d = pdrones_[i];
    if (d.active || now < d.destroyed_until) continue;
    if (!gaps_drone_due(i, stars)) { ++active; continue; }  // wanted_dispatch_guard.mjs: on its way (game_gaps.cpp)
    const f32 base = forward + kPi + (static_cast<f32>(i) - 2) * 0.34f;
    bool placed = false;
    for (int k = 0; k < 12 && !placed; ++k) {
      const f32 a = base + (k % 2 ? 1.0f : -1.0f) * std::ceil(static_cast<f32>(k) / 2) * 0.38f;
      const f32 r = wanted::spawn_radius(static_cast<i32>(i)) + static_cast<f32>(k >> 2) * 6;
      Vec3 p{anchor.x + std::cos(a) * r, anchor.y + std::sin(a) * r, 0};
      const RayResult roof = cast(world_->physics(), {p.x, p.y, 80}, {0, 0, -1}, 90, {}, kCatWorld);
      p.z = std::fmax(anchor.z + 0.18f + kPdFormation[i], (roof.hit ? roof.point.z : 0) + 2.5f);
      if (!line_of_sight(p, p + Vec3{0, 0, 0.5f})) continue;
      d = PoliceDrone{};
      d.active = true;
      d.pos = p;
      d.yaw = a + kPi;
      d.vel = {-std::sin(a) * 1.2f, std::cos(a) * 1.2f, 0};
      d.hp = kPdHp;
      d.next_shot = now + wanted::engage_delay_ms(stars) / 1000;
      d.serial = ++drone_serial_;
      gaps_drone_revealed(i, stars);  // arrived: the grace before the first shot (game_gaps.cpp)
      placed = true;
      ++active;
    }
  }
}

void Game::emp_pulse() {
  // EMP (wanted_police_drones.mjs): 30 m, 18 s cooldown; drones in range drop out of the sky.
  const f32 now = world_->sim_time();
  if (now < emp_ready_at_ || wanted_.stars() <= 0) { toast_ = now < emp_ready_at_ ? "EMP laedt ..." : "EMP: keine Ziele"; toast_age_ = 0; return; }
  const Vec3 me = player_position();
  {
    // wanted_police_drones.mjs triggerEmp: nothing in range = "NO TARGET IN RANGE" for 1.1 s, no cooldown
    i32 targets = 0;
    const f32 emp_range = wanted::emp_range();
    for (const PoliceDrone& d : pdrones_) targets += d.active && !d.emp && kern::length(d.pos - me) <= emp_range ? 1 : 0;
    if (targets == 0) { ctl_.emp_miss_until = now + 1.1f; return; }
  }
  emp_ready_at_ = now + wanted::emp_cooldown_ms() / 1000;
  u32 hit = 0;
  for (PoliceDrone& d : pdrones_) {
    if (!d.active || d.emp) continue;
    const f32 dist = kern::length(d.pos - me);
    const f32 impulse = wanted::emp_impulse(dist);
    if (impulse <= 0) continue;
    d.emp = true;
    d.sees = false;
    // triggerEmp: the kick is horizontal, away from the player, plus max(16, 0.16 J) N s downwards, on
    // the 18 kg body (massKg 18); its HP drop to 34 % at most
    Vec3 away{d.pos.x - me.x, d.pos.y - me.y, 0};
    const f32 al = std::hypot(away.x, away.y);
    if (al < 1e-2f) { const f32 a = static_cast<f32>(&d - pdrones_) / 5 * 2 * kPi; away = {std::cos(a), std::sin(a), 0}; }
    else away = away * (1.0f / al);
    d.vel += (away * impulse + Vec3{0, 0, -std::fmax(16.0f, impulse * 0.16f)}) * (1.0f / 18.0f);
    d.hp = std::fmin(d.hp, std::ceil(kPdHp * 0.34f));
    ++hit;
  }
  world_->particle_burst(me + Vec3{0, 0, 1.2f}, {0.4f, 0.8f, 2.4f});
  world_->play_sound("builtin:hazard", me, 0.9f, 1.4f, true);
  osfx("wanted_police_drones.emp", me);
  toast_ = hit ? "EMP! " + std::to_string(hit) + " Drohne(n) getroffen" : "EMP: keine Drohne in Reichweite";
  toast_age_ = 0;
}

bool Game::update_police_drones(f32 dt) {
  const f32 now = world_->sim_time();
  const i32 stars = wanted_.stars();
  if (stars > 0 && !player_dead_) spawn_police_drones();
  Vec3 me = player_position();
  if (mode_ == Mode::foot) me.z -= 0.73f;
  const Vec3 me_center = me + Vec3{0, 0, mode_ == Mode::foot ? 1.2f : 0.5f};
  const f32 radius = wanted::detection_radius(std::max(1, stars));
  bool sees = false;
  const bool searching = last_contact_ < now - 0.5f;
  for (u32 i = 0; i < 5; ++i) {
    PoliceDrone& d = pdrones_[i];
    if (!d.active) continue;
    if (d.emp) {
      // Knocked out: falls under 1.35 g (EMP_GRAVITY_SCALE), and breaks on the ground.
      d.vel.z -= kG * 1.35f * dt;
      d.vel = d.vel * std::exp(-0.3f * dt);
      const Vec3 step = d.vel * dt;
      const RayResult r = cast(world_->physics(), d.pos, kern::normalize(step, {0, 0, -1}), kern::length(step) + 0.2f, {}, kCatWorld);
      if (r.hit || d.pos.z < city_.ground_height(d.pos.x, d.pos.y) + 0.2f) {
        osfx("wanted_police_drones.down", d.pos);
        explosion(r.hit ? r.point : d.pos, 2.5f, 25, true);
        d.active = false;
        d.destroyed_until = now + 12;  // EMP_RESPAWN_MS 12000 (POLICE_RESPAWN_MS 9000 otherwise)
        continue;
      }
      d.pos += step;
      continue;
    }
    if (stars <= 0 || player_dead_) {
      // Called off: fly away and vanish.
      if (!d.retreating) { d.retreating = true; d.retreat_until = now + 3.6f + static_cast<f32>(i) * 0.14f; const Vec3 a = kern::normalize(d.pos - me, {1, 0, 0}); d.retreat_goal = d.pos + Vec3{a.x, a.y, 0} * 60.0f + Vec3{0, 0, 25}; }
      if (now >= d.retreat_until) { d.active = false; continue; }
    } else d.retreating = false;
    // Sensors.
    d.distance = kern::length(d.pos - me_center);
    if (now >= d.next_sensor) {
      d.next_sensor = now + kPdSensor + static_cast<f32>(i) * 0.007f;
      const bool had = d.sees;
      // wanted_police_pre_guard.mjs: a drone only sees (and so shoots) within its fire range
      d.sees = !player_dead_ && d.distance <= radius && d.distance <= wanted::fire_range(stars) && line_of_sight(d.pos, me_center);
      if (d.sees && !had) d.sight_since = now;
    }
    if (d.sees) { sees = true; last_known_ = me; last_contact_ = now; }
    // Motion: a ring around the player (or the last known position), at his height.
    Vec3 goal = d.retreating ? d.retreat_goal : Vec3{};
    if (!d.retreating) {
      const f32 ang = static_cast<f32>(i) / 5 * 2 * kPi + now * (searching ? 0.35f : 0.16f);
      const f32 r = searching ? 11 + static_cast<f32>(i % 3) * 5 : 8 + static_cast<f32>(i % 2) * 4;
      const Vec3 c = searching ? last_known_ : me;
      goal = {c.x + std::cos(ang) * r, c.y + std::sin(ang) * r, c.z + (searching ? 0.53f : 0.18f) + kPdFormation[i] + 1.2f};
      if (now >= d.roof_at) {
        d.roof_at = now + 0.125f + static_cast<f32>(i) * 0.009f;
        const RayResult roof = cast(world_->physics(), {goal.x, goal.y, 80}, {0, 0, -1}, 90, {}, kCatWorld);
        d.roof = roof.hit ? roof.point.z : 0;
      }
      goal.z = std::fmax(std::fmax(0.85f, goal.z), d.roof + 1.6f);
    }
    const Vec3 to = goal - d.pos;
    const f32 gd = kern::length(to);
    const f32 max_speed = (searching ? 6.4f : 8.4f + static_cast<f32>(stars) * 0.72f) + (gd > 45 ? 3.8f : 0);
    const f32 speed = std::fmin(max_speed, std::fmax(1.8f, gd * 0.72f));
    const Vec3 want = gd > 0.05f ? to * (speed / gd) : Vec3{};
    d.vel += (want - d.vel) * damp_alpha(2.2f, dt);
    Vec3 step = d.vel * dt;
    // Walls: climb over them instead of passing through.
    const f32 sl = kern::length(step);
    if (sl > 1e-4f && cast(world_->physics(), d.pos, step * (1.0f / sl), sl + 0.6f, {}, kCatWorld).hit) {
      step = {0, 0, std::fmax(3.0f, sl / dt) * dt};
      d.vel.z = std::fmax(d.vel.z, 3.0f);
    }
    d.pos += step;
    const f32 face = d.sees ? std::atan2(me.y - d.pos.y, me.x - d.pos.x) : std::atan2(d.vel.y, d.vel.x);
    if (kern::length(Vec3{d.vel.x, d.vel.y, 0}) > 0.3f || d.sees) d.yaw = approach_angle(d.yaw, face, dt * 4);
    // Attack (updateDroneAttack).
    if (d.retreating || searching || !d.sees || d.distance < 4.5f || d.distance > 34 + static_cast<f32>(stars) * 2 || now < d.next_shot) continue;
    if (now - d.sight_since < kPdReaction + static_cast<f32>(i) * 0.09f || now - last_drone_shot_ < kPdVolley) continue;
    if (!line_of_sight(d.pos, me_center)) { d.sees = false; d.next_shot = now + 0.19f; continue; }
    d.next_shot = now + std::max(3.9f, 5.1f - static_cast<f32>(stars) * 0.23f) + static_cast<f32>(i) * 0.073f;
    last_drone_shot_ = now;
    ++d.shots;
    const f32 player_speed = kern::length(world_->actor_velocity(mode_ == Mode::foot ? soldier_ : mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::jet ? jet_actor_ : drone_actor_));
    const f32 chance = wanted::hit_chance(stars, d.distance, player_speed);
    const f32 roll = std::fmod((static_cast<f32>(i) + 1) * 0.371f + static_cast<f32>(d.shots) * 0.618f + static_cast<f32>(d.serial) * 0.137f, 1.0f);
    const bool accurate = roll < chance;
    Vec3 aim = me_center;
    if (accurate) aim.x += i % 2 ? 0.15f : -0.15f;
    else {
      const f32 miss = std::fmod(static_cast<f32>(i) * 0.93f + static_cast<f32>(d.shots) * 2.17f, 2 * kPi);
      const f32 spread = 0.85f + (1 - chance) * 1.5f + d.distance * 0.018f;
      aim += Vec3{std::cos(miss) * spread, std::sin(miss) * spread, std::sin(miss * 1.7f) * spread * 0.55f};
    }
    kern::game::ShotEvent s;
    s.from = d.pos;
    const Vec3 dir = kern::normalize(aim - d.pos);
    const RayResult r = cast(world_->physics(), d.pos, dir, d.distance + 10, accurate ? kern::physics::Body{} : kern::physics::Body{}, kCatWorld);
    s.to = accurate ? aim : (r.hit ? r.point : d.pos + dir * (d.distance + 8));
    s.impact = accurate || r.hit;
    s.surface = accurate ? 3 : (r.hit ? 1 : 0);
    s.normal = r.normal;
    world_->shots().push_back(s);
    world_->play_sound(combat_sfx(combat_audio::Kind::shot), d.pos, 0.6f, 1.25f, true);
    osfx("wanted_police_drones.shot", d.pos);
    if (accurate && now - last_player_hit_ >= kPdDamageCooldown) {
      last_player_hit_ = now;
      if (mode_ == Mode::foot) world_->damage(soldier_, std::max(2.0f, std::round(wanted::police_damage(stars))), 0, false, d.pos);
      else damage_player_vehicle(mode_ == Mode::drone ? kDroneHit : mode_ == Mode::jet ? kJetHit : kCarHit);
    }
  }
  return sees;
}

void Game::update_police(f32 dt) {
  const f32 now = world_->sim_time();
  const i32 stars = wanted_.stars();
  if (stars > 0 && star_since_ < 0) star_since_ = now;
  if (stars == 0) { star_since_ = -1; next_drone_wave_ = -1; drone_wave_live_ = false; }
  // Units that are due (wantedPoliceCount, arrival delay, a break between waves).
  i32 active = 0;
  for (const PoliceUnit& u : units_)
    if (u.car >= 0 && u.state != PoliceUnit::State::leave && u.state != PoliceUnit::State::abandoned && u.state != PoliceUnit::State::wreck) ++active;
  if (stars > 0 && !player_dead_ && active < wanted::police_count(stars) && now - star_since_ > wanted::arrival_delay_ms(stars) / 1000 &&
      now - last_wave_ > wanted::wave_break_ms(stars) / 1000)
    spawn_police_unit();
  const Vec3 me = player_position();
  for (std::size_t ui = 0; ui < units_.size(); ++ui) {
    PoliceUnit& u = units_[ui];
    if (u.car < 0) continue;
    TrafficCar& c = cars_[u.car];
    // The player took the cruiser: the crew is out and stays on foot.
    if (mode_ == Mode::car && car_index_ == u.car && u.state != PoliceUnit::State::abandoned) {
      for (PoliceUnit::Cop& cop : u.cops)
        if (cop.inside && cop.seed) {
          const Vec3 at = matrix_of(c.actor).translation() + Vec3{2.5f, 2.5f, 0};
          cop.person = people_.add_officer({at.x, at.y, 0}, 0, static_cast<i32>(ui), cop.seed);
          cop.inside = false;
        }
      u.state = PoliceUnit::State::engage;
      c.unit = -1;
    }
    bool any = false;
    for (PoliceUnit::Cop& cop : u.cops) {
      if (!cop.seed) continue;
      if (!cop.inside) step_cop(u, cop, dt);
      if (cop.seed) any = true;
    }
    const bool own_car = c.unit == static_cast<i32>(ui);
    if (!any && own_car && u.state != PoliceUnit::State::wreck) {
      // Nobody left: the cruiser stays where it is and anyone may take it.
      u.state = PoliceUnit::State::abandoned;
      c.unit = -1;
      c.driver = false;
    }
    if (stars == 0 && (u.state == PoliceUnit::State::pursuit || u.state == PoliceUnit::State::stopping)) u.state = PoliceUnit::State::leave;
    if (stars == 0 && u.state == PoliceUnit::State::engage) u.state = PoliceUnit::State::back;
    if (own_car && c.unit >= 0) drive_police(u, dt);
    // Gone: far away (leaving or abandoned and out of sight), or nobody left of the unit.
    const f32 d = dist2d(matrix_of(c.actor).translation(), me);
    const bool done = (u.state == PoliceUnit::State::leave && (d > kDespawn || now - u.left_at > 30)) ||
                      ((u.state == PoliceUnit::State::abandoned || u.state == PoliceUnit::State::wreck) && !any && d > kDespawn) || (!any && !own_car);
    if (done) {
      for (PoliceUnit::Cop& cop : u.cops)
        if (cop.person >= 0) people_.at(cop.person).state = People::State::gone;
      if (c.unit == static_cast<i32>(ui) || u.state == PoliceUnit::State::leave || u.state == PoliceUnit::State::abandoned || u.state == PoliceUnit::State::wreck) {
        if (!(mode_ == Mode::car && car_index_ == u.car)) {
          world_->set_alive(c.actor, false);
          c.placed = false;
          c.unit = -1;
          c.wreck = false;
          c.hit_by_player = false;
          c.hp = car_max_hp(c);
        }
      }
      u = PoliceUnit{};
    }
  }
}

// ---------------------------------------------------------------------------------------------
// per frame

bool Game::request_respawn() {
  // a press right at the moment of death never skips the 3 s (__arondightRespawnReady)
  if (!player_dead_ || respawn_wait() > 0) return false;
  return world_->respawn_now(soldier_);
}

void Game::update_town(f32 dt) {
  if (!world_placed_) return;
  const f32 now = world_->sim_time();
  // Death: the wanted level is cleared, the police calls it off (SpawnCity game_reset.mjs).
  // Alone the player comes back when he asks (request_respawn); with mates on the session's timer.
  {
    const i32 rule = net_.active() ? 1 : 0;
    if (rule != respawn_rule_) {
      respawn_rule_ = rule;
      world_->set_property(soldier_, "components.health.respawnDelay", kern::json::Value(rule ? 3.0 : 1e5));
    }
  }
  const bool dead = world_->hud().respawn_in > 0 || (world_->hud().health >= 0 && world_->hud().health <= 0);
  if (reset_revive_) {
    // RESET raised him: the frame or two until Kern's step did it are no new death
    if (!dead) { reset_revive_ = false; world_->heal(soldier_, 10000); place_world(); }
    player_dead_ = false;
  } else if (dead && !player_dead_) {
    wanted_.clear();
    toast_ = "Erwischt. Neuer Versuch.";
    toast_age_ = 0;
    died_at_ = now;
    died_feet_ = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  } else if (!dead && player_dead_ && !net_.active() && died_at_ >= 0 && mode_ == Mode::foot) {
    // back near where he went down (relocateRespawnOrigin: 3..5 m, never into a wall)
    const Vec2 o = mp::respawn_offset(rng_);
    Spot s{};
    if (!clear_spot(died_feet_ + Vec3{o.x, o.y, 0}, {0.6f, 0.6f}, 24, s)) s = {died_feet_ + Vec3{o.x, o.y, 0}, 0};
    put_down(Vec3{s.p.x, s.p.y, city_.ground_height(s.p.x, s.p.y)}, s.yaw / kDeg);
  } else if (!dead && player_dead_ && net_.active() && mode_ == Mode::foot) {
    // with mates Kern brings him back at the spawn on its timer: put him on the ground as it is now
    // (a crater on the spawn sank it - he would start under the terrain and fall)
    const Vec3 at = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
    const Vec3 f = standing_feet(at);
    if (std::fabs(f.z - at.z) > 0.05f) world_->teleport(soldier_, f + Vec3{0, 0, 0.75f});
  }
  if (!dead) died_at_ = -1;
  player_dead_ = dead;
  handle_shots();
  const PeopleView view = people_view();
  people_.update(dt, now, view, rng_);
  // Burning and crashing cars (critical_damage_logic.mjs).
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (!c.placed || c.wreck) continue;
    const Vec3 v = world_->actor_velocity(c.actor);
    const Vec3 dv3 = v - c.last_velocity;
    const f32 dv = std::sqrt(dv3.x * dv3.x + dv3.y * dv3.y + 0.25f * dv3.z * dv3.z);
    // crashCheck: only the cruisers take crash damage (police_ground_units.mjs CRASH_DV 7, CRASH_DMG 16)
    if (c.police && dv > kCrashDv && dt < 0.1f) damage_car(static_cast<i32>(i), (dv - kCrashDv) * kCrashDamage, false);
    c.last_velocity = v;
    if (c.burn_until >= 0 && now >= c.burn_until) wreck_car(static_cast<i32>(i));
  }
  // Crimes the people saw (and what police within their detection radius see).
  std::vector<CrimeReport> reports;
  people_.take_reports(reports);
  bool police_sees = false;
  const Vec3 me = player_position();
  for (const PoliceUnit& u : units_) {
    if (u.car < 0) continue;
    for (const PoliceUnit::Cop& cop : u.cops)
      if (cop.person >= 0 && cop.los) police_sees = true;
    const Vec3 cp = matrix_of(cars_[u.car].actor).translation();
    if (dist2d(cp, me) < wanted::detection_radius(std::max(1, wanted_.stars())) && line_of_sight(cp + Vec3{0, 0, 1.3f}, me + Vec3{0, 0, 0.6f})) police_sees = true;
  }
  for (const CrimeReport& r : reports) wanted_.report(r.crime, r.witnessed || police_sees);
  if (update_police_drones(dt)) police_sees = true;
  if (police_sees) { last_known_ = me; last_contact_ = now; }
  // The police must arrive before you can shake them: while they are on their way the escape
  // clock does not run (wanted_police_drones.mjs).
  const i32 st = wanted_.stars();
  const bool en_route = st > 0 && star_since_ >= 0 && now < star_since_ + wanted::arrival_delay_ms(st) / 1000 + 6;
  const bool had = wanted_.stars() > 0;
  wanted_.update(dt, police_sees || en_route);
  const i32 stars = wanted_.stars();
  if (had && stars == 0) osfx("wanted_police_drones.escaped", me);  // clearWanted("escaped"): playTone 660 -> 1040
  if (stars > last_stars_) {
    toast_ = "Fahndung: " + std::string(static_cast<std::size_t>(stars), '*');
    toast_age_ = 0;
  } else if (stars == 0 && last_stars_ > 0 && !player_dead_) {
    toast_ = "Abgehaengt.";
    toast_age_ = 0;
  }
  last_stars_ = stars;
  update_police(dt);
}

void Game::update_lamps() {
  // street_lamps.mjs: placement, breakage and drawing live in game_lamps.cpp (sc-m3-gaps).
  gaps_lamps_place();
}

std::string Game::map_json() {
  // The minimap (host page): streets, the player, police and other players around him.
  if (!world_placed_) return "{}";
  const Vec3 me = focus_position();
  const f32 range = 180;
  std::string o;
  char b[96];
  const Vec3 look = cam_.target - cam_.eye;
  std::snprintf(b, sizeof b, "{\"me\":[%.1f,%.1f,%.3f],\"lanes\":[", me.x, me.y, std::atan2(look.y, look.x));
  o += b;
  bool first = true;
  for (const Lane& L : city_.lanes()) {
    if (!L.alive) continue;
    // Near when any segment passes within the range (streets can have far-apart vertices).
    bool near = false;
    for (std::size_t k = 0; k + 1 < L.points.size() && !near; ++k) {
      const Vec2 a = L.points[k], d = L.points[k + 1] - a;
      const f32 l2 = std::fmax(1e-6f, d.x * d.x + d.y * d.y);
      const f32 t = clampf(((me.x - a.x) * d.x + (me.y - a.y) * d.y) / l2, 0, 1);
      near = std::hypot(a.x + d.x * t - me.x, a.y + d.y * t - me.y) < range;
    }
    if (!near) continue;
    o += first ? "[" : ",[";
    first = false;
    for (std::size_t i = 0; i < L.points.size(); ++i) {
      std::snprintf(b, sizeof b, "%s%.0f,%.0f", i ? "," : "", L.points[i].x - me.x, L.points[i].y - me.y);
      o += b;
    }
    o += "]";
  }
  o += "],\"police\":[";
  first = true;
  for (const PoliceUnit& u : units_) {
    if (u.car < 0) continue;
    const Vec3 p = matrix_of(cars_[u.car].actor).translation();
    std::snprintf(b, sizeof b, "%s[%.0f,%.0f]", first ? "" : ",", p.x - me.x, p.y - me.y);
    o += b;
    first = false;
  }
  for (const PoliceDrone& d : pdrones_) {
    if (!d.active) continue;
    std::snprintf(b, sizeof b, "%s[%.0f,%.0f]", first ? "" : ",", d.pos.x - me.x, d.pos.y - me.y);
    o += b;
    first = false;
  }
  o += "],\"players\":[";
  first = true;
  for (const net::Remote& r : net_.remotes()) {
    std::snprintf(b, sizeof b, "%s[%.0f,%.0f]", first ? "" : ",", r.shown.x - me.x, r.shown.y - me.y);
    o += b;
    first = false;
  }
  o += "]";
  if (jet_placed_ && mode_ != Mode::jet) {
    const Vec3 j = matrix_of(jet_actor_).translation();
    std::snprintf(b, sizeof b, ",\"jet\":[%.0f,%.0f]", j.x - me.x, j.y - me.y);
    o += b;
  }
  return o + "}";
}

void Game::draw_town() {
  people_art_.begin();
  if (world_placed_) {
    if (show_lamps_) update_lamps();
    const std::vector<Lane>& lanes = city_.lanes();
    (void)lanes;
    gaps_lamps_draw();  // street_lamps.mjs (game_lamps.cpp)
    const f32 now = world_->sim_time();
    for (const People::Person& p : people_.all())
      if (p.state != People::State::gone && kern::length(p.pos - cam_.eye) < look_.people_m * std::fmin(view_zoom(), 4.0f) && !life_hides_person(static_cast<i32>(&p - people_.all().data()))) people_art_.person(p, now);
    life_draw_ragdolls();  // game_life.cpp
    draw_projectiles();
    draw_guided();  // game_guided.cpp
    for (u32 i = 0; i < 5; ++i) {
      const PoliceDrone& d = pdrones_[i];
      if (d.active) people_art_.police_drone(d.pos, d.yaw, clampf(-d.vel.y * 0.04f, -0.3f, 0.3f), clampf(d.vel.x * 0.04f, -0.3f, 0.3f), !d.emp, now, i);
    }
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      const TrafficCar& c = cars_[i];
      if (!c.placed || !world_->actor_alive(c.actor)) continue;
      if (c.police) {
        const bool on = c.unit >= 0 && c.unit < static_cast<i32>(units_.size()) && units_[c.unit].state != PoliceUnit::State::abandoned &&
                        units_[c.unit].state != PoliceUnit::State::wreck && units_[c.unit].state != PoliceUnit::State::leave;
        if (!(mode_ == Mode::car && car_index_ == static_cast<i32>(i) && car_cockpit_)) people_art_.light_bar(matrix_of(c.actor), on, now);
      }
      if (c.burn_until >= 0 || c.wreck) {
        const Vec3 at = matrix_of(c.actor).translation();
        // critical (burn_until): SpawnCity shows smoke and embers only (world_critical_damage_fx.mjs, game_gaps.cpp)
        if (c.burn_until >= 0) {}
        else if (now - c.wreck_at < 40) people_art_.fire(at + Vec3{0, 0, 0.4f}, 0.35f, now);
      }
    }
  }
  draw_remote_people();
  life_draw_people();  // M3 life: the running club
  people_art_.end();
}

}  // namespace sc

namespace sc {

std::string Game::minimap_json() {
  // real_world_bootstrap.mjs drawMinimap / cacheMinimapFeatures: the map's roads (transportation),
  // water (water, waterway), green areas (park, wood, grass, pitch) and buildings around the
  // subject, at most 80 features, every path thinned to <= 28 points; positions relative to the
  // subject in metres (east, north). The page draws them exactly as the original (web/view.mjs).
  if (!world_placed_) return "{}";
  const Vec3 me = focus_position();
  const f32 range = 420;  // the expanded map shows up to 380 m from the subject
  struct Feat { char kind; bool poly; const std::vector<Vec2>* pts; f32 d; };
  std::vector<Feat> feats;
  std::vector<const MapFeatures*> tiles;
  city_.ready_features(tiles);
  auto dist = [&](const std::vector<Vec2>& pts) {
    f32 best = 1e9f;
    for (std::size_t k = 0; k + 1 < pts.size(); ++k) {
      const Vec2 a = pts[k], d = pts[k + 1] - a;
      const f32 l2 = std::fmax(1e-6f, d.x * d.x + d.y * d.y);
      const f32 t = clampf(((me.x - a.x) * d.x + (me.y - a.y) * d.y) / l2, 0, 1);
      best = std::fmin(best, std::hypot(a.x + d.x * t - me.x, a.y + d.y * t - me.y));
    }
    return best;
  };
  for (const MapFeatures* f : tiles) {
    for (const auto& r : f->roads)
      if (r.points.size() >= 2) feats.push_back({road_is_water(r.cls) ? 'w' : 'r', false, &r.points, dist(r.points)});
    for (const auto& a : f->areas) {
      if (a.rings.empty() || a.rings[0].size() < 3) continue;
      const char k = a.kind == AreaKind::water ? 'w' : (a.kind == AreaKind::sand ? 0 : 'g');
      if (k) feats.push_back({k, true, &a.rings[0], dist(a.rings[0])});
    }
    // buildings: a house is under 300 m across - one far corner rules it out before the outline
    for (const auto& b : f->buildings) {
      if (b.rings.empty() || b.rings[0].size() < 3) continue;
      const Vec2 q = b.rings[0][0];
      if (std::fabs(q.x - me.x) > range + 300 || std::fabs(q.y - me.y) > range + 300) continue;
      feats.push_back({'b', true, &b.rings[0], dist(b.rings[0])});
    }
  }
  std::erase_if(feats, [&](const Feat& f) { return f.d > range; });
  std::sort(feats.begin(), feats.end(), [](const Feat& a, const Feat& b) { return a.d < b.d; });
  if (feats.size() > 80) feats.resize(80);
  // draw order of the original: the map's layer order (water, green, roads, buildings)
  auto order = [](char k) { return k == 'w' ? 0 : k == 'g' ? 1 : k == 'r' ? 2 : 3; };
  std::stable_sort(feats.begin(), feats.end(), [&](const Feat& a, const Feat& b) { return order(a.kind) < order(b.kind); });
  // the subject's heading (compass degrees) and the view's (the minimap turns with the look)
  const Vec3 look = cam_.target - cam_.eye;
  auto compass = [](f32 x, f32 y) { return 90.0f - std::atan2(y, x) * 180.0f / kPi; };
  Vec3 fwd = look;
  kern::game::ActorId a = soldier_;
  if (mode_ == Mode::car && car_index_ >= 0) a = cars_[car_index_].actor;
  else if (mode_ == Mode::jet) a = jet_actor_;
  else if (mode_ == Mode::drone && drone_out_) a = drone_actor_;
  if (a != soldier_) { const kern::Mat4 m = matrix_of(a); fwd = {m.m[0], m.m[1], 0}; }
  std::string o;
  char b[160];
  std::snprintf(b, sizeof b, "{\"me\":[%.1f,%.1f,%.1f],\"bearing\":%.2f,\"look\":%.2f,\"mode\":\"%s\",\"f\":[", me.x, me.y, me.z - city_.ground_height(me.x, me.y),
                compass(fwd.x, fwd.y), compass(look.x, look.y), mode_name(mode_));
  o += b;
  for (std::size_t i = 0; i < feats.size(); ++i) {
    const Feat& f = feats[i];
    const std::vector<Vec2>& p = *f.pts;
    const std::size_t step = std::max<std::size_t>(1, (p.size() + 27) / 28);
    std::snprintf(b, sizeof b, "%s[\"%c%s\"", i ? "," : "", f.kind, f.poly ? "p" : "");
    o += b;
    for (std::size_t k = 0; k < p.size(); k += step) {
      std::snprintf(b, sizeof b, ",%.1f,%.1f", p[k].x - me.x, p[k].y - me.y);
      o += b;
    }
    o += "]";
  }
  return o + "]}";
}

}  // namespace sc
