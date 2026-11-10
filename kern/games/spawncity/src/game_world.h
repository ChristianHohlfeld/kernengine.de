// SpawnCity on Kern, milestone 3: the world around the town - state of game_world.cpp and
// game_traffic.cpp (terrain and water physics, craters and marks, traffic rules, tow service).
// Kept apart from game.h so the other modules' headers do not change with it.
#pragma once

#include <memory>
#include <vector>

#include "car.h"
#include "common.h"
#include "kern/audio/audio.h"
#include "world_art.h"
#include "world_marks.h"
#include "world_traffic.h"

namespace sc {

/// The world's own synth sounds (splash, horn): Kern's mixer, made once.
struct WorldSounds {
  bool ready = false;
  kern::audio::ClipId splash = 0, splash_small = 0, horn = 0;
};

struct WorldLife {
  // water (world_water.mjs, world_rigid_body_physics.mjs)
  bool swimming = false;
  u32 splashes = 0;
  f32 in_water[32] = {};   // per body slot: submerged fraction last step (splash on entry)
  struct Splash { Vec3 at; f32 size; };
  std::vector<Splash> pending_splashes;  // found in the fixed step, shown in the frame
  WorldSounds sounds;
  // drawing and what blasts leave behind (world_art.h, world_marks.h)
  WorldArt art;
  bool art_ready = false;
  Marks marks;
  // traffic (world_traffic.h, game_traffic.cpp)
  TrafficRules rules;
  std::vector<f32> car_limit, car_limit_at;
  std::vector<TrafficRules::View> car_view;
  std::vector<Vec3> car_positions, honks;
  std::vector<TrafficRules::Vehicle> vehicles;  // every vehicle on the road this step (the cars by index, then the tow trucks)
  u64 vehicles_step = ~0ull;
  std::vector<Vec2> walkers;
  u32 honk_count = 0, ejected = 0;
  struct Fright { i32 person; f32 at; Vec3 from; };
  std::vector<Fright> delayed_fright;
  // tow service (tow_service.mjs)
  struct Watch { f32 cooldown = 0, flip_since = -1, slow_since = -1, off_since = -1; };
  std::vector<Watch> watch;
  struct TowTruck {
    enum class State : u8 { to_job, hook, tow, depot };
    std::unique_ptr<Car> car;
    kern::physics::Body body;
    State state = State::to_job;
    i32 job = -1;
    Vec2 depot, drop;
    f32 drop_yaw = 0, born = 0, state_at = 0, path_at = -99, stuck_since = -1, recover_until = -1, recover_start = 0, hp = 110;
    std::vector<Vec2> path;
    CarInput input;
    bool towing = false, wreck_job = false, exploded = false;
    const char* reason = "";
  };
  std::vector<TowTruck> trucks;
  f32 tow_scan_at = -99;
  u32 dispatches = 0, tow_done = 0;
  const char* last_reason = "";
};

}  // namespace sc
