struct life_cpp : virtual kern::Library, virtual city_stream, virtual city_stream_cpp, virtual common, virtual engine_math_math, virtual engine_physics_physics, virtual life, virtual ragdoll, virtual ragdoll_cpp, virtual terrain_cpp {
  // ---- types ----
  KERN_STRUCT(Lines) { Text mood = ""; Fixed<Text, 6> lines = List<Text>{}; U32 n = 0; };
  KERN_STRUCT(Life_rebuild_herds_Cand) { Life_Meadow m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true}; Float d = 0.0; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kHerdRadius = 420.0;
  KERN_CONST Float kMinArea = 1800.0;
  /// cows.mjs
  KERN_CONST U32 kMaxCows = 40;
  KERN_CONST Float kSpreadR = 14.0;
  KERN_CONST Fixed<U32, 5> kCoats = List<U32>{15855592, 15855592, 9067068, 3812388, 13609335};
  KERN_CONST Float kGrazeR = 6.0;
  KERN_CONST Float kPeriod = 24.0;
  KERN_CONST Float kWalk = 0.55;
  KERN_CONST Float kCowMass = 600.0;
  KERN_CONST Float kDespawn = 150.0;
  KERN_CONST Float kNear = 22.0;
  KERN_CONST Float kFar = 95.0;
  /// pigeons.mjs
  KERN_CONST U32 kPerFlockMin = 6;
  /// pigeons.mjs
  KERN_CONST U32 kPerFlockMax = 12;
  KERN_CONST Float kSpookPlayer = 4.2;
  KERN_CONST Float kSpookCar = 8.0;
  KERN_CONST Fixed<U32, 4> kCatColors = List<U32>{9276813, 14191162, 5984328, 15327958};
  KERN_CONST Fixed<U32, 5> kDogColors = List<U32>{9071178, 12884570, 3877922, 15260872, 7232070};
  KERN_CONST Float kSpeed = 2.9;
  KERN_CONST Float kFirst = 60.0;
  KERN_CONST Float kEveryMin = 170.0;
  KERN_CONST Float kEveryMax = 280.0;
  /// jogger_chain.mjs
  KERN_CONST U32 kRunners = 10;
  KERN_CONST Float kGap = 1.35;
  KERN_CONST Float kChainLife = 95.0;
  KERN_CONST Float kStreakWindow = 12.0;
  KERN_CONST Fixed<Text_6_row, 6> kCalls = List<Text_6_row>{Text_6_row{List<Text>{"Wer l\303\244uft mit?", "Wir laufen mit!"}}, Text_6_row{List<Text>{"Links, rechts, links!", "Wir sind nicht zu bremsen!"}}, Text_6_row{List<Text>{"Eins, zwei, drei, vier!", "Die Stra\303\237e geh\303\266rt uns hier!"}}, Text_6_row{List<Text>{"Wie weit noch?", "Bis zum See und weiter!"}}, Text_6_row{List<Text>{"Was wollen wir?", "Kilometer!"}}, Text_6_row{List<Text>{"Schneller?", "Immer schneller!"}}};
  /// street_voices.mjs
  KERN_CONST Float kShoutGap = 1.4;
  /// street_voices.mjs
  KERN_CONST Float kMaxHear = 38.0;
  /// street_voices.mjs
  KERN_CONST Float kSourceGap = 6.0;
  KERN_CONST Fixed<Lines, 5> kLines = List<Lines>{Lines{"bump", List<Text>{"Hey, pass doch auf!", "Geht's noch?", "Augen auf, Mann!", "Was soll das denn?", "Mann, ey!", "Hast du keine Augen im Kopf?"}, 6}, Lines{"angry", List<Text>{"Na warte!", "Jetzt reicht's mir!", "Willst du \303\204rger?", "Komm her, du Idiot!", "Dir zeig ich's!", "Du hast sie doch nicht alle!"}, 6}, Lines{"scared", List<Text>{"Hilfe!", "Weg hier!", "Der ist verr\303\274ckt!", "Lauf!", "Polizei! Hilfe!"}, 5}, Lines{"nearmiss", List<Text>{"Bist du blind?!", "Langsam, du Irrer!", "Spinnst du?!", "Pass doch auf, du Vollidiot!", "Fahr doch nicht wie ein Henker!"}, 5}, Lines{"honked", List<Text>{"Ja ja, ist ja gut!", "Reg dich ab!", "Hup nicht so!", "Selber!"}, 4}};
  /// street_voices.mjs
  KERN_CONST Float kBubble = 2.1;
  /// street_voices.mjs
  KERN_CONST Float kHonkGap = 1.2;

  // ---- functions ----
  struct Life_speciesResult { Life_Species Result = Life_Species::dog; };
  KERN_PURE Life_speciesResult Life_species(const Life& self, U32 i = 0, Float epoch = 0.0) {
    if (logic_and(math_compare<"==">(kern::to<Float>(i), 1.0), logic_or(int64_equal(int64_rem(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float epoch) -> Float { return kern::math::floor(kern::math::div(epoch, 120.0)); }, epoch))), Int64{0x2}), Int64{0x0}), self.cat_.on))) {
      return {Life_Species::black_cat};
    }
    return {logic_select<Life_Species>(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pet_base_, Float i) -> Float { return kern::math::mod(((pet_base_ != 0.0) ? kern::math::mod(i, pet_base_) : i), 3.0); }, kern::to<Float>(self.pet_base_), kern::to<Float>(i))))), 2.0), Life_Species::cat, Life_Species::dog)};
  }

  void Life_kill_pet(Life& self, LifeOut& out, U32 i = 0, Vector dir = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Life_Species sp = Life_Species::dog;
    Float sc = 0.0;
    Vector half = Vector{0.0, 0.0, 0.0};
    Float mass = 0.0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Float side = 0.0;
    Float w = 0.0;
    Float h = 0.0;
    Float I = 0.0;
    if (math_compare<">=">(self.pets_[i].dead_at, 0.0)) {
      return;
    }
    sp = Life_species(self, i, self.epoch_).Result;
    self.pets_[i].dead_at = now;
    list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(enum_equal<Life_Species>(sp, Life_Species::dog), LifeOut_Sound::dog, LifeOut_Sound::cat), self.pets_[i].pos, 0.8, 1.0));
    sc = kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : (a_2 * 0.62)); }, kern::to<Float>(enum_equal<Life_Species>(sp, Life_Species::dog)), self.pets_[i].scale);
    half = vector_scale(Vector{0.33, 0.13, 0.27}, sc);
    mass = kern::formula([](Float a, Float sc) -> Float { return (((((a != 0.0) ? 22.0 : 4.0) * sc) * sc) * sc); }, kern::to<Float>(enum_equal<Life_Species>(sp, Life_Species::dog)), sc);
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    bs.type = physics_BodyType::dynamic;
    bs.position = vector_make(vector_break(self.pets_[i].pos).x, vector_break(self.pets_[i].pos).y, kern::formula([](Float z, Float z_2) -> Float { return (z + z_2); }, vector_break(self.pets_[i].pos).z, vector_break(half).z));
    bs.rotation = common::yaw_quat(self.pets_[i].yaw).Result;
    bs.linear_damping = 0.9;
    bs.angular_damping = 2.5;
    engine_world_create_body(n10, self.world_, bs);
    create_body = n10.result;
    self.pets_[i].body = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    ss.kind = physics_ShapeKind::box;
    ss.size = vector_scale(half, 2.0);
    ss.material.density = kern::formula([](Float mass, Float x, Float y, Float z) -> Float { return kern::math::div(mass, (((8.0 * x) * y) * z)); }, mass, vector_break(half).x, vector_break(half).y, vector_break(half).z);
    ss.material.friction = 0.7;
    ss.material.category_bits = Int64{0x1};
    engine_world_create_shape(n11, self.world_, self.pets_[i].body, ss);
    create_shape = n11.result;
    f = vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.pets_[i].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.pets_[i].yaw), 0.0);
    side = kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((((x * (-y)) + (y_2 * x_2)) >= 0.0) ? 1.0 : (-1.0)); }, vector_break(dir).x, vector_break(f).y, vector_break(dir).y, vector_break(f).x);
    w = kern::formula([](Float y) -> Float { return (2.0 * y); }, vector_break(half).y);
    h = kern::formula([](Float z) -> Float { return (2.0 * z); }, vector_break(half).z);
    I = kern::formula([](Float mass, Float w, Float h) -> Float { return kern::math::div((mass * ((w * w) + (h * h))), 3.0); }, mass, w, h);
    engine_world_apply_angular_impulse(self.world_, self.pets_[i].body, vector_scale(f, kern::formula([](Float i, Float side) -> Float { return ((i * 5.0) * side); }, I, side)));
    engine_world_apply_linear_impulse_to_center(self.world_, self.pets_[i].body, vector_scale(vector_make(vector_break(dir).x, vector_break(dir).y, kern::formula([](Float z) -> Float { return kern::math::max(0.0, z); }, vector_break(dir).z)), mass));
  }

  /// A dog or cat killed on a client: the host's dies too (and every snapshot says so).
  void Life_net_kill_pet(Life& self, LifeOut& out, U32 index = 0, Vector dir = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(index), kern::to<Float>(list_length<Life_Pet>(self.pets_))), 0.5), math_compare<"<">(self.pets_[index].dead_at, 0.0))) {
      Life_kill_pet(self, out, index, dir, now);
    }
  }

  struct hash01Result { Float Result = 0.0; };
  KERN_PURE hash01Result hash01(Int64 v = Int64{0x0}) {
    Int64 v_ = Int64{0x0};
    v_ = v;
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    v_ = int64_mul(v_, Int64{0xff51afd7ed558ccd});
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    v_ = int64_mul(v_, Int64{0xc4ceb9fe1a85ec53});
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    return {kern::formula([](Float a) -> Float { return kern::math::div(a, 16777216.0); }, int64_to_float_unsigned(int64_shift_right_unsigned(v_, 40)))};
  }

  struct mixResult { Int64 Result = Int64{0x0}; };
  KERN_PURE mixResult mix(Int64 a = Int64{0x0}, Int64 b = Int64{0x0}) {
    return {int64_add(int64_add(int64_mul(a, Int64{0x9e3779b97f4a7c15}), b), Int64{0x632be59bd9b4e019})};
  }

  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct dist2d_2Result { Float Result = 0.0; };
  KERN_PURE dist2d_2Result dist2d_2(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(U32 h = 0) {
    return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(h)))};
  }

  struct in_ringResult { Bool Result = false; };
  KERN_PURE in_ringResult in_ring(const List<Vector>& r, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool c = false;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    c = false;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      a = r[i];
      b = r[j];
      if (logic_and(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(a).y, vector_break(p).y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(b).y, vector_break(p).y))))), math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float x_2, Float y, Float y_2, Float y_3) -> Float { return (kern::math::div(((x - x_2) * (y - y_2)), ((y_3 - y_2) + 1e-12)) + x_2); }, vector_break(b).x, vector_break(a).x, vector_break(p).y, vector_break(a).y, vector_break(b).y)))) {
        c = logic_not(c);
      }
      was = i;
      i += 1;
      j = was;
    }
    return {c};
  }

  struct seg_sphereResult { Float Result = 0.0; };
  /// Segment from `a` to `b` against a sphere: the entry fraction, or < 0.
  KERN_PURE seg_sphereResult seg_sphere(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Vector m = Vector{0.0, 0.0, 0.0};
    Float A = 0.0;
    Float B = 0.0;
    Float C = 0.0;
    Float disc = 0.0;
    Float t = 0.0;
    d = vector_sub(b, a);
    m = vector_sub(a, c);
    A = vector_dot(d, d);
    B = vector_dot(m, d);
    C = kern::formula([](Float a, Float r) -> Float { return (a - (r * r)); }, vector_dot(m, m), r);
    if (math_compare<"<">(A, 1e-09)) {
      return {-1.0};
    }
    disc = kern::formula([](Float b, Float a, Float c) -> Float { return ((b * b) - (a * c)); }, B, A, C);
    if (math_compare<"<">(disc, 0.0)) {
      return {-1.0};
    }
    t = kern::formula([](Float b, Float disc, Float a) -> Float { return kern::math::div(((-b) - kern::math::sqrt(disc)), a); }, B, disc, A);
    return {kern::formula([](Float t, Float c) -> Float { return (((t >= 0.0) && (t <= 1.0)) ? t : ((c < 0.0) ? 0.0 : (-1.0))); }, t, C)};
  }

  struct Life_in_ringsResult { Bool Result = false; };
  KERN_PURE Life_in_ringsResult Life_in_rings(const List<Vector_row>& rings, Vector p = Vector{0.0, 0.0, 0.0}) {
    if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(rings)), 0.0), logic_not(in_ring(rings[0].items, p).Result))) {
      return {false};
    }
    for (auto it12 = flow_for(n12, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(rings))))); it12.next();) {
      if (logic_and(math_compare<">">(kern::to<Float>(list_length<Vector>(rings[n12.index].items)), 2.0), in_ring(rings[n12.index].items, p).Result)) {
        return {false};
      }
    }
    return {true};
  }

  struct Life_ring_areaResult { Float Result = 0.0; };
  KERN_PURE Life_ring_areaResult Life_ring_area(const List<Vector>& r) {
    Float a = 0.0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    a = 0.0;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      a += kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x + x_2) * (y - y_2)); }, vector_break(r[j]).x, vector_break(r[i]).x, vector_break(r[j]).y, vector_break(r[i]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula([](Float a) -> Float { return (kern::math::abs(a) * 0.5); }, a)};
  }

  /// a dog's / cat's round by its index (the same on every machine)
  void Life_pet_params(Life_Pet& p, U32 i = 0) {
    Int64 h = Int64{0x0};
    h = mix(Int64{0xa11a}, int64_of_int(i)).Result;
    p.radius = kern::formula([](Float the_result) -> Float { return (13.0 + (45.0 * the_result)); }, hash01(int64_add(h, Int64{0x1})).Result);
    p.omega = kern::formula([](Float the_result, Float radius) -> Float { return kern::math::div((0.55 + (0.9 * the_result)), radius); }, hash01(int64_add(h, Int64{0x2})).Result, p.radius);
    p.phase = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, hash01(int64_add(h, Int64{0x3})).Result, kPi);
    p.wobble_omega = kern::formula([](Float the_result) -> Float { return (0.12 + (0.18 * the_result)); }, hash01(int64_add(h, Int64{0x4})).Result);
    p.wobble_r = kern::formula([](Float the_result) -> Float { return (1.2 + (2.3 * the_result)); }, hash01(int64_add(h, Int64{0x5})).Result);
    p.scale = kern::formula([](Float the_result) -> Float { return (0.72 + (0.46 * the_result)); }, hash01(int64_add(h, Int64{0x6})).Result);
  }

  void Life_init(Life& self, Object<kern::physics::World> w = Object<kern::physics::World>{}, Bool mobile = false) {
    Object<kern::physics::World> w_ = Object<kern::physics::World>{};
    Int count = 0;
    Life_Pet fill = Life_Pet{20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, physics_Body{Int64{0x0}}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    U32 n = 0;
    Int count_2 = 0;
    Life_Bird fill_2 = Life_Bird{0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, -1.0, -1.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false};
    w_ = w;
    self.world_ = w_;
    self.mobile_ = mobile;
    self.pet_base_ = kern::to<Int>(kern::formula([](Float mobile) -> Float { return ((mobile != 0.0) ? 12.0 : 20.0); }, kern::to<Float>(mobile)));
    count = self.pet_base_;
    fill = struct_make<Life_Pet>(20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, struct_make<physics_Body>(Int64{0x0}), struct_make<Quat>(0.0, 0.0, 0.0, 1.0), struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
    list_clear(self.pets_);
    list_resize(self.pets_, count, fill);
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it13.next();) {
      Life_pet_params(self.pets_[n13.index], n13.index);
    }
    ragdoll::Ragdolls_init(self.ragdolls_, w_, mobile);
    n = kern::to<Int>(kern::formula([](Float mobile) -> Float { return ((mobile != 0.0) ? 10.0 : 16.0); }, kern::to<Float>(mobile)));
    count_2 = n;
    fill_2 = struct_make<Life_Bird>(0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, -1.0, -1.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false);
    list_clear(self.birds_);
    list_resize(self.birds_, count_2, fill_2);
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it14.next();) {
      self.birds_[n14.index].seed = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc((the_result * 4294967295.0)); }, hash01(mix(Int64{0xb17d}, int64_of_int(n14.index)).Result).Result));
    }
  }

  void Life_clear(Life& self) {
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it15.next();) {
      both = logic_and(engine_physics_physics::Body_valid(self.pets_[n15.index].body).Result, object_valid<kern::physics::World>(self.world_));
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, self.pets_[n15.index].body);
        both = body_valid_2;
      }
      if (both) {
        engine_world_destroy_body(self.world_, self.pets_[n15.index].body);
      }
      self.pets_[n15.index].body = struct_make<physics_Body>(Int64{0x0});
      self.pets_[n15.index].dead_at = -1.0;
      self.pets_[n15.index].shown = false;
    }
    self.cat_ = struct_make<Life_CatAttack>(false, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -9.0, 0.0, 0.0, 0);
    self.pet_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0};
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it16.next();) {
      both_2 = logic_and(engine_physics_physics::Body_valid(self.cows_[n16.index].body).Result, object_valid<kern::physics::World>(self.world_));
      if (both_2) {
        body_valid_4 = engine_world_body_valid(self.world_, self.cows_[n16.index].body);
        both_2 = body_valid_4;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.cows_[n16.index].body);
      }
    }
    list_clear(self.cows_);
    list_clear(self.dead_cows_);
    list_clear(self.meadows_);
    self.herds_for_ = Vector{1000000000.0, 1000000000.0, 0.0};
    list_clear(self.flocks_);
    list_clear(self.trees_);
    set_clear(self.trees_gone_);
    self.tree_serial_ = 4294967295;
    self.chain_alive_ = false;
    self.next_chain_ = -1.0;
    self.streak_n_ = 0;
    list_clear(self.voices_);
    ragdoll_cpp::Ragdolls_clear(self.ragdolls_);
  }

  struct rebuild_herds_matchResult { Bool Result = false; };
  KERN_PURE rebuild_herds_matchResult rebuild_herds_match(const CityStream_AreaRef& more_n19_index, const CityStream_AreaRef& q) {
    return {int64_equal(q.key, more_n19_index.key)};
  }

  struct rebuild_herds_match_2Result { Bool Result = false; };
  KERN_PURE rebuild_herds_match_2Result rebuild_herds_match_2(const Life_rebuild_herds_Cand& a, const Life_rebuild_herds_Cand& b) {
    return {math_compare<"<">(a.d, b.d)};
  }

  struct rebuild_herds_match_3Result { Bool Result = false; };
  KERN_PURE rebuild_herds_match_3Result rebuild_herds_match_3(const Life_Cow& x, Int64 key = Int64{0x0}) {
    return {int64_equal(x.key, key)};
  }

  struct Life_graze_spotResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Life_graze_spotResult Life_graze_spot(const Life& self, const Life_Cow& c, Int64 k = Int64{0x0}) {
    Bool fenced = false;
    Int64 h = Int64{0x0};
    Float a = 0.0;
    Float r = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    fenced = logic_and(math_compare<">=">(kern::to<Float>(c.meadow), 0.0), math_compare<"<">(kern::to<Float>(c.meadow), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Life_Meadow>(self.meadows_))))))));
    for (auto it23 = flow_for(n23, 0, 5); it23.next();) {
      h = mix(c.key, int64_add(int64_mul(k, Int64{0x10}), int64_of_int(n23.index))).Result;
      a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, hash01(h).Result, kPi);
      r = kern::formula([](Float the_result, Float kGrazeR) -> Float { return (kern::math::sqrt(the_result) * kGrazeR); }, hash01(int64_add(h, Int64{0x1})).Result, kGrazeR);
      p = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(c.home).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin(a) * r)); }, vector_break(c.home).y, a, r), 0.0);
      if (logic_or(logic_not(fenced), Life_in_rings(self.meadows_[c.meadow].rings, p).Result)) {
        return {p};
      }
    }
    return {c.home};
  }

  void Life_rebuild_herds(Life& self, const LifeView& view, const CityStream& city, Float now = 0.0) {
    List<CityStream_AreaRef> areas = List<CityStream_AreaRef>{};
    List<Vector> foci = List<Vector>{};
    Int place = 0;
    List<CityStream_AreaRef> more = List<CityStream_AreaRef>{};
    Int at = 0;
    Int end = 0;
    Bool go = false;
    List<Life_rebuild_herds_Cand> cand = List<Life_rebuild_herds_Cand>{};
    Bool skip = false;
    MapFeatures_Area meadow = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    Float area = 0.0;
    Life_Meadow m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    List<Life_Cow> next = List<Life_Cow>{};
    List<Life_Meadow> meadows = List<Life_Meadow>{};
    Int at_2 = 0;
    Int count = 0;
    Bool stop = false;
    Life_Meadow m_2 = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    U32 n = 0;
    I32 mi = 0;
    Vector herd = Vector{0.0, 0.0, 0.0};
    I32 k = 0;
    Bool stop_2 = false;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    U32 i = 0;
    Bool skip_2 = false;
    Int64 key = Int64{0x0};
    Int old = 0;
    Int at_3 = 0;
    Int end_2 = 0;
    Bool go_2 = false;
    Life_Cow c_2 = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
    I32 k_2 = 0;
    Bool stop_3 = false;
    Float a = 0.0;
    Float r = 0.0;
    Vector p_3 = Vector{0.0, 0.0, 0.0};
    Int at_4 = 0;
    Int end_3 = 0;
    Bool go_3 = false;
    Vector g = Vector{0.0, 0.0, 0.0};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    areas = List<CityStream_AreaRef>{};
    foci = list_make<Vector, "1">(view.focus);
    place = list_length<Vector>(foci);
    list_insert_list(foci, place, view.mates);
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(foci))))); it18.next();) {
      more = List<CityStream_AreaRef>{};
      city_stream_cpp::CityStream_areas_near(city, more, vector_make(vector_break(foci[n18.index]).x, vector_break(foci[n18.index]).y, 0.0), kHerdRadius, AreaKind::grass);
      for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_AreaRef>(more))))); it19.next();) {
        at = 0;
        end = list_length<CityStream_AreaRef>(areas);
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
        while (loop(go)) {
          if (rebuild_herds_match(more[n19.index], areas[at]).Result) {
            go = false;
          } else {
            at += 1;
            go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
          }
        }
        if (math_compare<"==">(kern::to<Float>(at), kern::to<Float>(end))) {
          list_add(areas, more[n19.index]);
        }
      }
    }
    cand = List<Life_rebuild_herds_Cand>{};
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_AreaRef>(areas))))); it20.next();) {
      skip = false;
      meadow = list_get<MapFeatures_Area>(struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city, areas[n20.index].tile).Result).areas, areas[n20.index].index);
      area = Life_ring_area(meadow.rings[0].items).Result;
      if (math_compare<"<">(area, kMinArea)) {
        skip = true;
      }
      if (logic_not(skip)) {
        m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
        m.rings = meadow.rings;
        m.key = areas[n20.index].key;
        m.area = area;
        m.lo = Vector{1000000000.0, 1000000000.0, 0.0};
        m.hi = Vector{-1000000000.0, -1000000000.0, 0.0};
        c = Vector{0.0, 0.0, 0.0};
        for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(m.rings[0].items))))); it21.next();) {
          p = m.rings[0].items[n21.index];
          m.lo = vector_make(kern::formula([](Float x, Float x_2) -> Float { return kern::math::min(x, x_2); }, vector_break(m.lo).x, vector_break(p).x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::min(y, y_2); }, vector_break(m.lo).y, vector_break(p).y), 0.0);
          m.hi = vector_make(kern::formula([](Float x, Float x_2) -> Float { return kern::math::max(x, x_2); }, vector_break(m.hi).x, vector_break(p).x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::max(y, y_2); }, vector_break(m.hi).y, vector_break(p).y), 0.0);
          c = vector_add(c, p);
        }
        m.centre = vector_scale(c, kern::formula([](Float a) -> Float { return kern::math::div(1.0, (a < 0.0 ? a + 18446744073709551616.0 : a)); }, kern::to<Float>(list_length<Vector>(m.rings[0].items))));
        d = 1000000000.0;
        for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(foci))))); it22.next();) {
          d = kern::formula([](Float d, Float the_result) -> Float { return kern::math::min(d, the_result); }, d, dist2d_2(m.centre, vector_make(vector_break(foci[n22.index]).x, vector_break(foci[n22.index]).y, 0.0)).Result);
        }
        if (math_compare<">">(d, kern::formula([](Float kHerdRadius, Float area) -> Float { return (kHerdRadius + kern::math::sqrt(area)); }, kHerdRadius, area))) {
          skip = true;
        }
        if (logic_not(skip)) {
          list_add(cand, struct_make<Life_rebuild_herds_Cand>(m, d));
        }
      }
    }
    list_sort<&life_cpp::rebuild_herds_match_2>(*this, cand);
    next = List<Life_Cow>{};
    meadows = List<Life_Meadow>{};
    count = list_length<Life_rebuild_herds_Cand>(cand);
    at_2 = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(count))))) {
      m_2 = cand[at_2].m;
      n = kern::to<Int>(kern::formula([](Float area, Float the_result) -> Float { return kern::math::wrap32(kern::math::max(2.0, kern::math::min(7.0, (kern::math::wrap32s(kern::math::round(kern::math::div(area, 4000.0))) + kern::math::trunc((the_result * 2.0)))))); }, m_2.area, hash01(m_2.key).Result));
      mi = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Life_Meadow>(meadows))));
      list_add(meadows, m_2);
      herd = m_2.centre;
      k = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(k), 14.0)))) {
        p_2 = vector_make(kern::formula([](Float x, Float x_2, Float the_result) -> Float { return (x + ((x_2 - x) * the_result)); }, vector_break(m_2.lo).x, vector_break(m_2.hi).x, hash01(mix(m_2.key, int64_of_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (100.0 + k); }, kern::to<Float>(k))))).Result).Result), kern::formula([](Float y, Float y_2, Float the_result) -> Float { return (y + ((y_2 - y) * the_result)); }, vector_break(m_2.lo).y, vector_break(m_2.hi).y, hash01(mix(m_2.key, int64_of_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (200.0 + k); }, kern::to<Float>(k))))).Result).Result), 0.0);
        if (Life_in_rings(m_2.rings, p_2).Result) {
          herd = p_2;
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          k += 1;
        }
      }
      i = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), math_compare<">">(kern::formula([](Float a, Float kMaxCows) -> Float { return ((a < 0.0) != (kMaxCows < 0.0) ? (kMaxCows < 0.0) : (a < kMaxCows)); }, kern::to<Float>(list_length<Life_Cow>(next)), kern::to<Float>(kMaxCows)), 0.5)))) {
        skip_2 = false;
        key = mix(m_2.key, int64_of_int(i)).Result;
        at_3 = 0;
        end_2 = list_length<Life_Cow>(self.cows_);
        go_2 = math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_2));
        while (loop(go_2)) {
          if (rebuild_herds_match_3(self.cows_[at_3], key).Result) {
            go_2 = false;
          } else {
            at_3 += 1;
            go_2 = math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_2));
          }
        }
        old = at_3;
        if (logic_not(math_compare<"==">(kern::to<Float>(old), kern::to<Float>(list_length<Life_Cow>(self.cows_))))) {
          self.cows_[old].meadow = mi;
          list_add(next, self.cows_[old]);
          self.cows_[old].body = struct_make<physics_Body>(Int64{0x0});
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          c_2 = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
          c_2.key = key;
          c_2.meadow = mi;
          c_2.home = herd;
          k_2 = 0;
          stop_3 = false;
          while (loop(logic_and(logic_not(stop_3), math_compare<"<">(kern::to<Float>(k_2), 8.0)))) {
            a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, hash01(mix(key, int64_of_int(kern::to<Int>(kern::formula([](Float k_2) -> Float { return (300.0 + k_2); }, kern::to<Float>(k_2))))).Result).Result, kPi);
            r = kern::formula([](Float the_result, Float kSpreadR) -> Float { return (kern::math::sqrt(the_result) * kSpreadR); }, hash01(mix(key, int64_of_int(kern::to<Int>(kern::formula([](Float k_2) -> Float { return (400.0 + k_2); }, kern::to<Float>(k_2))))).Result).Result, kSpreadR);
            p_3 = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(herd).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin(a) * r)); }, vector_break(herd).y, a, r), 0.0);
            if (Life_in_rings(m_2.rings, p_3).Result) {
              c_2.home = p_3;
              stop_3 = true;
            }
            if (logic_not(stop_3)) {
              k_2 += 1;
            }
          }
          c_2.color = rgb(list_get<U32>(kCoats, kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::mod(kern::math::trunc((the_result * 5.0)), 5.0); }, hash01(mix(key, Int64{0x7}).Result).Result)))).Result;
          at_4 = 0;
          end_3 = list_length<Int64>(self.dead_cows_);
          go_3 = math_compare<"<">(kern::to<Float>(at_4), kern::to<Float>(end_3));
          while (loop(go_3)) {
            if (int64_equal(self.dead_cows_[at_4], key)) {
              go_3 = false;
            } else {
              at_4 += 1;
              go_3 = math_compare<"<">(kern::to<Float>(at_4), kern::to<Float>(end_3));
            }
          }
          c_2.dead = logic_not(math_compare<"==">(kern::to<Float>(at_4), kern::to<Float>(list_length<Int64>(self.dead_cows_))));
          c_2.next_moo = kern::formula([](Float now, Float the_result) -> Float { return ((now + 8.0) + (the_result * 30.0)); }, now, hash01(mix(key, Int64{0x9}).Result).Result);
          c_2.phase = kern::formula([](Float the_result) -> Float { return (the_result * 6.0); }, hash01(key).Result);
          g = Life_graze_spot(self, c_2, int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now, Float kPeriod) -> Float { return kern::math::div(now, kPeriod); }, now, kPeriod)))).Result;
          c_2.pos = vector_make(vector_break(g).x, vector_break(g).y, city_stream_cpp::CityStream_ground_height(city, vector_break(g).x, vector_break(g).y).Result);
          list_add(next, c_2);
        }
        i += 1;
      }
      if (math_compare<">">(kern::formula([](Float a, Float kMaxCows) -> Float { return (!((a < 0.0) != (kMaxCows < 0.0) ? (kMaxCows < 0.0) : (a < kMaxCows))); }, kern::to<Float>(list_length<Life_Cow>(next)), kern::to<Float>(kMaxCows)), 0.5)) {
        stop = true;
      }
      if (logic_not(stop)) {
        at_2 += 1;
      }
    }
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it24.next();) {
      both = engine_physics_physics::Body_valid(self.cows_[n24.index].body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n24.index].body);
        both = body_valid_2;
      }
      if (both) {
        engine_world_destroy_body(self.world_, self.cows_[n24.index].body);
      }
    }
    self.cows_ = next;
    self.meadows_ = meadows;
  }

  void Life_kill_cow(Life& self, Life_Cow& c, LifeOut& out, Vector impulse = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    if (c.dead) {
      return;
    }
    c.dead = true;
    c.hp = 0.0;
    list_add(self.dead_cows_, c.key);
    list_add(out.cow_deaths, struct_make<LifeOut_CowDeath>(c.pos, impulse));
    list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::moo_pain, vector_add(c.pos, Vector{0.0, 0.0, 1.4}), 1.0, 1.4));
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    bs.type = physics_BodyType::dynamic;
    bs.position = vector_add(c.pos, Vector{0.0, 0.0, 1.12});
    bs.rotation = common::yaw_quat(c.yaw).Result;
    bs.linear_damping = 0.05;
    bs.angular_damping = 0.3;
    engine_world_create_body(n26, self.world_, bs);
    create_body = n26.result;
    c.body = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    ss.kind = physics_ShapeKind::box;
    ss.size = Vector{1.64, 0.68, 0.76};
    ss.material.density = kern::formula([](Float kCowMass) -> Float { return kern::math::div(kCowMass, ((1.64 * 0.68) * 0.76)); }, kCowMass);
    ss.material.friction = 0.7;
    ss.material.restitution = 0.05;
    ss.material.category_bits = Int64{0x1};
    engine_world_create_shape(n27, self.world_, c.body, ss);
    create_shape = n27.result;
    engine_world_apply_linear_impulse(self.world_, c.body, impulse, vector_add(c.pos, Vector{0.0, 0.0, 1.45}));
  }

  void Life_scare_cow(Life_Cow& c, Vector from = Vector{0.0, 0.0, 0.0}, Float now = 0.0, Float secs = 0.0) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    if (c.dead) {
      return;
    }
    d = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(c.pos).x, vector_break(from).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(c.pos).y, vector_break(from).y), 0.0);
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
    d = logic_select<Vector>(math_compare<">">(l, 0.001), vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), Vector{1.0, 0.0, 0.0});
    c.flee_v = vector_scale(d, 2.8);
    c.flee_until = kern::formula([](Float now, Float secs) -> Float { return (now + secs); }, now, secs);
  }

  void Life_update_cows(Life& self, const LifeView& view, const CityStream& city, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Bool skip = false;
    Bool body = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Bool skip_2 = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    physics_Pose p_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool body_valid_5 = false;
    Bool body_valid_6 = false;
    Bool both_3 = false;
    physics_Pose p_3 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose_3 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float v = 0.0;
    Vector linear_velocity_2 = Vector{0.0, 0.0, 0.0};
    Vector f = Vector{0.0, 0.0, 0.0};
    Float clock = 0.0;
    Int64 k = Int64{0x0};
    Vector A = Vector{0.0, 0.0, 0.0};
    Vector B = Vector{0.0, 0.0, 0.0};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Float walk = 0.0;
    Float u = 0.0;
    Float clock_2 = 0.0;
    Int64 k_2 = Int64{0x0};
    Vector A_2 = Vector{0.0, 0.0, 0.0};
    Vector B_2 = Vector{0.0, 0.0, 0.0};
    Vector d_3 = Vector{0.0, 0.0, 0.0};
    Float len_2 = 0.0;
    Float walk_2 = 0.0;
    Float u_2 = 0.0;
    Vector base = Vector{0.0, 0.0, 0.0};
    Bool running = false;
    Vector p_4 = Vector{0.0, 0.0, 0.0};
    Int m_at = 0;
    the_RayResult_ wall = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Float vx = 0.0;
    Bool walking = false;
    Float yaw = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Bool skip_3 = false;
    Vector dl = Vector{0.0, 0.0, 0.0};
    Vector local = Vector{0.0, 0.0, 0.0};
    Float sp = 0.0;
    Float share = 0.0;
    Float lost = 0.0;
    Float l = 0.0;
    if (self.remote_animals_) {
      for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it17.next();) {
        skip = false;
        both = engine_physics_physics::Body_valid(self.cows_[n17.index].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n17.index].body);
          both = body_valid_2;
        }
        body = both;
        if (body) {
          pose = engine_world_pose(self.world_, self.cows_[n17.index].body);
          p = pose;
          self.cows_[n17.index].pos = p.position;
          self.cows_[n17.index].rot = p.rotation;
          if (logic_and(self.cows_[n17.index].carried, logic_not(self.cows_[n17.index].dead))) {
            linear_velocity = engine_world_linear_velocity(self.world_, self.cows_[n17.index].body);
            self.cows_[n17.index].carried_still = kern::formula([](Float a, Float a_2, Float dt) -> Float { return ((a < 0.35) ? (a_2 + dt) : 0.0); }, vector_length(linear_velocity), self.cows_[n17.index].carried_still, dt);
            if (math_compare<">">(self.cows_[n17.index].carried_still, 1.0)) {
              engine_world_destroy_body(self.world_, self.cows_[n17.index].body);
              self.cows_[n17.index].body = struct_make<physics_Body>(Int64{0x0});
              self.cows_[n17.index].carried = false;
              self.cows_[n17.index].pos = vector_make(vector_break(self.cows_[n17.index].pos).x, vector_break(self.cows_[n17.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.cows_[n17.index].pos).x, vector_break(self.cows_[n17.index].pos).y).Result);
            }
          }
          skip = true;
        }
        if (logic_not(skip)) {
          want = vector_add(self.cows_[n17.index].net_to, vector_scale(self.cows_[n17.index].net_v, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.cows_[n17.index].net_at), 0.0, 0.8))));
          d = vector_sub(want, self.cows_[n17.index].pos);
          if (math_compare<">">(vector_length(d), 8.0)) {
            self.cows_[n17.index].pos = want;
          } else {
            self.cows_[n17.index].pos = vector_add(self.cows_[n17.index].pos, vector_scale(d, kern::formula([](Float dt) -> Float { return kern::math::min(1.0, (dt * 8.0)); }, dt)));
          }
          if (logic_and(logic_not(self.cows_[n17.index].carried), logic_not(self.cows_[n17.index].net_dead))) {
            self.cows_[n17.index].pos = vector_make(vector_break(self.cows_[n17.index].pos).x, vector_break(self.cows_[n17.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.cows_[n17.index].pos).x, vector_break(self.cows_[n17.index].pos).y).Result);
          }
          self.cows_[n17.index].rot = self.cows_[n17.index].net_rot;
          self.cows_[n17.index].dead = self.cows_[n17.index].net_dead;
          self.cows_[n17.index].phase = kern::formula([](Float a, Float dt, Float a_2) -> Float { return (a + (dt * ((a_2 != 0.0) ? 3.2 : 0.0))); }, self.cows_[n17.index].phase, dt, kern::to<Float>(self.cows_[n17.index].walking));
        }
      }
      return;
    }
    if (math_compare<">">(kern::formula([](Float now, Float last_herd_check_) -> Float { return (now - last_herd_check_); }, now, self.last_herd_check_), 2.0)) {
      self.last_herd_check_ = now;
      if (logic_or(math_compare<">">(dist2d_2(self.herds_for_, vector_make(vector_break(view.focus).x, vector_break(view.focus).y, 0.0)).Result, 150.0), logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector>(view.mates)), 0.0)), math_compare<">">(kern::formula([](Float now, Float herds_at_) -> Float { return (now - herds_at_); }, now, self.herds_at_), 6.0)))) {
        self.herds_at_ = now;
        self.herds_for_ = vector_make(vector_break(view.focus).x, vector_break(view.focus).y, 0.0);
        Life_rebuild_herds(self, view, city, now);
      }
    }
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it25.next();) {
      skip_2 = false;
      if (self.cows_[n25.index].dead) {
        both_2 = engine_physics_physics::Body_valid(self.cows_[n25.index].body).Result;
        if (both_2) {
          body_valid_4 = engine_world_body_valid(self.world_, self.cows_[n25.index].body);
          both_2 = body_valid_4;
        }
        if (both_2) {
          pose_2 = engine_world_pose(self.world_, self.cows_[n25.index].body);
          p_2 = pose_2;
          self.cows_[n25.index].pos = p_2.position;
          self.cows_[n25.index].rot = p_2.rotation;
        }
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (self.cows_[n25.index].carried) {
          both_3 = engine_physics_physics::Body_valid(self.cows_[n25.index].body).Result;
          if (both_3) {
            body_valid_6 = engine_world_body_valid(self.world_, self.cows_[n25.index].body);
            both_3 = body_valid_6;
          }
          if (logic_not(both_3)) {
            self.cows_[n25.index].carried = false;
            self.cows_[n25.index].body = struct_make<physics_Body>(Int64{0x0});
          } else {
            pose_3 = engine_world_pose(self.world_, self.cows_[n25.index].body);
            p_3 = pose_3;
            self.cows_[n25.index].pos = p_3.position;
            self.cows_[n25.index].rot = p_3.rotation;
            linear_velocity_2 = engine_world_linear_velocity(self.world_, self.cows_[n25.index].body);
            v = vector_length(linear_velocity_2);
            self.cows_[n25.index].carried_still = kern::formula([](Float v, Float a, Float dt) -> Float { return ((v < 0.35) ? (a + dt) : 0.0); }, v, self.cows_[n25.index].carried_still, dt);
            if (math_compare<">">(self.cows_[n25.index].carried_still, 1.0)) {
              engine_world_destroy_body(self.world_, self.cows_[n25.index].body);
              self.cows_[n25.index].body = struct_make<physics_Body>(Int64{0x0});
              self.cows_[n25.index].carried = false;
              f = engine_math_math::Quat_rotate(p_3.rotation, Vector{1.0, 0.0, 0.0}).Result;
              self.cows_[n25.index].yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(f).y, vector_break(f).x);
              clock = kern::formula([](Float epoch, Float now) -> Float { return ((epoch > 0.0) ? epoch : now); }, view.epoch, now);
              k = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float clock, Float kPeriod) -> Float { return kern::math::floor(kern::math::div(clock, kPeriod)); }, clock, kPeriod)));
              A = Life_graze_spot(self, self.cows_[n25.index], k).Result;
              B = Life_graze_spot(self, self.cows_[n25.index], int64_add(k, Int64{0x1})).Result;
              d_2 = vector_sub(B, A);
              len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_2).x, vector_break(d_2).y);
              walk = kern::formula([](Float kPeriod, Float len, Float kWalk) -> Float { return kern::math::min((kPeriod * 0.75), kern::math::div(len, kWalk)); }, kPeriod, len, kWalk);
              u = kern::formula([](Float clock, Float a, Float kPeriod, Float walk) -> Float { return kern::math::min(1.0, kern::math::div((clock - (a * kPeriod)), kern::math::max(0.001, walk))); }, clock, int64_to_float(k), kPeriod, walk);
              self.cows_[n25.index].off = vector_sub(vector_make(vector_break(p_3.position).x, vector_break(p_3.position).y, 0.0), vector_add(A, vector_scale(d_2, u)));
              self.cows_[n25.index].pos = vector_make(vector_break(p_3.position).x, vector_break(p_3.position).y, city_stream_cpp::CityStream_ground_height(city, vector_break(p_3.position).x, vector_break(p_3.position).y).Result);
            }
            skip_2 = true;
          }
        }
        if (logic_not(skip_2)) {
          clock_2 = kern::formula([](Float epoch, Float now) -> Float { return ((epoch > 0.0) ? epoch : now); }, view.epoch, now);
          k_2 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float clock_2, Float kPeriod) -> Float { return kern::math::floor(kern::math::div(clock_2, kPeriod)); }, clock_2, kPeriod)));
          A_2 = Life_graze_spot(self, self.cows_[n25.index], k_2).Result;
          B_2 = Life_graze_spot(self, self.cows_[n25.index], int64_add(k_2, Int64{0x1})).Result;
          d_3 = vector_sub(B_2, A_2);
          len_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_3).x, vector_break(d_3).y);
          walk_2 = kern::formula([](Float kPeriod, Float len_2, Float kWalk) -> Float { return kern::math::min((kPeriod * 0.75), kern::math::div(len_2, kWalk)); }, kPeriod, len_2, kWalk);
          u_2 = kern::formula([](Float clock_2, Float a, Float kPeriod, Float walk_2) -> Float { return kern::math::min(1.0, kern::math::div((clock_2 - (a * kPeriod)), kern::math::max(0.001, walk_2))); }, clock_2, int64_to_float(k_2), kPeriod, walk_2);
          base = vector_add(A_2, vector_scale(d_3, u_2));
          running = math_compare<"<">(now, self.cows_[n25.index].flee_until);
          if (running) {
            self.cows_[n25.index].off = vector_add(self.cows_[n25.index].off, vector_scale(self.cows_[n25.index].flee_v, dt));
          } else {
            self.cows_[n25.index].off = vector_scale(self.cows_[n25.index].off, kern::formula([](Float dt) -> Float { return kern::math::exp(kern::math::div((-dt), 14.0)); }, dt));
          }
          p_4 = vector_add(base, self.cows_[n25.index].off);
          m_at = logic_select<Int>(logic_and(math_compare<">=">(kern::to<Float>(self.cows_[n25.index].meadow), 0.0), math_compare<"<">(kern::to<Float>(self.cows_[n25.index].meadow), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Life_Meadow>(self.meadows_)))))))), self.cows_[n25.index].meadow, -1);
          if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(m_at), 0.0), logic_not(Life_in_rings(self.meadows_[m_at].rings, p_4).Result)), Life_in_rings(self.meadows_[m_at].rings, base).Result)) {
            self.cows_[n25.index].off = vector_scale(self.cows_[n25.index].off, 0.9);
            p_4 = vector_add(base, self.cows_[n25.index].off);
          }
          wall = common::cast(view.physics, vector_make(vector_break(self.cows_[n25.index].pos).x, vector_break(self.cows_[n25.index].pos).y, kern::formula([](Float z) -> Float { return (z + 0.9); }, vector_break(self.cows_[n25.index].pos).z)), vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.cows_[n25.index].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.cows_[n25.index].yaw), 0.0), 1.2, struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result;
          if (logic_and(logic_and(wall.hit, math_compare<"<">(vector_break(wall.normal).z, 0.5)), running)) {
            vx = vector_break(self.cows_[n25.index].flee_v).x;
            self.cows_[n25.index].flee_v = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(self.cows_[n25.index].flee_v).y), vx, 0.0);
            p_4 = vector_make(vector_break(self.cows_[n25.index].pos).x, vector_break(self.cows_[n25.index].pos).y, 0.0);
            self.cows_[n25.index].off = vector_sub(p_4, base);
          }
          walking = logic_or(running, logic_and(math_compare<"<">(u_2, 1.0), math_compare<">">(len_2, 0.2)));
          yaw = kern::formula([](Float running, Float y, Float x, Float len_2, Float y_2, Float x_2, Float a) -> Float { return ((running != 0.0) ? kern::math::atan2(y, x) : ((len_2 > 0.2) ? kern::math::atan2(y_2, x_2) : a)); }, kern::to<Float>(running), vector_break(self.cows_[n25.index].flee_v).y, vector_break(self.cows_[n25.index].flee_v).x, len_2, vector_break(d_3).y, vector_break(d_3).x, self.cows_[n25.index].yaw);
          self.cows_[n25.index].yaw = kern::formula([](Float a, Float the_result, Float dt, Float running) -> Float { return (a + (the_result * kern::math::min(1.0, (dt * ((running != 0.0) ? 6.0 : 2.2))))); }, self.cows_[n25.index].yaw, common::wrap_pi(kern::formula([](Float yaw, Float a) -> Float { return (yaw - a); }, yaw, self.cows_[n25.index].yaw)).Result, dt, kern::to<Float>(running));
          self.cows_[n25.index].pos = vector_make(vector_break(p_4).x, vector_break(p_4).y, city_stream_cpp::CityStream_ground_height(city, vector_break(p_4).x, vector_break(p_4).y).Result);
          self.cows_[n25.index].walking = walking;
          self.cows_[n25.index].phase = kern::formula([](Float a, Float dt, Float running, Float walking) -> Float { return (a + (dt * ((running != 0.0) ? 7.0 : ((walking != 0.0) ? 3.2 : 0.0)))); }, self.cows_[n25.index].phase, dt, kern::to<Float>(running), kern::to<Float>(walking));
          if (math_compare<">">(now, self.cows_[n25.index].next_moo)) {
            self.cows_[n25.index].next_moo = kern::formula([](Float now, Float the_result) -> Float { return ((now + 15.0) + (the_result * 30.0)); }, now, hash01(mix(self.cows_[n25.index].key, int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, now))).Result).Result);
            if (logic_and(math_compare<"<">(dist2d(self.cows_[n25.index].pos, view.focus).Result, 35.0), math_compare<">">(kern::formula([](Float now, Float last_moo_) -> Float { return (now - last_moo_); }, now, self.last_moo_), 2.5))) {
              self.last_moo_ = now;
              list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::moo, vector_add(self.cows_[n25.index].pos, Vector{0.0, 0.0, 1.4}), 0.7, kern::formula([](Float the_result) -> Float { return (0.95 + (the_result * 0.15)); }, hash01(self.cows_[n25.index].key).Result)));
            }
          }
          count = list_length<LifeCar>(view.cars);
          at = 0;
          stop = false;
          while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
            skip_3 = false;
            if (view.cars[at].airborne) {
              skip_3 = true;
            }
            if (logic_not(logic_or(stop, skip_3))) {
              dl = vector_sub(self.cows_[n25.index].pos, view.cars[at].position);
              if (math_compare<">">(kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(dl).x, vector_break(dl).y), 40.0)) {
                skip_3 = true;
              }
              if (logic_not(logic_or(stop, skip_3))) {
                local = common::to_local(view.cars[at].rotation, dl).Result;
                if (logic_or(math_compare<">">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(local).x), kern::formula([](Float x) -> Float { return (x + 0.55); }, vector_break(view.cars[at].half).x)), math_compare<">">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(local).y), kern::formula([](Float y) -> Float { return (y + 0.45); }, vector_break(view.cars[at].half).y)))) {
                  skip_3 = true;
                }
                if (logic_not(logic_or(stop, skip_3))) {
                  sp = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(view.cars[at].velocity).x, vector_break(view.cars[at].velocity).y);
                  share = kern::formula([](Float kCowMass, Float a) -> Float { return kern::math::div(kCowMass, (a + kCowMass)); }, kCowMass, view.cars[at].mass);
                  if (math_compare<">">(sp, 3.0)) {
                    lost = kern::formula([](Float sp, Float share) -> Float { return (sp * share); }, sp, share);
                    if (math_compare<">=">(kern::to<Float>(view.cars[at].car), 0.0)) {
                      list_add(out.impulses, struct_make<LifeOut_Impulse>(view.cars[at].car, vector_scale(vector_make(kern::formula([](Float x, Float sp) -> Float { return kern::math::div((-x), sp); }, vector_break(view.cars[at].velocity).x, sp), kern::formula([](Float y, Float sp) -> Float { return kern::math::div((-y), sp); }, vector_break(view.cars[at].velocity).y, sp), 0.0), kern::formula([](Float a, Float lost) -> Float { return (a * lost); }, view.cars[at].mass, lost))));
                    }
                    if (math_compare<">">(sp, 8.0)) {
                      Life_kill_cow(self, self.cows_[n25.index], out, vector_make(kern::formula([](Float x, Float kCowMass) -> Float { return ((x * kCowMass) * 1.1); }, vector_break(view.cars[at].velocity).x, kCowMass), kern::formula([](Float y, Float kCowMass) -> Float { return ((y * kCowMass) * 1.1); }, vector_break(view.cars[at].velocity).y, kCowMass), kern::formula([](Float kCowMass, Float sp) -> Float { return (kCowMass * kern::math::min(6.0, (sp * 0.35))); }, kCowMass, sp)), now);
                    } else {
                      self.cows_[n25.index].off = vector_add(self.cows_[n25.index].off, vector_scale(vector_make(kern::formula([](Float x, Float sp) -> Float { return kern::math::div(x, sp); }, vector_break(view.cars[at].velocity).x, sp), kern::formula([](Float y, Float sp) -> Float { return kern::math::div(y, sp); }, vector_break(view.cars[at].velocity).y, sp), 0.0), 1.2));
                      Life_scare_cow(self.cows_[n25.index], vector_make(vector_break(view.cars[at].position).x, vector_break(view.cars[at].position).y, 0.0), now, 4.5);
                      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::moo_pain, vector_add(self.cows_[n25.index].pos, Vector{0.0, 0.0, 1.4}), 0.9, 1.4));
                    }
                  } else {
                    l = kern::formula([](Float x, Float y) -> Float { return (kern::math::sqrt((x * x) + (y * y)) + 0.001); }, vector_break(dl).x, vector_break(dl).y);
                    self.cows_[n25.index].off = vector_add(self.cows_[n25.index].off, vector_scale(vector_make(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, vector_break(dl).x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, vector_break(dl).y, l), 0.0), 0.08));
                  }
                  stop = true;
                }
              }
            }
            if (logic_not(stop)) {
              at += 1;
            }
          }
        }
      }
    }
  }

  struct update_pigeons_matchResult { Bool Result = false; };
  KERN_PURE update_pigeons_matchResult update_pigeons_match(const Life_LooseBird& b) {
    return {logic_and(logic_not(b.held), math_compare<">">(b.t, 6.0))};
  }

  struct update_pigeons_far_from_allResult { Bool Result = false; };
  KERN_PURE update_pigeons_far_from_allResult update_pigeons_far_from_all(const LifeView& view, Vector me = Vector{0.0, 0.0, 0.0}, Vector at = Vector{0.0, 0.0, 0.0}) {
    if (math_compare<"<=">(dist2d_2(at, me).Result, kDespawn)) {
      return {false};
    }
    for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(view.mates))))); it32.next();) {
      if (math_compare<"<=">(dist2d_2(at, vector_make(vector_break(view.mates[n32.index]).x, vector_break(view.mates[n32.index]).y, 0.0)).Result, kDespawn)) {
        return {false};
      }
    }
    return {true};
  }

  struct update_pigeons_match_2Result { Bool Result = false; };
  KERN_PURE update_pigeons_match_2Result update_pigeons_match_2(const LifeView& view, const Life_Flock& f, Vector me = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_not(f.air), update_pigeons_far_from_all(view, me, f.at).Result)};
  }

  struct Life_blockedResult { Bool Result = false; };
  KERN_PURE Life_blockedResult Life_blocked(const LifeView& view, const CityStream& city, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float g = 0.0;
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (terrain_cpp::Terrain_water_at(city.terrain_, vector_break(p).x, vector_break(p).y).Result) {
      return {true};
    }
    g = city_stream_cpp::CityStream_ground_height(city, vector_break(p).x, vector_break(p).y).Result;
    r = common::cast(view.physics, vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float g) -> Float { return (g + 40.0); }, g)), Vector{0.0, 0.0, -1.0}, 45.0, struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result;
    return {logic_and(r.hit, math_compare<">">(vector_break(r.point).z, kern::formula([](Float g) -> Float { return (g + 0.35); }, g)))};
  }

  struct Life_spot_nearResult { Bool Result = false; };
  Life_spot_nearResult Life_spot_near(const Life& self, const LifeView& view, const CityStream& city, Rng& rng, Vector& out, Vector c = Vector{0.0, 0.0, 0.0}, Float r0 = 0.0, Float r1 = 0.0) {
    Float a = 0.0;
    Float r = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    for (auto it39 = flow_for(n39, 0, 23); it39.next();) {
      n40 = common::Rng_unit(rng);
      a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, n40.Result, kPi);
      n41 = common::Rng_range(rng, r0, r1);
      r = n41.Result;
      p = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(c).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin(a) * r)); }, vector_break(c).y, a, r), 0.0);
      if (logic_not(Life_blocked(view, city, p).Result)) {
        out = p;
        return {true};
      }
    }
    return {false};
  }

  struct Life_spawn_flockResult { Bool Result = false; };
  Life_spawn_flockResult Life_spawn_flock(Life& self, Rng& rng, const LifeView& view, const CityStream& city, Vector at = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Life_Flock f = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}};
    U32 n = 0;
    Life_Pigeon b = Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    Float a = 0.0;
    Float r = 0.0;
    f = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}};
    n43 = Life_spot_near(self, view, city, rng, f.at, at, 0.0, 3.0);
    if (logic_not(n43.Result)) {
      return {false};
    }
    n44 = common::Rng_next(rng);
    n = kern::to<Int>(kern::formula([](Float kPerFlockMin, Float the_result, Float kPerFlockMax) -> Float { return (kPerFlockMin + kern::math::mod(the_result, kern::math::wrap32(kern::math::wrap32(kPerFlockMax - kPerFlockMin) + 1.0))); }, kern::to<Float>(kPerFlockMin), kern::to<Float>(n44.Result), kern::to<Float>(kPerFlockMax)));
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it45.next();) {
      b = Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
      n46 = common::Rng_unit(rng);
      a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, n46.Result, kPi);
      n47 = common::Rng_unit(rng);
      r = kern::formula([](Float the_result) -> Float { return (kern::math::sqrt(the_result) * 2.4); }, n47.Result);
      b.o = vector_make(kern::formula([](Float a, Float r) -> Float { return (kern::math::cos(a) * r); }, a, r), kern::formula([](Float a, Float r) -> Float { return (kern::math::sin(a) * r); }, a, r), 0.0);
      n48 = common::Rng_unit(rng);
      b.yaw = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, n48.Result, kPi);
      n49 = common::Rng_unit(rng);
      b.peck = kern::formula([](Float the_result) -> Float { return (the_result * 10.0); }, n49.Result);
      n50 = common::Rng_unit(rng);
      b.flap = kern::formula([](Float the_result) -> Float { return (the_result * 6.0); }, n50.Result);
      n51 = common::Rng_range(rng, 0.0, 0.45);
      b.lag = n51.Result;
      b.pos = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, vector_break(f.at).x, vector_break(b.o).x), kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, vector_break(f.at).y, vector_break(b.o).y), city_stream_cpp::CityStream_ground_height(city, vector_break(f.at).x, vector_break(f.at).y).Result);
      list_add(f.birds, b);
    }
    n52 = common::Rng_range(rng, 25.0, 70.0);
    f.next_flight = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n52.Result);
    n53 = common::Rng_range(rng, 2.0, 9.0);
    f.coo_at = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n53.Result);
    self.flock_serial_ = kern::to<Int>(kern::formula([](Float flock_serial_) -> Float { return (flock_serial_ + 1.0); }, kern::to<Float>(self.flock_serial_)));
    f.id = self.flock_serial_;
    list_add(self.flocks_, f);
    return {true};
  }

  struct update_pigeons_match_3Result { Bool Result = false; };
  KERN_PURE update_pigeons_match_3Result update_pigeons_match_3(const Life_LooseBird& b) {
    return {logic_and(logic_not(b.held), math_compare<">">(b.t, 6.0))};
  }

  struct update_pigeons_match_4Result { Bool Result = false; };
  KERN_PURE update_pigeons_match_4Result update_pigeons_match_4(const Life_Pigeon& b, Float now = 0.0) {
    return {logic_and(math_compare<">=">(b.dead_at, 0.0), math_compare<">">(kern::formula([](Float now, Float dead_at) -> Float { return (now - dead_at); }, now, b.dead_at), 12.0))};
  }

  void Life_spook(Life& self, Life_Flock& f, Rng& rng, const LifeView& view, const CityStream& city, LifeOut& out, Vector from = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Float away = 0.0;
    Float far = 0.0;
    Vector dest = Vector{0.0, 0.0, 0.0};
    Bool both = false;
    if (f.air) {
      return;
    }
    n59 = common::Rng_range(rng, -0.6, 0.6);
    away = kern::formula([](Float y, Float y_2, Float x, Float x_2, Float the_result) -> Float { return (kern::math::atan2((y - y_2), (x - x_2)) + the_result); }, vector_break(f.at).y, vector_break(from).y, vector_break(f.at).x, vector_break(from).x, n59.Result);
    n60 = common::Rng_range(rng, 25.0, 55.0);
    far = n60.Result;
    dest = Vector{0.0, 0.0, 0.0};
    n61 = Life_spot_near(self, view, city, rng, dest, vector_add(f.at, vector_scale(vector_make(kern::formula([](Float away) -> Float { return kern::math::cos(away); }, away), kern::formula([](Float away) -> Float { return kern::math::sin(away); }, away), 0.0), far)), 0.0, 18.0);
    both = logic_not(n61.Result);
    if (both) {
      n62 = Life_spot_near(self, view, city, rng, dest, f.at, 20.0, 50.0);
      both = logic_not(n62.Result);
    }
    if (both) {
      dest = f.at;
    }
    f.air = true;
    f.t = 0.0;
    f.c = vector_add(f.at, vector_scale(vector_make(kern::formula([](Float away) -> Float { return kern::math::cos(away); }, away), kern::formula([](Float away) -> Float { return kern::math::sin(away); }, away), 0.0), 12.0));
    n63 = common::Rng_range(rng, 9.0, 16.0);
    f.r = n63.Result;
    n64 = common::Rng_range(rng, 9.0, 18.0);
    f.h = n64.Result;
    n65 = common::Rng_unit(rng);
    f.dir = kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n65.Result);
    f.a0 = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(f.at).y, vector_break(f.c).y, vector_break(f.at).x, vector_break(f.c).x);
    n66 = common::Rng_range(rng, 7.0, 13.0);
    f.dur = n66.Result;
    f.dest = dest;
    f.from = f.at;
    if (math_compare<"<">(dist2d(view.focus, vector_make(vector_break(f.at).x, vector_break(f.at).y, vector_break(view.focus).z)).Result, 45.0)) {
      n67 = common::Rng_range(rng, 0.9, 1.15);
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::flap, vector_make(vector_break(f.at).x, vector_break(f.at).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.5); }, city_stream_cpp::CityStream_ground_height(city, vector_break(f.at).x, vector_break(f.at).y).Result)), 0.8, n67.Result));
    }
  }

  void Life_update_pigeons(Life& self, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Bool skip = false;
    Float gz = 0.0;
    Vector want = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Bool moving = false;
    Bool skip_2 = false;
    Float hs = 0.0;
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Vector me = Vector{0.0, 0.0, 0.0};
    U32 want_2 = 0;
    Int at_2 = 0;
    Int kept_2 = 0;
    Int end_2 = 0;
    Bool both = false;
    U32 pick = 0;
    Int chosen = 0;
    Vector around = Vector{0.0, 0.0, 0.0};
    U32 there = 0;
    Vector at_3 = Vector{0.0, 0.0, 0.0};
    Bool both_2 = false;
    Bool skip_3 = false;
    Vector h = Vector{0.0, 0.0, 0.0};
    Float hs_2 = 0.0;
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Int at_4 = 0;
    Int kept_3 = 0;
    Int end_3 = 0;
    Int at_5 = 0;
    Int kept_4 = 0;
    Int end_4 = 0;
    Float dp = 0.0;
    Int mate = 0;
    Int i = 0;
    Bool stop = false;
    Int at_6 = 0;
    Int count = 0;
    Bool stop_2 = false;
    Bool skip_4 = false;
    Float gz_2 = 0.0;
    Float sp = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    Float k = 0.0;
    Bool moving_2 = false;
    Float t_2 = 0.0;
    Float k_2 = 0.0;
    Float ang = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector l = Vector{0.0, 0.0, 0.0};
    Float land = 0.0;
    Float lift = 0.0;
    Vector dst = Vector{0.0, 0.0, 0.0};
    Vector src = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float z = 0.0;
    Vector dv = Vector{0.0, 0.0, 0.0};
    if (self.remote_animals_) {
      if (logic_not(view.birds_on)) {
        list_clear(self.flocks_);
      }
      for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it28.next();) {
        for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n28.index].birds))))); it29.next();) {
          skip = false;
          if (math_compare<">=">(self.flocks_[n28.index].birds[n29.index].dead_at, 0.0)) {
            gz = city_stream_cpp::CityStream_ground_height(city, vector_break(self.flocks_[n28.index].birds[n29.index].pos).x, vector_break(self.flocks_[n28.index].birds[n29.index].pos).y).Result;
            if (logic_or(math_compare<">">(vector_break(self.flocks_[n28.index].birds[n29.index].pos).z, kern::formula([](Float gz) -> Float { return (gz + 0.01); }, gz)), math_compare<">">(vector_break(self.flocks_[n28.index].birds[n29.index].fall_v).z, 0.0))) {
              self.flocks_[n28.index].birds[n29.index].fall_v = vector_make(vector_break(self.flocks_[n28.index].birds[n29.index].fall_v).x, vector_break(self.flocks_[n28.index].birds[n29.index].fall_v).y, kern::formula([](Float z, Float dt) -> Float { return (z - (9.81 * dt)); }, vector_break(self.flocks_[n28.index].birds[n29.index].fall_v).z, dt));
              self.flocks_[n28.index].birds[n29.index].pos = vector_add(self.flocks_[n28.index].birds[n29.index].pos, vector_scale(self.flocks_[n28.index].birds[n29.index].fall_v, dt));
              if (math_compare<"<=">(vector_break(self.flocks_[n28.index].birds[n29.index].pos).z, gz)) {
                self.flocks_[n28.index].birds[n29.index].pos = vector_make(vector_break(self.flocks_[n28.index].birds[n29.index].pos).x, vector_break(self.flocks_[n28.index].birds[n29.index].pos).y, gz);
                self.flocks_[n28.index].birds[n29.index].fall_v = Vector{0.0, 0.0, 0.0};
              }
            }
            skip = true;
          }
          if (logic_not(skip)) {
            want = vector_add(self.flocks_[n28.index].birds[n29.index].net_to, vector_scale(self.flocks_[n28.index].birds[n29.index].net_v, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.flocks_[n28.index].birds[n29.index].net_at), 0.0, 0.6))));
            d = vector_sub(want, self.flocks_[n28.index].birds[n29.index].pos);
            moving = math_compare<">">(vector_length(self.flocks_[n28.index].birds[n29.index].net_v), 0.05);
            if (math_compare<">">(vector_length(d), 15.0)) {
              self.flocks_[n28.index].birds[n29.index].pos = want;
            } else {
              self.flocks_[n28.index].birds[n29.index].pos = vector_add(self.flocks_[n28.index].birds[n29.index].pos, vector_scale(d, kern::formula([](Float dt) -> Float { return kern::math::min(1.0, (dt * 10.0)); }, dt)));
            }
            if (logic_not(self.flocks_[n28.index].air)) {
              self.flocks_[n28.index].birds[n29.index].pos = vector_make(vector_break(self.flocks_[n28.index].birds[n29.index].pos).x, vector_break(self.flocks_[n28.index].birds[n29.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.flocks_[n28.index].birds[n29.index].pos).x, vector_break(self.flocks_[n28.index].birds[n29.index].pos).y).Result);
            }
            self.flocks_[n28.index].birds[n29.index].yaw = kern::formula([](Float a, Float the_result, Float dt) -> Float { return (a + (the_result * kern::math::min(1.0, (dt * 8.0)))); }, self.flocks_[n28.index].birds[n29.index].yaw, common::wrap_pi(kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.flocks_[n28.index].birds[n29.index].net_yaw, self.flocks_[n28.index].birds[n29.index].yaw)).Result, dt);
            self.flocks_[n28.index].birds[n29.index].flap = kern::formula([](Float a, Float dt, Float a_2) -> Float { return (a + (dt * ((a_2 != 0.0) ? 16.0 : 0.0))); }, self.flocks_[n28.index].birds[n29.index].flap, dt, kern::to<Float>(self.flocks_[n28.index].air));
            self.flocks_[n28.index].birds[n29.index].peck = kern::formula([](Float a, Float dt, Float a_2, Float moving) -> Float { return (a + (dt * ((a_2 != 0.0) ? 0.0 : ((moving != 0.0) ? 9.0 : 3.0)))); }, self.flocks_[n28.index].birds[n29.index].peck, dt, kern::to<Float>(self.flocks_[n28.index].air), kern::to<Float>(moving));
          }
        }
      }
    }
    if (self.remote_animals_) {
      for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it30.next();) {
        skip_2 = false;
        if (self.loose_[n30.index].held) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.loose_[n30.index].t = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, self.loose_[n30.index].t, dt);
          self.loose_[n30.index].flap = kern::formula([](Float a, Float dt) -> Float { return (a + (dt * 22.0)); }, self.loose_[n30.index].flap, dt);
          self.loose_[n30.index].vel = vector_make(vector_break(self.loose_[n30.index].vel).x, vector_break(self.loose_[n30.index].vel).y, kern::formula([](Float z, Float dt) -> Float { return (z + ((4.0 - z) * kern::math::min(1.0, (dt * 1.5)))); }, vector_break(self.loose_[n30.index].vel).z, dt));
          self.loose_[n30.index].pos = vector_add(self.loose_[n30.index].pos, vector_scale(self.loose_[n30.index].vel, dt));
          hs = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(self.loose_[n30.index].vel).x, vector_break(self.loose_[n30.index].vel).y);
          if (math_compare<">">(hs, 0.1)) {
            self.loose_[n30.index].yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(self.loose_[n30.index].vel).y, vector_break(self.loose_[n30.index].vel).x);
          }
        }
      }
      at = 0;
      kept = at;
      end = list_length<Life_LooseBird>(self.loose_);
      while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
        if (logic_not(update_pigeons_match(self.loose_[at]).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
            var_take(n31, self.loose_[at]);
            self.loose_[kept] = n31.value;
          }
          kept += 1;
        }
        at += 1;
      }
      list_resize(self.loose_, kept, Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}});
      return;
    }
    me = vector_make(vector_break(view.focus).x, vector_break(view.focus).y, 0.0);
    want_2 = kern::to<Int>(kern::formula([](Float mobile_) -> Float { return ((mobile_ != 0.0) ? 3.0 : 5.0); }, kern::to<Float>(self.mobile_)));
    at_2 = 0;
    kept_2 = at_2;
    end_2 = list_length<Life_Flock>(self.flocks_);
    while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2)))) {
      if (logic_not(update_pigeons_match_2(view, self.flocks_[at_2], me).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_2))) {
          var_take(n33, self.flocks_[at_2]);
          self.flocks_[kept_2] = n33.value;
        }
        kept_2 += 1;
      }
      at_2 += 1;
    }
    list_resize(self.flocks_, kept_2, Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}});
    if (logic_not(view.birds_on)) {
      list_clear(self.flocks_);
      return;
    }
    both = math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Life_Flock>(self.flocks_)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float want_2, Float a) -> Float { return (want_2 * (1.0 + a)); }, kern::to<Float>(want_2), kern::to<Float>(list_length<Vector>(view.mates))))))), 0.5);
    if (both) {
      n35 = common::Rng_unit(rng);
      both = math_compare<"<">(n35.Result, kern::formula([](Float dt, Float a) -> Float { return (1.0 - kern::math::exp((((-3.0) * dt) * (a < 0.0 ? a + 18446744073709551616.0 : a)))); }, dt, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (1.0 + a); }, kern::to<Float>(list_length<Vector>(view.mates))))))));
    }
    if (both) {
      if (math_compare<"==">(kern::to<Float>(list_length<Vector>(view.mates)), 0.0)) {
        chosen = 0;
      } else {
        n37 = common::Rng_next(rng);
        chosen = kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, kern::math::wrap32((1.0 + a))); }, kern::to<Float>(n37.Result), kern::to<Float>(list_length<Vector>(view.mates))));
      }
      pick = kern::to<Int>(kern::formula([](Float chosen) -> Float { return chosen; }, kern::to<Float>(chosen)));
      around = logic_select<Vector>(math_compare<"==">(kern::to<Float>(pick), 0.0), me, vector_make(vector_break(view.mates[kern::to<Int>(kern::formula([](Float pick) -> Float { return kern::math::wrap32(pick - 1.0); }, kern::to<Float>(pick)))]).x, vector_break(view.mates[kern::to<Int>(kern::formula([](Float pick) -> Float { return kern::math::wrap32(pick - 1.0); }, kern::to<Float>(pick)))]).y, 0.0));
      there = 0;
      for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it38.next();) {
        there += kern::to<Int>(math_compare<"<=">(dist2d_2(self.flocks_[n38.index].at, around).Result, kDespawn));
      }
      at_3 = Vector{0.0, 0.0, 0.0};
      both_2 = math_compare<"<">(kern::to<Float>(there), kern::to<Float>(want_2));
      if (both_2) {
        n42 = Life_spot_near(self, view, city, rng, at_3, around, kNear, kFar);
        both_2 = n42.Result;
      }
      if (both_2) {
        n54 = Life_spawn_flock(self, rng, view, city, at_3, now);
      }
    }
    for (auto it55 = flow_for(n55, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it55.next();) {
      skip_3 = false;
      if (self.loose_[n55.index].held) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        self.loose_[n55.index].t = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, self.loose_[n55.index].t, dt);
        self.loose_[n55.index].flap = kern::formula([](Float a, Float dt) -> Float { return (a + (dt * 22.0)); }, self.loose_[n55.index].flap, dt);
        self.loose_[n55.index].vel = vector_make(vector_break(self.loose_[n55.index].vel).x, vector_break(self.loose_[n55.index].vel).y, kern::formula([](Float z, Float dt) -> Float { return (z + ((4.0 - z) * kern::math::min(1.0, (dt * 1.5)))); }, vector_break(self.loose_[n55.index].vel).z, dt));
        h = vector_make(vector_break(self.loose_[n55.index].vel).x, vector_break(self.loose_[n55.index].vel).y, 0.0);
        hs_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(h).x, vector_break(h).y);
        if (math_compare<"<">(hs_2, 6.0)) {
          d_2 = logic_select<Vector>(math_compare<">">(hs_2, 0.1), vector_scale(h, kern::formula([](Float hs_2) -> Float { return kern::math::div(1.0, hs_2); }, hs_2)), vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.loose_[n55.index].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.loose_[n55.index].yaw), 0.0));
          self.loose_[n55.index].vel = vector_make(kern::formula([](Float x, Float x_2, Float dt) -> Float { return (x + ((x_2 * dt) * 6.0)); }, vector_break(self.loose_[n55.index].vel).x, vector_break(d_2).x, dt), vector_break(self.loose_[n55.index].vel).y, vector_break(self.loose_[n55.index].vel).z);
          self.loose_[n55.index].vel = vector_make(vector_break(self.loose_[n55.index].vel).x, kern::formula([](Float y, Float y_2, Float dt) -> Float { return (y + ((y_2 * dt) * 6.0)); }, vector_break(self.loose_[n55.index].vel).y, vector_break(d_2).y, dt), vector_break(self.loose_[n55.index].vel).z);
        }
        self.loose_[n55.index].pos = vector_add(self.loose_[n55.index].pos, vector_scale(self.loose_[n55.index].vel, dt));
        if (math_compare<">">(hs_2, 0.1)) {
          self.loose_[n55.index].yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(self.loose_[n55.index].vel).y, vector_break(self.loose_[n55.index].vel).x);
        }
      }
    }
    at_4 = 0;
    kept_3 = at_4;
    end_3 = list_length<Life_LooseBird>(self.loose_);
    while (loop(math_compare<"<">(kern::to<Float>(at_4), kern::to<Float>(end_3)))) {
      if (logic_not(update_pigeons_match_3(self.loose_[at_4]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_3), kern::to<Float>(at_4))) {
          var_take(n56, self.loose_[at_4]);
          self.loose_[kept_3] = n56.value;
        }
        kept_3 += 1;
      }
      at_4 += 1;
    }
    list_resize(self.loose_, kept_3, Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}});
    for (auto it57 = flow_for(n57, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it57.next();) {
      at_5 = 0;
      kept_4 = at_5;
      end_4 = list_length<Life_Pigeon>(self.flocks_[n57.index].birds);
      while (loop(math_compare<"<">(kern::to<Float>(at_5), kern::to<Float>(end_4)))) {
        if (logic_not(update_pigeons_match_4(self.flocks_[n57.index].birds[at_5], now).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept_4), kern::to<Float>(at_5))) {
            var_take(n58, self.flocks_[n57.index].birds[at_5]);
            self.flocks_[n57.index].birds[kept_4] = n58.value;
          }
          kept_4 += 1;
        }
        at_5 += 1;
      }
      list_resize(self.flocks_[n57.index].birds, kept_4, Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0});
      if (logic_not(self.flocks_[n57.index].air)) {
        dp = dist2d_2(me, self.flocks_[n57.index].at).Result;
        if (logic_and(math_compare<"<">(dp, kern::formula([](Float kSpookPlayer) -> Float { return (kSpookPlayer + 2.4); }, kSpookPlayer)), math_compare<">">(view.player_speed, 0.3))) {
          Life_spook(self, self.flocks_[n57.index], rng, view, city, out, me, now);
        } else {
          mate = list_length<Vector>(view.mates);
          i = 0;
          stop = false;
          while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(view.mates))), 0.5)))) {
            if (math_compare<"<">(dist2d_2(vector_make(vector_break(view.mates[i]).x, vector_break(view.mates[i]).y, 0.0), self.flocks_[n57.index].at).Result, kern::formula([](Float kSpookPlayer) -> Float { return (kSpookPlayer + 1.4); }, kSpookPlayer))) {
              mate = i;
              stop = true;
            }
            if (logic_not(stop)) {
              i += 1;
            }
          }
          if (math_compare<">">(kern::formula([](Float mate, Float a) -> Float { return ((mate < 0.0) != (a < 0.0) ? (a < 0.0) : (mate < a)); }, kern::to<Float>(mate), kern::to<Float>(list_length<Vector>(view.mates))), 0.5)) {
            Life_spook(self, self.flocks_[n57.index], rng, view, city, out, vector_make(vector_break(view.mates[mate]).x, vector_break(view.mates[mate]).y, 0.0), now);
          } else {
            count = list_length<LifeCar>(view.cars);
            at_6 = 0;
            stop_2 = false;
            while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at_6), kern::to<Float>(count))))) {
              if (logic_and(math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(view.cars[at_6].position).x, vector_break(self.flocks_[n57.index].at).x, vector_break(view.cars[at_6].position).y, vector_break(self.flocks_[n57.index].at).y), kSpookCar), math_compare<">">(vector_length(view.cars[at_6].velocity), 2.0))) {
                Life_spook(self, self.flocks_[n57.index], rng, view, city, out, vector_make(vector_break(view.cars[at_6].position).x, vector_break(view.cars[at_6].position).y, 0.0), now);
                stop_2 = true;
              }
              if (logic_not(stop_2)) {
                at_6 += 1;
              }
            }
          }
        }
        if (logic_and(logic_not(self.flocks_[n57.index].air), math_compare<">">(now, self.flocks_[n57.index].next_flight))) {
          n68 = common::Rng_range(rng, -1.0, 1.0);
          n69 = common::Rng_range(rng, -1.0, 1.0);
          Life_spook(self, self.flocks_[n57.index], rng, view, city, out, vector_add(self.flocks_[n57.index].at, vector_make(n68.Result, n69.Result, 0.0)), now);
        }
        if (logic_and(logic_not(self.flocks_[n57.index].air), math_compare<">">(now, self.flocks_[n57.index].coo_at))) {
          n70 = common::Rng_range(rng, 3.5, 11.0);
          self.flocks_[n57.index].coo_at = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n70.Result);
          if (logic_and(math_compare<"<">(dp, 30.0), math_compare<">">(kern::formula([](Float now, Float last_coo_) -> Float { return (now - last_coo_); }, now, self.last_coo_), 1.6))) {
            self.last_coo_ = now;
            n71 = common::Rng_range(rng, 0.92, 1.08);
            list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::coo, vector_make(vector_break(self.flocks_[n57.index].at).x, vector_break(self.flocks_[n57.index].at).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.3); }, city_stream_cpp::CityStream_ground_height(city, vector_break(self.flocks_[n57.index].at).x, vector_break(self.flocks_[n57.index].at).y).Result)), kern::formula([](Float dp) -> Float { return (0.6 * kern::math::max(0.15, (1.0 - kern::math::div(dp, 30.0)))); }, dp), n71.Result));
          }
        }
      } else {
        self.flocks_[n57.index].t = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, self.flocks_[n57.index].t, dt);
        if (math_compare<">=">(self.flocks_[n57.index].t, self.flocks_[n57.index].dur)) {
          self.flocks_[n57.index].air = false;
          self.flocks_[n57.index].at = self.flocks_[n57.index].dest;
          n72 = common::Rng_range(rng, 30.0, 80.0);
          self.flocks_[n57.index].next_flight = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n72.Result);
          for (auto it73 = flow_for(n73, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n57.index].birds))))); it73.next();) {
            n74 = common::Rng_range(rng, -2.2, 2.2);
            n75 = common::Rng_range(rng, -2.2, 2.2);
            self.flocks_[n57.index].birds[n73.index].o = vector_make(n74.Result, n75.Result, 0.0);
          }
        }
      }
      for (auto it76 = flow_for(n76, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n57.index].birds))))); it76.next();) {
        skip_4 = false;
        if (math_compare<">=">(self.flocks_[n57.index].birds[n76.index].dead_at, 0.0)) {
          gz_2 = city_stream_cpp::CityStream_ground_height(city, vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y).Result;
          if (logic_or(math_compare<">">(vector_break(self.flocks_[n57.index].birds[n76.index].pos).z, kern::formula([](Float gz_2) -> Float { return (gz_2 + 0.01); }, gz_2)), math_compare<">">(vector_break(self.flocks_[n57.index].birds[n76.index].fall_v).z, 0.0))) {
            self.flocks_[n57.index].birds[n76.index].fall_v = vector_make(vector_break(self.flocks_[n57.index].birds[n76.index].fall_v).x, vector_break(self.flocks_[n57.index].birds[n76.index].fall_v).y, kern::formula([](Float z, Float dt) -> Float { return (z - (9.81 * dt)); }, vector_break(self.flocks_[n57.index].birds[n76.index].fall_v).z, dt));
            self.flocks_[n57.index].birds[n76.index].pos = vector_add(self.flocks_[n57.index].birds[n76.index].pos, vector_scale(self.flocks_[n57.index].birds[n76.index].fall_v, dt));
            if (math_compare<"<=">(vector_break(self.flocks_[n57.index].birds[n76.index].pos).z, gz_2)) {
              self.flocks_[n57.index].birds[n76.index].pos = vector_make(vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y, gz_2);
              self.flocks_[n57.index].birds[n76.index].fall_v = Vector{0.0, 0.0, 0.0};
            }
          }
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          if (logic_not(self.flocks_[n57.index].air)) {
            if (math_compare<">">(now, self.flocks_[n57.index].birds[n76.index].walk_until)) {
              n77 = common::Rng_unit(rng);
              if (math_compare<"<">(n77.Result, 0.6)) {
                n78 = common::Rng_range(rng, -1.4, 1.4);
                self.flocks_[n57.index].birds[n76.index].yaw = kern::formula([](Float a, Float the_result) -> Float { return (a + the_result); }, self.flocks_[n57.index].birds[n76.index].yaw, n78.Result);
                n79 = common::Rng_range(rng, 0.25, 0.55);
                sp = n79.Result;
                self.flocks_[n57.index].birds[n76.index].v = vector_make(kern::formula([](Float a, Float sp) -> Float { return (kern::math::cos(a) * sp); }, self.flocks_[n57.index].birds[n76.index].yaw, sp), kern::formula([](Float a, Float sp) -> Float { return (kern::math::sin(a) * sp); }, self.flocks_[n57.index].birds[n76.index].yaw, sp), 0.0);
                n80 = common::Rng_range(rng, 0.5, 1.8);
                self.flocks_[n57.index].birds[n76.index].walk_until = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n80.Result);
              } else {
                self.flocks_[n57.index].birds[n76.index].v = Vector{0.0, 0.0, 0.0};
                n81 = common::Rng_range(rng, 0.7, 2.4);
                self.flocks_[n57.index].birds[n76.index].walk_until = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n81.Result);
              }
            }
            n = vector_add(self.flocks_[n57.index].birds[n76.index].o, vector_scale(self.flocks_[n57.index].birds[n76.index].v, dt));
            if (math_compare<">">(kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(n).x, vector_break(n).y), 3.6)) {
              self.flocks_[n57.index].birds[n76.index].yaw = kern::formula([](Float a, Float kPi) -> Float { return (a + (kPi * 0.8)); }, self.flocks_[n57.index].birds[n76.index].yaw, kPi);
              self.flocks_[n57.index].birds[n76.index].v = vector_scale(self.flocks_[n57.index].birds[n76.index].v, -1.0);
              n = self.flocks_[n57.index].birds[n76.index].o;
            }
            self.flocks_[n57.index].birds[n76.index].o = n;
            t = vector_add(self.flocks_[n57.index].at, self.flocks_[n57.index].birds[n76.index].o);
            k = kern::formula([](Float dt) -> Float { return kern::math::min(1.0, (dt * 6.0)); }, dt);
            self.flocks_[n57.index].birds[n76.index].pos = vector_make(kern::formula([](Float x, Float x_2, Float k) -> Float { return (x + ((x_2 - x) * k)); }, vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, vector_break(t).x, k), vector_break(self.flocks_[n57.index].birds[n76.index].pos).y, vector_break(self.flocks_[n57.index].birds[n76.index].pos).z);
            self.flocks_[n57.index].birds[n76.index].pos = vector_make(vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, kern::formula([](Float y, Float y_2, Float k) -> Float { return (y + ((y_2 - y) * k)); }, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y, vector_break(t).y, k), vector_break(self.flocks_[n57.index].birds[n76.index].pos).z);
            self.flocks_[n57.index].birds[n76.index].pos = vector_make(vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.flocks_[n57.index].birds[n76.index].pos).x, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y).Result);
            moving_2 = math_compare<">">(kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(self.flocks_[n57.index].birds[n76.index].v).x, vector_break(self.flocks_[n57.index].birds[n76.index].v).y), 0.05);
            self.flocks_[n57.index].birds[n76.index].peck = kern::formula([](Float a, Float dt, Float moving_2) -> Float { return (a + (dt * ((moving_2 != 0.0) ? 9.0 : 3.0))); }, self.flocks_[n57.index].birds[n76.index].peck, dt, kern::to<Float>(moving_2));
          } else {
            t_2 = kern::formula([](Float a, Float a_2) -> Float { return kern::math::max(0.0, (a - a_2)); }, self.flocks_[n57.index].t, self.flocks_[n57.index].birds[n76.index].lag);
            k_2 = kern::formula([](Float t_2, Float a) -> Float { return kern::math::min(1.0, kern::math::div(t_2, a)); }, t_2, self.flocks_[n57.index].dur);
            ang = kern::formula([](Float a, Float a_2, Float t_2, Float x) -> Float { return ((a + ((a_2 * t_2) * 1.15)) + (x * 0.06)); }, self.flocks_[n57.index].a0, self.flocks_[n57.index].dir, t_2, vector_break(self.flocks_[n57.index].birds[n76.index].o).x);
            c = vector_add(self.flocks_[n57.index].c, vector_scale(self.flocks_[n57.index].birds[n76.index].o, 0.8));
            l = vector_add(c, vector_scale(vector_make(kern::formula([](Float ang) -> Float { return kern::math::cos(ang); }, ang), kern::formula([](Float ang) -> Float { return kern::math::sin(ang); }, ang), 0.0), self.flocks_[n57.index].r));
            land = kern::formula([](Float k_2) -> Float { return kern::math::max(0.0, kern::math::div((k_2 - 0.72), 0.28)); }, k_2);
            lift = kern::formula([](Float t_2) -> Float { return kern::math::min(1.0, kern::math::div(t_2, 1.2)); }, t_2);
            dst = vector_add(self.flocks_[n57.index].dest, self.flocks_[n57.index].birds[n76.index].o);
            src = vector_add(self.flocks_[n57.index].from, self.flocks_[n57.index].birds[n76.index].o);
            p = l;
            if (math_compare<"<">(t_2, 1.2)) {
              p = vector_add(src, vector_scale(vector_sub(l, src), lift));
            }
            if (math_compare<">">(land, 0.0)) {
              p = vector_add(l, vector_scale(vector_sub(dst, l), land));
            }
            z = kern::formula([](Float the_result, Float a, Float lift, Float land) -> Float { return ((the_result + (a * kern::math::min(lift, (1.0 - land)))) + 0.05); }, city_stream_cpp::CityStream_ground_height(city, vector_break(p).x, vector_break(p).y).Result, self.flocks_[n57.index].h, lift, land);
            dv = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(self.flocks_[n57.index].birds[n76.index].pos).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(self.flocks_[n57.index].birds[n76.index].pos).y), 0.0);
            if (math_compare<">">(kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(dv).x, vector_break(dv).y), 0.01)) {
              self.flocks_[n57.index].birds[n76.index].yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(dv).y, vector_break(dv).x);
            }
            self.flocks_[n57.index].birds[n76.index].pos = vector_make(vector_break(p).x, vector_break(p).y, z);
            self.flocks_[n57.index].birds[n76.index].flap = kern::formula([](Float a, Float dt, Float land, Float t_2) -> Float { return (a + (dt * (((land > 0.0) && (land < 1.0)) ? 9.0 : ((t_2 < 1.5) ? 24.0 : 14.0)))); }, self.flocks_[n57.index].birds[n76.index].flap, dt, land, t_2);
          }
        }
      }
    }
  }

  void Life_update_birds(Life& self, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Bool skip = false;
    Vector target = Vector{0.0, 0.0, 0.0};
    Float k = 0.0;
    Float gz = 0.0;
    Float phase = 0.0;
    Float a = 0.0;
    Float r = 0.0;
    Float z = 0.0;
    if (logic_not(view.birds_on)) {
      for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(self.birds_))))); it82.next();) {
        self.birds_[n82.index].dead_until = kern::formula([](Float a, Float now) -> Float { return kern::math::max(a, (now + 0.5)); }, self.birds_[n82.index].dead_until, now);
      }
      return;
    }
    for (auto it83 = flow_for(n83, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Bird>(self.birds_))))); it83.next();) {
      skip = false;
      target = vector_make(kern::formula([](Float x, Float a) -> Float { return ((x + kern::math::mod(kern::math::shr(a, 7.0), 25.0)) - 12.0); }, vector_break(view.focus).x, kern::to<Float>(self.birds_[n83.index].seed)), kern::formula([](Float y, Float a) -> Float { return ((y + kern::math::mod(kern::math::shr(a, 13.0), 25.0)) - 12.0); }, vector_break(view.focus).y, kern::to<Float>(self.birds_[n83.index].seed)), 0.0);
      if (logic_and(math_compare<">">(kern::formula([](Float x) -> Float { return (x == 0.0); }, vector_break(self.birds_[n83.index].center).x), 0.5), math_compare<">">(kern::formula([](Float y) -> Float { return (y == 0.0); }, vector_break(self.birds_[n83.index].center).y), 0.5))) {
        self.birds_[n83.index].center = target;
      }
      k = common::damp_alpha(0.03, dt).Result;
      self.birds_[n83.index].center = vector_add(self.birds_[n83.index].center, vector_scale(vector_sub(target, self.birds_[n83.index].center), k));
      if (math_compare<">">(dist2d_2(self.birds_[n83.index].center, target).Result, 400.0)) {
        self.birds_[n83.index].center = target;
      }
      if (math_compare<"<">(now, self.birds_[n83.index].dying_until)) {
        if (self.birds_[n83.index].down) {
          skip = true;
        }
        if (logic_not(skip)) {
          self.birds_[n83.index].fall_v = vector_make(vector_break(self.birds_[n83.index].fall_v).x, vector_break(self.birds_[n83.index].fall_v).y, kern::formula([](Float z, Float dt) -> Float { return (z - (5.4 * dt)); }, vector_break(self.birds_[n83.index].fall_v).z, dt));
          self.birds_[n83.index].pos = vector_add(self.birds_[n83.index].pos, vector_scale(self.birds_[n83.index].fall_v, dt));
          self.birds_[n83.index].yaw = kern::formula([](Float a, Float a_2, Float dt) -> Float { return (a + (a_2 * dt)); }, self.birds_[n83.index].yaw, self.birds_[n83.index].spin, dt);
          gz = city_stream_cpp::CityStream_ground_height(city, vector_break(self.birds_[n83.index].pos).x, vector_break(self.birds_[n83.index].pos).y).Result;
          if (math_compare<"<=">(vector_break(self.birds_[n83.index].pos).z, kern::formula([](Float gz) -> Float { return (gz + 0.05); }, gz))) {
            self.birds_[n83.index].pos = vector_make(vector_break(self.birds_[n83.index].pos).x, vector_break(self.birds_[n83.index].pos).y, kern::formula([](Float gz) -> Float { return (gz + 0.05); }, gz));
            self.birds_[n83.index].down = true;
            self.birds_[n83.index].dying_until = self.birds_[n83.index].dead_until;
            n84 = common::Rng_range(rng, 1.3, 1.6);
            list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::thud, self.birds_[n83.index].pos, 0.55, n84.Result));
            n85 = common::Rng_range(rng, 1.1, 1.4);
            list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::flap, self.birds_[n83.index].pos, 0.35, n85.Result));
          } else if (math_compare<">=">(kern::formula([](Float now, Float dt) -> Float { return (now + dt); }, now, dt), self.birds_[n83.index].dying_until)) {
            self.birds_[n83.index].dying_until = kern::formula([](Float now) -> Float { return (now + 0.2); }, now);
          }
          skip = true;
        }
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(now, self.birds_[n83.index].dead_until)) {
          skip = true;
        }
        if (logic_not(skip)) {
          phase = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::mod(a, 10000.0), 10000.0); }, kern::to<Float>(self.birds_[n83.index].seed));
          a = kern::formula([](Float now, Float a, Float phase, Float kPi) -> Float { return ((now * (0.22 + kern::math::div(kern::math::mod(a, 22.0), 1000.0))) + ((phase * 2.0) * kPi)); }, now, kern::to<Float>(self.birds_[n83.index].seed), phase, kPi);
          r = kern::formula([](Float a) -> Float { return (18.0 + kern::math::mod(a, 34.0)); }, kern::to<Float>(self.birds_[n83.index].seed));
          z = kern::formula([](Float a, Float a_2, Float index) -> Float { return ((8.0 + kern::math::mod(a, 13.0)) + (kern::math::sin(((a_2 * 2.3) + (index < 0.0 ? index + 18446744073709551616.0 : index))) * 2.1)); }, kern::to<Float>(self.birds_[n83.index].seed), a, kern::to<Float>(n83.index));
          self.birds_[n83.index].pos = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(self.birds_[n83.index].center).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin((a * 0.94)) * r)); }, vector_break(self.birds_[n83.index].center).y, a, r), kern::formula([](Float the_result, Float z) -> Float { return (the_result + z); }, city_stream_cpp::CityStream_ground_height(city, vector_break(self.birds_[n83.index].center).x, vector_break(self.birds_[n83.index].center).y).Result, z));
          self.birds_[n83.index].yaw = kern::formula([](Float a) -> Float { return kern::math::atan2((0.94 * kern::math::cos((a * 0.94))), (-kern::math::sin(a))); }, a);
          self.birds_[n83.index].flap = kern::formula([](Float now, Float index) -> Float { return (0.82 + (0.2 * kern::math::sin(((now * 10.0) + (index < 0.0 ? index + 18446744073709551616.0 : index))))); }, now, kern::to<Float>(n83.index));
        }
      }
    }
  }

  struct update_trees_key_ofResult { Int Result = 0; };
  KERN_PURE update_trees_key_ofResult update_trees_key_of(Int64 tile = Int64{0x0}, U32 index = 0) {
    return {int64_to_int(int64_add(int64_mul(tile, Int64{0x9e3779b97f4a7c15}), int64_of_int(index)))};
  }

  struct update_trees_matchResult { Bool Result = false; };
  KERN_PURE update_trees_matchResult update_trees_match(const Life_Tree& t, Float now = 0.0) {
    return {logic_and(math_compare<">=">(t.fell_at, 0.0), math_compare<">">(kern::formula([](Float now, Float fell_at) -> Float { return (now - fell_at); }, now, t.fell_at), 40.0))};
  }

  void Life_update_trees(Life& self, const LifeView& view, const CityStream& city, const List<Vec4>& ground_changes, Float now = 0.0) {
    Int64 key = Int64{0x0};
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    List<Int64> tiles = List<Int64>{};
    Map<Int64, Life_Tree> fallen = Map<Int64, Life_Tree>{};
    Life_Tree put = Life_Tree{Vector{0.0, 0.0, 0.0}, 1.0, 0, -1.0, Vector{1.0, 0.0, 0.0}, Int64{0x0}, 0};
    Int64 key_2 = Int64{0x0};
    Set<Int64> gone = Set<Int64>{};
    Int64 tile = Int64{0x0};
    List<MapFeatures_Spot> spots = List<MapFeatures_Spot>{};
    Bool skip = false;
    MapFeatures_Spot sp = MapFeatures_Spot{Vector{0.0, 0.0, 0.0}, 0.0};
    Int64 k = Int64{0x0};
    Int64 key_3 = Int64{0x0};
    Life_Tree t = Life_Tree{Vector{0.0, 0.0, 0.0}, 1.0, 0, -1.0, Vector{1.0, 0.0, 0.0}, Int64{0x0}, 0};
    Int it = 0;
    for (auto it86 = flow_for(n86, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(self.trees_))))); it86.next();) {
      if (logic_and(math_compare<">=">(self.trees_[n86.index].fell_at, 0.0), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.trees_[n86.index].fell_at), 40.0))) {
        key = int64_of_int(update_trees_key_of(self.trees_[n86.index].tile, self.trees_[n86.index].index).Result);
        set_add(n87, self.trees_gone_, key);
      }
    }
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Life_Tree>(self.trees_)), 0.0))) {
      at = 0;
      kept = at;
      end = list_length<Life_Tree>(self.trees_);
      while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
        if (logic_not(update_trees_match(self.trees_[at], now).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
            var_take(n88, self.trees_[at]);
            self.trees_[kept] = n88.value;
          }
          kept += 1;
        }
        at += 1;
      }
      list_resize(self.trees_, kept, Life_Tree{Vector{0.0, 0.0, 0.0}, 1.0, 0, -1.0, Vector{1.0, 0.0, 0.0}, Int64{0x0}, 0});
    }
    if (math_compare<"!=">(kern::to<Float>(self.tree_serial_), kern::to<Float>(city_stream::CityStream_furniture_serial(city).Result))) {
      self.tree_serial_ = city_stream::CityStream_furniture_serial(city).Result;
      tiles = List<Int64>{};
      city_stream_cpp::CityStream_furniture(city, tiles);
      fallen = Map<Int64, Life_Tree>{};
      for (auto it89 = flow_for(n89, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(self.trees_))))); it89.next();) {
        if (math_compare<">=">(self.trees_[n89.index].fell_at, 0.0)) {
          put = self.trees_[n89.index];
          key_2 = int64_of_int(update_trees_key_of(self.trees_[n89.index].tile, self.trees_[n89.index].index).Result);
          map_add(n90, fallen, key_2);
          fallen[n90.index] = put;
        }
      }
      gone = Set<Int64>{};
      list_clear(self.trees_);
      for (auto it91 = flow_for(n91, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(tiles))))); it91.next();) {
        tile = tiles[n91.index];
        spots = struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city, tile).Result).trees;
        for (auto it92 = flow_for(n92, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Spot>(spots))))); it92.next();) {
          skip = false;
          sp = spots[n92.index];
          k = int64_of_int(update_trees_key_of(tile, n92.index).Result);
          if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(self.trees_gone_, k)))), 0.0)) {
            key_3 = k;
            set_add(n93, gone, key_3);
            skip = true;
          }
          if (logic_not(skip)) {
            if (terrain_cpp::Terrain_water_at(city.terrain_, vector_break(sp.p).x, vector_break(sp.p).y).Result) {
              skip = true;
            }
            if (logic_not(skip)) {
              t = struct_make<Life_Tree>(vector_make(vector_break(sp.p).x, vector_break(sp.p).y, city_stream_cpp::CityStream_ground_height(city, vector_break(sp.p).x, vector_break(sp.p).y).Result), sp.a, 0, -1.0, Vector{1.0, 0.0, 0.0}, tile, n92.index);
              it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Life_Tree>(fallen, k).index), kern::to<Float>(map_length<Int64, Life_Tree>(fallen))));
              if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Life_Tree>(fallen))))) {
                t.fell_at = fallen[it].fell_at;
                t.fall_dir = fallen[it].fall_dir;
              }
              list_add(self.trees_, t);
            }
          }
        }
      }
      self.trees_gone_ = gone;
    }
    for (auto it94 = flow_for(n94, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vec4>(ground_changes))))); it94.next();) {
      for (auto it95 = flow_for(n95, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(self.trees_))))); it95.next();) {
        if (logic_and(logic_and(logic_and(math_compare<">=">(vector_break(self.trees_[n95.index].foot).x, ground_changes[n94.index].x), math_compare<"<=">(vector_break(self.trees_[n95.index].foot).x, ground_changes[n94.index].z)), math_compare<">=">(vector_break(self.trees_[n95.index].foot).y, ground_changes[n94.index].y)), math_compare<"<=">(vector_break(self.trees_[n95.index].foot).y, ground_changes[n94.index].w))) {
          self.trees_[n95.index].foot = vector_make(vector_break(self.trees_[n95.index].foot).x, vector_break(self.trees_[n95.index].foot).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.trees_[n95.index].foot).x, vector_break(self.trees_[n95.index].foot).y).Result);
        }
      }
    }
  }

  void Life_update_pets_drop(Life& self, U32 n103_index = 0) {
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    both = engine_physics_physics::Body_valid(self.pets_[n103_index].body).Result;
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, self.pets_[n103_index].body);
      both = body_valid_2;
    }
    if (both) {
      engine_world_destroy_body(self.world_, self.pets_[n103_index].body);
    }
    self.pets_[n103_index].body = struct_make<physics_Body>(Int64{0x0});
  }

  void Life_update_pets(Life& self, const LifeView& view, const CityStream& city, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Vector me = Vector{0.0, 0.0, 0.0};
    U32 i = 0;
    U32 base = 0;
    List<Vector> want = List<Vector>{};
    Vector a = Vector{0.0, 0.0, 0.0};
    Bool near = false;
    Bool skip = false;
    Vector mp = Vector{0.0, 0.0, 0.0};
    Bool covered = false;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Bool placed = false;
    Int k = 0;
    Int had = 0;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Int k_2 = 0;
    Float t = 0.0;
    Vector head = Vector{0.0, 0.0, 0.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float d = 0.0;
    Float step = 0.0;
    Float k_3 = 0.0;
    Bool skip_2 = false;
    Life_Species sp = Life_Species::dog;
    Bool allowed = false;
    U32 seed = 0;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    Bool body_valid_5 = false;
    Bool body_valid_6 = false;
    Bool both_3 = false;
    Bool both_4 = false;
    Vector to = Vector{0.0, 0.0, 0.0};
    Bool the_round = false;
    Float a_3 = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Vector last = Vector{0.0, 0.0, 0.0};
    Bool body_valid_7 = false;
    Bool body_valid_8 = false;
    Bool both_5 = false;
    physics_Pose q_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool body_valid_9 = false;
    Bool body_valid_10 = false;
    Bool both_6 = false;
    physics_Pose q_3 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Float a0 = 0.0;
    Vector q0 = Vector{0.0, 0.0, 0.0};
    Float cs = 0.0;
    Float a_4 = 0.0;
    Vector q_4 = Vector{0.0, 0.0, 0.0};
    Vector last_2 = Vector{0.0, 0.0, 0.0};
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Bool skip_3 = false;
    Float sp2 = 0.0;
    Vector l = Vector{0.0, 0.0, 0.0};
    self.epoch_ = view.epoch;
    me = vector_make(vector_break(view.focus).x, vector_break(view.focus).y, 0.0);
    if (logic_not(self.remote_animals_)) {
      if (math_compare<">">(dist2d_2(self.pet_anchor_, me).Result, 2500.0)) {
        self.pet_anchor_ = vector_make(kern::formula([](Float x) -> Float { return (kern::math::round(kern::math::div(x, 100.0)) * 100.0); }, vector_break(me).x), kern::formula([](Float y) -> Float { return (kern::math::round(kern::math::div(y, 100.0)) * 100.0); }, vector_break(me).y), 0.0);
        i = 0;
        while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(self.pet_base_)), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Life_Pet>(self.pets_))), 0.5)))) {
          self.pets_[i].shown = false;
          self.pets_[i].has_pose = false;
          i += 1;
        }
      }
      base = kern::to<Int>(kern::formula([](Float pet_base_) -> Float { return ((pet_base_ != 0.0) ? pet_base_ : 20.0); }, kern::to<Float>(self.pet_base_)));
      want = list_make<Vector, "1">(self.pet_anchor_);
      for (auto it96 = flow_for(n96, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(self.pet_anchors_))))); it96.next();) {
        a = self.pet_anchors_[n96.index];
        near = false;
        for (auto it97 = flow_for(n97, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(view.mates))))); it97.next();) {
          near = logic_or(near, math_compare<"<=">(dist2d_2(a, vector_make(vector_break(view.mates[n97.index]).x, vector_break(view.mates[n97.index]).y, 0.0)).Result, 2500.0));
        }
        list_add(want, logic_select<Vector>(near, a, Vector{1000000000.0, 1000000000.0, 0.0}));
      }
      for (auto it98 = flow_for(n98, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(view.mates))))); it98.next();) {
        skip = false;
        mp = vector_make(vector_break(view.mates[n98.index]).x, vector_break(view.mates[n98.index]).y, 0.0);
        covered = false;
        for (auto it99 = flow_for(n99, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(want))))); it99.next();) {
          covered = logic_or(covered, math_compare<"<=">(dist2d_2(want[n99.index], mp).Result, 2500.0));
        }
        if (covered) {
          skip = true;
        }
        if (logic_not(skip)) {
          a_2 = vector_make(kern::formula([](Float x) -> Float { return (kern::math::round(kern::math::div(x, 100.0)) * 100.0); }, vector_break(mp).x), kern::formula([](Float y) -> Float { return (kern::math::round(kern::math::div(y, 100.0)) * 100.0); }, vector_break(mp).y), 0.0);
          placed = false;
          k = 1;
          while (loop(logic_and(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(want))), 0.5), logic_not(placed)))) {
            if (math_compare<">">(vector_break(want[k]).x, 100000000.0)) {
              want[k] = a_2;
              placed = true;
            }
            k += 1;
          }
          if (logic_and(logic_not(placed), math_compare<"<">(kern::to<Float>(list_length<Vector>(want)), 8.0))) {
            list_add(want, a_2);
          }
        }
      }
      while (loop(logic_and(math_compare<">">(kern::to<Float>(list_length<Vector>(want)), 1.0), math_compare<">">(vector_break(want[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(want))))]).x, 100000000.0)))) {
        list_remove_at(want, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(want)))));
      }
      if (math_compare<"!=">(kern::to<Float>(list_length<Life_Pet>(self.pets_)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float base, Float a) -> Float { return (base * a); }, kern::to<Float>(base), kern::to<Float>(list_length<Vector>(want)))))))) {
        had = list_length<Life_Pet>(self.pets_);
        for (auto it100 = flow_for(n100, kern::to<Int>(kern::formula([](Float base, Float a) -> Float { return (base * a); }, kern::to<Float>(base), kern::to<Float>(list_length<Vector>(want)))), kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(had)))); it100.next();) {
          both = engine_physics_physics::Body_valid(self.pets_[n100.index].body).Result;
          if (both) {
            body_valid_2 = engine_world_body_valid(self.world_, self.pets_[n100.index].body);
            both = body_valid_2;
          }
          if (both) {
            engine_world_destroy_body(self.world_, self.pets_[n100.index].body);
          }
        }
        list_resize(self.pets_, kern::to<Int>(kern::formula([](Float base, Float a) -> Float { return (base * a); }, kern::to<Float>(base), kern::to<Float>(list_length<Vector>(want)))), Life_Pet{20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, physics_Body{Int64{0x0}}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}});
        for (auto it101 = flow_for(n101, had, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it101.next();) {
          Life_pet_params(self.pets_[n101.index], kern::to<Int>(kern::formula([](Float index, Float base) -> Float { return kern::math::wrap32(kern::math::mod(index, base)); }, kern::to<Float>(n101.index), kern::to<Float>(base))));
        }
      }
      k_2 = 1;
      while (loop(logic_and(math_compare<">">(kern::formula([](Float k_2, Float a) -> Float { return ((k_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (k_2 < a)); }, kern::to<Float>(k_2), kern::to<Float>(list_length<Vector>(want))), 0.5), math_compare<">">(kern::formula([](Float k_2, Float a) -> Float { return ((k_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (k_2 < a)); }, kern::to<Float>(k_2), kern::to<Float>(list_length<Vector>(self.pet_anchors_))), 0.5)))) {
        if (math_compare<">">(dist2d_2(want[k_2], self.pet_anchors_[k_2]).Result, 1.0)) {
          for (auto it102 = flow_for(n102, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(base)))); it102.next();) {
            self.pets_[kern::to<Int>(kern::formula([](Float k_2, Float base, Float index) -> Float { return ((k_2 * base) + index); }, kern::to<Float>(k_2), kern::to<Float>(base), kern::to<Float>(n102.index)))].shown = false;
            self.pets_[kern::to<Int>(kern::formula([](Float k_2, Float base, Float index) -> Float { return ((k_2 * base) + index); }, kern::to<Float>(k_2), kern::to<Float>(base), kern::to<Float>(n102.index)))].has_pose = false;
            self.pets_[kern::to<Int>(kern::formula([](Float k_2, Float base, Float index) -> Float { return ((k_2 * base) + index); }, kern::to<Float>(k_2), kern::to<Float>(base), kern::to<Float>(n102.index)))].off = Vector{0.0, 0.0, 0.0};
          }
        }
        k_2 += 1;
      }
      self.pet_anchors_ = want;
    }
    t = kern::formula([](Float epoch) -> Float { return kern::math::mod(epoch, (86400.0 * 4.0)); }, view.epoch);
    if (self.cat_.on) {
      if (logic_not(view.cats_on)) {
        self.cat_.on = false;
      }
      head = view.head;
      dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(head).x, vector_break(self.cat_.p).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(head).y, vector_break(self.cat_.p).y);
      d = kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt((dx * dx) + (dy * dy)); }, dx, dy);
      if (math_compare<"==">(kern::to<Float>(self.cat_.phase), 2.0)) {
        self.cat_.p = vector_add(self.cat_.p, vector_scale(vector_make(kern::formula([](Float away_yaw) -> Float { return kern::math::cos(away_yaw); }, self.cat_.away_yaw), kern::formula([](Float away_yaw) -> Float { return kern::math::sin(away_yaw); }, self.cat_.away_yaw), 0.0), kern::formula([](Float dt) -> Float { return (1.3 * dt); }, dt)));
        self.cat_.yaw = self.cat_.away_yaw;
        self.cat_.p = vector_make(vector_break(self.cat_.p).x, vector_break(self.cat_.p).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.cat_.p).x, vector_break(self.cat_.p).y).Result);
        if (logic_or(math_compare<">">(d, 28.0), math_compare<">">(kern::formula([](Float now, Float leave_at) -> Float { return (now - leave_at); }, now, self.cat_.leave_at), 20.0))) {
          self.cat_.on = false;
        }
      } else if (logic_not(view.on_foot)) {
        self.cat_.on = false;
      } else {
        self.cat_.yaw = kern::formula([](Float dy, Float dx) -> Float { return kern::math::atan2(dy, dx); }, dy, dx);
        if (math_compare<"==">(kern::to<Float>(self.cat_.phase), 0.0)) {
          if (math_compare<">">(kern::formula([](Float now, Float last_cry) -> Float { return (now - last_cry); }, now, self.cat_.last_cry), 0.78)) {
            self.cat_.last_cry = now;
            self.cat_.cries = kern::to<Int>(kern::formula([](Float cries) -> Float { return (cries + 1.0); }, kern::to<Float>(self.cat_.cries)));
            list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(math_compare<">">(kern::formula([](Float cries) -> Float { return (kern::math::mod(cries, 2.0) != 0.0); }, kern::to<Float>(self.cat_.cries)), 0.5), LifeOut_Sound::yowl, LifeOut_Sound::hiss), self.cat_.p, 1.0, 1.0));
          }
          step = kern::formula([](Float d, Float dt) -> Float { return kern::math::min(d, (11.0 * dt)); }, d, dt);
          self.cat_.p = vector_make(kern::formula([](Float x, Float dx, Float d, Float step) -> Float { return (x + (kern::math::div(dx, kern::math::max(0.001, d)) * step)); }, vector_break(self.cat_.p).x, dx, d, step), vector_break(self.cat_.p).y, vector_break(self.cat_.p).z);
          self.cat_.p = vector_make(vector_break(self.cat_.p).x, kern::formula([](Float y, Float dy, Float d, Float step) -> Float { return (y + (kern::math::div(dy, kern::math::max(0.001, d)) * step)); }, vector_break(self.cat_.p).y, dy, d, step), vector_break(self.cat_.p).z);
          self.cat_.p = vector_make(vector_break(self.cat_.p).x, vector_break(self.cat_.p).y, kern::formula([](Float the_result, Float now) -> Float { return (the_result + (kern::math::abs(kern::math::sin(kern::math::div(now, 0.055))) * 0.12)); }, city_stream_cpp::CityStream_ground_height(city, vector_break(self.cat_.p).x, vector_break(self.cat_.p).y).Result, now));
          if (math_compare<"<">(d, 1.6)) {
            self.cat_.phase = 1;
            self.cat_.leap_at = now;
            self.cat_.from = self.cat_.p;
            list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::screech, self.cat_.p, 1.0, 1.0));
          }
          if (math_compare<">">(kern::formula([](Float now, Float start) -> Float { return (now - start); }, now, self.cat_.start), 25.0)) {
            self.cat_.on = false;
          }
        } else {
          k_3 = kern::formula([](Float now, Float leap_at) -> Float { return kern::math::min(1.0, kern::math::div((now - leap_at), 0.34)); }, now, self.cat_.leap_at);
          self.cat_.p = vector_make(kern::formula([](Float x, Float x_2, Float k_3) -> Float { return (x + ((x_2 - x) * k_3)); }, vector_break(self.cat_.from).x, vector_break(head).x, k_3), vector_break(self.cat_.p).y, vector_break(self.cat_.p).z);
          self.cat_.p = vector_make(vector_break(self.cat_.p).x, kern::formula([](Float y, Float y_2, Float k_3) -> Float { return (y + ((y_2 - y) * k_3)); }, vector_break(self.cat_.from).y, vector_break(head).y, k_3), vector_break(self.cat_.p).z);
          self.cat_.p = vector_make(vector_break(self.cat_.p).x, vector_break(self.cat_.p).y, kern::formula([](Float the_result, Float z, Float z_2, Float k_3, Float kPi) -> Float { return kern::math::max((the_result + 0.18), ((z + (((z_2 - 0.05) - z) * k_3)) + (kern::math::sin((k_3 * kPi)) * 0.55))); }, city_stream_cpp::CityStream_ground_height(city, vector_break(self.cat_.p).x, vector_break(self.cat_.p).y).Result, vector_break(self.cat_.from).z, vector_break(head).z, k_3, kPi));
          if (math_compare<">=">(k_3, 1.0)) {
            out.kill_player = true;
            self.cat_.phase = 2;
            self.cat_.leave_at = now;
            self.cat_.away_yaw = kern::formula([](Float yaw, Float kPi, Float the_result) -> Float { return ((yaw + kPi) + ((the_result - 0.5) * 0.8)); }, self.cat_.yaw, kPi, hash01(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)))).Result);
          }
        }
      }
    }
    for (auto it103 = flow_for(n103, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it103.next();) {
      skip_2 = false;
      sp = Life_species(self, n103.index, view.epoch).Result;
      allowed = logic_select<Bool>(enum_equal<Life_Species>(sp, Life_Species::dog), view.dogs_on, view.cats_on);
      seed = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc((the_result * 1000.0)); }, hash01(mix(Int64{0xc010}, int64_of_int(n103.index)).Result).Result));
      self.pets_[n103.index].color = logic_select<Vector>(enum_equal<Life_Species>(sp, Life_Species::black_cat), rgb(855311).Result, logic_select<Vector>(enum_equal<Life_Species>(sp, Life_Species::cat), rgb(list_get<U32>(kCatColors, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(seed, 4.0); }, kern::to<Float>(seed))))).Result, rgb(list_get<U32>(kDogColors, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(seed, 5.0); }, kern::to<Float>(seed))))).Result));
      if (logic_not(allowed)) {
        Life_update_pets_drop(self, n103.index);
        self.pets_[n103.index].shown = false;
        self.pets_[n103.index].dead_at = -1.0;
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        both_4 = self.remote_animals_;
        if (both_4) {
          both_3 = engine_physics_physics::Body_valid(self.pets_[n103.index].body).Result;
          if (both_3) {
            body_valid_6 = engine_world_body_valid(self.world_, self.pets_[n103.index].body);
            both_3 = body_valid_6;
          }
          both_4 = logic_not(both_3);
        }
        if (both_4) {
          if (math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n103.index), kern::to<Float>(list_length<Vector>(self.pet_net_to_))), 0.5)) {
            to = self.pet_net_to_[n103.index];
            the_round = logic_and(logic_and(logic_and(self.pets_[n103.index].shown, math_compare<"<">(self.pets_[n103.index].dead_at, 0.0)), logic_not(self.pets_[n103.index].carried)), logic_not(logic_and(self.cat_.on, math_compare<"==">(kern::to<Float>(n103.index), 1.0))));
            if (the_round) {
              self.pets_[n103.index].off = vector_scale(self.pets_[n103.index].off, kern::formula([](Float dt) -> Float { return kern::math::exp(kern::math::div((-dt), 5.0)); }, dt));
              a_3 = kern::formula([](Float a, Float a_2, Float t) -> Float { return (a + (a_2 * t)); }, self.pets_[n103.index].phase, self.pets_[n103.index].omega, t);
              q = vector_add(vector_add(vector_add(life::Life_anchor_of(self, n103.index).Result, vector_scale(vector_make(kern::formula([](Float a_3) -> Float { return kern::math::cos(a_3); }, a_3), kern::formula([](Float a_3) -> Float { return kern::math::sin(a_3); }, a_3), 0.0), self.pets_[n103.index].radius)), vector_scale(vector_make(kern::formula([](Float a, Float t) -> Float { return kern::math::cos((a * t)); }, self.pets_[n103.index].wobble_omega, t), kern::formula([](Float a, Float t) -> Float { return kern::math::sin(((a * t) * 1.3)); }, self.pets_[n103.index].wobble_omega, t), 0.0), self.pets_[n103.index].wobble_r)), self.pets_[n103.index].off);
              to = vector_make(vector_break(q).x, vector_break(q).y, city_stream_cpp::CityStream_ground_height(city, vector_break(q).x, vector_break(q).y).Result);
              if (logic_and(self.pets_[n103.index].has_pose, math_compare<">">(dist2d_2(vector_make(vector_break(q).x, vector_break(q).y, 0.0), vector_make(vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y, 0.0)).Result, 0.0001))) {
                self.pet_net_yaw_[n103.index] = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(q).y, vector_break(self.pets_[n103.index].pos).y, vector_break(q).x, vector_break(self.pets_[n103.index].pos).x);
              }
            }
            d_2 = vector_sub(to, self.pets_[n103.index].pos);
            last = vector_make(vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y, 0.0);
            if (logic_or(logic_or(logic_not(self.pets_[n103.index].has_pose), math_compare<">">(vector_length(d_2), 6.0)), the_round)) {
              self.pets_[n103.index].pos = to;
            } else {
              self.pets_[n103.index].pos = vector_add(self.pets_[n103.index].pos, vector_scale(d_2, kern::formula([](Float dt) -> Float { return kern::math::min(1.0, (dt * 7.0)); }, dt)));
            }
            if (logic_and(logic_not(self.pets_[n103.index].carried), math_compare<"<">(self.pets_[n103.index].dead_at, 0.0))) {
              self.pets_[n103.index].pos = vector_make(vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y).Result);
            }
            self.pets_[n103.index].yaw = kern::formula([](Float a, Float the_result, Float dt) -> Float { return (a + (the_result * kern::math::min(1.0, (dt * 8.0)))); }, self.pets_[n103.index].yaw, common::wrap_pi(kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.pet_net_yaw_[n103.index], self.pets_[n103.index].yaw)).Result, dt);
            if (logic_and(self.pets_[n103.index].has_pose, math_compare<">">(dt, 0.0))) {
              self.pets_[n103.index].speed = kern::formula([](Float the_result, Float dt) -> Float { return kern::math::min(6.0, kern::math::div(the_result, dt)); }, dist2d_2(vector_make(vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y, 0.0), last).Result, dt);
            }
            self.pets_[n103.index].gait = kern::formula([](Float a, Float a_2, Float dt, Float a_3) -> Float { return (a + kern::math::div((a_2 * dt), (0.55 * a_3))); }, self.pets_[n103.index].gait, self.pets_[n103.index].speed, dt, self.pets_[n103.index].scale);
            self.pets_[n103.index].rot = self.pets_[n103.index].net_rot;
            self.pets_[n103.index].has_pose = true;
          }
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          if (math_compare<">=">(self.pets_[n103.index].dead_at, 0.0)) {
            if (math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.pets_[n103.index].dead_at), 25.0)) {
              Life_update_pets_drop(self, n103.index);
              self.pets_[n103.index].dead_at = -1.0;
              self.pets_[n103.index].shown = false;
              self.pets_[n103.index].has_pose = false;
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              both_5 = engine_physics_physics::Body_valid(self.pets_[n103.index].body).Result;
              if (both_5) {
                body_valid_8 = engine_world_body_valid(self.world_, self.pets_[n103.index].body);
                both_5 = body_valid_8;
              }
              if (both_5) {
                pose = engine_world_pose(self.world_, self.pets_[n103.index].body);
                q_2 = pose;
                self.pets_[n103.index].pos = q_2.position;
                self.pets_[n103.index].rot = q_2.rotation;
              }
              skip_2 = true;
            }
          }
          if (logic_not(skip_2)) {
            if (self.pets_[n103.index].carried) {
              both_6 = engine_physics_physics::Body_valid(self.pets_[n103.index].body).Result;
              if (both_6) {
                body_valid_10 = engine_world_body_valid(self.world_, self.pets_[n103.index].body);
                both_6 = body_valid_10;
              }
              if (logic_not(both_6)) {
                self.pets_[n103.index].carried = false;
                self.pets_[n103.index].body = struct_make<physics_Body>(Int64{0x0});
              } else {
                pose_2 = engine_world_pose(self.world_, self.pets_[n103.index].body);
                q_3 = pose_2;
                self.pets_[n103.index].pos = q_3.position;
                self.pets_[n103.index].rot = q_3.rotation;
                linear_velocity = engine_world_linear_velocity(self.world_, self.pets_[n103.index].body);
                self.pets_[n103.index].carried_still = kern::formula([](Float a, Float a_2, Float dt) -> Float { return ((a < 0.3) ? (a_2 + dt) : 0.0); }, vector_length(linear_velocity), self.pets_[n103.index].carried_still, dt);
                if (math_compare<">">(self.pets_[n103.index].carried_still, 0.8)) {
                  Life_update_pets_drop(self, n103.index);
                  self.pets_[n103.index].carried = false;
                  a0 = kern::formula([](Float a, Float a_2, Float t) -> Float { return (a + (a_2 * t)); }, self.pets_[n103.index].phase, self.pets_[n103.index].omega, t);
                  q0 = vector_add(vector_add(life::Life_anchor_of(self, n103.index).Result, vector_scale(vector_make(kern::formula([](Float a0) -> Float { return kern::math::cos(a0); }, a0), kern::formula([](Float a0) -> Float { return kern::math::sin(a0); }, a0), 0.0), self.pets_[n103.index].radius)), vector_scale(vector_make(kern::formula([](Float a, Float t) -> Float { return kern::math::cos((a * t)); }, self.pets_[n103.index].wobble_omega, t), kern::formula([](Float a, Float t) -> Float { return kern::math::sin(((a * t) * 1.3)); }, self.pets_[n103.index].wobble_omega, t), 0.0), self.pets_[n103.index].wobble_r));
                  self.pets_[n103.index].off = vector_sub(vector_make(vector_break(q_3.position).x, vector_break(q_3.position).y, 0.0), q0);
                  self.pets_[n103.index].has_pose = false;
                }
                skip_2 = true;
              }
            }
            if (logic_not(skip_2)) {
              self.pets_[n103.index].off = vector_scale(self.pets_[n103.index].off, kern::formula([](Float dt) -> Float { return kern::math::exp(kern::math::div((-dt), 5.0)); }, dt));
              if (logic_and(self.cat_.on, math_compare<"==">(kern::to<Float>(n103.index), 1.0))) {
                cs = kern::formula([](Float phase) -> Float { return ((phase == 2.0) ? 1.3 : 11.0); }, kern::to<Float>(self.cat_.phase));
                self.pets_[n103.index].pos = self.cat_.p;
                self.pets_[n103.index].yaw = self.cat_.yaw;
                self.pets_[n103.index].speed = cs;
                self.pets_[n103.index].gait = kern::formula([](Float a, Float cs, Float dt) -> Float { return (a + kern::math::div((cs * dt), 0.4)); }, self.pets_[n103.index].gait, cs, dt);
                self.pets_[n103.index].shown = true;
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                a_4 = kern::formula([](Float a, Float a_2, Float t) -> Float { return (a + (a_2 * t)); }, self.pets_[n103.index].phase, self.pets_[n103.index].omega, t);
                q_4 = vector_add(vector_add(vector_add(life::Life_anchor_of(self, n103.index).Result, vector_scale(vector_make(kern::formula([](Float a_4) -> Float { return kern::math::cos(a_4); }, a_4), kern::formula([](Float a_4) -> Float { return kern::math::sin(a_4); }, a_4), 0.0), self.pets_[n103.index].radius)), vector_scale(vector_make(kern::formula([](Float a, Float t) -> Float { return kern::math::cos((a * t)); }, self.pets_[n103.index].wobble_omega, t), kern::formula([](Float a, Float t) -> Float { return kern::math::sin(((a * t) * 1.3)); }, self.pets_[n103.index].wobble_omega, t), 0.0), self.pets_[n103.index].wobble_r)), self.pets_[n103.index].off);
                last_2 = vector_make(vector_break(self.pets_[n103.index].pos).x, vector_break(self.pets_[n103.index].pos).y, 0.0);
                if (logic_and(self.pets_[n103.index].has_pose, math_compare<">">(dist2d_2(q_4, last_2).Result, 0.0001))) {
                  self.pets_[n103.index].yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(q_4).y, vector_break(last_2).y, vector_break(q_4).x, vector_break(last_2).x);
                }
                self.pets_[n103.index].speed = kern::formula([](Float a, Float dt, Float the_result) -> Float { return (((a != 0.0) && (dt > 0.0)) ? kern::math::div(the_result, dt) : 0.0); }, kern::to<Float>(self.pets_[n103.index].has_pose), dt, dist2d_2(q_4, last_2).Result);
                if (math_compare<">">(self.pets_[n103.index].speed, 6.0)) {
                  self.pets_[n103.index].speed = 0.0;
                }
                self.pets_[n103.index].pos = vector_make(vector_break(q_4).x, vector_break(q_4).y, city_stream_cpp::CityStream_ground_height(city, vector_break(q_4).x, vector_break(q_4).y).Result);
                self.pets_[n103.index].gait = kern::formula([](Float a, Float a_2, Float dt, Float a_3) -> Float { return (a + kern::math::div((a_2 * dt), (0.55 * a_3))); }, self.pets_[n103.index].gait, self.pets_[n103.index].speed, dt, self.pets_[n103.index].scale);
                self.pets_[n103.index].has_pose = true;
                self.pets_[n103.index].shown = logic_not(Life_blocked(view, city, q_4).Result);
                if (logic_not(self.pets_[n103.index].shown)) {
                  skip_2 = true;
                }
                if (logic_not(skip_2)) {
                  count = list_length<LifeCar>(view.cars);
                  at = 0;
                  stop = false;
                  while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
                    skip_3 = false;
                    if (view.cars[at].airborne) {
                      skip_3 = true;
                    }
                    if (logic_not(logic_or(stop, skip_3))) {
                      sp2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(view.cars[at].velocity).x, vector_break(view.cars[at].velocity).y);
                      if (math_compare<"<">(sp2, 3.0)) {
                        skip_3 = true;
                      }
                      if (logic_not(logic_or(stop, skip_3))) {
                        l = common::to_local(view.cars[at].rotation, vector_sub(self.pets_[n103.index].pos, view.cars[at].position)).Result;
                        if (logic_and(logic_and(math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(l).x), kern::formula([](Float x) -> Float { return (x + 0.3); }, vector_break(view.cars[at].half).x)), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(l).y), kern::formula([](Float y) -> Float { return (y + 0.25); }, vector_break(view.cars[at].half).y))), math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(l).z), 2.0))) {
                          if (enum_equal<Life_Species>(sp, Life_Species::black_cat)) {
                            skip_3 = true;
                          }
                          if (logic_not(logic_or(stop, skip_3))) {
                            Life_kill_pet(self, out, n103.index, vector_add(vector_scale(vector_make(vector_break(view.cars[at].velocity).x, vector_break(view.cars[at].velocity).y, 0.0), 0.9), Vector{0.0, 0.0, 2.5}), now);
                            stop = true;
                          }
                        }
                      }
                    }
                    if (logic_not(stop)) {
                      at += 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  struct Life_sampleResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  Life_sampleResult Life_sample(const Life& self, Float& yaw, Vector& normal, Float s = 0.0) {
    Float L = 0.0;
    Float u = 0.0;
    Float dir = 0.0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float seg = 0.0;
    Float t = 0.0;
    Float sl = 0.0;
    L = self.chain_.len;
    u = kern::formula([](Float s, Float l) -> Float { return kern::math::mod((kern::math::mod(s, (2.0 * l)) + (2.0 * l)), (2.0 * l)); }, s, L);
    dir = 1.0;
    if (math_compare<">">(u, L)) {
      u = kern::formula([](Float l, Float u) -> Float { return ((2.0 * l) - u); }, L, u);
      dir = -1.0;
    }
    i = 1;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Float>(self.chain_.cum))))))), 0.5), math_compare<"<">(self.chain_.cum[i], u)))) {
      i += 1;
    }
    a = self.chain_.path[kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)))];
    b = self.chain_.path[i];
    seg = kern::formula([](Float a, Float a_2) -> Float { return kern::math::max(0.001, (a - a_2)); }, self.chain_.cum[i], self.chain_.cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)))]);
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float u, Float a, Float seg) -> Float { return kern::math::div((u - a), seg); }, u, self.chain_.cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)))], seg), 0.0, 1.0));
    yaw = kern::formula([](Float y, Float y_2, Float dir, Float x, Float x_2) -> Float { return kern::math::atan2(((y - y_2) * dir), ((x - x_2) * dir)); }, vector_break(b).y, vector_break(a).y, dir, vector_break(b).x, vector_break(a).x);
    sl = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::max(0.001, kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y);
    normal = vector_scale(vector_make(kern::formula([](Float y, Float y_2, Float sl) -> Float { return kern::math::div((-(y - y_2)), sl); }, vector_break(b).y, vector_break(a).y, sl), kern::formula([](Float x, Float x_2, Float sl) -> Float { return kern::math::div((x - x_2), sl); }, vector_break(b).x, vector_break(a).x, sl), 0.0), kern::to<Float>(self.chain_.side));
    return {vector_make(kern::formula([](Float x, Float x_2, Float t) -> Float { return (x + ((x_2 - x) * t)); }, vector_break(a).x, vector_break(b).x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, vector_break(a).y, vector_break(b).y, t), 0.0)};
  }

  struct Life_spawn_joggersResult { Bool Result = false; };
  Life_spawn_joggersResult Life_spawn_joggers(Life& self, const LifeView& view, const CityStream& city, Rng& rng, Float now = 0.0) {
    List<Lane> lanes = List<Lane>{};
    Int best = 0;
    Float best_score = 0.0;
    Bool skip = false;
    Lane l = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    Float len = 0.0;
    Float near = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    Float score = 0.0;
    Lane lane = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    I32 side0 = 0;
    I32 item = 0;
    I32 item_2 = 0;
    Fixed<I32, 2> items = Fixed<I32, 2>{};
    I32 side = 0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Float item_5 = 0.0;
    Fixed<Float, 3> items_2 = Fixed<Float, 3>{};
    Bool skip_2 = false;
    Float kerb = 0.0;
    Life_Chain c = Life_Chain{0, List<Vector>{}, List<Float>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0, 0, 1, List<Life_Runner>{}};
    List<Vector> pts = List<Vector>{};
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l_2 = 0.0;
    Int i = 0;
    Bool free = false;
    Float yaw = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    Vector q_2 = Vector{0.0, 0.0, 0.0};
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector b_2 = Vector{0.0, 0.0, 0.0};
    Vector me = Vector{0.0, 0.0, 0.0};
    Vector q_3 = Vector{0.0, 0.0, 0.0};
    lanes = city.lanes_;
    best = list_length<Lane>(lanes);
    best_score = -1000000000.0;
    for (auto it106 = flow_for(n106, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it106.next();) {
      skip = false;
      l = lanes[n106.index];
      if (logic_or(logic_or(logic_not(l.alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(l.points)), 2.0)), math_compare<">">(l.width, 14.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        len = 0.0;
        near = 1000000000.0;
        for (auto it107 = flow_for(n107, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(l.points))))); it107.next();) {
          len += dist2d_2(l.points[n107.index], l.points[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n107.index)))]).Result;
        }
        if (math_compare<"<">(len, 60.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it108 = flow_for(n108, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(l.points))))); it108.next();) {
            q = l.points[n108.index];
            near = kern::formula([](Float near, Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::min(near, kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, near, vector_break(q).x, vector_break(view.player).x, vector_break(q).y, vector_break(view.player).y);
          }
          if (logic_or(math_compare<"<">(near, 35.0), math_compare<">">(near, 150.0))) {
            skip = true;
          }
          if (logic_not(skip)) {
            n109 = common::Rng_unit(rng);
            score = kern::formula([](Float len, Float near, Float the_result) -> Float { return ((kern::math::min(len, 260.0) - (near * 0.6)) + (the_result * 40.0)); }, len, near, n109.Result);
            if (math_compare<">">(score, best_score)) {
              best_score = score;
              best = n106.index;
            }
          }
        }
      }
    }
    if (math_compare<"==">(kern::to<Float>(best), kern::to<Float>(list_length<Lane>(lanes)))) {
      return {false};
    }
    lane = lanes[best];
    n110 = common::Rng_unit(rng);
    side0 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n110.Result));
    item = side0;
    item_2 = kern::to<Int>(kern::formula([](Float side0) -> Float { return (-side0); }, kern::to<Float>(side0)));
    items = list_make<I32, "2">(item, item_2);
    for (auto it111 = flow_for(n111, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(items))))); it111.next();) {
      side = items[n111.index];
      item_3 = 1.7;
      item_4 = 1.2;
      item_5 = 2.4;
      items_2 = list_make<Float, "3">(item_3, item_4, item_5);
      for (auto it112 = flow_for(n112, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_2))))); it112.next();) {
        kerb = items_2[n112.index];
        skip_2 = false;
        c = Life_Chain{0, List<Vector>{}, List<Float>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0, 0, 1, List<Life_Runner>{}};
        c.side = side;
        c.off = kern::formula([](Float width, Float kerb) -> Float { return ((width * 0.5) + kerb); }, lane.width, kerb);
        pts = lane.points;
        for (auto it113 = flow_for(n113, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it113.next();) {
          a = pts[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index == 0.0) ? 0.0 : (index - 1.0)); }, kern::to<Float>(n113.index)))];
          b = pts[kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::min((a - 1.0), (index + 1.0)); }, kern::to<Float>(list_length<Vector>(pts)), kern::to<Float>(n113.index)))];
          d = vector_sub(b, a);
          l_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::max(0.001, kern::math::sqrt((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
          d = vector_scale(d, kern::formula([](Float l_2) -> Float { return kern::math::div(1.0, l_2); }, l_2));
          list_add(c.path, vector_make(kern::formula([](Float x, Float y, Float off, Float side) -> Float { return (x - ((y * off) * side)); }, vector_break(pts[n113.index]).x, vector_break(d).y, c.off, kern::to<Float>(side)), kern::formula([](Float y, Float x, Float off, Float side) -> Float { return (y + ((x * off) * side)); }, vector_break(pts[n113.index]).y, vector_break(d).x, c.off, kern::to<Float>(side)), 0.0));
        }
        list_add(c.cum, 0.0);
        i = 1;
        while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(c.path))), 0.5))) {
          list_add(c.cum, kern::formula([](Float a, Float the_result) -> Float { return (a + the_result); }, c.cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(c.cum))))], dist2d_2(c.path[i], c.path[kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)))]).Result));
          i += 1;
        }
        c.len = c.cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(c.cum))))];
        if (math_compare<"<">(c.len, 50.0)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.chain_ = c;
          free = true;
          yaw = 0.0;
          n = Vector{0.0, 0.0, 0.0};
          s = 0.0;
          while (loop(logic_and(math_compare<"<=">(s, c.len), free))) {
            n114 = Life_sample(self, yaw, n, s);
            q_2 = n114.Result;
            free = logic_not(Life_blocked(view, city, vector_make(vector_break(q_2).x, vector_break(q_2).y, 0.0)).Result);
            s += 1.5;
          }
          if (logic_not(free)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            a_2 = c.path[0];
            b_2 = c.path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(c.path))))];
            me = vector_make(vector_break(view.player).x, vector_break(view.player).y, 0.0);
            self.chain_.s = kern::formula([](Float the_result, Float the_result_2, Float kRunners, Float kGap, Float len) -> Float { return ((the_result > the_result_2) ? (kRunners * kGap) : (len + (kRunners * kGap))); }, dist2d_2(a_2, me).Result, dist2d_2(b_2, me).Result, kern::to<Float>(kRunners), kGap, c.len);
            self.chain_serial_ = kern::to<Int>(kern::formula([](Float chain_serial_) -> Float { return (chain_serial_ + 1.0); }, kern::to<Float>(self.chain_serial_)));
            self.chain_.id = self.chain_serial_;
            self.chain_.born = now;
            self.chain_.next_call = kern::formula([](Float now) -> Float { return (now + 1.5); }, now);
            self.chain_.next_beat = now;
            n115 = common::Rng_next(rng);
            self.chain_.call = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::wrap32s(kern::math::mod(the_result, 6.0)); }, kern::to<Float>(n115.Result)));
            for (auto it116 = flow_for(n116, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it116.next();) {
              self.chain_.runners[n116.index] = struct_make<Life_Runner>(Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0);
              n117 = Life_sample(self, yaw, n, kern::formula([](Float s, Float index, Float kGap) -> Float { return (s - (index * kGap)); }, self.chain_.s, kern::to<Float>(n116.index), kGap));
              q_3 = n117.Result;
              self.chain_.runners[n116.index].pos = vector_make(vector_break(q_3).x, vector_break(q_3).y, city_stream_cpp::CityStream_ground_height(city, vector_break(q_3).x, vector_break(q_3).y).Result);
              self.chain_.runners[n116.index].yaw_s = yaw;
              self.chain_.runners[n116.index].yaw = self.chain_.runners[n116.index].yaw_s;
              n118 = common::Rng_unit(rng);
              self.chain_.runners[n116.index].phase = kern::formula([](Float the_result) -> Float { return (the_result * 6.0); }, n118.Result);
            }
            self.chain_alive_ = true;
            return {true};
          }
        }
      }
    }
    return {false};
  }

  void Life_kill_runner(Life& self, LifeOut& out, I32 i = 0, Vector velocity = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Ragdolls_Colors col = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false};
    Fixed<U32, 5> skins = Fixed<U32, 5>{};
    U32 left = 0;
    Text b = "";
    if (logic_not(self.chain_.runners[i].alive)) {
      return;
    }
    self.chain_.runners[i].alive = false;
    col = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false};
    col.shirt = rgb(14221114).Result;
    col.pants = rgb(1845818).Result;
    skins = list_make<U32, "5">(14725260, 12881516, 11038802, 8079923, 13804158);
    col.skin = rgb(skins[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 5.0); }, kern::to<Float>(i)))]).Result;
    n124 = ragdoll_cpp::Ragdolls_spawn(self.ragdolls_, col, int64_add(int64_of_int(kern::to<Int>(kern::formula([](Float id) -> Float { return ((268435456.0 + id) * 16.0); }, kern::to<Float>(self.chain_.id)))), int64_of_int(i)), self.chain_.runners[i].pos, self.chain_.runners[i].yaw_s, velocity, false, now);
    list_add(out.bursts, struct_make<LifeOut_Burst>(vector_add(self.chain_.runners[i].pos, Vector{0.0, 0.0, 1.1}), Vector{0.6, 0.02, 0.02}));
    if (logic_or(math_compare<"!=">(kern::to<Float>(self.streak_chain_), kern::to<Float>(self.chain_.id)), math_compare<">">(kern::formula([](Float now, Float streak_at_) -> Float { return (now - streak_at_); }, now, self.streak_at_), kStreakWindow))) {
      self.streak_n_ = 0;
      self.streak_chain_ = self.chain_.id;
    }
    self.streak_n_ = kern::to<Int>(kern::formula([](Float streak_n_) -> Float { return (streak_n_ + 1.0); }, kern::to<Float>(self.streak_n_)));
    self.streak_at_ = now;
    left = 0;
    for (auto it125 = flow_for(n125, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(self.chain_.runners))))); it125.next();) {
      left += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(self.chain_.runners[n125.index].alive)));
    }
    b = "";
    if (logic_not(math_compare<"!=">(kern::to<Float>(left), 0.0))) {
      b = text_format<"KOMPLETTE LAUFGRUPPE! x%u">(63, self.streak_n_);
      out.banner = b;
      out.banner_for = 2.6;
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::jingle, self.chain_.runners[i].pos, 1.0, 1.0));
    } else if (math_compare<">=">(kern::to<Float>(self.streak_n_), 2.0)) {
      b = text_format<"JOGGER x%u">(63, self.streak_n_);
      out.banner = b;
      out.banner_for = 1.3;
    }
  }

  void Life_update_joggers(Life& self, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Bool skip = false;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float k = 0.0;
    I32 lead = 0;
    U32 i = 0;
    Bool stop = false;
    Bool skip_2 = false;
    Float yaw = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    Bool skip_3 = false;
    Float sp = 0.0;
    Bool skip_4 = false;
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Vector l = Vector{0.0, 0.0, 0.0};
    Float d_3 = 0.0;
    Float vol = 0.0;
    I32 k_2 = 0;
    Int was = 0;
    U32 alive = 0;
    if (logic_not(view.people_on)) {
      self.chain_alive_ = false;
      return;
    }
    if (self.remote_joggers_) {
      if (math_compare<">">(kern::formula([](Float now, Float remote_joggers_at_) -> Float { return (now - remote_joggers_at_); }, now, self.remote_joggers_at_), 1.5)) {
        self.chain_alive_ = false;
        return;
      }
      if (logic_not(self.chain_alive_)) {
        return;
      }
      for (auto it104 = flow_for(n104, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(self.chain_.runners))))); it104.next();) {
        skip = false;
        if (logic_not(self.chain_.runners[n104.index].alive)) {
          skip = true;
        }
        if (logic_not(skip)) {
          d = vector_sub(vector_add(self.chain_.runners[n104.index].net_to, vector_scale(self.chain_.runners[n104.index].net_v, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.chain_.runners[n104.index].net_at), 0.0, 0.6)))), self.chain_.runners[n104.index].pos);
          k = kern::formula([](Float dt) -> Float { return kern::math::min(1.0, (dt * 10.0)); }, dt);
          self.chain_.runners[n104.index].pos = vector_add(self.chain_.runners[n104.index].pos, vector_scale(d, k));
          self.chain_.runners[n104.index].pos = vector_make(vector_break(self.chain_.runners[n104.index].pos).x, vector_break(self.chain_.runners[n104.index].pos).y, city_stream_cpp::CityStream_ground_height(city, vector_break(self.chain_.runners[n104.index].pos).x, vector_break(self.chain_.runners[n104.index].pos).y).Result);
          self.chain_.runners[n104.index].yaw_s = kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.chain_.runners[n104.index].yaw_s, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, common::wrap_pi(kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.chain_.runners[n104.index].yaw, self.chain_.runners[n104.index].yaw_s)).Result, kern::formula([](Float dt) -> Float { return ((-9.0) * dt); }, dt), kern::formula([](Float dt) -> Float { return (9.0 * dt); }, dt)));
          self.chain_.runners[n104.index].phase = kern::formula([](Float a, Float kSpeed, Float dt) -> Float { return (a + ((kSpeed * dt) * 2.4)); }, self.chain_.runners[n104.index].phase, kSpeed, dt);
        }
      }
      return;
    }
    if (logic_not(self.chain_alive_)) {
      if (math_compare<"<">(self.next_chain_, 0.0)) {
        self.next_chain_ = kern::formula([](Float now, Float kFirst) -> Float { return (now + kFirst); }, now, kFirst);
      }
      if (math_compare<">=">(now, self.next_chain_)) {
        n105 = common::Rng_range(rng, kEveryMin, kEveryMax);
        self.next_chain_ = kern::formula([](Float now, Float the_result) -> Float { return (now + the_result); }, now, n105.Result);
        n119 = Life_spawn_joggers(self, view, city, rng, now);
        if (logic_not(n119.Result)) {
          self.next_chain_ = kern::formula([](Float now) -> Float { return (now + 15.0); }, now);
        }
      }
      return;
    }
    lead = -1;
    i = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(i), kern::to<Float>(kRunners))))) {
      if (self.chain_.runners[i].alive) {
        lead = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i)));
        stop = true;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    if (logic_or(math_compare<"<">(kern::to<Float>(lead), 0.0), logic_and(math_compare<">">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, self.chain_.born), kChainLife), math_compare<">">(dist2d(self.chain_.runners[lead].pos, view.player).Result, 70.0)))) {
      self.chain_alive_ = false;
      return;
    }
    self.chain_.s = kern::formula([](Float s, Float kSpeed, Float dt) -> Float { return (s + (kSpeed * dt)); }, self.chain_.s, kSpeed, dt);
    for (auto it120 = flow_for(n120, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it120.next();) {
      skip_2 = false;
      if (logic_not(self.chain_.runners[n120.index].alive)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        yaw = 0.0;
        n = Vector{0.0, 0.0, 0.0};
        n121 = Life_sample(self, yaw, n, kern::formula([](Float s, Float index, Float kGap) -> Float { return (s - (index * kGap)); }, self.chain_.s, kern::to<Float>(n120.index), kGap));
        q = n121.Result;
        self.chain_.runners[n120.index].pos = vector_make(vector_break(q).x, vector_break(q).y, city_stream_cpp::CityStream_ground_height(city, vector_break(q).x, vector_break(q).y).Result);
        self.chain_.runners[n120.index].yaw = yaw;
        self.chain_.runners[n120.index].yaw_s = kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.chain_.runners[n120.index].yaw_s, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, common::wrap_pi(kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.chain_.runners[n120.index].yaw, self.chain_.runners[n120.index].yaw_s)).Result, kern::formula([](Float dt) -> Float { return ((-9.0) * dt); }, dt), kern::formula([](Float dt) -> Float { return (9.0 * dt); }, dt)));
        self.chain_.runners[n120.index].phase = kern::formula([](Float a, Float kSpeed, Float dt) -> Float { return (a + ((kSpeed * dt) * 3.4)); }, self.chain_.runners[n120.index].phase, kSpeed, dt);
      }
    }
    for (auto it122 = flow_for(n122, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<LifeCar>(view.cars))))); it122.next();) {
      skip_3 = false;
      if (view.cars[n122.index].airborne) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        sp = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(view.cars[n122.index].velocity).x, vector_break(view.cars[n122.index].velocity).y);
        if (math_compare<"<">(sp, 2.5)) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          for (auto it123 = flow_for(n123, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it123.next();) {
            skip_4 = false;
            if (logic_not(self.chain_.runners[n123.index].alive)) {
              skip_4 = true;
            }
            if (logic_not(skip_4)) {
              d_2 = vector_sub(self.chain_.runners[n123.index].pos, view.cars[n122.index].position);
              if (math_compare<">">(kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(d_2).x, vector_break(d_2).y), kern::formula([](Float x) -> Float { return (9.0 + (x * x)); }, vector_break(view.cars[n122.index].half).x))) {
                skip_4 = true;
              }
              if (logic_not(skip_4)) {
                l = common::to_local(view.cars[n122.index].rotation, d_2).Result;
                if (logic_and(math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(l).x), kern::formula([](Float x) -> Float { return kern::math::max(2.15, (x + 0.3)); }, vector_break(view.cars[n122.index].half).x)), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(l).y), kern::formula([](Float y) -> Float { return kern::math::max(1.15, (y + 0.3)); }, vector_break(view.cars[n122.index].half).y)))) {
                  Life_kill_runner(self, out, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n123.index))), vector_make(kern::formula([](Float x) -> Float { return (x * 0.8); }, vector_break(view.cars[n122.index].velocity).x), kern::formula([](Float y) -> Float { return (y * 0.8); }, vector_break(view.cars[n122.index].velocity).y), kern::formula([](Float sp) -> Float { return (2.4 + (sp * 0.12)); }, sp)), now);
                }
              }
            }
          }
        }
      }
    }
    d_3 = vector_length(vector_sub(self.chain_.runners[lead].pos, view.eye));
    vol = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float d_3) -> Float { return (1.0 - kern::math::div(kern::math::div(d_3, 22.0), 3.0)); }, d_3), 0.0, 1.0));
    if (math_compare<">=">(now, self.chain_.next_beat)) {
      self.chain_.next_beat = kern::formula([](Float now) -> Float { return (now + 0.4); }, now);
      self.chain_.beat = kern::to<Int>(kern::formula([](Float beat) -> Float { return (beat + 1.0); }, kern::to<Float>(self.chain_.beat)));
      list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float beat) -> Float { return kern::math::mod(beat, 2.0); }, kern::to<Float>(self.chain_.beat))))), 0.0), LifeOut_Sound::stomp_accent, LifeOut_Sound::stomp), self.chain_.runners[lead].pos, kern::formula([](Float beat) -> Float { return ((kern::math::mod(beat, 2.0) == 0.0) ? 0.55 : 0.32); }, kern::to<Float>(self.chain_.beat)), 1.0));
    }
    if (math_compare<">=">(now, self.chain_.next_call)) {
      was = self.chain_.call;
      self.chain_.call = kern::to<Int>(kern::formula([](Float call) -> Float { return (call + 1.0); }, kern::to<Float>(self.chain_.call)));
      k_2 = kern::to<Int>(kern::formula([](Float was) -> Float { return kern::math::mod(was, 6.0); }, kern::to<Float>(was)));
      self.chain_.next_call = kern::formula([](Float now) -> Float { return (now + 6.4); }, now);
      if (math_compare<">">(vol, 0.06)) {
        list_add(out.speech, struct_make<LifeOut_Say>(list_get<Text>(struct_break<Text_6_row>(list_get<Text_6_row>(kCalls, k_2)).items, 0), 0.85, 1.08, vol, 0.0));
        alive = 0;
        for (auto it126 = flow_for(n126, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(self.chain_.runners))))); it126.next();) {
          alive += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(self.chain_.runners[n126.index].alive)));
        }
        if (math_compare<">">(kern::to<Float>(alive), 1.0)) {
          list_add(out.speech, struct_make<LifeOut_Say>(list_get<Text>(struct_break<Text_6_row>(list_get<Text_6_row>(kCalls, k_2)).items, 1), 1.15, 1.12, kern::formula([](Float vol) -> Float { return (vol * 0.95); }, vol), 1.7));
        }
      }
    }
  }

  struct update_voices_matchResult { Bool Result = false; };
  KERN_PURE update_voices_matchResult update_voices_match(const Life_Voice& v, Float now = 0.0) {
    return {math_compare<">">(now, v.until)};
  }

  void Life_update_voices(Life& self, const LifeView& view, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<Life_Voice>(self.voices_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_voices_match(self.voices_[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n127, self.voices_[at]);
          self.voices_[kept] = n127.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.voices_, kept, Life_Voice{Vector{0.0, 0.0, 0.0}, "", "", 0.0});
  }

  void Life_update(Life& self, const LifeView& view, const CityStream& city, const List<Vec4>& ground_changes, Rng& rng, LifeOut& out, Float dt = 0.0, Float now = 0.0) {
    if (logic_or(logic_not(object_valid<kern::physics::World>(self.world_)), math_compare<"<=">(dt, 0.0))) {
      return;
    }
    Life_update_cows(self, view, city, out, dt, now);
    Life_update_pigeons(self, view, city, rng, out, dt, now);
    Life_update_birds(self, view, city, rng, out, dt, now);
    Life_update_trees(self, view, city, ground_changes, now);
    Life_update_pets(self, view, city, out, dt, now);
    Life_update_joggers(self, view, city, rng, out, dt, now);
    Life_update_voices(self, view, out, dt, now);
    ragdoll_cpp::Ragdolls_update(self.ragdolls_, dt, now);
  }

  struct Life_spawn_herdResult { Bool Result = false; };
  Life_spawn_herdResult Life_spawn_herd(Life& self, Vector center = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, U32 n = 0, Float now = 0.0) {
    Life_Meadow m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    Float a = 0.0;
    I32 mi = 0;
    U32 i = 0;
    Life_Cow c = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
    Float a_2 = 0.0;
    Float r = 0.0;
    m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    m.key = mix(Int64{0xc0c0}, int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)))).Result;
    for (auto it128 = flow_for(n128, 0, 23); it128.next();) {
      a = kern::formula([](Float index, Float kPi) -> Float { return ((kern::math::div(index, 24.0) * 2.0) * kPi); }, kern::to<Float>(n128.index), kPi);
      list_resize(m.rings, 1, Vector_row{List<Vector>{}});
      list_add(m.rings[0].items, vector_make(kern::formula([](Float x, Float a, Float radius) -> Float { return (x + (kern::math::cos(a) * radius)); }, vector_break(center).x, a, radius), kern::formula([](Float y, Float a, Float radius) -> Float { return (y + (kern::math::sin(a) * radius)); }, vector_break(center).y, a, radius), 0.0));
    }
    m.lo = vector_sub(center, vector_make(radius, radius, 0.0));
    m.hi = vector_add(center, vector_make(radius, radius, 0.0));
    m.centre = center;
    m.area = Life_ring_area(m.rings[0].items).Result;
    mi = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Life_Meadow>(self.meadows_))));
    list_add(self.meadows_, m);
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), math_compare<">">(kern::formula([](Float a, Float kMaxCows) -> Float { return ((a < 0.0) != (kMaxCows < 0.0) ? (kMaxCows < 0.0) : (a < kMaxCows)); }, kern::to<Float>(list_length<Life_Cow>(self.cows_)), kern::to<Float>(kMaxCows)), 0.5)))) {
      c = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
      c.key = mix(m.key, int64_of_int(i)).Result;
      c.meadow = mi;
      a_2 = kern::formula([](Float i) -> Float { return (i * 2.4); }, kern::to<Float>(i));
      r = kern::formula([](Float i) -> Float { return (2.0 + (i * 1.5)); }, kern::to<Float>(i));
      c.home = vector_make(kern::formula([](Float x, Float a_2, Float r) -> Float { return (x + (kern::math::cos(a_2) * r)); }, vector_break(center).x, a_2, r), kern::formula([](Float y, Float a_2, Float r) -> Float { return (y + (kern::math::sin(a_2) * r)); }, vector_break(center).y, a_2, r), 0.0);
      c.color = rgb(list_get<U32>(kCoats, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 5.0); }, kern::to<Float>(i))))).Result;
      c.next_moo = kern::formula([](Float now) -> Float { return (now + 1.0); }, now);
      c.pos = vector_make(vector_break(c.home).x, vector_break(c.home).y, 0.0);
      list_add(self.cows_, c);
      i += 1;
    }
    self.herds_for_ = center;
    return {true};
  }

  struct Life_grab_animalResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  Life_grab_animalResult Life_grab_animal(Life& self, Vector at = Vector{0.0, 0.0, 0.0}, Float reach = 0.0) {
    Float best = 0.0;
    I32 cow = 0;
    I32 pet = 0;
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Float d = 0.0;
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector to_local = Vector{0.0, 0.0, 0.0};
    Float d_2 = 0.0;
    Bool skip_2 = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    physics_Pose pose_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose_3 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector chosen = Vector{0.0, 0.0, 0.0};
    Float d_3 = 0.0;
    Bool body_valid_5 = false;
    Bool body_valid_6 = false;
    Bool both_3 = false;
    Life_Species sp = Life_Species::dog;
    Float sc = 0.0;
    Vector half = Vector{0.0, 0.0, 0.0};
    Float mass = 0.0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    Bool body_valid_7 = false;
    Bool body_valid_8 = false;
    Bool both_4 = false;
    physics_BodySettings bs_2 = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body_2 = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss_2 = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape_2 = physics_Shape{Int64{0x0}};
    if (logic_not(object_valid<kern::physics::World>(self.world_))) {
      return {struct_make<physics_Body>(Int64{0x0})};
    }
    best = reach;
    cow = -1;
    pet = -1;
    for (auto it129 = flow_for(n129, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it129.next();) {
      skip = false;
      if (self.cows_[n129.index].dead) {
        skip = true;
      }
      if (logic_not(skip)) {
        both = logic_and(self.cows_[n129.index].carried, engine_physics_physics::Body_valid(self.cows_[n129.index].body).Result);
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n129.index].body);
          both = body_valid_2;
        }
        if (both) {
          pose = engine_world_pose(self.world_, self.cows_[n129.index].body);
          d = vector_length(vector_sub(struct_break<physics_Pose>(pose).position, at));
          if (math_compare<"<">(d, kern::formula([](Float best) -> Float { return (best + 0.6); }, best))) {
            best = d;
            cow = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n129.index)));
            pet = -1;
          }
          skip = true;
        }
        if (logic_not(skip)) {
          to_local = vector_sub(at, vector_add(self.cows_[n129.index].pos, Vector{0.0, 0.0, 1.1}));
          d_2 = kern::formula([](Float a) -> Float { return kern::math::max(0.0, (a - 0.75)); }, vector_length(to_local));
          if (math_compare<"<">(d_2, best)) {
            best = d_2;
            cow = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n129.index)));
            pet = -1;
          }
        }
      }
    }
    for (auto it130 = flow_for(n130, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it130.next();) {
      skip_2 = false;
      if (logic_or(logic_not(self.pets_[n130.index].shown), math_compare<">=">(self.pets_[n130.index].dead_at, 0.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        both_2 = logic_and(self.pets_[n130.index].carried, engine_physics_physics::Body_valid(self.pets_[n130.index].body).Result);
        if (both_2) {
          body_valid_4 = engine_world_body_valid(self.world_, self.pets_[n130.index].body);
          both_2 = body_valid_4;
        }
        if (both_2) {
          pose_3 = engine_world_pose(self.world_, self.pets_[n130.index].body);
          chosen = struct_break<physics_Pose>(pose_3).position;
        } else {
          chosen = vector_add(self.pets_[n130.index].pos, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return (0.3 * a); }, self.pets_[n130.index].scale)));
        }
        c = chosen;
        d_3 = kern::formula([](Float a, Float a_2) -> Float { return kern::math::max(0.0, (a - (0.25 * a_2))); }, vector_length(vector_sub(at, c)), self.pets_[n130.index].scale);
        if (math_compare<"<">(d_3, best)) {
          best = d_3;
          pet = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n130.index)));
          cow = -1;
        }
      }
    }
    if (math_compare<">=">(kern::to<Float>(pet), 0.0)) {
      both_3 = logic_and(self.pets_[pet].carried, engine_physics_physics::Body_valid(self.pets_[pet].body).Result);
      if (both_3) {
        body_valid_6 = engine_world_body_valid(self.world_, self.pets_[pet].body);
        both_3 = body_valid_6;
      }
      if (both_3) {
        self.pets_[pet].carried_still = 0.0;
        return {self.pets_[pet].body};
      }
      sp = Life_species(self, kern::to<Int>(kern::formula([](Float pet) -> Float { return kern::math::wrap32(pet); }, kern::to<Float>(pet))), self.epoch_).Result;
      sc = kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : (a_2 * 0.62)); }, kern::to<Float>(enum_equal<Life_Species>(sp, Life_Species::dog)), self.pets_[pet].scale);
      half = vector_scale(Vector{0.33, 0.13, 0.27}, sc);
      mass = kern::formula([](Float a, Float sc) -> Float { return (((((a != 0.0) ? 22.0 : 4.0) * sc) * sc) * sc); }, kern::to<Float>(enum_equal<Life_Species>(sp, Life_Species::dog)), sc);
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::dynamic;
      bs.position = vector_make(vector_break(self.pets_[pet].pos).x, vector_break(self.pets_[pet].pos).y, kern::formula([](Float z, Float z_2) -> Float { return (z + z_2); }, vector_break(self.pets_[pet].pos).z, vector_break(half).z));
      bs.rotation = common::yaw_quat(self.pets_[pet].yaw).Result;
      bs.linear_damping = 0.1;
      bs.angular_damping = 1.5;
      engine_world_create_body(n131, self.world_, bs);
      create_body = n131.result;
      self.pets_[pet].body = create_body;
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::box;
      ss.size = vector_scale(half, 2.0);
      ss.material.density = kern::formula([](Float mass, Float x, Float y, Float z) -> Float { return kern::math::div(mass, (((8.0 * x) * y) * z)); }, mass, vector_break(half).x, vector_break(half).y, vector_break(half).z);
      ss.material.friction = 0.7;
      ss.material.category_bits = Int64{0x1};
      engine_world_create_shape(n132, self.world_, self.pets_[pet].body, ss);
      create_shape = n132.result;
      self.pets_[pet].rot = bs.rotation;
      self.pets_[pet].carried = true;
      self.pets_[pet].carried_still = 0.0;
      return {self.pets_[pet].body};
    }
    if (math_compare<">=">(kern::to<Float>(cow), 0.0)) {
      both_4 = logic_and(self.cows_[cow].carried, engine_physics_physics::Body_valid(self.cows_[cow].body).Result);
      if (both_4) {
        body_valid_8 = engine_world_body_valid(self.world_, self.cows_[cow].body);
        both_4 = body_valid_8;
      }
      if (both_4) {
        self.cows_[cow].carried_still = 0.0;
        return {self.cows_[cow].body};
      }
      bs_2 = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs_2.type = physics_BodyType::dynamic;
      bs_2.position = vector_add(self.cows_[cow].pos, Vector{0.0, 0.0, 1.12});
      bs_2.rotation = common::yaw_quat(self.cows_[cow].yaw).Result;
      bs_2.linear_damping = 0.05;
      bs_2.angular_damping = 0.6;
      engine_world_create_body(n133, self.world_, bs_2);
      create_body_2 = n133.result;
      self.cows_[cow].body = create_body_2;
      ss_2 = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss_2.kind = physics_ShapeKind::box;
      ss_2.size = Vector{1.64, 0.68, 0.76};
      ss_2.material.density = kern::formula([](Float kCowMass) -> Float { return kern::math::div(kCowMass, ((1.64 * 0.68) * 0.76)); }, kCowMass);
      ss_2.material.friction = 0.7;
      ss_2.material.restitution = 0.05;
      ss_2.material.category_bits = Int64{0x1};
      engine_world_create_shape(n134, self.world_, self.cows_[cow].body, ss_2);
      create_shape_2 = n134.result;
      self.cows_[cow].rot = bs_2.rotation;
      self.cows_[cow].carried = true;
      self.cows_[cow].carried_still = 0.0;
      return {self.cows_[cow].body};
    }
    return {struct_make<physics_Body>(Int64{0x0})};
  }

  struct Life_catch_pigeonResult { I32 Result = 0; };
  Life_catch_pigeonResult Life_catch_pigeon(Life& self, Rng& rng, Vector hand = Vector{0.0, 0.0, 0.0}, Float hand_speed = 0.0, Float now = 0.0) {
    Bool skip = false;
    Int i = 0;
    Bool skip_2 = false;
    Bool either = false;
    Life_LooseBird lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
    for (auto it135 = flow_for(n135, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it135.next();) {
      skip = false;
      if (self.flocks_[n135.index].air) {
        skip = true;
      }
      if (logic_not(skip)) {
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n135.index].birds))), 0.5))) {
          skip_2 = false;
          if (logic_or(math_compare<">=">(self.flocks_[n135.index].birds[i].dead_at, 0.0), math_compare<">">(vector_length(vector_sub(hand, vector_add(self.flocks_[n135.index].birds[i].pos, Vector{0.0, 0.0, 0.15}))), 0.24))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            either = math_compare<">">(hand_speed, 1.0);
            if (logic_not(either)) {
              n137 = common::Rng_unit(rng);
              either = math_compare<"<">(n137.Result, 0.5);
            }
            if (either) {
              self.flocks_[n135.index].next_flight = now;
              return {-1};
            }
            lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
            self.loose_serial_ = kern::to<Int>(kern::formula([](Float loose_serial_) -> Float { return (loose_serial_ + 1.0); }, kern::to<Float>(self.loose_serial_)));
            lb.id = self.loose_serial_;
            lb.pos = vector_add(self.flocks_[n135.index].birds[i].pos, Vector{0.0, 0.0, 0.15});
            lb.yaw = self.flocks_[n135.index].birds[i].yaw;
            list_add(self.loose_, lb);
            list_remove_at(self.flocks_[n135.index].birds, i);
            self.flocks_[n135.index].next_flight = kern::formula([](Float now) -> Float { return (now + 0.4); }, now);
            return {lb.id};
          }
          i += 1;
        }
      }
    }
    return {-1};
  }

  void Life_hold_pigeon(Life& self, I32 id = 0, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0) {
    for (auto it138 = flow_for(n138, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it138.next();) {
      if (math_compare<"==">(kern::to<Float>(self.loose_[n138.index].id), kern::to<Float>(id))) {
        self.loose_[n138.index].pos = at;
        self.loose_[n138.index].yaw = yaw;
        self.loose_[n138.index].held = true;
        self.loose_[n138.index].flap = kern::formula([](Float a) -> Float { return (a + 0.9); }, self.loose_[n138.index].flap);
      }
    }
  }

  void Life_release_pigeon(Life& self, I32 id = 0, Vector velocity = Vector{0.0, 0.0, 0.0}) {
    for (auto it139 = flow_for(n139, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it139.next();) {
      if (math_compare<"==">(kern::to<Float>(self.loose_[n139.index].id), kern::to<Float>(id))) {
        self.loose_[n139.index].held = false;
        self.loose_[n139.index].t = 0.0;
        self.loose_[n139.index].vel = vector_add(velocity, Vector{0.0, 0.0, 2.5});
      }
    }
  }

  void Life_spook_pigeons(Life& self, Float now = 0.0) {
    for (auto it140 = flow_for(n140, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it140.next();) {
      self.flocks_[n140.index].next_flight = now;
    }
  }

  struct Life_provoke_catResult { Bool Result = false; };
  Life_provoke_catResult Life_provoke_cat(Life& self, Float now = 0.0) {
    if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(list_length<Life_Pet>(self.pets_)), 2.0), logic_not(self.pets_[1].shown)), self.cat_.on)) {
      return {false};
    }
    self.cat_ = struct_make<Life_CatAttack>(false, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -9.0, 0.0, 0.0, 0);
    self.cat_.on = true;
    self.cat_.p = self.pets_[1].pos;
    self.cat_.yaw = self.pets_[1].yaw;
    self.cat_.start = now;
    return {true};
  }

  void Life_apply_joggers(Life& self, const Fixed<Vector, 10>& pos, const Fixed<Float, 10>& yaw, const Fixed<Bool, 10>& alive, U32 id = 0, U32 n = 0, Float now = 0.0, Float at = 0.0) {
    U32 i = 0;
    U32 i_2 = 0;
    Bool skip = false;
    Ragdolls_Colors col = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false};
    Fixed<U32, 5> skins = Fixed<U32, 5>{};
    Vector v = Vector{0.0, 0.0, 0.0};
    if (logic_or(logic_not(self.chain_alive_), math_compare<"!=">(kern::to<Float>(self.chain_.id), kern::to<Float>(id)))) {
      self.chain_ = struct_make<Life_Chain>(0, List<Vector>{}, List<Float>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0, 0, 1, list_make<Life_Runner, "10">(Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, Life_Runner{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}));
      self.chain_.id = id;
      self.chain_.born = now;
      for (auto it141 = flow_for(n141, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it141.next();) {
        self.chain_.runners[n141.index].alive = false;
      }
      i = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), math_compare<"<">(kern::to<Float>(i), kern::to<Float>(kRunners))))) {
        self.chain_.runners[i].net_to = pos[i];
        self.chain_.runners[i].pos = self.chain_.runners[i].net_to;
        self.chain_.runners[i].yaw_s = yaw[i];
        self.chain_.runners[i].yaw = self.chain_.runners[i].yaw_s;
        self.chain_.runners[i].alive = alive[i];
        i += 1;
      }
      self.chain_alive_ = true;
    }
    i_2 = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(n)), math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(kRunners))))) {
      skip = false;
      if (logic_not(alive[i_2])) {
        if (self.chain_.runners[i_2].alive) {
          col = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false};
          col.shirt = rgb(14221114).Result;
          col.pants = rgb(1845818).Result;
          skins = list_make<U32, "5">(14725260, 12881516, 11038802, 8079923, 13804158);
          col.skin = rgb(skins[kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 5.0); }, kern::to<Float>(i_2)))]).Result;
          n142 = ragdoll_cpp::Ragdolls_spawn(self.ragdolls_, col, int64_add(int64_of_int(kern::to<Int>(kern::formula([](Float id) -> Float { return ((268435456.0 + id) * 16.0); }, kern::to<Float>(self.chain_.id)))), int64_of_int(i_2)), self.chain_.runners[i_2].pos, self.chain_.runners[i_2].yaw_s, vector_scale(vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.chain_.runners[i_2].yaw_s), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.chain_.runners[i_2].yaw_s), 0.4), 2.5), false, now);
        }
        self.chain_.runners[i_2].alive = false;
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_not(self.chain_.runners[i_2].alive)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(math_compare<">">(at, kern::formula([](Float a) -> Float { return (a + 0.02); }, self.chain_.runners[i_2].net_at)), math_compare<">">(self.chain_.runners[i_2].net_at, 0.0))) {
            v = vector_scale(vector_sub(pos[i_2], self.chain_.runners[i_2].net_to), kern::formula([](Float at, Float a) -> Float { return kern::math::div(1.0, (at - a)); }, at, self.chain_.runners[i_2].net_at));
            if (math_compare<"<">(vector_length(v), 12.0)) {
              self.chain_.runners[i_2].net_v = v;
            }
          }
          self.chain_.runners[i_2].net_to = pos[i_2];
          self.chain_.runners[i_2].net_at = at;
          self.chain_.runners[i_2].yaw = yaw[i_2];
        }
      }
      i_2 += 1;
    }
    self.remote_joggers_at_ = now;
  }

  void Life_set_remote_animals(Life& self, Bool on = false) {
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(on))), kern::to<Float>(value_int(kern::to<Int>(self.remote_animals_))))) {
      return;
    }
    self.remote_animals_ = on;
    for (auto it143 = flow_for(n143, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it143.next();) {
      both = engine_physics_physics::Body_valid(self.cows_[n143.index].body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n143.index].body);
        both = body_valid_2;
      }
      if (both) {
        engine_world_destroy_body(self.world_, self.cows_[n143.index].body);
      }
    }
    for (auto it144 = flow_for(n144, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it144.next();) {
      both_2 = engine_physics_physics::Body_valid(self.pets_[n144.index].body).Result;
      if (both_2) {
        body_valid_4 = engine_world_body_valid(self.world_, self.pets_[n144.index].body);
        both_2 = body_valid_4;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.pets_[n144.index].body);
      }
    }
    list_clear(self.cows_);
    list_clear(self.meadows_);
    list_clear(self.flocks_);
    for (auto it145 = flow_for(n145, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it145.next();) {
      self.pets_[n145.index].body = struct_make<physics_Body>(Int64{0x0});
      self.pets_[n145.index].shown = false;
      self.pets_[n145.index].has_pose = false;
      self.pets_[n145.index].carried = false;
      self.pets_[n145.index].dead_at = -1.0;
    }
    self.herds_for_ = Vector{1000000000.0, 1000000000.0, 0.0};
    self.pet_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0};
  }

  struct apply_cows_matchResult { Bool Result = false; };
  KERN_PURE apply_cows_matchResult apply_cows_match(const Life_NetCow& cows_n146_index, const Life_Cow& x) {
    return {int64_equal(x.key, cows_n146_index.key)};
  }

  void Life_apply_cows(Life& self, const List<Life_NetCow>& cows, Float now = 0.0, Float at = 0.0) {
    List<Life_Cow> next = List<Life_Cow>{};
    Int old = 0;
    Int at_2 = 0;
    Int end = 0;
    Bool go = false;
    Life_Cow c = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
    Bool mine = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    if (logic_not(self.remote_animals_)) {
      return;
    }
    self.remote_animals_at_ = now;
    next = List<Life_Cow>{};
    for (auto it146 = flow_for(n146, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetCow>(cows))))); it146.next();) {
      at_2 = 0;
      end = list_length<Life_Cow>(self.cows_);
      go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
      while (loop(go)) {
        if (apply_cows_match(cows[n146.index], self.cows_[at_2]).Result) {
          go = false;
        } else {
          at_2 += 1;
          go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
        }
      }
      old = at_2;
      c = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
      if (logic_not(math_compare<"==">(kern::to<Float>(old), kern::to<Float>(list_length<Life_Cow>(self.cows_))))) {
        c = self.cows_[old];
        self.cows_[old].body = struct_make<physics_Body>(Int64{0x0});
      } else {
        c.key = cows[n146.index].key;
        c.color = rgb(list_get<U32>(kCoats, kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::mod(kern::math::trunc((the_result * 5.0)), 5.0); }, hash01(mix(cows[n146.index].key, Int64{0x7}).Result).Result)))).Result;
        c.phase = kern::formula([](Float the_result) -> Float { return (the_result * 6.0); }, hash01(cows[n146.index].key).Result);
        c.pos = cows[n146.index].pos;
        c.yaw = cows[n146.index].yaw;
        c.next_moo = kern::formula([](Float now, Float the_result) -> Float { return ((now + 8.0) + (the_result * 30.0)); }, now, hash01(mix(cows[n146.index].key, Int64{0x9}).Result).Result);
      }
      both = engine_physics_physics::Body_valid(c.body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, c.body);
        both = body_valid_2;
      }
      mine = both;
      if (logic_not(mine)) {
        if (logic_and(math_compare<">">(at, kern::formula([](Float net_at) -> Float { return (net_at + 0.02); }, c.net_at)), math_compare<">">(c.net_at, 0.0))) {
          v = vector_scale(vector_sub(cows[n146.index].pos, c.net_to), kern::formula([](Float at, Float net_at) -> Float { return kern::math::div(1.0, (at - net_at)); }, at, c.net_at));
          c.net_v = logic_select<Vector>(math_compare<"<">(vector_length(v), 15.0), v, Vector{0.0, 0.0, 0.0});
        }
        c.net_at = at;
        c.net_to = cows[n146.index].pos;
        c.net_rot = cows[n146.index].rot;
        c.net_dead = cows[n146.index].dead;
        c.walking = cows[n146.index].walking;
        c.carried = cows[n146.index].carried;
        c.yaw = kern::formula([](Float yaw, Float the_result) -> Float { return (yaw + the_result); }, c.yaw, common::wrap_pi(kern::formula([](Float a, Float yaw) -> Float { return (a - yaw); }, cows[n146.index].yaw, c.yaw)).Result);
      }
      list_add(next, c);
    }
    for (auto it147 = flow_for(n147, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it147.next();) {
      both_2 = engine_physics_physics::Body_valid(self.cows_[n147.index].body).Result;
      if (both_2) {
        body_valid_4 = engine_world_body_valid(self.world_, self.cows_[n147.index].body);
        both_2 = body_valid_4;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.cows_[n147.index].body);
      }
    }
    self.cows_ = next;
  }

  void Life_apply_pets(Life& self, const List<Life_NetPet>& pets, const List<Vector>& pet_anchors, U32 pet_count = 0, Float now = 0.0) {
    Int had = 0;
    List<Bool> seen = List<Bool>{};
    List<Bool> made = List<Bool>{};
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    Bool body_valid_5 = false;
    Bool body_valid_6 = false;
    Bool both_3 = false;
    Bool both_4 = false;
    if (logic_not(self.remote_animals_)) {
      return;
    }
    self.remote_animals_at_ = now;
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector>(pet_anchors)), 0.0))) {
      self.pet_anchor_ = pet_anchors[0];
      self.pet_anchors_ = pet_anchors;
      self.pet_base_ = kern::to<Int>(kern::formula([](Float pet_count, Float a) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::div(pet_count, kern::math::wrap32(a)))); }, kern::to<Float>(pet_count), kern::to<Float>(list_length<Vector>(pet_anchors))));
    }
    if (math_compare<"!=">(kern::to<Float>(list_length<Life_Pet>(self.pets_)), kern::to<Float>(pet_count))) {
      had = list_length<Life_Pet>(self.pets_);
      list_resize(self.pets_, pet_count, Life_Pet{20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, physics_Body{Int64{0x0}}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}});
      for (auto it148 = flow_for(n148, had, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it148.next();) {
        Life_pet_params(self.pets_[n148.index], kern::to<Int>(kern::formula([](Float index, Float pet_base_) -> Float { return kern::math::mod(kern::math::wrap32(index), pet_base_); }, kern::to<Float>(n148.index), kern::to<Float>(self.pet_base_))));
      }
    }
    list_resize(self.pet_net_to_, pet_count, Vector{0.0, 0.0, 0.0});
    list_resize(self.pet_net_yaw_, pet_count, 0.0);
    made = List<Bool>{};
    list_resize(made, pet_count, false);
    seen = made;
    for (auto it149 = flow_for(n149, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetPet>(pets))))); it149.next();) {
      skip = false;
      if (math_compare<">=">(kern::to<Float>(pets[n149.index].idx), kern::to<Float>(pet_count))) {
        skip = true;
      }
      if (logic_not(skip)) {
        seen[pets[n149.index].idx] = true;
        both = engine_physics_physics::Body_valid(self.pets_[pets[n149.index].idx].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.pets_[pets[n149.index].idx].body);
          both = body_valid_2;
        }
        if (both) {
          skip = true;
        }
        if (logic_not(skip)) {
          self.pet_net_to_[pets[n149.index].idx] = pets[n149.index].pos;
          self.pet_net_yaw_[pets[n149.index].idx] = pets[n149.index].yaw;
          self.pets_[pets[n149.index].idx].off = pets[n149.index].off;
          self.pets_[pets[n149.index].idx].net_rot = pets[n149.index].rot;
          self.pets_[pets[n149.index].idx].shown = pets[n149.index].shown;
          self.pets_[pets[n149.index].idx].carried = pets[n149.index].carried;
          if (logic_and(pets[n149.index].dead, math_compare<"<">(self.pets_[pets[n149.index].idx].dead_at, 0.0))) {
            self.pets_[pets[n149.index].idx].dead_at = now;
          }
          if (logic_not(pets[n149.index].dead)) {
            self.pets_[pets[n149.index].idx].dead_at = -1.0;
          }
        }
      }
    }
    for (auto it150 = flow_for(n150, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(pet_count)))); it150.next();) {
      both_4 = logic_not(seen[n150.index]);
      if (both_4) {
        both_3 = engine_physics_physics::Body_valid(self.pets_[n150.index].body).Result;
        if (both_3) {
          body_valid_6 = engine_world_body_valid(self.world_, self.pets_[n150.index].body);
          both_3 = body_valid_6;
        }
        both_4 = logic_not(both_3);
      }
      if (both_4) {
        self.pets_[n150.index].shown = false;
        self.pets_[n150.index].has_pose = false;
      }
    }
  }

  struct apply_flocks_matchResult { Bool Result = false; };
  KERN_PURE apply_flocks_matchResult apply_flocks_match(const Life_NetFlock& flocks_n151_index, const Life_Flock& x) {
    return {math_compare<"==">(kern::to<Float>(x.id), kern::to<Float>(flocks_n151_index.id))};
  }

  void Life_apply_flocks(Life& self, const List<Life_NetFlock>& flocks, Float now = 0.0, Float at = 0.0) {
    List<Life_Flock> next = List<Life_Flock>{};
    Int old = 0;
    Int at_2 = 0;
    Int end = 0;
    Bool go = false;
    Life_Flock f = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}};
    Bool skip = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    if (logic_not(self.remote_animals_)) {
      return;
    }
    self.remote_animals_at_ = now;
    next = List<Life_Flock>{};
    list_reserve(next, list_length<Life_NetFlock>(flocks));
    for (auto it151 = flow_for(n151, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetFlock>(flocks))))); it151.next();) {
      at_2 = 0;
      end = list_length<Life_Flock>(self.flocks_);
      go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
      while (loop(go)) {
        if (apply_flocks_match(flocks[n151.index], self.flocks_[at_2]).Result) {
          go = false;
        } else {
          at_2 += 1;
          go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
        }
      }
      old = at_2;
      f = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}};
      if (logic_not(math_compare<"==">(kern::to<Float>(old), kern::to<Float>(list_length<Life_Flock>(self.flocks_))))) {
        var_take(n152, self.flocks_[old]);
        self.flocks_[old].id = struct_break<Life_Flock>(n152.value).id;
        self.flocks_[old].air = struct_break<Life_Flock>(n152.value).air;
        self.flocks_[old].at = struct_break<Life_Flock>(n152.value).at;
        self.flocks_[old].next_flight = struct_break<Life_Flock>(n152.value).next_flight;
        self.flocks_[old].t = struct_break<Life_Flock>(n152.value).t;
        self.flocks_[old].coo_at = struct_break<Life_Flock>(n152.value).coo_at;
        self.flocks_[old].c = struct_break<Life_Flock>(n152.value).c;
        self.flocks_[old].from = struct_break<Life_Flock>(n152.value).from;
        self.flocks_[old].dest = struct_break<Life_Flock>(n152.value).dest;
        self.flocks_[old].r = struct_break<Life_Flock>(n152.value).r;
        self.flocks_[old].h = struct_break<Life_Flock>(n152.value).h;
        self.flocks_[old].dir = struct_break<Life_Flock>(n152.value).dir;
        self.flocks_[old].a0 = struct_break<Life_Flock>(n152.value).a0;
        self.flocks_[old].dur = struct_break<Life_Flock>(n152.value).dur;
        f = n152.value;
      }
      f.id = flocks[n151.index].id;
      f.air = flocks[n151.index].air;
      f.at = flocks[n151.index].at;
      if (math_compare<"!=">(kern::to<Float>(list_length<Life_Pigeon>(f.birds)), kern::to<Float>(list_length<Vector>(flocks[n151.index].birds)))) {
        list_resize(f.birds, list_length<Vector>(flocks[n151.index].birds), Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0});
        for (auto it153 = flow_for(n153, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(flocks[n151.index].birds))))); it153.next();) {
          f.birds[n153.index].pos = flocks[n151.index].birds[n153.index];
          f.birds[n153.index].yaw = flocks[n151.index].yaws[n153.index];
          f.birds[n153.index].dead_at = -1.0;
          f.birds[n153.index].peck = kern::formula([](Float index) -> Float { return ((index < 0.0 ? index + 18446744073709551616.0 : index) * 1.7); }, kern::to<Float>(n153.index));
          f.birds[n153.index].net_at = 0.0;
        }
      }
      for (auto it154 = flow_for(n154, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(flocks[n151.index].birds))))); it154.next();) {
        skip = false;
        if (logic_and(math_compare<"!=">(kern::to<Float>(flocks[n151.index].dead[n154.index]), 0.0), math_compare<"<">(f.birds[n154.index].dead_at, 0.0))) {
          f.birds[n154.index].dead_at = now;
          f.birds[n154.index].fall_v = Vector{0.0, 0.0, 0.0};
        }
        if (logic_not(math_compare<"!=">(kern::to<Float>(flocks[n151.index].dead[n154.index]), 0.0))) {
          f.birds[n154.index].dead_at = -1.0;
        }
        if (math_compare<"!=">(kern::to<Float>(flocks[n151.index].dead[n154.index]), 0.0)) {
          if (math_compare<">">(vector_length(vector_sub(f.birds[n154.index].pos, flocks[n151.index].birds[n154.index])), 1.5)) {
            f.birds[n154.index].pos = flocks[n151.index].birds[n154.index];
          }
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(math_compare<">">(at, kern::formula([](Float a) -> Float { return (a + 0.02); }, f.birds[n154.index].net_at)), math_compare<">">(f.birds[n154.index].net_at, 0.0))) {
            v = vector_scale(vector_sub(flocks[n151.index].birds[n154.index], f.birds[n154.index].net_to), kern::formula([](Float at, Float a) -> Float { return kern::math::div(1.0, (at - a)); }, at, f.birds[n154.index].net_at));
            f.birds[n154.index].net_v = logic_select<Vector>(math_compare<"<">(vector_length(v), 25.0), v, Vector{0.0, 0.0, 0.0});
          }
          f.birds[n154.index].net_to = flocks[n151.index].birds[n154.index];
          f.birds[n154.index].net_at = at;
          f.birds[n154.index].net_yaw = flocks[n151.index].yaws[n154.index];
        }
      }
      list_add(next, f);
    }
    self.flocks_ = next;
  }

  struct Life_net_carry_cowResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  Life_net_carry_cowResult Life_net_carry_cow(Life& self, Int64 key = Int64{0x0}) {
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    for (auto it155 = flow_for(n155, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it155.next();) {
      skip = false;
      if (logic_or(logic_not(int64_equal(self.cows_[n155.index].key, key)), self.cows_[n155.index].dead)) {
        skip = true;
      }
      if (logic_not(skip)) {
        both = engine_physics_physics::Body_valid(self.cows_[n155.index].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n155.index].body);
          both = body_valid_2;
        }
        if (both) {
          return {self.cows_[n155.index].body};
        }
        n156 = Life_grab_animal(self, vector_add(self.cows_[n155.index].pos, Vector{0.0, 0.0, 1.1}), 0.05);
        return {n156.Result};
      }
    }
    return {struct_make<physics_Body>(Int64{0x0})};
  }

  struct Life_net_carry_petResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  Life_net_carry_petResult Life_net_carry_pet(Life& self, U32 index = 0) {
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    if (math_compare<">">(kern::formula([](Float index, Float a) -> Float { return (!((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a))); }, kern::to<Float>(index), kern::to<Float>(list_length<Life_Pet>(self.pets_))), 0.5)) {
      return {struct_make<physics_Body>(Int64{0x0})};
    }
    if (math_compare<">=">(self.pets_[index].dead_at, 0.0)) {
      return {struct_make<physics_Body>(Int64{0x0})};
    }
    both = engine_physics_physics::Body_valid(self.pets_[index].body).Result;
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, self.pets_[index].body);
      both = body_valid_2;
    }
    if (both) {
      return {self.pets_[index].body};
    }
    if (logic_not(self.pets_[index].shown)) {
      self.pets_[index].shown = true;
    }
    n157 = Life_grab_animal(self, vector_add(self.pets_[index].pos, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return (0.3 * a); }, self.pets_[index].scale))), 0.05);
    return {n157.Result};
  }

  void Life_net_bird(Life& self, Int64 key = Int64{0x0}, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector vel = Vector{0.0, 0.0, 0.0}, Bool held = false) {
    Life_LooseBird lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
    for (auto it158 = flow_for(n158, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it158.next();) {
      if (int64_equal(self.loose_[n158.index].net, key)) {
        if (held) {
          self.loose_[n158.index].pos = at;
          self.loose_[n158.index].yaw = yaw;
          self.loose_[n158.index].held = true;
          self.loose_[n158.index].flap = kern::formula([](Float a) -> Float { return (a + 0.9); }, self.loose_[n158.index].flap);
        } else if (self.loose_[n158.index].held) {
          self.loose_[n158.index].held = false;
          self.loose_[n158.index].t = 0.0;
          self.loose_[n158.index].pos = at;
          self.loose_[n158.index].vel = vector_add(vel, Vector{0.0, 0.0, 2.5});
        }
        return;
      }
    }
    lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
    self.loose_serial_ = kern::to<Int>(kern::formula([](Float loose_serial_) -> Float { return (loose_serial_ + 1.0); }, kern::to<Float>(self.loose_serial_)));
    lb.id = kern::to<Int>(kern::formula([](Float loose_serial_) -> Float { return (-loose_serial_); }, kern::to<Float>(self.loose_serial_)));
    lb.net = key;
    lb.pos = at;
    lb.yaw = yaw;
    lb.held = held;
    if (logic_not(held)) {
      lb.vel = vector_add(vel, Vector{0.0, 0.0, 2.5});
    }
    list_add(self.loose_, lb);
  }

  struct Life_shotResult { Bool Result = false; };
  Life_shotResult Life_shot(Life& self, Vector& to, LifeOut& out, Vector from = Vector{0.0, 0.0, 0.0}, Float damage = 0.0, Float now = 0.0, Bool melee = false) {
    Float damage_ = 0.0;
    Float best = 0.0;
    I32 kind = 0;
    I32 which = 0;
    Bool skip = false;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float t0 = 0.0;
    Float t1 = 0.0;
    Bool hit = false;
    I32 k = 0;
    Bool skip_2 = false;
    Float o = 0.0;
    Float dd = 0.0;
    Float hh = 0.0;
    Float e0 = 0.0;
    Float e1 = 0.0;
    Float swapped = 0.0;
    Bool skip_3 = false;
    Float t = 0.0;
    Int bi = 0;
    Bool skip_4 = false;
    Float t_2 = 0.0;
    Bool skip_5 = false;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Fixed<Float, 3> items = Fixed<Float, 3>{};
    Float z = 0.0;
    Float t_3 = 0.0;
    Bool skip_6 = false;
    Float sc = 0.0;
    Float t_4 = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float k_2 = 0.0;
    Float h_2 = 0.0;
    U32 i = 0;
    Life_Species sp = Life_Species::dog;
    Bool both = false;
    damage_ = damage;
    best = 2.0;
    kind = -1;
    which = -1;
    for (auto it159 = flow_for(n159, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it159.next();) {
      skip = false;
      if (self.cows_[n159.index].dead) {
        skip = true;
      }
      if (logic_not(skip)) {
        q = common::yaw_quat(self.cows_[n159.index].yaw).Result;
        a = common::to_local(q, vector_sub(from, vector_add(self.cows_[n159.index].pos, Vector{0.0, 0.0, 1.15}))).Result;
        b = common::to_local(q, vector_sub(to, vector_add(self.cows_[n159.index].pos, Vector{0.0, 0.0, 1.15}))).Result;
        h = Vector{0.95, 0.36, 0.42};
        d = vector_sub(b, a);
        t0 = 0.0;
        t1 = 1.0;
        hit = true;
        k = 0;
        while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), 3.0), hit))) {
          skip_2 = false;
          o = kern::formula([](Float k, Float x, Float y, Float z) -> Float { return ((k == 0.0) ? x : ((k == 1.0) ? y : z)); }, kern::to<Float>(k), vector_break(a).x, vector_break(a).y, vector_break(a).z);
          dd = kern::formula([](Float k, Float x, Float y, Float z) -> Float { return ((k == 0.0) ? x : ((k == 1.0) ? y : z)); }, kern::to<Float>(k), vector_break(d).x, vector_break(d).y, vector_break(d).z);
          hh = kern::formula([](Float k, Float x, Float y, Float z) -> Float { return ((k == 0.0) ? x : ((k == 1.0) ? y : z)); }, kern::to<Float>(k), vector_break(h).x, vector_break(h).y, vector_break(h).z);
          if (math_compare<"<">(kern::formula([](Float dd) -> Float { return kern::math::abs(dd); }, dd), 1e-07)) {
            if (math_compare<">">(kern::formula([](Float o) -> Float { return kern::math::abs(o); }, o), hh)) {
              hit = false;
            }
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            e0 = kern::formula([](Float hh, Float o, Float dd) -> Float { return kern::math::div(((-hh) - o), dd); }, hh, o, dd);
            e1 = kern::formula([](Float hh, Float o, Float dd) -> Float { return kern::math::div((hh - o), dd); }, hh, o, dd);
            if (math_compare<">">(e0, e1)) {
              swapped = e0;
              e0 = e1;
              e1 = swapped;
            }
            t0 = kern::formula([](Float t0, Float e0) -> Float { return kern::math::max(t0, e0); }, t0, e0);
            t1 = kern::formula([](Float t1, Float e1) -> Float { return kern::math::min(t1, e1); }, t1, e1);
            if (math_compare<">">(t0, t1)) {
              hit = false;
            }
          }
          k += 1;
        }
        if (logic_and(hit, math_compare<"<">(t0, best))) {
          best = t0;
          kind = 0;
          which = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n159.index)));
        }
      }
    }
    for (auto it160 = flow_for(n160, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Bird>(self.birds_))))); it160.next();) {
      skip_3 = false;
      if (logic_or(math_compare<"<">(now, self.birds_[n160.index].dead_until), math_compare<"<">(now, self.birds_[n160.index].dying_until))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        t = seg_sphere(from, to, self.birds_[n160.index].pos, 0.45).Result;
        if (logic_and(math_compare<">=">(t, 0.0), math_compare<"<">(t, best))) {
          best = t;
          kind = 1;
          which = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n160.index)));
        }
      }
    }
    for (auto it161 = flow_for(n161, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it161.next();) {
      bi = 0;
      while (loop(logic_and(math_compare<">">(kern::formula([](Float bi, Float a) -> Float { return ((bi < 0.0) != (a < 0.0) ? (a < 0.0) : (bi < a)); }, kern::to<Float>(bi), kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n161.index].birds))), 0.5), math_compare<">">(kern::formula([](Float bi) -> Float { return ((bi < 0.0) != (64.0 < 0.0) ? (64.0 < 0.0) : (bi < 64.0)); }, kern::to<Float>(bi)), 0.5)))) {
        skip_4 = false;
        if (math_compare<">=">(self.flocks_[n161.index].birds[bi].dead_at, 0.0)) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          t_2 = seg_sphere(from, to, vector_add(self.flocks_[n161.index].birds[bi].pos, Vector{0.0, 0.0, 0.15}), 0.2).Result;
          if (logic_and(math_compare<">=">(t_2, 0.0), math_compare<"<">(t_2, best))) {
            best = t_2;
            kind = 4;
            which = kern::to<Int>(kern::formula([](Float index, Float bi) -> Float { return kern::math::wrap32s(((index * 64.0) + bi)); }, kern::to<Float>(n161.index), kern::to<Float>(bi)));
          }
        }
        bi += 1;
      }
    }
    if (self.chain_alive_) {
      for (auto it162 = flow_for(n162, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it162.next();) {
        skip_5 = false;
        if (logic_not(self.chain_.runners[n162.index].alive)) {
          skip_5 = true;
        }
        if (logic_not(skip_5)) {
          item = 0.6;
          item_2 = 1.15;
          item_3 = 1.6;
          items = list_make<Float, "3">(item, item_2, item_3);
          for (auto it163 = flow_for(n163, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it163.next();) {
            z = items[n163.index];
            t_3 = seg_sphere(from, to, vector_add(self.chain_.runners[n162.index].pos, vector_make(0.0, 0.0, z)), 0.3).Result;
            if (logic_and(math_compare<">=">(t_3, 0.0), math_compare<"<">(t_3, best))) {
              best = t_3;
              kind = 2;
              which = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n162.index)));
            }
          }
        }
      }
    }
    for (auto it164 = flow_for(n164, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it164.next();) {
      skip_6 = false;
      if (logic_or(logic_not(self.pets_[n164.index].shown), math_compare<">=">(self.pets_[n164.index].dead_at, 0.0))) {
        skip_6 = true;
      }
      if (logic_not(skip_6)) {
        sc = kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : (a_2 * 0.62)); }, kern::to<Float>(enum_equal<Life_Species>(Life_species(self, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n164.index))), self.epoch_).Result, Life_Species::dog)), self.pets_[n164.index].scale);
        t_4 = seg_sphere(from, to, vector_add(self.pets_[n164.index].pos, vector_make(0.0, 0.0, kern::formula([](Float sc) -> Float { return (0.38 * sc); }, sc))), kern::formula([](Float sc) -> Float { return (0.34 * sc); }, sc)).Result;
        if (logic_and(math_compare<">=">(t_4, 0.0), math_compare<"<">(t_4, best))) {
          best = t_4;
          kind = 3;
          which = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n164.index)));
        }
      }
    }
    if (math_compare<"<">(kern::to<Float>(kind), 0.0)) {
      return {false};
    }
    to = vector_add(from, vector_scale(vector_sub(to, from), best));
    dir = vector_sub(to, from);
    l = kern::formula([](Float a) -> Float { return kern::math::max(0.0001, a); }, vector_length(dir));
    dir = vector_scale(dir, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l));
    if (math_compare<"==">(kern::to<Float>(kind), 0.0)) {
      if (math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.cows_[which].last_hit), 0.04)) {
        return {true};
      }
      self.cows_[which].last_hit = now;
      self.cow_hits_ = kern::to<Int>(kern::formula([](Float cow_hits_) -> Float { return (cow_hits_ + 1.0); }, kern::to<Float>(self.cow_hits_)));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::thud, to, 0.5, 1.0));
      if (melee) {
        damage_ = 6.0;
      }
      self.cows_[which].hp = kern::formula([](Float a, Float damage_) -> Float { return (a - damage_); }, self.cows_[which].hp, damage_);
      if (math_compare<"<=">(self.cows_[which].hp, 0.0)) {
        k_2 = kern::formula([](Float damage_) -> Float { return kern::math::min(4.0, (1.0 + kern::math::div(damage_, 120.0))); }, damage_);
        Life_kill_cow(self, self.cows_[which], out, vector_make(kern::formula([](Float x, Float kCowMass, Float k_2) -> Float { return ((x * kCowMass) * k_2); }, vector_break(dir).x, kCowMass, k_2), kern::formula([](Float y, Float kCowMass, Float k_2) -> Float { return ((y * kCowMass) * k_2); }, vector_break(dir).y, kCowMass, k_2), kern::formula([](Float kCowMass) -> Float { return (kCowMass * 1.2); }, kCowMass)), now);
      } else {
        self.cows_[which].off = vector_add(self.cows_[which].off, vector_scale(vector_make(vector_break(dir).x, vector_break(dir).y, 0.0), kern::formula([](Float melee) -> Float { return ((melee != 0.0) ? 0.35 : 0.12); }, kern::to<Float>(melee))));
        self.cows_[which].flinch_at = now;
        Life_scare_cow(self.cows_[which], vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 3.0)); }, vector_break(self.cows_[which].pos).x, vector_break(dir).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 3.0)); }, vector_break(self.cows_[which].pos).y, vector_break(dir).y), 0.0), now, 5.5);
        list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::moo_pain, vector_add(self.cows_[which].pos, Vector{0.0, 0.0, 1.4}), 0.9, 1.45));
      }
    } else if (math_compare<"==">(kern::to<Float>(kind), 1.0)) {
      self.bird_hits_ = kern::to<Int>(kern::formula([](Float bird_hits_) -> Float { return (bird_hits_ + 1.0); }, kern::to<Float>(self.bird_hits_)));
      self.birds_[which].dying_until = kern::formula([](Float now) -> Float { return (now + 1.1); }, now);
      self.birds_[which].dead_until = kern::formula([](Float now) -> Float { return (now + 14.0); }, now);
      self.birds_[which].down = false;
      self.birds_[which].fall_v = vector_make(kern::formula([](Float x) -> Float { return (x * 2.2); }, vector_break(dir).x), kern::formula([](Float y) -> Float { return (y * 2.2); }, vector_break(dir).y), -0.4);
      self.birds_[which].spin = kern::formula([](Float the_result) -> Float { return (4.0 + (the_result * 4.0)); }, hash01(mix(int64_of_int(self.birds_[which].seed), int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return (now * 100.0); }, now)))).Result).Result);
      h_2 = hash01(mix(int64_of_int(self.birds_[which].seed), int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)))).Result).Result;
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::squawk, self.birds_[which].pos, 1.0, kern::formula([](Float the_result, Float h_2) -> Float { return ((0.8 + (0.5 * the_result)) + (0.1 * h_2)); }, hash01(int64_of_int(self.birds_[which].seed)).Result, h_2)));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::squawk, self.birds_[which].pos, 0.7, kern::formula([](Float h_2) -> Float { return (1.3 + (0.3 * h_2)); }, h_2)));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::flap, self.birds_[which].pos, 0.9, 1.2));
      list_add(out.bursts, struct_make<LifeOut_Burst>(self.birds_[which].pos, Vector{0.8, 0.75, 0.66}));
    } else if (math_compare<"==">(kern::to<Float>(kind), 4.0)) {
      self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].dead_at = now;
      self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].fall_v = vector_make(kern::formula([](Float x) -> Float { return (x * 1.6); }, vector_break(dir).x), kern::formula([](Float y) -> Float { return (y * 1.6); }, vector_break(dir).y), kern::formula([](Float a) -> Float { return ((a != 0.0) ? (-0.5) : 1.2); }, kern::to<Float>(self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].air)));
      self.bird_hits_ = kern::to<Int>(kern::formula([](Float bird_hits_) -> Float { return (bird_hits_ + 1.0); }, kern::to<Float>(self.bird_hits_)));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::squawk, self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].pos, 0.9, kern::formula([](Float the_result) -> Float { return (1.05 + (0.35 * the_result)); }, hash01(int64_of_int(kern::to<Int>(kern::formula([](Float which, Float now) -> Float { return kern::math::wrap32(kern::math::mul32(kern::math::wrap32(which), 2654435761.0) + kern::math::trunc((now * 100.0))); }, kern::to<Float>(which), now)))).Result)));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::coo, self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].pos, 0.6, 1.5));
      list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::flap, self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].pos, 0.8, 1.0));
      list_add(out.bursts, struct_make<LifeOut_Burst>(vector_add(self.flocks_[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::trunc(kern::math::div(which, 64.0)); }, kern::to<Float>(which)))].birds[kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::mod(which, 64.0); }, kern::to<Float>(which)))].pos, Vector{0.0, 0.0, 0.15}), Vector{0.72, 0.74, 0.78}));
      Life_spook_pigeons(self, now);
    } else if (math_compare<"==">(kern::to<Float>(kind), 2.0)) {
      Life_kill_runner(self, out, which, vector_make(kern::formula([](Float x) -> Float { return (x * 2.5); }, vector_break(dir).x), kern::formula([](Float y) -> Float { return (y * 2.5); }, vector_break(dir).y), 1.4), now);
    } else {
      i = kern::to<Int>(kern::formula([](Float which) -> Float { return kern::math::wrap32(which); }, kern::to<Float>(which)));
      sp = Life_species(self, i, self.epoch_).Result;
      if (enum_equal<Life_Species>(sp, Life_Species::black_cat)) {
        both = logic_not(self.cat_.on);
        if (both) {
          n166 = Life_provoke_cat(self, now);
          both = n166.Result;
        }
        if (both) {
          list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::hiss, self.pets_[i].pos, 1.0, 1.0));
        }
      } else if (melee) {
        list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(enum_equal<Life_Species>(sp, Life_Species::cat), LifeOut_Sound::cat, LifeOut_Sound::dog), self.pets_[i].pos, 0.8, 1.0));
        self.pets_[i].punches = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(self.pets_[i].punches)));
        if (math_compare<">=">(kern::to<Float>(self.pets_[i].punches), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 2.0 : 3.0); }, kern::to<Float>(enum_equal<Life_Species>(sp, Life_Species::cat)))))))) {
          self.pets_[i].punches = 0;
          Life_kill_pet(self, out, i, vector_scale(dir, 3.0), now);
        }
      } else {
        Life_kill_pet(self, out, i, vector_scale(dir, 4.0), now);
      }
    }
    return {true};
  }

  void Life_remote_cow_death(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Vector impulse = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    I32 best = 0;
    Float bd = 0.0;
    best = -1;
    bd = 4.0;
    for (auto it167 = flow_for(n167, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it167.next();) {
      if (logic_and(logic_not(self.cows_[n167.index].dead), math_compare<"<">(dist2d(self.cows_[n167.index].pos, at).Result, bd))) {
        bd = dist2d(self.cows_[n167.index].pos, at).Result;
        best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n167.index)));
      }
    }
    if (math_compare<"<">(kern::to<Float>(best), 0.0)) {
      return;
    }
    Life_kill_cow(self, self.cows_[best], out, impulse, now);
    list_remove_at(out.cow_deaths, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<LifeOut_CowDeath>(out.cow_deaths)))));
  }

  void Life_heard_shot(Life& self, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Float len = 0.0;
    Vector mid = Vector{0.0, 0.0, 0.0};
    len = dist2d(from, to).Result;
    mid = vector_scale(vector_add(from, to), 0.5);
    for (auto it168 = flow_for(n168, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it168.next();) {
      if (logic_and(logic_not(self.cows_[n168.index].dead), math_compare<"<">(dist2d(self.cows_[n168.index].pos, mid).Result, kern::formula([](Float len) -> Float { return (kern::math::div(len, 2.0) + 12.0); }, len)))) {
        Life_scare_cow(self.cows_[n168.index], vector_make(vector_break(from).x, vector_break(from).y, 0.0), now, 3.0);
      }
    }
    for (auto it169 = flow_for(n169, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it169.next();) {
      if (logic_and(logic_not(self.flocks_[n169.index].air), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.flocks_[n169.index].at).x, vector_break(from).x, vector_break(self.flocks_[n169.index].at).y, vector_break(from).y), 40.0))) {
        self.flocks_[n169.index].next_flight = kern::formula([](Float a, Float now) -> Float { return kern::math::min(a, now); }, self.flocks_[n169.index].next_flight, now);
      }
    }
  }

  void Life_blast(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float now = 0.0) {
    Float r = 0.0;
    Bool skip = false;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float dist = 0.0;
    Float k = 0.0;
    Bool skip_2 = false;
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Float dist_2 = 0.0;
    Float R = 0.0;
    Bool skip_3 = false;
    Vector d_3 = Vector{0.0, 0.0, 0.0};
    Float dist_3 = 0.0;
    Float k_2 = 0.0;
    Bool skip_4 = false;
    Bool skip_5 = false;
    Vector d_4 = Vector{0.0, 0.0, 0.0};
    Float dist_4 = 0.0;
    r = kern::formula([](Float radius) -> Float { return kern::math::max(4.0, radius); }, radius);
    for (auto it170 = flow_for(n170, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it170.next();) {
      skip = false;
      if (self.cows_[n170.index].dead) {
        skip = true;
      }
      if (logic_not(skip)) {
        d = vector_sub(self.cows_[n170.index].pos, at);
        dist = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
        if (math_compare<"<">(dist, kern::formula([](Float r) -> Float { return (r * 0.75); }, r))) {
          k = kern::formula([](Float kCowMass, Float dist, Float r) -> Float { return kern::math::div((kCowMass * ((7.0 * (1.0 - kern::math::div(dist, r))) + 2.0)), kern::math::max(0.5, dist)); }, kCowMass, dist, r);
          Life_kill_cow(self, self.cows_[n170.index], out, vector_make(kern::formula([](Float x, Float k) -> Float { return (x * k); }, vector_break(d).x, k), kern::formula([](Float y, Float k) -> Float { return (y * k); }, vector_break(d).y, k), kern::formula([](Float kCowMass, Float dist, Float r) -> Float { return (((kCowMass * 5.0) * (1.0 - kern::math::div(dist, r))) + kCowMass); }, kCowMass, dist, r)), now);
        } else if (math_compare<"<">(dist, kern::formula([](Float r) -> Float { return (r * 4.0); }, r))) {
          Life_scare_cow(self.cows_[n170.index], vector_make(vector_break(at).x, vector_break(at).y, 0.0), now, 4.5);
        }
      }
    }
    for (auto it171 = flow_for(n171, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it171.next();) {
      if (logic_and(logic_not(self.flocks_[n171.index].air), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.flocks_[n171.index].at).x, vector_break(at).x, vector_break(self.flocks_[n171.index].at).y, vector_break(at).y), 90.0))) {
        self.flocks_[n171.index].next_flight = kern::formula([](Float a, Float now) -> Float { return kern::math::min(a, now); }, self.flocks_[n171.index].next_flight, now);
      }
    }
    for (auto it172 = flow_for(n172, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(self.trees_))))); it172.next();) {
      skip_2 = false;
      if (math_compare<">=">(self.trees_[n172.index].fell_at, 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        d_2 = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(self.trees_[n172.index].foot).x, vector_break(at).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(self.trees_[n172.index].foot).y, vector_break(at).y), 0.0);
        dist_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_2).x, vector_break(d_2).y);
        if (logic_or(math_compare<">">(dist_2, kern::formula([](Float r) -> Float { return (r * 0.9); }, r)), math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(self.trees_[n172.index].foot).z, vector_break(at).z), r))) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.trees_[n172.index].fell_at = now;
          self.trees_[n172.index].fall_dir = logic_select<Vector>(math_compare<">">(dist_2, 0.1), vector_scale(d_2, kern::formula([](Float dist_2) -> Float { return kern::math::div(1.0, dist_2); }, dist_2)), Vector{1.0, 0.0, 0.0});
          self.trees_down_ = kern::to<Int>(kern::formula([](Float trees_down_) -> Float { return (trees_down_ + 1.0); }, kern::to<Float>(self.trees_down_)));
          list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::thud, vector_add(self.trees_[n172.index].foot, Vector{0.0, 0.0, 1.0}), 0.8, 0.7));
          list_add(out.bursts, struct_make<LifeOut_Burst>(vector_add(self.trees_[n172.index].foot, Vector{0.0, 0.0, 2.6}), Vector{0.31, 0.52, 0.34}));
        }
      }
    }
    if (self.chain_alive_) {
      R = kern::formula([](Float radius) -> Float { return kern::math::max(3.0, radius); }, radius);
      for (auto it173 = flow_for(n173, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kRunners)))); it173.next();) {
        skip_3 = false;
        if (logic_not(self.chain_.runners[n173.index].alive)) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          d_3 = vector_sub(self.chain_.runners[n173.index].pos, at);
          dist_3 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_3).x, vector_break(d_3).y);
          if (math_compare<"<">(dist_3, kern::formula([](Float r) -> Float { return (r * 0.7); }, R))) {
            k_2 = kern::formula([](Float dist_3, Float r) -> Float { return kern::math::div(((1.0 - kern::math::div(dist_3, r)) * 7.0), kern::math::max(0.3, dist_3)); }, dist_3, R);
            Life_kill_runner(self, out, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n173.index))), vector_make(kern::formula([](Float x, Float k_2) -> Float { return (x * k_2); }, vector_break(d_3).x, k_2), kern::formula([](Float y, Float k_2) -> Float { return (y * k_2); }, vector_break(d_3).y, k_2), kern::formula([](Float dist_3, Float r) -> Float { return (4.0 + (3.0 * (1.0 - kern::math::div(dist_3, r)))); }, dist_3, R)), now);
          }
        }
      }
    }
    for (auto it174 = flow_for(n174, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(self.birds_))))); it174.next();) {
      skip_4 = false;
      if (logic_or(math_compare<"<">(now, self.birds_[n174.index].dead_until), math_compare<">">(vector_length(vector_sub(self.birds_[n174.index].pos, at)), r))) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        self.birds_[n174.index].dying_until = kern::formula([](Float now) -> Float { return (now + 1.1); }, now);
        self.birds_[n174.index].dead_until = kern::formula([](Float now) -> Float { return (now + 14.0); }, now);
        self.birds_[n174.index].down = false;
        self.birds_[n174.index].fall_v = vector_scale(vector_sub(self.birds_[n174.index].pos, at), 0.5);
        self.birds_[n174.index].spin = 6.0;
        list_add(out.sounds, struct_make<LifeOut_Play>(LifeOut_Sound::squawk, self.birds_[n174.index].pos, 0.9, kern::formula([](Float the_result) -> Float { return (0.9 + (0.4 * the_result)); }, hash01(int64_of_int(self.birds_[n174.index].seed)).Result)));
      }
    }
    for (auto it175 = flow_for(n175, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it175.next();) {
      skip_5 = false;
      if (logic_or(logic_or(logic_not(self.pets_[n175.index].shown), math_compare<">=">(self.pets_[n175.index].dead_at, 0.0)), logic_and(self.cat_.on, math_compare<"==">(kern::to<Float>(n175.index), 1.0)))) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        d_4 = vector_sub(self.pets_[n175.index].pos, at);
        dist_4 = vector_length(d_4);
        if (math_compare<"<">(dist_4, r)) {
          Life_kill_pet(self, out, n175.index, vector_add(vector_scale(logic_select<Vector>(math_compare<">">(dist_4, 0.1), vector_scale(d_4, kern::formula([](Float dist_4) -> Float { return kern::math::div(1.0, dist_4); }, dist_4)), Vector{0.0, 0.0, 1.0}), kern::formula([](Float dist_4, Float r) -> Float { return (8.0 * (1.0 - kern::math::div(dist_4, r))); }, dist_4, r)), Vector{0.0, 0.0, 3.0}), now);
        }
      }
    }
    ragdoll_cpp::Ragdolls_blast(self.ragdolls_, at, kern::formula([](Float r) -> Float { return (r * 1.2); }, r), 6.0);
  }

  struct Life_shoutResult { Bool Result = false; };
  Life_shoutResult Life_shout(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Text mood = "", U32 seed = 0, Float now = 0.0, Float d = 0.0) {
    Int pick = 0;
    Int i = 0;
    List<Lines> list = List<Lines>{};
    Lines lines = Lines{"", List<Text>{}, 0};
    Float r = 0.0;
    Text line = "";
    Float vol = 0.0;
    Float k = 0.0;
    if (logic_or(math_compare<"<">(kern::formula([](Float now, Float last_shout_) -> Float { return (now - last_shout_); }, now, self.last_shout_), kShoutGap), math_compare<">">(d, kMaxHear))) {
      return {false};
    }
    for (auto it176 = flow_for(n176, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.source_gap_))))); it176.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.source_gap_[n176.index].first), kern::to<Float>(seed)), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.source_gap_[n176.index].second), kSourceGap))) {
        return {false};
      }
    }
    pick = 0;
    i = 0;
    list = kLines;
    for (auto it177 = flow_for(n177, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lines>(list))))); it177.next();) {
      if (text_equal(list[n177.index].mood, mood)) {
        pick = i;
      }
      i += 1;
    }
    lines = list_get<Lines>(kLines, pick);
    r = hash01(mix(int64_of_int(seed), int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return kern::math::div(now, 0.997); }, now)))).Result).Result;
    line = lines.lines[kern::to<Int>(kern::formula([](Float r, Float n) -> Float { return kern::math::mod(kern::math::trunc((r * n)), n); }, r, kern::to<Float>(lines.n)))];
    self.last_shout_ = now;
    list_add(self.source_gap_, struct_make<Int_Float_pair>(seed, now));
    if (math_compare<">">(kern::to<Float>(list_length<Int_Float_pair>(self.source_gap_)), 32.0)) {
      list_remove_at(self.source_gap_, 0);
    }
    while (loop(math_compare<">=">(kern::to<Float>(list_length<Life_Voice>(self.voices_)), 3.0))) {
      list_remove_at(self.voices_, 0);
    }
    list_add(self.voices_, struct_make<Life_Voice>(vector_add(at, Vector{0.0, 0.0, 2.05}), line, mood, kern::formula([](Float now, Float kBubble) -> Float { return (now + kBubble); }, now, kBubble)));
    vol = kern::formula([](Float d, Float kMaxHear) -> Float { return kern::math::max(0.0, (1.0 - kern::math::div(d, kMaxHear))); }, d, kMaxHear);
    k = hash01(int64_of_int(seed)).Result;
    if (math_compare<">">(vol, 0.03)) {
      list_add(out.speech, struct_make<LifeOut_Say>(line, kern::formula([](Float k) -> Float { return (0.65 + (k * 0.8)); }, k), kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? 1.35 : (1.12 + (the_result * 0.2))); }, kern::to<Float>(text_equal(mood, "scared")), hash01(mix(int64_of_int(seed), Int64{0x5}).Result).Result), kern::formula([](Float vol) -> Float { return kern::math::min(1.0, (0.25 + (vol * 0.9))); }, vol), 0.0));
    }
    self.shouts_ = kern::to<Int>(kern::formula([](Float shouts_) -> Float { return (shouts_ + 1.0); }, kern::to<Float>(self.shouts_)));
    return {true};
  }

  struct Life_honkResult { Bool Result = false; };
  Life_honkResult Life_honk(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Bool bus = false, U32 source = 0, Float now = 0.0, Float d = 0.0) {
    if (logic_or(math_compare<"<">(kern::formula([](Float now, Float last_honk_) -> Float { return (now - last_honk_); }, now, self.last_honk_), kHonkGap), math_compare<">">(d, 160.0))) {
      return {false};
    }
    for (auto it178 = flow_for(n178, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.source_gap_))))); it178.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.source_gap_[n178.index].first), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float source) -> Float { return kern::math::bor(source, 2147483648.0); }, kern::to<Float>(source)))))), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.source_gap_[n178.index].second), 4.0))) {
        return {false};
      }
    }
    self.last_honk_ = now;
    list_add(self.source_gap_, struct_make<Int_Float_pair>(kern::to<Int>(kern::formula([](Float source) -> Float { return kern::math::bor(source, 2147483648.0); }, kern::to<Float>(source))), now));
    list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(bus, LifeOut_Sound::horn_bus, LifeOut_Sound::horn), vector_add(at, Vector{0.0, 0.0, 1.0}), 0.8, 1.0));
    self.honks_ = kern::to<Int>(kern::formula([](Float honks_) -> Float { return (honks_ + 1.0); }, kern::to<Float>(self.honks_)));
    return {true};
  }

  struct Life_screamResult { Bool Result = false; };
  Life_screamResult Life_scream(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, U32 seed = 0, Float now = 0.0, Float d = 0.0) {
    Float k = 0.0;
    if (logic_or(math_compare<"<">(kern::formula([](Float now, Float last_scream_) -> Float { return (now - last_scream_); }, now, self.last_scream_), 0.45), math_compare<">">(d, 55.0))) {
      return {false};
    }
    self.last_scream_ = now;
    k = hash01(int64_of_int(seed)).Result;
    list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(math_compare<">">(k, 0.45), LifeOut_Sound::scream_f, LifeOut_Sound::scream), vector_add(at, Vector{0.0, 0.0, 1.6}), 0.9, kern::formula([](Float k) -> Float { return (0.9 + (k * 0.25)); }, k)));
    return {true};
  }

  // ---- what nodes give and keep ----
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n10;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n26;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct { Life_LooseBird value = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}}; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct { Life_Flock value = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}}; } n33;
  common::Rng_unitResult n35;
  common::Rng_nextResult n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  common::Rng_unitResult n40;
  common::Rng_rangeResult n41;
  Life_spot_nearResult n42;
  Life_spot_nearResult n43;
  common::Rng_nextResult n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  common::Rng_unitResult n46;
  common::Rng_unitResult n47;
  common::Rng_unitResult n48;
  common::Rng_unitResult n49;
  common::Rng_unitResult n50;
  common::Rng_rangeResult n51;
  common::Rng_rangeResult n52;
  common::Rng_rangeResult n53;
  Life_spawn_flockResult n54;
  struct : kern::Node<55> { Int index = 0; } n55;
  struct { Life_LooseBird value = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}}; } n56;
  struct : kern::Node<57> { Int index = 0; } n57;
  struct { Life_Pigeon value = Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}; } n58;
  common::Rng_rangeResult n59;
  common::Rng_rangeResult n60;
  Life_spot_nearResult n61;
  Life_spot_nearResult n62;
  common::Rng_rangeResult n63;
  common::Rng_rangeResult n64;
  common::Rng_unitResult n65;
  common::Rng_rangeResult n66;
  common::Rng_rangeResult n67;
  common::Rng_rangeResult n68;
  common::Rng_rangeResult n69;
  common::Rng_rangeResult n70;
  common::Rng_rangeResult n71;
  common::Rng_rangeResult n72;
  struct : kern::Node<73> { Int index = 0; } n73;
  common::Rng_rangeResult n74;
  common::Rng_rangeResult n75;
  struct : kern::Node<76> { Int index = 0; } n76;
  common::Rng_unitResult n77;
  common::Rng_rangeResult n78;
  common::Rng_rangeResult n79;
  common::Rng_rangeResult n80;
  common::Rng_rangeResult n81;
  struct : kern::Node<82> { Int index = 0; } n82;
  struct : kern::Node<83> { Int index = 0; } n83;
  common::Rng_rangeResult n84;
  common::Rng_rangeResult n85;
  struct : kern::Node<86> { Int index = 0; } n86;
  struct { Bool added = false; Int index = 0; } n87;
  struct { Life_Tree value = Life_Tree{Vector{0.0, 0.0, 0.0}, 1.0, 0, -1.0, Vector{1.0, 0.0, 0.0}, Int64{0x0}, 0}; } n88;
  struct : kern::Node<89> { Int index = 0; } n89;
  struct { Bool added = false; Int index = 0; } n90;
  struct : kern::Node<91> { Int index = 0; } n91;
  struct : kern::Node<92> { Int index = 0; } n92;
  struct { Bool added = false; Int index = 0; } n93;
  struct : kern::Node<94> { Int index = 0; } n94;
  struct : kern::Node<95> { Int index = 0; } n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct : kern::Node<97> { Int index = 0; } n97;
  struct : kern::Node<98> { Int index = 0; } n98;
  struct : kern::Node<99> { Int index = 0; } n99;
  struct : kern::Node<100> { Int index = 0; } n100;
  struct : kern::Node<101> { Int index = 0; } n101;
  struct : kern::Node<102> { Int index = 0; } n102;
  struct : kern::Node<103> { Int index = 0; } n103;
  struct : kern::Node<104> { Int index = 0; } n104;
  common::Rng_rangeResult n105;
  struct : kern::Node<106> { Int index = 0; } n106;
  struct : kern::Node<107> { Int index = 0; } n107;
  struct : kern::Node<108> { Int index = 0; } n108;
  common::Rng_unitResult n109;
  common::Rng_unitResult n110;
  struct : kern::Node<111> { Int index = 0; } n111;
  struct : kern::Node<112> { Int index = 0; } n112;
  struct : kern::Node<113> { Int index = 0; } n113;
  Life_sampleResult n114;
  common::Rng_nextResult n115;
  struct : kern::Node<116> { Int index = 0; } n116;
  Life_sampleResult n117;
  common::Rng_unitResult n118;
  Life_spawn_joggersResult n119;
  struct : kern::Node<120> { Int index = 0; } n120;
  Life_sampleResult n121;
  struct : kern::Node<122> { Int index = 0; } n122;
  struct : kern::Node<123> { Int index = 0; } n123;
  ragdoll_cpp::Ragdolls_spawnResult n124;
  struct : kern::Node<125> { Int index = 0; } n125;
  struct : kern::Node<126> { Int index = 0; } n126;
  struct { Life_Voice value = Life_Voice{Vector{0.0, 0.0, 0.0}, "", "", 0.0}; } n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  struct : kern::Node<129> { Int index = 0; } n129;
  struct : kern::Node<130> { Int index = 0; } n130;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n131;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n132;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n133;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n134;
  struct : kern::Node<135> { Int index = 0; } n135;
  common::Rng_unitResult n137;
  struct : kern::Node<138> { Int index = 0; } n138;
  struct : kern::Node<139> { Int index = 0; } n139;
  struct : kern::Node<140> { Int index = 0; } n140;
  struct : kern::Node<141> { Int index = 0; } n141;
  ragdoll_cpp::Ragdolls_spawnResult n142;
  struct : kern::Node<143> { Int index = 0; } n143;
  struct : kern::Node<144> { Int index = 0; } n144;
  struct : kern::Node<145> { Int index = 0; } n145;
  struct : kern::Node<146> { Int index = 0; } n146;
  struct : kern::Node<147> { Int index = 0; } n147;
  struct : kern::Node<148> { Int index = 0; } n148;
  struct : kern::Node<149> { Int index = 0; } n149;
  struct : kern::Node<150> { Int index = 0; } n150;
  struct : kern::Node<151> { Int index = 0; } n151;
  struct { Life_Flock value = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}}; } n152;
  struct : kern::Node<153> { Int index = 0; } n153;
  struct : kern::Node<154> { Int index = 0; } n154;
  struct : kern::Node<155> { Int index = 0; } n155;
  Life_grab_animalResult n156;
  Life_grab_animalResult n157;
  struct : kern::Node<158> { Int index = 0; } n158;
  struct : kern::Node<159> { Int index = 0; } n159;
  struct : kern::Node<160> { Int index = 0; } n160;
  struct : kern::Node<161> { Int index = 0; } n161;
  struct : kern::Node<162> { Int index = 0; } n162;
  struct : kern::Node<163> { Int index = 0; } n163;
  struct : kern::Node<164> { Int index = 0; } n164;
  Life_provoke_catResult n166;
  struct : kern::Node<167> { Int index = 0; } n167;
  struct : kern::Node<168> { Int index = 0; } n168;
  struct : kern::Node<169> { Int index = 0; } n169;
  struct : kern::Node<170> { Int index = 0; } n170;
  struct : kern::Node<171> { Int index = 0; } n171;
  struct : kern::Node<172> { Int index = 0; } n172;
  struct : kern::Node<173> { Int index = 0; } n173;
  struct : kern::Node<174> { Int index = 0; } n174;
  struct : kern::Node<175> { Int index = 0; } n175;
  struct : kern::Node<176> { Int index = 0; } n176;
  struct : kern::Node<177> { Int index = 0; } n177;
  struct : kern::Node<178> { Int index = 0; } n178;
};
