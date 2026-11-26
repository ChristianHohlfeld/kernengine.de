// Play Project in the editor (docs/studio.md "Play Project", Studio::play_project): the project's program as the game
// runs it - game::ProgramApp on an app runtime (kern/app/runtime.h) of the editor's renderer, in the editor's host.
#include "project_play.h"

#include <string>

#include "kern/core/base.h"
#include "kern/game/level.h"
#include "kern/input/actions.h"

namespace kern::studio {

void StudioAppHost::pointer_lock(bool lock) {
  // (the editor's own capture of the active viewport: its page locks the pointer there and releases it)
  json::Value a = json::Value::object();
  a.set("action", lock ? "capturePointer" : "releasePointer");
  a.set("viewport", studio_.active_view_);
  studio_.push_ui(std::move(a));
}

void StudioAppHost::widgets(std::string_view json) {
  widgets_ = std::string(json);   // (Studio::status - the page reads it every frame)
}

Engine* Studio::project_engine() { return project_play_ ? &project_play_->runtime.engine() : nullptr; }
game::Program* Studio::project_program() { return project_play_ ? &project_play_->app.program : nullptr; }

bool Studio::play_project(std::string* error) {
  auto refuse = [&](const std::string& why) {
    report("Play Project: " + why, "error");
    if (error) *error = why;
    return false;
  };
  if (project_play_) return true;
  if (trainer_open()) return refuse("the Lehrer's stage is open - close it first");
  if (play_ != PlayState::editing) return refuse("Play or Simulate runs - stop it first (Esc)");
  end_gizmo(false);
  for (auto& v : views_) end_drag(v);
  // the project as it is now: the open level (its start level) with the edits not saved yet
  game::Project pr = project_;
  const std::string open = pr.first_level();
  for (auto& [path, text] : pr.level_texts)
    if (path == open) text = game::to_text(level_);
  auto play = std::make_unique<ProjectPlay>(*this, *r_);
  std::string why;
  play->app.project = pr;
  if (!play->app.program.start(pr, &why)) return refuse(why);
  for (const std::string& s : play->app.program.said()) report(s, s.rfind("error: ", 0) == 0 ? "error" : "info");
  play->runtime.give(play->app, pr.settings);
  if (mixer_->ready()) play->runtime.audio_init(mixer_->sample_rate());   // (the App's mixer at the rate the editor is heard at)
  const Viewport& v = views_[active_view_];
  play->runtime.set_view({v.rect.x, v.rect.y, v.rect.w, v.rect.h}, layer::scene);
  // the editor's scene off: the App's worlds are what the viewport shows
  world_->sync(game::Level{});
  keys_.clear();
  project_play_ = std::move(play);
  if (!project_play_->runtime.start(r_->width(), r_->height())) {
    why = last_error();
    for (const std::string& s : project_play_->app.program.said()) report(s, s.rfind("error: ", 0) == 0 ? "error" : "info");
    stop_project();
    return refuse(why);
  }
  report("Playing the project " + pr.name + " - Esc stops");
  touch();
  return true;
}

void Studio::frame_project(f64 now_ms, f32) {
  ProjectPlay& p = *project_play_;
  const Viewport& v = views_[active_view_];
  p.runtime.set_view({v.rect.x, v.rect.y, v.rect.w, v.rect.h}, layer::scene);
  const bool drawn = p.runtime.frame(now_ms, false);
  for (const std::string& s : p.app.program.said()) report(s, s.rfind("error: ", 0) == 0 ? "error" : "info");
  if (!drawn && !p.runtime.running()) stop_project();
}

void Studio::stop_project() {
  if (!project_play_) return;
  project_play_->runtime.stop();   // (App::stop, then the program ends: its worlds with it)
  project_play_.reset();
  keys_.clear();
  json::Value a = json::Value::object();
  a.set("action", "releasePointer");
  push_ui(std::move(a));
  // the editor's scene again
  ++scene_revision_;
  synced_revision_ = ~0ull;
  world_synced_ = ~0ull;
  report("Stopped the project - back to editing");
  touch();
}

bool Studio::project_key(std::string_view code, bool down, bool repeat) {
  if (!project_play_) return false;
  if (down && !repeat && code == "Escape") {
    stop_project();
    return true;
  }
  Key k = Key::unknown;
  if (key_of_code(code, k)) project_play_->runtime.engine().input().key(k, down);
  return true;   // (every key is the game's while it plays)
}

}  // namespace kern::studio
