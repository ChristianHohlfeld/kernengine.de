// The app runtime (kern/docs/app.md 1., kern/app/runtime.h) and the C ABI the platform layer (kern/web/app.mjs)
// drives. The App and the project's settings are what the program's kern_main() handed over with kern::run
// (kern/docs/project.md 4.).
#include "kern/app/runtime.h"
#include "kern/app/run.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>

#include "kern/api/c_api.h"

namespace kern {
/// The renderer the project's settings start: the device profile the launch argument profile=<name> names, else the
/// project's. A name the project has no profile of: an error in the log with the names it has, and the project's.
RendererOptions start_renderer(const ProjectSettings& p, const Engine& e) {
  const std::string want = e.arg("profile", "");
  if (want.empty()) return p.renderer;
  if (const DeviceProfile* d = p.profile(want)) return d->renderer;
  std::string names;
  for (const DeviceProfile& d : p.profiles) names += (names.empty() ? "" : ", ") + d.name;
  log(LogLevel::error, "the launch argument profile=%s names no device profile of the project (it has: %s) - the renderer starts with the project's",
      want.c_str(), names.empty() ? "none" : names.c_str());
  return p.renderer;
}
/// Between the renderer's init and App::start: the heap grown once - one block taken and given back stays in the
/// allocator's free space (wasm memory never shrinks), so the game does not stall growing it piece by piece.
void reserve_heap(const ProjectSettings& p) {
  if (p.heap_reserve_mb > 0)
    if (void* reserve = std::malloc(static_cast<std::size_t>(p.heap_reserve_mb) << 20)) std::free(reserve);
}

int AppRuntime::give(App& app, const ProjectSettings& settings) {
  if (app_) {
    log(LogLevel::error, "kern::run: the program has given its App already - kern_main hands over one App, once");
    return 1;
  }
  app_ = &app;
  settings_ = settings;
  engine_.set_actions(settings.input);
  return 0;
}

bool AppRuntime::start(i32 width, i32 height) {
  if (running_) return true;
  if (!app_) return fail("no App: the program's kern_main() hands the engine its App with kern::run (kern/docs/project.md 4.)");
  RendererOptions options = start_renderer(settings_, engine_);
  options.width = width;
  options.height = height;
  clear_error();
  if (!engine_.renderer().init(options, app_->extensions())) {
    const std::string why = *last_error() ? last_error() : "it named no reason";
    return fail("the renderer did not start: %s", why.c_str());
  }
  reserve_heap(settings_);
  // the mixer at the device's rate (kern_app_audio_init), else at 48000 Hz: ready when the App starts
  if (!engine_.mixer().ready() && !engine_.mixer().init(48000)) return fail("the mixer did not start: %s", last_error());
  // (what went wrong before the start is not its reason)
  clear_error();
  if (!app_->start(engine_)) {
    const std::string why = *last_error() ? last_error() : "its start() gave false and named no reason (an App says why with kern::fail)";
    engine_.renderer().shutdown();
    return fail("the App did not start: %s", why.c_str());
  }
  running_ = true;
  start_ms_ = last_ms_ = -1;
  frame_ = 0;
  return true;
}

Frame AppRuntime::next_frame(f64 now_ms) {
  if (start_ms_ < 0) start_ms_ = last_ms_ = now_ms;
  Frame frame;
  frame.time = (now_ms - start_ms_) / 1000.0;
  frame.dt = static_cast<f32>(clamp((now_ms - last_ms_) / 1000.0, 0.0, 0.1));
  frame.index = frame_++;
  last_ms_ = now_ms;
  return frame;
}

bool AppRuntime::frame(f64 now_ms, bool page_hidden) {
  if (!running_) return false;
  engine_.set_xr(false);
  const Frame frame = next_frame(now_ms);
  app_->update(engine_, frame);
  engine_.input().end_frame();
  if (engine_.renderer().lost()) return true;
  RenderParams params;
  params.clear_color = engine_.clear_color();
  params.page_hidden = page_hidden != 0;
  RenderStats stats;
  if (engine_.stereo() && engine_.renderer().width() >= 4) {
    // side by side: each half the camera's own view, the eye shifted half the eye distance along
    // the camera's right (parallel eyes, the same fov over half the width)
    Camera cam = engine_.renderer().camera();
    const Camera anchor = cam;
    const Vec3 f = normalize(anchor.target - anchor.eye, {0, 1, 0});
    const Vec3 r = normalize(cross(f, anchor.up), {1, 0, 0});
    const i32 w = engine_.renderer().width(), h = engine_.renderer().height(), half = w / 2;
    bool ok = true;
    for (int e = 0; e < 2; ++e) {
      const f32 side = (e == 0) != engine_.stereo_cross() ? -0.5f : 0.5f;
      cam = anchor;
      cam.eye = anchor.eye + r * (side * engine_.stereo_ipd());
      cam.target = anchor.target + r * (side * engine_.stereo_ipd());
      params.has_viewport = true;
      params.viewport = {e == 0 ? 0 : half, 0, e == 0 ? half : w - half, h};
      engine_.renderer().set_camera(cam);
      ok = engine_.renderer().render(params, stats) && ok;
    }
    engine_.renderer().set_camera(anchor);
    return ok;
  }
  return engine_.renderer().render(params, stats) ;
}

/// One WebXR frame: update once, then every eye. `views` holds `count` x 27 floats: the eye's
/// projection (16, column-major), its position (3) and orientation (quaternion x y z w) in the
/// reference space (WebXR: y up, -z forward, metres; "local": the origin where the head started),
/// and its viewport in the layer's framebuffer (x y w h). The app's camera after the update is
/// the anchor: the reference space's origin sits at the camera's eye, its forward along the
/// camera's heading (the app turns the player, the head looks around); Kern is z up.
bool AppRuntime::frame_xr(f64 now_ms, i32 count, const f32* views) {
  if (!running_ || count < 1 || count > 4 || !views) return false;
  const Frame frame = next_frame(now_ms);
  // the head's yaw in the room, before the update (the app may turn its body with it)
  {
    Quat q{views[19], views[20], views[21], views[22]};
    const f32 ql = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (ql > 1e-6f) {
      q = {q.x / ql, q.y / ql, q.z / ql, q.w / ql};
      const Vec3 hf = q.rotate({0, 0, -1});
      if (hf.x * hf.x + hf.z * hf.z > 1e-6f) engine_.set_xr_head_yaw(std::atan2(-hf.x, -hf.z) * (180.0f / kPi));
    }
  }
  app_->update(engine_, frame);
  engine_.input().end_frame();
  if (engine_.renderer().lost()) return true;
  Camera cam = engine_.renderer().camera();
  const Camera anchor = cam;
  Vec3 f = anchor.target - anchor.eye;
  f.z = 0;
  f = normalize(f, {0, 1, 0});
  Vec3 up{0, 0, 1};
  if (engine_.xr_hold_room()) f = normalize(Vec3{engine_.xr_room_forward().x, engine_.xr_room_forward().y, 0}, f);
  else if (engine_.xr_follow_head()) {
    // the body faces where the head does: the room is the body's heading less the head's yaw
    const f32 a = -engine_.xr_head_yaw() * (kPi / 180.0f), c = std::cos(a), s = std::sin(a);
    f = normalize(Vec3{f.x * c - f.y * s, f.x * s + f.y * c, 0}, {0, 1, 0});
  } else if (engine_.xr_full_room()) {
    // the craft is the rest frame: the room pitches and rolls with the camera
    const Vec3 ff = normalize(anchor.target - anchor.eye, f);
    const Vec3 rr = normalize(cross(ff, anchor.up), Vec3{f.y, -f.x, 0});
    f = ff;
    up = cross(rr, ff);
  }
  engine_.set_xr_room_forward(f);
  engine_.set_xr_room_up(up);

  const Vec3 r = normalize(cross(f, up), Vec3{f.y, -f.x, 0});
  const f32 vig = engine_.xr_vignette();
  const u32 vig_mesh = engine_.xr_vignette_mesh();
  if (vig_mesh) engine_.renderer().set_visible(vig_mesh, vig > 0.01f);
  // a WebXR vector (x right, y up, -z forward) in the world
  auto world = [&](Vec3 v) { return r * v.x + up * v.y + f * (-v.z); };
  bool ok = true;
  for (i32 i = 0; i < count; ++i) {
    const f32* v = views + i * 27;
    Quat q{v[19], v[20], v[21], v[22]};
    const f32 ql = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (!(ql > 1e-6f)) continue;
    q = {q.x / ql, q.y / ql, q.z / ql, q.w / ql};
    const Vec3 eye = anchor.eye + world({v[16], v[17], v[18]});
    cam = anchor;
    cam.eye = eye;
    cam.target = eye + world(q.rotate({0, 0, -1}));
    cam.up = world(q.rotate({0, 1, 0}));
    cam.has_projection = true;
    std::memcpy(cam.projection.m, v, 16 * sizeof(f32));
    RenderParams params;
    params.clear_color = engine_.clear_color();
    params.has_viewport = true;
    params.reuse_shadow = i > 0;  // one sun shadow map per XR frame, not per eye
    if (vig_mesh && vig > 0.01f) {
      // the comfort tunnel before this eye: the hole's half-angle 60 deg .. 26 deg
      const Vec3 er = world(q.rotate({1, 0, 0})), eu = world(q.rotate({0, 1, 0})), ef = world(q.rotate({0, 0, -1}));
      const f32 d = std::fmax(0.3f, cam.near * 1.5f), half = (60.0f - 34.0f * std::fmin(vig, 1.0f)) * (kPi / 180.0f), sc = d * std::tan(half);
      const Vec3 c = eye + ef * d;
      const f32 m[16] = {er.x * sc, er.y * sc, er.z * sc, 0, ef.x, ef.y, ef.z, 0, eu.x * sc, eu.y * sc, eu.z * sc, 0, c.x, c.y, c.z, 1};
      engine_.renderer().update_instances(vig_mesh, {m, 16}, {});
    }
    params.viewport = {static_cast<i32>(v[23]), static_cast<i32>(v[24]), static_cast<i32>(v[25]), static_cast<i32>(v[26])};
    RenderStats stats;
    engine_.renderer().set_camera(cam);
    ok = engine_.renderer().render(params, stats) && ok;
    if (i == 0) { engine_.set_xr(true); engine_.set_xr_eye(cam.eye); engine_.set_xr_forward(normalize(cam.target - cam.eye, {1, 0, 0})); }
  }
  if (engine_.xr_scope() && engine_.xr_scope_fbo() && engine_.xr_scope_size() > 0) {
    // the scope's own picture after the eyes (their shadow map reused; the lens shows it next frame);
    // the comfort tunnel is the eyes', not the scope's
    if (vig_mesh) engine_.renderer().set_visible(vig_mesh, false);
    cam = engine_.xr_scope_camera();
    cam.has_projection = false;
    RenderParams sp;
    sp.clear_color = engine_.clear_color();
    sp.has_viewport = true;
    sp.reuse_shadow = true;
    sp.framebuffer = engine_.xr_scope_fbo();
    sp.viewport = {0, 0, engine_.xr_scope_size(), engine_.xr_scope_size()};
    RenderStats ss;
    engine_.renderer().set_camera(cam);
    if (!engine_.renderer().render(sp, ss)) engine_.set_xr_scope(false);
  }
  engine_.renderer().set_camera(anchor);
  return ok;
}

bool AppRuntime::resize(i32 width, i32 height) {
  if (!running_) return false;
  if (!engine_.renderer().resize(width, height)) return false;
  app_->resized(engine_);
  return true;
}

bool AppRuntime::audio_init(u32 rate) {
  if (running_) return fail("kern_app_audio_init: the app runs already - the device's rate is given before kern_app_start");
  if (rate < 8000 || rate > 192000) return fail("kern_app_audio_init: a sample rate of %u Hz - the mixer takes 8000 to 192000", rate);
  if (engine_.mixer().ready() && engine_.mixer().sample_rate() == rate) return true;
  return engine_.mixer().init(rate) || fail("kern_app_audio_init: the mixer did not start at %u Hz: %s", rate, last_error());
}

Span<const f32> AppRuntime::audio(u32 frames) {
  if (!running_ || frames == 0) return {};
  if (frames > kMaxAudioFrames) {
    fail("kern_app_audio: %u frames in one call - at most %u", frames, kMaxAudioFrames);
    return {};
  }
  audio_out_.resize(static_cast<std::size_t>(frames) * 2);
  engine_.mixer().render(audio_out_.data(), Count<2>{frames});
  return {audio_out_.data(), audio_out_.size()};
}

void AppRuntime::stop() {
  if (!running_) return;
  app_->stop(engine_);
  // (the mixer ends with the app: its clips and voices go, the rate stays for the next start)
  engine_.mixer().init(engine_.mixer().sample_rate());
  engine_.renderer().shutdown();
  running_ = false;
}

AppRuntime& app_runtime() {
  static AppRuntime runtime(c_api_renderer());
  return runtime;
}

int run(App& app, const ProjectSettings& settings) { return app_runtime().give(app, settings); }

}  // namespace kern

using namespace kern;

/// Launch parameters, before kern_app_start ("k=v&k2=v2", URL-encoded).
KERN_EXPORT(kern_app_set_args) void kern_app_set_args(const char* text, u32 length) { app_runtime().engine().set_args(text, length); }
/// Starts the App. 0: kern_last_error says why (kern/docs/app.md 1.).
KERN_EXPORT(kern_app_start) i32 kern_app_start(i32 width, i32 height) { return app_runtime().start(width, height) ? 1 : 0; }
/// One display frame: update + render. Returns 0 when rendering failed.
KERN_EXPORT(kern_app_frame) i32 kern_app_frame(f64 now_ms, i32 page_hidden) { return app_runtime().frame(now_ms, page_hidden != 0) ? 1 : 0; }
KERN_EXPORT(kern_app_frame_xr) i32 kern_app_frame_xr(f64 now_ms, i32 count, const f32* views) { return app_runtime().frame_xr(now_ms, count, views) ? 1 : 0; }
/// The sound device's sample rate, before kern_app_start (kern/docs/app.md 3.). 0: kern_last_error says why.
KERN_EXPORT(kern_app_audio_init) i32 kern_app_audio_init(u32 rate) { return app_runtime().audio_init(rate) ? 1 : 0; }
/// The next `frames` frames of the app's mixer, interleaved stereo; `out_len` gets the count of numbers (2 a frame).
KERN_EXPORT(kern_app_audio) const f32* kern_app_audio(u32 frames, u32* out_len) {
  const Span<const f32> pcm = app_runtime().audio(frames);
  if (out_len) *out_len = static_cast<u32>(pcm.size);
  return pcm.data;
}
KERN_EXPORT(kern_app_resize) i32 kern_app_resize(i32 width, i32 height) { return app_runtime().resize(width, height) ? 1 : 0; }

namespace {
/// The input of the process's app (before its start too: keys held while the page loads).
Input& input() { return app_runtime().engine().input(); }
}  // namespace
KERN_EXPORT(kern_app_key) void kern_app_key(u32 key, i32 down) {
  if (key < static_cast<u32>(Key::count)) input().key(static_cast<Key>(key), down != 0);
}
KERN_EXPORT(kern_app_pointer) void kern_app_pointer(f32 x, f32 y, f32 dx, f32 dy, u32 buttons) {
  input().pointer_move({x, y}, {dx, dy});
  input().pointer_buttons(buttons);
}
KERN_EXPORT(kern_app_wheel) void kern_app_wheel(f32 dy) { input().add_wheel(dy); }
KERN_EXPORT(kern_app_touch) void kern_app_touch(i32 id, i32 phase, f32 x, f32 y) { input().touch(id, phase, {x, y}); }
/// The gamepad of this frame (kern/docs/app.md 4.: standard mapping; sticks with up and right +1, triggers 0..1,
/// buttons as bits); connected 0: none.
KERN_EXPORT(kern_app_gamepad) void kern_app_gamepad(i32 connected, f32 lx, f32 ly, f32 rx, f32 ry, f32 lt, f32 rt, u32 buttons) {
  input().set_gamepad({connected != 0, lx, ly, rx, ry, lt, rt, buttons});
}
KERN_EXPORT(kern_app_focus) void kern_app_focus(i32 focused) { input().set_focus(focused != 0); }
KERN_EXPORT(kern_app_stop) void kern_app_stop() { app_runtime().stop(); }
