#include "inner.h"
#include "kern/api/c_api.h"
namespace cases {
kern::i32 doubled_score(kern::i32 points) { return points * 2; }
/// An export inside the program's namespace (`extern "C"` in it, as a game's main.cpp has its exports).
KERN_EXPORT(case_doubled) kern::i32 case_doubled(kern::i32 points) { return doubled_score(points) + 1; }
}
