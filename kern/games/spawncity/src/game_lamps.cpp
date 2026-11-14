// SpawnCity on Kern: street lamps as street_lamps.mjs has them (rules in lamps.h).
//  * placed every 34 m along the streamed streets (not paths / tracks), alternating kerbs, 1.15 m
//    beyond the road edge; a 6.2 m dark steel pole (0x2b2e31) with a 1.5 m arm and a lamp head
//    (0.6 x 0.26 x 0.14 m, 1.42 m out) glowing sodium-orange (0xffb35a x 3.2) and a warm pool of
//    light on the ground (13 m, 0.55 x night) from dusk on,
//  * a round from the player on foot through the head puts it out (glass burst), a blast inside
//    60 % of its radius tops the post (a 140 kg Box3D prop that falls and rolls), further out it
//    darkens the head; a car faster than 3.5 m/s heading into a post shears it off (and loses some
//    speed), a slower one is stopped by it (clang); at most 14 fallen posts,
//  * the other players see the same: a lamp we break goes to them at once (Fx lamp: the glass, the
//    post falling the way it was hit), blasts break lamps on every machine on their own - and which
//    lamps are broken is the host's state (game_townsync.cpp): every break is told to him, he tells
//    everybody, a player who joins later gets them all; RESET puts every lamp back up.
#include <algorithm>
#include <cmath>

#include "game.h"
#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
namespace prim = kern::primitives;

Mat4 yaw_at(Vec3 p, f32 yaw) { return Mat4::trs(p, Quat::axis_angle({0, 0, 1}, yaw), {1, 1, 1}); }
Vec3 rgb(u32 hex) { return {static_cast<f32>((hex >> 16) & 255) / 255, static_cast<f32>((hex >> 8) & 255) / 255, static_cast<f32>(hex & 255) / 255}; }

// street_lamps.mjs glass(): white noise, cubic decay over 0.4 s, sparkles, high-passed at 3200 Hz
std::vector<f32> glass_pcm(u32 rate) {
  std::vector<f32> out(static_cast<std::size_t>(rate * 0.4f));
  u32 s = 0x9e3779b9u;
  f32 prev_x = 0, prev_y = 0;
  const f32 rc = 1.0f / (2 * kPi * 3200), dt = 1.0f / static_cast<f32>(rate), a = rc / (rc + dt);
  for (std::size_t i = 0; i < out.size(); ++i) {
    s = s * 1664525u + 1013904223u;
    const f32 r = static_cast<f32>(s >> 8) / 16777216.0f;
    s = s * 1664525u + 1013904223u;
    const f32 spark = static_cast<f32>(s >> 8) / 16777216.0f < 0.04f ? 3.0f : 1.0f;
    const f32 x = (r * 2 - 1) * std::pow(1 - static_cast<f32>(i) / static_cast<f32>(out.size()), 3.0f) * spark;
    const f32 y = a * (prev_y + x - prev_x);  // one-pole high-pass
    prev_x = x;
    prev_y = y;
    out[i] = y;
  }
  return out;
}

// clang(): three triangle partials 180 / 412 / 733 Hz, .32 each, exponential decay over 1.1 s
std::vector<f32> clang_pcm(u32 rate) {
  std::vector<f32> out(static_cast<std::size_t>(rate * 1.2f));
  for (f32 f : {180.0f, 412.0f, 733.0f})
    for (std::size_t i = 0; i < out.size(); ++i) {
      const f32 t = static_cast<f32>(i) / static_cast<f32>(rate), p = std::fmod(t * f, 1.0f);
      const f32 env = 0.32f * std::exp(std::log(0.001f / 0.32f) * std::fmin(1.0f, t / 1.1f));
      out[i] += (1 - 4 * std::fabs(p - 0.5f)) * env;
    }
  return out;
}
}  // namespace

void Game::gaps_lamps_init(kern::Renderer& r) {
  GapsState& g = *gaps_;
  auto make = [&](GapsState::Part& p, const kern::MeshData& mesh, kern::Material m) {
    p.id = r.allocate_id();
    kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
    in.material = m;
    in.visible = false;
    static const Mat4 parked = Mat4::translate({0, 0, -1000});
    in.transforms = kern::Span<const f32>(parked.m, 16);
    if (!r.upsert(p.id, in)) { r.release_id(p.id); p.id = 0; }
  };
  const Vec3 steel = rgb(0x2b2e31), head = rgb(0x2a2a28);
  kern::MeshData pole;
  prim::append(pole, prim::cylinder(0.09f, lamps::kPoleH, 7, steel), Mat4{});  // CylinderGeometry(.07, .11, 6.2, 7)
  prim::append(pole, prim::box({1.5f, 0.09f, 0.09f}, steel), Mat4::translate({0.72f, 0, lamps::kPoleH - 0.1f}));
  prim::append(pole, prim::box({0.6f, 0.26f, 0.14f}, head), Mat4::translate({lamps::kArm, 0, lamps::kHeadZ}));
  kern::Material pm;
  pm.roughness = 0.55f;
  pm.metalness = 0.6f;
  make(g.lamp_pole, pole, pm);
  make(g.lamp_fallen, pole, pm);
  // the glowing lens under the head (emissive 0xffb35a x 3.2, scaled by the night per instance)
  kern::Material lens;
  lens.unlit = true;
  lens.cast_shadow = false;
  lens.emissive = rgb(0xffb35a) * 3.2f;
  make(g.lamp_glow, prim::box({0.6f, 0.26f, 0.02f}, {1, 1, 1}), lens);
  // the pool of light: four stacked discs approximate the additive smoothstep falloff
  kern::Material pool;
  pool.alpha = kern::AlphaMode::blend;
  pool.unlit = true;
  pool.cast_shadow = false;
  pool.double_sided = true;
  pool.color = {1.0f, 0.72f, 0.38f, 1};
  make(g.lamp_pool, prim::cylinder(0.5f, 0.01f, 20, {1, 1, 1}), pool);
}

void Game::gaps_lamps_sounds() {
  if (!gaps_ || gaps_->lamp_sounds || !bank_) return;
  const u32 rate = 44100;
  const std::vector<f32> glass = glass_pcm(rate), clang = clang_pcm(rate);
  bank_->request("sc:lampglass");
  bank_->set_pcm("sc:lampglass", glass.data(), static_cast<u32>(glass.size()), 1, rate);
  bank_->request("sc:lampclang");
  bank_->set_pcm("sc:lampclang", clang.data(), static_cast<u32>(clang.size()), 1, rate);
  gaps_->lamp_sounds = true;
}

void Game::gaps_lamps_place() {
  // The lamps are the map tiles' (tiles.h furnish; street_lamps.mjs setLampRoads lays them anew
  // with every road set): taken whenever the set of tiles changes - a tile that goes takes its
  // lamps along, what was broken stays broken (by cell). Not in open water.
  GapsState& g = *gaps_;
  if (g.lamp_serial != city_.furniture_serial()) {
    g.lamp_serial = city_.furniture_serial();
    std::vector<u64> tiles;
    city_.furniture(tiles);
    std::vector<lamps::Field::Spot> spots;
    for (u64 tile : tiles)
      for (const MapFeatures::Spot& sp : city_.features(tile).lamps)
        if (!city_.terrain().water_at(sp.p.x, sp.p.y)) spots.push_back({sp.p.x, sp.p.y, sp.a, city_.ground_height(sp.p.x, sp.p.y)});
    g.lamps.rebuild(spots);
    g.lamps_dirty = true;
  }
  for (const kern::Vec4& r : ground_changes_) { g.lamps.reground(r.x, r.y, r.z, r.w, city_); g.lamps_dirty = true; }
  if (g.lamps_dirty) gaps_lamps_mirror();
}

void Game::gaps_lamps_mirror() {
  // the lamp list the look reads (point lights of the nearest three lit lamps)
  lamps_.clear();
  for (const lamps::Lamp& l : gaps_->lamps.all()) lamps_.push_back({l.foot, l.yaw, l.lane, l.state});
  gaps_->lamps_dirty = false;
}

void Game::gaps_lamp_break(i32 i, u8 state, Vec2 dir, f32 speed, bool net) {
  GapsState& g = *gaps_;
  if (i < 0 || i >= static_cast<i32>(g.lamps.all().size())) return;
  const u8 was = g.lamps.all()[static_cast<std::size_t>(i)].state;
  if (!g.lamps.set_state(i, state)) return;
  const lamps::Lamp& l = g.lamps.all()[static_cast<std::size_t>(i)];
  g.lamps_dirty = true;
  ++g.lamps_broken_events;
  townsync_lamp(l.key, state);  // in a session: the host keeps which lamps are broken (game_townsync.cpp)
  gaps_lamps_sounds();
  const f32 fx_d = kern::length(l.foot - cam_.eye), vol = std::fmax(0.0f, 1 - fx_d / 90) * 0.5f;
  if (was < 1 && vol > 0) world_->play_sound("sc:lampglass", cam_.eye, vol, 1, false);
  if (was < 1) osfx("street_lamps.glass", {l.head.x, l.head.y, l.foot.z + lamps::kPoleH - 0.2f});
  if (state == 2) {
    if (vol > 0) world_->play_sound("sc:lampclang", cam_.eye, vol, 1, false);
    osfx("street_lamps.clang", l.foot + Vec3{0, 0, 0.4f}, 1);
    // topple(): the post becomes a 140 kg box (0.22 x 0.22 x 6.2 m) and gets a push at 80 % height
    kern::physics::World& w = world_->physics();
    kern::physics::BodySettings bs;
    bs.type = kern::physics::BodyType::dynamic;
    bs.position = l.foot + Vec3{0, 0, lamps::kPoleH / 2 + 0.03f};
    bs.rotation = Quat::axis_angle({0, 0, 1}, l.yaw);
    bs.linear_damping = 0.05f;
    bs.angular_damping = 0.2f;
    const kern::physics::Body b = w.create_body(bs);
    kern::physics::ShapeSettings ss;
    ss.kind = kern::physics::ShapeKind::box;
    ss.size = {0.22f, 0.22f, lamps::kPoleH};
    ss.material.density = lamps::kPoleMass / (0.22f * 0.22f * lamps::kPoleH);
    ss.material.friction = 0.6f;
    ss.material.restitution = 0.05f;
    ss.material.category_bits = kCatVehicle;  // a prop: not a wall for the walkers' rays
    w.create_shape(b, ss);
    const f32 dl = std::fmax(1e-3f, std::hypot(dir.x, dir.y)), J = lamps::kPoleMass * std::fmin(6.0f, 1.2f + speed * 0.45f);
    w.apply_linear_impulse(b, {dir.x / dl * J, dir.y / dl * J, 0}, l.foot + Vec3{0, 0, lamps::kPoleH * 0.8f});
    g.fallen.push_back(b);
    while (g.fallen.size() > lamps::kFallenMax) {
      if (w.body_valid(g.fallen.front())) w.destroy_body(g.fallen.front());
      g.fallen.erase(g.fallen.begin());
    }
  }
  if (net && net_.active()) {
    // sendBreak: who breaks a lamp says so; the others break the lamp nearest that point
    net::Fx f;
    f.kind = net::Fx::lamp;
    f.p = l.foot;
    f.dir = {dir.x, dir.y, static_cast<f32>(state)};
    net_.send_fx(f);
  }
}

void Game::gaps_lamps_remote(const net::Fx& f) {
  GapsState& g = *gaps_;
  i32 best = -1;
  f32 bd = 4;
  for (i32 k : g.lamps.near(f.p.x, f.p.y, 4)) {
    const lamps::Lamp& l = g.lamps.all()[static_cast<std::size_t>(k)];
    const f32 d = std::hypot(l.foot.x - f.p.x, l.foot.y - f.p.y);
    if (d < bd) { bd = d; best = k; }
  }
  if (best >= 0) gaps_lamp_break(best, f.dir.z >= 1.5f ? 2 : 1, {f.dir.x, f.dir.y}, 3, false);
}

void Game::gaps_lamps_blast(Vec3 at, f32 radius) {
  if (!gaps_) return;
  for (const lamps::Field::BlastHit& h : gaps_->lamps.blast(at, radius)) gaps_lamp_break(h.lamp, h.state, h.dir, h.speed, false);
}

void Game::gaps_lamps_update(f32 dt) {
  (void)dt;
  if (!gaps_ || !world_placed_ || !show_lamps_) return;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  // rounds from the player on foot (the foot tracer)
  for (const kern::game::ShotEvent& s : world_->shots()) {
    if (s.shooter != soldier_) continue;
    const i32 hit = g.lamps.shot(s.from, s.to);
    if (hit >= 0) gaps_lamp_break(hit, 1, {}, 0, true);
  }
  // cars: every 66 ms
  if (now - g.lamp_car_check >= 0.066f || now < g.lamp_car_check) {
    g.lamp_car_check = now;
    for (TrafficCar& c : cars_) {
      if (!c.placed || !world_->actor_alive(c.actor)) continue;
      const bool bus = c.car->spec().radius > 0.4f;
      const BodyState s = state_of(c.actor);
      const Vec3 f = s.rotation.rotate({1, 0, 0});
      for (const lamps::Field::CarHit& h : g.lamps.car(s.position, std::atan2(f.y, f.x), s.velocity, bus ? 4.05f : 1.88f, bus ? 1.25f : 0.86f, c.car->spec().mass)) {
        world_->physics().apply_linear_impulse_to_center(body(c.actor), h.impulse);
        if (h.topple) gaps_lamp_break(h.lamp, 2, {s.velocity.x, s.velocity.y}, std::hypot(s.velocity.x, s.velocity.y), true);
        else if (h.clang) {
          gaps_lamps_sounds();
          const f32 vol = std::fmax(0.0f, 1 - kern::length(s.position - cam_.eye) / 90) * 0.5f * 0.5f;
          if (vol > 0) world_->play_sound("sc:lampclang", cam_.eye, vol, 1, false);
          osfx("street_lamps.clang", g.lamps.all()[static_cast<std::size_t>(h.lamp)].foot + Vec3{0, 0, 0.5f}, 0.5f);
        }
      }
    }
  }
  if (g.lamps_dirty) gaps_lamps_mirror();
}

void Game::gaps_lamps_reset() {
  if (!gaps_) return;
  GapsState& g = *gaps_;
  kern::physics::World& w = world_->physics();
  for (const kern::physics::Body& b : g.fallen)
    if (w.body_valid(b)) w.destroy_body(b);
  g.fallen.clear();
  g.lamps.reset_broken();
  g.lamps_dirty = true;
}

void Game::gaps_lamps_draw() {
  if (!gaps_) return;
  GapsState& g = *gaps_;
  for (GapsState::Part* p : {&g.lamp_pole, &g.lamp_glow, &g.lamp_pool, &g.lamp_fallen}) { p->transforms.clear(); p->tints.clear(); }
  auto put = [](GapsState::Part& p, const Mat4& m, kern::Vec4 tint) {
    p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
    p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, tint.w});
  };
  if (world_placed_ && show_lamps_) {
    const f32 glow = look::lamp_glow(night_);
    for (const lamps::Lamp& l : g.lamps.all()) {
      if (l.state == 2) continue;
      if (std::fabs(l.foot.x - cam_.eye.x) > look_.lamp_m * std::fmin(view_zoom(), 3.0f) || std::fabs(l.foot.y - cam_.eye.y) > look_.lamp_m * std::fmin(view_zoom(), 3.0f)) continue;
      const Mat4 m = yaw_at(l.foot, l.yaw);
      put(g.lamp_pole, m, {1, 1, 1, 1});
      if (l.state == 0 && glow > 0.01f) {
        put(g.lamp_glow, m * Mat4::translate({lamps::kArm, 0, lamps::kHeadZ - 0.075f}), {glow, glow, glow, 1});
        // the pool: a = smoothstep(1, 0, r)^2 * night * .55, as discs of falling radius
        const Vec3 c{l.head.x, l.head.y, city_.ground_height(l.head.x, l.head.y) + 0.14f};
        for (int k = 0; k < 4; ++k) {
          const f32 rr = 13.0f * (1 - static_cast<f32>(k) * 0.22f);
          put(g.lamp_pool, Mat4::translate(c + Vec3{0, 0, static_cast<f32>(k) * 0.004f}) * Mat4::scale({rr, rr, 1}), {1, 1, 1, glow * 0.55f * 0.22f});
        }
      }
    }
    kern::physics::World& w = world_->physics();
    for (const kern::physics::Body& b : g.fallen) {
      if (!w.body_valid(b)) continue;
      const BodyState s = read_body(w, b);
      put(g.lamp_fallen, Mat4::trs(s.position, s.rotation, {1, 1, 1}) * Mat4::translate({0, 0, -lamps::kPoleH / 2}), {1, 1, 1, 1});
    }
  }
  for (GapsState::Part* p : {&g.lamp_pole, &g.lamp_glow, &g.lamp_pool, &g.lamp_fallen}) {
    if (!p->id || !g.renderer) continue;
    if (p->transforms.empty()) { g.renderer->set_visible(p->id, false); continue; }
    g.renderer->update_instances(p->id, kern::Span<const f32>(p->transforms.data(), p->transforms.size()), kern::Span<const f32>(p->tints.data(), p->tints.size()));
    g.renderer->set_visible(p->id, true);
  }
}

}  // namespace sc
