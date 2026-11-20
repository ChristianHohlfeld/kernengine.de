// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Case : kern::Blueprint {
  // ---- variables ----
  List<Int> In = List<Int>{0, 1, -1, 2, 31, 32, 63, 64, 65, 2147483648, -2147483648, 4294967296, -4294967296, 9007199254740991, -9007199254740991, 9007199254740992, -9007199254740992, 9007199254740994, -9007199254740994, 4611686018427387904, -4611686018427387904, 9223372036854775807, -9223372036854775807};
  List<Int64> R = List<Int64>{};
  List<Int> W = List<Int>{};

  // ---- events ----
  KERN_EVENT(on1, event_begin)
  void on1() {
    const auto n2 = [&] { return In; };
    for (auto it3 = flow_for_each<Int>(n3, n2()); it3.next();) {
      list_add(W, kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::wrap32(a); }, n3.item)));
      list_add(W, kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::wrap32s(a); }, n3.item)));
      list_add(R, int64_bits_of_float(kern::formula([](Int a) -> Int { return kern::math::wrap32(kern::math::shl(a, 40.0)); }, n3.item)));
      list_add(W, kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::band(a, 0.5); }, n3.item)));
      list_add(R, int64_bits_of_float(kern::formula([](Int a) -> Int { return kern::math::shr(a, 1.0); }, n3.item)));
      for (auto it16 = flow_for_each<Int>(n16, n2()); it16.next();) {
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::band(a, b); }, n3.item, n16.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Int a, Int b) -> Int { return kern::math::bor(a, b); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::bxor(a, b); }, n3.item, n16.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Int a, Int b) -> Int { return kern::math::bxor(a, b); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::shl(a, b); }, n3.item, n16.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Int a, Int b) -> Int { return kern::math::shl(a, b); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::shr(a, b); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::mul32(a, b); }, n3.item, n16.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Int a, Int b) -> Int { return kern::math::mul32(a, b); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::bxor(kern::math::shl(a, 13.0), kern::math::shr(b, 17.0)); }, n3.item, n16.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Int a, Int b) -> Int { return kern::math::shl(a, kern::math::band(b, 63.0)); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return kern::math::shl(a, kern::math::band(b, 63.0)); }, n3.item, n16.item)));
        list_add(W, kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::shr(kern::math::shl(a, 10.0), 3.0); }, n3.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(n3.item), kern::to<Float>(n16.item))));
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<3> { Int item = 0; Int index = 0; } n3;
  struct : kern::Node<16> { Int item = 0; Int index = 0; } n16;
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
  node(10, 0, 0);
  node(9, 0, 0);
  node(11, 0, 0);
  node(12, 0, 0);
  node(13, 0, 0);
  node(15, 0, 0);
  node(14, 0, 0);
  node(16, 0, 0);
  node(17, 0, 0);
  node(18, 0, 0);
  node(19, 0, 0);
  node(21, 0, 0);
  node(20, 0, 0);
  node(22, 0, 0);
  node(23, 0, 0);
  node(24, 0, 0);
  node(26, 0, 0);
  node(25, 0, 0);
  node(27, 0, 0);
  node(28, 0, 0);
  node(29, 0, 0);
  node(31, 0, 0);
  node(30, 0, 0);
  node(32, 0, 0);
  node(33, 0, 0);
  node(34, 0, 0);
  node(35, 0, 0);
  node(36, 0, 0);
  node(38, 0, 0);
  node(37, 0, 0);
  node(39, 0, 0);
  node(40, 0, 0);
  node(41, 0, 0);
  node(43, 0, 0);
  node(42, 0, 0);
  node(44, 0, 0);
  node(45, 0, 0);
  node(46, 0, 0);
  node(47, 0, 0);
  node(48, 0, 0);
  node(49, 0, 0);
  next_id(50);
}
