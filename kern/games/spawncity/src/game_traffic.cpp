// SpawnCity on Kern, milestone 3: traffic in the world (see world_traffic.h for the rules):
//  * every AI driver asks TrafficRules for its speed limit (lights, right of way, people) and
//    honks when blocked (world_procedural_population.mjs honkCheck)
//  * carjacking: the driver gets out on the street side and runs off - or first stands and
//    stares for 2.6 s (world_procedural_population.mjs ejectDriver)
//  * the tow service (tow_service.mjs): traffic that is flipped (2.5 s), stuck (9 s, not waiting
//    at a junction) or off its street (9 m for 7 s), and burnt-out wrecks after 20 s, get a
//    yellow tow truck: it is sent from a depot 170..320 m away out of the player's reach (more
//    than 140 m), drives the street graph (A*, pure pursuit, backing out when wedged), hooks the
//    car (1.8 s), tows it 30 m on along its own lane and sets it down upright - a wreck it takes
//    away. Then back to the depot, gone when far. Two trucks at most, 110 HP (blasts wreck them).
#include <algorithm>
#include <cstdio>

#include "game.h"
#include "game_world.h"

namespace sc {

namespace {
// tow_service.mjs
constexpr u32 kMaxTrucks = 2;
constexpr f32 kFlipS = 2.5f, kStuckS = 9, kOffroadS = 7, kOffroadM = 9, kSpawnMin = 170, kSpawnMax = 320, kHookM = 7.5f, kHookS = 1.8f, kTowSpeed = 9,
              kDriveSpeed = 14, kGiveUpS = 70, kWreckWaitS = 20, kTruckHp = 110;
const Vec3 kTowYellow = kern::hex_color(0xf2b705);
constexpr f32 kCacheS = 0.12f;  // a driver looks again every 120 ms (yieldCheckAt)

f32 lane_distance(const Lane& l, Vec2 p) {
  f32 best = 1e9f;
  for (std::size_t i = 0; i + 1 < l.points.size(); ++i) {
    const Vec2 a = l.points[i], b = l.points[i + 1], d = b - a;
    const f32 l2 = d.x * d.x + d.y * d.y;
    const f32 t = l2 > 0 ? clampf(((p.x - a.x) * d.x + (p.y - a.y) * d.y) / l2, 0, 1) : 0;
    best = std::fmin(best, std::hypot(p.x - a.x - d.x * t, p.y - a.y - d.y * t));
  }
  return best;
}
}  // namespace

f32 Game::traffic_rules(i32 car, Vec3 pos, Vec3 fwd, f32 speed) {
  if (!life_ || car < 0) return 1e9f;
  WorldLife& L = *life_;
  if (static_cast<i32>(L.car_limit.size()) <= car) { L.car_limit.resize(car + 1, 1e9f); L.car_limit_at.resize(car + 1, -99); L.car_view.resize(car + 1); }
  const f32 now = world_->sim_time();
  L.rules.lights_time = static_cast<f32>(std::fmod(life_epoch(), 38.0 * 1000));
  if (now - L.car_limit_at[car] < kCacheS) return L.car_limit[car];
  L.car_limit_at[car] = now;
  if (L.rules.built_from != lane_signature(city_.lanes())) {
    L.rules.rebuild(city_.lanes());
    L.rules.built_from = lane_signature(city_.lanes());
  }
  std::vector<Vec3>& cars = L.car_positions;
  cars.assign(cars_.size(), Vec3{0, 0, -1e9f});
  for (std::size_t i = 0; i < cars_.size(); ++i)
    if (cars_[i].placed && world_->actor_alive(cars_[i].actor)) cars[i] = matrix_of(cars_[i].actor).translation();
  const f32 fl = std::hypot(fwd.x, fwd.y);
  const Vec2 f2 = fl > 1e-4f ? Vec2{fwd.x / fl, fwd.y / fl} : Vec2{1, 0};
  const Vec3 me = matrix_of(soldier_).translation();
  const bool bus = cars_[car].car->spec().radius > 0.4f;
  TrafficRules::View& v = L.car_view[car];
  const f32 lim = L.rules.limit(car, {pos.x, pos.y}, f2, speed, bus, now, cars, L.walkers, {me.x, me.y}, mode_ == Mode::foot && !player_dead_, v);
  if (v.honk) L.honks.push_back(pos + Vec3{0, 0, 0.4f});
  L.car_limit[car] = lim;
  return lim;
}

f32 Game::traffic_lane_distance(const TrafficCar& c) const {
  const std::vector<Lane>& lanes = city_.lanes();
  if (c.lane < 0 || c.lane >= static_cast<i32>(lanes.size())) return -1;
  const Vec3 p = matrix_of(c.actor).translation();
  return lane_distance(lanes[static_cast<std::size_t>(c.lane)], {p.x, p.y});
}

f32 Game::traffic_gap(i32 car, Vec3 pos, Vec3 fwd, i32* limiter) {
  if (limiter) *limiter = -1;
  if (!life_ || car < 0) return 1e9f;
  WorldLife& L = *life_;
  // every vehicle on the road, once a step: the cars (by index; with or without physics), then
  // the tow trucks
  if (L.vehicles_step != steps_) {
    L.vehicles_step = steps_;
    L.vehicles.assign(cars_.size() + L.trucks.size(), TrafficRules::Vehicle{});
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      const TrafficCar& c = cars_[i];
      if (!c.placed || !world_->actor_alive(c.actor)) continue;
      const Vec3 p = matrix_of(c.actor).translation();
      L.vehicles[i] = {{p.x, p.y}, c.car->spec().radius > 0.4f, true};
    }
    for (std::size_t k = 0; k < L.trucks.size(); ++k) {
      const WorldLife::TowTruck& t = L.trucks[k];
      if (!t.body.valid() || !world_->physics().body_valid(t.body)) continue;
      const Vec3 p = world_->physics().pose(t.body).position;
      L.vehicles[cars_.size() + k] = {{p.x, p.y}, false, true};
    }
  }
  const f32 fl = std::hypot(fwd.x, fwd.y);
  const Vec2 f2 = fl > 1e-4f ? Vec2{fwd.x / fl, fwd.y / fl} : Vec2{1, 0};
  i32 who = -1;
  const f32 v = TrafficRules::gap_speed(car, {pos.x, pos.y}, f2, cars_[static_cast<std::size_t>(car)].car->spec().radius > 0.4f, L.vehicles, &who);
  if (limiter) *limiter = who >= 0 && who < static_cast<i32>(cars_.size()) ? who : -1;
  return v;
}

void Game::eject_driver(i32 ci) {
  if (!life_ || ci < 0 || ci >= static_cast<i32>(cars_.size())) return;
  const BodyState s = state_of(cars_[ci].actor);
  const f32 yaw = heading_of(s.rotation.rotate({1, 0, 0}));
  // ejectDriver: 1.8 m to the driver's side (left), facing out; one of the city's people
  const Vec2 side{-std::sin(yaw), std::cos(yaw)};
  Vec3 at{s.position.x + side.x * 1.8f, s.position.y + side.y * 1.8f, 0};
  if (cast(world_->physics(), s.position, {side.x, side.y, 0}, 2.3f, body(cars_[ci].actor)).hit) at = {s.position.x - side.x * 1.8f, s.position.y - side.y * 1.8f, 0};
  at.z = city_.ground_height(at.x, at.y);
  const i32 p = people_.add_citizen(at, yaw + kPi / 2, rng_.next() | 1);
  People::Person& who = people_.at(p);
  // his pavement: the car's street
  const TrafficCar& c = cars_[ci];
  if (c.lane >= 0 && c.lane < static_cast<i32>(city_.lanes().size()) && city_.lanes()[c.lane].alive) {
    const Lane& l = city_.lanes()[c.lane];
    who.lane = c.lane;
    who.seg = std::clamp(c.index - (c.dir > 0 ? 1 : 0), 0, static_cast<i32>(l.points.size()) - 2);
    who.dir = rng_.unit() < 0.5f ? 1 : -1;
    who.side = 1;
    who.off = l.width * 0.5f + 1.7f;
    who.along = std::hypot(at.x - l.points[who.seg].x, at.y - l.points[who.seg].y);
  }
  const f32 now = world_->sim_time();
  if (rng_.unit() < 0.5f) people_.frighten(s.position, 4, now, 1.2f);  // a runner: off at once
  else {
    // stands, stares at the thief for 2.6 s, then runs (ejectDriver "wait")
    who.state = People::State::wait;
    who.wait_until = now + 2.6f;
    who.yaw = std::atan2(s.position.y - at.y, s.position.x - at.x);
    life_->delayed_fright.push_back({p, now + 2.6f, s.position});
  }
  ++life_->ejected;
}

void Game::update_traffic_world(f32 dt) {
  (void)dt;
  WorldLife& L = *life_;
  const f32 now = world_->sim_time();
  // The people drivers look out for (standing / walking near the focus).
  L.walkers.clear();
  const Vec3 focus = focus_position();
  for (const People::Person& p : people_.all())
    if (p.state != People::State::down && p.state != People::State::gone && std::hypot(p.pos.x - focus.x, p.pos.y - focus.y) < 320) L.walkers.push_back({p.pos.x, p.pos.y});
  for (auto it = L.delayed_fright.begin(); it != L.delayed_fright.end();) {
    if (now < it->at) { ++it; continue; }
    if (it->person >= 0 && it->person < static_cast<i32>(people_.all().size())) {
      People::Person& p = people_.at(it->person);
      if (p.state == People::State::wait) p.state = People::State::walk;
      people_.frighten(p.pos, 1.5f, now, 0.6f);
    }
    it = L.delayed_fright.erase(it);
  }
  for (const Vec3& h : L.honks)
    if (L.sounds.ready && std::hypot(h.x - focus.x, h.y - focus.y) < 120) {
      kern::audio::PlayParams pp;
      pp.spatial = true;
      pp.position = h;
      pp.max_distance = 120;
      pp.min_distance = 3;
      pp.volume = 0.7f;
      mixer_.play(L.sounds.horn, pp);
      ++L.honk_count;
    }
  L.honks.clear();

  // A tow truck shot to pieces burns out like the traffic (tow_service.mjs damageTruck).
  for (WorldLife::TowTruck& t : L.trucks)
    if (t.hp <= 0 && !t.exploded && t.body.valid() && world_->physics().body_valid(t.body)) {
      t.exploded = true;
      t.towing = false;
      explosion(world_->physics().pose(t.body).position + Vec3{0, 0, 0.3f}, 7, 110, false);
    }
  // ---- tow service: who needs help (every 0.5 s) ----
  if (puppet_world_) return;  // MP client in the host's town: his cars, his tow trucks
  if (now - L.tow_scan_at < 0.5f) return;
  L.tow_scan_at = now;
  if (L.watch.size() < cars_.size()) L.watch.resize(cars_.size());
  const Vec3 me = player_position();
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    WorldLife::Watch& w = L.watch[i];
    const bool busy = std::any_of(L.trucks.begin(), L.trucks.end(), [&](const WorldLife::TowTruck& t) { return t.job == static_cast<i32>(i); });
    // (a car without physics far away is nobody's job: tow_service.mjs watches the physical ones)
    if (!c.placed || c.far || c.police || static_cast<i32>(i) == car_index_ || busy || now < w.cooldown) { w = {w.cooldown}; continue; }
    const BodyState s = state_of(c.actor);
    const f32 up = s.rotation.rotate({0, 0, 1}).z, speed = std::hypot(s.velocity.x, s.velocity.y);
    // waiting: at a red light, for the junction or for somebody in the way - or in the queue
    // behind one who does (the car ahead that takes its way, and the one ahead of that ...). A
    // queue nobody at its head is waiting in - two cars nose to nose, a car behind a wreck -
    // stands for good: that is stuck.
    bool waiting = false;
    for (i32 k = static_cast<i32>(i), hop = 0; k >= 0 && k < static_cast<i32>(cars_.size()) && hop < 16; ++hop) {
      if (k < static_cast<i32>(L.car_view.size()) && (L.car_view[static_cast<std::size_t>(k)].wait || L.car_view[static_cast<std::size_t>(k)].yielding)) { waiting = true; break; }
      k = cars_[static_cast<std::size_t>(k)].ahead;
      if (k == static_cast<i32>(i)) break;
    }
    const bool on_lane = c.lane >= 0 && c.lane < static_cast<i32>(city_.lanes().size()) && city_.lanes()[c.lane].alive;
    const char* reason = nullptr;
    if (c.wreck) { if (now - c.wreck_at > kWreckWaitS) reason = "wreck"; }
    else if (c.driver) {
      w.flip_since = up < 0.5f ? (w.flip_since < 0 ? now : w.flip_since) : -1;
      w.slow_since = speed < 0.6f && on_lane && !waiting ? (w.slow_since < 0 ? now : w.slow_since) : -1;
      w.off_since = on_lane && lane_distance(city_.lanes()[c.lane], {s.position.x, s.position.y}) > kOffroadM ? (w.off_since < 0 ? now : w.off_since) : -1;
      if (w.flip_since >= 0 && now - w.flip_since > kFlipS) reason = "flipped";
      else if (w.off_since >= 0 && now - w.off_since > kOffroadS) reason = "off-road";
      else if (w.slow_since >= 0 && now - w.slow_since > kStuckS) reason = "stuck";
    }
    if (!reason || L.trucks.size() >= kMaxTrucks) continue;
    // The depot: a street point far from the job and out of the player's reach.
    Vec3 spot;
    f32 heading = 0;
    bool found = false;
    for (int k = 0; k < 40 && !found; ++k) {
      if (!city_.street_point(rng_, s.position, kSpawnMax, spot, heading)) break;
      const f32 dj = std::hypot(spot.x - s.position.x, spot.y - s.position.y), dp = std::hypot(spot.x - me.x, spot.y - me.y);
      // (on ground that stands - the truck is a physical car from its first step - and free)
      found = dj > kSpawnMin && dp > 140 && city_.terrain().ground_ready(spot.x, spot.y) && !cast(world_->physics(), spot + Vec3{0, 0, 3}, {0, 0, -1}, 2.6f, {}).hit;
    }
    if (!found) continue;
    WorldLife::TowTruck t;
    t.car = std::make_unique<Car>(CarSpec::car());
    const kern::physics::Pose rest = car_rest_pose(t.car->spec(), spot.x, spot.y, heading);
    t.body = create_body(world_->physics(), Car::rigid_spec(t.car->spec()), rest.position, rest.rotation, kCatVehicle);
    t.job = static_cast<i32>(i);
    t.depot = {spot.x, spot.y};
    t.born = now;
    t.reason = reason;
    t.wreck_job = c.wreck;
    L.trucks.push_back(std::move(t));
    ++L.dispatches;
    L.last_reason = reason;
  }
}

void Game::step_tow(f32 h) {
  if (!life_) return;
  WorldLife& L = *life_;
  kern::physics::World& w = world_->physics();
  const f32 now = world_->sim_time();
  const Vec3 me = player_position();
  if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
  for (std::size_t ti = 0; ti < L.trucks.size();) {
    WorldLife::TowTruck& t = L.trucks[ti];
    bool remove = !t.body.valid() || !w.body_valid(t.body);
    if (!remove) {
      const BodyState s = read_body(w, t.body);
      const Vec3 fwd = s.rotation.rotate({1, 0, 0});
      const f32 yaw = std::atan2(fwd.y, fwd.x), sp = std::hypot(s.velocity.x, s.velocity.y);
      TrafficCar* job = t.job >= 0 && t.job < static_cast<i32>(cars_.size()) ? &cars_[t.job] : nullptr;
      CarInput in;
      in.handbrake = true;
      auto follow = [&](Vec2 goal, f32 speed) -> f32 {
        const Vec2 p{s.position.x, s.position.y};
        const f32 d = std::hypot(goal.x - p.x, goal.y - p.y);
        if (t.path.empty() || now - t.path_at > 2.5f) {
          if (!roads_.path(p, goal, t.path)) t.path = {p, goal};
          t.path_at = now;
        }
        const Vec2 tp = RoadGraph::pursuit_point(t.path, p, std::fmax(7.0f, sp * 1.1f));
        const f32 herr = wrap_pi(std::atan2(tp.y - p.y, tp.x - p.x) - yaw);
        if (now < t.recover_until) {
          // backing out with the wheel turned (follow: recoverUntil)
          if (std::fabs(herr) < 0.45f && now - t.recover_start > 0.5f) t.recover_until = -1;
          else { in = CarInput{}; in.pedal = -0.75f; in.steer = -sign0(herr != 0 ? herr : 1); in.max_reverse = 4; return d; }
        }
        if (sp < 0.7f && d > 6) {
          if (t.stuck_since < 0) t.stuck_since = now;
          if (now - t.stuck_since > 2.2f) { t.recover_start = now; t.recover_until = now + 1.7f; t.stuck_since = -1; t.path.clear(); return d; }
        } else t.stuck_since = -1;
        const f32 want = d < 20 ? std::fmax(3.0f, d * 0.5f) : speed;
        in = ai_drive(s.position, fwd, t.car->forward_speed(), {tp.x, tp.y, s.position.z}, std::fmin(speed, want));
        return d;
      };
      // The player took the car the truck came for: the job is off.
      if (job && t.state != WorldLife::TowTruck::State::depot && mode_ == Mode::car && car_index_ == t.job) { t.state = WorldLife::TowTruck::State::depot; t.towing = false; t.path.clear(); }
      if (t.state != WorldLife::TowTruck::State::depot && now - t.born > kGiveUpS) { t.state = WorldLife::TowTruck::State::depot; t.towing = false; t.path.clear(); }
      switch (t.state) {
        case WorldLife::TowTruck::State::to_job: {
          if (!job || !job->placed) { t.state = WorldLife::TowTruck::State::depot; break; }
          const Vec3 cp = matrix_of(job->actor).translation();
          if (follow({cp.x, cp.y}, kDriveSpeed) < kHookM) { in = CarInput{}; in.handbrake = true; in.pedal = -0.3f * sign0(t.car->forward_speed()); t.state = WorldLife::TowTruck::State::hook; t.state_at = now; }
          break;
        }
        case WorldLife::TowTruck::State::hook:
          in = CarInput{};
          in.handbrake = true;
          if (now - t.state_at > kHookS && job) {
            t.towing = true;
            job->driver = !t.wreck_job;
            // the drop: 30 m on along its own lane (lanePose)
            t.drop = {s.position.x, s.position.y};
            t.drop_yaw = yaw;
            if (!t.wreck_job && job->lane >= 0 && job->lane < static_cast<i32>(city_.lanes().size()) && city_.lanes()[job->lane].alive) {
              const Lane& l = city_.lanes()[job->lane];
              const i32 n = static_cast<i32>(l.points.size());
              i32 k = std::clamp(job->index, 0, n - 1);
              f32 left = 30;
              while (left > 0 && k + job->dir >= 0 && k + job->dir < n) {
                left -= std::hypot(l.points[k + job->dir].x - l.points[k].x, l.points[k + job->dir].y - l.points[k].y);
                k += job->dir;
              }
              const i32 prev = std::clamp(k - job->dir, 0, n - 1);
              const Vec2 d = l.points[k] - l.points[prev];
              const f32 dl = std::hypot(d.x, d.y);
              t.drop_yaw = dl > 1e-3f ? std::atan2(d.y, d.x) : yaw;
              const f32 off = std::fmin(l.width * 0.25f, 1.8f);
              t.drop = {l.points[k].x + std::sin(t.drop_yaw) * off, l.points[k].y - std::cos(t.drop_yaw) * off};
              job->index = std::clamp(k + job->dir, 0, n - 1);
            }
            t.state = WorldLife::TowTruck::State::tow;
            t.path.clear();
          }
          break;
        case WorldLife::TowTruck::State::tow: {
          if (!job || !job->placed) { t.state = WorldLife::TowTruck::State::depot; t.towing = false; break; }
          const Vec2 goal = t.wreck_job ? t.depot : Vec2{t.drop.x + std::cos(t.drop_yaw) * 5.5f, t.drop.y + std::sin(t.drop_yaw) * 5.5f};
          const f32 d = follow(goal, kTowSpeed);
          if (!t.wreck_job && d < 4.5f && sp < 2.5f) {
            // set it down upright on its lane; the driver carries on
            t.towing = false;
            const Vec3 back{s.position.x - std::cos(yaw) * 5.1f, s.position.y - std::sin(yaw) * 5.1f, 0};
            const Vec3 rot{0, 0, yaw / kDeg};
            const kern::physics::Pose rest = car_rest_pose(job->car->spec(), back.x, back.y, yaw);
            world_->teleport(job->actor, rest.position, &rot);
            if (const kern::physics::Body jb = body(job->actor); jb.valid()) w.set_pose(jb, rest);
            world_->set_actor_velocity(job->actor, {});
            job->car->reset();
            t.state = WorldLife::TowTruck::State::depot;
            t.state_at = now;
            t.path.clear();
            ++L.tow_done;
            if (t.job < static_cast<i32>(L.watch.size())) L.watch[t.job] = {now + 60};
          } else if (t.wreck_job && (d < 12 || std::hypot(s.position.x - me.x, s.position.y - me.y) > 220)) remove = true;
          break;
        }
        case WorldLife::TowTruck::State::depot: {
          if (now - t.state_at < 1.2f) { in = CarInput{}; in.handbrake = true; break; }
          const f32 d = follow(t.depot, kDriveSpeed);
          if (d < 12 || std::hypot(s.position.x - me.x, s.position.y - me.y) > 200) remove = true;
          break;
        }
      }
      if (t.towing && job) {
        // the car hangs behind on the boom (front lifted a little)
        const Vec3 back{s.position.x - std::cos(yaw) * 5.1f, s.position.y - std::sin(yaw) * 5.1f, 0};
        const kern::physics::Body jb = body(job->actor);
        if (jb.valid()) {
          const Quat q = yaw_quat(yaw) * Quat::axis_angle({0, 1, 0}, -0.08f);
          w.set_pose(jb, {{back.x, back.y, city_.ground_height(back.x, back.y) + Car::ground_offset(job->car->spec()) + 0.35f}, q});
          w.set_linear_velocity(jb, {s.velocity.x, s.velocity.y, 0});
          w.set_angular_velocity(jb, {});
        }
      }
      t.input = in;
      t.car->step(w, t.body, h, in);
      if (t.hp <= 0 && t.exploded) remove = true;
    }
    if (remove) {
      if (t.towing && t.wreck_job && t.job >= 0 && t.job < static_cast<i32>(cars_.size())) {
        // the wreck is gone with the truck: back into the pool
        TrafficCar& c = cars_[t.job];
        world_->set_alive(c.actor, false);
        c.placed = false;
        c.wreck = false;
        c.hp = car_max_hp(c);
        ++L.tow_done;
      }
      if (t.body.valid() && w.body_valid(t.body)) w.destroy_body(t.body);
      L.trucks.erase(L.trucks.begin() + static_cast<std::ptrdiff_t>(ti));
      continue;
    }
    ++ti;
  }
}

void Game::draw_world_vehicles() {
  if (!life_) return;
  WorldLife& L = *life_;
  const kern::physics::World& w = world_->physics();
  for (const WorldLife::TowTruck& t : L.trucks) {
    if (!t.body.valid() || !w.body_valid(t.body)) continue;
    const kern::physics::Pose p = w.pose(t.body);
    const kern::Mat4 m = kern::Mat4::trs(p.position, p.rotation, {1, 1, 1});
    art_.car(m, *t.car, kTowYellow, false, t.input.pedal < -0.05f);
  }
}

void Game::draw_traffic_lights() {
  if (!life_ || !life_->art_ready) return;
  WorldLife& L = *life_;
  const f32 now = world_->sim_time();
  L.rules.lights_time = static_cast<f32>(std::fmod(life_epoch(), 38.0 * 1000));  // (the host's wall clock in a session)
  // the tow trucks' bed, boom, hook and amber bar (tow_service.mjs buildTruck)
  for (const WorldLife::TowTruck& t : L.trucks) {
    if (!t.body.valid() || !world_->physics().body_valid(t.body)) continue;
    const kern::physics::Pose p = world_->physics().pose(t.body);
    const kern::Mat4 m = kern::Mat4::trs(p.position, p.rotation, {1, 1, 1});
    {
      // bed, boom and hook (tow_service.mjs buildTruck), the amber bar flashing
      L.art.put(L.art.cube, m * kern::Mat4::translate({-0.95f, 0, 0.75f}) * kern::Mat4::scale({1.5f, 1.5f, 0.12f}), {0.14f, 0.15f, 0.16f});
      L.art.put(L.art.cube, m * kern::Mat4::translate({-1.35f, 0, 1.15f}) * kern::Mat4::trs({}, Quat::axis_angle({0, 1, 0}, -0.38f), {1.9f, 0.16f, 0.16f}), {0.54f, 0.56f, 0.59f});
      L.art.put(L.art.cube, m * kern::Mat4::translate({-2.2f, 0, 0.75f}) * kern::Mat4::scale({0.12f, 1.3f, 0.1f}), {0.54f, 0.56f, 0.59f});
      const bool on = static_cast<i32>(now / 0.26f) % 2 == 0;
      L.art.put(L.art.glow, m * kern::Mat4::translate({0.2f, 0.24f, 1.25f}) * kern::Mat4::scale({0.12f, 0.12f, 0.08f}), on ? Vec3{1, 0.64f, 0.1f} : Vec3{0.23f, 0.15f, 0.03f});
      L.art.put(L.art.glow, m * kern::Mat4::translate({0.2f, -0.24f, 1.25f}) * kern::Mat4::scale({0.12f, 0.12f, 0.08f}), on ? Vec3{0.23f, 0.15f, 0.03f} : Vec3{1, 0.64f, 0.1f});
    }
  }
  std::vector<i32> near;
  L.rules.near({cam_.eye.x, cam_.eye.y}, 160, near);
  for (i32 ji : near) {
    const TrafficRules::Junction& j = L.rules.junctions()[ji];
    if (!j.lights) continue;
    for (const TrafficRules::Arm& a : j.arms) {
      // a post on the kerb corner right of the incoming traffic (world_traffic.cpp)
      if (!a.has_pole) continue;
      const Vec2 p = j.p + a.pole;
      if (city_.terrain().water_at(p.x, p.y)) continue;
      const f32 g = city_.ground_height(p.x, p.y);
      L.art.put(L.art.cylinder, kern::Mat4::translate({p.x, p.y, g}) * kern::Mat4::scale({0.07f, 0.07f, 3.1f}), {0.22f, 0.23f, 0.24f});
      const f32 face = std::atan2(a.dir.y, a.dir.x);
      const kern::Mat4 head = kern::Mat4::translate({p.x, p.y, g + 2.6f}) * kern::Mat4::trs({}, yaw_quat(face), {1, 1, 1});
      L.art.put(L.art.cube, head * kern::Mat4::scale({0.22f, 0.34f, 0.9f}), {0.08f, 0.08f, 0.09f});
      const i32 l = L.rules.light(j, {-a.dir.x, -a.dir.y}, now);
      const Vec3 off{0.06f, 0.06f, 0.06f};
      const Vec3 cols[3] = {l == 2 ? Vec3{1.4f, 0.08f, 0.05f} : off, l == 1 ? Vec3{1.4f, 0.75f, 0.05f} : off, l == 0 ? Vec3{0.1f, 1.3f, 0.35f} : off};
      for (int k = 0; k < 3; ++k) L.art.put(L.art.glow, head * kern::Mat4::translate({0.12f, 0, 0.28f - 0.28f * k}) * kern::Mat4::scale({0.03f, 0.1f, 0.1f}), cols[k]);
    }
  }
}

std::string Game::traffic_json() {
  if (!life_) return "{}";
  WorldLife& L = *life_;
  if (L.rules.built_from != lane_signature(city_.lanes())) {  // the junctions of the streets as they are now
    L.rules.rebuild(city_.lanes());
    L.rules.built_from = lane_signature(city_.lanes());
  }
  u32 lights = 0, waiting = 0;
  for (const TrafficRules::Junction& j : L.rules.junctions()) lights += j.lights ? 1 : 0;
  for (const TrafficRules::View& v : L.car_view) waiting += v.wait ? 1 : 0;
  std::string trucks = "[";
  for (const WorldLife::TowTruck& t : L.trucks) {
    char b[96];
    static const char* states[] = {"to-job", "hook", "tow", "depot"};
    std::snprintf(b, sizeof b, "%s{\"job\":%d,\"state\":\"%s\",\"reason\":\"%s\"}", trucks.size() > 1 ? "," : "", t.job, states[static_cast<int>(t.state)], t.reason);
    trucks += b;
  }
  char b[320];
  std::snprintf(b, sizeof b, "{\"junctions\":%zu,\"lights\":%u,\"waiting\":%u,\"honks\":%u,\"ejected\":%u,\"dispatches\":%u,\"towed\":%u,\"trucks\":", L.rules.junctions().size(), lights,
                waiting, L.honk_count, L.ejected, L.dispatches, L.tow_done);
  return b + trucks + "]}";
}

bool Game::car_towed(i32 car) const {
  if (!life_) return false;
  for (const WorldLife::TowTruck& t : life_->trucks)
    if (t.job == car) return true;
  return false;
}

}  // namespace sc
