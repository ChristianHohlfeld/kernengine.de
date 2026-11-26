#include "numbers_sum.h"
namespace cases {
bool past_one_after(int frames) {
  f32 s = 0;
  for (int i = 0; i < frames; ++i) s += 1.0f / 60.0f;
  return s > 1.0f;
}
}
