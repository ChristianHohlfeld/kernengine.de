// The project's settings the engine applies when a program starts: the renderer on the desktop and on a phone,
// and the heap taken ahead (a level's "project", kern/docs/book/12-your-own-games.md "Project settings").
//
// The level carries them (game::Level::project, the key "project" after "name"); a game built from a level
// hands them to the engine as the text of that member (kern_project_text(), written by the build - SpawnCity:
// tools/embed_level.py). kern_app_start (src/app/app.cpp) reads them before App::configure() and App::start(): with settings,
// the renderer starts with theirs, the heap is grown once, and with the launch argument mobile=1 the phone's
// shadows and ambient occlusion replace the desktop's - all before the App starts. Without, the App decides
// (configure(), start()) as before.
//
// Header only: the level (kern/src/game) and the app runtime (kern/src/app) both read it, and either is built
// without the other.
#pragma once

#include <charconv>
#include <cstdlib>
#include <string>
#include <string_view>

#include "kern/core/json.h"
#include "kern/gfx/renderer.h"

namespace kern {

struct ProjectSettings {
  bool set = false;            // the level names its settings (else the App's configure() and start() decide)
  RendererOptions renderer;    // the renderer at start (width and height are the page's)
  bool mobile_set = false;     // a phone's renderer differs (the launch argument mobile=1)
  ShadowOptions mobile_shadow;
  AoOptions mobile_ao;
  u32 heap_reserve_mb = 0;     // taken once and given back before start: the allocator keeps it (wasm memory never shrinks)
};

namespace project_json {
/// A 32-bit number as the decimal it stands for (0.0014f -> 0.0014): the level file reads as written.
inline json::Value num(f32 x) {
  char b[32];
  const auto r = std::to_chars(b, b + sizeof b, x);
  return json::Value(std::strtod(std::string(b, r.ptr).c_str(), nullptr));
}
inline const char* quality_name(AoQuality q) {
  switch (q) {
    case AoQuality::low: return "low";
    case AoQuality::medium: return "medium";
    case AoQuality::high: return "high";
    case AoQuality::ultra: return "ultra";
  }
  return "medium";
}
inline AoQuality quality_of(const std::string& s, AoQuality fallback) {
  return s == "low" ? AoQuality::low : s == "medium" ? AoQuality::medium : s == "high" ? AoQuality::high : s == "ultra" ? AoQuality::ultra : fallback;
}
inline json::Value shadow_to(const ShadowOptions& s) {
  json::Value o = json::Value::object();
  o.set("resolution", s.resolution);
  o.set("extent", num(s.extent));
  o.set("bias", num(s.bias));
  o.set("filter", s.filter);
  return o;
}
inline void shadow_from(const json::Value& v, ShadowOptions& s) {
  if (!v.is_object()) return;
  s.resolution = static_cast<i32>(v["resolution"].as_number(s.resolution));
  s.extent = static_cast<f32>(v["extent"].as_number(s.extent));
  s.bias = static_cast<f32>(v["bias"].as_number(s.bias));
  s.filter = static_cast<i32>(v["filter"].as_number(s.filter));
}
inline json::Value ao_to(const AoOptions& a) {
  json::Value o = json::Value::object();
  o.set("enabled", a.enabled);
  o.set("quality", quality_name(a.quality));
  o.set("halfResolution", a.half_resolution);
  o.set("radius", num(a.radius));
  o.set("power", num(a.power));
  o.set("adaptive", a.adaptive);
  return o;
}
inline void ao_from(const json::Value& v, AoOptions& a) {
  if (!v.is_object()) return;
  a.enabled = v["enabled"].as_bool(a.enabled);
  if (v["quality"].is_string()) a.quality = quality_of(v["quality"].as_string(), a.quality);
  a.half_resolution = v["halfResolution"].as_bool(a.half_resolution);
  a.radius = static_cast<f32>(v["radius"].as_number(a.radius));
  a.power = static_cast<f32>(v["power"].as_number(a.power));
  a.adaptive = v["adaptive"].as_bool(a.adaptive);
}
}  // namespace project_json

/// The settings as the level file holds them (the member "project").
inline json::Value project_to_json(const ProjectSettings& p) {
  using namespace project_json;
  json::Value o = json::Value::object();
  json::Value r = json::Value::object();
  r.set("shadows", p.renderer.shadows);
  r.set("shadow", shadow_to(p.renderer.shadow));
  json::Value bloom = json::Value::object();
  bloom.set("enabled", p.renderer.bloom.enabled);
  bloom.set("forced", p.renderer.bloom.forced);
  r.set("bloom", std::move(bloom));
  r.set("ao", ao_to(p.renderer.ao));
  r.set("grade", p.renderer.grade);
  o.set("renderer", std::move(r));
  if (p.mobile_set) {
    json::Value m = json::Value::object();
    m.set("shadow", shadow_to(p.mobile_shadow));
    m.set("ao", ao_to(p.mobile_ao));
    o.set("mobile", std::move(m));
  }
  o.set("heapReserveMb", p.heap_reserve_mb);
  return o;
}

/// Reads the member "project" (an object; anything else: no settings). Missing keys keep their defaults.
inline bool project_from_json(const json::Value& v, ProjectSettings& out) {
  using namespace project_json;
  out = ProjectSettings{};
  if (!v.is_object()) return false;
  out.set = true;
  const json::Value& r = v["renderer"];
  if (r.is_object()) {
    out.renderer.shadows = r["shadows"].as_bool(out.renderer.shadows);
    shadow_from(r["shadow"], out.renderer.shadow);
    const json::Value& bloom = r["bloom"];
    if (bloom.is_object()) {
      out.renderer.bloom.enabled = bloom["enabled"].as_bool(out.renderer.bloom.enabled);
      out.renderer.bloom.forced = bloom["forced"].as_bool(out.renderer.bloom.forced);
    }
    ao_from(r["ao"], out.renderer.ao);
    out.renderer.grade = r["grade"].as_bool(out.renderer.grade);
  }
  const json::Value& m = v["mobile"];
  if (m.is_object()) {
    out.mobile_set = true;
    // a phone starts from the desktop's shadows and the default ambient occlusion
    out.mobile_shadow = out.renderer.shadow;
    shadow_from(m["shadow"], out.mobile_shadow);
    ao_from(m["ao"], out.mobile_ao);
  }
  const f64 mb = v["heapReserveMb"].as_number(0);
  out.heap_reserve_mb = mb > 0 && mb < 4096 ? static_cast<u32>(mb) : 0;
  return true;
}

/// The settings from the text of the member "project" (kern_project_text()); empty or broken: none.
inline bool project_from_text(std::string_view text, ProjectSettings& out) {
  out = ProjectSettings{};
  if (text.empty()) return false;
  json::Value v;
  return json::parse(text, v) && project_from_json(v, out);
}

}  // namespace kern
