// Multiplayer as SpawnCity plays it (vs_multiplayer.mjs, lan_vs.mjs): every player simulates his
// own world - the same streamed city from the host's map origin - and the players exchange their
// poses about 30 times a second (lan_vs.mjs SEND_MS 33): the active body (drone, car, jet,
// parachute or the person), the person's own position, and the parked drone and jet, so everybody
// sees everybody's craft where they stand.
//
// Authority (lan_vs.mjs getAuthorityId: the oldest participant) is the host. As in SpawnCity it
// judges every damage request between players (friendly-fire rule, spawn protection, already down),
// counts kills and deaths, hands joining players a spawn anchor near the largest group
// (vs_group_spawn.mjs), keeps the game rules and runs the unanimous rule votes
// (game_rules_sync.mjs). Effects (tracers, explosions, impacts), ping markers and voice frames go
// through the host to the others (voice only to those in range, proximity_voice.mjs).
// The victim's own machine applies an approved hit (Kern's soldier health, death and respawn stay
// local) and reports its death to the authority, which tells everybody (kill feed, scores).
//
// Transport: Kern's client / server connection (kern/net/client_server.h) over HostTransport; the
// page moves the datagrams (web/mp_host.mjs: BroadcastChannel, the Kern relay, or WebRTC data
// channels signalled over SpawnCity's public MQTT brokers).
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "kern/net/client_server.h"
#include "kern/net/transport.h"
#include "jet_seats.h"
#include "mp_rules.h"

namespace sc::net {

enum class PlayerMode : u8 { foot, drone, car, jet, parachute };
constexpr u8 kHost = 255;     // the host's slot
constexpr u8 kNobody = 254;   // no shooter (an accident, the police, a fall)

enum PlayerFlag : u16 {
  kDead = 1, kArmed = 2, kBus = 4, kGearDown = 8, kPoliceCar = 16,
  kDroneOut = 32,     // the drone is out (drone_* holds it unless it is the active body)
  kJetOut = 64,       // the jet is placed (jet_* holds it unless it is the active body)
  kProtected = 128,   // spawn protection
  kTalking = 256,     // the microphone is live
  kCockpit = 512,
  kJetpack = 1024,    // the jetpack burns (jetpack_audio.mjs: the others hear it in 3-D; game_equipment.cpp)
  kPassenger = 2048,  // rides in a mate's jet (jet_mode.mjs passenger): not drawn on foot
};

/// One player's state as the others see it.
struct PlayerState {
  u8 slot = 0;
  PlayerMode mode = PlayerMode::foot;
  u8 color = 0;           // car colour index
  u16 flags = 0;
  u32 t_ms = 0;           // the sender's clock (pose timeline)
  Vec3 position, velocity;  // the active body
  Quat rotation;
  Vec3 avatar;            // the person's feet (on foot, or the drone pilot)
  f32 avatar_yaw = 0;
  Vec3 drone_pos, jet_pos;  // parked craft
  Quat drone_rot, jet_rot;
  f32 yaw = 0, pitch = 0, speed = 0;
  f32 health = 100;       // 0..100
  f32 thrust = 0;         // jet 0..1
  u8 stars = 0, weapon = 0;
  f32 day = -1;           // time of day 0..1 (day_night_sync.mjs: clients follow the host's)
};

/// An effect another player sees (vs_multiplayer.mjs fx: shot, explosion, impact).
struct Fx {
  // cow_death: p where, dir impulse / 600 kg (cows.mjs sendDeath); push: p the velocity change,
  // dir.x the target's slot, a damage (player_push.mjs) - M3 life
  // lamp: street_lamps.mjs sendBreak (p foot, dir x/y + state); strike / flyover: fighter_jets.mjs
  // (p mark, a alt, dir.x offset); mil_tracer / mil_shell: military_response.mjs (p from, dir to) - sc-m3-gaps
  enum Kind : u8 { shot, explosion, impact, rocket, cow_death, push, lamp, strike, flyover, mil_tracer, mil_shell, kLast = mil_shell };
  u8 kind = shot, owner = 0;
  Vec3 p, dir;
  f32 a = 0;              // shot: speed; explosion: radius
};

/// A hit between players as the authority approved it.
struct Hit {
  u8 target = 0, by = kNobody, weapon = 0, zone = 0;
  f32 damage = 0;
  Vec3 from;
};
struct Kill { u8 victim = 0, by = kNobody, weapon = 0, zone = 0; };
struct PingEvent { u8 owner = 0; bool on = false; Vec3 p; u32 color = 0; };
struct VoiceFrame { u8 from = 0; u32 seq = 0; bool everywhere = false; std::vector<u8> data; };

/// A remote player as drawn here.
struct Remote {
  PlayerState state;
  mp::PoseTimeline body, person;
  mp::NavTimeline nav;
  Vec3 shown, person_shown;   // presented positions (timeline samples)
  Quat shown_rotation;
  f64 received_ms = -1e18;    // local time of the latest pose
  f32 phase = 0;              // gait
  f32 shown_hp = 100;
  mp::VoiceStream voice;
  f64 speaking_until = 0;
};

/// Units a player simulates himself and the others see and shoot (zombie_nights.mjs zombie-sync,
/// military_response.mjs mil-snap): his zombies, helicopters, tanks and soldiers, every 120 ms.
enum class SwarmKind : u8 { zombie = 0, heli = 1, tank = 2, soldier = 3 };
struct SwarmUnit {
  u16 id = 0;
  SwarmKind kind = SwarmKind::zombie;
  u8 flags = 0;     // zombie: attacking; heli: going down; soldier: 0 idle 1 aim 2 run 3 walk
  Vec3 p;
  f32 yaw = 0, aux = 0;  // aux: zombie speed, tank turret
};
struct Swarm { u8 owner = 0; std::vector<SwarmUnit> units; };
/// A hit on another player's unit (zombie-hit / mil-hit): the owner applies it.
struct SwarmHit { u8 from = 0, to = 0; u16 id = 0; SwarmKind kind = SwarmKind::zombie; f32 damage = 0; };

class Session {
 public:
  Session();
  ~Session();
  bool host(u64 self, f64 lon, f64 lat);
  bool join(u64 self, u64 server);
  void stop();
  bool active() const { return role_ != Role::none; }
  bool hosting() const { return role_ == Role::host; }
  bool connected() const;
  /// lan_vs.mjs getAuthorityId: a client whose host is gone (the connection dropped, or the host
  /// silent for 6 s after we were in). The session then goes on under the successor.
  bool host_lost() const;
  /// Who takes over: the lowest slot left among the players we still hear (us included) - every
  /// client computes the same.
  u8 successor() const;
  bool origin(f64& lon, f64& lat) const;
  u8 self_slot() const { return self_slot_; }

  // ---- host page ----
  void push(u64 from, const u8* bytes, u32 length) { transport_.push_incoming({from}, bytes, static_cast<int>(length)); }
  bool take(u64& to, std::vector<u8>& bytes) { kern::net::Address a; const bool ok = transport_.take_outgoing(a, bytes); to = a.id; return ok; }

  /// Per frame: networking; `me` is this player's state (sent at 30 Hz).
  void update(f64 time, f32 dt, const PlayerState& me, bool send_pose = true);

  // ---- outgoing ----
  void send_fx(Fx f);
  /// Our units (all kinds in one snapshot, at most 64); the others draw and shoot them.
  void send_swarm(const std::vector<SwarmUnit>& units);
  void send_swarm_hit(u8 to, u16 id, SwarmKind kind, f32 damage);
  /// A shot / blast of ours that reached player `target` (the authority judges it).
  void request_hit(u8 target, f32 damage, u8 weapon, u8 zone, Vec3 from);
  /// We went down (by kNobody: an accident).
  void report_death(u8 by, u8 weapon, u8 zone);
  void report_respawn();
  void send_ping(bool on, Vec3 p, u32 color);
  void send_voice(u32 seq, bool everywhere, const std::vector<u8>& mulaw);
  /// Ask for a rule change (friendly fire); the host starts the vote.
  bool propose_rule(bool friendly_fire);
  bool vote(bool approve);
  /// Jet seats (jet_seat_coordinator.mjs): ask the authority to claim / release a seat in
  /// `owner`'s jet (alone the table is local). The answer arrives with the table.
  void seat_request(bool claim, u8 owner, seats::Seat seat);
  const seats::Table& seats() const { return seats_; }
  seats::Table& seats() { return seats_; }

  // ---- incoming (since the last call) ----
  std::vector<Hit> take_hits() { return std::move(hits_); }
  std::vector<Hit> take_confirms() { return std::move(confirms_); }
  std::vector<Kill> take_kills() { return std::move(kills_); }
  std::vector<Fx> take_fx() { return std::move(fx_); }
  std::vector<Swarm> take_swarms() { return std::move(swarms_); }
  std::vector<SwarmHit> take_swarm_hits() { return std::move(swarm_hits_); }
  std::vector<PingEvent> take_pings() { return std::move(pings_); }
  std::vector<VoiceFrame> take_voice() { return std::move(voice_); }
  /// A spawn anchor from the host for this (joining) player.
  bool take_anchor(Vec3& p) { if (!anchor_ready_) return false; anchor_ready_ = false; p = anchor_; return true; }

  std::vector<Remote>& remotes() { return remotes_; }
  const std::vector<Remote>& remotes() const { return remotes_; }
  const Remote* remote(u8 slot) const;
  /// The participants (sorted slots, ourselves included) and their colours (vs_multiplayer.mjs).
  std::vector<u8> roster() const;
  u32 color_of(u8 slot) const;
  /// "P1".."P9": the player's place in the roster.
  u32 index_of(u8 slot) const;
  const mp::Scoreboard& scores() const { return scores_; }
  bool friendly_fire() const { return friendly_fire_; }
  void set_friendly_fire_initial(bool on) { if (role_ != Role::client) friendly_fire_ = on; }
  bool protected_now(u8 slot, f64 now_ms) const;
  f64 now_ms() const { return time_ * 1000; }
  /// The vote everybody sees: text, and whether our answer is missing.
  const std::string& vote_text() const { return vote_text_; }
  bool vote_pending() const { return remote_ballot_.id != 0; }
  std::string status() const;
  u32 sent_poses() const { return sent_poses_; }

 private:
  enum class Role : u8 { none, host, client };
  void on_pose(const PlayerState& s);
  void dispatch(u8 from, u16 type, const std::vector<u8>& data);  // host: from a client (or ourselves)
  void deliver(u16 type, const std::vector<u8>& data);           // a message from the host to us
  void to_client(u8 slot, int channel, u16 type, const std::vector<u8>& data);
  void to_all(int channel, u16 type, const std::vector<u8>& data, u8 except);
  void to_host(int channel, u16 type, const std::vector<u8>& data);
  void judge(u8 shooter, const Hit& h);
  void begin_ballot(bool value);
  void finish_ballot(mp::Ballot::Status s);
  void send_rules();
  void send_scores();
  void send_anchor(u8 slot);
  void send_seats();
  void judge_seat(u8 from, bool claim, u8 owner, seats::Seat seat);
  Vec3 focus_of(u8 slot) const;
  f32 hp_of(u8 slot) const;

  Role role_ = Role::none;
  kern::net::HostTransport transport_;
  std::unique_ptr<kern::net::Server> server_;
  std::unique_ptr<kern::net::Client> client_;
  kern::net::Address server_addr_;
  std::vector<Remote> remotes_;
  std::vector<Hit> hits_, confirms_;
  std::vector<Kill> kills_;
  std::vector<Fx> fx_;
  std::vector<Swarm> swarms_;
  std::vector<SwarmHit> swarm_hits_;
  std::vector<PingEvent> pings_;
  std::vector<VoiceFrame> voice_;
  std::vector<u8> pending_anchor_;  // host: clients waiting for their anchor (first pose)
  f64 lon_ = 0, lat_ = 0, time_ = 0, send_clock_ = 0, scores_clock_ = 0, rules_clock_ = 0;
  bool have_origin_ = false, connect_pending_ = false, anchor_ready_ = false, ever_in_ = false;
  f64 in_since_ = 0;
  std::vector<u8> mates_;  // the clients we last heard while the host was there (successor())
  Vec3 anchor_;
  u8 self_slot_ = kHost;
  PlayerState me_;
  mp::Scoreboard scores_;
  bool friendly_fire_ = false;  // world_options.mjs friendlyFire: def false, the first player decides
  std::vector<std::pair<u8, f64>> protected_;  // slot, until (ms)
  mp::Ballot ballot_, remote_ballot_;
  u32 ballot_serial_ = 0;
  std::string vote_text_ = "Aenderungen nur einstimmig";
  u32 sent_poses_ = 0;
  seats::Table seats_;
  f64 seats_clock_ = 0;
};

/// Byte packing of the pose (also used by the tests).
std::vector<u8> pack(const PlayerState& s);
bool unpack(const std::vector<u8>& b, std::size_t& at, PlayerState& s);

}  // namespace sc::net
