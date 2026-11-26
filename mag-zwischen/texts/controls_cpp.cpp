struct controls_cpp : virtual kern::Library, virtual blueprint_call, virtual controls {
  // ---- types ----
  KERN_STRUCT(controls_Cook) { I32 carry = 0; Float regen_at = 0.0; Bool cooking = false; Float at = 0.0; Float power = 0.0; I32 source = 0; };
  KERN_STRUCT(controls_Look) { Float yaw = 0.0; Float pitch = 0.0; Bool snapping = false; Float idle = 0.0; Bool owned = false; };

  // ---- functions ----
  struct look_ofResult { controls_Look Result = controls_Look{0.0, 0.0, false, 0.0, false}; };
  KERN_PURE look_ofResult look_of(const controls_WorldLook& l) {
    return {struct_make<controls_Look>(blueprint_call::decimal(l.yaw_deg).Result, blueprint_call::decimal(l.pitch_deg).Result, l.snapping, blueprint_call::decimal(l.idle).Result, l.owned)};
  }

  void take(const controls_Look& v, controls_WorldLook& l) {
    l.yaw_deg = v.yaw;
    l.pitch_deg = v.pitch;
    l.snapping = v.snapping;
    l.idle = v.idle;
    l.owned = v.owned;
  }

  struct cook_ofResult { controls_Cook Result = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; };
  KERN_PURE cook_ofResult cook_of(const controls_GrenadeCook& g) {
    return {struct_make<controls_Cook>(g.carry, blueprint_call::decimal(g.regen_at).Result, g.cooking, blueprint_call::decimal(g.at).Result, blueprint_call::decimal(g.power).Result, g.source)};
  }

  void take_2(const controls_Cook& c, controls_GrenadeCook& g) {
    g.carry = c.carry;
    g.regen_at = c.regen_at;
    g.cooking = c.cooking;
    g.at = c.at;
    g.power = c.power;
    g.source = c.source;
  }
};
