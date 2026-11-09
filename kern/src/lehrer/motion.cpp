// Keyframed joint motion for Kern Lehrer. A pose is a 14-element array of servo
// offsets from the Micro Duck's home angles, not a scene-space animation.
// The same 50 Hz Box3D robot runs the reference and the learned ONNX policy.
#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include "kern/core/json.h"
#include "kern/core/dmath.h"
#include "kern/game/walker.h"
#include "kern/lehrer/env.h"

namespace kern::lehrer {
namespace {
struct Frame {
  f32 time = 0;
  std::array<f32, kActions> joints{};
};

class MotionTask final : public Task {
 public:
  MotionTask(std::vector<Frame> frames, f32 duration)
      : frames_(std::move(frames)), duration_(duration) {
    noise.push = 0.12f;
  }
  std::string name() const override { return "motion"; }
  f32 episode_seconds() const override { return duration_; }
  Start reset(Rng& rng) override {
    Start s;
    s.yaw = rng.uniform(-10.0f, 10.0f);
    s.roll = rng.uniform(-1.0f, 1.0f);
    s.pitch = rng.uniform(-1.0f, 1.0f);
    return s;
  }
  void command(f32 o[kObs], i32 step) override {
    for (i32 i = 48; i < kObs; ++i) o[i] = 0;
    const f32 phase = 6.28318530718f * clamp(step * kControlDt / duration_, 0.0f, 1.0f);
    o[game::kJumpPhaseSlot] = kern::dm::cos(phase);
    o[game::kJumpPhaseSlot + 1] = kern::dm::sin(phase);
  }
  bool reference_action(i32 step, f32 out[kActions]) const override {
    const f32 t = clamp(step * kControlDt, 0.0f, duration_);
    auto b = frames_.begin() + 1;
    while (b != frames_.end() && b->time < t) ++b;
    if (b == frames_.end()) b = frames_.end() - 1;
    const Frame& before = *(b - 1);
    const f32 span = b->time - before.time;
    const f32 alpha = span > 0 ? clamp((t - before.time) / span, 0.0f, 1.0f) : 0.0f;
    const f32 smooth = alpha * alpha * (3 - 2 * alpha);
    for (i32 i = 0; i < kActions; ++i) out[i] = before.joints[i] + (b->joints[i] - before.joints[i]) * smooth;
    return true;
  }
  f32 reward(const Tick& tick, bool& terminal) override {
    f32 target[kActions];
    reference_action(tick.step, target);
    f32 pose_error = 0, action_error = 0, delta = 0;
    for (i32 j = 0; j < kActions; ++j) {
      const f32 d = tick.probe.joint[j] - target[j];
      pose_error += d * d;
      const f32 a = tick.action[j] - target[j];
      action_error += a * a;
      const f32 v = tick.action[j] - tick.previous_action[j];
      delta += v * v;
    }
    const f32 upright = kern::dm::exp(-(tick.probe.gravity.x * tick.probe.gravity.x +
                                       tick.probe.gravity.y * tick.probe.gravity.y) / 0.1f);
    const f32 reward = kControlDt * (12.0f * kern::dm::exp(-pose_error / 14 / 0.10f) +
                                   4.0f * kern::dm::exp(-action_error / 14 / 0.12f) +
                                   2.0f * upright - 0.03f * delta);
    terminal = tick.probe.gravity.z > -0.5f || tick.probe.position.z < 0.04f;
    terms.clear();
    terms.push_back({"motion", reward});
    if (terminal) terms.push_back({"fall", -10});
    return reward - (terminal ? 10.0f : 0.0f);
  }
 private:
  std::vector<Frame> frames_;
  f32 duration_;
};
}  // namespace

std::unique_ptr<Task> make_motion_task(const std::string& json_text) {
  if (json_text.size() > 16384) return nullptr;
  json::Value root;
  if (!json::parse(json_text, root) || !root.is_object()) return nullptr;
  if (!root["duration"].is_number() || !root["frames"].is_array()) return nullptr;
  const f64 length = root["duration"].as_number();
  if (!std::isfinite(length) || length < 0.3 || length > 15) return nullptr;
  const auto& entries = root["frames"].items();
  if (entries.size() < 2 || entries.size() > 32) return nullptr;
  std::vector<Frame> frames;
  for (const json::Value& v : entries) {
    if (!v.is_object() || !v["time"].is_number() || !v["joints"].is_array() ||
        v["joints"].items().size() != kActions) return nullptr;
    const f64 time = v["time"].as_number();
    if (!std::isfinite(time) || time < 0 || time > length + 1e-4 ||
        (!frames.empty() && time <= frames.back().time)) return nullptr;
    Frame f;
    f.time = static_cast<f32>(time);
    for (i32 i = 0; i < kActions; ++i) {
      const json::Value& val = v["joints"].items()[static_cast<std::size_t>(i)];
      if (!val.is_number()) return nullptr;
      const f64 a = val.as_number();
      if (!std::isfinite(a) || std::fabs(a) > 2.5) return nullptr;
      f.joints[static_cast<std::size_t>(i)] = static_cast<f32>(a);
    }
    frames.push_back(f);
  }
  if (frames.front().time > 1e-4 || std::fabs(frames.back().time - length) > 1e-3) return nullptr;
  return std::make_unique<MotionTask>(std::move(frames), static_cast<f32>(length));
}

}  // namespace kern::lehrer
