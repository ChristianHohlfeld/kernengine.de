// A type of node that an object is: the object says its type, label and event, and runs where a node of it runs.
//
// A program that adds a node type to the engine's logic (the nodes its level's graphs may hold) gives an object of a
// class that is a NodeType - the program's C++, or the object of the engine of a Blueprint that implements NodeType
// (kern::Implements<kern::logic::NodeType>, the Blueprint in the program's logic runtime) - to register_node_type. The node
// type it makes (a NodeDef in library()) is an event node when `event` names an event (it handles each such event:
// "logic.tick" each fixed step), an action otherwise; its run calls the object's run and ends the chain there.
// What the type, label and event are is read once, when it is registered.
#pragma once

#include <string>

namespace kern::logic {

/// The object a type of node is (register_node_type).
class NodeType {
 public:
  virtual ~NodeType() = default;
  /// The node type's name ("spawncity.fixed_step"): what a node of the graph names (Node::type).
  virtual std::string type() const = 0;
  /// What the editor shows ("SpawnCity Fixed Step").
  virtual std::string label() const = 0;
  /// The event its nodes handle ("logic.tick"), or "": an action.
  virtual std::string event() const = 0;
  /// Where it stands in the editor's list, and what it says it does ("" each: none).
  virtual std::string category() const { return ""; }
  virtual std::string help() const { return ""; }
  /// What a node of it does where it runs.
  virtual void run() = 0;
};

/// Adds the node type `object` is to library() (its NodeDef: library().find(object->type())). False: no object, one
/// without a type, or a type that is there already of another object (the first stays). The object must outlive every
/// graph that runs a node of it; registered once - the same object again is true and changes nothing.
bool register_node_type(NodeType* object);

}  // namespace kern::logic
