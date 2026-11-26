#include "numbers_same.h"
namespace cases {
u32 hash32(u32 h) { return h * 2654435761u; }
u64 lcg(u64 s) { return s * 6364136223846793005ull + 1442695040888963407ull; }
}
