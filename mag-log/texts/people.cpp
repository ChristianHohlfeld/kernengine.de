struct people : virtual kern::Library, virtual wanted {
  // ---- types ----
  KERN_STRUCT(People_Fall) { Int index = 0; Vector push = Vector{0.0, 0.0, 0.0}; Bool car = false; Bool alive = false; };
  KERN_ENUM(People_HurtBy, round_, blow, vehicle);
  KERN_ENUM(People_Kind, citizen, officer, soldier, zombie, enemy);
  KERN_STRUCT(People_RemoteHit) { Int index = 0; Float damage = 0.0; };
  KERN_ENUM(People_State, walk, wait, flee, down, gone, officer, fight);
  KERN_STRUCT(CrimeReport) { wanted_Crime crime = wanted_Crime::person; Vector at = Vector{0.0, 0.0, 0.0}; Bool witnessed = false; };
  KERN_STRUCT(People_Death) { Int index = 0; People_Kind kind = People_Kind::citizen; Bool by_player = false; Bool head = false; };
  KERN_STRUCT(People_Hurt) { Int net = 0; Int seed = 0; Float damage = 0.0; People_HurtBy by = People_HurtBy::round_; Vector push = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(People_Person) { People_Kind kind = People_Kind::citizen; People_State state = People_State::walk; Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float speed = 0.0; Float v = 0.0; Float hp = 100.0; Float phase = 0.0; Vector shirt = Vector{0.0, 0.0, 0.0}; Vector pants = Vector{0.0, 0.0, 0.0}; Vector skin = Vector{0.0, 0.0, 0.0}; Int lane = -1; Int seg = 0; Int dir = 1; Float along = 0.0; Float side = 1.0; Float off = 4.0; Float flee_until = 0.0; Float burst_until = 0.0; Float flee_speed = 0.0; Float hands_up_until = 0.0; Float shaken_until = 0.0; Float wait_until = 0.0; Vector flee_dir = Vector{0.0, 0.0, 0.0}; Float fitness = 1.0; Float down_at = 0.0; Vector fall_dir = Vector{0.0, 0.0, 0.0}; Int unit = -1; Bool aiming = false; Float aim_pitch = 0.0; Int held = -1; Bool hands = false; Vector hand_l = Vector{0.0, 0.0, 0.0}; Vector hand_r = Vector{0.0, 0.0, 0.0}; Int punch = 0; Int seed = 0; Float provoked_at = -99.0; Float fight_until = 0.0; Float last_attack = -99.0; Bool provoked_hit = false; Int role = 0; Float knocked_until = 0.0; Float hurt_at = -99.0; Bool remote = false; Int net = -1; Vector net_to = Vector{0.0, 0.0, 0.0}; Float net_at = 0.0; };
  KERN_STRUCT(People) { List<People_Person> people_ = List<People_Person>{}; List<CrimeReport> reports_ = List<CrimeReport>{}; List<People_Death> deaths_ = List<People_Death>{}; List<People_Fall> falls_ = List<People_Fall>{}; List<People_RemoteHit> remote_hits_ = List<People_RemoteHit>{}; List<People_Hurt> hurts_ = List<People_Hurt>{}; Bool last_head_ = false; Bool puppets_ = false; Int budget_ = 60; Int serial_ = 1; Int spawns_allowed_ = 0; Int spawns_denied_ = 0; Float show_ = 170.0; Float hide_ = 190.0; };

  // ---- functions ----
  void People_set_budget(People& self, Int people_2 = 0) {
    self.budget_ = people_2;
  }

  void People_set_ranges(People& self, Float show = 0.0, Float hide = 0.0) {
    self.show_ = show;
    self.hide_ = hide;
  }

  void People_clear(People& self) {
    list_clear(self.people_);
  }

  struct People_spawns_allowedResult { Int Result = 0; };
  /// What the spawn gate (PeopleView::spawn_viewers) said in the last update: arrivals it allowed, arrivals it denied.
  KERN_PURE People_spawns_allowedResult People_spawns_allowed(const People& self) {
    return {self.spawns_allowed_};
  }

  struct People_spawns_deniedResult { Int Result = 0; };
  KERN_PURE People_spawns_deniedResult People_spawns_denied(const People& self) {
    return {self.spawns_denied_};
  }

  /// Crimes since the last call (to the wanted tracker).
  void People_take_reports(People& self, List<CrimeReport>& out) {
    Int place = 0;
    place = list_length<CrimeReport>(out);
    list_insert_list(out, place, self.reports_);
    list_clear(self.reports_);
  }

  void People_take_deaths(People& self, List<People_Death>& out) {
    Int place = 0;
    place = list_length<People_Death>(out);
    list_insert_list(out, place, self.deaths_);
    list_clear(self.deaths_);
  }

  void People_take_remote_hits(People& self, List<People_RemoteHit>& out) {
    Int place = 0;
    place = list_length<People_RemoteHit>(out);
    list_insert_list(out, place, self.remote_hits_);
    list_clear(self.remote_hits_);
  }

  void People_take_hurts(People& self, List<People_Hurt>& out) {
    Int place = 0;
    place = list_length<People_Hurt>(out);
    list_insert_list(out, place, self.hurts_);
    list_clear(self.hurts_);
  }

  void People_take_falls(People& self, List<People_Fall>& out) {
    Int place = 0;
    place = list_length<People_Fall>(out);
    list_insert_list(out, place, self.falls_);
    list_clear(self.falls_);
  }

  void People_stand_up(People& self, Int i = 0, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0) {
    self.people_[i].pos = feet;
    self.people_[i].yaw = yaw;
    self.people_[i].knocked_until = 0.0;
  }

  /// MP client: the citizens are the host's (puppets eased to his snapshots, no own spawning).
  void People_set_puppets(People& self, Bool on = false) {
    if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(on))), kern::to<Float>(value_int(kern::to<Int>(self.puppets_))))) {
      self.puppets_ = on;
      for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it10.next();) {
        if (enum_equal<People_Kind>(self.people_[n10.index].kind, People_Kind::citizen)) {
          self.people_[n10.index].state = People_State::gone;
        }
      }
    }
  }

  struct People_puppetsResult { Bool Result = false; };
  KERN_PURE People_puppetsResult People_puppets(const People& self) {
    return {self.puppets_};
  }

  struct People_last_headResult { Bool Result = false; };
  /// Whether the last hit of shot() was on the head (headshot_feedback.mjs).
  KERN_PURE People_last_headResult People_last_head(const People& self) {
    return {self.last_head_};
  }

  /// Down as a ragdoll that gets up again (world_procedural_population.mjs knockPerson).
  void People_knock(People& self, Int i = 0, Vector push = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    list_add(self.falls_, struct_make<People_Fall>(i, push, false, true));
    self.people_[i].knocked_until = kern::formula([](Float now) -> Float { return (now + 6.0); }, now);
    self.people_[i].v = 0.0;
  }

  /// VR grab: the person goes down as a ragdoll (gets up again later), pushed by `push`.
  void People_grab_knock(People& self, Int index = 0, Vector push = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    if (logic_and(math_compare<">=">(kern::to<Float>(index), 0.0), math_compare<"<">(kern::to<Float>(index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(self.people_))))))))) {
      People_knock(self, index, push, now);
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
