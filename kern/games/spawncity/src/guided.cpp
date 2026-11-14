// The game's calls into the Blueprint of the actor "Guided" (see guided.h): no rule lives here.
#include "guided.h"

#include "blueprint_call.h"

namespace sc::guided {

namespace {
rules::Blueprint g_rules{"Guided"};

/// The Blueprint's struct Missile, as it takes and gives it.
struct MissileRun {
  Vec3 p, v, goal;
  f64 born = 0, cruise_z = 0;
  bool motor = false, pedestrian = false;
  f64 dodge_seconds = 0;
  bool dodge_lost = false;
  Vec3 dodge_last;
  bool tracking = true;
};
MissileRun flight_of(const Missile& m) {
  return {m.p, m.v, m.goal, rules::decimal(m.born), rules::decimal(m.cruise_z), m.motor, m.pedestrian, rules::decimal(m.dodge.seconds), m.dodge.lost, Vec3{m.dodge_last.x, m.dodge_last.y, 0},
          m.tracking};
}
void take(const MissileRun& f, Missile& m) {
  m.p = f.p;
  m.v = f.v;
  m.goal = f.goal;
  m.born = static_cast<f32>(f.born);
  m.cruise_z = static_cast<f32>(f.cruise_z);
  m.motor = f.motor;
  m.pedestrian = f.pedestrian;
  m.dodge.seconds = static_cast<f32>(f.dodge_seconds);
  m.dodge.lost = f.dodge_lost;
  m.dodge_last = {f.dodge_last.x, f.dodge_last.y};
  m.tracking = f.tracking;
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

f32 speed() { return static_cast<f32>(g_rules.get<f64>("Speed")); }
f32 turn_g() { return static_cast<f32>(g_rules.get<f64>("TurnG")); }
f32 blast_radius() { return static_cast<f32>(g_rules.get<f64>("BlastRadius")); }
f32 damage() { return static_cast<f32>(g_rules.get<f64>("Damage")); }
f32 cooldown() { return static_cast<f32>(g_rules.get<f64>("Cooldown")); }
f32 pick_px() { return static_cast<f32>(g_rules.get<f64>("PickPx")); }
f32 range() { return static_cast<f32>(g_rules.get<f64>("Range")); }
f32 ignite() { return static_cast<f32>(g_rules.get<f64>("Ignite")); }

Dodge update_dodge(Dodge state, Vec2 previous, Vec2 current, f32 dt, bool pedestrian) {
  f64 seconds = 0;
  bool lost = false;
  g_rules.call("UpdateDodge", std::tie(seconds, lost), rules::decimal(state.seconds), state.lost, Vec3{previous.x, previous.y, 0}, Vec3{current.x, current.y, 0}, rules::decimal(dt), pedestrian);
  return {static_cast<f32>(seconds), lost};
}

f32 pick_score(Vec3 eye, Vec3 forward, f32 fov, f32 height_px, Vec3 target) {
  return static_cast<f32>(g_rules.number("PickScore", Vec3(eye), Vec3(forward), rules::decimal(fov), rules::decimal(height_px), Vec3(target)));
}

Missile launch(Vec3 start, Vec3 target, f32 now, bool pedestrian) {
  MissileRun f;
  g_rules.call("Launch", std::tie(f), start, target, rules::decimal(now), pedestrian);
  Missile m;
  take(f, m);
  return m;
}

void track(Missile& m, Vec3 at, f32 dt) {
  MissileRun f;
  g_rules.call("Track", std::tie(f), flight_of(m), at, rules::decimal(dt));
  take(f, m);
}

Result step(Missile& m, f32 now, f32 dt, f32 ground) {
  MissileRun f;
  Result result = Result::flying;
  g_rules.call("Step", std::tie(f, result), flight_of(m), rules::decimal(now), rules::decimal(dt), rules::decimal(ground));
  take(f, m);
  return result;
}

}  // namespace sc::guided
