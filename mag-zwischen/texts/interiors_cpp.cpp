struct interiors_cpp : virtual kern::Library, virtual city_stream_cpp, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual interiors, virtual life_art_cpp {
  // ---- types ----
  KERN_STRUCT(Interiors_doors_of_Edge) { Float len = 0.0; Vector a = Vector{0.0, 0.0, 0.0}; Vector d = Vector{0.0, 0.0, 0.0}; Vector n = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(CoreFit) { Bool ok = false; Interiors_Core core = Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; };

  // ---- variables ----
  KERN_CONST Float kCoreL = 6.2;
  KERN_CONST Float kCoreW = 2.6;
  /// building_interiors.mjs
  KERN_CONST Float kDoorW = 1.8;
  KERN_CONST Float kSlab = 0.25;
  KERN_CONST Float kRoofHouse = 2.7;
  /// building_interiors.mjs
  KERN_CONST Float kDoorsRadius = 95.0;
  /// building_interiors.mjs
  KERN_CONST Float kDoorH = 2.55;
  KERN_CONST Float kLand = 1.3;

  // ---- functions ----
  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(Int h = 0) {
    return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(h)))};
  }

  struct signed_areaResult { Float Result = 0.0; };
  KERN_PURE signed_areaResult signed_area(const List<Vector>& r) {
    Float a = 0.0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    a = 0.0;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      a += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return ((x * y) - (x_2 * y_2)); }, vector_break(r[j]).x, vector_break(r[i]).y, vector_break(r[i]).x, vector_break(r[j]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula([](Float a) -> Float { return (a * 0.5); }, a)};
  }

  struct cleanResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE cleanResult clean(const List<Vector>& in) {
    List<Vector> r = List<Vector>{};
    Bool skip = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    r = List<Vector>{};
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(in))))); it10.next();) {
      p = in[n10.index];
      skip = false;
      if (logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).x), 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).y), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector>(r)), 0.0)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))))]).x, vector_break(p).x, vector_break(r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))))]).y, vector_break(p).y), 0.05))) {
          skip = true;
        }
        if (logic_not(skip)) {
          list_add(r, p);
        }
      }
    }
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<Vector>(r)), 2.0), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(r[0]).x, vector_break(r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))))]).x, vector_break(r[0]).y, vector_break(r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))))]).y), 0.05))) {
      list_remove_at(r, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(r)))));
    }
    return {r};
  }

  struct pipResult { Bool Result = false; };
  KERN_PURE pipResult pip(const List<Vector>& r, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool c = false;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    c = false;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      a = r[i];
      b = r[j];
      if (logic_and(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(a).y, vector_break(p).y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(b).y, vector_break(p).y))))), math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float x_2, Float y, Float y_2, Float y_3) -> Float { return (kern::math::div(((x - x_2) * (y - y_2)), (((y_3 - y_2) == 0.0) ? 1e-09 : (y_3 - y_2))) + x_2); }, vector_break(b).x, vector_break(a).x, vector_break(p).y, vector_break(a).y, vector_break(b).y)))) {
        c = logic_not(c);
      }
      was = i;
      i += 1;
      j = was;
    }
    return {c};
  }

  struct seg_distResult { Float Result = 0.0; };
  KERN_PURE seg_distResult seg_dist(Vector p = Vector{0.0, 0.0, 0.0}, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float t = 0.0;
    d = vector_sub(b, a);
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-09, ((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y, l), 0.0, 1.0));
    return {kern::formula([](Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x - x_2) - (x_3 * t)) * ((x - x_2) - (x_3 * t))) + (((y - y_2) - (y_3 * t)) * ((y - y_2) - (y_3 * t)))); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, t, vector_break(p).y, vector_break(a).y, vector_break(d).y)};
  }

  struct ring_distResult { Float Result = 0.0; };
  KERN_PURE ring_distResult ring_dist(const List<Vector>& r, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float d = 0.0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    d = 1000000000.0;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      d = kern::formula([](Float d, Float the_result) -> Float { return kern::math::min(d, the_result); }, d, seg_dist(p, r[j], r[i]).Result);
      was = i;
      i += 1;
      j = was;
    }
    return {d};
  }

  struct offset_ringResult { List<Vector> Result = List<Vector>{}; };
  /// Offset to the LEFT of the travel direction (inside of a CCW outer ring, solid side of a CW hole ring) with clamped miters (building_interiors.mjs offsetRing).
  KERN_PURE offset_ringResult offset_ring(const List<Vector>& r, Float d = 0.0) {
    Int n = 0;
    List<Vector> out = List<Vector>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float la = 0.0;
    Float lb = 0.0;
    Vector the_n0 = Vector{0.0, 0.0, 0.0};
    Vector the_n1 = Vector{0.0, 0.0, 0.0};
    Float dot = 0.0;
    Float k = 0.0;
    n = list_length<Vector>(r);
    out = List<Vector>{};
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it11.next();) {
      p = r[kern::to<Int>(kern::formula([](Float index, Float n) -> Float { return kern::math::mod(((index + n) - 1.0), n); }, kern::to<Float>(n11.index), kern::to<Float>(n)))];
      c = r[n11.index];
      q = r[kern::to<Int>(kern::formula([](Float index, Float n) -> Float { return kern::math::mod((index + 1.0), n); }, kern::to<Float>(n11.index), kern::to<Float>(n)))];
      a = vector_sub(c, p);
      b = vector_sub(q, c);
      la = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, kern::math::sqrt((x * x) + (y * y))); }, vector_break(a).x, vector_break(a).y);
      lb = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, kern::math::sqrt((x * x) + (y * y))); }, vector_break(b).x, vector_break(b).y);
      a = vector_scale(a, kern::formula([](Float la) -> Float { return kern::math::div(1.0, la); }, la));
      b = vector_scale(b, kern::formula([](Float lb) -> Float { return kern::math::div(1.0, lb); }, lb));
      the_n0 = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(a).y), vector_break(a).x, 0.0);
      the_n1 = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(b).y), vector_break(b).x, 0.0);
      dot = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(the_n0).x, vector_break(the_n1).x, vector_break(the_n0).y, vector_break(the_n1).y);
      k = kern::formula([](Float dot) -> Float { return kern::math::div(1.0, kern::math::max(0.25, (1.0 + dot))); }, dot);
      list_add(out, vector_add(c, vector_scale(vector_add(the_n0, the_n1), kern::formula([](Float k, Float d) -> Float { return (k * d); }, k, d))));
    }
    return {out};
  }

  struct hash_keyResult { Int Result = 0; };
  KERN_PURE hash_keyResult hash_key(Int seed = 0, Int salt = 0) {
    Int x = 0;
    x = kern::to<Int>(kern::formula([](Float seed, Float salt) -> Float { return kern::math::wrap32(kern::math::mul32(seed, 2654435761.0) + kern::math::mul32(salt, 2246822507.0)); }, kern::to<Float>(seed), kern::to<Float>(salt)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2146121005.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 15.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2221713035.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    return {x};
  }

  struct h01Result { Float Result = 0.0; };
  KERN_PURE h01Result h01(Int seed = 0, Int salt = 0) {
    return {kern::formula([](Float a) -> Float { return kern::math::div(kern::math::shr(a, 8.0), 16777216.0); }, kern::to<Float>(hash_key(seed, salt).Result))};
  }

  /// ---- geometry sinks: the triangles of a plan - drawn (its mesh) and for collision (its points and indices)
  void vtri(Interiors_Plan& P, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector col = Vector{0.0, 0.0, 0.0}) {
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 3> items = Fixed<Vector, 3>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    n = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(vector_sub(b, a), vector_sub(c, a))), 9.999999717180685e-10), vector_normalize(vector_cross(vector_sub(b, a), vector_sub(c, a))), Vector{0.0, 0.0, 1.0});
    item = a;
    item_2 = b;
    item_3 = c;
    items = list_make<Vector, "3">(item, item_2, item_3);
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items))))); it12.next();) {
      p = items[n12.index];
      engine_geo_mesh_data::MeshData_push(P.mesh, p, n, col, vector_make(kern::formula([](Float x, Float z) -> Float { return ((x * 0.5) + (z * 0.3)); }, vector_break(p).x, vector_break(p).z), kern::formula([](Float y) -> Float { return (y * 0.5); }, vector_break(p).y), 0.0));
    }
  }

  void vquad(Interiors_Plan& P, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector d = Vector{0.0, 0.0, 0.0}, Vector col = Vector{0.0, 0.0, 0.0}) {
    vtri(P, a, b, c, col);
    vtri(P, a, c, d, col);
  }

  void ctri(Interiors_Plan& P, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}) {
    Int base = 0;
    Int place = 0;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Int place_2 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    if (math_compare<"<">(vector_length(vector_cross(vector_sub(b, a), vector_sub(c, a))), 0.0001)) {
      return;
    }
    base = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(P.points))));
    place = list_length<Vector>(P.points);
    item = a;
    item_2 = b;
    item_3 = c;
    list_insert_list(P.points, place, list_make<Vector, "3">(item, item_2, item_3));
    place_2 = list_length<Int>(P.indices);
    item_4 = base;
    item_5 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
    item_6 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    list_insert_list(P.indices, place_2, list_make<Int, "3">(item_4, item_5, item_6));
  }

  void cquad(Interiors_Plan& P, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector d = Vector{0.0, 0.0, 0.0}, Bool two_sided = false) {
    ctri(P, a, b, c);
    ctri(P, a, c, d);
    if (two_sided) {
      ctri(P, a, c, b);
      ctri(P, a, d, c);
    }
  }

  /// a vertical wall piece between two ground points
  void wall(Interiors_Plan& P, Vector p0 = Vector{0.0, 0.0, 0.0}, Vector p1 = Vector{0.0, 0.0, 0.0}, Float z0 = 0.0, Float z1 = 0.0, Vector col = Vector{0.0, 0.0, 0.0}, Bool visual = false, Bool collide = false) {
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    if (logic_or(math_compare<"<">(kern::formula([](Float z1, Float z0) -> Float { return (z1 - z0); }, z1, z0), 0.01), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(p1).x, vector_break(p0).x, vector_break(p1).y, vector_break(p0).y), 0.01))) {
      return;
    }
    a = vector_make(vector_break(p0).x, vector_break(p0).y, z0);
    b = vector_make(vector_break(p1).x, vector_break(p1).y, z0);
    c = vector_make(vector_break(p1).x, vector_break(p1).y, z1);
    d = vector_make(vector_break(p0).x, vector_break(p0).y, z1);
    if (visual) {
      vquad(P, a, b, c, d, col);
    }
    if (collide) {
      cquad(P, a, b, c, d, true);
    }
  }

  /// a horizontal polygon with holes
  void slab(Interiors_Plan& P, const List<Vector>& outer, const List<Vector_row>& holes, Float z = 0.0, Vector col = Vector{0.0, 0.0, 0.0}, Bool up = false, Bool visual = false, Bool collide = false) {
    List<Float> xy = List<Float>{};
    List<Int> hs = List<Int>{};
    List<Int> tris = List<Int>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Bool skip = false;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Int place_2 = 0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Int ec = 0;
    Int Earcut_2 = 0;
    Int i = 0;
    Fixed<Vector, 3> p_3 = Fixed<Vector, 3>{};
    Vector swapped = Vector{0.0, 0.0, 0.0};
    xy = List<Float>{};
    hs = List<Int>{};
    tris = List<Int>{};
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(outer))))); it13.next();) {
      p = outer[n13.index];
      place = list_length<Float>(xy);
      item = vector_break(p).x;
      item_2 = vector_break(p).y;
      list_insert_list(xy, place, list_make<Float, "2">(item, item_2));
    }
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(holes))))); it14.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(holes[n14.index]).items)), 3.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_add(hs, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))));
        for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(holes[n14.index].items))))); it15.next();) {
          p_2 = holes[n14.index].items[n15.index];
          place_2 = list_length<Float>(xy);
          item_3 = vector_break(p_2).x;
          item_4 = vector_break(p_2).y;
          list_insert_list(xy, place_2, list_make<Float, "2">(item_3, item_4));
        }
      }
    }
    if (math_compare<"<">(kern::to<Float>(list_length<Float>(xy)), 6.0)) {
      return;
    }
    engine_earcut_make(n16);
    Earcut_2 = n16.result;
    ec = Earcut_2;
    engine_earcut_triangulate(n17, ec, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))), hs, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Int>(hs)))), tris);
    tris = n17.out;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Int>(tris))), 0.5))) {
      p_3 = List<Vector>{};
      for (auto it18 = flow_for(n18, 0, 2); it18.next();) {
        p_3[n18.index] = vector_make(xy[kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::mul32(a, 2.0); }, kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i, Float index) -> Float { return (i + index); }, kern::to<Float>(i), kern::to<Float>(n18.index)))])))], xy[kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 2.0) + 1.0); }, kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i, Float index) -> Float { return (i + index); }, kern::to<Float>(i), kern::to<Float>(n18.index)))])))], z);
      }
      if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(math_compare<"<">(vector_break(vector_cross(vector_sub(p_3[1], p_3[0]), vector_sub(p_3[2], p_3[0]))).z, 0.0)))), kern::to<Float>(value_int(kern::to<Int>(up))))) {
        swapped = p_3[1];
        p_3[1] = p_3[2];
        p_3[2] = swapped;
      }
      if (visual) {
        vtri(P, p_3[0], p_3[1], p_3[2], col);
      }
      if (collide) {
        ctri(P, p_3[0], p_3[1], p_3[2]);
      }
      i += 3;
    }
    if (math_compare<"!=">(kern::to<Float>(ec), 0.0)) {
      engine_earcut_release(ec);
    }
    ec = 0;
  }

  void box(Interiors_Plan& P, Vector c = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector col = Vector{0.0, 0.0, 0.0}, Bool collide = false) {
    Vector h = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Fixed<Vector, 8> v = Fixed<Vector, 8>{};
    Fixed<Int_4_row, 6> f = Fixed<Int_4_row, 6>{};
    h = vector_scale(size, 0.5);
    q = common::yaw_quat(yaw).Result;
    v = List<Vector>{};
    for (auto it19 = flow_for(n19, 0, 7); it19.next();) {
      v[n19.index] = vector_add(c, engine_math_math::Quat_rotate(q, vector_make(kern::formula([](Float index, Float x) -> Float { return ((kern::math::band(index, 1.0) != 0.0) ? x : (-x)); }, kern::to<Float>(n19.index), vector_break(h).x), kern::formula([](Float index, Float y) -> Float { return ((kern::math::band(index, 2.0) != 0.0) ? y : (-y)); }, kern::to<Float>(n19.index), vector_break(h).y), kern::formula([](Float index, Float z) -> Float { return ((kern::math::band(index, 4.0) != 0.0) ? z : (-z)); }, kern::to<Float>(n19.index), vector_break(h).z))).Result);
    }
    f = list_make<Int_4_row, "6">(struct_make<Int_4_row>(list_make<Int, "4">(4, 5, 7, 6)), struct_make<Int_4_row>(list_make<Int, "4">(0, 2, 3, 1)), struct_make<Int_4_row>(list_make<Int, "4">(0, 1, 5, 4)), struct_make<Int_4_row>(list_make<Int, "4">(2, 6, 7, 3)), struct_make<Int_4_row>(list_make<Int, "4">(1, 3, 7, 5)), struct_make<Int_4_row>(list_make<Int, "4">(0, 4, 6, 2)));
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_4_row>(f))))); it20.next();) {
      vquad(P, v[f[n20.index].items[0]], v[f[n20.index].items[1]], v[f[n20.index].items[2]], v[f[n20.index].items[3]], col);
      if (logic_and(collide, math_compare<"!=">(kern::to<Float>(f[n20.index].items[0]), 0.0))) {
        cquad(P, v[f[n20.index].items[0]], v[f[n20.index].items[1]], v[f[n20.index].items[2]], v[f[n20.index].items[3]], false);
      }
    }
  }

  struct find_core_matchResult { Bool Result = false; };
  KERN_PURE find_core_matchResult find_core_match(const Float_Vector_pair& a, const Float_Vector_pair& b) {
    return {math_compare<"<">(a.first, b.first)};
  }

  struct find_core_ok_pointResult { Bool Result = false; };
  KERN_PURE find_core_ok_pointResult find_core_ok_point(const List<Vector>& inner, const List<Vector_row>& inner_holes, Vector p = Vector{0.0, 0.0, 0.0}, Float clear = 0.0) {
    if (logic_or(logic_not(pip(inner, p).Result), math_compare<"<">(ring_dist(inner, p).Result, clear))) {
      return {false};
    }
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(inner_holes))))); it22.next();) {
      if (logic_or(pip(inner_holes[n22.index].items, p).Result, math_compare<"<">(ring_dist(inner_holes[n22.index].items, p).Result, clear))) {
        return {false};
      }
    }
    return {true};
  }

  struct find_core_PResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE find_core_PResult find_core_P(Vector o = Vector{0.0, 0.0, 0.0}, Vector u = Vector{0.0, 0.0, 0.0}, Vector v = Vector{0.0, 0.0, 0.0}, Float a = 0.0, Float b = 0.0) {
    return {vector_add(vector_add(o, vector_scale(u, a)), vector_scale(v, b))};
  }

  struct find_coreResult { CoreFit Result = CoreFit{false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}}; };
  KERN_PURE find_coreResult find_core(const List<Vector>& inner, const List<Vector_row>& inner_holes, const List<Vector_row>& holes, const List<Interiors_Door>& doors, Vector centroid = Vector{0.0, 0.0, 0.0}, Vector axis = Vector{0.0, 0.0, 0.0}) {
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float span = 0.0;
    Float step = 0.0;
    List<Float_Vector_pair> cands = List<Float_Vector_pair>{};
    Float x = 0.0;
    Float y = 0.0;
    Fixed<Vector, 4> dirs = Fixed<Vector, 4>{};
    Int i = 0;
    Bool skip = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool near_door = false;
    Bool skip_2 = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector o = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 7> pts = Fixed<Vector, 7>{};
    Bool all = false;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    List<Vector> rect = List<Vector>{};
    Bool hole_in = false;
    Vector p_3 = Vector{0.0, 0.0, 0.0};
    x0 = 1000000000.0;
    y0 = 1000000000.0;
    x1 = -1000000000.0;
    y1 = -1000000000.0;
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(inner))))); it21.next();) {
      p = inner[n21.index];
      x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(p).x);
      x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(p).x);
      y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(p).y);
      y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(p).y);
    }
    span = kern::formula([](Float x1, Float x0, Float y1, Float y0) -> Float { return kern::math::max((x1 - x0), (y1 - y0)); }, x1, x0, y1, y0);
    step = kern::formula([](Float span) -> Float { return kern::math::max(0.8, kern::math::div(span, 18.0)); }, span);
    cands = List<Float_Vector_pair>{};
    x = x0;
    while (loop(math_compare<"<=">(x, x1))) {
      y = y0;
      while (loop(math_compare<"<=">(y, y1))) {
        list_add(cands, struct_make<Float_Vector_pair>(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, x, vector_break(centroid).x, y, vector_break(centroid).y), vector_make(x, y, 0.0)));
        y += step;
      }
      x += step;
    }
    list_sort<&interiors_cpp::find_core_match>(*this, cands);
    dirs = list_make<Vector, "4">(axis, vector_scale(axis, -1.0), vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(axis).y), vector_break(axis).x, 0.0), vector_make(vector_break(axis).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(axis).x), 0.0));
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float_Vector_pair>(cands))), 0.5), math_compare<">">(kern::formula([](Float i) -> Float { return ((i < 0.0) != (500.0 < 0.0) ? (500.0 < 0.0) : (i < 500.0)); }, kern::to<Float>(i)), 0.5)))) {
      skip = false;
      c = cands[i].second;
      near_door = false;
      for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(doors))))); it23.next();) {
        near_door = logic_or(near_door, math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(doors[n23.index].c).x, vector_break(c).x, vector_break(doors[n23.index].c).y, vector_break(c).y), kern::formula([](Float kCoreL) -> Float { return ((kCoreL * 0.5) + 3.2); }, kCoreL)));
      }
      if (near_door) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(dirs))))); it24.next();) {
          v = dirs[n24.index];
          skip_2 = false;
          u = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(v).y), vector_break(v).x, 0.0);
          o = vector_sub(vector_sub(c, vector_scale(u, kern::formula([](Float kCoreW) -> Float { return kern::math::div(kCoreW, 2.0); }, kCoreW))), vector_scale(v, kern::formula([](Float kCoreL) -> Float { return kern::math::div(kCoreL, 2.0); }, kCoreL)));
          pts = list_make<Vector, "7">(find_core_P(o, u, v, 0.0, 0.0).Result, find_core_P(o, u, v, kCoreW, 0.0).Result, find_core_P(o, u, v, 0.0, kCoreL).Result, find_core_P(o, u, v, kCoreW, kCoreL).Result, find_core_P(o, u, v, kern::formula([](Float kCoreW) -> Float { return kern::math::div(kCoreW, 2.0); }, kCoreW), kern::formula([](Float kCoreL) -> Float { return kern::math::div(kCoreL, 2.0); }, kCoreL)).Result, find_core_P(o, u, v, 0.0, kern::formula([](Float kCoreL) -> Float { return kern::math::div(kCoreL, 2.0); }, kCoreL)).Result, find_core_P(o, u, v, kCoreW, kern::formula([](Float kCoreL) -> Float { return kern::math::div(kCoreL, 2.0); }, kCoreL)).Result);
          all = true;
          for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it25.next();) {
            p_2 = pts[n25.index];
            all = logic_and(all, find_core_ok_point(inner, inner_holes, p_2, 0.12).Result);
          }
          if (logic_or(logic_not(all), logic_not(find_core_ok_point(inner, inner_holes, find_core_P(o, u, v, kern::formula([](Float kCoreW) -> Float { return kern::math::div(kCoreW, 2.0); }, kCoreW), -1.4).Result, 0.7).Result))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            rect = list_make<Vector, "4">(find_core_P(o, u, v, 0.0, 0.0).Result, find_core_P(o, u, v, kCoreW, 0.0).Result, find_core_P(o, u, v, kCoreW, kCoreL).Result, find_core_P(o, u, v, 0.0, kCoreL).Result);
            hole_in = false;
            for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(holes))))); it26.next();) {
              for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(holes[n26.index].items))))); it27.next();) {
                p_3 = holes[n26.index].items[n27.index];
                hole_in = logic_or(hole_in, pip(rect, p_3).Result);
              }
            }
            if (hole_in) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              return {struct_make<CoreFit>(true, struct_make<Interiors_Core>(o, u, v))};
            }
          }
        }
      }
      i += 1;
    }
    return {struct_make<CoreFit>(false, struct_make<Interiors_Core>(Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}))};
  }

  struct door_gapResult { Bool Result = false; };
  /// door gap [t0, t1] along the edge a->b (metres from a) for doors on it, else none
  door_gapResult door_gap(const List<Interiors_Door>& doors, Float& t0, Float& t1, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float inset = 0.0) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector u = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    d = vector_sub(b, a);
    len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
    if (math_compare<"<">(len, 0.001)) {
      return {false};
    }
    u = vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(doors))))); it28.next();) {
      skip = false;
      c = vector_sub(doors[n28.index].c, vector_scale(doors[n28.index].n, inset));
      if (math_compare<">">(seg_dist(c, a, b).Result, kern::formula([](Float inset) -> Float { return (0.15 + (inset * 0.2)); }, inset))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::abs(((x * x_2) + (y * y_2))); }, vector_break(u).x, vector_break(doors[n28.index].d).x, vector_break(u).y, vector_break(doors[n28.index].d).y), 0.9)) {
          skip = true;
        }
        if (logic_not(skip)) {
          t = kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(c).x, vector_break(a).x, vector_break(u).x, vector_break(c).y, vector_break(a).y, vector_break(u).y);
          t0 = kern::formula([](Float t, Float kDoorW) -> Float { return (t - kern::math::div(kDoorW, 2.0)); }, t, kDoorW);
          t1 = kern::formula([](Float t, Float kDoorW) -> Float { return (t + kern::math::div(kDoorW, 2.0)); }, t, kDoorW);
          return {true};
        }
      }
    }
    return {false};
  }

  struct doors_of_matchResult { Bool Result = false; };
  KERN_PURE doors_of_matchResult doors_of_match(const Interiors_doors_of_Edge& p, const Interiors_doors_of_Edge& q) {
    return {math_compare<">">(p.len, q.len)};
  }

  struct doors_of_mkResult { Interiors_Door Result = Interiors_Door{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE doors_of_mkResult doors_of_mk(const Interiors_doors_of_Edge& e) {
    return {struct_make<Interiors_Door>(vector_add(e.a, vector_scale(e.d, kern::formula([](Float len) -> Float { return kern::math::div(len, 2.0); }, e.len))), e.d, e.n)};
  }

  struct Interiors_doors_ofResult { List<Interiors_Door> Result = List<Interiors_Door>{}; };
  KERN_PURE Interiors_doors_ofResult Interiors_doors_of(const List<Vector>& outer, Float area = 0.0) {
    List<Interiors_doors_of_Edge> edges = List<Interiors_doors_of_Edge>{};
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    List<Interiors_Door> out = List<Interiors_Door>{};
    Int i = 0;
    Bool stop = false;
    edges = List<Interiors_doors_of_Edge>{};
    for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(outer))))); it29.next();) {
      skip = false;
      a = outer[n29.index];
      b = outer[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n29.index), kern::to<Float>(list_length<Vector>(outer))))];
      len = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y);
      if (math_compare<"<">(len, kern::formula([](Float kDoorW) -> Float { return (kDoorW + 1.4); }, kDoorW))) {
        skip = true;
      }
      if (logic_not(skip)) {
        d = vector_scale(vector_sub(b, a), kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
        list_add(edges, struct_make<Interiors_doors_of_Edge>(len, a, d, vector_make(vector_break(d).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(d).x), 0.0)));
      }
    }
    list_sort<&interiors_cpp::doors_of_match>(*this, edges);
    out = List<Interiors_Door>{};
    if (math_compare<"==">(kern::to<Float>(list_length<Interiors_doors_of_Edge>(edges)), 0.0)) {
      return {out};
    }
    list_add(out, doors_of_mk(edges[0]).Result);
    if (math_compare<">">(area, 260.0)) {
      i = 1;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Interiors_doors_of_Edge>(edges))), 0.5)))) {
        if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(edges[i].n).x, vector_break(edges[0].n).x, vector_break(edges[i].n).y, vector_break(edges[0].n).y), -0.3)) {
          list_add(out, doors_of_mk(edges[i]).Result);
          stop = true;
        }
        if (logic_not(stop)) {
          i += 1;
        }
      }
    }
    return {out};
  }

  struct build_ceil_ofResult { Float Result = 0.0; };
  KERN_PURE build_ceil_ofResult build_ceil_of(const Interiors_Plan& P, Int F = 0, Float ceil_top = 0.0, Int k = 0) {
    return {kern::formula([](Float k, Float f, Float a, Float kSlab, Float ceil_top) -> Float { return ((kern::math::wrap32(k + 1.0) < f) ? (a - kSlab) : ceil_top); }, kern::to<Float>(k), kern::to<Float>(F), P.levels[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::wrap32(k + 1.0); }, kern::to<Float>(k)))], kSlab, ceil_top)};
  }

  struct build_CPResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE build_CPResult build_CP(const Interiors_Plan& P, Float u = 0.0, Float v = 0.0) {
    return {vector_add(vector_add(P.core.o, vector_scale(P.core.u, u)), vector_scale(P.core.v, v))};
  }

  struct build_rectResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE build_rectResult build_rect(const Interiors_Plan& P, Float u0 = 0.0, Float v0 = 0.0, Float u1 = 0.0, Float v1 = 0.0) {
    List<Vector> r = List<Vector>{};
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    r = list_make<Vector, "4">(build_CP(P, u0, v0).Result, build_CP(P, u1, v0).Result, build_CP(P, u1, v1).Result, build_CP(P, u0, v1).Result);
    if (math_compare<"<">(signed_area(r).Result, 0.0)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))));
      while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
        held = list_get<Vector>(r, first);
        r[first] = list_get<Vector>(r, last);
        r[last] = held;
        first += 1;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
      }
    }
    return {r};
  }

  struct build_cwResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE build_cwResult build_cw(List<Vector> r = List<Vector>{}) {
    List<Vector> r_ = List<Vector>{};
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    r_ = r;
    if (math_compare<">">(signed_area(r_).Result, 0.0)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r_))));
      while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
        held = list_get<Vector>(r_, first);
        r_[first] = list_get<Vector>(r_, last);
        r_[last] = held;
        first += 1;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
      }
    }
    return {r_};
  }

  void Interiors_clear(Interiors& self) {
    Bool remove = false;
    Bool remove_2 = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    if (logic_and(math_compare<"!=">(kern::to<Float>(self.r_), 0.0), math_compare<"!=">(kern::to<Float>(self.mesh_id_), 0.0))) {
      engine_renderer_remove(n39, self.r_, self.mesh_id_);
      remove = n39.result;
      self.mesh_id_ = 0;
    }
    if (logic_and(math_compare<"!=">(kern::to<Float>(self.r_), 0.0), math_compare<"!=">(kern::to<Float>(self.glass_id_), 0.0))) {
      engine_renderer_remove(n40, self.r_, self.glass_id_);
      remove_2 = n40.result;
      self.glass_id_ = 0;
    }
    both = logic_and(math_compare<"!=">(kern::to<Float>(self.world_), 0.0), engine_physics_physics::Body_valid(self.body_).Result);
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, self.body_);
      both = body_valid_2;
    }
    if (both) {
      engine_world_destroy_body(self.world_, self.body_);
    }
    self.body_ = struct_make<physics_Body>(Int64{0x0});
    self.active_ = false;
    list_clear(self.closed_);
  }

  void Interiors_release(Interiors& self, CityStream& city) {
    Interiors_clear(self);
    city_stream_cpp::CityStream_exclude_building(city, Int64{0x0}, -1);
  }

  struct Interiors_insideResult { Bool Result = false; };
  KERN_PURE Interiors_insideResult Interiors_inside(const Interiors& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    if (logic_or(logic_not(self.active_), logic_not(pip(self.plan_.outer, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result))) {
      return {false};
    }
    for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(self.plan_.holes))))); it41.next();) {
      if (pip(self.plan_.holes[n41.index].items, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result) {
        return {false};
      }
    }
    return {logic_and(math_compare<">">(vector_break(p).z, kern::formula([](Float base) -> Float { return (base - 1.0); }, self.plan_.base)), math_compare<"<">(vector_break(p).z, kern::formula([](Float top, Float kRoofHouse) -> Float { return ((top + kRoofHouse) + 1.0); }, self.plan_.top, kRoofHouse)))};
  }

  struct Interiors_level_atResult { Int Result = 0; };
  KERN_PURE Interiors_level_atResult Interiors_level_at(const Interiors& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Int k = 0;
    if (logic_not(Interiors_inside(self, p).Result)) {
      return {-1};
    }
    k = 0;
    for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(self.plan_.levels))))); it42.next();) {
      if (math_compare<">=">(vector_break(p).z, kern::formula([](Float a) -> Float { return (a - 0.3); }, self.plan_.levels[n42.index]))) {
        k = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n42.index)));
      }
    }
    if (logic_and(self.plan_.roof_access, math_compare<">=">(vector_break(p).z, kern::formula([](Float top) -> Float { return (top - 0.3); }, self.plan_.top)))) {
      k = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Float>(self.plan_.levels))));
    }
    return {k};
  }

  /// local: x along the wall, y outward (the door's normal is right of the wall direction: -y here)
  void draw_unit_bx(LifeArt& art, const Mat4& M, Vector c = Vector{0.0, 0.0, 0.0}, Vector s = Vector{0.0, 0.0, 0.0}, Vector t = Vector{0.0, 0.0, 0.0}) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_3 = engine_math_math::Mat4_translate(c).Result;
    a_2 = engine_math_math::Mat4_mul(M, a_3).Result;
    a_4 = engine_math_math::Mat4_scale(s).Result;
    a = engine_math_math::Mat4_mul(a_2, a_4).Result;
    life_art_cpp::LifeArt_box(art, a, t);
  }

  /// door unit (building_interiors.mjs doorGeometries): dark frame, canopy, a light, two leaves
  void draw_unit(LifeArt& art, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float angle = 0.0, Bool open_dark = false) {
    Mat4 M = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float W = 0.0;
    Float H = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float s = 0.0;
    Mat4 L = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    M = engine_math_math::Mat4_trs(at, common::yaw_quat(yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
    W = kDoorW;
    H = kDoorH;
    draw_unit_bx(art, M, vector_make(kern::formula([](Float w) -> Float { return (kern::math::div((-w), 2.0) - 0.05); }, W), -0.04, kern::formula([](Float h) -> Float { return kern::math::div(h, 2.0); }, H)), vector_make(0.1, 0.14, H), rgb(1908514).Result);
    draw_unit_bx(art, M, vector_make(kern::formula([](Float w) -> Float { return (kern::math::div(w, 2.0) + 0.05); }, W), -0.04, kern::formula([](Float h) -> Float { return kern::math::div(h, 2.0); }, H)), vector_make(0.1, 0.14, H), rgb(1908514).Result);
    draw_unit_bx(art, M, vector_make(0.0, -0.04, kern::formula([](Float h) -> Float { return (h + 0.06); }, H)), vector_make(kern::formula([](Float w) -> Float { return (w + 0.2); }, W), 0.14, 0.12), rgb(1908514).Result);
    draw_unit_bx(art, M, vector_make(0.0, -0.6, kern::formula([](Float h) -> Float { return (h + 0.42); }, H)), vector_make(kern::formula([](Float w) -> Float { return (w + 0.9); }, W), 1.2, 0.08), rgb(2763824).Result);
    a_3 = engine_math_math::Mat4_translate(vector_make(0.0, -0.75, kern::formula([](Float h) -> Float { return (h + 0.36); }, H))).Result;
    a_2 = engine_math_math::Mat4_mul(M, a_3).Result;
    a_4 = engine_math_math::Mat4_scale(Vector{0.3, 0.08, 0.04}).Result;
    a = engine_math_math::Mat4_mul(a_2, a_4).Result;
    life_art_cpp::LifeArt_glow(art, a, Vector{1.0, 0.95, 0.8});
    if (open_dark) {
      draw_unit_bx(art, M, vector_make(0.0, -0.015, kern::formula([](Float h) -> Float { return kern::math::div(h, 2.0); }, H)), vector_make(W, 0.01, H), rgb(723982).Result);
    }
    item = -1.0;
    item_2 = 1.0;
    items = list_make<Float, "2">(item, item_2);
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it45.next();) {
      s = items[n45.index];
      a_6 = engine_math_math::Mat4_translate(vector_make(kern::formula([](Float s, Float w) -> Float { return kern::math::div((s * w), 2.0); }, s, W), -0.03, 0.0)).Result;
      a_5 = engine_math_math::Mat4_mul(M, a_6).Result;
      a_7 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, kern::formula([](Float s, Float angle) -> Float { return (s * angle); }, s, angle)).Result, Vector{1.0, 1.0, 1.0}).Result;
      L = engine_math_math::Mat4_mul(a_5, a_7).Result;
      a_10 = engine_math_math::Mat4_translate(vector_make(kern::formula([](Float s, Float w) -> Float { return kern::math::div(((-s) * w), 4.0); }, s, W), 0.0, kern::formula([](Float h) -> Float { return kern::math::div(h, 2.0); }, H))).Result;
      a_9 = engine_math_math::Mat4_mul(L, a_10).Result;
      a_11 = engine_math_math::Mat4_scale(vector_make(kern::formula([](Float w) -> Float { return (kern::math::div(w, 2.0) - 0.04); }, W), 0.05, kern::formula([](Float h) -> Float { return (h - 0.04); }, H))).Result;
      a_8 = engine_math_math::Mat4_mul(a_9, a_11).Result;
      life_art_cpp::LifeArt_box(art, a_8, rgb(1976883).Result);
    }
  }

  void Interiors_draw(const Interiors& self, LifeArt& art, Float now = 0.0, Vector eye = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Closed>(self.closed_))))); it46.next();) {
      skip = false;
      if (math_compare<">">(vector_length(vector_sub(self.closed_[n46.index].at, eye)), kern::formula([](Float kDoorsRadius) -> Float { return (kDoorsRadius + 20.0); }, kDoorsRadius))) {
        skip = true;
      }
      if (logic_not(skip)) {
        draw_unit(art, self.closed_[n46.index].at, self.closed_[n46.index].yaw, 0.0, false);
      }
    }
    if (logic_not(self.active_)) {
      return;
    }
    for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(self.plan_.doors))))); it47.next();) {
      draw_unit(art, vector_make(vector_break(self.plan_.doors[n47.index].c).x, vector_break(self.plan_.doors[n47.index].c).y, self.plan_.levels[0]), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(self.plan_.doors[n47.index].d).y, vector_break(self.plan_.doors[n47.index].d).x), self.door_angle_[n47.index], logic_and(math_compare<">">(self.door_angle_[n47.index], 0.05), logic_not(Interiors_inside(self, eye).Result)));
    }
    for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.plan_.lamps))))); it48.next();) {
      if (logic_and(math_compare<"<">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(self.plan_.lamps[n48.index]).z, vector_break(self.feet_).z), 7.0), math_compare<"<">(vector_length(vector_sub(self.plan_.lamps[n48.index], eye)), 40.0))) {
        a_2 = engine_math_math::Mat4_translate(self.plan_.lamps[n48.index]).Result;
        a_3 = engine_math_math::Mat4_scale(Vector{1.4, 0.6, 0.03}).Result;
        a = engine_math_math::Mat4_mul(a_2, a_3).Result;
        life_art_cpp::LifeArt_glow(art, a, Vector{1.0, 0.95, 0.85});
      }
    }
  }

  void Interiors_hud(const Interiors& self, Text& s) {
    Text b = "";
    b = "";
    b = text_format<",\"interior\":{\"active\":%s,\"levels\":%u,\"roof\":%s,\"doors\":%u,\"closedDoors\":%u,\"inside\":%s,\"level\":%d,\"entered\":%u,\"built\":%u,\"furniture\":%u}">(255, logic_select<Text>(self.active_, "true", "false"), kern::to<Int>(kern::formula([](Float active_, Float a) -> Float { return ((active_ != 0.0) ? kern::math::wrap32(a) : 0.0); }, kern::to<Float>(self.active_), kern::to<Float>(list_length<Float>(self.plan_.levels)))), logic_select<Text>(logic_and(self.active_, self.plan_.roof_access), "true", "false"), kern::to<Int>(kern::formula([](Float active_, Float a) -> Float { return ((active_ != 0.0) ? kern::math::wrap32(a) : 0.0); }, kern::to<Float>(self.active_), kern::to<Float>(list_length<Interiors_Door>(self.plan_.doors)))), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Interiors_Closed>(self.closed_)))), logic_select<Text>(Interiors_inside(self, self.feet_).Result, "true", "false"), Interiors_level_at(self, self.feet_).Result, self.entered_, self.built_, kern::to<Int>(kern::formula([](Float active_, Float furniture) -> Float { return ((active_ != 0.0) ? furniture : 0.0); }, kern::to<Float>(self.active_), kern::to<Float>(self.plan_.furniture))));
    s = text_append(s, b, "");
  }

  struct Interiors_commandResult { Bool Result = false; };
  Interiors_commandResult Interiors_command(Interiors& self, const Text& cmd, Text& out, List<Vector>& go, Vector me = Vector{0.0, 0.0, 0.0}) {
    Text b = "";
    Vector to = Vector{0.0, 0.0, 0.0};
    Bool roof = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    b = "";
    if (text_equal(cmd, "interior")) {
      b = text_format<"{\"active\":%s,\"inside\":%s,\"level\":%d,\"levels\":%u,\"roof\":%s,\"z\":%.2f,\"floor0\":%.2f,\"top\":%.2f}">(255, logic_select<Text>(self.active_, "true", "false"), logic_select<Text>(Interiors_inside(self, me).Result, "true", "false"), Interiors_level_at(self, me).Result, kern::to<Int>(kern::formula([](Float active_, Float a) -> Float { return ((active_ != 0.0) ? kern::math::wrap32(a) : 0.0); }, kern::to<Float>(self.active_), kern::to<Float>(list_length<Float>(self.plan_.levels)))), logic_select<Text>(logic_and(self.active_, self.plan_.roof_access), "true", "false"), vector_break(me).z, kern::formula([](Float active_, Float a) -> Float { return ((active_ != 0.0) ? a : 0.0); }, kern::to<Float>(self.active_), self.plan_.levels[0]), kern::formula([](Float active_, Float top) -> Float { return ((active_ != 0.0) ? top : 0.0); }, kern::to<Float>(self.active_), self.plan_.top));
      out = b;
      return {true};
    }
    if (logic_not(self.active_)) {
      out = "no building";
      return {true};
    }
    to = Vector{0.0, 0.0, 0.0};
    if (text_equal(cmd, "interior door")) {
      to = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 1.5)); }, vector_break(self.plan_.doors[0].c).x, vector_break(self.plan_.doors[0].n).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 1.5)); }, vector_break(self.plan_.doors[0].c).y, vector_break(self.plan_.doors[0].n).y), self.plan_.levels[0]);
    } else if (text_equal(cmd, "interior in")) {
      to = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 2.0)); }, vector_break(self.plan_.doors[0].c).x, vector_break(self.plan_.doors[0].n).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 2.0)); }, vector_break(self.plan_.doors[0].c).y, vector_break(self.plan_.doors[0].n).y), kern::formula([](Float a) -> Float { return (a + 0.05); }, self.plan_.levels[0]));
    } else if (logic_or(text_equal(cmd, "interior up"), text_equal(cmd, "interior roof"))) {
      if (logic_not(self.plan_.has_core)) {
        out = "no stairs";
        return {true};
      }
      roof = text_equal(cmd, "interior roof");
      if (logic_and(roof, logic_not(self.plan_.roof_access))) {
        out = "no roof access";
        return {true};
      }
      q = vector_add(vector_add(self.plan_.core.o, vector_scale(self.plan_.core.u, kern::formula([](Float kCoreW) -> Float { return kern::math::div(kCoreW, 2.0); }, kCoreW))), vector_scale(self.plan_.core.v, kern::formula([](Float kLand) -> Float { return kern::math::div(kLand, 2.0); }, kLand)));
      to = vector_make(vector_break(q).x, vector_break(q).y, kern::formula([](Float roof, Float top, Float a) -> Float { return (((roof != 0.0) ? top : a) + 0.05); }, kern::to<Float>(roof), self.plan_.top, self.plan_.levels[1]));
    } else {
      out = "?";
      return {true};
    }
    go = list_make<Vector, "1">(to);
    return {true};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct { Int result = 0; } n16;
  struct { List<Int> out = List<Int>{}; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct { Bool result = false; } n39;
  struct { Bool result = false; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
};
