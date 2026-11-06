// Procedural people (SpawnCity crowd_characters.mjs draws its crowd from simple parts too): legs,
// torso, arms and head as instanced Kern meshes, tinted per person, with a walking swing; officers
// wear a cap; bodies lie flat. Police light bars on the cruisers.
#pragma once

#include <vector>

#include "kern/gfx/renderer.h"
#include "people.h"

namespace sc {

class PeopleArt {
 public:
  bool init(kern::Renderer& r);
  void begin();
  void person(const People::Person& p, f32 now);
  /// A light bar on a car roof (chassis frame), flashing when `on`.
  void light_bar(const kern::Mat4& chassis, bool on, f32 now);
  /// A muzzle flash / tracer stub is Kern's (ShotEvent); this is a burning wreck's glow.
  void fire(Vec3 at, f32 size, f32 now);
  /// A police drone (dark quad, blue / red strobes; `lights` off when knocked out).
  void police_drone(Vec3 at, f32 yaw, f32 tilt_x, f32 tilt_y, bool lights, f32 now, u32 index);
  /// A projectile: grenade, rocket or bomb (oriented along `dir`, length in metres).
  void projectile(Vec3 at, Vec3 dir, f32 radius, f32 length, bool flame);
  /// A street lamp on the pavement (head lit by `night` 0..1).
  void street_lamp(Vec3 foot, f32 arm_yaw, f32 night);
  void end();

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
    bool tinted = false;
  };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m, bool tinted);
  void put(Part& p, const kern::Mat4& m, Vec3 tint = {1, 1, 1});
  kern::Renderer* r_ = nullptr;
  Part leg_, torso_, arm_, head_, hand_, hair_, cap_, bar_, lamp_, flame_, pd_body_, pd_rotor_, shell_, pole_, lamp_head_;
};

}  // namespace sc
