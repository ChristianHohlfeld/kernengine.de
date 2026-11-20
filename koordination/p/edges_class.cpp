// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Case : kern::Blueprint {
  // ---- variables ----
  List<Float> In = List<Float>{0.0, 1.0, -1.0, 2147483648.0, -2147483648.0, 4294967296.0, -4294967296.0, 9007199254740991.0, -9007199254740991.0, 9007199254740992.0, -9007199254740992.0, 9007199254740994.0, -9007199254740994.0, 9.223372036854776e+18, -9.223372036854776e+18, 0.5, -2.75};
  List<Int64> R = List<Int64>{};
  List<Int> W = List<Int>{};

  // ---- events ----
  KERN_EVENT(on1, event_begin)
  void on1() {
    const auto n10 = [&] { return In; };
    list_add(In, kern::formula([](Float big) -> Float { return big * 10.0; }, 1e+308));
    list_add(In, kern::formula([](Float big) -> Float { return -(big * 10.0); }, 1e+308));
    list_add(In, kern::formula([](Float big) -> Float { return big * 10.0 - big * 10.0; }, 1e+308));
    list_add(In, kern::formula([](Float zero) -> Float { return -zero; }, 0.0));
    for (auto it11 = flow_for_each<Float>(n11, n10()); it11.next();) {
      list_add(R, int64_bits_of_float(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, n11.item)));
      list_add(W, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, n11.item)));
      list_add(R, int64_bits_of_float(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, n11.item)));
      list_add(W, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bxor(kern::math::wrap32(a), kern::math::wrap32s(a)); }, n11.item)));
      list_add(W, kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, n11.item)));
      for (auto it22 = flow_for_each<Float>(n22, n10()); it22.next();) {
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::band(a, b); }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::band(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::bor(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::bxor(a, b); }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::bxor(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::shl(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::shr(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::mul32(a, b); }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::mul32(a, b); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::bxor(kern::math::shl(a, 13.0), kern::math::shr(b, 17.0)); }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::bxor(kern::math::shl(a, 13.0), kern::math::shr(b, 17.0)); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::band(kern::math::bor(a, b), kern::math::wrap32s(kern::math::bxor(a, b))); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::shl(kern::math::wrap32(a), kern::math::shr(b, 60.0)); }, n11.item, n22.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return kern::math::mul32(kern::math::shr(a, 1.0), kern::math::band(b, 4294967295.0)) + 0.5; }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::shr(kern::math::shl(a, 10.0), 3.0); }, n11.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a) -> Float { return kern::math::shr(kern::math::shl(a, 10.0), 3.0); }, n11.item)));
        list_add(R, int64_bits_of_float(kern::formula([](Float a, Float b) -> Float { return a + b; }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a * b; }, n11.item, n22.item)));
        list_add(W, kern::to<Int>(kern::formula([](Float e, Float n, Float l, Float le, Float g, Float ge) -> Float { return e + n * 2.0 + l * 4.0 + le * 8.0 + g * 16.0 + ge * 32.0; }, kern::to<Float>(math_compare<"==">(n11.item, n22.item)), kern::to<Float>(math_compare<"!=">(n11.item, n22.item)), kern::to<Float>(math_compare<"<">(n11.item, n22.item)), kern::to<Float>(math_compare<"<=">(n11.item, n22.item)), kern::to<Float>(math_compare<">">(n11.item, n22.item)), kern::to<Float>(math_compare<">=">(n11.item, n22.item)))));
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<11> { Float item = 0.0; Int index = 0; } n11;
  struct : kern::Node<22> { Float item = 0.0; Int index = 0; } n22;
};

KERN_LAYOUT(Case) {
  node(1, 0, 0);
  node(10, 0, 0);
  node(2, 0, 0);
  node(3, 0, 0);
  node(4, 0, 0);
  node(5, 0, 0);
  node(6, 0, 0);
  node(7, 0, 0);
  node(8, 0, 0);
  node(9, 0, 0);
  node(11, 0, 0);
  node(12, 0, 0);
  node(67, 0, 0);
  node(13, 0, 0);
  node(14, 0, 0);
  node(15, 0, 0);
  node(16, 0, 0);
  node(68, 0, 0);
  node(17, 0, 0);
  node(18, 0, 0);
  node(19, 0, 0);
  node(20, 0, 0);
  node(21, 0, 0);
  node(22, 0, 0);
  node(23, 0, 0);
  node(69, 0, 0);
  node(24, 0, 0);
  node(25, 0, 0);
  node(26, 0, 0);
  node(27, 0, 0);
  node(70, 0, 0);
  node(28, 0, 0);
  node(29, 0, 0);
  node(71, 0, 0);
  node(30, 0, 0);
  node(31, 0, 0);
  node(32, 0, 0);
  node(33, 0, 0);
  node(72, 0, 0);
  node(34, 0, 0);
  node(35, 0, 0);
  node(73, 0, 0);
  node(36, 0, 0);
  node(37, 0, 0);
  node(74, 0, 0);
  node(38, 0, 0);
  node(39, 0, 0);
  node(40, 0, 0);
  node(41, 0, 0);
  node(75, 0, 0);
  node(42, 0, 0);
  node(43, 0, 0);
  node(44, 0, 0);
  node(45, 0, 0);
  node(76, 0, 0);
  node(46, 0, 0);
  node(47, 0, 0);
  node(77, 0, 0);
  node(48, 0, 0);
  node(49, 0, 0);
  node(78, 0, 0);
  node(50, 0, 0);
  node(51, 0, 0);
  node(52, 0, 0);
  node(53, 0, 0);
  node(79, 0, 0);
  node(54, 0, 0);
  node(55, 0, 0);
  node(80, 0, 0);
  node(56, 0, 0);
  node(57, 0, 0);
  node(58, 0, 0);
  node(65, 0, 0);
  node(66, 0, 0);
  node(59, 0, 0);
  node(60, 0, 0);
  node(61, 0, 0);
  node(62, 0, 0);
  node(63, 0, 0);
  node(64, 0, 0);
  next_id(81);
}
