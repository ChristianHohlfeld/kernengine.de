// The project file (kern/game/project.h, kern/docs/project.md).
#include "kern/game/project.h"

#include "kern/game/level.h"

namespace kern::game {
namespace {
bool fail(std::string* error, std::string text) {
  if (error) *error = std::move(text);
  return false;
}
std::string place(std::size_t i, const std::string& name) {
  std::string s = "blueprints[" + std::to_string(i) + "]";
  if (!name.empty()) s += " \"" + name + "\"";
  return s;
}
}  // namespace

ProgramBlueprint* Project::blueprint(std::string_view name) {
  for (ProgramBlueprint& b : blueprints)
    if (b.name == name) return &b;
  return nullptr;
}
const ProgramBlueprint* Project::blueprint(std::string_view name) const {
  for (const ProgramBlueprint& b : blueprints)
    if (b.name == name) return &b;
  return nullptr;
}
std::string Project::first_level() const {
  if (!start_level.empty()) return start_level;
  return levels.empty() ? std::string() : levels.front();
}

const std::string* Project::level_text(std::string_view path, std::string* error) const {
  bool named = false;
  std::string all;
  for (const std::string& l : levels) {
    named |= l == path;
    all += (all.empty() ? "" : ", ") + l;
  }
  if (!named) {
    if (error) *error = "the project " + name + " has no level " + std::string(path) + " - its levels: " + (all.empty() ? "none" : all);
    return nullptr;
  }
  for (const auto& [p, text] : level_texts)
    if (p == path) return &text;
  if (error) *error = "the level " + std::string(path) + " of the project " + name + " was not loaded with it (Project::level_texts)";
  return nullptr;
}

json::Value to_json(const Project& p) {
  json::Value o = json::Value::object();
  o.set("format", "kern-project");
  o.set("version", 1);
  o.set("name", p.name);
  o.set("settings", project_to_json(p.settings));
  json::Value levels = json::Value::array();
  for (const std::string& l : p.levels) levels.push(l);
  o.set("levels", std::move(levels));
  if (!p.start_level.empty()) o.set("startLevel", p.start_level);
  json::Value bps = json::Value::array();
  for (const ProgramBlueprint& b : p.blueprints) {
    json::Value x = json::Value::object();
    x.set("name", b.name);
    if (!b.folder.empty()) x.set("folder", b.folder);
    x.set("logic", logic::graph_to_json(b.logic));
    bps.push(std::move(x));
  }
  o.set("blueprints", std::move(bps));
  return o;
}

std::string to_text(const Project& p, i32 indent) { return json::stringify(to_json(p), indent) + "\n"; }

bool from_json(const json::Value& doc, Project& out, std::string* error) {
  if (!doc.is_object()) return fail(error, "a project must be a JSON object");
  if (doc["format"].as_string() != "kern-project") return fail(error, "format: not a Kern project (format must be \"kern-project\")");
  if (doc["version"].as_number(0) > 1) return fail(error, "version: the project was written by a newer Kern Studio");
  Project p;
  if (doc["name"].is_string()) p.name = doc["name"].as_string();
  if (const json::Value* s = doc.find("settings")) {
    std::string why;
    if (!project_from_json(*s, p.settings, &why)) return fail(error, why);
  }
  if (const json::Value* ls = doc.find("levels")) {
    if (!ls->is_array()) return fail(error, "levels: must be a list of paths");
    for (std::size_t i = 0; i < ls->size(); ++i) {
      const json::Value& l = (*ls)[i];
      const std::string at = "levels[" + std::to_string(i) + "]";
      if (!l.is_string() || l.as_string().empty()) return fail(error, at + ": must be the path of a level");
      for (std::size_t j = 0; j < p.levels.size(); ++j)
        if (p.levels[j] == l.as_string()) return fail(error, at + " \"" + l.as_string() + "\": named twice (also levels[" + std::to_string(j) + "])");
      p.levels.push_back(l.as_string());
    }
  }
  if (const json::Value* s = doc.find("startLevel")) {
    if (!s->is_string()) return fail(error, "startLevel: must be the path of one of the levels");
    p.start_level = s->as_string();
    bool known = false;
    for (const std::string& l : p.levels) known |= l == p.start_level;
    if (!known) return fail(error, "startLevel \"" + p.start_level + "\": is none of the project's levels");
  }
  if (const json::Value* bs = doc.find("blueprints")) {
    if (!bs->is_array()) return fail(error, "blueprints: must be a list");
    for (std::size_t i = 0; i < bs->size(); ++i) {
      const json::Value& b = (*bs)[i];
      if (!b.is_object()) return fail(error, place(i, "") + ": must be an object {name, logic}");
      ProgramBlueprint bp;
      bp.name = b["name"].is_string() ? b["name"].as_string() : std::string();
      if (bp.name.empty()) return fail(error, place(i, "") + ": has no name");
      if (const json::Value* f = b.find("folder")) {
        if (!f->is_string()) return fail(error, place(i, bp.name) + ": folder: must be a name (a text)");
        bp.folder = f->as_string();
      }
      for (std::size_t j = 0; j < p.blueprints.size(); ++j)
        if (p.blueprints[j].name == bp.name)
          return fail(error, place(i, bp.name) + ": the name stands twice (also blueprints[" + std::to_string(j) + "])");
      std::string why;
      if (!b["logic"].is_object()) return fail(error, place(i, bp.name) + ": has no logic (its Blueprint)");
      if (!logic::graph_from_json(b["logic"], bp.logic, &why)) return fail(error, place(i, bp.name) + ": " + why);
      p.blueprints.push_back(std::move(bp));
    }
  }
  out = std::move(p);
  return true;
}

bool from_text(std::string_view text, Project& out, std::string* error) {
  json::Value v;
  if (!json::parse(text, v)) return fail(error, "the project is not JSON");
  return from_json(v, out, error);
}

logic::Blueprints program_blueprints(const Project& project) {
  return [&project](std::string_view name) -> const logic::Graph* {
    const ProgramBlueprint* b = project.blueprint(name);
    return b ? &b->logic : nullptr;
  };
}

logic::Blueprints world_blueprints(const Level& level, const Project* project) {
  return [&level, project](std::string_view name) -> const logic::Graph* {
    for (const Actor& a : level.actors)
      if (a.logic && a.name == name) return &*a.logic;
    if (project)
      if (const ProgramBlueprint* b = project->blueprint(name)) return &b->logic;
    return nullptr;
  };
}

std::vector<std::string> library_conflicts(const Level& level, const Project& project) {
  std::vector<std::string> out;
  for (std::size_t i = 0; i < project.blueprints.size(); ++i)
    for (const Actor& a : level.actors)
      if (a.logic && a.name == project.blueprints[i].name) {
        out.push_back("the Blueprint " + a.name + " stands in the level (actor " + std::to_string(a.id) + ") and in the project (" +
                      place(i, "") + ") - rename one");
        break;
      }
  return out;
}

}  // namespace kern::game
