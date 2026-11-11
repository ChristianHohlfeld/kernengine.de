// Kern Studio's Lehrer workspace (docs/lehrer.md): the Micro Duck on a stage of its own in the
// viewport, and the motions it is taught.
//
// Posing: no gravity, the trunk pinned, every limb taken with Box3D's mouse joint (the viewport's
// grab, as in Simulate) and held where it is let go; a keyframe is the 14 joint angles the servos'
// encoders measure then. Trying: the duck on the ground its networks train on runs a learned move.
// The stage is a simulation of a level of its own - the level's actors are not touched, and the
// viewport, the tool and the selection come back as they were when the workspace closes.
//
// What the duck is taught belongs to the level (Level::motions): a motion's name, length and
// keyframes and its trained network are edited as steps of the level's history, saved with it and
// played by its walkers (GameWorld::walker_skill). Training itself runs in the page's workers
// (kern/web/studio/trainer.mjs) on the headless Lehrer (kern/src/lehrer); this file hands it the
// task and takes the trained network back.
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

using game::kMotionMaxAngle, game::kMotionMaxKeys, game::kMotionMaxSeconds, game::kMotionMinSeconds;
using game::MotionAsset, game::MotionKey;
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

/// The stage to try a move on: the ground the duck's networks train on (lehrer/duck_env.cpp),
/// and what the level taught its ducks.
game::Level test_stage(ActorId& duck, const std::vector<MotionAsset>& motions) {
  game::Level l = game::empty_level();
  l.name = "Lehrer";
  l.world.ground = true;
  l.world.ground_friction = 1.0f;
  l.motions = motions;
  duck = stage_duck(l);
  return l;
}

json::Value key_json(const MotionKey& k) {
  json::Value e = json::Value::object();
  e.set("time", internal::round_num(k.time, 1000));
  json::Value q = json::Value::array();
  for (f32 a : k.joints) q.push(internal::round_num(a));
  e.set("joints", std::move(q));
  return e;
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
  if (!level_.motion(trainer_.motion)) fresh_motion();
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

const MotionAsset& Studio::trainer_motion() const {
  const MotionAsset* m = level_.motion(trainer_.motion);
  return m ? *m : trainer_.fresh;
}

void Studio::fresh_motion() {
  MotionAsset m;
  m.name = "nod";
  for (i32 n = 2; level_.motion(m.name) || game::is_walker_move(m.name); ++n) m.name = "nod_" + std::to_string(n);
  m.keys.assign(1, MotionKey{});  // the home pose at the start
  trainer_.fresh = std::move(m);
  trainer_.motion.clear();
  touch();
}

bool Studio::edit_motion(const char* label, const std::function<bool(MotionAsset&)>& fn) {
  return edit(label, [&](game::Level& l) {
    MotionAsset* m = l.motion(trainer_.motion);
    if (!m) {  // the new motion enters the level
      if (l.motion(trainer_.fresh.name)) fresh_motion();  // (its name was taken meanwhile)
      l.motions.push_back(trainer_.fresh);
      m = &l.motions.back();
    }
    if (!fn(*m)) return false;
    trainer_.motion = m->name;
    return true;
  });
}

json::Value Studio::trainer_json() const {
  json::Value o = json::Value::object();
  o.set("stage", trainer_.stage == TrainerStage::test ? "test" : "pose");
  const MotionAsset& motion = trainer_motion();
  json::Value m = json::Value::object();
  m.set("name", motion.name);
  m.set("duration", internal::round_num(motion.duration, 1000));
  json::Value keys = json::Value::array();
  for (const MotionKey& k : motion.keys) keys.push(key_json(k));
  m.set("keys", std::move(keys));
  m.set("saved", level_.motion(trainer_.motion) != nullptr);  // in the level (a new one is not, before its first change)
  m.set("trained", motion.onnx != nullptr);
  o.set("motion", std::move(m));
  if (!trainer_.testing.empty()) o.set("testing", trainer_.testing);
  return o;
}

json::Value Studio::motions_json() const {
  json::Value list = json::Value::array();
  for (const MotionAsset& m : level_.motions) {
    json::Value e = json::Value::object();
    e.set("name", m.name);
    e.set("duration", internal::round_num(m.duration, 1000));
    e.set("keys", static_cast<u32>(m.keys.size()));
    e.set("bytes", static_cast<f64>(m.onnx ? m.onnx->size() : 0));  // its network (0: still to be taught)
    list.push(std::move(e));
  }
  return list;
}

json::Value Studio::trainer_command(const std::string& name, const json::Value& c) {
  auto num = [&](const char* k, f64 fallback = 0) { return c[k].is_number() ? c[k].as_number() : fallback; };
  auto str = [&](const char* k) -> std::string { return c[k].is_string() ? c[k].as_string() : std::string(); };
  Trainer& t = trainer_;
  // The answer of an edit that did or did not go through (edit() says why not in the status line).
  auto edited = [&](bool done) { return done ? ok() : error("Stop playing to edit the level"); };

  if (name == "trainer") {
    const bool open = c["open"].is_bool() ? c["open"].as_bool() : !trainer_open();
    if (!open) { close_trainer(); return ok(); }
    return ok(open_trainer());
  }
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

  // ---- the level's motions ----
  if (name == "trainerNew") { fresh_motion(); return ok(); }
  if (name == "trainerSelect") {
    if (!level_.motion(str("name"))) return error("The level has no such motion");
    t.motion = str("name");
    touch();
    return ok();
  }
  if (name == "trainerRemove") {
    const std::string which = str("name");
    if (!level_.motion(which)) return error("The level has no such motion");
    return edited(edit("Remove motion", [&](game::Level& l) {
      l.motions.erase(std::remove_if(l.motions.begin(), l.motions.end(), [&](const MotionAsset& m) { return m.name == which; }), l.motions.end());
      return true;
    }));
  }
  if (name == "trainerLearned") {
    // A trained network from the page's workers: the motion's, in the level. One training run
    // (its `run` token) is one undo step, however often it hands over a newer network.
    const std::string which = str("name");
    std::string bytes;
    game::WalkerPolicy net;
    if (!json::base64_decode(str("onnx"), bytes) ||
        !game::parse_onnx_policy(Span<const u8>(reinterpret_cast<const u8*>(bytes.data()), bytes.size()), net) || net.dims.empty() || net.dims.front() != 61 || net.dims.back() != 14)
      return error("The network is not one the robot runs");
    // Kern's jump is taught from a demonstration, not from keyframes: the level's own "jump".
    const bool jump = which == "jump";
    if (!jump && !level_.motion(which)) return error("The motion \"" + which + "\" is no longer in the level");
    if (!editable()) {  // the level is being played: the page hands the network over again when it stops
      json::Value r = edited(false);
      r.set("later", true);
      return r;
    }
    auto onnx = std::make_shared<const std::string>(std::move(bytes));
    coalesce_ = "motion.net:" + which;
    transaction_ = str("run");
    return edited(edit("Teach motion", [&](game::Level& l) {
      MotionAsset* m = l.motion(which);
      if (!m) {
        l.motions.push_back(MotionAsset{});
        m = &l.motions.back();
        m->name = which;
        m->duration = game::kJumpSeconds;
      }
      m->onnx = onnx;
      return true;
    }));
  }
  if (name == "trainerNet") {
    // A motion's network as a file to download (Base64).
    const MotionAsset* m = level_.motion(str("name"));
    if (!m || !m->onnx) return error("The level has no such network");
    json::Value r = ok();
    r.set("onnx", json::base64_encode(Span<const u8>(reinterpret_cast<const u8*>(m->onnx->data()), m->onnx->size())));
    return r;
  }
  if (name == "trainerImport") {
    // Networks trained elsewhere (the Lehrer's old page kept them on the device): into the level as
    // motions without keyframes. One undo step; a name the level has already is left alone.
    if (!editable()) return edited(false);
    json::Value names = json::Value::array();
    edit("Import learned motions", [&](game::Level& l) {
      for (const json::Value& v : c["motions"].items()) {
        MotionAsset m;
        m.name = v["name"].as_string();
        std::string bytes;
        game::WalkerPolicy net;
        if (!game::motion_name_ok(m.name) || l.motion(m.name) || (game::is_walker_move(m.name) && m.name != "jump") || !json::base64_decode(v["onnx"].as_string(), bytes) ||
            !game::parse_onnx_policy(Span<const u8>(reinterpret_cast<const u8*>(bytes.data()), bytes.size()), net) || net.dims.empty() || net.dims.front() != 61 || net.dims.back() != 14)
          continue;
        m.duration = m.name == "jump" ? game::kJumpSeconds : clamp(static_cast<f32>(v["duration"].as_number(1.2)), kMotionMinSeconds, kMotionMaxSeconds);
        m.onnx = std::make_shared<const std::string>(std::move(bytes));
        names.push(m.name);
        l.motions.push_back(std::move(m));
      }
      return names.size() > 0;
    });
    if (names.size() == 0) return error("Nothing to import: the level has these motions already");
    json::Value r = ok();
    r.set("names", std::move(names));
    return r;
  }

  // ---- the motion being taught ----
  if (name == "trainerMotion") {
    const MotionAsset& now = trainer_motion();
    std::string to = now.name;
    f32 d = now.duration;
    if (c["name"].is_string()) {
      to = str("name");
      if (!game::motion_name_ok(to)) return error("A motion's name is made of a-z, 0-9, _ and -");
      if (to != now.name && game::is_walker_move(to)) return error("\"" + to + "\" is a move of the robot's own");
      if (to != now.name && level_.motion(to)) return error("The level has a motion \"" + to + "\" already");
    }
    if (c["duration"].is_number()) {
      d = static_cast<f32>(internal::round_num(num("duration"), 1000));
      if (!(d >= kMotionMinSeconds && d <= kMotionMaxSeconds)) return error("A motion lasts 0.3 to 15 seconds");
    }
    return edited(edit_motion("Edit motion", [&](MotionAsset& m) {
      // The keyframe at the end stays the end.
      if (m.keys.size() > 1 && std::fabs(m.keys.back().time - m.duration) < kSameTime && d > m.keys[m.keys.size() - 2].time + kSameTime) m.keys.back().time = d;
      m.name = to;
      m.duration = d;
      return true;
    }));
  }
  if (name == "trainerKey") {
    const MotionAsset& now = trainer_motion();
    const f64 i = num("index", -1);
    if (!(i >= 0 && i < static_cast<f64>(now.keys.size())) || i != std::floor(i)) return error("No such keyframe");
    const std::size_t index = static_cast<std::size_t>(i);
    if (c["remove"].as_bool())
      return edited(edit_motion("Remove keyframe", [&](MotionAsset& m) {
        m.keys.erase(m.keys.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
      }));
    const f32 time = static_cast<f32>(internal::round_num(num("time", -1), 1000));
    if (!(time >= 0 && time <= now.duration)) return error("A keyframe lies between 0 and the motion's length");
    for (std::size_t k = 0; k < now.keys.size(); ++k)
      if (k != index && std::fabs(now.keys[k].time - time) < kSameTime) return error("There is a keyframe at " + seconds_text(time) + " already");
    return edited(edit_motion("Move keyframe", [&](MotionAsset& m) {
      m.keys[index].time = time;
      std::stable_sort(m.keys.begin(), m.keys.end(), [](const MotionKey& a, const MotionKey& b) { return a.time < b.time; });
      return true;
    }));
  }
  if (name == "trainerSpread") {
    // The keyframes in equal steps over the motion's length: the first at 0 s, the last at its end.
    const MotionAsset& now = trainer_motion();
    if (now.keys.size() < 2) return error("A motion needs two keyframes at least: its start and its end");
    const std::size_t last = now.keys.size() - 1;
    auto place = [&](std::size_t k) { return static_cast<f32>(internal::round_num(static_cast<f64>(now.duration) * static_cast<f64>(k) / static_cast<f64>(last), 1000)); };
    bool spread = true;
    for (std::size_t k = 0; k <= last; ++k) spread = spread && std::fabs(now.keys[k].time - place(k)) < 1e-6f;
    if (spread) return ok();  // (they are: nothing to undo)
    return edited(edit_motion("Spread keyframes evenly", [&](MotionAsset& m) {
      for (std::size_t k = 0; k <= last; ++k) m.keys[k].time = k == last ? m.duration : place(k);
      return true;
    }));
  }
  if (name == "trainerClear") {
    if (trainer_motion().keys.size() == 1 && trainer_motion().keys[0].time == 0) return ok();  // as a new motion has it
    return edited(edit_motion("Remove all keyframes", [&](MotionAsset& m) {
      m.keys.assign(1, MotionKey{});  // the home pose at the start
      return true;
    }));
  }
  if (name == "trainerTask") {
    // The motion as the Lehrer's task (lehrer/motion.cpp), or why it cannot be taught yet.
    const MotionAsset& m = trainer_motion();
    if (m.keys.size() < 2) return error("A motion needs two keyframes at least: its start and its end");
    if (m.keys.front().time > 1e-4f) return error("The first keyframe must be at 0 s");
    if (std::fabs(m.keys.back().time - m.duration) > kSameTime) return error("The last keyframe must be at the motion's end (" + seconds_text(m.duration) + ")");
    json::Value spec = json::Value::object();
    spec.set("name", m.name);
    spec.set("duration", internal::round_num(m.duration, 1000));
    json::Value frames = json::Value::array();
    for (const MotionKey& k : m.keys) frames.push(key_json(k));
    spec.set("frames", std::move(frames));
    json::Value r = ok();
    r.set("task", "motion:" + json::stringify(spec));
    r.set("name", m.name);
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
    const MotionAsset* taught = level_.motion(move);
    if (!(taught && taught->onnx) && !game::is_walker_move(move)) return error("no such move");
    ActorId duck = 0;
    const game::Level stage = test_stage(duck, level_.motions);
    if (!start_stage(TrainerStage::test, stage, duck)) return error("The Lehrer's stage could not start");
    const std::string why = world_->walker_skill(duck, move);
    if (!why.empty()) return error(why);
    t.testing = move;
    touch();
    return ok();
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
    const MotionAsset& now = trainer_motion();
    const f32 time = static_cast<f32>(internal::round_num(num("time", -1), 1000));
    if (!(time >= 0 && time <= now.duration)) return error("A keyframe lies between 0 and the motion's length");
    MotionKey key;
    key.time = time;
    for (i32 j = 0; j < 14; ++j) {
      key.joints[j] = static_cast<f32>(internal::round_num(probe.joint[j]));
      if (!std::isfinite(key.joints[j]) || std::fabs(key.joints[j]) > kMotionMaxAngle) return error("The pose is outside what the servos reach");
    }
    const bool again = std::any_of(now.keys.begin(), now.keys.end(), [&](const MotionKey& k) { return std::fabs(k.time - time) < kSameTime; });
    if (!again && now.keys.size() >= kMotionMaxKeys) return error("A motion has 32 keyframes at most");
    return edited(edit_motion("Add keyframe", [&](MotionAsset& m) {
      auto at = std::find_if(m.keys.begin(), m.keys.end(), [&](const MotionKey& k) { return std::fabs(k.time - time) < kSameTime; });
      if (at != m.keys.end()) *at = key;  // a pose taken again at the same time replaces the old one
      else m.keys.insert(std::upper_bound(m.keys.begin(), m.keys.end(), key, [](const MotionKey& a, const MotionKey& b) { return a.time < b.time; }), key);
      return true;
    }));
  }
  if (name == "trainerShow") {
    // The servos take the puppet to a keyframe, as the robot would reach it.
    const MotionAsset& m = trainer_motion();
    const f64 i = num("index", -1);
    if (!(i >= 0 && i < static_cast<f64>(m.keys.size())) || i != std::floor(i)) return error("No such keyframe");
    return ok(world_->set_walker_pose(t.duck, m.keys[static_cast<std::size_t>(i)].joints));
  }
  return error("unknown command " + name);
}

}  // namespace kern::studio
