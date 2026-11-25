struct lamps : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(lamps_Field_BlastHit) { I32 lamp = 0; U8 state = 0; Vector dir = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; };
  KERN_STRUCT(lamps_Field_CarHit) { I32 lamp = -1; Bool topple = false; Vector impulse = Vector{0.0, 0.0, 0.0}; Bool clang = false; };
  KERN_STRUCT(lamps_Field_Spot) { Float x = 0.0; Float y = 0.0; Float yaw = 0.0; Float z = 0.0; };
  KERN_STRUCT(lamps_Lamp) { Vector foot = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Vector head = Vector{0.0, 0.0, 0.0}; U8 state = 0; Int64 key = Int64{0x0}; U32 lane = 0; };
  KERN_STRUCT(lamps_Field) { List<lamps_Lamp> lamps_ = List<lamps_Lamp>{}; Set<Int64> seen_ = Set<Int64>{}; Map<Int64, Int> broken_ = Map<Int64, Int>{}; Map<Int64, List<I32>> grid_ = Map<Int64, List<I32>>{}; };

  // ---- variables ----
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kSpacing = 34.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kGridM = 10.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kKerb = 1.15;
  KERN_CONST Float kPoleMass = 140.0;
  KERN_CONST U32 kFallenMax = 14;
  KERN_CONST U32 kMax = 900;

  // ---- functions ----
  void Field_clear(lamps_Field& self) {
    list_clear(self.lamps_);
    set_clear(self.seen_);
    map_clear(self.grid_);
  }

  void Field_reset_broken(lamps_Field& self) {
    map_clear(self.broken_);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))); it10.next();) {
      self.lamps_[n10.index].state = 0;
    }
  }

  struct Field_brokenResult { Int Result = 0; };
  KERN_PURE Field_brokenResult Field_broken(const lamps_Field& self) {
    return {map_length<Int64, Int>(self.broken_)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
