struct qr_code : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(qr_Code) { I32 version = 0; I32 size = 0; List<U8> modules = List<U8>{}; };

  // ---- functions ----
  struct Code_darkResult { Bool Result = false; };
  KERN_PURE Code_darkResult Code_dark(const qr_Code& self, I32 x = 0, I32 y = 0) {
    return {logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(x), 0.0), math_compare<">=">(kern::to<Float>(y), 0.0)), math_compare<"<">(kern::to<Float>(x), kern::to<Float>(self.size))), math_compare<"<">(kern::to<Float>(y), kern::to<Float>(self.size))), math_compare<"!=">(kern::to<Float>(self.modules[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return ((y * size) + x); }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))]), 0.0))};
  }
};
