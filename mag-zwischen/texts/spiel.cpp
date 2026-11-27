struct spiel : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(PadButtonIndex, kPadA, kPadB, kPadX, kPadY, kPadLB, kPadRB, kPadLT, kPadRT, kPadView, kPadMenu, kPadL3, kPadR3, kPadUp, kPadDown, kPadLeft, kPadRight);
  KERN_STRUCT(PadState) { bool connected = false; f32 lx = 0.0f; f32 ly = 0.0f; f32 rx = 0.0f; f32 ry = 0.0f; f32 lt = 0.0f; f32 rt = 0.0f; u32 buttons = 0; u32 pressed = 0; bool active = false; };
};
