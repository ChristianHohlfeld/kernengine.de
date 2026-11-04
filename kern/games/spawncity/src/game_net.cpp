// SpawnCity on Kern: multiplayer in the game (see net_session.h) - this player's pose out, the
// others drawn from theirs, hits on them sent to their machines, hits on us applied here.
#include <algorithm>
#include <cmath>

#include "game.h"

namespace sc {

namespace {
// Shirt colours of the players (each slot its own), and the cars they drive.
const Vec3 kPlayerShirts[] = {{0.85f, 0.2f, 0.15f}, {0.15f, 0.45f, 0.85f}, {0.95f, 0.75f, 0.1f}, {0.2f, 0.7f, 0.3f}, {0.7f, 0.3f, 0.8f}, {0.95f, 0.5f, 0.15f},
                              {0.1f, 0.75f, 0.75f}, {0.9f, 0.9f, 0.9f}};
Vec3 shirt_of(u8 c) { return kPlayerShirts[c % 8]; }
u8 color_of_slot(u8 slot) { return static_cast<u8>(slot == 255 ? 0 : (slot + 1) % 8); }
}  // namespace

net::PlayerState Game::my_state() const {
  net::PlayerState s;
  s.mode = static_cast<net::PlayerMode>(mode_);
  s.color = color_of_slot(net_.self_slot());
  const kern::game::ActorId a = mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::jet ? jet_actor_
                                : mode_ == Mode::drone ? drone_actor_
                                : mode_ == Mode::parachute ? chute_actor_
                                                           : soldier_;
  const BodyState b = state_of(a);
  s.position = b.position;
  s.velocity = b.velocity;
  s.rotation = b.rotation;
  s.pilot = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  s.yaw = world_->camera_yaw() * kDeg;
  s.pitch = world_->camera_pitch() * kDeg;
  s.speed = std::hypot(b.velocity.x, b.velocity.y);
  s.health = world_->hud().health;
  s.stars = static_cast<u8>(wanted_.stars());
  s.weapon = static_cast<u8>(foot_weapon_);
  u8 f = 0;
  if (player_dead_) f |= 1;
  if (drone_.telemetry().armed) f |= 2;
  if (mode_ == Mode::car && car_index_ >= 0) {
    if (cars_[car_index_].car->spec().radius > 0.4f) f |= 4;
    if (cars_[car_index_].police) f |= 16;
    s.color = static_cast<u8>(car_index_ % 8);
  }
  if (jet_.gear_down()) f |= 8;
  s.flags = f;
  return s;
}

void Game::rebase_origin(f64 lon, f64 lat) {
  // A client plays in the host's city: the same streamed map from the same origin.
  lon_ = lon;
  lat_ = lat;
  city_.clear();
  city_.start(engine_->renderer, world_->physics(), lon_, lat_);
  if (engine_->arg_number("offline", 0) > 0) city_.go_offline();
  for (TrafficCar& c : cars_) {
    if (c.placed) world_->set_alive(c.actor, false);
    c.placed = false;
    c.unit = -1;
    c.wreck = false;
  }
  units_.clear();
  for (PoliceDrone& d : pdrones_) d.active = false;
  people_.clear();
  projectiles_.clear();
  wanted_.clear();
  if (mode_ != Mode::foot) command("mode foot");
  world_placed_ = false;
  world_->set_paused(true);
  toast_ = "Mit dem Spiel verbunden - die Stadt des Hosts wird geladen";
  toast_age_ = 0;
}

void Game::update_net(f32 dt) {
  if (!net_.active()) return;
  const f64 now = time_;
  net_.update(now, dt, world_placed_ ? my_state() : net::PlayerState{});
  // The client takes the host's map origin once.
  f64 lon, lat;
  if (!net_.hosting() && !rebased_ && net_.origin(lon, lat)) {
    rebased_ = true;
    if (std::fabs(lon - lon_) > 1e-7 || std::fabs(lat - lat_) > 1e-7) rebase_origin(lon, lat);
  }
  // Hits other players landed on us: the owner applies them.
  for (const net::Hit& h : net_.take_hits()) {
    if (!world_placed_ || player_dead_) continue;
    if (mode_ == Mode::foot) world_->damage(soldier_, h.damage, 0, false, h.from);
    else if (mode_ == Mode::parachute) world_->damage(soldier_, h.damage, 0, false, h.from);
    else damage_player_vehicle(h.damage);
  }
  // Present the others: ease towards the pose extrapolated from the last one (100 ms).
  for (net::Remote& r : net_.remotes()) {
    const f32 age = static_cast<f32>(std::fmin(0.5, now - r.received));
    const Vec3 target = r.state.position + r.state.velocity * age;
    if (kern::length(target - r.shown) > 12) r.shown = target;  // a jump (respawn, new vehicle)
    r.shown += (target - r.shown) * damp_alpha(14, dt);
    r.shown_rotation = slerp(r.shown_rotation, r.state.rotation, damp_alpha(14, dt));
    r.phase += r.state.speed * dt * 3.4f;
  }
}

bool Game::hit_remotes(Vec3 from, Vec3& to, f32 damage) {
  if (!net_.active()) return false;
  const Vec3 seg = to - from;
  const f32 l2 = std::fmax(1e-6f, kern::dot(seg, seg));
  i32 best = -1;
  f32 best_t = 2;
  std::vector<net::Remote>& rs = net_.remotes();
  for (std::size_t i = 0; i < rs.size(); ++i) {
    const net::Remote& r = rs[i];
    if (r.state.flags & 1) continue;
    // A capsule on foot, a sphere around a vehicle.
    Vec3 c = r.shown;
    f32 radius = 0.4f;
    switch (r.state.mode) {
      case net::PlayerMode::foot: c = r.shown + Vec3{0, 0, 0.2f}; radius = 0.45f; break;
      case net::PlayerMode::parachute: radius = 0.6f; break;
      case net::PlayerMode::drone: radius = 0.45f; break;
      case net::PlayerMode::car: radius = r.state.flags & 4 ? 3.2f : 2.0f; break;
      case net::PlayerMode::jet: radius = 5.5f; break;
    }
    const f32 t = clampf(kern::dot(c - from, seg) / l2, 0, 1);
    const Vec3 p = from + seg * t;
    // On foot: the closest point anywhere along the body's height (0.73 m below to 1.1 m above the centre).
    f32 d = kern::length(p - c);
    if (r.state.mode == net::PlayerMode::foot) {
      const Vec3 q{c.x, c.y, clampf(p.z, c.z - 0.73f, c.z + 1.1f)};
      d = kern::length(p - q);
    }
    if (d < radius && t < best_t) { best_t = t; best = static_cast<i32>(i); }
  }
  if (best < 0) return false;
  to = from + seg * best_t;
  const net::Remote& r = rs[static_cast<std::size_t>(best)];
  const bool head = r.state.mode == net::PlayerMode::foot && to.z > r.shown.z + 0.75f;
  net_.send_hit({r.state.slot, damage * (head ? 3.0f : 1.0f), from});
  return true;
}

void Game::blast_remotes(Vec3 at, f32 radius, f32 damage) {
  if (!net_.active()) return;
  for (const net::Remote& r : net_.remotes()) {
    if (r.state.flags & 1) continue;
    const f32 d = kern::length(r.shown - at);
    if (d < radius) net_.send_hit({r.state.slot, damage * weapons::blast_factor(d, radius, false), at});
  }
}

void Game::draw_remote_people() {
  if (!net_.active()) return;
  const f32 now = world_->sim_time();
  for (const net::Remote& r : net_.remotes()) {
    People::Person p;
    p.kind = People::Kind::citizen;
    p.shirt = shirt_of(r.state.color);
    p.pants = {0.12f, 0.13f, 0.16f};
    p.skin = {0.86f, 0.66f, 0.5f};
    p.phase = r.phase;
    p.yaw = r.state.yaw;
    if (r.state.flags & 1) {
      p.state = People::State::down;
      p.fall_dir = {std::cos(r.state.yaw), std::sin(r.state.yaw), 0};
    }
    if (r.state.mode == net::PlayerMode::foot) {
      p.pos = r.shown - Vec3{0, 0, 0.73f};
      p.v = r.state.speed;
      p.aiming = r.state.speed < 0.5f;
      people_art_.person(p, now);
    } else if (r.state.mode == net::PlayerMode::drone) {
      p.pos = r.state.pilot;
      p.v = 0;
      p.aiming = true;  // the remote control in both hands
      people_art_.person(p, now);
    }
  }
}

void Game::draw_remote_vehicles() {
  if (!net_.active()) return;
  if (!remote_car_) remote_car_ = std::make_unique<Car>(CarSpec::car());
  if (!remote_bus_) remote_bus_ = std::make_unique<Car>(CarSpec::bus());
  for (const net::Remote& r : net_.remotes()) {
    const kern::Mat4 m = kern::Mat4::trs(r.shown, r.shown_rotation, {1, 1, 1});
    switch (r.state.mode) {
      case net::PlayerMode::car: {
        const bool bus = r.state.flags & 4;
        const Vec3 color = r.state.flags & 16 ? Vec3{0.93f, 0.94f, 0.96f} : bus ? Vec3{0.92f, 0.78f, 0.1f} : shirt_of(r.state.color);
        art_.car(m, bus ? *remote_bus_ : *remote_car_, color, false, false);
        if (r.state.flags & 16) people_art_.light_bar(m, false, world_->sim_time());
        break;
      }
      case net::PlayerMode::jet: art_.jet(m, r.state.flags & 8, 0, clampf(r.state.speed / 120, 0.1f, 1), false); break;
      case net::PlayerMode::drone: {
        Vec3 motors[4];
        f32 angles[4];
        for (int i = 0; i < 4; ++i) {
          motors[i] = drone_.motor_position(i);
          angles[i] = r.state.flags & 2 ? static_cast<f32>(time_) * 160.0f * (i % 2 ? 1.0f : -1.0f) : 0;
        }
        art_.drone(m, motors, angles, drone_.params().prop_d, r.state.flags & 2, false);
        break;
      }
      case net::PlayerMode::parachute: art_.parachute(r.shown - Vec3{0, 0, 0.85f}, r.state.yaw, true); break;
      case net::PlayerMode::foot: break;
    }
  }
}

}  // namespace sc
