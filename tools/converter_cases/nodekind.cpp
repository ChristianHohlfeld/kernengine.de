#include "nodekind.h"
namespace sc {
bool is_event(kern::logic::NodeKind k) { return k == kern::logic::NodeKind::event; }
void register_mine() {
  static bool done = false;
  if (done) return;
  done = true;
  kern::logic::NodeDef d;
  d.type = "cases.mine";
  d.label = "Mine";
  d.kind = kern::logic::NodeKind::event;
  kern::logic::library().add(std::move(d));
}
}
