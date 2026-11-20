// Paket d: der Block "Platz der Engine" aus every_node_graph allein - check, Fixpunkt, Klasse.
#include <cstdio>
#include <fstream>
#include "kern/logic/logic.h"
using namespace kern;
int main() {
  const logic::Library& lib = logic::library();
  logic::Graph g;
  g.variables.push_back({"VI", logic::PinType::integer, json::Value(0), {}, ""});
  u32 next = 1;
  auto add = [&](const std::string& type, std::vector<std::pair<std::string, std::string>> params = {}) {
    logic::Node n; n.id = next++; n.type = type; n.params = std::move(params); g.nodes.push_back(n); return n.id; };
  auto wire = [&](u32 from, const std::string& out, u32 to, const std::string& in) { g.links.push_back({from, out, to, in}); };
  auto first_exec = [&](u32 id, bool input) {
    std::vector<logic::PinDef> in, out; lib.pins(*g.find(id), g, in, out);
    for (const logic::PinDef& p : input ? in : out) if (p.type == logic::PinType::exec) return p.id;
    return std::string(); };
#include "block.inc"
  g.next_id = next + 3;
  for (const logic::Issue& i : logic::check(g)) std::printf("%s %u: %s\n", i.error ? "FEHLER" : "Hinweis", i.node, i.text.c_str());
  std::string text, error, glue, why;
  if (!logic::to_cpp(g, "Every", text, &error)) { std::printf("to_cpp: %s\n", error.c_str()); return 1; }
  logic::Graph back; std::string name;
  if (!logic::from_cpp(text, back, &name, &error)) { std::printf("NICHT GELESEN: %s\n%s", error.c_str(), text.c_str()); return 1; }
  std::string again; logic::to_cpp(back, "Every", again);
  std::printf("Fixpunkt: %s, Knoten %zu/%zu, Drähte %zu/%zu\n", again == text ? "ja" : "NEIN", back.nodes.size(), g.nodes.size(), back.links.size(), g.links.size());
  if (!logic::native_glue(back, name, glue, &why)) std::printf("glue: %s\n", why.c_str());
  std::ofstream("every.class.cpp") << text << glue;
  std::printf("%s", text.substr(text.find("void")).c_str());
}
