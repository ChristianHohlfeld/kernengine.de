struct destruction_cpp : virtual kern::Library, virtual city_stream, virtual common, virtual destruction, virtual engine_geo_mesh_data, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(Destruction_fracture_Seg) { Vector m = Vector{0.0, 0.0, 0.0}; Float len = 0.0; Float yaw = 0.0; };

  // ---- variables ----
  KERN_CONST Int kTorn = 40;
  /// facade panels at most 7 m wide
  KERN_CONST Float kPanelMax = 7.0;
  KERN_CONST Float kFloor = 3.2;
  /// per broken building
  KERN_CONST Int kMaxPieces = 40;
  KERN_CONST Int kPieces = 2000;
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kEmber = Vector{1.0, 0.5411764705882353, 0.23921568627450981};
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kDim = Vector{0.4196078431372549, 0.35294117647058826, 0.2901960784313726};
  /// the pieces' colours: concrete, a few warm embers, a few dim ones
  KERN_CONST Vector kConcrete = Vector{0.62, 0.58, 0.52};

  // ---- functions ----
  struct norm2Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE norm2Result norm2(Vector v = Vector{0.0, 0.0, 0.0}, Vector fallback = Vector{0.0, 0.0, 0.0}) {
    Float l = 0.0;
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(v).x, vector_break(v).y);
    return {logic_select<Vector>(math_compare<">">(l, 0.001), vector_make(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, vector_break(v).x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, vector_break(v).y, l), 0.0), fallback)};
  }

  struct Destruction_merge_pointsResult { Bool Result = false; };
  Destruction_merge_pointsResult Destruction_merge_points(Destruction& self, const List<Int_Float_pair>& list, Bool the_log = false) {
    Bool changed = false;
    Bool skip = false;
    Int key = 0;
    Float value = 0.0;
    Int_Bool_pair v = Int_Bool_pair{0, false};
    changed = false;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(list))))); it10.next();) {
      skip = false;
      if (logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, list[n10.index].second), 0.0)), math_compare<"<">(list[n10.index].second, 0.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        key = list[n10.index].first;
        value = list[n10.index].second;
        map_add(n11, self.points_, key);
        if (n11.added) {
          self.points_[n11.index] = value;
        }
        v = struct_make<Int_Bool_pair>(n11.index, n11.added);
        if (logic_not(v.second)) {
          if (math_compare<">=">(list[n10.index].second, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.points_[v.first]))) {
            skip = true;
          }
          if (logic_not(skip)) {
            self.points_[v.first] = list[n10.index].second;
          }
        }
        if (logic_not(skip)) {
          changed = true;
          if (the_log) {
            list_add(self.points_log_, struct_make<Int_Float_pair>(list[n10.index].first, list[n10.index].second));
          }
        }
      }
    }
    return {changed};
  }

  struct Destruction_tearResult { Bool Result = false; };
  Destruction_tearResult Destruction_tear(Destruction& self, const Destruction_Broken& b, Rng& rng, Vector center = Vector{0.0, 0.0, 0.0}) {
    Int largest = 0;
    Float best = 0.0;
    List<Vector> rr = List<Vector>{};
    Float a = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    List<Vector> ring = List<Vector>{};
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float reach = 0.0;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Vector pivot = Vector{0.0, 0.0, 0.0};
    Float h = 0.0;
    List<Vector> pts = List<Vector>{};
    Vector p_3 = Vector{0.0, 0.0, 0.0};
    Float area = 0.0;
    Vector p_4 = Vector{0.0, 0.0, 0.0};
    Vector q_2 = Vector{0.0, 0.0, 0.0};
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    List<Float> xy = List<Float>{};
    Vector p_5 = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    List<Int> tris = List<Int>{};
    Int earcut = 0;
    Int Earcut_2 = 0;
    Bool returned = false;
    Vector col = Vector{0.0, 0.0, 0.0};
    Int value = 0;
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int i = 0;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    List<Vector> list = List<Vector>{};
    Vector q_3 = Vector{0.0, 0.0, 0.0};
    List<Vector> list_2 = List<Vector>{};
    Vector q_4 = Vector{0.0, 0.0, 0.0};
    Vector a_3 = Vector{0.0, 0.0, 0.0};
    Vector c_2 = Vector{0.0, 0.0, 0.0};
    Vector e = Vector{0.0, 0.0, 0.0};
    Vector nrm = Vector{0.0, 0.0, 0.0};
    List<Vector> list_3 = List<Vector>{};
    Vector q_5 = Vector{0.0, 0.0, 0.0};
    Destruction_Torn t = Destruction_Torn{0, Destruction_Broken{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 1.0, 0.0};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 at = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    Bool returned_2 = false;
    Float I = 0.0;
    Bool returned_3 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.r_), 0.0))) {
      return {false};
    }
    largest = -1;
    best = -1.0;
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(b.rings))))); it12.next();) {
      rr = b.rings[n12.index].items;
      a = 0.0;
      for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(rr))))); it13.next();) {
        p = rr[n13.index];
        q = rr[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n13.index), kern::to<Float>(list_length<Vector>(rr))))];
        a += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return ((x * y) - (x_2 * y_2)); }, vector_break(p).x, vector_break(q).y, vector_break(q).x, vector_break(p).y);
      }
      if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, a), best)) {
        best = kern::formula([](Float a) -> Float { return kern::math::abs(a); }, a);
        largest = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n12.index)));
      }
    }
    if (logic_or(math_compare<"<">(kern::to<Float>(largest), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(b.rings[largest].items)), 3.0))) {
      return {false};
    }
    ring = b.rings[largest].items;
    dir = norm2(vector_sub(b.c, vector_make(vector_break(center).x, vector_break(center).y, 0.0)), Vector{1.0, 0.0, 0.0}).Result;
    reach = -1e+30;
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(ring))))); it14.next();) {
      p_2 = ring[n14.index];
      reach = kern::formula([](Float reach, Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return kern::math::max(reach, (((x - x_2) * x_3) + ((y - y_2) * y_3))); }, reach, vector_break(p_2).x, vector_break(b.c).x, vector_break(dir).x, vector_break(p_2).y, vector_break(b.c).y, vector_break(dir).y);
    }
    pivot = vector_make(kern::formula([](Float x, Float x_2, Float reach) -> Float { return (x + (x_2 * reach)); }, vector_break(b.c).x, vector_break(dir).x, reach), kern::formula([](Float y, Float y_2, Float reach) -> Float { return (y + (y_2 * reach)); }, vector_break(b.c).y, vector_break(dir).y, reach), b.new_top);
    h = kern::formula([](Float top, Float new_top) -> Float { return (top - new_top); }, b.top, b.new_top);
    pts = List<Vector>{};
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(ring))))); it15.next();) {
      p_3 = ring[n15.index];
      list_add(pts, vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p_3).x, vector_break(pivot).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p_3).y, vector_break(pivot).y), 0.0));
    }
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<Vector>(pts)), 3.0), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(pts[0]).x, vector_break(pts[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))]).x, vector_break(pts[0]).y, vector_break(pts[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))]).y), 0.0001))) {
      list_remove_at(pts, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts)))));
    }
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(pts)), 3.0)) {
      return {false};
    }
    area = 0.0;
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it16.next();) {
      p_4 = pts[n16.index];
      q_2 = pts[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n16.index), kern::to<Float>(list_length<Vector>(pts))))];
      area += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return ((x * y) - (x_2 * y_2)); }, vector_break(p_4).x, vector_break(q_2).y, vector_break(q_2).x, vector_break(p_4).y);
    }
    if (math_compare<"<">(area, 0.0)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(pts))));
      while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
        held = list_get<Vector>(pts, first);
        pts[first] = list_get<Vector>(pts, last);
        pts[last] = held;
        first += 1;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
      }
    }
    xy = List<Float>{};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it17.next();) {
      p_5 = pts[n17.index];
      place = list_length<Float>(xy);
      item = vector_break(p_5).x;
      item_2 = vector_break(p_5).y;
      list_insert_list(xy, place, list_make<Float, "2">(item, item_2));
    }
    tris = List<Int>{};
    engine_earcut_make(n18);
    Earcut_2 = n18.result;
    earcut = Earcut_2;
    engine_earcut_triangulate(n19, earcut, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts)))), List<Int>{}, 0, tris);
    tris = n19.out;
    if (math_compare<"==">(kern::to<Float>(list_length<Int>(tris)), 0.0)) {
      returned = false;
      if (math_compare<"!=">(kern::to<Float>(earcut), 0.0)) {
        engine_earcut_release(earcut);
      }
      earcut = 0;
      return {returned};
    }
    value = 13482394;
    col = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value)));
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Int>(tris))), 0.5))) {
      a_2 = pts[tris[i]];
      c = pts[tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))]];
      d = pts[tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))]];
      list = list_make<Vector, "3">(a_2, c, d);
      for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(list))))); it20.next();) {
        q_3 = list[n20.index];
        engine_geo_mesh_data::MeshData_push(m, vector_make(vector_break(q_3).x, vector_break(q_3).y, h), Vector{0.0, 0.0, 1.0}, col, vector_make(kern::formula([](Float x) -> Float { return (x * 0.1); }, vector_break(q_3).x), kern::formula([](Float y) -> Float { return (y * 0.1); }, vector_break(q_3).y), 0.0));
      }
      list_2 = list_make<Vector, "3">(a_2, d, c);
      for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(list_2))))); it21.next();) {
        q_4 = list_2[n21.index];
        engine_geo_mesh_data::MeshData_push(m, vector_make(vector_break(q_4).x, vector_break(q_4).y, 0.0), Vector{0.0, 0.0, -1.0}, col, vector_make(kern::formula([](Float x) -> Float { return (x * 0.1); }, vector_break(q_4).x), kern::formula([](Float y) -> Float { return (y * 0.1); }, vector_break(q_4).y), 0.0));
      }
      i += 3;
    }
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it22.next();) {
      a_3 = pts[n22.index];
      c_2 = pts[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n22.index), kern::to<Float>(list_length<Vector>(pts))))];
      e = norm2(vector_sub(c_2, a_3), Vector{1.0, 0.0, 0.0}).Result;
      nrm = vector_make(vector_break(e).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(e).x), 0.0);
      list_3 = list_make<Vector, "6">(vector_make(vector_break(a_3).x, vector_break(a_3).y, 0.0), vector_make(vector_break(c_2).x, vector_break(c_2).y, 0.0), vector_make(vector_break(c_2).x, vector_break(c_2).y, h), vector_make(vector_break(a_3).x, vector_break(a_3).y, 0.0), vector_make(vector_break(c_2).x, vector_break(c_2).y, h), vector_make(vector_break(a_3).x, vector_break(a_3).y, h));
      for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(list_3))))); it23.next();) {
        q_5 = list_3[n23.index];
        engine_geo_mesh_data::MeshData_push(m, q_5, nrm, col, vector_make(kern::formula([](Float x, Float y) -> Float { return ((x * 0.1) + (y * 0.1)); }, vector_break(q_5).x, vector_break(q_5).y), kern::formula([](Float z) -> Float { return (z * 0.1); }, vector_break(q_5).z), 0.0));
      }
    }
    t = Destruction_Torn{0, Destruction_Broken{List<Vector_row>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 1.0, 0.0};
    engine_renderer_allocate_id(n24, self.r_);
    allocate_id = n24.result;
    t.mesh = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material.roughness = 0.9;
    look.material.double_sided = true;
    at = engine_math_math::Mat4_translate(pivot).Result;
    engine_upsert(n25, self.r_, t.mesh, m, look, at.m, List<Float>{});
    upsert = n25.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, t.mesh);
      returned_2 = false;
      if (math_compare<"!=">(kern::to<Float>(earcut), 0.0)) {
        engine_earcut_release(earcut);
      }
      earcut = 0;
      return {returned_2};
    }
    I = kern::formula([](Float intensity) -> Float { return kern::math::min(intensity, 2.4); }, b.intensity);
    t.b = b;
    t.center = center;
    t.dir = vector_make(vector_break(dir).x, vector_break(dir).y, 0.0);
    t.axis = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(dir).y), vector_break(dir).x, 0.0);
    t.pivot = pivot;
    n26 = common::Rng_range(rng, 0.8, 1.1);
    t.tilt = kern::formula([](Float i, Float h, Float the_result) -> Float { return (kern::math::min(1.45, ((0.35 + (0.55 * i)) + (0.25 * kern::math::min(1.0, kern::math::div(h, 30.0))))) * the_result); }, I, h, n26.Result);
    n27 = common::Rng_range(rng, 0.75, 1.15);
    t.speed = kern::formula([](Float i, Float the_result) -> Float { return ((6.0 + (22.0 * i)) * the_result); }, I, n27.Result);
    n28 = common::Rng_range(rng, 1.0, 4.0);
    t.lift = kern::formula([](Float the_result, Float i) -> Float { return (the_result + (3.0 * i)); }, n28.Result, I);
    n29 = common::Rng_range(rng, 0.85, 1.2);
    t.break_at = kern::formula([](Float i, Float the_result) -> Float { return (kern::math::max(0.28, (1.15 - (0.38 * i))) * the_result); }, I, n29.Result);
    list_add(self.torn_, t);
    returned_3 = true;
    if (math_compare<"!=">(kern::to<Float>(earcut), 0.0)) {
      engine_earcut_release(earcut);
    }
    earcut = 0;
    return {returned_3};
  }

  void Destruction_spawn(Destruction& self, Rng& rng, Vector at = Vector{0.0, 0.0, 0.0}, Vector v = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Destruction_Piece p = Destruction_Piece{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false};
    if (math_compare<">">(kern::formula([](Float a, Float kPieces) -> Float { return (!((a < 0.0) != (kPieces < 0.0) ? (kPieces < 0.0) : (a < kPieces))); }, kern::to<Float>(list_length<Destruction_Piece>(self.pieces_)), kern::to<Float>(kPieces)), 0.5)) {
      return;
    }
    p = Destruction_Piece{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false};
    p.p = at;
    p.v = v;
    p.size = size;
    p.yaw = yaw;
    n40 = common::Rng_range(rng, -1.0, 1.0);
    n41 = common::Rng_range(rng, -1.0, 1.0);
    n42 = common::Rng_range(rng, -1.0, 1.0);
    p.axis = logic_select<Vector>(math_compare<">">(vector_length(vector_make(n40.Result, n41.Result, n42.Result)), 9.999999717180685e-10), vector_normalize(vector_make(n40.Result, n41.Result, n42.Result)), Vector{1.0, 0.0, 0.0});
    n43 = common::Rng_range(rng, 0.0, 6.28);
    p.angle = n43.Result;
    n44 = common::Rng_range(rng, -9.0, 9.0);
    p.spin = n44.Result;
    p.tint = tint;
    list_add(self.pieces_, p);
  }

  void Destruction_fracture_launch(Destruction& self, Rng& rng, Vector offset = Vector{0.0, 0.0, 0.0}, Vector center = Vector{0.0, 0.0, 0.0}, Vector outward = Vector{0.0, 0.0, 0.0}, Float I = 0.0, Float boost = 0.0, Vector at = Vector{0.0, 0.0, 0.0}, Vector size = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Vector at_ = Vector{0.0, 0.0, 0.0};
    Vector radial = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    Vector v = Vector{0.0, 0.0, 0.0};
    at_ = at;
    at_ = vector_add(at_, offset);
    radial = norm2(vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(at_).x, vector_break(center).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(at_).y, vector_break(center).y), 0.0), outward).Result;
    n35 = common::Rng_range(rng, 0.55, 1.2);
    speed = kern::formula([](Float i, Float the_result, Float boost) -> Float { return (((9.0 + (30.0 * i)) * the_result) * boost); }, I, n35.Result, boost);
    n36 = common::Rng_range(rng, -5.0, 5.0);
    n37 = common::Rng_range(rng, -5.0, 5.0);
    n38 = common::Rng_range(rng, 2.0, 8.0);
    n39 = common::Rng_range(rng, 0.2, 1.0);
    v = vector_make(kern::formula([](Float x, Float speed, Float the_result) -> Float { return ((x * speed) + the_result); }, vector_break(radial).x, speed, n36.Result), kern::formula([](Float y, Float speed, Float the_result) -> Float { return ((y * speed) + the_result); }, vector_break(radial).y, speed, n37.Result), kern::formula([](Float the_result, Float i, Float the_result_2) -> Float { return (the_result + ((8.0 * i) * the_result_2)); }, n38.Result, I, n39.Result));
    Destruction_spawn(self, rng, at_, v, size, yaw, tint);
  }

  struct fracture_tintResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  fracture_tintResult fracture_tint(Rng& rng, Float ember = 0.0) {
    Float u = 0.0;
    n45 = common::Rng_unit(rng);
    u = n45.Result;
    return {logic_select<Vector>(math_compare<"<">(u, ember), kEmber, logic_select<Vector>(math_compare<"<">(u, 0.35), kDim, kConcrete))};
  }

  void Destruction_fracture(Destruction& self, const Destruction_Broken& b, Rng& rng, Vector center = Vector{0.0, 0.0, 0.0}, Vector offset = Vector{0.0, 0.0, 0.0}, Float boost = 0.0) {
    Float removed = 0.0;
    Vector outward = Vector{0.0, 0.0, 0.0};
    Float I = 0.0;
    List<Destruction_fracture_Seg> segs = List<Destruction_fracture_Seg>{};
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Int parts = 0;
    Float t = 0.0;
    Int floors = 0;
    Int roof = 0;
    Int stride = 0;
    Float the_band = 0.0;
    Int i = 0;
    Destruction_fracture_Seg s = Destruction_fracture_Seg{Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    removed = kern::formula([](Float top, Float new_top) -> Float { return (top - new_top); }, b.top, b.new_top);
    if (math_compare<"<">(removed, 0.5)) {
      return;
    }
    outward = norm2(vector_sub(b.c, vector_make(vector_break(center).x, vector_break(center).y, 0.0)), Vector{1.0, 0.0, 0.0}).Result;
    I = kern::formula([](Float intensity) -> Float { return kern::math::min(intensity, 2.5); }, b.intensity);
    segs = List<Destruction_fracture_Seg>{};
    for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(b.rings))))); it32.next();) {
      for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n32.index]).items))))); it33.next();) {
        skip = false;
        a = b.rings[n32.index].items[n33.index];
        c = b.rings[n32.index].items[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n33.index), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n32.index]).items))))];
        len = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
        if (math_compare<"<">(len, 0.4)) {
          skip = true;
        }
        if (logic_not(skip)) {
          parts = kern::to<Int>(kern::formula([](Float len, Float kPanelMax) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::ceil(kern::math::div(len, kPanelMax)))); }, len, kPanelMax));
          for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(parts)))); it34.next();) {
            t = kern::formula([](Float index, Float parts) -> Float { return kern::math::div((index + 0.5), parts); }, kern::to<Float>(n34.index), kern::to<Float>(parts));
            list_add(segs, struct_make<Destruction_fracture_Seg>(vector_add(a, vector_scale(vector_sub(c, a), t)), kern::formula([](Float len, Float parts) -> Float { return kern::math::div(len, parts); }, len, kern::to<Float>(parts)), kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(c).y, vector_break(a).y, vector_break(c).x, vector_break(a).x)));
          }
        }
      }
    }
    floors = kern::to<Int>(kern::formula([](Float removed, Float kFloor) -> Float { return kern::math::max(1.0, kern::math::wrap32(kern::math::round(kern::math::div(removed, kFloor)))); }, removed, kFloor));
    roof = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::wrap32(kern::math::round(kern::math::div(((x - x_2) * (y - y_2)), 60.0))); }, vector_break(b.hi).x, vector_break(b.lo).x, vector_break(b.hi).y, vector_break(b.lo).y)))), 1.0, 10.0)));
    while (loop(logic_and(math_compare<">">(kern::formula([](Float kMaxPieces, Float a) -> Float { return ((kMaxPieces < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxPieces < a)); }, kern::to<Float>(kMaxPieces), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float floors, Float roof) -> Float { return ((a * floors) + roof); }, kern::to<Float>(list_length<Destruction_fracture_Seg>(segs)), kern::to<Float>(floors), kern::to<Float>(roof)))))), 0.5), math_compare<">">(kern::to<Float>(floors), 1.0)))) {
      floors = kern::to<Int>(kern::formula([](Float floors) -> Float { return kern::math::wrap32(floors - 1.0); }, kern::to<Float>(floors)));
    }
    stride = kern::to<Int>(kern::formula([](Float a, Float kMaxPieces, Float roof) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::ceil(kern::math::div((a < 0.0 ? a + 18446744073709551616.0 : a), kern::math::wrap32(kMaxPieces - roof))))); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float floors) -> Float { return (a * floors); }, kern::to<Float>(list_length<Destruction_fracture_Seg>(segs)), kern::to<Float>(floors))))), kern::to<Float>(kMaxPieces), kern::to<Float>(roof)));
    the_band = kern::formula([](Float removed, Float floors) -> Float { return kern::math::div(removed, floors); }, removed, kern::to<Float>(floors));
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(floors)))); it46.next();) {
      i = kern::to<Int>(kern::formula([](Float index, Float stride) -> Float { return kern::math::mod(index, stride); }, kern::to<Float>(n46.index), kern::to<Float>(stride)));
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Destruction_fracture_Seg>(segs))), 0.5))) {
        s = segs[i];
        n47 = common::Rng_range(rng, 0.85, 1.0);
        n48 = common::Rng_range(rng, 0.8, 1.0);
        n49 = fracture_tint(rng, 0.18);
        Destruction_fracture_launch(self, rng, offset, center, outward, I, boost, vector_make(vector_break(s.m).x, vector_break(s.m).y, kern::formula([](Float new_top, Float the_band, Float index) -> Float { return (new_top + (the_band * (index + 0.5))); }, b.new_top, the_band, kern::to<Float>(n46.index))), vector_make(kern::formula([](Float len, Float the_result) -> Float { return (len * the_result); }, s.len, n47.Result), 0.45, kern::formula([](Float the_band, Float the_result) -> Float { return (the_band * the_result); }, the_band, n48.Result)), s.yaw, n49.Result);
        i += stride;
      }
    }
    for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(roof)))); it50.next();) {
      n51 = common::Rng_range(rng, vector_break(b.lo).x, vector_break(b.hi).x);
      n52 = common::Rng_range(rng, vector_break(b.lo).y, vector_break(b.hi).y);
      n53 = common::Rng_range(rng, 3.0, 7.0);
      n54 = common::Rng_range(rng, 3.0, 7.0);
      n55 = common::Rng_unit(rng);
      n56 = fracture_tint(rng, 0.25);
      Destruction_fracture_launch(self, rng, offset, center, outward, I, boost, vector_make(n51.Result, n52.Result, kern::formula([](Float top) -> Float { return (top - 0.3); }, b.top)), vector_make(n53.Result, n54.Result, 0.5), kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, n55.Result), n56.Result);
    }
  }

  void Destruction_collapse(Destruction& self, const Destruction_Broken& b, CityStream& city, Rng& rng, Vector center = Vector{0.0, 0.0, 0.0}) {
    Float removed = 0.0;
    Bool both = false;
    removed = kern::formula([](Float top, Float new_top) -> Float { return (top - new_top); }, b.top, b.new_top);
    if (math_compare<"<">(removed, 0.5)) {
      return;
    }
    both = logic_and(logic_and(math_compare<">">(removed, 2.0), math_compare<"<">(b.intensity, 2.4)), math_compare<">">(kern::formula([](Float a, Float kTorn) -> Float { return ((a < 0.0) != (kTorn < 0.0) ? (kTorn < 0.0) : (a < kTorn)); }, kern::to<Float>(list_length<Destruction_Torn>(self.torn_)), kern::to<Float>(kTorn)), 0.5));
    if (both) {
      n31 = Destruction_tear(self, b, rng, center);
      both = n31.Result;
    }
    if (both) {
      return;
    }
    Destruction_fracture(self, b, rng, center, Vector{0.0, 0.0, 0.0}, 1.0);
  }

  void Destruction_free_torn(Destruction& self, Destruction_Torn& t) {
    Bool remove = false;
    if (logic_and(math_compare<"!=">(kern::to<Float>(t.mesh), 0.0), math_compare<"!=">(kern::to<Float>(self.r_), 0.0))) {
      engine_renderer_remove(n57, self.r_, t.mesh);
      remove = n57.result;
    }
    t.mesh = 0;
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct { Int result = 0; } n18;
  struct { List<Int> out = List<Int>{}; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct { Int result = 0; } n24;
  struct { Bool result = false; } n25;
  common::Rng_rangeResult n26;
  common::Rng_rangeResult n27;
  common::Rng_rangeResult n28;
  common::Rng_rangeResult n29;
  Destruction_tearResult n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  common::Rng_rangeResult n35;
  common::Rng_rangeResult n36;
  common::Rng_rangeResult n37;
  common::Rng_rangeResult n38;
  common::Rng_rangeResult n39;
  common::Rng_rangeResult n40;
  common::Rng_rangeResult n41;
  common::Rng_rangeResult n42;
  common::Rng_rangeResult n43;
  common::Rng_rangeResult n44;
  common::Rng_unitResult n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  common::Rng_rangeResult n47;
  common::Rng_rangeResult n48;
  fracture_tintResult n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  common::Rng_rangeResult n51;
  common::Rng_rangeResult n52;
  common::Rng_rangeResult n53;
  common::Rng_rangeResult n54;
  common::Rng_unitResult n55;
  fracture_tintResult n56;
  struct { Bool result = false; } n57;
};
