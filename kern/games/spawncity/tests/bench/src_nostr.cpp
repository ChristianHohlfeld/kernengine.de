// The source side of the measurement (bench.h): nostr.cpp, as the game builds it.
#include "../../src/nostr.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_nostr.inc"
