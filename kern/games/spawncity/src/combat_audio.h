// SpawnCity's combat sound bank (combat_audio_bank.mjs, prebaked-pcm-buffer-bank-v4): every combat
// sound is synthesised once at start into mono PCM - no recordings. Kinds and variants as there:
// shot (MP, 3), pistol (hard 9 mm report, 4), hit (4), crack (headshot click, 4), damage (2),
// scream (4), explosion (2), step (3), bounce (40 mm grenade on the ground, 4), reward (3),
// fail (2). The game registers them with Kern's SoundBank as "sc:<kind><variant>".
#pragma once
#include <vector>

#include "common.h"

namespace sc::combat_audio {

enum class Kind : u8 { shot, pistol, hit, crack, damage, scream, explosion, step, bounce, reward, fail, count };
constexpr u32 kSampleRate = 44100;
constexpr u32 kVariants[] = {3, 4, 4, 4, 2, 4, 2, 3, 4, 3, 2};
const char* kind_name(Kind k);
/// The PCM of one variant (mono, kSampleRate, normalised and soft-clipped as in SpawnCity).
std::vector<f32> render(Kind k, u32 variant, u32 rate = kSampleRate);

}  // namespace sc::combat_audio
