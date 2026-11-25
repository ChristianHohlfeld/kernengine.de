// The project a game carries: its project file and its levels as the build embedded them (kern/tools/embed_project.py
// writes the source that defines embedded_project_text and embedded_level, from the project file and the levels it
// names - kern/docs/project.md). A program built without that source does not call them.
//
//   int kern_main() { return kern::run(kern::game::embedded_project()); }   // (kern/game/program.h)
#pragma once

#include <string_view>

#include "kern/core/base.h"
#include "kern/game/project.h"

namespace kern::game {

/// The text of the embedded project file.
std::string_view embedded_project_text();
/// The text of the embedded level of that path (as the project's "levels" name it); empty: the project has none of
/// that path.
std::string_view embedded_level(std::string_view path);

/// The embedded project, read once. The build checked it (embed_project.py); one that does not read is a fault of
/// the build: it is reported with the reason (kern::fail), and the project is then empty - it starts no program.
inline const Project& embedded_project() {
  static const Project project = [] {
    Project p;
    std::string error;
    if (!from_text(embedded_project_text(), p, &error)) {
      fail("the embedded project: %s", error.c_str());
      p = Project{};
    }
    return p;
  }();
  return project;
}

}  // namespace kern::game
