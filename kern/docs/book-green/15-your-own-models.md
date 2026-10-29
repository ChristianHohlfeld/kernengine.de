# 15 Training Your Own Models – the Workshop

> **In short:** Here you train for yourself. On the Lehrer page in the browser you teach the duck to
> jump, read its progress from curves, save a good network, and watch it jump in the Studio. On top of
> that come the rules for building good exercises, and the traps that the real training runs have
> already fallen into.

> **Key terms:**
> - **Lehrer page** – the web page kernengine.de/lehrer/, where you train in the browser.
> - **Command line** – a window where you type commands instead of clicking. You don't need it for
>   this chapter.
> - **Return** – all the points of an episode added up; that's what the **Mean episode return** readout
>   on the Lehrer page shows. (The **return** in Appendix A.10 is a variant that's used
>   in the learning calculation: points from now on, later ones worth a bit less.)
> - **Noise** – the randomness added to each of the network's commands: the duck's curiosity.
> - **Learning rate** – how far the evaluator (14.2) adjusts the sliders per step.
> - **Snapshot** – an intermediate state of the network saved along the way.
> - **Measurement** – a strict check of whether a movement really succeeded, independent of the
>   points.

How the Lehrer works inside is in [Chapter 14](14-lehrer.md). You'll need its terms again here
(episode, practice round, critic, variation, role model, imitation).

## 15.1 Two ways: browser or command line

There are two ways to train, and with the same settings both give **the same network**:

- **The Lehrer page in the browser:** nothing to install, everything with mouse clicks. That's the
  path of this chapter.
- **The tool on your own computer:** faster and more versatile, but you have to build it yourself and
  operate it from the command line.

**What you can skip without the command line:** all the "For the curious" boxes and section 15.7
(play layer). Nothing in the rest of the text depends on them. The principles in them – how to
recognize progress, how to find the best network, what makes good exercises – apply to both paths.

> **For the curious:** How to build the tool, which commands it knows, and which settings there are is
> in the blue book, Chapters 15.1 and 15.2. The processor settings when building only make it faster;
> the network stays exactly the same as in the browser, down to the last hair.

## 15.2 Training the jump in the browser

A jump run has two phases that you can follow on the page:

1. **Imitating:** for 8 rounds, the network learns from the built-in role model. The status line shows
   "Imitating: round … of 8". After that the network can usually already
   jump, and the page saves it for the Studio for the first time.
2. **Practicing:** now the duck learns for points, with variation. For the first 30 rounds only the
   critic learns (Chapter 14.5), then both. Every 25 rounds the page does 5 test jumps and saves the
   network.

**How long does it take?** On a test computer with 2 cores, the first real jump came in the browser
after just under 9 minutes, and imitation was finished after about 11 minutes. Practicing takes much
longer, because it needs thousands of rounds. How fast your device is, the page shows under
**Steps / s** (steps per second): that's how many decision ticks all ducks together manage per
second.

> **Try it!** Your first jump run:
> 1. Open **kernengine.de/lehrer/**. Wait until the status line reports "Ready".
> 2. Under **Motion**, choose **Jump**. All other fields stay as they are.
> 3. Press **Start** and leave the tab open. A phone belongs on its charger.
> 4. Watch the status line: first "Imitating: round 1 of 8" and so on, then "Learning: iteration …".
>    Under **Real jumps (clean robot)** you see how many of 5 test jumps succeed.
> 5. As soon as "Saved for Kern Studio as jump.onnx" appears below the buttons, your network is ready for 15.6.
>
> What you learn: only the role model makes the jump possible; practicing then makes it reliable.

> **Did you know?** The same starting number, the same number of ducks and the same **recipe** (the
> fixed settings of a training run, more in 15.8) give the same network in the browser as on the
> computer. The number of ducks is **workers × ducks per worker**; a worker is a helper in the
> browser's background that lets its ducks work out. The built-in jump network came from 32 ducks,
> starting number 1 and 3000 practice rounds. So with 4 workers of 8 ducks each (32 in total),
> starting number 1 (the **Seed** field) and 3000 iterations, you set exactly the same settings – if you
> have the time.

## 15.3 Reading progress

You read progress from **curves**, not from a single number. The values fluctuate from round to
round, because each round has different ducks working out with different randomness. What matters is
the trend over hundreds of rounds.

> **Imagine …** the fever chart in a hospital: a single measurement says little, the trend over days
> says everything.

The Lehrer page draws return and episode length as curves and shows numbers alongside:

| Readout | Meaning | What to watch for |
|---|---|---|
| **Mean episode return** | points of an episode, averaged | should rise over many rounds |
| **Episode length (s)** | how long an episode lasts | jump: 2 s means no fall; shorter means a fall or propping up |
| **Action noise (std)** | how much the duck tries things out | drops as it gets more confident (14.5) |
| **Real jumps (clean robot)** | out of 5 test jumps | **the actual goal** |
| **Steps / s** | the device's speed | says nothing about quality |

If the points rise but the real jumps don't, that's a warning sign: maybe the duck is getting its
points with something that isn't a jump at all (15.9).

> **For the curious:** The tool writes every round as a line of text and, if you want, into a table
> you can load into a spreadsheet. With the command line you can also look at a single episode tick by
> tick, with every points item – like an X-ray that shows what the duck really gets its points for.
> Line format and commands: blue book, Chapter 15.4.

## 15.4 Choosing the best network

**The last saved network isn't automatically the best one.** While practicing, a network can get
worse again after a good stage. And the test during the run is small: only 5 jumps on the clean
robot – a network can look good by chance.

> **Imagine …** you take a series of photos. You don't just take the last one; you lay them all out
> side by side and pick the best.

Here's how to go about it:

1. Keep **snapshots** along the way.
2. Measure each one thoroughly with many jumps, on the clean robot **and** with full variation.
3. Choose by: first real jumps with variation, then no falls, then height, flight time and tilt.
4. Check the winner in the Studio (15.6).

For the built-in network, this measurement gave 40 out of 40 real jumps clean, 36 out of 40 with
variation, and not a single fall.

In the browser, the page keeps only the **newest** state. If the readout currently shows 5 out of 5
real jumps, you can save this state as a file with **Download .onnx** before
practicing continues.

> **For the curious:** The tool can keep a snapshot every few hundred rounds and measure each one with
> 40 episodes, clean and with variation. Commands: blue book, Chapter 15.5.

## 15.5 Gently learning on

After imitation, the network can already jump. Now the rule is: **keep learning carefully.** A small
learning rate, little noise, and critic first – otherwise what was learned gets lost.

> **Imagine …** you've just painstakingly taught a dog a trick. If you now yell new commands and wave
> treats around wildly, the trick is quickly gone again.

That's why the fields **Initial action noise** and **Learning rate** on the Lehrer page are empty
(they show **Recipe**): then the proven recipe applies. What the real runs also showed:

- **More trying things out doesn't help.** With a lot of noise and no warm start, the duck only
  learned to stand, not to jump.
- **Few ducks slow things down.** Few ducks collect little experience per round, and the direction of
  learning becomes uncertain.
- **The number of ducks determines the result, the number of cores only the speed.** On the page,
  that means: the total workers × ducks per worker decides which network comes out. More workers with
  the same total don't change the network; they just spread the work over more cores. But if you only
  raise the workers and leave **Ducks per worker** as it is, you have more ducks – and a different
  network.
- **Don't make the physics coarser.** With more coarsely calculated physics, training would be
  1.6 times faster, but the network would learn a different world from the Studio's.

> **For the curious:** The tool has switches for all the recipe's settings and can continue a run.
> Watch out: when continuing, you give the number of rounds as a **target**. If you want 1000 more
> after 3000 rounds, you write 4000. Everything about it: blue book, Chapters 15.3 and 15.6.

## 15.6 Using it in the Studio

A network from the Lehrer page lands in the Studio by itself if both run in the **same browser on the
same device**. The Studio loads it at startup and takes over every new one immediately, even in tabs
that are already open. This applies to the jump.

> **Try it!** Your duck jumps with your network:
> 1. Start a jump run on the Lehrer page (15.2).
> 2. In the same browser, open a second tab with https://kernengine.de/ and there the example level
>    via **File › Example Levels › Micro Duck**.
> 3. Wait until the Lehrer page reports "Saved for Kern Studio as jump.onnx". That happens for the first time after imitation, then every 25 rounds.
> 4. In the Studio, start **Play**, take over the duck with **F8** and press **7**.
>
> What you see: now **your** network jumps, not the one that ships with Kern – yours replaces it. An
> early state often still jumps unsteadily. After a while of practice, press **7** again and compare.

> **For the curious:** You can also put a network into the source code as a file, and the browser's
> developer console reports when a network has been loaded. What form the network must have and where
> the files go: blue book, Chapter 15.8.

## 15.7 Training the play layer

The play layer can only be trained with the tool on the command line, not in the browser. If you're
not planning to do that, you can skip this section. But the process shows two nice principles:

- **First measure what the role model needs.** The role model has to know where the beak grips a sock
  well and how far the duck keeps walking after "stop." Both are measured in the physics instead of
  estimated (Chapters 14.8 and 16.6).
- **Measure the role model.** Before you test the network, you test the role model. That shows what
  the network can **at most** achieve through pure imitation. If even the role model can't manage
  something, it's not the network's fault.

After that come imitation, testing, watching, practicing, and a trial run in the example level.

> **Watch out!** During imitation, the output file is overwritten after **every** round. If you want
> to keep an earlier round, you have to copy it during the run. For today's play network, round 8 of
> 12 was the best (Chapter 14.8), not the last one – exactly the lesson from 15.4.

> **For the curious:** All six steps with commands are in the blue book, Chapter 15.7.

## 15.8 Adding a new movement

Suppose the duck should **take a bow**. For that you need five ingredients:

1. **A task:** How long does the bow last (about 2 seconds)? How does the duck start? What earns
   points (staying upright, moving calmly, and of course the bow itself), and what ends the episode
   (a fall)? And what may the critic know in addition?
2. **A recipe:** the settings for training, such as a warm start from velstand, little noise, a small
   learning rate. With that, the tool and the browser know the new exercise; on the Lehrer page all
   that's missing is an entry in the selection.
3. **A measurement:** a strict check of whether a bow really happened (15.9).
4. **The connection to the Studio:** a key, a touch button, the name in the **Robot Move** logic node,
   and the network file. In the Studio the duck then switches to the new network for the duration of
   the movement and back afterward.
5. **A role model (often needed):** for hard movements, practicing alone doesn't find the way
   (Chapter 14.6). Then you need a simple movement plan, a grade for the search, and a role model that
   knows an answer in **every** situation – otherwise imitating with correction doesn't work.

Each of these ingredients requires programming. The learning machine itself stays the same: practicing
and imitating only know "a world in which a duck acts and gets points." The play layer, with
completely different inputs and outputs, learns with exactly this machine too.

> **For the curious:** The blue book shows the skeleton of a bow task in C++ and lists all the places
> in the Studio you have to extend (Chapter 15.9).

## 15.9 Measurement isn't reward

**The measurement checks what a movement is – not how many points it earned.** That may be the most
important rule of the whole workshop.

Points are a signpost for learning. But networks find every loophole in them. If you measured success
by the points, you'd think a network was good when it was just cleverly collecting points. That's why
the jump has its own strict test: all parts in the air for at least 60 milliseconds, the torso rises,
at most 30° of tilt, landed upright (Chapter 14.6). Whether the duck got many or few points while doing
so doesn't matter to the test.

> **Imagine …** a driving test. You can rack up driving lessons however you like. In the test, the
> only thing that counts is whether you really drive safely.

## 15.10 Designing good exercises

Anyone building a new exercise designs three things: what the network **sees** (the observation), what
it can **do** (the actions), and what earns **points** (the reward). Kern's networks model seven rules
for this:

- **Only what the robot really has.** The acting network sees sensor values, never true positions. The
  play layer knows ball and sock only from the camera, noisy and with dropouts. Only the critic gets
  true knowledge.
- **Make time visible.** A network has no memory of its own: it only sees the present moment and
  whatever it's given (such as its own last command, 13.7). That's why you have to tell it how far
  along a movement is (the jump's clock) or how long it has been standing, walking or carrying.
- **Measure instead of commanding.** "Standing still" means: both soles on the floor, no movement
  running. What counts is what the legs do, not what was commanded.
- **Actions like controls.** Walking commands work like a joystick with a dead zone: small deflections
  mean "stand." A movement like the kick only starts when its value crosses a threshold.
- **Named points items.** Each item is visible on its own, small deductions curb fidgeting and wobbling,
  and curricula make penalties and variation stricter step by step.
- **A hard episode end.** A fall and floor contact by other body parts end the episode; otherwise
  propping itself up becomes a safe bet.
- **A role model without secret knowledge.** The role model only sees what the network sees. That
  applies to its timing too: the role model doesn't say "Kick now!" from one moment to the next but
  gets more impatient with every moment of standing still, like a countdown. That way the network can
  read from the standing time when it's time. A sudden "Now!" without a recognizable reason is
  something it couldn't copy.

> **Try it!** At https://kernengine.de/, open the Micro Duck level and start **Play**. It begins with
> the observer camera: you watch and can lift things with the mouse or a finger. Lift the ball to a
> different spot and let go. Watch how the play layer decides anew from its sensors alone: look, walk
> over, stand, kick. What you learn: the duck doesn't "know" where the ball is – it has to see it.

## 15.11 Pitfalls from the real runs

So you don't fall into them – this happened in the real runs:

- **The reward gets exploited.** Early ducks sat on their tails, did dive jumps, or propped themselves
  up. Each time, only a stricter rule helped: floor contact ends the episode, tilt costs, points only
  when upright.
- **The measurement was too loose.** The first test let through a duck that only straightened up and
  pulled its feet in, tilting 45° while doing so. Since then, only a real jump counts.
- **Practicing alone doesn't find rare movements.** Only the role model and imitation brought the jump.
- **A good student, a clueless critic.** Without "critic first," what had been imitated broke (15.5).
- **Bugs in the world instead of in the network.** Training the play layer brought three to light: the
  sock crept across the floor by itself, the ball rolled forever, and the idle yawn opened the beak
  during a pickup. If a network just won't learn at some point, it's worth taking a look at the world.
- **Hand-entered numbers go stale.** Where the beak grips used to be a fixed number in the
  documentation. Now the Lehrer measures it anew on every run.
- **Breaking sameness by accident.** Just one math function that rounds differently, or a sum in a
  changing order – and the network is a different one on another device (Chapter 14.7).
- **Different physics from the Studio.** Coarser physics, a different floor, different motors: then
  the network learns a different world from the one it's supposed to run in later.

## What you take away

- The Lehrer page in the browser is the easiest way; the tool on the computer is faster but gives the
  same network.
- You read progress from curves, not from one number. The actual goal is real jumps.
- The last network isn't always the best: save intermediate states and test them thoroughly, clean
  and with variation.
- After imitation, keep learning gently: small learning rate, little noise, critic first.
- The number of ducks determines the result, the number of cores only the speed.
- A new movement needs a task, a recipe, a measurement, a Studio connection, and often a role model.
- The measurement checks what a movement is – not how many points it earned.

## Brain teasers

1. You train in the browser with 4 workers of 8 ducks each. A friend uses 8 workers of 8 ducks each
   on her computer, with the same starting number. Does she get the same network? What would she have
   to set?
2. Why do you test the candidates at the end with 40 jumps instead of relying on the 5 test jumps
   during the run?
3. Why shouldn't you practice with a large learning rate and a lot of noise right after imitation?
4. Why shouldn't the measurement of a new movement simply add up the points?
5. A network just won't learn something, no matter how long you practice. Where else besides the
   network could the error be?

### Solutions

1. No: she has 64 ducks instead of 32, so a different network. With 8 workers of 4 ducks each (32 in
   total) she'd get the same network, just faster.
2. Five jumps are a very small test; a network can look good by chance. And only with variation does
   it show whether it's robust.
3. The network can already jump. Big steps and wild trying things out would destroy what was learned
   before the critic even estimates well.
4. Because networks exploit loopholes in the reward. The measurement has to check whether the movement
   really happened.
5. In the simulated world: with the play layer, for example, the sock crept away by itself.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
