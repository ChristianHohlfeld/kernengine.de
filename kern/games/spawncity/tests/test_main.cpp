// Native unit tests of SpawnCity's simulation models (no GL, deterministic):
// flight controller + drone plant, the jet's aerodynamics and fly-by-wire, the car's
// suspension and tyres, the map decoder and the game rules.
//
//   kern/games/spawncity/tests/run_tests.sh
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "test_util.h"

namespace sct {
std::vector<Case>& cases() { static std::vector<Case> c; return c; }
int failures = 0, checks = 0;
}  // namespace sct

int main(int argc, char** argv) {
  const char* only = argc > 1 ? argv[1] : nullptr;
  int ran = 0;
  for (const sct::Case& c : sct::cases()) {
    if (only && !std::strstr(c.name, only)) continue;
    const int before = sct::failures;
    c.fn();
    ++ran;
    std::printf("%s %s\n", sct::failures == before ? "ok  " : "FAIL", c.name);
  }
  std::printf("%d tests, %d checks, %d failures\n", ran, sct::checks, sct::failures);
  return sct::failures ? 1 : 0;
}
