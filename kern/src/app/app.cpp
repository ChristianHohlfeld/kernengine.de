// The application framework's own objects: Input bookkeeping and the Engine's host calls (pointer lock,
// launch parameters). The app runtime - the App's start, frames and stop, and the C ABI the platform layer
// (kern/web/app.mjs) drives - is kern/src/app/runtime.cpp.
#include "kern/app/app.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>


namespace kern {

void Input::key(Key k, bool is_down) {
  if (static_cast<u32>(k) >= static_cast<u32>(Key::count)) return;  // (no such key: nothing to keep)
  u8& s = keys_[static_cast<u32>(k)];
  if (is_down && !(s & kDown)) s |= kDown | kPressed;
  if (!is_down && (s & kDown)) { s &= ~kDown; s |= kReleased; }
}

void Input::pointer_move(Vec2 position, Vec2 delta) {
  pointer_ = position;
  pointer_delta_ += delta;
}

void Input::pointer_buttons(u32 buttons) {
  pressed_buttons_ |= buttons & ~buttons_;
  buttons_ = buttons;
}

void Input::touch(i32 id, i32 phase, Vec2 position) {
  Touch* slot = nullptr;
  for (Touch& t : touches_)
    if (t.id == id) { slot = &t; break; }
  if (phase == 0) {
    if (!slot)
      for (Touch& t : touches_)
        if (t.id < 0) { slot = &t; break; }
    if (!slot) return;
    slot->id = id;
    slot->start = slot->position = slot->previous = position;
    slot->began = true;
  } else if (slot) {
    slot->position = position;
    if (phase == 2) slot->id = -1;
  }
}

u32 Input::touch_count() const {
  u32 n = 0;
  for (const Touch& t : touches_) n += t.id >= 0;
  return n;
}

void Input::set_focus(bool focused) {
  focused_ = focused;
  if (!focused) {
    // Releasing everything on blur prevents stuck keys after alt-tab.
    for (u8& s : keys_) s = (s & kDown) ? kReleased : 0;
    buttons_ = 0;
  }
}

void Input::set_gamepad(const Gamepad& pad) {
  const Gamepad p = pad.connected ? pad : Gamepad{};
  pad_pressed_ |= p.buttons & ~pad_.buttons;
  pad_ = p;
}

void Input::end_frame() {
  pad_pressed_ = 0;
  for (u8& s : keys_) s &= kDown;
  pressed_buttons_ = 0;
  pointer_delta_ = {};
  wheel_ = 0;
  for (Touch& t : touches_) { t.previous = t.position; t.began = false; }
}

// ---- input actions (kern/docs/app.md 4.) ----

namespace {
bool key_held(const Input& in, const std::vector<Key>& keys) {
  for (Key k : keys)
    if (in.down(k)) return true;
  return false;
}
}  // namespace

const ActionBinding* Engine::find_action(const char* name) const {
  if (const ActionBinding* a = actions_.action(name ? name : "")) return a;
  std::string names;
  for (const ActionBinding& a : actions_.actions) names += (names.empty() ? "" : ", ") + a.name;
  fail("the project has no action \"%s\" (its actions: %s)", name ? name : "", names.empty() ? "none" : names.c_str());
  for (const std::string& n : unknown_actions_)
    if (n == (name ? name : "")) return nullptr;
  unknown_actions_.push_back(name ? name : "");
  log(LogLevel::error, "%s", last_error());
  return nullptr;
}

const AxisBinding* Engine::find_axis(const char* name) const {
  if (const AxisBinding* a = actions_.axis(name ? name : "")) return a;
  std::string names;
  for (const AxisBinding& a : actions_.axes) names += (names.empty() ? "" : ", ") + a.name;
  fail("the project has no axis \"%s\" (its axes: %s)", name ? name : "", names.empty() ? "none" : names.c_str());
  for (const std::string& n : unknown_actions_)
    if (n == (name ? name : "")) return nullptr;
  unknown_actions_.push_back(name ? name : "");
  log(LogLevel::error, "%s", last_error());
  return nullptr;
}

bool Engine::touch_in(TouchZone zone, bool pressed) const {
  if (zone == TouchZone::none) return false;
  const f32 half = static_cast<f32>(width()) * 0.5f;
  for (u32 i = 0; i < kMaxTouches; ++i) {
    const Touch& t = input_.touches()[i];
    if (t.id < 0 || (pressed && !t.began)) continue;
    if (zone == TouchZone::anywhere || (zone == TouchZone::left) == (t.start.x < half)) return true;
  }
  return false;
}

bool Engine::action(const char* name) const {
  const ActionBinding* a = find_action(name);
  if (!a) return false;
  if (key_held(input_, a->keys)) return true;
  for (PadButton b : a->pad)
    if (input_.pad_down(b)) return true;
  for (u32 b : a->mouse)
    if (input_.button(b)) return true;
  return touch_in(a->touch, false);
}

bool Engine::action_pressed(const char* name) const {
  const ActionBinding* a = find_action(name);
  if (!a) return false;
  for (Key k : a->keys)
    if (input_.pressed(k)) return true;
  for (PadButton b : a->pad)
    if (input_.pad_pressed(b)) return true;
  for (u32 b : a->mouse)
    if (input_.button_pressed(b)) return true;
  return touch_in(a->touch, true);
}

f32 Engine::axis(const char* name) const {
  const AxisBinding* a = find_axis(name);
  if (!a) return 0;
  f32 best = static_cast<f32>(key_held(input_, a->positive)) - static_cast<f32>(key_held(input_, a->negative));
  auto take = [&best](f32 v) { if (std::fabs(v) > std::fabs(best)) best = v; };
  const Gamepad& g = input_.gamepad();
  if (a->pad != PadAxis::none && g.connected) {
    const f32 raw = a->pad == PadAxis::left_x ? g.lx : a->pad == PadAxis::left_y ? g.ly : a->pad == PadAxis::right_x ? g.rx : a->pad == PadAxis::right_y ? g.ry : g.rt - g.lt;
    const f32 m = std::fabs(raw);
    if (m > a->deadzone) take(std::copysign(std::fmin(1.0f, (m - a->deadzone) / (1.0f - a->deadzone)), raw));
  }
  if (a->touch != TouchAxis::none) {
    // the touch stick: a finger that began on the left half; one radius (an eighth of the shorter side) is 1
    const f32 radius = static_cast<f32>(max_of(1, min_of(width(), height()))) * kTouchStickShare;
    for (u32 i = 0; i < kMaxTouches; ++i) {
      const Touch& t = input_.touches()[i];
      if (t.id < 0 || t.start.x >= static_cast<f32>(width()) * 0.5f) continue;
      const f32 d = a->touch == TouchAxis::stick_x ? t.position.x - t.start.x : t.start.y - t.position.y;
      take(clamp(d / radius, -1.0f, 1.0f));
      break;
    }
  }
  return clamp(best, -1.0f, 1.0f);
}

}  // namespace kern

#if defined(__wasm__)
#define KERN_ENV(name) __attribute__((import_module("kern_env"), import_name(#name)))
extern "C" {
KERN_ENV(pointer_lock) void kern_env_pointer_lock(kern::i32 lock);
}
#else
static void kern_env_pointer_lock(kern::i32) {}
#endif

namespace kern {

void Engine::request_pointer_lock(bool lock) { kern_env_pointer_lock(lock ? 1 : 0); }

namespace {
std::string url_decode(const char* s, std::size_t n) {
  std::string out;
  out.reserve(n);
  auto hex = [](char c) { return c >= '0' && c <= '9' ? c - '0' : (c | 32) >= 'a' && (c | 32) <= 'f' ? (c | 32) - 'a' + 10 : -1; };
  for (std::size_t i = 0; i < n; ++i) {
    if (s[i] == '+') out += ' ';
    else if (s[i] == '%' && i + 2 < n && hex(s[i + 1]) >= 0 && hex(s[i + 2]) >= 0) { out += static_cast<char>(hex(s[i + 1]) * 16 + hex(s[i + 2])); i += 2; }
    else out += s[i];
  }
  return out;
}
}  // namespace

void Engine::set_args(const char* text, u32 length) {
  args_.clear();
  std::size_t start = 0;
  if (length && text[0] == '?') start = 1;
  while (start < length) {
    std::size_t end = start;
    while (end < length && text[end] != '&') ++end;
    std::size_t eq = start;
    while (eq < end && text[eq] != '=') ++eq;
    if (eq > start) args_.emplace_back(url_decode(text + start, eq - start), eq < end ? url_decode(text + eq + 1, end - eq - 1) : std::string());
    start = end + 1;
  }
}

const char* Engine::arg(const char* key, const char* fallback) const {
  for (const auto& kv : args_)
    if (kv.first == key) return kv.second.c_str();
  return fallback;
}

bool Engine::has_arg(const char* key) const {
  for (const auto& kv : args_)
    if (kv.first == key) return true;
  return false;
}

f64 Engine::arg_number(const char* key, f64 fallback) const {
  const char* v = arg(key);
  if (!v || !*v) return fallback;
  char* end = nullptr;
  const f64 x = std::strtod(v, &end);
  return end && *end == 0 && std::isfinite(x) ? x : fallback;
}

}  // namespace kern

