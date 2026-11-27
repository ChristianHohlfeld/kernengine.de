struct mvt_cpp : virtual kern::Library, virtual mvt {
  // ---- types ----
  KERN_STRUCT(mvt_Reader) { u64 p = 0; u64 end = 0; bool ok = true; };

  // ---- functions ----
  struct Reader_moreResult { bool Result = false; };
  KERN_PURE Reader_moreResult Reader_more(const mvt_Reader& self) {
    return {logic_and(self.ok, math_compare<"<", u64>(self.p, self.end))};
  }

  struct Reader_varintResult { u64 Result = 0; };
  Reader_varintResult Reader_varint(mvt_Reader& self, List<u8> bytes = List<u8>{}) {
    u64 v = 0;
    i32 shift = 0;
    u8 b = 0;
    v = 0;
    shift = 0;
    while (loop(logic_and(math_compare<"<", u64>(self.p, self.end), math_compare<"<", i32>(shift, 64)))) {
      b = bytes[num_cast<i64, u64>(self.p)];
      self.p = kern::formula<u64>([](auto p) { return (p + 1); }, self.p);
      v = kern::formula<u64, u8, i32>([](auto v, auto b, auto shift) { return (v | (num_cast<u64, i32>((num_cast<i32, u8>(b) & 127)) << shift)); }, v, b, shift);
      if (logic_not(kern::formula<u8>([](auto b) { return num_cast<bool, i32>((num_cast<i32, u8>(b) & 128)); }, b))) {
        return {v};
      }
      shift += 7;
    }
    self.ok = false;
    return {0};
  }

  struct Reader_subResult { mvt_Reader Result = mvt_Reader{0, 0, true}; };
  Reader_subResult Reader_sub(mvt_Reader& self, List<u8> bytes = List<u8>{}) {
    u64 n = 0;
    mvt_Reader r = mvt_Reader{0, 0, true};
    n10 = Reader_varint(self, bytes);
    n = n10.Result;
    if (logic_or(logic_not(self.ok), math_compare<">", u64>(n, kern::formula<u64, u64>([](auto end, auto p) { return (end - p); }, self.end, self.p)))) {
      self.ok = false;
      return {struct_make<mvt_Reader>(self.end, self.end, false)};
    }
    r = struct_make<mvt_Reader>(self.p, kern::formula<u64, u64>([](auto p, auto n) { return (p + n); }, self.p, n), true);
    self.p = kern::formula<u64, u64>([](auto p, auto n) { return (p + n); }, self.p, n);
    return {r};
  }

  struct Reader_textResult { Text Result = ""; };
  Reader_textResult Reader_text(mvt_Reader& self, List<u8> bytes = List<u8>{}) {
    u64 n = 0;
    Text s = "";
    n11 = Reader_varint(self, bytes);
    n = n11.Result;
    if (logic_or(logic_not(self.ok), math_compare<">", u64>(n, kern::formula<u64, u64>([](auto end, auto p) { return (end - p); }, self.end, self.p)))) {
      self.ok = false;
      return {""};
    }
    s = text_of_bytes(bytes, num_cast<i64, u64>(self.p), num_cast<i64, u64>(n));
    self.p = kern::formula<u64, u64>([](auto p, auto n) { return (p + n); }, self.p, n);
    return {s};
  }

  void Reader_skip(mvt_Reader& self, List<u8> bytes = List<u8>{}, u32 wire = 0) {
    u32 selected = 0;
    u64 n = 0;
    selected = wire;
    if (math_compare<"==", u32>(selected, 0)) {
      n12 = Reader_varint(self, bytes);
    } else if (math_compare<"==", u32>(selected, 1)) {
      self.p = kern::formula<u64>([](auto p) { return (p + 8); }, self.p);
    } else if (math_compare<"==", u32>(selected, 2)) {
      n13 = Reader_varint(self, bytes);
      n = n13.Result;
      if (math_compare<">", u64>(n, kern::formula<u64, u64>([](auto end, auto p) { return (end - p); }, self.end, self.p))) {
        self.ok = false;
      } else {
        self.p = kern::formula<u64, u64>([](auto p, auto n) { return (p + n); }, self.p, n);
      }
    } else if (math_compare<"==", u32>(selected, 5)) {
      self.p = kern::formula<u64>([](auto p) { return (p + 4); }, self.p);
    } else {
      self.ok = false;
    }
    if (math_compare<">", u64>(self.p, self.end)) {
      self.ok = false;
    }
  }

  struct Reader_le32Result { u32 Result = 0; };
  /// (little-endian, as protobuf writes them: the low byte first)
  KERN_PURE Reader_le32Result Reader_le32(List<u8> bytes = List<u8>{}, u64 at = 0) {
    return {kern::formula<u32, u32, u32, u32>([](auto a, auto a_2, auto a_3, auto a_4) { return (((a | (a_2 << 8)) | (a_3 << 16)) | (a_4 << 24)); }, num_cast<u32, u8>(bytes[num_cast<i64, u64>(at)]), num_cast<u32, u8>(bytes[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 1); }, at))]), num_cast<u32, u8>(bytes[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 2); }, at))]), num_cast<u32, u8>(bytes[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 3); }, at))]))};
  }

  struct Reader_fixed64Result { f64 Result = 0.0; };
  Reader_fixed64Result Reader_fixed64(mvt_Reader& self, List<u8> bytes = List<u8>{}) {
    f64 d = 0.0;
    if (math_compare<"<", u64>(self.end, kern::formula<u64>([](auto p) { return (p + 8); }, self.p))) {
      self.ok = false;
      return {0.0};
    }
    d = num_bit_cast<f64, u64>(kern::formula<u64, u32>([](auto a, auto the_result) { return ((a << 32) | num_cast<u64, u32>(the_result)); }, num_cast<u64, u32>(Reader_le32(bytes, kern::formula<u64>([](auto p) { return (p + 4); }, self.p)).Result), Reader_le32(bytes, self.p).Result));
    self.p = kern::formula<u64>([](auto p) { return (p + 8); }, self.p);
    return {d};
  }

  struct Reader_fixed32Result { f32 Result = 0.0f; };
  Reader_fixed32Result Reader_fixed32(mvt_Reader& self, List<u8> bytes = List<u8>{}) {
    f32 f = 0.0f;
    if (math_compare<"<", u64>(self.end, kern::formula<u64>([](auto p) { return (p + 4); }, self.p))) {
      self.ok = false;
      return {0.0f};
    }
    f = num_bit_cast<f32, u32>(Reader_le32(bytes, self.p).Result);
    self.p = kern::formula<u64>([](auto p) { return (p + 4); }, self.p);
    return {f};
  }

  struct zigzagResult { i64 Result = 0; };
  KERN_PURE zigzagResult zigzag(u64 v = 0) {
    return {kern::formula<u64>([](auto v) { return (num_cast<i64, u64>((v >> 1)) ^ (-num_cast<i64, u64>((v & 1)))); }, v)};
  }

  struct read_valueResult { mvt_Value Result = mvt_Value{mvt_Value_Kind::none, "", 0.0, false}; };
  read_valueResult read_value(List<u8> bytes = List<u8>{}, mvt_Reader r = mvt_Reader{0, 0, true}) {
    mvt_Reader r_ = mvt_Reader{0, 0, true};
    mvt_Value v = mvt_Value{mvt_Value_Kind::none, "", 0.0, false};
    u64 key = 0;
    u32 field = 0;
    u32 wire = 0;
    u32 selected = 0;
    r_ = r;
    v = mvt_Value{mvt_Value_Kind::none, "", 0.0, false};
    while (loop(Reader_more(r_).Result)) {
      n14 = Reader_varint(r_, bytes);
      key = n14.Result;
      field = kern::formula<u64>([](auto key) { return num_cast<u32, u64>((key >> 3)); }, key);
      wire = kern::formula<u64>([](auto key) { return num_cast<u32, u64>((key & 7)); }, key);
      selected = field;
      if (math_compare<"==", u32>(selected, 1)) {
        v.kind = mvt_Value_Kind::string;
        n15 = Reader_text(r_, bytes);
        v.s = n15.Result;
      } else if (math_compare<"==", u32>(selected, 2)) {
        v.kind = mvt_Value_Kind::number;
        n16 = Reader_fixed32(r_, bytes);
        v.n = num_cast<f64, f32>(n16.Result);
      } else if (math_compare<"==", u32>(selected, 3)) {
        v.kind = mvt_Value_Kind::number;
        n17 = Reader_fixed64(r_, bytes);
        v.n = n17.Result;
      } else if (math_compare<"==", u32>(selected, 4)) {
        v.kind = mvt_Value_Kind::number;
        n18 = Reader_varint(r_, bytes);
        v.n = num_cast<f64, i64>(num_cast<i64, u64>(n18.Result));
      } else if (math_compare<"==", u32>(selected, 5)) {
        v.kind = mvt_Value_Kind::number;
        n19 = Reader_varint(r_, bytes);
        v.n = num_cast<f64, u64>(n19.Result);
      } else if (math_compare<"==", u32>(selected, 6)) {
        v.kind = mvt_Value_Kind::number;
        n20 = Reader_varint(r_, bytes);
        v.n = num_cast<f64, i64>(zigzag(n20.Result).Result);
      } else if (math_compare<"==", u32>(selected, 7)) {
        v.kind = mvt_Value_Kind::boolean;
        n21 = Reader_varint(r_, bytes);
        v.b = math_compare<"!=", u64>(n21.Result, 0);
      } else {
        Reader_skip(r_, bytes, wire);
      }
    }
    return {v};
  }

  void read_geometry(List<Vector_row>& parts, List<u8> bytes = List<u8>{}, mvt_Reader r = mvt_Reader{0, 0, true}, mvt_GeomType type = mvt_GeomType::unknown) {
    mvt_Reader r_ = mvt_Reader{0, 0, true};
    i64 x = 0;
    i64 y = 0;
    bool open = false;
    u64 c = 0;
    u32 id = 0;
    u32 count = 0;
    u32 i = 0;
    r_ = r;
    x = 0;
    y = 0;
    open = false;
    while (loop(Reader_more(r_).Result)) {
      n22 = Reader_varint(r_, bytes);
      c = n22.Result;
      id = kern::formula<u64>([](auto c) { return num_cast<u32, u64>((c & 7)); }, c);
      count = kern::formula<u64>([](auto c) { return num_cast<u32, u64>((c >> 3)); }, c);
      if (logic_or(math_compare<"==", u32>(id, 1), math_compare<"==", u32>(id, 2))) {
        i = 0;
        while (loop(logic_and(math_compare<"<", u32>(i, count), Reader_more(r_).Result))) {
          n23 = Reader_varint(r_, bytes);
          x += zigzag(n23.Result).Result;
          n24 = Reader_varint(r_, bytes);
          y += zigzag(n24.Result).Result;
          if (logic_or(math_compare<"==", u32>(id, 1), logic_not(open))) {
            list_add(parts, Vector_row{List<Vector>{}});
            open = true;
          }
          list_add(parts[math_sub<i64>(list_length<Vector_row>(parts), 1)].items, vector_make(num_cast<f32, i64>(x), num_cast<f32, i64>(y), 0.0f));
          i += 1;
        }
      } else if (math_compare<"==", u32>(id, 7)) {
      } else {
        return;
      }
    }
  }

  struct Layer_tagResult { i32 Result = 0; };
  KERN_PURE Layer_tagResult Layer_tag(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    for (auto it25 = flow_for(n25, 0, math_sub<i64>(list_length<u32_u32_pair>(f.tags), 1)); it25.next();) {
      if (logic_and(logic_and(math_compare<"<", u64>(num_cast<u64, u32>(f.tags[n25.index].first), num_cast<u64, i64>(list_length<Text>(self.keys))), math_compare<"<", u64>(num_cast<u64, u32>(f.tags[n25.index].second), num_cast<u64, i64>(list_length<mvt_Value>(self.values)))), text_equal(key, self.keys[num_cast<i64, u32>(f.tags[n25.index].first)]))) {
        return {num_cast<i32, u32>(f.tags[n25.index].second)};
      }
    }
    return {-1};
  }

  struct Layer_textResult { Text Result = ""; };
  KERN_PURE Layer_textResult Layer_text(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    i32 v = 0;
    v = Layer_tag(self, f, key).Result;
    return {logic_select<Text>(logic_and(math_compare<">=", i32>(v, 0), enum_equal<mvt_Value_Kind>(self.values[num_cast<i64, i32>(v)].kind, mvt_Value_Kind::string)), self.values[num_cast<i64, i32>(v)].s, "")};
  }

  struct Layer_numberResult { f64 Result = 0.0; };
  KERN_PURE Layer_numberResult Layer_number(const mvt_Layer& self, const mvt_Feature& f, Text key = "", f64 fallback = 0.0) {
    i32 v = 0;
    v = Layer_tag(self, f, key).Result;
    return {kern::formula<i32, bool, f64, f64>([](auto v, auto a, auto a_2, auto fallback) { return (((v >= 0) && a) ? a_2 : fallback); }, v, enum_equal<mvt_Value_Kind>(self.values[num_cast<i64, i32>(v)].kind, mvt_Value_Kind::number), self.values[num_cast<i64, i32>(v)].n, fallback)};
  }

  struct Tile_layerResult { i32 Result = 0; };
  KERN_PURE Tile_layerResult Tile_layer(const mvt_Tile& self, Text name = "") {
    for (auto it26 = flow_for(n26, 0, math_sub<i64>(list_length<mvt_Layer>(self.layers), 1)); it26.next();) {
      if (text_equal(name, self.layers[n26.index].name)) {
        return {num_cast<i32, i64>(n26.index)};
      }
    }
    return {-1};
  }

  struct ring_areaResult { f64 Result = 0.0; };
  KERN_PURE ring_areaResult ring_area(const List<Vector>& ring) {
    f64 a = 0.0;
    u64 n = 0;
    u64 i = 0;
    u64 j = 0;
    u64 was = 0;
    a = 0.0;
    n = num_cast<u64, i64>(list_length<Vector>(ring));
    i = 0;
    j = kern::formula<u64, u64>([](auto n, auto a0) { return (num_cast<bool, u64>(n) ? (n - 1) : a0); }, n, 0);
    while (loop(math_compare<"<", u64>(i, n))) {
      a += kern::formula<f32, f32, f32, f32>([](auto x, auto y, auto x_2, auto y_2) { return ((num_cast<f64, f32>(x) * num_cast<f64, f32>(y)) - (num_cast<f64, f32>(x_2) * num_cast<f64, f32>(y_2))); }, vector_break(ring[num_cast<i64, u64>(j)]).x, vector_break(ring[num_cast<i64, u64>(i)]).y, vector_break(ring[num_cast<i64, u64>(i)]).x, vector_break(ring[num_cast<i64, u64>(j)]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula<f64>([](auto a) { return (a * 0.5); }, a)};
  }

  struct wanted_layerResult { bool Result = false; };
  KERN_PURE wanted_layerResult wanted_layer(const Text& name, const List<Text>& wanted) {
    if (math_compare<"==", i64>(list_length<Text>(wanted), 0)) {
      return {true};
    }
    for (auto it27 = flow_for(n27, 0, math_sub<i64>(list_length<Text>(wanted), 1)); it27.next();) {
      if (text_equal(wanted[n27.index], name)) {
        return {true};
      }
    }
    return {false};
  }

  struct open_layerResult { bool Result = false; };
  /// Layer header (name, keys, values, extent) and where its features lie in the bytes.
  open_layerResult open_layer(mvt_Layer& layer, List<mvt_Piece>& features, List<u8> bytes = List<u8>{}, mvt_Reader lr = mvt_Reader{0, 0, true}) {
    mvt_Reader lr_ = mvt_Reader{0, 0, true};
    u64 k = 0;
    u32 field = 0;
    u32 wire = 0;
    u32 selected = 0;
    mvt_Reader f = mvt_Reader{0, 0, true};
    List<u8> arg = List<u8>{};
    lr_ = lr;
    while (loop(Reader_more(lr_).Result)) {
      n28 = Reader_varint(lr_, bytes);
      k = n28.Result;
      field = kern::formula<u64>([](auto k) { return num_cast<u32, u64>((k >> 3)); }, k);
      wire = kern::formula<u64>([](auto k) { return num_cast<u32, u64>((k & 7)); }, k);
      selected = field;
      if (math_compare<"==", u32>(selected, 1)) {
        n29 = Reader_text(lr_, bytes);
        layer.name = n29.Result;
      } else if (math_compare<"==", u32>(selected, 2)) {
        n30 = Reader_sub(lr_, bytes);
        f = n30.Result;
        list_add(features, struct_make<mvt_Piece>(f.p, f.end));
      } else if (math_compare<"==", u32>(selected, 3)) {
        n31 = Reader_text(lr_, bytes);
        list_add(layer.keys, n31.Result);
      } else if (math_compare<"==", u32>(selected, 4)) {
        arg = bytes;
        n32 = Reader_sub(lr_, bytes);
        n33 = read_value(arg, n32.Result);
        list_add(layer.values, n33.Result);
      } else if (math_compare<"==", u32>(selected, 5)) {
        n34 = Reader_varint(lr_, bytes);
        layer.extent = num_cast<u32, u64>(n34.Result);
      } else {
        Reader_skip(lr_, bytes, wire);
      }
    }
    return {lr_.ok};
  }

  struct read_featureResult { bool Result = false; };
  read_featureResult read_feature(mvt_Feature& f, List<u8> bytes = List<u8>{}, mvt_Reader fr = mvt_Reader{0, 0, true}) {
    mvt_Reader fr_ = mvt_Reader{0, 0, true};
    mvt_Reader geometry = mvt_Reader{0, 0, true};
    bool has_geometry = false;
    u64 k = 0;
    u32 field = 0;
    u32 wire = 0;
    mvt_Reader t = mvt_Reader{0, 0, true};
    u32 a = 0;
    u32 b = 0;
    fr_ = fr;
    geometry = mvt_Reader{0, 0, true};
    has_geometry = false;
    while (loop(Reader_more(fr_).Result)) {
      n35 = Reader_varint(fr_, bytes);
      k = n35.Result;
      field = kern::formula<u64>([](auto k) { return num_cast<u32, u64>((k >> 3)); }, k);
      wire = kern::formula<u64>([](auto k) { return num_cast<u32, u64>((k & 7)); }, k);
      if (logic_and(math_compare<"==", u32>(field, 1), math_compare<"==", u32>(wire, 0))) {
        n36 = Reader_varint(fr_, bytes);
        f.id = n36.Result;
      } else if (logic_and(math_compare<"==", u32>(field, 2), math_compare<"==", u32>(wire, 2))) {
        n37 = Reader_sub(fr_, bytes);
        t = n37.Result;
        while (loop(Reader_more(t).Result)) {
          n38 = Reader_varint(t, bytes);
          a = num_cast<u32, u64>(n38.Result);
          n39 = Reader_varint(t, bytes);
          b = num_cast<u32, u64>(n39.Result);
          list_add(f.tags, struct_make<u32_u32_pair>(a, b));
        }
      } else if (logic_and(math_compare<"==", u32>(field, 3), math_compare<"==", u32>(wire, 0))) {
        n40 = Reader_varint(fr_, bytes);
        f.type = enum_from_index<mvt_GeomType>(num_cast<i64, u64>(n40.Result)).value;
      } else if (logic_and(math_compare<"==", u32>(field, 4), math_compare<"==", u32>(wire, 2))) {
        n41 = Reader_sub(fr_, bytes);
        geometry = n41.Result;
        has_geometry = true;
      } else {
        Reader_skip(fr_, bytes, wire);
      }
    }
    if (logic_not(fr_.ok)) {
      return {false};
    }
    if (has_geometry) {
      read_geometry(f.parts, bytes, geometry, f.type);
    }
    return {true};
  }

  void Decoder_begin(mvt_Decoder& self, const List<Text>& wanted, List<u8> bytes = List<u8>{}, u64 at = 0) {
    mvt_Reader tile = mvt_Reader{0, 0, true};
    bool skip = false;
    u64 key = 0;
    mvt_Reader lr = mvt_Reader{0, 0, true};
    Text name = "";
    mvt_Reader peek = mvt_Reader{0, 0, true};
    bool stop = false;
    u64 k = 0;
    list_clear(self.layers_);
    list_clear(self.features_);
    list_clear(self.tile_.layers);
    self.feature_ = 0;
    self.layer_ = self.feature_;
    self.layer_open_ = false;
    self.done_ = false;
    self.failed_ = false;
    tile = struct_make<mvt_Reader>(kern::formula<u64, u64>([](auto at, auto a) { return ((at < a) ? at : a); }, at, num_cast<u64, i64>(list_length<u8>(bytes))), num_cast<u64, i64>(list_length<u8>(bytes)), true);
    while (loop(Reader_more(tile).Result)) {
      skip = false;
      n42 = Reader_varint(tile, bytes);
      key = n42.Result;
      if (logic_or(math_compare<"!=", u64>(kern::formula<u64>([](auto key) { return (key >> 3); }, key), 3), math_compare<"!=", u64>(kern::formula<u64>([](auto key) { return (key & 7); }, key), 2))) {
        Reader_skip(tile, bytes, kern::formula<u64>([](auto key) { return num_cast<u32, u64>((key & 7)); }, key));
        skip = true;
      }
      if (logic_not(skip)) {
        n43 = Reader_sub(tile, bytes);
        lr = n43.Result;
        name = "";
        peek = lr;
        stop = false;
        while (loop(logic_and(logic_not(stop), Reader_more(peek).Result))) {
          n44 = Reader_varint(peek, bytes);
          k = n44.Result;
          if (logic_and(math_compare<"==", u64>(kern::formula<u64>([](auto k) { return (k >> 3); }, k), 1), math_compare<"==", u64>(kern::formula<u64>([](auto k) { return (k & 7); }, k), 2))) {
            n45 = Reader_text(peek, bytes);
            name = n45.Result;
            stop = true;
          }
          if (logic_not(stop)) {
            Reader_skip(peek, bytes, kern::formula<u64>([](auto k) { return num_cast<u32, u64>((k & 7)); }, k));
          }
        }
        if (wanted_layer(name, wanted).Result) {
          list_add(self.layers_, struct_make<mvt_Piece>(lr.p, lr.end));
        }
      }
    }
    if (logic_not(tile.ok)) {
      self.failed_ = true;
      self.done_ = true;
    }
  }

  struct Decoder_stepResult { bool Result = false; };
  Decoder_stepResult Decoder_step(mvt_Decoder& self, List<u8> bytes = List<u8>{}, f64 budget_ms = 0.0) {
    f64 start = 0.0;
    f64 now_ms = 0.0;
    u32 work = 0;
    mvt_Layer layer = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
    List<mvt_Piece> pieces = List<mvt_Piece>{};
    u64 layer_at = 0;
    mvt_Feature f = mvt_Feature{0, mvt_GeomType::unknown, List<u32_u32_pair>{}, List<Vector_row>{}};
    f64 now_ms_2 = 0.0;
    f64 now_ms_3 = 0.0;
    bool both = false;
    if (self.done_) {
      return {true};
    }
    now_ms = engine_now_ms();
    start = now_ms;
    work = 0;
    while (loop(math_compare<"<", u64>(self.layer_, num_cast<u64, i64>(list_length<mvt_Piece>(self.layers_))))) {
      if (logic_not(self.layer_open_)) {
        layer = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
        pieces = List<mvt_Piece>{};
        n46 = open_layer(layer, pieces, bytes, struct_make<mvt_Reader>(self.layers_[num_cast<i64, u64>(self.layer_)].at, self.layers_[num_cast<i64, u64>(self.layer_)].end, true));
        if (logic_not(n46.Result)) {
          self.done_ = true;
          self.failed_ = self.done_;
          return {true};
        }
        list_clear(self.features_);
        for (auto it47 = flow_for(n47, 0, math_sub<i64>(list_length<mvt_Piece>(pieces), 1)); it47.next();) {
          list_add(self.features_, pieces[n47.index]);
        }
        list_reserve(layer.features, list_length<mvt_Piece>(self.features_));
        list_add(self.tile_.layers, layer);
        self.feature_ = 0;
        self.layer_open_ = true;
      }
      layer_at = num_cast<u64, i64>(math_sub<i64>(list_length<mvt_Layer>(self.tile_.layers), 1));
      while (loop(math_compare<"<", u64>(self.feature_, num_cast<u64, i64>(list_length<mvt_Piece>(self.features_))))) {
        f = mvt_Feature{0, mvt_GeomType::unknown, List<u32_u32_pair>{}, List<Vector_row>{}};
        n48 = read_feature(f, bytes, struct_make<mvt_Reader>(self.features_[num_cast<i64, u64>(self.feature_)].at, self.features_[num_cast<i64, u64>(self.feature_)].end, true));
        if (logic_not(n48.Result)) {
          self.done_ = true;
          self.failed_ = self.done_;
          return {true};
        }
        list_add(self.tile_.layers[num_cast<i64, u64>(layer_at)].features, f);
        self.feature_ = kern::formula<u64>([](auto feature_) { return (feature_ + 1); }, self.feature_);
        work += 1;
        both = logic_and(math_compare<"==", u32>(kern::formula<u32>([](auto work) { return (work & 63); }, work), 0), math_compare<">", f64>(budget_ms, 0.0));
        if (both) {
          now_ms_3 = engine_now_ms();
          both = math_compare<">", f64>(kern::formula<f64, f64>([](auto now_ms_3, auto start) { return (now_ms_3 - start); }, now_ms_3, start), budget_ms);
        }
        if (both) {
          return {false};
        }
      }
      self.layer_open_ = false;
      self.layer_ = kern::formula<u64>([](auto layer_) { return (layer_ + 1); }, self.layer_);
    }
    self.done_ = true;
    return {true};
  }

  struct decodeResult { bool Result = false; };
  decodeResult decode(mvt_Tile& out, const List<Text>& wanted, List<u8> bytes = List<u8>{}) {
    mvt_Decoder d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    Decoder_begin(d, wanted, bytes, 0);
    n49 = Decoder_step(d, bytes, 0.0);
    var_take(n50, d.tile_);
    out = n50.value;
    return {logic_not(mvt::Decoder_failed(d).Result)};
  }

  void put_varint(List<u8>& o, u64 v = 0) {
    u64 v_ = 0;
    v_ = v;
    while (loop(math_compare<">=", u64>(v_, 128))) {
      list_add(o, kern::formula<u64>([](auto v_) { return num_cast<u8, u64>((v_ | 128)); }, v_));
      v_ = kern::formula<u64>([](auto v_) { return (v_ >> 7); }, v_);
    }
    list_add(o, num_cast<u8, u64>(v_));
  }

  void put_key(List<u8>& o, u32 field = 0, u32 wire = 0) {
    put_varint(o, kern::formula<u32, u32>([](auto field, auto wire) { return ((num_cast<u64, u32>(field) << 3) | num_cast<u64, u32>(wire)); }, field, wire));
  }

  /// a length-delimited field: bytes (a packed field, a message) or a text (a name, a key, a value)
  void put_bytes(List<u8>& o, const List<u8>& data, u32 field = 0) {
    i64 place = 0;
    put_key(o, field, 2);
    put_varint(o, num_cast<u64, i64>(list_length<u8>(data)));
    place = list_length<u8>(o);
    list_insert_list(o, place, data);
  }

  void put_bytes_2(List<u8>& o, u32 field = 0, Text text = "") {
    put_key(o, field, 2);
    put_varint(o, num_cast<u64, i64>(text_length(text)));
    list_insert_text(o, list_length<u8>(o), text);
  }

  struct zzResult { u64 Result = 0; };
  KERN_PURE zzResult zz(i64 v = 0) {
    return {kern::formula<i64>([](auto v) { return ((num_cast<u64, i64>(v) << 1) ^ num_cast<u64, i64>((v >> 63))); }, v)};
  }

  void Writer_begin_layer(mvt_Writer& self, const Text& name, u32 extent = 0) {
    self.layer_ = struct_make<mvt_Writer_PendingLayer>("", 4096, List<Text>{}, List<mvt_Value>{}, List<u8_row>{});
    self.layer_.name = name;
    self.layer_.extent = extent;
  }

  void Writer_feature(mvt_Writer& self, const List<Vector_row>& parts, const List<Text_mvt_Value_pair>& tags, mvt_GeomType type = mvt_GeomType::unknown, u64 id = 0) {
    List<u8> f = List<u8>{};
    List<u8> t = List<u8>{};
    u32 ki = 0;
    u32 vi = 0;
    bool stop = false;
    List<u8> g = List<u8>{};
    i64 x = 0;
    i64 y = 0;
    bool skip = false;
    i64 px = 0;
    i64 py = 0;
    i64 qx = 0;
    i64 qy = 0;
    f = List<u8>{};
    if (num_cast<bool, u64>(id)) {
      put_key(f, 1, 0);
      put_varint(f, id);
    }
    t = List<u8>{};
    for (auto it51 = flow_for(n51, 0, math_sub<i64>(list_length<Text_mvt_Value_pair>(tags), 1)); it51.next();) {
      ki = 0;
      vi = 0;
      while (loop(logic_and(math_compare<"<", u64>(num_cast<u64, u32>(ki), num_cast<u64, i64>(list_length<Text>(self.layer_.keys))), logic_not(text_equal(self.layer_.keys[num_cast<i64, u32>(ki)], tags[n51.index].first))))) {
        ki += 1;
      }
      if (math_compare<"==", u64>(num_cast<u64, u32>(ki), num_cast<u64, i64>(list_length<Text>(self.layer_.keys)))) {
        list_add(self.layer_.keys, tags[n51.index].first);
      }
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<", u64>(num_cast<u64, u32>(vi), num_cast<u64, i64>(list_length<mvt_Value>(self.layer_.values)))))) {
        if (logic_and(logic_and(logic_and(enum_equal<mvt_Value_Kind>(self.layer_.values[num_cast<i64, u32>(vi)].kind, tags[n51.index].second.kind), text_equal(self.layer_.values[num_cast<i64, u32>(vi)].s, tags[n51.index].second.s)), math_compare<"==", f64>(self.layer_.values[num_cast<i64, u32>(vi)].n, tags[n51.index].second.n)), math_compare<"==", i32>(num_cast<i32, bool>(self.layer_.values[num_cast<i64, u32>(vi)].b), num_cast<i32, bool>(tags[n51.index].second.b)))) {
          stop = true;
        }
        if (logic_not(stop)) {
          vi += 1;
        }
      }
      if (math_compare<"==", u64>(num_cast<u64, u32>(vi), num_cast<u64, i64>(list_length<mvt_Value>(self.layer_.values)))) {
        list_add(self.layer_.values, tags[n51.index].second);
      }
      put_varint(t, num_cast<u64, u32>(ki));
      put_varint(t, num_cast<u64, u32>(vi));
    }
    put_bytes(f, t, 2);
    put_key(f, 3, 0);
    put_varint(f, num_cast<u64, i64>(enum_index<mvt_GeomType>(type)));
    g = List<u8>{};
    x = 0;
    y = 0;
    for (auto it52 = flow_for(n52, 0, math_sub<i64>(list_length<Vector_row>(parts), 1)); it52.next();) {
      skip = false;
      if (math_compare<"==", i64>(list_length<Vector>(struct_break<Vector_row>(parts[n52.index]).items), 0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        put_varint(g, 9);
        px = kern::formula<f32>([](auto x) { return num_cast<i64, f32>(round(x)); }, vector_break(parts[n52.index].items[0]).x);
        py = kern::formula<f32>([](auto y) { return num_cast<i64, f32>(round(y)); }, vector_break(parts[n52.index].items[0]).y);
        put_varint(g, zz(kern::formula<i64, i64>([](auto px, auto x) { return (px - x); }, px, x)).Result);
        put_varint(g, zz(kern::formula<i64, i64>([](auto py, auto y) { return (py - y); }, py, y)).Result);
        x = px;
        y = py;
        if (math_compare<">", u64>(num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(parts[n52.index]).items)), 1)) {
          put_varint(g, kern::formula<u64>([](auto a) { return (((a - 1) << 3) | 2); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(parts[n52.index]).items))));
          for (auto it53 = flow_for(n53, 1, math_sub<i64>(list_length<Vector>(struct_break<Vector_row>(parts[n52.index]).items), 1)); it53.next();) {
            qx = kern::formula<f32>([](auto x) { return num_cast<i64, f32>(round(x)); }, vector_break(parts[n52.index].items[n53.index]).x);
            qy = kern::formula<f32>([](auto y) { return num_cast<i64, f32>(round(y)); }, vector_break(parts[n52.index].items[n53.index]).y);
            put_varint(g, zz(kern::formula<i64, i64>([](auto qx, auto x) { return (qx - x); }, qx, x)).Result);
            put_varint(g, zz(kern::formula<i64, i64>([](auto qy, auto y) { return (qy - y); }, qy, y)).Result);
            x = qx;
            y = qy;
          }
        }
        if (enum_equal<mvt_GeomType>(type, mvt_GeomType::polygon)) {
          put_varint(g, 15);
        }
      }
    }
    put_bytes(f, g, 4);
    list_add(self.layer_.features, struct_make<u8_row>(f));
  }

  void Writer_end_layer(mvt_Writer& self) {
    List<u8> l = List<u8>{};
    List<u8> e = List<u8>{};
    u64 bits = 0;
    Fixed<u8, 8> bytes = Fixed<u8, 8>{};
    i64 p_at = 0;
    i64 place = 0;
    i64 first = 0;
    i64 last = 0;
    l = List<u8>{};
    put_key(l, 15, 0);
    put_varint(l, 2);
    put_bytes_2(l, 1, self.layer_.name);
    for (auto it54 = flow_for(n54, 0, math_sub<i64>(list_length<u8_row>(self.layer_.features), 1)); it54.next();) {
      put_bytes(l, self.layer_.features[n54.index].items, 2);
    }
    for (auto it55 = flow_for(n55, 0, math_sub<i64>(list_length<Text>(self.layer_.keys), 1)); it55.next();) {
      put_bytes_2(l, 3, self.layer_.keys[n55.index]);
    }
    for (auto it56 = flow_for(n56, 0, math_sub<i64>(list_length<mvt_Value>(self.layer_.values), 1)); it56.next();) {
      e = List<u8>{};
      switch (flow_switch_enum<mvt_Value_Kind>(self.layer_.values[n56.index].kind)) {
        case mvt_Value_Kind::none: {
        } break;
        case mvt_Value_Kind::string: {
          put_bytes_2(e, 1, self.layer_.values[n56.index].s);
        } break;
        case mvt_Value_Kind::number: {
          put_key(e, 3, 1);
          bits = num_bit_cast<u64, f64>(self.layer_.values[n56.index].n);
          bytes[0] = num_cast<u8, u64>(bits);
          bytes[1] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 8); }, bits);
          bytes[2] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 16); }, bits);
          bytes[3] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 24); }, bits);
          bytes[4] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 32); }, bits);
          bytes[5] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 40); }, bits);
          bytes[6] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 48); }, bits);
          bytes[7] = kern::formula<u64>([](auto b) { return num_cast<u8, u64>(b >> 56); }, bits);
          p_at = 0;
          place = list_length<u8>(e);
          first = p_at;
          last = math_sub<i64>(math_add<i64>(p_at, 8), 1);
          for (auto it57 = flow_for(n57, first, last); it57.next();) {
            list_insert(e, math_add<i64>(place, math_sub<i64>(n57.index, first)), list_get<u8>(bytes, n57.index));
          }
        } break;
        case mvt_Value_Kind::boolean: {
          put_key(e, 7, 0);
          put_varint(e, kern::formula<bool, i32, i32>([](auto a, auto a1, auto a0) { return num_cast<u64, i32>((a ? a1 : a0)); }, self.layer_.values[n56.index].b, 1, 0));
        } break;
      }
      put_bytes(l, e, 4);
    }
    put_key(l, 5, 0);
    put_varint(l, num_cast<u64, u32>(self.layer_.extent));
    put_bytes(self.out_, l, 3);
  }

  // ---- what nodes give and keep ----
  Reader_varintResult n10;
  Reader_varintResult n11;
  Reader_varintResult n12;
  Reader_varintResult n13;
  Reader_varintResult n14;
  Reader_textResult n15;
  Reader_fixed32Result n16;
  Reader_fixed64Result n17;
  Reader_varintResult n18;
  Reader_varintResult n19;
  Reader_varintResult n20;
  Reader_varintResult n21;
  Reader_varintResult n22;
  Reader_varintResult n23;
  Reader_varintResult n24;
  struct : kern::Node<25> { i64 index = 0; } n25;
  struct : kern::Node<26> { i64 index = 0; } n26;
  struct : kern::Node<27> { i64 index = 0; } n27;
  Reader_varintResult n28;
  Reader_textResult n29;
  Reader_subResult n30;
  Reader_textResult n31;
  Reader_subResult n32;
  read_valueResult n33;
  Reader_varintResult n34;
  Reader_varintResult n35;
  Reader_varintResult n36;
  Reader_subResult n37;
  Reader_varintResult n38;
  Reader_varintResult n39;
  Reader_varintResult n40;
  Reader_subResult n41;
  Reader_varintResult n42;
  Reader_subResult n43;
  Reader_varintResult n44;
  Reader_textResult n45;
  open_layerResult n46;
  struct : kern::Node<47> { i64 index = 0; } n47;
  read_featureResult n48;
  Decoder_stepResult n49;
  struct { mvt_Tile value = mvt_Tile{List<mvt_Layer>{}}; } n50;
  struct : kern::Node<51> { i64 index = 0; } n51;
  struct : kern::Node<52> { i64 index = 0; } n52;
  struct : kern::Node<53> { i64 index = 0; } n53;
  struct : kern::Node<54> { i64 index = 0; } n54;
  struct : kern::Node<55> { i64 index = 0; } n55;
  struct : kern::Node<56> { i64 index = 0; } n56;
  struct : kern::Node<57> { i64 index = 0; } n57;
};
