struct water_look : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(WaterLook) { Float time = 0.0; Int u_time_ = 0; };

  // ---- functions ----
  struct WaterLook_nameResult { Text Result = ""; };
  KERN_PURE WaterLook_nameResult WaterLook_name() {
    return {"spawncity-water"};
  }
};
