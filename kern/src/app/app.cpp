// The application framework's own objects: Input bookkeeping and the Engine's host calls (status, pointer lock,
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

void Input::end_frame() {
  for (u8& s : keys_) s &= kDown;
  pressed_buttons_ = 0;
  pointer_delta_ = {};
  wheel_ = 0;
  for (Touch& t : touches_) t.previous = t.position;
}

}  // namespace kern

#if defined(__wasm__)
#define KERN_ENV(name) __attribute__((import_module("kern_env"), import_name(#name)))
extern "C" {
KERN_ENV(status) void kern_env_status(const char* text, kern::u32 length);
KERN_ENV(pointer_lock) void kern_env_pointer_lock(kern::i32 lock);
}
#else
static void kern_env_status(const char* text, kern::u32 length) { std::printf("[status] %.*s\n", static_cast<int>(length), text); }
static void kern_env_pointer_lock(kern::i32) {}
#endif

namespace kern {

void Engine::set_status(const char* text) {
  static char last[512] = {};
  if (std::strncmp(last, text, sizeof last - 1) == 0) return;
  std::snprintf(last, sizeof last, "%s", text);
  kern_env_status(text, static_cast<u32>(std::strlen(text)));
}

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

