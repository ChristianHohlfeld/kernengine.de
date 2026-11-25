// A type of node that an object is (kern/logic/node_type.h).
#include "kern/logic/node_type.h"

#include <map>

#include "kern/logic/logic.h"

namespace kern::logic {
namespace {
/// The objects of the types registered so (by type).
std::map<std::string, NodeType*>& registered() {
  static std::map<std::string, NodeType*> by;
  return by;
}
}  // namespace

bool register_node_type(NodeType* object) {
  if (!object) return false;
  const std::string type = object->type();
  if (type.empty()) return false;
  if (library().find(type)) return registered().count(type) && registered()[type] == object;
  NodeDef d;
  d.type = type;
  d.label = object->label();
  d.category = object->category();
  d.help = object->help();
  d.event = object->event();
  if (!d.event.empty()) {
    d.kind = NodeKind::event;
    d.accepts = [](const Node&, const Event&) { return true; };
    d.run = [object](Context&) {
      object->run();
      return kStop;
    };
  } else {
    d.kind = NodeKind::action;
    d.world = true;   // (what the object does is its own: the world, as far as the graph knows)
    d.inputs = {{"exec", "", PinType::exec, {}}};
    d.outputs = {{"then", "", PinType::exec, {}}};
    d.run = [object](Context&) {
      object->run();
      return 0;
    };
  }
  library().add(std::move(d));
  registered()[type] = object;
  return true;
}

}  // namespace kern::logic
