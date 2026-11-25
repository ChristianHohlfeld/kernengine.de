// SpawnCity on Kern - one destroyed city in multiplayer: craters and broken buildings are the
// host's, and everybody's the same.
//
// What a blast does to the world is lasting state, not an effect: a crater changes the ground (the
// terrain mesh, its Box3D collision, the roads draped over it), a hit building loses floors (its
// mesh chunk, its collider, the minimap, the walls the marks sit on). Every machine already runs a
// blast it hears of (Fx::explosion) - but a machine where that part of town is not loaded, a player
// who joins later, or a crater dug only where the shooter stood, would leave the cities apart.
// So the destruction is replicated as state, the host the authority:
//  * craters: the host keeps THE list (at most 32, oldest out first - the terrain's own rule) and
//    sends it whole (16 bytes a crater); a client digs his own at once (no waiting for the round
//    trip) and tells the host, then takes the host's list - the same bowls in the same order, so
//    the same height function, the same mesh and the same collision on every machine
//  * buildings: by their seed (the same on every machine: the map feature's id and tile), the
//    height that is left and whether it collapsed, plus the structural points left - only ever
//    lower (a merge takes the lowest anybody saw), so whatever order the reports come in, every
//    machine ends at the same city; a building not loaded here keeps its damage for when its tile
//    streams in (CityStream::merge_damage)
//  * changes go as deltas (reliable, ordered), a joiner gets the whole state; every 2 s the host
//    sends a digest (a hash of the craters and the damage) - a check that the cities ARE equal
//    (a differing one is a defect, reported), not a repair
//  * RESET is an epoch: the host's next epoch clears the city on every machine
// Debris, dust, soot and scorch stay effects (every machine draws them from the blast it hears).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "game.h"
#include "game_life.h"
#include "wire.h"

namespace sc {

namespace {
constexpr u8 kEdit = 1, kState = 2, kDigest = 3, kAsk = 4;
constexpr f32 kDeltaEvery = 0.15f, kDigestEvery = 2.0f;
constexpr std::size_t kPerMessage = 400;  // building records per message (a full state is split)

using Craters = DestroySync::Craters;
using DamageList = DestroySync::DamageList;
using Points = DestroySync::Points;

void put_craters(std::vector<u8>& o, const Craters& c) {
  wire::put_u8(o, static_cast<u8>(std::min<std::size_t>(c.size(), 255)));
  for (std::size_t i = 0; i < c.size() && i < 255; ++i) { wire::put_f32(o, c[i].x); wire::put_f32(o, c[i].y); wire::put_f32(o, c[i].r); wire::put_f32(o, c[i].depth); }
}
bool get_craters(wire::Reader& in, const std::vector<u8>& b, Craters& c) {
  const u8 n = in.get_u8(b);
  c.clear();
  for (u8 i = 0; i < n && in.ok; ++i) { Terrain::CraterRec r; r.x = in.get_f32(b); r.y = in.get_f32(b); r.r = in.get_f32(b); r.depth = in.get_f32(b); c.push_back(r); }
  return in.ok;
}
void put_damage(std::vector<u8>& o, const DamageList& d, std::size_t from, std::size_t n) {
  wire::put_u16(o, static_cast<u16>(n));
  for (std::size_t i = from; i < from + n; ++i) { wire::put_u32(o, d[i].seed); wire::put_f32(o, d[i].height); wire::put_u8(o, static_cast<u8>(d[i].leveled)); }
}
bool get_damage(wire::Reader& in, const std::vector<u8>& b, DamageList& d) {
  const u16 n = in.get_u16(b);
  d.clear();
  for (u16 i = 0; i < n && in.ok; ++i) { CityStream::DamageRec r; r.seed = in.get_u32(b); r.height = in.get_f32(b); r.leveled = in.get_u8(b) != 0; d.push_back(r); }
  return in.ok;
}
void put_points(std::vector<u8>& o, const Points& p, std::size_t from, std::size_t n) {
  wire::put_u16(o, static_cast<u16>(n));
  for (std::size_t i = from; i < from + n; ++i) { wire::put_u32(o, p[i].first); wire::put_f32(o, p[i].second); }
}
bool get_points(wire::Reader& in, const std::vector<u8>& b, Points& p) {
  const u16 n = in.get_u16(b);
  p.clear();
  for (u16 i = 0; i < n && in.ok; ++i) { const u32 s = in.get_u32(b); const f32 hp = in.get_f32(b); p.emplace_back(s, hp); }
  return in.ok;
}
// FNV-1a over the craters (exact) and the damage (to the centimetre, by seed)
void fnv_byte(u32& h, u32 byte) {
  h ^= byte;
  h *= 16777619u;
}
// the four bytes of a 32-bit value in the order memory holds them (little-endian: the low one first)
void fnv_word(u32& h, u32 v) {
  fnv_byte(h, v & 0xff);
  fnv_byte(h, (v >> 8) & 0xff);
  fnv_byte(h, (v >> 16) & 0xff);
  fnv_byte(h, v >> 24);
}
// ... and of a 32-bit float (its bits, as a message holds them)
void fnv_float(u32& h, f32 v) {
  const std::array<u8, 4> b = wire::f32_bytes(v);
  for (u8 x : b) fnv_byte(h, x);
}
u32 digest_of(const Craters& c, const DamageList& d) {
  u32 h = 2166136261u;
  for (const Terrain::CraterRec& r : c) { fnv_float(h, r.x); fnv_float(h, r.y); fnv_float(h, r.r); fnv_float(h, r.depth); }
  for (const CityStream::DamageRec& r : d) {
    const i32 cm = static_cast<i32>(std::lround(r.height * 100));
    fnv_word(h, r.seed); fnv_word(h, static_cast<u32>(cm)); fnv_byte(h, r.leveled ? 1 : 0);
  }
  return h;
}
bool has(const Craters& v, const Terrain::CraterRec& c) { return std::find(v.begin(), v.end(), c) != v.end(); }
}  // namespace

void Game::dsync_ready() {
  if (!dsync_) dsync_ = std::make_unique<DestroySync>();
}

void Game::dsync_reset_epoch() {
  // RESET while hosting: a new epoch - every machine clears its city with the next state
  dsync_ready();
  DestroySync& d = *dsync_;
  if (net_.hosting()) { ++d.epoch; dsync_send_full(-1); }
}

void Game::dsync_send_full(i32 to) {
  dsync_ready();
  DestroySync& d = *dsync_;
  Craters c;
  city_.terrain().crater_list(c);
  DamageList dm;
  city_.damage_list(dm);
  Points pt;
  destruction_.points_list(pt);
  const std::size_t parts = std::max<std::size_t>(1, (std::max(dm.size(), pt.size()) + kPerMessage - 1) / kPerMessage);
  for (std::size_t k = 0; k < parts; ++k) {
    std::vector<u8> o;
    wire::put_u8(o, kState);
    wire::put_u32(o, d.epoch);
    wire::put_u8(o, static_cast<u8>(k == 0 ? 1 : 0));
    put_craters(o, c);
    const std::size_t a = std::min(dm.size(), k * kPerMessage), b = std::min(pt.size(), k * kPerMessage);
    put_damage(o, dm, a, std::min(kPerMessage, dm.size() - a));
    put_points(o, pt, b, std::min(kPerMessage, pt.size() - b));
    if (to < 0) net_.destroy_to_all(o, true);
    else net_.destroy_to(static_cast<u8>(to), o);
    ++d.sent_states;
  }
  if (to < 0) d.sent = c;
}

void Game::dsync_update(f32 dt) {
  dsync_ready();
  DestroySync& d = *dsync_;
  Terrain& terrain = city_.terrain();
  const bool active = net_.active();
  if (!active) {
    if (d.was_active) { d.mine.clear(); d.sent.clear(); d.known.clear(); d.dmg_out.clear(); d.pts_out.clear(); d.mismatches = 0; }
    d.was_active = false;
    (void)city_.take_damage_log();
    (void)destruction_.take_points_log();
    return;
  }
  const bool host = net_.hosting();
  if (!d.was_active || host != d.hosting) { d.mine.clear(); d.sent.clear(); d.known.clear(); d.mismatches = 0; if (!host) d.epoch = 0; }
  d.was_active = true;
  d.hosting = host;
  const f32 now = static_cast<f32>(time_);

  // what changed here since the last frame (a blast, a merge with log on: the host's relays)
  for (u32 seed : city_.take_damage_log()) {
    CityStream::DamageRec r;
    if (!city_.damage_of(seed, r)) continue;
    auto it = std::find_if(d.dmg_out.begin(), d.dmg_out.end(), [&](const CityStream::DamageRec& q) { return q.seed == seed; });
    if (it != d.dmg_out.end()) *it = r; else d.dmg_out.push_back(r);
  }
  for (const auto& [seed, hp] : destruction_.take_points_log()) {
    auto it = std::find_if(d.pts_out.begin(), d.pts_out.end(), [&](const std::pair<u32, f32>& q) { return q.first == seed; });
    if (it != d.pts_out.end()) it->second = std::fmin(it->second, hp); else d.pts_out.emplace_back(seed, hp);
  }

  for (const net::Session::Blob& m : net_.take_destroy()) {
    wire::Reader in;
    const u8 kind = in.get_u8(m.data);
    const u32 epoch = in.get_u32(m.data);
    if (!in.ok) continue;
    if (host) {
      if (kind == kAsk || epoch != d.epoch) { ++d.asks; dsync_send_full(m.from); continue; }  // a client behind: the whole state
      if (kind != kEdit) continue;
      Craters c;
      DamageList dm;
      Points pt;
      if (!get_craters(in, m.data, c) || !get_damage(in, m.data, dm) || !get_points(in, m.data, pt)) continue;
      // his craters join the list (in the order they reach the authority), his damage the city
      for (const Terrain::CraterRec& r : c) {
        Craters cur;
        terrain.crater_list(cur);
        if (!has(cur, r) && r.r > 0.5f && r.r < 40 && std::isfinite(r.x) && std::isfinite(r.y)) terrain.add_crater({r.x, r.y}, r.r, r.depth);
      }
      city_.merge_damage(dm, true);
      destruction_.merge_points(pt, true);
      continue;
    }
    // client
    if (kind == kDigest) {
      const u32 theirs = in.get_u32(m.data);
      if (!in.ok) continue;
      Craters c;
      terrain.crater_list(c);
      DamageList dm;
      city_.damage_list(dm);
      // the check, not a repair: the protocol keeps the cities equal by construction (one
      // authority, reliable ordered deltas, merges that only ever lower, the whole state for a
      // joiner and with every epoch) - a differing digest while nothing of ours is in flight is a
      // defect, counted and logged so it is seen (`destroy sync`), never papered over
      if (epoch == d.epoch && d.mine.empty() && d.dmg_out.empty() && d.pts_out.empty() && now - d.told_at > 1.0f && digest_of(c, dm) != theirs) {
        if (++d.mismatches == 1 || d.mismatches % 30 == 0) std::fprintf(stderr, "[spawncity] destruction digest differs from the host's (%u)\n", d.mismatches);
      }
      continue;
    }
    if (kind != kState) continue;
    const u8 full = in.get_u8(m.data);
    Craters c;
    DamageList dm;
    Points pt;
    if (!in.ok || !get_craters(in, m.data, c) || !get_damage(in, m.data, dm) || !get_points(in, m.data, pt)) continue;
    ++d.got_states;
    if (epoch != d.epoch) {
      if (!full && d.epoch != 0) {  // missed the start of it
        std::vector<u8> o;
        wire::put_u8(o, kAsk);
        wire::put_u32(o, d.epoch);
        net_.destroy_to_host(o);
        continue;
      }
      // the host's new epoch (a RESET there, or our first state): our city as his from scratch
      if (d.epoch != 0) { destruction_.reset(city_); d.mine.clear(); d.dmg_out.clear(); d.pts_out.clear(); }
      d.epoch = epoch;
    }
    d.sent = c;
    city_.merge_damage(dm, false);
    destruction_.merge_points(pt, false);
    (void)city_.take_damage_log();      // (merged quietly - nothing of ours to tell)
    (void)destruction_.take_points_log();
  }

  if (host) {
    // a mate who joined gets the whole state once
    for (const net::Remote& r : net_.remotes())
      if (std::find(d.known.begin(), d.known.end(), r.state.slot) == d.known.end()) { dsync_send_full(r.state.slot); d.known.push_back(r.state.slot); }
    d.known.erase(std::remove_if(d.known.begin(), d.known.end(), [&](u8 s) { return net_.remote_index(s) < 0; }), d.known.end());
    d.delta_clock += dt;
    if (d.delta_clock >= kDeltaEvery) {
      d.delta_clock = 0;
      Craters c;
      terrain.crater_list(c);
      if (c != d.sent || !d.dmg_out.empty() || !d.pts_out.empty()) {
        for (std::size_t a = 0; a == 0 || a < std::max(d.dmg_out.size(), d.pts_out.size()); a += kPerMessage) {
          std::vector<u8> o;
          wire::put_u8(o, kState);
          wire::put_u32(o, d.epoch);
          wire::put_u8(o, static_cast<u8>(0));
          put_craters(o, c);
          const std::size_t da = std::min(d.dmg_out.size(), a), pa = std::min(d.pts_out.size(), a);
          put_damage(o, d.dmg_out, da, std::min(kPerMessage, d.dmg_out.size() - da));
          put_points(o, d.pts_out, pa, std::min(kPerMessage, d.pts_out.size() - pa));
          net_.destroy_to_all(o, true);
          ++d.sent_states;
        }
        d.sent = c;
        d.dmg_out.clear();
        d.pts_out.clear();
      }
    }
    d.digest_clock += dt;
    if (d.digest_clock >= kDigestEvery) {
      d.digest_clock = 0;
      Craters c;
      terrain.crater_list(c);
      DamageList dm;
      city_.damage_list(dm);
      std::vector<u8> o;
      wire::put_u8(o, kDigest);
      wire::put_u32(o, d.epoch);
      wire::put_u32(o, digest_of(c, dm));
      net_.destroy_to_all(o, false);
    }
    return;
  }

  // client: the craters - the host's list, our own fresh ones kept on top until his list has them
  // (a crater dug here shows at once; after 3 s without the host taking it, it goes)
  Craters local;
  terrain.crater_list(local);
  for (const Terrain::CraterRec& r : local)
    if (!has(d.sent, r) && std::none_of(d.mine.begin(), d.mine.end(), [&](const DestroySync::Mine& q) { return q.r == r; })) d.mine.push_back({r, now, false});
  d.mine.erase(std::remove_if(d.mine.begin(), d.mine.end(), [&](const DestroySync::Mine& q) { return has(d.sent, q.r) || now - q.at > 3.0f; }), d.mine.end());
  if (d.epoch != 0) {
    Craters want = d.sent;
    for (const DestroySync::Mine& q : d.mine) want.push_back(q.r);
    while (want.size() > 32) want.erase(want.begin());
    terrain.set_craters(want);
  }
  // our own changes to the host (in one message per 150 ms)
  d.delta_clock += dt;
  std::vector<Terrain::CraterRec> tell;
  for (const DestroySync::Mine& q : d.mine) if (!q.told) tell.push_back(q.r);
  if (d.epoch == 0 || d.delta_clock < kDeltaEvery || (tell.empty() && d.dmg_out.empty() && d.pts_out.empty())) return;  // (the host's state first)
  d.delta_clock = 0;
  std::vector<u8> o;
  wire::put_u8(o, kEdit);
  wire::put_u32(o, d.epoch);
  put_craters(o, tell);
  const std::size_t nd = std::min(d.dmg_out.size(), kPerMessage), np = std::min(d.pts_out.size(), kPerMessage);
  put_damage(o, d.dmg_out, 0, nd);
  put_points(o, d.pts_out, 0, np);
  net_.destroy_to_host(o);
  d.told_at = now;
  for (DestroySync::Mine& q : d.mine) q.told = true;
  d.dmg_out.erase(d.dmg_out.begin(), d.dmg_out.begin() + static_cast<std::ptrdiff_t>(nd));
  d.pts_out.erase(d.pts_out.begin(), d.pts_out.begin() + static_cast<std::ptrdiff_t>(np));
}

std::string Game::dsync_json() {
  dsync_ready();
  DestroySync& d = *dsync_;
  Craters c;
  city_.terrain().crater_list(c);
  DamageList dm;
  city_.damage_list(dm);
  char b[256];
  std::snprintf(b, sizeof b, "{\"epoch\":%u,\"craters\":%zu,\"damaged\":%zu,\"digest\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"pending\":%zu,\"mismatches\":%u}", d.epoch, c.size(), dm.size(),
                digest_of(c, dm), d.sent_states, d.got_states, d.asks, d.mine.size(), d.mismatches);
  return b;
}

}  // namespace sc
