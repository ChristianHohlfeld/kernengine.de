struct mp_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(MpArt_Part) { U32 id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; };
  KERN_STRUCT(MpArt) { Object<kern::Renderer> r_ = Object<kern::Renderer>{}; MpArt_Part post_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part gem_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part ring_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part tracer_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part flash_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part canopy_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part led_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; MpArt_Part shield_ = MpArt_Part{0, List<Float>{}, List<Float>{}}; };

  // ---- functions ----
  struct MpArt_readyResult { Bool Result = false; };
  KERN_PURE MpArt_readyResult MpArt_ready(const MpArt& self) {
    return {object_valid<kern::Renderer>(self.r_)};
  }
};
