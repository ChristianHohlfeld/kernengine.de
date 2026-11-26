// The engine's own test of its native build (kern/tools/build_native.sh, kern/docs/app.md 7.): a program of a minimal
// project - no game's scripts, no list of the engine's objects. Its App starts the project's level in a world of the
// program, reads the project's action, shows a widget, saves a value, and runs frames in the app runtime.
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

#include "kern/app/run.h"
#include "kern/app/runtime.h"
#include "kern/game/embedded.h"
#include "kern/game/program.h"
#include "kern/game/runtime.h"

using namespace kern;

namespace {
struct Mini final : App {
  std::unique_ptr<game::GameWorld> world;
  int frames = 0;
  bool went = false;
  bool start(Engine& e) override {
    world = std::make_unique<game::GameWorld>(e);
    return world->start_level("levels/eins.klevel", game::SimMode::play);
  }
  void update(Engine& e, const Frame& f) override {
    ++frames;
    world->step(f.dt, {});
    went |= e.action("go");
    e.ui_text("frames", std::to_string(frames).c_str(), 0, 0);
  }
  void stop(Engine&) override { world.reset(); }
};
int failures = 0;
void check(bool ok, const char* what) {
  std::printf("%s %s\n", ok ? "ok  " : "FAIL", what);
  failures += !ok;
}
}  // namespace

int main() {
  static Mini app;
  check(run(app, game::embedded_project()) == 0, "kern::run takes the App with the embedded project");
  AppRuntime& rt = app_runtime();
  check(rt.start(64, 48), last_error());
  check(app.world && app.world->running() && app.world->level().name == "Eins", "the project's level runs in the program's world");
  rt.engine().input().key(Key::space, true);
  for (int i = 0; i < 10; ++i) rt.frame(i * 16.0, false);
  check(app.frames == 10 && app.went, "ten frames; the project's action \"go\" is the space key");
  check(rt.engine().ui_json().find("\"text\":\"10\"") != std::string::npos, "the widget of the last frame");
  const std::string dir = std::string(std::getenv("TMPDIR") ? std::getenv("TMPDIR") : "/tmp") + "/kern-native-build-test";
  const std::string args = "saves=" + dir;
  rt.engine().set_args(args.c_str(), static_cast<u32>(args.size()));
  check(rt.engine().save_int("mini", "frames", app.frames) && rt.engine().load_int("mini", "frames", 0) == 10 && rt.engine().erase_saved("mini"), "a save slot in a file");
  rt.stop();
  std::printf(failures ? "native_build: %d failures\n" : "native_build: good\n", failures);
  return failures ? 1 : 0;
}
