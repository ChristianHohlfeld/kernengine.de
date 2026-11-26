struct weapons : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(weapons_Id, smg, glock, sniper, grenade_launcher, rocket, fists, knife, hand_grenade, drone_gun, drone_rockets, drone_guided, jet_cannon, jet_missile, jet_bomb);
  KERN_STRUCT(weapons_Spec) { Text name = ""; Float interval_ms = 0.0; Float range_m = 0.0; Float damage = 0.0; Float radius_m = 0.0; Float speed_mps = 0.0; U8 key = 0; };

  // ---- variables ----
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};

  // ---- functions ----
  struct blast_factorResult { Float Result = 0.0; };
  /// Explosion falloff (applyBlast): smoothstep(1 - d / R), buildings in between x 0.18.
  KERN_PURE blast_factorResult blast_factor(Float distance = 0.0, Float radius = 0.0, Bool occluded = false) {
    Float t = 0.0;
    Float s = 0.0;
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float distance, Float radius) -> Float { return (1.0 - kern::math::div(distance, radius)); }, distance, radius), 0.0, 1.0));
    s = kern::formula([](Float t) -> Float { return ((t * t) * (3.0 - (2.0 * t))); }, t);
    return {kern::formula([](Float occluded, Float s) -> Float { return ((occluded != 0.0) ? (s * 0.18) : s); }, kern::to<Float>(occluded), s)};
  }
};
