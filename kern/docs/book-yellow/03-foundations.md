# 3 The Foundation

> **Your level:** Every skyscraper needs a foundation. Here you'll learn the four building blocks
> all of Kern stands on: types and errors, JSON, 3D math and a math library that computes exactly
> the same on every computer.

The four building blocks are the headers `base.h`, `json.h`, `math.h` and `dmath.h`.

## 3.1 The Toolbox: `base.h`

### Types with a Clear Width

For numbers, Kern always spells out the width: `u8` to `u64` (unsigned), `i8` to `i64` (signed),
`f32` and `f64` (decimal numbers). The number tells you how many bits the type has. At the border
to WebAssembly, that matters: there, a pointer is 32 bits wide, `f64` is exactly JavaScript's
`Number`, and the interface to the browser speaks in `u32` words.

### Errors Without Drama

Kern doesn't use exceptions. A function that can fail returns `false` (or 0) and leaves a message
with `fail()`. Because `fail()` itself returns `false`, a check fits on one line – like in picking:

```cpp
if (!camera_valid(camera))
  return fail("invalid camera");
```

If the camera is broken, Kern remembers "invalid camera" and reports `false`. You can read the
message with `last_error()`, and from JavaScript with `kern_last_error()`.

> **Watch out, trap:** There's only **one** message buffer (1024 characters) for all threads. Every
> `fail()` overwrites the previous one. So `last_error()` shows the latest message, not necessarily
> the one from your call. To be safe, call `clear_error()` first.

> **Crazy but true:** Kern often checks `!(x > 0)` instead of `x <= 0`. The reason is **NaN** ("not
> a number", for example 0 divided by 0). Every comparison with NaN is false. `x <= 0` would let NaN
> through; `!(x > 0)` catches it.

### Logging, Time and Helpers

`log()` knows the levels `debug`, `info`, `warn` and `error`. In the browser, the text lands in the
console with `[kern]` in front of it; natively, it goes to the output. From the bloom effect:

```cpp
supported_ = false;
log(LogLevel::warn, "bloom disabled: %s", last_error());
return false;
```

If bloom doesn't work: switch it off, warn with the reason, `false`.

`now_ms()` gives you milliseconds for measurements. **It never belongs in the simulation**: that
counts fixed steps, not clock time.
`min_of`, `max_of` and `clamp` (limit to a range) want two values of the same type.

### `Span`: The Shop Window

A `Span` points to values that live somewhere else: a pointer `data` plus a count `size`. It does
**not own** them. Any container with `data()` and `size()` turns into a span all by itself. This is
the format in which all data flows into the engine, from C++ as well as from JavaScript.

> **Picture it like this:** A span is a shop window: you can see the goods, but they don't belong
> to you. If the shop clears out, the window is empty.

So a span must never live longer than its data. If you want to keep data, you copy it – that's why
the renderer keeps its own copies ([Chapter 4](04-graphics.md)).

### Hashes

`hash_bytes` (64-bit FNV-1a) turns any number of bytes into an ID number. The renderer uses it to
spot values that haven't changed, and the duck's fingerprint from
[Chapter 2](02-building-and-testing.md) is exactly this kind of hash. `hash_u32` (*lowbias32*) mixes
up an integer, for example for noise in a game. Both compute only with integers: same bytes, same
ID number.

## 3.2 JSON

Levels, Studio commands, servo profiles: all **JSON**. Kern's JSON building block makes three
promises:

1. **Order stays.** Objects keep the order of their keys. A file written by Kern or by JavaScript's
   `JSON.stringify` comes out byte for byte the same after reading it in and writing it out again.
   That way, you can see changes to levels cleanly.
2. **Numbers like in JavaScript**, digit for digit like `JSON.stringify`.
3. **Safe against evil files.** It rejects more than 80 levels of nesting (Studio commands: 120),
   non-finite numbers and the keys `__proto__`, `prototype` and `constructor`, which can be used to
   attack JavaScript pages.

```cpp
CHECK(num(0.1) == "0.1");
CHECK(num(-0.0) == "0");
CHECK(num(1e21) == "1e+21");
CHECK(num(NAN) == "null");
```

This test pins it down: 0.1 stays "0.1", minus zero becomes "0", 10²¹ becomes "1e+21", NaN becomes
`null`.

> **Crazy but true:** Kern writes 10²⁰ out in full as "100000000000000000000", but 10²¹ as
> "1e+21" – exactly like JavaScript.

A missing key gives you `null` instead of a crash, `as_number(fallback)` gives you the fallback
value if the type is wrong, and `parse` writes its message into a text of its own (not into
`last_error()`) with the position ("at offset 17"). When loading, for example a servo profile, Kern
checks every field for type and range and only takes it over at the very end. A broken field leaves
the old profile untouched. In the same way, `parse` overwrites its result only if everything worked.
`stringify(value, 2)` turns it back into text with two spaces of indentation, like
`JSON.stringify(value, null, 2)`.

> **Mission:** Save a level with **Ctrl+S** and open the `.klevel` file in a text editor. It's
> JSON! Find `"format": "kern-level"` and the rotations of your actors in degrees.

## 3.3 The 3D Math

| Rule | Setting |
|---|---|
| Units | meters, seconds, kilograms |
| Up | **+Z** |
| Handedness | right-handed |
| Angles | radians in code, degrees in levels |
| Matrices | 4×4, column by column |

**Z up** applies everywhere: renderer, physics, audio, levels. In its own coordinates, the camera
looks along −Z, and the projection follows OpenGL. `math.h` lives entirely in the header and never
allocates memory. That way, the compiler can build the calculations right into the code, which
makes them fast.

> **Mission:** Choose **File → New Level...**, the template **Basic** and **Create**. Place a
> **Cube** using the Content Drawer (Ctrl+Space). At the bottom left, the axis cross shows X in
> red, Y in green, Z in blue – pointing up. Drag the blue arrow with the Move tool (**W**) and watch
> the **Location** Z field in **Details**.

### Vectors

`Vec2`, `Vec3`, `Vec4` are arrows made of 2, 3 or 4 numbers, with dot and cross product, length and
distance (Pythagoras!), and mixing (`lerp`). `normalize` makes an arrow 1 long. A zero arrow has no
direction – instead of NaN, Kern then returns a fallback, (0, 0, 1) by default. That way, an input
with two identical points doesn't ruin the whole scene.

### Rotations: `Quat`

Kern stores a rotation as a **quaternion** (x, y, z, w): a clever way of packing up "axis plus
angle". This is how `Quat::axis_angle` packs it:

```cpp
const Vec3 a = normalize(axis);
const f32 s = kern::dm::sin(radians * 0.5f);
return {a.x * s, a.y * s, a.z * s,
        kern::dm::cos(radians * 0.5f)};
```

For 90° around Z: half the angle is 45°, sin 45° ≈ 0.707, cos 45° ≈ 0.707. The axis (0, 0, 1)
times 0.707 gives x = 0, y = 0, z = 0.707, plus w = 0.707. No rotation is (0, 0, 0, 1). Notice the
`kern::dm::sin` – 3.4 shows why. You combine two rotations with `*`: in `q * b`, `b` acts first,
then `q`. `rotate(v)` rotates an arrow.

### Matrices: `Mat4`

A 4×4 matrix moves, rotates and scales all in one go. Its 16 numbers are stored **column by
column** (number = slot):

```
 0  4  8 12
 1  5  9 13
 2  6 10 14
 3  7 11 15
```

Row r, column c is in slot `c * 4 + r`. The translation is in slots 12, 13, 14.

`trs(t, q, s)` builds a matrix from translation, rotation and scale; in `A * B`, `B` acts first,
then `A`. From the test `math_matrices`: (1, 0, 0) stretched by 2 → (2, 0, 0), rotated 90° around
Z → (0, 2, 0), moved by (1, 2, 3) → **(1, 4, 3)**.

`transform_point` rotates, stretches and moves a point; `transform_vector` leaves out the move,
because a direction has no location. On top of that come `perspective` and `ortho` (projection),
`look_at` (where the camera looks), `normal_matrix3` (light on surfaces that are stretched
unevenly) and `invert` in double precision, so that click rays hit even far away.

> **Mission:** In **Details**, type `45*2` into your cube's **Rotation** Z and press Enter.
> Math expressions are allowed – the cube turns by 90°.

### Sphere, Box, Camera

A `Sphere` with a negative radius counts as empty; when in doubt, it gets drawn. An `Aabb`
(axis-aligned box) starts out "inside out", so that the first point defines it.

The `Camera` is a **description**, not a matrix: eye point, target, up (+Z), plus the **vertical**
field of view in radians (default π/3 = 60°) and the near and far limits. Orthographic cameras give
the visible height in meters instead of the angle. `camera_valid` checks everything. One test
checks that the target lands exactly in the center of the image. From the camera come the six walls
of the visible region for culling ([Chapter 5](05-scene-and-geometry.md)). From JavaScript, you set
the camera with twelve numbers; Kern rejects invalid ones with "invalid camera".

> **Mission:** Press **Alt+J** for the top view, an orthographic camera: the info line now shows
> the visible width in meters. **Alt+G** switches back to perspective.

## 3.4 Math That Computes the Same Everywhere

`sqrt` and `floor` are pinned down exactly, but `sin`, `cos`, `exp` or `atan2` aren't: the
libraries of Linux, Apple, Android and WebAssembly round differently in the last bit. In a robot
simulation, one bit turns into a different gait within seconds.

A second bit thief is called **FMA**: `a*b+c` in one step with only one rounding. A made-up
example: compute 0.15 · 3 − 0.04 and, after every step, round to one decimal place, the way you
learned at school. The computer rounds the same way, just in the last bit:

| Path | Calculation | Result |
|---|---|---|
| two steps | 0.45 → 0.5; 0.5 − 0.04 = 0.46 | 0.5 |
| one step | 0.45 − 0.04 = 0.41 | 0.4 |

Same problem, different answer. That's why `-ffp-contract=off` applies everywhere: no merging.

**The solution:** Kern brings along the functions of the **musl** library, renamed to
`kern_musl_*`, and compiles them equally strictly on every platform, without FMA. Box3D has its own
angle functions. Everything is used with `kern::dm::` in front: `kern::dm::sin`, `kern::dm::atan2`,
`kern::dm::pow` and so on. Just like with `std::`: `float` in, `float` out; `double` in, `double`
out. Exact functions like `sqrt` and `floor` stay with `std::`.

**The rules for new code:**

- Whatever **feeds the simulation** uses `kern::dm::`.
- Sums in a fixed order, never depending on the number of threads.
- No `-ffast-math` options.

Pure display may use the normal library; sound, primitives, particles, parts of the editor and the
camera of `hello` do that. But if such code delivers values for physics, control or training, it
has to switch to `kern::dm::`.

> **For pros:** The fingerprint from [Chapter 2](02-building-and-testing.md) shows every error down
> to the exact control tick. Nothing gets slower: musl is as fast as the computer's own library,
> and most of the time goes to Box3D's joint solver anyway.

## Level complete!

- You report errors with `fail()` and catch NaN with `!(x > 0)`.
- You know why a span must not live longer than its data.
- You know Kern's JSON and its three promises.
- You know the rules: meters, **Z up**, matrices column by column, quaternion (x, y, z, w).
- You know why the simulation needs `kern::dm::` instead of `std::`.

## Puzzle corner

1. Which direction is "up" in Kern?
2. Why does `!(x > 0)` catch more errors than `x <= 0`?
3. In which of the 16 slots is the translation in Z?
4. **Boss puzzle:** A controller computes the duck's walking direction with `std::atan2`. The
   fingerprint on the Mac differs. Why, and how do you fix it?

### Solutions

1. Toward +Z.
2. Every comparison with NaN is false: `x <= 0` would let NaN through; `!(x > 0)` becomes true and
   catches it.
3. In slot 14 (column 3, row 2: 3 · 4 + 2 = 14).
4. `std::atan2` comes from the computer's own library and rounds differently in the last bit there.
   Because the value feeds the simulation, the difference grows. The fix: `kern::dm::atan2`.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and Rights](00-imprint.md)
