# 17 Cheat Sheet: Keys, Commands, Terms

> **Your level:** The last level is your reference book. Here you'll quickly find the most
> important keys, the Lehrer's commands, the file formats, the interface to the browser and a
> glossary with every technical term in the book. The complete reference, with every function and
> every parameter, is in the blue book.

## 17.1 Keys in Studio

The key layout follows Unreal Engine. **F1** also shows the most important shortcuts in Studio.

### Camera in the Viewport

| Input | Effect |
|---|---|
| right mouse button + W A S D | fly |
| … + Q / E | down / up |
| … + Shift | 2.5 times faster |
| … + mouse wheel | flying speed 1–8 |
| Alt + left mouse button | orbit around the selection |
| middle mouse button | pan |
| F | frame the selection |
| Ctrl+1 … 9 / 1 … 9 | set / jump to a bookmark |

On a phone: one finger orbits, two fingers zoom and pan, tapping selects.

### Editing

| Key | Effect |
|---|---|
| Q / W / E / R | Select / Move / Rotate / Scale |
| [ / ] | grid smaller / bigger |
| Delete | delete the selection |
| End | drop the selection to the floor |
| H / Ctrl+H | hide / show everything |
| G | Game View |
| F2 | rename |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+C / V / D | copy / paste / duplicate |
| Ctrl+G | group |
| Ctrl+S / Ctrl+O / Ctrl+N | save / open / new level |
| Ctrl+Space | Content Drawer |
| Alt+2 / 3 / 4 / 5 | Wireframe / Unlit / Lit / Player Collision |
| Alt+P / Alt+S | Play / Simulate |

### Play and Simulate

| Key | Effect |
|---|---|
| Esc | Stop |
| F8 | leave the player (Eject) / take over (Possess) |
| Pause | pause / continue |
| . | while paused: one step forward |
| Ctrl+R | restart the game |
| K | in Simulate: keep the poses in the level |
| Shift+F1 | release the mouse |

In the game: W A S D to move, mouse to look around, Space to jump, Shift to boost, F to use.

### Logic editor

F7 compiles, F9 sets a breakpoint, Ctrl+F searches, C puts a comment around the selection, Esc
takes you back to the Viewport.

> **Mission:** Open Kern Studio at https://kernengine.de/ and press **F1**. Compare the Controls
> dialog with these tables. Then try Ctrl+1 on a nice camera position, fly away and press **1** –
> you jump right back.

## 17.2 Micro Duck keys

W/S to walk, A/D to turn, **1** sit, **2** pick up, **3** and **4** kick, **5** roll, **6** beak,
**7** jump, **Space** quack, **V** "Wheee." Gamepad and touch are in
[Chapter 13](13-micro-duck.md).

## 17.3 The Lehrer's commands

`kern_lehrer` is the Lehrer as a program for your computer (built with `kern/tools/lehrer.sh`,
[Chapter 2](02-building-and-testing.md), [Chapter 15](15-your-own-models.md)). It reads all the
options, loads Pollen's networks and then does **the first thing** that applies:

1. `--fingerprint N`: determinism fingerprint over N ticks.
2. `--eval FILE`: evaluate a jump.
3. `--trace FILE`: show one episode tick by tick.
4. `--kalibrieren`, `--abgleich`, `--beispiele`: calibration ([Chapter 16](16-calibration.md)).
5. `--vorbild-suchen`, `--vorbild-pruefen`: search for and check a jump demonstration.
6. `--spielen-pruefen`, `--spielen-ueben`, `--spielen-spur`: check, practice, trace the play
   network.
7. `--biss-fenster`, `--halt-messen`: measure the bite and the coasting.
8. `--demo-spur`: play the Micro Duck example level.
9. `--spielen`: learn the play network by imitation.
10. otherwise: **training** of the task `--task` (`jump` or `stand`).

Before number 2, it checks whether `--task` is `jump` or `stand`. `--spielen`, `--spielen-spur`,
`--biss-fenster`, `--halt-messen` and `--demo-spur` share one switch: the last one on the command
line wins. An unknown option shows the short help.

### The most important options

| Option | Default | Meaning |
|---|---|---|
| `--task` | `jump` | task |
| `--threads N` | hardware threads | compute threads |
| `--envs N` | 16 × threads | ducks at the same time |
| `--seed N` | 1 | starting value for randomness (with `--demo-spur`: duration in s) |
| `--iterations N` | 600 | training rounds |
| `--episodes N` | 10 | episodes when evaluating |
| `--out FILE` | `<task>.onnx` | output file |
| `--profil FILE` | XL330 | servo profile |

**Training (PPO), recipe `jump` / `stand`:** `--steps` (48 / 24), `--init-std` (0.3 / 1.0),
`--lr` learning rate (1e-4 / 1e-3), `--entropy` (0.005 / 0.01), `--actor` (512,256,128),
`--critic` (256,128), `--epochs` (5), `--minibatches` (4), `--adaptive-lr` and `--kl` (adjust the
learning rate, target 0.01), `--substeps` (16), `--warm-start` (`velstand` / `none`).

**Imitating:** `--vorbild builtin|demo.json|none`, `--nachahmen` (8 rounds), `--episoden` (96),
`--uebungs-std` (0.1), `--nachahmungs-lohn` (10).

**Saving:** `--log` (CSV per round), `--checkpoint` and `--resume` (save progress and continue),
`--save-every` (25), `--keep-every` (snapshots), `--eval-every` (10; reports `FIRST REAL JUMP`),
`--stop-after-jump`.

**Other:** `--randomized` (with variation), `--netz play.onnx` (play network for the trace),
`--policies DIR` (where Pollen's networks are).

```sh
kern/tools/lehrer.sh
cd kern/build
./kern_lehrer --task jump --iterations 3000
./kern_lehrer --eval jump.onnx --episodes 40
./kern_lehrer --trace jump.onnx
./kern_lehrer --fingerprint 400
```

Line 1 builds the program, line 2 switches into the folder. Line 3 trains the jump for 3,000
rounds. Line 4 evaluates it over 40 episodes, line 5 shows one episode tick by tick. Line 6 prints
the fingerprint, which you compare with `kern/tests/determinism_fingerprint.txt`.

> **For pros:** The result of a training run depends on the number of ducks (`--envs`), not on the
> number of threads. More threads make it faster, but not different.

## 17.4 File formats

### Levels (`.klevel`)

A level is a JSON text file with `"format": "kern-level"` and `"version": 1`. Inside are `name`,
`world` (World Settings), `camera`, `nextId`, `actors`, embedded `textures`, `sounds` and, if there
is any, `logic`. Units: meters, seconds, kilograms, Z points up, angles in degrees, colors from 0
to 1. Kern rejects a different format ("not a Kern level"), and a newer version too.

> **Mission:** Save a level with **Ctrl+S** and open the `.klevel` file in a text editor. Can you
> find `"format": "kern-level"` and your actors?

### Networks (`.onnx`)

Kern reads and writes networks in the shape Pollen uses for the Micro Duck:

```
obs → Sub → Div → (Gemm, Elu)* → Gemm → actions
```

First the inputs are normalized (minus the mean, divided by the spread), then come layers
(**Gemm** means: weights times inputs plus offset) with **ELU** in between. Motion networks have
61 inputs and 14 outputs; `play.onnx` has 23 and 9. Kern rejects anything else with a reason, for
example "expected Elu after a hidden Gemm." When exporting, the Lehrer writes **metadata** along
with it: joint names, home pose, task and "lehrer." Pollen's pick-up detector has a different shape
and is built in. Pollen's networks are in `kern/third_party/microduck/`, Kern's in
`kern/policies/`; networks trained in the browser are stored in the browser by the Lehrer page
(IndexedDB); on the same device they replace the bundled ones.

### Servo profile (`.json`)

```json
{
  "kind": "servo", "name": "xl330",
  "stiffness": 0.554, "damping": 0.053,
  "friction": 0.0048, "torque": 0.963,
  "armature": 0.0018, "delay": 0.02,
  "spreadArmature": 0.1, "spreadFriction": 0.1,
  "delayMin": 0.015, "delayMax": 0.03
}
```

`stiffness` and `torque` must be greater than 0; spreads and delays are between 0 and 1. Missing
fields are 0. Possible messages: `not a servo profile`, `bad value for <field>`.

### Logs (CSV)

A **test-bench log** has the load at the top (`# mass`, `# arm`, `# inertia`, `# horizontal`), then
`time,target,angle`. A **robot log** has 61 observations and 14 actions per control tick. Details
in [Chapter 16](16-calibration.md).

## 17.5 The C ABI: how JavaScript talks to Kern

The engine is a **WebAssembly module**. JavaScript in the browser calls its functions through a
plain list, the **C ABI**. The rules:

- Every function has the same name in the module as in the C++ code, for example `kern_render`.
- The browser calls `_initialize` **once**, then the functions (a "WASI reactor").
- Data goes through shared memory: the browser reserves space with `kern_alloc`, writes into it and
  passes the address (a **pointer**, 32 bits).
- A return value of 1 means success, 0 means error; `kern_last_error()` gives you the message.
- Text goes as a pointer plus a length, in UTF-8.
- Answers (JSON, bytes) belong to the module and stay valid until the next call in the same group.

The other way round, the module needs from the browser: the **WebGL2** functions (`kern_gl`), the
time, logging, height queries, status text, pointer lock and compiling logic (`kern_env`), plus a
few basic functions (`wasi_snapshot_preview1`). The engine never touches files.

| Group | What for |
|---|---|
| `kern_alloc`, `kern_last_error` … | memory and errors |
| `kern_renderer_*`, `kern_mesh_*` … | drawing: meshes, textures, camera, light, sky, shadows, picking |
| `kern_fx_*` | particles |
| `kern_geo_*` | buildings, roads, terrain |
| `kern_app_*` | your own C++ games (`--app`) |
| `kern_studio_*` | Kern Studio (with `--studio`) |
| `kern_lehrer_*` | the Lehrer in the browser (with `--lehrer`) |
| `kern_logic_*` | called by compiled logic |

Meshes are described with **descriptors**: fixed lists of numbers in which every position has a
meaning (positions, colors, flags such as "visible" or "casts shadows"). A game can bring its own
shader extensions along. Every signature is in the blue book.

## 17.6 Glossary

Every technical term in the book, explained briefly. The number in parentheses names the chapter
where it's covered in detail.

### Studio and games

- **Actor:** a thing in the level with a position and building blocks (8).
- **Autosave:** automatic backup of unsaved work (11).
- **Blueprint / logic graph:** a program made of nodes and connections, without typing (9).
- **Component:** a building block of an actor, such as a mesh, collider or sound (8).
- **Content Drawer:** the drawer full of things to place (11).
- **Controller:** decides how the player moves: walking, rolling, driving, flying (8).
- **Details:** the panel with all the settings of the selected actor (11).
- **Eject / Possess:** in the game, leave the player / take it over again (11).
- **Game View:** a view without editor helpers, as in the game (11).
- **Gizmo:** arrows and rings for moving, rotating, scaling (11).
- **HUD:** on-screen displays in the game, such as points and time (8).
- **Level:** a game world, saved as `.klevel` (8).
- **Node:** a building block in the logic graph (9).
- **Observer camera:** Play with a free camera for watching (11).
- **Outliner:** the tree view of all actors (11).
- **Package Game:** export a level as a self-contained HTML file (11).
- **Play / Simulate:** play / let the physics run while the editor stays usable (11).
- **Player-only:** starting without the editor, for exported games (11).
- **Snapping:** clicking into place on a grid (11).
- **Trigger:** an area that sets something off when you enter it (8).
- **Viewport:** the window through which you look into the 3D world (11).
- **World Settings:** settings for the whole world, such as the step rate (11).

### Graphics and geometry

- **BVH:** a tree of boxes that lets you quickly find what a ray hits (5).
- **Bloom:** bright spots glowing (4).
- **Context loss:** the browser briefly takes the graphics card away; Kern rebuilds everything (4).
- **Descriptor:** a fixed list of numbers that describes a mesh, for example (17).
- **Earcut:** a method that cuts polygons into triangles (5).
- **Frame:** a single image; many per second make motion (4).
- **Frustum culling:** not drawing what's outside the field of view (5).
- **GTAO:** darkens corners and cracks realistically (4).
- **Instancing:** drawing the same mesh many times at once (4).
- **Material:** what a surface looks like: color, roughness, metal (4).
- **Mesh:** a 3D shape made of triangles (4).
- **NightWindows:** shader extension for lit windows at night (4).
- **Particles:** lots of small dots for sparks, smoke, dust (4).
- **Pass:** one round of drawing, for example for shadows (4).
- **Picking:** finding out what's under the mouse pointer (5).
- **Renderer:** the part that draws the images (4).
- **Shader:** a small program on the graphics card that computes points and colors (4).
- **ShaderExtension:** an add-on to the standard shader for your own looks (4).
- **Skinning:** deforming a mesh with bones, for example for characters (4).
- **Texture:** an image on a surface (4).
- **Vertex:** a corner point of a triangle (4).
- **Viewmodel layer:** drawn after the depth is cleared, so it's always in front, for example
  for gizmos (4).
- **WebGL2:** the browser's interface to the graphics card (4).
- **Welding:** merging duplicate corner points without losing anything (5).

### Math and tech

- **C++ / JavaScript:** the engine's programming language / the language of web pages (1).
- **C ABI:** the list of functions through which JavaScript talks to the engine (17).
- **CI:** automatic checking of every change on a server (2).
- **DuckEnv:** the Lehrer's environment: one Micro Duck in its own game world (14).
- **Determinism:** the same inputs give bit for bit the same result (6, 14).
- **`dm`:** Kern's math functions that compute exactly the same everywhere (3).
- **Error channel:** Kern reports errors with `fail()` and `last_error()` (3).
- **Fingerprint:** a check number per tick that gives away any deviation (2).
- **GameWorld:** plays a level on Box3D and displays it (8).
- **JIT:** a logic graph is compiled into WebAssembly while the game is running (9).
- **Interpreter:** runs a logic graph node by node (9).
- **JSON:** a readable text format for data (3).
- **Matrix:** a table of numbers that describes rotating, moving and scaling all at once (3).
- **Pointer:** an address in memory (17).
- **Quaternion:** four numbers that describe a rotation (3).
- **Radian:** a unit of angle; a circle has 2π ≈ 6.28 rad (13).
- **Reactor (WASI):** a WebAssembly module without a main program, only functions (17).
- **Span:** a view of a row of elements, without copying them (3).
- **Tier:** how a graph runs: interpreter, WebAssembly or C++ (9).
- **Vector:** an arrow with a direction and a length, such as (x, y, z) (3).
- **wasi-sdk:** the tool that compiles C++ into WebAssembly (2).
- **WebAssembly:** a program format that browsers run very fast (1).

### Physics

- **Body / Shape:** what moves / what form it has (6).
- **Box3D:** Erin Catto's physics engine, which Kern includes unchanged (6).
- **CharacterMover:** moves a game character as a capsule that pushes other bodies (6).
- **Collider:** the invisible shape that something collides with (6).
- **Contact:** a spot where two bodies touch (6).
- **Convex hull:** a shape with no dents going inward (13).
- **Friction / restitution / rolling resistance:** sticking / bouncing / braking while rolling (6).
- **Inertia:** how hard it is to set something moving or spinning (6).
- **Joint:** connects two bodies, for example as a hinge (6).
- **MouseGrab:** grabbing a body with the mouse or a finger (6).
- **Ray cast:** a ray test: what does a ray hit first? (6)
- **Substeps:** sub-steps per physics step; 16 for the duck (6).
- **Time step:** the fixed amount of time the physics moves forward by (6, 8).
- **Torque:** a turning force, measured in N·m (13).

### Sound and networking

- **Mixer:** mixes all sounds into one (7).
- **Reverb:** the echo that a room adds to a sound (7).
- **Synthesizer:** creates sounds from calculation instead of recordings (7).
- **3D sound:** sound that comes from a spot in space (7).
- **Client / Server:** a fellow player / the computer that runs the game (10).
- **Datagram:** a single data packet on the network (10).
- **HostTransport:** a network whose packets are carried by the browser page, for example via
  BroadcastChannel or a relay (10).
- **Interpolation:** blending smoothly between two known states (10).
- **LoopbackNetwork:** a network in memory, for tests and multiplayer in the editor (10).
- **NetGame:** Kern's multiplayer: one server is in charge, everyone gets snapshots (10).
- **Relay:** a go-between that passes packets on between browsers (10).
- **Snapshot:** a snapshot of the game state (10).

### Robots

- **Armature:** the spinning mass of the motor inside the servo (13).
- **Control tick / decimation:** how often the network is asked / every how-manyth physics step
  (13).
- **Device model:** a small model of a real device, with a profile (13, 16).
- **Encoder:** measures a servo's angle (13).
- **Gear backlash:** a little wiggle in the gearbox (16).
- **Home pose:** the duck's basic standing position (13).
- **IMU:** orientation sensor: where is down, how fast is the body turning? (13)
- **Kern extras:** the switch for Kern's add-ons on the duck (13).
- **Low-pass / EMA:** smoothing values so nothing jerks (13).
- **Micro Duck:** two-legged walking robot from Pollen Robotics (13).
- **MJCF / MuJoCo:** blueprint format / the physics simulation Pollen trains with (13).
- **Pick-up detection:** notices that the duck is being lifted (13).
- **Play layer:** Kern's network for ball and sock (13).
- **Servo:** a motor that moves to an angle (13).
- **Servo profile:** the numbers of a servo type, as JSON (16).
- **ToF:** a distance sensor that measures with the travel time of light (13).
- **Walker:** controller type for robots driven by a network (13).

### Learning

- **Actor / Critic:** the part that acts / the part that judges (14, A).
- **Adam:** a method that adjusts weights cleverly (A).
- **Backpropagation:** working backward to figure out how much each weight is to blame for the
  error (A).
- **CMA-ES:** a search method that tries candidates and feels its way toward the best ones
  (14, 16).
- **Collector / Learner:** gathers experience / learns from it (14).
- **DAgger:** imitating: the network drives, the demonstration says what would be right (14).
- **Domain randomization:** training with random deviations (16).
- **ELU:** the activation between the layers: positive values stay, negative ones are gently
  squeezed to just above −1 (A).
- **Episode:** one run from start to finish (14).
- **GAE:** a calculation that estimates how much better an action was than expected (A).
- **Gradient descent:** going downhill in small steps toward the smallest error (A).
- **Learning rate:** how big the steps of learning are (14).
- **Lehrer:** Kern's training system; German for "teacher" (14).
- **Neural network / neuron:** a computing network made of many simple computing units (A).
- **Normalizing:** bringing inputs to a similar size (A).
- **ONNX:** exchange format for trained networks (13).
- **Overfitting:** memorizing instead of understanding (A).
- **Policy:** a trained network: observations in, actions out (13).
- **PPO:** the Lehrer's learning method, learning through reward (14).
- **Recipe:** the training settings for a task (14).
- **Reinforcement learning:** learning by trying things out and getting rewards (A).
- **Reward:** points that tell the network what was good (14).
- **Rollout:** gathered experience: observed, done, rewarded (14).
- **rsl_rl:** the learning library Pollen trains with (13).
- **Sim-to-real:** from the simulation to the real robot (16).
- **velstand:** Pollen's walking and standing network, built into Kern (13).
- **Vorbild:** a demonstration that the network imitates first; German for "role model" (14).

> **Mission:** Open **File › Example Levels › Physics Lab** and start **Simulate**. Count how many
> glossary words you spot along the way: body, joint, contact …

## Level complete!

- You can find every important key in Studio in seconds.
- You know the Lehrer's commands and which one wins first.
- You know how a `.klevel`, a network and a servo profile are built.
- You understand how JavaScript talks to the engine through the C ABI.
- You have a glossary with every technical term in the book.

## Puzzle corner

1. Which key do you use to take over the duck during Play?
2. Which line has to be in every `.klevel` file?
3. You run `kern_lehrer --eval jump.onnx --kalibrieren a.csv`. What happens?
4. **Boss puzzle:** A Kern function returns 0. How do you find out what went wrong?

### Solutions

1. **F8** (Possess). Pressing F8 again leaves it.
2. `"format": "kern-level"` (plus `"version": 1`).
3. It evaluates: `--eval` comes before `--kalibrieren` in the order.
4. 0 means error. `kern_last_error()` gives you the message.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
