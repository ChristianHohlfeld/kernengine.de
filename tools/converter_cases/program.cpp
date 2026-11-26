#include "program.h"
#include "kern/api/c_api.h"
#include "kern/app/run.h"
/// The host asks for a score: an export - it belongs to the program.
KERN_EXPORT(case_score) kern::i32 case_score(kern::i32 points) { return cases::scored(points); }
// The program's entry as kern/docs/project.md 4. writes it: the App's one object is made here, outside every namespace.
int kern_main() { static cases::CasesApp app; return kern::run(app); }
