# 6 Physics – How Things Fall, Roll and Hold Together

> **In short:** Kern lets crates fall, balls roll, doors swing and robots walk. The number
> crunching is done by **Box3D**, a ready-made physics engine by Erin Catto. Kern lays a thin
> layer on top. You'll get to know bodies, shapes, materials, joints, contacts, the beat of the
> simulation and the Micro Duck's servo model.

> **Key terms:**
> - **Servo** – a small positioning motor that moves to an angle on command and holds it, like
>   the one that moves the rudders of a model airplane. The Micro Duck has one in every joint.
> - **Hertz (Hz)** – "how many times per second". 60 Hz means 60 times per second. For a spring,
>   it says how fast it swings back and forth: the more hertz, the stiffer.
> - **Bit** – the smallest digit of a number in a computer, a single zero or one. "The same bit for
>   bit" means: the same down to the very last digit.
> - **Torque** – turning force. A wrench with a long handle gives more torque for the same force.
>   Measured in newton meters (N·m).
> - **Impulse** – a short kick, like kicking a ball. A **force**, on the other hand, pushes as long
>   as you apply it – in Kern, for one step of the simulation at a time.
> - **Collider** – the invisible, simple shape a thing bumps into things with. Often coarser than
>   what you see: a car, for example, bumps into things as a box.
> - **Solver** – the part of Box3D that satisfies all the rules at once in every step: nothing is
>   stuck inside anything else, joints hold together.
> - **Network** – the duck's learned control program, a neural network. It was trained in the
>   simulation through lots of trial and error ([Chapter 13](13-micro-duck.md) and
>   [14](14-lehrer.md)).

![The example level Physics Lab in Kern Studio: stack, pendulum, door, rope bridge, turntable, elevator and rover](../buch/bilder/studio-physics-lab.png)

**The basic rule: Box3D is never adapted to Kern.** Kern changes neither Box3D's program nor its
fixed values, and Kern doesn't turn any secret knobs inside it to "get a model working". If a model
doesn't fit, the **model** is adapted, not the physics. Whatever a real device does on top, such as
a servo with a controller and friction, gets its own small model in real units (6.15).

> **Imagine …** a referee whom all the teams accept. If a team doesn't like how the game goes, it
> trains differently. Bribing the referee is off-limits.

Kern talks to Box3D at only **one single hand-over point**. There, every input is checked, so that
nonsense like a negative radius never gets into the physics. Calculations are in meters, seconds
and kilograms, with Z up – as in the renderer.

The basics behind it (how a physics simulation thinks in the first place) are explained in
[Appendix C](C-physics.md). Here it's about **how Kern uses it**.

## 6.1 The right size

Box3D calculates with a fixed **slack of a few millimeters**: bodies may sink into each other by
about 5 mm, and Box3D notices a touch shortly before it happens (why is explained in
[Appendix C](C-physics.md)). That's on purpose: with no slack at all, things lying on top of each
other would jitter constantly. A bit of sinking in is the price of calm. For a crate, that's tiny.
For a limb 2 cm long, it's huge.

> **Imagine …** a ruler with millimeter marks. It's precise enough for a wardrobe, but not for an
> ant.

That leads to the first rule: **moving bodies belong between 10 cm and 10 m.** Much smaller, and
things get wobbly. As an emergency brake, the game runtime (the part of Kern that runs a level as a
game, [Chapter 8](08-game-runtime.md)) never makes moving shapes smaller than 1 cm – but the rule
for good behavior remains 10 cm.

> **Did you know?** Measured: the Micro Duck at real size has limbs of 2–4 cm. Its torso sank 9 cm
> through its legs within 10 frames. At four times the size, the joints hold.

### Scaling instead of changing the slack

Box3D could change its slack, but only for the **whole** world – and in a Kern level, cars stand
next to small robots. So Kern enlarges the **model**.

> **Imagine …** filmmakers shoot scenes with model ships and play them back slower so that the
> waves look "real". Kern does something very similar: the robot gets bigger, and a real second
> lasts longer in the world. The movement stays the same – only in sizes Box3D can handle well.

For the Micro Duck that means: **length ×4, time ×2, gravity stays the same.** Why only ×2 for
time? Something four times as big takes twice as long to fall, just as a big tree falls over more
slowly than a small stick. So that the big duck looks exactly like the small one, its clock has to
run twice as slowly too. Everything else (mass, force, momentum) is converted to match, so that the
movement looks exactly the same, only bigger and slower. That's **only a change of units**, not a
different robot. Toward the outside, for example to the network or in measurement logs, Kern keeps
speaking in real units.

A test proves it: a freely hanging servo leg moves **the same bit for bit** at normal size and at
four times the size, that is, down to the last digit of every number. That works because ×4 and ×2
are especially smooth numbers for the computer: it calculates in the binary system, and multiplying
by 2 or 4 is as easy for it as moving the decimal point is for us when multiplying by 10. No new
rounding error comes up. On the ground, where the fixed slack plays a part, they're less than a
third of a millimeter apart.

So wherever the simulation differs from the real robot, it's down to the models of servo, contact
and mass – never to the enlarging. These models are fitted to measurements
([Chapter 16](16-calibration.md)). But even the best simulation is no guarantee that the real robot
will behave safely.

> **For the curious:** At four times the size, Box3D's 5 mm slack acts like only 1.25 mm on the
> real robot – finer than at real size. The complete conversion table is in the blue book,
> Chapter 6.

## 6.2 The world and its bodies

Everything lives in a **world**. It holds all bodies, shapes and joints and calculates them on step
by step, like the pages of a flip book (6.12). In this world there are three kinds of **bodies**:

> **Imagine …** a theater. The stage floor never moves: **static**. The lift platform moves exactly
> as the stage crew commands and carries along everything standing on it: **kinematic**. And the
> ball an actor throws obeys gravity, impacts and friction: **dynamic**.

Why does the lift platform carry along what stands on it? Because Kern **drives** it instead of
putting it in its new place from frame to frame: in every step it gets the velocity with which it
reaches its next target. An elevator that jumps instead of driving would leave its passengers
hanging in the air.

You move a dynamic body in two ways:

- A **force** pushes as long as you apply it – in Kern, for exactly **one** step. Whatever is meant
  to push continuously, such as wind, has to act anew every step.
- An **impulse** is a one-off kick, for example for a jump.

How heavy a body is and how sluggishly it turns usually follows from its shapes and their
material. Imported robots, on the other hand, bring their measured values with them; then the
shapes only decide where they bump into things.

So that fast things don't slip through thin walls, Box3D checks their path between two steps
without gaps against static things; especially fast ones, such as player vehicles, also against
moving ones. Resting bodies may "sleep": they aren't calculated any more until something bumps
them. Players and robots never sleep, though. A duck walking very slowly would otherwise look to
the physics like a resting thing and would be frozen in mid-step.

Games don't build their bodies by hand but from the level ([Chapter 8](08-game-runtime.md)). Studio
draws the colliders in the editor from **the same plan** the game uses to build the physics. So
what you see is exactly what gets simulated.

> **For the curious:** The world calculates everything one after another, not several
> calculations at the same time. That way the order of all calculations is always the same – a
> precondition for the same result coming out everywhere (6.13).

## 6.3 Shapes

So that a body can bump into things, it gets **shapes**, several if you like. Kern offers five:

| Shape | Everyday picture |
|---|---|
| Box | a shoebox |
| Sphere | a ball |
| Capsule | a pill: a cylinder with two half-spheres on its ends |
| Hull | a rubber sheet stretched tight around a cloud of points |
| Triangle mesh | any shape, but **only for things that never move** |

The **hull** is the all-rounder: it adapts to almost any shape as long as it has no dents (more in
[Appendix C](C-physics.md)). Arbitrary triangle meshes are only allowed as a static floor or static
wall, because collisions between two such meshes would be far too expensive. Nonsense like a radius
of zero or a flat hull is rejected.

With static triangle meshes, Kern pays attention to the seams between the triangles. Otherwise a
rolling ball would get stuck on the seam between two floor tiles, as if there were an edge – a
so-called **ghost edge**.

> **Try it!** Use **File › Example Levels** to open the level **Physics Lab** and, under **Window ›
> Show**, switch on **Collision Shapes**. Now you see the colliders as lines over the things:
> boxes, spheres and other shapes. Compare: where is the shape coarser than the visible thing?

## 6.4 Material

Every shape has a **material**. It says how heavy, how slippery and how bouncy it is:

- **Density** – how heavy the stuff is. In the level, every collider starts with **1000 kg/m³**
  (kilograms per cubic meter), the density of water. A cube with 10 cm sides then weighs 1 kg, one
  with 1 m sides a metric ton.
- **Friction** – how grippy (6.6).
- **Restitution** – how strongly something bounces (6.6).
- **Rolling resistance** – how quickly rolling things roll to a stop (6.6).
- **Surface** – what it's made of, such as fabric or wood (6.5).
- **Sensor** – a switch: then the shape doesn't bump into anything but only reports when something
  enters it, like a light barrier.

### Who collides with whom

Not everything should collide with everything. The limbs of a robot, for example, shouldn't bump
into each other, or they'd get tangled up.

> **Imagine …** every shape wears a club badge and carries a list of the clubs it wants to scuffle
> with. Two shapes only collide if **both** want to have something to do with the other's club.

On top of that come **groups**: all parts of a robot get the same group number, which means "never
among ourselves". And bodies that are directly connected by a joint don't collide anyway.

## 6.5 Surfaces with pair friction

When two shapes meet, the contact needs **one** friction value. Normally Box3D forms a kind of
average from the friction values of both shapes that lies closer to the more slippery one: ice on
rubber stays slippery. For most things that's enough.

But not for all. The comments in the program put it nicely: *TPU on cotton has its own measured
value, not the mean of TPU on TPU and cotton on cotton.* (TPU is a soft, rubbery plastic from the
3D printer.)

> **Imagine …** the Micro Duck is supposed to grab a sock with its beak. The sock should stick to
> the beak but slide on the wooden floor. With a single friction value per thing that doesn't work:
> if the sock is grippy, it sticks to the floor too. It depends on the **pair**.

That's why Kern knows **surfaces**: fabric, TPU and floor. For certain pairs there are their own,
measured values, which say how firmly the dry pair holds before it slides. Friction values range
roughly from 0 (glassy smooth) to 1 (very grippy):

| Pair | Friction | Origin |
|---|---|---|
| Fabric on fabric | 0.5 | cotton knit on knit |
| Fabric on TPU | 0.8 | soft beak pads on cotton |
| Fabric on floor | 0.4 | cotton on wood/laminate |

If one side has no surface, the usual average from the shapes' friction values applies again. The
Micro Duck uses all three ([Chapter 13](13-micro-duck.md)): the lumps of its sock are fabric, the
printed pads on its beak are TPU, the wooden floor in the example level is floor. Its soles have
**no** surface: they keep exactly the friction its network was trained with.

> **For the curious:** Kern doesn't change Box3D for this. Box3D has a docking point provided for
> a custom friction rule, and Kern hooks its pair table in there. The code is in the blue book,
> Chapter 6; why the usual average is formed the way it is is explained in
> [Appendix C](C-physics.md).

## 6.6 Friction, bounciness, rolling resistance

Three material values decide how a thing feels:

- **Friction** makes things slippery (ice) or grippy (rubber).
- **Restitution** makes things bouncy: 0 means "plop, it lies there", 1 means "bounces back
  without loss". Of two shapes, the bouncier one counts. Very gentle, slow impacts never bounce,
  otherwise everything would jitter forever.
- **Rolling resistance** slows rolling things, because real balls and floors give a little.

Kern uses rolling resistance on purpose, and the comments in the code explain why:

- the ball in the Micro Duck level: "a rubber ball on a wooden floor rolls to a stop",
- the tires of the rover in the Physics Lab roll easily,
- the sock lumps brake hard: "a lump of fabric doesn't roll; round lumps would, and the pile
  crawls". The sock is made of lots of small spheres, you see. Without a brake they'd roll away,
  and the whole sock would wander across the floor by itself.

**Robot feet** are wide, flat shapes with high friction, so that the robot stands securely and
doesn't slip.

> **Try it!** Open the **Physics Lab** and select a crate. On the right in **Details**, under
> **Collider**, set the field **Restitution (bounce)** to **0.9**. Start **Simulate** with Alt+S,
> the test run with physics, grab the crate with the mouse, lift it up and let it drop. Now it
> bounces several times, a bit lower each time. Set the value to 0: it plops down and stays there.
> Same crate, different material.
>
> Good to know: things without their own collider have the same fields under **Static Mesh ›
> Physics**; you set up the endless ground under **World Settings › Physics (Box3D)** with
> **Ground friction** and **Ground bounciness**.

> **For the curious:** In the **World Settings** there are also fields for how softly contacts
> give way and how quickly Box3D pushes things that have got stuck in each other apart again.

## 6.7 Joints

A **joint** connects two bodies and allows only certain movements. Box3D knows nine kinds, and each
has a model in everyday life:

| Joint (editor) | Everyday picture |
|---|---|
| Hinge (revolute) | door hinge: turns around one axis |
| Slider (prismatic) | drawer: slides along a line |
| Ball (spherical) | shoulder joint, with a cone as the limit |
| Fixed (weld) | weld seam (may also spring a little) |
| Distance / rope | rod or rope |
| Wheel (suspension) | wheel with suspension and drive |
| Motor | pulls one part into a certain position relative to the other |
| Parallel | roly-poly toy: keeps the orientation softly |
| No collision (filter) | connects nothing, only prevents collisions |

**The door example:** a hinge connects the door frame and the door. At the hinge point, Kern sets a
vertical axis that the door may turn around. A **limit** of 100 degrees in each direction makes
sure the door doesn't swing through the wall. The angle zero is the position in which the door and
frame were built. That's why you always build joints in their resting position: if you build the
door in half open, Kern takes "half open" to be zero. The doorstop would then sit in the wrong
place.

Many joints have three extras:

- a **spring** that pulls back toward a position, like a door closer,
- **limits** that restrict the movement, like a doorstop,
- a **motor** that turns at a desired speed, but only up to its maximum force. It always needs
  this limit: a motor without a limit would be a rigid constraint that never gives, like a hand
  made of concrete.

During the game you can ask how strongly a joint is holding at the moment, and switch motors on
and off.

### When joints break

A joint can have a **breaking threshold** (in the editor, the fields **Break force (0 = never)** and
**Break torque (0 = never)**). If it's loaded more heavily, it breaks: the game runtime removes it,
writes a message to the log and plays the breaking sound. That's how the rope bridge in the Physics
Lab can snap: the joints of its middle planks have a breaking threshold of 60,000 N (newtons, the
unit for force).

> **Try it!** Start **Simulate** in the Physics Lab and pull hard on a plank of the rope bridge
> with the mouse: the chain gives way and swings, but doesn't snap. The mouse grip can't manage
> 60,000 N, because it never pulls harder than a multiple of the plank's weight. Want to see it
> snap? First end simulating with **Esc**, because you set the breaking threshold in the editor.
> Then select the plank "Plank 07" (folder "Bridge") and, in **Details** under **Breaking**, set
> **Break force (0 = never)** to **20000**. Start **Simulate** again: the bridge sags but holds.
> Now grab "Plank 07" and pull it hard 2–3 m upward: the joint breaks, and the bridge snaps in two.
> Under **Window › Show**, also switch on **Constraints**; then you see which joint is missing. For
> comparison: below about 10,000 N, the tensioned bridge snaps all by itself, under its own weight.

## 6.8 Queries: rays, spheres, explosions

You can also ask the physics questions without moving anything:

- **Ray query:** "What does an imaginary line from here to there hit first?" The answer: point,
  direction of the surface, body. Sensors are skipped, and you can exclude bodies. That way a
  robot's sensor doesn't see its own limbs.
- **Sphere query:** like the ray query, but with a sphere pushed along instead of a thin line. A
  "thick ray" doesn't miss any edge, good for ground probes, for example: is the foot standing on
  solid ground?
- **Explosion:** a kick that spreads out like a sphere from a point and pushes away everything
  nearby. With negative strength, it becomes a suction.

The clubs from 6.4 let you choose who a query sees at all.

## 6.9 Contacts and events

After every step, the physics reports what happened: two shapes start or stop touching. Something
enters or leaves a sensor. An impact was hard enough for a sound. A joint has broken.

> **Imagine …** a news ticker after every move in a game. But the ticker costs work, so you only
> subscribe to the messages you actually read.

> **Watch out!** Events cost computing time. That's why the game runtime only switches them on
> where the game really reads them – on the player, for example, so that they can collect things
> and step into invisible triggers.

You can also ask directly: "Which bodies is this body touching right now, and how firmly?" That's
how the Micro Duck knows whether its foot is touching the ground at the moment – just like a touch
sensor in a real sole. Studio shows the contact points with **Window › Show › Contact Points**.

## 6.10 MouseGrab: pulling bodies with the mouse

> **Imagine …** you hang an object on a rubber band and pull on the other end. It follows you,
> swings, bumps into things, and when you let go, it flies on with momentum.

That's exactly how the mouse grip works: an invisible point follows the mouse pointer, and a damped
spring connects it to the spot where you grabbed. The spring doesn't swing on and never pulls
harder than a multiple of the weight. The body may turn freely: it tips and swings on the grip. For
the Micro Duck's torso, the grip also holds the orientation, like a hand, so that it stays upright
when lifted. Static and kinematic bodies can't be grabbed.

> **Try it!** While **Simulate** is running, you grab any dynamic body with the left mouse button
> (or your finger). Moving the pointer up lifts, the mouse wheel brings it closer or pushes it
> away, letting go throws.

## 6.11 The Character Mover

Player characters do **not** move in a fully physical way. Real physics would leave a character
stuck on every curb and glued to walls. That's why the character calculates its movement itself,
outside the solver, and nudges things it walks into along the way.

> **Imagine …** the character stands on a pogo stick that always keeps it a little way above the
> ground. A small step simply slips under the stick, and the character is gently lifted to the new
> height. That way it walks up stairs and ramps without stumbling.

The character itself is a capsule. In every step it accelerates in the desired direction, checks
which surfaces it's touching, and slides along them instead of walking into them. So when it
pushes against a wall, no speed builds up – otherwise the character would shoot off as soon as the
wall ended. Surfaces that are too steep don't count as ground. The game runtime uses the Character
Mover for the player types **Character** and **Soldier** ([Chapter 8](08-game-runtime.md)).

> **For the curious:** The Character Mover has its own, snappier gravity, so that jumps feel good.
> All defaults are in the blue book, Chapter 6.

## 6.12 The beat: time step and sub-steps

> **Imagine …** a flip book. Every page is a physics step. If the pages are always equally far
> apart, the movie runs smoothly. If you sometimes skipped one page and sometimes five, everything
> would be jerky – and a spring could "rock itself up".

That's why Box3D **always gets equal time steps**, no matter how fast your screen is. The default
is 60 steps per second (60 Hz). You can change that with the level setting **Step rate**; but the
rules advise never going below 30.

But screens don't deliver their frames in time with the physics. How does that fit together?

> **Imagine …** a piggy bank. Every frame pays in the time that has passed since the last one. The
> physics withdraws it in fixed portions, as many as there are inside. Whatever is left waits for
> the next frame.

If the computer hangs for a moment (for example because a browser tab was in the background), it
doesn't catch up without limit. Otherwise catching up would take so long that you'd fall even
further behind, a vicious circle. Better to drop a few steps.

Simulation time **counts steps**, instead of adding up fractions of a second, where tiny rounding
errors would pile up over time. Input also applies per step: a key press or a command from the
controls counts for whole physics steps, not for a screen moment. That way the same result comes
out on every computer, no matter how fast its screen is.

**In-between frames:** what gets drawn is an in-between state of the last two physics steps,
depending on how much is left in the piggy bank. If there's still half a portion in it, Kern draws
the things exactly halfway between the last two physics frames. That way movement looks smooth at
any frame rate. The camera follows this smoothed state; with the raw state, fast cars would wobble
in the picture.

### Sub-steps

In Box3D, every step is split up once more into **sub-steps**, adjustable under **World Settings ›
Sub-steps**. More sub-steps mean that long chains of joints hold together better, because small
errors have less time to grow in shorter steps.

> **Did you know?** Measured at the duck's hip: with 4 sub-steps, the joint gap (the gap that opens
> between two connected parts) grew to 5 cm under load. With 16 sub-steps it stays under 1 mm. So
> when Studio inserts a robot, it sets 16 sub-steps and the robot's step rate (Micro Duck: 100
> steps per second of world time, see 6.15) and reports that.

Stiffness depends on the sub-steps too: **the stiffer a spring, the more sub-steps it needs.** A
spring that swings faster than the physics looks gets out of control. That's why Box3D and Kern
limit every spring to what the number of sub-steps can still handle cleanly. Stiffer only works
with more sub-steps.

## 6.13 Determinism

Kern is **deterministic bit for bit**: the same inputs give exactly the same result on a Linux PC,
on a Mac and in the WebAssembly module that the browser also uses, down to the last digit of every
number. All three are checked with every change.

Why so strict? Because physics blows up tiny differences within seconds. A difference in the last
digit quickly turns into a completely different step for a walking robot. And Kern needs things to
be the same: a training run should give the same network on every device, an error should be
reproducible, and a recording should play back the same everywhere.

The physics contributes to this with the fixed time step, by calculating everything one after
another instead of at the same time, and with Kern's own calculation rules for sine and co. Those
are calculations with angles that, for example, turn an angle of rotation into a direction. Every
device calculates them minimally differently, so Kern brings its own version. Why each of these
points is necessary is explained in [Appendix C](C-physics.md).

The check uses a **fingerprint**: the duck walks a short distance, after every step of its control
a short check number of its state is formed, and the list must be the same on all platforms
([Chapter 2](02-building-and-testing.md), [Chapter 14](14-lehrer.md)).

> **For the curious:** That's why for new code: no arithmetic shortcuts that are allowed to change
> the result in the last digit, and sine & co. only from Kern's own math library. The exact rules
> are in the blue book, Chapter 6.

## 6.14 Four principles

Kern has a binding list of rules for everything that creates bodies, shapes, joints or motors. It
is based on Erin Catto's own advice and on measurements in Kern. At its core are four principles:

1. **The right size.** Moving things between 10 cm and 10 m. Whatever is smaller is enlarged as a
   whole (6.1), instead of tinkering with the physics.
2. **Similar masses.** Neighboring parts shouldn't differ too much in weight, at most about ten
   times. If a block hangs on a thread, the joints stretch (why is explained in
   [Appendix C](C-physics.md) with the truck on the bicycle). That's why limbs that are too light
   get a minimum mass.

   And: **one rigid part is one body.** It's built as one body with several shapes, not from
   several bodies welded together – the solver never holds those completely rigid; they wobble
   against each other.
3. **Never outwit the physics.** No secret extra forces, no simply set velocities on bodies with
   joints or contacts, no home-made brake. That fights against the solver and pumps energy in.
   Measured: such a trick made the duck's torso jump. If something should stay softly upright, you
   use a Parallel joint instead, the roly-poly toy from 6.7: it goes through the solver instead of
   around it.
4. **Robots must be allowed to fall.** No supports, no locked axes for walking robots. They don't
   exist in the real world either – and a robot that gets propped up in the simulation learns
   nothing that helps it out there.

> **For the curious:** The complete list with all the rules, their reasons and a checklist for new
> robots is in the blue book, Chapter 6.

## 6.15 The servo model – bridge to the Micro Duck

Box3D calculates rigid bodies. A real servo does more: it steers itself toward the target, has
friction in its gearbox and reacts with a delay. For this, Kern has a small **device model** with
values in real units.

> **Imagine …** a hand pushing a lever toward a mark. The farther away the lever is, the harder it
> pushes, but never harder than its strength allows. If the lever gets fast, it brakes it a bit.
> The gearbox in between catches a little. And the instruction "Push it there!" always reaches the
> hand a moment too late.

These are the five ingredients:

- **Pulling:** the farther from the target, the more torque – like a torsion spring.
- **Braking (damping):** the faster the joint turns, the harder the servo brakes, like a shock
  absorber.
- **Maximum force:** from a certain distance on, the motor is already pushing with full force;
  farther away doesn't help any more.
- **Friction:** the gearbox holds the joint until enough force is applied.
- **Delay:** commands arrive a few hundredths of a second later.

Kern builds this only from Box3D's **own** joint parts, with no extra forces. The hinge's spring
takes over the pulling and braking. Why a spring? Box3D's joint spring stays calm at any stiffness,
because Box3D calculates it "backward" (that's explained in [Appendix C](C-physics.md)), and it gives
way like a real servo.

**The spring in Box3D's language.** Box3D doesn't describe a spring by its force but by how many
times per second it swings – in hertz, as in the key terms box. A servo, on the other hand, says
how hard it pulls per degree. But how fast a spring swings depends on how much hangs on it: a
rubber band with a bunch of keys bobs faster than with a full shopping bag. And how sluggishly the
two parts at a joint turn around its axis depends on their position – which changes with every
movement of the duck. That's why Kern converts the servo values into Box3D's language anew **in
every step**.

> **For the curious:** Erin Catto knows another recipe too: a motor that turns faster the farther
> away the target is. But that would be rigid, and during training the network got to know a servo
> that gives way.

For the maximum force there's a trick: Kern never moves the spring's target farther away than the
spring can pull with full force. That's like a rubber band that you're never allowed to stretch
beyond a mark: if you pull your hand farther away, the band slides along, but it doesn't pull
harder. That way the spring can never pull harder than the real motor. The friction is a motor that
wants "speed zero" and has only a little force for it: it holds the joint until someone pushes
harder.

The same calculation runs in Studio, in the Lehrer (the training program) and on the test bench for
the measurements, for every servo joint in every physics step.

> **For the curious:** The calculation is only a few lines long and is in the blue book, Chapter 6,
> with an explanation. The recipe for the joint friction comes from Erin Catto himself.

### Servos in the level

Every joint of the type **Hinge (revolute)** in the level can be turned into a servo. It then gets
a target angle, a stiffness, a damping and a friction; its maximum force is the joint's motor
force.

### Profiles and spread

All the values of a servo type are in a **profile**. A different servo is simply a different
profile. Profiles are fitted to measurements of the real device and checked against other
measurements ([Chapter 16](16-calibration.md)). The default profile is the Dynamixel XL330, the
Micro Duck's servo, with the values Pollen Robotics trained it with.

Every profile also says how **certain** its values are. During training, Kern draws slightly
different values within these limits for every servo.

> **Imagine …** a basketball player who always practices with the same ball. At the tournament the
> ball is a bit harder – and he stops scoring. Whoever practices with slightly different balls
> scores with any of them. In the same way, the network learns to cope with servos that are a bit
> different from what was measured.

### The boundary with Box3D

The duck lives in the world enlarged four times (6.1); the profiles speak real units. Kern converts
in exactly **one** place, when the values are written into the duck. The delay is converted too,
into whole physics steps.

**Worked through:** the XL330 reacts with a delay of 20 ms (thousandths of a second). In the
enlarged world, time runs twice as long, so that becomes 40 ms. The duck's world calculates 100
steps per second of **world time** (that's what **Step rate** in Studio shows), so one step lasts
10 ms. 40 ms is therefore exactly **4 steps**. Measured by the duck's own clock (the **duck
time**, which runs twice as slowly), that's 200 steps per second, by the way, as in the training of
the real robot – both numbers mean the same thing ([Chapter 13](13-micro-duck.md)). The commands
wait in a queue for that long before they reach the servo.

How the Micro Duck walks with this is in [Chapter 13](13-micro-duck.md); how profiles are measured
is in [Chapter 16](16-calibration.md). Automatic tests safeguard all of it, from the bodies through
all nine kinds of joint to the fingerprint.

## What you take away

- Box3D calculates. Kern never adapts Box3D, only the model.
- Moving bodies belong between 10 cm and 10 m; small robots are enlarged as a whole (duck: length
  ×4, time ×2) – just a change of units.
- There are static, kinematic and dynamic bodies; shapes and materials decide the contact.
- Some pairs have their own, measured friction: the sock sticks to the beak and slides on the
  floor.
- Joints connect bodies, have springs, limits, motors and can break.
- The physics calculates in fixed steps, saves up like a piggy bank, and an in-between state is
  drawn. More sub-steps hold chains of joints together more firmly.
- Never outwit the physics; let robots fall.
- A servo pulls toward its target like a spring, but only up to its maximum force, with some
  friction and delay – using values from a profile.

## Brain teasers

1. Why doesn't Kern simply change Box3D's slack instead of enlarging the duck?
2. A ball never bounces even though its restitution is 0.9. It only falls slowly. Why?
3. A sock (surface: fabric) lies on a floor with no surface at all. Which friction applies?
4. In every step you set the velocity of the duck's torso so that it stays upright. What's wrong
   with that?
5. A different servo reacts with a 30 ms delay. The duck lives in the world where time runs twice
   as long and calculates 100 steps per second. How many steps of delay is that?

### Solutions

1. The slack applies to the whole world, and in the same level, cars stand next to small robots.
2. It's too slow: very gentle, slow impacts never bounce, so that nothing jitters forever.
3. There's no separate value for this pair, because the floor has no surface. So the usual average
   of the two shapes' friction values applies.
4. That outwits the physics: it fights against the solver and pumps energy in (measured: the torso
   jumped). And a robot that gets help learns nothing for the real world.
5. 30 ms × 2 = 60 ms in the world. One step lasts 10 ms, so 6 steps.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
