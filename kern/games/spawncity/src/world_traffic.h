// Traffic rules at the junctions of the streamed streets (M3; SpawnCity's traffic has no signals -
// its drivers yield to people in their path and honk: world_procedural_population.mjs
// yieldsToPedestrian / honkCheck. The junction rules here are this game's own, see README):
//  * junctions are the street-graph nodes where three or more street arms meet
//  * junctions on a main road (an arm at least 9 m wide: secondary and up) get traffic lights,
//    two phase groups (arms along the junction's axis, arms across it), 14 s green, 3 s amber,
//    2 s all red per group (38 s cycle, offset per junction)
//  * the other junctions are first come, first served: a car inside claims the junction, the
//    others wait at its edge until it has left (right before left among cars arriving together)
//  * every driver brakes for people in its corridor (yieldsToPedestrian: reach 6 m (bus 9) +
//    0.5 s + v^2/9, half width 1.6 m (bus 1.95)) and honks when blocked (2.6 s for the player,
//    7 s for a pedestrian, then 9..16 s apart)
#pragma once

#include <unordered_map>
#include <vector>

#include "city_stream.h"

namespace sc {

class TrafficRules {
 public:
  struct Arm { Vec2 dir; f32 width = 6; };
  struct Junction {
    Vec2 p;
    std::vector<Arm> arms;
    bool lights = false;
    f32 offset = 0, radius = 8;
    Vec2 axis{1, 0};
    i32 claim = -1;
    f32 claim_at = -99;
  };
  /// What a driver sees ahead.
  struct View {
    i32 junction = -1, light = -1;   // light: 0 green, 1 amber, 2 red, -1 none
    f32 distance = 1e9f;             // to the junction's edge (stop line)
    bool wait = false, yielding = false, honk = false;
  };

  void rebuild(const std::vector<Lane>& lanes);
  u64 built_from = ~0ull;
  const std::vector<Junction>& junctions() const { return junctions_; }
  /// 0 green, 1 amber, 2 red for traffic moving along `heading` into junction j at `now`.
  i32 light(const Junction& j, Vec2 heading, f32 now) const;
  /// The highest speed car `car` may drive now (1e9: free). `cars`: positions of all cars (index
  /// = car id; z < -1e8 = not on the road); `people`: standing / walking people's feet.
  f32 limit(i32 car, Vec2 pos, Vec2 fwd, f32 speed, bool bus, f32 now, const std::vector<Vec3>& cars, const std::vector<Vec2>& people, Vec2 player,
            bool player_on_foot, View& view);
  /// Junctions near p (drawing).
  void near(Vec2 p, f32 r, std::vector<i32>& out) const;

 private:
  struct Blocked { f32 since = -1, next_honk = 0, wait_since = -1; };
  std::vector<Junction> junctions_;
  std::unordered_map<u64, std::vector<i32>> grid_;
  std::unordered_map<i32, Blocked> blocked_;
};

}  // namespace sc
