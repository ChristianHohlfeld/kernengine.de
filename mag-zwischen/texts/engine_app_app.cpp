struct engine_app_app : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Frame) { Float time = 0.0; Float dt = 0.0; Int64 index = Int64{0x0}; };
};
