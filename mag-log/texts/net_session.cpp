struct net_session : virtual kern::Library, virtual engine_net_transport, virtual jet_seats, virtual mp_rules {
  // ---- types ----
  KERN_STRUCT(net_Fx) { Int kind = 0; Int owner = 0; Vector p = Vector{0.0, 0.0, 0.0}; Vector dir = Vector{0.0, 0.0, 0.0}; Float a = 0.0; };
  KERN_STRUCT(net_Hit) { Int target = 0; Int by = 254; Int weapon = 0; Int zone = 0; Float damage = 0.0; Vector from = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(net_Kill) { Int victim = 0; Int by = 254; Int weapon = 0; Int zone = 0; };
  KERN_STRUCT(net_PingEvent) { Int owner = 0; Bool on = false; Vector p = Vector{0.0, 0.0, 0.0}; Int color = 0; };
  KERN_ENUM(net_PlayerFlag, kDead, kArmed, kBus, kGearDown, kPoliceCar, kDroneOut, kJetOut, kProtected, kTalking, kCockpit, kJetpack, kPassenger, kSeated);
  KERN_ENUM(net_PlayerMode, foot, drone, car, jet, parachute);
  KERN_STRUCT(net_Session_Blob) { Int from = 0; List<Int> data = List<Int>{}; };
  KERN_ENUM(net_Session_Role, none, host, client);
  KERN_ENUM(net_SwarmKind, zombie, heli, tank, soldier, officer, cruiser, police_drone, enemy);
  KERN_STRUCT(net_VoiceFrame) { Int from = 0; Int seq = 0; Bool everywhere = false; List<Int> data = List<Int>{}; };
  KERN_STRUCT(net_PlayerState) { Int slot = 0; net_PlayerMode mode = net_PlayerMode::foot; Int color = 0; Int flags = 0; Int t_ms = 0; Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector avatar = Vector{0.0, 0.0, 0.0}; Float avatar_yaw = 0.0; Vector drone_pos = Vector{0.0, 0.0, 0.0}; Vector jet_pos = Vector{0.0, 0.0, 0.0}; Quat drone_rot = Quat{0.0, 0.0, 0.0, 1.0}; Quat jet_rot = Quat{0.0, 0.0, 0.0, 1.0}; Float yaw = 0.0; Float pitch = 0.0; Float speed = 0.0; Float health = 100.0; Float thrust = 0.0; Int stars = 0; Int weapon = 0; Float day = -1.0; Fixed<Int, 6> hand = List<Int>{0, 0, 0, 0, 0, 0}; Int hands = 0; Int car = 255; };
  KERN_STRUCT(net_SwarmHit) { Int from = 0; Int to = 0; Int id = 0; net_SwarmKind kind = net_SwarmKind::zombie; Float damage = 0.0; };
  KERN_STRUCT(net_SwarmUnit) { Int id = 0; net_SwarmKind kind = net_SwarmKind::zombie; Int flags = 0; Vector p = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float aux = 0.0; };
  KERN_STRUCT(net_Session_SwarmParts) { Int owner = 0; Int serial = 0; Fixed<Int64, 2> have = List<Int64>{Int64{0x0}, Int64{0x0}}; Int last = -1; List<net_SwarmUnit> units = List<net_SwarmUnit>{}; };
  KERN_STRUCT(net_Swarm) { Int owner = 0; List<net_SwarmUnit> units = List<net_SwarmUnit>{}; };
  KERN_STRUCT(net_Remote) { net_PlayerState state = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}; mp_PoseTimeline body = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; mp_PoseTimeline person = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; mp_NavTimeline nav = mp_NavTimeline{List<mp_NavTimeline_Snap>{}}; Vector shown = Vector{0.0, 0.0, 0.0}; Vector person_shown = Vector{0.0, 0.0, 0.0}; Quat shown_rotation = Quat{0.0, 0.0, 0.0, 1.0}; Float received_ms = -1e+18; Float phase = 0.0; Float shown_hp = 100.0; mp_VoiceStream voice = mp_VoiceStream{List<mp_VoiceStream_Chunk>{}, 0.0, 0.0, 0, false}; Float speaking_until = 0.0; };
  KERN_STRUCT(net_Session) { net_Session_Role role_ = net_Session_Role::none; Int transport_ = 0; Int server_ = 0; Int client_ = 0; net_Address server_addr_ = net_Address{Int64{0x0}}; List<net_Remote> remotes_ = List<net_Remote>{}; List<net_Hit> hits_ = List<net_Hit>{}; List<net_Hit> confirms_ = List<net_Hit>{}; List<net_Kill> kills_ = List<net_Kill>{}; List<net_Fx> fx_ = List<net_Fx>{}; List<net_Swarm> swarms_ = List<net_Swarm>{}; List<net_SwarmHit> swarm_hits_ = List<net_SwarmHit>{}; List<Int_row> world_ = List<Int_row>{}; List<net_PingEvent> pings_ = List<net_PingEvent>{}; List<net_VoiceFrame> voice_ = List<net_VoiceFrame>{}; List<Int> pending_anchor_ = List<Int>{}; Float lon_ = 0.0; Float lat_ = 0.0; Float time_ = 0.0; Float send_clock_ = 0.0; Float scores_clock_ = 0.0; Float rules_clock_ = 0.0; Bool have_origin_ = false; Bool connect_pending_ = false; Bool anchor_ready_ = false; Bool ever_in_ = false; Float connect_at_ = 0.0; Int connects_ = 0; Float in_since_ = 0.0; List<Int> mates_ = List<Int>{}; Vector anchor_ = Vector{0.0, 0.0, 0.0}; Int self_slot_ = 255; net_PlayerState me_ = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}; mp_Scoreboard scores_ = mp_Scoreboard{List<mp_Score>{}}; Bool friendly_fire_ = false; List<net_Session_Blob> destroy_ = List<net_Session_Blob>{}; List<net_Session_Blob> toss_ = List<net_Session_Blob>{}; List<net_Session_Blob> proj_ = List<net_Session_Blob>{}; List<net_Session_Blob> town_ = List<net_Session_Blob>{}; List<net_Session_SwarmParts> swarm_parts_ = List<net_Session_SwarmParts>{}; Int swarm_serial_ = 0; Int world_rules_ = 0; Bool have_world_rules_ = false; List<Int_Float_pair> protected_ = List<Int_Float_pair>{}; mp_Ballot ballot_ = mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}; mp_Ballot remote_ballot_ = mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}; Int ballot_serial_ = 0; Text vote_text_ = "Aenderungen nur einstimmig"; Int sent_poses_ = 0; Int send_refused_ = 0; Int refused_said_ = 0; seats_Table seats_ = seats_Table{List<seats_Row>{}, 0}; Float seats_clock_ = 0.0; };

  // ---- functions ----
  struct Session_activeResult { Bool Result = false; };
  KERN_PURE Session_activeResult Session_active(const net_Session& self) {
    return {logic_not(enum_equal<net_Session_Role>(self.role_, net_Session_Role::none))};
  }

  struct Session_hostingResult { Bool Result = false; };
  KERN_PURE Session_hostingResult Session_hosting(const net_Session& self) {
    return {enum_equal<net_Session_Role>(self.role_, net_Session_Role::host)};
  }

  struct Session_self_slotResult { Int Result = 0; };
  KERN_PURE Session_self_slotResult Session_self_slot(const net_Session& self) {
    return {self.self_slot_};
  }

  struct Session_takeResult { Bool Result = false; };
  Session_takeResult Session_take(net_Session& self, Int64& to, List<Int>& bytes) {
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

  struct Session_take_worldResult { List<Int_row> Result = List<Int_row>{}; };
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
  Session_world_rulesResult Session_world_rules(const net_Session& self, Int& mask) {
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

  struct Session_sent_posesResult { Int Result = 0; };
  KERN_PURE Session_sent_posesResult Session_sent_poses(const net_Session& self) {
    return {self.sent_poses_};
  }

  struct Session_connectsResult { Int Result = 0; };
  /// How often this client asked the host to let it in (1: it got in at the first try).
  KERN_PURE Session_connectsResult Session_connects(const net_Session& self) {
    return {self.connects_};
  }

  struct Session_send_refusedResult { Int Result = 0; };
  /// Messages the transport did not take (too large, queue full): must stay 0.
  KERN_PURE Session_send_refusedResult Session_send_refused(const net_Session& self) {
    return {self.send_refused_};
  }

  // ---- what nodes give and keep ----
  struct { Bool result = false; net_Address to = net_Address{Int64{0x0}}; List<Int> data = List<Int>{}; } n10;
  struct { List<net_Hit> value = List<net_Hit>{}; } n11;
  struct { List<net_Hit> value = List<net_Hit>{}; } n12;
  struct { List<net_Kill> value = List<net_Kill>{}; } n13;
  struct { List<net_Fx> value = List<net_Fx>{}; } n14;
  struct { List<net_Swarm> value = List<net_Swarm>{}; } n15;
  struct { List<net_SwarmHit> value = List<net_SwarmHit>{}; } n16;
  struct { List<Int_row> value = List<Int_row>{}; } n17;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n18;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n19;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n20;
  struct { List<net_Session_Blob> value = List<net_Session_Blob>{}; } n21;
  struct { List<net_PingEvent> value = List<net_PingEvent>{}; } n22;
  struct { List<net_VoiceFrame> value = List<net_VoiceFrame>{}; } n23;
};
