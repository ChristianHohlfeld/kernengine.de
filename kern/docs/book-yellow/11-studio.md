# 11 Kern Studio: Your Workshop in the Browser

> **Your level:** Time to build! Kern Studio is the editor where you put worlds together, try out
> physics, test games, and send them off as a file of their own. You won't learn every key here,
> but the ideas an editor works by – best with Studio open next to you.

> **Words you need:**
> - **Editor** – a program for building and editing, here: game worlds.
> - **Level** – a whole game world, saved as one file.
> - **Actor** – a single thing in the level: a cube, a lamp, a car, the player character.
> - **Component** – a building block that an actor carries and that gives it an ability.
> - **Viewport** – a window through which you look into the 3D world with a camera.
> - **Rigid Body** – the component that turns a thing into a real physics body: it falls and can
>   be pushed.
> - **HUD** – the display on top of the game picture: points, time, messages.
> - **WebAssembly** – a fast machine language that every browser can run. Kern runs in it.

Kern Studio is built like the big professional editors, for example the Unreal Engine. The most
important idea: **What you see in the editor already is the game.** The whole editor uses exactly
the same program as your finished, exported game. Whatever falls, rolls and glows in the viewport
will do exactly the same later (11.16). How that becomes a whole game is shown in
[Chapter 12](12-your-own-games.md).

This chapter goes in the order in which you learn an editor: first understand what a world is made
of, then look, select and move, then undo mistakes, test, add rules and logic, and finally save
and share.

Studio shows its interface in English or German; without a setting, it follows the browser's
language, so English browsers get English labels. This book uses the English labels. The
**DE | EN** button bottom right in the status bar switches the language; the page reloads, and your
level is kept.

> **For pros:** You can also set the language in the address: add `?lang=en` or `?lang=de` at the
> end. Studio remembers it on this device.

---

## 11.1 Starting Studio

Studio runs in the browser, on phones and tablets too. You open it at https://kernengine.de/. On
the very first start, **Orb Run** is loaded: a random city with traffic, a hover car, 24 energy
orbs and a day that turns into night. After that, Studio always opens the level you worked on last
and offers **Start fresh** for a fresh start with Orb Run. If there were unsaved changes, it asks
whether to restore them (11.13).

> **Mission:** Open Kern Studio, wait for Orb Run and press **Alt+P**. Drive to three orbs with
> W A S D. **Esc** takes you back to the editor. You just played inside the editor – without
> exporting anything.

**As an app:** In Chrome and Edge the title bar shows **⤓ Install**; on iPhone and iPad the same
button explains how to put Studio on the home screen. Once installed, Studio starts full screen,
works offline too, and tells you when a new version is available.

> **For pros:** Studio also exists as a single HTML file that runs with no server at all, and as a
> developer version for working on Studio itself. How to build both is in the blue book,
> Chapter 11.

---

## 11.2 The Workshop at a Glance

Every big editor divides the screen into fixed areas so you always know where things are: the
world in the middle, the tools and lists around the edges. Like a workbench with a tool wall.

![Selected actor with the move gizmo and its components in Details](../buch/bilder/studio-selected.png)

| Area | Where | What for |
|---|---|---|
| Title bar | top | menus, level name (dot = unsaved) |
| Toolbar | below it | buttons for tools, grid, play, logic, export |
| Viewports | middle | 1, 2 or 4 views of the world |
| Outliner | top right | list of all actors |
| Details / **World Settings** | bottom right | properties of the selection or of the world |
| **Content Drawer** | bottom, foldable | building blocks to place, images, sounds, log |
| Status bar | very bottom | console, messages, numbers, version |

You can drag the dividing lines; Studio remembers the sizes.

**The menus** are sorted like in many programs: **File** for creating, opening, saving, exporting
and the example levels; **Edit** for undoing, copying, deleting and the preferences; **Window** for
views and what's shown; **Play** for playing, simulating and multiplayer; plus **Help**. Anything
that can't do something right now is grayed out. Many entries show their keyboard shortcut next to
them. You'll find the most important keys in [Chapter 17](17-reference.md), and all menu entries
in the blue book, Chapter 11. **F1** shows the most important keys right inside Studio too.

**File › New Level...** offers six templates (a double-click creates one right away):

| Template | What's in it |
|---|---|
| Orb Run | night city, hover car, 24 orbs |
| Physics Lab | all kinds of joints: hinge, rail, wheel, rope bridge, motor, crate pyramid |
| Shooting Range | soldier, targets, cover, multiplayer-ready |
| Micro Duck | the walking robot by Pollen Robotics |
| **Basic** | ground, a cube, sun and sky |
| **Empty** | nothing at all |

You'll find the same levels under **File › Example Levels**. For the missions in this chapter,
**Basic** is your practice ground.

---

## 11.3 A Level Is a Building Kit: Actors and Components

The most important principle of the editor: a world is made of **actors**, and every actor is
plugged together from **components**. A component gives it exactly one ability: "looks like a
cube," "falls down," "glows," "gives points," "plays music." What a thing **can do** depends only
on which components it carries.

> **Picture it like this:** a building kit. An actor is a base plate. Plug wheels onto it and it
> drives; plug a lamp onto it and it glows. The same bricks fit on every plate.

That's why Kern doesn't need its own kind of "crate," "door" or "car." A crate is an actor with a
shape and a physics body, a door also has a joint, an orb has a glowing sphere shape and "gives
points."

| Component | What for? |
|---|---|
| **Static Mesh** | the shape and the look: cube, sphere, ramp … |
| **Point Light** | a lamp that shines in all directions |
| **Rigid Body** | turns a thing into a physics body: it falls and pushes |
| **Collider** | the invisible shell where things bump into each other |
| **Constraint** | a joint to another thing: hinge, rope, motor |
| **Player Controller** | turns the thing into the character you control |
| **Weapon**, **Health** | weapon and health points for the soldier |
| **Pickup** | something to collect that gives points |
| **Trigger** | a zone that sets something off: goal, hazard, jump pad, teleporter … |
| **Rotator**, **Mover** | spins by itself or moves back and forth |
| **Procedural City** | builds a whole random city |
| **Audio Source** | plays a sound in space |

What each field does exactly is explained in [Chapter 6](06-physics.md) (physics),
[Chapter 7](07-audio.md) (sound) and [Chapter 8](08-game-runtime.md) (game rules). All field names
are in the blue book, Chapter 11.

**What else belongs to the level:** the **World Settings** with the rules for the whole world
(11.9), images, sounds, logic and the editor's camera position. Things are measured in meters,
seconds and kilograms, and **Z points up**. A rotation is given as three angles, like for an
airplane: roll (tipping sideways), pitch (nose up or down) and yaw (turning left or right).

### Where the Building Blocks Come From

Ready-made actors are in the **Content Drawer** (Ctrl+Space), sorted into groups like a toolbox
with compartments: **Players** with player characters like the **Rolling Ball**, **Gameplay** with
game elements such as an orb, **Shapes** such as a cube, **Physics** with physics things such as a
physics crate, **Lights** with lamps, **Audio** with sounds, and **World** with the random city.

- A **click** puts the building block where the camera is looking.
- **Dragging** it into the viewport puts it exactly where you let go.
- Or: **+ Add** in the toolbar, or right-click › **Place Actor**.

A new actor is selected right away. Anything from **Players** automatically becomes your player
character.

You bring in your own images and sounds through the **Textures** and **Sounds** tabs in the
Content Drawer (more on that in 11.13). The **Output Log** tab is the logbook with all of Studio's
messages.

### Outliner and Details: List and Profile

The **Outliner** is the list of all actors in your world, with a search field, folders and an eye
icon for hiding things. A click selects, a double-click brings the actor into view, F2 renames.
The player character wears the tag **PLAYER**.

**Details** is the profile of the selected actor: at the top its name and placement (**Transform**
with **Location**, **Rotation** and **Scale**, each three numbers X, Y, Z like in a coordinate
system), and below that a section for each component. With **+ Add** you plug in a component; the
trash can removes it. You can type numbers (even calculations like `2*3`), hold the mouse button
on the field name and drag left or right, or change them with the arrow keys.

Studio checks every value before it goes into the level. A size of 0 or a value that doesn't fit
never ends up in the level – that way a typo can't break your world.

> **Mission:** Open **File › Example Levels › Basic**. Click the cube "Cube": Details shows a
> **Static Mesh**, but no Rigid Body. From the Content Drawer (group **Physics**), place a
> **Physics Cube** by dragging it into the viewport next to the cube: it also has a **Rigid
> Body**. For both, set **Location** Z under **Transform** to 5 and press **Alt+S**. What falls,
> what floats? **Esc** stops – and both are back up top. You'll find out why in 11.8.

The cube without a Rigid Body is still there for the physics. If a thing has a shape with
collision but no Rigid Body, Studio gives it an invisible **fixed** body while testing: it doesn't
fall, but other things bump into it, like into a wall. That's why the physics crate can land on the
cube and your player character can run into it.

---

## 11.4 Looking into the World: Viewports and Camera

A **viewport** is a window with its own camera. Up to four at a time show the same world from
different sides. A click makes a viewport **active**; that's where your keys go.

There are two kinds of view:

- **Perspective**: like with your eyes. Things farther away look smaller.
- **Orthographic** (in the menu **Top**, **Front**, **Left** …): flat, without perspective, like a
  blueprint. Things of the same size are the same size everywhere. That's how you line things up
  exactly.

Bottom left, an **axis cross** shows the directions: X red, Y green, Z blue – and Z points up.

Each viewport has a bar at the top. In the **View Mode** you choose how the world is drawn:
**Lit** with light and shadows, **Unlit** with colors only, no light, **Wireframe** just as a wire
mesh of triangles, or **Player Collision**: darkened, with the shells things bump into drawn on
top. Under **Show** you switch helpers on and off, such as the ground grid, icons for lamps, or
the physics shells. **G** hides all helpers at once and shows the world like in the game.

### Flying Like in a Game

You steer the camera like a player character:

| Input | Effect |
|---|---|
| hold right mouse button + W A S D | fly, mouse looks around |
| … + Q / E | down / up |
| … + mouse wheel | faster / slower (level 1–8) |
| Alt + left mouse button | orbit around the selection |
| F | bring the selection into view |
| Ctrl+1 … 9 / 1 … 9 | set / jump to bookmark |

**With fingers:** one finger orbits, two fingers zoom and pan, a tap selects. Whatever is under
your fingers stays under them while you zoom.

> **For pros:** How fast you zoom depends on how far away the point under your fingers is: close
> things zoom in small steps, distant things in big ones. And for W A S D, what counts is the
> **position** of the key, not what's printed on it – on a French keyboard, the four keys in the
> same spot steer just the same. Shortcuts like Ctrl+Z, on the other hand, go by the label.

> **Mission:** In Orb Run, hold the right mouse button, fly over the city and turn the mouse wheel
> until you're really fast. Let go of the mouse button and save a view of a tall building with
> **Ctrl+1**. Fly away, let go and press **1** – you're back instantly. Without a keyboard, or if
> your browser jumps to another tab on Ctrl+1: **≡ › Bookmarks** in the viewport's bar, then
> **Set bookmark 1** and later **Jump to bookmark 1**.

All other camera keys are in [Chapter 17](17-reference.md).

---

## 11.5 Selecting and Moving: Gizmo and Grid

Before you change something, you tell Studio **what**: you select. A left click selects, Shift
adds, Ctrl toggles single items on and off, a click into empty space clears everything. **Left
dragging** always draws a selection rectangle, even if you start on an actor. It never moves
anything.

Things are moved **only with the gizmo**, the colorful handle on the selected actor. This
separation is on purpose: a click to select can never accidentally move something.

| Tool (with key) | Gizmo |
|---|---|
| **Select (Q)** | none, just select |
| **Move (W)** | three arrows, dragging moves |
| **Rotate (E)** | three rings, dragging rotates |
| **Scale (R)** | three axes, dragging enlarges |

Red is X, green Y, blue Z; the handle under the pointer lights up yellow. While dragging, a label
next to it shows how far you're moving (in m), rotating (in °) or enlarging. If you've selected
several actors, they all move; the gizmo sits on the one selected last.

**World or own axes?** A button in the toolbar switches between two pictures. If it shows a globe
(tooltip **World coordinates**), it lines up the gizmo with the **world**: the red arrow always
points the same way, like "north" on a map. If it shows a cube (**Local coordinates**), it lines
the gizmo up with the **actor itself**: if the actor is rotated, the gizmo rotates with it, like
"in front of me" for a person who turns around.

**Alt + drag** pulls out a **copy**; the original stays put, and afterward the copy is selected.
**Esc** while dragging cancels. If you copy actors with Ctrl+C, they land in the clipboard as text
– so you can even paste them with Ctrl+V into a different Studio tab.

### Snapping: Graph Paper for the World

> **Picture it like this:** Snapping is the graph paper in your math notebook. When every corner
> sits on a square, everything fits together exactly.

With **snapping**, a value always jumps to the **nearest multiple** of a step size:

| Grid | Default | Effect |
|---|---|---|
| **Grid snap** | on, 0.5 m | moving snaps |
| **Rotation snap** | on, 15° | rotating in 15° steps |
| **Scale snap** | off, 0.25 | enlarging in steps |

**Example:** Grid 0.5 m. You drag a cube to x = 1.3 m. The nearby multiples are 1.0 and 1.5. 1.5
is closer, so it lands at **x = 1.5 m**.

In world axes, the actor snaps to these fixed positions. In its own axes, it moves in steps of
0.5 m, starting from where it was. New actors also land on the grid. You choose the step sizes in
the toolbar.

> **Mission:** In the Basic level, select the cube "Cube." Press **E** and drag a ring: it rotates
> in 15° steps. **Ctrl+Z** undoes that. Press **W**, hold **Alt** and drag the red arrow 1 m – two
> cubes! The copy is now selected. Drag it (without Alt) 0.5 m up by the blue arrow. Repeat this
> with each new copy until you have a staircase with five steps.

---

## 11.6 Undo: Back to the Save Point

Mistakes are part of building. That's why **Ctrl+Z** can take back any step and **Ctrl+Y** can
redo it. The exciting part is **how** Studio does it.

> **Picture it like this:** Like save points in a game. Instead of painfully walking backward, you
> just load the old save – and it's guaranteed to be exactly the way it was.

After every step, Studio saves a copy of the **whole level**. Undo simply loads the previous copy.
So Studio never has to work out how to reverse a change – and that's why nothing can go wrong
there either. It keeps **500 steps**.

- A whole drag is **one** step, even if the value changes hundreds of times along the way.
- An Alt copy including the move is also **one** step.
- A change that changes nothing is **no** step.
- **Edit › History...** shows all steps as a list. A click jumps to that state.

> **Crazy but true:** If you drag a slider for three seconds, the value changes hundreds of times –
> and still **one** Ctrl+Z takes it all back. Each drag gets an ID, and all changes with the same
> ID become one step.

> **Mission:** After your staircase, open **Edit › History...**. Click the step where you dragged
> out the first copy: the staircase shrinks to two cubes. Then click the last step – all five steps
> are back. Now save the staircase with **Ctrl+S**, for example as `stairs.klevel` (more on saving
> in 11.13). You'll need it again in 11.14.

---

## 11.7 Connecting Things: Parents, Children and Joints

Some things belong together. Studio knows two ways to connect them.

**Parents and children.** If you attach one actor to another, it becomes its **child**. When the
**parent** moves, the child moves along – like a cup in your hand: when you walk, the cup goes
with you. To attach, you select the future parent **last** and use right-click › **Attach to …**,
or you drag one actor onto another in the Outliner. **Detach** releases it again. **Ctrl+G** puts
an empty actor in the middle of the selection and attaches everything to it. Folders in the
Outliner, on the other hand, are only for tidying up; they don't move anything.

> **For pros:** If a child has no **Rigid Body** of its own, the physics welds it to its parent's
> body. Both then move like a single piece ([Chapter 6](06-physics.md)).

**Joints.** A joint connects two things so they can **move**: a hinge like on a door, a rail, a
ball joint like your shoulder, a fixed connection, a rope, a wheel or a motor. This is the fastest
way:

1. Select the actor that should **hang**.
2. With Shift or Ctrl, add the one it hangs **from** – last.
3. Right-click › **Connect with Constraint** and pick a joint type there.

Studio gives the hanging actor a Rigid Body if it doesn't have one. A rope (**Distance / rope**)
keeps exactly the distance the two have at the start.

> **Mission:** Reopen the Basic level so there's only one cube again:
> **File › Example Levels › Basic**. Drag the cube 4 m up with the blue arrow. Place a **Physics
> Sphere** and push it 3 m to the side at a height of about 2 m. Click the sphere first, then the
> cube with Ctrl, right-click the cube › **Connect with Constraint › Distance / rope**, **Alt+S**:
> a pendulum! The cube floats in place: without a Rigid Body it gets an invisible fixed body
> (11.3) that doesn't fall and holds the rope.

---

## 11.8 Testing: Play and Simulate

An editor has to show whether the world works. Studio has three states for this:

- *Building*: you build. Nothing moves by itself.
- *Simulate*: physics and game rules run, you watch with the editor camera and can reach in.
- *Play*: you play with your player character.

The most important principle: **Testing doesn't change your level.** While something is running,
the level is locked. Whatever happens in the simulation disappears when you stop, and everything is
back where you built it. Like a trial run with toys that jump back to their starting positions by
themselves afterward. That's why both cubes were back up top after **Esc** in the mission from
11.3.

**Play** with Alt+P starts the game in the active viewport: the camera follows your player
character, and the HUD shows points, time and speed. On a computer, Studio captures the mouse;
**Esc** ends, **Ctrl+R** restarts. On a win or a loss, "Victory" or "Game over" appears with
**Play again**.

To play, the level needs an actor with a **Player Controller**. You get one when you place
something from the **Players** group or add the component in the Details panel. If several actors
have one, you choose which one you control with right-click › **Make Player (control it in
Play)**. If there's none, Studio only simulates and reports "Nothing to control yet." Studio also
checks the level before starting: errors (such as a joint to a deleted actor) prevent the start;
warnings only appear in the **Output Log**.

**F8** (eject) lets go of the player character: the game keeps running, and you fly around freely
like when building. Pressing **F8** again takes over the character again.

![Simulate in Orb Run with collision shapes shown (Show › Collision Shapes)](../buch/bilder/studio-simulate.png)

**Simulate** with Alt+S runs the world without a player. Now you can **grab** things: left mouse
button on a physics body, dragging moves it, dragging upward lifts it, letting go **throws** it.
Under **Show** you make visible what the physics sees: **Collision Shapes**, meaning the shells,
**Constraints** and **Contact Points**, meaning where things touch. **Pause** stops, and the
period key then goes one step further.

Sometimes you like what the simulation did – a crate lying at just the right angle. Then you select
it and press **K**, short for **Keep Simulation Changes**: its position goes into the level, and
the simulation ends. That's a completely normal undo step.

> **For pros:** The grab isn't a teleport. A motor joint with a spring pulls the grabbed point
> toward the pointer, "critically damped," meaning without swinging back and forth. That's why the
> body bumps into things and tips over like any other.

> **Mission:** Open **Physics Lab** and press **Alt+S**. Grab the top crate of the pyramid with the
> left mouse button, pull it up and drop it onto the pyramid. In the viewport's bar, switch on
> **Show › Contact Points**. Select a crate lying at an angle with a short click and press **K**:
> simulating ends, and the crate stays tilted like that in the level. **Ctrl+Z** puts it back.

With **Observer camera** in the **World Settings**, **Play** starts with a free camera: the game
runs, you watch and can grab things like when simulating. **F8** takes over the player character.
The Micro Duck level uses this.

---

## 11.9 The Rules of the World: World Settings and Console

Some settings don't belong to one thing but to the **whole world**. They're in the **World
Settings** tab:

- **Game**: How do you win? Collect everything, reach a goal, survive for a time, or reach a score.
  Plus time limit, player character and **Kill Z**: the height below which the character counts as
  having fallen off and gets reset.
- **Sky and light**: time of day, how fast the day passes, sun, fog, clouds, ground.
- **Physics**: gravity and how precisely the physics calculates ([Chapter 6](06-physics.md)).
- **Audio**: master volume, music, ambient sound, reverb.

The exact fields and ranges are in the blue book, Chapter 11.

**The console.** Bottom left in the status bar is the **Cmd** field (labeled the same in the German
interface). There you type commands instead of clicking. The up arrow brings back the last command.

| Command | Effect |
|---|---|
| `spawn` and a name | place a building block |
| `gravity x y z` | change gravity (empty: Earth) |
| `tod` and a number from 0 to 1 | time of day |
| `play`, `simulate`, `stop` | like the buttons |
| `help` | show all commands |

Gravity has a strength **and a direction**. That's why `gravity` needs three numbers: how strongly
it pulls in the X, Y and Z directions. Z points up, so `gravity 0 0 -9.81` means: 9.81 downward,
like on Earth. A plus at Z would make everything fall to the ceiling.

With `tod` ("time of day"), the day is a circle from 0 to 1: **0** is midnight, **0.25** sunrise,
**0.5** noon, **0.75** sunset. In the command you write the decimal with a point, so `tod 0.5`.

> **Mission:** Open **File › Example Levels › Basic**, click into the **Cmd** field and type
> `spawn physics_cube`. In Details, under **Transform**, set Z of **Location** to 10. Type
> `gravity 0 0 -1.6` (roughly the Moon's gravity pulling down; on Earth it's 9.81) and press
> **Alt+S**: how slowly does the crate fall? **Esc** stops, then `gravity` without numbers – back
> on Earth.

---

## 11.10 Logic Without Code: the Logic Editor

For something to **happen** in your world – a door opens, a zone gives points – it needs logic.
Studio has its own editor for that, in which you connect **nodes** with **wires** instead of
writing code.

> **Picture it like this:** Like Scratch, except the blocks aren't stacked under each other but
> connected with wires. An event node like "Begin Play" (the game starts) is the "when green flag
> clicked" block. The wire says what happens next.

Every actor and the level itself can carry such a **logic graph**. You open it with **Logic** in
the toolbar (for the level), with **Logic** in Details, or with right-click › **Edit Logic** for an
actor; if it doesn't have a graph yet, the entry is called **Add Logic (Blueprint)**.

**Important:** The logic editor isn't translated. Its buttons, nodes and templates are in English
even in the German interface – just like the messages in the **Output Log** and the console
commands. So if you switch Studio to German, you'll see the two languages mixed from here on. An
empty graph offers start nodes: **Begin Play**, **On Interact** (the character uses the actor),
**On Begin Overlap** (something enters it) and **Event Tick** (every frame). Under **Templates…**
there are ready-made examples:

| Template | What it does |
|---|---|
| Door that opens on Interact | key F opens and closes, with sound |
| Score zone (overlap +10) | entering gives 10 points |
| Spawner (a cube every 3 s) | a physics crate every 3 s, at most 10 |
| Traffic light (state machine) | traffic light green, yellow, red |
| Robot: walk and turn away with its ToF | the Micro Duck turns away from obstacles |

> **Mission:** Open the **Basic** level, click **Logic** in the toolbar, then **Begin Play**. On
> the right, the node has an output called "then," a small arrow. Drag a wire from there into
> empty space, type "show message," press Enter and enter "Hello world!" Go back with
> **Viewport**, place a **Rolling Ball**, press **Play**: your text is in the HUD.

When the game starts, Kern translates every graph into fast machine language. **Compile** means
"translate": the **Compile** button (F7) checks beforehand whether your graph has errors. The
errors then appear in the **Compiler Results** field below the graph.

> **Watch out, trap:** A graph with errors does **not** stop the game. At the start, Kern just
> switches off that one graph, reports "Logic error: …" in the **Output Log**, and the rest keeps
> running. If "nothing" happens, look in the log – or press **F7** beforehand, and the errors will
> be in the Compiler Results.

All nodes, variables and traffic-light circuits are explained in [Chapter 9](09-logic.md). How to
download a graph as C++ is shown in [Chapter 12](12-your-own-games.md).

---

## 11.11 One Studio for Every Device

Studio and your games run on computers, tablets and phones. The principle: **The engine notices by
itself what you're playing with right now**, and switches to the device you used last. A finger
switches to touch, a key press to keyboard, a gamepad button to gamepad. A mouse on a tablet keeps
the touch buttons.

![Playing on a phone in portrait: ⟵ EXIT, ❚❚ PAUSE and the touch sticks](../buch/bilder/phone-play-390.png)

**On a touchscreen**, the game fills the whole screen. Top left are **⟵ EXIT** and **❚❚ PAUSE**.
At the bottom are **two sticks**: left to move, right to look. Added to that are only the buttons
your player character really needs – a Rolling Ball only gets **JUMP** and **BOOST**, for example,
while a soldier also gets buttons for aiming, reloading and crouching. When the last finger lifts,
the engine lets go of everything so the character doesn't keep running. Which character gets which
buttons is in the blue book, Chapter 11.

**On a gamepad** (Xbox layout), the left stick moves, the right one looks, A jumps. The full layout
is in the blue book, Chapter 11.

**On narrow screens**, Studio itself becomes compact: the viewport gets the whole area. Outliner,
Details and World Settings open as a sheet through the panel button (tooltip **Outliner and
Details**), from the bottom on phones. The buttons for playing and simulating sit in a **dock** on
the right of the toolbar, which always stays put while the rest of the toolbar scrolls sideways.

![Studio in an 820 px wide window](../buch/bilder/studio-narrow.png)

![Phone in portrait: the dock with Play, Simulate and the panel button](../buch/bilder/phone-dock-390.png)

---

## 11.12 The Micro Duck in Studio

The Micro Duck is the two-legged robot by Pollen Robotics. In Studio it runs with the same learned
programs as the real robot. How that works is explained in [Chapter 13](13-micro-duck.md); here
it's only about how to operate it.

**The Micro Duck example level** is a room at its scale with furniture, a ball and two socks.
**Play** starts with the **Observer camera**: you watch the duck walk around on its own and play
with the ball, and you can lift things with the mouse or a finger. **F8** takes over the duck.

**Without input**, it explores the room with its distance sensor and its camera, steers around
things, and gets back up by itself after a fall. If you lift it up, it keeps still; if you set it
down, it walks on – just like the real robot.

**Steering it yourself:** W and S walk, A and D turn, Space quacks, **7** jumps, and the keys 1 to
6 trigger more movements like sitting or kicking. The full list is in
[Chapter 17](17-reference.md). Top right, Studio shows what the duck sees: its head camera and the
grid of its distance sensor ([Chapter 13](13-micro-duck.md)).

Each of the duck's movements is a learned program of its own, a **network**. Walking is built in;
Studio fetches the other networks from the internet. If one is missing, the duck stands still, and
Studio says why. Networks you train yourself in the Kern Lehrer are loaded by Studio right away
([Chapter 13](13-micro-duck.md) and [Chapter 14](14-lehrer.md)).

> **Mission:** Open the **Micro Duck** level, press **Play** and watch for a while. Then **F8** and
> key **7**: it jumps. Then let go of all keys and wait a few seconds: your input only has priority
> for a short time; after that, the duck decides on its own again. What does it do next?

---

## 11.13 Saving: File, Autosave and Preferences

A level is **one single file** ending in `.klevel`. Everything is in it: world, actors, logic, and
even your images and sounds. So you can send it to someone, and nothing is lost on their end. The
exact format of the file is in [Chapter 17](17-reference.md).

**Save** with Ctrl+S saves. In Chrome and Edge, after the first dialog Studio writes straight into
the same file; in other browsers you get a download. **Save As...** always asks for a name, and
**Open Level...** with Ctrl+O opens a level. If you drag a file onto the page, a level gets opened,
an image becomes a texture, and a sound becomes a sound.

**Autosave:** As long as something is unsaved, Studio puts a copy in the browser shortly after
every change. On the next start it asks: **Restore** brings the copy back, **Dismiss** deletes it.
As soon as you save properly, the copy isn't needed anymore and disappears.

> **Watch out, trap:** The autosave lives **only in this browser**. New computer or deleted browser
> data – gone. Only **Ctrl+S** gives you real safety.

**Updates never cost you work.** When there's a new Studio version, Studio loads it right away
while you're building. If a game is running, **New version available** appears, and you choose
**Update** or **Later**; at the latest when you stop, Studio updates by itself. During the update,
your level – including unsaved changes – travels into the new version via the autosave – like a
move where the boxes are safely in the car first.

**Preferences:** Under **Edit › Preferences...** you set camera speed, mouse sensitivity and the
grids. Studio only remembers **what you changed yourself**. Everything else follows the default
values of the current Studio version. If a default gets better in a new version, you get the
improvement automatically – unless you deliberately set that value yourself.
**Edit › Reset Settings to Defaults...** resets everything; your levels stay untouched.

---

## 11.14 Exporting a Game

In the end, your level should become a game that others can play. Choose
**File › Package Game (HTML)...** or the package button in the toolbar: check the **Game title**,
click **Package**. Studio downloads a file `<Title>.html` and offers **Play it**, which starts it
in a new tab.

The file is a **complete game**: the engine, the level, and everything that belongs to playing are
inside it. It needs neither a server nor the internet, and it even starts straight into the game
with a double-click, without the editor. You control it like when playing in Studio, with touch
sticks on a phone. Playing online works too ([Chapter 10](10-networking.md)).

Two things about the Micro Duck: the duck's downloaded networks are **not** in the file – walking
works, but for everything else their folders have to sit next to the file. And the duck's look
comes from Pollen Robotics and may only be used **non-commercially**, so not in a game you earn
money with ([Chapter 12](12-your-own-games.md)).

> **Mission:** Open your saved staircase from 11.6 with **Ctrl+O** (for example `stairs.klevel`).
> Give it a **Rolling Ball** as player character (just place one from the **Players** group) and
> export it. Click **Play it** – and send the file to a friend.

---

## 11.15 Multiple Players

In the **Play** menu you start your level with 2 to 4 players on one computer (**Number of
Players**), make the connection worse on purpose (**Network Emulation**), or play with other tabs
and computers (**Host Online Game...**, **Join Online Game...**). How that works and why it works
even with bad internet is shown in [Chapter 10](10-networking.md), with two missions.

---

## 11.16 How Studio Is Built

Finally, a look under the hood. Studio consists of two parts:

- the *engine* in C++, as WebAssembly in the browser. It contains the whole editor: level, undo,
  selection, viewports, menus, playing and multiplayer.
- the **web page**. It only draws the panels, passes on mouse and keys, and does what only a
  browser can: save files, capture the mouse, output sound, send packets.

> **Picture it like this:** Studio is the game console, the web page is the controller. The
> controller only presses buttons and shows what the console says. The thinking is done by the
> console.

**Why like this?** First, the editor runs exactly the code of your exported game. There's no second
version that could secretly calculate differently – what works in the editor works in the game.
Second, the whole editor can be **tested without a browser**: automatic tests can operate it
directly and check whether navigation, gizmos, playing and saving still work. That way a bug gets
noticed before anyone runs into it while building.

**Everything is a command.** Whatever you do in Studio – a menu entry, a button, a key, a line in
the console – becomes a **command** with a name and values, such as "spawn, physics_cube" or "set
the location to 0, 0, 2." The engine carries it out. Because everything goes through the same
door, the tests can operate Studio exactly like you do. Most of the Studio pictures in this book
come from such tests.

> **For pros:** The exact names of the C++ class, the functions between web page and engine, and
> all commands are in the blue book, Chapter 11, and in [Chapter 17](17-reference.md).

---

## Level complete!

- You know that a level is made of actors, and components decide what an actor can do.
- You look into the world with viewports and know perspective and blueprint view.
- You select, move only with the gizmo, and use the grid like graph paper.
- You know why undo works like a save point.
- You connect things as parents and children or with joints.
- You test with **Play** and **Simulate** without changing your level – except with K.
- You build logic without code, save, export, and know why every action is a command.

## Puzzle corner

1. An actor only has a **Static Mesh**. What do you have to add so it falls down when simulating?
2. You drag across a cube with the left mouse button. Does it move?
3. While simulating, you make a tower collapse and press Esc. What happened to the tower in the
   level?
4. Your logic graph has an error. Does the game start? Where's the message?
5. **Boss puzzle:** Grid 0.5 m, a cube at x = 0. You hold Alt and, in world axes, drag the red
   arrow 1.3 m in the arrow's direction. How many cubes are there, at which x does the moved one
   land, and how many times do you have to press Ctrl+Z until there's only one again?

### Solutions

1. A **Rigid Body**. Only that turns the actor into a physics body (11.3).
2. **No** – left dragging draws a selection rectangle. Only the gizmo moves things.
3. It's standing there **unchanged**. Testing doesn't change the level; only **K** keeps positions.
4. **Yes.** Only that graph gets switched off; "Logic error: …" is in the **Output Log**.
5. **Two** cubes; the original stays at 0. The copy snaps to the nearest multiple of 0.5 m: 1.3 is
   closer to 1.5 than to 1.0, so **x = 1.5 m**. **One** Ctrl+Z is enough – copy and move are one
   step.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
