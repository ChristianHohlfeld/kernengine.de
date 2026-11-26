#include "patch.h"
namespace cases {
using kern::audio::SynthPatch;
using kern::audio::Wave;
kern::f32 bright(kern::f32 frequency) {
  SynthPatch p;
  p.wave = Wave::saw;
  p.frequency = frequency;
  return p.wave == Wave::saw ? p.frequency * 2 : p.volume;
}
}
