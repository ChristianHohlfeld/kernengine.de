// The source side of the measurement in the numbers of the language (bench.h): person_hit.h as the test of
// tests/converted compiles it (test_person_hit.cpp, ../converted.h: f32 is double, the mathematics is kern::math) -
// the same cases as src_person_hit.cpp (src_person_hit.inc). The test itself is not run.
#include "kern/test/test.h"
#undef KERN_TEST
#define KERN_TEST(name) [[maybe_unused]] static void name()
#include "test_person_hit.cpp"
#include "bench.h"
namespace SC = converted_person_hit;
#define SRC(name) src64_##name
#include "src_person_hit.inc"
