#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "kern/logic/logic.h"
int main(int, char** argv) {
  std::stringstream ss; ss << std::ifstream(argv[1]).rdbuf(); const std::string text = ss.str();
  kern::logic::Graph g; std::string name, e, twice;
  if (!kern::logic::from_cpp(text, g, &name, &e)) { std::printf("Leser: %s\n", e.c_str()); return 1; }
  if (!kern::logic::to_cpp(g, name, twice, &e)) { std::printf("to_cpp: %s\n", e.c_str()); return 1; }
  if (std::getenv("CHK3_OUT")) { FILE* f = std::fopen(std::getenv("CHK3_OUT"), "w"); std::fputs(twice.c_str(), f); std::fclose(f); }
  std::printf(twice == text ? "Fixpunkt gleich\n" : "Fixpunkt VERSCHIEDEN\n");
}
