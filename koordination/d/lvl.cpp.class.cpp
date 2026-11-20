// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Level : kern::Blueprint {
  // ---- variables ----
  Int Count = 0;

  // ---- functions ----
  void Note() {
    engine_game_world_events(n2, 1);
    list_add(n2.result, "x");
    Count = list_length<Text>(n2.result);
  }

  // ---- what nodes give and keep ----
  struct { engine_game_world_events_place<List<Text>> result = {}; } n2;
};
// How the engine reaches the Blueprint Level by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace {
struct Level_Native final : kern::logic::NativeBlueprint {
  Level self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, const std::vector<kern::logic::Value>& args, std::vector<kern::logic::Value>& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Note") {
      kern::logic::native_call(true);
      self.Note();
      kern::logic::native_call(false);
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "Count") { out = kern::logic::to_value(self.Count); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    if (variable == "Count") { kern::logic::native_set(self.Count, value); return true; }
    return false;
  }
};
[[maybe_unused]] const bool Level_registered =
    (kern::logic::register_native_blueprint(0x7ee20882bd0b73deull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new Level_Native()); }), true);
}  // namespace
