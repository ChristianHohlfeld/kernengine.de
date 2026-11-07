// SpawnCity on Kern - one world in multiplayer: the host's traffic and citizens are everybody's.
//
// Alone every machine runs its own town (traffic placed around the player, citizens on the
// pavements). In a session that would be one town per player, drifting apart: a car here is not
// there, a passer-by walks elsewhere. So the host is the authority of the street life near him:
//  * every 100 ms he sends the cars and the citizens within reach of any mate (unreliable, the
//    newest wins): a car's index, kind, colour, pose and velocity; a citizen's index, seed (his
//    looks), state, place, heading and pace
//  * a client near the host (within 350 m: the host's streamed city reaches there) runs no
//    traffic and no citizens of his own - his cars are puppets steered onto the host's poses
//    (dynamic bodies driven by velocity: they still push and get pushed), his citizens are eased
//    to the host's places, a death or a fall there is one here (People::apply_snapshot)
//  * the car a player drives is his (PlayerState.car): the host leaves it out and hides his copy,
//    the others draw it as that player's car; let go, it is the host's again where it stopped
//  * a client far from the host (his own end of the map) keeps a town of his own there
// Police, the military and the zombies stay with the player they belong to (wanted, swarm).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game.h"

namespace sc {

namespace {
using i16 = std::int16_t;
inline f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
constexpr f32 kSendEvery = 0.1f;     // s between two snapshots of the host's world
constexpr f32 kReach = 320;          // m around a mate: what he can see of the street life
constexpr f32 kNearHost = 350;       // m: a client this close to the host lives in the host's town
constexpr u8 kCars = 1, kPeople = 2;

struct Wr {
  std::vector<u8> b;
  template <class T> void put(T v) { const u8* p = reinterpret_cast<const u8*>(&v); b.insert(b.end(), p, p + sizeof(T)); }
};
struct Rd {
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
i16 q16(f32 v) { return static_cast<i16>(std::lround(clampf(v, -1, 1) * 32767)); }
f32 uq16(i16 v) { return static_cast<f32>(v) / 32767; }
i16 cms(f32 v) { return static_cast<i16>(std::lround(clampf(v * 100, -32000, 32000))); }
u8 col8(f32 v) { return static_cast<u8>(std::lround(clampf(v, 0, 1) * 255)); }
}  // namespace

std::vector<Vec3> Game::mate_foci() const {
  std::vector<Vec3> out;
  if (!net_.active()) return out;
  for (const net::Remote& r : net_.remotes()) {
    if (r.state.flags & net::kPassenger) continue;
    out.push_back(r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone ? r.person_shown : r.shown);
  }
  return out;
}

bool Game::world_puppet() const {
  // a client in the host's town (near him): the host's street life, none of his own
  if (!net_.active() || net_.hosting()) return false;
  const net::Remote* h = net_.remote(net::kHost);
  if (!h) return false;
  const Vec3 hp = h->state.mode == net::PlayerMode::foot || h->state.mode == net::PlayerMode::drone ? h->person_shown : h->shown;
  return dist2d(hp, focus_position()) < kNearHost;
}

void Game::worldsync_update(f32 dt) {
  if (!world_placed_) return;
  const f32 now = world_->sim_time();
  // ---- host: the snapshots ----
  if (net_.active() && net_.hosting()) {
    // a car a mate drives is his: our copy hidden while he has it, back where he left it after
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      TrafficCar& c = cars_[i];
      i32 owner = -1;
      const net::Remote* who = nullptr;
      for (const net::Remote& r : net_.remotes())
        if (r.state.mode == net::PlayerMode::car && r.state.car == i) { owner = r.state.slot; who = &r; }
      if (car_lent_.size() < cars_.size()) { car_lent_.resize(cars_.size(), false); car_lent_at_.resize(cars_.size()); }
      const bool was = car_lent_[i];
      if (owner >= 0) {
        if (!was && c.placed) world_->set_alive(c.actor, false);
        car_lent_[i] = true;
        car_lent_at_[i] = {who->shown, who->shown_rotation};
      } else if (was) {
        car_lent_[i] = false;
        // his car comes back to the town where he got out (the pose he last sent)
        const Vec3 p = car_lent_at_[i].first;
        const Vec3 f = car_lent_at_[i].second.rotate({1, 0, 0});
        const Vec3 rot{0, 0, std::atan2(f.y, f.x) / kDeg};
        world_->set_alive(c.actor, true);
        world_->teleport(c.actor, p, &rot);
        world_->set_actor_velocity(c.actor, {});
        c.placed = true;
        c.driver = false;  // parked where he left it
      }
    }
    world_send_clock_ += dt;
    const std::vector<Vec3> mates = mate_foci();
    if (world_send_clock_ >= kSendEvery && !mates.empty()) {
      world_send_clock_ = 0;
      auto near_mate = [&](Vec3 p) {
        for (const Vec3& m : mates) if (dist2d(p, m) < kReach) return true;
        return false;
      };
      // (in messages of at most 20 cars / 24 people: one datagram each, never fragmented)
      Wr cw;
      cw.put(kCars);
      cw.put<u8>(0);
      u8 n = 0;
      std::vector<std::vector<u8>> car_msgs;
      for (std::size_t i = 0; i < cars_.size() && i < 255; ++i) {
        if (n == 20) { cw.b[1] = n; car_msgs.push_back(cw.b); cw.b.resize(2); n = 0; }
        const TrafficCar& c = cars_[i];
        if (!c.placed || c.police || !world_->actor_alive(c.actor) || (i < car_lent_.size() && car_lent_[i])) continue;
        if (mode_ == Mode::car && static_cast<i32>(i) == car_index_) continue;  // ours: drawn as our car
        const BodyState s = state_of(c.actor);
        if (!near_mate(s.position)) continue;
        cw.put(static_cast<u8>(i));
        cw.put(static_cast<u8>((c.car->spec().radius > 0.4f ? 1 : 0) | (c.wreck ? 2 : 0) | (c.input.pedal < -0.05f ? 4 : 0) | (c.burn_until >= 0 ? 8 : 0)));
        cw.put(col8(c.color.x)); cw.put(col8(c.color.y)); cw.put(col8(c.color.z));
        cw.put(s.position.x); cw.put(s.position.y); cw.put(s.position.z);
        cw.put(q16(s.rotation.x)); cw.put(q16(s.rotation.y)); cw.put(q16(s.rotation.z)); cw.put(q16(s.rotation.w));
        cw.put(cms(s.velocity.x)); cw.put(cms(s.velocity.y)); cw.put(cms(s.velocity.z));
        ++n;
      }
      cw.b[1] = n;
      car_msgs.push_back(cw.b);
      // the last one of the cars says "that was all" (bit 7 of the count): the client drops the rest
      car_msgs.back()[1] |= 0x80;
      for (const auto& m : car_msgs) net_.send_world(m);
      ++world_sent_;
      Wr pw;
      pw.put(kPeople);
      pw.put<u8>(0);
      u8 m = 0;
      std::vector<std::vector<u8>> people_msgs;
      const auto& all = people_.all();
      for (std::size_t i = 0; i < all.size() && i < 255; ++i) {
        if (m == 24) { pw.b[1] = m; people_msgs.push_back(pw.b); pw.b.resize(2); m = 0; }
        const People::Person& p = all[i];
        if (p.kind != People::Kind::citizen || p.state == People::State::gone || !near_mate(p.pos)) continue;
        pw.put(static_cast<u8>(i));
        pw.put(static_cast<u8>(p.state));
        pw.put(static_cast<u8>(now < p.knocked_until ? 1 : 0));
        pw.put(p.seed);
        pw.put(p.pos.x); pw.put(p.pos.y); pw.put(p.pos.z);
        pw.put(static_cast<i16>(std::lround(wrap_pi(p.yaw) * 10000)));
        pw.put(static_cast<u8>(std::lround(clampf(p.v * 20, 0, 255))));
        ++m;
      }
      pw.b[1] = m;
      people_msgs.push_back(pw.b);
      people_msgs.back()[1] |= 0x80;
      for (const auto& mm : people_msgs) net_.send_world(mm);
    }
  }
  // ---- client: the host's town around us, or our own far from him ----
  const bool puppet = world_puppet();
  if (puppet != puppet_world_) {
    puppet_world_ = puppet;
    people_.set_puppets(puppet);
    car_net_.assign(cars_.size(), -1);
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      TrafficCar& c = cars_[i];
      if (c.police || static_cast<i32>(i) == car_index_) continue;
      if (puppet && c.placed) { world_->set_alive(c.actor, false); c.placed = false; }  // ours go: the host's come
    }
  }
  std::vector<std::vector<u8>> blobs = net_.take_world();
  world_got_ += static_cast<u32>(blobs.size());
  if (!puppet) return;
  if (car_net_.size() < cars_.size()) car_net_.resize(cars_.size(), -1);
  if (car_puppet_.size() < cars_.size()) car_puppet_.resize(cars_.size());
  for (const std::vector<u8>& blob : blobs) {
    Rd r{blob};
    const u8 type = r.get<u8>(), count = r.get<u8>();
    const bool last = count & 0x80;
    const u8 n = count & 0x7f;
    if (type == kCars) {
      std::vector<bool>& seen = car_seen_;
      if (seen.size() != cars_.size()) seen.assign(cars_.size(), false);
      for (u8 k = 0; k < n && r.ok; ++k) {
        const u8 idx = r.get<u8>(), flags = r.get<u8>();
        const Vec3 col{r.get<u8>() / 255.0f, r.get<u8>() / 255.0f, r.get<u8>() / 255.0f};
        Vec3 p;
        p.x = r.get<f32>(); p.y = r.get<f32>(); p.z = r.get<f32>();
        Quat q;
        q.x = uq16(r.get<i16>()); q.y = uq16(r.get<i16>()); q.z = uq16(r.get<i16>()); q.w = uq16(r.get<i16>());
        Vec3 v;
        v.x = r.get<i16>() / 100.0f; v.y = r.get<i16>() / 100.0f; v.z = r.get<i16>() / 100.0f;
        if (!r.ok) break;
        const f32 ql = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        if (ql < 1e-3f) continue;
        q = {q.x / ql, q.y / ql, q.z / ql, q.w / ql};
        // which of our cars plays his: the one that did, else a free one of the kind (car / bus)
        i32 local = -1;
        for (std::size_t i = 0; i < cars_.size(); ++i) if (car_net_[i] == idx) { local = static_cast<i32>(i); break; }
        const bool bus = flags & 1;
        if (local < 0)
          for (std::size_t i = 0; i < cars_.size(); ++i) {
            const TrafficCar& c = cars_[i];
            if (c.police || car_net_[i] >= 0 || static_cast<i32>(i) == car_index_ || c.placed || (c.car->spec().radius > 0.4f) != bus) continue;
            local = static_cast<i32>(i);
            break;
          }
        if (local < 0 || local == car_index_) continue;
        TrafficCar& c = cars_[static_cast<std::size_t>(local)];
        seen[static_cast<std::size_t>(local)] = true;
        CarPuppet& cp = car_puppet_[static_cast<std::size_t>(local)];
        if (car_net_[static_cast<std::size_t>(local)] != idx || !c.placed) {
          car_net_[static_cast<std::size_t>(local)] = idx;
          if (!world_->actor_alive(c.actor)) world_->set_alive(c.actor, true);
          const Vec3 f = q.rotate({1, 0, 0});
          const Vec3 rot{0, 0, std::atan2(f.y, f.x) / kDeg};
          world_->teleport(c.actor, p, &rot);
          world_->set_actor_velocity(c.actor, v);
          c.car->reset();
          c.placed = true;
          c.far = false;
        }
        c.driver = false;
        c.color = col;
        c.wreck = flags & 2;
        c.input.pedal = (flags & 4) ? -0.5f : 0;
        cp.p = p;
        cp.q = q;
        cp.v = v;
        cp.at = now;
      }
      // the snapshot's last part: the host's cars we no longer hear of are gone here too
      if (last) {
        for (std::size_t i = 0; i < cars_.size(); ++i) {
          if (seen[i] || car_net_[i] < 0 || static_cast<i32>(i) == car_index_) continue;
          car_net_[i] = -1;
          if (cars_[i].placed) { world_->set_alive(cars_[i].actor, false); cars_[i].placed = false; }
        }
        seen.assign(cars_.size(), false);
      }
    } else if (type == kPeople) {
      std::vector<People::NetPerson>& list = people_acc_;
      for (u8 k = 0; k < n && r.ok; ++k) {
        People::NetPerson np;
        np.idx = r.get<u8>();
        np.state = r.get<u8>();
        np.flags = r.get<u8>();
        np.seed = r.get<u32>();
        np.pos.x = r.get<f32>(); np.pos.y = r.get<f32>(); np.pos.z = r.get<f32>();
        np.yaw = r.get<i16>() / 10000.0f;
        np.v = r.get<u8>() / 20.0f;
        if (r.ok) list.push_back(np);
      }
      if (last) { people_.apply_snapshot(list, now); list.clear(); }
    }
  }
}

std::string Game::worldsync_list() const {
  // {"puppet":0/1,"cars":[[x,y,colour],..],"people":[[seed,x,y],..]} - the cars near us sorted
  std::string o = "{\"puppet\":" + std::string(puppet_world_ ? "1" : "0") + ",\"sent\":" + std::to_string(world_sent_) + ",\"got\":" + std::to_string(world_got_) + ",\"mates\":" + std::to_string(mate_foci().size()) + ",\"cars\":[";
  char b[96];
  bool first = true;
  const Vec3 me = focus_position();
  for (const TrafficCar& c : cars_) {
    if (!c.placed || c.police || !world_->actor_alive(c.actor)) continue;
    const Vec3 p = matrix_of(c.actor).translation();
    if (dist2d(p, me) > 260) continue;
    std::snprintf(b, sizeof b, "%s[%.1f,%.1f,%u]", first ? "" : ",", p.x, p.y, (col8(c.color.x) << 16) | (col8(c.color.y) << 8) | col8(c.color.z));
    o += b;
    first = false;
  }
  o += "],\"people\":[";
  first = true;
  for (const People::Person& p : people_.all()) {
    if (p.kind != People::Kind::citizen || p.state == People::State::gone || dist2d(p.pos, me) > 200) continue;
    std::snprintf(b, sizeof b, "%s[%u,%.1f,%.1f]", first ? "" : ",", p.seed, p.pos.x, p.pos.y);
    o += b;
    first = false;
  }
  return o + "]}";
}

bool Game::puppet_car_step(TrafficCar& c, std::size_t i, f32 h) {
  // A car of the host's town: a dynamic body steered onto his pose - the velocity he sent plus the
  // way to where he has it now (extrapolated over the time since), the turn the same way; far off
  // (a jump of his, a lost packet run) it is put there
  if (!puppet_world_ || i >= car_net_.size() || car_net_[i] < 0 || static_cast<i32>(i) == car_index_) return false;
  const kern::physics::Body b = body(c.actor);
  if (!b.valid()) return true;
  kern::physics::World& w = world_->physics();
  const CarPuppet& cp = car_puppet_[i];
  const f32 age = clampf(world_->sim_time() - cp.at, 0, 0.3f);
  const Vec3 want = cp.p + cp.v * age;
  const BodyState s = read_body(w, b);
  const Vec3 err = want - s.position;
  if (kern::length(err) > 6) {
    const Vec3 f = cp.q.rotate({1, 0, 0});
    const Vec3 rot{0, 0, std::atan2(f.y, f.x) / kDeg};
    world_->teleport(c.actor, want, &rot);
    world_->set_actor_velocity(c.actor, cp.v);
    return true;
  }
  w.set_linear_velocity(b, cp.v + err * 5.0f);
  // the turn: the shortest rotation from ours to his, as an angular velocity
  Quat d = cp.q * Quat{-s.rotation.x, -s.rotation.y, -s.rotation.z, s.rotation.w};
  if (d.w < 0) d = {-d.x, -d.y, -d.z, -d.w};
  const f32 sn = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
  if (sn > 1e-5f) {
    const f32 ang = 2 * std::atan2(sn, d.w);
    w.set_angular_velocity(b, Vec3{d.x, d.y, d.z} * (ang / sn * 6.0f));
  } else w.set_angular_velocity(b, {});
  w.set_awake(b, true);
  (void)h;
  return true;
}

}  // namespace sc
