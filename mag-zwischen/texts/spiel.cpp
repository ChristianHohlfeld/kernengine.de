struct spiel : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(PadState) { Bool connected = false; Float lx = 0.0; Float ly = 0.0; Float rx = 0.0; Float ry = 0.0; Float lt = 0.0; Float rt = 0.0; U32 buttons = 0; U32 pressed = 0; Bool active = false; };
};
