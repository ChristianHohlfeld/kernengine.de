#pragma once
#include <memory>
#include <vector>
#include "kern/audio/audio.h"
namespace sc {
/// A value that holds an object of the engine by a pointer (a field of a struct).
struct Rig {
  kern::audio::Mixer* mixer = nullptr;
  int volume = 0;
};
/// A state that holds objects of the engine: in a list of pointers, owned (std::unique_ptr), in a field of a struct.
class Board {
 public:
  /// An object of the engine as an input; `if (p)`: whether there is one.
  void add(kern::audio::Mixer* m) {
    if (m) mixers_.push_back(m);
  }
  /// ... as what a function gives: the first of the list, or none (nullptr).
  kern::audio::Mixer* first() const { return mixers_.empty() ? nullptr : mixers_[0]; }
  /// How many the list holds.
  int count() const { return static_cast<int>(mixers_.size()); }
  /// The one it owns (unique_ptr<...>::get()), and whether it owns one (against nullptr).
  kern::audio::Mixer* own() const { return own_.get(); }
  bool owns() const { return own_.get() != nullptr; }
  /// Is the first of the list the one it owns (two objects compared)?
  bool first_is_own() const { return first() == own(); }
  /// An object of the engine in a field of a struct: set and read.
  void hold(const Rig& r) { rig_ = r; }
  kern::audio::Mixer* rigged() const { return rig_.mixer; }
  /// A function of the engine called on an object that may be none: its rate, 0 for none.
  int rate_of(const kern::audio::Mixer* m) const { return m != nullptr ? static_cast<int>(m->sample_rate()) : 0; }
 private:
  std::vector<kern::audio::Mixer*> mixers_;
  std::unique_ptr<kern::audio::Mixer> own_;
  Rig rig_;
};
Board& board();
}
