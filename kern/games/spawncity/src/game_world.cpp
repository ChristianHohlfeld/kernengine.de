// SpawnCity on Kern, milestone 3: the world (see game_world.h).
#include <cstdio>
#include <cstdlib>

#include "game.h"
#include "game_world.h"
#include "kern/audio/synth.h"

namespace sc {

using kern::game::ActorId;

namespace {

constexpr f32 kWaterDensity = 1000;
constexpr f32 kSwimHead = 0.42f;  // player_walk_mode_v4.mjs standHeightAt: eyes 0.42 m above the surface
constexpr f32 kEye = 1.68f, kFeetToCentre = 0.73f;

std::vector<f32> one_shot(std::initializer_list<kern::audio::SynthPatch> layers, u32 rate) {
  std::vector<f32> out;
  for (const kern::audio::SynthPatch& p : layers) kern::audio::mix_into(out, kern::audio::synthesize(p, rate), 0, rate, 1);
  kern::audio::normalize(out, 0.8f);
  return out;
}

}  // namespace

// ---------------------------------------------------------------------------------------------
// sounds: synth patches (no recorded assets)

static void init_world_sounds(kern::audio::Mixer& mixer, WorldSounds& s) {
  using kern::audio::SynthPatch;
  using kern::audio::Wave;
  const u32 rate = mixer.sample_rate();
  if (!rate) return;
  {  // splash: a burst of filtered noise that closes, and a low plop
    SynthPatch n;
    n.wave = Wave::noise;
    n.attack = 0.004f; n.sustain = 0.12f; n.release = 0.5f;
    n.lowpass = 5200; n.lowpass_sweep = -2.4f; n.highpass = 180; n.volume = 0.7f; n.seed = 11;
    SynthPatch plop;
    plop.wave = Wave::sine; plop.frequency = 180; plop.slide = -2.5f; plop.sustain = 0.06f; plop.release = 0.15f; plop.volume = 0.5f;
    s.splash = mixer.adopt_clip(one_shot({n, plop}, rate), 1, "spawncity:splash");
    n.sustain = 0.03f; n.release = 0.18f; n.lowpass = 7000;
    s.splash_small = mixer.adopt_clip(one_shot({n}, rate), 1, "spawncity:splash_small");
  }
  {  // car horn (world_procedural_population.mjs honk): two detuned squares
    SynthPatch h;
    h.wave = Wave::square; h.frequency = 392; h.sustain = 0.45f; h.release = 0.05f; h.lowpass = 2400; h.volume = 0.4f;
    SynthPatch h2 = h;
    h2.frequency = 494;
    s.horn = mixer.adopt_clip(one_shot({h, h2}, rate), 1, "spawncity:horn");
  }
  s.ready = true;
}

static void play_at(kern::audio::Mixer& mixer, kern::audio::ClipId clip, Vec3 at, f32 volume, f32 pitch, f32 max_distance = 60) {
  if (!clip) return;
  kern::audio::PlayParams p;
  p.volume = volume;
  p.pitch = pitch;
  p.spatial = true;
  p.position = at;
  p.min_distance = 2;
  p.max_distance = max_distance;
  mixer.play(clip, p);
}

// ---------------------------------------------------------------------------------------------
// HUD and frame

void Game::hud_world(std::string& s) {
  // The ground (terrain.h): elevation loaded, its range, ground tiles, water cells, craters.
  const Terrain& t = city_.terrain();
  const TerrainStats& ts = t.stats();
  char b[400];
  std::snprintf(b, sizeof b, ",\"terrain\":{\"dem\":%s,\"settled\":%s,\"min\":%.1f,\"max\":%.1f,\"render\":%u,\"collision\":%u,\"water\":%u,\"craters\":%u,\"demTiles\":%u,\"demFailed\":%u}",
                ts.dem ? "true" : "false", t.settled() ? "true" : "false", ts.min_height, ts.max_height, ts.render_tiles, ts.collision_tiles, ts.water_cells,
                ts.craters, ts.dem_tiles, ts.dem_failed);
  s += b;
  if (!life_) return;
  std::snprintf(b, sizeof b, ",\"swimming\":%s,\"splashes\":%u", life_->swimming ? "true" : "false", life_->splashes);
  s += b;
  const Marks::Stats& m = life_->marks.stats();
  std::snprintf(b, sizeof b, ",\"marks\":{\"scorch\":%u,\"soot\":%u,\"breach\":%u,\"rubble\":%u,\"craters\":%u}", m.scorches, m.soots, m.breaches, m.rubble,
                m.craters);
  s += b;
}

void Game::update_world(f32 dt) {
  if (!life_ || !world_placed_) return;
  WorldLife& L = *life_;
  if (!L.sounds.ready && mixer_.ready()) init_world_sounds(mixer_, L.sounds);
  if (!L.art_ready) L.art_ready = L.art.init(engine_->renderer);
  L.marks.update(world_->sim_time(), world_->physics());
  update_traffic_world(dt);
  const Terrain& t = city_.terrain();
  // Swimming (player_walk_mode_v4.mjs standHeightAt): in deep water the head stays above the
  // surface - the pilot floats with his eyes 0.42 m over it and walks / swims as on land.
  L.swimming = false;
  if (mode_ == Mode::foot && world_->actor_alive(soldier_)) {
    const Vec3 c = matrix_of(soldier_).translation();
    if (t.water_at(c.x, c.y)) {
      const f32 hold = t.water_level(c.x, c.y) + kSwimHead - kEye + kFeetToCentre;  // body centre
      if (c.z < hold) {
        L.swimming = true;
        Vec3 v = world_->actor_velocity(soldier_);
        if (c.z < hold - 0.05f) {
          if (v.z < -2.5f) L.pending_splashes.push_back({{c.x, c.y, t.water_level(c.x, c.y)}, 1.2f});
          world_->teleport(soldier_, {c.x, c.y, hold});
        }
        // Water drag on the swimmer: half the walking pace at most, no sinking.
        v.x *= 1 - damp_alpha(1.5f, dt);
        v.y *= 1 - damp_alpha(1.5f, dt);
        v.z = std::fmax(0.0f, v.z);
        world_->set_actor_velocity(soldier_, v);
      }
    }
  }
  for (const WorldLife::Splash& sp : L.pending_splashes) {
    ++L.splashes;
    world_->particle_burst(sp.at, {0.62f, 0.82f, 1.0f});  // world_impacts.mjs water chips 0x9fd0ff
    if (sp.size > 2) world_->particle_burst(sp.at + Vec3{0, 0, 0.6f}, {0.9f, 0.95f, 1.0f});
    if (L.sounds.ready) play_at(mixer_, sp.size > 1.5f ? L.sounds.splash : L.sounds.splash_small, sp.at, clampf(0.4f + sp.size * 0.2f, 0.3f, 1.0f),
                                1.15f - clampf(sp.size * 0.05f, 0, 0.4f), 80);
  }
  L.pending_splashes.clear();
}

// ---------------------------------------------------------------------------------------------
// fixed step: buoyancy and water drag (world_rigid_body_physics.mjs: Archimedes from the
// submerged fraction of the body's box, linear + quadratic drag against the current)

void Game::step_world(f32 h) {
  (void)h;
  if (!life_) return;
  WorldLife& L = *life_;
  const Terrain& t = city_.terrain();
  kern::physics::World& w = world_->physics();
  auto water = [&](u32 slot, ActorId a, Vec3 half, bool drone) {
    if (slot >= 32 || !a || !world_->actor_alive(a)) return;
    const kern::physics::Body b = body(a);
    if (!b.valid()) return;
    const BodyState s = read_body(w, b);
    f32 sub = 0;
    if (t.water_at(s.position.x, s.position.y)) sub = clampf((t.water_level(s.position.x, s.position.y) - (s.position.z - half.z)) / (2 * half.z), 0, 1);
    if (sub > 0) {
      const f32 m = w.mass(b);
      const Vec2 flow = t.flow_at(s.position.x, s.position.y);
      const Vec3 rel{s.velocity.x - flow.x, s.velocity.y - flow.y, s.velocity.z};
      const f32 speed = kern::length(rel), k = sub * (1.6f + 0.9f * speed) * m * 0.35f;
      // SpawnCity gives the drone no buoyancy (its motors are its lift); the water still drags it.
      const f32 volume = 8 * half.x * half.y * half.z;
      const f32 lift = drone ? 0.0f : kWaterDensity * kG * volume * sub * 0.62f;
      w.apply_force_to_center(b, {-rel.x * k, -rel.y * k, lift - rel.z * k * 1.4f});
      w.apply_torque(b, {-s.angular.x * m * sub * 0.8f, -s.angular.y * m * sub * 0.8f, -s.angular.z * m * sub * 0.5f});
      if (L.in_water[slot] <= 0 && std::fabs(s.velocity.z) > 1.5f)
        L.pending_splashes.push_back({{s.position.x, s.position.y, t.water_level(s.position.x, s.position.y)}, std::fmin(6.0f, 0.4f * std::fabs(s.velocity.z) * (1 + half.x))});
    }
    L.in_water[slot] = sub;
  };
  if (jet_placed_) water(0, jet_actor_, {0.8f, 8.1f, 0.8f}, false);  // the fuselage hull (jet.cpp rigid_spec)
  if (drone_out_) water(1, drone_actor_, {0.12f, 0.12f, 0.04f}, true);
  if (mode_ == Mode::parachute) water(2, chute_actor_, {0.3f, 0.3f, 0.85f}, false);
  for (std::size_t i = 0; i < cars_.size() && i + 3 < 32; ++i)
    if (cars_[i].placed) water(static_cast<u32>(i + 3), cars_[i].actor, cars_[i].car->spec().half, false);
  step_tow(h);
}

void Game::draw_world() {
  if (!life_ || !life_->art_ready) return;
  WorldLife& L = *life_;
  L.art.begin();
  L.marks.draw(L.art, world_->physics(), world_->sim_time());
  draw_traffic_lights();
  L.art.end();
}

void Game::world_blast(Vec3 at, f32 radius, f32 damage, bool by_player) {
  (void)by_player;
  if (!life_) return;
  const Terrain& t = city_.terrain();
  // world_impacts.mjs: a blast over water throws up a column of spray instead of leaving marks.
  if (t.water_at(at.x, at.y) && at.z - t.water_level(at.x, at.y) < radius) {
    life_->pending_splashes.push_back({{at.x, at.y, t.water_level(at.x, at.y)}, 2 + radius * 0.8f});
    return;
  }
  // Tow trucks are cars like any other (tow_service.mjs onExplosion: 1.3 x damage, falling off).
  for (WorldLife::TowTruck& tt : life_->trucks) {
    if (!tt.body.valid() || !world_->physics().body_valid(tt.body)) continue;
    const f32 d = kern::length(world_->physics().pose(tt.body).position - at);
    if (d < radius) tt.hp -= damage * 1.3f * std::pow(1 - d / radius, 1.2f);
  }
  // Craters, scorch, soot and breaches, rubble (world_marks.h); strength as facade_marks.mjs.
  life_->marks.blast(at, radius, std::fmin(1.4f, damage / 110), city_, world_->physics(), rng_, world_->sim_time(), ui_.settings.on(Opt::world_terrain));
}

bool Game::world_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  char b[320];
  const Terrain& t = city_.terrain();
  if (starts("ground")) {
    // "ground" (under the player) or "ground X Y": terrain height, normal, water there.
    Vec3 p = player_position();
    if (cmd.size() > 7) std::sscanf(cmd.c_str() + 7, "%f %f", &p.x, &p.y);
    const Vec3 n = t.normal(p.x, p.y);
    std::snprintf(b, sizeof b, "{\"x\":%.1f,\"y\":%.1f,\"z\":%.2f,\"elevation\":%.2f,\"normal\":[%.3f,%.3f,%.3f],\"water\":%s,\"level\":%.2f,\"playerZ\":%.2f}", p.x,
                  p.y, t.height(p.x, p.y), t.elevation(p.x, p.y), n.x, n.y, n.z, t.water_at(p.x, p.y) ? "true" : "false", t.water_level(p.x, p.y),
                  player_position().z);
    out = b;
    return true;
  }
  if (starts("traffic")) { out = traffic_json(); return true; }
  if (starts("blast")) {
    // Tests: "blast" = a rocket's blast (9.5 m, 160) 6 m in front of the player at knee height,
    // "blast wall" = against the nearest wall within 40 m.
    Vec3 me = player_position();
    const f32 yaw = world_->camera_yaw() * kDeg;
    Vec3 at{me.x + std::cos(yaw) * 6, me.y + std::sin(yaw) * 6, 0};
    if (cmd == "blast wall") {
      std::vector<CityStream::Wall> walls;
      city_.walls_near({me.x, me.y}, 40, walls);
      f32 best = 1e9f;
      for (const CityStream::Wall& wl : walls) {
        const f32 u = clampf((me.x - wl.a.x) * wl.dir.x + (me.y - wl.a.y) * wl.dir.y, 1, wl.len - 1);
        const Vec2 p = wl.a + wl.dir * u + wl.normal * 1.5f;
        const f32 d = std::hypot(p.x - me.x, p.y - me.y);
        if (t.water_at(p.x, p.y)) continue;  // over water a blast only throws up spray (no marks to test)
        // open ground in front of the wall: not inside or on top of the next house (terraced rows)
        if (std::fabs(city_.ground_height(p.x, p.y) - t.height(p.x, p.y)) > 0.5f) continue;
        if (cast(world_->physics(), {p.x, p.y, t.height(p.x, p.y) + 0.3f}, {0, 0, 1}, 25, {}, kCatWorld).hit) continue;
        if (d < best && wl.len > 3) { best = d; at = {p.x, p.y, 0}; }
      }
      if (best > 1e8f) { out = "no wall"; return true; }
    }
    const f32 before = t.height(at.x, at.y);  // "ground": the height before the blast (the crater test compares)
    at.z = city_.ground_height(at.x, at.y) + 0.5f;
    explosion(at, 9.5f, 160, true, Blast::weapon);
    std::snprintf(b, sizeof b, "{\"x\":%.1f,\"y\":%.1f,\"z\":%.1f,\"ground\":%.2f}", at.x, at.y, at.z, before);
    out = b;
    return true;
  }
  if (cmd == "water" || cmd == "water car") {
    // Tests: the nearest open water (spiral search, 8 m steps, 1.2 km); the player (or the
    // nearest car) is put 2 m above its surface.
    const Vec3 me = player_position();
    Vec2 found{1e9f, 1e9f};
    for (f32 r = 0; r <= 1200 && found.x > 1e8f; r += 8) {
      const int steps = r == 0 ? 1 : static_cast<int>(2 * kPi * r / 8);
      for (int i = 0; i < steps; ++i) {
        const f32 a = 2 * kPi * static_cast<f32>(i) / static_cast<f32>(steps);
        const Vec2 p{me.x + r * std::cos(a), me.y + r * std::sin(a)};
        // well inside the water (12 m from the bank)
        if (t.water_at(p.x, p.y) && t.water_at(p.x + 12, p.y) && t.water_at(p.x - 12, p.y) && t.water_at(p.x, p.y + 12) && t.water_at(p.x, p.y - 12)) { found = p; break; }
      }
    }
    if (found.x > 1e8f) { out = "no water"; return true; }
    const f32 level = t.water_level(found.x, found.y);
    if (cmd == "water car") {
      if (mode_ != Mode::car || car_index_ < 0) { out = "not in a car"; return true; }
      world_->teleport(cars_[car_index_].actor, {found.x, found.y, level + 2});
      world_->set_actor_velocity(cars_[car_index_].actor, {});
    } else {
      if (mode_ != Mode::foot) { out = "not on foot"; return true; }
      world_->teleport(soldier_, {found.x, found.y, level + 2});
      world_->set_actor_velocity(soldier_, {});
    }
    std::snprintf(b, sizeof b, "{\"x\":%.1f,\"y\":%.1f,\"level\":%.2f,\"bed\":%.2f}", found.x, found.y, level, t.height(found.x, found.y));
    out = b;
    return true;
  }
  return false;
}

}  // namespace sc
