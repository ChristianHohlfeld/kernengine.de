// Host tool: the Blueprints of a Kern level as C++ classes a game links.
//
//   tools/blueprint_cpp.sh <level.klevel> <out.cpp>      (builds this file once, then runs it)
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

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::fprintf(stderr, "usage: blueprint_cpp <level.klevel> <out.cpp>\n");
    return 2;
  }
  (void)game::logic_node_shapes_registered();  // the gameplay nodes, as the library knows them
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
    std::string code, glue;
    if (!logic::to_cpp(g, cls, code, &error) || !logic::native_glue(g, cls, glue, &error)) {
      std::fprintf(stderr, "%s: the Blueprint of %s: %s\n", argv[1], owner.c_str(), error.c_str());
      return 1;
    }
    // (a namespace of its own, named by the graph's hash: the class meets no other class of that
    // name - not the game's, not another level's)
    char space[48];
    std::snprintf(space, sizeof space, "kern_blueprint_%016llx", static_cast<unsigned long long>(logic::graph_hash(g)));
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
