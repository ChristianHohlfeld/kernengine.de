# 16 Calibration: from the Screen to the Table

> **Your level:** A network that runs on the computer should also run on the real Micro Duck. For
> that, the simulation has to match the real robot precisely. Here you'll learn how to measure a
> servo and adjust the simulation to reality.

> **Warning: safety – read this first!**
> From here on, this is about a **real robot**. Work with it **only together with an adult**.
>
> - **Hands off running servos.** Under load they swing out fast and hard.
> - Clamp the test bench down firmly, secure the arm and weight, keep hands and cables out of the
>   swing.
> - Limit the current and keep an emergency stop within reach.
> - Never use more voltage than the servo manufacturer allows.
> - Charge lithium batteries only with a suitable charger and under supervision.
> - Test new networks first while the robot is being held or hanging.
> - Everything at your own risk ([Imprint](00-imprint.md)).

Kern takes three routes, and a fourth principle is added:

1. **Device models with profiles**, fitted to measurements and checked against other measurements.
2. **Matching the whole robot:** a log from the real robot runs in the simulation.
3. **Domain randomization:** whatever you can't measure, the training varies.
4. **Measure in the physics instead of typing numbers in.**

Box3D stays untouched ([Chapter 6](06-physics.md)). The tool is `kern_lehrer`.

## 16.1 The servo model

You know the law from [Chapter 13](13-micro-duck.md): a pull of `K · (target − angle)`, at most T,
plus damping D, friction F, armature A and a delay. A **servo profile** stores these numbers as a
JSON file, for example `"stiffness": 0.554` for K. When reading it, Kern checks every value: is it
a number in the allowed range? Stiffness and torque must be greater than 0. Missing values stay 0.

A different servo is a different profile. A device that works in a fundamentally different way
(a linear motor, say) gets a model of its own. Then there are **spreads**: for the XL330, armature
and friction spread by ±10 %. From that, the training rolls separate values for each servo and
episode, plus a bus delay between 15 and 30 ms for each episode. That way the network doesn't rely
on a number nobody knows exactly. A well-measured servo gets small spreads, a poorly known one
large ones.

## 16.2 The test bench

### How to measure

1. **Setup:** servo firmly clamped, axis horizontal, an arm with a weight. Weigh the mass to within
   0.1 g, measure the distance of the center of mass from the axis (balance the arm on an edge).
2. **Run it as in the robot:** same settings, same voltage (7.4 V, if the manufacturer allows it).
3. **Signals**, at least 100 measurements per second: jumps between angles, a "chirp" (a
   swing that gets faster and faster, 0.5 → 4 Hz) and a square wave.
4. **Two or three loads**, for example 50 g at 5 cm and 120 g at 6 cm. At least one log per servo
   is kept back as a **check log**.
5. **Measure several servos.** How much they differ becomes the spread in the profile.

A log is a text file:

```
# mass 0.05
# arm 0.05
# horizontal 1
time,target,angle
0,-0.5,0
0.005,-0.5,-0.0123
```

At the top is the load: mass in kg, distance of the center of mass from the axis in m, optionally
`# inertia`. `horizontal 1` means a horizontal axis: gravity pulls on the arm like on a pendulum.
Below that, each line has the time in s, the target angle sent and the measured angle in rad.

### The test bench in the computer

Kern rebuilds the setup in Box3D, just like the duck: scale 4, the same step rate, the same joint
softness, armature as a disc. There's gravity only with a horizontal axis. Then Kern plays back the
targets from the log, each one later by the delay, and compares the angles. The error is the **RMS
error**: square all the deviations, take the average, take the square root. Example: deviations of
3 and 4 mrad give √((9 + 16) / 2) ≈ 3.5 mrad.

### Fitting and checking

```
kern_lehrer --kalibrieren steps.csv,chirp.csv
            --pruefen square.csv --out xl330_unit3.json
```

(In the terminal, on one line. `--kalibrieren` means "calibrate," `--pruefen` "check.") The search
method **CMA-ES** tries 12 candidates per round for K, D, F, T, A and the delay, and in 80 rounds
it feels its way toward the best ones (you can change that with `--iterations`; the start is
`--profil`, otherwise XL330). It changes K through A as a **factor**: doubling counts as much as
halving, no matter how big the number is. It searches for the delay between 0 and 0.2 s.

> **Picture it like this:** Playing hot-and-cold with twelve friends: everyone searches at the
> same time, and the next round starts where someone heard "hot!" the loudest.

What counts is the error on the **check log**. Only that shows whether the model really explains
the servo.

> **Watch out, trap:** If you only check on the fitting logs, it's like studying for a test using
> problems you already know. The check log is the new problem – a different load, a different
> signal.

In the test, a servo differs a lot from the nominal profile (for example K −20 %, 30 ms delay).
After fitting on two logs, the error on the third drops to less than a fifth of its starting
value and below 3 mrad.

Without a real servo, `kern_lehrer --beispiele DIR` ("examples") helps: it writes example logs
(`servo_steps.csv`, `servo_chirp.csv`, `robot.csv`) and the profile `xl330.json`, generated in the
simulation: 50 g at 5 cm with a ±0.5 rad square wave, 120 g at 6 cm with a chirp, 250 control
ticks of the whole robot.

## 16.3 Matching: the whole robot

For this you need a log from the robot: a header line, then 50 lines per second with 75 numbers
each (61 observations, 14 actions), starting still in the home pose. How the robot writes it isn't
covered in this book. `kern_lehrer --abgleich robot.csv --profil …` ("matching") gives a clean duck
with the profile's servos the actions from the log, tick by tick (as in training: scale 1, no
low-pass). Along the way it compares turning, gravity, joint angles and joint speeds with the log
and reports the RMS error for each group and for every joint individually.

A log from the simulation itself, with the same servos, gives a gap of **0**, bit for bit; with
different servos, a measurable gap. Because Kern is deterministic, any difference from a real log is
a real model error, not chance.

## 16.4 Training with your own profile

`kern_lehrer --task jump --profil xl330_unit3.json` trains the jump with the profile's servos and
its spreads. The play layer currently always uses the XL330 profile.

## 16.5 Domain randomization

Whatever isn't measured, the training rolls the dice on ([Chapter 14](14-lehrer.md)):

| Uncertainty | How the training covers it |
|---|---|
| Individual servos | friction, armature ±10 % |
| Bus latency | 15–30 ms |
| Build | trunk mass ±5 %, center of mass shifted |
| Floor | sole friction × 0.7–1.3 |
| IMU, encoders | noise, offset, delay |
| Disturbances | pushes every 3–6 s |
| Camera | position noise, up to 20 % dropouts |
| Toys | ball 3.5–6 cm, 15–50 g |

The judge in training (the **critic**) sees the clean state; only the network that goes onto the
robot gets noisy sensors. Gear backlash is missing from the model; Pollen's training varies it, and
velstand is robust against it. Kern's own networks learn without it.

> **Mission:** In the Micro Duck level, start **Simulate** (Alt+S), grab the duck by the head with
> the mouse, pull it to the side and let go once it falls over. Pollen trained velstand with lots of
> deviations – measured, in Kern it's back on its feet from lying on its belly or back within 12 s
> at most.

## 16.6 Measure in the physics instead of typing numbers in

Some numbers Kern measures in the simulation instead of writing them into the code: the **bite
position** (tried out at 99 spots: where does the beak grab a sock most reliably?), the **grip**
via the mouth encoder, and **standing still** via the foot contacts. Still typed in are the
coasting after "stop" (0.7 cm from 0.12 m/s, measured with `--halt-messen`, "measure stopping")
and the pair friction of the surfaces – candidates for measurements on the real material.

> **Mission:** Start **Simulate** and turn on **Contact Points** under Window › Show. You'll see
> where the soles touch the floor – that's exactly how the play layer knows whether the duck is
> standing.

## 16.7 Limits

There's **no real measurement data** yet; everything has been checked with logs from the
simulation. Still open are gear backlash, armature, the right kick, an IMU profile, friction mixing
and the camera detector. Profiles and logs are in real units; Kern only converts scale 4 at the
boundary to Box3D.

> **Mission:** If the level is running, stop it with **Esc**. Select the **Micro Duck**, set
> **Actions to servo goals** to `robot` in the Details panel, start Play and watch it walk. That's
> the setting Pollen's software uses to drive the real robot.

## Level complete!

- You work on the real robot only with an adult.
- You know what a servo profile is and what spreads are for.
- You know the test bench: clamp, measure, fit, check.
- You can calculate an RMS error.
- You understand why determinism makes matching so powerful.

## Puzzle corner

1. What do you do before you connect a real servo?
2. Calculate the RMS error for deviations of 1, 1, 1 and 5 mrad.
3. Why does a log with a different load belong in the check log?
4. **Boss puzzle:** Matching shows 10 times more error at one joint than at all the others. What
   do you conclude?

### Solutions

1. Get an adult, clamp the test bench, limit the current, have the emergency stop ready.
2. √((1 + 1 + 1 + 25) / 4) = √7 ≈ 2.6 mrad. One big outlier counts a lot.
3. Only then does it show whether the model explains the servo or has just memorized the curves.
4. Something is missing from the model there, for example gear backlash. It can't be chance,
   because the simulation is deterministic.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
