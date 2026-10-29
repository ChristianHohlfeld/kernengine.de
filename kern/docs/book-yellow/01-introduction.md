# 1 Welcome to the Workshop

> **Your level:** You're stepping into Kern, a real 3D engine in the browser. You'll find out
> what's inside it, which rules it's built by, and how to play this book.

> **Words you need:**
> - **Programming language** – a language in which people write down exact instructions for a
>   computer. C++ and JavaScript are two of them.
> - **Machine language** – commands that a processor can carry out directly, without having to
>   translate them first.
> - **Browser** – the program you open websites with, such as Chrome, Firefox or Safari.
> - **Graphics card** – the chip in the computer that draws the pictures.
> - **Bit** – the smallest unit in a computer: a 0 or a 1. All numbers are made of bits.
> - **Simulation** – a world inside the computer that keeps calculating step by step by fixed
>   rules, for example how a cube falls.
> - **Level** – a game world with everything that's in it.

Hi, and welcome! Underneath every game there's an *engine*, the motor under the hood. All the game
says is: "There's a car here, and when you press W, it drives." Drawing, physics and sound are the
engine's job.

In this book, you take the engine **Kern** apart. At https://kernengine.de/ you'll find everything
it's about:

- **Kern** is the engine: it draws, calculates physics and plays sounds, for every game that uses
  it.
- **Kern Studio** is the editor: there you build worlds out of ready-made parts and play them
  right away.
- The **Micro Duck** is a real two-legged robot made by the company Pollen Robotics. In Kern it
  runs as a replica in the simulation, controlled by its real networks.
- A **network** (more precisely: neural network) is an artificial "brain" made of lots of numbers
  that learns by practicing. In the Micro Duck, a network controls the legs.
- The **Lehrer** is the program that trains such networks: it lets lots of ducks practice in the
  simulation until they can walk or jump.

On top of that, there are playable levels like **Orb Run** and **Physics Lab**.

![Kern Studio with the level Orb Run](../buch/bilder/studio.png)

> **Mission:** Open Kern Studio: on https://kernengine.de/, click **Editor**. The very first time
> you start it, **Orb Run** is loaded (otherwise use **File › Example Levels › Orb Run**). Press
> **Play** (Alt+P), take off in the hovercar and collect orbs. **Esc** ends the game.

> **Good to know:** Studio shows its interface in English or German; if you don't choose, it
> follows your browser's language, so in an English browser you see English labels. This book names
> the labels the way Studio shows them in English, for example **File**. The **DE | EN** button at
> the bottom right of the status bar switches the language (or add `?lang=en` to the address).
> Studio remembers your choice on your device.

## 1.1 What Kern Is

Kern is written in the programming language **C++**. It's fast, but browsers don't understand it.
That's why Kern gets **translated** beforehand, into **WebAssembly**, a machine language that every
modern browser understands and runs very fast. The drawing is done by the graphics card; Kern talks
to it through **WebGL2**, the way a website gives drawing commands to the graphics card.

> **Picture it like this:** A book comes out in English. So that everyone in Germany can read it,
> somebody translates it once. After that, everyone reads the translation. In the same way, Kern is
> translated once from C++ into WebAssembly, and your browser runs the translation.

Kern grew out of the game **SpawnCity**. Even so, the engine doesn't know SpawnCity: **games talk
to the engine, never the other way around.** There's a good reason for that: if the engine knew
something about SpawnCity, it would rely on SpawnCity, and you could only build SpawnCity with it.
Because it knows nothing about any particular game, it fits under every game.

> **Picture it like this:** a building brick. It doesn't know whether you're building a ship or a
> house – and that's exactly why it fits into both. If someone had shaped it only for a ship, you
> could no longer build a house out of it.

Kern is made of these parts (the **game world** is everything that happens while you play; the
**waiter** is coming up in 1.2):

| Part | What it does | For example |
|---|---|---|
| Foundations, math | numbers, errors, positions | where the cube stands |
| Graphics | drawing pictures | houses, streets, terrain |
| Physics | collisions, joints, gravity | a crate tips over |
| Sound | mixing sounds, 3D sound | the quack gets louder as the duck comes closer |
| Game world | levels, players, rules | collecting an orb gives points |
| Logic, networking | game sequences, multiplayer | a door opens when you stand in front of it |
| Studio, Lehrer | editor and training program | building a world, training the duck |
| Waiter (JavaScript) | bridge to the browser | passing on a key press |

## 1.2 Kern's Rules

Kern follows three big rules.

**No outside motor for drawing.** Kern doesn't use anyone else's graphics engine. The code that
draws is Kern's own and talks to the graphics card directly. That way, for every picture you know
exactly where it comes from. Two building blocks from other people's free projects are built in
completely: the physics called **Box3D** ([Chapter 6](06-physics.md)) and a math library
([Chapter 3](03-foundations.md)). Kern builds in the physics **unchanged** and translates both in
exactly the same way on every computer. Besides that, Kern has taken over individual techniques
from free projects and rewritten them itself, a few calculation tricks; the list is in
[Appendix D](D-licences.md).

Also, Kern only uses features that almost every browser has. It would rather do without a few new
tricks than not run on older devices.

**JavaScript is just the waiter.** In the browser, the graphics card, keys, sounds and files can
only be reached by the programming language JavaScript. Exactly that – and only that – is what
Kern's JavaScript part does. What a game does is written in C++.

> **Picture it like this:** The waiter carries the order into the kitchen and the food to the
> table. He doesn't cook. JavaScript is the waiter, C++ is the kitchen. These roles stay the same
> throughout the whole book. And he works thriftily: for every picture he reuses the same memory
> instead of setting up new memory each time – like a waiter who always takes the same tray instead
> of fetching a new one for every course.

The advantage: you can also run C++ directly on the computer, without any browser at all.
JavaScript, on the other hand, needs the browser. Because everything a game does is written in
C++, it can also be tested without a browser (that's what [Chapter 2](02-building-and-testing.md)
shows).

**Exactly the same everywhere.** A simulation comes out the same **bit for bit** on a PC, on a Mac
and in the browser. That's called **determinism**. Why is that hard? Computers round. Divide 1 by
3, and you get 0.3333… without end. Somewhere the computer has to stop and round. If one computer
rounds a tiny bit differently at the very last digit than another one, you don't notice at first.
But a simulation calculates thousands of steps, and each one builds on the one before. So the tiny
difference builds up until, after a few seconds, the duck puts its foot down somewhere else.

> **Picture it like this:** the game of telephone. If the first person whispers just one sound a
> little differently, a completely different sentence arrives at the end of the line. Kern makes
> sure that every computer whispers exactly the same.

How Kern manages this is in [Chapter 3](03-foundations.md). How you prove it, with a
**fingerprint** of the simulation, is in [Chapter 2](02-building-and-testing.md).

Why go to all that trouble? If something goes wrong, it goes wrong exactly the same way on every
computer. Then you can make the bug happen again and again, find it and fix it.

> **Crazy but true:** Because Kern calculates so exactly, a difference between the real robot and
> the simulation is never down to chance. It shows that the computer's description of the duck is
> still not right at some point – and that's exactly what you want to know
> ([Chapter 16](16-calibration.md)).

**Four more small rules:**

- **Check first, then change.** Kern checks everything that comes from outside. A broken input
  changes nothing about what's already there.
- **No cheating.** The Micro Duck stands only thanks to the motors in its joints and its contact
  with the ground. No invisible hand holds it up. Why so strict? A duck that only stands thanks to
  an invisible hand would walk in the computer and fall over in reality. But what you learn in the
  simulation should also work for the real robot.
- **One world for everyone.** The editor, the exported game and the Lehrer use the same game
  world, the same motors and the same physics.
- **Don't fall over when the graphics card is briefly gone.** Sometimes the browser takes the
  graphics card away from a website for a moment. Then Kern pauses and afterwards rebuilds
  everything, from copies it kept itself ([Chapter 4](04-graphics.md)).

> **For pros:** Kern deliberately does without the graphics engine three.js and the translator
> Emscripten. The module runs on every browser with WebGL2, from Safari 15 on. The exact names and
> values are in the blue book, Chapter 1.

## 1.3 The Blueprint: Floors

Kern is like a high-rise: a floor may only use floors **below** it, never the ones above it. So
the graphics know nothing about game rules, but the game rules can use the graphics. That doesn't
mean every floor needs everything below it: sound, logic and networking get by with the
foundations, and the physics only borrows the description of a shape from the geometry. None of
these four can paint. Up top, in front of the building, stands the waiter; the arrows show what
goes in and out through the door.

![The Kern high-rise: up top in front of the building the waiter, inside the building the door and the floors down to the foundations](../abbildungen/architektur.svg)

The waiter is **not a floor**. He stands in front of the building at the door, carries orders in
and finished things out. The high-rise rule applies to the floors inside the building, not to him.

Not every floor is always built in. Physics, game rules, Studio and Lehrer are only added when you
ask for them. If you want Studio, you get game rules and physics automatically, because Studio
stands on top of them. Graphics, sound, logic and networking are always included.

Between C++ and JavaScript there's only **one door** for data: a memory area that both sides are
allowed to access.

> **Picture it like this:** a wall full of numbered lockers. The browser puts numbers into the
> lockers and only tells the engine: "Look from locker 1000 on." A text goes as "from locker 2000,
> so-and-so many characters." The engine puts its answers back into lockers in the same way.

Because the waiter isn't a floor, the kitchen may also call him: the engine asks the browser, for
instance, "draw this" or "what time is it?" That doesn't break the high-rise rule.

> **Watch out, trap:** Both sides have to know exactly which number lies in which locker. If you
> change the order on only one side, the other one reads nonsense. The list of all lockers is in
> [Chapter 17](17-reference.md) and in the blue book, Chapter 1.

## 1.4 Four Forms, One Code

The same C++ code of Kern is inside four different things. Depending on the purpose, different
floors are put together.

| Form | What it is | Example |
|---|---|---|
| Game fully in C++ | a game that uses Kern directly | the examples Hello and Orb Run |
| Graphics only | a website that draws with Kern | SpawnCity |
| Studio | the editor, also for finished games | your own level |
| Lehrer | the training program for the duck | a new jump network |

**A game fully in C++.** At startup, the game tells the engine what it needs, for example: "I want
shadows." After that, for every picture the screen shows, the browser asks the game: "What just
happened?" The game moves its things, and the engine draws the new picture. This round is called
the **game loop** ([Chapter 12](12-your-own-games.md)).

**Graphics only.** That's how SpawnCity uses Kern: a website loads Kern and only lets it draw
([Chapter 4](04-graphics.md)).

**Studio and exported games.** All of Studio is inside **one** single file, which even runs
without internet with a double-click. **File › Package Game (HTML)...** also turns your level into
a single file. In it, your level plays with exactly the same C++ code as in the editor
([Chapter 11](11-studio.md)).

**The Lehrer.** It trains networks for the Micro Duck: as a separate program that uses a whole
computer for calculating, or directly in the browser. If you start it twice in exactly the same
way, both times exactly the same network comes out, bit for bit. Studio loads this network just
like Pollen's networks ([Chapter 14](14-lehrer.md)).

In the Micro Duck, everything works together: physics for its limbs, motors in the joints,
networks for the brain, sound for its quack, logic for commands – and in Studio it's a thing in
the level like any other.

> **Mission:** Stop a running game with **Esc**, because the menu entries are locked while you
> play. Load **File › Example Levels › Micro Duck** and press **Play**. First you watch the duck
> walk around on its own. **F8** takes it over: **W** walks, **A**/**D** turn, **Space** quacks.

> **Mission:** From the start page, open the **Lehrer**. The Lehrer page is in German: under
> "Bewegung" (motion), pick "Stehen (Schnelltest)" (stand, quick test) and press **Start**. Watch
> the curve "Episodenlänge" (episode length). An **episode** is one attempt, and it ends as soon as
> a duck falls. If the curve rises, the ducks hold out longer – the network is learning right now.
> **Pause** stops it.

## 1.5 Where Everything Is

All of Kern's source code is **open** and lives at
https://github.com/ChristianHohlfeld/kernengine.de – anyone may look inside, learn from it, find
bugs, and use Kern for their own projects under the rules of the **license** (the rules for what
you're allowed to do with the code). And anyone can check whether what this book tells you is
really in the code that way.

> **For pros:** The engine lives in the folder `kern/`, with code, tests, examples, the
> JavaScript layer and the books inside. The license is the Apache License 2.0. The blue book,
> Chapter 1, explains the whole folder tree.

## 1.6 How to Play This Book

**Every chapter is a level.** At the top it says **Your level**, right below that **Words you
need**. It goes from the bottom up, like in the high-rise: building and foundations, graphics to
networking, Studio and your own games, Micro Duck to calibration, and at the end a
[cheat sheet](17-reference.md).

| Box | What's in it |
|---|---|
| **Mission** | a task in the real Studio |
| **Picture it like this** | a comparison from everyday life |
| **Crazy but true** | an amazing fact |
| **Watch out, trap** | a typical mistake |
| **For pros** | a look ahead for the curious |

At the end: **Level complete!** and the **Puzzle corner**.

**Your reading path.** Three bonus levels are your **reference help**: Appendix
[A](A-neural-networks.md) explains how networks learn, Appendix [B](B-rendering.md) how triangles
turn into pictures, and Appendix [C](C-physics.md) how a physics simulation thinks. You don't have
to read them first. But if you get stuck, look things up there. A good tip: read Appendix B before
[Chapter 4](04-graphics.md), Appendix C before [Chapter 6](06-physics.md) and Appendix A before
[Chapter 14](14-lehrer.md).

Want to get started right away? For your own games, [Chapter 11](11-studio.md) and
[Chapter 12](12-your-own-games.md) are your start. For the duck, read
[Chapter 13](13-micro-duck.md), [14](14-lehrer.md) and [16](16-calibration.md).

**The "Cmd" console.** In Studio, at the very bottom of the status bar, there's a small input
field labeled **Cmd**. That's the **console**: there you type short commands instead of searching
in the menu. The answer appears next to it as a message in the status bar. With ↑ and ↓ you get
earlier inputs back, **Esc** leaves the field. A few missions in this book use it.

> **Mission:** Click **Cmd** at the bottom, type `help` and press Enter: Studio shows all
> commands. Then type `tod 0.75` – the time of day jumps to sunset. With `tod 0.5` it's noon again.
> "tod" stands for *time of day*. The whole day is 1. Half of it, 0.5, is therefore 12 noon, three
> quarters, 0.75, is 6 p.m. (18:00). You type with a **period** as the decimal point: `0.75`,
> not `0,75` as people write it in Germany.

**Three books, one content.** You're reading the **yellow book**. The **green** one explains
everything simply for adults without prior knowledge, the **blue** one goes technical down to the
last detail and is the source for all three. If a topic grabs you, look it up in the blue book!

## Level complete!

- You know what an engine, Studio, the Micro Duck, a network and the Lehrer are.
- You know why the browser needs a translation of Kern.
- You know why JavaScript is just the waiter.
- You know why Kern has to calculate the same bit for bit on every computer.
- You know the high-rise: the top uses the bottom, never the other way around.
- You've played Orb Run, steered the Micro Duck and used the console.

## Puzzle corner

1. Why doesn't Kern run directly as C++ in the browser?
2. Why must the engine know nothing about a particular game?
3. **Boss puzzle:** Why does a network trained in the Lehrer run just the same in Studio?

### Solutions

1. Browsers don't understand C++. That's why Kern gets translated into WebAssembly beforehand, and
   every modern browser understands that.
2. If it knew something about one game, it would rely on it and only fit that one game. This way,
   though, it fits like a building brick under any number of games. Like in the high-rise, the game
   on top uses the engine below, never the other way around.
3. The Lehrer and Studio use the same game world, the same motors and the same physics, and Kern
   calculates the same bit for bit everywhere. So the network meets exactly the same world it
   practiced in.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
