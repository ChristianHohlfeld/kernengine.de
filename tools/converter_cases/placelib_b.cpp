#include "placelib_b.h"
namespace cases {
int events_twice(kern::game::GameWorld* w) { return take_events(w) + take_events(w); }
}
