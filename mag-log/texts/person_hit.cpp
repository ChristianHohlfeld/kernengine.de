struct person_hit : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(BodyPart, none, legs, torso, head);

  // ---- variables ----
  KERN_CONST Float kHipZ = 0.92;
  KERN_CONST Float kHeadUp = 0.73;
  KERN_CONST Float kHeadR = 0.155;

  // ---- functions ----
  struct leanResult { Float Result = 0.0; };
  /// The body's forward lean (people_art.cpp: the running lean).
  KERN_PURE leanResult lean(Float speed = 0.0) {
    return {kern::formula([](Float a) -> Float { return (a * 0.035); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float speed) -> Float { return (speed - 2.5); }, speed), 0.0, 5.0))};
  }

  struct box_entryResult { Bool Result = false; };
  /// Entry of the segment from + seg * t (t in 0..1) into a box (centre c, half extents h) whose axes are ax, ay, az; false when it misses.
  box_entryResult box_entry(Float& t_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector ax = Vector{0.0, 0.0, 0.0}, Vector ay = Vector{0.0, 0.0, 0.0}, Vector az = Vector{0.0, 0.0, 0.0}, Vector h = Vector{0.0, 0.0, 0.0}) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 3> po = Fixed<Float, 3>{};
    Fixed<Float, 3> pd = Fixed<Float, 3>{};
    Fixed<Float, 3> hh = Fixed<Float, 3>{};
    Float t0 = 0.0;
    Float t1 = 0.0;
    Bool skip = false;
    Float ta = 0.0;
    Float tb = 0.0;
    Float swapped = 0.0;
    o = vector_sub(from, c);
    po = list_make<Float, "3">(vector_dot(o, ax), vector_dot(o, ay), vector_dot(o, az));
    pd = list_make<Float, "3">(vector_dot(seg, ax), vector_dot(seg, ay), vector_dot(seg, az));
    hh = list_make<Float, "3">(vector_break(h).x, vector_break(h).y, vector_break(h).z);
    t0 = 0.0;
    t1 = 1.0;
    for (auto it10 = flow_for(n10, 0, 2); it10.next();) {
      skip = false;
      if (math_compare<"<">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, pd[n10.index]), 1e-07)) {
        if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, po[n10.index]), hh[n10.index])) {
          return {false};
        }
        skip = true;
      }
      if (logic_not(skip)) {
        ta = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::div(((-a) - a_2), a_3); }, hh[n10.index], po[n10.index], pd[n10.index]);
        tb = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::div((a - a_2), a_3); }, hh[n10.index], po[n10.index], pd[n10.index]);
        if (math_compare<">">(ta, tb)) {
          swapped = ta;
          ta = tb;
          tb = swapped;
        }
        t0 = kern::formula([](Float t0, Float ta) -> Float { return kern::math::max(t0, ta); }, t0, ta);
        t1 = kern::formula([](Float t1, Float tb) -> Float { return kern::math::min(t1, tb); }, t1, tb);
        if (math_compare<">">(t0, t1)) {
          return {false};
        }
      }
    }
    t_out = t0;
    return {true};
  }

  struct sphere_entryResult { Bool Result = false; };
  sphere_entryResult sphere_entry(Float& t_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Float a = 0.0;
    Float b = 0.0;
    Float cc = 0.0;
    Float disc = 0.0;
    Float t = 0.0;
    o = vector_sub(from, c);
    a = vector_dot(seg, seg);
    b = vector_dot(o, seg);
    cc = kern::formula([](Float a, Float r) -> Float { return (a - (r * r)); }, vector_dot(o, o), r);
    if (math_compare<"<">(a, 1e-09)) {
      return {false};
    }
    if (math_compare<"<=">(cc, 0.0)) {
      t_out = 0.0;
      return {true};
    }
    disc = kern::formula([](Float b, Float a, Float cc) -> Float { return ((b * b) - (a * cc)); }, b, a, cc);
    if (logic_or(math_compare<"<">(disc, 0.0), math_compare<">">(b, 0.0))) {
      return {false};
    }
    t = kern::formula([](Float b, Float disc, Float a) -> Float { return kern::math::div(((-b) - kern::math::sqrt(disc)), a); }, b, disc, a);
    if (math_compare<">">(t, 1.0)) {
      return {false};
    }
    t_out = kern::formula([](Float t) -> Float { return kern::math::max(0.0, t); }, t);
    return {true};
  }

  struct hitResult { BodyPart Result = BodyPart::none; };
  /// The part of a standing person (feet at `feet`, heading `yaw` rad - forward is (cos, sin) - walking at `speed` m/s, `scale` 1 for an adult) the segment from + seg enters first.
  hitResult hit(Float& t_out, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Float pad = 0.0, Float scale = 0.0) {
    Float cy = 0.0;
    Float sy = 0.0;
    Float l = 0.0;
    Float cl = 0.0;
    Float sl = 0.0;
    Vector fx = Vector{0.0, 0.0, 0.0};
    Vector fy = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector bz = Vector{0.0, 0.0, 0.0};
    Vector bx = Vector{0.0, 0.0, 0.0};
    Vector hip = Vector{0.0, 0.0, 0.0};
    BodyPart best = BodyPart::none;
    Float bt = 0.0;
    Float t = 0.0;
    cy = kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw);
    sy = kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw);
    l = lean(speed).Result;
    cl = kern::formula([](Float l) -> Float { return kern::math::cos(l); }, l);
    sl = kern::formula([](Float l) -> Float { return kern::math::sin(l); }, l);
    fx = vector_make(cy, sy, 0.0);
    fy = vector_make(kern::formula([](Float sy) -> Float { return (-sy); }, sy), cy, 0.0);
    up = Vector{0.0, 0.0, 1.0};
    bz = vector_add(vector_scale(up, cl), vector_scale(fx, sl));
    bx = vector_sub(vector_scale(fx, cl), vector_scale(up, sl));
    hip = vector_add(feet, vector_scale(up, kern::formula([](Float kHipZ, Float scale) -> Float { return (kHipZ * scale); }, kHipZ, scale)));
    best = BodyPart::none;
    bt = 2.0;
    t = 0.0;
    n11 = sphere_entry(t, from, seg, vector_add(hip, vector_scale(bz, kern::formula([](Float kHeadUp, Float scale) -> Float { return (kHeadUp * scale); }, kHeadUp, scale))), kern::formula([](Float kHeadR, Float pad, Float scale) -> Float { return ((kHeadR * pad) * scale); }, kHeadR, pad, scale));
    if (logic_and(n11.Result, math_compare<"<">(t, bt))) {
      bt = t;
      best = BodyPart::head;
    }
    n12 = box_entry(t, from, seg, vector_add(hip, vector_scale(bz, kern::formula([](Float scale) -> Float { return (0.29 * scale); }, scale))), bx, fy, bz, vector_scale(Vector{0.15, 0.31, 0.31}, kern::formula([](Float pad, Float scale) -> Float { return (pad * scale); }, pad, scale)));
    if (logic_and(n12.Result, math_compare<"<">(t, kern::formula([](Float bt) -> Float { return (bt - 1e-05); }, bt)))) {
      bt = t;
      best = BodyPart::torso;
    }
    n13 = box_entry(t, from, seg, vector_add(feet, vector_scale(up, kern::formula([](Float scale) -> Float { return (0.47 * scale); }, scale))), fx, fy, up, vector_scale(Vector{0.15, 0.2, 0.47}, kern::formula([](Float pad, Float scale) -> Float { return (pad * scale); }, pad, scale)));
    if (logic_and(n13.Result, math_compare<"<">(t, kern::formula([](Float bt) -> Float { return (bt - 1e-05); }, bt)))) {
      bt = t;
      best = BodyPart::legs;
    }
    t_out = bt;
    return {best};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  sphere_entryResult n11;
  box_entryResult n12;
  box_entryResult n13;
};
