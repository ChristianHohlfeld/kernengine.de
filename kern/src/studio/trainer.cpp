// Kern Studio's Lehrer workspace (docs/lehrer.md): the Micro Duck on a stage of its own in the
// viewport, and the motion being taught.
//
// Posing: no gravity, the trunk pinned, every limb taken with Box3D's mouse joint (the viewport's
// grab, as in Simulate) and held where it is let go; a keyframe is the 14 joint angles the servos'
// encoders measure then. Trying: the duck on the ground its networks train on runs a learned move.
// The stage is a simulation of a level of its own - the level open in the editor is not touched,
// and the viewport, the tool and the selection come back as they were when the workspace closes.
//
// Training itself runs in the page's workers (kern/web/studio/trainer.mjs) on the headless Lehrer
// (kern/src/lehrer); this file hands it the task and takes the trained network back.
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "internal.h"
#include "kern/game/demos.h"
#include "kern/game/walker.h"
#include "kern/game/walker_policy.h"
#include "kern/studio/studio.h"

namespace kern::studio {

namespace {

// What the Lehrer takes as a keyframed motion (lehrer/motion.cpp, make_motion_task).
constexpr f32 kMinSeconds = 0.3f, kMaxSeconds = 15.0f, kMaxAngle = 2.5f;
constexpr std::size_t kMaxKeys = 32;
constexpr f32 kSameTime = 1e-3f;  // two keyframes closer than this are one

json::Value ok(bool v = true) {
  json::Value r = json::Value::object();
  r.set("ok", v);
  return r;
}

json::Value error(const std::string& text) {
  json::Value r = ok(false);
  r.set("error", text);
  return r;
}

/// A move's name as Blueprints and file names take it: a-z, 0-9, _ and -, at most 38 long.
bool move_name(const std::string& s) {
  if (s.empty() || s.size() > 38) return false;
  for (char c : s)
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  return true;
}

std::string seconds_text(f32 t) {
  char text[32];
  std::snprintf(text, sizeof text, "%.2f s", static_cast<f64>(t));
  return text;
}

/// The stage's duck: alone, not roaming on its own, nobody's player.
ActorId stage_duck(game::Level& l) {
  const ActorId duck = game::add_micro_duck(l, {0, 0, 0});
  if (game::Actor* a = l.find(duck); a && a->controller) a->controller->autonomous = false;
  l.world.player = 0;
  l.world.time_of_day = 0.42f;
  l.world.ao_radius = 0.8f;       // the duck's scale (as its example level)
  l.world.shadow_distance = 10;
  return duck;
}

/// The posing stage: the Micro Duck weightless, nothing else to take hold of.
game::Level pose_stage(ActorId& duck) {
  game::Level l = game::empty_level();
  l.name = "Lehrer";
  l.world.gravity = {0, 0, 0};
  duck = stage_duck(l);
  // Only its hinges limit a pose: there is no floor, and its shells do not collide.
  for (game::Actor& a : l.actors) {
    a.colliders.clear();
    if (a.mesh) a.mesh->collision = false;
  }
  return l;
}

/// The stage to try a move on: the ground the duck's networks train on (lehrer/duck_env.cpp).
game::Level test_stage(ActorId& duck) {
  game::Level l = game::empty_level();
  l.name = "Lehrer";
  l.world.ground = true;
  l.world.ground_friction = 1.0f;
  duck = stage_duck(l);
  return l;
}

}  // namespace

bool Studio::start_stage(TrainerStage stage, const game::Level& level, ActorId duck) {
  end_gizmo(false);
  for (Viewport& v : views_) end_drag(v);
  if (!world_->start(level, game::SimMode::simulate)) {
    report(std::string("Could not start: ") + last_error(), "error");
    return false;
  }
  for (const std::string& e : world_->events()) report(e, "warn");
  world_->events().clear();
  play_ = PlayState::simulating;
  paused_ = false;
  ejected_ = observing_ = false;
  mouse_captured_ = false;
  touch_ctl_.release_all();
  free_touch_ = {};
  trainer_.stage = stage;
  trainer_.duck = duck;
  trainer_.testing.clear();
  ++scene_revision_;
  touch();
  return true;
}

void Studio::frame_stage() {
  Aabb box;
  for (const game::Actor& a : world_->level().actors) {
    Aabb b;
    if (world_->actor_bounds(a.id, b)) box.extend(b);
  }
  if (!box.valid()) return;
  Viewport& v = views_[0];
  // The sphere around the duck inside the view's cone - the narrower of its two angles (a phone
  // held upright is narrower than tall) - with some air around it for the bar on the stage.
  const f32 radius = length(box.extent()) * 0.5f;
  const f32 aspect = min_of(1.0f, v.css_width / max_of(1.0f, v.css_height));
  const f32 half = std::atan(std::tan(v.camera.fov * 0.5f * kDegToRad) * aspect);
  v.camera.position = box.center() - v.camera.forward() * (radius / std::sin(half) * 1.1f);
  touch();
}

bool Studio::open_trainer() {
  if (player_only_) return false;
  if (trainer_open()) return true;
  if (play_ != PlayState::editing) stop();
  Trainer::Before& b = trainer_.before;
  b = {prefs_.layout, active_view_, views_[0].type, views_[0].mode, views_[0].camera, tool_, selection_};
  ActorId duck = 0;
  const game::Level stage = pose_stage(duck);
  if (!start_stage(TrainerStage::pose, stage, duck) || !world_->set_walker_pose_edit(duck, true)) {
    if (play_ != PlayState::editing) end_simulation();
    trainer_.stage = TrainerStage::none;
    trainer_.duck = 0;
    report("The Lehrer's stage could not start", "error");
    return false;
  }
  // One perspective viewport on the duck; nothing of the level is selected or dragged meanwhile,
  // and the Blueprint editor does not lie over it.
  logic_open_ = false;
  selection_.clear();
  prefs_.layout = Layout::single;
  active_view_ = 0;
  views_[0].type = ViewType::perspective;
  views_[0].mode = ViewMode::lit;
  views_[0].camera.yaw = 45;
  views_[0].camera.pitch = -18;
  tool_ = Tool::select;
  frame_stage();
  report("Lehrer - drag a part of the duck to pose it; it holds where you let go");
  return true;
}

void Studio::close_trainer() {
  if (!trainer_open()) return;
  trainer_.stage = TrainerStage::none;
  trainer_.duck = 0;
  trainer_.testing.clear();
  end_simulation();
  const Trainer::Before& b = trainer_.before;
  prefs_.layout = b.layout;
  active_view_ = b.active;
  views_[0].type = b.type;
  views_[0].mode = b.mode;
  views_[0].camera = b.camera;
  views_[0].grid_key = 0;
  tool_ = b.tool;
  selection_ = b.selection;
  report("Lehrer closed - back to the level");
  changed();
}

json::Value Studio::trainer_json() const {
  json::Value o = json::Value::object();
  o.set("stage", trainer_.stage == TrainerStage::test ? "test" : "pose");
  json::Value m = json::Value::object();
  m.set("name", trainer_.name);
  m.set("move", "motion_" + trainer_.name);  // the name Robot Move (walker.skill) plays it by
  m.set("duration", internal::round_num(trainer_.duration, 1000));
  json::Value keys = json::Value::array();
  for (const MotionKey& k : trainer_.keys) {
    json::Value e = json::Value::object();
    e.set("time", internal::round_num(k.time, 1000));
    json::Value q = json::Value::array();
    for (f32 a : k.joints) q.push(internal::round_num(a));
    e.set("joints", std::move(q));
    keys.push(std::move(e));
  }
  m.set("keys", std::move(keys));
  o.set("motion", std::move(m));
  if (!trainer_.testing.empty()) o.set("testing", trainer_.testing);
  return o;
}

json::Value Studio::trainer_command(const std::string& name, const json::Value& c) {
  auto num = [&](const char* k, f64 fallback = 0) { return c[k].is_number() ? c[k].as_number() : fallback; };
  auto str = [&](const char* k) -> std::string { return c[k].is_string() ? c[k].as_string() : std::string(); };
  Trainer& t = trainer_;
  auto done = [&] { touch(); return ok(); };

  if (name == "trainer") {
    const bool open = c["open"].is_bool() ? c["open"].as_bool() : !trainer_open();
    if (!open) { close_trainer(); return ok(); }
    return ok(open_trainer());
  }
  // A learned network becomes a move of the duck (the page handed its bytes over: kern_studio_load_policy).
  if (name == "trainerRegister")
    return ok(game::register_learned_walker_skill(str("move"), str("file"), static_cast<f32>(num("duration"))));
  if (name == "trainerPlay") {
    // In the level: its first Micro Duck plays the move.
    close_trainer();
    if (play_ == PlayState::editing && !play(game::SimMode::simulate)) return error("Could not start the simulation");
    for (ActorId duck : world_->walkers()) {
      const std::string why = world_->walker_skill(duck, str("move"));
      return why.empty() ? ok() : error(why);
    }
    return error("This level has no Micro Duck");
  }

  // ---- the motion being taught ----
  if (name == "trainerMotion") {
    if (c["name"].is_string()) {
      if (!move_name(str("name"))) return error("A motion's name is made of a-z, 0-9, _ and -");
      t.name = str("name");
    }
    if (c["duration"].is_number()) {
      const f32 d = static_cast<f32>(internal::round_num(num("duration"), 1000));
      if (!(d >= kMinSeconds && d <= kMaxSeconds)) return error("A motion lasts 0.3 to 15 seconds");
      // The keyframe at the end stays the end.
      if (t.keys.size() > 1 && std::fabs(t.keys.back().time - t.duration) < kSameTime && d > t.keys[t.keys.size() - 2].time + kSameTime)
        t.keys.back().time = d;
      t.duration = d;
    }
    return done();
  }
  if (name == "trainerKey") {
    const f64 i = num("index", -1);
    if (!(i >= 0 && i < static_cast<f64>(t.keys.size())) || i != std::floor(i)) return error("No such keyframe");
    const std::size_t index = static_cast<std::size_t>(i);
    if (c["remove"].as_bool()) {
      t.keys.erase(t.keys.begin() + static_cast<std::ptrdiff_t>(index));
      return done();
    }
    const f32 time = static_cast<f32>(internal::round_num(num("time", -1), 1000));
    if (!(time >= 0 && time <= t.duration)) return error("A keyframe lies between 0 and the motion's length");
    for (std::size_t k = 0; k < t.keys.size(); ++k)
      if (k != index && std::fabs(t.keys[k].time - time) < kSameTime) return error("There is a keyframe at " + seconds_text(time) + " already");
    t.keys[index].time = time;
    std::stable_sort(t.keys.begin(), t.keys.end(), [](const MotionKey& a, const MotionKey& b) { return a.time < b.time; });
    return done();
  }
  if (name == "trainerClear") {
    t.keys.assign(1, MotionKey{});  // the home pose at the start, as a new motion has it
    return done();
  }
  if (name == "trainerTask") {
    // The motion as the Lehrer's task (lehrer/motion.cpp), or why it cannot be taught yet.
    if (!(t.duration >= kMinSeconds && t.duration <= kMaxSeconds)) return error("A motion lasts 0.3 to 15 seconds");
    if (t.keys.size() < 2) return error("A motion needs two keyframes at least: its start and its end");
    if (t.keys.front().time > 1e-4f) return error("The first keyframe must be at 0 s");
    if (std::fabs(t.keys.back().time - t.duration) > kSameTime) return error("The last keyframe must be at the motion's end (" + seconds_text(t.duration) + ")");
    json::Value spec = json::Value::object();
    spec.set("name", "motion_" + t.name);
    spec.set("duration", internal::round_num(t.duration, 1000));
    json::Value frames = json::Value::array();
    for (const MotionKey& k : t.keys) {
      json::Value f = json::Value::object();
      f.set("time", internal::round_num(k.time, 1000));
      json::Value q = json::Value::array();
      for (f32 a : k.joints) q.push(internal::round_num(a));
      f.set("joints", std::move(q));
      frames.push(std::move(f));
    }
    spec.set("frames", std::move(frames));
    json::Value r = ok();
    r.set("task", "motion:" + json::stringify(spec));
    r.set("move", "motion_" + t.name);
    r.set("file", "motion_" + t.name + ".onnx");
    r.set("duration", internal::round_num(t.duration, 1000));
    return r;
  }

  // ---- the stage ----
  if (!trainer_open()) return error("Open the Lehrer first");
  if (name == "trainerStage") {
    // Posing again, from the home pose.
    ActorId duck = 0;
    const game::Level stage = pose_stage(duck);
    if (!start_stage(TrainerStage::pose, stage, duck) || !world_->set_walker_pose_edit(duck, true)) return error("The Lehrer's stage could not start");
    return ok();
  }
  if (name == "trainerTest") {
    // The duck on the ground, as an episode of its training starts: it runs the learned move.
    const std::string move = str("move");
    ActorId duck = 0;
    const game::Level stage = test_stage(duck);
    if (!start_stage(TrainerStage::test, stage, duck)) return error("The Lehrer's stage could not start");
    const std::string why = world_->walker_skill(duck, move);
    if (!why.empty()) return error(why);
    t.testing = move;
    return done();
  }
  if (t.stage != TrainerStage::pose) return error("Go back to posing first");
  game::GameWorld::WalkerProbe probe;
  if (!world_->walker_probe(t.duck, probe)) return error("The stage has no Micro Duck");
  if (name == "trainerPose") {
    // What the servos' encoders read now, and which joint is which.
    json::Value r = ok();
    json::Value angles = json::Value::array(), names = json::Value::array();
    for (f32 a : probe.joint) angles.push(internal::round_num(a));
    for (const auto& joint : world_->walker_joints(t.duck))
      if (names.size() < 14) names.push(joint.name);
    r.set("joints", std::move(angles));
    r.set("names", std::move(names));
    return r;
  }
  if (name == "trainerCapture") {
    const f32 time = static_cast<f32>(internal::round_num(num("time", -1), 1000));
    if (!(time >= 0 && time <= t.duration)) return error("A keyframe lies between 0 and the motion's length");
    MotionKey key;
    key.time = time;
    for (i32 j = 0; j < 14; ++j) {
      key.joints[j] = static_cast<f32>(internal::round_num(probe.joint[j]));
      if (!std::isfinite(key.joints[j]) || std::fabs(key.joints[j]) > kMaxAngle) return error("The pose is outside what the servos reach");
    }
    auto at = std::find_if(t.keys.begin(), t.keys.end(), [&](const MotionKey& k) { return std::fabs(k.time - time) < kSameTime; });
    if (at != t.keys.end()) *at = key;  // a pose taken again at the same time replaces the old one
    else {
      if (t.keys.size() >= kMaxKeys) return error("A motion has 32 keyframes at most");
      t.keys.insert(std::upper_bound(t.keys.begin(), t.keys.end(), key, [](const MotionKey& a, const MotionKey& b) { return a.time < b.time; }), key);
    }
    return done();
  }
  if (name == "trainerShow") {
    // The servos take the puppet to a keyframe, as the robot would reach it.
    const f64 i = num("index", -1);
    if (!(i >= 0 && i < static_cast<f64>(t.keys.size())) || i != std::floor(i)) return error("No such keyframe");
    return ok(world_->set_walker_pose(t.duck, t.keys[static_cast<std::size_t>(i)].joints));
  }
  return error("unknown command " + name);
}

}  // namespace kern::studio
