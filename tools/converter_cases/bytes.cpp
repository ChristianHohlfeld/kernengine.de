#include "bytes.h"
#include "kern/api/c_api.h"
namespace cases {
Bytes& bytes() { static Bytes b; return b; }
kern::u32 sum_of(kern::Span<const kern::u8> b) { kern::u32 s = 0; for (std::size_t i = 0; i < b.size; ++i) s += b[i]; return s; }
}
/// The host hands bytes: their sum and their count.
KERN_EXPORT(take_bytes) void take_bytes(const kern::u8* data, kern::u32 length) {
  cases::bytes().sum = cases::sum_of({data, length});
  cases::bytes().count = length;
}
