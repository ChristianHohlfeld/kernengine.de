// Widgets of the app (kern/docs/app.md 6.): text at a place, bar, button, menu - immediate mode, drawn by the page
// (kern/web/app.mjs) from the list the engine hands over after each frame in which it changed; the status line is the
// one widget that stays.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "kern/app/app.h"

namespace kern {

void Engine::set_status(const char* text) {
  const std::string t = text ? text : "";
#if !defined(__wasm__)
  if (t != status_) std::printf("[status] %s\n", t.c_str());   // (natively the log is the page)
#endif
  status_ = t;
}

bool Engine::ui_add(Widget w) {
  const char* why = nullptr;
  if (w.id.empty() || w.id.size() > 64) why = "an id is 1 to 64 characters";
  else if (w.id == "status") why = "the id status is the status line's (set_status)";
  else if (!std::isfinite(w.x) || !std::isfinite(w.y) || !std::isfinite(w.value) || !std::isfinite(w.width)) why = "its place, value or width is not a finite number";
  else
    for (const Widget& o : widgets_)
      if (o.id == w.id) { why = "the id stands twice in this frame"; break; }
  if (why) {
    fail("the widget %s \"%s\": %s - it is not shown", w.kind.c_str(), w.id.c_str(), why);
    for (const std::string& s : ui_said_)
      if (s == w.id) return false;
    ui_said_.push_back(w.id);
    log(LogLevel::error, "%s", last_error());
    return false;
  }
  w.x = clamp(w.x, 0.0f, 1.0f);
  w.y = clamp(w.y, 0.0f, 1.0f);
  widgets_.push_back(std::move(w));
  return true;
}

i32 Engine::ui_event_of(const std::string& id) const {
  for (const auto& [eid, index] : ui_events_)
    if (eid == id) return index;
  return -1;
}

void Engine::ui_text(const char* id, const char* text, f32 x, f32 y) { ui_add({"text", id ? id : "", text ? text : "", 0, x, y, 0}); }

void Engine::ui_bar(const char* id, const char* label, f32 value, f32 x, f32 y, f32 width) {
  ui_add({"bar", id ? id : "", label ? label : "", std::isfinite(value) ? clamp(value, 0.0f, 1.0f) : value, x, y, std::isfinite(width) ? clamp(width, 0.02f, 1.0f) : width});
}

bool Engine::ui_button(const char* id, const char* label, f32 x, f32 y) {
  return ui_add({"button", id ? id : "", label ? label : "", 0, x, y, 0}) && ui_event_of(id) == 0;
}

i32 Engine::ui_menu(const char* id, const char* items, f32 x, f32 y) {
  const std::string list = items ? items : "";
  if (!ui_add({"menu", id ? id : "", list, 0, x, y, 0})) return -1;
  i32 count = list.empty() ? 0 : 1;
  for (char c : list) count += c == '\n';
  const i32 chosen = ui_event_of(id);
  return chosen >= 0 && chosen < count ? chosen : -1;
}

void Engine::ui_event(std::string_view id, i32 index) { ui_pending_.emplace_back(std::string(id), index); }

void Engine::ui_begin() {
  ui_events_ = std::move(ui_pending_);
  ui_pending_.clear();
  widgets_.clear();
}

void Engine::ui_end() {
  json::Value list = json::Value::array();
  auto place = [](json::Value& o, const Widget& w) {
    o.set("x", static_cast<f64>(w.x));
    o.set("y", static_cast<f64>(w.y));
  };
  if (!status_.empty()) {
    json::Value o = json::Value::object();
    o.set("kind", "text");
    o.set("id", "status");
    o.set("text", status_);
    o.set("x", 0.0);
    o.set("y", 0.0);
    o.set("status", true);
    list.push(std::move(o));
  }
  for (const Widget& w : widgets_) {
    json::Value o = json::Value::object();
    o.set("kind", w.kind);
    o.set("id", w.id);
    place(o, w);
    if (w.kind == "text") o.set("text", w.text);
    else if (w.kind == "bar") {
      o.set("label", w.text);
      o.set("value", static_cast<f64>(w.value));
      o.set("width", static_cast<f64>(w.width));
    } else if (w.kind == "button") o.set("label", w.text);
    else {
      json::Value items = json::Value::array();
      std::size_t at = 0;
      while (!w.text.empty() && at <= w.text.size()) {
        const std::size_t nl = std::min(w.text.find('\n', at), w.text.size());
        items.push(w.text.substr(at, nl - at));
        at = nl + 1;
      }
      o.set("items", std::move(items));
    }
    list.push(std::move(o));
  }
  std::string text = json::stringify(list);
  if (text == ui_sent_) return;
  ui_sent_ = std::move(text);
  host_->widgets(ui_sent_);
}

}  // namespace kern
