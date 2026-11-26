#include "logicarg_b.h"
namespace cases {
bool both_ready(kern::logic::Runtime& logic) { return ready_in(logic, 1) && ready_in(logic, 2); }
}
