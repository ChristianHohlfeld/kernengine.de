// Vehicle sounds (see sound_fx.h).
#include "sound_fx.h"

#include "kern/audio/synth.h"

namespace sc {

namespace {
using kern::audio::SynthPatch;
using kern::audio::Wave;

std::vector<f32> loop_of(std::initializer_list<SynthPatch> layers, u32 rate, f32 seconds) {
  std::vector<f32> out(static_cast<std::size_t>(seconds * rate), 0.0f);
  for (SynthPatch p : layers) {
    p.attack = 0.0f;
    p.sustain = seconds + 0.25f;
    p.release = 0.0f;
    kern::audio::mix_into(out, kern::audio::synthesize(p, rate), 0, rate, 1);
  }
  out.resize(static_cast<std::size_t>(seconds * rate));
  kern::audio::make_seamless(out, rate, 0.12f);
  kern::audio::normalize(out, 0.8f);
  return out;
}
}  // namespace

bool SoundFx::init(kern::audio::Mixer& mixer) {
  mixer_ = &mixer;
  const u32 rate = mixer.sample_rate();
  if (!rate) return false;
  // Drone: motor whine at 140 Hz with its harmonics (saw through a resonant low-pass), the blade
  // pass at 100 Hz as a tremolo, prop wash as coloured noise.
  {
    SynthPatch motor;
    motor.wave = Wave::saw;
    motor.frequency = 140;
    motor.lowpass = 2600;
    motor.resonance = 0.35f;
    motor.tremolo_depth = 0.45f;
    motor.tremolo_rate = 100;
    motor.volume = 0.6f;
    SynthPatch harm = motor;
    harm.wave = Wave::square;
    harm.frequency = 280;
    harm.duty = 0.3f;
    harm.volume = 0.18f;
    harm.lowpass = 3800;
    rotor_.clip = mixer.adopt_clip(loop_of({motor, harm}, rate, 1.0f), 1, "spawncity:rotor", true);
    SynthPatch wash;
    wash.wave = Wave::pink;
    wash.lowpass = 1400;
    wash.highpass = 120;
    wash.volume = 0.5f;
    wash.seed = 7;
    prop_wash_.clip = mixer.adopt_clip(loop_of({wash}, rate, 2.0f), 1, "spawncity:propwash", true);
  }
  // Car: a four-cylinder at idle (firing 30 Hz) with crank harmonics; pitch follows revs.
  {
    SynthPatch base;
    base.wave = Wave::pulse;
    base.duty = 0.22f;
    base.frequency = 55;
    base.lowpass = 900;
    base.resonance = 0.2f;
    base.volume = 0.6f;
    SynthPatch rumble;
    rumble.wave = Wave::sine;
    rumble.frequency = 27.5f;
    rumble.volume = 0.45f;
    SynthPatch intake;
    intake.wave = Wave::noise;
    intake.lowpass = 600;
    intake.volume = 0.12f;
    intake.seed = 3;
    engine_.clip = mixer.adopt_clip(loop_of({base, rumble, intake}, rate, 1.0f), 1, "spawncity:engine", true);
  }
  // Jet: turbine whine (two inharmonic sines) and the exhaust roar (low-passed pink noise).
  {
    SynthPatch whine;
    whine.wave = Wave::sine;
    whine.frequency = 1180;
    whine.vibrato_depth = 0.05f;
    whine.vibrato_rate = 3;
    whine.volume = 0.35f;
    SynthPatch whine2 = whine;
    whine2.frequency = 2240;
    whine2.volume = 0.15f;
    turbine_.clip = mixer.adopt_clip(loop_of({whine, whine2}, rate, 1.0f), 1, "spawncity:turbine", true);
    SynthPatch roar;
    roar.wave = Wave::pink;
    roar.lowpass = 700;
    roar.volume = 0.9f;
    roar.seed = 11;
    SynthPatch crackle;
    crackle.wave = Wave::noise;
    crackle.lowpass = 2400;
    crackle.highpass = 900;
    crackle.volume = 0.12f;
    crackle.seed = 13;
    roar_.clip = mixer.adopt_clip(loop_of({roar, crackle}, rate, 2.0f), 1, "spawncity:roar", true);
  }
  {
    SynthPatch air;
    air.wave = Wave::pink;
    air.lowpass = 900;
    air.highpass = 60;
    air.volume = 0.6f;
    air.seed = 21;
    wind_.clip = mixer.adopt_clip(loop_of({air}, rate, 3.0f), 1, "spawncity:wind", true);
  }
  return rotor_.clip && engine_.clip && turbine_.clip && roar_.clip && wind_.clip;
}

void SoundFx::drive(Loop& l, bool on, Vec3 pos, Vec3 vel, f32 volume, f32 pitch, bool spatial, f32 max_distance) {
  if (!mixer_ || !l.clip) return;
  if (!on || volume < 0.002f) {
    if (l.voice) { mixer_->stop(l.voice, 0.15f); l.voice = 0; }
    return;
  }
  if (!l.voice || !mixer_->playing(l.voice)) {
    kern::audio::PlayParams p;
    p.loop = true;
    p.volume = volume;
    p.pitch = pitch;
    p.spatial = spatial;
    p.position = pos;
    p.velocity = vel;
    p.min_distance = 3;
    p.max_distance = max_distance;
    p.bus = kern::audio::Bus::sfx;
    p.fade_in = 0.1f;
    l.voice = mixer_->play(l.clip, p);
    return;
  }
  mixer_->set_volume(l.voice, volume);
  mixer_->set_pitch(l.voice, clampf(pitch, 0.05f, 8));
  if (spatial) mixer_->set_position(l.voice, pos, vel);
}

void SoundFx::update(const SoundState& s, f32 dt) {
  (void)dt;
  // Drone: SpawnCity mixes it at 50 % (audio_settings.mjs drone 50).
  drive(rotor_, s.drone.active, s.drone.position, s.drone.velocity, 0.5f * clampf(0.15f + s.drone.rate, 0, 1), 0.35f + 1.9f * s.drone.rate, true, 140);
  drive(prop_wash_, s.drone.active, s.drone.position, s.drone.velocity, 0.35f * s.drone.load, 0.7f + 0.6f * s.drone.rate, true, 90);
  // Vehicle 58 %.
  drive(engine_, s.car.active, s.car.position, s.car.velocity, 0.58f * (0.35f + 0.65f * s.car.load), 0.75f + 2.2f * s.car.rate, true, 120);
  drive(turbine_, s.jet.active, s.jet.position, s.jet.velocity, 0.45f * clampf(0.2f + s.jet.rate, 0, 1), 0.55f + 0.8f * s.jet.rate, true, 900);
  drive(roar_, s.jet.active, s.jet.position, s.jet.velocity, 0.9f * s.jet.load, 0.7f + 0.5f * s.jet.load, true, 1500);
  drive(wind_, true, {}, {}, clampf((s.wind - 6) / 60, 0, 0.7f), 0.8f + std::fmin(0.6f, s.wind / 120), false, 0);
}

}  // namespace sc
