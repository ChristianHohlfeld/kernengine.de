# 5 Scene and Geometry – What the Camera Sees and What the City Is Built From

> **In short:** This chapter is about two toolboxes that sit next to the renderer. The first one
> (`kern::scene`) answers questions like "Do I even need to draw this?" and "What's under the mouse
> pointer?". The second one (`kern::geo`) builds shapes: cubes, buildings from footprints, roads,
> terrain. At the end, Kern conjures up an entire city from them.

The renderer is covered in [Chapter 4](04-graphics.md), the math in [Appendix B](B-rendering.md).

## 5.1 The ground rules

- **Z points up**, and everything is measured in meters – in graphics, physics, audio and all the
  builders.
- All builders deliver their result in the same container, **`MeshData`**: lists of positions,
  normals (which way a surface "faces"), colors in sRGB, texture coordinates (UVs) and, optionally,
  indices.
- Without indices, a mesh is a **triangle soup**: every three corners make one triangle.
- On top of that there can be an **extra stream**: a few additional numbers per corner for a custom
  shader. The renderer has two slots for it; `as_input` passes the mesh, extra stream included, on to
  the renderer. If you pick a slot of 2 or higher, the extra stream is left out.

## 5.2 Frustum culling: what's not in the picture doesn't get drawn

The camera sees only a slice of the world, shaped like a pyramid with its tip cut off: the
**frustum**. Anything lying completely outside it doesn't need to be drawn. Sorting those things out
is called **culling**.

> **Imagine …** you're standing in a theater with a flashlight. Before you shine it on someone, you
> ask: Are they even on the stage? Whoever is waiting behind the curtain costs you nothing.

Kern tests **one sphere per mesh** against the **six walls** of the frustum: six small calculations
instead of thousands of corners – much cheaper than drawing. Kern reads the walls straight off the
camera matrix (the Gribb and Hartmann method) and normalizes them so that every calculation gives the
real distance in meters. If the center of the sphere lies more than one radius behind a wall, the
sphere is definitely outside.

The test is **cautious**: a sphere near a corner of the pyramid is sometimes drawn when it didn't need
to be, but it is never wrongly left out. Broken spheres count as visible.

The renderer computes the spheres when a mesh is uploaded: one around the mesh, and one in the world
around all of its **instances** (copies in different places). For their size it uses a measure of the
matrix (the Frobenius norm) that is guaranteed to stretch at least as much as the matrix itself.

> **Watch out!** All copies of an instanced mesh share **one** sphere. A forest whose trees are
> scattered across the whole map is therefore almost always "visible". It's better to split such
> instances into spatial groups.

Never sorted out are **skinned** meshes (characters with bones, whose pose can leave the sphere) and
meshes whose material switches culling off. The shadow pass uses the same test against the sun's
frustum.

> **Try it!** Open Kern Studio at https://kernengine.de/ and turn on **Bounds** under
> **Window › Show**. Now you can see the bounding shapes Kern calculates with.

## 5.3 The BVH: boxes in boxes

When you click into a city, Kern has to find the triangle you hit among tens of thousands. Check every
single one? Far too slow. The solution is called a **BVH** (*bounding volume hierarchy*, a hierarchy of
bounding boxes).

> **Imagine …** you're looking for a screw in the basement. The screws are in little boxes, the little
> boxes are in crates, the crates are on shelves. If a shelf says "paint only", you don't open a
> single box on it.

**Building:** Every triangle gets a box. Kern looks for the direction in which the triangles are spread
out the most and splits them right down the middle there (at the **median**). Each half gets a box
around it, and this goes on until a box holds at most 8 triangles. Because every level halves the
triangles, the tree is at most 32 levels deep.

**Searching:** If the ray hits a box, it moves on to both children – **the nearer one first**. If Kern
finds a hit there early on, all the farther boxes drop out at the next test.

Only **static world geometry** gets a BVH: no lines, a single instance that doesn't move, not skinned,
at least 36 triangles. If the mesh is moved or copied, the renderer throws the BVH away; once the mesh
is still again, it rebuilds it. Everything else is checked triangle by triangle.

## 5.4 Picking: what's under the mouse pointer?

Kern converts the pixel (measured in the graphics' drawing buffer) into coordinates from −1 to 1 and
projects it backward through the camera onto the near and far walls of the frustum. The line between
them is the **screen ray**. With a perspective camera, it starts at the eye; with an orthographic one
(no perspective), all rays run parallel.

Then, for every mesh that's a candidate:

1. **Coarse test:** Does the ray miss the world sphere, or does the sphere lie behind the best hit so
   far? Done.
2. **Fine test:** with a BVH, down through the tree; otherwise every triangle of every instance. For
   animated characters, Kern computes the current pose from the bones. **What you can see, you can
   click.**
3. **Triangle test** using Möller–Trumbore. Back faces only count for double-sided materials.

The hit tells you the mesh, instance, triangle, distance, hit point and which way the surface faces.
In the example `examples/hello`, a click colors the crate you hit red:

```cpp
PickHit hit;
if (engine.renderer.pick(in.pointer().x, in.pointer().y,
                         {}, hit) && hit.mesh == boxes_) {
  tints_[hit.instance * 4 + 0] = 1.0f;
  tints_[hit.instance * 4 + 1] = 0.25f;
  tints_[hit.instance * 4 + 2] = 0.2f;
}
```

Line 1 creates an empty hit. Lines 2–3 ask: Is there something under the mouse pointer, and is it the
crate mesh? `{}` means default settings: up to 1800 m away, and invisible meshes and lines don't
count. You can also exclude a mesh and choose layers; a small statistic counts the triangles checked
and the meshes rejected by their sphere. The three lines after that color exactly this crate: lots of
red, a little green and blue.

Kern Studio picks the same way (only on the scene layer, not on the grid or gizmos), maps meshes to
their actors and shoots the ray on through the ground and through locked actors. You select lights
and empty actors by their icon.

> **Try it!** Click different objects in the Viewport and watch the Outliner jump along with you.
> Every click is a ray searching for the nearest triangle.

## 5.5 Earcut: cutting polygons into triangles

A building footprint is a polygon, often L-shaped (not convex) and sometimes with holes (courtyards).
The graphics processor can only handle triangles. Kern cuts polygons up with **Earcut**, a C++ port of
Mapbox Earcut v3.0.2 (ISC license) which, according to its header comment, gives the same results as
the JavaScript original.

> **Imagine …** you have a paper polygon and a pair of scissors. You look for a corner whose triangle
> with its two neighboring corners lies entirely on the paper – an **ear**. Snip! What's left is a
> polygon with one corner fewer. Keep going until only one triangle remains.

Every simple polygon has at least two ears; so n corners turn into n − 2 triangles. An ear is a corner
that bulges outward and whose triangle contains no other corner.

- **Messy data:** If Earcut goes all the way around once without finding an ear, it removes duplicate
  points and points lying on a straight line, then repairs small self-intersections and, if need be,
  splits the polygon in two along a diagonal.
- **Holes:** Ears only work with a single outline. So Earcut cuts a narrow **bridge** from each hole to
  the outer outline (Eberly's method), starting with the hole that lies farthest to the left.
  Afterward, everything is one outline.
- **Large polygons:** With more than 80 corners, every corner gets a house number on a **z-curve**: the
  bits of x and y are interlocked like the teeth of a zipper. Points that are close together usually
  have similar numbers, and Earcut only checks the neighborhood instead of every corner.

The function `deviation` measures how accurately the triangles match the area (0 means exactly).

> **Did you know?** After cutting, Kern compares the area of all the triangles with the area of the
> footprint. If they differ by more than 0.7 %, the building is left out instead of standing in the
> city with a broken roof.

## 5.6 Primitives: the building kit

`kern::primitives` builds basic shapes with Z pointing up: box (also standing on the ground), sphere,
capsule, torus (donut), cylinder, cone and plane. With `append` you attach shapes to one another, with
an offset and a rotation, so that you get **one** mesh, which is faster to draw (`append` rotates the
normals correctly along with them, but doesn't copy the extra stream). This is how Orb Run builds its
car out of three boxes, and all the city's ground tiles become a single mesh.

## 5.7 Welding: merging duplicate corners

The building and road builders deliver triangle soup. Where six triangles meet, the same corner is
stored six times in memory.

An **indexed mesh** stores each corner once, plus a list of numbers. **Welding** turns soup into an
indexed mesh. Kern only merges corners that are bit-for-bit identical in **everything**: position,
normal, color, UV, extra values. That's why it's lossless: a hard edge stays hard, because the surfaces
there have different normals. A hash table (a fast-lookup directory) finds the twins. And Kern only
indexes if the bytes saved exceed a minimum you can choose; otherwise the soup stays as it is. Welding
only accepts soup (no indices) and at most four extra values per corner; a small statistic reports
corners and bytes before and after.

> **Did you know?** For a real section of Konstanz with 1300 buildings, building and welding take
> about 45 ms in Kern – the earlier JavaScript version took about 1260 ms.

A fingerprint of all the numbers (`content_hash`) tells you whether a newly built piece differs from
the old one.

## 5.8 Buildings from footprints

> **Imagine …** a cookie cutter pressed into a thick slab of dough: the shape of the footprint on top
> and bottom, vertical walls in between. Pulling a shape upward like this is called **extruding**.

The `BuildingBuilder` extrudes footprints into buildings with a roof, walls and optionally a cornice
(the band sticking out just below the edge of the roof). The footprints arrive as a long list of
numbers, so that JavaScript can pass them in easily too. For each building, the list contains a
**seed** (a random number for colors and facade), the base height and eaves height, optionally a
center point, and the rings: first the outer outline, then the courtyards. In C++, `FootprintStream`
writes this list for you. Without a center point, the first point of the outer outline decides which
chunk the building falls into.

For each building:

1. **Tidy up:** Points closer than 1.5 cm to the previous one are thrown out; rings that are too small
   (fewer than 3 points or under 0.08 m²) become empty.
2. **Orient:** outer outline counterclockwise, holes clockwise.
3. **Sort out:** empty rings, walls under 10 cm, more than 0.7 % area error.
4. **Colors** and story height (3.1 to 3.6 m) are decided by the seed.
5. **Roof** from the Earcut triangles, turned to face upward, with courtyards left open; **walls** on
   every edge (the courtyards' too), with UVs in meters along the wall and above the base.
6. **Cornice:** 0.55 m high, sticking out 0.24 m, only on edges over 1.2 m and walls over 3.5 m.

Every corner carries four **extra values**: wall length (or a marker for roof or cornice), wall height,
a value from the seed and the story height. From these, the shader extension `NightWindows` from
[Chapter 4](04-graphics.md) draws grids of windows that light up at night.

So that a large city doesn't freeze the picture, the builder works on a **time budget**: `step(4.0)`
works for about 4 ms and reports whether it's finished; in between, drawing carries on. The buildings
end up in square **chunks** (320 m by default, at least 20 m; at most 2600 buildings), each one its
own mesh. Bigger chunks mean fewer draw calls, smaller ones better culling. Orb Run and the game
runtime build their small city in one go with `step(1e9)`. Counters report buildings and triangles
built and skipped; the cornice and the extra values can be switched off.

## 5.9 Roads

The `RoadBuilder` lays roads onto the terrain as **ribbons**: optionally a curb, then the roadway and a
dashed center line (3 m long dashes every 7 m, starting at 1 m). The input is again a list of numbers:
for each road, its width, switches (curb, marking), two color numbers from a palette, and the points.
So that the ribbons follow hills, each piece is split into sections of at most 6 m, and every corner
asks for the ground height.

| Layer | Height above terrain | Width |
|---|---|---|
| Curb | 9 cm | road + 3.4 m |
| Roadway | 11.2 cm | road width |
| Marking | 13.5 cm | 42 cm |

> **Watch out!** If two surfaces lie at exactly the same height, the graphics processor doesn't know
> which one is in front, and they flicker against each other: **z-fighting**. The staggered heights
> prevent that.

The ground height comes from a **HeightSampler**, a small function meaning "give me the height at
(x, y)"; without one, the ground is flat. The source code calls it "physics truth": the game should
supply the same function as its physics does, so that cars drive exactly on the road you can see. If
it returns a number that isn't finite, the builder stops with an error. In JavaScript you simply set
`kern.heightSampler = (x, y) => …`. Roads outside a chosen radius are skipped – that's how you build a
section of a large map.

## 5.10 Terrain tiles

`build_terrain_tile` builds landscape as an indexed grid with 1 to 128 cells per side; every grid
corner asks the HeightSampler, and every cell is split into two triangles along the southwest–northeast
diagonal. The UVs run across the tile from 0 to 1, and the function reports the lowest and highest
height (handy for bounding shapes or levels of detail). The normals come from the height difference
between neighbors. Because the function samples one extra row beyond the edge to do this, neighboring
tiles fit together **seamlessly**.

The `GeoProjector` converts GPS coordinates into meters east and north of an origin (earth radius
6,378,137 m). That way, footprints from map data come straight into Kern's world.

## 5.11 Geometry from JavaScript

All the builders are available in JavaScript too: run a job bit by bit, then `commit` each chunk to
weld it, upload it and free its memory (after all, the renderer has its own copy); `commit` reports the
bytes before and after welding. With `extraKey` you choose the extension stream for the extra values;
`commit` rejects an unknown name with an error instead of silently taking a different one.
`upsertTerrainTile` builds and uploads a terrain tile in a single call, and with `weld: true` Kern
welds a triangle soup you upload yourself before it goes up. This only works for pure soup with
normals, no skeleton, no lines and at most one extra stream; otherwise it arrives unchanged.

## 5.12 The procedural city

Now everything comes together. **Procedural City** is Orb Run's city generator, packaged as a building
block for any level. "Procedural" means: not modeled by hand, but generated from rules and a seed –
buildings, roads, ground tiles, trees, collision bodies and traffic.

![An exported game: the procedural city with buildings, roads and trees](../buch/bilder/game.png)

| Setting | Default | Limits / meaning |
|---|---|---|
| Seed | 20261022 | starting value for the randomness |
| Blocks per side | 7 | 1 to 16 |
| Block pitch | 80 m | street to street, at least 30 |
| Street width | 16 m | at least 6, up to half the pitch |
| Tallest building | 84 m | in the center |
| Trees / Traffic cars | on / 22 | trees, cars |
| Lit windows at night | on | windows that light up |

Same seed, same city (the same random number generator as in Orb Run):

1. **Grid:** blocks with 18 cm high ground tiles, streets in between.
2. **Height:** The closer to the center, the taller: the height limit grows from 14 m at the edge to
   the height you set in the center (with the square of how close to the center a block is). The
   center becomes the high-rise district.
3. **Development** per block (lot inset by 3 m), one of four kinds: one block-sized building (in 60 %
   of cases with a 14 m deep courtyard, and then collision as four wings), four separate buildings with
   5 m alleys, an L-shaped building plus a small one, or three slab blocks.
4. **Streets** along all block edges (ribbon width three quarters of the street width) with curb and
   center line, reaching 60 m beyond the city. They're built with the builders from above (chunks of
   200 m, six wall colors and three roof colors), and everything is welded (welding, 5.7).
5. **Trees** every 11 m along the block edges, with 35 % gaps, random size, rotation and shade of
   green; all the trunks (cylinders) are one instanced mesh, all the crowns (cones) a second one.

When uploaded, each chunk becomes a mesh. If you move the city, only one matrix changes; it is only
rebuilt when a setting really changes (a fingerprint of the settings checks this). Buildings only get
the extra values for the windows when "Lit windows at night" is on. Roads cast no shadow and are
double-sided and completely matte. If the city sits at the origin without rotation, each chunk gets a
BVH, and picking in the city goes through it.

For physics, the whole city becomes **one** static Box3D body with one box per building wing and
ground tile (friction 0.5, see [Chapter 6](06-physics.md)). In the editor the cars are parked. In
Play and Simulate they drive along their lane as kinematic bodies (9 to 15 m/s, five colors), jump to
the other side at the edge of the city, and stop when the player is standing less than 10 m in front
of them. All the cars are a single instanced mesh. A helper function returns random points on the
streets, for example for scattering orbs.

> **Try it!** Open the Content Drawer (Ctrl+Space) and place a **Procedural City** from the **World**
> group into the level. Change the **Seed** or **Blocks per side** in Details – a new city every time.
> With Simulate (Alt+S), the traffic starts moving.

## 5.13 Shapes in the level and built-in meshes

A **Static Mesh** has a shape: Box, Sphere, Cylinder, Cone, Capsule, Plane, Torus, Ramp or Mesh. Kern
builds a white mesh from it (the color comes from the material). Box, plane and ramp get texture
coordinates in meters, so that a texture looks the same size everywhere. Every shape sits centered on
the actor. The ramp is a wedge: full height at +X, flat at −X. Helpers also supply the shape's bounds,
points for a convex hull and, with `fit_collider`, a matching collision body (box, sphere, capsule,
otherwise hull) – more in [Chapter 6](06-physics.md).

The **Mesh** shape refers to a triangle mesh compiled permanently into the engine. Today these are the
Micro Duck's links: Pollen Robotics' meshes in simplified form, with names like `duck/trunk_base` or
`duck/neck` (`builtin_mesh_names()` returns the full list); they were simplified by the tool
`tools/micro_duck_import.py`. They are licensed under CC BY-NC-SA 4.0 (non-commercial only), so they
aren't shown here, and a commercial build leaves them out. They are stored compactly (positions in 16
bits, normals and colors in 8 bits, plus hull points for collision) and unpacked on first access.
Because the Lehrer ([Chapter 14](14-lehrer.md)) builds on several threads, a lock (mutex) protects this
store natively; in the browser there's only one thread. In the level, such a mesh can be stretched per
axis; for a mirroring, Kern flips the winding order of the triangles too.

> **Did you know?** An unknown mesh name deliberately produces a 10 cm cube. That way the mistake
> stands out in the level right away, instead of there simply being nothing to see.

How the duck is put together from these parts is told in [Chapter 13](13-micro-duck.md).

## What you take away

- Z is up, the unit is the meter, and all builders deliver `MeshData`.
- Culling tests one sphere per mesh against six walls – cheap and never wrongly invisible.
- A BVH is boxes in boxes; it makes picking fast.
- Picking shoots a ray from the pixel into the scene, even onto animated characters.
- Earcut snips off ears, connects holes via bridges and uses a z-curve for large polygons.
- Welding only merges bit-identical corners – lossless, and only when it saves something.
- Buildings and roads are made in chunks on a time budget; they share the height function with the
  physics.
- Procedural City turns a seed into a city with trees, collision and traffic.

## Brain teasers

1. Why does Kern test a sphere during culling instead of all the triangles?
2. A forest of 5000 trees as one instanced mesh is never sorted out. What helps?
3. A polygon has 12 corners and no holes. How many triangles does Earcut make from it?
4. Why does a hard cube edge stay hard after welding?
5. What would happen if the roadway and the center line lay at exactly the same height?

### Solutions

1. The test has to be much cheaper than drawing; six distance calculations cost next to nothing.
2. Split the trees into spatial groups, each with its own sphere; groups outside the picture drop out.
3. n − 2 = 10 triangles.
4. At the edge, the surfaces have different normals, so the corners aren't bit-identical.
5. Z-fighting: the surfaces flicker because it isn't clear which one is in front.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
