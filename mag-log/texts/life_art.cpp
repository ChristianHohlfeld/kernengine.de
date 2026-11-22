struct life_art : virtual kern::Library, virtual ragdoll {
  // ---- types ----
  KERN_STRUCT(LifeArt_Part) { U32 id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; };
  KERN_STRUCT(LifeArt) { Int r_ = 0; LifeArt_Part box_ = LifeArt_Part{0, List<Float>{}, List<Float>{}}; LifeArt_Part ball_ = LifeArt_Part{0, List<Float>{}, List<Float>{}}; LifeArt_Part cyl_ = LifeArt_Part{0, List<Float>{}, List<Float>{}}; LifeArt_Part glow_ = LifeArt_Part{0, List<Float>{}, List<Float>{}}; List<Ragdolls_Draw> bones_ = List<Ragdolls_Draw>{}; };
};
