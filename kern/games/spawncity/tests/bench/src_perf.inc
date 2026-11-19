// The cases of src_perf.cpp and src64_perf.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
bench::u64 SRC(perf_governor)() {
  static const auto in = [] { bench::Gen g(25); std::vector<double> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(g.range(0.005, 0.06)); return v; }();
  SC::perf::Governor gov;
  bench::u64 h = 0;
  for (double dt : in) h = bench::fold(h, gov.update(static_cast<SC::f32>(dt)));
  return h;  // (the test's copy of Governor has only what the converted functions use - not tier(), fps())
}
