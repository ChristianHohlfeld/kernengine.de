#include "rootc.h"
#include <cmath>
namespace cases {
f32 arm(f32 span) { return span / (2.0f * std::sqrt(2.0f)); }
f64 arm64(f64 span) { return span / (2.0 * std::sqrt(2.0)) + std::sqrt(3.0f); }
}
