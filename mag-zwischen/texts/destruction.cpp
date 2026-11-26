struct destruction : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_STRUCT(Destruction_Piece) { Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Vector size = Vector{0.0, 0.0, 0.0}; Vector axis = Vector{0.0, 0.0, 0.0}; Vector tint = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float angle = 0.0; Float spin = 0.0; Float age = 0.0; Bool rest = false; };
  KERN_STRUCT(Destruction_Stats) { U32 pieces = 0; U32 torn = 0; U32 hits = 0; U32 collapsed = 0; };
  KERN_STRUCT(Destruction_Broken) { List<Vector_row> rings = List<Vector_row>{}; Vector c = Vector{0.0, 0.0, 0.0}; Vector lo = Vector{0.0, 0.0, 0.0}; Vector hi = Vector{0.0, 0.0, 0.0}; Float new_top = 0.0; Float top = 0.0; Float intensity = 0.0; };
  KERN_STRUCT(Destruction_Torn) { U32 mesh = 0; Destruction_Broken b = Destruction_Broken{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0}; Vector center = Vector{0.0, 0.0, 0.0}; Vector dir = Vector{0.0, 0.0, 0.0}; Vector axis = Vector{0.0, 0.0, 0.0}; Vector pivot = Vector{0.0, 0.0, 0.0}; Vector offset = Vector{0.0, 0.0, 0.0}; Float tilt = 0.0; Float speed = 0.0; Float lift = 0.0; Float break_at = 1.0; Float age = 0.0; };
  KERN_STRUCT(Destruction) { Int r_ = 0; Map<Int, Float> points_ = Map<Int, Float>{}; List<Int_Float_pair> points_log_ = List<Int_Float_pair>{}; List<Destruction_Piece> pieces_ = List<Destruction_Piece>{}; List<Destruction_Torn> torn_ = List<Destruction_Torn>{}; Destruction_Stats stats_ = Destruction_Stats{0, 0, 0, 0}; };

  // ---- variables ----
  KERN_CONST U32 kPieces = 2000;
  KERN_CONST U32 kTorn = 40;
  KERN_CONST Float kFloor = 3.2;
  KERN_CONST Float kRubble = 0.8;
  KERN_CONST Float kPieceLife = 22.0;

  // ---- functions ----
  void Destruction_init(Destruction& self, Int r = 0) {
    self.r_ = r;
  }

  struct Destruction_take_points_logResult { List<Int_Float_pair> Result = List<Int_Float_pair>{}; };
  /// Destruction sync: the structural points left per building (seed) - only ever fewer. The points a blast here took (since the last call), and those another machine reports.
  Destruction_take_points_logResult Destruction_take_points_log(Destruction& self) {
    var_take(n10, self.points_log_);
    return {n10.value};
  }

  struct points_list_matchResult { Bool Result = false; };
  KERN_PURE points_list_matchResult points_list_match(const Int_Float_pair& a, const Int_Float_pair& b) {
    return {logic_or(math_compare<"<">(kern::to<Float>(a.first), kern::to<Float>(b.first)), logic_and(logic_not(math_compare<"<">(kern::to<Float>(b.first), kern::to<Float>(a.first))), math_compare<"<">(a.second, b.second)))};
  }

  void the_Destruction_points_list_(const Destruction& self, List<Int_Float_pair>& out) {
    list_clear(out);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Float>(self.points_))))); it11.next();) {
      list_add(out, struct_make<Int_Float_pair>(map_key_at<Int, Float>(self.points_, n11.index), self.points_[n11.index]));
    }
    list_sort<&destruction::points_list_match>(*this, out);
  }

  // ---- what nodes give and keep ----
  struct { List<Int_Float_pair> value = List<Int_Float_pair>{}; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
