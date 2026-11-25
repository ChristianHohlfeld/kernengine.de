struct facades : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Facades) { Float night = 0.0; U32 u_night_ = 0; };

  // ---- functions ----
  struct Facades_nameResult { Text Result = ""; };
  KERN_PURE Facades_nameResult Facades_name() {
    return {"spawncity-facades"};
  }
};
