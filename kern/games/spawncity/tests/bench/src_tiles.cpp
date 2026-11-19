// The source side of the measurement (bench.h): tiles.h, as the game builds it.
#include "../../src/tiles.h"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_tiles.inc"
