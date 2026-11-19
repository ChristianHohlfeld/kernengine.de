// The source side of the measurement (bench.h): mvt.cpp, as the game builds it.
#include "../../src/mvt.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_mvt.inc"
