struct game_barricades_cpp : virtual kern::Library, virtual interiors {
  // ---- functions ----
  struct building_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE building_keyResult building_key(const Interiors_Plan& p) {
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(p.outer)), 0.0)) {
      return {Int64{0x0}};
    }
    return {int64_xor(int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round((x * 10.0)); }, vector_break(p.outer[0]).x))), 32), int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round((y * 10.0)); }, vector_break(p.outer[0]).y))), Int64{0xffffffff})), int64_of_int(list_length<Vector>(p.outer)))};
  }
};
