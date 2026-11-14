// Host tool: the Blueprints of a Kern level as C++ classes a game links.
//
//   tools/blueprint_cpp.sh <level.klevel> <out.cpp>      (builds this file once, then runs it)
//   tools/blueprint_cpp.sh --apply <level.klevel> <actor | Level> <text.cpp> [<actor> <text.cpp> ...]
//                                                        (the other way: Blueprints from their C++ texts)
//   tools/blueprint_cpp.sh --node-names                  (the names the nodes have in a Blueprint's C++)
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
// A library - a Blueprint that keeps nothing - is written before the Blueprints that use it; in
// their namespace the library's class has the name they use it by. A Blueprint that uses
// libraries is registered under the hash of the one graph it is with them (logic::link): when a
// library changes, the Blueprints that use it change with it.
//
// <out.cpp> is only rewritten when it changes, so nothing recompiles for an unchanged level.
#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
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

/// In the level `doc`, the Blueprint of the actor called <actor> ("Level": the level's own)
/// becomes what the C++ text says - the text is the Blueprint's other notation
/// (kern/docs/blueprint_cpp.md), read as the editor's C++ view reads it (Apply). An actor of that
/// name that is not there yet is made, in the folder "Rules". 0: it is there; else the text is no
/// Blueprint that compiles, and what is wrong is said.
int apply_one(json::Value& doc, const char* level_path, const std::string& actor, const char* text_path) {
  std::string text, why;
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
  // (the libraries the text names: the Blueprints of the level's other actors, by their names)
  std::map<std::string, std::unique_ptr<logic::Graph>> others;
  const logic::Blueprints all = [&](std::string_view name) -> const logic::Graph* {
    auto known = others.find(std::string(name));
    if (known != others.end()) return known->second.get();
    std::unique_ptr<logic::Graph> found;
    if (name != actor)
      for (const json::Value& a : doc["actors"].items()) {
        const json::Value* logic = a["components"].find("logic");
        if (a["name"].as_string() != name || !logic || !logic->is_object()) continue;
        found = std::make_unique<logic::Graph>();
        if (!logic::graph_from_json(*logic, *found)) found.reset();
        break;
      }
    return (others[std::string(name)] = std::move(found)).get();
  };
  if (!(before.empty() ? logic::from_cpp(text, g, nullptr, &error, logic::library(), all) : logic::apply_cpp(text, before, g, &error, logic::library(), all))) {
    std::fprintf(stderr, "%s: %s\n", text_path, error.c_str());
    return 1;
  }
  int errors = 0;
  // (with its libraries it is one graph: the rules that follow calls judge that one)
  logic::Graph whole;
  if (!logic::link(g, all, whole, &error)) {
    std::fprintf(stderr, "%s: %s\n", text_path, error.c_str());
    return 1;
  }
  for (const logic::Issue& i : logic::check(whole)) {
    std::fprintf(stderr, "%s: %s%s%s\n", text_path, i.error ? "error: " : "warning: ", i.text.c_str(), i.node ? (" (node " + std::to_string(i.node) + ")").c_str() : "");
    errors += i.error ? 1 : 0;
  }
  if (errors) return 1;
  *held = logic::graph_to_json(g);
  std::printf("%s: the Blueprint of %s is %s (%zu nodes, %zu functions)\n", level_path, actor.c_str(), text_path, g.nodes.size(), g.functions.size());
  return 0;
}

/// --apply <level.klevel> <actor> <text.cpp> [<actor> <text.cpp> ...]: several Blueprints in the
/// order given - a library before the Blueprints that use it. The level is written when all of
/// them were read: all of them, or nothing.
int apply(const char* level_path, int count, char** pairs) {
  std::string level_text, why;
  json::Value doc;
  if (!read_file(level_path, level_text) || !json::parse(level_text, doc, &why)) {
    std::fprintf(stderr, "%s: %s\n", level_path, why.empty() ? "cannot read it" : why.c_str());
    return 1;
  }
  for (int k = 0; k + 1 < count; k += 2)
    if (apply_one(doc, level_path, pairs[k], pairs[k + 1]) != 0) return 1;
  const std::string out = json::stringify(doc, 1) + "\n";
  if (out != level_text) {
    FILE* f = std::fopen(level_path, "wb");
    if (!f || std::fwrite(out.data(), 1, out.size(), f) != out.size() || std::fclose(f) != 0) {
      std::fprintf(stderr, "cannot write %s\n", level_path);
      return 1;
    }
  }
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  (void)game::logic_node_shapes_registered();  // the gameplay nodes, as the library knows them
  if (argc >= 5 && argc % 2 == 1 && std::string(argv[1]) == "--apply") return apply(argv[2], argc - 3, argv + 3);
  if (argc == 2 && std::string(argv[1]) == "--node-names") {
    // (the names the nodes have in a Blueprint's C++ - math_add, list_get ...: nothing else of a Blueprint is called so)
    for (const auto& d : logic::library().all()) {
      std::string name = d->type;
      for (char& ch : name) if (ch == '.') ch = '_';
      std::printf("%s\n", name.c_str());
    }
    return 0;
  }
  if (argc < 3 || std::string(argv[1]) == "--apply") {
    std::fprintf(stderr, "usage: blueprint_cpp <level.klevel> <out.cpp>\n       blueprint_cpp --apply <level.klevel> <actor | Level> <text.cpp> [<actor> <text.cpp> ...]\n       blueprint_cpp --node-names\n");
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
  // Every Blueprint of the level, by the name the others use it by (an actor's name).
  struct Held {
    std::string owner, name;
    logic::Graph g;
    bool done = false;
  };
  std::vector<Held> held;
  for (const auto& [owner, json] : found) {
    Held h;
    h.owner = owner;
    h.name = owner == "the level" ? std::string("Level") : owner.substr(7, owner.size() - 8);
    std::string error;
    if (!logic::graph_from_json(*json, h.g, &error)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    if (h.g.empty()) continue;
    logic::cpp_names(h.g);  // (as the level loader does for levels from before names had to be C++ names)
    held.push_back(std::move(h));
  }
  const logic::Blueprints all = [&](std::string_view name) -> const logic::Graph* {
    for (const Held& h : held)
      if (h.owner != "the level" && h.name == name) return &h.g;
    return nullptr;
  };
  std::map<std::string, std::string> classes_of;  // a library that is written: its class, with its namespace
  std::set<u64> written;
  int classes = 0;
  // A library before the Blueprints that use it: what is written next is one whose libraries
  // are all written (or one that waits for nothing).
  for (std::size_t left = held.size(); left > 0;) {
    Held* next = nullptr;
    for (Held& h : held) {
      if (h.done) continue;
      bool ready = true;
      for (const std::string& used : h.g.uses) ready = ready && (classes_of.count(used) || !all(used) || std::any_of(held.begin(), held.end(), [&](const Held& o) { return o.name == used && o.done; }));
      if (ready) { next = &h; break; }
    }
    if (!next)  // (libraries that use each other: logic::link says so below)
      for (Held& h : held)
        if (!h.done) { next = &h; break; }
    Held& h = *next;
    h.done = true;
    --left;
    const logic::Graph& g = h.g;
    const std::string& owner = h.owner;
    std::string error;
    out += "\n// ---------------------------------------------------------------------------\n// The Blueprint of " + owner;
    // (with its libraries it is one graph: what runs, and what the rules that follow calls judge)
    logic::Graph whole;
    if (!logic::native_ready(g, &why) || !logic::link(g, all, whole, &why) || !logic::native_ready(whole, &why)) {
      out += ": runs in the interpreter -\n// " + why + "\n";
      continue;
    }
    std::string uses;
    for (const std::string& used : g.uses) {
      if (!classes_of.count(used)) why = "its library " + used + " is not written as a class";
      else uses += "using " + used + " = " + classes_of[used] + ";\n";
    }
    if (std::any_of(g.uses.begin(), g.uses.end(), [&](const std::string& used) { return !classes_of.count(used); })) {
      out += ": runs in the interpreter -\n// " + why + "\n";
      continue;
    }
    const std::string cls = logic::cpp_class_name(h.name, g);  // (each class has a namespace of its own, below)
    // (a namespace of its own, named by the graph's hash: the class meets no other class of that
    // name - not the game's, not another level's)
    char space[48];
    std::snprintf(space, sizeof space, "kern_blueprint_%016llx", static_cast<unsigned long long>(logic::graph_hash(whole)));
    if (g.library) classes_of[h.name] = std::string("::") + space + "::" + cls;
    if (!written.insert(logic::graph_hash(whole)).second) {
      out += ": the same Blueprint as one above.\n";
      continue;
    }
    std::string code, glue;
    if (!logic::to_cpp(g, cls, code, &error) || !logic::native_glue(g, cls, glue, &error, logic::library(), space, all)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    out += std::string("\nnamespace ") + space + " {\n\n" + uses + (uses.empty() ? "" : "\n") + code + "\n" + glue + "\n}  // namespace " + space + "\n";
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
