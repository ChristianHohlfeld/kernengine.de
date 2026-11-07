// SpawnCity on Kern - rounds: short games with a goal in the real city, for a session or alone.
//
// The sandbox stays the sandbox; a round is played on top of it when somebody asks for one:
//   vote (15 s, every player picks a mode) -> countdown (5 s) -> the round (3-4 min) -> results
//   (8 s, points, the winner) -> back to the sandbox.
// The modes:
//   1 Räuber & Polizei - one is the robber (by turns), the others hunt him; he wins by getting
//     away until the time is up, the cop who brings him down wins it for them
//   2 Drohnenrennen    - eight gates through the streets round the start, in order, by drone;
//     the order at the finish gives the points (alone: against the clock, your best time)
//   3 Zombienacht      - night falls, the zombies come; every zombie down is a point, who is
//     still standing at dawn gets three more
//   4 King of the Hill - a ring on a square nearby; every second one player stands in it alone
//     is his point (two in it: nobody scores)
//   5 Verstecken       - one seeks (by turns), the others have 30 s to hide; the seeker finds
//     one he sees within 15 m for 1.5 s; found ones count for him, hidden ones at the end for
//     themselves
// The host is the authority (the state, the roles, the scoring from where everybody is in his
// world); a client sends only his vote and what only his machine knows (a gate passed, a zombie
// down). The state goes to everybody on every change and once a second (reliable). Alone the
// own machine is its own host.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include "game.h"

namespace sc {

namespace {
using i16 = std::int16_t;
enum Phase : u8 { kFree = 0, kVote = 1, kCountdown = 2, kRunning = 3, kResults = 4 };
enum RMode : u8 { kNone = 0, kCops = 1, kRace = 2, kZombies = 3, kHill = 4, kHide = 5, kModes = 6 };
enum Role : u8 { kNoRole = 0, kRobber = 1, kCop = 2, kSeeker = 3, kHider = 4, kRacer = 5, kSurvivor = 6, kKing = 7 };
enum Req : u8 { kAskVote = 0, kCastVote = 1, kGatePassed = 2, kZombieDown = 3 };
constexpr f32 kVoteS = 15, kCountdownS = 5, kResultsS = 8, kHideS = 30;
constexpr f32 kDuration[kModes] = {0, 240, 240, 180, 180, 210};
constexpr u8 kGates = 8;
const char* kModeName[kModes] = {"", "RÄUBER & POLIZEI", "DROHNENRENNEN", "ZOMBIENACHT", "KING OF THE HILL", "VERSTECKEN"};
const char* kRoleName[] = {"", "RÄUBER", "POLIZEI", "SUCHER", "VERSTECKT", "RENNFAHRER", "ÜBERLEBENDER", "KÖNIG"};

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
f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }
std::string esc(const std::string& s) {
  std::string o;
  for (char c : s) { if (c == '"' || c == '\\') o += '\\'; o += c; }
  return o;
}
}  // namespace

struct RoundState {
  struct Player { u8 slot = 0, role = 0, vote = 0, progress = 0; i16 score = 0; bool out = false; f32 seen = 0, finish = -1; };
  // replicated
  u32 id = 0;
  u8 phase = kFree, mode = kNone, sub = 0;  // sub: Verstecken 0 hiding / 1 seeking
  f32 left = 0;                             // seconds left in this phase
  std::vector<Player> players;
  std::vector<Vec3> gates;
  Vec3 hill;
  f32 hill_r = 9;
  std::string result;
  // the host's own
  f32 send_clock = 0, tick = 0;
  std::vector<std::pair<u8, u8>> turns;  // slot -> times robber / seeker so far
  bool forced_night = false;
  f32 night_was = 0;
  bool day_was = false, zombies_was = false;
  // this machine's
  u8 my_gate = 0;
  f32 started_at = 0;
  f32 best_race = -1;   // alone: the best time this session (the page keeps it longer)
  u32 last_id = 0, last_seen_running = 0;
};

RoundState& Game::rounds() {
  if (!rounds_) rounds_ = std::make_shared<RoundState>();
  return *rounds_;
}

namespace {
RoundState::Player* find(RoundState& R, u8 slot) {
  for (RoundState::Player& p : R.players) if (p.slot == slot) return &p;
  return nullptr;
}
}  // namespace

// where a player is in the host's world (himself, or a mate as he is drawn)
bool Game::round_player_pos(u8 slot, Vec3& at, bool& on_foot, bool& alive) const {
  if (slot == net_.self_slot()) {
    at = mode_ == Mode::foot ? matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f} : focus_position();
    on_foot = mode_ == Mode::foot;
    alive = !player_dead_;
    return true;
  }
  const net::Remote* r = net_.remote(slot);
  if (!r) return false;
  on_foot = r->state.mode == net::PlayerMode::foot;
  at = on_foot ? r->person_shown : r->shown;
  alive = !(r->state.flags & net::kDead);
  return true;
}

void Game::rounds_send() {
  RoundState& R = rounds();
  if (!net_.active() || !net_.hosting()) return;
  Out o;
  o.put(static_cast<u8>(1)).put(R.id).put(R.phase).put(R.mode).put(R.sub).put(R.left);
  o.put(static_cast<u8>(R.players.size()));
  for (const RoundState::Player& p : R.players) o.put(p.slot).put(p.role).put(p.vote).put(p.progress).put(p.score).put(static_cast<u8>(p.out));
  o.put(static_cast<u8>(R.gates.size()));
  for (const Vec3& g : R.gates) o.put(g.x).put(g.y).put(g.z);
  o.put(R.hill.x).put(R.hill.y).put(R.hill.z).put(R.hill_r);
  const std::string res = R.result.substr(0, 200);
  o.put(static_cast<u8>(res.size()));
  for (char c : res) o.put(c);
  net_.round_to_all(o.b);
}

void Game::rounds_request(u8 what, u8 value) {
  // a client's wish / report goes to the host; the host (or alone) handles it at once
  if (net_.active() && !net_.hosting()) {
    Out o;
    o.put(static_cast<u8>(2)).put(rounds().id).put(what).put(value);
    net_.round_to_host(o.b);
    return;
  }
  rounds_handle(net_.self_slot(), what, value);
}

void Game::rounds_handle(u8 from, u8 what, u8 value) {
  RoundState& R = rounds();
  const f32 now = world_->sim_time();
  if (what == kAskVote) {
    if (R.phase != kFree) return;
    R.phase = kVote;
    R.left = kVoteS;
    R.mode = kNone;
    R.result.clear();
    R.players.clear();
    std::vector<u8> roster = net_.active() ? net_.roster() : std::vector<u8>{net_.self_slot()};
    for (u8 s : roster) { RoundState::Player p; p.slot = s; R.players.push_back(p); }
    ++R.id;
    R.send_clock = 99;
    return;
  }
  RoundState::Player* p = find(R, from);
  if (!p) return;
  if (what == kCastVote && R.phase == kVote && value > 0 && value < kModes) { p->vote = value; R.send_clock = 99; return; }
  if (R.phase != kRunning) return;
  if (what == kGatePassed && R.mode == kRace && value == p->progress && value < R.gates.size()) {
    // the gate in order: the next; the last is the finish (the order there gives the points)
    p->progress = static_cast<u8>(value + 1);
    if (p->progress >= R.gates.size()) {
      p->finish = now - R.started_at;
      u8 place = 0;
      for (const RoundState::Player& q : R.players) place += q.finish >= 0;
      p->score = static_cast<i16>(place == 1 ? 5 : place == 2 ? 3 : place == 3 ? 2 : 1);
    }
    R.send_clock = 99;
  } else if (what == kZombieDown && R.mode == kZombies) {
    p->score = static_cast<i16>(p->score + 1);
    R.send_clock = 99;
  }
  (void)now;
}

// the host: a mode's start (roles, gates, the hill, the night)
void Game::rounds_begin(u8 mode) {
  RoundState& R = rounds();
  R.mode = mode;
  R.sub = 0;
  R.gates.clear();
  for (RoundState::Player& p : R.players) { p.role = kNoRole; p.score = 0; p.progress = 0; p.out = false; p.seen = 0; p.finish = -1; }
  // by turns: the one who was robber / seeker the fewest times (then the lowest slot)
  auto pick = [&]() -> u8 {
    u8 best = R.players.empty() ? 0 : R.players[0].slot;
    u32 fewest = ~0u;
    for (const RoundState::Player& p : R.players) {
      u32 n = 0;
      for (auto& [s, c] : R.turns) if (s == p.slot) n = c;
      if (n < fewest) { fewest = n; best = p.slot; }
    }
    bool known = false;
    for (auto& [s, c] : R.turns) if (s == best) { ++c; known = true; }
    if (!known) R.turns.emplace_back(best, 1);
    return best;
  };
  Vec3 me;
  bool foot = true, alive = true;
  round_player_pos(net_.self_slot(), me, foot, alive);
  switch (mode) {
    case kCops: {
      const u8 robber = pick();
      for (RoundState::Player& p : R.players) p.role = p.slot == robber ? kRobber : kCop;
      break;
    }
    case kRace: {
      // eight gates along the streets, each 60-140 m on from the last, 6-18 m up
      for (RoundState::Player& p : R.players) p.role = kRacer;
      Vec3 at = me;
      for (u8 i = 0; i < kGates; ++i) {
        Vec3 pt;
        f32 heading = 0;
        bool ok = false;
        for (int tries = 0; tries < 12 && !ok; ++tries)
          if (city_.street_point(rng_, at, 140, pt, heading) && dist2d(pt, at) > 60) ok = true;
        if (!ok) pt = at + Vec3{std::cos(i * 0.8f) * 80, std::sin(i * 0.8f) * 80, 0};
        pt.z = city_.ground_height(pt.x, pt.y) + rng_.range(6, 18);
        R.gates.push_back(pt);
        at = pt;
      }
      break;
    }
    case kZombies: {
      for (RoundState::Player& p : R.players) p.role = kSurvivor;
      // night for the round, zombies on (the host's world: everybody's)
      Settings& st = ui_.settings;
      R.day_was = st.on(Opt::world_always_day);
      R.zombies_was = st.on(Opt::world_zombies);
      st.set(Opt::world_always_day, 0);
      st.set(Opt::world_zombies, 1);
      R.night_was = look_.tod;
      look_.tod = 22.5f / 24;
      R.forced_night = true;
      break;
    }
    case kHill: {
      for (RoundState::Player& p : R.players) p.role = kKing;
      Vec3 pt;
      f32 heading = 0;
      if (!city_.street_point(rng_, me, 90, pt, heading)) pt = me + Vec3{20, 0, 0};
      pt.z = city_.ground_height(pt.x, pt.y);
      R.hill = pt;
      break;
    }
    case kHide: {
      const u8 seeker = pick();
      for (RoundState::Player& p : R.players) p.role = p.slot == seeker ? kSeeker : kHider;
      break;
    }
    default: break;
  }
}

void Game::rounds_finish() {
  RoundState& R = rounds();
  // the last points of the mode, the winner
  if (R.mode == kCops) {
    for (RoundState::Player& p : R.players)
      if (p.role == kRobber && !p.out) p.score = static_cast<i16>(p.score + 3);
  } else if (R.mode == kZombies) {
    for (RoundState::Player& p : R.players) {
      Vec3 at;
      bool foot = true, alive = true;
      if (round_player_pos(p.slot, at, foot, alive) && alive) p.score = static_cast<i16>(p.score + 3);
    }
    if (R.forced_night) {
      ui_.settings.set(Opt::world_always_day, R.day_was ? 1.0f : 0.0f);
      ui_.settings.set(Opt::world_zombies, R.zombies_was ? 1.0f : 0.0f);
      look_.tod = R.night_was;
      R.forced_night = false;
    }
  } else if (R.mode == kHide) {
    for (RoundState::Player& p : R.players)
      if (p.role == kHider && !p.out) p.score = static_cast<i16>(p.score + 3);
  }
  const RoundState::Player* best = nullptr;
  bool tie = false;
  for (const RoundState::Player& p : R.players) {
    if (!best || p.score > best->score) { best = &p; tie = false; }
    else if (p.score == best->score) tie = true;
  }
  char b[160];
  if (!best || best->score <= 0) std::snprintf(b, sizeof b, "Keine Punkte");
  else if (tie) std::snprintf(b, sizeof b, "Unentschieden - %d Punkte", best->score);
  else std::snprintf(b, sizeof b, "P%u gewinnt mit %d Punkten", net_.active() ? net_.index_of(best->slot) : 1u, best->score);
  R.result = b;
  R.phase = kResults;
  R.left = kResultsS;
  R.send_clock = 99;
}

void Game::rounds_update(f32 dt) {
  RoundState& R = rounds();
  const f32 now = world_->sim_time();
  const bool host = !net_.active() || net_.hosting();
  // ---- a client: the host's state
  if (!host) {
    for (const net::Session::Blob& m : net_.take_round()) {
      In in{m.data};
      if (in.get<u8>() != 1) continue;
      RoundState S;
      S.id = in.get<u32>(); S.phase = in.get<u8>(); S.mode = in.get<u8>(); S.sub = in.get<u8>(); S.left = in.get<f32>();
      const u8 n = in.get<u8>();
      for (u8 k = 0; k < n && in.ok; ++k) {
        RoundState::Player p;
        p.slot = in.get<u8>(); p.role = in.get<u8>(); p.vote = in.get<u8>(); p.progress = in.get<u8>(); p.score = in.get<i16>(); p.out = in.get<u8>() != 0;
        S.players.push_back(p);
      }
      const u8 ng = in.get<u8>();
      for (u8 k = 0; k < ng && in.ok; ++k) { Vec3 g; g.x = in.get<f32>(); g.y = in.get<f32>(); g.z = in.get<f32>(); S.gates.push_back(g); }
      S.hill.x = in.get<f32>(); S.hill.y = in.get<f32>(); S.hill.z = in.get<f32>(); S.hill_r = in.get<f32>();
      const u8 nr = in.get<u8>();
      for (u8 k = 0; k < nr && in.ok; ++k) S.result += in.get<char>();
      if (!in.ok) continue;
      if (S.id != R.id || (S.phase == kRunning && R.phase != kRunning)) { R.my_gate = 0; R.started_at = now; }
      R.id = S.id; R.phase = S.phase; R.mode = S.mode; R.sub = S.sub; R.left = S.left;
      R.players = std::move(S.players); R.gates = std::move(S.gates); R.hill = S.hill; R.hill_r = S.hill_r; R.result = std::move(S.result);
    }
    if (R.phase != kFree) R.left = std::fmax(0.0f, R.left - dt);
    if (!net_.active()) R.phase = kFree;
  }
  if (host) {
    // requests of the clients
    for (const net::Session::Blob& m : net_.take_round()) {
      In in{m.data};
      if (in.get<u8>() != 2) continue;
      const u32 id = in.get<u32>();
      const u8 what = in.get<u8>(), value = in.get<u8>();
      if (in.ok && (what == kAskVote || what == kCastVote || id == R.id)) rounds_handle(m.from, what, value);
    }
    if (R.phase != kFree) {
      R.left -= dt;
      // somebody left: out of the list
      if (net_.active()) {
        const std::vector<u8> roster = net_.roster();
        R.players.erase(std::remove_if(R.players.begin(), R.players.end(), [&](const RoundState::Player& p) { return std::find(roster.begin(), roster.end(), p.slot) == roster.end(); }), R.players.end());
      }
      if (R.phase == kVote && R.left <= 0) {
        // the most votes (a tie: the lowest mode; nobody voted: the robber hunt with two or more,
        // the race alone)
        u32 count[kModes] = {};
        for (const RoundState::Player& p : R.players) if (p.vote) ++count[p.vote];
        u8 m = 0;
        for (u8 i = 1; i < kModes; ++i) if (count[i] > count[m]) m = i;
        if (!m) m = R.players.size() >= 2 ? kCops : kRace;
        if ((m == kCops || m == kHide) && R.players.size() < 2) m = kRace;  // (they need two)
        rounds_begin(m);
        R.phase = kCountdown;
        R.left = kCountdownS;
        R.send_clock = 99;
      } else if (R.phase == kCountdown && R.left <= 0) {
        R.phase = kRunning;
        R.left = kDuration[R.mode];
        R.started_at = now;
        R.my_gate = 0;
        if (R.mode == kHide) { R.sub = 0; R.left = kHideS; }
        R.send_clock = 99;
      } else if (R.phase == kRunning) {
        R.tick += dt;
        if (R.tick >= 0.1f) {
          const f32 step = R.tick;
          R.tick = 0;
          if (R.mode == kHill) {
            // one alone in the ring on foot: his point (a second per point)
            RoundState::Player* in_ring = nullptr;
            u32 n = 0;
            for (RoundState::Player& p : R.players) {
              Vec3 at;
              bool foot = true, alive = true;
              if (!round_player_pos(p.slot, at, foot, alive) || !foot || !alive) continue;
              if (dist2d(at, R.hill) < R.hill_r && std::fabs(at.z - R.hill.z) < 4) { in_ring = &p; ++n; }
            }
            if (n == 1) {
              in_ring->seen += step;
              while (in_ring->seen >= 1) { in_ring->seen -= 1; ++in_ring->score; R.send_clock = 99; }
            }
          } else if (R.mode == kHide && R.sub == 1) {
            // the seeker finds who he sees within 15 m for 1.5 s (a clear line from eye to chest)
            Vec3 se;
            bool sf = true, sa = true;
            RoundState::Player* seeker = nullptr;
            for (RoundState::Player& p : R.players) if (p.role == kSeeker) seeker = &p;
            if (seeker && round_player_pos(seeker->slot, se, sf, sa) && sa) {
              const Vec3 eye = se + Vec3{0, 0, sf ? 1.6f : 0.0f};
              for (RoundState::Player& p : R.players) {
                if (p.role != kHider || p.out) continue;
                Vec3 at;
                bool foot = true, alive = true;
                if (!round_player_pos(p.slot, at, foot, alive)) continue;
                const Vec3 chest = at + Vec3{0, 0, foot ? 1.1f : 0.0f};
                const Vec3 d = chest - eye;
                const f32 l = kern::length(d);
                bool sees = l < 15;
                if (sees && l > 0.5f) sees = !cast(world_->physics(), eye, d * (1 / l), l - 0.4f, {}, kCatWorld).hit;
                p.seen = sees ? p.seen + step : 0;
                if (p.seen >= 1.5f) { p.out = true; seeker->score = static_cast<i16>(seeker->score + 2); R.send_clock = 99; }
              }
              bool any = false;
              for (const RoundState::Player& p : R.players) any = any || (p.role == kHider && !p.out);
              if (!any) R.left = 0;  // all found: the round is over
            }
          } else if (R.mode == kRace) {
            bool all = true;
            for (const RoundState::Player& p : R.players) all = all && p.finish >= 0;
            if (all && !R.players.empty()) R.left = 0;
          }
        }
        if (R.left <= 0) {
          if (R.mode == kHide && R.sub == 0) { R.sub = 1; R.left = kDuration[kHide]; R.send_clock = 99; }
          else rounds_finish();
        }
      } else if (R.phase == kResults && R.left <= 0) {
        R.phase = kFree;
        R.mode = kNone;
        R.send_clock = 99;
      }
    }
    // the state to everybody: on a change, else once a second
    R.send_clock += dt;
    if (R.send_clock >= 1) { R.send_clock = 0; rounds_send(); }
  }

  // ---- this machine: a race starts - into the drone (the gates are for it)
  if (R.phase == kRunning && R.mode == kRace && R.last_seen_running != R.id) {
    R.last_seen_running = R.id;
    R.my_gate = 0;
    R.started_at = now;
    if (mode_ == Mode::foot && !gaps_riding()) toggle_drone();
  }
  // ---- this machine: the race gates in order (a drone within 6 m of the next)
  if (R.phase == kRunning && R.mode == kRace && R.my_gate < R.gates.size()) {
    const RoundState::Player* me = find(R, net_.self_slot());
    if (me && me->progress > R.my_gate) R.my_gate = me->progress;
    const Vec3 at = mode_ == Mode::drone && drone_out_ ? matrix_of(drone_actor_).translation() : Vec3{1e9f, 1e9f, 1e9f};
    if (R.my_gate < R.gates.size() && kern::length(at - R.gates[R.my_gate]) < 6) {
      osfx("fighter_jets.beep", at, R.my_gate + 1 == R.gates.size() ? 5.0f : 4.0f);
      rounds_request(kGatePassed, R.my_gate);
      ++R.my_gate;
      if (R.my_gate == R.gates.size()) {
        const f32 t = now - R.started_at;
        if (R.best_race < 0 || t < R.best_race) R.best_race = t;
        R.last_id = R.id;
      }
    }
  }
}

// a cop's kill of the robber (game_net.cpp: every kill the host hears)
void Game::rounds_on_kill(u8 victim, u8 by) {
  RoundState& R = rounds();
  if (!(!net_.active() || net_.hosting()) || R.phase != kRunning || R.mode != kCops) return;
  RoundState::Player* v = find(R, victim);
  RoundState::Player* k = find(R, by);
  if (!v || v->role != kRobber) return;
  v->out = true;
  if (k && k->role == kCop) k->score = static_cast<i16>(k->score + 3);
  for (RoundState::Player& p : R.players) if (p.role == kCop) p.score = static_cast<i16>(p.score + 1);
  rounds_finish();
}

void Game::rounds_zombie_down() {
  RoundState& R = rounds();
  if (R.phase == kRunning && R.mode == kZombies) rounds_request(kZombieDown, 0);
}

// the gates and the hill in the world
void Game::rounds_draw() {
  if (!rounds_) return;
  RoundState& R = *rounds_;
  if (R.phase != kRunning && R.phase != kCountdown) return;
  CombatArt& A = combat_art_;
  if (R.mode == kRace)
    for (std::size_t i = 0; i < R.gates.size(); ++i) {
      if (i < R.my_gate) continue;
      const Vec3 next = i + 1 < R.gates.size() ? R.gates[i + 1] : R.gates[i] + Vec3{1, 0, 0};
      const Vec3 d = next - R.gates[i];
      const Vec3 col = i == R.my_gate ? Vec3{0.43f, 0.96f, 0.66f} * 2.4f : i + 1 == R.gates.size() ? Vec3{1.0f, 0.85f, 0.3f} * 2.0f : Vec3{1, 1, 1} * 1.2f;
      A.gate(R.gates[i], std::atan2(d.y, d.x), 7, 7, col);
    }
  if (R.mode == kHill) {
    const f32 pulse = 0.85f + 0.15f * std::sin(world_->sim_time() * 3);
    const f32 s = R.hill_r / 0.8f;
    A.put(CombatArt::fade_ring, kern::Mat4::translate(R.hill + Vec3{0, 0, 0.12f}) * kern::Mat4::scale({s, s, 0.08f}), Vec3{1.0f, 0.75f, 0.2f} * 2.6f, 0.75f * pulse);
    people_art_.beacon(R.hill, {1.0f, 0.75f, 0.2f}, 0.8f);
  }
}

// the HUD: "round":{...} (the page's rounds panel: rounds.mjs)
std::string Game::rounds_json() {
  RoundState& R = rounds();
  std::string s = ",\"round\":{";
  char b[256];
  const RoundState::Player* me = find(R, net_.self_slot());
  // the next goal on screen (a gate, the hill)
  f32 tx = 0, ty = 0, td = 0;
  bool target = false;
  Vec3 goal;
  if (R.phase == kRunning && R.mode == kRace && R.my_gate < R.gates.size()) { goal = R.gates[R.my_gate]; target = true; }
  if (R.phase == kRunning && R.mode == kHill) { goal = R.hill + Vec3{0, 0, 2}; target = true; }
  bool on = false;
  if (target) {
    f32 nx, ny, depth;
    on = project_screen(goal, nx, ny, depth);
    tx = (clampf(nx, -0.92f, 0.92f) + 1) / 2 * frame_w_;
    ty = (1 - clampf(ny, -0.88f, 0.88f)) / 2 * frame_h_;
    td = kern::length(goal - cam_.eye);
  }
  std::string objective;
  if (R.phase == kRunning) {
    const u8 role = me ? me->role : 0;
    switch (R.mode) {
      case kCops: objective = role == kRobber ? "Du bist der RÄUBER - entkomme bis zum Schluss!" : "Schnapp den Räuber (rot markiert)!"; break;
      case kRace: objective = R.my_gate < R.gates.size() ? "Tor " + std::to_string(R.my_gate + 1) + " von " + std::to_string(R.gates.size()) + " - mit der Drohne hindurch" : "Im Ziel!"; break;
      case kZombies: objective = "Überlebe bis zum Morgen - jeder Zombie ein Punkt"; break;
      case kHill: objective = "Steh allein im Ring - jede Sekunde ein Punkt"; break;
      case kHide:
        objective = R.sub == 0 ? (role == kSeeker ? "Augen zu - gleich suchst du!" : "Versteck dich!") : (role == kSeeker ? "Finde sie - 15 m Sicht, kurz im Blick" : me && me->out ? "Gefunden!" : "Bleib versteckt!");
        break;
      default: break;
    }
  }
  std::snprintf(b, sizeof b, "\"id\":%u,\"phase\":%u,\"mode\":%u,\"modeName\":\"%s\",\"left\":%.1f,\"sub\":%u,\"role\":\"%s\",\"objective\":\"%s\",\"result\":\"%s\"", R.id, R.phase,
                R.mode, R.mode < kModes ? kModeName[R.mode] : "", R.left, R.sub, me && me->role < 8 ? kRoleName[me->role] : "", esc(objective).c_str(), esc(R.result).c_str());
  s += b;
  std::snprintf(b, sizeof b, ",\"target\":%s,\"tx\":%.0f,\"ty\":%.0f,\"td\":%.0f,\"tOn\":%s,\"bestRace\":%.2f,\"myTime\":%.2f,\"blind\":%s",
                target ? "true" : "false", tx, ty, td, on ? "true" : "false", R.best_race,
                R.mode == kRace && R.my_gate >= R.gates.size() && R.last_id == R.id ? R.best_race : -1.0f,
                R.phase == kRunning && R.mode == kHide && R.sub == 0 && me && me->role == kSeeker ? "true" : "false");
  s += b;
  s += ",\"players\":[";
  bool first = true;
  for (const RoundState::Player& p : R.players) {
    std::snprintf(b, sizeof b, "%s{\"slot\":%u,\"index\":%u,\"me\":%s,\"role\":\"%s\",\"score\":%d,\"vote\":%u,\"out\":%s,\"progress\":%u,\"color\":\"#%06x\"}", first ? "" : ",", p.slot,
                  net_.active() ? net_.index_of(p.slot) : 1u, p.slot == net_.self_slot() ? "true" : "false", p.role < 8 ? kRoleName[p.role] : "", p.score, p.vote, p.out ? "true" : "false",
                  p.progress, net_.active() ? net_.color_of(p.slot) : 0x29d6ffu);
    s += b;
    first = false;
  }
  return s + "]}";
}

// the robber on everybody's screen (a cop's plates mark him: game_net.cpp)
u8 Game::rounds_role_of(u8 slot) {
  if (!rounds_) return 0;
  RoundState& R = *rounds_;
  if (R.phase != kRunning) return 0;
  const RoundState::Player* p = find(R, slot);
  return p ? p->role : 0;
}

std::string Game::rounds_command(const std::string& cmd) {
  // "round start" (the vote), "round vote N", "round state"
  if (cmd == "round start") { rounds_request(kAskVote, 0); return "asked"; }
  if (cmd == "round skip" && (!net_.active() || net_.hosting())) { rounds().left = 0; return "skipped"; }  // tests: the phase to its end
  if (cmd == "round hill") { const RoundState& R = rounds(); return std::to_string(R.hill.x) + " " + std::to_string(R.hill.y); }  // tests: the ring
  if (cmd.rfind("round vote ", 0) == 0) { rounds_request(kCastVote, static_cast<u8>(std::atoi(cmd.c_str() + 11))); return "voted"; }
  std::string j = rounds_json();
  return "{" + j.substr(10);
}

}  // namespace sc
