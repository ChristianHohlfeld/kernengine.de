struct destruction_cpp : virtual kern::Library, virtual city_stream_cpp, virtual combat_art_cpp, virtual common, virtual destruction, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(Destruction_fracture_Seg) { Vector m = Vector{0.0, 0.0, 0.0}; Float len = 0.0; Float yaw = 0.0; };

  // ---- variables ----
  KERN_CONST Float kFloor = 3.2;
  /// facade panels at most 7 m wide
  KERN_CONST Float kPanelMax = 7.0;
  /// per broken building
  KERN_CONST U32 kMaxPieces = 40;
  KERN_CONST U32 kPieces = 2000;
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kEmber = Vector{1.0, 0.5411764705882353, 0.23921568627450981};
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kDim = Vector{0.4196078431372549, 0.35294117647058826, 0.2901960784313726};
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kConcrete = Vector{0.62, 0.58, 0.52};
  KERN_CONST Float kGravity = 9.81;
  KERN_CONST Float kPieceLife = 22.0;

  // ---- functions ----
  struct norm2Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE norm2Result norm2(Vector v = Vector{0.0, 0.0, 0.0}, Vector fallback = Vector{0.0, 0.0, 0.0}) {
    Float l = 0.0;
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(v).x, vector_break(v).y);
    return {logic_select<Vector>(math_compare<">">(l, 0.001), vector_make(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, vector_break(v).x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, vector_break(v).y, l), 0.0), fallback)};
  }

  struct Destruction_merge_pointsResult { Bool Result = false; };
  Destruction_merge_pointsResult Destruction_merge_points(Destruction& self, const List<Int_Float_pair>& list, Bool the_log = false) {
    Bool changed = false;
    Bool skip = false;
    Int key = 0;
    Float value = 0.0;
    Int_Bool_pair v = Int_Bool_pair{0, false};
    changed = false;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(list))))); it10.next();) {
      skip = false;
      if (logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, list[n10.index].second), 0.0)), math_compare<"<">(list[n10.index].second, 0.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        key = list[n10.index].first;
        value = list[n10.index].second;
        map_add(n11, self.points_, key);
        if (n11.added) {
          self.points_[n11.index] = value;
        }
        v = struct_make<Int_Bool_pair>(n11.index, n11.added);
        if (logic_not(v.second)) {
          if (math_compare<">=">(list[n10.index].second, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.points_[v.first]))) {
            skip = true;
          }
          if (logic_not(skip)) {
            self.points_[v.first] = list[n10.index].second;
          }
        }
        if (logic_not(skip)) {
          changed = true;
          if (the_log) {
            list_add(self.points_log_, struct_make<Int_Float_pair>(list[n10.index].first, list[n10.index].second));
          }
        }
      }
    }
    return {changed};
  }

  void Destruction_spawn(Destruction& self, Rng& rng, Vector at = Vector{0.0, 0.0, 0.0}, Vector v = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Destruction_Piece p = Destruction_Piece{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false};
    if (math_compare<">">(kern::formula([](Float a, Float kPieces) -> Float { return (!((a < 0.0) != (kPieces < 0.0) ? (kPieces < 0.0) : (a < kPieces))); }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_)), kern::to<Float>(kPieces)), 0.5)) {
      return;
    }
    p = Destruction_Piece{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false};
    p.p = at;
    p.v = v;
    p.size = size;
    p.yaw = yaw;
    n32 = common::Rng_range(rng, -1.0, 1.0);
    n33 = common::Rng_range(rng, -1.0, 1.0);
    n34 = common::Rng_range(rng, -1.0, 1.0);
    p.axis = logic_select<Vector>(math_compare<">">(vector_length(vector_make(n32.Result, n33.Result, n34.Result)), 9.999999717180685e-10), vector_normalize(vector_make(n32.Result, n33.Result, n34.Result)), Vector{1.0, 0.0, 0.0});
    n35 = common::Rng_range(rng, 0.0, 6.28);
    p.angle = n35.Result;
    n36 = common::Rng_range(rng, -9.0, 9.0);
    p.spin = n36.Result;
    p.tint = tint;
    list_add(self.pieces_, p);
  }

  void Destruction_fracture_launch(Destruction& self, Rng& rng, Vector offset = Vector{0.0, 0.0, 0.0}, Vector center = Vector{0.0, 0.0, 0.0}, Vector outward = Vector{0.0, 0.0, 0.0}, Float I = 0.0, Float boost = 0.0, Vector at = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Vector at_ = Vector{0.0, 0.0, 0.0};
    Vector radial = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    Vector v = Vector{0.0, 0.0, 0.0};
    at_ = at;
    at_ = vector_add(at_, offset);
    radial = norm2(vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(at_).x, vector_break(center).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(at_).y, vector_break(center).y), 0.0), outward).Result;
    n27 = common::Rng_range(rng, 0.55, 1.2);
    speed = kern::formula([](Float i, Float the_result, Float boost) -> Float { return (((9.0 + (30.0 * i)) * the_result) * boost); }, I, n27.Result, boost);
    n28 = common::Rng_range(rng, -5.0, 5.0);
    n29 = common::Rng_range(rng, -5.0, 5.0);
    n30 = common::Rng_range(rng, 2.0, 8.0);
    n31 = common::Rng_range(rng, 0.2, 1.0);
    v = vector_make(kern::formula([](Float x, Float speed, Float the_result) -> Float { return ((x * speed) + the_result); }, vector_break(radial).x, speed, n28.Result), kern::formula([](Float y, Float speed, Float the_result) -> Float { return ((y * speed) + the_result); }, vector_break(radial).y, speed, n29.Result), kern::formula([](Float the_result, Float i, Float the_result_2) -> Float { return (the_result + ((8.0 * i) * the_result_2)); }, n30.Result, I, n31.Result));
    Destruction_spawn(self, rng, at_, v, size, yaw, tint);
  }

  struct fracture_tintResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  fracture_tintResult fracture_tint(Rng& rng, Float ember = 0.0) {
    Float u = 0.0;
    n37 = common::Rng_unit(rng);
    u = n37.Result;
    return {logic_select<Vector>(math_compare<"<">(u, ember), kEmber, logic_select<Vector>(math_compare<"<">(u, 0.35), kDim, kConcrete))};
  }

  void Destruction_fracture(Destruction& self, const Destruction_Broken& b, Rng& rng, Vector center = Vector{0.0, 0.0, 0.0}, Vector offset = Vector{0.0, 0.0, 0.0}, Float boost = 0.0) {
    Float removed = 0.0;
    Vector outward = Vector{0.0, 0.0, 0.0};
    Float I = 0.0;
    List<Destruction_fracture_Seg> segs = List<Destruction_fracture_Seg>{};
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    U32 parts = 0;
    Float t = 0.0;
    U32 floors = 0;
    U32 roof = 0;
    Int stride = 0;
    Float the_band = 0.0;
    Int i = 0;
    Destruction_fracture_Seg s = Destruction_fracture_Seg{Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    Vector arg = Vector{0.0, 0.0, 0.0};
    Vector arg_2 = Vector{0.0, 0.0, 0.0};
    Vector arg_3 = Vector{0.0, 0.0, 0.0};
    Float arg_4 = 0.0;
    Float arg_5 = 0.0;
    Vector arg_6 = Vector{0.0, 0.0, 0.0};
    Vector arg_7 = Vector{0.0, 0.0, 0.0};
    Float arg_8 = 0.0;
    Vector arg_9 = Vector{0.0, 0.0, 0.0};
    Vector arg_10 = Vector{0.0, 0.0, 0.0};
    Vector arg_11 = Vector{0.0, 0.0, 0.0};
    Float arg_12 = 0.0;
    Float arg_13 = 0.0;
    Vector arg_14 = Vector{0.0, 0.0, 0.0};
    Vector arg_15 = Vector{0.0, 0.0, 0.0};
    Float arg_16 = 0.0;
    removed = kern::formula([](Float top, Float new_top) -> Float { return (top - new_top); }, b.top, b.new_top);
    if (math_compare<"<">(removed, 0.5)) {
      return;
    }
    outward = norm2(vector_sub(b.c, vector_make(vector_break(center).x, vector_break(center).y, 0.0)), Vector{1.0, 0.0, 0.0}).Result;
    I = kern::formula([](Float intensity) -> Float { return kern::math::min(intensity, 2.5); }, b.intensity);
    segs = List<Destruction_fracture_Seg>{};
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(b.rings))))); it24.next();) {
      for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n24.index]).items))))); it25.next();) {
        skip = false;
        a = b.rings[n24.index].items[n25.index];
        c = b.rings[n24.index].items[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n25.index), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n24.index]).items))))];
        len = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
        if (math_compare<"<">(len, 0.4)) {
          skip = true;
        }
        if (logic_not(skip)) {
          parts = kern::to<Int>(kern::formula([](Float len, Float kPanelMax) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::ceil(kern::math::div(len, kPanelMax)))); }, len, kPanelMax));
          for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(parts)))); it26.next();) {
            t = kern::formula([](Float index, Float parts) -> Float { return kern::math::div((index + 0.5), parts); }, kern::to<Float>(n26.index), kern::to<Float>(parts));
            list_add(segs, struct_make<Destruction_fracture_Seg>(vector_add(a, vector_scale(vector_sub(c, a), t)), kern::formula([](Float len, Float parts) -> Float { return kern::math::div(len, parts); }, len, kern::to<Float>(parts)), kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(c).y, vector_break(a).y, vector_break(c).x, vector_break(a).x)));
          }
        }
      }
    }
    floors = kern::to<Int>(kern::formula([](Float removed, Float kFloor) -> Float { return kern::math::max(1.0, kern::math::wrap32(kern::math::round(kern::math::div(removed, kFloor)))); }, removed, kFloor));
    roof = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::wrap32(kern::math::round(kern::math::div(((x - x_2) * (y - y_2)), 60.0))); }, vector_break(b.hi).x, vector_break(b.lo).x, vector_break(b.hi).y, vector_break(b.lo).y)))), 1.0, 10.0)));
    while (loop(logic_and(math_compare<">">(kern::formula([](Float kMaxPieces, Float a) -> Float { return ((kMaxPieces < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxPieces < a)); }, kern::to<Float>(kMaxPieces), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float floors, Float roof) -> Float { return ((a * floors) + roof); }, kern::to<Float>(list_length<Destruction_fracture_Seg>(segs)), kern::to<Float>(floors), kern::to<Float>(roof)))))), 0.5), math_compare<">">(kern::to<Float>(floors), 1.0)))) {
      floors = kern::to<Int>(kern::formula([](Float floors) -> Float { return kern::math::wrap32(floors - 1.0); }, kern::to<Float>(floors)));
    }
    stride = kern::to<Int>(kern::formula([](Float a, Float kMaxPieces, Float roof) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::ceil(kern::math::div((a < 0.0 ? a + 18446744073709551616.0 : a), kern::math::wrap32(kMaxPieces - roof))))); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float floors) -> Float { return (a * floors); }, kern::to<Float>(list_length<Destruction_fracture_Seg>(segs)), kern::to<Float>(floors))))), kern::to<Float>(kMaxPieces), kern::to<Float>(roof)));
    the_band = kern::formula([](Float removed, Float floors) -> Float { return kern::math::div(removed, floors); }, removed, kern::to<Float>(floors));
    for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(floors)))); it38.next();) {
      i = kern::to<Int>(kern::formula([](Float index, Float stride) -> Float { return kern::math::mod(index, stride); }, kern::to<Float>(n38.index), kern::to<Float>(stride)));
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Destruction_fracture_Seg>(segs))), 0.5))) {
        s = segs[i];
        arg = offset;
        arg_2 = center;
        arg_3 = outward;
        arg_4 = I;
        arg_5 = boost;
        arg_6 = vector_make(vector_break(s.m).x, vector_break(s.m).y, kern::formula([](Float new_top, Float the_band, Float index) -> Float { return (new_top + (the_band * (index + 0.5))); }, b.new_top, the_band, kern::to<Float>(n38.index)));
        n39 = common::Rng_range(rng, 0.85, 1.0);
        n40 = common::Rng_range(rng, 0.8, 1.0);
        arg_7 = vector_make(kern::formula([](Float len, Float the_result) -> Float { return (len * the_result); }, s.len, n39.Result), 0.45, kern::formula([](Float the_band, Float the_result) -> Float { return (the_band * the_result); }, the_band, n40.Result));
        arg_8 = s.yaw;
        n41 = fracture_tint(rng, 0.18);
        Destruction_fracture_launch(self, rng, arg, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, n41.Result);
        i += stride;
      }
    }
    for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(roof)))); it42.next();) {
      arg_9 = offset;
      arg_10 = center;
      arg_11 = outward;
      arg_12 = I;
      arg_13 = boost;
      n43 = common::Rng_range(rng, vector_break(b.lo).x, vector_break(b.hi).x);
      n44 = common::Rng_range(rng, vector_break(b.lo).y, vector_break(b.hi).y);
      arg_14 = vector_make(n43.Result, n44.Result, kern::formula([](Float top) -> Float { return (top - 0.3); }, b.top));
      n45 = common::Rng_range(rng, 3.0, 7.0);
      n46 = common::Rng_range(rng, 3.0, 7.0);
      arg_15 = vector_make(n45.Result, n46.Result, 0.5);
      n47 = common::Rng_unit(rng);
      arg_16 = kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, n47.Result);
      n48 = fracture_tint(rng, 0.25);
      Destruction_fracture_launch(self, rng, arg_9, arg_10, arg_11, arg_12, arg_13, arg_14, arg_15, arg_16, n48.Result);
    }
  }

  void Destruction_free_torn(Destruction& self, Destruction_Torn& t) {
    Bool remove = false;
    if (logic_and(math_compare<"!=">(kern::to<Float>(t.mesh), 0.0), object_valid<kern::Renderer>(self.r_))) {
      engine_renderer_remove(n49, self.r_, t.mesh);
      remove = n49.result;
    }
    t.mesh = 0;
  }

  void Destruction_update(Destruction& self, const CityStream& city, Rng& rng, Float dt = 0.0) {
    Int i = 0;
    Bool skip = false;
    Float k = 0.0;
    Float ease = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool update_instances = false;
    Int i_2 = 0;
    Bool skip_2 = false;
    Float the_floor = 0.0;
    if (math_compare<"<=">(dt, 0.0)) {
      return;
    }
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Destruction_Torn>(self.torn_))), 0.5))) {
      skip = false;
      self.torn_[i].age = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, self.torn_[i].age, dt);
      k = kern::formula([](Float a, Float a_2) -> Float { return kern::math::min(1.0, kern::math::div(a, a_2)); }, self.torn_[i].age, self.torn_[i].break_at);
      ease = kern::formula([](Float k) -> Float { return ((k * k) * (3.0 - (2.0 * k))); }, k);
      self.torn_[i].offset = vector_scale(self.torn_[i].dir, kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::div(((a * a_2) * a_2), (2.0 * a_3)); }, self.torn_[i].speed, self.torn_[i].age, self.torn_[i].break_at));
      self.torn_[i].offset = vector_make(vector_break(self.torn_[i].offset).x, vector_break(self.torn_[i].offset).y, kern::formula([](Float a, Float a_2, Float kGravity) -> Float { return ((a * a_2) - ((((0.5 * kGravity) * a_2) * a_2) * 0.35)); }, self.torn_[i].lift, self.torn_[i].age, kGravity));
      if (math_compare<">=">(self.torn_[i].age, self.torn_[i].break_at)) {
        Destruction_fracture(self, self.torn_[i].b, rng, self.torn_[i].center, self.torn_[i].offset, kern::formula([](Float a) -> Float { return (1.0 + (0.35 * a)); }, self.torn_[i].tilt));
        Destruction_free_torn(self, self.torn_[i]);
        list_remove_at(self.torn_, i);
        skip = true;
      }
      if (logic_not(skip)) {
        a = engine_math_math::Mat4_translate(vector_add(self.torn_[i].pivot, self.torn_[i].offset)).Result;
        a_2 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(self.torn_[i].axis, kern::formula([](Float a, Float ease) -> Float { return (a * ease); }, self.torn_[i].tilt, ease)).Result, Vector{1.0, 1.0, 1.0}).Result;
        m = engine_math_math::Mat4_mul(a, a_2).Result;
        if (object_valid<kern::Renderer>(self.r_)) {
          engine_renderer_update_instances(n50, self.r_, self.torn_[i].mesh, m.m, List<Float>{});
          update_instances = n50.result;
        }
        i += 1;
      }
    }
    i_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Destruction_Piece>(self.pieces_))), 0.5))) {
      skip_2 = false;
      self.pieces_[i_2].age = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, self.pieces_[i_2].age, dt);
      if (math_compare<">">(self.pieces_[i_2].age, kPieceLife)) {
        self.pieces_[i_2] = self.pieces_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_))))];
        list_remove_at(self.pieces_, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_)))));
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_not(self.pieces_[i_2].rest)) {
          self.pieces_[i_2].v = vector_make(vector_break(self.pieces_[i_2].v).x, vector_break(self.pieces_[i_2].v).y, kern::formula([](Float z, Float kGravity, Float dt) -> Float { return (z - (kGravity * dt)); }, vector_break(self.pieces_[i_2].v).z, kGravity, dt));
          self.pieces_[i_2].v = vector_scale(self.pieces_[i_2].v, kern::formula([](Float dt) -> Float { return kern::math::exp(((-0.08) * dt)); }, dt));
          self.pieces_[i_2].p = vector_add(self.pieces_[i_2].p, vector_scale(self.pieces_[i_2].v, dt));
          self.pieces_[i_2].angle = kern::formula([](Float a, Float a_2, Float dt) -> Float { return (a + (a_2 * dt)); }, self.pieces_[i_2].angle, self.pieces_[i_2].spin, dt);
          the_floor = kern::formula([](Float the_result, Float z) -> Float { return (the_result + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city, vector_break(self.pieces_[i_2].p).x, vector_break(self.pieces_[i_2].p).y).Result, vector_break(self.pieces_[i_2].size).z);
          if (math_compare<"<">(vector_break(self.pieces_[i_2].p).z, the_floor)) {
            self.pieces_[i_2].p = vector_make(vector_break(self.pieces_[i_2].p).x, vector_break(self.pieces_[i_2].p).y, the_floor);
            if (logic_and(math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(self.pieces_[i_2].v).z), 2.2), math_compare<"<">(kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(self.pieces_[i_2].v).x, vector_break(self.pieces_[i_2].v).y), 1.5))) {
              self.pieces_[i_2].rest = true;
              self.pieces_[i_2].v = Vector{0.0, 0.0, 0.0};
              self.pieces_[i_2].spin = 0.0;
            } else {
              self.pieces_[i_2].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.55); }, vector_break(self.pieces_[i_2].v).x), kern::formula([](Float y) -> Float { return (y * 0.55); }, vector_break(self.pieces_[i_2].v).y), kern::formula([](Float z) -> Float { return ((-z) * 0.28); }, vector_break(self.pieces_[i_2].v).z));
              self.pieces_[i_2].spin = kern::formula([](Float a) -> Float { return (a * 0.5); }, self.pieces_[i_2].spin);
            }
          }
        }
        i_2 += 1;
      }
    }
    self.stats_.pieces = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_))));
    self.stats_.torn = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Destruction_Torn>(self.torn_))));
  }

  void Destruction_draw(const Destruction& self, CombatArt& art) {
    Float sink = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it51 = flow_for(n51, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_))))); it51.next();) {
      sink = kern::formula([](Float a, Float kPieceLife, Float z) -> Float { return ((a > (kPieceLife - 3.0)) ? (kern::math::div((a - (kPieceLife - 3.0)), 3.0) * z) : 0.0); }, self.pieces_[n51.index].age, kPieceLife, vector_break(self.pieces_[n51.index].size).z);
      a = engine_math_math::Quat_axis_angle(self.pieces_[n51.index].axis, self.pieces_[n51.index].angle).Result;
      a_2 = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, self.pieces_[n51.index].yaw).Result;
      q = engine_math_math::Quat_mul(a, a_2).Result;
      a_3 = engine_math_math::Mat4_trs(vector_sub(self.pieces_[n51.index].p, vector_make(0.0, 0.0, sink)), q, self.pieces_[n51.index].size).Result;
      combat_art_cpp::CombatArt_put(art, a_3, CombatArt_Shape::box, self.pieces_[n51.index].tint);
    }
  }

  void Destruction_reset(Destruction& self, CityStream& city) {
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Destruction_Torn>(self.torn_))))); it52.next();) {
      Destruction_free_torn(self, self.torn_[n52.index]);
    }
    list_clear(self.torn_);
    list_clear(self.pieces_);
    map_clear(self.points_);
    list_clear(self.points_log_);
    city_stream_cpp::CityStream_clear_damage(city);
    self.stats_ = struct_make<Destruction_Stats>(0, 0, 0, 0);
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  common::Rng_rangeResult n27;
  common::Rng_rangeResult n28;
  common::Rng_rangeResult n29;
  common::Rng_rangeResult n30;
  common::Rng_rangeResult n31;
  common::Rng_rangeResult n32;
  common::Rng_rangeResult n33;
  common::Rng_rangeResult n34;
  common::Rng_rangeResult n35;
  common::Rng_rangeResult n36;
  common::Rng_unitResult n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  common::Rng_rangeResult n39;
  common::Rng_rangeResult n40;
  fracture_tintResult n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  common::Rng_rangeResult n43;
  common::Rng_rangeResult n44;
  common::Rng_rangeResult n45;
  common::Rng_rangeResult n46;
  common::Rng_unitResult n47;
  fracture_tintResult n48;
  struct { Bool result = false; } n49;
  struct { Bool result = false; Bool changed = false; } n50;
  struct : kern::Node<51> { Int index = 0; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
};
