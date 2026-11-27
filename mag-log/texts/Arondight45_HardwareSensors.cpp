struct Arondight45_HardwareSensors : virtual kern::Library, virtual Arondight45_DroneFC_Core, virtual Arondight45_StateControl {
  // ---- types ----
  KERN_STRUCT(hwcontract_NavigationWireFrame) { u32 magic = 827736398; u16 version = 1; u16 sequence = 0; i16 vx_cms = 0; i16 vy_cms = 0; i16 vz_cms = 0; u16 agl_mm = 0; u16 flags = 0; u16 crc16 = 0; };
  KERN_STRUCT(hwcontract_NavigationWireParser) { Fixed<u8, 20> buffer_ = List<u8>{}; u64 size_ = 0; };
  KERN_STRUCT(hwcontract_SbusWireParser) { Fixed<u8, 25> buffer_ = List<u8>{}; u64 size_ = 0; u64 last_us_ = 0; };

  // ---- variables ----
  /// "NAV1" little-endian.
  KERN_CONST u32 kNavigationMagic = 827736398;
  KERN_CONST u16 kNavigationVersion = 1;
  KERN_CONST u16 kNavigationSplitValidity = 32768;
  KERN_CONST u16 kNavigationVelocityValid = 1;
  KERN_CONST u16 kNavigationAglValid = 2;
  /// Legacy v1 senders used bit 0 to mean the whole NAV solution was valid.
  KERN_CONST u16 kNavigationValid = 1;
  KERN_CONST u16 kNavigationHeadingValid = 4;
  /// 12 bits, 0.1 degree/code.
  KERN_CONST u16 kNavigationHeadingMask = 32760;
  KERN_CONST u16 kNavigationHeadingShift = 3;
  KERN_CONST u32 kNavigationTimeoutUs = 60000;
  KERN_CONST u64 kNavigationFrameBytes = 20;

  // ---- functions ----
  struct crc16_ccittResult { u16 Result = 0; };
  KERN_PURE crc16_ccittResult crc16_ccitt(List<u8> data = List<u8>{}, i64 data_at = 0, u64 length = 0) {
    i64 bytes_at = 0;
    u16 crc = 0;
    bytes_at = data_at;
    crc = 65535;
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(num_cast<i64, u64>(length), 1)); it10.next();) {
      crc = num_cast<u16, i32>(kern::formula<u16, u8>([](auto crc, auto a) { return (num_cast<i32, u16>(crc) ^ (num_cast<i32, u16>(num_cast<u16, u8>(a)) << 8)); }, crc, data[math_add<i64>(bytes_at, n10.index)]));
      for (auto it11 = flow_for(n11, 0, 7); it11.next();) {
        crc = kern::formula<u16, u32, u32>([](auto crc, auto a4129, auto a0) { return num_cast<u16, u32>((num_cast<u32, i32>((num_cast<i32, u16>(crc) << 1)) ^ (num_cast<bool, u32>((num_cast<u32, u16>(crc) & 32768)) ? a4129 : a0))); }, crc, 4129, 0);
      }
    }
    return {crc};
  }

  struct read_be_i16Result { i16 Result = 0; };
  KERN_PURE read_be_i16Result read_be_i16(List<u8> p = List<u8>{}, i64 p_at = 0) {
    return {kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<i16, i32>(((num_cast<i32, u16>(num_cast<u16, u8>(a)) << 8) | num_cast<i32, u8>(a_2))); }, p[p_at], p[math_add<i64>(p_at, 1)])};
  }

  struct orient_targetResult { fc_V3 Result = fc_V3{0.0f, 0.0f, 0.0f}; };
  KERN_PURE orient_targetResult orient_target(fc_V3 value = fc_V3{0.0f, 0.0f, 0.0f}) {
    fc_V3 out = fc_V3{0.0f, 0.0f, 0.0f};
    out = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    out = value;
    return {out};
  }

  struct decode_icm42688_registersResult { bool Result = false; };
  /// This is the one ICM-42688-P register decoder used by production, physical HIL and browser SIL. The simulator emits the same 14 register bytes that SPI reads.
  decode_icm42688_registersResult decode_icm42688_registers(fc_Imu& out, List<u8> raw = List<u8>{}, i64 raw_at = 0) {
    i16 bad = 0;
    i16 ax = 0;
    i16 ay = 0;
    i16 az = 0;
    i16 gx = 0;
    i16 gy = 0;
    i16 gz = 0;
    if (logic_not(math_compare<">=", i64>(raw_at, 0))) {
      return {false};
    }
    bad = -32768;
    ax = read_be_i16(raw, math_add<i64>(raw_at, 2)).Result;
    ay = read_be_i16(raw, math_add<i64>(raw_at, 4)).Result;
    az = read_be_i16(raw, math_add<i64>(raw_at, 6)).Result;
    gx = read_be_i16(raw, math_add<i64>(raw_at, 8)).Result;
    gy = read_be_i16(raw, math_add<i64>(raw_at, 10)).Result;
    gz = read_be_i16(raw, math_add<i64>(raw_at, 12)).Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_or(math_compare<"==", i32>(num_cast<i32, i16>(ax), num_cast<i32, i16>(bad)), math_compare<"==", i32>(num_cast<i32, i16>(ay), num_cast<i32, i16>(bad))), math_compare<"==", i32>(num_cast<i32, i16>(az), num_cast<i32, i16>(bad))), math_compare<"==", i32>(num_cast<i32, i16>(gx), num_cast<i32, i16>(bad))), math_compare<"==", i32>(num_cast<i32, i16>(gy), num_cast<i32, i16>(bad))), math_compare<"==", i32>(num_cast<i32, i16>(gz), num_cast<i32, i16>(bad)))) {
      return {false};
    }
    out.a = orient_target(struct_make<fc_V3>(kern::formula<i16>([](auto ax) { return (num_cast<f32, i32>(num_cast<i32, i16>(ax)) / 2048.0f); }, ax), kern::formula<i16>([](auto ay) { return (num_cast<f32, i32>(num_cast<i32, i16>(ay)) / 2048.0f); }, ay), kern::formula<i16>([](auto az) { return (num_cast<f32, i32>(num_cast<i32, i16>(az)) / 2048.0f); }, az))).Result;
    out.g = orient_target(struct_make<fc_V3>(kern::formula<i16>([](auto gx) { return (num_cast<f32, i32>(num_cast<i32, i16>(gx)) / 16.4f); }, gx), kern::formula<i16>([](auto gy) { return (num_cast<f32, i32>(num_cast<i32, i16>(gy)) / 16.4f); }, gy), kern::formula<i16>([](auto gz) { return (num_cast<f32, i32>(num_cast<i32, i16>(gz)) / 16.4f); }, gz))).Result;
    return {logic_and(Arondight45_DroneFC_Core::finite(out.a).Result, Arondight45_DroneFC_Core::finite(out.g).Result)};
  }

  struct decode_navigation_wireResult { bool Result = false; };
  decode_navigation_wireResult decode_navigation_wire(const hwcontract_NavigationWireFrame& frame, fc_NavigationState& out) {
    Fixed<u8, 20> bytes = Fixed<u8, 20>{};
    bool split = false;
    bool legacy_valid = false;
    u16 code = 0;
    if (logic_or(math_compare<"!=", u32>(frame.magic, kNavigationMagic), math_compare<"!=", i32>(num_cast<i32, u16>(frame.version), num_cast<i32, u16>(kNavigationVersion)))) {
      return {false};
    }
    bytes = struct_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(frame);
    if (math_compare<"!=", i32>(num_cast<i32, u16>(crc16_ccitt(bytes, 0, 18).Result), num_cast<i32, u16>(frame.crc16))) {
      return {false};
    }
    out.velocity_world_mps = struct_make<fc_V3>(kern::formula<i16>([](auto vx_cms) { return (num_cast<f32, i32>(num_cast<i32, i16>(vx_cms)) * 0.01f); }, frame.vx_cms), kern::formula<i16>([](auto vy_cms) { return (num_cast<f32, i32>(num_cast<i32, i16>(vy_cms)) * 0.01f); }, frame.vy_cms), kern::formula<i16>([](auto vz_cms) { return (num_cast<f32, i32>(num_cast<i32, i16>(vz_cms)) * 0.01f); }, frame.vz_cms));
    out.agl_m = kern::formula<u16>([](auto agl_mm) { return (num_cast<f32, i32>(num_cast<i32, u16>(agl_mm)) * 0.001f); }, frame.agl_mm);
    split = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto flags, auto kNavigationSplitValidity) { return (num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationSplitValidity)); }, frame.flags, kNavigationSplitValidity), 0);
    if (split) {
      out.velocity_valid = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto flags, auto kNavigationVelocityValid) { return (num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationVelocityValid)); }, frame.flags, kNavigationVelocityValid), 0);
      out.agl_valid = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto flags, auto kNavigationAglValid) { return (num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationAglValid)); }, frame.flags, kNavigationAglValid), 0);
    } else {
      legacy_valid = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto flags, auto kNavigationValid) { return (num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationValid)); }, frame.flags, kNavigationValid), 0);
      out.velocity_valid = legacy_valid;
      out.agl_valid = legacy_valid;
    }
    out.heading_valid = logic_and(split, math_compare<"!=", i32>(kern::formula<u16, u16>([](auto flags, auto kNavigationHeadingValid) { return (num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationHeadingValid)); }, frame.flags, kNavigationHeadingValid), 0));
    out.heading_deg = 0.0f;
    if (out.heading_valid) {
      code = kern::formula<u16, u16, u16>([](auto flags, auto kNavigationHeadingMask, auto kNavigationHeadingShift) { return num_cast<u16, i32>(((num_cast<i32, u16>(flags) & num_cast<i32, u16>(kNavigationHeadingMask)) >> num_cast<i32, u16>(kNavigationHeadingShift))); }, frame.flags, kNavigationHeadingMask, kNavigationHeadingShift);
      if (math_compare<">=", u32>(num_cast<u32, u16>(code), 3600)) {
        return {false};
      }
      out.heading_deg = kern::formula<u16>([](auto code) { return (num_cast<f32, u16>(code) * 0.1f); }, code);
      if (math_compare<">=", f32>(out.heading_deg, 180.0f)) {
        out.heading_deg = kern::formula<f32>([](auto heading_deg) { return (heading_deg - 360.0f); }, out.heading_deg);
      }
    }
    out.valid = logic_and(out.velocity_valid, out.agl_valid);
    return {logic_and(logic_and(logic_or(logic_not(out.velocity_valid), Arondight45_DroneFC_Core::finite(out.velocity_world_mps).Result), logic_or(logic_not(out.agl_valid), logic_and(kern::formula<f32>([](auto agl_m) { return isfinite(agl_m); }, out.agl_m), math_compare<">=", f32>(out.agl_m, 0.0f)))), logic_or(logic_not(out.heading_valid), Arondight45_StateControl::navigation_heading_valid(out).Result))};
  }

  struct encode_navigation_wire_s16Result { i16 Result = 0; };
  KERN_PURE encode_navigation_wire_s16Result encode_navigation_wire_s16(f32 value = 0.0f) {
    i64 rounded = 0;
    rounded = kern::formula<f32>([](auto value) { return num_cast<i64, f32>(round((value * 100.0f))); }, value);
    return {kern::formula<i64>([](auto rounded) { return num_cast<i16, i64>((rounded < (-32767) ? (-32767) : (32767 < rounded ? 32767 : rounded))); }, rounded)};
  }

  struct encode_navigation_wireResult { hwcontract_NavigationWireFrame Result = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; };
  KERN_PURE encode_navigation_wireResult encode_navigation_wire(u16 sequence = 0, f32 vx_mps = 0.0f, f32 vy_mps = 0.0f, f32 vz_mps = 0.0f, f32 agl_m = 0.0f, bool velocity_valid = false, bool agl_valid = false, f32 heading_deg = 0.0f, bool heading_valid = false) {
    hwcontract_NavigationWireFrame frame = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0};
    i64 mm = 0;
    f32 wrapped = 0.0f;
    i64 code = 0;
    Fixed<u8, 20> bytes = Fixed<u8, 20>{};
    frame = struct_make<hwcontract_NavigationWireFrame>(827736398, 1, 0, 0, 0, 0, 0, 0, 0);
    frame.sequence = sequence;
    frame.vx_cms = encode_navigation_wire_s16(vx_mps).Result;
    frame.vy_cms = encode_navigation_wire_s16(vy_mps).Result;
    frame.vz_cms = encode_navigation_wire_s16(vz_mps).Result;
    mm = kern::formula<f32>([](auto agl_m) { return num_cast<i64, f32>(round((max(0.0f, agl_m) * 1000.0f))); }, agl_m);
    frame.agl_mm = kern::formula<i64>([](auto mm) { return num_cast<u16, i64>((mm < 0 ? 0 : (65535 < mm ? 65535 : mm))); }, mm);
    frame.flags = kern::formula<u16, bool, u16, u32, bool, u16>([](auto kNavigationSplitValidity, auto velocity_valid, auto kNavigationVelocityValid, auto a0, auto agl_valid, auto kNavigationAglValid) { return num_cast<u16, u32>(((num_cast<u32, u16>(kNavigationSplitValidity) | (velocity_valid ? num_cast<u32, u16>(kNavigationVelocityValid) : a0)) | (agl_valid ? num_cast<u32, u16>(kNavigationAglValid) : a0))); }, kNavigationSplitValidity, velocity_valid, kNavigationVelocityValid, 0, agl_valid, kNavigationAglValid);
    if (logic_and(heading_valid, kern::formula<f32>([](auto heading_deg) { return isfinite(heading_deg); }, heading_deg))) {
      wrapped = kern::formula<f32>([](auto heading_deg) { return fmod(heading_deg, 360.0f); }, heading_deg);
      if (math_compare<"<", f32>(wrapped, 0.0f)) {
        wrapped += 360.0f;
      }
      code = kern::formula<f32>([](auto wrapped) { return num_cast<i64, f32>(round((wrapped * 10.0f))); }, wrapped);
      if (math_compare<">=", i64>(code, 3600)) {
        code = kern::formula<i64>([](auto code) { return (code - 3600); }, code);
      }
      frame.flags = num_cast<u16, i32>(kern::formula<u16, u16, i64, u16, u16>([](auto flags, auto kNavigationHeadingValid, auto code, auto kNavigationHeadingShift, auto kNavigationHeadingMask) { return (num_cast<i32, u16>(flags) | (num_cast<i32, u16>(kNavigationHeadingValid) | num_cast<i32, u16>(num_cast<u16, i32>(((num_cast<i32, u16>(num_cast<u16, i64>(code)) << num_cast<i32, u16>(kNavigationHeadingShift)) & num_cast<i32, u16>(kNavigationHeadingMask)))))); }, frame.flags, kNavigationHeadingValid, code, kNavigationHeadingShift, kNavigationHeadingMask));
    }
    bytes = struct_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(frame);
    frame.crc16 = crc16_ccitt(bytes, 0, 18).Result;
    return {frame};
  }

  struct encode_navigation_wire_2Result { hwcontract_NavigationWireFrame Result = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; };
  KERN_PURE encode_navigation_wire_2Result encode_navigation_wire_2(u16 sequence = 0, f32 vx_mps = 0.0f, f32 vy_mps = 0.0f, f32 vz_mps = 0.0f, f32 agl_m = 0.0f, bool valid = false) {
    return {encode_navigation_wire(sequence, vx_mps, vy_mps, vz_mps, agl_m, valid, valid, 0.0f, false).Result};
  }

  struct NavigationWireParser_feedResult { bool Result = false; };
  NavigationWireParser_feedResult NavigationWireParser_feed(hwcontract_NavigationWireParser& self, hwcontract_NavigationWireFrame& out, u8 byte = 0) {
    Fixed<u8, 4> magic = Fixed<u8, 4>{};
    u64 was = 0;
    u64 was_2 = 0;
    magic = list_make<u8, "4">(78, 65, 86, 49);
    if (math_compare<"<", u64>(self.size_, 4)) {
      if (math_compare<"==", i32>(num_cast<i32, u8>(byte), num_cast<i32, u8>(magic[num_cast<i64, u64>(self.size_)]))) {
        was = self.size_;
        self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
        self.buffer_[num_cast<i64, u64>(was)] = byte;
      } else {
        self.size_ = kern::formula<u8, u8, u32, u32>([](auto byte, auto a, auto a1, auto a0) { return num_cast<u64, u32>(((num_cast<i32, u8>(byte) == num_cast<i32, u8>(a)) ? a1 : a0)); }, byte, magic[0], 1, 0);
        if (math_compare<"==", u64>(self.size_, 1)) {
          self.buffer_[0] = byte;
        }
      }
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
    self.buffer_[num_cast<i64, u64>(was_2)] = byte;
    if (math_compare<"<", u64>(self.size_, num_cast<u64, i64>(list_length<u8>(self.buffer_)))) {
      return {false};
    }
    out = struct_of_bytes<hwcontract_NavigationWireFrame, "20: u32@0 u16@4 u16@6 i16@8 i16@10 i16@12 u16@14 u16@16 u16@18">(self.buffer_, 0);
    self.size_ = 0;
    if (math_compare<"==", u32>(out.magic, kNavigationMagic)) {
      return {true};
    }
    return {false};
  }

  struct SbusWireParser_feedResult { bool Result = false; };
  SbusWireParser_feedResult SbusWireParser_feed(hwcontract_SbusWireParser& self, Fixed<u8, 25>& frame, u8 byte = 0, u64 us = 0) {
    u64 was = 0;
    u64 was_2 = 0;
    fc_RC decoded = fc_RC{List<u16>{}, false, false, false, 0};
    u64 i = 0;
    bool stop = false;
    u64 keep = 0;
    i64 count = 0;
    i64 to = 0;
    i64 from = 0;
    i64 last = 0;
    i64 k = 0;
    if (logic_and(logic_and(num_cast<bool, u64>(self.size_), math_compare<">", u64>(us, self.last_us_)), math_compare<">", u64>(kern::formula<u64, u64>([](auto us, auto last_us_) { return (us - last_us_); }, us, self.last_us_), 3000))) {
      self.size_ = 0;
    }
    self.last_us_ = us;
    if (logic_not(num_cast<bool, u64>(self.size_))) {
      if (math_compare<"!=", i32>(num_cast<i32, u8>(byte), 15)) {
        return {false};
      }
      was = self.size_;
      self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
      self.buffer_[num_cast<i64, u64>(was)] = byte;
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
    self.buffer_[num_cast<i64, u64>(was_2)] = byte;
    if (math_compare<"<", u64>(self.size_, num_cast<u64, i64>(list_length<u8>(self.buffer_)))) {
      return {false};
    }
    self.size_ = 0;
    decoded = struct_make<fc_RC>(List<u16>{}, false, false, false, 0);
    n12 = Arondight45_DroneFC_Core::decode_sbus(decoded, self.buffer_, 0);
    if (n12.Result) {
      frame = self.buffer_;
      return {true};
    }
    i = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<u8>(self.buffer_)))))) {
      if (math_compare<"==", i32>(num_cast<i32, u8>(self.buffer_[num_cast<i64, u64>(i)]), 15)) {
        keep = kern::formula<u64, u64>([](auto a, auto i) { return (a - i); }, num_cast<u64, i64>(list_length<u8>(self.buffer_)), i);
        count = num_cast<i64, u64>(keep);
        to = 0;
        from = num_cast<i64, u64>(i);
        last = math_sub<i64>(count, 1);
        if (math_compare<"<=", i64>(to, from)) {
          for (auto it13 = flow_for(n13, 0, last); it13.next();) {
            self.buffer_[math_add<i64>(to, n13.index)] = list_get<u8>(self.buffer_, math_add<i64>(from, n13.index));
          }
        } else {
          k = last;
          while (loop(math_compare<">=", i64>(k, 0))) {
            self.buffer_[math_add<i64>(to, k)] = list_get<u8>(self.buffer_, math_add<i64>(from, k));
            k = math_sub<i64>(k, 1);
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
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  Arondight45_DroneFC_Core::decode_sbusResult n12;
  struct : kern::Node<13> { i64 index = 0; } n13;
};
