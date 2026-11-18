struct settings_cpp : virtual kern::Library, virtual settings {
  // ---- variables ----
  KERN_CONST OptSpec kNone = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false};
  KERN_CONST Int kCount = 60;

  // ---- functions ----
  void escape_into(Text& out, Text s = "") {
    Int c = 0;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        out = text_append(out, text_of_code(92), "");
        out = text_append(out, text_of_code(c), "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        out = text_append(out, text_of_code(32), "");
      } else {
        out = text_append(out, text_of_code(c), "");
      }
    }
  }

  struct numResult { Text Result = ""; };
  KERN_PURE numResult num(Float v = 0.0) {
    Text b = "";
    b = "";
    b = text_format<"%g">(31, v);
    return {b};
  }

  struct opt_specResult { OptSpec Result = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}; };
  KERN_PURE opt_specResult opt_spec(const SettingsTable& table, Opt o = Opt::mouse_sensitivity) {
    return {logic_select<OptSpec>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(enum_index<Opt>(o)), kern::to<Float>(list_length<OptSpec>(table.specs))), 0.5), table.specs[enum_index<Opt>(o)], kNone)};
  }

  struct find_optResult { Bool Result = false; };
  find_optResult find_opt(const SettingsTable& table, Opt& out, Text key = "") {
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it11.next();) {
      if (text_equal(key, table.specs[n11.index].key)) {
        out = enum_from_index<Opt>(n11.index).value;
        return {true};
      }
    }
    return {false};
  }

  struct Settings_newResult { Settings Result = Settings{List<Float>{}, 1}; };
  KERN_PURE Settings_newResult Settings_new(const SettingsTable& table) {
    Settings self = Settings{List<Float>{}, 1};
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kCount)))); it12.next();) {
      self.v_[n12.index] = struct_break<OptSpec>(opt_spec(table, enum_from_index<Opt>(n12.index).value).Result).def;
    }
    return {self};
  }

  struct Settings_values_jsonResult { Text Result = ""; };
  KERN_PURE Settings_values_jsonResult Settings_values_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    o = "{";
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it15.next();) {
      if (math_compare<"!=">(kern::to<Float>(n15.index), 0.0)) {
        o = text_append(o, text_of_code(44), "");
      }
      o = text_append(o, text_of_code(34), "");
      o = text_append(o, table.specs[n15.index].key, "");
      o = text_append(o, "\":", "");
      o = text_append(o, num(self.v_[n15.index]).Result, "");
    }
    return {text_append(o, "}", "")};
  }

  struct Settings_schema_jsonResult { Text Result = ""; };
  Settings_schema_jsonResult Settings_schema_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    OptSpec s = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false};
    o = "[";
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it16.next();) {
      s = table.specs[n16.index];
      if (math_compare<"!=">(kern::to<Float>(n16.index), 0.0)) {
        o = text_append(o, text_of_code(44), "");
      }
      o = text_append(o, "{\"key\":\"", "");
      escape_into(o, s.key);
      o = text_append(o, "\",\"section\":\"", "");
      escape_into(o, s.section);
      o = text_append(o, "\",\"label\":\"", "");
      escape_into(o, s.label);
      o = text_append(o, "\",\"help\":\"", "");
      escape_into(o, s.help);
      o = text_append(o, text_append(text_append(text_append(text_append(text_append(text_append(text_append("\",\"min\":", num(s.the_min).Result, ""), ",\"max\":", ""), num(s.the_max).Result, ""), ",\"step\":", ""), num(s.step).Result, ""), ",\"def\":", ""), num(s.def).Result, ""), "");
      o = text_append(o, text_append(text_append(text_append(text_append(text_append(text_append(",\"toggle\":", logic_select<Text>(s.toggle, "true", "false"), ""), ",\"live\":", ""), logic_select<Text>(s.live, "true", "false"), ""), ",\"value\":", ""), num(self.v_[n16.index]).Result, ""), "}", ""), "");
    }
    return {text_append(o, "]", "")};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
};
