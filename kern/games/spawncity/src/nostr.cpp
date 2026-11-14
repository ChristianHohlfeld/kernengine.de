// SHA-256 (FIPS 180-4), secp256k1 and BIP-340 Schnorr signatures for the suggestion box (nostr.h).
// Plain 256-bit arithmetic with 32-bit limbs; modular multiplication by double-and-add (slow, small,
// portable): one signature takes a moment, which is fine for one idea.
#include "nostr.h"

#include <cstdio>
#include <cstring>
#include <vector>

namespace sc::nostr {

// ---------------------------------------------------------------------------------------------
// SHA-256

namespace {
constexpr u32 kK[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74,
    0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d,
    0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e,
    0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
inline u32 rotr(u32 x, u32 n) { return (x >> n) | (x << (32 - n)); }
void block(u32 h[8], const u8* p) {
  u32 w[64];
  for (int i = 0; i < 16; ++i) w[i] = (u32{p[4 * i]} << 24) | (u32{p[4 * i + 1]} << 16) | (u32{p[4 * i + 2]} << 8) | p[4 * i + 3];
  for (int i = 16; i < 64; ++i) {
    const u32 s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3), s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }
  u32 a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], k = h[7];
  for (int i = 0; i < 64; ++i) {
    const u32 t1 = k + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + kK[i] + w[i];
    const u32 t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
    k = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
  }
  h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += k;
}
}  // namespace

void sha256(const u8* data, std::size_t length, u8 out[32]) {
  u32 h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  std::size_t i = 0;
  for (; i + 64 <= length; i += 64) block(h, data + i);
  u8 tail[128] = {};
  const std::size_t rest = length - i;
  std::memcpy(tail, data + i, rest);
  tail[rest] = 0x80;
  const std::size_t total = rest + 9 <= 64 ? 64 : 128;
  const u64 bits = static_cast<u64>(length) * 8;
  for (int b = 0; b < 8; ++b) tail[total - 1 - b] = static_cast<u8>(bits >> (8 * b));
  block(h, tail);
  if (total == 128) block(h, tail + 64);
  for (int k = 0; k < 8; ++k) for (int b = 0; b < 4; ++b) out[4 * k + b] = static_cast<u8>(h[k] >> (24 - 8 * b));
}

Bytes32 sha256(std::string_view s) {
  Bytes32 o;
  sha256(reinterpret_cast<const u8*>(s.data()), s.size(), o.data());
  return o;
}

// ---------------------------------------------------------------------------------------------
// 256-bit numbers (8 x 32-bit limbs, little endian)

namespace {
struct U256 { u32 w[8] = {}; };

U256 from_be(const u8* b) {
  U256 r;
  for (int i = 0; i < 8; ++i) r.w[7 - i] = (u32{b[4 * i]} << 24) | (u32{b[4 * i + 1]} << 16) | (u32{b[4 * i + 2]} << 8) | b[4 * i + 3];
  return r;
}
void to_be(const U256& a, u8* b) {
  for (int i = 0; i < 8; ++i) for (int k = 0; k < 4; ++k) b[4 * i + k] = static_cast<u8>(a.w[7 - i] >> (24 - 8 * k));
}
U256 from_hex256(const char* s) {
  u8 b[32];
  from_hex(s, b, 32);
  return from_be(b);
}
int cmp(const U256& a, const U256& b) {
  for (int i = 7; i >= 0; --i) if (a.w[i] != b.w[i]) return a.w[i] < b.w[i] ? -1 : 1;
  return 0;
}
bool is_zero(const U256& a) { for (u32 x : a.w) if (x) return false; return true; }
u32 add(U256& r, const U256& a, const U256& b) {
  u64 c = 0;
  for (int i = 0; i < 8; ++i) { c += u64{a.w[i]} + b.w[i]; r.w[i] = static_cast<u32>(c); c >>= 32; }
  return static_cast<u32>(c);
}
u32 sub(U256& r, const U256& a, const U256& b) {
  i64 c = 0;
  for (int i = 0; i < 8; ++i) { c += static_cast<i64>(a.w[i]) - b.w[i]; r.w[i] = static_cast<u32>(c); c >>= 32; }
  return c < 0 ? 1 : 0;
}
U256 addm(const U256& a, const U256& b, const U256& m) {
  U256 r;
  const u32 carry = add(r, a, b);
  if (carry || cmp(r, m) >= 0) sub(r, r, m);
  return r;
}
U256 subm(const U256& a, const U256& b, const U256& m) {
  U256 r;
  if (sub(r, a, b)) add(r, r, m);
  return r;
}
U256 mulm(const U256& a, const U256& b, const U256& m) {
  U256 r;
  for (int i = 255; i >= 0; --i) {
    r = addm(r, r, m);
    if ((b.w[i / 32] >> (i % 32)) & 1) r = addm(r, a, m);
  }
  return r;
}
U256 powm(const U256& a, const U256& e, const U256& m) {
  U256 r;
  r.w[0] = 1;
  for (int i = 255; i >= 0; --i) {
    r = mulm(r, r, m);
    if ((e.w[i / 32] >> (i % 32)) & 1) r = mulm(r, a, m);
  }
  return r;
}
U256 reduce(const U256& a, const U256& m) {  // a < 2m for the moduli here
  U256 r = a;
  if (cmp(r, m) >= 0) sub(r, r, m);
  return r;
}

// (the numbers of the curve, least significant word first: from_hex256 of the hex the standard gives -
// P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F, N = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
constexpr U256 kP{{0xFFFFFC2F, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
constexpr U256 kN{{0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
const U256& P() { return kP; }
const U256& N() { return kN; }

// ---- secp256k1 points (Jacobian, a = 0, b = 7) ----
struct Pt { U256 x, y, z; };  // z = 0: infinity
// (the generator: x = 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798, y = 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8, z = 1)
constexpr Pt kG{{{0x16F81798, 0x59F2815B, 0x2DCE28D9, 0x029BFCDB, 0xCE870B07, 0x55A06295, 0xF9DCBBAC, 0x79BE667E}}, {{0xFB10D4B8, 0x9C47D08F, 0xA6855419, 0xFD17B448, 0x0E1108A8, 0x5DA4FBFC, 0x26A3C465, 0x483ADA77}}, {{1}}};
const Pt& G() { return kG; }
U256 fm(const U256& a, const U256& b) { return mulm(a, b, P()); }
U256 fa(const U256& a, const U256& b) { return addm(a, b, P()); }
U256 fs(const U256& a, const U256& b) { return subm(a, b, P()); }

Pt dbl(const Pt& a) {
  if (is_zero(a.z) || is_zero(a.y)) return Pt{};
  const U256 y2 = fm(a.y, a.y);
  const U256 s = fm(fa(fa(a.x, a.x), fa(a.x, a.x)), y2);  // 4 X Y^2
  const U256 x2 = fm(a.x, a.x);
  const U256 m = fa(fa(x2, x2), x2);                       // 3 X^2
  Pt r;
  r.x = fs(fm(m, m), fa(s, s));
  const U256 y4 = fm(y2, y2);
  U256 y4_8 = fa(y4, y4);
  y4_8 = fa(y4_8, y4_8);
  y4_8 = fa(y4_8, y4_8);
  r.y = fs(fm(m, fs(s, r.x)), y4_8);
  r.z = fm(fa(a.y, a.y), a.z);
  return r;
}
Pt add_pt(const Pt& a, const Pt& b) {
  if (is_zero(a.z)) return b;
  if (is_zero(b.z)) return a;
  const U256 z1z1 = fm(a.z, a.z), z2z2 = fm(b.z, b.z);
  const U256 u1 = fm(a.x, z2z2), u2 = fm(b.x, z1z1);
  const U256 s1 = fm(a.y, fm(b.z, z2z2)), s2 = fm(b.y, fm(a.z, z1z1));
  if (cmp(u1, u2) == 0) return cmp(s1, s2) == 0 ? dbl(a) : Pt{};
  const U256 h = fs(u2, u1), r = fs(s2, s1);
  const U256 h2 = fm(h, h), h3 = fm(h2, h), u1h2 = fm(u1, h2);
  Pt o;
  o.x = fs(fs(fm(r, r), h3), fa(u1h2, u1h2));
  o.y = fs(fm(r, fs(u1h2, o.x)), fm(s1, h3));
  o.z = fm(h, fm(a.z, b.z));
  return o;
}
Pt mul_pt(const Pt& p, const U256& k) {
  Pt r;
  for (int i = 255; i >= 0; --i) {
    r = dbl(r);
    if ((k.w[i / 32] >> (i % 32)) & 1) r = add_pt(r, p);
  }
  return r;
}
bool affine(const Pt& p, U256& x, U256& y) {
  if (is_zero(p.z)) return false;
  U256 e;
  sub(e, P(), U256{{2}});
  const U256 zi = powm(p.z, e, P()), zi2 = fm(zi, zi);
  x = fm(p.x, zi2);
  y = fm(p.y, fm(zi2, zi));
  return true;
}
bool lift_x(const U256& x, Pt& out) {
  if (cmp(x, P()) >= 0) return false;
  U256 seven;
  seven.w[0] = 7;
  const U256 c = fa(fm(fm(x, x), x), seven);
  U256 e;  // (p + 1) / 4
  add(e, P(), U256{{1}});
  for (int i = 0; i < 8; ++i) e.w[i] = (e.w[i] >> 2) | (i < 7 ? e.w[i + 1] << 30 : 0);
  U256 y = powm(c, e, P());
  if (cmp(fm(y, y), c) != 0) return false;
  if (y.w[0] & 1) y = fs(U256{}, y);
  out.x = x;
  out.y = y;
  out.z = U256{{1}};
  return true;
}

Bytes32 tagged(std::string_view tag, const std::vector<u8>& msg) {
  const Bytes32 t = sha256(tag);
  std::vector<u8> buf(t.begin(), t.end());
  buf.insert(buf.end(), t.begin(), t.end());
  buf.insert(buf.end(), msg.begin(), msg.end());
  Bytes32 o;
  sha256(buf.data(), buf.size(), o.data());
  return o;
}
void append(std::vector<u8>& v, const u8* b, std::size_t n) { v.insert(v.end(), b, b + n); }
}  // namespace

bool public_key(const Bytes32& secret, Bytes32& out) {
  const U256 d = from_be(secret.data());
  if (is_zero(d) || cmp(d, N()) >= 0) return false;
  U256 x, y;
  if (!affine(mul_pt(G(), d), x, y)) return false;
  to_be(x, out.data());
  return true;
}

bool sign(const Bytes32& secret, const Bytes32& message, const Bytes32& aux, std::array<u8, 64>& sig) {
  U256 d = from_be(secret.data());
  if (is_zero(d) || cmp(d, N()) >= 0) return false;
  U256 px, py;
  if (!affine(mul_pt(G(), d), px, py)) return false;
  if (py.w[0] & 1) d = subm(U256{}, d, N());
  u8 pxb[32], db[32];
  to_be(px, pxb);
  to_be(d, db);
  const Bytes32 ah = tagged("BIP0340/aux", std::vector<u8>(aux.begin(), aux.end()));
  std::vector<u8> m;
  for (int i = 0; i < 32; ++i) m.push_back(db[i] ^ ah[i]);
  append(m, pxb, 32);
  append(m, message.data(), 32);
  const Bytes32 rand = tagged("BIP0340/nonce", m);
  U256 k = reduce(from_be(rand.data()), N());
  if (is_zero(k)) return false;
  U256 rx, ry;
  if (!affine(mul_pt(G(), k), rx, ry)) return false;
  if (ry.w[0] & 1) k = subm(U256{}, k, N());
  u8 rxb[32];
  to_be(rx, rxb);
  std::vector<u8> c;
  append(c, rxb, 32);
  append(c, pxb, 32);
  append(c, message.data(), 32);
  const U256 e = reduce(from_be(tagged("BIP0340/challenge", c).data()), N());
  const U256 s = addm(k, mulm(e, d, N()), N());
  std::memcpy(sig.data(), rxb, 32);
  to_be(s, sig.data() + 32);
  return verify(Bytes32{[&] { Bytes32 b; std::memcpy(b.data(), pxb, 32); return b; }()}, message, sig);
}

bool verify(const Bytes32& pubkey, const Bytes32& message, const std::array<u8, 64>& sig) {
  Pt p;
  if (!lift_x(from_be(pubkey.data()), p)) return false;
  const U256 r = from_be(sig.data()), s = from_be(sig.data() + 32);
  if (cmp(r, P()) >= 0 || cmp(s, N()) >= 0) return false;
  std::vector<u8> c;
  append(c, sig.data(), 32);
  append(c, pubkey.data(), 32);
  append(c, message.data(), 32);
  const U256 e = reduce(from_be(tagged("BIP0340/challenge", c).data()), N());
  // R = sG - eP
  Pt ep = mul_pt(p, e);
  ep.y = fs(U256{}, ep.y);
  U256 x, y;
  if (!affine(add_pt(mul_pt(G(), s), ep), x, y)) return false;
  return (y.w[0] & 1) == 0 && cmp(x, r) == 0;
}

std::string hex(const u8* data, std::size_t n) {
  static const char* d = "0123456789abcdef";
  std::string o;
  for (std::size_t i = 0; i < n; ++i) { o += d[data[i] >> 4]; o += d[data[i] & 15]; }
  return o;
}

bool from_hex(std::string_view s, u8* out, std::size_t n) {
  if (s.size() != 2 * n) return false;
  auto v = [](char c) { return c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1; };
  for (std::size_t i = 0; i < n; ++i) {
    const int a = v(s[2 * i]), b = v(s[2 * i + 1]);
    if (a < 0 || b < 0) return false;
    out[i] = static_cast<u8>(a * 16 + b);
  }
  return true;
}

std::string escape(std::string_view s) {
  std::string o;
  for (char c : s) {
    switch (c) {
      case '\n': o += "\\n"; break;
      case '"': o += "\\\""; break;
      case '\\': o += "\\\\"; break;
      case '\r': o += "\\r"; break;
      case '\t': o += "\\t"; break;
      case '\b': o += "\\b"; break;
      case '\f': o += "\\f"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) { char b[8]; std::snprintf(b, sizeof b, "\\u%04x", c); o += b; }
        else o += c;
    }
  }
  return o;
}

bool event_json(const Bytes32& secret, const Bytes32& aux, i64 created_at, u32 kind, std::string_view tags, std::string_view content, std::string& out) {
  Bytes32 pk;
  if (!public_key(secret, pk)) return false;
  const std::string pkh = hex(pk.data(), 32), body = escape(content);
  char head[96];
  std::snprintf(head, sizeof head, "[0,\"%s\",%lld,%u,", pkh.c_str(), static_cast<long long>(created_at), kind);
  const std::string ser = std::string(head) + std::string(tags) + ",\"" + body + "\"]";
  const Bytes32 id = sha256(ser);
  std::array<u8, 64> sig;
  if (!sign(secret, id, aux, sig)) return false;
  char num[64];
  std::snprintf(num, sizeof num, ",\"created_at\":%lld,\"kind\":%u,\"tags\":", static_cast<long long>(created_at), kind);
  out = "{\"id\":\"" + hex(id.data(), 32) + "\",\"pubkey\":\"" + pkh + "\"" + num + std::string(tags) + ",\"content\":\"" + body + "\",\"sig\":\"" + hex(sig.data(), 64) + "\"}";
  return true;
}

}  // namespace sc::nostr
