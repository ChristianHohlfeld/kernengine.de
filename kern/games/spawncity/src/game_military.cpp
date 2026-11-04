// SpawnCity on Kern: the army at high wanted levels (attack helicopter, tank, a squad on foot),
// flak guns against the player's aircraft, fighter jets (low passes and the air strike) and the
// shared combat helpers. Values: military_response.mjs, flak_battery.mjs, fighter_jets.mjs.
#include <algorithm>
#include <cmath>

#include "game.h"
#include "weapons.h"

namespace sc {

using namespace combat;
using kern::Mat4;
using kern::game::ActorId;

namespace {
f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 approach_angle(f32 cur, f32 target, f32 max_step) { return cur + clampf(wrap_pi(target - cur), -max_step, max_step); }
const Vec3 kSoldierShirt{0.33f, 0.38f, 0.24f}, kSoldierPants{0.29f, 0.32f, 0.22f};
const Vec3 kSoldierSkins[4] = {{0.78f, 0.58f, 0.44f}, {0.66f, 0.44f, 0.32f}, {0.48f, 0.29f, 0.2f}, {0.82f, 0.64f, 0.49f}};
}  // namespace

// ---------------------------------------------------------------------------------------------
// shared helpers

bool Game::combat_target(Vec3& at, Vec3& vel, bool& drone, bool& air) const {
  drone = mode_ == Mode::drone && drone_out_;
  air = drone || mode_ == Mode::jet;
  if (player_dead_) return false;
  const ActorId a = drone ? drone_actor_ : mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::jet ? jet_actor_
                    : mode_ == Mode::parachute ? chute_actor_ : soldier_;
  at = matrix_of(a).translation();
  vel = world_->actor_velocity(a);
  return true;
}

void Game::hurt_player(f32 foot, f32 vehicle, Vec3 from) {
  if (mode_ == Mode::foot || mode_ == Mode::parachute) world_->damage(soldier_, foot * combat_.fun.police_damage_scale(), 0, false, from);
  else damage_player_vehicle(vehicle);
}

void Game::combat_tracer(Vec3 a, Vec3 b, Vec3 color) {
  if (combat_.tracers.size() > 96) combat_.tracers.erase(combat_.tracers.begin());
  combat_.tracers.push_back({a, b, world_->sim_time() + 0.08f, color});
}

void Game::combat_banner(const std::string& text) {
  combat_.banner = text;
  combat_.banner_at = world_->sim_time();
}

bool Game::spawn_unseen(Vec3 p, f32 height) const {
  // __spawnVisibilityGuard: out of the drawn view, or behind a building.
  const Vec3 top = p + Vec3{0, 0, height};
  const Vec3 fwd = kern::normalize(cam_.target - cam_.eye, {1, 0, 0}), to = top - cam_.eye;
  const f32 d = kern::length(to);
  if (d < 1) return false;
  if (kern::dot(fwd, to * (1.0f / d)) < std::cos(std::fmin(1.4f, cam_.fov * 0.5f * 1.9f + 0.15f))) return true;
  return cast(world_->physics(), cam_.eye, to * (1.0f / d), d - 1, {}, kCatWorld).hit;
}

// ---------------------------------------------------------------------------------------------
// hits and blasts from the player's weapons

bool Game::combat_shot(kern::game::ShotEvent& s) {
  const Vec3 seg = s.to - s.from;
  const f32 l2 = kern::dot(seg, seg);
  if (l2 < 1e-6f) return false;
  auto near_seg = [&](Vec3 c, f32 r, f32& t) {
    t = clampf(kern::dot(c - s.from, seg) / l2, 0, 1);
    return kern::length(s.from + seg * t - c) < r;
  };
  f32 t;
  for (Heli& h : combat_.helis)
    if (!h.dead && h.state != Heli::State::down && near_seg(h.p, 2.2f, t)) {
      s.to = s.from + seg * t;
      s.surface = 2;
      h.hp -= kMilHit;  // military_response.mjs hit(): HIT 25
      if (h.hp <= 0) { h.state = Heli::State::down; h.v.z = std::fmin(h.v.z, 2.0f); world_->play_sound(combat_sfx(combat_audio::Kind::explosion), h.p, 0.8f, 0.8f, true); }
      return true;
    }
  for (Tank& k : combat_.tanks)
    if (!k.dead && near_seg(k.p + Vec3{0, 0, 1.2f}, 2.6f, t)) {
      s.to = s.from + seg * t;
      s.surface = 2;
      k.hp -= 4;  // armour: a bullet does 4
      if (k.hp <= 0 && !k.wreck) { k.wreck = true; k.wreck_at = world_->sim_time(); explosion(k.p + Vec3{0, 0, 1.5f}, 12, 100, true); }
      return true;
    }
  for (FlakGun& g : combat_.flak)
    if (!g.dead && near_seg(g.p + Vec3{0, 0, 1.2f}, 1.8f, t)) {
      s.to = s.from + seg * t;
      s.surface = 2;
      // flak_battery.mjs onTracer: a sniper round 35, anything else 4
      g.hp -= foot_weapon_ == weapons::Id::sniper && mode_ == Mode::foot ? 35 : 4;
      if (g.hp <= 0) { g.dead = true; ++combat_.flak_destroyed; }
      return true;
    }
  return false;
}

void Game::combat_blast(Vec3 at, f32 radius, f32 damage, bool by_player) {
  const f32 r = clampf(radius, 2, 80), mx = std::fmax(40.0f, damage);
  // military_response.mjs onExplosion (soldiers are People: people_.blast hurts them; on top the
  // soldiers' extra blast damage)
  for (SoldierAi& s : combat_.soldiers) {
    if (s.person < 0) continue;
    People::Person& p = people_.at(s.person);
    if (p.state == People::State::down || p.state == People::State::gone) continue;
    const f32 d = dist2d(p.pos, at);
    if (d < r) { p.hp -= mx * (1 - d / r) * 1.5f + 20; if (p.hp <= 0) people_.blast(p.pos + Vec3{0, 0, 0.9f}, 0.25f, 1e4f, by_player, world_->sim_time()); }
  }
  for (Heli& h : combat_.helis) {
    const f32 d = kern::length(h.p - at);
    if (h.state != Heli::State::down && d < r + 4) {
      h.hp -= mx * (1 - d / (r + 4)) * 2 + 40;
      if (h.hp <= 0) { h.state = Heli::State::down; h.v.z = std::fmin(h.v.z, 2.0f); }
    }
  }
  for (Tank& k : combat_.tanks) {
    const f32 d = dist2d(k.p, at);
    if (!k.wreck && d < r + 3) {
      k.hp -= mx * (1 - d / (r + 3)) * 2.2f + 30;
      if (k.hp <= 0) { k.wreck = true; k.wreck_at = world_->sim_time(); explosion(k.p + Vec3{0, 0, 1.5f}, 12, 100, by_player); }
    }
  }
  // flak_battery.mjs onExplosion
  for (FlakGun& g : combat_.flak) {
    const f32 d = kern::length(g.p + Vec3{0, 0, 1} - at);
    if (!g.dead && d < radius * 0.8f) { g.hp -= std::fmax(100.0f, damage) * (1 - d / radius) * 1.5f; if (g.hp <= 0) { g.dead = true; ++combat_.flak_destroyed; } }
  }
  // zombie_nights.mjs onExplosion: 220 (1 - d / r) + 30
  for (ZombieAi& z : combat_.zombie_ai) {
    if (z.person < 0) continue;
    People::Person& p = people_.at(z.person);
    if (p.state == People::State::down || p.state == People::State::gone) continue;
    const f32 d = dist2d(p.pos, at), zr = clampf(radius, 3, 40);
    if (d < zr) { p.hp -= 220 * (1 - d / zr) + 30; if (p.hp <= 0) people_.blast(p.pos + Vec3{0, 0, 0.9f}, 0.25f, 1e4f, by_player, world_->sim_time()); }
  }
  // sandbox_game.mjs onExplosion: ceil(12 (1 - d / r)) + 1 hits
  for (EnemyAi& e : combat_.enemies) {
    if (e.person < 0) continue;
    People::Person& p = people_.at(e.person);
    if (p.state == People::State::down || p.state == People::State::gone) continue;
    const f32 d = dist2d(p.pos, at), er = clampf(radius, 2, 400);
    if (d < er) {
      e.hits += std::ceil(12 * (1 - d / er)) + 1;
      if (e.hits >= enemy_spec(e.type).hp) people_.blast(p.pos + Vec3{0, 0, 0.9f}, 0.25f, 1e4f, by_player, world_->sim_time());
    }
  }
}

// ---------------------------------------------------------------------------------------------
// military_response.mjs

void Game::spawn_heli(Vec3 target) {
  for (int k = 0; k < 12; ++k) {
    const f32 a = rng_.range(0, 2 * kPi);
    Vec3 p{target.x + std::cos(a) * 650, target.y + std::sin(a) * 650, 0};
    p.z = city_.ground_height(p.x, p.y) + 95;
    if (!spawn_unseen(p, 6)) continue;
    Heli h;
    h.p = p;
    h.orbit = a;
    h.yaw = std::atan2(target.y - p.y, target.x - p.x) - kPi / 2;
    h.next_burst = world_->sim_time() + 5;
    combat_.helis.push_back(h);
    --combat_.pending_heli;
    return;
  }
}

bool Game::spawn_tank(Vec3 target) {
  if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
  // a street point 170..300 m away, outside buildings and out of view
  const std::vector<Lane>& lanes = city_.lanes();
  for (int k = 0; k < 40 && !lanes.empty(); ++k) {
    const Lane& L = lanes[rng_.next() % lanes.size()];
    if (!L.alive || L.points.empty()) continue;
    const Vec2 q = L.points[rng_.next() % L.points.size()];
    const Vec3 p{q.x, q.y, city_.ground_height(q.x, q.y)};
    const f32 d = dist2d(p, target);
    if (d < 170 || d > 300 || !spawn_unseen(p, 2)) continue;
    Tank t;
    t.p = p;
    const f32 now = world_->sim_time();
    t.next_shell = now + 6;
    t.progress_p = {p.x, p.y};
    t.progress_t = now;
    roads_.path({p.x, p.y}, {target.x, target.y}, t.path);
    if (t.path.size() > 1) t.yaw = std::atan2(t.path[1].y - p.y, t.path[1].x - p.x);
    combat_.tanks.push_back(t);
    return true;
  }
  return false;
}

i32 Game::spawn_squad(Vec3 target, i32 n) {
  const std::vector<Lane>& lanes = city_.lanes();
  i32 alive = 0;
  for (const SoldierAi& s : combat_.soldiers) alive += s.person >= 0 ? 1 : 0;
  for (int k = 0; k < 30 && !lanes.empty(); ++k) {
    const Lane& L = lanes[rng_.next() % lanes.size()];
    if (!L.alive || L.points.empty()) continue;
    const Vec2 q = L.points[rng_.next() % L.points.size()];
    const Vec3 base{q.x, q.y, 0};
    const f32 d = dist2d(base, target);
    if (d < 90 || d > 170 || !spawn_unseen(base, 1)) continue;
    i32 placed = 0;
    for (; placed < n && alive < kMaxSoldiers; ++placed, ++alive) {
      const Vec3 p = base + Vec3{static_cast<f32>(placed % 2) * 1.6f, static_cast<f32>(placed / 2) * 1.6f, 0};
      const u32 seed = rng_.next();
      SoldierAi s;
      s.person = people_.add_scripted(People::Kind::soldier, p, 0, kSoldierHp, seed, kSoldierShirt, kSoldierPants, kSoldierSkins[seed % 4], static_cast<u8>(seed % 4));
      s.hold = 20 + rng_.unit() * 12;
      s.next_shot = world_->sim_time() + 3 + rng_.unit() * 2;
      combat_.soldiers.push_back(s);
    }
    return placed;
  }
  return 0;
}

void Game::step_heli(Heli& h, f32 dt) {
  const f32 now = world_->sim_time();
  h.rotor += dt * 28;
  if (h.state == Heli::State::down) {
    h.v.z -= kG * dt;
    h.v = h.v * (1 - 0.3f * dt);
    h.p += h.v * dt;
    h.yaw += 4 * dt;
    const RayResult r = cast(world_->physics(), h.p, {0, 0, -1}, 2.5f, {}, kCatWorld);
    if (r.hit || h.p.z < city_.ground_height(h.p.x, h.p.y) + 1) { explosion(h.p, 14, 120, true); h.dead = true; }
    return;
  }
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && mode_ != Mode::jet;
  const bool leaving = h.state == Heli::State::leave || !have;
  Vec3 goal;
  if (leaving) {
    goal = h.p + Vec3{std::cos(h.orbit), std::sin(h.orbit), 0} * 600.0f;
    goal.z = city_.ground_height(goal.x, goal.y) + 120;
  } else {
    if (h.state == Heli::State::inbound && dist2d(tp, h.p) < 170) h.state = Heli::State::orbit;
    h.orbit += dt * 0.22f;
    const f32 R = h.state == Heli::State::orbit ? 72 : 0;
    goal = {tp.x + std::cos(h.orbit) * R, tp.y + std::sin(h.orbit) * R, 0};
    goal.z = std::fmax(city_.ground_height(goal.x, goal.y) + 42, tp.z + 16);
  }
  // fly: towards the goal, at most 48 m/s
  const Vec3 to = goal - h.p;
  const f32 tl = kern::length(to);
  const Vec3 want = tl > 1e-3f ? to * (std::fmin(48.0f, tl * 0.6f) / tl) : Vec3{};
  h.v += (want - h.v) * damp_alpha(0.9f, dt);
  h.p += h.v * dt;
  const f32 gz = city_.ground_height(h.p.x, h.p.y);
  if (h.p.z < gz + 25) h.p.z += (gz + 25 - h.p.z) * std::fmin(1.0f, dt * 3);
  const f32 face = leaving || h.state == Heli::State::inbound ? std::atan2(h.v.y, h.v.x) : std::atan2(tp.y - h.p.y, tp.x - h.p.x);
  h.yaw += wrap_pi(face - kPi / 2 - h.yaw) * std::fmin(1.0f, dt * 1.6f);
  if (leaving) {
    if (dist2d(h.p, cam_.eye) > 650) h.dead = true;
    return;
  }
  // heavy machine gun: bursts of 7 every 3.5..5.15 s
  if (h.state != Heli::State::orbit) return;
  const f32 d = kern::length(h.p - tp);
  if (now > h.next_burst && d < 240 && line_of_sight(h.p, tp + Vec3{0, 0, 0.2f})) { h.burst = 7; h.next_burst = now + 3.5f + rng_.unit() * 1.65f; }
  if (h.burst > 0 && now > h.next_shot) {
    --h.burst;
    h.next_shot = now + 0.11f;
    const Vec3 fwd{-std::sin(h.yaw), std::cos(h.yaw), 0};
    const Vec3 from = h.p + fwd * 3.4f - Vec3{0, 0, 0.9f};
    const f32 chance = heli_hit_chance(d, kern::length(tv));
    Vec3 aim = tp;
    if (rng_.unit() < chance) hurt_player(drone ? kHeliDmgDrone : kHeliDmg, drone ? kHeliDmgDrone : kHeliDmg, from);
    else aim += Vec3{(rng_.unit() - 0.5f) * 3.6f, (rng_.unit() - 0.5f) * 3.6f, (rng_.unit() - 0.6f) * 1.8f};
    const Vec3 dir = kern::normalize(aim - from);
    const RayResult r = cast(world_->physics(), from, dir, d + 16, {}, kCatWorld);
    kern::game::ShotEvent s;
    s.from = from;
    s.to = r.hit ? r.point : from + dir * (d + 8);
    s.impact = r.hit;
    s.surface = r.hit ? 1 : 0;
    s.normal = r.normal;
    world_->shots().push_back(s);
    combat_tracer(from, s.to);
    world_->play_sound("builtin:smg", from, 0.6f, 0.55f, true);
  }
}

void Game::step_tank(Tank& k, f32 dt) {
  const f32 now = world_->sim_time();
  if (k.wreck) {
    if (now - k.wreck_at > 20 && dist2d(k.p, player_position()) > 120 && spawn_unseen(k.p, 2)) k.dead = true;
    return;
  }
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && mode_ != Mode::jet;
  if (!have) k.leaving = true;
  if (k.leaving && !k.has_exit) { k.has_exit = true; k.exit = {k.p.x + (k.p.x - tp.x) * 3, k.p.y + (k.p.y - tp.y) * 3}; }
  const Vec2 goal = k.leaving ? k.exit : Vec2{tp.x, tp.y};
  const f32 d = dist2d(k.p, tp);
  const Vec3 eye = k.p + Vec3{0, 0, 2.6f};
  const bool los = d < 160 && line_of_sight(eye, tp);
  // drive (police_ground_units-style A* over the street graph, pure pursuit, recovery)
  f32 want_speed = 0, steer_to = k.yaw;
  if (!(!k.leaving && los && d < 110)) {
    if (k.path.size() < 2 || now - k.path_at > 3) {
      if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
      if (!roads_.path({k.p.x, k.p.y}, goal, k.path)) k.path = {{k.p.x, k.p.y}, goal};
      k.path_at = now;
    }
    const Vec2 pt = RoadGraph::pursuit_point(k.path, {k.p.x, k.p.y}, std::fmax(8.0f, k.speed * 1.2f));
    steer_to = std::atan2(pt.y - k.p.y, pt.x - k.p.x);
    want_speed = 9;  // setTarget speedMps 9
    if (now < k.recover_until) {
      want_speed = -3.5f;  // backs out (maxReverse 3.5)
      steer_to = k.yaw + (k.fails % 2 ? 0.6f : -0.6f);
      if (std::fabs(wrap_pi(std::atan2(pt.y - k.p.y, pt.x - k.p.x) - k.yaw)) < 0.5f && now - k.recover_start > 0.9f) k.recover_until = -1;
    } else {
      if (std::hypot(k.p.x - k.progress_p.x, k.p.y - k.progress_p.y) > 4) { k.progress_p = {k.p.x, k.p.y}; k.progress_t = now; if (now - k.recover_start > 8) k.fails = 0; }
      if (std::fabs(k.speed) < 0.5f) { if (k.stuck_since < 0) k.stuck_since = now; }
      else k.stuck_since = -1;
      if ((k.stuck_since >= 0 && now - k.stuck_since > 2.5f) || now - k.progress_t > 7) {
        ++k.fails;
        k.recover_start = now;
        k.recover_until = now + 1.8f + static_cast<f32>(std::min(3, k.fails)) * 0.5f;
        k.stuck_since = -1;
        k.progress_t = now;
        k.path.clear();
      }
    }
  }
  // tracked vehicle: turns on the spot (0.6 rad/s), accelerates 2.5 m/s^2
  const f32 herr = wrap_pi(steer_to - k.yaw);
  k.yaw = approach_angle(k.yaw, steer_to, 0.6f * dt);
  const f32 target_speed = want_speed > 0 ? want_speed * clampf(1.2f - std::fabs(herr), 0, 1) : want_speed;
  k.speed = rate_limit(k.speed, target_speed, 2.5f * dt);
  const Vec2 fwd{std::cos(k.yaw), std::sin(k.yaw)};
  const f32 step = k.speed * dt;
  if (std::fabs(step) > 1e-4f) {
    // the hull (6.6 m) does not drive through walls: a ray ahead at hull height
    const Vec3 dir{fwd.x * sign0(step), fwd.y * sign0(step), 0};
    if (cast(world_->physics(), k.p + Vec3{0, 0, 1.0f}, dir, std::fabs(step) + 3.4f, {}, kCatWorld).hit) k.speed = 0;
    else k.p += Vec3{fwd.x, fwd.y, 0} * step;
  }
  k.p.z = city_.ground_height(k.p.x, k.p.y);
  // turret: slew to the target (0.7 rad/s), main gun every 6.5..9 s, a lead and a miss you can outrun
  const f32 want = std::atan2(tp.y - k.p.y, tp.x - k.p.x) - k.yaw;
  k.turret += clampf(wrap_pi(want - k.turret), -dt * 0.7f, dt * 0.7f);
  if (!k.leaving && los && now > k.next_shell && std::fabs(wrap_pi(want - k.turret)) < 0.06f) {
    k.next_shell = now + 6.5f + rng_.unit() * 2.5f;
    const f32 yaw = k.yaw + k.turret;
    const Vec3 muzzle = k.p + Vec3{std::cos(yaw) * 5.8f, std::sin(yaw) * 5.8f, 2};
    const f32 sp = kern::length(tv), err = tank_aim_error(d, sp);
    const Vec3 aim{tp.x + tv.x * 0.4f + (rng_.unit() - 0.5f) * err * 2, tp.y + tv.y * 0.4f + (rng_.unit() - 0.5f) * err * 2,
                   std::fmax(city_.ground_height(tp.x, tp.y), tp.z - 1.2f)};
    combat_.shells.push_back({muzzle, kern::normalize(aim - muzzle) * 130.0f, 3});
    world_->particle_burst(muzzle, {1.6f, 1.2f, 0.7f});
    world_->play_sound(combat_sfx(combat_audio::Kind::explosion), muzzle, 0.9f, 1.35f, true);
  }
  if (k.leaving && d > 300 && spawn_unseen(k.p, 2)) k.dead = true;
}

void Game::step_soldier(SoldierAi& s, f32 dt) {
  People::Person& p = people_.at(s.person);
  if (p.kind != People::Kind::soldier || p.state == People::State::down || p.state == People::State::gone) { s.person = -1; return; }
  const f32 now = world_->sim_time();
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && mode_ != Mode::jet;
  const bool active = combat_.military.active && have;
  const f32 dx = tp.x - p.pos.x, dy = tp.y - p.pos.y, d = std::hypot(dx, dy);
  if (now - s.los_at > 0.35f) {
    s.los_at = now;
    const bool was = s.los;
    s.los = have && d < 90 && line_of_sight(p.pos + Vec3{0, 0, 1.5f}, tp - Vec3{0, 0, 0.3f});
    if (s.los && !was && !s.burst) s.next_shot = std::fmax(s.next_shot, now + kSoldierReactS + rng_.unit() * 0.6f);
  }
  Vec2 move{0, 0};
  if (!active) move = d > 0.1f ? Vec2{-dx / d, -dy / d} : Vec2{};
  else if (!s.los) {
    // no sight: advance along the streets (A* over the road graph), not into walls
    if (s.path.size() < 2 || now - s.path_at > 3) {
      if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
      if (!roads_.path({p.pos.x, p.pos.y}, {tp.x, tp.y}, s.path)) s.path = {{p.pos.x, p.pos.y}, {tp.x, tp.y}};
      s.path_at = now;
    }
    const Vec2 wp = RoadGraph::pursuit_point(s.path, {p.pos.x, p.pos.y}, 5);
    const Vec2 e{wp.x - p.pos.x, wp.y - p.pos.y};
    const f32 el = std::fmax(1e-3f, std::hypot(e.x, e.y));
    move = e * (1.0f / el);
  } else if (d > s.hold + 8) move = {dx / d, dy / d};
  else if (d < s.hold - 10) move = {-dx / d * 0.5f, -dy / d * 0.5f};
  p.aiming = s.los && active;
  if (move.x != 0 || move.y != 0) {
    const f32 sp = active ? (d > 60 ? 3.8f : 2.6f) : 3.8f;
    const bool ok = people_.free_move(world_->physics(), p, move, sp * dt, false);
    p.v = ok ? sp : 0;
    const f32 face = s.los && active ? std::atan2(dy, dx) : std::atan2(move.y, move.x);
    p.yaw = approach_angle(p.yaw, face, dt * 9);
  } else {
    p.v = 0;
    p.yaw = approach_angle(p.yaw, std::atan2(dy, dx), dt * 10);
  }
  p.phase += p.v * dt * 3.4f;
  // rifle: 3-round bursts, no more than 3 soldiers opening up per 1.4 s
  if (!(active && s.los && d < s.hold + 20 && now > s.next_shot)) return;
  if (!s.burst) {
    std::erase_if(combat_.burst_starts, [&](f32 t) { return now - t >= kBurstWindowS; });
    if (static_cast<i32>(combat_.burst_starts.size()) >= kSoldierBursts) { s.next_shot = now + 0.25f + rng_.unit() * 0.4f; return; }
  }
  if (std::fabs(wrap_pi(p.yaw - std::atan2(dy, dx))) >= 0.3f) return;
  if (!s.burst) { s.burst = 3; combat_.burst_starts.push_back(now); }
  --s.burst;
  s.next_shot = s.burst ? now + 0.12f : now + 2.2f + rng_.unit() * 1.6f;
  s.last_shot = now;
  const Vec3 from = p.pos + Vec3{std::cos(p.yaw) * 0.55f, std::sin(p.yaw) * 0.55f, 1.4f};
  Vec3 aim = tp;
  if (rng_.unit() < soldier_hit_chance(d, kern::length(tv))) hurt_player(drone ? kSoldierDmgDrone : kSoldierDmg, drone ? kSoldierDmgDrone : kSoldierDmg, from);
  else aim += Vec3{(rng_.unit() - 0.5f) * 2.2f, (rng_.unit() - 0.5f) * 2.2f, (rng_.unit() - 0.6f) * 1.1f};
  const Vec3 dir = kern::normalize(aim - from);
  const RayResult r = cast(world_->physics(), from, dir, d + 16, {}, kCatWorld);
  kern::game::ShotEvent e;
  e.from = from;
  e.to = r.hit ? r.point : from + dir * (d + 8);
  e.impact = r.hit;
  e.surface = r.hit ? 1 : 0;
  e.normal = r.normal;
  world_->shots().push_back(e);
  combat_tracer(from, e.to);
  world_->play_sound("builtin:smg", from, 0.45f, 0.9f, true);
}

void Game::update_military(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && mode_ != Mode::jet;  // a pilot in the jet is out of their reach
  if (have) {
    switch (C.military.update(now, wanted_.stars(), wanted_.heat(), C.military_enabled)) {
      case MilEvent::alert: combat_banner("MILITAER ALARMIERT"); break;
      case MilEvent::cancelled: combat_banner("MILITAER-ALARM AUFGEHOBEN"); break;
      case MilEvent::start:
        C.pending_heli = 1;
        C.pending_squad = kSquad;
        C.pending_tank = 1;
        C.pending_at = 0;
        combat_banner("MILITAER RUECKT AN");
        break;
      case MilEvent::stop:
        C.pending_heli = C.pending_squad = C.pending_tank = 0;
        for (Heli& h : C.helis) if (h.state != Heli::State::down) h.state = Heli::State::leave;
        for (Tank& k : C.tanks) k.leaving = true;
        combat_banner("MILITAER ZIEHT AB");
        break;
      case MilEvent::none: break;
    }
    if (C.military.active && now > C.military.next_reinforce) {
      C.military.next_reinforce = now + kReinforceS;
      i32 helis = 0, soldiers = 0;
      bool tank = false;
      for (const Heli& h : C.helis) helis += !h.dead && h.state != Heli::State::down;
      for (const SoldierAi& s : C.soldiers) soldiers += s.person >= 0;
      for (const Tank& k : C.tanks) tank = tank || (!k.dead && !k.wreck);
      if (helis + C.pending_heli < 2) ++C.pending_heli;
      if (soldiers + C.pending_squad < kMaxSoldiers - 1) C.pending_squad += 2;
      if (!tank && !C.pending_tank) C.pending_tank = 1;
    }
    // what could not come in unseen yet is tried again a second later
    if (C.military.active && now >= C.pending_at) {
      C.pending_at = now + 1;
      if (C.pending_heli > 0) spawn_heli(tp);
      if (C.pending_squad > 0) C.pending_squad -= spawn_squad(tp, C.pending_squad);
      if (C.pending_tank > 0 && spawn_tank(tp)) --C.pending_tank;
    }
  }
  for (Heli& h : C.helis) step_heli(h, dt);
  for (Tank& k : C.tanks) step_tank(k, dt);
  for (SoldierAi& s : C.soldiers) if (s.person >= 0) step_soldier(s, dt);
  std::erase_if(C.helis, [](const Heli& h) { return h.dead; });
  std::erase_if(C.tanks, [](const Tank& k) { return k.dead; });
  // withdrawn soldiers leave where nobody sees them go
  if (!C.military.active)
    for (SoldierAi& s : C.soldiers)
      if (s.person >= 0) {
        People::Person& p = people_.at(s.person);
        if (dist2d(p.pos, tp) > 140 && spawn_unseen(p.pos, 1.8f)) { p.state = People::State::gone; s.person = -1; }
      }
  std::erase_if(C.soldiers, [](const SoldierAi& s) { return s.person < 0; });
  // shells: 130 m/s, 6 m blast of 30
  for (Shell& s : C.shells) {
    const Vec3 prev = s.p;
    s.p += s.v * dt;
    s.life -= dt;
    const Vec3 step = s.p - prev;
    const f32 l = kern::length(step);
    const RayResult r = l > 1e-4f ? cast(world_->physics(), prev, step * (1.0f / l), l, {}, kCatWorld) : RayResult{};
    bool hit = r.hit || s.p.z <= city_.ground_height(s.p.x, s.p.y) + 0.2f;
    Vec3 at = r.hit ? r.point : s.p;
    if (!hit && have && kern::length(s.p - tp) < 2.2f) hit = true;
    if (hit) { explosion(at, 6, kShellDmg, false); s.life = -1; }
    else if (s.life > 0) combat_tracer(prev, s.p, {3.0f, 1.6f, 0.6f});
  }
  std::erase_if(C.shells, [](const Shell& s) { return s.life <= 0; });
}

// ---------------------------------------------------------------------------------------------
// flak_battery.mjs

void Game::update_flak(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  // scan: the 3x3 cells of 700 m around the listener, a hash decides gun and spot
  if (now - C.flak_scan > 1.5f) {
    C.flak_scan = now;
    const i64 cx = static_cast<i64>(std::floor(cam_.eye.x / kFlakCell)), cy = static_cast<i64>(std::floor(cam_.eye.y / kFlakCell));
    std::vector<FlakGun> keep;
    for (i64 dx = -1; dx <= 1; ++dx)
      for (i64 dy = -1; dy <= 1; ++dy) {
        u32 seed;
        if (!flak_cell(cx + dx, cy + dy, seed)) continue;
        auto old = std::find_if(C.flak.begin(), C.flak.end(), [&](const FlakGun& g) { return g.cx == cx + dx && g.cy == cy + dy; });
        if (old != C.flak.end()) { keep.push_back(*old); continue; }
        for (u32 t = 0; t < 10; ++t) {
          const f32 x = (static_cast<f32>(cx + dx) + 0.15f + 0.7f * cell_rand(seed, 1 + t * 2)) * kFlakCell;
          const f32 y = (static_cast<f32>(cy + dy) + 0.15f + 0.7f * cell_rand(seed, 2 + t * 2)) * kFlakCell;
          // open ground: no roof and no wall within 3 m
          bool open = true;
          for (const Vec2 o : {Vec2{0, 0}, Vec2{3, 0}, Vec2{-3, 0}, Vec2{0, 3}, Vec2{0, -3}}) {
            const RayResult r = cast(world_->physics(), {x + o.x, y + o.y, 60}, {0, 0, -1}, 80, {}, kCatWorld);
            if (r.hit && r.point.z > city_.ground_height(x, y) + 0.4f) { open = false; break; }
          }
          if (!open) continue;
          FlakGun g;
          g.cx = cx + dx;
          g.cy = cy + dy;
          g.p = {x, y, city_.ground_height(x, y)};
          g.base_yaw = g.yaw = cell_rand(seed, 40) * 2 * kPi;
          g.burst_t = cell_rand(seed, 41) * 3;
          keep.push_back(g);
          break;
        }
      }
    C.flak.swap(keep);
  }
  // the target: your jet, else your drone in flight
  Vec3 tp, tv;
  bool drone, air;
  const bool have = combat_target(tp, tv, drone, air) && air;
  if (have) {
    if (C.target_had) { C.target_vel = (tp - C.target_last) * (1.0f / std::fmax(0.016f, dt)); if (kern::length(C.target_vel) > 400) C.target_vel = {}; }
    C.target_last = tp;
  }
  C.target_had = have;
  for (FlakGun& g : C.flak) {
    if (g.dead) continue;
    bool engaged = false;
    if (have) {
      const Vec3 base = g.p + Vec3{0, 0, 1.6f};
      const f32 dist = kern::length(tp - base), agl = tp.z - city_.ground_height(tp.x, tp.y);
      if (now - g.los_at >= 0.4f) { g.los_at = now; g.los = line_of_sight(base, tp); }
      if (dist < kFlakRange && agl > kFlakMinAgl && g.los) {
        engaged = true;
        // lead: where the target will be when the round gets there
        const Vec3 aim = tp + C.target_vel * (dist / kFlakMuzzle);
        const f32 want_yaw = std::atan2(aim.y - g.p.y, aim.x - g.p.x), want_pitch = std::atan2(aim.z - g.p.z - 1.6f, std::hypot(aim.x - g.p.x, aim.y - g.p.y));
        const f32 dyaw = wrap_pi(want_yaw - g.yaw);
        g.yaw += clampf(dyaw, -2.2f * dt, 2.2f * dt);
        g.pitch += clampf(want_pitch - g.pitch, -1.6f * dt, 1.6f * dt);
        g.burst_t += dt;
        const f32 phase = std::fmod(g.burst_t, kFlakOn + kFlakOff);
        if (phase < kFlakOn && std::fabs(dyaw) < 0.15f) {
          for (int n = 0; g.next_shot <= now && n < 8; ++n) {
            const Vec3 mz = g.p + Vec3{0, 0, 1.5f} + Vec3{std::cos(g.yaw) * std::cos(g.pitch), std::sin(g.yaw) * std::cos(g.pitch), std::sin(g.pitch)} * 2.7f;
            Vec3 dir = kern::normalize(aim - mz);
            dir = kern::normalize(dir + Vec3{rng_.unit() - 0.5f, rng_.unit() - 0.5f, rng_.unit() - 0.5f} * (kFlakSpread * 2));
            const f32 fuse = clampf(kern::length(aim - mz) / kFlakMuzzle + (rng_.unit() - 0.5f) * 0.06f, 0.05f, 1.6f);
            if (C.flak_rounds.size() < 180) C.flak_rounds.push_back({mz, dir * kFlakMuzzle, now + fuse});
            g.flash_at = now;
            ++g.barrel;
            if (g.barrel % 3 == 0) world_->play_sound("builtin:launcher", mz, clampf(1.4f / (1 + kern::length(mz - cam_.eye) / 90), 0, 1.2f) * 0.55f, 1.6f, true);
            g.next_shot = std::fmax(g.next_shot + 1.0f / kFlakRps, now - 0.05f);
          }
        }
      }
    }
    if (!engaged) { g.next_shot = now; g.pitch += (0.25f - g.pitch) * std::fmin(1.0f, dt * 0.5f); }
  }
  // tracers fly straight (900 m/s barely drops), airburst at the fuse
  for (FlakRound& r : C.flak_rounds) {
    if (now >= r.until) {
      if (C.puffs.size() > 60) C.puffs.erase(C.puffs.begin());
      C.puffs.push_back({r.p, now});
      if (have) {
        const f32 dmg = flak_burst_damage(kern::length(tp - r.p));
        if (dmg > 0) { damage_player_vehicle(dmg); ++C.flak_hits; }
      }
      r.until = -1;
      continue;
    }
    r.p += r.v * dt;
    r.v.z -= kG * dt;
  }
  std::erase_if(C.flak_rounds, [](const FlakRound& r) { return r.until < 0; });
  std::erase_if(C.puffs, [&](const Puff& p) { return now - p.born > 2.4f; });
}

// ---------------------------------------------------------------------------------------------
// fighter_jets.mjs

void Game::launch_jets(f32 cx, f32 cy, bool strike, f32 at, f32 alt, f32 offset) {
  const f32 z = city_.ground_height(cx, cy) + alt;
  // a pair heading north: the wingman 26 m left, 42 m behind, 6 m higher
  for (int k = 0; k < 2; ++k) {
    FighterJet j;
    j.x = cx + offset - static_cast<f32>(k) * 26;
    j.y0 = cy - kJetSpan - static_cast<f32>(k) * 42;
    j.z = z + static_cast<f32>(k) * 6;
    j.t0 = at;
    j.strike = strike && k == 0;
    j.target = {cx, cy};
    combat_.jets.push_back(j);
  }
}

bool Game::air_strike(Vec3 p) {
  const f32 now = world_->sim_time();
  if (now - combat_.last_strike < kStrikeCooldownS || player_dead_) return false;
  combat_.last_strike = now;
  p.z = city_.ground_height(p.x, p.y);
  combat_.flares.push_back({p, now, now + 9});
  launch_jets(p.x, p.y, true, strike_launch_time(now), 70, 0);
  combat_banner("LUFTSCHLAG ANGEFORDERT");
  return true;
}

void Game::update_jets(f32 dt) {
  CombatState& C = combat_;
  const f32 now = world_->sim_time();
  // ambient: a pair every 75..150 s, 55..85 m over the listener
  if (now > C.next_ambient) {
    C.next_ambient = now + kAmbientMinS + rng_.unit() * (kAmbientMaxS - kAmbientMinS);
    launch_jets(cam_.eye.x, cam_.eye.y, false, now, kJetAltMin + rng_.unit() * (kJetAltMax - kJetAltMin), (rng_.unit() - 0.5f) * 160);
  }
  for (FighterJet& j : C.jets) {
    const f32 y = j.y0 + kJetSpeed * (now - j.t0);
    const Vec3 p{j.x, y, j.z};
    // the roar arrives late (retarded position, 343 m/s): heard when the sound from the closest
    // point reaches the listener; a close pass shakes the camera
    const f32 d = kern::length(p - cam_.eye);
    const f32 closest_y = cam_.eye.y, closest = std::hypot(j.x - cam_.eye.x, j.z - cam_.eye.z);
    if (!j.heard && now >= j.t0 && y > closest_y && (y - closest_y) / kJetSpeed >= closest / kSoundMps) {
      j.heard = true;
      world_->play_sound("builtin:boost", {j.x, closest_y, j.z}, clampf(std::pow(85 / std::fmax(8.0f, closest), 1.35f) * 0.77f, 0.05f, 1.6f), 0.55f, true);
    }
    if (!j.shook && d < 160) { j.shook = true; world_->shake_camera(std::fmin(0.85f, 0.35f + 60 / d)); }
    // strike run: a stick of 10 across the mark, released 380 m before it
    if (j.strike && j.dropped < kStick && j.target.y - kStickLen / 2 - y < 380) {
      for (i32 k = j.dropped; k < kStick; ++k) {
        Vec3 to{j.target.x + (rng_.unit() - 0.5f) * 5, j.target.y - kStickLen / 2 + static_cast<f32>(k) * kStickLen / (kStick - 1), 0};
        to.z = city_.ground_height(to.x, to.y);
        C.bombs.push_back({p + Vec3{0, -2, -1.2f}, to, now + static_cast<f32>(k) * (kStickLen / (kStick - 1)) / kJetSpeed, 1.5f + rng_.unit() * 0.25f});
      }
      j.dropped = kStick;
    }
  }
  std::erase_if(C.jets, [&](const FighterJet& j) { return j.y0 + kJetSpeed * (now - j.t0) > j.y0 + 2 * kJetSpan; });
  for (Bomb& b : C.bombs) {
    if (now - b.t0 >= b.dur) {
      explosion(b.to + Vec3{0, 0, 0.4f}, kBombRadius, kBombDamage, true);
      C.blasts.push_back({b.to, now, 1});
      b.dur = -1;
    }
  }
  std::erase_if(C.bombs, [](const Bomb& b) { return b.dur < 0; });
  std::erase_if(C.blasts, [&](const Blast& b) { return now - b.born > 6; });
  std::erase_if(C.flares, [&](const Flare& f) { return now > f.until; });
  (void)dt;
}

// ---------------------------------------------------------------------------------------------
// drawing

void Game::draw_military() {
  CombatArt& A = combat_art_;
  const f32 now = world_->sim_time();
  for (const Heli& h : combat_.helis)
    A.heli(Mat4::translate(h.p) * Mat4::trs({}, Quat::axis_angle({0, 0, 1}, h.yaw) * Quat::axis_angle({1, 0, 0}, clampf(-kern::length(h.v) * 0.004f, -0.25f, 0)), {1, 1, 1}),
           h.rotor, h.state == Heli::State::down);
  for (const Tank& k : combat_.tanks) A.tank(Mat4::translate(k.p) * Mat4::trs({}, yaw_quat(k.yaw), {1, 1, 1}), k.turret, k.wreck);
  for (const Tank& k : combat_.tanks)
    if (k.wreck && now - k.wreck_at < 40) people_art_.fire(k.p + Vec3{0, 0, 1.6f}, 0.9f, now);
  for (const FlakGun& g : combat_.flak)
    if (kern::length(g.p - cam_.eye) < 900) A.flak(g.p, g.base_yaw, g.yaw, g.pitch, g.dead, now - g.flash_at < 0.045f);
  for (const FlakRound& r : combat_.flak_rounds) A.beam(r.p, r.p - kern::normalize(r.v) * std::fmin(14.0f, kern::length(r.v) * 0.016f), 0.12f, {3.2f, 1.8f, 0.6f});
  for (const Puff& p : combat_.puffs) {
    const f32 a = now - p.born, s = (1.2f + a * 2.2f) * 2;
    A.put(CombatArt::smoke, Mat4::translate(p.p + Vec3{0, 0, a * 0.6f}) * Mat4::scale({s, s, s}), {0.14f, 0.13f, 0.12f});
  }
  for (const FighterJet& j : combat_.jets) {
    const Vec3 p{j.x, j.y0 + kJetSpeed * (now - j.t0), j.z};
    if (kern::length(p - cam_.eye) < 2400) A.fighter(p, 0.8f + 0.5f * std::fabs(std::sin(now * 37 + j.x)));
  }
  for (const Bomb& b : combat_.bombs) {
    const f32 u = clampf((now - b.t0) / b.dur, 0, 1);
    if (now < b.t0) continue;
    Vec3 p = b.from + (b.to - b.from) * u;
    p.z = b.from.z + (b.to.z - b.from.z) * u * u;
    A.put(CombatArt::cylinder, Mat4::translate(p) * Mat4::scale({0.44f, 0.44f, 1.5f}), {0.23f, 0.25f, 0.21f});
  }
  for (const Blast& b : combat_.blasts) {
    const f32 t = now - b.born, e = 1 - std::exp(-t * 7);
    if (t < 0.62f) { const f32 s = (2 + 11 * e) * 2 * b.scale; A.put(CombatArt::glow_sphere, Mat4::translate(b.p) * Mat4::scale({s, s, s}), Vec3{3.0f, 1.8f, 0.7f} * (1 - t * 1.6f)); }
    const f32 s = (3 + 12 * std::fmin(1.0f, t * 0.8f)) * 2 * b.scale;
    A.put(CombatArt::smoke, Mat4::translate(b.p + Vec3{0, 0, t * 2.2f}) * Mat4::scale({s, s, s * 0.8f}), Vec3{0.17f, 0.15f, 0.14f} * (1 - t / 6) + Vec3{0.05f, 0.05f, 0.05f});
  }
  for (const Flare& f : combat_.flares) {
    // the red strike laser from the sky onto the mark (k: reaching the ground in 0.42 s)
    const f32 k = std::fmin(1.0f, (now - f.born) / 0.42f), len = 420 * (1 - (1 - k) * (1 - k) * (1 - k));
    const f32 pulse = 0.75f + 0.25f * std::sin(now * 18);
    A.put(CombatArt::glow_box, Mat4::translate(f.p + Vec3{0, 0, 420 - len * 0.5f}) * Mat4::scale({0.44f, 0.44f, len}), Vec3{3.0f, 0.3f, 0.2f} * pulse);
    A.put(CombatArt::glow_sphere, Mat4::translate(f.p + Vec3{0, 0, 0.3f}) * Mat4::scale({0.7f, 0.7f, 0.7f}), {4.0f, 0.4f, 0.2f});
  }
  for (const Tracer& t : combat_.tracers)
    if (now <= t.until) A.beam(t.a, t.b, 0.06f, t.color);
  std::erase_if(combat_.tracers, [&](const Tracer& t) { return now > t.until; });
}

}  // namespace sc
