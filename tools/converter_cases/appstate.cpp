#include "appstate.h"
#include "kern/api/c_api.h"
#include "kern/app/run.h"
#include "kern/logic/blueprint_class.h"
namespace cases {
Spiel& spiel() {
  static Spiel s;
  return s;
}
}
/// The host hands points to the game and asks for its score: exports that reach the App's state.
KERN_EXPORT(case_add) void case_add(kern::i32 n) { cases::spiel().add(n); }
KERN_EXPORT(case_score) kern::i32 case_score() { return cases::spiel().score(); }
/// A text and floats the export gives by a pointer and their length (out_len) - of the App's state.
KERN_EXPORT(case_label) const char* case_label(kern::u32* out_len) { return kern::logic::export_text<&case_label>(cases::spiel().label(), out_len); }
KERN_EXPORT(case_samples) const kern::f32* case_samples(kern::u32 n, kern::u32* out_len) { return kern::logic::export_floats<&case_samples>(cases::spiel().samples(n), out_len); }
KERN_EXPORT(case_take) const char* case_take(kern::u32* out_len) { return kern::logic::export_text<&case_take>(cases::spiel().take_label(), out_len); }
int kern_main() { return kern::run(cases::spiel()); }
