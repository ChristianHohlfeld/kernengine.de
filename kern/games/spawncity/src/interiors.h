// Walkable buildings (SpawnCity building_interiors.mjs). Every building of the map can be entered
// on foot through its front door(s): storeys at the facade's storey height, a switchback stairwell
// from the lobby up to the roof, furnished floors, window openings that look out onto the real
// city, ceiling lights and a stair house on the roof.
//
// Only ONE interior exists at a time - the building the player is in or walking up to (built when
// it becomes the nearest one within 30 m). Nearby buildings show their closed front doors. The
// interior brings its own static Box3D collision (outer and inner walls with the door openings,
// floors, landings, stair ramps, the roof with its hatch, furniture); the streamed city leaves
// that one building out of its collision while it is active (CityStream::exclude_building).
// From inside the facade's back faces are not drawn, so the window openings show the city.
#pragma once

#include <functional>
#include <string>
#include <vector>

#include "city_stream.h"
#include "common.h"
#include "kern/geo/mesh_data.h"
#include "kern/gfx/renderer.h"

namespace sc {

class LifeArt;

class Interiors {
 public:
  struct Door { Vec2 c, d, n; };  // centre on the facade, along the wall, outward normal
  struct Core { Vec2 o, u, v; };  // stair core origin, across (lanes), along (flights)
  /// A building's interior: geometry for drawing and collision, and what the game needs to know.
  struct Plan {
    std::vector<Vec2> outer, inner;
    std::vector<std::vector<Vec2>> holes, inner_holes;
    std::vector<Door> doors;
    std::vector<f32> levels;  // floor heights (the first is the lobby)
    f32 base = 0, top = 8, floor_h = 3.3f, area = 0;
    bool has_core = false, roof_access = false;
    Core core;
    kern::MeshData mesh;         // everything visible inside (vertex colours)
    std::vector<Vec3> points;    // collision triangles
    std::vector<u32> indices;
    std::vector<Vec3> lamps;     // ceiling light panels
    u32 furniture = 0;
  };
  /// Builds the plan of a building (outer ring first, then courtyards); `door_ground` is the
  /// ground height in front of the main door. False: not enterable (too small, too big, odd).
  static bool build(const MapFeatures::Building& b, f32 door_ground, Plan& out);
  /// The doors of a footprint (the longest edge, a second one on the opposite side when big).
  static std::vector<Door> doors_of(const std::vector<Vec2>& outer_ccw, f32 area);

  void init(kern::Renderer& r, kern::physics::World& w) { r_ = &r; world_ = &w; }
  void clear();
  void update(f32 dt, f32 now, Vec3 feet, bool on_foot, CityStream& city);
  void draw(LifeArt& art, f32 now, Vec3 eye) const;
  void hud(std::string& s) const;
  /// "interior" (state), "interior go" (to the nearest building's door), "interior up" (onto
  /// the next storey's stairs), "interior roof".
  bool command(const std::string& cmd, Vec3 me, std::string& out, const std::function<bool(Vec3, f32)>& teleport);
  bool active() const { return active_; }
  bool inside(Vec3 p) const;
  i32 level_at(Vec3 p) const;
  const Plan& plan() const { return plan_; }

 private:
  void activate(const CityStream::BuildingRef& ref, CityStream& city, f32 ground);
  void release(CityStream& city);
  kern::Renderer* r_ = nullptr;
  kern::physics::World* world_ = nullptr;
  bool active_ = false;
  u64 tile_ = 0;
  u32 index_ = 0;
  Plan plan_;
  u32 mesh_id_ = 0;
  kern::physics::Body body_;
  std::vector<f32> door_angle_;
  struct Closed { Vec3 at; f32 yaw; };
  std::vector<Closed> closed_;
  f32 next_scan_ = 0;
  Vec3 feet_;
  u32 entered_ = 0, built_ = 0;
  bool was_inside_ = false;
};

}  // namespace sc
