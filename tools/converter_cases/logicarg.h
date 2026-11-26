#pragma once
#include "kern/logic/logic.h"
namespace cases {
/// A function that takes the logic the level's Blueprints run in (kern::logic::Runtime&, no input of a Blueprint: its
/// own) and one that calls it from another file (logicarg_b.cpp), handing it on.
bool ready_in(kern::logic::Runtime& logic, int k);
bool both_ready(kern::logic::Runtime& logic);
}
