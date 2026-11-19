// The cases of src_combat_audio.cpp and src64_combat_audio.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
bench::u64 SRC(audio_render)() {
  const std::vector<SC::f32> pcm = SC::combat_audio::render(SC::combat_audio::Kind::shot, 1, 8000);
  bench::u64 h = bench::fold(0, pcm.size());
  for (SC::f32 x : pcm) h = bench::fold_number(h, x);
  return h;
}
