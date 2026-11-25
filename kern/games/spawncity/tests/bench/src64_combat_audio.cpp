// The source side of the measurement in the numbers of the language (bench.h): combat_audio.cpp as the test of
// tests/converted compiles it (test_combat_audio_cpp.cpp, ../converted.h: f32 is double, the mathematics is kern::math) -
// the same cases as src_combat_audio.cpp (src_combat_audio.inc). The test itself is not run.
#include "test_util.h"
#undef SC_TEST
#define SC_TEST(name) [[maybe_unused]] static void name()
#include "test_combat_audio_cpp.cpp"
#include "bench.h"
namespace SC = converted_combat_audio_cpp;
#define SRC(name) src64_##name
#include "src_combat_audio.inc"
