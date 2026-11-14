// SpawnCity's equipment: jetpack, flight logbook, procedural music (see equipment.h).
#include "equipment.h"

#include "blueprint_call.h"

#include <cmath>
#include <algorithm>
#include <cstdio>

namespace sc {

// ---------------------------------------------------------------------------------------------
// Jetpack (player_walk_mode_v4.mjs: JET_ACC 21, JET_BURST_S .6, JET_FUEL_PER_S .2, JET_RECHARGE_S 4,
// JET_AIR_ACCEL 14, JET_CUSHION_FUEL .02, JET_AIR_REGEN_DELAY_MS 350, JET_CLIMB_ACC 13.8,
// JET_MAX_CLIMB_MPS 8; a full tank is ~5 s of thrust, it refills in ~4 s on the ground)

namespace {
rules::Blueprint g_jetpack{"Jetpack"};
}  // namespace
void Jetpack::bind(kern::logic::Instance* blueprint) { g_jetpack.running = blueprint; }
f32 Jetpack::acc() { return static_cast<f32>(g_jetpack.get<f64>("Acc")); }
f32 Jetpack::air_accel() { return static_cast<f32>(g_jetpack.get<f64>("AirAccel")); }
f32 Jetpack::max_climb() { return static_cast<f32>(g_jetpack.get<f64>("MaxClimb")); }
f32 Jetpack::gravity() { return static_cast<f32>(g_jetpack.get<f64>("Gravity")); }
f32 Jetpack::fall_safe() { return static_cast<f32>(g_jetpack.get<f64>("FallSafe")); }
f32 Jetpack::ignition_damp() { return static_cast<f32>(g_jetpack.get<f64>("IgnitionDamp")); }

bool Jetpack::ignite(f32 vz) {
  (void)vz;
  Pack p;
  bool lit = false;
  g_jetpack.call("Ignite", std::tie(p, lit), pack());
  take(p);
  return lit;
}

f32 Jetpack::update(f32 dt, f32 now, bool airborne, f32 vz, f32 height, bool hold) {
  Pack p;
  f64 thrust = 0;
  g_jetpack.call("Update", std::tie(p, thrust), pack(), rules::decimal(dt), rules::decimal(now), airborne, rules::decimal(vz), rules::decimal(height), hold);
  take(p);
  return static_cast<f32>(thrust);
}

Jetpack::Pack Jetpack::pack() const {
  return {rules::decimal(fuel), thrusting, used, ignitions, rules::decimal(burst_left_), rules::decimal(ground_since_), rules::decimal(last_thrust_)};
}
void Jetpack::take(const Pack& p) {
  fuel = static_cast<f32>(p.fuel);
  thrusting = p.thrusting;
  used = p.used;
  ignitions = p.ignitions;
  burst_left_ = static_cast<f32>(p.burst_left);
  ground_since_ = static_cast<f32>(p.ground_since);
  last_thrust_ = static_cast<f32>(p.last_thrust);
}

// ---------------------------------------------------------------------------------------------
// Flight logbook (flight_logbook.mjs: 50 flights, path every 0.5 s, at most 600 points, 120 events)

void FlightLog::observe(const FlightSample& s, const char* disarm_reason) {
  if (s.armed && !active_) {
    cur_ = Flight{};
    cur_.id = next_id_++;
    cur_.start = s.t;
    cur_.volts_start = cur_.volts_end = cur_.volts_min = s.volts;
    cur_.max_alt = s.pos.z;
    active_ = true;
    have_last_ = false;
    last_path_ = -1e9f;
    record(s, true);
    return;
  }
  if (!active_) return;
  if (s.t + 1e-4f < last_t_) { finish("SIM_RESET"); return; }
  record(s, false);
  if (!s.armed) finish(disarm_reason);
}

void FlightLog::record(const FlightSample& s, bool force) {
  if (have_last_) {
    const f32 step = kern::length(s.pos - last_);
    if (step < 20) cur_.distance += step;  // a teleport is no flight
  }
  last_ = s.pos;
  have_last_ = true;
  last_t_ = s.t;
  cur_.duration = s.t - cur_.start;
  cur_.max_speed = std::fmax(cur_.max_speed, kern::length(s.vel));
  if (s.agl >= 0) cur_.max_agl = std::fmax(cur_.max_agl, s.agl);
  cur_.max_alt = std::fmax(cur_.max_alt, s.pos.z);
  cur_.volts_end = s.volts;
  cur_.volts_min = std::fmin(cur_.volts_min, s.volts);
  if ((force || s.t - last_path_ >= kPathInterval) && cur_.path.size() < kMaxPath) {
    last_path_ = s.t;
    cur_.path.push_back({s.pos.x, s.pos.y, s.agl});
  }
}

void FlightLog::finish(const char* reason) {
  if (!active_) return;
  active_ = false;
  cur_.reason = reason;
  flights_.insert(flights_.begin(), cur_);
  if (flights_.size() > kMaxFlights) flights_.resize(kMaxFlights);
  ++serial_;
}

void FlightLog::network(const std::string& stage, f32 t) {
  events_.insert(events_.begin(), Event{t, stage});
  if (events_.size() > kMaxEvents) events_.resize(kMaxEvents);
  ++serial_;
}

std::string FlightLog::json() const {
  std::string o = "{\"flights\":[";
  char b[256];
  for (std::size_t i = 0; i < flights_.size(); ++i) {
    const Flight& f = flights_[i];
    std::snprintf(b, sizeof b, "%s{\"id\":%u,\"start\":%.2f,\"duration\":%.2f,\"distance\":%.1f,\"maxSpeed\":%.2f,\"maxAgl\":%.2f,\"maxAlt\":%.2f,\"voltsStart\":%.2f,\"voltsEnd\":%.2f,\"voltsMin\":%.2f,\"reason\":\"%s\",\"path\":[",
                  i ? "," : "", f.id, f.start, f.duration, f.distance, f.max_speed, f.max_agl, f.max_alt, f.volts_start, f.volts_end, f.volts_min, f.reason.c_str());
    o += b;
    for (std::size_t k = 0; k < f.path.size(); ++k) {
      std::snprintf(b, sizeof b, "%s[%.1f,%.1f,%.1f]", k ? "," : "", f.path[k].x, f.path[k].y, f.path[k].z);
      o += b;
    }
    o += "]}";
  }
  o += "],\"events\":[";
  for (std::size_t i = 0; i < events_.size(); ++i) {
    std::string stage;
    for (char c : events_[i].stage) stage += (c == '"' || c == '\\') ? '\'' : (static_cast<unsigned char>(c) < 0x20 ? ' ' : c);
    std::snprintf(b, sizeof b, "%s{\"t\":%.1f,\"stage\":\"", i ? "," : "", events_[i].t);
    o += b + stage + "\"}";
  }
  return o + "]}";
}

// ---------------------------------------------------------------------------------------------
// Music: the title music (music_player.mjs plays an 8-bit track in the menu only) and the car radio
// (car_radio.mjs, N / B, a "RADIO AUS" slot) as generated programmes: a 16th-note sequencer over a
// four-chord progression, drums from noise and pitched sines, bass, lead and pad from simple
// oscillators. Everything is generated here; no recording is played.

const MusicSynth::Station MusicSynth::kStations[] = {
    {"KERN FM", "Synthwave", 100, 11},
    {"BODENSEE BEATS", "House", 124, 23},
    {"LOFI LAGUNE", "Lo-Fi", 78, 37},
    {"8-BIT RADIO", "Chiptune", 144, 41},
    {"NACHTSCHICHT", "Techno", 132, 53},
};
i32 MusicSynth::station_count() { return static_cast<i32>(sizeof kStations / sizeof kStations[0]); }

namespace {
enum Genre { synthwave, house, lofi, chip, techno };
struct Prog { Genre genre; f32 bpm; u32 seed; i32 root; bool minor; };
Prog prog_of(i32 p) {
  if (p == MusicSynth::kMenu) return {chip, 140, 7, 57, true};  // the title: 8 bits, A minor
  const MusicSynth::Station& s = MusicSynth::kStations[p];
  const Genre g = static_cast<Genre>(p % 5);
  return {g, s.bpm, s.seed, g == house ? 53 : g == lofi ? 55 : g == techno ? 50 : 52, g != house};
}
f32 midi(f32 n) { return 440.0f * std::pow(2.0f, (n - 69) / 12); }
constexpr i32 kMinorProg[4] = {0, 8, 3, 10};  // i - VI - III - VII
constexpr i32 kMajorProg[4] = {0, 7, 9, 5};   // I - V - vi - IV
constexpr i32 kMinorScale[7] = {0, 2, 3, 5, 7, 8, 10};
constexpr i32 kMajorScale[7] = {0, 2, 4, 5, 7, 9, 11};
}  // namespace

void MusicSynth::init(u32 rate) { rate_ = rate; }

void MusicSynth::select(i32 programme, f32 level) {
  want_ = programme;
  target_ = clampf(level, 0, 1);
}

void MusicSynth::step() {
  const Prog p = prog_of(programme_);
  const u32 pos = step_ % 16, bar = step_ / 16;
  auto rnd = [&]() { rng_ = rng_ * 1664525u + 1013904223u; return static_cast<f32>(rng_ >> 8) / 16777216.0f; };
  const i32 chord = (p.minor ? kMinorProg : kMajorProg)[bar % 4];
  const f32 rate = static_cast<f32>(rate_);
  auto decay = [&](f32 tau) { return std::exp(-1.0f / (tau * rate)); };
  // drums
  bool kick = false, snare = pos == 4 || pos == 12, hat = false;
  switch (p.genre) {
    case house: case techno: kick = pos % 4 == 0; hat = p.genre == house ? pos % 4 == 2 : true; break;
    case synthwave: kick = pos == 0 || pos == 8 || pos == 10; hat = pos % 2 == 0; break;
    case lofi: kick = pos == 0 || pos == 7 || pos == 10; hat = pos % 2 == 0 && rnd() < 0.8f; break;
    case chip: kick = pos % 8 == 0 || pos == 14; hat = pos % 2 == 1; break;
  }
  if (p.genre == techno) snare = pos == 4 || pos == 12 ? rnd() < 0.5f : false;
  if (kick) { kick_.env = 1; kick_.phase = 0; kick_.decay = decay(0.16f); }
  if (snare) { snare_.env = 1; snare_.decay = decay(p.genre == lofi ? 0.09f : 0.12f); }
  if (hat) { hat_.env = p.genre == techno && pos % 2 ? 0.5f : 1; hat_.decay = decay(p.genre == techno ? 0.03f : 0.045f); }
  // bass on the chord root
  const bool bass_hit = p.genre == house ? pos % 4 == 2 : p.genre == techno ? pos % 2 == 1 : p.genre == lofi ? (pos == 0 || pos == 10) : pos % 2 == 0;
  if (bass_hit) { bass_.freq = midi(static_cast<f32>(p.root - 12 + chord)); bass_.env = 1; bass_.decay = decay(p.genre == lofi ? 0.5f : 0.18f); }
  // lead: an arpeggio through the scale above the chord
  const f32 lead_prob = p.genre == chip ? 0.85f : p.genre == synthwave ? 0.55f : p.genre == lofi ? 0.3f : 0.25f;
  if ((pos % 2 == 0 || p.genre == chip) && rnd() < lead_prob) {
    const i32 deg = static_cast<i32>(rnd() * 7) % 7, oct = rnd() < 0.3f ? 12 : 0;
    lead_.freq = midi(static_cast<f32>(p.root + 12 + chord + (p.minor ? kMinorScale[deg] : kMajorScale[deg]) + oct));
    lead_.env = 1;
    lead_.decay = decay(p.genre == chip ? 0.09f : 0.22f);
  }
  // pad: the chord, a new one every bar
  if (pos == 0 && p.genre != chip && p.genre != techno) {
    const i32 third = p.minor ? 3 : 4;
    const i32 tones[3] = {0, third, 7};
    for (int i = 0; i < 3; ++i) { pad_[i].freq = midi(static_cast<f32>(p.root + chord + tones[i])); pad_[i].env = 1; pad_[i].decay = decay(1.6f); }
  }
  ++step_;
}

f32 MusicSynth::next_sample(f32& right) {
  const Prog p = prog_of(programme_);
  const f32 inv = 1.0f / static_cast<f32>(rate_);
  auto noise = [&]() { rng_ = rng_ * 1664525u + 1013904223u; return static_cast<f32>(static_cast<i32>(rng_)) / 2147483648.0f; };
  f32 s = 0;
  // kick: a sine dropping from 150 to 45 Hz
  if (kick_.env > 1e-4f) {
    const f32 f = 45 + 105 * kick_.env * kick_.env;
    kick_.phase += f * inv;
    s += std::sin(kick_.phase * 6.2831853f) * kick_.env * 0.9f;
    kick_.env *= kick_.decay;
  }
  if (snare_.env > 1e-4f) { s += noise() * snare_.env * 0.32f; snare_.env *= snare_.decay; }
  if (hat_.env > 1e-4f) {
    const f32 n = noise();
    s += (n - hat_.phase) * hat_.env * 0.12f;  // first difference: a crude high-pass
    hat_.phase = n;
    hat_.env *= hat_.decay;
  }
  if (bass_.env > 1e-4f) {
    bass_.phase += bass_.freq * inv;
    bass_.phase -= std::floor(bass_.phase);
    const f32 raw = p.genre == chip ? (bass_.phase < 0.5f ? 4 * bass_.phase - 1 : 3 - 4 * bass_.phase)  // triangle
                                    : 2 * bass_.phase - 1;                                              // saw
    bass_lp_ += (raw - bass_lp_) * (p.genre == lofi ? 0.04f : 0.12f);
    s += bass_lp_ * bass_.env * 0.45f;
    bass_.env *= bass_.decay;
  }
  f32 lead = 0;
  if (lead_.env > 1e-4f) {
    lead_.phase += lead_.freq * inv;
    lead_.phase -= std::floor(lead_.phase);
    const f32 duty = p.genre == chip ? 0.25f : 0.5f;
    const f32 sq = lead_.phase < duty ? 1.0f : -1.0f;
    const f32 tri = lead_.phase < 0.5f ? 4 * lead_.phase - 1 : 3 - 4 * lead_.phase;
    lead = (p.genre == chip || p.genre == synthwave ? sq * 0.6f + tri * 0.4f : tri) * lead_.env * 0.16f;
    lead_.env *= lead_.decay;
  }
  f32 pad = 0;
  for (Voice& v : pad_) {
    if (v.env < 1e-4f) continue;
    v.phase += v.freq * inv;
    v.phase2 += v.freq * 1.006f * inv;  // detuned twin
    v.phase -= std::floor(v.phase);
    v.phase2 -= std::floor(v.phase2);
    pad += ((2 * v.phase - 1) + (2 * v.phase2 - 1)) * v.env * 0.035f;
    v.env *= v.decay;
  }
  lp_ += (pad - lp_) * (p.genre == lofi ? 0.03f : 0.08f);
  // A dotted-eighth echo on the lead (synthwave, title), wider in stereo.
  const u32 echo_len = std::min<u32>(sizeof echo_ / sizeof echo_[0], static_cast<u32>(samples_per_step_ * 3));
  f32 echo = 0;
  if (echo_len > 0) {
    echo_at_ %= echo_len;
    echo = echo_[echo_at_];
    echo_[echo_at_] = lead + echo * 0.38f;
    ++echo_at_;
  }
  const f32 mono = s + lp_;
  right = mono + lead * 0.9f + echo * 0.5f;
  return mono + lead + echo * 0.3f;
}

void MusicSynth::render_add(std::vector<f32>& out, u32 frames) {
  peak = 0;
  if (!rate_) return;
  for (u32 i = 0; i < frames; ++i) {
    // Crossfade: a different programme fades the current one out (0.4 s), then starts.
    if (want_ != programme_) {
      fade_ -= 1.0f / (0.4f * static_cast<f32>(rate_));
      if (fade_ <= 0 || programme_ == -2) {
        programme_ = want_;
        fade_ = 0;
        step_ = 0;
        until_step_ = 0;
        if (programme_ != -2) {
          const Prog p = prog_of(programme_);
          rng_ = p.seed * 2654435761u + 1;
          samples_per_step_ = std::max<u32>(1, static_cast<u32>(static_cast<f32>(rate_) * 60 / p.bpm / 4));
        }
        kick_ = snare_ = hat_ = bass_ = lead_ = Voice{};
        for (Voice& v : pad_) v = Voice{};
      }
    } else if (fade_ < 1) fade_ = std::fmin(1.0f, fade_ + 1.0f / (0.6f * static_cast<f32>(rate_)));
    gain_ += (target_ - gain_) * 0.0005f;
    if (programme_ == -2 || gain_ < 1e-4f) continue;
    if (until_step_ == 0) { step(); until_step_ = samples_per_step_; }
    --until_step_;
    f32 r = 0;
    const f32 l = next_sample(r);
    const f32 g = gain_ * fade_ * 0.7f;
    out[2 * i] += l * g;
    out[2 * i + 1] += r * g;
    peak = std::fmax(peak, std::fabs(l * g));
  }
}

}  // namespace sc
