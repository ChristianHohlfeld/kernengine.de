struct game_details_cpp : virtual kern::Library, virtual game {
  // ---- variables ----
  /// The words of a command from `at` on, as std::istream's >> reads them (libstdc++, the C locale): spaces are skipped; a word goes up to the next space; a whole number is decimal, with a sign - none there: 0, beyond its type: the type's end - and once one read has failed (`good`), none after it reads anything or changes its value (also where only spaces are left).
  KERN_CONST Text kSpaces = " \t\n\013\014\r";

  // ---- functions ----
  struct is_busResult { Bool Result = false; };
  KERN_PURE is_busResult is_bus(const TrafficCar& c) {
    return {math_compare<">">(c.car[0].s_.radius, 0.4)};
  }

  struct read_wordResult { Bool Result = false; };
  read_wordResult read_word(const Text& s, Int& at, Bool& good, Text& w) {
    Int from = 0;
    Int to = 0;
    if (logic_not(good)) {
      return {false};
    }
    from = text_find_first_not_of(s, kSpaces, at);
    if (math_compare<"==">(kern::to<Float>(from), -1.0)) {
      good = false;
      return {false};
    }
    to = text_find_first_of(s, kSpaces, from);
    w = text_sub(s, from, kern::to<Int>(kern::formula([](Float to, Float from) -> Float { return ((to == (-1.0)) ? (-1.0) : (to - from)); }, kern::to<Float>(to), kern::to<Float>(from))));
    at = kern::to<Int>(kern::formula([](Float to, Float a) -> Float { return ((to == (-1.0)) ? a : to); }, kern::to<Float>(to), kern::to<Float>(text_length(s))));
    return {true};
  }

  struct next_atResult { Int Result = 0; };
  /// (where the next thing to read begins - npos: only spaces are left)
  KERN_PURE next_atResult next_at(const Text& s, Int at = 0) {
    return {text_find_first_not_of(s, kSpaces, at)};
  }

  struct number_atResult { Bool Result = false; };
  /// (the number that begins at `from`: strtoll reads the characters >> reads there - a sign, digits - and where they end; false: there is none)
  number_atResult number_at(const Text& s, Int& at, Int& x, Int from = 0) {
    Int end = 0;
    end = 0;
    end = text_parse_int(s, from, 10).end;
    x = int64_to_int(text_parse_int(s, from, 10).value);
    if (math_compare<"==">(kern::to<Float>(end), kern::to<Float>(from))) {
      return {false};
    }
    at = end;
    return {true};
  }

  struct read_intResult { Bool Result = false; };
  read_intResult read_int(const Text& s, Int& at, Bool& good, Int& v) {
    Int from = 0;
    Int x = 0;
    if (logic_not(good)) {
      return {false};
    }
    from = next_at(s, at).Result;
    x = 0;
    if (math_compare<"==">(kern::to<Float>(from), -1.0)) {
      good = false;
    } else {
      n10 = number_at(s, at, x, from);
      if (logic_not(n10.Result)) {
        v = 0;
        good = false;
      } else if (logic_or(math_compare<">">(kern::to<Float>(x), 2147483647.0), math_compare<"<">(kern::to<Float>(x), -2147483648.0))) {
        v = kern::to<Int>(kern::formula([](Float x) -> Float { return ((x > 2147483647.0) ? 2147483647.0 : ((-2147483647.0) - 1.0)); }, kern::to<Float>(x)));
        good = false;
      } else {
        v = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(x)));
      }
    }
    return {good};
  }

  struct read_u32Result { Bool Result = false; };
  read_u32Result read_u32(const Text& s, Int& at, Bool& good, Int& v) {
    Int from = 0;
    Int x = 0;
    Bool minus = false;
    Int m = 0;
    if (logic_not(good)) {
      return {false};
    }
    from = next_at(s, at).Result;
    x = 0;
    if (math_compare<"==">(kern::to<Float>(from), -1.0)) {
      good = false;
      return {false};
    }
    n11 = number_at(s, at, x, from);
    if (logic_not(n11.Result)) {
      v = 0;
      good = false;
      return {false};
    }
    minus = math_compare<"<">(kern::to<Float>(x), 0.0);
    m = kern::to<Int>(kern::formula([](Float minus, Float x) -> Float { return ((minus != 0.0) ? (0.0 - x) : x); }, kern::to<Float>(minus), kern::to<Float>(x)));
    if (math_compare<">">(kern::formula([](Float m) -> Float { return ((4294967295.0 < 0.0) != (m < 0.0) ? (m < 0.0) : (4294967295.0 < m)); }, kern::to<Float>(m)), 0.5)) {
      v = 4294967295;
      good = false;
      return {false};
    }
    v = kern::to<Int>(kern::formula([](Float minus, Float m) -> Float { return ((minus != 0.0) ? kern::math::wrap32(0.0 - kern::math::wrap32(m)) : kern::math::wrap32(m)); }, kern::to<Float>(minus), kern::to<Float>(m)));
    return {true};
  }

  // ---- what nodes give and keep ----
  number_atResult n10;
  number_atResult n11;
};
