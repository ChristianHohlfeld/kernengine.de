// The source side of the measurement (bench.h): qr_code.cpp, as the game builds it.
#include "../../src/qr_code.cpp"
#include "bench.h"
namespace SC = sc;
#define SRC(name) src_##name
#include "src_qr.inc"
