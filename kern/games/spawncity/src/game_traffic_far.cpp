// SpawnCity on Kern: cars far from the player as lightweight lane followers, as the original does
// (world_procedural_population.mjs PHYS_NEAR_M 150 / PHYS_FAR_M 180: a car only has a physics body
// near the focus; further away it follows its route analytically). Here its Box3D body turns
// kinematic and is moved along the lane (no wheel rays, no contacts with the static world); within
// 150 m it is dynamic again with the speed it had. Hooks: drive_traffic, manage_traffic.
#include <algorithm>
#include <cmath>

#include "game.h"

namespace sc {

namespace {
constexpr f32 kPhysNear = 150, kPhysFar = 180;  // PHYS_NEAR_M, PHYS_FAR_M
}

void Game::traffic_far_switch() {
  // the hysteresis: physics on under 150 m, off beyond 180 m (driven cars, police, wrecks keep theirs)
  kern::physics::World& w = world_->physics();
  const Vec3 me = focus_position();
  const std::vector<Vec3> mates = net_.hosting() ? mate_foci() : std::vector<Vec3>{};
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    const kern::physics::Body b = body(c.actor);
    if (!c.placed || !b.valid() || !w.body_valid(b)) { c.far = false; continue; }
    const bool keep = c.police || c.wreck || c.burn_until >= 0 || static_cast<i32>(i) == car_index_;
    const Vec3 p = matrix_of(c.actor).translation();
    // near any player real physics (MP host: the ground and the buildings are loaded around the
    // mates too - CityStream::extra_foci); never where no ground collision stands yet
    f32 d = std::hypot(p.x - me.x, p.y - me.y);
    for (const Vec3& m : mates) d = std::fmin(d, std::hypot(p.x - m.x, p.y - m.y));
    if (d <= kPhysFar && !city_.terrain().has_collision(p.x, p.y)) d = kPhysFar + 1;
    if (!c.far && !keep && d > kPhysFar) {
      c.far_speed = c.driver ? std::fmax(0.0f, c.car->forward_speed()) : 0;
      w.set_body_type(b, kern::physics::BodyType::kinematic);
      w.set_linear_velocity(b, {});
      w.set_angular_velocity(b, {});
      c.far = true;
    } else if (c.far && (keep || d < kPhysNear)) {
      w.set_body_type(b, kern::physics::BodyType::dynamic);
      const RigidSpec rs = Car::rigid_spec(c.car->spec());
      w.set_mass_data(b, rs.mass.mass, rs.mass.center, rs.mass.inertia);
      const Vec3 fwd = state_of(c.actor).rotation.rotate({1, 0, 0});
      c.car->reset();
      w.set_linear_velocity(b, fwd * c.far_speed);
      w.set_awake(b, true);
      c.far = false;
    }
  }
}

void Game::traffic_far_step(TrafficCar& c, f32 h) {
  // the analytic route: towards the lane's next point at the car's speed, turning onto it; the same
  // lane changes as the AI driver (drive_traffic)
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(c.actor);
  const BodyState s = read_body(w, b);
  const std::vector<Lane>& lanes = city_.lanes();
  if (!c.driver || c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive || lanes[c.lane].points.size() < 2) {
    w.set_target_pose(b, {s.position, s.rotation}, h);
    return;
  }
  const Lane& L = lanes[c.lane];
  const i32 n = static_cast<i32>(L.points.size());
  c.index = std::clamp(c.index, 0, n - 1);
  const Vec2 p = L.points[c.index];
  const i32 prev = std::clamp(c.index - c.dir, 0, n - 1);
  Vec2 d = p - L.points[prev];
  const f32 dl = std::hypot(d.x, d.y);
  const Vec3 fwd = s.rotation.rotate({1, 0, 0});
  d = dl > 1e-3f ? d * (1.0f / dl) : Vec2{fwd.x, fwd.y};
  const f32 off = std::fmin(L.width * 0.25f, 1.8f);
  const Vec2 target{p.x + d.y * off, p.y - d.x * off};
  Vec2 to{target.x - s.position.x, target.y - s.position.y};
  const f32 tl = std::hypot(to.x, to.y);
  if (tl < 7) traffic_advance(c);
  // speed: the car's own, slower towards the street's end (as drive_traffic)
  const Vec2 end = c.dir > 0 ? L.points.back() : L.points.front();
  const f32 to_end = std::hypot(end.x - s.position.x, end.y - s.position.y);
  f32 want = c.target_speed;
  if (to_end < 18) want = std::fmin(want, 4.5f + to_end * 0.3f);
  c.far_speed += clampf(want - c.far_speed, -4 * h, 3 * h);
  const Vec2 dir = tl > 1e-3f ? to * (1.0f / tl) : Vec2{fwd.x, fwd.y};
  Vec3 np = s.position + Vec3{dir.x, dir.y, 0} * (c.far_speed * h);
  np.z = city_.ground_height(np.x, np.y) + Car::ground_offset(c.car->spec());
  // heading eases onto the direction of travel (about 1.5 rad/s)
  const f32 yaw = heading_of(fwd), want_yaw = std::atan2(dir.y, dir.x);
  f32 dy = want_yaw - yaw;
  while (dy > kPi) dy -= 2 * kPi;
  while (dy < -kPi) dy += 2 * kPi;
  const f32 ny = yaw + clampf(dy, -1.5f * h, 1.5f * h);
  w.set_target_pose(b, {np, Quat::axis_angle({0, 0, 1}, ny)}, h);
}

}  // namespace sc
