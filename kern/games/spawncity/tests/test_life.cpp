// Street life (M3 life): ragdolls, walkable buildings, cows, birds, pigeons. They are the level's Blueprints
// ragdoll_cpp, interiors_cpp and life_cpp (their structs: ragdoll, interiors, life), called as their classes -
// as the program calls them (level_blueprints.h) - on the engine itself: the physics world is the test's,
// given to them by its handle.
#include <cmath>
#include <memory>

#include "../src/blueprint_call.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
using Rg = blueprints::ragdoll_cpp;
using In = blueprints::interiors_cpp;
using Lf = blueprints::life_cpp;
/// The class T of a program file's Blueprint, on the engine itself (RuleBlueprint::Engine::direct).
template <class T> struct ProgramClass {
  RuleBlueprint rules;
  bool bound = false;
  explicit ProgramClass(const char* actor) : rules(actor, nullptr, true, {}, RuleBlueprint::Engine::direct) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
#define CLASS_BOUND(c, name) \
  SC_CHECK((c).bound, name " runs as its class: %s", (c).rules.problems.c_str()); \
  if (!(c).bound) return

/// The ragdolls of a test world (Ragdolls::init(w, false): desktop).
struct Dolls {
  ProgramClass<Rg> bp{"ragdoll_cpp"};
  kern::physics::World w;
  i64 world = 0;
  std::unique_ptr<Rg::Ragdolls> d = std::make_unique<Rg::Ragdolls>();
  Dolls() {
    make_flat_world(w);
    if (!bp.bound) return;
    world = bp.rules.host.objects.adopt(w);
    (*bp).Ragdolls_init(*d, world, false);
  }
  ~Dolls() { if (bp.bound) (*bp).Ragdolls_end(*d); }
  /// spawn(key, feet, yaw, velocity, colors, recover, now) - the colours Ragdolls::Colors{}.
  bool spawn(u64 key, Vec3 feet, f64 yaw, Vec3 velocity, bool recover, f64 now) {
    return (*bp).Ragdolls_spawn(*d, Rg::Ragdolls_Colors{}, Rg::Int64{key}, feet, yaw, velocity, recover, now).Result;
  }
  void step(f32& now, f32 seconds) {
    for (int i = 0; i < static_cast<int>(seconds * 240); ++i) {
      w.step(1.0f / 240, 1);
      now += 1.0f / 240;
      if (i % 4 == 0) (*bp).Ragdolls_update(*d, 4.0f / 240, now);
    }
  }
};
In::MapFeatures_Building box_building(f32 x0, f32 y0, f32 x1, f32 y1, f32 top) {
  In::MapFeatures_Building b;
  In::Vector_row ring;
  ring.items = In::List<Vec3>(std::vector<Vec3>{{x0, y0, 0}, {x1, y0, 0}, {x1, y1, 0}, {x0, y1, 0}});
  b.rings.push_back(ring);
  b.base = 0;
  b.top = top;
  b.seed = 7;
  return b;
}
/// A town of a test: life_cpp on the physics world, an empty city (no tiles: flat ground, no meadows, no streets).
struct Town {
  ProgramClass<Lf> bp{"life_cpp"};
  kern::physics::World w;
  i64 world = 0;
  std::unique_ptr<Lf::CityStream> city = std::make_unique<Lf::CityStream>();
  std::unique_ptr<Lf::Life> life = std::make_unique<Lf::Life>();
  const Lf::List<Lf::Vec4> no_ground_changes;  // (nowhere did the ground change)
  Lf::LifeView v;
  Lf::LifeOut out;
  Town() {
    make_flat_world(w);
    if (!bp.bound) return;
    world = bp.rules.host.objects.adopt(w);
    (*bp).Life_init(*life, world, false);
    v.physics = world;
  }
  Lf& operator*() { return *bp; }
  void update(f32 dt, f32 now, Lf::Rng& rng) { (*bp).Life_update(*life, v, *city, no_ground_changes, rng, out, dt, now); }
  /// shot(from, to, damage, now, out, melee = false)
  bool shot(Vec3 from, Vec3& to, f32 damage, f32 now) { return (*bp).Life_shot(*life, to, out, from, damage, now, false).Result; }
};
}  // namespace

SC_TEST(life_ragdoll_falls_and_rests) {
  Dolls dolls;
  CLASS_BOUND(dolls.bp, "ragdoll_cpp");
  Rg& rg = *dolls.bp;
  f32 now = 0;
  SC_CHECK(dolls.spawn(1, {0, 0, 0}, 0, {4, 0, 2}, false, now), "a body is thrown");
  dolls.step(now, 4);
  Vec3 p;
  Rg::Float yaw = 0;
  SC_CHECK(rg.Ragdolls_where(*dolls.d, p, yaw, Rg::Int64{1}).Result, "it is there");
  SC_CHECK(std::isfinite(p.x) && p.z > 0.03f && p.z < 0.45f, "it lies on the ground (pelvis z %.2f)", p.z);
  SC_CHECK(p.x > 0.5f && p.x < 8, "thrown along the push (x %.2f)", p.x);
  Rg::List<Rg::Ragdolls_Draw> d;
  rg.Ragdolls_draw(*dolls.d, d);
  SC_CHECK(d.size() == static_cast<std::size_t>(Rg::Ragdolls_Bone::kBones), "eleven bones drawn (%zu)", static_cast<std::size_t>(d.size()));
  // The pool reuses the oldest body when full (10 on desktop).
  for (u64 k = 2; k < 14; ++k) dolls.spawn(k, {static_cast<f32>(k) * 3, 10, 0}, 0, {}, false, now);
  const u32 active = static_cast<u32>(rg.Ragdolls_active(*dolls.d).Result);
  SC_CHECK(active == 10, "pool of 10 (%u)", active);
}

SC_TEST(life_ragdoll_gets_up) {
  Dolls dolls;
  CLASS_BOUND(dolls.bp, "ragdoll_cpp");
  Rg& rg = *dolls.bp;
  f32 now = 0;
  dolls.spawn(5, {2, 3, 0}, 1.0f, {3, 1, 1.5f}, true, now);
  Rg::List<Rg::Ragdolls_Up> ups;
  for (int i = 0; i < 9 * 60 && ups.empty(); ++i) {
    dolls.step(now, 1.0f / 60);
    rg.Ragdolls_take_ups(*dolls.d, ups);
  }
  SC_CHECK(ups.size() == 1 && ups[0].key == Rg::Int64{5}, "the person stands up again (%zu)", static_cast<std::size_t>(ups.size()));
  if (!ups.empty()) SC_CHECK(std::fabs(ups[0].feet.z) < 0.3f && std::hypot(ups[0].feet.x - 2, ups[0].feet.y - 3) < 6, "on his feet where he fell (%.2f %.2f %.2f)", ups[0].feet.x, ups[0].feet.y, ups[0].feet.z);
  SC_CHECK(!rg.Ragdolls_has(*dolls.d, Rg::Int64{5}).Result, "the body is gone");
}

SC_TEST(life_interior_plan_and_stairs) {
  ProgramClass<In> bp("interiors_cpp");
  CLASS_BOUND(bp, "interiors_cpp");
  In& in = *bp;
  const In::MapFeatures_Building b = box_building(0, 0, 24, 14, 14);
  auto plan = std::make_unique<In::Interiors_Plan>();
  In::Interiors_Plan& P = *plan;
  // (build(b, door_ground, P): the door at ground 0)
  SC_CHECK(in.Interiors_build(b, P, 0).Result, "the building is enterable");
  SC_CHECK(P.levels.size() >= 3 && P.has_core && P.roof_access, "storeys, stairs and a roof hatch (%zu levels, core %d, roof %d)", static_cast<std::size_t>(P.levels.size()), static_cast<int>(static_cast<bool>(P.has_core)), static_cast<int>(static_cast<bool>(P.roof_access)));
  SC_CHECK(P.doors.size() == 2 && std::fabs(P.doors[0].c.y) < 0.01f, "front and back door on the long sides (%.1f %.1f)", P.doors[0].c.x, P.doors[0].c.y);
  SC_CHECK(P.furniture > 0 && !P.lamps.empty(), "furniture %u, lamps %zu", static_cast<u32>(P.furniture), static_cast<std::size_t>(P.lamps.size()));
  kern::physics::World w;
  make_flat_world(w);
  const i64 world = bp.rules.host.objects.adopt(w);
  const std::vector<u32> indices(P.indices.begin(), P.indices.end());
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::fixed;
  const kern::physics::Body body = w.create_body(bs);
  kern::physics::ShapeSettings ss;
  ss.kind = kern::physics::ShapeKind::mesh;
  ss.points = kern::Span<const Vec3>(P.points.data(), P.points.size());
  ss.indices = kern::Span<const u32>(indices.data(), indices.size());
  SC_CHECK(w.create_shape(body, ss).valid(), "Box3D takes the collision mesh (%zu triangles)", indices.size() / 3);
  // The facade stops a ray except at the door.
  const In::Interiors_Door& d = P.doors[0];
  // (cast(w, origin, dir, max_len, self, mask = ~0): no body of its own, every category)
  const auto cast = [&](Vec3 origin, Vec3 dir, f64 len) { return in.cast(world, origin, dir, len, {}, In::Int64{~0ull}).Result; };
  const auto wall = cast({d.c.x + 4, d.c.y - 3, 1.2f}, {0, 1, 0}, 6);
  SC_CHECK(wall.hit && std::fabs(wall.point.y) < 0.05f, "the wall beside the door (hit %d y %.2f)", static_cast<int>(static_cast<bool>(wall.hit)), wall.point.y);
  const auto door = cast({d.c.x, d.c.y - 3, 1.2f}, {0, 1, 0}, 6);
  SC_CHECK(!door.hit || door.point.y > 0.5f, "through the door (hit %d y %.2f)", static_cast<int>(static_cast<bool>(door.hit)), door.point.y);
  // Upper floor slab, roof.
  const auto up = cast({3, 7, static_cast<f32>(P.levels[1] + 1)}, {0, 0, -1}, 3);
  SC_CHECK(up.hit && std::fabs(up.point.z - P.levels[1]) < 0.05f, "first floor at %.2f (hit %.2f)", P.levels[1], up.point.z);
  const auto roof = cast({3, 7, 30}, {0, 0, -1}, 30);
  SC_CHECK(roof.hit && std::fabs(roof.point.z - 14) < 0.05f, "the roof (%.2f)", roof.point.z);
  // Walking up the first flight: from the lobby's near landing along the core onto the half landing.
  const In::Interiors_Core& c = P.core;
  kern::physics::CharacterMover mover;
  const Vec3 start = c.o + c.u * 0.6f + c.v * 0.4f;
  mover.reset({start.x, start.y, static_cast<f32>(P.levels[0] + 0.05f)});
  for (int i = 0; i < 240 * 3; ++i) {
    mover.step(w, 1.0f / 240, {c.v.x * 0.5f, c.v.y * 0.5f, 0}, false, false, {});
    w.step(1.0f / 240, 1);
  }
  const f32 half = static_cast<f32>((P.levels[1] - P.levels[0]) / 2);
  SC_CHECK(std::fabs(mover.position().z - (P.levels[0] + half)) < 0.35f, "up the stairs onto the half landing (z %.2f, want %.2f)", mover.position().z, static_cast<f64>(P.levels[0] + half));
  // Small and odd footprints are not entered.
  auto Q = std::make_unique<In::Interiors_Plan>();
  SC_CHECK(!in.Interiors_build(box_building(0, 0, 3, 3, 6), *Q, 0).Result, "a 9 m2 shed is not enterable");
}

SC_TEST(life_cows_birds_shots) {
  Town town;  // no tiles: flat ground, no meadows, no streets
  CLASS_BOUND(town.bp, "life_cpp");
  Lf& lf = *town;
  Lf::Life& life = *town.life;
  Lf::LifeView& v = town.v;
  Lf::LifeOut& out = town.out;
  v.focus = v.player = v.eye = {0, -20, 0};
  Lf::Rng rng{5};
  f32 now = 10;
  SC_CHECK(lf.Life_spawn_herd(life, {0, 0, 0}, 14, 5, now).Result, "a herd");
  town.update(1.0f / 60, now, rng);
  SC_CHECK(life.cows_.size() == 5, "five cows (%zu)", static_cast<std::size_t>(life.cows_.size()));
  // a round into the first cow
  const Vec3 cow = life.cows_[0].pos;
  Vec3 to = cow + Vec3{0, 30, 1.1f};
  SC_CHECK(town.shot(cow + Vec3{0, -30, 1.1f}, to, 30, now), "the bullet hits the cow");
  SC_CHECK(life.cows_[0].hp == 70 && !life.cows_[0].dead, "it is hurt (%.0f)", static_cast<f64>(life.cows_[0].hp));
  // a blast kills it and throws the 600 kg body
  lf.Life_blast(life, out, cow + Vec3{0.5f, 0, 0.3f}, 7, now);
  SC_CHECK(life.cows_[0].dead && lf.Body_valid(life.cows_[0].body).Result, "killed, a Box3D body");
  const f32 m = static_cast<f32>(lf.engine_world_mass(town.world, life.cows_[0].body));
  SC_CHECK(std::fabs(m - 600) < 5, "600 kg (%.0f)", m);
  for (int i = 0; i < 240; ++i) town.w.step(1.0f / 240, 1);
  town.update(1.0f / 60, now + 1, rng);
  SC_CHECK(kern::length(life.cows_[0].pos - (cow + Vec3{0, 0, 1.12f})) > 0.3f, "the body was thrown");
  // birds circle over the roofs and fall when hit
  for (int i = 0; i < 30; ++i) town.update(1.0f / 30, now + 1 + i / 30.0f, rng);
  const Lf::Life_Bird& b = life.birds_[0];
  SC_CHECK(b.pos.z > 7 && b.pos.z < 25, "bird at %.1f m", b.pos.z);
  Vec3 bt = b.pos + Vec3{0, 0, 20};
  SC_CHECK(town.shot(b.pos - Vec3{0, 0, 20}, bt, 30, now + 2) && lf.Life_bird_hits(life).Result == 1, "a bird shot down");
  // street voices are rate-limited
  lf.LifeOut_clear(out);
  SC_CHECK(lf.Life_shout(life, out, {0, 0, 0}, "bump", 1, now + 5, 5).Result && !out.speech.empty(), "a shout is spoken");
  SC_CHECK(!lf.Life_shout(life, out, {0, 0, 0}, "bump", 2, now + 5.5f, 5).Result, "and not again within 1.4 s");
}

SC_TEST(life_dogs_cats_black_cat) {
  Town town;
  CLASS_BOUND(town.bp, "life_cpp");
  Lf& lf = *town;
  Lf::Life& life = *town.life;
  Lf::LifeView& v = town.v;
  Lf::LifeOut& out = town.out;
  v.focus = v.player = v.eye = {0, 0, 0};
  v.head = {0, 0, 1.68f};
  v.epoch = 1000;  // floor(1000 / 120) = 8: the black cat is out (every other two minutes)
  Lf::Rng rng{9};
  f32 now = 1;
  for (int i = 0; i < 10; ++i) {
    now += 1.0f / 30;
    v.epoch += 1.0 / 30;
    town.update(1.0f / 30, now, rng);
  }
  SC_CHECK(life.pets_.size() == 20, "20 dogs and cats (%zu)", static_cast<std::size_t>(life.pets_.size()));
  SC_CHECK(lf.Life_species(life, 2, v.epoch).Result == Lf::Life_Species::cat && lf.Life_species(life, 0, v.epoch).Result == Lf::Life_Species::dog, "every third a cat");
  SC_CHECK(lf.Life_species(life, 1, v.epoch).Result == Lf::Life_Species::black_cat && lf.Life_species(life, 1, 1150).Result == Lf::Life_Species::dog, "the black cat now and then (else a dog)");
  // a dog shot dead rolls over as a body
  const Vec3 dog = life.pets_[0].pos;
  Vec3 to = dog + Vec3{0, 20, 0.3f};
  SC_CHECK(town.shot(dog + Vec3{0, -20, 0.3f}, to, 30, now) && life.pets_[0].dead_at >= 0 && lf.Body_valid(life.pets_[0].body).Result, "a dog shot dead");
  // the black cat charges when shot and gets the player
  const Vec3 cat = life.pets_[1].pos;
  to = cat + Vec3{0, 20, 0.2f};
  SC_CHECK(town.shot(cat + Vec3{0, -20, 0.2f}, to, 30, now) && life.cat_.on, "the black cat charges");
  bool killed = false;
  for (int i = 0; i < 30 * 15 && !killed; ++i) {
    lf.LifeOut_clear(out);
    now += 1.0f / 30;
    v.epoch += 1.0 / 30;
    town.update(1.0f / 30, now, rng);
    killed = out.kill_player;
  }
  SC_CHECK(killed, "and that is the end of you");
}
