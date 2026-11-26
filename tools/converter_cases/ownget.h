#pragma once
#include <memory>
#include "kern/game/runtime.h"
namespace cases {
/// A state that holds a world of the engine as its own (std::unique_ptr).
class Holder {
 public:
  /// The world it holds, by its handle (unique_ptr::get()).
  const kern::game::GameWorld* world() const { return world_.get(); }
  /// Whether it holds one.
  bool has_world() const { return world() != nullptr; }
 private:
  std::unique_ptr<kern::game::GameWorld> world_;
};
Holder& holder();
}
