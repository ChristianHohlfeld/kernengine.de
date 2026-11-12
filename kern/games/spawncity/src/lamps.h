// Street lamps as SpawnCity places and breaks them (street_lamps.mjs):
// every 34 m along the streets (not paths / tracks), alternating kerbs, 1.15 m beyond the road
// edge, the arm (1.42 m) over the road, one lamp per 8 m cell. A round through the head puts it
// out; a blast tops (inner 60 %) or darkens it; a car faster than 3.5 m/s heading into the post
// shears it off, a slower one is stopped by it. Broken lamps stay broken (by cell key) until RESET.
//
// What a round, a blast and a vehicle do to one lamp - and that a lamp only ever gets worse - are
// a Blueprint: the level's actor "Lamps" (level/spawncity.klevel, open it in Kern Studio), its
// functions ShotAt, BlastReach, BlastFate, CarFate and Worse with their numbers. The Field here is
// the lamps themselves: where they stand, the grid that finds the near ones, what is broken. It
// asks the Blueprint about each lamp in reach and decides nothing itself.
#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::lamps {

/// The running Blueprint of the actor "Lamps" (the game binds it once its world has started;
/// tests bind the one they run). Unbound, nothing happens to a lamp.
void bind(kern::logic::Instance* blueprint);
kern::logic::Instance* blueprint();
/// The Blueprint's variable PoleRadius.
f32 pole_radius();

// The lamp as it is built and laid out (the model and the field, not a rule).
constexpr f32 kSpacing = 34, kPoleH = 6.2f, kGridM = 10, kArm = 1.42f, kHeadZ = kPoleH - 0.2f, kKerb = 1.15f;
constexpr f32 kPoleMass = 140;
constexpr u32 kFallenMax = 14, kMax = 900;

struct Lamp {
  Vec3 foot;
  f32 yaw = 0;      // the arm's direction (over the road)
  Vec2 head;        // under the head
  u8 state = 0;     // 0 lit, 1 head shattered (dark), 2 post down
  i64 key = 0;      // 8 m cell (round(x / 8), round(y / 8))
  u32 lane = 0;
};

i64 cell_key(f32 x, f32 y);

class Field {
 public:
  /// Places the lamps of one street (centre line, width). `ground` gives the height.
  void place_road(const std::vector<Vec2>& pts, f32 width, u32 lane, const std::function<f32(f32, f32)>& ground);
  /// The lamps of the loaded map tiles (tiles.h furnish), all at once: the field is laid anew
  /// (what was broken stays broken, by cell key). `ground` gives the height.
  struct Spot { f32 x, y, yaw; };
  void rebuild(const std::vector<Spot>& spots, const std::function<f32(f32, f32)>& ground);
  /// Lamps whose foot lies in the rect stand on the ground as it is now.
  void reground(f32 x0, f32 y0, f32 x1, f32 y1, const std::function<f32(f32, f32)>& ground);
  void clear() { lamps_.clear(); seen_.clear(); grid_.clear(); }
  std::vector<Lamp>& all() { return lamps_; }
  const std::vector<Lamp>& all() const { return lamps_; }
  /// Lamps (indices) within r of (x, y).
  std::vector<i32> near(f32 x, f32 y, f32 r) const;
  /// setState: only ever worse. Returns true when it changed; remembers it for re-placement.
  bool set_state(i32 i, u8 state);
  /// The lit head nearest along the segment a -> b within 0.38 m (onTracer), or -1.
  i32 shot(Vec3 a, Vec3 b) const;
  struct BlastHit { i32 lamp; u8 state; Vec2 dir; f32 speed; };
  /// onExplosion: r = max(3, radius); inside 0.6 r the post goes down, else the head.
  std::vector<BlastHit> blast(Vec3 p, f32 radius) const;
  /// checkCars: a post inside the vehicle's footprint (half length / width). Fills what happens.
  struct CarHit { i32 lamp = -1; bool topple = false; Vec3 impulse; bool clang = false; };
  std::vector<CarHit> car(Vec3 pos, f32 yaw, Vec3 vel, f32 half_len, f32 half_w, f32 mass) const;
  void reset_broken() { broken_.clear(); for (Lamp& l : lamps_) l.state = 0; }
  /// Every broken lamp there is, placed here or not (cell key, state), sorted by key - the state a
  /// player who joins a session gets (game_townsync.cpp).
  std::vector<std::pair<i64, u8>> broken_list() const;
  /// A lamp broken on another machine: remembered by its cell (for when its tile is placed here)
  /// and set on the lamp standing there now. Only ever worse; true when something changed.
  bool merge_broken(i64 key, u8 state);
  std::size_t broken() const { return broken_.size(); }

 private:
  std::vector<Lamp> lamps_;
  std::unordered_set<i64> seen_;
  std::unordered_map<i64, u8> broken_;
  std::unordered_map<i64, std::vector<i32>> grid_;
};

}  // namespace sc::lamps
