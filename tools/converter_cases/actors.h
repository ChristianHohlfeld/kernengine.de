#pragma once
#include <vector>
#include "kern/logic/logic.h"
namespace sc {
/// A state that keeps actors (kern::logic::ActorHandle: the Blueprint's Actor - its id and its world, kern/docs/project.md 7).
class Crowd {
 public:
  /// An Actor as an input; against none (Same Actor with Actor{}): whether it is one.
  void add(kern::logic::ActorHandle a) {
    if (!(a == kern::logic::ActorHandle{})) members_.push_back(a);
  }
  /// ... as what a function gives: the first, or none.
  kern::logic::ActorHandle first() const { return members_.empty() ? kern::logic::ActorHandle{} : members_[0]; }
  /// Two compared: the same world and id (Same Actor).
  bool has(kern::logic::ActorHandle a) const {
    for (const kern::logic::ActorHandle& m : members_)
      if (m == a) return true;
    return false;
  }
  /// Its id: the one way to a number (Id of Actor).
  long long first_id() const { return members_.empty() ? 0 : static_cast<long long>(members_[0].id); }
  int count() const { return static_cast<int>(members_.size()); }
 private:
  std::vector<kern::logic::ActorHandle> members_;
};
Crowd& crowd();
/// An Actor made of a number: no Blueprint makes one - it waits, with the reason.
inline kern::logic::ActorHandle made_of(unsigned id) { return kern::logic::ActorHandle{id, 1}; }
}
