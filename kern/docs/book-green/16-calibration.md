# 16 Calibration: From the Computer into the Real World

> **In short:** A network from the Lehrer is supposed to run on the real Micro Duck. For that, the
> simulation has to match the robot well, and the network must not rely on anything the simulation
> doesn't know precisely. To get there, Kern measures servos, compares the whole robot with the
> simulation and covers the rest with randomness.

Kern follows three routes and one principle:

1. **Device models with profiles:** A servo with a controller, friction and delay is a small model
   with numbers in real units, fitted to measurements and checked against other measurements.
2. **Matching:** A recording from the real robot is replayed in the simulation; the difference is
   the remaining gap to reality.
3. **Domain randomization:** Whatever the measurement doesn't pin down, the training varies at
   random.
4. **Principle:** Whatever follows from the model and the physics, the Lehrer measures itself. Box3D
   stays untouched throughout ([Chapter 6](06-physics.md)).

> **Imagine …** you practice on a keyboard and tomorrow you'll play a real grand piano. You can
> check how heavy its keys are (device model), compare a recording of the piano with your practice
> (matching), and practice on lots of different instruments (randomization).

> **Watch out!** Servos under load can swing fast and hard. Clamp the test bench firmly, secure the
> arm and weight, keep hands and cables out of the swing range, limit the current, and keep an
> emergency stop within reach. Never exceed the manufacturer's operating voltage, and charge lithium
> batteries only with a suitable charger and under supervision. Test new networks on a robot that is
> held or suspended first. Everything at your own risk (see the [imprint](00-imprint.md)).

## 16.1 The servo model and its profile

You know the servo model from [Chapter 13](13-micro-duck.md): pulling toward the target up to the
maximum torque, braking, friction, rotor inertia (armature), delay – computed by the same function
in the Studio, in the Lehrer and on the test bench. The numbers are stored in a **profile**, a JSON
file (shortened):

```json
{
  "kind": "servo", "name": "xl330",
  "stiffness": 0.554, "damping": 0.053,
  "friction": 0.0048, "torque": 0.963,
  "armature": 0.0018, "delay": 0.02,
  "spreadFriction": 0.1, "spreadArmature": 0.1,
  "delayMin": 0.015, "delayMax": 0.03
}
```

Line by line: profile xl330; how firmly it pulls and brakes; friction and force; how sluggishly and
how late (20 ms) it reacts; friction and armature may deviate by ±10 % per unit; in training the
delay spreads between 15 and 30 ms. Kern checks every value against a sensible range. A different
servo is a different profile; a fundamentally different device, such as a linear actuator, would
get a model of its own.

The **spreads** are honestly admitted uncertainty: from them, the training draws separate values
per episode and servo. A well-measured servo gets small spreads, a poorly known one large spreads.
That way the network doesn't depend on numbers nobody knows exactly.

## 16.2 The test bench: measuring a servo

### How to measure

1. **Setup:** Clamp the servo with its axle horizontal and an arm with a weight; measure the mass
   (to 0.1 g) and the distance to the center of mass. Angle 0 means: the arm hangs straight down.
2. **Run it as in the robot:** the same firmware settings, voltage (7.4 V, if the manufacturer
   allows it) and bus rate.
3. **Signals**, sampled at least 100 times per second: small and large steps (about ±0.2 and
   ±0.9 rad), a chirp (a back-and-forth of 0.5 rad that speeds up from 0.5 to 4 Hz) and a square
   wave (±0.3 rad).
4. **Two or three loads**, for example 50 g at 5 cm and 120 g at 6 cm; at least one log per servo
   used only for checking.
5. **Measure several servos**; their differences become the spread.

### Log and test bench in the computer

A test-bench log is a text file: at the top, comment lines about the load (`# mass`, `# arm` …;
`# horizontal 1` means: the axle is horizontal, and gravity pulls the arm like a pendulum), then
lines with the time, the angle sent and the angle measured. Kern rebuilds the servo with its load in
Box3D, in the same environment as the duck (scale 4, same step rate, joint softness and armature),
plays back the sent targets with the delay, and compares the simulated and measured angles. The
error is the root mean square, **RMS** for short.

### Fitting and checking

```
kern_lehrer --kalibrieren steps.csv,chirp.csv \
  --pruefen square.csv --out xl330_unit3.json
```

This searches for a profile that best explains the first two logs and checks it on the third. The
search uses **CMA-ES** ([Chapter 14](14-lehrer.md)): 12 candidates per generation, 80 generations by
default. The six knobs are the five servo values (as a factor relative to the starting profile) and
the delay. With `--profil` the search starts from a starting profile of your own, `--iterations`
changes the number of generations, and `--threads` spreads the candidates over several CPU cores.

> **Imagine …** twelve guitars with six tuning pegs each, every one tuned a little differently.
> CMA-ES listens to which ones sound best and tunes the next twelve similarly to the winners.

What really counts is the **check error** ("held out"). Only it shows whether the model explains
the servo or has merely memorized the curves – like a test with new problems. That's why the check
log should have a different signal and a different load.

> **Did you know?** In the test, a servo deliberately deviates from the profile (stiffness −20 %,
> friction +50 %, 30 ms delay and more). Calibration pushes the check error down to less than a
> fifth and below 3 milliradians.

Without a real servo, `kern_lehrer --beispiele beispiele/` writes simulated example logs (steps,
chirp, a robot log) and the profile, so you can run through the whole chain once.

> **Try it!** Open the Micro Duck level in Kern Studio at https://kernengine.de/ and set the duck's
> **Actions to servo goals** to `robot` (this is how Pollen's software drives the real robot).
> Start Play, take over the duck with **F8** and kick the ball with 3. Do you notice the
> difference?

## 16.3 Matching: the whole robot

For **matching** you need a log from the real robot with all 61 observations and 14 actions, 50
times per second, starting at rest in the home pose. (How the robot writes it is not described in
this book.)

```
kern_lehrer --abgleich robot.csv --profil xl330_unit3.json
```

The simulation starts standing, on the clean robot, receives the same actions tick by tick as in
training (without scaling down or smoothing) and compares its sensors with the log: rotation,
gravity, joint angles, joint velocities – per group and for each joint individually. If one joint
keeps standing out, something is missing from the model there, such as gear backlash.

The nice part: a log from the simulation, played back with the same servos, gives a gap of
**exactly 0**, bit for bit, because the simulation is deterministic; with different servos, a
measurable gap appears. So every difference from a real log is a real model error, not noise.

> **Try it!** In the Micro Duck level, under **World Settings**, set the Step rate to 90 instead of
> 100 and start Play. Kern reports that the timing doesn't fit:
> "set World Settings > Step Rate to 100".

## 16.4 Training with your own profile

```
kern_lehrer --task jump --profil xl330_unit3.json --out jump.onnx
```

Now the ducks train with the measured servos, spreads included; when evaluating without variation,
the profile applies exactly. This affects the motions; the play layer currently always uses the
XL330 profile and varies friction and armature within its spread.

## 16.5 Domain randomization

Whatever isn't measured or varies, the training rolls the dice on:

| Uncertainty | How the training covers it |
|---|---|
| Individual servos | separate values per servo (XL330: ±10 %) |
| Bus latency | 15–30 ms |
| Build (battery, cables …) | trunk mass and inertia ±5 %, center of mass shifted |
| Floor | sole friction × 0.7–1.3 |
| Orientation sensor, joint encoders | noise, 0–1 tick delay, offset up to ±0.015 rad |
| Disturbances | pushes every 3–6 s, up to ±0.3 m/s |
| Camera detector | position noise, up to 20 % dropouts |
| Toys | ball radius 3.5–6 cm, 15–50 g |

The exact ranges are in [Chapter 14](14-lehrer.md). One trick: the **critic**, which does the
evaluating during training, always sees the clean state; only the **actor**, which goes onto the
robot, has to cope with noisy sensors. Gear backlash is varied only in Pollen's training, through
its own variants of the blueprint (velstand is robust against it); Kern's own networks learn
without it.

## 16.6 Measuring in the physics instead of writing numbers in

Numbers that follow from the model and the physics stop being right as fixed values in the code
after the next change. So Kern measures them in the simulation, just as on the real robot:

- **The bite position:** Where does the sock have to lie for the pick-up to grab and hold it? Before
  the first training, the Lehrer tries 99 spots and takes the most reliable one.
- **The coasting after "stop"** (`--halt-messen`): here the rule doesn't fully apply yet – the
  demonstration has the measured value (0.7 cm from 0.12 m/s) written in as a fixed number; after a
  change to the physics, you measure again.
- **The grip sensor:** "holding something" is whatever the mouth servo reports.
- **Standing still:** measured via the foot contacts, not via "no walking command".

What is written in, on the other hand, is the friction of the material pairs, justified by typical
values – it's waiting for a measurement on the real material.

## 16.7 Limits and next steps

- **No real measurement data yet** – everything has been checked with logs from the simulation.
- **Gear backlash** is missing; if matching reveals it, it goes into the servo model.
- **Armature** is a co-rotating disk; **kick right** sometimes tips over.
- The **orientation sensor** and **camera detector** don't have measured profiles yet.
- **Friction mixing** and **scale 4** are harmless: in the example level, floor and sole both grip
  with 1.0, and everything is in real units.

## What you take away

- Three routes to the real robot: device models, matching, randomization.
- A servo is a profile: a few numbers in real units plus spreads.
- CMA-ES fits the profile to test-bench measurements; what counts is the check error.
- Matching shows, joint by joint, where simulation and robot drift apart.
- Determinism turns every difference into a real model error.
- Whatever can be measured, Kern measures instead of writing it in.

## Brain teasers

1. Why isn't it enough to look only at the error on the fitting logs?
2. A servo has been measured very well. Should its spread be large or small?
3. A log from the simulation is matched with the same servos. What gap comes out, and why?
4. Why is it risky to write the bite position into the code as a fixed number?

### Solutions

1. Because otherwise the profile might just memorize the curves. Only new data shows whether it
   really explains the servo.
2. Small. Large spreads are for values you don't know precisely.
3. Exactly 0, bit for bit, because the simulation is deterministic.
4. After the next change to friction, the model or the servo, it's no longer right. Measured, it
   always fits the current physics.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
