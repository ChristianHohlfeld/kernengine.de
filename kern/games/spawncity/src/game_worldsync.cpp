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
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game.h"
#include "game_life.h"
#include "mp_rules.h"

namespace sc {

namespace {
using i16 = std::int16_t;
using i8 = std::int8_t;
inline f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
// Snapshot interpolation (Glenn Fiedler, "Snapshot Interpolation"): the receiver shows the sender's
// state a buffer behind - about three send intervals, so a lost snapshot leaves no gap. The cars
// (fast, turning) 20 times a second: 150 ms behind; the citizens and animals (slow, straight)
// five times a second, dead-reckoned in between.
constexpr f32 kSendEvery = 0.05f;    // s between two snapshots of the host's cars
constexpr f32 kPeopleEvery = 0.2f;   // ... of his citizens, animals and joggers
constexpr u32 kRestEvery = 20;       // a car at rest: its whole entry every 20th snapshot, its name in between
constexpr f32 kReach = 210;          // m around a mate: the citizens he can see (they are shown to 170 m, kept to 190 m)
constexpr f32 kCarReach = 460;       // ... the cars (the traffic lives within 330 m of a player; a wreck stays where it is)
constexpr f32 kAnimalReach = 320;    // ... the cows, dogs, cats and pigeons
constexpr u8 kCars = 1, kPeople = 2, kJoggers = 3, kCows = 5, kPets = 6, kFlocks = 7;  // (4 was the animals in one message)

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
  // The host is the authority over the whole town - around every player (he loads it around his
  // mates too: CityStream::extra_foci). A client in a session never runs street life of his own:
  // what he sees is what the host sends, from the first moment to the last (nothing yet, then his
  // world). A town of one's own beside the host's would be a second, different world.
  return net_.active() && !net_.hosting();
}

f64 Game::life_epoch() const {
  // the clock the animals' rounds run on: the wall clock - in a session the host's (a client
  // counts on from what the host last sent), so a dog is at the same place of its round everywhere
  if (net_.active() && !net_.hosting() && host_epoch_ok_) return kern::now_ms() / 1000 + host_epoch_off_;
  return static_cast<f64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / 1000.0;
}

void Game::car_body_dynamic(TrafficCar& c) {
  // a car that was the host's puppet (a kinematic body on his poses) is driven here again
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(c.actor);
  if (!b.valid() || !w.body_valid(b) || w.body_type(b) == kern::physics::BodyType::dynamic) return;
  w.set_body_type(b, kern::physics::BodyType::dynamic);
  const RigidSpec rs = Car::rigid_spec(c.car->spec());
  w.set_mass_data(b, rs.mass.mass, rs.mass.center, rs.mass.inertia);
  w.set_awake(b, true);
  c.car->reset();
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
    // (in real time: a host whose game runs slow still sends five times a second)
    {
      const f64 real = kern::now_ms();
      const f32 real_dt = world_send_real_ > 0 ? static_cast<f32>(std::min(1000.0, real - world_send_real_) / 1000) : dt;
      world_send_real_ = real;
      world_send_clock_ += real_dt;
      people_send_clock_ += real_dt;
    }
    const std::vector<Vec3> mates = mate_foci();
    // the town is loaded around them too (their tile and the ring; one tile on a phone)
    city_.extra_foci = mates;
    city_.mate_ring = mobile_ ? 0 : 1;
    // Every client gets the host's world as it is around him - all of it, nothing left out: the
    // cars, citizens, cows, dogs, cats and pigeons within the distance the host himself would
    // see them at. Each list is complete: it goes in as many messages as it takes (none over the
    // 1200 bytes an unreliable message may have - a longer one would not arrive at all), numbered,
    // the last one marked; a client that got every part knows the whole list - what it does not
    // name is not there. Places are relative to the client (5 cm, 1 cm up), on the host's clock.
    // Sent whether there is anything or not: an empty list is the truth too.
    const bool send_cars = world_send_clock_ >= kSendEvery, send_people = people_send_clock_ >= kPeopleEvery;
    if (send_cars) { world_send_clock_ = 0; ++world_tick_; }
    if (send_people) people_send_clock_ = 0;
    // (the real clock, not the game's: a machine whose game runs slow still stamps true time)
    const u32 t_ms = static_cast<u32>(static_cast<u64>(kern::now_ms()));
    auto ang16 = [](f32 a) { return static_cast<i16>(std::lround(wrap_pi(a) / kPi * 32767)); };
    auto ang8 = [](f32 a) { return static_cast<u8>(static_cast<i32>(std::lround(wrap_pi(a) / (2 * kPi) * 256)) & 255); };
    for (const net::Remote& rm : net_.remotes()) {
      if (!send_cars && !send_people) break;
      const u8 to = rm.state.slot;
      const Vec3 at = rm.state.mode == net::PlayerMode::foot || rm.state.mode == net::PlayerMode::drone ? rm.person_shown : rm.shown;
      const Vec3 o{std::round(at.x), std::round(at.y), std::round(at.z)};
      auto p5 = [](f32 v, f32 base) { return static_cast<i16>(std::lround(clampf((v - base) * 20, -32000, 32000))); };   // 5 cm
      auto p1 = [](f32 v, f32 base) { return static_cast<i16>(std::lround(clampf((v - base) * 100, -32000, 32000))); };  // 1 cm
      auto near = [&](Vec3 p, f32 reach) { return dist2d(p, at) < reach; };
      // one list: its entries packed into numbered parts of at most ~1100 bytes
      struct List {
        Game* g; u8 to, type; Vec3 o; u32 t_ms;
        std::vector<u8> extra;  // after the header of every part (the list's own header)
        Wr w;
        u8 n = 0, part = 0;
        void head() { w.b.clear(); w.put(type); w.put<u8>(0); w.put(part); w.put(o.x); w.put(o.y); w.put(o.z); w.put(t_ms); w.b.insert(w.b.end(), extra.begin(), extra.end()); n = 0; }
        void flush(bool last) { w.b[1] = static_cast<u8>(n | (last ? 0x80 : 0)); g->net_.send_world_to(to, w.b); ++part; head(); }
        void entry(const std::vector<u8>& e) { if (n >= 120 || w.b.size() + e.size() > 1100) flush(false); w.b.insert(w.b.end(), e.begin(), e.end()); ++n; }
      };
      auto list = [&](u8 type) { List l{this, to, type, o, t_ms, {}, {}, 0, 0}; return l; };
      if (send_cars) {
        List L = list(kCars);
        L.head();
        for (std::size_t i = 0; i < cars_.size() && i < 255; ++i) {
          const TrafficCar& c = cars_[i];
          if (!c.placed || c.police || !world_->actor_alive(c.actor) || (i < car_lent_.size() && car_lent_[i])) continue;
          if (mode_ == Mode::car && static_cast<i32>(i) == car_index_) continue;  // ours: drawn as our car
          const BodyState s = state_of(c.actor);
          if (!near(s.position, kCarReach)) continue;
          Wr e;
          e.put(static_cast<u8>(i));
          const u8 fl = static_cast<u8>((c.car->spec().radius > 0.4f ? 1 : 0) | (c.wreck ? 2 : 0) | (c.input.pedal < -0.05f ? 4 : 0) | (c.burn_until >= 0 ? 8 : 0));
          // at rest (standing, not turning): only named - it is where its last whole entry put it
          const bool rest = kern::length(s.velocity) < 0.03f && kern::length(s.angular) < 0.03f;
          if (rest && (world_tick_ + i) % kRestEvery != 0) {
            e.put(static_cast<u8>(fl | 0x80));
            L.entry(e.b);
            continue;
          }
          e.put(fl);
          e.put(static_cast<u16>(((col8(c.color.x) >> 3) << 11) | ((col8(c.color.y) >> 2) << 5) | (col8(c.color.z) >> 3)));
          e.put(p5(s.position.x, o.x)); e.put(p5(s.position.y, o.y)); e.put(p1(s.position.z, o.z));
          e.put(q16(s.rotation.x)); e.put(q16(s.rotation.y)); e.put(q16(s.rotation.z)); e.put(q16(s.rotation.w));
          e.put(cms(s.velocity.x)); e.put(cms(s.velocity.y)); e.put(cms(s.velocity.z));
          e.put(ang16(c.car->steer_angle()));
          L.entry(e.b);
        }
        L.flush(true);
        ++world_sent_;
      }
      if (send_people) {
        {
          List L = list(kPeople);
          L.head();
          const auto& all = people_.all();
          for (std::size_t i = 0; i < all.size() && i < 255; ++i) {
            const People::Person& p = all[i];
            if (p.kind != People::Kind::citizen || p.state == People::State::gone || !near(p.pos, kReach)) continue;
            Wr e;
            e.put(static_cast<u8>(i));
            e.put(static_cast<u8>((static_cast<u8>(p.state) & 15) | (now < p.knocked_until ? 16 : 0)));
            e.put(p.seed);
            e.put(p5(p.pos.x, o.x)); e.put(p5(p.pos.y, o.y));
            e.put(ang8(p.yaw));
            e.put(static_cast<u8>(std::lround(clampf(p.v * 20, 0, 255))));
            L.entry(e.b);
          }
          L.flush(true);
        }
        // the animals: the cows (by key), the dogs and cats (by index; their rounds are a function
        // of the host's wall clock and their neighbourhood's anchor - both sent, so they walk the
        // same round here at the same moment) and every pigeon of every flock near him
        if (glife_) {
          const Life& life = glife_->life;
          auto quat = [](Wr& e, const Quat& q) { e.put(q16(q.x)); e.put(q16(q.y)); e.put(q16(q.z)); e.put(q16(q.w)); };
          {
            List L = list(kCows);
            L.head();
            for (const Life::Cow& c : life.cows()) {
              if (!near(c.pos, kAnimalReach)) continue;
              Wr e;
              e.put(c.key);
              e.put(p5(c.pos.x, o.x)); e.put(p5(c.pos.y, o.y)); e.put(p1(c.pos.z, o.z));
              e.put(ang8(c.yaw));
              const u8 fl = static_cast<u8>((c.dead ? 1 : 0) | (c.walking ? 2 : 0) | (c.carried ? 4 : 0));
              e.put(fl);
              if (fl & 5) quat(e, c.rot);
              L.entry(e.b);
            }
            L.flush(true);
          }
          {
            List L = list(kPets);
            const auto& pets = life.pets();
            {
              Wr x;
              x.put(life_epoch());
              x.put(static_cast<u8>(std::min<std::size_t>(pets.size(), 255)));
              const u8 na = static_cast<u8>(std::min<std::size_t>(life.pet_anchors().size(), 8));
              x.put(na);
              for (u8 k = 0; k < na; ++k) { x.put(life.pet_anchors()[k].x); x.put(life.pet_anchors()[k].y); }
              L.extra = x.b;
            }
            L.head();
            for (std::size_t i = 0; i < pets.size() && i < 255; ++i) {
              const Life::Pet& p = pets[i];
              if ((!p.shown && p.dead_at < 0) || !near(p.pos, kAnimalReach)) continue;  // (in a house / the water, or not near him)
              Wr e;
              e.put(static_cast<u8>(i));
              e.put(p5(p.pos.x, o.x)); e.put(p5(p.pos.y, o.y)); e.put(p1(p.pos.z, o.z));
              e.put(ang8(p.yaw));
              e.put(p5(p.off.x, 0)); e.put(p5(p.off.y, 0));
              const u8 fl = static_cast<u8>((p.shown ? 1 : 0) | (p.dead_at >= 0 ? 2 : 0) | (p.carried ? 4 : 0));
              e.put(fl);
              if (fl & 6) quat(e, p.rot);
              L.entry(e.b);
            }
            L.flush(true);
          }
          {
            List L = list(kFlocks);
            L.head();
            for (const Life::Flock& f : life.flocks()) {
              if (!near({f.at.x, f.at.y, 0}, kAnimalReach + 60)) continue;
              Wr e;
              e.put(f.id);
              e.put(static_cast<u8>(f.air ? 1 : 0));
              e.put(p5(f.at.x, o.x)); e.put(p5(f.at.y, o.y));
              const u8 nb = static_cast<u8>(std::min<std::size_t>(f.birds.size(), 100));
              e.put(nb);
              for (u8 k = 0; k < nb; ++k) {
                const Life::Pigeon& b = f.birds[k];
                e.put(static_cast<u8>(b.dead_at >= 0 ? 1 : 0));
                e.put(p5(b.pos.x, o.x)); e.put(p5(b.pos.y, o.y)); e.put(p1(b.pos.z, o.z));
                e.put(ang8(b.yaw));
              }
              L.entry(e.b);
            }
            L.flush(true);
          }
          // the running club (jogger_chain.mjs): its id and its runners, when it runs near him
          {
            const Life::Chain* ch = life.chain();
            bool close = false;
            if (ch) for (const Life::Runner& rn : ch->runners) if (rn.alive && near(rn.pos, kReach)) close = true;
            List L = list(kJoggers);
            L.head();
            if (ch && close) {
              Wr e;
              e.put(ch->id);
              for (const Life::Runner& rn : ch->runners) {
                e.put<u8>(rn.alive ? 1 : 0);
                e.put(p5(rn.pos.x, o.x)); e.put(p5(rn.pos.y, o.y));
                e.put(ang8(rn.yaw));
              }
              L.entry(e.b);
            }
            L.flush(true);
          }
        }
      }
    }
  }
  if (!net_.hosting()) city_.extra_foci.clear();
  // ---- client: the host's town around us ----
  const bool puppet = world_puppet();
  if (puppet != puppet_world_) {
    puppet_world_ = puppet;
    people_.set_puppets(puppet);
    if (glife_) glife_->life.set_remote_joggers(puppet);
    if (glife_) glife_->life.set_remote_animals(puppet);
    car_net_.assign(cars_.size(), -1);
    car_line_.assign(cars_.size(), {});
    world_parts_.clear();
    host_epoch_ok_ = false;
    world_clock_ok_ = false;
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      TrafficCar& c = cars_[i];
      if (c.police || static_cast<i32>(i) == car_index_) continue;
      if (puppet && c.placed) { world_->set_alive(c.actor, false); c.placed = false; }  // ours go: the host's come
    }
  }
  // (every frame: the street life may only come to be after the session began)
  if (glife_) { glife_->life.set_remote_joggers(puppet); glife_->life.set_remote_animals(puppet); }
  std::vector<std::vector<u8>> blobs = net_.take_world();
  world_got_ += static_cast<u32>(blobs.size());
  if (!blobs.empty()) world_heard_at_ = now;
  if (!puppet) return;
  if (car_net_.size() < cars_.size()) car_net_.resize(cars_.size(), -1);
  if (car_line_.size() < cars_.size()) car_line_.resize(cars_.size());
  if (car_rest_q_.size() < cars_.size()) car_rest_q_.resize(cars_.size());
  const f64 now_ms = kern::now_ms();  // the real clock on both sides: the same pace everywhere
  for (const std::vector<u8>& blob : blobs) {
    Rd r{blob};
    const u8 type = r.get<u8>(), count = r.get<u8>(), part = r.get<u8>();
    const bool last = count & 0x80;
    const u8 n = count & 0x7f;
    const f32 ox = r.get<f32>(), oy = r.get<f32>(), oz = r.get<f32>();
    const u32 t_ms = r.get<u32>();
    if (!r.ok) continue;
    // the host's clock on ours: the smallest (now - his time) seen, rising slowly - every snapshot is
    // then placed at the moment it was taken, so the motion between snapshots is his, not a stepping
    {
      const f64 lag = now_ms - static_cast<f64>(t_ms);
      if (!world_clock_ok_ || lag < world_clock_) { world_clock_ = lag; world_clock_ok_ = true; }
      else world_clock_ += std::min(1.0, (lag - world_clock_) * 0.02);
    }
    // when the list was taken, on our game clock: now, less its age in real time
    const f64 age_ms = std::max(0.0, now_ms - (static_cast<f64>(t_ms) + world_clock_));
    const f32 snap_at = now - static_cast<f32>(age_ms / 1000);
    // the list this part belongs to: whole when every part up to the last one came
    WorldParts& P = world_parts_[type];
    if (P.t_ms != t_ms) {
      if (static_cast<i32>(t_ms - P.t_ms) < 0 && P.t_ms != 0) continue;  // an older list, late: the newer one stands
      P = WorldParts{};
      P.t_ms = t_ms;
      if (type == kCars) car_seen_.assign(cars_.size(), false);
      if (type == kPeople) people_acc_.clear();
      if (type == kCows) cows_acc_.clear();
      if (type == kPets) pets_acc_.clear();
      if (type == kFlocks) flocks_acc_.clear();
    }
    if (part < 64) P.mask |= 1ull << part;
    const bool whole = last && part < 64 && P.mask == (part == 63 ? ~0ull : (1ull << (part + 1)) - 1);
    auto un8 = [](u8 a) { return static_cast<f32>(static_cast<i8>(a)) / 128 * kPi; };
    auto x5 = [&](f32 base) { return base + r.get<i16>() / 20.0f; };
    auto x1 = [&](f32 base) { return base + r.get<i16>() / 100.0f; };
    auto quat = [&]() { Quat q; q.x = uq16(r.get<i16>()); q.y = uq16(r.get<i16>()); q.z = uq16(r.get<i16>()); q.w = uq16(r.get<i16>()); const f32 l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w); if (l > 1e-6f) { q.x /= l; q.y /= l; q.z /= l; q.w /= l; } else q = Quat{}; return q; };
    if (type == kCars) {
      std::vector<bool>& seen = car_seen_;
      if (seen.size() != cars_.size()) seen.assign(cars_.size(), false);
      for (u8 k = 0; k < n && r.ok; ++k) {
        const u8 idx = r.get<u8>(), flags = r.get<u8>();
        if (flags & 0x80) {
          // at rest: named only - ours of that name stays as it is (one we do not have yet waits
          // for its whole entry)
          for (std::size_t i = 0; i < cars_.size(); ++i)
            if (car_net_[i] == idx) {
              seen[i] = true;
              if (i < car_line_.size() && !car_line_[i].empty()) car_line_[i].push(car_line_[i].last_position(), car_rest_q_[i], {}, true, static_cast<f64>(t_ms), now_ms);
              break;
            }
          continue;
        }
        const u16 c565 = r.get<u16>();
        const Vec3 col{static_cast<f32>((c565 >> 11) & 31) / 31, static_cast<f32>((c565 >> 5) & 63) / 63, static_cast<f32>(c565 & 31) / 31};
        Vec3 p;
        p.x = x5(ox); p.y = x5(oy); p.z = x1(oz);
        const Quat q = quat();
        Vec3 v;
        v.x = r.get<i16>() / 100.0f; v.y = r.get<i16>() / 100.0f; v.z = r.get<i16>() / 100.0f;
        const f32 steer = static_cast<f32>(r.get<i16>()) / 32767 * kPi;
        if (!r.ok) break;
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
        if (car_net_[static_cast<std::size_t>(local)] != idx || !c.placed) {
          car_net_[static_cast<std::size_t>(local)] = idx;
          car_line_[static_cast<std::size_t>(local)].reset();
          if (!world_->actor_alive(c.actor)) { world_->set_alive(c.actor, true); c.solid_masks.clear(); }
          traffic_solid(c);  // (it had no physics far away here: his car is a body that pushes)
          // his car here: a kinematic body on his poses (it pushes what it meets; what happens
          // to it happens on his machine and comes back from there)
          kern::physics::World& w = world_->physics();
          const kern::physics::Body b = body(c.actor);
          c.car->detach();
          c.car->reset();
          if (b.valid() && w.body_valid(b)) {
            w.set_body_type(b, kern::physics::BodyType::kinematic);
            w.set_pose(b, {p, q});
            w.set_linear_velocity(b, {});
            w.set_angular_velocity(b, {});
          }
          c.placed = true;
        }
        c.driver = false;
        c.color = col;
        c.wreck = flags & 2;
        c.input.pedal = (flags & 4) ? -0.5f : 0;
        c.car->show(steer);
        car_line_[static_cast<std::size_t>(local)].push(p, q, v, true, static_cast<f64>(t_ms), now_ms);
        car_rest_q_[static_cast<std::size_t>(local)] = q;
      }
      // the whole list: the host's cars it does not name are gone here too
      if (whole) {
        for (std::size_t i = 0; i < cars_.size(); ++i) {
          if (seen[i] || car_net_[i] < 0 || static_cast<i32>(i) == car_index_) continue;
          car_net_[i] = -1;
          if (cars_[i].placed) { world_->set_alive(cars_[i].actor, false); cars_[i].placed = false; }
        }
      }
    } else if (type == kCows) {
      for (u8 k = 0; k < n && r.ok; ++k) {
        Life::NetCow c;
        c.key = r.get<u64>();
        c.pos.x = x5(ox); c.pos.y = x5(oy); c.pos.z = x1(oz);
        c.yaw = un8(r.get<u8>());
        const u8 fl = r.get<u8>();
        c.dead = fl & 1; c.walking = fl & 2; c.carried = fl & 4;
        c.rot = (fl & 5) ? quat() : Quat::axis_angle({0, 0, 1}, c.yaw);
        if (r.ok) cows_acc_.push_back(c);
      }
      if (whole && glife_) glife_->life.apply_cows(cows_acc_, now, snap_at);
    } else if (type == kPets) {
      const f64 epoch = r.get<f64>();
      const u8 pet_count = r.get<u8>(), na = r.get<u8>();
      std::vector<Vec2> anchors(std::min<u8>(na, 8));
      for (Vec2& a : anchors) { a.x = r.get<f32>(); a.y = r.get<f32>(); }
      if (!r.ok) continue;
      // the host's wall clock, counted on with ours
      {
        const f64 off = epoch - (static_cast<f64>(t_ms) + world_clock_) / 1000;  // his wall clock minus our real clock
        if (!host_epoch_ok_ || std::fabs(off - host_epoch_off_) > 0.5) host_epoch_off_ = off;
        else host_epoch_off_ += (off - host_epoch_off_) * 0.05;
        host_epoch_ok_ = true;
      }
      for (u8 k = 0; k < n && r.ok; ++k) {
        Life::NetPet p;
        p.idx = r.get<u8>();
        p.pos.x = x5(ox); p.pos.y = x5(oy); p.pos.z = x1(oz);
        p.yaw = un8(r.get<u8>());
        p.off.x = r.get<i16>() / 20.0f; p.off.y = r.get<i16>() / 20.0f;
        const u8 fl = r.get<u8>();
        p.shown = fl & 1; p.dead = fl & 2; p.carried = fl & 4;
        p.rot = (fl & 6) ? quat() : Quat::axis_angle({0, 0, 1}, p.yaw);
        if (r.ok) pets_acc_.push_back(p);
      }
      if (whole && glife_) glife_->life.apply_pets(pets_acc_, pet_count, anchors, now);
    } else if (type == kFlocks) {
      for (u8 k = 0; k < n && r.ok; ++k) {
        Life::NetFlock f;
        f.id = r.get<u32>();
        f.air = r.get<u8>() != 0;
        f.at.x = x5(ox); f.at.y = x5(oy);
        const u8 nb = r.get<u8>();
        for (u8 j = 0; j < nb && r.ok; ++j) {
          f.dead.push_back(r.get<u8>());
          Vec3 b;
          b.x = x5(ox); b.y = x5(oy); b.z = x1(oz);
          f.birds.push_back(b);
          f.yaws.push_back(un8(r.get<u8>()));
        }
        if (r.ok) flocks_acc_.push_back(std::move(f));
      }
      if (whole && glife_) glife_->life.apply_flocks(flocks_acc_, now, snap_at);
    } else if (type == kJoggers) {
      if (n == 0) continue;
      const u32 id = r.get<u32>();
      Vec3 pos[10];
      f32 yaw[10];
      bool alive[10];
      for (u32 k = 0; k < 10; ++k) {
        alive[k] = r.get<u8>() != 0;
        pos[k].x = x5(ox); pos[k].y = x5(oy);
        pos[k].z = city_.ground_height(pos[k].x, pos[k].y);
        yaw[k] = un8(r.get<u8>());
      }
      if (r.ok && glife_) glife_->life.apply_joggers(id, pos, yaw, alive, 10, now, snap_at);
    } else if (type == kPeople) {
      for (u8 k = 0; k < n && r.ok; ++k) {
        People::NetPerson np;
        np.idx = r.get<u8>();
        const u8 st = r.get<u8>();
        np.state = st & 15;
        np.flags = (st & 16) ? 1 : 0;
        np.seed = r.get<u32>();
        np.pos.x = x5(ox); np.pos.y = x5(oy);
        np.pos.z = city_.ground_height(np.pos.x, np.pos.y);
        np.yaw = un8(r.get<u8>());
        np.v = r.get<u8>() / 20.0f;
        np.at = snap_at;
        if (r.ok) people_acc_.push_back(np);
      }
      if (whole) people_.apply_snapshot(people_acc_, now);
    }
  }
}

std::string Game::worldsync_list() const {
  // {"puppet":0/1,"cars":[[x,y,colour],..],"people":[[seed,x,y],..]} - the cars near us sorted
  std::string o = "{\"puppet\":" + std::string(puppet_world_ ? "1" : "0") + ",\"refused\":" + std::to_string(net_.send_refused()) + ",\"sent\":" + std::to_string(world_sent_) + ",\"got\":" + std::to_string(world_got_) + ",\"mates\":" + std::to_string(mate_foci().size()) + ",\"cars\":[";
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
  o += "],\"pets\":[";
  first = true;
  u32 flocks = 0, pigeons = 0, pigeons_dead = 0, cows = 0;
  if (glife_) {
    const Life& L = glife_->life;
    for (std::size_t i = 0; i < L.pets().size(); ++i) {
      const Life::Pet& p = L.pets()[i];
      if (!p.shown || dist2d(p.pos, me) > 200) continue;
      std::snprintf(b, sizeof b, "%s[%zu,%.2f,%.2f,%d]", first ? "" : ",", i, p.pos.x, p.pos.y, p.dead_at >= 0 ? 1 : 0);
      o += b;
      first = false;
    }
    for (const Life::Flock& f : L.flocks()) {
      if (dist2d({f.at.x, f.at.y, 0}, me) > 250) continue;
      ++flocks;
      for (const Life::Pigeon& pg : f.birds) { ++pigeons; pigeons_dead += pg.dead_at >= 0; }
    }
    for (const Life::Cow& c : L.cows()) cows += dist2d(c.pos, me) < 250;
  }
  o += "],\"flockList\":[";
  first = true;
  if (glife_)
    for (const Life::Flock& f : glife_->life.flocks()) {
      std::snprintf(b, sizeof b, "%s[%u,%zu,%.0f,%.0f,%.0f]", first ? "" : ",", f.id, f.birds.size(), f.at.x, f.at.y, dist2d({f.at.x, f.at.y, 0}, me));
      o += b;
      first = false;
    }
  std::snprintf(b, sizeof b, "],\"me\":[%.0f,%.0f],\"flocks\":%u,\"pigeons\":%u,\"pigeonsDead\":%u,\"cows\":%u,\"epoch\":%.3f}", me.x, me.y, flocks, pigeons, pigeons_dead, cows, std::fmod(life_epoch(), 100000.0));
  return o + b;
}

bool Game::puppet_car_step(TrafficCar& c, std::size_t i, f32 h) {
  // A car of the host's town: a kinematic body on his poses - where he had it a moment ago
  // (mp::PoseTimeline: interpolated a jitter buffer behind his clock, a short extrapolation at
  // most), place, height and tilt, exactly. It pushes what it meets here; what is done to it is
  // done on his machine (he has every player's body there) and comes back with his next poses.
  if (!puppet_world_ || i >= car_net_.size() || car_net_[i] < 0 || static_cast<i32>(i) == car_index_) return false;
  const kern::physics::Body b = body(c.actor);
  if (!b.valid() || i >= car_line_.size()) return true;
  kern::physics::World& w = world_->physics();
  const mp::PoseSample s = car_line_[i].sample(kern::now_ms());
  if (!s.valid) return true;
  if (w.body_type(b) != kern::physics::BodyType::kinematic) w.set_body_type(b, kern::physics::BodyType::kinematic);
  const BodyState here = read_body(w, b);
  if (kern::length(s.p - here.position) > 8) {
    w.set_pose(b, {s.p, s.q});  // far off (a jump of his): put there
    w.set_linear_velocity(b, {});
    w.set_angular_velocity(b, {});
  } else {
    w.set_target_pose(b, {s.p, s.q}, h);
  }
  c.car->roll(kern::dot(w.linear_velocity(b), s.q.rotate({1, 0, 0})), h);
  return true;
}

}  // namespace sc
