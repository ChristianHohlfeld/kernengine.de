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
using kern::logic::Value;
rules::Blueprint g_rules;
/// The table as the Blueprint has it: a row's texts (the specs point into them), the specs, and
/// each row as the Blueprint's value (what Snap and InSection take).
struct Row { std::string key, section, label, help; };
std::vector<Row> g_texts;
std::vector<OptSpec> g_specs;
std::vector<Value> g_rows;
const OptSpec kNone = {"", "", "", "", 0, 0, 0, 0, false, false};
constexpr u32 kCount = static_cast<u32>(Opt::count);

void escape_into(std::string& out, const char* s) {
  for (; *s; ++s) {
    if (*s == '"' || *s == '\\') { out += '\\'; out += *s; }
    else if (static_cast<unsigned char>(*s) < 0x20) out += ' ';
    else out += *s;
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
  g_texts.clear();
  g_specs.clear();
  g_rows.clear();
  if (!blueprint) return false;
  const Value table = blueprint->get("Options");
  if (table.size() != kCount) return false;
  g_texts.reserve(kCount);  // (the specs point into the texts: they do not move again)
  for (u32 i = 0; i < kCount; ++i) {
    const Value row = table.item(i);
    if (row.fields() < 10) { g_texts.clear(); g_specs.clear(); g_rows.clear(); return false; }
    g_texts.push_back({row.field(0)->as_text(), row.field(1)->as_text(), row.field(2)->as_text(), row.field(3)->as_text()});
    const Row& t = g_texts.back();
    g_specs.push_back({t.key.c_str(), t.section.c_str(), t.label.c_str(), t.help.c_str(), static_cast<f32>(row.field(4)->as_number()),
                       static_cast<f32>(row.field(5)->as_number()), static_cast<f32>(row.field(6)->as_number()), static_cast<f32>(row.field(7)->as_number()),
                       row.field(8)->b, row.field(9)->b});
    g_rows.push_back(row);
  }
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
  const f32 v = static_cast<f32>(g_rules.number("Snap", {g_rows[i], rules::number(value)}));
  if (v != v_[i]) { v_[i] = v; ++version_; }
}

bool Settings::set(std::string_view key, std::string_view value) {
  Opt o;
  if (!find_opt(key, o)) return false;
  if (std::vector<Value> word; g_rules.call("Word", {Value::text(std::string(value))}, word) && word.size() >= 2 && word[0].b) {
    set(o, static_cast<f32>(word[1].as_number()));
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
    if (g_rules.flag("InSection", {g_rows[i], Value::text(std::string(section))})) set(static_cast<Opt>(i), g_specs[i].def);
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
