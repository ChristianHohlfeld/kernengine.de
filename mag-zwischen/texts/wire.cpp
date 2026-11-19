struct wire : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(wire_F32) { Float v = 0.0; };
  KERN_STRUCT(wire_F64) { Float v = 0.0; };
  KERN_STRUCT(wire_I16) { Int v = 0; };
  KERN_STRUCT(wire_I32) { Int v = 0; };
  KERN_STRUCT(wire_I64) { Int64 v = Int64{0x0}; };
  KERN_STRUCT(wire_I8) { Int v = 0; };
  KERN_STRUCT(wire_Q4) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 0.0; };
  KERN_STRUCT(wire_Reader) { Int at = 0; Bool ok = true; };
  KERN_STRUCT(wire_U16) { Int v = 0; };
  KERN_STRUCT(wire_U32) { Int v = 0; };
  KERN_STRUCT(wire_U64) { Int64 v = Int64{0x0}; };
  KERN_STRUCT(wire_V3) { Float x = 0.0; Float y = 0.0; Float z = 0.0; };

  // ---- functions ----
  struct f32_bytesResult { Fixed<Int, 4> Result = Fixed<Int, 4>{}; };
  /// The four bytes of a 32-bit float, the low one first.
  KERN_PURE f32_bytesResult f32_bytes(Float v = 0.0) {
    Fixed<Int, 4> bytes = Fixed<Int, 4>{};
    bytes = struct_bytes<wire_F32, "4: f32@0">(struct_make<wire_F32>(v));
    return {bytes};
  }

  /// ---- writing: each number's bytes go on the end of the message (one growth of the list each, as inserting them in one go does)
  void put_u8(List<Int>& b, Int v = 0) {
    list_add(b, v);
  }

  void put_i8(List<Int>& b, Int v = 0) {
    Fixed<Int, 1> x = Fixed<Int, 1>{};
    Fixed<Int, 1> bytes = Fixed<Int, 1>{};
    Int place = 0;
    bytes = struct_bytes<wire_I8, "1: i8@0">(struct_make<wire_I8>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_u16(List<Int>& b, Int v = 0) {
    Fixed<Int, 2> x = Fixed<Int, 2>{};
    Fixed<Int, 2> bytes = Fixed<Int, 2>{};
    Int place = 0;
    bytes = struct_bytes<wire_U16, "2: u16@0">(struct_make<wire_U16>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_i16(List<Int>& b, Int v = 0) {
    Fixed<Int, 2> x = Fixed<Int, 2>{};
    Fixed<Int, 2> bytes = Fixed<Int, 2>{};
    Int place = 0;
    bytes = struct_bytes<wire_I16, "2: i16@0">(struct_make<wire_I16>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_u32(List<Int>& b, Int v = 0) {
    Fixed<Int, 4> x = Fixed<Int, 4>{};
    Fixed<Int, 4> bytes = Fixed<Int, 4>{};
    Int place = 0;
    bytes = struct_bytes<wire_U32, "4: u32@0">(struct_make<wire_U32>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_i32(List<Int>& b, Int v = 0) {
    Fixed<Int, 4> x = Fixed<Int, 4>{};
    Fixed<Int, 4> bytes = Fixed<Int, 4>{};
    Int place = 0;
    bytes = struct_bytes<wire_I32, "4: i32@0">(struct_make<wire_I32>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_u64(List<Int>& b, Int64 v = Int64{0x0}) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_U64, "8: u64@0">(struct_make<wire_U64>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_i64(List<Int>& b, Int64 v = Int64{0x0}) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_I64, "8: i64@0">(struct_make<wire_I64>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_f32(List<Int>& b, Float v = 0.0) {
    Fixed<Int, 4> x = Fixed<Int, 4>{};
    Int place = 0;
    x = f32_bytes(v).Result;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_f64(List<Int>& b, Float v = 0.0) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_F64, "8: f64@0">(struct_make<wire_F64>(v));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_vec3(List<Int>& b, Vector v = Vector{0.0, 0.0, 0.0}) {
    Fixed<Int, 12> x = Fixed<Int, 12>{};
    Fixed<Int, 12> bytes = Fixed<Int, 12>{};
    Int place = 0;
    bytes = struct_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(struct_make<wire_V3>(vector_break(v).x, vector_break(v).y, vector_break(v).z));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  void put_quat(List<Int>& b, Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    Fixed<Int, 16> x = Fixed<Int, 16>{};
    Fixed<Int, 16> bytes = Fixed<Int, 16>{};
    Int place = 0;
    bytes = struct_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(struct_make<wire_Q4>(q.x, q.y, q.z, q.w));
    x = bytes;
    place = list_length<Int>(b);
    list_insert_list(b, place, x);
  }

  /// A list of bytes: its length (u16), then the bytes.
  void put_bytes(List<Int>& b, const List<Int>& v) {
    Int place = 0;
    put_u16(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 65535.0); }, kern::to<Float>(list_length<Int>(v)))));
    place = list_length<Int>(b);
    list_insert_list(b, place, v);
  }

  struct Reader_get_u8Result { Int Result = 0; };
  Reader_get_u8Result Reader_get_u8(wire_Reader& self, const List<Int>& b) {
    Int v = 0;
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    v = b[self.at];
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(self.at)));
    return {v};
  }

  struct Reader_get_i8Result { Int Result = 0; };
  Reader_get_i8Result Reader_get_i8(wire_Reader& self, const List<Int>& b) {
    wire_I8 x = wire_I8{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_I8{0};
    x = struct_of_bytes<wire_I8, "1: i8@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u16Result { Int Result = 0; };
  Reader_get_u16Result Reader_get_u16(wire_Reader& self, const List<Int>& b) {
    wire_U16 x = wire_U16{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_U16{0};
    x = struct_of_bytes<wire_U16, "2: u16@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_i16Result { Int Result = 0; };
  Reader_get_i16Result Reader_get_i16(wire_Reader& self, const List<Int>& b) {
    wire_I16 x = wire_I16{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_I16{0};
    x = struct_of_bytes<wire_I16, "2: i16@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u32Result { Int Result = 0; };
  Reader_get_u32Result Reader_get_u32(wire_Reader& self, const List<Int>& b) {
    wire_U32 x = wire_U32{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_U32{0};
    x = struct_of_bytes<wire_U32, "4: u32@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_i32Result { Int Result = 0; };
  Reader_get_i32Result Reader_get_i32(wire_Reader& self, const List<Int>& b) {
    wire_I32 x = wire_I32{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_I32{0};
    x = struct_of_bytes<wire_I32, "4: i32@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u64Result { Int64 Result = Int64{0x0}; };
  Reader_get_u64Result Reader_get_u64(wire_Reader& self, const List<Int>& b) {
    wire_U64 x = wire_U64{Int64{0x0}};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Int64{0x0}};
    }
    x = wire_U64{Int64{0x0}};
    x = struct_of_bytes<wire_U64, "8: u64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_i64Result { Int64 Result = Int64{0x0}; };
  Reader_get_i64Result Reader_get_i64(wire_Reader& self, const List<Int>& b) {
    wire_I64 x = wire_I64{Int64{0x0}};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Int64{0x0}};
    }
    x = wire_I64{Int64{0x0}};
    x = struct_of_bytes<wire_I64, "8: i64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_f32Result { Float Result = 0.0; };
  Reader_get_f32Result Reader_get_f32(wire_Reader& self, const List<Int>& b) {
    wire_F32 x = wire_F32{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F32{0.0};
    x = struct_of_bytes<wire_F32, "4: f32@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 4.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_f64Result { Float Result = 0.0; };
  Reader_get_f64Result Reader_get_f64(wire_Reader& self, const List<Int>& b) {
    wire_F64 x = wire_F64{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F64{0.0};
    x = struct_of_bytes<wire_F64, "8: f64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 8.0); }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_vec3Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  Reader_get_vec3Result Reader_get_vec3(wire_Reader& self, const List<Int>& b) {
    wire_V3 x = wire_V3{0.0, 0.0, 0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 12.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Vector{0.0, 0.0, 0.0}};
    }
    x = wire_V3{0.0, 0.0, 0.0};
    x = struct_of_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 12.0); }, kern::to<Float>(self.at)));
    return {vector_make(x.x, x.y, x.z)};
  }

  struct Reader_get_quatResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  Reader_get_quatResult Reader_get_quat(wire_Reader& self, const List<Int>& b) {
    wire_Q4 x = wire_Q4{0.0, 0.0, 0.0, 0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 16.0); }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {struct_make<Quat>(0.0, 0.0, 0.0, 1.0)};
    }
    x = wire_Q4{0.0, 0.0, 0.0, 0.0};
    x = struct_of_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 16.0); }, kern::to<Float>(self.at)));
    return {struct_make<Quat>(x.x, x.y, x.z, x.w)};
  }

  struct Reader_get_bytesResult { List<Int> Result = List<Int>{}; };
  /// A list of bytes (put_bytes): empty where it is not all there.
  Reader_get_bytesResult Reader_get_bytes(wire_Reader& self, const List<Int>& b) {
    Int n = 0;
    List<Int> v = List<Int>{};
    List<Int> made = List<Int>{};
    Int first = 0;
    Int last = 0;
    n10 = Reader_get_u16(self, b);
    n = n10.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return (at + n); }, kern::to<Float>(self.at), kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return {List<Int>{}};
    }
    made = List<Int>{};
    first = self.at;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return (at + n); }, kern::to<Float>(self.at), kern::to<Float>(n)))))));
    list_reserve(made, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
    for (auto it11 = flow_for(n11, first, last); it11.next();) {
      list_add(made, list_get<Int>(b, n11.index));
    }
    v = made;
    self.at = kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return (at + n); }, kern::to<Float>(self.at), kern::to<Float>(n)));
    return {v};
  }

  struct Reader_doneResult { Bool Result = false; };
  /// Every number was there, and the message has no more.
  KERN_PURE Reader_doneResult Reader_done(const wire_Reader& self, const List<Int>& b) {
    return {logic_and(self.ok, math_compare<"==">(kern::to<Float>(self.at), kern::to<Float>(list_length<Int>(b))))};
  }

  // ---- what nodes give and keep ----
  Reader_get_u16Result n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
