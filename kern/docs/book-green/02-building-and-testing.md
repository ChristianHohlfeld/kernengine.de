# 2 Building and Testing: the Workshop Behind Kern

> **In short:** Three principles stand behind Kern. First: the code is **translated** before it
> can run in the browser. Second: hundreds of **automatic checks** guard every change. Third: a
> **fingerprint** proves that Kern calculates exactly the same everywhere. An assembly line takes
> care of all three by itself with every change.

> **Key terms:**
> - **Source code** – the text of a program as people write it, here in C++.
> - **Header** – a shared agreement file in the source code that many parts of the program read
>   along, like house rules posted in every apartment.
> - **Processor** – the part of the computer that actually carries out the instructions.
> - **Memory** – a program's workspace in the computer, a long row of numbered compartments for
>   numbers.
> - **Simulation** – a world that is calculated: the computer works out step by step how things
>   move.
> - **Tick** – a single calculation step of the simulation. The duck decides afresh in every tick
>   how to move its motors.
> - **Server** – a computer on the internet that does jobs for others.
> - **GitHub** – a website where Kern's source code lives and where every change is recorded.

Picture a workshop with three stations: at the first, things get cooked (translated); at the
second, they get tasted (checked); at the third, the result is compared with a sample
(fingerprint). At the end comes delivery: the website https://kernengine.de/. This chapter goes
through the stations one by one.

## 2.1 Translating: from recipe to dish

A program is written in a language for people. Before a browser can run it, it has to be
translated into the browser's instructions. For Kern, that means: the C++ source code becomes a
WebAssembly **module**, a single file that the browser loads.

> **Imagine …** The source code is the recipe, the **build script** – a small program that steers
> the translating – is the cook, and the module is the finished dish. When you order, you say
> which courses you want: just the basics (painting, audio, logic, networking), or physics, game
> rules, the editor or the Lehrer on top.

The cook has two fixed house rules. First: **no arithmetic shortcuts.** Some processors can merge
two calculation steps into one and round differently from others while doing it. That's
forbidden, so that Kern calculates the same everywhere ([Chapter 1](01-introduction.md)).
Second: **serve only what was ordered.** The cook puts nothing on the plate that nobody eats –
that keeps the module small and makes it load faster.

### The pantry

Kern consists of many dozens of pieces of source code. Each is translated separately, many at the
same time, and at the end they're joined into one module. So that not everything gets cooked
from scratch with every build, each translated piece goes into a **pantry**. It carries a label,
a kind of fingerprint of its recipe and of **all** the headers: if even a single character
changes in them, the label looks completely different (how such a fingerprint is made is shown
in [Chapter 3](03-foundations.md)). If the label still matches, the piece is taken out of the
pantry instead of being cooked again.

> **Did you know?** If someone changes a single comma in a header, **everything** gets translated
> again – on purpose. Every piece reads the headers along, like every apartment reads the house
> rules. Finding out which pieces a change really affects would be tedious and error-prone; an
> overlooked piece would stay in the pantry, out of date. So Kern would rather cook everything
> again. That costs time, but it's simple and safe.

### Studio in a suitcase

A second script packs Kern Studio into **one single** HTML file – the kind of file web pages are
made of and that a browser opens with a double-click. Into it go the module, the JavaScript layer
and the look of the page, all in one suitcase. A web page file like that consists only of letters
and symbols, but the module consists of raw numbers. That's why it is disguised as a long string
of letters, a bit like Morse code disguises letters as dots and dashes. And that's why Studio
runs with a double-click, without internet and without installing anything.

The suitcase also holds the parts for finished games: the browser parts a game needs to run,
packed once more separately. Together with the module, which is in the suitcase anyway,
**File › Package Game (HTML)...** can therefore pack a standalone game even without internet.

### The Lehrer without a screen

The Lehrer can also be built as a separate, fast program for your own computer. It doesn't need a
screen, because training doesn't have to paint anything. That's why it even runs on a server
without a monitor. How to use it is shown in [Chapter 14](14-lehrer.md).

For your own computer, the compiler may pick the fastest instructions that exactly this processor
knows. Doesn't that contradict Chapter 1? No: what's forbidden there are only instructions that
**round differently** – such as "times and plus in one go". Allowed are instructions that do
**the same** calculation faster, for example several identical calculations side by side in one
go, each one exactly as before. That way you get the same result bit for bit – only faster.

The exact names and values are in the blue book, Chapter 2.

## 2.2 Automatic checking: hundreds of little inspections

A **test** is a small program that tries out a piece of Kern and checks whether the right thing
comes out. Kern has well over a hundred of them, each with many individual checks, and they run
with every change. Whoever breaks something finds out right away – not only when a player
stumbles on it.

Here's how checks sound in everyday language:

- Does Studio start with Orb Run?
- Does **Undo** undo a change exactly?
- Does Kern calculate the duck's network exactly like Pollen Robotics' original?
- Does the duck stand, walk and turn when its network steers it?

Every check reports "ok" or "FAIL". A single FAIL is enough, and the whole run counts as failed.

### Sniffer dogs

For testing, Kern's own code is translated with built-in **sniffer dogs**. One barks when a part
of the program reaches into a memory compartment that doesn't belong to it. The other barks when
the code does something the rules of C++ forbid – such as a calculation whose result gets too big
for its number box. Both stop immediately and name the spot.

Outside code, such as the physics library, runs without sniffer dogs. The reason: Kern doesn't
change outside code but builds it in as it is ([Chapter 1](01-introduction.md)). So whatever the
sniffer dogs found there, Kern couldn't fix by itself anyway. The search happens where Kern can
also repair things: in its own code.

The tests are also stricter than the normal build. Normally, a compiler **warning** is just a
hint: "Something looks suspicious here, but I'll translate it anyway." When testing, every
warning already counts as an error.

### The dummy

On the test computer there's no graphics card that a browser could drive. That's why a **dummy**
stands in for the graphics interface: it doesn't paint anything, but it writes down exactly what
Kern wanted from it.

> **Imagine …** A pilot practices in a flight simulator. No plane takes off, but every press of a
> button is recorded. Afterwards you can see exactly: did the landing gear come up in time?

That way the tests check, without a graphics card, whether the painting turns away broken input,
reuses things it has already calculated, and survives losing the graphics card.

### Game rules: three ways, one result

Game rules that you click together in Studio as little boxes and arrows ([Chapter 9](09-logic.md))
can be run by Kern in three ways. All three must give exactly the same result, and shared test
cases check exactly that.

> **Imagine …** A book in a foreign language. An **interpreter** reads it to you sentence by
> sentence (slow, but ready right away). Or someone **translates** it once, completely, and then
> you read it fluently. Or it appears as a fixed part of another book. Whoever reads it must get
> the same story every time.

In Kern that means ([Chapter 9](09-logic.md) explains it in more detail):

- **Interpreter:** Kern goes through the boxes one after another and does what they say.
- **Translation:** when **Play** or **Simulate** starts, Kern translates the boxes into fast
  machine instructions that the browser runs directly.
- **C++ export:** Kern writes finished C++ source code from the boxes, which can be built
  permanently into a program.

The fast way in the game is the translation. If it ever doesn't work, Kern simply keeps
interpreting – slower, but nothing breaks.

> **Did you know?** At the end of this test there's a race: game rules with a million
> repetitions, once interpreted, once translated. The output shows how much faster the
> translation is.

### The test clicker

The last check is the biggest: a remote-controlled browser operates Studio like a tireless human.
It opens menus, moves things, presses **Play**, packs a game and plays it, and it even pretends
to be a phone. It doesn't get a real graphics card, but an emulated one that manages Orb Run's
city at only about **one frame per second**. That's why every check waits until its effect is
visible, instead of waiting for a fixed time.

Whether anything was painted at all is revealed by a simple trick: the test clicker counts how
many **different colors** appear in the picture. An empty picture has hardly any colors, a
painted one has lots. It doesn't need to know exactly what the picture should look like – the
question "Was anything painted at all?" is answered by the number of colors alone, quickly and
even with the slow stand-in graphics. Along the way it can take screenshots – many pictures in
this book come from there.

![Simulate in the Physics Lab with collision shapes shown – this is how the test clicker operates Studio too](../buch/bilder/studio-simulate.png)

> **Try it!** Do what the test clicker does. Choose **File › New Level...** and pick **Physics
> Lab** there. Hold down the right mouse button and press W: now your **camera** flies forward,
> while the things in the world stay where they are. Then **Simulate** (Alt+S), that is, watch
> instead of playing along: the physics calculates, the pendulum swings, the turntable turns, the
> rover drives off. Imagine having to do that a hundred times after every change and looking
> closely each time – that's exactly what the test clicker does for you, without ever getting
> tired.

The exact names and values are in the blue book, Chapter 2.

## 2.3 The fingerprint: the same result everywhere

How do you prove that Kern calculates **exactly** the same on every computer? You let all the
computers do the same thing and compare – but not just the final result, every single step.

For that you need a **checksum**: a number that is calculated from lots of data and changes
completely as soon as even the tiniest thing changes – like a fingerprint. The same data always
gives the same number.

The test goes like this: the Micro Duck walks straight ahead a little way in Studio's physics and
then turns on the spot, steered by a network from Pollen Robotics. After **every** tick, its whole
state – where it stands, how it's rotated, how fast it's moving, how every joint is positioned –
becomes a checksum. 400 ticks give a list of 400 checksums. On a Linux PC, on a Mac with an Apple
chip and in the WebAssembly module that the browser also uses, this list must match a stored
reference **line by line**.

> **Imagine …** You photograph the duck 400 times and compare every photo with a reference album.
> If a feather shifts by a hair, you even know which photo it's on.

If a photo differs, there are only two explanations: either the simulation was changed on
purpose – then the reference has to be regenerated and the change explained. Or somewhere, code
calculates differently on different computers, for example because it uses the formulas for an
angle that the computer brings along by itself instead of Kern's own
([Chapter 3](03-foundations.md)).

A second test checks the learning: it trains a network with one, two and four helpers, who
together always look after eight ducks. All three networks must be the same down to the last bit.
That's harder than it sounds: four helpers finish in a different order than two. If you added up
their results in the order they arrive, you'd get – just like with the four-digit calculator from
[Chapter 1](01-introduction.md) – something different. That's why Kern always adds up in the
same, firmly agreed order.

> **For the curious:** The checksum is called FNV-1a and is 16 characters long. If you build the
> Lehrer on your own computer, you can generate the fingerprint yourself and compare it with the
> reference – how is in the blue book, Chapter 2.

## 2.4 The assembly line: the automatic test track

Nobody has to remember to start the checks. With every change on GitHub, an **assembly line**
starts up, called **CI** in English (*continuous integration*, "constantly merging"). GitHub's
servers then do everything by themselves: translate, check, compare the fingerprint, publish.

![The assembly line: every change starts three stations; after building come publishing and the test clicker](../abbildungen/ci-pipeline.svg)

The stations:

- **Tests with sniffer dogs:** all the checks from Section 2.2 on a Linux computer.
- **Fingerprint on the Mac:** the Lehrer is built on a Mac with an Apple chip and must move the
  duck exactly like the others.
- **Build and basic checks:** builds Studio, the example games and the Lehrer, runs the most
  important required checks and puts together the finished website.
- **Publish:** puts the website on https://kernengine.de/.
- **Test clicker:** operates Studio with exactly the files that get published.

What matters is what **waits** and what doesn't. Publishing happens as soon as the "Build and
basic checks" station is done. If a check fails there, nothing goes online. The other three test
stations – tests with sniffer dogs, fingerprint on the Mac and test clicker – on the other hand
run **alongside**: publishing doesn't wait for them. (The test clicker does need the finished
site and therefore only starts after building, but then at the same time as publishing.) If one
of them fails, the new version may already be online – but on GitHub a red mark lights up right
away, and the next change has to fix the error.

> **Imagine …** A bakery. Before a loaf goes into the shop, it has to be baked through – that's
> the requirement at the baking station. The tasting by the master baker runs at the same time as
> the selling. If something doesn't taste right to her, it gets fixed right away, but the bread on
> the shelf stays there.

The advantage: every change is online quickly. The price: an error that only the tests running
alongside find can be visible for a short while.

> **Try it!** In Studio, the status bar at the very bottom shows the **version**, for example
> "v 1af26d8": the short code of the change from which the assembly line built exactly this
> version. If you reload the page a few days later, there may already be a different code –
> then a new change has gone through the assembly line and gone online in the meantime.

> **Did you know?** Kern Studio can be installed on your computer like an app. The published
> Studio page, whether in a browser tab or as an installed app, notices by itself when the
> assembly line has published a new version: next to every version there's a small file with its
> ID. The page compares regularly and reloads itself as soon as you're not playing.

How to recreate the whole assembly line at home and what typical error messages mean is in the
blue book, Chapter 2.

## What you take away

- Code is translated before it runs in the browser; the build chooses which parts come along.
- A pantry with labels saves time; after a header change, everything is deliberately translated
  again.
- Well over a hundred tests check every change. Sniffer dogs find forbidden memory accesses, a
  dummy stands in for the graphics card, a test clicker operates Studio like a human.
- Game rules must give the same result on all three ways.
- The fingerprint compares 400 ticks of the duck on PC, Mac and in the browser module.
- The assembly line checks and publishes every change; only the build station holds up
  publishing, the other tests run alongside.

## Brain teasers

1. Why do the sniffer dogs only run over Kern's own code, not over outside code it has taken on?
2. You change a header. What happens at the next build, and why?
3. Why is the number of colors enough for the test clicker to check whether anything was painted?
4. The fingerprint matches up to tick 211 and differs from tick 212 on. What does that tell you?
5. The tests with sniffer dogs fail. Has the website been updated anyway?

### Solutions

1. Kern doesn't change outside code. Whatever the sniffer dogs found there, Kern couldn't fix by
   itself – so you search where Kern can also repair things.
2. Everything is translated again. Every piece reads the headers along, so their content is in
   every label. Instead of tediously finding out which piece is affected, Kern cooks everything
   again – that way no out-of-date piece is left in the pantry.
3. An empty picture has hardly any colors, a painted one has lots. For that, the test clicker
   doesn't need to know exactly what the picture should look like, and it works even with the
   slow stand-in graphics.
4. At tick 212, something different comes out for the first time. That's where the search for the
   error begins, for example at an angle calculation that doesn't use Kern's own formula.
5. Yes, if the build station succeeded: these tests run alongside and don't hold up publishing.
   But the red mark shows right away that something needs fixing.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
