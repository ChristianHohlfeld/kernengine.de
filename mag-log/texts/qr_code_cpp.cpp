struct qr_code_cpp : virtual kern::Library, virtual qr_code, virtual shapes {
  // ---- types ----
  KERN_STRUCT(qr_Grid) { I32 size = 0; List<U8> m = List<U8>{}; List<U8> fn = List<U8>{}; };

  // ---- variables ----
  /// Level L per version (ISO/IEC 18004 table 9): EC codewords per block, number of blocks.
  KERN_CONST Fixed<I32, 41> kEccPerBlock = List<I32>{-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
  KERN_CONST Fixed<I32, 41> kBlocks = List<I32>{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25};
  /// format bits of level L
  KERN_CONST U32 kLevelL = 1;

  // ---- functions ----
  struct raw_modulesResult { I32 Result = 0; };
  KERN_PURE raw_modulesResult raw_modules(I32 v = 0) {
    I32 r = 0;
    I32 n = 0;
    r = kern::to<Int>(kern::formula([](Float v) -> Float { return ((((16.0 * v) + 128.0) * v) + 64.0); }, kern::to<Float>(v)));
    if (math_compare<">=">(kern::to<Float>(v), 2.0)) {
      n = kern::to<Int>(kern::formula([](Float v) -> Float { return (kern::math::trunc(kern::math::div(v, 7.0)) + 2.0); }, kern::to<Float>(v)));
      r = kern::to<Int>(kern::formula([](Float r, Float n) -> Float { return (r - ((((25.0 * n) - 10.0) * n) - 55.0)); }, kern::to<Float>(r), kern::to<Float>(n)));
      if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
        r = kern::to<Int>(kern::formula([](Float r) -> Float { return (r - 36.0); }, kern::to<Float>(r)));
      }
    }
    return {r};
  }

  struct data_codewordsResult { I32 Result = 0; };
  KERN_PURE data_codewordsResult data_codewords(I32 v = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::trunc(kern::math::div(a, 8.0)) - (a_2 * a_3)); }, kern::to<Float>(raw_modules(v).Result), kern::to<Float>(list_get<I32>(kEccPerBlock, v)), kern::to<Float>(list_get<I32>(kBlocks, v))))};
  }

  struct gf_mulResult { U8 Result = 0; };
  KERN_PURE gf_mulResult gf_mul(U8 x = 0, U8 y = 0) {
    U32 z = 0;
    I32 i = 0;
    z = 0;
    i = 7;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      z = kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(z, 1.0)), kern::math::mul32(kern::math::shr(z, 7.0), 285.0)); }, kern::to<Float>(z)));
      z = kern::to<Int>(kern::formula([](Float z, Float y, Float i, Float x) -> Float { return kern::math::bxor(z, kern::math::mul32(kern::math::band(kern::math::wrap32(kern::math::shr(y, i)), 1.0), x)); }, kern::to<Float>(z), kern::to<Float>(y), kern::to<Float>(i), kern::to<Float>(x)));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
    return {kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::band(z, 255.0); }, kern::to<Float>(z)))};
  }

  struct rs_divisorResult { List<U8> Result = List<U8>{}; };
  KERN_PURE rs_divisorResult rs_divisor(I32 degree = 0) {
    List<U8> r = List<U8>{};
    List<U8> made = List<U8>{};
    U8 root = 0;
    Int j = 0;
    made = List<U8>{};
    list_resize(made, degree, 0);
    r = made;
    r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(r))))] = 1;
    root = 1;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(degree)))); it10.next();) {
      j = 0;
      while (loop(math_compare<">">(kern::formula([](Float j, Float a) -> Float { return ((j < 0.0) != (a < 0.0) ? (a < 0.0) : (j < a)); }, kern::to<Float>(j), kern::to<Float>(list_length<U8>(r))), 0.5))) {
        r[j] = gf_mul(r[j], root).Result;
        if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float j) -> Float { return (j + 1.0); }, kern::to<Float>(j))))), kern::to<Float>(list_length<U8>(r))), 0.5)) {
          r[j] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[j]), kern::to<Float>(r[kern::to<Int>(kern::formula([](Float j) -> Float { return (j + 1.0); }, kern::to<Float>(j)))])));
        }
        j += 1;
      }
      root = gf_mul(root, 2).Result;
    }
    return {r};
  }

  struct rs_remainderResult { List<U8> Result = List<U8>{}; };
  KERN_PURE rs_remainderResult rs_remainder(const List<U8>& data, const List<U8>& the_div) {
    List<U8> r = List<U8>{};
    List<U8> made = List<U8>{};
    U8 b = 0;
    U8 f = 0;
    Int i = 0;
    made = List<U8>{};
    list_resize(made, list_length<U8>(the_div), 0);
    r = made;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(data))))); it11.next();) {
      b = data[n11.index];
      f = kern::to<Int>(kern::formula([](Float b, Float a) -> Float { return kern::math::bxor(b, a); }, kern::to<Float>(b), kern::to<Float>(r[0])));
      list_remove_at(r, 0);
      list_add(r, 0);
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<U8>(r))), 0.5))) {
        r[i] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[i]), kern::to<Float>(gf_mul(the_div[i], f).Result)));
        i += 1;
      }
    }
    return {r};
  }

  struct alignment_positionsResult { List<I32> Result = List<I32>{}; };
  KERN_PURE alignment_positionsResult alignment_positions(I32 v = 0) {
    I32 n = 0;
    I32 size = 0;
    I32 step = 0;
    List<I32> r = List<I32>{};
    List<I32> made = List<I32>{};
    I32 i = 0;
    I32 pos = 0;
    if (math_compare<"==">(kern::to<Float>(v), 1.0)) {
      return {List<I32>{}};
    }
    n = kern::to<Int>(kern::formula([](Float v) -> Float { return (kern::math::trunc(kern::math::div(v, 7.0)) + 2.0); }, kern::to<Float>(v)));
    size = kern::to<Int>(kern::formula([](Float v) -> Float { return ((v * 4.0) + 17.0); }, kern::to<Float>(v)));
    step = kern::to<Int>(kern::formula([](Float v, Float n) -> Float { return ((v == 32.0) ? 26.0 : (kern::math::trunc(kern::math::div((((v * 4.0) + (n * 2.0)) + 1.0), ((n * 2.0) - 2.0))) * 2.0)); }, kern::to<Float>(v), kern::to<Float>(n)));
    made = List<I32>{};
    list_resize(made, n, 0);
    r = made;
    r[0] = 6;
    i = kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n)));
    pos = kern::to<Int>(kern::formula([](Float size) -> Float { return (size - 7.0); }, kern::to<Float>(size)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 1.0))) {
      r[i] = pos;
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
      pos = kern::to<Int>(kern::formula([](Float pos, Float step) -> Float { return (pos - step); }, kern::to<Float>(pos), kern::to<Float>(step)));
    }
    return {r};
  }

  void Grid_set(qr_Grid& self, I32 x = 0, I32 y = 0, Bool dark = false) {
    self.m[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return ((y * size) + x); }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = kern::to<Int>(dark);
    self.fn[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return ((y * size) + x); }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = 1;
  }

  void finder(qr_Grid& g, I32 cx = 0, I32 cy = 0) {
    Bool skip = false;
    I32 x = 0;
    I32 y = 0;
    I32 d = 0;
    for (auto it12 = flow_for(n12, -4, 4); it12.next();) {
      for (auto it13 = flow_for(n13, -4, 4); it13.next();) {
        skip = false;
        x = kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return (cx + index); }, kern::to<Float>(cx), kern::to<Float>(n13.index)));
        y = kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return (cy + index); }, kern::to<Float>(cy), kern::to<Float>(n12.index)));
        if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(x), 0.0), math_compare<"<">(kern::to<Float>(y), 0.0)), math_compare<">=">(kern::to<Float>(x), kern::to<Float>(g.size))), math_compare<">=">(kern::to<Float>(y), kern::to<Float>(g.size)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          d = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n13.index), kern::to<Float>(n12.index)));
          Grid_set(g, x, y, logic_and(math_compare<"!=">(kern::to<Float>(d), 2.0), math_compare<"!=">(kern::to<Float>(d), 4.0)));
        }
      }
    }
  }

  struct format_bits_bitResult { Bool Result = false; };
  KERN_PURE format_bits_bitResult format_bits_bit(U32 bits = 0, I32 i = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits, Float i) -> Float { return kern::math::band(kern::math::shr(bits, i), 1.0); }, kern::to<Float>(bits), kern::to<Float>(i))))), 0.0)};
  }

  void format_bits(qr_Grid& g, U32 mask = 0) {
    U32 data = 0;
    U32 rem = 0;
    U32 bits = 0;
    data = kern::to<Int>(kern::formula([](Float kLevelL, Float mask) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kLevelL, 3.0)), mask); }, kern::to<Float>(kLevelL), kern::to<Float>(mask)));
    rem = data;
    for (auto it14 = flow_for(n14, 0, 9); it14.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    bits = kern::to<Int>(kern::formula([](Float data, Float rem) -> Float { return kern::math::bxor(kern::math::bor(kern::math::wrap32(kern::math::shl(data, 10.0)), rem), 21522.0); }, kern::to<Float>(data), kern::to<Float>(rem)));
    for (auto it15 = flow_for(n15, 0, 5); it15.next();) {
      Grid_set(g, 8, n15.index, format_bits_bit(bits, n15.index).Result);
    }
    Grid_set(g, 8, 7, format_bits_bit(bits, 6).Result);
    Grid_set(g, 8, 8, format_bits_bit(bits, 7).Result);
    Grid_set(g, 7, 8, format_bits_bit(bits, 8).Result);
    for (auto it16 = flow_for(n16, 9, 14); it16.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float index) -> Float { return (14.0 - index); }, kern::to<Float>(n16.index))), 8, format_bits_bit(bits, n16.index).Result);
    }
    for (auto it17 = flow_for(n17, 0, 7); it17.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return ((size - 1.0) - index); }, kern::to<Float>(g.size), kern::to<Float>(n17.index))), 8, format_bits_bit(bits, n17.index).Result);
    }
    for (auto it18 = flow_for(n18, 8, 14); it18.next();) {
      Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return ((size - 15.0) + index); }, kern::to<Float>(g.size), kern::to<Float>(n18.index))), format_bits_bit(bits, n18.index).Result);
    }
    Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size) -> Float { return (size - 8.0); }, kern::to<Float>(g.size))), true);
  }

  void encode_put(List<Bool>& bits, U32 val = 0, I32 len = 0) {
    I32 i = 0;
    i = kern::to<Int>(kern::formula([](Float len) -> Float { return (len - 1.0); }, kern::to<Float>(len)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      list_add(bits, math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float val, Float i) -> Float { return kern::math::band(kern::math::shr(val, i), 1.0); }, kern::to<Float>(val), kern::to<Float>(i))))), 0.0));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return (i - 1.0); }, kern::to<Float>(i)));
    }
  }

  struct encodeResult { Bool Result = false; };
  encodeResult encode(const Text& text, qr_Code& out) {
    I32 n = 0;
    I32 v = 0;
    Bool stop = false;
    I32 count_bits = 0;
    List<Bool> bits = List<Bool>{};
    Int c = 0;
    Int cap = 0;
    U8 pad = 0;
    List<U8> data = List<U8>{};
    List<U8> made = List<U8>{};
    I32 blocks = 0;
    I32 ecc = 0;
    I32 raw = 0;
    I32 short_blocks = 0;
    I32 short_len = 0;
    List<U8> the_div = List<U8>{};
    List<U8_row> bl = List<U8_row>{};
    I32 i = 0;
    I32 k = 0;
    I32 len = 0;
    List<U8> d = List<U8>{};
    List<U8> made_2 = List<U8>{};
    Int first = 0;
    Int last = 0;
    List<U8> e = List<U8>{};
    Int place = 0;
    List<U8> words = List<U8>{};
    qr_Grid g = qr_Grid{0, List<U8>{}, List<U8>{}};
    Int count = 0;
    U8 fill = 0;
    Int count_2 = 0;
    U8 fill_2 = 0;
    I32 i_2 = 0;
    List<I32> al = List<I32>{};
    Bool skip = false;
    U32 rem = 0;
    U32 vb = 0;
    Bool b = false;
    I32 a = 0;
    I32 c_2 = 0;
    Int i_3 = 0;
    I32 right = 0;
    I32 vert = 0;
    Bool skip_2 = false;
    I32 x = 0;
    Bool up = false;
    I32 y = 0;
    Int at = 0;
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(text_length(text))));
    v = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<=">(kern::to<Float>(v), 40.0)))) {
      count_bits = kern::to<Int>(kern::formula([](Float v) -> Float { return ((v <= 9.0) ? 8.0 : 16.0); }, kern::to<Float>(v)));
      if (logic_and(math_compare<"<=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float count_bits, Float n) -> Float { return ((4.0 + count_bits) + (8.0 * n)); }, kern::to<Float>(count_bits), kern::to<Float>(n))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 8.0); }, kern::to<Float>(data_codewords(v).Result)))))), logic_or(math_compare<"==">(kern::to<Float>(count_bits), 16.0), math_compare<"<">(kern::to<Float>(n), 256.0)))) {
        stop = true;
      }
      if (logic_not(stop)) {
        v += 1;
      }
    }
    if (math_compare<">">(kern::to<Float>(v), 40.0)) {
      return {false};
    }
    bits = List<Bool>{};
    encode_put(bits, 4, 4);
    encode_put(bits, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32(n); }, kern::to<Float>(n))), kern::to<Int>(kern::formula([](Float v) -> Float { return ((v <= 9.0) ? 8.0 : 16.0); }, kern::to<Float>(v))));
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it19.next();) {
      c = text_code_at(text, n19.index);
      encode_put(bits, c, 8);
    }
    cap = kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 8.0); }, kern::to<Float>(data_codewords(v).Result)));
    encode_put(bits, 0, kern::to<Int>(kern::formula([](Float cap, Float a) -> Float { return kern::math::wrap32s(kern::math::min(4.0, (cap - a))); }, kern::to<Float>(cap), kern::to<Float>(list_length<Bool>(bits)))));
    while (loop(math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::mod(a, 8.0) != 0.0); }, kern::to<Float>(list_length<Bool>(bits))), 0.5))) {
      list_add(bits, false);
    }
    pad = 236;
    while (loop(math_compare<">">(kern::formula([](Float a, Float cap) -> Float { return ((a < 0.0) != (cap < 0.0) ? (cap < 0.0) : (a < cap)); }, kern::to<Float>(list_length<Bool>(bits)), kern::to<Float>(cap)), 0.5))) {
      encode_put(bits, pad, 8);
      pad = kern::to<Int>(kern::formula([](Float pad) -> Float { return kern::math::band(kern::math::bxor(pad, kern::math::bxor(236.0, 17.0)), 255.0); }, kern::to<Float>(pad)));
    }
    made = List<U8>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(list_length<Bool>(bits)))), 0);
    data = made;
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Bool>(bits))))); it20.next();) {
      if (bits[n20.index]) {
        data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::bor(a, kern::math::band(kern::math::wrap32s(kern::math::shl(1.0, (7.0 - kern::math::band(index, 7.0)))), 255.0)); }, kern::to<Float>(data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))]), kern::to<Float>(n20.index)));
      }
    }
    blocks = list_get<I32>(kBlocks, v);
    ecc = list_get<I32>(kEccPerBlock, v);
    raw = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(raw_modules(v).Result)));
    short_blocks = kern::to<Int>(kern::formula([](Float blocks, Float raw) -> Float { return (blocks - kern::math::mod(raw, blocks)); }, kern::to<Float>(blocks), kern::to<Float>(raw)));
    short_len = kern::to<Int>(kern::formula([](Float raw, Float blocks) -> Float { return kern::math::trunc(kern::math::div(raw, blocks)); }, kern::to<Float>(raw), kern::to<Float>(blocks)));
    the_div = rs_divisor(ecc).Result;
    bl = List<U8_row>{};
    i = 0;
    k = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(blocks)))) {
      len = kern::to<Int>(kern::formula([](Float short_len, Float ecc, Float i, Float short_blocks) -> Float { return ((short_len - ecc) + ((i < short_blocks) ? 0.0 : 1.0)); }, kern::to<Float>(short_len), kern::to<Float>(ecc), kern::to<Float>(i), kern::to<Float>(short_blocks)));
      made_2 = List<U8>{};
      first = k;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(k), kern::to<Float>(len)))))));
      list_reserve(made_2, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
      for (auto it21 = flow_for(n21, first, last); it21.next();) {
        list_add(made_2, list_get<U8>(data, n21.index));
      }
      d = made_2;
      k += len;
      e = rs_remainder(d, the_div).Result;
      if (math_compare<"<">(kern::to<Float>(i), kern::to<Float>(short_blocks))) {
        list_add(d, 0);
      }
      place = list_length<U8>(d);
      list_insert_list(d, place, e);
      list_add(bl, struct_make<U8_row>(d));
      i += 1;
    }
    words = List<U8>{};
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8>(bl[0].items))))); it22.next();) {
      for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8_row>(bl))))); it23.next();) {
        if (logic_or(math_compare<"!=">(kern::to<Float>(n22.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float short_len, Float ecc) -> Float { return (short_len - ecc); }, kern::to<Float>(short_len), kern::to<Float>(ecc)))))), math_compare<">">(kern::formula([](Float index, Float short_blocks) -> Float { return (!((index < 0.0) != (short_blocks < 0.0) ? (short_blocks < 0.0) : (index < short_blocks))); }, kern::to<Float>(n23.index), kern::to<Float>(short_blocks)), 0.5))) {
          list_add(words, bl[n23.index].items[n22.index]);
        }
      }
    }
    g = struct_make<qr_Grid>(kern::to<Int>(kern::formula([](Float v) -> Float { return ((v * 4.0) + 17.0); }, kern::to<Float>(v))), List<U8>{}, List<U8>{});
    count = kern::to<Int>(kern::formula([](Float size) -> Float { return (size * size); }, kern::to<Float>(g.size)));
    fill = 0;
    list_clear(g.m);
    list_resize(g.m, count, fill);
    count_2 = list_length<U8>(g.m);
    fill_2 = 0;
    list_clear(g.fn);
    list_resize(g.fn, count_2, fill_2);
    i_2 = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(g.size)))) {
      Grid_set(g, 6, i_2, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      Grid_set(g, i_2, 6, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      i_2 += 1;
    }
    finder(g, 3, 3);
    finder(g, kern::to<Int>(kern::formula([](Float size) -> Float { return (size - 4.0); }, kern::to<Float>(g.size))), 3);
    finder(g, 3, kern::to<Int>(kern::formula([](Float size) -> Float { return (size - 4.0); }, kern::to<Float>(g.size))));
    al = alignment_positions(v).Result;
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<I32>(al))))); it24.next();) {
      for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<I32>(al))))); it25.next();) {
        skip = false;
        if (logic_or(logic_or(logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), 0.0)), logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<I32>(al))))))))), logic_and(math_compare<"==">(kern::to<Float>(n24.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<I32>(al))))))), math_compare<"==">(kern::to<Float>(n25.index), 0.0)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it26 = flow_for(n26, -2, 2); it26.next();) {
            for (auto it27 = flow_for(n27, -2, 2); it27.next();) {
              Grid_set(g, kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return (a + index); }, kern::to<Float>(al[n24.index]), kern::to<Float>(n27.index))), kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return (a + index); }, kern::to<Float>(al[n25.index]), kern::to<Float>(n26.index))), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n27.index), kern::to<Float>(n26.index))))), 1.0));
            }
          }
        }
      }
    }
    format_bits(g, 0);
    if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
      rem = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(v); }, kern::to<Float>(v)));
      for (auto it28 = flow_for(n28, 0, 11); it28.next();) {
        rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 11.0), 7973.0)); }, kern::to<Float>(rem)));
      }
      vb = kern::to<Int>(kern::formula([](Float v, Float rem) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kern::math::wrap32(v), 12.0)), rem); }, kern::to<Float>(v), kern::to<Float>(rem)));
      for (auto it29 = flow_for(n29, 0, 17); it29.next();) {
        b = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float vb, Float index) -> Float { return kern::math::band(kern::math::shr(vb, index), 1.0); }, kern::to<Float>(vb), kern::to<Float>(n29.index))))), 0.0);
        a = kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return ((size - 11.0) + kern::math::mod(index, 3.0)); }, kern::to<Float>(g.size), kern::to<Float>(n29.index)));
        c_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::trunc(kern::math::div(index, 3.0)); }, kern::to<Float>(n29.index)));
        Grid_set(g, a, c_2, b);
        Grid_set(g, c_2, a, b);
      }
    }
    i_3 = 0;
    right = kern::to<Int>(kern::formula([](Float size) -> Float { return (size - 1.0); }, kern::to<Float>(g.size)));
    while (loop(math_compare<">=">(kern::to<Float>(right), 1.0))) {
      if (math_compare<"==">(kern::to<Float>(right), 6.0)) {
        right = 5;
      }
      vert = 0;
      while (loop(math_compare<"<">(kern::to<Float>(vert), kern::to<Float>(g.size)))) {
        for (auto it30 = flow_for(n30, 0, 1); it30.next();) {
          skip_2 = false;
          x = kern::to<Int>(kern::formula([](Float right, Float index) -> Float { return (right - index); }, kern::to<Float>(right), kern::to<Float>(n30.index)));
          up = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float right) -> Float { return kern::math::band((right + 1.0), 2.0); }, kern::to<Float>(right))))), 0.0);
          y = kern::to<Int>(kern::formula([](Float up, Float size, Float vert) -> Float { return ((up != 0.0) ? ((size - 1.0) - vert) : vert); }, kern::to<Float>(up), kern::to<Float>(g.size), kern::to<Float>(vert)));
          at = kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return ((y * size) + x); }, kern::to<Float>(y), kern::to<Float>(g.size), kern::to<Float>(x)));
          if (math_compare<"!=">(kern::to<Float>(g.fn[at]), 0.0)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            if (math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return ((i_3 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_3 < a)); }, kern::to<Float>(i_3), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 8.0); }, kern::to<Float>(list_length<U8>(words))))))), 0.5)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return kern::math::band(kern::math::shr(a, (7.0 - kern::math::band(i_3, 7.0))), 1.0); }, kern::to<Float>(words[kern::to<Int>(kern::formula([](Float i_3) -> Float { return kern::math::shr(i_3, 3.0); }, kern::to<Float>(i_3)))]), kern::to<Float>(i_3)));
              i_3 += 1;
            }
            if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x, Float y) -> Float { return kern::math::mod((x + y), 2.0); }, kern::to<Float>(x), kern::to<Float>(y))))), 0.0)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bxor(a, 1.0); }, kern::to<Float>(g.m[at])));
            }
          }
        }
        vert += 1;
      }
      right = kern::to<Int>(kern::formula([](Float right) -> Float { return (right - 2.0); }, kern::to<Float>(right)));
    }
    format_bits(g, 0);
    out.version = v;
    out.size = g.size;
    var_take(n31, g.m);
    out.modules = n31.value;
    return {true};
  }

  struct read_formatResult { Bool Result = false; };
  read_formatResult read_format(const qr_Code& c, U32& level_bits, U32& mask) {
    U32 bits = 0;
    U32 rem = 0;
    bits = 0;
    for (auto it32 = flow_for(n32, 0, 5); it32.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, n32.index).Result), kern::to<Float>(n32.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 6.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 7).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 7.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 8).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 8.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 7, 8).Result)));
    for (auto it33 = flow_for(n33, 9, 14); it33.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, kern::to<Int>(kern::formula([](Float index) -> Float { return (14.0 - index); }, kern::to<Float>(n33.index))), 8).Result), kern::to<Float>(n33.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::bxor(bits, 21522.0); }, kern::to<Float>(bits)));
    level_bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 13.0); }, kern::to<Float>(bits)));
    mask = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(kern::math::shr(bits, 10.0), 7.0); }, kern::to<Float>(bits)));
    rem = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 10.0); }, kern::to<Float>(bits)));
    for (auto it34 = flow_for(n34, 0, 9); it34.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    return {math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::band(rem, 1023.0); }, kern::to<Float>(rem))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(bits, 1023.0); }, kern::to<Float>(bits))))))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct { List<U8> value = List<U8>{}; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
};
