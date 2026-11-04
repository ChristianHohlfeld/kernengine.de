// Multiplayer art (procedural, instanced Kern meshes): ping markers in the player's colour
// (ping_marker.mjs: a slim post, a diamond on top, a ring on the ground), remote tracers and
// explosion flashes (vs_multiplayer.mjs fx pools), the colour accents on a mate's drone
// (peer_drone_model.mjs: canopy and LED ring in his colour) and the spawn-protection shimmer.
#pragma once

#include <vector>

#include "common.h"
#include "kern/gfx/renderer.h"

namespace sc {

class MpArt {
 public:
  bool init(kern::Renderer& r);
  void begin();
  /// A ping at `foot` (scale grows with distance so it stays readable), fade 0..1, bob/spin time.
  void ping(Vec3 foot, f32 scale, Vec3 color, f32 fade, f32 now);
  /// A tracer segment (centre, direction, length) in a player's colour.
  void tracer(Vec3 at, Vec3 dir, f32 length, Vec3 color);
  /// A growing, fading fire ball (t 0..1 over its life).
  void flash(Vec3 at, f32 radius, f32 t);
  /// Canopy and LED ring of a mate's drone (body frame as the drone art).
  void drone_accent(const kern::Mat4& body, Vec3 color);
  /// A bubble around a protected player.
  void shield(Vec3 center, f32 radius, f32 now);
  void end();
  bool ready() const { return r_ != nullptr; }

 private:
  struct Part { u32 id = 0; std::vector<f32> transforms, tints; };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m);
  void put(Part& p, const kern::Mat4& m, Vec3 tint);
  kern::Renderer* r_ = nullptr;
  Part post_, gem_, ring_, tracer_, flash_, canopy_, led_, shield_;
};

}  // namespace sc
