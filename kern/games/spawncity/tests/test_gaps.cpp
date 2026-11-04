// sc-m3-gaps: spawn visibility and vehicle clearance (spawn_guard.h), sirens, dispatch, critical
// smoke, organic hit audio (gaps_rules.h).
#include "../src/gaps_rules.h"
#include "../src/jet_seats.h"
#include "../src/lamps.h"
#include "../src/spawn_guard.h"
#include "test_util.h"

using namespace sc;

SC_TEST(spawn_guard_rules) {
  spawn::Viewer v;
  v.eye = {0, 0, 1.7f};
  v.forward = {1, 0, 0};
  v.half_angle = spawn::cone_half_angle(60 * kDeg, 16.0f / 9);
  v.local = true;
  const std::vector<spawn::Viewer> vs{v};
  // the 60 deg vertical / 16:9 view is ~91 deg wide: half 45.7 + 20 deg
  SC_CHECK(std::fabs(v.half_angle - (45.7f + 20) * kDeg) < 0.6f * kDeg, "half %.2f", v.half_angle / kDeg);
  SC_CHECK(!spawn::can_spawn(vs, {10, -15, 0}, 0, 2.2f, nullptr), "closer than 24 m is always seen");
  SC_CHECK(!spawn::can_spawn(vs, {-20, 5, 0}, 0, 2.2f, nullptr), "behind but within 24 m");
  SC_CHECK(spawn::can_spawn(vs, {-60, 0, 0}, 0, 2.2f, nullptr), "behind, far enough");
  SC_CHECK(!spawn::can_spawn(vs, {80, 20, 0}, 0, 2.2f, nullptr), "in view");
  // a building in between hides it - both the feet and the top
  auto wall = [](Vec3, Vec3) { return true; };
  SC_CHECK(spawn::can_spawn(vs, {80, 20, 0}, 0, 2.2f, wall), "hidden by a building");
  SC_CHECK(!spawn::can_spawn(vs, {500, 0, 0}, 0, 2.2f, wall), "beyond 420 m buildings do not count");
  SC_CHECK(spawn::can_spawn(vs, {1600, 0, 0}, 0, 2.2f, nullptr), "beyond the far plane");
  SC_CHECK(spawn::can_spawn(vs, {300, 0, 0}, 250, 2.2f, nullptr), "far entry allowed in view");
  // a mate looking the other way sees what we do not
  spawn::Viewer mate;
  mate.eye = {0, 0, 1.7f};
  mate.forward = {-1, 0, 0};
  mate.half_angle = v.half_angle;
  SC_CHECK(!spawn::can_spawn({v, mate}, {-60, 0, 0}, 0, 2.2f, nullptr), "in the mate's view");
}

SC_TEST(vehicle_clearance_sat) {
  const spawn::Footprint a{{0, 0}, 0, {2.3f, 0.95f}};
  SC_CHECK(spawn::footprints_overlap(a, {{4.0f, 0}, 0, {2.3f, 0.95f}}), "nose to tail 4 m apart overlap (4.6 m long)");
  SC_CHECK(!spawn::footprints_overlap(a, {{5.2f, 0}, 0, {2.3f, 0.95f}}), "5.2 m apart: clear with 0.35 margin");
  SC_CHECK(spawn::footprints_overlap(a, {{4.9f, 0}, 0, {2.3f, 0.95f}}), "4.9 m apart: inside the margin");
  SC_CHECK(!spawn::footprints_overlap(a, {{0, 2.5f}, 0, {2.3f, 0.95f}}), "side by side 2.5 m");
  SC_CHECK(spawn::footprints_overlap(a, {{2.5f, 1.4f}, kPi / 2, {2.3f, 0.95f}}), "crossing at a right angle");
}

SC_TEST(gaps_sirens) {
  SC_CHECK(gaps::horn_hz(0.1f) == gaps::kHornLow && gaps::horn_hz(0.7f) == gaps::kHornHigh && gaps::horn_hz(1.4f) == gaps::kHornLow, "two tones of 0.66 s");
  SC_CHECK(std::fabs(gaps::whoop_hz(0) - 760) < 1 && std::fabs(gaps::whoop_hz(0.6999f) - 1480) < 2 && gaps::whoop_hz(0.9f) == 760, "whoop %f", gaps::whoop_hz(0.6999f));
  SC_CHECK(std::fabs(gaps::siren_gain(0.06f, 0, 1) - 0.06f) < 1e-6f && gaps::siren_gain(0.06f, 261, 1) == 0, "level law");
  SC_CHECK(std::fabs(gaps::siren_gain(0.06f, 14, 1) - 0.03f) < 1e-6f, "half at 14 m");
  SC_CHECK(gaps::siren_cutoff(0) == 5200 && gaps::siren_cutoff(400) == 900 && std::fabs(gaps::siren_cutoff(100) - 3600) < 1e-3f, "cutoff");
  gaps::SirenVoice v;
  for (int i = 0; i < 120; ++i) v.update(100 - i * 0.25f, 1.0f / 60, gaps::kCarGain, 1);  // closing at 15 m/s
  SC_CHECK(v.doppler > 1.03f && v.doppler < 1.05f, "approaching raises the pitch %f", v.doppler);
  SC_CHECK(gaps::post_arrival_grace_ms(1) == 675 && gaps::post_arrival_grace_ms(5) == 495, "grace");
}

SC_TEST(gaps_smoke) {
  gaps::SmokeField f(78);
  const f32 at = f.start("car-1", {0, 0, 1}, 0.92f, 2.45f, 10);
  SC_CHECK(std::fabs(at - 12.45f) < 1e-4f && f.live_puffs() == 1, "first puff at once");
  SC_CHECK(std::fabs(f.start("car-1", {1, 0, 1}, 0.92f, 9, 11) - 12.45f) < 1e-4f, "a running effect keeps its clock");
  u32 embers = 0;
  for (f32 t = 10; t < 12.4f; t += 1.0f / 60) {
    f.update(t, 1.0f / 60);
    for (const gaps::Puff& p : f.puffs()) embers += p.alive && p.ember ? 1 : 0;
  }
  SC_CHECK(f.live_puffs() > 12 && f.live_puffs() <= 78, "puffs %u", f.live_puffs());
  SC_CHECK(embers > 0, "embers late in the countdown");
  SC_CHECK(f.accelerate("car-1", 0, 12.3f) && f.effects() == 1, "accelerate");
  f.update(12.49f, 0.01f);
  SC_CHECK(f.effects() == 0, "expired");
  SC_CHECK(gaps::organic_rate(gaps::Organic::bird) == 1.16f && gaps::organic_rate(gaps::Organic::animal) == 0.94f, "rates");
}

SC_TEST(gaps_visual_pose) {
  gaps::VisualPose v;
  Vec3 p;
  Quat q;
  v.update({0, 0, 1}, Quat{}, {0, 0, 1}, 1, 1.0f / 60, p, q);
  SC_CHECK(kern::length(p - Vec3{0, 0, 1}) < 1e-5f, "first frame: the raw pose");
  // a sudden 0.5 m jump of the body: the drawn airframe follows, but never more than 0.10 m behind
  v.update({0.5f, 0, 1}, Quat{}, {0, 0, 1}, 1, 1.0f / 60, p, q);
  SC_CHECK(p.x > 0.39f && p.x < 0.5f, "capped lag %f", p.x);
  // a 30 deg roll: the rotation error is capped at 10 deg
  const Quat roll = Quat::axis_angle({1, 0, 0}, 30 * kDeg);
  v.update({0.5f, 0, 1}, roll, {0, 0, 1}, 1, 1.0f / 60, p, q);
  SC_CHECK(gaps::quat_angle(q, roll) <= 10 * kDeg + 1e-3f && gaps::quat_angle(q, roll) > 5 * kDeg, "rot err %f", gaps::quat_angle(q, roll) / kDeg);
  for (int i = 0; i < 120; ++i) v.update({0.5f, 0, 1}, roll, {0, 0, 1}, 1, 1.0f / 60, p, q);
  SC_CHECK(gaps::quat_angle(q, roll) < 0.5f * kDeg && std::fabs(p.x - 0.5f) < 1e-3f, "settles");
}

SC_TEST(gaps_street_lamps) {
  lamps::Field f;
  // a straight 200 m street, 8 m wide: lamps from 17 m on every 34 m, alternating kerbs, 1.15 m out
  f.place_road({{0, 0}, {200, 0}}, 8, 0, [](f32, f32) { return 2.0f; });
  SC_CHECK(f.all().size() == 6, "lamps %zu", f.all().size());
  const lamps::Lamp& a = f.all()[0];
  SC_CHECK(std::fabs(a.foot.x - 17) < 1e-3f && std::fabs(std::fabs(a.foot.y) - 5.15f) < 1e-3f && a.foot.z == 2, "first lamp %f %f", a.foot.x, a.foot.y);
  SC_CHECK(f.all()[0].foot.y * f.all()[1].foot.y < 0, "alternating sides");
  SC_CHECK(std::fabs(std::fabs(a.head.y) - (5.15f - 1.42f)) < 1e-3f, "the arm reaches over the road");
  // placing the same street again adds nothing (one lamp per 8 m cell)
  f.place_road({{0, 0}, {200, 0}}, 8, 1, nullptr);
  SC_CHECK(f.all().size() == 6, "dedup");
  // a round through the head
  const Vec3 h{a.head.x, a.head.y, a.foot.z + lamps::kHeadZ};
  const i32 hit = f.shot(h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0});
  SC_CHECK(hit == 0 && f.set_state(hit, 1) && !f.set_state(hit, 1) && f.shot(h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0}) == -1, "shot out once");
  // a blast: posts within 60 % go down, further ones only darken
  const auto b = f.blast({51, 0, 0}, 12);
  SC_CHECK(b.size() == 1 && b[0].state == 2 && b[0].lamp == 1, "blast %zu", b.size());
  // a car at 10 m/s driving into a post topples it; at 2 m/s it is stopped
  const lamps::Lamp& c = f.all()[2];
  auto fast = f.car({c.foot.x - 1.5f, c.foot.y, 0}, 0, {10, 0, 0}, 1.88f, 0.86f, 1350);
  SC_CHECK(fast.size() == 1 && fast[0].topple && fast[0].impulse.x < 0, "sheared off");
  auto slow = f.car({c.foot.x - 1.5f, c.foot.y, 0}, 0, {2, 0, 0}, 1.88f, 0.86f, 1350);
  SC_CHECK(slow.size() == 1 && !slow[0].topple && std::fabs(slow[0].impulse.x + 1350 * 2 * 1.05f) < 1, "stopped %f", slow.empty() ? 0.0f : slow[0].impulse.x);
  // broken lamps stay broken when the street is placed again; RESET puts them back
  f.clear();
  f.place_road({{0, 0}, {200, 0}}, 8, 0, nullptr);
  SC_CHECK(f.all()[0].state == 1, "remembered");
  f.reset_broken();
  SC_CHECK(f.all()[0].state == 0 && f.broken() == 0, "reset");
}

SC_TEST(gaps_jet_seats) {
  using seats::Result;
  using seats::Seat;
  seats::Table t;
  SC_CHECK(t.claim(1, 2, Seat::pilot) == Result::owner_only_pilot, "only the owner flies");
  SC_CHECK(t.claim(1, 2, Seat::passenger) == Result::no_pilot, "no ride without a pilot");
  SC_CHECK(t.claim(1, 1, Seat::pilot) == Result::ok && t.claim(1, 1, Seat::pilot) == Result::unchanged, "pilot");
  SC_CHECK(t.claim(1, 2, Seat::passenger) == Result::ok, "passenger");
  SC_CHECK(t.claim(1, 3, Seat::passenger) == Result::seat_occupied, "one passenger");
  SC_CHECK(t.claim(3, 2, Seat::pilot) == Result::owner_only_pilot && t.claim(2, 2, Seat::pilot) == Result::already_seated, "one seat per player");
  seats::Row r;
  Seat s;
  SC_CHECK(t.seat_of(2, r, s) && r.owner == 1 && s == Seat::passenger, "seat_of");
  // the pilot leaving takes the passenger out (no unmanaged flight)
  SC_CHECK(t.release(1, 1, Seat::pilot) == Result::ok && !t.seat_of(2, r, s) && t.rows().empty(), "failsafe");
  SC_CHECK(t.claim(255, 255, Seat::pilot) == Result::ok && t.claim(255, 0, Seat::passenger) == Result::ok, "the host's jet");
  SC_CHECK(t.drop(0) && t.get(255).passenger == seats::kNone && t.get(255).pilot == 255, "a leaving passenger");
  seats::Table u;
  SC_CHECK(u.load(t.revision(), t.rows()) && u.get(255).pilot == 255, "load");
  SC_CHECK(!u.load(9, {{1, 2, seats::kNone}}), "a pilot must be the owner");
}
