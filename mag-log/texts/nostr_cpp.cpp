struct nostr_cpp : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(nostr_Hasher) { Fixed<Int, 8> h = List<Int>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}; Fixed<Int, 64> chunk = List<Int>{}; Int filled = 0; Int64 length = Int64{0x0}; };
  KERN_STRUCT(nostr_U256) { Fixed<Int, 8> w = List<Int>{}; };
  KERN_STRUCT(nostr_Pt) { nostr_U256 x = nostr_U256{List<Int>{}}; nostr_U256 y = nostr_U256{List<Int>{}}; nostr_U256 z = nostr_U256{List<Int>{}}; };

  // ---- variables ----
  KERN_CONST Fixed<Int, 64> kK = List<Int>{1116352408, 1899447441, 3049323471, 3921009573, 961987163, 1508970993, 2453635748, 2870763221, 3624381080, 310598401, 607225278, 1426881987, 1925078388, 2162078206, 2614888103, 3248222580, 3835390401, 4022224774, 264347078, 604807628, 770255983, 1249150122, 1555081692, 1996064986, 2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993, 338241895, 666307205, 773529912, 1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037, 2730485921, 2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344, 430227734, 506948616, 659060556, 883997877, 958139571, 1322822218, 1537002063, 1747873779, 1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298};
  /// (the numbers of the curve, least significant word first: from_hex256 of the hex the standard gives - P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F, N = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
  KERN_CONST nostr_U256 kP = nostr_U256{List<Int>{4294966319, 4294967294, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295}};
  KERN_CONST nostr_U256 kN = nostr_U256{List<Int>{3493216577, 3218235020, 2940772411, 3132021990, 4294967294, 4294967295, 4294967295, 4294967295}};
  /// (the generator: x = 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798, y = 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8, z = 1)
  KERN_CONST nostr_Pt kG = nostr_Pt{nostr_U256{List<Int>{385357720, 1509065051, 768485593, 43777243, 3464956679, 1436574357, 4191992748, 2042521214}}, nostr_U256{List<Int>{4212184248, 2621952143, 2793755673, 4246189128, 235997352, 1571093500, 648266853, 1211816567}}, nostr_U256{List<Int>{1}}};
  KERN_CONST Fixed<Int, 17> kHexDigits = List<Int>{48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102, 0};

  // ---- functions ----
  struct rotrResult { Int Result = 0; };
  KERN_PURE rotrResult rotr(Int x = 0, Int n = 0) {
    return {kern::to<Int>(kern::formula([](Float x, Float n) -> Float { return kern::math::bor(kern::math::shr(x, n), kern::math::wrap32(kern::math::shl(x, kern::math::wrap32(32.0 - n)))); }, kern::to<Float>(x), kern::to<Float>(n)))};
  }

  void block(Fixed<Int, 8>& h, const Fixed<Int, 64>& p) {
    Fixed<Int, 64> w = Fixed<Int, 64>{};
    Int s0 = 0;
    Int s1 = 0;
    Int a = 0;
    Int b = 0;
    Int c = 0;
    Int d = 0;
    Int e = 0;
    Int f = 0;
    Int g = 0;
    Int k = 0;
    Int t1 = 0;
    Int t2 = 0;
    w = List<Int>{};
    for (auto it10 = flow_for(n10, 0, 15); it10.next();) {
      w[n10.index] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 * index); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 1.0); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 2.0); }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 3.0); }, kern::to<Float>(n10.index)))])));
    }
    for (auto it11 = flow_for(n11, 16, 63); it11.next();) {
      s0 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 3.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))], 7).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))], 18).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 15.0); }, kern::to<Float>(n11.index)))])));
      s1 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 10.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))], 17).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))], 19).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 2.0); }, kern::to<Float>(n11.index)))])));
      w[n11.index] = kern::to<Int>(kern::formula([](Float a, Float s0, Float a_2, Float s1) -> Float { return kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(a + s0) + a_2) + s1); }, kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 16.0); }, kern::to<Float>(n11.index)))]), kern::to<Float>(s0), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 7.0); }, kern::to<Float>(n11.index)))]), kern::to<Float>(s1)));
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
      t1 = kern::to<Int>(kern::formula([](Float k, Float a, Float a_2, Float a_3, Float e, Float f, Float g, Float a_4, Float a_5) -> Float { return kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(k + kern::math::bxor(kern::math::bxor(a, a_2), a_3)) + kern::math::bxor(kern::math::band(e, f), kern::math::band(kern::math::wrap32(kern::math::bxor(e, -1.0)), g))) + a_4) + a_5); }, kern::to<Float>(k), kern::to<Float>(rotr(e, 6).Result), kern::to<Float>(rotr(e, 11).Result), kern::to<Float>(rotr(e, 25).Result), kern::to<Float>(e), kern::to<Float>(f), kern::to<Float>(g), kern::to<Float>(list_get<Int>(kK, n12.index)), kern::to<Float>(w[n12.index])));
      t2 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4, Float b, Float c) -> Float { return kern::math::wrap32(kern::math::bxor(kern::math::bxor(a, a_2), a_3) + kern::math::bxor(kern::math::bxor(kern::math::band(a_4, b), kern::math::band(a_4, c)), kern::math::band(b, c))); }, kern::to<Float>(rotr(a, 2).Result), kern::to<Float>(rotr(a, 13).Result), kern::to<Float>(rotr(a, 22).Result), kern::to<Float>(a), kern::to<Float>(b), kern::to<Float>(c)));
      k = g;
      g = f;
      f = e;
      e = kern::to<Int>(kern::formula([](Float d, Float t1) -> Float { return kern::math::wrap32(d + t1); }, kern::to<Float>(d), kern::to<Float>(t1)));
      d = c;
      c = b;
      b = a;
      a = kern::to<Int>(kern::formula([](Float t1, Float t2) -> Float { return kern::math::wrap32(t1 + t2); }, kern::to<Float>(t1), kern::to<Float>(t2)));
    }
    h[0] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::wrap32(a + a_2); }, kern::to<Float>(h[0]), kern::to<Float>(a)));
    h[1] = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::wrap32(a + b); }, kern::to<Float>(h[1]), kern::to<Float>(b)));
    h[2] = kern::to<Int>(kern::formula([](Float a, Float c) -> Float { return kern::math::wrap32(a + c); }, kern::to<Float>(h[2]), kern::to<Float>(c)));
    h[3] = kern::to<Int>(kern::formula([](Float a, Float d) -> Float { return kern::math::wrap32(a + d); }, kern::to<Float>(h[3]), kern::to<Float>(d)));
    h[4] = kern::to<Int>(kern::formula([](Float a, Float e) -> Float { return kern::math::wrap32(a + e); }, kern::to<Float>(h[4]), kern::to<Float>(e)));
    h[5] = kern::to<Int>(kern::formula([](Float a, Float f) -> Float { return kern::math::wrap32(a + f); }, kern::to<Float>(h[5]), kern::to<Float>(f)));
    h[6] = kern::to<Int>(kern::formula([](Float a, Float g) -> Float { return kern::math::wrap32(a + g); }, kern::to<Float>(h[6]), kern::to<Float>(g)));
    h[7] = kern::to<Int>(kern::formula([](Float a, Float k) -> Float { return kern::math::wrap32(a + k); }, kern::to<Float>(h[7]), kern::to<Float>(k)));
  }

  void push(nostr_Hasher& s, Int byte = 0) {
    s.chunk[s.filled] = byte;
    s.filled = kern::to<Int>(kern::formula([](Float filled) -> Float { return kern::math::wrap32(filled + 1.0); }, kern::to<Float>(s.filled)));
    s.length = int64_add(s.length, Int64{0x1});
    if (math_compare<"==">(kern::to<Float>(s.filled), 64.0)) {
      block(s.h, s.chunk);
      s.filled = 0;
    }
  }

  struct finishResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  /// The padding (0x80, zeros, the length in bits, big endian) and the hash.
  finishResult finish(nostr_Hasher& s) {
    Int64 bits = Int64{0x0};
    Int b = 0;
    Fixed<Int, 32> out = Fixed<Int, 32>{};
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
    out = List<Int>{};
    for (auto it13 = flow_for(n13, 0, 7); it13.next();) {
      for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
        out[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((4.0 * index) + index_2); }, kern::to<Float>(n13.index), kern::to<Float>(n14.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, (24.0 - (8.0 * index))), 255.0); }, kern::to<Float>(s.h[n13.index]), kern::to<Float>(n14.index)));
      }
    }
    return {out};
  }

  struct sha256Result { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  sha256Result sha256(const List<Int>& data) {
    nostr_Hasher s = nostr_Hasher{List<Int>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<Int>{}, 0, Int64{0x0}};
    Int byte = 0;
    s = nostr_Hasher{List<Int>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<Int>{}, 0, Int64{0x0}};
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it15.next();) {
      byte = data[n15.index];
      push(s, byte);
    }
    n16 = finish(s);
    return {n16.Result};
  }

  struct sha256_2Result { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  sha256_2Result sha256_2(Text text = "") {
    nostr_Hasher s = nostr_Hasher{List<Int>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<Int>{}, 0, Int64{0x0}};
    Int c = 0;
    s = nostr_Hasher{List<Int>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, List<Int>{}, 0, Int64{0x0}};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it17.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, n17.index))));
      push(s, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))));
    }
    n18 = finish(s);
    return {n18.Result};
  }

  struct from_beResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  KERN_PURE from_beResult from_be(const Fixed<Int, 32>& b) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    r = nostr_U256{List<Int>{}};
    for (auto it19 = flow_for(n19, 0, 7); it19.next();) {
      r.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (7.0 - index); }, kern::to<Float>(n19.index)))] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 * index); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 1.0); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 2.0); }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 3.0); }, kern::to<Float>(n19.index)))])));
    }
    return {r};
  }

  struct to_beResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  KERN_PURE to_beResult to_be(const nostr_U256& a) {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    for (auto it20 = flow_for(n20, 0, 7); it20.next();) {
      for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
        b[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((4.0 * index) + index_2); }, kern::to<Float>(n20.index), kern::to<Float>(n21.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, (24.0 - (8.0 * index))), 255.0); }, kern::to<Float>(a.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (7.0 - index); }, kern::to<Float>(n20.index)))]), kern::to<Float>(n21.index)));
      }
    }
    return {b};
  }

  struct halfResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  /// One half of a signature: its first (0) or its second (1) 32 bytes.
  KERN_PURE halfResult half(const Fixed<Int, 64>& sig, Int which = 0) {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    for (auto it22 = flow_for(n22, 0, 31); it22.next();) {
      b[n22.index] = sig[kern::to<Int>(kern::formula([](Float which, Float index) -> Float { return ((32.0 * which) + index); }, kern::to<Float>(which), kern::to<Float>(n22.index)))];
    }
    return {b};
  }

  struct from_hex_vResult { Int Result = 0; };
  KERN_PURE from_hex_vResult from_hex_v(Int c = 0) {
    return {kern::to<Int>(kern::formula([](Float c) -> Float { return (((c >= 48.0) && (c <= 57.0)) ? (c - 48.0) : (((c >= 97.0) && (c <= 102.0)) ? ((c - 97.0) + 10.0) : (((c >= 65.0) && (c <= 70.0)) ? ((c - 65.0) + 10.0) : (-1.0)))); }, kern::to<Float>(c)))};
  }

  struct from_hexResult { Bool Result = false; };
  from_hexResult from_hex(Fixed<Int, 32>& out, Text s = "") {
    Int n = 0;
    Int a = 0;
    Int b = 0;
    n = list_length<Int>(out);
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

  struct from_hex256Result { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  from_hex256Result from_hex256(Text s = "") {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    n24 = from_hex(b, s);
    return {from_be(b).Result};
  }

  struct cmpResult { Int Result = 0; };
  KERN_PURE cmpResult cmp(const nostr_U256& a, const nostr_U256& b) {
    Int i = 0;
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
  KERN_PURE is_zeroResult is_zero(const nostr_U256& a) {
    Int x = 0;
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(a.w))))); it25.next();) {
      x = a.w[n25.index];
      if (math_compare<"!=">(kern::to<Float>(x), 0.0)) {
        return {false};
      }
    }
    return {true};
  }

  struct addResult { Int Result = 0; };
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

  struct subResult { Int Result = 0; };
  subResult sub(nostr_U256& r, const nostr_U256& a, const nostr_U256& b) {
    Int64 c = Int64{0x0};
    c = Int64{0x0};
    for (auto it27 = flow_for(n27, 0, 7); it27.next();) {
      c = int64_add(c, int64_sub(int64_of_int(a.w[n27.index]), int64_of_int(b.w[n27.index])));
      r.w[n27.index] = int64_low(c);
      c = int64_shift_right(c, 32);
    }
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(int64_less(c, Int64{0x0}))))};
  }

  struct addmResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  addmResult addm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    Int carry = 0;
    r = nostr_U256{List<Int>{}};
    n28 = add(r, a, b);
    carry = n28.Result;
    if (logic_or(math_compare<"!=">(kern::to<Float>(carry), 0.0), math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0))) {
      n29 = sub(r, r, m);
    }
    return {r};
  }

  struct submResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  submResult subm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    r = nostr_U256{List<Int>{}};
    n30 = sub(r, a, b);
    if (math_compare<"!=">(kern::to<Float>(n30.Result), 0.0)) {
      n31 = add(r, r, m);
    }
    return {r};
  }

  struct mulmResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  mulmResult mulm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    Int i = 0;
    r = nostr_U256{List<Int>{}};
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

  struct powmResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  powmResult powm(const nostr_U256& a, const nostr_U256& e, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    Int i = 0;
    r = nostr_U256{List<Int>{}};
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

  struct reduceResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  reduceResult reduce(const nostr_U256& a, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{List<Int>{}};
    r = a;
    if (math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0)) {
      n36 = sub(r, r, m);
    }
    return {r};
  }

  struct fmResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  fmResult fm(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    a_2 = kP;
    n37 = mulm(a, b, a_2);
    return {n37.Result};
  }

  struct faResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  faResult fa(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    a_2 = kP;
    n38 = addm(a, b, a_2);
    return {n38.Result};
  }

  struct fsResult { nostr_U256 Result = nostr_U256{List<Int>{}}; };
  fsResult fs(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    a_2 = kP;
    n39 = subm(a, b, a_2);
    return {n39.Result};
  }

  struct dblResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}}; };
  dblResult dbl(const nostr_Pt& a) {
    nostr_U256 y2 = nostr_U256{List<Int>{}};
    nostr_U256 s = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    nostr_U256 a_3 = nostr_U256{List<Int>{}};
    nostr_U256 a_4 = nostr_U256{List<Int>{}};
    nostr_U256 x2 = nostr_U256{List<Int>{}};
    nostr_U256 m = nostr_U256{List<Int>{}};
    nostr_U256 a_5 = nostr_U256{List<Int>{}};
    nostr_Pt r = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a_6 = nostr_U256{List<Int>{}};
    nostr_U256 a_7 = nostr_U256{List<Int>{}};
    nostr_U256 y4 = nostr_U256{List<Int>{}};
    nostr_U256 y4_8 = nostr_U256{List<Int>{}};
    nostr_U256 a_8 = nostr_U256{List<Int>{}};
    nostr_U256 a_9 = nostr_U256{List<Int>{}};
    nostr_U256 a_10 = nostr_U256{List<Int>{}};
    if (logic_or(is_zero(a.z).Result, is_zero(a.y).Result)) {
      return {struct_make<nostr_Pt>(struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}))};
    }
    n40 = fm(a.y, a.y);
    y2 = n40.Result;
    n41 = fa(a.x, a.x);
    a_3 = n41.Result;
    n42 = fa(a.x, a.x);
    a_4 = n42.Result;
    n43 = fa(a_3, a_4);
    a_2 = n43.Result;
    n44 = fm(a_2, y2);
    s = n44.Result;
    n45 = fm(a.x, a.x);
    x2 = n45.Result;
    n46 = fa(x2, x2);
    a_5 = n46.Result;
    n47 = fa(a_5, x2);
    m = n47.Result;
    r = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    n48 = fm(m, m);
    a_6 = n48.Result;
    n49 = fa(s, s);
    a_7 = n49.Result;
    n50 = fs(a_6, a_7);
    r.x = n50.Result;
    n51 = fm(y2, y2);
    y4 = n51.Result;
    n52 = fa(y4, y4);
    y4_8 = n52.Result;
    n53 = fa(y4_8, y4_8);
    y4_8 = n53.Result;
    n54 = fa(y4_8, y4_8);
    y4_8 = n54.Result;
    n55 = fs(s, r.x);
    a_9 = n55.Result;
    n56 = fm(m, a_9);
    a_8 = n56.Result;
    n57 = fs(a_8, y4_8);
    r.y = n57.Result;
    n58 = fa(a.y, a.y);
    a_10 = n58.Result;
    n59 = fm(a_10, a.z);
    r.z = n59.Result;
    return {r};
  }

  struct add_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}}; };
  add_ptResult add_pt(const nostr_Pt& a, const nostr_Pt& b) {
    nostr_U256 z1z1 = nostr_U256{List<Int>{}};
    nostr_U256 z2z2 = nostr_U256{List<Int>{}};
    nostr_U256 u1 = nostr_U256{List<Int>{}};
    nostr_U256 u2 = nostr_U256{List<Int>{}};
    nostr_U256 s1 = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    nostr_U256 s2 = nostr_U256{List<Int>{}};
    nostr_U256 a_3 = nostr_U256{List<Int>{}};
    nostr_Pt chosen = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 h = nostr_U256{List<Int>{}};
    nostr_U256 r = nostr_U256{List<Int>{}};
    nostr_U256 h2 = nostr_U256{List<Int>{}};
    nostr_U256 h3 = nostr_U256{List<Int>{}};
    nostr_U256 u1h2 = nostr_U256{List<Int>{}};
    nostr_Pt o = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a_4 = nostr_U256{List<Int>{}};
    nostr_U256 a_5 = nostr_U256{List<Int>{}};
    nostr_U256 a_6 = nostr_U256{List<Int>{}};
    nostr_U256 a_7 = nostr_U256{List<Int>{}};
    nostr_U256 a_8 = nostr_U256{List<Int>{}};
    nostr_U256 a_9 = nostr_U256{List<Int>{}};
    nostr_U256 a_10 = nostr_U256{List<Int>{}};
    if (is_zero(a.z).Result) {
      return {b};
    }
    if (is_zero(b.z).Result) {
      return {a};
    }
    n60 = fm(a.z, a.z);
    z1z1 = n60.Result;
    n61 = fm(b.z, b.z);
    z2z2 = n61.Result;
    n62 = fm(a.x, z2z2);
    u1 = n62.Result;
    n63 = fm(b.x, z1z1);
    u2 = n63.Result;
    n64 = fm(b.z, z2z2);
    a_2 = n64.Result;
    n65 = fm(a.y, a_2);
    s1 = n65.Result;
    n66 = fm(a.z, z1z1);
    a_3 = n66.Result;
    n67 = fm(b.y, a_3);
    s2 = n67.Result;
    if (math_compare<"==">(kern::to<Float>(cmp(u1, u2).Result), 0.0)) {
      if (math_compare<"==">(kern::to<Float>(cmp(s1, s2).Result), 0.0)) {
        n69 = dbl(a);
        chosen = n69.Result;
      } else {
        chosen = struct_make<nostr_Pt>(struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}));
      }
      return {chosen};
    }
    n70 = fs(u2, u1);
    h = n70.Result;
    n71 = fs(s2, s1);
    r = n71.Result;
    n72 = fm(h, h);
    h2 = n72.Result;
    n73 = fm(h2, h);
    h3 = n73.Result;
    n74 = fm(u1, h2);
    u1h2 = n74.Result;
    o = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    n75 = fm(r, r);
    a_5 = n75.Result;
    n76 = fs(a_5, h3);
    a_4 = n76.Result;
    n77 = fa(u1h2, u1h2);
    a_6 = n77.Result;
    n78 = fs(a_4, a_6);
    o.x = n78.Result;
    n79 = fs(u1h2, o.x);
    a_8 = n79.Result;
    n80 = fm(r, a_8);
    a_7 = n80.Result;
    n81 = fm(s1, h3);
    a_9 = n81.Result;
    n82 = fs(a_7, a_9);
    o.y = n82.Result;
    n83 = fm(a.z, b.z);
    a_10 = n83.Result;
    n84 = fm(h, a_10);
    o.z = n84.Result;
    return {o};
  }

  struct mul_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}}; };
  mul_ptResult mul_pt(const nostr_Pt& p, const nostr_U256& k) {
    nostr_Pt r = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    Int i = 0;
    r = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n85 = dbl(r);
      r = n85.Result;
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return (kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0); }, kern::to<Float>(k.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n86 = add_pt(r, p);
        r = n86.Result;
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {r};
  }

  struct affineResult { Bool Result = false; };
  affineResult affine(const nostr_Pt& p, nostr_U256& x, nostr_U256& y) {
    nostr_U256 e = nostr_U256{List<Int>{}};
    nostr_U256 a = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    nostr_U256 zi = nostr_U256{List<Int>{}};
    nostr_U256 a_3 = nostr_U256{List<Int>{}};
    nostr_U256 zi2 = nostr_U256{List<Int>{}};
    nostr_U256 a_4 = nostr_U256{List<Int>{}};
    if (is_zero(p.z).Result) {
      return {false};
    }
    e = nostr_U256{List<Int>{}};
    a = kP;
    a_2 = struct_make<nostr_U256>(list_make<Int, "8">(2, 0, 0, 0, 0, 0, 0, 0));
    n87 = sub(e, a, a_2);
    a_3 = kP;
    n88 = powm(p.z, e, a_3);
    zi = n88.Result;
    n89 = fm(zi, zi);
    zi2 = n89.Result;
    n90 = fm(p.x, zi2);
    x = n90.Result;
    n91 = fm(zi2, zi);
    a_4 = n91.Result;
    n92 = fm(p.y, a_4);
    y = n92.Result;
    return {true};
  }

  struct lift_xResult { Bool Result = false; };
  lift_xResult lift_x(const nostr_U256& x, nostr_Pt& out) {
    nostr_U256 a = nostr_U256{List<Int>{}};
    nostr_U256 seven = nostr_U256{List<Int>{}};
    nostr_U256 c = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    nostr_U256 a_3 = nostr_U256{List<Int>{}};
    nostr_U256 e = nostr_U256{List<Int>{}};
    nostr_U256 a_4 = nostr_U256{List<Int>{}};
    nostr_U256 a_5 = nostr_U256{List<Int>{}};
    nostr_U256 y = nostr_U256{List<Int>{}};
    nostr_U256 a_6 = nostr_U256{List<Int>{}};
    nostr_U256 a_7 = nostr_U256{List<Int>{}};
    nostr_U256 a_8 = nostr_U256{List<Int>{}};
    a = kP;
    if (math_compare<">=">(kern::to<Float>(cmp(x, a).Result), 0.0)) {
      return {false};
    }
    seven = nostr_U256{List<Int>{}};
    seven.w[0] = 7;
    n93 = fm(x, x);
    a_3 = n93.Result;
    n94 = fm(a_3, x);
    a_2 = n94.Result;
    n95 = fa(a_2, seven);
    c = n95.Result;
    e = nostr_U256{List<Int>{}};
    a_4 = kP;
    a_5 = struct_make<nostr_U256>(list_make<Int, "8">(1, 0, 0, 0, 0, 0, 0, 0));
    n96 = add(e, a_4, a_5);
    for (auto it97 = flow_for(n97, 0, 7); it97.next();) {
      e.w[n97.index] = kern::to<Int>(kern::formula([](Float a, Float index, Float a_2) -> Float { return kern::math::bor(kern::math::shr(a, 2.0), ((index < 7.0) ? kern::math::wrap32(kern::math::shl(a_2, 30.0)) : 0.0)); }, kern::to<Float>(e.w[n97.index]), kern::to<Float>(n97.index), kern::to<Float>(e.w[kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n97.index)))])));
    }
    a_6 = kP;
    n98 = powm(c, e, a_6);
    y = n98.Result;
    n99 = fm(y, y);
    a_7 = n99.Result;
    if (math_compare<"!=">(kern::to<Float>(cmp(a_7, c).Result), 0.0)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(y.w[0])), 0.5)) {
      a_8 = struct_make<nostr_U256>(List<Int>{});
      n100 = fs(a_8, y);
      y = n100.Result;
    }
    out.x = x;
    out.y = y;
    out.z = struct_make<nostr_U256>(list_make<Int, "8">(1, 0, 0, 0, 0, 0, 0, 0));
    return {true};
  }

  struct taggedResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  taggedResult tagged(const List<Int>& msg, Text tag = "") {
    Fixed<Int, 32> t = Fixed<Int, 32>{};
    List<Int> buf = List<Int>{};
    Int place = 0;
    Int place_2 = 0;
    n101 = sha256_2(tag);
    t = n101.Result;
    buf = t;
    place = list_length<Int>(buf);
    list_insert_list(buf, place, t);
    place_2 = list_length<Int>(buf);
    list_insert_list(buf, place_2, msg);
    n102 = sha256(buf);
    return {n102.Result};
  }

  void append(List<Int>& v, const Fixed<Int, 32>& b) {
    Int place = 0;
    place = list_length<Int>(v);
    list_insert_list(v, place, b);
  }

  struct public_keyResult { Bool Result = false; };
  public_keyResult public_key(const Fixed<Int, 32>& secret, Fixed<Int, 32>& out) {
    nostr_U256 d = nostr_U256{List<Int>{}};
    nostr_U256 a = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    Bool either = false;
    nostr_U256 x = nostr_U256{List<Int>{}};
    nostr_U256 y = nostr_U256{List<Int>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    d = from_be(secret).Result;
    either = is_zero(d).Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    x = nostr_U256{List<Int>{}};
    y = nostr_U256{List<Int>{}};
    a_4 = kG;
    n103 = mul_pt(a_4, d);
    a_3 = n103.Result;
    n104 = affine(a_3, x, y);
    if (logic_not(n104.Result)) {
      return {false};
    }
    out = to_be(x).Result;
    return {true};
  }

  struct verifyResult { Bool Result = false; };
  verifyResult verify(const Fixed<Int, 32>& pubkey, const Fixed<Int, 32>& message, const Fixed<Int, 64>& sig) {
    nostr_Pt p = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a = nostr_U256{List<Int>{}};
    Fixed<Int, 32> rb = Fixed<Int, 32>{};
    nostr_U256 r = nostr_U256{List<Int>{}};
    nostr_U256 s = nostr_U256{List<Int>{}};
    Fixed<Int, 32> a_2 = Fixed<Int, 32>{};
    nostr_U256 a_3 = nostr_U256{List<Int>{}};
    nostr_U256 a_4 = nostr_U256{List<Int>{}};
    nostr_U256 a_5 = nostr_U256{List<Int>{}};
    Bool either = false;
    List<Int> c = List<Int>{};
    nostr_U256 e = nostr_U256{List<Int>{}};
    nostr_U256 a_6 = nostr_U256{List<Int>{}};
    Fixed<Int, 32> a_7 = Fixed<Int, 32>{};
    nostr_U256 a_8 = nostr_U256{List<Int>{}};
    nostr_Pt ep = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a_9 = nostr_U256{List<Int>{}};
    nostr_U256 x = nostr_U256{List<Int>{}};
    nostr_U256 y = nostr_U256{List<Int>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_Pt a_12 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    p = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    a = from_be(pubkey).Result;
    n120 = lift_x(a, p);
    if (logic_not(n120.Result)) {
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
    c = List<Int>{};
    append(c, rb);
    append(c, pubkey);
    append(c, message);
    n121 = tagged(c, "BIP0340/challenge");
    a_7 = n121.Result;
    a_6 = from_be(a_7).Result;
    a_8 = kN;
    n122 = reduce(a_6, a_8);
    e = n122.Result;
    n123 = mul_pt(p, e);
    ep = n123.Result;
    a_9 = struct_make<nostr_U256>(List<Int>{});
    n124 = fs(a_9, ep.y);
    ep.y = n124.Result;
    x = nostr_U256{List<Int>{}};
    y = nostr_U256{List<Int>{}};
    a_12 = kG;
    n125 = mul_pt(a_12, s);
    a_11 = n125.Result;
    n126 = add_pt(a_11, ep);
    a_10 = n126.Result;
    n127 = affine(a_10, x, y);
    if (logic_not(n127.Result)) {
      return {false};
    }
    return {logic_and(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0); }, kern::to<Float>(y.w[0]))))), 0.0), math_compare<"==">(kern::to<Float>(cmp(x, r).Result), 0.0))};
  }

  struct the_signResult { Bool Result = false; };
  the_signResult the_sign(const Fixed<Int, 32>& secret, const Fixed<Int, 32>& message, const Fixed<Int, 32>& aux, Fixed<Int, 64>& sig) {
    nostr_U256 d = nostr_U256{List<Int>{}};
    nostr_U256 a = nostr_U256{List<Int>{}};
    nostr_U256 a_2 = nostr_U256{List<Int>{}};
    Bool either = false;
    nostr_U256 px = nostr_U256{List<Int>{}};
    nostr_U256 py = nostr_U256{List<Int>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a_5 = nostr_U256{List<Int>{}};
    nostr_U256 a_6 = nostr_U256{List<Int>{}};
    Fixed<Int, 32> pxb = Fixed<Int, 32>{};
    Fixed<Int, 32> db = Fixed<Int, 32>{};
    Fixed<Int, 32> ah = Fixed<Int, 32>{};
    List<Int> a_7 = List<Int>{};
    List<Int> m = List<Int>{};
    Fixed<Int, 32> rand = Fixed<Int, 32>{};
    nostr_U256 k = nostr_U256{List<Int>{}};
    nostr_U256 a_8 = nostr_U256{List<Int>{}};
    nostr_U256 a_9 = nostr_U256{List<Int>{}};
    nostr_U256 rx = nostr_U256{List<Int>{}};
    nostr_U256 ry = nostr_U256{List<Int>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}, nostr_U256{List<Int>{}}};
    nostr_U256 a_12 = nostr_U256{List<Int>{}};
    nostr_U256 a_13 = nostr_U256{List<Int>{}};
    Fixed<Int, 32> rxb = Fixed<Int, 32>{};
    List<Int> c = List<Int>{};
    nostr_U256 e = nostr_U256{List<Int>{}};
    nostr_U256 a_14 = nostr_U256{List<Int>{}};
    Fixed<Int, 32> a_15 = Fixed<Int, 32>{};
    nostr_U256 a_16 = nostr_U256{List<Int>{}};
    nostr_U256 s = nostr_U256{List<Int>{}};
    nostr_U256 a_17 = nostr_U256{List<Int>{}};
    nostr_U256 a_18 = nostr_U256{List<Int>{}};
    nostr_U256 a_19 = nostr_U256{List<Int>{}};
    Fixed<Int, 32> sb = Fixed<Int, 32>{};
    d = from_be(secret).Result;
    either = is_zero(d).Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    px = nostr_U256{List<Int>{}};
    py = nostr_U256{List<Int>{}};
    a_4 = kG;
    n105 = mul_pt(a_4, d);
    a_3 = n105.Result;
    n106 = affine(a_3, px, py);
    if (logic_not(n106.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(py.w[0])), 0.5)) {
      a_5 = struct_make<nostr_U256>(List<Int>{});
      a_6 = kN;
      n107 = subm(a_5, d, a_6);
      d = n107.Result;
    }
    pxb = to_be(px).Result;
    db = to_be(d).Result;
    a_7 = aux;
    n108 = tagged(a_7, "BIP0340/aux");
    ah = n108.Result;
    m = List<Int>{};
    for (auto it109 = flow_for(n109, 0, 31); it109.next();) {
      list_add(m, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::bxor(a, a_2), 255.0); }, kern::to<Float>(db[n109.index]), kern::to<Float>(ah[n109.index]))));
    }
    append(m, pxb);
    append(m, message);
    n110 = tagged(m, "BIP0340/nonce");
    rand = n110.Result;
    a_8 = from_be(rand).Result;
    a_9 = kN;
    n111 = reduce(a_8, a_9);
    k = n111.Result;
    if (is_zero(k).Result) {
      return {false};
    }
    rx = nostr_U256{List<Int>{}};
    ry = nostr_U256{List<Int>{}};
    a_11 = kG;
    n112 = mul_pt(a_11, k);
    a_10 = n112.Result;
    n113 = affine(a_10, rx, ry);
    if (logic_not(n113.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(ry.w[0])), 0.5)) {
      a_12 = struct_make<nostr_U256>(List<Int>{});
      a_13 = kN;
      n114 = subm(a_12, k, a_13);
      k = n114.Result;
    }
    rxb = to_be(rx).Result;
    c = List<Int>{};
    append(c, rxb);
    append(c, pxb);
    append(c, message);
    n115 = tagged(c, "BIP0340/challenge");
    a_15 = n115.Result;
    a_14 = from_be(a_15).Result;
    a_16 = kN;
    n116 = reduce(a_14, a_16);
    e = n116.Result;
    a_18 = kN;
    n117 = mulm(e, d, a_18);
    a_17 = n117.Result;
    a_19 = kN;
    n118 = addm(k, a_17, a_19);
    s = n118.Result;
    sb = to_be(s).Result;
    for (auto it119 = flow_for(n119, 0, 31); it119.next();) {
      sig[n119.index] = rxb[n119.index];
      sig[kern::to<Int>(kern::formula([](Float index) -> Float { return (32.0 + index); }, kern::to<Float>(n119.index)))] = sb[n119.index];
    }
    n128 = verify(pxb, message, sig);
    return {n128.Result};
  }

  void hex_of(Text& o, Int byte = 0) {
    o = text_append(o, text_of_code(list_get<Int>(kHexDigits, kern::to<Int>(kern::formula([](Float byte) -> Float { return kern::math::shr(byte, 4.0); }, kern::to<Float>(byte))))), "");
    o = text_append(o, text_of_code(list_get<Int>(kHexDigits, kern::to<Int>(kern::formula([](Float byte) -> Float { return kern::math::band(byte, 15.0); }, kern::to<Float>(byte))))), "");
  }

  struct hexResult { Text Result = ""; };
  hexResult hex(const Fixed<Int, 32>& data) {
    Text o = "";
    Int byte = 0;
    o = "";
    for (auto it129 = flow_for(n129, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it129.next();) {
      byte = data[n129.index];
      hex_of(o, byte);
    }
    return {o};
  }

  struct hex_2Result { Text Result = ""; };
  hex_2Result hex_2(const Fixed<Int, 64>& data) {
    Text o = "";
    Int byte = 0;
    o = "";
    for (auto it130 = flow_for(n130, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it130.next();) {
      byte = data[n130.index];
      hex_of(o, byte);
    }
    return {o};
  }

  struct escapeResult { Text Result = ""; };
  KERN_PURE escapeResult escape(Text s = "") {
    Text o = "";
    Int c = 0;
    Int selected = 0;
    Text b = "";
    o = "";
    for (auto it131 = flow_for(n131, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it131.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n131.index))));
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
  event_jsonResult event_json(const Fixed<Int, 32>& secret, const Fixed<Int, 32>& aux, Text& out, Int64 created_at = Int64{0x0}, Int kind = 0, Text tags = "", Text content = "") {
    Fixed<Int, 32> pk = Fixed<Int, 32>{};
    Text pkh = "";
    Text body = "";
    Text head = "";
    Text digits = "";
    Text ser = "";
    Fixed<Int, 32> id = Fixed<Int, 32>{};
    Fixed<Int, 64> sig = Fixed<Int, 64>{};
    Text num = "";
    Text digits_2 = "";
    pk = List<Int>{};
    n132 = public_key(secret, pk);
    if (logic_not(n132.Result)) {
      return {false};
    }
    n133 = hex(pk);
    pkh = n133.Result;
    body = escape(content).Result;
    head = "";
    digits = int64_text_signed(created_at);
    head = text_format<"[0,\"%s\",%s,%u,">(95, pkh, digits, kind);
    ser = text_append(text_append(text_append(text_append(head, tags, ""), ",\"", ""), body, ""), "\"]", "");
    n134 = sha256_2(ser);
    id = n134.Result;
    sig = List<Int>{};
    n135 = the_sign(secret, id, aux, sig);
    if (logic_not(n135.Result)) {
      return {false};
    }
    num = "";
    digits_2 = int64_text_signed(created_at);
    num = text_format<",\"created_at\":%s,\"kind\":%u,\"tags\":">(63, digits_2, kind);
    n136 = hex(id);
    n137 = hex_2(sig);
    out = text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append("{\"id\":\"", n136.Result, ""), "\",\"pubkey\":\"", ""), pkh, ""), "\"", ""), num, ""), tags, ""), ",\"content\":\"", ""), body, ""), "\",\"sig\":\"", ""), n137.Result, ""), "\"}", "");
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
  fmResult n40;
  faResult n41;
  faResult n42;
  faResult n43;
  fmResult n44;
  fmResult n45;
  faResult n46;
  faResult n47;
  fmResult n48;
  faResult n49;
  fsResult n50;
  fmResult n51;
  faResult n52;
  faResult n53;
  faResult n54;
  fsResult n55;
  fmResult n56;
  fsResult n57;
  faResult n58;
  fmResult n59;
  fmResult n60;
  fmResult n61;
  fmResult n62;
  fmResult n63;
  fmResult n64;
  fmResult n65;
  fmResult n66;
  fmResult n67;
  dblResult n69;
  fsResult n70;
  fsResult n71;
  fmResult n72;
  fmResult n73;
  fmResult n74;
  fmResult n75;
  fsResult n76;
  faResult n77;
  fsResult n78;
  fsResult n79;
  fmResult n80;
  fmResult n81;
  fsResult n82;
  fmResult n83;
  fmResult n84;
  dblResult n85;
  add_ptResult n86;
  subResult n87;
  powmResult n88;
  fmResult n89;
  fmResult n90;
  fmResult n91;
  fmResult n92;
  fmResult n93;
  fmResult n94;
  faResult n95;
  addResult n96;
  struct : kern::Node<97> { Int index = 0; } n97;
  powmResult n98;
  fmResult n99;
  fsResult n100;
  sha256_2Result n101;
  sha256Result n102;
  mul_ptResult n103;
  affineResult n104;
  mul_ptResult n105;
  affineResult n106;
  submResult n107;
  taggedResult n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  taggedResult n110;
  reduceResult n111;
  mul_ptResult n112;
  affineResult n113;
  submResult n114;
  taggedResult n115;
  reduceResult n116;
  mulmResult n117;
  addmResult n118;
  struct : kern::Node<119> { Int index = 0; } n119;
  lift_xResult n120;
  taggedResult n121;
  reduceResult n122;
  mul_ptResult n123;
  fsResult n124;
  mul_ptResult n125;
  add_ptResult n126;
  affineResult n127;
  verifyResult n128;
  struct : kern::Node<129> { Int index = 0; } n129;
  struct : kern::Node<130> { Int index = 0; } n130;
  struct : kern::Node<131> { Int index = 0; } n131;
  public_keyResult n132;
  hexResult n133;
  sha256_2Result n134;
  the_signResult n135;
  hexResult n136;
  hex_2Result n137;
};
