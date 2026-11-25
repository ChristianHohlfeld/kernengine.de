struct world_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(WorldArt_Part) { U32 id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; Bool tinted = false; };
  KERN_STRUCT(WorldArt) { U32 cube = 0; U32 sphere = 0; U32 cylinder = 0; U32 glow = 0; Int r_ = 0; List<WorldArt_Part> parts_ = List<WorldArt_Part>{}; };

  // ---- functions ----
  struct WorldArt_rendererResult { Int Result = 0; };
  WorldArt_rendererResult WorldArt_renderer(WorldArt& self) {
    return {self.r_};
  }
};
