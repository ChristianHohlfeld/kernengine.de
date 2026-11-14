// SpawnCity on Kern: cars far from the player as lane followers without physics, as the original
// does (world_procedural_population.mjs PHYS_NEAR_M 150 / PHYS_FAR_M 180: a car only has a physics
// body near the focus; further away it follows its route analytically, and it only becomes
// physical again where no physical car stands - vehicleFootprintsOverlap).
//
// Here a far car keeps its Box3D body as a kinematic one that collides with nothing (its shapes'
// masks are taken away; rays and blasts still find it): it is moved along its lane under the same
// rules as every driver - the lights, the people, the vehicle ahead - and is never something a
// dynamic body could be pressed against. Within 150 m, on ground that stands and where its place
// is free, it is dynamic again: resting on its wheels as the ground lies, at the speed it had.
// Hooks: drive_traffic, manage_traffic.
#include <algorithm>
#include <cmath>

#include "game.h"
#include "spawn_guard.h"
#include "world_traffic.h"

namespace sc {

namespace {
constexpr f32 kPhysNear = 150, kPhysFar = 180;  // PHYS_NEAR_M, PHYS_FAR_M
}

kern::physics::Pose Game::car_rest_pose(const CarSpec& s, f32 x, f32 y, f32 yaw) const {
  // the ground under the four wheels: the axles' mean heights give the pitch, the sides' the roll
  const f32 cy = std::cos(yaw), sy = std::sin(yaw);
  f32 h[4];
  for (int i = 0; i < 4; ++i) {
    const f32 wx = x + cy * s.wheels[i].x - sy * s.wheels[i].y, wy = y + sy * s.wheels[i].x + cy * s.wheels[i].y;
    h[i] = city_.ground_height(wx, wy);
  }
  // (wheels: front left, front right, rear left, rear right - car.h)
  const f32 front = (h[0] + h[1]) / 2, rear = (h[2] + h[3]) / 2, left = (h[0] + h[2]) / 2, right = (h[1] + h[3]) / 2;
  const f32 xf = s.wheels[0].x, xr = s.wheels[2].x, track = s.wheels[0].y - s.wheels[1].y;
  const f32 pitch = std::atan2(front - rear, xf - xr), roll = std::atan2(left - right, track);  // nose up, left side up
  // the ground's height under the chassis' origin; the origin stands over (x, y) at its ride
  // height from the ground's plane (measured along the chassis' up: straight up it is 1 / up.z more)
  const f32 z0 = rear + (front - rear) * (0 - xr) / (xf - xr);
  const Quat q = qnormalize(yaw_quat(yaw) * Quat::axis_angle({0, 1, 0}, -pitch) * Quat::axis_angle({1, 0, 0}, roll));
  return {Vec3{x, y, z0 + Car::ground_offset(s) / std::fmax(0.5f, q.rotate({0, 0, 1}).z)}, q};
}

void Game::traffic_solid(TrafficCar& c) {
  // its shapes collide again with what they did (a car that was far)
  c.far = false;
  if (c.solid_masks.empty()) return;
  const kern::physics::Body b = body(c.actor);
  if (b.valid() && world_->physics().body_valid(b)) world_->physics().set_body_masks(b, kern::Span<const u64>(c.solid_masks.data(), c.solid_masks.size()));
  c.solid_masks.clear();
}

void Game::traffic_make_far(TrafficCar& c) {
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(c.actor);
  if (!b.valid() || !w.body_valid(b)) { c.far = false; return; }
  if (c.solid_masks.empty()) {
    w.body_masks(b, c.solid_masks);
    const u64 nothing = kCatQuery;  // (no shape is of this category: found by rays and blasts, touched by nothing)
    w.set_body_masks(b, kern::Span<const u64>(&nothing, 1));
  }
  if (w.body_type(b) != kern::physics::BodyType::kinematic) w.set_body_type(b, kern::physics::BodyType::kinematic);
  w.set_linear_velocity(b, {});
  w.set_angular_velocity(b, {});
  c.car->detach();
  c.far = true;
}

bool Game::traffic_make_near(TrafficCar& c, std::size_t i) {
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(c.actor);
  if (!b.valid() || !w.body_valid(b)) { c.far = false; return false; }
  const BodyState s = read_body(w, b);
  const Vec3 fwd = s.rotation.rotate({1, 0, 0});
  const f32 yaw = std::atan2(fwd.y, fwd.x);
  // only on ground that stands (its collision built, its streets' corridors laid) ...
  if (!city_.terrain().ground_ready(s.position.x, s.position.y)) return false;
  // ... and where no car with physics stands (vehicleFootprintsOverlap): else it drives on without
  // (it keeps its distance like every driver, so the place comes free) and is asked again
  const Vec3 h = c.car->spec().half;
  const spawn::Footprint me{{s.position.x, s.position.y}, yaw, {h.x, h.y}};
  for (std::size_t k = 0; k < cars_.size(); ++k) {
    const TrafficCar& o = cars_[k];
    if (k == i || !o.placed || o.far || !world_->actor_alive(o.actor)) continue;
    const BodyState os = state_of(o.actor);
    if (std::fabs(os.position.x - s.position.x) > 12 || std::fabs(os.position.y - s.position.y) > 12) continue;
    const Vec3 of = os.rotation.rotate({1, 0, 0});
    const Vec3 oh = o.car->spec().half;
    if (spawn::footprints_overlap(me, {{os.position.x, os.position.y}, std::atan2(of.y, of.x), {oh.x, oh.y}})) return false;
  }
  // (anything else in its place - a parked jet, a tank, a wreck's parts: a ray down through it)
  if (cast(w, s.position + Vec3{0, 0, 3}, {0, 0, -1}, 3.2f, b, ~static_cast<u64>(kCatWorld)).hit) return false;
  const kern::physics::Pose rest = car_rest_pose(c.car->spec(), s.position.x, s.position.y, yaw);
  w.set_pose(b, rest);
  w.set_body_type(b, kern::physics::BodyType::dynamic);
  const RigidSpec rs = Car::rigid_spec(c.car->spec());
  w.set_mass_data(b, rs.mass.mass, rs.mass.center, rs.mass.inertia);
  traffic_solid(c);
  c.car->reset();
  w.set_linear_velocity(b, rest.rotation.rotate({1, 0, 0}) * c.far_speed);
  w.set_angular_velocity(b, {});
  w.set_awake(b, true);
  return true;
}

void Game::traffic_far_switch() {
  // the hysteresis: physics on under 150 m, off beyond 180 m (driven cars, police, wrecks keep theirs)
  kern::physics::World& w = world_->physics();
  const Vec3 me = focus_position();
  const std::vector<Vec3> mates = net_.hosting() ? mate_foci() : std::vector<Vec3>{};
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    const kern::physics::Body b = body(c.actor);
    if (!c.placed || !b.valid() || !w.body_valid(b)) { c.far = false; c.solid_masks.clear(); continue; }  // (no body, or a new one: solid)
    if (puppet_world_ && i < car_net_.size() && car_net_[i] >= 0) { traffic_solid(c); continue; }  // the host's car here: on his poses (game_worldsync.cpp)
    const bool keep = c.police || c.wreck || c.burn_until >= 0 || static_cast<i32>(i) == car_index_ || (i < car_lent_.size() && car_lent_[i]);
    const Vec3 p = matrix_of(c.actor).translation();
    // near any player real physics (MP host: the ground and the buildings are loaded around the
    // mates too - CityStream::extra_foci); never where no ground collision stands yet
    f32 d = std::hypot(p.x - me.x, p.y - me.y);
    for (const Vec3& m : mates) d = std::fmin(d, std::hypot(p.x - m.x, p.y - m.y));
    if (d <= kPhysFar && !city_.terrain().has_collision(p.x, p.y)) d = kPhysFar + 1;
    if (!c.far && !keep && d > kPhysFar) {
      c.far_speed = c.driver ? std::fmax(0.0f, c.car->forward_speed()) : 0;
      traffic_make_far(c);
    } else if (c.far && (keep || d < kPhysNear)) {
      if (!traffic_make_near(c, i) && keep) {
        // (one that must have physics - a cruiser, a wreck, the player's: it has it where it is)
        w.set_body_type(b, kern::physics::BodyType::dynamic);
        const RigidSpec rs = Car::rigid_spec(c.car->spec());
        w.set_mass_data(b, rs.mass.mass, rs.mass.center, rs.mass.inertia);
        traffic_solid(c);
        c.car->reset();
        w.set_awake(b, true);
      }
    }
  }
}

void Game::traffic_far_step(TrafficCar& c, std::size_t i, f32 h) {
  // the route without physics: towards the lane's next point, turning onto it, with the same lane
  // changes and under the same rules as the AI driver (drive_traffic) - at 60 Hz like him
  // (staggered over the cars; the kinematic body moves on at its velocity in between)
  if ((steps_ + i) % 4 != 0) return;
  const f32 dt = h * 4;
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(c.actor);
  const BodyState s = read_body(w, b);
  const std::vector<Lane>& lanes = city_.lanes();
  if (!c.driver || c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive || lanes[c.lane].points.size() < 2) {
    c.far_speed = 0;
    c.want = 0;
    c.ahead = -1;
    w.set_target_pose(b, {s.position, s.rotation}, dt);
    return;
  }
  const Lane& L = lanes[c.lane];
  const i32 n = static_cast<i32>(L.points.size());
  c.index = std::clamp(c.index, 0, n - 1);
  const Vec3 fwd3 = s.rotation.rotate({1, 0, 0});
  const f32 fl = std::hypot(fwd3.x, fwd3.y);
  const Vec3 fwd = fl > 1e-4f ? Vec3{fwd3.x / fl, fwd3.y / fl, 0} : Vec3{1, 0, 0};
  const f32 off = std::fmin(L.width * 0.25f, 1.8f);
  // the same target as the driver with physics: a lookahead on along the lane, on its right
  const TrafficRules::Pursuit pu = TrafficRules::pursuit(L, c.index, c.dir, {s.position.x, s.position.y}, std::fmax(7.0f, c.target_speed * 1.3f), off);
  const Vec2 target = pu.target;
  Vec2 to{target.x - s.position.x, target.y - s.position.y};
  const f32 tl = std::hypot(to.x, to.y);
  if (c.index + c.dir < 0 || c.index + c.dir >= n ? pu.end < 2.5f : pu.passed) traffic_advance(c);
  // speed: the car's own, slower towards the street's end, what the lights, the people and the
  // vehicle ahead leave (as drive_traffic)
  f32 want = c.target_speed;
  if (pu.end < 18) want = std::fmin(want, 4.5f + pu.end * 0.3f);
  want = std::fmin(want, traffic_rules(static_cast<i32>(i), s.position, fwd, c.far_speed));
  {
    i32 limiter = -1;
    const f32 free_way = traffic_gap(static_cast<i32>(i), s.position, fwd, &limiter);
    c.ahead = free_way < 0.6f ? limiter : -1;
    want = std::fmin(want, free_way);
  }
  c.want = want;
  // (a driver's braking and pulling away: 4.5 and 3 m/s2)
  c.far_speed = std::fmax(0.0f, c.far_speed + clampf(want - c.far_speed, -4.5f * dt, 3 * dt));
  const Vec2 dir = tl > 1e-3f ? to * (1.0f / tl) : Vec2{fwd.x, fwd.y};
  // heading eases onto the direction of travel (about 1.5 rad/s, only while it rolls)
  const f32 yaw = std::atan2(fwd.y, fwd.x), want_yaw = std::atan2(dir.y, dir.x);
  f32 dy = want_yaw - yaw;
  while (dy > kPi) dy -= 2 * kPi;
  while (dy < -kPi) dy += 2 * kPi;
  const f32 turn = 1.5f * dt * std::fmin(1.0f, c.far_speed / 2);
  const f32 ny = yaw + clampf(dy, -turn, turn);
  const Vec2 np{s.position.x + dir.x * c.far_speed * dt, s.position.y + dir.y * c.far_speed * dt};
  w.set_target_pose(b, car_rest_pose(c.car->spec(), np.x, np.y, ny), dt);
}

}  // namespace sc
