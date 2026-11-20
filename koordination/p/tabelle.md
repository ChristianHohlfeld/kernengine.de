| Fall | Art | C++ µs | Klasse µs vorher | vorher | nach a | nach b | nach c | Klasse µs nach c | gleich |
|---|---|---|---|---|---|---|---|---|---|
| qr_encode | Int, Liste, Text | 27.78 | 786.60 | 28.32 | 28.17 | 19.62 | 14.40 | 391.59 | = |
| qr_gf_mul | Int (Bits) | 1.52 | 50.52 | 33.18 | 32.97 | 23.37 | 18.53 | 27.92 | = |
| qr_rs_remainder | Int, Liste | 53.10 | 1885.73 | 35.51 | 35.79 | 22.85 | 17.98 | 965.91 | = |
| nostr_sha256 | Int (Bits), Fixed | 3.00 | 66.20 | 22.08 | 21.64 | 16.19 | 14.15 | 42.03 | = |
| nostr_mulm | Int, Struktur, Fixed | 36.36 | 313.33 | 8.62 | 8.21 | 7.76 | 7.53 | 256.22 | = |
| nostr_from_hex | Text, Fixed | 1.31 | 9.55 | 7.32 | 8.27 | 5.74 | 5.54 | 6.36 | = |
| common_rng_next | Int (Bits), Ref | 0.51 | 10.67 | 20.92 | 20.55 | 16.72 | 12.87 | 6.38 | = |
| tiles_key_hash | Int64 | 0.46 | 0.59 | 1.29 | 1.28 | 1.12 | 1.68 | 1.63 | = |
| lamps_grid_key | Float->Int64 | 0.74 | 1.04 | 1.41 | 1.42 | 1.35 | 1.37 | 1.37 | = |
| mvt_zigzag | Int64 | 0.45 | 0.45 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | = |
| mvt_read_geometry | Bytes, Listen | 1.75 | 5.91 | 3.37 | 3.34 | 2.64 | 2.69 | 4.71 | = |
| wire_put_u32 | Bytes, Liste | 2.82 | 4.30 | 1.53 | 1.50 | 1.52 | 1.53 | 5.00 | = |
| wire_get_u32 | Bytes, Struktur | 0.36 | 1.60 | 4.46 | 4.42 | 3.88 | 3.95 | 1.46 | = |
| settings_escape | Text | 1.92 | 20.07 | 10.47 | 10.21 | 8.70 | 9.02 | 9.13 | = |
| settings_num | Float->Text | 11.13 | 13.90 | 1.25 | 1.26 | 1.27 | 1.29 | 1.28 | = |
| common_wrap_pi | Float, Schleife | 0.50 | 0.83 | 1.65 | 1.30 | 1.56 | 1.85 | 0.83 | f32 |
| common_damp_alpha | Float, exp | 1.44 | 1.50 | 1.04 | 1.04 | 1.05 | 1.05 | 1.51 | f32 |
| common_qnormalize | Quat | 1.69 | 2.02 | 1.20 | 1.19 | 1.19 | 1.20 | 1.20 | f32 |
| common_slerp | Quat, Bibl. | 12.58 | 14.99 | 1.19 | 1.19 | 1.19 | 1.19 | 14.97 | f32 |
| common_to_local | Vektor, Bibl. | 3.26 | 4.34 | 1.33 | 1.32 | 1.32 | 1.33 | 4.19 | f32 |
| common_rng_unit | Int->Float | 0.65 | 11.71 | 18.01 | 18.09 | 13.49 | 10.52 | 10.60 | = |
| person_hit | Vektor, Bibl. | 12.78 | 18.64 | 1.46 | 1.46 | 1.45 | 1.72 | 30.42 | f32 |
| person_sphere | Vektor | 1.61 | 1.56 | 0.97 | 0.98 | 0.93 | 0.92 | 0.98 | f32 |
| wire_put_f32 | Float->Bytes | 2.80 | 4.27 | 1.53 | 1.50 | 1.40 | 1.54 | 1.53 | = |
| mvt_ring_area | Liste v. Vektor | 0.23 | 0.77 | 3.39 | 3.48 | 3.62 | 4.22 | 0.77 | = |
| perf_governor | Struktur, Float | 1.37 | 2.18 | 1.59 | 1.47 | 1.44 | 1.43 | 1.88 | f32 |
| audio_render | Float, Liste | 109.23 | 181.30 | 1.66 | 1.67 | 1.51 | 1.43 | 153.42 | f32 |
| lamps_place_road | Map, Set, Liste | 4.00 | 4.76 | 1.19 | 1.22 | 1.19 | 1.19 | 4.50 | f32 |
| lamps_near | Map, Liste | 13.12 | 41.27 | 3.14 | 2.24 | 1.65 | 3.02 | 52.91 | f32 |
