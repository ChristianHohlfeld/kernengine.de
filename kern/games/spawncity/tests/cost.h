// What a function's own work costs: how often it asks for memory.
//
// The allocator of the tests counts while `counting` is on (test_main.cpp). The comparison of a
// converted function with its source turns it on around the source's call and - through the
// engine's logic::native_probe - around the call of the Blueprint's compiled class (converted.h):
// the class does not ask for memory more often than the source it was written from.
#pragma once

namespace sc::cost {

inline bool counting = false;  // the allocator counts
inline long asks = 0;          // what it counted since the count began

/// Called by the allocator for every piece of memory it is asked for.
inline void asked() {
  if (counting) ++asks;
}
inline void begin() {
  asks = 0;
  counting = true;
}
inline long end() {
  counting = false;
  return asks;
}

}  // namespace sc::cost
