# 3 Foundations: the Groundwork Beneath Everything

> **In short:** Every floor of Kern stands on four building blocks: a **toolbox** (numbers, error
> messages, clock, helpers), a **data reader** for the JSON format, the **space math** for the 3D
> world and **calculating without rounding differences**. Here you find out what each building
> block does and why it is built the way it is.

> **Key terms:**
> - **Function** – a named piece of program that does a job and gives an answer, such as "Is
>   this file okay? – Yes."
> - **Data** – everything a program processes: numbers, text, lists.
> - **Decimal number** – in a computer, a number with a fixed number of digits; whatever doesn't
>   fit gets rounded ([Chapter 1](01-introduction.md)).
> - **Coordinates** – three numbers that say where something is: how far right, forward and up.
> - **Axis** – an imaginary line through space that you can rotate something around.
> - **Renderer** – the part of Kern that paints the pictures.
> - **Text editor** – a plain writing program, called "Notepad" on Windows and "TextEdit" on the
>   Mac.
> - **Viewport** – the big window in the middle of Kern Studio where you see the world.

If the basement is crooked, so is the whole house. The exact names and values for this chapter
are in the blue book, Chapter 3.

## 3.1 The toolbox

### Numbers with a fixed number of digits

In a computer, a number takes up a fixed number of bit boxes. In C++, the size of an ordinary
number may differ from computer to computer if you don't state it explicitly – just as "a glass
of water" can sometimes be 200 and sometimes 300 milliliters. That's why Kern **always** states
the size explicitly.

> **Imagine …** An odometer with six digits jumps from 999,999 to 000,000; one with seven digits
> only later. If you compare two cars, you need to know how many digits each odometer has.

An example: each bit box can be 0 or 1, so each extra box doubles the possibilities. Eight boxes
give 2 · 2 · 2 · 2 · 2 · 2 · 2 · 2 = 256 possibilities, that is, the numbers 0 to 255 – roughly as
much as a three-digit counter. After that, such a counter starts again at 0. Because Kern always
fixes the size, every computer calculates with counters of the same size – and whatever goes to
the browser through the shared memory is the same size on both sides.

### When something goes wrong: say no and leave a note

Many programming languages have an **emergency button** for errors (technical term: exception):
the program jumps right out of the middle of its work until someone somewhere catches the
emergency. Kern does without it, because an emergency button can leave a job half done – like a
cook who runs out of the kitchen in the middle of stirring: the pot is still on the stove, and
nobody knows how far he got. So in Kern, a function that can fail simply answers **"no"** and
first pins a note with the reason to a bulletin board. Whoever called it decides for themselves
what happens next. Nothing gets torn off in the middle of the work.

If the renderer, for example, gets a 3D model with broken numbers, it answers "no", pins up the
reason, and the old model stays exactly as it was.

> **Imagine …** A hotel has exactly **one** bulletin board. Whoever reports something pins up a
> note and takes the old one down. Whoever checks always reads only the newest one.

That's how Kern's error channel works: one bulletin board for the whole program.

> **Watch out!** The bulletin board shows the **newest** note – not necessarily the one for the
> task you're asking about. If you want to be sure, clear the board first. And if several helpers
> calculate at the same time, they share that one bulletin board: the newest note may then even
> come from a different helper.

> **For the curious:** Kern likes to check "is **not** greater than zero" instead of "is less
> than or equal to zero". The reason is the special value **NaN** ("not a number"), which comes
> up for example with 0 divided by 0: every comparison with NaN is false, so "not greater than
> zero" is true – and the broken value gets turned away.

### The clock doesn't belong in the simulation

Kern has a clock that counts milliseconds. It's for **measuring**: how long did something take?
Has a task already used up the time it's allowed?

This clock has no business in the **simulation**. Physics and robots count **fixed steps**: a
tick is always the same length, no matter how fast the computer calculates it. If the duck looked
at the clock, it would walk differently on a slow computer than on a fast one
([Chapter 6](06-physics.md), [Chapter 13](13-micro-duck.md)).

> **Imagine …** A board game. You move round by round, not by the stopwatch – otherwise whoever
> rolls the dice faster wins, not whoever plays better. Kern's simulation counts rounds.

### The logbook

What Kern experiences along the way, it writes into a **logbook** (technical term: log). Every
message has a level, from "just for debugging" through "note" and "warning" up to "error". In the
browser, the messages end up in a hidden window that the browser keeps ready for developers –
that's not Studio's Cmd console. If, for example, the glow effect (the shine around bright lamps,
[Chapter 4](04-graphics.md)) can't start, Kern writes a warning into the logbook, and the game
keeps running without glow – again "say no and carry on" instead of a crash.

### A window onto data

Kern doesn't pass large amounts of data around, only a pointer to where they are. That saves a
lot of time: a big 3D world can consist of millions of numbers, and copying them at every
hand-over would noticeably slow things down.

> **Imagine …** A note: "Shelf 4, the next 20 books." It's light, because you don't have to carry
> the books around. But it doesn't **own** them. If someone clears the shelf, the note points at
> nothing.

A note like that holds only two pieces of information: where the data starts and how much there
is. Almost all data flows into the engine in this form. That leads to a rule: whoever wants to
**keep** the data copies it. The renderer, for example, keeps copies of its models so it can
later tell what you're clicking on and rebuild everything after losing the graphics card
([Chapter 4](04-graphics.md)). And do you remember the house rule from
[Chapter 1](01-introduction.md)? The browser side reuses its storage spot every frame. Whatever
is there is overwritten by the next frame – that's why Kern copies what it needs for longer.

> **For the curious:** In Kern's source code, a note like that is called a **span**.

### Fingerprints for data: hashes

A **hash** turns any amount of data into a short number. The same data always gives the same
number; if just one letter changes, a completely different one comes out.

> **Imagine …** A digit sum, only much more thorough: with a digit sum, 12 and 21 give the same,
> while a good hash notices the swap.

That lets Kern tell quickly whether something has changed: the renderer sends a long setting,
such as the position of a thing in space, to the graphics card again only if its hash is
different. The **checksum** from the duck's fingerprint in [Chapter 2](02-building-and-testing.md)
is exactly such a hash too – checksum and hash are the same thing.

A second hash has a different job: it turns a number into a **random-looking** number that still
comes out the same every time. What for? A game often wants a bit of chance, for example so that
a character's shots scatter slightly and not every shot sounds the same. But real chance would
make the world run differently on every computer. The mixing hash delivers chance you can
repeat: shot number 7 always scatters the same way, on every computer.

Both hashes calculate only with whole numbers, without rounding – so the result depends only on
the data, never on the computer.

## 3.2 JSON: Kern's favorite format

**JSON** is a text format for data that both people and programs can read, for example
`{"name": "Duck", "legs": 2}`: every entry has a name and a value. Kern stores in it levels,
commands between the Studio page and the engine, the duck's motor descriptions, the Lehrer's
practice data and the list of duck networks. Its JSON building block makes three promises.

**1. The order stays.** If Kern reads a file in and writes it out again, everything is in its
place. If a value in the level changes, only that spot in the file changes – so two versions are
easy to compare.

**2. Numbers look like they do in the browser.** A decimal number in a computer has a great many
digits. You could write each one with 20 digits after the decimal point – that would be
unreadable. Instead, Kern takes the **shortest** way of writing it that gives exactly the same
number again when read back in, by the same rule as JavaScript in the browser. That's why a file
looks the same whether Kern or the browser wrote it.

> **Did you know?** If you calculate 0.1 + 0.2 in the browser, you get **0.30000000000000004**,
> and Kern writes exactly the same. That's rounding: computers count only with 0 and 1, in the
> **binary system**. There, 0.1 doesn't come out even, just as 1/3 doesn't for us (0.3333 …,
> never ending). So what gets stored is a number just next to 0.1, likewise for 0.2, and their sum
> lands a tiny bit next to the number the computer stores for 0.3. That's why Kern writes all the
> necessary digits here – with the short "0.3", reading it back in wouldn't give exactly the same
> number. (Programs, by the way, always write a point as the decimal separator, in every
> language.) Exactly these tiny deviations are why Kern is so fussy about calculating
> ([Chapter 1](01-introduction.md)).

**3. Distrust of strangers' files.** A level can come from anyone. That's why Kern rejects
infinitely large numbers, absurdly deeply **nested** entries – entries inside entries inside
entries, like boxes in boxes in boxes – and a few entry names that have a special meaning in
JavaScript and could bend parts of the program there.

On top of that comes calm when reading: if an entry is missing, you get an empty value instead of
a crash. If a value has the wrong **type** – if, say, there's text where a number is expected –
you get a fallback value. And reading in delivers its result **only if it fully worked**;
otherwise it names the spot where it got stuck.

> **Try it!** Take a look under the hood of a level.
> 1. **File › New Level...** and pick **Basic** there: a floor and a cube.
> 2. Click the cube (in the Outliner it's called "Cube"). On the right in **Details**, under
>    **Transform**, there's the field **Location** with X, Y and Z. Z points up (more on that in
>    Section 3.3). Type a **3** for Z and press Enter: the cube floats 3 meters up.
> 3. **File › Save**, Ctrl+S. Chrome and Edge ask where to save; other browsers put the file
>    `Untitled.klevel` in the downloads folder.
> 4. Right-click the file, **Open with**, then **Notepad** (Windows) or **TextEdit** (Mac).
>
> At the very top it says `"format": "kern-level"` – that's how Kern recognizes its levels. Use
> Ctrl+F (Mac: Cmd+F) to search for "Cube", the cube's name: below it, under **position**, is the
> number sequence 0, 0, 3 – your cube. What's called **Location** in Studio is called
> **position** in the file. That's what a level looks like inside: names with values, in a fixed
> order.

### Check first, then accept: servo profiles

A **servo profile** describes one of the Micro Duck's motors: how strong it is, how stiffly and
with how much damping it gives way, how delayed it reacts ([Chapter 16](16-calibration.md)). Kern
first checks whether the text is a motor description at all, then each entry separately: an entry
may be missing (then the default applies), but if it's there, it must be a number in the allowed
range – otherwise it stops, naming the guilty entry. **Only at the very end** is the profile
accepted.

> **Imagine …** A bank counter. First the whole transfer form is checked, then it's booked. It
> never gets half booked before someone notices the account number is missing.

## 3.3 Math for 3D space

The space math is Kern's most-used tool: every frame, every moving thing, every camera and every
click needs it, often thousands of times. That's why it's built to calculate without detours and
never to have to look for space in memory first – like change in your pocket instead of a trip
to the bank.

### The ground rules

So that all parts mean the same world, this applies everywhere: lengths in **meters**, times in
**seconds**, masses in **kilograms**, and **Z points up**.

> **Imagine …** Lay graph paper on the table: X points right, Y forward, Z to the ceiling. That's
> called **right-handed**: spread the thumb, index finger and middle finger of your right hand at
> right angles – the thumb is X, the index finger Y, the middle finger Z.

In Studio and in levels, angles are in **degrees**. Inside, Kern calculates in **radians**, and
that's a simple picture: walk along the edge of a circle with a radius of 1 meter. Half a lap is
about 3.14 meters long (the number π). The angle in radians is the distance along the edge: 180°
is π, 90° about 1.57. Many formulas get simpler that way.

> **Try it!** Use **File › New Level...** to open the Physics Lab level. Type `gravity 0 0 -2`
> into the **Cmd** field at the bottom of the status bar, press Enter and start **Simulate**.
> Gravity now points weakly in the direction of minus Z, downward: everything falls almost as if
> on the moon. `gravity` without numbers restores Earth, roughly 0, 0, −9.8: falling things get
> about 9.8 meters per second faster every second.

### Vectors and rotations

A **vector** is a set of directions made of numbers: "3 meters to the right, 4 forward, 1 up", or
(3, 4, 1) for short. You can draw it as an arrow; the same set of numbers can also mean the point
where you end up.

Often you only need the **direction** of an arrow, for example "where is the camera looking?".
Then Kern brings the arrow to length 1: it only says *where to*, no longer *how far* – like a
signpost without a distance. But careful: if Kern wants the direction from point A to point B and
both points are the same, there's no arrow at all – it has length zero and points nowhere. Instead
of a nonsense value that would eat its way through all further calculations like a virus, Kern
then delivers a **fallback**: when in doubt, "up".

Every **rotation** in space can be described by an axis and an angle.

> **Imagine …** A rotisserie chicken on a spit. The spit is the axis, and the chicken is turned by
> an angle. Every rotation, no matter how skewed, is a spit like that with an angle.

Kern packs the axis and angle into four numbers, a **quaternion** (from Latin, roughly "a set of
four"). Roughly speaking, three numbers hold the direction of the spit and the fourth the angle –
cleverly packed into each other. The math behind it is tricky, but using it is easy: axis and
angle in, rotation out. The four numbers are made with Kern's own angle formulas from Section 3.4,
so that every rotation comes out the same everywhere.

Two rotations can be chained into one, and here too the order matters: tip a book forward first
and then turn it to the left – it ends up lying differently than if you turn it to the left first
and then tip it forward. Try it with a real book.

### Matrices: a whole movement in one package

A thing in the 3D world is usually **stretched**, **rotated** and **moved** all at once. A
**matrix** packs all three steps into one package: a small table of numbers. You put a point in
and get the fully moved point out. The renderer gets a package like that for every thing; in the
smallest example game, each of the spinning crates has its own.

An example on graph paper: the point (1, 0, 0) is one meter to the right of the origin. The
package says: stretch by 2, rotate by 90° around the Z axis, move by (1, 2, 3).

1. **Stretch** by 2: (1, 0, 0) becomes (2, 0, 0).
2. **Rotate** by 90° around Z, counterclockwise seen from above: what pointed right now points
   forward. (2, 0, 0) becomes (0, 2, 0).
3. **Move** by (1, 2, 3): (0, 2, 0) becomes **(1, 4, 3)**.

A test in Kern checks exactly this calculation.

> **Watch out!** Order matters. Taking two steps and then turning around is different from
> turning around first and then walking. That's why Kern fixes it: first stretch, then rotate,
> then move.

> **For the curious:** Mathematicians write a package like that "backwards": **Move · Rotate ·
> Stretch · Point**. You read from the point, right to left: whatever is closest to the point acts
> first – like getting dressed, where the sock right on your foot comes first and the shoe outside
> it after. By the way, the table has 4 by 4 numbers; the fourth row and column are needed so that
> moving fits into the package too. The exact rules are in the blue book, Chapter 3.

Usually a matrix can also be **inverted**; then it does the movement backwards. Kern needs that
when you click into the scene: the path leads from the screen back into the world. But not every
movement can be turned back: if a thing was stretched by 0 in one direction, that is, squashed
flat like a pancake, nobody knows any more how thick it was before. Then the inversion simply
answers "no" instead of calculating nonsense.

### Camera and field of view

In Kern, the **camera** isn't a matrix but a **description**, like one a photographer would give:
Where am I standing? Where am I looking? Which way is up? How wide is my lens open? On top of that
come a **near limit** and a **far limit**: whatever is closer to the lens than the near limit or
farther away than the far limit doesn't get painted. Things extremely close and infinitely far
can't sensibly be put into the picture, and the world has to end somewhere so that Kern doesn't
paint forever. The default values are in the blue book, Chapter 3. From the description, Kern
calculates a matrix package for the whole path from the world onto the picture. An impossible
camera, for example with the eye and the target in the same place, is turned away by Kern with a
note on the bulletin board.

Besides perspective, there's the **parallel view** without a vanishing point, as in a blueprint:
distant things don't get smaller. Studio uses it for the views from above, the front and the side.
Because there's no vanishing point here, you don't describe this camera with an opening angle but
with how many meters high the visible section is.

What the camera sees is a pyramid lying on its side with its tip cut off, the **view pyramid**
(technical term: **frustum**). Seen from above, it looks like this:

![The view pyramid from above, between the near and far limits: whatever is inside or half on the edge gets painted, whatever is completely outside is skipped](../abbildungen/sichtpyramide.svg)

Whatever is outside doesn't need to be painted. To make the check quick, Kern puts every thing
into an invisible **sphere**. If the sphere is entirely outside the pyramid, the thing is
invisible – one quick check instead of many triangles. If a thing doesn't have a valid sphere yet,
for example because it hasn't been calculated yet, it gets painted anyway. Kern's rule: **when in
doubt, draw** – better once too often than a hole in the world. More on that in
[Appendix B](B-rendering.md).

> **Try it!** Open Orb Run. Click **≡** in the bar at the top of the viewport and set **Field of
> View** first to 110°, then to 50°: wide angle with distorted edges, then as if zoomed in. Press
> **Alt+J** in the viewport: the view jumps to **Top**, the parallel view from above, like a city
> map. **Alt+G** brings **Perspective** back.

## 3.4 Math that calculates the same everywhere

Remember the hiking route from [Chapter 1](01-introduction.md)? A tiny difference at the start,
and by evening you're in a different village. This section is about where such differences come
from.

Some calculations have a clearly defined answer that every computer gives the same way: square
roots, rounding down, the absolute value of a number. With the **angle calculations** sine and
cosine, that's **not** the case.

> **Imagine …** A point runs around the circle with a radius of 1 meter from Section 3.3. The
> cosine says how far to the right of the center it is at the moment, the sine how far up. Anyone
> who wants to rotate something needs these numbers all the time.

Such values can only be approximated: the sine of 1, for example, has infinitely many digits after
the decimal point, which never fit into a fixed number box. Every system – Linux, macOS, Android,
WebAssembly – brings its own formulas, approximates in its own way, and in the very last bit
something different sometimes comes out. In a robot simulation, that turns into a different gait
within seconds, and a network trained in the browser would run differently on your own computer.

Kern's solution: **its own formulas** for sine, cosine and their relatives, from a free code
collection. They're translated the same way everywhere, without arithmetic shortcuts, and they
carry their own names so that they're never mixed up with the system's. The physics library – the
outside physics program from [Chapter 1](01-introduction.md), taken on unchanged – has its own
angle calculations anyway, which are the same everywhere. So for new code:

- Everything that **affects the simulation** uses Kern's own formulas.
- Sums are formed in a **fixed order**, no matter how many helpers are calculating: helpers finish
  in changing order, and as the four-digit calculator from Chapter 1 shows, a different order
  changes the result.
- No arithmetic shortcuts that calculate "fast, but roughly".

Pure **presentation** – sound, sparks, parts of the editor – may use the system's formulas: nobody
notices whether a spark flies a millimeter farther to the left. But if a value like that flows
into physics, control or training, it has to move over to Kern's own formulas.

> **Did you know?** The own formulas cost no speed: they're as fast as the system's, and most of
> the physics' calculation time goes into the joints anyway, not into sine and cosine. Whether
> everything is right is revealed by the fingerprint from [Chapter 2](02-building-and-testing.md).

> **For the curious:** The formulas come from the code collection **musl**; each one gets a
> prefix so that it never clashes with the system's version.

## What you take away

- Kern fixes for every number how many bit boxes it has.
- Kern reports errors with "no" and a note on a single bulletin board.
- The clock is for measuring, never for the simulation: that counts fixed steps like a board game.
- Kern passes data on as a note that only says where it is; whoever wants to keep it copies it.
- Kern's JSON keeps the order, writes numbers like the browser and distrusts strangers' files.
- Meters, seconds, kilograms, **Z up**; a matrix packs stretching, rotating and moving into one
  package, and the order matters.
- The camera is a description like a photographer's; invisible things are sorted out, but when in
  doubt they're painted.
- For the simulation: its own angle formulas, sums in a fixed order and no arithmetic shortcuts.

## Brain teasers

1. Two tasks fail shortly after each other. Then you read the bulletin board. Whose note do you
   see?
2. The point (1, 0, 0) is rotated by 90° around the Z axis and then moved by (5, 0, 0). Where does
   it end up? (Tip: graph paper, X to the right, Y forward.)
3. A spark effect uses the system's sine formula. A problem? And what if the sparks are supposed
   to nudge the duck?
4. Why doesn't 0.1 + 0.2 give exactly 0.3 in a computer?
5. Why mustn't the duck look at the clock to decide how far it walks in a tick?

### Solutions

1. The second task's: every new note replaces the old one.
2. Rotating turns (1, 0, 0) into the point (0, 1, 0), then moving gives (5, 1, 0).
3. For pure presentation it's allowed. If the sparks nudge the duck, they flow into the physics,
   and then Kern's own formula is needed.
4. Computers calculate in the binary system, only with 0 and 1. There, 0.1 and 0.2 never come out
   even, like 1/3 for us. What gets stored are numbers just next to them, and their sum lies just
   next to 0.3.
5. A slow computer would show more elapsed time than a fast one, and the duck would walk
   differently on the two. With fixed steps it walks the same everywhere.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
