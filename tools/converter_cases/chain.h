#pragma once
#include "kern/logic/logic.h"
namespace cases {
/// A state whose methods stand in two files (two parts of its Blueprint) and call each other in a chain:
/// root (here) <- step1 (chain_b.cpp) <- step2 (here) <- step3 (chain_b.cpp) <- step4 (here). root waits for a
/// reason of its own (an Actor made of a number); every one that calls it, itself or through others, waits too.
class Chain {
 public:
  kern::logic::ActorHandle root(unsigned id) const;
  long long step1(unsigned id) const;
  long long step2(unsigned id) const;
  long long step3(unsigned id) const;
  long long step4(unsigned id) const;
  /// Calls nothing that waits: written.
  long long free_of_it(long long v) const { return v + base_; }
 private:
  long long base_ = 3;
};
Chain& chain();
}
