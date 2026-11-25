// The source side of the measurement in the numbers of the language (bench.h): common.h as the test of
// tests/converted compiles it (test_common.cpp, ../converted.h: f32 is double, the mathematics is kern::math) -
// the same cases as src_common.cpp (src_common.inc). The test itself is not run.
#include "test_util.h"
#undef SC_TEST
#define SC_TEST(name) [[maybe_unused]] static void name()
#include "test_common.cpp"
#include "bench.h"
namespace SC = converted_common;
#define SRC(name) src64_##name
#include "src_common.inc"
