// SpawnCity on Kern: riding along in a mate's jet (jet_mode.mjs passenger, jet_seat_coordinator.mjs,
// rules in jet_seats.h). The authority keeps the seat table; a pilot holds the pilot seat of his own
// jet while he flies it. On foot, F next to a mate's landed jet with its pilot in (9 m, nearer than
// our own jet) asks for the passenger seat; once the table says so, we ride: the person is gone,
// the camera sits in the jet (0.48, 2.5, 1.25 in its frame, 72 deg), nothing to fire or fly. F again
// leaves: above 10 m over the ground on a parachute, else 5 m ahead on foot. The pilot leaving (or
// his pose going stale for 3.5 s) takes us out the same way.
#include <cmath>

#include "game.h"

namespace sc {

namespace {
constexpr f32 kBoardM = 9, kStaleS = 3.5f, kPendingS = 3;  // jet_mode.mjs ENTER_M, passengerJetPose 3500 ms, coordinator timeout
constexpr f32 kChuteAgl = 10;                               // leavePassenger: a parachute above 10 m
}  // namespace

bool Game::gaps_riding() const { return gaps_ && gaps_->riding; }

bool Game::gaps_ride_pose(Vec3& p, Quat& q) const {
  if (!gaps_ || !gaps_->riding) return false;
  const net::Remote* r = net_.remote(gaps_->ride_owner);
  if (!r) return false;
  if (r->state.mode == net::PlayerMode::jet || r->state.mode == net::PlayerMode::car) { p = r->shown; q = r->shown_rotation; return true; }
  if (r->state.flags & net::kJetOut) { p = r->state.jet_pos; q = r->state.jet_rot; return true; }
  return false;
}

bool Game::gaps_seat_press() {
  if (!gaps_ || !net_.active()) return false;
  GapsState& g = *gaps_;
  if (g.riding) { gaps_leave_ride(); return true; }
  if (mode_ != Mode::foot || player_dead_ || g.seat_pending_until > world_->sim_time()) return false;
  const Vec3 me = matrix_of(soldier_).translation();
  // nearestRemoteBoardable: landed, its pilot in his seat, within 9 m
  i32 best = -1;
  f32 bd = kBoardM;
  for (const net::Remote& r : net_.remotes()) {
    // a landed jet, or a car standing (nearly) still - its driver at the wheel - within reach
    const bool jet = r.state.mode == net::PlayerMode::jet && (r.state.flags & net::kGearDown) && r.state.speed <= 4;
    const bool car = r.state.mode == net::PlayerMode::car && r.state.speed <= 3;
    if (!jet && !car) continue;
    if (net_.seats().get(r.state.slot).pilot != r.state.slot) continue;
    const f32 d = std::hypot(r.shown.x - me.x, r.shown.y - me.y);
    if (d < (car ? 5.5f : bd) && d < bd) { bd = d; best = r.state.slot; }
  }
  if (best < 0) return false;
  if (jet_placed_) {
    const Vec3 j = matrix_of(jet_actor_).translation();
    if (std::hypot(j.x - me.x, j.y - me.y) <= bd) return false;  // our own jet is nearer
  }
  g.seat_pending_until = world_->sim_time() + kPendingS;
  g.pending_owner = static_cast<u8>(best);
  net_.seat_request(true, g.pending_owner, seats::Seat::passenger);
  return true;
}

void Game::gaps_leave_ride() {
  GapsState& g = *gaps_;
  Vec3 p;
  Quat q;
  const bool have = gaps_ride_pose(p, q);
  if (g.riding) net_.seat_request(false, g.ride_owner, seats::Seat::passenger);
  g.riding = false;
  if (!have) { p = matrix_of(soldier_).translation(); q = Quat{}; }
  const net::Remote* rr = net_.remote(g.ride_owner);
  const bool in_car = rr && rr->state.mode == net::PlayerMode::car;
  const Vec3 nose = in_car ? q.rotate({1, 0, 0}) : q.rotate({0, 1, 0});
  const f32 h = std::atan2(nose.y, nose.x), agl = p.z - city_.ground_height(p.x, p.y);
  if (in_car) {
    // out of the passenger door: 2.2 m to the right of the car
    Vec3 out = p + q.rotate({0, -2.2f, 0});
    show_soldier(Vec3{out.x, out.y, city_.ground_height(out.x, out.y)}, h / kDeg);
    mode_ = Mode::foot;
  } else if (agl > kChuteAgl) {
    // a controlled parachute instead of a jump to the ground mid-flight
    world_->set_alive(chute_actor_, true);
    world_->teleport(chute_actor_, p + Vec3{0, 0, 2});
    world_->set_actor_velocity(chute_actor_, {});
    chute_time_ = 0;
    chute_open_ = false;
    mode_ = Mode::parachute;
    rig_init_ = false;
  } else {
    Vec3 out = p + Vec3{std::cos(h), std::sin(h), 0} * 5.0f;
    out.z = city_.ground_height(out.x, out.y);
    show_soldier(out, h / kDeg);
    mode_ = Mode::foot;
  }
  toast_ = "Ausgestiegen";
  toast_age_ = 0;
}

void Game::gaps_seats_update(f32 dt) {
  (void)dt;
  if (!gaps_ || !world_placed_) return;
  GapsState& g = *gaps_;
  const f32 now = world_->sim_time();
  const u8 self = net_.self_slot();
  // the pilot seat of our own jet (or the driver's of our car) while we fly / drive it
  const bool at_wheel = mode_ == Mode::jet || mode_ == Mode::car;
  if (at_wheel && !g.pilot_held) { net_.seat_request(true, self, seats::Seat::pilot); g.pilot_held = true; }
  if (!at_wheel && g.pilot_held) { net_.seat_request(false, self, seats::Seat::pilot); g.pilot_held = false; }
  if (!net_.active()) { g.riding = false; return; }
  seats::Row row;
  seats::Seat seat;
  const bool seated = net_.seats().seat_of(self, row, seat) && seat == seats::Seat::passenger;
  if (!g.riding && g.seat_pending_until > 0) {
    if (seated && row.owner == g.pending_owner) {
      // beginPassenger: the claim came back - still next to the jet?
      g.seat_pending_until = -1;
      g.ride_owner = row.owner;
      const net::Remote* r = net_.remote(row.owner);
      const Vec3 me = matrix_of(soldier_).translation();
      if (!r || mode_ != Mode::foot || std::hypot(r->shown.x - me.x, r->shown.y - me.y) > kBoardM + 3) {
        net_.seat_request(false, row.owner, seats::Seat::passenger);
        return;
      }
      g.riding = true;
      g.ride_seen = now;
      hide_soldier();
      toast_ = r->state.mode == net::PlayerMode::car ? "Beifahrer" : "Mitflieger";
      toast_age_ = 0;
    } else if (now > g.seat_pending_until) {
      g.seat_pending_until = -1;
      toast_ = "Kein Platz frei";
      toast_age_ = 0;
    }
  }
  if (g.riding) {
    Vec3 p;
    Quat q;
    if (gaps_ride_pose(p, q)) g.ride_seen = now;
    // the pilot got out (his seat row is gone) or his jet went quiet: out we go
    if (!(seated && row.owner == g.ride_owner) || now - g.ride_seen > kStaleS) gaps_leave_ride();
  }
}

bool Game::gaps_ride_camera(kern::Camera& c) {
  Vec3 p;
  Quat q;
  if (!gaps_ride_pose(p, q)) return false;
  const net::Remote* r = gaps_ ? net_.remote(gaps_->ride_owner) : nullptr;
  if (r && r->state.mode == net::PlayerMode::car) {
    // the passenger seat: right of the driver (the car's frame: x forward, y left), looking ahead
    c.eye = p + q.rotate({0.04f, -0.36f, 0.7f});
    c.target = c.eye + q.rotate({1, 0, 0}) * 10.0f;
    c.up = q.rotate({0, 0, 1});
    c.fov = 80 * kDeg;
    c.near = 0.04f;
    c.far = 2400;
    return true;
  }
  // applyPassengerCamera: (0.48, 2.5, 1.25) in the jet's frame, looking along its nose
  c.eye = p + q.rotate({0.48f, 2.5f, 1.25f});
  c.target = c.eye + q.rotate({0, 1, 0}) * 10.0f;
  c.up = q.rotate({0, 0, 1});
  c.fov = 72 * kDeg;
  c.near = 0.5f;
  c.far = 6000;
  return true;
}

}  // namespace sc
