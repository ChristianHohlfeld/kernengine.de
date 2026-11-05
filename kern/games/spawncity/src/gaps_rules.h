// Pure rules of the modules sc-m3-gaps closed (PARITY.md), no GameWorld:
//  * police_sirens.mjs: the cruisers' two-tone horn and the police drones' whoop, level, air
//    absorption and Doppler,
//  * wanted_dispatch_guard.mjs / wanted_police_pre_guard.mjs: police drones arrive after the
//    dispatch delay and only see within the fire range,
//  * world_critical_damage_fx.mjs: the dark smoke discs and embers of the critical countdown,
//  * world_organic_hit_audio.mjs: the "knack" when a person or an animal dies,
//  * impact_explosion_overlay.mjs: the spark burst where a flight-fire round lands.
#pragma once

#include <string>
#include <vector>

#include "common.h"

namespace sc::gaps {

// ---- police_sirens.mjs
constexpr f32 kHornLow = 466.16f, kHornHigh = 622.25f, kHornToneS = 0.66f;  // B flat 4 / E flat 5
constexpr f32 kCarGain = 0.06f, kDroneGain = 0.032f, kAudibleM = 260, kSoundMps = 343;
constexpr f32 kWhoopBase = 760;
/// Pitch of the horn at time t (alternating tones, 0.66 s each).
f32 horn_hz(f32 t);
/// The drone whoop: 760 -> 1480 Hz over 0.7 s with an easing, then low until 1.05 s.
f32 whoop_hz(f32 t);
/// Level at distance d (0 beyond 260 m): base * level / (1 + (d / 14)^1.25).
f32 siren_gain(f32 base, f32 d, f32 level);
/// Low-pass cutoff for air absorption: 5200 - 16 d, 900 .. 5200 Hz.
f32 siren_cutoff(f32 d);

/// One positional siren voice: Doppler from the change of distance, smoothed (no chirp when the
/// nearest unit changes or teleports), as drive() in police_sirens.mjs.
struct SirenVoice {
  bool have_d = false;
  f32 last_d = 0, doppler = 1, gain = 0;
  /// d < 0: no unit in range. Returns the target gain (before the attack / release smoothing).
  f32 update(f32 d, f32 dt, f32 base, f32 level);
};

// ---- wanted_dispatch_guard.mjs / wanted_police_pre_guard.mjs
/// After the drones are revealed, a grace before the first shot: max(450, 720 - 45 * stars) ms.
f32 post_arrival_grace_ms(i32 stars);

// ---- world_critical_damage_fx.mjs
struct Puff {
  bool alive = false, ember = false;
  Vec3 pos, vel;
  f32 born = 0, expires = 0, s0 = 1, s1 = 2, opacity = 0, rotation = 0;
  u32 serial = 0;
  Vec3 color;  // linear 0..1 (0x8f949c, 0xb9bec6, ember 0xff8a3d)
};
struct SmokeEffect {
  std::string id;
  Vec3 at;                 // where the smoke comes from (world, updated by the owner each frame)
  f32 scale = 1, started = 0, expires = 0, last_emit = -1e9f;
  u32 serial = 0;
  u32 seed = 0;
};
class SmokeField {
 public:
  explicit SmokeField(u32 pool = 78) : puffs_(pool) {}
  /// startWorldCriticalDamage: returns the time it goes off (an existing effect keeps its clock).
  f32 start(const std::string& id, Vec3 at, f32 scale, f32 delay_s, f32 now);
  /// accelerateWorldCriticalDamage: never later, at least 0.18 s from now.
  bool accelerate(const std::string& id, f32 expires_at, f32 now);
  void stop(const std::string& id);
  void move(const std::string& id, Vec3 at);
  bool active(const std::string& id) const;
  /// Emits (interval 155 - 72 * progress ms, a second puff after 76 %) and ages the puffs.
  /// Expired effects are removed (their owners blow them up on their own clock).
  void update(f32 now, f32 dt);
  const std::vector<Puff>& puffs() const { return puffs_; }
  std::size_t effects() const { return effects_.size(); }
  u32 live_puffs() const;
  /// Opacity of a puff at `now` (the sine swell and fade of updatePuffs).
  static f32 opacity(const Puff& p, f32 now);
  static f32 size(const Puff& p, f32 now);

 private:
  void emit(SmokeEffect& e, f32 now);
  std::vector<SmokeEffect> effects_;
  std::vector<Puff> puffs_;
};
u32 hash_text(const std::string& s);
f32 unit_hash(u32 seed);

// ---- world_organic_hit_audio.mjs
enum class Organic : u8 { person, animal, bird };
/// Playback rate of the "crack" (bird 1.16, animal 0.94, person 1) and its gain .34 * fx.
f32 organic_rate(Organic k);
constexpr f32 kOrganicGain = 0.34f, kOrganicMinGapS = 0.024f;

// ---- visual_pose_stabilization.mjs: the drone as drawn in the follow / third camera rides on the
// camera anchor with a damped offset and a damped rotation, errors capped (soft / hard limits).
struct VisualProfile {
  f32 offset_rate, rotation_rate, soft_pos, max_pos, pos_recovery, soft_rot, max_rot, rot_recovery;
};
constexpr VisualProfile kVisualFollow{9.0f, 12.0f, 0.025f, 0.10f, 9.0f, 3 * kDeg, 10 * kDeg, 10.0f};
constexpr VisualProfile kVisualThird{5.2f, 7.0f, 0.050f, 0.18f, 7.0f, 5 * kDeg, 14 * kDeg, 7.5f};
Quat slerp(Quat a, Quat b, f32 t);
f32 quat_angle(Quat a, Quat b);
class VisualPose {
 public:
  void invalidate() { init_ = false; }
  /// mode 1 follow, 2 third. Returns the pose to draw (position, rotation).
  void update(Vec3 position, Quat rotation, Vec3 anchor, i32 mode, f32 dt, Vec3& out_p, Quat& out_q);

 private:
  bool init_ = false;
  i32 mode_ = 0;
  Vec3 offset_;
  Quat q_;
};

// ---- impact_explosion_overlay.mjs
constexpr u32 kBurstPool = 12, kBurstSparks = 9;
constexpr f32 kBurstLifeS = 0.33f;

}  // namespace sc::gaps
