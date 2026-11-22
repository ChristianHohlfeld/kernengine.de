struct engine_app_app : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Frame) { Float time = 0.0; Float dt = 0.0; Int64 index = Int64{0x0}; };
  KERN_ENUM(Key, unknown, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9, left, right, up, down, space, enter, escape, tab, backspace, shift_left, shift_right, control_left, control_right, alt_left, alt_right, count);
};
