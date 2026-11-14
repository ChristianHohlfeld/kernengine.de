// SpawnCity on Kern - the multiplayer rules as SpawnCity plays them, as plain logic without the
// GameWorld (unit-tested in tests/test_mp.cpp):
//   * pose interpolation with a jitter buffer on the sender's clock (vs_pose_sync.mjs VsPoseTimeline)
//     and the navigation-marker smoothing for cars / jets (nav_marker_timeline.mjs)
//   * player colours (vs_multiplayer.mjs PALETTE + assignments), damage per weapon and body zone,
//     spawn protection, health regeneration, respawn offsets, group spawn (vs_group_spawn.mjs)
//   * the unanimous rule vote (rules_vote_engine.mjs, game_rules_sync.mjs)
//   * proximity voice: 8 kHz mu-law frames, gain over distance, a per-player jitter playhead
//     (voice_codec.mjs, proximity_voice.mjs)
//   * navigation: route breadcrumbs and the next turn (nav_ar_route_math.mjs), map edge arrows
//     (map_peer_edge.mjs), safe nameplate positions on phones (mobile_peer_marker_layout.mjs)
//   * ping markers (ping_marker.mjs)
//
// What is a rule of the game - who gets which colour, what a hit costs, when a hit counts, how
// health comes back, where a player comes back and joins, the vote, how far a voice carries, where
// a marker is drawn, how long a ping lasts - is a Blueprint: the level's actor "Multiplayer"
// (level/spawncity.klevel, open it in Kern Studio). Its numbers are the k... marked "the
// Blueprint's" below, read when it is bound (0 before). What moves data - the pose and navigation
// timelines, the voice codec and its streams, the routes' geometry, the scoreboard - is here.
#pragma once

#include <array>
#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::mp {

/// The running Blueprint of the actor "Multiplayer" (the game binds it once its world has
/// started; tests bind the one they run).
void bind(kern::logic::Instance* blueprint);

// ---- pose timeline (vs_pose_sync.mjs) ----------------------------------------------------------
constexpr f64 kInterpolationDelayMs = 90;    // VS_POSE_INTERPOLATION_DELAY_MS
constexpr f64 kMaxExtrapolationMs = 100;     // VS_POSE_MAX_EXTRAPOLATION_MS
constexpr f64 kStaleHoldMs = 3000;           // VS_POSE_STALE_HOLD_MS
constexpr f64 kMeshStaleMs = 25000;          // vs_multiplayer.mjs MESH_STALE_MS: a mate's craft stays through gaps
constexpr f32 kTeleportM = 35;               // vs_multiplayer.mjs: a jump > 35 m resets the timeline
constexpr u32 kMaxSnapshots = 18;

struct PoseSample {
  Vec3 p;
  Quat q;
  f64 age_ms = 0;
  bool stale = false, extrapolated = false, valid = false;
};

/// Snapshots of one remote body on the sender's clock; sampled a jitter buffer behind the estimated
/// remote "now" (interpolation between snapshots, at most 100 ms of extrapolation, then hold). The
/// buffer adapts: 90 ms on a steady link, up to 320 ms as late as the snapshots really come (the
/// relay's jitter) - so the motion is smooth, never stepping between an extrapolation and the
/// late snapshot that corrects it; it grows at once and shrinks slowly (no visible time warp).
class PoseTimeline {
 public:
  void reset() { snaps_.clear(); offsets_.clear(); lags_.clear(); have_offset_ = false; delay_ = kInterpolationDelayMs; }
  /// `has_v`: the sender's velocity is known (it is blended 70/30 with the measured one).
  bool push(Vec3 p, Quat q, Vec3 v, bool has_v, f64 source_ms, f64 received_ms);
  PoseSample sample(f64 now_ms) const;
  bool empty() const { return snaps_.empty(); }
  Vec3 last_position() const { return snaps_.empty() ? Vec3{} : snaps_.back().p; }
  f64 clock_offset_ms() const { return offset_; }
  f64 delay_ms() const { return delay_; }

 private:
  struct Snap { Vec3 p; Quat q; Vec3 v; f64 received, source; };
  std::vector<Snap> snaps_;
  std::vector<f64> offsets_;
  f64 offset_ = 0, delay_ = kInterpolationDelayMs;
  std::vector<f64> lags_;  // how late each snapshot came over the fastest one (the jitter)
  bool have_offset_ = false;
};

/// Navigation-only smoothing of a remote car / jet marker (nav_marker_timeline.mjs): 90 ms behind,
/// no prediction, a new mode or a jump over 35 m starts over.
class NavTimeline {
 public:
  void reset() { snaps_.clear(); }
  bool push(Vec3 p, f32 heading, u8 mode, f64 at_ms);
  bool sample(f64 now_ms, Vec3& p, f32& heading) const;

 private:
  struct Snap { Vec3 p; f32 heading; u8 mode; f64 at; };
  std::vector<Snap> snaps_;
};

// ---- players -----------------------------------------------------------------------------------
constexpr u32 kMaxPlayers = 9;                 // vs_multiplayer.mjs MAX_PLAYERS
/// vs_multiplayer.mjs PALETTE (0xRRGGBB): the Blueprint's list Palette.
const std::vector<u32>& palette();
inline Vec3 rgb(u32 c) { return {((c >> 16) & 255) / 255.0f, ((c >> 8) & 255) / 255.0f, (c & 255) / 255.0f}; }
/// FNV-1a over the player's id text, as vs_multiplayer.mjs hashId.
u32 hash_id(const std::string& id);
/// Colours of a roster (sorted ids, each its hash slot, the next free one on a collision).
std::vector<u32> assign_colors(const std::vector<std::string>& sorted_ids);
/// The colour of one player by himself (his hash slot).
u32 color_of(const std::string& id);

// ---- combat (vs_multiplayer.mjs) ---------------------------------------------------------------
enum class Zone : u8 { torso, head, legs };
enum class Weapon : u8 { other, smg, glock, sniper, fists, knife, launcher, rocket, grenade, drone_gun, drone_rocket, jet_gun, jet_missile, jet_bomb, explosion };
std::string weapon_label(Weapon w);   // mp_fun_hud.mjs WEAPON_LABEL (German)
std::string zone_name(Zone z);
/// Damage of one round on a mate: by weapon (smg 13, glock 26, sniper 95, fists 12, other 20)
/// times where it lands (head x3, legs x0.65), 1..100 (VS_WEAPON_DAMAGE, VS_ZONE).
f32 round_damage(Weapon w, Zone z);
/// A blast on a mate: its damage rounded to 1..100 (blastHit), nothing below 1.
f32 blast_damage(f32 damage);

// The Blueprint's SpawnProtectMs, RegenDelayMs, RegenHpPerS, RegenTickMs, RespawnCooldownMs,
// RespawnRadiusMin and RespawnRadiusMax.
extern f64 kSpawnProtectMs, kRegenDelayMs, kRegenTickMs, kRespawnCooldownMs;
extern f32 kRegenHpPerS, kRespawnRadiusMin, kRespawnRadiusMax;

/// What the authority decides for a damage request (authorityHit).
enum class HitVerdict : u8 { applied, unknown_player, friendly_fire_off, protected_target, already_down };
HitVerdict judge_hit(bool target_known, bool shooter_known, bool self_damage, bool friendly_fire, f64 now_ms, f64 protected_until_ms, f32 target_hp);
/// Health after `dt` seconds of regeneration (0..100 scale): nothing while down, full or hurt
/// within the last 5 s, else 12 HP/s.
f32 regen(f32 hp, f64 now_ms, f64 last_hurt_ms, f32 dt);
/// Respawn offset 3..5 m around where the player went down, uniform over the ring's area.
Vec2 respawn_offset(Rng& rng);

/// Kills and deaths per player as the authority counts them.
struct Score { u8 slot = 0; u16 kills = 0, deaths = 0; };
class Scoreboard {
 public:
  void clear() { rows_.clear(); }
  void ensure(u8 slot);
  void remove(u8 slot);
  /// A death: the victim's deaths +1, the killer's kills +1 (not for an accident or suicide).
  void died(u8 victim, i32 by);
  const std::vector<Score>& rows() const { return rows_; }
  void set(const std::vector<Score>& rows) { rows_ = rows; }
  Score of(u8 slot) const;
  /// Sorted as the board shows them: most kills first, then fewest deaths.
  std::vector<Score> ranked() const;

 private:
  std::vector<Score> rows_;
};

/// The densest connected group of active players (links of 65 m), its medoid - a joining player
/// spawns there, never at a far-away home position (vs_group_spawn.mjs groupSpawnCenter).
bool group_spawn_center(const std::vector<Vec2>& points, Vec2& out, f32 link_m = 65);
/// Where a joining player stands around the anchor: golden-angle ring, 7 / 10 / 13 m
/// (vs_multiplayer.mjs applySpawnAnchor).
Vec2 spawn_ring_offset(u32 slot);

// ---- unanimous rule vote (rules_vote_engine.mjs) -----------------------------------------------
extern f64 kBallotMs;  // the Blueprint's
struct Ballot {
  u32 id = 0;
  u8 key = 0;            // 0: friendly fire (the only game rule, world_options.mjs)
  bool value = false;
  u8 host = 0;
  std::vector<u8> members, yes;
  f64 expires_ms = 0;
  /// (the Blueprint's enum BallotStatus, in its order; ignored: what a vote did that changed
  /// nothing; invalid: no ballot)
  enum class Status : u8 { pending, approved, rejected, cancelled, ignored, invalid } status = Status::pending;
};
/// A ballot from the host over the current roster (at least two players, each once, the host
/// among them; no ballot without an id). False - and `b` invalid - when there is none.
bool create_ballot(Ballot& b, u32 id, u8 key, bool value, u8 host, std::vector<u8> members, f64 now_ms);
/// One vote; returns the ballot's status after it. `did`: what this vote did - the same, or
/// ignored for somebody who is not asked or has voted.
Ballot::Status cast_vote(Ballot& b, u8 voter, bool approve, std::vector<u8> current_members, f64 now_ms, Ballot::Status& did);
Ballot::Status cast_vote(Ballot& b, u8 voter, bool approve, std::vector<u8> current_members, f64 now_ms);
/// A ballot is over without a result: it ran out, or the roster is no longer the one that was asked.
bool ballot_stale(const Ballot& b, const std::vector<u8>& current_members, f64 now_ms);
std::string ballot_name(Ballot::Status s);

// ---- proximity voice (voice_codec.mjs, proximity_voice.mjs) -------------------------------------
constexpr u32 kVoiceRate = 8000;        // VOICE_RATE
constexpr u32 kVoiceFrame = 640;        // 80 ms per packet (proximity_voice.mjs onAudio)
extern f32 kVoiceRadius;                // VOICE_RADIUS_M: the Blueprint's
u8 encode_mulaw(f32 sample);
f32 decode_mulaw(u8 byte);
/// 1 within 5 m, 0 from 70 m (linear in between).
f32 proximity_gain(f32 distance);
/// "Ueberall hoerbar" (team radio): at least 0.85.
f32 voice_gain(f32 distance, bool everywhere);

/// Incoming voice of one player, scheduled on a playhead 30..150 ms ahead (proximity_voice.mjs
/// receive) and rendered into the game's audio output with linear resampling.
class VoiceStream {
 public:
  /// A decoded frame (8 kHz) at gain `gain`; frames older than the last sequence number are dropped.
  bool push(u32 seq, const std::vector<u8>& mulaw, f32 gain);
  /// Adds `frames` stereo frames at `rate` into `out` (interleaved). Returns true while playing.
  bool render(std::vector<f32>& out, u32 frames, u32 rate);
  bool speaking() const { return !queue_.empty(); }
  void reset() { queue_.clear(); have_seq_ = false; lead_ = 0; }

 private:
  struct Chunk { std::vector<f32> pcm; f32 gain; };
  std::vector<Chunk> queue_;
  f64 pos_ = 0;          // position in the front chunk (8 kHz samples)
  f64 lead_ = 0;         // silence still to play before the front chunk (8 kHz samples)
  u32 last_seq_ = 0;
  bool have_seq_ = false;
};

/// Microphone side: samples from the page (any rate) decimated to 8 kHz by the stride
/// (proximity_voice.mjs onAudio), cut into 640-sample frames.
class VoiceCapture {
 public:
  void push(kern::Span<const f32> samples, u32 rate);
  bool take(std::vector<u8>& frame);
  void clear() { pending_.clear(); frames_.clear(); phase_ = 0; }

 private:
  std::vector<f32> pending_;
  std::vector<std::vector<u8>> frames_;
  f64 phase_ = 0;
};

// ---- navigation (nav_ar_route_math.mjs, map_peer_edge.mjs, mobile_peer_marker_layout.mjs) ----
struct Crumb { Vec2 p; f32 bearing = 0; f32 left_m = 0; };
/// Up to `count` points ahead along the route from where the player is (spacing 18 m, first at 9 m).
std::vector<Crumb> sample_breadcrumbs(const std::vector<Vec2>& route, Vec2 position, u32 count = 5, f32 spacing = 18, f32 start = 9);
struct Turn { enum class Dir : u8 { none, left, right, u_turn } dir = Dir::none; f32 distance = 0, angle_deg = 0; };
/// The first turn of at least 38 deg within 250 m along the route (headings over 12 m).
Turn upcoming_turn(const std::vector<Vec2>& route, Vec2 position, f32 scan = 250, f32 window = 12, f32 min_angle = 38);
/// Distance along the route that is left from the player to the end.
f32 route_left(const std::vector<Vec2>& route, Vec2 position);
struct MapEdge { Vec2 p; bool offmap = false; f32 rotation_deg = 0; std::string arrow; bool valid = false; };
/// A peer on the big map: inside the drawn area as is, outside clipped to its edge with an arrow
/// toward the real bearing (insets 16 / 16 / 30 px).
MapEdge locate_map_peer(Vec2 p, f32 width, f32 height, f32 inset_x = 16, f32 inset_top = 16, f32 inset_bottom = 30);
/// A nameplate on a phone: kept off the thumb sticks (bottom) and the tool bar (top).
Vec2 safe_mobile_marker_point(Vec2 p, f32 width, f32 height, bool mobile);
/// Compass arrow for a bearing (radians clockwise from north).
std::string bearing_arrow(f32 bearing);
/// "1,2 km" / "340 m" as the map labels distances (world_map_full.mjs fmtDist).
std::string format_distance(f32 m);

// ---- ping markers (ping_marker.mjs) -------------------------------------------------------------
// The Blueprint's PingLifeMs (LIFE_MS), PingRemoveM (REMOVE_R) and PingHoldMs (HOLD_MS, touch long press).
extern f64 kPingLifeMs, kPingHoldMs;
extern f32 kPingRemoveM;
struct Ping { u8 owner = 0; Vec3 p; u32 color = 0; f64 at_ms = 0; };
class PingBoard {
 public:
  /// The local player pings at `p`: his own marker within 5 m is removed (returns false), else set.
  bool toggle(u8 owner, Vec3 p, u32 color, f64 now_ms);
  void set(u8 owner, Vec3 p, u32 color, f64 now_ms);
  void clear(u8 owner);
  void expire(f64 now_ms);
  void clear_all() { pings_.clear(); }
  const std::vector<Ping>& all() const { return pings_; }
  /// 1 while fresh, fading over the last 4 s.
  static f32 fade(const Ping& p, f64 now_ms);

 private:
  std::vector<Ping> pings_;
};

}  // namespace sc::mp
