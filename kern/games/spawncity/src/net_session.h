// Multiplayer as SpawnCity plays it (vs_multiplayer.mjs, lan_vs.mjs): every player simulates his
// own world - the same streamed city from the host's map origin - and the players exchange their
// poses (mode, position, rotation, velocity, view, health, stars) about 15 times a second; hits on
// another player are sent to him and he applies them (the owner decides).
//
// Transport: Kern's client / server connection (kern/net/client_server.h) over HostTransport;
// the page moves the datagrams with Kern's KernNet (kern/web/studio/game.mjs): BroadcastChannel
// between tabs of one browser, or the Kern relay (tools/net_relay.mjs) over WebSocket. The host
// is the server and relays every pose to every client.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "kern/net/client_server.h"
#include "kern/net/transport.h"

namespace sc::net {

enum class PlayerMode : u8 { foot, drone, car, jet, parachute };

/// One player's state as the others see it.
struct PlayerState {
  u8 slot = 0;            // 255: the host
  PlayerMode mode = PlayerMode::foot;
  u8 color = 0, flags = 0;  // flags: 1 dead, 2 drone armed, 4 bus, 8 jet gear down, 16 police car
  Vec3 position, velocity;
  Quat rotation;
  Vec3 pilot;             // drone mode: where the pilot stands (feet)
  f32 yaw = 0, pitch = 0, speed = 0;
  f32 health = 100;
  u8 stars = 0, weapon = 0;
  f32 day = -1;           // time of day 0..1 (day_night_sync.mjs: clients follow the host's)
};

struct Hit {
  u8 target = 0;          // slot of the player hit (255: the host)
  f32 damage = 0;
  Vec3 from;
};

/// A remote player as drawn here: the latest state, smoothed.
struct Remote {
  PlayerState state;
  Vec3 shown;             // presented position (eases towards the extrapolated pose)
  Quat shown_rotation;
  f64 received = 0;       // local time of the latest pose
  f32 phase = 0;          // gait
  bool fresh = false;
};

class Session {
 public:
  Session();
  ~Session();
  /// Host or join (the page has opened its transport: `self` is this peer's address,
  /// `server` the host's (join)).
  bool host(u64 self, f64 lon, f64 lat);
  bool join(u64 self, u64 server);
  void stop();
  bool active() const { return role_ != Role::none; }
  bool hosting() const { return role_ == Role::host; }
  bool connected() const;
  /// The map origin of the session (a client gets it from the host's welcome).
  bool origin(f64& lon, f64& lat) const;
  u8 self_slot() const { return self_slot_; }

  // ---- host page ----
  void push(u64 from, const u8* bytes, u32 length) { transport_.push_incoming({from}, bytes, static_cast<int>(length)); }
  bool take(u64& to, std::vector<u8>& bytes) { kern::net::Address a; const bool ok = transport_.take_outgoing(a, bytes); to = a.id; return ok; }

  /// Per frame: networking; `me` is this player's state (sent at 15 Hz).
  void update(f64 time, f32 dt, const PlayerState& me);
  /// A hit on a remote player (sent to his machine).
  void send_hit(const Hit& h);
  /// Hits on this player since the last call.
  std::vector<Hit> take_hits() { return std::move(hits_); }
  std::vector<Remote>& remotes() { return remotes_; }
  std::string status() const;

 private:
  enum class Role : u8 { none, host, client };
  void on_pose(const PlayerState& s, f64 time);
  void route_hit(const Hit& h);
  Role role_ = Role::none;
  kern::net::HostTransport transport_;
  std::unique_ptr<kern::net::Server> server_;
  std::unique_ptr<kern::net::Client> client_;
  kern::net::Address server_addr_;
  std::vector<Remote> remotes_;
  std::vector<Hit> hits_;
  f64 lon_ = 0, lat_ = 0, send_clock_ = 0;
  bool have_origin_ = false, connect_pending_ = false;
  u8 self_slot_ = 255;
  PlayerState me_;
};

/// Byte packing of the messages (also used by the tests).
std::vector<u8> pack(const PlayerState& s);
bool unpack(const std::vector<u8>& b, std::size_t& at, PlayerState& s);

}  // namespace sc::net
