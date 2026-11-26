// The standard library's mathematics is the engine's, in WebAssembly too (math_names.h): built as a module of its own with
// kern/build.sh (wasi-libc's libm is below the engine's musl there), run by math_names_wasm_test.mjs.
#include "kern/api/c_api.h"
#include "math_names.h"

namespace {
math_names::Result g_result;
}
KERN_EXPORT(math_names_run) int math_names_run() { g_result = math_names::run(); return g_result.differ; }
KERN_EXPORT(math_names_compared) int math_names_compared() { return g_result.compared; }
KERN_EXPORT(math_names_digest_high) unsigned math_names_digest_high() { return static_cast<unsigned>(g_result.digest >> 32); }
KERN_EXPORT(math_names_digest_low) unsigned math_names_digest_low() { return static_cast<unsigned>(g_result.digest); }
KERN_EXPORT(math_names_first) const char* math_names_first() { return g_result.first.c_str(); }
