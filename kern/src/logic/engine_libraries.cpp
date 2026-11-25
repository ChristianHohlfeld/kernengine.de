// The engine's own libraries (kern/logic/engine_libraries.h): read once from the texts the build of the
// engine wrote (engine_libraries.gen.cpp), found by every reader before a project's Blueprints, and the
// engine's version taken where a level still holds an older copy of them.
#include "kern/logic/engine_libraries.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
#include <mutex>

#include "kern/core/json.h"

namespace kern::logic {
namespace {

struct Libraries {
  std::vector<std::string> names;
  std::map<std::string, std::unique_ptr<Graph>, std::less<>> graphs;
  std::map<std::string, std::string, std::less<>> marks;
};

bool off = false;
Libraries* building = nullptr;  // (while the libraries are read: one that uses another finds it here)

const Libraries& libraries() {
  static Libraries all;
  static std::once_flag once;
  std::call_once(once, [] {
    if (off) return;
    building = &all;
    for (const EngineLibraryText& t : engine_library_texts()) {
      json::Value j;
      auto g = std::make_unique<Graph>();
      std::string error;
      // (written by the build from the engine's own headers and read back there: a text that does not read
      // is a broken build of the engine, never a level's fault)
      if (!json::parse(t.json, j, &error) || !graph_from_json(j, *g, &error)) {
        std::fprintf(stderr, "kern: the engine's library %s does not read (%s) - rebuild the engine (tools/engine_api.py --libraries)\n", t.name, error.c_str());
        continue;
      }
      all.names.emplace_back(t.name);
      all.marks.emplace(t.name, t.mark);
      all.graphs.emplace(t.name, std::move(g));
    }
    building = nullptr;
  });
  return all;
}

}  // namespace

void engine_libraries_off() { off = true; }

const std::vector<std::string>& engine_library_names() { return libraries().names; }

const Graph* engine_library(std::string_view name) {
  const Libraries& all = building ? *building : libraries();
  auto found = all.graphs.find(name);
  return found == all.graphs.end() ? nullptr : found->second.get();
}

bool is_engine_library(std::string_view name) { return engine_library(name) != nullptr; }

std::string engine_library_mark(std::string_view name) {
  const Libraries& all = building ? *building : libraries();
  auto found = all.marks.find(name);
  return found == all.marks.end() ? std::string() : found->second;
}

std::string engine_changes(const std::map<std::string, std::string>& marks) {
  std::string out;
  for (const auto& [name, mark] : marks) {
    const std::string now = engine_library_mark(name);
    if (now == mark) continue;
    out += (out.empty() ? "" : "; ") + name + ": written with " + mark + ", the engine has " + (now.empty() ? std::string("no such library") : now);
  }
  return out;
}

Blueprints with_engine(const Blueprints& project) {
  return [project](std::string_view name) -> const Graph* {
    if (const Graph* g = engine_library(name)) return g;
    return project ? project(name) : nullptr;
  };
}

namespace {
/// The marks of the engine's libraries the Blueprint uses - named in its uses, or what it holds is of them.
json::Value with_marks(const Graph& g, json::Value j) {
  std::map<std::string, std::string> marks;
  auto mark = [&](const std::string& name) {
    if (const std::string m = engine_library_mark(name); !m.empty()) marks[name] = m;
  };
  for (const std::string& u : g.uses) mark(u);
  for (const StructType& s : g.structs) mark(s.from);
  for (const EnumType& e : g.enums) mark(e.from);
  for (const Function& f : g.functions) mark(f.from);
  if (marks.empty()) return j;
  json::Value o = json::Value::object();
  for (const auto& [name, m] : marks) o.set(name, m);
  j.set("engine", std::move(o));
  return j;
}
}  // namespace

json::Value graph_to_level_json(const Graph& g) {
  const auto of_engine = [](const std::string& from) { return !from.empty() && is_engine_library(from); };
  if (std::none_of(g.structs.begin(), g.structs.end(), [&](const StructType& s) { return of_engine(s.from); }) &&
      std::none_of(g.enums.begin(), g.enums.end(), [&](const EnumType& e) { return of_engine(e.from); }) &&
      std::none_of(g.functions.begin(), g.functions.end(), [&](const Function& f) { return of_engine(f.from); }))
    return with_marks(g, graph_to_json(g));
  Graph held = g;
  std::erase_if(held.structs, [&](const StructType& s) { return of_engine(s.from); });
  std::erase_if(held.enums, [&](const EnumType& e) { return of_engine(e.from); });
  std::erase_if(held.functions, [&](const Function& f) { return of_engine(f.from); });
  return with_marks(g, graph_to_json(held));
}

bool graph_from_level_json(const json::Value& j, Graph& out, std::string* error, std::map<std::string, std::string>* marks) {
  if (const json::Value* e = j.is_object() ? j.find("engine") : nullptr) {
    if (!e->is_object()) {
      if (error) *error = "engine: the marks of the engine's libraries must be an object {library: mark}";
      return false;
    }
    for (const auto& [name, mark] : e->members())
      if (marks) (*marks)[name] = mark.as_string();
  }
  for (const char* list : {"structs", "enums", "functions"}) {
    const json::Value* items = j.is_object() ? j.find(list) : nullptr;
    if (!items || !items->is_array()) continue;
    for (const json::Value& x : items->items()) {
      const json::Value* from = x.is_object() ? x.find("from") : nullptr;
      if (!from || !from->is_string() || !is_engine_library(from->as_string())) continue;
      const std::string what = std::string(list) == "structs" ? "struct" : std::string(list) == "enums" ? "enum" : "function";
      if (error)
        *error = "the " + what + " " + x["name"].as_string() + " is a copy of the engine's library " + from->as_string() +
                 " - a level holds none: the Blueprint has the engine's own " + what + " through its uses";
      return false;
    }
  }
  return graph_from_json(j, out, error);
}

bool hold_engine(Graph& g, const Blueprints& project, std::string* error) {
  std::vector<std::string> libraries;
  const Blueprints all = with_engine(project);
  if (!libraries_of(g, all, libraries, nullptr)) return true;  // (a library that is not there: linking the Blueprint says so)
  if (std::none_of(libraries.begin(), libraries.end(), [](const std::string& l) { return is_engine_library(l); })) return true;
  return take_libraries(g, all, error);
}

}  // namespace kern::logic
