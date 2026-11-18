// A shader extension made of plain data. The state of a look - Facades, WaterLook - is a class of
// the game with the functions of an extension (name(), attributes(), the pieces of shader text,
// bind(), frame_uniforms()) and no base class: it is state like any other state of the game. What
// the renderer takes, the engine's interface kern::ShaderExtension, is this adapter over it: every
// call of the renderer is the look's function of the same name.
#pragma once

#include "common.h"
#include "kern/gfx/shader.h"

namespace sc {

template <class Look> class LookExtension final : public kern::ShaderExtension {
 public:
  explicit LookExtension(Look& look) : look_(look) {}
  const char* name() const override { return look_.name(); }
  kern::Span<const kern::ExtensionAttribute> attributes() const override { return look_.attributes(); }
  const char* vertex_declarations() const override { return look_.vertex_declarations(); }
  const char* vertex() const override { return look_.vertex(); }
  const char* fragment_declarations() const override { return look_.fragment_declarations(); }
  const char* fragment_surface() const override { return look_.fragment_surface(); }
  void bind(const kern::Program& program) override { look_.bind(program); }
  void frame_uniforms(kern::UniformCache& cache, const kern::Renderer& renderer) override { look_.frame_uniforms(cache, renderer); }

 private:
  Look& look_;
};

}  // namespace sc
