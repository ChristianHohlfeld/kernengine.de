// Optional native HIP acceleration of the Lehrer's large dense operations.
// No GPU dependency in the normal C++ / WASM build.
#pragma once
#include "kern/core/base.h"
namespace kern::lehrer {
#ifdef KERN_LEHRER_HIP
int hip_lehrer_devices();
bool hip_lehrer_check();
void hip_lehrer_dense(const f32* x, i32 rows, i32 in, i32 out, const f32* w, const f32* b, f32* y);
void hip_lehrer_gradient(const f32* x, const f32* d, i32 rows, i32 in, i32 out, f32* gw, f32* gb);
#endif
}
