struct net_session_cpp : virtual kern::Library, virtual engine_net_client_server, virtual jet_seats, virtual jet_seats_cpp, virtual mp_rules, virtual net_session, virtual wire {
  // ---- variables ----
  /// ---- players -----------------------------------------------------------------------------------
  KERN_CONST Int kMaxPlayers = 9;
  /// units in one message (18 bytes each, under the 1200 of an unreliable one)
  KERN_CONST Int kSwarmPerMessage = 64;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// the host's slot
  KERN_CONST Int kHost = 255;
  /// a player silent this long is gone
  KERN_CONST Float kQuietS = 6.0;
  /// no shooter (an accident, the police, a fall)
  KERN_CONST Int kNobody = 254;

  // ---- functions ----
  struct configResult { net_ClientServerConfig Result = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}}; };
  KERN_PURE configResult config() {
    net_ClientServerConfig c = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}};
    net_ChannelConfig reliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    net_ChannelConfig unreliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    c = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}};
    c.protocol_id = Int64{0x53504157534e3032};
    c.max_clients = kern::to<Int>(kern::formula([](Float kMaxPlayers) -> Float { return kern::math::wrap32s(kern::math::wrap32(kMaxPlayers - 1.0)); }, kern::to<Float>(kMaxPlayers)));
    reliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    reliable.type = net_ChannelType::reliable_ordered;
    unreliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    unreliable.type = net_ChannelType::unreliable_unordered;
    unreliable.max_message_size = 1200;
    c.connection.channels = list_make<net_ChannelConfig, "2">(reliable, unreliable);
    c.connection.max_packet_size = 1300;
    return {c};
  }

  struct saneResult { Bool Result = false; };
  KERN_PURE saneResult sane(Vector v = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(logic_and(logic_and(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).x), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).y), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).z), 0.0)), math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(v).x), 1000000.0)), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(v).y), 1000000.0)), math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(v).z), 100000.0))};
  }

  struct pack_swarm_q16Result { Int Result = 0; };
  KERN_PURE pack_swarm_q16Result pack_swarm_q16(Float v = 0.0, Float k = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(a, 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v, Float k) -> Float { return kern::math::round((v * k)); }, v, k)))), -32767.0, 32767.0)))};
  }

  struct pack_swarmResult { List<Int> Result = List<Int>{}; };
  /// One part of a player's swarm snapshot: owner, the snapshot's serial, the part's number (bit 7: the last one), its units - 18 bytes a unit (64 fit in one unreliable message of 1200): z in dm, yaw and aux in 1/100. (`n` units of the list, from `from` on)
  pack_swarmResult pack_swarm(const List<net_SwarmUnit>& units, Int owner = 0, Int serial = 0, Int part = 0, Bool last = false, Int from = 0, Int n = 0) {
    List<Int> w = List<Int>{};
    net_SwarmUnit u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    w = List<Int>{};
    wire::put_u8(w, owner);
    wire::put_u8(w, serial);
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float part, Float last) -> Float { return kern::math::band(kern::math::bor(part, ((last != 0.0) ? 128.0 : 0.0)), 255.0); }, kern::to<Float>(part), kern::to<Float>(last))));
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::band(n, 255.0); }, kern::to<Float>(n))));
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it10.next();) {
      u = units[kern::to<Int>(kern::formula([](Float from, Float index) -> Float { return (from + index); }, kern::to<Float>(from), kern::to<Float>(n10.index)))];
      wire::put_u16(w, u.id);
      wire::put_u8(w, enum_index<net_SwarmKind>(u.kind));
      wire::put_u8(w, u.flags);
      wire::put_f32(w, vector_break(u.p).x);
      wire::put_f32(w, vector_break(u.p).y);
      wire::put_i16(w, pack_swarm_q16(vector_break(u.p).z, 10.0).Result);
      wire::put_i16(w, pack_swarm_q16(u.yaw, 100.0).Result);
      wire::put_i16(w, pack_swarm_q16(u.aux, 100.0).Result);
    }
    return {w};
  }

  struct unpack_swarmResult { Bool Result = false; };
  unpack_swarmResult unpack_swarm(const List<Int>& b, Int& owner, Int& serial, Int& part, Bool& last, List<net_SwarmUnit>& units) {
    wire_Reader r = wire_Reader{0, true};
    Int pt = 0;
    Int n = 0;
    Int i = 0;
    net_SwarmUnit u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    Int k = 0;
    r = wire_Reader{0, true};
    n11 = wire::Reader_get_u8(r, b);
    owner = n11.Result;
    n12 = wire::Reader_get_u8(r, b);
    serial = n12.Result;
    n13 = wire::Reader_get_u8(r, b);
    pt = n13.Result;
    part = kern::to<Int>(kern::formula([](Float pt) -> Float { return kern::math::band(kern::math::band(pt, 127.0), 255.0); }, kern::to<Float>(pt)));
    last = math_compare<">">(kern::formula([](Float pt) -> Float { return (kern::math::band(pt, 128.0) != 0.0); }, kern::to<Float>(pt)), 0.5);
    n14 = wire::Reader_get_u8(r, b);
    n = n14.Result;
    list_clear(units);
    if (math_compare<">">(kern::formula([](Float kSwarmPerMessage, Float n) -> Float { return ((kSwarmPerMessage < 0.0) != (n < 0.0) ? (n < 0.0) : (kSwarmPerMessage < n)); }, kern::to<Float>(kSwarmPerMessage), kern::to<Float>(n)), 0.5)) {
      return {false};
    }
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), r.ok))) {
      u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
      n15 = wire::Reader_get_u16(r, b);
      u.id = n15.Result;
      n16 = wire::Reader_get_u8(r, b);
      k = n16.Result;
      n17 = wire::Reader_get_u8(r, b);
      u.flags = n17.Result;
      n18 = wire::Reader_get_f32(r, b);
      u.p = vector_make(n18.Result, vector_break(u.p).y, vector_break(u.p).z);
      n19 = wire::Reader_get_f32(r, b);
      u.p = vector_make(vector_break(u.p).x, n19.Result, vector_break(u.p).z);
      n20 = wire::Reader_get_i16(r, b);
      u.p = vector_make(vector_break(u.p).x, vector_break(u.p).y, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 10.0); }, kern::to<Float>(n20.Result)));
      n21 = wire::Reader_get_i16(r, b);
      u.yaw = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, kern::to<Float>(n21.Result));
      n22 = wire::Reader_get_i16(r, b);
      u.aux = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, kern::to<Float>(n22.Result));
      if (logic_or(logic_or(logic_or(logic_or(logic_not(r.ok), math_compare<">">(kern::to<Float>(k), kern::to<Float>(enum_index<net_SwarmKind>(net_SwarmKind::enemy)))), logic_not(sane(u.p).Result)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, u.yaw), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, u.aux), 0.0)))) {
        return {false};
      }
      u.kind = enum_from_index<net_SwarmKind>(k).value;
      list_add(units, u);
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::band((i + 1.0), 255.0); }, kern::to<Float>(i)));
    }
    return {r.ok};
  }

  struct pack_swarm_hitResult { List<Int> Result = List<Int>{}; };
  pack_swarm_hitResult pack_swarm_hit(const net_SwarmHit& h) {
    List<Int> w = List<Int>{};
    w = List<Int>{};
    wire::put_u8(w, h.from);
    wire::put_u8(w, h.to);
    wire::put_u16(w, h.id);
    wire::put_u8(w, enum_index<net_SwarmKind>(h.kind));
    wire::put_f32(w, h.damage);
    return {w};
  }

  struct unpack_swarm_hitResult { Bool Result = false; };
  unpack_swarm_hitResult unpack_swarm_hit(const List<Int>& b, net_SwarmHit& h) {
    wire_Reader r = wire_Reader{0, true};
    Int k = 0;
    r = wire_Reader{0, true};
    n23 = wire::Reader_get_u8(r, b);
    h.from = n23.Result;
    n24 = wire::Reader_get_u8(r, b);
    h.to = n24.Result;
    n25 = wire::Reader_get_u16(r, b);
    h.id = n25.Result;
    n26 = wire::Reader_get_u8(r, b);
    k = n26.Result;
    n27 = wire::Reader_get_f32(r, b);
    h.damage = n27.Result;
    h.kind = enum_from_index<net_SwarmKind>(k).value;
    return {logic_and(logic_and(logic_and(logic_and(r.ok, math_compare<"<=">(kern::to<Float>(k), kern::to<Float>(enum_index<net_SwarmKind>(net_SwarmKind::enemy)))), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, h.damage), 0.0)), math_compare<">">(h.damage, 0.0)), math_compare<"<">(h.damage, 10000.0))};
  }

  struct pack_hitResult { List<Int> Result = List<Int>{}; };
  pack_hitResult pack_hit(const net_Hit& h) {
    List<Int> w = List<Int>{};
    w = List<Int>{};
    wire::put_u8(w, h.target);
    wire::put_u8(w, h.by);
    wire::put_u8(w, h.weapon);
    wire::put_u8(w, h.zone);
    wire::put_f32(w, h.damage);
    wire::put_vec3(w, h.from);
    return {w};
  }

  struct unpack_hitResult { Bool Result = false; };
  unpack_hitResult unpack_hit(const List<Int>& b, net_Hit& h) {
    wire_Reader r = wire_Reader{0, true};
    r = wire_Reader{0, true};
    n28 = wire::Reader_get_u8(r, b);
    h.target = n28.Result;
    n29 = wire::Reader_get_u8(r, b);
    h.by = n29.Result;
    n30 = wire::Reader_get_u8(r, b);
    h.weapon = n30.Result;
    n31 = wire::Reader_get_u8(r, b);
    h.zone = n31.Result;
    n32 = wire::Reader_get_f32(r, b);
    h.damage = n32.Result;
    n33 = wire::Reader_get_vec3(r, b);
    h.from = n33.Result;
    return {logic_and(logic_and(logic_and(logic_and(r.ok, math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, h.damage), 0.0)), math_compare<">">(h.damage, 0.0)), math_compare<"<=">(h.damage, 100.0)), sane(h.from).Result)};
  }

  struct pack_fxResult { List<Int> Result = List<Int>{}; };
  pack_fxResult pack_fx(const net_Fx& f) {
    List<Int> w = List<Int>{};
    w = List<Int>{};
    wire::put_u8(w, f.kind);
    wire::put_u8(w, f.owner);
    wire::put_vec3(w, f.p);
    wire::put_vec3(w, f.dir);
    wire::put_f32(w, f.a);
    return {w};
  }

  struct unpack_fxResult { Bool Result = false; };
  unpack_fxResult unpack_fx(const List<Int>& b, net_Fx& f) {
    wire_Reader r = wire_Reader{0, true};
    r = wire_Reader{0, true};
    n34 = wire::Reader_get_u8(r, b);
    f.kind = n34.Result;
    n35 = wire::Reader_get_u8(r, b);
    f.owner = n35.Result;
    n36 = wire::Reader_get_vec3(r, b);
    f.p = n36.Result;
    n37 = wire::Reader_get_vec3(r, b);
    f.dir = n37.Result;
    n38 = wire::Reader_get_f32(r, b);
    f.a = n38.Result;
    return {logic_and(logic_and(logic_and(logic_and(r.ok, math_compare<"<=">(kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::melee)))), sane(f.p).Result), sane(f.dir).Result), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, f.a), 0.0))};
  }

  struct packResult { List<Int> Result = List<Int>{}; };
  packResult pack(const net_PlayerState& s) {
    List<Int> w = List<Int>{};
    Int v = 0;
    w = List<Int>{};
    wire::put_u8(w, s.slot);
    wire::put_u8(w, enum_index<net_PlayerMode>(s.mode));
    wire::put_u8(w, s.color);
    wire::put_u16(w, s.flags);
    wire::put_u32(w, s.t_ms);
    wire::put_vec3(w, s.position);
    wire::put_vec3(w, s.velocity);
    wire::put_quat(w, s.rotation);
    wire::put_vec3(w, s.avatar);
    wire::put_f32(w, s.avatar_yaw);
    if (math_compare<">">(kern::formula([](Float flags, Float a) -> Float { return (kern::math::band(flags, a) != 0.0); }, kern::to<Float>(s.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDroneOut)))), 0.5)) {
      wire::put_vec3(w, s.drone_pos);
      wire::put_quat(w, s.drone_rot);
    }
    if (math_compare<">">(kern::formula([](Float flags, Float a) -> Float { return (kern::math::band(flags, a) != 0.0); }, kern::to<Float>(s.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5)) {
      wire::put_vec3(w, s.jet_pos);
      wire::put_quat(w, s.jet_rot);
    }
    wire::put_f32(w, s.yaw);
    wire::put_f32(w, s.pitch);
    wire::put_f32(w, s.speed);
    wire::put_f32(w, s.health);
    wire::put_f32(w, s.thrust);
    wire::put_u8(w, s.stars);
    wire::put_u8(w, s.weapon);
    wire::put_f32(w, s.day);
    wire::put_u8(w, s.car);
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(s.hand))))); it39.next();) {
      v = s.hand[n39.index];
      wire::put_i8(w, v);
    }
    wire::put_u8(w, s.hands);
    return {w};
  }

  struct unpackResult { Bool Result = false; };
  unpackResult unpack(const List<Int>& b, Int& at, net_PlayerState& s) {
    wire_Reader r = wire_Reader{0, true};
    Int mode = 0;
    r = struct_make<wire_Reader>(at, true);
    n40 = wire::Reader_get_u8(r, b);
    s.slot = n40.Result;
    n41 = wire::Reader_get_u8(r, b);
    mode = n41.Result;
    n42 = wire::Reader_get_u8(r, b);
    s.color = n42.Result;
    n43 = wire::Reader_get_u16(r, b);
    s.flags = n43.Result;
    n44 = wire::Reader_get_u32(r, b);
    s.t_ms = n44.Result;
    n45 = wire::Reader_get_vec3(r, b);
    s.position = n45.Result;
    n46 = wire::Reader_get_vec3(r, b);
    s.velocity = n46.Result;
    n47 = wire::Reader_get_quat(r, b);
    s.rotation = n47.Result;
    n48 = wire::Reader_get_vec3(r, b);
    s.avatar = n48.Result;
    n49 = wire::Reader_get_f32(r, b);
    s.avatar_yaw = n49.Result;
    if (math_compare<">">(kern::formula([](Float flags, Float a) -> Float { return (kern::math::band(flags, a) != 0.0); }, kern::to<Float>(s.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDroneOut)))), 0.5)) {
      n50 = wire::Reader_get_vec3(r, b);
      s.drone_pos = n50.Result;
      n51 = wire::Reader_get_quat(r, b);
      s.drone_rot = n51.Result;
    }
    if (math_compare<">">(kern::formula([](Float flags, Float a) -> Float { return (kern::math::band(flags, a) != 0.0); }, kern::to<Float>(s.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5)) {
      n52 = wire::Reader_get_vec3(r, b);
      s.jet_pos = n52.Result;
      n53 = wire::Reader_get_quat(r, b);
      s.jet_rot = n53.Result;
    }
    n54 = wire::Reader_get_f32(r, b);
    s.yaw = n54.Result;
    n55 = wire::Reader_get_f32(r, b);
    s.pitch = n55.Result;
    n56 = wire::Reader_get_f32(r, b);
    s.speed = n56.Result;
    n57 = wire::Reader_get_f32(r, b);
    s.health = n57.Result;
    n58 = wire::Reader_get_f32(r, b);
    s.thrust = n58.Result;
    n59 = wire::Reader_get_u8(r, b);
    s.stars = n59.Result;
    n60 = wire::Reader_get_u8(r, b);
    s.weapon = n60.Result;
    n61 = wire::Reader_get_f32(r, b);
    s.day = n61.Result;
    n62 = wire::Reader_get_u8(r, b);
    s.car = n62.Result;
    for (auto it63 = flow_for(n63, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(s.hand))))); it63.next();) {
      n64 = wire::Reader_get_i8(r, b);
      s.hand[n63.index] = n64.Result;
    }
    n65 = wire::Reader_get_u8(r, b);
    s.hands = n65.Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_or(logic_not(r.ok), math_compare<">">(kern::to<Float>(mode), kern::to<Float>(enum_index<net_PlayerMode>(net_PlayerMode::parachute)))), logic_not(sane(s.position).Result)), logic_not(sane(s.velocity).Result)), logic_not(sane(s.avatar).Result)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, s.health), 0.0)))) {
      return {false};
    }
    s.mode = enum_from_index<net_PlayerMode>(mode).value;
    s.health = kern::formula([](Float health) -> Float { return (health < 0.0 ? 0.0 : (100.0 < health ? 100.0 : health)); }, s.health);
    at = r.at;
    return {true};
  }

  void Session_stop(net_Session& self) {
    Int old = 0;
    Int old_2 = 0;
    if (math_compare<"!=">(kern::to<Float>(self.client_), 0.0)) {
      engine_client_disconnect(self.client_);
    }
    if (math_compare<"!=">(kern::to<Float>(self.server_), 0.0)) {
      engine_server_stop(self.server_);
    }
    old = self.client_;
    self.client_ = 0;
    if (math_compare<"!=">(kern::to<Float>(old), 0.0)) {
      engine_client_release(old);
    }
    old_2 = self.server_;
    self.server_ = 0;
    if (math_compare<"!=">(kern::to<Float>(old_2), 0.0)) {
      engine_server_release(old_2);
    }
    list_clear(self.remotes_);
    list_clear(self.swarm_parts_);
    list_clear(self.protected_);
    self.remote_ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending);
    self.ballot_ = self.remote_ballot_;
    self.role_ = net_Session_Role::none;
    self.ever_in_ = false;
    list_clear(self.mates_);
    self.have_world_rules_ = false;
  }

  struct Session_joinResult { Bool Result = false; };
  Session_joinResult Session_join(net_Session& self, Int64 self_2 = Int64{0x0}, Int64 server = Int64{0x0}) {
    Int old = 0;
    Int Client = 0;
    Session_stop(self);
    engine_host_transport_set_address(self.transport_, struct_make<net_Address>(self_2));
    old = self.client_;
    engine_client_make(n67, self.transport_, config().Result, int64_xor(self_2, Int64{0x2545f4914f6cdd1d}));
    Client = n67.result;
    self.client_ = Client;
    if (math_compare<"!=">(kern::to<Float>(old), 0.0)) {
      engine_client_release(old);
    }
    self.server_addr_ = struct_make<net_Address>(server);
    self.connect_pending_ = true;
    self.role_ = net_Session_Role::client;
    self.have_origin_ = false;
    self.ever_in_ = false;
    mp_rules::Scoreboard_clear(self.scores_);
    return {true};
  }

  struct Session_connectedResult { Bool Result = false; };
  KERN_PURE Session_connectedResult Session_connected(const net_Session& self) {
    Bool running = false;
    Bool running_2 = false;
    Bool both = false;
    Bool connected = false;
    Bool connected_2 = false;
    Bool both_2 = false;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      both = math_compare<"!=">(kern::to<Float>(self.server_), 0.0);
      if (both) {
        running_2 = engine_server_running(self.server_);
        both = running_2;
      }
      return {both};
    }
    both_2 = math_compare<"!=">(kern::to<Float>(self.client_), 0.0);
    if (both_2) {
      connected_2 = engine_client_connected(self.client_);
      both_2 = connected_2;
    }
    return {logic_and(both_2, self.have_origin_)};
  }

  struct Session_remote_indexResult { Int Result = 0; };
  KERN_PURE Session_remote_indexResult Session_remote_index(const net_Session& self, Int slot = 0) {
    for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it68.next();) {
      if (math_compare<"==">(kern::to<Float>(self.remotes_[n68.index].state.slot), kern::to<Float>(slot))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n68.index)))};
      }
    }
    return {-1};
  }

  struct Session_host_lostResult { Bool Result = false; };
  KERN_PURE Session_host_lostResult Session_host_lost(const net_Session& self) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool either = false;
    if (logic_or(logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client)), logic_not(self.ever_in_))) {
      return {false};
    }
    either = logic_not(math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (logic_not(either)) {
      connected_2 = engine_client_connected(self.client_);
      either = logic_not(connected_2);
    }
    if (either) {
      return {true};
    }
    return {logic_and(math_compare<">">(kern::formula([](Float the_result, Float in_since_) -> Float { return (the_result - in_since_); }, net_session::Session_now_ms(self).Result, self.in_since_), kern::formula([](Float kQuietS) -> Float { return ((kQuietS * 1000.0) + 2000.0); }, kQuietS)), math_compare<"<">(kern::to<Float>(Session_remote_index(self, kHost).Result), 0.0))};
  }

  struct Session_successorResult { Int Result = 0; };
  KERN_PURE Session_successorResult Session_successor(const net_Session& self) {
    Int best = 0;
    Int s = 0;
    best = self.self_slot_;
    for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.mates_))))); it69.next();) {
      s = self.mates_[n69.index];
      if (logic_and(math_compare<"!=">(kern::to<Float>(s), kern::to<Float>(kHost)), math_compare<"<">(kern::to<Float>(s), kern::to<Float>(best)))) {
        best = s;
      }
    }
    return {best};
  }

  /// A message that was not taken (too large for its channel, the send queue full) is counted and said once per kind: a list that silently never arrives is a world that silently differs.
  void Session_refused(net_Session& self, Int type = 0, Int bytes = 0) {
    self.send_refused_ = kern::to<Int>(kern::formula([](Float send_refused_) -> Float { return kern::math::wrap32(send_refused_ + 1.0); }, kern::to<Float>(self.send_refused_)));
    if (math_compare<"<">(kern::to<Float>(self.refused_said_), 8.0)) {
      self.refused_said_ = kern::to<Int>(kern::formula([](Float refused_said_) -> Float { return kern::math::wrap32(refused_said_ + 1.0); }, kern::to<Float>(self.refused_said_)));
      debug_print(text_format<"[spawncity net] message type %u (%zu bytes) was not sent">(-1, type, bytes), false);
    }
  }

  void Session_to_all(net_Session& self, const List<Int>& data, Int ch = 0, Int type = 0, Int except = 0) {
    Int c = 0;
    Int max_clients = 0;
    Bool client_connected = false;
    Bool client_connected_2 = false;
    Bool both = false;
    Bool send = false;
    Bool go = false;
    if (logic_or(logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)), logic_not(math_compare<"!=">(kern::to<Float>(self.server_), 0.0)))) {
      return;
    }
    c = 0;
    go = true;
    while (loop(go)) {
      max_clients = engine_server_max_clients(self.server_);
      if (math_compare<"<">(kern::to<Float>(c), kern::to<Float>(max_clients))) {
        both = math_compare<"!=">(kern::to<Float>(c), kern::to<Float>(except));
        if (both) {
          client_connected_2 = engine_server_client_connected(self.server_, c);
          both = client_connected_2;
        }
        if (both) {
          engine_server_send(n70, self.server_, c, ch, type, data);
          send = n70.result;
          if (logic_not(send)) {
            Session_refused(self, type, list_length<Int>(data));
          }
        }
        c += 1;
      } else {
        go = false;
      }
    }
  }

  /// The host moves the city (a new start place, his GPS position): every client rebases to it.
  void Session_set_origin(net_Session& self, Float lon = 0.0, Float lat = 0.0) {
    List<Int> w = List<Int>{};
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
      return;
    }
    self.lon_ = lon;
    self.lat_ = lat;
    self.have_origin_ = true;
    w = List<Int>{};
    wire::put_f64(w, self.lon_);
    wire::put_f64(w, self.lat_);
    Session_to_all(self, w, 0, 28, kHost);
  }

  struct Session_originResult { Bool Result = false; };
  Session_originResult Session_origin(const net_Session& self, Float& lon, Float& lat) {
    if (logic_not(self.have_origin_)) {
      return {false};
    }
    lon = self.lon_;
    lat = self.lat_;
    return {true};
  }

  struct Session_rosterResult { List<Int> Result = List<Int>{}; };
  KERN_PURE Session_rosterResult Session_roster(const net_Session& self) {
    List<Int> r = List<Int>{};
    Int kept = 0;
    Int count = 0;
    r = list_make<Int, "1">(self.self_slot_);
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it71.next();) {
      list_add(r, self.remotes_[n71.index].state.slot);
    }
    list_sort<nullptr>(r);
    count = list_length<Int>(r);
    if (math_compare<">">(kern::to<Float>(count), 0.0)) {
      kept = 0;
      for (auto it72 = flow_for(n72, 1, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it72.next();) {
        if (logic_not(math_compare<"==">(kern::to<Float>(r[kept]), kern::to<Float>(r[n72.index])))) {
          kept += 1;
          r[kept] = r[n72.index];
        }
      }
      list_resize(r, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(kept))), 0);
    }
    return {r};
  }

  struct index_of_matchResult { Bool Result = false; };
  KERN_PURE index_of_matchResult index_of_match(Int s = 0) {
    return {math_compare<"==">(kern::to<Float>(s), kern::to<Float>(kHost))};
  }

  struct Session_index_ofResult { Int Result = 0; };
  KERN_PURE Session_index_ofResult Session_index_of(const net_Session& self, Int slot = 0) {
    List<Int> r = List<Int>{};
    Int at = 0;
    Int end = 0;
    Bool go = false;
    Int kept = 0;
    List<Int> rest = List<Int>{};
    r = Session_roster(self).Result;
    at = 0;
    end = list_length<Int>(r);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (index_of_match(r[at]).Result) {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      } else {
        go = false;
      }
    }
    if (math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end))) {
      rest = List<Int>{};
      list_reserve(rest, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(end), kern::to<Float>(at))));
      var_take(n74, r[at]);
      list_add(rest, n74.value);
      kept = at;
      at += 1;
      while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
        if (index_of_match(r[at]).Result) {
          var_take(n75, r[at]);
          r[kept] = n75.value;
          kept += 1;
        } else {
          var_take(n76, r[at]);
          list_add(rest, n76.value);
        }
        at += 1;
      }
      for (auto it78 = flow_for(n78, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(rest))))); it78.next();) {
        var_take(n77, rest[n78.index]);
        r[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(kept), kern::to<Float>(n78.index)))] = n77.value;
      }
    }
    for (auto it79 = flow_for(n79, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(r))))); it79.next();) {
      if (math_compare<"==">(kern::to<Float>(r[n79.index]), kern::to<Float>(slot))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n79.index)))};
      }
    }
    return {0};
  }

  struct Session_protected_nowResult { Bool Result = false; };
  KERN_PURE Session_protected_nowResult Session_protected_now(const net_Session& self, Int slot = 0, Float now = 0.0) {
    for (auto it80 = flow_for(n80, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.protected_))))); it80.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.protected_[n80.index].first), kern::to<Float>(slot)), math_compare<"<">(now, self.protected_[n80.index].second))) {
        return {true};
      }
    }
    return {false};
  }

  struct Session_focus_ofResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Session_focus_ofResult Session_focus_of(const net_Session& self, Int slot = 0) {
    Int at = 0;
    if (math_compare<"==">(kern::to<Float>(slot), kern::to<Float>(self.self_slot_))) {
      return {logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(self.me_.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(self.me_.mode, net_PlayerMode::drone)), self.me_.avatar, self.me_.position)};
    }
    at = Session_remote_index(self, slot).Result;
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      return {Vector{0.0, 0.0, 0.0}};
    }
    return {logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(self.remotes_[at].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(self.remotes_[at].state.mode, net_PlayerMode::drone)), self.remotes_[at].person_shown, self.remotes_[at].shown)};
  }

  struct Session_hp_ofResult { Float Result = 0.0; };
  KERN_PURE Session_hp_ofResult Session_hp_of(const net_Session& self, Int slot = 0) {
    Int at = 0;
    if (math_compare<"==">(kern::to<Float>(slot), kern::to<Float>(self.self_slot_))) {
      return {kern::formula([](Float flags, Float a, Float health) -> Float { return ((kern::math::band(flags, a) != 0.0) ? 0.0 : health); }, kern::to<Float>(self.me_.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), self.me_.health)};
    }
    at = Session_remote_index(self, slot).Result;
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      return {0.0};
    }
    return {kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 0.0 : a_3); }, kern::to<Float>(self.remotes_[at].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), self.remotes_[at].state.health)};
  }

  void Session_to_client(net_Session& self, const List<Int>& data, Int slot = 0, Int ch = 0, Int type = 0) {
    Int max_clients = 0;
    Int max_clients_2 = 0;
    Bool both = false;
    Bool client_connected = false;
    Bool client_connected_2 = false;
    Bool both_2 = false;
    Bool send = false;
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host), math_compare<"!=">(kern::to<Float>(self.server_), 0.0));
    if (both) {
      max_clients_2 = engine_server_max_clients(self.server_);
      both = math_compare<"<">(kern::to<Float>(slot), kern::to<Float>(max_clients_2));
    }
    both_2 = both;
    if (both_2) {
      client_connected_2 = engine_server_client_connected(self.server_, slot);
      both_2 = client_connected_2;
    }
    if (both_2) {
      engine_server_send(n81, self.server_, slot, ch, type, data);
      send = n81.result;
      if (logic_not(send)) {
        Session_refused(self, type, list_length<Int>(data));
      }
    }
  }

  void Session_send_seats(net_Session& self) {
    List<Int> w = List<Int>{};
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
      return;
    }
    w = List<Int>{};
    wire::put_u32(w, jet_seats::Table_revision(self.seats_).Result);
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<seats_Row>(self.seats_.rows_)))));
    for (auto it88 = flow_for(n88, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<seats_Row>(self.seats_.rows_))))); it88.next();) {
      wire::put_u8(w, self.seats_.rows_[n88.index].owner);
      wire::put_u8(w, self.seats_.rows_[n88.index].pilot);
      wire::put_u8(w, self.seats_.rows_[n88.index].passenger);
    }
    Session_to_all(self, w, 0, 24, kNobody);
  }

  void Session_judge_seat(net_Session& self, Int from = 0, Bool claim = false, Int owner = 0, seats_Seat seat = seats_Seat::pilot) {
    the_seats_Result_ r = the_seats_Result_::ok;
    the_seats_Result_ chosen = the_seats_Result_::ok;
    if (claim) {
      n86 = jet_seats_cpp::Table_claim(self.seats_, owner, from, seat);
      chosen = n86.Result;
    } else {
      n87 = jet_seats_cpp::Table_release(self.seats_, owner, from, seat);
      chosen = n87.Result;
    }
    r = chosen;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      Session_send_seats(self);
    }
  }

  /// A part of somebody's snapshot came: the snapshot is handed on (take_swarms) once every part of it is here - a snapshot with a part lost on the way is dropped whole (the next one comes 120 ms later), never shown with units missing.
  void Session_swarm_part(net_Session& self, List<net_SwarmUnit>& units, Int owner = 0, Int serial = 0, Int part = 0, Bool last = false) {
    Int at = 0;
    Int64 mark = Int64{0x0};
    Int place = 0;
    at = -1;
    for (auto it93 = flow_for(n93, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Session_SwarmParts>(self.swarm_parts_))))); it93.next();) {
      if (math_compare<"==">(kern::to<Float>(self.swarm_parts_[n93.index].owner), kern::to<Float>(owner))) {
        at = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n93.index)));
      }
    }
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      list_add(self.swarm_parts_, struct_make<net_Session_SwarmParts>(0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{}));
      at = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 1.0); }, kern::to<Float>(list_length<net_Session_SwarmParts>(self.swarm_parts_))));
      self.swarm_parts_[at].owner = owner;
      self.swarm_parts_[at].serial = kern::to<Int>(kern::formula([](Float serial) -> Float { return kern::math::band((serial - 1.0), 255.0); }, kern::to<Float>(serial)));
    }
    if (math_compare<"!=">(kern::to<Float>(self.swarm_parts_[at].serial), kern::to<Float>(serial))) {
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float serial, Float a) -> Float { return (kern::math::bxor(kern::math::band((serial - a), 255.0), 128.0) - 128.0); }, kern::to<Float>(serial), kern::to<Float>(self.swarm_parts_[at].serial))))), 0.0)) {
        return;
      }
      self.swarm_parts_[at] = struct_make<net_Session_SwarmParts>(0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{});
      self.swarm_parts_[at].owner = owner;
      self.swarm_parts_[at].serial = serial;
    }
    mark = int64_shift_left(Int64{0x1}, kern::to<Int>(kern::formula([](Float part) -> Float { return kern::math::band(part, 63.0); }, kern::to<Float>(part))));
    if (logic_not(int64_equal(int64_and(self.swarm_parts_[at].have[kern::to<Int>(kern::formula([](Float part) -> Float { return kern::math::shr(part, 6.0); }, kern::to<Float>(part)))], mark), Int64{0x0}))) {
      return;
    }
    self.swarm_parts_[at].have[kern::to<Int>(kern::formula([](Float part) -> Float { return kern::math::shr(part, 6.0); }, kern::to<Float>(part)))] = int64_or(self.swarm_parts_[at].have[kern::to<Int>(kern::formula([](Float part) -> Float { return kern::math::shr(part, 6.0); }, kern::to<Float>(part)))], mark);
    place = list_length<net_SwarmUnit>(self.swarm_parts_[at].units);
    list_insert_list(self.swarm_parts_[at].units, place, units);
    if (last) {
      self.swarm_parts_[at].last = part;
    }
    if (math_compare<"<">(kern::to<Float>(self.swarm_parts_[at].last), 0.0)) {
      return;
    }
    for (auto it94 = flow_for(n94, 0, self.swarm_parts_[at].last); it94.next();) {
      if (logic_not(logic_not(int64_equal(int64_and(self.swarm_parts_[at].have[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 6.0); }, kern::to<Float>(n94.index)))], int64_shift_left(Int64{0x1}, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(index, 63.0); }, kern::to<Float>(n94.index))))), Int64{0x0})))) {
        return;
      }
    }
    var_take(n95, self.swarm_parts_[at].units);
    list_add(self.swarms_, struct_make<net_Swarm>(owner, n95.value));
    list_clear(self.swarm_parts_[at].units);
    self.swarm_parts_[at].last = -1;
  }

  void Session_town_to_host(net_Session& self, const List<Int>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    Bool send_2 = false;
    Bool both_2 = false;
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    both_2 = both;
    if (both_2) {
      engine_client_send(n102, self.client_, 0, 32, blob);
      send_2 = n102.result;
      both_2 = logic_not(send_2);
    }
    if (both_2) {
      Session_refused(self, 32, list_length<Int>(blob));
    }
  }

  void Session_town_to_all(net_Session& self, const List<Int>& blob) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, 0, 32, kHost);
    }
  }

  void Session_town_to(net_Session& self, const List<Int>& blob, Int slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 0, 32);
    }
  }

  void Session_send_world(net_Session& self, const List<Int>& blob) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, 1, 27, kHost);
    }
  }

  void Session_send_world_to(net_Session& self, const List<Int>& blob, Int slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 1, 27);
    }
  }

  /// Destruction sync (game_destruction_sync.cpp): reliable, the host is the authority.
  void Session_send_proj(net_Session& self, const List<Int>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    List<Int> w = List<Int>{};
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    if (both) {
      engine_client_send(n103, self.client_, 1, 31, blob);
      send = n103.result;
    } else if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      w = List<Int>{};
      wire::put_u8(w, self.self_slot_);
      wire::put_bytes(w, blob);
      Session_to_all(self, w, 1, 31, kHost);
    }
  }

  void Session_send_toss(net_Session& self, const List<Int>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    List<Int> w = List<Int>{};
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    if (both) {
      engine_client_send(n104, self.client_, 1, 30, blob);
      send = n104.result;
    } else if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      w = List<Int>{};
      wire::put_u8(w, self.self_slot_);
      wire::put_bytes(w, blob);
      Session_to_all(self, w, 1, 30, kHost);
    }
  }

  void Session_destroy_to_host(net_Session& self, const List<Int>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    if (both) {
      engine_client_send(n105, self.client_, 0, 29, blob);
      send = n105.result;
    }
  }

  void Session_destroy_to_all(net_Session& self, const List<Int>& blob, Bool reliable = false) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, kern::to<Int>(kern::formula([](Float reliable) -> Float { return ((reliable != 0.0) ? 0.0 : 1.0); }, kern::to<Float>(reliable))), 29, kHost);
    }
  }

  void Session_destroy_to(net_Session& self, const List<Int>& blob, Int slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 0, 29);
    }
  }

  void Session_send_scores(net_Session& self) {
    List<Int> w = List<Int>{};
    w = List<Int>{};
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<mp_Score>(self.scores_.rows_)))));
    for (auto it106 = flow_for(n106, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.scores_.rows_))))); it106.next();) {
      wire::put_u8(w, self.scores_.rows_[n106.index].slot);
      wire::put_u16(w, self.scores_.rows_[n106.index].kills);
      wire::put_u16(w, self.scores_.rows_[n106.index].deaths);
    }
    Session_to_all(self, w, 0, 11, kNobody);
  }

  void Session_send_rules(net_Session& self) {
    List<Int> w = List<Int>{};
    w = List<Int>{};
    wire::put_u8(w, kern::to<Int>(self.friendly_fire_));
    if (self.have_world_rules_) {
      wire::put_u16(w, self.world_rules_);
    }
    Session_to_all(self, w, 0, 17, kNobody);
  }

  void Session_set_world_rules(net_Session& self, Int mask = 0) {
    Bool changed = false;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::client)) {
      return;
    }
    changed = logic_or(logic_not(self.have_world_rules_), math_compare<"!=">(kern::to<Float>(mask), kern::to<Float>(self.world_rules_)));
    self.world_rules_ = mask;
    self.have_world_rules_ = true;
    if (logic_and(changed, enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
      Session_send_rules(self);
    }
  }

  void Session_finish_ballot(net_Session& self, mp_Ballot_Status s = mp_Ballot_Status::pending) {
    Int result = 0;
    List<Int> w = List<Int>{};
    result = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : ((a_2 != 0.0) ? 2.0 : 3.0)), 255.0); }, kern::to<Float>(enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::approved)), kern::to<Float>(enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::rejected))));
    if (enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::approved)) {
      self.friendly_fire_ = self.ballot_.value;
      Session_send_rules(self);
    }
    w = List<Int>{};
    wire::put_u32(w, self.ballot_.id);
    wire::put_u8(w, result);
    Session_to_all(self, w, 0, 21, kNobody);
    self.vote_text_ = logic_select<Text>(math_compare<"==">(kern::to<Float>(result), 1.0), "Einstimmig angenommen", logic_select<Text>(math_compare<"==">(kern::to<Float>(result), 2.0), "Abgelehnt", "Abstimmung abgelaufen / Spielerwechsel"));
    self.ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending);
  }

  struct Session_statusResult { Text Result = ""; };
  KERN_PURE Session_statusResult Session_status(const net_Session& self) {
    Text b = "";
    Int connected_clients = 0;
    Int connected_clients_2 = 0;
    Int chosen = 0;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return {""};
    }
    b = "";
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      if (math_compare<"!=">(kern::to<Float>(self.server_), 0.0)) {
        connected_clients_2 = engine_server_connected_clients(self.server_);
        chosen = connected_clients_2;
      } else {
        chosen = 0;
      }
      b = text_format<"Host - %d Spieler">(95, kern::to<Int>(kern::formula([](Float chosen) -> Float { return (1.0 + chosen); }, kern::to<Float>(chosen))));
    } else {
      b = text_format<"%s - %zu weitere Spieler">(95, logic_select<Text>(Session_connected(self).Result, "Verbunden", "Verbinde ..."), list_length<net_Remote>(self.remotes_));
    }
    return {b};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  wire::Reader_get_u8Result n11;
  wire::Reader_get_u8Result n12;
  wire::Reader_get_u8Result n13;
  wire::Reader_get_u8Result n14;
  wire::Reader_get_u16Result n15;
  wire::Reader_get_u8Result n16;
  wire::Reader_get_u8Result n17;
  wire::Reader_get_f32Result n18;
  wire::Reader_get_f32Result n19;
  wire::Reader_get_i16Result n20;
  wire::Reader_get_i16Result n21;
  wire::Reader_get_i16Result n22;
  wire::Reader_get_u8Result n23;
  wire::Reader_get_u8Result n24;
  wire::Reader_get_u16Result n25;
  wire::Reader_get_u8Result n26;
  wire::Reader_get_f32Result n27;
  wire::Reader_get_u8Result n28;
  wire::Reader_get_u8Result n29;
  wire::Reader_get_u8Result n30;
  wire::Reader_get_u8Result n31;
  wire::Reader_get_f32Result n32;
  wire::Reader_get_vec3Result n33;
  wire::Reader_get_u8Result n34;
  wire::Reader_get_u8Result n35;
  wire::Reader_get_vec3Result n36;
  wire::Reader_get_vec3Result n37;
  wire::Reader_get_f32Result n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  wire::Reader_get_u8Result n40;
  wire::Reader_get_u8Result n41;
  wire::Reader_get_u8Result n42;
  wire::Reader_get_u16Result n43;
  wire::Reader_get_u32Result n44;
  wire::Reader_get_vec3Result n45;
  wire::Reader_get_vec3Result n46;
  wire::Reader_get_quatResult n47;
  wire::Reader_get_vec3Result n48;
  wire::Reader_get_f32Result n49;
  wire::Reader_get_vec3Result n50;
  wire::Reader_get_quatResult n51;
  wire::Reader_get_vec3Result n52;
  wire::Reader_get_quatResult n53;
  wire::Reader_get_f32Result n54;
  wire::Reader_get_f32Result n55;
  wire::Reader_get_f32Result n56;
  wire::Reader_get_f32Result n57;
  wire::Reader_get_f32Result n58;
  wire::Reader_get_u8Result n59;
  wire::Reader_get_u8Result n60;
  wire::Reader_get_f32Result n61;
  wire::Reader_get_u8Result n62;
  struct : kern::Node<63> { Int index = 0; } n63;
  wire::Reader_get_i8Result n64;
  wire::Reader_get_u8Result n65;
  struct { Int result = 0; } n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct { Bool result = false; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct : kern::Node<72> { Int index = 0; } n72;
  struct { Int value = 0; } n74;
  struct { Int value = 0; } n75;
  struct { Int value = 0; } n76;
  struct { Int value = 0; } n77;
  struct : kern::Node<78> { Int index = 0; } n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<80> { Int index = 0; } n80;
  struct { Bool result = false; } n81;
  jet_seats_cpp::Table_claimResult n86;
  jet_seats_cpp::Table_releaseResult n87;
  struct : kern::Node<88> { Int index = 0; } n88;
  struct : kern::Node<93> { Int index = 0; } n93;
  struct : kern::Node<94> { Int index = 0; } n94;
  struct { List<net_SwarmUnit> value = List<net_SwarmUnit>{}; } n95;
  struct { Bool result = false; } n102;
  struct { Bool result = false; } n103;
  struct { Bool result = false; } n104;
  struct { Bool result = false; } n105;
  struct : kern::Node<106> { Int index = 0; } n106;
};
