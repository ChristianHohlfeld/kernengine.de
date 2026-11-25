struct game_destruction_sync_cpp : virtual kern::Library, virtual city_stream, virtual terrain, virtual wire {
  // ---- functions ----
  void put_craters(List<U8>& o, const List<Terrain_CraterRec>& c) {
    Int i = 0;
    wire::put_u8(o, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(kern::math::min(a, 255.0), 255.0); }, kern::to<Float>(list_length<Terrain_CraterRec>(c)))));
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Terrain_CraterRec>(c))), 0.5), math_compare<">">(kern::formula([](Float i) -> Float { return ((i < 0.0) != (255.0 < 0.0) ? (255.0 < 0.0) : (i < 255.0)); }, kern::to<Float>(i)), 0.5)))) {
      wire::put_f32(o, c[i].x);
      wire::put_f32(o, c[i].y);
      wire::put_f32(o, c[i].r);
      wire::put_f32(o, c[i].depth);
      i += 1;
    }
  }

  struct get_cratersResult { Bool Result = false; };
  get_cratersResult get_craters(wire_Reader& in, const List<U8>& b, List<Terrain_CraterRec>& c) {
    U8 n = 0;
    U8 i = 0;
    Terrain_CraterRec r = Terrain_CraterRec{0.0, 0.0, 0.0, 0.0};
    n10 = wire::Reader_get_u8(in, b);
    n = n10.Result;
    list_clear(c);
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), in.ok))) {
      r = Terrain_CraterRec{0.0, 0.0, 0.0, 0.0};
      n11 = wire::Reader_get_f32(in, b);
      r.x = n11.Result;
      n12 = wire::Reader_get_f32(in, b);
      r.y = n12.Result;
      n13 = wire::Reader_get_f32(in, b);
      r.r = n13.Result;
      n14 = wire::Reader_get_f32(in, b);
      r.depth = n14.Result;
      list_add(c, r);
      i += 1;
    }
    return {in.ok};
  }

  void put_damage(List<U8>& o, const List<CityStream_DamageRec>& d, Int from = 0, Int n = 0) {
    wire::put_u16(o, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::band(n, 65535.0); }, kern::to<Float>(n))));
    for (auto it15 = flow_for(n15, from, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float from, Float n) -> Float { return (from + n); }, kern::to<Float>(from), kern::to<Float>(n)))))))); it15.next();) {
      wire::put_u32(o, d[n15.index].seed);
      wire::put_f32(o, d[n15.index].height);
      wire::put_u8(o, kern::to<Int>(d[n15.index].leveled));
    }
  }

  struct get_damageResult { Bool Result = false; };
  get_damageResult get_damage(wire_Reader& in, const List<U8>& b, List<CityStream_DamageRec>& d) {
    U16 n = 0;
    U16 i = 0;
    CityStream_DamageRec r = CityStream_DamageRec{0, 0.0, false};
    n16 = wire::Reader_get_u16(in, b);
    n = n16.Result;
    list_clear(d);
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), in.ok))) {
      r = CityStream_DamageRec{0, 0.0, false};
      n17 = wire::Reader_get_u32(in, b);
      r.seed = n17.Result;
      n18 = wire::Reader_get_f32(in, b);
      r.height = n18.Result;
      n19 = wire::Reader_get_u8(in, b);
      r.leveled = math_compare<"!=">(kern::to<Float>(n19.Result), 0.0);
      list_add(d, r);
      i += 1;
    }
    return {in.ok};
  }

  void put_points(List<U8>& o, const List<Int_Float_pair>& p, Int from = 0, Int n = 0) {
    wire::put_u16(o, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::band(n, 65535.0); }, kern::to<Float>(n))));
    for (auto it20 = flow_for(n20, from, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float from, Float n) -> Float { return (from + n); }, kern::to<Float>(from), kern::to<Float>(n)))))))); it20.next();) {
      wire::put_u32(o, p[n20.index].first);
      wire::put_f32(o, p[n20.index].second);
    }
  }

  struct get_pointsResult { Bool Result = false; };
  get_pointsResult get_points(wire_Reader& in, const List<U8>& b, List<Int_Float_pair>& p) {
    U16 n = 0;
    U16 i = 0;
    U32 s = 0;
    Float hp = 0.0;
    n21 = wire::Reader_get_u16(in, b);
    n = n21.Result;
    list_clear(p);
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), in.ok))) {
      n22 = wire::Reader_get_u32(in, b);
      s = n22.Result;
      n23 = wire::Reader_get_f32(in, b);
      hp = n23.Result;
      list_add(p, struct_make<Int_Float_pair>(s, hp));
      i += 1;
    }
    return {in.ok};
  }

  /// FNV-1a over the craters (exact) and the damage (to the centimetre, by seed)
  void fnv_byte(U32& h, U32 byte = 0) {
    h = kern::to<Int>(kern::formula([](Float h, Float byte) -> Float { return kern::math::bxor(h, byte); }, kern::to<Float>(h), kern::to<Float>(byte)));
    h = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::mul32(h, 16777619.0); }, kern::to<Float>(h)));
  }

  /// the four bytes of a 32-bit value in the order memory holds them (little-endian: the low one first)
  void fnv_word(U32& h, U32 v = 0) {
    fnv_byte(h, kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(v, 255.0); }, kern::to<Float>(v))));
    fnv_byte(h, kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(kern::math::shr(v, 8.0), 255.0); }, kern::to<Float>(v))));
    fnv_byte(h, kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(kern::math::shr(v, 16.0), 255.0); }, kern::to<Float>(v))));
    fnv_byte(h, kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::shr(v, 24.0); }, kern::to<Float>(v))));
  }

  /// ... and of a 32-bit float (its bits, as a message holds them)
  void fnv_float(U32& h, Float v = 0.0) {
    Fixed<U8, 4> b = Fixed<U8, 4>{};
    U8 x = 0;
    b = wire::f32_bytes(v).Result;
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(b))))); it24.next();) {
      x = b[n24.index];
      fnv_byte(h, x);
    }
  }

  struct digest_ofResult { U32 Result = 0; };
  digest_ofResult digest_of(const List<Terrain_CraterRec>& c, const List<CityStream_DamageRec>& d) {
    U32 h = 0;
    I32 cm = 0;
    h = 2166136261;
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_CraterRec>(c))))); it25.next();) {
      fnv_float(h, c[n25.index].x);
      fnv_float(h, c[n25.index].y);
      fnv_float(h, c[n25.index].r);
      fnv_float(h, c[n25.index].depth);
    }
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_DamageRec>(d))))); it26.next();) {
      cm = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(kern::math::round((a * 100.0))); }, d[n26.index].height));
      fnv_word(h, d[n26.index].seed);
      fnv_word(h, kern::to<Int>(kern::formula([](Float cm) -> Float { return kern::math::wrap32(cm); }, kern::to<Float>(cm))));
      fnv_byte(h, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(d[n26.index].leveled))));
    }
    return {h};
  }

  struct hasResult { Bool Result = false; };
  KERN_PURE hasResult has(const List<Terrain_CraterRec>& v, const Terrain_CraterRec& c) {
    Int at = 0;
    Int end = 0;
    Bool go = false;
    at = 0;
    end = list_length<Terrain_CraterRec>(v);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (terrain::CraterRec_equal(v[at], c).Result) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    return {logic_not(math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<Terrain_CraterRec>(v))))};
  }

  // ---- what nodes give and keep ----
  wire::Reader_get_u8Result n10;
  wire::Reader_get_f32Result n11;
  wire::Reader_get_f32Result n12;
  wire::Reader_get_f32Result n13;
  wire::Reader_get_f32Result n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  wire::Reader_get_u16Result n16;
  wire::Reader_get_u32Result n17;
  wire::Reader_get_f32Result n18;
  wire::Reader_get_u8Result n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  wire::Reader_get_u16Result n21;
  wire::Reader_get_u32Result n22;
  wire::Reader_get_f32Result n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
};
