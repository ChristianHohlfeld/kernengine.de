# 11 Kern Studio: Your Workshop in the Browser

> **Your level:** Time to build! Kern Studio is the editor where you put worlds together, try out
> physics, test games and send them off as a file of their own. This chapter is a guided tour with
> lots of missions – it's best to have Studio open while you read.

Kern Studio works like the Unreal Engine (but Kern is not a product of Epic Games). A level is made
of **actors** – all the things in your world. Every actor carries **components**, building blocks
like "looks like a cube" or "falls down". You edit them in **viewports** (windows into the 3D
world), in the **Outliner**, in **Details** and in the **World Settings**. **Play** and
**Simulate** run right inside the viewport.

Here's the special part: the whole editor is C++ and runs in the **same WebAssembly module** as
graphics, physics and the game runtime. What you see in the viewport is calculated by exactly the
code that later runs your exported game (11.19). How to turn it into a game is shown in
[Chapter 12](12-your-own-games.md).

> **Watch out, trap:** Studio follows your system language. On a German system, it shows German
> labels – "File" becomes "Datei", "Play" becomes "Spielen". This book uses the English names. The
> **DE | EN** button at the bottom right of the status bar switches to English, and so does
> `?lang=en` at the end of the address.

---

## 11.1 Starting Studio

Three ways lead to Studio: **live** via https://kernengine.de/ (the editor lives in the folder
/editor/ and runs on phones too), as **a single file** `generated/studio.html`, which
`tools/build_studio.sh` builds and which runs offline, or for **development** via
`kern/web/studio/index.html` on a web server. The home page also links to the Lehrer, example
games, two levels to play, the license (Apache 2.0), the source code and the imprint.

After "Starting the engine", on the very first start, **Orb Run** is loaded: a random city with
traffic, a hovercar, 24 energy orbs and a day that turns into night ("Kern Studio ready - Orb Run
loaded"). After that, Studio always opens the level you last worked on and offers **Start fresh**
for a new beginning with Orb Run. If there were unsaved changes, it asks **Restore "…" from your
last session?** (11.15).

> **Mission:** Open Kern Studio, wait for Orb Run and press **Alt+P**. Drive to three orbs with
> W A S D. **Esc** takes you back to the editor.

### Installing as an App

In Chrome and Edge, the title bar shows **⤓ Install**; on iPhone and iPad, the same button explains
the steps **Share** and **Add to Home Screen**. Once installed, Studio starts in full screen, and
the notch and status bar stay clear. A service worker keeps the latest version ready offline. Studio
regularly checks for updates (also every 60 s). If nothing can be lost, it reloads right away;
otherwise **New version available** appears with **Update** and **Later** – your level travels
along via the autosave. The running version is shown at the bottom right of the status bar.

---

## 11.2 The Interface at a Glance

![Selected actor with the Move gizmo and its components in Details](../buch/bilder/studio-selected.png)

| Area | Where | Contents |
|---|---|---|
| Title bar | top | menus, level name (dot = unsaved), frames per second |
| Toolbar | below it | save, undo, Add, tools, grid, Play, Logic, Export |
| Viewports | middle | 1, 2 or 4 views into the world |
| Outliner | top right | all actors as a tree |
| Details / World Settings | bottom right | the selection or the whole world |
| Content Drawer | bottom, collapsible | Place Actors, Textures, Sounds, Output Log |
| Status bar | very bottom | console, messages, physics numbers, sound, version |

You can drag the dividing lines; Studio remembers the sizes and whether the Content Drawer was
open.

### Menu Bar

When a menu is open, you switch to the next one just by moving over it. With the mouse, submenus
open **next to** the menu; with a finger, or when there's no room, they replace it right where it
is, with a back row. That way, a menu never ends up under your finger. **Esc** closes, and anything
gray doesn't do anything right now.

- **File:** New Level (Ctrl+N), Open Level (Ctrl+O), Save (Ctrl+S), Save As (Ctrl+Shift+S), Import
  Texture, Package Game (HTML) and Example Levels.
- **Edit:** Undo (Ctrl+Z), Redo (Ctrl+Y), History, Cut/Copy/Paste, Duplicate (Ctrl+D), Delete
  (Del), Select All (Ctrl+A), Deselect (Esc), Group (Ctrl+G), Snap to Floor (End), Preferences,
  Reset Settings to Defaults.
- **Window:** Viewport Layout, Viewport Type, View Mode, Show, Game View (G), Focus Selection (F),
  Content Drawer (Ctrl+Space), Output Log, Fullscreen (F11).
- **Play:** Play (Alt+P), Simulate (Alt+S), Pause, Advance One Frame (.), Stop (Esc),
  Eject/Possess (F8), Restart Game (Ctrl+R), Keep Simulation Changes (K), Number of Players,
  Network Emulation, Host/Join/Leave Online Game.
- **Help:** Controls (F1) and About Kern Studio.

**New Level** has six templates (a double-click creates one right away):

| Template | What's inside |
|---|---|
| Orb Run | night city, hovercar, 24 orbs |
| Physics Lab | every Box3D joint: hinge, slider, wheel, rope bridge, motor, stack |
| Shooting Range | soldier, targets, cover, multiplayer-ready |
| Micro Duck | walking robot by Pollen Robotics with a trained network |
| Basic | ground, sun, sky |
| Empty | nothing at all |

Unsaved changes to the old level stay in the autosave until you edit the new one.

### Toolbar

From the left: save, undo/redo, **+ Add ▾** (all catalog groups), the tools **Select / Move /
Rotate / Scale** (Q / W / E / R), globe or cube for world or local axes (Ctrl+`), three snap buttons
(grid 1 cm to 100 m; angle 1° to 90°; scale 0.0625 to 1), camera speed 1–8, then Play, Simulate,
Pause, Frame, Stop, Eject, Layout, **Logic**, the package (export) and, on narrow screens,
**Panels**.

### Viewports

Every viewport has a bar: **≡** (Game View, field of view 50°–110°, Focus, bookmarks, Layout), the
**view type** (Perspective, Top, Bottom, Left, Right, Front, Back), the **View Mode** (Lit, Unlit,
Wireframe, Player Collision), **Show** (Grid, Icons, Selection outline, Collision shapes,
Constraints, Contact points, Bounds, Fog, Sky, Stats) and **Maximize**. Clicking a viewport makes it
**active**. In flat (orthographic) views, Studio switches to Wireframe, and back in perspective it
switches to Lit again. **Constraints** starts out off; otherwise every chain would draw helper
lines.

At the bottom left, the **axis gizmo** shows X in red, Y in green, Z in blue – **Z points up**. The
info and hint lines give the speed, the camera position (flat: the visible width) and what you can
do right now. While you drag the gizmo, it shows how far you're moving (in m), rotating (in °) or
scaling.

### Outliner

The Outliner lists all actors with an **eye**, **Label** and **Type** (such as Player, Pickup,
Static Mesh or Empty – depending on the components).

| Action | Effect |
|---|---|
| Click, Ctrl+click, Shift+click | select, add/remove one, everything in between |
| Double-click | bring into view; rename a folder |
| F2 | rename |
| Eye / arrow | show/hide / expand children |
| Drag onto an actor | attach as a child |
| Drag onto a folder / into empty space | into the folder / detach |
| Folder button | new folder for the selection |
| Search field | filter by name, type and tags |

Right-click opens the context menu; for folders, it also has Rename Folder and Remove Folder (keep
actors). The controlled actor is marked **PLAYER**, locked ones have a padlock. Folders are only for
top-level actors.

### Details

At the top are the name, **Logic** (this actor's graph) and **+ Add** (component); below them, a
search field and the sections **Actor**, **Transform** and one per component (collapsible, with a
trash can). With several actors selected, only shared components appear; different values show
"Multiple".

- **Numbers:** type them in (calculations like `2*3` work, and a comma counts as a decimal point),
  drag sideways on the label, or change them with ↑/↓ – Shift ×10, Ctrl ×0.1. Fields from 0 to 1
  have a slider.
- **Vectors** have X, Y, Z; Scale has a lock for uniform scaling. **Colors** via a picker or a hex
  value. **Tags** separated by commas.
- The **arrow** on the right resets to the default; right-click offers Reset to Default, Copy
  Value, Paste Value.
- Every change is checked and limited as if it came from a saved level file. So an impossible value
  never ends up in the level.

> **Crazy but true:** If you drag a slider for three seconds, the value changes hundreds of times –
> and still, **one** Ctrl+Z undoes all of it. Every drag gets an ID, and all changes with the same ID
> become one step.

### Status Bar and Console

At the bottom you'll find the Content Drawer, Output Log, the console **Cmd** (↑/↓ scrolls through
history), the last message, the physics numbers in Simulate/Play (online also ping and loss), the
speaker, the level meter, the selection and the version.

| Command | Effect |
|---|---|
| `play`, `simulate`, `stop` | like the buttons |
| `spawn <id>` | place an item |
| `select <name>` | select actors whose name contains the text |
| `tod <0..1>` | time of day |
| `gravity <x y z>` | gravity (empty: 0 0 −9.80665) |
| `camspeed <1-8>`, `stat`, `clear`, `help` | speed, numbers to the log, clear the log, help |

> **Mission:** Open File › Example Levels › **Basic**, click into the **Cmd** field and type
> `spawn physics_cube`. In Details under **Transform**, set Z of **Location** to 10. Type
> `gravity 0 0 -1.6` (about the Moon's gravity; on Earth it's 9.81) and press **Alt+S**: How slowly
> does the crate fall? **Esc** stops it, then `gravity` without numbers – back on Earth.

### Context Menu

Right-click (on a touchscreen, hold for 0.5 s) selects the actor under the pointer and opens the
menu – never the browser's. With a selection: Focus, Rename, Cut, Copy, Paste, Duplicate, Delete,
**Edit Logic** or **Add Logic (Blueprint)**, **Make Player**, Snap to Floor, Group, from two actors
on **Attach to …** and **Connect with Constraint ▸**, **Detach**, Hide, Lock/Unlock, Select Children
and **Place Actor ▸** (Cube, Sphere, Physics Cube, Point Light, Orb or Empty Actor exactly where you
clicked). Without a selection: Paste, Select All, Show All, Place Actor. In Simulate/Play: Focus and
Keep Simulation Changes.

---

## 11.3 Navigating the Viewport

The controls follow Unreal and apply to the viewport under the pointer.

| Input | Effect |
|---|---|
| right mouse button + W A S D | fly (arrow keys work too) |
| … + Q / E, move the mouse | down / up, look around |
| … + Shift / mouse wheel / Z, C | 2.5 times faster / speed 1–8 / zoom |
| right + left button, middle button | move, pan |
| Alt + left / right / middle | orbit the selection / closer, farther / pan |
| Mouse wheel | forward/back; flat: zoom toward the pointer |
| F or double-click | frame the selection (F without a selection: everything) |
| Ctrl+1 … 9 / 1 … 9 | set / jump to a bookmark |

While you're flying, W, E, R, Q don't switch tools. Movements read the **position** of the key
(WASD sits in the same place on every keyboard), shortcuts like Ctrl+Z read the **label**. Speed in
m/s: level 1 = 1, 2 = 2.5, 3 = 5, **4 = 10 (default)**, 5 = 20, 6 = 40, 7 = 80, 8 = 160 – from
level 3 on, it doubles every time.

**With your fingers:** one finger orbits (flat: moves), two zoom and move, a tap selects, a double
tap frames, a long press opens the context menu, and one finger on the gizmo moves the actor.
Whatever is under your fingers stays under them while you zoom – that's why the gesture feels the
same on a sock as on a city block. Afterwards, one finger orbits around the point you zoomed to. A
finger in another viewport makes that one active. The web page itself never zooms along.

Whether Alt + left button orbits the selection is controlled by the setting **Orbit around
selection** (default: on). Without a selection, the camera orbits the point under the pointer.

> **Mission:** In Orb Run, hold the right mouse button, fly over the city and turn the mouse wheel
> up to speed 8 while you do. Let go of the mouse button and save a view of a skyscraper with
> **Ctrl+1**. Fly away, let go and press **1**. Without a keyboard: **≡ › Bookmarks**.

---

## 11.4 Keyboard Shortcuts

**F1** shows the most important ones inside Studio too. For **editing**, on top of the menu
shortcuts:

| Key | Effect |
|---|---|
| Spacebar | Move → Rotate → Scale → Move |
| [ / ] | grid smaller / larger (0.01 m to 100 m) |
| H / Shift+H / Ctrl+H | hide / hide all others / show everything |
| Esc | cancel a gizmo drag, otherwise clear the selection |
| Ctrl+Shift+Z | Redo |
| Alt+G / J / H / K | perspective / Top / Front / side |
| Alt+2 / 3 / 4 / 5 | Wireframe / Unlit / Lit / Player Collision |
| Alt + drag the gizmo | drag a copy |

Copied actors land in the real clipboard as text – you can paste them in another tab.

**In Play and Simulate:** Esc stops, F8 leaves/takes over the player, Shift+F1 frees the mouse,
Pause (or Alt+Shift+P) pauses, Alt+P ends Play, Alt+S switches to Simulate, Ctrl+R restarts (not
with the Soldier – there, Ctrl is crouch), "." goes one step while paused. In Simulate and after
Eject, **K** keeps the position of the selection, and F and G work like in the editor. If the
browser has captured the mouse, it swallows the first Esc; Studio still stops right away anyway.

**The game keys:** W A S D move, the mouse looks, the spacebar jumps (Flyer: up; down with Q),
Shift is boost. Soldier: left mouse button fires, right aims, R reloads, C crouches. F uses things,
1–7 and V control the Micro Duck.

**In the logic editor:** F7 Compile, Ctrl+F search, F9 breakpoint, Q straightens wires,
Shift+W/A/S/D aligns, B, S, D, N, O, G or L plus a click inserts Branch, Sequence, Delay, Do N, Do
Once, Gate or For Loop, C adds a comment, F/Home frames, Esc takes you back. Here, the letters
belong to the graph; only Ctrl+Z, Ctrl+Y, Ctrl+S and F1 reach the level.

---

## 11.5 Selecting, Gizmos and Snapping

Left-click selects, Shift adds, Ctrl toggles, a click into empty space clears. **Left-dragging**
always draws a selection rectangle, even on an actor – it never moves the camera or an actor. **Only
the gizmo** moves things. The rectangle takes every visible, unlocked actor whose center lies inside
it (not the random city). You select lights and empty actors via their icon (Show › Icons); locked
ones not at all. The last one selected is the **primary** selection – that's where the gizmo sits.

The **gizmo** is the colorful handle: with **Move** (W), three arrows, three planes for two axes
each, and the center; with **Rotate** (E), three rings; with **Scale** (R), three axes and a center
handle; **Select** (Q) has none. Red is X, green Y, blue Z; the handle under the pointer lights up
yellow. The globe or cube button aligns it with the world or with the primary actor. All selected
actors move; children follow their parents (if a parent and its child are both selected, the child
moves along only once). **Alt + drag** drags a copy, and the original stays put – a single undo
step. **Esc** cancels.

| Snapping | Default | Effect |
|---|---|---|
| Grid | on, 0.5 m | Move snaps into place |
| Rotation | on, 15° | Rotate in steps |
| Scale | off, 0.25 | Scale in steps |

In world axes, Move snaps to fixed positions (multiples of the grid size); in local axes, to steps.
New actors land with X and Y on the grid. If you pick a size in the toolbar, the grid switches on
along with it.

> **Picture it like this:** Snapping is graph paper in your math notebook – all the corners line up
> exactly.

> **Mission:** In the Basic level, select the cube. Press **E** and drag a ring: it rotates in 15°
> steps. **Ctrl+Z** undoes that. Press **W**, hold **Alt** and drag the red arrow 1 m – two cubes!
> Drag the copy (without Alt) 0.5 m up by the blue arrow. Repeat that with each new copy until you
> have a staircase with five steps.

---

## 11.6 Levels, Actors and Components

A level is made of World Settings, actors, textures, sounds, logic and the editor camera, measured
in meters, seconds, kilograms and degrees; rotations are stored as three angles (Roll, Pitch, Yaw),
colors as sRGB. An actor has a name, parent, folder, tags, Hidden, Locked, Transform and components.

### Creating Actors

A click in the **Content Drawer** puts an item where the camera is looking (onto an object,
otherwise the ground, otherwise 10 m ahead). **Dragging** into the viewport puts it exactly at that
spot. Or: **+ Add**, **Place Actor ▸**, `spawn <id>`. New actors get unique names ("Cube",
"Cube 2") and are selected. Something from **Players** becomes the player: "… is the player now -
press Play (Alt+P) to control it".

### Parents, Children and Folders

To **attach**, you select the parent actor **last** and choose **Attach to …**, or you drag in the
Outliner; the position stays the same. **Detach** separates them. **Ctrl+G** puts an empty actor
"Group" in the middle and attaches everything to it. You get **folders** via the Outliner folder
field, the folder button or dragging; **Select Children** selects the children.

In the simulation, a child without its own Rigid Body gets welded to its parent's body. Shapes with
collision, pickups and triggers without a body get a fixed one; with a Rotator or Mover they get a
"kinematic" one, which moves but can't be pushed around ([Chapter 6](06-physics.md)).

### Components

**+ Add** only offers what's still missing; Collider and Constraint can be added more than once,
Procedural City only to an actor without a parent.

| Component | The most important bits |
|---|---|
| Static Mesh | shape (Box, Sphere, Ramp …), collision, material, texture |
| Point Light | color, intensity, range |
| Rigid Body | Mobility (Static, Kinematic, Dynamic), mass, damping |
| Collider | shape, density, friction, sensor, collision channels |
| Constraint | joint type (Hinge, Slider, Ball …), limits, motor, spring, break force |
| Player Controller | Hover car, Rolling ball, Character, Flyer, Vehicle, Soldier, Walker; speed, camera |
| Weapon | Rifle, Pistol, Shotgun, Sniper rifle; damage, ammo, spread |
| Health | health, armor, respawn, explosion |
| Pickup | points, respawn, healing, ammo |
| Trigger | Goal, Hazard, Checkpoint, Message, Jump pad, Teleport, Damage zone, Logic event only |
| Rotator / Mover | spin speed / back-and-forth movement (Sine, Ping-pong, Loop) |
| Procedural City | seed, blocks, houses, trees, cars |
| Audio Source | sound, volume, loop, 3D, range, Doppler |

All the field names are in the blue book, and what they mean is in Chapters 6, 7 and 8.

### Joints with "Connect"

Select the actor that should **hang**, then, with Shift or Ctrl, the one it hangs **from**, last.
Right-click › **Connect with Constraint ▸** › Hinge, Slider, Ball, Fixed (weld), Distance / rope,
Wheel or Motor. Studio puts the anchor halfway between them (except for Distance and Wheel) and
gives the hanging actor a Rigid Body if it needs one. If several are hanging, each one gets its own
joint to the one selected last. A **Distance** joint keeps the distance the two have at the start.

> **Mission:** In the Basic level, drag the cube 4 m up with the blue arrow. Place a **Physics
> Sphere** and push it 3 m to the side at a height of about 2 m. Click the sphere first, then the
> cube with Ctrl, right-click the cube › **Connect with Constraint › Distance / rope**, **Alt+S**: a
> pendulum! The cube hangs firmly in the air, because without a Rigid Body it doesn't fall.

### The Player, Errors and Warnings

Play controls an actor with a **Player Controller** – chosen by placing it from **Players**, by
right-click › **Make Player (control it in Play)** or via World Settings › **Player**. If there
isn't one, Play only simulates and reports "Nothing to control yet".

Before starting, Studio checks the level. **Errors** such as a joint to a deleted actor or invalid
values prevent the start: "Fix the errors above to play". **Warnings** only show up in the Output
Log.

---

## 11.7 Undo and History

Every step saves the **whole level**. Undo simply jumps to the previous state – no backward
calculation that could go wrong, and the result never depends on timing. Studio keeps **500 steps**.
**Edit › History...** jumps to any of them with a click. One drag is one step; a change that has no
effect isn't a step at all. In Play and Simulate, the level is locked: "Stop playing to edit the
level".

> **Picture it like this:** Like save games. Instead of walking backward, you load the old save –
> guaranteed to be exactly like it was back then.

---

## 11.8 Content Drawer and Built-In Content

The Content Drawer (Ctrl+Space) has four tabs and a search field. For the console, the items are
named with their ID in parentheses.

**Players:** Soldier (`soldier`, first-person shooter, online-ready), Character (`character`, walk,
jump, climb steps), Hover Car (`hover_car`), Rolling Ball (`rolling_ball`), Drone (`drone`), Micro
Duck (`micro_duck`) and Micro Duck (Rollers) (`micro_duck_rollers`, on roller skates).

**Gameplay:** Orb (`orb`, 1 point), Star (10 points), Health Pack (heals 50), Ammo Box (60 rounds),
Goal (wins), Hazard (loses), Damage Zone (25 damage per entry), Checkpoint, Jump Pad, Teleporter (A
to B), Message Trigger (`message`), Moving Platform, Spinner, Target Dummy and Explosive Barrel.

**Shapes:** Cube (1 m, with collision), Sphere, Cylinder, Cone, Capsule, Plane (10 × 10 m), Torus,
Ramp and Empty Actor (`empty`).

**Physics:** Physics Cube, Physics Sphere, Physics Barrel, Crate Stack (10 crates), Pendulum, Hinged
Door, Chain (8 links), See-Saw and Sock (`micro_duck_sock`), a sock at the Micro Duck's scale.
**Lights:** Point Light. **Audio:** Audio Source, Ambient Wind, City Ambience, Radio. **World:**
Procedural City (`city`). Where no ID is listed, it's the name in lowercase with `_` instead of
spaces or hyphens, such as `hover_car` or `see_saw`.

> **For pros:** With a Micro Duck, Studio sets the physics to **100 Hz with 16 sub-steps**. A robot
> with servos is a long chain of joints; with too few in-between steps, it would stretch like bubble
> gum. And the duck's network needs its fixed beat.

**Textures:** **Import image** (or dragging a picture onto the page) brings in PNG, JPEG or WebP,
scaled down if it's over 2048 px. Selected shapes get the texture, and their color turns white. A
double-click puts a texture on the selection, the trash can removes it. Textures are stored right
inside the level.

**Sounds:** A click plays a preview, a double-click puts the sound on the selection (with an Audio
Source), dragging into the viewport places an Audio Source. **Import audio** takes WAV, MP3, OGG,
M4A, FLAC and more, up to 24 MB; Undo takes back an import. Every sound field in Details has a
preview button. Sound only starts after your first click – a browser rule. Without Web Audio, Studio
stays silent.

**Output Log:** the last 200 messages with the time, warnings and errors in color; in Play also game
events.

---

## 11.9 World Settings

The **World Settings** tab (narrow: **World**) applies to the whole level:

- **Game:** Level name, Game title, Player, **Observer camera**, **Win condition** (None, Collect all
  pickups, Reach a goal trigger, Survive the time limit, Reach a target score), Time limit, Target
  score and **Kill Z** – if the player falls below it, they get reset.
- **Sky and light:** Time of day (0 to just under 1), Day length (0 = stands still), sun direction,
  fog, exposure, ambient occlusion, Shadow distance (2–2000 m, default 80; smaller = sharper),
  clouds, endless ground, ground color.
- **Physics:** Gravity, Sub-steps (1–16), ground friction and bounciness, contact values, Step rate
  (15–480 Hz).
- **Audio:** master volume, music, ambience, reverb, Doppler.

---

## 11.10 The Logic Editor

Every actor and the level can carry a **logic graph**, like the Blueprints in Unreal: nodes and
wires instead of lines of code. You open it with **Logic** in the toolbar (level), **Logic** in
Details, or **Edit Logic / Add Logic (Blueprint)** in the context menu (actor). It has a toolbar
(**Viewport**, **Compile**, Save, Find, **Templates**, **C++**, help), the **My Blueprint** list
(graphs, variables with their live value, events), the graph, a Details panel and **Compiler
Results**.

An empty graph offers **Begin Play**, **On Interact**, **On Begin Overlap**, **Event Tick** and
**Templates…**:

| Template | What it does |
|---|---|
| Door that opens on Interact | the F key opens and closes it, with sound |
| Score zone (overlap +10) | entering gives 10 points, "+10", sound |
| Spawner (a cube every 3 s) | a Physics Cube every 3 s, at most 10 |
| Traffic light (state machine) | traffic light green, yellow, red |
| Robot: walk and turn away with its ToF | the Micro Duck turns away at less than 1.6 m distance |

> **Mission:** Open **Basic**, click **Logic**, then **Begin Play**. Drag a wire from the "then"
> output into empty space, type "show message", press Enter, and enter "Hello world!". Go to
> **Viewport**, place a **Rolling Ball**, press **Play**: your text is in the HUD.

When Play or Simulate starts, Kern translates every graph into WebAssembly machine code.

> **Watch out, trap:** A graph with errors does **not** stop Play. At the start, Kern just switches
> off that one graph, reports "Logic error: …" in the Output Log, and the rest keeps running. If
> "nothing" happens, look in the log – or press **F7** beforehand, and the errors show up in Compiler
> Results.

**C++** downloads the graph as readable C++ text ([Chapter 12](12-your-own-games.md)). **F9** sets
breakpoints where the game stops. Everything else: [Chapter 9](09-logic.md).

---

## 11.11 Play and Simulate

Studio knows **Editing**, **Simulating** and **Playing**. While something is running, the level is
locked; whatever happens in the simulation disappears when you stop – unless you use **Keep
Simulation Changes**.

**Play** (Alt+P) starts the game in the active viewport: the physics world is created, the camera
follows your player, and the HUD shows the title, pickups, points, time, best time and speed with a
boost bar (for the Soldier: crosshair, health, ammo). On a computer, the mouse gets captured; if
it's free, you see "Click the viewport to control the game". Winning or losing shows "Victory" or
"Game over" with **Play again**; Ctrl+R restarts, Esc ends.

**F8 (Eject)** lets go of the player: the game keeps running, you fly around freely, select things
and grab bodies (state: Simulating). Pressing **F8** again takes it over once more.

![Simulate with collision shapes shown](../buch/bilder/studio-simulate.png)

**Simulate** (Alt+S) runs physics and rules without a player, using the editor camera.

- **Grabbing:** Left mouse button or a finger on a body that can move. Dragging moves it, dragging
  upward lifts it, the mouse wheel or a second finger brings it closer or pushes it away, and
  letting go **throws** it. A short click only selects.
- If you grab a **Micro Duck by its body**, the grip holds it upright like a hand; lifted up like
  that, it pauses, just like the real robot.
- **K** copies the position of the selected actors into the level and ends the simulation. **Pause**
  and **.** go step by step.
- Show › **Collision Shapes**, **Constraints** and **Contact Points** display shapes, joints and
  touch points; the status bar counts bodies, joints and contacts.

> **For pros:** The grip isn't a teleport: a motor joint with a spring (7.5 Hz, "critically damped",
> meaning without any wobbling back and forth) pulls the point toward the pointer. That's why the
> body bumps into things and tips over like any other.

> **Mission:** Open **Physics Lab** and press **Alt+S**. Grab the top crate of the pyramid with the
> left mouse button, pull it up and drop it onto the pyramid. In the viewport's bar, turn on
> **Show › Contact Points**. Select a crate that's lying crooked with a short click and press **K**:
> Simulate ends, and the crate stays in the level lying crooked just like that. **Ctrl+Z** puts it
> back.

**Observer camera** (World Settings) starts Play with a free camera – the Micro Duck level uses
this. You fly with W A S D without holding a mouse button (on a phone, one finger looks, two zoom),
grab bodies like in Simulate, and **F8** takes over the player.

---

## 11.12 Phone, Touch and Gamepad

The engine automatically switches to the device you used last: a finger switches to touch, a key
press to computer (a mouse on a tablet keeps the touch buttons), a gamepad on a button press or a
stick moved past 0.22.

![Play on a phone in portrait mode: EXIT, PAUSE and the touch sticks](../buch/bilder/phone-play-390.png)

**Play on a touchscreen** fills the screen; at the top left are **⟵ EXIT** and **‖ PAUSE**. At the
bottom are **two sticks** (168 px, smaller if needed, down to 72 px): left moves (**MOVE**, for
vehicles **STEER / GAS**, for the Flyer **FLY**), right looks (**LOOK**); beyond the ring, the
camera keeps turning. Pushing the left stick all the way forward is sprint. Dragging on the screen
looks around; for the Soldier with a weapon, one finger fires while doing that. On top of that,
there are buttons:

| Character | Buttons |
|---|---|
| Soldier | JUMP, RELOAD, AIM, CROUCH, USE |
| Character | JUMP, USE |
| Rolling ball | JUMP, BOOST |
| Flyer | UP, DOWN, BOOST |
| Hover car, Vehicle | BOOST, HOP, USE |
| Walker (Micro Duck) | QUACK, PICK, SIT, KICK L, KICK R, ROLL, BEAK, WHEEE, HOP |

Jump, boost and weapon buttons only appear if the character can do that. When the last finger
lifts, the engine lets go of everything.

**Gamepad** (Xbox layout): left stick moves, right looks, RT fires, LT aims, A jumps, B crouches, X
reloads, Y uses, L3 or LB sprint/boost, for the Flyer RB/LB up/down, Start pauses.

---

## 11.13 Controlling the Micro Duck

The Micro Duck is the two-legged robot by Pollen Robotics, and in Studio it runs on its real,
trained networks ([Chapter 13](13-micro-duck.md)).

**The example level** is a room at its scale with furniture, a ball and two socks. The duck plays
ball on its own, like in Pollen's original software; with **Kern extras** (on here), Kern's play
layer with ball and socks takes over. The level Blueprint makes it chirp with the **Chirp** node
when its head camera sees the ball. Play starts with the Observer camera: you watch and can lift
the ball with the mouse or a finger; **F8** takes over the duck.

**Without input** (Walker › **Autonomous**), it explores the room with its distance sensor (ToF)
and camera, steers around things and gets back up by itself after a fall. When it's picked up, it
pauses ("Held"); when it's set down, it keeps walking.

| Key | Movement |
|---|---|
| W / S, A / D | walk, turn |
| Spacebar | quack |
| 1 / 2 | sit down/stand up / pick up |
| 3 / 4 | kick left / right |
| 5 / 6 | roll / beak |
| 7 | jump (Kern's `jump.onnx`) |
| V | "Wheee" ride, as long as you hold it |

On a touchscreen, **HOP** is the jump, **QUACK** quacks, **WHEEE** rides, and the rest matches 1–6.
On a gamepad, RT quacks, LT starts Wheee, B sits, Y picks up, X rolls. Logic controls it with the
nodes in the **Robots** group.

During the game, Studio shows what the duck sees at the top right (Show › **Robot Camera**): **HEAD
CAMERA** with its state, and **TOF 8×8**, the grid of its distance sensor – red is an obstacle
(brighter = closer), green is floor, yellow is too close, dark is nothing, plus the smallest
distance or "clear".

> **Mission:** Open the **Micro Duck** level, press **Play** and watch. Then press **F8** and the
> **7** key: it jumps. Walk toward a wall with **W** and watch the ToF grid at the top right: red
> squares get brighter, and the distance below shrinks.

---

## 11.14 Loading Policies

The duck's movements are **neural networks** in the ONNX format. The walking network
`velstand.onnx` is built in. Pollen's networks (sit, pick up, kick, roll, roller skates) come from
the `microduck/` folder, Kern's own from `policies/`: `jump.onnx` for the jump and `play.onnx` for
the play layer with ball and sock (only with Kern extras). The engine reports what's missing, the
page fetches it, the engine checks it. If a file fails, the page tries again after 5, 10, 20 s …,
at most once a minute; until then, the duck stands still, and Studio tells you why.

> **Crazy but true:** Whatever you train in the Kern Lehrer ([Chapter 14](14-lehrer.md)), Studio
> loads instantly in the same browser – without copying a single file.

If you publish an exported game with the duck, you put `microduck/` and `policies/` next to the
HTML file, along with Pollen's license and NOTICE. The duck's appearance is only allowed for
**non-commercial** use ([Chapter 12](12-your-own-games.md)).

---

## 11.15 Saving, Loading, Autosave and Settings

A level is a JSON text file `.klevel` (`"format": "kern-level"`, `"version": 1`) with name, world,
camera, actors, textures, sounds and logic – pictures and sounds are packed right in. Studio refuses
files from a newer Studio version. [Chapter 8](08-game-runtime.md) describes the format.

**Save** (Ctrl+S), in Chrome and Edge, writes straight into the same file after the first dialog;
otherwise it saves as a download. **Save As** always asks, **Open** (Ctrl+O) opens `.klevel` and
`.json`. Dragged onto the page, levels get opened, pictures become textures and audio becomes
sounds. The file name is the level name; everything except letters, digits, `-` and `_` becomes
`_`. On Android, the dialog doesn't filter (it doesn't know `.klevel`); if it isn't a level, Studio
reports "… is not a Kern level".

**Autosave:** At most 2 s after a change, a copy lands in the browser – only as long as something is
unsaved, and never more than one. Saving deletes it, and so does undoing back to the saved state.
When you leave, when the page goes into the background and before updates, it's written right away;
in Play and Simulate only after Stop. If there are unsaved changes, the browser asks before you
leave. On the next start: **Restore** (loads the copy, but it stays unsaved and is offered again on
every start until you save) or **Dismiss** (deletes it). Separately, Studio remembers the level you
last edited.

> **Watch out, trap:** The autosave lives **only in this browser**. New computer or cleared browser
> data – gone. Only **Ctrl+S** keeps your work truly safe.

**Settings:** Studio remembers **only what you changed yourself**; everything else follows the
defaults of the current version. **Edit › Preferences** offers Camera speed (1–8, default 4), Mouse
sensitivity (0.1–5, default 1), Invert mouse Y, Orbit around selection and the three snaps. **Reset
Settings to Defaults** resets everything; levels stay untouched.

---

## 11.16 Exporting a Game

**File › Package Game (HTML)...** or the package button: check the **Game title** (filled in from
the World Settings), click **Package**. Studio downloads `<title>.html` and reports "Packaged …
(n MB)" with **Play it**, which starts the file in a new tab.

The file is a **complete game** with the engine, game host and level. It needs neither a server nor
the internet and even runs with a double-click – straight into Play, without the editor. It contains
Pollen's 3D models of the duck (CC BY-NC-SA 4.0, non-commercial), unless the engine was built as
commercial.

The controls are the same as in Play; Esc frees the mouse and pauses, P pauses, R or Enter after the
end and Ctrl+R restart, and F8 takes over the player with the Observer camera. Touchscreens get
sticks and a ↻ at the top left; on a computer, there's a hint with the keys at the bottom. Online
works with `?host=room`, `?join=room` and `&relay=` ([Chapter 10](10-networking.md)). The duck's
loadable networks are **not** included: walking works, but for everything else, `microduck/` and
`policies/` belong next to the file.

> **Mission:** Give your staircase level from 11.5 a Rolling Ball as the player and export it.
> Click **Play it** – and send the file to a friend.

---

## 11.17 Multiplayer

**Play › Number of Players › 2 Players (listen server)**, then **Play**: if only one viewport was
open, Studio switches to four – 1 is the server, 2 the first client, each with its own HUD. Only the
active viewport can be controlled. **Network Emulation** makes the connection worse, and the status
bar shows role, players, ping, loss and data rate. You'll find a picture in
[Chapter 10](10-networking.md).

**Online:** Host and guest open the same level, choose **Host Online Game...** or **Join Online
Game...**, the same **Room** (default "kern") and, for different computers, the same **Relay**
(`node tools/net_relay.mjs`, port 8790, package `ws`). Without a relay, tabs of the same browser
find each other; if the host is missing, Join reports that after 4 s. **Leave Online Game** leaves
the game; the host notices when a guest leaves. A host in a background tab keeps running so its
guests don't get kicked out. Logic only runs on the host; its changes reach everyone, latecomers
too.

---

## 11.18 Narrow Screens and Phones

Up to 900 px wide, or on touch devices with a height of at most 600 px, Studio goes compact:

- The viewport gets the whole area. Outliner, Details and World open via **Panels** as a sheet with
  big tabs – on the right in landscape, from the bottom in portrait (64% of the height).
- **Play and Simulate** sit together with Panels in a **dock** on the right of the toolbar, which
  always stays in place while the rest scrolls sideways. During the game, Pause, Stop and Eject are
  there; all buttons are at least 38 px in size.
- The snap selection, the axes button, the console and the physics numbers disappear. The Content
  Drawer takes up just under half the height at most; the page never scrolls sideways.

![Studio in an 820 px wide window](../buch/bilder/studio-narrow.png)

![Phone in portrait mode: the dock with Play, Simulate and Panels](../buch/bilder/phone-dock-390.png)

---

## 11.19 Architecture: How Studio Is Built

Studio is a C++ class, `kern::studio::Studio`, that owns everything: level, history, selection,
viewports, menus, Play and network games. The web page contains **no** editor logic. **Why?** First,
the editor runs exactly the code of your exported game – there's no second version that could
drift apart. Second, it can be tested without a browser.

The page talks to Studio through C functions that start with `kern_studio_`. And: **Everything you
do is a command** – a JSON object with `"cmd"`, every menu entry included.

```json
{"cmd": "spawn", "item": "physics_cube"}
{"cmd": "set", "path": "transform.position",
 "value": [0, 0, 2]}
```

The first line places a physics crate. The second one (split over two lines) sets the position of
the selected actors to x = 0, y = 0, z = 2: `path` follows their text form, `value` is the new
value. There are commands for selection, actors, properties, history, Play, network, views, panels,
documents, sound, settings and logic – all of them in the blue book and in
[Chapter 17](17-reference.md).

> **Picture it like this:** Studio is the console, the web page is the controller. The controller
> just presses buttons and shows what the console says.

The page `studio.mjs` has the engine calculate every frame, handles its requests (capturing the
mouse, dialogs, files) and only redraws panels when something has changed. It only does what a
browser can do and C++ can't. `game.mjs` is also the host of every exported game. Through
`window.KernStudio`, a browser test operates Studio automatically; most of the Studio pictures in
this book come from it. The C++ tests (`kern/tests/run.sh studio`) check navigation, gizmos,
commands, Play, saving and settings, even completely without a browser.

---

## Level complete!

- You know all the areas of Studio, the menus and the most important keys.
- You fly through viewports and move actors with the gizmo and the grid.
- You build actors from components and connect them with joints.
- You test with Play, Simulate, Eject and Keep, and you know where errors show up.
- You know that a broken logic graph only switches itself off.
- You control the Micro Duck, save, export and play with a friend.
- You know why the editor is C++ and every action is a command.

## Puzzle corner

1. Which key selects the rotate tool?
2. You drag across a cube with the left mouse button. Does it move?
3. Play without an actor with a Player Controller – what happens?
4. Your logic graph has an error. Does Play start? Where's the message?
5. **Boss puzzle:** Grid snap 0.5 m, a cube at x = 0. You hold Alt and, in world axes, drag the red
   arrow 1.3 m in the direction of the arrow. How many cubes are there, at which x does the moved
   one land, and how many times do you have to press Ctrl+Z?

### Solutions

1. **E** (Rotate).
2. **No** – left-dragging draws a selection rectangle. Only the gizmo moves things.
3. Studio **simulates** instead and reports "Nothing to control yet".
4. **Yes.** Only that graph gets switched off; "Logic error: …" is in the **Output Log**.
5. **Two** cubes, and the original stays at 0. The copy snaps to the nearest multiple of 0.5 m:
   1.3 is closer to 1.5 than to 1.0, so **x = 1.5 m**. **One** Ctrl+Z is enough – copy and move are
   one step.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
