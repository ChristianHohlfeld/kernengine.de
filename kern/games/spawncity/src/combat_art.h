// Procedural art for combat and events: everything (attack helicopter, tank, flak gun, fighter
// jets, bombs, race gates, tracers, muzzle flashes, airbursts, fireballs, car debris) is built
// from a handful of unit primitives drawn as tinted Kern instances - SpawnCity builds the same
// models from boxes, cylinders and spheres (military_response.mjs buildHeli/buildTank,
// flak_battery.mjs buildGun, fighter_jets.mjs jetModel, race_track.mjs gateFrame).
#pragma once

#include <vector>

#include "common.h"
#include "kern/gfx/renderer.h"

namespace sc {

class CombatArt {
 public:
  enum Shape : u8 { box, sphere, cylinder, cone, glow_box, glow_sphere, smoke, kShapes };
  bool init(kern::Renderer& r);
  void begin();
  /// One primitive: unit box (1 m, centred), sphere (diameter 1), cylinder / cone (diameter 1,
  /// height 1 from z = 0 up), scaled by `m`; glow shapes are unlit (tint = HDR colour).
  void put(Shape s, const kern::Mat4& m, Vec3 tint);
  /// A glowing bar from `a` to `b` (tracers).
  void beam(Vec3 a, Vec3 b, f32 width, Vec3 tint);
  void end();

  // models (frames as in SpawnCity: heli / jet forward +Y, tank / gun forward +X, Z up)
  void heli(const kern::Mat4& m, f32 rotor, bool wreck);
  void tank(const kern::Mat4& m, f32 turret, bool wreck);
  void flak(Vec3 at, f32 base_yaw, f32 yaw, f32 pitch, bool dead, bool flash);
  void fighter(Vec3 at, f32 flame);
  void gate(Vec3 center, f32 yaw, f32 width, f32 height, Vec3 color);

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
  };
  kern::Renderer* r_ = nullptr;
  Part parts_[kShapes];
};

}  // namespace sc
