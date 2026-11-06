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
#include <unordered_map>
#include <vector>

#include "car.h"
#include "city_stream.h"
#include "facades.h"
#include "water_look.h"
#include "combat_art.h"
#include "destruction.h"
#include "guided.h"
#include "combat_detail.h"
#include "combat_audio.h"
#include "drone.h"
#include "game_ui.h"
#include "game_combat.h"
#include "game_gaps.h"
#include "game_look.h"
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
#include "game_controls.h"  // controls state (game_controls.cpp)
#include "touch_play.h"     // touch controls of every mode (touch_play.cpp)
#include "vehicle_art.h"
#include "wanted.h"
#include "weapons.h"

namespace sc {

struct MpLocal;  // multiplayer presentation state (game_net.cpp)
struct MpLocalDelete { void operator()(MpLocal* m) const; };

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
  bool far = false;   // beyond 180 m: a kinematic lane follower (game_traffic_far.cpp)
  f32 far_speed = 0;
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
  bool resting = false;                  // a grenade lying still (gameplay_final_runtime_v2.mjs)
  f32 next_collision = 0, smoke_at = -1;  // bounce guard (36 / 60 ms), the rocket's last puff
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

struct WorldLife;  // game_world.h
struct GameLife;   // game_life.h (M3 life)

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
  void key_down_event() { ctl_.dev.key(); }
  /// Mouse movement under the pointer lock (CSS px, clamped per event) and the lock state (game_controls.cpp).
  void mouse_move(f32 dx, f32 dy) { ctl_.mouse.x += dx; ctl_.mouse.y += dy; }
  void pointer_locked(bool locked) { ctl_.locked = locked; }
  /// Microphone samples from the page (only while the mic is on; game_net.cpp).
  void voice_capture(const f32* samples, u32 count, u32 rate);
  /// The lit-windows shader extension (registered with the renderer, driven by GameWorld).
  Facades& windows() { return windows_; }  // the original's facades and lit windows (facades.h)
  WaterLook& water_look() { return water_look_; }  // the original's water shader (water_look.h)

 private:
  struct Spot { Vec3 p; f32 yaw; };
  kern::game::Level build_level() const;
  void place_world();
  bool clear_spot(Vec3 center, Vec2 half, f32 search, Spot& out, bool off_road = false) const;
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
  // controls (game_controls.cpp, CONTROLS.md): devices, pad actions, strike aiming, grenade cook, head look
  bool ctl_command(const std::string& cmd, std::string& out);
  std::string ctl_hud_json();
  void ctl_discrete(kern::Engine& e, f32 dt);
  void ctl_grenade(bool hold_key, bool hold_pad, f32 dt);
  void ctl_strike_start();
  void ctl_strike_confirm(bool at_point, f32 x, f32 y);
  bool ctl_screen_ray(f32 x, f32 y, Vec3& from, Vec3& dir) const;
  void ctl_camera(kern::Camera& c);
  void ctl_enter(bool jet_first);
  bool ctl_touch_shot(f32 x, f32 y);
  std::string ctl_view_json();  // what the original's HUD modules show (web/view_modes.mjs)
  controls::AimAssist ctl_assist(f32 in_yaw, f32 in_pitch, f32 input);

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
  /// What set a blast off: only weapons break buildings (destruction.h); the guided missile x 2.2.
  enum class BlastBy : u8 { other, weapon, missile };
  void explosion(Vec3 at, f32 radius, f32 damage, bool by_player, BlastBy kind = BlastBy::other);
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
  // multiplayer (game_net.cpp; its state lives in MpLocal, see mp_rules.h / net_session.h)
  MpLocal& mp_local();
  // ---- the mates' units (game_swarm.cpp: zombie_nights.mjs zombie-sync, military_response.mjs mil-snap)
  struct RemoteUnit { u16 id = 0; net::SwarmKind kind = net::SwarmKind::zombie; u8 flags = 0; Vec3 p, tp; f32 yaw = 0, tyaw = 0, aux = 0, rotor = 0; i32 person = -1; };
  struct RemoteSwarm { f64 seen = 0; std::vector<RemoteUnit> units; };
  std::unordered_map<u8, RemoteSwarm> swarms_;
  f64 swarm_clock_ = 0;
  u16 swarm_serial_ = 0;
  void swarm_update(f32 dt);
  void swarm_clear();
  bool swarm_shot(kern::game::ShotEvent& s);   // a mate's helicopter or tank on the line
  bool swarm_person_hit(i32 person, f32 damage);  // a mate's zombie / soldier: the hit goes to him
  void swarm_draw();
  void mp_input(kern::Engine& e, f32 dt);
  std::string mp_hud_json();
  std::string net_command(const std::string& cmd);
  std::string bigmap_json(f32 cx, f32 cy, f32 range);
  void update_nav();
  bool project_screen(Vec3 p, f32& x, f32& y, f32& depth) const;
  void mix_voice(f32* out, u32 frames);
  void update_net(f32 dt);
  net::PlayerState my_state() const;
  void rebase_origin(f64 lon, f64 lat);
  /// A shot or blast against the other players: their machines apply the damage.
  bool hit_remotes(Vec3 from, Vec3& to, f32 damage);
  void blast_remotes(Vec3 at, f32 radius, f32 damage, u8 breaks = 0);
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
  void launch_jets(f32 cx, f32 cy, bool strike, f32 at, f32 alt, f32 offset, bool remote = false);
  bool air_strike(Vec3 target);
  void update_fighters_back(f32 dt);
  void update_zombies(f32 dt);
  void update_enemies(f32 dt);
  void update_scores(f32 dt);
  void update_race();
  void update_debris(f32 dt);
  void draw_military();
  void draw_events();
  // GUIDED missile with target lock (game_guided.cpp, guided_missile.mjs)
  bool guided_press();
  void update_guided(f32 dt);
  void draw_guided();
  void guided_hud(std::string& s);
  bool guided_target_at(const guided::Target& t, Vec3& at) const;
  // more event / combat details: critical damage, bullet momentum, rule ballots (game_details.cpp)
  void update_details(f32 dt);
  void details_hud(std::string& s);
  bool details_command(const std::string& cmd, std::string& out);
  // SpawnCity's synthesised combat sounds (combat_audio.cpp) registered as "sc:<kind><n>"
  void combat_audio_init();
  const char* combat_sfx(combat_audio::Kind k);
  // world dressing
  void update_lamps();
  std::string map_json();
  std::string minimap_json();  // the original's minimap (real_world_bootstrap.mjs drawMinimap), game_town.cpp
  // the world (game_world.cpp, M3): terrain and water physics, craters and marks, traffic rules,
  // animals and street life, interiors, ragdolls. State lives in WorldLife (game_world.h).
  void hud_world(std::string& s);
  void update_world(f32 dt);
  void step_world(f32 h);
  void draw_world();
  bool world_command(const std::string& cmd, std::string& out);
  void world_blast(Vec3 at, f32 radius, f32 damage, bool by_player);
  void world_bullet(Vec3 at);  // a round that hit the world: a window pane cracks (world_marks.h)
  std::unique_ptr<WorldLife> life_;
  // street life, animals, ragdolls, walkable buildings (game_life.cpp, M3 life; state in
  // GameLife, game_life.h, declared last so it goes before the world it has bodies in)
  void life_update(f32 dt);
  void life_draw();
  void life_draw_people();  // the running club, inside draw_town's people drawing
  bool life_hides_person(i32 index) const;  // lies as a ragdoll
  bool life_shot(Vec3 from, Vec3& to, f32 damage);
  void life_blast(Vec3 at, f32 radius);
  void life_hud(std::string& s);
  bool life_command(const std::string& cmd, std::string& out);
  bool life_punch(Vec3 from, Vec3& to);   // fists / knife on animals
  void life_net_fx(const net::Fx& f);     // cow deaths and pushes from the other players
  // traffic (game_traffic.cpp, M3): junction rules, the driver getting out, the tow service
  f32 traffic_rules(i32 car, Vec3 pos, Vec3 fwd, f32 speed);
  void eject_driver(i32 car);
  void update_traffic_world(f32 dt);
  void step_tow(f32 h);
  void draw_world_vehicles();
  void draw_traffic_lights();
  std::string traffic_json();

  // menus, settings, start, equipment, presentation (game_ui.cpp)
  bool ui_command(const std::string& cmd, std::string& out);
  void ui_before_input(kern::Engine& e);
  void ui_after_input(f32 dt);
  void ui_apply_settings();
  void ui_update(f32 dt);
  void ui_clear_police();
  void ui_mix_audio(SoundState& s);
  void ui_mix_cues();
  void ui_reset();
  void ui_move_world(f64 lon, f64 lat);
  void ui_gps_update(f32 dt);
  void ui_camera(f32 dt);
  void ui_draw_gps(f32 dt);
  void ui_draw_helpers();
  std::string ui_hud_json();
  // equipment (game_equipment.cpp)
  void ui_audio_init();
  void ui_jetpack(f32 dt);
  void ui_equipment(f32 dt);
  void ui_radio(i32 dir);
  void ui_mix_equipment(f32 dt);
  void ui_render_music(f32* out, u32 frames);
  bool ui_equipment_command(const std::string& cmd, std::string& out);
  std::string ui_equipment_json();

  // performance and the look (game_look.cpp): frame times, automatic quality, camera shake,
  // FPV stabilizer, speed streaks, day/night clock, point lights, own first-person weapon models
  void look_begin(kern::Engine& e, const kern::Frame& f);
  void look_lap(perf::Slot s);
  void look_camera(f32 dt);
  void look_lighting(f32 dt);
  void look_draw(kern::Engine& e, f32 dt);
  void look_blast(Vec3 at, f32 radius);
  void look_apply_tier();
  void look_view_models(f32 dt);
  i32 look_people_budget() const;
  bool look_command(const std::string& cmd, std::string& out);
  std::string look_hud_json();

  // sc-m3-gaps (game_gaps.cpp, PARITY.md): sirens, police dispatch, critical smoke, organic hit
  // sound, impact bursts, spawn gates, camera guard, jet damage
  // on-foot projectiles and melee (game_foot_rounds.cpp, gameplay_final_runtime_v2.mjs)
  bool foot_round_step(Projectile& p, f32 dt, f32 now, Vec3& at);
  void gaps_sniper_rounds(const std::vector<kern::game::ShotEvent>& shots, std::vector<kern::game::ShotEvent>& arrived, f32 now);
  void grenade_bounce(Projectile& p, Vec3 point, Vec3 n, f32 now);
  bool car_near_point(Vec3 point) const;
  bool soft_target_near(Vec3 p) const;
  bool foot_remote_detonate(f32 now);
  void foot_rocket_launched();
  // updateRocketLaser: the dot's source - a finger on the picture > the point it was lifted at
  // (while a guided rocket flies) > the crosshair
  bool laser_finger_ = false, laser_pinned_ = false, laser_valid_ = false;
  f32 laser_fx_ = 0, laser_fy_ = 0;
  Vec3 laser_pin_{}, laser_dot_{};
  Vec3 rocket_laser_dot();
  // gameplay_final_runtime_v2.mjs visualBlast: every explosion's orange core and flat shock ring (540 ms)
  struct VisualBlast { Vec3 p; f32 born = 0, scale = 1; };
  std::vector<VisualBlast> visual_blasts_;
  // spawnGrenadeBounceFx: a launcher round's bounce - an orange ring on the surface and a white
  // flash, 180 ms (a pool of 8, the oldest reused)
  struct BounceFx { Vec3 p, n{0, 0, 1}; f32 born = -99, strength = 1; };
  BounceFx bounce_fx_[8];
  u32 bounce_fx_next_ = 0;
  void rocket_smoke_emit(Projectile& p, f32 now);
  void foot_rounds_update(f32 dt);
  void blast_police_drones(Vec3 at, f32 radius, f32 damage, bool by_player);
  // far traffic (game_traffic_far.cpp, PHYS_NEAR_M / PHYS_FAR_M)
  void traffic_far_switch();
  void traffic_far_step(TrafficCar& c, f32 h);
  void traffic_advance(TrafficCar& c);
  // cars and buses as the original (game_vehicle_damage.cpp)
  f32 car_max_hp(const TrafficCar& c) const;
  void car_bullet(i32 car, bool by_player);
  void blast_cars(Vec3 at, f32 radius, f32 damage, bool by_player);
  bool melee_hit(f32 reach, f32 sweep, f32 damage, bool knife, f32 momentum_ns);
  void melee_attack();
  void gaps_init(kern::Engine& e);
  void gaps_update(f32 dt);
  void gaps_sirens(f32 dt);
  void gaps_jet_audio(f32 dt);
  void gaps_guided(f32 dt);
  void gaps_guided_sounds();
  void gaps_draw();
  void gaps_hud(std::string& s);
  bool gaps_command(const std::string& cmd, std::string& out);
  void gaps_camera(kern::Camera& c);
  bool gaps_can_spawn(Vec3 p, f32 height, f32 far_entry) const;
  bool gaps_vehicle_spawn_ok(i32 car, Vec3 pos, f32 yaw) const;
  bool car_towed(i32 car) const;  // a tow truck has it as its job (game_traffic.cpp)
  bool gaps_drone_due(u32 slot, i32 stars);
  void gaps_drone_revealed(u32 slot, i32 stars);
  void gaps_jet_damage(f32 amount);
  kern::Mat4 gaps_drone_matrix();
  // zombie barricades (game_barricades.cpp, zombie_nights.mjs)
  bool gaps_barricade_doors(std::vector<GapsState::DoorView>& out);
  Vec3 gaps_zombie_goal(i32 person, Vec3 pos, Vec3 player, f32 now, bool& walled, bool& hold);
  bool gaps_repair_barricade();
  bool gaps_repair_offer() const;
  void gaps_barricades_draw();
  // street lamps (game_lamps.cpp, street_lamps.mjs)
  void gaps_lamps_init(kern::Renderer& r);
  void gaps_lamps_sounds();
  void gaps_lamps_place();
  void gaps_lamps_mirror();
  void gaps_lamp_break(i32 i, u8 state, Vec2 dir, f32 speed, bool net);
  void gaps_lamps_remote(const net::Fx& f);
  void gaps_lamps_blast(Vec3 at, f32 radius);
  void gaps_lamps_update(f32 dt);
  void gaps_lamps_reset();
  void gaps_lamps_draw();
  void gaps_remote_fx(const net::Fx& f);
  void gaps_send_jets(bool strike, Vec3 p, f32 offset, f32 alt);
  void gaps_send_military(bool shell, Vec3 from, Vec3 to);
  // riding along in a mate's jet (game_seats.cpp, jet_mode.mjs passenger, jet_seats.h)
  bool gaps_riding() const;
  bool gaps_ride_pose(Vec3& p, Quat& q) const;
  bool gaps_seat_press();
  void gaps_leave_ride();
  void gaps_seats_update(f32 dt);
  bool gaps_ride_camera(kern::Camera& c);
  std::unique_ptr<GapsState> gaps_;

  // presentation
  void update_camera(kern::Engine& e, f32 dt);
  void draw_vehicles();
  void update_sound(f32 dt);

  kern::Engine* engine_ = nullptr;
  bool vr_ = false;
  /// The view's magnification against the mode's own field of view (the sniper scope, ADS: > 1):
  /// draw distances follow it, so a zoomed view shows what an unzoomed one shows nearer.
  f32 view_zoom() const;  // the page runs a WebXR session ("vr 1")
  Facades windows_;
  WaterLook water_look_;
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
  i32 drone_cam_ = 0;        // 0 fpv, 1 follow, 2 third (the page restores the player's last choice; FPV first)
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
  kern::game::ControllerComponent soldier_controller_;
  TouchPlay pad_touch_;   // every mode's touch controls (touch_play.h)
  ControlsState ctl_;     // game_controls.h
  bool touch_mode_ = false;
  f32 frame_w_ = 960, frame_h_ = 540;
  kern::game::PlayInput play_in_;
  // the town
  People people_;
  PeopleArt people_art_;
  CombatArt combat_art_;
  Destruction destruction_;  // buildings broken by weapons (destruction.h)
  CombatState combat_;
  std::vector<guided::Flight> guided_;  // GUIDED missiles in flight (game_guided.cpp)
  f32 guided_last_ = -99;
  std::string guided_lock_;
  // game_details.cpp: critical countdowns (critical_damage_logic.mjs), the rule ballot
  std::vector<f32> car_last_hp_, car_burn_seen_;
  detail::CriticalClock pd_crit_[5], drone_crit_;
  detail::Vote ballot_;
  u32 bullet_pushes_ = 0, crit_blasts_ = 0;
  u32 sfx_cursor_[16] = {};
  bool combat_audio_ready_ = false;
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
  f32 jet_rocket_side_ = 1;
  f32 jacked_at_ = -1;
  Vec3 jet_cam_off_;         // jet_mode.mjs camOff (the chase camera's eased offset)       // tests: distance of the car "mode car" took  // jet_mode.mjs rocketSide: the pods fire in turn
  f32 next_attack_ = 0, next_grenade_ = 0, next_drone_fire_ = 0, next_jet_gun_ = 0, next_jet_missile_ = 0, next_jet_bomb_ = 0;
  bool trigger_was_ = false;
  u32 explosions_ = 0;
  // multiplayer
  net::Session net_;
  std::unique_ptr<Car> remote_car_, remote_bus_;
  bool rebased_ = false;
  u8 my_color_ = 0;
  std::unique_ptr<MpLocal, MpLocalDelete> mp_;  // pings, feed, voice, big map (game_net.cpp)
  // street lamps and night
  struct Lamp { Vec3 foot; f32 yaw; u32 lane; u8 state = 0; };  // state: street_lamps.mjs 0 lit, 1 dark, 2 down
  std::vector<Lamp> lamps_;
  std::size_t lamp_lanes_ = 0;  // lanes already given their lamps (the lane list only grows)
  f32 night_ = 0;
  bool show_lamps_ = true;
  f32 star_since_ = -1, last_wave_ = -99, last_cop_shot_ = -99, drone_hp_ = 100, jet_hp_ = 400, town_clock_ = 0;  // jet: jet_mode.mjs HP_MAX 400
  i32 last_stars_ = 0;
  bool player_dead_ = false;
  // vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the
  // RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  f32 died_at_ = -1;
  Vec3 died_feet_{};
  i32 respawn_rule_ = -1;
  bool reset_revive_ = false;  // RESET while down: Kern revives the soldier next step, then the start point  // the respawn delay set on Kern's soldier: 0 by input, 1 the mates' timer
  static constexpr f32 kRespawnCooldown = 3;
  bool request_respawn();
  f32 respawn_wait() const { return player_dead_ && died_at_ >= 0 ? std::fmax(0.0f, kRespawnCooldown - (world_->sim_time() - died_at_)) : -1; }
  // rules / feedback
  wanted::Tracker wanted_;
  std::string hint_, toast_;
  f32 toast_age_ = 99;
  f64 time_ = 0;
  f32 fps_ = 60;
  u64 steps_ = 0;
  Rng rng_{20261029};
  bool spawn_checked_ = false;  // world_spawn_guard (Game::spawn_guard)
  void spawn_guard();
  // audio
  kern::audio::Mixer mixer_;
  std::unique_ptr<kern::game::SoundBank> bank_;
  std::unique_ptr<kern::game::GameAudio> game_audio_;
  SoundFx fx_;
  std::vector<f32> audio_out_;
  // the original's sound on the page (game_audio_web.cpp): events and state for web/audio.mjs
  bool web_audio_ = false;
  std::string osfx_;
  void osfx(const char* fn, Vec3 at, f32 a = 0, f32 b = 0, f32 c = 0);
  bool shot_flesh_ = false;  // hit_scan's last round struck a person or a player
  f32 hit_material(Vec3 at, u8 surface, bool flesh) const;
  std::string web_audio_state();
  bool web_audio_command(const std::string& cmd, std::string& out);
  // start parameters
  f64 lon_ = kKonstanzLon, lat_ = kKonstanzLat;
  std::string start_mode_;
  bool mobile_ = false;
  // menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_;
  // performance and the look (game_look.h)
  LookState look_;
  std::unique_ptr<GameLife> glife_;  // M3 life (last: destroyed before world_)
};

Game& game();

}  // namespace sc
