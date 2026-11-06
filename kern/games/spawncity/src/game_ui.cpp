// SpawnCity on Kern: menus and settings on the game side. The page draws the title, pause and
// settings menus (DOM) from `settings schema` and sends every change as "set <key> <value>";
// this file applies them to the game: mouse and stick curves, inversions, speeds, cameras, the
// audio mix, the WORLD options; pause while a menu is open; RESET back to the start.
// Sources: mouse_settings.mjs, control_semantics.mjs, first_person_control_settings.mjs,
// vehicle_control_settings.mjs, camera_settings.mjs, audio_settings.mjs, world_options.mjs,
// game_reset.mjs, quit_confirm.mjs (the page side).
#include <cmath>
#include <cstdio>

#include "game.h"
#include "kern/geo/primitives.h"

namespace sc {

// ---------------------------------------------------------------------------------------------
// commands

bool Game::ui_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (ui_equipment_command(cmd, out)) return true;
  Settings& st = ui_.settings;
  if (cmd == "settings" || cmd == "settings schema") { out = st.schema_json(); return true; }
  if (cmd == "settings values") { out = st.values_json(); return true; }
  if (starts("settings reset")) {
    st.reset_section(cmd.size() > 15 ? cmd.substr(15) : std::string());
    out = st.values_json();
    return true;
  }
  if (starts("set ")) {
    // "set <key> <value>" (several pairs separated by ';' are fine: the page replays its copy).
    std::string rest = cmd.substr(4);
    u32 ok = 0, bad = 0;
    std::size_t at = 0;
    while (at < rest.size()) {
      std::size_t end = rest.find(';', at);
      if (end == std::string::npos) end = rest.size();
      const std::string pair = rest.substr(at, end - at);
      const std::size_t sp = pair.find(' ');
      if (sp != std::string::npos && st.set(std::string_view(pair).substr(0, sp), std::string_view(pair).substr(sp + 1))) ++ok;
      else if (!pair.empty()) ++bad;
      at = end + 1;
    }
    out = bad ? (ok ? "partly" : "unknown setting") : "ok";
    return true;
  }
  if (starts("get ")) {
    Opt o;
    if (!find_opt(cmd.substr(4), o)) { out = "unknown setting"; return true; }
    char b[32];
    std::snprintf(b, sizeof b, "%g", static_cast<double>(st.get(o)));
    out = b;
    return true;
  }
  if (starts("pause ")) {
    ui_.paused = cmd[6] == '1';
    if (engine_ && ui_.paused) engine_->request_pointer_lock(false);
    out = ui_.paused ? "paused" : "running";
    return true;
  }
  if (cmd == "reset") { ui_reset(); out = "ok"; return true; }
  // ---- start place (start_location.mjs, auto_flight_start.mjs) ----
  if (starts("start ")) {
    double lat = 0, lon = 0;
    char name[96] = {};
    const int n = std::sscanf(cmd.c_str() + 6, "%lf %lf %95[^\n]", &lat, &lon, name);
    if (n < 2 || !std::isfinite(lat) || !std::isfinite(lon) || std::fabs(lat) > 85 || std::fabs(lon) > 180) { out = "usage: start <lat> <lon> [name]"; return true; }
    if (net_.active()) { out = "im Mehrspieler nicht"; return true; }  // the host's city is everybody's
    ui_.place = name;
    ui_move_world(lon, lat);
    out = "moving";
    return true;
  }
  if (cmd == "origin") {
    char b[96];
    std::snprintf(b, sizeof b, "{\"lat\":%.6f,\"lon\":%.6f}", lat_, lon_);
    out = b;
    return true;
  }
  // ---- LIVE GPS (gps_live_mode.mjs) ----
  if (starts("gps ")) {
    const std::string rest = cmd.substr(4);
    GpsLive& g = ui_.gps;
    if (rest == "on") { if (!g.active()) { g.start(time_); ui_.trail.clear(); ui_.trail_dirty = true; } out = "on"; return true; }
    if (rest == "off") { g.stop(); ui_.gps_cam_init = false; out = "off"; return true; }
    if (rest.rfind("view", 0) == 0) {
      const std::string v = rest.size() > 5 ? rest.substr(5) : "next";
      g.view = v == "eye" ? GpsLive::View::eye : v == "follow" ? GpsLive::View::follow : v == "map" ? GpsLive::View::map
                                                                                              : static_cast<GpsLive::View>((static_cast<u8>(g.view) + 1) % 3);
      ui_.gps_cam_init = false;
      out = g.view == GpsLive::View::eye ? "eye" : g.view == GpsLive::View::follow ? "follow" : "map";
      return true;
    }
    if (rest.rfind("compass ", 0) == 0) { g.compass(static_cast<f32>(std::atof(rest.c_str() + 8))); out = "ok"; return true; }
    double lat = 0, lon = 0, acc = 0, speed = -1, course = -1;
    if (rest.rfind("fix ", 0) == 0 && std::sscanf(rest.c_str() + 4, "%lf %lf %lf %lf %lf", &lat, &lon, &acc, &speed, &course) >= 3) {
      if (!g.active()) { out = "off"; return true; }
      if (!world_placed_) { out = "loading"; return true; }
      f64 x = 0, y = 0;
      city_.projector().project(lon, lat, x, y);
      if (std::hypot(x, y) / 1000 > GpsLive::kReanchorKm && !net_.active()) {
        // Far from the world's origin: the world moves to you.
        ui_move_world(lon, lat);
        g.start(time_);
        out = "reanchor";
        return true;
      }
      const bool first = !g.has_fix();
      out = g.fix(time_, static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(acc), static_cast<f32>(speed),
                  course >= 0 ? static_cast<f32>(course * kDeg) : -1.0f) ? "ok" : "rejected";
      if (out == "ok") {
        // gps_live_mode.mjs addTrail: a point every 2 m, at most 4000.
        const Vec2 p = g.position();
        if (first || ui_.trail.empty() || std::hypot(p.x - ui_.trail.back().x, p.y - ui_.trail.back().y) >= 2) {
          ui_.trail.push_back(p);
          if (ui_.trail.size() > 4000) ui_.trail.erase(ui_.trail.begin());
          ui_.trail_dirty = true;
        }
      }
      return true;
    }
    out = "usage: gps on|off|view [eye|follow|map]|fix <lat> <lon> <acc> [speed] [course deg]|compass <rad>";
    return true;
  }
  return false;
}

// The world moves to another place: the city streams there and the player starts anew.
void Game::ui_move_world(f64 lon, f64 lat) {
  rebase_origin(lon, lat);
  ++ui_.moves;
  ui_.gps_cam_init = false;
  toast_ = ui_.place.empty() ? "Neuer Startort - die Stadt wird geladen" : "Startort: " + ui_.place;
  toast_age_ = 0;
}

void Game::ui_gps_update(f32 dt) {
  GpsLive& g = ui_.gps;
  if (!g.active() || !world_placed_ || ui_.paused) return;
  // The game can't hurt you in this mode: it mirrors real life (on foot, no damage, no wanted level).
  if (mode_ != Mode::foot) command("mode foot");
  g.frame(time_, dt);
  if (g.has_fix()) {
    const Vec2 p = g.shown();
    world_->teleport(soldier_, {p.x, p.y, city_.ground_height(p.x, p.y) + 0.73f});
    world_->set_actor_velocity(soldier_, {});
  }
  wanted_.clear();
  world_->heal(soldier_, 1000);
}

// The position puck and the track line on the ground (gps_live_mode.mjs ensureScene / pushTrailPoint).
void Game::ui_draw_gps(f32 dt) {
  kern::Renderer& r = engine_->renderer;
  if (!ui_.puck_acc) {
    namespace prim = kern::primitives;
    auto make = [&](u32& id, const kern::MeshData& m, Vec3 color, f32 alpha) {
      kern::Material mat;
      mat.unlit = true;
      mat.cast_shadow = false;
      mat.color = {color.x, color.y, color.z, alpha};
      mat.alpha = alpha < 1 ? kern::AlphaMode::blend : kern::AlphaMode::opaque;
      mat.double_sided = true;
      id = r.allocate_id();
      kern::MeshInput in = m.as_input();
      in.material = mat;
      in.visible = false;
      if (!r.upsert(id, in)) { r.release_id(id); id = 0; }
    };
    const Vec3 blue{0.26f, 0.52f, 0.96f};  // 0x4285f4
    make(ui_.puck_acc, prim::cylinder(1, 0.01f, 40), blue, 0.16f);
    make(ui_.puck_ring, prim::cylinder(0.62f, 0.012f, 28), {1, 1, 1}, 1);
    make(ui_.puck_dot, prim::cylinder(0.48f, 0.014f, 28), blue, 1);
    kern::MeshData wedge;
    const Vec3 c{1, 1, 1};
    for (int i = 0; i < 12; ++i) {  // a fan of 60 degrees, 4.2 m long, ahead of the dot
      const f32 a0 = -0.5f + i / 12.0f, a1 = -0.5f + (i + 1) / 12.0f;
      wedge.push({0, 0, 0}, {0, 0, 1}, c, {0, 0});
      wedge.push({std::sin(a0) * 4.2f, std::cos(a0) * 4.2f, 0}, {0, 0, 1}, c, {0, 0});
      wedge.push({std::sin(a1) * 4.2f, std::cos(a1) * 4.2f, 0}, {0, 0, 1}, c, {0, 0});
    }
    make(ui_.puck_wedge, wedge, blue, 0.28f);
  }
  const GpsLive& g = ui_.gps;
  const bool show = g.active() && g.has_fix() && world_placed_;
  for (u32 id : {ui_.puck_acc, ui_.puck_ring, ui_.puck_dot, ui_.puck_wedge})
    if (id) r.set_visible(id, show);
  if (show) {
    const Vec2 p = g.shown();
    const f32 z = city_.ground_height(p.x, p.y) + 0.05f, acc = std::fmax(1.0f, g.accuracy());
    auto put = [&](u32 id, const kern::Mat4& m) { if (id) r.update_instances(id, kern::Span<const f32>(m.m, 16), {}); };
    put(ui_.puck_acc, kern::Mat4::trs({p.x, p.y, z}, {}, {acc, acc, 1}));
    put(ui_.puck_ring, kern::Mat4::translate({p.x, p.y, z + 0.01f}));
    put(ui_.puck_dot, kern::Mat4::translate({p.x, p.y, z + 0.02f}));
    // heading: clockwise from north (+y); the wedge points along +y
    put(ui_.puck_wedge, kern::Mat4::trs({p.x, p.y, z + 0.03f}, Quat::axis_angle({0, 0, 1}, -g.heading()), {1, 1, 1}));
  }
  // The track line, re-uploaded at most twice a second.
  ui_.trail_upload += dt;
  if (ui_.trail_dirty && ui_.trail_upload > 0.5f) {
    ui_.trail_dirty = false;
    ui_.trail_upload = 0;
    if (!ui_.trail_mesh) ui_.trail_mesh = r.allocate_id();
    if (ui_.trail.size() < 2) r.set_visible(ui_.trail_mesh, false);
    else {
      kern::MeshData line;
      const Vec3 blue{0.26f, 0.52f, 0.96f};
      for (std::size_t i = 1; i < ui_.trail.size(); ++i) {
        for (const Vec2& q : {ui_.trail[i - 1], ui_.trail[i]}) line.push({q.x, q.y, city_.ground_height(q.x, q.y) + 0.12f}, {0, 0, 1}, blue, {0, 0});
      }
      kern::MeshInput in = line.as_input();
      in.material.lines = true;
      in.material.unlit = true;
      in.material.cast_shadow = false;
      in.material.frustum_culled = false;
      in.visible = true;
      if (!r.upsert(ui_.trail_mesh, in)) { r.release_id(ui_.trail_mesh); ui_.trail_mesh = 0; }
    }
  }
  if (ui_.trail_mesh && !g.active()) r.set_visible(ui_.trail_mesh, false);
  else if (ui_.trail_mesh && ui_.trail.size() >= 2 && !ui_.trail_dirty) r.set_visible(ui_.trail_mesh, true);
}

// settings_help.mjs worldGrid (1 m reference grid around the player) and box3dColliderDebug (Box3D's
// collision shapes as wire frames). Visual only.
void Game::ui_draw_helpers() {
  kern::Renderer& r = engine_->renderer;
  const Settings& st = ui_.settings;
  auto line_mesh = [&](u32& id, const std::vector<f32>& pos, const std::vector<f32>* col, Vec3 color) {
    if (!id) id = r.allocate_id();
    kern::MeshData m;
    for (std::size_t i = 0; i + 2 < pos.size(); i += 3) {
      const Vec3 c = col ? Vec3{(*col)[i], (*col)[i + 1], (*col)[i + 2]} : color;
      m.push({pos[i], pos[i + 1], pos[i + 2]}, {0, 0, 1}, c, {0, 0});
    }
    kern::MeshInput in = m.as_input();
    in.material.lines = true;
    in.material.unlit = true;
    in.material.cast_shadow = false;
    in.material.frustum_culled = false;
    in.material.linear_colors = col != nullptr;
    in.visible = !pos.empty();
    if (!r.upsert(id, in)) { r.release_id(id); id = 0; }
  };
  const bool grid = st.on(Opt::view_grid) && world_placed_;
  if (grid) {
    const Vec3 me = player_position();
    const f32 cx = std::round(me.x), cy = std::round(me.y);
    if (!ui_.grid_mesh || std::fabs(cx - ui_.grid_at.x) >= 1 || std::fabs(cy - ui_.grid_at.y) >= 1) {
      ui_.grid_at = {cx, cy, 0};
      std::vector<f32> p;
      const int n = 20;  // 40 x 40 m around the player
      for (int i = -n; i <= n; ++i) {
        for (int k = -n; k < n; ++k) {
          const f32 a = static_cast<f32>(k), b = static_cast<f32>(k + 1), o = static_cast<f32>(i);
          p.insert(p.end(), {cx + a, cy + o, city_.ground_height(cx + a, cy + o) + 0.03f, cx + b, cy + o, city_.ground_height(cx + b, cy + o) + 0.03f});
          p.insert(p.end(), {cx + o, cy + a, city_.ground_height(cx + o, cy + a) + 0.03f, cx + o, cy + b, city_.ground_height(cx + o, cy + b) + 0.03f});
        }
      }
      line_mesh(ui_.grid_mesh, p, nullptr, {0.55f, 0.85f, 1.0f});
    }
  } else if (ui_.grid_mesh) { r.set_visible(ui_.grid_mesh, false); ui_.grid_at = {1e9f, 1e9f, 0}; }
  if (st.on(Opt::debug_colliders) && world_placed_) {
    ui_.collider_clock += 1;
    if (static_cast<int>(ui_.collider_clock) % 6 == 0) {  // ten times a second is plenty
      std::vector<f32> pos, col;
      world_->debug_lines(true, false, false, pos, col);
      line_mesh(ui_.collider_mesh, pos, &col, {});
    }
  } else if (ui_.collider_mesh) r.set_visible(ui_.collider_mesh, false);
}

void Game::ui_camera(f32 dt) {
  (void)dt;
  const GpsLive& g = ui_.gps;
  if (!g.active() || !g.has_fix() || g.view == GpsLive::View::eye || mode_ != Mode::foot) return;
  // FOLLOW (behind and above, navigation style) or MAP (top-down, heading up), gps_live_mode.mjs.
  const Vec2 p = g.shown();
  const f32 h = g.heading(), fx = std::sin(h), fy = std::cos(h), gz = city_.ground_height(p.x, p.y), fast = clampf(g.speed() / 12, 0, 1);
  Vec3 eye, look, up{0, 0, 1};
  f32 fov = 62;
  if (g.view == GpsLive::View::follow) {
    const f32 back = 9 + 10 * fast, high = 6 + 6 * fast;
    eye = {p.x - fx * back, p.y - fy * back, gz + high};
    look = {p.x + fx * (6 + 10 * fast), p.y + fy * (6 + 10 * fast), gz + 1};
  } else {
    eye = {p.x - fx * 0.01f, p.y - fy * 0.01f, gz + 90 + 160 * fast};
    look = {p.x, p.y, gz};
    up = {fx, fy, 0};
    fov = 55;
  }
  if (!ui_.gps_cam_init) { ui_.gps_cam_eye = eye; ui_.gps_cam_look = look; ui_.gps_cam_init = true; }
  ui_.gps_cam_eye = ui_.gps_cam_eye + (eye - ui_.gps_cam_eye) * 0.18f;
  ui_.gps_cam_look = ui_.gps_cam_look + (look - ui_.gps_cam_look) * 0.25f;
  cam_.eye = ui_.gps_cam_eye;
  cam_.target = ui_.gps_cam_look;
  cam_.up = up;
  cam_.fov = fov * kDeg;
  cam_.near = 0.25f;
}

// ---------------------------------------------------------------------------------------------
// input: the settings on top of what read_input decided

void Game::ui_before_input(kern::Engine& e) {
  ui_.head_yaw_before = head_yaw_;
  ui_.head_pitch_before = head_pitch_;
  // mouse_settings.mjs scaleMouseDelta (invert Y) is applied to the mouse stream in game_controls.cpp.
}

void Game::ui_after_input(f32 dt) {
  (void)dt;
  const Settings& st = ui_.settings;
  switch (mode_) {
    case Mode::foot: {
      kern::game::PlayInput& p = play_in_;
      // curves, dead zone, sensitivities and inversions: game_controls.cpp (first_person_control_settings.mjs)
      // aim assist (first_person_control_settings.mjs / fps_control_math.mjs fpsAimAssist): applied to
      // the pad and touch look rates in game_controls.cpp (ctl_assist)
      (void)p;
      break;
    }
    case Mode::drone: {
      // Stick curves, inversions and the GAME top speed: applyGameStick in game_controls.cpp (control_semantics.mjs).
      if (!ui_.drone_seen) {
        // A freshly launched drone holds the chosen hover height (defaultHoverAgl).
        sticks_.clearance = st.get(Opt::drone_hover_agl);
        ui_.drone_seen = true;
      }
      break;
    }
    case Mode::car:
      // settings_help.mjs worldKeepLook: the head look stays (controls::WorldLook, game_controls.cpp)
      if (car_index_ >= 0) {
        CarInput& ci = cars_[car_index_].input;
        ci.max_speed = st.get(Opt::car_max_kmh) / 3.6f;
        ci.max_reverse = st.get(Opt::car_reverse_kmh) / 3.6f;
        ci.steer = clampf(ci.steer * st.get(Opt::car_steering) / 100, -1, 1);
        ci.pedal = clampf(ci.pedal * st.get(Opt::car_throttle) / 100, -1, 1);
      }
      break;
    default: break;
  }
  if (mode_ != Mode::drone) ui_.drone_seen = false;
  ui_jetpack(dt);  // game_equipment.cpp
}

// ---------------------------------------------------------------------------------------------
// per frame

void Game::ui_apply_settings() {
  Settings& st = ui_.settings;
  if (ui_.applied == 0 && engine_) {
    // Start parameters (?mouse=, ?fpvtilt=) are settings too.
    if (engine_->arg("mouse")) st.set(Opt::mouse_sensitivity, static_cast<f32>(engine_->arg_number("mouse", 100)));
    if (engine_->arg("fpvtilt")) st.set(Opt::cam_fpv_tilt, static_cast<f32>(engine_->arg_number("fpvtilt", -15)));
  }
  if (ui_.applied == st.version()) return;
  ui_.applied = st.version();
  sensitivity_ = st.get(Opt::mouse_sensitivity) / 100;
  fpv_tilt_deg_ = st.get(Opt::cam_fpv_tilt);
  fpv_fov_deg_ = st.get(Opt::cam_fov);
  // WORLD: switched-off kinds do not exist (world_options.mjs: no bodies, no AI, no draw).
  people_.set_budget(look_people_budget());  // the quality level's budget (game_look.cpp)
  if (!st.on(Opt::world_people)) people_.clear();
  // Military and zombie nights live in the combat systems (game_military.cpp, game_events.cpp); flak checks the option itself.
  // Only a change of the switch is passed on (the combat test commands may set them too).
  const i32 mil = st.on(Opt::world_military) ? 1 : 0, zom = st.on(Opt::world_zombies) ? 1 : 0;
  if (mil != ui_.world_military) { ui_.world_military = mil; if (ui_.world_military >= 0 && (mil == 0 || !combat_.military_enabled)) command(mil ? "military on" : "military off"); }
  if (zom != ui_.world_zombies) { ui_.world_zombies = zom; if (zom == 0 || !combat_.zombies_enabled) command(zom ? "zombies on" : "zombies off"); }
}

void Game::ui_update(f32 dt) {
  ui_apply_settings();
  ui_gps_update(dt);
  if (engine_) { ui_draw_gps(dt); ui_draw_helpers(); }
  if (world_placed_) ui_equipment(dt);
  const Settings& st = ui_.settings;
  if (!world_placed_) return;
  if (!st.on(Opt::world_traffic)) {
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      TrafficCar& c = cars_[i];
      if (c.police || static_cast<i32>(i) == car_index_ || !c.placed || !c.driver) continue;
      world_->set_alive(c.actor, false);
      c.placed = false;
    }
  }
  if (!st.on(Opt::world_police) && (wanted_.stars() > 0 || !units_.empty())) {
    wanted_.clear();
    ui_clear_police();
  }
}

void Game::ui_clear_police() {
  for (PoliceUnit& u : units_) {
    if (u.car >= 0 && u.car != car_index_) {
      world_->set_alive(cars_[u.car].actor, false);
      cars_[u.car].placed = false;
    }
    if (u.car >= 0) cars_[u.car].unit = -1;
  }
  units_.clear();
  for (PoliceDrone& d : pdrones_) d.active = false;
  drone_wave_live_ = false;
  next_drone_wave_ = -1;
  star_since_ = -1;
}

void Game::ui_mix_audio(SoundState& s) {
  // audio_settings.mjs: the sound switch, the effects / shots / footsteps / ambient / drone / vehicle
  // sliders (SpawnCity's defaults are this game's mix, so each slider scales relative to its default).
  const Settings& st = ui_.settings;
  kern::audio::Mixer& m = mixer_;
  m.bus(kern::audio::Bus::master).muted = !st.on(Opt::audio_on);
  m.bus(kern::audio::Bus::sfx).volume = st.get(Opt::audio_fx) / 72;
  m.bus(kern::audio::Bus::ambience).volume = st.get(Opt::audio_ambient) / 46;
  const f32 fx = std::fmax(0.01f, st.get(Opt::audio_fx) / 72);
  s.drone_mix = st.get(Opt::audio_drone) / 50 / fx;
  s.vehicle_mix = st.get(Opt::audio_vehicle) / 58 / fx;
  if (ui_.paused != ui_.audio_paused) {
    m.set_all_paused(ui_.paused);
    ui_.audio_paused = ui_.paused;
  }
  ui_mix_equipment(0);
}

void Game::ui_mix_cues() {
  // Shots and footsteps are cues on the effects bus: scale them by name before GameAudio plays them.
  const Settings& st = ui_.settings;
  const f32 fx = std::fmax(0.01f, st.get(Opt::audio_fx) / 72);
  for (kern::game::SoundCue& c : world_->sound_cues()) {
    const std::string& n = c.sound;
    // the soldier's own rounds (the weapon's fireSound, game_weapons.cpp): the original's foot weapon sound
    if (c.actor == soldier_ && (n == "sc:shot0" || n == "sc:pistol0" || n == "builtin:sniper")) osfx("foot.shot", c.position, static_cast<f32>(static_cast<i32>(foot_weapon_)));
    if (n == "builtin:step" || n == "builtin:land") c.volume *= st.get(Opt::audio_steps) / 30 / fx;
    else if (n == "builtin:pistol" || n == "builtin:smg" || n == "builtin:sniper" || n == "builtin:rifle" || n == "builtin:shotgun")
      c.volume *= st.get(Opt::audio_shots) / 82 / fx;
  }
}

// ---------------------------------------------------------------------------------------------
// RESET (game_reset.mjs, single player): everything back to the start - the start mode (on foot,
// pistol) at the start point, no wanted level, police gone, vehicles repaired, people anew.

void Game::ui_reset() {
  if (!world_placed_) return;
  gaps_lamps_reset();  // street_lamps.mjs onReset: every lamp stands and burns again (game_lamps.cpp)
  if (mode_ == Mode::parachute) {
    world_->set_alive(chute_actor_, false);
    show_soldier(matrix_of(chute_actor_).translation(), world_->camera_yaw());
    mode_ = Mode::foot;
  }
  if (mode_ != Mode::foot) command("mode foot");
  wanted_.clear();
  ui_clear_police();
  projectiles_.clear();
  people_.clear();
  for (TrafficCar& c : cars_) {
    if (c.police) continue;
    c.hp = car_max_hp(c);
    c.wreck = false;
    c.burn_until = -1;
  }
  drone_hp_ = 100;
  jet_hp_ = 400;  // jet_mode.mjs HP_MAX
  jet_.fuel = 1;
  player_dead_ = false;
  // Fresh weapons, the pistol in hand (Kern's weapon component follows select_foot_weapon).
  init_weapons();
  destruction_.reset(city_);  // every building whole again (world_destruction_state clearDestruction)
  foot_weapon_ = weapons::Id::knife;
  select_foot_weapon(weapons::Id::glock);
  world_->heal(soldier_, 10000);
  place_world();
  ++ui_.resets;
  toast_ = "Zurueck zum Start";
  toast_age_ = 0;
}

// ---------------------------------------------------------------------------------------------
// HUD

std::string Game::ui_hud_json() {
  char b[512];
  std::string s;
  std::snprintf(b, sizeof b, ",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u,\"moves\":%u,\"origin\":[%.6f,%.6f],\"northUp\":%s,\"assists\":%u,\"audioOn\":%s,\"musicOn\":%s}",
                world_placed_ ? "true" : "false", ui_.paused ? "true" : "false", ui_.settings.version(), ui_.resets, ui_.moves, lat_, lon_,
                ui_.settings.on(Opt::view_minimap_north) ? "true" : "false", ui_.assists, ui_.settings.on(Opt::audio_on) ? "true" : "false",
                ui_.settings.on(Opt::music_on) ? "true" : "false");
  s += b;
  const GpsLive& g = ui_.gps;
  if (g.active()) {
    const char* view = g.view == GpsLive::View::eye ? "AUGE" : g.view == GpsLive::View::follow ? "FOLGEN" : "KARTE";
    std::snprintf(b, sizeof b, ",\"gps\":{\"fix\":%s,\"activity\":\"%s\",\"speed\":%.2f,\"distance\":%.1f,\"time\":%.0f,\"accuracy\":%.0f,\"view\":\"%s\",\"accepted\":%u,\"rejected\":%u}",
                  g.has_fix() ? "true" : "false", g.activity_label(), g.speed(), g.distance(), g.elapsed(time_), g.accuracy(), view, g.accepted, g.rejected);
    s += b;
  }
  return s + ui_equipment_json();
}

}  // namespace sc
