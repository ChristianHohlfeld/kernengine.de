struct interiors_cpp : virtual kern::Library, virtual city_stream_cpp, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual interiors, virtual life_art_cpp {
  // ---- types ----
  KERN_STRUCT(Interiors_doors_of_Edge) { Float len = 0.0; Vector a = Vector{0.0, 0.0, 0.0}; Vector d = Vector{0.0, 0.0, 0.0}; Vector n = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(CoreFit) { Bool ok = false; Interiors_Core core = Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; };

  // ---- variables ----
  KERN_CONST Float kCoreL = 6.2;
  KERN_CONST Float kCoreW = 2.6;
  /// building_interiors.mjs
  KERN_CONST Float kDoorW = 1.8;
  /// building_interiors.mjs
  KERN_CONST Float kWallIn = 0.3;
  KERN_CONST U32 kMaxLevels = 12;
  KERN_CONST Float kSlab = 0.25;
  KERN_CONST Float kRoofHouse = 2.7;
  KERN_CONST Vector kWall = Vector{0.9137254901960784, 0.8941176470588236, 0.8549019607843137};
  /// building_interiors.mjs
  KERN_CONST Float kDoorH = 2.55;
  KERN_CONST Vector kTrim = Vector{0.3686274509803922, 0.24705882352941178, 0.16470588235294117};
  KERN_CONST Vector kStone = Vector{0.47843137254901963, 0.4549019607843137, 0.4196078431372549};
  KERN_CONST Vector kOak = Vector{0.5411764705882353, 0.37254901960784315, 0.22745098039215686};
  KERN_CONST Vector kCeil = Vector{0.9294117647058824, 0.9215686274509803, 0.9019607843137255};
  KERN_CONST Float kLand = 1.3;
  KERN_CONST Vector kConcrete = Vector{0.4627450980392157, 0.4627450980392157, 0.4588235294117647};
  KERN_CONST Vector kStairWall = Vector{0.8549019607843137, 0.8470588235294118, 0.8235294117647058};
  KERN_CONST Float kLane = 1.2;
  KERN_CONST Float kRun = 3.6;
  KERN_CONST Vector kAccent = Vector{0.4980392156862745, 0.6039215686274509, 0.5411764705882353};
  KERN_CONST Vector kRiser = Vector{0.6627450980392157, 0.6392156862745098, 0.6039215686274509};
  KERN_CONST Vector kDark = Vector{0.1411764705882353, 0.14901960784313725, 0.16470588235294117};
  KERN_CONST Float kPi = 3.141592653589793;
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  /// building_interiors.mjs
  KERN_CONST Float kDoorsRadius = 95.0;
  /// building_interiors.mjs
  KERN_CONST Float kActivate = 30.0;
  /// a little more than ACTIVATE: no flicker at the edge
  KERN_CONST Float kRelease = 45.0;
  KERN_CONST U32 kMaxClosedDoors = 24;

  // ---- functions ----
  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(U32 h = 0) {
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

  struct hash_keyResult { U32 Result = 0; };
  KERN_PURE hash_keyResult hash_key(U32 seed = 0, U32 salt = 0) {
    U32 x = 0;
    x = kern::to<Int>(kern::formula([](Float seed, Float salt) -> Float { return (kern::math::mul32(seed, 2654435761.0) + kern::math::mul32(salt, 2246822507.0)); }, kern::to<Float>(seed), kern::to<Float>(salt)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2146121005.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 15.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2221713035.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    return {x};
  }

  struct h01Result { Float Result = 0.0; };
  KERN_PURE h01Result h01(U32 seed = 0, U32 salt = 0) {
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
    U32 base = 0;
    Int place = 0;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Int place_2 = 0;
    U32 item_4 = 0;
    U32 item_5 = 0;
    U32 item_6 = 0;
    if (math_compare<"<">(vector_length(vector_cross(vector_sub(b, a), vector_sub(c, a))), 0.0001)) {
      return;
    }
    base = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(P.points))));
    place = list_length<Vector>(P.points);
    item = a;
    item_2 = b;
    item_3 = c;
    list_insert_list(P.points, place, list_make<Vector, "3">(item, item_2, item_3));
    place_2 = list_length<U32>(P.indices);
    item_4 = base;
    item_5 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
    item_6 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    list_insert_list(P.indices, place_2, list_make<U32, "3">(item_4, item_5, item_6));
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
    List<U32> hs = List<U32>{};
    List<U32> tris = List<U32>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Bool skip = false;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Int place_2 = 0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Object<kern::Earcut> ec = Object<kern::Earcut>{};
    Object<kern::Earcut> earcut = Object<kern::Earcut>{};
    Int i = 0;
    Fixed<Vector, 3> p_3 = Fixed<Vector, 3>{};
    Vector swapped = Vector{0.0, 0.0, 0.0};
    xy = List<Float>{};
    hs = List<U32>{};
    tris = List<U32>{};
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
    earcut = n16.result;
    ec = earcut;
    engine_earcut_triangulate(n17, ec, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))), hs, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<U32>(hs)))), tris);
    tris = n17.out;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<U32>(tris))), 0.5))) {
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
    if (object_valid<kern::Earcut>(ec)) {
      engine_earcut_release(ec);
    }
    ec = Object<kern::Earcut>{};
  }

  void box(Interiors_Plan& P, Vector c = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector col = Vector{0.0, 0.0, 0.0}, Bool collide = false) {
    Vector h = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Fixed<Vector, 8> v = Fixed<Vector, 8>{};
    Fixed<I32_4_row, 6> f = Fixed<I32_4_row, 6>{};
    h = vector_scale(size, 0.5);
    q = common::yaw_quat(yaw).Result;
    v = List<Vector>{};
    for (auto it19 = flow_for(n19, 0, 7); it19.next();) {
      v[n19.index] = vector_add(c, engine_math_math::Quat_rotate(q, vector_make(kern::formula([](Float index, Float x) -> Float { return ((kern::math::band(index, 1.0) != 0.0) ? x : (-x)); }, kern::to<Float>(n19.index), vector_break(h).x), kern::formula([](Float index, Float y) -> Float { return ((kern::math::band(index, 2.0) != 0.0) ? y : (-y)); }, kern::to<Float>(n19.index), vector_break(h).y), kern::formula([](Float index, Float z) -> Float { return ((kern::math::band(index, 4.0) != 0.0) ? z : (-z)); }, kern::to<Float>(n19.index), vector_break(h).z))).Result);
    }
    f = list_make<I32_4_row, "6">(struct_make<I32_4_row>(list_make<I32, "4">(4, 5, 7, 6)), struct_make<I32_4_row>(list_make<I32, "4">(0, 2, 3, 1)), struct_make<I32_4_row>(list_make<I32, "4">(0, 1, 5, 4)), struct_make<I32_4_row>(list_make<I32, "4">(2, 6, 7, 3)), struct_make<I32_4_row>(list_make<I32, "4">(1, 3, 7, 5)), struct_make<I32_4_row>(list_make<I32, "4">(0, 4, 6, 2)));
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32_4_row>(f))))); it20.next();) {
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
  Interiors_doors_ofResult Interiors_doors_of(const List<Vector>& outer, Float area = 0.0) {
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
  KERN_PURE build_ceil_ofResult build_ceil_of(const Interiors_Plan& P, U32 F = 0, Float ceil_top = 0.0, U32 k = 0) {
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

  struct build_matchResult { Bool Result = false; };
  KERN_PURE build_matchResult build_match(const Float_Float_pair& a, const Float_Float_pair& b) {
    return {logic_or(math_compare<"<">(a.first, b.first), logic_and(logic_not(math_compare<"<">(b.first, a.first)), math_compare<"<">(a.second, b.second)))};
  }

  struct build_match_2Result { Bool Result = false; };
  KERN_PURE build_match_2Result build_match_2(const Float_Float_pair& a, const Float_Float_pair& b) {
    return {logic_or(math_compare<"<">(a.first, b.first), logic_and(logic_not(math_compare<"<">(b.first, a.first)), math_compare<"<">(a.second, b.second)))};
  }

  void build_cwall(Interiors_Plan& P, Float u0 = 0.0, Float v0 = 0.0, Float u1 = 0.0, Float v1 = 0.0, Float za = 0.0, Float zb = 0.0, Vector col = Vector{0.0, 0.0, 0.0}) {
    wall(P, build_CP(P, u0, v0).Result, build_CP(P, u1, v1).Result, za, zb, col, true, true);
  }

  struct build_P3Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE build_P3Result build_P3(const Interiors_Plan& P, Float u = 0.0, Float v = 0.0, Float h = 0.0) {
    Vector q = Vector{0.0, 0.0, 0.0};
    q = build_CP(P, u, v).Result;
    return {vector_make(vector_break(q).x, vector_break(q).y, h)};
  }

  struct build_free_spotResult { Bool Result = false; };
  build_free_spotResult build_free_spot(Interiors_Plan& P, Vector p = Vector{0.0, 0.0, 0.0}, Float clear = 0.0) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Float u = 0.0;
    Float v = 0.0;
    if (logic_or(logic_not(pip(P.inner, p).Result), math_compare<"<">(ring_dist(P.inner, p).Result, clear))) {
      return {false};
    }
    for (auto it56 = flow_for(n56, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(P.inner_holes))))); it56.next();) {
      if (logic_or(pip(P.inner_holes[n56.index].items, p).Result, math_compare<"<">(ring_dist(P.inner_holes[n56.index].items, p).Result, clear))) {
        return {false};
      }
    }
    for (auto it57 = flow_for(n57, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(P.doors))))); it57.next();) {
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(P.doors[n57.index].c).x, vector_break(p).x, vector_break(P.doors[n57.index].c).y, vector_break(p).y), 3.2)) {
        return {false};
      }
    }
    if (P.has_core) {
      q = vector_sub(p, P.core.o);
      u = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(q).x, vector_break(P.core.u).x, vector_break(q).y, vector_break(P.core.u).y);
      v = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(q).x, vector_break(P.core.v).x, vector_break(q).y, vector_break(P.core.v).y);
      if (logic_and(logic_and(logic_and(math_compare<">">(u, -1.5), math_compare<"<">(u, kern::formula([](Float kCoreW) -> Float { return (kCoreW + 1.5); }, kCoreW))), math_compare<">">(v, -3.0)), math_compare<"<">(v, kern::formula([](Float kCoreL) -> Float { return (kCoreL + 1.5); }, kCoreL)))) {
        return {false};
      }
    }
    return {true};
  }

  struct Interiors_buildResult { Bool Result = false; };
  Interiors_buildResult Interiors_build(const MapFeatures_Building& b, Interiors_Plan& P, Float door_ground = 0.0) {
    Bool reversed = false;
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    List<Vector> h = List<Vector>{};
    Int first_2 = 0;
    Int last_2 = 0;
    Vector held_2 = Vector{0.0, 0.0, 0.0};
    Vector centroid = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float best = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    U32 seed = 0;
    Float ground_storey = 0.0;
    Float base0 = 0.0;
    Bool capped = false;
    CoreFit core = CoreFit{false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}};
    Float z = 0.0;
    Bool stop = false;
    U32 F = 0;
    Float ceil_top = 0.0;
    Float wall_top = 0.0;
    Float inset = 0.0;
    Int i = 0;
    Bool skip_2 = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector bb = Vector{0.0, 0.0, 0.0};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector u = Vector{0.0, 0.0, 0.0};
    Float g0 = 0.0;
    Float g1 = 0.0;
    Bool gap = false;
    List<Float_Float_pair> solid = List<Float_Float_pair>{};
    Float_Float_pair v = Float_Float_pair{0.0, 0.0};
    Float len_o = 0.0;
    Float to_inner = 0.0;
    Float z0 = 0.0;
    Float z1 = 0.0;
    Float zn = 0.0;
    Float row = 0.0;
    Float sill = 0.0;
    Float head = 0.0;
    List<Float_Float_pair> wins = List<Float_Float_pair>{};
    Bool rows = false;
    Float c0 = 0.0;
    Float w0 = 0.0;
    Float w1 = 0.0;
    Float t0 = 0.0;
    Float t = 0.0;
    Float_Float_pair v_2 = Float_Float_pair{0.0, 0.0};
    Vector g0_2 = Vector{0.0, 0.0, 0.0};
    Vector g1_2 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> q = Fixed<Vector, 4>{};
    Vector gn = Vector{0.0, 0.0, 0.0};
    I32 item = 0;
    I32 item_2 = 0;
    I32 item_3 = 0;
    I32 item_4 = 0;
    I32 item_5 = 0;
    I32 item_6 = 0;
    Fixed<I32, 6> items = Fixed<I32, 6>{};
    I32 v_3 = 0;
    Float inset_2 = 0.0;
    Int i_2 = 0;
    Bool skip_3 = false;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector bb_2 = Vector{0.0, 0.0, 0.0};
    Vector d_3 = Vector{0.0, 0.0, 0.0};
    Float len_2 = 0.0;
    Vector u_2 = Vector{0.0, 0.0, 0.0};
    Float g0_3 = 0.0;
    Float g1_3 = 0.0;
    Bool gap_2 = false;
    List<Float_Float_pair> solid_2 = List<Float_Float_pair>{};
    Float_Float_pair v_4 = Float_Float_pair{0.0, 0.0};
    Float len_o_2 = 0.0;
    Float to_inner_2 = 0.0;
    Float z0_2 = 0.0;
    Float z1_2 = 0.0;
    Float zn_2 = 0.0;
    Float row_2 = 0.0;
    Float sill_2 = 0.0;
    Float head_2 = 0.0;
    List<Float_Float_pair> wins_2 = List<Float_Float_pair>{};
    Bool rows_2 = false;
    Float c0_2 = 0.0;
    Float w0_2 = 0.0;
    Float w1_2 = 0.0;
    Float t0_2 = 0.0;
    Float t_2 = 0.0;
    Float_Float_pair v_5 = Float_Float_pair{0.0, 0.0};
    Vector g0_4 = Vector{0.0, 0.0, 0.0};
    Vector g1_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> q_2 = Fixed<Vector, 4>{};
    Vector gn_2 = Vector{0.0, 0.0, 0.0};
    I32 item_7 = 0;
    I32 item_8 = 0;
    I32 item_9 = 0;
    I32 item_10 = 0;
    I32 item_11 = 0;
    I32 item_12 = 0;
    Fixed<I32, 6> items_2 = Fixed<I32, 6>{};
    I32 v_6 = 0;
    Int h_2 = 0;
    List<Vector> R = List<Vector>{};
    Vector o0 = Vector{0.0, 0.0, 0.0};
    Vector o1 = Vector{0.0, 0.0, 0.0};
    Vector i0 = Vector{0.0, 0.0, 0.0};
    Vector i1 = Vector{0.0, 0.0, 0.0};
    Vector f0 = Vector{0.0, 0.0, 0.0};
    Vector f1 = Vector{0.0, 0.0, 0.0};
    List<Vector_row> floor_holes = List<Vector_row>{};
    List<Vector_row> core_holes = List<Vector_row>{};
    Float z_2 = 0.0;
    Bool which = false;
    List<Vector> a_3 = List<Vector>{};
    List<Vector_row> a_4 = List<Vector_row>{};
    List<Vector> a_5 = List<Vector>{};
    List<Vector_row> a_6 = List<Vector_row>{};
    List<Vector_row> roof_holes = List<Vector_row>{};
    List<Vector_row> a_7 = List<Vector_row>{};
    List<Vector> a_8 = List<Vector>{};
    List<Vector_row> a_9 = List<Vector_row>{};
    Float z0_3 = 0.0;
    List<Float_Float_pair> stairs = List<Float_Float_pair>{};
    U32 k = 0;
    Float_Float_pair v_7 = Float_Float_pair{0.0, 0.0};
    Float half = 0.0;
    List<Vector> a_10 = List<Vector>{};
    List<Vector_row> a_11 = List<Vector_row>{};
    List<Vector> a_12 = List<Vector>{};
    List<Vector_row> a_13 = List<Vector_row>{};
    U32 n = 0;
    Float rise = 0.0;
    Float tread = 0.0;
    Float yaw = 0.0;
    Float top_a = 0.0;
    Float top_b = 0.0;
    Vector ca = Vector{0.0, 0.0, 0.0};
    Vector cb = Vector{0.0, 0.0, 0.0};
    List<Vector> a_14 = List<Vector>{};
    List<Vector_row> a_15 = List<Vector_row>{};
    List<Vector> a_16 = List<Vector>{};
    List<Vector_row> a_17 = List<Vector_row>{};
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Float yaw0 = 0.0;
    Float z_3 = 0.0;
    Float zc = 0.0;
    U32 placed = 0;
    U32 lamps = 0;
    Float x = 0.0;
    Float y = 0.0;
    Bool skip_4 = false;
    Vector p_3 = Vector{0.0, 0.0, 0.0};
    U32 h_3 = 0;
    Bool either = false;
    Float yaw_2 = 0.0;
    U32 kind = 0;
    Int selected = 0;
    Quat a_18 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_19 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_20 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_21 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat q_3 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat q_4 = Quat{0.0, 0.0, 0.0, 1.0};
    Float item_13 = 0.0;
    Float item_14 = 0.0;
    Fixed<Float, 2> items_3 = Fixed<Float, 2>{};
    Float cx = 0.0;
    Float item_15 = 0.0;
    Float item_16 = 0.0;
    Fixed<Float, 2> items_4 = Fixed<Float, 2>{};
    Float cy = 0.0;
    Quat q_5 = Quat{0.0, 0.0, 0.0, 1.0};
    Float x_2 = 0.0;
    Float y_2 = 0.0;
    U32 ac = 0;
    Float x_3 = 0.0;
    Float y_3 = 0.0;
    Bool skip_5 = false;
    Vector q_6 = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float cu = 0.0;
    Float cv = 0.0;
    P = struct_make<Interiors_Plan>(List<Vector>{}, List<Vector>{}, List<Vector_row>{}, List<Vector_row>{}, List<Interiors_Door>{}, List<Float>{}, 0.0, 8.0, 3.3, 0.0, false, false, struct_make<Interiors_Core>(Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}), struct_make<MeshData>(List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}), struct_make<MeshData>(List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}), List<Vector>{}, List<U32>{}, List<Vector>{}, 0);
    if (math_compare<"==">(kern::to<Float>(list_length<Vector_row>(b.rings)), 0.0)) {
      return {false};
    }
    P.outer = clean(b.rings[0].items).Result;
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(P.outer)), 3.0)) {
      return {false};
    }
    reversed = math_compare<"<">(signed_area(P.outer).Result, 0.0);
    if (reversed) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(P.outer))));
      while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
        held = list_get<Vector>(P.outer, first);
        P.outer[first] = list_get<Vector>(P.outer, last);
        P.outer[last] = held;
        first += 1;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
      }
    }
    for (auto it30 = flow_for(n30, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(b.rings))))); it30.next();) {
      skip = false;
      h = clean(b.rings[n30.index].items).Result;
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(h)), 3.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<">">(signed_area(h).Result, 0.0)) {
          first_2 = 0;
          last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(h))));
          while (loop(math_compare<"<">(kern::to<Float>(first_2), kern::to<Float>(last_2)))) {
            held_2 = list_get<Vector>(h, first_2);
            h[first_2] = list_get<Vector>(h, last_2);
            h[last_2] = held_2;
            first_2 += 1;
            last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last_2)));
          }
        }
        list_add(P.holes, struct_make<Vector_row>(h));
      }
    }
    P.area = signed_area(P.outer).Result;
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(P.holes))))); it31.next();) {
      P.area = kern::formula([](Float area, Float the_result) -> Float { return (area - kern::math::abs(the_result)); }, P.area, signed_area(P.holes[n31.index].items).Result);
    }
    if (logic_or(math_compare<"<">(P.area, 24.0), math_compare<">">(P.area, 40000.0))) {
      return {false};
    }
    P.inner = offset_ring(P.outer, kWallIn).Result;
    for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(P.holes))))); it32.next();) {
      list_add(P.inner_holes, struct_make<Vector_row>(offset_ring(P.holes[n32.index].items, kWallIn).Result));
    }
    if (math_compare<"<">(signed_area(P.inner).Result, 8.0)) {
      return {false};
    }
    n33 = Interiors_doors_of(P.outer, P.area);
    P.doors = n33.Result;
    if (math_compare<"==">(kern::to<Float>(list_length<Interiors_Door>(P.doors)), 0.0)) {
      return {false};
    }
    centroid = Vector{0.0, 0.0, 0.0};
    for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(P.outer))))); it34.next();) {
      p = P.outer[n34.index];
      centroid = vector_add(centroid, vector_scale(p, kern::formula([](Float a) -> Float { return kern::math::div(1.0, (a < 0.0 ? a + 18446744073709551616.0 : a)); }, kern::to<Float>(list_length<Vector>(P.outer)))));
    }
    axis = Vector{1.0, 0.0, 0.0};
    best = 0.0;
    for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(P.outer))))); it35.next();) {
      d = vector_sub(P.outer[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n35.index), kern::to<Float>(list_length<Vector>(P.outer))))], P.outer[n35.index]);
      l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
      if (math_compare<">">(l, best)) {
        best = l;
        axis = vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l));
      }
    }
    seed = b.seed;
    P.floor_h = kern::formula([](Float seed) -> Float { return (3.1 + kern::math::div((0.5 * kern::math::mod(kern::math::shr(seed, 12.0), 1000.0)), 1000.0)); }, kern::to<Float>(seed));
    ground_storey = P.floor_h;
    base0 = kern::formula([](Float base, Float door_ground) -> Float { return (kern::math::max(base, door_ground) + 0.03); }, b.base, door_ground);
    P.base = b.base;
    P.top = b.top;
    if (math_compare<"<">(kern::formula([](Float top, Float base0) -> Float { return (top - base0); }, P.top, base0), 2.8)) {
      return {false};
    }
    list_add(P.levels, base0);
    capped = false;
    core = CoreFit{false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}};
    if (math_compare<">">(kern::formula([](Float top, Float base0) -> Float { return (top - base0); }, P.top, base0), kern::formula([](Float ground_storey) -> Float { return (ground_storey + 2.7); }, ground_storey))) {
      core = find_core(P.inner, P.inner_holes, P.holes, P.doors, centroid, axis).Result;
    }
    if (core.ok) {
      z = kern::formula([](Float base, Float ground_storey, Float base0) -> Float { return kern::math::max((base + ground_storey), (base0 + 2.7)); }, P.base, ground_storey, base0);
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<=">(kern::formula([](Float z) -> Float { return (z + 2.7); }, z), kern::formula([](Float top) -> Float { return (top - 0.05); }, P.top))))) {
        if (math_compare<">">(kern::formula([](Float a, Float kMaxLevels) -> Float { return (!((a < 0.0) != (kMaxLevels < 0.0) ? (kMaxLevels < 0.0) : (a < kMaxLevels))); }, kern::to<Float>(list_length<Float>(P.levels)), kern::to<Float>(kMaxLevels)), 0.5)) {
          capped = true;
          stop = true;
        }
        if (logic_not(stop)) {
          list_add(P.levels, z);
        }
        if (logic_not(stop)) {
          z += P.floor_h;
        }
      }
    }
    P.has_core = logic_and(core.ok, math_compare<">">(kern::to<Float>(list_length<Float>(P.levels)), 1.0));
    P.core = core.core;
    F = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(P.levels))));
    P.roof_access = logic_and(logic_and(logic_and(P.has_core, logic_not(capped)), math_compare<"<=">(kern::formula([](Float top, Float a) -> Float { return (top - a); }, P.top, P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))]), kern::formula([](Float floor_h) -> Float { return (floor_h + 1.8); }, P.floor_h))), math_compare<"<">(P.area, 12000.0));
    ceil_top = kern::formula([](Float capped, Float a, Float floor_h, Float kSlab, Float f, Float top, Float base0, Float ground_storey, Float base) -> Float { return ((capped != 0.0) ? ((a + floor_h) - kSlab) : (((f == 1.0) && ((top - base0) > (ground_storey + 0.5))) ? ((base + ground_storey) - kSlab) : (top - 0.3))); }, kern::to<Float>(capped), P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))], P.floor_h, kSlab, kern::to<Float>(F), P.top, base0, ground_storey, P.base);
    wall_top = kern::formula([](Float roof_access, Float top, Float kRoofHouse, Float ceil_top) -> Float { return ((roof_access != 0.0) ? (top + kRoofHouse) : ceil_top); }, kern::to<Float>(P.roof_access), P.top, kRoofHouse, ceil_top);
    for (auto it36 = flow_for(n36, 0, 1); it36.next();) {
      if (math_compare<"==">(kern::to<Float>(n36.index), 0.0)) {
        inset = kern::formula([](Float index, Float kWallIn) -> Float { return ((index == 0.0) ? 0.0 : kWallIn); }, kern::to<Float>(n36.index), kWallIn);
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(P.outer))), 0.5))) {
          skip_2 = false;
          a = P.outer[i];
          bb = P.outer[kern::to<Int>(kern::formula([](Float i, Float a) -> Float { return kern::math::mod((i + 1.0), a); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(P.outer))))];
          d_2 = vector_sub(bb, a);
          len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_2).x, vector_break(d_2).y);
          if (math_compare<"<">(len, 0.02)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            u = vector_scale(d_2, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
            g0 = 0.0;
            g1 = 0.0;
            n37 = door_gap(P.doors, g0, g1, a, bb, inset);
            gap = n37.Result;
            solid = List<Float_Float_pair>{};
            if (gap) {
              list_add(solid, struct_make<Float_Float_pair>(0.0, kern::formula([](Float g0) -> Float { return kern::math::max(0.0, g0); }, g0)));
              list_add(solid, struct_make<Float_Float_pair>(kern::formula([](Float len, Float g1) -> Float { return kern::math::min(len, g1); }, len, g1), len));
            } else {
              list_add(solid, struct_make<Float_Float_pair>(0.0, len));
            }
            for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Float_pair>(solid))))); it38.next();) {
              v = solid[n38.index];
              wall(P, vector_add(a, vector_scale(u, v.first)), vector_add(a, vector_scale(u, v.second)), kern::formula([](Float base) -> Float { return (base - 0.5); }, P.base), P.top, kWall, false, true);
            }
            if (gap) {
              wall(P, vector_add(a, vector_scale(u, g0)), vector_add(a, vector_scale(u, g1)), kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH), P.top, kWall, false, true);
            }
            if (math_compare<"==">(kern::to<Float>(n36.index), 0.0)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              len_o = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(P.outer[kern::to<Int>(kern::formula([](Float i, Float a) -> Float { return kern::math::mod((i + 1.0), a); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(P.outer))))]).x, vector_break(P.outer[i]).x, vector_break(P.outer[kern::to<Int>(kern::formula([](Float i, Float a) -> Float { return kern::math::mod((i + 1.0), a); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(P.outer))))]).y, vector_break(P.outer[i]).y);
              to_inner = kern::formula([](Float len, Float len_o) -> Float { return kern::math::div(len, kern::math::max(0.01, len_o)); }, len, len_o);
              for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(F)))); it39.next();) {
                z0 = P.levels[n39.index];
                z1 = build_ceil_of(P, F, ceil_top, n39.index).Result;
                zn = kern::formula([](Float index, Float f, Float a, Float z1) -> Float { return ((kern::math::wrap32(index + 1.0) < f) ? a : (z1 + 0.3)); }, kern::to<Float>(n39.index), kern::to<Float>(F), P.levels[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index + 1.0); }, kern::to<Float>(n39.index)))], z1);
                row = kern::formula([](Float z0, Float base, Float floor_h) -> Float { return kern::math::floor(kern::math::div(((z0 + 0.5) - base), floor_h)); }, z0, P.base, P.floor_h);
                sill = kern::formula([](Float base, Float row, Float floor_h) -> Float { return (base + ((row + 0.28) * floor_h)); }, P.base, row, P.floor_h);
                head = kern::formula([](Float z1, Float base, Float row, Float floor_h) -> Float { return kern::math::min((z1 - 0.05), (base + ((row + 0.78) * floor_h))); }, z1, P.base, row, P.floor_h);
                wins = List<Float_Float_pair>{};
                rows = logic_and(logic_and(math_compare<">=">(row, 1.0), math_compare<"<=">(kern::formula([](Float base, Float row, Float floor_h) -> Float { return (base + ((row + 0.78) * floor_h)); }, P.base, row, P.floor_h), kern::formula([](Float top) -> Float { return (top - 1.0); }, P.top))), math_compare<">">(head, kern::formula([](Float sill) -> Float { return (sill + 0.3); }, sill)));
                c0 = 0.0;
                while (loop(logic_and(rows, math_compare<"<=">(kern::formula([](Float c0) -> Float { return (c0 + 2.34); }, c0), len_o)))) {
                  w0 = kern::formula([](Float c0) -> Float { return (c0 + 0.66); }, c0);
                  w1 = kern::formula([](Float c0) -> Float { return (c0 + 2.34); }, c0);
                  if (reversed) {
                    t0 = kern::formula([](Float len_o, Float w1) -> Float { return (len_o - w1); }, len_o, w1);
                    w1 = kern::formula([](Float len_o, Float w0) -> Float { return (len_o - w0); }, len_o, w0);
                    w0 = t0;
                  }
                  list_add(wins, struct_make<Float_Float_pair>(kern::formula([](Float w0, Float to_inner) -> Float { return (w0 * to_inner); }, w0, to_inner), kern::formula([](Float w1, Float to_inner) -> Float { return (w1 * to_inner); }, w1, to_inner)));
                  c0 += 3.0;
                }
                list_sort<&interiors_cpp::build_match>(*this, wins);
                t = 0.0;
                for (auto it40 = flow_for(n40, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Float_pair>(wins))))); it40.next();) {
                  v_2 = wins[n40.index];
                  wall(P, vector_add(a, vector_scale(u, t)), vector_add(a, vector_scale(u, v_2.first)), z0, z1, kWall, true, false);
                  wall(P, vector_add(a, vector_scale(u, v_2.first)), vector_add(a, vector_scale(u, v_2.second)), z0, sill, kWall, true, false);
                  wall(P, vector_add(a, vector_scale(u, v_2.first)), vector_add(a, vector_scale(u, v_2.second)), head, z1, kWall, true, false);
                  g0_2 = vector_add(vector_add(a, vector_scale(u, v_2.first)), vector_scale(vector_make(vector_break(u).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(u).x), 0.0), kern::formula([](Float kWallIn) -> Float { return (kWallIn * 0.5); }, kWallIn)));
                  g1_2 = vector_add(vector_add(a, vector_scale(u, v_2.second)), vector_scale(vector_make(vector_break(u).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(u).x), 0.0), kern::formula([](Float kWallIn) -> Float { return (kWallIn * 0.5); }, kWallIn)));
                  q = list_make<Vector, "4">(vector_make(vector_break(g0_2).x, vector_break(g0_2).y, sill), vector_make(vector_break(g1_2).x, vector_break(g1_2).y, sill), vector_make(vector_break(g1_2).x, vector_break(g1_2).y, head), vector_make(vector_break(g0_2).x, vector_break(g0_2).y, head));
                  gn = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(vector_sub(q[1], q[0]), vector_sub(q[2], q[0]))), 9.999999717180685e-10), vector_normalize(vector_cross(vector_sub(q[1], q[0]), vector_sub(q[2], q[0]))), Vector{1.0, 0.0, 0.0});
                  item = 0;
                  item_2 = 1;
                  item_3 = 2;
                  item_4 = 0;
                  item_5 = 2;
                  item_6 = 3;
                  items = list_make<I32, "6">(item, item_2, item_3, item_4, item_5, item_6);
                  for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(items))))); it41.next();) {
                    v_3 = items[n41.index];
                    engine_geo_mesh_data::MeshData_push(P.glass, q[v_3], gn, Vector{0.62, 0.7, 0.74}, Vector{0.0, 0.0, 0.0});
                  }
                  t = v_2.second;
                }
                if (logic_and(math_compare<"==">(kern::to<Float>(n39.index), 0.0), gap)) {
                  if (math_compare<"<">(t, g0)) {
                    wall(P, vector_add(a, vector_scale(u, t)), vector_add(a, vector_scale(u, g0)), z0, z1, kWall, true, false);
                  }
                  wall(P, vector_add(a, vector_scale(u, g0)), vector_add(a, vector_scale(u, g1)), kern::formula([](Float z0, Float kDoorH) -> Float { return (z0 + kDoorH); }, z0, kDoorH), z1, kWall, true, false);
                  t = kern::formula([](Float t, Float g1) -> Float { return kern::math::max(t, g1); }, t, g1);
                }
                wall(P, vector_add(a, vector_scale(u, t)), vector_add(a, vector_scale(u, len)), z0, z1, kWall, true, false);
                wall(P, a, bb, z1, zn, kWall, true, false);
                wall(P, a, bb, z0, kern::formula([](Float z0) -> Float { return (z0 + 0.1); }, z0), kTrim, true, false);
              }
            }
          }
          i += 1;
        }
      } else {
        inset_2 = kern::formula([](Float index, Float kWallIn) -> Float { return ((index == 0.0) ? 0.0 : kWallIn); }, kern::to<Float>(n36.index), kWallIn);
        i_2 = 0;
        while (loop(math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Vector>(P.inner))), 0.5))) {
          skip_3 = false;
          a_2 = P.inner[i_2];
          bb_2 = P.inner[kern::to<Int>(kern::formula([](Float i_2, Float a) -> Float { return kern::math::mod((i_2 + 1.0), a); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Vector>(P.inner))))];
          d_3 = vector_sub(bb_2, a_2);
          len_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_3).x, vector_break(d_3).y);
          if (math_compare<"<">(len_2, 0.02)) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            u_2 = vector_scale(d_3, kern::formula([](Float len_2) -> Float { return kern::math::div(1.0, len_2); }, len_2));
            g0_3 = 0.0;
            g1_3 = 0.0;
            n42 = door_gap(P.doors, g0_3, g1_3, a_2, bb_2, inset_2);
            gap_2 = n42.Result;
            solid_2 = List<Float_Float_pair>{};
            if (gap_2) {
              list_add(solid_2, struct_make<Float_Float_pair>(0.0, kern::formula([](Float g0_3) -> Float { return kern::math::max(0.0, g0_3); }, g0_3)));
              list_add(solid_2, struct_make<Float_Float_pair>(kern::formula([](Float len_2, Float g1_3) -> Float { return kern::math::min(len_2, g1_3); }, len_2, g1_3), len_2));
            } else {
              list_add(solid_2, struct_make<Float_Float_pair>(0.0, len_2));
            }
            for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Float_pair>(solid_2))))); it43.next();) {
              v_4 = solid_2[n43.index];
              wall(P, vector_add(a_2, vector_scale(u_2, v_4.first)), vector_add(a_2, vector_scale(u_2, v_4.second)), kern::formula([](Float base) -> Float { return (base - 0.5); }, P.base), P.top, kWall, false, true);
            }
            if (gap_2) {
              wall(P, vector_add(a_2, vector_scale(u_2, g0_3)), vector_add(a_2, vector_scale(u_2, g1_3)), kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH), P.top, kWall, false, true);
            }
            if (math_compare<"==">(kern::to<Float>(n36.index), 0.0)) {
              skip_3 = true;
            }
            if (logic_not(skip_3)) {
              len_o_2 = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(P.outer[kern::to<Int>(kern::formula([](Float i_2, Float a) -> Float { return kern::math::mod((i_2 + 1.0), a); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Vector>(P.outer))))]).x, vector_break(P.outer[i_2]).x, vector_break(P.outer[kern::to<Int>(kern::formula([](Float i_2, Float a) -> Float { return kern::math::mod((i_2 + 1.0), a); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Vector>(P.outer))))]).y, vector_break(P.outer[i_2]).y);
              to_inner_2 = kern::formula([](Float len_2, Float len_o_2) -> Float { return kern::math::div(len_2, kern::math::max(0.01, len_o_2)); }, len_2, len_o_2);
              for (auto it44 = flow_for(n44, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(F)))); it44.next();) {
                z0_2 = P.levels[n44.index];
                z1_2 = build_ceil_of(P, F, ceil_top, n44.index).Result;
                zn_2 = kern::formula([](Float index, Float f, Float a, Float z1_2) -> Float { return ((kern::math::wrap32(index + 1.0) < f) ? a : (z1_2 + 0.3)); }, kern::to<Float>(n44.index), kern::to<Float>(F), P.levels[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index + 1.0); }, kern::to<Float>(n44.index)))], z1_2);
                row_2 = kern::formula([](Float z0_2, Float base, Float floor_h) -> Float { return kern::math::floor(kern::math::div(((z0_2 + 0.5) - base), floor_h)); }, z0_2, P.base, P.floor_h);
                sill_2 = kern::formula([](Float base, Float row_2, Float floor_h) -> Float { return (base + ((row_2 + 0.28) * floor_h)); }, P.base, row_2, P.floor_h);
                head_2 = kern::formula([](Float z1_2, Float base, Float row_2, Float floor_h) -> Float { return kern::math::min((z1_2 - 0.05), (base + ((row_2 + 0.78) * floor_h))); }, z1_2, P.base, row_2, P.floor_h);
                wins_2 = List<Float_Float_pair>{};
                rows_2 = logic_and(logic_and(math_compare<">=">(row_2, 1.0), math_compare<"<=">(kern::formula([](Float base, Float row_2, Float floor_h) -> Float { return (base + ((row_2 + 0.78) * floor_h)); }, P.base, row_2, P.floor_h), kern::formula([](Float top) -> Float { return (top - 1.0); }, P.top))), math_compare<">">(head_2, kern::formula([](Float sill_2) -> Float { return (sill_2 + 0.3); }, sill_2)));
                c0_2 = 0.0;
                while (loop(logic_and(rows_2, math_compare<"<=">(kern::formula([](Float c0_2) -> Float { return (c0_2 + 2.34); }, c0_2), len_o_2)))) {
                  w0_2 = kern::formula([](Float c0_2) -> Float { return (c0_2 + 0.66); }, c0_2);
                  w1_2 = kern::formula([](Float c0_2) -> Float { return (c0_2 + 2.34); }, c0_2);
                  if (reversed) {
                    t0_2 = kern::formula([](Float len_o_2, Float w1_2) -> Float { return (len_o_2 - w1_2); }, len_o_2, w1_2);
                    w1_2 = kern::formula([](Float len_o_2, Float w0_2) -> Float { return (len_o_2 - w0_2); }, len_o_2, w0_2);
                    w0_2 = t0_2;
                  }
                  list_add(wins_2, struct_make<Float_Float_pair>(kern::formula([](Float w0_2, Float to_inner_2) -> Float { return (w0_2 * to_inner_2); }, w0_2, to_inner_2), kern::formula([](Float w1_2, Float to_inner_2) -> Float { return (w1_2 * to_inner_2); }, w1_2, to_inner_2)));
                  c0_2 += 3.0;
                }
                list_sort<&interiors_cpp::build_match_2>(*this, wins_2);
                t_2 = 0.0;
                for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Float_pair>(wins_2))))); it45.next();) {
                  v_5 = wins_2[n45.index];
                  wall(P, vector_add(a_2, vector_scale(u_2, t_2)), vector_add(a_2, vector_scale(u_2, v_5.first)), z0_2, z1_2, kWall, true, false);
                  wall(P, vector_add(a_2, vector_scale(u_2, v_5.first)), vector_add(a_2, vector_scale(u_2, v_5.second)), z0_2, sill_2, kWall, true, false);
                  wall(P, vector_add(a_2, vector_scale(u_2, v_5.first)), vector_add(a_2, vector_scale(u_2, v_5.second)), head_2, z1_2, kWall, true, false);
                  g0_4 = vector_add(vector_add(a_2, vector_scale(u_2, v_5.first)), vector_scale(vector_make(vector_break(u_2).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(u_2).x), 0.0), kern::formula([](Float kWallIn) -> Float { return (kWallIn * 0.5); }, kWallIn)));
                  g1_4 = vector_add(vector_add(a_2, vector_scale(u_2, v_5.second)), vector_scale(vector_make(vector_break(u_2).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(u_2).x), 0.0), kern::formula([](Float kWallIn) -> Float { return (kWallIn * 0.5); }, kWallIn)));
                  q_2 = list_make<Vector, "4">(vector_make(vector_break(g0_4).x, vector_break(g0_4).y, sill_2), vector_make(vector_break(g1_4).x, vector_break(g1_4).y, sill_2), vector_make(vector_break(g1_4).x, vector_break(g1_4).y, head_2), vector_make(vector_break(g0_4).x, vector_break(g0_4).y, head_2));
                  gn_2 = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(vector_sub(q_2[1], q_2[0]), vector_sub(q_2[2], q_2[0]))), 9.999999717180685e-10), vector_normalize(vector_cross(vector_sub(q_2[1], q_2[0]), vector_sub(q_2[2], q_2[0]))), Vector{1.0, 0.0, 0.0});
                  item_7 = 0;
                  item_8 = 1;
                  item_9 = 2;
                  item_10 = 0;
                  item_11 = 2;
                  item_12 = 3;
                  items_2 = list_make<I32, "6">(item_7, item_8, item_9, item_10, item_11, item_12);
                  for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(items_2))))); it46.next();) {
                    v_6 = items_2[n46.index];
                    engine_geo_mesh_data::MeshData_push(P.glass, q_2[v_6], gn_2, Vector{0.62, 0.7, 0.74}, Vector{0.0, 0.0, 0.0});
                  }
                  t_2 = v_5.second;
                }
                if (logic_and(math_compare<"==">(kern::to<Float>(n44.index), 0.0), gap_2)) {
                  if (math_compare<"<">(t_2, g0_3)) {
                    wall(P, vector_add(a_2, vector_scale(u_2, t_2)), vector_add(a_2, vector_scale(u_2, g0_3)), z0_2, z1_2, kWall, true, false);
                  }
                  wall(P, vector_add(a_2, vector_scale(u_2, g0_3)), vector_add(a_2, vector_scale(u_2, g1_3)), kern::formula([](Float z0_2, Float kDoorH) -> Float { return (z0_2 + kDoorH); }, z0_2, kDoorH), z1_2, kWall, true, false);
                  t_2 = kern::formula([](Float t_2, Float g1_3) -> Float { return kern::math::max(t_2, g1_3); }, t_2, g1_3);
                }
                wall(P, vector_add(a_2, vector_scale(u_2, t_2)), vector_add(a_2, vector_scale(u_2, len_2)), z0_2, z1_2, kWall, true, false);
                wall(P, a_2, bb_2, z1_2, zn_2, kWall, true, false);
                wall(P, a_2, bb_2, z0_2, kern::formula([](Float z0_2) -> Float { return (z0_2 + 0.1); }, z0_2), kTrim, true, false);
              }
            }
          }
          i_2 += 1;
        }
      }
    }
    h_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float h_2, Float a) -> Float { return ((h_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (h_2 < a)); }, kern::to<Float>(h_2), kern::to<Float>(list_length<Vector_row>(P.holes))), 0.5))) {
      for (auto it47 = flow_for(n47, 0, 1); it47.next();) {
        R = logic_select<List<Vector>>(math_compare<"==">(kern::to<Float>(n47.index), 0.0), P.holes[h_2].items, P.inner_holes[h_2].items);
        for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(R))))); it48.next();) {
          wall(P, R[n48.index], R[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n48.index), kern::to<Float>(list_length<Vector>(R))))], kern::formula([](Float index, Float base0, Float base) -> Float { return ((index != 0.0) ? base0 : (base - 0.5)); }, kern::to<Float>(n47.index), base0, P.base), kern::formula([](Float index, Float ceil_top, Float top) -> Float { return ((index != 0.0) ? ceil_top : top); }, kern::to<Float>(n47.index), ceil_top, P.top), kWall, math_compare<"==">(kern::to<Float>(n47.index), 1.0), true);
        }
      }
      h_2 += 1;
    }
    for (auto it49 = flow_for(n49, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(P.doors))))); it49.next();) {
      o0 = vector_sub(P.doors[n49.index].c, vector_scale(P.doors[n49.index].d, kern::formula([](Float kDoorW) -> Float { return kern::math::div(kDoorW, 2.0); }, kDoorW)));
      o1 = vector_add(P.doors[n49.index].c, vector_scale(P.doors[n49.index].d, kern::formula([](Float kDoorW) -> Float { return kern::math::div(kDoorW, 2.0); }, kDoorW)));
      i0 = vector_sub(o0, vector_scale(P.doors[n49.index].n, kWallIn));
      i1 = vector_sub(o1, vector_scale(P.doors[n49.index].n, kWallIn));
      wall(P, o0, i0, base0, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH), kStone, true, true);
      wall(P, i1, o1, base0, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH), kStone, true, true);
      vquad(P, vector_make(vector_break(o0).x, vector_break(o0).y, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH)), vector_make(vector_break(o1).x, vector_break(o1).y, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH)), vector_make(vector_break(i1).x, vector_break(i1).y, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH)), vector_make(vector_break(i0).x, vector_break(i0).y, kern::formula([](Float base0, Float kDoorH) -> Float { return (base0 + kDoorH); }, base0, kDoorH)), kStone);
      f0 = vector_add(o0, vector_scale(P.doors[n49.index].n, 0.6));
      f1 = vector_add(o1, vector_scale(P.doors[n49.index].n, 0.6));
      vquad(P, vector_make(vector_break(f0).x, vector_break(f0).y, kern::formula([](Float base0) -> Float { return (base0 - 0.01); }, base0)), vector_make(vector_break(f1).x, vector_break(f1).y, kern::formula([](Float base0) -> Float { return (base0 - 0.01); }, base0)), vector_make(vector_break(i1).x, vector_break(i1).y, base0), vector_make(vector_break(i0).x, vector_break(i0).y, base0), kStone);
      cquad(P, vector_make(vector_break(f0).x, vector_break(f0).y, base0), vector_make(vector_break(f1).x, vector_break(f1).y, base0), vector_make(vector_break(i1).x, vector_break(i1).y, base0), vector_make(vector_break(i0).x, vector_break(i0).y, base0), false);
    }
    floor_holes = List<Vector_row>{};
    for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(P.inner_holes))))); it50.next();) {
      list_add(floor_holes, struct_make<Vector_row>(build_cw(struct_break<Vector_row>(P.inner_holes[n50.index]).items).Result));
    }
    core_holes = floor_holes;
    if (P.has_core) {
      list_add(core_holes, struct_make<Vector_row>(build_cw(build_rect(P, 0.0, 0.0, kCoreW, kCoreL).Result).Result));
    }
    for (auto it51 = flow_for(n51, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(F)))); it51.next();) {
      z_2 = P.levels[n51.index];
      which = math_compare<"==">(kern::to<Float>(n51.index), 0.0);
      if (which) {
        slab(P, P.inner, floor_holes, z_2, logic_select<Vector>(math_compare<"==">(kern::to<Float>(n51.index), 0.0), kStone, kOak), true, true, true);
      } else {
        slab(P, P.inner, core_holes, z_2, logic_select<Vector>(math_compare<"==">(kern::to<Float>(n51.index), 0.0), kStone, kOak), true, true, true);
      }
      if (math_compare<">">(kern::to<Float>(n51.index), 0.0)) {
        slab(P, P.inner, core_holes, kern::formula([](Float z_2, Float kSlab) -> Float { return (z_2 - kSlab); }, z_2, kSlab), kCeil, false, true, false);
        a_3 = build_rect(P, 0.0, 0.0, kCoreW, kLand).Result;
        a_4 = List<Vector_row>{};
        slab(P, a_3, a_4, z_2, kConcrete, true, true, true);
        a_5 = build_rect(P, 0.0, 0.0, kCoreW, kLand).Result;
        a_6 = List<Vector_row>{};
        slab(P, a_5, a_6, kern::formula([](Float z_2, Float kSlab) -> Float { return (z_2 - kSlab); }, z_2, kSlab), kCeil, false, true, false);
      }
    }
    roof_holes = List<Vector_row>{};
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(P.holes))))); it52.next();) {
      list_add(roof_holes, P.holes[n52.index]);
    }
    if (P.roof_access) {
      list_add(roof_holes, struct_make<Vector_row>(build_cw(build_rect(P, 0.0, kLand, kCoreW, kCoreL).Result).Result));
    }
    a_7 = logic_select<List<Vector_row>>(P.roof_access, floor_holes, floor_holes);
    slab(P, P.inner, a_7, ceil_top, kCeil, false, true, false);
    if (P.roof_access) {
      a_8 = build_rect(P, 0.0, 0.0, kCoreW, kLand).Result;
      a_9 = List<Vector_row>{};
      slab(P, a_8, a_9, P.top, kConcrete, true, true, false);
    }
    slab(P, P.outer, roof_holes, P.top, kConcrete, true, false, true);
    if (P.has_core) {
      z0_3 = base0;
      build_cwall(P, 0.0, 0.0, 0.0, kCoreL, z0_3, wall_top, kStairWall);
      build_cwall(P, kCoreW, 0.0, kCoreW, kCoreL, z0_3, wall_top, kStairWall);
      build_cwall(P, 0.0, kCoreL, kCoreW, kCoreL, z0_3, wall_top, kStairWall);
      build_cwall(P, kern::formula([](Float kLane) -> Float { return (kLane + 0.1); }, kLane), kLand, kern::formula([](Float kLane) -> Float { return (kLane + 0.1); }, kLane), kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), z0_3, kern::formula([](Float wall_top, Float roof_access, Float kRoofHouse) -> Float { return (wall_top - ((roof_access != 0.0) ? kRoofHouse : 0.0)); }, wall_top, kern::to<Float>(P.roof_access), kRoofHouse), kAccent);
      stairs = List<Float_Float_pair>{};
      k = 0;
      while (loop(math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::wrap32(k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(F)))) {
        list_add(stairs, struct_make<Float_Float_pair>(P.levels[k], kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, P.levels[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::wrap32(k + 1.0); }, kern::to<Float>(k)))], P.levels[k])));
        k += 1;
      }
      if (P.roof_access) {
        list_add(stairs, struct_make<Float_Float_pair>(P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))], kern::formula([](Float top, Float a) -> Float { return (top - a); }, P.top, P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))])));
      }
      for (auto it53 = flow_for(n53, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Float_pair>(stairs))))); it53.next();) {
        v_7 = stairs[n53.index];
        half = kern::formula([](Float second) -> Float { return kern::math::div(second, 2.0); }, v_7.second);
        cquad(P, build_P3(P, 0.0, kLand, v_7.first).Result, build_P3(P, kLane, kLand, v_7.first).Result, build_P3(P, kLane, kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kern::formula([](Float first, Float half) -> Float { return (first + half); }, v_7.first, half)).Result, build_P3(P, 0.0, kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kern::formula([](Float first, Float half) -> Float { return (first + half); }, v_7.first, half)).Result, true);
        cquad(P, build_P3(P, kern::formula([](Float kCoreW, Float kLane) -> Float { return (kCoreW - kLane); }, kCoreW, kLane), kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kern::formula([](Float first, Float half) -> Float { return (first + half); }, v_7.first, half)).Result, build_P3(P, kCoreW, kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kern::formula([](Float first, Float half) -> Float { return (first + half); }, v_7.first, half)).Result, build_P3(P, kCoreW, kLand, kern::formula([](Float first, Float second) -> Float { return (first + second); }, v_7.first, v_7.second)).Result, build_P3(P, kern::formula([](Float kCoreW, Float kLane) -> Float { return (kCoreW - kLane); }, kCoreW, kLane), kLand, kern::formula([](Float first, Float second) -> Float { return (first + second); }, v_7.first, v_7.second)).Result, true);
        a_10 = build_rect(P, 0.0, kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kCoreW, kCoreL).Result;
        a_11 = List<Vector_row>{};
        slab(P, a_10, a_11, kern::formula([](Float first, Float half) -> Float { return (first + half); }, v_7.first, half), kConcrete, true, true, true);
        a_12 = build_rect(P, 0.0, kern::formula([](Float kLand, Float kRun) -> Float { return (kLand + kRun); }, kLand, kRun), kCoreW, kCoreL).Result;
        a_13 = List<Vector_row>{};
        slab(P, a_12, a_13, kern::formula([](Float first, Float half) -> Float { return ((first + half) - 0.2); }, v_7.first, half), kCeil, false, true, false);
        n = kern::to<Int>(kern::formula([](Float half) -> Float { return kern::math::max(6.0, kern::math::wrap32(kern::math::round(kern::math::div(half, 0.18)))); }, half));
        rise = kern::formula([](Float half, Float n) -> Float { return kern::math::div(half, n); }, half, kern::to<Float>(n));
        tread = kern::formula([](Float kRun, Float n) -> Float { return kern::math::div(kRun, n); }, kRun, kern::to<Float>(n));
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(P.core.v).y, vector_break(P.core.v).x);
        for (auto it54 = flow_for(n54, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it54.next();) {
          top_a = kern::formula([](Float first, Float rise, Float index) -> Float { return (first + (rise * kern::math::wrap32(index + 1.0))); }, v_7.first, rise, kern::to<Float>(n54.index));
          top_b = kern::formula([](Float first, Float half, Float rise, Float index) -> Float { return ((first + half) + (rise * kern::math::wrap32(index + 1.0))); }, v_7.first, half, rise, kern::to<Float>(n54.index));
          ca = build_CP(P, kern::formula([](Float kLane) -> Float { return kern::math::div(kLane, 2.0); }, kLane), kern::formula([](Float kLand, Float tread, Float index) -> Float { return (kLand + (tread * (index + 0.5))); }, kLand, tread, kern::to<Float>(n54.index))).Result;
          cb = build_CP(P, kern::formula([](Float kCoreW, Float kLane) -> Float { return (kCoreW - kern::math::div(kLane, 2.0)); }, kCoreW, kLane), kern::formula([](Float kLand, Float kRun, Float tread, Float index) -> Float { return ((kLand + kRun) - (tread * (index + 0.5))); }, kLand, kRun, tread, kern::to<Float>(n54.index))).Result;
          box(P, vector_make(vector_break(ca).x, vector_break(ca).y, kern::formula([](Float top_a, Float rise) -> Float { return (top_a - (rise * 0.5)); }, top_a, rise)), vector_make(tread, kLane, rise), yaw, logic_select<Vector>(math_compare<">">(kern::formula([](Float index) -> Float { return (kern::math::mod(index, 2.0) != 0.0); }, kern::to<Float>(n54.index)), 0.5), kRiser, kConcrete), false);
          box(P, vector_make(vector_break(cb).x, vector_break(cb).y, kern::formula([](Float top_b, Float rise) -> Float { return (top_b - (rise * 0.5)); }, top_b, rise)), vector_make(tread, kLane, rise), yaw, logic_select<Vector>(math_compare<">">(kern::formula([](Float index) -> Float { return (kern::math::mod(index, 2.0) != 0.0); }, kern::to<Float>(n54.index)), 0.5), kRiser, kConcrete), false);
        }
      }
      if (logic_not(P.roof_access)) {
        build_cwall(P, 0.0, kLand, kLane, kLand, P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))], kern::formula([](Float a) -> Float { return (a + 1.1); }, P.levels[kern::to<Int>(kern::formula([](Float f) -> Float { return kern::math::wrap32(f - 1.0); }, kern::to<Float>(F)))]), kDark);
      } else {
        a_14 = build_rect(P, 0.0, 0.0, kCoreW, kCoreL).Result;
        a_15 = List<Vector_row>{};
        slab(P, a_14, a_15, kern::formula([](Float top, Float kRoofHouse) -> Float { return (top + kRoofHouse); }, P.top, kRoofHouse), kConcrete, false, true, true);
        a_16 = build_rect(P, 0.0, 0.0, kCoreW, kCoreL).Result;
        a_17 = List<Vector_row>{};
        slab(P, a_16, a_17, kern::formula([](Float top, Float kRoofHouse) -> Float { return ((top + kRoofHouse) + 0.01); }, P.top, kRoofHouse), kConcrete, true, true, true);
      }
    }
    x0 = 1000000000.0;
    y0 = 1000000000.0;
    x1 = -1000000000.0;
    y1 = -1000000000.0;
    for (auto it55 = flow_for(n55, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(P.inner))))); it55.next();) {
      p_2 = P.inner[n55.index];
      x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(p_2).x);
      x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(p_2).x);
      y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(p_2).y);
      y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(p_2).y);
    }
    yaw0 = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(axis).y, vector_break(axis).x);
    for (auto it58 = flow_for(n58, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(F)))); it58.next();) {
      z_3 = P.levels[n58.index];
      zc = build_ceil_of(P, F, ceil_top, n58.index).Result;
      placed = 0;
      lamps = 0;
      x = kern::formula([](Float x0) -> Float { return (x0 + 1.8); }, x0);
      while (loop(logic_and(math_compare<"<">(x, x1), math_compare<"<">(kern::to<Float>(placed), 10.0)))) {
        y = kern::formula([](Float y0) -> Float { return (y0 + 1.8); }, y0);
        while (loop(logic_and(math_compare<"<">(y, y1), math_compare<"<">(kern::to<Float>(placed), 10.0)))) {
          skip_4 = false;
          p_3 = vector_make(x, y, 0.0);
          h_3 = hash_key(kern::to<Int>(kern::formula([](Float seed, Float index) -> Float { return kern::math::wrap32(seed + kern::math::mul32(index, 131.0)); }, kern::to<Float>(seed), kern::to<Float>(n58.index))), kern::to<Int>(kern::formula([](Float x, Float y) -> Float { return kern::math::wrap32(((kern::math::trunc((x * 7.0)) * 31.0) + kern::math::trunc((y * 13.0)))); }, x, y))).Result;
          either = math_compare<">">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float h_3) -> Float { return kern::math::mod(h_3, 100.0); }, kern::to<Float>(h_3))))), 38.0);
          if (logic_not(either)) {
            n60 = build_free_spot(P, p_3, 1.3);
            either = logic_not(n60.Result);
          }
          if (either) {
            skip_4 = true;
          }
          if (logic_not(skip_4)) {
            yaw_2 = kern::formula([](Float yaw0, Float h_3, Float kPi) -> Float { return (yaw0 + kern::math::div((kern::math::band(kern::math::shr(h_3, 8.0), 3.0) * kPi), 2.0)); }, yaw0, kern::to<Float>(h_3), kPi);
            kind = kern::to<Int>(kern::formula([](Float index, Float placed, Float h_3) -> Float { return ((index == 0.0) ? ((placed == 0.0) ? 9.0 : kern::math::wrap32(kern::math::wrap32(3.0 + kern::math::mod(kern::math::shr(h_3, 12.0), 3.0)) + kern::math::mul32(kern::math::band(kern::math::shr(h_3, 14.0), 1.0), 3.0))) : kern::math::mod(kern::math::shr(h_3, 12.0), 9.0)); }, kern::to<Float>(n58.index), kern::to<Float>(placed), kern::to<Float>(h_3)));
            selected = kind;
            if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.37); }, z_3)), Vector{1.6, 0.8, 0.74}, yaw_2, kOak, true);
              a_18 = common::yaw_quat(yaw_2).Result;
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 1.0); }, z_3)), engine_math_math::Quat_rotate(a_18, Vector{0.0, 0.22, 0.0}).Result), Vector{0.62, 0.035, 0.38}, yaw_2, rgb(1118740).Result, false);
              a_19 = common::yaw_quat(yaw_2).Result;
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.25); }, z_3)), engine_math_math::Quat_rotate(a_19, Vector{0.0, -0.7, 0.0}).Result), Vector{0.5, 0.5, 0.5}, yaw_2, rgb(3817287).Result, false);
            } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.27); }, z_3)), Vector{1.72, 2.18, 0.54}, yaw_2, rgb(15658214).Result, true);
              a_20 = common::yaw_quat(yaw_2).Result;
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.55); }, z_3)), engine_math_math::Quat_rotate(a_20, Vector{0.0, 1.05, 0.0}).Result), Vector{1.74, 0.08, 1.1}, yaw_2, rgb(6176554).Result, false);
            } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.95); }, z_3)), Vector{1.2, 0.36, 1.9}, yaw_2, rgb(15658214).Result, true);
            } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.3); }, z_3)), Vector{2.1, 0.92, 0.6}, yaw_2, rgb(kern::to<Int>(kern::formula([](Float h_3) -> Float { return kern::math::wrap32(5205370.0 + kern::math::band(h_3, 986895.0)); }, kern::to<Float>(h_3)))).Result, true);
              a_21 = common::yaw_quat(yaw_2).Result;
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.66); }, z_3)), engine_math_math::Quat_rotate(a_21, Vector{0.0, 0.36, 0.0}).Result), Vector{2.1, 0.2, 0.5}, yaw_2, rgb(kern::to<Int>(kern::formula([](Float h_3) -> Float { return kern::math::wrap32(5205370.0 + kern::math::band(h_3, 986895.0)); }, kern::to<Float>(h_3)))).Result, false);
            } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.37); }, z_3)), Vector{0.9, 0.9, 0.74}, yaw_2, rgb(15722975).Result, true);
            } else if (math_compare<"==">(kern::to<Float>(selected), 6.0)) {
              q_3 = common::yaw_quat(yaw_2).Result;
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.22); }, z_3)), Vector{0.86, 0.84, 0.44}, yaw_2, rgb(kern::to<Int>(kern::formula([](Float h_3) -> Float { return kern::math::wrap32(9064252.0 + kern::math::band(h_3, 460551.0)); }, kern::to<Float>(h_3)))).Result, true);
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.66); }, z_3)), engine_math_math::Quat_rotate(q_3, Vector{0.0, 0.34, 0.0}).Result), Vector{0.86, 0.18, 0.56}, yaw_2, rgb(kern::to<Int>(kern::formula([](Float h_3) -> Float { return kern::math::wrap32(9064252.0 + kern::math::band(h_3, 460551.0)); }, kern::to<Float>(h_3)))).Result, false);
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.75); }, z_3)), engine_math_math::Quat_rotate(q_3, Vector{0.75, 0.2, 0.0}).Result), Vector{0.04, 0.04, 1.45}, yaw_2, kDark, false);
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 1.55); }, z_3)), engine_math_math::Quat_rotate(q_3, Vector{0.75, 0.2, 0.0}).Result), Vector{0.36, 0.36, 0.26}, yaw_2, rgb(16049346).Result, false);
            } else if (math_compare<"==">(kern::to<Float>(selected), 7.0)) {
              q_4 = common::yaw_quat(yaw_2).Result;
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.38); }, z_3)), Vector{1.8, 0.9, 0.76}, yaw_2, rgb(6176554).Result, true);
              item_13 = -0.45;
              item_14 = 0.45;
              items_3 = list_make<Float, "2">(item_13, item_14);
              for (auto it61 = flow_for(n61, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_3))))); it61.next();) {
                cx = items_3[n61.index];
                item_15 = -0.68;
                item_16 = 0.68;
                items_4 = list_make<Float, "2">(item_15, item_16);
                for (auto it62 = flow_for(n62, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_4))))); it62.next();) {
                  cy = items_4[n62.index];
                  box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.45); }, z_3)), engine_math_math::Quat_rotate(q_4, vector_make(cx, cy, 0.0)).Result), Vector{0.44, 0.42, 0.9}, yaw_2, rgb(12159836).Result, false);
                }
              }
            } else if (math_compare<"==">(kern::to<Float>(selected), 8.0)) {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.45); }, z_3)), Vector{2.4, 0.6, 0.9}, yaw_2, rgb(15658214).Result, true);
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.915); }, z_3)), Vector{2.44, 0.64, 0.04}, yaw_2, rgb(4013890).Result, false);
            } else if (math_compare<"==">(kern::to<Float>(selected), 9.0)) {
              q_5 = common::yaw_quat(yaw_2).Result;
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.55); }, z_3)), Vector{2.8, 0.75, 1.1}, yaw_2, rgb(6176554).Result, true);
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 1.12); }, z_3)), Vector{2.9, 0.85, 0.05}, yaw_2, rgb(14276303).Result, false);
              box(P, vector_add(vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.55); }, z_3)), engine_math_math::Quat_rotate(q_5, Vector{0.0, -0.39, 0.0}).Result), Vector{2.6, 0.01, 0.5}, yaw_2, rgb(13083482).Result, false);
            } else {
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.21); }, z_3)), Vector{0.4, 0.4, 0.42}, yaw_2, rgb(14275786).Result, true);
              box(P, vector_make(vector_break(p_3).x, vector_break(p_3).y, kern::formula([](Float z_3) -> Float { return (z_3 + 0.9); }, z_3)), Vector{0.6, 0.6, 0.9}, kern::formula([](Float yaw_2) -> Float { return (yaw_2 + 0.4); }, yaw_2), rgb(4025139).Result, false);
            }
            placed += 1;
            P.furniture = kern::to<Int>(kern::formula([](Float furniture) -> Float { return (furniture + 1.0); }, kern::to<Float>(P.furniture)));
          }
          y += 3.4;
        }
        x += 3.4;
      }
      x_2 = kern::formula([](Float x0) -> Float { return (x0 + 1.8); }, x0);
      while (loop(logic_and(math_compare<"<">(x_2, x1), math_compare<"<">(kern::to<Float>(lamps), 24.0)))) {
        y_2 = kern::formula([](Float y0) -> Float { return (y0 + 1.8); }, y0);
        while (loop(logic_and(math_compare<"<">(y_2, y1), math_compare<"<">(kern::to<Float>(lamps), 24.0)))) {
          if (logic_and(pip(P.inner, vector_make(x_2, y_2, 0.0)).Result, math_compare<">">(ring_dist(P.inner, vector_make(x_2, y_2, 0.0)).Result, 0.8))) {
            list_add(P.lamps, vector_make(x_2, y_2, kern::formula([](Float zc) -> Float { return (zc - 0.02); }, zc)));
            lamps += 1;
          }
          y_2 += 3.6;
        }
        x_2 += 3.6;
      }
    }
    if (P.roof_access) {
      ac = 0;
      x_3 = kern::formula([](Float x0) -> Float { return (x0 + 2.0); }, x0);
      while (loop(logic_and(math_compare<"<">(x_3, kern::formula([](Float x1) -> Float { return (x1 - 1.0); }, x1)), math_compare<"<">(kern::to<Float>(ac), 3.0)))) {
        y_3 = kern::formula([](Float y0) -> Float { return (y0 + 2.0); }, y0);
        while (loop(logic_and(math_compare<"<">(y_3, kern::formula([](Float y1) -> Float { return (y1 - 1.0); }, y1)), math_compare<"<">(kern::to<Float>(ac), 3.0)))) {
          skip_5 = false;
          q_6 = vector_make(x_3, y_3, 0.0);
          if (logic_or(logic_not(pip(P.inner, q_6).Result), math_compare<"<">(ring_dist(P.inner, q_6).Result, 1.5))) {
            skip_5 = true;
          }
          if (logic_not(skip_5)) {
            c = vector_sub(q_6, P.core.o);
            cu = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(c).x, vector_break(P.core.u).x, vector_break(c).y, vector_break(P.core.u).y);
            cv = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(c).x, vector_break(P.core.v).x, vector_break(c).y, vector_break(P.core.v).y);
            if (logic_and(logic_and(logic_and(math_compare<">">(cu, -2.5), math_compare<"<">(cu, kern::formula([](Float kCoreW) -> Float { return (kCoreW + 2.5); }, kCoreW))), math_compare<">">(cv, -3.5)), math_compare<"<">(cv, kern::formula([](Float kCoreL) -> Float { return (kCoreL + 2.5); }, kCoreL)))) {
              skip_5 = true;
            }
            if (logic_not(skip_5)) {
              if (math_compare<">">(h01(seed, kern::to<Int>(kern::formula([](Float ac, Float x_3, Float y_3) -> Float { return kern::math::wrap32(kern::math::wrap32(900.0 + kern::math::mul32(ac, 7.0)) + kern::math::trunc(((x_3 * 3.0) + y_3))); }, kern::to<Float>(ac), x_3, y_3))).Result, 0.5)) {
                skip_5 = true;
              }
              if (logic_not(skip_5)) {
                box(P, vector_make(vector_break(q_6).x, vector_break(q_6).y, kern::formula([](Float top) -> Float { return (top + 0.55); }, P.top)), Vector{1.5, 1.1, 1.1}, yaw0, rgb(11120049).Result, true);
                box(P, vector_make(vector_break(q_6).x, vector_break(q_6).y, kern::formula([](Float top) -> Float { return (top + 1.12); }, P.top)), Vector{0.8, 0.8, 0.05}, yaw0, rgb(2895667).Result, false);
                ac += 1;
              }
            }
          }
          y_3 += 5.0;
        }
        x_3 += 5.0;
      }
    }
    return {logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector>(P.points)), 0.0))};
  }

  void Interiors_clear(Interiors& self) {
    Bool remove = false;
    Bool remove_2 = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    if (logic_and(object_valid<kern::Renderer>(self.r_), math_compare<"!=">(kern::to<Float>(self.mesh_id_), 0.0))) {
      engine_renderer_remove(n63, self.r_, self.mesh_id_);
      remove = n63.result;
      self.mesh_id_ = 0;
    }
    if (logic_and(object_valid<kern::Renderer>(self.r_), math_compare<"!=">(kern::to<Float>(self.glass_id_), 0.0))) {
      engine_renderer_remove(n64, self.r_, self.glass_id_);
      remove_2 = n64.result;
      self.glass_id_ = 0;
    }
    both = logic_and(object_valid<kern::physics::World>(self.world_), engine_physics_physics::Body_valid(self.body_).Result);
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

  void Interiors_activate(Interiors& self, const CityStream_BuildingRef& ref, CityStream& city, Float ground = 0.0) {
    Interiors_Plan plan = Interiors_Plan{List<Vector>{}, List<Vector>{}, List<Vector_row>{}, List<Vector_row>{}, List<Interiors_Door>{}, List<Float>{}, 0.0, 8.0, 3.3, 0.0, false, false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, List<Vector>{}, List<U32>{}, List<Vector>{}, 0};
    MapFeatures_Building a = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape a_2 = physics_Shape{Int64{0x0}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    Int allocate_id_2 = 0;
    MeshLook glass = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert_2 = false;
    Int count = 0;
    Float fill = 0.0;
    plan = Interiors_Plan{List<Vector>{}, List<Vector>{}, List<Vector_row>{}, List<Vector_row>{}, List<Interiors_Door>{}, List<Float>{}, 0.0, 8.0, 3.3, 0.0, false, false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, List<Vector>{}, List<U32>{}, List<Vector>{}, 0};
    a = list_get<MapFeatures_Building>(struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city, ref.tile).Result).buildings, ref.index);
    n65 = Interiors_build(a, plan, ground);
    if (logic_not(n65.Result)) {
      return;
    }
    Interiors_clear(self);
    self.plan_ = plan;
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    bs.type = physics_BodyType::fixed;
    engine_world_create_body(n66, self.world_, bs);
    create_body = n66.result;
    self.body_ = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    ss.kind = physics_ShapeKind::mesh;
    ss.points = self.plan_.points;
    ss.indices = self.plan_.indices;
    ss.material.friction = 0.7;
    ss.material.category_bits = Int64{0x1};
    engine_world_create_shape(n67, self.world_, self.body_, ss);
    create_shape = n67.result;
    a_2 = create_shape;
    if (logic_not(engine_physics_physics::Shape_valid(a_2).Result)) {
      Interiors_clear(self);
      return;
    }
    engine_weld(n68, self.plan_.mesh, 0);
    weld = n68.result;
    self.plan_.mesh = n68.mesh;
    engine_renderer_allocate_id(n69, self.r_);
    allocate_id = n69.result;
    self.mesh_id_ = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material.roughness = 0.85;
    look.material.double_sided = true;
    look.material.cast_shadow = false;
    look.extra_slot = kMaxExtensionAttributes;
    engine_upsert(n70, self.r_, self.mesh_id_, self.plan_.mesh, look, List<Float>{}, List<Float>{});
    upsert = n70.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, self.mesh_id_);
      self.mesh_id_ = 0;
    }
    self.plan_.mesh = struct_make<MeshData>(List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{});
    if (math_compare<"!=">(kern::to<Float>(engine_geo_mesh_data::MeshData_vertex_count(self.plan_.glass).Result), 0.0)) {
      engine_renderer_allocate_id(n71, self.r_);
      allocate_id_2 = n71.result;
      self.glass_id_ = allocate_id_2;
      glass = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      glass.material.roughness = 0.05;
      glass.material.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, 0.22).Result;
      glass.material.alpha = AlphaMode::blend;
      glass.material.double_sided = true;
      glass.material.cast_shadow = false;
      glass.material.emissive = Vector{0.02, 0.025, 0.03};
      glass.extra_slot = kMaxExtensionAttributes;
      engine_upsert(n72, self.r_, self.glass_id_, self.plan_.glass, glass, List<Float>{}, List<Float>{});
      upsert_2 = n72.result;
      if (logic_not(upsert_2)) {
        engine_renderer_release_id(self.r_, self.glass_id_);
        self.glass_id_ = 0;
      }
      self.plan_.glass = struct_make<MeshData>(List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{});
    }
    self.active_ = true;
    self.tile_ = ref.tile;
    self.index_ = ref.index;
    count = list_length<Interiors_Door>(self.plan_.doors);
    fill = 0.0;
    list_clear(self.door_angle_);
    list_resize(self.door_angle_, count, fill);
    city_stream_cpp::CityStream_exclude_building(city, self.tile_, kern::to<Int>(kern::formula([](Float index_) -> Float { return kern::math::wrap32s(index_); }, kern::to<Float>(self.index_))));
    self.built_ = kern::to<Int>(kern::formula([](Float built_) -> Float { return (built_ + 1.0); }, kern::to<Float>(self.built_)));
  }

  struct Interiors_insideResult { Bool Result = false; };
  KERN_PURE Interiors_insideResult Interiors_inside(const Interiors& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    if (logic_or(logic_not(self.active_), logic_not(pip(self.plan_.outer, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result))) {
      return {false};
    }
    for (auto it73 = flow_for(n73, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(self.plan_.holes))))); it73.next();) {
      if (pip(self.plan_.holes[n73.index].items, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result) {
        return {false};
      }
    }
    return {logic_and(math_compare<">">(vector_break(p).z, kern::formula([](Float base) -> Float { return (base - 1.0); }, self.plan_.base)), math_compare<"<">(vector_break(p).z, kern::formula([](Float top, Float kRoofHouse) -> Float { return ((top + kRoofHouse) + 1.0); }, self.plan_.top, kRoofHouse)))};
  }

  struct Interiors_level_atResult { I32 Result = 0; };
  KERN_PURE Interiors_level_atResult Interiors_level_at(const Interiors& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    I32 k = 0;
    if (logic_not(Interiors_inside(self, p).Result)) {
      return {-1};
    }
    k = 0;
    for (auto it74 = flow_for(n74, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(self.plan_.levels))))); it74.next();) {
      if (math_compare<">=">(vector_break(p).z, kern::formula([](Float a) -> Float { return (a - 0.3); }, self.plan_.levels[n74.index]))) {
        k = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n74.index)));
      }
    }
    if (logic_and(self.plan_.roof_access, math_compare<">=">(vector_break(p).z, kern::formula([](Float top) -> Float { return (top - 0.3); }, self.plan_.top)))) {
      k = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Float>(self.plan_.levels))));
    }
    return {k};
  }

  struct update_matchResult { Bool Result = false; };
  KERN_PURE update_matchResult update_match(const Float_Interiors_Closed_pair& a, const Float_Interiors_Closed_pair& b) {
    return {math_compare<"<">(a.first, b.first)};
  }

  void Interiors_update(Interiors& self, CityStream& city, Float dt = 0.0, Float now = 0.0, Vector feet = Vector{0.0, 0.0, 0.0}, Bool on_foot = false) {
    Vector f = Vector{0.0, 0.0, 0.0};
    Bool near = false;
    Bool in = false;
    List<CityStream_BuildingRef> near_2 = List<CityStream_BuildingRef>{};
    I32 best = 0;
    Float best_d = 0.0;
    List<Vector> o = List<Vector>{};
    Float d = 0.0;
    Float d_2 = 0.0;
    Bool keep = false;
    Bool other = false;
    Float ground = 0.0;
    List<Float_Interiors_Closed_pair> doors = List<Float_Interiors_Closed_pair>{};
    Bool skip = false;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    List<Vector> o_2 = List<Vector>{};
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    Float area = 0.0;
    List<Interiors_Door> list = List<Interiors_Door>{};
    Bool skip_2 = false;
    Float dist = 0.0;
    Int i = 0;
    self.feet_ = feet;
    f = vector_make(vector_break(feet).x, vector_break(feet).y, 0.0);
    if (self.active_) {
      for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(self.plan_.doors))))); it75.next();) {
        near = logic_and(math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(f).x, vector_break(self.plan_.doors[n75.index].c).x, vector_break(f).y, vector_break(self.plan_.doors[n75.index].c).y), 2.6), math_compare<"<">(kern::formula([](Float z, Float a) -> Float { return kern::math::abs((z - a)); }, vector_break(feet).z, self.plan_.levels[0]), 2.0));
        self.door_angle_[n75.index] = kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.door_angle_[n75.index], kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float near, Float a) -> Float { return (((near != 0.0) ? 1.45 : 0.0) - a); }, kern::to<Float>(near), self.door_angle_[n75.index]), kern::formula([](Float dt) -> Float { return ((-dt) * 3.2); }, dt), kern::formula([](Float dt) -> Float { return (dt * 3.2); }, dt)));
      }
      in = Interiors_inside(self, feet).Result;
      if (logic_and(in, logic_not(self.was_inside_))) {
        self.entered_ = kern::to<Int>(kern::formula([](Float entered_) -> Float { return (entered_ + 1.0); }, kern::to<Float>(self.entered_)));
      }
      self.was_inside_ = in;
    }
    if (math_compare<"<">(now, self.next_scan_)) {
      return;
    }
    self.next_scan_ = kern::formula([](Float now) -> Float { return (now + 0.5); }, now);
    near_2 = List<CityStream_BuildingRef>{};
    city_stream_cpp::CityStream_buildings_near(city, near_2, f, kDoorsRadius);
    best = -1;
    best_d = kActivate;
    for (auto it76 = flow_for(n76, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<CityStream_BuildingRef>(near_2))))); it76.next();) {
      o = struct_break<Vector_row>(list_get<Vector_row>(struct_break<MapFeatures_Building>(list_get<MapFeatures_Building>(struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city, near_2[n76.index].tile).Result).buildings, near_2[n76.index].index)).rings, 0)).items;
      d = kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? 0.0 : the_result); }, kern::to<Float>(pip(o, f).Result), ring_dist(o, f).Result);
      if (math_compare<"<">(d, best_d)) {
        best_d = d;
        best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n76.index)));
      }
    }
    if (self.active_) {
      d_2 = kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? 0.0 : the_result); }, kern::to<Float>(pip(self.plan_.outer, f).Result), ring_dist(self.plan_.outer, f).Result);
      keep = logic_or(Interiors_inside(self, feet).Result, math_compare<"<">(d_2, kRelease));
      other = logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(best), 0.0), logic_not(logic_and(int64_equal(near_2[best].tile, self.tile_), math_compare<"==">(kern::to<Float>(near_2[best].index), kern::to<Float>(self.index_))))), math_compare<"<">(best_d, kern::formula([](Float d_2) -> Float { return (0.5 * d_2); }, d_2))), logic_not(Interiors_inside(self, feet).Result));
      if (logic_or(logic_or(logic_not(keep), other), logic_and(logic_not(on_foot), math_compare<">">(d_2, kActivate)))) {
        Interiors_release(self, city);
      }
    }
    if (logic_and(logic_and(logic_not(self.active_), on_foot), math_compare<">=">(kern::to<Float>(best), 0.0))) {
      ground = city_stream_cpp::CityStream_ground_height(city, vector_break(f).x, vector_break(f).y).Result;
      Interiors_activate(self, near_2[best], city, ground);
    }
    doors = List<Float_Interiors_Closed_pair>{};
    for (auto it77 = flow_for(n77, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_BuildingRef>(near_2))))); it77.next();) {
      skip = false;
      if (logic_and(logic_and(self.active_, int64_equal(near_2[n77.index].tile, self.tile_)), math_compare<"==">(kern::to<Float>(near_2[n77.index].index), kern::to<Float>(self.index_)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        b = list_get<MapFeatures_Building>(struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city, near_2[n77.index].tile).Result).buildings, near_2[n77.index].index);
        o_2 = clean(b.rings[0].items).Result;
        if (math_compare<"<">(kern::to<Float>(list_length<Vector>(o_2)), 3.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"<">(signed_area(o_2).Result, 0.0)) {
            first = 0;
            last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(o_2))));
            while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
              held = list_get<Vector>(o_2, first);
              o_2[first] = list_get<Vector>(o_2, last);
              o_2[last] = held;
              first += 1;
              last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
            }
          }
          area = signed_area(o_2).Result;
          if (logic_or(logic_or(math_compare<"<">(area, 24.0), math_compare<">">(area, 40000.0)), math_compare<"<">(kern::formula([](Float top, Float base) -> Float { return (top - base); }, b.top, b.base), 2.8))) {
            skip = true;
          }
          if (logic_not(skip)) {
            n78 = Interiors_doors_of(o_2, area);
            list = n78.Result;
            for (auto it79 = flow_for(n79, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(list))))); it79.next();) {
              skip_2 = false;
              dist = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(list[n79.index].c).x, vector_break(f).x, vector_break(list[n79.index].c).y, vector_break(f).y);
              if (math_compare<">">(dist, kDoorsRadius)) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                list_add(doors, struct_make<Float_Interiors_Closed_pair>(dist, struct_make<Interiors_Closed>(vector_make(vector_break(list[n79.index].c).x, vector_break(list[n79.index].c).y, kern::formula([](Float base, Float the_result) -> Float { return kern::math::max(base, the_result); }, b.base, city_stream_cpp::CityStream_ground_height(city, kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, vector_break(list[n79.index].c).x, vector_break(list[n79.index].n).x), kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, vector_break(list[n79.index].c).y, vector_break(list[n79.index].n).y)).Result)), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(list[n79.index].d).y, vector_break(list[n79.index].d).x))));
              }
            }
          }
        }
      }
    }
    list_sort<&interiors_cpp::update_match>(*this, doors);
    list_clear(self.closed_);
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float_Interiors_Closed_pair>(doors))), 0.5), math_compare<">">(kern::formula([](Float i, Float kMaxClosedDoors) -> Float { return ((i < 0.0) != (kMaxClosedDoors < 0.0) ? (kMaxClosedDoors < 0.0) : (i < kMaxClosedDoors)); }, kern::to<Float>(i), kern::to<Float>(kMaxClosedDoors)), 0.5)))) {
      list_add(self.closed_, doors[i].second);
      i += 1;
    }
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
    for (auto it80 = flow_for(n80, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it80.next();) {
      s = items[n80.index];
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
    for (auto it81 = flow_for(n81, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Interiors_Closed>(self.closed_))))); it81.next();) {
      skip = false;
      if (math_compare<">">(vector_length(vector_sub(self.closed_[n81.index].at, eye)), kern::formula([](Float kDoorsRadius) -> Float { return (kDoorsRadius + 20.0); }, kDoorsRadius))) {
        skip = true;
      }
      if (logic_not(skip)) {
        draw_unit(art, self.closed_[n81.index].at, self.closed_[n81.index].yaw, 0.0, false);
      }
    }
    if (logic_not(self.active_)) {
      return;
    }
    for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(self.plan_.doors))))); it82.next();) {
      draw_unit(art, vector_make(vector_break(self.plan_.doors[n82.index].c).x, vector_break(self.plan_.doors[n82.index].c).y, self.plan_.levels[0]), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(self.plan_.doors[n82.index].d).y, vector_break(self.plan_.doors[n82.index].d).x), self.door_angle_[n82.index], logic_and(math_compare<">">(self.door_angle_[n82.index], 0.05), logic_not(Interiors_inside(self, eye).Result)));
    }
    for (auto it83 = flow_for(n83, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.plan_.lamps))))); it83.next();) {
      if (logic_and(math_compare<"<">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(self.plan_.lamps[n83.index]).z, vector_break(self.feet_).z), 7.0), math_compare<"<">(vector_length(vector_sub(self.plan_.lamps[n83.index], eye)), 40.0))) {
        a_2 = engine_math_math::Mat4_translate(self.plan_.lamps[n83.index]).Result;
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
  struct { Object<kern::Earcut> result = Object<kern::Earcut>{}; } n16;
  struct { List<U32> out = List<U32>{}; } n17;
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
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  Interiors_doors_ofResult n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  door_gapResult n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  door_gapResult n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  struct : kern::Node<44> { Int index = 0; } n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  struct : kern::Node<49> { Int index = 0; } n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  struct : kern::Node<51> { Int index = 0; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  struct : kern::Node<53> { Int index = 0; } n53;
  struct : kern::Node<54> { Int index = 0; } n54;
  struct : kern::Node<55> { Int index = 0; } n55;
  struct : kern::Node<56> { Int index = 0; } n56;
  struct : kern::Node<57> { Int index = 0; } n57;
  struct : kern::Node<58> { Int index = 0; } n58;
  build_free_spotResult n60;
  struct : kern::Node<61> { Int index = 0; } n61;
  struct : kern::Node<62> { Int index = 0; } n62;
  struct { Bool result = false; } n63;
  struct { Bool result = false; } n64;
  Interiors_buildResult n65;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n66;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n67;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n68;
  struct { Int result = 0; } n69;
  struct { Bool result = false; } n70;
  struct { Int result = 0; } n71;
  struct { Bool result = false; } n72;
  struct : kern::Node<73> { Int index = 0; } n73;
  struct : kern::Node<74> { Int index = 0; } n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  struct : kern::Node<76> { Int index = 0; } n76;
  struct : kern::Node<77> { Int index = 0; } n77;
  Interiors_doors_ofResult n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<80> { Int index = 0; } n80;
  struct : kern::Node<81> { Int index = 0; } n81;
  struct : kern::Node<82> { Int index = 0; } n82;
  struct : kern::Node<83> { Int index = 0; } n83;
};
