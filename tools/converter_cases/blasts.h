#pragma once
#include <vector>
#include "kern/math/math.h"
namespace cases {
/// A Blast of the namespace - and a state with a Blast of its own (as a game has several structs of one short name).
struct Blast { kern::Vec3 p; kern::f32 born = 0; };
struct LookState {
  struct Blast { kern::Vec3 p; kern::f32 at, ms; };
  std::vector<Blast> blasts;
};
class Viewer {
 public:
  void blast(kern::Vec3 at, kern::f32 t);
  kern::i32 count() const { return static_cast<kern::i32>(look_.blasts.size()); }
 private:
  LookState look_;
  std::vector<Blast> own_;
};
Viewer& viewer();
}
