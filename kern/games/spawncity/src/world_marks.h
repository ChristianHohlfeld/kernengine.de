// What blasts leave behind (SpawnCity terrain_craters.mjs, world_impacts.mjs, facade_marks.mjs):
//  * a crater in the terrain for the big ones (rockets, grenades, bombs): the bowl-and-rim
//    profile of craterProfile, scaled to the blast - physics, ground mesh and the streets on it
//    follow (Terrain::add_crater re-lays them)
//  * a scorch on the ground, draped on the terrain (addScorch: 40 at most)
//  * soot on the walls within reach, clipped to the wall (addWallSoot: 48, two walls a blast)
//  * a breach in the wall that opens further with every blast on the same 2 m patch, and concrete
//    chunks flying out as Box3D bodies for 25 s (addWallBreach / spawnRubble: 64 breaches, 36 chunks)
//  * a spider-web crack on a window pane a bullet hit (addGlassCrack: 160; the glass never breaks)
#pragma once

#include <vector>

#include "city_stream.h"
#include "world_art.h"

namespace sc {

class Marks {
 public:
  struct Stats { u32 scorches = 0, soots = 0, breaches = 0, rubble = 0, craters = 0; };
  void clear(kern::physics::World* w);
  /// A blast at `at`, radius `r`; `strength` ~ damage / 110 (facade_marks.mjs).
  void blast(Vec3 at, f32 r, f32 strength, CityStream& city, kern::physics::World& w, Rng& rng, f32 now, bool craters = true);
  /// A bullet that hit `p` (world): on a window pane of a building it cracks the glass.
  bool bullet(Vec3 p, const CityStream& city, Rng& rng);
  /// `city`: the town the blasts and bullets were in (marks go with the walls they sit on).
  void update(f32 now, const CityStream& city, kern::physics::World& w);
  void draw(WorldArt& art, const CityStream& city, kern::physics::World& w, f32 now);
  const Stats& stats() const { return stats_; }

 private:
  struct Scorch { Vec2 c; f32 r = 0; u32 seed = 0; };
  struct WallMark { Vec3 c; Vec2 n, d; f32 size = 0, level = 0; u32 seed = 0; bool breach = false; u64 cell = 0; };
  struct Chunk { kern::physics::Body body; Vec3 half; f32 until = 0; };
  void rebuild(const CityStream& city);
  void add_soot(Vec3 at, f32 r, const std::vector<CityStream::Wall>& walls, const Terrain& t, Rng& rng);
  void add_breach(Vec3 at, f32 r, f32 strength, const std::vector<CityStream::Wall>& walls, const Terrain& t, kern::physics::World& w, Rng& rng, f32 now);
  void spawn_rubble(Vec3 at, Vec2 n, i32 count, kern::physics::World& w, Rng& rng, f32 now);
  void prune_walls(const CityStream& city);  // marks whose wall is gone (a floor blown off, a house down) go with it

  std::vector<Scorch> scorches_;
  std::vector<WallMark> soots_, breaches_, cracks_;
  u32 crack_cursor_ = 0;
  std::vector<Chunk> rubble_;
  u32 scorch_cursor_ = 0, soot_cursor_ = 0, breach_cursor_ = 0, rubble_cursor_ = 0;
  std::vector<CityStream::Wall> near_;
  u32 ground_mesh_[3] = {}, wall_mesh_ = 0;
  bool dirty_ = false;
  u32 damage_seen_ = 0;  // CityStream::damage_serial at the last prune
  Stats stats_;
  kern::MeshData ground_data_[3], wall_data_;  // scorches: the char, then two fading rims
  bool struck_ = false;  // a blast or a bullet was here: there may be marks to prune and to draw
};

}  // namespace sc
