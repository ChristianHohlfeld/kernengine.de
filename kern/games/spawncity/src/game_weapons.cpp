// SpawnCity on Kern: the weapon set (weapons.h) - on foot the hitscan guns are Kern's soldier
// weapons (MP, Glock, Sniper with SpawnCity's numbers), launchers, melee and the hand grenade are
// the game's; the drone has its gun, rockets and GUIDED, the jet its cannon, missile and bombs
// (gameplay_final_runtime_v2.mjs, hand_grenade.mjs, drone_weapon_modes.mjs, jet_mode.mjs).
#include <algorithm>
#include <cmath>

#include "game.h"
#include "person_hit.h"

namespace sc {

using weapons::Id;

namespace {
constexpr f32 kGrenadeFuse = 4.0f, kGrenadeThrow = 14;  // hand_grenade.mjs FUSE_MS 4000
constexpr f32 kGuidedTurnG = 2.2f;                      // guided_balance.mjs turnG: lateral acceleration limit
bool hitscan_on_foot(Id id) { return id == Id::smg || id == Id::glock || id == Id::sniper; }

struct Loadout { Id id; i32 magazine, reserve; };
// Magazines and spare rounds on foot (gameplay_final_runtime_v2.mjs WEAPONS).
constexpr Loadout kFoot[] = {{Id::smg, 30, 150}, {Id::glock, 17, 85}, {Id::sniper, 5, 25}, {Id::grenade_launcher, 6, 18},
                             {Id::rocket, 1, 6}, {Id::fists, -1, -1}, {Id::knife, -1, -1}};
}  // namespace

void Game::init_weapons() {
  for (const Loadout& l : kFoot) {
    ammo_mag_[static_cast<u8>(l.id)] = l.magazine;
    ammo_reserve_[static_cast<u8>(l.id)] = l.reserve;
  }
  grenades_ = 4;
  foot_weapon_ = Id::glock;  // SpawnCity starts with the pistol
}

void Game::select_foot_weapon(Id id) {
  if (id == foot_weapon_) return;
  // Keep the rounds of the gun we put away.
  kern::game::SoldierState st;
  if (world_->soldier_state(soldier_, st) && hitscan_on_foot(foot_weapon_) && st.ammo >= 0) {
    ammo_mag_[static_cast<u8>(foot_weapon_)] = st.ammo;
    ammo_reserve_[static_cast<u8>(foot_weapon_)] = st.reserve;
  }
  foot_weapon_ = id;
  const weapons::Spec& s = weapons::spec(id);
  using kern::json::Value;
  auto set = [&](const char* key, Value v) { world_->set_property(soldier_, std::string("components.weapon.") + key, v); };
  // The view model: Kern's kinds; launchers look like the shotgun, melee like the pistol (the
  // game fires those itself, so Kern's trigger is kept off while they are in hand).
  const char* kind = id == Id::smg ? "rifle" : id == Id::sniper ? "sniper" : (id == Id::grenade_launcher || id == Id::rocket) ? "shotgun" : "pistol";
  set("kind", Value(kind));
  if (hitscan_on_foot(id)) {
    set("damage", Value(static_cast<f64>(s.damage)));
    set("fireRate", Value(60000.0 / static_cast<f64>(s.interval_ms)));
    set("automatic", Value(id == Id::smg));
    set("range", Value(static_cast<f64>(s.range_m)));
    set("headshot", Value(3.0));  // VS zones: head x3
    for (const Loadout& l : kFoot)
      if (l.id == id) set("magazine", Value(static_cast<f64>(l.magazine)));
    set("fireSound", Value(id == Id::smg ? "sc:shot0" : id == Id::sniper ? "builtin:sniper" : "sc:pistol0"));
    set("pellets", Value(1.0));
    // first_person_controller_v5.mjs ADS: x 1.45 for the guns, the sniper's scope x 8 (true
    // magnification through tan; Kern zooms by tan(fov / 2) / tan(aimFov / 2) from the 59 deg view)
    set("aimFov", Value(id == Id::sniper ? 8.09 : 42.6));
    // the original's rounds go exactly through the crosshair / the finger: no cone, no bloom, no
    // view kick beyond the camera's own shot impulse (fps_control_math.mjs addFpsShotImpulse)
    set("spread", Value(0.0));
    set("aimSpread", Value(0.0));
    set("recoil", Value(0.0));
    if (world_->soldier_state(soldier_, st)) {
      st.ammo = ammo_mag_[static_cast<u8>(id)];
      st.reserve = ammo_reserve_[static_cast<u8>(id)];
      world_->apply_soldier_state(st);
    }
  }
  toast_ = std::string("Waffe: ") + s.name;
  toast_age_ = 0;
  next_attack_ = std::max(next_attack_, world_->sim_time() + 0.25f);
}

void Game::cycle_foot_weapon(i32 step) {
  const i32 n = static_cast<i32>(sizeof kFoot / sizeof kFoot[0]);
  i32 cur = 0;
  for (i32 i = 0; i < n; ++i)
    if (kFoot[i].id == foot_weapon_) cur = i;
  select_foot_weapon(kFoot[((cur + step) % n + n) % n].id);
}

bool Game::game_weapon_in_hand() const { return mode_ == Mode::foot && !hitscan_on_foot(foot_weapon_); }

// ---------------------------------------------------------------------------------------------
// hits

Vec3 Game::aim_ray(Vec3& dir) const {
  // touch: the shot goes through the finger (foot_touch_controller.mjs fireAt, game_controls.cpp)
  if (ctl_.aim_point || (engine_ && engine_->xr)) { Vec3 from; ctl_screen_ray(ctl_.aim_x, ctl_.aim_y, from, dir); return from; }
  dir = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
  return cam_.eye;
}

bool Game::hit_scan(Vec3 from, Vec3 dir, f32 range, f32 damage, bool by_player, kern::physics::Body ignore, kern::game::ShotEvent& out) {
  const f32 now = world_->sim_time();
  const RayResult r = cast(world_->physics(), from, dir, range, ignore);
  out = {};
  out.from = from;
  out.to = r.hit ? r.point : from + dir * range;
  out.impact = r.hit;
  out.normal = r.normal;
  out.surface = r.hit ? 1 : 0;
  if (by_player) people_.frighten(from, 22, now, 1);
  Vec3 to = out.to;
  // Police drones on the line.
  for (PoliceDrone& d : pdrones_) {
    if (!d.active) continue;
    const Vec3 seg = to - from;
    const f32 t = clampf(kern::dot(d.pos - from, seg) / std::fmax(1e-6f, kern::dot(seg, seg)), 0, 1);
    if (kern::length(from + seg * t - d.pos) < 0.55f) {
      out.to = from + seg * t;
      out.surface = 2;
      out.impact = true;
      d.hp -= damage > 60 ? damage : 34;  // wanted_police_drones.mjs POLICE_HIT_DAMAGE
      if (d.hp <= 0) {
        osfx("wanted_police_drones.down", d.pos);
        explosion(d.pos, 3, 40, by_player);
        if (by_player) wanted_.report(wanted::Crime::police_drone, true);
        d.active = false;
        d.destroyed_until = now + 9;
      }
      return true;
    }
  }
  i32 person = -1;
  bool remote = false;
  shot_flesh_ = false;
  // the pilot standing below his drone: your own rounds hurt him as anyone's (the original's
  // self-hit; his body is the soldier's capsule, the hit is by the person's pose)
  if (by_player && mode_ == Mode::drone && !player_dead_ && world_->actor_alive(soldier_)) {
    const Vec3 c = matrix_of(soldier_).translation();
    f32 t = 2;
    const BodyPart part = person_hit::hit(c - Vec3{0, 0, 0.73f}, world_->camera_yaw() * kDeg, 0, from, to - from, t);
    if (part != BodyPart::none) {
      out.to = from + (to - from) * t;
      out.surface = 3;
      out.impact = true;
      shot_flesh_ = true;
      world_->damage(soldier_, damage * (part == BodyPart::head ? 2.0f : part == BodyPart::legs ? 0.7f : 1.0f), 0, false, from);
      return true;
    }
  }
  if ((by_player && (remote = hit_remotes(from, to, damage))) || (person = people_.shot(from, to, damage, by_player, now)) >= 0 || (by_player && life_shot(from, to, damage))) {  // life_shot: M3 life
    shot_flesh_ = remote || person >= 0;  // world_action_feedback.mjs materialKind: person / player -> flesh
    if (person >= 0 && by_player) osfx("gameplay_polish_lite.playPersonGroan", to);  // a person hit (registerWorldPopulationHit)
    // a touch shot with the Glock or the sniper goes through bodies too (glockShotAt / sniperShotAt:
    // up to three, 0.6 m on past each), as handle_shots does for Kern's own rounds
    if (by_player && mode_ == Mode::foot && (foot_weapon_ == Id::glock || foot_weapon_ == Id::sniper)) {
      const Vec3 end = r.hit ? r.point : from + dir * range;
      Vec3 at = to + dir * 0.6f;
      for (int pierced = 1; pierced < 3 && kern::dot(end - at, dir) > 0.5f; ++pierced) {
        Vec3 next = end;
        if (!(hit_remotes(at, next, damage) || people_.shot(at, next, damage, true, now) >= 0 || life_shot(at, next, damage))) break;
        at = next + dir * 0.6f;
      }
    }
    out.to = to;
    out.surface = 3;
    out.impact = true;
    return true;
  }
  if (r.hit) {
    // A car? (any body of a car, by its footprint)
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      const TrafficCar& c = cars_[i];
      if (!c.placed || c.wreck) continue;
      const BodyState b = state_of(c.actor);
      if (kern::length(b.position - r.point) > 6) continue;
      const Vec3 local = to_local(b.rotation, r.point - b.position);
      const Vec3 h = c.car->spec().half;
      if (std::fabs(local.x) < h.x + 0.4f && std::fabs(local.y) < h.y + 0.4f && std::fabs(local.z) < 2.0f) {
        car_bullet(static_cast<i32>(i), by_player);  // a round per kind: 22 / 30 / 26, sniper share (game_vehicle_damage.cpp)
        out.surface = 2;
        break;
      }
    }
  }
  return r.hit;
}

void Game::fire_projectile(Id id, Vec3 from, Vec3 vel, bool by_player, kern::physics::Body ignore) {
  Projectile p;
  p.id = id;
  p.pos = from;
  p.vel = vel;
  p.born = world_->sim_time();
  p.by_player = by_player;
  p.ignore = ignore;
  p.gravity = id == Id::grenade_launcher || id == Id::hand_grenade || id == Id::jet_bomb;
  // GRENADE_FUSE_MS 2800 / ROCKET_FOOT_TTL_MS 9000 (gameplay_final_runtime_v2.mjs)
  p.fuse = id == Id::jet_missile ? 5.0f /* jet_mode.mjs life 5 s */ : id == Id::hand_grenade ? kGrenadeFuse : id == Id::drone_rockets ? 7.2f /* direct_fire_missiles.mjs TTL */ : id == Id::grenade_launcher ? 2.8f : id == Id::rocket ? 9.0f : 12;
  if (id == Id::hand_grenade) p.next_collision = p.born + 0.06f;  // throwHandGrenade nextCollisionAt +60 ms
  p.guided = id == Id::drone_guided;
  p.net = ++proj_serial_ & 0x7fff;
  projectiles_.push_back(p);
  const char* snd = id == Id::hand_grenade || id == Id::jet_bomb ? "builtin:jump" : "builtin:launcher";
  world_->play_sound(snd, from, 0.9f, id == Id::jet_missile ? 0.8f : 1.0f, true);
  osfx("launch", from, static_cast<f32>(static_cast<i32>(id)), by_player ? 1 : 0);  // audio.mjs: the weapon's own launch sound
  if (by_player) people_.frighten(from, 18, world_->sim_time(), 1);
}

void Game::update_projectiles(f32 dt) {
  if (projectiles_.empty()) return;
  const f32 now = world_->sim_time();
  Vec3 aim_dir;
  const Vec3 aim_from = aim_ray(aim_dir);
  for (Projectile& p : projectiles_) {
    if (!p.alive || p.ghost) continue;  // (a mate's round: moved by its stream, game_projsync.cpp)
    bool boom = false;
    Vec3 at = p.pos;
    if (p.id == Id::grenade_launcher || p.id == Id::rocket || p.id == Id::hand_grenade) {
      boom = foot_round_step(p, dt, now, at);  // gameplay_final_runtime_v2.mjs (game_foot_rounds.cpp)
      p.pos = at;
      goto settle;
    }
    if (p.guided && mode_ == Mode::drone) {
      // GUIDED: the missile turns towards where the camera looks.
      const RayResult r = cast(world_->physics(), aim_from, aim_dir, 600, body(drone_actor_));
      const Vec3 goal = r.hit ? r.point : aim_from + aim_dir * 600.0f;
      const f32 speed = kern::length(p.vel);
      const Vec3 want = kern::normalize(goal - p.pos, p.vel * (1.0f / std::fmax(1e-3f, speed)));
      const Vec3 cur = p.vel * (1.0f / std::fmax(1e-3f, speed));
      const f32 max_turn = kGuidedTurnG * kG / std::fmax(1.0f, speed) * dt;  // rad this step
      const f32 ang = std::acos(clampf(kern::dot(cur, want), -1, 1));
      const Vec3 turned = ang <= max_turn || ang < 1e-5f ? want : kern::normalize(cur + (want - cur) * (max_turn / ang), cur);
      p.vel = turned * speed;
    }
    if (p.gravity) p.vel.z -= kG * dt;
    if (p.id == Id::jet_missile) p.vel.z -= kG * 0.15f * dt;    // jet_mode.mjs stepRockets: G * .15
    if (p.id == Id::jet_bomb) p.vel = p.vel * (1 - 0.04f * dt);  // jet_mode.mjs stepBombs: drag .04 / s
    {
    Vec3 step = p.vel * dt;
    const f32 len = kern::length(step);
    boom = now - p.born > p.fuse;
    at = p.pos + step;
    if (len > 1e-5f) {
      const RayResult r = cast(world_->physics(), p.pos, step * (1.0f / len), len, p.ignore);
      if (r.hit) {
        boom = true;
        at = r.point - step * (0.05f / len);
      }
      // A person or a police drone in the way.
      Vec3 seg_to = p.pos + step;
      if (!boom && people_.shot(p.pos, seg_to, 0, false, now, -1, 0) >= 0) { boom = true; at = seg_to; }
      if (!boom && p.by_player && round_meets_mate(p.pos, seg_to, at)) boom = true;  // a mate's body or vehicle
      if (!boom)
        for (const PoliceDrone& d : pdrones_)
          if (d.active && kern::length(d.pos - (p.pos + step * 0.5f)) < 1.2f + len * 0.5f) { boom = true; at = d.pos; }
    }
    p.pos = at;
    }
  settle:
    if (boom || at.z < city_.ground_height(at.x, at.y) - 5 || now - p.born > 20) {
      const weapons::Spec& s = weapons::spec(p.id);
      // a jet rocket whose 5 s run out is gone without a blast (stepRockets: blast only on a hit)
      const bool spent = p.id == Id::jet_missile && now - p.born > p.fuse;
      if (!spent && s.radius_m > 0 && at.z > city_.ground_height(at.x, at.y) - 5) explosion(at, s.radius_m, s.damage, p.by_player, BlastBy::weapon);
      p.alive = false;
    }
  }
  projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), [](const Projectile& p) { return !p.alive; }), projectiles_.end());
}

// ---------------------------------------------------------------------------------------------
// attacks per mode

void Game::foot_attack(bool trigger, bool pressed) {
  const f32 now = world_->sim_time();
  if (gaps_riding() && !(gaps_ && gaps_->ride_car)) return;  // a jet's passenger has nothing to fire; a car's does (game_seats.cpp)
  // launchFootGrenade: the clacker comes before the cooldown - a press while a round flies sets it off
  if (trigger && pressed && !player_dead_ && foot_weapon_ == Id::grenade_launcher && foot_remote_detonate(now)) return;
  if (!trigger || now < next_attack_ || player_dead_) return;
  const weapons::Spec& s = weapons::spec(foot_weapon_);
  Vec3 dir;
  const Vec3 eye = aim_ray(dir);
  const u8 slot = static_cast<u8>(foot_weapon_);
  switch (foot_weapon_) {
    case Id::fists:
    case Id::knife: {
      if (!pressed) return;  // one blow per click
      melee_attack();  // two fists with combos, the knife's strike frame (game_foot_rounds.cpp)
      return;
    }
    case Id::grenade_launcher:
    case Id::rocket: {
      if (!pressed) return;
      // gameplay_final_runtime_v2.mjs has no magazines: only the cooldown (760 / 900 ms) gates a shot
      (void)slot;
      next_attack_ = now + s.interval_ms / 1000;
      // footMuzzle: the launcher's muzzle (its view model held right of the view, game_look.cpp:
      // .245 / -.2 / -.47 + the barrel's mouth .03 up, .81 forward), then .10 (grenade) / .25 (rocket)
      // along the shot's ray - the ray itself goes through the finger / the crosshair (footRay)
      (void)eye;
      const Vec3 cf = kern::normalize(cam_.target - cam_.eye, {1, 0, 0}), cr = kern::normalize(kern::cross(cf, cam_.up), {0, -1, 0}), cu = kern::cross(cr, cf);
      kern::Mat4 vrh;
      const bool vrm = vr_hand_matrix(1, vrh);
      const Vec3 muzzle = vrm ? vrh.translation() + kern::normalize(Vec3{-vrh.m[8], -vrh.m[9], -vrh.m[10]}, dir) * (foot_weapon_ == Id::rocket ? 0.75f : 0.85f)  // VR: the launcher's mouth in the hand, along its ray
                        : foot_weapon_ == Id::rocket ? cam_.eye + cr * 0.285f - cu * 0.215f + cf * 1.14f + dir * 0.25f  // the new tube's mouth (game_look.cpp)
                                                     : cam_.eye + cr * 0.245f - cu * 0.17f + cf * 1.28f + dir * 0.10f;
      // GRENADE_SPEED_MPS 40 straight along the aim; the rocket leaves at ROCKET_FOOT_LAUNCH_MPS 22
      const Vec3 v = dir * (foot_weapon_ == Id::rocket ? 22.0f : s.speed_mps);
      fire_projectile(foot_weapon_, muzzle, v, true, body(soldier_));
      if (foot_weapon_ == Id::rocket) foot_rocket_launched();
      world_->shake_camera(foot_weapon_ == Id::rocket ? 0.35f : 0.2f);
      return;
    }
    default: return;  // Kern fires the hitscan guns
  }
}

void Game::throw_grenade() {
  const f32 now = world_->sim_time();
  if (grenades_ <= 0 || now < next_grenade_ || player_dead_) {
    toast_ = grenades_ <= 0 ? "Keine Granaten mehr" : "";
    toast_age_ = grenades_ <= 0 ? 0 : 99;
    return;
  }
  --grenades_;
  next_grenade_ = now + weapons::spec(Id::hand_grenade).interval_ms / 1000;
  Vec3 dir;
  const Vec3 eye = aim_ray(dir);
  const Vec3 v = dir * kGrenadeThrow + Vec3{0, 0, 3.5f} + world_->actor_velocity(soldier_);
  fire_projectile(Id::hand_grenade, eye + dir * 0.5f, v, true, body(soldier_));
}

void Game::drone_attack(bool trigger, bool pressed) {
  const f32 now = world_->sim_time();
  if (!drone_out_ || !trigger) return;
  const BodyState s = state_of(drone_actor_);
  Vec3 dir;
  aim_ray(dir);
  // From the nose (-X of the airframe) of the drone as it is drawn (not its physics step: strafing,
  // the round would leave beside the drone), along the camera's aim.
  const Vec3 nose = matrix_of(drone_actor_).transform_point({-0.16f, 0, -0.02f});
  if (drone_weapon_ == 0) {
    const weapons::Spec& g = weapons::spec(Id::drone_gun);
    if (now < next_drone_fire_) return;
    next_drone_fire_ = now + g.interval_ms / 1000;
    kern::game::ShotEvent e;
    hit_scan(nose, dir, g.range_m, g.damage, true, body(drone_actor_), e);
    e.shooter = drone_actor_;
    world_->shots().push_back(e);
    // the original's HUD (flight_fire_fx.mjs): an impact mark, the crosshair's hit confirm on a target
    if (e.impact) ctl_.impacts[ctl_.impact_serial++ % 8] = e.to;
    if (e.surface >= 2) ++ctl_.drone_hits;
    world_->play_sound(combat_sfx(combat_audio::Kind::shot), nose, 0.55f, 1.35f, true);
    osfx("flight_fire_fx.fire", e.to, e.impact ? hit_material(e.to, e.surface, shot_flesh_) : 0);  // shotSound + playShotBody(1) + playImpactSound(materialKind)
  } else if (pressed && drone_weapon_ == 2) {
    guided_press();  // lock on what is nearest the crosshair and launch (game_guided.cpp)
  } else if (pressed) {
    const Id id = drone_weapon_ == 1 ? Id::drone_rockets : Id::drone_guided;
    const weapons::Spec& g = weapons::spec(id);
    if (now < next_drone_fire_) return;
    next_drone_fire_ = now + g.interval_ms / 1000;
    fire_projectile(id, nose + dir * 0.3f, dir * g.speed_mps + s.velocity, true, body(drone_actor_));
  }
}

void Game::jet_attack(bool gun, bool missile, bool bomb) {
  if (!jet_placed_ || mode_ != Mode::jet) return;
  const f32 now = world_->sim_time();
  const BodyState s = state_of(jet_actor_);
  const Vec3 fwd = s.rotation.rotate({0, 1, 0});
  if (gun && now >= next_jet_gun_) {
    const weapons::Spec& g = weapons::spec(Id::jet_cannon);
    next_jet_gun_ = now + g.interval_ms / 1000;
    const Vec3 o = s.position + fwd * 10.5f;
    Vec3 aim = fwd;
    if (ctl_.jet_aim) {
      // jet_mode.mjs tapAim: through the finger, clamped to a 0.62 rad cone around the nose
      Vec3 from;
      ctl_screen_ray(ctl_.aim_x, ctl_.aim_y, from, aim);
      const f32 c = kern::dot(aim, fwd);
      if (c < std::cos(0.62f)) aim = kern::normalize(fwd * std::cos(0.62f) + kern::normalize(aim - fwd * c, fwd) * std::sin(0.62f), fwd);
    }
    Vec3 d = aim + Vec3{rng_.range(-0.003f, 0.003f), rng_.range(-0.003f, 0.003f), rng_.range(-0.003f, 0.003f)};
    d = kern::normalize(d, fwd);
    kern::game::ShotEvent e;
    const bool hit = hit_scan(o, d, g.range_m, g.damage, true, body(jet_actor_), e);
    e.shooter = jet_actor_;
    if (kern::length(e.to - o) > 600) e.to = o + d * 600.0f;
    world_->shots().push_back(e);
    // A third of the rounds burst (jet_mode.mjs: 34 %, 3.2 m, 70).
    if (hit && rng_.unit() < 0.34f) explosion(e.to, 3.2f, 70, true, BlastBy::weapon);  // the jet cannon
    world_->play_sound("builtin:smg", o, 0.5f, 0.7f, true);
    osfx("jet_mode.gun", o);
  }
  if (missile && now >= next_jet_missile_ && jet_.telemetry().contacts == 0) {
    // jet_mode.mjs fireRocket: from the wing pods, left and right in turn (3.2 m out, 2 m ahead,
    // 0.6 m down), along the aim (tapAim) at 260 m/s on top of the jet's velocity; not on the ground
    const weapons::Spec& m = weapons::spec(Id::jet_missile);
    next_jet_missile_ = now + m.interval_ms / 1000;
    Vec3 aim = fwd;
    if (ctl_.jet_aim) {
      Vec3 from;
      ctl_screen_ray(ctl_.aim_x, ctl_.aim_y, from, aim);
      const f32 c = kern::dot(aim, fwd);
      if (c < std::cos(0.62f)) aim = kern::normalize(fwd * std::cos(0.62f) + kern::normalize(aim - fwd * c, fwd) * std::sin(0.62f), fwd);
    }
    jet_rocket_side_ = -jet_rocket_side_;
    fire_projectile(Id::jet_missile, s.position + s.rotation.rotate({jet_rocket_side_ * 3.2f, 0, 0}) + fwd * 2.0f + s.rotation.rotate({0, 0, -0.6f}), aim * m.speed_mps + s.velocity, true, body(jet_actor_));
  }
  if (bomb && now >= next_jet_bomb_ && jet_.telemetry().contacts == 0) {
    next_jet_bomb_ = now + weapons::spec(Id::jet_bomb).interval_ms / 1000;
    fire_projectile(Id::jet_bomb, s.position + s.rotation.rotate({0, 0, -1.6f}), s.velocity, true, body(jet_actor_));
  }
}

void Game::draw_projectiles() {
  for (const Projectile& p : projectiles_) {
    switch (p.id) {
      case Id::hand_grenade: people_art_.projectile(p.pos, p.vel, 0.05f, 0.14f, false); break;
      case Id::grenade_launcher: break;  // the glowing 40 mm round: draw_military (makeGrenade)
      case Id::jet_bomb: people_art_.projectile(p.pos, p.vel, 0.25f, 1.9f, false); break;
      case Id::jet_missile: people_art_.projectile(p.pos, p.vel, 0.12f, 2.8f, true); break;
      default: people_art_.projectile(p.pos, p.vel, 0.06f, 0.7f, true); break;
    }
  }
}

}  // namespace sc
