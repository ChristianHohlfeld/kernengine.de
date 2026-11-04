// SpawnCity's touch controls (see touch_play.h).
#include "touch_play.h"

#include <algorithm>
#include <cstdio>

#include "controls.h"

namespace sc {

namespace {
constexpr f32 kSafe = 10, kGap = 8;  // edge_hud_layout.mjs SAFE / GAP

std::string esc(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (static_cast<unsigned char>(c) < 0x20) { o += ' '; continue; }
    if (c == '"' || c == '\\') o += '\\';
    o += c;
  }
  return o;
}

TouchBtn btn(const char* id, const std::string& label, f32 w, f32 h, u32 color, bool round, bool on_down = true) {
  TouchBtn b;
  b.id = id;
  b.label = label;
  b.w = w;
  b.h = h;
  b.color = color;
  b.round = round;
  b.on_down = on_down;
  return b;
}
}  // namespace

void TouchPlay::set_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb) {
  w_ = w; h_ = h; sl_ = sl; st_ = st; sr_ = sr; sb_ = sb;
  layout();
}

void TouchPlay::set_layout(const TouchLayout& l) {
  const bool mode_changed = l.mode != lay_.mode;
  lay_ = l;
  if (mode_changed) release_all();
  layout();
}

void TouchPlay::place_row(std::vector<TouchBtn>& items, f32 x0, f32 bottom, f32 dir, f32 max_w) {
  // edge_hud_layout.mjs packRows: fill a row along dir, wrap upwards when it would exceed max_w.
  f32 x = x0, row_h = 0, y_bottom = bottom, used = 0;
  for (TouchBtn& b : items) {
    if (used > 0 && used + b.w > max_w) { y_bottom -= row_h + kGap; x = x0; used = 0; row_h = 0; }
    b.x = dir > 0 ? x : x - b.w;
    b.y = y_bottom - b.h;
    x += dir * (b.w + kGap);
    used += b.w + kGap;
    row_h = std::max(row_h, b.h);
    add(b);
  }
}

void TouchPlay::layout() {
  // Keep the pressed state of buttons that stay.
  std::vector<std::string> down;
  for (const TouchBtn& b : buttons_)
    if (b.pressed) down.push_back(b.id);
  buttons_.clear();
  pad_w_ = pad_h_ = 0;
  if (lay_.mode == TouchMode::off) return;
  const TouchMode m = lay_.mode;
  // The sticks: 168 px flush in the safe corners (thumb_layout.mjs).
  const f32 Lx = sl_, Ly = h_ - sb_ - kStick, Rx = w_ - sr_ - kStick, Ry = h_ - sb_ - kStick;
  constexpr u32 dark = 0x0d1118, amber = 0x2a1d0c, blue = 0x0e2a3e, red = 0x8b2436, green = 0x2f9e5a, yellow = 0xffc93c;
  if (lay_.aiming) {
    // fighter_jets.mjs targeting overlay: tap the ground, or ABBRECHEN.
    TouchBtn c = btn("strike_cancel", "✕ ABBRECHEN", 120, 40, red, false, false);
    c.x = (w_ - c.w) / 2;
    c.y = std::max(8.0f, st_) + 60;
    add(c);
    return;
  }
  // ---- left thumb ----
  if (m == TouchMode::foot || m == TouchMode::car) {
    // the mode switch low, right of the move stick (game_exit_button.mjs: ⇄ DRONE / ⇄ FPS)
    TouchBtn mode = btn("mode", m == TouchMode::foot ? (lay_.drone_cooldown ? "⇄ DRONE …" : "⇄ DRONE") : "⇄ FPS", 96, 40, m == TouchMode::foot ? 0x3a2810 : blue, false, false);
    mode.x = Lx + kStick + 8;
    mode.y = h_ - kSafe - mode.h;
    add(mode);
  }
  std::vector<TouchBtn> left;
  if (m == TouchMode::drone) left.push_back(btn("mode", "⇄ FPS", 96, 40, blue, false, false));
  if (m == TouchMode::foot || m == TouchMode::drone) left.push_back(btn("strike", "STRIKE", 58, 58, lay_.strike_ready ? dark : 0x3a3a3a, true));
  if (m == TouchMode::foot && lay_.grenade) {
    TouchBtn g = btn("nade", lay_.grenade_left >= 0 ? (std::to_string(static_cast<int>(lay_.grenade_left * 10) / 10) + "." + std::to_string(static_cast<int>(lay_.grenade_left * 10) % 10)) : "● " + std::to_string(lay_.grenades), 64, 64,
                     lay_.grenade_left >= 0 ? 0x3a1c0c : 0x1c2414, true);
    g.on = lay_.grenade_left >= 0;
    left.push_back(g);
  }
  place_row(left, std::max(kSafe, Lx + 10), Ly - 8, 1, std::max(kStick, 190.0f));
  if (m == TouchMode::drone) {
    // the altitude pad right of the move stick (simulator.mjs #soloClearance 48 x 132)
    TouchBtn p = btn("clearance", "▲ HÖHE ▼", 48, 132, 0x101820, false);
    p.x = Lx + kStick + 8;
    p.y = h_ - kSafe - p.h;
    pad_x_ = p.x; pad_y_ = p.y; pad_w_ = p.w; pad_h_ = p.h;
    add(p);
  }
  // ---- right thumb ----
  std::vector<TouchBtn> low;
  if (m == TouchMode::car) low.push_back(btn("enter", "↗", 54, 54, 0xff6b5a, true, false));
  else if (m == TouchMode::drone) {
    low.push_back(btn("wpn", "⇄ " + lay_.weapon, 60, 60, amber, true));
    low.push_back(btn("arm", lay_.armed ? "ARMED" : "ARM", 104, 44, lay_.armed ? green : 0xf5b301, false, false));
    low.push_back(btn("stop", "STOP", 94, 44, red, false, false));
  } else if (m == TouchMode::foot) {
    low.push_back(btn("wpn", "⇄ " + lay_.weapon, 60, 60, amber, true));
    low.push_back(btn("jump", "JUMP", 64, 64, dark, true));
  }
  f32 x = Rx - 8;
  for (TouchBtn& b : low) {
    b.x = x - b.w;
    b.y = h_ - kSafe - b.h;
    x -= b.w + 8;
    add(b);
  }
  if (m == TouchMode::foot && lay_.sniper) {
    // sniper_scope.mjs: SCOPE straight above the weapon button
    for (const TouchBtn& b : buttons_)
      if (b.id == "wpn") {
        TouchBtn s = btn("scope", "◎ SCOPE", 60, 60, lay_.scope_on ? 0xb3261e : 0x0c1a14, true);
        s.on = lay_.scope_on;
        s.x = b.x;
        s.y = b.y - 66;
        add(s);
        break;
      }
  }
  if (m == TouchMode::car) {
    // #vehicleButtons: a column above the look stick, right aligned, gap 8
    TouchBtn cam = btn("cam", lay_.cockpit ? "🎥 AUSSEN" : "🎥 INNEN", 104, 36, dark, false, false);
    TouchBtn hb = btn("hb", "HANDBREMSE", 112, 36, dark, false);
    hb.x = w_ - kSafe - hb.w;
    hb.y = Ry - 8 - hb.h;
    cam.x = w_ - kSafe - cam.w;
    cam.y = hb.y - 8 - cam.h;
    add(cam);
    add(hb);
  } else if (m == TouchMode::foot || m == TouchMode::drone) {
    std::vector<TouchBtn> up;
    if (m == TouchMode::foot && lay_.near_car) up.push_back(btn("enter", "🚗", 54, 54, yellow, true, false));
    if (m == TouchMode::foot && lay_.near_jet) up.push_back(btn("jet", "✈", 54, 54, 0x7fd3ff, true, false));
    if (lay_.emp) up.push_back(btn("emp", "EMP", 62, 36, 0x286eaa, false));
    place_row(up, w_ - kSafe, Ry - 8, -1, std::max(kStick, 190.0f));
  }
  if (m == TouchMode::jet) {
    // jet_mode.mjs: the buttons centred at the bottom, EJECT top left (hold), KANONE left of the look stick
    std::vector<TouchBtn> row = {btn("vtol", lay_.hover ? "⇄ FLUG" : "⇄ SCHWEBEN", 96, 40, 0x08140c, false, false), btn("rkt", "RAKETE", 72, 40, lay_.rocket ? 0x65400b : 0x08140c, false, false),
                                 btn("bomb", "BOMBE", 70, 40, 0x08140c, false, false), btn("cam", "CAM", 60, 40, 0x08140c, false, false)};
    row[1].on = lay_.rocket;
    f32 total = -8;
    for (const TouchBtn& b : row) total += b.w + 8;
    f32 bx = (w_ - total) / 2;
    for (TouchBtn& b : row) {
      b.x = bx;
      b.y = h_ - std::max(16.0f, sb_) - b.h;
      bx += b.w + 8;
      add(b);
    }
    TouchBtn ej = btn("eject", lay_.ground ? "AUSSTEIGEN" : "EJECT", 84, 38, 0x411811, false, false);
    ej.x = std::max(8.0f, sl_);
    ej.y = st_ + 55;
    add(ej);
    const f32 size = clampf(w_ * 0.13f, 96, 116);
    TouchBtn fire = btn("fire", "KANONE", size, size, 0x7a1a12, true);
    fire.x = w_ - (std::max(12.0f, sr_) + std::min(w_ * 0.25f, 148.0f) + 18) - size;
    fire.y = h_ - std::max(22.0f, sb_ + 18) - size;
    add(fire);
  }
  for (TouchBtn& b : buttons_)
    if (std::find(down.begin(), down.end(), b.id) != down.end()) b.pressed = true;
}

bool TouchPlay::in_circle(f32 cx, f32 cy, f32 r, f32 x, f32 y) const { return std::hypot(x - cx, y - cy) <= r; }

TouchBtn* TouchPlay::button_at(f32 x, f32 y) {
  // EMP first (foot_touch_controller.mjs hit-tests it before anything else).
  for (TouchBtn& b : buttons_)
    if (b.id == "emp" && x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h) return &b;
  for (TouchBtn& b : buttons_) {
    if (b.round ? in_circle(b.x + b.w / 2, b.y + b.h / 2, b.w / 2, x, y) : (x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h)) return &b;
  }
  return nullptr;
}

bool TouchPlay::pointer(u32 type, i32 id, f32 x, f32 y, f64 t_ms) {
  if (lay_.mode == TouchMode::off) return false;
  auto it = std::find_if(fingers_.begin(), fingers_.end(), [&](const Finger& f) { return f.id == id; });
  const f32 t = static_cast<f32>(t_ms / 1000.0);
  if (type == 0) {
    if (it != fingers_.end()) fingers_.erase(it);
    Finger f;
    f.id = id;
    f.x0 = f.x = f.last_x = x;
    f.y0 = f.y = f.last_y = y;
    f.t0 = f.last_t = t_ms;
    const f32 r = kStick / 2;
    const f32 Lcx = sl_ + r, Lcy = h_ - sb_ - r, Rcx = w_ - sr_ - r, Rcy = h_ - sb_ - r;
    if (TouchBtn* b = button_at(x, y)) {
      f.kind = Finger::press;
      f.button = b->id;
      b->pressed = true;
      if (b->on_down) pressed_.push_back(b->id);
      if (b->id == "clearance") { f.kind = Finger::clearance; clearance_ = controls::clearance_pad_axis(pad_y_ + pad_h_ / 2, y, pad_h_); }
      if (b->id == "nade") nade_up_ = 0;
    } else if (lay_.aiming) {
      f.kind = Finger::strike;
    } else if (lay_.alive && in_circle(Lcx, Lcy, r, x, y)) {
      // The newest finger takes the stick over; deflection counts from where it lands.
      fingers_.erase(std::remove_if(fingers_.begin(), fingers_.end(), [](const Finger& g) { return g.kind == Finger::move; }), fingers_.end());
      f.kind = Finger::move;
      f.base = {};
      move_id_ = id;
      move_ = {};
    } else if (lay_.alive && in_circle(Rcx, Rcy, r, x, y)) {
      fingers_.erase(std::remove_if(fingers_.begin(), fingers_.end(), [](const Finger& g) { return g.kind == Finger::look; }), fingers_.end());
      f.kind = Finger::look;
      look_id_ = id;
      look_ = look_edge_ = {};
    } else if (lay_.alive && (lay_.mode == TouchMode::foot || lay_.mode == TouchMode::drone || lay_.mode == TouchMode::jet)) {
      // The screen fires at the finger (foot / drone / jet); dragging moves the aim point.
      f.kind = Finger::fire;
      TouchFire fire;
      fire.id = id;
      fire.x = x;
      fire.y = y;
      fire.down_t = t;
      fire.hand = x < w_ / 2 ? 1 : 0;
      fires_.push_back(fire);
      if (lay_.mode == TouchMode::jet && std::count_if(fingers_.begin(), fingers_.end(), [](const Finger& g) { return g.kind == Finger::fire; }) > 0) {
        fires_.pop_back();  // jet_mode.mjs: one tap finger at a time
        f.kind = Finger::none;
      }
    } else if (lay_.mode == TouchMode::car) {
      f.kind = Finger::pinch;
      const Finger* other = nullptr;
      for (const Finger& g : fingers_)
        if (g.kind == Finger::pinch) other = &g;
      if (other) pinch_d_ = std::hypot(other->x - x, other->y - y);
    } else {
      f.kind = Finger::none;
    }
    fingers_.push_back(f);
    return true;
  }
  if (it == fingers_.end()) return false;
  Finger& f = *it;
  if (type == 1) {
    const f32 dx = x - f.last_x, dy = y - f.last_y;
    const f64 dt_ms = std::max(1.0, t_ms - f.last_t);
    f.x = x;
    f.y = y;
    const f32 radius = std::max(1.0f, kStick * kRadiusK);
    switch (f.kind) {
      case Finger::move: {
        f32 vx = f.base.x + (x - f.x0) / radius, vy = f.base.y + (y - f.y0) / radius;
        const f32 m = std::hypot(vx, vy);
        if (m > 1) { vx /= m; vy /= m; }
        move_ = {vx, vy};
        break;
      }
      case Finger::look: {
        if (lay_.mode == TouchMode::foot) {
          // (a) 1:1 drag with the flick gain, (b) the edge turn past the ring (the origin follows).
          if (dx != 0 || dy != 0) {
            f32 yaw, pitch;
            controls::touch_look_delta(dx, dy, static_cast<f32>(std::hypot(dx, dy) / dt_ms * 1000.0), yaw, pitch);
            drag_.x += yaw;
            drag_.y += pitch;
          }
          const f32 cap = radius * kEdgeStart + kEdgeRampPx;
          f32 ox = x - f.x0, oy = y - f.y0, dist = std::hypot(ox, oy);
          if (dist > cap) {
            const f32 pull = (dist - cap) / dist;
            f.x0 += ox * pull;
            f.y0 += oy * pull;
            ox *= 1 - pull;
            oy *= 1 - pull;
            dist = cap;
          }
          const f32 edge = clampf((dist - kEdgeStart * radius) / kEdgeRampPx, 0, 1);
          const f32 m = dist / radius;
          look_ = {std::min(1.0f, m) * (dist > 0 ? ox / dist : 0), std::min(1.0f, m) * (dist > 0 ? oy / dist : 0)};
          look_edge_ = edge > 0 && m > 0 ? Vec2{ox / radius / m * edge, oy / radius / m * edge} : Vec2{};
        } else {
          f32 vx = (x - f.x0) / radius, vy = (y - f.y0) / radius;
          const f32 m = std::hypot(vx, vy);
          if (m > 1) { vx /= m; vy /= m; }
          look_ = {vx, vy};
        }
        break;
      }
      case Finger::fire:
        for (TouchFire& fi : fires_)
          if (fi.id == id) { fi.x = x; fi.y = y; }
        break;
      case Finger::clearance: clearance_ = controls::clearance_pad_axis(pad_y_ + pad_h_ / 2, y, pad_h_); break;
      case Finger::press:
        if (f.button == "nade") nade_up_ = f.y0 - y;  // drag "up" = stronger throw
        break;
      case Finger::pinch: {
        const Finger* other = nullptr;
        for (const Finger& g : fingers_)
          if (g.kind == Finger::pinch && g.id != id) other = &g;
        if (other) {
          const f32 d = std::hypot(other->x - x, other->y - y);
          if (pinch_d_ > 10 && d > 10) zoom_ *= pinch_d_ / d;
          pinch_d_ = d;
        }
        break;
      }
      default: break;
    }
    f.last_x = x;
    f.last_y = y;
    f.last_t = t_ms;
    return true;
  }
  // up / cancel
  switch (f.kind) {
    case Finger::move: move_ = {}; move_id_ = -1; break;
    case Finger::look: look_ = look_edge_ = {}; look_id_ = -1; break;
    case Finger::fire: fires_.erase(std::remove_if(fires_.begin(), fires_.end(), [&](const TouchFire& fi) { return fi.id == id; }), fires_.end()); break;
    case Finger::clearance: clearance_ = 0; break;
    case Finger::strike:
      if (type == 2) { strike_tap_ = true; strike_x_ = x; strike_y_ = y; }
      break;
    case Finger::press: {
      bool still = false;
      for (const Finger& g : fingers_)
        if (g.id != id && g.kind == Finger::press && g.button == f.button) still = true;
      for (TouchBtn& b : buttons_)
        if (b.id == f.button) {
          if (!still) b.pressed = false;
          const bool inside = x >= b.x - 4 && x <= b.x + b.w + 4 && y >= b.y - 4 && y <= b.y + b.h + 4;
          if (!b.on_down && type == 2 && inside) {
            // jet EJECT: only after a 650 ms hold
            if (b.id != "eject" || (t_ms - f.t0) / 1000.0 >= kEjectHold) pressed_.push_back(b.id);
          }
        }
      released_.push_back(f.button);
      break;
    }
    default: break;
  }
  fingers_.erase(it);
  return true;
}

void TouchPlay::release_all() {
  for (const Finger& f : fingers_)
    if (f.kind == Finger::press) released_.push_back(f.button);
  fingers_.clear();
  fires_.clear();
  move_ = look_ = look_edge_ = drag_ = {};
  move_id_ = look_id_ = -1;
  clearance_ = 0;
  for (TouchBtn& b : buttons_) b.pressed = false;
}

Vec2 TouchPlay::take_drag() {
  const Vec2 d = drag_;
  drag_ = {};
  return d;
}

bool TouchPlay::sprint() const {
  const f32 m = std::min(1.0f, std::hypot(move_.x, move_.y));
  return m >= 0.90f && move_.y < -0.35f;
}

f32 TouchPlay::take_zoom() {
  const f32 z = zoom_;
  zoom_ = 1;
  return z;
}

bool TouchPlay::held(const std::string& id) const {
  for (const TouchBtn& b : buttons_)
    if (b.id == id) return b.pressed;
  return false;
}

bool TouchPlay::take_pressed(const std::string& id) {
  auto it = std::find(pressed_.begin(), pressed_.end(), id);
  if (it == pressed_.end()) return false;
  pressed_.erase(it);
  return true;
}

bool TouchPlay::take_released(const std::string& id) {
  auto it = std::find(released_.begin(), released_.end(), id);
  if (it == released_.end()) return false;
  released_.erase(it);
  return true;
}

bool TouchPlay::take_strike_tap(f32& x, f32& y) {
  if (!strike_tap_) return false;
  strike_tap_ = false;
  x = strike_x_;
  y = strike_y_;
  return true;
}

std::string TouchPlay::view_json() const {
  if (lay_.mode == TouchMode::off) return "null";
  char buf[320];
  const f32 Ly = h_ - sb_ - kStick;
  const char* ml = lay_.mode == TouchMode::car ? "LENKEN / GAS" : lay_.mode == TouchMode::jet ? "STEIGEN / SCHUB" : "";
  const char* rl = lay_.mode == TouchMode::car ? "UMSCHAUEN" : lay_.mode == TouchMode::jet ? "KNÜPPEL" : "";
  // The overlay draws the knob at 50 + 31 k %; the drone's knob travels 42 % (control_semantics.mjs).
  const f32 kk = lay_.mode == TouchMode::drone ? 42.0f / 31.0f : 1.0f;
  const bool aiming = lay_.aiming;
  auto stick = [&](f32 x, Vec2 k, bool live, bool accent, const char* label) {
    std::snprintf(buf, sizeof buf, "{\"x\":%.1f,\"y\":%.1f,\"size\":%.1f,\"kx\":%.3f,\"ky\":%.3f,\"live\":%s,\"accent\":%s,\"label\":\"%s\"}", aiming ? -9999.0f : x, Ly, kStick,
                  k.x * kk, k.y * kk, live ? "true" : "false", accent ? "true" : "false", label);
    return std::string(buf);
  };
  std::string s = "{\"move\":" + stick(sl_, move_, move_id_ >= 0, sprint() && lay_.mode == TouchMode::foot, ml);
  s += ",\"look\":" + stick(w_ - sr_ - kStick, look_, look_id_ >= 0, false, rl);
  s += ",\"buttons\":[";
  bool first = true;
  for (const TouchBtn& b : buttons_) {
    s += first ? "" : ",";
    first = false;
    s += "{\"id\":\"" + esc(b.id) + "\",\"label\":\"" + esc(b.label) + "\"";
    std::snprintf(buf, sizeof buf, ",\"x\":%.1f,\"y\":%.1f,\"w\":%.1f,\"h\":%.1f,\"round\":%s,\"pressed\":%s,\"on\":%s,\"color\":\"#%06x\"}", b.x, b.y, b.w, b.h, b.round ? "true" : "false",
                  b.pressed ? "true" : "false", b.on ? "true" : "false", b.color & 0xffffff);
    s += buf;
  }
  s += "]";
  if (!fires_.empty()) {
    std::snprintf(buf, sizeof buf, ",\"fire\":true,\"fireX\":%.1f,\"fireY\":%.1f", fires_.back().x, fires_.back().y);
    s += buf;
  }
  std::snprintf(buf, sizeof buf, ",\"mode\":%d,\"clearance\":%.3f", static_cast<int>(lay_.mode), clearance_);
  s += buf;
  return s + "}";
}

}  // namespace sc
