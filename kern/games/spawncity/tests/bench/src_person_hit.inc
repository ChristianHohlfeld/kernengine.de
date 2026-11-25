// The cases of src_person_hit.cpp and src64_person_hit.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
#include "src_helpers.inc"
bench::u64 SRC(person_hit)() {
  static const auto in = numbers(15, 10 * bench::kMany, -4, 4);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    const double* p = &in[i];
    const SC::Vec3 feet{static_cast<f32>(p[0] / 4), static_cast<f32>(p[1] / 4), 0}, from{static_cast<f32>(p[4]), static_cast<f32>(p[5]), static_cast<f32>(p[6] / 4 + 1)};
    const SC::Vec3 seg{static_cast<f32>((p[0] / 4 - p[4]) * 2 + p[7] / 8), static_cast<f32>((p[1] / 4 - p[5]) * 2 + p[8] / 8), static_cast<f32>(p[9] / 8)};
    f32 t = 0;
    const SC::BodyPart part = SC::person_hit::hit(feet, static_cast<f32>(p[2]), static_cast<f32>(p[3] + 4), from, seg, t);
    h = bench::fold_number(bench::fold(h, static_cast<bench::u64>(part)), t);
  }
  return h;
}
bench::u64 SRC(person_sphere)() {
  static const auto in = numbers(16, 10 * bench::kMany, -4, 4);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    f32 t = 0;
    const bool hit = SC::person_hit::sphere_entry(vec(&in[i]), vec(&in[i + 3]), vec(&in[i + 6]), static_cast<f32>(in[i + 9] / 4 + 1.5), t);
    h = bench::fold_number(bench::fold(h, hit), t);
  }
  return h;
}
