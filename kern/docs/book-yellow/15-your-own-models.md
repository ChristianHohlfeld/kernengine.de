# 15 Training Your Own Models – Your Network, Your Duck

> **Your level:** Now you train for yourself. You'll learn how to start the Lehrer, read its
> progress, pick the best network and make it jump in Studio – and how to add a new motion.

This is the how-to; how the Lehrer computes is in [Chapter 14](14-lehrer.md), the foundations in
[Appendix A](A-neural-networks.md). There are two ways: the **Lehrer page in the browser** (jump and
stand only) and the **tool** `kern_lehrer` for everyone who builds Kern from source
([Chapter 2](02-building-and-testing.md)).

> **Mission:** From https://kernengine.de/, open the Lehrer, choose "Sprung" ("jump") and start.
> Leave the tab open (plug a phone into its charger). Watch "echte Sprünge (sauberer Roboter)"
> ("real jumps, clean robot"): from when on does your network jump?

## 15.1 Building the Tool

```
kern/tools/lehrer.sh         # builds kern/build/kern_lehrer
kern/tools/lehrer.sh /path/out # builds into /path/out
```

The script runs on Linux and macOS (x86 and ARM), compiles Box3D, Kern's math library and the sources
except Studio and App with the switches for determinism, and only rebuilds what has changed.
`CC`/`CXX` choose the compiler, `KERN_JOBS` the number of parallel compilers, and `KERN_NATIVE=0`
turns off the optimization for your processor – it changes **no result**. The tool loads Pollen's
networks from `third_party/microduck/` (or `--policies DIR`).

## 15.2 All Commands

| Command | Purpose |
|---|---|
| `--task jump` / `stand` | train a motion with PPO |
| `--eval net.onnx` | measure the jump, clean and varied |
| `--trace …` | one episode tick by tick |
| `--vorbild-suchen demo.json` | search for a jump demonstration with CMA-ES |
| `--vorbild-pruefen …` | let a demonstration jump |
| `--spielen` | imitate the play layer (DAgger) |
| `--spielen-pruefen …` | let a demonstration or network play |
| `--spielen-ueben play.onnx` | practice the play layer with PPO |
| `--spielen-spur` | one play episode step by step |
| `--biss-fenster` | where the beak grips a sock |
| `--halt-messen` | how far the duck walks after "stop" |
| `--demo-spur` | the example level as in Studio |
| `--fingerprint 400` | determinism fingerprint |
| `--kalibrieren` etc. | calibration ([Chapter 16](16-calibration.md)) |

The options keep their German names: *vorbild suchen/prüfen* = search/check demonstration,
*spielen* = play, *üben* = practice, *Spur* = trace, *Biss-Fenster* = bite window,
*Halt messen* = measure stopping. For all of them: `--threads N` (default: all cores) and
`--seed N` (default 1).

## 15.3 Training the Jump

### The Standard Run

```
kern/build/kern_lehrer --task jump --iterations 3000 \
  --out jump.onnx --log jump.csv \
  --checkpoint run.ckpt --keep-every 250
```

This is how Kern's `jump.onnx` was made (32 ducks on 2 cores, seed 1). What happens:

1. **Load the recipe:** warm start from velstand, imitation (8 rounds of 96 episodes each), practice
   with noise 0.1 and learning rate 10⁻⁴. Options override individual values.
2. **Create the ducks:** `--envs` (16 per thread), rounded down to a multiple of the threads.
3. **Imitate,** then write `jump_nachgeahmt.onnx` ("imitated") and `run.ckpt.nachgeahmt`.
4. **Practice:** 30 iterations of critic only; every 10 iterations (`--eval-every`), 5 test jumps.
5. **Save:** every 25 iterations (`--save-every`), at the end, and every 250 as a snapshot
   (`jump_it00250.onnx`).

With at least 4 of 5 real test jumps, `FIRST REAL JUMP at iteration …` appears and the network is
saved; `--stop-after-jump` ends the run there.

### Resuming, Without a Demonstration, Your Own Demonstration

```
kern_lehrer --task jump --iterations 6000 \
  --resume run.ckpt --checkpoint run.ckpt
```

`--resume` loads the whole learner and skips imitation; `--iterations` is the **target**. With
`run.ckpt.nachgeahmt`, practice starts over.

`--vorbild none` is pure PPO (with the term `track_height`) – this route didn't find the take-off.
For experimenting: `--vorbild demo.json`, `--nachahmen 5 --episoden 64`, `--uebungs-std 0.2`,
`--nachahmungs-lohn 0`. You search for your own demonstration with
`--vorbild-suchen demo.json --iterations 120` (continue with `--vorbild demo.json`) and check it with
`--vorbild-pruefen`. Each generation reports "a real jump", "no real jump" or "fell".

The Lehrer page trains the same recipe: the same seed and the same total number of ducks (workers ×
ducks per worker) give the same network as on the computer.

## 15.4 Reading Progress

Each practice iteration writes a line with these fields:

| Field | Meaning | What to watch for |
|---|---|---|
| `return` | mean return | rises slowly |
| `len` | episode length in ticks | 100 = 2 s without a fall |
| `value` | the critic's error | high at first, then falling |
| `std` | noise σ | drops as the network gets surer |
| `lr` | learning rate | stuck at 1e-5: steps too big |
| `steps/s` | duck steps per second | your device's speed |
| `jumps n/5` | jump test | the actual goal |

Imitation reports examples (`samples`) and error (`error`) per round. With `--log jump.csv`, every
iteration lands in a CSV file for your spreadsheet (`jumps` = −1: not measured). If something goes
wrong, `--trace jump.onnx` shows an episode tick by tick with every reward term (`builtin` instead of
a file: the demonstration; `--randomized`: with variation). That way you can see whether the network
jumps – or just tricks the reward.

## 15.5 Picking the Best Network

The last network isn't always the best one. So:

1. Keep snapshots with `--keep-every`.
2. Measure each one with more episodes, clean **and** varied:

   ```
   for f in jump_it*.onnx; do
     kern_lehrer --eval "$f" --episodes 40
   done
   ```

3. Choose by: real jumps with variation, then falls (0), then rise, flight time, tilt.
4. Check it in Studio (15.8).

For Kern's `jump.onnx`, this gave 40 of 40 clean and 36 of 40 varied, without a fall.

## 15.6 Choosing Hyperparameters

**Hyperparameters** are the settings before learning. One iteration with 32 ducks: 32 × 48 = 1536
rows (noise 0.1), split into 4 minibatches of 384 rows (each in 8 pieces of 48), 5 epochs × 4 =
20 Adam steps, each preceded by the learning-rate check. During imitation, 8 × 96 × 100 = 76,800
examples would be possible; there were 49,936, because an episode ends early as soon as the duck
falls or a part other than the soles touches the ground.

| Option | Jump recipe | Effect |
|---|---|---|
| `--envs N` | 16 per thread | more data; **changes the network** |
| `--threads N` | all cores | speed (with fixed `--envs`) |
| `--steps N` | 48 | ticks per duck and iteration |
| `--epochs`, `--minibatches` | 5, 4 | learning steps per iteration |
| `--lr X` | 10⁻⁴ | starting learning rate |
| `--kl X` | 0.01 | size of each step |
| `--init-std X` | 0.3 | noise before imitation |
| `--uebungs-std X` | 0.1 | noise during practice |
| `--entropy X` | 0.005 | slows the drop of σ |
| `--actor`, `--critic` | 512,256,128 / 256,128 | network sizes |
| `--warm-start` | velstand | or `none` |
| `--substeps N` | 16 | don't change! |
| `--profil servo.json` | XL330 | calibrated servos |

Plus `--adaptive-lr 0|1`. With a warm start, the actor has to match velstand. Without `--envs`,
"16 per thread" applies: if you then change the threads, you also change the number of ducks – and
with it the network. `--warm-start` only knows `velstand`; any other value starts with random
weights. A warm start from a file only exists when practicing the play layer. Tips from real runs:
be careful after imitation; more noise doesn't help with discovering; few ducks slow things down;
with 8 instead of 16 sub-steps, the network learns a different physics.

> **Picture it like this:** The learning rate is like your stride length when you're walking in the
> dark. Too short, and you never get there. Too long, and you trip over the curb.

## 15.7 Training the Play Layer

This only works with the tool, in six steps:

1. **Measure:** `--biss-fenster` shows on a grid of 11 × 9 spots where the beak holds a sock, plus
   the best spot (training measures this by itself). `--halt-messen` shows how far the duck keeps
   going after "stop".
2. **Imitate:**

   ```
   kern_lehrer --spielen --nachahmen 12 \
     --episoden 32 --out play.onnx
   ```

   Default: 8 rounds of 64 episodes; every round measures the network in 16 clean episodes.
3. **Check:** `--spielen-pruefen vorbild` or `play.onnx` counts kicks, pick-ups, spit-outs, grips
   and falls, clean and varied. The demonstration shows the most you can get.
4. **Watch:** `--spielen-spur --seed 3` shows every decision (25 per second); with `--netz`
   ("network"), the network drives, and you see where it differs from the demonstration.
5. **Practice:** `--spielen-ueben play.onnx --iterations 300 --envs 32`: noise 0.1, first
   20 iterations of critic only, a snapshot every 20 iterations.
6. **Check in the level:** `--demo-spur --netz play_geuebt.onnx --seed 120` plays the Micro Duck
   level as in Studio, here for 120 s – for this command, `--seed` is the duration in seconds! With
   `KERN_JUMP_AT=SECOND`, it presses key 7 at that point.

> **Watch out, trap:** `--out` is overwritten after **every** round. If you want to keep an earlier
> round, copy the file during the run. For the old `play.onnx`, round 10 of 12 was the best.

## 15.8 Using It in Studio

Kern's networks have the fixed names `jump.onnx` and `play.onnx`. This is how yours gets into Studio:

1. **Into the repository:** copy the file to `kern/policies/`. A finished page loads from
   `policies/` next to the page.
2. **From the browser Lehrer:** It saves its network on the same device. Studio loads it on startup
   and picks up every new one right away, even in open tabs. This only applies to the jump.
3. **Check:** open the Micro Duck level, Play, key **7**. For the play layer, leave the duck alone,
   with Autonomous and Kern extras on and a ball or sock in view.

The network needs the shape of an rsl_rl actor; a play network without 23 inputs and 9 outputs isn't
computed.

> **Mission:** Did you train a jump in the browser Lehrer? On the same device, open Kern Studio,
> load File › Example Levels › Micro Duck (if Play is running, stop it with Esc first), start Play,
> press F8 and then **7**. Open the browser console: it says "Kern: trained network jump.onnx
> loaded". Now **your** network is jumping.

> **Mission:** In the Micro Duck level, click the duck and look under Details › Player Controller ›
> Walker for **Autonomous** and **Kern extras**. Start Play once with and once without Kern extras
> (to switch, stop with Esc first). What does the duck do differently with the ball and sock?

## 15.9 Adding a New Task

A new motion (61 → 14) needs four things, often plus a demonstration.

**1. The task:** in `src/lehrer/`, derive from `Task`. The start of an example "bow":

```cpp
class BowTask : public Task {
 public:
  std::string name() const override { return "bow"; }
  f32 episode_seconds() const override { return 2.0f; }
  i32 privileged() const override { return 4; }
```

Line 1: `BowTask` is a `Task`. Lines 3–5: name, 2 s duration, 4 extra values for the critic. Then
come the start, phase signal (slots 55/56), curricula and reward with helpers like `upright` and
`fell`. The full skeleton is in the blue book.

**2. Enter the recipe and name** in `recipe.cpp` (for example warm start velstand, learning rate
10⁻⁴). Then the tool and the browser know the task; the Lehrer page is only missing a menu entry. If
the task uses slots velstand has never seen, they have to count as new inputs for the warm start;
right now Kern only does that for the phase slots 55 and 56.

**3. Measure** like the `JumpMeter`: check what the motion **is**, not what the reward counts.

**4. A demonstration (optional).** If PPO doesn't find the motion: a few numbers that describe it, a
score for CMA-ES, and a demonstration that has an answer in **every** state (for DAgger).
`cmaes_minimise` and `Ppo::imitate` are general; the rest is tailored to the jump.

**5. Wire it up in Studio** like the jump: an entry in the motion list, a key, a touch button, the
name in the logic node **Robot Move**, the file name in `game.mjs`, the file in `kern/policies/`. The
play layer shows how a network with a **different** format works: its own environment, its own
learner, its own evaluation – PPO and imitation stay the same.

## 15.10 Designing Observation, Action and Reward

- **Only what the robot has:** sensors for the actor, the truth for the critic.
- **Make time visible:** phase signal (cos, sin), standing and walking time.
- **Measure instead of command:** "standing" means what the legs are doing.
- **Actions like controls:** a dead zone like on a stick (below 20 %), a motion starts above 0.5.
- **Named terms**, penalties grow through curricula.
- **A hard end** on a fall and wrong ground contact.
- **A demonstration without secret knowledge;** requests rise as a ramp.

## 15.11 Pitfalls from the Real Runs

- **Reward tricked:** The duck sat on its tail or propped itself up. Only stricter rules helped.
- **Measurement too lax:** A "jump" with 45° of tipping got through. Now only a real one counts.
- **Rare motions:** PPO alone doesn't find them; a demonstration and imitation do.
- **A clueless critic** wrecks a good actor – that's why it's critic first.
- **Drifting normalizers:** The old policy gets re-evaluated.
- **Bugs in the world:** The sock crept, the ball rolled forever, yawning opened the beak. If a
  network fails to learn something, it may be pointing at a bug like that.
- **Hand-entered numbers go stale:** The bite spot is measured on every run.
- **Breaking determinism** with `std::sin` or sums in thread order; `--fingerprint 400` shows it.
- **Different physics from Studio** means: the network learns a different world.

## Level complete!

- You can train a jump in the browser or with `kern_lehrer`.
- You can read `return`, `len`, `std` and the jump test.
- You can pick the best network, measured clean and varied.
- You know what changes the network and what only changes the speed.
- You can get your network into Studio.
- You know the steps for a new motion.

## Puzzle corner

1. Which key starts the jump in Studio?
2. Does `--threads 1` instead of `--threads 8` change the network? And `--envs`?
3. Your run keeps showing `lr 1e-05`. What does that mean?
4. **Boss puzzle:** With `--demo-spur --seed 120` – how long does the level run, and why is that a
   trap?

### Solutions

1. The **7** (on a phone, the HOP button).
2. It depends: with a fixed `--envs`, threads only change the speed. Without `--envs`, 16 ducks per
   thread applies – 16 instead of 128 ducks, so different data and a different network. `--envs`
   always changes it.
3. The policy changes too much per step; the rule pushes the learning rate all the way down.
4. 120 seconds: only here does `--seed` mean the duration, not the random start.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
