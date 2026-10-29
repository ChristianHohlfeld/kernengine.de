// SpawnCity on Kern: the game. One kern::game::GameWorld level holds the player (Kern's soldier on
// Box3D's character mover), the drone, the jet, the traffic cars and the parachute as actors; the
// streamed real city adds static collision to the same Box3D world. The vehicles' own force models
// (drone + firmware, jet aero + FBW, car tyres) run in every fixed physics step through a Kern Logic
// node registered by the game (Box3D clears forces after each step).
//
// Modes (SpawnCity's): on foot, drone (V), car (F at a car: carjacking when it has a driver),
// jet (F at the jet; F in the air = ejection seat and parachute). Switching never loads anything.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "car.h"
#include "city_stream.h"
#include "drone.h"
#include "jet.h"
#include "kern/app/app.h"
#include "kern/audio/audio.h"
#include "kern/game/runtime.h"
#include "kern/game/sound.h"
#include "kern/game/touch.h"
#include "sound_fx.h"
#include "touch_pad.h"
#include "vehicle_art.h"
#include "wanted.h"

namespace sc {

enum class Mode : u8 { foot, drone, car, jet, parachute };
const char* mode_name(Mode m);

/// Xbox layout (standard mapping) as the page reads it each frame.
struct PadState {
  bool connected = false;
  f32 lx = 0, ly = 0, rx = 0, ry = 0, lt = 0, rt = 0;  // ly / ry: up positive
  u32 buttons = 0, pressed = 0;                         // bit = standard button index
  bool active = false;                                  // used this frame (device arbitration)
  bool down(int b) const { return buttons & (1u << b); }
  bool hit(int b) const { return pressed & (1u << b); }
};
enum PadButtonIndex { kPadA = 0, kPadB, kPadX, kPadY, kPadLB, kPadRB, kPadLT, kPadRT, kPadView, kPadMenu, kPadL3, kPadR3, kPadUp, kPadDown, kPadLeft, kPadRight };

struct TrafficCar {
  kern::game::ActorId actor = 0;
  std::unique_ptr<Car> car;
  Vec3 color{0.7f, 0.1f, 0.1f};
  bool placed = false, driver = true;
  i32 lane = -1, index = 0, dir = 1;
  f32 target_speed = 10, stuck = 0;
  CarInput input;  // what the driver (AI or player) does this step
};

class Game {
 public:
  Game();
  ~Game();
  bool start(kern::Engine& e);
  void update(kern::Engine& e, const kern::Frame& f);
  void stop();
  /// One fixed physics step (called by the game's logic node after Box3D's step).
  void fixed_step(f32 h);

  // ---- host interface (exports.cpp) ----
  CityStream& city() { return city_; }
  PadState pad;
  void set_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32 dpr);
  bool touch(u32 type, i32 id, f32 x, f32 y, f64 t_ms);
  std::string touch_view_json();
  std::string hud_json();
  std::string command(const std::string& cmd);
  bool audio_init(u32 rate);
  const f32* audio_render(u32 frames);
  void key_down_event() { arbiter_.key_down(); }
  /// The lit-windows shader extension (registered with the renderer, driven by GameWorld).
  kern::game::NightWindows& windows() { return windows_; }

 private:
  struct Spot { Vec3 p; f32 yaw; };
  kern::game::Level build_level() const;
  void place_world();
  bool clear_spot(Vec3 center, Vec2 half, f32 search, Spot& out) const;
  kern::physics::Body body(kern::game::ActorId a) const { return world_->actor_body(a); }
  BodyState state_of(kern::game::ActorId a) const { return read_body(world_->physics(), body(a)); }
  kern::Mat4 matrix_of(kern::game::ActorId a) const;
  Vec3 player_position() const;
  Vec3 focus_position() const;

  // modes
  void switch_mode(Mode m, i32 vehicle = -1);
  void hide_soldier();
  void show_soldier(Vec3 at, f32 yaw_deg);
  void try_enter();
  void exit_vehicle();
  void eject();
  void toggle_drone();

  // input per frame
  void read_input(kern::Engine& e, f32 dt);
  kern::game::PlayInput foot_input(kern::Engine& e, f32 dt);
  void drone_input(kern::Engine& e, f32 dt);
  void car_input(kern::Engine& e, f32 dt);
  void jet_input(kern::Engine& e, f32 dt);
  void parachute_input(kern::Engine& e, f32 dt);
  void update_touch_layout();

  // simulation pieces
  void drive_traffic(f32 h);
  void manage_traffic();
  void step_parachute(f32 h);

  // presentation
  void update_camera(kern::Engine& e, f32 dt);
  void draw_vehicles();
  void update_sound(f32 dt);

  kern::Engine* engine_ = nullptr;
  kern::game::NightWindows windows_;
  std::unique_ptr<kern::game::GameWorld> world_;
  CityStream city_;
  VehicleArt art_;
  kern::game::ActorId soldier_ = 0, drone_actor_ = 0, jet_actor_ = 0, chute_actor_ = 0;
  Drone drone_;
  Jet jet_;
  std::vector<TrafficCar> cars_;
  Mode mode_ = Mode::foot;
  i32 car_index_ = -1;       // the car the player drives
  bool drone_out_ = false, jet_placed_ = false, world_placed_ = false, chute_open_ = false;
  f32 chute_time_ = 0;
  Vec2 chute_steer_;
  DroneSticks sticks_;
  JetInput jet_in_;
  bool landing_ = false;     // drone: ARM in the air lands it (simulator.mjs)
  f32 landing_still_ = 0;
  bool height_held_ = false;
  i32 drone_cam_ = 1;        // 0 fpv, 1 follow, 2 third (SpawnCity default: follow)
  bool car_cockpit_ = false, jet_cockpit_ = false;
  f32 fpv_tilt_deg_ = -15, fpv_fov_deg_ = 105;
  f32 jet_pitch_rate_ = 0, jet_roll_rate_ = 0;  // mouse rate commands (tau 25 ms)
  f32 drone_yaw_rate_ = 0;
  Spot spawn_{};
  kern::Camera cam_;
  bool cam_valid_ = false;
  Vec3 cam_anchor_, cam_vel_;
  Vec2 cam_heading_{1, 0};
  bool rig_init_ = false;
  f32 head_yaw_ = 0, head_pitch_ = 0;
  f32 sensitivity_ = 1;
  f32 dpr_ = 1;
  // input devices
  kern::game::InputArbiter arbiter_;
  kern::game::TouchControls foot_touch_;
  kern::game::ControllerComponent soldier_controller_;
  TouchPad pad_touch_;
  bool touch_mode_ = false;
  f32 frame_w_ = 960, frame_h_ = 540;
  kern::game::PlayInput play_in_;
  // rules / feedback
  wanted::Tracker wanted_;
  std::string hint_, toast_;
  f32 toast_age_ = 99;
  f64 time_ = 0;
  f32 fps_ = 60;
  u64 steps_ = 0;
  Rng rng_{20261029};
  // audio
  kern::audio::Mixer mixer_;
  std::unique_ptr<kern::game::SoundBank> bank_;
  std::unique_ptr<kern::game::GameAudio> game_audio_;
  SoundFx fx_;
  std::vector<f32> audio_out_;
  // start parameters
  f64 lon_ = kKonstanzLon, lat_ = kKonstanzLat;
  std::string start_mode_;
  bool mobile_ = false;
};

Game& game();

}  // namespace sc
