// SpawnCity's equipment as pure logic (tests/test_equipment.cpp):
//   * Jetpack (player_walk_mode_v4.mjs, WORLD option): jump again in the air = a thrust burst, hold
//     jump to keep flying, fuel recharges, a cushion fires before a hard landing.
//   * FlightLog (flight_logbook.mjs): every drone flight from ARM to DISARM with distance, top
//     speed, heights, battery and a path; network events of the session.
//   * MusicSynth (music_player.mjs, car_radio.mjs): the title music and the car radio as
//     procedural synth programmes - SpawnCity streams a music file and real radio stations; here
//     everything is generated (no foreign music).
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc {

/// The jetpack. Its rules are a Blueprint: the level's actor "Jetpack" (level/spawncity.klevel) -
/// thrust, fuel and times as variables, the functions Ignite and Update. Here is a player's pack
/// (the Blueprint's struct Pack) and how the game's C++ asks.
class Jetpack {
 public:
  /// The running Blueprint of the actor "Jetpack" (the game binds it once its world has started;
  /// tests bind the one they run).
  static void bind(kern::logic::Instance* blueprint);
  // The Blueprint's variables Acc, AirAccel, MaxClimb, Gravity, FallSafe and IgnitionDamp.
  static f32 acc();
  static f32 air_accel();
  static f32 max_climb();
  static f32 gravity();
  static f32 fall_safe();
  /// vz is scaled by this at ignition (a falling player is caught: `jump.vz *= .35`).
  static f32 ignition_damp();
  /// The second JUMP press in the air: ignition (false without fuel).
  bool ignite(f32 vz);
  /// One frame. `airborne`, vertical speed, height of the feet above what is below, jump held.
  /// Returns the upward acceleration of the thrust this frame (0 = not burning).
  f32 update(f32 dt, f32 now, bool airborne, f32 vz, f32 height, bool hold);
  f32 fuel = 1;
  bool thrusting = false, used = false;
  u32 ignitions = 0;

 private:
  /// The Blueprint's struct Pack, as it takes and gives it.
  struct Pack {
    f64 fuel = 1;
    bool thrusting = false, used = false;
    u32 ignitions = 0;
    f64 burst_left = 0, ground_since = -1, last_thrust = -99;
  };
  Pack pack() const;
  void take(const Pack& p);
  f32 burst_left_ = 0, ground_since_ = -1, last_thrust_ = -99;
};

struct FlightSample {
  f32 t = 0;            // simulation time
  Vec3 pos, vel;
  f32 agl = -1, volts = 0;
  bool armed = false;
};

struct Flight {
  u32 id = 0;
  f32 start = 0, duration = 0, distance = 0, max_speed = 0, max_agl = 0, max_alt = 0, volts_start = 0, volts_end = 0, volts_min = 0;
  std::string reason;
  std::vector<Vec3> path;  // x, y, agl every 0.5 s (at most 600)
};

class FlightLog {
 public:
  static constexpr u32 kMaxFlights = 50, kMaxPath = 600, kMaxEvents = 120;
  static constexpr f32 kPathInterval = 0.5f;
  void observe(const FlightSample& s, const char* disarm_reason = "FC_DISARM");
  /// Ends a running flight (drone stowed, crash, reset).
  void finish(const char* reason);
  void network(const std::string& stage, f32 t);
  const std::vector<Flight>& flights() const { return flights_; }  // newest first
  bool flying() const { return active_; }
  u32 serial() const { return serial_; }  // counts finished flights and events
  std::string json() const;

 private:
  void record(const FlightSample& s, bool force);
  std::vector<Flight> flights_;
  struct Event { f32 t; std::string stage; };
  std::vector<Event> events_;
  Flight cur_;
  bool active_ = false, have_last_ = false;
  Vec3 last_;
  f32 last_t_ = 0, last_path_ = -1e9f;
  u32 next_id_ = 1, serial_ = 0;
};

class MusicSynth {
 public:
  struct Station { const char* name; const char* genre; f32 bpm; u32 seed; };
  static constexpr i32 kMenu = -1;
  static const Station kStations[];
  static i32 station_count();

  void init(u32 rate);
  /// The programme to play (kMenu, a station index, or -2 = silence) and its level 0..1.
  void select(i32 programme, f32 level);
  /// Adds `frames` stereo frames into `out` (interleaved).
  void render_add(std::vector<f32>& out, u32 frames);
  i32 programme() const { return programme_; }
  f32 level() const { return gain_; }
  f32 peak = 0;  // of the last block (tests)

 private:
  struct Voice { f32 phase = 0, freq = 0, env = 0, decay = 0.9995f, amp = 0, phase2 = 0; };
  void step();
  f32 next_sample(f32& right);
  u32 rate_ = 0;
  i32 programme_ = -2, want_ = -2;
  f32 gain_ = 0, target_ = 0, fade_ = 1;  // fade: 1 = the current programme fully in
  u64 sample_ = 0;
  u32 step_ = 0, samples_per_step_ = 1, until_step_ = 0, rng_ = 1;
  Voice kick_, snare_, hat_, bass_, lead_, pad_[3];
  f32 lp_ = 0, bass_lp_ = 0, echo_[24000] = {};
  u32 echo_at_ = 0;
};

}  // namespace sc
