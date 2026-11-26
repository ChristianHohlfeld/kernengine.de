struct settings_cpp : virtual kern::Library, virtual blueprint_call, virtual settings {
  // ---- variables ----
  KERN_CONST U32 kCount = 60;
  KERN_CONST OptSpec kNone = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false};

  // ---- functions ----
  struct Settings_SnapResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("Snap") Settings_SnapResult Settings_Snap(Option option = Option{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}, Float value = 0.0);

  struct Settings_WordResult { Bool known = false; Float value = 0.0; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("Word") Settings_WordResult Settings_Word(Text text = "");

  struct Settings_InSectionResult { Bool in = false; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("InSection") Settings_InSectionResult Settings_InSection(Option option = Option{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}, Text section = "");

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

  struct bind_settingsResult { Bool Result = false; };
  bind_settingsResult bind_settings(SettingsTable& table) {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Settings"));
    list_clear(table.specs);
    table.rows = func_get_on<"Options", List<Option>>(actor_find("Settings"));
    if (logic_or(logic_not(runs), math_compare<"!=">(kern::to<Float>(list_length<Option>(table.rows)), kern::to<Float>(kCount)))) {
      list_clear(table.rows);
      return {false};
    }
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Option>(table.rows))))); it11.next();) {
      list_add(table.specs, struct_make<OptSpec>(table.rows[n11.index].key, table.rows[n11.index].section, table.rows[n11.index].label, table.rows[n11.index].help, table.rows[n11.index].the_min, table.rows[n11.index].the_max, table.rows[n11.index].step, table.rows[n11.index].def, table.rows[n11.index].toggle, table.rows[n11.index].live));
    }
    return {true};
  }

  struct opt_specResult { OptSpec Result = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}; };
  KERN_PURE opt_specResult opt_spec(const SettingsTable& table, Opt o = Opt::mouse_sensitivity) {
    return {logic_select<OptSpec>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(enum_index<Opt>(o)), kern::to<Float>(list_length<OptSpec>(table.specs))), 0.5), table.specs[enum_index<Opt>(o)], kNone)};
  }

  struct find_optResult { Bool Result = false; };
  find_optResult find_opt(const SettingsTable& table, Opt& out, Text key = "") {
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it12.next();) {
      if (text_equal(key, table.specs[n12.index].key)) {
        out = enum_from_index<Opt>(n12.index).value;
        return {true};
      }
    }
    return {false};
  }

  struct Settings_newResult { Settings Result = Settings{List<Float>{}, 1}; };
  Settings_newResult Settings_new(const SettingsTable& table) {
    Settings self = Settings{List<Float>{}, 1};
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kCount)))); it13.next();) {
      self.v_[n13.index] = struct_break<OptSpec>(opt_spec(table, enum_from_index<Opt>(n13.index).value).Result).def;
    }
    return {self};
  }

  void Settings_set(Settings& self, const SettingsTable& table, Opt o = Opt::mouse_sensitivity, Float value = 0.0) {
    U32 i = 0;
    Float v = 0.0;
    i = enum_index<Opt>(o);
    if (logic_or(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (!((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a))); }, kern::to<Float>(i), kern::to<Float>(list_length<Option>(table.rows))), 0.5), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, value), 0.0)))) {
      return;
    }
    func_call_on<"Settings_Snap">(actor_find("Settings"), kern::out(n14.value), table.rows[i], blueprint_call::decimal(value).Result);
    v = n14.value;
    if (math_compare<">">(kern::formula([](Float v, Float a) -> Float { return (v != a); }, v, self.v_[i]), 0.5)) {
      self.v_[i] = v;
      self.version_ = kern::to<Int>(kern::formula([](Float version_) -> Float { return (version_ + 1.0); }, kern::to<Float>(self.version_)));
    }
  }

  struct Settings_set_2Result { Bool Result = false; };
  Settings_set_2Result Settings_set_2(Settings& self, const SettingsTable& table, Text key = "", Text value = "") {
    Opt o = Opt::mouse_sensitivity;
    Bool known = false;
    Float word = 0.0;
    Text text = "";
    Int end = 0;
    Float d = 0.0;
    o = Opt::mouse_sensitivity;
    n15 = find_opt(table, o, key);
    if (logic_not(n15.Result)) {
      return {false};
    }
    known = false;
    word = 0.0;
    func_call_on<"Settings_Word">(actor_find("Settings"), kern::out(n16.known, n16.value), value);
    known = n16.known;
    word = n16.value;
    if (known) {
      Settings_set(self, table, o, word);
      return {true};
    }
    text = value;
    end = 0;
    end = text_parse_float(text, 0).end;
    d = text_parse_float(text, 0).value;
    if (math_compare<"==">(kern::to<Float>(end), 0.0)) {
      return {false};
    }
    Settings_set(self, table, o, d);
    return {true};
  }

  void Settings_reset_section(Settings& self, const SettingsTable& table, Text section = "") {
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Option>(table.rows))))); it17.next();) {
      func_call_on<"Settings_InSection">(actor_find("Settings"), kern::out(n18.in), table.rows[n17.index], section);
      if (n18.in) {
        Settings_set(self, table, enum_from_index<Opt>(n17.index).value, table.specs[n17.index].def);
      }
    }
  }

  struct Settings_values_jsonResult { Text Result = ""; };
  KERN_PURE Settings_values_jsonResult Settings_values_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    o = "{";
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it19.next();) {
      if (math_compare<"!=">(kern::to<Float>(n19.index), 0.0)) {
        o = text_append(o, text_of_code(44), "");
      }
      o = text_append(o, text_of_code(34), "");
      o = text_append(o, table.specs[n19.index].key, "");
      o = text_append(o, "\":", "");
      o = text_append(o, num(self.v_[n19.index]).Result, "");
    }
    return {text_append(o, "}", "")};
  }

  struct Settings_schema_jsonResult { Text Result = ""; };
  Settings_schema_jsonResult Settings_schema_json(const Settings& self, const SettingsTable& table) {
    Text o = "";
    OptSpec s = OptSpec{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false};
    o = "[";
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<OptSpec>(table.specs))))); it20.next();) {
      s = table.specs[n20.index];
      if (math_compare<"!=">(kern::to<Float>(n20.index), 0.0)) {
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
      o = text_append(o, text_append(text_append(text_append(text_append(text_append(text_append(",\"toggle\":", logic_select<Text>(s.toggle, "true", "false"), ""), ",\"live\":", ""), logic_select<Text>(s.live, "true", "false"), ""), ",\"value\":", ""), num(self.v_[n20.index]).Result, ""), "}", ""), "");
    }
    return {text_append(o, "]", "")};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct { Float value = 0.0; } n14;
  find_optResult n15;
  struct { Bool known = false; Float value = 0.0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct { Bool in = false; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
};
