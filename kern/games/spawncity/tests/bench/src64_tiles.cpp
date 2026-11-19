// The source side of the measurement in the numbers of the language (bench.h): tiles.h as the test of
// tests/converted compiles it (test_tiles.cpp, ../converted.h: f32 is double, the mathematics is kern::math) -
// the same cases as src_tiles.cpp (src_tiles.inc). The test itself is not run.
#include "test_util.h"
#undef SC_TEST
#define SC_TEST(name) [[maybe_unused]] static void name()
#include "test_tiles.cpp"
#include "bench.h"
namespace SC = converted_tiles;
#define SRC(name) src64_##name
#include "src_tiles.inc"
