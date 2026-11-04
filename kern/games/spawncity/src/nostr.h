// The suggestion box's Nostr event (suggestion_box.mjs): SpawnCity sends an idea anonymously as a
// signed Nostr event (kind 30078, application data) with a throw-away key to public relays. The
// signature is BIP-340 Schnorr over secp256k1; the browser has no secp256k1 (Web Crypto) and the
// page uses no third-party library, so SHA-256, the curve and the signature are here (small and
// slow on purpose: one signature per idea). The page opens the relay WebSockets.
// tests/test_nostr.cpp checks the BIP-340 test vectors.
#pragma once

#include <array>
#include <string>
#include <string_view>

#include "common.h"

namespace sc::nostr {

using Bytes32 = std::array<u8, 32>;

void sha256(const u8* data, std::size_t length, u8 out[32]);
Bytes32 sha256(std::string_view s);
/// x-only public key of a secret key (false for 0 or >= n).
bool public_key(const Bytes32& secret, Bytes32& out);
/// BIP-340 signature of a 32-byte message with auxiliary randomness.
bool sign(const Bytes32& secret, const Bytes32& message, const Bytes32& aux, std::array<u8, 64>& sig);
/// BIP-340 verification.
bool verify(const Bytes32& pubkey, const Bytes32& message, const std::array<u8, 64>& sig);

std::string hex(const u8* data, std::size_t n);
bool from_hex(std::string_view s, u8* out, std::size_t n);
/// NIP-01 string escaping (\n \" \\ \r \t \b \f).
std::string escape(std::string_view s);
/// A signed event as JSON. `tags` is the tags array as compact JSON; `content` the raw text.
bool event_json(const Bytes32& secret, const Bytes32& aux, i64 created_at, u32 kind, std::string_view tags, std::string_view content, std::string& out);

}  // namespace sc::nostr
