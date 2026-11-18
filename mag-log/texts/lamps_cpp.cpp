struct lamps_cpp : virtual kern::Library, virtual lamps, virtual shapes {
  // ---- variables ----
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kGridM = 10.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kSpacing = 34.0;
  KERN_CONST Int kMax = 900;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kKerb = 1.15;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;

  // ---- functions ----
  struct grid_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE grid_keyResult grid_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float kGridM) -> Float { return kern::math::floor(kern::math::div(x, kGridM)); }, x, kGridM))), 32), int64_and(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float kGridM) -> Float { return kern::math::floor(kern::math::div(y, kGridM)); }, y, kGridM))), Int64{0xffffffff}))};
  }

  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(kern::math::div(x, 8.0)); }, x))), 32), int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(kern::math::div(y, 8.0)); }, y))), Int64{0xffffffff}))};
  }

  void Field_place_road(lamps_Field& self, const List<Vector>& pts, Float width = 0.0, Int lane = 0, Float ground = 0.0) {
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
          set_add(n10, self.seen_, key_2);
          if (logic_not(struct_break<Int_Bool_pair>(struct_make<Int_Bool_pair>(n10.index, n10.added)).second)) {
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
            map_add(n11, self.grid_, key_3);
            put = t_2;
            t_2 = self.grid_[n11.index];
            list_add(t_2, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)))));
            self.grid_[n11.index] = put;
            list_add(self.lamps_, lp);
          }
          t += kSpacing;
        }
        carry = kern::formula([](Float carry, Float l, Float kSpacing) -> Float { return kern::math::mod((kern::math::mod((carry - l), kSpacing) + kSpacing), kSpacing); }, carry, l, kSpacing);
      }
      i += 1;
    }
  }

  struct Field_nearResult { List<Int> Result = List<Int>{}; };
  KERN_PURE Field_nearResult Field_near(const lamps_Field& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    List<Int> out = List<Int>{};
    Int64 g0 = Int64{0x0};
    Int64 g1 = Int64{0x0};
    Int64 h0 = Int64{0x0};
    Int64 h1 = Int64{0x0};
    Int64 i = Int64{0x0};
    Int64 j = Int64{0x0};
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    Int k = 0;
    out = List<Int>{};
    g0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((x - r), kGridM)); }, x, r, kGridM)));
    g1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((x + r), kGridM)); }, x, r, kGridM)));
    h0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((y - r), kGridM)); }, y, r, kGridM)));
    h1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div((y + r), kGridM)); }, y, r, kGridM)));
    i = g0;
    while (loop(logic_not(int64_less(g1, i)))) {
      j = h0;
      while (loop(logic_not(int64_less(h1, j)))) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.grid_, int64_xor(int64_shift_left(i, 32), int64_and(j, Int64{0xffffffff}))).index), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.grid_[at]))))); it14.next();) {
            k = self.grid_[at][n14.index];
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

  void Field_set(lamps_Field& self, Int i = 0, Int state = 0) {
    Int put = 0;
    Int64 key = Int64{0x0};
    self.lamps_[i].state = state;
    put = state;
    key = self.lamps_[i].key;
    map_add(n15, self.broken_, key);
    self.broken_[n15.index] = put;
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
    for (auto it16 = flow_for(n16, first, last); it16.next();) {
      list_add(made, struct_make<Int64_Int_pair>(map_key_at<Int64, Int>(self.broken_, n16.index), self.broken_[n16.index]));
    }
    out = made;
    list_sort<&lamps_cpp::broken_list_match>(*this, out);
    return {out};
  }

  // ---- what nodes give and keep ----
  struct { Bool added = false; Int index = 0; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct { Bool added = false; Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
};
