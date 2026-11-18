struct life_cpp : virtual kern::Library, virtual common, virtual engine_physics_physics, virtual life, virtual ragdoll {
  // ---- types ----
  KERN_STRUCT(Lines) { Text mood = ""; Fixed<Text, 6> lines = List<Text>{}; Int n = 0; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  /// cows.mjs
  KERN_CONST Int kMaxCows = 40;
  KERN_CONST Fixed<Int, 5> kCoats = List<Int>{15855592, 15855592, 9067068, 3812388, 13609335};
  KERN_CONST Float kGrazeR = 6.0;
  KERN_CONST Float kCowMass = 600.0;
  /// street_voices.mjs
  KERN_CONST Float kShoutGap = 1.4;
  /// street_voices.mjs
  KERN_CONST Float kMaxHear = 38.0;
  /// street_voices.mjs
  KERN_CONST Float kSourceGap = 6.0;
  KERN_CONST Fixed<Lines, 5> kLines = List<Lines>{Lines{"bump", List<Text>{"Hey, pass doch auf!", "Geht's noch?", "Augen auf, Mann!", "Was soll das denn?", "Mann, ey!", "Hast du keine Augen im Kopf?"}, 6}, Lines{"angry", List<Text>{"Na warte!", "Jetzt reicht's mir!", "Willst du \303\204rger?", "Komm her, du Idiot!", "Dir zeig ich's!", "Du hast sie doch nicht alle!"}, 6}, Lines{"scared", List<Text>{}, 5}, Lines{"nearmiss", List<Text>{}, 5}, Lines{"honked", List<Text>{}, 4}};
  /// street_voices.mjs
  KERN_CONST Float kBubble = 2.1;
  /// street_voices.mjs
  KERN_CONST Float kHonkGap = 1.2;

  // ---- functions ----
  struct Life_speciesResult { Life_Species Result = Life_Species::dog; };
  KERN_PURE Life_speciesResult Life_species(const Life& self, Int i = 0, Float epoch = 0.0) {
    if (logic_and(math_compare<"==">(kern::to<Float>(i), 1.0), logic_or(int64_equal(int64_rem(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float epoch) -> Float { return kern::math::floor(kern::math::div(epoch, 120.0)); }, epoch))), Int64{0x2}), Int64{0x0}), self.cat_.on))) {
      return {Life_Species::black_cat};
    }
    return {logic_select<Life_Species>(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pet_base_, Float i) -> Float { return kern::math::mod(((pet_base_ != 0.0) ? kern::math::mod(i, pet_base_) : i), 3.0); }, kern::to<Float>(self.pet_base_), kern::to<Float>(i))))), 2.0), Life_Species::cat, Life_Species::dog)};
  }

  void Life_kill_pet(Life& self, LifeOut& out, Int i = 0, Vector dir = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Life_Species sp = Life_Species::dog;
    Float sc = 0.0;
    Vector half = Vector{0.0, 0.0, 0.0};
    Float mass = 0.0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
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
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
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
  void Life_net_kill_pet(Life& self, LifeOut& out, Int index = 0, Vector dir = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
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
  KERN_PURE rgbResult rgb(Int h = 0) {
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
  void Life_pet_params(Life_Pet& p, Int i = 0) {
    Int64 h = Int64{0x0};
    h = mix(Int64{0xa11a}, int64_of_int(i)).Result;
    p.radius = kern::formula([](Float the_result) -> Float { return (13.0 + (45.0 * the_result)); }, hash01(int64_add(h, Int64{0x1})).Result);
    p.omega = kern::formula([](Float the_result, Float radius) -> Float { return kern::math::div((0.55 + (0.9 * the_result)), radius); }, hash01(int64_add(h, Int64{0x2})).Result, p.radius);
    p.phase = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, hash01(int64_add(h, Int64{0x3})).Result, kPi);
    p.wobble_omega = kern::formula([](Float the_result) -> Float { return (0.12 + (0.18 * the_result)); }, hash01(int64_add(h, Int64{0x4})).Result);
    p.wobble_r = kern::formula([](Float the_result) -> Float { return (1.2 + (2.3 * the_result)); }, hash01(int64_add(h, Int64{0x5})).Result);
    p.scale = kern::formula([](Float the_result) -> Float { return (0.72 + (0.46 * the_result)); }, hash01(int64_add(h, Int64{0x6})).Result);
  }

  void Life_init(Life& self, Int w = 0, Bool mobile = false) {
    Int w_ = 0;
    Int count = 0;
    Life_Pet fill = Life_Pet{20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, physics_Body{Int64{0x0}}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Int n = 0;
    Int count_2 = 0;
    Life_Bird fill_2 = Life_Bird{0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, -1.0, -1.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false};
    w_ = w;
    self.world_ = w_;
    self.mobile_ = mobile;
    self.pet_base_ = kern::to<Int>(kern::formula([](Float mobile) -> Float { return kern::math::wrap32(((mobile != 0.0) ? 12.0 : 20.0)); }, kern::to<Float>(mobile)));
    count = self.pet_base_;
    fill = struct_make<Life_Pet>(20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, struct_make<physics_Body>(Int64{0x0}), struct_make<Quat>(0.0, 0.0, 0.0, 1.0), struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
    list_clear(self.pets_);
    list_resize(self.pets_, count, fill);
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it13.next();) {
      Life_pet_params(self.pets_[n13.index], n13.index);
    }
    ragdoll::Ragdolls_init(self.ragdolls_, w_, mobile);
    n = kern::to<Int>(kern::formula([](Float mobile) -> Float { return kern::math::wrap32(((mobile != 0.0) ? 10.0 : 16.0)); }, kern::to<Float>(mobile)));
    count_2 = n;
    fill_2 = struct_make<Life_Bird>(0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, -1.0, -1.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false);
    list_clear(self.birds_);
    list_resize(self.birds_, count_2, fill_2);
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it14.next();) {
      self.birds_[n14.index].seed = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc((the_result * 4294967295.0)); }, hash01(mix(Int64{0xb17d}, int64_of_int(n14.index)).Result).Result));
    }
  }

  struct Life_spawn_herdResult { Bool Result = false; };
  Life_spawn_herdResult Life_spawn_herd(Life& self, Vector center = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Int n = 0, Float now = 0.0) {
    Life_Meadow m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    Float a = 0.0;
    Int mi = 0;
    Int i = 0;
    Life_Cow c = Life_Cow{Int64{0x0}, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 100.0, 0.0, 0.0, 0.0, -9.0, -9.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, false, false, false, 0.0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Quat{0.0, 0.0, 0.0, 1.0}, false};
    Float a_2 = 0.0;
    Float r = 0.0;
    m = Life_Meadow{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Int64{0x0}, true};
    m.key = mix(Int64{0xc0c0}, int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)))).Result;
    for (auto it19 = flow_for(n19, 0, 23); it19.next();) {
      a = kern::formula([](Float index, Float kPi) -> Float { return ((kern::math::div(index, 24.0) * 2.0) * kPi); }, kern::to<Float>(n19.index), kPi);
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
      c.color = rgb(list_get<Int>(kCoats, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 5.0); }, kern::to<Float>(i))))).Result;
      c.next_moo = kern::formula([](Float now) -> Float { return (now + 1.0); }, now);
      c.pos = vector_make(vector_break(c.home).x, vector_break(c.home).y, 0.0);
      list_add(self.cows_, c);
      i += 1;
    }
    self.herds_for_ = center;
    return {true};
  }

  struct Life_graze_spotResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Life_graze_spotResult Life_graze_spot(const Life& self, const Life_Cow& c, Int64 k = Int64{0x0}) {
    Bool fenced = false;
    Int64 h = Int64{0x0};
    Float a = 0.0;
    Float r = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    fenced = logic_and(math_compare<">=">(kern::to<Float>(c.meadow), 0.0), math_compare<"<">(kern::to<Float>(c.meadow), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Life_Meadow>(self.meadows_))))))));
    for (auto it20 = flow_for(n20, 0, 5); it20.next();) {
      h = mix(c.key, int64_add(int64_mul(k, Int64{0x10}), int64_of_int(n20.index))).Result;
      a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, hash01(h).Result, kPi);
      r = kern::formula([](Float the_result, Float kGrazeR) -> Float { return (kern::math::sqrt(the_result) * kGrazeR); }, hash01(int64_add(h, Int64{0x1})).Result, kGrazeR);
      p = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(c.home).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin(a) * r)); }, vector_break(c.home).y, a, r), 0.0);
      if (logic_or(logic_not(fenced), Life_in_rings(self.meadows_[c.meadow].rings, p).Result)) {
        return {p};
      }
    }
    return {c.home};
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

  void Life_kill_cow(Life& self, Life_Cow& c, LifeOut& out, Vector impulse = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
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
    engine_world_create_body(n21, self.world_, bs);
    create_body = n21.result;
    c.body = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    ss.kind = physics_ShapeKind::box;
    ss.size = Vector{1.64, 0.68, 0.76};
    ss.material.density = kern::formula([](Float kCowMass) -> Float { return kern::math::div(kCowMass, ((1.64 * 0.68) * 0.76)); }, kCowMass);
    ss.material.friction = 0.7;
    ss.material.restitution = 0.05;
    ss.material.category_bits = Int64{0x1};
    engine_world_create_shape(n22, self.world_, c.body, ss);
    create_shape = n22.result;
    engine_world_apply_linear_impulse(self.world_, c.body, impulse, vector_add(c.pos, Vector{0.0, 0.0, 1.45}));
  }

  struct Life_grab_animalResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  Life_grab_animalResult Life_grab_animal(Life& self, Vector at = Vector{0.0, 0.0, 0.0}, Float reach = 0.0) {
    Float best = 0.0;
    Int cow = 0;
    Int pet = 0;
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
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    Bool body_valid_7 = false;
    Bool body_valid_8 = false;
    Bool both_4 = false;
    physics_BodySettings bs_2 = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body_2 = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss_2 = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape_2 = physics_Shape{Int64{0x0}};
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.world_), 0.0))) {
      return {struct_make<physics_Body>(Int64{0x0})};
    }
    best = reach;
    cow = -1;
    pet = -1;
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it23.next();) {
      skip = false;
      if (self.cows_[n23.index].dead) {
        skip = true;
      }
      if (logic_not(skip)) {
        both = logic_and(self.cows_[n23.index].carried, engine_physics_physics::Body_valid(self.cows_[n23.index].body).Result);
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n23.index].body);
          both = body_valid_2;
        }
        if (both) {
          pose = engine_world_pose(self.world_, self.cows_[n23.index].body);
          d = vector_length(vector_sub(struct_break<physics_Pose>(pose).position, at));
          if (math_compare<"<">(d, kern::formula([](Float best) -> Float { return (best + 0.6); }, best))) {
            best = d;
            cow = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n23.index)));
            pet = -1;
          }
          skip = true;
        }
        if (logic_not(skip)) {
          to_local = vector_sub(at, vector_add(self.cows_[n23.index].pos, Vector{0.0, 0.0, 1.1}));
          d_2 = kern::formula([](Float a) -> Float { return kern::math::max(0.0, (a - 0.75)); }, vector_length(to_local));
          if (math_compare<"<">(d_2, best)) {
            best = d_2;
            cow = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n23.index)));
            pet = -1;
          }
        }
      }
    }
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it24.next();) {
      skip_2 = false;
      if (logic_or(logic_not(self.pets_[n24.index].shown), math_compare<">=">(self.pets_[n24.index].dead_at, 0.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        both_2 = logic_and(self.pets_[n24.index].carried, engine_physics_physics::Body_valid(self.pets_[n24.index].body).Result);
        if (both_2) {
          body_valid_4 = engine_world_body_valid(self.world_, self.pets_[n24.index].body);
          both_2 = body_valid_4;
        }
        if (both_2) {
          pose_3 = engine_world_pose(self.world_, self.pets_[n24.index].body);
          chosen = struct_break<physics_Pose>(pose_3).position;
        } else {
          chosen = vector_add(self.pets_[n24.index].pos, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return (0.3 * a); }, self.pets_[n24.index].scale)));
        }
        c = chosen;
        d_3 = kern::formula([](Float a, Float a_2) -> Float { return kern::math::max(0.0, (a - (0.25 * a_2))); }, vector_length(vector_sub(at, c)), self.pets_[n24.index].scale);
        if (math_compare<"<">(d_3, best)) {
          best = d_3;
          pet = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n24.index)));
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
      engine_world_create_body(n25, self.world_, bs);
      create_body = n25.result;
      self.pets_[pet].body = create_body;
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::box;
      ss.size = vector_scale(half, 2.0);
      ss.material.density = kern::formula([](Float mass, Float x, Float y, Float z) -> Float { return kern::math::div(mass, (((8.0 * x) * y) * z)); }, mass, vector_break(half).x, vector_break(half).y, vector_break(half).z);
      ss.material.friction = 0.7;
      ss.material.category_bits = Int64{0x1};
      engine_world_create_shape(n26, self.world_, self.pets_[pet].body, ss);
      create_shape = n26.result;
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
      engine_world_create_body(n27, self.world_, bs_2);
      create_body_2 = n27.result;
      self.cows_[cow].body = create_body_2;
      ss_2 = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss_2.kind = physics_ShapeKind::box;
      ss_2.size = Vector{1.64, 0.68, 0.76};
      ss_2.material.density = kern::formula([](Float kCowMass) -> Float { return kern::math::div(kCowMass, ((1.64 * 0.68) * 0.76)); }, kCowMass);
      ss_2.material.friction = 0.7;
      ss_2.material.restitution = 0.05;
      ss_2.material.category_bits = Int64{0x1};
      engine_world_create_shape(n28, self.world_, self.cows_[cow].body, ss_2);
      create_shape_2 = n28.result;
      self.cows_[cow].rot = bs_2.rotation;
      self.cows_[cow].carried = true;
      self.cows_[cow].carried_still = 0.0;
      return {self.cows_[cow].body};
    }
    return {struct_make<physics_Body>(Int64{0x0})};
  }

  struct Life_catch_pigeonResult { Int Result = 0; };
  Life_catch_pigeonResult Life_catch_pigeon(Life& self, Rng& rng, Vector hand = Vector{0.0, 0.0, 0.0}, Float hand_speed = 0.0, Float now = 0.0) {
    Bool skip = false;
    Int i = 0;
    Bool skip_2 = false;
    Bool either = false;
    Life_LooseBird lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
    for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it34.next();) {
      skip = false;
      if (self.flocks_[n34.index].air) {
        skip = true;
      }
      if (logic_not(skip)) {
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Life_Pigeon>(self.flocks_[n34.index].birds))), 0.5))) {
          skip_2 = false;
          if (logic_or(math_compare<">=">(self.flocks_[n34.index].birds[i].dead_at, 0.0), math_compare<">">(vector_length(vector_sub(hand, vector_add(self.flocks_[n34.index].birds[i].pos, Vector{0.0, 0.0, 0.15}))), 0.24))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            either = math_compare<">">(hand_speed, 1.0);
            if (logic_not(either)) {
              n36 = common::Rng_unit(rng);
              either = math_compare<"<">(n36.Result, 0.5);
            }
            if (either) {
              self.flocks_[n34.index].next_flight = now;
              return {-1};
            }
            lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
            self.loose_serial_ = kern::to<Int>(kern::formula([](Float loose_serial_) -> Float { return (loose_serial_ + 1.0); }, kern::to<Float>(self.loose_serial_)));
            lb.id = self.loose_serial_;
            lb.pos = vector_add(self.flocks_[n34.index].birds[i].pos, Vector{0.0, 0.0, 0.15});
            lb.yaw = self.flocks_[n34.index].birds[i].yaw;
            list_add(self.loose_, lb);
            list_remove_at(self.flocks_[n34.index].birds, i);
            self.flocks_[n34.index].next_flight = kern::formula([](Float now) -> Float { return (now + 0.4); }, now);
            return {lb.id};
          }
          i += 1;
        }
      }
    }
    return {-1};
  }

  void Life_hold_pigeon(Life& self, Int id = 0, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0) {
    for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it37.next();) {
      if (math_compare<"==">(kern::to<Float>(self.loose_[n37.index].id), kern::to<Float>(id))) {
        self.loose_[n37.index].pos = at;
        self.loose_[n37.index].yaw = yaw;
        self.loose_[n37.index].held = true;
        self.loose_[n37.index].flap = kern::formula([](Float a) -> Float { return (a + 0.9); }, self.loose_[n37.index].flap);
      }
    }
  }

  void Life_release_pigeon(Life& self, Int id = 0, Vector velocity = Vector{0.0, 0.0, 0.0}) {
    for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it38.next();) {
      if (math_compare<"==">(kern::to<Float>(self.loose_[n38.index].id), kern::to<Float>(id))) {
        self.loose_[n38.index].held = false;
        self.loose_[n38.index].t = 0.0;
        self.loose_[n38.index].vel = vector_add(velocity, Vector{0.0, 0.0, 2.5});
      }
    }
  }

  void Life_spook_pigeons(Life& self, Float now = 0.0) {
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it39.next();) {
      self.flocks_[n39.index].next_flight = now;
    }
  }

  struct update_trees_key_ofResult { Int Result = 0; };
  KERN_PURE update_trees_key_ofResult update_trees_key_of(Int64 tile = Int64{0x0}, Int index = 0) {
    return {int64_to_int(int64_add(int64_mul(tile, Int64{0x9e3779b97f4a7c15}), int64_of_int(index)))};
  }

  struct update_trees_matchResult { Bool Result = false; };
  KERN_PURE update_trees_matchResult update_trees_match(const Life_Tree& t, Float now = 0.0) {
    return {logic_and(math_compare<">=">(t.fell_at, 0.0), math_compare<">">(kern::formula([](Float now, Float fell_at) -> Float { return (now - fell_at); }, now, t.fell_at), 40.0))};
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
    for (auto it64 = flow_for(n64, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it64.next();) {
      both = engine_physics_physics::Body_valid(self.cows_[n64.index].body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n64.index].body);
        both = body_valid_2;
      }
      if (both) {
        engine_world_destroy_body(self.world_, self.cows_[n64.index].body);
      }
    }
    for (auto it65 = flow_for(n65, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it65.next();) {
      both_2 = engine_physics_physics::Body_valid(self.pets_[n65.index].body).Result;
      if (both_2) {
        body_valid_4 = engine_world_body_valid(self.world_, self.pets_[n65.index].body);
        both_2 = body_valid_4;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.pets_[n65.index].body);
      }
    }
    list_clear(self.cows_);
    list_clear(self.meadows_);
    list_clear(self.flocks_);
    for (auto it66 = flow_for(n66, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it66.next();) {
      self.pets_[n66.index].body = struct_make<physics_Body>(Int64{0x0});
      self.pets_[n66.index].shown = false;
      self.pets_[n66.index].has_pose = false;
      self.pets_[n66.index].carried = false;
      self.pets_[n66.index].dead_at = -1.0;
    }
    self.herds_for_ = Vector{1000000000.0, 1000000000.0, 0.0};
    self.pet_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0};
  }

  struct apply_cows_matchResult { Bool Result = false; };
  KERN_PURE apply_cows_matchResult apply_cows_match(const Life_NetCow& cows_n67_index, const Life_Cow& x) {
    return {int64_equal(x.key, cows_n67_index.key)};
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
    for (auto it67 = flow_for(n67, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetCow>(cows))))); it67.next();) {
      at_2 = 0;
      end = list_length<Life_Cow>(self.cows_);
      go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
      while (loop(go)) {
        if (apply_cows_match(cows[n67.index], self.cows_[at_2]).Result) {
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
        c.key = cows[n67.index].key;
        c.color = rgb(list_get<Int>(kCoats, kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::mod(kern::math::trunc((the_result * 5.0)), 5.0); }, hash01(mix(cows[n67.index].key, Int64{0x7}).Result).Result)))).Result;
        c.phase = kern::formula([](Float the_result) -> Float { return (the_result * 6.0); }, hash01(cows[n67.index].key).Result);
        c.pos = cows[n67.index].pos;
        c.yaw = cows[n67.index].yaw;
        c.next_moo = kern::formula([](Float now, Float the_result) -> Float { return ((now + 8.0) + (the_result * 30.0)); }, now, hash01(mix(cows[n67.index].key, Int64{0x9}).Result).Result);
      }
      both = engine_physics_physics::Body_valid(c.body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(self.world_, c.body);
        both = body_valid_2;
      }
      mine = both;
      if (logic_not(mine)) {
        if (logic_and(math_compare<">">(at, kern::formula([](Float net_at) -> Float { return (net_at + 0.02); }, c.net_at)), math_compare<">">(c.net_at, 0.0))) {
          v = vector_scale(vector_sub(cows[n67.index].pos, c.net_to), kern::formula([](Float at, Float net_at) -> Float { return kern::math::div(1.0, (at - net_at)); }, at, c.net_at));
          c.net_v = logic_select<Vector>(math_compare<"<">(vector_length(v), 15.0), v, Vector{0.0, 0.0, 0.0});
        }
        c.net_at = at;
        c.net_to = cows[n67.index].pos;
        c.net_rot = cows[n67.index].rot;
        c.net_dead = cows[n67.index].dead;
        c.walking = cows[n67.index].walking;
        c.carried = cows[n67.index].carried;
        c.yaw = kern::formula([](Float yaw, Float the_result) -> Float { return (yaw + the_result); }, c.yaw, common::wrap_pi(kern::formula([](Float a, Float yaw) -> Float { return (a - yaw); }, cows[n67.index].yaw, c.yaw)).Result);
      }
      list_add(next, c);
    }
    for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it68.next();) {
      both_2 = engine_physics_physics::Body_valid(self.cows_[n68.index].body).Result;
      if (both_2) {
        body_valid_4 = engine_world_body_valid(self.world_, self.cows_[n68.index].body);
        both_2 = body_valid_4;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.cows_[n68.index].body);
      }
    }
    self.cows_ = next;
  }

  void Life_apply_pets(Life& self, const List<Life_NetPet>& pets, const List<Vector>& pet_anchors, Int pet_count = 0, Float now = 0.0) {
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
      for (auto it69 = flow_for(n69, had, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(self.pets_))))); it69.next();) {
        Life_pet_params(self.pets_[n69.index], kern::to<Int>(kern::formula([](Float index, Float pet_base_) -> Float { return kern::math::mod(kern::math::wrap32(index), pet_base_); }, kern::to<Float>(n69.index), kern::to<Float>(self.pet_base_))));
      }
    }
    list_resize(self.pet_net_to_, pet_count, Vector{0.0, 0.0, 0.0});
    list_resize(self.pet_net_yaw_, pet_count, 0.0);
    made = List<Bool>{};
    list_resize(made, pet_count, false);
    seen = made;
    for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetPet>(pets))))); it70.next();) {
      skip = false;
      if (math_compare<">=">(kern::to<Float>(pets[n70.index].idx), kern::to<Float>(pet_count))) {
        skip = true;
      }
      if (logic_not(skip)) {
        seen[pets[n70.index].idx] = true;
        both = engine_physics_physics::Body_valid(self.pets_[pets[n70.index].idx].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.pets_[pets[n70.index].idx].body);
          both = body_valid_2;
        }
        if (both) {
          skip = true;
        }
        if (logic_not(skip)) {
          self.pet_net_to_[pets[n70.index].idx] = pets[n70.index].pos;
          self.pet_net_yaw_[pets[n70.index].idx] = pets[n70.index].yaw;
          self.pets_[pets[n70.index].idx].off = pets[n70.index].off;
          self.pets_[pets[n70.index].idx].net_rot = pets[n70.index].rot;
          self.pets_[pets[n70.index].idx].shown = pets[n70.index].shown;
          self.pets_[pets[n70.index].idx].carried = pets[n70.index].carried;
          if (logic_and(pets[n70.index].dead, math_compare<"<">(self.pets_[pets[n70.index].idx].dead_at, 0.0))) {
            self.pets_[pets[n70.index].idx].dead_at = now;
          }
          if (logic_not(pets[n70.index].dead)) {
            self.pets_[pets[n70.index].idx].dead_at = -1.0;
          }
        }
      }
    }
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(pet_count)))); it71.next();) {
      both_4 = logic_not(seen[n71.index]);
      if (both_4) {
        both_3 = engine_physics_physics::Body_valid(self.pets_[n71.index].body).Result;
        if (both_3) {
          body_valid_6 = engine_world_body_valid(self.world_, self.pets_[n71.index].body);
          both_3 = body_valid_6;
        }
        both_4 = logic_not(both_3);
      }
      if (both_4) {
        self.pets_[n71.index].shown = false;
        self.pets_[n71.index].has_pose = false;
      }
    }
  }

  struct apply_flocks_matchResult { Bool Result = false; };
  KERN_PURE apply_flocks_matchResult apply_flocks_match(const Life_NetFlock& flocks_n72_index, const Life_Flock& x) {
    return {math_compare<"==">(kern::to<Float>(x.id), kern::to<Float>(flocks_n72_index.id))};
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
    for (auto it72 = flow_for(n72, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_NetFlock>(flocks))))); it72.next();) {
      at_2 = 0;
      end = list_length<Life_Flock>(self.flocks_);
      go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
      while (loop(go)) {
        if (apply_flocks_match(flocks[n72.index], self.flocks_[at_2]).Result) {
          go = false;
        } else {
          at_2 += 1;
          go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
        }
      }
      old = at_2;
      f = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}};
      if (logic_not(math_compare<"==">(kern::to<Float>(old), kern::to<Float>(list_length<Life_Flock>(self.flocks_))))) {
        var_take(n73, self.flocks_[old]);
        self.flocks_[old].id = struct_break<Life_Flock>(n73.value).id;
        self.flocks_[old].air = struct_break<Life_Flock>(n73.value).air;
        self.flocks_[old].at = struct_break<Life_Flock>(n73.value).at;
        self.flocks_[old].next_flight = struct_break<Life_Flock>(n73.value).next_flight;
        self.flocks_[old].t = struct_break<Life_Flock>(n73.value).t;
        self.flocks_[old].coo_at = struct_break<Life_Flock>(n73.value).coo_at;
        self.flocks_[old].c = struct_break<Life_Flock>(n73.value).c;
        self.flocks_[old].from = struct_break<Life_Flock>(n73.value).from;
        self.flocks_[old].dest = struct_break<Life_Flock>(n73.value).dest;
        self.flocks_[old].r = struct_break<Life_Flock>(n73.value).r;
        self.flocks_[old].h = struct_break<Life_Flock>(n73.value).h;
        self.flocks_[old].dir = struct_break<Life_Flock>(n73.value).dir;
        self.flocks_[old].a0 = struct_break<Life_Flock>(n73.value).a0;
        self.flocks_[old].dur = struct_break<Life_Flock>(n73.value).dur;
        f = n73.value;
      }
      f.id = flocks[n72.index].id;
      f.air = flocks[n72.index].air;
      f.at = flocks[n72.index].at;
      if (math_compare<"!=">(kern::to<Float>(list_length<Life_Pigeon>(f.birds)), kern::to<Float>(list_length<Vector>(flocks[n72.index].birds)))) {
        list_resize(f.birds, list_length<Vector>(flocks[n72.index].birds), Life_Pigeon{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0});
        for (auto it74 = flow_for(n74, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(flocks[n72.index].birds))))); it74.next();) {
          f.birds[n74.index].pos = flocks[n72.index].birds[n74.index];
          f.birds[n74.index].yaw = flocks[n72.index].yaws[n74.index];
          f.birds[n74.index].dead_at = -1.0;
          f.birds[n74.index].peck = kern::formula([](Float index) -> Float { return ((index < 0.0 ? index + 18446744073709551616.0 : index) * 1.7); }, kern::to<Float>(n74.index));
          f.birds[n74.index].net_at = 0.0;
        }
      }
      for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(flocks[n72.index].birds))))); it75.next();) {
        skip = false;
        if (logic_and(math_compare<"!=">(kern::to<Float>(flocks[n72.index].dead[n75.index]), 0.0), math_compare<"<">(f.birds[n75.index].dead_at, 0.0))) {
          f.birds[n75.index].dead_at = now;
          f.birds[n75.index].fall_v = Vector{0.0, 0.0, 0.0};
        }
        if (logic_not(math_compare<"!=">(kern::to<Float>(flocks[n72.index].dead[n75.index]), 0.0))) {
          f.birds[n75.index].dead_at = -1.0;
        }
        if (math_compare<"!=">(kern::to<Float>(flocks[n72.index].dead[n75.index]), 0.0)) {
          if (math_compare<">">(vector_length(vector_sub(f.birds[n75.index].pos, flocks[n72.index].birds[n75.index])), 1.5)) {
            f.birds[n75.index].pos = flocks[n72.index].birds[n75.index];
          }
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(math_compare<">">(at, kern::formula([](Float a) -> Float { return (a + 0.02); }, f.birds[n75.index].net_at)), math_compare<">">(f.birds[n75.index].net_at, 0.0))) {
            v = vector_scale(vector_sub(flocks[n72.index].birds[n75.index], f.birds[n75.index].net_to), kern::formula([](Float at, Float a) -> Float { return kern::math::div(1.0, (at - a)); }, at, f.birds[n75.index].net_at));
            f.birds[n75.index].net_v = logic_select<Vector>(math_compare<"<">(vector_length(v), 25.0), v, Vector{0.0, 0.0, 0.0});
          }
          f.birds[n75.index].net_to = flocks[n72.index].birds[n75.index];
          f.birds[n75.index].net_at = at;
          f.birds[n75.index].net_yaw = flocks[n72.index].yaws[n75.index];
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
    for (auto it76 = flow_for(n76, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it76.next();) {
      skip = false;
      if (logic_or(logic_not(int64_equal(self.cows_[n76.index].key, key)), self.cows_[n76.index].dead)) {
        skip = true;
      }
      if (logic_not(skip)) {
        both = engine_physics_physics::Body_valid(self.cows_[n76.index].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.cows_[n76.index].body);
          both = body_valid_2;
        }
        if (both) {
          return {self.cows_[n76.index].body};
        }
        n77 = Life_grab_animal(self, vector_add(self.cows_[n76.index].pos, Vector{0.0, 0.0, 1.1}), 0.05);
        return {n77.Result};
      }
    }
    return {struct_make<physics_Body>(Int64{0x0})};
  }

  struct Life_net_carry_petResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  Life_net_carry_petResult Life_net_carry_pet(Life& self, Int index = 0) {
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
    n78 = Life_grab_animal(self, vector_add(self.pets_[index].pos, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return (0.3 * a); }, self.pets_[index].scale))), 0.05);
    return {n78.Result};
  }

  void Life_net_bird(Life& self, Int64 key = Int64{0x0}, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector vel = Vector{0.0, 0.0, 0.0}, Bool held = false) {
    Life_LooseBird lb = Life_LooseBird{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, true, Int64{0x0}};
    for (auto it79 = flow_for(n79, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(self.loose_))))); it79.next();) {
      if (int64_equal(self.loose_[n79.index].net, key)) {
        if (held) {
          self.loose_[n79.index].pos = at;
          self.loose_[n79.index].yaw = yaw;
          self.loose_[n79.index].held = true;
          self.loose_[n79.index].flap = kern::formula([](Float a) -> Float { return (a + 0.9); }, self.loose_[n79.index].flap);
        } else if (self.loose_[n79.index].held) {
          self.loose_[n79.index].held = false;
          self.loose_[n79.index].t = 0.0;
          self.loose_[n79.index].pos = at;
          self.loose_[n79.index].vel = vector_add(vel, Vector{0.0, 0.0, 2.5});
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

  void Life_remote_cow_death(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Vector impulse = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Int best = 0;
    Float bd = 0.0;
    best = -1;
    bd = 4.0;
    for (auto it87 = flow_for(n87, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it87.next();) {
      if (logic_and(logic_not(self.cows_[n87.index].dead), math_compare<"<">(dist2d(self.cows_[n87.index].pos, at).Result, bd))) {
        bd = dist2d(self.cows_[n87.index].pos, at).Result;
        best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n87.index)));
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
    for (auto it88 = flow_for(n88, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(self.cows_))))); it88.next();) {
      if (logic_and(logic_not(self.cows_[n88.index].dead), math_compare<"<">(dist2d(self.cows_[n88.index].pos, mid).Result, kern::formula([](Float len) -> Float { return (kern::math::div(len, 2.0) + 12.0); }, len)))) {
        Life_scare_cow(self.cows_[n88.index], vector_make(vector_break(from).x, vector_break(from).y, 0.0), now, 3.0);
      }
    }
    for (auto it89 = flow_for(n89, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(self.flocks_))))); it89.next();) {
      if (logic_and(logic_not(self.flocks_[n89.index].air), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.flocks_[n89.index].at).x, vector_break(from).x, vector_break(self.flocks_[n89.index].at).y, vector_break(from).y), 40.0))) {
        self.flocks_[n89.index].next_flight = kern::formula([](Float a, Float now) -> Float { return kern::math::min(a, now); }, self.flocks_[n89.index].next_flight, now);
      }
    }
  }

  struct Life_shoutResult { Bool Result = false; };
  Life_shoutResult Life_shout(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Text mood = "", Int seed = 0, Float now = 0.0, Float d = 0.0) {
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
    for (auto it94 = flow_for(n94, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.source_gap_))))); it94.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.source_gap_[n94.index].first), kern::to<Float>(seed)), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.source_gap_[n94.index].second), kSourceGap))) {
        return {false};
      }
    }
    pick = 0;
    i = 0;
    list = kLines;
    for (auto it95 = flow_for(n95, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lines>(list))))); it95.next();) {
      if (text_equal(list[n95.index].mood, mood)) {
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
    self.shouts_ = kern::to<Int>(kern::formula([](Float shouts_) -> Float { return kern::math::wrap32(shouts_ + 1.0); }, kern::to<Float>(self.shouts_)));
    return {true};
  }

  struct Life_honkResult { Bool Result = false; };
  Life_honkResult Life_honk(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Bool bus = false, Int source = 0, Float now = 0.0, Float d = 0.0) {
    if (logic_or(math_compare<"<">(kern::formula([](Float now, Float last_honk_) -> Float { return (now - last_honk_); }, now, self.last_honk_), kHonkGap), math_compare<">">(d, 160.0))) {
      return {false};
    }
    for (auto it96 = flow_for(n96, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.source_gap_))))); it96.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(self.source_gap_[n96.index].first), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float source) -> Float { return kern::math::bor(source, 2147483648.0); }, kern::to<Float>(source)))))), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.source_gap_[n96.index].second), 4.0))) {
        return {false};
      }
    }
    self.last_honk_ = now;
    list_add(self.source_gap_, struct_make<Int_Float_pair>(kern::to<Int>(kern::formula([](Float source) -> Float { return kern::math::bor(source, 2147483648.0); }, kern::to<Float>(source))), now));
    list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(bus, LifeOut_Sound::horn_bus, LifeOut_Sound::horn), vector_add(at, Vector{0.0, 0.0, 1.0}), 0.8, 1.0));
    self.honks_ = kern::to<Int>(kern::formula([](Float honks_) -> Float { return kern::math::wrap32(honks_ + 1.0); }, kern::to<Float>(self.honks_)));
    return {true};
  }

  struct Life_screamResult { Bool Result = false; };
  Life_screamResult Life_scream(Life& self, LifeOut& out, Vector at = Vector{0.0, 0.0, 0.0}, Int seed = 0, Float now = 0.0, Float d = 0.0) {
    Float k = 0.0;
    if (logic_or(math_compare<"<">(kern::formula([](Float now, Float last_scream_) -> Float { return (now - last_scream_); }, now, self.last_scream_), 0.45), math_compare<">">(d, 55.0))) {
      return {false};
    }
    self.last_scream_ = now;
    k = hash01(int64_of_int(seed)).Result;
    list_add(out.sounds, struct_make<LifeOut_Play>(logic_select<LifeOut_Sound>(math_compare<">">(k, 0.45), LifeOut_Sound::scream_f, LifeOut_Sound::scream), vector_add(at, Vector{0.0, 0.0, 1.6}), 0.9, kern::formula([](Float k) -> Float { return (0.9 + (k * 0.25)); }, k)));
    return {true};
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
          var_take(n97, self.voices_[at]);
          self.voices_[kept] = n97.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.voices_, kept, Life_Voice{Vector{0.0, 0.0, 0.0}, "", "", 0.0});
  }

  // ---- what nodes give and keep ----
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n10;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n21;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n25;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n26;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n27;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n28;
  struct : kern::Node<34> { Int index = 0; } n34;
  common::Rng_unitResult n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<64> { Int index = 0; } n64;
  struct : kern::Node<65> { Int index = 0; } n65;
  struct : kern::Node<66> { Int index = 0; } n66;
  struct : kern::Node<67> { Int index = 0; } n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct : kern::Node<72> { Int index = 0; } n72;
  struct { Life_Flock value = Life_Flock{0, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 12.0, 12.0, 1.0, 0.0, 10.0, List<Life_Pigeon>{}}; } n73;
  struct : kern::Node<74> { Int index = 0; } n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  struct : kern::Node<76> { Int index = 0; } n76;
  Life_grab_animalResult n77;
  Life_grab_animalResult n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<87> { Int index = 0; } n87;
  struct : kern::Node<88> { Int index = 0; } n88;
  struct : kern::Node<89> { Int index = 0; } n89;
  struct : kern::Node<94> { Int index = 0; } n94;
  struct : kern::Node<95> { Int index = 0; } n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct { Life_Voice value = Life_Voice{Vector{0.0, 0.0, 0.0}, "", "", 0.0}; } n97;
};
