#include <chrono>
#include <cstdio>
#include <string>
#include "level_blueprints.h"
int main() {
  const std::string url = "https://kernengine.de/kern/games/spawncity/web/index.html?mp=join&room=lobby&net=public";
  blueprints::qr_code_cpp q;
  // (the loop guard as in the game: the runtime's counters, the limit rules::kLoopBudget - a compare and a count per pass)
  kern::i32 counters[5] = {0, 2000000000, 0, 0, 0};
  q.kern_world.counters = counters;
  blueprints::qr_code::qr_Code c;
  long sum = 0;
  const int n = 200;
  auto t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < n; ++i) { q.encode(url, c); sum += c.modules[(i * 7) % c.modules.size()]; }
  auto t1 = std::chrono::steady_clock::now();
  std::printf("Blueprint class: encode v%lld %.2f us per call (%ld)\n", static_cast<long long>(c.version), std::chrono::duration<double, std::micro>(t1 - t0).count() / n, sum);
}

// (never reached under the limit: the runtime's way when a loop is over it)
bool kern::logic::Runtime::pass(Instance&) { return false; }
