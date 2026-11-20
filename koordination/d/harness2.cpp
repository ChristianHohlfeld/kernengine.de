// Paket d: ein Blueprint-Text auf der Engine selbst (geo) - Interpreter durch die Tür gegen die Klasse direkt, und gegen den
// Stellvertreter Interpreter gegen Klasse.   harness2 <text.cpp> Lauf...   (die Klasse des Texts ist mit hineinübersetzt)
#include <cstdio>
#include <fstream>
#include <sstream>
#include "kern/logic/logic.h"
#include "kern/logic/engine_api.h"
#include "native_show.h"
using namespace kern;

static std::string stand_in(const logic::Graph& g, const std::string& name, const std::vector<std::string>& runs, bool native, std::string& calls) {
  NativeTestHost host;
  logic::Runtime rt(host);
  rt.jit = false;
  rt.native = native;
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
  std::ifstream f(argv[1]);
  std::stringstream ss;
  ss << f.rdbuf();
  logic::Graph g;
  std::string name, error;
  if (!logic::from_cpp(ss.str(), g, &name, &error)) { std::printf("NICHT GELESEN: %s\n", error.c_str()); return 1; }
  const std::vector<std::string> runs(argv + 2, argv + argc);
  std::string c1, c2, problem;
  const std::string a = stand_in(g, name, runs, false, c1), b = stand_in(g, name, runs, true, c2);
  std::printf("Stellvertreter: %s / %s: %s\n", a.substr(3, a.find('\n') - 3).c_str(), b.substr(3, b.find('\n') - 3).c_str(),
              a.substr(a.find('\n')) == b.substr(b.find('\n')) && c1 == c2 ? "GLEICH" : "ANDERS");
  if (a.substr(a.find('\n')) != b.substr(b.find('\n')) || c1 != c2) std::printf("%s%s---\n%s%s", a.c_str(), c1.c_str(), b.c_str(), c2.c_str());
  const std::string door = native_play_on_engine(g, name, runs, false, problem), itself = native_play_on_engine(g, name, runs, true, problem);
  std::printf("Engine: %s %s\n", door == itself ? "GLEICH" : "ANDERS", problem.c_str());
  std::printf("%s", door.c_str());
  if (door != itself) std::printf("--- Klasse:\n%s", itself.c_str());
  return 0;
}
