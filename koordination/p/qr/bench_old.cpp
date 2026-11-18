#include <chrono>
#include <cstdio>
#include <string>
#include "qr_code.h"
int main() {
  using namespace sc;
  const std::string url = "https://kernengine.de/kern/games/spawncity/web/index.html?mp=join&room=lobby&net=public";
  qr::Code c;
  long sum = 0;
  const int n = 2000;
  auto t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < n; ++i) { qr::encode(url, c); sum += c.modules[(i * 7) % c.modules.size()]; }
  auto t1 = std::chrono::steady_clock::now();
  std::printf("C++ (f32 build as the game): encode v%d %.2f us per call (%ld)\n", c.version, std::chrono::duration<double, std::micro>(t1 - t0).count() / n, sum);
}
