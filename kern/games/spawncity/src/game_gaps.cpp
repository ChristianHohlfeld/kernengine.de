// SpawnCity on Kern: the modules sc-m3-gaps closed (PARITY.md), applied to the game.
//  * police_sirens.mjs: the cruisers' "Martinshorn" (B flat 4 466 Hz / E flat 5 622 Hz, 0.66 s per
//    tone: sawtooth + square + triangle through a 1650 Hz formant), the police drones' rising
//    whoop (760 -> 1480 Hz); the nearest unit drives one voice each, level 0.06 / 0.032 falling
//    with (d / 14)^1.25 up to 260 m, far sirens lose their top (two loops, bright and low-passed,
//    cross-faded by 5200 - 16 d), a passing car bends the pitch (Doppler from the distance change),
//  * wanted_dispatch_guard.mjs: a police drone is dispatched, then arrives after the dispatch delay
//    (16 s - 1.3 s per level, at least 9 s) - unseen until then; the first shot after the grace
//    max(450, 720 - 45 * stars) ms; the HUD shows the drones inbound,
//  * world_critical_damage_fx.mjs: flat dark smoke discs with a rim and, late in the countdown,
//    embers, rising from a critical car / bus / police drone / the player's drone / jet,
//  * world_organic_hit_audio.mjs: the bank's "crack" when a person dies (gain .34 x effects),
//  * impact_explosion_overlay.mjs: a spark burst where a flight-fire (drone MG) round lands (the
//    page draws it at the screen point the game reports),
//  * spawn_visibility_guard.mjs / vehicle_spawn_clearance.mjs: spawn gates (spawn_guard.h),
//  * camera_collision_guard.mjs: the camera never under the ground, the car's chase camera rises
//    over the roof when a wall pushes it in,
//  * jet_mode.mjs damage: 400 HP, thrust falls with damage, burning at 15 % for 12 s, then the
//    airframe breaks up (blast 14 m / 180) and takes the pilot with it.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <tuple>

#include "game.h"
#include "kern/audio/synth.h"
#include "kern/geo/primitives.h"
#include "game_world.h"
#include "spawn_guard.h"

namespace sc {

namespace {
using kern::Mat4;

// ---- synthesis of the siren loops (the oscillators of police_sirens.mjs voice())
struct Biquad {
  f32 b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
  f32 run(f32 x) { const f32 y = b0 * x + z1; z1 = b1 * x - a1 * y + z2; z2 = b2 * x - a2 * y; return y; }
  static Biquad lowpass(f32 f, f32 q, f32 rate) {
    const f32 w = 2 * kPi * f / rate, c = std::cos(w), al = std::sin(w) / (2 * q), a0 = 1 + al;
    Biquad b;
    b.b0 = (1 - c) / 2 / a0; b.b1 = (1 - c) / a0; b.b2 = b.b0; b.a1 = -2 * c / a0; b.a2 = (1 - al) / a0;
    return b;
  }
  static Biquad peaking(f32 f, f32 q, f32 gain_db, f32 rate) {
    const f32 A = std::pow(10.0f, gain_db / 40), w = 2 * kPi * f / rate, c = std::cos(w), al = std::sin(w) / (2 * q), a0 = 1 + al / A;
    Biquad b;
    b.b0 = (1 + al * A) / a0; b.b1 = -2 * c / a0; b.b2 = (1 - al * A) / a0; b.a1 = -2 * c / a0; b.a2 = (1 - al / A) / a0;
    return b;
  }
};
enum class Osc : u8 { saw, square, triangle, sine };
struct OscSpec { Osc type; f32 cents; f32 gain; };

std::vector<f32> siren_loop(u32 rate, f32 f0, std::initializer_list<OscSpec> oscs, bool formant, f32 cutoff) {
  const f32 secs = std::round(f0 * 2) / f0;  // whole periods of the fundamental
  std::vector<f32> out(static_cast<std::size_t>(secs * static_cast<f32>(rate)), 0.0f);
  Biquad peak = Biquad::peaking(1650, 0.9f, 7, static_cast<f32>(rate)), lp = Biquad::lowpass(4200, 0.4f, static_cast<f32>(rate)),
         air = Biquad::lowpass(cutoff, 0.5f, static_cast<f32>(rate));
  std::vector<f64> phase(oscs.size(), 0.0);
  for (std::size_t n = 0; n < out.size(); ++n) {
    f32 s = 0;
    std::size_t k = 0;
    for (const OscSpec& o : oscs) {
      const f64 f = f0 * std::pow(2.0, o.cents / 1200.0);
      phase[k] += f / rate;
      phase[k] -= std::floor(phase[k]);
      const f32 p = static_cast<f32>(phase[k]);
      f32 v = 0;
      switch (o.type) {
        case Osc::saw: v = 2 * p - 1; break;
        case Osc::square: v = p < 0.5f ? 1.0f : -1.0f; break;
        case Osc::triangle: v = 1 - 4 * std::fabs(p - 0.5f); break;
        case Osc::sine: v = std::sin(2 * kPi * p); break;
      }
      s += v * o.gain;
      ++k;
    }
    if (formant) s = peak.run(s);
    s = lp.run(s);
    if (cutoff < 4000) s = air.run(s);
    out[n] = s;
  }
  kern::audio::make_seamless(out, rate, 0.05f);
  kern::audio::normalize(out, 0.8f);
  return out;
}

Mat4 billboard(Vec3 p, Vec3 right, Vec3 up, Vec3 normal, f32 size, f32 rotation) {
  const f32 c = std::cos(rotation), s = std::sin(rotation);
  const Vec3 r = (right * c + up * s) * size, u = (up * c - right * s) * size;
  Mat4 m;
  m.m[0] = r.x; m.m[1] = r.y; m.m[2] = r.z;
  m.m[4] = u.x; m.m[5] = u.y; m.m[6] = u.z;
  m.m[8] = normal.x; m.m[9] = normal.y; m.m[10] = normal.z;
  m.m[12] = p.x; m.m[13] = p.y; m.m[14] = p.z;
  return m;
}

// The smoke sprite of world_critical_damage_fx.mjs makeTexture(): a filled white disc (radius
// 27/32) with a grey (180/255) rim 3 px wide - as a mesh, the rim in the vertex colours.
kern::MeshData smoke_disc() {
  kern::MeshData d;
  const u32 seg = 24;
  const f32 inner = 25.5f / 32, outer = 28.5f / 32;
  const Vec3 n{0, 0, 1}, white{1, 1, 1}, rim{0.706f, 0.706f, 0.706f};
  d.push({0, 0, 0}, n, white, {0.5f, 0.5f});
  for (u32 i = 0; i < seg; ++i) {
    const f32 a = static_cast<f32>(i) / seg * 2 * kPi;
    d.push({std::cos(a) * inner, std::sin(a) * inner, 0}, n, white, {0, 0});
    d.push({std::cos(a) * inner, std::sin(a) * inner, 0}, n, rim, {0, 0});
    d.push({std::cos(a) * outer, std::sin(a) * outer, 0}, n, rim, {0, 0});
  }
  for (u32 i = 0; i < seg; ++i) {
    const u32 j = (i + 1) % seg, a = 1 + i * 3, b = 1 + j * 3;
    d.indices.insert(d.indices.end(), {0, a, b});
    d.indices.insert(d.indices.end(), {a + 1, a + 2, b + 2, a + 1, b + 2, b + 1});
  }
  return d;
}

constexpr f32 kJetHpMax = 400;       // jet_mode.mjs HP_MAX
constexpr f32 kJetBurnS = 12;        // startWorldCriticalDamage delayMs 12000 (jet)
constexpr f32 kPeopleEntryM = 150, kPeopleEntryMobileM = 125, kVehicleEntryM = 600;  // world_procedural_population.mjs
constexpr f32 kSpawnGraceS = 6;      // SPAWN_GRACE_MS: right after the world is shown, anything may appear
}  // namespace

void Game::gaps_init(kern::Engine& e) {
  gaps_ = std::make_unique<GapsState>();
  GapsState& g = *gaps_;
  g.smoke = gaps::SmokeField::of(mobile_ ? 48 : 78);  // MAX_PUFFS
  g.renderer = &e.renderer;
  const kern::MeshData disc = smoke_disc();
  for (GapsState::Part* p : {&g.disc, &g.ember}) {
    kern::Material m;
    m.alpha = kern::AlphaMode::blend;
    m.unlit = true;
    m.double_sided = true;
    m.cast_shadow = false;
    if (p == &g.ember) m.emissive = {1.0f, 0.54f, 0.24f};  // 0xff8a3d, additive in SpawnCity
    p->id = e.renderer.allocate_id();
    kern::MeshInput in = disc.as_input(kern::kMaxExtensionAttributes);
    in.material = m;
    in.visible = false;
    static const Mat4 parked = Mat4::translate({0, 0, -1000});
    in.transforms = kern::Span<const f32>(parked.m, 16);
    if (!e.renderer.upsert(p->id, in)) { e.renderer.release_id(p->id); p->id = 0; }
  }
  gaps_lamps_init(e.renderer);  // street lamps (game_lamps.cpp)
  {
    // the GUIDED trail: soft light-grey puffs (0xd9dcdf, opacity .42, 1.6 m)
    kern::Material m;
    m.alpha = kern::AlphaMode::blend;
    m.unlit = true;
    m.double_sided = true;
    m.cast_shadow = false;
    m.color = {0.851f, 0.863f, 0.875f, 1};
    g.trail_part.id = e.renderer.allocate_id();
    kern::MeshInput in = disc.as_input(kern::kMaxExtensionAttributes);
    in.material = m;
    in.visible = false;
    static const Mat4 parked = Mat4::translate({0, 0, -1000});
    in.transforms = kern::Span<const f32>(parked.m, 16);
    if (!e.renderer.upsert(g.trail_part.id, in)) { e.renderer.release_id(g.trail_part.id); g.trail_part.id = 0; }
    // the foot rocket's smoke (gameplay_final_runtime_v2.mjs ensureSmoke: 0xd8d8d8 sprites)
    m.color = {0.847f, 0.847f, 0.847f, 1};
    in.material = m;
    g.rocket_part.id = e.renderer.allocate_id();
    if (!e.renderer.upsert(g.rocket_part.id, in)) { e.renderer.release_id(g.rocket_part.id); g.rocket_part.id = 0; }
  }
}

void Game::gaps_guided_sounds() {
  // guided_missile.mjs lockTone (two square beeps 1760 Hz, 50 ms, 90 ms apart, .05) and the
  // ignition bang (sine 110 -> 40 Hz over .3 s, .6 -> .001 over .35 s), into Kern's SoundBank
  if (!gaps_ || gaps_->guided_sounds || !audio_.bank) return;
  const u32 rate = 44100;
  std::vector<f32> lock(static_cast<std::size_t>(rate * 0.15f), 0.0f), bang(static_cast<std::size_t>(rate * 0.4f), 0.0f);
  for (int k = 0; k < 2; ++k)
    for (u32 i = 0; i < rate * 0.05f; ++i) {
      const std::size_t n = static_cast<std::size_t>(k * 0.09f * rate) + i;
      if (n < lock.size()) lock[n] = std::fmod(static_cast<f32>(i) * 1760 / rate, 1.0f) < 0.5f ? 0.05f : -0.05f;
    }
  f64 ph = 0;
  for (std::size_t i = 0; i < bang.size(); ++i) {
    const f32 t = static_cast<f32>(i) / rate;
    ph += 110 * std::pow(40.0 / 110.0, std::fmin(1.0, t / 0.3)) / rate;
    bang[i] = static_cast<f32>(std::sin(2 * 3.14159265358979 * ph)) * 0.6f * std::pow(0.001f / 0.6f, std::fmin(1.0f, t / 0.35f));
  }
  audio_.bank->request("sc:locktone");
  audio_.bank->set_pcm("sc:locktone", lock.data(), static_cast<u32>(lock.size()), 1, rate);
  audio_.bank->request("sc:guidedbang");
  audio_.bank->set_pcm("sc:guidedbang", bang.data(), static_cast<u32>(bang.size()), 1, rate);
  gaps_->guided_sounds = true;
}

void Game::gaps_guided(f32 dt) {
  (void)dt;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  if (audio_.mixer.ready() && !g.motor_clip) {
    // motorVoice: brown-ish noise through a 700 Hz band-pass (Q .7) plus a 1300 Hz sawtooth whine (.05)
    const u32 rate = audio_.mixer.sample_rate();
    std::vector<f32> d(static_cast<std::size_t>(rate) * 2);
    u32 s = 0x51ed27u;
    f32 b = 0;
    Biquad bp;
    {
      const f32 w = 2 * kPi * 700 / static_cast<f32>(rate), c = std::cos(w), al = std::sin(w) / (2 * 0.7f), a0 = 1 + al;
      bp.b0 = al / a0; bp.b1 = 0; bp.b2 = -al / a0; bp.a1 = -2 * c / a0; bp.a2 = (1 - al) / a0;
    }
    for (std::size_t i = 0; i < d.size(); ++i) {
      s ^= s << 13; s ^= s >> 17; s ^= s << 5;
      const f32 w = static_cast<f32>(s) / 2147483648.0f - 1;
      b = 0.96f * b + 0.04f * w;
      const f32 saw = 2 * std::fmod(static_cast<f32>(i) * 1300 / static_cast<f32>(rate), 1.0f) - 1;
      d[i] = bp.run(w * 0.5f + b * 2) + saw * 0.05f;
    }
    kern::audio::make_seamless(d, rate, 0.05f);
    kern::audio::normalize(d, 0.8f);
    g.motor_clip = audio_.mixer.adopt_clip(std::move(d), 1, "spawncity:guided-motor", true);
  }
  for (GapsState::MotorVoice& v : g.motors) v.alive = false;
  for (const guided::Flight& f : guided_.flights) {
    GapsState::MotorVoice* v = nullptr;
    for (GapsState::MotorVoice& q : g.motors)
      if (q.born == f.m.born) { v = &q; break; }
    if (!v) { g.motors.push_back({f.m.born, -1, 0, false}); v = &g.motors.back(); }
    v->alive = true;
    if (!f.m.motor) continue;  // dropped off the rail, the motor not lit yet
    if (v->lit < 0) {
      v->lit = now;
      gaps_guided_sounds();
      world_->play_sound("sc:guidedbang", view_.cam.eye, 1, 1, false);
      if (g.motor_clip) {
        kern::audio::PlayParams p;
        p.loop = true;
        p.spatial = true;
        p.position = f.m.p;
        p.velocity = f.m.v;
        p.min_distance = 6;  // refDistance 6, inverse
        p.max_distance = 600;
        p.volume = 0.5f;
        p.bus = kern::audio::Bus::sfx;
        p.fade_in = 0.06f;
        v->v = audio_.mixer.play(g.motor_clip, p);
      }
    }
    if (v->v) {
      // .5 at 60 ms, exponentially to .28 at .6 s
      const f32 t = now - v->lit;
      const f32 vol = t < 0.06f ? 0.5f * t / 0.06f : 0.5f * std::pow(0.28f / 0.5f, std::fmin(1.0f, (t - 0.06f) / 0.54f));
      audio_.mixer.set_volume(v->v, vol);
      audio_.mixer.set_position(v->v, f.m.p, f.m.v);
    }
    // a puff 0.9 m behind the missile every frame
    const f32 sp = kern::length(f.m.v);
    GapsState::TrailPuff& pf = g.trail[g.trail_cursor++ % g.trail.size()];
    pf.p = f.m.p - f.m.v * (0.9f / std::fmax(1.0f, sp)) + Vec3{(gaps::unit_hash(g.trail_cursor) - 0.5f) * 0.2f, (gaps::unit_hash(g.trail_cursor + 7) - 0.5f) * 0.2f, (gaps::unit_hash(g.trail_cursor + 13) - 0.5f) * 0.2f};
    pf.born = now;
  }
  for (GapsState::MotorVoice& v : g.motors)
    if (!v.alive && v.v) audio_.mixer.stop(v.v, 0.05f);
  std::erase_if(g.motors, [](const GapsState::MotorVoice& v) { return !v.alive; });
  for (GapsState::TrailPuff& p : g.trail) {
    if (p.born < 0) continue;
    if (now - p.born > 2.6f) { p.born = -1; continue; }
    p.p.z += 0.006f * 60 * g.frame_dt;  // .006 per frame (60 fps) upwards
  }
}

void Game::gaps_remote_fx(const net::Fx& f) {
  // effects of the other players this branch adds to the fx channel
  if (!gaps_) return;
  const f32 now = world_->sim_time();
  if (f.kind == net::Fx::lamp) gaps_lamps_remote(f);
  else if (f.kind == net::Fx::strike) {
    // a mate's air strike: his mark (flare) and his jets with the stick of bombs, here too
    const Vec3 p{f.p.x, f.p.y, city_.ground_height(f.p.x, f.p.y)};
    combat_.flares.push_back({p, now, now + 9});
    launch_jets(p.x, p.y, true, combat::strike_launch_time(now), 70, 0, true);
  } else if (f.kind == net::Fx::flyover) {
    launch_jets(f.p.x, f.p.y, false, now, f.a > 0 ? f.a : 70, f.dir.x, true);
  } else if (f.kind == net::Fx::mil_tracer) {
    if (ui_.settings.on(Opt::world_military)) combat_tracer(f.p, f.dir);
  } else if (f.kind == net::Fx::mil_shell) {
    if (ui_.settings.on(Opt::world_military)) combat_.shells.push_back({f.p, kern::normalize(f.dir - f.p) * 130.0f, 3, true});
  } else if (f.kind == net::Fx::npc_death) {
    // world_procedural_population.mjs handleRemoteFx: the mate killed this person - ours with the
    // same id falls (where ours is); not here: the nearest citizen within 3 m of where it was
    const u32 id = static_cast<u32>(f.dir.x) | (static_cast<u32>(f.dir.y) << 16);
    i32 who = -1;
    f32 bd = 3;
    const auto& all = people_.all();
    for (std::size_t i = 0; i < all.size(); ++i) {
      const People::Person& p = all[i];
      if (p.kind != People::Kind::citizen || p.state == People::State::down || p.state == People::State::gone) continue;
      if (p.seed == id) { who = static_cast<i32>(i); break; }
      const f32 d = std::hypot(p.pos.x - f.p.x, p.pos.y - f.p.y);
      if (d < bd) { bd = d; who = static_cast<i32>(i); }
    }
    if (who >= 0) people_.hurt(who, 1e4f, {std::cos(f.dir.z), std::sin(f.dir.z), 0}, false, now);
  }
}

void Game::gaps_send_military(bool shell, Vec3 from, Vec3 to) {
  if (!net_.active()) return;
  net::Fx f;
  f.kind = shell ? net::Fx::mil_shell : net::Fx::mil_tracer;
  f.p = from;
  f.dir = to;
  net_.send_fx(f);
}

void Game::gaps_send_jets(bool strike, Vec3 p, f32 offset, f32 alt) {
  if (!net_.active()) return;
  net::Fx f;
  f.kind = strike ? net::Fx::strike : net::Fx::flyover;
  f.p = p;
  f.dir = {offset, 0, 0};
  f.a = alt;
  net_.send_fx(f);
}

// ---------------------------------------------------------------------------------------------
// spawn gates

bool Game::gaps_spawn_viewers(std::vector<spawn::Viewer>& vs) const {
  if (!gaps_ || !world_) return false;
  if (gaps_->world_shown >= 0 && world_->sim_time() - gaps_->world_shown < kSpawnGraceS) return false;
  // the presented camera, judged with the unscoped field of view (spawnViewFovDeg)
  spawn::Viewer me;
  me.eye = view_.cam.eye;
  me.forward = kern::normalize(view_.cam.target - view_.cam.eye, {1, 0, 0});
  const f32 aspect = frame_w_ / std::fmax(1.0f, frame_h_);
  const f32 foot_fov = 2 * std::atan(std::tan(45 * kDeg) / std::fmax(0.2f, aspect));  // Kern's soldier: 90 deg horizontal
  me.half_angle = spawn::cone_half_angle(mode_ == Mode::foot ? std::fmax(view_.cam.fov, foot_fov) : view_.cam.fov, aspect);
  me.far = std::fmax(200.0f, view_.cam.far > 0 ? view_.cam.far : spawn::default_far());
  me.local = true;
  vs.push_back(me);
  // the other players' views (collectPeerViewers: a drone looks along its nose, a person ahead)
  for (const net::Remote& r : net_.remotes()) {
    spawn::Viewer v;
    v.half_angle = me.half_angle;
    v.far = me.far;
    const net::PlayerState& s = r.state;
    switch (s.mode) {
      case net::PlayerMode::foot:
      case net::PlayerMode::parachute: v.eye = r.person_shown + Vec3{0, 0, 1.55f}; v.forward = {std::cos(s.avatar_yaw), std::sin(s.avatar_yaw), 0}; break;
      case net::PlayerMode::drone: v.eye = r.shown; v.forward = r.shown_rotation.rotate({-1, 0, 0}); break;
      case net::PlayerMode::car: v.eye = r.shown + Vec3{0, 0, 1.2f}; v.forward = r.shown_rotation.rotate({1, 0, 0}); break;
      case net::PlayerMode::jet: v.eye = r.shown; v.forward = r.shown_rotation.rotate({0, 1, 0}); break;
    }
    v.forward = kern::normalize(v.forward, {1, 0, 0});
    vs.push_back(v);
  }
  return true;
}

bool Game::gaps_can_spawn(Vec3 p, f32 height, f32 far_entry) const {
  std::vector<spawn::Viewer> vs;
  if (!gaps_spawn_viewers(vs)) return true;
  // (whether buildings hide the point is the Blueprint's own question to the world: a ray against the channel "World")
  const bool ok = spawn::can_spawn(vs, p, far_entry, height);
  ++(ok ? gaps_->spawn_allowed : gaps_->spawn_denied);
  return ok;
}

bool Game::gaps_vehicle_spawn_ok(i32 car, Vec3 pos, f32 yaw) const {
  // vehicle_spawn_clearance.mjs: no overlap with any placed vehicle (0.35 m margin), and nobody sees it appear
  const Vec3 h = cars_[static_cast<std::size_t>(car)].car->spec().half;
  const spawn::Footprint me{{pos.x, pos.y}, yaw, {h.x, h.y}};
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& o = cars_[i];
    if (static_cast<i32>(i) == car || !o.placed || !world_->actor_alive(o.actor)) continue;
    const BodyState s = state_of(o.actor);
    const Vec3 f = s.rotation.rotate({1, 0, 0});
    const Vec3 oh = o.car->spec().half;
    if (spawn::footprints_overlap(me, {{s.position.x, s.position.y}, std::atan2(f.y, f.x), {oh.x, oh.y}})) { ++gaps_->overlap_denied; return false; }
  }
  return gaps_can_spawn(pos, 2.2f, kVehicleEntryM);
}

// ---------------------------------------------------------------------------------------------
// police drone dispatch (wanted_dispatch_guard.mjs, wanted_police_pre_guard.mjs)

bool Game::gaps_drone_due(u32 slot, i32 stars) {
  // A free slot is dispatched first; it arrives (is placed) when its dispatch delay is over.
  if (!gaps_ || slot >= 5) return true;
  const f32 now = world_->sim_time();
  f32& due = gaps_->pd_due[slot];
  if (due < 0) due = now + wanted::arrival_delay_ms(stars) / 1000;
  return now >= due;
}

void Game::gaps_drone_revealed(u32 slot, i32 stars) {
  if (!gaps_ || slot >= 5) return;
  const f32 now = world_->sim_time();
  // engageAt = reveal + grace; the reveal sets the next shot to max(engageAt, now + 260 + 90 * index ms)
  police_.pdrones[slot].next_shot = std::fmax(now + gaps::post_arrival_grace_ms(stars) / 1000, now + 0.26f + static_cast<f32>(slot) * 0.09f);
  gaps_->pd_due[slot] = -1;
}

// ---------------------------------------------------------------------------------------------
// per frame

void Game::gaps_update(f32 dt) {
  if (!gaps_ || !world_placed_) return;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  g.frame_dt = dt;
  if (g.world_shown < 0) g.world_shown = now;
  if (wanted::stars() <= 0 || player_dead_)
    for (f32& d : g.pd_due) d = -1;
  // reportCrime: a crime that keeps (or raises) a wanted level reveals the player - the last contact
  // is now and the last known position is where he is (the drones search there); a new star sounds
  // a short square chirp 520 + 90 s -> 760 + 100 s Hz (0.16 s, .022).
  {
    const f32 heat = wanted::heat();
    const i32 stars = wanted::stars();
    if (heat > g.last_heat + 1e-3f && stars > 0) {
      police_.last_contact = now;
      police_.last_known = player_position();
      if (mode_ == Mode::foot) police_.last_known.z -= 0.73f;
    }
    if (stars > g.last_stars && stars > 0) {
      if (audio_.bank && !g.star_tones) {
        const u32 rate = 44100;
        for (i32 s = 1; s <= 5; ++s) {
          std::vector<f32> pcm(static_cast<std::size_t>(rate * 0.16f));
          f64 ph = 0;
          const f64 f0 = 520 + 90 * s, f1 = 760 + 100 * s;
          for (std::size_t i = 0; i < pcm.size(); ++i) {
            const f64 t = static_cast<f64>(i) / rate;
            ph += f0 * std::pow(f1 / f0, t / 0.16) / rate;  // exponential ramp (playTone endFrequency)
            const f32 env = static_cast<f32>(std::fmin(1.0, t / 0.005) * std::pow(0.001 / 1.0, t / 0.16));
            pcm[i] = (std::fmod(ph, 1.0) < 0.5 ? 0.022f : -0.022f) * env;
          }
          const std::string id = "sc:star" + std::to_string(s);
          audio_.bank->request(id);
          audio_.bank->set_pcm(id, pcm.data(), static_cast<u32>(pcm.size()), 1, rate);
        }
        g.star_tones = true;
      }
      if (g.star_tones) world_->play_sound("sc:star" + std::to_string(std::min(5, stars)), view_.cam.eye, 1, 1, false);
      osfx("wanted_police_drones.star", view_.cam.eye, static_cast<f32>(stars));
    }
    g.last_heat = heat;
    g.last_stars = stars;
  }
  // ---- critical smoke (world_critical_damage_fx.mjs) for everything with a countdown
  if (g.car_crit.size() != cars_.size()) g.car_crit.assign(cars_.size(), -1);
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    const std::string id = "car-" + std::to_string(i);
    if (!c.placed || c.wreck || c.burn_until < 0) { if (g.car_crit[i] >= 0) g.smoke.stop(id); g.car_crit[i] = -1; continue; }
    const bool bus = c.car->spec().radius > 0.4f;
    const Mat4 m = matrix_of(c.actor);
    // criticalVisualProfile: car offset (1.2, 0, .82) scale .92, bus (2.7, 0, 1.75) scale 1.35
    const Vec3 at = (m * Mat4::translate(bus ? Vec3{2.7f, 0, 1.75f} : Vec3{1.2f, 0, 0.82f})).translation();
    if (g.car_crit[i] < 0) g.smoke.start(id, at, bus ? 1.35f : 0.92f, c.burn_until - now + 1, now);
    else g.smoke.move(id, at);
    g.car_crit[i] = c.burn_until;
  }
  for (u32 i = 0; i < 5; ++i) {
    const std::string id = "pd-" + std::to_string(i);
    const PoliceDrone& d = police_.pdrones[i];
    if (d.active && details_.pd_crit[i].armed()) {
      const Vec3 at = d.pos + Vec3{-0.12f * std::cos(d.yaw), -0.12f * std::sin(d.yaw), 0.18f};  // offset (-.12, 0, .18), scale .68
      if (!g.smoke.active(id)) g.smoke.start(id, at, 0.68f, 30, now);
      else g.smoke.move(id, at);
    } else g.smoke.stop(id);
  }
  if (vehicle_.drone_out && details_.drone_crit.armed()) {
    const Vec3 at = matrix_of(drone_actor_).translation() + Vec3{0, 0, 0.4f};  // the generic profile (0, 0, .4) scale .7
    if (!g.smoke.active("drone")) g.smoke.start("drone", at, 0.7f, 30, now);
    else g.smoke.move("drone", at);
  } else g.smoke.stop("drone");
  if (g.jet_burning && vehicle_.jet_placed) {
    const Mat4 m = matrix_of(jet_actor_);
    g.smoke.move("jet", (m * Mat4::translate({0, -6, 0.4f})).translation());  // offset (0, -6, .4) scale 2.2
    if (!g.smoke.active("jet")) {
      // the 12 s are over: the airframe breaks up (destroyAirframe)
      g.jet_burning = false;
      const Vec3 at = m.translation();
      osfx("jet_mode.boom", at);  // destroyAirframe -> boom()
      explosion(at, 14, 180, false);  // blast radius 14 m, 180 (also the army's and the look's)
      vehicle_.jet_hp = kJetHpMax;
      jet_.thrust_scale = 1;
      if (mode_ == Mode::jet) {
        // finish() 6 m ahead on foot, then __arondightPlayerDamageModel.damage(1000, "jet-destroyed")
        const BodyState s = state_of(jet_actor_);
        const Vec3 nose = s.rotation.rotate({0, 1, 0});
        const f32 h = std::atan2(nose.y, nose.x);
        Vec3 out = at + Vec3{std::cos(h), std::sin(h), 0} * 6.0f;
        out.z = city_.ground_height(out.x, out.y);
        jet_.pilot = false;
        jet_.engine_on = false;
        show_soldier(out, h / kDeg);
        mode_ = Mode::foot;
        world_->damage(soldier_, 1000, 0, false, at);
      }
      vehicle_.jet_placed = false;
      world_->set_alive(jet_actor_, false);
    }
  }
  g.smoke.update(now, dt);
  if (!combat_.zombies.active) g.zombie_doors.clear();  // dawn: the dead are gone
  gaps_lamps_update(dt);  // street lamps: shots, cars (game_lamps.cpp)
  // tow_service.mjs onTracer: a round from the player on foot that ends within 2.3 m of a tow truck
  // hurts it (MP 5, Glock 9, Sniper 70, anything else 8 of its 110 HP)
  if (life_) {
    for (const kern::game::ShotEvent& s : world_->shots()) {
      if (s.shooter != soldier_) continue;
      const f32 dmg = weapons_.foot_weapon == weapons::Id::smg ? 5.0f : weapons_.foot_weapon == weapons::Id::glock ? 9.0f : weapons_.foot_weapon == weapons::Id::sniper ? 70.0f : 8.0f;
      for (WorldLife::TowTruck& tt : life_->trucks) {
        if (!tt.body.valid() || !world_->physics().body_valid(tt.body)) continue;
        if (kern::length(world_->physics().pose(tt.body).position - s.to) < 2.3f) { tt.hp -= dmg; break; }
      }
    }
  }
  gaps_seats_update(dt);  // jet seats (game_seats.cpp)
  gaps_guided(dt);  // GUIDED smoke trail and motor (game_gaps.cpp)
  foot_rounds_update(dt);  // rocket smoke, shots through grenades, the knife's strike (game_foot_rounds.cpp)
  // ---- a person died: the bank's crack (world_organic_hit_audio.mjs, "arondight:world-kill")
  if (g.death_scan < 0) g.death_scan = now;
  for (const People::Person& p : people_.all()) {
    if (p.state != People::State::down || p.down_at <= g.death_scan || p.kind == People::Kind::zombie) continue;
    if (now - g.last_organic < gaps::organic_min_gap_s()) continue;
    g.last_organic = now;
    ++g.organic_plays;
    world_->play_sound(combat_sfx(combat_audio::Kind::crack), view_.cam.eye, gaps::organic_gain() * clampf(ui_.settings.get(Opt::audio_fx) / 100, 0, 1), gaps::organic_rate(gaps::Organic::person), false);
    osfx("world-kill", p.pos, 0);  // kind person
    // and the voice: a dying cry at the body, its pitch the person's (low, middle, high voices by
    // the seed, a little random on top), now and then a long one
    {
      const f32 voice = p.seed % 3 == 0 ? 0.78f : p.seed % 3 == 1 ? 0.92f : 1.12f;
      world_->play_sound(combat_sfx(combat_audio::Kind::scream), p.pos + Vec3{0, 0, 1.5f}, 0.42f * clampf(ui_.settings.get(Opt::audio_fx) / 100, 0, 1), voice * (0.94f + rng_.unit() * 0.12f), true);
    }
  }
  // a hit that does not kill: a short pain grunt (the voice's scream, faster and quieter), at most
  // one every 0.12 s in the whole street
  for (const People::Person& p : people_.all()) {
    if (p.state == People::State::down || p.state == People::State::gone || p.hurt_at <= g.death_scan || p.hurt_at > now) continue;
    if (p.kind == People::Kind::zombie || now - g.last_pain < 0.12f) continue;
    g.last_pain = now;
    const f32 voice = p.seed % 3 == 0 ? 0.85f : p.seed % 3 == 1 ? 1.0f : 1.2f;
    world_->play_sound(combat_sfx(combat_audio::Kind::scream), p.pos + Vec3{0, 0, 1.5f}, 0.26f * clampf(ui_.settings.get(Opt::audio_fx) / 100, 0, 1), voice * (1.35f + rng_.unit() * 0.25f), true);
  }
  g.death_scan = now;
  // ---- flight-fire impacts (impact_explosion_overlay.mjs): the drone MG's rounds that hit something
  if (mode_ == Mode::drone && vehicle_.drone_out) {
    const Vec3 nose = matrix_of(drone_actor_).translation();
    for (const kern::game::ShotEvent& s : world_->shots()) {
      if (!s.impact || kern::length(s.from - nose) > 1.5f) continue;
      if (g.bursts.size() >= gaps::kBurstPool) g.bursts.erase(g.bursts.begin());
      g.bursts.push_back({s.to, now, ++g.burst_serial});
    }
  }
  g.bursts.erase(std::remove_if(g.bursts.begin(), g.bursts.end(), [&](const GapsState::Burst& b) { return now - b.t > gaps::kBurstLifeS; }), g.bursts.end());
}

void Game::gaps_jet_damage(f32 amount) {
  // jet_mode.mjs damageAirframe: HP 400; thrust .35 + hp / 200 * .65 (.35 .. 1); at 15 % it burns
  // for 12 s, at 0 it breaks up at once.
  if (!(amount > 0) || !vehicle_.jet_placed) return;
  if (amount > 8 && mode_ == Mode::jet) osfx("jet_mode.knock", matrix_of(jet_actor_).translation());  // damageAirframe: sfx("bounce", .3, .6)
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  vehicle_.jet_hp = std::fmax(0.0f, std::fmin(vehicle_.jet_hp, kJetHpMax) - amount);
  jet_.thrust_scale = clampf(0.35f + vehicle_.jet_hp / (kJetHpMax * 0.5f) * 0.65f, 0.35f, 1);
  look_.shake.trauma = std::fmin(1.0f, look_.shake.trauma + std::fmin(0.6f, amount / 120));  // addTrauma
  if (vehicle_.jet_hp <= kJetHpMax * 0.15f && vehicle_.jet_hp > 0 && !g.jet_burning) {
    g.jet_burning = true;
    g.smoke.start("jet", matrix_of(jet_actor_).translation(), 2.2f, kJetBurnS, now);
    toast_ = "JET BRENNT";
    toast_age_ = 0;
  }
  if (vehicle_.jet_hp <= 0) {
    g.jet_burning = true;
    g.smoke.accelerate("jet", now, now);  // goes off on the next frame (at least 0.18 s)
    g.smoke.stop("jet");
  }
}

// ---------------------------------------------------------------------------------------------
// sound

void Game::gaps_sirens(f32 dt) {
  if (!gaps_ || !audio_.mixer.ready()) return;
  GapsState& g = *gaps_;
  const u32 rate = audio_.mixer.sample_rate();
  if (!g.horn) {
    // voice(ctx, "car"): sawtooth 0 ct .5, square +7 ct .22, triangle -5 ct .45 -> peaking 1650 Hz Q .9 +7 dB -> low-pass 4200
    g.horn = audio_.mixer.adopt_clip(siren_loop(rate, gaps::horn_low(), {{Osc::saw, 0, 0.5f}, {Osc::square, 7, 0.22f}, {Osc::triangle, -5, 0.45f}}, true, 5200), 1, "spawncity:horn", true);
    g.horn_dull = audio_.mixer.adopt_clip(siren_loop(rate, gaps::horn_low(), {{Osc::saw, 0, 0.5f}, {Osc::square, 7, 0.22f}, {Osc::triangle, -5, 0.45f}}, true, 900), 1, "spawncity:horn-far", true);
    // voice(ctx, "drone"): sine .8 + triangle +4 ct .35 -> low-pass 4200
    g.whoop = audio_.mixer.adopt_clip(siren_loop(rate, gaps::whoop_base(), {{Osc::sine, 0, 0.8f}, {Osc::triangle, 4, 0.35f}}, false, 5200), 1, "spawncity:whoop", true);
    g.whoop_dull = audio_.mixer.adopt_clip(siren_loop(rate, gaps::whoop_base(), {{Osc::sine, 0, 0.8f}, {Osc::triangle, 4, 0.35f}}, false, 900), 1, "spawncity:whoop-far", true);
  }
  g.clock += dt;
  const Vec3 ear = view_.cam.eye;
  // the nearest cruiser on a call (lights on: not wrecked, abandoned, leaving or driven by the player)
  f32 dc = -1;
  for (const PoliceUnit& u : police_.units) {
    if (u.car < 0 || u.state == PoliceUnit::State::abandoned || u.state == PoliceUnit::State::wreck || u.state == PoliceUnit::State::leave) continue;
    if (mode_ == Mode::car && car_index_ == u.car) continue;
    const f32 d = kern::length(matrix_of(cars_[u.car].actor).translation() + Vec3{0, 0, 0.9f} - ear);
    if (dc < 0 || d < dc) dc = d;
  }
  // the nearest police drone (active, not retreating, not knocked out by the EMP)
  f32 dd = -1;
  for (const PoliceDrone& d : police_.pdrones) {
    if (!d.active || d.retreating || d.emp) continue;
    const f32 l = kern::length(d.pos - ear);
    if (dd < 0 || l < dd) dd = l;
  }
  const bool silent = ui_.paused || !ui_.settings.on(Opt::audio_on);
  auto drive = [&](gaps::SirenVoice& v, f32 d, f32 base, f32& level, kern::audio::ClipId bright, kern::audio::ClipId dull, kern::audio::VoiceId& vb,
                   kern::audio::VoiceId& vd, f32 pitch_hz, f32 f0, f32 pitch_tau) {
    // level(): fxVolume / 72 is the effects bus volume; the gain is set relative to it
    f32 target = silent ? 0 : v.update(d, dt, base, 1);
    const f32 tau = target > level ? 0.35f : 0.25f;
    level += (target - level) * (1 - std::exp(-dt / tau));
    const bool on = level > 1e-4f;
    const f32 w = d >= 0 ? (gaps::siren_cutoff(d) - 900) / (5200 - 900) : 1;
    const f32 pitch = pitch_hz / f0 * v.doppler;
    (void)pitch_tau;
    for (auto [clip, voice, vol] : {std::tuple<kern::audio::ClipId, kern::audio::VoiceId*, f32>{bright, &vb, level * w}, {dull, &vd, level * (1 - w)}}) {
      if (!on) { if (*voice) { audio_.mixer.stop(*voice, 0.1f); *voice = 0; } continue; }
      if (!*voice || !audio_.mixer.playing(*voice)) {
        kern::audio::PlayParams p;
        p.loop = true;
        p.volume = vol;
        p.pitch = pitch;
        p.bus = kern::audio::Bus::sfx;
        *voice = audio_.mixer.play(clip, p);
      } else {
        audio_.mixer.set_volume(*voice, vol);
        audio_.mixer.set_pitch(*voice, clampf(pitch, 0.05f, 8));
      }
    }
  };
  drive(g.car, dc, gaps::car_gain(), g.car_level, g.horn, g.horn_dull, g.v_horn, g.v_horn_dull, gaps::horn_hz(g.clock), gaps::horn_low(), 0.014f);
  drive(g.drone, dd, gaps::drone_gain(), g.drone_level, g.whoop, g.whoop_dull, g.v_whoop, g.v_whoop_dull, gaps::whoop_hz(g.clock), gaps::whoop_base(), 0.03f);
}

namespace {
// fighter_jets.mjs ensureAudio(): 4 s of rumble (noise low-passed twice), roar, hiss and a turbine
// whine at 1180 Hz (vibrato 3 Hz) + 2360 Hz, loop cross-faded over 50 ms, normalised.
std::vector<f32> jet_roar_pcm(u32 rate, f32 cutoff) {
  const std::size_t n = static_cast<std::size_t>(rate) * 4;
  std::vector<f32> d(n);
  u32 s = 0x2545f491u;
  f32 lp = 0, lp2 = 0, br = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const f32 t = static_cast<f32>(i) / static_cast<f32>(rate);
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    const f32 w = static_cast<f32>(s) / 2147483648.0f - 1;
    lp += (w - lp) * 0.08f;
    lp2 += (lp - lp2) * 0.02f;
    br += (w - br) * 0.6f;
    const f32 whine = std::sin(2 * kPi * 1180 * t + std::sin(2 * kPi * 3 * t) * 2) * 0.05f + std::sin(2 * kPi * 2360 * t) * 0.025f;
    d[i] = lp2 * 2.4f + lp * 0.9f + (w - br) * 0.18f + whine;
  }
  const std::size_t f = static_cast<std::size_t>(rate * 0.05f);
  for (std::size_t i = 0; i < f; ++i) { const f32 a = static_cast<f32>(i) / static_cast<f32>(f); d[n - f + i] = d[n - f + i] * (1 - a) + d[i] * a; }
  Biquad lpf = Biquad::lowpass(cutoff, 0.7f, static_cast<f32>(rate));
  for (f32& v : d) v = lpf.run(v);
  kern::audio::normalize(d, 0.9f);
  return d;
}
}  // namespace

void Game::gaps_jet_audio(f32 dt) {
  (void)dt;
  if (!gaps_ || !audio_.mixer.ready() || !world_) return;
  GapsState& g = *gaps_;
  if (!g.jet_roar) {
    const u32 rate = audio_.mixer.sample_rate();
    g.jet_roar = audio_.mixer.adopt_clip(jet_roar_pcm(rate, 9000), 1, "spawncity:jetroar", true);
    g.jet_roar_dull = audio_.mixer.adopt_clip(jet_roar_pcm(rate, 900), 1, "spawncity:jetroar-far", true);
  }
  const f32 now = world_->sim_time();
  for (GapsState::JetVoice& v : g.jet_voices) v.alive = false;
  const Vec3 L = mode_ == Mode::foot ? player_position() : view_.cam.eye;  // listenerPos
  const bool silent = ui_.paused || !ui_.settings.on(Opt::audio_on);
  for (const FighterJet& j : combat_.jets) {
    GapsState::JetVoice* v = nullptr;
    for (GapsState::JetVoice& q : g.jet_voices)
      if (q.t0 == j.t0 && q.x == j.x && q.y0 == j.y0) { v = &q; break; }
    if (!v) {
      g.jet_voices.push_back({j.x, j.y0, j.z, j.t0, gaps::unit_hash(++g.jet_voice_count * 2654435761u), 0, 0, false});
      v = &g.jet_voices.back();
    }
    v->alive = true;
    // retarded time: the sound we hear now left the jet when |p(te) - L| = c (now - te)
    f32 te = now;
    Vec3 p;
    for (int i = 0; i < 4; ++i) { p = {j.x, j.y0 + combat::kJetSpeed * (te - j.t0), j.z}; te = now - kern::length(p - L) / combat::kSoundMps; }
    p = {j.x, j.y0 + combat::kJetSpeed * (te - j.t0), j.z};
    const f32 d = std::fmax(8.0f, kern::length(p - L));
    const Vec3 dir = kern::normalize(L - p, {0, 1, 0});
    const f32 vr = combat::kJetSpeed * dir.y, doppler = clampf(combat::kSoundMps / (combat::kSoundMps - vr), 0.45f, 2.6f);
    const bool started = te >= j.t0 - 1.5f;
    const f32 gain = silent || !started ? 0 : std::fmin(1.8f, std::pow(85 / d, 1.35f)) * 0.9f * 0.85f;
    const f32 cutoff = std::fmin(9000.0f, 900 + 60000 / d), w = clampf((cutoff - 900) / (9000 - 900), 0, 1);
    const f32 rate_k = doppler * (0.92f + v->pitch * 0.16f);
    for (auto [clip, voice, vol] : {std::tuple<kern::audio::ClipId, kern::audio::VoiceId*, f32>{g.jet_roar, &v->bright, gain * w}, {g.jet_roar_dull, &v->dull, gain * (1 - w)}}) {
      if (!*voice || !audio_.mixer.playing(*voice)) {
        kern::audio::PlayParams pp;
        pp.loop = true;
        pp.volume = vol;
        pp.pitch = rate_k;
        pp.bus = kern::audio::Bus::sfx;
        pp.start = v->pitch * 3;  // src.start(0, random * 3)
        *voice = audio_.mixer.play(clip, pp);
      } else {
        audio_.mixer.set_volume(*voice, vol, 0.05f);
        audio_.mixer.set_pitch(*voice, clampf(rate_k, 0.05f, 8));
      }
    }
  }
  // stopJetVoice: fade 0.3 s when the jet is gone
  for (GapsState::JetVoice& v : g.jet_voices)
    if (!v.alive) {
      if (v.bright) audio_.mixer.stop(v.bright, 0.3f);
      if (v.dull) audio_.mixer.stop(v.dull, 0.3f);
    }
  std::erase_if(g.jet_voices, [](const GapsState::JetVoice& v) { return !v.alive; });
}

// ---------------------------------------------------------------------------------------------
// camera (camera_collision_guard.mjs)

void Game::gaps_camera(kern::Camera& c) {
  if (gaps_ && gaps_->riding && gaps_ride_camera(c)) return;  // in a mate's jet (game_seats.cpp)
  if (!gaps_ || !world_placed_ || mode_ == Mode::jet || mode_ == Mode::parachute || mode_ == Mode::foot) return;  // the jet / chute camera is free in the sky
  bool collided = false;
  const f32 floor = city_.ground_height(c.eye.x, c.eye.y) + 0.12f;
  if (c.eye.z < floor) { c.target.z += floor - c.eye.z; c.eye.z = floor; collided = true; }
  gaps_->camera_lifted = false;
  if (mode_ == Mode::car && car_index_ >= 0 && !view_.car_cockpit) {
    // a wall right behind the car pulled the camera in: rise over the roof and look down at the car
    const Vec3 a = matrix_of(cars_[car_index_].actor).translation();
    const f32 h = std::hypot(c.eye.x - a.x, c.eye.y - a.y), min_h = 3.2f;
    const bool bus = cars_[car_index_].car->spec().radius > 0.4f;
    if (h < min_h * (bus ? 1.75f : 1.0f) * 0.999f || collided) {
      const f32 lift = a.z + 1.25f + (min_h - std::fmin(h, min_h)) * 0.75f;
      if (c.eye.z < lift) c.eye.z = lift;
      c.target = a + Vec3{0, 0, 0.15f};
      c.up = {0, 0, 1};
      gaps_->camera_lifted = true;
    }
    c.near = clampf(c.near, 0.045f, 0.08f);
  }
}

// ---------------------------------------------------------------------------------------------
// drawing, HUD, commands

kern::Mat4 Game::gaps_drone_matrix() {
  const Mat4 raw = matrix_of(drone_actor_);
  if (!gaps_ || mode_ != Mode::drone || view_.drone_cam == 0) { if (gaps_) gaps_->drone_visual.invalidate(); return raw; }
  // the external cameras draw the airframe on the camera anchor with a damped offset and rotation
  Vec3 p;
  Quat q;
  gaps_->drone_visual.update(raw.translation(), state_of(drone_actor_).rotation, view_.cam_anchor, view_.drone_cam == 2 ? 2 : 1, gaps_->frame_dt, p, q);
  return Mat4::trs(p, q, {1, 1, 1});
}

void Game::gaps_draw() {
  if (!gaps_ || !gaps_->renderer) return;
  gaps_barricades_draw();  // zombie barricades (game_barricades.cpp)
  GapsState& g = *gaps_;
  const f32 now = world_ ? world_->sim_time() : 0;
  const Vec3 f = kern::normalize(view_.cam.target - view_.cam.eye, {1, 0, 0});
  Vec3 right = kern::normalize(kern::cross(f, Vec3{0, 0, 1}), {1, 0, 0});
  const Vec3 up = kern::cross(right, f);
  for (GapsState::Part* p : {&g.disc, &g.ember}) { p->transforms.clear(); p->tints.clear(); }
  for (const gaps::Puff& p : g.smoke.puffs()) {
    if (!p.alive) continue;
    GapsState::Part& part = p.ember ? g.ember : g.disc;
    const Mat4 m = billboard(p.pos, right, up, f * -1.0f, gaps::SmokeField::size(p, now) * 0.5f, p.rotation);
    part.transforms.insert(part.transforms.end(), m.m, m.m + 16);
    part.tints.insert(part.tints.end(), {p.color.x, p.color.y, p.color.z, p.opacity});
  }
  g.trail_part.transforms.clear();
  g.trail_part.tints.clear();
  for (const GapsState::TrailPuff& tp : g.trail) {
    if (tp.born < 0) continue;
    const Mat4 m = billboard(tp.p, right, up, f * -1.0f, 0.8f, 0);
    g.trail_part.transforms.insert(g.trail_part.transforms.end(), m.m, m.m + 16);
    g.trail_part.tints.insert(g.trail_part.tints.end(), {1, 1, 1, 0.42f});
  }
  // updateRocketSmoke: scale .18 + 1.1 sqrt(t), opacity .7 (1 - t)^1.6 (gameplay_final_runtime_v2.mjs)
  g.rocket_part.transforms.clear();
  g.rocket_part.tints.clear();
  for (const GapsState::RocketPuff& r : g.rocket_smoke) {
    if (r.born < 0) continue;
    const f32 t = clampf((now - r.born) / r.life, 0, 1);
    const Mat4 m = billboard(r.p, right, up, f * -1.0f, (0.18f + 1.1f * std::sqrt(t)) * 0.5f, r.rot);
    g.rocket_part.transforms.insert(g.rocket_part.transforms.end(), m.m, m.m + 16);
    g.rocket_part.tints.insert(g.rocket_part.tints.end(), {1, 1, 1, 0.7f * std::pow(1 - t, 1.6f)});
  }
  for (GapsState::Part* p : {&g.disc, &g.ember, &g.trail_part, &g.rocket_part}) {
    if (!p->id) continue;
    if (p->transforms.empty()) { g.renderer->set_visible(p->id, false); continue; }
    g.renderer->update_instances(p->id, kern::Span<const f32>(p->transforms.data(), p->transforms.size()), kern::Span<const f32>(p->tints.data(), p->tints.size()));
    g.renderer->set_visible(p->id, true);
  }
}

void Game::gaps_hud(std::string& s) {
  if (!gaps_ || !world_) return;
  const GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  char b[400];
  // police dispatch (renderInboundHud): drones inbound and the next arrival
  u32 inbound = 0;
  f32 next = 1e9f;
  if (wanted::stars() > 0)
    for (u32 i = 0; i < 5; ++i)
      if (g.pd_due[i] > now && !police_.pdrones[i].active) { ++inbound; next = std::fmin(next, g.pd_due[i] - now); }
  std::snprintf(b, sizeof b, ",\"gaps\":{\"inbound\":%u,\"inboundS\":%.1f,\"sirenCar\":%.4f,\"sirenDrone\":%.4f,\"smoke\":%zu,\"puffs\":%u,\"organic\":%u,"
                "\"repair\":%s,\"spawnAllowed\":%u,\"spawnDenied\":%u,\"overlapDenied\":%u,\"camLifted\":%s,\"jetBurning\":%s,\"lamps\":%zu,\"lampsBroken\":%zu,\"lampsFallen\":%zu,\"bursts\":[",
                inbound, inbound ? next : 0.0f, g.car_level, g.drone_level, g.smoke.effects(), g.smoke.live_puffs(), g.organic_plays, gaps_repair_offer() ? "true" : "false", g.spawn_allowed, g.spawn_denied,
                g.overlap_denied, g.camera_lifted ? "true" : "false", g.jet_burning ? "true" : "false", g.lamps.all().size(), g.lamps.broken(), g.fallen.size());
  s += b;
  bool first = true;
  for (const GapsState::Burst& br : g.bursts) {
    f32 x, y, depth;
    if (!project_screen(br.p, x, y, depth)) continue;
    std::snprintf(b, sizeof b, "%s[%.4f,%.4f,%u]", first ? "" : ",", x, y, br.serial);
    s += b;
    first = false;
  }
  u32 placed = 0, far = 0;
  for (const TrafficCar& c : cars_) { placed += c.placed ? 1 : 0; far += c.far ? 1 : 0; }  // far traffic (game_traffic_far.cpp)
  std::snprintf(b, sizeof b, "],\"footShots\":%u,\"rocketSmoke\":%u,\"carsPlaced\":%u,\"farCars\":%u}", g.foot_shots,
                static_cast<u32>(std::count_if(g.rocket_smoke.begin(), g.rocket_smoke.end(), [](const GapsState::RocketPuff& r) { return r.born >= 0; })), placed, far);
  s += b;
}

bool Game::gaps_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (cmd == "gaps") {
    std::string s;
    gaps_hud(s);
    out = s.size() > 8 ? s.substr(8) : "{}";
    return true;
  }
  if (starts("canspawn ")) {
    // Tests: canspawn X Y -> would something appear there unseen? (world metres, on the ground)
    f32 x = 0, y = 0;
    std::sscanf(cmd.c_str() + 9, "%f %f", &x, &y);
    const f32 shown = gaps_->world_shown;
    gaps_->world_shown = -1;  // past the start grace
    const bool ok = gaps_can_spawn({x, y, city_.ground_height(x, y)}, 2.2f, 0);
    gaps_->world_shown = shown;
    out = ok ? "yes" : "no";
    return true;
  }
  if (starts("burn ")) {
    // Tests: burn N -> set traffic car N on fire (the critical countdown)
    const i32 i = std::atoi(cmd.c_str() + 5);
    if (i < 0 || i >= static_cast<i32>(cars_.size()) || !cars_[i].placed) { out = "no car"; return true; }
    damage_car(i, cars_[i].hp - 1, true);
    out = cars_[i].burn_until >= 0 ? "burning" : "not burning";
    return true;
  }
  if (cmd == "seats") {
    // Tests: the jet seat table and whether we ride
    std::string s = std::string("{\"riding\":") + (gaps_->riding ? "true" : "false") + ",\"pending\":" + (gaps_->seat_pending_until > 0 ? "true" : "false") + ",\"rows\":[";
    char b[64];
    bool first = true;
    for (const seats::Row& r : net_.seats().rows()) {
      std::snprintf(b, sizeof b, "%s[%u,%d,%d]", first ? "" : ",", r.owner, r.pilot == seats::kNone ? -1 : r.pilot, r.passenger == seats::kNone ? -1 : r.passenger);
      s += b;
      first = false;
    }
    out = s + "]}";
    return true;
  }
  if (cmd == "board?") {
    const Vec3 me = matrix_of(soldier_).translation();
    char b[400];
    std::string s = "[";
    for (const net::Remote& r : net_.remotes()) {
      std::snprintf(b, sizeof b, "{\"slot\":%u,\"mode\":%u,\"shown\":[%.1f,%.1f],\"me\":[%.1f,%.1f],\"speed\":%.2f,\"flags\":%u,\"pilot\":%d}", r.state.slot, static_cast<u32>(r.state.mode), r.shown.x, r.shown.y, me.x, me.y, r.state.speed, r.state.flags, net_.seats().get(r.state.slot).pilot);
      s += b;
    }
    out = s + "]";
    return true;
  }
  if (cmd == "board") { out = gaps_seat_press() ? "asked" : "nothing to board"; return true; }
  if (cmd == "repair") { out = gaps_repair_barricade() ? "repaired" : "nothing to repair"; return true; }
  if (cmd == "barricades") {
    std::vector<GapsState::DoorView> doors;
    std::string s = "{\"doors\":[";
    if (gaps_barricade_doors(doors))
      for (std::size_t i = 0; i < doors.size(); ++i) s += (i ? "," : "") + std::to_string(doors[i].planks);
    out = s + "],\"broken\":" + std::to_string(gaps_->planks_broken) + ",\"offer\":" + (gaps_repair_offer() ? "true" : "false") + "}";
    return true;
  }
  if (starts("lampbreak ")) {
    // Tests: lampbreak STATE -> break the lamp nearest the player (1 dark, 2 down)
    const u8 st = static_cast<u8>(std::atoi(cmd.c_str() + 10));
    const Vec3 me = player_position();
    i32 best = -1;
    f32 bd = 1e9f;
    for (std::size_t i = 0; i < gaps_->lamps.all().size(); ++i) {
      const f32 d = kern::length(gaps_->lamps.all()[i].foot - me);
      if (gaps_->lamps.all()[i].state < st && d < bd) { bd = d; best = static_cast<i32>(i); }
    }
    if (best < 0) { out = "no lamp"; return true; }
    gaps_lamp_break(best, st, {1, 0}, 6, true);
    char b[64];
    std::snprintf(b, sizeof b, "lamp %d at %.0f m state %u", best, bd, gaps_->lamps.all()[static_cast<std::size_t>(best)].state);
    out = b;
    return true;
  }
  if (cmd == "jet dive") {
    // Tests: the flown jet 40 m up, nose and speed straight down into the ground (crash damage)
    if (!vehicle_.jet_placed || mode_ != Mode::jet) { out = "not in the jet"; return true; }
    const Vec3 p = matrix_of(jet_actor_).translation();
    const Vec3 rot{-80, 0, 0};
    world_->teleport(jet_actor_, {p.x, p.y, city_.ground_height(p.x, p.y) + 40}, &rot);
    world_->set_actor_velocity(jet_actor_, {0, 0, -70});
    out = "diving";
    return true;
  }
  if (cmd == "jet hp") {
    char b[64];
    std::snprintf(b, sizeof b, "jet %.0f placed %d mode %s", vehicle_.jet_hp, vehicle_.jet_placed ? 1 : 0, mode_name(mode_));
    out = b;
    return true;
  }
  if (cmd == "jetdamage" || starts("jetdamage ")) {
    gaps_jet_damage(cmd.size() > 10 ? static_cast<f32>(std::atof(cmd.c_str() + 10)) : 50);
    char b[64];
    std::snprintf(b, sizeof b, "jet %.0f thrust %.2f", vehicle_.jet_hp, jet_.thrust_scale);
    out = b;
    return true;
  }
  return false;
}

}  // namespace sc
