struct world_traffic_cpp : virtual kern::Library, virtual blueprint_call, virtual city_stream, virtual world_traffic {
  // ---- variables ----
  /// junction lookup grid
  KERN_CONST Float kCell = 40.0;

  // ---- functions ----
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
  KERN_PURE cellResult cell(Int x = 0, Int y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
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
    for (auto it10 = flow_for(n10, cy0, cy1); it10.next();) {
      for (auto it11 = flow_for(n11, cx0, cx1); it11.next();) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.grid_, cell(n11.index, n10.index).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.grid_[at]))))); it12.next();) {
            j = self.grid_[at][n12.index];
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

  struct TrafficRules_lightResult { Int Result = 0; };
  TrafficRules_lightResult TrafficRules_light(const TrafficRules& self, const TrafficRules_Junction& j, Vector heading = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Int light = 0;
    if (logic_not(j.lights)) {
      return {-1};
    }
    light = 0;
    func_call_on<"Traffic_Light">(actor_find("Traffic"), kern::out(n13.light), blueprint_call::decimal(kern::formula([](Float lights_time, Float now) -> Float { return ((lights_time >= 0.0) ? lights_time : now); }, self.lights_time, now)).Result, blueprint_call::decimal(j.offset).Result, blueprint_call::decimal(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(heading).x, vector_break(j.axis).x, vector_break(heading).y, vector_break(j.axis).y)).Result);
    light = n13.light;
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
    n14 = pursuit_along(t, a, b);
    len = n14.Result;
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
        n15 = pursuit_along(t, a, b);
        left = n15.Result;
      }
    }
    out.tangent = t;
    out.target = vector_add(vector_add(cur, vector_scale(t, want)), vector_scale(vector_make(vector_break(t).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(t).x), 0.0), off));
    return {out};
  }

  struct TrafficRules_gap_speedResult { Float Result = 0.0; };
  TrafficRules_gap_speedResult TrafficRules_gap_speed(const TrafficRules& self, const List<TrafficRules_Vehicle>& all, Int& limiter, Int self_2 = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Bool bus = false) {
    Float side = 0.0;
    Float far = 0.0;
    Fixed<Float, 2> bodies = Fixed<Float, 2>{};
    Float gap = 0.0;
    Int who = 0;
    Bool skip = false;
    TrafficRules_Vehicle o = TrafficRules_Vehicle{Vector{0.0, 0.0, 0.0}, false, false};
    Float dx = 0.0;
    Float dy = 0.0;
    Float along = 0.0;
    Float g = 0.0;
    func_call_on<"Traffic_SideReach">(actor_find("Traffic"), kern::out(n16.reach), bus);
    side = n16.reach;
    far = self.laws_.ahead_max;
    func_call_on<"Traffic_Bodies">(actor_find("Traffic"), kern::out(n17.length), bus, false);
    func_call_on<"Traffic_Bodies">(actor_find("Traffic"), kern::out(n18.length), bus, true);
    bodies = list_make<Float, "2">(n17.length, n18.length);
    gap = 1000000000.0;
    who = -1;
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficRules_Vehicle>(all))))); it19.next();) {
      skip = false;
      o = all[n19.index];
      if (logic_or(logic_not(o.there), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index))))), kern::to<Float>(self_2)))) {
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
                who = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index)));
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
    func_call_on<"Traffic_GapSpeed">(actor_find("Traffic"), kern::out(n20.speed), blueprint_call::decimal(gap).Result);
    return {n20.speed};
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
  TrafficRules_limitResult TrafficRules_limit(TrafficRules& self, const List<Vector>& cars, const List<Vector>& people, TrafficRules_View& view, Int car = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Float speed = 0.0, Bool bus = false, Float now = 0.0, Vector player = Vector{0.0, 0.0, 0.0}, Bool player_on_foot = false) {
    Float lim = 0.0;
    Float v = 0.0;
    Float reach = 0.0;
    Float half = 0.0;
    Bool by_player = false;
    Bool yielding = false;
    Int i = 0;
    Int key = 0;
    Int key_2 = 0;
    TrafficRules_Blocked t = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_3 = 0;
    TrafficRules_Blocked put = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Float roll = 0.0;
    Float next = 0.0;
    Int key_4 = 0;
    Int key_5 = 0;
    TrafficRules_Blocked t_2 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_6 = 0;
    TrafficRules_Blocked put_2 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    TrafficRules_Blocked t_3 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_7 = 0;
    TrafficRules_Blocked put_3 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    List<Int> cand = List<Int>{};
    Float best = 0.0;
    Bool skip = false;
    Int ji = 0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float along = 0.0;
    Float lateral = 0.0;
    Float dist = 0.0;
    TrafficRules_Blocked t_4 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_8 = 0;
    TrafficRules_Blocked put_4 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int j_at = 0;
    Float stop = 0.0;
    Bool must_stop = false;
    TrafficRules_Junction a = TrafficRules_Junction{Vector{0.0, 0.0, 0.0}, List<TrafficRules_Arm>{}, false, 0.0, 8.0, Vector{1.0, 0.0, 0.0}, -1, -99.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Int key_9 = 0;
    Int key_10 = 0;
    Int key_11 = 0;
    Int key_12 = 0;
    Int key_13 = 0;
    Bool either = false;
    Bool both = false;
    Int k = 0;
    Bool stop_2 = false;
    Bool skip_2 = false;
    Vector o = Vector{0.0, 0.0, 0.0};
    Float od = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int key_14 = 0;
    Int key_15 = 0;
    Int key_16 = 0;
    Bool both_2 = false;
    TrafficRules_Blocked t_5 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_17 = 0;
    TrafficRules_Blocked put_5 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    TrafficRules_Blocked t_6 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    Int key_18 = 0;
    TrafficRules_Blocked put_6 = TrafficRules_Blocked{-1.0, 0.0, -1.0};
    view = struct_make<TrafficRules_View>(-1, -1, 1000000000.0, false, false, false);
    lim = 1000000000.0;
    v = kern::formula([](Float speed) -> Float { return kern::math::max(0.0, speed); }, speed);
    func_call_on<"Traffic_YieldReach">(actor_find("Traffic"), kern::out(n21.reach), bus, blueprint_call::decimal(v).Result);
    reach = n21.reach;
    func_call_on<"Traffic_YieldHalfWidth">(actor_find("Traffic"), kern::out(n22.half), bus);
    half = n22.half;
    by_player = logic_and(player_on_foot, limit_in_lane(pos, fwd, reach, half, player).Result);
    yielding = by_player;
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(people))), 0.5), logic_not(yielding)))) {
      yielding = limit_in_lane(pos, fwd, reach, half, people[i]).Result;
      i += 1;
    }
    key = car;
    map_add(n23, self.blocked_, key);
    if (yielding) {
      lim = 0.0;
      view.yielding = true;
      key_2 = car;
      map_add(n24, self.blocked_, key_2);
      if (math_compare<"<">(struct_break<TrafficRules_Blocked>(self.blocked_[n24.index]).since, 0.0)) {
        key_3 = car;
        map_add(n25, self.blocked_, key_3);
        put = t;
        t = self.blocked_[n25.index];
        t.since = now;
        self.blocked_[n25.index] = put;
      }
      roll = kern::formula([](Float car, Float now) -> Float { return kern::math::div(kern::math::mod(kern::math::wrap32(kern::math::mul32(kern::math::wrap32(car), 2654435761.0) + kern::math::trunc(now)), 700.0), 700.0); }, kern::to<Float>(car), now);
      next = 0.0;
      key_4 = car;
      map_add(n26, self.blocked_, key_4);
      key_5 = car;
      map_add(n27, self.blocked_, key_5);
      func_call_on<"Traffic_Honk">(actor_find("Traffic"), kern::out(n28.honk, n28.next), by_player, blueprint_call::decimal(struct_break<TrafficRules_Blocked>(self.blocked_[n26.index]).since).Result, blueprint_call::decimal(struct_break<TrafficRules_Blocked>(self.blocked_[n27.index]).next_honk).Result, blueprint_call::decimal(now).Result, blueprint_call::decimal(roll).Result);
      view.honk = n28.honk;
      next = n28.next;
      key_6 = car;
      map_add(n29, self.blocked_, key_6);
      put_2 = t_2;
      t_2 = self.blocked_[n29.index];
      t_2.next_honk = next;
      self.blocked_[n29.index] = put_2;
    } else {
      key_7 = car;
      map_add(n30, self.blocked_, key_7);
      put_3 = t_3;
      t_3 = self.blocked_[n30.index];
      t_3.since = -1.0;
      self.blocked_[n30.index] = put_3;
    }
    cand = List<Int>{};
    TrafficRules_near(self, cand, vector_add(pos, vector_scale(fwd, 25.0)), 34.0);
    best = 1000000000.0;
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(cand))))); it31.next();) {
      ji = cand[n31.index];
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
      key_8 = car;
      map_add(n32, self.blocked_, key_8);
      put_4 = t_4;
      t_4 = self.blocked_[n32.index];
      t_4.wait_since = -1.0;
      self.blocked_[n32.index] = put_4;
    }
    if (math_compare<">=">(kern::to<Float>(view.junction), 0.0)) {
      j_at = view.junction;
      stop = kern::formula([](Float distance, Float stop_line) -> Float { return kern::math::max(0.0, (distance - stop_line)); }, view.distance, self.laws_.stop_line);
      must_stop = false;
      a = self.junctions_[j_at];
      n33 = TrafficRules_light(self, a, fwd, now);
      view.light = n33.Result;
      if (math_compare<">=">(kern::to<Float>(view.light), 0.0)) {
        func_call_on<"Traffic_MustStop">(actor_find("Traffic"), kern::out(n34.stop), view.light, blueprint_call::decimal(stop).Result, blueprint_call::decimal(v).Result);
        must_stop = n34.stop;
      } else if (math_compare<"<">(view.distance, self.laws_.first_come)) {
        if (logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(self.junctions_[j_at].claim), 0.0), math_compare<"!=">(kern::to<Float>(self.junctions_[j_at].claim), kern::to<Float>(car))), math_compare<"<">(kern::to<Float>(self.junctions_[j_at].claim), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(cars)))))))), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.junctions_[j_at].claim_at), self.laws_.claim_s))) {
          c = cars[self.junctions_[j_at].claim];
          if (logic_and(math_compare<">">(vector_break(c).z, -100000000.0), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(self.junctions_[j_at].p).x, vector_break(c).y, vector_break(self.junctions_[j_at].p).y), kern::formula([](Float a) -> Float { return (a + 3.0); }, self.junctions_[j_at].radius)))) {
            must_stop = true;
          }
        }
        key_9 = car;
        map_add(n35, self.blocked_, key_9);
        both = logic_not(must_stop);
        if (both) {
          key_11 = car;
          map_add(n36, self.blocked_, key_11);
          either = math_compare<"<">(struct_break<TrafficRules_Blocked>(self.blocked_[n36.index]).wait_since, 0.0);
          if (logic_not(either)) {
            key_13 = car;
            map_add(n37, self.blocked_, key_13);
            either = math_compare<"<">(kern::formula([](Float now, Float wait_since) -> Float { return (now - wait_since); }, now, struct_break<TrafficRules_Blocked>(self.blocked_[n37.index]).wait_since), self.laws_.patience_s);
          }
          both = either;
        }
        if (both) {
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
      key_14 = car;
      map_add(n38, self.blocked_, key_14);
      both_2 = logic_and(must_stop, math_compare<"<">(v, 0.5));
      if (both_2) {
        key_16 = car;
        map_add(n39, self.blocked_, key_16);
        both_2 = math_compare<"<">(struct_break<TrafficRules_Blocked>(self.blocked_[n39.index]).wait_since, 0.0);
      }
      if (both_2) {
        key_17 = car;
        map_add(n40, self.blocked_, key_17);
        put_5 = t_5;
        t_5 = self.blocked_[n40.index];
        t_5.wait_since = now;
        self.blocked_[n40.index] = put_5;
      }
      if (logic_or(math_compare<">">(view.distance, self.laws_.first_come), math_compare<">=">(kern::to<Float>(view.light), 0.0))) {
        key_18 = car;
        map_add(n41, self.blocked_, key_18);
        put_6 = t_6;
        t_6 = self.blocked_[n41.index];
        t_6.wait_since = -1.0;
        self.blocked_[n41.index] = put_6;
      }
      if (must_stop) {
        view.wait = true;
        func_call_on<"Traffic_StopSpeed">(actor_find("Traffic"), kern::out(n42.speed), blueprint_call::decimal(stop).Result);
        lim = kern::formula([](Float lim, Float speed) -> Float { return kern::math::min(lim, speed); }, lim, n42.speed);
      }
    }
    return {lim};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Int light = 0; } n13;
  pursuit_alongResult n14;
  pursuit_alongResult n15;
  struct { Float reach = 0.0; } n16;
  struct { Float length = 0.0; } n17;
  struct { Float length = 0.0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct { Float speed = 0.0; } n20;
  struct { Float reach = 0.0; } n21;
  struct { Float half = 0.0; } n22;
  struct { Bool added = false; Int index = 0; } n23;
  struct { Bool added = false; Int index = 0; } n24;
  struct { Bool added = false; Int index = 0; } n25;
  struct { Bool added = false; Int index = 0; } n26;
  struct { Bool added = false; Int index = 0; } n27;
  struct { Bool honk = false; Float next = 0.0; } n28;
  struct { Bool added = false; Int index = 0; } n29;
  struct { Bool added = false; Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct { Bool added = false; Int index = 0; } n32;
  TrafficRules_lightResult n33;
  struct { Bool stop = false; } n34;
  struct { Bool added = false; Int index = 0; } n35;
  struct { Bool added = false; Int index = 0; } n36;
  struct { Bool added = false; Int index = 0; } n37;
  struct { Bool added = false; Int index = 0; } n38;
  struct { Bool added = false; Int index = 0; } n39;
  struct { Bool added = false; Int index = 0; } n40;
  struct { Bool added = false; Int index = 0; } n41;
  struct { Float speed = 0.0; } n42;
};
