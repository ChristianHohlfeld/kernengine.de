// Procedural people (SpawnCity crowd_characters.mjs draws its crowd from simple parts too): legs,
// torso, arms and head as instanced Kern meshes, tinted per person, with a walking swing; officers
// wear a cap; bodies lie flat. Police light bars on the cruisers.
#pragma once

#include <vector>

#include "kern/gfx/renderer.h"
#include "people.h"
#include "ragdoll.h"

namespace sc {

class PeopleArt {
 public:
  bool init(kern::Renderer& r);
  void begin();
  void person(const People::Person& p, f32 now);
  /// The fallen as ragdolls (ragdoll.cpp bones): the same rounded body, face and hair as standing.
  void ragdolls(const std::vector<Ragdolls::Draw>& bones);
  /// A person's hair colour (the dead's dull, a zombie's grey-green).
  static Vec3 hair_color(const People::Person& p);
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
  /// A mate's beacon: a thin pillar of his colour rising over him (seen from afar, MP).
  void beacon(Vec3 foot, Vec3 color, f32 alpha);
  void end();

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
    bool tinted = false;
  };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m, bool tinted);
  void put(Part& p, const kern::Mat4& m, Vec3 tint = {1, 1, 1});
  /// A part's instances: none at the start of a frame (begin), handed to the renderer at its end (end).
  static void clear(Part& p);
  void upload(Part& p);
  kern::Renderer* r_ = nullptr;
  Part leg_, torso_, arm_, head_, hand_, hair_, vest_, door_, cap_, bar_, lamp_, flame_, pd_body_, pd_rotor_, shell_, pole_, lamp_head_;
  Part rd_pelvis_, rd_chest_, rd_upper_, rd_fore_, rd_thigh_, rd_shin_;  // a ragdoll's bones, rounded
  Part beacon_;
  Part gun_pistol_, gun_smg_, gun_sniper_, gun_tube_, gun_rocket_, gun_knife_;  // a mate's weapon in his hand (barrel along +x)
};

}  // namespace sc
