struct blueprint_call : virtual kern::Library {
  // ---- variables ----
  /// The loop guard of the logic the game's Blueprints run in (Runtime::budget: loop passes in one step). The functions the game calls loop long - a code encoded bit by bit, a sound sample by sample, a number raised to a 256-bit power -, so the game and its tests set the guard to this.
  KERN_CONST u32 kLoopBudget = 2000000000;

  // ---- functions ----
  struct decimalResult { f64 Result = 0.0; };
  /// A number of the game's 32-bit state as a Blueprint gets it: the decimal number it stands for. A Blueprint computes with 64-bit numbers (as the original's JavaScript does) and its rules name their thresholds in decimals - 0.1, 2.6, 0.42. The game keeps its state in 32 bit, where "0.1" is 0.100000001490116...; passed on as that, it would lie just beyond a threshold of 0.1 it stands exactly on. So a 32-bit number crosses over as the shortest decimal that is this very 32-bit number (0.1f -> 0.1): every comparison of it with a decimal threshold then answers exactly as the comparison in 32 bit did, on both sides of the threshold and on it. The way is one to one and keeps the order: back in 32 bit it is the same number again, and of two numbers the larger stays the larger. (Whole numbers, and numbers beyond the range a game counts in, are what they are.)
  KERN_PURE decimalResult decimal(f32 x = 0.0f) {
    f64 d = 0.0;
    f64 a = 0.0;
    f64 scale = 0.0;
    f64 m = 0.0;
    f64 m_2 = 0.0;
    i32 digits = 0;
    f64 c = 0.0;
    d = num_cast<f64, f32>(x);
    a = kern::formula<f64>([](auto d) { return abs(d); }, d);
    if (logic_or(logic_not(logic_and(math_compare<">=", f64>(a, 1e-09), math_compare<"<", f64>(a, 10000000.0))), math_compare<"==", f64>(d, kern::formula<f64>([](auto d) { return trunc(d); }, d)))) {
      return {d};
    }
    scale = 1.0;
    m = a;
    while (loop(math_compare<"<", f64>(m, 1.0))) {
      scale = kern::formula<f64>([](auto scale) { return (scale * 10.0); }, scale);
      m = kern::formula<f64>([](auto m) { return (m * 10.0); }, m);
    }
    m_2 = a;
    while (loop(math_compare<">=", f64>(m_2, 10.0))) {
      scale = kern::formula<f64>([](auto scale) { return (scale / 10.0); }, scale);
      m_2 = kern::formula<f64>([](auto m_2) { return (m_2 / 10.0); }, m_2);
    }
    digits = 1;
    while (loop(math_compare<"<=", i32>(digits, 9))) {
      c = kern::formula<f64, f64>([](auto scale, auto d) { return ((scale >= 1.0) ? (round((d * scale)) / scale) : (round((d * scale)) * round((1.0 / scale)))); }, scale, d);
      if (math_compare<"==", f32>(num_cast<f32, f64>(c), x)) {
        return {c};
      }
      digits += 1;
      scale = kern::formula<f64>([](auto scale) { return (scale * 10.0); }, scale);
    }
    return {d};
  }
};
