# 15 Training Your Own Models – the Workshop

> **In short:** This is where it gets practical. You build the tool `kern_lehrer`, train a jump,
> read the progress, pick the best network and bring it into the Studio. Finally, you see how a
> completely new movement gets added, and you learn about the traps that the real training runs
> have already fallen into.

How the Lehrer computes on the inside is in [Chapter 14](14-lehrer.md), the foundations are in
[Appendix A](A-neural-networks.md).

## 15.1 Building the tool

```
kern/tools/lehrer.sh              # builds kern/build/kern_lehrer
kern/tools/lehrer.sh /path/out    # builds /path/out/kern_lehrer
```

The script runs on Linux and macOS, on x86 and ARM. It compiles everything with full optimization,
without FMA contraction and with Box3D without SIMD, and only rebuilds what has changed. `CC`/`CXX`
choose the compiler, `KERN_JOBS` the number of parallel compilers, and `KERN_NATIVE=0` turns off the
optimization for your own processor.

> **Did you know?** These processor switches only choose instructions; they do **not** change any
> result. The network stays the same, bit for bit, as in the browser and in the Studio
> ([Section 14.7](14-lehrer.md)).

On startup, `kern_lehrer` loads Pollen's networks from `third_party/microduck/` (or
`--policies DIR`). It needs them for the warm start (velstand), the demonstration and the play
layer. How Kern itself is built is described in [Chapter 2](02-building-and-testing.md).

## 15.2 Everything the tool can do

| Commands | What for |
|---|---|
| `--task jump` / `stand` | train a movement with PPO |
| `--eval`, `--trace` | measure a network; look at an episode tick by tick |
| `--vorbild-suchen`, `--vorbild-pruefen` | search for and test a jump demonstration with CMA-ES |
| `--spielen`, `--spielen-pruefen`, `--spielen-ueben`, `--spielen-spur` | imitate, measure, practice and watch the play layer |
| `--biss-fenster`, `--halt-messen` | measure the bite and the stopping distance |
| `--demo-spur` | play the example level as in the Studio |
| `--fingerprint 400` | check determinism |
| `--kalibrieren` and friends | calibration ([Chapter 16](16-calibration.md)) |

For all of them: `--threads N` (default: all cores) and `--seed N` (default 1).

## 15.3 Training the jump

### The standard run

```
kern/build/kern_lehrer --task jump --iterations 3000 \
    --out jump.onnx --log jump.csv \
    --checkpoint run.ckpt --keep-every 250
```

This is exactly how `policies/jump.onnx` was made (32 ducks on 2 cores, seed 1). What happens:

1. **Load the recipe:** warm start from velstand, imitation of the built-in demonstration (8 rounds
   of 96 episodes each), then practice with noise 0.1, learning rate 10⁻⁴ and imitation reward 10.
   Options on the command line override individual values.
2. **Create the ducks:** `--envs` (default 16 per thread), rounded down to a multiple of the
   threads.
3. **Imitate** with DAgger. Afterward, `jump_nachgeahmt.onnx` is there and, with `--checkpoint`,
   the learner in `run.ckpt.nachgeahmt`.
4. **Practice** with PPO: 30 iterations of only the critic, then both. Every 10 iterations, the
   network jumps 5 times on the clean robot as a test.
5. **Save:** every 25 iterations and at the end; with `--keep-every`, also snapshots such as
   `jump_it00250.onnx`.

When a test reaches at least 4 out of 5 real jumps for the first time, the tool cheers
`FIRST REAL JUMP at iteration …` and saves immediately. With `--stop-after-jump`, it then stops.

### Resuming, no demonstration, your own demonstration

`--resume run.ckpt` loads the whole learner and skips the imitation.

> **Watch out!** When resuming, `--iterations` is the **target**, not the number of additional
> iterations. If you resume after 3000 with `--iterations 3000`, you won't get a single new one. So
> for another 3000, use `--iterations 6000`.

With `run.ckpt.nachgeahmt`, practice starts over from the imitation. `--vorbild none` tries pure
PPO – the route that did not find the takeoff. So that the duck has any direction at all without a
demonstration, the reward then gets a reference height (`track_height`). Further switches change
the demonstration, the DAgger rounds, the practice noise and the imitation reward.

This is how you search for your own demonstration:

```
kern/build/kern_lehrer --vorbild-suchen demo.json --iterations 120
kern/build/kern_lehrer --vorbild-pruefen demo.json --episodes 20
kern/build/kern_lehrer --task jump --vorbild demo.json \
    --out jump2.onnx
```

For each generation, a line appears with the score and a verdict ("a real jump," "no real jump,"
"fell"). With `--vorbild start.json`, you continue searching from an older result.

**In the browser,** the page `/lehrer/` trains the same recipe ([Section 14.11](14-lehrer.md)). The
same seed and the same total number of ducks (workers × ducks per worker) give the same network as
natively.

## 15.4 Reading progress

Each practice iteration writes a line like `it 512 … return … len … value … std … lr …`, and with
an evaluation also `| jumps 4/5 up … rise … flight … tilt … falls …`.

| Field | Meaning | What to watch for |
|---|---|---|
| `return` | mean return | should rise over hundreds of iterations |
| `len` | episode length in ticks | 100 = 2 s; shorter: fall or ground contact |
| `value` | the critic's error | high at first, then falling |
| `std` | action noise σ | drops as the duck becomes more confident (min. 0.001) |
| `lr` | learning rate | stuck at 1e-5: the policy is changing too much |
| `steps/s` | duck steps per second | speed of the device |
| `jumps n/5` | test on the clean robot | the actual goal |

During imitation, each round shows the examples, the error and the real jumps. With
`--log jump.csv`, every iteration ends up as a CSV line in a file you can load straight into a
spreadsheet (`jumps` is −1 where there was no evaluation).

**If something goes wrong,** `--trace` shows an episode tick by tick: height, sole contacts, ground
contact of other parts, tilt and every reward term.

```
kern/build/kern_lehrer --trace jump_it01000.onnx
kern/build/kern_lehrer --trace builtin          # the demonstration
kern/build/kern_lehrer --trace jump.onnx --randomized
```

That way you see which term brings in the points – and whether the duck is tricking the reward.

> **Imagine …** the log lines are the fever chart in a hospital: a single measurement says little,
> the trend over hundreds of iterations says everything. And `--trace` is the X-ray.

## 15.5 Picking the best network

The last saved file is not always the best: PPO can get worse again after a good stretch, and the
test during the run has only 5 episodes. So:

1. Keep snapshots with `--keep-every N`.
2. Measure each one with more episodes:
   `for f in jump_it*.onnx; do kern/build/kern_lehrer --eval "$f" --episodes 40; done`.
   You get two lines: "clean robot" and "randomised" (full variation).
3. Choose by: real jumps with variation, then zero falls, then rise, flight time, tilt.
4. Check in the Studio (15.8).

For `policies/jump.onnx`, that gave 40 of 40 clean and 36 of 40 with variation, without a fall.

## 15.6 Turning the knobs

### One iteration in numbers

What a switch does is easiest to see in a worked-out iteration of the jump recipe with 32 ducks:

- **Collect:** 32 ducks × 48 ticks = **1536 rows**, each with noise σ = 0.1.
- **Normalizers:** the critic's learns from the 1536 rows; the actor's is frozen.
- **GAE:** per duck, backward over 48 ticks, γ = 0.99, λ = 0.95; then the advantages are brought to
  mean 0 and spread 1.
- **Minibatches:** 1536 / 4 = **384 rows**, each split into 8 pieces of 48 rows.
- **Updates:** 5 epochs × 4 minibatches = **20 Adam steps**, each preceded by the KL measurement.

The imitation would have had at most 76,800 examples (8 × 96 episodes × 100 ticks); there were
49,936, because episodes end earlier on a fall or ground contact. After each round, the actor is
fitted to all examples so far in 10 epochs (minibatch 256).

### The most important switches

| Option | Jump recipe | Effect |
|---|---|---|
| `--envs N` | 16 per thread | more data, calmer gradient; **determines the result** |
| `--threads N` | all cores | the speed – with a fixed `--envs`; without `--envs` you get 16 ducks per thread |
| `--steps N` | 48 | ticks per duck and iteration |
| `--lr X`, `--kl X` | 10⁻⁴, 0.01 | starting learning rate, target KL |
| `--uebungs-std X` | 0.1 | noise during practice |
| `--entropy X` | 0.005 | slows down the shrinking of the noise |
| `--substeps N` | 16 | don't change it! |

On top of that come epochs, minibatches, network sizes (with a warm start, the actor has to match
velstand), initial noise (`--init-std`), adaptive learning rate on/off, warm start (only `velstand`
works; anything else means random weights; only the play layer starts from a file) and
`--profil servo.json` for calibrated servos ([Chapter 16](16-calibration.md)); everything else is in
[Chapter 17](17-reference.md).

> **Imagine …** you have just taught a dog a trick with a lot of effort. If you now shout new
> commands and wave treats around wildly, the trick is quickly gone again. It's the same with the
> network: after imitation, a **small learning rate (10⁻⁴) and small noise (0.1)** are a must.

Also: more noise does **not** help with finding things (without a warm start, the duck only learned
to stand with it). Few ducks slow things down, because the gradient gets noisy. And fewer sub-steps
would be 1.6 times faster, but then the network would learn a different physics than the Studio's.

## 15.7 Training the play layer

Six steps:

1. **Measure what the demonstration needs:** `--biss-fenster` places a sock on an 11 × 9 grid in
   front of the duck and prints where the bite holds. `--halt-messen` shows the stopping distance
   after "stop." Training measures the bite by itself anyway; the commands are for looking things
   up.
2. **Imitate:**
   `kern_lehrer --spielen --nachahmen 12 --episoden 32 --out play.onnx --checkpoint play.ckpt`.
   The default is 8 rounds of 64 episodes each. For each round, the tool measures the network in
   16 clean episodes.
3. **Check:** `--spielen-pruefen vorbild` and `--spielen-pruefen play.onnx`. Measuring the
   demonstration shows the most the network can reach through pure imitation.
4. **Watch:** `--spielen-spur --seed 3` shows an episode decision by decision (25 per second); with
   `--netz play.onnx`, the network drives, and the actions of the demonstration and the network
   appear side by side. That way you see where they diverge.
5. **Practice:** `--spielen-ueben play.onnx --iterations 300 --envs 32 --out play_geuebt.onnx`:
   warm start from the file, noise 0.1, 20 iterations of only the critic, then PPO. Every
   20 iterations, a measurement and a snapshot.
6. **Check in the example level:** `--demo-spur --netz play_geuebt.onnx --seed 120` plays the
   Micro Duck level as the Studio does, for 120 s (here `--seed` is the duration!), one line per
   second: position, state, what the tracker (the play layer's memory) knows and what the play
   network answers. It takes `jump.onnx` and your play network (otherwise `play.onnx`). With
   `KERN_JUMP_AT=SECOND`, it presses key 7 in that second.

> **Watch out!** During imitation, `--out` is overwritten after **every** round. If you want to keep
> an earlier round, copy the file while the run is going. (For the old `play.onnx`, round 10 of 12
> was the best.)

## 15.8 Using it in the Studio

Kern's own networks have fixed names: `jump.onnx` and `play.onnx`. Three ways into the Studio:

1. **Into the repository:** copy to `kern/policies/`. The developer page loads from there, a
   deployed page from `policies/` next to it. The engine only fetches a network when a duck needs
   it.
2. **From the browser Lehrer:** the Lehrer page saves its network on the same device; the Studio
   loads it on startup and picks up every new one immediately, even in open tabs (for the jump
   only).
3. **Check:** open the Micro Duck level, Play, take control of the duck with **F8**, key **7** for
   the jump, or leave the duck alone (play layer: Autonomous and Kern extras on, ball or sock in
   sight). The browser console reports "Kern: trained network jump.onnx loaded" or warns you if
   something doesn't load.

The network must have the shape of an rsl_rl actor (Sub, Div, Gemm/Elu …, Gemm) – exactly how the
Lehrer exports it. A play layer without 23 inputs and 9 outputs is not computed (its action stays
0).

> **Try it!** Train a jump on the Lehrer page while keeping the Micro Duck level open on
> https://kernengine.de/, start Play, take control of the duck with **F8** and press **7** as soon
> as the console reports "trained network jump.onnx loaded." Now **your** duck is jumping.

## 15.9 Adding a new movement

A new movement with Pollen's layout (61 observations → 14 actions) needs four things, and difficult
movements often a fifth:

1. **A `Task`** in `src/lehrer/`. As a skeleton, the blue book shows a bow, `BowTask`: 2 s duration,
   random start, phase signal in slots 55 and 56, four secret values for the critic, a curriculum
   for the fall penalty, a reward made of named terms. Helpers such as `upright`, `action_rate` and
   `fell` are already in `tasks.cpp`; a factory function `make_bow_task()` makes the class usable
   from outside.
2. **Recipe and name** in `src/lehrer/recipe.cpp`:

   ```cpp
   std::unique_ptr<Task> make_task(const std::string& name) {
     if (name == "jump") return make_jump_task();
     if (name == "stand") return make_stand_task();
     if (name == "bow") return make_bow_task();          // new
     return nullptr;
   }
   ```

   The function translates names into tasks; the new line delivers the bow for "bow," and anything
   unknown gives nothing. In addition, `recipe_for` gets an entry (warm start, noise, learning
   rate). After that, `--task bow` and the browser know the task; the Lehrer page only lacks a menu
   entry.
3. **A measurement** that checks what the movement **is**, not what the reward counts. `JumpMeter`
   shows the pattern: observe tick by tick, decide at the end. `--eval` and the test during the run
   currently only know the jump; a new task needs its own evaluation.
4. **The wiring in the Studio,** as for the jump: movement list, key, touch button, name in the
   logic node **Robot Move**, file name and the file in `kern/policies/`. In the Studio, the duck
   then switches hard to the new network and back again after its duration; the Studio builds the
   observation exactly as in training (with phase).
5. **A demonstration (optional):** a small parametrization, a score for CMA-ES and a demonstration
   object that has an answer in **every** state (otherwise no DAgger). CMA-ES and imitation are
   general; the demonstration functions are currently tailored to the jump.

Inputs that velstand has never seen must be declared as new for the warm start (currently the phase
slots 55 and 56). For a completely different layout, the play layer shows the way: its own runtime
layer, environment, learner, metadata and evaluation – PPO and imitation stay the same.

## 15.10 Designing good tasks

What Kern's networks model for you:

- **Only what the robot really has.** The actor sees sensor values (IMU, joints, camera detector,
  mouth encoder), never true positions; the play layer knows ball and sock only with noise and
  dropouts. The truth belongs in the critic's secret values.
- **Make time visible.** A movement with a sequence gets a phase signal (cos, sin), not a counter.
  A network without memory needs time quantities: the play layer sees how long it has been
  standing, walking and carrying.
- **Measure instead of command.** "Standing still" means "both soles on the ground, no movement
  running" – what the legs do, not what was commanded.
- **Actions like controls:** walking commands with a dead zone like a stick (below 20 % the duck
  stands), movements via a threshold (above 0.5 they start), limits for head and neck.
- **Reward made of named terms,** weight × term × dt, with brakes as with Pollen (action change,
  rotation rate, self-contact); curricula make penalties and variation stricter step by step.
- **Hard episode ends:** a fall and ground contact by other parts end the episode.
- **Demonstration without secret knowledge:** it sees only what the network sees; a wish that
  follows from a duration rises as a ramp.

> **Try it!** On https://kernengine.de/, start Play in the Micro Duck level (Kern extras is on there)
> and lift the ball with the mouse to another spot. Watch how the play layer decides anew from its
> sensors alone: look, walk over, stand, kick.

## 15.11 Pitfalls from the real runs

From `docs/lehrer.md`, so you don't fall into them:

- **The reward gets exploited.** Early ducks sat on their tails, did dives or propped themselves
  up. Every time, only a stricter rule helped: ground contact ends the episode, tilt costs, points
  only when upright.
- **The measurement was too lax.** The first one let through a duck that merely straightened up and
  pulled up its feet, tipping by 45°. Since then, only a real jump counts (rise in the air, flight,
  at most 30° of tilt, landing).
- **PPO alone does not find rare movements;** without "critic first," what was imitated breaks;
  drifting normalizers distort the update (all in Chapter 14).
- **Bugs in the world instead of the network.** Training the play layer brought three to light: the
  sock crept across the floor by itself, the ball rolled forever, and the idle yawn opened the beak
  during pick-up. The fix: rolling resistance for sock and ball, and a running movement no longer
  counts as standing still. If a network fails to learn at some point, `--spielen-spur` and
  `--trace` often reveal a bug like that.
- **Hand-entered numbers go stale.** The bite position used to be a number in the docs; now the
  Lehrer measures it on every run.
- **Breaking determinism:** one `std::sin`, one sum in thread order or `-ffast-math`, and the
  network is a different one on another device. `--fingerprint 400` gives it away.
- **Different physics than the Studio:** fewer sub-steps, a different floor, a different servo
  profile – and the network learns a different world.

## What you take away

- `kern/tools/lehrer.sh` builds the tool; processor switches don't change any result.
- A standard run first imitates, then practices, and regularly saves snapshots.
- `return`, `len` and above all `jumps` show the progress; `--trace` shows what goes wrong.
- The last network is not always the best: measure snapshots with `--eval`, clean and with
  variation.
- The number of ducks determines the result, the number of threads only the speed.
- After imitation: small learning rate, small noise, critic first.
- A new movement needs a task, a recipe, a measurement, Studio wiring and often a demonstration.

## Brain teasers

1. You have trained 3000 iterations and want 1000 more. What value do you give `--iterations`?
2. Why do you measure the candidates at the end with 40 episodes instead of relying on the 5-episode
   test?
3. Your computer suddenly has twice as many cores. Does your network change if you double
   `--threads`? And if you double `--envs`?
4. Why shouldn't the measurement of a new movement simply add up the reward?

### Solutions

1. 4000 – `--iterations` is the target.
2. Five episodes are a very small, noisy test; a network can look good by chance. And only with
   variation does it become clear whether it is robust.
3. More threads with a fixed `--envs`: the same network, just faster. Without `--envs`, though,
   more threads bring more ducks with them (16 per thread) – and more ducks means a different
   network.
4. Because networks exploit loopholes in the reward. The measurement has to check whether the
   movement really happened.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
