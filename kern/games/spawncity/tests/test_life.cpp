// Street life (M3 life): ragdolls, walkable buildings, cows, birds, pigeons.
#include <cmath>

#include "../src/interiors.h"
#include "../src/life.h"
#include "../src/ragdoll.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
void step(kern::physics::World& w, Ragdolls* r, f32& now, f32 seconds) {
  for (int i = 0; i < static_cast<int>(seconds * 240); ++i) {
    w.step(1.0f / 240, 4);
    now += 1.0f / 240;
    if (r && i % 4 == 0) r->update(4.0f / 240, now);
  }
}
MapFeatures::Building box_building(f32 x0, f32 y0, f32 x1, f32 y1, f32 top) {
  MapFeatures::Building b;
  b.rings.push_back({{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}});
  b.base = 0;
  b.top = top;
  b.seed = 7;
  return b;
}
}  // namespace

SC_TEST(life_ragdoll_falls_and_rests) {
  kern::physics::World w;
  make_flat_world(w);
  Ragdolls dolls;
  dolls.init(w, false);
  f32 now = 0;
  SC_CHECK(dolls.spawn(1, {0, 0, 0}, 0, {4, 0, 2}, {}, false, now), "a body is thrown");
  step(w, &dolls, now, 4);
  Vec3 p;
  f32 yaw;
  SC_CHECK(dolls.where(1, p, yaw), "it is there");
  SC_CHECK(std::isfinite(p.x) && p.z > 0.03f && p.z < 0.45f, "it lies on the ground (pelvis z %.2f)", p.z);
  SC_CHECK(p.x > 0.5f && p.x < 8, "thrown along the push (x %.2f)", p.x);
  std::vector<Ragdolls::Draw> d;
  dolls.draw(d);
  SC_CHECK(d.size() == Ragdolls::kBones, "eleven bones drawn (%zu)", d.size());
  // The pool reuses the oldest body when full (10 on desktop).
  for (u64 k = 2; k < 14; ++k) dolls.spawn(k, {static_cast<f32>(k) * 3, 10, 0}, 0, {}, {}, false, now);
  SC_CHECK(dolls.active() == 10, "pool of 10 (%u)", dolls.active());
}

SC_TEST(life_ragdoll_gets_up) {
  kern::physics::World w;
  make_flat_world(w);
  Ragdolls dolls;
  dolls.init(w, false);
  f32 now = 0;
  dolls.spawn(5, {2, 3, 0}, 1.0f, {3, 1, 1.5f}, {}, true, now);
  std::vector<Ragdolls::Up> ups;
  for (int i = 0; i < 9 * 60 && ups.empty(); ++i) {
    step(w, &dolls, now, 1.0f / 60);
    dolls.take_ups(ups);
  }
  SC_CHECK(ups.size() == 1 && ups[0].key == 5, "the person stands up again (%zu)", ups.size());
  if (!ups.empty()) SC_CHECK(std::fabs(ups[0].feet.z) < 0.3f && std::hypot(ups[0].feet.x - 2, ups[0].feet.y - 3) < 6, "on his feet where he fell (%.2f %.2f %.2f)", ups[0].feet.x, ups[0].feet.y, ups[0].feet.z);
  SC_CHECK(!dolls.has(5), "the body is gone");
}

SC_TEST(life_interior_plan_and_stairs) {
  const MapFeatures::Building b = box_building(0, 0, 24, 14, 14);
  Interiors::Plan P;
  SC_CHECK(Interiors::build(b, 0, P), "the building is enterable");
  SC_CHECK(P.levels.size() >= 3 && P.has_core && P.roof_access, "storeys, stairs and a roof hatch (%zu levels, core %d, roof %d)", P.levels.size(), P.has_core, P.roof_access);
  SC_CHECK(P.doors.size() == 2 && std::fabs(P.doors[0].c.y) < 0.01f, "front and back door on the long sides (%.1f %.1f)", P.doors[0].c.x, P.doors[0].c.y);
  SC_CHECK(P.furniture > 0 && !P.lamps.empty(), "furniture %u, lamps %zu", P.furniture, P.lamps.size());
  kern::physics::World w;
  make_flat_world(w);
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::fixed;
  const kern::physics::Body body = w.create_body(bs);
  kern::physics::ShapeSettings ss;
  ss.kind = kern::physics::ShapeKind::mesh;
  ss.points = kern::Span<const Vec3>(P.points.data(), P.points.size());
  ss.indices = kern::Span<const u32>(P.indices.data(), P.indices.size());
  SC_CHECK(w.create_shape(body, ss).valid(), "Box3D takes the collision mesh (%zu triangles)", P.indices.size() / 3);
  // The facade stops a ray except at the door.
  const Interiors::Door& d = P.doors[0];
  const RayResult wall = cast(w, {d.c.x + 4, d.c.y - 3, 1.2f}, {0, 1, 0}, 6, {});
  SC_CHECK(wall.hit && std::fabs(wall.point.y) < 0.05f, "the wall beside the door (hit %d y %.2f)", wall.hit, wall.point.y);
  const RayResult door = cast(w, {d.c.x, d.c.y - 3, 1.2f}, {0, 1, 0}, 6, {});
  SC_CHECK(!door.hit || door.point.y > 0.5f, "through the door (hit %d y %.2f)", door.hit, door.point.y);
  // Upper floor slab, roof.
  const RayResult up = cast(w, {3, 7, P.levels[1] + 1}, {0, 0, -1}, 3, {});
  SC_CHECK(up.hit && std::fabs(up.point.z - P.levels[1]) < 0.05f, "first floor at %.2f (hit %.2f)", P.levels[1], up.point.z);
  const RayResult roof = cast(w, {3, 7, 30}, {0, 0, -1}, 30, {});
  SC_CHECK(roof.hit && std::fabs(roof.point.z - 14) < 0.05f, "the roof (%.2f)", roof.point.z);
  // Walking up the first flight: from the lobby's near landing along the core onto the half landing.
  const Interiors::Core& c = P.core;
  kern::physics::CharacterMover mover;
  const Vec2 start = c.o + c.u * 0.6f + c.v * 0.4f;
  mover.reset({start.x, start.y, P.levels[0] + 0.05f});
  for (int i = 0; i < 240 * 3; ++i) {
    mover.step(w, 1.0f / 240, {c.v.x * 0.5f, c.v.y * 0.5f, 0}, false, false, {});
    w.step(1.0f / 240, 4);
  }
  const f32 half = (P.levels[1] - P.levels[0]) / 2;
  SC_CHECK(std::fabs(mover.position().z - (P.levels[0] + half)) < 0.35f, "up the stairs onto the half landing (z %.2f, want %.2f)", mover.position().z, P.levels[0] + half);
  // Small and odd footprints are not entered.
  Interiors::Plan Q;
  SC_CHECK(!Interiors::build(box_building(0, 0, 3, 3, 6), 0, Q), "a 9 m2 shed is not enterable");
}

SC_TEST(life_cows_birds_shots) {
  kern::physics::World w;
  make_flat_world(w);
  CityStream city;  // no tiles: flat ground, no meadows, no streets
  Life life;
  life.init(w, false);
  LifeView v;
  v.physics = &w;
  v.city = &city;
  v.focus = v.player = v.eye = {0, -20, 0};
  Rng rng(5);
  LifeOut out;
  f32 now = 10;
  SC_CHECK(life.spawn_herd({0, 0}, 14, 5, now), "a herd");
  life.update(1.0f / 60, now, v, rng, out);
  SC_CHECK(life.cows().size() == 5, "five cows (%zu)", life.cows().size());
  // a round into the first cow
  const Vec3 cow = life.cows()[0].pos;
  Vec3 to = cow + Vec3{0, 30, 1.1f};
  SC_CHECK(life.shot(cow + Vec3{0, -30, 1.1f}, to, 30, now, out), "the bullet hits the cow");
  SC_CHECK(life.cows()[0].hp == 70 && !life.cows()[0].dead, "it is hurt (%.0f)", life.cows()[0].hp);
  // a blast kills it and throws the 600 kg body
  life.blast(cow + Vec3{0.5f, 0, 0.3f}, 7, now, out);
  SC_CHECK(life.cows()[0].dead && life.cows()[0].body.valid(), "killed, a Box3D body");
  const f32 m = w.mass(life.cows()[0].body);
  SC_CHECK(std::fabs(m - 600) < 5, "600 kg (%.0f)", m);
  for (int i = 0; i < 240; ++i) w.step(1.0f / 240, 4);
  life.update(1.0f / 60, now + 1, v, rng, out);
  SC_CHECK(kern::length(life.cows()[0].pos - (cow + Vec3{0, 0, 1.12f})) > 0.3f, "the body was thrown");
  // birds circle over the roofs and fall when hit
  for (int i = 0; i < 30; ++i) life.update(1.0f / 30, now + 1 + i / 30.0f, v, rng, out);
  const Life::Bird& b = life.birds()[0];
  SC_CHECK(b.pos.z > 7 && b.pos.z < 25, "bird at %.1f m", b.pos.z);
  Vec3 bt = b.pos + Vec3{0, 0, 20};
  SC_CHECK(life.shot(b.pos - Vec3{0, 0, 20}, bt, 30, now + 2, out) && life.bird_hits() == 1, "a bird shot down");
  // street voices are rate-limited
  out.clear();
  SC_CHECK(life.shout({0, 0, 0}, "bump", 1, now + 5, 5, out) && !out.speech.empty(), "a shout is spoken");
  SC_CHECK(!life.shout({0, 0, 0}, "bump", 2, now + 5.5f, 5, out), "and not again within 1.4 s");
}

SC_TEST(life_dogs_cats_black_cat) {
  kern::physics::World w;
  make_flat_world(w);
  CityStream city;
  Life life;
  life.init(w, false);
  LifeView v;
  v.physics = &w;
  v.city = &city;
  v.focus = v.player = v.eye = {0, 0, 0};
  v.head = {0, 0, 1.68f};
  v.epoch = 1000;  // floor(1000 / 120) = 8: the black cat is out (every other two minutes)
  Rng rng(9);
  LifeOut out;
  f32 now = 1;
  for (int i = 0; i < 10; ++i) {
    now += 1.0f / 30;
    v.epoch += 1.0 / 30;
    life.update(1.0f / 30, now, v, rng, out);
  }
  SC_CHECK(life.pets().size() == 20, "20 dogs and cats (%zu)", life.pets().size());
  SC_CHECK(life.species(2, v.epoch) == Life::Species::cat && life.species(0, v.epoch) == Life::Species::dog, "every third a cat");
  SC_CHECK(life.species(1, v.epoch) == Life::Species::black_cat && life.species(1, 1150) == Life::Species::dog, "the black cat now and then (else a dog)");
  // a dog shot dead rolls over as a body
  const Vec3 dog = life.pets()[0].pos;
  Vec3 to = dog + Vec3{0, 20, 0.3f};
  SC_CHECK(life.shot(dog + Vec3{0, -20, 0.3f}, to, 30, now, out) && life.pets()[0].dead_at >= 0 && life.pets()[0].body.valid(), "a dog shot dead");
  // the black cat charges when shot and gets the player
  const Vec3 cat = life.pets()[1].pos;
  to = cat + Vec3{0, 20, 0.2f};
  SC_CHECK(life.shot(cat + Vec3{0, -20, 0.2f}, to, 30, now, out) && life.cat_attack().on, "the black cat charges");
  bool killed = false;
  for (int i = 0; i < 30 * 15 && !killed; ++i) {
    out.clear();
    now += 1.0f / 30;
    v.epoch += 1.0 / 30;
    life.update(1.0f / 30, now, v, rng, out);
    killed = out.kill_player;
  }
  SC_CHECK(killed, "and that is the end of you");
}
