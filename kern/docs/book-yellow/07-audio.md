# 7 Audio – Kern Turns Up the Volume

> **Your level:** A game without sound is a silent movie. Here you'll get to know a complete
> recording studio: a mixing desk for 3D sound, a synthesizer that builds every noise out of math,
> and the voice of the Micro Duck.

Kern has its own **sound engine** in C++, inside the same WebAssembly program as graphics and
physics. The browser is just the loudspeaker and unpacks imported files.

Sound is trembling air. The computer measures the wave very many times per second, and each
measurement is a **sample**. How often it measures is called the **sample rate**: 48,000 Hz means
48,000 numbers per second for each ear. A row of numbers like that is called **PCM**.

## 7.1 Why a mixer of its own?

- **Always the same:** The same events produce the same samples on every device
  (*deterministic*).
- **One code for everything:** Studio, exported games and tests all use the same C++ code.
- **Full control:** no clicking, no distortion – every sample stays between −1 and +1.

## 7.2 The mixer

> **Picture it like this:** a DJ desk. Every track has a fader, tracks flow together in groups,
> and at the very end the master fader sits in front of the speakers.

The mixer has a fixed supply of **voices** – sounds that are currently playing. Studio uses 64, and
128 are possible. The output is stereo. What gets played are **clips**: chunks of PCM, mono or
stereo, at most as big as 60 seconds of stereo at 384,000 Hz. If a file's rate doesn't match the
device, Kern converts it **once** while loading, with a clean, expensive filter (*windowed sinc*)
that prevents fake "mirror tones."

Every voice gets settings: volume (0 to 16), pitch (0.05 to 8), pan, loop, bus, fade-in, priority
and, for 3D sounds, position, speed, `min_distance` (1 m), `max_distance` (50 m), falloff curve,
width and Doppler. Nonsense values get trimmed. A voice's number contains a counter that goes up
every time a voice starts: an outdated number never hits somebody else's sound.

**All slots full?** Then Kern steals a voice with the same or lower priority: the least important
one, then the quietest, then the oldest. If all of them are more important, the new sound doesn't
start.

**Stopping** always fades out, for at least 5 ms – cut off hard, it **clicks**. Volume changes glide
too (30 ms). Pausing the game holds voices without losing their position.

**Mixing:** The mixer works in blocks of 128 samples. For each block it works out the left and right
volume of every voice and lets it glide smoothly across the block – otherwise it rasps. For each
sample it reads the clip; if the position lies between two samples, it lays a smooth curve through
four neighbors (*Hermite interpolation*). Then come a low-pass filter (7.3), adding into the bus, and
moving forward by "pitch × Doppler."

> **Crazy but true:** If a 3D voice is quieter than 0.00001, it becomes **virtual**: it isn't mixed,
> but its position keeps running. When you come back to the machine, it keeps humming at exactly the
> right spot.

> **Mission:** Open the Content Drawer (Ctrl+Space) in Kern Studio on https://kernengine.de/ and
> go to the **Sounds** tab. One click plays a sound. Listen to one from each category. You only get sound after
> your first click or key press.

## 7.3 3D sound

The **listener** sits at the camera. For every 3D voice, Kern asks: How loud? Which side? How
muffled? How high?

**Distance:** With the `inverse` curve, the volume is `min_distance` divided by the distance. With
1 m: at 2 m away it's 0.5, at 4 m 0.25, at 10 m 0.1. `linear` drops evenly to zero, `exponential`
more steeply than `inverse`. Over the last 20% before `max_distance`, Kern also fades out, so it's
really silent beyond that.

**Left or right:** The position of the source becomes a pan value from −1 to +1, weaker for close
and wide sources. Then **equal power** applies:

```
a     = (pan + 1) · π/4
left  = cos(a) · √2
right = sin(a) · √2
```

At pan 0, a = 45°: cos and sin ≈ 0.707, times √2 makes both sides 1. At pan +1, a = 90°: left 0,
right ≈ 1.41. Because cos² + sin² = 1 (Pythagoras in a circle), the sound is equally loud
everywhere. A linear crossfade would make it too quiet in the middle.

**More muffled:** High tones fade away in the air faster. A **low-pass filter** only lets tones
through up to a limit: in front of you 20,000 Hz, at 120 m only 10,000 Hz. Behind you it gets even
more muffled. That's the most important clue you use to tell front from back.

**Doppler:** An ambulance drives past – first higher, then lower.

```
f' = f · (c − v_listener) / (c − v_source)
```

`c` = 343 m/s. A car comes toward you at 34.3 m/s: f' = f · 343 / 308.7 ≈ 1.11 · f, so 11% higher.
Kern limits the factor to 0.5 to 2 and lets it glide.

> **Mission:** From Place Actors › Audio, place a **Radio**, start **Simulate** (Alt+S) and fly
> around holding the right mouse button and using W A S D. Can you hear distance and direction?

## 7.4 Buses, reverb and limiter

Every voice plays on a **bus**: `music`, `sfx` (effects), `ambience` (background), `ui` (interface)
or `voice` (voices). They all flow into the `master`.

```mermaid
flowchart LR
  S[Voices] --> B[Buses]
  B --> M[Master]
  B --> H[Reverb]
  H --> M
  M --> L[Limiter]
  L --> O[Browser]
```

Every bus has a volume, a mute (which glides, so it doesn't click), optionally a low-pass filter
and a reverb control. At the start: music 0.7, ambience 0.8, effects and voices a bit of reverb.
For each bus, Kern measures the peak and average level.

**Reverb:** Kern uses **Freeverb**, a free reverb recipe: per channel, eight **comb filters** –
echoes that feed back into themselves and get quieter, the highs faster – and after that four
**all-pass filters** that smear the rest. The right channel has slightly longer echoes; that makes
the reverb wide.

> **Picture it like this:** Clap in an empty gym. Echoes from all the walls melt into one reverb
> tail. The eight combs play those walls.

**Limiter:** No peak may go above 0.92. The limiter lowers the gain **instantly** and lets go again
in about 120 ms. A soft curve (`tanh`) rounds off the rest. Studio's status bar shows the mixer's
statistics (voices, clips, levels) as a **level meter**; the peak falls at 20 dB per second, like a
real meter needle.

> **Mission:** Use File › New Level... to create a **Basic** level, place a **Crate Stack** from
> Place Actors › Physics, and in **Simulate** throw crates around with the left mouse button. Watch
> the level meter at the bottom. Stop with Esc, set **Reverb** to 1 in **World Settings** › Audio
> and try it again.

## 7.5 The synthesizer

Kern ships **not a single sound file**. Every built-in sound is created from code while loading, in
a small synthesizer modeled on the retro tool sfxr, extended with resonant filters, noise colors, FM
and layers, plus a step sequencer for music. Fixed random seeds make everything the same
everywhere.

A sound is a **patch**, a list of knob settings: waveform (square, sawtooth, sine, triangle, noise,
pink noise, pulse), envelope (attack, hold, "punch," decay), pitch with slide, vibrato and jump,
pulse width, FM, low-pass and high-pass, tremolo, repeat, bitcrush.

- Noise is re-rolled in time with the tone frequency; low noise sounds darker.
- Slides, vibrato and filter sweeps are only recalculated every 16 samples. That saves time, and
  nobody can hear steps that short.
- Bitcrush holds values – that broken retro sound.
- Repeat gives you the power-up sound, a single pitch jump the pickup sound.
- Helper functions layer sounds and make **seamless loops** by softly fading the end over the
  beginning.

The recipe for `pickup` (shortened):

```cpp
SynthPatch a = patch(Wave::pulse, 988,
                     0.002f, 0.05f, 0.28f, 0.42f);
a.duty = 0.35f;
a.arp_ratio = 1.5f;
a.arp_time = 0.065f;
out = synthesize(a, rate);
```

A pulse tone at 988 Hz with four envelope times, pulse width 35%. After 65 ms it jumps to 1.5
times the frequency, a fifth higher. Then it gets computed; in the original, a bell is added too.

**Strikes:** For impacts of small solid things, Kern mixes a few quickly fading sine tones – the
body's **modes** – with a short burst of noise. Solid plastic sounds neither low nor long; otherwise
it would sound like a hollow box. That's why the tones of `plastic` sit at 760, 1650 and 3100 Hz.

**Music:** `music_drive` has 120 beats per minute, Am–F–C–G, kick drum, snare, bass, arpeggio and
pads. `music_calm` has 80 beats, calm chords and a bell melody. Both: 8 bars, stereo, seamless.
Because music needs a few hundred milliseconds of computing time, Kern computes it **in bite-sized
pieces** of about one bar each. That way loading doesn't stutter.

Built-in sounds are called `builtin:` plus the name (* = loop):

| Category | Sounds |
|---|---|
| Gameplay | pickup, powerup, jump, land, goal, hazard, checkpoint, lose, laser |
| Combat | gunshot, smg, pistol, sniper, launcher, reload, hit, down |
| Physics | explosion, impact, impact_light, break, step, plastic, plastic_step |
| Vehicle | engine*, hover*, boost |
| Ambience | wind*, city*, hum* |
| Robot | servo*, quack, beep |
| UI | click, confirm, error |
| Music | music_drive*, music_calm* |

## 7.6 The voice of the Micro Duck

The Micro Duck has a synthesizer of its own, ported from Pollen Robotics' original project (written
in Rust there). A single number, the **seed**, determines its sound: pitch, nasality, vibrato,
breath, "quackiness," tempo and more. The same seed always sounds the same. Like the robot, Kern
makes it from an identifier, using a **SHA-256** fingerprint. The robot uses its serial number; Kern
uses the actor's name and number.

A sound is made of seven overtones, a pitch curve with vibrato, fast volume flutter (the "quack")
and a bit of noise as breath. There are seven kinds of sounds: `alarm`, `greet`, `inquire`, `peck`,
`chirp`, `coo`, `wheee` – each with 6 to 12 variants, one of which is picked at random. `wheee` has a
start, a loop and an end, so it lasts as long as a ride.

> **Crazy but true:** The duck's voice is **bit-exact** like the original. If you compare all 328
> sounds with the Rust original, they have the same length and a difference of exactly 0.

Finally, a filter removes everything below 300 Hz – the robot's little speaker can't go that low.
In the game, every sound plays on the `voice` bus at the duck's **head**. A new sound cuts off the
old one – just like a single real speaker. During `wheee`, other sounds are skipped. Because one
sound costs a few tens of milliseconds of computing time on a phone, Kern computes them in advance,
one per frame. When the duck calls out what: [Chapter 13](13-micro-duck.md).

## 7.7 Sound in the game

In a level, a sound is named as text: `builtin:<name>`, the number of an imported sound (WAV, MP3,
OGG, M4A, FLAC; the file is stored inside the level), `none` for silence or `auto` for the default.

The **SoundBank** turns these into clips. It computes built-in sounds in bite-sized pieces, and the
browser unpacks imported ones. Before the start, it orders **everything** the level might need, so
that everything is ready when you hit Play.

Every frame, **GameAudio** copies the level settings onto the mixer (reverb goes to effects, voices
and ambience, never to music and interface) and then does this:

1. Take over the world's **pause**.
2. Put the **listener** at the camera. If the camera jumps faster than 200 m/s, it was a cut, and
   its speed counts as 0 – otherwise you'd get an absurd Doppler howl.
3. Start **music and ambience** as loops (priority 100).
4. The **player's engine** as a 3D loop (priority 90), **without Doppler**, because the camera rides
   along. More speed and throttle: louder and higher. For the duck, the servos whir.
5. **Audio Sources** follow their actors (loops priority 40, one-shots 20).
6. Play the **cues** (7.8). If a clip isn't ready after 0.25 s, the cue is dropped.

> **Watch out, trap:** A bang half a second after the flash looks broken. Too late is worse than
> never – that's why Kern throws away late cues.

Cues have priority 10 (50 on the `ui` bus) and therefore never push out music, engine or loops. If
more than 64 cues are waiting, the rest get cut off. The **speaker button** in Studio's status bar
mutes the master; it stays that way across Play and Stop too. With engine "auto," Hover Car and
Flyer get `hover`, Vehicle gets `engine`, the Micro Duck gets `servo`, and the others get none.

## 7.8 Cues and impact sounds

The simulation never plays sound itself. In the fixed physics steps it writes **cues**, little order
slips: sound, position, volume, bus, actor. That's why they're deterministic. In a network game,
sounds are sent without a guarantee, but important events like a collected pickup are sent
reliably ([Chapter 10](10-networking.md)). Without anyone listening (for example during training),
Kern keeps only the newest 64 cues.

Every Rigid Body has an **impact sound** (default `builtin:impact`) and a threshold of 2.5 m/s.
Every collider can have its own – the duck's sole makes `plastic_step`, its housing `plastic`. The
rules:

1. At most 6 impact sounds per physics step.
2. Below the threshold it stays silent.
3. **Debouncing:** A step sounds at most every 0.5 s, a knock every 0.09 s – a rolling sole bounces
   a tiny bit and would otherwise report lots of hits.
4. **Volume** = (v − threshold) / (5 · threshold) + 0.2, between 0.2 and 1. With a threshold of
   2.5 m/s and 10 m/s: 7.5 / 12.5 + 0.2 = 0.8.
5. Light bumps (below 2.2 times the threshold) play the gentler `impact_light`.
6. Heavy things sound lower, light things only a little higher – very high would sound like glass.

The Micro Duck has a threshold of 0.12 m/s. Breaking joints play `builtin:break`.

## 7.9 Level settings and Audio Source

**World Settings** › Audio holds Master volume, Music, Ambience with their volumes, Reverb (0.25)
and Doppler.

The **Audio Source** component turns any actor into a sound source:

| Field | Default | Meaning |
|---|---|---|
| Sound | `builtin:hum` | which sound |
| Volume, Pitch | 1, 1 | volume, speed |
| Loop, Play on start | on, on | repeat, start with Play |
| Spatial (3D) | on | off: no position |
| Inner radius | 2 m | full volume up to here |
| Falloff distance | 40 m | silent from here on |
| Pitch by speed | 0 | higher the faster it goes |
| Start delay | 0 s | waiting time |

Plus bus, roll-off and Doppler. Ready-made variants: Audio Source, Ambient Wind, City Ambience,
Radio ([Chapter 11](11-studio.md)).

> **Mission:** Place a **Physics Cube** (Place Actors › Physics) and keep it selected. In the
> **Sounds** tab, double-click `engine` – the box gets an Audio Source. Set **Pitch by speed** to 1,
> start Simulate and throw the box. The engine revs up with the speed!

## 7.10 The browser connection

**KernAudio** gets the numbers into the speaker. It gives the mixer the device's sample rate so
nothing has to be converted. On the iPhone it registers as playback, so the mute switch doesn't
silence games. **You only get sound after a gesture** – the first click, key press or tap. Every
20 ms a timer tops things up, even when the tab is in the background.

Preferably, sound runs through an **AudioWorklet**, a mini program just for audio. Kern sends it
chunks into a **queue** and keeps 70 ms in reserve at the start. If the queue runs dry, 30 ms are
added, up to 200 ms; after 15 s without dropouts, the reserve shrinks again. If it does run empty,
the sound glides to zero in 2 ms instead of crackling. Without a worklet, Kern uses the older
*ScriptProcessor*.

The browser unpacks **imported files** (`decodeAudioData`) and hands the numbers to the engine. If a
file has more than two channels, it's mixed down to stereo first. If unpacking fails, KernAudio
reports the error, and cues with that sound are thrown away instead of waiting forever.

> **Picture it like this:** a water canister with a tap. At the top, the game keeps pouring in; at
> the bottom, it flows out evenly. If it ever ran dry, you fill in more ahead of time from then on.

## 7.11 Tips for your games

- Built-in sounds cost no download; your own files are worth it for music.
- Use the buses properly and set `max_distance` honestly.
- Give continuous sounds high priority and short effects low priority.
- Impact thresholds that are too low make resting bodies knock constantly.

> **For pros:** The mixer doesn't depend on the game runtime. In your own C++ program
> ([Chapter 12](12-your-own-games.md)) you use the mixer and synthesizer directly, but you have to
> hook up a browser output like KernAudio yourself. If you want to know every detail: the blue book.

## Level complete!

- You know what samples, sample rate and PCM are.
- You understand voices, priorities and voice stealing.
- You can calculate volume, panning and Doppler.
- You know buses, reverb and the limiter.
- You know that all sounds and the duck's voice are made from math.
- You know why the browser only makes sound after a click.

## Puzzle corner

1. How many voices does Studio use, and how many would be possible?
2. `inverse` curve, `min_distance` 2 m, distance 8 m: How loud is the source?
3. Why does the player's engine have no Doppler?
4. 30 crates hit the ground in the same physics step. What's the maximum number of impact sounds?
5. **Boss puzzle:** A source moves away from you at 68.6 m/s. By what percentage does the pitch
   change? (Hint: when moving away, c + 68.6 goes in the denominator.)

### Solutions

1. 64, at most 128.
2. 2 / 8 = 0.25 – a quarter.
3. The camera rides along. Listener and engine move together, so a Doppler would sound wrong.
4. Six. The mixer would steal any more anyway.
5. 343 / 411.6 ≈ 0.83. The sound gets about 17% lower.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
