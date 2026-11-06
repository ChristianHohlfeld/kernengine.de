// Ragdolls (SpawnCity world_person_ragdoll.mjs, people_impacts.mjs): a person who is shot, blown
// up or run over falls as a Box3D body - eleven capsules / boxes (pelvis, chest, head, upper and
// lower arms, thighs, shins) held by limited joints with joint friction, like Erin Catto's human
// that SpawnCity uses. Box3D is used as it is (no solver settings touched): the joints carry
// cone and hinge limits and a friction motor (zero speed, a small torque).
//
// "Reverse ragdoll": somebody who still has life gets up again. Once the body has settled (or after
// a while) the bodies go and the bones blend from where they lie into the standing pose; the
// person walks on from exactly there.
#pragma once

#include <vector>

#include "common.h"
#include "kern/math/math.h"

namespace sc {

class Ragdolls {
 public:
  enum Bone : u8 { pelvis, chest, head, upper_arm_l, upper_arm_r, lower_arm_l, lower_arm_r, thigh_l, thigh_r, shin_l, shin_r, kBones };
  struct Colors { Vec3 shirt{0.35f, 0.44f, 0.53f}, pants{0.17f, 0.19f, 0.22f}, skin{0.82f, 0.66f, 0.53f}; };
  /// A person who stood up again (reverse ragdoll): where and facing which way.
  struct Up { u64 key; Vec3 feet; f32 yaw; };

  ~Ragdolls() { clear(); }
  void init(kern::physics::World& w, bool mobile) { world_ = &w; max_ = mobile ? 6 : 10; }  // MAX_RAGDOLLS
  /// Throws a person standing at `feet` (facing `yaw`) with `velocity` (m/s). `recover`: the person
  /// gets up again when the body has settled. Reuses the oldest body when the pool is full.
  bool spawn(u64 key, Vec3 feet, f32 yaw, Vec3 velocity, const Colors& colors, bool recover, f32 now);
  void update(f32 dt, f32 now);
  /// A blast pushes every body in range (world_person_ragdoll: the bodies are in the same world).
  void blast(Vec3 at, f32 radius, f32 strength);
  void remove(u64 key);
  void clear();
  bool has(u64 key) const { return find(key) >= 0; }
  /// The pelvis of a body (where it lies), false when there is none.
  bool where(u64 key, Vec3& pelvis, f32& yaw) const;
  void take_ups(std::vector<Up>& out) { out.insert(out.end(), ups_.begin(), ups_.end()); ups_.clear(); }
  u32 active() const;
  u32 spawned() const { return spawned_; }
  bool physical(u64 key) const;  // still Box3D bodies (not getting up)
  /// A bone's body (VR grab: the hand takes the doll by its chest), invalid when there is none.
  kern::physics::Body bone_body(u64 key, Bone b) const { const i32 i = find(key); return i >= 0 && dolls_[i].active && !dolls_[i].getting_up ? dolls_[i].bodies[b] : kern::physics::Body{}; }

  /// Drawing: every active body's bones as box transforms (unit box scaled to the bone) + tint.
  struct Draw { kern::Mat4 m; Vec3 tint; bool sphere; };
  void draw(std::vector<Draw>& out) const;

  /// Bone sizes (rest pose, standing at the origin facing +X): centre, half extents (capsule:
  /// radius x radius x half length incl. caps).
  static Vec3 rest_center(u32 bone);
  static Vec3 rest_half(u32 bone);

 private:
  struct Doll {
    bool active = false, recover = false, getting_up = false;
    u64 key = 0;
    kern::physics::Body bodies[kBones];
    kern::physics::Joint joints[kBones];
    kern::physics::Pose pose[kBones], from[kBones], stand[kBones];
    Colors colors;
    f32 born = 0, settled = 0, getup_at = 0, stand_yaw = 0;
    Vec3 stand_feet;
    i32 group = 0;
  };
  i32 find(u64 key) const;
  void release(Doll& d);
  void begin_getup(Doll& d, f32 now);
  kern::physics::World* world_ = nullptr;
  std::vector<Doll> dolls_;
  std::vector<Up> ups_;
  u32 max_ = 10, spawned_ = 0;
  i32 group_ = 0;
};

}  // namespace sc
