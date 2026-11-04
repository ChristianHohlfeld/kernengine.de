# 1 Welcome to Kern!

> **In short:** Kern is a game engine that runs right in your browser – nothing to install. It
> paints 3D worlds, works out how things fall and collide, plays sounds, runs games and even
> teaches a little robot to walk. Here you get to know Kern, its rules, how it is built – and how
> to read this book.

> **Key terms:**
> - **C++ and JavaScript** – two **programming languages**, that is, precisely defined ways of
>   writing instructions for a computer, just as a knitting pattern is a way of writing down
>   stitches. C++ is very fast and is used for big programs like games; JavaScript is understood
>   by every browser out of the box.
> - **Translator** (technical term: **compiler**) – a program that turns text written in a
>   programming language into instructions a machine can carry out directly.
> - **WebAssembly** – an instruction language that all modern browsers can run. Kern is translated
>   into it from C++. The result is a **module**: a single file that the browser loads.
> - **Frame** – one of the many single pictures per second that make up movement on the screen,
>   like the pages of a flip book.
> - **Play** and **Simulate** – two buttons in Kern Studio. With **Play** you join in yourself
>   and steer the player character. With **Simulate** the world runs with its physics, and you
>   watch with a camera you can move freely.
> - **Interface** – the agreed hand-over point between two parts of a program, like the counter
>   between the kitchen and the dining room.
> - **Neural network** – a computational model that learns through practice; here it learns how
>   the robot duck should move its motors (more in [Appendix A](A-neural-networks.md)).
> - **Bit** – the smallest unit in a computer: a little box holding a 0 or a 1. All numbers are
>   made of many such boxes.

Great to have you here! Picture a workshop: on the **canvas**, pictures appear (the graphics); at
the **workbench**, things get pushed, pulled and dropped (the physics); at the **mixing desk**,
sounds are made (the audio); and on the **stage**, the game runs. In the corner, a robot duck is
learning to walk. This workshop is called **Kern**, and its doors are open at
https://kernengine.de/ – with **Kern Studio**, the editor where you build worlds and play them
right away, with **the Lehrer**, which teaches robot brains to walk, and with playable example
levels.

## 1.1 What Kern is

A **game engine** is the motor under the hood of a game: it paints, works out collisions, plays
sounds and reads the mouse and keyboard. All the game has to say is: "There's a car here, and
when you press W, it drives."

> **Imagine …** A theater. The engine is the stage machinery: lights, curtain, sound. The game is
> the play. The same machinery can carry many different plays.

Kern is written in **C++**, because C++ is fast. But the browser doesn't understand C++ directly.
That's why Kern is translated into **WebAssembly** beforehand. The result is a **module** that
the browser loads just like a picture. To paint, Kern uses the browser's **graphics interface** –
the hand-over point through which a web page is allowed to tell the graphics card what to draw.

All the parts of the workshop live in that same module:

- the **foundations**: numbers, error messages and the math for 3D space
  ([Chapter 3](03-foundations.md));
- the **painting**: light, shadows, sky, plus building blocks for houses, streets and terrain;
- the **physics**: how things fall, roll, collide and are connected by joints;
- the **audio**: sounds that come from the left, the right, near or far;
- the **game rules**: levels, things in the world, multiplayer and game logic you click
  together;
- **Kern Studio**, the editor, and **the Lehrer**, which teaches robot brains to walk and also
  compares the simulation with measurements on the real robot.

And then there's the **Micro Duck**: a small two-legged robot. It was designed by the robotics
company **Pollen Robotics**, which published its blueprints and its trained neural networks
freely. Kern uses these published blueprints and networks, but it is an independent project: it
has no connection with the company itself.

Kern grew out of the game SpawnCity. Even so, the engine doesn't know a single game. In the
theater picture: the play uses the stage machinery – it says "lights on" and "curtain up" – but
the machinery knows nothing about the play. That's why tomorrow it can carry a completely
different play just as well.

The exact names and values are in the blue book, Chapter 1.

## 1.2 The rules Kern is built by

### Home-cooked

Kern doesn't rely on anybody else's graphics engine. The painting is Kern's own code, which talks
directly to the graphics interface. The payoff: you know exactly what's going on.

It can't be done entirely without outside parts, though. For physics, Kern uses a ready-made
**physics library** called Box3D – a collection of code that someone else wrote and published
freely. Kern doesn't change it but builds it in exactly as it is. What Kern does with it, Kern
decides for itself.

> **Imagine …** A cook who stirs every sauce himself instead of using ready-made sauce. He buys a
> few ingredients too – he doesn't grind his own flour. But he takes them as they are and mixes
> them himself. That way he knows what's in every dish.

Kern also asks nothing fancy of the browser. It's like a letter in plain language: everybody
understands it. That's why Kern runs in every current browser.

### No engine logic in JavaScript

What a game does is written in **one** place: the C++ part. But in the browser, some things can
only be done by JavaScript – getting to the graphics card, reading the mouse and keyboard,
playing sound, opening files. Exactly that, and **only** that, is handled by a thin JavaScript
layer. Kern Studio sticks to this too: every click in the editor becomes a command to the engine.

> **Imagine …** The JavaScript layer is a waiter. He takes your order and brings the food to your
> table. But all the cooking happens in the kitchen – in C++.

The payoff: a game behaves in the editor exactly as it does as a finished game, and it can be
tested automatically even without a browser ([Chapter 2](02-building-and-testing.md)).

### The same result everywhere

**Determinism** means: same input, same result – **bit for bit**, whether on a PC, a Mac or in the
browser. That sounds obvious. But it isn't, because computers **round**.

A computer stores every decimal number with a fixed number of digits. Whatever doesn't fit gets
rounded off, after every single calculation step. That's why, on a computer, even the order in
which you add things up matters. Here's an example with a calculator that has only four digits:

- (1000 + 0.4) + 0.4: first you get 1000.4, rounded to 1000. Then 1000.4 again, rounded to
  **1000**.
- 1000 + (0.4 + 0.4): first you get 0.8. Then 1000.8, rounded to **1001**.

Real computers have many more digits, but the principle is the same.

For plus, minus, times and divide there is a fixed rounding rule that is the same worldwide: every
computer sticks to it. Not so for **angle calculations**. A game needs angles all the time – every
rotation, every joint of the duck is an angle. But calculations like the sine (more in
[Chapter 3](03-foundations.md)) have infinitely many digits after the decimal point and can only
be approximated. Every computer brings its own formulas for this and sometimes lands a tiny bit
somewhere else in the last bit.

A difference like that in the last digit wouldn't matter – if it stayed small. But in a
**simulation**, a world that is calculated step by step, each step keeps calculating with the
result of the one before. The error gets dragged along and grows. With a walking robot duck,
after a few seconds a foot lands differently.

> **Imagine …** Two kids roll dice to pick a hiking route, one roll at every crossroads. If just
> the first roll is different, by evening they end up in different villages.

That's why Kern keeps to three rules:

- **No arithmetic shortcuts.** The part of the computer that does the math is called the
  **processor**. Some processors compute "times, then plus" in a single go and round only once
  instead of twice – faster, but with a different last bit. The compiler is not allowed to use
  such shortcuts. The physics library also does without special instructions that only some
  processors have, and calculates the simple way everywhere, just as in the browser.
- **Its own angle formulas.** For angle calculations, Kern brings its own formulas, which
  calculate the same everywhere ([Chapter 3](03-foundations.md)).
- **A fixed order when adding up.** Remember the calculator? Order matters. When the duck learns
  to walk (with the Lehrer, Section 1.4), several helpers calculate at the same time and finish
  sometimes in this order, sometimes in that one. If you added up their results in the order they
  arrive, you'd get something different every time. That's why Kern fixes the order, no matter how
  many helpers are calculating.

The proof is a **fingerprint**: the duck walks a little way, and after every step its state is
boiled down to a check number. This list must be exactly the same on every computer
([Chapter 2](02-building-and-testing.md)).

> **Did you know?** Thanks to determinism, an error in the simulation is a fixed, repeatable error
> instead of random noise. If the simulated duck walks differently from the real one, it really
> is down to how the duck is modeled and not to the computer ([Chapter 16](16-calibration.md)).

### A few more house rules

- **Broken data is turned away before it can do damage.** Every input is checked first and only
  then accepted. If it's broken, everything stays as it was.
- **Errors are reported politely.** A task that doesn't work out answers "no" and leaves a note
  with the reason; the program keeps running ([Chapter 3](03-foundations.md)).
- **The physics isn't prettied up.** The Micro Duck stands only on its own motors and feet – no
  invisible hand holds it up ([Chapter 6](06-physics.md), [Chapter 13](13-micro-duck.md)).
- **One implementation for every place.** The editor, the finished game and the Lehrer share the
  same game world and the same physics. That's why a network that learned in the Lehrer (the
  program that teaches the duck to walk, Section 1.4) runs unchanged in Studio.
- **No disposable dishes.** The JavaScript waiter from before brings Kern new numbers every
  frame – always on the same tray, instead of fetching a new one each time and throwing it away.
  Throwing away means clearing up, and clearing up costs time in the middle of a game
  ([Chapter 3](03-foundations.md)).
- **Robust:** a browser may take the graphics card away from a page at any time – for example
  when memory runs low, when the graphics driver restarts, or on a phone when the page is in the
  background. Kern then waits and afterwards rebuilds everything ([Chapter 4](04-graphics.md)).

## 1.3 A house with floors

Kern is built in layers, like a house. Each floor uses the ones below it, never the ones above.
The advantage: you can remodel upstairs without anything collapsing downstairs, and every floor
can be tested on its own.

![Kern's floors: the browser part at the top, below it the program from the editor down to the foundation](../abbildungen/architektur.svg)

How to read the picture: each floor uses the ones below. At the very top is the browser part, the
waiter: it takes your input and passes it down through the **hand-over point** – the interface
from the key terms. "Networking" here means playing with several people over the internet, not
the duck's neural network.

Not every program needs the whole house. Painting, audio, logic, networking and the foundation are
always included. Physics, game rules, editor and Lehrer are chosen when building, so that a
program doesn't get needlessly big: a game that only paints doesn't need an editor
([Chapter 2](02-building-and-testing.md)). If you choose an upper floor, you automatically get
the ones below it: the editor and the Lehrer need the game rules, and the game rules need the
physics. There's also a small annex that the picture leaves out: a frame for games written
entirely in C++ (Section 1.4).

C++ and JavaScript talk to each other only through a shared area of memory. JavaScript puts data
there at agreed places and tells the engine where it is. The engine answers the same way. When
it needs something from the browser in turn – the graphics card, the time, a random number – it
calls the browser through a short list of agreed requests.

> **Imagine …** Two neighbors without a common language share a mailbox. One puts notes in, the
> other picks them up. On top of that there's a doorbell with a few agreed signals: the engine is
> allowed to ring and ask for, say, the time or a random number. There's no other contact.

The exact names and values are in the blue book, Chapter 1; all the functions at this boundary
are listed in [Chapter 17](17-reference.md).

## 1.4 Four ways to use Kern

All four ways are made of the same C++ code, just put together differently.

**A game entirely in C++.** For programmers: you write a small C++ program that sets up the world
at the start and then, every frame, says what moves; Kern paints. The smallest example puts a
field of spinning crates on a floor. The big example is a whole game with a city, driving
physics, traffic, day and night ([Chapter 12](12-your-own-games.md)).

**Only the painting, for a web page.** Web pages can also use Kern just as a painter; that's what
the game SpawnCity does ([Chapter 4](04-graphics.md)).

**Kern Studio and finished games.** For everyone who'd rather click than type. The whole of Studio
is **one** file, which runs with a double-click, even without internet. You build a level out of
**Actors** (things in the world) and try it out right in the window: with **Play** you join in
yourself, with **Simulate** you watch the running world. **File › Package Game (HTML)...** turns
it into a standalone game, again a single file that you can send to people. It runs on the same
engine as the editor and therefore behaves the same way ([Chapter 8](08-game-runtime.md),
[Chapter 11](11-studio.md)).

> **Try it!** Open Kern Studio at https://kernengine.de/. On first start, Orb Run is loaded
> (otherwise: **File › Example Levels › Orb Run**). Press **Play** (Alt+P) and head off in the
> floating car, the hovercar: **W A S D** or the arrow keys steer and accelerate, **Shift** gives
> a boost. Collect a few **orbs**, the glowing energy balls on the streets. Then **Stop** or Esc.
> Everything you just saw and heard – light, driving physics, sound – was calculated by the
> engine in the browser.

> **Did you know?** Kern Studio shows its interface in German or English; unless you choose, it
> follows the browser's language, so in an English browser you see English labels. This book
> names the labels as Studio shows them in English. The **DE | EN** button at the bottom right of
> the status bar switches the language; Studio remembers the choice on your device. Three things
> are always in English: the logic editor ([Chapter 9](09-logic.md)), the messages in the output
> log and the console commands.

**The Lehrer.** It trains networks for the Micro Duck: on your computer as a program of its own,
or in the browser on the Lehrer page. There, several background helpers gather experience with
simulated ducks at the same time, and one learns from it. Both versions use the same training
recipes and the same game world. Learning starts with a **seed** for chance – like a deck of
cards that is shuffled in exactly the same way every time. Same seed, same number of ducks:
**the same network**, here as there. The Lehrer saves a finished network in the same format as
Pollen Robotics' original networks; that's why Studio loads it just like those
([Chapter 14](14-lehrer.md)).

The duck shows you everything at once:

- Its **body** is modeled on Pollen Robotics' original blueprints: physics parts connected by
  joints.
- Its **motors** are reproduced exactly, right down to their strength and their small delay.
- Its **brain** is a neural network that Kern calculates itself.
- Its **voice** comes from the sound mixer.
- **Game logic** you click together can control it.
- In Studio it is simply an **Actor** like any other thing.

![The Micro Duck template in Kern Studio: the duck in a walled room with crates and a ball](../buch/bilder/duck-level.png)

> **Try it!** Choose **File › New Level...** and pick one of the six templates there, for example
> **Micro Duck**. Press **Play** and watch the duck set off, steered by its trained brain. Nobody
> wrote out its steps for it: at every moment the network decides afresh how the motors move.

### A tool for later: the console

At the very bottom of Studio is the **status bar**. In it there's an input field labeled **Cmd**
(short for "command"): the **console**. Here you type commands to Studio and send them with
Enter; `help` shows all commands. The answers appear in the **Output Log**, which you open with
the button of the same name in the status bar. The field has nothing to do with the Mac's Cmd
key. You'll use it more often in the coming chapters.

## 1.5 Where things are

Kern's entire blueprint is open for anyone to see:
https://github.com/ChristianHohlfeld/kernengine.de. Kern is under the **Apache License 2.0**, a
license that allows free use; other people's components are listed with their own licenses.

The exact folder map is in the blue book, Chapter 1.

## 1.6 How to read this book

You are reading the **green book**. Its big brother, the **blue book**, has the same content
chapter for chapter, but technical right down to every setting. If a topic grabs you, look it up
there.

Along the way you'll keep meeting the same boxes: **In short**, **Key terms** (the words the
chapter needs), **Imagine …** (an everyday comparison), **Try it!** (a task in Kern Studio),
**Did you know?**, **Watch out!** (a typical mistake) and **For the curious** (a technical detail
you can skip). At the end of each chapter come **What you take away** and the **Brain teasers**,
with the solutions right below. No peeking! (Well, okay, a little.)

The book is built from the bottom up, just like Kern itself: first building and foundations, then
graphics, physics, audio, game, logic and networking, then Studio, the Micro Duck and the Lehrer,
and finally the [reference](17-reference.md). Appendices explain neural networks
([A](A-neural-networks.md)), real-time graphics ([B](B-rendering.md)) and physics
([C](C-physics.md)).

Where do you start?

- **You simply want to understand how games and learning robots work, with no technical
  background?** Then after this chapter read [Appendix B](B-rendering.md) and
  [Chapter 4](04-graphics.md) (how pictures are made), then [Appendix C](C-physics.md) and
  [Chapter 6](06-physics.md) (how things fall and collide), then
  [Appendix A](A-neural-networks.md), [Chapter 13](13-micro-duck.md) and
  [Chapter 14](14-lehrer.md) (how the duck learns to walk). In between, it's worth reading
  [Chapter 11](11-studio.md) so that you can try everything out yourself in Studio. You can skim
  Chapters 2 and 3 and look things up there when you need to.
- **You want to build a game?** Chapter 11 (Studio) or 12 (C++), plus 8 and 9.
- **You love the duck?** Chapters 13, 14 and 16.
- **You want to change Kern?** Chapters 2 and 3, and before physics or the robot also 6 and 13.
- **You want to build Kern into a web page?** Chapters 4 and 17.

## What you take away

- Kern is a game engine that runs right in the browser – nothing to install.
- Kern is written in C++ and translated into WebAssembly for the browser.
- Painting, physics, audio, game rules, editor and Lehrer all live in the same module.
- JavaScript is only the waiter: the cooking happens in C++.
- Computers round; small differences build up. That's why Kern calculates the same everywhere, bit
  for bit – a fingerprint proves it.
- The same code gives you C++ games, painting for web pages, Studio and the Lehrer.
- The green book explains things simply; the blue book goes into depth.

## Brain teasers

1. Why is it handy that there's no engine logic in JavaScript?
2. A network was trained in the browser. Why does it run exactly the same in Studio?
3. Work it out with the four-digit calculator: does (1000 + 0.3) + 0.3 give the same as
   1000 + (0.3 + 0.3)? What does that tell you about simulations?
4. What happens if the browser takes the graphics card away from Kern in the middle of a game?

### Solutions

1. Everything a game does is in one place, in C++: testable without a browser, and the same in
   the finished game as in the editor.
2. The Lehrer and Studio share the game world, motors and physics, and Kern calculates the same
   everywhere.
3. No: the left side gives 1000 (1000.3 is rounded down to 1000 twice), the right side 1001 (adding
   0.6 gives 1000.6, rounded to 1001). On a computer, order matters – and in a simulation, such a
   difference grows with every step.
4. The browser is allowed to do that, for example when memory runs low or the graphics driver
   restarts. Kern waits until the graphics card is back and then rebuilds everything.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
