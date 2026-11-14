// SpawnCity on Kern - thrown things in multiplayer: what one player grabs and throws (a passer-by,
// a dog, a cat, a cow, a pigeon) flies on every machine, with physics.
//
// A body in someone's hand is driven by that hand - on one machine. The state-of-the-art way to
// share such a body is ownership: the machine whose hand took it owns it until it lies still
// again, simulates it, and streams its pose and velocity (20 Hz); every other machine runs the
// same body in its own Box3D world and steers it onto the owner's track by its velocity (a
// correction, not a teleport: it still collides, tumbles and is hit there). So the throw is
// physics everywhere - the same arc, the same landing - and nobody's world disagrees about where
// the dog came down.
//  * a passer-by: the host's citizen by his index (the clients' are puppets of his: People.net);
//    he goes down as a ragdoll on every machine, the owner's chest drives the others' dolls
//  * a cow by its key, a dog / cat by the host's index (game_worldsync.cpp: everybody's animals
//    are the host's); the others make the same body (Life::net_carry_*) and steer it
//  * a pigeon: the owner's bird, in his hand or flying off - drawn on every machine
// The last message of a toss says "done" (sent three times: unreliable, 20 Hz): the bodies are let
// go there and settle on their own (a doll gets up, an animal stands); a toss nobody hears of for
// half a second is let go too.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstring>

#include "game.h"
#include "game_life.h"
#include "mp_rules.h"

namespace sc {

namespace {
constexpr u8 kPerson = 0, kCow = 1, kPet = 2, kBird = 3;
constexpr u8 kHeld = 1, kDone = 2;
constexpr f32 kSendEvery = 0.05f, kQuiet = 0.5f;

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
kern::i16 q16(f32 v) { return static_cast<kern::i16>(std::lround(clampf(v, -1, 1) * 32767)); }
f32 uq16(kern::i16 v) { return static_cast<f32>(v) / 32767; }
kern::i16 cms(f32 v) { return static_cast<kern::i16>(std::lround(clampf(v * 100, -32000, 32000))); }
}  // namespace

TossState& Game::toss() {
  if (!toss_) toss_ = std::make_unique<TossState>();
  return *toss_;
}

// The hand took something (game_vr.cpp vr_grab): ours to stream until it lies still.
void Game::toss_own(u8 kind, i32 local, u64 id) {
  if (!net_.active()) return;
  TossState& t = toss();
  u64 net_id = id;
  if (kind == kPerson) {
    if (local < 0 || local >= static_cast<i32>(people_.all().size())) return;
    // the host's citizen: his index (on the host ours, on a client the puppet's)
    net_id = net_.hosting() ? static_cast<u64>(local) : static_cast<u64>(people_.all()[static_cast<std::size_t>(local)].net);
    if (!net_.hosting() && people_.all()[static_cast<std::size_t>(local)].net < 0) return;  // not one of the shared ones
  } else if (kind == kBird) {
    net_id = (static_cast<u64>(net_.self_slot()) << 32) | static_cast<u32>(local);
  }
  for (TossState::Own& o : t.own)
    if (o.kind == kind && o.id == net_id) { o.still = 0; o.done_sent = 0; o.local = local; return; }
  t.own.push_back({kind, net_id, local, 0, world_->sim_time(), 0});
}

void Game::toss_own_body(kern::physics::Body b) {
  if (!glife_ || !net_.active()) return;
  u64 id = 0;
  const u8 a = glife_->life.animal_of(b, id);
  if (a == 1) toss_own(kCow, -1, id);
  else if (a == 2) toss_own(kPet, static_cast<i32>(id), id);
}

void Game::toss_update(f32 dt) {
  TossState& t = toss();
  if (!net_.active() || !glife_) { t.own.clear(); t.remote.clear(); (void)net_.take_toss(); return; }
  kern::physics::World& w = world_->physics();
  Life& L = glife_->life;
  const f32 now = world_->sim_time();
  auto body_ok = [&](kern::physics::Body b) { return b.valid() && w.body_valid(b) && w.body_type(b) == kern::physics::BodyType::dynamic; };
  auto held_here = [&](const TossState::Own& o) {
    for (const VrHand& h : vr_.hands) {
      if (o.kind == kPerson && h.grab_person == o.local) return true;
      if (o.kind == kBird && h.bird == o.local) return true;
      if ((o.kind == kCow || o.kind == kPet) && h.grab.valid()) { u64 id = 0; const u8 a = L.animal_of(h.grab, id); if ((a == 1 && o.kind == kCow && id == o.id) || (a == 2 && o.kind == kPet && id == o.id)) return true; }
    }
    return false;
  };
  // ---- ours: where they are now (their body), still in a hand, lying still yet
  struct Rec { u8 kind, flags; u64 id; Vec3 p, v; Quat q; };
  std::vector<Rec> recs;
  for (TossState::Own& o : t.own) {
    Rec r{o.kind, 0, o.id, {}, {}, {}};
    bool have = false;
    if (o.kind == kBird) {
      for (const Life::LooseBird& b : L.loose_birds())
        if (b.id == o.local && !b.net) { r.p = b.pos; r.v = b.vel; r.q = Quat::axis_angle({0, 0, 1}, b.yaw); have = true; }
      if (have && !held_here(o) && now - o.born > 0.2f) o.still += dt;  // let go: it flies off by itself everywhere
      if (o.still > 1.0f) have = false;
    } else {
      kern::physics::Body b;
      if (o.kind == kPerson) b = L.ragdolls().bone_body(static_cast<u64>(o.local) + 1, Ragdolls::chest);
      else if (o.kind == kCow) b = L.cow_body(o.id);
      else b = L.pet_body(static_cast<u32>(o.id));
      if (body_ok(b)) {
        const kern::physics::Pose ps = w.pose(b);
        r.p = ps.position;
        r.q = ps.rotation;
        r.v = w.linear_velocity(b);
        have = true;
        o.still = !held_here(o) && kern::length(r.v) < 0.3f ? o.still + dt : 0;
        if (o.still > 1.0f) have = false;  // it lies still: the toss is over
      } else if (now - o.born < 0.5f) {
        continue;  // the doll / body comes next frame
      }
    }
    if (held_here(o)) r.flags |= kHeld;
    if (!have) { r.flags |= kDone; ++o.done_sent; }
    recs.push_back(r);
  }
  t.clock += dt;
  if (t.clock >= kSendEvery && !recs.empty()) {
    t.clock = 0;
    Out o;
    o.put(static_cast<u32>(std::lround(static_cast<f64>(now) * 1000)));  // our clock: the others play the track on it
    o.put(static_cast<u8>(std::min<std::size_t>(recs.size(), 16)));
    for (std::size_t i = 0; i < recs.size() && i < 16; ++i) {
      const Rec& r = recs[i];
      o.put(r.kind).put(r.flags).put(r.id).put(r.p.x).put(r.p.y).put(r.p.z);
      o.put(q16(r.q.x)).put(q16(r.q.y)).put(q16(r.q.z)).put(q16(r.q.w));
      o.put(cms(r.v.x)).put(cms(r.v.y)).put(cms(r.v.z));
    }
    net_.send_toss(o.b);
    ++t.sent;
  }
  t.own.erase(std::remove_if(t.own.begin(), t.own.end(), [](const TossState::Own& o) { return o.done_sent >= 3; }), t.own.end());

  // ---- theirs: the newest record of every toss
  for (const net::Session::Blob& m : net_.take_toss()) {
    In in{m.data};
    const u32 t_ms = in.get<u32>();
    const u8 n = in.get<u8>();
    for (u8 k = 0; k < n && in.ok; ++k) {
      TossState::Remote r;
      r.from = m.from;
      r.kind = in.get<u8>();
      r.flags = in.get<u8>();
      r.id = in.get<u64>();
      r.p.x = in.get<f32>(); r.p.y = in.get<f32>(); r.p.z = in.get<f32>();
      Quat q;
      q.x = uq16(in.get<kern::i16>()); q.y = uq16(in.get<kern::i16>()); q.z = uq16(in.get<kern::i16>()); q.w = uq16(in.get<kern::i16>());
      const f32 ql = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
      r.q = ql > 1e-6f ? Quat{q.x / ql, q.y / ql, q.z / ql, q.w / ql} : Quat{};
      r.v.x = in.get<kern::i16>() / 100.0f; r.v.y = in.get<kern::i16>() / 100.0f; r.v.z = in.get<kern::i16>() / 100.0f;
      if (!in.ok || !std::isfinite(r.p.x) || !std::isfinite(r.p.y) || !std::isfinite(r.p.z)) break;
      r.at = now;
      ++t.got;
      auto it = std::find_if(t.remote.begin(), t.remote.end(), [&](const TossState::Remote& q2) { return q2.from == r.from && q2.kind == r.kind && q2.id == r.id; });
      // the track on his clock (mp::PoseTimeline: a jitter buffer behind it, interpolated) - the
      // body here follows the throw as he threw it, not as the packets happened to arrive
      // (a body heard of before keeps its track: handed on to the new record, not copied)
      if (it == t.remote.end()) { r.line.push(r.p, r.q, r.v, true, t_ms, static_cast<f64>(now) * 1000); t.remote.push_back(std::move(r)); }
      else { r.local = it->local; r.line = std::move(it->line); r.line.push(r.p, r.q, r.v, true, t_ms, static_cast<f64>(now) * 1000); *it = std::move(r); }
    }
  }
  // ---- theirs: our copy of each body steered onto their track
  for (TossState::Remote& r : t.remote) {
    const bool live = !(r.flags & kDone) && now - r.at < kQuiet;
    if (r.kind == kBird) {
      Vec3 bp = r.p;
      { const mp::PoseSample smp = r.line.sample(static_cast<f64>(now) * 1000); if (smp.valid) bp = smp.p; }
      L.net_bird(r.id, bp, std::atan2(r.q.rotate({1, 0, 0}).y, r.q.rotate({1, 0, 0}).x), r.v, live && (r.flags & kHeld));
      continue;
    }
    if (!live) continue;
    kern::physics::Body b;
    if (r.kind == kPerson) {
      // the host's citizen here: on the host himself, on a client the puppet of him
      if (r.local < 0) {
        const auto& all = people_.all();
        for (std::size_t i = 0; i < all.size(); ++i)
          if (net_.hosting() ? static_cast<u64>(i) == r.id : (all[i].net >= 0 && static_cast<u64>(all[i].net) == r.id)) { r.local = static_cast<i32>(i); break; }
        if (r.local < 0) continue;
      }
      b = L.ragdolls().bone_body(static_cast<u64>(r.local) + 1, Ragdolls::chest);
      if (!body_ok(b)) {
        // down as a ragdoll here too (the doll appears next frame), then steered
        if (!L.ragdolls().has(static_cast<u64>(r.local) + 1)) people_.grab_knock(r.local, r.v * 0.2f, now);
        continue;
      }
    } else if (r.kind == kCow) {
      b = L.net_carry_cow(r.id);
    } else {
      b = L.net_carry_pet(static_cast<u32>(r.id));
    }
    if (!body_ok(b)) continue;
    // steering: his velocity plus a pull onto his place (a spring of ~0.1 s), the turn likewise
    const kern::physics::Pose ps = w.pose(b);
    Vec3 want_p = r.p + r.v * (now - r.at);
    Quat want_q = r.q;
    { const mp::PoseSample smp = r.line.sample(static_cast<f64>(now) * 1000); if (smp.valid) { want_p = smp.p; want_q = smp.q; } }
    const Vec3 off = want_p - ps.position;
    if (kern::length(off) > 6) { w.set_pose(b, {want_p, want_q}); w.set_linear_velocity(b, r.v); continue; }  // too far apart (a hitch): there
    w.set_linear_velocity(b, r.v + off * 10.0f);
    Quat e = want_q * Quat{-ps.rotation.x, -ps.rotation.y, -ps.rotation.z, ps.rotation.w};
    if (e.w < 0) e = Quat{-e.x, -e.y, -e.z, -e.w};
    w.set_angular_velocity(b, Vec3{e.x, e.y, e.z} * 2.0f * 8.0f);
    w.set_awake(b, true);
    if (r.kind == kCow || r.kind == kPet) L.hold_animal(b);  // it stands up only once he lets it settle
  }
  t.remote.erase(std::remove_if(t.remote.begin(), t.remote.end(), [&](const TossState::Remote& r) { return (r.flags & kDone || now - r.at > kQuiet) && now - r.at > 1.5f; }), t.remote.end());
}

// tests: "toss person" - the nearest citizen goes down as ours (as a hand would take him);
// "toss fling X Y Z" - his doll flies; "toss where I" - the chest of the host's citizen I here
std::string Game::toss_command(const std::string& cmd) {
  if (!glife_) return "no life";
  Life& L = glife_->life;
  kern::physics::World& w = world_->physics();
  const f32 now = world_->sim_time();
  if (cmd == "toss person") {
    const Vec3 me = player_position();
    i32 best = -1;
    f32 bd = 60;
    const auto& all = people_.all();
    for (std::size_t i = 0; i < all.size(); ++i) {
      const People::Person& p = all[i];
      if (p.state == People::State::down || p.state == People::State::gone || now < p.knocked_until) continue;
      if (!net_.hosting() && p.net < 0) continue;
      const f32 d = kern::length(p.pos - me);
      if (d < bd) { bd = d; best = static_cast<i32>(i); }
    }
    if (best < 0) return "nobody";
    people_.grab_knock(best, {0, 0, 0.6f}, now);
    toss_own(0, best, 0);
    toss_test_ = best;
    return std::to_string(net_.hosting() ? best : people_.all()[static_cast<std::size_t>(best)].net);
  }
  if (cmd.rfind("toss fling ", 0) == 0) {
    Vec3 v;
    if (toss_test_ < 0 || std::sscanf(cmd.c_str() + 11, "%f %f %f", &v.x, &v.y, &v.z) != 3) return "usage";
    for (u8 b = 0; b < Ragdolls::kBones; ++b) {
      const kern::physics::Body bb = L.ragdolls().bone_body(static_cast<u64>(toss_test_) + 1, static_cast<Ragdolls::Bone>(b));
      if (bb.valid() && w.body_valid(bb)) { w.set_linear_velocity(bb, v); w.set_awake(bb, true); }
    }
    return "ok";
  }
  if (cmd.rfind("toss where ", 0) == 0) {
    const i32 id = std::atoi(cmd.c_str() + 11);
    const auto& all = people_.all();
    for (std::size_t i = 0; i < all.size(); ++i)
      if (net_.hosting() ? static_cast<i32>(i) == id : all[i].net == id) {
        const kern::physics::Body b = L.ragdolls().bone_body(static_cast<u64>(i) + 1, Ragdolls::chest);
        if (!(b.valid() && w.body_valid(b))) return "{\"doll\":false}";
        const Vec3 p = w.pose(b).position;
        char o[96];
        std::snprintf(o, sizeof o, "{\"doll\":true,\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}", p.x, p.y, p.z);
        return o;
      }
    return "{\"none\":true}";
  }
  if (cmd.rfind("toss fire ", 0) == 0) {  // tests: a round of weapon N from the eye, up and ahead
    const i32 id = std::atoi(cmd.c_str() + 10);
    Vec3 dir;
    const Vec3 eye = aim_ray(dir);
    const Vec3 d = kern::normalize(dir + Vec3{0, 0, 0.35f}, dir);
    fire_projectile(static_cast<weapons::Id>(id), eye + d * 1.2f, d * (id == 7 ? 14.0f : id == 4 ? 22.0f : 40.0f), true, body(soldier_));
    return "fired";
  }
  if (cmd == "toss animals") {
    // tests: the cows (key low bits, x, y), the dogs and cats shown, every pigeon (x, y)
    std::string o = "{\"epoch\":" + std::to_string(L.epoch()) + ",\"cows\":[";
    char t[96];
    bool first = true;
    for (const Life::Cow& c : L.cows()) { std::snprintf(t, sizeof t, "%s[%u,%.1f,%.1f]", first ? "" : ",", static_cast<u32>(c.key & 0xffff), c.pos.x, c.pos.y); o += t; first = false; }
    o += "],\"pets\":[";
    first = true;
    for (std::size_t i = 0; i < L.pets().size(); ++i) { const Life::Pet& p = L.pets()[i]; if (!p.shown) continue; std::snprintf(t, sizeof t, "%s[%zu,%.1f,%.1f]", first ? "" : ",", i, p.pos.x, p.pos.y); o += t; first = false; }
    o += "],\"birds\":[";
    first = true;
    for (const Life::Flock& f : L.flocks()) for (const Life::Pigeon& b : f.birds) { std::snprintf(t, sizeof t, "%s[%.1f,%.1f]", first ? "" : ",", b.pos.x, b.pos.y); o += t; first = false; }
    return o + "]}";
  }
  return toss_json();
}

std::string Game::toss_json() {
  TossState& t = toss();
  char b[256];
  u32 ghosts = 0, rounds = 0;
  Vec3 gp;
  for (const Projectile& p : weapons_.projectiles) { if (!p.alive) continue; if (p.ghost) { ++ghosts; gp = p.pos; } else ++rounds; }
  std::snprintf(b, sizeof b, "{\"own\":%zu,\"remote\":%zu,\"sent\":%u,\"got\":%u,\"rounds\":%u,\"ghosts\":%u,\"ghost\":[%.2f,%.2f,%.2f]}", t.own.size(), t.remote.size(), t.sent, t.got, rounds, ghosts, gp.x, gp.y, gp.z);
  return b;
}

}  // namespace sc
