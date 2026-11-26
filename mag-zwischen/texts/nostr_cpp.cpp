struct nostr_cpp : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(nostr_Hasher) { Fixed<U32, 8> h = List<U32>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}; Fixed<U8, 64> chunk = List<U8>{}; U32 filled = 0; Int64 length = Int64{0x0}; };
  KERN_STRUCT(nostr_U256) { Fixed<U32, 8> w = List<U32>{}; };
  KERN_STRUCT(nostr_Pt) { nostr_U256 x = nostr_U256{List<U32>{}}; nostr_U256 y = nostr_U256{List<U32>{}}; nostr_U256 z = nostr_U256{List<U32>{}}; };

  // ---- variables ----
  KERN_CONST Fixed<U32, 64> kK = List<U32>{1116352408, 1899447441, 3049323471, 3921009573, 961987163, 1508970993, 2453635748, 2870763221, 3624381080, 310598401, 607225278, 1426881987, 1925078388, 2162078206, 2614888103, 3248222580, 3835390401, 4022224774, 264347078, 604807628, 770255983, 1249150122, 1555081692, 1996064986, 2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993, 338241895, 666307205, 773529912, 1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037, 2730485921, 2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344, 430227734, 506948616, 659060556, 883997877, 958139571, 1322822218, 1537002063, 1747873779, 1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298};
  /// (the numbers of the curve, least significant word first: from_hex256 of the hex the standard gives - P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F, N = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
  KERN_CONST nostr_U256 kP = nostr_U256{List<U32>{4294966319, 4294967294, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295}};
  KERN_CONST nostr_U256 kN = nostr_U256{List<U32>{3493216577, 3218235020, 2940772411, 3132021990, 4294967294, 4294967295, 4294967295, 4294967295}};
  /// (the generator: x = 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798, y = 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8, z = 1)
  KERN_CONST nostr_Pt kG = nostr_Pt{nostr_U256{List<U32>{385357720, 1509065051, 768485593, 43777243, 3464956679, 1436574357, 4191992748, 2042521214}}, nostr_U256{List<U32>{4212184248, 2621952143, 2793755673, 4246189128, 235997352, 1571093500, 648266853, 1211816567}}, nostr_U256{List<U32>{1}}};
  KERN_CONST Fixed<Int, 17> kHexDigits = List<Int>{48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102, 0};

  // ---- functions ----
  struct rotrResult { U32 Result = 0; };
  KERN_PURE rotrResult rotr(U32 x = 0, U32 n = 0) {
    return {kern::to<Int>(kern::formula([](Float x, Float n) -> Float { return kern::math::bor(kern::math::shr(x, n), kern::math::wrap32(kern::math::shl(x, kern::math::wrap32(32.0 - n)))); }, kern::to<Float>(x), kern::to<Float>(n)))};
  }

  void block(Fixed<U32, 8>& h, const Fixed<U8, 64>& p) {
    Fixed<U32, 64> w = Fixed<U32, 64>{};
    U32 s0 = 0;
    U32 s1 = 0;
    U32 a = 0;
    U32 b = 0;
    U32 c = 0;
    U32 d = 0;
    U32 e = 0;
    U32 f = 0;
    U32 g = 0;
    U32 k = 0;
    U32 t1 = 0;
    U32 t2 = 0;
    w = List<U32>{};
    for (auto it10 = flow_for(n10, 0, 15); it10.next();) {
      w[n10.index] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 * index); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 1.0); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 2.0); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 3.0); }, kern::to<Float>(n10.index)))])));
    }
    for (auto it11 = flow_for(n11, 16, 63); it11.next();) {
      s0 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 3.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))], 7).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))], 18).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))])));
      s1 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 10.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))], 17).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))], 19).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))])));
      w[n11.index] = kern::to<Int>(kern::formula([](Float a, Float s0, Float a_2, Float s1) -> Float { return (kern::math::wrap32(kern::math::wrap32(a + s0) + a_2) + s1); }, kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 16.0); }, kern::to<Float>(n11.index)))]), kern::to<Float>(s0), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 7.0); }, kern::to<Float>(n11.index)))]), kern::to<Float>(s1)));
    }
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];
    f = h[5];
    g = h[6];
    k = h[7];
    for (auto it12 = flow_for(n12, 0, 63); it12.next();) {
      t1 = kern::to<Int>(kern::formula([](Float k, Float a, Float a_2, Float a_3, Float e, Float f, Float g, Float a_4, Float a_5) -> Float { return (kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(k + kern::math::bxor(kern::math::bxor(a, a_2), a_3)) + kern::math::bxor(kern::math::band(e, f), kern::math::band(kern::math::wrap32(kern::math::bxor(e, -1.0)), g))) + a_4) + a_5); }, kern::to<Float>(k), kern::to<Float>(rotr(e, 6).Result), kern::to<Float>(rotr(e, 11).Result), kern::to<Float>(rotr(e, 25).Result), kern::to<Float>(e), kern::to<Float>(f), kern::to<Float>(g), kern::to<Float>(list_get<U32>(kK, n12.index)), kern::to<Float>(w[n12.index])));
      t2 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4, Float b, Float c) -> Float { return (kern::math::bxor(kern::math::bxor(a, a_2), a_3) + kern::math::bxor(kern::math::bxor(kern::math::band(a_4, b), kern::math::band(a_4, c)), kern::math::band(b, c))); }, kern::to<Float>(rotr(a, 2).Result), kern::to<Float>(rotr(a, 13).Result), kern::to<Float>(rotr(a, 22).Result), kern::to<Float>(a), kern::to<Float>(b), kern::to<Float>(c)));
      k = g;
      g = f;
      f = e;
      e = kern::to<Int>(kern::formula([](Float d, Float t1) -> Float { return kern::math::wrap32(d + t1); }, kern::to<Float>(d), kern::to<Float>(t1)));
      d = c;
      c = b;
      b = a;
      a = kern::to<Int>(kern::formula([](Float t1, Float t2) -> Float { return kern::math::wrap32(t1 + t2); }, kern::to<Float>(t1), kern::to<Float>(t2)));
    }
    h[0] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, kern::to<Float>(h[0]), kern::to<Float>(a)));
    h[1] = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return (a + b); }, kern::to<Float>(h[1]), kern::to<Float>(b)));
    h[2] = kern::to<Int>(kern::formula([](Float a, Float c) -> Float { return (a + c); }, kern::to<Float>(h[2]), kern::to<Float>(c)));
    h[3] = kern::to<Int>(kern::formula([](Float a, Float d) -> Float { return (a + d); }, kern::to<Float>(h[3]), kern::to<Float>(d)));
    h[4] = kern::to<Int>(kern::formula([](Float a, Float e) -> Float { return (a + e); }, kern::to<Float>(h[4]), kern::to<Float>(e)));
    h[5] = kern::to<Int>(kern::formula([](Float a, Float f) -> Float { return (a + f); }, kern::to<Float>(h[5]), kern::to<Float>(f)));
    h[6] = kern::to<Int>(kern::formula([](Float a, Float g) -> Float { return (a + g); }, kern::to<Float>(h[6]), kern::to<Float>(g)));
    h[7] = kern::to<Int>(kern::formula([](Float a, Float k) -> Float { return (a + k); }, kern::to<Float>(h[7]), kern::to<Float>(k)));
  }

  void push(nostr_Hasher& s, U8 byte = 0) {
    s.chunk[s.filled] = byte;
    s.filled = kern::to<Int>(kern::formula([](Float filled) -> Float { return (filled + 1.0); }, kern::to<Float>(s.filled)));
    s.length = int64_add(s.length, Int64{0x1});
    if (math_compare<"==">(kern::to<Float>(s.filled), 64.0)) {
      block(s.h, s.chunk);
      s.filled = 0;
    }
  }

  struct finishResult { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  /// The padding (0x80, zeros, the length in bits, big endian) and the hash.
  finishResult finish(nostr_Hasher& s) {
    Int64 bits = Int64{0x0};
    I32 b = 0;
    Fixed<U8, 32> out = Fixed<U8, 32>{};
    bits = int64_mul(s.length, Int64{0x8});
    push(s, 128);
    while (loop(math_compare<"!=">(kern::to<Float>(s.filled), 56.0))) {
      push(s, 0);
    }
    b = 7;
    while (loop(math_compare<">=">(kern::to<Float>(b), 0.0))) {
      push(s, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(bits, kern::to<Int>(kern::formula([](Float b) -> Float { return (8.0 * b); }, kern::to<Float>(b)))))))));
      b = kern::to<Int>(kern::formula([](Float b) -> Float { return (b - 1.0); }, kern::to<Float>(b)));
    }
    out = List<U8>{};
    for (auto it13 = flow_for(n13, 0, 7); it13.next();) {
      for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
        out[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((4.0 * index) + index_2); }, kern::to<Float>(n13.index), kern::to<Float>(n14.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, (24.0 - (8.0 * index))), 255.0); }, kern::to<Float>(s.h[n13.index]), kern::to<Float>(n14.index)));
      }
    }
    return {out};
  }

  struct sha256Result { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  sha256Result sha256(const List<U8>& data) {
    nostr_Hasher s = nostr_Hasher{List<U32>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<U8>{}, 0, Int64{0x0}};
    U8 byte = 0;
    s = nostr_Hasher{List<U32>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<U8>{}, 0, Int64{0x0}};
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(data))))); it15.next();) {
      byte = data[n15.index];
      push(s, byte);
    }
    n16 = finish(s);
    return {n16.Result};
  }

  struct sha256_2Result { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  sha256_2Result sha256_2(Text text = "") {
    nostr_Hasher s = nostr_Hasher{List<U32>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<U8>{}, 0, Int64{0x0}};
    Int c = 0;
    s = nostr_Hasher{List<U32>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<U8>{}, 0, Int64{0x0}};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it17.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, n17.index))));
      push(s, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))));
    }
    n18 = finish(s);
    return {n18.Result};
  }

  struct from_beResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  KERN_PURE from_beResult from_be(const Fixed<U8, 32>& b) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    r = nostr_U256{List<U32>{}};
    for (auto it19 = flow_for(n19, 0, 7); it19.next();) {
      r.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (7.0 - index); }, kern::to<Float>(n19.index)))] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 * index); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 1.0); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 2.0); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 3.0); }, kern::to<Float>(n19.index)))])));
    }
    return {r};
  }

  struct to_beResult { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  KERN_PURE to_beResult to_be(const nostr_U256& a) {
    Fixed<U8, 32> b = Fixed<U8, 32>{};
    b = List<U8>{};
    for (auto it20 = flow_for(n20, 0, 7); it20.next();) {
      for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
        b[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((4.0 * index) + index_2); }, kern::to<Float>(n20.index), kern::to<Float>(n21.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, (24.0 - (8.0 * index))), 255.0); }, kern::to<Float>(a.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (7.0 - index); }, kern::to<Float>(n20.index)))]), kern::to<Float>(n21.index)));
      }
    }
    return {b};
  }

  struct halfResult { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  /// One half of a signature: its first (0) or its second (1) 32 bytes.
  KERN_PURE halfResult half(const Fixed<U8, 64>& sig, I32 which = 0) {
    Fixed<U8, 32> b = Fixed<U8, 32>{};
    b = List<U8>{};
    for (auto it22 = flow_for(n22, 0, 31); it22.next();) {
      b[n22.index] = sig[kern::to<Int>(kern::formula([](Float which, Float index) -> Float { return ((32.0 * which) + index); }, kern::to<Float>(which), kern::to<Float>(n22.index)))];
    }
    return {b};
  }

  struct from_hex_vResult { I32 Result = 0; };
  KERN_PURE from_hex_vResult from_hex_v(Int c = 0) {
    return {kern::to<Int>(kern::formula([](Float c) -> Float { return (((c >= 48.0) && (c <= 57.0)) ? (c - 48.0) : (((c >= 97.0) && (c <= 102.0)) ? ((c - 97.0) + 10.0) : (((c >= 65.0) && (c <= 70.0)) ? ((c - 65.0) + 10.0) : (-1.0)))); }, kern::to<Float>(c)))};
  }

  struct from_hexResult { Bool Result = false; };
  from_hexResult from_hex(Fixed<U8, 32>& out, Text s = "") {
    Int n = 0;
    I32 a = 0;
    I32 b = 0;
    n = list_length<U8>(out);
    if (math_compare<"!=">(kern::to<Float>(text_length(s)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (2.0 * n); }, kern::to<Float>(n))))))) {
      return {false};
    }
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it23.next();) {
      a = from_hex_v(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, kern::to<Int>(kern::formula([](Float index) -> Float { return (2.0 * index); }, kern::to<Float>(n23.index)))))))).Result;
      b = from_hex_v(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, kern::to<Int>(kern::formula([](Float index) -> Float { return ((2.0 * index) + 1.0); }, kern::to<Float>(n23.index)))))))).Result;
      if (logic_or(math_compare<"<">(kern::to<Float>(a), 0.0), math_compare<"<">(kern::to<Float>(b), 0.0))) {
        return {false};
      }
      out[n23.index] = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::band(((a * 16.0) + b), 255.0); }, kern::to<Float>(a), kern::to<Float>(b)));
    }
    return {true};
  }

  struct from_hex256Result { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  from_hex256Result from_hex256(Text s = "") {
    Fixed<U8, 32> b = Fixed<U8, 32>{};
    b = List<U8>{};
    n24 = from_hex(b, s);
    return {from_be(b).Result};
  }

  struct cmpResult { I32 Result = 0; };
  KERN_PURE cmpResult cmp(const nostr_U256& a, const nostr_U256& b) {
    I32 i = 0;
    i = 7;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      if (math_compare<"!=">(kern::to<Float>(a.w[i]), kern::to<Float>(b.w[i]))) {
        return {kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a < a_2) ? (-1.0) : 1.0); }, kern::to<Float>(a.w[i]), kern::to<Float>(b.w[i])))};
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {0};
  }

  struct is_zeroResult { Bool Result = false; };
  is_zeroResult is_zero(const nostr_U256& a) {
    U32 x = 0;
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(a.w))))); it25.next();) {
      x = a.w[n25.index];
      if (math_compare<"!=">(kern::to<Float>(x), 0.0)) {
        return {false};
      }
    }
    return {true};
  }

  struct addResult { U32 Result = 0; };
  addResult add(nostr_U256& r, const nostr_U256& a, const nostr_U256& b) {
    Int64 c = Int64{0x0};
    c = Int64{0x0};
    for (auto it26 = flow_for(n26, 0, 7); it26.next();) {
      c = int64_add(c, int64_add(int64_of_int(a.w[n26.index]), int64_of_int(b.w[n26.index])));
      r.w[n26.index] = int64_low(c);
      c = int64_shift_right_unsigned(c, 32);
    }
    return {int64_low(c)};
  }

  struct subResult { U32 Result = 0; };
  subResult sub(nostr_U256& r, const nostr_U256& a, const nostr_U256& b) {
    Int64 c = Int64{0x0};
    c = Int64{0x0};
    for (auto it27 = flow_for(n27, 0, 7); it27.next();) {
      c = int64_add(c, int64_sub(int64_of_int(a.w[n27.index]), int64_of_int(b.w[n27.index])));
      r.w[n27.index] = int64_low(c);
      c = int64_shift_right(c, 32);
    }
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(int64_less(c, Int64{0x0}))))};
  }

  struct addmResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  addmResult addm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    U32 carry = 0;
    r = nostr_U256{List<U32>{}};
    n28 = add(r, a, b);
    carry = n28.Result;
    if (logic_or(math_compare<"!=">(kern::to<Float>(carry), 0.0), math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0))) {
      n29 = sub(r, r, m);
    }
    return {r};
  }

  struct submResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  submResult subm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    r = nostr_U256{List<U32>{}};
    n30 = sub(r, a, b);
    if (math_compare<"!=">(kern::to<Float>(n30.Result), 0.0)) {
      n31 = add(r, r, m);
    }
    return {r};
  }

  struct mulmResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  mulmResult mulm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    I32 i = 0;
    r = nostr_U256{List<U32>{}};
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n32 = addm(r, r, m);
      r = n32.Result;
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return (kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0); }, kern::to<Float>(b.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n33 = addm(r, a, m);
        r = n33.Result;
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {r};
  }

  struct powmResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  powmResult powm(const nostr_U256& a, const nostr_U256& e, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    I32 i = 0;
    r = nostr_U256{List<U32>{}};
    r.w[0] = 1;
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n34 = mulm(r, r, m);
      r = n34.Result;
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return (kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0); }, kern::to<Float>(e.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n35 = mulm(r, a, m);
        r = n35.Result;
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {r};
  }

  struct reduceResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  reduceResult reduce(const nostr_U256& a, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<U32>{}};
    r = a;
    if (math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0)) {
      n36 = sub(r, r, m);
    }
    return {r};
  }

  struct fmResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  fmResult fm(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    a_2 = kP;
    n37 = mulm(a, b, a_2);
    return {n37.Result};
  }

  struct faResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  faResult fa(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    a_2 = kP;
    n38 = addm(a, b, a_2);
    return {n38.Result};
  }

  struct fsResult { nostr_U256 Result = nostr_U256{List<U32>{}}; };
  fsResult fs(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    a_2 = kP;
    n39 = subm(a, b, a_2);
    return {n39.Result};
  }

  struct dblResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}}; };
  dblResult dbl(const nostr_Pt& a) {
    Bool either = false;
    nostr_U256 y2 = nostr_U256{List<U32>{}};
    nostr_U256 s = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    nostr_U256 a_3 = nostr_U256{List<U32>{}};
    nostr_U256 a_4 = nostr_U256{List<U32>{}};
    nostr_U256 x2 = nostr_U256{List<U32>{}};
    nostr_U256 m = nostr_U256{List<U32>{}};
    nostr_U256 a_5 = nostr_U256{List<U32>{}};
    nostr_Pt r = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a_6 = nostr_U256{List<U32>{}};
    nostr_U256 a_7 = nostr_U256{List<U32>{}};
    nostr_U256 y4 = nostr_U256{List<U32>{}};
    nostr_U256 y4_8 = nostr_U256{List<U32>{}};
    nostr_U256 a_8 = nostr_U256{List<U32>{}};
    nostr_U256 a_9 = nostr_U256{List<U32>{}};
    nostr_U256 a_10 = nostr_U256{List<U32>{}};
    n40 = is_zero(a.z);
    either = n40.Result;
    if (logic_not(either)) {
      n42 = is_zero(a.y);
      either = n42.Result;
    }
    if (either) {
      return {struct_make<nostr_Pt>(struct_make<nostr_U256>(List<U32>{}), struct_make<nostr_U256>(List<U32>{}), struct_make<nostr_U256>(List<U32>{}))};
    }
    n43 = fm(a.y, a.y);
    y2 = n43.Result;
    n44 = fa(a.x, a.x);
    a_3 = n44.Result;
    n45 = fa(a.x, a.x);
    a_4 = n45.Result;
    n46 = fa(a_3, a_4);
    a_2 = n46.Result;
    n47 = fm(a_2, y2);
    s = n47.Result;
    n48 = fm(a.x, a.x);
    x2 = n48.Result;
    n49 = fa(x2, x2);
    a_5 = n49.Result;
    n50 = fa(a_5, x2);
    m = n50.Result;
    r = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    n51 = fm(m, m);
    a_6 = n51.Result;
    n52 = fa(s, s);
    a_7 = n52.Result;
    n53 = fs(a_6, a_7);
    r.x = n53.Result;
    n54 = fm(y2, y2);
    y4 = n54.Result;
    n55 = fa(y4, y4);
    y4_8 = n55.Result;
    n56 = fa(y4_8, y4_8);
    y4_8 = n56.Result;
    n57 = fa(y4_8, y4_8);
    y4_8 = n57.Result;
    n58 = fs(s, r.x);
    a_9 = n58.Result;
    n59 = fm(m, a_9);
    a_8 = n59.Result;
    n60 = fs(a_8, y4_8);
    r.y = n60.Result;
    n61 = fa(a.y, a.y);
    a_10 = n61.Result;
    n62 = fm(a_10, a.z);
    r.z = n62.Result;
    return {r};
  }

  struct add_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}}; };
  add_ptResult add_pt(const nostr_Pt& a, const nostr_Pt& b) {
    nostr_U256 z1z1 = nostr_U256{List<U32>{}};
    nostr_U256 z2z2 = nostr_U256{List<U32>{}};
    nostr_U256 u1 = nostr_U256{List<U32>{}};
    nostr_U256 u2 = nostr_U256{List<U32>{}};
    nostr_U256 s1 = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    nostr_U256 s2 = nostr_U256{List<U32>{}};
    nostr_U256 a_3 = nostr_U256{List<U32>{}};
    nostr_Pt chosen = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 h = nostr_U256{List<U32>{}};
    nostr_U256 r = nostr_U256{List<U32>{}};
    nostr_U256 h2 = nostr_U256{List<U32>{}};
    nostr_U256 h3 = nostr_U256{List<U32>{}};
    nostr_U256 u1h2 = nostr_U256{List<U32>{}};
    nostr_Pt o = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a_4 = nostr_U256{List<U32>{}};
    nostr_U256 a_5 = nostr_U256{List<U32>{}};
    nostr_U256 a_6 = nostr_U256{List<U32>{}};
    nostr_U256 a_7 = nostr_U256{List<U32>{}};
    nostr_U256 a_8 = nostr_U256{List<U32>{}};
    nostr_U256 a_9 = nostr_U256{List<U32>{}};
    nostr_U256 a_10 = nostr_U256{List<U32>{}};
    n63 = is_zero(a.z);
    if (n63.Result) {
      return {b};
    }
    n64 = is_zero(b.z);
    if (n64.Result) {
      return {a};
    }
    n65 = fm(a.z, a.z);
    z1z1 = n65.Result;
    n66 = fm(b.z, b.z);
    z2z2 = n66.Result;
    n67 = fm(a.x, z2z2);
    u1 = n67.Result;
    n68 = fm(b.x, z1z1);
    u2 = n68.Result;
    n69 = fm(b.z, z2z2);
    a_2 = n69.Result;
    n70 = fm(a.y, a_2);
    s1 = n70.Result;
    n71 = fm(a.z, z1z1);
    a_3 = n71.Result;
    n72 = fm(b.y, a_3);
    s2 = n72.Result;
    if (math_compare<"==">(kern::to<Float>(cmp(u1, u2).Result), 0.0)) {
      if (math_compare<"==">(kern::to<Float>(cmp(s1, s2).Result), 0.0)) {
        n74 = dbl(a);
        chosen = n74.Result;
      } else {
        chosen = struct_make<nostr_Pt>(struct_make<nostr_U256>(List<U32>{}), struct_make<nostr_U256>(List<U32>{}), struct_make<nostr_U256>(List<U32>{}));
      }
      return {chosen};
    }
    n75 = fs(u2, u1);
    h = n75.Result;
    n76 = fs(s2, s1);
    r = n76.Result;
    n77 = fm(h, h);
    h2 = n77.Result;
    n78 = fm(h2, h);
    h3 = n78.Result;
    n79 = fm(u1, h2);
    u1h2 = n79.Result;
    o = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    n80 = fm(r, r);
    a_5 = n80.Result;
    n81 = fs(a_5, h3);
    a_4 = n81.Result;
    n82 = fa(u1h2, u1h2);
    a_6 = n82.Result;
    n83 = fs(a_4, a_6);
    o.x = n83.Result;
    n84 = fs(u1h2, o.x);
    a_8 = n84.Result;
    n85 = fm(r, a_8);
    a_7 = n85.Result;
    n86 = fm(s1, h3);
    a_9 = n86.Result;
    n87 = fs(a_7, a_9);
    o.y = n87.Result;
    n88 = fm(a.z, b.z);
    a_10 = n88.Result;
    n89 = fm(h, a_10);
    o.z = n89.Result;
    return {o};
  }

  struct mul_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}}; };
  mul_ptResult mul_pt(const nostr_Pt& p, const nostr_U256& k) {
    nostr_Pt r = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    I32 i = 0;
    r = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n90 = dbl(r);
      r = n90.Result;
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return (kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0); }, kern::to<Float>(k.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n91 = add_pt(r, p);
        r = n91.Result;
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {r};
  }

  struct affineResult { Bool Result = false; };
  affineResult affine(const nostr_Pt& p, nostr_U256& x, nostr_U256& y) {
    nostr_U256 e = nostr_U256{List<U32>{}};
    nostr_U256 a = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    nostr_U256 zi = nostr_U256{List<U32>{}};
    nostr_U256 a_3 = nostr_U256{List<U32>{}};
    nostr_U256 zi2 = nostr_U256{List<U32>{}};
    nostr_U256 a_4 = nostr_U256{List<U32>{}};
    n92 = is_zero(p.z);
    if (n92.Result) {
      return {false};
    }
    e = nostr_U256{List<U32>{}};
    a = kP;
    a_2 = struct_make<nostr_U256>(list_make<U32, "8">(2, 0, 0, 0, 0, 0, 0, 0));
    n93 = sub(e, a, a_2);
    a_3 = kP;
    n94 = powm(p.z, e, a_3);
    zi = n94.Result;
    n95 = fm(zi, zi);
    zi2 = n95.Result;
    n96 = fm(p.x, zi2);
    x = n96.Result;
    n97 = fm(zi2, zi);
    a_4 = n97.Result;
    n98 = fm(p.y, a_4);
    y = n98.Result;
    return {true};
  }

  struct lift_xResult { Bool Result = false; };
  lift_xResult lift_x(const nostr_U256& x, nostr_Pt& out) {
    nostr_U256 a = nostr_U256{List<U32>{}};
    nostr_U256 seven = nostr_U256{List<U32>{}};
    nostr_U256 c = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    nostr_U256 a_3 = nostr_U256{List<U32>{}};
    nostr_U256 e = nostr_U256{List<U32>{}};
    nostr_U256 a_4 = nostr_U256{List<U32>{}};
    nostr_U256 a_5 = nostr_U256{List<U32>{}};
    nostr_U256 y = nostr_U256{List<U32>{}};
    nostr_U256 a_6 = nostr_U256{List<U32>{}};
    nostr_U256 a_7 = nostr_U256{List<U32>{}};
    nostr_U256 a_8 = nostr_U256{List<U32>{}};
    a = kP;
    if (math_compare<">=">(kern::to<Float>(cmp(x, a).Result), 0.0)) {
      return {false};
    }
    seven = nostr_U256{List<U32>{}};
    seven.w[0] = 7;
    n99 = fm(x, x);
    a_3 = n99.Result;
    n100 = fm(a_3, x);
    a_2 = n100.Result;
    n101 = fa(a_2, seven);
    c = n101.Result;
    e = nostr_U256{List<U32>{}};
    a_4 = kP;
    a_5 = struct_make<nostr_U256>(list_make<U32, "8">(1, 0, 0, 0, 0, 0, 0, 0));
    n102 = add(e, a_4, a_5);
    for (auto it103 = flow_for(n103, 0, 7); it103.next();) {
      e.w[n103.index] = kern::to<Int>(kern::formula([](Float a, Float index, Float a_2) -> Float { return kern::math::bor(kern::math::shr(a, 2.0), ((index < 7.0) ? kern::math::wrap32(kern::math::shl(a_2, 30.0)) : 0.0)); }, kern::to<Float>(e.w[n103.index]), kern::to<Float>(n103.index), kern::to<Float>(e.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n103.index)))])));
    }
    a_6 = kP;
    n104 = powm(c, e, a_6);
    y = n104.Result;
    n105 = fm(y, y);
    a_7 = n105.Result;
    if (math_compare<"!=">(kern::to<Float>(cmp(a_7, c).Result), 0.0)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(y.w[0])), 0.5)) {
      a_8 = struct_make<nostr_U256>(List<U32>{});
      n106 = fs(a_8, y);
      y = n106.Result;
    }
    out.x = x;
    out.y = y;
    out.z = struct_make<nostr_U256>(list_make<U32, "8">(1, 0, 0, 0, 0, 0, 0, 0));
    return {true};
  }

  struct taggedResult { Fixed<U8, 32> Result = Fixed<U8, 32>{}; };
  taggedResult tagged(const List<U8>& msg, Text tag = "") {
    Fixed<U8, 32> t = Fixed<U8, 32>{};
    List<U8> buf = List<U8>{};
    Int place = 0;
    Int place_2 = 0;
    n107 = sha256_2(tag);
    t = n107.Result;
    buf = t;
    place = list_length<U8>(buf);
    list_insert_list(buf, place, t);
    place_2 = list_length<U8>(buf);
    list_insert_list(buf, place_2, msg);
    n108 = sha256(buf);
    return {n108.Result};
  }

  void append(List<U8>& v, const Fixed<U8, 32>& b) {
    Int place = 0;
    place = list_length<U8>(v);
    list_insert_list(v, place, b);
  }

  struct public_keyResult { Bool Result = false; };
  public_keyResult public_key(const Fixed<U8, 32>& secret, Fixed<U8, 32>& out) {
    nostr_U256 d = nostr_U256{List<U32>{}};
    nostr_U256 a = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    Bool either = false;
    nostr_U256 x = nostr_U256{List<U32>{}};
    nostr_U256 y = nostr_U256{List<U32>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    d = from_be(secret).Result;
    n109 = is_zero(d);
    either = n109.Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    x = nostr_U256{List<U32>{}};
    y = nostr_U256{List<U32>{}};
    a_4 = kG;
    n110 = mul_pt(a_4, d);
    a_3 = n110.Result;
    n111 = affine(a_3, x, y);
    if (logic_not(n111.Result)) {
      return {false};
    }
    out = to_be(x).Result;
    return {true};
  }

  struct verifyResult { Bool Result = false; };
  verifyResult verify(const Fixed<U8, 32>& pubkey, const Fixed<U8, 32>& message, const Fixed<U8, 64>& sig) {
    nostr_Pt p = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a = nostr_U256{List<U32>{}};
    Fixed<U8, 32> rb = Fixed<U8, 32>{};
    nostr_U256 r = nostr_U256{List<U32>{}};
    nostr_U256 s = nostr_U256{List<U32>{}};
    Fixed<U8, 32> a_2 = Fixed<U8, 32>{};
    nostr_U256 a_3 = nostr_U256{List<U32>{}};
    nostr_U256 a_4 = nostr_U256{List<U32>{}};
    nostr_U256 a_5 = nostr_U256{List<U32>{}};
    Bool either = false;
    List<U8> c = List<U8>{};
    nostr_U256 e = nostr_U256{List<U32>{}};
    nostr_U256 a_6 = nostr_U256{List<U32>{}};
    Fixed<U8, 32> a_7 = Fixed<U8, 32>{};
    nostr_U256 a_8 = nostr_U256{List<U32>{}};
    nostr_Pt ep = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a_9 = nostr_U256{List<U32>{}};
    nostr_U256 x = nostr_U256{List<U32>{}};
    nostr_U256 y = nostr_U256{List<U32>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_Pt a_12 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    p = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    a = from_be(pubkey).Result;
    n129 = lift_x(a, p);
    if (logic_not(n129.Result)) {
      return {false};
    }
    rb = half(sig, 0).Result;
    r = from_be(rb).Result;
    a_2 = half(sig, 1).Result;
    s = from_be(a_2).Result;
    a_3 = kP;
    either = math_compare<">=">(kern::to<Float>(cmp(r, a_3).Result), 0.0);
    if (logic_not(either)) {
      a_5 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(s, a_5).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    c = List<U8>{};
    append(c, rb);
    append(c, pubkey);
    append(c, message);
    n130 = tagged(c, "BIP0340/challenge");
    a_7 = n130.Result;
    a_6 = from_be(a_7).Result;
    a_8 = kN;
    n131 = reduce(a_6, a_8);
    e = n131.Result;
    n132 = mul_pt(p, e);
    ep = n132.Result;
    a_9 = struct_make<nostr_U256>(List<U32>{});
    n133 = fs(a_9, ep.y);
    ep.y = n133.Result;
    x = nostr_U256{List<U32>{}};
    y = nostr_U256{List<U32>{}};
    a_12 = kG;
    n134 = mul_pt(a_12, s);
    a_11 = n134.Result;
    n135 = add_pt(a_11, ep);
    a_10 = n135.Result;
    n136 = affine(a_10, x, y);
    if (logic_not(n136.Result)) {
      return {false};
    }
    return {logic_and(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0); }, kern::to<Float>(y.w[0]))))), 0.0), math_compare<"==">(kern::to<Float>(cmp(x, r).Result), 0.0))};
  }

  struct the_signResult { Bool Result = false; };
  the_signResult the_sign(const Fixed<U8, 32>& secret, const Fixed<U8, 32>& message, const Fixed<U8, 32>& aux, Fixed<U8, 64>& sig) {
    nostr_U256 d = nostr_U256{List<U32>{}};
    nostr_U256 a = nostr_U256{List<U32>{}};
    nostr_U256 a_2 = nostr_U256{List<U32>{}};
    Bool either = false;
    nostr_U256 px = nostr_U256{List<U32>{}};
    nostr_U256 py = nostr_U256{List<U32>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a_5 = nostr_U256{List<U32>{}};
    nostr_U256 a_6 = nostr_U256{List<U32>{}};
    Fixed<U8, 32> pxb = Fixed<U8, 32>{};
    Fixed<U8, 32> db = Fixed<U8, 32>{};
    Fixed<U8, 32> ah = Fixed<U8, 32>{};
    List<U8> a_7 = List<U8>{};
    List<U8> m = List<U8>{};
    Fixed<U8, 32> rand = Fixed<U8, 32>{};
    nostr_U256 k = nostr_U256{List<U32>{}};
    nostr_U256 a_8 = nostr_U256{List<U32>{}};
    nostr_U256 a_9 = nostr_U256{List<U32>{}};
    nostr_U256 rx = nostr_U256{List<U32>{}};
    nostr_U256 ry = nostr_U256{List<U32>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}, nostr_U256{List<U32>{}}};
    nostr_U256 a_12 = nostr_U256{List<U32>{}};
    nostr_U256 a_13 = nostr_U256{List<U32>{}};
    Fixed<U8, 32> rxb = Fixed<U8, 32>{};
    List<U8> c = List<U8>{};
    nostr_U256 e = nostr_U256{List<U32>{}};
    nostr_U256 a_14 = nostr_U256{List<U32>{}};
    Fixed<U8, 32> a_15 = Fixed<U8, 32>{};
    nostr_U256 a_16 = nostr_U256{List<U32>{}};
    nostr_U256 s = nostr_U256{List<U32>{}};
    nostr_U256 a_17 = nostr_U256{List<U32>{}};
    nostr_U256 a_18 = nostr_U256{List<U32>{}};
    nostr_U256 a_19 = nostr_U256{List<U32>{}};
    Fixed<U8, 32> sb = Fixed<U8, 32>{};
    d = from_be(secret).Result;
    n112 = is_zero(d);
    either = n112.Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    px = nostr_U256{List<U32>{}};
    py = nostr_U256{List<U32>{}};
    a_4 = kG;
    n113 = mul_pt(a_4, d);
    a_3 = n113.Result;
    n114 = affine(a_3, px, py);
    if (logic_not(n114.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(py.w[0])), 0.5)) {
      a_5 = struct_make<nostr_U256>(List<U32>{});
      a_6 = kN;
      n115 = subm(a_5, d, a_6);
      d = n115.Result;
    }
    pxb = to_be(px).Result;
    db = to_be(d).Result;
    a_7 = aux;
    n116 = tagged(a_7, "BIP0340/aux");
    ah = n116.Result;
    m = List<U8>{};
    for (auto it117 = flow_for(n117, 0, 31); it117.next();) {
      list_add(m, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(db[n117.index]), kern::to<Float>(ah[n117.index]))));
    }
    append(m, pxb);
    append(m, message);
    n118 = tagged(m, "BIP0340/nonce");
    rand = n118.Result;
    a_8 = from_be(rand).Result;
    a_9 = kN;
    n119 = reduce(a_8, a_9);
    k = n119.Result;
    n120 = is_zero(k);
    if (n120.Result) {
      return {false};
    }
    rx = nostr_U256{List<U32>{}};
    ry = nostr_U256{List<U32>{}};
    a_11 = kG;
    n121 = mul_pt(a_11, k);
    a_10 = n121.Result;
    n122 = affine(a_10, rx, ry);
    if (logic_not(n122.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(ry.w[0])), 0.5)) {
      a_12 = struct_make<nostr_U256>(List<U32>{});
      a_13 = kN;
      n123 = subm(a_12, k, a_13);
      k = n123.Result;
    }
    rxb = to_be(rx).Result;
    c = List<U8>{};
    append(c, rxb);
    append(c, pxb);
    append(c, message);
    n124 = tagged(c, "BIP0340/challenge");
    a_15 = n124.Result;
    a_14 = from_be(a_15).Result;
    a_16 = kN;
    n125 = reduce(a_14, a_16);
    e = n125.Result;
    a_18 = kN;
    n126 = mulm(e, d, a_18);
    a_17 = n126.Result;
    a_19 = kN;
    n127 = addm(k, a_17, a_19);
    s = n127.Result;
    sb = to_be(s).Result;
    for (auto it128 = flow_for(n128, 0, 31); it128.next();) {
      sig[n128.index] = rxb[n128.index];
      sig[kern::to<Int>(kern::formula([](Float index) -> Float { return (32.0 + index); }, kern::to<Float>(n128.index)))] = sb[n128.index];
    }
    n137 = verify(pxb, message, sig);
    return {n137.Result};
  }

  void hex_of(Text& o, U8 byte = 0) {
    o = text_append(o, text_of_code(list_get<Int>(kHexDigits, kern::to<Int>(kern::formula([](Float byte) -> Float { return kern::math::shr(byte, 4.0); }, kern::to<Float>(byte))))), "");
    o = text_append(o, text_of_code(list_get<Int>(kHexDigits, kern::to<Int>(kern::formula([](Float byte) -> Float { return kern::math::band(byte, 15.0); }, kern::to<Float>(byte))))), "");
  }

  struct hexResult { Text Result = ""; };
  hexResult hex(const Fixed<U8, 32>& data) {
    Text o = "";
    U8 byte = 0;
    o = "";
    for (auto it138 = flow_for(n138, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(data))))); it138.next();) {
      byte = data[n138.index];
      hex_of(o, byte);
    }
    return {o};
  }

  struct hex_2Result { Text Result = ""; };
  hex_2Result hex_2(const Fixed<U8, 64>& data) {
    Text o = "";
    U8 byte = 0;
    o = "";
    for (auto it139 = flow_for(n139, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(data))))); it139.next();) {
      byte = data[n139.index];
      hex_of(o, byte);
    }
    return {o};
  }

  struct escapeResult { Text Result = ""; };
  escapeResult escape(Text s = "") {
    Text o = "";
    Int c = 0;
    Int selected = 0;
    Text b = "";
    o = "";
    for (auto it140 = flow_for(n140, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it140.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n140.index))));
      selected = c;
      if (math_compare<"==">(kern::to<Float>(selected), 10.0)) {
        o = text_append(o, "\\n", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 34.0)) {
        o = text_append(o, "\\\"", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 92.0)) {
        o = text_append(o, "\\\\", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 13.0)) {
        o = text_append(o, "\\r", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 9.0)) {
        o = text_append(o, "\\t", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 8.0)) {
        o = text_append(o, "\\b", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 12.0)) {
        o = text_append(o, "\\f", "");
      } else {
        if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
          b = "";
          b = text_format<"\\u%04x">(7, c);
          o = text_append(o, b, "");
        } else {
          o = text_append(o, text_of_code(c), "");
        }
      }
    }
    return {o};
  }

  struct event_jsonResult { Bool Result = false; };
  event_jsonResult event_json(const Fixed<U8, 32>& secret, const Fixed<U8, 32>& aux, Text& out, Int64 created_at = Int64{0x0}, U32 kind = 0, Text tags = "", Text content = "") {
    Fixed<U8, 32> pk = Fixed<U8, 32>{};
    Text pkh = "";
    Text body = "";
    Text head = "";
    Text ser = "";
    Fixed<U8, 32> id = Fixed<U8, 32>{};
    Fixed<U8, 64> sig = Fixed<U8, 64>{};
    Text num = "";
    pk = List<U8>{};
    n141 = public_key(secret, pk);
    if (logic_not(n141.Result)) {
      return {false};
    }
    n142 = hex(pk);
    pkh = n142.Result;
    n143 = escape(content);
    body = n143.Result;
    head = "";
    head = text_format<"[0,\"%s\",%s,%u,">(95, pkh, int64_text_signed(created_at), kind);
    ser = text_append(text_append(text_append(text_append(head, tags, ""), ",\"", ""), body, ""), "\"]", "");
    n144 = sha256_2(ser);
    id = n144.Result;
    sig = List<U8>{};
    n145 = the_sign(secret, id, aux, sig);
    if (logic_not(n145.Result)) {
      return {false};
    }
    num = "";
    num = text_format<",\"created_at\":%s,\"kind\":%u,\"tags\":">(63, int64_text_signed(created_at), kind);
    n146 = hex(id);
    n147 = hex_2(sig);
    out = text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append("{\"id\":\"", n146.Result, ""), "\",\"pubkey\":\"", ""), pkh, ""), "\"", ""), num, ""), tags, ""), ",\"content\":\"", ""), body, ""), "\",\"sig\":\"", ""), n147.Result, ""), "\"}", "");
    return {true};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  finishResult n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  finishResult n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  from_hexResult n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  addResult n28;
  subResult n29;
  subResult n30;
  addResult n31;
  addmResult n32;
  addmResult n33;
  mulmResult n34;
  mulmResult n35;
  subResult n36;
  mulmResult n37;
  addmResult n38;
  submResult n39;
  is_zeroResult n40;
  is_zeroResult n42;
  fmResult n43;
  faResult n44;
  faResult n45;
  faResult n46;
  fmResult n47;
  fmResult n48;
  faResult n49;
  faResult n50;
  fmResult n51;
  faResult n52;
  fsResult n53;
  fmResult n54;
  faResult n55;
  faResult n56;
  faResult n57;
  fsResult n58;
  fmResult n59;
  fsResult n60;
  faResult n61;
  fmResult n62;
  is_zeroResult n63;
  is_zeroResult n64;
  fmResult n65;
  fmResult n66;
  fmResult n67;
  fmResult n68;
  fmResult n69;
  fmResult n70;
  fmResult n71;
  fmResult n72;
  dblResult n74;
  fsResult n75;
  fsResult n76;
  fmResult n77;
  fmResult n78;
  fmResult n79;
  fmResult n80;
  fsResult n81;
  faResult n82;
  fsResult n83;
  fsResult n84;
  fmResult n85;
  fmResult n86;
  fsResult n87;
  fmResult n88;
  fmResult n89;
  dblResult n90;
  add_ptResult n91;
  is_zeroResult n92;
  subResult n93;
  powmResult n94;
  fmResult n95;
  fmResult n96;
  fmResult n97;
  fmResult n98;
  fmResult n99;
  fmResult n100;
  faResult n101;
  addResult n102;
  struct : kern::Node<103> { Int index = 0; } n103;
  powmResult n104;
  fmResult n105;
  fsResult n106;
  sha256_2Result n107;
  sha256Result n108;
  is_zeroResult n109;
  mul_ptResult n110;
  affineResult n111;
  is_zeroResult n112;
  mul_ptResult n113;
  affineResult n114;
  submResult n115;
  taggedResult n116;
  struct : kern::Node<117> { Int index = 0; } n117;
  taggedResult n118;
  reduceResult n119;
  is_zeroResult n120;
  mul_ptResult n121;
  affineResult n122;
  submResult n123;
  taggedResult n124;
  reduceResult n125;
  mulmResult n126;
  addmResult n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  lift_xResult n129;
  taggedResult n130;
  reduceResult n131;
  mul_ptResult n132;
  fsResult n133;
  mul_ptResult n134;
  add_ptResult n135;
  affineResult n136;
  verifyResult n137;
  struct : kern::Node<138> { Int index = 0; } n138;
  struct : kern::Node<139> { Int index = 0; } n139;
  struct : kern::Node<140> { Int index = 0; } n140;
  public_keyResult n141;
  hexResult n142;
  escapeResult n143;
  sha256_2Result n144;
  the_signResult n145;
  hexResult n146;
  hex_2Result n147;
};
