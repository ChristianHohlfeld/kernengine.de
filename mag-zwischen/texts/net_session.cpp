struct net_session : virtual kern::Library, virtual engine_net_transport, virtual jet_seats, virtual mp_rules {
  // ---- types ----
  KERN_STRUCT(net_Fx) { U8 kind = 0; U8 owner = 0; Vector p = Vector{0.0, 0.0, 0.0}; Vector dir = Vector{0.0, 0.0, 0.0}; Float a = 0.0; };
  KERN_ENUM(net_Fx_Kind, shot, explosion, impact, rocket, cow_death, push, lamp, strike, flyover, mil_tracer, mil_shell, npc_death, melee);
  KERN_STRUCT(net_Hit) { U8 target = 0; U8 by = 254; U8 weapon = 0; U8 zone = 0; Float damage = 0.0; Vector from = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(net_Kill) { U8 victim = 0; U8 by = 254; U8 weapon = 0; U8 zone = 0; };
  KERN_STRUCT(net_PingEvent) { U8 owner = 0; Bool on = false; Vector p = Vector{0.0, 0.0, 0.0}; U32 color = 0; };
  KERN_ENUM(net_PlayerFlag, kDead, kArmed, kBus, kGearDown, kPoliceCar, kDroneOut, kJetOut, kProtected, kTalking, kCockpit, kJetpack, kPassenger, kSeated);
  KERN_ENUM(net_PlayerMode, foot, drone, car, jet, parachute);
  KERN_STRUCT(net_Session_Blob) { U8 from = 0; List<U8> data = List<U8>{}; };
  KERN_ENUM(net_Session_Role, none, host, client);
  KERN_ENUM(net_SwarmFlag, kUnitDead, kOfficerAims, kCruiserWreck, kCruiserBurns, kEnemyTypeMask);
  KERN_ENUM(net_SwarmKind, zombie, heli, tank, soldier, officer, cruiser, police_drone, enemy);
  KERN_STRUCT(net_VoiceFrame) { U8 from = 0; U32 seq = 0; Bool everywhere = false; List<U8> data = List<U8>{}; };
  KERN_STRUCT(net_PlayerState) { U8 slot = 0; net_PlayerMode mode = net_PlayerMode::foot; U8 color = 0; U16 flags = 0; U32 t_ms = 0; Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector avatar = Vector{0.0, 0.0, 0.0}; Float avatar_yaw = 0.0; Vector drone_pos = Vector{0.0, 0.0, 0.0}; Vector jet_pos = Vector{0.0, 0.0, 0.0}; Quat drone_rot = Quat{0.0, 0.0, 0.0, 1.0}; Quat jet_rot = Quat{0.0, 0.0, 0.0, 1.0}; Float yaw = 0.0; Float pitch = 0.0; Float speed = 0.0; Float health = 100.0; Float thrust = 0.0; U8 stars = 0; U8 weapon = 0; Float day = -1.0; Fixed<I8, 6> hand = List<I8>{0, 0, 0, 0, 0, 0}; U8 hands = 0; U8 car = 255; };
  KERN_STRUCT(net_SwarmHit) { U8 from = 0; U8 to = 0; U16 id = 0; net_SwarmKind kind = net_SwarmKind::zombie; Float damage = 0.0; };
  KERN_STRUCT(net_SwarmUnit) { U16 id = 0; net_SwarmKind kind = net_SwarmKind::zombie; U8 flags = 0; Vector p = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float aux = 0.0; };
  KERN_STRUCT(net_Session_SwarmParts) { U8 owner = 0; U8 serial = 0; Fixed<Int64, 2> have = List<Int64>{Int64{0x0}, Int64{0x0}}; I32 last = -1; List<net_SwarmUnit> units = List<net_SwarmUnit>{}; };
  KERN_STRUCT(net_Swarm) { U8 owner = 0; List<net_SwarmUnit> units = List<net_SwarmUnit>{}; };
  KERN_STRUCT(net_Remote) { net_PlayerState state = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}; mp_PoseTimeline body = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; mp_PoseTimeline person = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; mp_NavTimeline nav = mp_NavTimeline{List<mp_NavTimeline_Snap>{}}; Vector shown = Vector{0.0, 0.0, 0.0}; Vector person_shown = Vector{0.0, 0.0, 0.0}; Quat shown_rotation = Quat{0.0, 0.0, 0.0, 1.0}; Float received_ms = -1e+18; Float phase = 0.0; Float shown_hp = 100.0; mp_VoiceStream voice = mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}; Float speaking_until = 0.0; };
  KERN_STRUCT(net_Session) { net_Session_Role role_ = net_Session_Role::none; Int transport_ = 0; Int server_ = 0; Int client_ = 0; net_Address server_addr_ = net_Address{Int64{0x0}}; List<net_Remote> remotes_ = List<net_Remote>{}; List<net_Hit> hits_ = List<net_Hit>{}; List<net_Hit> confirms_ = List<net_Hit>{}; List<net_Kill> kills_ = List<net_Kill>{}; List<net_Fx> fx_ = List<net_Fx>{}; List<net_Swarm> swarms_ = List<net_Swarm>{}; List<net_SwarmHit> swarm_hits_ = List<net_SwarmHit>{}; List<U8_row> world_ = List<U8_row>{}; List<net_PingEvent> pings_ = List<net_PingEvent>{}; List<net_VoiceFrame> voice_ = List<net_VoiceFrame>{}; List<U8> pending_anchor_ = List<U8>{}; Float lon_ = 0.0; Float lat_ = 0.0; Float time_ = 0.0; Float send_clock_ = 0.0; Float scores_clock_ = 0.0; Float rules_clock_ = 0.0; Bool have_origin_ = false; Bool connect_pending_ = false; Bool anchor_ready_ = false; Bool ever_in_ = false; Float connect_at_ = 0.0; U32 connects_ = 0; Float in_since_ = 0.0; List<U8> mates_ = List<U8>{}; Vector anchor_ = Vector{0.0, 0.0, 0.0}; U8 self_slot_ = 255; net_PlayerState me_ = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}; mp_Scoreboard scores_ = mp_Scoreboard{List<mp_Score>{}}; Bool friendly_fire_ = false; List<net_Session_Blob> destroy_ = List<net_Session_Blob>{}; List<net_Session_Blob> toss_ = List<net_Session_Blob>{}; List<net_Session_Blob> proj_ = List<net_Session_Blob>{}; List<net_Session_Blob> town_ = List<net_Session_Blob>{}; List<net_Session_SwarmParts> swarm_parts_ = List<net_Session_SwarmParts>{}; U8 swarm_serial_ = 0; U16 world_rules_ = 0; Bool have_world_rules_ = false; List<Int_Float_pair> protected_ = List<Int_Float_pair>{}; mp_Ballot ballot_ = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; mp_Ballot remote_ballot_ = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; U32 ballot_serial_ = 0; Text vote_text_ = "Aenderungen nur einstimmig"; U32 sent_poses_ = 0; U32 send_refused_ = 0; U32 refused_said_ = 0; seats_Table seats_ = seats_Table{List<seats_Row>{}, 0}; Float seats_clock_ = 0.0; };

  // ---- functions ----
  struct Session_activeResult { Bool Result = false; };
  KERN_PURE Session_activeResult Session_active(const net_Session& self) {
    return {logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none))};
  }

  struct Session_hostingResult { Bool Result = false; };
  KERN_PURE Session_hostingResult Session_hosting(const net_Session& self) {
    return {enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)};
  }

  struct Session_self_slotResult { U8 Result = 0; };
  KERN_PURE Session_self_slotResult Session_self_slot(const net_Session& self) {
    return {self.self_slot_};
  }

  /// ---- host page ----
  void Session_push(net_Session& self, const List<U8>& bytes, Int64 from = Int64{0x0}) {
    engine_host_transport_push_incoming(self.transport_, struct_make<net_Address>(from), bytes, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::wrap32s(a)); }, kern::to<Float>(list_length<U8>(bytes)))));
  }

  struct Session_takeResult { Bool Result = false; };
  Session_takeResult Session_take(net_Session& self, Int64& to, List<U8>& bytes) {
    net_Address a = net_Address{Int64{0x0}};
    Bool ok = false;
    Bool take_outgoing = false;
    a = net_Address{Int64{0x0}};
    engine_host_transport_take_outgoing(n10, self.transport_, a, bytes);
    take_outgoing = n10.result;
    a = n10.to;
    bytes = n10.data;
    ok = take_outgoing;
    to = a.id;
    return {ok};
  }

  struct Session_take_hitsResult { List<net_Hit> Result = List<net_Hit>{}; };
  /// ---- incoming (since the last call) ----
  Session_take_hitsResult Session_take_hits(net_Session& self) {
    var_take(n11, self.hits_);
    return {n11.value};
  }

  struct Session_take_confirmsResult { List<net_Hit> Result = List<net_Hit>{}; };
  Session_take_confirmsResult Session_take_confirms(net_Session& self) {
    var_take(n12, self.confirms_);
    return {n12.value};
  }

  struct Session_take_killsResult { List<net_Kill> Result = List<net_Kill>{}; };
  Session_take_killsResult Session_take_kills(net_Session& self) {
    var_take(n13, self.kills_);
    return {n13.value};
  }

  struct Session_take_fxResult { List<net_Fx> Result = List<net_Fx>{}; };
  Session_take_fxResult Session_take_fx(net_Session& self) {
    var_take(n14, self.fx_);
    return {n14.value};
  }

  struct Session_take_swarmsResult { List<net_Swarm> Result = List<net_Swarm>{}; };
  Session_take_swarmsResult Session_take_swarms(net_Session& self) {
    var_take(n15, self.swarms_);
    return {n15.value};
  }

  struct Session_take_swarm_hitsResult { List<net_SwarmHit> Result = List<net_SwarmHit>{}; };
  Session_take_swarm_hitsResult Session_take_swarm_hits(net_Session& self) {
    var_take(n16, self.swarm_hits_);
    return {n16.value};
  }

  struct Session_take_worldResult { List<U8_row> Result = List<U8_row>{}; };
  Session_take_worldResult Session_take_world(net_Session& self) {
    var_take(n17, self.world_);
    return {n17.value};
  }

  struct Session_take_destroyResult { List<net_Session_Blob> Result = List<net_Session_Blob>{}; };
  Session_take_destroyResult Session_take_destroy(net_Session& self) {
    var_take(n18, self.destroy_);
    return {n18.value};
  }

  struct Session_take_townResult { List<net_Session_Blob> Result = List<net_Session_Blob>{}; };
  Session_take_townResult Session_take_town(net_Session& self) {
    var_take(n19, self.town_);
    return {n19.value};
  }

  struct Session_take_tossResult { List<net_Session_Blob> Result = List<net_Session_Blob>{}; };
  Session_take_tossResult Session_take_toss(net_Session& self) {
    var_take(n20, self.toss_);
    return {n20.value};
  }

  struct Session_take_projResult { List<net_Session_Blob> Result = List<net_Session_Blob>{}; };
  Session_take_projResult Session_take_proj(net_Session& self) {
    var_take(n21, self.proj_);
    return {n21.value};
  }

  struct Session_take_pingsResult { List<net_PingEvent> Result = List<net_PingEvent>{}; };
  Session_take_pingsResult Session_take_pings(net_Session& self) {
    var_take(n22, self.pings_);
    return {n22.value};
  }

  struct Session_take_voiceResult { List<net_VoiceFrame> Result = List<net_VoiceFrame>{}; };
  Session_take_voiceResult Session_take_voice(net_Session& self) {
    var_take(n23, self.voice_);
    return {n23.value};
  }

  struct Session_take_anchorResult { Bool Result = false; };
  /// A spawn anchor from the host for this (joining) player.
  Session_take_anchorResult Session_take_anchor(net_Session& self, Vector& p) {
    if (logic_not(self.anchor_ready_)) {
      return {false};
    }
    self.anchor_ready_ = false;
    p = self.anchor_;
    return {true};
  }

  struct Session_friendly_fireResult { Bool Result = false; };
  KERN_PURE Session_friendly_fireResult Session_friendly_fire(const net_Session& self) {
    return {self.friendly_fire_};
  }

  void Session_set_friendly_fire_initial(net_Session& self, Bool on = false) {
    if (logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::client))) {
      self.friendly_fire_ = on;
    }
  }

  struct Session_world_rulesResult { Bool Result = false; };
  Session_world_rulesResult Session_world_rules(const net_Session& self, U16& mask) {
    if (logic_not(self.have_world_rules_)) {
      return {false};
    }
    mask = self.world_rules_;
    return {true};
  }

  struct Session_now_msResult { Float Result = 0.0; };
  KERN_PURE Session_now_msResult Session_now_ms(const net_Session& self) {
    return {kern::formula([](Float time_) -> Float { return (time_ * 1000.0); }, self.time_)};
  }

  struct Session_vote_pendingResult { Bool Result = false; };
  KERN_PURE Session_vote_pendingResult Session_vote_pending(const net_Session& self) {
    return {math_compare<"!=">(kern::to<Float>(self.remote_ballot_.id), 0.0)};
  }

  struct Session_sent_posesResult { U32 Result = 0; };
  KERN_PURE Session_sent_posesResult Session_sent_poses(const net_Session& self) {
    return {self.sent_poses_};
  }

  struct Session_connectsResult { U32 Result = 0; };
  /// How often this client asked the host to let it in (1: it got in at the first try).
  KERN_PURE Session_connectsResult Session_connects(const net_Session& self) {
    return {self.connects_};
  }

  struct Session_send_refusedResult { U32 Result = 0; };
  /// Messages the transport did not take (too large, queue full): must stay 0.
  KERN_PURE Session_send_refusedResult Session_send_refused(const net_Session& self) {
    return {self.send_refused_};
  }

  // ---- what nodes give and keep ----
  struct { Bool result = false; net_Address to = net_Address{Int64{0x0}}; List<U8> data = List<U8>{}; } n10;
  struct { List<net_Hit> value = List<net_Hit>{}; } n11;
  struct { List<net_Hit> value = List<net_Hit>{}; } n12;
  struct { List<net_Kill> value = List<net_Kill>{}; } n13;
  struct { List<net_Fx> value = List<net_Fx>{}; } n14;
  struct { List<net_Swarm> value = List<net_Swarm>{}; } n15;
  struct { List<net_SwarmHit> value = List<net_SwarmHit>{}; } n16;
  struct { List<U8_row> value = List<U8_row>{}; } n17;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n18;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n19;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n20;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n21;
  struct { List<net_PingEvent> value = List<net_PingEvent>{}; } n22;
  struct { List<net_VoiceFrame> value = List<net_VoiceFrame>{}; } n23;
};
