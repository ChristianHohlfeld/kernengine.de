// The program's one entry (kern/docs/project.md 4.): the program defines kern_main(), the platform layer
// (kern/web/app.mjs) calls it once, before the app starts, and it hands the engine the App - no symbol the linker
// chooses, no factory the engine asks for:
//
//   int kern_main() { static MyGame game; return kern::run(game); }                    // an App of C++
//   int kern_main() { return kern::run(kern::game::embedded_project()); }              // the App of a project
//                                                                                      // (kern/game/program.h)
#pragma once

#include "kern/app/app.h"
#include "kern/app/project.h"

namespace kern {

/// Hands the app runtime the program's App and the project's settings (kern/app/project.h) - from the program's
/// kern_main(), once. The platform then starts it (kern_app_start: the settings before the renderer, then
/// App::start), drives its frames and stops it. `app` must outlive the app runtime. Returns 0; a second call is
/// refused: 1, and the log says why (the first App stays).
int run(App& app, const ProjectSettings& settings = {});

}  // namespace kern

/// The program's one entry, which the program defines (C linkage; the platform layer calls it once, before
/// kern_app_start): it hands the engine its App with kern::run and returns what run gave. A module without it
/// cannot start an app (kern/web/app.mjs says so).
extern "C" int kern_main();
