// The source side of the measurement (bench.h): wire.h, as the game builds it.
#include "../../src/wire.h"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_wire.inc"
