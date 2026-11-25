// Host tool: the Blueprints of a Kern level as C++ classes a game links.
//
//   tools/blueprint_cpp.sh <level.klevel> <out.cpp> [<out.h> <folder>]   (builds this file once, then runs it)
//   tools/blueprint_cpp.sh --apply <level.klevel> <actor | Level> <text.cpp> [<actor> <text.cpp> ...]
//                                                        (the other way: Blueprints from their C++ texts)
//   tools/blueprint_cpp.sh --node-names                  (the names the nodes have in a Blueprint's C++)
//   tools/blueprint_cpp.sh --classes <level.klevel> <out.h> <actor> [<actor> ...]
//                                                        (only the classes of these actors' Blueprints and of the
//                                                        libraries they use, without glue (only how their structs are
//                                                        held as values), named in the namespace
//                                                        `blueprints`: what a program that calls them directly, or a
//                                                        measurement of them (tools/blueprint_bench.sh), compiles)
//   tools/blueprint_cpp.sh --formula "tan(0.5)"          (what a formula without inputs gives, with every bit)
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
// With <out.h> and <folder>: the Blueprints of the level's folder <folder> are those the program's
// own C++ calls directly, as their classes (kern/logic/blueprint_class.h). Their classes - and those
// of the libraries they use - are written into <out.h> instead (<out.cpp> includes it and has their
// glue), each once, and named there by their class names in the namespace `blueprints`
// (blueprints::qr_code_cpp), with blueprints::bind(Runtime&): it binds each of them to the object the
// runtime runs it as. The program calls their functions on that object, typed and directly, with
// the types they declare - no value is packed, nothing is looked up by name.
//
// <out.cpp> and <out.h> are only rewritten when they change, so nothing recompiles for an unchanged level.
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "kern/core/json.h"
#include "kern/game/logic_nodes.h"
#include "kern/logic/engine_libraries.h"
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
  if (held->is_object() && held->find("nodes")) logic::graph_from_level_json(*held, before, &error);
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
        if (!logic::graph_from_level_json(*logic, *found)) found.reset();
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
  *held = logic::graph_to_level_json(g);
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
  // (the run that writes the engine's libraries: its level holds the new ones - kern/logic/engine_libraries.h)
  if (const char* off = std::getenv("KERN_ENGINE_LIBRARIES"); off && std::string(off) == "off") logic::engine_libraries_off();
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
  if (argc == 3 && std::string(argv[1]) == "--formula") {
    // (what a formula without inputs gives: the number as the language computes it - its own sin, cos, tan ... -,
    // said with every bit: C's hexadecimal notation of a double)
    const logic::Formula f(argv[2]);
    if (!f.ok() || !f.inputs().empty()) {
      std::fprintf(stderr, "%s: %s\n", argv[2], f.ok() ? "a formula without inputs is asked for" : f.error().c_str());
      return 1;
    }
    std::printf("%a\n", f.evaluate(nullptr, 0));
    return 0;
  }
  // --engine-classes: the classes of the engine's own libraries, once for every program (kern/logic/engine_libraries.h):
  // the header every level's classes include, and the glue the engine links
  const bool engine_classes = argc == 4 && std::string(argv[1]) == "--engine-classes";
  if (engine_classes) argv += 1, argc -= 1;
  // --classes: only these actors' Blueprints and the libraries they use, as classes without glue
  std::set<std::string> only;
  const bool classes_only = argc >= 5 && std::string(argv[1]) == "--classes";
  if (classes_only) {
    for (int k = 4; k < argc; ++k) only.insert(argv[k]);
    argv += 1, argc -= 1;
  }
  if (!engine_classes && (argc < 3 || (!classes_only && argc != 3 && argc != 5) || std::string(argv[1]) == "--apply")) {
    std::fprintf(stderr, "usage: blueprint_cpp <level.klevel> <out.cpp> [<out.h> <folder>]\n       blueprint_cpp --classes <level.klevel> <out.h> <actor> [<actor> ...]\n       blueprint_cpp --apply <level.klevel> <actor | Level> <text.cpp> [<actor> <text.cpp> ...]\n       blueprint_cpp --engine-classes <out.cpp> <out.h>\n       blueprint_cpp --node-names\n       blueprint_cpp --formula <formula without inputs>\n");
    return 2;
  }
  // (the header of the Blueprints the program calls by their classes, and the folder they stand in)
  const char* header_path = engine_classes ? argv[2] : !classes_only && argc == 5 ? argv[3] : nullptr;
  const std::string called_folder = engine_classes ? std::string() : header_path ? argv[4] : "";
  std::string text, why;
  json::Value doc;
  if (engine_classes) {
    // (a level of the engine's libraries alone: each one an actor, all of them in the header)
    doc = json::Value::object();
    json::Value actors = json::Value::array();
    for (const std::string& n : logic::engine_library_names()) {
      json::Value a = json::Value::object(), c = json::Value::object();
      a.set("name", n);
      a.set("folder", "Engine");
      c.set("logic", logic::graph_to_json(*logic::engine_library(n)));
      a.set("components", std::move(c));
      actors.push(std::move(a));
    }
    doc.set("actors", std::move(actors));
  } else if (!read_file(argv[1], text) || !json::parse(text, doc, &why)) {
    std::fprintf(stderr, "%s: %s\n", argv[1], why.empty() ? "cannot read it" : why.c_str());
    return 1;
  }
  // (owner as the file says it, the graph)
  std::vector<std::pair<std::string, const json::Value*>> found;
  std::set<std::string> called;  // (the actors of the folder whose Blueprints the program calls by their classes)
  if (const json::Value* g = doc.find("logic"); g && g->is_object()) found.push_back({"the level", g});
  for (const json::Value& a : doc["actors"].items())
    if (const json::Value* g = a["components"].find("logic"); g && g->is_object()) {
      found.push_back({"actor \"" + a["name"].as_string() + "\"", g});
      if (header_path && (engine_classes || a["folder"].as_string() == called_folder)) called.insert(a["name"].as_string());
    }

  std::string name = argv[1];
  if (const std::size_t slash = name.find_last_of("/\\"); slash != std::string::npos) name.erase(0, slash + 1);
  std::string out = "// Generated from " + name + " by tools/blueprint_cpp - do not edit: these are the level's Blueprints\n"
                    "// as C++ classes (kern/docs/blueprint_cpp.md). Change the Blueprint in the level; the build writes\n"
                    "// this file again.\n"
                    "#include \"kern/logic/blueprint.h\"\n"
                    "#include \"kern/logic/blueprint_class.h\"  // (the exports of Blueprints: KERN_EXPORT_AS)\n";
  std::string header;  // (with <out.h>: the classes the program calls, and the libraries they use)
  if (header_path) {
    std::string included = header_path;
    if (const std::size_t slash = included.find_last_of("/\\"); slash != std::string::npos) included.erase(0, slash + 1);
    if (engine_classes) included = "kern/logic/engine_libraries.gen.h";
    out += "#include \"" + included + "\"\n";
    header = "// Generated from " + name + " by tools/blueprint_cpp - do not edit: the Blueprints of the level's folder\n"
             "// \"" + called_folder + "\" - those the program's own C++ calls directly, as their classes - and the libraries\n"
             "// they use (kern/logic/blueprint_class.h, kern/docs/blueprint_cpp.md). Change the Blueprint in the level;\n"
             "// the build writes this file again.\n"
             "#pragma once\n\n"
             "#include \"kern/logic/blueprint.h\"\n"
             "#include \"kern/logic/blueprint_class.h\"\n"
             "#include \"kern/logic/engine_libraries.gen.h\"  // (the classes of the engine's own libraries)\n";
    if (engine_classes)
      header = "// The classes of the engine's own libraries (kern/logic/engine_libraries.h), once for every program: written by\n"
               "// tools/blueprint_cpp --engine-classes (tools/engine_api.py --libraries) - not edited by hand. The classes of a\n"
               "// level's Blueprints include this header and name these classes; none writes them again.\n"
               "#pragma once\n\n"
               "#include \"kern/logic/blueprint.h\"\n"
               "#include \"kern/logic/blueprint_class.h\"\n";
    if (engine_classes)
      out = "// The glue of the classes of the engine's own libraries (kern/logic/engine_libraries.gen.h): written by\n"
            "// tools/blueprint_cpp --engine-classes - not edited by hand.\n"
            "#include \"kern/logic/blueprint.h\"\n#include \"kern/logic/blueprint_class.h\"\n#include \"kern/logic/engine_libraries.gen.h\"\n";
  }
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
    if (!(engine_classes ? logic::graph_from_json(*json, h.g, &error) : logic::graph_from_level_json(*json, h.g, &error))) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    if (h.g.empty()) continue;
    if (!engine_classes) logic::cpp_names(h.g);  // (as the level loader does for levels from before names had to be C++ names)
    held.push_back(std::move(h));
  }
  // The engine's libraries are the engine's (kern/logic/engine_libraries.h): no actor of a level is one, a
  // library of the engine a Blueprint uses is there without an actor, and each Blueprint is given what it uses
  // of them - as the level loader does. Their classes are written from the engine's graphs.
  // The classes of the engine's libraries are the engine's (blueprint_cpp --engine-classes ->
  // kern/logic/engine_libraries.gen.h, which the header includes): here they are named, not written again.
  std::map<std::string, std::string> engine_classes_of;
  if (!engine_classes) {
    for (const Held& h : held)
      if (h.owner != "the level" && logic::is_engine_library(h.name)) {
        std::fprintf(stderr, "%s: the Blueprint of %s is called like the engine's library %s - the engine's libraries are the engine's, a level does not hold them\n",
                     argv[1], h.owner.c_str(), h.name.c_str());
        return 1;
      }
    const logic::Blueprints project = [&](std::string_view name) -> const logic::Graph* {
      for (const Held& h : held)
        if (h.owner != "the level" && h.name == name) return &h.g;
      return nullptr;
    };
    for (Held& h : held) {
      std::string why;
      if (!logic::hold_engine(h.g, project, &why)) {
        std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], h.owner.c_str(), why.c_str());
        return 1;
      }
    }
    for (const std::string& n : logic::engine_library_names()) {
      const logic::Graph& g = *logic::engine_library(n);
      logic::Graph whole;
      std::string why;
      if (!logic::native_ready(g, &why) || !logic::link(g, project, whole, &why) || !logic::native_ready(whole, &why)) continue;  // (one with no class: a Blueprint that uses it runs in the interpreter, and says why below)
      const std::string cls = logic::cpp_class_name(n, g);
      u64 key = logic::graph_hash(whole);
      for (unsigned char ch : cls) key = (key ^ ch) * 0x100000001b3ull;
      char space[48];
      std::snprintf(space, sizeof space, "kern_blueprint_%016llx", static_cast<unsigned long long>(key));
      engine_classes_of[n] = std::string("::") + space + "::" + cls;
    }
  }
  const logic::Blueprints all = [&](std::string_view name) -> const logic::Graph* {
    for (const Held& h : held)
      if (h.owner != "the level" && h.name == name) return &h.g;
    return nullptr;
  };
  if (classes_only) {  // (the actors asked for and, through `uses`, every library they need)
    for (std::vector<std::string> todo(only.begin(), only.end()); !todo.empty();) {
      const std::string next = todo.back();
      todo.pop_back();
      const logic::Graph* g = all(next);
      if (!g) {
        std::fprintf(stderr, "%s: no actor %s with a Blueprint\n", argv[1], next.c_str());
        return 1;
      }
      for (const std::string& used : g->uses)
        if (only.insert(used).second) todo.push_back(used);
    }
    std::erase_if(held, [&](const Held& h) { return h.owner == "the level" || !only.count(h.name); });
    out = "// Generated from " + name + " by tools/blueprint_cpp --classes - do not edit: the classes of Blueprints of the\n"
          "// level (kern/docs/blueprint_cpp.md), without glue, named in the namespace blueprints.\n"
          "#pragma once\n#include \"kern/logic/blueprint.h\"\n";
  }
  std::string aliases;  // (--classes: every class by its name)
  std::map<std::string, std::string> classes_of;  // a library that is written: its class, with its namespace
  classes_of.insert(engine_classes_of.begin(), engine_classes_of.end());
  std::set<u64> written;
  int classes = 0;
  // What goes into the header: the Blueprints of the folder and, before them, the libraries they use - and
  // those the libraries use (by their names, as `uses` says them).
  std::set<std::string> in_header;
  for (std::vector<std::string> todo(called.begin(), called.end()); !todo.empty();) {
    const std::string next = todo.back();
    todo.pop_back();
    if (!in_header.insert(next).second) continue;
    if (const logic::Graph* g = all(next)) todo.insert(todo.end(), g->uses.begin(), g->uses.end());
  }
  std::vector<std::pair<std::string, std::string>> named;  // (in the header: a class's name, and the class with its namespace)
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
    const bool to_header = h.owner != "the level" && in_header.count(h.name);
    if (!logic::native_ready(g, &why) || !logic::link(g, all, whole, &why) || !logic::native_ready(whole, &why)) {
      if (classes_only || to_header) {  // (asked for as a class, or the program calls it as its class: there must be one)
        std::fprintf(stderr, "%s: the Blueprint of %s %s: %s\n", argv[1], owner.c_str(), to_header ? "is called by the program as its class, but it has none" : "has no class", why.c_str());
        return 1;
      }
      out += ": runs in the interpreter -\n// " + why + "\n";
      continue;
    }
    std::string uses;
    for (const std::string& used : g.uses) {
      if (!classes_of.count(used)) why = "its library " + used + " is not written as a class";
      else uses += "using " + used + " = " + classes_of[used] + ";\n";
    }
    if (std::any_of(g.uses.begin(), g.uses.end(), [&](const std::string& used) { return !classes_of.count(used); })) {
      if (to_header) {
        std::fprintf(stderr, "%s: the Blueprint of %s is called by the program as its class, but %s\n", argv[1], owner.c_str(), why.c_str());
        return 1;
      }
      out += ": runs in the interpreter -\n// " + why + "\n";
      continue;
    }
    const std::string cls = logic::cpp_class_name(h.name, g);  // (each class has a namespace of its own, below)
    // (a namespace of its own, named by the graph's hash and the class's name: the class meets no other
    // class of that name - not the game's, not another level's; two libraries whose graphs are the same
    // - both empty - are two classes, each by its own name)
    u64 key = logic::graph_hash(whole);
    for (unsigned char ch : cls) key = (key ^ ch) * 0x100000001b3ull;
    char space[48];
    std::snprintf(space, sizeof space, "kern_blueprint_%016llx", static_cast<unsigned long long>(key));
    if (g.library) classes_of[h.name] = std::string("::") + space + "::" + cls;
    if (classes_only) aliases += "using " + cls + " = ::" + space + "::" + cls + ";\n";
    if (to_header) named.push_back({cls, std::string("::") + space + "::" + cls});
    if (!written.insert(key).second) {
      out += ": the same Blueprint as one above.\n";
      continue;
    }
    std::string code, glue;
    if (!logic::to_cpp(g, cls, code, &error) || !logic::native_glue(g, cls, glue, &error, logic::library(), space, all, classes_only)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    if (to_header) {  // (the class in the header, its glue here)
      header += "\n// ---------------------------------------------------------------------------\n// The Blueprint of " + owner + "\n" +
                std::string("namespace ") + space + " {\n\n" + uses + (uses.empty() ? "" : "\n") + code + "\n}  // namespace " + space + "\n";
      out += " (its class: in the header)\n" + std::string("namespace ") + space + " {\n\n" + glue + "\n}  // namespace " + space + "\n";
    } else {
      out += std::string("\nnamespace ") + space + " {\n\n" + uses + (uses.empty() ? "" : "\n") + code + "\n" + glue + "\n}  // namespace " + space + "\n";
    }
    ++classes;
  }
  if (classes_only) out += "\nnamespace blueprints {\n" + aliases + "}  // namespace blueprints\n";
  if (header_path && !engine_classes) {
    for (const std::string& c : called)
      if (std::none_of(held.begin(), held.end(), [&](const Held& h) { return h.owner != "the level" && h.name == c; })) {
        std::fprintf(stderr, "%s: the actor %s of the folder %s has no Blueprint\n", argv[1], c.c_str(), called_folder.c_str());
        return 1;
      }
    // The classes by their names, and how the program binds them (each to the object the runtime runs it as).
    header += "\n// ---------------------------------------------------------------------------\n"
              "// The classes by their names: the program's C++ calls their functions on the object blueprints::bind\n"
              "// binds (kern::logic::blueprint<blueprints::Name>()), with the types they declare.\n"
              "namespace blueprints {\n\n";
    for (const auto& [cls, full] : named) header += "using " + cls + " = " + full + ";\n";
    header += "\n/// Binds each class above to the object `logic` runs its Blueprint as (kern::logic::bind_blueprint) - once the\n"
              "/// level's logic has started. False: one of them has no Blueprint there that runs as its class.\n"
              "bool bind(kern::logic::Runtime& logic);\n\n}  // namespace blueprints\n";
    out += "\n// ---------------------------------------------------------------------------\n"
           "// How the program binds the classes of the header (blueprints::bind).\n"
           "bool blueprints::bind(kern::logic::Runtime& logic) {\n  (void)logic;\n  bool all = true;\n";
    for (const auto& [cls, full] : named) out += "  all = kern::logic::bind_blueprint<" + cls + ">(logic) && all;\n";
    out += "  return all;\n}\n";
  }
  auto write = [](const char* path, const std::string& text) {
    std::string before;
    if (read_file(path, before) && before == text) return true;
    FILE* f = std::fopen(path, "wb");
    if (!f || std::fwrite(text.data(), 1, text.size(), f) != text.size() || std::fclose(f) != 0) {
      std::fprintf(stderr, "cannot write %s\n", path);
      return false;
    }
    return true;
  };
  if (header_path && !write(header_path, header)) return 1;
  const char* out_path = engine_classes ? argv[1] : argv[2];
  if (!write(out_path, out)) return 1;
  std::printf("%s: %d of %zu Blueprint(s) as C++ classes", out_path, classes, found.size());
  if (header_path) std::printf(" - %zu of them in %s, which the program calls (folder %s) and the libraries they use", named.size(), header_path, called_folder.c_str());
  std::printf("\n");
  return 0;
}
