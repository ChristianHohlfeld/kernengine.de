struct lamps_cpp : virtual kern::Library, virtual blueprint_call, virtual lamps, virtual shapes {
  // ---- variables ----
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kGridM = 10.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kSpacing = 34.0;
  KERN_CONST U32 kMax = 900;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kKerb = 1.15;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;

  // ---- functions ----
  struct Lamps_WorseResult { Bool worse = false; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("Worse") Lamps_WorseResult Lamps_Worse(Int state = 0, Int to = 0);

  struct Lamps_ShotAtResult { Bool hit = false; Float along = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("ShotAt") Lamps_ShotAtResult Lamps_ShotAt(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector head = Vector{0.0, 0.0, 0.0}, Int state = 0);

  struct Lamps_BlastReachResult { Float reach = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("BlastReach") Lamps_BlastReachResult Lamps_BlastReach(Float radius = 0.0);

  struct Lamps_BlastFateResult { Int state = 0; Vector push = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("BlastFate") Lamps_BlastFateResult Lamps_BlastFate(Vector foot = Vector{0.0, 0.0, 0.0}, Vector at = Vector{0.0, 0.0, 0.0}, Float reach = 0.0);

  struct Lamps_CarFateResult { Bool hit = false; Bool topple = false; Vector impulse = Vector{0.0, 0.0, 0.0}; Bool clang = false; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("CarFate") Lamps_CarFateResult Lamps_CarFate(Vector foot = Vector{0.0, 0.0, 0.0}, Int state = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector velocity = Vector{0.0, 0.0, 0.0}, Float halfLength = 0.0, Float halfWidth = 0.0, Float mass = 0.0);

  struct worseResult { Bool Result = false; };
  worseResult worse(U8 state = 0, U8 to = 0) {
    Bool worse_2 = false;
    worse_2 = false;
    func_call_on<"Lamps_Worse">(actor_find("Lamps"), kern::out(n10.worse), state, to);
    worse_2 = n10.worse;
    return {worse_2};
  }

  struct grid_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE grid_keyResult grid_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float kGridM) -> Float { return kern::math::floor(kern::math::div(x, kGridM)); }, x, kGridM))), 32), int64_and(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float kGridM) -> Float { return kern::math::floor(kern::math::div(y, kGridM)); }, y, kGridM))), Int64{0xffffffff}))};
  }

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("Lamps"))};
  }

  struct pole_radiusResult { Float Result = 0.0; };
  KERN_PURE pole_radiusResult pole_radius() {
    return {func_get_on<"PoleRadius", Float>(actor_find("Lamps"))};
  }

  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(kern::math::div(x, 8.0)); }, x))), 32), int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(kern::math::div(y, 8.0)); }, y))), Int64{0xffffffff}))};
  }

  void Field_place_road(lamps_Field& self, const List<Vector>& pts, Float width = 0.0, U32 lane = 0, Float ground = 0.0) {
    Float carry = 0.0;
    Float side = 0.0;
    Int i = 0;
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float l = 0.0;
    Float ux = 0.0;
    Float uy = 0.0;
    Float nx = 0.0;
    Float ny = 0.0;
    Float t = 0.0;
    Bool skip_2 = false;
    Float off = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    lamps_Lamp lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
    Int br = 0;
    List<I32> t_2 = List<I32>{};
    Int64 key_3 = Int64{0x0};
    List<I32> put = List<I32>{};
    carry = kern::formula([](Float kSpacing) -> Float { return (kSpacing * 0.5); }, kSpacing);
    side = 1.0;
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(pts))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float kMax) -> Float { return kern::math::mul32(kMax, 8.0); }, kern::to<Float>(kMax)))))), 0.5)))) {
      skip = false;
      a = pts[i];
      b = pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
      dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(b).x, vector_break(a).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(b).y, vector_break(a).y);
      l = kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt((dx * dx) + (dy * dy)); }, dx, dy);
      if (math_compare<"<">(l, 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        ux = kern::formula([](Float dx, Float l) -> Float { return kern::math::div(dx, l); }, dx, l);
        uy = kern::formula([](Float dy, Float l) -> Float { return kern::math::div(dy, l); }, dy, l);
        nx = kern::formula([](Float uy) -> Float { return (-uy); }, uy);
        ny = ux;
        t = carry;
        while (loop(math_compare<"<">(t, l))) {
          skip_2 = false;
          side = kern::formula([](Float side) -> Float { return (-side); }, side);
          off = kern::formula([](Float width, Float kKerb) -> Float { return (kern::math::div(width, 2.0) + kKerb); }, width, kKerb);
          x = kern::formula([](Float x, Float ux, Float t, Float nx, Float off, Float side) -> Float { return ((x + (ux * t)) + ((nx * off) * side)); }, vector_break(a).x, ux, t, nx, off, side);
          y = kern::formula([](Float y, Float uy, Float t, Float ny, Float off, Float side) -> Float { return ((y + (uy * t)) + ((ny * off) * side)); }, vector_break(a).y, uy, t, ny, off, side);
          key = cell_key(x, y).Result;
          key_2 = key;
          set_add(n11, self.seen_, key_2);
          if (logic_not(struct_break<Int_Bool_pair>(struct_make<Int_Bool_pair>(n11.index, n11.added)).second)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
            lp.foot = vector_make(x, y, ground);
            lp.yaw = kern::formula([](Float ny, Float side, Float nx) -> Float { return kern::math::atan2(((-ny) * side), ((-nx) * side)); }, ny, side, nx);
            lp.head = vector_make(kern::formula([](Float x, Float yaw, Float kArm) -> Float { return (x + (kern::math::cos(yaw) * kArm)); }, x, lp.yaw, kArm), kern::formula([](Float y, Float yaw, Float kArm) -> Float { return (y + (kern::math::sin(yaw) * kArm)); }, y, lp.yaw, kArm), 0.0);
            lp.key = key;
            lp.lane = lane;
            br = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, U8>(self.broken_, key).index), kern::to<Float>(map_length<Int64, U8>(self.broken_))));
            if (logic_not(math_compare<"==">(kern::to<Float>(br), kern::to<Float>(map_length<Int64, U8>(self.broken_))))) {
              lp.state = self.broken_[br];
            }
            key_3 = grid_key(x, y).Result;
            map_add(n12, self.grid_, key_3);
            t_2 = self.grid_[n12.index];
            list_add(t_2, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)))));
            put = t_2;
            self.grid_[n12.index] = put;
            list_add(self.lamps_, lp);
          }
          t += kSpacing;
        }
        carry = kern::formula([](Float carry, Float l, Float kSpacing) -> Float { return kern::math::mod((kern::math::mod((carry - l), kSpacing) + kSpacing), kSpacing); }, carry, l, kSpacing);
      }
      i += 1;
    }
  }

  void Field_rebuild(lamps_Field& self, const List<lamps_Field_Spot>& spots) {
    Bool skip = false;
    lamps_Lamp lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
    Int64 key = Int64{0x0};
    Int br = 0;
    List<I32> t = List<I32>{};
    Int64 key_2 = Int64{0x0};
    List<I32> put = List<I32>{};
    lamps::Field_clear(self);
    list_reserve(self.lamps_, list_length<lamps_Field_Spot>(spots));
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Field_Spot>(spots))))); it13.next();) {
      skip = false;
      lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
      lp.foot = vector_make(spots[n13.index].x, spots[n13.index].y, spots[n13.index].z);
      lp.yaw = spots[n13.index].yaw;
      lp.head = vector_make(kern::formula([](Float a, Float a_2, Float kArm) -> Float { return (a + (kern::math::cos(a_2) * kArm)); }, spots[n13.index].x, spots[n13.index].yaw, kArm), kern::formula([](Float a, Float a_2, Float kArm) -> Float { return (a + (kern::math::sin(a_2) * kArm)); }, spots[n13.index].y, spots[n13.index].yaw, kArm), 0.0);
      lp.key = cell_key(spots[n13.index].x, spots[n13.index].y).Result;
      key = lp.key;
      set_add(n14, self.seen_, key);
      if (logic_not(struct_break<Int_Bool_pair>(struct_make<Int_Bool_pair>(n14.index, n14.added)).second)) {
        skip = true;
      }
      if (logic_not(skip)) {
        br = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, U8>(self.broken_, lp.key).index), kern::to<Float>(map_length<Int64, U8>(self.broken_))));
        if (logic_not(math_compare<"==">(kern::to<Float>(br), kern::to<Float>(map_length<Int64, U8>(self.broken_))))) {
          lp.state = self.broken_[br];
        }
        key_2 = grid_key(spots[n13.index].x, spots[n13.index].y).Result;
        map_add(n15, self.grid_, key_2);
        t = self.grid_[n15.index];
        list_add(t, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)))));
        put = t;
        self.grid_[n15.index] = put;
        list_add(self.lamps_, lp);
      }
    }
  }

  struct Field_nearResult { List<I32> Result = List<I32>{}; };
  KERN_PURE Field_nearResult Field_near(const lamps_Field& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    List<I32> out = List<I32>{};
    Int64 g0 = Int64{0x0};
    Int64 g1 = Int64{0x0};
    Int64 h0 = Int64{0x0};
    Int64 h1 = Int64{0x0};
    Int64 i = Int64{0x0};
    Int64 j = Int64{0x0};
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    I32 k = 0;
    out = List<I32>{};
    g0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((x - r), kGridM)); }, x, r, kGridM)));
    g1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((x + r), kGridM)); }, x, r, kGridM)));
    h0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((y - r), kGridM)); }, y, r, kGridM)));
    h1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((y + r), kGridM)); }, y, r, kGridM)));
    i = g0;
    while (loop(logic_not(int64_less(g1, i)))) {
      j = h0;
      while (loop(logic_not(int64_less(h1, j)))) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<I32>>(self.grid_, int64_xor(int64_shift_left(i, 32), int64_and(j, Int64{0xffffffff}))).index), kern::to<Float>(map_length<Int64, List<I32>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<I32>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(self.grid_[at]))))); it17.next();) {
            k = self.grid_[at][n17.index];
            if (math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.lamps_[k].foot).x, x, vector_break(self.lamps_[k].foot).y, y), r)) {
              list_add(out, k);
            }
          }
        }
        j = int64_add(j, Int64{0x1});
      }
      i = int64_add(i, Int64{0x1});
    }
    return {out};
  }

  void Field_set(lamps_Field& self, I32 i = 0, U8 state = 0) {
    U8 put = 0;
    Int64 key = Int64{0x0};
    self.lamps_[i].state = state;
    put = state;
    key = self.lamps_[i].key;
    map_add(n19, self.broken_, key);
    self.broken_[n19.index] = put;
  }

  struct Field_set_stateResult { Bool Result = false; };
  Field_set_stateResult Field_set_state(lamps_Field& self, I32 i = 0, U8 state = 0) {
    if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))))))) {
      return {false};
    }
    n18 = worse(self.lamps_[i].state, state);
    if (logic_not(n18.Result)) {
      return {false};
    }
    Field_set(self, i, state);
    return {true};
  }

  struct broken_list_matchResult { Bool Result = false; };
  KERN_PURE broken_list_matchResult broken_list_match(const Int64_Int_pair& a, const Int64_Int_pair& b) {
    return {logic_or(int64_less(a.first, b.first), logic_and(logic_not(int64_less(b.first, a.first)), math_compare<"<">(kern::to<Float>(a.second), kern::to<Float>(b.second))))};
  }

  struct the_Field_broken_list_Result { List<Int64_Int_pair> Result = List<Int64_Int_pair>{}; };
  KERN_PURE the_Field_broken_list_Result the_Field_broken_list_(const lamps_Field& self) {
    List<Int64_Int_pair> out = List<Int64_Int_pair>{};
    List<Int64_Int_pair> made = List<Int64_Int_pair>{};
    Int first = 0;
    Int last = 0;
    made = List<Int64_Int_pair>{};
    first = 0;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, U8>(self.broken_))));
    list_reserve(made, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
    for (auto it20 = flow_for(n20, first, last); it20.next();) {
      list_add(made, struct_make<Int64_Int_pair>(map_key_at<Int64, U8>(self.broken_, n20.index), self.broken_[n20.index]));
    }
    out = made;
    list_sort<&lamps_cpp::broken_list_match>(*this, out);
    return {out};
  }

  struct Field_merge_brokenResult { Bool Result = false; };
  Field_merge_brokenResult Field_merge_broken(lamps_Field& self, Int64 key = Int64{0x0}, U8 state = 0) {
    Int was = 0;
    U8 put = 0;
    Int64 key_2 = Int64{0x0};
    Bool both = false;
    was = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, U8>(self.broken_, key).index), kern::to<Float>(map_length<Int64, U8>(self.broken_))));
    n21 = worse(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? 0.0 : a_2), 255.0); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(was), kern::to<Float>(map_length<Int64, U8>(self.broken_)))), kern::to<Float>(self.broken_[was]))), state);
    if (logic_not(n21.Result)) {
      return {false};
    }
    put = state;
    key_2 = key;
    map_add(n22, self.broken_, key_2);
    self.broken_[n22.index] = put;
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))); it23.next();) {
      both = int64_equal(self.lamps_[n23.index].key, key);
      if (both) {
        n25 = worse(self.lamps_[n23.index].state, state);
        both = n25.Result;
      }
      if (both) {
        self.lamps_[n23.index].state = state;
      }
    }
    return {true};
  }

  struct Field_shotResult { I32 Result = 0; };
  Field_shotResult Field_shot(const lamps_Field& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Float L = 0.0;
    Float mx = 0.0;
    Float my = 0.0;
    I32 best = 0;
    Float first = 0.0;
    List<I32> list = List<I32>{};
    Bool skip = false;
    I32 k = 0;
    Bool hit = false;
    Float along = 0.0;
    L = vector_length(vector_sub(b, a));
    mx = kern::formula([](Float x, Float x_2) -> Float { return kern::math::div((x + x_2), 2.0); }, vector_break(a).x, vector_break(b).x);
    my = kern::formula([](Float y, Float y_2) -> Float { return kern::math::div((y + y_2), 2.0); }, vector_break(a).y, vector_break(b).y);
    best = -1;
    first = 1000000000.0;
    list = Field_near(self, mx, my, kern::formula([](Float l) -> Float { return (kern::math::div(l, 2.0) + 2.0); }, L)).Result;
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(list))))); it26.next();) {
      k = list[n26.index];
      skip = false;
      hit = false;
      along = 0.0;
      func_call_on<"Lamps_ShotAt">(actor_find("Lamps"), kern::out(n27.hit, n27.along), a, b, vector_make(vector_break(self.lamps_[k].head).x, vector_break(self.lamps_[k].head).y, kern::formula([](Float z, Float kHeadZ) -> Float { return (z + kHeadZ); }, vector_break(self.lamps_[k].foot).z, kHeadZ)), self.lamps_[k].state);
      hit = n27.hit;
      along = n27.along;
      if (logic_not(hit)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(along, first)) {
          first = along;
          best = k;
        }
      }
    }
    return {best};
  }

  struct Field_blastResult { List<lamps_Field_BlastHit> Result = List<lamps_Field_BlastHit>{}; };
  Field_blastResult Field_blast(const lamps_Field& self, Vector p = Vector{0.0, 0.0, 0.0}, Float radius = 0.0) {
    List<lamps_Field_BlastHit> out = List<lamps_Field_BlastHit>{};
    Float blast_reach = 0.0;
    Float reach = 0.0;
    List<I32> list = List<I32>{};
    I32 k = 0;
    U8 state = 0;
    Vector push = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    out = List<lamps_Field_BlastHit>{};
    blast_reach = 0.0;
    func_call_on<"Lamps_BlastReach">(actor_find("Lamps"), kern::out(n28.reach), blueprint_call::decimal(radius).Result);
    blast_reach = n28.reach;
    reach = blast_reach;
    list = Field_near(self, vector_break(p).x, vector_break(p).y, reach).Result;
    for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(list))))); it29.next();) {
      k = list[n29.index];
      state = 0;
      push = Vector{0.0, 0.0, 0.0};
      speed = 0.0;
      func_call_on<"Lamps_BlastFate">(actor_find("Lamps"), kern::out(n30.state, n30.push, n30.speed), self.lamps_[k].foot, p, blueprint_call::decimal(reach).Result);
      state = n30.state;
      push = n30.push;
      speed = n30.speed;
      list_add(out, struct_make<lamps_Field_BlastHit>(k, state, vector_make(vector_break(push).x, vector_break(push).y, 0.0), speed));
    }
    return {out};
  }

  struct Field_carResult { List<lamps_Field_CarHit> Result = List<lamps_Field_CarHit>{}; };
  Field_carResult Field_car(const lamps_Field& self, Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector v = Vector{0.0, 0.0, 0.0}, Float hl = 0.0, Float hw = 0.0, Float m = 0.0) {
    List<lamps_Field_CarHit> out = List<lamps_Field_CarHit>{};
    List<I32> list = List<I32>{};
    Bool skip = false;
    I32 k = 0;
    Bool hit = false;
    lamps_Field_CarHit h = lamps_Field_CarHit{-1, false, Vector{0.0, 0.0, 0.0}, false};
    out = List<lamps_Field_CarHit>{};
    list = Field_near(self, vector_break(pos).x, vector_break(pos).y, kern::formula([](Float hl) -> Float { return (hl + 1.0); }, hl)).Result;
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(list))))); it31.next();) {
      k = list[n31.index];
      skip = false;
      hit = false;
      h = lamps_Field_CarHit{-1, false, Vector{0.0, 0.0, 0.0}, false};
      func_call_on<"Lamps_CarFate">(actor_find("Lamps"), kern::out(n32.hit, n32.topple, n32.impulse, n32.clang), self.lamps_[k].foot, self.lamps_[k].state, pos, blueprint_call::decimal(yaw).Result, v, blueprint_call::decimal(hl).Result, blueprint_call::decimal(hw).Result, blueprint_call::decimal(m).Result);
      hit = n32.hit;
      h.topple = n32.topple;
      h.impulse = n32.impulse;
      h.clang = n32.clang;
      if (logic_not(hit)) {
        skip = true;
      }
      if (logic_not(skip)) {
        h.lamp = k;
        list_add(out, h);
      }
    }
    return {out};
  }

  // ---- what nodes give and keep ----
  struct { Bool worse = false; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct { Bool added = false; Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct { Bool added = false; Int index = 0; } n14;
  struct { Bool added = false; Int index = 0; } n15;
  struct : kern::Node<17> { Int index = 0; } n17;
  worseResult n18;
  struct { Bool added = false; Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  worseResult n21;
  struct { Bool added = false; Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  worseResult n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct { Bool hit = false; Float along = 0.0; } n27;
  struct { Float reach = 0.0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct { Int state = 0; Vector push = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct { Bool hit = false; Bool topple = false; Vector impulse = Vector{0.0, 0.0, 0.0}; Bool clang = false; } n32;
};
