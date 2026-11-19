struct game_destruction_sync : virtual kern::Library, virtual city_stream {
  // ---- types ----
  KERN_STRUCT(DestroySync_Mine) { Terrain_CraterRec r = Terrain_CraterRec{0.0, 0.0, 0.0, 0.0}; Float at = 0.0; Bool told = false; };
  KERN_STRUCT(DestroySync) { Int epoch = 1; Float delta_clock = 0.0; Float digest_clock = 0.0; List<Terrain_CraterRec> sent = List<Terrain_CraterRec>{}; List<DestroySync_Mine> mine = List<DestroySync_Mine>{}; List<CityStream_DamageRec> dmg_out = List<CityStream_DamageRec>{}; List<Int_Float_pair> pts_out = List<Int_Float_pair>{}; List<Int> known = List<Int>{}; Int mismatches = 0; Float told_at = -99.0; Bool was_active = false; Bool hosting = false; Int sent_states = 0; Int got_states = 0; Int asks = 0; };
};
