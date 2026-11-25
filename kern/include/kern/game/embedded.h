// The project a game carries: its project file and its levels as the build embedded them (kern/tools/embed_project.py
// writes the source that defines these, from the project file and the levels it names - kern/docs/project.md).
// A program built without that source does not call them.
#pragma once

#include <string_view>

namespace kern::game {

/// The text of the embedded project file.
std::string_view embedded_project();
/// The text of the embedded level of that path (as the project's "levels" name it); empty: the project has none of
/// that path.
std::string_view embedded_level(std::string_view path);

}  // namespace kern::game
