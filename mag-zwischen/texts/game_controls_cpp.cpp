struct game_controls_cpp : virtual kern::Library, virtual engine_app_app, virtual game {
  // ---- types ----
  KERN_STRUCT(AimTarget) { I32 kind = 0; I32 index = -1; Float scan_at = -99.0; };

  // ---- functions ----
  struct keyResult { Float Result = 0.0; };
  KERN_PURE keyResult key(Int in = 0, Key k = Key::unknown) {
    Bool down = false;
    down = engine_input_down(in, k);
    return {kern::formula([](Float down) -> Float { return ((down != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(down))};
  }

  struct hint_weaponResult { Text Result = ""; };
  KERN_PURE hint_weaponResult hint_weapon(Mode m = Mode::foot, weapons_Id foot = weapons_Id::smg, I32 drone = 0) {
    if (enum_equal<Mode>(m, Mode::drone)) {
      return {logic_select<Text>(math_compare<"==">(kern::to<Float>(drone), 0.0), "GUN", logic_select<Text>(math_compare<"==">(kern::to<Float>(drone), 1.0), "MISSILE", ""))};
    }
    switch (flow_switch_enum<weapons_Id>(foot)) {
      case weapons_Id::smg: {
        return {"SMG"};
      } break;
      case weapons_Id::glock: {
        return {"PISTOL"};
      } break;
      case weapons_Id::sniper: {
        return {""};
      } break;
      case weapons_Id::grenade_launcher: {
        return {"GRENADE LAUNCHER"};
      } break;
      case weapons_Id::rocket: {
        return {"ROCKET LAUNCHER"};
      } break;
      case weapons_Id::fists: {
        return {""};
      } break;
      case weapons_Id::knife: {
        return {""};
      } break;
      case weapons_Id::hand_grenade: {
        return {""};
      } break;
      case weapons_Id::drone_gun: {
        return {""};
      } break;
      case weapons_Id::drone_rockets: {
        return {""};
      } break;
      case weapons_Id::drone_guided: {
        return {""};
      } break;
      case weapons_Id::jet_cannon: {
        return {""};
      } break;
      case weapons_Id::jet_missile: {
        return {""};
      } break;
      case weapons_Id::jet_bomb: {
        return {""};
      } break;
    }
  }

  struct jescResult { Text Result = ""; };
  KERN_PURE jescResult jesc(const Text& s) {
    Text o = "";
    Bool skip = false;
    Int c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        o = text_append(o, text_of_code(92), "");
      }
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {o};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
