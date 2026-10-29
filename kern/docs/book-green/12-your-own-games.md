# 12 Your Own Game

> **In short:** Now it's your turn! Kern offers three paths to your own game. Path A needs no
> programming at all and happens in the Studio, and that's the one we'll walk step by step here: at
> the end you'll have a small collecting game that even runs on your phone. Paths B and C are for
> people who program. We'll look over their shoulders and pull out the ideas that are in every game.

> **Key terms:**
> - **Frame** – a single picture on the screen; a game paints 60 or more of them per second, like a flip book
> - **C++ and JavaScript** – two programming languages; Kern itself is written in C++, web pages speak JavaScript
> - **Module (.wasm)** – the fully translated engine as one file that any browser can run
> - **Web server** – a program that delivers files the way a website delivers them
> - **Open source** – the program text is open; anyone may read, use and pass it on, according to the rules of the license
> - **License** – the rules of the game for what you may do with a work
> - **Shader** – a small program that runs on the graphics card and colors every pixel
> - **Trigger** – an invisible zone that sets something off when someone walks into it
>
> An **HTML file** is the file format of web pages; every browser opens it.

| Path | What you need | What comes out |
|---|---|---|
| **A** in the Studio | just the mouse, no programming | an HTML file: your game |
| **B** in C++ | programming skills | a game where you write everything yourself |
| **C** from JavaScript | programming skills | Kern as a building block in your own web page |

To be honest: for paths B and C you need to know how to program. Path A you can do right away. All
three paths use the same engine, so the same graphics and the same physics, and you can mix them.

---

## 12.1 Path A: a game in the Studio, without code

Building a game here means placing ready-made building blocks in a world and telling them how the
game is won. It's like a construction kit with rules. We'll build "Orb Garden": a character runs
across an area with obstacles, collects glowing balls (**orbs**), avoids a hazard zone, and wins when
everything has been collected. You know how to use the panels from [Chapter 11](11-studio.md). Let's
go!

### Step 1: A new level

Open Kern Studio at https://kernengine.de/, then **File › New Level...**, template **Basic**,
**Create**. You have a floor of 40 × 40 m, sun, sky and a cube. In the **World Settings**, enter
"Orb Garden" as the **Level name** and as the **Game title**.

> **Watch out!** Why not **Empty**? Because the **Empty** template really has **nothing**, not even a
> floor. Your character would fall into the void.

### Step 2: The player character

A quick orientation in the Studio: the **viewport** is the big picture of the world in the middle.
The **Content Drawer** is the parts drawer at the bottom. The **Outliner** is the list of all things
in the level, and **Details** shows the settings of the thing you've just selected. A picture of the
Studio with all areas and a table showing where each one is can be found in
[Chapter 11](11-studio.md), section 11.2.

Open the **Content Drawer** (**Ctrl+Space**) and drag a **Character** from **Players** into the
viewport. The status bar reports "Character is the player now - press Play (Alt+P) to control it"
(the **Output Log** shows the same message, because the log stays English), and in the Outliner the
new actor "Character" carries the **PLAYER** badge: this character is now the player. In
**Details › Player Controller** you set how it feels: speed, acceleration, jump strength, and the
camera, which can look from behind, through the character's eyes, or from above.

> **Try it!** Press **Play** (Alt+P) right away. W A S D runs, the mouse looks, Space jumps,
> **Esc** takes you back. Just for fun, swap the character for a **Rolling Ball** or a **Drone** from
> **Players**: the one placed last is the player, or you choose one with right-click ›
> **Make Player (control it in Play)**.

### Step 3: Obstacles and physics

1. From **Shapes**, place a few **Cube**s, a **Ramp** and a **Plane** and nudge them into place with
   the **gizmo**. The gizmo is the colorful handle arrows on the selected thing: you drag them to
   move it, rotate it, or make it bigger.
2. From **Physics**, a **Crate Stack** and a **Hinged Door**: these react to your character.
3. From **Gameplay**, a **Moving Platform** and a **Spinner**, which move by themselves.
4. Check it with **Simulate** (Alt+S). You can grab and throw bodies.

A **Cube** from **Shapes** is a solid obstacle: you bump into it, but it doesn't move. If it should
fall, you have to give it physics: in Details with **Add › Rigid Body** and **Mobility** set to
**Dynamic (simulate physics)**, or you take a **Physics Cube** right away ([Chapter 6](06-physics.md)).

> **For the curious:** After **Simulate**, **K** takes over the simulated pose into the level, for
> example for a crate stack that should look naturally collapsed.

### Step 4: Something to collect

1. From **Gameplay**, scatter several **Orb**s (1 point) and a **Star** (10 points). Trick:
   **Alt + drag gizmo** pulls out a copy with every drag.
2. To keep the list in the Outliner tidy, put the orbs into a folder: in the Outliner, click the first
   orb, Ctrl-click the others to add them, then press the folder button at the top of the Outliner.
   (A folder is only for keeping things tidy; it doesn't change anything in the game.)
3. In **World Settings › Win condition**, choose **Collect all pickups**. **Pickups** means
   everything you can collect, so here the orbs and the star.

### Step 5: Hazard, checkpoint and time limit

1. Place a **Hazard** and drag it to size with **Scale**: anyone who walks into it has lost the game,
   and no checkpoint will help.
2. Place a **Checkpoint**. This is where the character restarts if it falls too low. How low "too
   low" is gets set by **Kill Z (respawn below)** in the **World Settings**: a height below which the
   game brings the character back.
3. If you like, set a **Time limit (0 = none)** in the **World Settings**: the **HUD** (the display at
   the edge of the screen with points and time) counts down.

> **Watch out!** Instead of a **Hazard** there's also a **Damage Zone**, which costs health. But it
> only works on characters that have health at all. The "Character" character doesn't have any out of
> the box; give it health in Details with **Add › Health**, or it walks through the zone unharmed.

Other win conditions: **Reach a goal trigger** (with a **Goal**), **Survive the time limit** or
**Reach a target score**.

### Step 6: Logic without programming

Up to here, every building block has its built-in behavior. If you want your own behavior, you build
it from **logic graphs**: little boxes you connect with wires ([Chapter 9](09-logic.md)). Let's make
a zone that gives points:

1. Place a **Message Trigger** and in **Details › Trigger** set the **Action** to
   **Logic event only**.
2. Click **Logic** in Details. The logic editor opens this trigger's graph. The logic editor isn't
   translated: its buttons are English in the German interface too.
3. **Templates › Score zone (overlap +10)**: anyone who enters the zone gets 10 points, a "+10"
   message and a sound.
4. **Compile** (F7), then **Viewport**.

Why **Logic event only**? Normally a trigger does something itself as soon as someone walks in: it
makes you win, lose, or shows a message with a sound. With **Logic event only**, it does nothing on
its own and just tells your logic: "Someone's in here." That way your graph alone decides what
happens, and nothing happens twice. Like a door sensor that doesn't ring the bell itself but only
reports: what happens next is up to you.

Why **Compile**? Compile checks your graph for errors, such as a wire leading nowhere, and shows a
traffic light: OK, warning or error. That matters, because a graph with errors gets switched off when
the game starts. The game still runs, but your zone gives no points, and you're left wondering. Check
first, then play.

More templates: a door that opens on **Interact** (key F), a spawner (a generator that makes things
appear), a traffic light, and a behavior for the Micro Duck.

### Step 7: Sound

In **World Settings › Audio**, choose some **Music** and an **Ambience** (background sound). From the
**Audio** group, place something like **Ambient Wind**, or import your own file in the **Sounds** tab
of the **Content Drawer** and drag it into the viewport. Characters, pickups and triggers already
bring their own sounds ([Chapter 7](07-audio.md)).

### Step 8: Testing

Press **Play** and go! With **F8** you stop controlling the character and watch with a free camera
while the game keeps running; press F8 again, and you're in control again. With
**Play › Number of Players › 2 Players (listen server)** you test with two players. If the Studio
finds an error in the level, the game doesn't start, and the reason is in the **Output Log**, the log
at the bottom in the **Content Drawer**.

### Step 9: Saving

**Ctrl+S** saves a `.klevel` file. It contains everything, including textures (the surface images)
and imported sounds. You can pass it on and open it in any Studio (Ctrl+O or drag it onto the page).
The Studio also backs up unsaved work in the browser and offers it at the next start.

### Step 10: Exporting

**File › Package Game (HTML)...**, check the title, **Package**. The Studio downloads
`Orb_Garden.html` and offers **Play it**. This one file contains the engine, the level and the game.
It runs without internet, straight from the hard drive, from any homepage, and on a phone with touch
sticks. There's one exception: the Micro Duck's movements other than walking are separate files that
the game loads later (12.6 and [Chapter 13](13-micro-duck.md)).

> **Try it!** Play `Orb_Garden.html` on the computer first: double-click, and it opens in the
> browser. Then get it onto your phone as described in 11.13: through a cloud storage you also use on
> your phone, and there with "Open with" in the browser. (If you have your own homepage, you can
> upload the file there too.) The engine recognizes the touchscreen and draws sticks and buttons by
> itself. You've just built a phone game!

![The exported Orb Run on a phone in landscape mode: the engine recognizes the touchscreen and draws sticks and buttons](../buch/bilder/game-phone.png)

---

## 12.2 Path B: a game in C++

> **Just for a peek:** This section and the next two show how pros control Kern from code. You
> don't need to know how to program for this. Read them as a look behind the scenes: the ideas in
> them are in every game, including your Studio game.

For games that want more than the ready-made building blocks, you write the whole game yourself in
C++. The engine takes care of drawing and everything that has to do with the browser. The game just
tells it what's in the world and what moves.

> **Imagine …** a theater. The engine is the stage crew with lights, curtain and scenery. Your game
> is the play. The crew asks you at every moment: "What happens now?" You tell them, and they make it
> visible.

### The game loop

Every game, whether from Kern or anyone else, follows the same basic pattern, the **game loop**:

1. **Prepare once at the start:** What things are there? What do they look like? Where's the camera?
2. **Then in every frame, again and again:** read input (which keys are pressed?), move the world one
   step further (car drives, crate turns), paint the picture.
3. **Clean up at the end.**

It's a flip book that draws itself: for each page, the engine asks your game what has changed since
the last page and then paints the new page.

One small catch: whatever happened "in this frame," such as "Space was just pressed," only counts for
that one frame. After that, the engine resets it. So a program has to evaluate something like that
right away, or it has missed the key press.

> **For the curious:** In C++, the game is a class with parts for "prepare," "every frame,"
> "window has a new size" and "clean up." The exact names and values are in the blue book,
> Chapter 12.

### Equally fast on every screen

A fast screen shows 120 pictures per second, a slow one maybe 60. If the game turned the camera by a
fixed amount in every picture, it would turn twice as fast on the fast screen. That would be unfair
and confusing.

The solution: in every frame, the engine tells the game **how much time has passed since the last
picture**. The game calculates: distance = speed times elapsed time.

An example: the camera should turn 90 degrees per second.

| Screen | Time per picture | Turn per picture | Turn per second |
|---|---|---|---|
| 60 pictures/s | 1/60 s | 1.5 degrees | 60 · 1.5 = 90 degrees |
| 120 pictures/s | 1/120 s | 0.75 degrees | 120 · 0.75 = 90 degrees |

More pictures, but smaller steps, and per second exactly the same comes out. So that a pause (for
example, when you briefly had another tab open) doesn't cause a giant jump, Kern counts at most a
tenth of a second per frame.

### The smallest Kern game: hello

The **hello** example shows 64 orange crates on a field that spin and cast shadows. With the mouse or
the arrow keys you orbit around them, and a click colors a crate red. Behind it are two lovely ideas:

**Instancing, the stamp principle.** You could send 64 crates to the graphics card one by one. That
would be 64 draw calls, and each one costs time. Instead, hello sends **one** crate and a list of 64
entries: where each one stands, how it's rotated, what color it is. The graphics card then stamps the
one crate 64 times in different places, with a single draw call. That's how games paint whole forests
or streets full of cars. And because only the list changes in each frame, the game only sends the list
again.

> **Imagine …** you're decorating a birthday card with 64 stars. Drawing them one by one takes
> forever. With a star stamp it's done in no time, and with different ink pads each star even gets
> its own color.

**Picking, pointing at something.** How does the game know which crate you clicked? The engine shoots
an invisible ray from the camera through the spot you clicked, into the world. The first thing the ray
hits is the clicked crate. For the stamped crate, the engine even tells you which of the 64 stamps was
hit.

### Building

The browser can't run C++ program text directly. A **translation program** (compiler) turns it into a
module, a `.wasm` file that any browser understands. Kern remembers every translated file. The second
time, only what changed gets translated, and that's much faster. The commands and switches for this
are in the blue book, Chapter 12; more on building in [Chapter 2](02-building-and-testing.md).

### A whole game: orbrun

The **orbrun** example is a complete racing game, made only of program text: a floating car (hover
car) races through a city and collects 24 orbs while night slowly falls. Controls: W/S drive, A/D
steer, Space boost, N makes time run faster, Enter builds a new city. On a phone, the left half of the
screen steers and the right half boosts.

> **Watch out!** Don't mix them up: the **Orb Run** level in the Studio is a **different** program
> with the same idea, built from Studio building blocks. The orbrun example does everything itself,
> even driving physics and collisions.

What you can learn from it:

- **A city out of randomness.** Every block gets its buildings at random. Randomness in a computer is a calculation that turns a starting number
  into a long, well-shuffled row of numbers, like a deck of cards that's always shuffled the same way
  for the same starting number. Same starting number, same city. Enter picks a new starting number and
  with it a new city.
- **Not "tunneling" through walls.** A fast car jumps quite a distance from picture to picture. If the
  jump is bigger than a wall is thick, the car is in front of the wall in one picture and behind it in
  the next, and it never "touched" the wall. That's why orbrun splits every movement into small
  substeps and checks each one.
- **Game feel instead of real physics.** The car slides briefly in curves and recovers, steering only
  works with speed, and the camera pulls back with speed. That isn't exact physics; it's made to feel
  good.

The showpiece is a **shader extension** that makes windows light up at night. (A shader is the small
program on the graphics card that colors every pixel; Kern uses a shared one for all things, the
**standard shader**.) The building generator gives each wall some extra information: how long and
tall it is, how tall a floor is, and a random number. The small program on the graphics card turns
that into a grid of windows while coloring, some of which are randomly lit at night. The special
thing: the game has added a look of its own to the standard shader **without changing the engine
itself**. In the distance, the windows get smaller than a pixel. Then the grid would flicker like a
fine check pattern on TV, so it blurs there into a soft shimmer. How such extensions work is in
[Chapter 4](04-graphics.md).

> **Try it!** In the Studio, open **File › Example Levels › Orb Run** and drag **Time of day** in the
> **World Settings**. Here too, the windows come on at night. Same idea, just in a Studio level.

> **For the curious:** orbrun can do even more: collisions without any physics engine (for the
> calculation, the car is a circle and each house a polygon), traffic that stops in front of you, and
> headlights as point lights, of which the game picks the most important ones in every picture. All
> the numbers for the city, driving physics and traffic are in the blue book, Chapter 12.

---

## 12.3 The browser helper

A C++ game doesn't want to deal with browser odds and ends. Two small helper files that Kern provides,
and that always sit side by side, take care of that: one starts the game and looks after the browser,
the other connects the web page to the module. Together they take care of:

- **Size:** the drawing area always adapts to the window, at full screen sharpness.
- **Keyboard:** keys count by their **position**, not by their label. So W A S D is in the right place
  even on a French keyboard. Arrows and Space no longer scroll the page.
- **Mouse and touch:** position, buttons, wheel and fingers; a right-click on the game doesn't open a
  browser menu.
- **Focus:** if you briefly switch to another window (for example with Alt+Tab), the engine releases
  all keys. Otherwise the character would keep running forever because the game never noticed you let
  go.
- **Capturing the mouse:** if the game wants, the mouse pointer disappears and the mouse only controls
  the view.
- **Graphics card briefly gone:** if the browser briefly loses access to the graphics card, the
  engine's drawing part pauses. Kern keeps a copy of everything it sends to the graphics card and
  afterward reloads everything from these copies.

The exact names and options are in the blue book, Chapter 12.

---

## 12.4 Path C: from JavaScript

Path C is for people who already have their own web page or a JavaScript game and want to use Kern
as a building block in it. There are two ready-made connections:

**Using only Kern's graphics.** The game logic, meaning the game's rules and sequences, stays in
JavaScript, and Kern paints. The drone game SpawnCity, for example, works this way.

**Building a Studio level into your own page.** Your page loads a level from the Studio and plays it,
just like an exported game. Then it can send the game commands as if it were a gamepad made of program
text, and ask how things stand: for example, send "run forward and jump" and then ask "Has the game
been won?" so it can show its own winner screen. Such commands mix with keyboard, touch and gamepad.
Besides running and jumping there are, for example, boost, fire, crouch and looking around.

> **For the curious:** Here's what that looks like in JavaScript. The first line means "run fully
> forward and jump," the second "stop again," the third asks whether the game has been won
> (`showMyWinScreen` you write yourself):
>
> ```js
> game.command({cmd: "gameInput", moveY: 1, jump: true});
> game.command({cmd: "gameInput", moveY: 0, jump: false});
> if (game.status.hud?.status === "won") showMyWinScreen();
> ```

**Logic graphs as C++.** In the logic editor, the **C++** button downloads your graph as readable
program text. A programmer can build it permanently into Kern; then it runs fastest. So your graph can
run in three ways that differ only in speed, never in result ([Chapter 9](09-logic.md)): the
**interpreter** reads it out box by box; the **translation** (JIT, "just in time") turns it into
machine code when the game starts; and the **C++ export** is the permanently built-in version, like a
finished printed book. The content is always the same.

All commands and names are in the blue book, Chapter 12, and in [Chapter 17](17-reference.md).

---

## 12.5 Hosting: why C++ games need a web server

A self-written C++ game consists of a few files: the web page, the module, and Kern's two helper files
from 12.3. When starting, the web page loads the module **afterward**. And that's exactly what browsers
don't allow, for security reasons, for a page you simply open from the hard drive: otherwise any
downloaded file could rummage through your other files. That's why a C++ game needs a **web server**,
a program that delivers the files the way a real website does. On your own computer a very simple one
is enough; on the internet it's your homepage. Which file has to go where is in the blue book,
Chapter 12.

> **Did you know?** An exported **Studio** game doesn't need any of this. It loads nothing afterward,
> because everything is already in the one file. That's why it runs straight from the hard drive too.

---

## 12.6 Publishing

### The rights

Kern is **open source**: the program text is open, and anyone may use it according to the rules of its
license. Kern's license is called the **Apache License 2.0**, and it's generous. Your games and your
networks (trained neural networks that, for example, control movements; more in
[Chapter 13](13-micro-duck.md)) you may use, pass on and **sell**, even without publishing your own program text. Because your
exported game contains the engine, you include the license text and the notice file (NOTICE, a short
list of who contributed what) when you pass it on. In the exported game, a note with a link is
enough. The names "Kern" and "Kern Studio" and the logo are not part of the license: you may write
"Made with Kern," but you may not call your own product "Kern." Parts by others keep their own
licenses ([Appendix D](D-licences.md)).

> **Watch out!** The **Micro Duck** is an exception. It's a small walking robot by Pollen Robotics, a
> company that builds the real robot. The Studio and every exported game contain Pollen's 3D models of
> the duck, and these may only be used **non-commercially**. If you want to sell, you need a version
> without these models; a programmer builds it with a switch of its own (blue book, Chapter 12). Then
> the look is missing and the duck is invisible. Pollen's networks, on the other hand, meaning what
> makes the duck walk, may come along with their license. Anyone who passes on a game with the duck for
> free leaves the note about Pollen Robotics and the license in the file and credits Pollen Robotics
> when showing pictures of the duck.

### Studio games

You upload the HTML file to your homepage or simply send it to people. It runs without internet, in
full screen, with keyboard, mouse, gamepad and touch.

- **With the Micro Duck:** this is the exception from step 10. The duck's movements are separate
  files that the game loads later; only walking is inside the HTML file. So put the two folders with
  these files that Kern provides (microduck and policies) next to the HTML file; otherwise the duck
  can only walk.
  Pollen's folder comes with its license and NOTICE. Why the networks are loaded later is explained in
  [Chapter 13](13-micro-duck.md).
- **Playing online:** one person opens the game, the others join. Without help, only tabs in the same
  browser find each other this way. For players on different computers, you need a go-between on the
  network, a small program on a server through which everyone is connected
  ([Chapter 10](10-networking.md)).

### C++ games

Upload what 12.5 describes, meaning page, module and the two helper files, in the same layout. Kern
itself is published exactly this way: an automatic process builds the Studio and the examples and puts
everything on https://kernengine.de/.

### Checklist

| Item | Studio game | C++ game |
|---|---|---|
| Finding errors | **Output Log**, **Play** | the browser's error display |
| Phone | sticks appear by themselves | program touch yourself |
| Sound | starts with the first tap | up to the game |
| Files | one HTML (plus duck folders) | page, module, two helpers |
| Delivery | anywhere, even from the drive | only via a web server |

By the way, the first tap starts the sound because browsers only let pages make noise after you've
clicked or tapped once.

---

## What you take away

- Three paths to a game: **Studio without code**, **C++** and **JavaScript**, all on the same engine.
- A Studio game comes together in ten steps: level, character, obstacles, pickups, hazard, logic,
  sound, testing, saving, exporting. **Package Game** turns it into one file that runs anywhere.
- **Logic event only** turns a trigger into a pure messenger for your logic; **Compile** checks the
  logic before playing.
- Every game is a **game loop**: prepare once, then in every frame read input, move the world on,
  paint.
- **Frame-rate independent:** if you calculate with the elapsed time, you're equally fast on every screen.
- **Instancing** is a stamp: one thing, many stamps, one draw call.
- Fast things move in **substeps** so they don't tunnel through walls.
- A **shader** is a program on the graphics card; an extension gives the game a look of its own.
- You may sell your games; only the Micro Duck's look is non-commercial.

## Brain teasers

1. Why do you start your game with the **Basic** template and not with **Empty**?
2. Your cube from **Shapes** doesn't fall down. What's missing?
3. Lena plays hello on a screen with 120 pictures per second, Tom on one with 60. Both hold the arrow
   key for one second. Whose camera has turned further?
4. Your self-written C++ game doesn't start when you double-click the page. Your exported Studio game
   does. What's the difference?
5. You want to sell a game with the Micro Duck. What do you have to watch out for?

### Solutions

1. **Empty** has no floor; the character would fall into the void.
2. Physics. A cube from **Shapes** is a solid obstacle; only with physics can it fall (or you take a
   **Physics Cube** right away).
3. Neither; both the same. Lena's game turns only half as far per picture but makes twice as many
   pictures, because the game calculates with the elapsed time.
4. The C++ game loads its module afterward, and browsers don't allow that for a page from the hard
   drive; it needs a web server. In the Studio game everything is in the one file; it loads nothing
   afterward.
5. Pollen's 3D models of the duck may only be used non-commercially. A version without the models
   leaves them out; the duck is then invisible.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
