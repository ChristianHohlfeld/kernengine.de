// SpawnCity on Kern: the drone's GUIDED missile in the world (guided_missile.mjs press-fires-v3,
// guided_balance.mjs; the flight logic is guided.cpp). A press picks what is nearest to the
// crosshair (people, cars and buses, police drones, other players) and launches at once; with
// nothing there it flies to the aimed point. 5 s between launches; an early press says how long.
#include <cmath>

#include "game.h"
#include "guided.h"

namespace sc {

using kern::Mat4;

namespace {
constexpr f32 kGuidedMarkSeconds = 8;
bool person_up(const People::Person& p) { return p.state != People::State::down && p.state != People::State::gone; }
const char* person_label(const People::Person& p) {
  switch (p.kind) {
    case People::Kind::officer: return "POLIZIST";
    case People::Kind::soldier: return "SOLDAT";
    case People::Kind::zombie: return "ZOMBIE";
    case People::Kind::enemy: return "GEGNER";
    default: return "PERSON";
  }
}
}  // namespace

bool Game::guided_target_at(const guided::Target& t, Vec3& at) const {
  using guided::TargetKind;
  switch (t.kind) {
    case TargetKind::person: {
      if (t.index < 0 || t.index >= static_cast<i32>(people_.all().size())) return false;
      const People::Person& p = people_.all()[static_cast<std::size_t>(t.index)];
      if (!person_up(p)) return false;
      at = p.pos + Vec3{0, 0, 1.1f};
      return true;
    }
    case TargetKind::car: {
      if (t.index < 0 || t.index >= static_cast<i32>(cars_.size()) || !cars_[t.index].placed) return false;
      at = matrix_of(cars_[t.index].actor).translation() + Vec3{0, 0, 0.8f};
      return true;
    }
    case TargetKind::police_drone:
      if (t.index < 0 || t.index >= 5 || !pdrones_[t.index].active) return false;
      at = pdrones_[t.index].pos;
      return true;
    case TargetKind::player:
      for (const net::Remote& r : const_cast<Game*>(this)->net_.remotes())
        if (r.state.slot == t.slot) { at = r.shown + Vec3{0, 0, r.state.mode == net::PlayerMode::foot ? 1.1f : 0.0f}; return true; }
      return false;
    default: return false;
  }
}

bool Game::guided_press() {
  // Two taps: the first marks what is nearest the crosshair (a ring sits on it, two beeps), the
  // second launches at the mark. A mark lasts 8 s; a lost target (dead, gone) drops it.
  const f32 now = world_->sim_time();
  if (!drone_out_ || player_dead_) return false;
  if (now < guided_last_ + guided::kCooldown) {
    // A press during the reload never silently does nothing: it says how long is left.
    char b[64];
    std::snprintf(b, sizeof b, "LENKRAKETE LAEDT - %.1f s", guided_last_ + guided::kCooldown - now);
    toast_ = b;
    toast_age_ = 0;
    world_->play_sound("builtin:click", cam_.eye, 0.4f, 0.7f, false);
    osfx("guided_missile.denied", cam_.eye);
    return false;
  }
  Vec3 fwd;
  const Vec3 eye = aim_ray(fwd);
  Vec3 mark_now;
  const bool marked = guided_marked_ && now - guided_mark_time_ < kGuidedMarkSeconds &&
                      (guided_mark_.kind == guided::TargetKind::point || guided_target_at(guided_mark_, mark_now));
  if (!marked) {
    const f32 height = std::fmax(1.0f, frame_h_);
    guided::Target best;
    f32 best_score = guided::kPickPx;
    Vec3 best_at;
    auto consider = [&](const guided::Target& t, Vec3 at) {
      const f32 dist = kern::length(at - eye);
      if (dist > guided::kRange || dist < 3) return;
      const f32 s = guided::pick_score(eye, fwd, cam_.fov, height, at);
      if (s >= 0 && s < best_score) { best_score = s; best = t; best_at = at; }
    };
    for (std::size_t i = 0; i < people_.all().size(); ++i) {
      const People::Person& p = people_.all()[i];
      if (person_up(p)) consider({guided::TargetKind::person, static_cast<i32>(i), 0, person_label(p)}, p.pos + Vec3{0, 0, 1.1f});
    }
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      if (!cars_[i].placed || cars_[i].wreck) continue;
      const bool bus = cars_[i].car->spec().radius > 0.4f;
      consider({guided::TargetKind::car, static_cast<i32>(i), 0, bus ? "BUS" : "AUTO"}, matrix_of(cars_[i].actor).translation() + Vec3{0, 0, 0.8f});
    }
    for (i32 i = 0; i < 5; ++i)
      if (pdrones_[i].active) consider({guided::TargetKind::police_drone, i, 0, "POLIZEIDROHNE"}, pdrones_[i].pos);
    for (const net::Remote& r : net_.remotes())
      consider({guided::TargetKind::player, -1, r.state.slot, r.state.mode == net::PlayerMode::drone ? "DROHNE" : "SPIELER"},
               r.shown + Vec3{0, 0, r.state.mode == net::PlayerMode::foot ? 1.1f : 0.0f});
    if (best.kind == guided::TargetKind::point) {
      // Nothing to home on: the spot you aimed at.
      const RayResult r = cast(world_->physics(), eye, fwd, guided::kRange, body(drone_actor_));
      best_at = r.hit ? r.point : eye + fwd * guided::kRange;
      best.label = "ZIELPUNKT";
    }
    gaps_guided_sounds();
    world_->play_sound("sc:locktone", eye, 1, 1, false);  // lockTone: two 1760 Hz beeps (game_gaps.cpp)
    osfx("guided_missile.lockTone", eye);
    guided_mark_ = best;
    guided_mark_at_ = best_at;
    guided_mark_time_ = now;
    guided_marked_ = true;
    toast_ = std::string("MARKIERT: ") + best.label + " - NOCHMAL TIPPEN = FEUER";
    toast_age_ = 0;
    return false;
  }
  const guided::Target best = guided_mark_;
  const Vec3 best_at = best.kind == guided::TargetKind::point ? guided_mark_at_ : mark_now;
  guided_marked_ = false;
  const BodyState s = state_of(drone_actor_);
  bool pedestrian = best.kind == guided::TargetKind::person;
  if (best.kind == guided::TargetKind::player)
    for (const net::Remote& r : net_.remotes())
      if (r.state.slot == best.slot) pedestrian = r.state.mode == net::PlayerMode::foot;
  guided::Flight f;
  f.m = guided::launch(s.position - Vec3{0, 0, 0.35f}, best_at, now, pedestrian);
  f.m.v = f.m.v + s.velocity;  // it leaves the drone with the drone's speed
  f.t = best;
  f.prev = f.m.p;
  guided_.push_back(f);
  guided_last_ = now;
  guided_lock_ = best.label;
  if (best.kind != guided::TargetKind::point && pedestrian) {
    toast_ = std::string("GUIDED LOCK - ") + best.label + " - SPRINT ZUM AUSWEICHEN";
    toast_age_ = 0;
  } else {
    toast_ = std::string("LENKRAKETE AUF ") + best.label;
    toast_age_ = 0;
  }
  // no launch sound: it drops off the rail; the ignition bang and the motor come when it lights (game_gaps.cpp)
  people_.frighten(s.position, 18, now, 1);
  return true;
}

void Game::update_guided(f32 dt) {
  if (guided_.empty() || dt <= 0) return;
  const f32 now = world_->sim_time();
  for (std::size_t i = 0; i < guided_.size();) {
    guided::Flight& f = guided_[i];
    Vec3 at;
    if (f.m.tracking) {
      if (f.t.kind != guided::TargetKind::point && !guided_target_at(f.t, at)) f.m.tracking = false;  // gone: the last place it knew
      else if (f.t.kind != guided::TargetKind::point) guided::track(f.m, at, dt);
    }
    const guided::Result res = guided::step(f.m, now, dt, city_.ground_height(f.m.p.x, f.m.p.y));
    bool boom = res != guided::Result::flying;
    Vec3 blast = f.m.p;
    // A building (or anything solid) in the way: it goes off at the wall.
    if (!boom) {
      const Vec3 d = f.m.p - f.prev;
      const f32 l = kern::length(d);
      if (l > 1e-4f) {
        const RayResult r = cast(world_->physics(), f.prev, d * (1.0f / l), l, body(drone_actor_), kCatWorld);
        if (r.hit && now > f.m.born + 0.15f) { boom = true; blast = r.point - d * (0.1f / l); }
      }
    }
    if (boom) {
      explosion(blast, guided::kBlastRadius, guided::kDamage, true, BlastBy::missile);
      guided_[i] = guided_.back();
      guided_.pop_back();
      continue;
    }
    f.prev = f.m.p;
    ++i;
  }
}

void Game::draw_guided() {
  for (const guided::Flight& f : guided_) people_art_.projectile(f.m.p, f.m.v, 0.06f, 0.9f, f.m.motor);
}

void Game::draw_guided_mark() {
  // the mark: a red ring on the ground under it and a pulsing halo on it, turned to the eye
  const f32 now = world_->sim_time();
  if (!guided_marked_ || now - guided_mark_time_ >= kGuidedMarkSeconds || !drone_out_) return;
  Vec3 at = guided_mark_at_;
  if (guided_mark_.kind != guided::TargetKind::point && !guided_target_at(guided_mark_, at)) return;
  const f32 pulse = 0.85f + 0.15f * std::sin(now * 12);
  const f32 d = std::fmax(1.0f, kern::length(at - cam_.eye));
  const f32 r = std::fmax(1.2f, d * 0.025f) * pulse;
  const Vec3 n = kern::normalize(cam_.eye - at, {0, 0, 1});
  const Vec3 ax = kern::cross({0, 0, 1}, n);
  const f32 al = kern::length(ax);
  const Quat q = al > 1e-4f ? Quat::axis_angle(ax * (1 / al), std::acos(clampf(n.z, -1, 1))) : Quat{};
  CombatArt& A = combat_art_;
  A.put(CombatArt::fade_ring, Mat4::translate(at) * Mat4::trs({}, q, {1, 1, 1}) * Mat4::scale({r, r, 0.05f}), Vec3{1.0f, 0.12f, 0.08f} * 3.0f, 0.85f);
  const f32 g = city_.ground_height(at.x, at.y) + 0.08f;
  A.put(CombatArt::fade_ring, Mat4::translate({at.x, at.y, g}) * Mat4::scale({r * 1.4f, r * 1.4f, 0.05f}), Vec3{1.0f, 0.12f, 0.08f} * 2.5f, 0.6f);
}

void Game::guided_hud(std::string& s) {
  const f32 now = world_->sim_time();
  char b[160];
  f32 dist = -1;
  const char* lock = "";
  if (!guided_.empty()) {
    const guided::Flight& f = guided_.back();
    dist = kern::length(f.m.goal - f.m.p);
    lock = f.m.tracking ? f.t.label : "VERLOREN";
  }
  const bool marked = guided_marked_ && now - guided_mark_time_ < kGuidedMarkSeconds;
  std::snprintf(b, sizeof b, ",\"guided\":{\"cd\":%.1f,\"flying\":%zu,\"lock\":\"%s\",\"dist\":%.0f,\"last\":\"%s\",\"marked\":\"%s\"}",
                std::fmax(0.0f, guided_last_ + guided::kCooldown - now), guided_.size(), lock, dist, guided_lock_.c_str(), marked ? guided_mark_.label : "");
  s += b;
}

}  // namespace sc
