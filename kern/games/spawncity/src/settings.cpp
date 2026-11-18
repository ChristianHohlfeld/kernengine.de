// SpawnCity's settings: the table and its laws are the Blueprint of the level's actor "Settings"
// (see settings.h); here are a player's values and the JSON the page gets. The table the game holds
// (SettingsTable) is handed to every function that reads it.
#include "settings.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "blueprint_call.h"

namespace sc {

namespace {
rules::Blueprint g_rules{"Settings"};
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

bool bind_settings(kern::logic::Runtime& logic, SettingsTable& table) {
  const bool runs = g_rules.bind(logic);
  table.specs.clear();
  table.rows = g_rules.get<std::vector<Option>>("Options");
  if (!runs || table.rows.size() != kCount) {
    table.rows.clear();
    return false;
  }
  for (const Option& t : table.rows)
    table.specs.push_back({t.key, t.section, t.label, t.help, static_cast<f32>(t.min), static_cast<f32>(t.max), static_cast<f32>(t.step), static_cast<f32>(t.def),
                       t.toggle, t.live});
  return true;
}

const OptSpec& opt_spec(const SettingsTable& table, Opt o) {
  return static_cast<u32>(o) < table.specs.size() ? table.specs[static_cast<u32>(o)] : kNone;
}

bool find_opt(const SettingsTable& table, std::string_view key, Opt& out) {
  for (u32 i = 0; i < table.specs.size(); ++i)
    if (key == table.specs[i].key) { out = static_cast<Opt>(i); return true; }
  return false;
}

Settings::Settings(const SettingsTable& table) {
  for (u32 i = 0; i < kCount; ++i) v_[i] = opt_spec(table, static_cast<Opt>(i)).def;
}

void Settings::set(const SettingsTable& table, Opt o, f32 value) {
  const u32 i = static_cast<u32>(o);
  if (i >= table.rows.size() || !std::isfinite(value)) return;
  const f32 v = static_cast<f32>(g_rules.number("Snap", table.rows[i], rules::decimal(value)));
  if (v != v_[i]) { v_[i] = v; ++version_; }
}

bool Settings::set(const SettingsTable& table, std::string_view key, std::string_view value) {
  Opt o;
  if (!find_opt(table, key, o)) return false;
  bool known = false;
  f64 word = 0;
  g_rules.call("Word", std::tie(known, word), value);
  if (known) {
    set(table, o, static_cast<f32>(word));
    return true;
  }
  const std::string text(value);
  char* end = nullptr;
  const double d = std::strtod(text.c_str(), &end);
  if (end == text.c_str()) return false;
  set(table, o, static_cast<f32>(d));
  return true;
}

void Settings::reset_section(const SettingsTable& table, std::string_view section) {
  for (u32 i = 0; i < table.rows.size(); ++i)
    if (g_rules.flag("InSection", table.rows[i], section)) set(table, static_cast<Opt>(i), table.specs[i].def);
}

std::string Settings::values_json(const SettingsTable& table) const {
  std::string o = "{";
  for (u32 i = 0; i < table.specs.size(); ++i) {
    if (i) o += ',';
    o += '"';
    o += table.specs[i].key;
    o += "\":";
    o += num(v_[i]);
  }
  return o + "}";
}

std::string Settings::schema_json(const SettingsTable& table) const {
  std::string o = "[";
  for (u32 i = 0; i < table.specs.size(); ++i) {
    const OptSpec& s = table.specs[i];
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
