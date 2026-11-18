struct engine_net_transport : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(net_Address) { Int64 id = Int64{0x0}; };
};
