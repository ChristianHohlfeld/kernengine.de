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
#include "combat_art.h"
#include "drone.h"
#include "game_combat.h"
#include "jet.h"
#include "kern/app/app.h"
#include "kern/audio/audio.h"
#include "kern/game/runtime.h"
#include "kern/game/sound.h"
#include "kern/game/touch.h"
#include "net_session.h"
#include "people.h"
#include "people_art.h"
#include "road_graph.h"
#include "sound_fx.h"
#include "touch_pad.h"
#include "vehicle_art.h"
#include "wanted.h"
#include "weapons.h"

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
  // police cruisers and damage (police_ground_units.mjs: 300 HP, burning below 25 %)
  bool police = false;
  i32 unit = -1;          // the police unit using it
  f32 hp = 300, burn_until = -1, wreck_at = -1;
  bool wreck = false, hit_by_player = false;
  Vec3 last_velocity;
};

/// A police unit: a cruiser with two officers (police_ground_units.mjs).
struct PoliceUnit {
  enum class State : u8 { pursuit, stopping, engage, back, leave, abandoned, wreck };
  i32 car = -1;
  State state = State::pursuit;
  struct Cop { i32 person = -1; u32 seed = 0; bool inside = true; f32 next_shot = 0, los_at = 0; bool los = false; i32 seat = 1; };
  Cop cops[2];
  std::vector<Vec2> path;
  f32 path_at = -99, recover_until = -1, recover_start = 0, stuck_since = -1, turn_since = -1, corner_speed = 17;
  Vec2 last_target;
  Vec3 progress_at;
  f32 progress_t = 0;
  f32 left_at = -1;
};

/// A grenade, rocket, missile or bomb in flight.
struct Projectile {
  weapons::Id id = weapons::Id::grenade_launcher;
  Vec3 pos, vel;
  f32 born = 0, fuse = 12;
  bool by_player = true, gravity = false, guided = false, alive = true;
  i32 bounces = 0;
  kern::physics::Body ignore;
};

/// A police drone (wanted_police_drones.mjs): flies in a ring around the player at about his
/// height, sees within the detection radius, shoots now and then; EMP or bullets bring it down.
struct PoliceDrone {
  bool active = false, sees = false, emp = false, retreating = false;
  Vec3 pos, vel, target, retreat_goal;
  f32 hp = 100, yaw = 0, next_sensor = 0, next_shot = 0, sight_since = 0, destroyed_until = -1, retreat_until = -1, distance = 0, roof = 0, roof_at = -1;
  u32 shots = 0, serial = 0;
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
  net::Session& net() { return net_; }
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
  // the town: people, police, damage (game_town.cpp)
  void update_town(f32 dt);
  PeopleView people_view() const;
  void handle_shots();
  void update_police(f32 dt);
  void spawn_police_unit();
  void drive_police(PoliceUnit& u, f32 dt);
  void step_cop(PoliceUnit& u, PoliceUnit::Cop& c, f32 dt);
  bool player_driving() const { return mode_ == Mode::car || mode_ == Mode::jet || mode_ == Mode::drone; }
  bool line_of_sight(Vec3 a, Vec3 b) const;
  bool update_police_drones(f32 dt);  // true: a drone sees the player
  void spawn_police_drones();
  void emp_pulse();
  void damage_car(i32 car, f32 amount, bool by_player);
  void wreck_car(i32 car);
  void damage_player_vehicle(f32 amount);
  void explosion(Vec3 at, f32 radius, f32 damage, bool by_player);
  void draw_town();
  // weapons (game_weapons.cpp)
  void init_weapons();
  void select_foot_weapon(weapons::Id id);
  void cycle_foot_weapon(i32 step);
  bool game_weapon_in_hand() const;
  Vec3 aim_ray(Vec3& dir) const;
  bool hit_scan(Vec3 from, Vec3 dir, f32 range, f32 damage, bool by_player, kern::physics::Body ignore, kern::game::ShotEvent& out);
  void fire_projectile(weapons::Id id, Vec3 from, Vec3 vel, bool by_player, kern::physics::Body ignore);
  void update_projectiles(f32 dt);
  void foot_attack(bool trigger, bool pressed);
  void throw_grenade();
  void drone_attack(bool trigger, bool pressed);
  void jet_attack(bool gun, bool missile, bool bomb);
  void draw_projectiles();
  // multiplayer (game_net.cpp)
  void update_net(f32 dt);
  net::PlayerState my_state() const;
  void rebase_origin(f64 lon, f64 lat);
  /// A shot or blast against the other players: their machines apply the damage.
  bool hit_remotes(Vec3 from, Vec3& to, f32 damage);
  void blast_remotes(Vec3 at, f32 radius, f32 damage);
  void draw_remote_people();
  void draw_remote_vehicles();
  // combat and events (game_military.cpp, game_events.cpp) - hooked into update, draw, hud_json,
  // command, handle_shots and explosion
  void update_combat(f32 dt);
  void draw_combat();
  void combat_hud(std::string& s);
  bool combat_command(const std::string& cmd, std::string& out);
  bool combat_shot(kern::game::ShotEvent& s);  // true: a combat target took the bullet
  void combat_blast(Vec3 at, f32 radius, f32 damage, bool by_player);
  bool combat_target(Vec3& at, Vec3& vel, bool& drone, bool& air) const;  // what enemies shoot at
  void hurt_player(f32 foot_damage, f32 vehicle_damage, Vec3 from);
  void combat_tracer(Vec3 a, Vec3 b, Vec3 color = {3, 2.4f, 1.2f});
  void combat_banner(const std::string& text);
  bool spawn_unseen(Vec3 p, f32 height) const;
  void update_military(f32 dt);
  void spawn_heli(Vec3 target);
  bool spawn_tank(Vec3 target);
  i32 spawn_squad(Vec3 target, i32 n);
  void step_heli(Heli& h, f32 dt);
  void step_tank(Tank& t, f32 dt);
  void step_soldier(SoldierAi& s, f32 dt);
  void update_flak(f32 dt);
  void update_jets(f32 dt);
  void launch_jets(f32 cx, f32 cy, bool strike, f32 at, f32 alt, f32 offset);
  bool air_strike(Vec3 target);
  void update_fighters_back(f32 dt);
  void update_zombies(f32 dt);
  void update_enemies(f32 dt);
  void update_scores(f32 dt);
  void update_race();
  void update_debris(f32 dt);
  void draw_military();
  void draw_events();
  // world dressing
  void update_lamps();
  std::string map_json();

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
  // the town
  People people_;
  PeopleArt people_art_;
  CombatArt combat_art_;
  CombatState combat_;
  RoadGraph roads_;
  std::vector<PoliceUnit> units_;
  PoliceDrone pdrones_[5];
  Vec3 last_known_;
  f32 last_contact_ = -99, next_drone_wave_ = -1, emp_ready_at_ = 0, last_drone_shot_ = -99, last_player_hit_ = -99;
  u32 drone_serial_ = 0;
  bool drone_wave_live_ = false;
  // weapons
  std::vector<Projectile> projectiles_;
  weapons::Id foot_weapon_ = weapons::Id::glock;
  i32 ammo_mag_[16] = {}, ammo_reserve_[16] = {};
  i32 grenades_ = 4, drone_weapon_ = 0;
  f32 next_attack_ = 0, next_grenade_ = 0, next_drone_fire_ = 0, next_jet_gun_ = 0, next_jet_missile_ = 0, next_jet_bomb_ = 0;
  bool trigger_was_ = false;
  u32 explosions_ = 0;
  // multiplayer
  net::Session net_;
  std::unique_ptr<Car> remote_car_, remote_bus_;
  bool rebased_ = false;
  u8 my_color_ = 0;
  // street lamps and night
  struct Lamp { Vec3 foot; f32 yaw; u32 lane; };
  std::vector<Lamp> lamps_;
  std::size_t lamp_lanes_ = 0;  // lanes already given their lamps (the lane list only grows)
  f32 night_ = 0;
  bool show_lamps_ = true;
  f32 star_since_ = -1, last_wave_ = -99, last_cop_shot_ = -99, drone_hp_ = 100, jet_hp_ = 600, town_clock_ = 0;
  i32 last_stars_ = 0;
  bool player_dead_ = false;
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
