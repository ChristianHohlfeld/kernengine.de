struct wire : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(wire_F32) { f32 v = 0.0f; };
  KERN_STRUCT(wire_F64) { f64 v = 0.0; };
  KERN_STRUCT(wire_I16) { i16 v = 0; };
  KERN_STRUCT(wire_I32) { i32 v = 0; };
  KERN_STRUCT(wire_I64) { i64 v = 0; };
  KERN_STRUCT(wire_I8) { i8 v = 0; };
  KERN_STRUCT(wire_Q4) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 0.0f; };
  KERN_STRUCT(wire_Reader) { u64 at = 0; bool ok = true; };
  KERN_STRUCT(wire_U16) { u16 v = 0; };
  KERN_STRUCT(wire_U32) { u32 v = 0; };
  KERN_STRUCT(wire_U64) { u64 v = 0; };
  KERN_STRUCT(wire_V3) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; };

  // ---- functions ----
  struct f32_bytesResult { Fixed<u8, 4> Result = Fixed<u8, 4>{}; };
  /// The four bytes of a 32-bit float, the low one first.
  KERN_PURE f32_bytesResult f32_bytes(f32 v = 0.0f) {
    Fixed<u8, 4> bytes = Fixed<u8, 4>{};
    bytes = struct_bytes<wire_F32, "4: f32@0">(struct_make<wire_F32>(v));
    return {bytes};
  }

  /// ---- writing: each number's bytes go on the end of the message (one growth of the list each, as inserting them in one go does)
  void put_u8(List<u8>& b, u8 v = 0) {
    list_add(b, v);
  }

  void put_i8(List<u8>& b, i8 v = 0) {
    Fixed<u8, 1> x = Fixed<u8, 1>{};
    Fixed<u8, 1> bytes = Fixed<u8, 1>{};
    i64 place = 0;
    bytes = struct_bytes<wire_I8, "1: i8@0">(struct_make<wire_I8>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_u16(List<u8>& b, u16 v = 0) {
    Fixed<u8, 2> x = Fixed<u8, 2>{};
    Fixed<u8, 2> bytes = Fixed<u8, 2>{};
    i64 place = 0;
    bytes = struct_bytes<wire_U16, "2: u16@0">(struct_make<wire_U16>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_i16(List<u8>& b, i16 v = 0) {
    Fixed<u8, 2> x = Fixed<u8, 2>{};
    Fixed<u8, 2> bytes = Fixed<u8, 2>{};
    i64 place = 0;
    bytes = struct_bytes<wire_I16, "2: i16@0">(struct_make<wire_I16>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_u32(List<u8>& b, u32 v = 0) {
    Fixed<u8, 4> x = Fixed<u8, 4>{};
    Fixed<u8, 4> bytes = Fixed<u8, 4>{};
    i64 place = 0;
    bytes = struct_bytes<wire_U32, "4: u32@0">(struct_make<wire_U32>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_i32(List<u8>& b, i32 v = 0) {
    Fixed<u8, 4> x = Fixed<u8, 4>{};
    Fixed<u8, 4> bytes = Fixed<u8, 4>{};
    i64 place = 0;
    bytes = struct_bytes<wire_I32, "4: i32@0">(struct_make<wire_I32>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_u64(List<u8>& b, u64 v = 0) {
    Fixed<u8, 8> x = Fixed<u8, 8>{};
    Fixed<u8, 8> bytes = Fixed<u8, 8>{};
    i64 place = 0;
    bytes = struct_bytes<wire_U64, "8: u64@0">(struct_make<wire_U64>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_i64(List<u8>& b, i64 v = 0) {
    Fixed<u8, 8> x = Fixed<u8, 8>{};
    Fixed<u8, 8> bytes = Fixed<u8, 8>{};
    i64 place = 0;
    bytes = struct_bytes<wire_I64, "8: i64@0">(struct_make<wire_I64>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_f32(List<u8>& b, f32 v = 0.0f) {
    Fixed<u8, 4> x = Fixed<u8, 4>{};
    i64 place = 0;
    x = f32_bytes(v).Result;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_f64(List<u8>& b, f64 v = 0.0) {
    Fixed<u8, 8> x = Fixed<u8, 8>{};
    Fixed<u8, 8> bytes = Fixed<u8, 8>{};
    i64 place = 0;
    bytes = struct_bytes<wire_F64, "8: f64@0">(struct_make<wire_F64>(v));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_vec3(List<u8>& b, Vector v = Vector{0.0, 0.0, 0.0}) {
    Fixed<u8, 12> x = Fixed<u8, 12>{};
    Fixed<u8, 12> bytes = Fixed<u8, 12>{};
    i64 place = 0;
    bytes = struct_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(struct_make<wire_V3>(vector_break(v).x, vector_break(v).y, vector_break(v).z));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  void put_quat(List<u8>& b, Quat q = Quat{0.0f, 0.0f, 0.0f, 1.0f}) {
    Fixed<u8, 16> x = Fixed<u8, 16>{};
    Fixed<u8, 16> bytes = Fixed<u8, 16>{};
    i64 place = 0;
    bytes = struct_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(struct_make<wire_Q4>(q.x, q.y, q.z, q.w));
    x = bytes;
    place = list_length<u8>(b);
    list_insert_list(b, place, x);
  }

  /// A list of bytes: its length (u16), then the bytes.
  void put_bytes(List<u8>& b, const List<u8>& v) {
    i64 place = 0;
    put_u16(b, num_cast<u16, i64>(list_length<u8>(v)));
    place = list_length<u8>(b);
    list_insert_list(b, place, v);
  }

  struct Reader_get_u8Result { u8 Result = 0; };
  Reader_get_u8Result Reader_get_u8(wire_Reader& self, const List<u8>& b) {
    u8 v = 0;
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 1); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    v = b[num_cast<i64, u64>(self.at)];
    self.at = kern::formula<u64>([](auto at) { return (at + 1); }, self.at);
    return {v};
  }

  struct Reader_get_i8Result { i8 Result = 0; };
  Reader_get_i8Result Reader_get_i8(wire_Reader& self, const List<u8>& b) {
    wire_I8 x = wire_I8{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 1); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_I8{0};
    x = struct_of_bytes<wire_I8, "1: i8@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 1); }, self.at);
    return {x.v};
  }

  struct Reader_get_u16Result { u16 Result = 0; };
  Reader_get_u16Result Reader_get_u16(wire_Reader& self, const List<u8>& b) {
    wire_U16 x = wire_U16{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 2); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_U16{0};
    x = struct_of_bytes<wire_U16, "2: u16@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 2); }, self.at);
    return {x.v};
  }

  struct Reader_get_i16Result { i16 Result = 0; };
  Reader_get_i16Result Reader_get_i16(wire_Reader& self, const List<u8>& b) {
    wire_I16 x = wire_I16{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 2); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_I16{0};
    x = struct_of_bytes<wire_I16, "2: i16@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 2); }, self.at);
    return {x.v};
  }

  struct Reader_get_u32Result { u32 Result = 0; };
  Reader_get_u32Result Reader_get_u32(wire_Reader& self, const List<u8>& b) {
    wire_U32 x = wire_U32{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 4); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_U32{0};
    x = struct_of_bytes<wire_U32, "4: u32@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 4); }, self.at);
    return {x.v};
  }

  struct Reader_get_i32Result { i32 Result = 0; };
  Reader_get_i32Result Reader_get_i32(wire_Reader& self, const List<u8>& b) {
    wire_I32 x = wire_I32{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 4); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_I32{0};
    x = struct_of_bytes<wire_I32, "4: i32@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 4); }, self.at);
    return {x.v};
  }

  struct Reader_get_u64Result { u64 Result = 0; };
  Reader_get_u64Result Reader_get_u64(wire_Reader& self, const List<u8>& b) {
    wire_U64 x = wire_U64{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 8); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_U64{0};
    x = struct_of_bytes<wire_U64, "8: u64@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 8); }, self.at);
    return {x.v};
  }

  struct Reader_get_i64Result { i64 Result = 0; };
  Reader_get_i64Result Reader_get_i64(wire_Reader& self, const List<u8>& b) {
    wire_I64 x = wire_I64{0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 8); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0};
    }
    x = wire_I64{0};
    x = struct_of_bytes<wire_I64, "8: i64@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 8); }, self.at);
    return {x.v};
  }

  struct Reader_get_f32Result { f32 Result = 0.0f; };
  Reader_get_f32Result Reader_get_f32(wire_Reader& self, const List<u8>& b) {
    wire_F32 x = wire_F32{0.0f};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 4); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0.0f};
    }
    x = wire_F32{0.0f};
    x = struct_of_bytes<wire_F32, "4: f32@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 4); }, self.at);
    return {x.v};
  }

  struct Reader_get_f64Result { f64 Result = 0.0; };
  Reader_get_f64Result Reader_get_f64(wire_Reader& self, const List<u8>& b) {
    wire_F64 x = wire_F64{0.0};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 8); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F64{0.0};
    x = struct_of_bytes<wire_F64, "8: f64@0">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 8); }, self.at);
    return {x.v};
  }

  struct Reader_get_vec3Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  Reader_get_vec3Result Reader_get_vec3(wire_Reader& self, const List<u8>& b) {
    wire_V3 x = wire_V3{0.0f, 0.0f, 0.0f};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 12); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {Vector{0.0, 0.0, 0.0}};
    }
    x = wire_V3{0.0f, 0.0f, 0.0f};
    x = struct_of_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 12); }, self.at);
    return {vector_make(x.x, x.y, x.z)};
  }

  struct Reader_get_quatResult { Quat Result = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  Reader_get_quatResult Reader_get_quat(wire_Reader& self, const List<u8>& b) {
    wire_Q4 x = wire_Q4{0.0f, 0.0f, 0.0f, 0.0f};
    if (math_compare<">", u64>(kern::formula<u64>([](auto at) { return (at + 16); }, self.at), num_cast<u64, i64>(list_length<u8>(b)))) {
      self.ok = false;
      return {struct_make<Quat>(0.0f, 0.0f, 0.0f, 1.0f)};
    }
    x = wire_Q4{0.0f, 0.0f, 0.0f, 0.0f};
    x = struct_of_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(b, num_cast<i64, u64>(self.at));
    self.at = kern::formula<u64>([](auto at) { return (at + 16); }, self.at);
    return {struct_make<Quat>(x.x, x.y, x.z, x.w)};
  }

  struct Reader_get_bytesResult { List<u8> Result = List<u8>{}; };
  /// A list of bytes (put_bytes): empty where it is not all there.
  Reader_get_bytesResult Reader_get_bytes(wire_Reader& self, const List<u8>& b) {
    u16 n = 0;
    List<u8> v = List<u8>{};
    List<u8> made = List<u8>{};
    i64 first = 0;
    i64 last = 0;
    n10 = Reader_get_u16(self, b);
    n = n10.Result;
    if (logic_or(logic_not(self.ok), math_compare<">", u64>(kern::formula<u64, u16>([](auto at, auto n) { return (at + num_cast<u64, u16>(n)); }, self.at, n), num_cast<u64, i64>(list_length<u8>(b))))) {
      self.ok = false;
      return {List<u8>{}};
    }
    made = List<u8>{};
    first = num_cast<i64, u64>(self.at);
    last = math_sub<i64>(kern::formula<u64, u16>([](auto at, auto n) { return num_cast<i64, u64>((at + num_cast<u64, u16>(n))); }, self.at, n), 1);
    list_reserve(made, math_add<i64>(math_sub<i64>(last, first), 1));
    for (auto it11 = flow_for(n11, first, last); it11.next();) {
      list_add(made, list_get<u8>(b, n11.index));
    }
    v = made;
    self.at = kern::formula<u64, u16>([](auto at, auto n) { return (at + num_cast<u64, u16>(n)); }, self.at, n);
    return {v};
  }

  struct Reader_doneResult { bool Result = false; };
  /// Every number was there, and the message has no more.
  KERN_PURE Reader_doneResult Reader_done(const wire_Reader& self, const List<u8>& b) {
    return {logic_and(self.ok, math_compare<"==", u64>(self.at, num_cast<u64, i64>(list_length<u8>(b))))};
  }

  // ---- what nodes give and keep ----
  Reader_get_u16Result n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
};
