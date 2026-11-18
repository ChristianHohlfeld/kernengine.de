struct facades : virtual kern::Library {
  // ---- functions ----
  struct Facades_nameResult { Text Result = ""; };
  KERN_PURE Facades_nameResult Facades_name() {
    return {"spawncity-facades"};
  }
};
