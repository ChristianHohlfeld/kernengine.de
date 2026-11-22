struct game_traffic_cpp : virtual kern::Library, virtual city_stream {
  // ---- functions ----
  struct lane_distanceResult { Float Result = 0.0; };
  KERN_PURE lane_distanceResult lane_distance(const Lane& l, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float best = 0.0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    best = 1000000000.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(l.points))), 0.5))) {
      a = l.points[i];
      b = l.points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
      d = vector_sub(b, a);
      l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
      t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 0.0) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
      best = kern::formula([](Float best, Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::min(best, kern::math::sqrt((((x - x_2) - (x_3 * t)) * ((x - x_2) - (x_3 * t))) + (((y - y_2) - (y_3 * t)) * ((y - y_2) - (y_3 * t))))); }, best, vector_break(p).x, vector_break(a).x, vector_break(d).x, t, vector_break(p).y, vector_break(a).y, vector_break(d).y);
      i += 1;
    }
    return {best};
  }
};
