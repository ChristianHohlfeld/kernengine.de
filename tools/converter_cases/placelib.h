#pragma once
#include "kern/game/runtime.h"
namespace cases {
/// A function of a library (this file's Blueprint) that names a place of the engine (`auto& ev = w->events();` -
/// GameWorld::events is KERN_PLACE: its node binds the place, the nodes that read and clear it name that node) -
/// and a function of another file (placelib_b.cpp) that calls it.
int take_events(kern::game::GameWorld* w);
}
