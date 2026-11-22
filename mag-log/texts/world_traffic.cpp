struct world_traffic : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(TrafficRules_Arm) { Vector dir = Vector{0.0, 0.0, 0.0}; Float width = 6.0; Vector pole = Vector{0.0, 0.0, 0.0}; Bool has_pole = false; };
  KERN_STRUCT(TrafficRules_Blocked) { Float since = -1.0; Float next_honk = 0.0; Float wait_since = -1.0; };
  KERN_STRUCT(TrafficRules_Laws) { Float ahead_min = 0.0; Float ahead_max = 0.0; Float stop_line = 0.0; Float first_come = 0.0; Float claim_s = 0.0; Float patience_s = 0.0; };
  KERN_STRUCT(TrafficRules_Vehicle) { Vector p = Vector{0.0, 0.0, 0.0}; Bool bus = false; Bool there = false; };
  KERN_STRUCT(TrafficRules_View) { I32 junction = -1; I32 light = -1; Float distance = 1000000000.0; Bool wait = false; Bool yielding = false; Bool honk = false; };
  KERN_STRUCT(TrafficRules_Junction) { Vector p = Vector{0.0, 0.0, 0.0}; List<TrafficRules_Arm> arms = List<TrafficRules_Arm>{}; Bool lights = false; Float offset = 0.0; Float radius = 8.0; Vector axis = Vector{1.0, 0.0, 0.0}; I32 claim = -1; Float claim_at = -99.0; };
  KERN_STRUCT(TrafficRules) { Int64 built_from = Int64{0xffffffffffffffff}; Float lights_time = -1.0; TrafficRules_Laws laws_ = TrafficRules_Laws{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; List<TrafficRules_Junction> junctions_ = List<TrafficRules_Junction>{}; Map<Int64, List<I32>> grid_ = Map<Int64, List<I32>>{}; Map<Int, TrafficRules_Blocked> blocked_ = Map<Int, TrafficRules_Blocked>{}; };
};
