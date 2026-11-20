struct qr_code : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(qr_Code) { Int version = 0; Int size = 0; List<Int> modules = List<Int>{}; };

  // ---- functions ----
  struct Code_darkResult { Bool Result = false; };
  KERN_PURE Code_darkResult Code_dark(const qr_Code& self, Int x = 0, Int y = 0) {
    return {logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(x), 0.0), math_compare<">=">(kern::to<Float>(y), 0.0)), math_compare<"<">(kern::to<Float>(x), kern::to<Float>(self.size))), math_compare<"<">(kern::to<Float>(y), kern::to<Float>(self.size))), math_compare<"!=">(kern::to<Float>(self.modules[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return ((y * size) + x); }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))]), 0.0))};
  }
};
