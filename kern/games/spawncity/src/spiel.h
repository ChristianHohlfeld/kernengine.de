// SpawnCity on Kern, the small game (26.11.2026): a drone over a real city. The city streams from map data (map
// tiles, elevation, aerial imagery: city_stream.h); the drone is the level's template "Drone", flown by its flight
// controller firmware (drone.h, firmware/) on the engine's world (kern::game::GameWorld). Keyboard, gamepad, mouse
// and touch fly it; the camera looks through its FPV camera, follows it or watches from behind. The page
// (web/host.mjs) shows the HUD the game writes as JSON and plays the original's sound from its audio state.
//
// What the game had besides (on foot, weapons, cars, the jet, people, multiplayer ...) is out of this game and comes
// back as building blocks of the engine (BLUEPRINTS.md, "Herausgenommen am 26.11.").
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "city_stream.h"
#include "controls.h"
#include "drone.h"
#include "drone_art.h"
#include "kern/app/app.h"  // (kern::Engine, kern::Frame)
#include "kern/audio/audio.h"
#include "kern/game/runtime.h"
#include "kern/game/sound.h"
#include "settings.h"
#include "sound_fx.h"
#include "touch_play.h"

namespace sc {

/// Xbox layout (standard mapping) as the page reads it each frame.
struct PadState {
  bool connected = false;
  f32 lx = 0, ly = 0, rx = 0, ry = 0, lt = 0, rt = 0;  // ly / ry: up positive
  u32 buttons = 0, pressed = 0;                         // bit = standard button index
  bool active = false;                                  // used this frame (device arbitration)
};
enum PadButtonIndex { kPadA = 0, kPadB, kPadX, kPadY, kPadLB, kPadRB, kPadLT, kPadRT, kPadView, kPadMenu, kPadL3, kPadR3, kPadUp, kPadDown, kPadLeft, kPadRight };

/// Whether pad button `b` is held (down) or went down this frame (hit).
bool pad_down(const PadState& p, int b);
bool pad_hit(const PadState& p, int b);

/// The game (its one object: game()) - and the App the engine runs (kern::App): its start, its frames and its stop
/// are the game's own (main.cpp hands game() to kern::run).
class SpawnCity final : public kern::App {
 public:
  bool start(kern::Engine& e) override;
  void update(kern::Engine& e, const kern::Frame& f) override;
  void stop(kern::Engine& e) override;

  // ---- what the page hands over (main.cpp: the exports)
  CityStream& city() { return city_; }
  PadState pad;
  void mouse_move(f32 dx, f32 dy);
  void pointer_locked(bool locked) { locked_ = locked; }
  void set_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32 dpr);
  bool touch(u32 type, i32 id, f32 x, f32 y);
  std::string touch_view_json();
  std::string hud_json();
  std::string command(const std::string& cmd);
  bool audio_init(u32 rate);
  const std::vector<f32>& audio_render(u32 frames);

  // ---- what a test reads
  bool placed() const { return placed_; }
  const kern::game::GameWorld* world() const { return world_.get(); }
  const Drone& drone() const { return drone_; }
  const DroneSticks& sticks() const { return sticks_; }
  /// The drone's place in the world (its body).
  Vec3 drone_position() const;
  /// The fixed steps run so far (1/240 s each).
  u64 steps() const { return steps_; }

 private:
  bool place();
  kern::Mat4 drone_matrix() const;  // the drone's pose as drawn (interpolated between the last two steps)
  void read_input(kern::Engine& e, f32 dt);
  void fly(f32 dt);
  void update_camera(kern::Engine& e, f32 dt);
  void update_sound(f32 dt);
  void apply_settings();
  void move_world(f64 lon, f64 lat);
  void reset();
  std::string audio_frame();
  std::string settings_command(const std::string& cmd);

  kern::Engine* engine_ = nullptr;
  std::unique_ptr<kern::game::GameWorld> world_;
  kern::game::ActorId drone_actor_ = 0;
  CityStream city_;
  Drone drone_;
  DroneSticks sticks_;
  DroneArt art_;
  SettingsTable settings_table_;
  Settings settings_;
  u32 applied_ = 0;            // the version of the settings applied
  TouchPlay touch_;
  bool touch_mode_ = false;    // a finger was last (the touch controls show)
  controls::RateStick mouse_;  // the mouse as the right stick (desktop_controls.mjs: 1500 px/s, 50 ms)
  Vec2 mouse_px_;              // mouse movement since the last frame (CSS px)
  bool locked_ = false;        // the pointer lock
  u32 pad_was_ = 0;            // the pad's buttons of the frame before
  f64 lon_ = 0, lat_ = 0;
  bool mobile_ = false, placed_ = false, paused_ = false, landing_ = false, height_held_ = false;
  f32 landing_still_ = 0;
  f64 accumulator_ = 0;        // the time the fixed steps have not taken yet (s)
  u64 steps_ = 0;
  i32 cam_mode_ = 0;           // 0 FPV, 1 follow, 2 third (simulator.mjs camFpv / camFollow / camThird)
  kern::Camera cam_;
  Vec3 cam_anchor_, cam_vel_;
  Vec2 cam_heading_{1, 0};
  bool rig_init_ = false;
  Vec3 focus_prev_;
  bool focus_prev_valid_ = false;
  f32 fps_ = 60, time_ = 0;
  std::string toast_;
  f32 toast_age_ = 99;
  u32 resets_ = 0, moves_ = 0;
  std::vector<kern::Vec4> ground_changes_;
  // sound: Kern's mixer (?audio=kern) or the original's on the page (audio web 1: the page asks for "audio frame")
  kern::audio::Mixer mixer_;
  std::unique_ptr<kern::game::SoundBank> bank_;
  std::unique_ptr<kern::game::GameAudio> game_audio_;
  SoundFx fx_;
  std::vector<f32> audio_out_;
  bool web_audio_ = false;
};

/// The one game (the entry gives it to the engine: main.cpp).
SpawnCity& game();

}  // namespace sc
