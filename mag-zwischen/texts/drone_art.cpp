struct drone_art : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(DroneArt_Part) { u32 id = 0; List<f32> transforms = List<f32>{}; List<f32> tints = List<f32>{}; bool tinted = false; };
  KERN_STRUCT(DroneArt) { Object<kern::Renderer> r_ = Object<kern::Renderer>{}; DroneArt_Part frame_ = DroneArt_Part{0, List<f32>{}, List<f32>{}, false}; DroneArt_Part motor_ = DroneArt_Part{0, List<f32>{}, List<f32>{}, false}; DroneArt_Part prop_a_ = DroneArt_Part{0, List<f32>{}, List<f32>{}, false}; DroneArt_Part prop_b_ = DroneArt_Part{0, List<f32>{}, List<f32>{}, false}; DroneArt_Part led_ = DroneArt_Part{0, List<f32>{}, List<f32>{}, false}; };
};
