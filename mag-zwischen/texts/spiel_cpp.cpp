struct spiel_cpp : virtual kern::Library, virtual engine_input_actions, virtual spiel {
  // ---- functions ----
  struct keyResult { Float Result = 0.0; };
  KERN_PURE keyResult key(Object<kern::Input> in = Object<kern::Input>{}, Key k = Key::unknown) {
    Bool down = false;
    down = engine_input_down(in, k);
    return {kern::formula([](Float down) -> Float { return ((down != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(down))};
  }

  struct json_escapeResult { Text Result = ""; };
  KERN_PURE json_escapeResult json_escape(const Text& s) {
    Text o = "";
    Bool skip = false;
    Int c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        o = text_append(o, text_of_code(92), "");
      }
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {o};
  }

  struct pad_downResult { Bool Result = false; };
  KERN_PURE pad_downResult pad_down(const PadState& p, I32 b = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float buttons, Float b) -> Float { return kern::math::band(buttons, kern::math::wrap32(kern::math::shl(1.0, b))); }, kern::to<Float>(p.buttons), kern::to<Float>(b))))), 0.0)};
  }

  struct pad_hitResult { Bool Result = false; };
  KERN_PURE pad_hitResult pad_hit(const PadState& p, I32 b = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pressed, Float b) -> Float { return kern::math::band(pressed, kern::math::wrap32(kern::math::shl(1.0, b))); }, kern::to<Float>(p.pressed), kern::to<Float>(b))))), 0.0)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
