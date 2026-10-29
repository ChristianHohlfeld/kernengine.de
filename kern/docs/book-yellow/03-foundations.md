# 3 The Foundation

> **Your level:** Every high-rise needs a foundation. Here you learn the ideas all of Kern stands
> on: how a computer stores and rounds numbers, how it reports errors, how it writes data down as
> text, how it calculates in space – and why it has to calculate exactly the same on every
> computer.

> **Words you need:**
> - **Function** – a small named piece of a program: it gets values, calculates and gives back a
>   result.
> - **Library** – a collection of ready-made functions that many programs share, for example for
>   math.
> - **Data** – all the numbers and texts a program works with.
> - **Renderer** – the part of the engine that draws the pictures.
> - **Browser console** – a hidden window in the browser where websites leave messages. In many
>   browsers it opens with F12.
> - **File** – a saved piece of data with a name, such as a photo or a level.
> - **Servo** – one of the small motors in the Micro Duck's joints. Its **profile** describes in a
>   few numbers how strong, how stiff and how fast it is.

## 3.1 Numbers in the Computer

A computer stores every number in a fixed number of **bits**. More bits means: bigger numbers or
more digits. For every number, Kern writes down how many bits it has and whether it may have a
sign.

> **Picture it like this:** a combination lock whose little wheels only know 0 and 1. With one
> wheel there are 2 possibilities, with two wheels 2 · 2 = 4, with eight wheels
> 2 · 2 · 2 · 2 · 2 · 2 · 2 · 2 = 256. So a number made of 8 bits can have the values 0 to 255.

Kern uses numbers with 8, 16, 32 or 64 bits. Why is the exact width so important? At the door
between C++ and JavaScript ([Chapter 1](01-introduction.md)), both sides have to know how big each
locker is. If one side reads 32 bits where the other put down 64, nonsense comes out.

**Decimal numbers get rounded.** Decimal numbers also only have a fixed number of bits. A decimal
number with 32 bits has about 7 digits, one with 64 bits about 16. Everything after that gets
rounded. So 1 : 3 doesn't give 0.3333… without end, but for example 0.3333333. That's why almost
every calculation with decimal numbers ends with a small rounding step. That's not a mistake, it's
just how it is. It becomes important in 3.9.

> **For pros:** Kern names the number types by kind and width, such as "u8" (whole number without
> sign, 8 bits) or "f64" (decimal number, 64 bits, exactly JavaScript's number type). All the types
> are in the blue book, Chapter 3.

## 3.2 Reporting Errors Cleanly

### Errors Without Drama

Some things can go wrong: a broken file, an impossible camera, an effect the graphics card can't
do. Many programs then throw out an "alarm," a so-called **exception**, that flies right through
the program until someone catches it – or until the program crashes. Kern never does that. In
Kern, a function that can fail simply answers **"didn't work"** and first leaves a note with the
reason.

> **Picture it like this:** You ask your brother to get milk. He comes back and says: "Didn't
> work – the store was closed." No shouting, but you know what's going on and can decide what to do
> now.

An example: when you click a thing, Kern first checks whether the camera is valid, because Kern
has to work out where you're pointing through the camera. If it isn't, Kern writes "invalid
camera" on the note and reports "didn't work." Anyone who wants to know why reads the note – from
JavaScript too.

> **For pros:** There's only **one** note for everyone. Every new error message overwrites the
> previous one. If you want to be sure to read the message for your own call, you wipe the note
> clean beforehand. The function names are in the blue book, Chapter 3.

### The Not-a-Number: NaN

If a computer calculates 0 divided by 0, no number comes out but **NaN** ("Not a Number"). NaN has
a crazy property: **every comparison with NaN is false.** "NaN is greater than 0" – false. "NaN is
less than or equal to 0" – also false.

That's a trap. Suppose a quantity x has to be positive, and Kern wants to catch everything
invalid. Work through both checks with x = NaN:

| Check | with x = NaN | Error noticed? |
|---|---|---|
| "Is x less than or equal to 0?" | false | no, NaN slips through |
| "Is x **not** greater than 0?" | "greater than 0" is false, "not false" is true | yes |

That's why Kern often asks "Is x not greater than 0?" instead of "Is x less than or equal to 0?"
For normal numbers both mean the same. Only the second question also catches NaN.

### Writing Down Messages

Kern writes down messages, from harmless hints to errors. In the browser they end up in the
browser console, with "[kern]" in front. An example: if the graphics card can't do the glow effect
around bright lights (**bloom**), Kern switches it off, writes down a warning with the reason and
carries on without it. The picture is plainer then, but the game runs.

## 3.3 The Clock Stays Outside

Kern can read the clock, accurate to the thousandth of a second. It needs that for **measuring**:
how long does a picture take? How much time is left to prepare sounds? But the time of day never
belongs in the **simulation**. A simulation counts steps, not seconds on the clock.

> **Picture it like this:** a flip book. Every page is a step. Whether you flip fast or slowly,
> the story on the pages stays the same.

Why is that so important? Imagine the simulation goes by the clock. A fast PC manages many small
steps in a second, an old tablet only a few big ones. Every step ends with a small rounding step
(3.1), so both round a different number of times at different places. After a few seconds, the
duck stands somewhere else on the PC than on the tablet, and the fingerprint from
[Chapter 2](02-building-and-testing.md) would be broken.

That's why Kern counts fixed steps. The Micro Duck's physics, for example, calculates 100 fixed
steps per second of simulation time. If a slow computer needs two real seconds for these 100
steps, the duck moves across the screen more slowly, but it takes exactly the same steps.

## 3.4 Borrowing and Recognizing

### The Span: the Shop Window

Copying data takes time, especially when it's thousands of numbers, such as the corners of a 3D
model. That's why you often don't give the engine a copy, just a pointer: "The numbers are over
there, and there are so-and-so many." Kern calls this pointer a **span**. A span doesn't **own**
the data, it only points to it. All data flows into the engine in this format, from C++ as well as
from JavaScript.

> **Picture it like this:** A span is a shop window: you see the goods, but they don't belong to
> you. That's much faster than buying everything. But if the store clears out, the window is empty.

That leads to a rule: a span may never be used longer than its data lives. Anyone who wants to keep
data copies it. That's why the renderer keeps its own copies of everything it draws. With them, it
finds out what's under the mouse, and it rebuilds everything when the browser has briefly taken the
graphics card away from it ([Chapter 1](01-introduction.md), [Chapter 4](04-graphics.md)).

### Hashes: the Code Number

You already know a **hash** from [Chapter 2](02-building-and-testing.md): a code number made from
lots of data. Instead of comparing 1000 numbers one by one, you compare two code numbers. If
they're different, something has changed. That's how the renderer recognizes, for example, that it
doesn't need to send a value to the graphics card again at all.

A second kind of hash mixes a single number up thoroughly. With it, the game makes "randomness"
that looks random but comes out the same every time it starts. A sound then doesn't sound exactly
the same every time, for example, but sometimes a bit higher, sometimes a bit lower. Why should the
randomness be the same every time? Real randomness would come out differently on every computer
and at every start. Then a game would never run the same way twice, and you couldn't repeat a bug
to find it ([Chapter 1](01-introduction.md)). Both kinds calculate only with whole numbers, so
without rounding: the same data gives the same code number on every computer.

## 3.5 Data as JSON

Levels, Studio commands, servo settings: Kern writes all of this down as **JSON**. JSON is a
simple way of storing data as text that people and programs can read. This is roughly what a cube
in a level looks like:

```json
{
  "name": "Cube",
  "transform": {
    "position": [0, 0, 1],
    "rotation": [0, 0, 90]
  }
}
```

Inside curly braces are pairs of **name: value**. A value can be a number, a text in quotation
marks, a list in square brackets, or another block of pairs. "position" is the place, "rotation"
the turning, and "transform" combines both, so it's the placement of the thing. Here the actor is
called "Cube." It's rotated by 90° around the vertical axis (the third number in "rotation"), and
it stands 1 meter above the ground, because in "position" the third number is the height. Why the
third one, of all things, is shown in 3.6.

> **Picture it like this:** a filled-out profile card. Every line has a heading and an entry. A
> profile card can even contain a smaller profile card.

> **Mission:** Save a level with **Ctrl+S** (on a Mac **Cmd+S**). The first time, the browser asks
> you for a location, or the file lands in your Downloads folder. Your computer doesn't know the
> ending ".klevel," so double-clicking won't help. Instead, right-click the file, choose **Open
> with** and then a simple text editor (on Windows "Notepad," on a Mac "TextEdit"). It's JSON! Find
> `"format": "kern-level"` at the very top – this line tells every program: "I'm a Kern level
> file." Then look for the rotations in degrees in your actors.

Kern's JSON building block promises three things:

1. **The order stays.** If Kern reads in a file and writes it back out unchanged, exactly the same
   file comes out, character for character. If you change a level, comparing the old and new file
   shows only the lines that really changed.
2. **Numbers like in the browser.** Kern writes every number exactly the way JavaScript does. So
   the same number gives the same text on both sides. What for? If JavaScript wrote a file and
   Kern reads it in and saves it again, the same file comes out, character for character. So
   promise 1 holds for numbers too.
3. **Safe against nasty files.** A file from the internet can be broken or built to be mean on
   purpose. Kern rejects, for example, profile cards that are nested too deeply inside each other,
   like a Russian doll with hundreds of dolls, and numbers that would be infinitely large.

If an entry is missing, Kern gets "nothing" back instead of a crash.

> **For pros:** Kern writes very large numbers like JavaScript does: 10²⁰ written out in full, with
> 20 zeros, but 10²¹ as "1e+21." The exact depth limit and the forbidden names that could otherwise
> be used to attack JavaScript pages are in the blue book, Chapter 3.

### Check First, Then Take Over

This is one of the most important principles in Kern: **First everything gets checked, then it
gets taken over – all or nothing.**

An example: Kern loads the profile of a servo (from the word box) from a file, let's say five
values. Kern reads value 1 and checks it: is it a number, is it within the allowed range? Then
value 2, value 3. Value 4 is broken. Now Kern stops and reports the reason. The old servo profile
stays **completely** untouched. Only when all five values have been checked does Kern replace the
old profile with the new one in one go.

> **Picture it like this:** cooking from a recipe. You first check whether all the ingredients are
> there, and only then do you start. Otherwise you're standing there with half-mixed batter, and
> the egg is missing.

Why so strict? A half-taken-over profile – three new values, two old ones – would be a mix nobody
wanted. In the same way, the JSON reader only delivers its result if the whole file was okay, and
its error message names the spot where it gets stuck.

## 3.6 Positions and Directions as Arrows

### Kern's Rules for Space

| Rule | Setting |
|---|---|
| Units | meters, seconds, kilograms |
| Up | **+Z** |
| Axes | "right-handed" |

**Z points up**, everywhere: in the graphics, the physics, the sound and the levels. X and Y lie
flat on the ground like the coordinate system in your notebook.

You can check "right-handed" with your right hand: stretch your thumb to the right, your index
finger forward and your middle finger up. Thumb is X, index finger Y, middle finger Z. With your
left hand this doesn't work – there the middle finger points down.

> **Mission:** Choose **File › New Level...**, the template **Basic** and **Create**. Through the
> **Content Drawer** – the button at the far left of the status bar – place a **Cube**. At the
> bottom left, the axis cross shows X red, Y green, Z blue – pointing up. Drag the blue arrow with
> the tool **Move (W)** and watch the field **Location** Z in **Details**.

### Vectors: Arrows Made of Numbers

A **vector** is an arrow. In the plane, two numbers describe it: how far to the right, how far
forward. In space, a third one is added: how far up. With vectors, Kern describes positions (an
arrow from the origin (0, 0, 0) to the point), directions and speeds.

Draw an arrow in your notebook from (0, 0) to (3, 4): 3 squares to the right, 4 up. How long is
it? Put your ruler on it and measure in squares: exactly **5**. You can also calculate it, with the
Pythagorean theorem, which you usually learn in grade 8 or 9: 3 · 3 + 4 · 4 = 9 + 16 = 25, and 25
is 5 · 5. For now it's enough to know: the length of an arrow can be calculated from its numbers,
and Kern does that all the time.

> **For pros:** In space it works just the same, only with three numbers. The arrow (2, 3, 6) has
> 2 · 2 + 3 · 3 + 6 · 6 = 4 + 9 + 36 = 49, and 49 is 7 · 7. So it's **7** meters long.

Often you only need the **direction** of an arrow, not its length. Then you divide every number by
the length: (3, 4) divided by 5 gives (0.6, 0.8). The arrow still points the same way, but it's
exactly 1 long.

> **Watch out, trap:** The arrow (0, 0, 0) has length 0 and no direction at all. If you divide by
> 0, NaN comes out (3.2), and that would eat its way through the whole scene. It happens quickly:
> an arrow from a point to exactly the same point has length 0, for example when someone clicks the
> same point twice. In that case, Kern returns a substitute arrow pointing up.

More arrow calculations, such as distances or mixing two arrows, are in the blue book, Chapter 3.

## 3.7 Turning

### Degrees and Radians

In Studio and in the level files, you measure angles in **degrees**: a whole turn has 360°. In
the C++ code, on the other hand, Kern calculates in **radians**. Radians means: how far do you
travel along a circle with a 1-meter radius? A whole turn is the whole circumference. The
circumference of a circle is 2 · π · radius, where the number π is about 3.14. With a 1-meter
radius, that's about 6.28 meters. Half a turn (180°) is about 3.14, a quarter turn (90°) about
1.57. Radians are handier for math formulas, degrees for people. That's why Kern converts between
the two.

> **Mission:** In **Details**, under **Rotation** Z of your cube, type `45*2` and press Enter.
> Calculations are allowed – the cube turns by 90°.

### Rotations in Four Numbers

Every rotation in space can be described like this: **around this axis, by so many degrees.**

> **Picture it like this:** Stick a skewer through an apple. The skewer is the axis. Now you turn
> the apple around the skewer – by 30°, by 90°, however you like. Every rotation there is, is a
> skewer like that plus an angle.

Kern cleverly packs axis and angle into **four numbers**. This packaging is called a
**quaternion**. You don't need the math behind it. What matters is why Kern uses it: you can
cleanly combine two rotations into a single one with it.

With two rotations one after the other, the **order** matters. Try it with a book on the table:

- First turn it 90° around the vertical axis, then tip it 90° forward.
- Now do it all the other way around: first tip, then turn.

The book ends up lying differently! That's why Kern sets exactly which rotation takes effect
first.

> **For pros:** The four numbers are **not** simply three numbers for the axis plus one for the
> angle. They're converted from those with sine and cosine (grade 9 or 10). That's why "no
> rotation" looks unusual: (0, 0, 0, 1). The math is in the blue book, Chapter 3.

### Matrices: Everything in One Go

A thing in the level is often **stretched**, **rotated** and **moved**. Kern packs all three
motions into a table of 4 × 4 = 16 numbers, a **matrix**. Why 4 and not 3, even though space only
has three axes? For stretching and rotating, 3 × 3 numbers would be enough. The extra column takes
on the moving, so that all three motions fit into a single table. How a matrix changes a point is
explained in Appendix [B](B-rendering.md) in the section "Matrices without fear." Here the
principle is enough: a matrix is a recipe "first stretch, then rotate, then move" that you can
apply to any point.

An example from Kern's tests that you can draw in your notebook:

1. Start: the point (1, 0, 0).
2. Stretched by 2: (2, 0, 0).
3. Rotated by 90° around the Z axis: the point moves from the X axis onto the Y axis, so
   (0, 2, 0).
4. Moved by (1, 2, 3): (0 + 1, 2 + 2, 0 + 3) = **(1, 4, 3)**.

If you change the order, something else comes out, just like with the book. That's why Kern sets
exactly in which order two matrices take effect. The rule is in the blue book, Chapter 3.

Matrices tell **points** and **directions** apart. A point gets stretched, rotated and moved. A
direction, such as "north," only gets stretched and rotated, never moved. Because a direction has
no place.

> **For pros:** In memory, the 16 numbers aren't stored as a table but in one long row, **column
> by column**. Because matrices travel through the locker wall from
> [Chapter 1](01-introduction.md), C++ and JavaScript must use exactly the same order. And to find
> out what's under the mouse, Kern calculates a matrix "backwards," with twice as many bits.
> Everything else is in the blue book, Chapter 3.

## 3.8 The Camera

In Kern, a camera isn't a matrix but a **description**:

- **Eye point** – where the camera stands,
- **Target** – where it looks,
- **Up** – which direction is up in the picture (normally +Z),
- **Field of view** – how far it sees from top to bottom, normally 60°,
- **Near and far limit** – whatever is closer or farther away doesn't get drawn.

> **Picture it like this:** a flashlight in the dark. Where you stand, where you shine, how wide
> the cone of light is and how far the light reaches – that's all you need to know what you see.

From this description, the renderer, clicking and the editor work out their matrices themselves.
Before Kern uses a camera, it checks it: is the far limit behind the near one? Is the angle
greater than 0° and less than 180°? Kern rejects invalid cameras with "invalid camera" (3.2).

Besides the normal camera with perspective, there's the **top view without perspective**: there,
nothing gets smaller when it's farther away, like on a map. Instead of an angle, you say here how
many meters the picture shows, like the scale of a map.

> **Mission:** Press **Alt+J** for the top view. The small gray text at the bottom left of the
> Viewport (the 3D window) now shows how many meters wide the picture is. Turn the mouse wheel and
> watch the number change. **Alt+G** switches back to perspective.

**What the camera doesn't see doesn't get drawn.** A camera's field of view looks like a pyramid
lying in front of the camera with its tip cut off at the near limit – like a lampshade with its
narrow side pointing toward you. You'll find a picture of it in [Chapter 5](05-scene-and-geometry.md),
section 5.2. It has six walls: left, right, top, bottom, near, far. Around every thing, Kern puts
a simple shell, a sphere or a box. If the shell lies completely outside the walls, Kern saves
itself the drawing ([Chapter 5](05-scene-and-geometry.md)). If a shell is invalid, for example
because it's still empty, Kern doesn't know where the thing is and draws it anyway. When in doubt,
better to draw once too often than to have a hole in the picture.

> **For pros:** A new box starts "upside down": its smallest value hugely large, its largest
> hugely small. The first point you add then sets it right immediately – like a high-score list
> that starts with an unbeatably bad score.

## 3.9 Math That Calculates the Same Everywhere

Remember the game of telephone from [Chapter 1](01-introduction.md)? Here you see where the small
differences come from and how Kern prevents them. There are three sources.

### Source 1: Calculating Functions That Round Differently

Some calculations are fixed down to the last bit on every computer, for example taking square
roots or rounding down. Others aren't: such as **sine** and **cosine**, which you use to calculate
rotations (they come in grade 9 or 10), and a few more functions from high-school math. There,
every math library may round a little differently, and the ones from Linux, the Mac, Android and
the one in the browser do – at the very last digit. In a robot simulation, this one bit turns into
a different gait within seconds.

**Kern's solution:** Kern brings its own math library. It comes from a free project and is
translated in exactly the same way on every computer. That way it rounds the same everywhere. The
physics Box3D from [Chapter 1](01-introduction.md) has its own, equally fixed angle functions.
Nothing gets slower because of this.

### Source 2: Merged Calculation Steps

Some processors can calculate "times and then plus or minus" in **one** step, rounding only once
instead of twice. That sounds harmless but changes the result. Work it out yourself! Problem:
0.15 · 3 − 0.04. We round to one decimal place like in school; the computer does the same, just
much further back:

| Way | Calculation | Result |
|---|---|---|
| two steps | 0.15 · 3 = 0.45 → 0.5, then 0.5 − 0.04 = 0.46 → 0.5 | **0.5** |
| one step | 0.15 · 3 − 0.04 = 0.41 → 0.4 | **0.4** |

Same problem, different answer. That's why Kern forbids the compiler from merging steps everywhere
([Chapter 2](02-building-and-testing.md)).

### Source 3: the Order When Adding Up

In school you learn: 1 + 2 + 3 = 3 + 2 + 1, the order doesn't matter. On a computer that's not
quite true, because it rounds after every step. Again to one decimal place:

| Order | Calculation | Result |
|---|---|---|
| 1 + 0.04 + 0.04 | 1 + 0.04 = 1.04 → 1.0, then 1.0 + 0.04 = 1.04 → 1.0 | **1.0** |
| 0.04 + 0.04 + 1 | 0.04 + 0.04 = 0.08 → 0.1, then 0.1 + 1 = 1.1 | **1.1** |

If several helpers work on a sum at the same time, the order depends on who finishes first. That's
why Kern always adds up sums in a fixed order, no matter how many helpers are calculating along.

The same goes for randomness in training: every duck gets its own fixed **starting value** from
which its "randomness" is calculated, like with the hashes from 3.4. Which helper happens to be
calculating it doesn't matter ([Chapter 14](14-lehrer.md)).

### The Rules

- Whatever **feeds the simulation** uses Kern's own math library. That also applies to packing a
  rotation into four numbers from 3.7, because you need sine and cosine for that.
- Sums are always formed in the same order.
- Shortcuts that make the compiler faster but make it calculate differently depending on the
  computer are forbidden.
- The physics does without its turbo path with special commands and calculates everywhere the
  same simple way as in the browser ([Chapter 2](02-building-and-testing.md)).

Whatever you only **see or hear**, such as sound, particles or parts of the editor, may use the
computer's ordinary library. But as soon as such code delivers values for the physics, the control
or the training, it has to switch to Kern's own library.

> **For pros:** The own library comes from musl. You call it with `kern::dm::`, for example
> `kern::dm::sin` instead of `std::sin`. The fingerprint from
> [Chapter 2](02-building-and-testing.md) shows every violation down to the exact control tick. The
> names of all functions and compiler options are in the blue book, Chapter 3.

## Level complete!

- You know how many values fit into 8 bits and why decimal numbers get rounded.
- You know how Kern reports errors and why "not greater than 0" also catches NaN.
- You know why the time of day must never get into the simulation.
- You know what JSON is and why Kern checks everything first and then takes it over.
- You know Kern's space: meters, **Z up**, right hand; arrows, rotations and matrices.
- You know what a camera describes and why invisible things don't get drawn.
- You know why Kern brings its own math.

## Puzzle corner

1. Which direction is "up" in Kern?
2. x is NaN. Why doesn't "Is x less than or equal to 0?" notice the error, but "Is x not greater
   than 0?" does?
3. Take the point (1, 0, 0) from 3.7 through the same three steps in reverse order: first move by
   (1, 2, 3), then rotate by 90° around the Z axis, then stretch by 2. Does (1, 4, 3) come out
   again? Hint: the rotation is a quarter turn, counterclockwise seen from above. What pointed to
   the right points forward afterwards (that's how (2, 0, 0) became (0, 2, 0) in the example), and
   what pointed forward points to the left afterwards.
4. A servo file has five values, and the fourth is broken. What happens to the old profile?
5. **Boss puzzle:** A control program calculates the duck's walking direction with the computer's
   ordinary math library. The fingerprint on the Mac differs. Why, and how do you fix it?

### Solutions

1. Toward +Z.
2. Every comparison with NaN is false. So "less than or equal to 0?" is false, and the error slips
   through. "Greater than 0?" is also false – and "not false" is true. That's how NaN gets caught.
3. No. Moving: (1 + 1, 0 + 2, 0 + 3) = (2, 2, 3). Rotating around the Z axis: the height 3 stays,
   "2 to the right, 2 forward" becomes "2 to the left, 2 forward," so (−2, 2, 3). Stretching:
   (−4, 4, 6). That's a completely different point from (1, 4, 3) – with matrices, the order
   matters.
4. Nothing. Kern first checks all values and only takes them over if all of them are right. The
   old profile stays completely intact, and a message names the broken value.
5. The computer's library rounds differently at the last digit on the Mac than on the PC. Because
   the value feeds the simulation, the difference builds up from step to step. Solution: calculate
   the direction with Kern's own math library, which rounds the same everywhere.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
