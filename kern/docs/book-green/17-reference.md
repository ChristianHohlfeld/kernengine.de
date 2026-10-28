# 17 Quick Reference

> **In short:** You don't read this chapter from start to finish; you look things up in it. Here
> you'll find the most important keys in the Studio and for the Micro Duck, the commands of the
> Lehrer tool, the file formats, a look at the interface between the engine and the browser, and a
> glossary of all the technical terms in the book. The complete reference with every function and
> every option is in Chapter 17 of the blue book.

> **Imagine …** this chapter is the sheet of paper taped to the inside of the workshop door: where
> each wrench lives, which switch does what, and what "torque" actually means. Every machine's
> manual is on the shelf (in the blue book). For everyday work, the sheet on the door is usually
> enough.

## 17.1 Keys in the Studio

Kern Studio's controls follow Unreal Engine. **F1** also shows the most important shortcuts in the
Studio itself. All the details are in [Chapter 11](11-studio.md).

### Camera in the viewport

| Input | Effect |
|---|---|
| Right mouse button + W A S D | fly (Fly Mode) |
| … + Q / E | down / up |
| … + Shift | 2.5 times faster |
| … + mouse wheel | flying speed (level 1–8) |
| Alt + left mouse button | orbit around the selection |
| Drag with the middle mouse button | pan |
| F | bring the selection into view |
| Ctrl+1 … 9 / 1 … 9 | set / jump to bookmark |

On a phone, one finger orbits, two fingers zoom and pan, a double tap brings the selection into
view, and a long press opens the context menu.

### Editing

| Key | Effect |
|---|---|
| Q / W / E / R | Select / Move / Rotate / Scale tool |
| Space | cycle Move → Rotate → Scale |
| [ / ] | finer / coarser grid |
| Delete | delete selection |
| End | drop selection to the floor |
| H / Ctrl+H | hide / show everything |
| G | Game View (editor helpers off) |
| F2 | rename |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+C / X / V / D | copy / cut / paste / duplicate |
| Ctrl+G | group |
| Ctrl+S / Ctrl+O / Ctrl+N | save / open / new level |
| Ctrl+Space | Content Drawer |
| Alt+P / Alt+S | Play / Simulate |

> **Did you know?** Copied actors land as text on your system's clipboard. A Ctrl+V in another
> browser tab pastes them there – that's how you move things from one level to another.

### Play and Simulate

| Key | Effect |
|---|---|
| Esc | Stop |
| F8 | Eject / Possess (leave / take over the player) |
| Pause or Alt+Shift+P | pause / resume |
| . | one step forward while paused |
| Ctrl+R | restart the game |
| K | keep simulated poses in the level (Simulate) |

In the game itself, the usual applies: W A S D to move, the mouse to look around, Space to jump,
Shift to sprint, F to use. In the logic editor, **F7** compiles the graph, **F9** sets a
breakpoint, and **Ctrl+F** searches.

> **Try it!** At https://kernengine.de/ open the **Physics Lab** level, press Alt+S for Simulate,
> then **Pause** (or Alt+Shift+P), and tap the period key a few times. You'll see the physics step by
> step – like a flip book you leaf through yourself.

## 17.2 Micro Duck keys

| Motion | Keyboard | Gamepad | Touch |
|---|---|---|---|
| Walk / turn | W S / A D | left stick | MOVE |
| Sit down / stand up | 1 | B | SIT |
| Pick up (on rollers: crouch) | 2 | Y | PICK |
| Kick left / right | 3 / 4 | – | KICK L / R |
| Roll (hold: keep going) | 5 | X | ROLL |
| Beak open / closed | 6 | – | BEAK |
| Jump | 7 | – | HOP |
| Quack / "Wheee" | Space / V | RT / LT | QUACK / WHEEE |

Each of these keys hands you the controls for 2 seconds; the duck's brain and play layer take a
break for that long ([Chapter 13](13-micro-duck.md)).

> **Try it!** Load the **Micro Duck** template, press Play, take over the duck with **F8**, and try
> all the keys from 1 to 7 in order. What happens if you press 3 while the duck is sitting?

## 17.3 Lehrer commands (`kern_lehrer`)

The Lehrer runs in the browser, but it also exists as a command-line program. You build it with
`kern/tools/lehrer.sh`; afterwards it's at `kern/build/kern_lehrer`
([Chapter 2](02-building-and-testing.md), [Chapter 14](14-lehrer.md)).

The program has many **modes of operation**. Which one applies is decided by a fixed order of
priority: the first one that applies wins, `--fingerprint` comes first of all, and without any
special option it trains. A task other than `jump` or `stand`, and any unknown option, end the
program with return code 2 (an unknown option with a short help text). The modes:

| Option | What happens |
|---|---|
| `--task jump` or `stand` | train (default: `jump`) |
| `--eval FILE` | evaluate a jump |
| `--trace FILE` | show an episode tick by tick |
| `--fingerprint N` | determinism fingerprint over N ticks |
| `--kalibrieren LIST` | fit a servo to test-bench logs |
| `--abgleich FILE` | robot log against the simulation |
| `--beispiele DIR` | write example logs and a profile |
| `--vorbild-suchen FILE` | search for a jump demonstration with CMA-ES |
| `--vorbild-pruefen X` | evaluate a demonstration |
| `--spielen` | learn a play network through imitation |
| `--spielen-ueben FILE` | keep practicing a play network with PPO |
| `--spielen-pruefen X` | evaluate a play network or demonstration |
| `--spielen-spur` | one play episode, decision by decision |
| `--biss-fenster` | measure where the beak grips the sock |
| `--halt-messen` | measure how far the duck keeps walking after "stop" |
| `--demo-spur` | play the Micro Duck level and record it |

The five play measurements (`--spielen`, `--spielen-spur`, `--biss-fenster`, `--halt-messen`,
`--demo-spur`) share one switch: the last one on the command line wins.

On top of that come the adjustment knobs: `--threads` and `--envs` (how many CPU cores and how many
ducks at once), `--iterations` (how long; training 600, calibration 80 generations), `--episodes`
(how many evaluation rounds, default 10), `--seed` (which randomness), `--out` (where to),
`--policies` (where Pollen's networks are), `--profil` (which servo profile), `--randomized`
(`--trace` or a trace with variation) and `--netz` (which play network drives a trace). For
advanced users, the recipe's values can be overridden: steps, initial noise, learning rate,
entropy, size of actor and critic, epochs, minibatches, adaptive learning rate with target KL,
substeps and the warm start from the velstand network; the same goes for imitation (demonstration,
rounds, episodes, practice noise, weight of the imitation reward).

Important for long training runs:

- `--log` writes one CSV line per iteration (reward, losses, jumps …).
- `--checkpoint` saves the whole learner (by default every 25 iterations, `--save-every`), and
  `--resume` picks up from there. `--keep-every` keeps intermediate states as separate `.onnx`
  files.
- For the jump, the program evaluates five episodes every 10 iterations (`--eval-every`). If the
  duck really jumps in 4 out of 5, it reports `FIRST REAL JUMP` and writes the network
  immediately; `--stop-after-jump` then stops.
- With imitation, `<name>_nachgeahmt.onnx` is created as well: the network before practice.
- `--demo-spur` takes `--seed` as the duration in seconds (default 120); the environment variable
  `KERN_JUMP_AT` triggers the jump in a particular second.

All options with their default values are in the blue book.

> **Watch out!** The result of a training run depends on the number of ducks (`--envs`), not on
> the number of CPU cores. If you train on a different computer with different `--threads` but the
> same number of ducks, you get the same network. But careful: `--envs` is rounded down to a
> multiple of the threads, and if you don't specify it, there are 16 ducks per thread – then the
> number of ducks changes along with the threads.

A few everyday examples:

```sh
kern/tools/lehrer.sh
kern/build/kern_lehrer --task jump --iterations 3000 \
    --out jump.onnx --log jump.csv --checkpoint run.ckpt
kern/build/kern_lehrer --eval jump.onnx --episodes 40
kern/build/kern_lehrer --kalibrieren steps.csv,chirp.csv \
    --pruefen square.csv --out xl330_unit3.json
kern/build/kern_lehrer --fingerprint 400 \
    | diff - kern/tests/determinism_fingerprint.txt
```

Line by line: first the program is built. Then it trains a jump over 3000 iterations, writes the
network to `jump.onnx`, a log to `jump.csv`, and regularly a checkpoint you can resume from later.
The third line evaluates the finished network over 40 episodes. The fourth calibrates a servo
([Chapter 16](16-calibration.md)). The last one checks whether the simulation on this computer
computes exactly the same thing, bit for bit, as the reference: if `diff` prints nothing,
everything is fine.

## 17.4 File formats

| File | What's in it | more in |
|---|---|---|
| `.klevel` | a level as JSON | [Chapter 8](08-game-runtime.md), [11](11-studio.md) |
| `.onnx` | a trained network | [Chapter 13](13-micro-duck.md), [14](14-lehrer.md) |
| Servo profile (`.json`) | a servo's numbers | [Chapter 16](16-calibration.md) |
| Test-bench log (`.csv`) | time, target, angle | [Chapter 16](16-calibration.md) |
| Robot log (`.csv`) | 61 observations + 14 actions per tick | [Chapter 16](16-calibration.md) |

### Levels (`.klevel`)

A level is a JSON file marked with `"format": "kern-level"` and `"version": 1`. It contains the
name, the World Settings, the editor camera, all actors with their components, embedded textures
and sounds and, if there is one, the level's logic graph.

The units are meters, seconds and kilograms, Z points up, rotations are in degrees, and colors are
numbers from 0 to 1. Kern rejects a file with a foreign format ("not a Kern level"), as well as one
from a newer Studio ("level was written by a newer Kern Studio").

### Networks (`.onnx`)

Kern reads and writes networks in exactly the shape in which Pollen Robotics exports them for the
Micro Duck. In words: measurements in, normalize, a few layers with ELU in between, a linear layer
at the end, commands out. Motion networks have 61 inputs and 14 outputs, the play network
`play.onnx` 23 inputs and 9 outputs. If a file doesn't fit, Kern says why, for example "not an
rsl_rl actor" or "unsupported ONNX operator".

On export, Kern writes extra information into the network: joint names, home pose, task and a note
that Kern's Lehrer trained it; for the play network also its rate (25 Hz) and the names of its 23
inputs and 9 outputs. The runtime parameters of each motion (kind, duration, scaling) aren't in the
network but in the built-in manifest. Pollen's networks are in `kern/third_party/microduck/`, Kern's
own (`jump.onnx`, `play.onnx`) in `kern/policies/`; on the published site in `editor/microduck/` and
`editor/policies/`. A network trained in the browser stays stored on your device and replaces the
bundled one there. The duck's pick-up detector is a special case with a different shape; it is
built in.

### Servo profile and logs

A **servo profile** is a small JSON file with the numbers of a servo type: stiffness, damping,
friction, maximum torque, armature, delay and spreads. At the very least, stiffness and maximum
torque must be in it and be positive; if anything else is missing, it counts as 0. Kern itself
always writes all fields in a fixed order. If something doesn't fit, it reports something like "bad
value for …" or "stiffness and torque must be positive".

A **test-bench log** starts with comment lines about the load (`# mass`, `# arm`, `# inertia`,
`# horizontal`) and then has lines with the time, target angle and measured angle. A **robot log**
has a header line and then 75 numbers per control tick. `kern_lehrer --beispiele` generates
examples.

## 17.5 The interface to the browser (C ABI)

Kern is written in C++ and runs in the browser as **WebAssembly**, a format that browsers execute
almost as fast as real programs. The page around it is JavaScript. The two talk to each other
through a flat list of functions, the **C ABI**.

> **Imagine …** the engine sits in a room without doors, with only a serving hatch like in a
> cafeteria. JavaScript puts something in ("draw this picture", "here's a key press"), and the
> engine puts something back. What fits through the hatch is precisely defined – that's the C ABI.

The basic rules, in brief:

- Every function has the same name in the module as in the C++ code, for example `kern_render`.
  Natively, for instance in tests and in the Lehrer program, they are perfectly ordinary C
  functions.
- The module is a **reactor**: it has no `main`. The page calls `_initialize` once and then the
  functions.
- Data goes through shared memory: the page reserves space (`kern_alloc`), writes into it and
  passes the address (32 bits).
- **1 means success, 0 failure**; you get the message with `kern_last_error()`.
- Text goes in as UTF-8. Answers such as JSON belong to the module and remain valid until the next
  call in the same group.
- In the other direction, the module needs a few things from the browser: WebGL2 for drawing, the
  time, log output, the terrain height for the geometry, status text and pointer lock for C++
  games, and compiling logic graphs. On top of that comes a tiny stand-in for WASI, WebAssembly's
  system interface; the engine never touches files. If the engine needs a new WebGL function, it
  has to be added in two places: in `gl.h` and in `kern.mjs`.
- A few symbols are supplied by the game itself: a C++ game defines `kern_create_app()`, and it can
  register its own shader extensions (`kern_game_extensions`, otherwise none). Natively, tests
  replace the terrain height with a function of their own.
- Fixed blocks of data, called descriptors (for meshes, startup, rendering, lighting, sky,
  picking …), have precisely defined slots that must match in C++ and in `kern.mjs`.

The functions are sorted into groups:

| Group | What for | more in |
|---|---|---|
| `kern_alloc`, `kern_last_error` … | memory and errors | – |
| `kern_renderer_*`, `kern_mesh_*` … | drawing, camera, lighting, picking | [Chapter 4](04-graphics.md) |
| `kern_fx_*` | particles | [Chapter 4](04-graphics.md) |
| `kern_geo_*` | buildings, roads, terrain | [Chapter 5](05-scene-and-geometry.md) |
| `kern_app_*` | your own C++ games | [Chapter 12](12-your-own-games.md) |
| `kern_studio_*` | the Studio and exported games | [Chapter 11](11-studio.md) |
| `kern_lehrer_*` | training in the browser | [Chapter 14](14-lehrer.md) |
| `kern_logic_*` | compiled logic graphs | [Chapter 9](09-logic.md) |

A few things worth knowing about the groups:

- **Renderer:** There is one renderer per module. Mesh ids belong to whoever creates them; invalid
  input leaves an existing mesh unchanged. After a WebGL context loss, the engine rebuilds
  everything.
- **Particles:** one system per module, which draws into a mesh reserved beforehand.
- **Geometry:** Buildings and roads are created as jobs in bite-sized chunks with a time budget, so
  that no frame stutters.
- **App:** `kern/web/app.mjs` drives a C++ game frame by frame; the time step is limited to 0.1 s,
  and when the window loses focus, all keys are released.
- **Studio:** The page sends commands as JSON (`{"cmd": …}`), reads the state of the panels and the
  per-frame values, passes on pointers, fingers and keys, supplies textures, networks and sounds,
  only moves data packets for multiplayer, and collects due autosaves. Before initialization,
  almost nothing answers; only networks can be loaded beforehand. With "player only", the same
  module starts without the editor for exported games.
- **Lehrer:** one module per web worker; each worker collects experience, learns from it, or both.
- **Logic:** These functions aren't called by the page but by a logic graph's code generated at
  runtime. If compiling fails, the graph stays in the interpreter.

Which groups a module contains depends on how it was built: graphics, particles, geometry and logic
are always included, App, Studio and Lehrer only if you switch them on
([Chapter 2](02-building-and-testing.md)). Every single function with its parameters is in the blue
book.

> **Did you know?** The Studio's web page contains no editor logic. Menus, Outliner, Details,
> gizmos, undo – all of that is C++ in the engine. The page only fetches a JSON description of the
> panels through `kern_studio_state()` and draws it. The advantage: the editor runs exactly the same
> code as the exported game, and the whole editor can be tested without a browser.

> **Try it!** In the Studio, open the Content Drawer with Ctrl+Space and look at the **Output
> Log**. It shows the last 200 messages with timestamps, warnings and errors in color, and while
> playing also game events. This list comes from the engine too and has just traveled through the
> serving hatch.

## 17.6 Small glossary

The technical terms from the whole book, briefly explained. The parentheses say where they're
covered in detail.

### Studio and games

| Term | Meaning |
|---|---|
| **Actor** | a thing in the level with a position and building blocks (components) ([Ch. 8](08-game-runtime.md)) |
| **Blueprint / logic graph** | a program made of nodes and connections, without typing ([Ch. 9](09-logic.md)) |
| **Content Drawer** | the Studio's drawer of things to place |
| **Details** | panel with all the settings of the selected actor |
| **Eject / Possess** | in the game, leave / take over the player again |
| **Gizmo** | arrows and rings for moving, rotating, scaling |
| **Level** | a game world, saved as `.klevel` |
| **Outliner** | tree view of all actors |
| **Package Game** | export a level as a self-contained HTML file |
| **Play / Simulate** | play the game / let the physics run while the editor stays usable |
| **Player-only** | the Studio module without the editor, for exported games |
| **Viewport** | the window through which you look into the 3D world |
| **World Settings** | settings for the whole world, such as the step rate |

### Technology under the hood

| Term | Meaning |
|---|---|
| **Box3D** | Erin Catto's physics engine, which Kern includes unmodified ([Ch. 6](06-physics.md)) |
| **C ABI** | the list of functions through which JavaScript talks to the engine |
| **Determinism** | the same inputs produce, bit for bit, the same result |
| **`dm`** | Kern's math functions (sine and friends), which compute the same everywhere |
| **Error channel** | Kern reports errors via `fail()` / `last_error()` instead of exceptions |
| **GameWorld** | displays a level and plays it on Box3D |
| **GTAO** | a technique that realistically darkens corners and crevices ([Ch. 4](04-graphics.md)) |
| **HostTransport** | a network path where the browser page carries the data packets |
| **JIT** | a logic graph is compiled into WebAssembly while the game runs |
| **LoopbackNetwork** | a network inside the same program, for tests and multiplayer in the editor |
| **Mesh** | a 3D shape made of triangles |
| **NetGame** | multiplayer: one server decides, everyone else gets snapshots ([Ch. 10](10-networking.md)) |
| **NightWindows** | shader extension for glowing windows at night |
| **Reactor** | a WebAssembly module without `main`: start it once, then call functions |
| **ShaderExtension** | an add-on for the standard shader that gives a game its own looks |
| **Span** | a view of a row of elements without owning them |
| **Substeps** | sub-steps of the physics per world step; 16 for the duck |
| **Tier** | execution level of a logic graph: interpreter, compiled, native C++ |
| **Viewmodel layer** | a drawing layer on top of everything, e.g. for gizmos |
| **wasi-sdk** | the toolchain that builds Kern for WebAssembly (release 25) |
| **WebAssembly** | a program format that browsers execute very fast |
| **Welding** | merging duplicate vertices of a mesh without loss |

### Robots and learning

| Term | Meaning |
|---|---|
| **Actor / critic** | in training: the part that acts / the part that evaluates |
| **Armature** | extra inertia from the fast-spinning motor inside the servo |
| **CMA-ES** | a search method that tries out candidates and homes in on the best ones |
| **Collector** | runs ducks with a network and delivers rollouts |
| **DAgger** | imitation: the network drives, the demonstration says what would have been right |
| **Device model** | a small model of a real device (servo, detector) with a profile |
| **Domain randomization** | training with random deviations so the network becomes robust |
| **DuckEnv** | the Lehrer's practice ground: one duck in a world of its own |
| **Fingerprint** | a checksum per tick that proves determinism |
| **IMU** | orientation sensor: which way is down, how fast the body is turning |
| **Kern extras** | switch for Kern's own add-ons to the duck, e.g. the play layer |
| **Learner** | the part of training that learns from experience |
| **Lehrer** | Kern's training system for the Micro Duck's networks ([Ch. 14](14-lehrer.md)) |
| **Micro Duck** | two-legged walking robot by Pollen Robotics ([Ch. 13](13-micro-duck.md)) |
| **ONNX** | exchange format for trained networks |
| **Play layer** | Kern's network that decides what the duck does with the ball and sock |
| **Policy** | a trained network: observations in, actions out |
| **PPO** | the Lehrer's learning method (learning through rewards) |
| **Recipe** | the training settings for a task |
| **Rollout** | collected experience: what was observed, done and rewarded |
| **rsl_rl** | the learning library Pollen trains with; the model for Kern's PPO |
| **Servo** | a motor that moves to a given angle |
| **Servo profile** | a servo type's numbers as JSON ([Ch. 16](16-calibration.md)) |
| **ToF** | distance sensor that measures with light (time of flight) |
| **velstand** | Pollen's walking and standing network, built into Kern |
| **Vorbild** | a demonstration that a network imitates first |
| **Walker** | control type for jointed robots driven by a network |

The exact definitions are in the blue book's glossary.

## What you take away

- **F1** in the Studio shows the most important shortcuts; Alt+P plays, Alt+S simulates, Esc
  stops.
- You control the duck with W A S D and the keys 1 to 7.
- `kern_lehrer` trains, evaluates, calibrates and measures – a fixed order of priority among the
  options decides which.
- Levels are `.klevel` files in JSON, networks `.onnx`, servo profiles JSON, logs CSV.
- The engine and the browser talk through the C ABI: 1 means success, 0 failure.
- For every single function and option, look it up in the blue book.

## Brain teasers

1. You've selected a box in the viewport, but you can't see it. Which key helps?
2. A C ABI function returns 0. How do you find out what went wrong?
3. You want to check whether Kern computes exactly the same thing on your new laptop as on the old
   one. Which `kern_lehrer` option do you use?
4. What's the difference between Play and Simulate?

### Solutions

1. **F** brings the selection into view (so does a double-click in the viewport).
2. With `kern_last_error()`, which returns the last error message.
3. `--fingerprint 400`, and compare the result with `kern/tests/determinism_fingerprint.txt`.
4. Play plays the level as a player. Simulate runs the physics and rules while the editor stays
   usable: you can grab bodies and keep poses with K.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
