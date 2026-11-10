// The Blueprints of a game as C++ classes do what they do in the interpreter (run.sh).
//
//   game_native_test <dir>
//
// <dir> has the levels of game_cases.h as kern_tests wrote them (--emit-game-native: arena.klevel,
// robot.klevel); their Blueprints are linked into this program as the classes tools/blueprint_cpp
// wrote from those files - what a game's build does. Every scenario is played twice, in the
// interpreter and as those classes, and everything it shows must be the same: every Blueprint's
// variables, every actor, the HUD, what the game reported.
#include <cstdio>
#include <string>

#include "game_cases.h"
#include "kern/app/app.h"

kern::App* kern_create_app() { return nullptr; }  // (the app runtime is linked in; it needs a factory)

int main(int argc, char** argv) {
  using namespace kern;
  if (argc < 2) {
    std::fprintf(stderr, "usage: game_native_test <dir with the levels>\n");
    return 2;
  }
  int failures = 0;
  std::size_t blueprints = 0, lines = 0;
  for (const game_cases::Case& c : game_cases::cases()) {
    const std::string path = std::string(argv[1]) + "/" + c.name + ".klevel";
    std::string text, error;
    if (FILE* f = std::fopen(path.c_str(), "rb")) {
      char b[65536];
      std::size_t n;
      while ((n = std::fread(b, 1, sizeof b, f)) > 0) text.append(b, n);
      std::fclose(f);
    }
    game::Level level;
    if (!game::from_text(text, level, &error)) {
      std::printf("FAIL game_native_test: %s: %s\n", path.c_str(), error.empty() ? "cannot read it" : error.c_str());
      return 1;
    }
    const game_cases::Played plain = game_cases::play(level, c.scenario, false), native = game_cases::play(level, c.scenario, true);
    // every Blueprint ran as its class - and, asked not to, none did
    for (const std::string& t : native.tiers)
      if (t.size() < 12 || t.compare(t.size() - 12, 12, ": native C++") != 0) {
        std::printf("FAIL game_native_test (%s): %s, not as its class\n", c.name, t.c_str());
        ++failures;
      }
    for (const std::string& t : plain.tiers)
      if (t.find("native C++") != std::string::npos) {
        std::printf("FAIL game_native_test (%s): %s, although GameWorld::logic_native is off\n", c.name, t.c_str());
        ++failures;
      }
    if (native.tiers.empty()) {
      std::printf("FAIL game_native_test (%s): no Blueprint ran\n%s", c.name, native.text.c_str());
      ++failures;
    }
    blueprints += native.tiers.size();
    // line by line
    std::size_t a = 0, b = 0, shown = 0;
    std::string where;
    while (a < native.text.size() || b < plain.text.size()) {
      const std::size_t ae = native.text.find('\n', a), be = plain.text.find('\n', b);
      const std::string x = a < native.text.size() ? native.text.substr(a, ae == std::string::npos ? ae : ae - a) : "(nothing)";
      const std::string y = b < plain.text.size() ? plain.text.substr(b, be == std::string::npos ? be : be - b) : "(nothing)";
      if (x.rfind("-- ", 0) == 0) where = x.substr(3);
      if (x != y) {
        if (shown++ < 12) std::printf("FAIL game_native_test (%s, %s):\n     the classes:     %s\n     the interpreter: %s\n", c.name, where.c_str(), x.c_str(), y.c_str());
        ++failures;
      }
      ++lines;
      a = ae == std::string::npos ? native.text.size() : ae + 1;
      b = be == std::string::npos ? plain.text.size() : be + 1;
    }
  }
  std::printf("%s game_native_test (%zu Blueprints of two levels as their C++ classes, %zu lines compared with the interpreter's)\n", failures ? "FAIL" : "ok  ", blueprints, lines);
  return failures ? 1 : 0;
}
