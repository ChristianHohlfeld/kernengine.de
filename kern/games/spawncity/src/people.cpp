// The town's people (see people.h).
#include "people.h"

#include <algorithm>
#include <cmath>

namespace sc {

namespace {

// pedestrian_agents.mjs / world_procedural_population.mjs
constexpr f32 kShow = 170, kHide = 190;       // people appear within 170 m and go beyond 190 m
constexpr f32 kPavement = 1.6f;               // pavement centre beyond the carriageway edge
constexpr f32 kWitness = 45;                  // wanted_system_logic: a witness within sight
constexpr f32 kBodySeconds = 25;              // a body stays this long
constexpr f32 kCapsuleR = 0.32f, kHeadZ = 1.5f, kTopZ = 1.8f;

const Vec3 kShirts[] = {{0.62f, 0.12f, 0.1f}, {0.16f, 0.28f, 0.55f}, {0.85f, 0.82f, 0.74f}, {0.2f, 0.42f, 0.25f}, {0.9f, 0.62f, 0.15f},
                        {0.35f, 0.35f, 0.38f}, {0.55f, 0.3f, 0.55f}, {0.1f, 0.1f, 0.12f}, {0.75f, 0.45f, 0.4f}, {0.3f, 0.6f, 0.7f}};
const Vec3 kPants[] = {{0.12f, 0.14f, 0.22f}, {0.2f, 0.2f, 0.22f}, {0.38f, 0.3f, 0.22f}, {0.08f, 0.08f, 0.09f}, {0.45f, 0.45f, 0.48f}, {0.25f, 0.3f, 0.45f}};
const Vec3 kSkins[] = {{0.94f, 0.76f, 0.62f}, {0.78f, 0.58f, 0.43f}, {0.66f, 0.44f, 0.32f}, {0.48f, 0.31f, 0.22f}, {0.86f, 0.66f, 0.5f}};

f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 approach_angle(f32 cur, f32 target, f32 max_step) { return cur + clampf(wrap_pi(target - cur), -max_step, max_step); }

/// Closest approach of segment p0-p1 to segment q0-q1: returns the squared distance, `s` along p.
f32 segment_distance2(Vec3 p0, Vec3 p1, Vec3 q0, Vec3 q1, f32& s, Vec3& on_q) {
  const Vec3 d1 = p1 - p0, d2 = q1 - q0, r = p0 - q0;
  const f32 a = kern::dot(d1, d1), e = kern::dot(d2, d2), f = kern::dot(d2, r);
  f32 t;
  if (a <= 1e-9f) { s = 0; t = clampf(f / e, 0, 1); }
  else {
    const f32 c = kern::dot(d1, r);
    const f32 b = kern::dot(d1, d2), den = a * e - b * b;
    s = den > 1e-9f ? clampf((b * f - c * e) / den, 0, 1) : 0;
    t = (b * s + f) / e;
    if (t < 0) { t = 0; s = clampf(-c / a, 0, 1); }
    else if (t > 1) { t = 1; s = clampf((b - c) / a, 0, 1); }
  }
  const Vec3 cp = p0 + d1 * s;
  on_q = q0 + d2 * t;
  const Vec3 dd = cp - on_q;
  return kern::dot(dd, dd);
}

bool inside_building(const kern::physics::World& w, Vec3 p, f32 ground) {
  // A roof above the ground here (the terrain itself answers at ground height).
  const RayResult h = cast(w, {p.x, p.y, ground + 60}, {0, 0, -1}, 70, {}, kCatWorld);
  return h.hit && h.point.z > ground + 0.35f;
}

}  // namespace

bool People::route_point(const Person& p, const std::vector<Lane>& lanes, Vec3& out, f32& heading) const {
  if (p.lane < 0 || p.lane >= static_cast<i32>(lanes.size())) return false;
  const Lane& L = lanes[p.lane];
  if (!L.alive || p.seg < 0 || p.seg + 1 >= static_cast<i32>(L.points.size())) return false;
  const Vec2 a = L.points[p.seg], b = L.points[p.seg + 1];
  const Vec2 d = b - a;
  const f32 len = std::fmax(1e-3f, std::hypot(d.x, d.y));
  const Vec2 u = d * (1.0f / len), walk = u * static_cast<f32>(p.dir), right{walk.y, -walk.x};
  const Vec2 c = a + u * clampf(p.along, 0, len) + right * (p.side * p.off);
  out = {c.x, c.y, 0};
  heading = std::atan2(walk.y, walk.x);
  return true;
}

bool People::free_move(const kern::physics::World& w, Person& p, Vec2 dir, f32 step, bool keep) {
  const f32 l = std::hypot(dir.x, dir.y);
  if (l < 1e-5f || step <= 0) return true;
  dir = dir * (1.0f / l);
  for (f32 ang : {0.0f, 0.5f, -0.5f, 1.0f, -1.0f, 1.6f, -1.6f}) {
    const f32 c = std::cos(ang), s = std::sin(ang);
    const Vec2 d{dir.x * c - dir.y * s, dir.x * s + dir.y * c};
    if (cast(w, p.pos + Vec3{0, 0, 1.0f}, {d.x, d.y, 0}, step + kCapsuleR, {}, kCatWorld).hit) continue;
    p.pos.x += d.x * step;
    p.pos.y += d.y * step;
    if (keep && ang != 0) p.flee_dir = d;
    return true;
  }
  return false;
}

i32 People::add_officer(Vec3 at, f32 yaw, i32 unit, u32 seed) {
  Person p;
  p.kind = Kind::officer;
  p.state = State::officer;
  p.pos = at;
  p.yaw = yaw;
  p.hp = 100;  // police_ground_units.mjs COP_HP
  p.unit = unit;
  p.seed = seed;
  p.shirt = {0.14f, 0.19f, 0.31f};
  p.pants = {0.1f, 0.13f, 0.19f};
  p.skin = kSkins[seed % 5];
  for (std::size_t i = 0; i < people_.size(); ++i)
    if (people_[i].state == State::gone) { people_[i] = p; return static_cast<i32>(i); }
  people_.push_back(p);
  return static_cast<i32>(people_.size() - 1);
}

i32 People::add_citizen(Vec3 at, f32 yaw, u32 seed) {
  Person p;
  p.kind = Kind::citizen;
  p.state = State::walk;
  p.pos = at;
  p.yaw = yaw;
  p.seed = seed;
  p.speed = 1.0f + static_cast<f32>((seed >> 7) % 66) / 100;
  p.fitness = 0.75f + static_cast<f32>((seed >> 11) % 51) / 100;
  p.shirt = kShirts[(seed >> 3) % 10];
  p.pants = kPants[(seed >> 9) % 6];
  p.skin = kSkins[(seed >> 13) % 5];
  for (std::size_t i = 0; i < people_.size(); ++i)
    if (people_[i].state == State::gone) { people_[i] = p; return static_cast<i32>(i); }
  people_.push_back(p);
  return static_cast<i32>(people_.size() - 1);
}

i32 People::add_scripted(Kind k, Vec3 at, f32 yaw, f32 hp, u32 seed, Vec3 shirt, Vec3 pants, Vec3 skin, u8 role) {
  Person p;
  p.kind = k;
  p.state = State::officer;  // moved by its owner, not by walk()
  p.pos = at;
  p.yaw = yaw;
  p.hp = hp;
  p.seed = seed;
  p.shirt = shirt;
  p.pants = pants;
  p.skin = skin;
  p.role = role;
  for (std::size_t i = 0; i < people_.size(); ++i)
    if (people_[i].state == State::gone) { people_[i] = p; return static_cast<i32>(i); }
  people_.push_back(p);
  return static_cast<i32>(people_.size() - 1);
}

u32 People::count(Kind k, bool alive_only) const {
  u32 n = 0;
  for (const Person& p : people_)
    if (p.kind == k && p.state != State::gone && (!alive_only || p.state != State::down)) ++n;
  return n;
}

bool People::witnessed(Vec3 at) const {
  for (const Person& p : people_)
    if (p.kind == Kind::citizen && (p.state == State::walk || p.state == State::wait || p.state == State::flee) && dist2d(p.pos, at) < kWitness) return true;
  return false;
}

void People::spawn(const PeopleView& view, Rng& rng, f32 now) {
  const std::vector<Lane>& lanes = view.city->lanes();
  if (lanes.empty()) return;
  u32 alive = count(Kind::citizen, false);
  const f32 near = alive < budget_ / 2 ? 12.0f : 70.0f;  // the first fill may start close by
  for (int made = 0, attempt = 0; alive < budget_ && made < 6 && attempt < 40; ++attempt) {
    Person p;
    p.lane = static_cast<i32>(rng.next() % lanes.size());
    const Lane& L = lanes[p.lane];
    if (!L.alive || L.points.size() < 2) continue;
    p.seg = static_cast<i32>(rng.next() % (L.points.size() - 1));
    const Vec2 a = L.points[p.seg], b = L.points[p.seg + 1];
    const f32 len = std::hypot(b.x - a.x, b.y - a.y);
    if (len < 3) continue;
    p.along = rng.range(0, len);
    p.dir = rng.unit() < 0.5f ? 1 : -1;
    p.side = rng.unit() < 0.5f ? 1.0f : -1.0f;
    p.off = L.width * 0.5f + kPavement;
    Vec3 at;
    f32 heading;
    route_point(p, lanes, at, heading);
    const f32 d = dist2d(at, view.focus);
    if (d < near || d > kShow) continue;
    if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) {
      p.side = -p.side;  // the other pavement, else closer to the kerb
      route_point(p, lanes, at, heading);
      if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) {
        p.off = L.width * 0.5f + 0.5f;
        route_point(p, lanes, at, heading);
        if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) continue;
      }
    }
    const u32 seed = serial_++ * 2654435761u;
    p.seed = seed;
    p.pos = at;
    p.yaw = heading;
    p.speed = 1.0f + static_cast<f32>((seed >> 7) % 66) / 100;  // 1.00 .. 1.65 m/s
    p.fitness = 0.75f + static_cast<f32>((seed >> 11) % 51) / 100;
    p.shirt = kShirts[(seed >> 3) % 10];
    p.pants = kPants[(seed >> 9) % 6];
    p.skin = kSkins[(seed >> 13) % 5];
    p.phase = rng.range(0, 6.28f);
    p.state = State::walk;
    bool placed = false;
    for (Person& slot : people_)
      if (slot.state == State::gone) { slot = p; placed = true; break; }
    if (!placed) people_.push_back(p);
    ++alive;
    ++made;
    (void)now;
  }
}

void People::frighten(Vec3 at, f32 radius, f32 now, f32 strength) {
  for (Person& p : people_) {
    if (p.kind != Kind::citizen || p.state == State::down || p.state == State::gone || p.state == State::fight) continue;
    const f32 d = dist2d(p.pos, at);
    if (d > radius) continue;
    // Close by everyone runs; further out about half do (pedestrian_agents.mjs frighten).
    const u32 roll_bits = (p.seed ^ static_cast<u32>(now * 1000)) * 2246822519u;
    const f32 roll = static_cast<f32>(roll_bits >> 8) / 16777216.0f;
    if (d > radius * 0.45f && roll < 0.5f) continue;
    Vec2 away{p.pos.x - at.x, p.pos.y - at.y};
    const f32 l = std::hypot(away.x, away.y);
    away = l > 0.3f ? away * (1.0f / l) : Vec2{std::cos(roll * 6.28f), std::sin(roll * 6.28f)};
    const f32 s = strength;
    p.flee_dir = away;
    p.flee_speed = clampf((3.4f + 1.5f * s) * p.fitness, 2.8f, 7.2f);
    p.flee_until = std::fmax(p.flee_until, now + 5.6f * (1.3f + 0.7f * roll));
    if (s >= 1) {
      p.burst_until = std::fmax(p.burst_until, now + 1.8f + 2.6f * std::fmin(1.6f, s) * roll);
      if (roll < 0.55f) p.hands_up_until = now + 0.9f + 1.8f * roll;
    }
    p.shaken_until = p.flee_until + 12 + 10 * roll;
    p.state = State::flee;
  }
}

void People::kill(Person& p, Vec3 from_dir, bool by_player, f32 now, bool hit_by_car) {
  if (p.state == State::down || p.state == State::gone) return;
  const Vec3 at = p.pos;
  const bool officer = p.kind == Kind::officer;
  p.state = State::down;
  p.hp = 0;
  p.down_at = now;
  p.v = 0;
  const f32 l = std::hypot(from_dir.x, from_dir.y);
  p.fall_dir = l > 1e-3f ? Vec3{from_dir.x / l, from_dir.y / l, 0} : Vec3{std::cos(p.yaw), std::sin(p.yaw), 0};
  deaths_.push_back({static_cast<i32>(&p - people_.data()), p.kind, by_player, last_head_});
  // Soldiers and zombies are no crime (military_response.mjs, zombie_nights.mjs); a sandbox
  // enemy down counts as "police-kill" severity 1 (sandbox_game.mjs killEnemy).
  if (by_player && (p.kind == Kind::citizen || officer)) {
    const wanted::Crime c = officer ? wanted::Crime::police_officer : wanted::Crime::person;
    reports_.push_back({c, at, officer || witnessed(at)});
  } else if (by_player && p.kind == Kind::enemy) reports_.push_back({wanted::Crime::person_hit, at, true});
  (void)hit_by_car;
  frighten(at, 22, now, 1.2f);
}

i32 People::shot(Vec3 from, Vec3& to, f32 damage, bool by_player, f32 now) {
  i32 best = -1;
  f32 best_s = 2;
  bool head = false;
  Vec3 hit_point;
  for (std::size_t i = 0; i < people_.size(); ++i) {
    const Person& p = people_[i];
    if (p.state == State::down || p.state == State::gone) continue;
    if (dist2d(p.pos, from) < 0.5f) continue;  // the shooter himself
    f32 s;
    Vec3 on;
    const f32 d2 = segment_distance2(from, to, p.pos + Vec3{0, 0, 0.3f}, p.pos + Vec3{0, 0, kTopZ - kCapsuleR}, s, on);
    if (d2 < kCapsuleR * kCapsuleR && s < best_s) {
      best_s = s;
      best = static_cast<i32>(i);
      head = on.z - p.pos.z > kHeadZ - 0.05f;
      // Where the line enters the capsule, not its closest approach.
      const f32 len = std::fmax(1e-4f, kern::length(to - from));
      const f32 back = std::sqrt(std::fmax(0.0f, kCapsuleR * kCapsuleR - d2)) / len;
      hit_point = from + (to - from) * std::fmax(0.0f, s - back);
    }
  }
  last_head_ = false;
  if (best < 0) return -1;
  Person& p = people_[static_cast<std::size_t>(best)];
  to = hit_point;
  last_head_ = head;
  if (by_player && p.kind == Kind::citizen) { p.provoked_at = now; p.provoked_hit = true; }
  p.hp -= damage * (head ? 3.0f : 1.0f);  // VS zones: head x3
  const Vec3 dir = to - from;
  if (p.hp <= 0) kill(p, dir, by_player, now, false);
  else {
    if (by_player && p.kind == Kind::citizen) reports_.push_back({wanted::Crime::person_hit, p.pos, true});
    if (p.kind == Kind::citizen) frighten(p.pos, 3, now, 1.6f);
  }
  return best;
}

void People::blast(Vec3 at, f32 radius, f32 damage, bool by_player, f32 now) {
  for (Person& p : people_) {
    if (p.state == State::down || p.state == State::gone) continue;
    const f32 d = kern::length(p.pos + Vec3{0, 0, 0.9f} - at);
    if (d >= radius) continue;
    const f32 t = clampf(1 - d / radius, 0, 1);
    p.hp -= damage * t * t * (3 - 2 * t);
    if (p.hp <= 0) kill(p, p.pos - at, by_player, now, false);
  }
  frighten(at, radius * 2.5f, now, 1.6f);
}

void People::walk(Person& p, f32 dt, f32 now, const PeopleView& view, Rng& rng) {
  const std::vector<Lane>& lanes = view.city->lanes();
  const kern::physics::World& w = *view.physics;
  if (p.state == State::flee) {
    if (now < p.flee_until) {
      const f32 top = now < p.burst_until ? std::fmax(p.flee_speed, std::fmin(8.5f, 6.6f * p.fitness)) : p.flee_speed;
      p.v += clampf(top - p.v, -3 * dt, 4.2f * dt);
      if (!free_move(w, p, p.flee_dir, p.v * dt, true)) p.flee_dir = p.flee_dir * -1.0f;
      p.yaw = approach_angle(p.yaw, std::atan2(p.flee_dir.y, p.flee_dir.x), dt * 6);
      return;
    }
    // Calm again: back to the nearest pavement and walk on, still hurried for a while.
    f32 best = 30;
    for (std::size_t li = 0; li < lanes.size(); ++li) {
      const Lane& L = lanes[li];
      if (!L.alive) continue;
      for (std::size_t k = 0; k + 1 < L.points.size(); ++k) {
        const Vec2 a = L.points[k], b = L.points[k + 1], d = b - a;
        const f32 l2 = d.x * d.x + d.y * d.y;
        if (l2 < 1) continue;
        const f32 t = clampf(((p.pos.x - a.x) * d.x + (p.pos.y - a.y) * d.y) / l2, 0, 1);
        const Vec2 q = a + d * t;
        const f32 dd = std::hypot(p.pos.x - q.x, p.pos.y - q.y);
        if (dd < best) {
          best = dd;
          p.lane = static_cast<i32>(li);
          p.seg = static_cast<i32>(k);
          p.along = t * std::sqrt(l2);
          p.dir = rng.unit() < 0.5f ? 1 : -1;
          const f32 cross = d.x * (p.pos.y - a.y) - d.y * (p.pos.x - a.x);  // > 0: left of the lane
          const f32 right_of_lane = cross < 0 ? 1.0f : -1.0f;
          p.side = right_of_lane * static_cast<f32>(p.dir);
          p.off = L.width * 0.5f + kPavement;
        }
      }
    }
    p.state = State::wait;
    p.wait_until = now + 0.8f + rng.unit() * 1.5f;
    return;
  }
  if (p.state == State::wait) {
    p.v = std::fmax(0.0f, p.v - 3 * dt);
    if (now >= p.wait_until) p.state = State::walk;
    return;
  }
  // Walking along the pavement: advance the route, then step towards its point.
  const f32 hurry = now < p.shaken_until ? 1.45f : 1.0f;
  p.v += clampf(p.speed * hurry - p.v, -3 * dt, 1.5f * dt);
  if (p.lane < 0 || p.lane >= static_cast<i32>(lanes.size()) || !lanes[p.lane].alive) { p.state = State::gone; return; }
  const Lane& L = lanes[p.lane];
  const i32 n = static_cast<i32>(L.points.size());
  auto seg_len = [&](i32 k) { const Vec2 d = L.points[k + 1] - L.points[k]; return std::hypot(d.x, d.y); };
  Vec3 target;
  f32 heading;
  route_point(p, lanes, target, heading);
  const f32 behind = dist2d(target, p.pos);
  if (behind < 1.6f) {  // on the route: walk it (catching up first when pushed off it)
    p.along += p.dir * p.v * dt;
    // Along the polyline: `along` counts from the segment's first point in the lane's own direction.
    while (p.dir > 0 && p.along > seg_len(p.seg)) {
      p.along -= seg_len(p.seg);
      if (++p.seg >= n - 1) break;
    }
    while (p.dir < 0 && p.along < 0) {
      if (--p.seg < 0) break;
      p.along += seg_len(p.seg);
    }
    if (p.seg < 0 || p.seg >= n - 1) {
      // End of the street: go on along a street that starts here, else turn round (and keep to
      // the same pavement: the side flips with the walking direction).
      const Vec2 end = p.seg < 0 ? L.points.front() : L.points.back();
      i32 next = -1, next_dir = 1;
      const u32 start = rng.next();
      for (std::size_t k = 0; k < lanes.size() && next < 0; ++k) {
        const std::size_t j = (start + k) % lanes.size();
        if (static_cast<i32>(j) == p.lane || !lanes[j].alive || lanes[j].points.size() < 2) continue;
        const Vec2 a = lanes[j].points.front(), z = lanes[j].points.back();
        if (std::hypot(a.x - end.x, a.y - end.y) < 4) { next = static_cast<i32>(j); next_dir = 1; }
        else if (std::hypot(z.x - end.x, z.y - end.y) < 4) { next = static_cast<i32>(j); next_dir = -1; }
      }
      if (next >= 0) {
        p.lane = next;
        p.dir = next_dir;
        const Lane& N = lanes[next];
        p.seg = next_dir > 0 ? 0 : static_cast<i32>(N.points.size()) - 2;
        const Vec2 d = N.points[p.seg + 1] - N.points[p.seg];
        p.along = next_dir > 0 ? 0 : std::hypot(d.x, d.y);
        p.off = N.width * 0.5f + kPavement;
      } else {
        p.dir = -p.dir;
        p.side = -p.side;
        p.seg = std::clamp(p.seg, 0, n - 2);
        p.along = p.dir > 0 ? 0 : seg_len(p.seg);
      }
      if (rng.unit() < 0.25f) { p.state = State::wait; p.wait_until = now + 1 + rng.unit() * 2.5f; }
    }
    route_point(p, lanes, target, heading);
  }
  const Vec2 to{target.x - p.pos.x, target.y - p.pos.y};
  const f32 dl = std::hypot(to.x, to.y);
  if (dl > 0.02f) {
    const f32 step = std::fmin(dl, p.v * dt * (behind < 1.6f ? 1.25f : 1.6f));
    if (behind < 1.6f) { p.pos.x += to.x / dl * step; p.pos.y += to.y / dl * step; }
    else free_move(w, p, {to.x / dl, to.y / dl}, step, false);
    p.yaw = approach_angle(p.yaw, std::atan2(to.y, to.x), dt * 4.5f);
  }
}

void People::update(f32 dt, f32 now, const PeopleView& view, Rng& rng) {
  if (!view.physics || !view.city) return;
  if (dt <= 0) return;
  for (Person& p : people_) {
    if (p.state == State::gone) continue;
    if (p.state == State::down) {
      if (now - p.down_at > kBodySeconds || dist2d(p.pos, view.focus) > kHide) p.state = State::gone;
      continue;
    }
    if (p.kind == Kind::citizen) {
      if (dist2d(p.pos, view.focus) > kHide) { p.state = State::gone; continue; }
      if (p.state != State::fight) walk(p, dt, now, view, rng);
    }
    p.phase += p.v * dt * 3.4f;
    // On the terrain (terrain.h): everybody stands on the ground's surface.
    p.pos.z = view.city->ground_height(p.pos.x, p.pos.y);
    // Vehicles: knocked down by anything driving into them (SpawnCity: person-hit / person).
    for (const VehicleView& v : view.vehicles) {
      if (v.airborne) {
        // A drone or jet sweeping low over people scatters them.
        const f32 agl = v.position.z;
        if (p.kind == Kind::citizen && p.state != State::flee && agl < 14 && dist2d(v.position, p.pos) < 14 && kern::length(v.velocity) > 2)
          frighten(p.pos, 6, now, 1);
        continue;
      }
      const f32 speed = std::hypot(v.velocity.x, v.velocity.y);
      if (speed < 2.5f || dist2d(v.position, p.pos) > 8) continue;
      const Vec3 local = to_local(v.rotation, p.pos - v.position);
      if (std::fabs(local.x) > v.half.x + 0.35f || std::fabs(local.y) > v.half.y + 0.35f) continue;
      const f32 damage = (speed - 2) * 14;
      p.hp -= damage;
      if (p.hp <= 0) kill(p, v.velocity, v.player, now, true);
      else {
        // Thrown aside, out of the footprint.
        const Vec3 side = v.rotation.rotate({0, local.y >= 0 ? 1.0f : -1.0f, 0});
        p.pos = p.pos + side * (v.half.y + 0.6f - std::fabs(local.y));
        if (v.player && p.kind == Kind::citizen) reports_.push_back({wanted::Crime::person_hit, p.pos, true});
        if (p.kind == Kind::citizen) frighten(p.pos, 8, now, 1.4f);
      }
      break;
    }
  }
  spawn(view, rng, now);
}

}  // namespace sc
