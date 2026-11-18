struct Arondight45_HardwareSensors : virtual kern::Library, virtual Arondight45_DroneFC_Core {
  // ---- types ----
  KERN_STRUCT(hwcontract_NavigationWireFrame) { Int magic = 827736398; Int version = 1; Int sequence = 0; Int vx_cms = 0; Int vy_cms = 0; Int vz_cms = 0; Int agl_mm = 0; Int flags = 0; Int crc16 = 0; };
  KERN_STRUCT(hwcontract_NavigationWireParser) { Fixed<Int, 20> buffer_ = List<Int>{}; Int size_ = 0; };

  // ---- variables ----
  /// "NAV1" little-endian.
  KERN_CONST Int kNavigationMagic = 827736398;

  // ---- functions ----
  struct orient_targetResult { fc_V3 Result = fc_V3{0.0, 0.0, 0.0}; };
  KERN_PURE orient_targetResult orient_target(fc_V3 value = fc_V3{0.0, 0.0, 0.0}) {
    fc_V3 out = fc_V3{0.0, 0.0, 0.0};
    out = struct_make<fc_V3>(0.0, 0.0, 0.0);
    out = value;
    return {out};
  }

  struct encode_navigation_wire_s16Result { Int Result = 0; };
  KERN_PURE encode_navigation_wire_s16Result encode_navigation_wire_s16(Float value = 0.0) {
    Int rounded = 0;
    rounded = kern::to<Int>(kern::formula([](Float value) -> Float { return kern::math::round((value * 100.0)); }, value));
    return {kern::to<Int>(kern::formula([](Float rounded) -> Float { return (kern::math::bxor(kern::math::band((rounded < (-32767.0) ? (-32767.0) : (32767.0 < rounded ? 32767.0 : rounded)), 65535.0), 32768.0) - 32768.0); }, kern::to<Float>(rounded)))};
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
};
