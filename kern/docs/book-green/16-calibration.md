# 16 Calibration: From the Computer into the Real World

> **In short:** A network from the Lehrer is supposed to run on the real Micro Duck. For that, the
> simulation has to match the robot well, and the network must not rely on anything the simulation
> doesn't know precisely. To get there, Kern measures the motors, compares the whole robot with the
> simulation, and trains for the rest under many slightly different conditions.

> **Key terms:**
> - **Servo** – a small motor that moves to a given angle.
> - **Network** – the duck's learned calculating machine: it gets measurements and answers with
>   targets for the servos.
> - **Lehrer** – Kern's training system, where the duck's networks learn ([Chapter 14](14-lehrer.md)).
> - **Play layer** – Kern's own network that decides what the duck does with ball and sock.
> - **Simulation** – the rebuilt duck inside the computer, which runs by the rules of physics.
> - **Episode** – one practice run in training, from the start until a fall or until time is up.
> - **Spread** – how far a value may vary, such as "plus or minus ten percent."
> - **Deterministic** – given the same inputs, the simulation always calculates exactly the same thing.

Kern pursues three approaches and one principle:

1. **Device models with profiles:** a servo gets a small calculation model, described by a few
   numbers. You fit these numbers to measurements and check them against other measurements.
2. **Matching:** a recording of the real robot is replayed in the simulation; the difference is the
   distance still missing to reality.
3. **Variation:** whatever no measurement pins down exactly, training rerolls in every episode.
4. **Principle:** whatever follows from the model and the physics, the Lehrer measures in the
   simulation instead of writing it in as a fixed number – such as where the sock has to lie for the
   duck to grab it (16.6).

Nothing is changed in the physics engine itself (Box3D, [Chapter 6](06-physics.md)). It might be
tempting to bend the physics until the duck fits – but then it would no longer be right for everything
else in the level. That's why the physics stays the same for all things, and whatever a motor does on
top comes as a building block of its own, just as a motor is screwed onto a joint.

> **Imagine …** you practice on a keyboard and tomorrow you'll play on a real grand piano. You can
> check how heavy its keys are (device model), compare a recording of the piano with your practice
> (matching), and practice on many different instruments (variation).

> **Watch out!** This chapter also describes measurements on real hardware. Servos under load can
> swing out fast and hard. Clamp the test bench firmly, secure the arm and weight, keep hands and
> cables out of the swing range, limit the current, have an emergency stop within reach. Never exceed
> the manufacturer's operating voltage, and charge lithium batteries only with a suitable charger and
> under supervision. Test new networks first on a robot that's being held or hung up. Everything at
> your own risk (see the [imprint](00-imprint.md)).

## 16.1 The servo model and its profile

A servo isn't rebuilt in the computer gear by gear. Kern only describes **how it feels**: how firmly
it pulls, how it brakes, how sluggishly and how late it reacts. These few numbers together are called
a **profile**. A different servo is simply a different profile.

The easiest way to picture the servo is as a spring and shock absorber between the joint and the
target angle, plus friction, a flywheel and a small delay:

![The servo as a spring and shock absorber between target angle and joint](../abbildungen/servo-modell.svg)

| Property | Everyday picture | What it does |
|---|---|---|
| Stiffness | a spring pulling toward the target | the farther away, the stronger the pull |
| Force limit | a person who can only pull so hard | far away, it doesn't pull any harder |
| Damping | the shock absorber on a bike | fast movements get braked |
| Friction | a stiff hinge | small forces don't move anything at first |
| Armature | a heavy flywheel | hard to get going and hard to stop |
| Delay | a command delivered by messenger | the servo reacts a few hundredths later |

The **armature** needs one more sentence: inside the servo, a small motor turns very fast and drives
the joint through a gearbox. This nimble motor has to be accelerated along with every movement, which
is why the joint feels heavier than its parts weigh.

The same model calculates in the Studio, in the Lehrer, and on the test bench you'll meet in 16.2; so
whatever is measured in one place applies everywhere. A fundamentally different device would get a
model of its own.

You don't start at zero: Kern ships with a finished **standard profile**, the values Pollen Robotics
determined for the Micro Duck's servo. Your own measurements only refine it for your servos.

All numbers in the profile are in real units, just as you measure them on the real servo. The fact
that the duck in the computer is four times as big ([Chapter 13](13-micro-duck.md)) is converted by
Kern only at the very last moment: exactly when it hands the servo's force over to the physics engine. That's why a profile you measured on the real servo applies in the
simulation too, without any conversion.

Every number comes with a **spread**: an honestly admitted uncertainty. In every episode, training
draws values of its own from it for each servo. A well-measured servo gets a small spread, a poorly
known one a large spread. That way the network doesn't depend on numbers nobody knows exactly.

The exact values and the profile's file format are in the blue book, Chapter 16.

> **Try it!** In Kern Studio at https://kernengine.de/, open **File › Example Levels › Micro Duck**,
> start **Play** and **don't** take over the duck: you watch with the observer camera and can grab
> bodies. Grab the standing duck by one foot with the mouse, pull it carefully a little to the side,
> and let go. The leg returns to its posture, but softly and without snapping back hard: the spring
> pulls it toward the target, the shock absorber brakes the return. Exactly these two numbers (how
> firmly, how strongly braked) are what calibration measures. Pull too far, and the duck tips over;
> then it gets back up by itself.

## 16.2 The test bench: measuring a servo

Where do a profile's numbers come from? You measure them. For that you clamp a single servo in place,
hang a lever arm with a weight on it, send it movement commands, and record what it really does. This
part is aimed at tinkerers with real hardware; to understand it, knowing the principle is enough.

### How to measure

1. **Setup:** clamp the servo firmly, axis horizontal, an arm with a weight on it; measure the weight
   and lever length precisely. At rest, the arm hangs straight down like a clock pendulum.
2. **Run it as in the robot:** the same settings and the same voltage as in the duck.
3. **Send different commands** and read the angle at least 100 times per second: small jumps (about
   10 degrees) and large ones (about 50 degrees), a back-and-forth that keeps getting faster (a
   **chirp**, like rising birdsong), and a steady jumping back and forth between two angles.
4. **Try two or three loads.** Set aside at least one measurement per servo; it's used later only for
   checking.
5. **Measure several servos.** How much they differ becomes the spread in the profile.

### Replaying the measurement in the computer

The recording (the **log**) is a simple text file: for each moment, the commanded and the measured
angle. Kern now rebuilds the same servo with the same load in the simulation and sends it the same
commands.

What matters here: the rebuilt test bench calculates with exactly the same settings as the duck in
the Studio – the same enlargement scale, the same number of physics steps per second of world time,
equally soft joints. Otherwise a profile that fits on the test bench would no longer fit in the
duck – like pants you tried on with thick winter boots that fit differently with sandals.

Two curves come out:

![Measured and simulated curve after the same command; the distance between them is the error measure (curve shape schematic)](../abbildungen/kalibrierung-kurven.svg)

The better the profile, the more closely the curves overlap. Kern sums up the distance in **one
number**: the average distance between the two curves, with big outliers counting more. Experts call
this the **RMS error**.

> **Try it!** You can see in the Studio how strictly Kern insists on identical settings. In the Micro
> Duck level, under **World Settings**, set the **Step rate**, the number of physics steps per second
> of world time, to 90 instead of 100 and start **Play**. Kern reports in the **Output Log**:
> "set World Settings > Step Rate to 100." You have to set it back yourself. Everything – training,
> measuring, playing – calculates with the same step rate: 100 steps per second of world time = 200 in
> duck time (13.5). With 90 it would be a different world, and no profile and no network would be
> tuned to it anymore. Afterward, set it back to 100.

### Fitting and checking

Now the computer searches for the profile values at which the simulated curve best matches the
measured one. It turns six adjusting screws: the servo's five properties and the delay. The search
uses **CMA-ES**, the search method from [Chapter 14](14-lehrer.md): it tries out a group of
candidates, moves toward the better ones, and keeps trying near them.

> **Imagine …** twelve guitars with six tuning pegs each, each tuned a bit differently. CMA-ES
> listens for which ones sound best and tunes the next twelve similarly to the winners.

What counts afterward is the **check error**: the distance on the measurement that was set aside.
Only it shows whether the profile really explains the servo or just knows the curves by heart – like
a class test with new problems instead of the practiced ones. That's why the check measurement should
have a different signal and a different load.

> **Did you know?** Kern tests the procedure with a deliberately "detuned" servo: it pulls more
> weakly, has more friction, and reacts later than the starting profile. After fitting, the
> simulation matches even the check measurement almost exactly – what remains is under a fifth of a
> degree.

Without a real servo, you can try all the steps once with example measurements from the simulation.
The commands for that are in the blue book, Chapters 16 and 17.

## 16.3 Matching: the whole robot

The test bench measures one servo. **Matching** checks the whole robot: you take a recording of the
real duck – what it measured at each moment and which commands its network gave – and replay the same
commands in the simulation. Then you compare what the real and the simulated duck measured.

> **Imagine …** two musicians play from the same sheet music, one on stage, one in the rehearsal
> room. You lay the two recordings on top of each other. Where they drift apart, something is off
> with the instrument in the rehearsal room.

The simulated duck starts still in its base pose, without variation, with the servos from the profile,
and gets the commands from the recording 50 times per second of duck time, exactly as the real robot
got them. What gets compared is what its sensors report – how the body is turning, where "down" is,
how the joints are positioned and how fast they're moving – for each joint separately.

If one joint stands out, something is missing in the model there. One candidate is **gear backlash**:
a bit of wobble in the gearbox before the gears engage. Kern's servo model doesn't know about that yet.

The nice thing: if you replay a recording **from the simulation** with the same servos, the distance
is **exactly zero**, down to the last digit, because the simulation is deterministic. With different
servos there's a measurable distance. So the simulation doesn't roll any dice when matching; chance is
ruled out as a cause. That's why every difference from a real recording is a real error in the model.

How the real robot writes such a recording isn't described in this book; the command for matching is
given in the blue book, Chapter 16.

> **Try it!** You can see in the Studio how much small differences matter. Open
> **File › Example Levels › Micro Duck** and select the "Micro Duck" actor in the **Outliner** (or
> click right on the duck's torso). In the **Details** panel, under **Player Controller › Walker**,
> there's the **Actions to servo goals** field. Leave it at **As trained (scale 1, no filter)**,
> start **Play**, take over the duck with **F8** and kick with **3**. You don't have to hit the ball –
> just watch whether the duck stays standing after the kick. Stop with **Esc**, set the field to
> **As the robot's daemon (scale 0.9, low-pass 0.5 / 0.7)** and kick again. This setting weakens and smooths the commands the way the
> real robot gets its commands – and now the kicks tip over. That isn't matching yet, but it shows
> why you need it: the same network, just a slightly different path to the motor, and the result is
> different. Matching is meant to track down such differences between simulation and robot.

## 16.4 Training with your own profile

Once you've measured your servos, you train new networks with exactly that profile, including its
spreads. In every episode the ducks get servos that differ slightly from each other within the spread.
That way the network learns to cope with every individual unit. Only in the test at the end – on an
undisturbed duck without variation – does the profile apply exactly.

> **For the curious:** Your own profile affects the training of movements, such as the jump. The play
> layer currently always practices with the standard profile that ships with Kern. The command for
> this is in the blue book, Chapter 16.

## 16.5 Variation: practicing under changing conditions

Whatever hasn't been measured, or varies from duck to duck, training deliberately makes vary. Every
episode gets slightly different motors, a slightly different floor, slightly less reliable sensors. A
network that copes in all these worlds also copes with the real one – which is then just one more
variant. Experts call this **domain randomization**.

> **Imagine …** you practice table tennis sometimes with a heavy ball, sometimes with a light one, on
> slippery and on grippy floors. On tournament day, you don't care what the hall is like.

| What's uncertain | How training covers it |
|---|---|
| every servo is one of a kind | every servo a bit different, within its spread |
| delay on the data cable | commands sometimes arrive earlier, sometimes later |
| build (battery, cables …) | torso sometimes a bit heavier, center of mass a bit shifted |
| floor | sometimes smoother, sometimes grippier |
| tilt and joint sensors | noisy (jitter randomly), lag behind, are slightly off |
| disturbances | a random shove now and then |
| camera (looks for ball and sock) | inaccurate position, sometimes it sees nothing at all |
| toys | balls sometimes bigger, sometimes smaller, sometimes heavier |

One trick here: the **critic** – the second network, which during training only judges how good a
situation is – always sees the clean truth. Only the acting network, which later goes onto the robot,
has to cope with noisy sensors. The critic stays behind in training, so it's allowed to "cheat."

The exact ranges are in the blue book, Chapters 14 and 16.

> **For the curious:** Pollen Robotics varies gear backlash in its own training. That's why Pollen's
> walking and standing network velstand copes with wobbly gearboxes. Kern's own networks learn without
> it so far.

> **Try it!** Start **Play** in the Micro Duck level without taking over the duck. You watch and can
> grab bodies with the left mouse button. Grab the ball, drag it to the walking duck, and let go so
> that it bumps into the duck. Watch how it reacts. Random shoves like this are part of what gets
> varied in training – so a bump doesn't take the duck by surprise.

## 16.6 Measuring in the physics instead of entering numbers

Some numbers follow from the physics. If you wrote them in as fixed values, they'd no longer be right
after the next change to friction, model or servo. So Kern measures them in the simulation, just as
you'd measure them on the real robot.

> **Imagine …** a recipe says "bake for 20 minutes." With a new oven, that's no longer right. Better
> is "bake until a toothpick comes out dry" – that works in any oven.

- **The bite position:** where does the sock have to lie for the duck to grab and hold it when picking
  up? Before the first training, the Lehrer tries out 99 spots. It takes the spot that, together with
  its neighboring spots, works most reliably – so that it still works even if the sock is lying a
  little off.
- **"Holding something in the beak":** the beak servo is set to "closed" and still stands a bit
  open – then something is stuck in between.
- **Standing still:** the play layer is continuously told how long the duck has been standing still –
  that helps it decide what to do next. What counts as "standing" is what the legs do: both feet on the
  floor. "There's no walking command" would be the worse rule: the duck sometimes doesn't take a step
  at all in response to a very small walking command. It's then standing even though there's a
  command – and where that threshold lies is determined by the physics.
- **Exception, the overrun after "stop":** how far the duck keeps going when stopping was measured
  once and then entered as a fixed value. After a change to the physics, it has to be measured again.

Entered as fixed values, on the other hand, is the friction between materials, such as fabric on a
wooden floor. These values come from typical tables and are waiting to be measured on the real
material.

## 16.7 Limits and next steps

To be honest: so far, everything has only been tested with measurements **from the simulation**; real
measurements are still missing. As soon as they exist, the same tools work with them. In addition:

- **Gear backlash** is missing. If matching shows it's needed, it goes into the servo model.
- **A known wobble:** after the right kick, the duck sometimes tips over in Kern. Possible reasons are
  wobble in the gearbox, feet that touch down slightly too softly on the floor in the computer, or the
  foot friction – a typical case for matching against a real recording.
- **Tilt sensor and camera** don't have measured profiles of their own yet; their noise is so far an
  assumption of the training.

Other small simplifications that have no effect in the example level are described in the blue book,
Chapter 16.

## What you take away

- Three approaches to the real robot: measure device models, match against recordings, practice
  under variation.
- A servo is a profile: a spring with a force limit, a shock absorber, friction, a flywheel and a
  delay, plus honest spreads.
- The test bench measures a servo; the computer tunes the profile to it. What counts is the error on
  a measurement that wasn't used for fitting.
- Test bench, training and game calculate with the same settings; otherwise a profile would only
  apply in one place.
- Matching shows joint by joint where simulation and robot drift apart.
- Because the simulation always calculates exactly the same way, every difference is a real model
  error.
- Whatever can be measured, Kern measures instead of entering it.

## Brain teasers

1. Why isn't it enough to look only at the error on the measurements the profile was fitted to?
2. A servo is very well measured. Should its spread be large or small?
3. A recording from the simulation is matched with the same servos. What distance comes out, and why?
4. Why is it risky to write the bite position into the code as a fixed number?

### Solutions

1. Because otherwise the profile could just learn the curves by heart. Only a new measurement shows
   whether it really explains the servo – like a class test with new problems.
2. Small. Large spreads are for values you don't know exactly.
3. Exactly zero, down to the last digit, because the simulation always calculates exactly the same
   thing given the same inputs.
4. After the next change to friction, model or servo, it's no longer right. Measured, it always fits
   the current physics.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
