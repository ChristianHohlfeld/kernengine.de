# 6 Physics – How Things Fall, Roll and Hold Together

> **In short:** Kern drops crates, rolls balls, swings doors and makes robots walk. The number
> crunching is done by **Box3D**, a physics engine by Erin Catto (the 3D successor of Box2D v3). Kern
> puts a thin layer on top of it. You'll get to know bodies, shapes, materials, joints, contacts, the
> rhythm of the simulation and the Micro Duck's servo model.

The fundamentals behind it (integration, collision, solver, control loops, determinism) are explained
in [Appendix C](C-physics.md). Here it's about **how Kern uses them**.

![The example level Physics Lab in Kern Studio: stack, pendulum, door, rope bridge, turntable, lift and rover](../buch/bilder/studio-physics-lab.png)

One ground rule first, because it shapes everything: **Box3D is never adapted to Kern.** Neither its
source code nor its constants are changed, and Kern doesn't turn solver knobs to "get a model to
work". If a model doesn't fit, the **model** is adapted (scaled, masses redistributed, more sub-steps),
not the physics. Whatever a real device does on top of that, such as a servo with a controller and
friction, gets its own small device model in real units (section 6.17).

## 6.1 How the layer is built

Box3D sits unchanged under `third_party/box3d` and is compiled into the same WebAssembly module as
everything else. On top of it sits `kern::physics` – the **only place** where Kern talks to Box3D. You
build it with `kern/build.sh --physics` ([Chapter 2](02-building-and-testing.md)).

The agreements:

- Meters, seconds, kilograms, **Z up** – just like in the renderer.
- In physics, angles are in **radians**. Degrees only exist in level files and in the Details panel.
- Bodies, shapes and joints are **handles** (numbers, 0 means "none"); the `World` owns everything and
  frees it all when it is destroyed.
- Errors return an invalid handle and an explanation in `last_error()`
  ([Chapter 3](03-foundations.md)). Every input is checked: a pose with `NaN` or a negative radius
  never reaches Box3D.

## 6.2 The right size

Box3D calculates with fixed tolerances **in meters**: bodies may overlap by 5 mm ("linear slop"),
contacts are created from as far as 20 mm apart ("speculative distance"), and bounding boxes have up
to 50 mm of margin. That leads to the first rule: **Moving bodies belong between 0.1 m and 10 m, never
below 1 cm.** The game runtime enforces this: moving shapes are raised to at least 1 cm edge length
(spheres to a 5 mm radius), while fixed ones may be as thin as 2 mm.

> **Did you know?** Measured: the Micro Duck at real size has links of 2–4 cm. Its trunk sank 9 cm
> through its legs within 10 frames. At four times the size, the joints hold.

### Scale instead of adjusting tolerances

Box3D could change its tolerances, but only for the **whole** world – and in a Kern level, cars stand
next to little robots. So Kern scales the **model** by a factor S, and it scales every quantity to
match its dimension (Froude similarity). For the Micro Duck, S = 4:

| Quantity | Factor | at S = 4 |
|---|---|---|
| Length | S | ×4 |
| Time, speed | √S | ×2 |
| Mass | S³ | ×64 |
| Torque, stiffness | S⁴ | ×256 |
| Damping | S⁴·√S | ×512 |
| Moment of inertia | S⁵ | ×1024 |
| Frequencies | 1/√S | ×½ |
| Acceleration, gravity | 1 | stays 9.81 m/s² |

> **Imagine …** filmmakers shoot scenes with model ships and play them back slower so the waves look
> "real". Kern does it the other way around: the robot gets bigger, and one real second lasts two
> seconds in the world. The movement stays the same – just in units that Box3D handles well.

This is a **change of units**, not a bigger robot; to the outside world (networks, logs, calibration)
everything goes out in real units. One test builds a servo leg at 1, 4 and 8 times the size: hanging
freely, 1× and 4× run **bit for bit identically**. On the ground, where the fixed tolerances kick in,
they are only 0.23 to 0.27 mm apart. At 4×, Box3D's tolerances correspond to just 1.25 mm and 5 mm on
the real robot – finer than at real size.

Fixed settings are **calibrated, not scaled**. Because time runs twice as long, frequencies on the real
robot correspond to double, and speeds to half: a contact stiffness of 30 Hz in the world is a real
60 Hz, the servo joint softness of 240 Hz a real 480 Hz, the push-out speed of 3 m/s a real 1.5 m/s,
and the restitution threshold of 1 m/s a real 0.5 m/s. So differences from the real robot come from
the models of servo, contact and masses, which are matched against measurements
([Chapter 16](16-calibration.md)). The simulation is no guarantee of safe behavior on real hardware.
A robot of a different size simply gets a different S.

## 6.3 The world

Everything starts with a world and its settings:

```cpp
physics::WorldSettings ws;
physics::World world;
if (!world.create(ws)) { /* last_error() */ }
world.step(1.0f / 60, 4);   // one step, 4 sub-steps
```

Lines 1–3 create a world with default settings (or report why they couldn't). Line 4 moves forward by
one sixtieth of a second, split into four sub-steps.

The most important defaults: gravity 9.80665 m/s² downward, bodies at rest may sleep, continuous
collision is on, impacts below 1 m/s don't bounce, contacts are 30 Hz stiff with damping 10 and are
pushed apart at no more than 3 m/s, hit events start at an impact of 1 m/s, and the maximum speed is
400 m/s. A zero or a negative value for the contact values and the maximum speed leaves Box3D's own
default in place.

Kern always sets two things itself: **a single worker thread** (WebAssembly runs single-threaded, and
it makes the order unambiguous) and its own **friction function** for material pairs (6.6). Because
the Lehrer runs many worlds on several threads ([Chapter 14](14-lehrer.md)), a lock natively protects
the creation and destruction of worlds.

A `step` clears the events of the last step, ignores nonsensical time steps, calculates with 1 to 64
sub-steps, keeps count and collects the new events (6.10). How big the step is gets decided by the
caller – in a game, `GameWorld::step` (6.13).

## 6.4 Bodies

Box3D knows three kinds of bodies:

> **Imagine …** a theater. The stage floor never moves: **fixed** (static). The stage lift moves
> exactly as the crew commands and pushes along everything standing on it: **kinematic**. And the ball
> an actor throws obeys gravity, impacts and friction: **dynamic**.

When you create one, you specify the kind, starting pose, velocities, damping, gravity factor and a
free user value. With `bullet`, Box3D also checks a fast body against other moving bodies without any
gaps.

At runtime you can read and set the pose and velocity, drive a kinematic body to a target pose, lock
axes (`set_motion_locks`) and apply forces or impulses:

- A **force** acts for exactly **one** step. Continuous loads have to be set again every step.
- An **impulse** is a one-time push (in newton-seconds), for example for a jump.

Forces and impulses wake a sleeping body. Changing the type is possible, but expensive.

Mass, center of mass and inertia normally come from the shapes and their density. With `set_mass` you
set only the mass (the inertia grows to match; the body stays consistent). With `set_mass_data` you set
everything individually – the route for imported robot descriptions (MJCF, URDF) that bring their
measured values with them. The collision shapes then only determine the contact.

## 6.5 Shapes

So that a body can bump into things, it gets **shapes**, and it's fine to give it several (a
*compound*).

| Shape | What you specify |
|---|---|
| `box` | full edge lengths, placement |
| `sphere` | radius |
| `capsule` | radius, distance between the hemispheres along Z |
| `hull` | at least 4 points: convex hull |
| `mesh` | triangles counterclockwise, **only on fixed bodies** |

A **convex hull** is the shape you get when you stretch a rubber sheet around a cloud of points (more
in [Appendix C](C-physics.md)). Anything nonsensical is rejected: radius ≤ 0, flat hulls, a mesh on a
moving body. For meshes, Kern welds corners with a 0.5 mm tolerance and lets Box3D detect inner edges.
That way nothing trips over "ghost edges" between two triangles. If a hull has too many corners, Box3D
reduces them to its maximum number.

### Material and filter

Every shape has a material: **density** (default 1 kg/m³), **friction** (0.6), **restitution**
(bounciness, 0), **rolling resistance** (0), a **surface** (6.6) and the **Sensor** switch – then the
shape only reports overlaps and doesn't push anything. On top of that come collision filters:

- **Channels:** Every shape has category bits and mask bits. Two shapes only collide if each one
  appears in the other's mask.
- **Groups:** The same positive group always collides, the same negative group never does. Handy for
  robots whose links shouldn't get in each other's way.
- **Event switches:** which reports the shape generates (6.10).

`hull_mesh` builds the triangles of exactly the hull that collides (4 to 4096 points, not flat); Studio
draws them as lines.

## 6.6 Surfaces with pair friction

When two shapes meet, the contact needs **one** friction value. Box3D normally takes √(a · b), the
square root of the product (why is explained in [Appendix C](C-physics.md)). For restitution it takes
the maximum. That's good enough for most things. But the source code puts it well: *TPU on cotton has
its own measured value, not the average of TPU on TPU and cotton on cotton.*

That's why Kern has **surface materials**: `none`, `cloth`, `tpu` and `floor`. Certain pairs get their
own values (static friction of dry pairs):

| Pair | Friction | Source |
|---|---|---|
| cloth – cloth | 0.5 | cotton knit on knit |
| cloth – tpu | 0.8 | soft beak pads on cotton |
| cloth – floor | 0.4 | cotton on wood/laminate |

Kern doesn't change Box3D for this, but uses two docking points provided for it: the surface is
written into the shape as a material number, and the world gets Kern's friction function:

```cpp
float pair_friction(float fa, uint64_t ma,
                    float fb, uint64_t mb) {
  const f32 f = ma && mb && ma <= 3 && mb <= 3
      ? surface_friction(Surface(ma), Surface(mb)) : -1.0f;
  return f >= 0 ? f : sqrtf(fa * fb);
}
```

Box3D calls it for every contact, with the friction values (`fa`, `fb`) and material numbers (`ma`,
`mb`) of both shapes. Lines 3–4: if both have a real surface (1 to 3), it looks them up in the pair
table; if there's no entry, it returns −1. Line 5: if there is a value, it applies; otherwise the usual
square root does. So if one side is `none`, the shapes' own friction values always count. Restitution
and rolling resistance are left untouched.

> **Watch out!** If you add a new surface, you also have to adjust the `<= 3` limit, enter its name and
> write its pairs into the table. Otherwise it gets silently ignored.

In the level, the surface is stored as text (`"cloth"` and so on) in three places: on a mesh's
automatic collider, on a custom collider and on the endless ground. There's no dedicated field for it
in the Details panel (as of this code); it's set in the level document. Unknown names become `none`.

The Micro Duck uses all three ([Chapter 13](13-micro-duck.md)): the lumps of its sock are `cloth`, the
printed pads on its beak are `tpu`, and in the example level the wooden floor is `floor`. Its soles have
**no** surface: they keep the friction values its network was trained with (1.0 on 1.0 gives √1 = 1).

## 6.7 Friction, bounciness, rolling resistance

You set **friction** per shape. In the editor: Details › Mesh › **Physics** for the automatic collider,
or Details › Collider for custom ones. You'll find the endless ground under World Settings › Physics
(Box3D) › **Ground friction** and **Ground bounciness**.

**Restitution** (0 to 1) decides how strongly something bounces; Box3D takes the maximum of both
shapes. Impacts below 1 m/s don't bounce at all. **Contact softness** (Contact stiffness 30 Hz,
Contact damping ratio 10, Contact push-out speed 3 m/s) is in the World Settings as well.

**Rolling resistance** slows down things that roll. Box3D takes the maximum of both shapes times a
"rolling radius" (the radius for spheres and capsules, a quarter of the inner radius for hulls – boxes
are hulls in Box3D, too) and uses it to limit the braking torque. Kern uses it deliberately:

- the ball in the Micro Duck level: 0.03 ("a rubber ball on a wooden floor rolls to a stop"),
- the rover's tires in the Physics Lab: 0.02,
- the sock lumps: 0.5 ("a lump of fabric doesn't roll; round lumps would, and the pile crawls").

**Robot feet** are wide, flat boxes or hulls with high friction.

> **Try it!** Open the **Physics Lab** via **File › Example Levels**, select a crate and turn up the
> restitution in the Details panel under **Collider** (the crates have their own colliders; Mesh ›
> Physics only applies to the automatic one). Start Simulate (Alt+S) and throw the crate onto the
> floor with the mouse.

## 6.8 Joints

A **joint** connects two bodies and limits how they may move relative to each other. The textbook
example is a door:

```cpp
physics::JointSettings j;
j.type = physics::JointType::revolute;
j.body_a = frame_body;  j.body_b = door;
j.frame_a = physics::frame_from_axis(
    world.local_point(frame_body, hinge), {0, 0, 1}, false);
j.frame_b = physics::frame_from_axis(
    world.local_point(door, hinge), {0, 0, 1}, false);
j.enable_limit = true;  j.lower = -1.7f;  j.upper = 1.7f;
const physics::Joint joint = world.create_joint(j);
```

Line 2 picks a rotating joint (*revolute*, a hinge). Line 3 says it connects the door frame and the
door. Lines 4–7 build a **frame** (a small set of coordinate axes) at the hinge point for both bodies,
whose Z axis points straight up – the door turns around this axis. Line 8 limits the angle to ±1.7
radians (a good ±97°), so the door doesn't swing through the wall. Line 9 builds the joint.

A few agreements: revolute, spherical and parallel joints work **around the Z axis** of their frame,
prismatic and wheel joints **along the X axis**. `frame_from_axis` builds such a frame from an anchor
and an axis. The zero angle is where both frames line up.

| Joint (editor) | Box3D | Everyday picture |
|---|---|---|
| Hinge | revolute | door hinge |
| Slider | prismatic | drawer |
| Ball | spherical | shoulder joint, with a cone limit |
| Fixed | weld | weld seam (soft, too) |
| Distance / rope | distance | rod or rope |
| Wheel | wheel | wheel with suspension and drive |
| Motor | motor | pulls B toward a placement relative to A |
| Parallel | parallel | gently keeps something upright |
| No collision | filter | only prevents collision |

Many joints have three extras: a **spring** (frequency in hertz and damping ratio), **limits** and a
**motor** (target speed and maximum force or torque). A rope is a distance joint with a maximum length
(rest length at least 1 mm). A weld joint at 0 Hz is rigid, and soft with more hertz; the spherical
joint limits with a cone. Kern swaps limits that are the wrong way around, and negative maximum forces
become 0. Connected bodies don't collide with each other by default. And every joint has a **softness
in the solver** (Box3D's default: 60 Hz, damping 2). A joint that tries to connect a body to itself is
rejected.

At runtime you can switch motors, read a hinge's angle and rotational speed, query the **constraint
force** and constraint torque (how hard the joint is holding right now), set reporting thresholds and
remove joints (the bodies are woken up when you do).

### When joints break

With breaking thresholds (*Break force* and *Break torque* in the editor), Box3D reports a joint as
soon as it is loaded too heavily. The game runtime then destroys it, counts it, writes
"Joint on "…" broke" to the log and plays the breaking sound. That's how the rope bridge in the
Physics Lab tears.

> **Try it!** Start Simulate in the Physics Lab and tug hard on a plank of the rope bridge with the
> mouse: the chain gives way and swings. (You'll hardly tear it like this – the mouse grab has a
> limited force, and the weak joints only break under a heavy load.) Turn on **Collision Shapes** and
> **Constraints** under **Window › Show**, and you'll see the shapes and joints.

### The joint spring in real units

Box3D sets up springs as a frequency and a damping ratio. Servos and robot descriptions, however,
calculate in newton-meters per radian (stiffness) and newton-meter-seconds per radian (damping).
`set_joint_spring` converts at the boundary, exactly the way Box3D does internally: via the joint's
**effective mass**, that is, how heavy the rotation around the axis feels. Because that depends on the
pose, the function is called **every step**. The target angle is limited to ±π, and a stiffness ≤ 0
switches the spring off. Box3D solves the spring implicitly; it's stable at any stiffness. The formulas
are in [Appendix C](C-physics.md).

## 6.9 Queries: rays, spheres, explosions

- **`cast_ray`** shoots a ray and reports the nearest hit: point, normal, body, shape and how far along
  the line. Sensors are skipped, and so is a body you exclude – or a whole list (that's how a robot's
  sensor doesn't see its own links).
- **`cast_sphere`** pushes a sphere instead of a ray: a "thick ray" for ground probes that don't miss
  any edge.
- **`explode`** spreads a push out spherically around a point onto spheres, capsules and hulls; a
  negative value makes an implosion. A mask over the category bits chooses who the queries see.
- **`push_body`** gives a body a shove just strong enough that it doesn't stay in the way of a moving
  pusher. On top of that come two capsule queries for the character mover (6.12): how far the capsule
  can travel and which planes (at most 32) it touches.

## 6.10 Contacts and events

After every step, the events of **that** step are ready: shapes begin or stop touching, a visitor
enters or leaves a sensor, an impact was hard enough for a **hit event** (with point and impact speed),
a joint has exceeded its breaking threshold. Because a shape may have been destroyed in the meantime,
the events also carry the bodies.

> **Watch out!** Events cost computing time. The game runtime only switches them on where the game
> reads them: sensor events on sensors and players (with logic graphs, on all moving shapes), contact
> events only on the player (for pickups and triggers), hit events on players, on bodies with an
> impact sound, or with logic.

Two queries read contact data directly: `contact_points` returns all of a body's touch points with
normal, penetration depth and normal impulse – Studio draws them with the show flag **Contact Points**.
And `touching` tells you which bodies a body is touching right now, and how firmly. That's exactly what
a contact sensor reports, such as a sole on the ground; the Micro Duck uses it for its feet.

## 6.11 MouseGrab: dragging bodies with the mouse

> **Imagine …** you hang an object on a rubber band and pull on the other end. It follows you, swings,
> bumps into things, and when you let go, it flies onward with momentum.

That's exactly how `MouseGrab` works: an invisible kinematic "mouse body" follows the pointer, and a
motor joint with a spring connects it to the grab point. The spring has 7.5 Hz and is critically
damped (it doesn't keep swinging). Its force is limited to a hundred times the weight. The body may
rotate freely – it tilts and dangles from the grab. With `hold_rotation`, the grab also holds the
orientation in place, like a hand (rotational spring at half the frequency, maximum torque a quarter of
the maximum force). Studio uses this for the Micro Duck's trunk, so that it stays upright when lifted.
Fixed and kinematic bodies can't be grabbed; if the grabbed body disappears, the grab ends on its own.
Studio grabs in Simulate, after Eject and with the observer camera.

> **Try it!** In Simulate you can grab any dynamic body with the left mouse button (or your finger).
> Moving the pointer up lifts it, the mouse wheel pulls it closer or pushes it away, and letting go
> throws it.

## 6.12 The character mover

For player characters there's the `CharacterMover`, built on Box3D's experimental mover interface. The
character is a capsule that runs **outside the solver**: it doesn't fight against contacts, but
calculates its movement itself and gives a shove to dynamic bodies it walks into.

Defaults: 0.4 m radius, 1.8 m height, 6 m/s, jump at 6 m/s, sprint ×1.6, a quarter of the acceleration
in the air, surfaces steeper than 50° aren't ground, and its own, snappier gravity of 20 m/s². How hard
it shoves bodies can be set (0 = not at all).

Each step:

1. Friction, acceleration toward the desired direction, jump, gravity.
2. **Pogo spring:** a sphere probe measures the distance to the ground, and a damped spring (4 Hz,
   damping 0.7) keeps the capsule at step height (0.45 m). That's how it walks up stairs and ramps.
3. Up to five times: collect the touched planes, solve the movement against them and cut off the rest
   at the planes. So pushing against a wall doesn't build up any speed.
4. Dynamic bodies in the touched planes get a push.

Afterward, the mover knows its foot position and the touched planes (for triggers and pickups). The
game runtime uses it for the "Character" and "Soldier" players and makes their bodies kinematic for
this ([Chapter 8](08-game-runtime.md)).

## 6.13 The rhythm: time step and sub-steps

> **Imagine …** a flip book. Every page is one physics step. If the pages are always the same distance
> apart, the movie runs smoothly. If you skipped one page here and five pages there, everything would
> be jerky – and a spring could "build itself up".

Box3D **never gets a fluctuating time step**. The step rate is a level setting (**Step rate**, default
60 Hz, allowed 15 to 480 Hz; never coarser than 30 Hz). `GameWorld::step` uses an **accumulator**
following Glenn Fiedler ("Fix Your Timestep"):

- Every frame pays its time into a piggy bank. The physics withdraws it in fixed portions; the rest
  stays for next time.
- A long frame (say, a background tab) counts for at most 0.25 s, and at most 0.1 s of simulation runs
  per frame. This prevents the **"spiral of death"**, in which the physics never catches up. If you
  fall behind anyway, whole steps are dropped, and the rest in the piggy bank stays in rhythm.
- Simulation time **counts steps** instead of adding up decimal numbers that drift over time.
- Inputs apply per step; a controller that calculates at 50 Hz counts steps, not seconds.

Drawing happens **between** the last two physics states, weighted by what's left in the piggy bank.
That way movement is smooth at any frame rate. The camera follows this smoothed pose; with the raw
pose, fast cars would wobble on screen.

### Sub-steps

Box3D splits every step once more into **sub-steps** (World Settings › **Sub-steps**, default 4, 1 to
16 in the level). Long chains of joints stretch less with more sub-steps.

> **Did you know?** Measured at the duck's hip: with 4 sub-steps, the gap in the joint grew to 5 cm
> under load. With 16 sub-steps, it stays below 1 mm. So when Studio inserts a robot, it sets 16
> sub-steps and the robot's step rate (Micro Duck: 100 Hz) and reports this.

Stiffness depends on the sub-steps, too: Box3D limits joint springs to a quarter and contact springs to
an eighth of the sub-step rate. On top of that, Kern keeps every spring in the level below half the
sub-step frequency. Servo joints get a joint softness of 240 Hz (or less, if the limit demands it). So
stiffer is only possible with more sub-steps.

## 6.14 Determinism

Kern is **deterministic bit for bit**: the same inputs give exactly the same result on all tested
platforms (GCC on x86, Apple clang on ARM, WebAssembly). The physics contributes through a fixed time
step, a single thread, Box3D without SIMD shortcuts, switching off FMA contractions and Kern's own math
library for sine and friends. Why each of these points is necessary is explained in
[Appendix C](C-physics.md).

It is checked with a **fingerprint**: the duck walks 400 control ticks, each one is hashed, and the
hashes have to match the same reference file natively, as WebAssembly and on ARM
([Chapter 14](14-lehrer.md)). The rule for new code: `kern::dm::` instead of `std::` for sin, cos,
exp, log and co., no sums whose order depends on threads, no `-ffast-math`.

## 6.15 The rules

`docs/physics.md` is binding for any code that creates bodies, shapes, joints or motors. The rules are
based on Erin Catto's own documentation and talks and on measurements in Kern. The essentials:

- **Sizes:** meters, kilograms, seconds. Moving bodies 0.1 to 10 m. Scale small models as a whole,
  convert units only at the boundary to Box3D. Off-limits: changing Box3D's length unit.
- **Masses:** neighboring masses at most about 10 : 1 (Erin Catto's advice for extreme ratios: make
  light parts heavier and heavy parts lighter). A rigid part is **one** body with several shapes, not
  several welded together; children that only have a mesh are welded by Kern as shapes into the
  nearest ancestor that has a body. Light links get a minimum mass (the duck's 5 g hip brackets weigh
  at least 20 g).
- **Rhythm:** fixed time step, interpolation, camera on the smoothed pose, long chains with more
  sub-steps, springs below half the sub-step frequency.
- **Joints:** build them in the rest pose; the limits have to include zero. Filter self-collision with
  a negative group or a no-collision joint. Don't touch Box3D's default softness (60 Hz, 2), except for
  servos.
- **Motors:** Always set a **finite** maximum force. Servos are joint springs with physical values
  (6.17); the holding torque comes from limiting the error. **Not** "motor speed = factor × error" for
  robots with a trained network – that would be rigid control, but the network has learned a compliant
  servo. Joint friction is a motor with speed 0. Rotor inertia is a mass (a disk on the axis); what
  Box3D can't do is documented, not faked. Keep things upright with a parallel joint. Friction per
  shape, rolling resistance for round things.
- **Forces:** continuous loads as a force every step, jumps as an impulse, damping via Box3D's built-in
  damping – never home-made as a force against the velocity, which becomes unstable on light bodies.
- **Off-limits: going around the physics.** No extra torques that "imitate" something, no setting
  velocities on bodies with joints or contacts, no support forces for robots. Measured: a "gyro" at the
  velocity level made the duck's trunk jump.
- **Bodies:** create them at their final pose instead of teleporting them, set the type when creating.
  Move kinematic bodies via velocity or a target pose (a target slower than the sleep threshold is
  skipped). Characters run on the character mover. Imported robots bring their own mass, center of mass
  and inertia; joints that aren't built at their zero position carry their build angle as a reference.
  Motion locks only for vehicles, characters and fliers, **never for walkers** – a robot has to be able
  to fall, or it won't learn anything real.
- **Sleep and events:** leave continuous collision on; fast player vehicles are bullets. Events only on
  shapes whose reports the game reads. Players and robots never sleep (a slow gait lies below the
  sleep threshold of 5 cm/s).

The checklist for a new articulated preset sums it up: every moving part at real size from 10 cm up
(otherwise scale), one body per rigid segment, masses within range, built in the rest pose, servos as
joint springs, 8 to 16 sub-steps, self-collision via a negative group – and at the end a test: the
preset stands, its joints hold (gap below 1 % of the link length), and it deterministically fulfills
its task.

## 6.16 How the game runtime uses the physics

Games build their physics from the level ([Chapter 8](08-game-runtime.md)): `plan_bodies` plans bodies
and shapes (Studio draws the colliders from the same plan), `create_body` and `create_joints` carry it
out (including degrees → radians, materials, filters, events, masses, locks, spring limit, breaking
thresholds), and `fixed_step` drives the controllers, kinematic animations, traffic, walkers, servos
and mouse grab every step, calls `World::step` and processes the events.

## 6.17 The servo model – bridge to the Micro Duck

Box3D calculates rigid bodies. A real servo does more: it controls itself, has friction and reacts with
a delay. For this, Kern has a small **device model** with values in real units, the **profile**. A
different servo is a different profile; an actuator that behaves fundamentally differently would need a
different model. Profiles are calibrated against measurements and checked against other measurements.

The law in words: the torque is the stiffness K times the distance to the target (but at most the
maximum torque T), minus the damping D times the speed. On top of that come friction F in the gearbox,
the inertia of the rotor (armature) and a delay until a command arrives. It's implemented with Box3D's
own joint parts:

```cpp
void servo_step(physics::World& world,
                const physics::Joint& joint, f32 target,
                f32 stiffness, f32 damping,
                f32 friction, f32 torque) {
  const f32 q = world.joint_angle(joint);
  const f32 k = max_of(1e-6f, stiffness);
  const f32 reach = torque > 0 ? torque / k : 1e9f;
  world.set_joint_spring(joint,
      q + clamp(target - q, -reach, reach), k,
      max_of(0.0f, damping));
  world.set_joint_motor(joint, friction > 0, 0,
                        max_of(0.0f, friction));
}
```

Line 5 reads the current angle. Line 7 works out how far away the target may be at most: T divided by
K. Lines 8–10 set the joint spring to a target that is never farther away than that – so the force
never exceeds the maximum torque, just like a real servo whose drive saturates. Lines 11–12 switch on a
motor with target speed 0 and the friction as its maximum torque: it holds the joint in place until
more force is applied. That's Erin Catto's recipe for joint friction.

The same function runs in Studio, in the Lehrer and on the calibration test bench, for every servo
joint in every physics step.

### Servos in the level

A Hinge joint becomes a servo with `servo = true`. Defaults: target 0°, stiffness 50 N·m/rad, damping
2 N·m·s/rad, friction 0.1 N·m; the maximum torque is the joint's motor force. Servo joints get the
stiffer joint softness from 6.13, so that light links don't stretch.

### Profiles and the XL330

A profile (`ServoProfile`, can be saved as JSON) contains K, D, F, T, armature and delay – plus
**spreads**. They say how certain the values are. Training draws its own values for each servo within
these limits, so that the network doesn't depend on whatever the measurement couldn't pin down.
Nonsensical values are rejected; stiffness and maximum torque have to be positive.

The default profile is the Dynamixel XL330-M288 the way Pollen Robotics trains it (model "m6" from
BAM – Better Actuator Models by Rhoban – at 7.4 V):

| Value | XL330 |
|---|---|
| Stiffness K | 0.554 N·m/rad |
| Damping D | 0.053 N·m·s/rad |
| Friction F | 0.0048 N·m |
| Maximum torque T | 0.963 N·m |
| Armature A | 0.0018 kg·m² |
| Delay | 20 ms (training 15–30 ms) |
| Spread | armature, friction ±10 % each |

### The boundary to Box3D

When the values are entered into the duck, they are scaled – at exactly this one place: stiffness,
friction and maximum torque times S⁴, damping times S⁴ · √S, inertia times S⁵. The armature is added
as a thin disk on the joint axis. (An inertia only around the axis wouldn't be a real rigid body, and
Box3D would gain energy from it.) The delay runs through a queue in whole physics steps: at 100 Hz and
S = 4, a real 20 ms is exactly 4 steps.

How the Micro Duck walks with this is in [Chapter 13](13-micro-duck.md), and how profiles are measured
is in [Chapter 16](16-calibration.md).

## 6.18 Tests

Eight tests safeguard everything, from bodies and events through all nine joint types, scaling and
materials, up to the Physics Lab with its breaking bridge, the character mover and the fingerprint.

## What you take away

- Box3D does the calculating; Kern never adapts Box3D, only the model.
- Moving bodies belong between 0.1 and 10 m; small robots are scaled as a whole (duck: S = 4).
- There are fixed, kinematic and dynamic bodies; shapes and materials determine the contact.
- Surfaces like `cloth`, `tpu` and `floor` give material pairs their own measured friction.
- Joints connect bodies, have springs, limits and motors, and can break.
- A fixed time step, an accumulator and interpolation make for smooth, repeatable movement; sub-steps
  make joint chains stiff.
- A servo is a joint spring with a limited target plus a friction motor – with values from a profile.

## Brain teasers

1. Why doesn't Kern simply change Box3D's length unit instead of scaling the duck?
2. A ball never bounces, even though its restitution is 0.9. It only falls slowly. What's going on?
3. A sock (`cloth`) lies on a floor without a surface (`none`). Which friction applies?
4. You set the velocity of the duck's trunk every step to keep it upright. What's wrong with that?
5. How many physics steps of delay does a servo with 30 ms have at 100 Hz and S = 4?

### Solutions

1. The tolerances apply to the whole world, and cars stand next to little robots in the same level.
2. Impacts below the restitution threshold of 1 m/s don't bounce.
3. No pair friction, because one side is `none`: √(a · b) from the shapes' friction values.
4. Setting velocities on bodies with joints goes around the solver and pumps energy in (measured: the
   trunk jumped). And a robot that gets help learns nothing for the real world.
5. 30 ms × √4 = 60 ms of world time, so 6 steps at 100 Hz.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
