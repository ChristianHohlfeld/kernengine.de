struct game_life_cpp : virtual kern::Library {
  // ---- functions ----
  struct json_escapeResult { Text Result = ""; };
  KERN_PURE json_escapeResult json_escape(const Text& s) {
    Text o = "";
    Int c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        o = text_append(o, text_of_code(92), "");
        o = text_append(o, text_of_code(c), "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        o = text_append(o, text_of_code(32), "");
      } else {
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {o};
  }

  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
