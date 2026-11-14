// Kern application framework: write a game in C++ against Engine/App and the
// platform layer (kern/web/app.mjs) runs it - frame loop, drawing-buffer
// size, input, context loss and a status-text channel are handled for you.
//
//   class MyGame final : public kern::App {
//     bool start(kern::Engine& engine) override { ...create meshes... }
//     void update(kern::Engine& engine, const kern::Frame& frame) override { ...move things... }
//   };
//   kern::App* kern_create_app() { static MyGame game; return &game; }
//
// update() runs once per display frame; the engine renders right after it.
#pragma once

#include <string>
#include <utility>
#include <vector>

#include "kern/gfx/renderer.h"

namespace kern {

/// Keyboard keys (KeyboardEvent.code order in kern/web/app.mjs KEY_CODES).
enum class Key : u8 {
  unknown = 0,
  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
  digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9,
  left, right, up, down, space, enter, escape, tab, backspace, shift_left, shift_right, control_left, control_right, alt_left, alt_right,
  count
};

constexpr u32 kMaxTouches = 10;

struct Touch {
  i32 id = -1;  // < 0: slot unused
  Vec2 start, position, previous;
};

class Input {
 public:
  bool down(Key k) const { return keys_[static_cast<u32>(k)] & kDown; }
  bool pressed(Key k) const { return keys_[static_cast<u32>(k)] & kPressed; }
  bool released(Key k) const { return keys_[static_cast<u32>(k)] & kReleased; }
  /// Pointer position in drawing-buffer pixels, movement since the last frame (pixels, includes locked movement).
  Vec2 pointer() const { return pointer_; }
  Vec2 pointer_delta() const { return pointer_delta_; }
  bool button(u32 b) const { return buttons_ & (1u << b); }
  bool button_pressed(u32 b) const { return pressed_buttons_ & (1u << b); }
  f32 wheel() const { return wheel_; }
  const Touch* touches() const { return touches_; }
  u32 touch_count() const;
  bool focused() const { return focused_; }

  // ---- platform side ----
  void key(Key k, bool down);
  void pointer_move(Vec2 position, Vec2 delta);
  void pointer_buttons(u32 buttons);
  void add_wheel(f32 dy) { wheel_ += dy; }
  void touch(i32 id, i32 phase, Vec2 position);  // phase 0 start, 1 move, 2 end/cancel
  void set_focus(bool focused);
  void end_frame();  // clears per-frame edges and deltas

 private:
  enum : u8 { kDown = 1, kPressed = 2, kReleased = 4 };
  u8 keys_[static_cast<u32>(Key::count)] = {};
  Vec2 pointer_, pointer_delta_;
  u32 buttons_ = 0, pressed_buttons_ = 0;
  f32 wheel_ = 0;
  Touch touches_[kMaxTouches];
  bool focused_ = true;
};

struct Frame {
  f64 time = 0;   // seconds since start
  f32 dt = 0;     // seconds since the previous frame (clamped to 0.1)
  u64 index = 0;
};

class Engine {
 public:
  explicit Engine(Renderer& renderer) : renderer_(renderer) {}
  /// The renderer and the input of the frame: objects of the engine (a Blueprint reaches them by these).
  Renderer& renderer() const { return renderer_; }
  Input& input() { return input_; }
  const Input& input() const { return input_; }
  /// Text shown by the host page in its status overlay (empty hides it).
  void set_status(const char* text);
  /// Asks the host to capture (or release) the mouse for mouse-look controls.
  void request_pointer_lock(bool lock);
  /// Launch parameter `key` from the host (the page's query string by default), or `fallback`.
  const char* arg(const char* key, const char* fallback = nullptr) const;
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
  Input input_;
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
  /// Renderer configuration, asked once before start().
  virtual RendererOptions configure() { return {}; }
  /// Shader extensions the game uses (must outlive the app).
  virtual Span<ShaderExtension* const> extensions() { return {}; }
  virtual bool start(Engine& engine) = 0;
  virtual void update(Engine& engine, const Frame& frame) = 0;
  virtual void resized(Engine& engine) { (void)engine; }
  virtual void stop(Engine& engine) { (void)engine; }
};

}  // namespace kern

/// Implemented by the game: returns its App instance (called once).
kern::App* kern_create_app();
