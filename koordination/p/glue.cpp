// Paket p: Klasse + Glue eines logic_cases-Falls schreiben (Syntaxprüfung des Glue).
#include <cstdio>
#include "logic_cases.h"
int main(int, char** argv) {
  for (const auto& c : logic_cases::cases()) {
    if (std::string(c.name) != argv[1]) continue;
    kern::logic::Graph g = logic_cases::parse(c);
    std::string text, glue, err;
    if (!kern::logic::to_cpp(g, "Case", text, &err)) std::printf("to_cpp: %s\n", err.c_str());
    if (!kern::logic::native_glue(g, "Case", glue, &err)) std::printf("glue: %s\n", err.c_str());
    FILE* f = std::fopen(argv[2], "w"); std::fputs(text.c_str(), f); std::fputs(glue.c_str(), f); std::fclose(f);
  }
}
