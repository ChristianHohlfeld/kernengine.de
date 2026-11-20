#include <cstdio>
#include "kern/logic/logic.h"
int main(){ int bad=0;
auto in_event = [](const std::string& lines) { return "  KERN_EVENT(on1, event_custom<\"Go\">)\n  void on1(Actor actor, Float value, Text text) {\n" + lines + "\n  }\n"; };
auto refused = [&](const std::string& body, const char* why) { const std::string text = "struct B : kern::Blueprint {\n  Float Heat = 0.0;\n  Int Count = 0;\n  Text Name = \"\";\n" + body + "\n};\n"; kern::logic::Graph g; std::string e; bool ok = kern::logic::from_cpp(text, g, nullptr, &e); if (ok || e.find(why) == std::string::npos) { ++bad; std::printf("NICHT ABGELEHNT/anders: %s -> %s\n", why, e.c_str()); } };
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return a + 1.0; }, Count));"), "this one is a formula of Floats");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(kern::math::div(a, 2.0), 3.0); }, Count));"), "this one is a formula of Floats");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(-a + 1.0, 3.0); }, Count));"), "this one is a formula of Floats");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::wrap32(a * kern::math::sqrt(2.0)); }, Count));"), "this one is a formula of Floats");
{ const std::string text = "struct B : kern::Blueprint {\n  Float Heat = 0.0;\n  Int Count = 0;\n  Text Name = \"\";\n" + in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(a + 1.0, 3.0); }, Count));") + "\n};\n"; kern::logic::Graph g; std::string e; if (!kern::logic::from_cpp(text, g, nullptr, &e)) { ++bad; std::printf("ABGELEHNT (soll gelesen werden): %s\n", e.c_str()); } }
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Float { return kern::math::band(a, 3.0); }, Count));"), "(a formula with Int inputs gives an Int)");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a, Float b) -> Int { return kern::math::band(a, b); }, Count, Heat));"), "(the inputs of a formula of whole numbers are Int");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(a, 3.0); }, Heat));"), "an input of a formula of whole numbers is an Int");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(a, 3.0); }, kern::to<Float>(Count)));"), "an Int as it is: no kern::to");
refused(in_event("    Count = kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(a, 3.0); }, 5));"), "no number written out");
{ auto t = [](const char* fx) { return std::string("struct B : kern::Blueprint {\n  KERN_STRUCT(Trio) { Fixed<Float, 3> m = Fixed<Float, 3>{}; Int k = 0; };\n  Trio P = Trio{Fixed<Float, 3>{}, 0};\n  KERN_EVENT(on1, event_begin)\n  void on1() {\n    P = struct_make<Trio>(") + fx + ", 7);\n  }\n};\n"; };
  for (const char* fx : {"Fixed<Float, 3>{1.5, -2.0}", "Fixed<Float, 4>{1.5, -2.0}", "Fixed<Float, 2>{1.5}", "List<Float>{1.5}"}) { kern::logic::Graph g; std::string e; bool ok = kern::logic::from_cpp(t(fx), g, nullptr, &e); std::printf("%s -> %s %s\n", fx, ok ? "gelesen" : "abgelehnt:", e.c_str()); } }
{ const std::string text = std::string("struct B : kern::Blueprint {\n  Float Heat = 0.0;\n  Int Count = 0;\n  Text Name = \"\";\n") + "  KERN_STRUCT(Trio) { Fixed<Float, 3> m = Fixed<Float, 3>{}; Int k = 0; };\n  Trio P = Trio{Fixed<Float, 3>{}, 0};\n" + in_event("    P = struct_make<Trio>(Fixed<Float, 3>{1.5}, 7);") + "\n};\n"; kern::logic::Graph g; std::string e; std::printf("Form refused: %s %s\n", kern::logic::from_cpp(text, g, nullptr, &e) ? "gelesen" : "abgelehnt", e.c_str()); }
std::printf("%d falsch\n", bad); }
