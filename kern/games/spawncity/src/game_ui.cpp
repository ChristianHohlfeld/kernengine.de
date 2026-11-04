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

namespace sc {

namespace {
f32 pad_deadzone(f32 v, f32 dz) {
  const f32 a = std::fabs(v);
  return a <= dz ? 0.0f : sign0(v) * (a - dz) / (1 - dz);
}
}  // namespace

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
    if (rest == "on") { if (!g.active()) g.start(time_); out = "on"; return true; }
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
      out = g.fix(time_, static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(acc), static_cast<f32>(speed),
                  course >= 0 ? static_cast<f32>(course * kDeg) : -1.0f) ? "ok" : "rejected";
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
  // mouse_settings.mjs scaleMouseDelta: invert Y for every mode.
  if (ui_.settings.on(Opt::mouse_invert_y)) {
    const Vec2 d = e.input.pointer_delta();
    e.input.pointer_move(e.input.pointer(), {0, -2 * d.y});
  }
}

void Game::ui_after_input(f32 dt) {
  (void)dt;
  const Settings& st = ui_.settings;
  switch (mode_) {
    case Mode::foot: {
      kern::game::PlayInput& p = play_in_;
      const f32 h = st.get(Opt::foot_look_h) / 100, v = st.get(Opt::foot_look_v) / 100;
      const f32 ih = st.on(Opt::foot_invert_look_h) ? -1.0f : 1.0f, iv = st.on(Opt::foot_invert_look_v) ? -1.0f : 1.0f;
      if (pad.connected && pad.active && !touch_mode_) {
        // The pad's look stick: SpawnCity's deadzone (2..20 %) and curve instead of the fixed 5 %.
        p.look_stick_x = fineness_axis(pad_deadzone(pad.rx, st.get(Opt::foot_deadzone) / 100), st.get(Opt::foot_look_fineness));
        p.look_stick_y = -fineness_axis(pad_deadzone(pad.ry, st.get(Opt::foot_deadzone) / 100), st.get(Opt::foot_look_fineness));
        p.move_x = fineness_axis(p.move_x, st.get(Opt::foot_move_fineness));
        p.move_y = fineness_axis(p.move_y, st.get(Opt::foot_move_fineness));
      }
      p.look_x *= h * ih;
      p.look_y *= v * iv;
      p.look_stick_x *= h * ih;
      p.look_stick_y *= v * iv;
      if (st.on(Opt::foot_invert_move_h)) p.move_x = -p.move_x;
      break;
    }
    case Mode::drone: {
      // Stick curves (phoneAxis), inversions and the GAME top speed (gameHorizontalSpeedScale: km/h / 90).
      const f32 right = st.get(Opt::drone_right_fineness), left = st.get(Opt::drone_left_fineness);
      const f32 speed = sticks_.game_mode ? st.get(Opt::drone_max_speed_kmh) / 90 : 1.0f;
      sticks_.roll = fineness_axis(sticks_.roll, right) * speed * (st.on(Opt::drone_invert_roll) ? -1.0f : 1.0f);
      sticks_.pitch = fineness_axis(sticks_.pitch, right) * speed * (st.on(Opt::drone_invert_pitch) ? -1.0f : 1.0f);
      sticks_.yaw = fineness_axis(sticks_.yaw, left) * (st.on(Opt::drone_invert_yaw) ? -1.0f : 1.0f);
      if (!ui_.drone_seen) {
        // A freshly launched drone holds the chosen hover height (defaultHoverAgl).
        sticks_.clearance = st.get(Opt::drone_hover_agl);
        ui_.drone_seen = true;
      }
      break;
    }
    case Mode::car:
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
}

void Game::ui_update(f32 dt) {
  ui_apply_settings();
  ui_gps_update(dt);
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
    c.hp = 300;
    c.wreck = false;
    c.burn_until = -1;
  }
  drone_hp_ = 100;
  jet_hp_ = 400;  // jet_mode.mjs HP_MAX
  jet_.fuel = 1;
  player_dead_ = false;
  // Fresh weapons, the pistol in hand (Kern's weapon component follows select_foot_weapon).
  init_weapons();
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
  std::snprintf(b, sizeof b, ",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u,\"moves\":%u,\"origin\":[%.6f,%.6f]}",
                world_placed_ ? "true" : "false", ui_.paused ? "true" : "false", ui_.settings.version(), ui_.resets, ui_.moves, lat_, lon_);
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
