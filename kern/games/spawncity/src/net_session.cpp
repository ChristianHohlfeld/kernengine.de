// Multiplayer (see net_session.h).
#include "net_session.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace sc::net {

namespace {

enum : u16 {
  kPose = 1, kPoseRelay = 2, kWelcome = 4, kFxMsg = 5, kHitReq = 6, kHitMsg = 7, kConfirm = 8, kDeath = 9, kKillMsg = 10, kScores = 11,
  kRespawn = 12, kProtect = 13, kAnchor = 14, kPingMsg = 15, kVoice = 16, kRules = 17, kPropose = 18, kBallotMsg = 19, kVote = 20,
  kBallotResult = 21, kLeft = 22, kSeatReq = 23, kSeatState = 24, kSwarmMsg = 25, kSwarmHitMsg = 26, kWorldMsg = 27, kOriginMsg = 28, kDestroyMsg = 29, kTossMsg = 30, kProjMsg = 31, kTownMsg = 32,
};
constexpr f64 kSeatsEvery = 1.5;         // jet_seat_coordinator.mjs tick: the table again every 1.5 s
enum : int { kReliable = 0, kUnreliable = 1 };
constexpr f64 kSendInterval = 0.033;     // lan_vs.mjs SEND_MS
constexpr f64 kQuietS = 6;               // a player silent this long is gone
constexpr f64 kScoresEvery = 2;          // the board is re-sent now and then (late joiners)
constexpr f64 kRulesEvery = 4;           // game_rules_sync.mjs SEND_MS

kern::net::ClientServerConfig config() {
  kern::net::ClientServerConfig c;
  c.protocol_id = 0x53504157534e3032ull;  // "SPAWSN02": SpawnCity sessions only
  c.max_clients = mp::kMaxPlayers - 1;
  kern::net::ChannelConfig reliable;
  reliable.type = kern::net::ChannelType::reliable_ordered;
  kern::net::ChannelConfig unreliable;
  unreliable.type = kern::net::ChannelType::unreliable_unordered;
  unreliable.max_message_size = 1200;
  c.connection.channels = {reliable, unreliable};
  c.connection.max_packet_size = 1300;
  return c;
}

struct W {
  std::vector<u8> b;
  template <class T> W& put(const T& v) { const u8* p = reinterpret_cast<const u8*>(&v); b.insert(b.end(), p, p + sizeof(T)); return *this; }
  W& bytes(const std::vector<u8>& v) { put(static_cast<u16>(v.size())); b.insert(b.end(), v.begin(), v.end()); return *this; }
};
struct R {
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
  std::vector<u8> bytes() {
    const u16 n = get<u16>();
    if (!ok || at + n > b.size()) { ok = false; return {}; }
    std::vector<u8> v(b.begin() + static_cast<std::ptrdiff_t>(at), b.begin() + static_cast<std::ptrdiff_t>(at + n));
    at += n;
    return v;
  }
};

bool sane(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z) && std::fabs(v.x) < 1e6f && std::fabs(v.y) < 1e6f && std::fabs(v.z) < 1e5f; }

// One part of a player's swarm snapshot: owner, the snapshot's serial, the part's number (bit 7: the
// last one), its units - 18 bytes a unit (64 fit in one unreliable message of 1200): z in dm, yaw
// and aux in 1/100.
// (`n` units of the list, from `from` on)
std::vector<u8> pack_swarm(u8 owner, u8 serial, u8 part, bool last, const std::vector<SwarmUnit>& units, std::size_t from, std::size_t n) {
  W w;
  w.put(owner).put(serial).put(static_cast<u8>(part | (last ? 0x80 : 0))).put(static_cast<u8>(n));
  for (std::size_t i = 0; i < n; ++i) {
    const SwarmUnit& u = units[from + i];
    const auto q16 = [](f32 v, f32 k) { return static_cast<std::int16_t>(std::clamp(std::lround(v * k), -32767L, 32767L)); };
    w.put(u.id).put(static_cast<u8>(u.kind)).put(u.flags).put(u.p.x).put(u.p.y).put(q16(u.p.z, 10)).put(q16(u.yaw, 100)).put(q16(u.aux, 100));
  }
  return w.b;
}
bool unpack_swarm(const std::vector<u8>& b, u8& owner, u8& serial, u8& part, bool& last, std::vector<SwarmUnit>& units) {
  R r{b};
  owner = r.get<u8>();
  serial = r.get<u8>();
  const u8 pt = r.get<u8>();
  part = pt & 0x7f;
  last = pt & 0x80;
  const u8 n = r.get<u8>();
  units.clear();
  if (n > kSwarmPerMessage) return false;
  for (u8 i = 0; i < n && r.ok; ++i) {
    SwarmUnit u;
    u.id = r.get<u16>();
    const u8 k = r.get<u8>();
    u.flags = r.get<u8>();
    u.p.x = r.get<f32>();
    u.p.y = r.get<f32>();
    u.p.z = r.get<std::int16_t>() / 10.0f;
    u.yaw = r.get<std::int16_t>() / 100.0f;
    u.aux = r.get<std::int16_t>() / 100.0f;
    if (!r.ok || k > static_cast<u8>(SwarmKind::kLast) || !sane(u.p) || !std::isfinite(u.yaw) || !std::isfinite(u.aux)) return false;
    u.kind = static_cast<SwarmKind>(k);
    units.push_back(u);
  }
  return r.ok;
}
std::vector<u8> pack_swarm_hit(const SwarmHit& h) { return W().put(h.from).put(h.to).put(h.id).put(static_cast<u8>(h.kind)).put(h.damage).b; }
bool unpack_swarm_hit(const std::vector<u8>& b, SwarmHit& h) {
  R r{b};
  h.from = r.get<u8>(); h.to = r.get<u8>(); h.id = r.get<u16>();
  const u8 k = r.get<u8>();
  h.damage = r.get<f32>();
  h.kind = static_cast<SwarmKind>(k);
  return r.ok && k <= static_cast<u8>(SwarmKind::kLast) && std::isfinite(h.damage) && h.damage > 0 && h.damage < 1e4f;
}

std::vector<u8> pack_hit(const Hit& h) { return W().put(h.target).put(h.by).put(h.weapon).put(h.zone).put(h.damage).put(h.from).b; }
bool unpack_hit(const std::vector<u8>& b, Hit& h) {
  R r{b};
  h.target = r.get<u8>(); h.by = r.get<u8>(); h.weapon = r.get<u8>(); h.zone = r.get<u8>(); h.damage = r.get<f32>(); h.from = r.get<Vec3>();
  return r.ok && std::isfinite(h.damage) && h.damage > 0 && h.damage <= 100 && sane(h.from);
}
std::vector<u8> pack_fx(const Fx& f) { return W().put(f.kind).put(f.owner).put(f.p).put(f.dir).put(f.a).b; }
bool unpack_fx(const std::vector<u8>& b, Fx& f) {
  R r{b};
  f.kind = r.get<u8>(); f.owner = r.get<u8>(); f.p = r.get<Vec3>(); f.dir = r.get<Vec3>(); f.a = r.get<f32>();
  return r.ok && f.kind <= Fx::kLast && sane(f.p) && sane(f.dir) && std::isfinite(f.a);
}

}  // namespace

std::vector<u8> pack(const PlayerState& s) {
  W w;
  w.put(s.slot).put(static_cast<u8>(s.mode)).put(s.color).put(s.flags).put(s.t_ms).put(s.position).put(s.velocity).put(s.rotation).put(s.avatar).put(s.avatar_yaw);
  if (s.flags & kDroneOut) w.put(s.drone_pos).put(s.drone_rot);
  if (s.flags & kJetOut) w.put(s.jet_pos).put(s.jet_rot);
  w.put(s.yaw).put(s.pitch).put(s.speed).put(s.health).put(s.thrust).put(s.stars).put(s.weapon).put(s.day).put(s.car);
  for (kern::i8 v : s.hand) w.put(v);
  w.put(s.hands);
  return w.b;
}

bool unpack(const std::vector<u8>& b, std::size_t& at, PlayerState& s) {
  R r{b, at};
  s.slot = r.get<u8>();
  const u8 mode = r.get<u8>();
  s.color = r.get<u8>(); s.flags = r.get<u16>(); s.t_ms = r.get<u32>();
  s.position = r.get<Vec3>(); s.velocity = r.get<Vec3>(); s.rotation = r.get<Quat>(); s.avatar = r.get<Vec3>(); s.avatar_yaw = r.get<f32>();
  if (s.flags & kDroneOut) { s.drone_pos = r.get<Vec3>(); s.drone_rot = r.get<Quat>(); }
  if (s.flags & kJetOut) { s.jet_pos = r.get<Vec3>(); s.jet_rot = r.get<Quat>(); }
  s.yaw = r.get<f32>(); s.pitch = r.get<f32>(); s.speed = r.get<f32>(); s.health = r.get<f32>(); s.thrust = r.get<f32>(); s.stars = r.get<u8>(); s.weapon = r.get<u8>(); s.day = r.get<f32>(); s.car = r.get<u8>();
  for (kern::i8& v : s.hand) v = r.get<kern::i8>();
  s.hands = r.get<u8>();
  if (!r.ok || mode > static_cast<u8>(PlayerMode::parachute) || !sane(s.position) || !sane(s.velocity) || !sane(s.avatar) || !std::isfinite(s.health)) return false;
  s.mode = static_cast<PlayerMode>(mode);
  s.health = clampf(s.health, 0, 100);
  at = r.at;
  return true;
}

Session::Session() = default;
Session::~Session() = default;

bool Session::host(u64 self, f64 lon, f64 lat) {
  stop();
  transport_.set_address({self});
  server_ = std::make_unique<kern::net::Server>(transport_, config(), self ^ 0x9e3779b97f4a7c15ull);
  server_->start(mp::kMaxPlayers - 1);
  role_ = Role::host;
  self_slot_ = kHost;
  lon_ = lon;
  lat_ = lat;
  have_origin_ = true;
  scores_.clear();
  scores_.ensure(kHost);
  return true;
}

bool Session::join(u64 self, u64 server) {
  stop();
  transport_.set_address({self});
  client_ = std::make_unique<kern::net::Client>(transport_, config(), self ^ 0x2545f4914f6cdd1dull);
  server_addr_ = {server};
  connect_pending_ = true;
  role_ = Role::client;
  have_origin_ = false;
  ever_in_ = false;
  scores_.clear();
  return true;
}

void Session::stop() {
  if (client_) client_->disconnect();
  if (server_) server_->stop();
  client_.reset();
  server_.reset();
  remotes_.clear();
  swarm_parts_.clear();
  protected_.clear();
  ballot_ = remote_ballot_ = mp::Ballot{};
  role_ = Role::none;
  ever_in_ = false;
  mates_.clear();
  have_world_rules_ = false;  // a new session: the new host's switches
}

bool Session::connected() const {
  if (role_ == Role::host) return server_ && server_->running();
  return client_ && client_->connected() && have_origin_;
}

bool Session::host_lost() const {
  if (role_ != Role::client || !ever_in_) return false;
  if (!client_ || !client_->connected()) return true;
  return now_ms() - in_since_ > kQuietS * 1000 + 2000 && remote_index(kHost) < 0;
}

u8 Session::successor() const {
  // from who was there while the host still was (by now the others may have gone quiet as well)
  u8 best = self_slot_;
  for (u8 s : mates_)
    if (s != kHost && s < best) best = s;
  return best;
}

// The host moves the city (a new start place, his GPS position): every client rebases to it.
void Session::set_origin(f64 lon, f64 lat) {
  if (role_ != Role::host) return;
  lon_ = lon;
  lat_ = lat;
  have_origin_ = true;
  to_all(kReliable, kOriginMsg, W().put(lon_).put(lat_).b, kHost);
}
bool Session::origin(f64& lon, f64& lat) const {
  if (!have_origin_) return false;
  lon = lon_;
  lat = lat_;
  return true;
}

i32 Session::remote_index(u8 slot) const {
  for (std::size_t i = 0; i < remotes_.size(); ++i) if (remotes_[i].state.slot == slot) return static_cast<i32>(i);
  return -1;
}

std::vector<u8> Session::roster() const {
  std::vector<u8> r{self_slot_};
  for (const Remote& x : remotes_) r.push_back(x.state.slot);
  std::stable_sort(r.begin(), r.end());
  r.erase(std::unique(r.begin(), r.end()), r.end());
  return r;
}

u32 Session::color_of(u8 slot) const {
  const std::vector<u8> r = roster();
  std::vector<std::string> ids;
  for (u8 s : r) ids.push_back(std::to_string(s));
  const std::vector<u32> c = mp::assign_colors(ids);
  for (std::size_t i = 0; i < r.size(); ++i) if (r[i] == slot) return c[i];
  return mp::color_of(std::to_string(slot));
}

u32 Session::index_of(u8 slot) const {
  // The host is P1 (the oldest), then the clients in slot order.
  std::vector<u8> r = roster();
  std::stable_partition(r.begin(), r.end(), [](u8 s) { return s == kHost; });
  for (std::size_t i = 0; i < r.size(); ++i) if (r[i] == slot) return static_cast<u32>(i + 1);
  return 0;
}

bool Session::protected_now(u8 slot, f64 now) const {
  for (const auto& [s, until] : protected_) if (s == slot && now < until) return true;
  return false;
}

Vec3 Session::focus_of(u8 slot) const {
  if (slot == self_slot_) return me_.mode == PlayerMode::foot || me_.mode == PlayerMode::drone ? me_.avatar : me_.position;
  const i32 at = remote_index(slot);
  if (at < 0) return {};
  const Remote& r = remotes_[static_cast<std::size_t>(at)];
  return r.state.mode == PlayerMode::foot || r.state.mode == PlayerMode::drone ? r.person_shown : r.shown;
}

f32 Session::hp_of(u8 slot) const {
  if (slot == self_slot_) return me_.flags & kDead ? 0 : me_.health;
  const i32 at = remote_index(slot);
  if (at < 0) return 0;
  const Remote& r = remotes_[static_cast<std::size_t>(at)];
  return r.state.flags & kDead ? 0 : r.state.health;
}

// ---- routing ------------------------------------------------------------------------------------

// A message that was not taken (too large for its channel, the send queue full) is counted and
// said once per kind: a list that silently never arrives is a world that silently differs.
void Session::refused(u16 type, std::size_t bytes) {
  ++send_refused_;
  if (refused_said_ < 8) { ++refused_said_; std::fprintf(stderr, "[spawncity net] message type %u (%zu bytes) was not sent\n", static_cast<unsigned>(type), bytes); }
}
void Session::to_client(u8 slot, int ch, u16 type, const std::vector<u8>& data) {
  if (role_ == Role::host && server_ && slot < server_->max_clients() && server_->client_connected(slot))
    if (!server_->send(slot, ch, type, data)) refused(type, data.size());
}
void Session::to_all(int ch, u16 type, const std::vector<u8>& data, u8 except) {
  if (role_ != Role::host || !server_) return;
  for (int c = 0; c < server_->max_clients(); ++c)
    if (c != except && server_->client_connected(c))
      if (!server_->send(c, ch, type, data)) refused(type, data.size());
}
void Session::to_host(int ch, u16 type, const std::vector<u8>& data) {
  if (role_ == Role::host) dispatch(kHost, type, data);
  else if (client_ && client_->connected()) client_->send(ch, type, data);
}

void Session::send_fx(Fx f) {
  if (role_ == Role::none) return;
  f.owner = self_slot_;
  to_host(f.kind == Fx::shot ? kUnreliable : kReliable, kFxMsg, pack_fx(f));
}
bool Session::send_swarm(const std::vector<SwarmUnit>& units) {
  if (role_ == Role::none) return true;
  if (units.size() > kSwarmMaxUnits) { refused(kSwarmMsg, units.size() * 18); return false; }
  // every unit goes: 64 a message, the parts numbered, the last one marked (an empty list is one
  // empty part - "I have none" is a snapshot too)
  ++swarm_serial_;
  const std::size_t parts = std::max<std::size_t>(1, (units.size() + kSwarmPerMessage - 1) / kSwarmPerMessage);
  for (std::size_t k = 0; k < parts; ++k) {
    const std::size_t from = k * kSwarmPerMessage, n = std::min(kSwarmPerMessage, units.size() - from);
    to_host(kUnreliable, kSwarmMsg, pack_swarm(self_slot_, swarm_serial_, static_cast<u8>(k), k + 1 == parts, units, from, n));
  }
  return true;
}
// A part of somebody's snapshot came: the snapshot is handed on (take_swarms) once every part of
// it is here - a snapshot with a part lost on the way is dropped whole (the next one comes 120 ms
// later), never shown with units missing.
void Session::swarm_part(u8 owner, u8 serial, u8 part, bool last, std::vector<SwarmUnit>& units) {
  i32 at = -1;
  for (std::size_t i = 0; i < swarm_parts_.size(); ++i) if (swarm_parts_[i].owner == owner) at = static_cast<i32>(i);
  if (at < 0) {
    swarm_parts_.push_back({});
    at = static_cast<i32>(swarm_parts_.size()) - 1;
    swarm_parts_[static_cast<std::size_t>(at)].owner = owner;
    swarm_parts_[static_cast<std::size_t>(at)].serial = static_cast<u8>(serial - 1);
  }
  SwarmParts& p = swarm_parts_[static_cast<std::size_t>(at)];
  if (p.serial != serial) {
    if (static_cast<kern::i8>(serial - p.serial) < 0) return;  // a part of an older snapshot, late
    p = SwarmParts{};
    p.owner = owner;
    p.serial = serial;
  }
  const u64 mark = 1ull << (part & 63);
  if (p.have[part >> 6] & mark) return;  // (twice)
  p.have[part >> 6] |= mark;
  p.units.insert(p.units.end(), units.begin(), units.end());
  if (last) p.last = part;
  if (p.last < 0) return;
  for (int k = 0; k <= p.last; ++k) if (!(p.have[k >> 6] & (1ull << (k & 63)))) return;
  swarms_.push_back({owner, std::move(p.units)});
  p.units.clear();
  p.last = -1;  // (handed on: its parts stay marked, so a duplicate does not start it again)
}
void Session::town_to_host(const std::vector<u8>& blob) {
  if (role_ == Role::client && client_ && client_->connected() && !client_->send(kReliable, kTownMsg, blob)) refused(kTownMsg, blob.size());
}
void Session::town_to_all(const std::vector<u8>& blob) {
  if (role_ == Role::host) to_all(kReliable, kTownMsg, blob, kHost);
}
void Session::town_to(u8 slot, const std::vector<u8>& blob) {
  if (role_ == Role::host) to_client(slot, kReliable, kTownMsg, blob);
}
void Session::send_world(const std::vector<u8>& blob) {
  if (role_ == Role::host) to_all(kUnreliable, kWorldMsg, blob, kHost);
}
void Session::send_world_to(u8 slot, const std::vector<u8>& blob) {
  if (role_ == Role::host) to_client(slot, kUnreliable, kWorldMsg, blob);
}
// Destruction sync (game_destruction_sync.cpp): reliable, the host is the authority.
void Session::send_proj(const std::vector<u8>& blob) {
  if (role_ == Role::client && client_ && client_->connected()) client_->send(kUnreliable, kProjMsg, blob);
  else if (role_ == Role::host) to_all(kUnreliable, kProjMsg, W().put(self_slot_).bytes(blob).b, kHost);
}
void Session::send_toss(const std::vector<u8>& blob) {
  if (role_ == Role::client && client_ && client_->connected()) client_->send(kUnreliable, kTossMsg, blob);
  else if (role_ == Role::host) to_all(kUnreliable, kTossMsg, W().put(self_slot_).bytes(blob).b, kHost);
}
void Session::destroy_to_host(const std::vector<u8>& blob) {
  if (role_ == Role::client && client_ && client_->connected()) client_->send(kReliable, kDestroyMsg, blob);
}
void Session::destroy_to_all(const std::vector<u8>& blob, bool reliable) {
  if (role_ == Role::host) to_all(reliable ? kReliable : kUnreliable, kDestroyMsg, blob, kHost);
}
void Session::destroy_to(u8 slot, const std::vector<u8>& blob) {
  if (role_ == Role::host) to_client(slot, kReliable, kDestroyMsg, blob);
}
void Session::send_swarm_hit(u8 to, u16 id, SwarmKind kind, f32 damage) {
  if (role_ == Role::none) return;
  SwarmHit h{self_slot_, to, id, kind, damage};
  to_host(kReliable, kSwarmHitMsg, pack_swarm_hit(h));
}
void Session::request_hit(u8 target, f32 damage, u8 weapon, u8 zone, Vec3 from) {
  if (role_ == Role::none || damage < 1) return;
  Hit h{target, self_slot_, weapon, zone, clampf(std::round(damage), 1, 100), from};
  to_host(kReliable, kHitReq, pack_hit(h));
}
void Session::report_death(u8 by, u8 weapon, u8 zone) { if (role_ != Role::none) to_host(kReliable, kDeath, W().put(by).put(weapon).put(zone).b); }
void Session::report_respawn() { if (role_ != Role::none) to_host(kReliable, kRespawn, {}); }
void Session::send_ping(bool on, Vec3 p, u32 color) { if (role_ != Role::none) to_host(kReliable, kPingMsg, W().put(self_slot_).put(static_cast<u8>(on)).put(p).put(color).b); }
void Session::send_voice(u32 seq, bool all, const std::vector<u8>& d) {
  if (role_ != Role::none && !remotes_.empty()) to_host(kUnreliable, kVoice, W().put(self_slot_).put(seq).put(static_cast<u8>(all)).bytes(d).b);
}
bool Session::propose_rule(bool ff) {
  if (role_ == Role::none || roster().size() < 2 || ff == friendly_fire_) return false;
  to_host(kReliable, kPropose, W().put(static_cast<u8>(0)).put(static_cast<u8>(ff)).b);
  return true;
}
bool Session::vote(bool approve) {
  if (!remote_ballot_.id || role_ != Role::client) return false;
  to_host(kReliable, kVote, W().put(remote_ballot_.id).put(static_cast<u8>(approve)).b);
  vote_text_ = approve ? "JA gesendet" : "NEIN gesendet";
  remote_ballot_ = mp::Ballot{};
  return true;
}

// ---- authority (host) ----------------------------------------------------------------------------

void Session::judge(u8 shooter, const Hit& req) {
  const std::vector<u8> r = roster();
  const bool target_known = std::find(r.begin(), r.end(), req.target) != r.end(), shooter_known = std::find(r.begin(), r.end(), shooter) != r.end();
  f64 until = 0;
  for (const auto& [s, u] : protected_) if (s == req.target) until = u;
  const mp::HitVerdict v = mp::judge_hit(target_known, shooter_known, req.target == shooter, friendly_fire_, now_ms(), until, hp_of(req.target));
  if (v != mp::HitVerdict::applied) return;
  Hit h = req;
  h.by = shooter;
  const std::vector<u8> bytes = pack_hit(h);
  if (h.target == self_slot_) hits_.push_back(h);
  else to_client(h.target, kReliable, kHitMsg, bytes);
  if (shooter != h.target) {
    if (shooter == self_slot_) confirms_.push_back(h);
    else to_client(shooter, kReliable, kConfirm, bytes);
  }
}

void Session::send_scores() {
  W w;
  w.put(static_cast<u8>(scores_.rows().size()));
  for (const mp::Score& s : scores_.rows()) w.put(s.slot).put(s.kills).put(s.deaths);
  to_all(kReliable, kScores, w.b, kNobody);
}

void Session::send_rules() {
  W w;
  w.put(static_cast<u8>(friendly_fire_));
  if (have_world_rules_) w.put(world_rules_);
  to_all(kReliable, kRules, w.b, kNobody);
}
void Session::set_world_rules(u16 mask) {
  if (role_ == Role::client) return;
  const bool changed = !have_world_rules_ || mask != world_rules_;
  world_rules_ = mask;
  have_world_rules_ = true;
  if (changed && role_ == Role::host) send_rules();
}

void Session::send_anchor(u8 slot) {
  // The largest fresh group in the shared frame; dead or quiet players do not pull.
  std::vector<Vec2> pts;
  const Vec3 mine = focus_of(self_slot_);
  if (!(me_.flags & kDead)) pts.push_back({mine.x, mine.y});
  for (const Remote& r : remotes_)
    if (r.state.slot != slot && !(r.state.flags & kDead) && now_ms() - r.received_ms < 3500) { const Vec3 p = focus_of(r.state.slot); pts.push_back({p.x, p.y}); }
  Vec2 c{mine.x, mine.y};
  mp::group_spawn_center(pts, c);
  to_client(slot, kReliable, kAnchor, W().put(Vec3{c.x, c.y, mine.z}).b);
}

void Session::begin_ballot(bool value) {
  if (role_ != Role::host || ballot_.id || value == friendly_fire_) return;
  if (!mp::create_ballot(ballot_, ++ballot_serial_, 0, value, self_slot_, roster(), now_ms())) return;
  W w;
  w.put(ballot_.id).put(ballot_.key).put(static_cast<u8>(value)).put(static_cast<u8>(ballot_.members.size()));
  for (u8 m : ballot_.members) w.put(m);
  to_all(kReliable, kBallotMsg, w.b, kNobody);
  char t[96];
  std::snprintf(t, sizeof t, "FRIENDLY FIRE: %s - Abstimmung 1/%zu", value ? "EIN" : "AUS", ballot_.members.size());
  vote_text_ = t;
}

void Session::finish_ballot(mp::Ballot::Status s) {
  const u8 result = s == mp::Ballot::Status::approved ? 1 : s == mp::Ballot::Status::rejected ? 2 : 3;
  if (s == mp::Ballot::Status::approved) { friendly_fire_ = ballot_.value; send_rules(); }
  to_all(kReliable, kBallotResult, W().put(ballot_.id).put(result).b, kNobody);
  vote_text_ = result == 1 ? "Einstimmig angenommen" : result == 2 ? "Abgelehnt" : "Abstimmung abgelaufen / Spielerwechsel";
  ballot_ = mp::Ballot{};
}

void Session::seat_request(bool claim, u8 owner, seats::Seat seat) {
  // no session: single-player runs locally; the host judges its own requests at once
  if (role_ == Role::none) { const u8 me = self_slot_; claim ? seats_.claim(owner, me, seat) : seats_.release(owner, me, seat); return; }
  if (role_ == Role::host) { judge_seat(self_slot_, claim, owner, seat); return; }
  to_host(kReliable, kSeatReq, W().put(static_cast<u8>(claim)).put(owner).put(static_cast<u8>(seat)).b);
}

void Session::judge_seat(u8 from, bool claim, u8 owner, seats::Seat seat) {
  const seats::Result r = claim ? seats_.claim(owner, from, seat) : seats_.release(owner, from, seat);
  if (r == seats::Result::ok) send_seats();
}

void Session::send_seats() {
  if (role_ != Role::host) return;
  W w;
  w.put(seats_.revision()).put(static_cast<u8>(seats_.rows().size()));
  for (const seats::Row& r : seats_.rows()) w.put(r.owner).put(r.pilot).put(r.passenger);
  to_all(kReliable, kSeatState, w.b, kNobody);
}

void Session::dispatch(u8 from, u16 type, const std::vector<u8>& d) {
  switch (type) {
    case kDestroyMsg: { if (role_ == Role::host && destroy_.size() < 64) destroy_.push_back({from, d}); return; }
    case kTownMsg: { if (role_ == Role::host && from != self_slot_ && town_.size() < 512) town_.push_back({from, d}); return; }
    case kProjMsg: {  // a client's rounds in flight: drawn here, and on to the others
      if (role_ != Role::host) return;
      if (proj_.size() < 64) proj_.push_back({from, d});
      to_all(kUnreliable, kProjMsg, W().put(from).bytes(d).b, from);
      return;
    }
    case kTossMsg: {  // a client's thrown things: ours to drive, and on to the others
      if (role_ != Role::host) return;
      if (toss_.size() < 64) toss_.push_back({from, d});
      to_all(kUnreliable, kTossMsg, W().put(from).bytes(d).b, from);
      return;
    }
    case kSeatReq: {
      R r{d};
      const u8 claim = r.get<u8>(), owner = r.get<u8>(), seat = r.get<u8>();
      if (r.ok && seat <= 1) judge_seat(from, claim != 0, owner, static_cast<seats::Seat>(seat));
      return;
    }
    case kFxMsg: {
      Fx f;
      if (!unpack_fx(d, f)) return;
      f.owner = from;
      if (from != self_slot_) fx_.push_back(f);
      to_all(f.kind == Fx::shot ? kUnreliable : kReliable, kFxMsg, pack_fx(f), from);
      return;
    }
    case kSwarmMsg: {
      // the host passes a player's units on to everybody else (the owner is who sent them)
      u8 owner, serial, part;
      bool last;
      std::vector<SwarmUnit> units;
      if (!unpack_swarm(d, owner, serial, part, last, units)) return;
      to_all(kUnreliable, kSwarmMsg, pack_swarm(from, serial, part, last, units, 0, units.size()), from);
      if (from != self_slot_) swarm_part(from, serial, part, last, units);
      return;
    }
    case kSwarmHitMsg: {
      SwarmHit h;
      if (!unpack_swarm_hit(d, h)) return;
      h.from = from;
      if (h.to == self_slot_) swarm_hits_.push_back(h);
      else if (h.to != kHost) to_client(h.to, kReliable, kSwarmHitMsg, pack_swarm_hit(h));
      return;
    }
    case kHitReq: {
      Hit h;
      if (unpack_hit(d, h)) judge(from, h);
      return;
    }
    case kDeath: {
      R r{d};
      Kill k{from, r.get<u8>(), r.get<u8>(), r.get<u8>()};
      if (!r.ok) return;
      const std::vector<u8> ro = roster();
      if (std::find(ro.begin(), ro.end(), k.by) == ro.end()) k.by = kNobody;
      scores_.died(k.victim, k.by == kNobody ? -1 : k.by);
      kills_.push_back(k);
      to_all(kReliable, kKillMsg, W().put(k.victim).put(k.by).put(k.weapon).put(k.zone).b, kNobody);
      send_scores();
      return;
    }
    case kRespawn: {
      const f64 until = now_ms() + mp::kSpawnProtectMs;
      bool found = false;
      for (auto& [s, u] : protected_) if (s == from) { u = until; found = true; }
      if (!found) protected_.push_back({from, until});
      to_all(kReliable, kProtect, W().put(from).put(static_cast<f32>(mp::kSpawnProtectMs)).b, kNobody);
      return;
    }
    case kPingMsg: {
      R r{d};
      r.get<u8>();
      PingEvent p{from, r.get<u8>() != 0, r.get<Vec3>(), r.get<u32>()};
      if (!r.ok || !sane(p.p)) return;
      if (from != self_slot_) pings_.push_back(p);
      to_all(kReliable, kPingMsg, W().put(from).put(static_cast<u8>(p.on)).put(p.p).put(p.color).b, from);
      return;
    }
    case kVoice: {
      R r{d};
      r.get<u8>();
      VoiceFrame v{from, r.get<u32>(), r.get<u8>() != 0, r.bytes()};
      if (!r.ok || v.data.size() < 4 || v.data.size() > 800) return;
      // Only mates in range hear it (proximity_voice.mjs transmit), unless "everywhere".
      const Vec3 at = focus_of(from);
      if (from != self_slot_ && (v.everywhere || kern::length(focus_of(self_slot_) - at) <= mp::kVoiceRadius)) voice_.push_back(v);
      const std::vector<u8> bytes = W().put(from).put(v.seq).put(static_cast<u8>(v.everywhere)).bytes(v.data).b;
      for (const Remote& x : remotes_)
        if (x.state.slot != from && (v.everywhere || kern::length(focus_of(x.state.slot) - at) <= mp::kVoiceRadius)) to_client(x.state.slot, kUnreliable, kVoice, bytes);
      return;
    }
    case kPropose: {
      R r{d};
      r.get<u8>();
      const bool value = r.get<u8>() != 0;
      if (r.ok) begin_ballot(value);
      return;
    }
    case kVote: {
      R r{d};
      const u32 id = r.get<u32>();
      const bool yes = r.get<u8>() != 0;
      if (!r.ok || !ballot_.id || id != ballot_.id) return;
      const mp::Ballot::Status s = mp::cast_vote(ballot_, from, yes, roster(), now_ms());
      if (s == mp::Ballot::Status::pending) {
        char t[96];
        std::snprintf(t, sizeof t, "FRIENDLY FIRE: %zu/%zu JA", ballot_.yes.size(), ballot_.members.size());
        vote_text_ = t;
      } else {
        finish_ballot(s);
      }
      return;
    }
    default: return;
  }
}

void Session::deliver(u16 type, const std::vector<u8>& d) {
  R r{d};
  switch (type) {
    case kOriginMsg: {
      const f64 lon = r.get<f64>(), lat = r.get<f64>();
      if (r.ok && role_ == Role::client && std::isfinite(lon) && std::isfinite(lat) && std::fabs(lat) <= 85 && std::fabs(lon) <= 180) { lon_ = lon; lat_ = lat; have_origin_ = true; }
      return;
    }
    case kWelcome: {
      self_slot_ = r.get<u8>();
      lon_ = r.get<f64>();
      lat_ = r.get<f64>();
      friendly_fire_ = r.get<u8>() != 0;
      if (r.ok) have_origin_ = true;
      return;
    }
    case kPoseRelay: {
      PlayerState s;
      std::size_t at = 0;
      if (unpack(d, at, s)) on_pose(s);
      return;
    }
    case kFxMsg: { Fx f; if (unpack_fx(d, f) && f.owner != self_slot_) fx_.push_back(f); return; }
    case kSwarmMsg: {
      u8 owner, serial, part;
      bool last;
      std::vector<SwarmUnit> units;
      if (unpack_swarm(d, owner, serial, part, last, units) && owner != self_slot_) swarm_part(owner, serial, part, last, units);
      return;
    }
    case kSwarmHitMsg: { SwarmHit h; if (unpack_swarm_hit(d, h) && h.to == self_slot_) swarm_hits_.push_back(h); return; }
    case kWorldMsg: { if (world_.size() < 8) world_.push_back(d); return; }
    case kDestroyMsg: { if (role_ == Role::client && destroy_.size() < 64) destroy_.push_back({kHost, d}); return; }
    case kTownMsg: { if (role_ == Role::client && town_.size() < 512) town_.push_back({kHost, d}); return; }
    case kProjMsg: {  // (relayed: the shooter's slot first)
      if (role_ != Role::client) return;
      const u8 from = r.get<u8>();
      std::vector<u8> body = r.bytes();
      if (r.ok && from != self_slot_ && proj_.size() < 64) proj_.push_back({from, std::move(body)});
      return;
    }
    case kTossMsg: {  // (relayed: the thrower's slot first)
      if (role_ != Role::client) return;
      const u8 from = r.get<u8>();
      std::vector<u8> body = r.bytes();
      if (r.ok && from != self_slot_ && toss_.size() < 64) toss_.push_back({from, std::move(body)});
      return;
    }
    case kSeatState: {
      const u32 rev = r.get<u32>();
      const u8 n = r.get<u8>();
      std::vector<seats::Row> rows;
      for (u8 i = 0; i < n && r.ok; ++i) { seats::Row q; q.owner = r.get<u8>(); q.pilot = r.get<u8>(); q.passenger = r.get<u8>(); rows.push_back(q); }
      if (r.ok) seats_.load(rev, rows);
      return;
    }
    case kHitMsg: { Hit h; if (unpack_hit(d, h) && h.target == self_slot_) hits_.push_back(h); return; }
    case kConfirm: { Hit h; if (unpack_hit(d, h)) confirms_.push_back(h); return; }
    case kKillMsg: { Kill k{r.get<u8>(), r.get<u8>(), r.get<u8>(), r.get<u8>()}; if (r.ok) kills_.push_back(k); return; }
    case kScores: {
      const u8 n = r.get<u8>();
      std::vector<mp::Score> rows;
      for (u8 i = 0; i < n && r.ok; ++i) { mp::Score s; s.slot = r.get<u8>(); s.kills = r.get<u16>(); s.deaths = r.get<u16>(); rows.push_back(s); }
      if (r.ok) scores_.set(rows);
      return;
    }
    case kProtect: {
      const u8 slot = r.get<u8>();
      const f32 ms = r.get<f32>();
      if (!r.ok) return;
      bool found = false;
      for (auto& [s, u] : protected_) if (s == slot) { u = now_ms() + ms; found = true; }
      if (!found) protected_.push_back({slot, now_ms() + ms});
      return;
    }
    case kAnchor: { const Vec3 p = r.get<Vec3>(); if (r.ok && sane(p)) { anchor_ = p; anchor_ready_ = true; } return; }
    case kPingMsg: {
      PingEvent p{r.get<u8>(), r.get<u8>() != 0, r.get<Vec3>(), r.get<u32>()};
      if (r.ok && p.owner != self_slot_ && sane(p.p)) pings_.push_back(p);
      return;
    }
    case kVoice: {
      VoiceFrame v{r.get<u8>(), r.get<u32>(), r.get<u8>() != 0, r.bytes()};
      if (r.ok && v.from != self_slot_ && v.data.size() >= 4 && v.data.size() <= 800) voice_.push_back(v);
      return;
    }
    case kRules: {
      const u8 ff = r.get<u8>();
      if (!r.ok) return;
      friendly_fire_ = ff != 0;
      if (role_ == Role::client) { const u16 m = r.get<u16>(); if (r.ok) { world_rules_ = m; have_world_rules_ = true; } }
      return;
    }
    case kBallotMsg: {
      mp::Ballot b;
      b.id = r.get<u32>(); b.key = r.get<u8>(); b.value = r.get<u8>() != 0;
      const u8 n = r.get<u8>();
      for (u8 i = 0; i < n && r.ok; ++i) b.members.push_back(r.get<u8>());
      if (!r.ok || b.members != roster()) return;  // a different roster: the vote is not ours
      b.expires_ms = now_ms() + mp::kBallotMs;
      remote_ballot_ = b;
      vote_text_ = std::string("FRIENDLY FIRE: ") + (b.value ? "EIN" : "AUS") + "? Deine Stimme fehlt";
      return;
    }
    case kBallotResult: {
      r.get<u32>();
      const u8 res = r.get<u8>();
      remote_ballot_ = mp::Ballot{};
      vote_text_ = res == 1 ? "Angenommen" : res == 2 ? "Abgelehnt" : "Abstimmung abgebrochen";
      return;
    }
    case kLeft: {
      const u8 slot = r.get<u8>();
      remotes_.erase(std::remove_if(remotes_.begin(), remotes_.end(), [&](const Remote& x) { return x.state.slot == slot; }), remotes_.end());
      std::erase_if(swarm_parts_, [&](const SwarmParts& p) { return p.owner == slot; });
      return;
    }
    default: return;
  }
}

void Session::on_pose(const PlayerState& s) {
  if (s.slot == self_slot_) return;
  const f64 now = now_ms();
  // the remote of this slot: its place in remotes_ (the last one of the slot), a new one at the end if none
  std::size_t at = remotes_.size();
  for (std::size_t i = 0; i < remotes_.size(); ++i) if (remotes_[i].state.slot == s.slot) at = i;
  if (at == remotes_.size()) {
    remotes_.emplace_back();
    Remote& added = remotes_.back();
    added.shown = s.position;
    added.shown_rotation = s.rotation;
    added.person_shown = s.avatar;
    if (role_ == Role::host) { scores_.ensure(s.slot); send_scores(); }
  }
  Remote& r = remotes_[at];
  // A mode change or a jump (respawn, new vehicle) starts the timelines over.
  if (r.state.mode != s.mode || kern::length(r.body.last_position() - s.position) > mp::kTeleportM) r.body.reset();
  if (kern::length(r.person.last_position() - s.avatar) > mp::kTeleportM) r.person.reset();
  r.state = s;
  r.received_ms = now;
  r.body.push(s.position, s.rotation, s.velocity, true, s.t_ms, now);
  r.person.push(s.avatar, yaw_quat(s.avatar_yaw), {}, false, s.t_ms, now);
  if (s.mode == PlayerMode::car || s.mode == PlayerMode::jet) {
    const Vec3 fwd = s.rotation.rotate(s.mode == PlayerMode::jet ? Vec3{0, 1, 0} : Vec3{1, 0, 0});
    r.nav.push(s.position, std::atan2(fwd.x, fwd.y), static_cast<u8>(s.mode), now);
  } else {
    r.nav.reset();
  }
}

void Session::update(f64 time, f32 dt, const PlayerState& me, bool send_pose) {
  (void)dt;
  if (role_ == Role::none) return;
  time_ = time;
  me_ = me;
  me_.slot = self_slot_;
  me_.t_ms = static_cast<u32>(time * 1000);
  if (protected_now(self_slot_, now_ms())) me_.flags |= kProtected;
  transport_.advance_time(time);
  const bool tick = time - send_clock_ >= kSendInterval;
  if (tick) send_clock_ = time;
  if (role_ == Role::host) {
    server_->advance_time(time);
    server_->receive_packets();
    for (const auto& [index, up] : server_->take_events()) {
      const u8 slot = static_cast<u8>(index);
      if (up) {
        // Welcome: the slot, the map origin (everybody streams the same city) and the rules.
        server_->send(index, kReliable, kWelcome, W().put(slot).put(lon_).put(lat_).put(static_cast<u8>(friendly_fire_)).b);
        pending_anchor_.push_back(slot);
        scores_.ensure(slot);
        send_scores();
        if (ballot_.id) finish_ballot(mp::Ballot::Status::cancelled);  // the roster changed
      } else {
        remotes_.erase(std::remove_if(remotes_.begin(), remotes_.end(), [&](const Remote& r) { return r.state.slot == slot; }), remotes_.end());
        std::erase_if(swarm_parts_, [&](const SwarmParts& p) { return p.owner == slot; });  // (the slot's next player starts his own serials)
        scores_.remove(slot);
        if (seats_.drop(slot)) send_seats();  // a player who leaves loses his seats
        to_all(kReliable, kLeft, W().put(slot).b, kNobody);
        send_scores();
        if (ballot_.id) finish_ballot(mp::Ballot::Status::cancelled);
      }
    }
    for (int c = 0; c < server_->max_clients(); ++c) {
      if (!server_->client_connected(c)) continue;
      for (int ch : {kReliable, kUnreliable})
        while (auto m = server_->receive(c, ch)) {
          if (m->type == kPose) {
            std::size_t at = 0;
            PlayerState s;
            if (unpack(m->data, at, s)) {
              s.slot = static_cast<u8>(c);
              if (protected_now(s.slot, now_ms())) s.flags |= kProtected;
              on_pose(s);
              // Relay at once (no extra 33 ms): the others' timelines run on the sender's clock.
              to_all(kUnreliable, kPoseRelay, pack(s), static_cast<u8>(c));
            }
          } else {
            dispatch(static_cast<u8>(c), m->type, m->data);
          }
        }
    }
    // A joining player gets his anchor once his first pose is in (the group is known then).
    for (std::size_t i = 0; i < pending_anchor_.size();)
      if (remote_index(pending_anchor_[i]) >= 0) { send_anchor(pending_anchor_[i]); pending_anchor_.erase(pending_anchor_.begin() + static_cast<std::ptrdiff_t>(i)); }
      else ++i;
    if (tick && send_pose) { to_all(kUnreliable, kPoseRelay, pack(me_), kNobody); ++sent_poses_; }
    if (time - scores_clock_ > kScoresEvery) { scores_clock_ = time; send_scores(); }
    if (time - rules_clock_ > kRulesEvery) { rules_clock_ = time; send_rules(); }
    if (time - seats_clock_ > kSeatsEvery) { seats_clock_ = time; send_seats(); }
    if (ballot_.id && mp::ballot_stale(ballot_, roster(), now_ms())) finish_ballot(mp::Ballot::Status::cancelled);
    server_->send_packets();
  } else {
    // A client whose way to the host broke - it never came about in time, or the host went quiet
    // (a tab in the background, a phone that loads) - asks again, once a second, as the original's
    // peers say hello every second (mqtt_data_relay.mjs HELLO_MS): it does not stay outside for good.
    if (!connect_pending_ && client_->state() < kern::net::ClientState::disconnected && time - connect_at_ >= kReconnectS) connect_pending_ = true;
    if (connect_pending_) { client_->connect(server_addr_, time); connect_pending_ = false; connect_at_ = time; ++connects_; }
    client_->advance_time(time);
    client_->receive_packets();
    for (int ch : {kReliable, kUnreliable})
      while (auto m = client_->receive(ch)) deliver(m->type, m->data);
    if (client_->connected() && have_origin_ && !ever_in_) { ever_in_ = true; in_since_ = now_ms(); }
    if (client_->connected() && remote_index(kHost) >= 0) {
      mates_.clear();
      for (const Remote& r : remotes_) if (r.state.slot != kHost) mates_.push_back(r.state.slot);
    }
    if (client_->connected() && have_origin_ && tick && send_pose) { client_->send(kUnreliable, kPose, pack(me_)); ++sent_poses_; }
    if (remote_ballot_.id && now_ms() >= remote_ballot_.expires_ms) { remote_ballot_ = mp::Ballot{}; vote_text_ = "Abstimmung abgelaufen"; }
    client_->send_packets();
  }
  protected_.erase(std::remove_if(protected_.begin(), protected_.end(), [&](const auto& p) { return now_ms() > p.second; }), protected_.end());
  // Players who went quiet are gone.
  remotes_.erase(std::remove_if(remotes_.begin(), remotes_.end(), [&](const Remote& r) { return now_ms() - r.received_ms > kQuietS * 1000; }), remotes_.end());
}

std::string Session::status() const {
  if (role_ == Role::none) return "";
  char b[96];
  if (role_ == Role::host) std::snprintf(b, sizeof b, "Host - %d Spieler", 1 + (server_ ? server_->connected_clients() : 0));
  else std::snprintf(b, sizeof b, "%s - %zu weitere Spieler", connected() ? "Verbunden" : "Verbinde ...", remotes_.size());
  return b;
}

}  // namespace sc::net
