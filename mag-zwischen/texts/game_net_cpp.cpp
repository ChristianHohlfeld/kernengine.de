struct game_net_cpp : virtual kern::Library, virtual mp_rules, virtual weapons {
  // ---- functions ----
  struct escResult { Text Result = ""; };
  KERN_PURE escResult esc(const Text& s) {
    Text o = "";
    Int c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        o = text_append(o, text_of_code(92), "");
        o = text_append(o, text_of_code(c), "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        o = text_append(o, text_of_code(32), "");
      } else {
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {o};
  }

  struct hexResult { Text Result = ""; };
  KERN_PURE hexResult hex(U32 c = 0) {
    Text b = "";
    b = "";
    b = text_format<"#%06x">(15, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 16777215.0); }, kern::to<Float>(c))));
    return {b};
  }

  struct mp_weaponResult { mp_Weapon Result = mp_Weapon::other; };
  KERN_PURE mp_weaponResult mp_weapon(weapons_Id id = weapons_Id::smg) {
    switch (flow_switch_enum<weapons_Id>(id)) {
      case weapons_Id::smg: {
        return {mp_Weapon::smg};
      } break;
      case weapons_Id::glock: {
        return {mp_Weapon::glock};
      } break;
      case weapons_Id::sniper: {
        return {mp_Weapon::sniper};
      } break;
      case weapons_Id::grenade_launcher: {
        return {mp_Weapon::launcher};
      } break;
      case weapons_Id::rocket: {
        return {mp_Weapon::rocket};
      } break;
      case weapons_Id::fists: {
        return {mp_Weapon::fists};
      } break;
      case weapons_Id::knife: {
        return {mp_Weapon::knife};
      } break;
      case weapons_Id::hand_grenade: {
        return {mp_Weapon::grenade};
      } break;
      case weapons_Id::drone_gun: {
        return {mp_Weapon::drone_gun};
      } break;
      case weapons_Id::drone_rockets: {
        return {mp_Weapon::drone_rocket};
      } break;
      case weapons_Id::drone_guided: {
        return {mp_Weapon::drone_rocket};
      } break;
      case weapons_Id::jet_cannon: {
        return {mp_Weapon::jet_gun};
      } break;
      case weapons_Id::jet_missile: {
        return {mp_Weapon::jet_missile};
      } break;
      case weapons_Id::jet_bomb: {
        return {mp_Weapon::jet_bomb};
      } break;
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
