#pragma once
#include "kern/gfx/shader.h"
namespace cases {
/// A shader extension of the program, its one object behind a getter (as a game's looks are): the engine
/// object of its Blueprint (kern/docs/engine_object.md).
class Look final : public kern::ShaderExtension {
 public:
  const char* name() const override { return "cases-look"; }
};
Look& look();
}
