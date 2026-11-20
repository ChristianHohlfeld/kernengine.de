// Kleinst-Harness Paket d: liest einen Blueprint-Text, prüft check(), Fixpunkt (to_cpp), spielt Läufe im Interpreter und
// im JIT gegen den Stellvertreter und zeigt die Variablen nach jedem Lauf.   harness <text.cpp> [Lauf ...]
#include <cstdio>
#include <fstream>
#include <sstream>
#include "kern/logic/logic.h"
#include "kern/logic/engine_api.h"
#include "native_show.h"
using namespace kern;

static std::string play(const logic::Graph& g, const std::string& name, const std::vector<std::string>& runs, bool jit, std::string& calls) {
  NativeTestHost host;
  logic::Runtime rt(host);
  rt.jit = jit;
  logic::Instance* inst = rt.add(g, 0, name);
  rt.begin();
  std::string out = std::string("== ") + inst->tier() + "\n";
  for (const std::string& run : runs) {
    std::vector<logic::Value> results;
    out += run + (inst->call(run, {}, results) ? " ->" : " (no call) ->");
    for (const std::string& e : host.errors) out += " !" + e;
    host.errors.clear();
    for (const logic::Variable& v : g.variables) out += " " + v.name + "=" + native_show(inst->get(v.name));
    out += "\n";
  }
  for (const std::string& c : host.stand_in.calls) calls += c + "\n";
  return out;
}

int main(int argc, char** argv) {
  if (argc < 2) return 2;
  std::ifstream f(argv[1]);
  std::stringstream ss;
  ss << f.rdbuf();
  const std::string text = ss.str();
  logic::Graph g;
  std::string name, error;
  if (!logic::from_cpp(text, g, &name, &error)) {
    std::printf("NICHT GELESEN: %s\n", error.c_str());
    return 1;
  }
  int bad = 0;
  for (const logic::Issue& i : logic::check(g)) {
    std::printf("%s node %u: %s\n", i.error ? "FEHLER" : "Hinweis", i.node, i.text.c_str());
    bad += i.error;
  }
  std::string back;
  if (!logic::to_cpp(g, name, back, &error, logic::library(), false)) std::printf("to_cpp: %s\n", error.c_str());
  std::ofstream(std::string(argv[1]) + ".back") << back;
  std::printf("Fixpunkt: %s\n", back == text ? "ja" : "NEIN (diff gegen .back)");
  std::string why, glue;
  if (logic::native_glue(g, name, glue, &why)) std::ofstream(std::string(argv[1]) + ".class.cpp") << back << glue;
  else std::printf("native_glue: %s\n", why.c_str());
  std::printf("native_ready: %s %s\n", logic::native_ready(g, &why) ? "ja" : "nein", why.c_str());
  const std::vector<std::string> runs(argv + 2, argv + argc);
  std::string c1, c2;
  const std::string a = play(g, name, runs, false, c1), b = play(g, name, runs, true, c2);
  std::printf("%s", a.c_str());
  if (b.substr(b.find('\n')) != a.substr(a.find('\n')) || c1 != c2) std::printf("JIT ANDERS:\n%s", b.c_str());
  else std::printf("JIT gleich (%s)\n", b.substr(3, b.find('\n') - 3).c_str());
  std::printf("Aufrufe:\n%s", c1.c_str());
  return bad ? 1 : 0;
}
