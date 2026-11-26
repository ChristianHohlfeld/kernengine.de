#pragma once
#include "kern/audio/synth.h"
namespace cases {
/// A patch of the engine's synthesizer (kern::audio::SynthPatch, its enum Wave), named as the program's file does it:
/// `using kern::audio::Wave;` before the function.
kern::f32 bright(kern::f32 frequency);
}
