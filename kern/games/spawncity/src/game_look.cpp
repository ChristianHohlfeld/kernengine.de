// SpawnCity on Kern: performance and the look. Frame times per part of the frame and the automatic
// quality level (perf.h), the camera's FPV optical stabilizer and trauma shake, speed streaks, the
// game's own day clock ("time"), the scene's point lights (street lamps, headlights, blue lights,
// muzzle flashes, rockets, blasts, jetpack) and first-person models for the game's own weapons.
// Only Kern renderer APIs are used (lighting, sky, point lights, shadow distance and filter, AO,
// meshes); the engine is not changed. Sources (nostack-dev/spawncity-de, sim/): camera_shake.mjs,
// fpv_optical_stabilizer.mjs, speed_lines.mjs, scene_lighting.mjs, dynamic_lights.mjs,
// day_night_sync.mjs, police_ground_units.mjs, wanted_police_drones.mjs, weapon_models.mjs,
// viewmodel_arms.mjs, neon_bloom.mjs (performance first).
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "game.h"
#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
using kern::MeshData;
namespace prim = kern::primitives;
using perf::kTier;

f64 clock_ms() { return kern::now_ms(); }

Vec3 srgb(u32 hex) { return {static_cast<f32>((hex >> 16) & 255) / 255, static_cast<f32>((hex >> 8) & 255) / 255, static_cast<f32>(hex & 255) / 255}; }

// Model builders in SpawnCity's view-model frame (weapon_models.mjs): x right, y up, barrel along -z.
void box(MeshData& m, Vec3 size, u32 color, Vec3 at, const Mat4& extra = Mat4{}) { prim::append(m, prim::box(size, srgb(color)), extra * Mat4::translate(at)); }
/// three's CylinderGeometry turned onto the z axis (rotation.x = pi/2), centred at `at`.
void cyl(MeshData& m, f32 r, f32 len, u32 color, Vec3 at, u32 seg = 14, const Mat4& extra = Mat4{}) {
  prim::append(m, prim::cylinder(r, len, seg, srgb(color)), extra * Mat4::translate({at.x, at.y, at.z - len * 0.5f}));
}
void glow_box(MeshData& m, Vec3 size, Vec3 hdr, Vec3 at) { prim::append(m, prim::box(size, hdr), Mat4::translate(at)); }
void glow_cyl(MeshData& m, f32 r, f32 len, Vec3 hdr, Vec3 at) { prim::append(m, prim::cylinder(r, len, 14, hdr), Mat4::translate({at.x, at.y, at.z - len * 0.5f})); }
// Glow colours: linear HDR (above 1 the bloom picks them up), like the MeshBasicMaterial glows.
constexpr Vec3 kGlowRed{3.2f, 0.12f, 0.12f}, kGlowAmber{3.0f, 1.25f, 0.1f}, kGlowOrange{3.0f, 0.9f, 0.18f};

/// A gloved forearm and sleeve reaching back from a grip at `grip` (viewmodel_arms.mjs).
void arm(MeshData& m, Vec3 grip, f32 side) {
  const Mat4 tilt = Mat4::translate(grip) * Mat4::trs({}, Quat::axis_angle({1, 0, 0}, -0.12f), {1, 1, 1});
  box(m, {0.075f, 0.08f, 0.1f}, 0x1d232c, {0, 0, 0.0f}, tilt);                 // glove
  cyl(m, 0.04f, 0.3f, 0xc58c66, {side * 0.012f, -0.03f, 0.2f}, 10, tilt);      // forearm
  cyl(m, 0.06f, 0.16f, 0x2c3440, {side * 0.02f, -0.05f, 0.39f}, 10, tilt);     // sleeve
  cyl(m, 0.063f, 0.03f, 0x1d232c, {side * 0.016f, -0.043f, 0.32f}, 10, tilt);  // cuff
}

/// BOOMSTICK 40 mm (weapon_models.mjs buildBoomstick): drum launcher with hazard stripes.
void boomstick(MeshData& m, MeshData& g) {
  box(m, {0.17f, 0.125f, 0.32f}, 0x4a5a3a, {0, 0.012f, -0.1f});     // body (profile -0.06 .. 0.26)
  box(m, {0.1f, 0.2f, 0.1f}, 0x8a5a3b, {0, -0.155f, -0.095f});      // wooden grip
  box(m, {0.13f, 0.12f, 0.24f}, 0x1d2024, {0, -0.02f, 0.18f});      // stock
  cyl(m, 0.11f, 0.2f, 0x4a5a3a, {0, -0.005f, -0.36f}, 10);          // drum
  for (int i = 0; i < 4; ++i) cyl(m, 0.113f, 0.03f, i % 2 ? 0x121316 : 0xffd23f, {0, -0.005f, -0.29f - i * 0.045f}, 10);
  for (int i = 0; i < 6; ++i) {
    const f32 a = static_cast<f32>(i) / 6 * 2 * kPi;
    cyl(m, 0.026f, 0.205f, 0x1d2024, {std::cos(a) * 0.07f, -0.005f + std::sin(a) * 0.07f, -0.36f}, 8);
  }
  cyl(m, 0.058f, 0.36f, 0x1d2024, {0, 0.03f, -0.62f});              // fat barrel
  cyl(m, 0.068f, 0.06f, 0xff7a1a, {0, 0.03f, -0.81f});              // orange muzzle
  box(m, {0.05f, 0.03f, 0.26f}, 0x121316, {0, 0.13f, -0.2f});        // top handle
  box(m, {0.04f, 0.06f, 0.03f}, 0x121316, {0, 0.1f, -0.08f});
  box(m, {0.04f, 0.06f, 0.03f}, 0x121316, {0, 0.1f, -0.32f});
  glow_cyl(g, 0.064f, 0.012f, kGlowOrange, {0, 0.03f, -0.842f});    // glowing muzzle ring
  glow_box(g, {0.02f, 0.04f, 0.02f}, kGlowOrange, {0, 0.16f, -0.3f});
  arm(m, {0, -0.15f, -0.095f}, 1);
}

/// ROCKET LAUNCHER (buildRocketLauncher): gunmetal tube, open bore, guidance box with a red lens.
void rocket_launcher(MeshData& m, MeshData& g) {
  cyl(m, 0.068f, 0.96f, 0x2b3038, {0, 0.03f, -0.35f}, 18);          // main tube
  cyl(m, 0.084f, 0.11f, 0x121418, {0, 0.03f, -0.81f}, 18);          // muzzle shroud
  cyl(m, 0.058f, 0.012f, 0x030303, {0, 0.03f, -0.867f}, 16);        // open bore
  for (f32 z : {-0.05f, -0.3f, -0.62f}) cyl(m, 0.072f, 0.022f, 0x8a9199, {0, 0.03f, z}, 18);  // rings
  cyl(m, 0.098f, 0.15f, 0x121418, {0, 0.03f, 0.18f}, 18);           // rear venturi
  cyl(m, 0.086f, 0.012f, 0x030303, {0, 0.03f, 0.258f}, 16);         // rear bore
  box(m, {0.05f, 0.03f, 0.5f}, 0x121418, {0, 0.105f, -0.36f});      // top rail
  box(m, {0.06f, 0.17f, 0.08f}, 0x1b1d20, {0, -0.115f, -0.14f});    // pistol grip
  box(m, {0.055f, 0.12f, 0.07f}, 0x1b1d20, {0, -0.09f, -0.475f});   // fore grip
  box(m, {0.012f, 0.05f, 0.11f}, 0x121418, {0, -0.025f, -0.17f});   // trigger guard
  box(m, {0.06f, 0.075f, 0.24f}, 0x2b3038, {-0.095f, 0.06f, -0.46f});  // guidance unit
  box(m, {0.07f, 0.012f, 0.25f}, 0x121418, {-0.095f, 0.104f, -0.46f});
  cyl(m, 0.022f, 0.02f, 0x8a9199, {-0.095f, 0.06f, -0.585f});
  glow_cyl(g, 0.015f, 0.012f, kGlowRed, {-0.095f, 0.06f, -0.597f});  // laser lens
  glow_box(g, {0.014f, 0.014f, 0.014f}, kGlowAmber, {-0.128f, 0.08f, -0.38f});
  glow_box(g, {0.014f, 0.014f, 0.014f}, kGlowRed, {-0.128f, 0.08f, -0.355f});
  arm(m, {0, -0.14f, -0.145f}, 1);
}

/// FISTS (buildFists): one taped fist with forearm and sleeve; the left one is its mirror.
void fist(MeshData& m) {
  box(m, {0.076f, 0.078f, 0.1f}, 0xc58c66, {0, 0.002f, -0.004f});    // fist body
  box(m, {0.09f, 0.05f, 0.04f}, 0xc58c66, {0, 0.012f, -0.045f});     // knuckle ridge
  cyl(m, 0.0405f, 0.018f, 0xe9e4d8, {0, 0.002f, 0.016f}, 10);         // tape band
  box(m, {0.03f, 0.03f, 0.06f}, 0xa9714f, {-0.022f, -0.03f, -0.03f}); // thumb
  cyl(m, 0.038f, 0.05f, 0xe9e4d8, {0, -0.002f, 0.066f}, 10);          // wrist wrap
  const Mat4 back = Mat4::trs({}, Quat::axis_angle({1, 0, 0}, -0.12f), {1, 1, 1});
  cyl(m, 0.043f, 0.3f, 0xc58c66, {0.012f, -0.03f, 0.24f}, 10, back);  // forearm
  cyl(m, 0.06f, 0.16f, 0x2c3440, {0.02f, -0.05f, 0.43f}, 10, back);   // sleeve
  cyl(m, 0.062f, 0.03f, 0x1d232c, {0.016f, -0.043f, 0.36f}, 10, back);  // cuff
}

/// KNIFE (buildKnife): clip-point blade with fuller and guard in the right fist, blade forward-up.
void knife(MeshData& m, MeshData& g) {
  fist(m);
  const f32 s = 1.45f * 1.08f;  // FIST_HAND_SCALE x 1.08
  const Mat4 grip = Mat4::trs({0, 0.01f, -0.02f}, Quat::axis_angle({1, 0, 0}, 0.32f) * Quat::axis_angle({0, 1, 0}, 0.18f) * Quat::axis_angle({0, 0, 1}, -0.5f), {s, s, s});
  box(m, {0.0054f, 0.032f, 0.2f}, 0x6e757b, {0, 0.002f, -0.15f}, grip);   // blade
  box(m, {0.006f, 0.006f, 0.17f}, 0xd8dde2, {0, -0.014f, -0.14f}, grip);  // bright edge bevel
  box(m, {0.0062f, 0.006f, 0.1f}, 0x17191c, {0, 0.006f, -0.105f}, grip);  // fuller
  box(m, {0.016f, 0.05f, 0.012f}, 0x17191c, {0, 0, -0.046f}, grip);       // cross guard
  cyl(m, 0.0135f, 0.095f, 0x1a1c1e, {0, 0, 0}, 10, grip);                  // ribbed handle
  cyl(m, 0.014f, 0.0125f, 0x2a2d31, {0, 0, 0.054f}, 10, grip);             // pommel
  (void)g;
}

kern::Material vm_material(bool glow) {
  kern::Material mt;
  mt.viewmodel = true;
  mt.cast_shadow = false;
  mt.frustum_culled = false;
  mt.roughness = glow ? 1 : 0.55f;
  mt.metalness = glow ? 0 : 0.25f;
  mt.unlit = glow;
  mt.linear_colors = glow;
  mt.double_sided = true;  // mirrored left fist
  return mt;
}

int vm_index(weapons::Id id) {
  switch (id) {
    case weapons::Id::grenade_launcher: return 0;
    case weapons::Id::rocket: return 1;
    case weapons::Id::fists: return 2;
    case weapons::Id::knife: return 3;
    default: return -1;
  }
}
const char* kVmNames[4] = {"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};

/// Camera space (x right, y up, z back) -> world.
Mat4 camera_frame(const kern::Camera& c) {
  const Vec3 f = kern::normalize(c.target - c.eye, {1, 0, 0});
  const Vec3 r = kern::normalize(kern::cross(f, c.up), {0, -1, 0});
  const Vec3 u = kern::cross(r, f);
  Mat4 m;
  m.m[0] = r.x; m.m[1] = r.y; m.m[2] = r.z; m.m[3] = 0;
  m.m[4] = u.x; m.m[5] = u.y; m.m[6] = u.z; m.m[7] = 0;
  m.m[8] = -f.x; m.m[9] = -f.y; m.m[10] = -f.z; m.m[11] = 0;
  m.m[12] = c.eye.x; m.m[13] = c.eye.y; m.m[14] = c.eye.z; m.m[15] = 1;
  return m;
}

Mat4 euler(f32 x, f32 y, f32 z) {
  return Mat4::trs({}, Quat::axis_angle({1, 0, 0}, x) * Quat::axis_angle({0, 1, 0}, y) * Quat::axis_angle({0, 0, 1}, z), {1, 1, 1});
}
}  // namespace

// ---------------------------------------------------------------------------------------------
// performance

void Game::look_begin(kern::Engine& e, const kern::Frame& f) {
  LookState& L = look_;
  L.meter.begin_frame(clock_ms());
  if (!L.started) {
    L.started = true;
    L.gov.reset(mobile_ ? perf::kMobileStart : perf::kDesktopStart, mobile_ ? perf::kMobileMax : perf::kTiers - 1);
    // ?quality=0..4 fixes a level (tests, slow machines); the setting can be changed with "quality".
    if (e.arg("quality")) {
      const f64 q = e.arg_number("quality", -1);
      if (q >= 0 && q < perf::kTiers) L.gov.set_fixed(static_cast<i32>(q));
    }
    look_apply_tier();
  }
  const bool live = world_placed_ && !ui_.paused;
  if (live) {
    // The day: SpawnCity's 480 s cycle on the game's own clock.
    L.tod += f.dt / look::kDayLength;
    L.tod -= std::floor(L.tod);
    if (L.gov.update(f.dt)) look_apply_tier();
  }
  // day_night_sync.mjs: a client follows the host's phase (the host sends it with its pose).
  L.sync_clock += f.dt;
  if (net_.active() && !net_.hosting() && L.sync_clock > 1.0f) {
    L.sync_clock = 0;
    for (const net::Remote& r : net_.remotes())
      if (r.state.slot == 255 && r.state.day >= 0) L.tod = look::follow_phase(L.tod, r.state.day);
  }
  if (ui_.applied != L.applied_settings) {  // WORLD people on / off changes the budget
    L.applied_settings = ui_.applied;
    people_.set_budget(look_people_budget());
  }
}

void Game::look_lap(perf::Slot s) { look_.meter.lap(s, clock_ms()); }

i32 Game::look_people_budget() const { return ui_.settings.on(Opt::world_people) ? kTier[look_.gov.tier()].people : 0; }

void Game::look_apply_tier() {
  LookState& L = look_;
  const i32 t = L.gov.tier();
  const perf::Tier& q = kTier[t];
  people_.set_budget(look_people_budget());
  L.lamp_m = q.lamp_m;
  L.people_m = q.people_m;
  if (L.pool_size != static_cast<u32>(q.lights)) {
    L.pool_size = static_cast<u32>(q.lights);
    L.lights = kern::LightPool(L.pool_size, 96);
  }
  if (engine_) {
    kern::Renderer& r = engine_->renderer;
    r.set_shadow_filter(q.shadow_filter);
    kern::AoOptions ao;
    ao.enabled = q.ao > 0;
    ao.quality = q.ao >= 2 ? kern::AoQuality::medium : kern::AoQuality::low;
    ao.half_resolution = q.ao < 3;
    r.set_ambient_occlusion(ao);
  }
  if (L.applied_tier >= 0 && L.applied_tier != t && L.gov.automatic()) {
    toast_ = std::string("Grafik automatisch: ") + q.name;
    toast_age_ = 0;
  }
  L.applied_tier = t;
}

// ---------------------------------------------------------------------------------------------
// camera

void Game::look_camera(f32 dt) {
  LookState& L = look_;
  if (static_cast<i32>(mode_) != L.last_mode) {
    L.last_mode = static_cast<i32>(mode_);
    L.stab.reset();
    L.vel_valid = false;
  }
  if (!world_placed_) return;
  // fpv_optical_stabilizer.mjs: only the FPV view (rigid on the frame) is filtered.
  if (mode_ == Mode::drone && drone_cam_ == 0) L.stab.apply(cam_, dt);
  else L.stab.reset();
  // Hard impacts of the car / drone the player controls (camera_shake.mjs onImpact).
  if ((mode_ == Mode::car && car_index_ >= 0) || mode_ == Mode::drone) {
    const Vec3 v = world_->actor_velocity(mode_ == Mode::car ? cars_[car_index_].actor : drone_actor_);
    if (L.vel_valid && !ui_.paused) L.shake.add(look::impact_trauma(kern::length(v - L.last_vel)));
    L.last_vel = v;
    L.vel_valid = true;
  } else L.vel_valid = false;
  // One shake model for the vehicles; on foot Kern's soldier camera has its own (shake_camera).
  Vec3 angles;
  Vec2 offset;
  if (L.shake.sample(dt, time_, mode_ == Mode::drone ? 0.85f : 1.0f, angles, offset) && mode_ != Mode::foot) look::shake_camera(cam_, angles, offset);
}

void Game::look_blast(Vec3 at, f32 radius) {
  LookState& L = look_;
  // scene_lighting.mjs onExplosion: bright, then a quick fade (420 ms), reach 4 x radius.
  const f32 r = std::fmax(6.0f, radius);
  if (L.blasts.size() >= 4) L.blasts.erase(L.blasts.begin());
  L.blasts.push_back({at + Vec3{0, 0, 1.5f}, static_cast<f32>(time_), 0.42f, 220 + r * 18, r * 4});
  if (mode_ != Mode::foot) L.shake.add(look::blast_trauma(kern::length(cam_.eye - at), radius));
}

// ---------------------------------------------------------------------------------------------
// day, night and point lights

void Game::look_lighting(f32 dt) {
  LookState& L = look_;
  kern::Renderer& r = engine_->renderer;
  const kern::game::WorldSettings& w = world_->level().world;
  // The game's day clock instead of GameWorld's (same sun path and colours; "time" can set it).
  night_ = look::day_lighting(L.tod, w.sun_azimuth, w.fog_density, w.exposure, r.lighting, r.sky);
  windows_.night = night_;
  r.set_shadow_distance(kTier[L.gov.tier()].shadow_m);
  const f32 now = static_cast<f32>(time_);
  const Vec3 eye = cam_.eye;
  auto req = [&](Vec3 p, u32 hex, f32 three_intensity, f32 range, f32 d_ref) {
    const f32 k = look::kern_intensity(three_intensity, range, d_ref);
    if (k > 0) L.lights.request(p, look::hex_linear(hex), k, range);
  };
  // Kern's own lights (light components, the soldier's muzzle flash) stay in the pool.
  {
    const kern::PointLights& pl = r.point_lights();
    for (u32 i = 0; i < pl.count; ++i)
      L.lights.request({pl.pos_range[i * 4], pl.pos_range[i * 4 + 1], pl.pos_range[i * 4 + 2]}, {pl.color_intensity[i * 4], pl.color_intensity[i * 4 + 1], pl.color_intensity[i * 4 + 2]},
                       pl.color_intensity[i * 4 + 3], pl.pos_range[i * 4 + 3]);
  }
  if (world_placed_) {
    // Shots of this frame (before present() takes them): tracers glint, the player's guns flash.
    const Vec3 me = focus_position();
    for (const kern::game::ShotEvent& s : world_->shots()) {
      if (kern::length(s.from - eye) > 60) continue;
      if (kern::length(s.to - s.from) >= 2) {
        if (L.glints.size() >= 3) L.glints.erase(L.glints.begin());
        L.glints.push_back({s.from, s.to, now});
      }
      if ((mode_ == Mode::drone || mode_ == Mode::jet) && kern::length(s.from - me) < 14) {
        L.flash_until = now + 0.055f;
        f32 cap = 1;
        const f32 t = look::weapon_trauma(true, false, mode_ == Mode::drone, cap);
        L.shake.add(t, cap);
      }
    }
    // Launches of this frame: rockets / grenades punch the camera.
    const f32 sim = world_->sim_time();
    for (const Projectile& p : projectiles_)
      if (p.by_player && p.born > sim - dt - 1e-3f && kern::length(p.pos - me) < 16) {
        const bool heavy = p.id == weapons::Id::rocket || p.id == weapons::Id::drone_rockets || p.id == weapons::Id::drone_guided || p.id == weapons::Id::jet_missile;
        f32 cap = 1;
        L.shake.add(look::weapon_trauma(false, heavy, mode_ == Mode::drone, cap), cap);
        if (mode_ == Mode::foot) world_->shake_camera(heavy ? 0.1f : 0.06f);
        L.flash_until = now + 0.055f;
      }
    const f32 glow = look::lamp_glow(night_);
    // Street lamps: the nearest three within 70 m, re-picked every 350 ms (scene_lighting.mjs).
    if (glow > 0.01f && show_lamps_) {
      if (now - L.lamp_pick_at > 0.35f || now < L.lamp_pick_at) {
        L.lamp_pick_at = now;
        std::vector<std::pair<f32, Vec3>> near;
        const std::vector<Lane>& lanes = city_.lanes();
        for (const Lamp& l : lamps_) {
          if (l.lane >= lanes.size() || !lanes[l.lane].alive) continue;
          const Vec3 head = l.foot + Vec3{std::cos(l.yaw) * 1.2f, std::sin(l.yaw) * 1.2f, 5.75f};
          const f32 d = std::hypot(head.x - eye.x, head.y - eye.y);
          if (d <= 70) near.push_back({d, head});
        }
        std::sort(near.begin(), near.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        L.near_lamps.clear();
        for (std::size_t i = 0; i < near.size() && i < 3; ++i) L.near_lamps.push_back(near[i].second);
      }
      for (const Vec3& h : L.near_lamps) req(h, 0xffb35a, 46 * glow, 26, 5.75f);
    }
    // Headlights: a pool of light ~7 m ahead of the car the player drives.
    if (glow > 0.05f && mode_ == Mode::car && car_index_ >= 0) {
      const kern::Mat4 m = matrix_of(cars_[car_index_].actor);
      const Vec3 fwd = kern::normalize(Vec3{m.m[0], m.m[1], 0}, {1, 0, 0});
      req(m.translation() + fwd * 7.0f + Vec3{0, 0, 0.9f}, 0xfff2d8, 60 * glow, 22, 2.5f);
    }
    // Muzzle flash in front of the eye (55 ms per shot).
    if (now < L.flash_until) {
      const Vec3 d = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
      req(eye + d * 0.9f - Vec3{0, 0, 0.1f}, 0xffc272, 22, 13, 1.5f);
    }
    // Blasts: bright, then a quick fade.
    for (std::size_t i = L.blasts.size(); i-- > 0;) {
      const LookState::Blast& b = L.blasts[i];
      const f32 t = (now - b.at) / b.ms;
      if (t >= 1 || t < 0) { L.blasts.erase(L.blasts.begin() + static_cast<std::ptrdiff_t>(i)); continue; }
      req(b.p, 0xffa04a, b.peak * (1 - t) * (1 - t), b.r, b.r * 0.25f);
    }
    // Rockets in flight light what they pass (the nearest two within 90 m).
    {
      int k = 0;
      for (const Projectile& p : projectiles_) {
        if (k >= 2) break;
        if (!p.alive || p.gravity || std::hypot(p.pos.x - eye.x, p.pos.y - eye.y) > 90) continue;
        req(p.pos, 0xff9a40, 16, 10, 2.5f);
        ++k;
      }
    }
    // Tracers: a travelling glint along the round's path (45 ms).
    for (std::size_t i = L.glints.size(); i-- > 0;) {
      const LookState::Glint& g = L.glints[i];
      const f32 u = (now - g.at) / 0.045f;
      if (u >= 1 || u < 0) { L.glints.erase(L.glints.begin() + static_cast<std::ptrdiff_t>(i)); continue; }
      req(g.a + (g.b - g.a) * u, 0xffd27a, 7 * (1 - u), 6, 1.5f);
    }
    // Jetpack flame under the player.
    if (ui_.jetpack.thrusting && mode_ == Mode::foot) req(player_position() - Vec3{0, 0, 1.6f}, 0xff8a2a, 14, 9, 2);
    // Police light bars (police_ground_units.mjs flashLights: 14, 18 m, 2 m above the car).
    for (const TrafficCar& c : cars_) {
      if (!c.police || !c.placed || c.wreck || c.unit < 0 || c.unit >= static_cast<i32>(units_.size()) || !world_->actor_alive(c.actor)) continue;
      const PoliceUnit::State st = units_[c.unit].state;
      if (st == PoliceUnit::State::abandoned || st == PoliceUnit::State::wreck || st == PoliceUnit::State::leave) continue;
      const int fl = look::police_flash(now);
      if (fl) req(matrix_of(c.actor).translation() + Vec3{0, 0, 2}, fl == 1 ? 0xff2030 : 0x2a6bff, 14, 18, 3);
    }
    // Police drones (wanted_police_drones.mjs: 24, 28 m).
    for (u32 i = 0; i < 5; ++i) {
      const PoliceDrone& d = pdrones_[i];
      if (!d.active || d.emp) continue;
      const int fl = look::drone_flash(now, static_cast<int>(i));
      if (fl) req(d.pos, fl == 1 ? 0xff1834 : 0x1688ff, 24, 28, 4);
    }
  }
  const kern::PointLights& out = L.lights.resolve(eye);
  L.lights_used = out.count;
  r.set_point_lights(out);
}

// ---------------------------------------------------------------------------------------------
// drawing: speed streaks, weapon models, the frame's end

void Game::look_draw(kern::Engine& e, f32 dt) {
  LookState& L = look_;
  kern::Renderer& r = e.renderer;
  // speed_lines.mjs: radial streaks on a plane 0.6 m in front of the eye.
  if (!L.streak_mesh) {
    L.streak_mesh = r.allocate_id();
    const f32 pos[] = {0, -0.5f, 0, 1, -0.5f, 0, 1, 0.5f, 0, 0, -0.5f, 0, 1, 0.5f, 0, 0, 0.5f, 0};
    const f32 nrm[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};
    kern::MeshInput in;
    in.positions = {pos, 18};
    in.normals = {nrm, 18};
    in.material.unlit = true;
    in.material.alpha = kern::AlphaMode::blend;
    in.material.cast_shadow = false;
    in.material.frustum_culled = false;
    in.material.double_sided = true;
    in.visible = false;
    if (!r.upsert(L.streak_mesh, in)) { r.release_id(L.streak_mesh); L.streak_mesh = 0; }
  }
  const f32 k = world_placed_ && !ui_.paused ? L.lines.update(cam_.eye, dt) : 0;
  if (L.streak_mesh) {
    if (k <= 0) r.set_visible(L.streak_mesh, false);
    else {
      L.streak_t.clear();
      L.streak_tint.clear();
      const f32 D = std::fmax(0.3f, cam_.near * 4);
      const f32 hy = D * std::tan(cam_.fov * 0.5f), hx = hy * e.aspect(), R = std::hypot(hx, hy);
      const f32 px = 2 * hy / std::fmax(60.0f, frame_h_ * 0.5f);  // one pixel of the half-resolution canvas
      const Vec3 f = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
      const Vec3 rt = kern::normalize(kern::cross(f, cam_.up), {0, -1, 0});
      const Vec3 up = kern::cross(rt, f);
      for (const look::SpeedLines::Streak& s : L.lines.s) {
        const f32 r0 = s.r * R, r1 = (s.r + s.len * (0.5f + k)) * R, ca = std::cos(s.a), sa = std::sin(s.a);
        const f32 alpha = k * std::fmin(1.0f, s.r * 1.6f) * 0.55f, width = (0.8f + 1.6f * k * s.r) * px;
        const Vec3 dir = rt * ca + up * sa, side = up * ca - rt * sa;
        const Vec3 o = cam_.eye + f * D + dir * r0, x = dir * (r1 - r0), y = side * width, z = f * -1.0f;
        const f32 m[16] = {x.x, x.y, x.z, 0, y.x, y.y, y.z, 0, z.x, z.y, z.z, 0, o.x, o.y, o.z, 1};
        L.streak_t.insert(L.streak_t.end(), m, m + 16);
        const Vec3 c = s.warm ? Vec3{1.0f, 0.925f, 0.784f} : Vec3{1, 1, 1};
        L.streak_tint.insert(L.streak_tint.end(), {c.x, c.y, c.z, alpha});
      }
      r.update_instances(L.streak_mesh, {L.streak_t.data(), L.streak_t.size()}, {L.streak_tint.data(), L.streak_tint.size()});
      r.set_visible(L.streak_mesh, true);
    }
  }
  look_view_models(dt);
  look_lap(perf::Slot::draw);
  const kern::RenderStats& rs = e.stats();
  L.meter.end_frame(clock_ms(), static_cast<f32>(rs.cpu_ms), static_cast<f32>(rs.gpu_ms));
}

void Game::look_view_models(f32 dt) {
  LookState& L = look_;
  kern::Renderer& r = engine_->renderer;
  if (!L.vm_built) {
    L.vm_built = true;
    for (int i = 0; i < 4; ++i) {
      MeshData body, glow;
      switch (i) {
        case 0: boomstick(body, glow); break;
        case 1: rocket_launcher(body, glow); break;
        case 2: fist(body); break;
        default: knife(body, glow); break;
      }
      auto make = [&](const MeshData& d, bool g, u32& id) {
        if (d.positions.empty()) return;
        id = r.allocate_id();
        kern::MeshInput in = d.as_input(kern::kMaxExtensionAttributes);
        in.material = vm_material(g);
        in.visible = false;
        if (!r.upsert(id, in)) { r.release_id(id); id = 0; }
      };
      make(body, false, L.vm_body[i]);
      make(glow, true, L.vm_glow[i]);
    }
  }
  const int want = world_placed_ && mode_ == Mode::foot && !player_dead_ && world_->hud().health > 0 ? vm_index(foot_weapon_) : -1;
  const f32 now = world_->sim_time();
  // Attacks: the game's weapons move next_attack_ forward when they fire / strike.
  const i32 wid = static_cast<i32>(foot_weapon_);
  if (wid != L.vm_prev_weapon) { L.vm_prev_weapon = wid; L.vm_switch_at = now; }
  else if (next_attack_ > L.vm_prev_next_attack + 1e-4f && now - L.vm_switch_at > 0.05f) { L.vm_fire_at = now; L.vm_punch ^= 1; }
  L.vm_prev_next_attack = next_attack_;
  for (int i = 0; i < 4; ++i)
    if (i != want) {
      if (L.vm_body[i]) r.set_visible(L.vm_body[i], false);
      if (L.vm_glow[i]) r.set_visible(L.vm_glow[i], false);
    }
  L.vm_shown = want;
  if (want < 0) return;
  // Kern draws its own first-person weapon for the soldier's kind; while ours is in hand it hides.
  for (kern::Mesh* m : r.meshes()) {
    if (!m->visible || !m->material.viewmodel) continue;
    bool mine = m->id == L.streak_mesh;
    for (int i = 0; i < 4; ++i) mine = mine || m->id == L.vm_body[i] || m->id == L.vm_glow[i];
    if (!mine) r.set_visible(m->id, false);
  }
  // Walk bob from the soldier's speed, raise after a weapon change, kick / punch / slash after an attack.
  const Vec3 v = world_->actor_velocity(soldier_);
  const f32 sp = std::hypot(v.x, v.y);
  L.vm_bob += dt * sp * 1.6f;
  L.vm_bob_w += (std::fmin(1.0f, sp / 5.5f) - L.vm_bob_w) * damp_alpha(8, dt);
  const f32 bx = std::sin(L.vm_bob) * 0.014f * L.vm_bob_w, bz = -std::cos(2 * L.vm_bob) * 0.017f * L.vm_bob_w;
  const f32 raise = clampf((now - L.vm_switch_at) / 0.25f, 0, 1), drop = (1 - raise) * (1 - raise) * 0.25f;
  const f32 age = now - L.vm_fire_at;
  const Mat4 cam = camera_frame(cam_);
  std::vector<f32> t;
  auto put = [&](const Mat4& m) { t.insert(t.end(), m.m, m.m + 16); };
  switch (want) {
    case 0:
    case 1: {
      // Launchers: held right of the view (Kern's rest pose .245 / -.225 / -.445), kick back and up.
      const f32 kick = age >= 0 && age < 0.22f ? std::sin(age / 0.22f * kPi) : 0;
      put(cam * Mat4::translate({0.245f + bx * 0.45f, -0.2f + bz * 0.2f - drop, -0.47f + kick * 0.08f}) * euler(-0.03f + kick * 0.12f, -0.06f, -0.018f));
      break;
    }
    case 2: {
      // Fists: a low guard; each strike jabs the other hand forward (buildFists, 300 ms).
      const f32 jab = age >= 0 && age < 0.3f ? std::sin(age / 0.3f * kPi) : 0;
      for (int side = -1; side <= 1; side += 2) {
        const f32 j = (side > 0) == (L.vm_punch == 1) ? jab : 0;
        const Mat4 hand = cam * Mat4::translate({side * 0.17f + bx * 0.4f, -0.22f + bz * 0.2f - drop + j * 0.05f, -0.42f - j * 0.24f}) * euler(0.1f, -side * 0.12f * j, 0);
        put(side < 0 ? hand * Mat4::scale({-1, 1, 1}) : hand);
      }
      break;
    }
    default: {
      // Knife: low right, blade forward; a slash sweeps it across the view (380 ms).
      const f32 u = age >= 0 && age < 0.38f ? age / 0.38f : 1;
      const f32 sl = u < 1 ? std::sin(u * kPi) : 0;
      put(cam * Mat4::translate({0.2f - sl * 0.26f + bx * 0.4f, -0.21f + bz * 0.2f - drop + sl * 0.06f, -0.4f - sl * 0.12f}) * euler(-0.2f * sl, 0.9f * sl, 0.5f * sl));
      break;
    }
  }
  for (u32 id : {L.vm_body[want], L.vm_glow[want]}) {
    if (!id) continue;
    r.update_instances(id, {t.data(), t.size()}, {});
    r.set_visible(id, true);
  }
}

// ---------------------------------------------------------------------------------------------
// HUD and commands

std::string Game::look_hud_json() {
  const LookState& L = look_;
  const perf::Tier& q = kTier[L.gov.tier()];
  const f32 hours = L.tod * 24;
  const int hh = static_cast<int>(hours) % 24, mm = static_cast<int>((hours - std::floor(hours)) * 60);
  char b[720];
  std::snprintf(b, sizeof b,
                ",\"perf\":{\"fps\":%.1f,\"tier\":%d,\"tierName\":\"%s\",\"auto\":%s,\"show\":%s,\"changes\":%u,\"lights\":%u,\"ms\":{\"logic\":%.2f,\"physics\":%.2f,"
                "\"stream\":%.2f,\"draw\":%.2f,\"update\":%.2f,\"render\":%.2f,\"gpu\":%.2f}},\"renderScale\":%.2f,\"timeOfDay\":%.4f,\"clock\":\"%02d:%02d\","
                "\"night\":%.2f,\"look\":{\"shake\":%.2f,\"speedLines\":%.2f,\"fpvStab\":%s,\"viewModel\":\"%s\",\"stereo\":false}",
                L.gov.fps(), L.gov.tier(), q.name, L.gov.automatic() ? "true" : "false", L.show_perf ? "true" : "false", L.gov.changes(), L.lights_used,
                L.meter.ms(perf::Slot::logic), L.meter.ms(perf::Slot::physics), L.meter.ms(perf::Slot::stream), L.meter.ms(perf::Slot::draw), L.meter.update_ms(),
                L.meter.render_ms(), L.meter.gpu_ms(), q.render_scale, L.tod, hh, mm, night_, L.shake.trauma, L.lines.k, L.stab.init ? "true" : "false",
                L.vm_shown >= 0 ? kVmNames[L.vm_shown] : "");
  return b;
}

bool Game::look_command(const std::string& cmd, std::string& out) {
  LookState& L = look_;
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  char b[160];
  if (cmd == "time" || starts("time ")) {
    // "time 0.75" (phase), "time 21:30" / "time 21" (hours): sets the game's day clock.
    f32 phase;
    if (cmd.size() > 5 && look::parse_time(cmd.c_str() + 5, phase)) L.tod = phase;
    else if (cmd.size() > 5) { out = "usage: time <0..1 | HH:MM | HH>"; return true; }
    const f32 hours = L.tod * 24;
    std::snprintf(b, sizeof b, "%.4f %02d:%02d", L.tod, static_cast<int>(hours) % 24, static_cast<int>((hours - std::floor(hours)) * 60));
    out = b;
    return true;
  }
  if (cmd == "quality" || starts("quality ")) {
    // "quality auto" | "quality 0..4" | a level's name; plain "quality" reports.
    const std::string a = cmd.size() > 8 ? cmd.substr(8) : "";
    if (a == "auto") L.gov.set_auto();
    else if (!a.empty()) {
      i32 t = -1;
      for (i32 i = 0; i < perf::kTiers; ++i)
        if (a == kTier[i].name) t = i;
      if (t < 0 && a.size() == 1 && a[0] >= '0' && a[0] <= '4') t = a[0] - '0';
      if (t < 0) { out = "usage: quality auto | 0..4 | minimal | niedrig | mittel | hoch | maximal"; return true; }
      L.gov.set_fixed(t);
    }
    if (L.gov.tier() != L.applied_tier) look_apply_tier();
    const perf::Tier& q = kTier[L.gov.tier()];
    std::snprintf(b, sizeof b, "{\"tier\":%d,\"name\":\"%s\",\"auto\":%s,\"people\":%d,\"shadow\":%.0f,\"ao\":%d,\"scale\":%.2f,\"lights\":%d}", L.gov.tier(), q.name,
                  L.gov.automatic() ? "true" : "false", q.people, q.shadow_m, q.ao, q.render_scale, q.lights);
    out = b;
    return true;
  }
  if (cmd == "perf" || starts("perf ")) {
    // "perf 1" / "perf 0": the HUD's frame-time line; plain "perf" toggles.
    L.show_perf = cmd.size() > 5 ? cmd[5] == '1' : !L.show_perf;
    out = L.show_perf ? "on" : "off";
    return true;
  }
  if (starts("shake ")) {  // tests: add camera trauma
    L.shake.add(static_cast<f32>(std::atof(cmd.c_str() + 6)));
    std::snprintf(b, sizeof b, "%.2f", L.shake.trauma);
    out = b;
    return true;
  }
  if (cmd == "viewmodel") {  // tests: the shown first-person model, where it is relative to the eye
    if (L.vm_shown < 0) { out = "none"; return true; }
    const kern::Mesh* m = engine_->renderer.mesh(L.vm_body[L.vm_shown]);
    if (!m || m->transforms.size() < 16) { out = "no mesh"; return true; }
    const Vec3 p{m->transforms[12], m->transforms[13], m->transforms[14]};
    const Vec3 f = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
    std::snprintf(b, sizeof b, "%s visible %d ahead %.2f off %.2f verts %u", kVmNames[L.vm_shown], m->visible ? 1 : 0, kern::dot(p - cam_.eye, f), kern::length(p - cam_.eye),
                  m->vertex_count());
    out = b;
    return true;
  }
  if (cmd == "lights") {  // tests: the point lights of the last frame
    const kern::PointLights& pl = engine_->renderer.point_lights();
    out = "[";
    for (u32 i = 0; i < pl.count; ++i) {
      std::snprintf(b, sizeof b, "%s{\"p\":[%.1f,%.1f,%.1f],\"range\":%.1f,\"i\":%.2f}", i ? "," : "", pl.pos_range[i * 4], pl.pos_range[i * 4 + 1], pl.pos_range[i * 4 + 2],
                    pl.pos_range[i * 4 + 3], pl.color_intensity[i * 4 + 3]);
      out += b;
    }
    out += "]";
    return true;
  }
  return false;
}

}  // namespace sc
