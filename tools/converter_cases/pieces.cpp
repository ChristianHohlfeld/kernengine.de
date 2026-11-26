#include "pieces.h"
#include "kern/api/c_api.h"
#include "kern/logic/blueprint_class.h"
namespace cases {
std::string label_of(kern::i32 k) { return k < 0 ? std::string("minus") : std::string(static_cast<std::size_t>(k % 7), 'x'); }
std::vector<kern::u8> bytes_of(kern::i32 k) { return {static_cast<kern::u8>(k), static_cast<kern::u8>(k >> 8)}; }
}
/// Exports that give a text and bytes by a pointer and their length (out_len: kern::logic::export_text, export_bytes).
KERN_EXPORT(case_label) const char* case_label(kern::i32 k, kern::u32* out_len) { return kern::logic::export_text<&case_label>(cases::label_of(k), out_len); }
KERN_EXPORT(case_bytes) const kern::u8* case_bytes(kern::i32 k, kern::u32* out_len) { return kern::logic::export_bytes<&case_bytes>(cases::bytes_of(k), out_len); }
