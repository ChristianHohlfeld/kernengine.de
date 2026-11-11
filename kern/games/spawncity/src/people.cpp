// The town's people (see people.h).
#include "people.h"
#include "person_hit.h"

#include <algorithm>
#include <cmath>

namespace sc {

namespace {

// pedestrian_agents.mjs / world_procedural_population.mjs
// world_procedural_population.mjs PEOPLE_SHOW_M / PEOPLE_HIDE_M: 170 / 190 m (phone 140 / 160), set_ranges
constexpr f32 kPavement = 1.6f;               // pavement centre beyond the carriageway edge
constexpr f32 kWitness = 45;                  // wanted_system_logic: a witness within sight
constexpr f32 kBodySeconds = 25;              // a body stays this long
constexpr f32 kCapsuleR = 0.32f;

const Vec3 kShirts[] = {{0.62f, 0.12f, 0.1f}, {0.16f, 0.28f, 0.55f}, {0.85f, 0.82f, 0.74f}, {0.2f, 0.42f, 0.25f}, {0.9f, 0.62f, 0.15f},
                        {0.35f, 0.35f, 0.38f}, {0.55f, 0.3f, 0.55f}, {0.1f, 0.1f, 0.12f}, {0.75f, 0.45f, 0.4f}, {0.3f, 0.6f, 0.7f}};
const Vec3 kPants[] = {{0.12f, 0.14f, 0.22f}, {0.2f, 0.2f, 0.22f}, {0.38f, 0.3f, 0.22f}, {0.08f, 0.08f, 0.09f}, {0.45f, 0.45f, 0.48f}, {0.25f, 0.3f, 0.45f}};
const Vec3 kSkins[] = {{0.94f, 0.76f, 0.62f}, {0.78f, 0.58f, 0.43f}, {0.66f, 0.44f, 0.32f}, {0.48f, 0.31f, 0.22f}, {0.86f, 0.66f, 0.5f}};

f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 approach_angle(f32 cur, f32 target, f32 max_step) { return cur + clampf(wrap_pi(target - cur), -max_step, max_step); }

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

namespace {
// the nearest player (MP: the host's people live around every player)
f32 near_any(const PeopleView& v, Vec3 p) {
  f32 d = dist2d(p, v.focus);
  for (const Vec3& f : v.foci) d = std::fmin(d, dist2d(p, f));
  return d;
}
}  // namespace

void People::dress(Person& p) {
  const u32 seed = p.seed;
  p.speed = 1.0f + static_cast<f32>((seed >> 7) % 66) / 100;
  p.fitness = 0.75f + static_cast<f32>((seed >> 11) % 51) / 100;
  p.shirt = kShirts[(seed >> 3) % 10];
  p.pants = kPants[(seed >> 9) % 6];
  p.skin = kSkins[(seed >> 13) % 5];
}

void People::apply_snapshot(const std::vector<NetPerson>& list, f32 now) {
  std::vector<bool> seen(people_.size(), false);
  for (const NetPerson& n : list) {
    i32 at = -1;
    for (std::size_t i = 0; i < people_.size(); ++i)
      if (people_[i].kind == Kind::citizen && people_[i].net == n.idx && people_[i].state != State::gone) { at = static_cast<i32>(i); break; }
    const bool host_down = n.state == static_cast<u8>(State::down);
    if (at < 0) {
      if (host_down) continue;  // a body we never saw walk: not worth showing
      Person p;
      p.kind = Kind::citizen;
      p.seed = n.seed;
      dress(p);
      p.net = n.idx;
      p.pos = p.net_to = n.pos;
      p.yaw = n.yaw;
      p.state = static_cast<State>(n.state <= static_cast<u8>(State::fight) ? n.state : 0);
      bool placed = false;
      for (std::size_t i = 0; i < people_.size(); ++i)
        if (people_[i].state == State::gone) { people_[i] = p; at = static_cast<i32>(i); placed = true; break; }
      if (!placed) { people_.push_back(p); seen.push_back(false); at = static_cast<i32>(people_.size() - 1); }
    }
    seen[static_cast<std::size_t>(at)] = true;
    Person& p = people_[static_cast<std::size_t>(at)];
    if (p.seed != n.seed) { p.seed = n.seed; dress(p); p.pos = n.pos; }
    if (p.state == State::down) continue;
    if (host_down) { kill(p, Vec3{std::cos(n.yaw), std::sin(n.yaw), 0}, false, now, false); continue; }
    if ((n.flags & 1) && now >= p.knocked_until) { knock(p, Vec3{std::cos(n.yaw), std::sin(n.yaw), 0.5f}, now); continue; }
    p.net_to = n.pos;
    p.net_at = n.at;
    p.v = n.v;
    if (now >= p.knocked_until) {
      p.state = static_cast<State>(n.state <= static_cast<u8>(State::fight) ? n.state : 0);
      // far off (a respawn of his, a teleport): there at once
      if (dist2d(p.pos, n.pos) > 6) p.pos = n.pos;
      p.yaw = n.yaw;
    }
  }
  for (std::size_t i = 0; i < seen.size() && i < people_.size(); ++i) {
    Person& p = people_[i];
    if (seen[i] || p.kind != Kind::citizen || p.net < 0) continue;
    if (p.state == State::down || now < p.knocked_until) continue;  // a body / a ragdoll stays its time
    p.state = State::gone;
  }
}

void People::spawn(const PeopleView& view, Rng& rng, f32 now) {
  if (puppets_) return;  // MP client: the host's citizens
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
    // world_procedural_population.mjs seedRouteAgents: the people of a street are the street's -
    // one in every 9 m slot of a pavement, its id from the street's own geometry, the segment, the
    // slot and the side: every machine in a round has the same person in the same place (and a
    // death there is a death here, "npc-death")
    const u32 slots = std::max(1u, static_cast<u32>(len / 9));
    const u32 slot = static_cast<u32>(rng.next() % slots);
    p.along = (static_cast<f32>(slot) + 0.5f) * len / static_cast<f32>(slots);
    p.side = rng.unit() < 0.5f ? 1.0f : -1.0f;
    const u32 street = static_cast<u32>(std::lround(L.points.front().x)) * 73856093u ^ static_cast<u32>(std::lround(L.points.front().y)) * 19349663u ^
                       static_cast<u32>(std::lround(L.points.back().x)) * 83492791u ^ static_cast<u32>(std::lround(L.points.back().y)) * 2971215073u;
    const u32 id = (street ^ (static_cast<u32>(p.seg) * 2654435761u) ^ (slot * 40503u) ^ (p.side > 0 ? 0x9e3779b9u : 0u)) | 1u;
    bool taken = false;
    for (const Person& o : people_) taken = taken || (o.state != State::gone && o.seed == id);
    if (taken) continue;
    p.dir = (id >> 5) & 1 ? 1 : -1;
    p.off = L.width * 0.5f + kPavement;
    Vec3 at;
    f32 heading;
    route_point(p, lanes, at, heading);
    // around a player: ours, or (MP host) one of the others - near none of them, far from none
    const f32 d = near_any(view, at);
    const Vec3 around = view.foci.empty() || (attempt % (view.foci.size() + 1)) == 0 ? view.focus : view.foci[attempt % (view.foci.size() + 1) - 1];
    if (d < near || dist2d(at, around) > show_) continue;
    if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) {
      p.side = -p.side;  // the other pavement, else closer to the kerb
      route_point(p, lanes, at, heading);
      if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) {
        p.off = L.width * 0.5f + 0.5f;
        route_point(p, lanes, at, heading);
        if (inside_building(*view.physics, at, view.city->ground_height(at.x, at.y))) continue;
      }
    }
    if (view.can_spawn && !view.can_spawn(at)) continue;  // nobody may see them appear
    const u32 seed = id;
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
  falls_.push_back({static_cast<i32>(&p - people_.data()), from_dir, hit_by_car, false});  // ragdoll (M3 life)
  // Soldiers and zombies are no crime (military_response.mjs, zombie_nights.mjs); a sandbox
  // enemy down counts as "police-kill" severity 1 (sandbox_game.mjs killEnemy).
  if (by_player && (p.kind == Kind::citizen || officer)) {
    const wanted::Crime c = officer ? wanted::Crime::police_officer : wanted::Crime::person;
    reports_.push_back({c, at, officer || witnessed(at)});
  } else if (by_player && p.kind == Kind::enemy) reports_.push_back({wanted::Crime::person_hit, at, true});
  (void)hit_by_car;
  frighten(at, 22, now, 1.2f);
}

void People::mate_hurt(i32 index, u32 seed, f32 damage, HurtBy by, Vec3 push, f32 now) {
  if (index < 0 || index >= static_cast<i32>(people_.size()) || !(damage > 0)) return;
  Person& p = people_[static_cast<std::size_t>(index)];
  if (p.kind != Kind::citizen || p.seed != seed || p.state == State::down || p.state == State::gone || now < p.knocked_until) return;
  p.hp -= damage;
  p.hurt_at = now;
  if (p.hp <= 0) { kill(p, push, false, now, by == HurtBy::vehicle); return; }
  if (by == HurtBy::vehicle) {
    falls_.push_back({index, push, true, true});
    p.knocked_until = now + 6;
    p.v = 0;
    frighten(p.pos, 8, now, 1.4f);
    return;
  }
  if (by == HurtBy::blow && p.hp > 35) p.v = 0;  // a blow he takes standing: a stagger
  else knock(p, push, now);
  frighten(p.pos, 3, now, 1.6f);
}

i32 People::shot(Vec3 from, Vec3& to, f32 damage, bool by_player, f32 now, f32 melee_ns, f32 self_r) {
  i32 best = -1;
  f32 best_s = 2;
  bool head = false;
  Vec3 hit_point;
  for (std::size_t i = 0; i < people_.size(); ++i) {
    const Person& p = people_[i];
    if (p.state == State::down || p.state == State::gone) continue;
    if (dist2d(p.pos, from) < self_r) continue;  // the shooter himself
    if (p.kind == Kind::citizen && now < p.knocked_until) continue;  // down as a ragdoll: the body is hidden
    // the body as it is drawn (person_hit.h): legs, the leaning torso with the arms, the head
    f32 s = 2;
    const BodyPart part = person_hit::hit(p.pos, p.yaw, p.v, from, to - from, s, 1.08f);
    if (part != BodyPart::none && s < best_s) {
      best_s = s;
      best = static_cast<i32>(i);
      head = part == BodyPart::head;
      hit_point = from + (to - from) * s;
    }
  }
  last_head_ = false;
  if (best < 0) return -1;
  Person& p = people_[static_cast<std::size_t>(best)];
  to = hit_point;
  last_head_ = head;
  if (damage > 0 || melee_ns >= 0) p.hurt_at = now;
  if (by_player && p.kind == Kind::citizen) { p.provoked_at = now; p.provoked_hit = true; }
  const Vec3 dir = to - from;
  if (p.kind == Kind::citizen && damage > 0) {
    const f32 hd = std::fmax(1e-3f, std::hypot(dir.x, dir.y));
    const Vec3 flat{dir.x / hd, dir.y / hd, 0};
    const f32 hp_before = p.hp;
    if (melee_ns >= 0) {
      // world_procedural_population.mjs punch(): hp - damage; dead at 0, down at 35, else a stagger
      const f32 k = std::fmax(0.5f, melee_ns / 30);
      p.hp -= damage;
      if (p.hp <= 0) kill(p, dir, by_player, now, false);
      else if (p.hp <= 35) knock(p, flat * (2 * k) + Vec3{0, 0, 0.9f}, now);
      else { const f32 step = std::fmin(0.6f, 0.22f * k); p.pos = p.pos + flat * step; p.v = 0; }
    } else {
      // gameplay_final_runtime_v2.mjs: weaponDamage 55 (default), sniper 200; headshots SMG 165,
      // Glock / sniper 999 (Kern's damage 13 / 26 / 95 tells the gun)
      const f32 dmg = head ? (damage >= 26 ? 999.0f : 165.0f) : (damage >= 90 ? 200.0f : 55.0f);
      p.hp -= dmg;
      if (p.hp <= 0) kill(p, dir, by_player, now, false);
      else knock(p, flat * 2.5f + Vec3{0, 0, 1.4f}, now);
    }
    if (p.state != State::down) {
      if (by_player) reports_.push_back({wanted::Crime::person_hit, p.pos, true});
      frighten(p.pos, 3, now, 1.6f);
      // the host's citizen (MP client): what we did to him goes to the host (take_hurts)
      if (by_player && puppets_ && p.net >= 0)
        hurts_.push_back({static_cast<u8>(p.net), p.seed, hp_before - p.hp, melee_ns >= 0 ? HurtBy::blow : HurtBy::round,
                          melee_ns >= 0 ? flat * (2 * std::fmax(0.5f, melee_ns / 30)) + Vec3{0, 0, 0.9f} : flat * 2.5f + Vec3{0, 0, 1.4f}});
    }
    return best;
  }
  f32 dealt = damage * (head ? 3.0f : 1.0f);  // VS zones: head x3
  if (p.kind == Kind::officer && damage > 0 && melee_ns < 0) {
    // police_ground_units.mjs registerPoliceHit: HIT_DAMAGE 25 per counted hit; the gun decides how
    // often a round counts (gameplay_final_runtime_v2.mjs): MP 1 (head 3, routeKill max 3), Glock and
    // sniper 6 (hitWeight; head 10)
    const f32 hits = damage >= 26 ? (head ? 10.0f : 6.0f) : (head ? 3.0f : 1.0f);
    dealt = 25 * hits;
  }
  // a mate's unit: the hit is his to apply (take_remote_hits), by the same rule
  if (p.remote) {
    if (by_player && dealt > 0) remote_hits_.push_back({best, dealt});
    return best;
  }
  p.hp -= dealt;
  if (p.hp <= 0) kill(p, dir, by_player, now, false);
  else {
    if (by_player && p.kind == Kind::citizen) reports_.push_back({wanted::Crime::person_hit, p.pos, true});
    if (p.kind == Kind::citizen) frighten(p.pos, 3, now, 1.6f);
  }
  return best;
}

void People::blast(Vec3 at, f32 radius, f32 damage, bool by_player, f32 now) {
  for (Person& p : people_) {
    if (p.state == State::down || p.state == State::gone || p.remote) continue;  // (a mate's unit: his machine runs the blast on it)
    const f32 d = kern::length(p.pos + Vec3{0, 0, 0.9f} - at);
    if (d >= radius) continue;
    const f32 t = clampf(1 - d / radius, 0, 1);
    if (p.kind == Kind::citizen) {
      // gameplay_final_runtime_v2.mjs dispatchExplosion: a person within the falloff > .22 takes one
      // hit (55): the first knocks down, the next kills; lying there it takes nothing
      if (t * t * (3 - 2 * t) <= 0.22f || now < p.knocked_until) continue;
      p.hp -= 55;
      if (p.hp <= 0) kill(p, p.pos - at, by_player, now, false);
      else knock(p, p.pos - at, now);
      continue;
    }
    if (p.kind == Kind::officer) {
      // police_ground_units.mjs onExplosion: an officer out of the car within r (2..60) takes
      // COP_HP (1 - d / r) 1.6 + 20, d across the ground
      const f32 r = clampf(radius, 2, 60), d2 = dist2d(p.pos, at);
      if (d2 < r) p.hp -= 100 * (1 - d2 / r) * 1.6f + 20;
    } else
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
      if (now - p.down_at > kBodySeconds || near_any(view, p.pos) > hide_) p.state = State::gone;
      continue;
    }
    if (now < p.knocked_until) continue;  // lying on the street as a ragdoll (M3 life)
    if (p.kind == Kind::citizen && puppets_ && p.net >= 0) {
      // the host's citizen: where he is by now - his snapshot's place walked on at his pace and
      // heading for the time since it was taken (no stop-and-go between snapshots) - eased to
      const f32 age = clampf(now - p.net_at, 0, 0.8f);
      const Vec3 want = p.net_to + Vec3{std::cos(p.yaw), std::sin(p.yaw), 0} * (p.v * age);
      const Vec3 d = want - p.pos;
      const f32 k = std::fmin(1.0f, dt * 10);
      p.pos.x += d.x * k;
      p.pos.y += d.y * k;
    } else if (p.kind == Kind::citizen) {
      if (near_any(view, p.pos) > hide_) { p.state = State::gone; continue; }
      if (p.state != State::fight) walk(p, dt, now, view, rng);
    }
    p.phase += p.v * dt * 3.4f;
    // On the terrain (terrain.h): everybody stands on the ground's surface.
    p.pos.z = view.city->ground_height(p.pos.x, p.pos.y);
    if (p.remote) continue;  // a mate's unit stands where he has it: nothing here throws it
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
      if (speed < 2.2f || dist2d(v.position, p.pos) > 8) continue;  // people_impacts.mjs MIN_SPEED
      const Vec3 local = to_local(v.rotation, p.pos - v.position);
      if (std::fabs(local.x) > v.half.x + 0.32f || std::fabs(local.y) > v.half.y + 0.32f) continue;  // people_impacts.mjs PAD
      const f32 damage = std::fmin(260.0f, std::fmax(6.0f, (speed - 1.5f) * 8.5f));  // people_impacts.mjs
      p.hp -= damage;
      if (p.hp <= 0) kill(p, v.velocity, v.player, now, true);
      else {
        // Thrown aside as a ragdoll that gets up again (people_impacts.mjs: the car's momentum,
        // sideways 1.4 + 0.12 v, up 1.2 + 0.16 v, at most 15 m/s).
        const f32 sd = local.y >= 0 ? 1.0f : -1.0f;
        Vec3 push = Vec3{v.velocity.x, v.velocity.y, 0} * 1.05f + v.rotation.rotate({0, sd, 0}) * (1.4f + speed * 0.12f) + Vec3{0, 0, 1.2f + speed * 0.16f};
        const f32 pl = kern::length(push);
        if (pl > 15) push = push * (15 / pl);
        falls_.push_back({static_cast<i32>(&p - people_.data()), push, true, true});
        p.knocked_until = now + 6;
        p.v = 0;
        if (v.player && p.kind == Kind::citizen && puppets_ && p.net >= 0) hurts_.push_back({static_cast<u8>(p.net), p.seed, damage, HurtBy::vehicle, push});  // the host's citizen under our car
        if (v.player && p.kind == Kind::citizen) reports_.push_back({wanted::Crime::person_hit, p.pos, true});
        if (p.kind == Kind::citizen) frighten(p.pos, 8, now, 1.4f);
      }
      break;
    }
  }
  spawn(view, rng, now);
}

}  // namespace sc
