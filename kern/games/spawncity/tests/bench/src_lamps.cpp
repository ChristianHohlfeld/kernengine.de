// The source side of the measurement (bench.h): lamps.cpp, as the game builds it.
#include "../../src/lamps.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_lamps.inc"
