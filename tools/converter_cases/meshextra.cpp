#include "meshextra.h"
namespace cases {
u32 extra_of_the_shore(f32 shore) {
  kern::MeshData m;
  m.extra_size = 3;
  const f32 rec[3] = {shore + 1, 0, 0};
  m.push_extra(rec);
  m.push_extra(rec);
  return static_cast<u32>(m.extra.size());
}
}
