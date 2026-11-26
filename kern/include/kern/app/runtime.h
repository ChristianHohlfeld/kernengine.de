// The app runtime (kern/docs/app.md 1.): runs the program's App - the settings before the renderer, App::start,
// a frame (update, then render) per display frame, App::stop. The platform layer (kern/web/app.mjs) drives the one
// of the process, app_runtime(), through the C ABI kern_app_* (kern/src/app/runtime.cpp); a test makes its own.
#pragma once

#include "kern/app/app.h"
#include "kern/app/project.h"

namespace kern {

class AppRuntime {
 public:
  explicit AppRuntime(Renderer& renderer) : engine_(renderer) {}
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
  /// App::stop, then the renderer ends. Not running: nothing.
  void stop();

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
};

/// The process's one app runtime: the C ABI kern_app_* drives it, kern::run gives it the App.
AppRuntime& app_runtime();

}  // namespace kern
