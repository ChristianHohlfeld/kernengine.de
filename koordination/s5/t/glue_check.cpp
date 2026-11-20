#include "kern/logic/blueprint.h"
#include "kern/logic/blueprint_class.h"
namespace kern_blueprint_test {
// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Host : kern::Blueprint {
  // ---- functions ----
  struct game_hudResult { Text Result = ""; };
  KERN_EXPORT_AS("game_hud", "text len bytes len i32 f32 -> text") game_hudResult game_hud(Text ask = "", Int ask_length = 0, List<Int> data = List<Int>{}, Int data_length = 0, Int view = 0, Float scale = 0.0) {
    return {std::move(ask)};
  }

  KERN_EXPORT_AS("game_tick", "f64") void game_tick(Float dt = 0.0) {
  }
};

KERN_LAYOUT(Host) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 0, 200);
  next_id(4);
}

// How the engine reaches the Blueprint Host by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace {
struct Host_Native final : kern::logic::NativeBlueprint {
  Host self;
  // (it gives functions to the host under their C names: the exports below call them on this object - the
  // instance that started last as this class)
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; kern::logic::blueprint_object<Host> = &self; }
  ~Host_Native() override { if (kern::logic::blueprint_object<Host> == &self) kern::logic::blueprint_object<Host> = nullptr; }
  void* object(const void* type) override { return type == &kern::logic::class_key<Host> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "game_hud") {
      kern::logic::Text in0 = native_arg<kern::logic::Text>(args, 0, "");
      kern::i64 in1 = native_arg<kern::i64>(args, 1, 0);
      kern::List<kern::logic::Whole> in2 = native_arg<kern::List<kern::logic::Whole>>(args, 2, kern::List<kern::logic::Whole>{});
      kern::i64 in3 = native_arg<kern::i64>(args, 3, 0);
      kern::i64 in4 = native_arg<kern::i64>(args, 4, 0);
      kern::f64 in5 = native_arg<kern::f64>(args, 5, 0.0);
      kern::logic::native_call(true);
      const auto out = self.game_hud(std::move(in0), std::move(in1), std::move(in2), std::move(in3), std::move(in4), std::move(in5));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "game_tick") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::logic::native_call(true);
      self.game_tick(std::move(in0));
      kern::logic::native_call(false);
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool Host_registered =
    (kern::logic::register_native_blueprint(0x06bd3ae10b620337ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new Host_Native()); }), true);
}  // namespace
KERN_EXPORT(game_hud) const char* game_hud(const char* a0, kern::u32 a1, const kern::u8* a2, kern::u32 a3, kern::i32 a4, kern::f32 a5) {
  Host* const object = kern::logic::blueprint_object<Host>;
  if (!object) return "";
  return kern::logic::export_text_out(object->game_hud(kern::logic::export_text_in(a0, a1), kern::logic::export_in<kern::i64>(a1), kern::logic::export_bytes_in(a2, a3), kern::logic::export_in<kern::i64>(a3), kern::logic::export_in<kern::i64>(a4), kern::logic::export_in<kern::f64>(a5)).Result);
}
KERN_EXPORT(game_tick) void game_tick(kern::f64 a0) {
  Host* const object = kern::logic::blueprint_object<Host>;
  if (!object) return;
  object->game_tick(kern::logic::export_in<kern::f64>(a0));
}

}
