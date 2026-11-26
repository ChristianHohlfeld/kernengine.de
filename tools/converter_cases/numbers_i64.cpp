#include "numbers_i64.h"
namespace cases {
long long mix(long long h, int c) { return (h ^ c) / 3 + 7; }
long long mixed_from_the_start() { return mix(1152921504606846977LL, 97); }
}
