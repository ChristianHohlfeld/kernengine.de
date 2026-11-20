// Ragdolls (see ragdoll.h).
#include "ragdoll.h"

#include <cmath>

namespace sc {

namespace {
using kern::Mat4;
namespace ph = kern::physics;

// world_person_ragdoll.mjs
constexpr f32 kLife = 7.0f;            // LIFE_MS (a body that does not get up)
constexpr f32 kGetup = 1.15f;          // GETUP_MS
constexpr f32 kRecoverAfter = 2.6f;    // recover.afterMs default
constexpr f32 kSettleSpeed = 0.06f, kSettleTime = 0.35f;  // settledMs > 350 at < 0.06 m/s
constexpr f32 kFriction = 8;           // createHuman frictionTorque: 8
constexpr f32 kNoise = 0.65f;          // per-joint velocity noise

// Rest pose (world_person_ragdoll.mjs REST, x lateral there; here +X forward, +Y left, Z up).
// Capsules: radius, half length incl. caps along Z.
struct BoneDef { Vec3 center; Vec3 half; bool capsule; };
const BoneDef kDefs[Ragdolls::kBones] = {
    {{0, 0, 0.98f}, {0.11f, 0.17f, 0.11f}, false},      // pelvis (box)
    {{0, 0, 1.30f}, {0.14f, 0.14f, 0.24f}, true},       // chest
    {{0, 0, 1.74f}, {0.11f, 0.11f, 0.11f}, true},       // head (sphere)
    {{0, 0.225f, 1.35f}, {0.05f, 0.05f, 0.17f}, true},  // upper arms
    {{0, -0.225f, 1.35f}, {0.05f, 0.05f, 0.17f}, true},
    {{0, 0.225f, 1.065f}, {0.045f, 0.045f, 0.16f}, true},  // lower arms
    {{0, -0.225f, 1.065f}, {0.045f, 0.045f, 0.16f}, true},
    {{0, 0.1f, 0.715f}, {0.07f, 0.07f, 0.24f}, true},   // thighs
    {{0, -0.1f, 0.715f}, {0.07f, 0.07f, 0.24f}, true},
    {{0, 0.1f, 0.27f}, {0.06f, 0.06f, 0.24f}, true},    // shins
    {{0, -0.1f, 0.27f}, {0.06f, 0.06f, 0.24f}, true},
};
// parent, joint anchor (rest), kind (0 spherical cone, 1 hinge about the lateral axis), limits
struct JointDef { i32 parent; Vec3 anchor; u8 kind; f32 lo, hi; };
const JointDef kJoints[Ragdolls::kBones] = {
    {-1, {}, 0, 0, 0},
    {Ragdolls::pelvis, {0, 0, 1.08f}, 0, 0, 0.5f},           // waist
    {Ragdolls::chest, {0, 0, 1.60f}, 0, 0, 0.6f},            // neck
    {Ragdolls::chest, {0, 0.225f, 1.50f}, 0, 0, 1.6f},       // shoulders
    {Ragdolls::chest, {0, -0.225f, 1.50f}, 0, 0, 1.6f},
    {Ragdolls::upper_arm_l, {0, 0.225f, 1.20f}, 1, -2.4f, 0.05f},  // elbows bend forward
    {Ragdolls::upper_arm_r, {0, -0.225f, 1.20f}, 1, -2.4f, 0.05f},
    {Ragdolls::pelvis, {0, 0.1f, 0.94f}, 0, 0, 1.2f},        // hips
    {Ragdolls::pelvis, {0, -0.1f, 0.94f}, 0, 0, 1.2f},
    {Ragdolls::thigh_l, {0, 0.1f, 0.49f}, 1, -0.05f, 2.4f},  // knees bend backward
    {Ragdolls::thigh_r, {0, -0.1f, 0.49f}, 1, -0.05f, 2.4f},
};

f32 hash_noise(u64 seed, u32 i) {
  u32 x = static_cast<u32>(seed ^ (seed >> 32)) + (i + 1) * 0x9e3779b1u;
  x ^= x >> 16; x *= 0x7feb352du; x ^= x >> 15; x *= 0x846ca68bu; x ^= x >> 16;
  return static_cast<f32>(x) / 4294967295.0f * 2 - 1;
}
f32 smooth(f32 t) { t = clampf(t, 0, 1); return t * t * (3 - 2 * t); }
}  // namespace

Vec3 Ragdolls::rest_center(u32 b) { return kDefs[b].center; }
Vec3 Ragdolls::rest_half(u32 b) { return kDefs[b].half; }

i32 Ragdolls::find(u64 key) const {
  for (std::size_t i = 0; i < dolls_.size(); ++i)
    if (dolls_[i].active && dolls_[i].key == key) return static_cast<i32>(i);
  return -1;
}

void Ragdolls::release(Doll& d) {
  if (!world_) return;
  for (u32 b = 0; b < kBones; ++b) {
    if (d.joints[b].valid() && world_->joint_valid(d.joints[b])) world_->destroy_joint(d.joints[b], false);
    d.joints[b] = {};
  }
  for (u32 b = 0; b < kBones; ++b) {
    if (d.bodies[b].valid() && world_->body_valid(d.bodies[b])) world_->destroy_body(d.bodies[b]);
    d.bodies[b] = {};
  }
}

void Ragdolls::clear() {
  for (Doll& d : dolls_) { release(d); d.active = false; }
  dolls_.clear();
  ups_.clear();
}

void Ragdolls::remove(u64 key) {
  const i32 i = find(key);
  if (i < 0) return;
  release(dolls_[i]);
  dolls_[i].active = false;
}

bool Ragdolls::spawn(u64 key, Vec3 feet, f32 yaw, Vec3 velocity, const Colors& colors, bool recover, f32 now) {
  if (!world_) return false;
  remove(key);
  i32 at = -1;  // (the doll it becomes: its place in dolls_)
  for (std::size_t i = 0; i < dolls_.size(); ++i)
    if (!dolls_[i].active) { at = static_cast<i32>(i); break; }
  if (at < 0 && dolls_.size() < max_) { dolls_.push_back(Doll{}); at = static_cast<i32>(dolls_.size()) - 1; }
  if (at < 0) {
    // The pool is full: the oldest goes (one that was going to get up does so right now).
    std::size_t old = 0;
    for (std::size_t i = 0; i < dolls_.size(); ++i)
      if (dolls_[i].born < dolls_[old].born) old = i;
    if (dolls_[old].recover) {
      Vec3 p; f32 y;
      where(dolls_[old].key, p, y);
      ups_.push_back({dolls_[old].key, {p.x, p.y, feet.z}, y});
    }
    release(dolls_[old]);
    at = static_cast<i32>(old);
  }
  Doll& d = dolls_[static_cast<std::size_t>(at)];
  d = Doll{};
  d.active = true;
  d.key = key;
  d.recover = recover;
  d.colors = colors;
  d.born = now;
  d.group = -(1 + (group_++ % 30000));  // one negative group per body: its bones never collide with each other
  ++spawned_;
  const Quat q = yaw_quat(yaw);
  for (u32 b = 0; b < kBones; ++b) {
    const BoneDef& def = kDefs[b];
    ph::BodySettings bs;
    bs.type = ph::BodyType::dynamic;
    bs.position = feet + q.rotate(def.center);
    bs.rotation = q;
    const f32 boost = b == chest || b == head ? 1.15f : (b == lower_arm_l || b == lower_arm_r || b == shin_l || b == shin_r) ? 1.28f : 1.0f;
    bs.linear_velocity = {velocity.x * boost + hash_noise(key, b * 3) * kNoise, velocity.y * boost + hash_noise(key, b * 3 + 1) * kNoise,
                          velocity.z * boost + 1.1f + std::fabs(hash_noise(key, b * 3 + 2)) * 0.9f};
    bs.angular_velocity = {hash_noise(key, 40 + b) * 2, hash_noise(key, 60 + b) * 2, hash_noise(key, 80 + b)};
    bs.linear_damping = 0.05f;
    bs.angular_damping = 0.4f;
    d.bodies[b] = world_->create_body(bs);
    ph::ShapeSettings ss;
    if (b == head) { ss.kind = ph::ShapeKind::sphere; ss.radius = def.half.x; }
    else if (def.capsule) { ss.kind = ph::ShapeKind::capsule; ss.radius = def.half.x; ss.length = 2 * (def.half.z - def.half.x); }
    else { ss.kind = ph::ShapeKind::box; ss.size = def.half * 2.0f; }
    ss.material.density = 1000;  // about water: a 70 kg person
    ss.material.friction = 0.72f;
    ss.material.restitution = 0.05f;
    ss.material.group_index = d.group;
    ss.material.category_bits = kCatWorld;
    world_->create_shape(d.bodies[b], ss);
  }
  for (u32 b = 1; b < kBones; ++b) {
    const JointDef& j = kJoints[b];
    ph::JointSettings js;
    js.body_a = d.bodies[j.parent];
    js.body_b = d.bodies[b];
    const Vec3 la = j.anchor - kDefs[j.parent].center, lb = j.anchor - kDefs[b].center;
    if (j.kind == 1) {
      js.type = ph::JointType::revolute;
      js.frame_a = ph::frame_from_axis(la, {0, 1, 0}, false);
      js.frame_b = ph::frame_from_axis(lb, {0, 1, 0}, false);
      js.enable_limit = true;
      js.lower = j.lo;
      js.upper = j.hi;
      js.enable_motor = true;   // joint friction
      js.motor_speed = 0;
      js.max_motor_torque = kFriction * 0.5f;
    } else {
      js.type = ph::JointType::spherical;
      // the cone opens along the bone (down from shoulders and hips, up from waist and neck)
      const Vec3 axis = b == chest || b == head ? Vec3{0, 0, 1} : Vec3{0, 0, -1};
      js.frame_a = ph::frame_from_axis(la, axis, false);
      js.frame_b = ph::frame_from_axis(lb, axis, false);
      js.enable_limit = true;
      js.upper = j.hi;
      js.enable_motor = true;
      js.max_motor_torque = kFriction * 0.5f;
    }
    d.joints[b] = world_->create_joint(js);
  }
  for (u32 b = 0; b < kBones; ++b) d.pose[b] = world_->pose(d.bodies[b]);
  return true;
}

void Ragdolls::begin_getup(Doll& d, f32 now) {
  // Facing: across the hips (the body's left axis), feet under the pelvis.
  for (u32 b = 0; b < kBones; ++b) d.from[b] = d.pose[b];
  const Vec3 left = d.pose[pelvis].rotation.rotate({0, 1, 0});
  f32 yaw = std::atan2(left.y, left.x) - kPi / 2;
  if (!std::isfinite(yaw)) yaw = 0;
  const Vec3 pp = d.pose[pelvis].position;
  d.stand_yaw = yaw;
  d.stand_feet = {pp.x, pp.y, pp.z - 0.12f - kDefs[pelvis].center.z * 0.0f};
  // The feet: the lowest bone's bottom (lying, the pelvis is ~0.12 m over the ground).
  f32 low = 1e9f;
  for (u32 b = 0; b < kBones; ++b) low = std::fmin(low, d.pose[b].position.z - kDefs[b].half.x);
  d.stand_feet.z = low;
  const Quat q = yaw_quat(yaw);
  for (u32 b = 0; b < kBones; ++b) d.stand[b] = {d.stand_feet + q.rotate(kDefs[b].center), q};
  release(d);
  d.getting_up = true;
  d.getup_at = now;
}

void Ragdolls::update(f32 dt, f32 now) {
  (void)dt;
  for (Doll& d : dolls_) {
    if (!d.active) continue;
    if (d.getting_up) {
      const f32 t = (now - d.getup_at) / kGetup;
      // through a kneel: the pelvis stays low for the first half
      const f32 k = smooth(t);
      for (u32 b = 0; b < kBones; ++b) {
        Vec3 p = d.from[b].position + (d.stand[b].position - d.from[b].position) * k;
        if (t < 0.45f) p.z = d.from[b].position.z + (d.stand_feet.z + kDefs[b].center.z * 0.55f - d.from[b].position.z) * smooth(t / 0.45f);
        else p.z = d.stand_feet.z + kDefs[b].center.z * (0.55f + 0.45f * smooth((t - 0.45f) / 0.55f));
        d.pose[b] = {p, slerp(d.from[b].rotation, d.stand[b].rotation, k)};
      }
      if (t >= 1) {
        ups_.push_back({d.key, d.stand_feet, d.stand_yaw});
        d.active = false;
      }
      continue;
    }
    f32 vmax = 0;
    for (u32 b = 0; b < kBones; ++b) {
      d.pose[b] = world_->pose(d.bodies[b]);
      if (world_->awake(d.bodies[b])) vmax = std::fmax(vmax, kern::length(world_->linear_velocity(d.bodies[b])));
    }
    d.settled = vmax < kSettleSpeed ? d.settled + dt : 0;
    if (d.recover) {
      if (d.settled > kSettleTime || now - d.born > kRecoverAfter + 2) begin_getup(d, now);
    }
    // A body that does not get up stays (Box3D puts it to sleep) until its person is gone
    // (People's body time) or the pool needs it; kLife is SpawnCity's minimum lifetime.
    (void)kLife;
  }
}

void Ragdolls::blast(Vec3 at, f32 radius, f32 strength) {
  if (!world_) return;
  for (Doll& d : dolls_) {
    if (!d.active || d.getting_up) continue;
    for (u32 b = 0; b < kBones; ++b) {
      if (!world_->body_valid(d.bodies[b]) || world_->body_type(d.bodies[b]) != ph::BodyType::dynamic) continue;
      const Vec3 p = world_->pose(d.bodies[b]).position, dir = p - at;
      const f32 l = kern::length(dir);
      if (l > radius) continue;
      const f32 k = strength * (1 - l / radius) * world_->mass(d.bodies[b]);
      world_->apply_linear_impulse_to_center(d.bodies[b], (l > 1e-3f ? dir * (1 / l) : Vec3{0, 0, 1}) * k + Vec3{0, 0, k * 0.5f});
    }
  }
}

bool Ragdolls::where(u64 key, Vec3& p, f32& yaw) const {
  const i32 i = find(key);
  if (i < 0) return false;
  const Doll& d = dolls_[i];
  p = d.pose[pelvis].position;
  const Vec3 up = d.pose[chest].position - d.pose[pelvis].position;
  yaw = std::atan2(up.y, up.x);
  return true;
}

bool Ragdolls::physical(u64 key) const {
  const i32 i = find(key);
  return i >= 0 && !dolls_[i].getting_up;
}

u32 Ragdolls::active() const {
  u32 n = 0;
  for (const Doll& d : dolls_) n += d.active ? 1 : 0;
  return n;
}

void Ragdolls::draw(std::vector<Draw>& out) const {
  for (u32 i = 0; i < dolls_.size(); ++i) {
    const Doll& d = dolls_[i];
    if (!d.active) continue;
    for (u32 b = 0; b < kBones; ++b) {
      const Vec3 h = kDefs[b].half;
      const Vec3 tint = b == head || b == lower_arm_l || b == lower_arm_r ? d.colors.skin
                        : (b == chest || b == upper_arm_l || b == upper_arm_r) ? d.colors.shirt : d.colors.pants;
      out.push_back({Mat4::trs(d.pose[b].position, d.pose[b].rotation, h * 2.0f), tint, b == head, static_cast<u8>(b), i, Mat4::trs(d.pose[b].position, d.pose[b].rotation, {1, 1, 1})});
    }
  }
}

}  // namespace sc
