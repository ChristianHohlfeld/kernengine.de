// Multiplayer (see net_session.h).
#include "net_session.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace sc::net {

namespace {

enum : u16 { kPose = 1, kPoses = 2, kHit = 3, kWelcome = 4 };
enum : int { kReliable = 0, kUnreliable = 1 };
constexpr f64 kSendInterval = 1.0 / 15;  // vs_pose_sync: ~15 Hz
constexpr u8 kHost = 255;

kern::net::ClientServerConfig config() {
  kern::net::ClientServerConfig c;
  c.protocol_id = 0x53504157534e3031ull;  // "SPAWSN01": SpawnCity sessions only
  c.max_clients = 15;
  kern::net::ChannelConfig reliable;
  reliable.type = kern::net::ChannelType::reliable_ordered;
  kern::net::ChannelConfig unreliable;
  unreliable.type = kern::net::ChannelType::unreliable_unordered;
  unreliable.max_message_size = 1200;
  c.connection.channels = {reliable, unreliable};
  c.connection.max_packet_size = 1300;
  return c;
}

template <class T>
void put(std::vector<u8>& b, const T& v) {
  const u8* p = reinterpret_cast<const u8*>(&v);
  b.insert(b.end(), p, p + sizeof(T));
}
template <class T>
bool get(const std::vector<u8>& b, std::size_t& at, T& v) {
  if (at + sizeof(T) > b.size()) return false;
  std::memcpy(&v, b.data() + at, sizeof(T));
  at += sizeof(T);
  return true;
}

std::vector<u8> pack_hit(const Hit& h) {
  std::vector<u8> b;
  put(b, h.target);
  put(b, h.damage);
  put(b, h.from);
  return b;
}
bool unpack_hit(const std::vector<u8>& b, Hit& h) {
  std::size_t at = 0;
  return get(b, at, h.target) && get(b, at, h.damage) && get(b, at, h.from);
}

}  // namespace

std::vector<u8> pack(const PlayerState& s) {
  std::vector<u8> b;
  put(b, s.slot);
  put(b, static_cast<u8>(s.mode));
  put(b, s.color);
  put(b, s.flags);
  put(b, s.position);
  put(b, s.velocity);
  put(b, s.rotation);
  put(b, s.pilot);
  put(b, s.yaw);
  put(b, s.pitch);
  put(b, s.speed);
  put(b, s.health);
  put(b, s.stars);
  put(b, s.weapon);
  return b;
}

bool unpack(const std::vector<u8>& b, std::size_t& at, PlayerState& s) {
  u8 mode = 0;
  if (!(get(b, at, s.slot) && get(b, at, mode) && get(b, at, s.color) && get(b, at, s.flags) && get(b, at, s.position) && get(b, at, s.velocity) &&
        get(b, at, s.rotation) && get(b, at, s.pilot) && get(b, at, s.yaw) && get(b, at, s.pitch) && get(b, at, s.speed) && get(b, at, s.health) && get(b, at, s.stars) &&
        get(b, at, s.weapon)))
    return false;
  if (mode > static_cast<u8>(PlayerMode::parachute)) return false;
  s.mode = static_cast<PlayerMode>(mode);
  return true;
}

Session::Session() = default;
Session::~Session() = default;

bool Session::host(u64 self, f64 lon, f64 lat) {
  stop();
  transport_.set_address({self});
  server_ = std::make_unique<kern::net::Server>(transport_, config(), self ^ 0x9e3779b97f4a7c15ull);
  server_->start(15);
  role_ = Role::host;
  self_slot_ = kHost;
  lon_ = lon;
  lat_ = lat;
  have_origin_ = true;
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
  return true;
}

void Session::stop() {
  if (client_) client_->disconnect();
  if (server_) server_->stop();
  client_.reset();
  server_.reset();
  remotes_.clear();
  role_ = Role::none;
}

bool Session::connected() const {
  if (role_ == Role::host) return server_ && server_->running();
  return client_ && client_->connected() && have_origin_;
}

bool Session::origin(f64& lon, f64& lat) const {
  if (!have_origin_) return false;
  lon = lon_;
  lat = lat_;
  return true;
}

void Session::on_pose(const PlayerState& s, f64 time) {
  if (s.slot == self_slot_) return;
  for (Remote& r : remotes_)
    if (r.state.slot == s.slot) {
      r.state = s;
      r.received = time;
      r.fresh = true;
      return;
    }
  Remote r;
  r.state = s;
  r.shown = s.position;
  r.shown_rotation = s.rotation;
  r.received = time;
  r.fresh = true;
  remotes_.push_back(r);
}

void Session::route_hit(const Hit& h) {
  if (h.target == self_slot_) { hits_.push_back(h); return; }
  if (role_ == Role::host && server_) {
    if (server_->client_connected(h.target)) server_->send(h.target, kReliable, kHit, pack_hit(h));
  } else if (client_ && client_->connected()) {
    client_->send(kReliable, kHit, pack_hit(h));  // the host forwards it
  }
}

void Session::send_hit(const Hit& h) { route_hit(h); }

void Session::update(f64 time, f32 dt, const PlayerState& me) {
  (void)dt;
  if (role_ == Role::none) return;
  me_ = me;
  me_.slot = self_slot_;
  transport_.advance_time(time);
  if (role_ == Role::host) {
    server_->advance_time(time);
    server_->receive_packets();
    for (const auto& [index, up] : server_->take_events()) {
      if (up) {
        // Welcome: the slot and the map origin, so the client streams the same city.
        std::vector<u8> b;
        put(b, static_cast<u8>(index));
        put(b, lon_);
        put(b, lat_);
        server_->send(index, kReliable, kWelcome, b);
      } else {
        remotes_.erase(std::remove_if(remotes_.begin(), remotes_.end(), [&](const Remote& r) { return r.state.slot == index; }), remotes_.end());
      }
    }
    for (int c = 0; c < server_->max_clients(); ++c) {
      if (!server_->client_connected(c)) continue;
      for (int ch : {kReliable, kUnreliable})
        while (auto m = server_->receive(c, ch)) {
          if (m->type == kPose) {
            std::size_t at = 0;
            PlayerState s;
            if (unpack(m->data, at, s)) { s.slot = static_cast<u8>(c); on_pose(s, time); }
          } else if (m->type == kHit) {
            Hit h;
            if (unpack_hit(m->data, h)) route_hit(h);
          }
        }
    }
    if (time - send_clock_ >= kSendInterval) {
      send_clock_ = time;
      // Everybody's pose to everybody (the receiver skips his own).
      std::vector<u8> all;
      const std::vector<u8> mine = pack(me_);
      all.insert(all.end(), mine.begin(), mine.end());
      for (const Remote& r : remotes_)
        if (time - r.received < 3) {
          const std::vector<u8> b = pack(r.state);
          all.insert(all.end(), b.begin(), b.end());
        }
      for (int c = 0; c < server_->max_clients(); ++c)
        if (server_->client_connected(c)) server_->send(c, kUnreliable, kPoses, all);
    }
    server_->send_packets();
  } else {
    if (connect_pending_) { client_->connect(server_addr_, time); connect_pending_ = false; }
    client_->advance_time(time);
    client_->receive_packets();
    for (int ch : {kReliable, kUnreliable})
      while (auto m = client_->receive(ch)) {
        if (m->type == kWelcome) {
          std::size_t at = 0;
          u8 slot = 0;
          if (get(m->data, at, slot) && get(m->data, at, lon_) && get(m->data, at, lat_)) { self_slot_ = slot; have_origin_ = true; }
        } else if (m->type == kPoses) {
          std::size_t at = 0;
          PlayerState s;
          while (unpack(m->data, at, s)) on_pose(s, time);
        } else if (m->type == kHit) {
          Hit h;
          if (unpack_hit(m->data, h) && h.target == self_slot_) hits_.push_back(h);
        }
      }
    if (client_->connected() && have_origin_ && time - send_clock_ >= kSendInterval) {
      send_clock_ = time;
      client_->send(kUnreliable, kPose, pack(me_));
    }
    client_->send_packets();
  }
  // Players who went quiet are gone.
  remotes_.erase(std::remove_if(remotes_.begin(), remotes_.end(), [&](const Remote& r) { return time - r.received > 6; }), remotes_.end());
}

std::string Session::status() const {
  if (role_ == Role::none) return "";
  char b[96];
  if (role_ == Role::host) std::snprintf(b, sizeof b, "Host - %d Spieler", 1 + (server_ ? server_->connected_clients() : 0));
  else std::snprintf(b, sizeof b, "%s - %zu weitere Spieler", connected() ? "Verbunden" : "Verbinde ...", remotes_.size());
  return b;
}

}  // namespace sc::net
