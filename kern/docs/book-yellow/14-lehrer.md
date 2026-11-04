# 14 The Lehrer – a Training Gym for the Duck

> **Your level:** You're looking into Kern's own training gym. Dozens of ducks practice there at the
> same time, until random numbers have turned into a network that makes the Micro Duck jump. You'll
> find out how the Lehrer works and why it builds the same network down to the last digit on every
> device.

> **Words you need:**
> - **Network** – a calculating machine made of a huge number of numbers, like a mixing desk with
>   countless sliders
> - **Reward** – points the duck gets for good movements and loses for bad ones
> - **Noise** – a little bit of chance that's added to every movement while learning
> - **Episode** – one attempt from start to finish, until a fall or until time is up
> - **Policy** – the duck's decision rule: what do I do when I measure this?
> - **Actor** – the player: the network that decides and later goes into Studio
> - **Critic** – the coach on the sideline: estimates how many points are still to be expected
> - **Iteration** – one training round: all ducks try, then the Lehrer learns from it

The **Lehrer** (German for "teacher") is Kern's own training program for the Micro Duck. It teaches
it new movements, for example a jump. It runs on a big computer and just the same in the browser,
on computers and phones.

The most important thing first: the duck in the Lehrer is **the same** as in Studio – the same game
world, the same servos and sensors, the same physics ([Chapter 13](13-micro-duck.md)). That's why a
network that learned here runs in Studio without any change. On the real robot, a network you
trained yourself isn't tested: anyone who loads it onto hardware does so at their own risk – and
only together with an adult ([Chapter 16](16-calibration.md)).

That's how Kern's two own networks came about: the **jump** and the **play layer** (ball and
sock). How you train one yourself is in [Chapter 15](15-your-own-models.md).

> **For pros:** Simulation, learning and export are written in C++ and run without Python and
> PyTorch, which are what people usually use for training. In the browser, the same code runs as
> WebAssembly.

## What You Need to Know for This

The words from the box above are connected. Here you'll see how; [Appendix A](A-neural-networks.md)
explains it in even more detail.

**The network is a mixing desk.** A neural network is a calculating machine made of a huge number
of numbers; picture them as sliders on a gigantic mixing desk. In the front goes what the duck
measures (tilt, joint angles …), and out the back come 14 numbers: where each servo should turn. At
the start, the sliders are set randomly, and nonsense comes out. **Learning** means: turning each
slider a little bit in the right direction, again and again.

**Learning through reward.** Nobody tells the duck which movement is right. It tries, and a rule
hands out **points**: plus for staying upright, minus for falling over. Its goal is to collect as
many points as possible **in total** in one attempt – including the ones that only come later. So
that it discovers anything new at all, a bit of **chance** is added to every movement while
learning: the **noise**.

**Episode.** One attempt from start to finish is called an episode. It ends when the duck falls or
time is up (for the jump, after 2 seconds). Then a new one begins.

**Policy.** The policy is the duck's decision rule: what do I do when I measure this? That's the
network with its chance. Learning means improving the policy.

**Player and coach (actor and critic).** While learning, there are two networks. The **actor** is
the player: it decides what the duck does. The **critic** is the coach on the sideline: it estimates
how many points are still to be expected from a situation. If things went better than estimated,
the movement was good. Only the player is allowed onto the field, meaning later into Studio and onto
the robot.

> **Watch out, trap:** Don't confuse this with the **actor** in Studio ([Chapter 11](11-studio.md)),
> meaning a thing in the level. The same word means something completely different here: the
> network that acts. And the "coach on the sideline" is just a picture for the critic – the whole
> training program is called the **Lehrer**.

## 14.1 Overview

The Lehrer teaches the duck a jump in three steps, the way you might learn a trick on a skateboard:

1. **Find a role model:** A search method tries out lots of jumps until one works.
2. **Copy:** The network learns to imitate this role model.
3. **Practice:** The network keeps practicing by itself and collects points until the jump is solid –
   even with a slightly different duck.

What's already done:

| Part | Status |
|---|---|
| Simulation, network, learning by reward, saving as a file | done |
| Variation, learning in the browser, same result everywhere | done |
| Jump: role model → copy → practice | done |
| Measuring the real robot's servos | tool done, real measurements still missing |
| Playing with ball and sock | being reworked |

What "variation" and "same result everywhere" mean is explained in 14.3 and 14.7, and the measuring
in [Chapter 16](16-calibration.md). The files and program parts are in the blue book, Chapter 14.

## 14.2 Architecture

The Lehrer splits the work into five roles. That way the roles can run in different places: on
many **cores** of one computer – a processor has several of them, and each can calculate on its own
– or in several helpers in the browser (14.11).

> **Picture it like this:** a sports school. The **training plan** (the recipe) sets what's
> practiced and how. The **scoring rules** (the task) say what earns points. In each **gym** (the
> environment), one duck works out. **Helpers with clipboards** (the collectors) write everything
> down. The **analyst at the desk** (the learner) reads all the notes and writes new instructions –
> the new policy. The helpers get those for the next round.

![Training plan, scoring rules and gym; the analyst gives the new policy to the helpers, and the helpers bring notes back](../abbildungen/lehrer-rollen.svg)

One such round – all ducks try, the analyst learns from it – is called an **iteration**. A training
run has hundreds to thousands of them.

The training plan is the same everywhere, on the computer as in the browser. That's one of the
conditions for the same network coming out everywhere (14.7).

> **For pros:** Collectors and learner pack their data into simple rows of numbers that can be sent
> between cores or browser helpers. That's why the whole training loop is only a few lines long;
> it's in the blue book, Chapter 14.

## 14.3 The Duck's Environment

### One Step Is One Control Tick

The environment is one duck in its own little world, without a picture – it's only calculated. Each
step is a **control tick** of 20 milliseconds, so 50 steps per second. In every tick, the same cycle
runs:

1. **Look:** The duck measures how it's standing right now: tilt, joint angles, foot contact.
2. **Decide:** From that, the network calculates 14 servo targets.
3. **Move:** The servos turn, the physics calculates on. Every now and then the duck gets a little
   push.
4. **Points:** The task checks the new situation, hands out points and says whether the episode is
   over.

Then the cycle starts again.

> **Picture it like this:** like riding a bike. You feel that you're tipping, steer against it a
> bit, feel again – except the duck does that 50 times per second.

### The Task

The **task** is the scoring rules: how an episode begins, what earns points and when it's over.
Every point item has a **name**, such as "upright" or "fall." That way you can later look up for
each item separately where the points came from (with the command-line program,
[Chapter 15](15-your-own-models.md)).

The points are handed out per tick, in such a way that **time** matters. For this, every item has
a **weight**: a number that says how much this item counts (this has nothing to do with the weights
in the network from [Appendix A](A-neural-networks.md)). A tick lasts 20 milliseconds, which is
**0.02 seconds**. Let's work out the item "upright" with weight 2: if the duck stands perfectly
straight, each tick brings weight × tick length = 2 × 0.02 = 0.04 points. A second has 50 ticks, so
50 × 0.04 = **2 points per second**. A fall, on the other hand, counts once, all at once.

Two tasks are built in. **Stand** is the quick test: stay standing for a short time, points for
staying upright, for the home pose and the right height, a small deduction for fidgeting; a fall
costs points and ends the episode. **Jump** comes in 14.6.

### The Asymmetric Critic

Actor and critic do **not** see the same thing – that's why it's "asymmetric," meaning unequal. The
actor only sees what the robot's sensors deliver, with all their measurement errors. Only the actor
later goes onto the robot. The critic exists only while learning. It gets the measurements without
errors, plus things no sensor measures, for the jump for example the true height of the body and
whether the duck has already been in the air. That way it estimates more precisely, and its
judgments get better.

> **Picture it like this:** The player on the field only sees what's happening in front of her. The
> coach on the sideline has the slow-motion replay. That makes the coach's tips better – but she's
> the one who has to play.

### Variation

The real robot is never exactly like the simulation. Sometimes a servo is a bit stiffer, sometimes
the floor is more slippery. That's why the Lehrer builds a **slightly different duck** for every
episode. This is called **variation** (technical term: domain randomization).

| Area | What changes |
|---|---|
| Start | facing direction, slight tilt, slipperiness of the floor |
| Build | weight and center of mass of the body |
| Servos | friction, delay |
| Sensors | small measurement errors (actor only) |
| Shoves | a push every now and then |

A network that can only stand with **one** mass and **one** floor won't stand on the real robot.
With variation, it learns not to rely on anything the simulation doesn't know exactly.

The pushes only come once the duck can already do something – like in sports: first you practice
without an opponent, later one joins in. So the exercise gets harder over time. Something like that
is called a **curriculum**. At the end, every network is measured twice: on the **clean** robot
without variation, and with **full** variation. The exact ranges are in the blue book, Chapter 14.

### Chance That Repeats

The chance in the Lehrer is a special kind of chance: it repeats. A computer doesn't really roll
dice; it calculates random numbers from a **starting number**, the **seed**. Same seed – same
sequence of numbers, every time.

> **Picture it like this:** a die that always rolls the same sequence when you tell it the same
> number. With number 1 you might get 4, 2, 6, 6, 1 … and next time with number 1, exactly 4, 2, 6,
> 6, 1 again.

Every duck gets its own sequence of numbers, worked out from the seed and its number. That's why
every run can be repeated exactly – and a bug that happened once happens again next time and can be
found.

> **For pros:** The random number generator is called splitmix64. The formula for each duck's
> starting number is in the blue book, Chapter 14.

## 14.4 The Network and Its Helpers

The jump network is a mixing desk with several **layers**: the measurements go through three
in-between layers, then the 14 servo targets come out. How a single calculation node calculates and
how the sliders get turned is shown step by step in [Appendix A](A-neural-networks.md). Three
helpers work together with the network.

**The converter (normalizer).** The duck's measurements come in very different sizes: a joint angle
might be 0.3, a rotation speed 25. Before the network calculates, the converter brings them all to a
similar size. To do this, it keeps track of how big each value normally is.

> **Picture it like this:** In a class with grades from A to F and a test with 0 to 100 points, you
> can only compare the two once you convert them to the same scale.

**The warm start.** The Lehrer doesn't start from zero. It takes the finished walking and standing
network from Pollen Robotics (velstand, [Chapter 13](13-micro-duck.md)) together with its converter
as a starting point – a network that can already stand. But the jump needs a new input that
velstand has never seen: how far along the jump already is (14.6). Its sliders start at 0. So the
new network first acts exactly like the old one and only gradually learns to use the new input.

**The slider turner (Adam).** A clever slider turner called Adam turns the sliders while learning.
It picks a suitable step size for each slider and has some momentum, like a rolling ball. In front
of it sits an **emergency brake**: if a calculation ever says "turn everything really far," the step
still gets cut down to a fixed maximum length. That way a single outlier can't destroy the network.
[Appendix A](A-neural-networks.md) explains both with pictures.

> **For pros:** The exact form of the layers, the converter's formula and Adam's settings are in the
> blue book, Chapter 14.

## 14.5 PPO Step by Step

**PPO** is the name of the method the duck uses to learn through reward – the same one Pollen
Robotics uses to train the Micro Duck's networks. The idea in one sentence: **get better in small,
careful steps.**

> **Picture it like this:** You're seasoning a soup. After each taste you add a pinch of salt, not
> half the shaker. Because what you tasted applies to the soup as it was a moment ago. If you dump
> in too much, the soup is ruined, and every taste after that only tastes of salt.

It's the same with the duck: everything the helpers wrote down comes from the **old** policy. But
the analyst learns **several times in a row from the same notes** – that saves computing time,
because collecting new notes is expensive. With each pass, the policy moves further away from the
one that wrote the notes. If it changes too much, the notes don't fit anymore – and a good duck can
unlearn everything in a single step. That's why PPO needs a brake.

An iteration goes like this:

1. **Try:** Each duck does a series of ticks. In every tick, the network names its wished-for
   target, and some noise gets added. That way each duck tries slightly different movements.
2. **Sort:** The analyst puts all notes into a **fixed order**, tick by tick, duck by duck. Why that
   matters is shown in 14.7.
3. **Judge:** For each movement, it asks: did things go **better or worse than expected** after it?
   Expected means: as many points as the critic had estimated. The difference is called the
   **advantage**.
4. **Improve:** Movements with a plus become a bit more likely, those with a minus a bit less likely
   – but never by much at once.
5. **Repeat** with the new policy.

An example for step 3: the critic estimates that the duck will still get 5 points from here. It gets
7. Advantage: 7 − 5 = **+2**; the movement was good and becomes a bit more frequent. If it only gets
3, the advantage is −2, and the movement becomes rarer.

How careful "never by much at once" is, Kern controls in three ways:

- Per iteration, a movement may become at most about a fifth more or less likely. Example: if the
  old policy chose a movement in 10 out of 100 cases, from about 12 out of 100 it's no longer worth
  it for learning to make it even more frequent – no matter how good it was.
- If the policy has already changed a lot, the next step automatically gets smaller. The size of
  the steps is called the **learning rate**.
- A small **curiosity bonus** makes sure the noise doesn't disappear too early and the duck keeps
  trying out new things.

One more detail that makes sense: if an episode ends with a **fall**, no more points come after
that. If it ends because **time is up**, it actually would have gone on – then the critic estimates
how many points would still have come. Running out of time isn't failure.

> **Crazy but true:** 32 ducks × 48 ticks = 1536 ticks per iteration. Each tick is 0.02 s, so per
> iteration the Lehrer collects 1536 × 0.02 s ≈ 31 seconds of duck life.

> **For pros:** 48 ticks are shorter than one jump episode (2 seconds = 100 ticks). So an iteration
> is only a **slice**: the episode doesn't stop but simply continues in the next iteration. Only a
> fall or the end of the time starts it over.

**Critic first:** After copying (14.6), the player is already good, but the coach is clueless – it
has never seen the new player before. Its wrong judgments would wreck everything for her. That's why
in the first rounds **only the critic** learns.

**Save game:** The Lehrer can save the whole learning state, like a save game. A training run can be
interrupted and continued exactly where it left off.

> **For pros:** All settings for the jump (how many learning steps per iteration, how far ahead it
> looks, the formulas for advantage and learning rate) are in the blue book, Chapter 14.

## 14.6 Imitation, Role Models and CMA-ES

### Why Learning Through Reward Alone Failed

Pure PPO never found the takeoff, no matter what settings. Three reasons:

- For the takeoff, **all leg servos at the same time** have to push with full force. The servos are
  soft: they only give full force when the target is far from the current angle. So the network has
  to deliberately aim further than the joint can even turn. Through random trying, that practically
  never happens.
- Standing is safe and brings points. A failed attempt costs a fall.
- With only a few dozen ducks, each round's notes are few and random. So the direction of learning
  wobbles a lot, and the brake from 14.5 keeps the steps small. Like a poll with only three people:
  the result jumps back and forth, and you don't dare make much of it.

> **Picture it like this:** You type randomly on a keyboard and hope a poem comes out. One letter
> might be right now and then – but all of them being right at the same time is something you'll
> never see.

That's why: **find a role model → copy → practice.** The role model only exists while learning; in
Studio, the network jumps on its own.

### A Jump in 15 Numbers

The role model is a jump described with only **15 numbers**: where the hip, knee and ankle of the
left leg should aim at five moments – **crouch, push-off, tuck, landing, cushioning**. The right leg
does it mirrored. Before and after the jump, velstand keeps the balance.

The 15 numbers are found by a search method called **CMA-ES**, which works like evolution:

1. It scatters lots of candidates around a center – a **search cloud**.
2. Each candidate jumps several times: once on the clean robot, the other times with variation. In
   one generation, **all candidates jump on the same** random ducks, and every new generation gets
   different ones. That way the comparison is fair – like a race where everyone runs on the same
   track in the same wind. And because the ducks change every generation, no role model wins that
   only works on a few particular ducks.
3. There are points for rising, for flight time and height, for landing on both soles, and for a
   real jump. There are deductions for tipping and for falls – early falls cost more.
4. The cloud moves toward the better candidates. Then the next generation begins.

> **Picture it like this:** A "hot and cold" search game with lots of players. Everyone feels
> around in different spots, and whoever hears "hot!" pulls the whole group a bit in their
> direction.

At the end, the Lehrer takes the **center of the search cloud**, not the best candidate. Why?
Because every rating contains a bit of chance: the best one might just have been lucky. The center
is the average of many good candidates and therefore more reliable. The built-in role model came
about over a few hundred generations.

### Imitation with DAgger

Copying sounds simple: the role model jumps, the network watches and learns. But there's a catch.
The network then only knows situations the role model gets into. If it makes a small mistake itself,
it ends up in a situation it has never seen. There it makes a bigger mistake – and the mistakes
grow. This is called **distribution shift**.

> **Picture it like this:** You don't learn to ride a bike by just watching. You have to ride
> yourself, and someone runs alongside and shouts: "Steer left now!"

That's exactly what **DAgger** does: the network rides itself, and the role model says in every tick
what **it** would have done now.

1. **Round 0:** The role model jumps, and every situation gets labeled with its movement.
2. **From round 1 on:** The network jumps itself. The role model writes down for each situation what
   it would have done.
3. After every round, the network learns from **all** examples so far.

That way, exactly the situations the network itself gets into end up in the collection – with the
right answer. For that, the role model has to have a sensible answer in every situation. After 8
rounds, the network can jump, and then it **practices** further with PPO: with little noise, critic
first, full variation, and extra points as long as it stays close to the role model.

### The "Jump" Task

How does the network know **where** in the jump it is right now? It gets a **phase signal**.

> **Picture it like this:** a clock with a single hand that goes around once in 2 seconds. At the
> start of the episode it points right; after a quarter turn – half a second – it points up. The
> network gets the two coordinates of the tip of the hand, nothing more.

Otherwise there's no help: no script, no forces from outside. The whole movement comes only from the
network's 14 servo targets. An episode lasts 2 seconds. The most important point items:

| Item | For what |
|---|---|
| flying, pushing off | lots of points, but only upright |
| landing | points for landing on both soles |
| imitating | points as long as it's close to the role model |
| upright / tipping | plus for staying upright, deduction for tipping |
| extra hops, sliding | deduction |
| fidgeting | deduction, gets bigger over time |
| fall | big deduction, once, gets bigger over time |

An episode ends immediately on a fall **and on any ground contact of a part that isn't a sole**.

> **Watch out, trap:** Without this rule, the network learns to prop itself up on its beak or tail.
> For the points, that's the safest bet – but it isn't a jump.

**A real jump** means: for at least 60 milliseconds, all parts are in the air, the body rises at
least 5 millimeters, the duck never tips more than 30°, and at the end it stands upright on both
soles. Just pulling up the feet doesn't count.

## 14.7 Determinism

**Deterministic** means: same input, same result – down to the very last digit. With Kern, that
holds on completely different computers and in the browser. What for? It turns a bug in the
simulation into a fixed bug that can be repeated and found, instead of a noise that looks different
on every computer.

That sounds obvious – but it isn't. The reason is **rounding**. Imagine a calculator that only
remembers **three digits** and rounds everything else:

- (100 + 0.4) + 0.4: First 100 + 0.4 = 100.4, rounded **100**. Then 100 + 0.4 = 100.4, rounded
  **100**.
- 100 + (0.4 + 0.4): First 0.4 + 0.4 = 0.8. Then 100 + 0.8 = 100.8, rounded **101**.

The same three numbers, just added in a different order – and a different result! A computer
remembers many more digits, but not infinitely many. At the last digit, exactly the same thing
happens.

> **Picture it like this:** the butterfly. Weather scientists say that a single flap of a wing can
> decide the weather somewhere completely different weeks later – tiny differences grow. Same with
> the duck: one different last digit, and after a few seconds it puts a foot down differently. Then
> it walks a different gait, and in the end a different network comes out.

What Kern does about it:

- **Always round the same way:** Some processors calculate "times and plus" in one go and round only
  once, others twice. Kern forbids the shortcut so that all of them round the same way.
- **Its own math:** Every operating system calculates sine, cosine and friends a tiny bit
  differently at the last digit. Kern brings its own version and uses the same one everywhere.
- **Fixed order:** Each duck gets its chance from its own number, not from the helper that happens to
  be calculating it. And the analyst always adds in the same order – no matter how many helpers are
  calculating along. That's exactly the lesson from the calculator.

That's why: **The same seed and the same number of ducks give the same network** – on one core, on
16 cores or in 4 browser helpers. More helpers only make it faster.

This is checked with a **fingerprint**: a duck walks 400 ticks, and after each tick a check number
is formed from its exact state, similar to a digit sum. These check numbers are compared on all
platforms. If they differ, the first tick that differs shows where two computers drift apart.

> **For pros:** Anyone who writes code for Kern's simulation must follow these rules: Kern's own
> math functions instead of the system's, sums in a fixed order, no "fast math" setting when
> compiling. The exact switches are in the blue book, Chapter 14; how Kern is built is in
> [Chapter 2](02-building-and-testing.md).

## 14.8 Learning the Play Layer

The **play layer** ([Chapter 13](13-micro-duck.md)) is a second, smaller network. It doesn't decide
how the legs move, but **what** the duck does next:

> find a target → go there → stop → kick or pick up → carry → spit out

The walking and the kick itself are still taken care of by Pollen Robotics' networks. The play
layer decides 25 times per second and learns like the jump: role model, copying with DAgger,
practicing with PPO.

In every episode there's a ball, a sock or both in front of the duck, sometimes right in front of
it, sometimes behind it. Balls differ in size, weight and how easily they roll. On top of that comes
variation, for the camera too: it recognizes ball and sock imprecisely and sometimes misses them.
Points come when the duck gets closer to the target, picks up the sock and holds on to it, kicks the
ball away, or spits the sock out again a bit further on. Every second without success costs a
little, a fall a lot.

**The role model mustn't know more than the network.** It only sees the same measurements as the
network and remembers almost nothing. Why? Otherwise the network can't copy it. That would be like a
teacher who looks at the answer key: they know the result, but you can't follow their path. So every
decision of the role model follows from what the network sees right now and from how long it has
already been standing. How to design such rules is in [Chapter 15](15-your-own-models.md), section
15.10.

**Measured instead of written down:** Where the beak grips a sock well, the Lehrer measures anew on
every run: it places the sock at lots of spots in front of the duck and checks where it's still in
the beak after 3 seconds of carrying. No bite spot is written into the program as a fixed number. If
the physics changes, the measurement is still right.

**Result.** Kern's play network was tested in 32 trial episodes, once on the clean robot and once
with full variation. In 11 of them there was only a ball, in the other 21 a sock (sometimes plus a
ball). That's why the ball counts out of 11 and the sock out of 21:

| Result | clean | variation |
|---|---|---|
| Ball kicked away | 9 of 11 | 7 of 11 |
| Sock carried, spit out | 1 of 21 | 0 of 21 |
| Falls | 2 of 32 | 10 of 32 |

The network usually kicks the ball away, but almost never gets the sock to the goal. The **ball is
learned**. The network heads for the sock just as correctly, but the bite rarely holds: the beak
slips off the cotton. So the bottleneck is the **grip in the physics, not the decision**. That's why
the table in 14.1 says: being reworked.

> **For pros:** How big the play network is, how often ball and sock show up, how many points each
> item brings, and the whole results table are in the blue book, Chapter 14.

## 14.9 Results of the Jump

Kern's jump network was trained with 32 ducks, seed 1, and then tested for 40 episodes each:

| Measurement | clean | full variation |
|---|---|---|
| real jumps | 40 of 40 | 36 of 40 |
| strongest tilt per jump (average) | 8° | 9° |
| falls | 0 | 0 |

So the network almost always really jumps, even with a changed duck, and never falls doing it. The
way there: first the Lehrer searched for the role model, once. After that, the network made its
first real jump already during copying. Practice did the rest.

Because everything is calculated the same way everywhere down to the last digit, these results hold
on every device. Only the **time** differs: in the browser, on the same computer, it takes just
under four to five times as long as with the program – but **the same network** comes out.

> **For pros:** The browser is slower because it calculates without certain accelerators. The
> physics could also be calculated with half as many sub-steps ([Chapter 6](06-physics.md)) and
> would then be faster – but the duck's movements would change, and the network wouldn't fit Studio
> anymore. How high and how long the duck flies, and all the times, are in the blue book,
> Chapter 14.

> **Mission:** Open Kern Studio on https://kernengine.de/ and load
> **File › Example Levels › Micro Duck**; if **Play** or **Simulate** is running, first stop it
> with Esc. Start **Play**, take over the duck with F8 and press **7**. That's the jump network from
> this chapter. On a phone, the button is called **HOP**. Watch closely: crouch, push-off, tuck,
> landing, cushioning – do you recognize the role model's five moments?

## 14.10 ONNX Export and Metadata

The finished network is saved as a file in the **ONNX** format. ONNX is like PDF for networks:
completely different programs can open the file – Studio just as well as the real robot's software.
Kern saves its networks in exactly the form Pollen Robotics uses too.

Only the **player** gets saved, meaning the actor, with its converter in front – and only its
wished-for target **without the noise**. The wobble was only there for trying things out while
learning; on the robot it should decide calmly and always the same way. The critic stays at home;
it's only needed while learning. Added to that is an **instruction leaflet** (the metadata): the
names of the joints, the home pose, which task the network can do, and that Kern trained it.

To be safe, an automatic test reads such a file back in and checks that the network calculates the
same as in the Lehrer, down to 0.0001.

> **For pros:** How the file is built inside and exactly which entries the leaflet has is in the
> blue book, Chapter 14.

## 14.11 The Lehrer in the Browser

![The Lehrer page during training: settings, live numbers and the return curve](../buch/bilder/lehrer.png)

You can run the Lehrer yourself in the browser. The home page https://kernengine.de/ links to it. To
make it fast, the page splits the work across several **helpers** (technical term: Web Workers). A
helper is a part of the program that calculates in the background, and each one can use its own
**core** of your processor.

> **Picture it like this:** A processor with several cores is like a kitchen with several cooks.
> Each cook can chop something different at the same time.

Each helper is a **collector** with its own group of ducks. One more helper is the **analyst**: it
has the network copy, learns with PPO, checks the jump every 25 iterations, and saves the network.

Just like Studio, the Lehrer page speaks English or German: it follows your browser's language,
and the **DE | EN** button at the bottom right switches. You set: which movement (**Motion**:
**Jump**, or **Stand (quick test)**), how many helpers (**Workers** on the page), how many ducks per
helper (**Ducks per worker**), how many **Iterations**, and the **Seed**. Two more fields,
**Initial action noise** and **Learning rate**, can be left empty (they say **Recipe**); then the
training plan applies. The page
shows you what's happening, as numbers and as a curve. What the numbers mean is explained in
[Chapter 15](15-your-own-models.md).

Every 25 iterations, the page saves the network **on your device**, and Kern Studio in the same
browser picks it up right away. With **Download .onnx** you get the file. The
play layer can't be trained here, only jump and stand.

> **Mission:** Open the Lehrer, choose **Stand (quick test)** and press **Start**.
> Do the math: **Workers** × **Ducks per worker** = ducks in total. How many steps per second does
> your device manage? That's the **Steps / s** display.

> **Mission:** Test the determinism. Choose **Stand (quick test)**, leave the **Seed** at 1 and enter
> **20** under **Iterations**. Press **Start** and wait until the page reports "Done". Write down
> the number at **Mean episode return**. Reload the
> page, enter 20 again, change nothing else, and start once more. Is the number exactly the same?
> Then try seed 2: now a different number comes out.

## 14.12 Tests

A **test** is a small program that checks whether everything still calculates the same as
yesterday. Kern runs its tests after every change. For the Lehrer, they check among other things:

- whether the network learns correctly and PPO manages a simple task,
- whether the same episode runs identically down to the last digit,
- whether the fingerprint matches on all platforms,
- whether 1, 2 or 4 helpers give exactly the same network,
- whether the saved file is read correctly,
- whether the duck in Studio really jumps for real,
- whether measuring the servos works ([Chapter 16](16-calibration.md)).

The names of the tests are in the blue book, Chapter 14.

## Level complete!

- You know the roles: training plan, scoring rules, gym, helpers and analyst.
- You know why the critic is allowed to see more than the actor.
- You can explain why every episode builds a slightly different duck.
- You know why PPO learns in small, careful steps.
- You know the way to the jump: find a role model, copy, practice.
- You know what a real jump is and why the rules are strict.
- You can use the three-digit calculator to show why the order of calculating matters.

## Puzzle corner

1. How many times per second does the duck's network decide where the servos turn?
2. Calculate with the three-digit calculator: (200 + 0.3) + 0.3 and 200 + (0.3 + 0.3). What do you
   learn from this?
3. Why does only the actor get the sensors' measurement errors?
4. 32 ducks on 2 cores versus 32 ducks on 16 cores – same network? And with 16 ducks?
5. **Boss puzzle:** Why does a beak on the ground end the episode instead of just taking off a few
   points?

### Solutions

1. 50 times: a control tick lasts 20 milliseconds, and 1000 : 20 = 50.
2. Left: 200.3 becomes 200, and once more 200.3 becomes 200 again. Right: adding 0.6 gives 200.6, so
   201. The order changes the result. That's why Kern always adds in the same order.
3. Only the actor later runs with real, imprecise sensors on the robot. It has to learn to cope with
   that. The critic, on the other hand, should judge as precisely as possible and isn't needed later.
4. Yes, the number of cores only changes the speed. But with 16 ducks, different experiences come
   about, and so a different network.
5. The duck wants to collect as many points as possible in total in an episode. If propping itself
   up were just a bit more expensive, it could be worth it: it then doesn't fall and keeps
   collecting points for staying upright. Ending immediately takes away all later points – so
   propping itself up is never the safe bet.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
