// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/audio/audio.h"
#include "kern/audio/synth.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_audio_Bus { using type = kern::audio::Bus; static constexpr type values[] = {kern::audio::Bus::master, kern::audio::Bus::music, kern::audio::Bus::sfx, kern::audio::Bus::ambience, kern::audio::Bus::ui, kern::audio::Bus::voice, kern::audio::Bus::count}; };
struct E_audio_Rolloff { using type = kern::audio::Rolloff; static constexpr type values[] = {kern::audio::Rolloff::inverse, kern::audio::Rolloff::linear, kern::audio::Rolloff::exponential}; };
struct E_audio_Wave { using type = kern::audio::Wave; static constexpr type values[] = {kern::audio::Wave::square, kern::audio::Wave::saw, kern::audio::Wave::sine, kern::audio::Wave::triangle, kern::audio::Wave::noise, kern::audio::Wave::pink, kern::audio::Wave::pulse}; };
struct S_audio_BuiltinSound { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::BuiltinSound& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::BuiltinSound& x); };
struct S_audio_BusSettings { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::BusSettings& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::BusSettings& x); };
struct S_audio_ClipInfo { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::ClipInfo& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::ClipInfo& x); };
struct S_audio_Listener { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::Listener& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::Listener& x); };
struct S_audio_Meter { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::Meter& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::Meter& x); };
struct S_audio_PlayParams { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::PlayParams& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::PlayParams& x); };
struct S_audio_Stats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::Stats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::Stats& x); };
struct S_audio_SynthPatch { [[maybe_unused]] static Value to(EngineObjects& o, const kern::audio::SynthPatch& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::audio::SynthPatch& x); };
Value S_audio_BuiltinSound::to(EngineObjects& o, const kern::audio::BuiltinSound& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Text>::to(o, x.label), ev::Conv<ev::Text>::to(o, x.category), ev::Conv<ev::Bool>::to(o, x.loop)}); }
void S_audio_BuiltinSound::from(EngineObjects& o, const Value& v, kern::audio::BuiltinSound& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.label); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.category); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.loop); }
Value S_audio_BusSettings::to(EngineObjects& o, const kern::audio::BusSettings& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.volume), ev::Conv<ev::Bool>::to(o, x.muted), ev::Conv<ev::Float>::to(o, x.lowpass_hz), ev::Conv<ev::Float>::to(o, x.reverb_send)}); }
void S_audio_BusSettings::from(EngineObjects& o, const Value& v, kern::audio::BusSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.volume); if (const Value* f = v.field(1)) ev::Conv<ev::Bool>::from(o, *f, x.muted); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.lowpass_hz); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.reverb_send); }
Value S_audio_ClipInfo::to(EngineObjects& o, const kern::audio::ClipInfo& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Int>::to(o, x.frames), ev::Conv<ev::Int>::to(o, x.channels), ev::Conv<ev::Float>::to(o, x.duration), ev::Conv<ev::Float>::to(o, x.peak), ev::Conv<ev::Bool>::to(o, x.loopable)}); }
void S_audio_ClipInfo::from(EngineObjects& o, const Value& v, kern::audio::ClipInfo& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.frames); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.channels); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.duration); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.peak); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.loopable); }
Value S_audio_Listener::to(EngineObjects& o, const kern::audio::Listener& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.forward), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Vector>::to(o, x.velocity)}); }
void S_audio_Listener::from(EngineObjects& o, const Value& v, kern::audio::Listener& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.forward); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.velocity); }
Value S_audio_Meter::to(EngineObjects& o, const kern::audio::Meter& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.peak), ev::Conv<ev::List<ev::Float>>::to(o, x.rms)}); }
void S_audio_Meter::from(EngineObjects& o, const Value& v, kern::audio::Meter& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.peak); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.rms); }
Value S_audio_PlayParams::to(EngineObjects& o, const kern::audio::PlayParams& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.volume), ev::Conv<ev::Float>::to(o, x.pitch), ev::Conv<ev::Float>::to(o, x.pan), ev::Conv<ev::Bool>::to(o, x.loop), ev::Conv<ev::Enum<E_audio_Bus>>::to(o, x.bus), ev::Conv<ev::Float>::to(o, x.fade_in), ev::Conv<ev::Float>::to(o, x.start), ev::Conv<ev::Int>::to(o, x.priority), ev::Conv<ev::Bool>::to(o, x.spatial), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.velocity), ev::Conv<ev::Float>::to(o, x.min_distance), ev::Conv<ev::Float>::to(o, x.max_distance), ev::Conv<ev::Enum<E_audio_Rolloff>>::to(o, x.rolloff), ev::Conv<ev::Float>::to(o, x.spread), ev::Conv<ev::Float>::to(o, x.doppler)}); }
void S_audio_PlayParams::from(EngineObjects& o, const Value& v, kern::audio::PlayParams& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.volume); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.pitch); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.pan); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.loop); if (const Value* f = v.field(4)) ev::Conv<ev::Enum<E_audio_Bus>>::from(o, *f, x.bus); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.fade_in); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.start); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.priority); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.spatial); if (const Value* f = v.field(9)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(10)) ev::Conv<ev::Vector>::from(o, *f, x.velocity); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.min_distance); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.max_distance); if (const Value* f = v.field(13)) ev::Conv<ev::Enum<E_audio_Rolloff>>::from(o, *f, x.rolloff); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.spread); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.doppler); }
Value S_audio_Stats::to(EngineObjects& o, const kern::audio::Stats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.voices), ev::Conv<ev::Int>::to(o, x.audible), ev::Conv<ev::Int>::to(o, x.virtual_voices), ev::Conv<ev::Int>::to(o, x.stolen), ev::Conv<ev::Int>::to(o, x.clips), ev::Conv<ev::Int64>::to(o, x.frames_rendered), ev::Conv<ev::Float>::to(o, x.limiter_reduction_db), ev::Conv<ev::Struct<S_audio_Meter>>::to(o, x.master), ev::Conv<ev::List<ev::Struct<S_audio_Meter>>>::to(o, x.buses), ev::Conv<ev::List<ev::Float>>::to(o, x.peak_hold)}); }
void S_audio_Stats::from(EngineObjects& o, const Value& v, kern::audio::Stats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.voices); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.audible); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.virtual_voices); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.stolen); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.clips); if (const Value* f = v.field(5)) ev::Conv<ev::Int64>::from(o, *f, x.frames_rendered); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.limiter_reduction_db); if (const Value* f = v.field(7)) ev::Conv<ev::Struct<S_audio_Meter>>::from(o, *f, x.master); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Struct<S_audio_Meter>>>::from(o, *f, x.buses); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.peak_hold); }
Value S_audio_SynthPatch::to(EngineObjects& o, const kern::audio::SynthPatch& x) { return Value::structure({ev::Conv<ev::Enum<E_audio_Wave>>::to(o, x.wave), ev::Conv<ev::Float>::to(o, x.volume), ev::Conv<ev::Float>::to(o, x.attack), ev::Conv<ev::Float>::to(o, x.sustain), ev::Conv<ev::Float>::to(o, x.punch), ev::Conv<ev::Float>::to(o, x.release), ev::Conv<ev::Float>::to(o, x.frequency), ev::Conv<ev::Float>::to(o, x.slide), ev::Conv<ev::Float>::to(o, x.slide_accel), ev::Conv<ev::Float>::to(o, x.min_frequency), ev::Conv<ev::Float>::to(o, x.vibrato_depth), ev::Conv<ev::Float>::to(o, x.vibrato_rate), ev::Conv<ev::Float>::to(o, x.arp_ratio), ev::Conv<ev::Float>::to(o, x.arp_time), ev::Conv<ev::Float>::to(o, x.duty), ev::Conv<ev::Float>::to(o, x.duty_sweep), ev::Conv<ev::Float>::to(o, x.fm_ratio), ev::Conv<ev::Float>::to(o, x.fm_depth), ev::Conv<ev::Float>::to(o, x.lowpass), ev::Conv<ev::Float>::to(o, x.lowpass_sweep), ev::Conv<ev::Float>::to(o, x.resonance), ev::Conv<ev::Float>::to(o, x.highpass), ev::Conv<ev::Float>::to(o, x.tremolo_depth), ev::Conv<ev::Float>::to(o, x.tremolo_rate), ev::Conv<ev::Float>::to(o, x.repeat), ev::Conv<ev::Float>::to(o, x.bitcrush), ev::Conv<ev::Int>::to(o, x.seed)}); }
void S_audio_SynthPatch::from(EngineObjects& o, const Value& v, kern::audio::SynthPatch& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_audio_Wave>>::from(o, *f, x.wave); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.volume); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.attack); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.sustain); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.punch); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.release); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.frequency); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.slide); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.slide_accel); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.min_frequency); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.vibrato_depth); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.vibrato_rate); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.arp_ratio); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.arp_time); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.duty); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.duty_sweep); if (const Value* f = v.field(16)) ev::Conv<ev::Float>::from(o, *f, x.fm_ratio); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.fm_depth); if (const Value* f = v.field(18)) ev::Conv<ev::Float>::from(o, *f, x.lowpass); if (const Value* f = v.field(19)) ev::Conv<ev::Float>::from(o, *f, x.lowpass_sweep); if (const Value* f = v.field(20)) ev::Conv<ev::Float>::from(o, *f, x.resonance); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.highpass); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.tremolo_depth); if (const Value* f = v.field(23)) ev::Conv<ev::Float>::from(o, *f, x.tremolo_rate); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.repeat); if (const Value* f = v.field(25)) ev::Conv<ev::Float>::from(o, *f, x.bitcrush); if (const Value* f = v.field(26)) ev::Conv<ev::Int>::from(o, *f, x.seed); }

// engine.audio.bus_name: kern::audio::bus_name
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Bus>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 0, a0);
  decltype(auto) r = kern::audio::bus_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.audio.parse_bus: kern::audio::parse_bus
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::audio::Bus&>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 1, a1);
  decltype(auto) r = kern::audio::parse_bus(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_audio_Bus>>(o, out, 1, a1);
  return true;
}

// engine.audio.rolloff_name: kern::audio::rolloff_name
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Rolloff>>();
  ev::take<ev::Enum<E_audio_Rolloff>>(o, in, 0, a0);
  decltype(auto) r = kern::audio::rolloff_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.audio.parse_rolloff: kern::audio::parse_rolloff
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::audio::Rolloff&>>();
  ev::take<ev::Enum<E_audio_Rolloff>>(o, in, 1, a1);
  decltype(auto) r = kern::audio::parse_rolloff(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_audio_Rolloff>>(o, out, 1, a1);
  return true;
}

// engine.mixer.init: kern::audio::Mixer::init
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->init(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mixer.sample_rate: kern::audio::Mixer::sample_rate
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  decltype(auto) r = self->sample_rate();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mixer.ready: kern::audio::Mixer::ready
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  decltype(auto) r = self->ready();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mixer.create_clip: kern::audio::Mixer::create_clip
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  std::vector<float> a0;
  ev::take<ev::List<ev::Float>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>>();
  ev::take<ev::Text>(o, in, 5, a4);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list data has fewer entries than samples says"; return false; }
  decltype(auto) r = self->create_clip(a0.empty() ? nullptr : a0.data(), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mixer.adopt_clip: kern::audio::Mixer::adopt_clip
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >>>();
  ev::take<ev::List<ev::Float>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>>();
  ev::take<ev::Text>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  decltype(auto) r = self->adopt_clip(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mixer.destroy_clip: kern::audio::Mixer::destroy_clip
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->destroy_clip(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mixer.clip_info: kern::audio::Mixer::clip_info
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->clip_info(std::move(a0));
  ev::give<ev::List<ev::Struct<S_audio_ClipInfo>>>(o, out, 0, r);
  return true;
}

// engine.mixer.clip_samples: kern::audio::Mixer::clip_samples
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->clip_samples(std::move(a0));
  ev::give<ev::List<ev::Row<ev::List<ev::Float>>>>(o, out, 0, r);
  return true;
}

// engine.mixer.clip_count: kern::audio::Mixer::clip_count
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  decltype(auto) r = self->clip_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mixer.play: kern::audio::Mixer::play
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::audio::PlayParams const&>>();
  ev::take<ev::Struct<S_audio_PlayParams>>(o, in, 2, a1);
  decltype(auto) r = self->play(std::move(a0), a1);
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.mixer.stop: kern::audio::Mixer::stop
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->stop(std::move(a0), std::move(a1));
  return true;
}

// engine.mixer.stop_all: kern::audio::Mixer::stop_all
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Bus>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->stop_all(std::move(a0), std::move(a1));
  return true;
}

// engine.mixer.playing: kern::audio::Mixer::playing
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  decltype(auto) r = self->playing(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mixer.set_volume: kern::audio::Mixer::set_volume
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->set_volume(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.mixer.set_pitch: kern::audio::Mixer::set_pitch
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->set_pitch(std::move(a0), std::move(a1));
  return true;
}

// engine.mixer.set_pan: kern::audio::Mixer::set_pan
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->set_pan(std::move(a0), std::move(a1));
  return true;
}

// engine.mixer.set_position: kern::audio::Mixer::set_position
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  self->set_position(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.mixer.set_paused: kern::audio::Mixer::set_paused
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->set_paused(std::move(a0), std::move(a1));
  return true;
}

// engine.mixer.set_all_paused: kern::audio::Mixer::set_all_paused
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_all_paused(std::move(a0));
  return true;
}

// engine.mixer.position_of: kern::audio::Mixer::position_of
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::u64>>();
  ev::take<ev::Int64>(o, in, 1, a0);
  decltype(auto) r = self->position_of(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.mixer.set_listener: kern::audio::Mixer::set_listener
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Listener const&>>();
  ev::take<ev::Struct<S_audio_Listener>>(o, in, 1, a0);
  self->set_listener(a0);
  return true;
}

// engine.mixer.listener: kern::audio::Mixer::listener
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  decltype(auto) r = self->listener();
  ev::give<ev::Struct<S_audio_Listener>>(o, out, 0, r);
  return true;
}

// engine.mixer.bus: kern::audio::Mixer::bus
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Bus>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 1, a0);
  if (static_cast<std::size_t>(a0) >= 6) { o.error = "the input b names no entry: there are 6"; return false; }
  decltype(auto) r = self->bus(std::move(a0));
  ev::give<ev::Struct<S_audio_BusSettings>>(o, out, 0, r);
  return true;
}

// engine.mixer.bus@set: what stands at the place kern::audio::Mixer::bus gives
bool c26_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Bus>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 1, a0);
  if (static_cast<std::size_t>(a0) >= 6) { o.error = "the input b names no entry: there are 6"; return false; }
  ev::take<ev::Struct<S_audio_BusSettings>>(o, in, 2, self->bus(std::move(a0)));
  return true;
}

// engine.mixer.bus_2: kern::audio::Mixer::bus
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::Bus>>();
  ev::take<ev::Enum<E_audio_Bus>>(o, in, 1, a0);
  decltype(auto) r = self->bus(std::move(a0));
  ev::give<ev::Struct<S_audio_BusSettings>>(o, out, 0, r);
  return true;
}

// engine.mixer.set_reverb: kern::audio::Mixer::set_reverb
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->set_reverb(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.mixer.render: kern::audio::Mixer::render
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<2u>>>();
  ev::take<ev::Int>(o, in, 1, a1);
  ev::Written<float> a0(ev::room(a1) * 2);
  self->render(a0.data(), std::move(a1));
  if (!a0.kept(o, "out")) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, a0.room);
  return true;
}

// engine.mixer.stats: kern::audio::Mixer::stats
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_audio_Stats>>(o, out, 0, r);
  return true;
}

// engine.audio.synthesize: kern::audio::synthesize
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::audio::SynthPatch const&>>();
  ev::take<ev::Struct<S_audio_SynthPatch>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::audio::synthesize(a0, std::move(a1));
  ev::give<ev::List<ev::Float>>(o, out, 0, r);
  return true;
}

// engine.audio.mix_into: kern::audio::mix_into
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> > const&>>();
  ev::take<ev::List<ev::Float>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a4);
  kern::audio::mix_into(a0, a1, std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::List<ev::Float>>(o, out, 0, a0);
  return true;
}

// engine.audio.make_seamless: kern::audio::make_seamless
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a2);
  kern::audio::make_seamless(a0, std::move(a1), std::move(a2));
  ev::give<ev::List<ev::Float>>(o, out, 0, a0);
  return true;
}

// engine.audio.normalize: kern::audio::normalize
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a1);
  kern::audio::normalize(a0, std::move(a1));
  ev::give<ev::List<ev::Float>>(o, out, 0, a0);
  return true;
}

// engine.audio.builtin_sounds: kern::audio::builtin_sounds
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::audio::builtin_sounds();
  ev::give<ev::List<ev::Struct<S_audio_BuiltinSound>>>(o, out, 0, r);
  return true;
}

// engine.audio.find_builtin: kern::audio::find_builtin
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::audio::find_builtin(a0);
  ev::give<ev::List<ev::Struct<S_audio_BuiltinSound>>>(o, out, 0, r);
  return true;
}

// engine.audio.render_builtin: kern::audio::render_builtin
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int&>>();
  ev::take<ev::Int>(o, in, 3, a3);
  decltype(auto) r = kern::audio::render_builtin(a0, std::move(a1), a2, a3);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::List<ev::Float>>(o, out, 1, a2);
  ev::give<ev::Int>(o, out, 2, a3);
  return true;
}

// engine.builtin_job.start: kern::audio::BuiltinJob::start
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::BuiltinJob>(o, in, 0, "builtin_job");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->start(a0, std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.builtin_job.step: kern::audio::BuiltinJob::step
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::BuiltinJob>(o, in, 0, "builtin_job");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  decltype(auto) r = self->step(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.builtin_job.done: kern::audio::BuiltinJob::done
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::BuiltinJob>(o, in, 0, "builtin_job");
  if (!self) return false;
  decltype(auto) r = self->done();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.builtin_job.name: kern::audio::BuiltinJob::name
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::BuiltinJob>(o, in, 0, "builtin_job");
  if (!self) return false;
  decltype(auto) r = self->name();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.builtin_job.take: kern::audio::BuiltinJob::take
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::BuiltinJob>(o, in, 0, "builtin_job");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int&>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->take(a0);
  ev::give<ev::List<ev::Float>>(o, out, 0, r);
  ev::give<ev::Int>(o, out, 1, a0);
  return true;
}

// engine.builtin_job.make: kern::audio::BuiltinJob::BuiltinJob
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::audio::BuiltinJob>(std::unique_ptr<kern::audio::BuiltinJob>(new kern::audio::BuiltinJob()))));
  return true;
}

// engine.builtin_job.release: kern::audio::BuiltinJob::~BuiltinJob
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "builtin_job", r)) return false;
  return ev::release_of<kern::audio::BuiltinJob>(o, r, "builtin_job", "BuiltinJob");
}

// engine.mixer.make: kern::audio::Mixer::Mixer
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::audio::Mixer>(std::unique_ptr<kern::audio::Mixer>(new kern::audio::Mixer()))));
  return true;
}

// engine.mixer.release: kern::audio::Mixer::~Mixer
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "mixer", r)) return false;
  return ev::release_of<kern::audio::Mixer>(o, r, "mixer", "Mixer");
}

// engine.mixer.speed_of_sound: kern::audio::Mixer::speed_of_sound
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->speed_of_sound);
  return true;
}

// engine.mixer.speed_of_sound@set: what stands at the place kern::audio::Mixer::speed_of_sound gives
bool c47_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->speed_of_sound);
  return true;
}

// engine.mixer.doppler_factor: kern::audio::Mixer::doppler_factor
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->doppler_factor);
  return true;
}

// engine.mixer.doppler_factor@set: what stands at the place kern::audio::Mixer::doppler_factor gives
bool c48_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->doppler_factor);
  return true;
}

const EngineCall kCalls[] = {
    {"engine.audio.bus_name", &c0},
    {"engine.audio.parse_bus", &c1},
    {"engine.audio.rolloff_name", &c2},
    {"engine.audio.parse_rolloff", &c3},
    {"engine.mixer.init", &c4},
    {"engine.mixer.sample_rate", &c5},
    {"engine.mixer.ready", &c6},
    {"engine.mixer.create_clip", &c7},
    {"engine.mixer.adopt_clip", &c8},
    {"engine.mixer.destroy_clip", &c9},
    {"engine.mixer.clip_info", &c10},
    {"engine.mixer.clip_samples", &c11},
    {"engine.mixer.clip_count", &c12},
    {"engine.mixer.play", &c13},
    {"engine.mixer.stop", &c14},
    {"engine.mixer.stop_all", &c15},
    {"engine.mixer.playing", &c16},
    {"engine.mixer.set_volume", &c17},
    {"engine.mixer.set_pitch", &c18},
    {"engine.mixer.set_pan", &c19},
    {"engine.mixer.set_position", &c20},
    {"engine.mixer.set_paused", &c21},
    {"engine.mixer.set_all_paused", &c22},
    {"engine.mixer.position_of", &c23},
    {"engine.mixer.set_listener", &c24},
    {"engine.mixer.listener", &c25},
    {"engine.mixer.bus", &c26},
    {"engine.mixer.bus_2", &c27},
    {"engine.mixer.set_reverb", &c28},
    {"engine.mixer.render", &c29},
    {"engine.mixer.stats", &c30},
    {"engine.audio.synthesize", &c31},
    {"engine.audio.mix_into", &c32},
    {"engine.audio.make_seamless", &c33},
    {"engine.audio.normalize", &c34},
    {"engine.audio.builtin_sounds", &c35},
    {"engine.audio.find_builtin", &c36},
    {"engine.audio.render_builtin", &c37},
    {"engine.builtin_job.start", &c38},
    {"engine.builtin_job.step", &c39},
    {"engine.builtin_job.done", &c40},
    {"engine.builtin_job.name", &c41},
    {"engine.builtin_job.take", &c42},
    {"engine.builtin_job.make", &c43},
    {"engine.builtin_job.release", &c44},
    {"engine.mixer.make", &c45},
    {"engine.mixer.release", &c46},
    {"engine.mixer.speed_of_sound", &c47},
    {"engine.mixer.doppler_factor", &c48},
    {"engine.mixer.bus@get", &c26},
    {"engine.mixer.bus@set", &c26_set},
    {"engine.mixer.speed_of_sound@get", &c47},
    {"engine.mixer.speed_of_sound@set", &c47_set},
    {"engine.mixer.doppler_factor@get", &c48},
    {"engine.mixer.doppler_factor@set", &c48_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::audio::BuiltinJob>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::audio::BuiltinJob>(handle, std::unique_ptr<kern::audio::BuiltinJob>(new kern::audio::BuiltinJob())); }},
    {EngineObjects::kind<kern::audio::Mixer>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::audio::Mixer>(handle, std::unique_ptr<kern::audio::Mixer>(new kern::audio::Mixer())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

}  // namespace
}  // namespace kern::logic
