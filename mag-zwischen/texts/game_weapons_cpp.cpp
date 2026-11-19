struct game_weapons_cpp : virtual kern::Library, virtual weapons {
  // ---- types ----
  KERN_STRUCT(Loadout) { weapons_Id id = weapons_Id::smg; Int magazine = 0; Int reserve = 0; };

  // ---- functions ----
  struct hitscan_on_footResult { Bool Result = false; };
  KERN_PURE hitscan_on_footResult hitscan_on_foot(weapons_Id id = weapons_Id::smg) {
    return {logic_or(logic_or(enum_equal<weapons_Id>(id, weapons_Id::smg), enum_equal<weapons_Id>(id, weapons_Id::glock)), enum_equal<weapons_Id>(id, weapons_Id::sniper))};
  }
};
