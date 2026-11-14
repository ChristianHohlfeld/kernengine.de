// Multiplayer rules (see mp_rules.h). Values from SpawnCity's sim/*.mjs, file names at each.
#include "mp_rules.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "blueprint_call.h"

namespace sc::mp {

// The Blueprint of the level's actor "Multiplayer" and its numbers (read when it is bound).
f64 kSpawnProtectMs = 0, kRegenDelayMs = 0, kRegenTickMs = 0, kRespawnCooldownMs = 0, kBallotMs = 0, kPingLifeMs = 0, kPingHoldMs = 0;
f32 kRegenHpPerS = 0, kRespawnRadiusMin = 0, kRespawnRadiusMax = 0, kVoiceRadius = 0, kPingRemoveM = 0;
namespace {
using kern::logic::Value;
rules::Blueprint g_rules;
std::vector<u32> g_palette;
f32 num(const char* function, const std::vector<Value>& args) { return static_cast<f32>(g_rules.number(function, args)); }
Vec2 pair(const char* function, const std::vector<Value>& args) {
  std::vector<Value> r;
  if (!g_rules.call(function, args, r) || r.size() < 2) return {};
  return {static_cast<f32>(r[0].as_number()), static_cast<f32>(r[1].as_number())};
}
}  // namespace

void bind(kern::logic::Instance* blueprint) {
  g_rules.running = blueprint;
  g_palette.clear();
  kSpawnProtectMs = g_rules.variable("SpawnProtectMs");
  kRegenDelayMs = g_rules.variable("RegenDelayMs");
  kRegenTickMs = g_rules.variable("RegenTickMs");
  kRespawnCooldownMs = g_rules.variable("RespawnCooldownMs");
  kBallotMs = g_rules.variable("BallotMs");
  kPingLifeMs = g_rules.variable("PingLifeMs");
  kPingHoldMs = g_rules.variable("PingHoldMs");
  kRegenHpPerS = static_cast<f32>(g_rules.variable("RegenHpPerS"));
  kRespawnRadiusMin = static_cast<f32>(g_rules.variable("RespawnRadiusMin"));
  kRespawnRadiusMax = static_cast<f32>(g_rules.variable("RespawnRadiusMax"));
  kVoiceRadius = static_cast<f32>(g_rules.variable("VoiceRadius"));
  kPingRemoveM = static_cast<f32>(g_rules.variable("PingRemoveM"));
  if (!blueprint) return;
  const Value colors = blueprint->get("Palette");
  for (std::size_t k = 0; k < colors.size(); ++k) g_palette.push_back(static_cast<u32>(colors.item(k).i));
}
const std::vector<u32>& palette() { return g_palette; }

namespace {
f64 fmin2(f64 a, f64 b) { return a < b ? a : b; }
f64 clampd(f64 v, f64 lo, f64 hi) { return v < lo ? lo : (v > hi ? hi : v); }
Quat nlerp_slerp(Quat a, Quat b, f32 t) { return slerp(a, b, t); }
}  // namespace

// ---- PoseTimeline (vs_pose_sync.mjs) ----------------------------------------------------------

bool PoseTimeline::push(Vec3 p, Quat q, Vec3 v, bool has_v, f64 source, f64 received) {
  if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) return false;
  q = qnormalize(q);
  if (!snaps_.empty()) {
    const Snap& prev = snaps_.back();
    if (received <= prev.received || source <= prev.source) return false;
  }
  // Measured velocity from the last snapshot, blended with the sender's (blendVelocity).
  Vec3 measured;
  bool has_measured = false;
  if (!snaps_.empty()) {
    const f64 dt = (source - snaps_.back().source) / 1000;
    if (dt > 0.001 && dt < 1) { measured = (p - snaps_.back().p) * static_cast<f32>(1 / dt); has_measured = true; }
  }
  Vec3 target = has_v && has_measured ? v * 0.7f + measured * 0.3f : has_v ? v : has_measured ? measured : Vec3{};
  Vec3 vel = snaps_.empty() ? target : snaps_.back().v + (target - snaps_.back().v) * 0.35f;
  // Clock offset: the floor of (received - source) over the window, rising slowly (8 %).
  offsets_.push_back(received - source);
  while (offsets_.size() > kMaxSnapshots) offsets_.erase(offsets_.begin());
  const f64 floor = *std::min_element(offsets_.begin(), offsets_.end());
  if (!have_offset_) { offset_ = floor; have_offset_ = true; }
  else if (floor < offset_) offset_ = floor;
  else offset_ += fmin2(1, (floor - offset_) * 0.08);
  // the jitter buffer: the latest snapshots' lateness over the fastest (offset_), the gap between
  // two snapshots; the delay covers the latest of them (no snapshot missing when it is due)
  lags_.push_back(std::max(0.0, received - source - offset_));
  while (lags_.size() > kMaxSnapshots) lags_.erase(lags_.begin());
  const f64 late = *std::max_element(lags_.begin(), lags_.end());
  const f64 gap = snaps_.empty() ? 33 : std::min(250.0, source - snaps_.back().source);
  const f64 want = clampd(late + gap + 15, kInterpolationDelayMs, 320);
  delay_ += want > delay_ ? std::min(want - delay_, 40.0) : std::max(want - delay_, -3.0);
  snaps_.push_back({p, q, vel, received, source});
  while (snaps_.size() > kMaxSnapshots) snaps_.erase(snaps_.begin());
  return true;
}

PoseSample PoseTimeline::sample(f64 now) const {
  PoseSample out;
  if (snaps_.empty()) return out;
  const Snap& latest = snaps_.back();
  out.valid = true;
  out.age_ms = now > latest.received ? now - latest.received : 0;
  out.stale = out.age_ms > kStaleHoldMs;
  const f64 target = now - offset_ - delay_;
  // the snapshots on both sides of the target time (their places in snaps_; no_right: none after it)
  constexpr std::size_t no_right = static_cast<std::size_t>(-1);
  std::size_t left_at = 0, right_at = no_right;
  for (std::size_t i = 1; i < snaps_.size(); ++i) {
    if (snaps_[i].source >= target) { right_at = i; break; }
    left_at = i;
  }
  if (right_at != no_right) {
    const Snap& left = snaps_[left_at];
    const Snap& right = snaps_[right_at];
    const f64 span = std::max(0.001, right.source - left.source);
    const f32 a = static_cast<f32>(clampd((target - left.source) / span, 0, 1));
    // Hermite through both snapshots with their velocities (Glenn Fiedler, "Snapshot
    // Interpolation": the curve passes through both points and matches their velocities - no
    // kink at the samples); a jump between them (a teleport) is not a flight: straight then
    const f32 T = static_cast<f32>(span / 1000);
    const Vec3 d = right.p - left.p;
    const f32 flown = (kern::length(left.v) + kern::length(right.v)) * 0.5f * T;
    if (T < 0.6f && kern::length(d) <= flown * 2.5f + 0.5f) {
      const f32 a2 = a * a, a3 = a2 * a;
      out.p = left.p * (2 * a3 - 3 * a2 + 1) + left.v * ((a3 - 2 * a2 + a) * T) + right.p * (-2 * a3 + 3 * a2) + right.v * ((a3 - a2) * T);
    } else {
      out.p = left.p + d * a;
    }
    out.q = nlerp_slerp(left.q, right.q, a);
    return out;
  }
  const f64 ex = clampd(target - latest.source, 0, kMaxExtrapolationMs);
  out.p = latest.p + latest.v * static_cast<f32>(ex / 1000);
  out.q = latest.q;
  out.extrapolated = ex > 0;
  return out;
}

bool NavTimeline::push(Vec3 p, f32 heading, u8 mode, f64 at) {
  if (!snaps_.empty() && at <= snaps_.back().at) return false;
  if (!snaps_.empty() && (snaps_.back().mode != mode || kern::length(snaps_.back().p - p) > kTeleportM)) reset();
  snaps_.push_back({p, wrap_pi(heading), mode, at});
  while (snaps_.size() > 10) snaps_.erase(snaps_.begin());
  return true;
}

bool NavTimeline::sample(f64 now, Vec3& p, f32& heading) const {
  if (snaps_.empty()) return false;
  const f64 at = now - kInterpolationDelayMs;
  if (at <= snaps_[0].at) { p = snaps_[0].p; heading = snaps_[0].heading; return true; }
  for (std::size_t i = 1; i < snaps_.size(); ++i) {
    const Snap& b = snaps_[i];
    if (at > b.at) continue;
    const Snap& a = snaps_[i - 1];
    const f32 t = static_cast<f32>(clampd((at - a.at) / (b.at - a.at), 0, 1));
    p = a.p + (b.p - a.p) * t;
    heading = wrap_pi(a.heading + wrap_pi(b.heading - a.heading) * t);
    return true;
  }
  p = snaps_.back().p;  // no blind prediction: hold
  heading = snaps_.back().heading;
  return true;
}

// ---- players ------------------------------------------------------------------------------------

u32 hash_id(const std::string& id) {
  u32 h = 2166136261u;
  for (unsigned char c : id) { h ^= c; h *= 16777619u; }
  return h;
}

std::vector<u32> assign_colors(const std::vector<std::string>& ids) {
  std::vector<i64> numbers;
  for (const std::string& id : ids) numbers.push_back(hash_id(id));
  std::vector<Value> r;
  std::vector<u32> out;
  if (!g_rules.call("ColorSlots", {Value::of(std::move(numbers))}, r) || r.empty()) return out;
  for (std::size_t k = 0; k < r[0].size(); ++k) out.push_back(static_cast<u32>(r[0].item(k).i));
  return out;
}
u32 color_of(const std::string& id) {
  const std::vector<u32> one = assign_colors({id});
  return one.empty() ? 0 : one[0];
}

// ---- combat -------------------------------------------------------------------------------------

std::string weapon_label(Weapon w) {
  std::vector<Value> r;
  return g_rules.call("WeaponLabel", {rules::of_enum(w)}, r) && !r.empty() ? r[0].as_text() : std::string();
}
std::string zone_name(Zone z) {
  std::vector<Value> r;
  return g_rules.call("ZoneName", {rules::of_enum(z)}, r) && !r.empty() ? r[0].as_text() : std::string();
}
f32 round_damage(Weapon w, Zone z) { return num("RoundDamage", {rules::of_enum(w), rules::of_enum(z)}); }
f32 blast_damage(f32 damage) { return num("BlastDamage", {rules::number(damage)}); }

HitVerdict judge_hit(bool target_known, bool shooter_known, bool self_damage, bool friendly_fire, f64 now, f64 protected_until, f32 hp) {
  std::vector<Value> r;
  if (!g_rules.call("JudgeHit", {Value::boolean(target_known), Value::boolean(shooter_known), Value::boolean(self_damage), Value::boolean(friendly_fire), rules::number(now),
                                 rules::number(protected_until), rules::number(hp)},
                    r) ||
      r.empty())
    return HitVerdict::unknown_player;
  return static_cast<HitVerdict>(r[0].i);
}

f32 regen(f32 hp, f64 now, f64 last_hurt, f32 dt) { return num("Regen", {rules::number(hp), rules::number(now), rules::number(last_hurt), rules::number(dt)}); }

Vec2 respawn_offset(Rng& rng) {
  const f32 r1 = rng.unit(), r2 = rng.unit();  // (how far out, then which way)
  return pair("RespawnOffset", {rules::number(r1), rules::number(r2)});
}

void Scoreboard::ensure(u8 slot) {
  for (const Score& s : rows_) if (s.slot == slot) return;
  rows_.push_back({slot, 0, 0});
}
void Scoreboard::remove(u8 slot) { rows_.erase(std::remove_if(rows_.begin(), rows_.end(), [&](const Score& s) { return s.slot == slot; }), rows_.end()); }
void Scoreboard::died(u8 victim, i32 by) {
  ensure(victim);
  for (Score& s : rows_) if (s.slot == victim) ++s.deaths;
  if (by >= 0 && by != victim) {
    ensure(static_cast<u8>(by));
    for (Score& s : rows_) if (s.slot == by) ++s.kills;
  }
}
Score Scoreboard::of(u8 slot) const {
  for (const Score& s : rows_) if (s.slot == slot) return s;
  return {slot, 0, 0};
}
std::vector<Score> Scoreboard::ranked() const {
  std::vector<Score> r = rows_;
  std::stable_sort(r.begin(), r.end(), [](const Score& a, const Score& b) { return a.kills != b.kills ? a.kills > b.kills : a.deaths < b.deaths; });
  return r;
}

bool group_spawn_center(const std::vector<Vec2>& pts, Vec2& out, f32 link) {
  if (pts.empty()) return false;
  std::vector<bool> seen(pts.size(), false);
  std::vector<Vec2> best;
  for (std::size_t i = 0; i < pts.size(); ++i) {
    if (seen[i]) continue;
    std::vector<std::size_t> queue{i};
    seen[i] = true;
    std::vector<Vec2> comp;
    for (std::size_t k = 0; k < queue.size(); ++k) {
      const Vec2 a = pts[queue[k]];
      comp.push_back(a);
      for (std::size_t b = 0; b < pts.size(); ++b)
        if (!seen[b] && std::hypot(a.x - pts[b].x, a.y - pts[b].y) <= link) { seen[b] = true; queue.push_back(b); }
    }
    if (comp.size() > best.size()) best = comp;
  }
  // Medoid: stays on an actual player, not at an empty centroid across buildings.
  f32 score = 1e30f;
  for (const Vec2& p : best) {
    f32 d = 0;
    for (const Vec2& q : best) d += std::hypot(p.x - q.x, p.y - q.y);
    if (d < score) { score = d; out = p; }
  }
  return true;
}

Vec2 spawn_ring_offset(u32 slot) { return pair("SpawnRingOffset", {Value::integer(slot)}); }

// ---- vote -----------------------------------------------------------------------------------------
namespace {
Value ints(const std::vector<u8>& v) { return Value::of(std::vector<i64>(v.begin(), v.end())); }
std::vector<u8> bytes(const Value& v) {
  std::vector<u8> out;
  for (std::size_t k = 0; k < v.size(); ++k) out.push_back(static_cast<u8>(v.item(k).i));
  return out;
}
Value to_value(const Ballot& b) {
  return Value::structure({Value::integer(b.id), Value::integer(b.key), Value::boolean(b.value), Value::integer(b.host), ints(b.members), ints(b.yes), rules::number(b.expires_ms),
                           rules::of_enum(b.status)});
}
bool take(const Value& v, Ballot& b) {
  if (v.fields() < 8) return false;
  b.id = static_cast<u32>(v.field(0)->i);
  b.key = static_cast<u8>(v.field(1)->i);
  b.value = v.field(2)->b;
  b.host = static_cast<u8>(v.field(3)->i);
  b.members = bytes(*v.field(4));
  b.yes = bytes(*v.field(5));
  b.expires_ms = v.field(6)->as_number();
  b.status = static_cast<Ballot::Status>(v.field(7)->i);
  return true;
}
/// A roster as the vote compares it: in order, each player once.
void roster(std::vector<u8>& members) {
  std::sort(members.begin(), members.end());
  members.erase(std::unique(members.begin(), members.end()), members.end());
}
}  // namespace

bool create_ballot(Ballot& b, u32 id, u8 key, bool value, u8 host, std::vector<u8> members, f64 now) {
  roster(members);
  b = Ballot{};
  b.status = Ballot::Status::invalid;
  std::vector<Value> r;
  if (!g_rules.call("CreateBallot", {Value::integer(id), Value::integer(key), Value::boolean(value), Value::integer(host), ints(members), rules::number(now)}, r) || r.size() < 2) return false;
  take(r[1], b);
  return r[0].b;
}

Ballot::Status cast_vote(Ballot& b, u8 voter, bool approve, std::vector<u8> current, f64 now, Ballot::Status& did) {
  roster(current);
  std::vector<Value> r;
  did = b.status;
  if (g_rules.call("CastVote", {to_value(b), Value::integer(voter), Value::boolean(approve), ints(current), rules::number(now)}, r) && r.size() >= 2) {
    take(r[0], b);
    did = static_cast<Ballot::Status>(r[1].i);
  }
  return b.status;
}
Ballot::Status cast_vote(Ballot& b, u8 voter, bool approve, std::vector<u8> current, f64 now) {
  Ballot::Status did = b.status;
  return cast_vote(b, voter, approve, std::move(current), now, did);
}

bool ballot_stale(const Ballot& b, const std::vector<u8>& current, f64 now) { return g_rules.flag("Stale", {to_value(b), ints(current), rules::number(now)}); }

std::string ballot_name(Ballot::Status s) {
  std::vector<Value> r;
  return g_rules.call("BallotName", {rules::of_enum(s)}, r) && !r.empty() ? r[0].as_text() : std::string();
}

// ---- voice ----------------------------------------------------------------------------------------

u8 encode_mulaw(f32 sample) {
  const f32 v = clampf(sample, -1, 1);
  i32 x = std::min<i32>(32635, static_cast<i32>(std::lround(std::fabs(v) * 32767)));
  i32 exp = 7;
  x += 132;
  for (i32 mask = 0x4000; exp > 0 && !(x & mask); --exp, mask >>= 1) {}
  const i32 mant = (x >> (exp + 3)) & 15;
  return static_cast<u8>(((v < 0 ? 0x7f : 0xff) ^ (exp << 4 | mant)) & 255);
}

f32 decode_mulaw(u8 byte) {
  const i32 x = (~byte) & 255, sign = x & 128, exp = (x >> 4) & 7, mant = x & 15;
  const i32 pcm = ((mant << 3) + 132) << exp;
  return static_cast<f32>(sign ? 132 - pcm : pcm - 132) / 32768.0f;
}

f32 proximity_gain(f32 d) { return voice_gain(d, false); }
f32 voice_gain(f32 d, bool everywhere) { return std::isfinite(d) ? num("VoiceGain", {rules::number(d), Value::boolean(everywhere)}) : everywhere ? voice_gain(1e9f, true) : 0; }

bool VoiceStream::push(u32 seq, const std::vector<u8>& mulaw, f32 gain) {
  if (gain <= 0 || mulaw.empty() || mulaw.size() > 800) return false;
  if (have_seq_ && seq <= last_seq_) return false;
  have_seq_ = true;
  last_seq_ = seq;
  // The playhead: at least 30 ms ahead, never more than 150 ms of backlog (proximity_voice.mjs).
  f64 queued = -pos_;
  for (const Chunk& c : queue_) queued += static_cast<f64>(c.pcm.size());
  queued += lead_;
  const f64 min_lead = 0.03 * kVoiceRate, max_lead = 0.15 * kVoiceRate;
  if (queue_.empty()) { lead_ = min_lead; pos_ = 0; }
  else if (queued > max_lead + kVoiceFrame) {
    // Too far behind: drop the oldest audio (a burst after a stall would only add delay).
    while (!queue_.empty() && queued > max_lead) { queued -= static_cast<f64>(queue_.front().pcm.size()) - pos_; pos_ = 0; queue_.erase(queue_.begin()); }
  }
  Chunk c;
  c.gain = gain;
  c.pcm.resize(mulaw.size());
  for (std::size_t i = 0; i < mulaw.size(); ++i) c.pcm[i] = decode_mulaw(mulaw[i]);
  queue_.push_back(std::move(c));
  return true;
}

bool VoiceStream::render(std::vector<f32>& out, u32 frames, u32 rate) {
  if (queue_.empty() || rate == 0) return false;
  const f64 step = static_cast<f64>(kVoiceRate) / rate;
  if (frames > out.size() / 2) frames = static_cast<u32>(out.size() / 2);  // (two samples a frame, as many as the buffer has)
  for (u32 i = 0; i < frames && !queue_.empty(); ++i) {
    if (lead_ > 0) { lead_ -= step; continue; }
    const Chunk& c = queue_.front();
    const std::size_t k = static_cast<std::size_t>(pos_);
    const f32 frac = static_cast<f32>(pos_ - static_cast<f64>(k));
    const f32 a = c.pcm[k], b = k + 1 < c.pcm.size() ? c.pcm[k + 1] : (queue_.size() > 1 ? queue_[1].pcm[0] : a);
    const f32 s = (a + (b - a) * frac) * c.gain;
    out[2 * i] += s;
    out[2 * i + 1] += s;
    pos_ += step;
    if (pos_ >= static_cast<f64>(c.pcm.size())) { pos_ -= static_cast<f64>(c.pcm.size()); queue_.erase(queue_.begin()); }
  }
  return !queue_.empty();
}

void VoiceCapture::push(kern::Span<const f32> samples, u32 rate) {
  if (!rate) return;
  const u32 count = static_cast<u32>(samples.size);
  const f64 stride = static_cast<f64>(rate) / kVoiceRate;
  f64 i = phase_;
  for (; i < count; i += stride) {
    pending_.push_back(samples[static_cast<u32>(i)]);
    if (pending_.size() == kVoiceFrame) {
      std::vector<u8> f(kVoiceFrame);
      for (u32 k = 0; k < kVoiceFrame; ++k) f[k] = encode_mulaw(pending_[k]);
      frames_.push_back(std::move(f));
      pending_.clear();
      if (frames_.size() > 8) frames_.erase(frames_.begin());
    }
  }
  phase_ = i - count;
}

bool VoiceCapture::take(std::vector<u8>& frame) {
  if (frames_.empty()) return false;
  frame = std::move(frames_.front());
  frames_.erase(frames_.begin());
  return true;
}

// ---- navigation -------------------------------------------------------------------------------------

namespace {
struct Along { f32 along = 0, distance = 1e30f; };
Along project_on(const std::vector<Vec2>& pts, Vec2 pos, std::vector<f32>& cum) {
  cum.assign(1, 0);
  for (std::size_t i = 0; i + 1 < pts.size(); ++i) cum.push_back(cum.back() + std::hypot(pts[i + 1].x - pts[i].x, pts[i + 1].y - pts[i].y));
  Along best;
  for (std::size_t i = 0; i + 1 < pts.size(); ++i) {
    const f32 len = cum[i + 1] - cum[i];
    if (len < 0.001f) continue;
    const Vec2 a = pts[i], b = pts[i + 1];
    const f32 t = clampf(((pos.x - a.x) * (b.x - a.x) + (pos.y - a.y) * (b.y - a.y)) / (len * len), 0, 1);
    const f32 d = std::hypot(pos.x - (a.x + (b.x - a.x) * t), pos.y - (a.y + (b.y - a.y) * t));
    if (d < best.distance) { best.distance = d; best.along = cum[i] + len * t; }
  }
  return best;
}
Vec2 point_at(const std::vector<Vec2>& pts, const std::vector<f32>& cum, f32 d) {
  d = clampf(d, 0, cum.back());
  std::size_t i = 0;
  while (i + 2 < cum.size() && cum[i + 1] < d) ++i;
  const f32 len = cum[i + 1] - cum[i];
  const f32 t = len > 1e-6f ? clampf((d - cum[i]) / len, 0, 1) : 0;
  return {pts[i].x + (pts[i + 1].x - pts[i].x) * t, pts[i].y + (pts[i + 1].y - pts[i].y) * t};
}
}  // namespace

std::vector<Crumb> sample_breadcrumbs(const std::vector<Vec2>& pts, Vec2 pos, u32 count, f32 spacing, f32 start) {
  std::vector<Crumb> out;
  if (pts.size() < 2) return out;
  std::vector<f32> cum;
  const Along a = project_on(pts, pos, cum);
  const f32 total = cum.back();
  if (total < 0.01f) return out;
  const u32 n = std::min<u32>(count, 8);
  const f32 step = clampf(spacing, 2, 500), first = clampf(start, 1, 500);
  for (u32 j = 0; j < n; ++j) {
    const f32 at = a.along + first + static_cast<f32>(j) * step;
    if (at > total + 0.001f) break;
    for (std::size_t i = 0; i + 1 < pts.size(); ++i) {
      const f32 len = cum[i + 1] - cum[i];
      if (at > cum[i + 1] + 0.0001f || len < 0.001f) continue;
      const f32 t = clampf((at - cum[i]) / len, 0, 1);
      const Vec2 p0 = pts[i], p1 = pts[i + 1];
      out.push_back({{p0.x + (p1.x - p0.x) * t, p0.y + (p1.y - p0.y) * t}, std::atan2(p1.x - p0.x, p1.y - p0.y), total - at});
      break;
    }
  }
  return out;
}

Turn upcoming_turn(const std::vector<Vec2>& pts, Vec2 pos, f32 scan, f32 window, f32 min_angle) {
  Turn out;
  if (pts.size() < 3) return out;
  std::vector<f32> cum;
  const Along a = project_on(pts, pos, cum);
  if (cum.back() < 2) return out;
  scan = clampf(scan, 5, 1000);
  window = clampf(window, 3, 80);
  min_angle = clampf(min_angle, 15, 160);
  for (std::size_t i = 1; i + 1 < pts.size(); ++i) {
    const f32 away = cum[i] - a.along;
    if (away < 1 || away > scan) continue;
    const Vec2 c = pts[i], prev = point_at(pts, cum, cum[i] - window), next = point_at(pts, cum, cum[i] + window);
    const f32 bx = c.x - prev.x, by = c.y - prev.y, ax = next.x - c.x, ay = next.y - c.y;
    if (std::hypot(bx, by) < 2 || std::hypot(ax, ay) < 2) continue;
    const f32 angle = std::atan2(bx * ay - by * ax, bx * ax + by * ay) / kDeg;
    if (std::fabs(angle) < min_angle) continue;
    out.dir = std::fabs(angle) > 155 ? Turn::Dir::u_turn : angle < 0 ? Turn::Dir::right : Turn::Dir::left;
    out.distance = away;
    out.angle_deg = std::fabs(angle);
    return out;
  }
  return out;
}

f32 route_left(const std::vector<Vec2>& pts, Vec2 pos) {
  if (pts.size() < 2) return pts.empty() ? 0 : std::hypot(pts[0].x - pos.x, pts[0].y - pos.y);
  std::vector<f32> cum;
  const Along a = project_on(pts, pos, cum);
  return cum.back() - a.along + a.distance;
}

std::string bearing_arrow(f32 b) {
  std::vector<Value> r;
  return g_rules.call("BearingArrow", {rules::number(b)}, r) && !r.empty() ? r[0].as_text() : std::string();
}

MapEdge locate_map_peer(Vec2 p, f32 w, f32 h, f32 ix, f32 it, f32 ib) {
  MapEdge out;
  if (!(std::isfinite(p.x) && std::isfinite(p.y))) return out;
  std::vector<Value> r;
  if (!g_rules.call("LocateMapPeer", {rules::number(p.x), rules::number(p.y), rules::number(w), rules::number(h), rules::number(ix), rules::number(it), rules::number(ib)}, r) || r.size() < 6)
    return out;
  out.valid = r[0].b;
  out.p = {static_cast<f32>(r[1].as_number()), static_cast<f32>(r[2].as_number())};
  out.offmap = r[3].b;
  out.rotation_deg = static_cast<f32>(r[4].as_number());
  out.arrow = r[5].as_text();
  return out;
}

Vec2 safe_mobile_marker_point(Vec2 p, f32 w, f32 h, bool mobile) {
  return pair("SafeMobileMarkerPoint", {rules::number(p.x), rules::number(p.y), rules::number(w), rules::number(h), Value::boolean(mobile)});
}

std::string format_distance(f32 m) {
  char b[32];
  if (m >= 1000) {
    std::snprintf(b, sizeof b, m >= 10000 ? "%.0f km" : "%.1f km", m / 1000);
    for (char* c = b; *c; ++c) if (*c == '.') *c = ',';
  } else {
    std::snprintf(b, sizeof b, "%.0f m", std::fmax(10.0f, std::round(m / 10) * 10));
  }
  return b;
}

// ---- pings ----------------------------------------------------------------------------------------

bool PingBoard::toggle(u8 owner, Vec3 p, u32 color, f64 now) {
  for (const Ping& q : pings_)
    if (q.owner == owner && g_rules.flag("PingToggles", {rules::number(std::hypot(q.p.x - p.x, q.p.y - p.y))})) { clear(owner); return false; }
  set(owner, p, color, now);
  return true;
}
void PingBoard::set(u8 owner, Vec3 p, u32 color, f64 now) {
  for (Ping& q : pings_)
    if (q.owner == owner) { q.p = p; q.color = color; q.at_ms = now; return; }
  pings_.push_back({owner, p, color, now});
}
void PingBoard::clear(u8 owner) { pings_.erase(std::remove_if(pings_.begin(), pings_.end(), [&](const Ping& q) { return q.owner == owner; }), pings_.end()); }
void PingBoard::expire(f64 now) { pings_.erase(std::remove_if(pings_.begin(), pings_.end(), [&](const Ping& q) { return g_rules.flag("PingExpired", {rules::number(now - q.at_ms)}); }), pings_.end()); }

f32 PingBoard::fade(const Ping& p, f64 now_ms) { return num("PingFade", {rules::number(now_ms - p.at_ms)}); }

}  // namespace sc::mp
