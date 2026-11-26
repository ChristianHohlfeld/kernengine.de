// Kern application framework: write a game in C++ against Engine/App and the
// platform layer (kern/web/app.mjs) runs it - frame loop, drawing-buffer
// size, input, context loss and a status-text channel are handled for you.
//
//   class MyGame final : public kern::App {
//     bool start(kern::Engine& engine) override { ...create meshes... }
//     void update(kern::Engine& engine, const kern::Frame& frame) override { ...move things... }
//   };
//   int kern_main() { static MyGame game; return kern::run(game); }
//
// kern_main() is the program's one entry (kern/app/run.h, kern/docs/project.md 4.): the platform layer calls it once,
// before the app starts; it hands the engine the App (kern::run - or kern::run(project) of kern/game/program.h for a
// game whose App is a Blueprint of its project). update() runs once per display frame; the engine renders right
// after it.
#pragma once

#include <string>
#include <utility>
#include <vector>

#include "kern/app/storage.h"
#include "kern/audio/audio.h"
#include "kern/core/json.h"
#include "kern/gfx/renderer.h"
#include "kern/input/actions.h"

namespace kern {

namespace game { struct Project; }

constexpr u32 kMaxTouches = 10;

struct KERN_BLUEPRINT_TYPE Touch {
  i32 id = -1;  // < 0: slot unused
  Vec2 start, position, previous;
  bool began = false;  // it touched down this frame
};

/// A gamepad in the browser's standard mapping (kern/docs/app.md 4.): sticks -1..1 (right and up are +1), the
/// triggers 0..1, the buttons as bits (bit i = PadButton i).
struct KERN_BLUEPRINT_TYPE Gamepad {
  bool connected = false;
  f32 lx = 0, ly = 0, rx = 0, ry = 0;
  f32 lt = 0, rt = 0;
  u32 buttons = 0;
};

class Input {
 public:
  // (a key past the last one - Key::count, or any value of the type beyond it - is never down)
  bool down(Key k) const { return state(k) & kDown; }
  bool pressed(Key k) const { return state(k) & kPressed; }
  bool released(Key k) const { return state(k) & kReleased; }
  /// Pointer position in drawing-buffer pixels, movement since the last frame (pixels, includes locked movement).
  Vec2 pointer() const { return pointer_; }
  Vec2 pointer_delta() const { return pointer_delta_; }
  bool button(u32 b) const { return buttons_ & (1u << b); }
  bool button_pressed(u32 b) const { return pressed_buttons_ & (1u << b); }
  f32 wheel() const { return wheel_; }
  const Touch* touches() const { return touches_; }
  u32 touch_count() const;
  bool focused() const { return focused_; }
  /// The gamepad (the first connected one, standard mapping) and the buttons pressed down this frame (bits).
  const Gamepad& gamepad() const { return pad_; }
  bool pad_down(PadButton b) const { return pad_.buttons & (1u << static_cast<u32>(b)); }
  bool pad_pressed(PadButton b) const { return pad_pressed_ & (1u << static_cast<u32>(b)); }

  // ---- platform side ----
  void key(Key k, bool down);
  void pointer_move(Vec2 position, Vec2 delta);
  void pointer_buttons(u32 buttons);
  void add_wheel(f32 dy) { wheel_ += dy; }
  void touch(i32 id, i32 phase, Vec2 position);  // phase 0 start, 1 move, 2 end/cancel
  void set_focus(bool focused);
  void set_gamepad(const Gamepad& pad);
  void end_frame();  // clears per-frame edges and deltas

 private:
  enum : u8 { kDown = 1, kPressed = 2, kReleased = 4 };
  u8 keys_[static_cast<u32>(Key::count)] = {};
  u8 state(Key k) const { return static_cast<u32>(k) < static_cast<u32>(Key::count) ? keys_[static_cast<u32>(k)] : 0; }
  Vec2 pointer_, pointer_delta_;
  u32 buttons_ = 0, pressed_buttons_ = 0;
  f32 wheel_ = 0;
  Touch touches_[kMaxTouches];
  bool focused_ = true;
  Gamepad pad_;
  u32 pad_pressed_ = 0;
};

struct KERN_BLUEPRINT_TYPE Frame {
  f64 time = 0;   // seconds since start
  f32 dt = 0;     // seconds since the previous frame (clamped to 0.1)
  u64 index = 0;
};

/// What the engine asks of the place it runs in (kern/docs/app.md 1.): the page of a game - the platform's
/// (platform_app_host: kern/web/app.mjs through kern_env) - or the editor that plays a project in its viewport
/// (kern/src/studio, docs/studio.md "Play Project"). The one way the engine reaches out of the program: nothing calls
/// the platform past it. (The slots of a save have a host of their own: Engine::set_storage, kern/app/storage.h.)
class AppHost {
 public:
  virtual ~AppHost() = default;
  /// Capture (true) or release the mouse for mouse-look controls.
  virtual void pointer_lock(bool lock) = 0;
  /// The frame's widgets, after each frame in which they changed (JSON, kern/docs/app.md 6.).
  virtual void widgets(std::string_view json) = 0;
};
/// The platform's: the page (kern_env pointer_lock and ui); natively nothing - a test reads Engine::ui_json.
AppHost& platform_app_host();

class Engine {
 public:
  explicit Engine(Renderer& renderer) : renderer_(renderer), host_(&platform_app_host()) {}
  /// An engine in another host than the platform's (the editor's Play Project): `host` must outlive it.
  Engine(Renderer& renderer, AppHost& host) : renderer_(renderer), host_(&host) {}
  /// The host it runs in (platform_app_host unless it was made with one).
  AppHost& host() const { return *host_; }
  /// The renderer and the input of the frame: objects of the engine (a Blueprint reaches them by these).
  Renderer& renderer() const { return renderer_; }
  Input& input() { return input_; }
  const Input& input() const { return input_; }
  /// The project of the program this engine runs (kern::run(App&, const Project&) or kern::run(Project),
  /// kern/game/program.h) - null for an App without a project (kern::run(App&, settings)). A world the program makes
  /// from its Engine has it (GameWorld(Engine&)): the program never hands it on itself.
  const game::Project* project() const { return project_; }
  /// Program side (kern/game/program.h, while its App runs): the project of the program.
  void set_project(const game::Project* project) { project_ = project; }
  /// The device profile the program runs with (the launch argument profile=<name>, kern/app/project.h; "": none) - its
  /// renderer started the renderer, its world settings lie on a level a world of the program starts (GameWorld(Engine&)).
  const std::string& profile() const { return profile_; }
  void set_profile(std::string name) { profile_ = std::move(name); }
  /// The app's one mixer (kern/docs/app.md 3.): ready from the App's start at the device's rate; the app runtime
  /// makes it heard (kern/web/app.mjs pulls it). A game fills it - GameAudio(engine.mixer(), bank) - and nothing else.
  audio::Mixer& mixer() { return mixer_; }
  /// The project's input actions (kern/docs/app.md 4.): `action` held, `action_pressed` pressed down this frame (a key,
  /// a pad button, a mouse button or a finger of its touch zone), `axis` -1..1 (keys -1/+1, the pad's stick past its
  /// deadzone, the touch stick - the largest wins). A name the project has none of: false / 0, last_error names it and
  /// the names there are, the log says it once a name.
  bool action(const char* name) const;
  bool action_pressed(const char* name) const;
  f32 axis(const char* name) const;
  /// The actions the app runtime gives the engine (the project's settings "input").
  void set_actions(const InputActions& actions) { actions_ = actions; }
  const InputActions& actions() const { return actions_; }
  /// Saving and loading (kern/docs/app.md 5.): named slots of named values (a slot and a key: 1..64 of A-Z a-z 0-9
  /// _ - .). A save writes the slot to the host at once; false: last_error says why (a name, a value that is none, the
  /// host, a slot that is no kern-save). A load gives the value, or `fallback` when there is none - or, with the
  /// reason in last_error, when it is of another type or the slot cannot be read.
  bool save_int(const char* slot, const char* key, i64 value);
  bool save_float(const char* slot, const char* key, f64 value);
  bool save_bool(const char* slot, const char* key, bool value);
  bool save_text(const char* slot, const char* key, const char* value);
  i64 load_int(const char* slot, const char* key, i64 fallback);
  f64 load_float(const char* slot, const char* key, f64 fallback);
  bool load_bool(const char* slot, const char* key, bool fallback);
  /// (the text lives until the next load_text)
  const char* load_text(const char* slot, const char* key, const char* fallback);
  /// Whether the slot holds a value of that key.
  bool saved(const char* slot, const char* key);
  /// Removes the whole slot from the host.
  bool erase_saved(const char* slot);
  /// The host of the slots (default: the platform's, kern/app/storage.h - natively the folder of the launch parameter
  /// saves=<folder>, else kern-saves). The slots read so far are read again from it.
  void set_storage(StorageHost& host);
  /// The status line (kern/docs/app.md 6.): a text widget "status" at the top left that stays until it changes
  /// (empty: gone) - the widgets' one way for a lasting line (loading, an error before the first frame).
  void set_status(const char* text);
  /// Widgets, immediate mode (kern/docs/app.md 6.): named in every frame they are seen; x, y 0..1 in the view (0,0
  /// top left), the widget hung by the same point of itself. An id: 1..64 characters, once a frame; a widget that is
  /// none (an id, a place not finite) is not shown and last_error says why.
  void ui_text(const char* id, const char* text, f32 x, f32 y);
  /// A bar: `value` 0..1, `width` a share of the view's width.
  void ui_bar(const char* id, const char* label, f32 value, f32 x, f32 y, f32 width);
  /// A button: true in the frame after it was clicked or tapped.
  bool ui_button(const char* id, const char* label, f32 x, f32 y);
  /// A menu: `items` one a line ('\n'); the index chosen in the frame after the choice, else -1.
  i32 ui_menu(const char* id, const char* items, f32 x, f32 y);
  /// The widgets as the page got them last (JSON) - natively, what a test reads.
  const std::string& ui_json() const { return ui_sent_; }
  // ---- platform side (the app runtime) ----
  /// An event of the page for the next frame: a button (index 0) or a menu's entry.
  void ui_event(std::string_view id, i32 index);
  void ui_begin();
  /// The frame's widgets to the page, when they changed.
  void ui_end();
  /// Asks the host to capture (or release) the mouse for mouse-look controls.
  void request_pointer_lock(bool lock);
  /// Launch parameter `key` from the host (the page's query string by default), or `fallback`.
  const char* arg(const char* key, const char* fallback = nullptr) const;
  /// Whether the host gave the launch parameter `key` (also when it is empty).
  bool has_arg(const char* key) const;
  f64 arg_number(const char* key, f64 fallback) const;
  /// Platform side: "k=v&k2=v2" (URL-encoded) launch parameters.
  void set_args(const char* text, u32 length);
  i32 width() const { return renderer_.width(); }
  i32 height() const { return renderer_.height(); }
  f32 aspect() const { return static_cast<f32>(renderer_.width()) / static_cast<f32>(max_of(1, renderer_.height())); }
  const RenderStats& stats() const { return renderer_.last_stats(); }
  /// Clear colour for the frame (behind the sky only where no sky is drawn).
  Vec4 clear_color() const { return clear_color_; }
  void set_clear_color(Vec4 color) { clear_color_ = color; }
  /// WebXR (kern_app_frame_xr): the headset's eye in the world and where it looks, as drawn last
  /// frame (the first eye); `xr` false outside a session.
  bool xr() const { return xr_; }
  void set_xr(bool on) { xr_ = on; }
  Vec3 xr_eye() const { return xr_eye_; }
  void set_xr_eye(Vec3 eye) { xr_eye_ = eye; }
  Vec3 xr_forward() const { return xr_forward_; }
  void set_xr_forward(Vec3 forward) { xr_forward_ = forward; }
  /// The room (WebXR "local" space) in the world: its forward (where the headset's -z points at
  /// zero head yaw) and the head's yaw in it this frame (degrees, counter-clockwise from above; set
  /// before the app's update). With `xr_follow_head` the app turns its body with the head: the room
  /// is the body's heading minus that yaw, so turning in the real room turns the player.
  Vec3 xr_room_forward() const { return xr_room_forward_; }
  void set_xr_room_forward(Vec3 forward) { xr_room_forward_ = forward; }
  f32 xr_head_yaw() const { return xr_head_yaw_; }
  void set_xr_head_yaw(f32 yaw) { xr_head_yaw_ = yaw; }
  bool xr_follow_head() const { return xr_follow_head_; }
  void set_xr_follow_head(bool on) { xr_follow_head_ = on; }
  /// Keep the room where it is (the app's camera spins or falls: death, a ragdoll).
  bool xr_hold_room() const { return xr_hold_room_; }
  void set_xr_hold_room(bool on) { xr_hold_room_ = on; }
  /// The room turns with the camera fully (pitch and roll too: a cockpit, an FPV drone - the craft
  /// is the rest frame); `xr_room_up` is then its up (else the world's).
  bool xr_full_room() const { return xr_full_room_; }
  void set_xr_full_room(bool on) { xr_full_room_ = on; }
  Vec3 xr_room_up() const { return xr_room_up_; }
  void set_xr_room_up(Vec3 up) { xr_room_up_ = up; }
  /// The comfort tunnel (0 off .. 1 narrow): `xr_vignette_mesh` (the app's mesh: a black ring in
  /// its x-z plane facing +y, see-through inside radius 1, opaque from 2 out) is put before each
  /// eye, the hole's half-angle 60 deg at 0 down to 26 deg at 1; hidden at 0.
  f32 xr_vignette() const { return xr_vignette_; }
  void set_xr_vignette(f32 amount) { xr_vignette_ = amount; }
  u32 xr_vignette_mesh() const { return xr_vignette_mesh_; }
  void set_xr_vignette_mesh(u32 mesh) { xr_vignette_mesh_ = mesh; }
  /// A second view drawn first in an XR frame (a sniper's scope): into `xr_scope_fbo` (a render
  /// target of xr_scope_size px, Renderer::create_render_target) through `xr_scope_camera`.
  bool xr_scope() const { return xr_scope_; }
  void set_xr_scope(bool on) { xr_scope_ = on; }
  const Camera& xr_scope_camera() const { return xr_scope_camera_; }
  void set_xr_scope_camera(const Camera& camera) { xr_scope_camera_ = camera; }
  u32 xr_scope_fbo() const { return xr_scope_fbo_; }
  void set_xr_scope_fbo(u32 fbo) { xr_scope_fbo_ = fbo; }
  i32 xr_scope_size() const { return xr_scope_size_; }
  void set_xr_scope_size(i32 size) { xr_scope_size_ = size; }
  /// Side-by-side stereo (a phone in a cardboard viewer, 3D TVs): the frame is drawn twice, the
  /// left and the right half, the eyes `stereo_ipd` metres apart; `stereo_cross`: cross-eye
  /// viewing, the right eye's image on the left half.
  bool stereo() const { return stereo_; }
  void set_stereo(bool on) { stereo_ = on; }
  f32 stereo_ipd() const { return stereo_ipd_; }
  void set_stereo_ipd(f32 ipd) { stereo_ipd_ = ipd; }
  bool stereo_cross() const { return stereo_cross_; }
  void set_stereo_cross(bool on) { stereo_cross_ = on; }

 private:
  Renderer& renderer_;
  const game::Project* project_ = nullptr;
  std::string profile_;
  Input input_;
  audio::Mixer mixer_;
  InputActions actions_;
  struct Slot {
    std::string name;
    json::Value values;     // key -> {"int": "..."} / {"float": x} / {"bool": b} / {"text": "..."}
    std::string broken;     // the host's text is no kern-save (why): not read, not written over
  };
  struct Widget {
    std::string kind, id, text;
    f32 value = 0, x = 0, y = 0, width = 0;
  };
  std::string status_;
  std::vector<Widget> widgets_;
  std::vector<std::pair<std::string, i32>> ui_pending_, ui_events_;
  std::string ui_sent_;
  mutable std::vector<std::string> ui_said_;
  bool ui_add(Widget w);
  i32 ui_event_of(const std::string& id) const;
  AppHost* host_;
  StorageHost* storage_ = nullptr;
  std::vector<Slot> slots_;
  std::string loaded_text_;
  Slot* slot(const char* name, const char* key, bool for_write);
  bool store(Slot& s, const char* key, json::Value value);
  const json::Value* stored(const char* slot, const char* key, const char* type);
  mutable std::vector<std::string> unknown_actions_;   // (said once each)
  const ActionBinding* find_action(const char* name) const;
  const AxisBinding* find_axis(const char* name) const;
  bool touch_in(TouchZone zone, bool pressed) const;
  Vec4 clear_color_{0.045f, 0.07f, 0.12f, 1};
  bool xr_ = false;
  Vec3 xr_eye_, xr_forward_{1, 0, 0};
  Vec3 xr_room_forward_{1, 0, 0};
  f32 xr_head_yaw_ = 0;
  bool xr_follow_head_ = false;
  bool xr_hold_room_ = false;
  bool xr_full_room_ = false;
  Vec3 xr_room_up_{0, 0, 1};
  f32 xr_vignette_ = 0;
  u32 xr_vignette_mesh_ = 0;
  bool xr_scope_ = false;
  Camera xr_scope_camera_;
  u32 xr_scope_fbo_ = 0;
  i32 xr_scope_size_ = 0;
  bool stereo_ = false;
  f32 stereo_ipd_ = 0.064f;
  bool stereo_cross_ = true;
  std::vector<std::pair<std::string, std::string>> args_;
};

class App {
 public:
  virtual ~App() = default;
  /// Shader extensions the game uses (must outlive the app).
  virtual Span<ShaderExtension* const> extensions() { return {}; }
  virtual bool start(Engine& engine) = 0;
  virtual void update(Engine& engine, const Frame& frame) = 0;
  virtual void resized(Engine& engine) { (void)engine; }
  virtual void stop(Engine& engine) { (void)engine; }
};

}  // namespace kern
