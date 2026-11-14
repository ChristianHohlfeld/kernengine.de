// SpawnCity on Kern - every round in flight in multiplayer: a mate's hand grenade, 40 mm round,
// rocket, guided missile, drone rocket, jet missile or bomb flies on every machine on his track.
//
// A round is the shooter's: his machine flies it (its bounces, its steering by his laser or his
// camera, its hit) and decides where it goes off (the blast is an Fx of its own, reliable, with
// the damage and the destruction). Every other machine draws it where it is on his - the shooter
// streams each of his rounds at 20 Hz (id, kind, place, velocity, on his clock), and here it runs
// on a pose timeline (mp::PoseTimeline: interpolated a jitter buffer behind his clock, a short
// extrapolation at most) as a ghost: the same model, glow, light and smoke as his, no hits of its
// own. Its end is his: the "gone" record, or his blast where it is.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>

#include "game.h"
#include "mp_rules.h"

namespace sc {

namespace {
constexpr u8 kGone = 1;
constexpr f32 kSendEvery = 0.05f, kQuiet = 0.6f;

struct Out {
  std::vector<u8> b;
  template <class T> Out& put(T v) { const auto* p = reinterpret_cast<const u8*>(&v); b.insert(b.end(), p, p + sizeof(T)); return *this; }
};
struct In {
  const std::vector<u8>& b;
  std::size_t at = 0;
  bool ok = true;
  template <class T> T get() {
    T v{};
    if (at + sizeof(T) > b.size()) { ok = false; return v; }
    std::memcpy(&v, b.data() + at, sizeof(T));
    at += sizeof(T);
    return v;
  }
};
kern::i16 dms(f32 v) { return static_cast<kern::i16>(std::lround(clampf(v * 10, -32000, 32000))); }
}  // namespace

void Game::projsync_ready() {
  if (!projsync_) projsync_ = std::make_unique<ProjSync>();
}

void Game::projsync_blast(u8 owner, Vec3 at) {
  // his blast: his round is over here too (whatever the stream still says)
  if (!projsync_) return;
  for (Projectile& p : projectiles_)
    if (p.ghost && p.alive && (p.net >> 16) == owner && kern::length(p.pos - at) < 8) {
      p.alive = false;
      projsync_->ghosts.erase(p.net);
    }
}

void Game::projsync_update(f32 dt) {
  projsync_ready();
  ProjSync& S = *projsync_;
  if (!net_.active()) {
    if (!S.ghosts.empty()) { for (Projectile& p : projectiles_) if (p.ghost) p.alive = false; S.ghosts.clear(); }
    (void)net_.take_proj();
    return;
  }
  const f32 now = world_->sim_time();
  const f64 now_ms = static_cast<f64>(now) * 1000;

  // ---- ours: every round of the player in flight, and those that ended since
  S.clock += dt;
  if (S.clock >= kSendEvery) {
    S.clock = 0;
    struct Rec { u16 id; u8 kind, flags; Vec3 p, v; };
    std::vector<Rec> recs;
    for (const Projectile& p : projectiles_)
      if (p.alive && !p.ghost && p.by_player && p.net) recs.push_back({static_cast<u16>(p.net), static_cast<u8>(p.id), 0, p.pos, p.vel});
    for (const guided::Flight& f : guided_)
      recs.push_back({static_cast<u16>(0x8000 | (static_cast<u32>(std::lround(f.m.born * 1000)) & 0x7fff)), static_cast<u8>(weapons::Id::drone_guided), static_cast<u8>(f.m.motor ? 2 : 0), f.m.p, f.m.v});
    // the ones that ended: "gone", three times (the stream is unreliable)
    std::vector<std::pair<u16, u8>> next;
    for (auto& [id, n] : S.sent) {
      if (std::any_of(recs.begin(), recs.end(), [&](const Rec& r) { return r.id == id; })) continue;
      if (n >= 3) continue;
      recs.push_back({id, 0, kGone, {}, {}});
      next.emplace_back(id, static_cast<u8>(n + 1));
    }
    for (const Rec& r : recs) if (!(r.flags & kGone)) next.emplace_back(r.id, 0);
    S.sent = std::move(next);
    if (!recs.empty()) {
      Out o;
      o.put(static_cast<u32>(std::lround(now_ms)));
      o.put(static_cast<u8>(std::min<std::size_t>(recs.size(), 48)));
      for (std::size_t i = 0; i < recs.size() && i < 48; ++i) {
        const Rec& r = recs[i];
        o.put(r.id).put(r.kind).put(r.flags);
        if (r.flags & kGone) continue;
        o.put(r.p.x).put(r.p.y).put(r.p.z).put(dms(r.v.x)).put(dms(r.v.y)).put(dms(r.v.z));
      }
      net_.send_proj(o.b);
      ++S.sent_msgs;
    }
  }

  // ---- theirs: onto their timelines
  for (const net::Session::Blob& m : net_.take_proj()) {
    In in{m.data};
    const u32 t = in.get<u32>();
    const u8 n = in.get<u8>();
    for (u8 k = 0; k < n && in.ok; ++k) {
      const u16 id = in.get<u16>();
      const u8 kind = in.get<u8>(), flags = in.get<u8>();
      const u32 key = static_cast<u32>(m.from) << 16 | id;
      if (flags & kGone) { if (auto it = S.ghosts.find(key); it != S.ghosts.end()) it->second.gone = true; continue; }
      Vec3 p, v;
      p.x = in.get<f32>(); p.y = in.get<f32>(); p.z = in.get<f32>();
      v.x = in.get<kern::i16>() / 10.0f; v.y = in.get<kern::i16>() / 10.0f; v.z = in.get<kern::i16>() / 10.0f;
      if (!in.ok || !std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) break;
      ++S.got;
      auto [it, fresh] = S.ghosts.try_emplace(key);
      ProjSync::Ghost& g = it->second;
      g.line.push(p, Quat{}, v, true, static_cast<f64>(t), now_ms);
      g.v = v;
      g.heard = now;
      if (fresh) {
        // a new round of his: drawn here from now on (no hits, no blast of its own)
        Projectile q;
        q.id = static_cast<weapons::Id>(kind);
        q.pos = p;
        q.vel = v;
        q.born = now;
        q.by_player = false;
        q.ghost = true;
        q.net = key;
        q.gravity = q.id == weapons::Id::grenade_launcher || q.id == weapons::Id::hand_grenade || q.id == weapons::Id::jet_bomb;
        q.fuse = 1e9f;
        projectiles_.push_back(q);
        osfx("launch", p, static_cast<f32>(kind), 0);  // his launch, heard where it is
      }
      (void)flags;
    }
  }
  // ---- theirs: where each is now on his clock
  for (Projectile& p : projectiles_) {
    if (!p.ghost || !p.alive) continue;
    auto it = S.ghosts.find(p.net);
    if (it == S.ghosts.end() || it->second.gone || now - it->second.heard > kQuiet) { p.alive = false; continue; }
    const mp::PoseSample s = it->second.line.sample(now_ms);
    if (!s.valid) continue;
    p.pos = s.p;
    p.vel = it->second.v;
    if (p.id == weapons::Id::rocket || p.id == weapons::Id::drone_rockets || p.id == weapons::Id::jet_missile || p.id == weapons::Id::drone_guided) rocket_smoke_emit(p, now);
  }
  for (auto it = S.ghosts.begin(); it != S.ghosts.end();) {
    if (it->second.gone || now - it->second.heard > kQuiet + 1) it = S.ghosts.erase(it);
    else ++it;
  }
}

}  // namespace sc
