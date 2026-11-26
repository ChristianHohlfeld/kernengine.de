struct drone_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(DroneArt_Part) { U32 id = 0; List<Float> transforms = List<Float>{}; List<Float> tints = List<Float>{}; Bool tinted = false; };
  KERN_STRUCT(DroneArt) { Object<kern::Renderer> r_ = Object<kern::Renderer>{}; DroneArt_Part frame_ = DroneArt_Part{0, List<Float>{}, List<Float>{}, false}; DroneArt_Part motor_ = DroneArt_Part{0, List<Float>{}, List<Float>{}, false}; DroneArt_Part prop_a_ = DroneArt_Part{0, List<Float>{}, List<Float>{}, false}; DroneArt_Part prop_b_ = DroneArt_Part{0, List<Float>{}, List<Float>{}, false}; DroneArt_Part led_ = DroneArt_Part{0, List<Float>{}, List<Float>{}, false}; };
};
