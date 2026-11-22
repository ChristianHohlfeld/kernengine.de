struct world_traffic_cpp : virtual kern::Library, virtual blueprint_call, virtual city_stream, virtual world_traffic {
  // ---- types ----
  KERN_STRUCT(TrafficRules_rebuild_Seg) { Vector a = Vector{0.0, 0.0, 0.0}; Vector b = Vector{0.0, 0.0, 0.0}; Float width = 0.0; U32 lane = 0; };
  KERN_STRUCT(TrafficRules_rebuild_Node) { Vector p = Vector{0.0, 0.0, 0.0}; List<TrafficRules_Arm> arms = List<TrafficRules_Arm>{}; };

  // ---- variables ----
  /// street vertices closer than this are one node
  KERN_CONST Float kMerge = 3.0;
  /// junction lookup grid
  KERN_CONST Float kCell = 40.0;

  // ---- functions ----
  struct Traffic_JunctionLightsResult { Bool lights = false; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("JunctionLights") Traffic_JunctionLightsResult Traffic_JunctionLights(Float widest = 0.0);

  struct Traffic_JunctionRadiusResult { Float radius = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("JunctionRadius") Traffic_JunctionRadiusResult Traffic_JunctionRadius(Float widest = 0.0);

  struct Traffic_LightResult { Int light = 0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("Light") Traffic_LightResult Traffic_Light(Float time = 0.0, Float offset = 0.0, Float headingDot = 0.0);

  struct Traffic_SideReachResult { Float reach = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("SideReach") Traffic_SideReachResult Traffic_SideReach(Bool bus = false);

  struct Traffic_BodiesResult { Float length = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("Bodies") Traffic_BodiesResult Traffic_Bodies(Bool bus = false, Bool otherBus = false);

  struct Traffic_GapSpeedResult { Float speed = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("GapSpeed") Traffic_GapSpeedResult Traffic_GapSpeed(Float gap = 0.0);

  struct Traffic_YieldReachResult { Float reach = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("YieldReach") Traffic_YieldReachResult Traffic_YieldReach(Bool bus = false, Float speed_2 = 0.0);

  struct Traffic_YieldHalfWidthResult { Float half = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("YieldHalfWidth") Traffic_YieldHalfWidthResult Traffic_YieldHalfWidth(Bool bus = false);

  struct Traffic_HonkResult { Bool honk = false; Float next = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("Honk") Traffic_HonkResult Traffic_Honk(Bool byPlayer = false, Float since = 0.0, Float next = 0.0, Float now = 0.0, Float roll = 0.0);

  struct Traffic_MustStopResult { Bool stop = false; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("MustStop") Traffic_MustStopResult Traffic_MustStop(Int light = 0, Float stop_2 = 0.0, Float speed_2 = 0.0);

  struct Traffic_StopSpeedResult { Float speed = 0.0; };
  /// (of the Blueprint of the actor Traffic, called on it)
  KERN_EXTERN_AS("StopSpeed") Traffic_StopSpeedResult Traffic_StopSpeed(Float stop_2 = 0.0);

  struct cellResult { Int64 Result = Int64{0x0}; };
  /// The Blueprint's distances and times the loops below compare with (read when it is bound).
  KERN_PURE cellResult cell(I32 x = 0, I32 y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct rebuild_node_atResult { I32 Result = 0; };
  rebuild_node_atResult rebuild_node_at(Map<Int64, List<I32>>& cells, List<TrafficRules_rebuild_Node>& nodes, Vector p = Vector{0.0, 0.0, 0.0}) {
    I32 cx = 0;
    I32 cy = 0;
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    I32 i = 0;
    List<I32> t = List<I32>{};
    Int64 key = Int64{0x0};
    List<I32> put = List<I32>{};
    cx = kern::to<Int>(kern::formula([](Float x, Float kMerge) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kMerge))); }, vector_break(p).x, kMerge));
    cy = kern::to<Int>(kern::formula([](Float y, Float kMerge) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kMerge))); }, vector_break(p).y, kMerge));
    for (auto it10 = flow_for(n10, -1, 1); it10.next();) {
      for (auto it11 = flow_for(n11, -1, 1); it11.next();) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<I32>>(cells, cell(kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return (cx + index); }, kern::to<Float>(cx), kern::to<Float>(n11.index))), kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return (cy + index); }, kern::to<Float>(cy), kern::to<Float>(n10.index)))).Result).index), kern::to<Float>(map_length<Int64, List<I32>>(cells))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<I32>>(cells)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(cells[at]))))); it12.next();) {
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
    t = cells[n13.index];
    list_add(t, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<TrafficRules_rebuild_Node>(nodes)))));
    put = t;
    cells[n13.index] = put;
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<TrafficRules_rebuild_Node>(nodes))))};
  }

  void rebuild_arm(List<TrafficRules_rebuild_Node>& nodes, Map<Int64, List<I32>>& cells, const Lane& lanes_n14_index, Vector p = Vector{0.0, 0.0, 0.0}, Vector q = Vector{0.0, 0.0, 0.0}) {
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

  void TrafficRules_rebuild(TrafficRules& self, const List<Lane>& lanes) {
    List<TrafficRules_rebuild_Node> nodes = List<TrafficRules_rebuild_Node>{};
    Map<Int64, List<I32>> cells = Map<Int64, List<I32>>{};
    Bool skip = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    List<TrafficRules_rebuild_Seg> segs = List<TrafficRules_rebuild_Seg>{};
    Map<Int64, List<U32>> buckets = Map<Int64, List<U32>>{};
    Bool skip_2 = false;
    Lane l = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    Int i = 0;
    TrafficRules_rebuild_Seg s = TrafficRules_rebuild_Seg{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0};
    U32 si = 0;
    I32 x0 = 0;
    I32 x1 = 0;
    I32 y0 = 0;
    I32 y1 = 0;
    List<U32> t = List<U32>{};
    Int64 key = Int64{0x0};
    List<U32> put = List<U32>{};
    Bool skip_3 = false;
    TrafficRules_rebuild_Seg s_2 = TrafficRules_rebuild_Seg{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0};
    TrafficRules_rebuild_Seg t_2 = TrafficRules_rebuild_Seg{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    Float den = 0.0;
    Vector w = Vector{0.0, 0.0, 0.0};
    Float u = 0.0;
    Float v = 0.0;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    I32 ni = 0;
    U32 item = 0;
    U32 item_2 = 0;
    Fixed<U32, 2> items = Fixed<U32, 2>{};
    U32 gi = 0;
    TrafficRules_rebuild_Seg g = TrafficRules_rebuild_Seg{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 2> items_2 = Fixed<Vector, 2>{};
    Bool skip_4 = false;
    Vector end = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Bool dup = false;
    Int at = 0;
    Bool skip_5 = false;
    TrafficRules_Junction j = TrafficRules_Junction{Vector{0.0, 0.0, 0.0}, List<TrafficRules_Arm>{}, false, 0.0, 8.0, Vector{1.0, 0.0, 0.0}, -1, -99.0};
    Float wmax = 0.0;
    Float best = 0.0;
    Bool skip_6 = false;
    Float bearing = 0.0;
    Int ai = 0;
    Bool skip_7 = false;
    Float aa = 0.0;
    Float best_2 = 0.0;
    Int next = 0;
    Bool skip_8 = false;
    TrafficRules_Arm b = TrafficRules_Arm{Vector{0.0, 0.0, 0.0}, 6.0, Vector{0.0, 0.0, 0.0}, false};
    Float d_2 = 0.0;
    Float half = 0.0;
    Float s_3 = 0.0;
    Float r_2 = 0.0;
    U32 h = 0;
    I32 id = 0;
    List<I32> t_3 = List<I32>{};
    Int64 key_2 = Int64{0x0};
    List<I32> put_2 = List<I32>{};
    list_clear(self.junctions_);
    map_clear(self.grid_);
    map_clear(self.blocked_);
    nodes = List<TrafficRules_rebuild_Node>{};
    cells = Map<Int64, List<I32>>{};
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it14.next();) {
      skip = false;
      if (logic_or(logic_not(lanes[n14.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(lanes[n14.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(lanes[n14.index].points))))); it15.next();) {
          p = lanes[n14.index].points[n15.index];
          if (math_compare<">">(kern::formula([](Float index) -> Float { return ((0.0 < 0.0) != (index < 0.0) ? (index < 0.0) : (0.0 < index)); }, kern::to<Float>(n15.index)), 0.5)) {
            rebuild_arm(nodes, cells, lanes[n14.index], p, lanes[n14.index].points[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n15.index)))]);
          }
          if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n15.index))))), kern::to<Float>(list_length<Vector>(lanes[n14.index].points))), 0.5)) {
            rebuild_arm(nodes, cells, lanes[n14.index], p, lanes[n14.index].points[kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n15.index)))]);
          }
        }
      }
    }
    segs = List<TrafficRules_rebuild_Seg>{};
    buckets = Map<Int64, List<U32>>{};
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it19.next();) {
      skip_2 = false;
      l = lanes[n19.index];
      if (logic_or(logic_not(l.alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(l.points)), 2.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(l.points))), 0.5))) {
          s = struct_make<TrafficRules_rebuild_Seg>(l.points[i], l.points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], l.width, n19.index);
          si = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<TrafficRules_rebuild_Seg>(segs))));
          list_add(segs, s);
          x0 = kern::to<Int>(kern::formula([](Float x, Float x_2, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(kern::math::min(x, x_2), kCell))); }, vector_break(s.a).x, vector_break(s.b).x, kCell));
          x1 = kern::to<Int>(kern::formula([](Float x, Float x_2, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(kern::math::max(x, x_2), kCell))); }, vector_break(s.a).x, vector_break(s.b).x, kCell));
          y0 = kern::to<Int>(kern::formula([](Float y, Float y_2, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(kern::math::min(y, y_2), kCell))); }, vector_break(s.a).y, vector_break(s.b).y, kCell));
          y1 = kern::to<Int>(kern::formula([](Float y, Float y_2, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(kern::math::max(y, y_2), kCell))); }, vector_break(s.a).y, vector_break(s.b).y, kCell));
          for (auto it20 = flow_for(n20, x0, x1); it20.next();) {
            for (auto it21 = flow_for(n21, y0, y1); it21.next();) {
              key = cell(n20.index, n21.index).Result;
              map_add(n22, buckets, key);
              t = buckets[n22.index];
              list_add(t, si);
              put = t;
              buckets[n22.index] = put;
            }
          }
          i += 1;
        }
      }
    }
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U32>>(buckets))))); it23.next();) {
      for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U32>(buckets[n23.index]))))); it24.next();) {
        for (auto it25 = flow_for(n25, kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n24.index))), kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U32>(buckets[n23.index]))))); it25.next();) {
          skip_3 = false;
          s_2 = segs[buckets[n23.index][n24.index]];
          t_2 = segs[buckets[n23.index][n25.index]];
          if (math_compare<"==">(kern::to<Float>(s_2.lane), kern::to<Float>(t_2.lane))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            r = vector_sub(s_2.b, s_2.a);
            q = vector_sub(t_2.b, t_2.a);
            den = kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(r).x, vector_break(q).y, vector_break(r).y, vector_break(q).x);
            if (math_compare<"<">(kern::formula([](Float den) -> Float { return kern::math::abs(den); }, den), 1e-06)) {
              skip_3 = true;
            }
            if (logic_not(skip_3)) {
              w = vector_sub(t_2.a, s_2.a);
              u = kern::formula([](Float x, Float y, Float y_2, Float x_2, Float den) -> Float { return kern::math::div(((x * y) - (y_2 * x_2)), den); }, vector_break(w).x, vector_break(q).y, vector_break(w).y, vector_break(q).x, den);
              v = kern::formula([](Float x, Float y, Float y_2, Float x_2, Float den) -> Float { return kern::math::div(((x * y) - (y_2 * x_2)), den); }, vector_break(w).x, vector_break(r).y, vector_break(w).y, vector_break(r).x, den);
              if (logic_or(logic_or(logic_or(math_compare<"<">(u, 0.0), math_compare<">">(u, 1.0)), math_compare<"<">(v, 0.0)), math_compare<">">(v, 1.0))) {
                skip_3 = true;
              }
              if (logic_not(skip_3)) {
                p_2 = vector_add(s_2.a, vector_scale(r, u));
                n26 = rebuild_node_at(cells, nodes, p_2);
                ni = n26.Result;
                item = buckets[n23.index][n24.index];
                item_2 = buckets[n23.index][n25.index];
                items = list_make<U32, "2">(item, item_2);
                for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(items))))); it27.next();) {
                  gi = items[n27.index];
                  g = segs[gi];
                  item_3 = g.a;
                  item_4 = g.b;
                  items_2 = list_make<Vector, "2">(item_3, item_4);
                  for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items_2))))); it28.next();) {
                    end = items_2[n28.index];
                    skip_4 = false;
                    d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(end).x, vector_break(p_2).x, vector_break(end).y, vector_break(p_2).y);
                    if (math_compare<"<">(d, 0.5)) {
                      skip_4 = true;
                    }
                    if (logic_not(skip_4)) {
                      dir = vector_make(kern::formula([](Float x, Float x_2, Float d) -> Float { return kern::math::div((x - x_2), d); }, vector_break(end).x, vector_break(p_2).x, d), kern::formula([](Float y, Float y_2, Float d) -> Float { return kern::math::div((y - y_2), d); }, vector_break(end).y, vector_break(p_2).y, d), 0.0);
                      dup = false;
                      at = ni;
                      for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficRules_Arm>(nodes[at].arms))))); it29.next();) {
                        dup = logic_or(dup, math_compare<">">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(nodes[at].arms[n29.index].dir).x, vector_break(dir).x, vector_break(nodes[at].arms[n29.index].dir).y, vector_break(dir).y), 0.97));
                      }
                      if (logic_not(dup)) {
                        list_add(nodes[ni].arms, struct_make<TrafficRules_Arm>(dir, g.width, Vector{0.0, 0.0, 0.0}, false));
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficRules_rebuild_Node>(nodes))))); it30.next();) {
      skip_5 = false;
      if (math_compare<"<">(kern::to<Float>(list_length<TrafficRules_Arm>(nodes[n30.index].arms)), 3.0)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        j = TrafficRules_Junction{Vector{0.0, 0.0, 0.0}, List<TrafficRules_Arm>{}, false, 0.0, 8.0, Vector{1.0, 0.0, 0.0}, -1, -99.0};
        j.p = nodes[n30.index].p;
        j.arms = nodes[n30.index].arms;
        wmax = 0.0;
        for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficRules_Arm>(nodes[n30.index].arms))))); it31.next();) {
          wmax = kern::formula([](Float wmax, Float a) -> Float { return kern::math::max(wmax, a); }, wmax, nodes[n30.index].arms[n31.index].width);
        }
        func_call_on<"Traffic_JunctionLights">(actor_find("Traffic"), kern::out(n32.lights), blueprint_call::decimal(wmax).Result);
        j.lights = n32.lights;
        func_call_on<"Traffic_JunctionRadius">(actor_find("Traffic"), kern::out(n33.radius), blueprint_call::decimal(wmax).Result);
        j.radius = n33.radius;
        best = 1000000000.0;
        for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficRules_Arm>(nodes[n30.index].arms))))); it34.next();) {
          skip_6 = false;
          if (math_compare<"<">(nodes[n30.index].arms[n34.index].width, wmax)) {
            skip_6 = true;
          }
          if (logic_not(skip_6)) {
            bearing = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(nodes[n30.index].arms[n34.index].dir).y, vector_break(nodes[n30.index].arms[n34.index].dir).x);
            if (math_compare<"<">(bearing, 0.0)) {
              bearing += 3.14159265;
            }
            if (math_compare<">=">(bearing, 3.1405926500000003)) {
              bearing = 0.0;
            }
            if (math_compare<"<">(bearing, best)) {
              best = bearing;
              j.axis = nodes[n30.index].arms[n34.index].dir;
            }
          }
        }
        ai = 0;
        while (loop(math_compare<">">(kern::formula([](Float ai, Float a) -> Float { return ((ai < 0.0) != (a < 0.0) ? (a < 0.0) : (ai < a)); }, kern::to<Float>(ai), kern::to<Float>(list_length<TrafficRules_Arm>(j.arms))), 0.5))) {
          skip_7 = false;
          aa = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(j.arms[ai].dir).y, vector_break(j.arms[ai].dir).x);
          best_2 = 6.2831853;
          next = list_length<TrafficRules_Arm>(j.arms);
          for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficRules_Arm>(j.arms))))); it35.next();) {
            skip_8 = false;
            if (math_compare<"==">(kern::to<Float>(n35.index), kern::to<Float>(ai))) {
              skip_8 = true;
            }
            if (logic_not(skip_8)) {
              b = j.arms[n35.index];
              d_2 = kern::formula([](Float y, Float x, Float aa) -> Float { return (kern::math::atan2(y, x) - aa); }, vector_break(b.dir).y, vector_break(b.dir).x, aa);
              while (loop(math_compare<"<=">(d_2, 0.0001))) {
                d_2 += 6.2831853;
              }
              if (math_compare<"<">(d_2, best_2)) {
                best_2 = d_2;
                next = n35.index;
              }
            }
          }
          half = kern::formula([](Float best_2) -> Float { return kern::math::div(best_2, 2.0); }, best_2);
          if (logic_or(math_compare<"==">(kern::to<Float>(next), kern::to<Float>(list_length<TrafficRules_Arm>(j.arms))), math_compare<">=">(best_2, 3.1415927))) {
            j.arms[ai].pole = vector_scale(vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(j.arms[ai].dir).y), vector_break(j.arms[ai].dir).x, 0.0), kern::formula([](Float a) -> Float { return ((a * 0.5) + 1.2); }, j.arms[ai].width));
            j.arms[ai].has_pole = true;
            skip_7 = true;
          }
          if (logic_not(skip_7)) {
            s_3 = kern::formula([](Float half) -> Float { return kern::math::sin(half); }, half);
            r_2 = kern::formula([](Float a, Float a_2, Float s_3) -> Float { return kern::math::div(((kern::math::max(a, a_2) * 0.5) + 1.2), kern::math::max(s_3, 0.05)); }, j.arms[ai].width, j.arms[next].width, s_3);
            if (math_compare<">">(r_2, 30.0)) {
              skip_7 = true;
            }
            if (logic_not(skip_7)) {
              j.arms[ai].pole = vector_scale(vector_make(kern::formula([](Float aa, Float half) -> Float { return kern::math::cos((aa + half)); }, aa, half), kern::formula([](Float aa, Float half) -> Float { return kern::math::sin((aa + half)); }, aa, half), 0.0), r_2);
              j.arms[ai].has_pole = true;
            }
          }
          ai += 1;
        }
        h = kern::to<Int>(kern::formula([](Float x, Float y) -> Float { return kern::math::bxor(kern::math::mul32(kern::math::wrap32(kern::math::wrap32s(kern::math::round(kern::math::div(x, 3.0)))), 73856093.0), kern::math::mul32(kern::math::wrap32(kern::math::wrap32s(kern::math::round(kern::math::div(y, 3.0)))), 19349663.0)); }, vector_break(nodes[n30.index].p).x, vector_break(nodes[n30.index].p).y));
        j.offset = kern::formula([](Float h) -> Float { return kern::math::div(kern::math::mod(h, 3800.0), 100.0); }, kern::to<Float>(h));
        list_add(self.junctions_, j);
        id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<TrafficRules_Junction>(self.junctions_))));
        key_2 = cell(kern::to<Int>(kern::formula([](Float x, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kCell))); }, vector_break(nodes[n30.index].p).x, kCell)), kern::to<Int>(kern::formula([](Float y, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kCell))); }, vector_break(nodes[n30.index].p).y, kCell))).Result;
        map_add(n36, self.grid_, key_2);
        t_3 = self.grid_[n36.index];
        list_add(t_3, id);
        put_2 = t_3;
        self.grid_[n36.index] = put_2;
      }
    }
  }

  void TrafficRules_near(const TrafficRules& self, List<I32>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    I32 cx0 = 0;
    I32 cx1 = 0;
    I32 cy0 = 0;
    I32 cy1 = 0;
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    I32 j = 0;
    list_clear(out);
    cx0 = kern::to<Int>(kern::formula([](Float x, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((x - r), kCell))); }, vector_break(p).x, r, kCell));
    cx1 = kern::to<Int>(kern::formula([](Float x, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((x + r), kCell))); }, vector_break(p).x, r, kCell));
    cy0 = kern::to<Int>(kern::formula([](Float y, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((y - r), kCell))); }, vector_break(p).y, r, kCell));
    cy1 = kern::to<Int>(kern::formula([](Float y, Float r, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((y + r), kCell))); }, vector_break(p).y, r, kCell));
    for (auto it37 = flow_for(n37, cy0, cy1); it37.next();) {
      for (auto it38 = flow_for(n38, cx0, cx1); it38.next();) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<I32>>(self.grid_, cell(n38.index, n37.index).Result).index), kern::to<Float>(map_length<Int64, List<I32>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<I32>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(self.grid_[at]))))); it39.next();) {
            j = self.grid_[at][n39.index];
            if (math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.junctions_[j].p).x, vector_break(p).x, vector_break(self.junctions_[j].p).y, vector_break(p).y), r)) {
              list_add(out, j);
            }
          }
        }
      }
    }
  }

  struct TrafficRules_bindResult { Bool Result = false; };
  KERN_PURE TrafficRules_bindResult TrafficRules_bind() {
    return {func_has_blueprint(actor_find("Traffic"))};
  }

  void TrafficRules_take_laws(TrafficRules& self) {
    self.laws_.ahead_min = func_get_on<"AheadMinM", Float>(actor_find("Traffic"));
    self.laws_.ahead_max = func_get_on<"AheadMaxM", Float>(actor_find("Traffic"));
    self.laws_.stop_line = func_get_on<"StopLineM", Float>(actor_find("Traffic"));
    self.laws_.first_come = func_get_on<"FirstComeM", Float>(actor_find("Traffic"));
    self.laws_.claim_s = func_get_on<"ClaimS", Float>(actor_find("Traffic"));
    self.laws_.patience_s = func_get_on<"PatienceS", Float>(actor_find("Traffic"));
  }

  struct TrafficRules_lightResult { I32 Result = 0; };
  TrafficRules_lightResult TrafficRules_light(const TrafficRules& self, const TrafficRules_Junction& j, Vector heading = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    I32 light = 0;
    if (logic_not(j.lights)) {
      return {-1};
    }
    light = 0;
    func_call_on<"Traffic_Light">(actor_find("Traffic"), kern::out(n40.light), blueprint_call::decimal(kern::formula([](Float lights_time, Float now) -> Float { return ((lights_time >= 0.0) ? lights_time : now); }, self.lights_time, now)).Result, blueprint_call::decimal(j.offset).Result, blueprint_call::decimal(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(heading).x, vector_break(j.axis).x, vector_break(heading).y, vector_break(j.axis).y)).Result);
    light = n40.light;
    return {light};
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
  TrafficRules_pursuitResult TrafficRules_pursuit(const Lane& lane, I32 index = 0, I32 dir = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Float look = 0.0, Float off = 0.0) {
    I32 dir_ = 0;
    TrafficRules_Pursuit out = TrafficRules_Pursuit{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, false, 0.0};
    I32 n = 0;
    I32 i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Float at = 0.0;
    Float s = 0.0;
    Vector cur = Vector{0.0, 0.0, 0.0};
    Float left = 0.0;
    Float want = 0.0;
    I32 k = 0;
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
    n41 = pursuit_along(t, a, b);
    len = n41.Result;
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
        n42 = pursuit_along(t, a, b);
        left = n42.Result;
      }
    }
    out.tangent = t;
    out.target = vector_add(vector_add(cur, vector_scale(t, want)), vector_scale(vector_make(vector_break(t).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(t).x), 0.0), off));
    return {out};
  }

  struct TrafficRules_gap_speedResult { Float Result = 0.0; };
  TrafficRules_gap_speedResult TrafficRules_gap_speed(const TrafficRules& self, const List<TrafficRules_Vehicle>& all, I32& limiter, I32 self_2 = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Bool bus = false) {
    Float side = 0.0;
    Float far = 0.0;
    Fixed<Float, 2> bodies = Fixed<Float, 2>{};
    Float gap = 0.0;
    I32 who = 0;
    Bool skip = false;
    TrafficRules_Vehicle o = TrafficRules_Vehicle{Vector{0.0, 0.0, 0.0}, false, false};
    Float dx = 0.0;
    Float dy = 0.0;
    Float along = 0.0;
    Float g = 0.0;
    func_call_on<"Traffic_SideReach">(actor_find("Traffic"), kern::out(n43.reach), bus);
    side = n43.reach;
    far = self.laws_.ahead_max;
    func_call_on<"Traffic_Bodies">(actor_find("Traffic"), kern::out(n44.length), bus, false);
    func_call_on<"Traffic_Bodies">(actor_find("Traffic"), kern::out(n45.length), bus, true);
    bodies = list_make<Float, "2">(n44.length, n45.length);
    gap = 1000000000.0;
    who = -1;
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficRules_Vehicle>(all))))); it46.next();) {
      skip = false;
      o = all[n46.index];
      if (logic_or(logic_not(o.there), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n46.index))))), kern::to<Float>(self_2)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(o.p).x, vector_break(pos).x);
        dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(o.p).y, vector_break(pos).y);
        if (logic_or(math_compare<">">(kern::formula([](Float dx) -> Float { return kern::math::abs(dx); }, dx), far), math_compare<">">(kern::formula([](Float dy) -> Float { return kern::math::abs(dy); }, dy), far))) {
          skip = true;
        }
        if (logic_not(skip)) {
          along = kern::formula([](Float dx, Float x, Float dy, Float y) -> Float { return ((dx * x) + (dy * y)); }, dx, vector_break(fwd).x, dy, vector_break(fwd).y);
          if (logic_or(math_compare<"<">(along, self.laws_.ahead_min), math_compare<">">(along, far))) {
            skip = true;
          }
          if (logic_not(skip)) {
            if (math_compare<">">(kern::formula([](Float dx, Float y, Float dy, Float x) -> Float { return kern::math::abs((((-dx) * y) + (dy * x))); }, dx, vector_break(fwd).y, dy, vector_break(fwd).x), side)) {
              skip = true;
            }
            if (logic_not(skip)) {
              g = kern::formula([](Float along, Float a) -> Float { return (along - a); }, along, bodies[kern::to<Int>(kern::formula([](Float bus) -> Float { return ((bus != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(o.bus)))]);
              if (math_compare<"<">(g, gap)) {
                gap = g;
                who = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n46.index)));
              }
            }
          }
        }
      }
    }
    limiter = who;
    if (math_compare<"<">(kern::to<Float>(who), 0.0)) {
      return {1000000000.0};
    }
    func_call_on<"Traffic_GapSpeed">(actor_find("Traffic"), kern::out(n47.speed), blueprint_call::decimal(gap).Result);
    return {n47.speed};
  }

  struct limit_in_laneResult { Bool Result = false; };
  KERN_PURE limit_in_laneResult limit_in_lane(Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Float reach = 0.0, Float half = 0.0, Vector q = Vector{0.0, 0.0, 0.0}) {
    Float dx = 0.0;
    Float dy = 0.0;
    Float along = 0.0;
    dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(q).x, vector_break(pos).x);
    dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(q).y, vector_break(pos).y);
    along = kern::formula([](Float dx, Float x, Float dy, Float y) -> Float { return ((dx * x) + (dy * y)); }, dx, vector_break(fwd).x, dy, vector_break(fwd).y);
    return {logic_and(logic_and(math_compare<">=">(along, -0.5), math_compare<"<=">(along, reach)), math_compare<"<">(kern::formula([](Float dx, Float y, Float dy, Float x) -> Float { return kern::math::abs((((-dx) * y) + (dy * x))); }, dx, vector_break(fwd).y, dy, vector_break(fwd).x), half))};
  }

  struct TrafficRules_limitResult { Float Result = 0.0; };
  TrafficRules_limitResult TrafficRules_limit(TrafficRules& self, const List<Vector>& cars, const List<Vector>& people, TrafficRules_View& view, I32 car = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Float speed = 0.0, Bool bus = false, Float now = 0.0, Vector player = Vector{0.0, 0.0, 0.0}, Bool player_on_foot = false) {
    Float lim = 0.0;
    Float v = 0.0;
    Float reach = 0.0;
    Float half = 0.0;
    Bool by_player = false;
    Bool yielding = false;
    Int i = 0;
    Int key = 0;
    Int b_row = 0;
    Float roll = 0.0;
    Float next = 0.0;
    List<I32> cand = List<I32>{};
    Float best = 0.0;
    Bool skip = false;
    I32 ji = 0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float along = 0.0;
    Float lateral = 0.0;
    Float dist = 0.0;
    TrafficRules_Blocked t = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_2 = 0;
    TrafficRules_Blocked put = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int j_at = 0;
    Float stop = 0.0;
    Bool must_stop = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Int key_3 = 0;
    Int b_row_2 = 0;
    Int k = 0;
    Bool stop_2 = false;
    Bool skip_2 = false;
    Vector o = Vector{0.0, 0.0, 0.0};
    Float od = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int key_4 = 0;
    Int b_row_3 = 0;
    view = struct_make<TrafficRules_View>(-1, -1, 1000000000.0, false, false, false);
    lim = 1000000000.0;
    v = kern::formula([](Float speed) -> Float { return kern::math::max(0.0, speed); }, speed);
    func_call_on<"Traffic_YieldReach">(actor_find("Traffic"), kern::out(n48.reach), bus, blueprint_call::decimal(v).Result);
    reach = n48.reach;
    func_call_on<"Traffic_YieldHalfWidth">(actor_find("Traffic"), kern::out(n49.half), bus);
    half = n49.half;
    by_player = logic_and(player_on_foot, limit_in_lane(pos, fwd, reach, half, player).Result);
    yielding = by_player;
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(people))), 0.5), logic_not(yielding)))) {
      yielding = limit_in_lane(pos, fwd, reach, half, people[i]).Result;
      i += 1;
    }
    key = car;
    map_add(n50, self.blocked_, key);
    b_row = n50.index;
    if (yielding) {
      lim = 0.0;
      view.yielding = true;
      if (math_compare<"<">(self.blocked_[b_row].since, 0.0)) {
        self.blocked_[b_row].since = now;
      }
      roll = kern::formula([](Float car, Float now) -> Float { return kern::math::div(kern::math::mod(kern::math::wrap32(kern::math::mul32(kern::math::wrap32(car), 2654435761.0) + kern::math::trunc(now)), 700.0), 700.0); }, kern::to<Float>(car), now);
      next = 0.0;
      func_call_on<"Traffic_Honk">(actor_find("Traffic"), kern::out(n51.honk, n51.next), by_player, blueprint_call::decimal(self.blocked_[b_row].since).Result, blueprint_call::decimal(self.blocked_[b_row].next_honk).Result, blueprint_call::decimal(now).Result, blueprint_call::decimal(roll).Result);
      view.honk = n51.honk;
      next = n51.next;
      self.blocked_[b_row].next_honk = next;
    } else {
      self.blocked_[b_row].since = -1.0;
    }
    cand = List<I32>{};
    TrafficRules_near(self, cand, vector_add(pos, vector_scale(fwd, 25.0)), 34.0);
    best = 1000000000.0;
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I32>(cand))))); it52.next();) {
      ji = cand[n52.index];
      skip = false;
      d = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(self.junctions_[ji].p).x, vector_break(pos).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(self.junctions_[ji].p).y, vector_break(pos).y), 0.0);
      along = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(d).x, vector_break(fwd).x, vector_break(d).y, vector_break(fwd).y);
      lateral = kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return kern::math::abs((((-x) * y) + (y_2 * x_2))); }, vector_break(d).x, vector_break(fwd).y, vector_break(d).y, vector_break(fwd).x);
      dist = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
      if (math_compare<"<">(dist, self.junctions_[ji].radius)) {
        if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(self.junctions_[ji].claim), 0.0), math_compare<"==">(kern::to<Float>(self.junctions_[ji].claim), kern::to<Float>(car))), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.junctions_[ji].claim_at), self.laws_.claim_s))) {
          self.junctions_[ji].claim = car;
          self.junctions_[ji].claim_at = now;
        }
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(math_compare<"==">(kern::to<Float>(self.junctions_[ji].claim), kern::to<Float>(car)), math_compare<">">(dist, kern::formula([](Float a) -> Float { return (a + 3.0); }, self.junctions_[ji].radius)))) {
          self.junctions_[ji].claim = -1;
        }
        if (logic_or(logic_or(math_compare<"<">(along, 0.0), math_compare<">">(lateral, kern::formula([](Float a) -> Float { return (a + 2.0); }, self.junctions_[ji].radius))), math_compare<">=">(along, best))) {
          skip = true;
        }
        if (logic_not(skip)) {
          best = along;
          view.junction = ji;
          view.distance = kern::formula([](Float along, Float a) -> Float { return (along - a); }, along, self.junctions_[ji].radius);
        }
      }
    }
    if (math_compare<"<">(kern::to<Float>(view.junction), 0.0)) {
      key_2 = car;
      map_add(n53, self.blocked_, key_2);
      t = self.blocked_[n53.index];
      t.wait_since = -1.0;
      put = t;
      self.blocked_[n53.index] = put;
    }
    if (math_compare<">=">(kern::to<Float>(view.junction), 0.0)) {
      j_at = view.junction;
      stop = kern::formula([](Float distance, Float stop_line) -> Float { return kern::math::max(0.0, (distance - stop_line)); }, view.distance, self.laws_.stop_line);
      must_stop = false;
      n54 = TrafficRules_light(self, self.junctions_[j_at], fwd, now);
      view.light = n54.Result;
      if (math_compare<">=">(kern::to<Float>(view.light), 0.0)) {
        func_call_on<"Traffic_MustStop">(actor_find("Traffic"), kern::out(n55.stop), view.light, blueprint_call::decimal(stop).Result, blueprint_call::decimal(v).Result);
        must_stop = n55.stop;
      } else if (math_compare<"<">(view.distance, self.laws_.first_come)) {
        if (logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(self.junctions_[j_at].claim), 0.0), math_compare<"!=">(kern::to<Float>(self.junctions_[j_at].claim), kern::to<Float>(car))), math_compare<"<">(kern::to<Float>(self.junctions_[j_at].claim), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(cars)))))))), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.junctions_[j_at].claim_at), self.laws_.claim_s))) {
          c = cars[self.junctions_[j_at].claim];
          if (logic_and(math_compare<">">(vector_break(c).z, -100000000.0), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(self.junctions_[j_at].p).x, vector_break(c).y, vector_break(self.junctions_[j_at].p).y), kern::formula([](Float a) -> Float { return (a + 3.0); }, self.junctions_[j_at].radius)))) {
            must_stop = true;
          }
        }
        key_3 = car;
        map_add(n56, self.blocked_, key_3);
        b_row_2 = n56.index;
        if (logic_and(logic_not(must_stop), logic_or(math_compare<"<">(self.blocked_[b_row_2].wait_since, 0.0), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.blocked_[b_row_2].wait_since), self.laws_.patience_s)))) {
          k = 0;
          stop_2 = false;
          while (loop(logic_and(logic_not(stop_2), math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(cars))), 0.5)))) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::wrap32s(k); }, kern::to<Float>(k))))), kern::to<Float>(car)), math_compare<"<">(vector_break(cars[k]).z, -100000000.0))) {
              skip_2 = true;
            }
            if (logic_not(logic_or(stop_2, skip_2))) {
              o = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(cars[k]).x, vector_break(self.junctions_[j_at].p).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(cars[k]).y, vector_break(self.junctions_[j_at].p).y), 0.0);
              od = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(o).x, vector_break(o).y);
              if (logic_or(logic_or(math_compare<"<">(od, self.junctions_[j_at].radius), math_compare<">">(od, kern::formula([](Float a) -> Float { return (a + 10.0); }, self.junctions_[j_at].radius))), math_compare<">">(od, kern::formula([](Float distance, Float a) -> Float { return ((distance + a) + 2.0); }, view.distance, self.junctions_[j_at].radius)))) {
                skip_2 = true;
              }
              if (logic_not(logic_or(stop_2, skip_2))) {
                me = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(pos).x, vector_break(self.junctions_[j_at].p).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(pos).y, vector_break(self.junctions_[j_at].p).y), 0.0);
                if (logic_and(math_compare<">">(kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(me).x, vector_break(o).y, vector_break(me).y, vector_break(o).x), 0.0), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2, Float od) -> Float { return kern::math::abs(kern::math::div(((x * x_2) + (y * y_2)), (kern::math::sqrt((x * x) + (y * y)) * od))); }, vector_break(me).x, vector_break(o).x, vector_break(me).y, vector_break(o).y, od), 0.5))) {
                  must_stop = true;
                  stop_2 = true;
                }
              }
            }
            if (logic_not(stop_2)) {
              k += 1;
            }
          }
        }
      }
      key_4 = car;
      map_add(n57, self.blocked_, key_4);
      b_row_3 = n57.index;
      if (logic_and(logic_and(must_stop, math_compare<"<">(v, 0.5)), math_compare<"<">(self.blocked_[b_row_3].wait_since, 0.0))) {
        self.blocked_[b_row_3].wait_since = now;
      }
      if (logic_or(math_compare<">">(view.distance, self.laws_.first_come), math_compare<">=">(kern::to<Float>(view.light), 0.0))) {
        self.blocked_[b_row_3].wait_since = -1.0;
      }
      if (must_stop) {
        view.wait = true;
        func_call_on<"Traffic_StopSpeed">(actor_find("Traffic"), kern::out(n58.speed), blueprint_call::decimal(stop).Result);
        lim = kern::formula([](Float lim, Float speed) -> Float { return kern::math::min(lim, speed); }, lim, n58.speed);
      }
    }
    return {lim};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool added = false; Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  rebuild_node_atResult n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  rebuild_node_atResult n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct { Bool added = false; Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  rebuild_node_atResult n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct { Bool lights = false; } n32;
  struct { Float radius = 0.0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct { Bool added = false; Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct { Int light = 0; } n40;
  pursuit_alongResult n41;
  pursuit_alongResult n42;
  struct { Float reach = 0.0; } n43;
  struct { Float length = 0.0; } n44;
  struct { Float length = 0.0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct { Float speed = 0.0; } n47;
  struct { Float reach = 0.0; } n48;
  struct { Float half = 0.0; } n49;
  struct { Bool added = false; Int index = 0; } n50;
  struct { Bool honk = false; Float next = 0.0; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  struct { Bool added = false; Int index = 0; } n53;
  TrafficRules_lightResult n54;
  struct { Bool stop = false; } n55;
  struct { Bool added = false; Int index = 0; } n56;
  struct { Bool added = false; Int index = 0; } n57;
  struct { Float speed = 0.0; } n58;
};
