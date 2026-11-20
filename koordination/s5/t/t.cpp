#include <cstdio>
#include <string>
#include "kern/logic/logic.h"
namespace logic = kern::logic;
#define CHECK(x) do { if (!(x)) { std::printf("check failed: %s (line %d)\n", #x, __LINE__); ++failed; } } while (0)
int main() {
  int failed = 0;
  // A function the program gives its host under a fixed C name (Function::exported, KERN_EXPORT_AS): the name
  // and how its values cross (the wire) are part of the Blueprint - in its graph, its text, both ways - and the
  // glue of its class writes the C function (logic::native_glue; blueprint_native_test calls the case's).
  const char* text = R"kn(
struct Host : kern::Blueprint {
  struct game_hudResult { Text Result = ""; };
  KERN_EXPORT_AS("game_hud", "text len bytes len i32 f32 -> text") game_hudResult game_hud(Text ask = "", Int ask_length = 0, List<Int> data = List<Int>{}, Int data_length = 0, Int view = 0, Float scale = 0.0) {
    return {ask};
  }
  KERN_EXPORT_AS("game_tick", "f64") void game_tick(Float dt = 0.0) {
  }
  KERN_EXPORT_AS("game_push", "bytes at len") void game_push(const List<Int>& data, Int data_at = 0, Int length = 0) {
  }
};
)kn";
  logic::Graph g;
  std::string error;
  CHECK(logic::from_cpp(text, g, nullptr, &error));
  if (!error.empty()) std::printf("%s\n", error.c_str());
  const logic::Function* hud = g.function("game_hud");
  CHECK(hud && hud->exported == "game_hud" && hud->wire == "text len bytes len i32 f32 -> text");
  CHECK(g.function("game_tick") && g.function("game_tick")->exported == "game_tick" && g.function("game_tick")->wire == "f64");
  logic::Graph back;
  CHECK(logic::graph_from_json(logic::graph_to_json(g), back) && back == g);
  std::string written;
  CHECK(logic::to_cpp(g, "Host", written, &error) && written.find("  KERN_EXPORT_AS(\"game_hud\", \"text len bytes len i32 f32 -> text\") game_hudResult game_hud(") != std::string::npos);
  logic::Graph read;
  CHECK(logic::from_cpp(written, read, nullptr, &error) && read == g);
  std::string glue;
  CHECK(logic::native_glue(g, "Host", glue, &error) && glue.find("KERN_EXPORT(game_hud) const char* game_hud(const char* a0, kern::u32 a1, const kern::u8* a2, kern::u32 a3, kern::i32 a4, kern::f32 a5) {") != std::string::npos &&
        glue.find("KERN_EXPORT(game_tick) void game_tick(kern::f64 a0) {") != std::string::npos);
  // a pointer of C is a list and the place in it (at): no C parameter - from the host the place is 0
  CHECK(g.function("game_push") && g.function("game_push")->wire == "bytes at len" &&
        glue.find("KERN_EXPORT(game_push) void game_push(const kern::u8* a0, kern::u32 a2) {") != std::string::npos &&
        glue.find("(kern::logic::export_bytes_in(a0, a2), ") != std::string::npos);
  // what does not fit is said
  logic::Function f = hud ? *hud : logic::Function{};
  CHECK(logic::export_ok(f));
  f.wire = "text len bytes len i32 -> text";   // (one word too few)
  CHECK(!logic::export_ok(f));
  f.wire = "text len bytes i32 i32 f32 -> text";   // (bytes without their len)
  CHECK(!logic::export_ok(f));
  f.wire = "text text bytes len i32 f32 -> text";   // (a text where an Int is)
  CHECK(!logic::export_ok(f));
  f.wire = "text len bytes len i32 f32 -> bytes";   // (bytes do not go out)
  CHECK(!logic::export_ok(f));
  f.wire = "text len bytes at i32 i32 f32 -> text";   // (an at where the bytes' len is)
  CHECK(!logic::export_ok(f));
  f.wire = "text at bytes len i32 f32 -> text";   // (an at after a text: a text has no place, it is a Text)
  CHECK(!logic::export_ok(f));
  f.wire = "i32 at bytes len i32 f32 -> text";   // (an at after no bytes)
  CHECK(!logic::export_ok(f));
  f.wire = "text len bytes len i32 f32 -> text";
  f.exported = "game hud";
  CHECK(!logic::export_ok(f));
  CHECK(!logic::from_cpp(std::string(text).replace(std::string(text).find("\"f64\""), 5, "\"text\""), read, nullptr, &error));

  std::printf("failed %d\n", failed);
  return failed != 0;
}
