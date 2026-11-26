#include "views.h"
namespace cases {
f32 sum_of(kern::Span<const f32> values) {
  f32 s = 0;
  for (f32 v : values) s += v;
  return s;
}
f32 first_of(const f32* values) { return values[0] + values[1]; }
u32 extra_from_an_array(f32 shore) {
  kern::MeshData m;
  m.extra_size = 3;
  const f32 rec[3] = {shore + 1, 0, 0};
  m.push_extra(rec);
  m.push_extra(rec);
  return static_cast<u32>(m.extra.size());
}
u32 extra_from_a_list(f32 shore) {
  kern::MeshData m;
  m.extra_size = 2;
  std::vector<f32> rec = {shore, shore * 2, shore * 3};
  m.push_extra(rec.data());
  m.push_extra(rec.data() + 1);
  return static_cast<u32>(m.extra.size());
}
f32 sums() {
  const f32 three[3] = {1, 2, 3};
  std::vector<f32> many = {4, 5, 6, 7};
  return sum_of(kern::Span<const f32>(three, 3)) + sum_of(many) + first_of(three) + first_of(many.data() + 2);
}
void fill(kern::Span<f32> out, f32 v) {
  for (std::size_t k = 0; k < out.size; ++k) out[k] = v + static_cast<f32>(k);
}
f32 filled() {
  std::vector<f32> many(4);
  fill(many, 2);
  return many[0] + many[3];
}
}
