// SpawnCity on Kern - one world in multiplayer: the host's traffic and citizens are everybody's.
//
// Alone every machine runs its own town (traffic placed around the player, citizens on the
// pavements). In a session that would be one town per player, drifting apart: a car here is not
// there, a passer-by walks elsewhere. So the host is the authority of the street life near him:
//  * every 100 ms he sends the cars and the citizens within reach of any mate (unreliable, the
//    newest wins): a car's index, kind, colour, pose and velocity; a citizen's index, seed (his
//    looks), state, place, heading and pace
//  * the host loads the town around every mate too (CityStream::extra_foci) and places his
//    traffic and people around all of them; a client runs no traffic and no citizens of his own
//    while the host's world comes - his cars are puppets steered onto the host's poses
//    (dynamic bodies driven by velocity: they still push and get pushed), his citizens are eased
//    to the host's places, a death or a fall there is one here (People::apply_snapshot)
//  * the car a player drives is his (PlayerState.car): the host leaves it out and hides his copy,
//    the others draw it as that player's car; let go, it is the host's again where it stopped
//  * only until the host's world reaches a client (the first seconds there, the town loading on
//    the host) the client's own town fills the gap; then it is the host's, everywhere
// Police, the military and the zombies stay with the player they belong to (wanted, swarm).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game.h"
#include "game_life.h"

namespace sc {

namespace {
using i16 = std::int16_t;
using i8 = std::int8_t;
inline f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
constexpr f32 kSendEvery = 0.2f;     // s between two snapshots of the host's cars
constexpr f32 kPeopleEvery = 0.4f;   // ... of his citizens and joggers
constexpr f32 kReach = 200;          // m around a mate: what he can see of the street life
constexpr u8 kCars = 1, kPeople = 2, kJoggers = 3, kAnimals = 4;

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
  // the host is the authority over the whole town - around every player (he loads it around his
  // mates too: CityStream::extra_foci): a client lives in his world as soon as it comes and as
  // long as it keeps coming (3 s); only before the host has the town around him (the first
  // seconds after a long trip) or with a host that sends none, his own fills the gap
  if (!net_.active() || net_.hosting()) return false;
  return world_->sim_time() - world_heard_at_ < 3.0f;
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
    people_send_clock_ += dt;
    const std::vector<Vec3> mates = mate_foci();
    // the town is loaded around them too (their tile and the ring; one tile on a phone)
    city_.extra_foci = mates;
    city_.mate_ring = mobile_ ? 0 : 1;
    // Small on the wire (a relay over the brokers carries it when no direct channel is open): the
    // cars 5 times a second, the citizens and the joggers 2.5 times, each in one message, places
    // relative to the message's anchor in 10 cm, headings in bytes - some 1 KB/s
    auto near_mate = [&](Vec3 p) {
      for (const Vec3& m : mates) if (dist2d(p, m) < kReach) return true;
      return false;
    };
    const Vec2 o = mates.empty() ? Vec2{} : Vec2{std::round(mates[0].x), std::round(mates[0].y)};
    auto dm = [&](f32 v, f32 base) { return static_cast<i16>(std::lround(clampf((v - base) * 10, -32000, 32000))); };
    auto ang8 = [](f32 a) { return static_cast<u8>(static_cast<i32>(std::lround(wrap_pi(a) / (2 * kPi) * 256)) & 255); };
    if (world_send_clock_ >= kSendEvery && !mates.empty()) {
      world_send_clock_ = 0;
      Wr cw;
      cw.put(kCars);
      cw.put<u8>(0);
      cw.put(o.x); cw.put(o.y);
      u8 n = 0;
      // the nearest to a mate first (at most 32: what he sees around him)
      std::vector<std::pair<f32, std::size_t>> order;
      for (std::size_t i = 0; i < cars_.size() && i < 255; ++i) {
        const TrafficCar& c = cars_[i];
        if (!c.placed || c.police || !world_->actor_alive(c.actor) || (i < car_lent_.size() && car_lent_[i])) continue;
        if (mode_ == Mode::car && static_cast<i32>(i) == car_index_) continue;  // ours: drawn as our car
        const Vec3 cpos = matrix_of(c.actor).translation();
        f32 d = 1e9f;
        for (const Vec3& mt : mates) d = std::fmin(d, dist2d(cpos, mt));
        if (d < kReach) order.push_back({d, i});
      }
      std::sort(order.begin(), order.end());
      for (const auto& [dist, i] : order) {
        if (n >= 32) break;
        const TrafficCar& c = cars_[i];
        const BodyState s = state_of(c.actor);
        cw.put(static_cast<u8>(i));
        cw.put(static_cast<u8>((c.car->spec().radius > 0.4f ? 1 : 0) | (c.wreck ? 2 : 0) | (c.input.pedal < -0.05f ? 4 : 0) | (c.burn_until >= 0 ? 8 : 0)));
        const u16 rgb565 = static_cast<u16>(((col8(c.color.x) >> 3) << 11) | ((col8(c.color.y) >> 2) << 5) | (col8(c.color.z) >> 3));
        cw.put(rgb565);
        cw.put(dm(s.position.x, o.x)); cw.put(dm(s.position.y, o.y)); cw.put(dm(s.position.z, 0));
        // heading, pitch and roll of the body (x forward)
        const Vec3 f = s.rotation.rotate({1, 0, 0}), l = s.rotation.rotate({0, 1, 0});
        cw.put(ang8(std::atan2(f.y, f.x)));
        cw.put(static_cast<i8>(std::lround(clampf(std::asin(clampf(f.z, -1, 1)) / kPi * 127, -127, 127))));
        cw.put(static_cast<i8>(std::lround(clampf(std::asin(clampf(l.z, -1, 1)) / kPi * 127, -127, 127))));
        cw.put(cms(s.velocity.x)); cw.put(cms(s.velocity.y));
        ++n;
      }
      cw.b[1] = static_cast<u8>(n | 0x80);
      // nothing yet around the mates (the town loading there): his own town stays; one empty
      // snapshot after cars clears them there
      if (n > 0 || cars_were_) net_.send_world(cw.b);
      cars_were_ = n > 0;
      ++world_sent_;
    }
    if (people_send_clock_ >= kPeopleEvery && !mates.empty()) {
      people_send_clock_ = 0;
      Wr pw;
      pw.put(kPeople);
      pw.put<u8>(0);
      pw.put(o.x); pw.put(o.y);
      u8 m = 0;
      const auto& all = people_.all();
      for (std::size_t i = 0; i < all.size() && i < 255 && m < 48; ++i) {
        const People::Person& p = all[i];
        if (p.kind != People::Kind::citizen || p.state == People::State::gone || !near_mate(p.pos)) continue;
        pw.put(static_cast<u8>(i));
        pw.put(static_cast<u8>((static_cast<u8>(p.state) & 15) | (now < p.knocked_until ? 16 : 0)));
        pw.put(p.seed);
        pw.put(dm(p.pos.x, o.x)); pw.put(dm(p.pos.y, o.y));
        pw.put(ang8(p.yaw));
        pw.put(static_cast<u8>(std::lround(clampf(p.v * 20, 0, 255))));
        ++m;
      }
      pw.b[1] = static_cast<u8>(m | 0x80);
      if (m > 0) net_.send_world(pw.b);
      // the animals: the cows near a mate (by key), every dog and cat (by index), every flock with
      // its birds - the clients' are these (Life::apply_animals)
      if (glife_) {
        const Life& L = glife_->life;
        Wr aw;
        aw.put(kAnimals);
        aw.put<u8>(0x80);
        aw.put(o.x); aw.put(o.y);
        auto quat = [&](const Quat& q) { aw.put(q16(q.x)); aw.put(q16(q.y)); aw.put(q16(q.z)); aw.put(q16(q.w)); };
        std::vector<const Life::Cow*> cows;
        for (const Life::Cow& c : L.cows()) if (near_mate(c.pos) && cows.size() < 40) cows.push_back(&c);
        aw.put(static_cast<u8>(cows.size()));
        for (const Life::Cow* c : cows) {
          aw.put(c->key);
          aw.put(dm(c->pos.x, o.x)); aw.put(dm(c->pos.y, o.y)); aw.put(dm(c->pos.z, 0));
          aw.put(ang8(c->yaw));
          const u8 fl = static_cast<u8>((c->dead ? 1 : 0) | (c->walking ? 2 : 0) | (c->carried ? 4 : 0));
          aw.put(fl);
          if (fl & 5) quat(c->rot);
        }
        const auto& pets = L.pets();
        aw.put(L.pet_anchor().x); aw.put(L.pet_anchor().y);
        aw.put(static_cast<u8>(std::min<std::size_t>(pets.size(), 255)));
        u8 np = 0;
        const std::size_t np_at = aw.b.size();
        aw.put<u8>(0);
        for (std::size_t i = 0; i < pets.size() && i < 255; ++i) {
          const Life::Pet& p = pets[i];
          if (!p.shown && p.dead_at < 0) continue;
          aw.put(static_cast<u8>(i));
          aw.put(dm(p.pos.x, o.x)); aw.put(dm(p.pos.y, o.y)); aw.put(dm(p.pos.z, 0));
          aw.put(ang8(p.yaw));
          aw.put(dm(p.off.x, 0)); aw.put(dm(p.off.y, 0));
          const u8 fl = static_cast<u8>((p.shown ? 1 : 0) | (p.dead_at >= 0 ? 2 : 0) | (p.carried ? 4 : 0));
          aw.put(fl);
          if (fl & 6) quat(p.rot);
          ++np;
        }
        aw.b[np_at] = np;
        const auto& flocks = L.flocks();
        aw.put(static_cast<u8>(std::min<std::size_t>(flocks.size(), 24)));
        for (std::size_t i = 0; i < flocks.size() && i < 24; ++i) {
          const Life::Flock& f = flocks[i];
          aw.put(static_cast<u8>(f.air ? 1 : 0));
          aw.put(dm(f.at.x, o.x)); aw.put(dm(f.at.y, o.y));
          u8 nb = 0;
          for (const Life::Pigeon& b : f.birds) nb += b.dead_at < 0 && nb < 20;
          aw.put(nb);
          u8 k = 0;
          for (const Life::Pigeon& b : f.birds) {
            if (b.dead_at >= 0 || k >= nb) continue;
            aw.put(dm(b.pos.x, o.x)); aw.put(dm(b.pos.y, o.y)); aw.put(dm(b.pos.z, 0));
            aw.put(ang8(b.yaw));
            ++k;
          }
        }
        net_.send_world(aw.b);
      }
      // the running club (jogger_chain.mjs): its id and its runners, when it runs near a mate
      if (glife_) {
        const Life::Chain* ch = glife_->life.chain();
        bool near = false;
        if (ch) for (const Life::Runner& rn : ch->runners) if (rn.alive && near_mate(rn.pos)) near = true;
        if (ch && near) {
          Wr jw;
          jw.put(kJoggers);
          jw.put<u8>(0x80 | 10);
          jw.put(o.x); jw.put(o.y);
          jw.put(ch->id);
          for (const Life::Runner& rn : ch->runners) {
            jw.put<u8>(rn.alive ? 1 : 0);
            jw.put(dm(rn.pos.x, o.x)); jw.put(dm(rn.pos.y, o.y));
            jw.put(ang8(rn.yaw));
          }
          net_.send_world(jw.b);
        }
      }
    }
  }
  if (!net_.hosting()) city_.extra_foci.clear();
  // ---- client: the host's town around us (or our own until his arrives) ----
  const bool puppet = world_puppet();
  if (puppet != puppet_world_) {
    puppet_world_ = puppet;
    people_.set_puppets(puppet);
    if (glife_) glife_->life.set_remote_joggers(puppet);
    if (glife_) glife_->life.set_remote_animals(puppet);
    car_net_.assign(cars_.size(), -1);
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      TrafficCar& c = cars_[i];
      if (c.police || static_cast<i32>(i) == car_index_) continue;
      if (puppet && c.placed) { world_->set_alive(c.actor, false); c.placed = false; }  // ours go: the host's come
    }
  }
  std::vector<std::vector<u8>> blobs = net_.take_world();
  world_got_ += static_cast<u32>(blobs.size());
  if (!blobs.empty()) world_heard_at_ = now;
  if (!puppet) return;
  if (car_net_.size() < cars_.size()) car_net_.resize(cars_.size(), -1);
  if (car_puppet_.size() < cars_.size()) car_puppet_.resize(cars_.size());
  for (const std::vector<u8>& blob : blobs) {
    Rd r{blob};
    const u8 type = r.get<u8>(), count = r.get<u8>();
    const bool last = count & 0x80;
    const u8 n = count & 0x7f;
    const f32 ox = r.get<f32>(), oy = r.get<f32>();
    if (!r.ok) continue;
    auto un8 = [](u8 a) { return static_cast<f32>(static_cast<i8>(a)) / 128 * kPi; };
    if (type == kCars) {
      std::vector<bool>& seen = car_seen_;
      if (seen.size() != cars_.size()) seen.assign(cars_.size(), false);
      for (u8 k = 0; k < n && r.ok; ++k) {
        const u8 idx = r.get<u8>(), flags = r.get<u8>();
        const u16 c565 = r.get<u16>();
        const Vec3 col{static_cast<f32>((c565 >> 11) & 31) / 31, static_cast<f32>((c565 >> 5) & 63) / 63, static_cast<f32>(c565 & 31) / 31};
        Vec3 p;
        p.x = ox + r.get<i16>() / 10.0f; p.y = oy + r.get<i16>() / 10.0f; p.z = r.get<i16>() / 10.0f;
        const f32 yaw = un8(r.get<u8>());
        const f32 pitch = static_cast<f32>(r.get<i8>()) / 127 * kPi, roll = static_cast<f32>(r.get<i8>()) / 127 * kPi;
        Vec3 v;
        v.x = r.get<i16>() / 100.0f; v.y = r.get<i16>() / 100.0f; v.z = 0;
        if (!r.ok) break;
        // heading about z, then the nose up (about -y: x forward towards z), then the roll about x
        const Quat q = Quat::axis_angle({0, 0, 1}, yaw) * Quat::axis_angle({0, 1, 0}, -pitch) * Quat::axis_angle({1, 0, 0}, roll);
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
          const Vec3 rot{0, 0, yaw / kDeg};
          world_->teleport(c.actor, p + Vec3{0, 0, 0.05f}, &rot);
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
    } else if (type == kAnimals) {
      auto quat = [&]() { Quat q; q.x = uq16(r.get<i16>()); q.y = uq16(r.get<i16>()); q.z = uq16(r.get<i16>()); q.w = uq16(r.get<i16>()); const f32 l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w); if (l > 1e-6f) { q.x /= l; q.y /= l; q.z /= l; q.w /= l; } else q = Quat{}; return q; };
      std::vector<Life::NetCow> cows;
      const u8 nc = r.get<u8>();
      for (u8 k = 0; k < nc && r.ok; ++k) {
        Life::NetCow c;
        c.key = r.get<u64>();
        c.pos.x = ox + r.get<i16>() / 10.0f; c.pos.y = oy + r.get<i16>() / 10.0f; c.pos.z = r.get<i16>() / 10.0f;
        c.yaw = un8(r.get<u8>());
        const u8 fl = r.get<u8>();
        c.dead = fl & 1; c.walking = fl & 2; c.carried = fl & 4;
        c.rot = (fl & 5) ? quat() : Quat::axis_angle({0, 0, 1}, c.yaw);
        cows.push_back(c);
      }
      Vec2 anchor;
      anchor.x = r.get<f32>(); anchor.y = r.get<f32>();
      const u8 pet_count = r.get<u8>(), npets = r.get<u8>();
      std::vector<Life::NetPet> pets;
      for (u8 k = 0; k < npets && r.ok; ++k) {
        Life::NetPet p;
        p.idx = r.get<u8>();
        p.pos.x = ox + r.get<i16>() / 10.0f; p.pos.y = oy + r.get<i16>() / 10.0f; p.pos.z = r.get<i16>() / 10.0f;
        p.yaw = un8(r.get<u8>());
        p.off.x = r.get<i16>() / 10.0f; p.off.y = r.get<i16>() / 10.0f;
        const u8 fl = r.get<u8>();
        p.shown = fl & 1; p.dead = fl & 2; p.carried = fl & 4;
        p.rot = (fl & 6) ? quat() : Quat::axis_angle({0, 0, 1}, p.yaw);
        pets.push_back(p);
      }
      std::vector<Life::NetFlock> flocks;
      const u8 nf = r.get<u8>();
      for (u8 k = 0; k < nf && r.ok; ++k) {
        Life::NetFlock f;
        f.idx = k;
        f.air = r.get<u8>() != 0;
        f.at.x = ox + r.get<i16>() / 10.0f; f.at.y = oy + r.get<i16>() / 10.0f;
        const u8 nb = r.get<u8>();
        for (u8 j = 0; j < nb && r.ok; ++j) {
          Vec3 b;
          b.x = ox + r.get<i16>() / 10.0f; b.y = oy + r.get<i16>() / 10.0f; b.z = r.get<i16>() / 10.0f;
          f.birds.push_back(b);
          f.yaws.push_back(un8(r.get<u8>()));
        }
        flocks.push_back(std::move(f));
      }
      if (r.ok && glife_) glife_->life.apply_animals(cows, pets, pet_count, anchor, flocks, now);
    } else if (type == kJoggers) {
      const u32 id = r.get<u32>();
      Vec3 pos[10];
      f32 yaw[10];
      bool alive[10];
      for (u32 k = 0; k < 10 && k < n; ++k) {
        alive[k] = r.get<u8>() != 0;
        pos[k].x = ox + r.get<i16>() / 10.0f; pos[k].y = oy + r.get<i16>() / 10.0f;
        pos[k].z = city_.ground_height(pos[k].x, pos[k].y);
        yaw[k] = un8(r.get<u8>());
      }
      if (r.ok && glife_) glife_->life.apply_joggers(id, pos, yaw, alive, std::min<u32>(n, 10), now);
    } else if (type == kPeople) {
      std::vector<People::NetPerson>& list = people_acc_;
      for (u8 k = 0; k < n && r.ok; ++k) {
        People::NetPerson np;
        np.idx = r.get<u8>();
        const u8 st = r.get<u8>();
        np.state = st & 15;
        np.flags = (st & 16) ? 1 : 0;
        np.seed = r.get<u32>();
        np.pos.x = ox + r.get<i16>() / 10.0f; np.pos.y = oy + r.get<i16>() / 10.0f;
        np.pos.z = city_.ground_height(np.pos.x, np.pos.y);
        np.yaw = un8(r.get<u8>());
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
  const f32 age = clampf(world_->sim_time() - cp.at, 0, 0.5f);
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
  // (the height is the car's own: its wheels on our ground - only the plan view is steered)
  const Vec3 lv = w.linear_velocity(b);
  w.set_linear_velocity(b, Vec3{cp.v.x + err.x * 4.0f, cp.v.y + err.y * 4.0f, lv.z});
  // the turn: the heading only (pitch and roll are the car's own on our ground; a wreck tumbling
  // in his world is put as he has it by the jump above when it is far off)
  const Vec3 fh = cp.q.rotate({1, 0, 0}), fo = s.rotation.rotate({1, 0, 0});
  const f32 dyaw = wrap_pi(std::atan2(fh.y, fh.x) - std::atan2(fo.y, fo.x));
  const Vec3 av = w.angular_velocity(b);
  w.set_angular_velocity(b, Vec3{av.x, av.y, dyaw * 6.0f});
  w.set_awake(b, true);
  (void)h;
  return true;
}

}  // namespace sc
