#pragma once
#include "kern/app/app.h"
#include "kern/logic/logic.h"   // (it has a class Frame of its own, inside kern::logic::Instance)
namespace sc {
using namespace kern;
/// The seconds a frame lasted, from the engine's Frame named as the program writes it (`using namespace kern;`).
f32 seconds_of(const Frame& frame);
}
