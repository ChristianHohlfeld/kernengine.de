struct game : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(PadState) { Bool connected = false; Float lx = 0.0; Float ly = 0.0; Float rx = 0.0; Float ry = 0.0; Float lt = 0.0; Float rt = 0.0; U32 buttons = 0; U32 pressed = 0; Bool active = false; };

  // ---- functions ----
  struct PadState_downResult { Bool Result = false; };
  KERN_PURE PadState_downResult PadState_down(const PadState& self, I32 b = 0) {
    return {math_compare<">">(kern::formula([](Float buttons, Float b) -> Float { return (kern::math::band(buttons, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.buttons), kern::to<Float>(b)), 0.5)};
  }

  struct PadState_hitResult { Bool Result = false; };
  KERN_PURE PadState_hitResult PadState_hit(const PadState& self, I32 b = 0) {
    return {math_compare<">">(kern::formula([](Float pressed, Float b) -> Float { return (kern::math::band(pressed, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.pressed), kern::to<Float>(b)), 0.5)};
  }
};
