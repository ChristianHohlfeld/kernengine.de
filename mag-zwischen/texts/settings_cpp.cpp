struct settings_cpp : virtual kern::Library, virtual settings {
  // ---- variables ----
  KERN_CONST OptSpec kNone = OptSpec{"", "", "", "", 0.0f, 0.0f, 0.0f, 0.0f, false, false};
  KERN_CONST u32 kCount = 60;

  // ---- functions ----
  void escape_into(Text& out, Text s = "") {
    i8 c = 0;
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(text_length(s), 1)); it10.next();) {
      c = num_cast<i8, i64>(text_code_at(s, n10.index));
      if (logic_or(math_compare<"==", i32>(num_cast<i32, i8>(c), 34), math_compare<"==", i32>(num_cast<i32, i8>(c), 92))) {
        out = text_append(out, text_of_code(92), "");
        out = text_append(out, text_of_code(num_cast<i64, i8>(c)), "");
      } else if (math_compare<"<", i32>(num_cast<i32, u8>(num_cast<u8, i8>(c)), 32)) {
        out = text_append(out, text_of_code(32), "");
      } else {
        out = text_append(out, text_of_code(num_cast<i64, i8>(c)), "");
      }
    }
  }

  struct numResult { Text Result = ""; };
  KERN_PURE numResult num(f32 v = 0.0f) {
    Text b = "";
    b = "";
    b = text_format<"%g">(31, num_cast<f64, f32>(v));
    return {b};
  }

  struct opt_specResult { OptSpec Result = OptSpec{"", "", "", "", 0.0f, 0.0f, 0.0f, 0.0f, false, false}; };
  KERN_PURE opt_specResult opt_spec(const SettingsTable& table, Opt o = Opt::mouse_sensitivity) {
    return {logic_select<OptSpec>(math_compare<"<", u64>(num_cast<u64, u32>(num_cast<u32, i64>(enum_index<Opt>(o))), num_cast<u64, i64>(list_length<OptSpec>(table.specs))), table.specs[num_cast<i64, u32>(num_cast<u32, i64>(enum_index<Opt>(o)))], kNone)};
  }

  struct find_optResult { bool Result = false; };
  find_optResult find_opt(const SettingsTable& table, Opt& out, Text key = "") {
    u32 i = 0;
    i = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<OptSpec>(table.specs))))) {
      if (text_equal(key, table.specs[num_cast<i64, u32>(i)].key)) {
        out = enum_from_index<Opt>(num_cast<i64, u32>(i)).value;
        return {true};
      }
      i += 1;
    }
    return {false};
  }

  struct Settings_newResult { Settings Result = Settings{List<f32>{}, 1}; };
  Settings_newResult Settings_new(const SettingsTable& table) {
    Settings self = Settings{List<f32>{}, 1};
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(num_cast<i64, u32>(kCount), 1)); it11.next();) {
      self.v_[num_cast<i64, u32>(num_cast<u32, i64>(n11.index))] = struct_break<OptSpec>(opt_spec(table, enum_from_index<Opt>(num_cast<i64, u32>(num_cast<u32, i64>(n11.index))).value).Result).def;
    }
    return {self};
  }

  struct Settings_values_jsonResult { Text Result = ""; };
  KERN_PURE Settings_values_jsonResult Settings_values_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    u32 i = 0;
    o = "{";
    i = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<OptSpec>(table.specs))))) {
      if (num_cast<bool, u32>(i)) {
        o = text_append(o, text_of_code(44), "");
      }
      o = text_append(o, text_of_code(34), "");
      o = text_append(o, table.specs[num_cast<i64, u32>(i)].key, "");
      o = text_append(o, "\":", "");
      o = text_append(o, num(self.v_[num_cast<i64, u32>(i)]).Result, "");
      i += 1;
    }
    return {text_append(o, "}", "")};
  }

  struct Settings_schema_jsonResult { Text Result = ""; };
  Settings_schema_jsonResult Settings_schema_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    u32 i = 0;
    OptSpec s = OptSpec{"", "", "", "", 0.0f, 0.0f, 0.0f, 0.0f, false, false};
    o = "[";
    i = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<OptSpec>(table.specs))))) {
      s = table.specs[num_cast<i64, u32>(i)];
      if (num_cast<bool, u32>(i)) {
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
      o = text_append(o, text_append(text_append(text_append(text_append(text_append(text_append(",\"toggle\":", logic_select<Text>(s.toggle, "true", "false"), ""), ",\"live\":", ""), logic_select<Text>(s.live, "true", "false"), ""), ",\"value\":", ""), num(self.v_[num_cast<i64, u32>(i)]).Result, ""), "}", ""), "");
      i += 1;
    }
    return {text_append(o, "]", "")};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
};
