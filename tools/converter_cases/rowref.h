#pragma once
#include <vector>
#include "kern/core/base.h"
#include "kern/core/map.h"
namespace cases {
using kern::u32; using kern::u64; using kern::i32; using kern::f32;
class Water {
 public:
  bool cell(i32 cx, i32 cy) const;
  void set_cell(i32 cx, i32 cy);
  u32 cells = 0;
 private:
  struct Block { u64 bits[4] = {}; std::vector<f32> speed; };
  kern::Map<u64, Block> water_;
};
Water& water();
bool case_cell(i32 x, i32 y);
void case_set(i32 x, i32 y);
}
