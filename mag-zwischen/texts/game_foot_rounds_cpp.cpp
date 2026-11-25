struct game_foot_rounds_cpp : virtual kern::Library, virtual weapons {
  // ---- functions ----
  struct foot_roundResult { Bool Result = false; };
  KERN_PURE foot_roundResult foot_round(weapons_Id id = weapons_Id::smg) {
    return {logic_or(logic_or(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(id, weapons_Id::rocket)), enum_equal<weapons_Id>(id, weapons_Id::hand_grenade))};
  }
};
