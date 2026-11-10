// Street life and animals (see life.h). Values from SpawnCity's sim/*.mjs as named.
#include "life.h"

#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstdio>

namespace sc {

namespace {
namespace ph = kern::physics;

// cows.mjs
constexpr u32 kMaxCows = 40;
constexpr f32 kMinArea = 1800, kHerdRadius = 420, kPeriod = 24, kWalk = 0.55f, kCowMass = 600, kGrazeR = 6, kSpreadR = 14;
const u32 kCoats[5] = {0xf1efe8, 0xf1efe8, 0x8a5a3c, 0x3a2c24, 0xcfa977};
// pigeons.mjs
constexpr u32 kPerFlockMin = 6, kPerFlockMax = 12;
constexpr f32 kNear = 22, kFar = 95, kDespawn = 150, kSpookPlayer = 4.2f, kSpookCar = 8;
// jogger_chain.mjs
constexpr u32 kRunners = 10;
constexpr f32 kGap = 1.35f, kSpeed = 2.9f, kChainLife = 95, kFirst = 60, kEveryMin = 170, kEveryMax = 280, kStreakWindow = 12;
const char* kCalls[6][2] = {{"Wer l\xc3\xa4uft mit?", "Wir laufen mit!"}, {"Links, rechts, links!", "Wir sind nicht zu bremsen!"},
                            {"Eins, zwei, drei, vier!", "Die Stra\xc3\x9f" "e geh\xc3\xb6rt uns hier!"}, {"Wie weit noch?", "Bis zum See und weiter!"},
                            {"Was wollen wir?", "Kilometer!"}, {"Schneller?", "Immer schneller!"}};
// street_voices.mjs
constexpr f32 kMaxHear = 38, kShoutGap = 1.4f, kSourceGap = 6, kHonkGap = 1.2f, kBubble = 2.1f;
struct Lines { const char* mood; const char* lines[6]; u32 n; };
const Lines kLines[] = {
    {"bump", {"Hey, pass doch auf!", "Geht's noch?", "Augen auf, Mann!", "Was soll das denn?", "Mann, ey!", "Hast du keine Augen im Kopf?"}, 6},
    {"angry", {"Na warte!", "Jetzt reicht's mir!", "Willst du \xc3\x84rger?", "Komm her, du Idiot!", "Dir zeig ich's!", "Du hast sie doch nicht alle!"}, 6},
    {"scared", {"Hilfe!", "Weg hier!", "Der ist verr\xc3\xbc" "ckt!", "Lauf!", "Polizei! Hilfe!"}, 5},
    {"nearmiss", {"Bist du blind?!", "Langsam, du Irrer!", "Spinnst du?!", "Pass doch auf, du Vollidiot!", "Fahr doch nicht wie ein Henker!"}, 5},
    {"honked", {"Ja ja, ist ja gut!", "Reg dich ab!", "Hup nicht so!", "Selber!"}, 4},
};

f32 hash01(u64 v) {
  v ^= v >> 33; v *= 0xff51afd7ed558ccdull; v ^= v >> 33; v *= 0xc4ceb9fe1a85ec53ull; v ^= v >> 33;
  return static_cast<f32>(v >> 40) / 16777216.0f;
}
u64 mix(u64 a, u64 b) { return a * 0x9e3779b97f4a7c15ull + b + 0x632be59bd9b4e019ull; }
f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 dist2d(Vec2 a, Vec2 b) { return std::hypot(a.x - b.x, a.y - b.y); }
Vec3 rgb(u32 h) { return {((h >> 16) & 255) / 255.0f, ((h >> 8) & 255) / 255.0f, (h & 255) / 255.0f}; }
f32 ground(const LifeView& v, f32 x, f32 y) { return v.city ? v.city->ground_height(x, y) : 0; }

bool in_ring(const std::vector<Vec2>& r, Vec2 p) {
  bool c = false;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) {
    const Vec2 a = r[i], b = r[j];
    if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y + 1e-12f) + a.x) c = !c;
  }
  return c;
}

// Segment from `a` to `b` against a sphere: the entry fraction, or < 0.
f32 seg_sphere(Vec3 a, Vec3 b, Vec3 c, f32 r) {
  const Vec3 d = b - a, m = a - c;
  const f32 A = kern::dot(d, d), B = kern::dot(m, d), C = kern::dot(m, m) - r * r;
  if (A < 1e-9f) return -1;
  const f32 disc = B * B - A * C;
  if (disc < 0) return -1;
  const f32 t = (-B - std::sqrt(disc)) / A;
  return t >= 0 && t <= 1 ? t : (C < 0 ? 0.0f : -1.0f);
}
}  // namespace

bool Life::in_rings(const std::vector<std::vector<Vec2>>& rings, Vec2 p) {
  if (rings.empty() || !in_ring(rings[0], p)) return false;
  for (std::size_t i = 1; i < rings.size(); ++i)
    if (rings[i].size() > 2 && in_ring(rings[i], p)) return false;
  return true;
}

f32 Life::ring_area(const std::vector<Vec2>& r) {
  f32 a = 0;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) a += (r[j].x + r[i].x) * (r[j].y - r[i].y);
  return std::fabs(a) * 0.5f;
}

// a dog's / cat's round by its index (the same on every machine)
void Life::pet_params(Pet& p, u32 i) {
  const u64 h = mix(0xa11a, i);
  p.radius = 13 + 45 * hash01(h + 1);
  p.omega = (0.55f + 0.9f * hash01(h + 2)) / p.radius;
  p.phase = hash01(h + 3) * 2 * kPi;
  p.wobble_omega = 0.12f + 0.18f * hash01(h + 4);
  p.wobble_r = 1.2f + 2.3f * hash01(h + 5);
  p.scale = 0.72f + 0.46f * hash01(h + 6);
}

void Life::init(kern::physics::World& w, bool mobile) {
  world_ = &w;
  mobile_ = mobile;
  // world_procedural_population.mjs AMBIENT_ANIMAL_COUNT, their circling paths and sizes
  pet_base_ = mobile ? 12 : 20;
  pets_.assign(pet_base_, {});
  for (u32 i = 0; i < pets_.size(); ++i) pet_params(pets_[i], i);
  ragdolls_.init(w, mobile);
  // world_liveliness.mjs BIRD_COUNT
  const u32 n = mobile ? 10 : 16;
  birds_.assign(n, {});
  for (u32 i = 0; i < n; ++i) birds_[i].seed = static_cast<u32>(hash01(mix(0xb17d, i)) * 4294967295.0);
}

void Life::clear() {
  for (Pet& p : pets_) {
    if (p.body.valid() && world_ && world_->body_valid(p.body)) world_->destroy_body(p.body);
    p.body = {};
    p.dead_at = -1;
    p.shown = false;
  }
  cat_ = {};
  pet_anchor_ = {1e9f, 1e9f};
  for (Cow& c : cows_)
    if (c.body.valid() && world_ && world_->body_valid(c.body)) world_->destroy_body(c.body);
  cows_.clear();
  dead_cows_.clear();
  meadows_.clear();
  herds_for_ = {1e9f, 1e9f};
  flocks_.clear();
  trees_.clear();
  trees_gone_.clear();
  tree_serial_ = ~0u;
  chain_alive_ = false;
  next_chain_ = -1;
  streak_n_ = 0;
  voices_.clear();
  ragdolls_.clear();
}

void Life::update(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out) {
  if (!world_ || !view.city || dt <= 0) return;
  update_cows(dt, now, view, out);
  update_pigeons(dt, now, view, rng, out);
  update_birds(dt, now, view, rng, out);
  update_trees(view, now);
  update_pets(dt, now, view, out);
  update_joggers(dt, now, view, rng, out);
  update_voices(dt, now, view, out);
  ragdolls_.update(dt, now);
}

// ---------------------------------------------------------------------------------------------
// cows (cows.mjs): herds on the meadows, a clock-driven grazing walk, a Box3D body when killed

void Life::rebuild_herds(const LifeView& view, f32 now) {
  std::vector<CityStream::AreaRef> areas;
  // around the player and (host) around every mate: the same herds for everybody
  std::vector<Vec3> foci{view.focus};
  foci.insert(foci.end(), view.mates.begin(), view.mates.end());
  for (const Vec3& f : foci) {
    std::vector<CityStream::AreaRef> more;
    view.city->areas_near({f.x, f.y}, kHerdRadius, AreaKind::grass, more);
    for (const CityStream::AreaRef& a : more)
      if (std::none_of(areas.begin(), areas.end(), [&](const CityStream::AreaRef& q) { return q.key == a.key; })) areas.push_back(a);
  }
  struct Cand { Meadow m; f32 d; };
  std::vector<Cand> cand;
  for (const CityStream::AreaRef& a : areas) {
    const f32 area = ring_area(a.area->rings[0]);
    if (area < kMinArea) continue;
    Meadow m;
    m.rings = a.area->rings;
    m.key = a.key;
    m.area = area;
    m.lo = {1e9f, 1e9f};
    m.hi = {-1e9f, -1e9f};
    Vec2 c{};
    for (Vec2 p : m.rings[0]) { m.lo = {std::fmin(m.lo.x, p.x), std::fmin(m.lo.y, p.y)}; m.hi = {std::fmax(m.hi.x, p.x), std::fmax(m.hi.y, p.y)}; c = c + p; }
    m.centre = c * (1.0f / static_cast<f32>(m.rings[0].size()));
    f32 d = 1e9f;
    for (const Vec3& f : foci) d = std::fmin(d, dist2d(m.centre, Vec2{f.x, f.y}));
    if (d > kHerdRadius + std::sqrt(area)) continue;
    cand.push_back({std::move(m), d});
  }
  std::sort(cand.begin(), cand.end(), [](const Cand& a, const Cand& b) { return a.d < b.d; });
  // keep the cows of herds that stay (dead ones keep their bodies), add the new herds
  std::vector<Cow> next;
  std::vector<Meadow> meadows;
  for (Cand& c : cand) {
    const Meadow& m = c.m;
    const u32 n = static_cast<u32>(std::max(2, std::min(7, static_cast<int>(std::lround(m.area / 4000)) + static_cast<int>(hash01(m.key) * 2))));
    const i32 mi = static_cast<i32>(meadows.size());
    meadows.push_back(m);
    // the herd's spot, a home for each cow around it (inside the pasture)
    Vec2 herd = m.centre;
    for (int k = 0; k < 14; ++k) {
      const Vec2 p{m.lo.x + (m.hi.x - m.lo.x) * hash01(mix(m.key, 100 + k)), m.lo.y + (m.hi.y - m.lo.y) * hash01(mix(m.key, 200 + k))};
      if (in_rings(m.rings, p)) { herd = p; break; }
    }
    for (u32 i = 0; i < n && next.size() < kMaxCows; ++i) {
      const u64 key = mix(m.key, i);
      auto old = std::find_if(cows_.begin(), cows_.end(), [&](const Cow& x) { return x.key == key; });
      if (old != cows_.end()) { old->meadow = mi; next.push_back(*old); old->body = {}; continue; }
      Cow c;
      c.key = key;
      c.meadow = mi;
      c.home = herd;
      for (int k = 0; k < 8; ++k) {
        const f32 a = hash01(mix(key, 300 + k)) * 2 * kPi, r = std::sqrt(hash01(mix(key, 400 + k))) * kSpreadR;
        const Vec2 p{herd.x + std::cos(a) * r, herd.y + std::sin(a) * r};
        if (in_rings(m.rings, p)) { c.home = p; break; }
      }
      c.color = rgb(kCoats[static_cast<u32>(hash01(mix(key, 7)) * 5) % 5]);
      c.dead = std::find(dead_cows_.begin(), dead_cows_.end(), key) != dead_cows_.end();
      c.next_moo = now + 8 + hash01(mix(key, 9)) * 30;
      c.phase = hash01(key) * 6;
      const Vec2 g = graze_spot(c, static_cast<i64>(now / kPeriod));
      c.pos = {g.x, g.y, ground(view, g.x, g.y)};
      next.push_back(c);
    }
    if (next.size() >= kMaxCows) break;
  }
  for (Cow& c : cows_)
    if (c.body.valid() && world_->body_valid(c.body)) world_->destroy_body(c.body);
  cows_ = std::move(next);
  meadows_ = std::move(meadows);
}

bool Life::spawn_herd(Vec2 center, f32 radius, u32 n, f32 now) {
  // Tests: a round pasture around `center` with n cows.
  Meadow m;
  m.key = mix(0xc0c0, static_cast<u64>(now * 1000));
  for (int i = 0; i < 24; ++i) {
    const f32 a = static_cast<f32>(i) / 24 * 2 * kPi;
    m.rings.resize(1);
    m.rings[0].push_back({center.x + std::cos(a) * radius, center.y + std::sin(a) * radius});
  }
  m.lo = center - Vec2{radius, radius};
  m.hi = center + Vec2{radius, radius};
  m.centre = center;
  m.area = ring_area(m.rings[0]);
  const i32 mi = static_cast<i32>(meadows_.size());
  meadows_.push_back(m);
  for (u32 i = 0; i < n && cows_.size() < kMaxCows; ++i) {
    Cow c;
    c.key = mix(m.key, i);
    c.meadow = mi;
    const f32 a = static_cast<f32>(i) * 2.4f, r = 2 + static_cast<f32>(i) * 1.5f;
    c.home = {center.x + std::cos(a) * r, center.y + std::sin(a) * r};
    c.color = rgb(kCoats[i % 5]);
    c.next_moo = now + 1;
    c.pos = {c.home.x, c.home.y, 0};
    cows_.push_back(c);
  }
  herds_for_ = center;  // keep this herd until the player moves on
  return true;
}

Vec2 Life::graze_spot(const Cow& c, i64 k) const {
  // A cow grazes around its own patch (GRAZE_R around its home spot): short legs at a cow's pace.
  const Meadow* m = c.meadow >= 0 && c.meadow < static_cast<i32>(meadows_.size()) ? &meadows_[c.meadow] : nullptr;
  for (int j = 0; j < 6; ++j) {
    const u64 h = mix(c.key, static_cast<u64>(k) * 16 + j);
    const f32 a = hash01(h) * 2 * kPi, r = std::sqrt(hash01(h + 1)) * kGrazeR;
    const Vec2 p{c.home.x + std::cos(a) * r, c.home.y + std::sin(a) * r};
    if (!m || in_rings(m->rings, p)) return p;
  }
  return c.home;
}

void Life::scare_cow(Cow& c, Vec2 from, f32 now, f32 secs) {
  if (c.dead) return;
  Vec2 d{c.pos.x - from.x, c.pos.y - from.y};
  const f32 l = std::hypot(d.x, d.y);
  d = l > 1e-3f ? d * (1 / l) : Vec2{1, 0};
  c.flee_v = d * 2.8f;
  c.flee_until = now + secs;
}

void Life::kill_cow(Cow& c, Vec3 impulse, f32 now, LifeOut& out) {
  if (c.dead) return;
  c.dead = true;
  c.hp = 0;
  dead_cows_.push_back(c.key);
  out.cow_deaths.push_back({c.pos, impulse});
  out.sounds.push_back({LifeOut::Sound::moo_pain, c.pos + Vec3{0, 0, 1.4f}, 1, 1.4f});
  // The 600 kg body thrown as a Box3D rigid body (cows.mjs kill: halfExtents .82/.34/.38).
  ph::BodySettings bs;
  bs.type = ph::BodyType::dynamic;
  bs.position = c.pos + Vec3{0, 0, 1.12f};
  bs.rotation = yaw_quat(c.yaw);
  bs.linear_damping = 0.05f;
  bs.angular_damping = 0.3f;
  c.body = world_->create_body(bs);
  ph::ShapeSettings ss;
  ss.kind = ph::ShapeKind::box;
  ss.size = {1.64f, 0.68f, 0.76f};
  ss.material.density = kCowMass / (1.64f * 0.68f * 0.76f);
  ss.material.friction = 0.7f;
  ss.material.restitution = 0.05f;
  ss.material.category_bits = kCatWorld;
  world_->create_shape(c.body, ss);
  world_->apply_linear_impulse(c.body, impulse, c.pos + Vec3{0, 0, 1.45f});
  (void)now;
}

kern::physics::Body Life::grab_animal(Vec3 at, f32 reach) {
  if (!world_) return {};
  f32 best = reach;
  Cow* cow = nullptr;
  i32 pet = -1;
  for (Cow& c : cows_) {
    if (c.dead) continue;
    if (c.carried && c.body.valid() && world_->body_valid(c.body)) {
      const f32 d = kern::length(world_->pose(c.body).position - at);
      if (d < best + 0.6f) { best = d; cow = &c; pet = -1; }
      continue;
    }
    // the hand on its back, its neck or its flank (the body 1.6 m long, 1.45 m high)
    const Vec3 to_local = at - (c.pos + Vec3{0, 0, 1.1f});
    const f32 d = std::fmax(0.0f, kern::length(to_local) - 0.75f);
    if (d < best) { best = d; cow = &c; pet = -1; }
  }
  for (std::size_t i = 0; i < pets_.size(); ++i) {
    Pet& p = pets_[i];
    if (!p.shown || p.dead_at >= 0) continue;
    const Vec3 c = p.carried && p.body.valid() && world_->body_valid(p.body) ? world_->pose(p.body).position : p.pos + Vec3{0, 0, 0.3f * p.scale};
    const f32 d = std::fmax(0.0f, kern::length(at - c) - 0.25f * p.scale);
    if (d < best) { best = d; pet = static_cast<i32>(i); cow = nullptr; }
  }
  if (pet >= 0) {
    Pet& p = pets_[static_cast<std::size_t>(pet)];
    if (p.carried && p.body.valid() && world_->body_valid(p.body)) { p.carried_still = 0; return p.body; }
    const Species sp = species(static_cast<u32>(pet), epoch_);
    const f32 sc = sp == Species::dog ? p.scale : p.scale * 0.62f;
    const Vec3 half = Vec3{0.33f, 0.13f, 0.27f} * sc;
    const f32 mass = (sp == Species::dog ? 22.0f : 4.0f) * sc * sc * sc;
    ph::BodySettings bs;
    bs.type = ph::BodyType::dynamic;
    bs.position = {p.pos.x, p.pos.y, p.pos.z + half.z};
    bs.rotation = yaw_quat(p.yaw);
    bs.linear_damping = 0.1f;
    bs.angular_damping = 1.5f;
    p.body = world_->create_body(bs);
    ph::ShapeSettings ss;
    ss.kind = ph::ShapeKind::box;
    ss.size = half * 2.0f;
    ss.material.density = mass / (8 * half.x * half.y * half.z);
    ss.material.friction = 0.7f;
    ss.material.category_bits = kCatWorld;
    world_->create_shape(p.body, ss);
    p.rot = bs.rotation;
    p.carried = true;
    p.carried_still = 0;
    return p.body;
  }
  if (cow) {
    Cow& c = *cow;
    if (c.carried && c.body.valid() && world_->body_valid(c.body)) { c.carried_still = 0; return c.body; }
    ph::BodySettings bs;
    bs.type = ph::BodyType::dynamic;
    bs.position = c.pos + Vec3{0, 0, 1.12f};
    bs.rotation = yaw_quat(c.yaw);
    bs.linear_damping = 0.05f;
    bs.angular_damping = 0.6f;
    c.body = world_->create_body(bs);
    ph::ShapeSettings ss;
    ss.kind = ph::ShapeKind::box;
    ss.size = {1.64f, 0.68f, 0.76f};
    ss.material.density = kCowMass / (1.64f * 0.68f * 0.76f);
    ss.material.friction = 0.7f;
    ss.material.restitution = 0.05f;
    ss.material.category_bits = kCatWorld;
    world_->create_shape(c.body, ss);
    c.rot = bs.rotation;
    c.carried = true;
    c.carried_still = 0;
    return c.body;
  }
  return {};
}

void Life::update_cows(f32 dt, f32 now, const LifeView& view, LifeOut& out) {
  if (remote_animals_) {
    // the host's herds: eased to his snapshots; one that is a body here (ours: in our hand, thrown,
    // killed here) follows its body until it stands or lies again
    for (Cow& c : cows_) {
      const bool body = c.body.valid() && world_->body_valid(c.body);
      if (body) {
        const ph::Pose p = world_->pose(c.body);
        c.pos = p.position;
        c.rot = p.rotation;
        if (c.carried && !c.dead) {
          c.carried_still = kern::length(world_->linear_velocity(c.body)) < 0.35f ? c.carried_still + dt : 0;
          if (c.carried_still > 1.0f) { world_->destroy_body(c.body); c.body = {}; c.carried = false; c.pos.z = ground(view, c.pos.x, c.pos.y); }
        }
        continue;
      }
      const Vec3 want = c.net_to + c.net_v * clampf(now - c.net_at, 0, 0.8f);
      const Vec3 d = want - c.pos;
      if (kern::length(d) > 8) c.pos = want;
      else c.pos = c.pos + d * std::fmin(1.0f, dt * 8);
      if (!c.carried && !c.net_dead) c.pos.z = ground(view, c.pos.x, c.pos.y);
      c.rot = c.net_rot;
      c.dead = c.net_dead;
      c.phase += dt * (c.walking ? 3.2f : 0.0f);
    }
    return;
  }
  // herds around the player (re-checked every 2 s when he moved 150 m)
  if (now - last_herd_check_ > 2) {
    last_herd_check_ = now;
    if (dist2d(herds_for_, Vec2{view.focus.x, view.focus.y}) > 150 || (!view.mates.empty() && now - herds_at_ > 6)) {
      herds_at_ = now;
      herds_for_ = {view.focus.x, view.focus.y};
      rebuild_herds(view, now);
    }
  }
  for (Cow& c : cows_) {
    if (c.dead) {
      if (c.body.valid() && world_->body_valid(c.body)) {
        const ph::Pose p = world_->pose(c.body);
        c.pos = p.position;  // the body centre (drawing offsets it)
        c.rot = p.rotation;
      }
      continue;
    }
    if (c.carried) {
      // held or thrown: the body's pose; once it lies still for a second (upright enough) it stands
      // up where it is and grazes back to its patch
      if (!(c.body.valid() && world_->body_valid(c.body))) { c.carried = false; c.body = {}; }
      else {
        const ph::Pose p = world_->pose(c.body);
        c.pos = p.position;
        c.rot = p.rotation;
        const f32 v = kern::length(world_->linear_velocity(c.body));
        c.carried_still = v < 0.35f ? c.carried_still + dt : 0;
        if (c.carried_still > 1.0f) {
          world_->destroy_body(c.body);
          c.body = {};
          c.carried = false;
          const Vec3 f = p.rotation.rotate({1, 0, 0});
          c.yaw = std::atan2(f.y, f.x);
          const f64 clock = view.epoch > 0 ? view.epoch : now;
          const i64 k = static_cast<i64>(std::floor(clock / kPeriod));
          const Vec2 A = graze_spot(c, k), B = graze_spot(c, k + 1), d = B - A;
          const f32 len = std::hypot(d.x, d.y), walk = std::fmin(kPeriod * 0.75f, len / kWalk);
          const f32 u = std::fmin(1.0f, static_cast<f32>(clock - static_cast<f64>(k) * kPeriod) / std::fmax(0.001f, walk));
          c.off = Vec2{p.position.x, p.position.y} - (A + d * u);
          c.pos = {p.position.x, p.position.y, ground(view, p.position.x, p.position.y)};
        }
        continue;
      }
    }
    // the clock-driven grazing walk on the wall clock: the same on every machine (cows.mjs pathPose)
    const f64 clock = view.epoch > 0 ? view.epoch : now;
    const i64 k = static_cast<i64>(std::floor(clock / kPeriod));
    const Vec2 A = graze_spot(c, k), B = graze_spot(c, k + 1), d = B - A;
    const f32 len = std::hypot(d.x, d.y), walk = std::fmin(kPeriod * 0.75f, len / kWalk);
    const f32 u = std::fmin(1.0f, static_cast<f32>(clock - static_cast<f64>(k) * kPeriod) / std::fmax(0.001f, walk));
    const Vec2 base = A + d * u;
    const bool running = now < c.flee_until;
    if (running) c.off = c.off + c.flee_v * dt;
    else c.off = c.off * std::exp(-dt / 14);
    Vec2 p = base + c.off;
    const Meadow* m = c.meadow >= 0 && c.meadow < static_cast<i32>(meadows_.size()) ? &meadows_[c.meadow] : nullptr;
    if (m && !in_rings(m->rings, p) && in_rings(m->rings, base)) { c.off = c.off * 0.9f; p = base + c.off; }
    // never into a building: stay put, a fleeing cow turns along the wall
    const RayResult wall = cast(*view.physics, {c.pos.x, c.pos.y, c.pos.z + 0.9f}, {std::cos(c.yaw), std::sin(c.yaw), 0}, 1.2f, {}, kCatWorld);
    if (wall.hit && wall.normal.z < 0.5f && running) { const f32 vx = c.flee_v.x; c.flee_v = {-c.flee_v.y, vx}; p = {c.pos.x, c.pos.y}; c.off = p - base; }
    const bool walking = running || (u < 1 && len > 0.2f);
    const f32 yaw = running ? std::atan2(c.flee_v.y, c.flee_v.x) : len > 0.2f ? std::atan2(d.y, d.x) : c.yaw;
    c.yaw += wrap_pi(yaw - c.yaw) * std::fmin(1.0f, dt * (running ? 6.0f : 2.2f));
    c.pos = {p.x, p.y, ground(view, p.x, p.y)};
    c.walking = walking;
    c.phase += dt * (running ? 7.0f : walking ? 3.2f : 0.0f);
    if (now > c.next_moo) {
      c.next_moo = now + 15 + hash01(mix(c.key, static_cast<u64>(now))) * 30;
      if (dist2d(c.pos, view.focus) < 35 && now - last_moo_ > 2.5f) { last_moo_ = now; out.sounds.push_back({LifeOut::Sound::moo, c.pos + Vec3{0, 0, 1.4f}, 0.7f, 0.95f + hash01(c.key) * 0.15f}); }
    }
    // cars (cows.mjs checkCars): a slow car nudges, a fast one kills; the car loses momentum
    for (const LifeCar& car : view.cars) {
      if (car.airborne) continue;
      const Vec3 dl = c.pos - car.position;
      if (dl.x * dl.x + dl.y * dl.y > 40) continue;
      const Vec3 local = to_local(car.rotation, dl);
      if (std::fabs(local.x) > car.half.x + 0.55f || std::fabs(local.y) > car.half.y + 0.45f) continue;
      const f32 sp = std::hypot(car.velocity.x, car.velocity.y), share = kCowMass / (car.mass + kCowMass);
      if (sp > 3) {
        const f32 lost = sp * share;
        if (car.car >= 0) out.impulses.push_back({car.car, Vec3{-car.velocity.x / sp, -car.velocity.y / sp, 0} * (car.mass * lost)});
        if (sp > 8) kill_cow(c, {car.velocity.x * kCowMass * 1.1f, car.velocity.y * kCowMass * 1.1f, kCowMass * std::fmin(6.0f, sp * 0.35f)}, now, out);
        else {
          c.off = c.off + Vec2{car.velocity.x / sp, car.velocity.y / sp} * 1.2f;
          scare_cow(c, {car.position.x, car.position.y}, now, 4.5f);
          out.sounds.push_back({LifeOut::Sound::moo_pain, c.pos + Vec3{0, 0, 1.4f}, 0.9f, 1.4f});
        }
      } else {
        const f32 l = std::hypot(dl.x, dl.y) + 1e-3f;
        c.off = c.off + Vec2{dl.x / l, dl.y / l} * 0.08f;
      }
      break;
    }
  }
}

// ---------------------------------------------------------------------------------------------
// pigeons (pigeons.mjs)

bool Life::blocked(const LifeView& view, Vec2 p) const {
  if (view.city->terrain().water_at(p.x, p.y)) return true;
  const f32 g = ground(view, p.x, p.y);
  const RayResult r = cast(*view.physics, {p.x, p.y, g + 40}, {0, 0, -1}, 45, {}, kCatWorld);
  return r.hit && r.point.z > g + 0.35f;
}

bool Life::spot_near(const LifeView& view, Vec2 c, f32 r0, f32 r1, Rng& rng, Vec2& out) const {
  for (int i = 0; i < 24; ++i) {
    const f32 a = rng.unit() * 2 * kPi, r = rng.range(r0, r1);
    const Vec2 p{c.x + std::cos(a) * r, c.y + std::sin(a) * r};
    if (!blocked(view, p)) { out = p; return true; }
  }
  return false;
}

bool Life::spawn_flock(Vec2 at, f32 now, Rng& rng, const LifeView& view) {
  Flock f;
  if (!spot_near(view, at, 0, 3, rng, f.at)) return false;
  const u32 n = kPerFlockMin + rng.next() % (kPerFlockMax - kPerFlockMin + 1);
  for (u32 i = 0; i < n; ++i) {
    Pigeon b;
    const f32 a = rng.unit() * 2 * kPi, r = std::sqrt(rng.unit()) * 2.4f;
    b.o = {std::cos(a) * r, std::sin(a) * r};
    b.yaw = rng.unit() * 2 * kPi;
    b.peck = rng.unit() * 10;
    b.flap = rng.unit() * 6;
    b.lag = rng.range(0, 0.45f);
    b.pos = {f.at.x + b.o.x, f.at.y + b.o.y, ground(view, f.at.x, f.at.y)};
    f.birds.push_back(b);
  }
  f.next_flight = now + rng.range(25, 70);
  f.coo_at = now + rng.range(2, 9);
  f.id = ++flock_serial_;
  flocks_.push_back(std::move(f));
  return true;
}

void Life::spook(Flock& f, Vec2 from, f32 now, Rng& rng, const LifeView& view, LifeOut& out) {
  if (f.air) return;
  const f32 away = std::atan2(f.at.y - from.y, f.at.x - from.x) + rng.range(-0.6f, 0.6f);
  const f32 far = rng.range(25, 55);
  Vec2 dest;
  if (!spot_near(view, f.at + Vec2{std::cos(away), std::sin(away)} * far, 0, 18, rng, dest) && !spot_near(view, f.at, 20, 50, rng, dest)) dest = f.at;
  f.air = true;
  f.t = 0;
  f.c = f.at + Vec2{std::cos(away), std::sin(away)} * 12;
  f.r = rng.range(9, 16);
  f.h = rng.range(9, 18);
  f.dir = rng.unit() < 0.5f ? 1.0f : -1.0f;
  f.a0 = std::atan2(f.at.y - f.c.y, f.at.x - f.c.x);
  f.dur = rng.range(7, 13);
  f.dest = dest;
  f.from = f.at;
  if (dist2d(view.focus, Vec3{f.at.x, f.at.y, view.focus.z}) < 45) out.sounds.push_back({LifeOut::Sound::flap, {f.at.x, f.at.y, ground(view, f.at.x, f.at.y) + 0.5f}, 0.8f, rng.range(0.9f, 1.15f)});
  (void)now;
}

i32 Life::catch_pigeon(Vec3 hand, f32 hand_speed, f32 now, Rng& rng) {
  for (Flock& f : flocks_) {
    if (f.air) continue;
    for (std::size_t i = 0; i < f.birds.size(); ++i) {
      const Pigeon& b = f.birds[i];
      if (b.dead_at >= 0 || kern::length(hand - (b.pos + Vec3{0, 0, 0.15f})) > 0.24f) continue;
      // a quick hand, or bad luck (one in two): the flock is up and away
      if (hand_speed > 1.0f || rng.unit() < 0.5f) { f.next_flight = now; return -1; }
      LooseBird lb;
      lb.id = ++loose_serial_;
      lb.pos = b.pos + Vec3{0, 0, 0.15f};
      lb.yaw = b.yaw;
      loose_.push_back(lb);
      f.birds.erase(f.birds.begin() + static_cast<std::ptrdiff_t>(i));
      f.next_flight = now + 0.4f;  // the others take fright a moment later
      return lb.id;
    }
  }
  return -1;
}

void Life::hold_pigeon(i32 id, Vec3 at, f32 yaw) {
  for (LooseBird& b : loose_)
    if (b.id == id) { b.pos = at; b.yaw = yaw; b.held = true; b.flap += 0.9f; }  // it struggles
}

void Life::release_pigeon(i32 id, Vec3 velocity) {
  for (LooseBird& b : loose_)
    if (b.id == id) { b.held = false; b.t = 0; b.vel = velocity + Vec3{0, 0, 2.5f}; }
}

void Life::spook_pigeons(f32 now) {
  for (Flock& f : flocks_) { f.next_flight = now; }
}

void Life::update_pigeons(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out) {
  if (remote_animals_) {
    // the host's flocks: every bird eased to where he has it (walking, pecking, flying)
    if (!view.birds_on) { flocks_.clear(); }
    for (Flock& f : flocks_)
      for (Pigeon& b : f.birds) {
        if (b.dead_at >= 0) {
          const f32 gz = ground(view, b.pos.x, b.pos.y);
          if (b.pos.z > gz + 0.01f || b.fall_v.z > 0) { b.fall_v.z -= 9.81f * dt; b.pos = b.pos + b.fall_v * dt; if (b.pos.z <= gz) { b.pos.z = gz; b.fall_v = {}; } }
          continue;
        }
        const Vec3 want = b.net_to + b.net_v * clampf(now - b.net_at, 0, 0.6f);
        const Vec3 d = want - b.pos;
        const bool moving = kern::length(b.net_v) > 0.05f;
        if (kern::length(d) > 15) b.pos = want;
        else b.pos = b.pos + d * std::fmin(1.0f, dt * 10);
        if (!f.air) b.pos.z = ground(view, b.pos.x, b.pos.y);
        b.yaw += wrap_pi(b.net_yaw - b.yaw) * std::fmin(1.0f, dt * 8);
        b.flap += dt * (f.air ? 16.0f : 0.0f);
        b.peck += dt * (f.air ? 0.0f : moving ? 9.0f : 3.0f);
      }
  }
  if (remote_animals_) {
    for (LooseBird& b : loose_) {
      if (b.held) continue;
      b.t += dt;
      b.flap += dt * 22;
      b.vel.z += (4.0f - b.vel.z) * std::fmin(1.0f, dt * 1.5f);
      b.pos = b.pos + b.vel * dt;
      const f32 hs = std::hypot(b.vel.x, b.vel.y);
      if (hs > 0.1f) b.yaw = std::atan2(b.vel.y, b.vel.x);
    }
    loose_.erase(std::remove_if(loose_.begin(), loose_.end(), [](const LooseBird& b) { return !b.held && b.t > 6; }), loose_.end());
    (void)rng;
    (void)out;
    return;
  }
  const Vec2 me{view.focus.x, view.focus.y};
  const u32 want = mobile_ ? 3 : 5;  // FLOCKS
  auto far_from_all = [&](Vec2 at) {
    if (dist2d(at, me) <= kDespawn) return false;
    for (const Vec3& m : view.mates) if (dist2d(at, Vec2{m.x, m.y}) <= kDespawn) return false;
    return true;
  };
  flocks_.erase(std::remove_if(flocks_.begin(), flocks_.end(), [&](const Flock& f) { return !f.air && far_from_all(f.at); }), flocks_.end());
  if (!view.birds_on) { flocks_.clear(); return; }
  // (host: as many again around every mate, spawned round them in turn)
  if (flocks_.size() < want * (1 + view.mates.size()) && rng.unit() < 1 - std::exp(-3 * dt * static_cast<f32>(1 + view.mates.size()))) {
    const u32 pick = view.mates.empty() ? 0 : rng.next() % static_cast<u32>(1 + view.mates.size());
    const Vec2 around = pick == 0 ? me : Vec2{view.mates[pick - 1].x, view.mates[pick - 1].y};
    u32 there = 0;
    for (const Flock& f : flocks_) there += dist2d(f.at, around) <= kDespawn;
    Vec2 at;
    if (there < want && spot_near(view, around, kNear, kFar, rng, at)) spawn_flock(at, now, rng, view);
  }
  // a pigeon let go flies off: up and away (it beats its wings), gone after 6 s
  for (LooseBird& b : loose_) {
    if (b.held) continue;
    b.t += dt;
    b.flap += dt * 22;
    b.vel.z += (4.0f - b.vel.z) * std::fmin(1.0f, dt * 1.5f);
    const Vec2 h{b.vel.x, b.vel.y};
    const f32 hs = std::hypot(h.x, h.y);
    if (hs < 6) { const Vec2 d = hs > 0.1f ? h * (1 / hs) : Vec2{std::cos(b.yaw), std::sin(b.yaw)}; b.vel.x += d.x * dt * 6; b.vel.y += d.y * dt * 6; }
    b.pos = b.pos + b.vel * dt;
    if (hs > 0.1f) b.yaw = std::atan2(b.vel.y, b.vel.x);
  }
  loose_.erase(std::remove_if(loose_.begin(), loose_.end(), [](const LooseBird& b) { return !b.held && b.t > 6; }), loose_.end());
  for (Flock& f : flocks_) {
    f.birds.erase(std::remove_if(f.birds.begin(), f.birds.end(), [&](const Pigeon& b) { return b.dead_at >= 0 && now - b.dead_at > 12; }), f.birds.end());
    if (!f.air) {
      const f32 dp = dist2d(me, f.at);
      if (dp < kSpookPlayer + 2.4f && view.player_speed > 0.3f) spook(f, me, now, rng, view, out);
      else if (const Vec3* mate = [&]() -> const Vec3* { for (const Vec3& m : view.mates) if (dist2d(Vec2{m.x, m.y}, f.at) < kSpookPlayer + 1.4f) return &m; return nullptr; }())
        spook(f, {mate->x, mate->y}, now, rng, view, out);  // a mate walking up to them (host: his flocks are everybody's)
      else
        for (const LifeCar& c : view.cars)
          if (std::hypot(c.position.x - f.at.x, c.position.y - f.at.y) < kSpookCar && kern::length(c.velocity) > 2) { spook(f, {c.position.x, c.position.y}, now, rng, view, out); break; }
      if (!f.air && now > f.next_flight) spook(f, f.at + Vec2{rng.range(-1, 1), rng.range(-1, 1)}, now, rng, view, out);
      if (!f.air && now > f.coo_at) {
        f.coo_at = now + rng.range(3.5f, 11);
        if (dp < 30 && now - last_coo_ > 1.6f) { last_coo_ = now; out.sounds.push_back({LifeOut::Sound::coo, {f.at.x, f.at.y, ground(view, f.at.x, f.at.y) + 0.3f}, 0.6f * std::fmax(0.15f, 1 - dp / 30), rng.range(0.92f, 1.08f)}); }
      }
    } else {
      f.t += dt;
      if (f.t >= f.dur) {
        f.air = false;
        f.at = f.dest;
        f.next_flight = now + rng.range(30, 80);
        for (Pigeon& b : f.birds) b.o = {rng.range(-2.2f, 2.2f), rng.range(-2.2f, 2.2f)};
      }
    }
    for (Pigeon& b : f.birds) {
      if (b.dead_at >= 0) {
        const f32 gz = ground(view, b.pos.x, b.pos.y);
        if (b.pos.z > gz + 0.01f || b.fall_v.z > 0) {
          b.fall_v.z -= 9.81f * dt;
          b.pos = b.pos + b.fall_v * dt;
          if (b.pos.z <= gz) { b.pos.z = gz; b.fall_v = {}; }
        }
        continue;
      }
      if (!f.air) {
        // short walks, nodding, pecking; never into a wall
        if (now > b.walk_until) {
          if (rng.unit() < 0.6f) {
            b.yaw += rng.range(-1.4f, 1.4f);
            const f32 sp = rng.range(0.25f, 0.55f);
            b.v = {std::cos(b.yaw) * sp, std::sin(b.yaw) * sp};
            b.walk_until = now + rng.range(0.5f, 1.8f);
          } else {
            b.v = {};
            b.walk_until = now + rng.range(0.7f, 2.4f);
          }
        }
        Vec2 n = b.o + b.v * dt;
        if (std::hypot(n.x, n.y) > 3.6f) { b.yaw += kPi * 0.8f; b.v = b.v * -1.0f; n = b.o; }
        b.o = n;
        const Vec2 t = f.at + b.o;
        const f32 k = std::fmin(1.0f, dt * 6);
        b.pos.x += (t.x - b.pos.x) * k;
        b.pos.y += (t.y - b.pos.y) * k;
        b.pos.z = ground(view, b.pos.x, b.pos.y);
        const bool moving = std::hypot(b.v.x, b.v.y) > 0.05f;
        b.peck += dt * (moving ? 9.0f : 3.0f);
      } else {
        // up and away, a couple of loops, then a glide down onto the new spot
        const f32 t = std::fmax(0.0f, f.t - b.lag), k = std::fmin(1.0f, t / f.dur);
        const f32 ang = f.a0 + f.dir * t * 1.15f + b.o.x * 0.06f;
        const Vec2 c = f.c + b.o * 0.8f, l = c + Vec2{std::cos(ang), std::sin(ang)} * f.r;
        const f32 land = std::fmax(0.0f, (k - 0.72f) / 0.28f), lift = std::fmin(1.0f, t / 1.2f);
        const Vec2 dst = f.dest + b.o, src = f.from + b.o;
        Vec2 p = l;
        if (t < 1.2f) p = src + (l - src) * lift;
        if (land > 0) p = l + (dst - l) * land;
        const f32 z = ground(view, p.x, p.y) + f.h * std::fmin(lift, 1 - land) + 0.05f;
        const Vec2 dv{p.x - b.pos.x, p.y - b.pos.y};
        if (std::hypot(dv.x, dv.y) > 0.01f) b.yaw = std::atan2(dv.y, dv.x);
        b.pos = {p.x, p.y, z};
        b.flap += dt * (land > 0 && land < 1 ? 9.0f : t < 1.5f ? 24.0f : 14.0f);
      }
    }
  }
}

// ---------------------------------------------------------------------------------------------
// birds over the roofs and trees along the streets (world_liveliness.mjs)

void Life::update_birds(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out) {
  if (!view.birds_on) {
    for (Bird& b : birds_) b.dead_until = std::fmax(b.dead_until, now + 0.5f);  // switched off: not drawn, not hit
    return;
  }
  for (std::size_t i = 0; i < birds_.size(); ++i) {
    Bird& b = birds_[i];
    // their circling centre drifts after the player (SpawnCity circles round the map origin)
    const Vec2 target{view.focus.x + static_cast<f32>((b.seed >> 7) % 25) - 12, view.focus.y + static_cast<f32>((b.seed >> 13) % 25) - 12};
    if (b.center.x == 0 && b.center.y == 0) b.center = target;
    const f32 k = damp_alpha(0.03f, dt);
    b.center = b.center + (target - b.center) * k;
    if (dist2d(b.center, target) > 400) b.center = target;
    if (now < b.dying_until) {
      // it tumbles down out of the sky and lies where it hit the ground (a thud, a last flutter)
      if (b.down) continue;
      b.fall_v.z -= 5.4f * dt;
      b.pos = b.pos + b.fall_v * dt;
      b.yaw += b.spin * dt;
      const f32 gz = ground(view, b.pos.x, b.pos.y);
      if (b.pos.z <= gz + 0.05f) {
        b.pos.z = gz + 0.05f;
        b.down = true;
        b.dying_until = b.dead_until;
        out.sounds.push_back({LifeOut::Sound::thud, b.pos, 0.55f, rng.range(1.3f, 1.6f)});
        out.sounds.push_back({LifeOut::Sound::flap, b.pos, 0.35f, rng.range(1.1f, 1.4f)});
      } else if (now + dt >= b.dying_until) b.dying_until = now + 0.2f;  // still falling
      continue;
    }
    if (now < b.dead_until) continue;
    const f32 phase = static_cast<f32>(b.seed % 10000) / 10000;
    const f32 a = now * (0.22f + static_cast<f32>(b.seed % 22) / 1000) + phase * 2 * kPi, r = 18 + static_cast<f32>(b.seed % 34);
    const f32 z = 8 + static_cast<f32>(b.seed % 13) + std::sin(a * 2.3f + static_cast<f32>(i)) * 2.1f;
    b.pos = {b.center.x + std::cos(a) * r, b.center.y + std::sin(a * 0.94f) * r, ground(view, b.center.x, b.center.y) + z};
    b.yaw = std::atan2(0.94f * std::cos(a * 0.94f), -std::sin(a));
    b.flap = 0.82f + 0.2f * std::sin(now * 10 + static_cast<f32>(i));
  }
}

void Life::update_trees(const LifeView& view, f32 now) {
  auto key_of = [](u64 tile, u32 index) { return tile * 0x9e3779b97f4a7c15ull + index; };
  // blown-over trees lie 40 s, then they are gone (and stay gone while their tile is loaded)
  for (const Tree& t : trees_) if (t.fell_at >= 0 && now - t.fell_at > 40) trees_gone_.insert(key_of(t.tile, t.index));
  if (!trees_.empty()) trees_.erase(std::remove_if(trees_.begin(), trees_.end(), [&](const Tree& t) { return t.fell_at >= 0 && now - t.fell_at > 40; }), trees_.end());
  // The trees are the map tiles' (tiles.h furnish: beside the streets, clear of every street,
  // path and house - from the tile's data alone, so every machine has the same trees): taken
  // whenever the set of tiles changes; a tile that goes takes its trees along.
  if (tree_serial_ != view.city->furniture_serial()) {
    tree_serial_ = view.city->furniture_serial();
    std::vector<CityStream::Furniture> tiles;
    view.city->furniture(tiles);
    std::unordered_map<u64, Tree> fallen;
    for (const Tree& t : trees_) if (t.fell_at >= 0) fallen[key_of(t.tile, t.index)] = t;
    std::unordered_set<u64> gone;
    trees_.clear();
    for (const CityStream::Furniture& f : tiles)
      for (u32 i = 0; i < f.trees->size(); ++i) {
        const MapFeatures::Spot& sp = (*f.trees)[i];
        const u64 k = key_of(f.tile, i);
        if (trees_gone_.count(k)) { gone.insert(k); continue; }
        if (view.city->terrain().water_at(sp.p.x, sp.p.y)) continue;
        Tree t{{sp.p.x, sp.p.y, ground(view, sp.p.x, sp.p.y)}, sp.a, 0, -1, {1, 0}, f.tile, i};
        if (const auto it = fallen.find(k); it != fallen.end()) { t.fell_at = it->second.fell_at; t.fall_dir = it->second.fall_dir; }
        trees_.push_back(t);
      }
    trees_gone_ = std::move(gone);
  }
  // the ground changed somewhere (a crater, a new elevation window): the trees there stand on it
  if (view.ground_changes)
    for (const kern::Vec4& r : *view.ground_changes)
      for (Tree& t : trees_)
        if (t.foot.x >= r.x && t.foot.x <= r.z && t.foot.y >= r.y && t.foot.y <= r.w) t.foot.z = ground(view, t.foot.x, t.foot.y);
}


// ---------------------------------------------------------------------------------------------
// dogs and cats (world_procedural_population.mjs): 20 (mobile 12) wander on circling paths round
// the neighbourhood's anchor; every third is a cat, the rest dogs; every other two minutes #1 is
// the BLACK CAT - shoot it and it charges at 11 m/s, leaps at 1.6 m and that is the end of you,
// then it saunters off. A shot, a blast or a car kills the others (the body rolls over as a
// Box3D box, gone after 25 s); fists take three blows for a dog, two for a cat.

namespace {
const u32 kDogColors[5] = {0x8a6a4a, 0xc49a5a, 0x3b2c22, 0xe8dcc8, 0x6e5a46}, kCatColors[4] = {0x8d8d8d, 0xd88a3a, 0x5b5048, 0xe9e2d6};
constexpr f32 kPetTorso = 0.11f;
}  // namespace

Life::Species Life::species(u32 i, f64 epoch) const {
  if (i == 1 && (static_cast<i64>(std::floor(epoch / 120)) % 2 == 0 || cat_.on)) return Species::black_cat;
  return (pet_base_ ? i % pet_base_ : i) % 3 == 2 ? Species::cat : Species::dog;  // every neighbourhood the same mix
}

bool Life::provoke_cat(f32 now) {
  if (pets_.size() < 2 || !pets_[1].shown || cat_.on) return false;
  cat_ = {};
  cat_.on = true;
  cat_.p = pets_[1].pos;
  cat_.yaw = pets_[1].yaw;
  cat_.start = now;
  return true;
}

void Life::kill_pet(u32 i, Vec3 dir, f32 now, LifeOut& out) {
  Pet& p = pets_[i];
  if (p.dead_at >= 0) return;
  const Species sp = species(i, epoch_);
  p.dead_at = now;
  out.sounds.push_back({sp == Species::dog ? LifeOut::Sound::dog : LifeOut::Sound::cat, p.pos, 0.8f, 1});
  // the legs give way and the body rolls over onto its side, away from the shot (collapseAnimal)
  const f32 sc = sp == Species::dog ? p.scale : p.scale * 0.62f;
  const Vec3 half = Vec3{0.33f, 0.13f, 0.27f} * sc;
  const f32 mass = (sp == Species::dog ? 22.0f : 4.0f) * sc * sc * sc;
  ph::BodySettings bs;
  bs.type = ph::BodyType::dynamic;
  bs.position = {p.pos.x, p.pos.y, p.pos.z + half.z};
  bs.rotation = yaw_quat(p.yaw);
  bs.linear_damping = 0.9f;
  bs.angular_damping = 2.5f;
  p.body = world_->create_body(bs);
  ph::ShapeSettings ss;
  ss.kind = ph::ShapeKind::box;
  ss.size = half * 2.0f;
  ss.material.density = mass / (8 * half.x * half.y * half.z);
  ss.material.friction = 0.7f;
  ss.material.category_bits = kCatWorld;
  world_->create_shape(p.body, ss);
  const Vec3 f{std::cos(p.yaw), std::sin(p.yaw), 0};
  const f32 side = dir.x * -f.y + dir.y * f.x >= 0 ? 1.0f : -1.0f;
  const f32 w = 2 * half.y, h = 2 * half.z, I = mass * (w * w + h * h) / 3;
  world_->apply_angular_impulse(p.body, f * (I * 5 * side));
  world_->apply_linear_impulse_to_center(p.body, Vec3{dir.x, dir.y, std::fmax(0.0f, dir.z)} * mass);
}

void Life::update_pets(f32 dt, f32 now, const LifeView& view, LifeOut& out) {
  epoch_ = view.epoch;
  // the anchor: the neighbourhood (re-anchored when the player is 2.5 km away, REANCHOR_M)
  const Vec2 me{view.focus.x, view.focus.y};
  if (!remote_animals_) {
    // (a client takes the host's neighbourhoods: apply_animals)
    if (dist2d(pet_anchor_, me) > 2500) {
      pet_anchor_ = {std::round(me.x / 100) * 100, std::round(me.y / 100) * 100};
      for (u32 i = 0; i < pet_base_ && i < pets_.size(); ++i) { pets_[i].shown = false; pets_[i].has_pose = false; }
    }
    // host: a neighbourhood of dogs and cats round every mate far from the others too (as he would
    // have alone) - the same rounds, its own group of the list; one nobody is near any more goes
    const u32 base = pet_base_ ? pet_base_ : 20;
    std::vector<Vec2> want{pet_anchor_};
    for (std::size_t k = 1; k < pet_anchors_.size(); ++k) {
      const Vec2 a = pet_anchors_[k];
      bool near = false;
      for (const Vec3& m : view.mates) near = near || dist2d(a, Vec2{m.x, m.y}) <= 2500;
      want.push_back(near ? a : Vec2{1e9f, 1e9f});
    }
    for (const Vec3& m : view.mates) {
      const Vec2 mp{m.x, m.y};
      bool covered = false;
      for (const Vec2& a : want) covered = covered || dist2d(a, mp) <= 2500;
      if (covered) continue;
      const Vec2 a{std::round(mp.x / 100) * 100, std::round(mp.y / 100) * 100};
      bool placed = false;
      for (std::size_t k = 1; k < want.size() && !placed; ++k) if (want[k].x > 1e8f) { want[k] = a; placed = true; }
      if (!placed && want.size() < 8) want.push_back(a);
    }
    while (want.size() > 1 && want.back().x > 1e8f) want.pop_back();
    if (pets_.size() != base * want.size()) {
      const std::size_t had = pets_.size();
      for (std::size_t i = base * want.size(); i < had; ++i)
        if (pets_[i].body.valid() && world_->body_valid(pets_[i].body)) world_->destroy_body(pets_[i].body);
      pets_.resize(base * want.size());
      for (std::size_t i = had; i < pets_.size(); ++i) pet_params(pets_[i], static_cast<u32>(i % base));
    }
    for (std::size_t k = 1; k < want.size() && k < pet_anchors_.size(); ++k)
      if (dist2d(want[k], pet_anchors_[k]) > 1)
        for (u32 i = 0; i < base; ++i) { Pet& p = pets_[k * base + i]; p.shown = false; p.has_pose = false; p.off = {}; }
    pet_anchors_ = want;
  }
  const f32 t = static_cast<f32>(std::fmod(view.epoch, 86400.0 * 4));
  // the black cat's charge (updateCatAttack)
  if (cat_.on) {
    if (!view.cats_on) cat_.on = false;
    const Vec3 head = view.head;
    const f32 dx = head.x - cat_.p.x, dy = head.y - cat_.p.y, d = std::hypot(dx, dy);
    if (cat_.phase == 2) {
      cat_.p = cat_.p + Vec3{std::cos(cat_.away_yaw), std::sin(cat_.away_yaw), 0} * (1.3f * dt);
      cat_.yaw = cat_.away_yaw;
      cat_.p.z = ground(view, cat_.p.x, cat_.p.y);
      if (d > 28 || now - cat_.leave_at > 20) cat_.on = false;
    } else if (!view.on_foot) {
      cat_.on = false;
    } else {
      cat_.yaw = std::atan2(dy, dx);
      if (cat_.phase == 0) {
        if (now - cat_.last_cry > 0.78f) { cat_.last_cry = now; ++cat_.cries; out.sounds.push_back({cat_.cries % 2 ? LifeOut::Sound::yowl : LifeOut::Sound::hiss, cat_.p, 1, 1}); }
        const f32 step = std::fmin(d, 11 * dt);
        cat_.p.x += dx / std::fmax(1e-3f, d) * step;
        cat_.p.y += dy / std::fmax(1e-3f, d) * step;
        cat_.p.z = ground(view, cat_.p.x, cat_.p.y) + std::fabs(std::sin(now / 0.055f)) * 0.12f;
        if (d < 1.6f) { cat_.phase = 1; cat_.leap_at = now; cat_.from = cat_.p; out.sounds.push_back({LifeOut::Sound::screech, cat_.p, 1, 1}); }
        if (now - cat_.start > 25) cat_.on = false;
      } else {
        const f32 k = std::fmin(1.0f, (now - cat_.leap_at) / 0.34f);
        cat_.p.x = cat_.from.x + (head.x - cat_.from.x) * k;
        cat_.p.y = cat_.from.y + (head.y - cat_.from.y) * k;
        cat_.p.z = std::fmax(ground(view, cat_.p.x, cat_.p.y) + 0.18f, cat_.from.z + (head.z - 0.05f - cat_.from.z) * k + std::sin(k * kPi) * 0.55f);
        if (k >= 1) {
          out.kill_player = true;
          cat_.phase = 2;
          cat_.leave_at = now;
          cat_.away_yaw = cat_.yaw + kPi + (hash01(static_cast<u64>(now * 1000)) - 0.5f) * 0.8f;
        }
      }
    }
  }
  for (u32 i = 0; i < pets_.size(); ++i) {
    Pet& p = pets_[i];
    const Species sp = species(i, view.epoch);
    const bool allowed = sp == Species::dog ? view.dogs_on : view.cats_on;
    const u32 seed = static_cast<u32>(hash01(mix(0xc010, i)) * 1000);
    p.color = sp == Species::black_cat ? rgb(0x0d0d0f) : sp == Species::cat ? rgb(kCatColors[seed % 4]) : rgb(kDogColors[seed % 5]);
    auto drop = [&]() {
      if (p.body.valid() && world_->body_valid(p.body)) world_->destroy_body(p.body);
      p.body = {};
    };
    if (!allowed) { drop(); p.shown = false; p.dead_at = -1; continue; }
    if (remote_animals_ && !(p.body.valid() && world_->body_valid(p.body))) {
      // the host's dog / cat: eased to his snapshots (a body here is ours: below)
      if (i < pet_net_to_.size()) {
        Vec3 to = pet_net_to_[i];
        // walking its round: the round is the wall clock's (the host's anchor, the same epoch) - so
        // here exactly where it is there, not where his last snapshot had it
        const bool round = p.shown && p.dead_at < 0 && !p.carried && !(cat_.on && i == 1);
        if (round) {
          p.off = p.off * std::exp(-dt / 5);
          const f32 a = p.phase + p.omega * t;
          const Vec2 q = anchor_of(i) + Vec2{std::cos(a), std::sin(a)} * p.radius + Vec2{std::cos(p.wobble_omega * t), std::sin(p.wobble_omega * t * 1.3f)} * p.wobble_r + p.off;
          to = {q.x, q.y, ground(view, q.x, q.y)};
          if (p.has_pose && dist2d(Vec2{q.x, q.y}, Vec2{p.pos.x, p.pos.y}) > 1e-4f) pet_net_yaw_[i] = std::atan2(q.y - p.pos.y, q.x - p.pos.x);
        }
        const Vec3 d = to - p.pos;
        const Vec2 last{p.pos.x, p.pos.y};
        if (!p.has_pose || kern::length(d) > 6 || round) p.pos = to;
        else p.pos = p.pos + d * std::fmin(1.0f, dt * 7);
        if (!p.carried && p.dead_at < 0) p.pos.z = ground(view, p.pos.x, p.pos.y);
        p.yaw += wrap_pi(pet_net_yaw_[i] - p.yaw) * std::fmin(1.0f, dt * 8);
        if (p.has_pose && dt > 0) p.speed = std::fmin(6.0f, dist2d(Vec2{p.pos.x, p.pos.y}, last) / dt);
        p.gait += p.speed * dt / (0.55f * p.scale);
        p.rot = p.net_rot;
        p.has_pose = true;
      }
      continue;
    }
    if (p.dead_at >= 0) {
      // a dead animal is gone after a while (25 s); another wanders in later
      if (now - p.dead_at > 25) { drop(); p.dead_at = -1; p.shown = false; p.has_pose = false; continue; }
      if (p.body.valid() && world_->body_valid(p.body)) { const ph::Pose q = world_->pose(p.body); p.pos = q.position; p.rot = q.rotation; }
      continue;
    }
    if (p.carried) {
      // held or thrown alive: the body's pose; lying still for 0.8 s it gets up and trots back
      if (!(p.body.valid() && world_->body_valid(p.body))) { p.carried = false; p.body = {}; }
      else {
        const ph::Pose q = world_->pose(p.body);
        p.pos = q.position;
        p.rot = q.rotation;
        p.carried_still = kern::length(world_->linear_velocity(p.body)) < 0.3f ? p.carried_still + dt : 0;
        if (p.carried_still > 0.8f) {
          drop();
          p.carried = false;
          const f32 a0 = p.phase + p.omega * t;
          const Vec2 q0 = anchor_of(i) + Vec2{std::cos(a0), std::sin(a0)} * p.radius + Vec2{std::cos(p.wobble_omega * t), std::sin(p.wobble_omega * t * 1.3f)} * p.wobble_r;
          p.off = Vec2{q.position.x, q.position.y} - q0;
          p.has_pose = false;
        }
        continue;
      }
    }
    p.off = p.off * std::exp(-dt / 5);
    if (cat_.on && i == 1) {
      const f32 cs = cat_.phase == 2 ? 1.3f : 11;
      p.pos = cat_.p;
      p.yaw = cat_.yaw;
      p.speed = cs;
      p.gait += cs * dt / 0.4f;
      p.shown = true;
      continue;
    }
    const f32 a = p.phase + p.omega * t;
    const Vec2 q = anchor_of(i) + Vec2{std::cos(a), std::sin(a)} * p.radius + Vec2{std::cos(p.wobble_omega * t), std::sin(p.wobble_omega * t * 1.3f)} * p.wobble_r + p.off;
    const Vec2 last{p.pos.x, p.pos.y};
    if (p.has_pose && dist2d(q, last) > 1e-4f) p.yaw = std::atan2(q.y - last.y, q.x - last.x);
    p.speed = p.has_pose && dt > 0 ? dist2d(q, last) / dt : 0;
    if (p.speed > 6) p.speed = 0;  // re-anchored
    p.pos = {q.x, q.y, ground(view, q.x, q.y)};
    p.gait += p.speed * dt / (0.55f * p.scale);
    p.has_pose = true;
    // in a building or the water: not there (the path goes on, it reappears outside)
    p.shown = !blocked(view, q);
    if (!p.shown) continue;
    // cars (a blow that changes its velocity this hard kills it)
    for (const LifeCar& c : view.cars) {
      if (c.airborne) continue;
      const f32 sp2 = std::hypot(c.velocity.x, c.velocity.y);
      if (sp2 < 3) continue;
      const Vec3 l = to_local(c.rotation, p.pos - c.position);
      if (std::fabs(l.x) < c.half.x + 0.3f && std::fabs(l.y) < c.half.y + 0.25f && std::fabs(l.z) < 2) {
        if (sp == Species::black_cat) continue;  // it jumps aside
        kill_pet(i, Vec3{c.velocity.x, c.velocity.y, 0} * 0.9f + Vec3{0, 0, 2.5f}, now, out);
        break;
      }
    }
  }
  (void)kPetTorso;
}

// ---------------------------------------------------------------------------------------------
// the running club (jogger_chain.mjs)

Vec3 Life::sample(f32 s, f32& yaw, Vec2& normal) const {
  const Chain& c = chain_;
  const f32 L = c.len;
  f32 u = std::fmod(std::fmod(s, 2 * L) + 2 * L, 2 * L);
  f32 dir = 1;
  if (u > L) { u = 2 * L - u; dir = -1; }  // run to the end, turn round, run back
  std::size_t i = 1;
  while (i < c.cum.size() - 1 && c.cum[i] < u) ++i;
  const Vec2 a = c.path[i - 1], b = c.path[i];
  const f32 seg = std::fmax(1e-3f, c.cum[i] - c.cum[i - 1]), t = clampf((u - c.cum[i - 1]) / seg, 0, 1);
  yaw = std::atan2((b.y - a.y) * dir, (b.x - a.x) * dir);
  const f32 sl = std::fmax(1e-3f, std::hypot(b.x - a.x, b.y - a.y));
  normal = Vec2{-(b.y - a.y) / sl, (b.x - a.x) / sl} * static_cast<f32>(c.side);
  return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, 0};
}

bool Life::spawn_joggers(const LifeView& view, f32 now, Rng& rng) {
  const std::vector<Lane>& lanes = view.city->lanes();
  const Lane* best = nullptr;
  f32 best_score = -1e9f;
  for (const Lane& l : lanes) {
    if (!l.alive || l.points.size() < 2 || l.width > 14) continue;
    f32 len = 0, near = 1e9f;
    for (std::size_t i = 1; i < l.points.size(); ++i) len += dist2d(l.points[i], l.points[i - 1]);
    if (len < 60) continue;
    for (Vec2 q : l.points) near = std::fmin(near, std::hypot(q.x - view.player.x, q.y - view.player.y));
    if (near < 35 || near > 150) continue;
    const f32 score = std::fmin(len, 260.0f) - near * 0.6f + rng.unit() * 40;
    if (score > best_score) { best_score = score; best = &l; }
  }
  if (!best) return false;
  const i32 side0 = rng.unit() < 0.5f ? 1 : -1;
  for (i32 side : {side0, -side0})
    for (f32 kerb : {1.7f, 1.2f, 2.4f}) {
      Chain c;
      c.side = side;
      c.off = best->width * 0.5f + kerb;
      const std::vector<Vec2>& pts = best->points;
      for (std::size_t i = 0; i < pts.size(); ++i) {
        const Vec2 a = pts[i == 0 ? 0 : i - 1], b = pts[std::min(pts.size() - 1, i + 1)];
        Vec2 d = b - a;
        const f32 l = std::fmax(1e-3f, std::hypot(d.x, d.y));
        d = d * (1 / l);
        c.path.push_back({pts[i].x - d.y * c.off * static_cast<f32>(side), pts[i].y + d.x * c.off * static_cast<f32>(side)});
      }
      c.cum.push_back(0);
      for (std::size_t i = 1; i < c.path.size(); ++i) c.cum.push_back(c.cum.back() + dist2d(c.path[i], c.path[i - 1]));
      c.len = c.cum.back();
      if (c.len < 50) continue;
      chain_ = c;
      bool free = true;
      f32 yaw;
      Vec2 n;
      for (f32 s = 0; s <= c.len && free; s += 1.5f) {
        const Vec3 q = sample(s, yaw, n);
        free = !blocked(view, {q.x, q.y});
      }
      if (!free) continue;
      // start at the end further from the player (they come towards you)
      const Vec2 a = c.path.front(), b = c.path.back(), me{view.player.x, view.player.y};
      chain_.s = dist2d(a, me) > dist2d(b, me) ? kRunners * kGap : c.len + kRunners * kGap;
      chain_.id = ++chain_serial_;
      chain_.born = now;
      chain_.next_call = now + 1.5f;
      chain_.next_beat = now;
      chain_.call = static_cast<i32>(rng.next() % 6);
      for (u32 i = 0; i < kRunners; ++i) {
        Runner& r = chain_.runners[i];
        r = Runner{};
        const Vec3 q = sample(chain_.s - static_cast<f32>(i) * kGap, yaw, n);
        r.pos = {q.x, q.y, ground(view, q.x, q.y)};
        r.yaw = r.yaw_s = yaw;
        r.phase = rng.unit() * 6;
      }
      chain_alive_ = true;
      return true;
    }
  return false;
}

void Life::kill_runner(i32 i, Vec3 velocity, f32 now, LifeOut& out) {
  Runner& r = chain_.runners[i];
  if (!r.alive) return;
  r.alive = false;
  Ragdolls::Colors col;
  col.shirt = rgb(0xd8ff3a);
  col.pants = rgb(0x1c2a3a);
  const u32 skins[5] = {0xe0b08c, 0xc48e6c, 0xa87052, 0x7b4a33, 0xd2a27e};
  col.skin = rgb(skins[i % 5]);
  ragdolls_.spawn((0x10000000ull + chain_.id) * 16 + static_cast<u64>(i), r.pos, r.yaw_s, velocity, col, false, now);
  out.bursts.push_back({r.pos + Vec3{0, 0, 1.1f}, {0.6f, 0.02f, 0.02f}});
  if (streak_chain_ != chain_.id || now - streak_at_ > kStreakWindow) { streak_n_ = 0; streak_chain_ = chain_.id; }
  ++streak_n_;
  streak_at_ = now;
  u32 left = 0;
  for (const Runner& x : chain_.runners) left += x.alive ? 1 : 0;
  char b[64];
  if (!left) {
    std::snprintf(b, sizeof b, "KOMPLETTE LAUFGRUPPE! x%u", streak_n_);
    out.banner = b;
    out.banner_for = 2.6f;
    out.sounds.push_back({LifeOut::Sound::jingle, r.pos, 1, 1});
  } else if (streak_n_ >= 2) {
    std::snprintf(b, sizeof b, "JOGGER x%u", streak_n_);
    out.banner = b;
    out.banner_for = 1.3f;
  }
}

void Life::apply_joggers(u32 id, const Vec3* pos, const f32* yaw, const bool* alive, u32 n, f32 now, f32 at) {
  Chain& c = chain_;
  if (!chain_alive_ || c.id != id) {
    c = Chain{};
    c.id = id;
    c.born = now;
    for (u32 i = 0; i < kRunners; ++i) { c.runners[i].alive = false; }
    for (u32 i = 0; i < n && i < kRunners; ++i) { c.runners[i].pos = c.runners[i].net_to = pos[i]; c.runners[i].yaw = c.runners[i].yaw_s = yaw[i]; c.runners[i].alive = alive[i]; }
    chain_alive_ = true;
  }
  for (u32 i = 0; i < n && i < kRunners; ++i) {
    Runner& r = c.runners[i];
    if (!alive[i]) {
      if (r.alive) {
        // down on the host's side: down here too (the doll falls along his heading)
        Ragdolls::Colors col;
        col.shirt = rgb(0xd8ff3a);
        col.pants = rgb(0x1c2a3a);
        const u32 skins[5] = {0xe0b08c, 0xc48e6c, 0xa87052, 0x7b4a33, 0xd2a27e};
        col.skin = rgb(skins[i % 5]);
        ragdolls_.spawn((0x10000000ull + c.id) * 16 + static_cast<u64>(i), r.pos, r.yaw_s, Vec3{std::cos(r.yaw_s), std::sin(r.yaw_s), 0.4f} * 2.5f, col, false, now);
      }
      r.alive = false;
      continue;
    }
    if (!r.alive) continue;  // down here (a hit of ours): stays down
    // his pace from his last two snapshots (on his clock): the runner goes on between them
    if (at > r.net_at + 0.02f && r.net_at > 0) { const Vec3 v = (pos[i] - r.net_to) * (1.0f / (at - r.net_at)); if (kern::length(v) < 12) r.net_v = v; }
    r.net_to = pos[i];
    r.net_at = at;
    r.yaw = yaw[i];
  }
  remote_joggers_at_ = now;
}

// ---------------------------------------------------------------------------------------------
// the host's animals (MP client) and thrown ones (game_toss.cpp)

void Life::set_remote_animals(bool on) {
  if (on == remote_animals_) return;
  remote_animals_ = on;
  // the other world's animals go (bodies too); the next snapshot / our own spawning brings them
  for (Cow& c : cows_) if (c.body.valid() && world_->body_valid(c.body)) world_->destroy_body(c.body);
  for (Pet& p : pets_) if (p.body.valid() && world_->body_valid(p.body)) world_->destroy_body(p.body);
  cows_.clear();
  meadows_.clear();
  flocks_.clear();
  for (Pet& p : pets_) { p.body = {}; p.shown = false; p.has_pose = false; p.carried = false; p.dead_at = -1; }
  herds_for_ = {1e9f, 1e9f};
  pet_anchor_ = {1e9f, 1e9f};
}

void Life::apply_cows(const std::vector<NetCow>& cows, f32 now, f32 at) {
  if (!remote_animals_) return;
  remote_animals_at_ = now;
  // cows by key: known ones go where he has them, new ones appear, the rest go
  std::vector<Cow> next;
  for (const NetCow& n : cows) {
    auto old = std::find_if(cows_.begin(), cows_.end(), [&](const Cow& x) { return x.key == n.key; });
    Cow c;
    if (old != cows_.end()) { c = *old; old->body = {}; }
    else {
      c.key = n.key;
      c.color = rgb(kCoats[static_cast<u32>(hash01(mix(n.key, 7)) * 5) % 5]);
      c.phase = hash01(n.key) * 6;
      c.pos = n.pos;
      c.yaw = n.yaw;
      c.next_moo = now + 8 + hash01(mix(n.key, 9)) * 30;
    }
    const bool mine = c.body.valid() && world_->body_valid(c.body);
    if (!mine) {
      if (at > c.net_at + 0.02f && c.net_at > 0) { const Vec3 v = (n.pos - c.net_to) * (1.0f / (at - c.net_at)); c.net_v = kern::length(v) < 15 ? v : Vec3{}; }
      c.net_at = at;
      c.net_to = n.pos;
      c.net_rot = n.rot;
      c.net_dead = n.dead;
      c.walking = n.walking;
      c.carried = n.carried;
      c.yaw += wrap_pi(n.yaw - c.yaw);
    }
    next.push_back(c);
  }
  for (Cow& c : cows_) if (c.body.valid() && world_->body_valid(c.body)) world_->destroy_body(c.body);
  cows_ = std::move(next);
}

void Life::apply_pets(const std::vector<NetPet>& pets, u32 pet_count, const std::vector<Vec2>& pet_anchors, f32 now) {
  if (!remote_animals_) return;
  remote_animals_at_ = now;
  // the host's neighbourhoods: the round of every dog and cat is then the same here
  if (!pet_anchors.empty()) {
    pet_anchor_ = pet_anchors[0];
    pet_anchors_ = pet_anchors;
    pet_base_ = std::max<u32>(1, pet_count / static_cast<u32>(pet_anchors.size()));
  }
  // dogs and cats by his index (his count: the species of an index is the same everywhere)
  if (pets_.size() != pet_count) {  // (a phone host has fewer: his count, each one's round by its index)
    const std::size_t had = pets_.size();
    pets_.resize(pet_count);
    for (std::size_t i = had; i < pets_.size(); ++i) pet_params(pets_[i], static_cast<u32>(i) % pet_base_);
  }
  pet_net_to_.resize(pet_count);
  pet_net_yaw_.resize(pet_count, 0);
  std::vector<bool> seen(pet_count, false);
  for (const NetPet& n : pets) {
    if (n.idx >= pet_count) continue;
    seen[n.idx] = true;
    Pet& p = pets_[n.idx];
    if (p.body.valid() && world_->body_valid(p.body)) continue;  // ours right now
    pet_net_to_[n.idx] = n.pos;
    pet_net_yaw_[n.idx] = n.yaw;
    p.off = n.off;
    p.net_rot = n.rot;
    p.shown = n.shown;
    p.carried = n.carried;
    if (n.dead && p.dead_at < 0) p.dead_at = now;
    if (!n.dead) p.dead_at = -1;
  }
  for (u32 i = 0; i < pet_count; ++i)
    if (!seen[i] && !(pets_[i].body.valid() && world_->body_valid(pets_[i].body))) { pets_[i].shown = false; pets_[i].has_pose = false; }
}

void Life::apply_flocks(const std::vector<NetFlock>& flocks, f32 now, f32 at) {
  if (!remote_animals_) return;
  remote_animals_at_ = now;
  // pigeon flocks by his id, every bird by its place in the flock (a dead one stays in its place)
  std::vector<Flock> next;
  next.reserve(flocks.size());
  for (const NetFlock& n : flocks) {
    auto old = std::find_if(flocks_.begin(), flocks_.end(), [&](const Flock& x) { return x.id == n.id; });
    Flock f;
    if (old != flocks_.end()) f = std::move(*old);
    f.id = n.id;
    f.air = n.air;
    f.at = n.at;
    if (f.birds.size() != n.birds.size()) {
      f.birds.resize(n.birds.size());
      for (std::size_t k = 0; k < n.birds.size(); ++k) { f.birds[k].pos = n.birds[k]; f.birds[k].yaw = n.yaws[k]; f.birds[k].dead_at = -1; f.birds[k].peck = static_cast<f32>(k) * 1.7f; f.birds[k].net_at = 0; }
    }
    for (std::size_t k = 0; k < n.birds.size(); ++k) {
      Pigeon& b = f.birds[k];
      // shot there: it drops here (where it lies is his)
      if (n.dead[k] && b.dead_at < 0) { b.dead_at = now; b.fall_v = {}; }
      if (!n.dead[k]) b.dead_at = -1;
      if (n.dead[k]) { if (kern::length(b.pos - n.birds[k]) > 1.5f) b.pos = n.birds[k]; continue; }
      // its flight between his snapshots: the pace of his last two (on his clock)
      if (at > b.net_at + 0.02f && b.net_at > 0) { const Vec3 v = (n.birds[k] - b.net_to) * (1.0f / (at - b.net_at)); b.net_v = kern::length(v) < 25 ? v : Vec3{}; }
      b.net_to = n.birds[k];
      b.net_at = at;
      b.net_yaw = n.yaws[k];
    }
    next.push_back(std::move(f));
  }
  flocks_ = std::move(next);
}

kern::physics::Body Life::net_carry_cow(u64 key) {
  for (Cow& c : cows_) {
    if (c.key != key || c.dead) continue;
    if (c.body.valid() && world_->body_valid(c.body)) return c.body;
    return grab_animal(c.pos + Vec3{0, 0, 1.1f}, 0.05f);
  }
  return {};
}

kern::physics::Body Life::net_carry_pet(u32 index) {
  if (index >= pets_.size()) return {};
  Pet& p = pets_[index];
  if (p.dead_at >= 0) return {};
  if (p.body.valid() && world_->body_valid(p.body)) return p.body;
  if (!p.shown) { p.shown = true; }
  return grab_animal(p.pos + Vec3{0, 0, 0.3f * p.scale}, 0.05f);
}

void Life::net_bird(u64 key, Vec3 at, f32 yaw, Vec3 vel, bool held) {
  for (LooseBird& b : loose_)
    if (b.net == key) {
      if (held) { b.pos = at; b.yaw = yaw; b.held = true; b.flap += 0.9f; }
      else if (b.held) { b.held = false; b.t = 0; b.pos = at; b.vel = vel + Vec3{0, 0, 2.5f}; }
      return;
    }
  LooseBird lb;
  lb.id = -static_cast<i32>(++loose_serial_);
  lb.net = key;
  lb.pos = at;
  lb.yaw = yaw;
  lb.held = held;
  if (!held) lb.vel = vel + Vec3{0, 0, 2.5f};
  loose_.push_back(lb);
}

void Life::update_joggers(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out) {
  if (!view.people_on) { chain_alive_ = false; return; }
  if (remote_joggers_) {
    // the host's club: eased to his runners (100 ms snapshots), gone when he stops sending it
    if (now - remote_joggers_at_ > 1.5f) { chain_alive_ = false; return; }
    if (!chain_alive_) return;
    for (Runner& r : chain_.runners) {
      if (!r.alive) continue;
      const Vec3 d = r.net_to + r.net_v * clampf(now - r.net_at, 0, 0.6f) - r.pos;
      const f32 k = std::fmin(1.0f, dt * 10);
      r.pos = r.pos + d * k;
      r.pos.z = ground(view, r.pos.x, r.pos.y);
      r.yaw_s += clampf(wrap_pi(r.yaw - r.yaw_s), -9 * dt, 9 * dt);
      r.phase += kSpeed * dt * 2.4f;
    }
    return;
  }
  if (!chain_alive_) {
    if (next_chain_ < 0) next_chain_ = now + kFirst;
    if (now >= next_chain_) {
      next_chain_ = now + rng.range(kEveryMin, kEveryMax);
      if (!spawn_joggers(view, now, rng)) next_chain_ = now + 15;
    }
    return;
  }
  Chain& c = chain_;
  i32 lead = -1;
  for (u32 i = 0; i < kRunners; ++i)
    if (c.runners[i].alive) { lead = static_cast<i32>(i); break; }
  if (lead < 0 || (now - c.born > kChainLife && dist2d(c.runners[lead].pos, view.player) > 70)) { chain_alive_ = false; return; }
  c.s += kSpeed * dt;
  for (u32 i = 0; i < kRunners; ++i) {
    Runner& r = c.runners[i];
    if (!r.alive) continue;
    f32 yaw;
    Vec2 n;
    const Vec3 q = sample(c.s - static_cast<f32>(i) * kGap, yaw, n);
    r.pos = {q.x, q.y, ground(view, q.x, q.y)};
    r.yaw = yaw;
    r.yaw_s += clampf(wrap_pi(r.yaw - r.yaw_s), -9 * dt, 9 * dt);  // a quick pivot at the end of the street
    r.phase += kSpeed * dt * 3.4f;
  }
  // cars run the line over (checkCar: footprint 2.15 x 1.15 m, above 2.5 m/s)
  for (const LifeCar& car : view.cars) {
    if (car.airborne) continue;
    const f32 sp = std::hypot(car.velocity.x, car.velocity.y);
    if (sp < 2.5f) continue;
    for (u32 i = 0; i < kRunners; ++i) {
      const Runner& r = c.runners[i];
      if (!r.alive) continue;
      const Vec3 d = r.pos - car.position;
      if (d.x * d.x + d.y * d.y > 9 + car.half.x * car.half.x) continue;
      const Vec3 l = to_local(car.rotation, d);
      if (std::fabs(l.x) < std::fmax(2.15f, car.half.x + 0.3f) && std::fabs(l.y) < std::fmax(1.15f, car.half.y + 0.3f))
        kill_runner(static_cast<i32>(i), {car.velocity.x * 0.8f, car.velocity.y * 0.8f, 2.4f + sp * 0.12f}, now, out);
    }
  }
  // chant and beat
  const Runner& L = c.runners[lead];
  const f32 d = kern::length(L.pos - view.eye), vol = clampf(1 - d / 22 / 3, 0, 1);
  if (now >= c.next_beat) {
    c.next_beat = now + 0.4f;
    ++c.beat;
    out.sounds.push_back({c.beat % 2 == 0 ? LifeOut::Sound::stomp_accent : LifeOut::Sound::stomp, L.pos, c.beat % 2 == 0 ? 0.55f : 0.32f, 1});
  }
  if (now >= c.next_call) {
    const i32 k = c.call++ % 6;
    c.next_call = now + 6.4f;
    if (vol > 0.06f) {
      out.speech.push_back({kCalls[k][0], 0.85f, 1.08f, vol});
      u32 alive = 0;
      for (const Runner& x : c.runners) alive += x.alive ? 1 : 0;
      if (alive > 1) out.speech.push_back({kCalls[k][1], 1.15f, 1.12f, vol * 0.95f, 1.7f});  // the group answers after 1.7 s
    }
  }
}

// ---------------------------------------------------------------------------------------------
// hits

bool Life::shot(Vec3 from, Vec3& to, f32 damage, f32 now, LifeOut& out, bool melee) {
  f32 best = 2;
  i32 kind = -1, which = -1;
  // cows: the body as an oriented box
  for (std::size_t i = 0; i < cows_.size(); ++i) {
    const Cow& c = cows_[i];
    if (c.dead) continue;
    const Quat q = yaw_quat(c.yaw);
    const Vec3 a = to_local(q, from - (c.pos + Vec3{0, 0, 1.15f})), b = to_local(q, to - (c.pos + Vec3{0, 0, 1.15f}));
    const Vec3 h{0.95f, 0.36f, 0.42f}, d = b - a;
    f32 t0 = 0, t1 = 1;
    bool hit = true;
    for (int k = 0; k < 3 && hit; ++k) {
      const f32 o = k == 0 ? a.x : k == 1 ? a.y : a.z, dd = k == 0 ? d.x : k == 1 ? d.y : d.z, hh = k == 0 ? h.x : k == 1 ? h.y : h.z;
      if (std::fabs(dd) < 1e-7f) { if (std::fabs(o) > hh) hit = false; continue; }
      f32 e0 = (-hh - o) / dd, e1 = (hh - o) / dd;
      if (e0 > e1) std::swap(e0, e1);
      t0 = std::fmax(t0, e0);
      t1 = std::fmin(t1, e1);
      if (t0 > t1) hit = false;
    }
    if (hit && t0 < best) { best = t0; kind = 0; which = static_cast<i32>(i); }
  }
  for (std::size_t i = 0; i < birds_.size(); ++i) {
    const Bird& b = birds_[i];
    if (now < b.dead_until || now < b.dying_until) continue;
    const f32 t = seg_sphere(from, to, b.pos, 0.45f);
    if (t >= 0 && t < best) { best = t; kind = 1; which = static_cast<i32>(i); }
  }
  // pigeons on the ground and in the air: a small sphere each (kind 4, which = flock * 64 + bird)
  for (std::size_t fi = 0; fi < flocks_.size(); ++fi)
    for (std::size_t bi = 0; bi < flocks_[fi].birds.size() && bi < 64; ++bi) {
      const Pigeon& pg = flocks_[fi].birds[bi];
      if (pg.dead_at >= 0) continue;
      const f32 t = seg_sphere(from, to, pg.pos + Vec3{0, 0, 0.15f}, 0.2f);
      if (t >= 0 && t < best) { best = t; kind = 4; which = static_cast<i32>(fi * 64 + bi); }
    }
  if (chain_alive_)
    for (u32 i = 0; i < kRunners; ++i) {
      const Runner& r = chain_.runners[i];
      if (!r.alive) continue;
      for (f32 z : {0.6f, 1.15f, 1.6f}) {
        const f32 t = seg_sphere(from, to, r.pos + Vec3{0, 0, z}, 0.3f);
        if (t >= 0 && t < best) { best = t; kind = 2; which = static_cast<i32>(i); }
      }
    }
  for (std::size_t i = 0; i < pets_.size(); ++i) {
    const Pet& p = pets_[i];
    if (!p.shown || p.dead_at >= 0) continue;
    const f32 sc = species(static_cast<u32>(i), epoch_) == Species::dog ? p.scale : p.scale * 0.62f;
    const f32 t = seg_sphere(from, to, p.pos + Vec3{0, 0, 0.38f * sc}, 0.34f * sc);
    if (t >= 0 && t < best) { best = t; kind = 3; which = static_cast<i32>(i); }
  }
  if (kind < 0) return false;
  to = from + (to - from) * best;
  Vec3 dir = to - from;
  const f32 l = std::fmax(1e-4f, kern::length(dir));
  dir = dir * (1 / l);
  if (kind == 0) {
    Cow& c = cows_[which];
    if (now - c.last_hit < 0.04f) return true;
    c.last_hit = now;
    ++cow_hits_;
    out.sounds.push_back({LifeOut::Sound::thud, to, 0.5f, 1});
    if (melee) damage = 6;  // cows.mjs: a fist does 6
    c.hp -= damage;
    if (c.hp <= 0) {
      const f32 k = std::fmin(4.0f, 1 + damage / 120);
      kill_cow(c, {dir.x * kCowMass * k, dir.y * kCowMass * k, kCowMass * 1.2f}, now, out);
    } else {
      // a hit rocks the 600 kg animal a step aside, then it bolts
      c.off = c.off + Vec2{dir.x, dir.y} * (melee ? 0.35f : 0.12f);
      c.flinch_at = now;
      scare_cow(c, {c.pos.x - dir.x * 3, c.pos.y - dir.y * 3}, now, 5.5f);
      out.sounds.push_back({LifeOut::Sound::moo_pain, c.pos + Vec3{0, 0, 1.4f}, 0.9f, 1.45f});
    }
  } else if (kind == 1) {
    Bird& b = birds_[which];
    ++bird_hits_;
    b.dying_until = now + 1.1f;
    b.dead_until = now + 14;
    b.down = false;
    b.fall_v = {dir.x * 2.2f, dir.y * 2.2f, -0.4f};
    b.spin = 4 + hash01(mix(b.seed, static_cast<u64>(now * 100))) * 4;
    {
      // a shrill cry, its pitch the bird's own and a little random, and the wings beating wildly
      const f32 h = hash01(mix(b.seed, static_cast<u64>(now * 1000)));
      out.sounds.push_back({LifeOut::Sound::squawk, b.pos, 1.0f, 0.8f + 0.5f * hash01(b.seed) + 0.1f * h});
      out.sounds.push_back({LifeOut::Sound::squawk, b.pos, 0.7f, 1.3f + 0.3f * h});
      out.sounds.push_back({LifeOut::Sound::flap, b.pos, 0.9f, 1.2f});
    }
    out.bursts.push_back({b.pos, {0.8f, 0.75f, 0.66f}});
  } else if (kind == 4) {
    // a shot pigeon drops where it was, feathers fly, the rest of the flock takes off
    Flock& f = flocks_[static_cast<std::size_t>(which / 64)];
    Pigeon& pg = f.birds[static_cast<std::size_t>(which % 64)];
    pg.dead_at = now;
    pg.fall_v = {dir.x * 1.6f, dir.y * 1.6f, f.air ? -0.5f : 1.2f};
    ++bird_hits_;
    out.sounds.push_back({LifeOut::Sound::squawk, pg.pos, 0.9f, 1.05f + 0.35f * hash01(static_cast<u32>(which) * 2654435761u + static_cast<u32>(now * 100))});
    out.sounds.push_back({LifeOut::Sound::coo, pg.pos, 0.6f, 1.5f});
    out.sounds.push_back({LifeOut::Sound::flap, pg.pos, 0.8f, 1});
    out.bursts.push_back({pg.pos + Vec3{0, 0, 0.15f}, {0.72f, 0.74f, 0.78f}});
    spook_pigeons(now);
  } else if (kind == 2) {
    kill_runner(which, {dir.x * 2.5f, dir.y * 2.5f, 1.4f}, now, out);
  } else {
    const u32 i = static_cast<u32>(which);
    const Species sp = species(i, epoch_);
    if (sp == Species::black_cat) {
      // shoot the black cat and it charges (world_procedural_population.mjs killAnimal)
      if (!cat_.on && provoke_cat(now)) out.sounds.push_back({LifeOut::Sound::hiss, pets_[i].pos, 1, 1});
    } else if (melee) {
      Pet& p = pets_[i];
      out.sounds.push_back({sp == Species::cat ? LifeOut::Sound::cat : LifeOut::Sound::dog, p.pos, 0.8f, 1});
      if (++p.punches >= (sp == Species::cat ? 2 : 3)) { p.punches = 0; kill_pet(i, dir * 3.0f, now, out); }
    } else {
      kill_pet(i, dir * 4.0f, now, out);
    }
  }
  return true;
}

void Life::remote_cow_death(Vec3 at, Vec3 impulse, f32 now, LifeOut& out) {
  Cow* best = nullptr;
  f32 bd = 4;
  for (Cow& c : cows_)
    if (!c.dead && dist2d(c.pos, at) < bd) { bd = dist2d(c.pos, at); best = &c; }
  if (!best) return;
  kill_cow(*best, impulse, now, out);
  out.cow_deaths.pop_back();  // not sent back
}

void Life::heard_shot(Vec3 from, Vec3 to, f32 now) {
  // a shot near the herd startles it (cows.mjs onTracer); pigeons in 40 m fly (pigeons.mjs)
  const f32 len = dist2d(from, to);
  const Vec3 mid = (from + to) * 0.5f;
  for (Cow& c : cows_)
    if (!c.dead && dist2d(c.pos, mid) < len / 2 + 12) scare_cow(c, {from.x, from.y}, now, 3);
  for (Flock& f : flocks_)
    if (!f.air && std::hypot(f.at.x - from.x, f.at.y - from.y) < 40) f.next_flight = std::fmin(f.next_flight, now);
}

void Life::blast(Vec3 at, f32 radius, f32 now, LifeOut& out) {
  const f32 r = std::fmax(4.0f, radius);
  for (Cow& c : cows_) {
    if (c.dead) continue;
    const Vec3 d = c.pos - at;
    const f32 dist = std::hypot(d.x, d.y);
    if (dist < r * 0.75f) {
      const f32 k = kCowMass * (7 * (1 - dist / r) + 2) / std::fmax(0.5f, dist);
      kill_cow(c, {d.x * k, d.y * k, kCowMass * 5 * (1 - dist / r) + kCowMass}, now, out);
    } else if (dist < r * 4) scare_cow(c, {at.x, at.y}, now, 4.5f);
  }
  for (Flock& f : flocks_)
    if (!f.air && std::hypot(f.at.x - at.x, f.at.y - at.y) < 90) f.next_flight = std::fmin(f.next_flight, now);
  // trees in the blast fall over, away from it, about their foot (nothing left in the air)
  for (Tree& t : trees_) {
    if (t.fell_at >= 0) continue;
    const Vec2 d{t.foot.x - at.x, t.foot.y - at.y};
    const f32 dist = std::hypot(d.x, d.y);
    if (dist > r * 0.9f || std::fabs(t.foot.z - at.z) > r) continue;
    t.fell_at = now;
    t.fall_dir = dist > 0.1f ? d * (1 / dist) : Vec2{1, 0};
    ++trees_down_;
    out.sounds.push_back({LifeOut::Sound::thud, t.foot + Vec3{0, 0, 1}, 0.8f, 0.7f});
    out.bursts.push_back({t.foot + Vec3{0, 0, 2.6f}, {0.31f, 0.52f, 0.34f}});
  }
  if (chain_alive_) {
    const f32 R = std::fmax(3.0f, radius);
    for (u32 i = 0; i < kRunners; ++i) {
      const Runner& rn = chain_.runners[i];
      if (!rn.alive) continue;
      const Vec3 d = rn.pos - at;
      const f32 dist = std::hypot(d.x, d.y);
      if (dist < R * 0.7f) {
        const f32 k = (1 - dist / R) * 7 / std::fmax(0.3f, dist);
        kill_runner(static_cast<i32>(i), {d.x * k, d.y * k, 4 + 3 * (1 - dist / R)}, now, out);
      }
    }
  }
  for (Bird& b : birds_) {
    if (now < b.dead_until || kern::length(b.pos - at) > r) continue;
    b.dying_until = now + 1.1f;
    b.dead_until = now + 14;
    b.down = false;
    b.fall_v = (b.pos - at) * 0.5f;
    b.spin = 6;
    out.sounds.push_back({LifeOut::Sound::squawk, b.pos, 0.9f, 0.9f + 0.4f * hash01(b.seed)});
  }
  for (u32 i = 0; i < pets_.size(); ++i) {
    Pet& p = pets_[i];
    if (!p.shown || p.dead_at >= 0 || (cat_.on && i == 1)) continue;
    const Vec3 d = p.pos - at;
    const f32 dist = kern::length(d);
    if (dist < r) kill_pet(i, (dist > 0.1f ? d * (1 / dist) : Vec3{0, 0, 1}) * (8 * (1 - dist / r)) + Vec3{0, 0, 3}, now, out);
  }
  ragdolls_.blast(at, r * 1.2f, 6);
}

// ---------------------------------------------------------------------------------------------
// street voices (street_voices.mjs)

bool Life::shout(Vec3 at, const char* mood, u32 seed, f32 now, f32 d, LifeOut& out) {
  if (now - last_shout_ < kShoutGap || d > kMaxHear) return false;
  for (auto& [s, t] : source_gap_)
    if (s == seed && now - t < kSourceGap) return false;
  const Lines* lines = &kLines[0];
  for (const Lines& l : kLines)
    if (std::string(l.mood) == mood) lines = &l;
  const f32 r = hash01(mix(seed, static_cast<u64>(now / 0.997f)));
  const char* line = lines->lines[static_cast<u32>(r * static_cast<f32>(lines->n)) % lines->n];
  last_shout_ = now;
  source_gap_.push_back({seed, now});
  if (source_gap_.size() > 32) source_gap_.erase(source_gap_.begin());
  while (voices_.size() >= 3) voices_.erase(voices_.begin());
  voices_.push_back({at + Vec3{0, 0, 2.05f}, line, mood, now + kBubble});
  const f32 vol = std::fmax(0.0f, 1 - d / kMaxHear);
  const f32 k = hash01(seed);
  if (vol > 0.03f) out.speech.push_back({line, 0.65f + k * 0.8f, std::string(mood) == "scared" ? 1.35f : 1.12f + hash01(mix(seed, 5)) * 0.2f, std::fmin(1.0f, 0.25f + vol * 0.9f)});
  ++shouts_;
  return true;
}

bool Life::honk(Vec3 at, bool bus, u32 source, f32 now, f32 d, LifeOut& out) {
  if (now - last_honk_ < kHonkGap || d > 160) return false;
  for (auto& [s, t] : source_gap_)
    if (s == (source | 0x80000000u) && now - t < 4) return false;
  last_honk_ = now;
  source_gap_.push_back({source | 0x80000000u, now});
  out.sounds.push_back({bus ? LifeOut::Sound::horn_bus : LifeOut::Sound::horn, at + Vec3{0, 0, 1}, 0.8f, 1});
  ++honks_;
  return true;
}

bool Life::scream(Vec3 at, u32 seed, f32 now, f32 d, LifeOut& out) {
  if (now - last_scream_ < 0.45f || d > 55) return false;
  last_scream_ = now;
  const f32 k = hash01(seed);
  out.sounds.push_back({k > 0.45f ? LifeOut::Sound::scream_f : LifeOut::Sound::scream, at + Vec3{0, 0, 1.6f}, 0.9f, 0.9f + k * 0.25f});
  return true;
}

void Life::update_voices(f32 dt, f32 now, const LifeView& view, LifeOut& out) {
  (void)dt; (void)view; (void)out;
  voices_.erase(std::remove_if(voices_.begin(), voices_.end(), [&](const Voice& v) { return now > v.until; }), voices_.end());
}

}  // namespace sc
