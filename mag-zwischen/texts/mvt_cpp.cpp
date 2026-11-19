struct mvt_cpp : virtual kern::Library, virtual mvt {
  // ---- types ----
  KERN_STRUCT(mvt_Reader) { Int p = 0; Int end = 0; Bool ok = true; };

  // ---- functions ----
  struct Reader_moreResult { Bool Result = false; };
  KERN_PURE Reader_moreResult Reader_more(const mvt_Reader& self) {
    return {logic_and(self.ok, math_compare<">">(kern::formula([](Float p, Float end) -> Float { return ((p < 0.0) != (end < 0.0) ? (end < 0.0) : (p < end)); }, kern::to<Float>(self.p), kern::to<Float>(self.end)), 0.5))};
  }

  struct Reader_varintResult { Int64 Result = Int64{0x0}; };
  Reader_varintResult Reader_varint(mvt_Reader& self, const List<Int>& bytes) {
    Int64 v = Int64{0x0};
    Int shift = 0;
    Int b = 0;
    v = Int64{0x0};
    shift = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float p, Float end) -> Float { return ((p < 0.0) != (end < 0.0) ? (end < 0.0) : (p < end)); }, kern::to<Float>(self.p), kern::to<Float>(self.end)), 0.5), math_compare<"<">(kern::to<Float>(shift), 64.0)))) {
      b = bytes[self.p];
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 1.0); }, kern::to<Float>(self.p)));
      v = int64_or(v, int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float b) -> Float { return kern::math::band(b, 127.0); }, kern::to<Float>(b)))), shift));
      if (logic_not(math_compare<">">(kern::formula([](Float b) -> Float { return (kern::math::band(b, 128.0) != 0.0); }, kern::to<Float>(b)), 0.5))) {
        return {v};
      }
      shift += 7;
    }
    self.ok = false;
    return {Int64{0x0}};
  }

  struct Reader_subResult { mvt_Reader Result = mvt_Reader{0, 0, true}; };
  Reader_subResult Reader_sub(mvt_Reader& self, const List<Int>& bytes) {
    Int64 n = Int64{0x0};
    mvt_Reader r = mvt_Reader{0, 0, true};
    n10 = Reader_varint(self, bytes);
    n = n10.Result;
    if (logic_or(logic_not(self.ok), int64_less_unsigned(int64_of_int(kern::to<Int>(kern::formula([](Float end, Float p) -> Float { return (end - p); }, kern::to<Float>(self.end), kern::to<Float>(self.p)))), n))) {
      self.ok = false;
      return {struct_make<mvt_Reader>(self.end, self.end, false)};
    }
    r = struct_make<mvt_Reader>(self.p, int64_to_int(int64_add(int64_of_int(self.p), n)), true);
    self.p = int64_to_int(int64_add(int64_of_int(self.p), n));
    return {r};
  }

  struct Reader_textResult { Text Result = ""; };
  Reader_textResult Reader_text(mvt_Reader& self, const List<Int>& bytes) {
    Int64 n = Int64{0x0};
    Text s = "";
    n11 = Reader_varint(self, bytes);
    n = n11.Result;
    if (logic_or(logic_not(self.ok), int64_less_unsigned(int64_of_int(kern::to<Int>(kern::formula([](Float end, Float p) -> Float { return (end - p); }, kern::to<Float>(self.end), kern::to<Float>(self.p)))), n))) {
      self.ok = false;
      return {""};
    }
    s = text_of_bytes(bytes, self.p, int64_to_int(n));
    self.p = int64_to_int(int64_add(int64_of_int(self.p), n));
    return {s};
  }

  void Reader_skip(mvt_Reader& self, const List<Int>& bytes, Int wire = 0) {
    Int selected = 0;
    Int64 n = Int64{0x0};
    selected = wire;
    if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
      n12 = Reader_varint(self, bytes);
    } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 8.0); }, kern::to<Float>(self.p)));
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      n13 = Reader_varint(self, bytes);
      n = n13.Result;
      if (int64_less_unsigned(int64_of_int(kern::to<Int>(kern::formula([](Float end, Float p) -> Float { return (end - p); }, kern::to<Float>(self.end), kern::to<Float>(self.p)))), n)) {
        self.ok = false;
      } else {
        self.p = int64_to_int(int64_add(int64_of_int(self.p), n));
      }
    } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)));
    } else {
      self.ok = false;
    }
    if (math_compare<">">(kern::formula([](Float end, Float p) -> Float { return ((end < 0.0) != (p < 0.0) ? (p < 0.0) : (end < p)); }, kern::to<Float>(self.end), kern::to<Float>(self.p)), 0.5)) {
      self.ok = false;
    }
  }

  struct Reader_le32Result { Int Result = 0; };
  /// (little-endian, as protobuf writes them: the low byte first)
  KERN_PURE Reader_le32Result Reader_le32(const List<Int>& bytes, Int at = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(a, kern::math::wrap32(kern::math::shl(a_2, 8.0))), kern::math::wrap32(kern::math::shl(a_3, 16.0))), kern::math::wrap32(kern::math::shl(a_4, 24.0))); }, kern::to<Float>(bytes[at]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(at)))]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(at)))]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 3.0); }, kern::to<Float>(at)))])))};
  }

  struct Reader_fixed64Result { Float Result = 0.0; };
  Reader_fixed64Result Reader_fixed64(mvt_Reader& self, const List<Int>& bytes) {
    Float d = 0.0;
    if (math_compare<">">(kern::formula([](Float end, Float a) -> Float { return ((end < 0.0) != (a < 0.0) ? (a < 0.0) : (end < a)); }, kern::to<Float>(self.end), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 8.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    d = int64_float_of_bits(int64_or(int64_shift_left(int64_of_int(Reader_le32(bytes, kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)))).Result), 32), int64_of_int(Reader_le32(bytes, self.p).Result)));
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 8.0); }, kern::to<Float>(self.p)));
    return {d};
  }

  struct Reader_fixed32Result { Float Result = 0.0; };
  Reader_fixed32Result Reader_fixed32(mvt_Reader& self, const List<Int>& bytes) {
    Float f = 0.0;
    if (math_compare<">">(kern::formula([](Float end, Float a) -> Float { return ((end < 0.0) != (a < 0.0) ? (a < 0.0) : (end < a)); }, kern::to<Float>(self.end), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    f = math_float32_of_bits(Reader_le32(bytes, self.p).Result);
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)));
    return {f};
  }

  struct zigzagResult { Int64 Result = Int64{0x0}; };
  KERN_PURE zigzagResult zigzag(Int64 v = Int64{0x0}) {
    return {int64_xor(int64_shift_right_unsigned(v, 1), int64_sub(Int64{0x0}, int64_and(v, Int64{0x1})))};
  }

  struct read_valueResult { mvt_Value Result = mvt_Value{mvt_Value_Kind::none, "", 0.0, false}; };
  read_valueResult read_value(const List<Int>& bytes, mvt_Reader r = mvt_Reader{0, 0, true}) {
    mvt_Reader r_ = mvt_Reader{0, 0, true};
    mvt_Value v = mvt_Value{mvt_Value_Kind::none, "", 0.0, false};
    Int64 key = Int64{0x0};
    Int field = 0;
    Int wire = 0;
    Int selected = 0;
    r_ = r;
    v = mvt_Value{mvt_Value_Kind::none, "", 0.0, false};
    while (loop(Reader_more(r_).Result)) {
      n14 = Reader_varint(r_, bytes);
      key = n14.Result;
      field = int64_low(int64_shift_right_unsigned(key, 3));
      wire = int64_low(int64_and(key, Int64{0x7}));
      selected = field;
      if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
        v.kind = mvt_Value_Kind::string;
        n15 = Reader_text(r_, bytes);
        v.s = n15.Result;
      } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
        v.kind = mvt_Value_Kind::number;
        n16 = Reader_fixed32(r_, bytes);
        v.n = n16.Result;
      } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
        v.kind = mvt_Value_Kind::number;
        n17 = Reader_fixed64(r_, bytes);
        v.n = n17.Result;
      } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
        v.kind = mvt_Value_Kind::number;
        n18 = Reader_varint(r_, bytes);
        v.n = int64_to_float(n18.Result);
      } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
        v.kind = mvt_Value_Kind::number;
        n19 = Reader_varint(r_, bytes);
        v.n = int64_to_float_unsigned(n19.Result);
      } else if (math_compare<"==">(kern::to<Float>(selected), 6.0)) {
        v.kind = mvt_Value_Kind::number;
        n20 = Reader_varint(r_, bytes);
        v.n = int64_to_float(zigzag(n20.Result).Result);
      } else if (math_compare<"==">(kern::to<Float>(selected), 7.0)) {
        v.kind = mvt_Value_Kind::boolean;
        n21 = Reader_varint(r_, bytes);
        v.b = logic_not(int64_equal(n21.Result, Int64{0x0}));
      } else {
        Reader_skip(r_, bytes, wire);
      }
    }
    return {v};
  }

  void read_geometry(const List<Int>& bytes, List<Vector_row>& parts, mvt_Reader r = mvt_Reader{0, 0, true}, mvt_GeomType type = mvt_GeomType::unknown) {
    mvt_Reader r_ = mvt_Reader{0, 0, true};
    Int64 x = Int64{0x0};
    Int64 y = Int64{0x0};
    Bool open = false;
    Int64 c = Int64{0x0};
    Int id = 0;
    Int count = 0;
    Int i = 0;
    r_ = r;
    x = Int64{0x0};
    y = Int64{0x0};
    open = false;
    while (loop(Reader_more(r_).Result)) {
      n22 = Reader_varint(r_, bytes);
      c = n22.Result;
      id = int64_low(int64_and(c, Int64{0x7}));
      count = int64_low(int64_shift_right_unsigned(c, 3));
      if (logic_or(math_compare<"==">(kern::to<Float>(id), 1.0), math_compare<"==">(kern::to<Float>(id), 2.0))) {
        i = 0;
        while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(count)), Reader_more(r_).Result))) {
          n23 = Reader_varint(r_, bytes);
          x = int64_add(x, zigzag(n23.Result).Result);
          n24 = Reader_varint(r_, bytes);
          y = int64_add(y, zigzag(n24.Result).Result);
          if (logic_or(math_compare<"==">(kern::to<Float>(id), 1.0), logic_not(open))) {
            list_add(parts, Vector_row{List<Vector>{}});
            open = true;
          }
          list_add(parts[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(parts))))].items, vector_make(int64_to_float(x), int64_to_float(y), 0.0));
          i = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(i + 1.0); }, kern::to<Float>(i)));
        }
      } else if (math_compare<"==">(kern::to<Float>(id), 7.0)) {
      } else {
        return;
      }
    }
  }

  struct Layer_tagResult { Int Result = 0; };
  KERN_PURE Layer_tagResult Layer_tag(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Int_pair>(f.tags))))); it25.next();) {
      if (logic_and(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(f.tags[n25.index].first), kern::to<Float>(list_length<Text>(self.keys))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(f.tags[n25.index].second), kern::to<Float>(list_length<mvt_Value>(self.values))), 0.5)), text_equal(key, self.keys[f.tags[n25.index].first]))) {
        return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(f.tags[n25.index].second)))};
      }
    }
    return {-1};
  }

  struct Layer_textResult { Text Result = ""; };
  KERN_PURE Layer_textResult Layer_text(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    Int v = 0;
    v = Layer_tag(self, f, key).Result;
    return {logic_select<Text>(logic_and(math_compare<">=">(kern::to<Float>(v), 0.0), enum_equal<mvt_Value_Kind>(self.values[v].kind, mvt_Value_Kind::string)), self.values[v].s, "")};
  }

  struct Layer_numberResult { Float Result = 0.0; };
  KERN_PURE Layer_numberResult Layer_number(const mvt_Layer& self, const mvt_Feature& f, Text key = "", Float fallback = 0.0) {
    Int v = 0;
    v = Layer_tag(self, f, key).Result;
    return {kern::formula([](Float v, Float a, Float a_2, Float fallback) -> Float { return (((v >= 0.0) && (a != 0.0)) ? a_2 : fallback); }, kern::to<Float>(v), kern::to<Float>(enum_equal<mvt_Value_Kind>(self.values[v].kind, mvt_Value_Kind::number)), self.values[v].n, fallback)};
  }

  struct Tile_layerResult { Int Result = 0; };
  KERN_PURE Tile_layerResult Tile_layer(const mvt_Tile& self, Text name = "") {
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<mvt_Layer>(self.layers))))); it26.next();) {
      if (text_equal(name, self.layers[n26.index].name)) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n26.index)))};
      }
    }
    return {-1};
  }

  struct ring_areaResult { Float Result = 0.0; };
  KERN_PURE ring_areaResult ring_area(const List<Vector>& ring) {
    Float a = 0.0;
    Int n = 0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    a = 0.0;
    n = list_length<Vector>(ring);
    i = 0;
    j = kern::to<Int>(kern::formula([](Float n) -> Float { return ((n != 0.0) ? (n - 1.0) : 0.0); }, kern::to<Float>(n)));
    while (loop(math_compare<">">(kern::formula([](Float i, Float n) -> Float { return ((i < 0.0) != (n < 0.0) ? (n < 0.0) : (i < n)); }, kern::to<Float>(i), kern::to<Float>(n)), 0.5))) {
      a += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return ((x * y) - (x_2 * y_2)); }, vector_break(ring[j]).x, vector_break(ring[i]).y, vector_break(ring[i]).x, vector_break(ring[j]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula([](Float a) -> Float { return (a * 0.5); }, a)};
  }

  struct wanted_layerResult { Bool Result = false; };
  KERN_PURE wanted_layerResult wanted_layer(const Text& name, const List<Text>& wanted) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(wanted)), 0.0)) {
      return {true};
    }
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(wanted))))); it27.next();) {
      if (text_equal(wanted[n27.index], name)) {
        return {true};
      }
    }
    return {false};
  }

  struct open_layerResult { Bool Result = false; };
  /// Layer header (name, keys, values, extent) and where its features lie in the bytes.
  open_layerResult open_layer(const List<Int>& bytes, mvt_Layer& layer, List<mvt_Piece>& features, mvt_Reader lr = mvt_Reader{0, 0, true}) {
    mvt_Reader lr_ = mvt_Reader{0, 0, true};
    Int64 k = Int64{0x0};
    Int field = 0;
    Int wire = 0;
    Int selected = 0;
    mvt_Reader f = mvt_Reader{0, 0, true};
    lr_ = lr;
    while (loop(Reader_more(lr_).Result)) {
      n28 = Reader_varint(lr_, bytes);
      k = n28.Result;
      field = int64_low(int64_shift_right_unsigned(k, 3));
      wire = int64_low(int64_and(k, Int64{0x7}));
      selected = field;
      if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
        n29 = Reader_text(lr_, bytes);
        layer.name = n29.Result;
      } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
        n30 = Reader_sub(lr_, bytes);
        f = n30.Result;
        list_add(features, struct_make<mvt_Piece>(f.p, f.end));
      } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
        n31 = Reader_text(lr_, bytes);
        list_add(layer.keys, n31.Result);
      } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
        n32 = Reader_sub(lr_, bytes);
        n33 = read_value(bytes, n32.Result);
        list_add(layer.values, n33.Result);
      } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
        n34 = Reader_varint(lr_, bytes);
        layer.extent = int64_low(n34.Result);
      } else {
        Reader_skip(lr_, bytes, wire);
      }
    }
    return {lr_.ok};
  }

  struct read_featureResult { Bool Result = false; };
  read_featureResult read_feature(const List<Int>& bytes, mvt_Feature& f, mvt_Reader fr = mvt_Reader{0, 0, true}) {
    mvt_Reader fr_ = mvt_Reader{0, 0, true};
    mvt_Reader geometry = mvt_Reader{0, 0, true};
    Bool has_geometry = false;
    Int64 k = Int64{0x0};
    Int field = 0;
    Int wire = 0;
    mvt_Reader t = mvt_Reader{0, 0, true};
    Int a = 0;
    Int b = 0;
    fr_ = fr;
    geometry = mvt_Reader{0, 0, true};
    has_geometry = false;
    while (loop(Reader_more(fr_).Result)) {
      n35 = Reader_varint(fr_, bytes);
      k = n35.Result;
      field = int64_low(int64_shift_right_unsigned(k, 3));
      wire = int64_low(int64_and(k, Int64{0x7}));
      if (logic_and(math_compare<"==">(kern::to<Float>(field), 1.0), math_compare<"==">(kern::to<Float>(wire), 0.0))) {
        n36 = Reader_varint(fr_, bytes);
        f.id = n36.Result;
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 2.0), math_compare<"==">(kern::to<Float>(wire), 2.0))) {
        n37 = Reader_sub(fr_, bytes);
        t = n37.Result;
        while (loop(Reader_more(t).Result)) {
          n38 = Reader_varint(t, bytes);
          a = int64_low(n38.Result);
          n39 = Reader_varint(t, bytes);
          b = int64_low(n39.Result);
          list_add(f.tags, struct_make<Int_Int_pair>(a, b));
        }
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 3.0), math_compare<"==">(kern::to<Float>(wire), 0.0))) {
        n40 = Reader_varint(fr_, bytes);
        f.type = enum_from_index<mvt_GeomType>(int64_to_int(n40.Result)).value;
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 4.0), math_compare<"==">(kern::to<Float>(wire), 2.0))) {
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
      read_geometry(bytes, f.parts, geometry, f.type);
    }
    return {true};
  }

  void Decoder_begin(mvt_Decoder& self, const List<Int>& bytes, const List<Text>& wanted, Int at = 0) {
    mvt_Reader tile = mvt_Reader{0, 0, true};
    Bool skip = false;
    Int64 key = Int64{0x0};
    mvt_Reader lr = mvt_Reader{0, 0, true};
    Text name = "";
    mvt_Reader peek = mvt_Reader{0, 0, true};
    Bool stop = false;
    Int64 k = Int64{0x0};
    list_clear(self.layers_);
    list_clear(self.features_);
    list_clear(self.tile_.layers);
    self.feature_ = 0;
    self.layer_ = self.feature_;
    self.layer_open_ = false;
    self.done_ = false;
    self.failed_ = false;
    tile = struct_make<mvt_Reader>(kern::to<Int>(kern::formula([](Float at, Float a) -> Float { return ((at < a) ? at : a); }, kern::to<Float>(at), kern::to<Float>(list_length<Int>(bytes)))), list_length<Int>(bytes), true);
    while (loop(Reader_more(tile).Result)) {
      skip = false;
      n42 = Reader_varint(tile, bytes);
      key = n42.Result;
      if (logic_or(logic_not(int64_equal(int64_shift_right_unsigned(key, 3), Int64{0x3})), logic_not(int64_equal(int64_and(key, Int64{0x7}), Int64{0x2})))) {
        Reader_skip(tile, bytes, int64_low(int64_and(key, Int64{0x7})));
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
          if (logic_and(int64_equal(int64_shift_right_unsigned(k, 3), Int64{0x1}), int64_equal(int64_and(k, Int64{0x7}), Int64{0x2}))) {
            n45 = Reader_text(peek, bytes);
            name = n45.Result;
            stop = true;
          }
          if (logic_not(stop)) {
            Reader_skip(peek, bytes, int64_low(int64_and(k, Int64{0x7})));
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

  void put_varint(List<Int>& o, Int64 v = Int64{0x0}) {
    Int64 v_ = Int64{0x0};
    v_ = v;
    while (loop(logic_not(int64_less_unsigned(v_, Int64{0x80})))) {
      list_add(o, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(int64_or(v_, Int64{0x80}))))));
      v_ = int64_shift_right_unsigned(v_, 7);
    }
    list_add(o, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(v_)))));
  }

  void put_key(List<Int>& o, Int field = 0, Int wire = 0) {
    put_varint(o, int64_or(int64_shift_left(int64_of_int(field), 3), int64_of_int(wire)));
  }

  /// a length-delimited field: bytes (a packed field, a message) or a text (a name, a key, a value)
  void put_bytes(List<Int>& o, const List<Int>& data, Int field = 0) {
    Int place = 0;
    put_key(o, field, 2);
    put_varint(o, int64_of_int(list_length<Int>(data)));
    place = list_length<Int>(o);
    list_insert_list(o, place, data);
  }

  void put_bytes_2(List<Int>& o, Int field = 0, Text text = "") {
    put_key(o, field, 2);
    put_varint(o, int64_of_int(text_length(text)));
    list_insert_text(o, list_length<Int>(o), text);
  }

  struct zzResult { Int64 Result = Int64{0x0}; };
  KERN_PURE zzResult zz(Int64 v = Int64{0x0}) {
    return {int64_xor(int64_shift_left(v, 1), int64_shift_right(v, 63))};
  }

  void Writer_begin_layer(mvt_Writer& self, const Text& name, Int extent = 0) {
    self.layer_ = struct_make<mvt_Writer_PendingLayer>("", 4096, List<Text>{}, List<mvt_Value>{}, List<Int_row>{});
    self.layer_.name = name;
    self.layer_.extent = extent;
  }

  void Writer_feature(mvt_Writer& self, const List<Vector_row>& parts, const List<Text_mvt_Value_pair>& tags, mvt_GeomType type = mvt_GeomType::unknown, Int64 id = Int64{0x0}) {
    List<Int> f = List<Int>{};
    List<Int> t = List<Int>{};
    Int ki = 0;
    Int vi = 0;
    Bool stop = false;
    List<Int> g = List<Int>{};
    Int64 x = Int64{0x0};
    Int64 y = Int64{0x0};
    Bool skip = false;
    Int64 px = Int64{0x0};
    Int64 py = Int64{0x0};
    Int64 qx = Int64{0x0};
    Int64 qy = Int64{0x0};
    f = List<Int>{};
    if (logic_not(int64_equal(id, Int64{0x0}))) {
      put_key(f, 1, 0);
      put_varint(f, id);
    }
    t = List<Int>{};
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text_mvt_Value_pair>(tags))))); it46.next();) {
      ki = 0;
      vi = 0;
      while (loop(logic_and(math_compare<">">(kern::formula([](Float ki, Float a) -> Float { return ((ki < 0.0) != (a < 0.0) ? (a < 0.0) : (ki < a)); }, kern::to<Float>(ki), kern::to<Float>(list_length<Text>(self.layer_.keys))), 0.5), logic_not(text_equal(self.layer_.keys[ki], tags[n46.index].first))))) {
        ki = kern::to<Int>(kern::formula([](Float ki) -> Float { return kern::math::wrap32(ki + 1.0); }, kern::to<Float>(ki)));
      }
      if (math_compare<"==">(kern::to<Float>(ki), kern::to<Float>(list_length<Text>(self.layer_.keys)))) {
        list_add(self.layer_.keys, tags[n46.index].first);
      }
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float vi, Float a) -> Float { return ((vi < 0.0) != (a < 0.0) ? (a < 0.0) : (vi < a)); }, kern::to<Float>(vi), kern::to<Float>(list_length<mvt_Value>(self.layer_.values))), 0.5)))) {
        if (logic_and(logic_and(logic_and(enum_equal<mvt_Value_Kind>(self.layer_.values[vi].kind, tags[n46.index].second.kind), text_equal(self.layer_.values[vi].s, tags[n46.index].second.s)), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a == a_2); }, self.layer_.values[vi].n, tags[n46.index].second.n), 0.5)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(self.layer_.values[vi].b))), kern::to<Float>(value_int(kern::to<Int>(tags[n46.index].second.b)))))) {
          stop = true;
        }
        if (logic_not(stop)) {
          vi = kern::to<Int>(kern::formula([](Float vi) -> Float { return kern::math::wrap32(vi + 1.0); }, kern::to<Float>(vi)));
        }
      }
      if (math_compare<"==">(kern::to<Float>(vi), kern::to<Float>(list_length<mvt_Value>(self.layer_.values)))) {
        list_add(self.layer_.values, tags[n46.index].second);
      }
      put_varint(t, int64_of_int(ki));
      put_varint(t, int64_of_int(vi));
    }
    put_bytes(f, t, 2);
    put_key(f, 3, 0);
    put_varint(f, int64_of_int(enum_index<mvt_GeomType>(type)));
    g = List<Int>{};
    x = Int64{0x0};
    y = Int64{0x0};
    for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(parts))))); it47.next();) {
      skip = false;
      if (math_compare<"==">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n47.index]).items)), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        put_varint(g, Int64{0x9});
        px = int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(parts[n47.index].items[0]).x)));
        py = int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(parts[n47.index].items[0]).y)));
        put_varint(g, zz(int64_sub(px, x)).Result);
        put_varint(g, zz(int64_sub(py, y)).Result);
        x = px;
        y = py;
        if (math_compare<">">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n47.index]).items)), 1.0)) {
          put_varint(g, int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n47.index]).items))))), 3), Int64{0x2}));
          for (auto it48 = flow_for(n48, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n47.index]).items))))); it48.next();) {
            qx = int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(parts[n47.index].items[n48.index]).x)));
            qy = int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(parts[n47.index].items[n48.index]).y)));
            put_varint(g, zz(int64_sub(qx, x)).Result);
            put_varint(g, zz(int64_sub(qy, y)).Result);
            x = qx;
            y = qy;
          }
        }
        if (enum_equal<mvt_GeomType>(type, mvt_GeomType::polygon)) {
          put_varint(g, Int64{0xf});
        }
      }
    }
    put_bytes(f, g, 4);
    list_add(self.layer_.features, struct_make<Int_row>(f));
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
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
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
  struct : kern::Node<46> { Int index = 0; } n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
};
