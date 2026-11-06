// Buildings broken by weapons (spawncity.de nuke_destruction.mjs, "everyday destructibles"): the
// rules the player sees are the original's, the implementation is Kern's own.
//
// The rules: only weapons break buildings (rockets, grenades, air strikes, bombs, jet rockets,
// tank shells, enemy rockets; a burning car or a falling police drone does not - in the original
// that once set off chains that took whole blocks). Every building has structural points in
// proportion to its volume (at least 220); a blast within its radius takes
// power x (1 - d / r)^1.5 x 2.6 of them (a guided missile x 2.2). What is left decides the height:
// whole floors of 3.2 m break off the top, at zero the house falls to its 0.8 m rubble stump. The
// part that comes off is torn away as one piece that tips away from the blast and breaks up, or -
// for small parts - shatters at once into fracture debris along the building's own outline
// (facade panels per floor, roof slabs; at most 40 pieces a building).
//
// Kern's way (no copy of the original's code):
//  * the damage is the building's map feature (CityStream::damage_building): one truth for the
//    drawing, the collision, the minimap and the interiors; it outlives a re-streamed tile;
//  * the drawing rebuilds only the building's 160 m mesh chunk as a job and swaps it in one step;
//    the torn piece covers those frames;
//  * the collision replaces that one collider on the static body (Box3D: no teleports, no
//    disabled bodies, no filter changes - see Box3D's simulation.md);
//  * the structural points are kept against the undamaged building (the original measured the
//    already shortened one, so every hit made the house tougher again);
//  * debris is a fixed pool of 2000 boxes drawn as one instanced batch, flying ballistically and
//    landing on the real ground (the original's landed on z = 0), gone after 22 s; no Box3D bodies
//    (2000 bodies would be the cost the pool is there to avoid);
//  * torn pieces are a pool of at most 40 meshes, freed when they break; everything goes on RESET.
#pragma once

#include <unordered_map>
#include <vector>

#include "city_stream.h"
#include "combat_art.h"
#include "common.h"

namespace sc {

class Destruction {
 public:
  static constexpr u32 kPieces = 2000, kTorn = 40;
  static constexpr f32 kFloor = 3.2f, kRubble = 0.8f, kPieceLife = 22;

  void init(kern::Renderer& r) { r_ = &r; }
  /// A weapon's blast at `at` (radius m, the blast's damage); `missile`: the guided missile (x 2.2).
  /// Returns how many buildings lost height.
  u32 blast(Vec3 at, f32 radius, f32 damage, bool missile, CityStream& city, Rng& rng);
  void update(f32 dt, const CityStream& city, Rng& rng);
  void draw(CombatArt& art) const;
  /// RESET / a new city: every building whole, no debris, no points lost.
  void reset(CityStream& city);

  struct Stats { u32 pieces = 0, torn = 0, hits = 0, collapsed = 0; };
  const Stats& stats() const { return stats_; }

 private:
  // what fracture needs of a broken building (a copy: the tile may go while a piece flies)
  struct Broken {
    std::vector<std::vector<Vec2>> rings;
    Vec2 c, lo, hi;
    f32 new_top = 0, top = 0, intensity = 0;
  };
  struct Piece { Vec3 p, v, size, axis, tint; f32 yaw = 0, angle = 0, spin = 0, age = 0; bool rest = false; };
  struct Torn {
    u32 mesh = 0;
    Broken b;
    Vec3 center, dir, axis, pivot, offset;
    f32 tilt = 0, speed = 0, lift = 0, break_at = 1, age = 0;
  };
  void collapse(const Broken& b, Vec3 center, CityStream& city, Rng& rng);
  bool tear(const Broken& b, Vec3 center, Rng& rng);
  void fracture(const Broken& b, Vec3 center, Vec3 offset, f32 boost, Rng& rng);
  void spawn(Vec3 at, Vec3 v, Vec3 size, f32 yaw, Vec3 tint, Rng& rng);
  void free_torn(Torn& t);

  kern::Renderer* r_ = nullptr;
  std::unordered_map<u32, f32> points_;  // structural points left, by building seed
  std::vector<Piece> pieces_;
  std::vector<Torn> torn_;
  Stats stats_;
};

}  // namespace sc
