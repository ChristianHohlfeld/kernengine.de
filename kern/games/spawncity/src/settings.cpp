// SpawnCity's settings: the table and its laws are the Blueprint of the level's actor "Settings"
// (see settings.h); here are a player's values and the JSON the page gets.
#include "settings.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "blueprint_call.h"

namespace sc {

namespace {
rules::Blueprint g_rules{"Settings"};
/// The Blueprint's struct Option, as it gives and takes it.
struct Option {
  std::string key, section, label, help;
  f64 min = 0, max = 0, step = 0, def = 0;
  bool toggle = false, live = false;
};
/// The table as the Blueprint has it - its rows (what Snap and InSection take; the specs point into
/// their texts) - and the specs.
std::vector<Option> g_rows;
std::vector<OptSpec> g_specs;
const OptSpec kNone = {"", "", "", "", 0, 0, 0, 0, false, false};
constexpr u32 kCount = static_cast<u32>(Opt::count);

void escape_into(std::string& out, std::string_view s) {
  for (char c : s) {
    if (c == '"' || c == '\\') { out += '\\'; out += c; }
    else if (static_cast<unsigned char>(c) < 0x20) out += ' ';
    else out += c;
  }
}
std::string num(f32 v) {
  char b[32];
  std::snprintf(b, sizeof b, "%g", static_cast<double>(v));
  return b;
}
}  // namespace

bool bind_settings(kern::logic::Instance* blueprint) {
  g_rules.running = blueprint;
  g_specs.clear();
  g_rows = g_rules.get<std::vector<Option>>("Options");  // (the rows stay as they are now: the specs point into their texts)
  if (g_rows.size() != kCount) {
    g_rows.clear();
    return false;
  }
  for (const Option& t : g_rows)
    g_specs.push_back({t.key.c_str(), t.section.c_str(), t.label.c_str(), t.help.c_str(), static_cast<f32>(t.min), static_cast<f32>(t.max), static_cast<f32>(t.step), static_cast<f32>(t.def),
                       t.toggle, t.live});
  return true;
}

const OptSpec& opt_spec(Opt o) { return static_cast<u32>(o) < g_specs.size() ? g_specs[static_cast<u32>(o)] : kNone; }

bool find_opt(std::string_view key, Opt& out) {
  for (u32 i = 0; i < g_specs.size(); ++i)
    if (key == g_specs[i].key) { out = static_cast<Opt>(i); return true; }
  return false;
}

Settings::Settings() {
  for (u32 i = 0; i < kCount; ++i) v_[i] = opt_spec(static_cast<Opt>(i)).def;
}

void Settings::set(Opt o, f32 value) {
  const u32 i = static_cast<u32>(o);
  if (i >= g_rows.size() || !std::isfinite(value)) return;
  const f32 v = static_cast<f32>(g_rules.number("Snap", g_rows[i], rules::decimal(value)));
  if (v != v_[i]) { v_[i] = v; ++version_; }
}

bool Settings::set(std::string_view key, std::string_view value) {
  Opt o;
  if (!find_opt(key, o)) return false;
  bool known = false;
  f64 word = 0;
  g_rules.call("Word", std::tie(known, word), value);
  if (known) {
    set(o, static_cast<f32>(word));
    return true;
  }
  const std::string text(value);
  char* end = nullptr;
  const double d = std::strtod(text.c_str(), &end);
  if (end == text.c_str()) return false;
  set(o, static_cast<f32>(d));
  return true;
}

void Settings::reset_section(std::string_view section) {
  for (u32 i = 0; i < g_rows.size(); ++i)
    if (g_rules.flag("InSection", g_rows[i], section)) set(static_cast<Opt>(i), g_specs[i].def);
}

std::string Settings::values_json() const {
  std::string o = "{";
  for (u32 i = 0; i < g_specs.size(); ++i) {
    if (i) o += ',';
    o += '"';
    o += g_specs[i].key;
    o += "\":";
    o += num(v_[i]);
  }
  return o + "}";
}

std::string Settings::schema_json() const {
  std::string o = "[";
  for (u32 i = 0; i < g_specs.size(); ++i) {
    const OptSpec& s = g_specs[i];
    if (i) o += ',';
    o += "{\"key\":\"";
    escape_into(o, s.key);
    o += "\",\"section\":\"";
    escape_into(o, s.section);
    o += "\",\"label\":\"";
    escape_into(o, s.label);
    o += "\",\"help\":\"";
    escape_into(o, s.help);
    o += "\",\"min\":" + num(s.min) + ",\"max\":" + num(s.max) + ",\"step\":" + num(s.step) + ",\"def\":" + num(s.def);
    o += std::string(",\"toggle\":") + (s.toggle ? "true" : "false") + ",\"live\":" + (s.live ? "true" : "false") + ",\"value\":" + num(v_[i]) + "}";
  }
  return o + "]";
}

}  // namespace sc
