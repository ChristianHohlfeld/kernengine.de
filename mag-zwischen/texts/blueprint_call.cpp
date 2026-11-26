struct blueprint_call : virtual kern::Library {
  // ---- variables ----
  /// The loop guard of the logic the game's Blueprints run in (Runtime::budget: loop passes in one step). The functions the game calls loop long - a code encoded bit by bit, a sound sample by sample, a number raised to a 256-bit power -, so the game and its tests set the guard to this.
  KERN_CONST U32 kLoopBudget = 2000000000;

  // ---- functions ----
  struct decimalResult { Float Result = 0.0; };
  /// A number of the game's 32-bit state as a Blueprint gets it: the decimal number it stands for. A Blueprint computes with 64-bit numbers (as the original's JavaScript does) and its rules name their thresholds in decimals - 0.1, 2.6, 0.42. The game keeps its state in 32 bit, where "0.1" is 0.100000001490116...; passed on as that, it would lie just beyond a threshold of 0.1 it stands exactly on. So a 32-bit number crosses over as the shortest decimal that is this very 32-bit number (0.1f -> 0.1): every comparison of it with a decimal threshold then answers exactly as the comparison in 32 bit did, on both sides of the threshold and on it. The way is one to one and keeps the order: back in 32 bit it is the same number again, and of two numbers the larger stays the larger. (Whole numbers, and numbers beyond the range a game counts in, are what they are.)
  KERN_PURE decimalResult decimal(Float x = 0.0) {
    Float d = 0.0;
    Float a = 0.0;
    Float scale = 0.0;
    Float m = 0.0;
    Float m_2 = 0.0;
    I32 digits = 0;
    Float c = 0.0;
    d = x;
    a = kern::formula([](Float d) -> Float { return kern::math::abs(d); }, d);
    if (logic_or(logic_not(logic_and(math_compare<">=">(a, 1e-09), math_compare<"<">(a, 10000000.0))), math_compare<">">(kern::formula([](Float d) -> Float { return (d == kern::math::trunc(d)); }, d), 0.5))) {
      return {d};
    }
    scale = 1.0;
    m = a;
    while (loop(math_compare<"<">(m, 1.0))) {
      scale = kern::formula([](Float scale) -> Float { return (scale * 10.0); }, scale);
      m = kern::formula([](Float m) -> Float { return (m * 10.0); }, m);
    }
    m_2 = a;
    while (loop(math_compare<">=">(m_2, 10.0))) {
      scale = kern::formula([](Float scale) -> Float { return kern::math::div(scale, 10.0); }, scale);
      m_2 = kern::formula([](Float m_2) -> Float { return kern::math::div(m_2, 10.0); }, m_2);
    }
    digits = 1;
    while (loop(math_compare<"<=">(kern::to<Float>(digits), 9.0))) {
      c = kern::formula([](Float scale, Float d) -> Float { return ((scale >= 1.0) ? kern::math::div(kern::math::round((d * scale)), scale) : (kern::math::round((d * scale)) * kern::math::round(kern::math::div(1.0, scale)))); }, scale, d);
      if (math_compare<">">(kern::formula([](Float c, Float x) -> Float { return (c == x); }, c, x), 0.5)) {
        return {c};
      }
      digits += 1;
      scale = kern::formula([](Float scale) -> Float { return (scale * 10.0); }, scale);
    }
    return {d};
  }
};
