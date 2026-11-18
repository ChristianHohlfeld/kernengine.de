// A small main for the converted tests of the mini files alone (the registry of test_util.h, the allocator's count of cost.h).
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>
#include "cost.h"
#include "test_util.h"
namespace sct {
std::vector<Case>& cases() { static std::vector<Case> c; return c; }
int failures = 0, checks = 0;
}  // namespace sct
void* operator new(std::size_t size) {
  sc::cost::asked();
  if (void* p = std::malloc(size ? size : 1)) return p;
  std::abort();
}
void* operator new[](std::size_t size) { return operator new(size); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
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
