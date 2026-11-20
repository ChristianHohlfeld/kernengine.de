// Port of SpawnCity's combat_audio_bank.mjs renderers (see combat_audio.h). Same formulas, same
// seeds (xorshift32), computed in double like the JavaScript.
#include "combat_audio.h"

#include <cmath>

namespace sc::combat_audio {

namespace {
constexpr double TAU = 6.283185307179586;
using Buf = std::vector<double>;

struct Random {
  u32 s;
  explicit Random(u32 seed) : s(seed ? seed : 1) {}
  double operator()() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return static_cast<double>(s) / 4294967296.0; }
};
double envelope(double t, double attack, double release, double duration) {
  return std::fmin(1.0, t / std::fmax(0.001, attack)) * std::fmin(1.0, (duration - t) / std::fmax(0.001, release));
}
std::vector<f32> finish(Buf& d, double target) {
  double peak = 1e-6;
  for (double v : d) peak = std::fmax(peak, std::fabs(v));
  const double gain = target / peak;
  std::vector<f32> out(d.size());
  for (std::size_t i = 0; i < d.size(); ++i) out[i] = static_cast<f32>(std::tanh(d[i] * gain * 1.35));  // softClip
  return out;
}
std::size_t samples(double duration, u32 rate) { return static_cast<std::size_t>(std::ceil(duration * rate)); }
double nwave_at(double t, double rise, double fall) {
  if (t < rise) return t / rise;
  if (t < rise + fall) return 1 - 1.6 * (t - rise) / fall;
  if (t < rise + fall * 1.9) return -0.6 * (1 - (t - rise - fall) / (fall * 0.9));
  return 0;
}

std::vector<f32> shot(u32 rate, u32 v) {
  const double duration = 0.2 + v * 0.01;
  Buf data(samples(duration, rate)), dry(data.size());
  Random random(0x5a1c0d + v * 4049);
  double lpA = 0, lpB = 0, phase = 0;
  const double rise = 0.00024, fall = 0.0009 + v * 0.00005;
  for (std::size_t n = 0; n < data.size(); ++n) {
    const double t = static_cast<double>(n) / rate, white = random() * 2 - 1;
    const double nwave = nwave_at(t, rise, fall);
    lpA += 0.6 * (white - lpA);
    lpB += 0.25 * (lpA - lpB);
    const double blast = ((lpA - lpB) * 0.9 + (white - lpA) * 0.5) * std::exp(-t / 0.008);
    phase += TAU * (185 * std::exp(-t * 30) + 70) / rate;
    const double thump = std::sin(phase) * std::exp(-t / 0.02) * (1 - std::exp(-t / 0.0008));
    const double u = t - 0.017 - v * 0.001;
    const double bolt = u > 0 && u < 0.012 ? (std::sin(TAU * 2700 * u) + 0.5 * (random() * 2 - 1)) * std::exp(-u / 0.003) * 0.25 : 0;
    dry[n] = nwave * 1.2 + blast * 0.9 + thump * 0.7 + bolt;
  }
  const double taps[3][2] = {{0.011, 0.32}, {0.019, 0.22}, {0.031, 0.14}};
  for (const auto& tap : taps) {
    const std::size_t k = static_cast<std::size_t>((tap[0] + v * 0.0006) * rate);
    for (std::size_t n = data.size(); n-- > k;) data[n] += dry[n - k] * tap[1];
  }
  double lp = 0;
  for (std::size_t n = 0; n < data.size(); ++n) { lp += 0.5 * (data[n] - lp); data[n] = std::tanh((dry[n] + lp * 0.8) * 2.1); }
  return finish(data, 0.95);
}

std::vector<f32> pistol(u32 rate, u32 v) {
  const double duration = 0.58 + v * 0.012;
  Buf data(samples(duration, rate)), dry(data.size());
  Random random(0x61c0a7 + v * 7919);
  const double rise = 0.00028 + v * 0.00002, fall = 0.00105 + v * 0.00006;
  double hp = 0, lpA = 0, lpB = 0, tail = 0, phase = 0, ring2 = 0;
  const double mech[3][4] = {{0.0215 + v * 0.0011, 2450, 3810, 1}, {0.0452 + v * 0.0017, 2080, 3260, 0.8}, {0.0705 + v * 0.0013, 2900, 4400, 0.35}};
  for (std::size_t n = 0; n < data.size(); ++n) {
    const double t = static_cast<double>(n) / rate, white = random() * 2 - 1;
    const double nwave = nwave_at(t, rise, fall);
    lpA += 0.55 * (white - lpA);
    hp = white - lpA;
    lpB += 0.22 * (lpA - lpB);
    const double band = lpA - lpB, blast = (band * 0.9 + hp * 0.55) * std::exp(-t / 0.0105);
    const double f = 185 * std::exp(-t * 22) + 50;
    phase += TAU * f / rate;
    const double thump = std::sin(phase) * std::exp(-t / 0.042) * (1 - std::exp(-t / 0.0009));
    const double boom = std::sin(TAU * 44 * t) * std::exp(-t / 0.11) * (1 - std::exp(-t / 0.004));  // the low body you feel
    const double snap = t < 0.0018 ? (random() * 2 - 1) * (1 - t / 0.0018) : 0;                     // the bright first snap
    double m = 0;
    for (const auto& c : mech) {
      const double u = t - c[0];
      if (u >= 0 && u < 0.02) m += c[3] * (std::sin(TAU * c[1] * u) + 0.6 * std::sin(TAU * c[2] * u) + 0.4 * (random() * 2 - 1)) * std::exp(-u / 0.0042);
    }
    dry[n] = nwave * 1.3 + snap * 0.7 + blast * 1.0 + thump * 1.15 + boom * 0.38 + m * 0.36;
  }
  // reflections + street tail
  const double taps[5][2] = {{0.0142, 0.42}, {0.0235, 0.30}, {0.0371, 0.24}, {0.0516, 0.17}, {0.0784, 0.11}};
  std::size_t d[5];
  for (int i = 0; i < 5; ++i) d[i] = static_cast<std::size_t>((taps[i][0] + v * 0.0007) * rate);
  for (std::size_t n = 0; n < data.size(); ++n) {
    const double t = static_cast<double>(n) / rate;
    double val = dry[n];
    for (int i = 0; i < 5; ++i)
      if (n >= d[i]) val += taps[i][1] * dry[n - d[i]] * 0.8;
    ring2 += 0.08 * ((random() * 2 - 1) - ring2);
    tail += 0.15 * (ring2 - tail);
    const double diffuse = t > 0.012 ? tail * 0.42 * std::exp(-(t - 0.012) / 0.16) : 0;
    data[n] = val + diffuse;
  }
  // low-pass the reflections a bit (walls absorb highs) while keeping the dry crack
  double lp = 0;
  for (std::size_t n = 0; n < data.size(); ++n) { lp += 0.45 * (data[n] - lp); data[n] = dry[n] + (lp - dry[n] * 0.45); }
  for (double& x : data) x = std::tanh(x * 2.6);
  return finish(data, 0.99);
}

std::vector<f32> hit(u32 rate, u32 v) {
  const double duration = 0.105 + v * 0.009;
  Buf data(samples(duration, rate));
  Random random(0x91cdef + v * 733);
  const std::size_t delay = static_cast<std::size_t>(rate * (0.008 + v * 0.001));
  double low = 0, phase = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    low += 0.20 * (white - low);
    const double surface = (white * 0.35 + low * 0.65) * std::exp(-t * (37.0 - v * 2.0));
    const double frequency = (112 + v * 9) * std::exp(-t * 13) + 46;
    phase += TAU * frequency / rate;
    const double thud = std::sin(phase) * std::exp(-t * 30), slap = i >= delay ? data[i - delay] * 0.16 : 0;
    data[i] = surface * 0.78 + thud * 0.58 + slap;
  }
  return finish(data, 0.82);
}

std::vector<f32> crack(u32 rate, u32 v) {
  const double duration = 0.082 + v * 0.006;
  Buf data(samples(duration, rate));
  Random random(0xc12ac7 + v * 991);
  double low = 0, phase = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    low += 0.08 * (white - low);
    const double snap = (white - low) * std::exp(-t * (92.0 - v * 4.0));
    const double frequency = (760 + v * 85) * std::exp(-t * 24) + (135 + v * 13);
    phase += TAU * frequency / rate;
    const double wood = (std::sin(phase) + 0.28 * std::sin(phase * 2.07 + 0.4)) * std::exp(-t * (42.0 - v * 2.0));
    data[i] = snap * 0.82 + wood * 0.52;
  }
  return finish(data, 0.84);
}

std::vector<f32> damage(u32 rate, u32 v) {
  const double duration = 0.21 + v * 0.035;
  Buf data(samples(duration, rate));
  Random random(0x7ad0b1 + v * 811);
  double low = 0, phase = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    low += 0.065 * (white - low);
    const double frequency = (92 + v * 12) * std::exp(-t * 8) + 31;
    phase += TAU * frequency / rate;
    const double pressure = std::sin(phase + 0.3 * std::sin(phase * 0.5)) * std::exp(-t * 13), rush = low * std::exp(-t * 8);
    data[i] = pressure * 0.70 + rush * 0.76;
  }
  return finish(data, 0.86);
}

double gaussian(double value, double center, double width) { const double x = (value - center) / width; return std::exp(-0.5 * x * x); }

std::vector<f32> scream(u32 rate, u32 v) {
  const double duration = 0.56 + v * 0.055;
  Buf data(samples(duration, rate));
  Random random(0xc0ffee + v * 1297);
  const double bases[4] = {238, 276, 218, 258}, formants[4][3] = {{760, 1280, 2480}, {870, 1430, 2670}, {690, 1130, 2320}, {820, 1510, 2580}};
  const double base = bases[v % 4];
  const u32 which = v % 4;  // (its formants: formants[which])
  double phase = 0, breath = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, p = t / duration, rise = std::sin(std::fmin(1.0, p / 0.34) * 3.141592653589793 / 2);
    const double fall = std::fmax(0.0, (p - 0.42) / 0.58);
    const double f0 = base * (0.82 + 0.34 * rise - 0.35 * fall) * (1 + 0.018 * std::sin(TAU * (5.1 + v * 0.35) * t));
    phase += TAU * f0 / rate;
    double voice = 0;
    for (int h = 1; h <= 12; ++h) {
      const double hz = f0 * h, shape = 0.18 + 1.35 * gaussian(hz, formants[which][0], 260) + 1.02 * gaussian(hz, formants[which][1], 390) + 0.55 * gaussian(hz, formants[which][2], 620);
      voice += std::sin(phase * h + h * 0.31 * v) * shape / std::pow(static_cast<double>(h), 0.82);
    }
    const double white = random() * 2 - 1;
    breath += 0.14 * (white - breath);
    const double air = (white - breath) * (0.18 + 0.22 * rise), amp = envelope(t, 0.024, 0.15, duration) * (0.72 + 0.28 * std::sin(3.141592653589793 * p));
    data[i] = (voice * 0.30 + air) * amp;
  }
  return finish(data, 0.90);
}

std::vector<f32> explosion(u32 rate, u32 v) {
  const double duration = 0.76 + v * 0.08;
  Buf data(samples(duration, rate));
  Random random(0xb0057 + v * 1901);
  double rumble = 0, phase = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    rumble += 0.035 * (white - rumble);
    const double crk = (white - rumble) * std::exp(-t * 24), frequency = (88 + v * 13) * std::exp(-t * 5.7) + 22;
    phase += TAU * frequency / rate;
    const double boom = std::sin(phase + 0.45 * std::sin(phase * 0.37)) * std::exp(-t * 4.7), tail = rumble * std::exp(-t * 3.1);
    data[i] = crk * 0.50 + boom * 0.82 + tail * 1.1;
  }
  return finish(data, 0.94);
}

std::vector<f32> step(u32 rate, u32 v) {
  const double duration = 0.075 + v * 0.008;
  Buf data(samples(duration, rate));
  Random random(0x57e9 + v * 431);
  double low = 0, phase = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    low += 0.09 * (white - low);
    phase += TAU * (78 + v * 9) * std::exp(-t * 12) / rate;
    data[i] = (low * 0.92 + std::sin(phase) * 0.36) * std::exp(-t * 38);
  }
  return finish(data, 0.72);
}

std::vector<f32> bounce(u32 rate, u32 v) {
  // A 40 mm grenade hitting the ground: a hard, dry "dock", short hollow knock, a little ring.
  const double duration = 0.16 + v * 0.01;
  Buf data(samples(duration, rate));
  Random random(0xd0c4 + v * 977);
  double lp = 0, phase = 0, ring = 0;
  const double f0 = 150 + v * 18, fr = 1850 + v * 240;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double t = static_cast<double>(i) / rate, white = random() * 2 - 1;
    lp += 0.35 * (white - lp);
    const double click = (white - lp) * std::exp(-t / 0.0022);
    phase += TAU * (f0 * (1 + 0.6 * std::exp(-t * 90))) / rate;
    const double knock = std::sin(phase) * std::exp(-t / 0.026) * (1 - std::exp(-t / 0.0006));
    ring += TAU * fr / rate;
    const double shell = std::sin(ring) * std::exp(-t / 0.012) * 0.35;
    const double grit = lp * std::exp(-t / 0.05) * 0.18 * (random() < 0.04 ? 2.5 : 1);
    data[i] = click * 0.9 + knock * 1.1 + shell + grit;
  }
  return finish(data, 0.82);
}

// Reward / fail cues in E-flat minor at ~176 BPM, 16ths, a filtered pulse over a sine sub and a
// dotted-8th echo (renderCue).
constexpr double kSixteenth = 60.0 / 176 / 4;
double note_hz(int n) { return 440 * std::pow(2.0, (n - 69) / 12.0); }
std::vector<f32> cue(u32 rate, std::vector<int> notes, std::vector<double> lengths, double cutoff, double echo, double level) {
  double total = 0;
  for (double l : lengths) total += l;
  total *= kSixteenth;
  Buf dry(samples(total + kSixteenth * 6, rate));
  double t0 = 0;
  for (std::size_t n = 0; n < notes.size(); ++n) {
    const double len = lengths[n] * kSixteenth;
    if (notes[n] < 0) { t0 += len; continue; }  // a rest
    const double f = note_hz(notes[n]), dur = len * 1.6;
    const std::size_t start = static_cast<std::size_t>(t0 * rate), count = static_cast<std::size_t>(dur * rate);
    double phase = 0, sub = 0, lp = 0;
    for (std::size_t i = 0; i < count && start + i < dry.size(); ++i) {
      const double t = static_cast<double>(i) / rate, env = std::fmin(1.0, t / 0.006) * std::exp(-t / (len * 0.75));
      phase += f / rate;
      sub += f * 0.5 / rate;
      const double pulse = std::fmod(phase, 1.0) < 0.5 ? 1 : -1, alpha = std::fmin(1.0, TAU * cutoff * (1 - 0.5 * t / dur) / rate);
      lp += alpha * (pulse - lp);
      dry[start + i] += (lp * 0.55 + std::sin(TAU * sub) * 0.28) * env;
    }
    t0 += len;
  }
  Buf data(dry.size());
  const std::size_t delay = static_cast<std::size_t>(kSixteenth * 3 * rate);
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double e1 = i >= delay ? dry[i - delay] * echo : 0, e2 = i >= 2 * delay ? dry[i - 2 * delay] * echo * echo : 0;
    data[i] = dry[i] + e1 + e2;
  }
  const std::size_t fade = static_cast<std::size_t>(0.04 * rate);
  for (std::size_t i = 0; i < fade && i < data.size(); ++i) data[data.size() - 1 - i] *= static_cast<double>(i) / fade;
  return finish(data, level);
}
}  // namespace

const char* kind_name(Kind k) {
  switch (k) {
    case Kind::shot: return "shot";
    case Kind::pistol: return "pistol";
    case Kind::hit: return "hit";
    case Kind::crack: return "crack";
    case Kind::damage: return "damage";
    case Kind::scream: return "scream";
    case Kind::explosion: return "explosion";
    case Kind::step: return "step";
    case Kind::bounce: return "bounce";
    case Kind::reward: return "reward";
    case Kind::fail: return "fail";
    default: return "";
  }
}

std::vector<f32> render(Kind k, u32 v, u32 rate) {
  rate = rate < 8000 ? 8000 : rate;
  switch (k) {
    case Kind::shot: return shot(rate, v);
    case Kind::pistol: return pistol(rate, v);
    case Kind::hit: return hit(rate, v);
    case Kind::crack: return crack(rate, v);
    case Kind::damage: return damage(rate, v);
    case Kind::scream: return scream(rate, v);
    case Kind::explosion: return explosion(rate, v);
    case Kind::step: return step(rate, v);
    case Kind::bounce: return bounce(rate, v);
    case Kind::reward:
      if (v % 3 == 0) return cue(rate, {70, 75}, {1, 2}, 2400, 0.3, 0.75);                          // B-flat to E-flat: clean confirm
      if (v % 3 == 1) return cue(rate, {63, 66, 70, 75}, {1, 1, 1, 2}, 2800, 0.3, 0.75);            // E-flat minor arpeggio: hit / kill
      return cue(rate, {66, 70, 73, 78, -1, 82}, {1, 1, 1, 1, 1, 3}, 3200, 0.36, 0.75);              // streak
    case Kind::fail:
      if (v % 2 == 0) return cue(rate, {70, 69, 66}, {1, 1, 3}, 1400, 0.22, 0.7);                    // denied
      return cue(rate, {63, 58, 51}, {1, 1, 4}, 1100, 0.2, 0.72);                                    // falling octaves: lost
    default: return {};
  }
}

}  // namespace sc::combat_audio
