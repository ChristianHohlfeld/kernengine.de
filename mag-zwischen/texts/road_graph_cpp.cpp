struct road_graph_cpp : virtual kern::Library, virtual city_stream, virtual road_graph {
  // ---- variables ----
  /// vertices closer than this (same grid cell or a neighbour) merge
  KERN_CONST Float kCell = 3.5;

  // ---- functions ----
  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(I32 x = 0, I32 y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct distResult { Float Result = 0.0; };
  KERN_PURE distResult dist(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct beforeResult { Bool Result = false; };
  KERN_PURE beforeResult before(const Float_Int_pair& x, const Float_Int_pair& y) {
    return {logic_or(math_compare<"<">(x.first, y.first), logic_and(logic_not(math_compare<"<">(y.first, x.first)), math_compare<"<">(kern::to<Float>(x.second), kern::to<Float>(y.second))))};
  }

  void heap_push(List<Float_Int_pair>& h, Float_Int_pair x = Float_Int_pair{0.0, 0}) {
    Int i = 0;
    Bool stop = false;
    Int up = 0;
    Float_Int_pair swapped = Float_Int_pair{0.0, 0};
    i = list_length<Float_Int_pair>(h);
    list_add(h, x);
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i) -> Float { return ((0.0 < 0.0) != (i < 0.0) ? (i < 0.0) : (0.0 < i)); }, kern::to<Float>(i)), 0.5)))) {
      up = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div((i - 1.0), 2.0)); }, kern::to<Float>(i)));
      if (logic_not(before(h[i], h[up]).Result)) {
        stop = true;
      }
      if (logic_not(stop)) {
        swapped = h[i];
        h[i] = h[up];
        h[up] = swapped;
        i = up;
      }
    }
  }

  struct heap_popResult { Float_Int_pair Result = Float_Int_pair{0.0, 0}; };
  heap_popResult heap_pop(List<Float_Int_pair>& h) {
    Float_Int_pair top = Float_Int_pair{0.0, 0};
    Int i = 0;
    Bool stop = false;
    Int l = 0;
    Int r = 0;
    Int m = 0;
    Float_Int_pair swapped = Float_Int_pair{0.0, 0};
    top = h[0];
    h[0] = h[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Int_pair>(h))))];
    list_remove_at(h, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float_Int_pair>(h)))));
    i = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return ((2.0 * i) + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Float_Int_pair>(h))), 0.5)))) {
      l = kern::to<Int>(kern::formula([](Float i) -> Float { return ((2.0 * i) + 1.0); }, kern::to<Float>(i)));
      r = kern::to<Int>(kern::formula([](Float l) -> Float { return (l + 1.0); }, kern::to<Float>(l)));
      m = kern::to<Int>(kern::formula([](Float a, Float l, Float i) -> Float { return ((a != 0.0) ? l : i); }, kern::to<Float>(before(h[l], h[i]).Result), kern::to<Float>(l), kern::to<Float>(i)));
      if (logic_and(math_compare<">">(kern::formula([](Float r, Float a) -> Float { return ((r < 0.0) != (a < 0.0) ? (a < 0.0) : (r < a)); }, kern::to<Float>(r), kern::to<Float>(list_length<Float_Int_pair>(h))), 0.5), before(h[r], h[m]).Result)) {
        m = r;
      }
      if (math_compare<"==">(kern::to<Float>(m), kern::to<Float>(i))) {
        stop = true;
      }
      if (logic_not(stop)) {
        swapped = h[i];
        h[i] = h[m];
        h[m] = swapped;
        i = m;
      }
    }
    return {top};
  }

  struct lane_signatureResult { Int64 Result = Int64{0x0}; };
  KERN_PURE lane_signatureResult lane_signature(const List<Lane>& lanes) {
    Int64 h = Int64{0x0};
    h = Int64{0x14650fb0739d0383};
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it10.next();) {
      h = int64_xor(h, int64_or(int64_shift_left(int64_of_int(n10.index), 1), int64_of_int(kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(lanes[n10.index].alive))))));
      h = int64_mul(h, Int64{0x100000001b3});
    }
    return {h};
  }

  struct RoadGraph_node_atResult { I32 Result = 0; };
  RoadGraph_node_atResult RoadGraph_node_at(RoadGraph& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    I32 cx = 0;
    I32 cy = 0;
    Int it = 0;
    I32 id = 0;
    Int put = 0;
    Int64 key = Int64{0x0};
    cx = kern::to<Int>(kern::formula([](Float x, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kCell))); }, vector_break(p).x, kCell));
    cy = kern::to<Int>(kern::formula([](Float y, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kCell))); }, vector_break(p).y, kCell));
    for (auto it11 = flow_for(n11, -1, 1); it11.next();) {
      for (auto it12 = flow_for(n12, -1, 1); it12.next();) {
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.cells_, cell_key(kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return (cx + index); }, kern::to<Float>(cx), kern::to<Float>(n11.index))), kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return (cy + index); }, kern::to<Float>(cy), kern::to<Float>(n12.index)))).Result).index), kern::to<Float>(map_length<Int64, Int>(self.cells_))));
        if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Int>(self.cells_)))), math_compare<"<">(dist(self.nodes_[self.cells_[it]].p, p).Result, kCell))) {
          return {self.cells_[it]};
        }
      }
    }
    id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_))));
    list_add(self.nodes_, struct_make<RoadGraph_Node>(p, List<Int_Float_pair>{}));
    put = id;
    key = cell_key(cx, cy).Result;
    map_add(n13, self.cells_, key);
    self.cells_[n13.index] = put;
    return {id};
  }

  void RoadGraph_rebuild_link(RoadGraph& self, Float len = 0.0, I32 a = 0, I32 b = 0) {
    Int at = 0;
    at = a;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.nodes_[at].edges))))); it18.next();) {
      if (math_compare<"==">(kern::to<Float>(self.nodes_[at].edges[n18.index].first), kern::to<Float>(b))) {
        return;
      }
    }
    list_add(self.nodes_[a].edges, struct_make<Int_Float_pair>(b, len));
  }

  void RoadGraph_rebuild(RoadGraph& self, const List<Lane>& lanes) {
    Bool skip = false;
    I32 prev = 0;
    Bool skip_2 = false;
    I32 cur = 0;
    Float len = 0.0;
    list_clear(self.nodes_);
    map_clear(self.cells_);
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it14.next();) {
      skip = false;
      if (logic_or(logic_not(lanes[n14.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(lanes[n14.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n15 = RoadGraph_node_at(self, lanes[n14.index].points[0]);
        prev = n15.Result;
        for (auto it16 = flow_for(n16, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(lanes[n14.index].points))))); it16.next();) {
          skip_2 = false;
          n17 = RoadGraph_node_at(self, lanes[n14.index].points[n16.index]);
          cur = n17.Result;
          if (math_compare<"==">(kern::to<Float>(cur), kern::to<Float>(prev))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            len = dist(self.nodes_[prev].p, self.nodes_[cur].p).Result;
            RoadGraph_rebuild_link(self, len, prev, cur);
            RoadGraph_rebuild_link(self, len, cur, prev);
            prev = cur;
          }
        }
      }
    }
    self.built_from = lane_signature(lanes).Result;
  }

  struct RoadGraph_nearestResult { I32 Result = 0; };
  KERN_PURE RoadGraph_nearestResult RoadGraph_nearest(const RoadGraph& self, Vector p = Vector{0.0, 0.0, 0.0}, Float max_m = 0.0) {
    I32 best = 0;
    Float bd = 0.0;
    Float d = 0.0;
    best = -1;
    bd = max_m;
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_))))); it19.next();) {
      d = dist(self.nodes_[n19.index].p, p).Result;
      if (math_compare<"<">(d, bd)) {
        bd = d;
        best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index)));
      }
    }
    return {best};
  }

  struct RoadGraph_pathResult { Bool Result = false; };
  RoadGraph_pathResult RoadGraph_path(const RoadGraph& self, List<Vector>& out, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}) {
    I32 a = 0;
    I32 b = 0;
    Int n = 0;
    List<Float> g = List<Float>{};
    List<Float> made = List<Float>{};
    List<I32> came = List<I32>{};
    List<I32> made_2 = List<I32>{};
    List<U8> closed = List<U8>{};
    List<U8> made_3 = List<U8>{};
    List<Float_Int_pair> open = List<Float_Int_pair>{};
    U32 expanded = 0;
    Bool stop = false;
    Bool skip = false;
    I32 cur = 0;
    Bool either = false;
    Int at = 0;
    Float ng = 0.0;
    List<Vector> rev = List<Vector>{};
    I32 c = 0;
    list_clear(out);
    a = RoadGraph_nearest(self, from, 80.0).Result;
    b = RoadGraph_nearest(self, to, 80.0).Result;
    if (logic_or(math_compare<"<">(kern::to<Float>(a), 0.0), math_compare<"<">(kern::to<Float>(b), 0.0))) {
      return {false};
    }
    n = list_length<RoadGraph_Node>(self.nodes_);
    made = List<Float>{};
    list_resize(made, n, 1e+30);
    g = made;
    made_2 = List<I32>{};
    list_resize(made_2, n, -1);
    came = made_2;
    made_3 = List<U8>{};
    list_resize(made_3, n, 0);
    closed = made_3;
    open = List<Float_Int_pair>{};
    g[a] = 0.0;
    heap_push(open, struct_make<Float_Int_pair>(dist(self.nodes_[a].p, self.nodes_[b].p).Result, a));
    expanded = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), logic_not(math_compare<"==">(kern::to<Float>(list_length<Float_Int_pair>(open)), 0.0))))) {
      skip = false;
      n20 = heap_pop(open);
      cur = struct_break<Float_Int_pair>(n20.Result).second;
      if (math_compare<"!=">(kern::to<Float>(closed[cur]), 0.0)) {
        skip = true;
      }
      if (logic_not(logic_or(stop, skip))) {
        closed[cur] = 1;
        either = math_compare<"==">(kern::to<Float>(cur), kern::to<Float>(b));
        if (logic_not(either)) {
          expanded += 1;
          either = math_compare<">">(kern::to<Float>(expanded), 20000.0);
        }
        if (either) {
          stop = true;
        }
        if (logic_not(logic_or(stop, skip))) {
          at = cur;
          for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.nodes_[at].edges))))); it21.next();) {
            ng = kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, g[cur], self.nodes_[at].edges[n21.index].second);
            if (math_compare<"<">(ng, g[self.nodes_[at].edges[n21.index].first])) {
              g[self.nodes_[at].edges[n21.index].first] = ng;
              came[self.nodes_[at].edges[n21.index].first] = cur;
              heap_push(open, struct_make<Float_Int_pair>(kern::formula([](Float ng, Float the_result) -> Float { return (ng + the_result); }, ng, dist(self.nodes_[self.nodes_[at].edges[n21.index].first].p, self.nodes_[b].p).Result), self.nodes_[at].edges[n21.index].first));
            }
          }
        }
      }
    }
    if (logic_and(math_compare<"!=">(kern::to<Float>(a), kern::to<Float>(b)), math_compare<"<">(kern::to<Float>(came[b]), 0.0))) {
      return {false};
    }
    rev = List<Vector>{};
    c = b;
    while (loop(math_compare<">=">(kern::to<Float>(c), 0.0))) {
      list_add(rev, self.nodes_[c].p);
      c = kern::to<Int>(kern::formula([](Float c, Float a, Float a_2) -> Float { return ((c == a) ? (-1.0) : a_2); }, kern::to<Float>(c), kern::to<Float>(a), kern::to<Float>(came[c])));
    }
    list_add(out, from);
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(rev))))); it22.next();) {
      list_add(out, rev[kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return ((a - 1.0) - index); }, kern::to<Float>(list_length<Vector>(rev)), kern::to<Float>(n22.index)))]);
    }
    list_add(out, to);
    return {true};
  }

  struct RoadGraph_pursuit_pointResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE RoadGraph_pursuit_pointResult RoadGraph_pursuit_point(const List<Vector>& path, Vector p = Vector{0.0, 0.0, 0.0}, Float look = 0.0) {
    Int bi = 0;
    Float bt = 0.0;
    Float bd = 0.0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    Float dd = 0.0;
    Float left = 0.0;
    Float t_2 = 0.0;
    Int i_2 = 0;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector b_2 = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float rest = 0.0;
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(path)), 0.0)) {
      return {p};
    }
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(path)), 2.0)) {
      return {path[0]};
    }
    bi = 0;
    bt = 0.0;
    bd = 1e+30;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(path))), 0.5))) {
      a = path[i];
      b = path[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
      d = vector_sub(b, a);
      l2 = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, ((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
      t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
      dd = dist(p, vector_add(a, vector_scale(d, t))).Result;
      if (math_compare<"<">(dd, bd)) {
        bd = dd;
        bi = i;
        bt = t;
      }
      i += 1;
    }
    left = look;
    t_2 = bt;
    i_2 = bi;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2))))), kern::to<Float>(list_length<Vector>(path))), 0.5))) {
      a_2 = path[i_2];
      b_2 = path[kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2)))];
      l = kern::formula([](Float the_result) -> Float { return kern::math::max(1e-06, the_result); }, dist(a_2, b_2).Result);
      rest = kern::formula([](Float t_2, Float l) -> Float { return ((1.0 - t_2) * l); }, t_2, l);
      if (math_compare<">=">(rest, left)) {
        return {vector_add(a_2, vector_scale(vector_sub(b_2, a_2), kern::formula([](Float t_2, Float left, Float l) -> Float { return (t_2 + kern::math::div(left, l)); }, t_2, left, l)))};
      }
      left = kern::formula([](Float left, Float rest) -> Float { return (left - rest); }, left, rest);
      t_2 = 0.0;
      i_2 += 1;
    }
    return {path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool added = false; Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  RoadGraph_node_atResult n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  RoadGraph_node_atResult n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  heap_popResult n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
};
