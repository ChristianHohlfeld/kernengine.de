// The cases of the measurement (bench.h): CASE(name, kind, same) - `same`: the checksums of source
// and class must be equal (the source computes in the numbers of the language).
// The source side of a case is src_<name>() in src_<unit>.cpp, the class side cls_<name>() in bench.cpp.
#define BENCH_CASES(CASE)                     \
  CASE(qr_encode, "Int, Liste, Text", 1)      \
  CASE(qr_gf_mul, "Int (Bits)", 1)            \
  CASE(qr_rs_remainder, "Int, Liste", 1)      \
  CASE(nostr_sha256, "Int (Bits), Fixed", 1)  \
  CASE(nostr_mulm, "Int, Struktur, Fixed", 1) \
  CASE(nostr_from_hex, "Text, Fixed", 1)      \
  CASE(common_rng_next, "Int (Bits), Ref", 1) \
  CASE(tiles_key_hash, "Int64", 1)            \
  CASE(lamps_grid_key, "Float->Int64", 1)     \
  CASE(mvt_zigzag, "Int64", 1)                \
  CASE(mvt_read_geometry, "Bytes, Listen", 1) \
  CASE(wire_put_u32, "Bytes, Liste", 1)       \
  CASE(wire_get_u32, "Bytes, Struktur", 1)    \
  CASE(seats_claim, "Liste v. Strukt.", 1)    \
  CASE(settings_escape, "Text", 1)            \
  CASE(settings_num, "Float->Text", 0)        \
  CASE(common_wrap_pi, "Float, Schleife", 0)  \
  CASE(common_damp_alpha, "Float, exp", 0)    \
  CASE(common_qnormalize, "Quat", 0)          \
  CASE(common_slerp, "Quat, Bibl.", 0)        \
  CASE(common_to_local, "Vektor, Bibl.", 0)   \
  CASE(common_rng_unit, "Int->Float", 0)      \
  CASE(person_hit, "Vektor, Bibl.", 0)        \
  CASE(person_sphere, "Vektor", 0)            \
  CASE(wire_put_f32, "Float->Bytes", 0)       \
  CASE(mvt_ring_area, "Liste v. Vektor", 0)   \
  CASE(perf_governor, "Struktur, Float", 0)   \
  CASE(audio_render, "Float, Liste", 0)       \
  CASE(lamps_place_road, "Map, Set, Liste", 0) \
  CASE(lamps_near, "Map, Liste", 0)
