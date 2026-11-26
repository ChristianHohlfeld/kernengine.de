// The tests of a game: a small registry (KERN_TEST), checks that count (KERN_CHECK), and what a program that runs them
// needs (kern/tools/test_main.cpp runs every registered test; a game with a runner of its own runs cases() itself).
// The tests tools/cpp_to_blueprint.py --test writes (kern/test/converted.h) are registered here, beside a game's own.
//
//   KERN_TEST(my_test) { KERN_CHECK(1 + 1 == 2, "one and one: %d", 1 + 1); }
#pragma once

#include <cstdio>
#include <vector>

namespace kern::test {

/// One registered test: its name and its body.
struct Case {
  const char* name;
  void (*fn)();
};
/// Every registered test, in the order the program registered them.
inline std::vector<Case>& cases() {
  static std::vector<Case> all;
  return all;
}
/// The checks made and the checks that failed (a runner reads them after each test).
inline int failures = 0, checks = 0;
struct Register {
  Register(const char* n, void (*f)()) { cases().push_back({n, f}); }
};

}  // namespace kern::test

/// A test of this name: registered when the program starts.
#define KERN_TEST(name)                                         \
  static void name();                                           \
  static ::kern::test::Register name##_registered(#name, &name); \
  static void name()

/// A check: counted; when `cond` is false, it fails and says where, what and why (the printf arguments after it).
#define KERN_CHECK(cond, ...)                                                 \
  do {                                                                        \
    ++::kern::test::checks;                                                   \
    if (!(cond)) {                                                            \
      ++::kern::test::failures;                                               \
      std::printf("  check failed %s:%d: %s | ", __FILE__, __LINE__, #cond);  \
      std::printf(__VA_ARGS__);                                               \
      std::printf("\n");                                                      \
    }                                                                         \
  } while (0)
