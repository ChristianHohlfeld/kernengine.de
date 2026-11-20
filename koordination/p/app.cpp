// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Case : kern::Blueprint {
  // ---- variables ----
  Text T = "a";
  Text U = "";

  // ---- events ----
  KERN_EVENT(on1, event_begin)
  void on1() {
    for (auto it2 = flow_for(n2, 1, 4); it2.next();) {
      T = text_append(std::move(T), kern::to<Text>(n2.index), "-");
      T = text_append(std::move(T), T, "");
      U = text_append(std::move(U), T, "");
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<2> { Int index = 0; } n2;
};

KERN_LAYOUT(Case) {
  node(1, 0, 0);
  node(2, 0, 0);
  node(3, 0, 0);
  node(4, 0, 0);
  node(5, 0, 0);
  node(6, 0, 0);
  node(7, 0, 0);
  node(8, 0, 0);
  node(9, 0, 0);
  node(10, 0, 0);
  node(11, 0, 0);
  node(12, 0, 0);
  node(13, 0, 0);
  next_id(14);
}
