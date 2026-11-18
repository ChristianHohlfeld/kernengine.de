struct Arondight45_HardwareSensors : virtual kern::Library, virtual Arondight45_DroneFC_Core, virtual Arondight45_StateControl {
  // ---- types ----
  KERN_STRUCT(hwcontract_NavigationWireFrame) { Int magic = 827736398; Int version = 1; Int sequence = 0; Int vx_cms = 0; Int vy_cms = 0; Int vz_cms = 0; Int agl_mm = 0; Int flags = 0; Int crc16 = 0; };
  KERN_STRUCT(hwcontract_NavigationWireParser) { Fixed<Int, 20> buffer_ = List<Int>{}; Int size_ = 0; };
  KERN_STRUCT(hwcontract_SbusWireParser) { Fixed<Int, 25> buffer_ = List<Int>{}; Int size_ = 0; Int64 last_us_ = Int64{0x0}; };

  // ---- variables ----
  /// "NAV1" little-endian.
  KERN_CONST Int kNavigationMagic = 827736398;
  KERN_CONST Int kNavigationVersion = 1;
  KERN_CONST Int kNavigationSplitValidity = 32768;
  KERN_CONST Int kNavigationVelocityValid = 1;
  KERN_CONST Int kNavigationAglValid = 2;
  /// Legacy v1 senders used bit 0 to mean the whole NAV solution was valid.
  KERN_CONST Int kNavigationValid = 1;
  KERN_CONST Int kNavigationHeadingValid = 4;
  /// 12 bits, 0.1 degree/code.
  KERN_CONST Int kNavigationHeadingMask = 32760;
  KERN_CONST Int kNavigationHeadingShift = 3;

  // ---- functions ----
  struct crc16_ccittResult { Int Result = 0; };
  KERN_PURE crc16_ccittResult crc16_ccitt(const Fixed<Int, 20>& data, Int data_at = 0, Int length = 0) {
    Int bytes_at = 0;
    Int crc = 0;
    bytes_at = data_at;
    crc = 65535;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(length)))); it10.next();) {
      crc = kern::to<Int>(kern::formula([](Float crc, Float a) -> Float { return kern::math::bxor(crc, kern::math::wrap32s(kern::math::shl(a, 8.0))); }, kern::to<Float>(crc), kern::to<Float>(data[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(bytes_at), kern::to<Float>(n10.index)))])));
      for (auto it11 = flow_for(n11, 0, 7); it11.next();) {
        crc = kern::to<Int>(kern::formula([](Float crc) -> Float { return kern::math::band(kern::math::bxor(kern::math::wrap32(kern::math::wrap32s(kern::math::shl(crc, 1.0))), ((kern::math::band(crc, 32768.0) != 0.0) ? 4129.0 : 0.0)), 65535.0); }, kern::to<Float>(crc)));
      }
    }
    return {crc};
  }

  struct read_be_i16Result { Int Result = 0; };
  KERN_PURE read_be_i16Result read_be_i16(const Fixed<Int, 14>& p, Int p_at = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::bxor(kern::math::band(kern::math::bor(kern::math::wrap32s(kern::math::shl(a, 8.0)), a_2), 65535.0), 32768.0) - 32768.0); }, kern::to<Float>(p[p_at]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(p_at)))])))};
  }

  struct orient_targetResult { fc_V3 Result = fc_V3{0.0, 0.0, 0.0}; };
  KERN_PURE orient_targetResult orient_target(fc_V3 value = fc_V3{0.0, 0.0, 0.0}) {
    fc_V3 out = fc_V3{0.0, 0.0, 0.0};
    out = struct_make<fc_V3>(0.0, 0.0, 0.0);
    out = value;
    return {out};
  }

  struct decode_icm42688_registersResult { Bool Result = false; };
  /// This is the one ICM-42688-P register decoder used by production, physical HIL and browser SIL. The simulator emits the same 14 register bytes that SPI reads.
  decode_icm42688_registersResult decode_icm42688_registers(const Fixed<Int, 14>& raw, fc_Imu& out, Int raw_at = 0) {
    Int bad = 0;
    Int ax = 0;
    Int ay = 0;
    Int az = 0;
    Int gx = 0;
    Int gy = 0;
    Int gz = 0;
    if (logic_not(math_compare<">=">(kern::to<Float>(raw_at), 0.0))) {
      return {false};
    }
    bad = -32768;
    ax = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 2.0; }, kern::to<Float>(raw_at)))).Result;
    ay = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 4.0; }, kern::to<Float>(raw_at)))).Result;
    az = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 6.0; }, kern::to<Float>(raw_at)))).Result;
    gx = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 8.0; }, kern::to<Float>(raw_at)))).Result;
    gy = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 10.0; }, kern::to<Float>(raw_at)))).Result;
    gz = read_be_i16(raw, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 12.0; }, kern::to<Float>(raw_at)))).Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_or(math_compare<"==">(kern::to<Float>(ax), kern::to<Float>(bad)), math_compare<"==">(kern::to<Float>(ay), kern::to<Float>(bad))), math_compare<"==">(kern::to<Float>(az), kern::to<Float>(bad))), math_compare<"==">(kern::to<Float>(gx), kern::to<Float>(bad))), math_compare<"==">(kern::to<Float>(gy), kern::to<Float>(bad))), math_compare<"==">(kern::to<Float>(gz), kern::to<Float>(bad)))) {
      return {false};
    }
    out.a = orient_target(struct_make<fc_V3>(kern::formula([](Float ax) -> Float { return kern::math::div(ax, 2048.0); }, kern::to<Float>(ax)), kern::formula([](Float ay) -> Float { return kern::math::div(ay, 2048.0); }, kern::to<Float>(ay)), kern::formula([](Float az) -> Float { return kern::math::div(az, 2048.0); }, kern::to<Float>(az)))).Result;
    out.g = orient_target(struct_make<fc_V3>(kern::formula([](Float gx) -> Float { return kern::math::div(gx, 16.4); }, kern::to<Float>(gx)), kern::formula([](Float gy) -> Float { return kern::math::div(gy, 16.4); }, kern::to<Float>(gy)), kern::formula([](Float gz) -> Float { return kern::math::div(gz, 16.4); }, kern::to<Float>(gz)))).Result;
    return {logic_and(Arondight45_DroneFC_Core::finite(out.a).Result, Arondight45_DroneFC_Core::finite(out.g).Result)};
  }

  struct decode_navigation_wireResult { Bool Result = false; };
  decode_navigation_wireResult decode_navigation_wire(const hwcontract_NavigationWireFrame& frame, fc_NavigationState& out) {
    Fixed<Int, 20> bytes = Fixed<Int, 20>{};
    Bool split = false;
    Bool legacy_valid = false;
    Int code = 0;
    if (logic_or(math_compare<"!=">(kern::to<Float>(frame.magic), kern::to<Float>(kNavigationMagic)), math_compare<"!=">(kern::to<Float>(frame.version), kern::to<Float>(kNavigationVersion)))) {
      return {false};
    }
    bytes = struct_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(frame);
    if (math_compare<"!=">(kern::to<Float>(crc16_ccitt(bytes, 0, 18).Result), kern::to<Float>(frame.crc16))) {
      return {false};
    }
    out.velocity_world_mps = struct_make<fc_V3>(kern::formula([](Float vx_cms) -> Float { return (vx_cms * 0.01); }, kern::to<Float>(frame.vx_cms)), kern::formula([](Float vy_cms) -> Float { return (vy_cms * 0.01); }, kern::to<Float>(frame.vy_cms)), kern::formula([](Float vz_cms) -> Float { return (vz_cms * 0.01); }, kern::to<Float>(frame.vz_cms)));
    out.agl_m = kern::formula([](Float agl_mm) -> Float { return (agl_mm * 0.001); }, kern::to<Float>(frame.agl_mm));
    split = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float flags, Float kNavigationSplitValidity) -> Float { return kern::math::band(flags, kNavigationSplitValidity); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationSplitValidity))))), 0.0);
    if (split) {
      out.velocity_valid = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float flags, Float kNavigationVelocityValid) -> Float { return kern::math::band(flags, kNavigationVelocityValid); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationVelocityValid))))), 0.0);
      out.agl_valid = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float flags, Float kNavigationAglValid) -> Float { return kern::math::band(flags, kNavigationAglValid); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationAglValid))))), 0.0);
    } else {
      legacy_valid = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float flags, Float kNavigationValid) -> Float { return kern::math::band(flags, kNavigationValid); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationValid))))), 0.0);
      out.velocity_valid = legacy_valid;
      out.agl_valid = legacy_valid;
    }
    out.heading_valid = logic_and(split, math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float flags, Float kNavigationHeadingValid) -> Float { return kern::math::band(flags, kNavigationHeadingValid); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationHeadingValid))))), 0.0));
    out.heading_deg = 0.0;
    if (out.heading_valid) {
      code = kern::to<Int>(kern::formula([](Float flags, Float kNavigationHeadingMask, Float kNavigationHeadingShift) -> Float { return kern::math::band(kern::math::shr(kern::math::band(flags, kNavigationHeadingMask), kNavigationHeadingShift), 65535.0); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationHeadingMask), kern::to<Float>(kNavigationHeadingShift)));
      if (math_compare<">=">(kern::to<Float>(code), 3600.0)) {
        return {false};
      }
      out.heading_deg = kern::formula([](Float code) -> Float { return (code * 0.1); }, kern::to<Float>(code));
      if (math_compare<">=">(out.heading_deg, 180.0)) {
        out.heading_deg = kern::formula([](Float heading_deg) -> Float { return (heading_deg - 360.0); }, out.heading_deg);
      }
    }
    out.valid = logic_and(out.velocity_valid, out.agl_valid);
    return {logic_and(logic_and(logic_or(logic_not(out.velocity_valid), Arondight45_DroneFC_Core::finite(out.velocity_world_mps).Result), logic_or(logic_not(out.agl_valid), logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, out.agl_m), 0.0), math_compare<">=">(out.agl_m, 0.0)))), logic_or(logic_not(out.heading_valid), Arondight45_StateControl::navigation_heading_valid(out).Result))};
  }

  struct encode_navigation_wire_s16Result { Int Result = 0; };
  KERN_PURE encode_navigation_wire_s16Result encode_navigation_wire_s16(Float value = 0.0) {
    Int rounded = 0;
    rounded = kern::to<Int>(kern::formula([](Float value) -> Float { return kern::math::round((value * 100.0)); }, value));
    return {kern::to<Int>(kern::formula([](Float rounded) -> Float { return (kern::math::bxor(kern::math::band((rounded < (-32767.0) ? (-32767.0) : (32767.0 < rounded ? 32767.0 : rounded)), 65535.0), 32768.0) - 32768.0); }, kern::to<Float>(rounded)))};
  }

  struct encode_navigation_wireResult { hwcontract_NavigationWireFrame Result = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; };
  KERN_PURE encode_navigation_wireResult encode_navigation_wire(Int sequence = 0, Float vx_mps = 0.0, Float vy_mps = 0.0, Float vz_mps = 0.0, Float agl_m = 0.0, Bool velocity_valid = false, Bool agl_valid = false, Float heading_deg = 0.0, Bool heading_valid = false) {
    hwcontract_NavigationWireFrame frame = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0};
    Int mm = 0;
    Float wrapped = 0.0;
    Int code = 0;
    Fixed<Int, 20> bytes = Fixed<Int, 20>{};
    frame = struct_make<hwcontract_NavigationWireFrame>(827736398, 1, 0, 0, 0, 0, 0, 0, 0);
    frame.sequence = sequence;
    frame.vx_cms = encode_navigation_wire_s16(vx_mps).Result;
    frame.vy_cms = encode_navigation_wire_s16(vy_mps).Result;
    frame.vz_cms = encode_navigation_wire_s16(vz_mps).Result;
    mm = kern::to<Int>(kern::formula([](Float agl_m) -> Float { return kern::math::round((kern::math::max(0.0, agl_m) * 1000.0)); }, agl_m));
    frame.agl_mm = kern::to<Int>(kern::formula([](Float mm) -> Float { return kern::math::band((mm < 0.0 ? 0.0 : (65535.0 < mm ? 65535.0 : mm)), 65535.0); }, kern::to<Float>(mm)));
    frame.flags = kern::to<Int>(kern::formula([](Float kNavigationSplitValidity, Float velocity_valid, Float kNavigationVelocityValid, Float agl_valid, Float kNavigationAglValid) -> Float { return kern::math::band(kern::math::bor(kern::math::bor(kNavigationSplitValidity, ((velocity_valid != 0.0) ? kNavigationVelocityValid : 0.0)), ((agl_valid != 0.0) ? kNavigationAglValid : 0.0)), 65535.0); }, kern::to<Float>(kNavigationSplitValidity), kern::to<Float>(velocity_valid), kern::to<Float>(kNavigationVelocityValid), kern::to<Float>(agl_valid), kern::to<Float>(kNavigationAglValid)));
    if (logic_and(heading_valid, math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, heading_deg), 0.0))) {
      wrapped = kern::formula([](Float heading_deg) -> Float { return kern::math::mod(heading_deg, 360.0); }, heading_deg);
      if (math_compare<"<">(wrapped, 0.0)) {
        wrapped += 360.0;
      }
      code = kern::to<Int>(kern::formula([](Float wrapped) -> Float { return kern::math::round((wrapped * 10.0)); }, wrapped));
      if (math_compare<">=">(kern::to<Float>(code), 3600.0)) {
        code = kern::to<Int>(kern::formula([](Float code) -> Float { return (code - 3600.0); }, kern::to<Float>(code)));
      }
      frame.flags = kern::to<Int>(kern::formula([](Float flags, Float kNavigationHeadingValid, Float code, Float kNavigationHeadingShift, Float kNavigationHeadingMask) -> Float { return kern::math::bor(flags, kern::math::bor(kNavigationHeadingValid, kern::math::band(kern::math::band(kern::math::wrap32s(kern::math::shl(kern::math::band(code, 65535.0), kNavigationHeadingShift)), kNavigationHeadingMask), 65535.0))); }, kern::to<Float>(frame.flags), kern::to<Float>(kNavigationHeadingValid), kern::to<Float>(code), kern::to<Float>(kNavigationHeadingShift), kern::to<Float>(kNavigationHeadingMask)));
    }
    bytes = struct_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(frame);
    frame.crc16 = crc16_ccitt(bytes, 0, 18).Result;
    return {frame};
  }

  struct encode_navigation_wire_2Result { hwcontract_NavigationWireFrame Result = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; };
  KERN_PURE encode_navigation_wire_2Result encode_navigation_wire_2(Int sequence = 0, Float vx_mps = 0.0, Float vy_mps = 0.0, Float vz_mps = 0.0, Float agl_m = 0.0, Bool valid = false) {
    return {encode_navigation_wire(sequence, vx_mps, vy_mps, vz_mps, agl_m, valid, valid, 0.0, false).Result};
  }

  struct NavigationWireParser_feedResult { Bool Result = false; };
  NavigationWireParser_feedResult NavigationWireParser_feed(hwcontract_NavigationWireParser& self, hwcontract_NavigationWireFrame& out, Int byte = 0) {
    Fixed<Int, 4> magic = Fixed<Int, 4>{};
    Int was = 0;
    Int was_2 = 0;
    magic = list_make<Int, "4">(78, 65, 86, 49);
    if (math_compare<">">(kern::formula([](Float size_) -> Float { return ((size_ < 0.0) != (4.0 < 0.0) ? (4.0 < 0.0) : (size_ < 4.0)); }, kern::to<Float>(self.size_)), 0.5)) {
      if (math_compare<"==">(kern::to<Float>(byte), kern::to<Float>(magic[self.size_]))) {
        was = self.size_;
        self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
        self.buffer_[was] = byte;
      } else {
        self.size_ = kern::to<Int>(kern::formula([](Float byte, Float a) -> Float { return ((byte == a) ? 1.0 : 0.0); }, kern::to<Float>(byte), kern::to<Float>(magic[0])));
        if (math_compare<"==">(kern::to<Float>(self.size_), 1.0)) {
          self.buffer_[0] = byte;
        }
      }
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
    self.buffer_[was_2] = byte;
    if (math_compare<">">(kern::formula([](Float size_, Float a) -> Float { return ((size_ < 0.0) != (a < 0.0) ? (a < 0.0) : (size_ < a)); }, kern::to<Float>(self.size_), kern::to<Float>(list_length<Int>(self.buffer_))), 0.5)) {
      return {false};
    }
    out = struct_of_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(self.buffer_, 0);
    self.size_ = 0;
    if (math_compare<"==">(kern::to<Float>(out.magic), kern::to<Float>(kNavigationMagic))) {
      return {true};
    }
    return {false};
  }

  struct SbusWireParser_feedResult { Bool Result = false; };
  SbusWireParser_feedResult SbusWireParser_feed(hwcontract_SbusWireParser& self, Fixed<Int, 25>& frame, Int byte = 0, Int64 us = Int64{0x0}) {
    Int was = 0;
    Int was_2 = 0;
    fc_RC decoded = fc_RC{List<Int>{}, false, false, false, Int64{0x0}};
    Int i = 0;
    Bool stop = false;
    Int keep = 0;
    Int count = 0;
    Int to = 0;
    Int from = 0;
    Int last = 0;
    Int k = 0;
    if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.size_), 0.0), int64_less_unsigned(self.last_us_, us)), int64_less_unsigned(Int64{0xbb8}, int64_sub(us, self.last_us_)))) {
      self.size_ = 0;
    }
    self.last_us_ = us;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.size_), 0.0))) {
      if (math_compare<"!=">(kern::to<Float>(byte), 15.0)) {
        return {false};
      }
      was = self.size_;
      self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
      self.buffer_[was] = byte;
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
    self.buffer_[was_2] = byte;
    if (math_compare<">">(kern::formula([](Float size_, Float a) -> Float { return ((size_ < 0.0) != (a < 0.0) ? (a < 0.0) : (size_ < a)); }, kern::to<Float>(self.size_), kern::to<Float>(list_length<Int>(self.buffer_))), 0.5)) {
      return {false};
    }
    self.size_ = 0;
    decoded = struct_make<fc_RC>(List<Int>{}, false, false, false, Int64{0x0});
    n12 = Arondight45_DroneFC_Core::decode_sbus(self.buffer_, decoded, 0);
    if (n12.Result) {
      frame = self.buffer_;
      return {true};
    }
    i = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Int>(self.buffer_))), 0.5)))) {
      if (math_compare<"==">(kern::to<Float>(self.buffer_[i]), 15.0)) {
        keep = kern::to<Int>(kern::formula([](Float a, Float i) -> Float { return (a - i); }, kern::to<Float>(list_length<Int>(self.buffer_)), kern::to<Float>(i)));
        count = keep;
        to = 0;
        from = i;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)));
        if (math_compare<"<=">(kern::to<Float>(to), kern::to<Float>(from))) {
          for (auto it13 = flow_for(n13, 0, last); it13.next();) {
            self.buffer_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(to), kern::to<Float>(n13.index)))] = list_get<Int>(self.buffer_, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(from), kern::to<Float>(n13.index))));
          }
        } else {
          k = last;
          while (loop(math_compare<">=">(kern::to<Float>(k), 0.0))) {
            self.buffer_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(to), kern::to<Float>(k)))] = list_get<Int>(self.buffer_, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(from), kern::to<Float>(k))));
            k = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(k)));
          }
        }
        self.size_ = keep;
        stop = true;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    return {false};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  Arondight45_DroneFC_Core::decode_sbusResult n12;
  struct : kern::Node<13> { Int index = 0; } n13;
};
