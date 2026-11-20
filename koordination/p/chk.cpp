// (Paket p: one case of logic_cases.h - check, the interpreter, to_cpp - without the whole test)
#include <cstdio>
#include <cstdlib>
#include "logic_cases.h"
int main(int argc, char** argv) {
  for (const auto& c : logic_cases::cases()) {
    if (std::string(c.name) != argv[1]) continue;
    kern::logic::Graph g = logic_cases::parse(c);
    for (const auto& i : kern::logic::check(g)) std::printf("issue: %s\n", i.text.c_str());
    std::string text, err;
    if (!kern::logic::to_cpp(g, "Case", text, &err)) std::printf("to_cpp: %s\n", err.c_str());
    if (argc > 2) { FILE* f = std::fopen(argv[2], "w"); std::fputs(text.c_str(), f); std::fclose(f); }
    logic_cases::Host host;
    kern::logic::Runtime rt(host);
    kern::logic::Instance* inst = rt.add(g, {}, "Case");
    const std::string out = logic_cases::play(rt, host, inst);
    if (std::getenv("CHK_FULL")) std::fputs(out.c_str(), stdout); else std::printf("%zu bytes: %.600s\n", out.size(), out.c_str());
  }
}
