#include "far_b.h"
#include "far_rules.h"
namespace cases::far {
namespace {
rules::Blueprint g_rules{"Lamps"};
}
float pole_radius() { return static_cast<float>(g_rules.get<double>("PoleRadius")); }
}
