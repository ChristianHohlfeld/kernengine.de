// Vehicle touch pad (see touch_pad.h).
#include "touch_pad.h"

#include <algorithm>
#include <cstdio>

namespace sc {

namespace {
std::string esc(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (c == '"' || c == '\\') o += '\\';
    o += c;
  }
  return o;
}
std::string color_hex(u32 c) {
  char b[8];
  std::snprintf(b, sizeof b, "#%06x", c & 0xffffff);
  return b;
}
}  // namespace

void TouchPad::set_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb) {
  w_ = w; h_ = h; sl_ = sl; st_ = st; sr_ = sr; sb_ = sb;
  size_ = clampf(std::fmin(w, h) * 0.42f, 120, 168);
  layout();
}

void TouchPad::set_layout(bool sticks, const std::vector<PadButton>& buttons, const char* left_label, const char* right_label) {
  bool same = sticks == sticks_ && buttons.size() == wanted_.size() && left_label_ == left_label && right_label_ == right_label;
  for (std::size_t i = 0; same && i < buttons.size(); ++i) same = buttons[i].id == wanted_[i].id && buttons[i].label == wanted_[i].label && buttons[i].on == wanted_[i].on;
  if (same) return;
  const bool sticks_changed = sticks != sticks_;
  sticks_ = sticks;
  wanted_ = buttons;
  left_label_ = left_label;
  right_label_ = right_label;
  if (sticks_changed) release_all();
  layout();
}

void TouchPad::layout() {
  std::vector<Btn> old = std::move(buttons_);
  buttons_.clear();
  // Buttons stack above the right stick's corner (SpawnCity: context buttons over the right
  // thumb; mode buttons over the left one), 58 px round targets.
  const f32 b = 58, gap = 10;
  const f32 rx = w_ - sr_ - 18 - size_, ry = h_ - sb_ - 18 - size_;
  const f32 lx = sl_ + 18, ly = h_ - sb_ - 18 - size_;
  int left_i = 0, right_i = 0;
  for (const PadButton& p : wanted_) {
    Btn btn{p, 0, 0, b, b};
    const bool left_side = p.id == "mode" || p.id == "drone" || p.id == "jet";
    if (left_side) {
      btn.x = lx + (left_i % 2) * (b + gap);
      btn.y = ly - (b + gap) * (1 + left_i / 2);
      ++left_i;
    } else {
      btn.x = rx + size_ - b - (right_i % 2) * (b + gap);
      btn.y = ry - (b + gap) * (1 + right_i / 2);
      ++right_i;
    }
    for (const Btn& o : old)
      if (o.b.id == p.id) btn.pressed = o.pressed;
    buttons_.push_back(btn);
  }
}

bool TouchPad::pointer(u32 type, i32 id, f32 x, f32 y) {
  auto it = std::find_if(fingers_.begin(), fingers_.end(), [&](const Finger& f) { return f.id == id; });
  if (type == 0) {
    for (Btn& btn : buttons_)
      if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) {
        btn.pressed = true;
        pressed_.push_back(btn.b.id);
        fingers_.push_back({id, 2, btn.b.id, x, y});
        return true;
      }
    if (!sticks_) return false;
    const f32 ly = h_ - sb_ - 18 - size_, lx = sl_ + 18, rx = w_ - sr_ - 18 - size_;
    // Generous catch areas: each half of the lower screen belongs to its stick.
    if (y > ly - size_ * 0.6f) {
      const bool left = x < w_ * 0.5f;
      fingers_.push_back({id, left ? 0 : 1, "", left ? lx + size_ / 2 : rx + size_ / 2, ly + size_ / 2});
      return pointer(1, id, x, y);
    }
    return false;
  }
  if (it == fingers_.end()) return false;
  if (type == 1) {
    if (it->kind < 2) {
      const Vec2 d{(x - it->x0) / (size_ * 0.5f), -(y - it->y0) / (size_ * 0.5f)};
      const f32 l = std::sqrt(d.x * d.x + d.y * d.y);
      const Vec2 v = l > 1 ? d * (1.0f / l) : d;
      (it->kind == 0 ? left_ : right_) = v;
    }
    return true;
  }
  // up / cancel
  if (it->kind == 0) left_ = {};
  else if (it->kind == 1) right_ = {};
  else
    for (Btn& btn : buttons_)
      if (btn.b.id == it->button) btn.pressed = false;
  fingers_.erase(it);
  return true;
}

void TouchPad::release_all() {
  fingers_.clear();
  left_ = right_ = {};
  for (Btn& b : buttons_) b.pressed = false;
}

bool TouchPad::held(const std::string& id) const {
  for (const Btn& b : buttons_)
    if (b.b.id == id) return b.pressed;
  return false;
}

bool TouchPad::take_pressed(const std::string& id) {
  auto it = std::find(pressed_.begin(), pressed_.end(), id);
  if (it == pressed_.end()) return false;
  pressed_.erase(it);
  return true;
}

std::string TouchPad::view_json(const kern::game::TouchView* base) const {
  char buf[256];
  auto stick = [&](f32 x, f32 y, f32 size, Vec2 knob, bool live, bool accent, const std::string& label) {
    std::snprintf(buf, sizeof buf, "{\"x\":%.1f,\"y\":%.1f,\"size\":%.1f,\"kx\":%.3f,\"ky\":%.3f,\"live\":%s,\"accent\":%s,\"label\":\"%s\"}", x, y, size, knob.x,
                  knob.y, live ? "true" : "false", accent ? "true" : "false", esc(label).c_str());
    return std::string(buf);
  };
  auto button = [&](const std::string& id, const std::string& label, f32 x, f32 y, f32 w, f32 h, bool round, bool pressed, bool on, u32 color) {
    std::string s = "{\"id\":\"" + esc(id) + "\",\"label\":\"" + esc(label) + "\"";
    std::snprintf(buf, sizeof buf, ",\"x\":%.1f,\"y\":%.1f,\"w\":%.1f,\"h\":%.1f,\"round\":%s,\"pressed\":%s,\"on\":%s,\"color\":\"%s\"}", x, y, w, h,
                  round ? "true" : "false", pressed ? "true" : "false", on ? "true" : "false", color_hex(color).c_str());
    return s + buf;
  };
  std::string s = "{";
  bool any_finger_left = false, any_finger_right = false;
  for (const Finger& f : fingers_) { any_finger_left |= f.kind == 0; any_finger_right |= f.kind == 1; }
  if (sticks_ || !base) {
    const f32 ly = h_ - sb_ - 18 - size_;
    s += "\"move\":" + stick(sl_ + 18, ly, size_, {left_.x, -left_.y}, any_finger_left, false, left_label_);
    s += ",\"look\":" + stick(w_ - sr_ - 18 - size_, ly, size_, {right_.x, -right_.y}, any_finger_right, true, right_label_);
  } else {
    s += "\"move\":" + stick(base->move.x, base->move.y, base->move.size, {base->move.knob_x, base->move.knob_y}, base->move.live, base->move.accent, base->move.label);
    s += ",\"look\":" + stick(base->look.x, base->look.y, base->look.size, {base->look.knob_x, base->look.knob_y}, base->look.live, base->look.accent, base->look.label);
  }
  s += ",\"buttons\":[";
  bool first = true;
  if (!sticks_ && base)
    for (const kern::game::TouchButton& b : base->buttons) {
      s += (first ? "" : ",") + button(b.id, b.label, b.x, b.y, b.w, b.h, b.round, b.pressed, b.on, b.color);
      first = false;
    }
  for (const Btn& b : buttons_) {
    s += (first ? "" : ",") + button(b.b.id, b.b.label, b.x, b.y, b.w, b.h, true, b.pressed, b.b.on, b.b.color);
    first = false;
  }
  s += "]";
  if (!sticks_ && base && base->fire) {
    std::snprintf(buf, sizeof buf, ",\"fire\":true,\"fireX\":%.1f,\"fireY\":%.1f", base->fire_x, base->fire_y);
    s += buf;
  }
  return s + "}";
}

}  // namespace sc
