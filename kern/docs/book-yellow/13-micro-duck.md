# 13 The Micro Duck – a Real Robot, Servo by Servo

> **Your level:** Now it gets real. The Micro Duck is a walking robot that actually exists, and
> Kern runs, right in your browser, the very networks that control the real robot. You'll get to
> know its motors, sensors and networks – and everything the duck can do.

The Micro Duck comes from the company **Pollen Robotics**: about 25 cm tall, 0.74 kg, with 14
**servos** of the type Dynamixel XL330 in its legs and neck and a fifteenth one for the lower beak.
A servo is a small motor that you give an angle, and then it moves there.

Pollen didn't teach the duck to walk with rules but with **neural networks**, trained in the
physics simulation MuJoCo (repository `microduck_rl`, framework mjlab, learning algorithm rsl_rl).
The finished networks are saved as **ONNX** files, and those are exactly what the real robot loads.

Kern rebuilds the robot on its physics engine Box3D and runs the same networks. The goal is called
**fidelity**: the same model, the same servos, the same timing. On top of that come three things of
Kern's own: the **Lehrer** (German for "teacher") for training ([Chapter 14](14-lehrer.md),
[Chapter 15](15-your-own-models.md)), the **jump** (`jump.onnx`) and the **play layer**
(`play.onnx`).

![The Micro Duck level in the Studio (duck shown in this book's own design)](../buch/bilder/duck-level.png)

> **Mission:** Open Kern Studio at https://kernengine.de/ and choose **File › Example Levels ›
> Micro Duck**. Press **Play** (Alt+P) and use the Observer camera to watch the duck explore the
> room. Press **F8** to take it over, and F8 again to go back to watching.

## 13.1 Where the code lives

Almost everything is in `kern/src/game/`: `walker.cpp` (build, controller, sensors, driving),
`walker_policy.cpp` (networks), `servo.cpp`, `walker_brain.cpp` (behavior) and `walker_play.cpp`
(play layer). Pollen's networks and blueprints are in `third_party/microduck/`, Kern's own networks
in `policies/`.

## 13.2 The golden rule: no cheating

A trained network only works if the simulation computes the same physics as in training:

- No helper forces, no holding it in place, no teleporting, no blending between networks. The duck
  stands only by its servos and foot contacts.
- A servo is made only of Box3D's own joint features (Section 13.4).
- Fixed time steps: the same inputs give **bit for bit** the same gait, on every device
  (**determinism**, [Chapter 14](14-lehrer.md)).

> **Watch out, trap:** A tiny invisible prop "so it doesn't fall over" sounds harmless. But the
> network would come to rely on it – and the real robot doesn't have one.

## 13.3 The model

### Blueprint, links, joints

Pollen describes the duck in an **MJCF** file, MuJoCo's blueprint format. An import tool turns it
into Kern's model data and stops if anything doesn't fit (joint names and order, home pose, the
structure of the network).

In Kern the duck has **16 links**: the trunk, 14 links for the networks, and the lower beak. Each
leg has five joints: hip yaw, roll, pitch, then knee and ankle. Then come the neck, head pitch, yaw
and roll, and as the 15th servo joint the mouth. **Yaw, pitch, roll** are the three directions of
turning: shaking your head "no," nodding "yes," tilting your head toward your shoulder.

Every joint has hard limits and a **home angle** for the home pose. Angles are measured in
**radians**: a full circle is 2π ≈ 6.28 rad, and 1 rad is about 57°.

> **Crazy but true:** Head yaw goes from −2.967 to +2.967 rad, about ±170°. The duck can almost
> look straight behind itself.

### Masses and armature

Mass, center of mass and **inertia** (how hard it is to set a part spinning) come from the
blueprint: trunk 0.199 kg, head 0.155 kg, 0.74 kg in total.

Inside the servo, a tiny motor turns through a 288 : 1 gearbox. Its spinning mass is called the
**armature**. Box3D has nothing like that, so every link gets an imaginary thin disc on its joint
axis. It's not exact, but with it the duck jitters a lot less (Section 13.14).

### What touches the world

What collides are **convex hulls**: like wrapping paper pulled tight around a part, with no dents
going inward. The soles have friction 1, the beak pad made of TPU rubber also 1, the servo housings
0 (as in training), the shells of trunk and head 0.5 – so a fallen duck doesn't slide around on
them like on ice.

Since the play layer was reworked, the physics knows **surfaces** with their own pair values:
cotton on cotton 0.5, TPU on cotton 0.8, cotton on a wooden floor 0.4. All other pairs Box3D mixes
as the **geometric mean** √(a · b). Example: sole 1 on a floor of 0.6 gives √0.6 ≈ 0.77. A surface
can belong to colliders, meshes or the world's ground (`groundSurface` in the level JSON); in the
Micro Duck level the ground is `floor`. Box3D allows up to 5 mm of overlap in contacts (slop);
measured, the soles sink only 0.3–0.6 mm into the ground.

### Scale 4

Box3D is built for bodies from 0.1 to 10 m, but the duck's links are only 2 to 4 cm. That's why
Kern builds the duck **4 times as big** and converts in exactly one place, following **Froude
similarity**: if you let everything grow in the right proportions, the big world behaves exactly
like the small one.

Length times 4, mass times 4³ = 64, time and speed times √4 = 2, turning speed times ½, torque and
servo stiffness times 4⁴ = 256, inertia times 4⁵ = 1024. Mass goes times 64 because a cube with
4 times the edge length holds 4 · 4 · 4 = 64 times as much. The network still gets real units:
Kern multiplies turning speeds by 2 first.

> **Picture it like this:** When a model house collapses in a movie, it only looks real if it's
> played in slow motion. Bigger things move more leisurely – and that's exactly what the factor √4
> for time says.

### Look, roller skates, adding a duck

The duck's look comes from Pollen's 3D models (license CC BY-NC-SA 4.0, non-commercial), which is
why the pictures in this book show a design of its own. The catalog also has **Micro Duck
(Rollers)**: skids with two free wheels each instead of soles, 23.5 mm higher.

```cpp
game::Level level;
const game::ActorId duck =
    game::add_micro_duck(level, {0, 0, 0});
```

Line 1 creates a level; lines 2–3 put the duck at (0, 0, 0) and set the world to 100 steps per
second with 16 sub-steps. Studio does the same when you insert the duck from the catalog.

## 13.4 The servo model

Kern rebuilds Pollen's measured model of the XL330 as a **device model**: a small law whose numbers
are kept in a **profile**.

| Number | Value | Meaning |
|---|---|---|
| K stiffness | 0.554 N·m/rad | how hard it pulls toward the target |
| D damping | 0.053 N·m·s/rad | brakes fast motion |
| F friction | 0.0048 N·m | friction in the joint |
| T maximum torque | 0.963 N·m | it can't push harder than this |
| A armature | 0.0018 kg·m² | spinning mass of the motor |
| Delay | 20 ms | the command arrives later |

The law: the servo pulls with `K · (target − angle)`, at most with T. Damping brakes, friction
holds against it.

**Worked example:** 0.5 rad away from the target, it pulls with 0.554 · 0.5 ≈ 0.28 N·m – not even a
third of its 0.963 N·m. Full force only comes at 0.963 / 0.554 ≈ 1.7 rad, almost 100°. So the duck's
servos are **soft**.

> **Picture it like this:** A rubber band between the joint and its target. The farther away, the
> stronger the pull, up to a limit. Damping is like honey in the joint.

One single function applies this law in every physics step – in Studio, in the Lehrer and on the
test bench from [Chapter 16](16-calibration.md). Box3D computes the spring so that it always stays
stable; friction is a motor with target speed 0. Every duck, even every single servo, can get a
profile of its own.

> **Watch out, trap:** The real servo's firmware can limit the current to 1.75 A (0.64 N·m). Then
> the duck can no longer get up off its back. Pollen trains without this limit, and so does Kern.

## 13.5 The timing

In training, one physics step lasts 5 ms. In Kern it lasts 10 ms of world time, with 16 sub-steps.
Why that fits: in the 4-times-bigger world, time runs √4 = 2 times slower, so 10 ms of world time
is exactly 5 ms of robot time.

Every **4 physics steps**, Kern asks the network – that's the **control tick**: 20 ms, so 50 times
per second. Counting only every 4th step is called **decimation**. The command delay is 4 steps too
(20 ms; in training 15–30 ms): the servos always get the target from 4 steps earlier. If the
world's step rate doesn't fit, Kern rounds to whole steps and reports "set World Settings > Step
Rate to 100". Fixed steps are explained in [Chapter 8](08-game-runtime.md).

> **Picture it like this:** The drummer plays sixteenth notes (physics), the singer comes in on
> every fourth beat (control tick), the choir only on every eighth – that's the play layer, with
> 25 decisions per second.

## 13.6 The duck's networks

Kern loads **the same ONNX files** as the robot. The walking and standing network `velstand.onnx`
is built in; Studio loads the others later. Kern reads ONNX itself and accepts exactly one shape:
normalize the inputs, layers of neurons with the function **ELU** in between, and a linear layer at
the end. Everything else it rejects, with a reason. The built-in velstand computes the same as the
reference to within 2 hundred-thousandths.

| Motion | Network |
|---|---|
| Walking and standing | `velstand.onnx` (v8) |
| older gait | `alpha_walking`, `alpha_stand` |
| Sit / stand up | `alpha_sitstand.onnx` |
| Pick up a sock | `alpha_ground_pick.onnx` |
| Kick left / right | `ball_kick_left` / `_right` |
| Roll | `roulade.onnx` |
| Roller skating, crouching | `roller`, `roller_crouch` |
| **Jump** (Kern) | `jump.onnx` |

Walking runs all the time; the others are **episodes** with a fixed length, for example kick 0.5 s,
roll 1 s, jump 2 s. Picking up gets a phase with a 4 s period and ends at 70 %, so after 2.8 s.
Sitting down takes 2 s, standing up 1 s; the roll keeps going as long as you hold the key. The beak
is a servo without a network.

### Whose turn is it?

On every control tick, Kern follows the same order as Pollen's control program robotd: end motions
whose time is up; choose a network (priority: a running motion, picking up, sitting, rolling,
standing, otherwise walking); compute and convert into servo targets; move the time windows on.
Each network gets exactly the commands it was trained with.

A motion only starts if none is running, the duck isn't sitting and the network is loaded –
otherwise you get the reason back. If a network is missing, no other one steps in: the servos hold
the home pose, and the duck reports once which file is missing.

### From network to servo target

Every servo target is **home angle + scale · output**, at most ±π. Kern runs every network **as
trained**: scale 1, no smoothing (only roller skating and crouching: 0.8). Pollen's program on the
real robot uses scale 0.9 and a **low-pass filter**: new target = 0.7 · network value + 0.3 · old
target (head: 0.5). The field **Actions to servo goals** chooses `training` (default) or `robot`.

> **Mission:** If the level is running, stop it with **Esc** – you can't change settings during
> Play. Select the **Micro Duck** in the Outliner and set **Actions to servo goals** to `robot` in
> the Details panel. Start Play, take over the duck with **F8** and press **3** for the kick. Then
> do the same with `training`. Measured, the kicks topple over with the robot profile and stay
> upright with `training`.

## 13.7 What the network sees: 61 in, 14 out

| Slots | Contents |
|---|---|
| 0–2 | how fast the trunk is turning |
| 3–5 | where "down" is; upright (0, 0, −1) |
| 6–19 | joint angles minus home pose |
| 20–33 | joint speeds (one tick old) |
| 34–47 | its own last output |
| 48–50 | walking command: forward, sideways, turn |
| 51–54 | head command |
| 55–60 | body pose; the jump uses 55/56 |

The walking command goes up to ±0.4 m/s forward, ±0.3 m/s sideways, ±1 rad/s turning. It's
smoothed just like on the robot: every tick moves 20 % closer to the target (**EMA**). So from 0 to
1 it goes 0.2 – 0.36 – 0.49 …

velstand computes: normalize, then 512, 256 and 128 neurons, then 14 outputs – the 14 servo
targets. What a neuron is, is explained in [Appendix A](A-neural-networks.md).

> **For pros:** Why does the network get its own last output back? That way it knows its last
> command, even though the servos only get it 20 ms later. Kern fills these slots exactly as in
> training.

## 13.8 Sensors

The duck sees only what the real robot sees, from the same spots.

**Head ToF (VL53L8CX):** "Time of Flight" measures how long light takes to get there and back.
8 × 8 zones, field of view 45° × 45°, 4 m range, 15 frames per second. In Kern each zone is a
**ray test** (ray cast) that skips the duck's own links. Each zone counts as **floor** (a ray
pointing down that reaches at least 85 % of the sensor's height), **too close** (horizontal, under
10 cm) or **obstacle** – plus distance, direction and whether the left or right side is free
(blocked under 1 m).

**Head camera (IMX219):** mounted sideways, 62° high, 37.4° wide. "Does it see the ball?" means:
the ball's center is in the image, and a ray toward it hits the ball first.

**IMU:** feels turning and where down is. "Fallen" means: clearly tilted for 200 ms (the value for
down above −0.5). A quick stumble doesn't count. velstand v8 gets back up on its own.

**Mouth encoder:** an encoder measures the servo's angle. If the mouth is set to "closed" but is
still more than 2° open, something is stuck in it. Empty, it closes at 0.0°; a sock holds it
3.6–7.8° open. It travels from −5° to 30°.

> **Mission:** During Play, Studio shows **HEAD CAMERA** (camera image and state) and **TOF 8×8**
> at the top right: red for obstacle (brighter the closer), green for floor, yellow for too close.
> Take over the duck with **F8**, steer it toward a wall and watch the grid. You turn the display
> on and off under Window › Show › **Robot Camera**.

## 13.9 Who's driving?

1. **Hand:** a stick (more than 5 %) or a robot key gives you priority for 2 s.
2. **Logic:** the Blueprint nodes **Walk** or **Look**.
3. **Autonomy:** the duck's own behavior, when **Autonomous** is on.

If the duck is standing still and nobody has steered its head for 2 s, the **idle gaze** from
Pollen's software takes over: head movements along smooth curves, slow drifting, and patterns that
Pollen calls "dozing" and "yawning."

### Picked up

When the real robot is picked up, its legs shouldn't keep walking in the air. For that, Pollen's
small network `pickup_detector.onnx` (about 8,000 numbers) reads the last second on every control
tick: 50 rows of 63 measurements each. Out comes a probability p for "is being held."

- **Pause** when p > 0.8 on 5 ticks in a row: the servo targets glide into the standing pose in
  300 ms, and the state is called "Held."
- **Resume** when p < 0.35 on 4 ticks, no earlier than after 300 ms. The networks start fresh.

Only a full second is evaluated, and only while the walking or standing network is driving. A
motion, sitting, roller skates or the Lehrer end a pause. Kern has no servo currents, but the
network doesn't read those columns anyway.

> **Mission:** Start **Simulate** (Alt+S) and lift the duck **by its trunk** with the left mouse
> button (during Play this works while you're watching with the Observer camera). The grab holds it
> in its orientation, like a hand. The legs stop. Put it down – it walks on.

## 13.10 On its own: the behavior program

When nobody is driving, a program chooses the behavior (in the code, the "brain"). It carries over
Pollen's program `autod` (commit `df9a750` from 01.10.2026) with all its states and numbers:

- **Chill** (stands, 3–7 s), **LookAround** (big glances), **Wander** (1–3 m to a rarely visited
  target), **TurnInPlace** (turning), **Zoomies** (full speed with "Wheee," only with more than
  1.2 m of clear path),
- **Stretch**, **Ruffle**, **Dance**, **Sneeze** – short sequences of moves and sounds,
- **GroundPick** (picking up), **Nap** (sitting, 12–40 s), **Petted** (being petted),
  **BallPlay** and **Kick**.

An **energy** from 0 to 1 weights the next state. An **obstacle model** keeps the duck at a
distance: under 0.45 m it brakes and swerves; under 0.15 m it turns away and avoids the spot for
5 minutes. A **novelty grid** of 0.5 m cells makes it prefer rarely visited targets; on top of that
come bursts of quacks and hearing: a voice gets an answer, a noise briefly wakes it. Kern has no
microphone and no touch sensor; hearing and petting come in through commands. Even the randomness
is deterministic. While a motion is running, or the duck is sitting down, standing up or has
fallen, the program doesn't pick anything new.

### BallPlay

Pollen's earlier, non-public software had a state called BallPlay: approach, line up, kick. Kern
rebuilds it from that description, with Pollen's kick network. It's **on by default**:

1. **See:** a ball (a sphere with tag `ball` or the name "Ball," at most 8 cm high) in the camera
   image.
2. **Approach:** head lowered, slower and slower, down to 35 cm. If the ball is more than 0.6 rad
   off to the side, the duck first turns on the spot.
3. **Line up:** small steps until the ball is 9 cm in front of and 4.2 cm beside the left foot –
   exactly as in the kick training. If it loses sight of the ball right in front of it, it steps
   back until it sees it again.
4. **Kick:** head straight, 3 s of calm, kick, quacks. After that, the ball is uninteresting for
   2–5 s.
5. **Give up:** after 6 s without seeing it (when it's far away) or after 45 s without a kick
   (then 20 s of rest); LookAround comes next.

It only kicks with the left foot, because the right kick sometimes tips over in Kern. The
"detector" is a geometric visibility test, not a network looking at the camera image. With **Kern
extras** and `play.onnx` loaded, the play layer takes over instead of BallPlay; while it's engaged,
the program starts neither picking up nor sitting.

## 13.11 The play layer

Pollen's networks are motions; none of them can see. What the duck does with a ball or a sock –
look at it, walk to it, kick it, pick it up, carry it off, spit it out – is decided by the network
`play.onnx`. Around it there's only perception, no hand-written play rules.

![The duck at play (duck shown in this book's own design)](../buch/bilder/duck-play.png)

The play layer sits where a gamepad would and answers 25 times per second with a walking command,
head, beak, and which motion should start (state "Play"). It needs **Kern extras** (Details ›
Controller), **Autonomous**, a mouth servo, no roller skates, and `play.onnx`. It's engaged as
long as it knows an object, something is in the beak or its motion is running – and nobody is
driving by hand or via logic.

> **Mission:** Stop the example level with **Esc**, select the **Micro Duck** and switch **Kern
> extras** off in the Details panel. Start Play: now it's not the learned network that plays, but
> BallPlay. Watch the difference when the duck spots the ball.

### Detector and tracker

Like the servo, the **detector** is a device model. For each kind of object it reports the nearest
visible one, measured at its center on the floor. A sock is an actor with tag `sock`; it counts as
seen if any part of it is visible. A ball higher than 8 cm or a sock higher than 3 cm doesn't
count – the sock in the beak, for example. On top of that come **noise** and **dropouts**.

**Worked example** with the example values from the code: the estimate scatters by 0.002 m +
0.01 · distance. At 1 m, that's 0.002 + 0.01 = 0.012 m, about 1.2 cm. The farther away, the less
accurate – like judging distance with one eye. In Studio the detector is noise-free; in training
the Lehrer draws random values.

The **tracker** remembers an object that disappears from the image for 3 s (30 s if it's less
than 0.35 m in front of the duck). It doesn't report a kicked ball for 15 s, or a sock that was
carried off for 40 s – a sock counts as carried off from 0.25 m. From the moment it grabs, it
counts how far and how long the duck has been carrying.

### 23 in, 9 out

The **23 inputs** are all in real meters from the duck's point of view (x ahead, y left): where
ball and sock probably are and how old that report is, where the left foot is, whether something
is in the beak, how far the beak is open, whether the duck is walking, picking up or kicking, where
down is, how far and how long it has been carrying, how long it has been standing or walking. It
never gets the true positions.

**Standing still** is measured at the legs: both soles on something that isn't the duck, and no
motion running. That's because Pollen's gait sometimes doesn't start at all on "a little forward."

The **9 outputs**: forward, sideways, turn, head yaw, head pitch, beak open, kick left, pick up,
bow the neck. The walking commands have a **dead zone** like a gamepad stick: below 20 %, the duck
stands. All values are first limited to ±1.5.

- **Backward**, it starts off the way Pollen's `autod` does: 0.6 s at 0.3 m/s, then never below
  0.15 m/s. Any slower, and the gait doesn't start at all from a stand.
- The head follows with smoothing 0.3; body pose and "Wheee" are off.
- A motion only starts if none is running and the beak is empty.
- During a pick, the network controls the beak itself (without the play layer, Kern extras opens
  it at phase 0.16 and bites at 0.24).

### Sock, ball, example level

The **sock** is seven cloth lumps of 12.5 mm, loosely heaped 3.4 cm high (four at the bottom, two
on top of them, one on top), 25 g in total. Soft springs (15 Hz, damping 0.7) connect neighbors:
whatever the beak grabs drags the rest along. Each lump has the surface `cloth` and rolling
resistance 0.5 – otherwise round lumps would roll, and the heap would creep away. In code:

```cpp
std::vector<game::Actor> sock =
    game::micro_duck_sock_actors();
```

Index 0 is the root "Sock." You remap ids, parents and joint targets as with `add_micro_duck`, then
everything goes into the level with `level.add`.

The **ball** in the example level is 12 cm across, weighs 50 g and has rolling resistance 0.03 –
otherwise it would roll forever.

The **example level** is a 2.5 × 2.5 m room with a wooden floor (friction 1), a shoebox, a stack of
books, a crate, a flowerpot, a ball and two socks. The duck is autonomous, and Kern extras is on. A
level Blueprint makes the duck chirp once (node **Chirp**) when the ball comes into the camera
image – a demonstration of the Blueprint, not behavior of the duck.

### How well it works

32 episodes each; "variation" means with random deviations in build, servos and sensors:

| Result | clean | variation |
|---|---|---|
| Ball kicked away | 9 of 11 | 7 of 11 |
| Sock picked up | 4 of 21 | 13 of 21 |
| picked up, carried, spat out | 1 of 21 | 0 of 21 |
| Grips that held | 2 of 14 | 1 of 15 |
| Falls | 2 of 32 | 10 of 32 |

Honestly: kicking works well, carrying hardly at all yet. Two tests check that only the network
decides: a ball 30 cm in front of the duck gets kicked more than 25 cm away; a sock 30 cm in front
of it, the duck walks up to, picks up and bites.

## 13.12 Controlling the duck

| Motion | Key | Gamepad / touch |
|---|---|---|
| Sit / stand up | 1 | B / SIT |
| Pick up (on rollers: crouch) | 2 | Y / PICK |
| Kick left / right | 3 / 4 | – / KICK L, KICK R |
| Roll (held: keep going) | 5 | X / ROLL |
| Beak open / closed | 6 | – / BEAK |
| Quack | Space | RT / QUACK |
| "Wheee" (held) | V | LT / WHEEE |
| **Jump** | **7** | – / **HOP** |

Walking: W/S forward and back (up to 0.4 m/s), A/D to turn (up to 1 rad/s), or the left stick or
the MOVE stick. On the gamepad there's a round dead zone of 10 % and a curve with exponent 1.28. On
a phone, QUACK and PICK sit at the bottom, under your thumb.

The sounds follow Pollen's robotd: "greet" at the start and when the gamepad comes back, "inquire"
when it's gone, "chirp" on every new press of quack, "wheee" while held, "peck" when sitting down.
They come from a bit-exact port of Pollen's synthesizer, with a voice of its own for each duck
([Chapter 7](07-audio.md)).

**The jump:** key 7, HOP or the logic node **Robot Move** with "jump." Kern switches hard to
`jump.onnx` and back after 2 s; the network learns the time through slots 55 and 56, which Pollen's
networks never use. In Studio: 3 out of 3 real jumps, 9.0 mm rise, 120 ms of flight, 8° tilt, and
after that the duck stands and walks again.

> **Crazy but true:** Pollen's set of networks has no jump. This network was trained by Kern's own
> Lehrer, on the same physics you see in Studio ([Chapter 14](14-lehrer.md)).

### Loading networks

Only velstand is built in. Studio asks the engine for the other networks and loads them: Pollen's
from the folder `microduck/`, Kern's `jump.onnx` and `play.onnx` from `policies/`. If a file fails
to load, it asks again after 5, 10, 20 s …, at most once a minute. Networks you've trained with
the Lehrer in the browser are stored on your device (IndexedDB "kern-lehrer"). Studio loads them
too, and open Studio tabs pick up a new one immediately.

**Details panel:** **Autonomous (camera, ToF)**, **Walking network** (`velstand` or `alpha`),
**Actions to servo goals**, **Kern extras**; only in the level JSON: `rollers` and `servoDelay`
(default 0.02 s).

**Blueprint category Robots** ([Chapter 9](09-logic.md)): Walk, Look, Set Autonomous, Chirp, Robot
Move (`sit`, `pick`, `kick_left`, `kick_right`, `roulade`, `jump`), Robot Beak, Pet Robot, Robot
Hears, Robot ToF, Robot Camera Sees, Robot State, Robot Holds Something.

## 13.13 Controlling the duck from C++

```cpp
w.walker_command(duck, 0.2f, 0, 0, 3.0f);
w.walker_look(duck, 0.5f, 0.2f, 2.0f);
std::string why = w.walker_skill(duck, "pick");
bool holds = w.walker_holding(duck);
```

Line 1: 0.2 m/s forward, for 3 s. Line 2: head yaw and pitch. Line 3: start picking up; an empty
text means "started," otherwise it contains the reason. Line 4: is something in the beak? More
functions give you the sensors (`walker_sensors`, `walker_sees`), the running network
(`walker_driving`) and the state (`walker_state`). What the Lehrer needs on top of that is in
[Chapter 14](14-lehrer.md).

## 13.14 Measured

| Case | Kern | Pollen (MuJoCo) |
|---|---|---|
| Command 0 | stands, drifts less than 2 mm | stands |
| forward 0.4 m/s | ≈ 0.135 m/s | ≈ 0.17 m/s |
| turn 1 rad/s | ≈ 0.61 rad/s | ≈ 0.97 rad/s |
| on its back | stands after ≤ 12 s | gets up |

Without armature, Kern's duck jittered a good three times as much. All motions work; the left kick
sends the ball 4 m; the right kick sometimes tips over, and the duck gets back up. No joint goes
past its limit by more than 0.004 rad. BallPlay kicks even without Kern extras, without falling.

## 13.15 What's still missing

Gear backlash (±1°) is missing, the armature is only a disc, Box3D mixes friction differently from
MuJoCo, the right kick tips over, BallPlay is a rebuild, servo currents are missing. Only Kern has
the play layer, the beak timing and the jump. If you work with the real robot: the safety rules are
in [Chapter 16](16-calibration.md) – only together with an adult.

## Level complete!

- You know the Micro Duck: 25 cm, 0.74 kg, 14 servos plus the beak.
- You know why Kern never cheats with the physics.
- You can convert back and forth with scale 4 between Kern's world and the real robot.
- You can work out a servo's torque from K and the distance to the target.
- You know what the network sees (61 numbers) and what it commands (14 targets).
- You know ToF, camera, IMU, mouth encoder and the order hand – logic – autonomy.
- You can control all of the duck's motions, including Kern's jump.

## Puzzle corner

1. What does the duck's fifteenth servo move?
2. Why does Kern build the duck 4 times as big?
3. A servo is 0.2 rad away from its target. With what torque does it pull?
4. The duck has a sock in its beak. Why doesn't the detector report it?
5. **Boss puzzle:** In Kern, one physics step lasts 10 ms of world time. How much real robot time
   passes between two decisions of the play layer?

### Solutions

1. The lower beak. The other 14 are in the legs and neck.
2. Box3D computes best with bodies from 0.1 to 10 m; the links are only 2–4 cm.
3. 0.554 · 0.2 ≈ 0.11 N·m – far below the maximum torque of 0.963 N·m.
4. A sock only counts if its center is at most 3 cm high. In the beak it's higher.
5. One step is 5 ms of robot time, a control tick is 4 steps, and the play layer decides on every
   second tick: 8 · 5 ms = 40 ms.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
