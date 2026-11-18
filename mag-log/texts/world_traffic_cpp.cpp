struct world_traffic_cpp : virtual kern::Library, virtual city_stream, virtual world_traffic {
  // ---- types ----
  KERN_STRUCT(TrafficRules_rebuild_Node) { Vector p = Vector{0.0, 0.0, 0.0}; List<TrafficRules_Arm> arms = List<TrafficRules_Arm>{}; };

  // ---- variables ----
  /// street vertices closer than this are one node
  KERN_CONST Float kMerge = 3.0;
  /// junction lookup grid
  KERN_CONST Float kCell = 40.0;

  // ---- functions ----
  struct cellResult { Int64 Result = Int64{0x0}; };
  /// The Blueprint's distances and times the loops below compare with (read when it is bound).
  KERN_PURE cellResult cell(Int x = 0, Int y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct rebuild_node_atResult { Int Result = 0; };
  rebuild_node_atResult rebuild_node_at(Map<Int64, List<Int>>& cells, List<TrafficRules_rebuild_Node>& nodes, Vector p = Vector{0.0, 0.0, 0.0}) {
    Int cx = 0;
    Int cy = 0;
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    Int i = 0;
    List<Int> t = List<Int>{};
    Int64 key = Int64{0x0};
    List<Int> put = List<Int>{};
    cx = kern::to<Int>(kern::formula([](Float x, Float kMerge) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kMerge))); }, vector_break(p).x, kMerge));
    cy = kern::to<Int>(kern::formula([](Float y, Float kMerge) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kMerge))); }, vector_break(p).y, kMerge));
    for (auto it10 = flow_for(n10, -1, 1); it10.next();) {
      for (auto it11 = flow_for(n11, -1, 1); it11.next();) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(cells, cell(kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return (cx + index); }, kern::to<Float>(cx), kern::to<Float>(n11.index))), kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return (cy + index); }, kern::to<Float>(cy), kern::to<Float>(n10.index)))).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(cells))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(cells)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(cells[at]))))); it12.next();) {
            i = cells[at][n12.index];
            if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(nodes[i].p).x, vector_break(p).x, vector_break(nodes[i].p).y, vector_break(p).y), kMerge)) {
              return {i};
            }
          }
        }
      }
    }
    list_add(nodes, struct_make<TrafficRules_rebuild_Node>(p, List<TrafficRules_Arm>{}));
    key = cell(cx, cy).Result;
    map_add(n13, cells, key);
    put = t;
    t = cells[n13.index];
    list_add(t, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<TrafficRules_rebuild_Node>(nodes)))));
    cells[n13.index] = put;
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<TrafficRules_rebuild_Node>(nodes))))};
  }

  void rebuild_arm(List<TrafficRules_rebuild_Node>& nodes, Map<Int64, List<Int>>& cells, const Lane& lanes_n14_index, Vector p = Vector{0.0, 0.0, 0.0}, Vector q = Vector{0.0, 0.0, 0.0}) {
    Float d = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Int at = 0;
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(q).x, vector_break(p).x, vector_break(q).y, vector_break(p).y);
    if (math_compare<"<">(d, 0.5)) {
      return;
    }
    dir = vector_make(kern::formula([](Float x, Float x_2, Float d) -> Float { return kern::math::div((x - x_2), d); }, vector_break(q).x, vector_break(p).x, d), kern::formula([](Float y, Float y_2, Float d) -> Float { return kern::math::div((y - y_2), d); }, vector_break(q).y, vector_break(p).y, d), 0.0);
    n16 = rebuild_node_at(cells, nodes, p);
    at = n16.Result;
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficRules_Arm>(nodes[at].arms))))); it17.next();) {
      if (math_compare<">">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(nodes[at].arms[n17.index].dir).x, vector_break(dir).x, vector_break(nodes[at].arms[n17.index].dir).y, vector_break(dir).y), 0.97)) {
        return;
      }
    }
    n18 = rebuild_node_at(cells, nodes, p);
    list_add(nodes[n18.Result].arms, struct_make<TrafficRules_Arm>(dir, lanes_n14_index.width, Vector{0.0, 0.0, 0.0}, false));
  }

  void TrafficRules_near(const TrafficRules& self, List<Int>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Int cx0 = 0;
    Int cx1 = 0;
    Int cy0 = 0;
    Int cy1 = 0;
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    Int j = 0;
    list_clear(out);
    cx0 = kern::to<Int>(kern::formula([](Float x, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((x - r), kCell))); }, vector_break(p).x, r, kCell));
    cx1 = kern::to<Int>(kern::formula([](Float x, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((x + r), kCell))); }, vector_break(p).x, r, kCell));
    cy0 = kern::to<Int>(kern::formula([](Float y, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((y - r), kCell))); }, vector_break(p).y, r, kCell));
    cy1 = kern::to<Int>(kern::formula([](Float y, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((y + r), kCell))); }, vector_break(p).y, r, kCell));
    for (auto it32 = flow_for(n32, cy0, cy1); it32.next();) {
      for (auto it33 = flow_for(n33, cx0, cx1); it33.next();) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.grid_, cell(n33.index, n32.index).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.grid_[at]))))); it34.next();) {
            j = self.grid_[at][n34.index];
            if (math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.junctions_[j].p).x, vector_break(p).x, vector_break(self.junctions_[j].p).y, vector_break(p).y), r)) {
              list_add(out, j);
            }
          }
        }
      }
    }
  }

  struct pursuit_alongResult { Float Result = 0.0; };
  pursuit_alongResult pursuit_along(Vector& t, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    d = vector_sub(to, from);
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
    if (math_compare<">">(l, 0.001)) {
      t = vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l));
    }
    return {l};
  }

  struct TrafficRules_pursuitResult { TrafficRules_Pursuit Result = TrafficRules_Pursuit{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, false, 0.0}; };
  TrafficRules_pursuitResult TrafficRules_pursuit(const Lane& lane, Int index = 0, Int dir = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Float look = 0.0, Float off = 0.0) {
    Int dir_ = 0;
    TrafficRules_Pursuit out = TrafficRules_Pursuit{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, false, 0.0};
    Int n = 0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Float at = 0.0;
    Float s = 0.0;
    Vector cur = Vector{0.0, 0.0, 0.0};
    Float left = 0.0;
    Float want = 0.0;
    Int k = 0;
    Bool stop = false;
    dir_ = dir;
    out = TrafficRules_Pursuit{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, false, 0.0};
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(lane.points))));
    if (math_compare<"<">(kern::to<Float>(n), 2.0)) {
      out.target = logic_select<Vector>(math_compare<"!=">(kern::to<Float>(n), 0.0), lane.points[0], pos);
      out.passed = true;
      return {out};
    }
    dir_ = kern::to<Int>(kern::formula([](Float dir_) -> Float { return ((dir_ < 0.0) ? (-1.0) : 1.0); }, kern::to<Float>(dir_)));
    i = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(index), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))));
    if (logic_or(math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i, Float dir_) -> Float { return (i - dir_); }, kern::to<Float>(i), kern::to<Float>(dir_))))), 0.0), math_compare<">=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i, Float dir_) -> Float { return (i - dir_); }, kern::to<Float>(i), kern::to<Float>(dir_))))), kern::to<Float>(n)))) {
      i += dir_;
    }
    a = lane.points[kern::to<Int>(kern::formula([](Float i, Float dir_) -> Float { return (i - dir_); }, kern::to<Float>(i), kern::to<Float>(dir_)))];
    b = lane.points[i];
    t = Vector{1.0, 0.0, 0.0};
    n35 = pursuit_along(t, a, b);
    len = n35.Result;
    at = kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(pos).x, vector_break(a).x, vector_break(t).x, vector_break(pos).y, vector_break(a).y, vector_break(t).y);
    out.passed = logic_select<Bool>(math_compare<"!=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(index), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))))))), false, math_compare<">=">(at, kern::formula([](Float len) -> Float { return (len - 0.5); }, len)));
    s = kern::formula([](Float at, Float len) -> Float { return (at < 0.0 ? 0.0 : (len < at ? len : at)); }, at, len);
    cur = vector_add(a, vector_scale(t, s));
    left = kern::formula([](Float len, Float s) -> Float { return (len - s); }, len, s);
    want = kern::formula([](Float look) -> Float { return kern::math::max(0.0, look); }, look);
    out.end = left;
    k = i;
    while (loop(logic_and(math_compare<">=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float dir_) -> Float { return (k + dir_); }, kern::to<Float>(k), kern::to<Float>(dir_))))), 0.0), math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float dir_) -> Float { return (k + dir_); }, kern::to<Float>(k), kern::to<Float>(dir_))))), kern::to<Float>(n))))) {
      out.end = kern::formula([](Float end, Float x, Float x_2, Float y, Float y_2) -> Float { return (end + kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, out.end, vector_break(lane.points[kern::to<Int>(kern::formula([](Float k, Float dir_) -> Float { return (k + dir_); }, kern::to<Float>(k), kern::to<Float>(dir_)))]).x, vector_break(lane.points[k]).x, vector_break(lane.points[kern::to<Int>(kern::formula([](Float k, Float dir_) -> Float { return (k + dir_); }, kern::to<Float>(k), kern::to<Float>(dir_)))]).y, vector_break(lane.points[k]).y);
      k += dir_;
    }
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(want, left)))) {
      if (logic_or(math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i, Float dir_) -> Float { return (i + dir_); }, kern::to<Float>(i), kern::to<Float>(dir_))))), 0.0), math_compare<">=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i, Float dir_) -> Float { return (i + dir_); }, kern::to<Float>(i), kern::to<Float>(dir_))))), kern::to<Float>(n)))) {
        want = left;
        stop = true;
      }
      if (logic_not(stop)) {
        want = kern::formula([](Float want, Float left) -> Float { return (want - left); }, want, left);
        cur = b;
        a = b;
        i += dir_;
        b = lane.points[i];
        n36 = pursuit_along(t, a, b);
        left = n36.Result;
      }
    }
    out.tangent = t;
    out.target = vector_add(vector_add(cur, vector_scale(t, want)), vector_scale(vector_make(vector_break(t).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(t).x), 0.0), off));
    return {out};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool added = false; Int index = 0; } n13;
  rebuild_node_atResult n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  rebuild_node_atResult n18;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  pursuit_alongResult n35;
  pursuit_alongResult n36;
};
