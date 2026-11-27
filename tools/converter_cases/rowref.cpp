#include "rowref.h"
namespace cases {
namespace {
u64 key2(i32 i, i32 j) { return (static_cast<u64>(static_cast<u32>(i)) << 32) | static_cast<u32>(j); }
}
Water& water() { static Water w; return w; }
bool Water::cell(i32 cx, i32 cy) const {
  auto it = water_.find(key2(cx >> 4, cy >> 4));
  if (it == water_.end()) return false;
  const u32 idx = static_cast<u32>((cy & 15) * 16 + (cx & 15));
  return (it->second.bits[idx >> 6] >> (idx & 63)) & 1u;
}
void Water::set_cell(i32 cx, i32 cy) {
  Block& b = water_[key2(cx >> 4, cy >> 4)];
  const u32 idx = static_cast<u32>((cy & 15) * 16 + (cx & 15));
  if (!((b.bits[idx >> 6] >> (idx & 63)) & 1u)) ++cells;
  b.bits[idx >> 6] |= u64{1} << (idx & 63);
}
bool case_cell(i32 x, i32 y) { return water().cell(x, y); }
void case_set(i32 x, i32 y) { water().set_cell(x, y); }
}
