// A QR code encoder (ISO/IEC 18004, byte mode, error correction level L as SpawnCity's
// qr_pairing.mjs uses it) for the join link on the share panel: another phone scans it with its
// camera and opens the game in the same room. Versions 1..40, mask pattern 0 (every mask is a
// valid code; scanners read all eight).
#pragma once

#include <string>
#include <vector>

#include "common.h"

namespace sc::qr {

struct Code {
  i32 version = 0, size = 0;
  std::vector<u8> modules;  // size * size, 1 = dark, row major
  bool dark(i32 x, i32 y) const { return x >= 0 && y >= 0 && x < size && y < size && modules[static_cast<std::size_t>(y * size + x)]; }
};

/// Encodes `text` (bytes as given, UTF-8) in the smallest version that holds it; false when too long.
bool encode(const std::string& text, Code& out);
/// The format information read back from the code (tests): level bits and mask.
bool read_format(const Code& c, u32& level_bits, u32& mask);

}  // namespace sc::qr
