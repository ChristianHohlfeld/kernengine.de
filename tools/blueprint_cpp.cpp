// Host tool: the Blueprints of a Kern level as C++ classes a game links.
//
//   tools/blueprint_cpp.sh <level.klevel> <out.cpp>      (builds this file once, then runs it)
//   tools/blueprint_cpp.sh --apply <level.klevel> <actor | Level> <text.cpp>
//                                                        (the other way: a Blueprint from its C++ text)
//
// For every Blueprint of the level - the level's own and each actor's - that a C++ compiler can
// run as it stands (logic::native_ready), <out.cpp> gets its C++ text, exactly as logic::to_cpp
// writes it (kern/docs/blueprint_cpp.md), and the glue that makes the class reachable by name and
// registers it under the graph's hash (logic::native_glue). A game that links <out.cpp> runs
// these Blueprints as compiled code: logic::Runtime finds the class by the hash of the graph it
// is given.
//
// The level stays the one source. The class only ever runs the very graph it was written from: a
// Blueprint that was changed since has another hash and runs in the interpreter until the build
// writes this file again. Blueprints that are not ready yet are named in a comment, with what is
// missing; they run in the interpreter (or the JIT).
//
// <out.cpp> is only rewritten when it changes, so nothing recompiles for an unchanged level.
#include <cstdio>
#include <set>
#include <string>
#include <vector>

#include "kern/core/json.h"
#include "kern/game/logic_nodes.h"
#include "kern/logic/logic.h"

using namespace kern;

namespace {

bool read_file(const char* path, std::string& out) {
  FILE* f = std::fopen(path, "rb");
  if (!f) return false;
  char b[65536];
  std::size_t n;
  while ((n = std::fread(b, 1, sizeof b, f)) > 0) out.append(b, n);
  std::fclose(f);
  return true;
}

/// --apply <level.klevel> <actor> <text.cpp>: the Blueprint of the actor called <actor> ("Level":
/// the level's own) becomes what the C++ text says - the text is the Blueprint's other notation
/// (kern/docs/blueprint_cpp.md), read as the editor's C++ view reads it (Apply). An actor of that
/// name that is not there yet is made, in the folder "Rules". Nothing is written unless the text
/// is a Blueprint that compiles.
int apply(const char* level_path, const std::string& actor, const char* text_path) {
  std::string level_text, text, why;
  json::Value doc;
  if (!read_file(level_path, level_text) || !json::parse(level_text, doc, &why)) {
    std::fprintf(stderr, "%s: %s\n", level_path, why.empty() ? "cannot read it" : why.c_str());
    return 1;
  }
  if (!read_file(text_path, text)) {
    std::fprintf(stderr, "%s: cannot read it\n", text_path);
    return 1;
  }
  // (where the level holds that Blueprint: made empty where there is none yet)
  auto member = [](json::Value& of, const char* key, json::Value fresh) -> json::Value* {
    if (!of.find(key) || (fresh.is_object() && !of.find(key)->is_object()) || (fresh.is_array() && !of.find(key)->is_array())) of.set(key, std::move(fresh));
    return of.find(key);
  };
  json::Value* held = nullptr;
  if (actor == "Level") {
    held = member(doc, "logic", json::Value::object());
  } else {
    json::Value* actors = member(doc, "actors", json::Value::array());
    auto named = [&]() -> json::Value* {
      for (json::Value& a : actors->items())
        if (a["name"].as_string() == actor) return &a;
      return nullptr;
    };
    if (!named()) {
      const f64 id = doc["nextId"].as_number(1);
      json::Value a = json::Value::object(), transform = json::Value::object();
      auto triple = [](f64 v) {
        json::Value t = json::Value::array();
        for (int k = 0; k < 3; ++k) t.push(v);
        return t;
      };
      a.set("id", id);
      a.set("name", actor);
      a.set("folder", "Rules");
      transform.set("position", triple(0));
      transform.set("rotation", triple(0));
      transform.set("scale", triple(1));
      a.set("transform", std::move(transform));
      actors->push(std::move(a));
      doc.set("nextId", id + 1);
    }
    held = member(*member(*named(), "components", json::Value::object()), "logic", json::Value::object());
  }
  logic::Graph before, g;
  std::string error;
  if (held->is_object() && held->find("nodes")) logic::graph_from_json(*held, before, &error);
  if (!(before.empty() ? logic::from_cpp(text, g, nullptr, &error) : logic::apply_cpp(text, before, g, &error))) {
    std::fprintf(stderr, "%s: %s\n", text_path, error.c_str());
    return 1;
  }
  int errors = 0;
  for (const logic::Issue& i : logic::check(g)) {
    std::fprintf(stderr, "%s: %s%s%s\n", text_path, i.error ? "error: " : "warning: ", i.text.c_str(), i.node ? (" (node " + std::to_string(i.node) + ")").c_str() : "");
    errors += i.error ? 1 : 0;
  }
  if (errors) return 1;
  *held = logic::graph_to_json(g);
  const std::string out = json::stringify(doc, 1) + "\n";
  if (out != level_text) {
    FILE* f = std::fopen(level_path, "wb");
    if (!f || std::fwrite(out.data(), 1, out.size(), f) != out.size() || std::fclose(f) != 0) {
      std::fprintf(stderr, "cannot write %s\n", level_path);
      return 1;
    }
  }
  std::printf("%s: the Blueprint of %s is %s (%zu nodes, %zu functions)\n", level_path, actor.c_str(), text_path, g.nodes.size(), g.functions.size());
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  (void)game::logic_node_shapes_registered();  // the gameplay nodes, as the library knows them
  if (argc == 5 && std::string(argv[1]) == "--apply") return apply(argv[2], argv[3], argv[4]);
  if (argc < 3) {
    std::fprintf(stderr, "usage: blueprint_cpp <level.klevel> <out.cpp>\n       blueprint_cpp --apply <level.klevel> <actor | Level> <text.cpp>\n");
    return 2;
  }
  std::string text, why;
  json::Value doc;
  if (!read_file(argv[1], text) || !json::parse(text, doc, &why)) {
    std::fprintf(stderr, "%s: %s\n", argv[1], why.empty() ? "cannot read it" : why.c_str());
    return 1;
  }
  // (owner as the file says it, the graph)
  std::vector<std::pair<std::string, const json::Value*>> found;
  if (const json::Value* g = doc.find("logic"); g && g->is_object()) found.push_back({"the level", g});
  for (const json::Value& a : doc["actors"].items())
    if (const json::Value* g = a["components"].find("logic"); g && g->is_object()) found.push_back({"actor \"" + a["name"].as_string() + "\"", g});

  std::string name = argv[1];
  if (const std::size_t slash = name.find_last_of("/\\"); slash != std::string::npos) name.erase(0, slash + 1);
  std::string out = "// Generated from " + name + " by tools/blueprint_cpp - do not edit: these are the level's Blueprints\n"
                    "// as C++ classes (kern/docs/blueprint_cpp.md). Change the Blueprint in the level; the build writes\n"
                    "// this file again.\n"
                    "#include \"kern/logic/blueprint.h\"\n";
  std::set<u64> written;
  int classes = 0;
  for (const auto& [owner, json] : found) {
    logic::Graph g;
    std::string error;
    if (!logic::graph_from_json(*json, g, &error)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    if (g.empty()) continue;
    logic::cpp_names(g);  // (as the level loader does for levels from before names had to be C++ names)
    out += "\n// ---------------------------------------------------------------------------\n// The Blueprint of " + owner;
    if (!logic::native_ready(g, &why)) {
      out += ": runs in the interpreter -\n// " + why + "\n";
      continue;
    }
    if (!written.insert(logic::graph_hash(g)).second) {
      out += ": the same Blueprint as one above.\n";
      continue;
    }
    std::string label = owner == "the level" ? std::string("Level") : owner.substr(7, owner.size() - 8);
    const std::string cls = logic::cpp_class_name(label, g);  // (each class has a namespace of its own, below)
    // (a namespace of its own, named by the graph's hash: the class meets no other class of that
    // name - not the game's, not another level's)
    char space[48];
    std::snprintf(space, sizeof space, "kern_blueprint_%016llx", static_cast<unsigned long long>(logic::graph_hash(g)));
    std::string code, glue;
    if (!logic::to_cpp(g, cls, code, &error) || !logic::native_glue(g, cls, glue, &error, logic::library(), space)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    out += std::string("\nnamespace ") + space + " {\n\n" + code + "\n" + glue + "\n}  // namespace " + space + "\n";
    ++classes;
  }
  std::string before;
  if (!read_file(argv[2], before) || before != out) {
    FILE* f = std::fopen(argv[2], "wb");
    if (!f || std::fwrite(out.data(), 1, out.size(), f) != out.size() || std::fclose(f) != 0) {
      std::fprintf(stderr, "cannot write %s\n", argv[2]);
      return 1;
    }
  }
  std::printf("%s: %d of %zu Blueprint(s) as C++ classes\n", argv[2], classes, found.size());
  return 0;
}
