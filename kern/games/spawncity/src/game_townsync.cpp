// SpawnCity on Kern - one town in multiplayer, the part that is not a pose: what a client does to
// the host's town, and the town's lasting state (the bytes: town_sync.h).
//
// The town is the host's (game_worldsync.cpp). His cars and passers-by are puppets on a client -
// so what a client does to them has to happen on the host's machine:
//  * a hit on a traffic or parked car (a round, a blast): damage_car on a client sends it instead
//    of applying it (the car, the damage, what did it; who did it is the connection it came on);
//    the host applies it with damage_car as if it had happened there - the car turns critical,
//    burns, goes up - and everybody sees it through his car list (wreck, burning)
//  * a hit a passer-by survives (a round, a blow, the client's car): People notes it on the client
//    (take_hurts), the host applies it (People::mate_hurt) - the person is hurt, falls, runs, for
//    everybody; a death goes its own way as before (Fx npc_death)
// And three things were only effects, so a player who joined later did not have them:
//  * broken street lamps: by their 8 m cell, only ever worse - whoever breaks one tells the host,
//    the host tells everybody (the Fx `lamp` still carries the moment: the glass, the fall)
//  * barricade planks (zombie nights): per building and door. They are part of the building, not
//    of one player's night - two players at one door must see one barricade - so they are the
//    host's like the lamps: a plank broken or nailed back is a step the host applies, the count he
//    then has goes to everybody
//  * scorch, soot and wall breaches: they stay (pools of 40 / 48 / 64, world_marks.h), so the
//    blasts that left them are remembered (the newest 64) and a joiner runs them again
// A joiner gets all of it once; the host's RESET is a new epoch (everybody starts from his state
// again), a client's RESET asks for the host's state.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "game.h"
#include "game_world.h"

namespace sc {

struct TownSync {
  u32 epoch = 1;                          // the host's (a client: the one he has; 0 none yet)
  bool was_active = false, hosting = false;
  bool marks_taken = false;               // client: the host's marks were run here (once a session)
  std::vector<u8> known;                  // host: the slots that have the whole state
  std::vector<town::LampRec> lamps_out;   // lamps that broke here (a host: or at a client's), still to tell
  std::vector<town::MarkRec> marks;       // the blasts that left marks here, the newest last
  std::vector<town::MarkRec> marks_in;    // client: the host's, to run once the world stands
  u32 car_hits[3] = {0, 0, 0}, person_hits = 0, sent_states = 0, got_states = 0, asks = 0;
};

TownSync& Game::townsync() {
  if (!townsync_) townsync_ = std::make_shared<TownSync>();
  return *townsync_;
}

bool Game::town_car_remote(i32 car) const {
  return puppet_world_ && car >= 0 && static_cast<std::size_t>(car) < car_net_.size() && car_net_[static_cast<std::size_t>(car)] >= 0 &&
         !(mode_ == Mode::car && car == car_index_);
}

void Game::townsync_car_hit(i32 car, f32 amount, town::Cause cause) {
  net_.town_to_host(town::pack(town::CarHit{static_cast<u8>(car_net_[static_cast<std::size_t>(car)]), amount, cause}));
  ++townsync().car_hits[static_cast<std::size_t>(cause)];
}

void Game::townsync_lamp(i64 key, u8 state) {
  if (net_.active()) townsync().lamps_out.push_back({key, state});
}

void Game::townsync_plank(i64 key, u8 door, u8 doors, i32 step) {
  if (!net_.active() || !gaps_ || doors < 1 || doors > town::kDoorsMax || door >= doors) return;
  if (net_.hosting()) net_.town_to_all(town::pack(town::PlankRec{key, door, doors, gaps_->planks[key][door]}));
  else net_.town_to_host(town::pack(town::PlankStep{key, door, doors, static_cast<kern::i8>(step)}));
}

void Game::town_mark(Vec3 at, f32 radius, f32 strength, bool craters) {
  if (!life_) return;
  life_->marks.blast(at, radius, strength, city_, world_->physics(), rng_, world_->sim_time(), craters);
  TownSync& t = townsync();
  t.marks.push_back({at, radius, strength});
  if (t.marks.size() > town::kMarks) t.marks.erase(t.marks.begin());
}

void Game::townsync_send_full(i32 to) {
  if (!gaps_) return;
  TownSync& t = townsync();
  std::vector<town::LampRec> lamps;
  for (const auto& [key, state] : gaps_->lamps.broken_list()) lamps.push_back({key, state});
  std::vector<town::PlankRec> planks;
  for (const auto& [key, doors] : gaps_->planks) {
    if (doors.empty() || doors.size() > town::kDoorsMax) continue;
    for (std::size_t i = 0; i < doors.size(); ++i)
      if (doors[i] < town::kPlanksMax) planks.push_back({key, static_cast<u8>(i), static_cast<u8>(doors.size()), doors[i]});
  }
  std::sort(planks.begin(), planks.end(), [](const town::PlankRec& a, const town::PlankRec& b) { return a.key != b.key ? a.key < b.key : a.door < b.door; });
  for (const std::vector<u8>& part : town::pack_state(t.epoch, lamps, planks, t.marks)) {
    if (to < 0) net_.town_to_all(part);
    else net_.town_to(static_cast<u8>(to), part);
    ++t.sent_states;
  }
}

void Game::townsync_reset() {
  // RESET (after the lamps here were put back up): the host's is everybody's - a new epoch, his
  // state from scratch; a client's is his own picture only, so he takes the host's state again
  if (!net_.active()) return;
  TownSync& t = townsync();
  t.lamps_out.clear();
  if (net_.hosting()) { ++t.epoch; townsync_send_full(-1); }
  else { net_.town_to_host(town::pack_ask()); ++t.asks; }
}

void Game::townsync_update(f32 dt) {
  (void)dt;
  TownSync& t = townsync();
  std::vector<People::Hurt> hurts;
  people_.take_hurts(hurts);
  if (!net_.active() || !gaps_) {
    if (t.was_active) { t.known.clear(); t.lamps_out.clear(); t.marks_in.clear(); }
    t.was_active = false;
    return;
  }
  const bool host = net_.hosting();
  if (!t.was_active || host != t.hosting) {
    t.known.clear();
    t.marks_in.clear();
    if (!host) { t.epoch = 0; t.marks_taken = false; t.lamps_out.clear(); }
    else if (t.epoch == 0) t.epoch = 1;  // (a client who took over: the town goes on from what he has)
  }
  t.was_active = true;
  t.hosting = host;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  auto set_planks = [&](const town::PlankRec& r) {
    std::vector<u8>& st = g.planks[r.key];
    if (st.size() != r.doors) st.assign(r.doors, town::kPlanksMax);
    st[r.door] = r.planks;
  };

  for (const net::Session::Blob& m : net_.take_town()) {
    const town::Kind kind = town::kind_of(m.data);
    if (host) {
      if (kind == town::Kind::car_hit) {
        town::CarHit h;
        if (!town::unpack(m.data, h) || h.car >= cars_.size()) continue;
        const TrafficCar& c = cars_[h.car];
        // his hit on our car, as if it had happened here (a cruiser is its owner's, a car a mate
        // drives is that mate's)
        if (c.police || !c.placed || (h.car < car_lent_.size() && car_lent_[h.car])) continue;
        damage_car(h.car, h.damage, false, h.cause);
        ++t.car_hits[static_cast<std::size_t>(h.cause)];
      } else if (kind == town::Kind::person_hit) {
        town::PersonHit h;
        if (!town::unpack(m.data, h)) continue;
        people_.mate_hurt(h.person, h.seed, h.damage, static_cast<People::HurtBy>(h.by), h.push, now);
        ++t.person_hits;
      } else if (kind == town::Kind::lamps) {
        std::vector<town::LampRec> lamps;
        if (!town::unpack_lamps(m.data, lamps)) continue;
        for (const town::LampRec& r : lamps)
          if (g.lamps.merge_broken(r.key, r.state)) { g.lamps_dirty = true; t.lamps_out.push_back(r); }  // new to the town: on to everybody
      } else if (kind == town::Kind::plank_step) {
        town::PlankStep s;
        if (!town::unpack(m.data, s)) continue;
        std::vector<u8>& st = g.planks[s.key];
        if (st.size() != s.doors) st.assign(s.doors, town::kPlanksMax);
        u8& n = st[s.door];
        if (s.step < 0 && n > 0) --n;
        else if (s.step > 0 && n < town::kPlanksMax) ++n;
        net_.town_to_all(town::pack(town::PlankRec{s.key, s.door, s.doors, n}));  // (to him too: the door as the town has it)
      } else if (kind == town::Kind::ask) {
        ++t.asks;
        townsync_send_full(m.from);
      }
      continue;
    }
    // client
    if (kind == town::Kind::lamps) {
      std::vector<town::LampRec> lamps;
      if (!town::unpack_lamps(m.data, lamps)) continue;
      for (const town::LampRec& r : lamps)
        if (g.lamps.merge_broken(r.key, r.state)) g.lamps_dirty = true;
    } else if (kind == town::Kind::planks) {
      town::PlankRec r;
      if (town::unpack(m.data, r)) set_planks(r);
    } else if (kind == town::Kind::state) {
      town::State s;
      if (!town::unpack_state(m.data, s)) continue;
      ++t.got_states;
      if (s.first) {
        // the town is the host's: ours from scratch - every lamp up, every door boarded, then his
        gaps_lamps_reset();
        g.planks.clear();
        t.lamps_out.clear();
        if (!t.marks_taken) t.marks_in.clear();
      }
      t.epoch = s.epoch;
      for (const town::LampRec& r : s.lamps) g.lamps.merge_broken(r.key, r.state);
      g.lamps_dirty = true;
      for (const town::PlankRec& r : s.planks) set_planks(r);
      if (!t.marks_taken) {
        t.marks_in.insert(t.marks_in.end(), s.marks.begin(), s.marks.end());
        if (s.last) {
          // the marks of his town instead of ours (what we blew up alone is not in this world)
          t.marks_taken = true;
          t.marks.clear();
          if (life_) life_->marks.clear(&world_->physics());
        }
      }
    }
  }

  if (host) {
    // a mate who joined gets the whole state once
    for (const net::Remote& r : net_.remotes())
      if (std::find(t.known.begin(), t.known.end(), r.state.slot) == t.known.end()) { townsync_send_full(r.state.slot); t.known.push_back(r.state.slot); }
    t.known.erase(std::remove_if(t.known.begin(), t.known.end(), [&](u8 s) { return net_.remote_index(s) < 0; }), t.known.end());
    for (const std::vector<u8>& part : town::pack_lamps(t.lamps_out)) net_.town_to_all(part);
    t.lamps_out.clear();
    return;
  }
  // client: the host's marks, once his town stands here (the walls they sit on are the loaded ones)
  if (t.marks_taken && !t.marks_in.empty() && life_) {
    for (const town::MarkRec& m : t.marks_in) town_mark(m.at, m.radius, m.strength, false);
    t.marks_in.clear();
  }
  // ours to the host - once we have his state (what broke before it came went with it)
  if (t.epoch == 0) { t.lamps_out.clear(); return; }
  for (const std::vector<u8>& part : town::pack_lamps(t.lamps_out)) net_.town_to_host(part);
  t.lamps_out.clear();
  for (const People::Hurt& h : hurts) {
    net_.town_to_host(town::pack(town::PersonHit{h.net, h.seed, h.damage, static_cast<u8>(h.by), h.push}));
    ++t.person_hits;
  }
}

std::string Game::townsync_json() {
  const TownSync& t = townsync();
  std::size_t open_doors = 0;
  if (gaps_)
    for (const auto& [key, doors] : gaps_->planks)
      for (u8 n : doors) open_doors += n < town::kPlanksMax;
  char b[256];
  std::snprintf(b, sizeof b, "{\"epoch\":%u,\"lamps\":%zu,\"doors\":%zu,\"marks\":%zu,\"carHits\":[%u,%u,%u],\"personHits\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"refused\":%u}", t.epoch,
                gaps_ ? gaps_->lamps.broken() : 0, open_doors, t.marks.size(), t.car_hits[0], t.car_hits[1], t.car_hits[2], t.person_hits, t.sent_states, t.got_states, t.asks,
                net_.send_refused());
  return b;
}

}  // namespace sc
