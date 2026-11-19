// The helpers of the cases of the game's headers (src_<header>.inc): SC is the namespace of the source.
using SC::f32;
namespace {
std::vector<double> numbers(bench::u64 seed, int n, double a, double b) {
  bench::Gen g(seed);
  std::vector<double> v;
  for (int i = 0; i < n; ++i) v.push_back(g.range(a, b));
  return v;
}
using Component = decltype(SC::Vec3::x);  // (a vector's components are 32-bit on both sides)
SC::Vec3 vec(const double* v) { return {static_cast<Component>(v[0]), static_cast<Component>(v[1]), static_cast<Component>(v[2])}; }
bench::u64 fold(bench::u64 h, SC::Vec3 v) { return bench::fold_number(bench::fold_number(bench::fold_number(h, v.x), v.y), v.z); }
}  // namespace
