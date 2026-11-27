struct spiel_cpp : virtual kern::Library, virtual engine_input_actions, virtual spiel {
  // ---- functions ----
  struct keyResult { f32 Result = 0.0f; };
  KERN_PURE keyResult key(Object<kern::Input> in = Object<kern::Input>{}, Key k = Key::unknown) {
    bool down = false;
    down = engine_input_down(in, k);
    return {kern::formula<bool, f32, f32>([](auto down, auto a0f, auto a0f_2) { return (down ? a0f : a0f_2); }, down, 1.0f, 0.0f)};
  }

  struct json_escapeResult { Text Result = ""; };
  KERN_PURE json_escapeResult json_escape(const Text& s) {
    Text o = "";
    bool skip = false;
    i8 c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(text_length(s), 1)); it10.next();) {
      c = num_cast<i8, i64>(text_code_at(s, n10.index));
      skip = false;
      if (logic_or(math_compare<"==", i32>(num_cast<i32, i8>(c), 34), math_compare<"==", i32>(num_cast<i32, i8>(c), 92))) {
        o = text_append(o, text_of_code(92), "");
      }
      if (math_compare<"<", i32>(num_cast<i32, u8>(num_cast<u8, i8>(c)), 32)) {
        skip = true;
      }
      if (logic_not(skip)) {
        o = text_append(o, text_of_code(num_cast<i64, i8>(c)), "");
      }
    }
    return {o};
  }

  struct pad_downResult { bool Result = false; };
  KERN_PURE pad_downResult pad_down(const PadState& p, i32 b = 0) {
    return {math_compare<"!=", u32>(kern::formula<u32, u32, i32>([](auto buttons, auto a1, auto b) { return (buttons & (a1 << b)); }, p.buttons, 1, b), 0)};
  }

  struct pad_hitResult { bool Result = false; };
  KERN_PURE pad_hitResult pad_hit(const PadState& p, i32 b = 0) {
    return {math_compare<"!=", u32>(kern::formula<u32, u32, i32>([](auto pressed, auto a1, auto b) { return (pressed & (a1 << b)); }, p.pressed, 1, b), 0)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
};
