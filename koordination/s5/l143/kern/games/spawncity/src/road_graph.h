// The street network as a graph for police pursuit (SpawnCity police_ground_units.mjs: findPath
// over roadGraph, pure pursuit along the path). Nodes are the vertices of the streamed street
// centre lines; vertices that lie within a few metres of each other (where streets meet, or one
// street continues across a tile border) are the same node.
#pragma once

#include <vector>

#include "city_stream.h"

#include "kern/core/map.h"

namespace sc {

class RoadGraph {
 public:
  /// Rebuilds from the live lanes (cheap; call when the lane set changed).
  void rebuild(const std::vector<Lane>& lanes);
  bool empty() const { return nodes_.empty(); }
  std::size_t size() const { return nodes_.size(); }
  /// The node closest to `p` (-1 when the graph is empty or nothing lies within `max_m`).
  i32 nearest(Vec2 p, f32 max_m = 80) const;
  /// A* from the street closest to `from` to the one closest to `to`; `out` gets the points
  /// (starting at `from`'s projection, ending at `to`). False when no route exists.
  bool path(Vec2 from, Vec2 to, std::vector<Vec2>& out) const;
  /// Pure pursuit: the point `look` metres further along `path` from the projection of `p`.
  static Vec2 pursuit_point(const std::vector<Vec2>& path, Vec2 p, f32 look);
  /// Signature of the lane set it was built from (rebuild when it differs).
  u64 built_from = ~0ull;

 private:
  struct Node {
    Vec2 p;
    std::vector<std::pair<i32, f32>> edges;  // neighbour, length
  };
  i32 node_at(Vec2 p);
  std::vector<Node> nodes_;
  kern::Map<u64, i32> cells_;
};

/// A signature of the live lanes: changes when lanes appear or die.
u64 lane_signature(const std::vector<Lane>& lanes);

}  // namespace sc
