// The source side of the measurement (bench.h): settings.cpp, as the game builds it.
#include "../../src/settings.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_settings.inc"
