// SpawnCity on Kern: the projectiles and melee on foot, 1:1 after gameplay_final_runtime_v2.mjs
// (updateFootGrenades, bounceFootGrenade, launchFootGrenade, launchFootRocket, steerRocket,
// emitRocketSmoke, punchAt, knifeAt, meleeFallback, shotThroughGrenades). Hooks: update_projectiles
// and foot_attack (game_weapons.cpp), gaps_update / gaps_draw (game_gaps.cpp).
#include <algorithm>
#include <cmath>

#include "game.h"
#include "game_gaps.h"
#include "game_life.h"

namespace sc {

using weapons::Id;

namespace {
// gameplay_final_runtime_v2.mjs
constexpr f32 kGrenadeR = 0.039f * 1.35f;        // GRENADE_BASE_RADIUS_M * GRENADE_VISUAL_SCALE
constexpr f32 kGrenadeGravity = 9.81f;           // GRENADE_GRAVITY_MPS2
constexpr f32 kRestitution = 0.58f;              // GRENADE_RESTITUTION
constexpr f32 kSurfaceDamping = 0.88f;           // GRENADE_SURFACE_DAMPING
constexpr i32 kMaxBounces = 7;                   // GRENADE_MAX_BOUNCES
constexpr f32 kFloorNormalZ = 0.58f;             // GRENADE_FLOOR_NORMAL_Z
constexpr f32 kGrenadeTtl = 5.6f;                // GRENADE_TTL_MS
constexpr f32 kRocketTtl = 9.0f;                 // ROCKET_FOOT_TTL_MS
constexpr f32 kRocketTop = 40, kRocketAccel = 55;  // ROCKET_FOOT_SPEED_MPS, ROCKET_FOOT_ACCEL_MPS2
constexpr f32 kRocketTurn = 2.8f;                // ROCKET_TURN_RAD_S
constexpr f32 kLaserRange = 420;                 // ROCKET_LASER_RANGE_M
constexpr f32 kShotHit = 0.35f;                  // GRENADE_SHOT_HIT_M
constexpr f32 kFistRecover[2] = {0.30f, 0.25f};  // FIST_RECOVER_MS
constexpr f32 kFistDmg[2] = {24, 16};            // PUNCH_DMG
constexpr f32 kCombo = 0.43f;                    // COMBO_MS
constexpr f32 kPunchReach = 1.8f;                // PUNCH_REACH_M
constexpr f32 kKnifeRecover = 0.38f, kKnifeReach = 1.6f, kKnifeDmg = 100, kKnifeStrike = 0.13f;  // KNIFE_* (Kern: a stab kills, as Call of Duty - 45 left them standing up again)

bool foot_round(Id id) { return id == Id::grenade_launcher || id == Id::rocket || id == Id::hand_grenade; }
}  // namespace

bool Game::soft_target_near(Vec3 p) const {
  // softTargetNear: lamp posts (0.35 m round the post up to 6.4 m, 0.45 m / 0.4 m at the head 6 m up)
  // and cows (1.05 m, up to 1.7 m over the ground) are not in the round's raycast set: it goes off there
  if (gaps_)
    for (const lamps::Lamp& l : gaps_->lamps.all()) {
      if (l.state == 2) continue;
      if (std::fabs(l.foot.x - p.x) < 0.35f && std::fabs(l.foot.y - p.y) < 0.35f && p.z > l.foot.z && p.z < l.foot.z + 6.4f) return true;
      if (std::fabs(l.head.x - p.x) < 0.45f && std::fabs(l.head.y - p.y) < 0.45f && std::fabs(l.foot.z + 6 - p.z) < 0.4f) return true;
    }
  if (glife_ && glife_->ready)
    for (const Life::Cow& c : glife_->life.cows()) {
      if (c.dead) continue;
      const f32 dx = p.x - c.pos.x, dy = p.y - c.pos.y;
      if (dx * dx + dy * dy < 1.1f && p.z < city_.ground_height(c.pos.x, c.pos.y) + 1.7f) return true;
    }
  return false;
}

bool Game::car_near_point(Vec3 point) const {
  // grenadeImpactMode "target": a car (by its footprint, as hit_scan finds one)
  for (const TrafficCar& c : cars_) {
    if (!c.placed || c.wreck) continue;
    const BodyState b = state_of(c.actor);
    if (kern::length(b.position - point) > 6) continue;
    const Vec3 local = to_local(b.rotation, point - b.position);
    const Vec3 h = c.car->spec().half;
    if (std::fabs(local.x) < h.x + 0.4f && std::fabs(local.y) < h.y + 0.4f && std::fabs(local.z) < 2.0f) return true;
  }
  return false;
}

void Game::grenade_bounce(Projectile& p, Vec3 point, Vec3 n, f32 now) {
  // bounceFootGrenade: v -= (1 + .58) vn n, x .88, nextCollisionAt +36 ms, lifted R + .014 off the
  // surface and .02 along the new flight; at the 7th bounce slower than 2.8 m/s x .38
  const f32 impact = kern::length(p.vel);
  if (kern::dot(p.vel, n) > 0) n = n * -1.0f;
  const f32 vn = kern::dot(p.vel, n);
  if (vn < 0) p.vel = p.vel - n * ((1 + kRestitution) * vn);
  p.vel = p.vel * kSurfaceDamping;
  p.bounces = std::min(kMaxBounces, p.bounces + 1);
  p.resting = false;
  p.next_collision = now + 0.036f;
  p.pos = point + n * (kGrenadeR + 0.014f);
  const f32 sp = kern::length(p.vel);
  if (sp > 0.05f) p.pos = p.pos + p.vel * (0.020f / sp);
  // audioGrenadeBounce: gain .16 + v * .018 (.16 .. .44), rate .92 + n * .035 + .02 min(v, 10) (.88 .. 1.32)
  osfx("gameplay_final_runtime_v2.audioGrenadeBounce", point, impact, static_cast<f32>(p.bounces));
  if (p.id == Id::grenade_launcher) bounce_fx_[bounce_fx_next_++ % 8] = {point + n * 0.008f, n, now, clampf(impact / 12, 0.55f, 1.35f)};
  world_->play_sound(combat_sfx(combat_audio::Kind::bounce), point, clampf(0.16f + impact * 0.018f, 0.16f, 0.44f) / 0.44f * 0.5f,
                     clampf(0.92f + p.bounces * 0.035f + 0.02f * std::fmin(impact, 10.0f), 0.88f, 1.32f), true);
  if (p.bounces >= kMaxBounces && kern::length(p.vel) < 2.8f) p.vel = p.vel * 0.38f;
}

bool Game::foot_round_step(Projectile& p, f32 dt, f32 now, Vec3& at) {
  // updateFootGrenades: fuse / TTL, then up to 4 substeps of 14 ms
  at = p.pos;
  if (now - p.born >= p.fuse || now - p.born > (p.id == Id::rocket ? kRocketTtl : kGrenadeTtl)) return true;
  const i32 n = std::clamp(static_cast<i32>(std::ceil(dt / 0.014f)), 1, 4);
  const f32 sdt = dt / static_cast<f32>(n);
  const bool hand = p.id == Id::hand_grenade;
  for (i32 i = 0; i < n; ++i) {
    if (p.id == Id::rocket) {
      // the laser: only while the launcher is in hand on foot; once off, the rocket flies unguided
      const bool laser = mode_ == Mode::foot && foot_weapon_ == Id::rocket && !player_dead_;
      if (p.guided && !laser) p.guided = false;
      if (!p.guided) p.vel.z -= 1.2f * sdt;
      f32 sp = kern::length(p.vel);
      Vec3 dir = p.vel * (1.0f / std::fmax(0.001f, sp));
      if (p.guided) {
        // steerRocket: toward the dot (the crosshair ray's hit within 420 m, else its end), at most 2.8 rad/s
        const Vec3 dot_at = rocket_laser_dot();
        const Vec3 to = dot_at - p.pos;
        const f32 d = kern::length(to);
        if (d >= 0.5f) {
          const Vec3 want = to * (1.0f / d);
          const f32 ang = std::acos(clampf(kern::dot(dir, want), -1, 1)), max_turn = kRocketTurn * sdt;
          if (ang >= 1e-4f) {
            const f32 turn = std::fmin(ang, max_turn);
            const Vec3 perp = kern::normalize(want - dir * std::cos(ang), dir);
            dir = kern::normalize(dir * std::cos(turn) + perp * std::sin(turn), dir);
          }
        }
      }
      sp = std::fmin(kRocketTop, sp + kRocketAccel * sdt);
      p.vel = dir * sp;
      const f32 trav = sp * sdt;
      rocket_smoke_emit(p, now);
      const RayResult r = cast(world_->physics(), p.pos, dir, trav + 0.2f, p.ignore);
      if (r.hit && r.distance <= trav + 0.2f) { at = r.point; return true; }
      Vec3 seg_to = p.pos + dir * trav;
      if (people_.shot(p.pos, seg_to, 0, false, now, -1, 0) >= 0) { at = seg_to; return true; }  // organic
      if (soft_target_near(p.pos)) { at = p.pos; return true; }
      for (const PoliceDrone& d : pdrones_)
        if (d.active && kern::length(d.pos - p.pos) < 1.2f + trav) { at = d.pos; return true; }
      const Vec3 next = p.pos + dir * trav;
      if (next.z <= city_.ground_height(next.x, next.y) + 0.08f) { at = next; return true; }
      p.pos = next;
      at = next;
      continue;
    }
    if (p.resting) { p.pos.z = city_.ground_height(p.pos.x, p.pos.y) + kGrenadeR; break; }
    p.vel.z -= kGrenadeGravity * sdt;
    f32 speed = kern::length(p.vel);
    if (speed < 0.05f) {
      const f32 ground = city_.ground_height(p.pos.x, p.pos.y);
      if (p.pos.z > ground + kGrenadeR + 0.08f) p.vel.z = -1.15f;
      else { p.pos.z = ground + kGrenadeR; p.vel = {}; p.resting = true; break; }
      speed = kern::length(p.vel);
    }
    const Vec3 dir = p.vel * (1.0f / std::fmax(0.001f, speed));
    const f32 travel = std::fmax(0.001f, speed * sdt);
    if (now >= p.next_collision) {
      const RayResult r = cast(world_->physics(), p.pos, dir, travel + kGrenadeR + 0.035f, p.ignore);
      if (r.hit && r.distance <= travel + kGrenadeR + 0.018f) {
        // grenadeImpactMode: a car / drone is a "target", an upward normal (z >= .58) the "floor",
        // anything else a "wall"; a launcher round goes off on all but the floor, a hand grenade bounces
        const bool floor = r.normal.z >= kFloorNormalZ && !car_near_point(r.point);
        if (!hand && !floor) { at = r.point; return true; }
        grenade_bounce(p, r.point, r.normal, now);
        continue;
      }
    }
    {
      // organic: a person on the way sets the round off - a hand grenade too (it does not fly
      // through a body: hitting one it goes off there)
      Vec3 seg_to = p.pos + dir * (travel + kGrenadeR);
      if (people_.shot(p.pos, seg_to, 0, false, now, -1, 0) >= 0) { at = seg_to; return true; }
    }
    if (!hand) {
      // a police drone or a soft target on the way sets a launcher round off
      if (soft_target_near(p.pos)) { at = p.pos; return true; }
      for (const PoliceDrone& d : pdrones_)
        if (d.active && kern::length(d.pos - p.pos) < 1.2f + travel) { at = d.pos; return true; }
    }
    Vec3 next = p.pos + dir * travel;
    const f32 floor_z = city_.ground_height(next.x, next.y) + kGrenadeR;
    if (next.z <= floor_z) {
      next.z = floor_z;
      const f32 horizontal = std::hypot(p.vel.x, p.vel.y), down = std::fabs(p.vel.z);
      if (p.bounces >= 3 && down < 1.15f && horizontal < 1.35f) { p.pos = next; p.vel = {}; p.resting = true; break; }
      grenade_bounce(p, next, {0, 0, 1}, now);
      continue;
    }
    p.pos = next;
  }
  // air-stall recovery: hanging slow above the ground falls at least 1.15 m/s
  if (p.id != Id::rocket && !p.resting && p.pos.z > city_.ground_height(p.pos.x, p.pos.y) + kGrenadeR + 0.12f && kern::length(p.vel) < 0.6f)
    p.vel.z = std::fmin(p.vel.z, -1.15f);
  at = p.pos;
  return false;
}

bool Game::foot_remote_detonate(f32 now) {
  // launchFootGrenade: while an own round flies or bounces (launcher or rocket, not a hand grenade)
  // the next press fires the clacker: all of them go off now, the launcher is ready 380 ms later
  bool any = false;
  for (Projectile& p : projectiles_)
    if (p.alive && p.by_player && (p.id == Id::grenade_launcher || p.id == Id::rocket)) { p.fuse = 0; p.born = now - 1; any = true; }
  if (any) next_attack_ = now + weapons::spec(Id::grenade_launcher).interval_ms / 1000 * 0.5f;
  return any;
}

void Game::blast_police_drones(Vec3 at, f32 radius, f32 damage, bool by_player) {
  // dispatchExplosion: a police drone in the blast takes min(3, round(amount / 34)) hits of 34
  // (amount = damage x falloff x exposure; registerPoliceHit, wanted_police_drones.mjs)
  const f32 now = world_->sim_time();
  for (PoliceDrone& d : pdrones_) {
    if (!d.active) continue;
    const f32 dd = kern::length(d.pos - at);
    if (dd >= radius) continue;
    const f32 amount = damage * weapons::blast_factor(dd, radius, !line_of_sight(at, d.pos));
    const f32 hits = std::fmin(3.0f, std::fmax(0.0f, std::round(amount / 34)));
    if (hits <= 0) continue;
    d.hp -= 34 * hits;
    if (d.hp <= 0) {
      d.active = false;
      d.destroyed_until = now + 9;
      if (by_player) wanted_.report(wanted::Crime::police_drone, true);
    }
  }
}

Vec3 Game::rocket_laser_dot() {
  // updateRocketLaser: pinned point > the finger's ray > the crosshair ray; hit within 420 m, else its end
  bool flying = false;
  for (const Projectile& pr : projectiles_) flying = flying || (pr.alive && pr.by_player && pr.id == Id::rocket && pr.guided);
  if (!flying) laser_pinned_ = false;
  if (laser_pinned_) { laser_valid_ = true; laser_dot_ = laser_pin_; return laser_dot_; }
  Vec3 from, ad;
  if (engine_ && engine_->xr) { from = engine_->xr_eye; ad = engine_->xr_forward; }  // VR: where you look steers the rocket
  else if (laser_finger_) ctl_screen_ray(laser_fx_, laser_fy_, from, ad);
  else { ad = kern::normalize(cam_.target - cam_.eye, {1, 0, 0}); from = cam_.eye; }
  const RayResult r = cast(world_->physics(), from, ad, kLaserRange, body(soldier_));
  laser_valid_ = r.hit;
  laser_dot_ = r.hit ? r.point : from + ad * kLaserRange;
  return laser_dot_;
}

void Game::foot_rocket_launched() {
  // a new rocket takes the laser over (never blocked): the pinned point goes
  laser_pinned_ = false;
  // launchFootRocket: a new rocket takes the laser over, the one still in the air flies on unguided
  for (std::size_t i = 0; i + 1 < projectiles_.size(); ++i)
    if (projectiles_[i].id == Id::rocket && projectiles_[i].by_player) projectiles_[i].guided = false;
  if (!projectiles_.empty() && projectiles_.back().id == Id::rocket) projectiles_.back().guided = true;
}

void Game::rocket_smoke_emit(Projectile& p, f32 now) {
  // emitRocketSmoke: every 25 ms a puff .42 m behind the tail (+-2.5 cm), life 1.1 .. 1.7 s
  if (!gaps_ || now - p.smoke_at < 0.025f) return;
  p.smoke_at = now;
  GapsState& g = *gaps_;
  GapsState::RocketPuff& r = g.rocket_smoke[g.rocket_cursor++ % g.rocket_smoke.size()];
  const f32 l = std::fmax(0.001f, kern::length(p.vel));
  r.p = p.pos - p.vel * (0.42f / l) + Vec3{(gaps::unit_hash(g.rocket_cursor * 3 + 1) - 0.5f) * 0.05f, (gaps::unit_hash(g.rocket_cursor * 3 + 2) - 0.5f) * 0.05f, 0};
  r.born = now;
  r.life = 1.1f + gaps::unit_hash(g.rocket_cursor * 3 + 3) * 0.6f;
  r.rot = gaps::unit_hash(g.rocket_cursor * 5 + 4) * 6.28f;
}

void Game::foot_rounds_update(f32 dt) {
  if (!gaps_ || !world_) return;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  // gameplay_final_runtime_v2.mjs has no magazines (SMG every 55 ms, Glock 110 ms, Sniper 1150 ms
  // for as long as the trigger is held): Kern's soldier gun is kept full, it never reloads
  if (mode_ == Mode::foot && !game_weapon_in_hand()) {
    kern::game::SoldierState st;
    if (world_->soldier_state(soldier_, st) && st.magazine > 0 && st.ammo >= 0 && st.ammo < st.magazine) {
      st.ammo = st.magazine;
      world_->apply_soldier_state(st);
    }
  }
  // the player's shots on foot: Kern's soldier, or the game's touch shot through the finger (from the eye)
  auto own = [&](const kern::game::ShotEvent& s) { return s.shooter == soldier_ || (s.shooter == 0 && mode_ == Mode::foot && kern::length(s.from - cam_.eye) < 0.5f); };
  for (const kern::game::ShotEvent& s : world_->shots())
    if (own(s)) ++g.foot_shots;
  // updateRocketSmoke: rises .004 per frame (60 fps)
  for (GapsState::RocketPuff& r : g.rocket_smoke) {
    if (r.born < 0) continue;
    if (now - r.born >= r.life) { r.born = -1; continue; }
    r.p.z += 0.004f * 60 * dt;
  }
  // shotThroughGrenades: an own round passing within .35 m of a live grenade sets it off
  for (const kern::game::ShotEvent& s : world_->shots()) {
    if (!own(s)) continue;
    const Vec3 seg = s.to - s.from;
    const f32 l2 = kern::dot(seg, seg);
    if (l2 < 1e-6f) continue;
    Projectile* best = nullptr;
    f32 bt = 2;
    for (Projectile& p : projectiles_) {
      if (!p.alive || !foot_round(p.id)) continue;
      const f32 t = clampf(kern::dot(p.pos - s.from, seg) / l2, 0, 1);
      if (kern::length(s.from + seg * t - p.pos) <= kShotHit && t < bt) { bt = t; best = &p; }
    }
    if (best) { best->fuse = 0; best->born = now - 1; }
  }
  // knifeAt: the blade connects at the strike frame, 130 ms after the press, along the ray then
  if (g.knife_due >= 0 && now >= g.knife_due) {
    g.knife_due = -1;
    const bool was = ctl_.aim_point;
    const f32 wx = ctl_.aim_x, wy = ctl_.aim_y;
    ctl_.aim_point = g.knife_at_point;
    ctl_.aim_x = g.knife_x;
    ctl_.aim_y = g.knife_y;
    if (mode_ == Mode::foot && !player_dead_ && foot_weapon_ == Id::knife && melee_hit(kKnifeReach + 0.3f, 1.2f, kKnifeDmg, true, 12)) {  // knifeStrike momentumNs 12
      osfx("gameplay_final_runtime_v2.knifeHit", cam_.eye, 1);
      // the hit is felt: a red spray where the blade went in, a sharper jolt of the view
      Vec3 dir;
      const Vec3 eye = aim_ray(dir);
      world_->particle_burst(eye + dir * 1.2f, {0.55f, 0.02f, 0.02f});
      look_.shake.add(0.22f, 0.3f);
    }
    ctl_.aim_point = was;
    ctl_.aim_x = wx;
    ctl_.aim_y = wy;
  }
}

void Game::gaps_sniper_rounds(const std::vector<kern::game::ShotEvent>& shots, std::vector<kern::game::ShotEvent>& arrived, f32 now) {
  // sniperShotAt atArrival: the round's effects at distance / 850 m/s (handle_shots, game_town.cpp)
  if (!gaps_) {
    for (const kern::game::ShotEvent& s : shots)
      if (s.shooter == soldier_ && mode_ == Mode::foot && foot_weapon_ == Id::sniper) arrived.push_back(s);
    return;
  }
  GapsState& g = *gaps_;
  for (const kern::game::ShotEvent& s : shots)
    if (s.shooter == soldier_ && mode_ == Mode::foot && foot_weapon_ == Id::sniper) g.sniper_rounds.push_back({s, now + kern::length(s.to - s.from) / 850.0f});
  for (const GapsState::SniperRound& r : g.sniper_rounds)
    if (now >= r.due) arrived.push_back(r.s);
  std::erase_if(g.sniper_rounds, [&](const GapsState::SniperRound& r) { return now >= r.due; });
}

bool Game::melee_hit(f32 reach, f32 sweep, f32 damage, bool knife, f32 momentum_ns) {
  const f32 now = world_->sim_time();
  Vec3 dir;
  const Vec3 eye = aim_ray(dir);
  Vec3 to = eye + dir * reach;
  // a mate in front takes the blow first (his machine applies it: the authority approves)
  {
    Vec3 mt = to;
    if (hit_remotes(eye, mt, damage, true)) return true;
  }
  i32 hit = people_.shot(eye, to, damage, true, now, momentum_ns);
  if (hit < 0 && life_punch(eye, to)) hit = 0;  // animals (M3 life)
  if (hit < 0) {
    // meleeFallback: nobody on the line - whoever stands in front within reach (+.35 for a person),
    // nearest in the ~40 deg cone (cos .76) of the view, takes it
    const Vec3 me = state_of(soldier_).position;
    const f32 fl = std::hypot(dir.x, dir.y);
    if (fl > 1e-4f) {
      const f32 fx = dir.x / fl, fy = dir.y / fl;
      i32 best = -1;
      f32 bd = 1e9f;
      const auto& all = people_.all();
      for (std::size_t i = 0; i < all.size(); ++i) {
        const People::Person& p = all[i];
        if (p.state == People::State::down || p.state == People::State::gone || p.knocked_until > now) continue;
        const f32 dx = p.pos.x - me.x, dy = p.pos.y - me.y, d = std::hypot(dx, dy);
        if (d > sweep + 0.35f || d < 0.05f || (dx * fx + dy * fy) / d < 0.76f) continue;
        if (d < bd) { bd = d; best = static_cast<i32>(i); }
      }
      if (best >= 0) {
        const Vec3 chest = all[static_cast<std::size_t>(best)].pos + Vec3{0, 0, 1.1f};
        Vec3 aim = kern::normalize(chest - eye, dir);
        Vec3 end = chest + aim * 0.6f;
        hit = people_.shot(eye, end, damage, true, now, momentum_ns);
      }
    }
  }
  if (hit < 0) {
    // punchAt / knifeStrike: a loose body in reach (crate, drum, cone, car) takes the blow's momentum
    // (pushHitBody: dir x momentumNs at the hit point)
    const kern::physics::RayHit h = world_->physics().cast_ray(eye, dir * reach, ~0ull, body(soldier_));
    if (h.hit && h.body.valid()) world_->physics().apply_linear_impulse(h.body, dir * momentum_ns, h.point);
  }
  world_->play_sound(hit >= 0 ? combat_sfx(combat_audio::Kind::hit) : "builtin:click", eye + dir, 0.7f, knife ? 1.3f : 0.8f, true);
  return hit >= 0;
}

void Game::melee_attack() {
  if (!gaps_) return;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  if (foot_weapon_ == Id::knife) {
    // knifeAt: 380 ms recover; the swing starts now, the hit lands at the strike frame
    if (now - g.knife_last < kKnifeRecover) return;
    g.knife_last = now;
    g.knife_due = now + kKnifeStrike;
    // Call of Duty's knife: a target within 3 m ahead (a 35 degree cone) draws the player a short
    // lunge onto it - the blade connects at the strike frame - and the view gives a little
    {
      Vec3 dir;
      const Vec3 eye = aim_ray(dir);
      const Vec3 me = state_of(soldier_).position;
      const f32 fl = std::hypot(dir.x, dir.y);
      f32 best = 3.0f;
      Vec3 to{};
      for (const People::Person& p : people_.all()) {
        if (p.state == People::State::down || p.state == People::State::gone) continue;
        const f32 dx = p.pos.x - me.x, dy = p.pos.y - me.y, d = std::hypot(dx, dy);
        if (d < 0.6f || d > best || fl < 1e-4f || (dx * dir.x + dy * dir.y) / (d * fl) < 0.82f) continue;
        best = d;
        to = {dx / d, dy / d, 0};
      }
      if (best < 3.0f) {
        const Vec3 v = world_->actor_velocity(soldier_);
        const f32 sp = std::fmin(9.0f, (best - 0.9f) / kKnifeStrike);
        world_->set_actor_velocity(soldier_, {to.x * sp, to.y * sp, v.z});
      }
      look_.shake.add(0.12f, 0.2f);
      (void)eye;
    }
    g.knife_at_point = ctl_.aim_point;
    g.knife_x = ctl_.aim_x;
    g.knife_y = ctl_.aim_y;
    look_.vm_fire_at = now;  // weaponFired("knife"): the slash across the view (game_look.cpp)
    osfx("gameplay_final_runtime_v2.knifeSwish", cam_.eye);
    return;
  }
  // punchAt: two hands, each with its own recover (right 300 ms, left 250 ms); the other hand within
  // 430 ms makes a combo (damage x 1.35 rounded). LMB / LT the left hand (1), RMB / RT / touch the right (0).
  u8 hands[2];
  u32 n = 0;
  if (ctl_.lmb_hit) hands[n++] = 1;
  if (ctl_.rmb_hit) hands[n++] = 0;
  if (n == 0 && ctl_.aim_point && ctl_.aim_hand >= 0) hands[n++] = static_cast<u8>(ctl_.aim_hand);  // touch: the left half the left hand
  if (n == 0) hands[n++] = ctl_.pad_lt_fire_was && !ctl_.pad_fire_was ? 1 : 0;
  for (u32 k = 0; k < n; ++k) {
    const u8 h = hands[k];
    if (now - g.fist_last[h] < kFistRecover[h]) continue;
    const bool combo = now - g.fist_last[1 - h] < kCombo;
    g.fist_last[h] = now;
    look_.vm_fire_at = now;  // weaponFired("fists", {hand}): that hand jabs (game_look.cpp)
    look_.vm_punch = h == 0 ? 1 : 0;
    // PUNCH_NS [40, 26] x 1.2 on a combo
    osfx("gameplay_final_runtime_v2.punchWhoosh", cam_.eye, h, combo ? 1 : 0);
    if (melee_hit(kPunchReach + 0.4f, 1.25f, std::round(kFistDmg[h] * (combo ? 1.35f : 1.0f)), false, (h ? 26.0f : 40.0f) * (combo ? 1.2f : 1.0f)))
      osfx("gameplay_final_runtime_v2.punchThud", cam_.eye, h, combo ? 1 : 0);
  }
}

}  // namespace sc
