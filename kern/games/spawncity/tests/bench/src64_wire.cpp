// The source side of the measurement in the numbers of the language (bench.h): wire.h as the test of
// tests/converted compiles it (test_wire.cpp, ../converted.h: f32 is double, the mathematics is kern::math) -
// the same cases as src_wire.cpp (src_wire.inc). The test itself is not run.
#include "kern/test/test.h"
#undef KERN_TEST
#define KERN_TEST(name) [[maybe_unused]] static void name()
#include "test_wire.cpp"
#include "bench.h"
namespace SC = converted_wire;
#define SRC(name) src64_##name
#include "src_wire.inc"
