// SpawnCity on Kern - entry point (kern::App) and the game's own exports for the host page
// (kern/games/spawncity/web/host.mjs). The page stays a thin host: it fetches map tiles, polls the
// gamepad, routes touch fingers, moves audio samples and draws the HUD JSON; all game logic runs
// here in C++.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <cstring>
#include <string>

#include "game.h"
#include "kern/api/c_api.h"
#include "kern/kern.h"

namespace {

class SpawnCityApp final : public kern::App {
 public:
  kern::RendererOptions configure() override {
    kern::RendererOptions o;
    o.shadows = true;
    o.shadow.resolution = 2048;  // stylized_world_style.mjs: 2048, mobile 1024 (the page passes ?mobile=1)
    o.shadow.extent = 110;
    o.shadow.bias = 0.0014f;
    o.bloom.enabled = true;
    return o;
  }
  kern::Span<kern::ShaderExtension* const> extensions() override {
    static kern::ShaderExtension* ext[1] = {&sc::game().windows()};
    return {ext, 1};
  }
  bool start(kern::Engine& e) override {
    const bool mobile = e.arg_number("mobile", 0) > 0;
    if (mobile) {
      kern::ShadowOptions s;
      s.resolution = 1024;
      s.extent = 70;
      s.bias = 0.0016f;
      s.filter = 4;
      e.renderer.set_shadows(&s);
      kern::AoOptions ao;
      ao.quality = kern::AoQuality::low;
      ao.half_resolution = true;
      e.renderer.set_ambient_occlusion(ao);
    } else {
      kern::AoOptions ao;
      ao.quality = kern::AoQuality::medium;
      ao.half_resolution = false;
      e.renderer.set_ambient_occlusion(ao);
    }
    return sc::game().start(e);
  }
  void update(kern::Engine& e, const kern::Frame& f) override { sc::game().update(e, f); }
  void stop(kern::Engine&) override { sc::game().stop(); }

};

std::string g_result;
const char* result(std::string s) {
  g_result = std::move(s);
  return g_result.c_str();
}

}  // namespace

kern::App* kern_create_app() {
  static SpawnCityApp app;
  return &app;
}

/// Length of the last string an sc_* export returned.
KERN_EXPORT(sc_result_length) kern::u32 sc_result_length() { return static_cast<kern::u32>(g_result.size()); }

// ---- map tiles: the page fetches what the game asks for ----
KERN_EXPORT(sc_tile_requests) const char* sc_tile_requests() { return result(sc::game().city().take_requests()); }
KERN_EXPORT(sc_tile_data) void sc_tile_data(kern::i32 z, kern::i32 x, kern::i32 y, const kern::u8* bytes, kern::u32 length) {
  sc::game().city().provide(sc::TileKey{z, x, y}, bytes, length);
}
KERN_EXPORT(sc_tile_failed) void sc_tile_failed(kern::i32 z, kern::i32 x, kern::i32 y) { sc::game().city().fail(sc::TileKey{z, x, y}); }
KERN_EXPORT(sc_offline) void sc_offline() { sc::game().city().go_offline(); }

// ---- input the App host does not forward ----
KERN_EXPORT(sc_gamepad) void sc_gamepad(kern::i32 connected, kern::f32 lx, kern::f32 ly, kern::f32 rx, kern::f32 ry, kern::f32 lt, kern::f32 rt, kern::u32 buttons,
                                        kern::i32 active) {
  sc::PadState& p = sc::game().pad;
  p.pressed = buttons & ~p.buttons;
  p.connected = connected != 0;
  p.lx = lx; p.ly = ly; p.rx = rx; p.ry = ry; p.lt = lt; p.rt = rt;
  p.buttons = buttons;
  p.active = active != 0;
}
/// Viewport in CSS px, safe-area insets and the device pixel ratio.
KERN_EXPORT(sc_touch_frame) void sc_touch_frame(kern::f32 w, kern::f32 h, kern::f32 sl, kern::f32 st, kern::f32 sr, kern::f32 sb, kern::f32 dpr) {
  sc::game().set_touch_frame(w, h, sl, st, sr, sb, dpr);
}
/// A touch / pen finger (type 0 down, 1 move, 2 up, 3 cancel; CSS px). Returns 1 when used.
KERN_EXPORT(sc_touch) kern::i32 sc_touch(kern::u32 type, kern::i32 id, kern::f32 x, kern::f32 y, kern::f64 t) {
  return sc::game().touch(type, id, x, y, t) ? 1 : 0;
}
KERN_EXPORT(sc_touch_view) const char* sc_touch_view() { return result(sc::game().touch_view_json()); }

// ---- presentation ----
KERN_EXPORT(sc_hud) const char* sc_hud() { return result(sc::game().hud_json()); }
KERN_EXPORT(sc_command) const char* sc_command(const char* text, kern::u32 length) { return result(sc::game().command(std::string(text, length))); }

// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
KERN_EXPORT(sc_audio_init) kern::i32 sc_audio_init(kern::u32 rate) { return sc::game().audio_init(rate) ? 1 : 0; }
KERN_EXPORT(sc_audio_render) const kern::f32* sc_audio_render(kern::u32 frames) { return sc::game().audio_render(frames); }
