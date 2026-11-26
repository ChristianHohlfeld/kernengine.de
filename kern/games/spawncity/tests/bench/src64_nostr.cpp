// The source side of the measurement in the numbers of the language (bench.h): nostr.cpp as the
// test of tests/converted compiles it (its file test_<file>.cpp, ../converted.h: f32 is double, the
// mathematics is kern::math) - the same cases as src_nostr.cpp (src_nostr.inc). The test itself is not run.
#include "kern/test/test.h"
#undef KERN_TEST
#define KERN_TEST(name) [[maybe_unused]] static void name()
#include "test_nostr_cpp.cpp"
#include "bench.h"
namespace SC = converted_nostr_cpp;
#define SRC(name) src64_##name
#include "src_nostr.inc"
