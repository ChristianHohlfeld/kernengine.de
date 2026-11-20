struct game_lamps_cpp : virtual kern::Library, virtual engine_math_math {
  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct yaw_atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE yaw_atResult yaw_at(Vector p = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0) {
    return {engine_math_math::Mat4_trs(p, engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(Int hex = 0) {
    return {vector_make(kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 16.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 8.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(hex, 255.0), 255.0); }, kern::to<Float>(hex)))};
  }

  struct glass_pcmResult { List<Float> Result = List<Float>{}; };
  /// street_lamps.mjs glass(): white noise, cubic decay over 0.4 s, sparkles, high-passed at 3200 Hz
  KERN_PURE glass_pcmResult glass_pcm(Int rate = 0) {
    List<Float> out = List<Float>{};
    List<Float> made = List<Float>{};
    Int s = 0;
    Float prev_x = 0.0;
    Float prev_y = 0.0;
    Float rc = 0.0;
    Float dt = 0.0;
    Float a = 0.0;
    Int i = 0;
    Float r = 0.0;
    Float spark = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((rate * 0.4)); }, kern::to<Float>(rate))), 0.0);
    out = made;
    s = 2654435769;
    prev_x = 0.0;
    prev_y = 0.0;
    rc = kern::formula([](Float kPi) -> Float { return kern::math::div(1.0, ((2.0 * kPi) * 3200.0)); }, kPi);
    dt = kern::formula([](Float rate) -> Float { return kern::math::div(1.0, rate); }, kern::to<Float>(rate));
    a = kern::formula([](Float rc, Float dt) -> Float { return kern::math::div(rc, (rc + dt)); }, rc, dt);
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(out))), 0.5))) {
      s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::wrap32(kern::math::mul32(s, 1664525.0) + 1013904223.0); }, kern::to<Float>(s)));
      r = kern::formula([](Float s) -> Float { return kern::math::div(kern::math::shr(s, 8.0), 16777216.0); }, kern::to<Float>(s));
      s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::wrap32(kern::math::mul32(s, 1664525.0) + 1013904223.0); }, kern::to<Float>(s)));
      spark = kern::formula([](Float s) -> Float { return ((kern::math::div(kern::math::shr(s, 8.0), 16777216.0) < 0.04) ? 3.0 : 1.0); }, kern::to<Float>(s));
      x = kern::formula([](Float r, Float i, Float a, Float spark) -> Float { return ((((r * 2.0) - 1.0) * kern::math::pow((1.0 - kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), (a < 0.0 ? a + 18446744073709551616.0 : a))), 3.0)) * spark); }, r, kern::to<Float>(i), kern::to<Float>(list_length<Float>(out)), spark);
      y = kern::formula([](Float a, Float prev_y, Float x, Float prev_x) -> Float { return (a * ((prev_y + x) - prev_x)); }, a, prev_y, x, prev_x);
      prev_x = x;
      prev_y = y;
      out[i] = y;
      i += 1;
    }
    return {out};
  }

  struct clang_pcmResult { List<Float> Result = List<Float>{}; };
  /// clang(): three triangle partials 180 / 412 / 733 Hz, .32 each, exponential decay over 1.1 s
  KERN_PURE clang_pcmResult clang_pcm(Int rate = 0) {
    List<Float> out = List<Float>{};
    List<Float> made = List<Float>{};
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Fixed<Float, 3> items = Fixed<Float, 3>{};
    Float f = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float p = 0.0;
    Float env = 0.0;
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((rate * 1.2)); }, kern::to<Float>(rate))), 0.0);
    out = made;
    item = 180.0;
    item_2 = 412.0;
    item_3 = 733.0;
    items = list_make<Float, "3">(item, item_2, item_3);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it10.next();) {
      f = items[n10.index];
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(out))), 0.5))) {
        t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
        p = kern::formula([](Float t, Float f) -> Float { return kern::math::mod((t * f), 1.0); }, t, f);
        env = kern::formula([](Float t) -> Float { return (0.32 * kern::math::exp((kern::math::log(kern::math::div(0.001, 0.32)) * kern::math::min(1.0, kern::math::div(t, 1.1))))); }, t);
        out[i] = kern::formula([](Float a, Float p, Float env) -> Float { return (a + ((1.0 - (4.0 * kern::math::abs((p - 0.5)))) * env)); }, out[i], p, env);
        i += 1;
      }
    }
    return {out};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
