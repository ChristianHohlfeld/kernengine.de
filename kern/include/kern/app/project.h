// The project's settings the engine applies when a program starts - the one way to set up the renderer: the renderer,
// the device profiles beside it, and the heap taken ahead - the member "settings" of the project file
// (kern/game/project.h, kern/docs/project.md 2.).
//
// The program's kern_main() hands them to the engine with its App (kern::run(app, settings), kern/app/run.h; a project
// whose App is a program Blueprint: kern::run(project)). kern_app_start (src/app/app.cpp) applies them before the App
// starts: the renderer starts with `renderer` - or, when the launch argument profile=<name> names a device profile,
// with that profile's - and the heap is grown once. A profile the argument names that the project does not have is
// an error in the log (with the names it has); the renderer then starts with `renderer`.
//
// Header only: the project (kern/src/game) and the app runtime (kern/src/app) both read it, and either is built
// without the other.
#pragma once

#include <charconv>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

#include "kern/core/json.h"
#include "kern/gfx/renderer.h"

namespace kern {

/// A device profile: the renderer on a kind of device (a phone, a weak laptop), chosen at start by the launch
/// argument profile=<name>. Its renderer is the project's with the keys the profile names on top.
struct DeviceProfile {
  std::string name;
  RendererOptions renderer;
};

struct ProjectSettings {
  RendererOptions renderer;             // the renderer at start (width and height are the page's)
  std::vector<DeviceProfile> profiles;  // the renderer on other devices, by name (the launch argument profile=<name>)
  u32 heap_reserve_mb = 0;              // taken once and given back before start: the allocator keeps it (wasm memory never shrinks)

  /// The profile of that name (null: none).
  const DeviceProfile* profile(std::string_view name) const {
    for (const DeviceProfile& p : profiles)
      if (p.name == name) return &p;
    return nullptr;
  }
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

namespace project_json {
inline json::Value renderer_to(const RendererOptions& o) {
  json::Value r = json::Value::object();
  r.set("shadows", o.shadows);
  r.set("shadow", shadow_to(o.shadow));
  json::Value bloom = json::Value::object();
  bloom.set("enabled", o.bloom.enabled);
  bloom.set("forced", o.bloom.forced);
  r.set("bloom", std::move(bloom));
  r.set("ao", ao_to(o.ao));
  r.set("grade", o.grade);
  return r;
}
/// The keys of "renderer" a document names, on top of `o` (missing keys keep what `o` has).
inline void renderer_from(const json::Value& r, RendererOptions& o) {
  if (!r.is_object()) return;
  o.shadows = r["shadows"].as_bool(o.shadows);
  shadow_from(r["shadow"], o.shadow);
  const json::Value& bloom = r["bloom"];
  if (bloom.is_object()) {
    o.bloom.enabled = bloom["enabled"].as_bool(o.bloom.enabled);
    o.bloom.forced = bloom["forced"].as_bool(o.bloom.forced);
  }
  ao_from(r["ao"], o.ao);
  o.grade = r["grade"].as_bool(o.grade);
}
}  // namespace project_json

/// The settings as the project file holds them (the member "settings"; a profile with its whole renderer).
inline json::Value project_to_json(const ProjectSettings& p) {
  using namespace project_json;
  json::Value o = json::Value::object();
  o.set("renderer", renderer_to(p.renderer));
  json::Value profiles = json::Value::array();
  for (const DeviceProfile& d : p.profiles) {
    json::Value x = json::Value::object();
    x.set("name", d.name);
    x.set("renderer", renderer_to(d.renderer));
    profiles.push(std::move(x));
  }
  o.set("profiles", std::move(profiles));
  o.set("heapReserveMb", p.heap_reserve_mb);
  return o;
}

/// Reads the member "settings". Missing keys keep the engine's defaults; a profile starts from the project's renderer.
/// False - `error` says where and why - for settings that are no object, a profile without a name or with a name
/// twice, "mobile" (the fixed phone profile of before: now one of "profiles"), or a heapReserveMb outside 0..4095.
inline bool project_from_json(const json::Value& v, ProjectSettings& out, std::string* error = nullptr) {
  using namespace project_json;
  auto fail = [&](std::string why) {
    if (error) *error = "settings: " + std::move(why);
    return false;
  };
  out = ProjectSettings{};
  if (!v.is_object()) return fail("must be an object");
  if (v.has("mobile")) return fail("mobile: a device profile is one of \"profiles\" ({\"name\": \"mobile\", \"renderer\": {...}})");
  renderer_from(v["renderer"], out.renderer);
  if (const json::Value* list = v.find("profiles")) {
    if (!list->is_array()) return fail("profiles: must be a list");
    for (std::size_t i = 0; i < list->size(); ++i) {
      const json::Value& x = (*list)[i];
      const std::string at = "profiles[" + std::to_string(i) + "]";
      if (!x.is_object() || !x["name"].is_string() || x["name"].as_string().empty()) return fail(at + ": a profile has a name");
      DeviceProfile d;
      d.name = x["name"].as_string();
      if (out.profile(d.name)) return fail(at + ": the profile " + d.name + " stands twice");
      d.renderer = out.renderer;
      renderer_from(x["renderer"], d.renderer);
      out.profiles.push_back(std::move(d));
    }
  }
  const f64 mb = v["heapReserveMb"].as_number(0);
  if (!(mb >= 0 && mb < 4096)) return fail("heapReserveMb: from 0 to 4095");
  out.heap_reserve_mb = static_cast<u32>(mb);
  return true;
}

}  // namespace kern
