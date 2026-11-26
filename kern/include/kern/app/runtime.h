// The app runtime (kern/docs/app.md 1.): runs the program's App - the settings before the renderer, App::start,
// a frame (update, then render) per display frame, App::stop. The platform layer (kern/web/app.mjs) drives the one
// of the process, app_runtime(), through the C ABI kern_app_* (kern/src/app/runtime.cpp); a test makes its own.
#pragma once

#include "kern/app/app.h"
#include "kern/app/project.h"

namespace kern {

class AppRuntime {
 public:
  /// The runtime of a game: its renderer is started by start and ended by stop, the engine's host is the platform's.
  explicit AppRuntime(Renderer& renderer) : engine_(renderer) {}
  /// The runtime of an App in a host whose renderer runs already (the editor's Play Project, docs/studio.md): the
  /// engine asks `host` (pointer lock, widgets); start does not start the renderer and stop does not end it - an App
  /// with shader extensions is refused there (they are given to a renderer when it starts). `host` and `renderer`
  /// outlive the runtime.
  AppRuntime(Renderer& renderer, AppHost& host) : engine_(renderer, host), renderer_runs_(true) {}
  AppRuntime(const AppRuntime&) = delete;
  AppRuntime& operator=(const AppRuntime&) = delete;

  /// The program's App and the project's settings (kern::run gives them to the process's runtime). `app` must outlive
  /// the runtime. Returns 0; a second call is refused: 1, and the log says why (the first App stays).
  int give(App& app, const ProjectSettings& settings = {});
  /// Starts the App: the settings' renderer (start_renderer), the heap reserve, App::start. False: `last_error()`
  /// says why - no App, the renderer did not start, or the App did not (its own reason, given with kern::fail).
  /// Started already: true.
  bool start(i32 width, i32 height);
  /// One display frame: update, then render. False: not running, or rendering failed.
  bool frame(f64 now_ms, bool page_hidden);
  /// One WebXR frame (kern_app_frame_xr in runtime.cpp says what `views` holds).
  bool frame_xr(f64 now_ms, i32 count, const f32* views);
  bool resize(i32 width, i32 height);
  /// The device's sample rate for the mixer (kern/docs/app.md 3.), before the start: 8000..192000. False (with the
  /// reason in last_error) outside that or once started. Without it the mixer starts at 48000 Hz.
  bool audio_init(u32 rate);
  /// The next `frames` frames of the mixer, interleaved stereo (2 numbers a frame; at most kMaxAudioFrames a call).
  /// Not running or too many frames: none (too many: last_error says why).
  Span<const f32> audio(u32 frames);
  static constexpr u32 kMaxAudioFrames = 8192;
  /// App::stop, then the renderer ends (not the renderer of a host that runs it already). Not running: nothing.
  void stop();
  /// Where a frame draws, for a host that draws more than the App (the editor: its active viewport, the layers of the
  /// App's meshes - not the editor's grid and gizmos). Without it: the whole drawing buffer, every layer.
  void set_view(const Viewport& view, u32 layers);

  bool running() const { return running_; }
  App* app() const { return app_; }
  Engine& engine() { return engine_; }
  const ProjectSettings& settings() const { return settings_; }

 private:
  Frame next_frame(f64 now_ms);

  Engine engine_;
  App* app_ = nullptr;
  ProjectSettings settings_;
  f64 start_ms_ = -1, last_ms_ = -1;
  u64 frame_ = 0;
  bool running_ = false;
  bool renderer_runs_ = false;   // (a host's renderer: not started, not ended here)
  bool has_view_ = false;
  Viewport view_;
  u32 layers_ = ~0u;
  std::vector<f32> audio_out_;
};

/// The process's one app runtime: the C ABI kern_app_* drives it, kern::run gives it the App.
AppRuntime& app_runtime();

}  // namespace kern
