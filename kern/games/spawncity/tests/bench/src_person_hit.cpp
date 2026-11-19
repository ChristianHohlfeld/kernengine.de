// The source side of the measurement (bench.h): person_hit.h, as the game builds it.
#include "../../src/person_hit.h"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_person_hit.inc"
