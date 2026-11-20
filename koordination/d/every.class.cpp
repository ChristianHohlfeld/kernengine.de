// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Every : kern::Blueprint {
  // ---- variables ----
  Int VI = 0;

  // ---- events ----
  KERN_EVENT(on1, event_custom<"Bound">)
  void on1(Actor actor, Float value, Text text) {
    engine_game_world_events(n2, 3);
    list_add(n2.result, "x \"y\"");
    n2.result[1] = "z";
    VI = list_length<Text>(n2.result);
  }

  // ---- what nodes give and keep ----
  struct { engine_game_world_events_place<List<Text>> result = {}; } n2;
};

KERN_LAYOUT(Every) {
  node(1, 0, 0);
  node(2, 0, 0);
  node(3, 0, 0);
  node(4, 0, 0);
  node(7, 0, 0);
  node(6, 0, 0);
  node(5, 0, 0);
  next_id(11);
}
// How the engine reaches the Blueprint Every by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace {
struct Every_Native final : kern::logic::NativeBlueprint {
  Every self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    switch (node) {
      case 0: self.on1(native_out<kern::logic::ActorHandle>(*self.kern_world.instance, 0, 1), native_out<kern::f64>(*self.kern_world.instance, 0, 2), native_out<kern::logic::Text>(*self.kern_world.instance, 0, 3)); break;  // Event Bound
      default: break;
    }
  }
  bool call(std::string_view function, const std::vector<kern::logic::Value>& args, std::vector<kern::logic::Value>& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "VI") { out = kern::logic::to_value(self.VI); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    if (variable == "VI") { kern::logic::native_set(self.VI, value); return true; }
    return false;
  }
};
[[maybe_unused]] const bool Every_registered =
    (kern::logic::register_native_blueprint(0x100ceaae9f0909a8ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new Every_Native()); }), true);
}  // namespace
