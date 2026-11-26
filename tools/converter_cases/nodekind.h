#pragma once
#include <string>
#include "kern/logic/logic.h"
namespace sc {
/// Whether a node of the program's own is an event.
bool is_event(kern::logic::NodeKind k);
/// Registers the program's node with the library (once): an event node.
void register_mine();
}
