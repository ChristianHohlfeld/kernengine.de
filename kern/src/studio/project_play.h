// Play Project in the editor (docs/studio.md "Play Project", Studio::play_project): the project's program with its App
// on an Engine of the editor's renderer, in the editor's host.
#pragma once

#include <memory>
#include <string>

#include "kern/app/app.h"
#include "kern/app/runtime.h"
#include "kern/game/program.h"
#include "kern/studio/studio.h"

namespace kern::studio {

/// The editor as the App's host (AppHost): the pointer lock is the editor's (captured over the active viewport,
/// released), the widgets are kept for the state (Studio::state "projectPlay").
class StudioAppHost final : public AppHost {
 public:
  explicit StudioAppHost(Studio& studio) : studio_(studio) {}
  void pointer_lock(bool lock) override;
  void widgets(std::string_view json) override;
  const std::string& widgets_json() const { return widgets_; }

 private:
  Studio& studio_;
  std::string widgets_ = "[]";
};

/// The project's program on an app runtime of the editor's renderer (AppRuntime in a host: the renderer runs already)
/// - the same App the game runs (game::ProgramApp), in the editor's host.
struct Studio::ProjectPlay {
  ProjectPlay(Studio& studio, Renderer& renderer) : host(studio), runtime(renderer, host) {}
  StudioAppHost host;
  AppRuntime runtime;
  game::ProgramApp app;
};

}  // namespace kern::studio
