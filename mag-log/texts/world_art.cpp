struct world_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(WorldArt_Part) { Int id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; Bool tinted = false; };
  KERN_STRUCT(WorldArt) { Int cube = 0; Int sphere = 0; Int cylinder = 0; Int glow = 0; Int r_ = 0; List<WorldArt_Part> parts_ = List<WorldArt_Part>{}; };

  // ---- functions ----
  struct WorldArt_rendererResult { Int Result = 0; };
  WorldArt_rendererResult WorldArt_renderer(WorldArt& self) {
    return {self.r_};
  }
};
