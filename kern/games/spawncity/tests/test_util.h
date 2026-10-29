// Tiny test registry for the SpawnCity native tests.
#pragma once

#include <cmath>
#include <cstdio>
#include <vector>

namespace sct {
struct Case { const char* name; void (*fn)(); };
std::vector<Case>& cases();
extern int failures, checks;
struct Register { Register(const char* n, void (*f)()) { cases().push_back({n, f}); } };
}  // namespace sct

#define SC_TEST(name)                                         \
  static void name();                                         \
  static sct::Register name##_registered(#name, &name);       \
  static void name()

#define SC_CHECK(cond, ...)                                                   \
  do {                                                                        \
    ++sct::checks;                                                            \
    if (!(cond)) {                                                            \
      ++sct::failures;                                                        \
      std::printf("  check failed %s:%d: %s | ", __FILE__, __LINE__, #cond);  \
      std::printf(__VA_ARGS__);                                               \
      std::printf("\n");                                                      \
    }                                                                         \
  } while (0)
