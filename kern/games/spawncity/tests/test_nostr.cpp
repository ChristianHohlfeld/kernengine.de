// The suggestion box's signature: SHA-256 and the BIP-340 test vectors (bip-0340/test-vectors.csv 0 and 1).
#include <string>

#include "../src/nostr.h"
#include "test_util.h"

using namespace sc;
using namespace sc::nostr;

namespace {
Bytes32 b32(const char* h) { Bytes32 b{}; from_hex(h, b); return b; }
}  // namespace

SC_TEST(nostr_sha256) {
  const Bytes32 a = sha256("abc");
  SC_CHECK(hex(a) == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", "sha256(abc)");
  const std::string long_text(1000, 'a');
  const Bytes32 l = sha256(long_text);
  SC_CHECK(hex(l) == "41edece42d63e8d9bf515a9ba6932e1c20cbc9f5a5d134645adb5db1b9737ea3", "sha256 of 1000 a");
}

SC_TEST(nostr_bip340_vectors) {
  {
    Bytes32 pk;
    SC_CHECK(public_key(b32("0000000000000000000000000000000000000000000000000000000000000003"), pk) &&
                 hex(pk) == "f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9", "vector 0 public key");
    std::array<u8, 64> sig;
    SC_CHECK(sign(b32("0000000000000000000000000000000000000000000000000000000000000003"), Bytes32{}, Bytes32{}, sig) &&
                 hex(sig) == "e907831f80848d1069a5371b402410364bdf1c5f8307b0084c55f1ce2dca821525f66a4a85ea8b71e482a74f382d2ce5ebeee8fdb2172f477df4900d310536c0",
             "vector 0 signature %s", hex(sig).c_str());
  }
  {
    const Bytes32 sk = b32("B7E151628AED2A6ABF7158809CF4F3C762E7160F38B4DA56A784D9045190CFEF");
    const Bytes32 aux = b32("0000000000000000000000000000000000000000000000000000000000000001");
    const Bytes32 msg = b32("243F6A8885A308D313198A2E03707344A4093822299F31D0082EFA98EC4E6C89");
    Bytes32 pk;
    std::array<u8, 64> sig;
    SC_CHECK(public_key(sk, pk) && hex(pk) == "dff1d77f2a671c5f36183726db2341be58feae1da2deced843240f7b502ba659", "vector 1 public key");
    SC_CHECK(sign(sk, msg, aux, sig) &&
                 hex(sig) == "6896bd60eeae296db48a229ff71dfe071bde413e6d43f917dc8dcf8c78de33418906d11ac976abccb20b091292bff4ea897efcb639ea871cfa95f6de339e4b0a",
             "vector 1 signature");
    SC_CHECK(verify(pk, msg, sig), "verifies");
    sig[63] ^= 1;
    SC_CHECK(!verify(pk, msg, sig), "a changed signature does not");
  }
  std::string ev;
  SC_CHECK(event_json(b32("0000000000000000000000000000000000000000000000000000000000000003"), Bytes32{}, 1700000000, 30078, "[[\"t\",\"spawncity-suggestion\"]]",
                      "Mehr \"Zombies\"\nbitte", ev) &&
               ev.find("\"kind\":30078") != std::string::npos && ev.find("Mehr \\\"Zombies\\\"\\nbitte") != std::string::npos && ev.find("\"sig\":\"") != std::string::npos,
           "a signed event %s", ev.c_str());
}
