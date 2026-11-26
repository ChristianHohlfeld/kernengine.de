#include "program.h"
#include "kern/api/c_api.h"
namespace sc {
CasesApp& app() { static CasesApp a; return a; }
}
/// The host asks for a score: an export - it belongs to the program.
KERN_EXPORT(sc_case_score) kern::i32 sc_case_score(kern::i32 points) { return sc::scored(points); }
