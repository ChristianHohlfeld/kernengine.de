// SpawnCity's level file and the Blueprints in it (see rules.h).
#include "rules.h"

#include "kern/core/json.h"

namespace sc::rules {

bool actor_graph(std::string_view actor, kern::logic::Graph& out, std::string* error) {
  kern::json::Value doc;
  std::string why;
  if (!kern::json::parse(level_text(), doc, &why)) {
    if (error) *error = "the level file: " + why;
    return false;
  }
  for (const kern::json::Value& a : doc["actors"].items()) {
    if (a["name"].as_string() != actor) continue;
    const kern::json::Value& graph = a["components"]["logic"];  // (an actor's Blueprint is one of its components)
    if (!graph.is_object()) break;
    return kern::logic::graph_from_json(graph, out, error);
  }
  if (error) *error = "the level has no actor \"" + std::string(actor) + "\" with a Blueprint";
  return false;
}

}  // namespace sc::rules
