// The source side of the measurement (bench.h): perf.cpp, as the game builds it.
#include "../../src/perf.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_perf.inc"
