// The suggestion box's signature: SHA-256 and the BIP-340 test vectors (bip-0340/test-vectors.csv 0 and 1).
// The signature is the level's Blueprint nostr_cpp, called as its class - as the program calls it
// (level_blueprints.h), with the game's loop guard.
#include <string>
#include <string_view>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Nostr = blueprints::nostr_cpp;
using Bytes32 = kern::Fixed<Nostr::U8, 32>;
using Bytes64 = kern::Fixed<Nostr::U8, 64>;
/// nostr_cpp running as its class in a logic of its own.
struct NostrBlueprint {
  kern::test::BlueprintRun rules{"nostr_cpp", nullptr, true};
  bool bound = false;
  NostrBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Nostr>(rules.rt);
  }
  Nostr& operator*() { return kern::logic::blueprint<Nostr>(); }
};
Bytes32 b32(Nostr& n, const char* h) { Bytes32 b{}; n.from_hex(b, std::string(h)); return b; }
std::string text(const kern::logic::Text& t) { return std::string(t.view()); }
}  // namespace

KERN_TEST(nostr_sha256) {
  NostrBlueprint bp;
  KERN_CHECK(bp.bound, "nostr_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Nostr& n = *bp;
  const Bytes32 a = n.sha256_2(std::string("abc")).Result;
  KERN_CHECK(text(n.hex(a).Result) == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", "sha256(abc)");
  const std::string long_text(1000, 'a');
  const Bytes32 l = n.sha256_2(long_text).Result;
  KERN_CHECK(text(n.hex(l).Result) == "41edece42d63e8d9bf515a9ba6932e1c20cbc9f5a5d134645adb5db1b9737ea3", "sha256 of 1000 a");
}

KERN_TEST(nostr_bip340_vectors) {
  NostrBlueprint bp;
  KERN_CHECK(bp.bound, "nostr_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Nostr& n = *bp;
  {
    Bytes32 pk;
    KERN_CHECK(n.public_key(b32(n, "0000000000000000000000000000000000000000000000000000000000000003"), pk).Result &&
                 text(n.hex(pk).Result) == "f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9", "vector 0 public key");
    Bytes64 sig;
    KERN_CHECK(n.the_sign(b32(n, "0000000000000000000000000000000000000000000000000000000000000003"), Bytes32{}, Bytes32{}, sig).Result &&
                 text(n.hex_2(sig).Result) == "e907831f80848d1069a5371b402410364bdf1c5f8307b0084c55f1ce2dca821525f66a4a85ea8b71e482a74f382d2ce5ebeee8fdb2172f477df4900d310536c0",
             "vector 0 signature %s", text(n.hex_2(sig).Result).c_str());
  }
  {
    const Bytes32 sk = b32(n, "B7E151628AED2A6ABF7158809CF4F3C762E7160F38B4DA56A784D9045190CFEF");
    const Bytes32 aux = b32(n, "0000000000000000000000000000000000000000000000000000000000000001");
    const Bytes32 msg = b32(n, "243F6A8885A308D313198A2E03707344A4093822299F31D0082EFA98EC4E6C89");
    Bytes32 pk;
    Bytes64 sig;
    KERN_CHECK(n.public_key(sk, pk).Result && text(n.hex(pk).Result) == "dff1d77f2a671c5f36183726db2341be58feae1da2deced843240f7b502ba659", "vector 1 public key");
    KERN_CHECK(n.the_sign(sk, msg, aux, sig).Result &&
                 text(n.hex_2(sig).Result) == "6896bd60eeae296db48a229ff71dfe071bde413e6d43f917dc8dcf8c78de33418906d11ac976abccb20b091292bff4ea897efcb639ea871cfa95f6de339e4b0a",
             "vector 1 signature");
    KERN_CHECK(n.verify(pk, msg, sig).Result, "verifies");
    sig[63] = Nostr::U8(static_cast<kern::u8>(static_cast<kern::u8>(sig[63]) ^ 1));
    KERN_CHECK(!n.verify(pk, msg, sig).Result, "a changed signature does not");
  }
  kern::logic::Text ev;
  KERN_CHECK(n.event_json(b32(n, "0000000000000000000000000000000000000000000000000000000000000003"), Bytes32{}, ev, Nostr::Int64{1700000000}, 30078,
                        std::string("[[\"t\",\"spawncity-suggestion\"]]"), std::string("Mehr \"Zombies\"\nbitte")).Result &&
               text(ev).find("\"kind\":30078") != std::string::npos && text(ev).find("Mehr \\\"Zombies\\\"\\nbitte") != std::string::npos &&
               text(ev).find("\"sig\":\"") != std::string::npos,
           "a signed event %s", text(ev).c_str());
}
