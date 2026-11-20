struct weapons : virtual kern::Library {
  // ---- functions ----
  struct blast_factorResult { Float Result = 0.0; };
  /// Explosion falloff (applyBlast): smoothstep(1 - d / R), buildings in between x 0.18.
  KERN_PURE blast_factorResult blast_factor(Float distance = 0.0, Float radius = 0.0, Bool occluded = false) {
    Float t = 0.0;
    Float s = 0.0;
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float distance, Float radius) -> Float { return (1.0 - kern::math::div(distance, radius)); }, distance, radius), 0.0, 1.0));
    s = kern::formula([](Float t) -> Float { return ((t * t) * (3.0 - (2.0 * t))); }, t);
    return {kern::formula([](Float occluded, Float s) -> Float { return ((occluded != 0.0) ? (s * 0.18) : s); }, kern::to<Float>(occluded), s)};
  }
};
