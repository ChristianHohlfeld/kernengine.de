struct combat_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(CombatArt_Part) { U32 id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; };
  KERN_ENUM(CombatArt_Shape, box, sphere, cylinder, cone, glow_box, glow_sphere, smoke, fade_sphere, fade_ring, kShapes);
  KERN_STRUCT(CombatArt) { Object<kern::Renderer> r_ = Object<kern::Renderer>{}; Fixed<CombatArt_Part, 9> parts_ = List<CombatArt_Part>{}; };
};
