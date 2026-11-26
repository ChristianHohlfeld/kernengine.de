// What a Blueprint of a world may not hold (kern/game/project.h, kern/docs/project.md 4.) - a source of its own: the
// engine's level reader, the editor and the tool that reads Blueprints into a level (tools/blueprint_cpp) ask the same.
#include "kern/game/project.h"

namespace kern::game {

std::string world_export(const logic::Graph& g) {
  for (const logic::Function& f : g.functions)
    if (!f.exported.empty() && !f.external && f.from.empty())
      return "the function " + f.name + " is an export (" + f.exported + ") - exports belong to the program: a Blueprint with an export stands in the project, not in a world (kern/docs/project.md 4.)";
  return {};
}

}  // namespace kern::game
