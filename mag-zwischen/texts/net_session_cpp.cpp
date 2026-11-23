struct net_session_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual engine_net_client_server, virtual jet_seats, virtual jet_seats_cpp, virtual mp_rules, virtual mp_rules_cpp, virtual net_session, virtual wire {
  // ---- variables ----
  /// ---- players -----------------------------------------------------------------------------------
  KERN_CONST U32 kMaxPlayers = 9;
  /// units in one message (18 bytes each, under the 1200 of an unreliable one)
  KERN_CONST Int kSwarmPerMessage = 64;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// the host's slot
  KERN_CONST U8 kHost = 255;
  /// a player silent this long is gone
  KERN_CONST Float kQuietS = 6.0;
  /// no shooter (an accident, the police, a fall)
  KERN_CONST U8 kNobody = 254;
  /// what one snapshot can carry in its numbered parts
  KERN_CONST Int kSwarmMaxUnits = 8128;
  /// vs_multiplayer.mjs: a jump > 35 m resets the timeline
  KERN_CONST Float kTeleportM = 35.0;
  /// lan_vs.mjs SEND_MS
  KERN_CONST Float kSendInterval = 0.033;
  /// the board is re-sent now and then (late joiners)
  KERN_CONST Float kScoresEvery = 2.0;
  /// game_rules_sync.mjs SEND_MS
  KERN_CONST Float kRulesEvery = 4.0;
  /// jet_seat_coordinator.mjs tick: the table again every 1.5 s
  KERN_CONST Float kSeatsEvery = 1.5;
  /// The number of each value of net_ClientState, in their order
  KERN_CONST List<Int> net_ClientState_numbers = List<Int>{-3, -2, -1, 0, 1, 2, 3};
  /// a client that is not in asks again this often
  KERN_CONST Float kReconnectS = 1.0;

  // ---- functions ----
  struct configResult { net_ClientServerConfig Result = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}}; };
  KERN_PURE configResult config() {
    net_ClientServerConfig c = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}};
    net_ChannelConfig reliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    net_ChannelConfig unreliable = net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1};
    c = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}};
    c.protocol_id = Int64{0x53504157534e3032};
    c.max_clients = kern::to<Int>(kern::formula([](Float kMaxPlayers) -> Float { return (kMaxPlayers - 1.0); }, kern::to<Float>(kMaxPlayers)));
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

  struct pack_swarm_q16Result { I16 Result = 0; };
  KERN_PURE pack_swarm_q16Result pack_swarm_q16(Float v = 0.0, Float k = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(a, 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v, Float k) -> Float { return kern::math::round((v * k)); }, v, k)))), -32767.0, 32767.0)))};
  }

  struct pack_swarmResult { List<U8> Result = List<U8>{}; };
  /// One part of a player's swarm snapshot: owner, the snapshot's serial, the part's number (bit 7: the last one), its units - 18 bytes a unit (64 fit in one unreliable message of 1200): z in dm, yaw and aux in 1/100. (`n` units of the list, from `from` on)
  pack_swarmResult pack_swarm(const List<net_SwarmUnit>& units, U8 owner = 0, U8 serial = 0, U8 part = 0, Bool last = false, Int from = 0, Int n = 0) {
    List<U8> w = List<U8>{};
    net_SwarmUnit u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    w = List<U8>{};
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
  unpack_swarmResult unpack_swarm(const List<U8>& b, U8& owner, U8& serial, U8& part, Bool& last, List<net_SwarmUnit>& units) {
    wire_Reader r = wire_Reader{0, true};
    U8 pt = 0;
    U8 n = 0;
    U8 i = 0;
    net_SwarmUnit u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    U8 k = 0;
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
      i += 1;
    }
    return {r.ok};
  }

  struct pack_swarm_hitResult { List<U8> Result = List<U8>{}; };
  pack_swarm_hitResult pack_swarm_hit(const net_SwarmHit& h) {
    List<U8> w = List<U8>{};
    w = List<U8>{};
    wire::put_u8(w, h.from);
    wire::put_u8(w, h.to);
    wire::put_u16(w, h.id);
    wire::put_u8(w, enum_index<net_SwarmKind>(h.kind));
    wire::put_f32(w, h.damage);
    return {w};
  }

  struct unpack_swarm_hitResult { Bool Result = false; };
  unpack_swarm_hitResult unpack_swarm_hit(const List<U8>& b, net_SwarmHit& h) {
    wire_Reader r = wire_Reader{0, true};
    U8 k = 0;
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

  struct pack_hitResult { List<U8> Result = List<U8>{}; };
  pack_hitResult pack_hit(const net_Hit& h) {
    List<U8> w = List<U8>{};
    w = List<U8>{};
    wire::put_u8(w, h.target);
    wire::put_u8(w, h.by);
    wire::put_u8(w, h.weapon);
    wire::put_u8(w, h.zone);
    wire::put_f32(w, h.damage);
    wire::put_vec3(w, h.from);
    return {w};
  }

  struct unpack_hitResult { Bool Result = false; };
  unpack_hitResult unpack_hit(const List<U8>& b, net_Hit& h) {
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

  struct pack_fxResult { List<U8> Result = List<U8>{}; };
  pack_fxResult pack_fx(const net_Fx& f) {
    List<U8> w = List<U8>{};
    w = List<U8>{};
    wire::put_u8(w, f.kind);
    wire::put_u8(w, f.owner);
    wire::put_vec3(w, f.p);
    wire::put_vec3(w, f.dir);
    wire::put_f32(w, f.a);
    return {w};
  }

  struct unpack_fxResult { Bool Result = false; };
  unpack_fxResult unpack_fx(const List<U8>& b, net_Fx& f) {
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

  struct packResult { List<U8> Result = List<U8>{}; };
  packResult pack(const net_PlayerState& s) {
    List<U8> w = List<U8>{};
    I8 v = 0;
    w = List<U8>{};
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
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I8>(s.hand))))); it39.next();) {
      v = s.hand[n39.index];
      wire::put_i8(w, v);
    }
    wire::put_u8(w, s.hands);
    return {w};
  }

  struct unpackResult { Bool Result = false; };
  unpackResult unpack(const List<U8>& b, Int& at, net_PlayerState& s) {
    wire_Reader r = wire_Reader{0, true};
    U8 mode = 0;
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
    for (auto it63 = flow_for(n63, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I8>(s.hand))))); it63.next();) {
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
    self.remote_ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending);
    self.ballot_ = self.remote_ballot_;
    self.role_ = net_Session_Role::none;
    self.ever_in_ = false;
    list_clear(self.mates_);
    self.have_world_rules_ = false;
  }

  struct Session_hostResult { Bool Result = false; };
  Session_hostResult Session_host(net_Session& self, Int64 self_2 = Int64{0x0}, Float lon = 0.0, Float lat = 0.0) {
    Int old = 0;
    Int Server = 0;
    Session_stop(self);
    engine_host_transport_set_address(self.transport_, struct_make<net_Address>(self_2));
    old = self.server_;
    engine_server_make(n66, self.transport_, config().Result, int64_xor(self_2, Int64{0x9e3779b97f4a7c15}));
    Server = n66.result;
    self.server_ = Server;
    if (math_compare<"!=">(kern::to<Float>(old), 0.0)) {
      engine_server_release(old);
    }
    engine_server_start(self.server_, kern::to<Int>(kern::formula([](Float kMaxPlayers) -> Float { return kern::math::wrap32s(kern::math::wrap32(kMaxPlayers - 1.0)); }, kern::to<Float>(kMaxPlayers))));
    self.role_ = net_Session_Role::host;
    self.self_slot_ = kHost;
    self.lon_ = lon;
    self.lat_ = lat;
    self.have_origin_ = true;
    mp_rules::Scoreboard_clear(self.scores_);
    mp_rules_cpp::Scoreboard_ensure(self.scores_, kHost);
    return {true};
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

  struct Session_remote_indexResult { I32 Result = 0; };
  KERN_PURE Session_remote_indexResult Session_remote_index(const net_Session& self, U8 slot = 0) {
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

  struct Session_successorResult { U8 Result = 0; };
  KERN_PURE Session_successorResult Session_successor(const net_Session& self) {
    U8 best = 0;
    U8 s = 0;
    best = self.self_slot_;
    for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(self.mates_))))); it69.next();) {
      s = self.mates_[n69.index];
      if (logic_and(math_compare<"!=">(kern::to<Float>(s), kern::to<Float>(kHost)), math_compare<"<">(kern::to<Float>(s), kern::to<Float>(best)))) {
        best = s;
      }
    }
    return {best};
  }

  /// A message that was not taken (too large for its channel, the send queue full) is counted and said once per kind: a list that silently never arrives is a world that silently differs.
  void Session_refused(net_Session& self, U16 type = 0, Int bytes = 0) {
    self.send_refused_ = kern::to<Int>(kern::formula([](Float send_refused_) -> Float { return (send_refused_ + 1.0); }, kern::to<Float>(self.send_refused_)));
    if (math_compare<"<">(kern::to<Float>(self.refused_said_), 8.0)) {
      self.refused_said_ = kern::to<Int>(kern::formula([](Float refused_said_) -> Float { return (refused_said_ + 1.0); }, kern::to<Float>(self.refused_said_)));
      debug_print(text_format<"[spawncity net] message type %u (%zu bytes) was not sent">(-1, type, bytes), false);
    }
  }

  void Session_to_all(net_Session& self, const List<U8>& data, I32 ch = 0, U16 type = 0, U8 except = 0) {
    I32 c = 0;
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
            Session_refused(self, type, list_length<U8>(data));
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
    List<U8> w = List<U8>{};
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
      return;
    }
    self.lon_ = lon;
    self.lat_ = lat;
    self.have_origin_ = true;
    w = List<U8>{};
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

  struct Session_rosterResult { List<U8> Result = List<U8>{}; };
  KERN_PURE Session_rosterResult Session_roster(const net_Session& self) {
    List<U8> r = List<U8>{};
    Int kept = 0;
    Int count = 0;
    r = list_make<U8, "1">(self.self_slot_);
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it71.next();) {
      list_add(r, self.remotes_[n71.index].state.slot);
    }
    list_sort<nullptr>(r);
    count = list_length<U8>(r);
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

  struct Session_color_ofResult { U32 Result = 0; };
  Session_color_ofResult Session_color_of(const net_Session& self, U8 slot = 0) {
    List<U8> r = List<U8>{};
    List<Text> ids = List<Text>{};
    U8 s = 0;
    List<U32> c = List<U32>{};
    Text a = "";
    r = Session_roster(self).Result;
    ids = List<Text>{};
    for (auto it73 = flow_for(n73, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(r))))); it73.next();) {
      s = r[n73.index];
      list_add(ids, text_format<"%d">(-1, s));
    }
    n74 = mp_rules_cpp::assign_colors(ids);
    c = n74.Result;
    for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8>(r))))); it75.next();) {
      if (math_compare<"==">(kern::to<Float>(r[n75.index]), kern::to<Float>(slot))) {
        return {c[n75.index]};
      }
    }
    a = text_format<"%d">(-1, slot);
    n76 = mp_rules_cpp::color_of(a);
    return {n76.Result};
  }

  struct index_of_matchResult { Bool Result = false; };
  KERN_PURE index_of_matchResult index_of_match(U8 s = 0) {
    return {math_compare<"==">(kern::to<Float>(s), kern::to<Float>(kHost))};
  }

  struct Session_index_ofResult { U32 Result = 0; };
  KERN_PURE Session_index_ofResult Session_index_of(const net_Session& self, U8 slot = 0) {
    List<U8> r = List<U8>{};
    Int at = 0;
    Int end = 0;
    Bool go = false;
    Int kept = 0;
    List<U8> rest = List<U8>{};
    r = Session_roster(self).Result;
    at = 0;
    end = list_length<U8>(r);
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
      rest = List<U8>{};
      list_reserve(rest, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(end), kern::to<Float>(at))));
      var_take(n77, r[at]);
      list_add(rest, n77.value);
      kept = at;
      at += 1;
      while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
        if (index_of_match(r[at]).Result) {
          var_take(n78, r[at]);
          r[kept] = n78.value;
          kept += 1;
        } else {
          var_take(n79, r[at]);
          list_add(rest, n79.value);
        }
        at += 1;
      }
      for (auto it81 = flow_for(n81, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(rest))))); it81.next();) {
        var_take(n80, rest[n81.index]);
        r[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(kept), kern::to<Float>(n81.index)))] = n80.value;
      }
    }
    for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8>(r))))); it82.next();) {
      if (math_compare<"==">(kern::to<Float>(r[n82.index]), kern::to<Float>(slot))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n82.index)))};
      }
    }
    return {0};
  }

  struct Session_protected_nowResult { Bool Result = false; };
  KERN_PURE Session_protected_nowResult Session_protected_now(const net_Session& self, U8 slot = 0, Float now = 0.0) {
    for (auto it83 = flow_for(n83, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.protected_))))); it83.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.protected_[n83.index].first), kern::to<Float>(slot)), math_compare<"<">(now, self.protected_[n83.index].second))) {
        return {true};
      }
    }
    return {false};
  }

  struct Session_focus_ofResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Session_focus_ofResult Session_focus_of(const net_Session& self, U8 slot = 0) {
    I32 at = 0;
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
  KERN_PURE Session_hp_ofResult Session_hp_of(const net_Session& self, U8 slot = 0) {
    I32 at = 0;
    if (math_compare<"==">(kern::to<Float>(slot), kern::to<Float>(self.self_slot_))) {
      return {kern::formula([](Float flags, Float a, Float health) -> Float { return ((kern::math::band(flags, a) != 0.0) ? 0.0 : health); }, kern::to<Float>(self.me_.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), self.me_.health)};
    }
    at = Session_remote_index(self, slot).Result;
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      return {0.0};
    }
    return {kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 0.0 : a_3); }, kern::to<Float>(self.remotes_[at].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), self.remotes_[at].state.health)};
  }

  void Session_to_client(net_Session& self, const List<U8>& data, U8 slot = 0, I32 ch = 0, U16 type = 0) {
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
      engine_server_send(n84, self.server_, slot, ch, type, data);
      send = n84.result;
      if (logic_not(send)) {
        Session_refused(self, type, list_length<U8>(data));
      }
    }
  }

  void Session_send_seats(net_Session& self) {
    List<U8> w = List<U8>{};
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
      return;
    }
    w = List<U8>{};
    wire::put_u32(w, jet_seats::Table_revision(self.seats_).Result);
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<seats_Row>(self.seats_.rows_)))));
    for (auto it91 = flow_for(n91, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<seats_Row>(self.seats_.rows_))))); it91.next();) {
      wire::put_u8(w, self.seats_.rows_[n91.index].owner);
      wire::put_u8(w, self.seats_.rows_[n91.index].pilot);
      wire::put_u8(w, self.seats_.rows_[n91.index].passenger);
    }
    Session_to_all(self, w, 0, 24, kNobody);
  }

  void Session_judge_seat(net_Session& self, U8 from = 0, Bool claim = false, U8 owner = 0, seats_Seat seat = seats_Seat::pilot) {
    the_seats_Result_ r = the_seats_Result_::ok;
    the_seats_Result_ chosen = the_seats_Result_::ok;
    if (claim) {
      n89 = jet_seats_cpp::Table_claim(self.seats_, owner, from, seat);
      chosen = n89.Result;
    } else {
      n90 = jet_seats_cpp::Table_release(self.seats_, owner, from, seat);
      chosen = n90.Result;
    }
    r = chosen;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      Session_send_seats(self);
    }
  }

  /// A part of somebody's snapshot came: the snapshot is handed on (take_swarms) once every part of it is here - a snapshot with a part lost on the way is dropped whole (the next one comes 120 ms later), never shown with units missing.
  void Session_swarm_part(net_Session& self, List<net_SwarmUnit>& units, U8 owner = 0, U8 serial = 0, U8 part = 0, Bool last = false) {
    I32 at = 0;
    Int64 mark = Int64{0x0};
    Int place = 0;
    at = -1;
    for (auto it96 = flow_for(n96, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Session_SwarmParts>(self.swarm_parts_))))); it96.next();) {
      if (math_compare<"==">(kern::to<Float>(self.swarm_parts_[n96.index].owner), kern::to<Float>(owner))) {
        at = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n96.index)));
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
    for (auto it97 = flow_for(n97, 0, self.swarm_parts_[at].last); it97.next();) {
      if (logic_not(logic_not(int64_equal(int64_and(self.swarm_parts_[at].have[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 6.0); }, kern::to<Float>(n97.index)))], int64_shift_left(Int64{0x1}, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(index, 63.0); }, kern::to<Float>(n97.index))))), Int64{0x0})))) {
        return;
      }
    }
    var_take(n98, self.swarm_parts_[at].units);
    list_add(self.swarms_, struct_make<net_Swarm>(owner, n98.value));
    list_clear(self.swarm_parts_[at].units);
    self.swarm_parts_[at].last = -1;
  }

  void Session_judge(net_Session& self, const net_Hit& req, U8 shooter = 0) {
    List<U8> r = List<U8>{};
    Bool target_known = false;
    Int value = 0;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    Bool shooter_known = false;
    Int value_2 = 0;
    Int at_2 = 0;
    Int end_2 = 0;
    Bool go_2 = false;
    Float until = 0.0;
    mp_HitVerdict v = mp_HitVerdict::applied;
    net_Hit h = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
    List<U8> bytes = List<U8>{};
    r = Session_roster(self).Result;
    value = req.target;
    at = 0;
    end = list_length<U8>(r);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (math_compare<"==">(kern::to<Float>(r[at]), kern::to<Float>(value))) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    target_known = logic_not(math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<U8>(r))));
    value_2 = shooter;
    at_2 = 0;
    end_2 = list_length<U8>(r);
    go_2 = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2));
    while (loop(go_2)) {
      if (math_compare<"==">(kern::to<Float>(r[at_2]), kern::to<Float>(value_2))) {
        go_2 = false;
      } else {
        at_2 += 1;
        go_2 = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2));
      }
    }
    shooter_known = logic_not(math_compare<"==">(kern::to<Float>(at_2), kern::to<Float>(list_length<U8>(r))));
    until = 0.0;
    for (auto it102 = flow_for(n102, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.protected_))))); it102.next();) {
      if (math_compare<"==">(kern::to<Float>(self.protected_[n102.index].first), kern::to<Float>(req.target))) {
        until = self.protected_[n102.index].second;
      }
    }
    n103 = mp_rules_cpp::judge_hit(target_known, shooter_known, math_compare<"==">(kern::to<Float>(req.target), kern::to<Float>(shooter)), self.friendly_fire_, net_session::Session_now_ms(self).Result, until, Session_hp_of(self, req.target).Result);
    v = n103.Result;
    if (logic_not(enum_equal<mp_HitVerdict>(v, mp_HitVerdict::applied))) {
      return;
    }
    h = req;
    h.by = shooter;
    n104 = pack_hit(h);
    bytes = n104.Result;
    if (math_compare<"==">(kern::to<Float>(h.target), kern::to<Float>(self.self_slot_))) {
      list_add(self.hits_, h);
    } else {
      Session_to_client(self, bytes, h.target, 0, 7);
    }
    if (math_compare<"!=">(kern::to<Float>(shooter), kern::to<Float>(h.target))) {
      if (math_compare<"==">(kern::to<Float>(shooter), kern::to<Float>(self.self_slot_))) {
        list_add(self.confirms_, h);
      } else {
        Session_to_client(self, bytes, shooter, 0, 8);
      }
    }
  }

  void Session_send_scores(net_Session& self) {
    List<U8> w = List<U8>{};
    w = List<U8>{};
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<mp_Score>(self.scores_.rows_)))));
    for (auto it108 = flow_for(n108, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.scores_.rows_))))); it108.next();) {
      wire::put_u8(w, self.scores_.rows_[n108.index].slot);
      wire::put_u16(w, self.scores_.rows_[n108.index].kills);
      wire::put_u16(w, self.scores_.rows_[n108.index].deaths);
    }
    Session_to_all(self, w, 0, 11, kNobody);
  }

  void Session_begin_ballot(net_Session& self, Bool value = false) {
    List<U8> w = List<U8>{};
    U8 m = 0;
    Text t = "";
    if (logic_or(logic_or(logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)), math_compare<"!=">(kern::to<Float>(self.ballot_.id), 0.0)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(value))), kern::to<Float>(value_int(kern::to<Int>(self.friendly_fire_)))))) {
      return;
    }
    self.ballot_serial_ = kern::to<Int>(kern::formula([](Float ballot_serial_) -> Float { return (ballot_serial_ + 1.0); }, kern::to<Float>(self.ballot_serial_)));
    n121 = mp_rules_cpp::create_ballot(self.ballot_, self.ballot_serial_, 0, value, self.self_slot_, Session_roster(self).Result, net_session::Session_now_ms(self).Result);
    if (logic_not(n121.Result)) {
      return;
    }
    w = List<U8>{};
    wire::put_u32(w, self.ballot_.id);
    wire::put_u8(w, self.ballot_.key);
    wire::put_u8(w, kern::to<Int>(value));
    wire::put_u8(w, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<U8>(self.ballot_.members)))));
    for (auto it122 = flow_for(n122, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(self.ballot_.members))))); it122.next();) {
      m = self.ballot_.members[n122.index];
      wire::put_u8(w, m);
    }
    Session_to_all(self, w, 0, 19, kNobody);
    t = "";
    t = text_format<"FRIENDLY FIRE: %s - Abstimmung 1/%zu">(95, logic_select<Text>(value, "EIN", "AUS"), list_length<U8>(self.ballot_.members));
    self.vote_text_ = t;
  }

  void Session_send_rules(net_Session& self) {
    List<U8> w = List<U8>{};
    w = List<U8>{};
    wire::put_u8(w, kern::to<Int>(self.friendly_fire_));
    if (self.have_world_rules_) {
      wire::put_u16(w, self.world_rules_);
    }
    Session_to_all(self, w, 0, 17, kNobody);
  }

  void Session_finish_ballot(net_Session& self, mp_Ballot_Status s = mp_Ballot_Status::pending) {
    U8 result = 0;
    List<U8> w = List<U8>{};
    result = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? 1.0 : ((a_2 != 0.0) ? 2.0 : 3.0)); }, kern::to<Float>(enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::approved)), kern::to<Float>(enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::rejected))));
    if (enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::approved)) {
      self.friendly_fire_ = self.ballot_.value;
      Session_send_rules(self);
    }
    w = List<U8>{};
    wire::put_u32(w, self.ballot_.id);
    wire::put_u8(w, result);
    Session_to_all(self, w, 0, 21, kNobody);
    self.vote_text_ = logic_select<Text>(math_compare<"==">(kern::to<Float>(result), 1.0), "Einstimmig angenommen", logic_select<Text>(math_compare<"==">(kern::to<Float>(result), 2.0), "Abgelehnt", "Abstimmung abgelaufen / Spielerwechsel"));
    self.ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending);
  }

  void Session_dispatch(net_Session& self, const List<U8>& d, U8 from = 0, U16 type = 0) {
    Int selected = 0;
    List<U8> w = List<U8>{};
    List<U8> w_2 = List<U8>{};
    wire_Reader r = wire_Reader{0, true};
    U8 claim = 0;
    U8 owner = 0;
    U8 seat = 0;
    net_Fx f = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    List<U8> a = List<U8>{};
    I32 arg = 0;
    U8 owner_2 = 0;
    U8 serial = 0;
    U8 part = 0;
    Bool last = false;
    List<net_SwarmUnit> units = List<net_SwarmUnit>{};
    List<U8> a_2 = List<U8>{};
    net_SwarmHit h = net_SwarmHit{0, 0, 0, net_SwarmKind::zombie, 0.0};
    List<U8> a_3 = List<U8>{};
    U8 arg_2 = 0;
    net_Hit h_2 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
    wire_Reader r_2 = wire_Reader{0, true};
    net_Kill k = net_Kill{0, 254, 0, 0};
    List<U8> ro = List<U8>{};
    Int value = 0;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    List<U8> w_3 = List<U8>{};
    Float protect_ms = 0.0;
    Float until = 0.0;
    Bool found = false;
    List<U8> w_4 = List<U8>{};
    wire_Reader r_3 = wire_Reader{0, true};
    net_PingEvent p = net_PingEvent{0, false, Vector{0.0, 0.0, 0.0}, 0};
    List<U8> w_5 = List<U8>{};
    wire_Reader r_4 = wire_Reader{0, true};
    net_VoiceFrame v = net_VoiceFrame{0, 0, false, List<U8>{}};
    Vector at_2 = Vector{0.0, 0.0, 0.0};
    Float radius = 0.0;
    List<U8> bytes = List<U8>{};
    wire_Reader r_5 = wire_Reader{0, true};
    Bool value_2 = false;
    wire_Reader r_6 = wire_Reader{0, true};
    U32 id = 0;
    Bool yes = false;
    mp_Ballot_Status s = mp_Ballot_Status::pending;
    Text t = "";
    selected = type;
    if (math_compare<"==">(kern::to<Float>(selected), 29.0)) {
      if (logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.destroy_)), 64.0))) {
        list_add(self.destroy_, struct_make<net_Session_Blob>(from, d));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 32.0)) {
      if (logic_and(logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host), math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_))), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.town_)), 512.0))) {
        list_add(self.town_, struct_make<net_Session_Blob>(from, d));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 31.0)) {
      if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
        return;
      }
      if (math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.proj_)), 64.0)) {
        list_add(self.proj_, struct_make<net_Session_Blob>(from, d));
      }
      w = List<U8>{};
      wire::put_u8(w, from);
      wire::put_bytes(w, d);
      Session_to_all(self, w, 1, 31, from);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 30.0)) {
      if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::host))) {
        return;
      }
      if (math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.toss_)), 64.0)) {
        list_add(self.toss_, struct_make<net_Session_Blob>(from, d));
      }
      w_2 = List<U8>{};
      wire::put_u8(w_2, from);
      wire::put_bytes(w_2, d);
      Session_to_all(self, w_2, 1, 30, from);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 23.0)) {
      r = wire_Reader{0, true};
      n85 = wire::Reader_get_u8(r, d);
      claim = n85.Result;
      n86 = wire::Reader_get_u8(r, d);
      owner = n86.Result;
      n87 = wire::Reader_get_u8(r, d);
      seat = n87.Result;
      if (logic_and(r.ok, math_compare<"<=">(kern::to<Float>(seat), 1.0))) {
        Session_judge_seat(self, from, math_compare<"!=">(kern::to<Float>(claim), 0.0), owner, enum_from_index<seats_Seat>(seat).value);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
      f = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
      n92 = unpack_fx(d, f);
      if (logic_not(n92.Result)) {
        return;
      }
      f.owner = from;
      if (math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_))) {
        list_add(self.fx_, f);
      }
      arg = kern::to<Int>(kern::formula([](Float kind, Float a) -> Float { return ((kind == a) ? 1.0 : 0.0); }, kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::shot))));
      n93 = pack_fx(f);
      a = n93.Result;
      Session_to_all(self, a, arg, 5, from);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 25.0)) {
      owner_2 = 0;
      serial = 0;
      part = 0;
      last = false;
      units = List<net_SwarmUnit>{};
      n94 = unpack_swarm(d, owner_2, serial, part, last, units);
      if (logic_not(n94.Result)) {
        return;
      }
      n95 = pack_swarm(units, from, serial, part, last, 0, list_length<net_SwarmUnit>(units));
      a_2 = n95.Result;
      Session_to_all(self, a_2, 1, 25, from);
      if (math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_))) {
        Session_swarm_part(self, units, from, serial, part, last);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 26.0)) {
      h = net_SwarmHit{0, 0, 0, net_SwarmKind::zombie, 0.0};
      n99 = unpack_swarm_hit(d, h);
      if (logic_not(n99.Result)) {
        return;
      }
      h.from = from;
      if (math_compare<"==">(kern::to<Float>(h.to), kern::to<Float>(self.self_slot_))) {
        list_add(self.swarm_hits_, h);
      } else if (math_compare<"!=">(kern::to<Float>(h.to), kern::to<Float>(kHost))) {
        arg_2 = h.to;
        n100 = pack_swarm_hit(h);
        a_3 = n100.Result;
        Session_to_client(self, a_3, arg_2, 0, 26);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 6.0)) {
      h_2 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
      n101 = unpack_hit(d, h_2);
      if (n101.Result) {
        Session_judge(self, h_2, from);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 9.0)) {
      r_2 = wire_Reader{0, true};
      n105 = wire::Reader_get_u8(r_2, d);
      n106 = wire::Reader_get_u8(r_2, d);
      n107 = wire::Reader_get_u8(r_2, d);
      k = struct_make<net_Kill>(from, n105.Result, n106.Result, n107.Result);
      if (logic_not(r_2.ok)) {
        return;
      }
      ro = Session_roster(self).Result;
      value = k.by;
      at = 0;
      end = list_length<U8>(ro);
      go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      while (loop(go)) {
        if (math_compare<"==">(kern::to<Float>(ro[at]), kern::to<Float>(value))) {
          go = false;
        } else {
          at += 1;
          go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
        }
      }
      if (math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<U8>(ro)))) {
        k.by = kNobody;
      }
      mp_rules_cpp::Scoreboard_died(self.scores_, k.victim, kern::to<Int>(kern::formula([](Float by, Float kNobody) -> Float { return ((by == kNobody) ? (-1.0) : by); }, kern::to<Float>(k.by), kern::to<Float>(kNobody))));
      list_add(self.kills_, k);
      w_3 = List<U8>{};
      wire::put_u8(w_3, k.victim);
      wire::put_u8(w_3, k.by);
      wire::put_u8(w_3, k.weapon);
      wire::put_u8(w_3, k.zone);
      Session_to_all(self, w_3, 0, 10, kNobody);
      Session_send_scores(self);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 12.0)) {
      protect_ms = mp_rules_cpp::spawn_protect_ms().Result;
      until = kern::formula([](Float the_result, Float protect_ms) -> Float { return (the_result + protect_ms); }, net_session::Session_now_ms(self).Result, protect_ms);
      found = false;
      for (auto it109 = flow_for(n109, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.protected_))))); it109.next();) {
        if (math_compare<"==">(kern::to<Float>(self.protected_[n109.index].first), kern::to<Float>(from))) {
          self.protected_[n109.index].second = until;
          found = true;
        }
      }
      if (logic_not(found)) {
        list_add(self.protected_, struct_make<Int_Float_pair>(from, until));
      }
      w_4 = List<U8>{};
      wire::put_u8(w_4, from);
      wire::put_f32(w_4, protect_ms);
      Session_to_all(self, w_4, 0, 13, kNobody);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 15.0)) {
      r_3 = wire_Reader{0, true};
      n110 = wire::Reader_get_u8(r_3, d);
      n111 = wire::Reader_get_u8(r_3, d);
      n112 = wire::Reader_get_vec3(r_3, d);
      n113 = wire::Reader_get_u32(r_3, d);
      p = struct_make<net_PingEvent>(from, math_compare<"!=">(kern::to<Float>(n111.Result), 0.0), n112.Result, n113.Result);
      if (logic_or(logic_not(r_3.ok), logic_not(sane(p.p).Result))) {
        return;
      }
      if (math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_))) {
        list_add(self.pings_, p);
      }
      w_5 = List<U8>{};
      wire::put_u8(w_5, from);
      wire::put_u8(w_5, kern::to<Int>(p.on));
      wire::put_vec3(w_5, p.p);
      wire::put_u32(w_5, p.color);
      Session_to_all(self, w_5, 0, 15, from);
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 16.0)) {
      r_4 = wire_Reader{0, true};
      n114 = wire::Reader_get_u8(r_4, d);
      n115 = wire::Reader_get_u32(r_4, d);
      n116 = wire::Reader_get_u8(r_4, d);
      n117 = wire::Reader_get_bytes(r_4, d);
      v = struct_make<net_VoiceFrame>(from, n115.Result, math_compare<"!=">(kern::to<Float>(n116.Result), 0.0), n117.Result);
      if (logic_or(logic_or(logic_not(r_4.ok), math_compare<"<">(kern::to<Float>(list_length<U8>(v.data)), 4.0)), math_compare<">">(kern::to<Float>(list_length<U8>(v.data)), 800.0))) {
        return;
      }
      at_2 = Session_focus_of(self, from).Result;
      radius = mp_rules_cpp::voice_radius().Result;
      if (logic_and(math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_)), logic_or(v.everywhere, math_compare<"<=">(vector_length(vector_sub(Session_focus_of(self, self.self_slot_).Result, at_2)), radius)))) {
        list_add(self.voice_, v);
      }
      bytes = List<U8>{};
      wire::put_u8(bytes, from);
      wire::put_u32(bytes, v.seq);
      wire::put_u8(bytes, kern::to<Int>(v.everywhere));
      wire::put_bytes(bytes, v.data);
      for (auto it118 = flow_for(n118, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it118.next();) {
        if (logic_and(math_compare<"!=">(kern::to<Float>(self.remotes_[n118.index].state.slot), kern::to<Float>(from)), logic_or(v.everywhere, math_compare<"<=">(vector_length(vector_sub(Session_focus_of(self, self.remotes_[n118.index].state.slot).Result, at_2)), radius)))) {
          Session_to_client(self, bytes, self.remotes_[n118.index].state.slot, 1, 16);
        }
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 18.0)) {
      r_5 = wire_Reader{0, true};
      n119 = wire::Reader_get_u8(r_5, d);
      n120 = wire::Reader_get_u8(r_5, d);
      value_2 = math_compare<"!=">(kern::to<Float>(n120.Result), 0.0);
      if (r_5.ok) {
        Session_begin_ballot(self, value_2);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 20.0)) {
      r_6 = wire_Reader{0, true};
      n123 = wire::Reader_get_u32(r_6, d);
      id = n123.Result;
      n124 = wire::Reader_get_u8(r_6, d);
      yes = math_compare<"!=">(kern::to<Float>(n124.Result), 0.0);
      if (logic_or(logic_or(logic_not(r_6.ok), logic_not(math_compare<"!=">(kern::to<Float>(self.ballot_.id), 0.0))), math_compare<"!=">(kern::to<Float>(id), kern::to<Float>(self.ballot_.id)))) {
        return;
      }
      n125 = mp_rules_cpp::cast_vote_2(self.ballot_, from, yes, Session_roster(self).Result, net_session::Session_now_ms(self).Result);
      s = n125.Result;
      if (enum_equal<mp_Ballot_Status>(s, mp_Ballot_Status::pending)) {
        t = "";
        t = text_format<"FRIENDLY FIRE: %zu/%zu JA">(95, list_length<U8>(self.ballot_.yes), list_length<U8>(self.ballot_.members));
        self.vote_text_ = t;
      } else {
        Session_finish_ballot(self, s);
      }
      return;
    } else {
      return;
    }
  }

  void Session_to_host(net_Session& self, const List<U8>& data, I32 ch = 0, U16 type = 0) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_dispatch(self, data, kHost, type);
    } else {
      both = math_compare<"!=">(kern::to<Float>(self.client_), 0.0);
      if (both) {
        connected_2 = engine_client_connected(self.client_);
        both = connected_2;
      }
      if (both) {
        engine_client_send(n126, self.client_, ch, type, data);
        send = n126.result;
      }
    }
  }

  void Session_send_fx(net_Session& self, net_Fx f = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}) {
    net_Fx f_ = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    List<U8> a = List<U8>{};
    I32 arg = 0;
    f_ = f;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return;
    }
    f_.owner = self.self_slot_;
    arg = kern::to<Int>(kern::formula([](Float kind, Float a) -> Float { return ((kind == a) ? 1.0 : 0.0); }, kern::to<Float>(f_.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::shot))));
    n127 = pack_fx(f_);
    a = n127.Result;
    Session_to_host(self, a, arg, 5);
  }

  struct Session_send_swarmResult { Bool Result = false; };
  Session_send_swarmResult Session_send_swarm(net_Session& self, const List<net_SwarmUnit>& units) {
    Int parts = 0;
    Int from = 0;
    Int n = 0;
    List<U8> a = List<U8>{};
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return {true};
    }
    if (math_compare<">">(kern::formula([](Float kSwarmMaxUnits, Float a) -> Float { return ((kSwarmMaxUnits < 0.0) != (a < 0.0) ? (a < 0.0) : (kSwarmMaxUnits < a)); }, kern::to<Float>(kSwarmMaxUnits), kern::to<Float>(list_length<net_SwarmUnit>(units))), 0.5)) {
      Session_refused(self, 25, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 18.0); }, kern::to<Float>(list_length<net_SwarmUnit>(units)))));
      return {false};
    }
    self.swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return (swarm_serial_ + 1.0); }, kern::to<Float>(self.swarm_serial_)));
    parts = kern::to<Int>(kern::formula([](Float a, Float kSwarmPerMessage) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::div(((a + kSwarmPerMessage) - 1.0), kSwarmPerMessage))); }, kern::to<Float>(list_length<net_SwarmUnit>(units)), kern::to<Float>(kSwarmPerMessage)));
    for (auto it128 = flow_for(n128, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(parts)))); it128.next();) {
      from = kern::to<Int>(kern::formula([](Float index, Float kSwarmPerMessage) -> Float { return (index * kSwarmPerMessage); }, kern::to<Float>(n128.index), kern::to<Float>(kSwarmPerMessage)));
      n = kern::to<Int>(kern::formula([](Float kSwarmPerMessage, Float a, Float from) -> Float { return kern::math::min(kSwarmPerMessage, (a - from)); }, kern::to<Float>(kSwarmPerMessage), kern::to<Float>(list_length<net_SwarmUnit>(units)), kern::to<Float>(from)));
      n129 = pack_swarm(units, self.self_slot_, self.swarm_serial_, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(index, 255.0); }, kern::to<Float>(n128.index))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n128.index))))), kern::to<Float>(parts)), from, n);
      a = n129.Result;
      Session_to_host(self, a, 1, 25);
    }
    return {true};
  }

  void Session_town_to_host(net_Session& self, const List<U8>& blob) {
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
      engine_client_send(n131, self.client_, 0, 32, blob);
      send_2 = n131.result;
      both_2 = logic_not(send_2);
    }
    if (both_2) {
      Session_refused(self, 32, list_length<U8>(blob));
    }
  }

  void Session_town_to_all(net_Session& self, const List<U8>& blob) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, 0, 32, kHost);
    }
  }

  void Session_town_to(net_Session& self, const List<U8>& blob, U8 slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 0, 32);
    }
  }

  void Session_send_world(net_Session& self, const List<U8>& blob) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, 1, 27, kHost);
    }
  }

  void Session_send_world_to(net_Session& self, const List<U8>& blob, U8 slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 1, 27);
    }
  }

  /// Destruction sync (game_destruction_sync.cpp): reliable, the host is the authority.
  void Session_send_proj(net_Session& self, const List<U8>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    List<U8> w = List<U8>{};
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    if (both) {
      engine_client_send(n132, self.client_, 1, 31, blob);
      send = n132.result;
    } else if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      w = List<U8>{};
      wire::put_u8(w, self.self_slot_);
      wire::put_bytes(w, blob);
      Session_to_all(self, w, 1, 31, kHost);
    }
  }

  void Session_send_toss(net_Session& self, const List<U8>& blob) {
    Bool connected = false;
    Bool connected_2 = false;
    Bool both = false;
    Bool send = false;
    List<U8> w = List<U8>{};
    both = logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"!=">(kern::to<Float>(self.client_), 0.0));
    if (both) {
      connected_2 = engine_client_connected(self.client_);
      both = connected_2;
    }
    if (both) {
      engine_client_send(n133, self.client_, 1, 30, blob);
      send = n133.result;
    } else if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      w = List<U8>{};
      wire::put_u8(w, self.self_slot_);
      wire::put_bytes(w, blob);
      Session_to_all(self, w, 1, 30, kHost);
    }
  }

  void Session_destroy_to_host(net_Session& self, const List<U8>& blob) {
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
      engine_client_send(n134, self.client_, 0, 29, blob);
      send = n134.result;
    }
  }

  void Session_destroy_to_all(net_Session& self, const List<U8>& blob, Bool reliable = false) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_all(self, blob, kern::to<Int>(kern::formula([](Float reliable) -> Float { return ((reliable != 0.0) ? 0.0 : 1.0); }, kern::to<Float>(reliable))), 29, kHost);
    }
  }

  void Session_destroy_to(net_Session& self, const List<U8>& blob, U8 slot = 0) {
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_to_client(self, blob, slot, 0, 29);
    }
  }

  void Session_send_swarm_hit(net_Session& self, U8 to = 0, U16 id = 0, net_SwarmKind kind = net_SwarmKind::zombie, Float damage = 0.0) {
    net_SwarmHit h = net_SwarmHit{0, 0, 0, net_SwarmKind::zombie, 0.0};
    List<U8> a = List<U8>{};
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return;
    }
    h = struct_make<net_SwarmHit>(self.self_slot_, to, id, kind, damage);
    n135 = pack_swarm_hit(h);
    a = n135.Result;
    Session_to_host(self, a, 0, 26);
  }

  void Session_request_hit(net_Session& self, U8 target = 0, Float damage = 0.0, U8 weapon = 0, U8 zone = 0, Vector from = Vector{0.0, 0.0, 0.0}) {
    net_Hit h = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
    List<U8> a = List<U8>{};
    if (logic_or(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none), math_compare<"<">(damage, 1.0))) {
      return;
    }
    h = struct_make<net_Hit>(target, self.self_slot_, weapon, zone, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float damage) -> Float { return kern::math::round(damage); }, damage), 1.0, 100.0)), from);
    n136 = pack_hit(h);
    a = n136.Result;
    Session_to_host(self, a, 0, 6);
  }

  void Session_report_death(net_Session& self, U8 by = 0, U8 weapon = 0, U8 zone = 0) {
    List<U8> w = List<U8>{};
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return;
    }
    w = List<U8>{};
    wire::put_u8(w, by);
    wire::put_u8(w, weapon);
    wire::put_u8(w, zone);
    Session_to_host(self, w, 0, 9);
  }

  void Session_report_respawn(net_Session& self) {
    List<U8> a = List<U8>{};
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none))) {
      a = List<U8>{};
      Session_to_host(self, a, 0, 12);
    }
  }

  void Session_send_ping(net_Session& self, Bool on = false, Vector p = Vector{0.0, 0.0, 0.0}, U32 color = 0) {
    List<U8> w = List<U8>{};
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return;
    }
    w = List<U8>{};
    wire::put_u8(w, self.self_slot_);
    wire::put_u8(w, kern::to<Int>(on));
    wire::put_vec3(w, p);
    wire::put_u32(w, color);
    Session_to_host(self, w, 0, 15);
  }

  void Session_send_voice(net_Session& self, const List<U8>& d, U32 seq = 0, Bool all = false) {
    List<U8> w = List<U8>{};
    if (logic_or(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none), math_compare<"==">(kern::to<Float>(list_length<net_Remote>(self.remotes_)), 0.0))) {
      return;
    }
    w = List<U8>{};
    wire::put_u8(w, self.self_slot_);
    wire::put_u32(w, seq);
    wire::put_u8(w, kern::to<Int>(all));
    wire::put_bytes(w, d);
    Session_to_host(self, w, 1, 16);
  }

  struct Session_propose_ruleResult { Bool Result = false; };
  Session_propose_ruleResult Session_propose_rule(net_Session& self, Bool ff = false) {
    List<U8> w = List<U8>{};
    if (logic_or(logic_or(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none), math_compare<"<">(kern::to<Float>(list_length<U8>(Session_roster(self).Result)), 2.0)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(ff))), kern::to<Float>(value_int(kern::to<Int>(self.friendly_fire_)))))) {
      return {false};
    }
    w = List<U8>{};
    wire::put_u8(w, 0);
    wire::put_u8(w, kern::to<Int>(ff));
    Session_to_host(self, w, 0, 18);
    return {true};
  }

  struct Session_voteResult { Bool Result = false; };
  Session_voteResult Session_vote(net_Session& self, Bool approve = false) {
    List<U8> w = List<U8>{};
    if (logic_or(logic_not(math_compare<"!=">(kern::to<Float>(self.remote_ballot_.id), 0.0)), logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client)))) {
      return {false};
    }
    w = List<U8>{};
    wire::put_u32(w, self.remote_ballot_.id);
    wire::put_u8(w, kern::to<Int>(approve));
    Session_to_host(self, w, 0, 20);
    self.vote_text_ = logic_select<Text>(approve, "JA gesendet", "NEIN gesendet");
    self.remote_ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending);
    return {true};
  }

  void Session_set_world_rules(net_Session& self, U16 mask = 0) {
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

  void Session_send_anchor(net_Session& self, U8 slot = 0) {
    List<Vector> pts = List<Vector>{};
    Vector mine = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    List<U8> w = List<U8>{};
    pts = List<Vector>{};
    mine = Session_focus_of(self, self.self_slot_).Result;
    if (logic_not(math_compare<">">(kern::formula([](Float flags, Float a) -> Float { return (kern::math::band(flags, a) != 0.0); }, kern::to<Float>(self.me_.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5))) {
      list_add(pts, vector_make(vector_break(mine).x, vector_break(mine).y, 0.0));
    }
    for (auto it137 = flow_for(n137, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it137.next();) {
      if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.remotes_[n137.index].state.slot), kern::to<Float>(slot)), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(self.remotes_[n137.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5))), math_compare<"<">(kern::formula([](Float the_result, Float a) -> Float { return (the_result - a); }, net_session::Session_now_ms(self).Result, self.remotes_[n137.index].received_ms), 3500.0))) {
        p = Session_focus_of(self, self.remotes_[n137.index].state.slot).Result;
        list_add(pts, vector_make(vector_break(p).x, vector_break(p).y, 0.0));
      }
    }
    c = vector_make(vector_break(mine).x, vector_break(mine).y, 0.0);
    n138 = mp_rules_cpp::group_spawn_center(pts, c, 65.0);
    w = List<U8>{};
    wire::put_vec3(w, vector_make(vector_break(c).x, vector_break(c).y, vector_break(mine).z));
    Session_to_client(self, w, slot, 0, 14);
  }

  void Session_seat_request(net_Session& self, Bool claim = false, U8 owner = 0, seats_Seat seat = seats_Seat::pilot) {
    U8 me = 0;
    List<U8> w = List<U8>{};
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      me = self.self_slot_;
      if (claim) {
        n139 = jet_seats_cpp::Table_claim(self.seats_, owner, me, seat);
      } else {
        n140 = jet_seats_cpp::Table_release(self.seats_, owner, me, seat);
      }
      return;
    }
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      Session_judge_seat(self, self.self_slot_, claim, owner, seat);
      return;
    }
    w = List<U8>{};
    wire::put_u8(w, kern::to<Int>(claim));
    wire::put_u8(w, owner);
    wire::put_u8(w, enum_index<seats_Seat>(seat));
    Session_to_host(self, w, 0, 23);
  }

  void Session_on_pose(net_Session& self, const net_PlayerState& s) {
    Float now = 0.0;
    Int at = 0;
    Vector fwd = Vector{0.0, 0.0, 0.0};
    if (math_compare<"==">(kern::to<Float>(s.slot), kern::to<Float>(self.self_slot_))) {
      return;
    }
    now = net_session::Session_now_ms(self).Result;
    at = list_length<net_Remote>(self.remotes_);
    for (auto it148 = flow_for(n148, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it148.next();) {
      if (math_compare<"==">(kern::to<Float>(self.remotes_[n148.index].state.slot), kern::to<Float>(s.slot))) {
        at = n148.index;
      }
    }
    if (math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<net_Remote>(self.remotes_)))) {
      list_add(self.remotes_, net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0});
      self.remotes_[at].shown = s.position;
      self.remotes_[at].shown_rotation = s.rotation;
      self.remotes_[at].person_shown = s.avatar;
      if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
        mp_rules_cpp::Scoreboard_ensure(self.scores_, s.slot);
        Session_send_scores(self);
      }
    }
    if (logic_or(logic_not(enum_equal<net_PlayerMode>(self.remotes_[at].state.mode, s.mode)), math_compare<">">(vector_length(vector_sub(mp_rules::PoseTimeline_last_position(self.remotes_[at].body).Result, s.position)), kTeleportM))) {
      mp_rules::PoseTimeline_reset(self.remotes_[at].body);
    }
    if (math_compare<">">(vector_length(vector_sub(mp_rules::PoseTimeline_last_position(self.remotes_[at].person).Result, s.avatar)), kTeleportM)) {
      mp_rules::PoseTimeline_reset(self.remotes_[at].person);
    }
    self.remotes_[at].state = s;
    self.remotes_[at].received_ms = now;
    n149 = mp_rules_cpp::PoseTimeline_push(self.remotes_[at].body, s.position, s.rotation, s.velocity, true, kern::to<Float>(s.t_ms), now);
    n150 = mp_rules_cpp::PoseTimeline_push(self.remotes_[at].person, s.avatar, common::yaw_quat(s.avatar_yaw).Result, Vector{0.0, 0.0, 0.0}, false, kern::to<Float>(s.t_ms), now);
    if (logic_or(enum_equal<net_PlayerMode>(s.mode, net_PlayerMode::car), enum_equal<net_PlayerMode>(s.mode, net_PlayerMode::jet))) {
      fwd = engine_math_math::Quat_rotate(s.rotation, logic_select<Vector>(enum_equal<net_PlayerMode>(s.mode, net_PlayerMode::jet), Vector{0.0, 1.0, 0.0}, Vector{1.0, 0.0, 0.0})).Result;
      n151 = mp_rules_cpp::NavTimeline_push(self.remotes_[at].nav, s.position, kern::formula([](Float x, Float y) -> Float { return kern::math::atan2(x, y); }, vector_break(fwd).x, vector_break(fwd).y), enum_index<net_PlayerMode>(s.mode), now);
    } else {
      mp_rules::NavTimeline_reset(self.remotes_[at].nav);
    }
  }

  struct deliver_matchResult { Bool Result = false; };
  KERN_PURE deliver_matchResult deliver_match(const net_Remote& x, U8 slot_2 = 0) {
    return {math_compare<"==">(kern::to<Float>(x.state.slot), kern::to<Float>(slot_2))};
  }

  struct deliver_match_2Result { Bool Result = false; };
  KERN_PURE deliver_match_2Result deliver_match_2(const net_Session_SwarmParts& p, U8 slot_2 = 0) {
    return {math_compare<"==">(kern::to<Float>(p.owner), kern::to<Float>(slot_2))};
  }

  void Session_deliver(net_Session& self, const List<U8>& d, U16 type = 0) {
    wire_Reader r = wire_Reader{0, true};
    Int selected = 0;
    Float lon = 0.0;
    Float lat = 0.0;
    net_PlayerState s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
    Int at = 0;
    net_Fx f = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    U8 owner = 0;
    U8 serial = 0;
    U8 part = 0;
    Bool last = false;
    List<net_SwarmUnit> units = List<net_SwarmUnit>{};
    net_SwarmHit h = net_SwarmHit{0, 0, 0, net_SwarmKind::zombie, 0.0};
    U8 from = 0;
    List<U8> body = List<U8>{};
    U8 from_2 = 0;
    List<U8> body_2 = List<U8>{};
    U32 rev = 0;
    U8 n = 0;
    List<seats_Row> rows = List<seats_Row>{};
    U8 i = 0;
    seats_Row q = seats_Row{253, 253, 253};
    net_Hit h_2 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
    net_Hit h_3 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
    net_Kill k = net_Kill{0, 254, 0, 0};
    U8 n_2 = 0;
    List<mp_Score> rows_2 = List<mp_Score>{};
    U8 i_2 = 0;
    mp_Score s_2 = mp_Score{0, 0, 0};
    U8 slot = 0;
    Float ms = 0.0;
    Bool found = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    net_PingEvent p_2 = net_PingEvent{0, false, Vector{0.0, 0.0, 0.0}, 0};
    net_VoiceFrame v = net_VoiceFrame{0, 0, false, List<U8>{}};
    U8 ff = 0;
    U16 m = 0;
    mp_Ballot b = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending};
    U8 n_3 = 0;
    U8 i_3 = 0;
    U8 res = 0;
    U8 slot_2 = 0;
    Int at_2 = 0;
    Int kept = 0;
    Int end = 0;
    Int at_3 = 0;
    Int kept_2 = 0;
    Int end_2 = 0;
    r = wire_Reader{0, true};
    selected = type;
    if (math_compare<"==">(kern::to<Float>(selected), 28.0)) {
      n141 = wire::Reader_get_f64(r, d);
      lon = n141.Result;
      n142 = wire::Reader_get_f64(r, d);
      lat = n142.Result;
      if (logic_and(logic_and(logic_and(logic_and(logic_and(r.ok, enum_equal<net_Session_Role>(self.role_, net_Session_Role::client)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lon), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lat), 0.0)), math_compare<"<=">(kern::formula([](Float lat) -> Float { return kern::math::abs(lat); }, lat), 85.0)), math_compare<"<=">(kern::formula([](Float lon) -> Float { return kern::math::abs(lon); }, lon), 180.0))) {
        self.lon_ = lon;
        self.lat_ = lat;
        self.have_origin_ = true;
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
      n143 = wire::Reader_get_u8(r, d);
      self.self_slot_ = n143.Result;
      n144 = wire::Reader_get_f64(r, d);
      self.lon_ = n144.Result;
      n145 = wire::Reader_get_f64(r, d);
      self.lat_ = n145.Result;
      n146 = wire::Reader_get_u8(r, d);
      self.friendly_fire_ = math_compare<"!=">(kern::to<Float>(n146.Result), 0.0);
      if (r.ok) {
        self.have_origin_ = true;
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
      at = 0;
      n147 = unpack(d, at, s);
      if (n147.Result) {
        Session_on_pose(self, s);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
      f = net_Fx{0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
      n152 = unpack_fx(d, f);
      if (logic_and(n152.Result, math_compare<"!=">(kern::to<Float>(f.owner), kern::to<Float>(self.self_slot_)))) {
        list_add(self.fx_, f);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 25.0)) {
      owner = 0;
      serial = 0;
      part = 0;
      last = false;
      units = List<net_SwarmUnit>{};
      n153 = unpack_swarm(d, owner, serial, part, last, units);
      if (logic_and(n153.Result, math_compare<"!=">(kern::to<Float>(owner), kern::to<Float>(self.self_slot_)))) {
        Session_swarm_part(self, units, owner, serial, part, last);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 26.0)) {
      h = net_SwarmHit{0, 0, 0, net_SwarmKind::zombie, 0.0};
      n154 = unpack_swarm_hit(d, h);
      if (logic_and(n154.Result, math_compare<"==">(kern::to<Float>(h.to), kern::to<Float>(self.self_slot_)))) {
        list_add(self.swarm_hits_, h);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 27.0)) {
      if (math_compare<"<">(kern::to<Float>(list_length<U8_row>(self.world_)), 8.0)) {
        list_add(self.world_, struct_make<U8_row>(d));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 29.0)) {
      if (logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.destroy_)), 64.0))) {
        list_add(self.destroy_, struct_make<net_Session_Blob>(kHost, d));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 32.0)) {
      if (logic_and(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.town_)), 512.0))) {
        list_add(self.town_, struct_make<net_Session_Blob>(kHost, d));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 31.0)) {
      if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client))) {
        return;
      }
      n155 = wire::Reader_get_u8(r, d);
      from = n155.Result;
      n156 = wire::Reader_get_bytes(r, d);
      body = n156.Result;
      if (logic_and(logic_and(r.ok, math_compare<"!=">(kern::to<Float>(from), kern::to<Float>(self.self_slot_))), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.proj_)), 64.0))) {
        list_add(self.proj_, struct_make<net_Session_Blob>(from, body));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 30.0)) {
      if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client))) {
        return;
      }
      n157 = wire::Reader_get_u8(r, d);
      from_2 = n157.Result;
      n158 = wire::Reader_get_bytes(r, d);
      body_2 = n158.Result;
      if (logic_and(logic_and(r.ok, math_compare<"!=">(kern::to<Float>(from_2), kern::to<Float>(self.self_slot_))), math_compare<"<">(kern::to<Float>(list_length<net_Session_Blob>(self.toss_)), 64.0))) {
        list_add(self.toss_, struct_make<net_Session_Blob>(from_2, body_2));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 24.0)) {
      n159 = wire::Reader_get_u32(r, d);
      rev = n159.Result;
      n160 = wire::Reader_get_u8(r, d);
      n = n160.Result;
      rows = List<seats_Row>{};
      i = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), r.ok))) {
        q = seats_Row{253, 253, 253};
        n161 = wire::Reader_get_u8(r, d);
        q.owner = n161.Result;
        n162 = wire::Reader_get_u8(r, d);
        q.pilot = n162.Result;
        n163 = wire::Reader_get_u8(r, d);
        q.passenger = n163.Result;
        list_add(rows, q);
        i += 1;
      }
      if (r.ok) {
        n164 = jet_seats_cpp::Table_load(self.seats_, rows, rev);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 7.0)) {
      h_2 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
      n165 = unpack_hit(d, h_2);
      if (logic_and(n165.Result, math_compare<"==">(kern::to<Float>(h_2.target), kern::to<Float>(self.self_slot_)))) {
        list_add(self.hits_, h_2);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 8.0)) {
      h_3 = net_Hit{0, 254, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}};
      n166 = unpack_hit(d, h_3);
      if (n166.Result) {
        list_add(self.confirms_, h_3);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 10.0)) {
      n167 = wire::Reader_get_u8(r, d);
      n168 = wire::Reader_get_u8(r, d);
      n169 = wire::Reader_get_u8(r, d);
      n170 = wire::Reader_get_u8(r, d);
      k = struct_make<net_Kill>(n167.Result, n168.Result, n169.Result, n170.Result);
      if (r.ok) {
        list_add(self.kills_, k);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 11.0)) {
      n171 = wire::Reader_get_u8(r, d);
      n_2 = n171.Result;
      rows_2 = List<mp_Score>{};
      i_2 = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(n_2)), r.ok))) {
        s_2 = mp_Score{0, 0, 0};
        n172 = wire::Reader_get_u8(r, d);
        s_2.slot = n172.Result;
        n173 = wire::Reader_get_u16(r, d);
        s_2.kills = n173.Result;
        n174 = wire::Reader_get_u16(r, d);
        s_2.deaths = n174.Result;
        list_add(rows_2, s_2);
        i_2 += 1;
      }
      if (r.ok) {
        mp_rules::Scoreboard_set(self.scores_, rows_2);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 13.0)) {
      n175 = wire::Reader_get_u8(r, d);
      slot = n175.Result;
      n176 = wire::Reader_get_f32(r, d);
      ms = n176.Result;
      if (logic_not(r.ok)) {
        return;
      }
      found = false;
      for (auto it177 = flow_for(n177, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.protected_))))); it177.next();) {
        if (math_compare<"==">(kern::to<Float>(self.protected_[n177.index].first), kern::to<Float>(slot))) {
          self.protected_[n177.index].second = kern::formula([](Float the_result, Float ms) -> Float { return (the_result + ms); }, net_session::Session_now_ms(self).Result, ms);
          found = true;
        }
      }
      if (logic_not(found)) {
        list_add(self.protected_, struct_make<Int_Float_pair>(slot, kern::formula([](Float the_result, Float ms) -> Float { return (the_result + ms); }, net_session::Session_now_ms(self).Result, ms)));
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 14.0)) {
      n178 = wire::Reader_get_vec3(r, d);
      p = n178.Result;
      if (logic_and(r.ok, sane(p).Result)) {
        self.anchor_ = p;
        self.anchor_ready_ = true;
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 15.0)) {
      n179 = wire::Reader_get_u8(r, d);
      n180 = wire::Reader_get_u8(r, d);
      n181 = wire::Reader_get_vec3(r, d);
      n182 = wire::Reader_get_u32(r, d);
      p_2 = struct_make<net_PingEvent>(n179.Result, math_compare<"!=">(kern::to<Float>(n180.Result), 0.0), n181.Result, n182.Result);
      if (logic_and(logic_and(r.ok, math_compare<"!=">(kern::to<Float>(p_2.owner), kern::to<Float>(self.self_slot_))), sane(p_2.p).Result)) {
        list_add(self.pings_, p_2);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 16.0)) {
      n183 = wire::Reader_get_u8(r, d);
      n184 = wire::Reader_get_u32(r, d);
      n185 = wire::Reader_get_u8(r, d);
      n186 = wire::Reader_get_bytes(r, d);
      v = struct_make<net_VoiceFrame>(n183.Result, n184.Result, math_compare<"!=">(kern::to<Float>(n185.Result), 0.0), n186.Result);
      if (logic_and(logic_and(logic_and(r.ok, math_compare<"!=">(kern::to<Float>(v.from), kern::to<Float>(self.self_slot_))), math_compare<">=">(kern::to<Float>(list_length<U8>(v.data)), 4.0)), math_compare<"<=">(kern::to<Float>(list_length<U8>(v.data)), 800.0))) {
        list_add(self.voice_, v);
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 17.0)) {
      n187 = wire::Reader_get_u8(r, d);
      ff = n187.Result;
      if (logic_not(r.ok)) {
        return;
      }
      self.friendly_fire_ = math_compare<"!=">(kern::to<Float>(ff), 0.0);
      if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::client)) {
        n188 = wire::Reader_get_u16(r, d);
        m = n188.Result;
        if (r.ok) {
          self.world_rules_ = m;
          self.have_world_rules_ = true;
        }
      }
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 19.0)) {
      b = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending};
      n189 = wire::Reader_get_u32(r, d);
      b.id = n189.Result;
      n190 = wire::Reader_get_u8(r, d);
      b.key = n190.Result;
      n191 = wire::Reader_get_u8(r, d);
      b.value = math_compare<"!=">(kern::to<Float>(n191.Result), 0.0);
      n192 = wire::Reader_get_u8(r, d);
      n_3 = n192.Result;
      i_3 = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(i_3), kern::to<Float>(n_3)), r.ok))) {
        n193 = wire::Reader_get_u8(r, d);
        list_add(b.members, n193.Result);
        i_3 += 1;
      }
      if (logic_or(logic_not(r.ok), logic_not(list_equal<U8>(b.members, Session_roster(self).Result)))) {
        return;
      }
      b.expires_ms = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result + the_result_2); }, net_session::Session_now_ms(self).Result, mp_rules_cpp::ballot_ms().Result);
      self.remote_ballot_ = b;
      self.vote_text_ = text_append(text_append("FRIENDLY FIRE: ", logic_select<Text>(b.value, "EIN", "AUS"), ""), "? Deine Stimme fehlt", "");
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 21.0)) {
      n194 = wire::Reader_get_u32(r, d);
      n195 = wire::Reader_get_u8(r, d);
      res = n195.Result;
      self.remote_ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending);
      self.vote_text_ = logic_select<Text>(math_compare<"==">(kern::to<Float>(res), 1.0), "Angenommen", logic_select<Text>(math_compare<"==">(kern::to<Float>(res), 2.0), "Abgelehnt", "Abstimmung abgebrochen"));
      return;
    } else if (math_compare<"==">(kern::to<Float>(selected), 22.0)) {
      n196 = wire::Reader_get_u8(r, d);
      slot_2 = n196.Result;
      at_2 = 0;
      kept = at_2;
      end = list_length<net_Remote>(self.remotes_);
      while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end)))) {
        if (logic_not(deliver_match(self.remotes_[at_2], slot_2).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at_2))) {
            var_take(n197, self.remotes_[at_2]);
            self.remotes_[kept] = n197.value;
          }
          kept += 1;
        }
        at_2 += 1;
      }
      list_resize(self.remotes_, kept, net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0});
      at_3 = 0;
      kept_2 = at_3;
      end_2 = list_length<net_Session_SwarmParts>(self.swarm_parts_);
      while (loop(math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_2)))) {
        if (logic_not(deliver_match_2(self.swarm_parts_[at_3], slot_2).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_3))) {
            var_take(n198, self.swarm_parts_[at_3]);
            self.swarm_parts_[kept_2] = n198.value;
          }
          kept_2 += 1;
        }
        at_3 += 1;
      }
      list_resize(self.swarm_parts_, kept_2, net_Session_SwarmParts{0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{}});
      return;
    } else {
      return;
    }
  }

  struct update_matchResult { Bool Result = false; };
  KERN_PURE update_matchResult update_match(const net_Remote& r, U8 slot = 0) {
    return {math_compare<"==">(kern::to<Float>(r.state.slot), kern::to<Float>(slot))};
  }

  struct update_match_2Result { Bool Result = false; };
  /// (the slot's next player starts his own serials)
  KERN_PURE update_match_2Result update_match_2(const net_Session_SwarmParts& p, U8 slot = 0) {
    return {math_compare<"==">(kern::to<Float>(p.owner), kern::to<Float>(slot))};
  }

  struct Session_update_matchResult { Bool Result = false; };
  Session_update_matchResult Session_update_match(net_Session& self, const Int_Float_pair& p) {
    return {math_compare<">">(net_session::Session_now_ms(self).Result, p.second)};
  }

  struct Session_update_match_2Result { Bool Result = false; };
  /// Players who went quiet are gone.
  Session_update_match_2Result Session_update_match_2(net_Session& self, const net_Remote& r) {
    return {math_compare<">">(kern::formula([](Float the_result, Float received_ms) -> Float { return (the_result - received_ms); }, net_session::Session_now_ms(self).Result, r.received_ms), kern::formula([](Float kQuietS) -> Float { return (kQuietS * 1000.0); }, kQuietS))};
  }

  void Session_update(net_Session& self, const net_PlayerState& me, Float time = 0.0, Float dt = 0.0, Bool send_pose = false) {
    Bool tick = false;
    List<Int_Bool_pair> list = List<Int_Bool_pair>{};
    List<Int_Bool_pair> take_events = List<Int_Bool_pair>{};
    U8 slot = 0;
    List<U8> w = List<U8>{};
    Bool send = false;
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Int at_2 = 0;
    Int kept_2 = 0;
    Int end_2 = 0;
    List<U8> w_2 = List<U8>{};
    I32 c = 0;
    Int max_clients = 0;
    Bool skip = false;
    Bool client_connected = false;
    I32 item = 0;
    I32 item_2 = 0;
    Fixed<I32, 2> items = Fixed<I32, 2>{};
    I32 ch = 0;
    List<net_Message> m = List<net_Message>{};
    List<net_Message> receive = List<net_Message>{};
    Int at_3 = 0;
    net_PlayerState s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
    List<U8> a = List<U8>{};
    Bool go = false;
    Bool go_2 = false;
    Int i = 0;
    List<U8> a_2 = List<U8>{};
    List<U8> a_3 = List<U8>{};
    List<U8> a_4 = List<U8>{};
    Bool both = false;
    net_ClientState state = net_ClientState::denied;
    net_ClientState state_2 = net_ClientState::denied;
    Bool both_2 = false;
    I32 item_3 = 0;
    I32 item_4 = 0;
    Fixed<I32, 2> items_2 = Fixed<I32, 2>{};
    I32 ch_2 = 0;
    List<net_Message> m_2 = List<net_Message>{};
    List<net_Message> receive_2 = List<net_Message>{};
    Bool go_3 = false;
    Bool connected = false;
    Bool connected_2 = false;
    Bool connected_3 = false;
    Int arg = 0;
    Bool send_2 = false;
    Int at_4 = 0;
    Int kept_3 = 0;
    Int end_3 = 0;
    Int at_5 = 0;
    Int kept_4 = 0;
    Int end_4 = 0;
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::none)) {
      return;
    }
    self.time_ = time;
    self.me_ = me;
    self.me_.slot = self.self_slot_;
    self.me_.t_ms = kern::to<Int>(kern::formula([](Float time) -> Float { return kern::math::trunc((time * 1000.0)); }, time));
    if (Session_protected_now(self, self.self_slot_, net_session::Session_now_ms(self).Result).Result) {
      self.me_.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::bor(flags, a); }, kern::to<Float>(self.me_.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kProtected)))));
    }
    engine_host_transport_advance_time(self.transport_, time);
    tick = math_compare<">=">(kern::formula([](Float time, Float send_clock_) -> Float { return (time - send_clock_); }, time, self.send_clock_), kSendInterval);
    if (tick) {
      self.send_clock_ = time;
    }
    if (enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)) {
      engine_server_advance_time(self.server_, time);
      engine_server_receive_packets(self.server_);
      engine_server_take_events(n199, self.server_);
      take_events = n199.result;
      list = take_events;
      for (auto it200 = flow_for(n200, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Bool_pair>(list))))); it200.next();) {
        slot = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list[n200.index].first)));
        if (list[n200.index].second) {
          w = List<U8>{};
          wire::put_u8(w, slot);
          wire::put_f64(w, self.lon_);
          wire::put_f64(w, self.lat_);
          wire::put_u8(w, kern::to<Int>(self.friendly_fire_));
          engine_server_send(n201, self.server_, list[n200.index].first, 0, 4, w);
          send = n201.result;
          list_add(self.pending_anchor_, slot);
          mp_rules_cpp::Scoreboard_ensure(self.scores_, slot);
          Session_send_scores(self);
          if (math_compare<"!=">(kern::to<Float>(self.ballot_.id), 0.0)) {
            Session_finish_ballot(self, mp_Ballot_Status::cancelled);
          }
        } else {
          at = 0;
          kept = at;
          end = list_length<net_Remote>(self.remotes_);
          while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
            if (logic_not(update_match(self.remotes_[at], slot).Result)) {
              if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
                var_take(n202, self.remotes_[at]);
                self.remotes_[kept] = n202.value;
              }
              kept += 1;
            }
            at += 1;
          }
          list_resize(self.remotes_, kept, net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0});
          at_2 = 0;
          kept_2 = at_2;
          end_2 = list_length<net_Session_SwarmParts>(self.swarm_parts_);
          while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2)))) {
            if (logic_not(update_match_2(self.swarm_parts_[at_2], slot).Result)) {
              if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_2))) {
                var_take(n203, self.swarm_parts_[at_2]);
                self.swarm_parts_[kept_2] = n203.value;
              }
              kept_2 += 1;
            }
            at_2 += 1;
          }
          list_resize(self.swarm_parts_, kept_2, net_Session_SwarmParts{0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{}});
          mp_rules_cpp::Scoreboard_remove(self.scores_, slot);
          n204 = jet_seats_cpp::Table_drop(self.seats_, slot);
          if (n204.Result) {
            Session_send_seats(self);
          }
          w_2 = List<U8>{};
          wire::put_u8(w_2, slot);
          Session_to_all(self, w_2, 0, 22, kNobody);
          Session_send_scores(self);
          if (math_compare<"!=">(kern::to<Float>(self.ballot_.id), 0.0)) {
            Session_finish_ballot(self, mp_Ballot_Status::cancelled);
          }
        }
      }
      c = 0;
      go_2 = true;
      while (loop(go_2)) {
        max_clients = engine_server_max_clients(self.server_);
        if (math_compare<"<">(kern::to<Float>(c), kern::to<Float>(max_clients))) {
          skip = false;
          client_connected = engine_server_client_connected(self.server_, c);
          if (logic_not(client_connected)) {
            skip = true;
          }
          if (logic_not(skip)) {
            item = 0;
            item_2 = 1;
            items = list_make<I32, "2">(item, item_2);
            for (auto it205 = flow_for(n205, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(items))))); it205.next();) {
              ch = items[n205.index];
              go = true;
              while (loop(go)) {
                engine_server_receive(n206, self.server_, c, ch);
                receive = n206.result;
                m = receive;
                if (math_compare<">">(kern::to<Float>(list_length<net_Message>(m)), 0.0)) {
                  if (math_compare<"==">(kern::to<Float>(m[0].type), 1.0)) {
                    at_3 = 0;
                    s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
                    n207 = unpack(m[0].data, at_3, s);
                    if (n207.Result) {
                      s.slot = kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c)));
                      if (Session_protected_now(self, s.slot, net_session::Session_now_ms(self).Result).Result) {
                        s.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::bor(flags, a); }, kern::to<Float>(s.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kProtected)))));
                      }
                      Session_on_pose(self, s);
                      n208 = pack(s);
                      a = n208.Result;
                      Session_to_all(self, a, 1, 2, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))));
                    }
                  } else {
                    Session_dispatch(self, m[0].data, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))), m[0].type);
                  }
                } else {
                  go = false;
                }
              }
            }
          }
          c += 1;
        } else {
          go_2 = false;
        }
      }
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<U8>(self.pending_anchor_))), 0.5))) {
        if (math_compare<">=">(kern::to<Float>(Session_remote_index(self, self.pending_anchor_[i]).Result), 0.0)) {
          Session_send_anchor(self, self.pending_anchor_[i]);
          list_remove_at(self.pending_anchor_, i);
        } else {
          i += 1;
        }
      }
      if (logic_and(tick, send_pose)) {
        n209 = pack(self.me_);
        a_2 = n209.Result;
        Session_to_all(self, a_2, 1, 2, kNobody);
        self.sent_poses_ = kern::to<Int>(kern::formula([](Float sent_poses_) -> Float { return (sent_poses_ + 1.0); }, kern::to<Float>(self.sent_poses_)));
      }
      if (math_compare<">">(kern::formula([](Float time, Float scores_clock_) -> Float { return (time - scores_clock_); }, time, self.scores_clock_), kScoresEvery)) {
        self.scores_clock_ = time;
        Session_send_scores(self);
      }
      if (math_compare<">">(kern::formula([](Float time, Float rules_clock_) -> Float { return (time - rules_clock_); }, time, self.rules_clock_), kRulesEvery)) {
        self.rules_clock_ = time;
        Session_send_rules(self);
      }
      if (math_compare<">">(kern::formula([](Float time, Float seats_clock_) -> Float { return (time - seats_clock_); }, time, self.seats_clock_), kSeatsEvery)) {
        self.seats_clock_ = time;
        Session_send_seats(self);
      }
      both = math_compare<"!=">(kern::to<Float>(self.ballot_.id), 0.0);
      if (both) {
        a_4 = Session_roster(self).Result;
        n210 = mp_rules_cpp::ballot_stale(self.ballot_, a_4, net_session::Session_now_ms(self).Result);
        both = n210.Result;
      }
      if (both) {
        Session_finish_ballot(self, mp_Ballot_Status::cancelled);
      }
      engine_server_send_packets(self.server_);
    } else {
      both_2 = logic_not(self.connect_pending_);
      if (both_2) {
        state_2 = engine_client_state(self.client_);
        both_2 = math_compare<"<">(kern::to<Float>(list_get<Int>(net_ClientState_numbers, enum_index<net_ClientState>(state_2))), kern::to<Float>(list_get<Int>(net_ClientState_numbers, enum_index<net_ClientState>(net_ClientState::disconnected))));
      }
      if (logic_and(both_2, math_compare<">=">(kern::formula([](Float time, Float connect_at_) -> Float { return (time - connect_at_); }, time, self.connect_at_), kReconnectS))) {
        self.connect_pending_ = true;
      }
      if (self.connect_pending_) {
        engine_client_connect(self.client_, self.server_addr_, time);
        self.connect_pending_ = false;
        self.connect_at_ = time;
        self.connects_ = kern::to<Int>(kern::formula([](Float connects_) -> Float { return (connects_ + 1.0); }, kern::to<Float>(self.connects_)));
      }
      engine_client_advance_time(self.client_, time);
      engine_client_receive_packets(self.client_);
      item_3 = 0;
      item_4 = 1;
      items_2 = list_make<I32, "2">(item_3, item_4);
      for (auto it211 = flow_for(n211, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(items_2))))); it211.next();) {
        ch_2 = items_2[n211.index];
        go_3 = true;
        while (loop(go_3)) {
          engine_client_receive(n212, self.client_, ch_2);
          receive_2 = n212.result;
          m_2 = receive_2;
          if (math_compare<">">(kern::to<Float>(list_length<net_Message>(m_2)), 0.0)) {
            Session_deliver(self, m_2[0].data, m_2[0].type);
          } else {
            go_3 = false;
          }
        }
      }
      connected = engine_client_connected(self.client_);
      if (logic_and(logic_and(connected, self.have_origin_), logic_not(self.ever_in_))) {
        self.ever_in_ = true;
        self.in_since_ = net_session::Session_now_ms(self).Result;
      }
      connected_2 = engine_client_connected(self.client_);
      if (logic_and(connected_2, math_compare<">=">(kern::to<Float>(Session_remote_index(self, kHost).Result), 0.0))) {
        list_clear(self.mates_);
        for (auto it213 = flow_for(n213, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(self.remotes_))))); it213.next();) {
          if (math_compare<"!=">(kern::to<Float>(self.remotes_[n213.index].state.slot), kern::to<Float>(kHost))) {
            list_add(self.mates_, self.remotes_[n213.index].state.slot);
          }
        }
      }
      connected_3 = engine_client_connected(self.client_);
      if (logic_and(logic_and(logic_and(connected_3, self.have_origin_), tick), send_pose)) {
        arg = self.client_;
        n214 = pack(self.me_);
        engine_client_send(n215, arg, 1, 1, n214.Result);
        send_2 = n215.result;
        self.sent_poses_ = kern::to<Int>(kern::formula([](Float sent_poses_) -> Float { return (sent_poses_ + 1.0); }, kern::to<Float>(self.sent_poses_)));
      }
      if (logic_and(math_compare<"!=">(kern::to<Float>(self.remote_ballot_.id), 0.0), math_compare<">=">(net_session::Session_now_ms(self).Result, self.remote_ballot_.expires_ms))) {
        self.remote_ballot_ = struct_make<mp_Ballot>(0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending);
        self.vote_text_ = "Abstimmung abgelaufen";
      }
      engine_client_send_packets(self.client_);
    }
    at_4 = 0;
    kept_3 = at_4;
    end_3 = list_length<Int_Float_pair>(self.protected_);
    while (loop(math_compare<"<">(kern::to<Float>(at_4), kern::to<Float>(end_3)))) {
      n216 = Session_update_match(self, self.protected_[at_4]);
      if (logic_not(n216.Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_3), kern::to<Float>(at_4))) {
          var_take(n217, self.protected_[at_4]);
          self.protected_[kept_3] = n217.value;
        }
        kept_3 += 1;
      }
      at_4 += 1;
    }
    list_resize(self.protected_, kept_3, Int_Float_pair{0, 0.0});
    at_5 = 0;
    kept_4 = at_5;
    end_4 = list_length<net_Remote>(self.remotes_);
    while (loop(math_compare<"<">(kern::to<Float>(at_5), kern::to<Float>(end_4)))) {
      n218 = Session_update_match_2(self, self.remotes_[at_5]);
      if (logic_not(n218.Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_4), kern::to<Float>(at_5))) {
          var_take(n219, self.remotes_[at_5]);
          self.remotes_[kept_4] = n219.value;
        }
        kept_4 += 1;
      }
      at_5 += 1;
    }
    list_resize(self.remotes_, kept_4, net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0});
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
  struct { Int result = 0; } n66;
  struct { Int result = 0; } n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct { Bool result = false; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct : kern::Node<72> { Int index = 0; } n72;
  struct : kern::Node<73> { Int index = 0; } n73;
  mp_rules_cpp::assign_colorsResult n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  mp_rules_cpp::color_ofResult n76;
  struct { U8 value = 0; } n77;
  struct { U8 value = 0; } n78;
  struct { U8 value = 0; } n79;
  struct { U8 value = 0; } n80;
  struct : kern::Node<81> { Int index = 0; } n81;
  struct : kern::Node<82> { Int index = 0; } n82;
  struct : kern::Node<83> { Int index = 0; } n83;
  struct { Bool result = false; } n84;
  wire::Reader_get_u8Result n85;
  wire::Reader_get_u8Result n86;
  wire::Reader_get_u8Result n87;
  jet_seats_cpp::Table_claimResult n89;
  jet_seats_cpp::Table_releaseResult n90;
  struct : kern::Node<91> { Int index = 0; } n91;
  unpack_fxResult n92;
  pack_fxResult n93;
  unpack_swarmResult n94;
  pack_swarmResult n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct : kern::Node<97> { Int index = 0; } n97;
  struct { List<net_SwarmUnit> value = List<net_SwarmUnit>{}; } n98;
  unpack_swarm_hitResult n99;
  pack_swarm_hitResult n100;
  unpack_hitResult n101;
  struct : kern::Node<102> { Int index = 0; } n102;
  mp_rules_cpp::judge_hitResult n103;
  pack_hitResult n104;
  wire::Reader_get_u8Result n105;
  wire::Reader_get_u8Result n106;
  wire::Reader_get_u8Result n107;
  struct : kern::Node<108> { Int index = 0; } n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  wire::Reader_get_u8Result n110;
  wire::Reader_get_u8Result n111;
  wire::Reader_get_vec3Result n112;
  wire::Reader_get_u32Result n113;
  wire::Reader_get_u8Result n114;
  wire::Reader_get_u32Result n115;
  wire::Reader_get_u8Result n116;
  wire::Reader_get_bytesResult n117;
  struct : kern::Node<118> { Int index = 0; } n118;
  wire::Reader_get_u8Result n119;
  wire::Reader_get_u8Result n120;
  mp_rules_cpp::create_ballotResult n121;
  struct : kern::Node<122> { Int index = 0; } n122;
  wire::Reader_get_u32Result n123;
  wire::Reader_get_u8Result n124;
  mp_rules_cpp::cast_vote_2Result n125;
  struct { Bool result = false; } n126;
  pack_fxResult n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  pack_swarmResult n129;
  struct { Bool result = false; } n131;
  struct { Bool result = false; } n132;
  struct { Bool result = false; } n133;
  struct { Bool result = false; } n134;
  pack_swarm_hitResult n135;
  pack_hitResult n136;
  struct : kern::Node<137> { Int index = 0; } n137;
  mp_rules_cpp::group_spawn_centerResult n138;
  jet_seats_cpp::Table_claimResult n139;
  jet_seats_cpp::Table_releaseResult n140;
  wire::Reader_get_f64Result n141;
  wire::Reader_get_f64Result n142;
  wire::Reader_get_u8Result n143;
  wire::Reader_get_f64Result n144;
  wire::Reader_get_f64Result n145;
  wire::Reader_get_u8Result n146;
  unpackResult n147;
  struct : kern::Node<148> { Int index = 0; } n148;
  mp_rules_cpp::PoseTimeline_pushResult n149;
  mp_rules_cpp::PoseTimeline_pushResult n150;
  mp_rules_cpp::NavTimeline_pushResult n151;
  unpack_fxResult n152;
  unpack_swarmResult n153;
  unpack_swarm_hitResult n154;
  wire::Reader_get_u8Result n155;
  wire::Reader_get_bytesResult n156;
  wire::Reader_get_u8Result n157;
  wire::Reader_get_bytesResult n158;
  wire::Reader_get_u32Result n159;
  wire::Reader_get_u8Result n160;
  wire::Reader_get_u8Result n161;
  wire::Reader_get_u8Result n162;
  wire::Reader_get_u8Result n163;
  jet_seats_cpp::Table_loadResult n164;
  unpack_hitResult n165;
  unpack_hitResult n166;
  wire::Reader_get_u8Result n167;
  wire::Reader_get_u8Result n168;
  wire::Reader_get_u8Result n169;
  wire::Reader_get_u8Result n170;
  wire::Reader_get_u8Result n171;
  wire::Reader_get_u8Result n172;
  wire::Reader_get_u16Result n173;
  wire::Reader_get_u16Result n174;
  wire::Reader_get_u8Result n175;
  wire::Reader_get_f32Result n176;
  struct : kern::Node<177> { Int index = 0; } n177;
  wire::Reader_get_vec3Result n178;
  wire::Reader_get_u8Result n179;
  wire::Reader_get_u8Result n180;
  wire::Reader_get_vec3Result n181;
  wire::Reader_get_u32Result n182;
  wire::Reader_get_u8Result n183;
  wire::Reader_get_u32Result n184;
  wire::Reader_get_u8Result n185;
  wire::Reader_get_bytesResult n186;
  wire::Reader_get_u8Result n187;
  wire::Reader_get_u16Result n188;
  wire::Reader_get_u32Result n189;
  wire::Reader_get_u8Result n190;
  wire::Reader_get_u8Result n191;
  wire::Reader_get_u8Result n192;
  wire::Reader_get_u8Result n193;
  wire::Reader_get_u32Result n194;
  wire::Reader_get_u8Result n195;
  wire::Reader_get_u8Result n196;
  struct { net_Remote value = net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0}; } n197;
  struct { net_Session_SwarmParts value = net_Session_SwarmParts{0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{}}; } n198;
  struct { List<Int_Bool_pair> result = List<Int_Bool_pair>{}; } n199;
  struct : kern::Node<200> { Int index = 0; } n200;
  struct { Bool result = false; } n201;
  struct { net_Remote value = net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0}; } n202;
  struct { net_Session_SwarmParts value = net_Session_SwarmParts{0, 0, List<Int64>{Int64{0x0}, Int64{0x0}}, -1, List<net_SwarmUnit>{}}; } n203;
  jet_seats_cpp::Table_dropResult n204;
  struct : kern::Node<205> { Int index = 0; } n205;
  struct { List<net_Message> result = List<net_Message>{}; } n206;
  unpackResult n207;
  packResult n208;
  packResult n209;
  mp_rules_cpp::ballot_staleResult n210;
  struct : kern::Node<211> { Int index = 0; } n211;
  struct { List<net_Message> result = List<net_Message>{}; } n212;
  struct : kern::Node<213> { Int index = 0; } n213;
  packResult n214;
  struct { Bool result = false; } n215;
  Session_update_matchResult n216;
  struct { Int_Float_pair value = Int_Float_pair{0, 0.0}; } n217;
  Session_update_match_2Result n218;
  struct { net_Remote value = net_Remote{net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}, mp_NavTimeline{List<mp_NavTimeline_Snap>{}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, -1e+18, 0.0, 100.0, mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}, 0.0}; } n219;
};
