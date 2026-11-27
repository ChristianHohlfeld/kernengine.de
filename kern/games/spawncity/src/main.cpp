// SpawnCity on Kern - the entry (kern::run: the App and the project the game carries) and the game's own exports for
// the host page (kern/games/spawncity/web/host.mjs). The page stays a thin host: it fetches map tiles, polls the
// gamepad, routes touch fingers, moves audio samples and draws the HUD JSON; the game runs here.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <string>

#include "spiel.h"
#include "kern/api/c_api.h"
#include "kern/game/embedded.h"  // (the project the game carries: kern::run)
#include "kern/game/program.h"  // (kern::run(App&, const Project&): the program beside the App)
#include "kern/logic/blueprint_class.h"  // (what an export gives: kern::logic::export_text, export_floats)
#include "kern/kern.h"

namespace sc {

// The game is the App the engine runs (kern::App, spiel.h): game(). (The city's
// shader extensions - the facades' wall records, the water - are out of the small game for now: an App's extensions
// are pointers to objects of the engine, which a Blueprint cannot hand on yet - BLUEPRINTS.md, "Herausgenommen am
// 26.11.".)
}  // namespace sc

// The program's entry (kern/docs/project.md 4.): the App, with the project the game carries - its settings (the
// renderer, the device profile "mobile"), its level.
int kern_main() { return kern::run(sc::game(), kern::game::embedded_project()); }

namespace sc {

// The exports, in the program's namespace (C names all the same - extern "C").
// (a text or floats an export gives: a piece {pointer, length} of that export's own place - kern::logic::export_text /
// export_floats; the pointer is returned, the length written to out_len, a place the page gives)

// ---- map tiles: the page fetches what the game asks for ----
KERN_EXPORT(sc_tile_requests) const char* sc_tile_requests(u32* out_len) { return kern::logic::export_text<&sc_tile_requests>(game().city().take_requests(), out_len); }
KERN_EXPORT(sc_tile_data) void sc_tile_data(i32 z, i32 x, i32 y, const u8* bytes, u32 length) {
  game().city().provide(TileKey{z, x, y}, {bytes, length});
}
KERN_EXPORT(sc_tile_failed) void sc_tile_failed(i32 z, i32 x, i32 y) { game().city().fail(TileKey{z, x, y}); }
KERN_EXPORT(sc_offline) void sc_offline() { game().city().go_offline(); }

// ---- elevation tiles (Terrarium PNGs): the page decodes the image, the game the heights ----
KERN_EXPORT(sc_dem_requests) const char* sc_dem_requests(u32* out_len) { return kern::logic::export_text<&sc_dem_requests>(game().city().terrain().take_requests(), out_len); }
/// RGBA pixels of a requested elevation tile (`length` = width x height x 4 bytes, copied).
KERN_EXPORT(sc_dem_data) void sc_dem_data(i32 z, i32 x, i32 y, const u8* rgba, u32 length, u32 width, u32 height) {
  game().city().terrain().provide(TileKey{z, x, y}, {rgba, length}, width, height);
}
KERN_EXPORT(sc_dem_failed) void sc_dem_failed(i32 z, i32 x, i32 y) { game().city().terrain().fail(TileKey{z, x, y}); }

// ---- aerial imagery for the ground colour (world_ground.mjs): the page decodes, the game averages ----
KERN_EXPORT(sc_image_requests) const char* sc_image_requests(u32* out_len) { return kern::logic::export_text<&sc_image_requests>(game().city().terrain().take_image_requests(), out_len); }
/// RGBA pixels of a requested aerial tile (`length` = width x height x 4 bytes, copied).
KERN_EXPORT(sc_image_data) void sc_image_data(i32 z, i32 x, i32 y, const u8* rgba, u32 length, u32 width, u32 height) {
  game().city().terrain().provide_image(TileKey{z, x, y}, {rgba, length}, width, height);
}
KERN_EXPORT(sc_image_failed) void sc_image_failed(i32 z, i32 x, i32 y) { game().city().terrain().fail_image(TileKey{z, x, y}); }

// ---- input the App host does not forward ----
KERN_EXPORT(sc_gamepad) void sc_gamepad(i32 connected, f32 lx, f32 ly, f32 rx, f32 ry, f32 lt, f32 rt, u32 buttons, i32 active) {
  // (the pad of the game's one object, named where it is set: its fields)
  game().pad.pressed = buttons & ~game().pad.buttons;
  game().pad.connected = connected != 0;
  game().pad.lx = lx; game().pad.ly = ly; game().pad.rx = rx; game().pad.ry = ry; game().pad.lt = lt; game().pad.rt = rt;
  game().pad.buttons = buttons;
  game().pad.active = active != 0;
}
/// Mouse movement under the pointer lock (movementX/Y of mousemove, CSS px, clamped +-420 per event by the page:
/// desktop_controls.mjs MAX_EVENT_PX) and the lock state.
KERN_EXPORT(sc_mouse) void sc_mouse(f32 dx, f32 dy) { game().mouse_move(dx, dy); }
KERN_EXPORT(sc_pointer_lock) void sc_pointer_lock(i32 locked) { game().pointer_locked(locked != 0); }
/// Viewport in CSS px, safe-area insets and the device pixel ratio.
KERN_EXPORT(sc_touch_frame) void sc_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32 dpr) {
  game().set_touch_frame(w, h, sl, st, sr, sb, dpr);
}
/// A touch / pen finger (type 0 down, 1 move, 2 up, 3 cancel; CSS px; `t` the time in ms, not needed). Returns 1 when used.
KERN_EXPORT(sc_touch) i32 sc_touch(u32 type, i32 id, f32 x, f32 y, f64 t) {
  (void)t;
  return game().touch(type, id, x, y) ? 1 : 0;
}
KERN_EXPORT(sc_touch_view) const char* sc_touch_view(u32* out_len) { return kern::logic::export_text<&sc_touch_view>(game().touch_view_json(), out_len); }

// ---- presentation ----
KERN_EXPORT(sc_hud) const char* sc_hud(u32* out_len) { return kern::logic::export_text<&sc_hud>(game().hud_json(), out_len); }
KERN_EXPORT(sc_command) const char* sc_command(const char* text, u32 length, u32* out_len) { return kern::logic::export_text<&sc_command>(game().command(std::string(text, length)), out_len); }

// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
KERN_EXPORT(sc_audio_init) i32 sc_audio_init(u32 rate) { return game().audio_init(rate) ? 1 : 0; }
KERN_EXPORT(sc_audio_render) const f32* sc_audio_render(u32 frames, u32* out_len) { return kern::logic::export_floats<&sc_audio_render>(game().audio_render(frames), out_len); }

}  // namespace sc
