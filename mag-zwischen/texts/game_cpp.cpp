struct game_cpp : virtual kern::Library, virtual game {
  // ---- variables ----
  KERN_CONST Fixed<Vector, 10> c = List<Vector>{Vector{0.62, 0.05, 0.04}, Vector{0.08, 0.16, 0.42}, Vector{0.85, 0.85, 0.86}, Vector{0.06, 0.06, 0.07}, Vector{0.45, 0.47, 0.5}, Vector{0.12, 0.32, 0.18}, Vector{0.9, 0.62, 0.08}, Vector{0.3, 0.5, 0.75}, Vector{0.55, 0.36, 0.22}, Vector{0.7, 0.7, 0.66}};

  // ---- functions ----
  struct car_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE car_colorResult car_color(U32 i = 0) {
    return {list_get<Vector>(c, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 10.0); }, kern::to<Float>(i))))};
  }

  struct json_escapeResult { Text Result = ""; };
  KERN_PURE json_escapeResult json_escape(const Text& s) {
    Text o = "";
    Bool skip = false;
    Int c_2 = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(c_2), 34.0), math_compare<"==">(kern::to<Float>(c_2), 92.0))) {
        o = text_append(o, text_of_code(92), "");
      }
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c_2) -> Float { return kern::math::band(c_2, 255.0); }, kern::to<Float>(c_2))))), 32.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        o = text_append(o, text_of_code(c_2), "");
      }
    }
    return {o};
  }

  struct mode_nameResult { Text Result = ""; };
  KERN_PURE mode_nameResult mode_name(Mode m = Mode::foot) {
    switch (flow_switch_enum<Mode>(m)) {
      case Mode::foot: {
        return {"foot"};
      } break;
      case Mode::drone: {
        return {"drone"};
      } break;
      case Mode::car: {
        return {"car"};
      } break;
      case Mode::jet: {
        return {"jet"};
      } break;
      case Mode::parachute: {
        return {"parachute"};
      } break;
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
