#pragma once
#include "kern/app/app.h"
#include "kern/math/math.h"
namespace cases {
/// The program's App (an object of the engine's interface kern::App): its one object is made in the entry kern_main.
class CasesApp final : public kern::App {
 public:
  bool start(kern::Engine&) override { return true; }
  void update(kern::Engine&, const kern::Frame&) override { frames = frames + 1; }
  kern::i32 frames = 0;
};
/// What the export computes with: a library the program uses.
inline kern::i32 scored(kern::i32 points) { return points * 3 + 1; }
}
