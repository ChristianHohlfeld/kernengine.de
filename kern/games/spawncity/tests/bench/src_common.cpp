// The source side of the measurement (bench.h): common.h, as the game builds it.
#include "../../src/common.h"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_common.inc"
