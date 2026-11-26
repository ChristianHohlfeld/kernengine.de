#include "table.h"
namespace cases::format {
kern::f32 width_and_glock(Spec s) { return static_cast<kern::f32>(s.width) + weapons::spec(weapons::Id::glock).interval_ms; }
}
