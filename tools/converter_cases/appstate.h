#pragma once
#include <string>
#include <vector>
#include "kern/app/app.h"
namespace cases {
/// The program's App that keeps the game's state itself (kern::App - its one object made by `static T x;` in spiel()), and
/// the exports that reach that state (appstate.cpp): functions of the App's Blueprint, as the App's own methods are.
class Spiel final : public kern::App {
 public:
  bool start(kern::Engine&) override {
    score_ = 0;
    return true;
  }
  void update(kern::Engine&, const kern::Frame&) override { score_ = score_ + 1; }
  kern::i32 score() const { return score_; }
  void add(kern::i32 n) { score_ = score_ + n; }
  std::string label() const { return score_ > 3 ? std::string("viel") : std::string("wenig"); }
  /// What the host takes (and so is gone): a text of it, the state changed.
  std::string take_label() {
    const std::string l = label();
    score_ = 0;
    return l;
  }
  std::vector<kern::f32> samples(kern::u32 n) const {
    std::vector<kern::f32> out;
    for (kern::u32 k = 0; k < n % 5; ++k) out.push_back(static_cast<kern::f32>(score_) * 0.5f);
    return out;
  }

 private:
  kern::i32 score_ = 0;
};
Spiel& spiel();
}
