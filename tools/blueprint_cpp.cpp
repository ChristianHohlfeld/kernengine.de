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
// of the libraries they use - are reached through <out.h> instead (not written into <out.cpp>),
// each once, and named there by their class names in the namespace `blueprints`
// (blueprints::qr_code_cpp), with blueprints::bind(Runtime&): it binds each of them to the object the
// runtime runs it as. Each of these classes stands in a header of its own (<out.h without .h>/<namespace>.h, with how
// its structs are held as values), its glue in a source of its own (<out.cpp without .cpp>_<namespace>.cpp): every
// file compiles by itself, with what its class needs. The program calls their functions on that object, typed and directly, with
// the types they declare - no value is packed, nothing is looked up by name.
//
// <out.cpp> and <out.h> are only rewritten when they change, so nothing recompiles for an unchanged level.
#include <algorithm>
#include <cstdio>
#include <filesystem>
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
  // --classes: only these actors' Blueprints and the libraries they use, as classes without glue
  std::set<std::string> only;
  const bool classes_only = argc >= 5 && std::string(argv[1]) == "--classes";
  if (classes_only) {
    for (int k = 4; k < argc; ++k) only.insert(argv[k]);
    argv += 1, argc -= 1;
  }
  if (argc < 3 || (!classes_only && argc != 3 && argc != 5) || std::string(argv[1]) == "--apply") {
    std::fprintf(stderr, "usage: blueprint_cpp <level.klevel> <out.cpp> [<out.h> <folder>]\n       blueprint_cpp --classes <level.klevel> <out.h> <actor> [<actor> ...]\n       blueprint_cpp --apply <level.klevel> <actor | Level> <text.cpp> [<actor> <text.cpp> ...]\n       blueprint_cpp --node-names\n       blueprint_cpp --formula <formula without inputs>\n");
    return 2;
  }
  // (the header of the Blueprints the program calls by their classes, and the folder they stand in)
  const char* header_path = !classes_only && argc == 5 ? argv[3] : nullptr;
  const std::string called_folder = header_path ? argv[4] : "";
  std::string text, why;
  json::Value doc;
  if (!read_file(argv[1], text) || !json::parse(text, doc, &why)) {
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
      if (header_path && a["folder"].as_string() == called_folder) called.insert(a["name"].as_string());
    }

  std::string name = argv[1];
  if (const std::size_t slash = name.find_last_of("/\\"); slash != std::string::npos) name.erase(0, slash + 1);
  std::string out = "// Generated from " + name + " by tools/blueprint_cpp - do not edit: these are the level's Blueprints\n"
                    "// as C++ classes (kern/docs/blueprint_cpp.md). Change the Blueprint in the level; the build writes\n"
                    "// this file again.\n"
                    "#include \"kern/logic/blueprint.h\"\n"
                    "#include \"kern/logic/blueprint_class.h\"  // (the exports of Blueprints: KERN_EXPORT_AS)\n";
  std::string header;  // (with <out.h>: the classes the program calls, and the libraries they use)
  // With a header, each class of it is written into a header of its own (<out.h without .h>/<namespace>.h, which
  // includes those of the libraries it uses) and its glue into a source of its own (<out.cpp without .cpp>_<namespace>.cpp):
  // every file is compiled by itself and needs only what its class needs. <out.h> includes them all.
  std::string pieces_dir, pieces_include, glue_stem;
  std::vector<std::pair<std::string, std::string>> pieces;  // (path, text) of the headers and sources of the classes
  std::map<std::string, std::string> piece_of;              // a class of the header (by its Blueprint's name): its header, as included
  if (header_path) {
    std::string included = header_path;
    if (const std::size_t slash = included.find_last_of("/\\"); slash != std::string::npos) included.erase(0, slash + 1);
    pieces_include = included.substr(0, included.size() > 2 && included.ends_with(".h") ? included.size() - 2 : included.size());
    pieces_dir = std::string(header_path).substr(0, std::string(header_path).size() - included.size()) + pieces_include;
    glue_stem = argv[2];
    if (glue_stem.ends_with(".cpp")) glue_stem.resize(glue_stem.size() - 4);
    header = "// Generated from " + name + " by tools/blueprint_cpp - do not edit: the Blueprints of the level's folder\n"
             "// \"" + called_folder + "\" - those the program's own C++ calls directly, as their classes - and the libraries\n"
             "// they use (kern/logic/blueprint_class.h, kern/docs/blueprint_cpp.md). Change the Blueprint in the level;\n"
             "// the build writes this file again.\n"
             "#pragma once\n\n"
             "#include \"kern/logic/blueprint.h\"\n"
             "#include \"kern/logic/blueprint_class.h\"\n";
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
  std::vector<std::string> binds;                          // (the namespaces of the classes of the header, each once)
  std::set<std::string> out_includes;                      // (the headers of classes of the header that Blueprints of <out.cpp> use)
  const std::string header_path_name = header_path ? std::string(header_path).substr(std::string(header_path).find_last_of("/\\") + 1) : "";
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
      // How its structs are held as values (kern::logic::detail::Held) goes with the class into the header: every file of
      // the program that includes it compiles the class's functions, and those that hand a struct to a node of the engine
      // need it there. The glue here has it no more (each specialization once in the program).
      std::string held;
      if (!logic::native_glue(g, cls, held, &error, logic::library(), space, all, true)) {
        std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
        return 1;
      }
      if (!held.empty()) {
        const std::size_t at = glue.find(held);
        if (at == std::string::npos) {
          std::fprintf(stderr, "%s: the Blueprint of %s: its glue does not hold how its structs are held as values\n", argv[1], owner.c_str());
          return 1;
        }
        glue.erase(at, held.size());
      }
      std::string includes;
      for (const std::string& used : g.uses)
        if (piece_of.count(used)) includes += "#include \"" + piece_of[used] + "\"\n";
      const std::string piece = pieces_include + "/" + space + ".h";
      piece_of[h.name] = piece;
      pieces.push_back({pieces_dir + "/" + space + ".h",
                        "// Generated from " + name + " by tools/blueprint_cpp - do not edit: the class of the Blueprint of " + owner +
                            " (one of\n// " + header_path_name + "; kern/docs/blueprint_cpp.md). Change the Blueprint in the level; the build writes this file again.\n"
                            "#pragma once\n\n#include \"kern/logic/blueprint.h\"\n#include \"kern/logic/blueprint_class.h\"\n" + includes +
                            "\n// ---------------------------------------------------------------------------\n// The Blueprint of " + owner + "\n" +
                            std::string("namespace ") + space + " {\n\n" + uses + (uses.empty() ? "" : "\n") + code + "\n" + held + "}  // namespace " + space + "\n"});
      header += "#include \"" + piece + "\"  // the Blueprint of " + owner + "\n";
      pieces.push_back({glue_stem + "_" + space + ".cpp",
                        "// Generated from " + name + " by tools/blueprint_cpp - do not edit: how the engine reaches the Blueprint of " + owner +
                            "\n// (its class: " + piece + "). Change the Blueprint in the level; the build writes this file again.\n"
                            "#include \"kern/logic/blueprint.h\"\n#include \"kern/logic/blueprint_class.h\"  // (the exports of Blueprints: KERN_EXPORT_AS)\n"
                            "#include \"" + piece + "\"\n\n" + std::string("namespace ") + space + " {\n\n" + glue + "\n}  // namespace " + space + "\n\n"
                            "// (how blueprints::bind binds it)\nnamespace blueprints {\nbool bind_" + space + "(kern::logic::Runtime& logic) { return kern::logic::bind_blueprint<::" + space + "::" + cls + ">(logic); }\n}  // namespace blueprints\n"});
      binds.push_back(space);
      out += " (its class: " + piece + ", its glue: " + glue_stem.substr(glue_stem.find_last_of("/\\") + 1) + "_" + space + ".cpp)\n";
    } else {
      for (const std::string& used : g.uses)
        if (piece_of.count(used)) out_includes.insert(piece_of[used]);
      out += std::string("\nnamespace ") + space + " {\n\n" + uses + (uses.empty() ? "" : "\n") + code + "\n" + glue + "\n}  // namespace " + space + "\n";
    }
    ++classes;
  }
  if (classes_only) out += "\nnamespace blueprints {\n" + aliases + "}  // namespace blueprints\n";
  if (header_path) {
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
           "// How the program binds the classes of the header (blueprints::bind; each binds in the source of its glue).\n"
           "namespace blueprints {\n";
    for (const std::string& b : binds) out += "bool bind_" + b + "(kern::logic::Runtime& logic);\n";
    out += "}  // namespace blueprints\n"
           "bool blueprints::bind(kern::logic::Runtime& logic) {\n  (void)logic;\n  bool all = true;\n";
    for (const std::string& b : binds) out += "  all = bind_" + b + "(logic) && all;\n";
    out += "  return all;\n}\n";
    // (<out.cpp> includes the classes of the header its own Blueprints use - after its first lines)
    std::string inc;
    for (const std::string& i : out_includes) inc += "#include \"" + i + "\"\n";
    const std::size_t at = out.find("\n\n");
    out.insert(at == std::string::npos ? out.size() : at + 1, inc);
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
  if (header_path) {
    // (the headers and sources of the classes; those of classes the level has no more go)
    std::error_code ec;
    std::filesystem::create_directories(pieces_dir, ec);
    std::set<std::string> now;
    for (const auto& [path, text] : pieces) {
      if (!write(path.c_str(), text)) return 1;
      now.insert(std::filesystem::path(path).lexically_normal().string());
    }
    const std::filesystem::path glue_dir = std::filesystem::path(glue_stem).parent_path().empty() ? std::filesystem::path(".") : std::filesystem::path(glue_stem).parent_path();
    const std::string glue_prefix = std::filesystem::path(glue_stem).filename().string() + "_kern_blueprint_";
    for (const std::filesystem::path dir : {std::filesystem::path(pieces_dir), glue_dir})
      for (const auto& e : std::filesystem::directory_iterator(dir, ec)) {
        const std::string file = e.path().filename().string();
        const bool ours = dir == glue_dir ? file.starts_with(glue_prefix) && file.ends_with(".cpp") : file.starts_with("kern_blueprint_") && file.ends_with(".h");
        if (!ours) continue;
        const std::string kept = (dir == glue_dir ? (glue_dir / file) : (std::filesystem::path(pieces_dir) / file)).lexically_normal().string();
        if (!now.count(kept)) std::filesystem::remove(e.path(), ec);
      }
    if (!write(header_path, header)) return 1;
  }
  if (!write(argv[2], out)) return 1;
  std::printf("%s: %d of %zu Blueprint(s) as C++ classes", argv[2], classes, found.size());
  if (header_path) std::printf(" - %zu of them in %s, which the program calls (folder %s) and the libraries they use", named.size(), header_path, called_folder.c_str());
  std::printf("\n");
  return 0;
}
