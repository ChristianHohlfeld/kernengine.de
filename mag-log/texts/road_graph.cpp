struct road_graph : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_STRUCT(RoadGraph_Node) { Vector p = Vector{0.0, 0.0, 0.0}; List<Int_Float_pair> edges = List<Int_Float_pair>{}; };
  KERN_STRUCT(RoadGraph) { Int64 built_from = Int64{0xffffffffffffffff}; List<RoadGraph_Node> nodes_ = List<RoadGraph_Node>{}; Map<Int64, Int> cells_ = Map<Int64, Int>{}; };

  // ---- functions ----
  struct RoadGraph_emptyResult { Bool Result = false; };
  KERN_PURE RoadGraph_emptyResult RoadGraph_empty(const RoadGraph& self) {
    return {math_compare<"==">(kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_)), 0.0)};
  }

  struct RoadGraph_sizeResult { Int Result = 0; };
  KERN_PURE RoadGraph_sizeResult RoadGraph_size(const RoadGraph& self) {
    return {list_length<RoadGraph_Node>(self.nodes_)};
  }
};
