// Vehicle sounds on Kern's mixer: synth patches rendered once into seamless loops (SpawnCity
// builds them procedurally too: motor_sound.mjs blade 100 Hz + motor 140 Hz with harmonics, the
// jet's turbine and roar, the car's engine), played as 3D voices whose pitch and level follow the
// simulation every frame. Everything else (shots, steps, impacts) comes from Kern's GameAudio.
#pragma once

#include "common.h"
#include "kern/audio/audio.h"
#include "kern/math/math.h"

namespace sc {

struct EngineSound {
  bool active = false;
  Vec3 position, velocity;
  f32 rate = 0;   // 0..1: rotor speed / engine revs / spool
  f32 load = 0;   // 0..1: thrust / throttle
};

struct SoundState {
  EngineSound drone, car, jet;
  f32 wind = 0;   // m/s of the player (air rush)
};

class SoundFx {
 public:
  bool init(kern::audio::Mixer& mixer);
  void update(const SoundState& s, f32 dt);
  bool ready() const { return mixer_ != nullptr; }

 private:
  struct Loop {
    kern::audio::ClipId clip = 0;
    kern::audio::VoiceId voice = 0;
    f32 volume = 0;
  };
  void drive(Loop& l, bool on, Vec3 pos, Vec3 vel, f32 volume, f32 pitch, bool spatial, f32 max_distance);

  kern::audio::Mixer* mixer_ = nullptr;
  Loop rotor_, prop_wash_, engine_, turbine_, roar_, wind_;
};

}  // namespace sc
