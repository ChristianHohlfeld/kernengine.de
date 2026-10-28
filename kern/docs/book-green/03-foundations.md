# 3 Foundations: The Groundwork Beneath Everything

> **In short:** Every floor of Kern stands on four building blocks: a toolbox with number types,
> error messages and helpers, a JSON reader of its own, math for 3D space, and trigonometric
> functions that deliver exactly the same bits on every computer – along with the reason behind
> every decision.

If the basement is crooked, so is the whole house. The four building blocks are called
`core/base.h`, `core/json.h`, `math/math.h` and `core/dmath.h`.

## 3.1 The Toolbox: `base.h`

### Numbers with a Clear Size

An `int` can be a different size depending on the computer. That's why Kern always spells out the
width: `u8` to `u64` (unsigned), `i8` to `i64` (signed), `f32` and `f64` (decimal numbers); the
number gives the bits. At the boundary to WebAssembly, every bit counts: there, a pointer is 32 bits
wide, `f64` is exactly JavaScript's `Number`, and the interface's data are 32-bit words
([Chapter 17](17-reference.md)).

### When Something Goes Wrong: `fail()` and `last_error()`

Kern does without "exceptions," the emergency button of many languages. A function that can fail
returns `false` (or 0, or an empty pointer) and first leaves a readable message with `fail()`. The
trick: `fail()` **always** returns `false`, so every check becomes a single line:

```cpp
if (!camera_valid(camera))
  return fail("invalid camera");
bool ok = false;
const Mat4 view = Mat4::look_at(camera.eye, camera.target,
                                camera.up, &ok);
if (!ok) return fail("degenerate camera direction/up");
```

Invalid camera: leave a message, return `false`. Otherwise, the view direction is computed, and
`ok` says whether that worked (it doesn't, for instance, when the eye and the target are at the
same point).

> **Imagine …** A hotel has exactly **one** bulletin board. Whoever has something to report pins up
> a note and takes the old one down. Whoever checks always reads only the newest one.

That's how the error channel behaves: one slot for the whole program, 1024 characters, shared by
all threads; each message overwrites the previous one. JavaScript reads it with `kern_last_error()`
and clears it with `kern_clear_error()`. The JSON parser and a few other modules, on the other
hand, write their messages on a note of their own.

> **Watch out!** `last_error()` shows the message of the **last** `fail()` – not necessarily that of
> the call that just failed. If you want to be sure, call `clear_error()` first.

Kern likes to check `!(x > 0)` instead of `x <= 0`. That catches **NaN** ("not a number," for
example 0 divided by 0): every comparison with NaN is false, so `!(NaN > 0)` is true.

### Logging, Clock and Little Helpers

`log()` writes at four levels (`debug`, `info`, `warn`, `error`): in the browser, with the prefix
`[kern]`, to the console – errors as `console.error`, warnings as `console.warn`, everything else
as `console.log` – or to a receiver of the host's own choosing; natively, warnings and errors go to
`stderr`, the rest to `stdout`. If, say, the glow effect bloom can't start, Kern logs
`bloom disabled:` plus the message from the bulletin board – and the game keeps running without the
glow.

`now_ms()` returns milliseconds from a clock that only ever moves forward (in the browser,
`performance.now()`) – for measurements and time budgets, for example when the sound bank may only
spend a limited time precomputing sounds.

> **Watch out!** The clock has no business in the simulation! Physics and robots count **fixed
> steps**. If the duck looked at the clock, it would walk differently on a slow computer than on a
> fast one ([Chapter 6](06-physics.md), [Chapter 13](13-micro-duck.md)).

`min_of`, `max_of` and `clamp` return the smaller value, the larger value, and "keep it between two
limits"; with mixed types, you name the type, e.g. `min_of<u32>(…)`.

### `Span`: A Window onto Data

A `Span` consists only of `data` (where do the values start?) and `size` (how many are there?). It
doesn't **own** the data, it just points to it – like `std::span`, only lighter, and `data` and
`size` are plain fields instead of methods. Any container with `data()` and `size()`, such as a
`std::vector`, automatically becomes a span. This is how data flows into the engine, from C++ as
well as from JavaScript: `MeshInput`, the package for a mesh, is made of spans too, and the C
interface builds them directly from pointers into memory.

> **Imagine …** A span is a note: "Shelf 4, the next 20 books." Light and handy – but if someone
> clears out the shelf, it points at nothing.

That's why a span must not live longer than its data. If you want to keep the data, you **copy**
it – like the renderer, which keeps copies for picking and for rebuilding after a graphics loss
([Chapter 4](04-graphics.md)), or the geometry jobs, because JavaScript reuses its staging memory
right away.

### Hashes

A **hash** turns any number of bytes into a short, fixed number. With `hash_bytes` (**FNV-1a**,
64 bits, choosable starting value), the renderer recognizes unchanged values and the key of its
shadow map – and the duck's fingerprint from [Chapter 2](02-building-and-testing.md) is an FNV-1a
too. `hash_u32` (*lowbias32*) mixes numbers, for example for noise that still always comes out the
same. Both compute with whole numbers only: the result depends only on the bytes, never on the
computer.

## 3.2 JSON: Kern's Favorite Format

**JSON** is a text format for structured data, such as `{"name": "Duck", "legs": 2}`. Kern needs it
for levels (`.klevel`), Studio commands, servo profiles, training data and the Micro Duck's list of
networks. Its JSON building block promises three things:

1. **The order stays put.** If Kern or JavaScript wrote the file, reading it in and writing it back
   out gives the same thing, byte for byte. If one value in the level changes, only one line of the
   file changes.
2. **Numbers look the way they do in JavaScript** – the shortest sequence of digits that describes
   the number unambiguously, exactly as with `JSON.stringify`.
3. **Distrust of files from outside.** Rejected are infinite numbers, nesting that is too deep
   (more than 80 levels; Studio commands may have 120) and the keys `__proto__`, `prototype` and
   `constructor`, which could cause mischief on the JavaScript side (*prototype pollution*).

> **Did you know?** Kern writes `0.30000000000000004` – exactly like JavaScript does for 0.1 + 0.2.
> A test checks cases like this, for example 10²¹ → `1e+21` and NaN → `null`.

A `json::Value` is null, a Boolean, a number, a string, a list or an object. It can tell you its
type, return values (`truthy()` even with JavaScript's truthiness rules), edit lists and objects
and compare itself with `==`; the complete list of functions is in the blue book. Here's how it
makes code safe:

- A missing key returns **null** instead of crashing; `v["a"][2]["b"]` always works.
- `as_number` and `as_bool` return a fallback value if the type is wrong. If you want to tell
  "missing" apart from "wrong type," ask with `find` and `is_number`.
- `set` replaces a key **in its place**; `push` first turns a non-list into an empty list.
- `parse` writes the result **only on success**. Error messages name the position
  (`at offset 17`); text after the value gives `trailing characters`; a UTF-8 BOM at the start is
  allowed. Depth and the key block can be configured in the `ParseOptions`.
- `stringify(v, indent)` works like `JSON.stringify(value, null, indent)`: compact for
  `indent <= 0`, indented otherwise; non-finite numbers (infinity, NaN) become `null`, −0 becomes
  `0`.

### An Example: Servo Profiles

A **servo profile** describes one motor of the Micro Duck ([Chapter 16](16-calibration.md)). Kern
reads it following a fixed pattern: parse the text; if it isn't an object or `kind` isn't `servo`,
give up. Look up each known field one at a time – it's allowed to be missing. If it's there, it has
to be a number in the allowed range, otherwise give up with `bad value for` and the field name.
**Only at the very end** is the finished profile adopted; a half-broken one never ends up in the
program. It is written out with an indentation of two spaces.

> **Try it!** In the Studio, save a level with **File → Save** (Ctrl+S) and open the `.klevel` file
> in a text editor. That's Kern's JSON: keys in a fixed order, numbers just the way JavaScript would
> write them.

## 3.3 Math for 3D Space

`math.h` lives entirely in the header and never allocates memory, so that the compiler can make it
especially fast. First, the ground rules:

| Rule | Convention |
|---|---|
| Units | meters, seconds, kilograms |
| Coordinates | right-handed, **+Z is up** |
| Angles | radians in code, degrees in levels |
| Matrices | 4×4, stored column by column |

> **Imagine …** Lay a sheet of graph paper on the table: X points to the right, Y points forward, Z
> points straight up to the ceiling. Right-handed means: stick out the thumb, index finger and
> middle finger of your right hand at right angles – those are X, Y and Z.

Column by column means: the 16 numbers sit in memory one column after another, with the translation
in slots 12, 13 and 14 – as in OpenGL. Points are multiplied onto the matrix from the right
(`p' = M * p`). In camera view, you look along −Z, and the projection lands in OpenGL's image space
with a depth from −1 to 1.

Z up applies everywhere: renderer, physics, sound, levels, basic shapes. In **radians**, half a
circle is π (about 3.14) instead of 180°; levels and the Details panel in the Studio show degrees
([Chapter 8](08-game-runtime.md)).

> **Try it!** Open **File → New Level → Physics Lab**, type `gravity 0 0 -2` into the console at the
> bottom (**Cmd**) and start **Simulate**. Gravity now points weakly toward minus Z – everything
> falls almost like on the Moon. `gravity` without values restores 0 0 −9.80665.

### Vectors and Quaternions

`Vec2`, `Vec3` and `Vec4` are arrows (or points) made of two, three or four numbers, with length,
distance, blending (`lerp`), minimum/maximum, dot and cross product, plus `finite` (is everything
finite?) and `hex_color(0xRRGGBB)` for colors. `normalize`, which brings an arrow to length 1, is
clever: if the arrow has length zero – say, after two clicks on the same point – it returns a
**fallback value** (pointing up by default) instead of NaN, which would eat its way through the
scene like a virus.

Kern stores rotations as a **quaternion**: four numbers `(x, y, z, w)`; the default `w = 1` means
"no rotation." Sounds scary, but it's simple to use: `axis_angle(axis, angle)` creates a rotation
(bringing the axis to length 1 along the way), `a * b` rotates by `b` first, then by `a`, and
`rotate` rotates a vector.

### Matrices

A **matrix** is a 4×4 table that moves, rotates and scales a point in one go. The most important
tools:

- `trs` builds one from those three parts.
- `perspective` and `ortho` build views with and without a vanishing point, `look_at` the camera's
  view matrix. If the view direction is impossible, `look_at` returns the identity matrix (which
  changes nothing) and `ok = false`.
- `transform_point` moves a point, `transform_vector` only a direction (without translation), and
  `transform` computes with all four numbers, for example on the way to the screen.
- `invert` inverts a matrix, in double precision, so that clicks hit their target even far away.
  If the matrix is (almost) not invertible, it reports `false`.
- `normal_matrix3` makes sure that normals – arrows pointing straight out of a surface – stay
  perpendicular even after uneven stretching.

With `A * B`, **B acts first**, then A. A test checks `trs` on the point (1, 0, 0): scale by 2
→ (2, 0, 0), rotate 90° around Z → (0, 2, 0), translate by (1, 2, 3) → **(1, 4, 3)**. That's
exactly how every crate in `hello` gets its matrix, 16 numbers per crate.

> **Watch out!** Walking first and then turning is different from turning first and then walking –
> you end up somewhere else. That's why `trs` fixes the order.

### Spheres, Boxes, Camera

`Sphere` and `Aabb` (axis-aligned box) help with sorting out what can't be seen. An invalid sphere
(radius −1) is drawn anyway: **when in doubt, draw**. An empty box starts out "inside out," so that
the first point sets it correctly.

The **camera** isn't a matrix but a description: eye, target, up. Default: eye (0, −18, 8), target
(0, 0, 2), up +Z, field of view π/3 = **60°**, measured **vertically**, drawn from 0.05 m to
1800 m. The Studio's top, front and side views use an orthographic camera with a visible height
(default 20 m). Anything invalid (for example `far` not greater than `near`) is rejected by
`camera_valid`. `projection_matrix` picks perspective or parallel view. Projection times view matrix
gives the view-projection matrix, the whole path from the world to the screen. From it,
`Frustum::from_matrix` extracts the six walls of the visible region, which Kern uses to sort out
what can't be seen. JavaScript sets the camera with twelve numbers via `kern_camera_set`, which
rejects anything invalid with `invalid camera`.

## 3.4 Math That Computes the Same Everywhere

Square root, absolute value and rounding down deliver the same bits on every computer. Sine,
cosine, the exponential function and friends do **not**: Linux, macOS, Android and WebAssembly
round differently in the last bit. In a robot simulation, one bit turns into a different gait
within seconds – and a network learned in the browser would run differently natively.

> **Imagine …** Two kids roll dice to pick a hiking route, one roll at every crossroads. If only the
> first roll differs, they end up in different villages by evening.

The solution: Kern brings along the functions of the C library **musl** and compiles them the same
way on all three build paths, without merged computing steps. New names (`sin` becomes
`kern_musl_sin`) prevent mix-ups with the system's versions. Box3D computes on the scalar path and
has its own deterministic trigonometric functions anyway.

You use them in the namespace `kern::dm` exactly like `std::`: `sin`, `cos`, `tan`, `asin`, `acos`,
`atan`, `exp`, `expm1`, `log`, `log10`, `atan2`, `pow` and `hypot`. With `float`, the `float`
version computes; otherwise the `double` version does. `sqrt`, `fabs` and `floor` stay with
`std::`, because they are exact anyway. From Kern's math:

```cpp
static Quat axis_angle(Vec3 axis, f32 radians) {
  const Vec3 a = normalize(axis);
  const f32 s = kern::dm::sin(radians * 0.5f);
  return {a.x * s, a.y * s, a.z * s,
          kern::dm::cos(radians * 0.5f)};
}
```

Bring the axis to length 1, multiply the sine of half the angle – with `kern::dm::sin`! – onto the
axis, and put the cosine in as the fourth number. That way every rotation gives the same bits
everywhere. `perspective` uses `kern::dm::tan` too.

The rule for new code:

- Everything that **feeds the simulation** uses `kern::dm::` instead of `std::`.
- No sum may depend on the number of threads.
- No `-ffast-math` options.

Pure **presentation** – sound, particles, parts of the editor, the camera of `hello` – may use the
system math. If such code goes into physics, controllers or training, it has to move over to
`kern::dm::`.

> **Did you know?** Kern's own math doesn't cost any speed: musl is as fast as the system, and the
> speed of the physics is determined by Box3D's joint solver anyway. Whether everything is right is
> revealed by the fingerprint from [Chapter 2](02-building-and-testing.md).

## What you take away

- Kern reports errors with `false` and `fail()`; the message hangs on a single bulletin board.
- `!(x > 0)` instead of `x <= 0` catches NaN too.
- The clock is for measurements, never for the simulation.
- A `Span` is a window onto data; if you want to keep the data, copy it.
- Kern's JSON keeps the order, writes numbers like JavaScript and distrusts files from outside.
- Meters, right-handed, **Z up**, matrices column by column, field of view vertical in radians.
- For the simulation: `kern::dm::`, `-ffp-contract=off`, fixed sums.

## Brain teasers

1. Why does `fail()` always return `false`?
2. (1, 0, 0) is rotated 90° around Z and then translated by (5, 0, 0). Where does the point end up?
3. A particle effect uses `std::sin`. Problem? And what if the particles are supposed to nudge the
   duck?

### Solutions

1. So that `return fail("…");` leaves a message and reports failure in a single line.
2. Rotating gives (0, 1, 0), translating gives (5, 1, 0).
3. Allowed for pure presentation. If the particles affect the physics, `kern::dm::sin` is a must.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and Rights](00-imprint.md)
