# 17 Cheat Sheet: Keys, Files, Terms

> **Your level:** The last level is your reference book. Here you'll quickly find the most important
> keys in Kern Studio, the files Kern works with, and a big glossary with all the technical terms in
> the book, from "Actor" to "Warm start." Forgetting is allowed – and so is looking things up.

## 17.1 Keys in Studio

Keyboard shortcuts are shortcuts: instead of clicking through menus, you press a key. Kern Studio
uses the same layout as the Unreal Engine, a big professional tool for games. If you know one, you'll
find your way around the other. **F1** shows the most important shortcuts in Studio too.

### Camera in the Viewport

The **viewport** is the window through which you look into the 3D world.

| Input | Effect |
|---|---|
| right mouse button + W A S D | fly |
| … + Q / E | down / up |
| … + Shift | 2.5 times faster |
| … + mouse wheel | flying speed 1–8 |
| Alt + left mouse button | orbit around the selection |
| middle mouse button | pan |
| F | frame the selection |
| Ctrl+1 … 9 / 1 … 9 | set / jump to bookmark |

On a phone: one finger orbits, two fingers zoom and pan, a tap selects.

### Editing

| Key | Effect |
|---|---|
| Q / W / E / R | Select / Move / Rotate / Scale |
| [ / ] | grid smaller / bigger |
| Delete | delete selection |
| End | drop selection onto the ground |
| H / Ctrl+H | hide / show all |
| G | **Game View**: view as in the game |
| F2 | rename |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+C / V / D | copy / paste / duplicate |
| Ctrl+G | group |
| Ctrl+S / Ctrl+O / Ctrl+N | save / open / new level |
| Ctrl+Space | **Content Drawer** |
| Alt+2 | wireframe only: **Wireframe** |
| Alt+3 | colors without light: **Unlit** |
| Alt+4 | normal with light: **Lit** |
| Alt+5 | **Player Collision**: all collision shells over the darkened picture |
| Alt+P / Alt+S | **Play** / **Simulate** |

### Play and Simulate

In **Play**, you play yourself. In **Simulate**, the world runs without a player character: you
watch with the editor camera, grab bodies and keep positions with K – you can only edit the level
after stopping.

| Key | Effect |
|---|---|
| Esc | **Stop** |
| F8 | leave / take over the player |
| Pause | pause / continue |
| . | paused: one step further |
| Ctrl+R | restart the game (not with the soldier) |
| K | while simulating: **Keep Simulation Changes**, meaning take the positions of things into the level |
| Shift+F1 | free the mouse |

The **K** key is handy: while simulating, let a stack of crates fall over, select them and press K.
Then they'll still be lying the way the physics put them, even after stopping.

In the game: W A S D move, mouse looks around, Space jumps, Shift boosts, F uses.

### Logic Editor

- **F7** compiles: the logic graph is translated into fast machine code.
- **F9** sets a **breakpoint**: the game pauses when this node gets its turn. That way you can look
  at what's happening right now.
- **Ctrl+F** searches, **C** puts a comment around the selection, **Esc** takes you back to the
  viewport.

> **Mission:** Open Kern Studio on https://kernengine.de/ and press **F1** – that's
> **Help › Controls...**. Compare the window with these tables. Then try Ctrl+1 at a nice camera
> position, fly away and press **1** – you jump back.

## 17.2 Micro Duck Keys

W/S walk, A/D turn, **1** sit, **2** pick up, **3** and **4** kick, **5** roll, **6** beak, **7**
jump, **Space** quack, **V** "Wheee." Gamepad and touch are in [Chapter 13](13-micro-duck.md).

## 17.3 The Lehrer: Ducks Change the Network, Threads Only the Speed

When you train a network yourself, there's one rule you should know: **The number of ducks decides
which network comes out. The number of calculation helpers only decides how fast it goes.**

A **thread** is a strand of calculation: a processor with several cores can calculate several
threads at the same time. In the browser, the helpers are called **Web Workers**. Each helper takes
on a group of ducks and collects their experience.

> **Picture it like this:** A class of 32 students fills out a questionnaire. Whether two or four
> teachers collect the sheets doesn't matter, as long as all sheets are then sorted by student number
> and added up in the same order. The result is the same; only the collecting goes faster. But if
> you ask 64 students, other answers come in, and the result changes.

That's exactly how Kern does it:

- Every duck gets its own chance from its **number**, not from the helper that calculates it.
- Before learning, Kern sorts all collected data into a **fixed order**.
- Adding up always happens in the same fixed chunks, no matter how many helpers are calculating.

**Example:** 32 ducks on 2 threads and 32 ducks on 4 threads give exactly the same network, bit for
bit. The second run is just faster. 64 ducks give a different network.

> **Mission:** Open the Lehrer on https://kernengine.de/, choose "Stehen (Schnelltest)" (stand,
> quick test), seed 1, **2 Workers** with **4 ducks** each, and enter **20** under "Iterationen"
> (iterations). Press Start, wait until the page reports "Fertig" (done), and write down the number
> at "mittlere Episoden-Rückgabe" (mean episode return). Reload and start the same way, but with
> **1 Worker** and **8 ducks** (again 20 iterations). Both times it's 8 ducks in total – the numbers
> match exactly. For comparison, try 2 Workers with 8 ducks each: now it's 16 ducks, and the number
> is different. Always compare only at "Fertig": in the middle of a run, the two attempts might be at
> different iterations.

For young explorers, the Lehrer in the browser is the easiest way. The Lehrer also exists as a
program for the **terminal** (a window where you type commands). All its commands and settings are in
the blue book, Chapter 17.

## 17.4 Files

### Levels

A level is saved as a **text file** ending in `.klevel`. It contains everything about your world:
name, settings, camera, all actors, embedded images and sounds, and the logic. The format is called
**JSON**: names and values, neatly packed in brackets. The beginning looks something like this:

```
{
  "format": "kern-level",
  "version": 1,
  "name": "My Level",
  "actors": [ … ]
}
```

- Lines 1 and 6: the curly braces enclose the whole level.
- Line 2: this is how Kern recognizes that it's a level. If it's missing, Kern rejects the file.
- Line 3: the version of the format. Kern rejects a file from a newer Studio.
- Line 4: the name of the level.
- Line 5: the square brackets hold the list of all actors.

Kern calculates in meters, seconds and kilograms. **Z points up**: X and Y lie flat on the ground
like the axes on graph paper, and Z sticks straight up. Angles are stored in the file in degrees
(internally, for example for the duck's joints, Kern calculates in radians,
[Chapter 13](13-micro-duck.md)), colors as numbers from 0 to 1.

> **Mission:** Save a level with **Ctrl+S**. Depending on the browser, Studio asks for a location,
> or the file lands in your Downloads folder. Open the `.klevel` file with a text editor (Notepad on
> Windows, TextEdit on a Mac). Can you find `"kern-level"` and the names of your actors?

### Networks

Trained networks are saved as **ONNX** files. ONNX is to networks what PDF is to text: a format that
many programs can read, on the robot, in the browser and on the server. Kern reads Pollen's networks
and writes its own in the same format. The converter that brings the measurements to one scale
([Chapter 14](14-lehrer.md)) is included in the file – so the network comes with its own tape
measure. Networks you train in the browser stay on your device and replace the supplied ones there.

### Servo Profile

A **servo profile** is a small text file with a servo's numbers: how hard its spring pulls, how
strongly it brakes, how much force it has at most, and how uncertain each number is
([Chapter 16](16-calibration.md)).

### Logs

Measurements are saved as **CSV** files: a table as text, with the values separated by commas. It
also opens in a spreadsheet program. A test bench log contains the time, target angle and measured
angle of a servo. A robot log contains, 50 times per second, what the duck's sensors report and what
commands its network gives.

The exact fields of all files are in the blue book, Chapter 17.

## 17.5 For Developers: the Blue Book

If you want to write your own programs with Kern, you need the list of all functions through which
the browser talks to the engine, and all the Lehrer's commands. That's all in the blue book,
Chapter 17. Two principles from it are worth knowing for you too:

**The engine never touches files itself.** Everything it needs – levels, images, networks – is handed
in by its host: in Studio the browser, in the tests and the Lehrer program the program in question.
That's why the same engine runs everywhere unchanged; only the host that hands it the data is
different.

> **Picture it like this:** a cook who never goes shopping. The cook only cooks what's passed
> through the serving hatch. That's why the cook can work in any kitchen, whether in a restaurant or
> on a ship.

**Errors get reported, not swallowed.** If something goes wrong, for example because a file is
broken, the engine doesn't just crash. It says "that didn't work" and leaves a message about what was
wrong. That way you find out what caused it.

## 17.6 Glossary

All the technical terms in the book, briefly explained. The number in parentheses names the chapter
where it's covered in detail (A = Appendix A).

### Studio and Games

Studio controls are listed here with the English labels Studio shows.

- **Actor:** a thing in the level, with a position and building blocks (8, 11). Don't confuse it
  with the **actor in learning** (see Learning).
- **Autosave:** automatic backup of unsaved work, only in this browser (11).
- **Blueprint / logic graph:** a program made of nodes and connections, without typing (9).
- **Box / cuboid:** the same shape; Studio calls it a box, the geometry a cuboid (5, 8).
- **Breakpoint:** the game pauses when a certain node gets its turn (9).
- **Component:** a building block of an actor, such as a mesh, collider or sound (8, 11).
- **Console** (**Cmd** field): the line in Studio's status bar where you type commands (11).
- **Content Drawer:** the drawer with things to place (11).
- **Controller:** decides how the player moves: running, rolling, driving, flying (8).
- **Details:** the panel with all settings of the selected actor (11).
- **Dock:** the buttons for playing and simulating that always stay visible on narrow screens (11).
- **dt:** the time since the last frame in seconds; movement is multiplied by it (12).
- **Eject / possess (F8):** leave the player in the game / take it over again (11).
- **Engine:** the program that draws, calculates and plays a game world (1).
- **Event:** something that happens and sets something in motion in the logic graph (9).
- **Game loop:** read input, update the world, draw the picture – anew for every frame (12).
- **Game View:** view without editor helpers, like in the game (11).
- **Gizmo:** arrows and rings for moving, rotating, scaling (11).
- **HUD:** displays in the game, like points and time (8).
- **Kill Z (respawn below):** a height; if the player falls below it, they get reset (11).
- **Level:** a game world, saved as `.klevel` (8).
- **Listen server:** a server that plays along itself (10).
- **Logic editor:** Studio's tool for logic graphs; its labels are always in English (9, 11).
- **Module:** the translated file of the engine or a game that the browser loads (2, 12).
- **Mover / Rotator:** components that make a thing move back and forth / spin (8, 11). Don't
  confuse them with the **Character Mover** (see Physics).
- **Node:** a building block in the logic graph (9).
- **Observer camera (Play: fly, watch, drag bodies):** playing with a free camera for watching (11).
- **Outliner:** the list of all actors at the top right, like a table of contents of the level (11).
- **Output Log:** the logbook with all of Studio's messages (11).
- **Package Game:** export a level as a standalone web page file that you can pass on (11).
- **Pawn:** the player character that belongs to a player (8, 10).
- **Pin / variable:** a connector on the edge of a node / a labeled slot where a graph remembers a
  value (9).
- **Platform layer:** a small JavaScript helper between a C++ game and the browser (12).
- **Play / Simulate:** play yourself / let the world run without a player character and watch;
  editing only works after stopping (11).
- **Rigid Body:** the component that turns a thing into a physics body (11).
- **Snapping:** clicking into place on a grid (11).
- **Transform:** location, rotation and scale of an actor (8).
- **Trigger:** an area that sets something off when you enter it (8, 12).
- **Viewport:** the window through which you look into the 3D world (11).
- **Web server:** a program that delivers files the way a website does (12).
- **World Settings:** settings for the whole world, such as gravity and step rate (6, 11).

### Graphics and Geometry

- **Albedo:** how much light a color reflects: white 1, black 0 (B).
- **Ambient occlusion:** darkens corners and cracks that little sky shines into (4, B).
- **Bloom (glow):** bright spots shine out beyond their edges (4, B).
- **BVH:** boxes in boxes. Big boxes contain smaller ones, and right inside are the triangles. A ray
  only has to look into the boxes it hits – that saves a lot of searching (5, B).
- **Context loss:** the browser briefly takes the graphics card away; Kern rebuilds everything
  (4, B).
- **Depth buffer:** an invisible image that stores, for every pixel, how far away the frontmost thing
  so far is (4, B).
- **Dot product:** a number for how much two arrows point in the same direction (B).
- **Draw call:** an order to the graphics card: "Draw this mesh!" (4, B).
- **Fragment:** a pixel candidate whose color the graphics card calculates (4, B).
- **Frame:** a single picture; many per second make movement (4, 12).
- **Framebuffer:** an invisible image the graphics card draws into before it goes to the screen (B).
- **Fresnel:** looking at something at a flat angle reflects more – like on a wet road (B).
- **Frustum culling:** not drawing what's outside the viewing pyramid (5, B).
- **Gamma / sRGB:** conversion between "this much light" and "the way the eye sees it"; dark tones get
  more steps (4, B).
- **GPU (graphics card):** the chip that draws pictures and calculates thousands of small tasks at
  the same time (4, B).
- **Indices:** a list of numbers saying which three corners form a triangle (5).
- **Instance / instancing:** the same mesh again at another place / drawing all copies with a single
  draw call (4, 5, B).
- **Material:** what a surface looks like: color, roughness, metal (4).
- **Mesh:** a 3D shape made of triangles (4).
- **Normal:** an arrow pointing straight out of a surface; light calculates with it (5, B).
- **Particles:** lots of small points for sparks, smoke, dust (4).
- **Pass:** one run through drawing, for example for shadows (4).
- **PBR:** "physically based rendering," with tiny little mirrors (microfacets) (B).
- **Picking:** finding out what's under the mouse pointer (5).
- **Rasterization:** checking which pixel centers lie inside a triangle (B).
- **Renderer:** the part that draws the pictures (3, 4).
- **Shader:** a small program on the graphics card that calculates points and colors (4, B).
- **Shadow map:** an image from the sun's point of view that remembers the distance to the first
  surface (4, B).
- **Skinning:** deforming a mesh with bones, for example for characters (4, B).
- **Texture / texel:** an image on a surface / one of its pixels (4).
- **Tone mapping:** gently squeezing very bright light into the screen's range at the end (4, B).
- **UV:** two numbers per corner saying which spot of a texture gets stuck on there (5).
- **Vertex:** a corner point of a triangle, with a position and often a color, normal and UV (5, B).
- **Viewmodel layer:** gets drawn last after clearing the depth, so it's always in front, for
  example for the weapon in your hand or the gizmos (4, B).
- **WebGL2:** the way the browser uses the graphics card (4, B).
- **Welding:** merging duplicate corner points into one (5).
- **Winding order:** the order of the corners reveals whether you see a triangle from the front or
  the back (4, B).

### Math and Technology

- **Bit:** the smallest unit in a computer: 0 or 1. "Bit for bit the same" means: exactly the same
  (3, 10).
- **Browser console:** a hidden window in the browser for messages from web pages, often with F12
  (3, 15).
- **C++ / JavaScript:** the programming language of the engine / the language of web pages (1, 4).
- **CI:** automatic checking of every change on a server (2).
- **Compiler / compile:** a program that translates code into machine language that the processor
  runs directly (2, 12). In the logic editor, the button for it is called **Compile** (11).
- **CSV:** a table as a text file, with the values separated by commas (17.4).
- **Determinism / deterministic:** the same inputs give bit for bit the same result, on every device
  (6, 7, 9, 14).
- **Fingerprint:** a check number that reveals even the tiniest deviation – per tick for the
  simulation (2, 14), for a logic graph its content (9).
- **Function:** a small named part of a program that gets values and delivers a result (3).
- **GitHub:** the website where Kern's source code lives (2).
- **Hertz (Hz):** how often something happens per second. 50 Hz means 50 times per second (6, 7, 16).
- **Interpreting / compiling (JIT):** a logic graph is run node by node / translated into fast code
  during the game (9).
- **JSON:** a readable text format for data: names and values in brackets (3, 17.4).
- **Library:** a collection of ready-made functions that many programs share (3).
- **Matrix:** a table of numbers that describes rotating, moving and scaling all at once (3, B).
- **Processor / cores:** the calculating unit in a computer / its parts that can calculate at the
  same time – they have nothing to do with the Kern engine (2, 14).
- **Quaternion:** four numbers that describe a rotation (3, B).
- **Radian (rad):** a different angle measure from degrees. 1 rad is about 57°, a full circle about
  6.28 rad (6, 13).
- **Seed:** the starting number for chance in the computer. Same seed, same random numbers – that way
  every run can be repeated exactly (10, 14).
- **Source code:** the text of a program, the way humans write it (2).
- **Terminal / command line:** a window where you type commands instead of clicking (15, 16, 17.3).
- **Thread:** a strand of calculation. A processor with several cores calculates several threads at
  the same time (17.3, A).
- **Vector (arrow):** an arrow with a direction and a length, such as (x, y, z) (3, B).
- **WebAssembly:** a program format that browsers run very fast (1, B).
- **Web Worker:** a calculation helper in the browser that runs alongside the web page; on the Lehrer
  page they're called "Worker" (14).

### Physics

- **Body / shape:** what moves / what form it has (6).
- **Box3D:** the physics engine by Erin Catto that Kern brings along unchanged (6).
- **Character Mover:** Box3D's helper for player characters: the character floats as if on a pogo
  spring (6).
- **Collider:** the invisible shape that something collides with (6, 8).
- **Contact:** a spot where two bodies touch (6).
- **Convex hull:** a shape with no dents inward (13).
- **Damping:** the brake on swinging, like a shock absorber (6, 13).
- **Friction / restitution / rolling resistance:** sticking / bouncing / braking while rolling
  (6, 13).
- **Impulse:** a short push that changes the speed all at once (6, C).
- **Inertia / moment of inertia:** how hard it is to get something moving / spinning (6, 13).
- **Joint:** connects two bodies, for example as a hinge (6).
- **Newton (N):** the unit of force; 100 g push with just under 1 N (6, C).
- **Ray cast (ray test):** what does a ray hit first? (5, 6, 13)
- **Rigid body:** a thing that never bends, like a wooden block (6).
- **Sensor:** a collider that doesn't stop anything but only reports when something comes in (8).
- **Solver:** the part of the physics that tries to satisfy all contacts and joints at the same time
  (C).
- **static / kinematic / dynamic:** stands fixed / follows a given path and can't be pushed / real
  physics with falling and bumping (6, 8).
- **Step rate:** how many fixed steps the physics calculates per second; default 60, for the Micro
  Duck 100 per second of world time (6, 13).
- **Sub-steps:** the physics splits each step into smaller pieces so it calculates more precisely;
  for the Micro Duck 16 per step (6, 13, C).
- **Time step (fixed step):** the fixed time the physics moves forward by each time (6, 8).
- **Torque:** force times lever, measured in newton meters (N·m) – like with a wrench (6, 13, C).

### Sound and Networking

- **3D sound:** sound that comes from a spot in space (7).
- **Clip / loop:** a finished piece of sound / a sound that repeats endlessly (7).
- **Filter / low-pass:** a sieve for sounds; a low-pass only lets the low parts through, and in the
  same way it also smooths values so nothing jerks (7, 13).
- **Frequency:** how many times per second a sound vibrates; high = high-pitched sound (7).
- **Mixer:** mixes all sounds into one (7).
- **Mono / stereo:** one sound channel / two channels, left and right (7).
- **Pan value:** how far left (−1) or right (+1) a sound plays (7).
- **Reverb:** the echo of a room (7).
- **Synthesizer:** creates sounds from calculation instead of recordings (7, 13).
- **Authoritative:** only the server decides what really happens – like a referee (10).
- **Checksum (CRC32):** a control number that reveals broken or foreign packets (10).
- **Client / server:** fellow player / the computer that runs the game (8, 10).
- **Datagram:** a single packet of data on the network, with no guarantee (10).
- **Interpolation:** blending smoothly between two known photos; for this, the client shows the world
  0.1 s in the past (10).
- **Jitter:** how much the delay on the network wobbles (10).
- **KernNet:** Studio's little messenger that carries packets between tabs and computers (10).
- **Lag compensation:** the server turns back time to check hits (8, 10).
- **Ping:** how long a message takes to get to the other side and back (10).
- **Priority pot:** a points account per thing: moving things often get into the snapshot, resting
  ones rarely (10).
- **Registered mail / postcard:** the two channels: arrives safely and in order / sent once, lost is
  lost (10).
- **Relay:** a go-between that passes packets between browsers without opening them (10).
- **Replica:** the client's copy of the world, without its own physics and logic (10).
- **Room:** the name under which host and guests meet online (10).
- **Salt / challenge:** the client's random number when knocking on the door / the server's answer
  with its own random number (10).
- **Serialization:** packing numbers and text tightly into bits and unpacking them again (10).
- **Slot:** the player number the server gives a client (10).
- **Snapshot:** a photo of the game state that the server sends 30 times a second (10).

### Robots

- **Armature (the motor's momentum):** the small motor in the servo spins very fast through the
  gearbox. That's why it's hard to get going – it counts like extra weight on the arm (13, 16).
- **BallPlay:** Kern's rebuild of Pollen's ball-play behavior, a recipe in five steps (13).
- **Behavior program:** Pollen's program that picks the duck's next activity when there's no input
  (13).
- **Calibrating:** adjusting the simulation until it behaves like the real device (16).
- **Check measurement:** a measurement that isn't used for adjusting. Only it shows whether the model
  really fits (16).
- **Comparison:** the simulated duck replays the real robot's commands; you compare what both measure
  (16).
- **Control tick:** one round of "ask the network and give the command"; it happens 50 times per
  second in the real duck's time (13).
- **Daemon:** the control program that runs on the real robot (13, 16).
- **Detector / tracker:** estimates from the camera image where the ball and sock are / remembers
  where they last were (13).
- **Device model:** a small model of a real device, described by a profile (13, 16).
- **Encoder (angle sensor):** measures how a joint is positioned right now (13).
- **Gear backlash:** a little wobble in the gearbox when the gears don't mesh quite tightly (13, 16).
- **Home pose / home angle:** the duck's resting posture / the angle of each joint in it (13).
- **Idle gaze:** soft head movements from Pollen's software when the duck is standing still (13).
- **IMU (motion sensor):** senses where down is and how fast the body is turning (13).
- **Kern extras (ball play, sock fetching, beak in the pick):** the switch for Kern's additions to
  the duck (13).
- **Low-pass:** see Filter (13).
- **Micro Duck:** two-legged walking robot by Pollen Robotics (13).
- **Novelty grid:** remembers where the duck has already been, so it prefers visiting new places
  (13).
- **Picked up (detection):** Pollen's small network notices that the duck is being held; then the
  legs pause (13).
- **Play layer:** Kern's network that decides what the duck does with the ball and sock (13, 14).
- **RMS error:** combines many deviations into one number: square, average, take the square root. Big
  outliers count strongly (16).
- **Scale 4:** internally, Kern calculates with a duck four times as big, because the physics
  calculates badly with such tiny parts (13).
- **Servo:** a motor with a gearbox that moves to an angle (13).
- **Servo profile:** a servo's numbers: spring, force limit, brake, friction, momentum, delay, and
  how sure each number is (16).
- **Test bench:** a clamped servo with an arm and a weight that you measure on (16).
- **ToF:** a distance sensor that measures how long light takes to get there and back (13).
- **Walker:** the control type for robots that a network drives (13).
- **World time / robot time:** the clock in Kern's four-times-bigger world / the clock of the real
  robot; 1 second of world time is ½ second of robot time (13).
- **Yaw / pitch / roll:** the three directions of turning: shaking your head "no," nodding "yes,"
  tipping to the side (11, 13).

### Learning

- **Action / observation:** what the network answers / what it gets to see in one tick (15, A).
- **Action noise (σ, say "sigma"):** how much the duck randomly deviates from its plan while learning,
  to try out new things. It gets smaller during learning (14, A).
- **Activation / ELU:** the bend after each neuron: positive stays, negative gets squeezed, never
  below −1 (A).
- **Actor (in learning):** the network that acts – the "player." Only the actor later goes onto the
  robot (14, A). Don't confuse it with the actor in Studio (11).
- **Adam:** a clever method that adjusts each slider with a suitable step size and some momentum (A).
- **Advantage:** was an action better or worse than expected? Positive means better (A).
- **Agent / environment:** the one who acts (the network) / the world that responds to it (14, A).
- **Analyst:** = learner; reads all the helpers' notes and makes a new policy from them (14).
- **Asymmetric critic:** the critic sees more than the duck, such as its true height. It's allowed
  to, because it only exists while learning (14, A).
- **Backpropagation:** calculating backward how much each slider is to blame for the error (A).
- **Bias:** a base setting that a neuron always adds on (A).
- **Checkpoint:** a saved in-between state of learning that you can continue from. In the text it's
  called a **save game** (the whole learning state) or a **snapshot** (an in-between state of the
  network) (14, 15). Don't confuse it with the **snapshot** in networking (10).
- **CMA-ES:** a search method like evolution: try out lots of variants, keep the best, keep searching
  there (14, 16, A).
- **Collector / learner:** collects experience / learns from it. In the text: "helpers with
  clipboards" and "analyst" (14).
- **Converter:** = normalizer; see Normalizing (14, A).
- **Critic:** the second network in learning. It estimates how good a situation is, like a coach on
  the sideline. It doesn't go onto the robot (14, A).
- **Curiosity bonus:** = entropy bonus; a few extra points for the duck still trying things out
  (14, A).
- **Curriculum:** hard things only get added later, such as the shoves. Too much chaos at the start
  would prevent learning (14, A).
- **DAgger:** the network drives itself, and the role model says in every situation what would be
  right (14, A).
- **Dead zone:** a range in which a small input doesn't do anything yet, like with a gamepad stick
  (8, 13, 15).
- **Distribution shift:** if you only watch, you don't know the situations you'll get into yourself –
  like learning to ride a bike just by watching (A).
- **Emergency brake:** cuts a learning step that would be too long down to a fixed maximum length
  (14, A).
- **Entropy:** how unpredictably the duck still acts. A small bonus for it keeps it curious, so it
  doesn't stop trying things out too early (A).
- **Episode:** one run from start to finish, for example a jump (14).
- **Epoch:** learning once through all the collected data (A).
- **GAE:** a way of calculating that estimates how good an action was – also through what came after
  it (A).
- **Gradient:** the slope for all sliders together. It shows in which direction the error grows
  fastest; you go the opposite way (A).
- **Gradient descent:** going downhill in small steps to the smallest error, like in fog (A).
- **Hyperparameter:** settings of learning itself, such as the learning rate or the number of ducks.
  They aren't learned but fixed beforehand (15).
- **Iteration:** one round of training: collect experience, then learn from it (14).
- **KL divergence:** measures how differently the old and the new duck act. Kern adjusts the learning
  rate by it (A).
- **Layer:** lots of neurons side by side that all get the same inputs (A).
- **Learning rate:** how big the steps of learning are (A).
- **Lehrer:** Kern's training program, in the browser and as a program for the command line (14, 15).
  Only the critic is called the "coach" in the picture.
- **Loss:** a number for how wrong the network is right now (A).
- **Minibatch:** a small selection of the data from which one learning step is made – like a poll of
  30 instead of all 800 students (A).
- **MLP (multilayer perceptron):** a network of several layers one after the other (A).
- **Neural network / neuron:** a calculating network made of many simple calculation units (A).
- **Normalizing:** bringing all inputs to a similar size so big numbers don't drown everything out
  (A).
- **ONNX:** file format for trained networks, something like PDF for networks (13, A).
- **Overfitting:** memorizing instead of understanding (A).
- **Phase signal (clock hand):** a clock with one hand that tells the jump network where in the jump
  it is right now (14, 15).
- **Policy:** the duck's decision rule: observation in, action out. While learning it's a bit random,
  on the robot it isn't (14, A).
- **PPO:** the Lehrer's learning method. It improves the duck only a little per step, as if with a
  seat belt (14, A).
- **Recipe (training plan):** the training settings of a task (14, 15).
- **Reinforcement learning:** learning through trial and reward (A).
- **Return:** all the points of an attempt added up (15). While learning, later points count a bit
  less, with the future factor (A).
- **Reward:** points that tell the network what was good (14, A).
- **Role model:** a demonstration that the network imitates first (14).
- **Save game:** see Checkpoint.
- **Search cloud:** the many candidates that CMA-ES tries out around a center (14, A).
- **Sim-to-real:** from the simulation to the real robot (16, A).
- **Snapshot (in training):** see Checkpoint.
- **Spread:** how far values typically lie from the average (16, A).
- **Surprise (TD error):** how much better or worse a single step went than the critic expected (A).
- **Value:** how many rewards you still expect on average from a situation (A).
- **Variation (domain randomization):** every practice round gets a slightly different duck in a
  slightly different world. Then the real duck is just one more variant (14, 16, A).
- **velstand:** Pollen's network for walking and standing, built into Kern; the starting point for
  Kern's jump (13, 14).
- **Warm start:** training doesn't start from zero but from a finished network that can already
  balance (14, A).
- **Weight:** a slider in the network: how important an input is, negative too (A).
- **Wobble strength:** = action noise σ (A).

> **Mission:** Open **File › Example Levels › Physics Lab**, switch on
> **Window › Show › Contact Points** and start **Simulate**. Count how many glossary words you see
> while doing it: body, joint, contact …

## Level complete!

- You find every important key in Studio in seconds.
- You know why the number of ducks changes the network, but the number of threads only the speed.
- You know what's in a `.klevel` file and how to look at it.
- You know the files for networks, servo profiles and measurements.
- You have a glossary with all the technical terms in the book.

## Puzzle corner

1. Which key do you use to take over the duck while playing?
2. What's the difference between **Play** and **Simulate**?
3. Which glossary word fits: "The coach is allowed to see the duck's true height, the duck itself
   isn't"?
4. **Boss puzzle:** Lena trains with 16 ducks on 2 threads, Tom with 16 ducks on 8 threads, Mia with
   32 ducks on 2 threads. They all use the same seed. Whose networks are the same, and who finishes
   fastest?

### Solutions

1. **F8**. Pressing F8 again leaves it.
2. In **Play**, you play yourself. In **Simulate**, the world runs without a player character: you
   watch with the editor camera and grab bodies; you can only edit the level after stopping.
3. **Asymmetric critic.** The critic only exists while learning, which is why it's allowed to see
   more.
4. Lena and Tom get exactly the same network, because both have 16 ducks. Tom is faster, because more
   threads calculate at the same time (if his computer has that many cores). Mia's network is
   different, because she has more ducks.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
