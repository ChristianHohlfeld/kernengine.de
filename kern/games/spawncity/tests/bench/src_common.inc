// The cases of src_common.cpp and src64_common.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
#include "src_helpers.inc"
namespace {
SC::Quat quat(const double* q) { return {static_cast<f32>(q[0]), static_cast<f32>(q[1]), static_cast<f32>(q[2]), static_cast<f32>(q[3])}; }
bench::u64 fold(bench::u64 h, SC::Quat q) { return bench::fold_number(bench::fold_number(bench::fold_number(bench::fold_number(h, q.x), q.y), q.z), q.w); }
}  // namespace
bench::u64 SRC(common_rng_next)() {
  SC::Rng r{12345u};
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, r.next());
  return h;
}
bench::u64 SRC(common_rng_unit)() {
  SC::Rng r{12345u};
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold_number(h, r.unit());
  return h;
}
bench::u64 SRC(common_wrap_pi)() {
  static const auto in = numbers(10, bench::kMany, -20, 20);
  bench::u64 h = 0;
  for (double a : in) h = bench::fold_number(h, SC::wrap_pi(static_cast<f32>(a)));
  return h;
}
bench::u64 SRC(common_damp_alpha)() {
  static const auto in = numbers(11, 2 * bench::kMany, 0, 0.5);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold_number(h, SC::damp_alpha(static_cast<f32>(in[i] * 20), static_cast<f32>(in[i + 1] / 5)));
  return h;
}
bench::u64 SRC(common_qnormalize)() {
  static const auto in = numbers(12, 4 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 4) h = fold(h, SC::qnormalize(quat(&in[i])));
  return h;
}
bench::u64 SRC(common_slerp)() {
  static const auto in = numbers(13, 9 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 9)
    h = fold(h, SC::slerp(SC::qnormalize(quat(&in[i])), SC::qnormalize(quat(&in[i + 4])), static_cast<f32>(in[i + 8] * 0.5 + 0.5)));
  return h;
}
bench::u64 SRC(common_to_local)() {
  static const auto in = numbers(14, 7 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 7) h = fold(h, SC::to_local(SC::qnormalize(quat(&in[i])), vec(&in[i + 4])));
  return h;
}
