struct controls_cpp : virtual kern::Library, virtual blueprint_call, virtual controls {
  // ---- types ----
  KERN_STRUCT(controls_Cook) { i32 carry = 0; f64 regen_at = 0.0; bool cooking = false; f64 at = 0.0; f64 power = 0.0; i32 source = 0; };
  KERN_STRUCT(controls_Look) { f64 yaw = 0.0; f64 pitch = 0.0; bool snapping = false; f64 idle = 0.0; bool owned = false; };

  // ---- functions ----
  struct look_ofResult { controls_Look Result = controls_Look{0.0, 0.0, false, 0.0, false}; };
  KERN_PURE look_ofResult look_of(const controls_WorldLook& l) {
    return {struct_make<controls_Look>(blueprint_call::decimal(l.yaw_deg).Result, blueprint_call::decimal(l.pitch_deg).Result, l.snapping, blueprint_call::decimal(l.idle).Result, l.owned)};
  }

  void take(const controls_Look& v, controls_WorldLook& l) {
    l.yaw_deg = num_cast<f32, f64>(v.yaw);
    l.pitch_deg = num_cast<f32, f64>(v.pitch);
    l.snapping = v.snapping;
    l.idle = num_cast<f32, f64>(v.idle);
    l.owned = v.owned;
  }

  struct cook_ofResult { controls_Cook Result = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; };
  KERN_PURE cook_ofResult cook_of(const controls_GrenadeCook& g) {
    return {struct_make<controls_Cook>(g.carry, blueprint_call::decimal(g.regen_at).Result, g.cooking, blueprint_call::decimal(g.at).Result, blueprint_call::decimal(g.power).Result, g.source)};
  }

  void take_2(const controls_Cook& c, controls_GrenadeCook& g) {
    g.carry = c.carry;
    g.regen_at = num_cast<f32, f64>(c.regen_at);
    g.cooking = c.cooking;
    g.at = num_cast<f32, f64>(c.at);
    g.power = num_cast<f32, f64>(c.power);
    g.source = c.source;
  }
};
