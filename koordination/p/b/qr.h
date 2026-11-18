// Generated from spawncity.klevel by tools/blueprint_cpp --classes - do not edit: the classes of Blueprints of the
// level (kern/docs/blueprint_cpp.md), without glue, named in the namespace blueprints.
#pragma once
#include "kern/logic/blueprint.h"

// ---------------------------------------------------------------------------
// The Blueprint of actor "shapes"
namespace kern_blueprint_6025b4a819f918d1 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_2_row) {
    Fixed<Float, 2> items = Fixed<Float, 2>{};
  };
  KERN_STRUCT(Float_3_row) {
    Fixed<Float, 3> items = Fixed<Float, 3>{};
  };
  KERN_STRUCT(Float_4_row) {
    Fixed<Float, 4> items = Fixed<Float, 4>{};
  };
  KERN_STRUCT(Float_5_row) {
    Fixed<Float, 5> items = Fixed<Float, 5>{};
  };
  KERN_STRUCT(Float_Vector_pair) {
    Float first = 0.0;
    Vector second = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(Int64_Int_pair) {
    Int64 first = Int64{0x0};
    Int second = 0;
  };
  KERN_STRUCT(Int_4_row) {
    Fixed<Int, 4> items = Fixed<Int, 4>{};
  };
  KERN_STRUCT(Int_Bool_pair) {
    Int first = 0;
    Bool second = false;
  };
  KERN_STRUCT(Int_Float_pair) {
    Int first = 0;
    Float second = 0.0;
  };
  KERN_STRUCT(Int_Int_pair) {
    Int first = 0;
    Int second = 0;
  };
  KERN_STRUCT(Int_row) {
    List<Int> items = List<Int>{};
  };
  KERN_STRUCT(Vector_row) {
    List<Vector> items = List<Vector>{};
  };
  KERN_STRUCT(Vector_row_row) {
    List<Vector_row> items = List<Vector_row>{};
  };
};

KERN_LAYOUT(shapes) {
  next_id(1);
}


}  // namespace kern_blueprint_6025b4a819f918d1

// ---------------------------------------------------------------------------
// The Blueprint of actor "qr_code"
namespace kern_blueprint_f1248d4ad2e4cc95 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct qr_code : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(qr_Code) {
    Int version = 0;
    Int size = 0;
    List<Int> modules = List<Int>{};
  };

  // ---- functions ----
  struct Code_darkResult { Bool Result = false; };
  KERN_PURE Code_darkResult Code_dark(const qr_Code& self, Int x = 0, Int y = 0) {
    return {logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(x), 0.0), math_compare<">=">(kern::to<Float>(y), 0.0)), math_compare<"<">(kern::to<Float>(x), kern::to<Float>(self.size))), math_compare<"<">(kern::to<Float>(y), kern::to<Float>(self.size))), math_compare<"!=">(kern::to<Float>(self.modules[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))]), 0.0))};
  }
};

KERN_LAYOUT(qr_code) {
  node(22, 0, 0);
  node(23, 280, 0);
  node(24, 160, 150);
  node(25, 160, 240);
  node(26, 160, 330);
  node(27, 160, 420);
  node(28, 160, 510);
  node(29, 160, 600);
  node(30, 160, 690);
  node(31, 160, 780);
  node(32, 160, 870);
  node(33, 160, 960);
  node(34, 160, 1050);
  node(35, 160, 1140);
  node(36, 160, 1230);
  node(37, 160, 1320);
  next_id(374);
}


}  // namespace kern_blueprint_f1248d4ad2e4cc95

// ---------------------------------------------------------------------------
// The Blueprint of actor "qr_code_cpp"
namespace kern_blueprint_fe59a0c6af5496ca {

using qr_code = ::kern_blueprint_f1248d4ad2e4cc95::qr_code;
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct qr_code_cpp : virtual kern::Library, virtual qr_code, virtual shapes {
  // ---- types ----
  KERN_STRUCT(qr_Grid) {
    Int size = 0;
    List<Int> m = List<Int>{};
    List<Int> fn = List<Int>{};
  };

  // ---- variables ----
  /// Level L per version (ISO/IEC 18004 table 9): EC codewords per block, number of blocks.
  KERN_CONST Fixed<Int, 41> kEccPerBlock = Fixed<Int, 41>{-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
  KERN_CONST Fixed<Int, 41> kBlocks = Fixed<Int, 41>{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25};
  /// format bits of level L
  KERN_CONST Int kLevelL = 1;

  // ---- functions ----
  struct raw_modulesResult { Int Result = 0; };
  KERN_PURE raw_modulesResult raw_modules(Int v = 0) {
    Int r = 0;
    Int n = 0;
    r = kern::to<Int>(kern::formula([](Float v) -> Float { return (16.0 * v + 128.0) * v + 64.0; }, kern::to<Float>(v)));
    if (math_compare<">=">(kern::to<Float>(v), 2.0)) {
      n = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::trunc(kern::math::div(v, 7.0)) + 2.0; }, kern::to<Float>(v)));
      r = kern::to<Int>(kern::formula([](Float r, Float n) -> Float { return r - ((25.0 * n - 10.0) * n - 55.0); }, kern::to<Float>(r), kern::to<Float>(n)));
      if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
        r = kern::to<Int>(kern::formula([](Float r) -> Float { return r - 36.0; }, kern::to<Float>(r)));
      }
    }
    return {r};
  }

  struct data_codewordsResult { Int Result = 0; };
  KERN_PURE data_codewordsResult data_codewords(Int v = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)) - a_2 * a_3; }, kern::to<Float>(raw_modules(v).Result), kern::to<Float>(list_get<Int>(kEccPerBlock, v)), kern::to<Float>(list_get<Int>(kBlocks, v))))};
  }

  struct gf_mulResult { Int Result = 0; };
  KERN_PURE gf_mulResult gf_mul(Int x = 0, Int y = 0) {
    Int z = 0;
    Int i = 0;
    z = 0;
    i = 7;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      z = kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(z, 1.0)), kern::math::mul32(kern::math::shr(z, 7.0), 285.0)); }, kern::to<Float>(z)));
      z = kern::to<Int>(kern::formula([](Float z, Float y, Float i, Float x) -> Float { return kern::math::bxor(z, kern::math::mul32(kern::math::band(kern::math::wrap32(kern::math::shr(y, i)), 1.0), x)); }, kern::to<Float>(z), kern::to<Float>(y), kern::to<Float>(i), kern::to<Float>(x)));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::band(z, 255.0); }, kern::to<Float>(z)))};
  }

  struct rs_divisorResult { List<Int> Result = List<Int>{}; };
  KERN_PURE rs_divisorResult rs_divisor(Int degree = 0) {
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int root = 0;
    Int j = 0;
    made = List<Int>{};
    list_resize(made, degree, 0);
    r = std::move(made);
    r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(r))))] = 1;
    root = 1;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(degree)))); it10.next();) {
      j = 0;
      while (loop(math_compare<">">(kern::formula([](Float j, Float a) -> Float { return (j < 0.0) != (a < 0.0) ? a < 0.0 : j < a; }, kern::to<Float>(j), kern::to<Float>(list_length<Int>(r))), 0.5))) {
        r[j] = gf_mul(r[j], root).Result;
        if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float j) -> Float { return j + 1.0; }, kern::to<Float>(j))))), kern::to<Float>(list_length<Int>(r))), 0.5)) {
          r[j] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[j]), kern::to<Float>(r[kern::to<Int>(kern::formula([](Float j) -> Float { return j + 1.0; }, kern::to<Float>(j)))])));
        }
        j += 1;
      }
      root = gf_mul(root, 2).Result;
    }
    return {std::move(r)};
  }

  struct rs_remainderResult { List<Int> Result = List<Int>{}; };
  KERN_PURE rs_remainderResult rs_remainder(const List<Int>& data, const List<Int>& the_div) {
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int b = 0;
    Int f = 0;
    Int i = 0;
    made = List<Int>{};
    list_resize(made, list_length<Int>(the_div), 0);
    r = std::move(made);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it11.next();) {
      b = data[n11.index];
      f = kern::to<Int>(kern::formula([](Float b, Float a) -> Float { return kern::math::band(kern::math::bxor(b, a), 255.0); }, kern::to<Float>(b), kern::to<Float>(r[0])));
      list_remove_at(r, 0);
      list_add(r, 0);
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Int>(r))), 0.5))) {
        r[i] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[i]), kern::to<Float>(gf_mul(the_div[i], f).Result)));
        i += 1;
      }
    }
    return {std::move(r)};
  }

  struct alignment_positionsResult { List<Int> Result = List<Int>{}; };
  KERN_PURE alignment_positionsResult alignment_positions(Int v = 0) {
    Int n = 0;
    Int size = 0;
    Int step = 0;
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int i = 0;
    Int pos = 0;
    if (math_compare<"==">(kern::to<Float>(v), 1.0)) {
      return {List<Int>{}};
    }
    n = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::trunc(kern::math::div(v, 7.0)) + 2.0; }, kern::to<Float>(v)));
    size = kern::to<Int>(kern::formula([](Float v) -> Float { return v * 4.0 + 17.0; }, kern::to<Float>(v)));
    step = kern::to<Int>(kern::formula([](Float v, Float n) -> Float { return v == 32.0 ? 26.0 : kern::math::trunc(kern::math::div(v * 4.0 + n * 2.0 + 1.0, n * 2.0 - 2.0)) * 2.0; }, kern::to<Float>(v), kern::to<Float>(n)));
    made = List<Int>{};
    list_resize(made, n, 0);
    r = std::move(made);
    r[0] = 6;
    i = kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)));
    pos = kern::to<Int>(kern::formula([](Float size) -> Float { return size - 7.0; }, kern::to<Float>(size)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 1.0))) {
      r[i] = pos;
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
      pos = kern::to<Int>(kern::formula([](Float pos, Float step) -> Float { return pos - step; }, kern::to<Float>(pos), kern::to<Float>(step)));
    }
    return {std::move(r)};
  }

  void Grid_set(qr_Grid& self, Int x = 0, Int y = 0, Bool dark = false) {
    self.m[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = kern::to<Int>(dark);
    self.fn[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = 1;
  }

  void finder(qr_Grid& g, Int cx = 0, Int cy = 0) {
    Bool skip = false;
    Int x = 0;
    Int y = 0;
    Int d = 0;
    for (auto it12 = flow_for(n12, -4, 4); it12.next();) {
      for (auto it13 = flow_for(n13, -4, 4); it13.next();) {
        skip = false;
        x = kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return cx + index; }, kern::to<Float>(cx), kern::to<Float>(n13.index)));
        y = kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return cy + index; }, kern::to<Float>(cy), kern::to<Float>(n12.index)));
        if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(x), 0.0), math_compare<"<">(kern::to<Float>(y), 0.0)), math_compare<">=">(kern::to<Float>(x), kern::to<Float>(g.size))), math_compare<">=">(kern::to<Float>(y), kern::to<Float>(g.size)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          d = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n13.index), kern::to<Float>(n12.index)));
          Grid_set(g, x, y, logic_and(math_compare<"!=">(kern::to<Float>(d), 2.0), math_compare<"!=">(kern::to<Float>(d), 4.0)));
        }
      }
    }
  }

  struct format_bits_bitResult { Bool Result = false; };
  KERN_PURE format_bits_bitResult format_bits_bit(Int bits = 0, Int i = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits, Float i) -> Float { return kern::math::band(kern::math::shr(bits, i), 1.0); }, kern::to<Float>(bits), kern::to<Float>(i))))), 0.0)};
  }

  void format_bits(qr_Grid& g, Int mask = 0) {
    Int data = 0;
    Int rem = 0;
    Int bits = 0;
    data = kern::to<Int>(kern::formula([](Float kLevelL, Float mask) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kLevelL, 3.0)), mask); }, kern::to<Float>(kLevelL), kern::to<Float>(mask)));
    rem = data;
    for (auto it14 = flow_for(n14, 0, 9); it14.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    bits = kern::to<Int>(kern::formula([](Float data, Float rem) -> Float { return kern::math::bxor(kern::math::bor(kern::math::wrap32(kern::math::shl(data, 10.0)), rem), 21522.0); }, kern::to<Float>(data), kern::to<Float>(rem)));
    for (auto it15 = flow_for(n15, 0, 5); it15.next();) {
      Grid_set(g, 8, n15.index, format_bits_bit(bits, n15.index).Result);
    }
    Grid_set(g, 8, 7, format_bits_bit(bits, 6).Result);
    Grid_set(g, 8, 8, format_bits_bit(bits, 7).Result);
    Grid_set(g, 7, 8, format_bits_bit(bits, 8).Result);
    for (auto it16 = flow_for(n16, 9, 14); it16.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float index) -> Float { return 14.0 - index; }, kern::to<Float>(n16.index))), 8, format_bits_bit(bits, n16.index).Result);
    }
    for (auto it17 = flow_for(n17, 0, 7); it17.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 1.0 - index; }, kern::to<Float>(g.size), kern::to<Float>(n17.index))), 8, format_bits_bit(bits, n17.index).Result);
    }
    for (auto it18 = flow_for(n18, 8, 14); it18.next();) {
      Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 15.0 + index; }, kern::to<Float>(g.size), kern::to<Float>(n18.index))), format_bits_bit(bits, n18.index).Result);
    }
    Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 8.0; }, kern::to<Float>(g.size))), true);
  }

  void encode_put(List<Bool>& bits, Int val = 0, Int len = 0) {
    Int i = 0;
    i = kern::to<Int>(kern::formula([](Float len) -> Float { return len - 1.0; }, kern::to<Float>(len)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      list_add(bits, math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float val, Float i) -> Float { return kern::math::band(kern::math::shr(val, i), 1.0); }, kern::to<Float>(val), kern::to<Float>(i))))), 0.0));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
  }

  struct encodeResult { Bool Result = false; };
  encodeResult encode(const Text& text, qr_Code& out) {
    Int n = 0;
    Int v = 0;
    Bool stop = false;
    Int count_bits = 0;
    List<Bool> bits = List<Bool>{};
    Int c = 0;
    Int cap = 0;
    Int pad = 0;
    List<Int> data = List<Int>{};
    List<Int> made = List<Int>{};
    Int blocks = 0;
    Int ecc = 0;
    Int raw = 0;
    Int short_blocks = 0;
    Int short_len = 0;
    List<Int> the_div = List<Int>{};
    List<Int_row> bl = List<Int_row>{};
    Int i = 0;
    Int k = 0;
    Int len = 0;
    List<Int> d = List<Int>{};
    List<Int> made_2 = List<Int>{};
    Int first = 0;
    Int last = 0;
    List<Int> e = List<Int>{};
    Int place = 0;
    List<Int> words = List<Int>{};
    qr_Grid g = qr_Grid{0, List<Int>{}, List<Int>{}};
    Int count = 0;
    Int fill = 0;
    Int count_2 = 0;
    Int fill_2 = 0;
    Int i_2 = 0;
    List<Int> al = List<Int>{};
    Bool skip = false;
    Int rem = 0;
    Int vb = 0;
    Bool b = false;
    Int a = 0;
    Int c_2 = 0;
    Int i_3 = 0;
    Int right = 0;
    Int vert = 0;
    Bool skip_2 = false;
    Int x = 0;
    Bool up = false;
    Int y = 0;
    Int at = 0;
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(text_length(text))));
    v = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<=">(kern::to<Float>(v), 40.0)))) {
      count_bits = kern::to<Int>(kern::formula([](Float v) -> Float { return v <= 9.0 ? 8.0 : 16.0; }, kern::to<Float>(v)));
      if (logic_and(math_compare<"<=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float count_bits, Float n) -> Float { return 4.0 + count_bits + 8.0 * n; }, kern::to<Float>(count_bits), kern::to<Float>(n))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(data_codewords(v).Result)))))), logic_or(math_compare<"==">(kern::to<Float>(count_bits), 16.0), math_compare<"<">(kern::to<Float>(n), 256.0)))) {
        stop = true;
      }
      if (logic_not(stop)) {
        v += 1;
      }
    }
    if (math_compare<">">(kern::to<Float>(v), 40.0)) {
      return {false};
    }
    bits = List<Bool>{};
    encode_put(bits, 4, 4);
    encode_put(bits, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32(n); }, kern::to<Float>(n))), kern::to<Int>(kern::formula([](Float v) -> Float { return v <= 9.0 ? 8.0 : 16.0; }, kern::to<Float>(v))));
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it19.next();) {
      c = text_code_at(text, n19.index);
      encode_put(bits, c, 8);
    }
    cap = kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(data_codewords(v).Result)));
    encode_put(bits, 0, kern::to<Int>(kern::formula([](Float cap, Float a) -> Float { return kern::math::wrap32s(kern::math::min(4.0, cap - a)); }, kern::to<Float>(cap), kern::to<Float>(list_length<Bool>(bits)))));
    while (loop(math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::mod(a, 8.0) != 0.0; }, kern::to<Float>(list_length<Bool>(bits))), 0.5))) {
      list_add(bits, false);
    }
    pad = 236;
    while (loop(math_compare<">">(kern::formula([](Float a, Float cap) -> Float { return (a < 0.0) != (cap < 0.0) ? cap < 0.0 : a < cap; }, kern::to<Float>(list_length<Bool>(bits)), kern::to<Float>(cap)), 0.5))) {
      encode_put(bits, pad, 8);
      pad = kern::to<Int>(kern::formula([](Float pad) -> Float { return kern::math::bxor(pad, kern::math::bxor(236.0, 17.0)); }, kern::to<Float>(pad)));
    }
    made = List<Int>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(list_length<Bool>(bits)))), 0);
    data = std::move(made);
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Bool>(bits))))); it20.next();) {
      if (bits[n20.index]) {
        data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::bor(a, kern::math::band(kern::math::wrap32s(kern::math::shl(1.0, 7.0 - kern::math::band(index, 7.0))), 255.0)); }, kern::to<Float>(data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))]), kern::to<Float>(n20.index)));
      }
    }
    blocks = list_get<Int>(kBlocks, v);
    ecc = list_get<Int>(kEccPerBlock, v);
    raw = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(raw_modules(v).Result)));
    short_blocks = kern::to<Int>(kern::formula([](Float blocks, Float raw) -> Float { return blocks - kern::math::mod(raw, blocks); }, kern::to<Float>(blocks), kern::to<Float>(raw)));
    short_len = kern::to<Int>(kern::formula([](Float raw, Float blocks) -> Float { return kern::math::trunc(kern::math::div(raw, blocks)); }, kern::to<Float>(raw), kern::to<Float>(blocks)));
    the_div = rs_divisor(ecc).Result;
    bl = List<Int_row>{};
    i = 0;
    k = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(blocks)))) {
      len = kern::to<Int>(kern::formula([](Float short_len, Float ecc, Float i, Float short_blocks) -> Float { return short_len - ecc + (i < short_blocks ? 0.0 : 1.0); }, kern::to<Float>(short_len), kern::to<Float>(ecc), kern::to<Float>(i), kern::to<Float>(short_blocks)));
      made_2 = List<Int>{};
      first = k;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(k), kern::to<Float>(len)))))));
      list_reserve(made_2, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
      for (auto it21 = flow_for(n21, first, last); it21.next();) {
        list_add(made_2, list_get<Int>(data, n21.index));
      }
      d = std::move(made_2);
      k += len;
      e = rs_remainder(d, the_div).Result;
      if (math_compare<"<">(kern::to<Float>(i), kern::to<Float>(short_blocks))) {
        list_add(d, 0);
      }
      place = list_length<Int>(d);
      list_insert_list(d, place, std::move(e));
      list_add(bl, struct_make<Int_row>(d));
      i += 1;
    }
    words = List<Int>{};
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(bl[0].items))))); it22.next();) {
      for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int_row>(bl))))); it23.next();) {
        if (logic_or(math_compare<"!=">(kern::to<Float>(n22.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float short_len, Float ecc) -> Float { return short_len - ecc; }, kern::to<Float>(short_len), kern::to<Float>(ecc)))))), math_compare<">">(kern::formula([](Float index, Float short_blocks) -> Float { return !((index < 0.0) != (short_blocks < 0.0) ? short_blocks < 0.0 : index < short_blocks); }, kern::to<Float>(n23.index), kern::to<Float>(short_blocks)), 0.5))) {
          list_add(words, bl[n23.index].items[n22.index]);
        }
      }
    }
    g = struct_make<qr_Grid>(kern::to<Int>(kern::formula([](Float v) -> Float { return v * 4.0 + 17.0; }, kern::to<Float>(v))), List<Int>{}, List<Int>{});
    count = kern::to<Int>(kern::formula([](Float size) -> Float { return size * size; }, kern::to<Float>(g.size)));
    fill = 0;
    list_clear(g.m);
    list_resize(g.m, count, fill);
    count_2 = list_length<Int>(g.m);
    fill_2 = 0;
    list_clear(g.fn);
    list_resize(g.fn, count_2, fill_2);
    i_2 = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(g.size)))) {
      Grid_set(g, 6, i_2, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      Grid_set(g, i_2, 6, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      i_2 += 1;
    }
    finder(g, 3, 3);
    finder(g, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 4.0; }, kern::to<Float>(g.size))), 3);
    finder(g, 3, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 4.0; }, kern::to<Float>(g.size))));
    al = alignment_positions(v).Result;
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(al))))); it24.next();) {
      for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(al))))); it25.next();) {
        skip = false;
        if (logic_or(logic_or(logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), 0.0)), logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(al))))))))), logic_and(math_compare<"==">(kern::to<Float>(n24.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(al))))))), math_compare<"==">(kern::to<Float>(n25.index), 0.0)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it26 = flow_for(n26, -2, 2); it26.next();) {
            for (auto it27 = flow_for(n27, -2, 2); it27.next();) {
              Grid_set(g, kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return a + index; }, kern::to<Float>(al[n24.index]), kern::to<Float>(n27.index))), kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return a + index; }, kern::to<Float>(al[n25.index]), kern::to<Float>(n26.index))), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n27.index), kern::to<Float>(n26.index))))), 1.0));
            }
          }
        }
      }
    }
    format_bits(g, 0);
    if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
      rem = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(v); }, kern::to<Float>(v)));
      for (auto it28 = flow_for(n28, 0, 11); it28.next();) {
        rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 11.0), 7973.0)); }, kern::to<Float>(rem)));
      }
      vb = kern::to<Int>(kern::formula([](Float v, Float rem) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kern::math::wrap32(v), 12.0)), rem); }, kern::to<Float>(v), kern::to<Float>(rem)));
      for (auto it29 = flow_for(n29, 0, 17); it29.next();) {
        b = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float vb, Float index) -> Float { return kern::math::band(kern::math::shr(vb, index), 1.0); }, kern::to<Float>(vb), kern::to<Float>(n29.index))))), 0.0);
        a = kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 11.0 + kern::math::mod(index, 3.0); }, kern::to<Float>(g.size), kern::to<Float>(n29.index)));
        c_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::trunc(kern::math::div(index, 3.0)); }, kern::to<Float>(n29.index)));
        Grid_set(g, a, c_2, b);
        Grid_set(g, c_2, a, b);
      }
    }
    i_3 = 0;
    right = kern::to<Int>(kern::formula([](Float size) -> Float { return size - 1.0; }, kern::to<Float>(g.size)));
    while (loop(math_compare<">=">(kern::to<Float>(right), 1.0))) {
      if (math_compare<"==">(kern::to<Float>(right), 6.0)) {
        right = 5;
      }
      vert = 0;
      while (loop(math_compare<"<">(kern::to<Float>(vert), kern::to<Float>(g.size)))) {
        for (auto it30 = flow_for(n30, 0, 1); it30.next();) {
          skip_2 = false;
          x = kern::to<Int>(kern::formula([](Float right, Float index) -> Float { return right - index; }, kern::to<Float>(right), kern::to<Float>(n30.index)));
          up = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float right) -> Float { return kern::math::band(right + 1.0, 2.0); }, kern::to<Float>(right))))), 0.0);
          y = kern::to<Int>(kern::formula([](Float up, Float size, Float vert) -> Float { return up != 0.0 ? size - 1.0 - vert : vert; }, kern::to<Float>(up), kern::to<Float>(g.size), kern::to<Float>(vert)));
          at = kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(g.size), kern::to<Float>(x)));
          if (math_compare<"!=">(kern::to<Float>(g.fn[at]), 0.0)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            if (math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return (i_3 < 0.0) != (a < 0.0) ? a < 0.0 : i_3 < a; }, kern::to<Float>(i_3), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(list_length<Int>(words))))))), 0.5)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return kern::math::band(kern::math::band(kern::math::shr(a, 7.0 - kern::math::band(i_3, 7.0)), 1.0), 255.0); }, kern::to<Float>(words[kern::to<Int>(kern::formula([](Float i_3) -> Float { return kern::math::shr(i_3, 3.0); }, kern::to<Float>(i_3)))]), kern::to<Float>(i_3)));
              i_3 += 1;
            }
            if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x, Float y) -> Float { return kern::math::mod(x + y, 2.0); }, kern::to<Float>(x), kern::to<Float>(y))))), 0.0)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bxor(a, 1.0); }, kern::to<Float>(g.m[at])));
            }
          }
        }
        vert += 1;
      }
      right = kern::to<Int>(kern::formula([](Float right) -> Float { return right - 2.0; }, kern::to<Float>(right)));
    }
    format_bits(g, 0);
    out.version = v;
    out.size = g.size;
    var_take(n31, g.m);
    out.modules = std::move(n31.value);
    return {true};
  }

  struct read_formatResult { Bool Result = false; };
  read_formatResult read_format(const qr_Code& c, Int& level_bits, Int& mask) {
    Int bits = 0;
    Int rem = 0;
    bits = 0;
    for (auto it32 = flow_for(n32, 0, 5); it32.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, n32.index).Result), kern::to<Float>(n32.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 6.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 7).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 7.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 8).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 8.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 7, 8).Result)));
    for (auto it33 = flow_for(n33, 9, 14); it33.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, kern::to<Int>(kern::formula([](Float index) -> Float { return 14.0 - index; }, kern::to<Float>(n33.index))), 8).Result), kern::to<Float>(n33.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::bxor(bits, 21522.0); }, kern::to<Float>(bits)));
    level_bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 13.0); }, kern::to<Float>(bits)));
    mask = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(kern::math::shr(bits, 10.0), 7.0); }, kern::to<Float>(bits)));
    rem = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 10.0); }, kern::to<Float>(bits)));
    for (auto it34 = flow_for(n34, 0, 9); it34.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    return {math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::band(rem, 1023.0); }, kern::to<Float>(rem))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(bits, 1023.0); }, kern::to<Float>(bits))))))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct { List<Int> value = List<Int>{}; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
};

KERN_LAYOUT(qr_code_cpp) {
  node(361, 0, 8304);
  node(362, 288, 8360);
  node(363, 240, 8512);
  node(364, 576, 8416);
  node(365, 528, 8568);
  node(366, 864, 8472);
  node(367, 816, 8624);
  node(368, 1152, 8528);
  node(369, 1104, 8680);
  node(370, 1104, 8784);
  node(371, 1104, 8888);
  node(372, 1440, 8584);
  node(373, 1392, 8736);
  node(35, 1680, 0);
  node(36, 1560, 150);
  node(37, 1560, 240);
  node(38, 1960, 0);
  node(39, 1840, 150);
  node(40, 0, 470);
  node(41, 280, 470);
  node(42, 160, 620);
  node(43, 160, 710);
  node(44, 160, 800);
  node(45, 160, 890);
  node(46, 160, 980);
  node(47, 160, 1070);
  node(48, 0, 1210);
  node(49, 280, 1210);
  node(50, 560, 1210);
  node(51, 840, 1210);
  node(52, 720, 1360);
  node(53, 720, 1450);
  node(54, 1120, 1210);
  node(55, 1000, 1360);
  node(56, 1000, 1450);
  node(57, 1400, 1210);
  node(58, 1280, 1360);
  node(59, 1280, 1450);
  node(60, 1280, 1540);
  node(61, 1680, 1210);
  node(62, 1560, 1360);
  node(63, 1560, 1450);
  node(64, 1960, 1210);
  node(65, 1840, 1360);
  node(66, 1840, 1450);
  node(67, 0, 1680);
  node(70, 840, 1680);
  node(358, 1128, 1736);
  node(78, 1680, 1680);
  node(79, 1560, 1830);
  node(80, 1960, 1680);
  node(81, 1840, 1830);
  node(82, 1840, 1920);
  node(83, 1840, 2010);
  node(84, 2240, 1680);
  node(10, 2520, 1680);
  node(85, 2400, 1830);
  node(86, 2800, 1680);
  node(87, 3080, 1680);
  node(88, 2960, 1830);
  node(89, 2960, 1920);
  node(90, 2960, 2010);
  node(91, 2960, 2100);
  node(92, 2960, 2190);
  node(93, 3360, 1680);
  node(94, 3240, 1830);
  node(95, 3240, 1920);
  node(96, 3240, 2010);
  node(97, 3240, 2100);
  node(98, 3240, 2190);
  node(99, 3640, 1680);
  node(100, 3520, 1830);
  node(101, 3520, 1920);
  node(102, 3520, 2010);
  node(103, 3520, 2100);
  node(104, 3520, 2190);
  node(105, 3520, 2280);
  node(106, 3520, 2370);
  node(107, 3920, 1680);
  node(108, 3800, 1830);
  node(109, 3800, 1920);
  node(110, 3800, 2010);
  node(111, 3800, 2100);
  node(112, 3800, 2190);
  node(113, 3800, 2280);
  node(114, 3800, 2370);
  node(115, 4200, 1680);
  node(116, 4480, 1680);
  node(117, 4360, 1830);
  node(118, 4360, 1920);
  node(119, 4760, 1680);
  node(120, 4640, 1830);
  node(121, 0, 2510);
  node(123, 560, 2510);
  node(359, 848, 2568);
  node(124, 440, 2660);
  node(125, 440, 2750);
  node(134, 1680, 2510);
  node(135, 1560, 2660);
  node(11, 1960, 2510);
  node(136, 1840, 2660);
  node(137, 1840, 2750);
  node(138, 1840, 2840);
  node(139, 2240, 2510);
  node(140, 2120, 2660);
  node(141, 2520, 2510);
  node(142, 2400, 2660);
  node(143, 2400, 2750);
  node(144, 2400, 2840);
  node(145, 2800, 2510);
  node(146, 3080, 2510);
  node(147, 3360, 2510);
  node(148, 3640, 2510);
  node(149, 3520, 2660);
  node(150, 3520, 2750);
  node(151, 3520, 2840);
  node(152, 3520, 2930);
  node(153, 3520, 3020);
  node(154, 3920, 2510);
  node(155, 3800, 2660);
  node(156, 3800, 2750);
  node(157, 3800, 2840);
  node(158, 3800, 2930);
  node(159, 3800, 3020);
  node(160, 3800, 3110);
  node(161, 3800, 3200);
  node(162, 3800, 3290);
  node(163, 4200, 2510);
  node(164, 4480, 2510);
  node(165, 4360, 2660);
  node(166, 0, 3430);
  node(167, 280, 3430);
  node(168, 160, 3580);
  node(169, 560, 3430);
  node(170, 840, 3430);
  node(171, 720, 3580);
  node(172, 1120, 3430);
  node(173, 1000, 3580);
  node(174, 1400, 3430);
  node(175, 1280, 3580);
  node(176, 1280, 3670);
  node(178, 1960, 3430);
  node(360, 2248, 3488);
  node(179, 1840, 3580);
  node(186, 2800, 3430);
  node(187, 2680, 3580);
  node(188, 3080, 3430);
  node(189, 3360, 3430);
  node(190, 3240, 3580);
  node(191, 3240, 3670);
  node(192, 3640, 3430);
  node(193, 3520, 3580);
  node(194, 3520, 3670);
  node(195, 3920, 3430);
  node(196, 3800, 3580);
  node(197, 3800, 3670);
  node(198, 4200, 3430);
  node(199, 4080, 3580);
  node(200, 4080, 3670);
  node(201, 4480, 3430);
  node(202, 4360, 3580);
  node(203, 4360, 3670);
  node(204, 4760, 3430);
  node(205, 4640, 3580);
  node(206, 4640, 3670);
  node(207, 4640, 3760);
  node(208, 5040, 3430);
  node(209, 4920, 3580);
  node(210, 0, 3990);
  node(211, 280, 3990);
  node(212, 160, 4140);
  node(213, 160, 4230);
  node(214, 560, 3990);
  node(215, 440, 4140);
  node(216, 440, 4230);
  node(217, 0, 4370);
  node(12, 280, 4370);
  node(13, 560, 4370);
  node(218, 840, 4370);
  node(219, 1120, 4370);
  node(220, 1000, 4520);
  node(221, 1400, 4370);
  node(222, 1280, 4520);
  node(223, 1680, 4370);
  node(224, 1560, 4520);
  node(225, 1560, 4610);
  node(226, 1560, 4700);
  node(227, 1560, 4790);
  node(228, 1560, 4880);
  node(229, 1560, 4970);
  node(230, 1560, 5060);
  node(231, 1560, 5150);
  node(232, 1560, 5240);
  node(233, 1560, 5330);
  node(234, 1560, 5420);
  node(235, 1560, 5510);
  node(236, 1560, 5600);
  node(237, 1960, 4370);
  node(238, 2240, 4370);
  node(239, 2120, 4520);
  node(240, 2120, 4610);
  node(241, 2520, 4370);
  node(242, 2400, 4520);
  node(243, 2800, 4370);
  node(244, 2680, 4520);
  node(245, 2680, 4610);
  node(246, 2680, 4700);
  node(247, 2680, 4790);
  node(248, 2680, 4880);
  node(249, 2680, 4970);
  node(250, 2680, 5060);
  node(251, 0, 5740);
  node(252, 280, 5740);
  node(253, 160, 5890);
  node(254, 160, 5980);
  node(255, 160, 6070);
  node(256, 0, 6210);
  node(257, 280, 6210);
  node(258, 160, 6360);
  node(259, 160, 6450);
  node(260, 560, 6210);
  node(261, 440, 6360);
  node(14, 840, 6210);
  node(262, 1120, 6210);
  node(263, 1000, 6360);
  node(264, 1000, 6450);
  node(265, 1400, 6210);
  node(266, 1280, 6360);
  node(267, 1280, 6450);
  node(268, 1280, 6540);
  node(15, 1680, 6210);
  node(269, 1960, 6210);
  node(270, 1840, 6360);
  node(271, 1840, 6450);
  node(272, 2240, 6210);
  node(273, 2120, 6360);
  node(274, 2120, 6450);
  node(275, 2520, 6210);
  node(276, 2400, 6360);
  node(277, 2400, 6450);
  node(278, 2800, 6210);
  node(279, 2680, 6360);
  node(280, 2680, 6450);
  node(16, 3080, 6210);
  node(281, 3360, 6210);
  node(282, 3240, 6360);
  node(283, 3240, 6450);
  node(284, 3240, 6540);
  node(17, 3640, 6210);
  node(285, 3920, 6210);
  node(286, 3800, 6360);
  node(287, 3800, 6450);
  node(288, 3800, 6540);
  node(289, 3800, 6630);
  node(18, 4200, 6210);
  node(290, 4480, 6210);
  node(291, 4360, 6360);
  node(292, 4360, 6450);
  node(293, 4360, 6540);
  node(294, 4360, 6630);
  node(295, 4760, 6210);
  node(296, 4640, 6360);
  node(297, 4640, 6450);
  node(298, 0, 6770);
  node(299, 280, 6770);
  node(300, 160, 6920);
  node(301, 560, 6770);
  node(302, 440, 6920);
  node(303, 440, 7010);
  node(304, 840, 6770);
  node(305, 720, 6920);
  node(306, 720, 7010);
  node(307, 720, 7100);
  node(308, 720, 7190);
  node(309, 1120, 6770);
  node(310, 1000, 6920);
  node(311, 1000, 7010);
  node(374, 0, 9168);
  node(375, 288, 9224);
  node(376, 240, 9376);
  node(377, 240, 9480);
  node(378, 240, 9584);
  node(379, 576, 9280);
  node(380, 864, 9336);
  node(381, 1152, 9392);
  node(382, 1104, 9544);
  node(383, 1104, 9648);
  node(384, 1104, 9752);
  node(385, 1104, 9856);
  node(386, 1104, 9960);
  node(387, 1440, 9448);
  node(388, 1392, 9600);
  node(389, 1392, 9704);
  node(390, 1728, 9504);
  node(391, 1680, 9656);
  node(392, 1680, 9760);
  node(393, 1680, 9864);
  node(394, 1680, 9968);
  node(395, 1680, 10072);
  node(396, 1680, 10176);
  node(397, 1680, 10280);
  node(398, 1680, 10384);
  node(399, 1680, 10488);
  node(400, 1680, 10592);
  node(401, 1680, 10696);
  node(402, 1680, 10800);
  node(403, 1680, 10904);
  node(404, 1680, 11008);
  node(405, 1680, 11112);
  node(406, 2016, 9560);
  node(407, 2304, 9616);
  node(408, 2256, 9768);
  node(409, 2256, 9872);
  node(410, 2592, 9672);
  node(411, 2880, 9728);
  node(412, 2832, 9880);
  node(413, 2832, 9984);
  node(414, 3168, 9784);
  node(415, 3456, 9840);
  node(416, 3744, 9896);
  node(417, 4032, 9952);
  node(418, 3984, 10104);
  node(419, 3984, 10208);
  node(420, 3984, 10312);
  node(421, 3984, 10416);
  node(19, 560, 7330);
  node(422, 512, 7480);
  node(423, 512, 7584);
  node(424, 512, 7688);
  node(425, 848, 7384);
  node(426, 800, 7536);
  node(427, 800, 7640);
  node(428, 1136, 7440);
  node(429, 1088, 7592);
  node(430, 1424, 7496);
  node(431, 1376, 7648);
  node(432, 1376, 7752);
  node(433, 1376, 7856);
  node(434, 1712, 7552);
  node(435, 1664, 7704);
  node(436, 1664, 7808);
  node(437, 1664, 7912);
  node(438, 1664, 8016);
  node(439, 2000, 7608);
  node(440, 1952, 7760);
  node(441, 1952, 7864);
  node(442, 1952, 7968);
  node(443, 1952, 8072);
  node(444, 2288, 7664);
  node(445, 2576, 7720);
  node(446, 2864, 7776);
  node(447, 2816, 7928);
  node(448, 2816, 8032);
  node(449, 2816, 8136);
  node(450, 2816, 8240);
  node(451, 2816, 8344);
  node(452, 3152, 7832);
  node(453, 3104, 7984);
  node(454, 3440, 7888);
  node(455, 3392, 8040);
  node(456, 3392, 8144);
  node(457, 3728, 7944);
  node(458, 4016, 8000);
  node(459, 3968, 8152);
  node(460, 3968, 8256);
  node(461, 3968, 8360);
  node(462, 4304, 8056);
  node(463, 4256, 8208);
  node(20, 1960, 7330);
  node(464, 1912, 7480);
  node(465, 1912, 7584);
  node(466, 1912, 7688);
  node(467, 2248, 7384);
  node(468, 2200, 7536);
  node(469, 2536, 7440);
  node(470, 2488, 7592);
  node(471, 2488, 7696);
  node(472, 2488, 7800);
  node(473, 2488, 7904);
  node(474, 2824, 7496);
  node(475, 2776, 7648);
  node(476, 2776, 7752);
  node(477, 2776, 7856);
  node(478, 3112, 7552);
  node(479, 3064, 7704);
  node(480, 3064, 7808);
  node(481, 3064, 7912);
  node(482, 3400, 7608);
  node(483, 3352, 7760);
  node(484, 3352, 7864);
  node(485, 3352, 7968);
  node(486, 3688, 7664);
  node(487, 3640, 7816);
  node(488, 3640, 7920);
  node(489, 3640, 8024);
  node(490, 3976, 7720);
  node(491, 3928, 7872);
  node(492, 3928, 7976);
  node(493, 3928, 8080);
  node(494, 4264, 7776);
  node(495, 4216, 7928);
  node(496, 4216, 8032);
  node(497, 4552, 7832);
  node(498, 4840, 7888);
  node(499, 5128, 7944);
  node(500, 5416, 8000);
  node(501, 5368, 8152);
  node(502, 5368, 8256);
  node(503, 5368, 8360);
  node(504, 5704, 8056);
  node(505, 5656, 8208);
  node(506, 5656, 8312);
  node(507, 5656, 8416);
  node(508, 5656, 8520);
  node(509, 5656, 8624);
  node(510, 5992, 8112);
  node(511, 6280, 8168);
  node(512, 6232, 8320);
  node(513, 6568, 8224);
  node(514, 6520, 8376);
  node(515, 6520, 8480);
  node(516, 6520, 8584);
  node(517, 6520, 8688);
  node(518, 6520, 8792);
  node(519, 6856, 8280);
  node(520, 6808, 8432);
  node(521, 6808, 8536);
  node(522, 6808, 8640);
  node(523, 6808, 8744);
  node(524, 6808, 8848);
  node(21, 3640, 7330);
  node(525, 3592, 7480);
  node(526, 3592, 7584);
  node(527, 3928, 7384);
  node(528, 3880, 7536);
  node(529, 3880, 7640);
  node(530, 4216, 7440);
  node(531, 4168, 7592);
  node(532, 4504, 7496);
  node(533, 4456, 7648);
  node(534, 4792, 7552);
  node(535, 4744, 7704);
  node(536, 5080, 7608);
  node(537, 5032, 7760);
  node(538, 5032, 7864);
  node(539, 5032, 7968);
  node(540, 5368, 7664);
  node(541, 5656, 7720);
  node(542, 5608, 7872);
  node(543, 5608, 7976);
  node(544, 5944, 7776);
  node(545, 5896, 7928);
  node(546, 5896, 8032);
  node(547, 6232, 7832);
  node(548, 6184, 7984);
  node(549, 6184, 8088);
  node(550, 6520, 7888);
  node(551, 6808, 7944);
  node(22, 7096, 8000);
  node(552, 7048, 8152);
  node(553, 7048, 8256);
  node(554, 7048, 8360);
  node(23, 7384, 8056);
  node(555, 7336, 8208);
  node(556, 7336, 8312);
  node(557, 7336, 8416);
  node(558, 7672, 8112);
  node(559, 7624, 8264);
  node(560, 7624, 8368);
  node(561, 7624, 8472);
  node(562, 7624, 8576);
  node(563, 7624, 8680);
  node(564, 7624, 8784);
  node(565, 7624, 8888);
  node(566, 7624, 8992);
  node(567, 7624, 9096);
  node(568, 7960, 8168);
  node(569, 7912, 8320);
  node(570, 8248, 8224);
  node(571, 8200, 8376);
  node(572, 8200, 8480);
  node(573, 8200, 8584);
  node(574, 8536, 8280);
  node(575, 8488, 8432);
  node(576, 8488, 8536);
  node(577, 8824, 8336);
  node(578, 9112, 8392);
  node(579, 9400, 8448);
  node(580, 9352, 8600);
  node(581, 9352, 8704);
  node(582, 9688, 8504);
  node(583, 9640, 8656);
  node(584, 9640, 8760);
  node(585, 9976, 8560);
  node(586, 10264, 8616);
  node(587, 10552, 8672);
  node(588, 10504, 8824);
  node(589, 10504, 8928);
  node(590, 10840, 8728);
  node(591, 11128, 8784);
  node(592, 11080, 8936);
  node(593, 11080, 9040);
  node(594, 11080, 9144);
  node(595, 11416, 8840);
  node(596, 11368, 8992);
  node(597, 11368, 9096);
  node(598, 11368, 9200);
  node(599, 11368, 9304);
  node(600, 11368, 9408);
  node(601, 11704, 8896);
  node(602, 11656, 9048);
  node(603, 11656, 9152);
  node(604, 11656, 9256);
  node(605, 11656, 9360);
  node(606, 11656, 9464);
  node(607, 11992, 8952);
  node(608, 12280, 9008);
  node(609, 12568, 9064);
  node(610, 12520, 9216);
  node(611, 12520, 9320);
  node(612, 12856, 9120);
  node(613, 12808, 9272);
  node(614, 12808, 9376);
  node(615, 13144, 9176);
  node(616, 13096, 9328);
  node(617, 13096, 9432);
  node(24, 13432, 9232);
  node(618, 13384, 9384);
  node(619, 13384, 9488);
  node(620, 13384, 9592);
  node(25, 13720, 9288);
  node(621, 13672, 9440);
  node(622, 13672, 9544);
  node(623, 13672, 9648);
  node(624, 14008, 9344);
  node(625, 14296, 9400);
  node(626, 14248, 9552);
  node(627, 14248, 9656);
  node(628, 14248, 9760);
  node(629, 14248, 9864);
  node(630, 14248, 9968);
  node(631, 14248, 10072);
  node(632, 14248, 10176);
  node(633, 14248, 10280);
  node(634, 14248, 10384);
  node(635, 14248, 10488);
  node(636, 14248, 10592);
  node(637, 14248, 10696);
  node(638, 14248, 10800);
  node(639, 14248, 10904);
  node(640, 14248, 11008);
  node(641, 14248, 11112);
  node(642, 14248, 11216);
  node(643, 14248, 11320);
  node(644, 14248, 11424);
  node(645, 14584, 9456);
  node(646, 14872, 9512);
  node(647, 14824, 9664);
  node(648, 14824, 9768);
  node(26, 15160, 9568);
  node(27, 15448, 9624);
  node(649, 15736, 9680);
  node(650, 15688, 9832);
  node(651, 15688, 9936);
  node(652, 15688, 10040);
  node(653, 15688, 10144);
  node(654, 15688, 10248);
  node(655, 15688, 10352);
  node(656, 15688, 10456);
  node(657, 16024, 9736);
  node(658, 16312, 9792);
  node(659, 16264, 9944);
  node(660, 16264, 10048);
  node(661, 16600, 9848);
  node(662, 16552, 10000);
  node(663, 16552, 10104);
  node(28, 16888, 9904);
  node(664, 17176, 9960);
  node(665, 17128, 10112);
  node(666, 17128, 10216);
  node(667, 17464, 10016);
  node(668, 17416, 10168);
  node(669, 17416, 10272);
  node(670, 17416, 10376);
  node(29, 17752, 10072);
  node(671, 18040, 10128);
  node(672, 17992, 10280);
  node(673, 17992, 10384);
  node(674, 17992, 10488);
  node(675, 17992, 10592);
  node(676, 18328, 10184);
  node(677, 18280, 10336);
  node(678, 18280, 10440);
  node(679, 18616, 10240);
  node(680, 18568, 10392);
  node(681, 18904, 10296);
  node(682, 18856, 10448);
  node(683, 18856, 10552);
  node(684, 18856, 10656);
  node(685, 19192, 10352);
  node(686, 19144, 10504);
  node(687, 19144, 10608);
  node(688, 19144, 10712);
  node(689, 19480, 10408);
  node(690, 19768, 10464);
  node(691, 19720, 10616);
  node(692, 19720, 10720);
  node(693, 20056, 10520);
  node(694, 20008, 10672);
  node(695, 20008, 10776);
  node(696, 20344, 10576);
  node(697, 20296, 10728);
  node(698, 20296, 10832);
  node(699, 20632, 10632);
  node(700, 20920, 10688);
  node(701, 21208, 10744);
  node(702, 21160, 10896);
  node(703, 21160, 11000);
  node(704, 21160, 11104);
  node(30, 21496, 10800);
  node(705, 21784, 10856);
  node(706, 22072, 10912);
  node(707, 22024, 11064);
  node(708, 22024, 11168);
  node(709, 22360, 10968);
  node(710, 22312, 11120);
  node(711, 22312, 11224);
  node(712, 22312, 11328);
  node(713, 22312, 11432);
  node(714, 22648, 11024);
  node(715, 22600, 11176);
  node(716, 22600, 11280);
  node(717, 22600, 11384);
  node(718, 22600, 11488);
  node(719, 22936, 11080);
  node(720, 22888, 11232);
  node(721, 22888, 11336);
  node(722, 22888, 11440);
  node(723, 22888, 11544);
  node(724, 23224, 11136);
  node(725, 23176, 11288);
  node(726, 23176, 11392);
  node(727, 23176, 11496);
  node(728, 23512, 11192);
  node(729, 23800, 11248);
  node(730, 23752, 11400);
  node(731, 23752, 11504);
  node(732, 24088, 11304);
  node(733, 24040, 11456);
  node(734, 24040, 11560);
  node(735, 24040, 11664);
  node(736, 24040, 11768);
  node(737, 24040, 11872);
  node(738, 24040, 11976);
  node(739, 24040, 12080);
  node(740, 24376, 11360);
  node(741, 24328, 11512);
  node(742, 24328, 11616);
  node(743, 24328, 11720);
  node(744, 24328, 11824);
  node(745, 24328, 11928);
  node(746, 24328, 12032);
  node(747, 24664, 11416);
  node(748, 24952, 11472);
  node(749, 24904, 11624);
  node(750, 24904, 11728);
  node(751, 24904, 11832);
  node(752, 24904, 11936);
  node(753, 24904, 12040);
  node(754, 25240, 11528);
  node(755, 25192, 11680);
  node(756, 25192, 11784);
  node(757, 25192, 11888);
  node(758, 25192, 11992);
  node(759, 25528, 11584);
  node(760, 25816, 11640);
  node(761, 25768, 11792);
  node(762, 25768, 11896);
  node(763, 26104, 11696);
  node(764, 26392, 11752);
  node(765, 26344, 11904);
  node(766, 26680, 11808);
  node(767, 26632, 11960);
  node(31, 26968, 11864);
  node(768, 27256, 11920);
  node(769, 27544, 11976);
  node(312, 0, 7330);
  node(313, 280, 7330);
  node(32, 568, 7384);
  node(314, 840, 7330);
  node(315, 720, 7480);
  node(316, 720, 7570);
  node(317, 720, 7660);
  node(318, 1120, 7330);
  node(319, 1000, 7480);
  node(320, 1000, 7570);
  node(321, 1000, 7660);
  node(322, 1400, 7330);
  node(323, 1280, 7480);
  node(324, 1280, 7570);
  node(325, 1280, 7660);
  node(326, 1680, 7330);
  node(327, 1560, 7480);
  node(328, 1560, 7570);
  node(329, 1560, 7660);
  node(33, 1968, 7384);
  node(330, 2240, 7330);
  node(331, 2120, 7480);
  node(332, 2120, 7570);
  node(333, 2120, 7660);
  node(334, 2120, 7750);
  node(335, 2520, 7330);
  node(336, 2400, 7480);
  node(337, 2400, 7570);
  node(338, 2800, 7330);
  node(339, 2680, 7480);
  node(340, 2680, 7570);
  node(341, 3080, 7330);
  node(342, 2960, 7480);
  node(343, 2960, 7570);
  node(344, 3360, 7330);
  node(345, 3240, 7480);
  node(346, 3240, 7570);
  node(34, 3648, 7384);
  node(347, 3920, 7330);
  node(348, 3800, 7480);
  node(349, 3800, 7570);
  node(350, 4200, 7330);
  node(351, 4080, 7480);
  node(352, 4080, 7570);
  node(353, 4080, 7660);
  node(354, 4080, 7750);
  node(355, 4080, 7840);
  node(356, 4080, 7930);
  node(357, 4080, 8020);
  next_id(770);
}


}  // namespace kern_blueprint_fe59a0c6af5496ca

namespace blueprints {
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;
using qr_code = ::kern_blueprint_f1248d4ad2e4cc95::qr_code;
using qr_code_cpp = ::kern_blueprint_fe59a0c6af5496ca::qr_code_cpp;
}  // namespace blueprints
