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
  return false;
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
  people_.set_budget(st.on(Opt::world_people) ? (mobile_ ? 44 : 80) : 0);
  if (!st.on(Opt::world_people)) people_.clear();
}

void Game::ui_update(f32 dt) {
  (void)dt;
  ui_apply_settings();
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
  jet_hp_ = 600;
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
  char b[160];
  std::snprintf(b, sizeof b, ",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u}", world_placed_ ? "true" : "false", ui_.paused ? "true" : "false",
                ui_.settings.version(), ui_.resets);
  return b;
}

}  // namespace sc
