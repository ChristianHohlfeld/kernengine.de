// Geometry of the built-in shapes, collider fitting and the procedural city
// (the Orb Run generator as a level component).
#pragma once

#include <cmath>

#include <vector>

#include "kern/game/level.h"
#include "kern/geo/buildings.h"
#include "kern/geo/mesh_data.h"

namespace kern::game {

/// Small deterministic generator (xorshift32), the same sequence as Orb Run: the same on every
/// platform for a seed.
struct Rng {
  u32 s;
  explicit Rng(u32 seed = 0x9e3779b9u) : s(seed ? seed : 0x9e3779b9u) {}
  u32 next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
  f32 unit() { return static_cast<f32>(next() >> 8) * (1.0f / 16777216.0f); }
  f32 range(f32 a, f32 b) { return a + (b - a) * unit(); }
  u32 below(u32 n) { return next() % n; }
  /// Standard normal (Box-Muller): sensor noise.
  f32 gaussian() {
    if (has_spare_) { has_spare_ = false; return spare_; }
    const f32 u = std::fmax(1e-12f, unit()), v = unit();
    const f32 r = std::sqrt(-2.0f * std::log(u));
    spare_ = r * std::sin(2 * 3.14159265358979323846f * v);
    has_spare_ = true;
    return r * std::cos(2 * 3.14159265358979323846f * v);
  }

 private:
  f32 spare_ = 0;
  bool has_spare_ = false;
};

/// Mesh of a shape, centred on the actor origin, white vertex colours (the
/// material colour tints it). UVs: metres * tiling on planar faces.
MeshData shape_mesh(const MeshComponent& mesh);
/// Local bounds of a shape (centred on the origin).
Aabb shape_bounds(const MeshComponent& mesh);
/// A concrete collider (box, sphere, capsule or hull) matching a mesh.
ColliderComponent fit_collider(const MeshComponent& mesh);
/// Hull points of a shape (used for automatic colliders of cylinders, cones, ramps, tori).
std::vector<Vec3> shape_hull_points(const MeshComponent& mesh);

// ---- procedural city ----

struct CityBox {
  Vec3 center, size;  // city-local axis-aligned box
};

struct CityGeometry {
  std::vector<MeshData> buildings;  // chunks; extra stream = 4-float wall records (night windows)
  std::vector<MeshData> roads;      // chunks; welded
  MeshData plates;                  // block pavements
  MeshData trunk, crown;            // tree meshes (instanced)
  std::vector<f32> trunk_transforms, crown_transforms, crown_tints;
  std::vector<CityBox> colliders;   // buildings and pavements
  f32 half = 0;                     // half extent of the block grid
  u32 building_count = 0;
};

/// Builds the city (deterministic for a seed). Fails only for invalid parameters.
bool build_city(const CityComponent& city, CityGeometry& out);
/// A street position on the grid lines (city-local).
Vec2 city_street_point(const CityComponent& city, Rng& rng);

}  // namespace kern::game
