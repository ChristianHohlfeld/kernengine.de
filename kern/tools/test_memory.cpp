// The allocator of a test program (kern/test/cost.h): it says every piece of memory it is asked for
// (kern::test::cost::asked - the comparison of a converted function with its source counts them), and what it gives
// is zeroed: what C++ leaves open in an object that nothing sets is then the zero the Blueprint starts from, one of
// the values C++ allows (kern/test/converted.h, zero_stack). With the sanitizers it is their allocator, through its
// hook; without them, operator new itself. Link it into every program that runs the tests of kern/test/
// (kern/tools/test_main.cpp, or a game's own runner) - once.
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "kern/test/cost.h"

#if defined(KERN_ASAN)
// (the sanitizers' own interface - sanitizer/allocator_interface.h, where a compiler brings that header)
extern "C" int __sanitizer_install_malloc_and_free_hooks(void (*asked)(const volatile void*, std::size_t), void (*given_back)(const volatile void*));
static void asked_for_memory(const volatile void* p, std::size_t size) {
  kern::test::cost::asked();
  if (p != nullptr) std::memset(const_cast<void*>(p), 0, size);
}
static void gave_memory_back(const volatile void*) {}
// (installed as the program starts: nothing is counted before a comparison begins - cost::begin)
[[maybe_unused]] static const int g_hooked = __sanitizer_install_malloc_and_free_hooks(asked_for_memory, gave_memory_back);
// What the C runtime asks for itself when a function's static registers how it ends (__cxa_atexit keeps its list
// in blocks and asks for a new one whenever the last is full - which call meets that depends on how many statics
// the whole process has registered before, not on the function) is not memory the function asks for: operator new
// does not see it either. So the registration runs with the count off.
#include <dlfcn.h>
extern "C" int __cxa_atexit(void (*end)(void*), void* object, void* dso) {
  using Register = int (*)(void (*)(void*), void*, void*);
  static const Register next = reinterpret_cast<Register>(dlsym(RTLD_NEXT, "__cxa_atexit"));
  const bool counted = kern::test::cost::counting;
  kern::test::cost::counting = false;
  const int r = next(end, object, dso);
  kern::test::cost::counting = counted;
  return r;
}
#else
#include <new>
void* operator new(std::size_t size) {
  kern::test::cost::asked();
  if (void* p = std::calloc(1, size ? size : 1)) return p;   // (zeroed: see the sanitizers' hook above)
  std::abort();
}
void* operator new[](std::size_t size) { return operator new(size); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
#endif
