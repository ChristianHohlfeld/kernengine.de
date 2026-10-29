# 16 Calibration: from the Screen to the Table

> **Your level:** A network that runs in the computer should also run on the real Micro Duck. For
> that, the simulation has to match the real robot exactly. Here you'll learn how to measure a servo
> and adjust the simulation to reality.

> **Words you need:**
> - **Servo** – a small motor with a gearbox that turns a joint to a certain angle
> - **Simulation** – the robot as a replica in the computer, calculated by the rules of physics
> - **Network** – the small calculating program that tells the duck how to move its joints
> - **Training** – lots of practice rounds in the simulation in which the network gets better
> - **Log** – a text file with measurements, line by line
> - **Sensor** – a part that measures something, such as an angle or a rotation
> - **deterministic** – always calculates exactly the same result from the same inputs

> **Attention, safety – read this first!**
> From here on it's about a **real robot**. Work with it **only together with an adult**.
>
> - **Hands off running servos.** Under load they swing out fast and hard.
> - Clamp the test bench firmly, secure the arm and weight, keep hands and cables out of the swing
>   range.
> - Limit the current (for example with a lab power supply on which you can set a current limit)
>   and keep an **emergency stop** within reach: a switch that cuts off all power immediately.
> - Never more voltage than the servo manufacturer allows.
> - Charge lithium batteries only with a suitable charger and under supervision.
> - Test new networks first while the robot is being held or is hanging.
> - Everything at your own risk ([Imprint](00-imprint.md)).

Calibrating means: adjusting the simulation until it behaves like the real device. On top of that,
the network shouldn't rely on anything the simulation doesn't know exactly. Kern follows **four
principles** for this:

1. **One model for each device:** A servo is described by a few numbers. You measure these numbers
   on the real servo and then check them with a second, new measurement.
2. **Compare the whole robot:** The real robot writes down what it does. The simulation copies
   it, and you compare.
3. **Variation:** Whatever can't be measured exactly, training keeps changing a little.
4. **Measure in the physics instead of typing in:** Numbers that follow from the physics, Kern
   measures in the simulation instead of typing them in by hand (16.6).

The physics engine itself stays untouched. It calculates rigid bodies, joints and contacts
([Chapter 6](06-physics.md)). Whatever a real device does beyond that – a servo with a motor,
friction and delay – is a small model that sits **on top**. Just like a bike computer you clip onto
the handlebars: the bike stays the same, the computer gets added.

> **Picture it like this:** On the left is the real robot, on the right the simulation. In between,
> the same circle keeps running: **measure → adjust → check**. Every round brings the two closer
> together.

## 16.1 The Servo Model

So that the simulation can copy a servo, Kern describes it as a picture made of a few parts. Each
part has a number, and the six numbers together are called the **servo profile**: **spring**,
**force limit**, **shock absorber**, **friction**, **momentum** and **delay**. The pictures for them
come now.

Important: this one servo calculation runs everywhere – in Studio, in the Lehrer and on the test
bench (16.2). That's why numbers measured on the test bench also apply without conversion to the
duck in Studio and in training.

> **Picture it like this:** Between the arm and its target hangs a **spring**. The farther the arm
> is from the target, the harder it pulls it back. A **motor** delivers this force, but it has a
> **force limit**: no more than that, no matter how far away the target is. A **shock absorber**
> like on a bike brakes fast movements so the arm doesn't swing back and forth. On top of that comes
> some **friction**, which holds every movement back a little. (The motor itself has no number of
> its own: it just delivers the spring's force, up to the force limit.)

Two things are still missing from the picture:

- **The motor's momentum:** The small motor in the servo spins very fast through the gearbox. That's
  why it's hard to get going and hard to stop, as if extra weight hung on the arm. Experts call this
  **armature**.
- **Delay:** A command takes about 20 thousandths of a second (20 ms) to reach the servo, like a
  message that's on its way for a moment.

**Example:** If the arm is twice as far from the target, the spring pulls twice as hard. But with
the Micro Duck's servos, it only reaches the force limit when the arm is almost 100° off. So the
servos are rather **soft**, like a loose rubber band ([Chapter 13](13-micro-duck.md)).

A different servo is simply a different profile, meaning different numbers. Only a device that works
completely differently (such as a motor that pushes instead of turning) gets a model of its own.

### How Sure Is a Number?

Every profile also includes how **sure** you are about each number. That's called **spread**. For
the Micro Duck's servo, momentum and friction spread by ±10%, and the delay lies somewhere between 15
and 30 ms. From this, training rolls its own values for each practice round and each servo.

**That's the principle: A well-measured servo gets small spreads, a poorly known one big ones.**
That way the network never relies on a number that nobody actually knows exactly.

> **For pros:** A profile is a small text file. When loading, Kern checks every value to see
> whether it's a number in the allowed range. The exact names and values are in the blue book,
> Chapter 16.

## 16.2 The Test Bench

To find out a servo's numbers, you build a **test bench**: the servo moves an arm with a known
weight, and you record what happens. Then you rebuild the same setup in the computer and turn the
numbers until both look the same.

### How to Measure

1. **Setup:** Clamp the servo firmly, axis horizontal, an arm with a weight. Weigh the mass exactly
   and measure how far the center of mass is from the axis (balance the arm on an edge). With a
   horizontal axis, gravity pulls on the arm like on a pendulum.
2. **Run it like in the robot:** the same settings in the servo itself (for example how strongly it
   steers back when it's off its target) and the same voltage as later in the duck. Otherwise you're
   measuring a different servo from the one that will run later – like testing a bike with too
   little air in the tire.
3. **Give movements** and measure at least 100 times per second where the arm is right now.
4. **Use two or three different weights**, such as 50 g and 120 g. Set aside at least one
   measurement per servo: that's the **check measurement**.
5. **Measure several servos.** How much they differ becomes the spread in the profile.

The movements are three kinds of commands. This is what they look like over time:

![The three kinds of commands over 2.4 seconds (upward: the target angle): steps, a chirp that gets faster and faster, and a square wave](../abbildungen/testsignale.svg)

Upward is the **target angle** each time, to the right is time. The **step** jumps to a new angle
and stays there; experts call it a "step signal" – it has nothing to do with the duck's jump. The
"chirp" (like a bird's chirp) swings back and forth, starts slowly with half a swing per second and
ends with four. The unit for this is called **hertz (Hz)**: swings per second. The **square wave**
keeps jumping back and forth between the same two angles. Three different commands are like three
different test questions: something only one question asks about, you can get right by chance.

Everything goes into a **log**. Each line contains the time, the target angle sent and the measured
angle.

### The Test Bench in the Computer

Kern rebuilds the same setup in the simulation: servo, arm, weight, gravity. The test bench in the
computer calculates with **exactly the same physics as the duck**: the same scale, the same number of
calculation steps per second, equally soft joints. Otherwise the numbers found would fit the test
bench but not the duck – like a recipe you try out on a gas stove and then cook in the oven. Then it
sends the same commands from the log, each one later by the delay, and compares the simulated angle
with the real one, moment by moment.

> **Picture it like this:** Draw the real and the simulated curve on top of each other on graph
> paper. At each moment you measure the distance between the two lines. Lots of small distances
> means: the model fits.

Kern combines all distances into one number, the **RMS error**: multiply each distance by itself
(square it), take the average of that, and take the **square root** of that. The square root is the
reverse of squaring: √25 = 5, because 5 · 5 = 25. For awkward numbers, you use the √ key on your
calculator.

**Example:** Distances 1° and 7°.

- Square: 1 and 49
- Average: (1 + 49) / 2 = 25
- Square root: √25 = **5°**

The normal average would only be 4°. Through squaring, the big outlier counts more. That's exactly
what you want: a model that's sometimes way off should do badly.

### Adjusting and Checking

Now the computer searches for the six numbers of the profile that make the error as small as
possible. A search method called **CMA-ES** does this ([Appendix A](A-neural-networks.md)): in each
round it tries out a dozen variants and moves to where the best ones were.

> **Picture it like this:** A "hot and cold" search game with twelve friends: everyone searches at
> the same time, and the next round starts where "hot!" was loudest.

At the end, Kern takes the **best** candidate here – unlike the role model search for the jump,
where the center of the search cloud wins ([Chapter 14](14-lehrer.md)). The reason: with the jump,
every rating contained chance, and the best one might have been lucky. On the test bench there's no
chance: the same numbers always give the same error. Whoever is ahead here really is better.

What decides it in the end is the error on the **check measurement**, which wasn't used in the
search. Only it shows whether the model really explains the servo.

> **Watch out, trap:** If you only check against the measurements you adjusted with, it's like
> studying for a test where you already know the questions. The check measurement is the new
> question – a different weight, a different movement.

In Kern's test, one servo deliberately deviates strongly from the usual profile. After adjusting to
two measurements, the error on the third, unknown one shrank to less than a fifth.

> **For pros:** The tool for this is the Lehrer program for the command line
> ([Chapter 15](15-your-own-models.md)). It runs in the **terminal**, the text window where you type
> commands instead of clicking. Without a real servo, it can also write example logs from the
> simulation, which let you play through the whole chain once. The exact commands are in the blue
> book, Chapter 16.

## 16.3 Comparison: the Whole Robot

After the single servo comes the whole robot. The real robot writes down, 50 times per second, what
its sensors report and what commands its network gives. The simulated duck then gets exactly the same
commands, tick by tick, and Kern compares what its sensors report: how it turns, where "down" is for
it, how the joints are positioned and how fast they move. Each joint gets its own RMS error.

> **Picture it like this:** A dancer records her choreography. A second one dances it after her,
> step by step. Where the two drift apart, something's not right with the second one.

Kern checks the method with a log from the simulation itself: with the same servos, the distance is
**exactly 0**, "bit for bit" – meaning down to the very last digit of every number, not just
roughly. With different servos, it's measurably bigger.

**That's the principle: Because Kern is deterministic ([Chapter 14](14-lehrer.md)), every difference
from a real log is a real error in the model and not chance.**

And because each joint gets its own error, the comparison also shows **where** the error is: if the
distance gets smaller everywhere with better profiles, but a single joint stays conspicuous, then
something is missing in the model of **exactly that joint** – gear backlash, for example (16.5).
Like at the doctor: if only one knee hurts, they examine the knee, not the whole body.

## 16.4 Training with Your Own Profile

Once you've measured the profile of your own servos, you retrain the movements with it. The ducks in
training then get exactly these servos, with their spreads. That way the network fits exactly your
robot.

> **For pros:** That applies to movements like the jump. The play layer (the network for playing with
> ball and sock) currently always uses the supplied servo profile. The command is in the blue book,
> Chapter 16.

## 16.5 Variation (Domain Randomization)

Whatever can't be measured, training rolls the dice on ([Chapter 14](14-lehrer.md)). Every practice
round gets a slightly different duck in a slightly different world. For the network, the real Micro
Duck is then just one more variant.

> **Picture it like this:** If you only practice soccer on perfect grass, you'll stumble on a dirt
> lot. If you've practiced on grass, asphalt and mud, you can handle anything.

| What's uncertain | How training covers it |
|---|---|
| servos, each a bit different | friction and momentum ±10% |
| delay in the cable | 15 to 30 ms |
| build (battery, cables) | body a bit heavier, center of mass shifted |
| floor | soles sometimes more slippery, sometimes grippier |
| motion sensor, angle sensor | noise, offset, delay |
| shoves | a push every 3 to 6 seconds |
| camera | imprecise position, sometimes no image at all |
| toys | balls of different sizes and weights |

The **motion sensor** (technical term: IMU) senses where down is and how the duck turns. The **angle
sensor** (encoder) reports how a joint is positioned. **Noise** means: the values jitter a bit
around the true value.

The coach in learning, the **critic**, sees the clean truth. Only the network that later goes onto
the robot gets the noisy sensor values. That way it learns with what it'll have in reality too.

There's one gap: **gear backlash**, the little wobble in the gearbox when the gears don't mesh quite
tightly. It's missing from Kern's model. Pollen's walking network velstand was trained with gear
backlash and copes with it; Kern's own networks don't know it yet. On the real robot, that's a risk.

> **Mission:** In the Micro Duck level, start **Simulate** (Alt+S), drag the duck sideways by its
> head with the mouse until it tips over, and then let go. Watch what happens next: it gets back up
> by itself. That works because velstand was constantly pushed in training and practiced with lots of
> deviations.

## 16.6 Measuring in the Physics Instead of Typing In

Some numbers follow from the model and the physics. Kern doesn't type them in by hand but **measures
them in the simulation**, just as you would on the real robot. The reason: a typed-in number is no
longer right as soon as something changes, such as the friction or the beak. A measured one you
simply measure again.

**Example bite position:** Where does a sock have to lie in front of the duck for the beak to grip it
safely? Kern puts it down at 99 spots, tries picking it up each time, and takes the spot where it
works most reliably. No bite position is written into the program as a fixed number.

The same goes for:

- **"Is holding something"** Kern reads off the angle sensor of the beak servo: it's supposed to be
  closed but is still open? Then something is stuck in between.
- **"Standing still"** means: both soles touch the ground. The physics measures that through the
  contacts.

Two things are still typed in by hand: how far the duck keeps walking after "stop," and how strongly
different materials rub against each other. Both are candidates for measurements on real material.

> **Mission:** Start **Simulate** and switch on **Window › Show › Contact Points**. You see where
> the soles touch the ground – that's exactly how the **play layer** (the network that decides what
> the duck does with ball and sock) recognizes whether the duck is standing.

## 16.7 Limits

**To be completely honest: there's no real measurement data yet.** Everything in this chapter has
been checked with logs from the simulation. As soon as a real test bench measures, the same tool
works with real logs.

Still open:

- **Gear backlash** is missing from the model (see 16.5).
- **The motor's momentum** is rebuilt in a simplified way.
- **One kick** of the duck sometimes tips over in Kern. A comparison with the real robot is
  supposed to clear up why.
- **Motion sensor, friction between materials and camera** haven't been measured on the real
  device yet.

> **For pros:** Internally, Kern calculates with a duck four times as big
> ([Chapter 13](13-micro-duck.md)). Profiles and logs are still in real units; conversion only
> happens at the boundary to the physics.

> **Mission:** If the level is running, stop it with **Esc**. Select the actor "Micro Duck," set
> **Actions to servo goals** in the **Details** panel to `robot`, meaning **As the robot's daemon
> (scale 0.9, low-pass 0.5 / 0.7)**, and start **Play**. That's how Pollen's software drives the
> real robot: it makes the network's commands a bit smaller and smooths them. Take over the duck
> with **F8** and press **3** for the kick. Then the same with `training`, meaning **As trained
> (scale 1, no filter)**. With `robot` the kick tips over, with `training` it stays standing. Even
> such a small difference changes the behavior. That's why it's worth comparing simulation and
> reality carefully.

## Level complete!

- You work on the real robot only with an adult.
- You can describe a servo as a spring, a motor with a force limit, a shock absorber and friction.
- You know what spreads are for: uncertain numbers get rolled in training.
- You know the test bench: clamp, measure, adjust, check with a new measurement.
- You can calculate an RMS error (calculator allowed).
- You understand why determinism makes the comparison so powerful.

## Puzzle corner

1. What do you do before you connect a real servo?
2. Calculate the RMS error for the distances 0°, 0°, 0° and 10°. Compare with the normal average.
3. Why does the check measurement have to use a different weight from the measurements used for
   adjusting?
4. One servo is measured very precisely; for another you're unsure. Which one gets the bigger spread
   in the profile, and why?
5. **Boss puzzle:** The comparison shows 10 times more error for one joint than for all the others.
   What do you conclude?

### Solutions

1. Get an adult, clamp the test bench, limit the current, have the emergency stop ready.
2. Squares 0, 0, 0, 100; average 100 / 4 = 25; √25 = 5°. The normal average would only be 2.5°: a
   big outlier counts strongly in the RMS error.
3. Only that way does it show whether the model explains the servo or just knows the curves by
   heart.
4. The uncertain one. Then training rolls more dice there, and the network doesn't rely on a number
   that nobody knows exactly.
5. Something is missing in the model of this joint (gear backlash, for example). It can't be chance,
   because the simulation is deterministic: it always calculates the same way. And because the error
   is only that big for this one joint, the gap is there and not in the whole model.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
