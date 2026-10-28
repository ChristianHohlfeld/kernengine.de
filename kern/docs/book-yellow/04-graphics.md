# 4 Graphics – How Kern Paints 60 Pictures per Second

> **Your level:** Now you get to watch the engine paint. You'll find out how C++ code in the browser
> reaches the graphics card, how triangles, light and shadow turn into a picture, and which tricks
> Kern uses to do as little work as possible. By the end, the windows of an entire city light up –
> without anyone modeling a single window.

This chapter shows **what Kern actually does**. The foundations – coordinates and matrices, how a
triangle becomes pixels, shaders, the depth buffer, light, shadows, PBR, gamma – are explained in
[Appendix B](B-rendering.md), with pictures and worked examples. If something here sounds strange,
flip over there.

## 4.1 Overview

The **renderer** paints the pictures. Three ground rules hold it together:

1. **One program for everything.** Every object is painted with the same shader. Games change the
   look through small extensions that get plugged into this one program (4.6).
2. **Upload once, then only movement.** The shape goes to the graphics card once; after that, only
   *where* something is. The work per frame depends on what moves – not on how big the world is.
3. **Kern's own copy is the truth.** Kern keeps the vertices, colors and positions of every object
   itself. From them it works out what's visible and what you can click on – and it rebuilds
   everything when the browser takes the graphics card away (4.11).

> **Picture it like this:** A theater. The sets are built once, and during the show only the actors
> move around. And the director has the plan of the stage in their head – if it burns down, they
> rebuild it exactly the same way.

## 4.2 The WebGL2 Binding

Kern is compiled to WebAssembly with the tool *wasi-sdk* – without the well-known Emscripten and
without automatically generated JavaScript glue. The graphics commands are ordinary **imports**:
when it loads, the module says "I need `drawElementsInstanced` from the package `kern_gl`," and
JavaScript hands the function in. The list in `gl.h` is "exactly what the engine uses"; a new command
gets added there and in `web/kern.mjs`. Tests without a browser use a stand-in that just takes notes.

**Only numbers** cross the border:

- **Graphics objects are numbers.** JavaScript puts the real object into a table and hands back the
  slot number; 0 means "nothing."
- **Data is an address plus a length.** "Starting at memory location X, this many bytes" –
  JavaScript reads directly from the module's memory, without copying. If the memory grows, it
  renews its views onto it.

`kern.mjs` has exactly three jobs and no game logic: load the module with its imports (graphics,
plus clock, terrain height, status, pointer lock, log, the fast path of the logic graphs from
[Chapter 9](09-logic.md) and a mini-WASI for console, randomness and clock), manage the number
tables including context loss, and offer the wrapper `KernRenderer`. **The engine doesn't touch any
files.**

The web page creates the drawing canvas – in `kern.mjs` without anti-aliasing, because the renderer
paints into its own framebuffers (4.7). Kern Studio and exported games switch it on.

**C++ games** write an `App`: every frame, first game logic, then painting
([Chapter 12](12-your-own-games.md)). **JavaScript programs** like SpawnCity take exactly one
`KernRenderer` per module. Kern Studio has its own way in ([Chapter 11](11-studio.md)) to the same
renderer.

## 4.3 Programs and the Uniform Cache

A **program** is a finished shader made of a vertex part and a fragment part
([Appendix B.3](B-rendering.md)). If compiling fails, the driver's message (up to 767 characters)
lands in the log with a label like "standard fragment shader:". If Kern asks for a setting the shader
doesn't even use, it gets slot 0 back – and everything at slot 0 is ignored. So anything optimized
away costs nothing and causes no error.

**Uniforms** are values that apply to a whole draw command, like "roughness = 0.5." Once set, they
stay put. The **uniform cache** remembers every value it last sent:

```cpp
void UniformCache::set_float(u32 location, f32 v) {
  if (!location) return;
  Entry& e = entry(location);
  if (e.kind == 2 && e.count == 1 && e.f[0] == v) return;
  e.kind = 2; e.count = 1; e.f[0] = v;
  gl::uniform1f(location, v);
}
```

Line 2: slot 0 – do nothing. Line 3: grab the note. Line 4: is this number already written there?
Done. Line 5: write down the new value. Line 6: only now actually send it.

For lists and matrices it only remembers a 64-bit "fingerprint" (hash); bones are deliberately sent
uncached, because they change every frame. It pays off because every command takes the long road
through JavaScript to the browser.

## 4.4 Mesh, Material and MeshInput

A **mesh** is a shape made of triangles; the **material** describes color, shine and glow.

### Fixed Slots

| Slot | Contents | applies per |
|---|---|---|
| 0–3 | position, normal, color, UV | vertex |
| 4–7 | model matrix | instance |
| 8 | tint with opacity | instance |
| 9–11 | normal matrix | instance |
| 12–13 | bones and weights | vertex |
| 14–15 | free for extensions | vertex |

Kern draws **every** mesh instanced ([Appendix B.3](B-rendering.md)), even if it only shows up once.
It computes the normal matrix ([Appendix B.1](B-rendering.md)) per instance on the processor; a
matrix that squashes an object flat is rejected. Limits: 32 bones, 6 point lights, 2 extension
streams, 8 extra numbers in the material.

### The Material

| Field | Meaning |
|---|---|
| `color`, `alpha` | base color; opaque, cut out, transparent |
| `roughness`, `metalness` | roughness, metal |
| `emissive` | self-glow, feeds the bloom |
| `unlit`, `double_sided` | no lighting; both sides |
| `cast_shadow` | casts shadows |
| `viewmodel` | first-person layer, painted last |
| `lines` | lines instead of triangles |

Colors are **sRGB** and get converted to linear for lighting ([Appendix B.6](B-rendering.md)), unless
a switch says "already linear"; `emissive` is always linear. Only **opaque** materials cast shadows.
The **first-person layer** (the weapon in your hand, Studio's gizmos) comes last, after the depth
buffer has been cleared – no wall can cut it off. **Lines** have neither light nor shadow and can't
be clicked. What gets skipped ([Appendix B.2](B-rendering.md)) is normally the back side; with
instances that are all mirrored, the front side; with double-sided meshes, lines or partly mirrored
meshes, nothing – a mirror image flips the winding order of the vertices.

### Creating with `upsert`

`upsert` gets positions (required) and optionally everything else; anything missing is filled in
(white, one instance at the origin, flat face normals or +Z). It's **all or nothing**: first Kern
checks that all numbers are real, every index points to a vertex, the lengths match and the four
bone weights of every vertex add up to 1 (±0.001). Only then is the new mesh built and swapped in for
the old one.

> **Crazy but true:** That's why an editor can't break anything while you type. An invalid
> in-between state doesn't delete any object – Kern just keeps showing the last good one.

Only the renderer itself hands out mesh numbers; that way, two parts of a program can never end up
sharing a slot.

Uploading creates fixed buffers for the vertex data, three changeable ones for the instances and one
for the triangle list. Up to 65,536 vertices, Kern stores each vertex number with 16 instead of 32
bits, because 2¹⁶ = 65,536 – more numbers don't fit. That cuts the triangle list in half.
Calm, large objects (36 triangles or more, one instance, no bones) get a search tree for clicking
([Chapter 5](05-scene-and-geometry.md)).

### Moving with `update_instances`

- Only new tints? Then only that buffer is sent; shadows stay valid. (Without exactly 4 numbers per
  instance, "all white" applies.)
- Everything identical byte for byte? Then nothing happens at all.
- More instances than there's room for? The space is **at least doubled**, so a number that keeps
  going up and down doesn't need new memory all the time.

Other commands change bones, visibility, material or layers. Every change to a shadow caster counts
the shadow counter up (4.8.1).

**Layers** are a number whose bits work like switches (default 1). They're used as a filter when
painting, for shadows and when clicking. That's how Kern Studio keeps the grid, icons, gizmos per
viewport, collision shapes and the worlds of several network players apart.

**Textures** are either created by Kern itself, which keeps the pixels (with mipmaps, tiled or in
pixel-art look), or JavaScript registers its own, together with a recipe for refilling them. A mesh
has at most **one**.

## 4.5 The Standard Shader

The standard shader is put together at startup from fixed building blocks; the snippets of the
extensions go in at fixed spots. What comes out is **one** program.

The **vertex shader** builds the model matrix from the instance columns, works the bones into
characters (*linear blend skinning*) and moves the vertex onto the screen. Skinning example: a vertex
at the elbow is attached 70% to the upper arm and 30% to the forearm. If the forearm rises by 10 cm,
the vertex moves along by 0.3 · 10 = 3 cm. That's how the skin bends smoothly.

The **fragment shader** writes **three results at once**: the finished picture (8 bits, sRGB, with
fog), the bright raw light for the bloom (4.8.3) and a record for the occlusion (4.8.4). Along the
way: read the texture, opacity (instance × material × texture), throw away cut-out pixels, make the
color linear, lighting, tone mapping, fog.

### Light and Material

Kern takes its cue from the standard material of the web engine three.js. The theory (Lambert,
Fresnel, GGX) is worked through in [Appendix B.4](B-rendering.md). Kern mixes seven ingredients:
**sun** (times shadow), **hemisphere light** (sky from above, ground from below), **sky gradient**,
up to 6 **point lights**, **shine**, **glass** and **glow**.

For the **sky gradient**, what matters is which way the surface faces: a vertical wall gets half
ground color, half sky color. Roughness goes from 0.04 to 1. Kern keeps the light from the sky and the
surroundings (*indirect*) **separate**, because later the occlusion is supposed to darken only that.
Metal has no matte part; `unlit` shows the base color pure. **Shine** only exists for metal or
roughness below 0.95; the sun's highlight is capped, so that no super-bright single pixels flicker in
the bloom. **Glass** (set by extensions) reflects the sky – the flatter you look at it, the more.
**Glow** has no upper limit; values above 1 turn into bloom.

> **Mission:** Open Kern Studio at https://kernengine.de/, choose **File › New Level...** ›
> **Basic** and use **+ Add** to place a **Sphere** (Shapes). In **Details**, under *Material*, set
> **Roughness** to 1 and **Metallic** to 0: matte. Then Roughness 0.1 and Metallic 1: the sphere
> reflects the sky. Finally, turn **Emissive** (color and strength) way up – it glows.

### From Raw Light to Pixel

First the raw light is saved for the bloom. Then it's multiplied by exposure ÷ 0.6, and the **ACES
filmic curve** (three.js's version) squeezes all brightness values into 0 to 1
([Appendix B.6](B-rendering.md)). After that comes Kern's **color grading**, which you can switch
off: saturation × 1.14, a soft S-curve (22%), warmer highlights, cooler shadows. Finally sRGB.

Like in three.js, the **fog** comes *after* the sRGB conversion. Three extras: **height fog** (Kern
averages the density along your line of sight; the haze hugs the ground, above the rooftops it's
clear), **sun scattering** (toward the sun, the fog takes on color) and **horizon fog** (4.8.2). If
all of them are off, the fog looks just like in three.js.

Into the **AO record**, the shader writes, per color channel, which share of the light is indirect
(already reduced by the fog), and how bright the base color is. If 0.3 out of 1.0 light comes from
the sky, it says 0.3 there. Transparent things and the first-person layer write 0, so they count as
not occluded.

## 4.6 Shader Extensions

A **shader extension** supplies GLSL snippets for fixed insertion points. The engine stays
unchanged; the game hands in its extensions at startup.

| Insertion point | Where | Allowed |
|---|---|---|
| `vertex_declarations` | vertex | own variables |
| `vertex` | vertex | change color, opacity, UV |
| `fragment_declarations` | fragment | variables, functions |
| `fragment_discard` | fragment, early | throw pixels away |
| `fragment_input` | fragment | switch "color is linear" |
| `fragment_surface` | before lighting | change color, glass, glow |

In front of each snippet, Kern puts a comment line with the extension's name, so errors can be traced
back to it. Extensions may bring their own data streams per vertex (1 to 4 numbers, at most two in
total, slots 14 and 15; if a mesh lacks the stream, the shader reads (0, 0, 0, 1)), integer material
values (at most 8 in total) and their own uniforms – best through the cache, and looked up again
after every rebuild of the program. An example follows in 4.13.

## 4.7 The Sequence of a Frame

[Appendix B.11](B-rendering.md) shows the overview as a picture. In detail:

0. **Check** the viewport and camera before anything gets changed.
1. **Measure GPU time**, if possible; the answer arrives frames later (at most 4 on the way).
2. **Sun shadows**, only if something has changed (4.8.1).
3. **Choose the target.** Without a sub-rectangle, Kern paints into its own framebuffers, as long as
   bloom is on and the device can handle it; then there's bloom and occlusion. Otherwise, and always
   with a sub-rectangle, it paints straight onto the screen, without either.
4. **Sort.** Invisible things, switched-off layers and everything outside the view frustum drop out
   – except characters with bones, whose pose can leave the stored outline. Opaque things are sorted
   **front to back** (hidden stuff fails the depth test early), transparent things **back to front**
   (so colors layer correctly).
5. **Opaque objects:** shared values once per frame, then one draw command per mesh; texture and
   face skipping are only switched when they change.
6. **Sky**, only with perspective (4.8.2). 7. **Transparent things**, without writing into the depth
   buffer.
8. **Occlusion** – *before* the first-person layer, because clearing for it destroys the world depth.
9. **First-person layer.** 10. **Composite** (4.8.3). 11. **Statistics.**

| Framebuffer | Size | Contents |
|---|---|---|
| shadow map | its own | depth as seen from the sun |
| scene: color, 4 bytes | full | finished picture |
| scene: HDR, 8 bytes | full | raw light |
| scene: AO, 4 bytes | full | indirect share |
| scene: depth | full | for GTAO |
| bloom | 1/4 and 1/8 | soft light |
| GTAO | half or full | visibility, depth |

4 + 8 + 4 = **16 bytes per pixel** – exactly the amount that fits into the fast tile memory of phone
chips that paint in tiles. That's why the AO record has only 8 bits per channel. The sky writes all
three results too; WebGL requires it.

![Four viewports in Kern Studio: partial views draw directly onto the screen](../buch/bilder/studio-quad.png)

Kern Studio paints each viewport separately. A single large viewport has no sub-rectangle and gets
bloom and occlusion; in the four-way view, both are missing.

> **Mission:** Load **File › Example Levels ▸ Orb Run** and, in the viewport menu **≡** under
> **Layout**, switch to **4 viewports**. With the button on the far right of a viewport's bar
> (**Maximise**), it becomes single and large again. Fly close to the corner of a house in the shadow
> and switch back and forth. The difference is subtle: in the large view, the corner often looks a
> bit darker and more three-dimensional, because only there does the occlusion run.

## 4.8 The Passes in Detail

### 4.8.1 Sun Shadows

The idea of the shadow map and its typical errors are in [Appendix B.5](B-rendering.md). Here: how
Kern solves them. Kern uses **one** depth map as seen from the sun, with parallel rays.

| Field | Default | allowed |
|---|---|---|
| resolution | 512 texels | power of two 128–4096 |
| half width | 120 m | 10–2000 m |
| depth bias | 0.0012 | 0 to below 0.04 |
| filter | 9 samples | 1, 4 or 9 |

Kern Studio picks the filter by device: phones 4, computers 9.

**Shadow distance.** World Settings › **Shadow distance** sets how far into your view the map
reaches (default 80 m, 10 m in the Micro Duck level). Kern puts a **sphere** around this part of the
view frustum. Why a sphere? Spin around on the spot – it stays the same size, and so do the texels.

A worked example: the editor camera in Studio has a 70° field of view, let's say the picture is 16:9,
and the map has 2048 texels (that's how Studio sets it up). At 80 m, the formula from the blue book
gives a sphere radius of about 114 m. The map is 2 · 114 = 228 m wide, one texel 228 ÷ 2048 ≈
**11 cm**. At 10 m: radius a bit over 14 m, one texel ≈ **1.4 cm**. A smaller distance means sharper
shadows that don't reach as far.

**Snapping.** If the map slides along with the camera by fractions of a texel, shadow edges land on
the grid a little differently every frame – they "swim." So Kern rounds the position to **whole
texels**. Along the sun direction, a coarse 8 m step is enough.

> **Picture it like this:** Graph paper on which you may only move your drawing by whole squares.
> Then every line always lies on the same squares.

**Reusing.** The rounded position, sun direction and size make up a fingerprint; the shadow counter
and layers are added to it. If everything stays the same, nothing gets repainted.

> **Crazy but true:** If the camera stands still above a quiet city, the shadow costs **not a
> single** draw command after that.

**Painting and looking up.** The sun only "photographs" the depth of the opaque shadow casters;
*polygon offset* helps against acne. The remedies PCF and normal offset are explained in
[Appendix B.5](B-rendering.md): level 1 is the hardware's four-way comparison, levels 4 and 9 build
soft 3 × 3 and 5 × 5 filters out of it, following Ignacio Castaño (2013, the game "The Witness").
Outside the map, everything is lit; with a shadow distance, the shadow fades out over the last tenth.
Orb Run (C++) uses 2048 texels over ±110 m, so 220 ÷ 2048 ≈ 11 cm per texel; the example *hello* uses
1024 texels over ±40 m.

> **Mission:** Load Orb Run and fly close to a house, until the ground is less than 10 m in front of
> you. In **World Settings**, set **Shadow distance (smaller: sharper)** to 10, then 80, then 500. At
> 10, the shadows right in front of you are sharpest, and nothing farther than 10 m has any. At 500
> they reach far, but their edges get coarser.

### 4.8.2 Sky

The sky is a single screen-filling triangle all the way at the back on the far plane, painted
**after** the opaque things. The depth test only lets it into the gaps – the expensive sky shader
only computes where you can actually see sky. It mixes a **gradient** from horizon to zenith, the
**golden hour** (low sun: horizon orange, zenith violet), the **sun** with halo and disk, the **moon
and stars** at night, **clouds** made from noise in 5 levels of detail that drift with the wind (an
explosion pushes them away), and, if you want, **horizon fog** in the scene's fog color. The physics
behind it is in [Appendix B.9](B-rendering.md). Views without perspective have no sky.

### 4.8.3 Bloom and Composite

What **bloom** is, [Appendix B.7](B-rendering.md) explains. For it, Kern needs framebuffers with
decimal numbers; if they're missing, it reports "unsupported" and paints directly. The steps: cut out
the bright parts (soft threshold from 0.88 to 1.23) at a quarter of the resolution, blur, shrink to an
eighth, blur once more, composite. The fine level counts with 0.13, the wide one with 0.08.

There's a clever trick in the **composite**: the finished picture already contains the fog, the raw
light doesn't. Tone mapping it again would erase the fog. So Kern runs "raw light with bloom and
occlusion" and "raw light without" through the same curve and adds only the **difference**. Pixels
with neither are just copied.

**Frame-time guard.** Kern keeps a moving average of the frame duration (outliers above 200 ms don't
count). If it stays above 27 ms for 3.5 s – that is, below about 37 frames per second – the bloom
switches itself off, unless it's forced; the composite keeps running for the occlusion. If the page
is hidden, the guard starts over – after all, the browser deliberately slows down background tabs.

### 4.8.4 Ambient Occlusion (GTAO)

Less sky light gets into corners and cracks; [Appendix B.8](B-rendering.md) explains the GTAO
method. Kern follows Intel's XeGTAO. What's special is how it's built in, **without an extra geometry
pass**:

1. The normal painting pass notes down the indirect share of every pixel (4.5).
2. GTAO works out from the finished depth buffer how occluded each point is.
3. The composite darkens **only** the indirect share.

Kern also saves a depth pass: chips from Apple and Mali throw away hidden stuff by themselves anyway.

Kern **guesses the normal from the depth** (the neighbor with the smaller jump in depth is more likely
on the same surface). The radius is converted into pixels, at most 20% of the picture height; below
1.5 pixels, a point counts as unoccluded. Distant points count less and less over the last 61.5% of
the radius. The visibility is also raised to the power of 2.2 (at least 0.03). A 4 × 4 pattern
rotates the directions from pixel to pixel; a filter averages it away without smearing across edges.

**Multi-bounce:** bright surfaces throw light back and forth. At visibility 0.5, a dark surface
(brightness 0.1) stays at 0.51, a bright one (0.8) rises to 0.81.

| Quality | Directions | Steps per side |
|---|---|---|
| low | 1 | 2 |
| medium | 2 | 2 |
| high | 3 | 3 |
| ultra | 9 | 3 |

Default: on, medium, half resolution, radius 1.6 m, adaptive – a guard like the one for bloom steps
down to *low* on slow devices, and then to half resolution. Studio uses medium at half resolution on
phones, otherwise high at full resolution. AO only runs with its own framebuffer and perspective. The
scene sets the radius: World Settings › **Ambient occlusion radius (0 = off)**.

> **Mission:** In **World Settings**, set **Ambient occlusion radius (0 = off)** to 0, then to 3. In
> a large viewport, look at house corners in the shadow and at the ground under objects. The effect
> is subtle; it's clearest in the shadow, because almost only indirect light arrives there – and
> that's the only light the occlusion darkens.

## 4.9 Particles and Point Lights

**Particles** are small squares turned to face the camera – sparks, smoke, muzzle flashes – and all
of them together are **one** instanced, transparent mesh without collision. Each one has a starting
position, speed, color, lifetime (default 350 ms, 16 to 10,000), size (0.35 m), gravity, growth and
extra fade-out; a system holds 1 to 4096. The path is the throw from physics class: if a spark starts
at 5 m/s upward with 9.81 m/s², after 0.5 s it's at 5 · 0.5 − ½ · 9.81 · 0.5² ≈ **1.27 m**. Time
never runs backward (otherwise old sparks would come back), sorting is back to front, and the opacity
drops to 0. No light, no shadow; the game runtime holds 512 particles.

The shader only knows 6 point lights. The **LightPool** picks the most important ones each frame
out of any number of requests:

> Points = intensity ÷ (1 + distance ÷ range), range at least 4

Lamp A (intensity 10, range 5, 20 m away): 10 ÷ 5 = 2. Lamp B (intensity 4, range 20, 10 m away):
4 ÷ 1.5 ≈ 2.67. B wins, even though it's weaker. In a tie, the earlier request wins; intensity and
range are capped at 100 and 1000. Orb Run requests one headlight and one light per orb.

## 4.10 Settings

**At startup:** 960 × 540 pixels, shadows off (512 / 120 / 0.0012 / 9), bloom, AO and grading on –
all changeable later. **Light** ("production day," linear): sunlight (2.8, 2.47, 2.11), fog density
0.00085, exposure 1, ambient 0.9, atmosphere extras off. The sky has its own values for sun, moon,
night, clouds, wind and explosion. In the game, the runtime computes everything from the **time of
day**, with sun to sky light about 5 : 1 ([Chapter 8](08-game-runtime.md)). **Per frame** there's a
background color, sub-rectangle, "page hidden," layers and sky on/off. **Camera:** up is +Z, 60°
field of view, near plane 0.05 m, far plane 1800 m. (The editor viewports in Studio use 70°.)

## 4.11 Context Loss

The browser is allowed to take the graphics card away from Kern **at any time** – when memory runs
low, when the driver restarts, or when the phone sends the page into the background.

1. **Loss:** Kern forgets all graphics numbers (the objects are already dead) and stops painting.
   The copies stay, and `upsert` keeps working.
2. **Recovery:** rebuild programs, passes and extensions, upload everything from the copies.
3. **If that fails,** Kern cleans up and tries again from scratch later.

The same path allows running **with no graphics card at all** – that's how the Lehrer
([Chapter 14](14-lehrer.md)) computes the same world without painting.

> **Watch out, trap:** If you keep your data only on the graphics card, you'll see black after a
> context loss. That's exactly what ground rule 3 is for.

## 4.12 Performance Tips

- **Instancing:** The city paints all tree trunks as one mesh, all treetops as a second one.
- **Merging:** a few large meshes instead of many small ones.
- **Send only changes**; pure color changes leave the shadow valid.
- **Go easy on shadows:** every moving shadow caster repaints the whole map. Streets, ground and
  particles don't need one – `cast_shadow = false`. With a shadow distance, a moving camera also
  repaints the map as soon as the focus shifts by a texel.
- **Indices:** `weld` ([Chapter 5](05-scene-and-geometry.md)) turns loose triangles into indexed
  ones.
- **Leave the frustum test on**, unless the outline is wrong (particles).
- **Measure:** the statistics list commands, triangles, instances and times; in Studio,
  **Show › Stats** shows the frames per second. Bloom and AO adapt by themselves.

## 4.13 Example: Windows That Light Up at Night

The houses in Orb Run have **no** windows in their shape. Even so, thousands of them light up at
night. Every vertex carries a "wall record": wall length (−1 roof, −2 cornice), wall height, a random
number for the house and the story height. The UVs of the walls are meters: u along the wall, v
upward. The heart of the extension, shortened:

```glsl
float column = floor(u / 3.0);
float floorIndex = floor(z / storey);
vec2 cell = vec2(fract(u / 3.0), fract(z / storey));
float win = step(0.22, cell.x) * step(cell.x, 0.78)
          * step(0.28, cell.y) * step(cell.y, 0.78);
```

Line 1: the wall is split into **3 m wide columns**; at u = 10 m, that's column 3. Line 2: the same
for the stories. Line 3: `fract` is what's left after the decimal point, so where you are in the
cell: 10 ÷ 3 = 3.33, remainder 0.33. Lines 4–5: `step(a, b)` is 1 if b ≥ a, otherwise 0. There's a
window where the remainder lies between 0.22 and 0.78 horizontally and between 0.28 and 0.78
vertically. 0.33 fits horizontally – if the height fits too, you're standing in front of a window.

Also: the data stream "wall" sits at slot 14 and goes to the pixels **unchanged** (`flat`) – blending
would scramble the random number. Windows only appear on walls, starting one floor above the ground
floor, up to 1 m below the roof; a random number made from column, story and house switches about 58%
of them on. If a window gets smaller than a pixel (`fwidth` measures that), the facade glows evenly
instead – against flickering. Dark windows become **glass**, lit ones get a warm **glow** (up to 1.6)
and shine in the bloom. `uNight` arrives once per frame through the cache. Streets and ground have no
stream; there the shader reads 0, and nothing happens. Kern Studio and exported games always register
a copy of this extension; in the **Procedural City** component, **Lit windows at night** decides
whether the houses get the stream ([Chapter 5](05-scene-and-geometry.md)).

> **Mission:** Load Orb Run and, in World Settings, drag **Time of day** to about 0.9 (0.25 is
> sunrise, 0.5 noon, 0.75 sunset; at 0.9 it's full night): the windows turn on. Select the actor
> **City** in the Outliner and, in Details, switch **Lit windows at night** off and back on.

## Level complete!

- C++ controls the graphics card through numbers and memory addresses.
- Three ground rules: one program, upload once, the copy is the truth.
- Mesh, material, instance – and why `upsert` is all or nothing.
- The sequence of a frame, from checking to compositing.
- Why Kern's shadows don't jitter: sphere, snapping, reusing.
- Bloom and occlusion without an extra painting pass.
- One extension lights up an entire city.

## Puzzle corner

1. Why are opaque things painted front to back, but transparent things back to front?
2. A mesh has 70,000 vertices. Does it get 16-bit indices?
3. Why is ambient occlusion missing in Studio's four-way view?
4. Lamp A: intensity 8, range 10, 30 m away. Lamp B: intensity 3, range 30, 30 m away. Which one
   wins?
5. **Boss puzzle:** The camera stands still, and a car drives through the city. Does the shadow map
   get repainted? And what if the car had `cast_shadow = false`?

### Solutions

1. That way, hidden stuff fails the depth test early; transparent things have to layer correctly on
   top of each other.
2. No: 16 bits are enough for 2¹⁶ = 65,536 numbers; 70,000 vertices need 32 bits.
3. Every viewport has a sub-rectangle and is painted directly – no own framebuffer, no AO.
4. A: 8 ÷ (1 + 3) = 2. B: 3 ÷ (1 + 1) = 1.5. A wins.
5. Yes: a shadow caster moves, the shadow counter goes up, and the whole map is repainted. Without
   `cast_shadow`, the map would stay valid – but the car would have no shadow.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
