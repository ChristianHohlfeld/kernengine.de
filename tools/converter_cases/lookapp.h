#pragma once
#include "kern/app/app.h"
#include "look.h"
namespace cases {
/// An App whose field starts as the engine object of another Blueprint (`&cases::look()`, the extensions
/// of a game's App): an Object<kern::ShaderExtension>, set where C++ makes the object - with no constructor written.
class LookApp final : public kern::App {
 public:
  kern::Span<kern::ShaderExtension* const> extensions() override { return {ext_, 1}; }
  bool start(kern::Engine&) override { return true; }
  void update(kern::Engine&, const kern::Frame&) override {}
 private:
  kern::ShaderExtension* ext_[1] = {&look()};
};
LookApp& look_app();
}
