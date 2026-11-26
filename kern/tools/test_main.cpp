// A program that runs the tests of kern/test/test.h - the tests tools/cpp_to_blueprint.py --test writes (kern/test/
// converted.h) and a game's own -, one after the other, in the order they were registered. Build it with the tests,
// the engine, the embedded project (kern/tools/embed_project.py), the Blueprints' classes (tools/blueprint_cpp.sh) and
// kern/tools/test_memory.cpp (the allocator the comparison counts with).
//
//   tests                    every test
//   tests --name <n> ...     exactly the tests named so (a whole name; one that is no test's: FAIL)
//   KERN_TEST_ENGINE=real    the engine itself answers the Blueprints' calls of the engine (kern/test/blueprint.h)
//
// Each test says "ok <name>" or "FAIL <name> (<k> of <n> checks)"; the last line counts them. It ends with 1 when a
// check failed or a name is no test's.
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "kern/test/test.h"

int main(int argc, char** argv) {
  std::vector<std::string> names;
  if (argc > 1) {
    if (std::strcmp(argv[1], "--name") != 0) {
      std::printf("FAIL %s: tests [--name <test> ...]\n", argv[1]);
      return 1;
    }
    for (int k = 2; k < argc; ++k) names.emplace_back(argv[k]);
  }
  int unknown = 0;
  for (const std::string& n : names) {
    bool known = false;
    for (const kern::test::Case& c : kern::test::cases()) known = known || n == c.name;
    if (!known) {
      std::printf("FAIL %s (no test has this name)\n", n.c_str());
      ++unknown;
    }
  }
  int ran = 0, failed = 0, checks = 0, failures = 0;
  for (const kern::test::Case& c : kern::test::cases()) {
    bool wanted = names.empty();
    for (const std::string& n : names) wanted = wanted || n == c.name;
    if (!wanted) continue;
    kern::test::checks = kern::test::failures = 0;
    c.fn();
    ++ran;
    checks += kern::test::checks;
    failures += kern::test::failures;
    if (kern::test::failures) {
      ++failed;
      std::printf("FAIL %s (%d of %d checks)\n", c.name, kern::test::failures, kern::test::checks);
    } else {
      std::printf("ok   %s (%d checks)\n", c.name, kern::test::checks);
    }
    std::fflush(stdout);
  }
  std::printf("%d tests, %d checks, %d failures%s\n", ran, checks, failures, unknown ? " (and names that are no test's)" : "");
  return failures || unknown ? 1 : 0;
}
