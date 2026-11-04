# 15 Training Your Own Models – Your Network, Your Duck

> **Your level:** Now you train yourself. You start the Lehrer in the browser, read its progress,
> pick the best network and make it jump in Studio. And you learn what matters when you want to
> teach a duck a completely new movement.

> **Words you need:**
> - **Return** – all the points a duck collected in one episode, added up
> - **Observation** – everything the network gets to see in one tick
> - **Action** – what the network answers, for example 14 servo targets
> - **Hyperparameters** – settings you fix before learning, such as the number of ducks
> - **Learning rate** – how big each learning step is, meaning how far the sliders are turned at once
> - **Snapshot** – a saved in-between state of the network during training
> - **Command line** – a window where you type commands instead of clicking buttons
> - **Dead zone** – a range in which a small input doesn't do anything yet

How the Lehrer calculates inside – actor and critic, PPO, role model, copying, variation – is in
[Chapter 14](14-lehrer.md). Here you try it out and learn the rules for planning a training run.

There are two ways. The **main way** is the **Lehrer page in the browser**: you don't need to
install anything, and it can train the jump and standing. The second way is a program for the
command line. Only people who build Kern themselves from the source code get it, and it can do a few
more things. In this chapter it only appears in the "For pros" boxes.

The Lehrer page follows your browser's language, just like Studio; the **DE | EN** button at the
bottom right switches between German and English. This chapter names its labels the way the page
shows them in English.

> **Mission:** On https://kernengine.de/, open the "Der Lehrer" entry, choose **Jump** and
> press **Start**. Leave the tab open (plug a phone into its charger). Expect 10 minutes or more
> before something happens. Watch the **Real jumps (clean robot)** display: from when on does your network jump?

## 15.1 The Lehrer Page

The Lehrer page is a complete training gym in the browser. At the top you set what gets trained,
press Start, and at the bottom you watch the network get better. It calculates with the same
training plan and the same physics as the program on a big computer.

> **Picture it like this:** like an oven with a window. You set temperature and time, slide the
> dough in, and can watch through the glass as it rises.

The fields on the page:

| Field | What it means |
|---|---|
| **Motion** | **Jump**, or **Stand (quick test)** |
| **Workers** | how many helpers calculate at the same time |
| **Ducks per worker** | how many ducks each helper runs |
| **Iterations** | how many training rounds |
| **Seed** | the starting number for chance |
| **Initial action noise** | how much chance at the start (empty, shows **Recipe**: as in the plan) |
| **Learning rate** | how big the steps are (empty, shows **Recipe**: as in the plan) |

The page suggests the number of workers itself, to suit your device. Below are the buttons
**Start**, **Pause** and **Download .onnx**.

> **For pros:** If you build Kern on your own computer ([Chapter 2](02-building-and-testing.md),
> best together with an adult), a script also gets you the Lehrer program for the command line. It
> can also continue save games, search for role models of its own, and train the play layer.
> Exciting: the script may speed up the program exactly for your processor **without changing the
> order or the rounding of the calculations**. It only picks faster instructions for the same
> calculation steps. That's why it doesn't change any result; the network stays the same down to
> the last digit – unlike the forbidden "fast math" from section 14.7, which rearranges
> calculations. Building and all commands are in the blue book, Chapter 15.

## 15.2 Everything the Lehrer Can Do

The Lehrer is more than a training button. It's a whole toolbox, so you don't just train but also
**measure** and **look at** what happens during training.

> **Picture it like this:** A sports club doesn't just have a training field, but also a stopwatch,
> a video camera for slow motion, and a doctor who checks that everything's okay.

This is what the Lehrer can do:

- **Train:** jumping or standing (in the browser), the play layer (only in the program).
- **Measure:** How often does a network really jump, clean and with variation?
- **Look at:** view an episode tick by tick, with every point item separately.
- **Find a role model:** find a new jump for copying.
- **Check:** form the fingerprint for determinism (section 14.7).
- **Measure servos:** for calibration ([Chapter 16](16-calibration.md)).

In the browser, you get training and measuring jumps. Only the program can do the rest. The exact
commands are in the blue book, Chapter 15.

## 15.3 Training the Jump

A jump training run always follows the same plan, the **recipe**. You don't have to set any of it –
but you should know what's happening while you watch.

> **Picture it like this:** like a training day at a sports club: warm up with what you can already
> do, watch the coach and copy her, then practice yourself until it's solid.

The process as a timeline:

1. **Load the recipe:** start from the walking and standing network velstand, which can already
   stand (warm start).
2. **Create ducks:** as many ducks as **Workers** × **Ducks per worker** gives, each with its own chance.
3. **Copy:** For 8 rounds, the network imitates the role model. The page reports "Imitating: round …
   of 8" and checks after every round whether the network already jumps.
4. **Critic first:** For the first 30 iterations, only the critic learns.
5. **Practice:** After that, both practice with PPO, with little noise and full variation.
6. **Check and save:** Every 25 iterations, the network does 5 test jumps, and the page saves it.

Often the network already jumps after copying. Practice then makes the jump **robust**, meaning
reliable even with a slightly different duck.

Two things from real runs are worth learning from:

- **It doesn't work without a role model.** Pure practice with points never found the takeoff
  (section 14.6).
- **A training run is like a save game.** The program can save the whole learning state and later
  continue exactly there. Because of determinism, the result is the same as without a pause.

The Lehrer page trains exactly the same recipe as the program. The same seed and the same total
number of ducks give the same network – in the browser as on the big computer.

> **For pros:** Kern's jump network was made with the program: 32 ducks, seed 1, 3000 iterations of
> practice, snapshots every 250 iterations. There you can also train without a role model, search
> for a role model of your own, and override individual values of the recipe. Commands and file
> names: blue book, Chapter 15.

## 15.4 Reading Progress

During training, the page shows numbers and a curve after every iteration. They reveal whether the
network is really learning something. The most important question: are the points going up, **and**
can the duck do what it's supposed to?

> **Picture it like this:** like your report card and a test. The points (return) are the grades.
> But only the real test (real jumps) shows whether you can really do it – or have only learned how
> to collect points.

| Display | Meaning | What to look for |
|---|---|---|
| **Mean episode return** | points per attempt | rises over many iterations |
| **Episode length (s)** | how long an attempt lasts | jump: 2 s means no fall |
| **Action noise (std)** | wobble strength | falls as the network gets surer |
| **Real jumps (clean robot)** | test: 5 jumps | the actual goal |
| **Jump height (mm)**, **Flight time (ms)** | how well it jumped | should rise |
| **Steps / s** | your device's speed | only speed, not learning |

The curve shows return and episode length over the iterations. A typical good course: it jitters
from round to round but slowly rises over hundreds of iterations. If the episode length is shorter
than 2 seconds, attempts end too early – through a fall or because a wrong part touches the ground.

"std" is a technical abbreviation for the wobble strength: how much the chance typically shifts the
movements.

When something goes wrong, **looking at** helps: the command-line program shows an episode tick by
tick, with every point item separately. That way you see whether the network jumps – or just tricks
the reward. The Lehrer page in the browser can't do that. There, you look closely at your network in
Studio (15.8): does the duck really jump, or does it do something completely different?

> **For pros:** The program shows two more values. The critic's error is high after copying (it
> doesn't know the new player yet) and should then fall. The learning rate adjusts itself: if the
> policy changes too much per step, it gets smaller. If it sticks to the smallest allowed value all
> the time, the steps were constantly **too big** – the brake is fully on. All fields: blue book,
> Chapter 15.

## 15.5 Picking the Best Network

The last network isn't always the best. A training run can get worse again after a good state. And
the test during training is only 5 jumps – that could be luck.

> **Picture it like this:** like at a sports day. It's not the last throw that counts, but the
> best. And a single lucky hit doesn't make a winner.

How to pick properly:

1. **Keep in-between states:** save snapshots instead of just the last state.
2. **Measure thoroughly:** check each one with many more episodes, clean **and** with variation.
3. **Sort properly:** first real jumps with variation, then falls (ideally 0), then rising, flight
   time and tipping.
4. **Try it out in Studio** (15.8).

For Kern's jump network, that gave 40 of 40 real jumps clean and 36 of 40 with variation, with no
fall.

In the browser, the page saves the newest state every 25 iterations and overwrites the old one. If
you see a good state, secure it with **Download .onnx**.

> **For pros:** The program keeps snapshots automatically and measures each with 40 episodes. How
> that works is in the blue book, Chapter 15.

## 15.6 Choosing Hyperparameters

Hyperparameters are the settings **before** learning. Three rules from the real runs are enough to
understand them.

**Rule 1: The learning rate is the stride length.**

> **Picture it like this:** The learning rate is like your stride length when walking in the dark.
> Too short, and you never arrive. Too long, and you trip over the curb.

After copying, the duck is already good. That's why it keeps learning with small steps and little
noise afterward. Big steps and lots of chance would easily wreck a good network. And more noise
doesn't help with finding: with lots of chance and no starting network, the duck learned to stand,
not to jump – because standing brings safe points, and chance still doesn't hit the takeoff
(section 14.6).

**Starting network and network size belong together.** If you start from the finished standing
network velstand, you have to keep its size, meaning the same number of layers and sliders. A
starting network is like a half-finished jigsaw puzzle: you can keep adding pieces, but you can't
suddenly take a bigger frame. A network of a different size has to start from zero.

**Rule 2: More ducks change the network, more helpers only the speed.** An example:

| Workers × Ducks per worker | Ducks | Result |
|---|---|---|
| 2 × 16 | 32 | network A |
| 4 × 8 | 32 | network A again, just faster |
| 4 × 16 | 64 | network B, a different one |

More ducks means more experience per iteration – so different data and a different network. Few
ducks slow learning down, because each round then brings little experience. More helpers just spread
the same work over more cooks.

**Rule 3: The physics has to be exactly as in Studio.** You could calculate the physics more roughly
and be faster. But then the duck moves differently, and the network learns a different world from
the one it's supposed to jump in later.

> **For pros:** The program has a switch of its own for each hyperparameter: how many ticks each
> duck does per iteration, how often the analyst goes through the data, how big the networks are,
> which servo profile applies. The list with all values of the jump recipe is in the blue book,
> Chapter 15.

## 15.7 Training the Play Layer

The play layer – the duck goes to the ball or sock, kicks or picks up – can only be trained with the
program, not in the browser. It's still exciting, because it shows the whole path in six steps.

> **Picture it like this:** like trying out a recipe. First you check what your kitchen can do.
> Then you cook it, taste, see where it turned out different, improve – and finally serve it to your
> guests.

1. **Measure:** Where does the beak grip a sock well? The Lehrer measures that in the physics on
   every run instead of writing it down. How far the duck keeps walking after "stop" was worked out
   once by a measuring command; that number is still fixed in the program.
2. **Copy:** The network imitates the play layer's role model, round by round.
3. **Check:** Count kicks, pickups, spit-outs and falls – for the network **and** for the role
   model.
4. **Look at it:** an episode decision by decision, role model and network side by side. That way
   you see where the network deviates.
5. **Practice:** with PPO, critic first, with little noise.
6. **Check in the level:** have the example level played through exactly as in Studio.

An important thought is hidden in step 3: **The role model is the upper limit.** A network that only
copies becomes at most as good as its role model. If the role model itself can't hold on to the
sock, the problem isn't the network. It can only get better than the role model during practice,
because there only what really works counts.

> **For pros:** All commands for the six steps are in the blue book, Chapter 15.

## 15.8 Using It in Studio

A network you train in the browser ends up in Studio by itself – on the same device, in the same
browser. The Lehrer page saves it on your device, and Kern Studio loads it at startup. Open Studio
tabs pick up every new network right away. That applies to the jump.

> **Picture it like this:** like a save game that two games on your console are allowed to share.
> One saves it, the other loads it.

To check: open the Micro Duck level, **Play**, key **7**. The play layer works by itself when the
duck is autonomous, the Kern extras are on, and the ball or sock is in sight
([Chapter 13](13-micro-duck.md)).

> **Mission:** Did you train a jump in the browser? Open Kern Studio in the same browser, load
> **File › Example Levels › Micro Duck** – if a game is running, first stop it with Esc –, start
> **Play**, press F8 and then **7**. Now **your** network is jumping. On a computer you can check:
> press F12, choose the "Console" tab, and it says "Kern: trained network jump.onnx loaded." On a
> phone there's no such console – there the jump is proof enough.

> **Mission:** In the Micro Duck level, click the duck and look under
> **Details › Player Controller › Walker** for **Autonomous (camera, ToF)** and **Kern extras (ball
> play, sock fetching, beak in the pick)**. Start **Play** once with and once without Kern extras
> (to switch, first stop with Esc). Watch the ball and sock: with Kern extras, the learned play layer
> decides what the duck does – it also goes to the sock and tries to pick it up. Without Kern
> extras, Pollen's rebuilt ball game takes over (BallPlay, [Chapter 13](13-micro-duck.md)): the duck
> only walks to the ball and kicks it; it doesn't deliberately take care of the sock.

> **For pros:** If you build Kern yourself, you can also put a network as a file into Kern's folder
> for networks. Kern checks whether the network has the right shape; a network of the wrong size
> isn't used. Folders and file names: blue book, Chapter 15.

## 15.9 Adding a New Task

Teaching a new movement is more than "go train." You first have to say exactly what should be
learned and how you'll recognize success. There are **five steps** for this; step 4, the role model,
is only needed for hard movements.

> **Picture it like this:** like a new sport at a club. You need rules, a training plan, a referee,
> sometimes someone to demonstrate – and in the end a spot on the schedule.

Let's take a **bow** as an example:

1. **The task (the rules):** How does an episode start, how long does it last (for the bow, say,
   2 seconds), and what earns points? For example: points when the head goes low and both feet stay
   on the ground, deductions for fidgeting, the end on a fall. Plus: what may the critic see extra –
   the true height of the body, for example?
2. **The recipe (the training plan):** Which network does training start from, with how much noise
   and which learning rate? After that, the program and the browser know the new task; the Lehrer
   page only still needs an entry in its menu. If the movement needs a new input that the starting
   network has never seen (for the jump, the clock hand), its sliders start at 0 – as in section
   14.4: first the new network acts like the old one, then it learns to use the new input.
3. **The measurement (the referee):** A checker that determines whether a bow **really** succeeded –
   just as it's defined for the jump what a real jump is.
4. **A role model (only if needed):** If practice doesn't find the movement, you describe it with a
   few numbers, let CMA-ES search for the best ones, and build a role model that has an answer in
   **every** situation. Only then can the network copy it with DAgger.
5. **Build it into Studio (the schedule):** a key, a button for phones, a name for the logic node
   **Robot Move** ([Chapter 13](13-micro-duck.md)) and the network itself. The new movement then
   runs like the jump: when you press the key, Kern switches hard to its own network, and after its
   duration (for the bow, 2 seconds) back to the walking network. Like a relay baton: exactly one
   network always holds the legs.

The most important sentence of this section is in step 3: **Measure what the movement is – not what
the points count.** The points are just bait. A network often finds a way to get points without
doing the movement. Only a strict measurement uncovers that.

A movement that needs completely different inputs and outputs than the leg networks is shown by the
play layer: it has its own gym, its own points and its own measurement. The learning itself – PPO
and copying – stays the same.

> **For pros:** What task, recipe and setup look like in program code is in the blue book,
> Chapter 15, with a complete framework for the bow.

## 15.10 Designing Observation, Action and Reward

This is the most important section of this chapter. A network only learns what you make possible
for it: what it sees (the **observation**), what it can do (the **action**) and what it gets points
for (the **reward**). Design that well, and you get a network that works on the real robot. Design
it badly, and you get a network that cheats or relies on something that doesn't even exist.

> **Picture it like this:** You make up a game for your little sister. If you say "Whoever finishes
> first wins," she'll be fast – but maybe sloppy. Whatever gets rewarded gets done. It's exactly the
> same with the network.

Kern's networks show seven rules.

### 1. Only Show What the Robot Really Has

The actor only gets what the real robot's sensors measure: tilt, rotation, joint angles, what the
camera recognizes. Never the true position of the ball or sock. The play layer only knows the ball
the way the camera sees it – imprecisely and sometimes not at all.

Why so strict? A network relies on everything it gets. If it gets the true ball position in
training, it uses it. On the real robot there is none – and the network is blind. That's why only
the critic gets the true values, because the critic never goes onto the robot.

### 2. Make Time Visible

A network has **no memory**. It only sees the current tick. How is it supposed to know where in the
jump it is, or how long it has been standing? You have to show it.

For the jump, that's the clock with one hand (section 14.6): the network gets the two coordinates of
the tip of the hand, and the hand goes around once in 2 seconds. An example: after 0.5 seconds it
has made a quarter turn. If it started on the right at (1 | 0), it's now at the top at (0 | 1). Both
numbers always stay between −1 and +1, however long the episode runs. A simple counter, on the other
hand, would keep getting bigger.

The play layer gets to see times instead: how long it has been standing, how long it has been
walking, and how long it has been carrying something.

### 3. Measure Instead of Command

"The duck is standing" means in the play layer: **both soles have been on the ground for more than a
quarter of a second** – so what the legs really do. Not: "The walk command is zero." Why? With very
small walk commands, the gait sometimes doesn't get going at all. Then the duck **is standing** even
though there's a command. If you believe the command instead of the measurement, you're wrong.

The same idea is in the real jump (section 14.6): what counts is what the body does – flight,
rising, landing – not what the network wanted.

### 4. Actions Like Controls

The play layer's actions work like a gamepad:

- **Walk commands with a dead zone,** like a stick: below 20% deflection, the duck stands. So at 15%
  nothing happens; at 60% it starts walking. That way it doesn't tremble at every tiny number.
- **Movements as buttons:** kick and pick-up are numbers between −1 and +1. Only above 0.5 does the
  button count as pressed.
- **Limits for head and neck,** so the network doesn't ask for anything impossible.

Actions like these are easy for the network to hit: it doesn't have to catch an exact number, just
the right range.

### 5. Point Items with Names

The reward isn't a single number but a sum of items with names: "upright," "landing," "fidgeting,"
"fall" … When looking at an episode in the program (15.4), you can view each item separately. If the
points go up but the duck doesn't jump, you can immediately see **which** item brings the points –
and find the trick the network is cheating with.

On top of that come small deductions that encourage good manners: for fidgeting, for wobbling, when
the duck touches itself. And some penalties grow according to a **curriculum**: at first the duck may
still fidget and fall without it costing much. As it gradually masters the movement, the penalties
get bigger.

### 6. A Hard Ending

A fall and any ground contact of a part that isn't a sole end the episode **immediately**. A few
minus points wouldn't be enough. Let's do the math: if propping itself up on its beak costs 1 point,
but the duck stays standing for 1 more second that way and gets 2 points for staying upright, then
propping itself up pays off. Ending immediately takes away all later points – then it never pays
off.

### 7. A Role Model Without Secret Knowledge

If the network is supposed to copy a role model, the role model may only see what the network sees,
and may not secretly remember anything. Otherwise the network can't learn it – like with a teacher
who looks at the answer key.

An example from the play layer: the role model wants to kick when the duck has been standing long
enough. If it reported that as a single button press in exactly one tick, the network would have to
hit that one tick. Instead, the wish rises as a **ramp**, like a slanted line instead of a step:

| Standing time | Kick wish |
|---|---|
| 1 second | −1 (definitely not) |
| 1.25 seconds | 0 (undecided) |
| 1.5 seconds | +1 (kick now) |

In between, it rises evenly. The network sees the standing time (rule 2). So it can read the wish
directly from what it sees. Above 0.5, the button counts as pressed (rule 4).

## 15.11 Pitfalls from the Real Runs

When training Kern's networks, quite a few things went wrong. Every mistake turned into a rule. If
you train yourself, you'd otherwise trip over the same stones.

> **Picture it like this:** like a mistakes notebook in math. If you write down what went wrong,
> you don't make the same mistake twice.

- **The reward was tricked.** The duck sat down on its tail, did a dive, or propped itself up – all
  of it earned points, none of it was a jump. Only stricter rules helped: a hard ending on ground
  contact, a deduction for tipping, points only when upright.
- **The measurement was too loose.** A "jump" in which the duck only straightened up, pulled in its
  feet and tipped 45° was counted at first. Since then, only a real jump counts.
- **Practice alone doesn't find rare movements.** Something that needs all servos at full force at
  the same time practically never comes about by chance. Role model and copying solved that.
- **A clueless critic** wrecks a good player. That's why only the critic learns at first after
  copying.
- **Bugs in the world instead of in the network.** The sock crept across the floor by itself, the
  ball rolled forever, and the duck's yawn (one of its idle movements,
  [Chapter 13](13-micro-duck.md)) opened the beak while picking up. If a network just won't learn at
  some point, a bug like that in the world might be behind it – looking at it tick by tick in the
  program shows it.
- **Numbers written by hand go out of date.** Where the beak grips used to be a fixed number in the
  description. If the physics changes, it's no longer right. Now the Lehrer measures it anew on
  every run.
- **Determinism breaks easily.** A single calculation that rounds differently, and the network is a
  different one on another device (section 14.7). The fingerprint shows it.
- **Different physics than in Studio** means: the network learns a different world.

> **For pros:** One pitfall concerns the converter (section 14.4). With a few dozen ducks, it
> changes noticeably from iteration to iteration. That's why, before every learning step, the
> analyst recalculates the old policy with the new converter – otherwise it would compare old and
> new in a skewed way. More in the blue book, Chapter 15.

## Level complete!

- You train a jump on the Lehrer page and know what's happening while you do.
- You read return, episode length, noise and the jump test – and know that only the test shows
  whether the duck can really do it.
- You pick the best network, measured clean and with variation.
- You know what changes the network (the number of ducks) and what only changes the speed (the
  number of helpers).
- You bring your network into Studio.
- You know the seven rules for designing observation, action and reward.

## Puzzle corner

1. Which key starts the jump in Studio?
2. You train with 2 Workers × 16 ducks, seed 1. Your friend takes 4 Workers × 8 ducks, also seed 1.
   Do you get the same network? And if she takes 4 × 16?
3. Why does the duck keep learning with small steps and little noise after copying?
4. You want to teach the duck a bow and give points the lower the head is. What could the duck learn
   instead, and which rule helps?
5. **Boss puzzle:** Your network keeps getting more points, but **Real jumps (clean robot)** always
   shows 0 of 5. What probably happened, and how do you find out?

### Solutions

1. The **7** (on a phone, the **HOP** button).
2. Yes: 2 × 16 = 32 and 4 × 8 = 32 ducks, same seed – the same network, hers just faster. With
   4 × 16 = 64 ducks, different experiences come about, and so a different network.
3. After copying, it can already jump. Big steps and lots of chance could wreck the good network –
   like too much salt in a finished soup.
4. It could lie down or prop its head on the ground. That brings lots of points but isn't a bow.
   What helps: a hard ending when a part other than the soles touches the ground, and a strict
   measurement of what a real bow is.
5. The network is tricking the reward: it found a way to get points without jumping. Find out
   **which** point item brings the points: with the command-line program you look at an episode
   tick by tick; in the browser you load the network into Studio and watch closely what the duck
   does instead (propping itself up, sitting on its tail …). Then you make the rules stricter.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
