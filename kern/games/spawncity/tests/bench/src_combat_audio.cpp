// The source side of the measurement (bench.h): combat_audio.cpp, as the game builds it.
#include "../../src/combat_audio.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_combat_audio.inc"
