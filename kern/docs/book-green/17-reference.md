# 17 For Looking Things Up

> **In short:** You don't read this chapter from start to finish; you look things up in it. Here
> you'll find the keys in the Studio and for the Micro Duck, the most important file types, and a big
> glossary with every term you might stumble over in this book. Whatever only programmers need is in
> the blue book.

This book comes in three editions with the same content: the **green** one (this one, simply
explained), the **blue** one (complete and technical, for programmers, with every function and every
option), and the **yellow** one (for young readers). When it says "in the blue book, Chapter 17"
here, it means the chapter of the same name in the blue edition.

> **Key terms:**
> - **Glossary** – a small dictionary of technical terms, here in 17.5.
> - **Keyboard shortcut** – a key or key combination that triggers a command, without the mouse.
> - **File type** (format) – how a file's content is structured; you can usually tell it by the
>   extension after the dot, such as `.klevel`.

> **Imagine …** this chapter is the note pinned to the inside of the workshop door: which switch does
> what, and what does "torque" actually mean? The operating manual for every machine is on the shelf
> (in the blue book). For everyday work, the note is usually enough.

## 17.1 Keys in the Studio

You operate Kern Studio much like the big game construction kits that pros use. You don't have to
memorize anything: **F1** shows the most important shortcuts right in the Studio at any time. The keys
are named here as on the keyboard: **Ctrl**, **Shift**, **Delete**, **End**, **Space**. The Studio
shows them the same way. On a Mac, the Studio treats the Cmd key like Ctrl. All the details are in
[Chapter 11](11-studio.md).

### Camera in the viewport

The **viewport** is the big window in the middle through which you look into the 3D world. You steer
the camera in it like this:

| Input | Effect |
|---|---|
| Hold right mouse button + W A S D | fly like in a game (Fly Mode) |
| … + Q / E | down / up |
| … + Shift | fly faster |
| … + mouse wheel | set the flying speed |
| Alt + left mouse button | orbit around the selection like around a sculpture |
| Drag with middle mouse button | pan the view sideways |
| F | bring the selection into view |
| Double-click on a thing | while editing: select it and bring it into view |
| Ctrl+1 … 9 / 1 … 9 | remember a camera position / jump back to it |

The remembered camera positions are called **Bookmarks**: like bookmarks in a book, only for
viewpoints. Remembering and jumping back works in edit mode; while playing, the number keys control
the duck, for example. A double-click on empty space brings nothing into view: the first click on
empty space clears the selection. On a phone, one finger orbits, two fingers zoom and pan, a double
tap brings the selection into view, and a long press opens the context menu.

### Editing

Here you press the keys **without** holding the right mouse button. With the right mouse button held,
the camera flies, and Q and E then mean "down" and "up" (see above).

| Key | Effect |
|---|---|
| Q / W / E / R | tool: select / move / rotate / scale (change size) |
| Space | move → rotate → scale in turn |
| [ / ] | snapping grid finer / coarser (the grid things snap to when moved) |
| Delete | delete the selection |
| End | set the selection down on the floor |
| H / Ctrl+H | hide / show everything again |
| G | **Game View**: editor helpers off, picture as in the finished game |
| F2 | rename |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+C / X / V / D | copy / cut / paste / duplicate |
| Ctrl+G | group |
| Ctrl+S / Ctrl+O / Ctrl+N | save / open / new level |
| Ctrl+Space | open and close the Content Drawer |
| Alt+P / Alt+S | Play / Simulate |

The **Content Drawer** is the drawer at the bottom of the Studio with parts, images and sounds; the
**Output Log** is the list of the Studio's messages. You can also reach both with the mouse, via a
button in the status bar at the very bottom. That helps if your system already uses Ctrl+Space for
something else, such as switching the keyboard language.

> **Did you know?** Copied things land as text in your computer's clipboard. A Ctrl+V in another
> browser tab with the Studio pastes them there – that's how you move things from one level to
> another.

### Play and Simulate

Two ways to run a level: in **Play** you play yourself, and the camera follows your character. Some
levels, such as the Micro Duck, start Play as a spectator with a free camera, set in the
**World Settings** with the field
**Observer camera (Play: fly, watch, drag bodies)**; with **F8** you then take over the character. In
**Simulate**, physics and game rules run but nobody plays; you keep the editor's free camera and can
grab, drag and throw bodies with the mouse. In both cases the level is locked, and when you stop,
everything is as it was before – unless you take over the new poses with **K**.

| Key | Effect |
|---|---|
| Esc | stop |
| F8 | let go of / take over the character again (menu: Eject from Player / Possess Player) |
| Pause or Alt+Shift+P | pause / resume |
| . (period) | one step further, when paused |
| Ctrl+R | restart the game |
| K | while simulating: take the poses of the selected things over into the level |

In the game itself the usual applies: W A S D moves, the mouse looks around, Space jumps, Shift
sprints, F uses. So the F key has two jobs: if you're controlling a character, it uses something;
otherwise it brings the selection into view. The keys of the logic editor are in
[Chapter 9](09-logic.md).

> **Try it!** At https://kernengine.de/, open **File › Example Levels › Physics Lab** and press
> **Alt+S** for **Simulate**. Then press **Pause** and tap the **period** a few times. You see the
> physics step by step – like a flip book you flip through yourself. Press **Pause** again so time
> runs on. Then grab a body with the left mouse button and throw it; with **Esc** everything is back
> in its place. (Many laptops have no Pause key; then use Alt+Shift+P.)

## 17.2 Keys for the Micro Duck

| Movement | Keyboard | Gamepad | Touch |
|---|---|---|---|
| Walk / turn | W S / A D | left stick | MOVE |
| Sit / stand up | 1 | B | SIT |
| Pick up (on roller skates: crouch) | 2 | Y | PICK |
| Kick left / right | 3 / 4 | – | KICK L / KICK R |
| Roll (hold: keep going) | 5 | X | ROLL |
| Beak open / closed | 6 | – | BEAK |
| Jump | 7 | – | HOP |
| Quack / "Wheee" | Space / V | right / left rear shoulder button (RT / LT) | QUACK / WHEEE |

The duck also comes as a roller-skate variant, **Micro Duck (Rollers)**; it then skates instead of
walking ([Chapter 13](13-micro-duck.md)).

With **F8** you take over the duck; then W A S D and the number keys listen to you. Every input counts
as "hand on the wheel" for 2 seconds. During that time, Pollen's decision logic, which
[Chapter 13](13-micro-duck.md) calls the duck's **brain**, and the play layer (Kern's network that
decides by itself whether to fetch the ball or the sock) hold still. If you don't press anything for
longer, the duck decides for itself again, provided **Autonomous (camera, ToF)** is on in the Details
panel.

> **Try it!** Open **File › Example Levels › Micro Duck**, press **Play**, take over the duck with
> **F8**, and try the keys 2 to 7 one after another. After each key, wait until the movement is
> finished: as long as one is running, no new one starts. **2** (pick up) only shows a bend without a
> sock in front of the beak, and with **5** it keeps rolling as long as you hold the key. Then press
> **1** so it sits down, and right after that **3**. Nothing happens: from sitting, no movement starts.
> Wait until it's fully seated and press **1** again – only once it has stood up does it kick.

## 17.3 Files

Kern stores its things in a few file types. Most are simple **text files**: you can open them with any
text editor. **JSON** is a text format made of names and values ("name: Orb Run"), **CSV** is a table
as text: one table row per line of text, the columns separated by commas – any spreadsheet program
opens CSV files too.

| File | What's in it | more in |
|---|---|---|
| Level (`.klevel`) | a whole game world, as JSON | [Chapter 8](08-game-runtime.md), [11](11-studio.md) |
| Network (`.onnx`) | a trained network | [Chapter 13](13-micro-duck.md), [14](14-lehrer.md) |
| Servo profile (JSON) | the numbers for a servo type | [Chapter 16](16-calibration.md) |
| Measurements (CSV) | recordings from the test bench or the robot | [Chapter 16](16-calibration.md) |

**Levels.** A level contains everything in the world: the things with their building blocks, the
world settings, the editor's camera position, embedded images and sounds, and the logic. A foreign
file, or one from a newer Studio, Kern would rather not open at all than read it half wrong.

**Networks.** Kern reads and writes networks in exactly the form Pollen Robotics uses for the Micro
Duck. That's why Pollen's networks run in Kern and Kern's networks run on the robot. A network you
train with the Lehrer in the browser stays stored on your device and replaces the one that ships with
Kern there.

The exact formats are in the blue book, Chapter 17.

## 17.4 What's only in the blue book

Two parts of the reference are pure programmer tools and are therefore only in the blue book,
Chapter 17:

- **The Lehrer as a command-line program:** besides the Lehrer page in the browser, the Lehrer also
  exists as a program you start with typed commands – for training, evaluating, calibrating and
  measuring. All commands with their settings and examples are there.
- **The interface between engine and browser:** the engine is the heart of Kern, the program that
  calculates, draws and runs the physics. It's written in a programming language for fast programs
  (C++) and runs in the browser in a fast program format (WebAssembly); the web page around it is
  written in the language of web pages (JavaScript). The two only talk to each other through a
  precisely defined list of functions, like through the serving hatch of a cafeteria. If something
  goes wrong, the web page gets a message back instead of the program crashing. And the engine never
  touches files on your device itself: whatever it needs, the web page hands through to it.

> **Did you know?** The Studio's web page contains no editor logic. Menus, Outliner, Details,
> gizmos, undo – all of that lives in the engine. The page only asks through the serving hatch what it
> should display, and draws it. The advantage: exactly the same code runs in the editor as in the
> finished game, and the whole editor can be tested without a browser. That way automatic checks find
> bugs without anyone having to click.

## 17.5 Glossary

The technical terms of the green book, briefly in everyday language. In parentheses: where they come
up in detail. If a word is missing, the glossary of the blue book will help.

> **Watch out!** The word **actor** has two meanings in this book. In the **Studio**, an actor is a
> thing in the level: a crate, a lamp, the duck. In **learning** (Chapter 14, Appendix A), the actor
> is the network that acts – as opposed to the critic, which only judges. The chapter always tells you
> which meaning is meant.

### Studio and games

| Term | Meaning |
|---|---|
| **Actor (in the Studio)** | a thing in the level with a position and building blocks ([Ch. 8](08-game-runtime.md)) |
| **Autosave** | the Studio regularly backs up your work by itself |
| **Blueprint / logic graph** | a program made of little boxes and connecting lines, without typing ([Ch. 9](09-logic.md)) |
| **Bookmarks** | a remembered camera position (Ctrl+1 … 9) |
| **Character** | the player character that walks, runs and jumps ([Ch. 12](12-your-own-games.md)) |
| **Collider** | the invisible shape a thing bumps into things with in the physics |
| **Component** | a building block of an actor, such as light, sound or physics |
| **Console** (Cmd) | the field in the status bar where you type commands, such as `gravity` |
| **Content Drawer** | the drawer at the bottom of the Studio: parts, images, sounds, Output Log |
| **Context menu** | the menu on right-click (on a touchscreen: press and hold) |
| **Details** | the panel with all the settings of the selected thing |
| **Editor** | a program for editing things; here, the Kern Studio workshop |
| **Eject / possess (F8)** | in the game, let go of the character / take it over again |
| **Example Levels** | ready-made example worlds in the **File** menu, such as Orb Run or Micro Duck |
| **Game View** | a view without editor helpers, as in the finished game |
| **GameWorld** | the part of Kern that brings a level to life and runs it ([Ch. 8](08-game-runtime.md)) |
| **Gizmo** | arrows and rings on the selected thing for moving, rotating, scaling |
| **HTML file** | the file format of web pages; an exported game is a single HTML file ([Ch. 11](11-studio.md)) |
| **HUD** | displays on top of the game picture: points, time, speed |
| **Kern Studio, Studio** | the editor in the browser, where you build and play levels ([Ch. 11](11-studio.md)) |
| **Kill Z** | a height (Z points up); anyone who falls lower is reset but doesn't lose ([Ch. 8](08-game-runtime.md), [11](11-studio.md)) |
| **Level** | a game world, saved as `.klevel` |
| **Logic editor** | the window for logic graphs; its labels stay English ([Ch. 9](09-logic.md)) |
| **Mobility** | a Rigid Body field: static, kinematic or dynamic ([Ch. 11](11-studio.md)) |
| **Node / pin** | a little box in the logic graph / its connectors for connecting lines |
| **Observer camera** | Play starts with a free camera for watching; F8 takes over the character ([Ch. 11](11-studio.md)) |
| **Outliner** | the list of all things in the level, top right, arranged as a tree |
| **Output Log** | the list of the Studio's messages, with times |
| **Package Game** | save a level as a self-contained web page for passing on |
| **Pickup** | something to collect, such as a glowing orb |
| **Play / Simulate** | play yourself / run the physics and watch (17.1) |
| **Rigid Body** | the physics component: the body falls, rolls and bumps into things, but never bends ([Ch. 6](06-physics.md)) |
| **Snapping** | moving and rotating in fixed steps, like magnets on graph paper ([Ch. 11](11-studio.md)) |
| **Status bar** | the bar at the very bottom: messages, console, sound, language |
| **Toolbar** | the tool bar below the menus |
| **Transform** | an actor's location, rotation and size |
| **Trigger** | an invisible zone that sets something off when someone gets into it (for the gamepad buttons of the same name, this book says "rear shoulder button") |
| **Viewport** | the big window through which you look into the 3D world |
| **World Settings** | settings for the whole world, such as gravity and step rate |

### Technology under the hood

| Term | Meaning |
|---|---|
| **Authority** | who has the last word; in multiplayer, only the server calculates ([Ch. 9](09-logic.md)) |
| **Bit** | the smallest unit in a computer: a little box holding 0 or 1; eight bits make a byte |
| **Bloom** | the glow around very bright spots, like backlight ([Ch. 4](04-graphics.md)) |
| **Box3D** | the physics engine Kern ships unmodified ([Ch. 6](06-physics.md)) |
| **Build script** | a small program that controls the translating of Kern ([Ch. 2](02-building-and-testing.md)) |
| **C++** | the programming language Kern is written in |
| **Channel** | a way of sending in multiplayer: registered mail (reliable) or postcard ([Ch. 10](10-networking.md)) |
| **Checksum** | a small number made from the content; if it doesn't match, something is damaged or foreign ([Ch. 10](10-networking.md)) |
| **Compiler, compile** | translate program text into instructions the computer runs directly |
| **Constraint** (joint) | a connection between two bodies, such as a hinge |
| **Contact** | a spot where two bodies touch |
| **Core** | one of the calculating units in the processor; several can calculate at the same time |
| **Depth buffer** | remembers for each pixel how far away what was painted there is ([Ch. 4](04-graphics.md)) |
| **Determinism** | the same inputs give the same result down to the last digit, on every device |
| **Draw call** | an instruction to the graphics card to draw something ([Ch. 4](04-graphics.md)) |
| **Engine, game engine** | the heart of Kern: the program that calculates, draws and runs the physics |
| **Fingerprint** | a check number per tick; if it matches everywhere, Kern calculates the same way everywhere |
| **Frame** | a single picture on the screen; many per second make movement |
| **Frustum** (viewing pyramid) | the space the camera sees; whatever lies outside isn't painted ([Ch. 3](03-foundations.md), [5](05-scene-and-geometry.md), Appendix B) |
| **Game loop** | prepare once, then in every frame: read input, move the world on, paint ([Ch. 12](12-your-own-games.md)) |
| **Graphics card** | the part of the computer that paints pictures, very fast and many things at once ([Ch. 4](04-graphics.md)) |
| **GTAO** | a method that darkens corners and cracks naturally ([Ch. 4](04-graphics.md)) |
| **Handshake** | signing in with a callback before game data flows in multiplayer ([Ch. 10](10-networking.md)) |
| **Hash** | a short number made from any amount of data; same data, same number ([Ch. 3](03-foundations.md)) |
| **Instance** (instancing) | a copy of the same shape in another place, like a stamp ([Ch. 5](05-scene-and-geometry.md), [12](12-your-own-games.md)) |
| **Interface** (hand-over point) | the agreed place where two parts of a program exchange things, like a counter ([Ch. 1](01-introduction.md)) |
| **Interpolation** | mixing in-between values, for example between two snapshots ([Ch. 4](04-graphics.md), [10](10-networking.md)) |
| **Interpreter** | runs a logic graph box by box; the other two ways are the **translation** (JIT) and the **C++ export** ([Ch. 9](09-logic.md)) |
| **JavaScript** | the programming language of web pages |
| **JIT** | the **translation**: turns a logic graph into instructions the computer runs directly at startup ([Ch. 9](09-logic.md)) |
| **Jitter** | packets arrive sometimes faster, sometimes slower ([Ch. 10](10-networking.md)) |
| **Lag, lag compensation** | delay caused by the connection / the server briefly turns back time for a hit ([Ch. 8](08-game-runtime.md), [10](10-networking.md)) |
| **License** | the rules of the game for what you may do with a work; Kern is under Apache 2.0 ([Ch. 12](12-your-own-games.md)) |
| **Listen server** | a server that plays along itself ([Ch. 10](10-networking.md)) |
| **Machine code** | the instructions a processor understands directly |
| **Matrix** | a table of 4 by 4 numbers that bundles stretching, rotating and moving ([Ch. 3](03-foundations.md)) |
| **Mesh** | a 3D shape built from many small triangles |
| **Module** (`.wasm`) | the fully translated engine as one file the browser runs ([Ch. 12](12-your-own-games.md)) |
| **Network copy** (replica) | the copy of the game on the guest's side, which follows the server's snapshots ([Ch. 8](08-game-runtime.md), [10](10-networking.md)) |
| **Normal** | the direction a surface "faces" ([Ch. 4](04-graphics.md), Appendix B) |
| **Open source** | the program text is open and may be used according to the rules of the license |
| **Packet** | a small bundle of data that travels across the network as one piece ([Ch. 10](10-networking.md)) |
| **Particles** | lots of tiny bits for sparks, dust or smoke |
| **Physics step** | one picture in the physics flip book; the world moves on a little bit |
| **Picking** | finding out what you clicked on, with an invisible ray ([Ch. 5](05-scene-and-geometry.md)) |
| **Piggy bank** | collects the time of the screen pictures and pays it out in whole physics steps; the rest waits for the next picture ([Ch. 6](06-physics.md), 6.12) |
| **Pixel** | a single dot of the picture |
| **Record collection / sound engineer** | in the game: keeps all sounds ready / plays them at the right time ([Ch. 7](07-audio.md)) |
| **Relay** | a "post office" on the internet that passes game packets between computers ([Ch. 10](10-networking.md)) |
| **Renderer** | the part of Kern that paints the picture ([Ch. 4](04-graphics.md)) |
| **Round-trip time** (ping) | how long a packet takes to go there and back ([Ch. 10](10-networking.md)) |
| **Serialization** | packing data into a row of bits to save space, and back ([Ch. 10](10-networking.md)) |
| **Server / client** | in multiplayer: the computer that decides / the computers that play along (guests) |
| **Shader** | a small program on the graphics card that calculates the color of each pixel |
| **Simulation** | the rebuilt world inside the computer that keeps running by the rules of physics |
| **Snapshot** | a server's "situation report" in multiplayer: where everything is right now ([Ch. 10](10-networking.md)) |
| **Solver** | the part of the physics that satisfies all the rules (joints, contacts) together ([Ch. 6](06-physics.md), Appendix C) |
| **Span** | a "note" that points to data (where it starts, how many) without owning it ([Ch. 3](03-foundations.md)) |
| **Step rate** | how many physics steps are calculated per second of world time; with the duck 100, which is 200 in duck time ([Ch. 6](06-physics.md), [13](13-micro-duck.md)) |
| **Sub-steps, substeps** | in-between steps within one physics step, for more precision; 16 for the duck |
| **Texture** | an image glued onto a surface |
| **Thread, worker** | a strand of work; several calculate at the same time on several cores |
| **UV** | coordinates that say how an image is laid onto a shape, like wrapping paper ([Ch. 5](05-scene-and-geometry.md)) |
| **Vector** | a set of directions made of numbers, such as "3 m to the right, 1 m forward" ([Ch. 3](03-foundations.md)) |
| **Web server** | a program that delivers files the way a website does ([Ch. 12](12-your-own-games.md)) |
| **WebAssembly** | a program format that browsers run almost as fast as real programs |
| **Welding** | merging duplicate corners of a mesh into one ([Ch. 5](05-scene-and-geometry.md)) |
| **Z-fighting** | two surfaces at almost the same distance flicker because the depth isn't precise enough ([Ch. 5](05-scene-and-geometry.md), Appendix B) |

### Robots and measuring

| Term | Meaning |
|---|---|
| **Armature** | extra inertia, because the nimble motor in the servo has to go along with every movement |
| **BallPlay** | the rebuild of Pollen's ball game in the brain; plays when the Kern extras are off ([Ch. 13](13-micro-duck.md)) |
| **Base pose** | the starting position of the joints when the duck stands calmly |
| **Brain (of the duck)** | in Chapter 13, Pollen's decision logic: what the duck does when nobody is steering |
| **Calibration** | fitting a model's numbers to measurements |
| **Camera detector** | the play layer's "eye": reports where the duck's camera sees the ball or the sock |
| **Check error** | the error on a measurement that wasn't used for fitting |
| **Chirp** | a measuring signal: a back-and-forth that keeps getting faster, like rising birdsong |
| **Control tick, tick** | the heartbeat of the control: 50 times per second (in duck time), every servo gets a new target |
| **CSV** | a table as a text file, columns separated by commas |
| **Daemon** | the background program on the real robot that passes commands on to the servos ([Ch. 13](13-micro-duck.md), [16](16-calibration.md)) |
| **Device model** | a small calculation model of a real device, such as a servo |
| **Duck time / world time** | time as the giant duck experiences it / the time on your clock; the duck walks in slow motion, one second of world time is half a second for it ([Ch. 13](13-micro-duck.md)) |
| **Friction** | how strongly something grips, usually a number from 0 (ice) to 1 (rubber) |
| **Gear backlash** | a bit of wobble in the gearbox before the gears engage |
| **Hz (hertz)** | "times per second": 50 Hz means 50 times per second |
| **Inertia** | how much effort it takes to get something moving or turning |
| **Joint sensor (encoder)** | measures inside the servo which angle the joint has right now |
| **JSON** | a text format made of names and values |
| **Kern extras** | switch in the duck's Details panel: play layer, sock fetching, beak in the pick ([Ch. 13](13-micro-duck.md)) |
| **Log** | a recording, line by line: what was sent and what was measured (in the Studio, the list of messages is called the Output Log) |
| **Low-pass** | a filter that swallows fast jitter and only lets the calm movement through, like a shock absorber |
| **Matching** | replaying a recording of the real robot in the simulation and comparing ([Ch. 16](16-calibration.md)) |
| **Material pair** | two materials with a friction value of their own, a measured one, such as sock on wood ([Ch. 13](13-micro-duck.md)) |
| **Micro Duck** | the two-legged walking robot by Pollen Robotics ([Ch. 13](13-micro-duck.md)) |
| **ms (millisecond)** | a thousandth of a second |
| **Pick-up detector** | a tiny network by Pollen that recognizes whether the duck is being lifted ([Ch. 13](13-micro-duck.md)) |
| **Pollen Robotics** | the company that built the real Micro Duck and its networks; Kern is not affiliated with it |
| **rad (radian)** | an engineering unit of angle: 1 rad is about 57 degrees, a full circle about 6.28 rad |
| **RMS error** | the average distance between two curves, with big outliers counting more |
| **Scale 4** | Kern builds the duck four times as big so the physics calculates stably: 64 times as heavy, half as fast ([Ch. 13](13-micro-duck.md)) |
| **Servo** | a motor that moves to a given angle |
| **Servo profile** | the few numbers that describe a servo type ([Ch. 16](16-calibration.md)) |
| **Sim-to-real gap** | the difference between the simulation and the real robot |
| **Spread** | how far a value may vary, or varies from unit to unit |
| **Test bench** | a single servo with an arm and weight, clamped in place for measuring |
| **Tilt sensor (IMU)** | senses where down is and how fast the body is turning |
| **ToF** | a distance sensor that measures how long light takes to go there and back |
| **Torque** | turning force: how hard a motor can turn a joint |
| **Tracker** | the play layer's "memory": remembers for a while where the ball or the sock last was |
| **Walker** | the control type in the Studio for walking robots like the duck, moved by a network |

A picture for **rad**: lay a string as long as a circle's radius along its edge. The angle it spans as
seen from the center is 1 rad. About six and a quarter such strings go once around the circle.

### Networks and learning

| Term | Meaning |
|---|---|
| **Action** | what the network does on one tick: 14 target angles for the servos |
| **Action noise** | the randomness added to each command while practicing: the duck's curiosity ([Ch. 14](14-lehrer.md), [15](15-your-own-models.md)) |
| **Activation (ELU)** | the "bending" in the neuron: positive numbers stay, negative ones get squashed |
| **Actor (when learning)** | the network that acts and later goes onto the robot ([Appendix A](A-neural-networks.md)) |
| **Adam** | a clever method for adjusting the sliders in even, calm steps |
| **Advantage** | how much better an action was than what the duck usually does here |
| **Agent** | the one acting in learning through reward, here the duck with its network |
| **Asymmetric critic** | while practicing, the critic sees more than the actor, such as the exact height |
| **Backpropagation** | spreading the error backward from the output onto all sliders |
| **Bias** | a neuron's base setting, which is always added in |
| **Chain rule** | the calculation rule backpropagation uses to pass the error back layer by layer |
| **CMA-ES** | search method: try out a cloud of candidates, move toward the better ones |
| **Critic** | the second network while practicing; it estimates how good a situation is and never acts |
| **Critic first** | after imitation, only the critic learns for a while, until its judgments are any good |
| **Curriculum** | difficulties are added only step by step, as when practicing the piano |
| **DAgger** | imitation in which the network drives itself and the role model comments on every situation |
| **Distribution shift** | an imitating network, when doing things itself, gets into situations it has never seen |
| **Entropy** | a measure of curiosity: how widely the duck tries things around its best idea |
| **Episode** | one practice run, from the start until a fall or until time is up |
| **Episode return** ("Episoden-Rückgabe") | all the points of an episode simply added up; that's the readout on the Lehrer page ([Ch. 15](15-your-own-models.md)) |
| **Epoch** | going through all the collected practice data once |
| **Evaluator / helpers** | in the Lehrer: the part that learns from the notes / the parts that let ducks work out and take notes; technical terms learner / collector ([Ch. 14](14-lehrer.md)) |
| **Exponential function** | shrinking by the same share per step; ELU uses it to squash negative numbers |
| **GAE** | estimates the advantage of a step: its own surprise plus part of its consequences |
| **Generation** | one round of the role model search with CMA-ES: 16 attempts are graded ([Ch. 14](14-lehrer.md)) |
| **Gradient** | the slope of the loss landscape: which direction is uphill |
| **Gradient descent** | learning by walking downhill step by step in the loss landscape |
| **Imitation (behavior cloning)** | the network learns to do what a role model demonstrates |
| **Iteration, practice round** | a training round: all ducks practice a bit, then the network is improved once |
| **Layer** | many neurons side by side that see the same inputs |
| **Learning rate** | the step length when adjusting the sliders |
| **Lehrer** | Kern's training system for the Micro Duck's networks ([Ch. 14](14-lehrer.md)) |
| **Lehrer page** | the web page kernengine.de/lehrer/, where you train in the browser ([Ch. 14](14-lehrer.md), [15](15-your-own-models.md)) |
| **Loss** | a number that says how wrong the network is right now; smaller is better |
| **Measurement** | a strict check of whether a movement really succeeded, independent of the points ([Ch. 15](15-your-own-models.md)) |
| **Minibatch** | a small, random handful of practice data for one learning step |
| **Network (neural network)** | a calculating machine with many sliders: numbers in, numbers out |
| **Neuron** | the smallest building block: multiply, add up, bend |
| **Noise, σ (sigma)** | random deviation; σ says how wide it is |
| **Normal distribution (bell curve)** | small deviations are common, big ones rare |
| **Normalizer** | converts each measurement to a common scale: "how far above the usual?" |
| **Observation** | what the duck measures on one tick: 61 numbers about pose, joints, commands |
| **ONNX** | an exchange format for networks that many programs can read |
| **Overfitting** | memorizing instead of understanding: good at the exercises, bad at anything new |
| **Play layer** | Kern's network that decides what the duck does with ball and sock |
| **Policy** | the duck's decision rule, that is, its trained network |
| **PPO** | the Lehrer's learning method: learning from reward, in small, careful steps |
| **Recipe** | all the training settings of a task together |
| **Return** | what learning calculates with: the points from now on, later ones worth a bit less ([Appendix A](A-neural-networks.md)); not the same as the episode return |
| **Reward** | points per tick that say how well the duck just did |
| **Role model** | a demonstration that a network imitates first |
| **Seed** | the starting number for randomness: same seed, same randomness |
| **Snapshot** | an intermediate state of the network saved along the way ([Ch. 15](15-your-own-models.md)) |
| **Supervised learning** | learning with an answer key: every exercise comes with the right answer |
| **Training** | adjusting a network's sliders until the right thing comes out |
| **Value** | how many points the duck can still expect from a situation |
| **Variation** | each episode rolls slightly different conditions; technical term domain randomization |
| **velstand** | Pollen's network for walking and standing, built into Kern |
| **Warm start** | a new training run begins with an already trained network instead of randomness |
| **Weight** | a slider in the network: how much an input counts |

The exact definitions are in the glossary of the blue book, Chapter 17.

## What you take away

- **F1** in the Studio shows the most important shortcuts; Alt+P plays, Alt+S simulates, Esc stops.
- **Play** means playing yourself, **Simulate** means watching and grabbing bodies.
- You steer the duck with W A S D and the keys 1 to 7.
- Levels are text files, networks are `.onnx` in the same format as Pollen Robotics uses.
- The whole editor lives in the engine; the web page only draws what the engine tells it.
- "Actor" means a thing in the level in the Studio, and the acting network when learning.
- For every function, every option and every file format, you look things up in the blue book.

## Brain teasers

1. You've selected a crate, but it's outside the picture. How do you bring it into view?
2. You read "The actor learns from the advantages the critic estimates." Does this mean a crate in
   the level?
3. You press 3 for the duck, but nothing happens. What could be the reason?
4. What's the difference between **Play** and **Simulate**?

### Solutions

1. **F** brings the selection into view, and so does a double-click on its name in the Outliner. (A
   double-click on empty space doesn't help; it clears the selection. And if you hid the crate with
   **H**, **Ctrl+H** brings it back.)
2. No. Next to the critic, the actor is the acting network when learning, not a thing in the level.
3. Maybe the duck is sitting right now: from sitting, no movement starts. First have it stand up with
   1. Or another movement is still running; then you wait until it's finished. (And don't forget F8,
   so you're actually steering the duck.)
4. In Play you play yourself, and the camera follows the character (in some levels you watch first and
   take over with F8). In Simulate, physics and rules run without a player; you keep the free camera
   and can grab bodies and take over their new poses with K.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
