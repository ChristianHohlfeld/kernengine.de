// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/net/client_server.h"
#include "kern/net/connection.h"
#include "kern/net/reliable.h"
#include "kern/net/serialize.h"
#include "kern/net/transport.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_net_ChannelType { using type = kern::net::ChannelType; static constexpr type values[] = {kern::net::ChannelType::reliable_ordered, kern::net::ChannelType::unreliable_unordered}; };
struct E_net_ClientState { using type = kern::net::ClientState; static constexpr type values[] = {kern::net::ClientState::denied, kern::net::ClientState::timed_out, kern::net::ClientState::error, kern::net::ClientState::disconnected, kern::net::ClientState::connecting, kern::net::ClientState::challenged, kern::net::ClientState::connected}; };
struct E_net_ConnectionError { using type = kern::net::ConnectionError; static constexpr type values[] = {kern::net::ConnectionError::none, kern::net::ConnectionError::desync, kern::net::ConnectionError::send_queue_full, kern::net::ConnectionError::message_too_large, kern::net::ConnectionError::bad_packet, kern::net::ConnectionError::out_of_memory}; };
struct S_net_Address { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::Address& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::Address& x); };
struct S_net_ChannelConfig { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::ChannelConfig& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::ChannelConfig& x); };
struct S_net_ClientServerConfig { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::ClientServerConfig& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::ClientServerConfig& x); };
struct S_net_ConnectionConfig { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::ConnectionConfig& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::ConnectionConfig& x); };
struct S_net_EndpointConfig { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::EndpointConfig& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::EndpointConfig& x); };
struct S_net_EndpointStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::EndpointStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::EndpointStats& x); };
struct S_net_Message { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::Message& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::Message& x); };
struct S_net_SimulatorSettings { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::SimulatorSettings& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::SimulatorSettings& x); };
struct S_net_Simulator_Packet { [[maybe_unused]] static Value to(EngineObjects& o, const kern::net::Simulator::Packet& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::net::Simulator::Packet& x); };
Value S_net_Address::to(EngineObjects& o, const kern::net::Address& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_net_Address::from(EngineObjects& o, const Value& v, kern::net::Address& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_net_ChannelConfig::to(EngineObjects& o, const kern::net::ChannelConfig& x) { return Value::structure({ev::Conv<ev::Enum<E_net_ChannelType>>::to(o, x.type), ev::Conv<ev::Int>::to(o, x.send_queue_size), ev::Conv<ev::Int>::to(o, x.receive_queue_size), ev::Conv<ev::Int>::to(o, x.max_messages_per_packet), ev::Conv<ev::Int>::to(o, x.packet_budget), ev::Conv<ev::Int>::to(o, x.max_message_size), ev::Conv<ev::Int>::to(o, x.block_threshold), ev::Conv<ev::Int>::to(o, x.block_fragment_size), ev::Conv<ev::Float>::to(o, x.resend_time)}); }
void S_net_ChannelConfig::from(EngineObjects& o, const Value& v, kern::net::ChannelConfig& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_net_ChannelType>>::from(o, *f, x.type); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.send_queue_size); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.receive_queue_size); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.max_messages_per_packet); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.packet_budget); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.max_message_size); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.block_threshold); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.block_fragment_size); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.resend_time); }
Value S_net_ClientServerConfig::to(EngineObjects& o, const kern::net::ClientServerConfig& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.protocol_id), ev::Conv<ev::Int>::to(o, x.max_clients), ev::Conv<ev::Float>::to(o, x.timeout), ev::Conv<ev::Float>::to(o, x.request_interval), ev::Conv<ev::Float>::to(o, x.connect_timeout), ev::Conv<ev::Float>::to(o, x.keepalive_interval), ev::Conv<ev::Struct<S_net_ConnectionConfig>>::to(o, x.connection)}); }
void S_net_ClientServerConfig::from(EngineObjects& o, const Value& v, kern::net::ClientServerConfig& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.protocol_id); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.max_clients); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.timeout); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.request_interval); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.connect_timeout); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.keepalive_interval); if (const Value* f = v.field(6)) ev::Conv<ev::Struct<S_net_ConnectionConfig>>::from(o, *f, x.connection); }
Value S_net_ConnectionConfig::to(EngineObjects& o, const kern::net::ConnectionConfig& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_net_ChannelConfig>>>::to(o, x.channels), ev::Conv<ev::Int>::to(o, x.max_packet_size), ev::Conv<ev::Int>::to(o, x.max_message_types), ev::Conv<ev::Struct<S_net_EndpointConfig>>::to(o, x.endpoint)}); }
void S_net_ConnectionConfig::from(EngineObjects& o, const Value& v, kern::net::ConnectionConfig& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_net_ChannelConfig>>>::from(o, *f, x.channels); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.max_packet_size); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.max_message_types); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_net_EndpointConfig>>::from(o, *f, x.endpoint); }
Value S_net_EndpointConfig::to(EngineObjects& o, const kern::net::EndpointConfig& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.max_packet_size), ev::Conv<ev::Int>::to(o, x.fragment_above), ev::Conv<ev::Int>::to(o, x.fragment_size), ev::Conv<ev::Int>::to(o, x.max_fragments), ev::Conv<ev::Int>::to(o, x.ack_buffer_size), ev::Conv<ev::Int>::to(o, x.sent_packets_buffer_size), ev::Conv<ev::Int>::to(o, x.received_packets_buffer_size), ev::Conv<ev::Int>::to(o, x.fragment_reassembly_buffer_size), ev::Conv<ev::Float>::to(o, x.rtt_smoothing), ev::Conv<ev::Float>::to(o, x.packet_loss_smoothing), ev::Conv<ev::Float>::to(o, x.bandwidth_smoothing), ev::Conv<ev::Int>::to(o, x.packet_header_size)}); }
void S_net_EndpointConfig::from(EngineObjects& o, const Value& v, kern::net::EndpointConfig& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.max_packet_size); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.fragment_above); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.fragment_size); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.max_fragments); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.ack_buffer_size); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.sent_packets_buffer_size); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.received_packets_buffer_size); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.fragment_reassembly_buffer_size); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.rtt_smoothing); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.packet_loss_smoothing); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.bandwidth_smoothing); if (const Value* f = v.field(11)) ev::Conv<ev::Int>::from(o, *f, x.packet_header_size); }
Value S_net_EndpointStats::to(EngineObjects& o, const kern::net::EndpointStats& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.rtt_ms), ev::Conv<ev::Float>::to(o, x.packet_loss), ev::Conv<ev::Float>::to(o, x.sent_kbps), ev::Conv<ev::Float>::to(o, x.received_kbps), ev::Conv<ev::Float>::to(o, x.acked_kbps), ev::Conv<ev::Int64>::to(o, x.packets_sent), ev::Conv<ev::Int64>::to(o, x.packets_received), ev::Conv<ev::Int64>::to(o, x.packets_acked), ev::Conv<ev::Int64>::to(o, x.packets_stale), ev::Conv<ev::Int64>::to(o, x.packets_invalid), ev::Conv<ev::Int64>::to(o, x.packets_too_large_to_send), ev::Conv<ev::Int64>::to(o, x.packets_too_large_to_receive), ev::Conv<ev::Int64>::to(o, x.fragments_sent), ev::Conv<ev::Int64>::to(o, x.fragments_received), ev::Conv<ev::Int64>::to(o, x.fragments_invalid)}); }
void S_net_EndpointStats::from(EngineObjects& o, const Value& v, kern::net::EndpointStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.rtt_ms); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.packet_loss); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.sent_kbps); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.received_kbps); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.acked_kbps); if (const Value* f = v.field(5)) ev::Conv<ev::Int64>::from(o, *f, x.packets_sent); if (const Value* f = v.field(6)) ev::Conv<ev::Int64>::from(o, *f, x.packets_received); if (const Value* f = v.field(7)) ev::Conv<ev::Int64>::from(o, *f, x.packets_acked); if (const Value* f = v.field(8)) ev::Conv<ev::Int64>::from(o, *f, x.packets_stale); if (const Value* f = v.field(9)) ev::Conv<ev::Int64>::from(o, *f, x.packets_invalid); if (const Value* f = v.field(10)) ev::Conv<ev::Int64>::from(o, *f, x.packets_too_large_to_send); if (const Value* f = v.field(11)) ev::Conv<ev::Int64>::from(o, *f, x.packets_too_large_to_receive); if (const Value* f = v.field(12)) ev::Conv<ev::Int64>::from(o, *f, x.fragments_sent); if (const Value* f = v.field(13)) ev::Conv<ev::Int64>::from(o, *f, x.fragments_received); if (const Value* f = v.field(14)) ev::Conv<ev::Int64>::from(o, *f, x.fragments_invalid); }
Value S_net_Message::to(EngineObjects& o, const kern::net::Message& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Int>::to(o, x.type), ev::Conv<ev::List<ev::Int>>::to(o, x.data)}); }
void S_net_Message::from(EngineObjects& o, const Value& v, kern::net::Message& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.type); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.data); }
Value S_net_SimulatorSettings::to(EngineObjects& o, const kern::net::SimulatorSettings& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.latency_ms), ev::Conv<ev::Float>::to(o, x.jitter_ms), ev::Conv<ev::Float>::to(o, x.loss_percent), ev::Conv<ev::Float>::to(o, x.duplicate_percent)}); }
void S_net_SimulatorSettings::from(EngineObjects& o, const Value& v, kern::net::SimulatorSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.latency_ms); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.jitter_ms); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.loss_percent); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.duplicate_percent); }
Value S_net_Simulator_Packet::to(EngineObjects& o, const kern::net::Simulator::Packet& x) { return Value::structure({ev::Conv<ev::Struct<S_net_Address>>::to(o, x.from), ev::Conv<ev::Struct<S_net_Address>>::to(o, x.to), ev::Conv<ev::Float>::to(o, x.deliver), ev::Conv<ev::List<ev::Int>>::to(o, x.data)}); }
void S_net_Simulator_Packet::from(EngineObjects& o, const Value& v, kern::net::Simulator::Packet& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_net_Address>>::from(o, *f, x.from); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_net_Address>>::from(o, *f, x.to); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.deliver); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.data); }

// engine.net.client_state_name: kern::net::client_state_name
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::ClientState>>();
  ev::take<ev::Enum<E_net_ClientState>>(o, in, 0, a0);
  decltype(auto) r = kern::net::client_state_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.client.connect: kern::net::Client::connect
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->connect(std::move(a0), std::move(a1));
  return true;
}

// engine.client.disconnect: kern::net::Client::disconnect
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  self->disconnect();
  return true;
}

// engine.client.state: kern::net::Client::state
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->state();
  ev::give<ev::Enum<E_net_ClientState>>(o, out, 0, r);
  return true;
}

// engine.client.connected: kern::net::Client::connected
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->connected();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.client.client_index: kern::net::Client::client_index
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->client_index();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.client.max_clients: kern::net::Client::max_clients
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->max_clients();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.client.receive_packets: kern::net::Client::receive_packets
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  self->receive_packets();
  return true;
}

// engine.client.advance_time: kern::net::Client::advance_time
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.client.send_packets: kern::net::Client::send_packets
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  self->send_packets();
  return true;
}

// engine.client.send: kern::net::Client::send
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned short>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >>>();
  ev::take<ev::List<ev::Int>>(o, in, 3, a2);
  decltype(auto) r = self->send(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.client.receive: kern::net::Client::receive
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->receive(std::move(a0));
  ev::give<ev::List<ev::Struct<S_net_Message>>>(o, out, 0, r);
  return true;
}

// engine.client.connection: kern::net::Client::connection
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->connection();
  out[0] = Value::integer(ev::handle_of<kern::net::Connection>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.client.connection_2: kern::net::Client::connection
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->connection();
  out[0] = Value::integer(ev::handle_of<kern::net::Connection>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.client.server: kern::net::Client::server
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Client>(o, in, 0, "client");
  if (!self) return false;
  decltype(auto) r = self->server();
  ev::give<ev::Struct<S_net_Address>>(o, out, 0, r);
  return true;
}

// engine.server.start: kern::net::Server::start
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->start(std::move(a0));
  return true;
}

// engine.server.stop: kern::net::Server::stop
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  self->stop();
  return true;
}

// engine.server.running: kern::net::Server::running
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  decltype(auto) r = self->running();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.server.max_clients: kern::net::Server::max_clients
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  decltype(auto) r = self->max_clients();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.server.client_connected: kern::net::Server::client_connected
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->client_connected(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.server.connected_clients: kern::net::Server::connected_clients
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  decltype(auto) r = self->connected_clients();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.server.client_address: kern::net::Server::client_address
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->client_address(std::move(a0));
  ev::give<ev::Struct<S_net_Address>>(o, out, 0, r);
  return true;
}

// engine.server.disconnect_client: kern::net::Server::disconnect_client
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->disconnect_client(std::move(a0));
  return true;
}

// engine.server.receive_packets: kern::net::Server::receive_packets
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  self->receive_packets();
  return true;
}

// engine.server.advance_time: kern::net::Server::advance_time
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.server.send_packets: kern::net::Server::send_packets
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  self->send_packets();
  return true;
}

// engine.server.send: kern::net::Server::send
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned short>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >>>();
  ev::take<ev::List<ev::Int>>(o, in, 4, a3);
  decltype(auto) r = self->send(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.server.receive: kern::net::Server::receive
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->receive(std::move(a0), std::move(a1));
  ev::give<ev::List<ev::Struct<S_net_Message>>>(o, out, 0, r);
  return true;
}

// engine.server.connection: kern::net::Server::connection
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->connection(std::move(a0));
  out[0] = Value::integer(ev::handle_of<kern::net::Connection>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.server.take_events: kern::net::Server::take_events
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Server>(o, in, 0, "server");
  if (!self) return false;
  decltype(auto) r = self->take_events();
  ev::give<ev::List<ev::Pair<ev::Int, ev::Bool>>>(o, out, 0, r);
  return true;
}

// engine.net.connection_error_name: kern::net::connection_error_name
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::ConnectionError>>();
  ev::take<ev::Enum<E_net_ConnectionError>>(o, in, 0, a0);
  decltype(auto) r = kern::net::connection_error_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.connection.reset: kern::net::Connection::reset
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  self->reset();
  return true;
}

// engine.connection.can_send: kern::net::Connection::can_send
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->can_send(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.connection.send: kern::net::Connection::send
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned short>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >>>();
  ev::take<ev::List<ev::Int>>(o, in, 3, a2);
  decltype(auto) r = self->send(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.connection.receive: kern::net::Connection::receive
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->receive(std::move(a0));
  ev::give<ev::List<ev::Struct<S_net_Message>>>(o, out, 0, r);
  return true;
}

// engine.connection.send_packet: kern::net::Connection::send_packet
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  self->send_packet();
  return true;
}

// engine.connection.receive_packet: kern::net::Connection::receive_packet
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  self->receive_packet(a0.empty() ? nullptr : a0.data(), std::move(a1));
  return true;
}

// engine.connection.advance_time: kern::net::Connection::advance_time
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.connection.error: kern::net::Connection::error
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  decltype(auto) r = self->error();
  ev::give<ev::Enum<E_net_ConnectionError>>(o, out, 0, r);
  return true;
}

// engine.connection.stats: kern::net::Connection::stats
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_net_EndpointStats>>(o, out, 0, r);
  return true;
}

// engine.connection.endpoint: kern::net::Connection::endpoint
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  decltype(auto) r = self->endpoint();
  out[0] = Value::integer(ev::handle_of<kern::net::Endpoint>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.connection.channel_count: kern::net::Connection::channel_count
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  decltype(auto) r = self->channel_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.connection.pending: kern::net::Connection::pending
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Connection>(o, in, 0, "connection");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->pending(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.endpoint.reset: kern::net::Endpoint::reset
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  self->reset();
  return true;
}

// engine.endpoint.next_sequence: kern::net::Endpoint::next_sequence
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  decltype(auto) r = self->next_sequence();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.endpoint.send_packet: kern::net::Endpoint::send_packet
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list payload has fewer entries than bytes says"; return false; }
  decltype(auto) r = self->send_packet(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.endpoint.receive_packet: kern::net::Endpoint::receive_packet
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list packet has fewer entries than bytes says"; return false; }
  self->receive_packet(a0.empty() ? nullptr : a0.data(), std::move(a1));
  return true;
}

// engine.endpoint.acks: kern::net::Endpoint::acks
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  decltype(auto) r = self->acks();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.endpoint.clear_acks: kern::net::Endpoint::clear_acks
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  self->clear_acks();
  return true;
}

// engine.endpoint.update: kern::net::Endpoint::update
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->update(std::move(a0));
  return true;
}

// engine.endpoint.time: kern::net::Endpoint::time
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  decltype(auto) r = self->time();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.endpoint.stats: kern::net::Endpoint::stats
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_net_EndpointStats>>(o, out, 0, r);
  return true;
}

// engine.endpoint.config: kern::net::Endpoint::config
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Endpoint>(o, in, 0, "endpoint");
  if (!self) return false;
  decltype(auto) r = self->config();
  ev::give<ev::Struct<S_net_EndpointConfig>>(o, out, 0, r);
  return true;
}

// engine.bit_writer.write_bits: kern::net::BitWriter::write_bits
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->write_bits(std::move(a0), std::move(a1));
  return true;
}

// engine.bit_writer.write_align: kern::net::BitWriter::write_align
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  self->write_align();
  return true;
}

// engine.bit_writer.write_bytes: kern::net::BitWriter::write_bytes
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  self->write_bytes(a0.empty() ? nullptr : a0.data(), std::move(a1));
  return true;
}

// engine.bit_writer.flush: kern::net::BitWriter::flush
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  self->flush();
  return true;
}

// engine.bit_writer.align_bits: kern::net::BitWriter::align_bits
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  decltype(auto) r = self->align_bits();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_writer.bits_written: kern::net::BitWriter::bits_written
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  decltype(auto) r = self->bits_written();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_writer.bits_available: kern::net::BitWriter::bits_available
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  decltype(auto) r = self->bits_available();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_writer.bytes_written: kern::net::BitWriter::bytes_written
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  decltype(auto) r = self->bytes_written();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_writer.overflow: kern::net::BitWriter::overflow
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitWriter>(o, in, 0, "bit_writer");
  if (!self) return false;
  decltype(auto) r = self->overflow();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.bit_reader.would_overflow: kern::net::BitReader::would_overflow
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->would_overflow(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.bit_reader.read_bits: kern::net::BitReader::read_bits
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->read_bits(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_reader.read_align: kern::net::BitReader::read_align
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  decltype(auto) r = self->read_align();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.bit_reader.read_bytes: kern::net::BitReader::read_bytes
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 1, a1);
  ev::Written<unsigned char> a0(ev::room(a1) * 1);
  self->read_bytes(a0.data(), std::move(a1));
  if (!a0.kept(o, "data")) return false;
  ev::give<ev::List<ev::Int>>(o, out, 0, a0.room);
  return true;
}

// engine.bit_reader.align_bits: kern::net::BitReader::align_bits
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  decltype(auto) r = self->align_bits();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_reader.bits_read: kern::net::BitReader::bits_read
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  decltype(auto) r = self->bits_read();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_reader.bits_remaining: kern::net::BitReader::bits_remaining
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  decltype(auto) r = self->bits_remaining();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bit_reader.bytes_read: kern::net::BitReader::bytes_read
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::BitReader>(o, in, 0, "bit_reader");
  if (!self) return false;
  decltype(auto) r = self->bytes_read();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.write_stream.serialize_integer: kern::net::WriteStream::serialize_integer
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  decltype(auto) r = self->serialize_integer(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.write_stream.serialize_bits: kern::net::WriteStream::serialize_bits
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->serialize_bits(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.write_stream.serialize_bytes: kern::net::WriteStream::serialize_bytes
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  decltype(auto) r = self->serialize_bytes(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.write_stream.serialize_align: kern::net::WriteStream::serialize_align
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  decltype(auto) r = self->serialize_align();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.write_stream.serialize_check: kern::net::WriteStream::serialize_check
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->serialize_check(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.write_stream.flush: kern::net::WriteStream::flush
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  self->flush();
  return true;
}

// engine.write_stream.bits_processed: kern::net::WriteStream::bits_processed
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  decltype(auto) r = self->bits_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.write_stream.bytes_processed: kern::net::WriteStream::bytes_processed
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  decltype(auto) r = self->bytes_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.write_stream.align_bits: kern::net::WriteStream::align_bits
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  decltype(auto) r = self->align_bits();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.write_stream.overflow: kern::net::WriteStream::overflow
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::WriteStream>(o, in, 0, "write_stream");
  if (!self) return false;
  decltype(auto) r = self->overflow();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.read_stream.serialize_integer: kern::net::ReadStream::serialize_integer
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int&>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  decltype(auto) r = self->serialize_integer(a0, std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Int>(o, out, 1, a0);
  return true;
}

// engine.read_stream.serialize_bits: kern::net::ReadStream::serialize_bits
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int&>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->serialize_bits(a0, std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Int>(o, out, 1, a0);
  return true;
}

// engine.read_stream.serialize_bytes: kern::net::ReadStream::serialize_bytes
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 1, a1);
  ev::Written<unsigned char> a0(ev::room(a1) * 1);
  decltype(auto) r = self->serialize_bytes(a0.data(), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  if (!a0.kept(o, "data")) return false;
  ev::give<ev::List<ev::Int>>(o, out, 1, a0.room);
  return true;
}

// engine.read_stream.serialize_align: kern::net::ReadStream::serialize_align
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  decltype(auto) r = self->serialize_align();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.read_stream.serialize_check: kern::net::ReadStream::serialize_check
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->serialize_check(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.read_stream.bits_processed: kern::net::ReadStream::bits_processed
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  decltype(auto) r = self->bits_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.read_stream.bytes_processed: kern::net::ReadStream::bytes_processed
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  decltype(auto) r = self->bytes_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.read_stream.bits_remaining: kern::net::ReadStream::bits_remaining
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  decltype(auto) r = self->bits_remaining();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.read_stream.align_bits: kern::net::ReadStream::align_bits
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::ReadStream>(o, in, 0, "read_stream");
  if (!self) return false;
  decltype(auto) r = self->align_bits();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.measure_stream.serialize_integer: kern::net::MeasureStream::serialize_integer
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  decltype(auto) r = self->serialize_integer(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.measure_stream.serialize_bits: kern::net::MeasureStream::serialize_bits
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->serialize_bits(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.measure_stream.serialize_bytes: kern::net::MeasureStream::serialize_bytes
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  std::vector<unsigned char> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list value1 has fewer entries than bytes says"; return false; }
  decltype(auto) r = self->serialize_bytes(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.measure_stream.serialize_align: kern::net::MeasureStream::serialize_align
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  decltype(auto) r = self->serialize_align();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.measure_stream.serialize_check: kern::net::MeasureStream::serialize_check
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->serialize_check(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.measure_stream.bits_processed: kern::net::MeasureStream::bits_processed
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  decltype(auto) r = self->bits_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.measure_stream.bytes_processed: kern::net::MeasureStream::bytes_processed
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  decltype(auto) r = self->bytes_processed();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.measure_stream.align_bits: kern::net::MeasureStream::align_bits
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::MeasureStream>(o, in, 0, "measure_stream");
  if (!self) return false;
  decltype(auto) r = self->align_bits();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.transport.address: kern::net::Transport::address
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!self) return false;
  decltype(auto) r = self->address();
  ev::give<ev::Struct<S_net_Address>>(o, out, 0, r);
  return true;
}

// engine.transport.send: kern::net::Transport::send
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  decltype(auto) r = self->send(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.transport.receive: kern::net::Transport::receive
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address&>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a2);
  ev::Written<unsigned char> a1(ev::room(a2) * 1);
  decltype(auto) r = self->receive(a0, a1.data(), std::move(a2));
  ev::give<ev::Int>(o, out, 0, r);
  ev::give<ev::Struct<S_net_Address>>(o, out, 1, a0);
  if (!a1.kept(o, "data")) return false;
  ev::give<ev::List<ev::Int>>(o, out, 2, a1.room);
  return true;
}

// engine.transport.advance_time: kern::net::Transport::advance_time
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.simulator.send: kern::net::Simulator::send
bool c101(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 2, a1);
  std::vector<unsigned char> a2;
  ev::take<ev::List<ev::Int>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 5, a4);
  if (ev::numbers(a3) > a2.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  self->send(std::move(a0), std::move(a1), a2.empty() ? nullptr : a2.data(), std::move(a3), std::move(a4));
  return true;
}

// engine.simulator.take_due: kern::net::Simulator::take_due
bool c102(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<kern::net::Simulator::Packet, std::allocator<kern::net::Simulator::Packet> >&>>();
  ev::take<ev::List<ev::Struct<S_net_Simulator_Packet>>>(o, in, 2, a1);
  self->take_due(std::move(a0), a1);
  ev::give<ev::List<ev::Struct<S_net_Simulator_Packet>>>(o, out, 0, a1);
  return true;
}

// engine.simulator.clear: kern::net::Simulator::clear
bool c103(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.simulator.in_flight: kern::net::Simulator::in_flight
bool c104(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  decltype(auto) r = self->in_flight();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.simulator.dropped: kern::net::Simulator::dropped
bool c105(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  decltype(auto) r = self->dropped();
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.simulator.duplicated: kern::net::Simulator::duplicated
bool c106(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  decltype(auto) r = self->duplicated();
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.loopback_network.create: kern::net::LoopbackNetwork::create
bool c107(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::LoopbackNetwork>(o, in, 0, "loopback_network");
  if (!self) return false;
  decltype(auto) r = self->create();
  out[0] = Value::integer(ev::handle_of<kern::net::Transport>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.loopback_network.simulator: kern::net::LoopbackNetwork::simulator
bool c108(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::LoopbackNetwork>(o, in, 0, "loopback_network");
  if (!self) return false;
  decltype(auto) r = self->simulator();
  out[0] = Value::integer(ev::handle_of<kern::net::Simulator>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.loopback_network.advance_time: kern::net::LoopbackNetwork::advance_time
bool c109(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::LoopbackNetwork>(o, in, 0, "loopback_network");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.loopback_network.time: kern::net::LoopbackNetwork::time
bool c110(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::LoopbackNetwork>(o, in, 0, "loopback_network");
  if (!self) return false;
  decltype(auto) r = self->time();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.loopback_network.delivered: kern::net::LoopbackNetwork::delivered
bool c111(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::LoopbackNetwork>(o, in, 0, "loopback_network");
  if (!self) return false;
  decltype(auto) r = self->delivered();
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.host_transport.address: kern::net::HostTransport::address
bool c112(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  decltype(auto) r = self->address();
  ev::give<ev::Struct<S_net_Address>>(o, out, 0, r);
  return true;
}

// engine.host_transport.set_address: kern::net::HostTransport::set_address
bool c113(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  self->set_address(std::move(a0));
  return true;
}

// engine.host_transport.send: kern::net::HostTransport::send
bool c114(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  decltype(auto) r = self->send(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.host_transport.receive: kern::net::HostTransport::receive
bool c115(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address&>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a2);
  ev::Written<unsigned char> a1(ev::room(a2) * 1);
  decltype(auto) r = self->receive(a0, a1.data(), std::move(a2));
  ev::give<ev::Int>(o, out, 0, r);
  ev::give<ev::Struct<S_net_Address>>(o, out, 1, a0);
  if (!a1.kept(o, "data")) return false;
  ev::give<ev::List<ev::Int>>(o, out, 2, a1.room);
  return true;
}

// engine.host_transport.advance_time: kern::net::HostTransport::advance_time
bool c116(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->advance_time(std::move(a0));
  return true;
}

// engine.host_transport.push_incoming: kern::net::HostTransport::push_incoming
bool c117(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list data has fewer entries than bytes says"; return false; }
  self->push_incoming(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  return true;
}

// engine.host_transport.take_outgoing: kern::net::HostTransport::take_outgoing
bool c118(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address&>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  decltype(auto) r = self->take_outgoing(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_net_Address>>(o, out, 1, a0);
  ev::give<ev::List<ev::Int>>(o, out, 2, a1);
  return true;
}

// engine.host_transport.simulator: kern::net::HostTransport::simulator
bool c119(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::HostTransport>(o, in, 0, "host_transport");
  if (!self) return false;
  decltype(auto) r = self->simulator();
  out[0] = Value::integer(ev::handle_of<kern::net::Simulator>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.client.make: kern::net::Client::Client
bool c120(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::net::Transport* a0 = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::net::ClientServerConfig const&>>();
  ev::take<ev::Struct<S_net_ClientServerConfig>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 2, a2);
  out[0] = Value::integer(o.own<kern::net::Client>(std::unique_ptr<kern::net::Client>(new kern::net::Client(*a0, a1, std::move(a2)))));
  return true;
}

// engine.client.release: kern::net::Client::~Client
bool c121(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::Client>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Client that was made";
    return false;
  }
  return true;
}

// engine.connection.make: kern::net::Connection::Connection
bool c122(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::ConnectionConfig const&>>();
  ev::take<ev::Struct<S_net_ConnectionConfig>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a1);
  out[0] = Value::integer(o.own<kern::net::Connection>(std::unique_ptr<kern::net::Connection>(new kern::net::Connection(a0, std::move(a1)))));
  return true;
}

// engine.connection.release: kern::net::Connection::~Connection
bool c123(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::Connection>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Connection that was made";
    return false;
  }
  return true;
}

// engine.host_transport.make: kern::net::HostTransport::HostTransport
bool c124(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::net::Address>>();
  ev::take<ev::Struct<S_net_Address>>(o, in, 0, a0);
  out[0] = Value::integer(o.own<kern::net::HostTransport, kern::net::Transport>(std::unique_ptr<kern::net::HostTransport>(new kern::net::HostTransport(std::move(a0)))));
  return true;
}

// engine.host_transport.release: kern::net::HostTransport::~HostTransport
bool c125(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::HostTransport>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no HostTransport that was made";
    return false;
  }
  return true;
}

// engine.loopback_network.make: kern::net::LoopbackNetwork::LoopbackNetwork
bool c126(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  out[0] = Value::integer(o.own<kern::net::LoopbackNetwork>(std::unique_ptr<kern::net::LoopbackNetwork>(new kern::net::LoopbackNetwork(std::move(a0)))));
  return true;
}

// engine.loopback_network.release: kern::net::LoopbackNetwork::~LoopbackNetwork
bool c127(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::LoopbackNetwork>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no LoopbackNetwork that was made";
    return false;
  }
  return true;
}

// engine.measure_stream.make: kern::net::MeasureStream::MeasureStream
bool c128(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::net::MeasureStream>(std::unique_ptr<kern::net::MeasureStream>(new kern::net::MeasureStream())));
  return true;
}

// engine.measure_stream.release: kern::net::MeasureStream::~MeasureStream
bool c129(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::MeasureStream>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no MeasureStream that was made";
    return false;
  }
  return true;
}

// engine.server.make: kern::net::Server::Server
bool c130(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::net::Transport* a0 = ev::object<kern::net::Transport>(o, in, 0, "transport");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::net::ClientServerConfig const&>>();
  ev::take<ev::Struct<S_net_ClientServerConfig>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 2, a2);
  out[0] = Value::integer(o.own<kern::net::Server>(std::unique_ptr<kern::net::Server>(new kern::net::Server(*a0, a1, std::move(a2)))));
  return true;
}

// engine.server.release: kern::net::Server::~Server
bool c131(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::Server>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Server that was made";
    return false;
  }
  return true;
}

// engine.simulator.make: kern::net::Simulator::Simulator
bool c132(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  out[0] = Value::integer(o.own<kern::net::Simulator>(std::unique_ptr<kern::net::Simulator>(new kern::net::Simulator(std::move(a0)))));
  return true;
}

// engine.simulator.release: kern::net::Simulator::~Simulator
bool c133(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::net::Simulator>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Simulator that was made";
    return false;
  }
  return true;
}

// engine.simulator.settings: kern::net::Simulator::settings
bool c134(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  ev::give<ev::Struct<S_net_SimulatorSettings>>(o, out, 0, self->settings);
  return true;
}

// engine.simulator.settings@set: what stands at the place kern::net::Simulator::settings gives
bool c134_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::net::Simulator>(o, in, 0, "simulator");
  if (!self) return false;
  ev::take<ev::Struct<S_net_SimulatorSettings>>(o, in, 1, self->settings);
  return true;
}

// kern::net::Transport: the object of the engine a Blueprint is (Graph::implements "net_Transport")
class Implements_net_Transport final : public kern::net::Transport {
 public:
  Implements_net_Transport(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<const kern::net::Transport&>().address()) address() const override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("address", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::net::Transport&>().address()) kern_r = ev::blank<decltype(std::declval<const kern::net::Transport&>().address())>();
    if (kern_given) ev::Conv<ev::Struct<S_net_Address>>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::net::Transport&>().send(std::declval<kern::net::Address>(), std::declval<unsigned char const*>(), std::declval<kern::Count<1u>>())) send(kern::net::Address kern_p0, unsigned char const* kern_p1, kern::Count<1u> kern_p2) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Struct<S_net_Address>>::to(o_, kern_p0), ev::Conv<ev::List<ev::Int>>::to(o_, kern::Span<const unsigned char>(kern_p1, static_cast<std::size_t>(ev::numbers(kern_p2)))), ev::Conv<ev::Int>::to(o_, kern_p2)};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("send", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<kern::net::Transport&>().send(std::declval<kern::net::Address>(), std::declval<unsigned char const*>(), std::declval<kern::Count<1u>>())) kern_r = ev::blank<decltype(std::declval<kern::net::Transport&>().send(std::declval<kern::net::Address>(), std::declval<unsigned char const*>(), std::declval<kern::Count<1u>>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::net::Transport&>().receive(std::declval<kern::net::Address&>(), std::declval<unsigned char*>(), std::declval<kern::Count<1u>>())) receive(kern::net::Address& kern_p0, unsigned char* kern_p1, kern::Count<1u> kern_p2) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Struct<S_net_Address>>::to(o_, kern_p0), ev::Conv<ev::Int>::to(o_, kern_p2)};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("receive", kern_in, kern_out) && !kern_out.empty();
    if (kern_given && kern_out.size() > 1) ev::Conv<ev::Struct<S_net_Address>>::from(o_, kern_out[1], kern_p0);
    if (kern_given && kern_out.size() > 2) {
      std::vector<std::remove_pointer_t<decltype(kern_p1)>> kern_w;
      ev::Conv<ev::List<ev::Int>>::from(o_, kern_out[2], kern_w);
      std::copy_n(kern_w.begin(), std::min(kern_w.size(), static_cast<std::size_t>(ev::numbers(kern_p2))), kern_p1);
    }
    decltype(std::declval<kern::net::Transport&>().receive(std::declval<kern::net::Address&>(), std::declval<unsigned char*>(), std::declval<kern::Count<1u>>())) kern_r = ev::blank<decltype(std::declval<kern::net::Transport&>().receive(std::declval<kern::net::Address&>(), std::declval<unsigned char*>(), std::declval<kern::Count<1u>>()))>();
    if (kern_given) ev::Conv<ev::Int>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::net::Transport&>().advance_time(std::declval<double>())) advance_time(double kern_p0) override {
    if (!by_->has("advance_time")) return kern::net::Transport::advance_time(kern_p0);
    const std::vector<Value> kern_in{ev::Conv<ev::Float>::to(o_, kern_p0)};
    std::vector<Value> kern_out;
    by_->call("advance_time", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
};
i64 implements_net_Transport(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_net_Transport, kern::net::Transport>(std::make_unique<Implements_net_Transport>(o, std::move(by)));
}

const EngineCall kCalls[] = {
    {"engine.net.client_state_name", &c0},
    {"engine.client.connect", &c1},
    {"engine.client.disconnect", &c2},
    {"engine.client.state", &c3},
    {"engine.client.connected", &c4},
    {"engine.client.client_index", &c5},
    {"engine.client.max_clients", &c6},
    {"engine.client.receive_packets", &c7},
    {"engine.client.advance_time", &c8},
    {"engine.client.send_packets", &c9},
    {"engine.client.send", &c10},
    {"engine.client.receive", &c11},
    {"engine.client.connection", &c12},
    {"engine.client.connection_2", &c13},
    {"engine.client.server", &c14},
    {"engine.server.start", &c15},
    {"engine.server.stop", &c16},
    {"engine.server.running", &c17},
    {"engine.server.max_clients", &c18},
    {"engine.server.client_connected", &c19},
    {"engine.server.connected_clients", &c20},
    {"engine.server.client_address", &c21},
    {"engine.server.disconnect_client", &c22},
    {"engine.server.receive_packets", &c23},
    {"engine.server.advance_time", &c24},
    {"engine.server.send_packets", &c25},
    {"engine.server.send", &c26},
    {"engine.server.receive", &c27},
    {"engine.server.connection", &c28},
    {"engine.server.take_events", &c29},
    {"engine.net.connection_error_name", &c30},
    {"engine.connection.reset", &c31},
    {"engine.connection.can_send", &c32},
    {"engine.connection.send", &c33},
    {"engine.connection.receive", &c34},
    {"engine.connection.send_packet", &c35},
    {"engine.connection.receive_packet", &c36},
    {"engine.connection.advance_time", &c37},
    {"engine.connection.error", &c38},
    {"engine.connection.stats", &c39},
    {"engine.connection.endpoint", &c40},
    {"engine.connection.channel_count", &c41},
    {"engine.connection.pending", &c42},
    {"engine.endpoint.reset", &c43},
    {"engine.endpoint.next_sequence", &c44},
    {"engine.endpoint.send_packet", &c45},
    {"engine.endpoint.receive_packet", &c46},
    {"engine.endpoint.acks", &c47},
    {"engine.endpoint.clear_acks", &c48},
    {"engine.endpoint.update", &c49},
    {"engine.endpoint.time", &c50},
    {"engine.endpoint.stats", &c51},
    {"engine.endpoint.config", &c52},
    {"engine.bit_writer.write_bits", &c53},
    {"engine.bit_writer.write_align", &c54},
    {"engine.bit_writer.write_bytes", &c55},
    {"engine.bit_writer.flush", &c56},
    {"engine.bit_writer.align_bits", &c57},
    {"engine.bit_writer.bits_written", &c58},
    {"engine.bit_writer.bits_available", &c59},
    {"engine.bit_writer.bytes_written", &c60},
    {"engine.bit_writer.overflow", &c61},
    {"engine.bit_reader.would_overflow", &c62},
    {"engine.bit_reader.read_bits", &c63},
    {"engine.bit_reader.read_align", &c64},
    {"engine.bit_reader.read_bytes", &c65},
    {"engine.bit_reader.align_bits", &c66},
    {"engine.bit_reader.bits_read", &c67},
    {"engine.bit_reader.bits_remaining", &c68},
    {"engine.bit_reader.bytes_read", &c69},
    {"engine.write_stream.serialize_integer", &c70},
    {"engine.write_stream.serialize_bits", &c71},
    {"engine.write_stream.serialize_bytes", &c72},
    {"engine.write_stream.serialize_align", &c73},
    {"engine.write_stream.serialize_check", &c74},
    {"engine.write_stream.flush", &c75},
    {"engine.write_stream.bits_processed", &c76},
    {"engine.write_stream.bytes_processed", &c77},
    {"engine.write_stream.align_bits", &c78},
    {"engine.write_stream.overflow", &c79},
    {"engine.read_stream.serialize_integer", &c80},
    {"engine.read_stream.serialize_bits", &c81},
    {"engine.read_stream.serialize_bytes", &c82},
    {"engine.read_stream.serialize_align", &c83},
    {"engine.read_stream.serialize_check", &c84},
    {"engine.read_stream.bits_processed", &c85},
    {"engine.read_stream.bytes_processed", &c86},
    {"engine.read_stream.bits_remaining", &c87},
    {"engine.read_stream.align_bits", &c88},
    {"engine.measure_stream.serialize_integer", &c89},
    {"engine.measure_stream.serialize_bits", &c90},
    {"engine.measure_stream.serialize_bytes", &c91},
    {"engine.measure_stream.serialize_align", &c92},
    {"engine.measure_stream.serialize_check", &c93},
    {"engine.measure_stream.bits_processed", &c94},
    {"engine.measure_stream.bytes_processed", &c95},
    {"engine.measure_stream.align_bits", &c96},
    {"engine.transport.address", &c97},
    {"engine.transport.send", &c98},
    {"engine.transport.receive", &c99},
    {"engine.transport.advance_time", &c100},
    {"engine.simulator.send", &c101},
    {"engine.simulator.take_due", &c102},
    {"engine.simulator.clear", &c103},
    {"engine.simulator.in_flight", &c104},
    {"engine.simulator.dropped", &c105},
    {"engine.simulator.duplicated", &c106},
    {"engine.loopback_network.create", &c107},
    {"engine.loopback_network.simulator", &c108},
    {"engine.loopback_network.advance_time", &c109},
    {"engine.loopback_network.time", &c110},
    {"engine.loopback_network.delivered", &c111},
    {"engine.host_transport.address", &c112},
    {"engine.host_transport.set_address", &c113},
    {"engine.host_transport.send", &c114},
    {"engine.host_transport.receive", &c115},
    {"engine.host_transport.advance_time", &c116},
    {"engine.host_transport.push_incoming", &c117},
    {"engine.host_transport.take_outgoing", &c118},
    {"engine.host_transport.simulator", &c119},
    {"engine.client.make", &c120},
    {"engine.client.release", &c121},
    {"engine.connection.make", &c122},
    {"engine.connection.release", &c123},
    {"engine.host_transport.make", &c124},
    {"engine.host_transport.release", &c125},
    {"engine.loopback_network.make", &c126},
    {"engine.loopback_network.release", &c127},
    {"engine.measure_stream.make", &c128},
    {"engine.measure_stream.release", &c129},
    {"engine.server.make", &c130},
    {"engine.server.release", &c131},
    {"engine.simulator.make", &c132},
    {"engine.simulator.release", &c133},
    {"engine.simulator.settings", &c134},
    {"engine.simulator.settings@get", &c134},
    {"engine.simulator.settings@set", &c134_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::net::Client>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::net::Client>>(); auto& d0 = made->keep(new kern::net::HostTransport()); made->object.emplace(d0, ev::blank<std::remove_cvref_t<kern::net::ClientServerConfig const&>>(), ev::blank<std::remove_cvref_t<kern::u64>>()); return o.put_made<kern::net::Client>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::net::Connection>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::net::Connection>(handle, std::unique_ptr<kern::net::Connection>(new kern::net::Connection())); }},
    {EngineObjects::kind<kern::net::HostTransport>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::net::HostTransport, kern::net::Transport>(handle, std::unique_ptr<kern::net::HostTransport>(new kern::net::HostTransport())); }},
    {EngineObjects::kind<kern::net::LoopbackNetwork>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::net::LoopbackNetwork>(handle, std::unique_ptr<kern::net::LoopbackNetwork>(new kern::net::LoopbackNetwork())); }},
    {EngineObjects::kind<kern::net::MeasureStream>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::net::MeasureStream>(handle, std::unique_ptr<kern::net::MeasureStream>(new kern::net::MeasureStream())); }},
    {EngineObjects::kind<kern::net::Server>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::net::Server>>(); auto& d0 = made->keep(new kern::net::HostTransport()); made->object.emplace(d0, ev::blank<std::remove_cvref_t<kern::net::ClientServerConfig const&>>(), ev::blank<std::remove_cvref_t<kern::u64>>()); return o.put_made<kern::net::Server>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::net::Simulator>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::net::Simulator>(handle, std::unique_ptr<kern::net::Simulator>(new kern::net::Simulator())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

const EngineImplement kAdapters[] = {
    {"net_Transport", &implements_net_Transport},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
