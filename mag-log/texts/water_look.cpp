struct water_look : virtual kern::Library {
  // ---- functions ----
  struct WaterLook_nameResult { Text Result = ""; };
  KERN_PURE WaterLook_nameResult WaterLook_name() {
    return {"spawncity-water"};
  }
};
