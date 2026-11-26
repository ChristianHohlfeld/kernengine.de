#include "blasts.h"
namespace cases {
void Viewer::blast(kern::Vec3 at, kern::f32 t) {
  LookState& L = look_;
  if (L.blasts.size() >= 4) L.blasts.erase(L.blasts.begin());
  L.blasts.push_back({at, t, 0.42f});
  own_.push_back({at, t});
}
}
