struct lamps : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(lamps_Lamp) { Vector foot = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Vector head = Vector{0.0, 0.0, 0.0}; Int state = 0; Int64 key = Int64{0x0}; Int lane = 0; };
  KERN_STRUCT(lamps_Field) { List<lamps_Lamp> lamps_ = List<lamps_Lamp>{}; Set<Int64> seen_ = Set<Int64>{}; Map<Int64, Int> broken_ = Map<Int64, Int>{}; Map<Int64, List<Int>> grid_ = Map<Int64, List<Int>>{}; };

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
