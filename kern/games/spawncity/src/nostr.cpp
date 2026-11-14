// SHA-256 (FIPS 180-4), secp256k1 and BIP-340 Schnorr signatures for the suggestion box (nostr.h).
// Plain 256-bit arithmetic with 32-bit limbs; modular multiplication by double-and-add (slow, small,
// portable): one signature takes a moment, which is fine for one idea.
#include "nostr.h"

#include <cstdio>
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
void block(std::array<u32, 8>& h, const std::array<u8, 64>& p) {
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
/// A hash in progress: the state, the block being filled and how many bytes went in. The bytes
/// come one by one - from a list of bytes or from a text -, so nothing is copied to hash it.
struct Hasher {
  std::array<u32, 8> h = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  std::array<u8, 64> chunk = {};
  u32 filled = 0;
  u64 length = 0;
};
void push(Hasher& s, u8 byte) {
  s.chunk[s.filled] = byte;
  s.filled += 1;
  s.length += 1;
  if (s.filled == 64) {
    block(s.h, s.chunk);
    s.filled = 0;
  }
}
/// The padding (0x80, zeros, the length in bits, big endian) and the hash.
Bytes32 finish(Hasher& s) {
  const u64 bits = s.length * 8;
  push(s, 0x80);
  while (s.filled != 56) push(s, 0);
  for (int b = 7; b >= 0; --b) push(s, static_cast<u8>(bits >> (8 * b)));
  Bytes32 out;
  for (int k = 0; k < 8; ++k) for (int b = 0; b < 4; ++b) out[4 * k + b] = static_cast<u8>(s.h[k] >> (24 - 8 * b));
  return out;
}
}  // namespace

Bytes32 sha256(const std::vector<u8>& data) {
  Hasher s;
  for (u8 byte : data) push(s, byte);
  return finish(s);
}

Bytes32 sha256(std::string_view text) {
  Hasher s;
  for (char c : text) push(s, static_cast<u8>(c));
  return finish(s);
}

// ---------------------------------------------------------------------------------------------
// 256-bit numbers (8 x 32-bit limbs, little endian)

namespace {
struct U256 { u32 w[8] = {}; };

U256 from_be(const Bytes32& b) {
  U256 r;
  for (int i = 0; i < 8; ++i) r.w[7 - i] = (u32{b[4 * i]} << 24) | (u32{b[4 * i + 1]} << 16) | (u32{b[4 * i + 2]} << 8) | b[4 * i + 3];
  return r;
}
Bytes32 to_be(const U256& a) {
  Bytes32 b;
  for (int i = 0; i < 8; ++i) for (int k = 0; k < 4; ++k) b[4 * i + k] = static_cast<u8>(a.w[7 - i] >> (24 - 8 * k));
  return b;
}
/// One half of a signature: its first (0) or its second (1) 32 bytes.
Bytes32 half(const std::array<u8, 64>& sig, int which) {
  Bytes32 b;
  for (int i = 0; i < 32; ++i) b[i] = sig[32 * which + i];
  return b;
}
U256 from_hex256(const char* s) {
  Bytes32 b{};
  from_hex(s, b);
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

const U256& P() { static const U256 v = from_hex256("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"); return v; }
const U256& N() { static const U256 v = from_hex256("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"); return v; }

// ---- secp256k1 points (Jacobian, a = 0, b = 7) ----
struct Pt { U256 x, y, z; };  // z = 0: infinity
const Pt& G() {
  static const Pt g = [] {
    Pt p;
    p.x = from_hex256("79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    p.y = from_hex256("483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
    p.z.w[0] = 1;
    return p;
  }();
  return g;
}
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
  return sha256(buf);
}
void append(std::vector<u8>& v, const Bytes32& b) { v.insert(v.end(), b.begin(), b.end()); }
}  // namespace

bool public_key(const Bytes32& secret, Bytes32& out) {
  const U256 d = from_be(secret);
  if (is_zero(d) || cmp(d, N()) >= 0) return false;
  U256 x, y;
  if (!affine(mul_pt(G(), d), x, y)) return false;
  out = to_be(x);
  return true;
}

bool sign(const Bytes32& secret, const Bytes32& message, const Bytes32& aux, std::array<u8, 64>& sig) {
  U256 d = from_be(secret);
  if (is_zero(d) || cmp(d, N()) >= 0) return false;
  U256 px, py;
  if (!affine(mul_pt(G(), d), px, py)) return false;
  if (py.w[0] & 1) d = subm(U256{}, d, N());
  const Bytes32 pxb = to_be(px), db = to_be(d);
  const Bytes32 ah = tagged("BIP0340/aux", std::vector<u8>(aux.begin(), aux.end()));
  std::vector<u8> m;
  for (int i = 0; i < 32; ++i) m.push_back(db[i] ^ ah[i]);
  append(m, pxb);
  append(m, message);
  const Bytes32 rand = tagged("BIP0340/nonce", m);
  U256 k = reduce(from_be(rand), N());
  if (is_zero(k)) return false;
  U256 rx, ry;
  if (!affine(mul_pt(G(), k), rx, ry)) return false;
  if (ry.w[0] & 1) k = subm(U256{}, k, N());
  const Bytes32 rxb = to_be(rx);
  std::vector<u8> c;
  append(c, rxb);
  append(c, pxb);
  append(c, message);
  const U256 e = reduce(from_be(tagged("BIP0340/challenge", c)), N());
  const U256 s = addm(k, mulm(e, d, N()), N());
  const Bytes32 sb = to_be(s);
  for (int i = 0; i < 32; ++i) {
    sig[i] = rxb[i];
    sig[32 + i] = sb[i];
  }
  return verify(pxb, message, sig);
}

bool verify(const Bytes32& pubkey, const Bytes32& message, const std::array<u8, 64>& sig) {
  Pt p;
  if (!lift_x(from_be(pubkey), p)) return false;
  const Bytes32 rb = half(sig, 0);
  const U256 r = from_be(rb), s = from_be(half(sig, 1));
  if (cmp(r, P()) >= 0 || cmp(s, N()) >= 0) return false;
  std::vector<u8> c;
  append(c, rb);
  append(c, pubkey);
  append(c, message);
  const U256 e = reduce(from_be(tagged("BIP0340/challenge", c)), N());
  // R = sG - eP
  Pt ep = mul_pt(p, e);
  ep.y = fs(U256{}, ep.y);
  U256 x, y;
  if (!affine(add_pt(mul_pt(G(), s), ep), x, y)) return false;
  return (y.w[0] & 1) == 0 && cmp(x, r) == 0;
}

namespace {
constexpr char kHexDigits[] = "0123456789abcdef";
void hex_of(std::string& o, u8 byte) {
  o += kHexDigits[byte >> 4];
  o += kHexDigits[byte & 15];
}
}  // namespace

std::string hex(const Bytes32& data) {
  std::string o;
  for (u8 byte : data) hex_of(o, byte);
  return o;
}

std::string hex(const std::array<u8, 64>& data) {
  std::string o;
  for (u8 byte : data) hex_of(o, byte);
  return o;
}

bool from_hex(std::string_view s, Bytes32& out) {
  const std::size_t n = out.size();
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
  const std::string pkh = hex(pk), body = escape(content);
  char head[96];
  std::snprintf(head, sizeof head, "[0,\"%s\",%lld,%u,", pkh.c_str(), static_cast<long long>(created_at), kind);
  const std::string ser = std::string(head) + std::string(tags) + ",\"" + body + "\"]";
  const Bytes32 id = sha256(ser);
  std::array<u8, 64> sig;
  if (!sign(secret, id, aux, sig)) return false;
  char num[64];
  std::snprintf(num, sizeof num, ",\"created_at\":%lld,\"kind\":%u,\"tags\":", static_cast<long long>(created_at), kind);
  out = "{\"id\":\"" + hex(id) + "\",\"pubkey\":\"" + pkh + "\"" + num + std::string(tags) + ",\"content\":\"" + body + "\",\"sig\":\"" + hex(sig) + "\"}";
  return true;
}

}  // namespace sc::nostr
