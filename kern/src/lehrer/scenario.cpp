// Declarative Lehrer scenarios: the same C++ Task runs in the native trainer and the browser.
// Geometry is rebuilt in Box3D every episode; observations are limited to the robot's 61 inputs.
// Pollen's sim2real PPO/DR recipe remains unchanged. No scripts, eval or arbitrary native execution.
#include "kern/lehrer/env.h"

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "kern/core/json.h"

namespace kern::lehrer {
namespace {

constexpr f32 kScale = game::kWalkerScale;
constexpr f32 kDeg = 0.017453292519943295f;

struct Waypoint { f32 x = 0, y = 0, radius = 0.15f; };
struct Object {
  std::string kind = "box";
  f32 x = 0, y = 0, width = 0.15f, depth = 0.15f, height = 0.15f;
  f32 radius = 0.08f, friction = 0.7f, mass = 0.05f;
  bool dynamic = false;
};

struct Weights {
  f32 progress = 12, waypoint = 8, finish = 20, upright = 2;
  f32 action_rate = -0.1f, fall = -12, contact = -2;
  f32 height = 0, speed = 0, pose = 0;
};

// Closed numeric ranges stop accidental extreme rewards, pathological geometry and non-finite data.
bool field(const json::Value& v, const char* key, f32 def, f32 lo, f32 hi, f32& out) {
  out = def;
  if (!v.has(key)) return true;
  const json::Value& n = v[key];
  if (!n.is_number() || !std::isfinite(n.as_number()) || n.as_number() < lo || n.as_number() > hi) return false;
  out = static_cast<f32>(n.as_number());
  return true;
}

bool flag(const json::Value& v, const char* key, bool def, bool& out) {
  out = def;
  if (!v.has(key)) return true;
  if (!v[key].is_bool()) return false;
  out = v[key].as_bool();
  return true;
}

class ScenarioTask final : public Task {
 public:
  ScenarioTask(std::string label, f32 duration, f32 yaw, f32 spread, f32 forward,
               f32 lateral, f32 turn, f32 desired_height, f32 desired_speed,
               bool end_on_contact, Weights weights, std::vector<Waypoint> points,
               std::vector<Object> objects, std::vector<f32> pose, f32 push)
      : label_(std::move(label)), duration_(duration), yaw_(yaw), spread_(spread),
        forward_(forward), lateral_(lateral), turn_(turn), desired_height_(desired_height),
        desired_speed_(desired_speed), end_on_contact_(end_on_contact),
        weights_(weights), points_(std::move(points)), objects_(std::move(objects)),
        pose_(std::move(pose)) {
    noise.push = push;
  }

  std::string name() const override { return label_; }
  f32 episode_seconds() const override { return duration_; }
  bool succeeded() const override { return completed_; }

  Start reset(Rng& rng) override {
    completed_ = false;
    stage_ = 0;
    measured_ = false;
    Start s;
    s.yaw = yaw_ + rng.uniform(-spread_, spread_);
    s.roll = rng.uniform(-2.0f, 2.0f);
    s.pitch = rng.uniform(-2.0f, 2.0f);
    s.foot_friction = rng.uniform(0.8f, 1.2f);
    return s;
  }

  void scene(game::Level& level, Rng&) override {
    for (const Object& o : objects_) {
      game::Actor a;
      a.name = "Training " + o.kind;
      a.transform.position = {o.x * kScale, o.y * kScale,
                              (o.kind == "sphere" ? o.radius : o.height * 0.5f) * kScale};
      game::BodyComponent body;
      body.motion = o.dynamic ? game::Motion::dynamic : game::Motion::fixed;
      body.mass = o.dynamic ? o.mass * kScale * kScale * kScale : 0;
      a.body = body;
      game::ColliderComponent c;
      c.shape = o.kind == "sphere" ? game::ColliderShape::sphere : game::ColliderShape::box;
      c.size = {o.width * kScale, o.depth * kScale, o.height * kScale};
      c.radius = o.radius * kScale;
      c.friction = o.friction;
      a.colliders.push_back(c);
      level.add(std::move(a));
    }
  }

  void command(f32 o[kObs], i32) override {
    for (i32 i = 48; i < kObs; ++i) o[i] = 0;
    o[48] = forward_;  // Pollen velocity command, real m/s
    o[49] = lateral_;
    o[50] = turn_;     // rad/s
  }

  void augment_observation(f32 o[kObs], const game::GameWorld::WalkerProbe& p) override {
    // Slots 55 and 56 are unused by velstand. A navigational command source supplies the next
    // waypoint in the robot's local coordinates. On real hardware this input must be supplied
    // by an external localisation/goal provider; it is NOT an onboard pose sensor.
    if (stage_ >= points_.size()) return;
    const Waypoint& w = points_[stage_];
    const f32 dx = w.x - p.position.x, dy = w.y - p.position.y;
    const f32 yaw = game::quat_to_euler(p.rotation).z * kDeg;
    const f32 c = kern::dm::cos(yaw), s = kern::dm::sin(yaw);
    o[55] = clamp(c * dx + s * dy, -5.0f, 5.0f);
    o[56] = clamp(-s * dx + c * dy, -5.0f, 5.0f);
  }

  f32 reward(const Tick& t, bool& terminal) override {
    terms.clear();
    f32 sum = 0;
    auto add = [&](const char* key, f32 weight, f32 value, bool tick = true) {
      const f32 r = weight * value * (tick ? kControlDt : 1.0f);
      terms.push_back({key, r});
      sum += r;
    };
    const auto& p = t.probe;
    const f32 inclination = p.gravity.x * p.gravity.x + p.gravity.y * p.gravity.y;
    add("upright", weights_.upright, kern::dm::exp(-inclination / 0.05f));
    const f32 dz = p.position.z - desired_height_;
    add("height", weights_.height, kern::dm::exp(-dz * dz / (0.035f * 0.035f)));
    const f32 speed = std::sqrt(p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y);
    const f32 vs = speed - desired_speed_;
    add("speed", weights_.speed, kern::dm::exp(-vs * vs / (0.15f * 0.15f)));
    f32 pose_error = 0, action_change = 0;
    for (i32 i = 0; i < kActions; ++i) {
      const f32 d = p.joint[i] - pose_[static_cast<std::size_t>(i)];
      pose_error += d * d;
      const f32 a = t.action[i] - t.previous_action[i];
      action_change += a * a;
    }
    add("pose", weights_.pose, kern::dm::exp(-pose_error / (kActions * 0.3f * 0.3f)));
    add("action_rate", weights_.action_rate, action_change);
    if (p.body_contact) add("contact", weights_.contact, 1.0f);

    if (stage_ < points_.size()) {
      const Waypoint& w = points_[stage_];
      const f32 dx = p.position.x - w.x, dy = p.position.y - w.y;
      const f32 distance = std::sqrt(dx * dx + dy * dy);
      if (!measured_) { last_distance_ = distance; measured_ = true; }
      add("progress", weights_.progress, clamp(last_distance_ - distance, -0.05f, 0.05f), false);
      last_distance_ = distance;
      if (distance <= w.radius) {
        add("waypoint", weights_.waypoint, 1.0f, false);
        ++stage_;
        measured_ = false;  // never reward the distance jump to the next waypoint
        if (stage_ == points_.size()) {
          completed_ = true;
          add("finish", weights_.finish, 1.0f, false);
        }
      }
    }

    const bool fell = p.gravity.z > -0.5f || p.position.z < 0.05f;
    terminal = completed_ || fell || (end_on_contact_ && p.body_contact);
    if (fell) add("fall", weights_.fall, 1.0f, false);
    return sum;
  }

 private:
  std::string label_;
  f32 duration_, yaw_, spread_, forward_, lateral_, turn_, desired_height_, desired_speed_;
  bool end_on_contact_ = false;
  Weights weights_;
  std::vector<Waypoint> points_;
  std::vector<Object> objects_;
  std::vector<f32> pose_;
  std::size_t stage_ = 0;
  f32 last_distance_ = 0;
  bool measured_ = false, completed_ = false;
};

}  // namespace

std::unique_ptr<Task> make_scenario_task(const std::string& spec) {
  if (spec.empty() || spec.size() > 16384) return nullptr;
  json::Value cfg;
  if (!json::parse(spec, cfg) || !cfg.is_object()) return nullptr;
  auto isobj = [](const json::Value& v) { return v.is_null() || v.is_object(); };
  if (!isobj(cfg["start"]) || !isobj(cfg["commands"]) || !isobj(cfg["rewards"])) return nullptr;
  const json::Value& start = cfg["start"];
  const json::Value& cmd = cfg["commands"];
  const json::Value& rw = cfg["rewards"];
  f32 duration, yaw, spread, forward, lateral, turn, height, speed, push;
  bool on_contact;
  if (!field(cfg, "duration", 6, 0.5f, 60, duration) ||
      !field(start, "yaw", 0, -180, 180, yaw) ||
      !field(start, "yawSpread", 0, 0, 30, spread) ||
      !field(cmd, "forward", 0, -0.4f, 0.4f, forward) ||
      !field(cmd, "lateral", 0, -0.3f, 0.3f, lateral) ||
      !field(cmd, "turn", 0, -1, 1, turn) ||
      !field(cfg, "targetHeight", 0.115f, 0.04f, 0.3f, height) ||
      !field(cfg, "targetSpeed", 0, 0, 1.0f, speed) ||
      !field(cfg, "push", 0.3f, 0, 0.6f, push) ||
      !flag(cfg, "endOnContact", false, on_contact)) return nullptr;
  Weights w;
  if (!field(rw, "progress", w.progress, -100, 100, w.progress) ||
      !field(rw, "waypoint", w.waypoint, -100, 100, w.waypoint) ||
      !field(rw, "finish", w.finish, -100, 100, w.finish) ||
      !field(rw, "upright", w.upright, -100, 100, w.upright) ||
      !field(rw, "actionRate", w.action_rate, -100, 100, w.action_rate) ||
      !field(rw, "fall", w.fall, -100, 100, w.fall) ||
      !field(rw, "contact", w.contact, -100, 100, w.contact) ||
      !field(rw, "height", w.height, -100, 100, w.height) ||
      !field(rw, "speed", w.speed, -100, 100, w.speed) ||
      !field(rw, "pose", w.pose, -100, 100, w.pose)) return nullptr;

  std::vector<Waypoint> points;
  if (!cfg["waypoints"].is_null()) {
    if (!cfg["waypoints"].is_array() || cfg["waypoints"].size() > 16) return nullptr;
    for (const json::Value& p : cfg["waypoints"].items()) {
      if (!p.is_object() || !p.has("x") || !p.has("y")) return nullptr;
      Waypoint q;
      if (!field(p, "x", 0, -5, 5, q.x) || !field(p, "y", 0, -5, 5, q.y) ||
          !field(p, "radius", 0.15f, 0.03f, 1.0f, q.radius)) return nullptr;
      points.push_back(q);
    }
  }
  std::vector<Object> objects;
  if (!cfg["objects"].is_null()) {
    if (!cfg["objects"].is_array() || cfg["objects"].size() > 32) return nullptr;
    for (const json::Value& v : cfg["objects"].items()) {
      if (!v.is_object() || !v["kind"].is_string() || !v.has("x") || !v.has("y")) return nullptr;
      Object o;
      o.kind = v["kind"].as_string();
      if (o.kind != "box" && o.kind != "sphere") return nullptr;
      if (!field(v, "x", 0, -5, 5, o.x) || !field(v, "y", 0, -5, 5, o.y) ||
          !field(v, "width", 0.15f, 0.02f, 2, o.width) ||
          !field(v, "depth", 0.15f, 0.02f, 2, o.depth) ||
          !field(v, "height", 0.15f, 0.02f, 2, o.height) ||
          !field(v, "radius", 0.08f, 0.01f, 1, o.radius) ||
          !field(v, "mass", 0.05f, 0.001f, 5, o.mass) ||
          !field(v, "friction", 0.7f, 0.01f, 3, o.friction) ||
          !flag(v, "dynamic", false, o.dynamic)) return nullptr;
      objects.push_back(o);
    }
  }
  std::vector<f32> pose(kActions, 0.0f);
  if (!cfg["pose"].is_null()) {
    if (!cfg["pose"].is_array() || cfg["pose"].size() != kActions) return nullptr;
    for (i32 i = 0; i < kActions; ++i) {
      const json::Value& v = cfg["pose"][static_cast<std::size_t>(i)];
      if (!v.is_number() || !std::isfinite(v.as_number()) ||
          v.as_number() < -2 || v.as_number() > 2) return nullptr;
      pose[static_cast<std::size_t>(i)] = static_cast<f32>(v.as_number());
    }
  }
  if (points.empty() && w.pose == 0 && w.height == 0 && w.speed == 0 && w.upright == 0) return nullptr;
  if (cfg.has("name") && (!cfg["name"].is_string() || cfg["name"].as_string().size() > 80)) return nullptr;
  return std::make_unique<ScenarioTask>(
      cfg["name"].is_string() ? cfg["name"].as_string() : "scenario",
      duration, yaw, spread, forward, lateral, turn, height, speed, on_contact, w,
      std::move(points), std::move(objects), std::move(pose), push);
}

}  // namespace kern::lehrer
