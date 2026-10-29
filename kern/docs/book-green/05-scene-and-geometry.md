# 5 Scene and Geometry – What the Camera Sees and What the City Is Built From

> **In short:** Next to the renderer – the part of Kern that paints the picture – there are two
> toolboxes. The first answers questions like "Do I need to paint this at all?" and "What's under
> the mouse pointer?". The second builds shapes: cubes, houses from floor plans, streets, terrain.
> In the end, a whole city comes out of it – from a single starting number.

> **Key terms:**
> - **Triangle mesh** – every 3D shape on the screen is really a net of lots of small, flat
>   triangles, like a wire frame covered with fabric.
> - **Mesh** – such a triangle net as a finished thing in memory: a car, a house, a tree.
> - **Corner** – a corner point of the net (a vertex). Every corner carries a few details: where it
>   is, what color it has, which way the surface faces there.
> - **Normal** – an invisible arrow pointing straight out of a surface: "this is where I'm
>   looking". The light needs it to know how bright the surface is.
> - **UV** – the detail saying which spot of a picture (a **texture**, such as a brick pattern) is
>   stuck onto a corner, like wrapping a present: which spot of the paper ends up on which corner
>   of the box.
> - **Index** – a number that points to a corner. Instead of writing every corner down again for
>   every triangle, you note "triangle from corners 3, 7 and 8".
> - **Instance** – a copy of the same mesh in another place, such as a hundred identical trees.
> - **Seed** – a starting number for chance. The same number always gives the same "random"
>   results.

How the renderer paints is shown in [Chapter 4](04-graphics.md); the math behind it in
[Appendix B](B-rendering.md).

## 5.1 The ground rules

So that all parts of Kern fit together, the same agreements apply everywhere: **Z points up**, and
everything is measured in **meters** – in graphics, physics, audio and in all the shape builders.

What does "Z" mean? Every point in space has three details, like an address made of three numbers:
how far right or left (**X**), how far forward or back (**Y**) and how far up or down (**Z**). So in
Kern, Z is always the height.

> **Imagine …** a building site where every tradesperson uses a different tape measure: one in
> inches, another in centimeters, and for the third, "up" is where the others say "north".
> Nothing would fit. Kern has one tape measure for everyone.

All shape builders deliver their result in the **same container**: a list of corners, and for every
corner its position, normal, color and UV. That way the renderer can treat every result the same,
whether it's a house or a street.

There are two ways to write down the triangles:

- **Triangle soup:** every three corners in a row are a triangle. Simple, but a corner where six
  triangles meet is in the list six times.
- **With an index:** every corner is there once, and a list of numbers says which three corners
  form a triangle. That saves space (more on that in 5.7).

If you want to paint your own special effect, you can give every corner a few extra numbers. That's
how the houses later get their glowing windows (5.8).

> **Try it!** Open Kern Studio at https://kernengine.de/, use the **Content Drawer**, the parts box
> (Ctrl+Space), to get a **Sphere** from the **Shapes** group – click it, and it appears where the
> camera is looking – and press **Alt+2** to switch to **Wireframe**. The smooth sphere falls apart
> into a net of many small, flat faces. That's its triangle mesh. **Alt+4** takes you back to the
> normal picture.

## 5.2 Frustum culling: whatever isn't in the picture isn't painted

The camera sees only a section of the world: the **view pyramid** from
[Chapter 3](03-foundations.md), a pyramid lying on its side with its tip cut off (technical term:
**frustum**). The figure there shows it from above. Whatever lies completely outside doesn't need
to be painted. This sorting out is called **culling**. It saves a lot of work, because in a city
most of it usually lies behind you or next to you.

> **Imagine …** you're standing in a theater with a flashlight. Before you shine it on someone,
> you ask: is that person on the stage at all? Whoever is waiting behind the curtain costs you
> nothing.

The trick: the test has to be much cheaper than painting, otherwise it isn't worth it. So Kern
doesn't check every triangle but puts an **invisible sphere** around every mesh and only asks:
does this sphere lie completely outside one of the pyramid's **six walls** (left, right, top,
bottom, front, back)? Six small calculations instead of thousands of corners.

The test is **cautious**: a sphere near a corner of the pyramid is sometimes painted
unnecessarily, but **never** wrongly left out. Better to work once too often than to have a house
missing.

Kern puts a sphere not only around a single mesh but also around all its instances together.

> **Watch out!** All copies of a mesh share **one** sphere. A forest whose trees are scattered
> across the whole map has a huge sphere and is therefore almost always "visible". Better to split
> such copies into spatial groups; then every group outside the picture drops away.

Never sorted out are characters with bones, that is, characters whose arms and legs are moved by an
invisible skeleton (such as a walking figure). Their arms and legs can leave the sphere.

> **For the curious:** Kern calculates the six walls so that each test gives the distance in real
> meters. Then it can be compared directly with the sphere's radius.

> **Try it!** In Kern Studio, under **Window › Show**, switch on **Bounds**. A yellowish box of
> lines appears around every thing: its bounding box, that is, the space it takes up at most.
> Studio doesn't show the sphere for the test. Picture it like this: its center sits in the middle
> of this box, and it's just big enough to enclose the whole thing. Move a thing, and the box and
> the imagined sphere move along with it.

## 5.3 The BVH: boxes in boxes

When you click into a city, Kern has to find the triangle you hit among tens of thousands. Check
them all one by one? Far too slow. The solution is called a **BVH** (*bounding volume hierarchy*):
triangles are packed into boxes, the boxes into bigger boxes.

> **Imagine …** you're looking for a screw in the basement. The screws are in little boxes, the
> little boxes in crates, the crates on shelves. If a shelf says "paint only", you don't open a
> single little box on it.

**Packing:** Kern looks in which direction the triangles are spread out the most and splits them
there into two halves of equal size. Each half gets a box around it. Then each half is split
again, and so on, until each box holds only a handful of triangles.

**Searching:** the search uses a **ray**, an imaginary line from the mouse into the scene (5.4). If
the ray misses a box, everything in it is done with, without a single triangle being checked. If
it hits it, Kern looks into the two sub-boxes, **the nearer one first**. If it finds a hit there
early, all boxes farther away drop out right away at the next comparison: everything in them is
farther away than the hit Kern already has, so it can't be the nearest triangle.

A BVH is only worth it for big things that stand still, such as the city's houses. If such a thing
is moved, Kern throws the boxes away. Small or moving things Kern simply checks triangle by
triangle.

> **For the curious:** Because every level halves, the tower of boxes grows only very slowly: for
> twice as many triangles you need just one more level. The exact building rules are in the blue
> book, Chapter 5.

## 5.4 Picking: what's under the mouse pointer?

**Picking** means: finding out which thing you clicked. For this, Kern sends a ray from the mouse
pointer straight into the scene and looks for the **nearest** triangle it hits.

> **Imagine …** you hold a laser pointer exactly at the spot on the screen where you click and
> shine it "through the glass" into the 3D world. The first object the red dot lands on is your
> hit.

With a normal camera with perspective, the ray starts at the viewer's eye and goes through the
mouse point. In views without perspective (such as a top view like a blueprint), all rays run
parallel.

Then Kern goes through every mesh that's a candidate:

1. **Coarse:** does the ray miss the mesh's invisible sphere, or is the sphere farther away than
   the best hit so far? Then this mesh is done with.
2. **Fine:** otherwise it goes through the BVH's boxes or triangle by triangle. For characters with
   bones, Kern works out their current pose for this. **What you see, you can click.**

As a result, Kern knows: which mesh, which copy, which triangle, how far away and at which spot.

Kern Studio selects your objects exactly like this. The ray simply passes through the ground and
locked objects (ones you've protected in Studio against being clicked by accident,
[Chapter 11](11-studio.md)). Lights and empty objects have no triangles; you select them via their
little icon.

> **Try it!** Click different objects in the viewport (the big 3D window). On the right in the
> Outliner, the list of all objects, the highlight jumps along. Every click is a ray in search of
> the nearest triangle.

## 5.5 Earcut: cutting polygons into triangles

A house's floor plan is a polygon, often L-shaped and sometimes with holes (courtyards). But the
graphics processor can only do triangles. So someone has to cut the polygon into triangles. In
Kern, that's done by **Earcut**, "ear cutting".

> **Imagine …** you have a paper polygon and a pair of scissors. You look for a corner whose
> triangle with its two neighboring corners lies completely within the paper – an **ear**. Snip!
> What's left is a polygon with one corner fewer. Keep going until only one triangle is left.

**Worked through:** a pentagon has five corners. First ear off: four corners remain. Second ear
off: three corners, that is, the last triangle. That makes three triangles. In general, n corners
always give **n − 2 triangles**. You can always find an ear: every polygon that doesn't cross
itself has at least two of them.

Earcut has to handle three special cases:

- **Dirty data:** real map data has duplicate points or edges that cross. If Earcut can't find an
  ear any more, it cleans up, repairs small errors and, if necessary, splits the polygon in two.
- **Holes:** ears only work with a single edge. That's why Earcut cuts a wafer-thin **slit** from
  every courtyard to the outer edge. After that, everything is a single, twisty edge (see the
  picture below).
- **Big polygons:** with very many corners, it would be too slow to check all corners for every
  ear. Earcut then sorts the corners by area and only looks around in the neighborhood.

Why does a slit turn two edges into one? Take a picture frame made of paper: it has an outer edge
and an inner edge. If you cut through it at one spot from inside to outside, you can unfold it
into a long, curved strip. If you now run your finger along the edge, you pass both the outer and
inner edge without lifting it. There's only **one** edge left.

![A slit connects the courtyard to the outer edge: two edges become one](../abbildungen/earcut-loch.svg)

> **Did you know?** After cutting, Kern compares the area of all the triangles with that of the
> floor plan. If it's off by more than 0.7%, the building is left out instead of standing in the
> city with a broken roof.

> **For the curious:** Kern's Earcut is a port of the well-known Earcut by the company Mapbox from
> JavaScript, the browser's programming language, and gives the same results. Origin and license
> are in the blue book, Chapter 5, and in Appendix D.

## 5.6 Primitives: the building set

**Primitives** are ready-made basic solids: box, sphere, capsule, torus (donut), cylinder, cone and
plane, all with Z up. You can **glue** several of them, moved and rotated, **into a single mesh**.

> **Imagine …** you send three letters to the same address. Three envelopes, three trips to the
> mailbox. If you put them all in one envelope, it's one trip.

It's just the same with painting: for every mesh, the graphics card gets its own job, and every job
costs admin time. A glued-together thing is **one** job instead of three. That's how Orb Run builds
its car from three boxes, and all the city's ground tiles become a single mesh.

## 5.7 Welding: merging duplicate corners

The house and street builders deliver triangle soup (5.1). Where six triangles meet, the same corner
is stored six times. **Welding** turns this into a mesh with an index: every corner is there once,
and the triangles point to it by number. That saves memory and time when uploading to the graphics
card, and the graphics card can reuse a corner that it has already converted to its place on the
screen for all the triangles attached to it.

> **Imagine …** a guest list on which every guest appears as many times as they have neighbors at
> the table. Welding crosses out the duplicates and writes "see guest no. 7" instead.

The most important rule: Kern only merges corners that are exactly the same in **everything**, not
just in position but also in normal, color and UV. That's why nothing about the look changes.

**The cube as an example:** at a cube's edge, two corners sit in the same place. One belongs to the
top face; its normal points up. The other belongs to the side wall; its normal points sideways.
Same place, but a different viewing direction – so **not** the same corner. They stay separate,
and the edge stays sharp. If Kern merged them, the corner would have to choose one direction, and
the cube would look soft and washed out at the edges.

Kern also only welds when it really saves something. If it brings too little, the soup stays as it
is.

> **Did you know?** For a real section of Konstanz with 1300 buildings, building and welding in
> Kern take about 45 thousandths of a second – the earlier version needed about 1260.

Finally, every finished piece gets a **fingerprint**: a short check number that changes completely
with the slightest change. That way a program notices whether a newly built piece is any different
from the old one at all.

## 5.8 Houses from floor plans

A house is made from its **floor plan**, that is, the shape it has seen from above. Kern pulls this
shape up into the air.

> **Imagine …** a cookie cutter that you press into a thick slab of dough: top and bottom have the
> shape of the floor plan, with vertical walls in between. This pulling up is called
> **extruding**.

Per house, Kern gets only a little: the outline, the outlines of the courtyards, the height of the
base and the roof edge, and a seed. From that it does three things:

1. **Tidy up:** points that almost lie on top of each other and tiny outlines are thrown out. All
   outer edges are brought into the same direction of travel, all courtyards into the opposite
   direction. That way Kern knows at every edge where "outside" is, and all walls face outward –
   like hikers who all walk around a lake in the same direction and therefore all know which side
   the water is on.
2. **Build:** the roof is made from the Earcut triangles (5.5); courtyards stay open. Along every
   edge a wall is made. If you want, a cornice is added, the band that sticks out under the roof
   edge. Wall color, roof color and story height are rolled by the seed.
3. **Mark the windows:** every corner gets extra numbers, such as how long and high its wall is.
   From these, a special effect from [Chapter 4](04-graphics.md) paints a grid of glowing windows at
   night – without a single window having to be built as a triangle.

Whatever can't be built cleanly (for example because the area doesn't match) is left out.

**Building in bite-sized pieces.** Building a big city with thousands of houses all at once would
freeze the picture. That's why the builder works with a **time budget**: build for a few
milliseconds, then a frame may be painted again, then keep building.

**Neighborhood packages.** The houses end up in square **chunks**, like neighborhoods on a city map.
Each chunk becomes its own mesh. It's a trade-off: big chunks mean few jobs for the graphics card
(5.6), small chunks are easier to sort out (5.2).

**For web pages too.** The house builder – as well as the builders for streets and terrain from 5.9
and 5.10 – can be used directly by a web page that uses Kern only as a painter. That's how the game
SpawnCity builds its city. There too, the builder works in bite-sized pieces, so that the page
doesn't freeze while building.

> **For the curious:** Depending on the seed, the story height is between 3.1 and 3.6 m. All other
> measurements and the names of the builders are in the blue book, Chapter 5.

## 5.9 Streets

Streets are **flat strips** that lie on the terrain: at the bottom a wide strip that peeks out
from under the roadway on the left and right and so looks like a curb, on top of it the roadway,
and on top of that the dashed center line. So that the strips follow hills, Kern cuts every street
into short pieces, and every corner asks for the ground height at its spot.

> **Imagine …** you lay tape on a duvet. A long, stiff board would float above the bumps; lots of
> short pieces of tape hug the surface.

The three strips lie a little way above one another.

> **Watch out!** If two surfaces lie at exactly the same height, the graphics processor doesn't
> know which is in front, and they flicker against each other: **z-fighting**. The heights,
> staggered by a few centimeters, prevent that.

The ground height comes from a **height lookup**: you ask "How high is the ground at this point?"
and get a number. If there isn't one, the ground is flat. If it answers with nonsense instead of a
number, the street builder stops with an error message instead of quietly building a wrong street.
Important: the game should give **the same** height lookup to its physics too. Then cars drive
exactly on the street you see, instead of floating above it or sinking into it.

Streets that are farther away than a chosen radius are skipped. That way you can build a section of
a big map.

## 5.10 Terrain tiles

Kern builds hilly landscape out of **tiles**: square pieces that lie next to each other.

> **Imagine …** you lay a fishing net over a landscape. Every knot asks the height lookup: "How
> high is it here?" and slides to that height. Between the knots, Kern stretches triangles. And
> there's a piece of terrain.

The most important thing: neighboring tiles fit together **seamlessly**. So that the light doesn't
jump at the border, every corner needs the right normal, and that comes from how steeply it goes
up and down to the neighboring knots. But a knot at the edge is missing its neighbors on the other
side.

> **Imagine …** two neighbors who each know only their own half of the garden. At the fence, each
> estimates the slope of the hill differently, and in the evening light you see a bright stripe
> exactly there.

That's why every tile looks one row beyond its own edge, as if over the fence. That way the edge
knots also know their neighbors, and both tiles calculate the same thing at the seam.

For real places there's a converter: it turns GPS coordinates (longitude and latitude) into "so
many meters east and north of here". That way floor plans from map data come straight into Kern's
world. The exact names and values are in the blue book, Chapter 5.

## 5.11 The procedural city

Now everything comes together. **Procedural City** is Orb Run's city generator as a building block
for any level. "Procedural" means: nobody built this city by hand. It comes from **rules** and a
**seed**. Buildings, streets, ground tiles, trees, collision bodies (invisible blocks that cars and
players bump into) and traffic – all from a single number.

> **Imagine …** a dice game where you draw a city on paper by fixed rules: "On a 1, a big house
> with a courtyard; on a 2, four small houses …" If you give someone the rules and **exactly the
> same sequence of rolls**, they draw exactly your city. The seed is that sequence of rolls.

**Same seed, same city** – on every computer, every time. That's why nobody has to save the city;
it's enough to remember the number and the few settings.

![An exported game: the procedural city with buildings, streets and trees](../buch/bilder/game.png)

The rules in words:

1. **Grid:** the city is a chessboard of blocks with streets in between.
2. **Tall in the middle, low at the edge:** the closer a block is to the city center, the taller its
   houses may get. In the middle, a high-rise district appears.
3. **Development:** each block randomly gets one of four kinds of development: one big house (often
   with a courtyard), four single houses with alleys, an L-shaped house with a small one next to it,
   or three long rows.
4. **Streets** along all block edges, with curb and center line, built with the street builder from
   5.9.
5. **Trees** along the block edges, with random gaps, sizes and shades of green.

Building uses the tools of this chapter: houses from floor plans (5.8) in neighborhood packages,
everything welded (5.7). All tree trunks are a single mesh with many instances, all treetops a
second.

**Rebuilding smartly:** if you move the city, Kern doesn't recalculate anything; it just moves the
finished result. It only rebuilds when a setting really changes.

**Physics and traffic:** for the physics ([Chapter 6](06-physics.md)), the city becomes a single
immovable body made of many simple boxes, one per building part and ground tile. In the editor, the
cars stand still. As soon as the game runs, they drive along their lane, reappear on the other side
at the city's edge and stop when you're standing in front of them.

> **For the curious:** All of the city's settings with their defaults and limits are in the blue
> book, Chapter 5. The random generator is the same as in Orb Run, which is why the city looks the
> same there and in the level.

> **Try it!** Open the **Content Drawer** with Ctrl+Space and place a **Procedural City** from the
> **World** group into the level. On the right in **Details**, change the **Seed**: every number
> gives a new city. Type the old number back in, and the old city is back, house for house. With
> **Blocks per side**, the city gets bigger or smaller. With **Simulate**, the test run with
> physics (Alt+S), the traffic starts moving.

## 5.12 Shapes in the level and built-in meshes

Every visible thing in the level has a shape: one of the basic solids from 5.6, plus a ramp (a
wedge) or a ready-made mesh. Kern builds the shape white; the color comes from the **material**,
that is, the nature of the surface: color, shine, pattern. So that a texture looks just as big on
a large wall as on a small one, the pattern is laid out by meters, not by the size of the surface.
For every shape, Kern can also suggest a matching collision body – more in
[Chapter 6](06-physics.md).

The shape **Mesh** points to a triangle mesh that's built in permanently. Today those are the
Micro Duck's limbs: Pollen Robotics' parts in simplified form (more in
[Chapter 13](13-micro-duck.md)).

These parts are **packed to save space**: every detail is rounded just coarsely enough that you
can't quite see it. On first use, Kern unpacks a part once and then keeps it. That's like a piece
of furniture delivered flat in a box: once assembled, it stays standing.

> **Did you know?** An unknown mesh name deliberately gives a 10 cm cube. That way the mistake
> stands out in the level right away, instead of simply nothing being visible.

The names of the parts and exactly how they're packed are in the blue book, Chapter 5. How the duck
is made from them is told in [Chapter 13](13-micro-duck.md).

## What you take away

- Every 3D shape is a net of triangles; every corner knows where it is, what color it has and
  which way its surface faces (normal).
- Culling checks one invisible sphere per mesh against the view pyramid – cheap and never wrongly
  invisible.
- A BVH is boxes in boxes; it makes clicking fast.
- Picking sends a ray from the mouse into the scene and takes the nearest triangle.
- Earcut cuts polygons into triangles ear by ear; courtyards are first connected to the edge by a
  narrow slit.
- Welding only merges corners that are the same in everything – that's why hard edges stay hard.
- Houses are made by pulling the floor plan up, in bite-sized pieces and in neighborhood
  packages; streets are strips that use the same height lookup as the physics.
- The procedural city comes from rules and a seed: same number, same city.

## Brain teasers

1. Why does Kern test a sphere for culling instead of all the triangles?
2. A forest of 5000 trees as one mesh with many copies is never sorted out. What helps?
3. A polygon has 12 corners and no holes. How many triangles does Earcut make of it?
4. At a cube's edge, two corners sit in the same place. Why doesn't welding merge them?
5. What would happen if the roadway and the center line were at exactly the same height?
6. Your friend has found a beautiful high-rise in her city. What does she have to send you so that
   you get the same city?

### Solutions

1. The test has to be much cheaper than painting; six small calculations cost almost nothing.
2. Split the trees into spatial groups with their own sphere; groups outside the picture drop away.
3. n − 2 = 10 triangles.
4. One belongs to the top face and looks up, the other to the side wall and looks sideways. Their
   normals are different, so they're not the same in everything – the edge stays sharp.
5. Z-fighting: the surfaces flicker because it isn't clear which one is in front.
6. The seed and the city's other settings. Same number, same rules, same city.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
