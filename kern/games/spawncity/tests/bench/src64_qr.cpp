// The source side of the measurement in the numbers of the language (bench.h): qr_code.cpp as the
// test of tests/converted compiles it (its file test_<file>.cpp, ../converted.h: f32 is double, the
// mathematics is kern::math) - the same cases as src_qr.cpp (src_qr.inc). The test itself is not run.
#include "test_util.h"
#undef SC_TEST
#define SC_TEST(name) [[maybe_unused]] static void name()
#include "test_qr_code_cpp.cpp"
#include "bench.h"
namespace SC = converted_qr_code_cpp;
#define SRC(name) src64_##name
#include "src_qr.inc"
