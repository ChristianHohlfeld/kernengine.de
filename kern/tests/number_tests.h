// The engine tests of the numbers (number_cases.h): numbers.h against C++, the nodes in the interpreter against
// numbers.h. Included by test_main.cpp (TEST, CHECK).
#pragma once

#include "number_cases.h"

// The data model of the subset (kern/docs/blueprint_cpp.md "Zahl-Typen"): LP64 - where the engine is built natively (64-bit
// Linux and macOS) C++ has it so, and the converter maps the types so on every platform.
#if !defined(__wasm__)
static_assert(sizeof(int) == 4 && sizeof(long) == 8 && sizeof(long long) == 8 && sizeof(std::size_t) == 8 && sizeof(std::ptrdiff_t) == 8 && sizeof(short) == 2,
              "the data model of the subset is LP64");
#endif

TEST(numbers_against_cpp) {
  using namespace number_cases;
  namespace num = kern::logic::num;
  failures = 0;
  whole_numbers<i8>(), whole_numbers<i16>(), whole_numbers<i32>(), whole_numbers<i64>();
  whole_numbers<u8>(), whole_numbers<u16>(), whole_numbers<u32>(), whole_numbers<u64>();
  floating_numbers<f32>(), floating_numbers<f64>();
  every_conversion<bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64>();
  CHECK(failures == 0);

  // What C++ says, written out: toward zero, the remainder has the sign of the dividend.
  CHECK(num::div<i32>(-7, 2) == -3 && num::rem<i32>(-7, 2) == -1 && num::div<i32>(7, -2) == -3 && num::rem<i32>(7, -2) == 1);
  CHECK(num::div<i64>(std::numeric_limits<i64>::min(), -1) == std::numeric_limits<i64>::min() && num::div<i32>(5, 0) == 0 && num::rem<u8>(5, 0) == 0);
  // The unsigned types go round, the signed ones (UB in C++) as fixed: in two's complement.
  CHECK(num::add<u32>(0xffffffffu, 1) == 0 && num::sub<u8>(0, 1) == 255 && num::mul<u16>(65535, 65535) == 1 && num::mul<u32>(0x9e3779b9u, 0x85ebca6bu) == static_cast<u32>(0x9e3779b9u * 0x85ebca6bu));
  CHECK(num::add<i32>(std::numeric_limits<i32>::max(), 1) == std::numeric_limits<i32>::min() && num::neg<i8>(-128) == -128 && num::abs<i64>(std::numeric_limits<i64>::min()) == std::numeric_limits<i64>::min());
  // Shifts at the width: the amount mod the width; >> of a signed number brings the sign in.
  CHECK(num::shl<u32>(1u, 32) == 1u && num::shl<u32>(1u, 31) == 0x80000000u && num::shr<i32>(-8, 1) == -4 && num::shr<u32>(0x80000000u, 31) == 1u && num::shl<u8>(0x81, 1) == 0x02);
  // 64 bits beyond 2^53: exact (FNV-1a over one byte; the mix of the converter's case bigint).
  CHECK(num::add<i64>((i64{1} << 53), 1) == (i64{1} << 53) + 1 && num::mul<u64>(0xcbf29ce484222325ull ^ 97u, 0x100000001b3ull) == static_cast<u64>((0xcbf29ce484222325ull ^ 97u) * 0x100000001b3ull));
  CHECK(num::add<i64>(num::mul<i64>(num::bit_xor<i64>(1469598103934665603LL, 97), 31), 7) == static_cast<i64>(static_cast<u64>(1469598103934665603LL ^ 97) * 31u + 7u));
  // Floating numbers to whole numbers beyond their ends (UB in C++): the nearer end, NaN 0.
  CHECK(num::cast<i32>(1e10f) == std::numeric_limits<i32>::max() && num::cast<i32>(-1e10f) == std::numeric_limits<i32>::min() && num::cast<u32>(-1.5) == 0u && num::cast<u32>(-0.5) == 0u);
  CHECK(num::cast<i64>(std::numeric_limits<f64>::quiet_NaN()) == 0 && num::cast<u64>(std::numeric_limits<f32>::infinity()) == std::numeric_limits<u64>::max() && num::cast<i64>(9223372036854775808.0) == std::numeric_limits<i64>::max());
  CHECK(num::cast<i8>(127.9f) == 127 && num::cast<i8>(-128.9) == -128 && num::cast<i8>(-129.5) == -128 && num::cast<u8>(255.9f) == 255 && num::cast<u32>(4294967295.5) == 4294967295u);
  // Whole numbers to narrower ones: the low bits; to floating ones: the nearest.
  CHECK(num::cast<u8>(i32{-1}) == 255 && num::cast<i8>(u32{200}) == -56 && num::cast<i64>(std::numeric_limits<u64>::max()) == -1 && num::cast<f32>(i32{16777217}) == 16777216.0f && num::cast<f64>((i64{1} << 53) + 1) == 9007199254740992.0);
  // f64 to f32: the nearest, beyond f32: inf; f32 keeps its denormals; -0 stays -0.
  CHECK(num::cast<f32>(1e300) == std::numeric_limits<f32>::infinity() && num::cast<f32>(0.1) == 0.1f && num::mul(std::numeric_limits<f32>::denorm_min(), 2.0f) == 2 * std::numeric_limits<f32>::denorm_min() && std::signbit(num::mul(-1.0f, 0.0f)));
  CHECK(num::div(1.0f, -0.0f) == -std::numeric_limits<f32>::infinity() && std::isnan(num::div(0.0, 0.0)) && std::isnan(num::sqrt(-1.0f)) && std::signbit(num::sqrt(-0.0f)) && std::isnan(num::rem(1.0f, 0.0f)));
  // f32 is not f64 rounded at the end: 0.1f * 0.1f is a product of its own, and so is a*b + c.
  {
    const f32 a = 0.1f;
    CHECK(num::mul(a, a) == a * a && static_cast<f64>(num::mul(a, a)) != static_cast<f64>(a) * static_cast<f64>(a));
    int differs = 0;
    for (f32 x : float_edges<f32>())
      for (f32 y : float_edges<f32>())
        for (f32 c : {0.3f, -0.01f, 1.0f})
          if (std::isfinite(x * y + c) && !same(num::add(num::mul(x, y), c), static_cast<f32>(static_cast<f64>(x) * y + c))) ++differs;
    CHECK(differs > 0);  // (so the comparison above would have seen a tier that computes in double)
  }
  // The bits of a value stay its bits - a NaN's too (bits_of / of_bits; a Value holds them so).
  CHECK(num::bits_of(std::bit_cast<f32>(0x7f800001u)) == 0x7f800001u && num::bits_of(std::bit_cast<f64>(0xfff8000000000123ull)) == 0xfff8000000000123ull && num::bits_of(i8{-1}) == ~u64{0} && num::bits_of(u8{255}) == 255u);
  CHECK(num::held_bits(num::Kind::u8, 0x1ff) == 0xff && num::held_bits(num::Kind::i8, 0x80) == ~u64{0x7f} && num::held_bits(num::Kind::f32, 0xffffffff3f800000ull) == 0x3f800000u);
  // The names of the types are the C++ names.
  num::Kind k{};
  CHECK(num::kind_from_name("u32", k) && k == num::Kind::u32 && num::kind_name(num::Kind::f32) == "f32" && !num::kind_from_name("float", k) && !num::kind_from_name("int", k));
  CHECK(num::cast_kind(num::Kind::f32, num::Kind::f64) == num::Cast::floating && num::cast_name(num::cast_kind(num::Kind::i32, num::Kind::f32)) == "IntegralToFloating" && num::is_promotion(num::Kind::u8, num::Kind::i32));
}

TEST(numbers_nodes_interpreter) {
  // Each node of the numbers in the interpreter gives what numbers.h gives (and numbers.h what C++ gives: above) -
  // every operation of every type, with the edge values; and a wire of one number type into an input of another
  // is an error of the graph (no wire converts).
  using namespace number_cases;
  logic::Graph g;
  std::string error;
  json::Value j;
  CHECK(json::parse(numbers_graph_json(), j, &error) && logic::graph_from_json(j, g, &error));
  if (!error.empty()) std::printf("  numbers graph: %s\n", error.c_str());
  for (const logic::Issue& i : logic::check(g))
    if (i.error) {
      std::printf("  numbers graph: %s\n", i.text.c_str());
      CHECK(!i.error);
      break;
    }
  logic_cases::Host host;
  logic::Runtime rt(host);
  logic::Instance* inst = rt.add(g, 0, "numbers");
  CHECK(inst != nullptr);
  int wrong = 0, calls = 0;
  u64 other = 0;   // (numbers a node read as another type than its own - logic.h numbers_read_as_another_type: none)
  for (const auto& [fn, args] : numbers_calls(g)) {
    std::vector<logic::Value> got;
    ++calls;
    logic::numbers_read_as_another_type() = 0;
    const bool called = inst->call(fn, args, got);
    if (logic::numbers_read_as_another_type() && other++ < 5) std::printf("  numbers: %s reads a number as another type than its own\n", fn.c_str());
    const bool ok = called && got.size() == 1 && same_value(got[0], node_reference(fn, args));
    if (!ok && ++wrong <= 10)
      std::printf("  numbers: %s(%s%s%s) gives %s, numbers.h %s\n", fn.c_str(), args[0].as_text().c_str(), args.size() > 1 ? ", " : "", args.size() > 1 ? args[1].as_text().c_str() : "",
                  got.empty() ? "nothing" : got[0].as_text().c_str(), node_reference(fn, args).as_text().c_str());
  }
  CHECK(wrong == 0 && other == 0);
  CHECK(calls > 20000);
  // (no wire converts: an f32 into an f64 input, an i32 into an i64 one is an error of the graph)
  const char* mixed = R"({"variables":[{"name":"X","type":"f32","value":0.5},{"name":"Y","type":"f64","value":0}],
    "nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"var.set","params":{"variable":"Y"}},{"id":3,"type":"var.get","params":{"variable":"X"}}],
    "links":[[1,"then",2,"exec"],[3,"value",2,"value"]]})";
  logic::Graph m;
  CHECK(json::parse(mixed, j, &error) && logic::graph_from_json(j, m, &error));
  bool refused = false;
  for (const logic::Issue& i : logic::check(m)) refused = refused || i.error;
  CHECK(refused);
  // (a text of the time before the number types is refused with the place and the reason)
  logic::Graph old;
  CHECK(json::parse(R"({"variables":[{"name":"X","type":"float","value":0}]})", j, &error) && !logic::graph_from_json(j, old, &error) && error.find("before the number types") != std::string::npos);
}
