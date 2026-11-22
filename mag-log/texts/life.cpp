struct life : virtual kern::Library, virtual engine_physics_physics, virtual ragdoll, virtual shapes {
  // ---- types ----
  KERN_STRUCT(LifeOut_Burst) { Vector at = Vector{0.0, 0.0, 0.0}; Vector color = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(LifeOut_CowDeath) { Vector at = Vector{0.0, 0.0, 0.0}; Vector impulse = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(LifeOut_Impulse) { Int car = 0; Vector impulse = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(LifeOut_Say) { Text text = ""; Float pitch = 1.0; Float rate = 1.0; Float volume = 1.0; Float delay = 0.0; };
  KERN_ENUM(LifeOut_Sound, moo, moo_pain, coo, flap, squawk, thud, stomp, stomp_accent, jingle, horn, horn_bus, scream, scream_f, dog, cat, hiss, yowl, screech, kCount);
  KERN_STRUCT(Life_Bird) { Int seed = 0; Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float flap = 1.0; Float dead_until = -1.0; Float dying_until = -1.0; Float spin = 0.0; Vector fall_v = Vector{0.0, 0.0, 0.0}; Vector center = Vector{0.0, 0.0, 0.0}; Bool down = false; };
  KERN_STRUCT(Life_CatAttack) { Bool on = false; Int phase = 0; Vector p = Vector{0.0, 0.0, 0.0}; Vector from = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float start = 0.0; Float leap_at = 0.0; Float last_cry = -9.0; Float away_yaw = 0.0; Float leave_at = 0.0; Int cries = 0; };
  KERN_STRUCT(Life_LooseBird) { Int id = 0; Vector pos = Vector{0.0, 0.0, 0.0}; Vector vel = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float flap = 0.0; Float t = 0.0; Bool held = true; Int64 net = Int64{0x0}; };
  KERN_STRUCT(Life_NetFlock) { Int id = 0; Bool air = false; Vector at = Vector{0.0, 0.0, 0.0}; List<Vector> birds = List<Vector>{}; List<Float> yaws = List<Float>{}; List<Int> dead = List<Int>{}; };
  KERN_STRUCT(Life_Pigeon) { Vector o = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float walk_until = 0.0; Float peck = 0.0; Float flap = 0.0; Float lag = 0.0; Float dead_at = -1.0; Vector fall_v = Vector{0.0, 0.0, 0.0}; Vector net_to = Vector{0.0, 0.0, 0.0}; Vector net_v = Vector{0.0, 0.0, 0.0}; Float net_yaw = 0.0; Float net_at = 0.0; };
  KERN_STRUCT(Life_Runner) { Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float yaw_s = 0.0; Float lat = 0.0; Float phase = 0.0; Bool alive = true; Vector net_to = Vector{0.0, 0.0, 0.0}; Vector net_v = Vector{0.0, 0.0, 0.0}; Float net_at = 0.0; };
  KERN_ENUM(Life_Species, dog, cat, black_cat);
  KERN_STRUCT(Life_Tree) { Vector foot = Vector{0.0, 0.0, 0.0}; Float scale = 1.0; Int lane = 0; Float fell_at = -1.0; Vector fall_dir = Vector{1.0, 0.0, 0.0}; Int64 tile = Int64{0x0}; Int index = 0; };
  KERN_STRUCT(Life_Voice) { Vector at = Vector{0.0, 0.0, 0.0}; Text text = ""; Text mood = ""; Float until = 0.0; };
  KERN_STRUCT(LifeCar) { Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector half = Vector{0.0, 0.0, 0.0}; Float mass = 1400.0; Int car = -1; Bool player = false; Bool airborne = false; Bool stuck = false; Bool driver = true; };
  KERN_STRUCT(LifeOut_Play) { LifeOut_Sound sound = LifeOut_Sound::moo; Vector at = Vector{0.0, 0.0, 0.0}; Float volume = 0.0; Float pitch = 0.0; };
  KERN_STRUCT(Life_Chain) { Int id = 0; List<Vector> path = List<Vector>{}; List<Float> cum = List<Float>{}; Float len = 0.0; Float s = 0.0; Float born = 0.0; Float next_call = 0.0; Float next_beat = 0.0; Float off = 4.0; Int call = 0; Int beat = 0; Int side = 1; Fixed<Life_Runner, 10> runners = List<Life_Runner>{}; };
  KERN_STRUCT(Life_Cow) { Int64 key = Int64{0x0}; Int meadow = -1; Vector home = Vector{0.0, 0.0, 0.0}; Vector off = Vector{0.0, 0.0, 0.0}; Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float hp = 100.0; Float phase = 0.0; Float next_moo = 0.0; Float flee_until = 0.0; Float flinch_at = -9.0; Float last_hit = -9.0; Vector flee_v = Vector{0.0, 0.0, 0.0}; Vector color = Vector{0.0, 0.0, 0.0}; Quat rot = Quat{0.0, 0.0, 0.0, 1.0}; Bool dead = false; Bool walking = false; Bool carried = false; Float carried_still = 0.0; physics_Body body = physics_Body{Int64{0x0}}; Vector net_to = Vector{0.0, 0.0, 0.0}; Vector net_v = Vector{0.0, 0.0, 0.0}; Float net_at = 0.0; Quat net_rot = Quat{0.0, 0.0, 0.0, 1.0}; Bool net_dead = false; };
  KERN_STRUCT(Life_Flock) { Int id = 0; Bool air = false; Vector at = Vector{0.0, 0.0, 0.0}; Float next_flight = 0.0; Float t = 0.0; Float coo_at = 0.0; Vector c = Vector{0.0, 0.0, 0.0}; Vector from = Vector{0.0, 0.0, 0.0}; Vector dest = Vector{0.0, 0.0, 0.0}; Float r = 12.0; Float h = 12.0; Float dir = 1.0; Float a0 = 0.0; Float dur = 10.0; List<Life_Pigeon> birds = List<Life_Pigeon>{}; };
  KERN_STRUCT(Life_Meadow) { List<Vector_row> rings = List<Vector_row>{}; Vector lo = Vector{0.0, 0.0, 0.0}; Vector hi = Vector{0.0, 0.0, 0.0}; Vector centre = Vector{0.0, 0.0, 0.0}; Float area = 0.0; Int64 key = Int64{0x0}; Bool alive = true; };
  KERN_STRUCT(Life_NetCow) { Int64 key = Int64{0x0}; Vector pos = Vector{0.0, 0.0, 0.0}; Quat rot = Quat{0.0, 0.0, 0.0, 1.0}; Float yaw = 0.0; Bool dead = false; Bool walking = false; Bool carried = false; };
  KERN_STRUCT(Life_NetPet) { Int idx = 0; Vector pos = Vector{0.0, 0.0, 0.0}; Quat rot = Quat{0.0, 0.0, 0.0, 1.0}; Float yaw = 0.0; Float speed = 0.0; Vector off = Vector{0.0, 0.0, 0.0}; Bool shown = false; Bool dead = false; Bool carried = false; };
  KERN_STRUCT(Life_Pet) { Float radius = 20.0; Float omega = 0.05; Float phase = 0.0; Float wobble_omega = 0.2; Float wobble_r = 2.0; Float scale = 1.0; Vector pos = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float gait = 0.0; Float speed = 0.0; Float dead_at = -1.0; Bool shown = false; Bool has_pose = false; Bool carried = false; Float carried_still = 0.0; Vector off = Vector{0.0, 0.0, 0.0}; Int punches = 0; Vector color = Vector{0.0, 0.0, 0.0}; physics_Body body = physics_Body{Int64{0x0}}; Quat rot = Quat{0.0, 0.0, 0.0, 1.0}; Quat net_rot = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_STRUCT(LifeOut) { List<LifeOut_Play> sounds = List<LifeOut_Play>{}; List<LifeOut_Say> speech = List<LifeOut_Say>{}; List<LifeOut_Impulse> impulses = List<LifeOut_Impulse>{}; List<LifeOut_Burst> bursts = List<LifeOut_Burst>{}; Text banner = ""; Float banner_for = 0.0; Bool kill_player = false; List<LifeOut_CowDeath> cow_deaths = List<LifeOut_CowDeath>{}; };
  KERN_STRUCT(LifeView) { Int physics = 0; Vector focus = Vector{0.0, 0.0, 0.0}; Vector player = Vector{0.0, 0.0, 0.0}; Vector eye = Vector{0.0, 0.0, 0.0}; Vector head = Vector{0.0, 0.0, 0.0}; Float epoch = 0.0; Bool dogs_on = true; Bool cats_on = true; Float player_speed = 0.0; Bool on_foot = true; Bool people_on = true; Bool birds_on = true; List<LifeCar> cars = List<LifeCar>{}; List<Vector> mates = List<Vector>{}; };
  KERN_STRUCT(Life) { List<Life_LooseBird> loose_ = List<Life_LooseBird>{}; Int loose_serial_ = 0; Bool remote_animals_ = false; List<Vector> pet_anchors_ = List<Vector>{}; Int pet_base_ = 20; Float remote_animals_at_ = -9.0; List<Vector> pet_net_to_ = List<Vector>{}; List<Float> pet_net_yaw_ = List<Float>{}; List<Int64> loose_keys_ = List<Int64>{}; Bool remote_joggers_ = false; Float remote_joggers_at_ = -9.0; Int world_ = 0; Bool mobile_ = false; Ragdolls ragdolls_ = Ragdolls{0, List<Ragdolls_Doll>{}, List<Ragdolls_Up>{}, 10, 0, 0}; List<Life_Meadow> meadows_ = List<Life_Meadow>{}; List<Life_Cow> cows_ = List<Life_Cow>{}; List<Int64> dead_cows_ = List<Int64>{}; Vector herds_for_ = Vector{1000000000.0, 1000000000.0, 0.0}; Float last_herd_check_ = -99.0; Float last_moo_ = -99.0; Float herds_at_ = -99.0; List<Life_Flock> flocks_ = List<Life_Flock>{}; Int flock_serial_ = 0; Int trees_down_ = 0; Int tree_ground_cursor_ = 0; List<Life_Bird> birds_ = List<Life_Bird>{}; List<Life_Tree> trees_ = List<Life_Tree>{}; List<Life_Pet> pets_ = List<Life_Pet>{}; Life_CatAttack cat_ = Life_CatAttack{false, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -9.0, 0.0, 0.0, 0}; Vector pet_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0}; Float epoch_ = 0.0; Int tree_serial_ = 4294967295; Set<Int64> trees_gone_ = Set<Int64>{}; Life_Chain chain_ = Life_Chain{0, List<Vector>{}, List<Float>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0, 0, 1, List<Life_Runner>{}}; Bool chain_alive_ = false; Float next_chain_ = -1.0; Float streak_at_ = -99.0; Int chain_serial_ = 0; Int streak_n_ = 0; Int streak_chain_ = 0; List<Life_Voice> voices_ = List<Life_Voice>{}; Float last_shout_ = -99.0; Float last_honk_ = -99.0; Float last_scream_ = -99.0; Float last_coo_ = -99.0; Float ambient_chirp_ = 0.0; List<Int_Float_pair> source_gap_ = List<Int_Float_pair>{}; Int cow_hits_ = 0; Int bird_hits_ = 0; Int shouts_ = 0; Int honks_ = 0; };

  // ---- functions ----
  void LifeOut_clear(LifeOut& self) {
    list_clear(self.sounds);
    list_clear(self.speech);
    list_clear(self.impulses);
    list_clear(self.bursts);
    self.banner = "";
    self.banner_for = 0.0;
    self.kill_player = false;
    list_clear(self.cow_deaths);
  }

  struct Life_trees_downResult { Int Result = 0; };
  KERN_PURE Life_trees_downResult Life_trees_down(const Life& self) {
    return {self.trees_down_};
  }

  struct Life_epochResult { Float Result = 0.0; };
  KERN_PURE Life_epochResult Life_epoch(const Life& self) {
    return {self.epoch_};
  }

  struct Life_has_chainResult { Bool Result = false; };
  KERN_PURE Life_has_chainResult Life_has_chain(const Life& self) {
    return {self.chain_alive_};
  }

  /// MP client: the running club is the host's (game_worldsync.cpp) - no own chain, his runners eased to where his snapshots have them; none heard for 1.5 s: gone.
  void Life_set_remote_joggers(Life& self, Bool on = false) {
    if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(on))), kern::to<Float>(value_int(kern::to<Int>(self.remote_joggers_))))) {
      self.remote_joggers_ = on;
      self.chain_alive_ = false;
    }
  }

  struct Life_animal_ofResult { Int Result = 0; };
  /// Which animal a body is (a carried / thrown one): 1 a cow (id: its key), 2 a dog or cat (id: its index); 0 none.
  Life_animal_ofResult Life_animal_of(const Life& self, Int64& id, physics_Body b = physics_Body{Int64{0x0}}) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it10.next();) {
      if (logic_and(engine_physics_physics::Body_equal(self.cows_[n10.index].body, b).Result, engine_physics_physics::Body_valid(b).Result)) {
        id = self.cows_[n10.index].key;
        return {1};
      }
    }
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it11.next();) {
      if (logic_and(engine_physics_physics::Body_equal(self.pets_[n11.index].body, b).Result, engine_physics_physics::Body_valid(b).Result)) {
        id = int64_of_int(n11.index);
        return {2};
      }
    }
    return {0};
  }

  struct Life_cow_bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  KERN_PURE Life_cow_bodyResult Life_cow_body(const Life& self, Int64 key = Int64{0x0}) {
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it12.next();) {
      if (int64_equal(self.cows_[n12.index].key, key)) {
        return {self.cows_[n12.index].body};
      }
    }
    return {struct_make<physics_Body>(Int64{0x0})};
  }

  struct Life_pet_bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  KERN_PURE Life_pet_bodyResult Life_pet_body(const Life& self, Int i = 0) {
    return {logic_select<physics_Body>(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Life_Pet>(self.pets_))), 0.5), self.pets_[i].body, struct_make<physics_Body>(Int64{0x0}))};
  }

  struct Life_remote_animalsResult { Bool Result = false; };
  KERN_PURE Life_remote_animalsResult Life_remote_animals(const Life& self) {
    return {self.remote_animals_};
  }

  struct Life_anchor_ofResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Life_anchor_ofResult Life_anchor_of(const Life& self, Int i = 0) {
    Int k = 0;
    k = kern::to<Int>(kern::formula([](Float pet_base_, Float i) -> Float { return ((pet_base_ != 0.0) ? kern::math::trunc(kern::math::div(i, pet_base_)) : 0.0); }, kern::to<Float>(self.pet_base_), kern::to<Float>(i)));
    return {logic_select<Vector>(logic_and(math_compare<">">(kern::to<Float>(k), 0.0), math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(self.pet_anchors_))), 0.5)), self.pet_anchors_[k], self.pet_anchor_)};
  }

  struct Life_cow_hitsResult { Int Result = 0; };
  KERN_PURE Life_cow_hitsResult Life_cow_hits(const Life& self) {
    return {self.cow_hits_};
  }

  struct Life_bird_hitsResult { Int Result = 0; };
  KERN_PURE Life_bird_hitsResult Life_bird_hits(const Life& self) {
    return {self.bird_hits_};
  }

  struct Life_jogger_streakResult { Int Result = 0; };
  KERN_PURE Life_jogger_streakResult Life_jogger_streak(const Life& self) {
    return {self.streak_n_};
  }

  struct Life_shoutsResult { Int Result = 0; };
  KERN_PURE Life_shoutsResult Life_shouts(const Life& self) {
    return {self.shouts_};
  }

  struct Life_honksResult { Int Result = 0; };
  KERN_PURE Life_honksResult Life_honks(const Life& self) {
    return {self.honks_};
  }

  /// The animal with this body is in a hand this frame: it does not stand up there (held still).
  void Life_hold_animal(Life& self, physics_Body b = physics_Body{Int64{0x0}}) {
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it13.next();) {
      if (logic_and(self.cows_[n13.index].carried, engine_physics_physics::Body_equal(self.cows_[n13.index].body, b).Result)) {
        self.cows_[n13.index].carried_still = 0.0;
      }
    }
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it14.next();) {
      if (logic_and(self.pets_[n14.index].carried, engine_physics_physics::Body_equal(self.pets_[n14.index].body, b).Result)) {
        self.pets_[n14.index].carried_still = 0.0;
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
};
