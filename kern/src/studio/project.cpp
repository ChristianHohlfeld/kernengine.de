// Kern Studio: the project the open level belongs to (kern/game/project.h, kern/docs/project.md) - its name, its
// levels and start level, its settings and the program's Blueprints. The program's Blueprints are edited in the
// logic editor like a level's (logicOpen {program}); the commands here change the project as a whole:
//
//   projectNew {name}                 a new project with one empty level, levels/start.klevel (opened)
//   projectOpen {text}                a .kproject (errors say where and why; nothing changes then)
//   projectSave -> {text, file}       the project as text and its file name; projectSaved when it is written
//   projectSet {name, levels, startLevel, settings}   any of them (read like the file: the same checks; null: not named)
//   projectBlueprint {op: add|remove|rename, name, to, library}   a program Blueprint
//
// Every change is a step of the undo history (HistoryEntry::project).

#include <cctype>

#include "kern/game/demos.h"
#include "kern/studio/studio.h"

namespace kern::studio {

namespace {
json::Value ok_result(bool v = true) {
  json::Value r = json::Value::object();
  r.set("ok", v);
  return r;
}
json::Value fail_result(const std::string& text) {
  json::Value r = json::Value::object();
  r.set("ok", false);
  r.set("error", text);
  return r;
}
std::string file_name(const std::string& name) {
  std::string file = name.empty() ? "project" : name;
  for (char& ch : file)
    if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != '_') ch = '_';
  return file;
}
/// Which Blueprints use `name` as a library - of the project, and of the level ("" when none): a rename or a removal
/// would leave them without it.
std::string users_of(const game::Project& p, const game::Level& level, const std::string& name) {
  std::string users;
  for (const game::ProgramBlueprint& b : p.blueprints)
    if (b.name != name && b.logic.uses_library(name)) users += (users.empty() ? "" : ", ") + b.name;
  for (const game::Actor& a : level.actors)
    if (a.logic && a.logic->uses_library(name)) users += (users.empty() ? "" : ", ") + a.name + " (in the level)";
  return users;
}
}  // namespace

const game::Level& Studio::logic_level() const {
  if (!logic_program_) return level_;
  if (program_view_stale_) {
    program_view_ = game::Level{};
    program_view_.name = project_.name;
    for (std::size_t i = 0; i < project_.blueprints.size(); ++i) {
      game::Actor a;
      a.id = static_cast<ActorId>(i + 1);
      a.name = project_.blueprints[i].name;
      a.logic = project_.blueprints[i].logic;
      program_view_.actors.push_back(std::move(a));
    }
    program_view_.next_id = static_cast<ActorId>(project_.blueprints.size() + 1);
    program_view_stale_ = false;
  }
  return program_view_;
}

bool Studio::load_project_text(std::string_view text, std::string* error) {
  game::Project p;
  std::string why;
  if (!game::from_text(text, p, &why)) {
    report("project: " + why, "error");
    if (error) *error = why;
    return false;
  }
  if (const std::vector<std::string> conflicts = game::library_conflicts(level_, p); !conflicts.empty()) {
    report("project: " + conflicts.front(), "error");
    if (error) *error = conflicts.front();
    return false;
  }
  project_ = std::move(p);
  project_rev_ = project_saved_rev_ = next_project_rev_++;
  program_view_stale_ = true;
  if (logic_program_) logic_open_ = false;
  logic_program_ = false;
  load_level(level_);  // a new history: its first step holds this project
  report("Opened project " + project_.name);
  return true;
}

json::Value Studio::project_json() const {
  json::Value o = json::Value::object();
  o.set("name", project_.name);
  o.set("dirty", project_dirty());
  json::Value levels = json::Value::array();
  for (const std::string& l : project_.levels) levels.push(l);
  o.set("levels", std::move(levels));
  o.set("startLevel", project_.first_level());
  o.set("settings", project_to_json(project_.settings));
  json::Value bps = json::Value::array();
  for (std::size_t i = 0; i < project_.blueprints.size(); ++i) {
    const game::ProgramBlueprint& b = project_.blueprints[i];
    json::Value x = json::Value::object();
    x.set("name", b.name);
    x.set("library", b.logic.library);
    x.set("implements", b.logic.implements);
    x.set("nodes", static_cast<u32>(b.logic.nodes.size()));
    x.set("open", logic_open_ && logic_program_ && logic_owner_ == i + 1);
    bps.push(std::move(x));
  }
  o.set("blueprints", std::move(bps));
  return o;
}

json::Value Studio::project_command(const std::string& name, const json::Value& c) {
  auto str = [&](const char* k) -> std::string { return c[k].is_string() ? c[k].as_string() : std::string(); };
  if (!editable() && name != "projectSave" && name != "projectSaved") return fail_result("Stop playing to change the project");
  if (name == "projectNew") {
    game::Project p;
    if (!str("name").empty()) p.name = str("name");
    p.levels = {"levels/start.klevel"};
    p.start_level = p.levels.front();
    game::Level start = game::empty_level();
    start.name = "start";
    project_ = std::move(p);
    project_rev_ = next_project_rev_++;  // (not written yet: dirty)
    program_view_stale_ = true;
    if (logic_program_) logic_open_ = false;
    logic_program_ = false;
    load_level(std::move(start));
    report("New project: " + project_.name);
    return ok_result();
  }
  if (name == "projectOpen") {
    std::string error;
    if (!load_project_text(str("text"), &error)) return fail_result(error);
    return ok_result();
  }
  if (name == "projectSave") {
    json::Value r = ok_result();
    r.set("text", project_text());
    r.set("file", file_name(project_.name) + ".kproject");
    return r;
  }
  if (name == "projectSaved") {
    project_saved_rev_ = project_rev_;
    report("Saved project " + project_.name);
    touch();
    return ok_result();
  }
  if (name == "projectSet") {
    // Read as the file is: the project with the given members replaced, through game::from_json - one set of checks.
    json::Value doc = game::to_json(project_);
    for (const char* k : {"name", "levels", "startLevel", "settings"})
      if (const json::Value* v = c.find(k)) {
        if (v->is_null()) doc.remove(k);  // (startLevel null: none named - the first level starts)
        else doc.set(k, *v);
      }
    game::Project p;
    std::string error;
    if (!game::from_json(doc, p, &error)) return fail_result(error);
    if (const std::vector<std::string> conflicts = game::library_conflicts(level_, p); !conflicts.empty()) return fail_result(conflicts.front());
    return ok_result(edit("Edit Project", [&](game::Level&) {
      project_ = std::move(p);
      program_view_stale_ = true;
      project_touched_ = true;
      return true;
    }));
  }
  if (name == "projectBlueprint") {
    const std::string op = str("op"), which = str("name"), to = str("to");
    game::ProgramBlueprint* b = project_.blueprint(which);
    auto taken = [&](const std::string& n) -> std::string {
      if (n.empty()) return "a program Blueprint needs a name";
      if (project_.blueprint(n)) return "the project has a Blueprint \"" + n + "\" already";
      for (const game::Actor& a : level_.actors)
        if (a.logic && a.name == n)
          return "the Blueprint " + n + " stands in the level (actor " + std::to_string(a.id) + ") - a name stands in one place only";
      return {};
    };
    if (op == "add") {
      if (const std::string why = taken(which); !why.empty()) return fail_result(why);
      const bool library = c["library"].as_bool(false);
      return ok_result(edit("Add Program Blueprint", [&](game::Level&) {
        game::ProgramBlueprint nb;
        nb.name = which;
        nb.logic.library = library;
        project_.blueprints.push_back(std::move(nb));
        program_view_stale_ = true;
        project_touched_ = true;
        return true;
      }));
    }
    if (!b) return fail_result("the project has no Blueprint \"" + which + "\"");
    if (op == "rename") {
      if (const std::string why = taken(to); !why.empty()) return fail_result(why);
      if (const std::string users = users_of(project_, level_, which); !users.empty())
        return fail_result("the Blueprint " + which + " is a library of " + users + ": they name it - change them first");
      return ok_result(edit("Rename Program Blueprint", [&](game::Level&) {
        project_.blueprint(which)->name = to;
        program_view_stale_ = true;
        project_touched_ = true;
        return true;
      }));
    }
    if (op == "remove") {
      if (const std::string users = users_of(project_, level_, which); !users.empty())
        return fail_result("the Blueprint " + which + " is a library of " + users + ": they need it");
      std::size_t at = static_cast<std::size_t>(b - project_.blueprints.data());
      const bool done = edit("Remove Program Blueprint", [&](game::Level&) {
        project_.blueprints.erase(project_.blueprints.begin() + static_cast<std::ptrdiff_t>(at));
        program_view_stale_ = true;
        project_touched_ = true;
        return true;
      });
      if (done && logic_program_) {
        if (logic_owner_ == at + 1) logic_open_ = false, logic_program_ = false, logic_owner_ = 0;
        else if (logic_owner_ > at + 1) --logic_owner_;
      }
      return ok_result(done);
    }
    return fail_result("projectBlueprint: op must be add, remove or rename (not \"" + op + "\")");
  }
  return fail_result("unknown command " + name);
}

}  // namespace kern::studio
