# 12 Your Own Game

> **In short:** Now it's your turn! Kern offers three paths to your own game: in Studio with no code at
> all, in C++ with the app API, or from JavaScript. All three use the same engine with the same
> renderer and the same physics. First we'll build a small collecting game in Studio, step by step,
> and then look at how pros control Kern from code.

| Path | Tool | Result | Ideal for |
|---|---|---|---|
| **A** without code | Kern Studio, logic graphs | one HTML file | levels, prototypes, teaching |
| **B** in C++ | the app API | a `.wasm` and a page | your own simulation, your own shaders |
| **C** from JavaScript | the module's C ABI | your own page | existing JS projects |

The paths don't exclude each other. There's even a hybrid: logic graphs from Studio can be exported as
C++ (12.4).

---

## 12.1 Path A: a game in Studio, without code

We're building "Orb Garden": a character runs across an area with obstacles, collects orbs, avoids a
hazard zone and wins once everything has been collected. You know how to use the panels from
[Chapter 11](11-studio.md). Let's go!

### Step 1: A new level

Open Kern Studio (via https://kernengine.de/ or offline as `generated/studio.html`), then
**File › New Level...**, template **Basic**, **Create**. You have a ground (40 × 40 m), sun, sky and a
cube. In **World Settings**, enter "Orb Garden" as the **Level name** and the **Game title**.

> **Watch out!** Why not **Empty**? Because Empty really has **nothing**, not even a ground. Your
> character would fall into the void. (World Settings does have an **Endless ground**, but a visible
> ground with a color is easier to picture at the start.)

### Step 2: The player character

Open the Content Drawer (**Ctrl+Space**) and drag a **Character** from **Players** into the viewport.
The log reports "Character is the player now", and in the Outliner it wears the **PLAYER** badge. In
**Details › Player Controller** you set the speed (**Max speed**), **Acceleration**, **Jump speed** and
the camera (**Chase**, **First person**, **Top down**).

> **Try it!** Press **Play** (Alt+P) right away. W A S D runs, the mouse looks, the space bar jumps,
> **Esc** takes you back. Just for fun, swap the Character for a **Rolling Ball** or a **Drone** from
> **Players**: the one placed last is the player, or you choose one with right-click › **Make
> Player**.

### Step 3: Obstacles and physics

1. From **Shapes**, place a few **Cube**s, a **Ramp** and a **Plane**. Adjust them with **W**, **E**,
   **R** and the gizmo; the 0.5 m grid keeps the edges neat, and **End** puts things on the ground.
2. From **Physics**, a **Crate Stack** and a **Hinged Door**: they react to your character.
3. From **Gameplay**, a **Moving Platform** and a **Spinner**, which move by themselves.
4. Check with **Simulate** (Alt+S). You can grab and throw bodies, and **K** keeps the simulated
   position, for example for a crate stack that should look naturally collapsed.

A **Cube** from Shapes has collision but doesn't move. If it should fall, give it physics in Details
with **+ Add › Rigid Body** and **Mobility = Dynamic**, or just take a **Physics Cube** right away
([Chapter 6](06-physics.md)).

### Step 4: Something to collect

1. From **Gameplay**, spread out several **Orb**s (1 point) and a **Star** (10 points). Trick:
   **Alt + dragging the gizmo** pulls out a copy with every drag.
2. Select all the orbs and put them into a folder with the folder button in the Outliner.
3. In **World Settings › Win condition**, choose **Collect all pickups**.

### Step 5: Hazard, checkpoint and time limit

1. Place a **Hazard** (loses the game) or a **Damage Zone** and drag it to size with Scale.
2. Place a **Checkpoint**: if the character falls below **Kill Z** or loses, it starts over here.
3. If you like, set a **Time limit** in World Settings: the HUD counts down.

Other win conditions: **Reach a goal trigger** (with a **Goal**), **Survive the time limit** or
**Reach a target score**.

### Step 6: Logic without programming

1. Place a **Message Trigger** and in Details › Trigger set the **Action** to **Logic event only**.
2. Click **Logic** in Details.
3. **Templates › Score zone (overlap +10)**: whoever enters the zone gets 10 points, a "+10" message
   and a sound.
4. **Compile** (F7), then **Viewport**.

More templates: a door that opens on **Interact** (F key), a spawner, a traffic light and a behavior
for the Micro Duck. All the nodes are explained in [Chapter 9](09-logic.md).

### Step 7: Sound

In **World Settings › Audio**, choose a **Music** and an **Ambience**. From the **Audio** group, place
something like **Ambient Wind**, or import a file of your own in the **Sounds** tab and drag it into
the viewport. Characters, pickups and triggers already bring their own sounds
([Chapter 7](07-audio.md)).

### Step 8: Testing

**Play** and play! **F8** lets go of the character, the world keeps running, and you look around; F8
again takes it over. With **Play › Number of Players › 2 Players** you test with two players. Errors in
the level (such as a joint pointing to a deleted actor) prevent Play and are listed in the **Output
Log**.

### Step 9: Saving

**Ctrl+S** saves a `.klevel` file. It contains everything, including textures and imported sounds. You
can pass it on and open it in any Studio (Ctrl+O or drag it onto the page). Studio also backs up unsaved
work in the browser and offers it at the next startup.

### Step 10: Exporting

**File › Package Game (HTML)...**, check the title, **Package**. Studio downloads `Orb_Garden.html`
and offers **Play it**. This one file contains the engine, the level and the game. It runs offline,
straight from the hard drive, from any web server and on a phone with touch sticks.

> **Try it!** Send `Orb_Garden.html` to your phone and open it. The engine detects the touchscreen and
> draws sticks and buttons by itself. You've just built a mobile game!

![The same game on a phone: the engine detects touch and draws sticks and buttons](../buch/bilder/game-phone.png)

---

## 12.2 Path B: a game in C++ with the app API

For games that want more than actors and components, you write the game yourself in C++. Your game is
a class that inherits from `kern::App`. The engine calls it every frame and draws afterward. The
platform layer `app.mjs` takes care of everything browser-specific (12.3).

> **Imagine …** a theater. The engine is the stage crew with lights, curtain and scenery. Your app is
> the play. At every moment, the crew asks you: "What happens now?" You tell them, and they make it
> visible.

### The building blocks

Your app has a few methods that the engine calls at the right time:

- **`configure()`** once, up front: shadows, bloom and other renderer wishes.
- **`start()`** once: create meshes, get everything ready. If it returns `false`, everything is
  aborted.
- **`update()`** **every screen frame**: read input, advance the game, change the scene.
- **`resized()`** after a size change, **`stop()`** for cleaning up. Plus `extensions()` for your own
  shaders (they must live as long as the app).

In return, it gets the **engine** with the renderer ([Chapter 4](04-graphics.md)), the **input**
(keys, mouse, up to 10 fingers), `set_status` for a text on the page, the option to ask for the mouse
to be captured, and start parameters from the address (such as `?time=0.6`). Every **frame** gives
`time` (seconds since the start), `dt` (seconds since the last frame, at most 0.1) and a running
number. You query the input like this: `down` (key held), `pressed`/`released` (pressed or released in
this frame), pointer position and movement in pixels of the drawing surface (even with the mouse
captured), mouse buttons, mouse wheel, fingers and whether the page has focus. Keys count by their
**position**, as in Studio. The engine also provides the size of the drawing surface, the statistics
of the last frame (draw calls, triangles) and a background color for areas without sky. All the
signatures are in the blue book.

### A frame, step by step

The page (`app.mjs`) starts the module: hand over start parameters, get `kern_create_app()`,
`configure`, create the renderer, `start`. After that it passes along keys, mouse, fingers and focus.
For every screen image, it calls the frame: first your `update`, then the engine resets the values that
only apply to this one frame, then it draws. A size change leads to `resized`, a context loss pauses
the renderer, and the page's `stop()` calls your `stop` and shuts down the renderer.

> **Watch out!** "Pressed in this frame" (`pressed`), "released", mouse movement and mouse wheel apply
> only to **this one** frame; after that the engine resets them. So read them in `update` and don't
> store them for later.

### The smallest Kern game: hello

`examples/hello` has just 83 lines: a field of 64 spinning boxes with shadows, a camera you can orbit
with the mouse or arrow keys, and a click turns a box red. Here's how it starts:

```cpp
class Hello final : public App {
 public:
  RendererOptions configure() override {
    RendererOptions o;
    o.shadows = true;
    o.shadow.resolution = 1024;
    o.shadow.extent = 40;
    return o;
  }
```

Line by line: `Hello` is your game and inherits from `App`. `configure` sets up the renderer before it
exists: shadows on, the shadow map is 1024 pixels on each side and covers 40 m. At the end, the
function returns the wishes.

A few more ideas from hello, in words:

- **Creating meshes:** every mesh gets an ID (`allocate_id`). `upsert` creates it or replaces it; if
  it's invalid, the old one stays untouched and `upsert` returns `false`, so `start` aborts. The ground
  and the box come from ready-made basic shapes (`primitives::plane`, `box_on_ground`).
- **Instancing:** the 64 boxes are **one** mesh with 64 copies (instances), each with its own position
  (a 4×4 matrix of 16 numbers) and color (a "tint" of 4 numbers; white leaves the orange base color as
  it is). A single drawing command (draw call) paints them all. They stand in an 8×8 grid, 3 m apart;
  every frame, `update_instances` uploads the new matrices, and the renderer spots what hasn't changed
  and leaves it out.
- **Frame-rate independent:** the arrow keys turn the camera by `dt` × 1.5. On a fast screen there are
  more but smaller steps; the rotation per second stays the same. Holding the left mouse button also
  turns it, and the mouse wheel changes the distance (8 to 80 m). The camera is simply a field of the
  renderer with an eye point and a target.
- **Picking:** a click shoots a ray through the pixel. If it hits a box, the renderer reveals which
  instance it was, and its color turns red.
- **Status:** `set_status` sends a text such as the number of draw calls to the page; the small
  `index.html` starts the module with `runKernApp` and shows the text at the bottom left.

At the very end is the one function every game must have:

```cpp
App* kern_create_app() {
  static Hello app;
  return &app;
}
```

The runtime calls it exactly once at startup and gets your game. The `static` makes sure it lives as
long as the module.

### Building

You need the wasi-sdk tool (release 25). Then:

```sh
export WASI_SDK=/opt/wasi-sdk-25.0-x86_64-linux
kern/build.sh --app kern/examples/hello/hello.wasm \
  kern/examples/hello/hello.cpp
```

`--app` adds the app runtime. Other switches: `--physics` for Box3D, `--game` for levels and the game
runtime, `--studio` for all of Studio, `--lehrer` for the Lehrer. Everything that ends in `.cpp` or
`.cc` is your game's source code (several files are fine), and all other arguments go to the compiler
as switches. Environment variables select, for example, a debug build (`KERN_BUILD=debug`); the list
is in the blue book.

> **Did you know?** The build remembers every compiled file under a fingerprint made from the source,
> the settings and all included files. A second build only compiles what has changed, and Studio and
> the examples share the compiled engine. The finished module runs in any browser with WebGL2, from
> Safari 15 on ([Chapter 2](02-building-and-testing.md)).

### A whole game: orbrun

`examples/orbrun` (726 lines) is a complete racing game, written only against the engine API: a
hovercar races through a city and collects 24 orbs while night falls. Controls: W/S drive, A/D steer,
space bar boost, N fast time, Enter new city; on a phone, the left half of the screen steers and the
right half boosts.

> **Watch out!** Don't mix them up: the **Orb Run** level in Studio is a **different** program with the
> same idea, built from actors and components. The C++ example does everything itself, even the driving
> physics and collision.

Every frame of orbrun reads the input, drives, moves the traffic, collects, adjusts the time of day
and the camera, uploads whatever changes to the graphics card and writes the status line, such as
`ORBS 5/24 | 01:12.4 | BOOST [######----] | 87 km/h`. `?time=0.6` in the address starts at night.
What's inside:

- **The city:** 7 × 7 blocks 80 m apart, 16 m wide streets, each block randomly (from a seed) given one
  of four layouts: one large building (often with a courtyard), four plots, an L-shaped building or
  three bars; toward the center, the buildings get taller. The trees are instanced (one mesh, many
  copies). The engine's building and road builders generate the meshes
  ([Chapter 5](05-scene-and-geometry.md)).
- **Collision without a physics engine:** every building is a polygon, and the car is a circle with a
  1.7 m radius that is pushed out of the walls of the neighboring cells (40 m grid) and bounces off;
  the city edge is a solid boundary.
- **Driving feel:** at most 36 m/s, with boost 60 m/s. Gas accelerates at 24 m/s², reverse at 21,
  boost with an extra 42. Sideways sliding fades quickly, steering only takes effect with speed (fully
  from 9 m/s) and reverses when going backward. The boost supply drains at 0.45 per second and
  recharges at 0.12. So that the car doesn't "tunnel" through walls while boosting, it moves in
  sub-steps of at most 0.6 m.
- **Traffic:** 22 cars at 9 to 15 m/s, 2.8 m beside the center of the street; if the player is less
  than 10 m in front of one, it stops. At the city edge, they reappear on the opposite side.
- **Orbs:** closer than 3.2 m, and the orb sprays 28 sparks from the engine's particle system (one draw
  call). With the last one, the run ends with a best time.
- **Day and night:** a day lasts 480 s, with N only 40 s. Sun, moon, sky and fog are blended to match.
  Headlights and orbs glow, more strongly at night; of all the light requests, a `LightPool` gives the
  renderer the six most important ones, because the standard shader knows six point lights.
- **Camera:** follows the car smoothly, pulls back with speed, shakes on impacts, widens the field of
  view above 30 m/s; the mouse wheel changes the distance.

The showpiece is the shader extension **NightWindows**. A **shader** is a small program on the
graphics card that colors each pixel. The building builder gives every wall four extra values (length,
height, random value, story height). The extension turns them into a grid of windows, some of which
light up randomly at night, without touching the engine. The values travel on as `flat`, meaning
unchanged across the whole wall, because blending them would destroy the random pattern of the
windows. A `uNight` control (set once per frame) dims the windows during the day. Only buildings get
these extra values, not streets. In the distance, the grid blurs into a soft shimmer so nothing
flickers. How such extensions work is in [Chapter 4](04-graphics.md).

### Creating your own game

1. Create a folder such as `mygame/`, with `mygame.cpp` (a class that inherits from `App`, plus
   `kern_create_app()`) and an `index.html` as in hello.
2. Build with `kern/build.sh --app`; add `--physics` for Box3D and `--game` for levels.
3. Serve the folder together with `kern.mjs` and `app.mjs` through a web server (12.5).

---

## 12.3 The platform layer `app.mjs`

`app.mjs` is the host for C++ games in the browser. It only takes on what the browser forces on you in
JavaScript. A single line `runKernApp({canvas, url, onStatus})` starts your game, and then it takes
care of:

- **Size:** the drawing surface always adapts to the window, at full device resolution.
- **Keyboard:** by key position; arrow keys and the space bar no longer scroll the page.
- **Mouse and touch:** position, buttons, wheel, fingers; no context menu and no scrolling on the
  game. Repeated key events don't count.
- **Focus:** when the window loses focus, the engine releases all keys. After Alt+Tab, nothing gets
  stuck.
- **Capturing the mouse** (*pointer lock*) when the game asks for it; when the page is hidden, the
  renderer learns about it every frame.
- **Context loss:** if the browser briefly loses the graphics card, the renderer pauses and afterward
  uploads everything again from its copies.

It can load the module by address, bytes or Base64; further options (error callback, maximum pixel
density, start parameters) are in the blue book. What comes back is an object with `stop()`. It
rejects a module built without `--app`: "module was not built with --app".

---

## 12.4 Path C: from JavaScript

The module offers its functions as a flat **C ABI**: simple functions that JavaScript can call
directly. There are two ready-made facades for that.

**Using the renderer from JavaScript.** `kern.mjs` contains `KernRenderer`. That's how SpawnCity
works: the game logic is JavaScript, the drawing happens in C++. After `loadKern` (loads the module),
you create meshes under a name with number arrays, set the camera, render (you get statistics back)
and pick down to the exact triangle. The facade copies the arrays into a reused area in the module's
memory; no new memory blocks are created during the frame. There is one `KernRenderer` per module. The
methods are in [Chapter 17](17-reference.md).

**Embedding Studio levels in your own page.** `game.mjs` offers `startGame`, the same game host that
exported games contain. The module is Studio's (built with `--studio`). You give `startGame` the text
of a `.klevel` file; it needs a canvas and the ready-made HUD elements from `game.mjs`, starts the
module in player mode, loads the Micro Duck's networks if needed and starts Play. The example page
`play.html` plays the built-in levels this way (`?level=orbRun`, `physicsLab`, `shootingRange`,
`microDuck`, `basic`). What comes back is an object (also as `window.KernGame`) with `command`,
`status`, the engine, sound and network. With it you can control the game from your code, for example
with your own on-screen buttons:

```js
game.command({cmd: "gameInput", moveY: 1, jump: true});
game.command({cmd: "gameInput", moveY: 0, jump: false});
if (game.status.hud?.status === "won") showMyWinScreen();
```

The first line makes the character run forward and jump (values from −1 to 1, or true/false), the
second stops again. The third asks the HUD: if the player has won, you show your own victory screen.
`showMyWinScreen` is your own function here. Your inputs add up with keyboard, touch and gamepad.
Besides movement and jump, `gameInput` knows up, boost, fire, aim, reload, crouch, interact and look.
If you don't want a facade, call the `kern_studio_*` functions directly, as Studio itself does
([Chapter 11](11-studio.md)).

**Logic graphs as C++.** In the logic editor, the **C++** button downloads a graph as readable source
code. It registers itself with its graph via a fingerprint (hash). If you build it into a module with
`--studio`, the graph runs as native code instead of in the interpreter or JIT. Interpreter, JIT and
C++ produce the same result ([Chapter 9](09-logic.md)).

---

## 12.5 Hosting

A C++ game consists of just a few files: your `index.html`, your `.wasm`, and `app.mjs` and
`kern.mjs`, which have to sit **side by side**. Because the page loads the module afterward, you need a
web server; double-clicking it as a local file doesn't work. On your own computer, any simple server in
the repository's root folder will do; then you open `/kern/examples/hello/index.html`. The examples
expect exactly this layout (`../../web/app.mjs`).

> **Did you know?** An exported **Studio** game doesn't need any of that. It's a single file that also
> runs straight from the hard drive.

---

## 12.6 Publishing

### The rights

Kern is open source under the **Apache License 2.0**. You may use, pass on and **sell** your games and
networks, even without source code. If you pass on an exported game, include the license and the
NOTICE (in the HTML, a note with a link is enough) and leave the copyright notices in place. The names
"Kern" and "Kern Studio" and the logo are not part of the license: "Made with Kern" is fine, calling
your own product "Kern" is not. Other people's components keep their own licenses
([Appendix D](D-licences.md)).

> **Watch out!** The **Micro Duck** is an exception. The Studio module and every exported game contain
> Pollen's 3D models of the duck, and they may only be used **non-commercially** (CC BY-NC-SA 4.0). If
> you want to sell, build the module with `KERN_COMMERCIAL=1 tools/build_studio.sh` (or
> `kern/build.sh --commercial`). Then the models are left out, and the duck is invisible. Pollen's
> networks (Apache 2.0) may come along with their license and NOTICE.

### Studio games

You upload the HTML file to any web space, put it in a repository with GitHub Pages, or simply send it
to someone. It runs offline, full screen, with keyboard, mouse, gamepad and touch.

- **With the Micro Duck:** put the `microduck/` and `policies/` folders next to it, otherwise the duck
  can only walk. Pollen's license and NOTICE belong with `microduck/`.
- **Online:** adding `?host=raum` to the address offers a game, `?join=raum` joins. For different
  computers, the relay runs on a reachable server (port 8790, npm package `ws`), and everyone adds
  `&relay=` with its address ([Chapter 10](10-networking.md)).

### C++ games

Upload what 12.5 shows, with the same relative paths. Kern publishes itself on GitHub Pages in exactly
this way: the automated pipeline builds Studio, hello and orbrun and copies everything, including the
duck's network folders, into place; Studio also gets a manifest, service worker, icons and
`build.json` for app installation and updates. The result is at https://kernengine.de/.

### Checklist

| Item | Studio game | C++ game |
|---|---|---|
| No errors | Output Log, Play | browser console |
| Phone | sticks appear by themselves | handle touch yourself |
| Sound | starts with the first tap | up to the game |
| Files | one HTML (plus duck folders) | HTML, `.wasm`, two `.mjs` |
| Delivery | anywhere, even from the drive | web server |

---

## What you take away

- Three paths: **Studio without code**, **C++ with the app API**, **JavaScript via the C ABI**, all on
  the same engine.
- A Studio game comes together in ten steps: level, character, obstacles, pickups, hazard, logic,
  sound, testing, saving, exporting.
- **Package Game** turns it into a single HTML file that runs anywhere, even on a phone.
- A C++ app inherits from `App`; `update` runs every frame, `kern_create_app()` delivers the game.
- `hello` shows instancing and picking, `orbrun` a whole game with its own shader extension.
- From JavaScript you control the renderer (`KernRenderer`) or whole Studio levels (`startGame`).
- You may sell your games; only the Micro Duck with Pollen's models is non-commercial.

## Brain teasers

1. Why do you start your game with **Basic** and not with **Empty**?
2. Your cube from **Shapes** doesn't fall down. What's missing?
3. Why do you read `pressed()` in `update` and not store it for later?
4. Your C++ game doesn't start when you double-click `index.html`. Why?
5. You want to sell a game with the Micro Duck. What do you need to keep in mind?

### Solutions

1. Empty has no ground; the character would fall.
2. A **Rigid Body** with **Mobility = Dynamic** (or just take a **Physics Cube**).
3. It only applies to this one frame and is reset afterward.
4. The page loads the module afterward; for that it needs a web server.
5. Pollen's 3D models may only be used non-commercially. A commercial build leaves them out, and the
   duck is then invisible.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
