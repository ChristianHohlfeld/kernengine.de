// QR code encoder (see qr_code.h). The construction follows the standard step by step: data bits,
// Reed-Solomon error correction per block over GF(256) (0x11D), interleaving, function patterns,
// zig-zag placement, mask, format and version information.
#include "qr_code.h"

#include <algorithm>
#include <cstdlib>

namespace sc::qr {

namespace {

// Level L per version (ISO/IEC 18004 table 9): EC codewords per block, number of blocks.
constexpr i32 kEccPerBlock[41] = {-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
constexpr i32 kBlocks[41] = {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25};
constexpr u32 kLevelL = 1;  // format bits of level L

i32 raw_modules(i32 v) {
  i32 r = (16 * v + 128) * v + 64;
  if (v >= 2) {
    const i32 n = v / 7 + 2;
    r -= (25 * n - 10) * n - 55;
    if (v >= 7) r -= 36;
  }
  return r;
}
i32 data_codewords(i32 v) { return raw_modules(v) / 8 - kEccPerBlock[v] * kBlocks[v]; }

u8 gf_mul(u8 x, u8 y) {
  u32 z = 0;
  for (i32 i = 7; i >= 0; --i) {
    z = (z << 1) ^ ((z >> 7) * 0x11D);
    z ^= ((y >> i) & 1u) * x;
  }
  return static_cast<u8>(z);
}
std::vector<u8> rs_divisor(i32 degree) {
  std::vector<u8> r(static_cast<std::size_t>(degree), 0);
  r.back() = 1;
  u8 root = 1;
  for (i32 i = 0; i < degree; ++i) {
    for (std::size_t j = 0; j < r.size(); ++j) {
      r[j] = gf_mul(r[j], root);
      if (j + 1 < r.size()) r[j] ^= r[j + 1];
    }
    root = gf_mul(root, 2);
  }
  return r;
}
std::vector<u8> rs_remainder(const std::vector<u8>& data, const std::vector<u8>& div) {
  std::vector<u8> r(div.size(), 0);
  for (u8 b : data) {
    const u8 f = b ^ r[0];
    r.erase(r.begin());
    r.push_back(0);
    for (std::size_t i = 0; i < r.size(); ++i) r[i] ^= gf_mul(div[i], f);
  }
  return r;
}

std::vector<i32> alignment_positions(i32 v) {
  if (v == 1) return {};
  const i32 n = v / 7 + 2, size = v * 4 + 17;
  const i32 step = v == 32 ? 26 : (v * 4 + n * 2 + 1) / (n * 2 - 2) * 2;
  std::vector<i32> r(static_cast<std::size_t>(n));
  r[0] = 6;
  for (i32 i = n - 1, pos = size - 7; i >= 1; --i, pos -= step) r[static_cast<std::size_t>(i)] = pos;
  return r;
}

struct Grid {
  i32 size;
  std::vector<u8> m, fn;
  void set(i32 x, i32 y, bool dark) { m[static_cast<std::size_t>(y * size + x)] = dark; fn[static_cast<std::size_t>(y * size + x)] = 1; }
};

void finder(Grid& g, i32 cx, i32 cy) {
  for (i32 dy = -4; dy <= 4; ++dy)
    for (i32 dx = -4; dx <= 4; ++dx) {
      const i32 x = cx + dx, y = cy + dy;
      if (x < 0 || y < 0 || x >= g.size || y >= g.size) continue;
      const i32 d = std::max(std::abs(dx), std::abs(dy));
      g.set(x, y, d != 2 && d != 4);
    }
}

void format_bits(Grid& g, u32 mask) {
  const u32 data = kLevelL << 3 | mask;
  u32 rem = data;
  for (i32 i = 0; i < 10; ++i) rem = (rem << 1) ^ ((rem >> 9) * 0x537);
  const u32 bits = (data << 10 | rem) ^ 0x5412;
  auto bit = [&](i32 i) { return ((bits >> i) & 1) != 0; };
  for (i32 i = 0; i <= 5; ++i) g.set(8, i, bit(i));
  g.set(8, 7, bit(6));
  g.set(8, 8, bit(7));
  g.set(7, 8, bit(8));
  for (i32 i = 9; i < 15; ++i) g.set(14 - i, 8, bit(i));
  for (i32 i = 0; i < 8; ++i) g.set(g.size - 1 - i, 8, bit(i));
  for (i32 i = 8; i < 15; ++i) g.set(8, g.size - 15 + i, bit(i));
  g.set(8, g.size - 8, true);  // the dark module
}

}  // namespace

bool encode(const std::string& text, Code& out) {
  const i32 n = static_cast<i32>(text.size());
  i32 v = 1;
  for (; v <= 40; ++v) {
    const i32 count_bits = v <= 9 ? 8 : 16;
    if (4 + count_bits + 8 * n <= data_codewords(v) * 8 && (count_bits == 16 || n < 256)) break;
  }
  if (v > 40) return false;
  // Data bits: byte mode, length, bytes, terminator, padding.
  std::vector<bool> bits;
  auto put = [&](u32 val, i32 len) { for (i32 i = len - 1; i >= 0; --i) bits.push_back(((val >> i) & 1) != 0); };
  put(4, 4);
  put(static_cast<u32>(n), v <= 9 ? 8 : 16);
  for (unsigned char c : text) put(c, 8);
  const std::size_t cap = static_cast<std::size_t>(data_codewords(v)) * 8;
  put(0, static_cast<i32>(std::min<std::size_t>(4, cap - bits.size())));
  while (bits.size() % 8) bits.push_back(false);
  for (u8 pad = 0xEC; bits.size() < cap; pad ^= 0xEC ^ 0x11) put(pad, 8);
  std::vector<u8> data(bits.size() / 8, 0);
  for (std::size_t i = 0; i < bits.size(); ++i) if (bits[i]) data[i >> 3] |= static_cast<u8>(1 << (7 - (i & 7)));
  // Error correction per block and interleaving.
  const i32 blocks = kBlocks[v], ecc = kEccPerBlock[v], raw = raw_modules(v) / 8;
  const i32 short_blocks = blocks - raw % blocks, short_len = raw / blocks;
  const std::vector<u8> div = rs_divisor(ecc);
  std::vector<std::vector<u8>> bl;
  for (i32 i = 0, k = 0; i < blocks; ++i) {
    const i32 len = short_len - ecc + (i < short_blocks ? 0 : 1);
    std::vector<u8> d(data.begin() + k, data.begin() + k + len);
    k += len;
    const std::vector<u8> e = rs_remainder(d, div);
    if (i < short_blocks) d.push_back(0);
    d.insert(d.end(), e.begin(), e.end());
    bl.push_back(d);
  }
  std::vector<u8> words;
  for (std::size_t i = 0; i < bl[0].size(); ++i)
    for (std::size_t j = 0; j < bl.size(); ++j)
      if (i != static_cast<std::size_t>(short_len - ecc) || j >= static_cast<std::size_t>(short_blocks)) words.push_back(bl[j][i]);
  // Function patterns.
  Grid g{v * 4 + 17, {}, {}};
  g.m.assign(static_cast<std::size_t>(g.size * g.size), 0);
  g.fn.assign(g.m.size(), 0);
  for (i32 i = 0; i < g.size; ++i) { g.set(6, i, i % 2 == 0); g.set(i, 6, i % 2 == 0); }
  finder(g, 3, 3);
  finder(g, g.size - 4, 3);
  finder(g, 3, g.size - 4);
  const std::vector<i32> al = alignment_positions(v);
  for (std::size_t i = 0; i < al.size(); ++i)
    for (std::size_t j = 0; j < al.size(); ++j) {
      if ((i == 0 && j == 0) || (i == 0 && j == al.size() - 1) || (i == al.size() - 1 && j == 0)) continue;
      for (i32 dy = -2; dy <= 2; ++dy)
        for (i32 dx = -2; dx <= 2; ++dx) g.set(al[i] + dx, al[j] + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
    }
  format_bits(g, 0);  // reserve (the real bits follow after masking)
  if (v >= 7) {
    u32 rem = static_cast<u32>(v);
    for (i32 i = 0; i < 12; ++i) rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    const u32 vb = static_cast<u32>(v) << 12 | rem;
    for (i32 i = 0; i < 18; ++i) {
      const bool b = ((vb >> i) & 1) != 0;
      const i32 a = g.size - 11 + i % 3, c = i / 3;
      g.set(a, c, b);
      g.set(c, a, b);
    }
  }
  // Codewords in the zig-zag, then mask 0 ((x + y) even) on the data modules.
  std::size_t i = 0;
  for (i32 right = g.size - 1; right >= 1; right -= 2) {
    if (right == 6) right = 5;
    for (i32 vert = 0; vert < g.size; ++vert)
      for (i32 j = 0; j < 2; ++j) {
        const i32 x = right - j;
        const bool up = ((right + 1) & 2) == 0;
        const i32 y = up ? g.size - 1 - vert : vert;
        const std::size_t at = static_cast<std::size_t>(y * g.size + x);
        if (g.fn[at]) continue;
        if (i < words.size() * 8) { g.m[at] = (words[i >> 3] >> (7 - (i & 7))) & 1; ++i; }
        if ((x + y) % 2 == 0) g.m[at] ^= 1;
      }
  }
  format_bits(g, 0);
  out.version = v;
  out.size = g.size;
  out.modules = std::move(g.m);
  return true;
}

bool read_format(const Code& c, u32& level_bits, u32& mask) {
  u32 bits = 0;
  for (i32 i = 0; i <= 5; ++i) bits |= static_cast<u32>(c.dark(8, i)) << i;
  bits |= static_cast<u32>(c.dark(8, 7)) << 6;
  bits |= static_cast<u32>(c.dark(8, 8)) << 7;
  bits |= static_cast<u32>(c.dark(7, 8)) << 8;
  for (i32 i = 9; i < 15; ++i) bits |= static_cast<u32>(c.dark(14 - i, 8)) << i;
  bits ^= 0x5412;
  level_bits = bits >> 13;
  mask = (bits >> 10) & 7;
  // The 10 BCH bits must match.
  u32 rem = bits >> 10;
  for (i32 i = 0; i < 10; ++i) rem = (rem << 1) ^ ((rem >> 9) * 0x537);
  return (rem & 0x3ff) == (bits & 0x3ff);
}

}  // namespace sc::qr
