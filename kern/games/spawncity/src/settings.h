// SpawnCity's player settings as one table (the game's single source): mouse, drone, on foot,
// car, camera, audio and the WORLD options of the title menu. The page builds its menus from
// `schema_json()` and sends changes as commands ("set <key> <value>"); it keeps a copy in
// localStorage only as a convenience and replays it with "set" at start. Values, ranges and the
// one-line help come from SpawnCity:
//   mouse_settings.mjs, drone_control_settings.mjs + control_semantics.mjs, first_person_control_settings.mjs,
//   vehicle_control_settings.mjs, camera_settings.mjs, stereo_sbs.mjs, audio_settings.mjs,
//   world_options.mjs, settings_help.mjs (help lines, here in German).
#pragma once

#include <string>
#include <string_view>

#include "common.h"

namespace sc {

enum class Opt : u8 {
  // mouse_settings.mjs
  mouse_sensitivity, mouse_invert_y,
  // drone (control_semantics.mjs DEFAULT_PHONE_SETTINGS)
  drone_left_fineness, drone_right_fineness, drone_invert_yaw, drone_invert_roll, drone_invert_pitch, drone_max_speed_kmh, drone_hover_agl,
  // on foot (first_person_control_settings.mjs)
  foot_move_fineness, foot_look_fineness, foot_look_h, foot_look_v, foot_deadzone, foot_invert_move_h, foot_invert_look_h, foot_invert_look_v,
  // car (vehicle_control_settings.mjs)
  car_steering, car_throttle, car_max_kmh, car_reverse_kmh, car_cam_distance, car_cam_height,
  // camera (camera_settings.mjs, stereo_sbs.mjs)
  cam_fpv_tilt, cam_fov, cam_third_distance, cam_stereo,
  // audio (audio_settings.mjs)
  audio_on, audio_drone, audio_shots, audio_fx, audio_steps, audio_vehicle, audio_ambient, music_on, music_volume,
  // world options (world_options.mjs)
  world_people, world_traffic, world_dogs, world_cats, world_birds, world_zombies, world_police, world_military, world_flak, world_jetpack,
  world_friendly_fire, world_terrain,
  // view and helpers (settings_help.mjs: worldGrid, worldKeepLook, worldMinimapAxisLock, box3dColliderDebug; aim assist)
  foot_aim_assist, pad_aim_scheme, view_grid, view_keep_look, view_minimap_north, debug_colliders,
  count
};

struct OptSpec {
  const char* key;      // command / storage key
  const char* section;  // menu section: maus, drohne, fuss, auto, kamera, ton, welt
  const char* label;    // menu label (German)
  const char* help;     // one line (settings_help.mjs)
  f32 min, max, step, def;
  bool toggle;
  bool live;            // false: a WORLD kind the game does not have yet (shown as "bald")
};

const OptSpec& opt_spec(Opt o);
bool find_opt(std::string_view key, Opt& out);

class Settings {
 public:
  Settings();
  f32 get(Opt o) const { return v_[static_cast<u32>(o)]; }
  bool on(Opt o) const { return get(o) > 0.5f; }
  /// Clamped and snapped to the step; false for an unknown key or a non-number.
  bool set(std::string_view key, std::string_view value);
  void set(Opt o, f32 value);
  void reset_section(std::string_view section);  // "" = everything
  /// Increases on every change (the game re-applies what depends on the settings).
  u32 version() const { return version_; }
  /// {"key":value,...}
  std::string values_json() const;
  /// [{"key":..,"section":..,"label":..,"help":..,"min":..,"max":..,"step":..,"def":..,"toggle":..,"live":..,"value":..},...]
  std::string schema_json() const;

 private:
  f32 v_[static_cast<u32>(Opt::count)];
  u32 version_ = 1;
};

/// control_semantics.mjs phoneAxis: x (1 - e) + x^3 e with e = 0.70 (level - 1) / 9 * 0.38.
f32 fineness_axis(f32 x, f32 fineness);

}  // namespace sc
