struct lamps_cpp : virtual kern::Library, virtual lamps, virtual shapes {
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

  // ---- functions ----
  struct Lamps_WorseResult { Bool worse = false; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("Worse") Lamps_WorseResult Lamps_Worse(Int state = 0, Int to = 0);

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
    List<Int> t_2 = List<Int>{};
    Int64 key_3 = Int64{0x0};
    List<Int> put = List<Int>{};
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
            br = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.broken_, key).index), kern::to<Float>(map_length<Int64, Int>(self.broken_))));
            if (logic_not(math_compare<"==">(kern::to<Float>(br), kern::to<Float>(map_length<Int64, Int>(self.broken_))))) {
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
    List<Int> t = List<Int>{};
    Int64 key_2 = Int64{0x0};
    List<Int> put = List<Int>{};
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
        br = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.broken_, lp.key).index), kern::to<Float>(map_length<Int64, Int>(self.broken_))));
        if (logic_not(math_compare<"==">(kern::to<Float>(br), kern::to<Float>(map_length<Int64, Int>(self.broken_))))) {
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

  void Field_set(lamps_Field& self, I32 i = 0, U8 state = 0) {
    Int put = 0;
    Int64 key = Int64{0x0};
    self.lamps_[i].state = state;
    put = state;
    key = self.lamps_[i].key;
    map_add(n18, self.broken_, key);
    self.broken_[n18.index] = put;
  }

  struct Field_set_stateResult { Bool Result = false; };
  Field_set_stateResult Field_set_state(lamps_Field& self, I32 i = 0, U8 state = 0) {
    if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))))))) {
      return {false};
    }
    n17 = worse(self.lamps_[i].state, state);
    if (logic_not(n17.Result)) {
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
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Int>(self.broken_))));
    list_reserve(made, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
    for (auto it19 = flow_for(n19, first, last); it19.next();) {
      list_add(made, struct_make<Int64_Int_pair>(map_key_at<Int64, Int>(self.broken_, n19.index), self.broken_[n19.index]));
    }
    out = made;
    list_sort<&lamps_cpp::broken_list_match>(*this, out);
    return {out};
  }

  struct Field_merge_brokenResult { Bool Result = false; };
  Field_merge_brokenResult Field_merge_broken(lamps_Field& self, Int64 key = Int64{0x0}, U8 state = 0) {
    Int was = 0;
    Int put = 0;
    Int64 key_2 = Int64{0x0};
    Bool both = false;
    was = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.broken_, key).index), kern::to<Float>(map_length<Int64, Int>(self.broken_))));
    n20 = worse(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? 0.0 : a_2), 255.0); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(was), kern::to<Float>(map_length<Int64, Int>(self.broken_)))), kern::to<Float>(self.broken_[was]))), state);
    if (logic_not(n20.Result)) {
      return {false};
    }
    put = state;
    key_2 = key;
    map_add(n21, self.broken_, key_2);
    self.broken_[n21.index] = put;
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))); it22.next();) {
      both = int64_equal(self.lamps_[n22.index].key, key);
      if (both) {
        n24 = worse(self.lamps_[n22.index].state, state);
        both = n24.Result;
      }
      if (both) {
        self.lamps_[n22.index].state = state;
      }
    }
    return {true};
  }

  // ---- what nodes give and keep ----
  struct { Bool worse = false; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct { Bool added = false; Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct { Bool added = false; Int index = 0; } n14;
  struct { Bool added = false; Int index = 0; } n15;
  worseResult n17;
  struct { Bool added = false; Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  worseResult n20;
  struct { Bool added = false; Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  worseResult n24;
};
