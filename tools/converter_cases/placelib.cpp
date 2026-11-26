#include "placelib.h"
namespace cases {
int take_events(kern::game::GameWorld* w) {
  auto& ev = w->events();
  const int n = static_cast<int>(ev.size());
  ev.clear();
  return n;
}
}
