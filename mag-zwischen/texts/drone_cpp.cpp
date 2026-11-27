struct drone_cpp : virtual kern::Library, virtual common {
  // ---- functions ----
  struct battery_under_loadResult { f32 Result = 0.0f; };
  KERN_PURE battery_under_loadResult battery_under_load(f32 ocv = 0.0f, f32 amps = 0.0f, f32 r = 0.0f, f32 cells = 0.0f) {
    return {common::clampf(kern::formula<f32, f32, f32>([](auto ocv, auto amps, auto r) { return (ocv - (fmax(0.0f, amps) * r)); }, ocv, amps, r), kern::formula<f32>([](auto cells) { return (3.0f * cells); }, cells), kern::formula<f32>([](auto cells) { return (4.2f * cells); }, cells)).Result};
  }

  void encode_sbus(const Fixed<u16, 16>& ch, Fixed<u8, 25>& p) {
    u8 fill = 0;
    i64 last = 0;
    i32 k = 0;
    fill = 0;
    last = math_sub<i64>(list_length<u8>(p), 1);
    for (auto it10 = flow_for(n10, 0, last); it10.next();) {
      p[n10.index] = fill;
    }
    p[0] = 15;
    for (auto it11 = flow_for(n11, 0, 15); it11.next();) {
      for (auto it12 = flow_for(n12, 0, 10); it12.next();) {
        if (kern::formula<u16, u32, i64>([](auto a, auto a1, auto index) { return num_cast<bool, u32>((num_cast<u32, u16>(a) & (a1 << num_cast<i32, i64>(index)))); }, ch[num_cast<i64, i32>(num_cast<i32, i64>(n11.index))], 1, n12.index)) {
          k = kern::formula<i64, i64>([](auto index, auto index_2) { return ((8 + (num_cast<i32, i64>(index) * 11)) + num_cast<i32, i64>(index_2)); }, n11.index, n12.index);
          p[num_cast<i64, u64>(kern::formula<i32>([](auto k) { return num_cast<u64, i32>((k / 8)); }, k))] = num_cast<u8, i32>(kern::formula<u8, u32, i32>([](auto a, auto a1, auto k) { return (num_cast<i32, u8>(a) | num_cast<i32, u8>(num_cast<u8, u32>((a1 << (k % 8))))); }, p[num_cast<i64, u64>(kern::formula<i32>([](auto k) { return num_cast<u64, i32>((k / 8)); }, k))], 1, k));
        }
      }
    }
  }

  /// a big-endian 16-bit register pair at `at`
  void put_be16(Fixed<u8, 14>& out, u32 at = 0, f32 v = 0.0f) {
    i64 r = 0;
    u16 u = 0;
    r = kern::formula<f32>([](auto the_result) { return num_cast<i64, f32>(round(the_result)); }, common::clampf(v, -32767.0f, 32767.0f).Result);
    u = num_cast<u16, i16>(num_cast<i16, i64>(r));
    out[num_cast<i64, u32>(at)] = kern::formula<u16>([](auto u) { return num_cast<u8, i32>((num_cast<i32, u16>(u) >> 8)); }, u);
    out[num_cast<i64, u64>(kern::formula<u32>([](auto at) { return num_cast<u64, u32>((at + 1)); }, at))] = kern::formula<u16>([](auto u) { return num_cast<u8, i32>((num_cast<i32, u16>(u) & 255)); }, u);
  }

  struct clearance_rateResult { f32 Result = 0.0f; };
  KERN_PURE clearance_rateResult clearance_rate(f32 axis = 0.0f) {
    f32 x = 0.0f;
    f32 m = 0.0f;
    f32 unit = 0.0f;
    x = common::clampf(axis, -1.0f, 1.0f).Result;
    m = kern::formula<f32>([](auto x) { return abs(x); }, x);
    if (math_compare<"<=", f32>(m, 0.08f)) {
      return {0.0f};
    }
    unit = kern::formula<f32>([](auto m) { return ((m - 0.08f) / 0.92f); }, m);
    return {kern::formula<f32, f32>([](auto the_result, auto unit) { return (((the_result * 12.0f) * unit) * (0.25f + (0.75f * unit))); }, common::sign0(x).Result, unit)};
  }

  struct clearance_leadResult { f32 Result = 0.0f; };
  KERN_PURE clearance_leadResult clearance_lead(f32 v = 0.0f) {
    bool up = false;
    f32 a = 0.0f;
    f32 tau = 0.0f;
    f32 k = 0.0f;
    f32 s = 0.0f;
    f32 e = 0.0f;
    up = math_compare<">=", f32>(v, 0.0f);
    a = kern::formula<bool, f32, f32>([](auto up, auto a0f, auto a0f_2) { return (up ? a0f : a0f_2); }, up, 6.0f, 4.0f);
    tau = kern::formula<bool, f32, f32>([](auto up, auto a25f, auto a125f) { return (up ? a25f : a125f); }, up, 0.25f, 0.125f);
    k = 2.0f;
    s = kern::formula<f32>([](auto v) { return abs(v); }, v);
    e = kern::formula<f32, f32, f32, f32>([](auto s, auto k, auto a, auto tau) { return fmax((s / k), (((s * s) + (((2.0f * a) * tau) * s)) / (2.0f * a))); }, s, k, a, tau);
    return {kern::formula<bool, f32>([](auto up, auto e) { return (up ? e : (-e)); }, up, e)};
  }

  struct step_centeredResult { u16 Result = 0; };
  KERN_PURE step_centeredResult step_centered(f32 v = 0.0f) {
    return {kern::formula<f32>([](auto the_result) { return num_cast<u16, i64>(num_cast<i64, f32>(round((992.0f + (820.0f * the_result))))); }, common::clampf(v, -1.0f, 1.0f).Result)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct : kern::Node<12> { i64 index = 0; } n12;
};
