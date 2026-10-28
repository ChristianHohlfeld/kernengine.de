# 12 Your Own Game

> **Your level:** Now you'll build a whole game – with a player character, obstacles, orbs, danger,
> logic and sound – and send it off as a single file. After that, you'll look over the shoulders of
> pros who write games in C++ or JavaScript.

Kern offers **three ways** to your own game, all with the same graphics, physics and platform
layer:

| Way | With what | Good for |
|---|---|---|
| **A** Studio, no code | Kern Studio, logic graphs | quick games, classes; result: one HTML file |
| **B** C++ | a class that inherits from `kern::App` | your own simulation, your own graphics, your own shaders |
| **C** JavaScript | the engine's C interface | your own pages that control the engine (like SpawnCity) |

On top of that, there's a mix: exporting logic graphs from Studio as C++ (12.4).

---

## 12.1 Way A: Orb Garden, a Game Without Code

We're building a collecting game: a character runs across an area with obstacles, collects orbs,
dodges a danger and wins once everything is collected. [Chapter 11](11-studio.md) explains the
windows.

### Step 1: A New Level

**File › New Level...**, template **Basic**, **Create** – with a ground (40 × 40 m), sun, sky and a
cube. In **World Settings**, set **Level name** and **Game title** to "Orb Garden". Why not Empty?
There isn't even a ground there. **Endless ground** would work, but a visible ground is easier to
picture when you're starting out.

### Step 2: The Player Character

Content Drawer (**Ctrl+Space**) › **Players** › drag **Character** into the viewport. The log
reports "Character is the player now", and the Outliner shows **PLAYER**. In **Details › Player
Controller**, you set **Max speed**, **Acceleration**, **Jump speed** and the camera (**Chase**,
**First person**, **Top down**). **Play** (Alt+P): W A S D runs, the spacebar jumps, **Esc** takes
you back. The player is the character you placed last, or the one you picked with right-click ›
**Make Player**.

> **Mission:** Rebuild steps 1 and 2. Play once with **Top down** and once with **First person**.
> Which view fits a collecting game better?

### Step 3: Obstacles and Physics

1. From **Shapes**, place a few **Cube**s, a **Ramp** and a **Plane**, and adjust them with **W**,
   **E**, **R**; the grid (0.5 m) keeps the edges clean, **End** puts things on the ground.
2. From **Physics**, a **Crate Stack** and a **Hinged Door** – they react to your character.
3. From **Gameplay**, a **Moving Platform** and a **Spinner**, which move on their own.
4. Check with **Simulate** (Alt+S), grab crates and throw them. **K** keeps the position of the
   selected actors – handy for a stack that should look like it fell over naturally. For that, select
   the **crates themselves** in Simulate (short click, Shift+click for more). The Crate Stack is just
   an empty parent actor with the crates attached as children; if it's selected, K only keeps its
   own position, and the crates jump back when you stop.

> **Watch out, trap:** A **Cube** from Shapes has collision, but it doesn't fall. Give it a body
> with **+ Add › Rigid Body** and set **Mobility** to **Dynamic** – or use the **Physics Cube**
> ([Chapter 6](06-physics.md)).

### Step 4: Something to Collect

Spread out **Orbs** (1 point) and a **Star** (10 points); with **Alt + drag the gizmo**, you copy
quickly. Select all the orbs and put them into a folder with the folder button in the Outliner.
Then set **World Settings › Win condition** to **Collect all pickups**.

### Step 5: Danger, Checkpoint and Time Limit

A **Hazard** (loses the game) or a **Damage Zone**, scaled to size with Scale. A **Checkpoint**: if
the character falls below **Kill Z** or loses, it starts over there. If you like, set a **Time
limit**; the HUD counts down. Other goals: **Reach a goal trigger** (with a **Goal**), **Survive the
time limit**, **Reach a target score**.

> **Mission:** Build steps 3 to 5 with a 60-second time limit. Too easy? Put the Star on the Moving
> Platform.

### Step 6: Logic Without Programming

1. Place a **Message Trigger**, and in Details › Trigger set the **Action** to **Logic event only**.
2. Click **Logic** in Details; the logic editor opens this actor's graph.
3. **Templates › Score zone (overlap +10)**: entering gives 10 points, "+10" and a sound.
4. **Compile** (F7) checks, **Viewport** takes you back.

More templates: a door on **Interact** (F key), a spawner, a traffic light and a behavior for the
Micro Duck ([Chapter 9](09-logic.md)).

> **Watch out, trap:** A graph with errors doesn't stop Play. At the start, Kern just switches off
> that one graph, writes "Logic error: …" to the Output Log, and the rest keeps running. If nothing
> happens in your zone, look in the log.

### Step 7: Sound

In **World Settings › Audio**, pick a **Music** and an **Ambience**. From **Audio**, place something
like **Ambient Wind**, or import a file of your own in the **Sounds** tab and drag it into the
viewport. Characters, pickups and triggers already bring their own sounds
([Chapter 7](07-audio.md)).

### Steps 8 to 10: Testing, Saving, Exporting

- **Play** tests, **F8** lets go of the character and takes it over again. **Play › Number of
  Players › 2 Players** tests with two players. **Errors in the level**, such as a joint to a
  deleted actor, prevent Play and show up in the Output Log.
- **Ctrl+S** saves a `.klevel` file including textures and sounds, which you can pass on and open
  in any Studio. Studio also backs up anything unsaved in the browser.
- **File › Package Game (HTML)...**, check the title, **Package**: Studio downloads
  `Orb_Garden.html` (the space became `_`) and offers **Play it**.

The file contains the engine, the level and the game host. It runs offline, as a plain file, from
any web server, and on a phone with touch sticks.

![The same game on a phone: the engine detects touch and draws sticks and buttons](../buch/bilder/game-phone.png)

> **Mission:** Export Orb Garden, click **Play it** and play a round. Then open the file on a phone
> or tablet: the touch sticks show up all by themselves.

---

## 12.2 Way B: A Game in C++

Here, your game is a **class** that inherits from `kern::App`. The engine calls it every frame and
draws afterward; `kern/web/app.mjs` takes care of the browser stuff.

> **Picture it like this:** The engine is the field and the scoreboard, your class is the team.
> Every moment it's "Your turn!" – the team makes its move, and the scoreboard shows it.

### The Building Blocks

Your **App** can fill in `configure()` (graphics options such as shadows and bloom), `extensions()`
(your own shaders), `start()` (create shapes; "false" aborts), `update()` (every frame),
`resized()` and `stop()`. The **Engine** gives you the `renderer` (shapes, camera, light, clicking
on things, [Chapter 4](04-graphics.md)), `input`, `set_status(text)` for the page,
`request_pointer_lock`, start values from the address (`arg`, such as `?time=0.6`), size, `stats()`
and `clear_color`.

**Input** knows `down(Key)` (held), `pressed`/`released` (in this frame), the pointer, mouse
buttons, mouse wheel, up to 10 fingers and the focus. Keys are called `a` to `z`, `digit0` to
`digit9`, `left`, `space`, `enter` and so on – by their **position**. **Frame** provides `time`,
`dt` (seconds since the last frame, at most 0.1) and `index`.

`update` runs **exactly once** per frame, then everything gets drawn. `pressed`, `released`,
pointer movement and the mouse wheel only count for this frame – read them in `update` instead of
saving them for later.

### What Happens in a Frame

The page starts the module, passes on the start values from the address and calls
`kern_create_app()`, `configure()` and `start()`. After that, it passes keys, mouse and fingers
along and calls `update` once per frame, and then the engine draws. If the window size changes,
`resized()` comes; when quitting, `stop()`.

### The Smallest Kern Game: hello

`kern/examples/hello/hello.cpp` (83 lines): 64 crates that spin and cast shadows, a camera you can
orbit, and clicking on things. `configure` turns on shadows (a 1024 map, 40 m). `start` gets a
number for each shape (`allocate_id()`) and creates it with `upsert`.

> **Crazy but true:** The 64 crates are **one** shape with 64 copies ("instances"), each with its
> own 4×4 matrix (16 numbers for position, rotation, size) and color. The graphics card draws all
> of them with **one** draw call.

```cpp
Input& in = engine.input;
if (in.down(Key::left)) yaw_ += frame.dt * 1.5f;
if (in.down(Key::right)) yaw_ -= frame.dt * 1.5f;
```

Line 1 gets the input. If you hold ←, the rotation angle `yaw_` grows by `dt × 1.5` (line 2); with
→, it shrinks (line 3). Do the math: at 60 frames per second, each frame adds 1.5 ÷ 60 = 0.025, and
after one second that's 1.5. At 30 frames, 30 × 0.05 = 1.5. Equally fast – no matter how speedy
your computer is. (1.5 in radians is about 86°.)

The mouse rotates too, the mouse wheel changes the distance (8 to 80 m), and then the camera gets
set again (Z up). Every crate spins at its own speed; `update_instances` only uploads what has
changed. A click shoots a ray through the pixel – the crate it hits turns red. `set_status` writes
draw calls and triangles at the bottom of the page.

```cpp
App* kern_create_app() {
  static Hello app;
  return &app;
}
```

Every game needs this function: the engine calls it once at startup and gets your game. `static`
makes it live as long as the module. The page that goes with it is tiny: a canvas, a status line
and `runKernApp` (12.3).

### Building

You need **wasi-sdk**, release 25 (available on GitHub):

```sh
export WASI_SDK=/opt/wasi-sdk-25.0-x86_64-linux
kern/build.sh --app \
  kern/examples/hello/hello.wasm \
  kern/examples/hello/hello.cpp
```

Line 1 says where wasi-sdk lives; the rest is one command (`\` continues it): build the file
`hello.wasm` from `hello.cpp`. Renderer, geometry and math are always included; switches add more
parts: `--app` (app runtime), `--physics` (Box3D), `--game` (levels and game runtime, with Box3D),
`--studio` (Kern Studio, with `--game`), `--lehrer` (the Lehrer, with `--game`). Files ending in
`.cpp` or `.cc` are sources, the rest goes to the compiler. `KERN_BUILD=debug` builds for hunting
bugs, `KERN_JOBS` and `KERN_OBJ_CACHE` control speed and caching – only what has changed gets
compiled. The result runs in any browser with WebGL2, including Safari 15 and later
([Chapter 2](02-building-and-testing.md)).

### A Whole Game: orbrun

`kern/examples/orbrun/orbrun.cpp` (726 lines): a hovercar collects 24 orbs in a random city while
night falls. W/S drive, A/D steer, spacebar boosts, N makes time go fast, Enter makes a new city; on
a phone, the left half steers and the right half boosts. `?time=0.6` in the address starts at night.
Every frame, the game reads the input, drives, moves the traffic, collects, sets up light and
camera, and uploads whatever changed to the graphics card.

> **Watch out, trap:** The Studio level **Orb Run** is a different program with the same idea: built
> from actors, with Box3D. The C++ example does everything by itself.

- **City:** 7 × 7 blocks, 80 m apart, 16 m wide streets, one of four building layouts per block,
  taller toward the center ([Chapter 5](05-scene-and-geometry.md)).
- **Collision without a physics engine:** The car is a circle (radius 1.7 m) that gets pushed out of
  the buildings and bounces off walls. When boosting, it moves in steps of at most 0.6 m so it
  doesn't slip through walls.
- **Driving physics:** Top speed 36 m/s, with boost 60 m/s – that's 36 × 3.6 = **129.6 km/h** and
  **216 km/h**. Steering only works when you're moving.
- **Traffic:** 22 cars, 9 to 15 m/s; if you're less than 10 m in front of one, it stops.
- **Orbs:** closer than 3.2 m – gone, 28 sparks, and a best time at the end.
- **Day and night:** 480 s per day, with N 40 s; sun, moon, sky and fog follow along. Out of all the
  light requests (headlights, one per orb), the renderer gets the six most important ones.
- **Camera:** follows smoothly, pulls back at speed, shakes on crashes; the mouse wheel changes the
  distance.
- **Status:** a line like `ORBS 5/24 | 01:12.4 | … | 87 km/h`, only sent again when it changes.

> **For pros:** The shader extension **NightWindows** shows how a game can add something to the
> standard shader without touching the engine: every building wall gets four numbers (length,
> height, random value, floor height), and from those the shader paints a grid of windows that glows
> at night. In the distance it blurs so nothing flickers ([Chapter 4](04-graphics.md)).

### Setting Up Your Own Game

Create a folder `mygame/` with `mygame.cpp` (your App class plus `kern_create_app()`) and
`index.html`, build with `kern/build.sh --app` (add `--physics` for Box3D, `--game` for levels)
and serve the folder with `kern.mjs` and `app.mjs` from a web server (12.5).

---

## 12.3 The Platform Layer app.mjs

`kern/web/app.mjs` only does what a browser allows exclusively in JavaScript.

```js
const app = await runKernApp({
  canvas: document.getElementById("game"),
  url: "./mygame.wasm",
});
```

Line 1 starts your game and waits until it's running; line 2 says where to draw; line 3, which file
to load. Instead of `url`, bytes or Base64 work too. Optionally there's `onStatus`, `onError`,
`maxPixelRatio`, extras for WebGL2 and start values; what you get back is an object with `stop()`.
The host keeps the canvas at the right size, reads keys by position (arrow keys don't scroll),
passes mouse and fingers along, releases all keys when focus is lost, captures the mouse on request
and rebuilds everything after a graphics card crash. If the module was built without `--app`, you
get "module was not built with --app".

---

## 12.4 Way C: From JavaScript

**Graphics from JavaScript (like SpawnCity):** `kern/web/kern.mjs` with `loadKern` and
`KernRenderer`. Game logic in JavaScript, drawing in C++: create shapes with names (`upsert`),
change copies, set the camera, `render()`, click on triangles with `pick`. Nothing new gets created
during a frame; there's one renderer per module ([Chapter 17](17-reference.md)).

**Building in Studio levels:** `startGame` from `kern/web/studio/game.mjs` is the same game host as
in exported files. `play.html` plays built-in levels (`?level=orbRun`, `physicsLab`,
`shootingRange`, `microDuck`, `basic`); you pass in a level of your own as the text of your
`.klevel` file. What comes back is an object (also `window.KernGame`):

```js
game.command({cmd: "gameInput", moveY: 1, jump: true});
game.command({cmd: "gameInput", moveY: 0, jump: false});
if (game.status.hud?.status === "won") showMyWinScreen();
```

Line 1 makes the character run forward and jump – like your own on-screen buttons. Line 2 lets go.
Line 3 asks whether the game was won and then calls `showMyWinScreen()`. This function doesn't
exist in Kern: it's an example of your own code that shows your victory screen. `gameInput` also
knows `moveX`, `up`, `boost`, `fire`, `aim`, `reload`, `crouch`, `interact`, `lookX` and `lookY`;
the values add up with keyboard, touch and gamepad. The object also has `engine` (all Studio
functions), `audio` and `net`. If you want to work with no help at all, you call the
`kern_studio_…` functions directly.

**Logic graphs as C++:** The **C++** button in the logic editor downloads the graph as C++ text.
Built in with `kern/build.sh --studio`, the engine finds it by a fingerprint (hash) of the graph and
runs it as machine code instead of through the interpreter or JIT – with the same result
([Chapter 9](09-logic.md)).

---

## 12.5 Hosting

A C++ game consists of `mygame/index.html`, `mygame.wasm` and `kern/web/app.mjs` with `kern.mjs`
right next to it. Because the page loads the module over the network, you need a **web server**; on
your own computer, a simple one in the main folder of the repository is enough.

> **Picture it like this:** The exported Studio game is a ready-made packed lunch in a box. The C++
> game is cooking in the school kitchen – you need the kitchen for it.

---

## 12.6 Publishing

**The rights:** Kern is open source (Apache License 2.0). You may use, pass on and sell your own
games and networks, even without showing your source code. If you pass on an exported game, include
the license and NOTICE (in the HTML, a note with a link is enough) and leave the copyright notices
in. "Made with Kern" is allowed; calling your product "Kern" is not. Parts from others keep their
licenses ([Appendix D](D-licences.md)).

**The Micro Duck exception:** Pollen's 3D models of the duck (CC BY-NC-SA 4.0) are allowed **only
for non-commercial** use. To sell, you build with `KERN_COMMERCIAL=1 tools/build_studio.sh` (or
`kern/build.sh --commercial`); then the duck has no appearance. Pollen's networks (Apache 2.0) may
come along with the license and NOTICE.

**Studio games:** you upload them to web space, put them in a repository with GitHub Pages or send
them directly. With the duck, `microduck/` and `policies/` go next to them; otherwise it can only
walk ([Chapter 11](11-studio.md)). Online play works with `?host=room`, `?join=room` and, for
different computers, a relay (`node tools/net_relay.mjs`, port 8790) plus `&relay=`
([Chapter 10](10-networking.md)).

**C++ games:** you upload them with the same folder structure. That's how Kern itself, in its CI
(`.github/workflows/ci.yml`), builds Studio, hello and orbrun and publishes everything on GitHub
Pages at https://kernengine.de/ – Studio as an installable app in the `editor/` folder, with the
duck's network folders; the source code is at https://github.com/ChristianHohlfeld/kernengine.de.

| Checklist | Studio game | C++ game |
|---|---|---|
| no errors | Output Log | browser console, `onError` |
| phone | sticks show up by themselves | handle touch yourself |
| sound | after the first click | up to the game |
| files | one HTML (plus the duck folders) | HTML, `.wasm`, `kern.mjs`, `app.mjs` |
| delivery | anywhere, even as a file | web server |

> **Mission:** Go through the checklist for Orb Garden: Log without errors? Does it run on a phone?
> Music after the first tap? Then: publish!

---

## Level complete!

- You know the three ways to a Kern game.
- You built a collecting game in ten steps, saved it and exported it.
- You know that a broken logic graph doesn't stop the game – and where to look.
- You understand `update` and `dt` in a C++ game.
- You know why 64 crates are drawn with one draw call.
- You know the rules for passing on and selling.

## Puzzle corner

1. Why is **Basic** a better start than **Empty**?
2. Your Shapes cube doesn't fall. What's missing?
3. Why does the exported Studio game run with a double-click, but the C++ game doesn't?
4. **Boss puzzle:** A game moves a character with `x += 5 * frame.dt`. Computer A manages 50 frames
   per second, B manages 25. How far does the character get in 2 seconds? And with `x += 0.1`
   without `dt`?

### Solutions

1. Empty has **no ground**.
2. A **Rigid Body** with **Mobility = Dynamic** – or just use a Physics Cube.
3. The Studio game has **everything in one file**; the C++ game loads `.wasm` and modules
   separately, which the browser only allows through a web server.
4. With `dt`, both get **10 m** (5 m per second times 2 s). Without `dt`, A gets 100 × 0.1 = 10 m
   far, B only 50 × 0.1 = **5 m**. That's why games calculate with `dt`.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
