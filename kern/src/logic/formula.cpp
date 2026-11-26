// Kern Logic: formulas (the Formula node). A recursive-descent parser builds the tree of the formula as C++ binds it,
// a second pass gives every value its type (the types of the inputs, of the numbers written out, of each operation),
// and the tree becomes a small stack program: each step one function of numbers.h on the bits of its type.
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <unordered_map>

#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

namespace {

using num::Kind;

enum Code : u8 {
  kConst, kInput, kAdd, kSub, kMul, kDiv, kRem, kNeg, kBitNot, kNot, kBitAnd, kBitOr, kBitXor, kShl, kShr,
  kLt, kLe, kGt, kGe, kEq, kNe, kAnd, kOr, kSelect, kCast, kBitCast,
  kMin, kMax, kAbs, kClamp, kLerp,
  kSqrt, kFloor, kCeil, kTrunc, kRound, kSin, kCos, kTan, kAsin, kAcos, kAtan, kExp, kExpm1, kLog, kLog10, kSinh, kCosh, kTanh, kAsinh, kAcosh, kAtanh, kLog2, kLog1p, kExp2, kCbrt, kRemainder,
  kAtan2, kPow, kHypot, kCopysign, kFmin, kFmax, kFmod,
  kIsnan, kIsinf, kIsfinite, kSignbit,
};

// What a function takes: numbers of one type that are whole or floating (arithmetic), floating ones only, or floating
// ones that it tells something of (a bool).
enum class Takes : u8 { arithmetic, floating, floating_is };
struct FormulaFunction { const char* name; Code code; u8 args; Takes takes; };
constexpr FormulaFunction kFunctions[] = {
    {"min", kMin, 2, Takes::arithmetic},     {"max", kMax, 2, Takes::arithmetic},     {"abs", kAbs, 1, Takes::arithmetic},
    {"clamp", kClamp, 3, Takes::arithmetic}, {"lerp", kLerp, 3, Takes::floating},     {"sqrt", kSqrt, 1, Takes::floating},
    {"floor", kFloor, 1, Takes::floating},   {"ceil", kCeil, 1, Takes::floating},     {"trunc", kTrunc, 1, Takes::floating},
    {"round", kRound, 1, Takes::floating},   {"sin", kSin, 1, Takes::floating},       {"cos", kCos, 1, Takes::floating},
    {"tan", kTan, 1, Takes::floating},       {"asin", kAsin, 1, Takes::floating},     {"acos", kAcos, 1, Takes::floating},
    {"atan", kAtan, 1, Takes::floating},     {"exp", kExp, 1, Takes::floating},       {"expm1", kExpm1, 1, Takes::floating},
    {"sinh", kSinh, 1, Takes::floating},     {"cosh", kCosh, 1, Takes::floating},     {"tanh", kTanh, 1, Takes::floating},
    {"asinh", kAsinh, 1, Takes::floating},     {"acosh", kAcosh, 1, Takes::floating},   {"atanh", kAtanh, 1, Takes::floating},
    {"log2", kLog2, 1, Takes::floating},       {"log1p", kLog1p, 1, Takes::floating},   {"exp2", kExp2, 1, Takes::floating},
    {"cbrt", kCbrt, 1, Takes::floating},       {"remainder", kRemainder, 2, Takes::floating},
    {"log", kLog, 1, Takes::floating},       {"log10", kLog10, 1, Takes::floating},   {"atan2", kAtan2, 2, Takes::floating},
    {"pow", kPow, 2, Takes::floating},       {"hypot", kHypot, 2, Takes::floating},   {"copysign", kCopysign, 2, Takes::floating},
    {"fmin", kFmin, 2, Takes::floating},     {"fmax", kFmax, 2, Takes::floating},     {"fmod", kFmod, 2, Takes::floating},
    {"isnan", kIsnan, 1, Takes::floating_is}, {"isinf", kIsinf, 1, Takes::floating_is}, {"isfinite", kIsfinite, 1, Takes::floating_is},
    {"signbit", kSignbit, 1, Takes::floating_is},
};
const FormulaFunction* function_of(u8 code) {
  for (const FormulaFunction& f : kFunctions) if (f.code == code) return &f;
  return nullptr;
}
constexpr int kMaxStack = 48;

bool letter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool digit(char c) { return c >= '0' && c <= '9'; }
std::string kind_text(Kind k) { return std::string(num::kind_name(k)); }
std::string a_kind(Kind k) { return (k == Kind::i8 || k == Kind::i16 || k == Kind::i32 || k == Kind::i64 || k == Kind::f32 || k == Kind::f64 ? "an " : "a ") + kind_text(k); }

// ---- each step of the program: one function of numbers.h on the bits of its type ----
template <class T> T in(u64 b) { return num::of_bits<T>(b); }
template <class T> u64 out(T x) { return num::bits_of(x); }
using Step = u64 (*)(u64, u64, u64);

/// The step of an operation `code` on values of the type `k` (a conversion: from `k` to `to`); nullptr where the type
/// has no such operation (the second pass has refused that already).
Step step_of(u8 code, Kind k, Kind to) {
  if (code == kCast || code == kBitCast)
    return num::visit(to, [&](auto t) -> Step {
      using To = typename decltype(t)::type;
      return num::visit(k, [&](auto f) -> Step {
        using From = typename decltype(f)::type;
        if (code == kCast) return [](u64 a, u64, u64) { return out(num::cast<To>(in<From>(a))); };
        if constexpr (sizeof(To) == sizeof(From) && !std::same_as<To, bool> && !std::same_as<From, bool>)
          return [](u64 a, u64, u64) { return out(num::reinterpret<To>(in<From>(a))); };
        return nullptr;
      });
    });
  return num::visit(k, [&](auto t) -> Step {
    using T = typename decltype(t)::type;
    switch (static_cast<Code>(code)) {
      case kEq: return [](u64 a, u64 b, u64) { return out(num::eq(in<T>(a), in<T>(b))); };
      case kNe: return [](u64 a, u64 b, u64) { return out(num::ne(in<T>(a), in<T>(b))); };
      case kSelect: return [](u64 a, u64 b, u64 c) { return a != 0 ? b : c; };
      default: break;
    }
    if constexpr (std::same_as<T, bool>) {
      switch (static_cast<Code>(code)) {
        case kNot: return [](u64 a, u64, u64) { return out(!in<bool>(a)); };
        case kAnd: return [](u64 a, u64 b, u64) { return out(in<bool>(a) && in<bool>(b)); };
        case kOr: return [](u64 a, u64 b, u64) { return out(in<bool>(a) || in<bool>(b)); };
        default: return nullptr;
      }
    } else {
      switch (static_cast<Code>(code)) {
        case kAdd: return [](u64 a, u64 b, u64) { return out(num::add(in<T>(a), in<T>(b))); };
        case kSub: return [](u64 a, u64 b, u64) { return out(num::sub(in<T>(a), in<T>(b))); };
        case kMul: return [](u64 a, u64 b, u64) { return out(num::mul(in<T>(a), in<T>(b))); };
        case kDiv: return [](u64 a, u64 b, u64) { return out(num::div(in<T>(a), in<T>(b))); };
        case kNeg: return [](u64 a, u64, u64) { return out(num::neg(in<T>(a))); };
        case kLt: return [](u64 a, u64 b, u64) { return out(num::lt(in<T>(a), in<T>(b))); };
        case kLe: return [](u64 a, u64 b, u64) { return out(num::le(in<T>(a), in<T>(b))); };
        case kGt: return [](u64 a, u64 b, u64) { return out(num::gt(in<T>(a), in<T>(b))); };
        case kGe: return [](u64 a, u64 b, u64) { return out(num::ge(in<T>(a), in<T>(b))); };
        case kMin: return [](u64 a, u64 b, u64) { return out(num::min(in<T>(a), in<T>(b))); };
        case kMax: return [](u64 a, u64 b, u64) { return out(num::max(in<T>(a), in<T>(b))); };
        case kAbs: return [](u64 a, u64, u64) { return out(num::abs(in<T>(a))); };
        case kClamp: return [](u64 a, u64 b, u64 c) { return out(num::clamp(in<T>(a), in<T>(b), in<T>(c))); };
        default: break;
      }
      if constexpr (num::Integer<T>) {
        switch (static_cast<Code>(code)) {
          case kRem: return [](u64 a, u64 b, u64) { return out(num::rem(in<T>(a), in<T>(b))); };
          case kBitNot: return [](u64 a, u64, u64) { return out(num::bit_not(in<T>(a))); };
          case kBitAnd: return [](u64 a, u64 b, u64) { return out(num::bit_and(in<T>(a), in<T>(b))); };
          case kBitOr: return [](u64 a, u64 b, u64) { return out(num::bit_or(in<T>(a), in<T>(b))); };
          case kBitXor: return [](u64 a, u64 b, u64) { return out(num::bit_xor(in<T>(a), in<T>(b))); };
          // (the amount: a whole number of any type, its bits as bits_of holds them - its low bits are its own)
          case kShl: return [](u64 a, u64 b, u64) { return out(num::shl(in<T>(a), static_cast<i64>(b))); };
          case kShr: return [](u64 a, u64 b, u64) { return out(num::shr(in<T>(a), static_cast<i64>(b))); };
          default: return nullptr;
        }
      } else {
        switch (static_cast<Code>(code)) {
          case kLerp: return [](u64 a, u64 b, u64 c) { return out(num::lerp(in<T>(a), in<T>(b), in<T>(c))); };
          case kSqrt: return [](u64 a, u64, u64) { return out(num::sqrt(in<T>(a))); };
          case kFloor: return [](u64 a, u64, u64) { return out(num::floor(in<T>(a))); };
          case kCeil: return [](u64 a, u64, u64) { return out(num::ceil(in<T>(a))); };
          case kTrunc: return [](u64 a, u64, u64) { return out(num::trunc(in<T>(a))); };
          case kRound: return [](u64 a, u64, u64) { return out(num::round(in<T>(a))); };
          case kSin: return [](u64 a, u64, u64) { return out(num::sin(in<T>(a))); };
          case kCos: return [](u64 a, u64, u64) { return out(num::cos(in<T>(a))); };
          case kTan: return [](u64 a, u64, u64) { return out(num::tan(in<T>(a))); };
          case kAsin: return [](u64 a, u64, u64) { return out(num::asin(in<T>(a))); };
          case kAcos: return [](u64 a, u64, u64) { return out(num::acos(in<T>(a))); };
          case kAtan: return [](u64 a, u64, u64) { return out(num::atan(in<T>(a))); };
          case kExp: return [](u64 a, u64, u64) { return out(num::exp(in<T>(a))); };
          case kExpm1: return [](u64 a, u64, u64) { return out(num::expm1(in<T>(a))); };
          case kSinh: return [](u64 a, u64, u64) { return out(num::sinh(in<T>(a))); };
          case kCosh: return [](u64 a, u64, u64) { return out(num::cosh(in<T>(a))); };
          case kTanh: return [](u64 a, u64, u64) { return out(num::tanh(in<T>(a))); };
          case kAsinh: return [](u64 a, u64, u64) { return out(num::asinh(in<T>(a))); };
          case kLog: return [](u64 a, u64, u64) { return out(num::log(in<T>(a))); };
          case kLog10: return [](u64 a, u64, u64) { return out(num::log10(in<T>(a))); };
          case kAtan2: return [](u64 a, u64 b, u64) { return out(num::atan2(in<T>(a), in<T>(b))); };
          case kPow: return [](u64 a, u64 b, u64) { return out(num::pow(in<T>(a), in<T>(b))); };
          case kHypot: return [](u64 a, u64 b, u64) { return out(num::hypot(in<T>(a), in<T>(b))); };
          case kRemainder: return [](u64 a, u64 b, u64) { return out(num::remainder(in<T>(a), in<T>(b))); };
          case kAcosh: return [](u64 a, u64, u64) { return out(num::acosh(in<T>(a))); };
          case kAtanh: return [](u64 a, u64, u64) { return out(num::atanh(in<T>(a))); };
          case kLog2: return [](u64 a, u64, u64) { return out(num::log2(in<T>(a))); };
          case kLog1p: return [](u64 a, u64, u64) { return out(num::log1p(in<T>(a))); };
          case kExp2: return [](u64 a, u64, u64) { return out(num::exp2(in<T>(a))); };
          case kCbrt: return [](u64 a, u64, u64) { return out(num::cbrt(in<T>(a))); };
          case kCopysign: return [](u64 a, u64 b, u64) { return out(num::copysign(in<T>(a), in<T>(b))); };
          case kFmin: return [](u64 a, u64 b, u64) { return out(num::fmin(in<T>(a), in<T>(b))); };
          case kFmax: return [](u64 a, u64 b, u64) { return out(num::fmax(in<T>(a), in<T>(b))); };
          case kFmod: return [](u64 a, u64 b, u64) { return out(num::fmod(in<T>(a), in<T>(b))); };
          case kIsnan: return [](u64 a, u64, u64) { return out(num::isnan(in<T>(a))); };
          case kIsinf: return [](u64 a, u64, u64) { return out(num::isinf(in<T>(a))); };
          case kIsfinite: return [](u64 a, u64, u64) { return out(num::isfinite(in<T>(a))); };
          case kSignbit: return [](u64 a, u64, u64) { return out(num::signbit(in<T>(a))); };
          default: return nullptr;
        }
      }
    }
  });
}

}  // namespace

struct FormulaParser {
  Formula& f;
  std::string_view text;
  std::size_t at = 0;
  int nesting = 0;

  void fail_at(std::size_t where, const std::string& why) {
    if (f.error_.empty()) f.error_ = why + " (at " + std::to_string(where + 1) + ")";
  }
  void fail(const std::string& why) { fail_at(at, why); }
  bool failed() const { return !f.error_.empty(); }
  void space() { while (at < text.size() && (text[at] == ' ' || text[at] == '\t' || text[at] == '\n' || text[at] == '\r')) ++at; }
  bool take(std::string_view token) {
    space();
    if (text.substr(at, token.size()) != token) return false;
    at += token.size();
    return true;
  }
  /// A one-character operator that is not the start of a longer one ("<" but not "<=" or "<<").
  bool take_single(char c, std::string_view not_followed_by) {
    space();
    if (at >= text.size() || text[at] != c) return false;
    if (at + 1 < text.size() && not_followed_by.find(text[at + 1]) != std::string_view::npos) return false;
    ++at;
    return true;
  }
  int node(Code code, std::size_t where, int a = -1, int b = -1, int c = -1) {
    Formula::Term t;
    t.code = code;
    t.a = a;
    t.b = b;
    t.c = c;
    t.at = static_cast<u32>(where);
    f.tree_.push_back(t);
    return static_cast<int>(f.tree_.size()) - 1;
  }
  Formula::Term& term(int t) { return f.tree_[static_cast<std::size_t>(t)]; }

  // ---- reading: the levels of C++, loosest first ----
  int ternary() {
    if (++nesting > 64) { fail("too many nested brackets"); return -1; }
    const std::size_t where = (space(), at);
    int r = logical_or();
    if (!failed() && take("?")) {
      const int yes = ternary();
      if (!failed() && !take(":")) { fail("\":\" expected after \"?\""); --nesting; return -1; }
      const int no = ternary();
      r = node(kSelect, where, r, yes, no);
    }
    --nesting;
    return r;
  }
  template <class Next> int left(Next next, std::initializer_list<std::pair<const char*, Code>> ops) {
    int r = next();
    for (;;) {
      if (failed()) return r;
      space();
      const std::size_t where = at;
      bool found = false;
      for (const auto& [token, code] : ops) {
        const std::string_view t = token;
        bool ok = false;
        // (a one-character operator that begins a longer one is that one: & of &&, < of <= and <<)
        if (t == "&") ok = take_single('&', "&=");
        else if (t == "|") ok = take_single('|', "|=");
        else if (t == "<") ok = take_single('<', "<=");
        else if (t == ">") ok = take_single('>', ">=");
        else if (t == "<<" || t == ">>") ok = text.substr(at, 2) == t && !(at + 2 < text.size() && text[at + 2] == '=') && take(t);
        else ok = take(t);
        if (!ok) continue;
        r = node(code, where, r, next());
        found = true;
        break;
      }
      if (!found) return r;
    }
  }
  int logical_or() { return left([&] { return logical_and(); }, {{"||", kOr}}); }
  int logical_and() { return left([&] { return bit_or(); }, {{"&&", kAnd}}); }
  int bit_or() { return left([&] { return bit_xor(); }, {{"|", kBitOr}}); }
  int bit_xor() { return left([&] { return bit_and(); }, {{"^", kBitXor}}); }
  int bit_and() { return left([&] { return equality(); }, {{"&", kBitAnd}}); }
  int equality() { return left([&] { return relation(); }, {{"==", kEq}, {"!=", kNe}}); }
  int relation() { return left([&] { return shift(); }, {{"<=", kLe}, {">=", kGe}, {"<", kLt}, {">", kGt}}); }
  int shift() { return left([&] { return sum(); }, {{"<<", kShl}, {">>", kShr}}); }
  int sum() { return left([&] { return product(); }, {{"+", kAdd}, {"-", kSub}}); }
  int product() { return left([&] { return unary(); }, {{"*", kMul}, {"/", kDiv}, {"%", kRem}}); }
  int unary() {
    if (failed()) return -1;
    space();
    const std::size_t where = at;
    if (take("-")) {
      space();
      // (a sign in front of a number is the number's: -1, -0.5f)
      if (at < text.size() && (digit(text[at]) || (text[at] == '.' && at + 1 < text.size() && digit(text[at + 1])))) return number(where, true);
      return node(kNeg, where, unary());
    }
    if (take_single('!', "=")) return node(kNot, where, unary());
    if (take("~")) return node(kBitNot, where, unary());
    if (take("+")) { fail_at(where, "no + in front of a value (C++ would promote it)"); return -1; }
    return primary();
  }
  int number(std::size_t where, bool minus) {
    space();
    const std::size_t start = at;
    bool real = false;
    while (at < text.size() && digit(text[at])) ++at;
    if (at < text.size() && text[at] == '.') {
      real = true;
      ++at;
      while (at < text.size() && digit(text[at])) ++at;
    }
    if (at < text.size() && (text[at] == 'e' || text[at] == 'E')) {
      std::size_t k = at + 1;
      if (k < text.size() && (text[k] == '+' || text[k] == '-')) ++k;
      if (k < text.size() && digit(text[k])) {
        while (k < text.size() && digit(text[k])) ++k;
        at = k;
        real = true;
      }
    }
    const std::string digits(text.substr(start, at - start));
    const int t = node(kConst, where);
    Formula::Term& n = term(t);
    if (real) {
      const bool single = at < text.size() && (text[at] == 'f' || text[at] == 'F');
      if (single) ++at;
      // (read as the compiler reads it: an f32 rounded once from the decimal, an f64 the nearest)
      const f64 v = scan::real(digits, single).value;
      if (!std::isfinite(v)) { fail_at(start, "the number is too large for " + std::string(single ? "an f32" : "an f64")); return -1; }
      if (single) n.kind = Kind::f32, n.bits = num::bits_of(minus ? -static_cast<f32>(v) : static_cast<f32>(v));
      else n.kind = Kind::f64, n.bits = num::bits_of(minus ? -v : v);
    } else {
      if (digits.size() > 1 && digits[0] == '0') { fail_at(start, "a whole number has no 0 in front (C++ reads 017 as octal)"); return -1; }
      n.loose = true;
      n.minus = minus;
      n.unsigned_ = at < text.size() && (text[at] == 'u' || text[at] == 'U');
      if (n.unsigned_) ++at;
      const scan::Whole w = scan::whole(digits, 10, false);
      if (w.end != digits.size() || digits.size() > 20 || (digits.size() == 20 && digits > "18446744073709551615")) { fail_at(start, "the number is too large for a whole number"); return -1; }
      n.bits = w.bits;
      if (minus && n.unsigned_) { fail_at(where, "-" + digits + "u: an unsigned number is not below 0"); return -1; }
      if (!n.unsigned_ && n.bits > static_cast<u64>(std::numeric_limits<i64>::max())) {
        fail_at(start, digits + " is beyond i64: as an unsigned number it is " + digits + "u");
        return -1;
      }
    }
    if (at < text.size() && (letter(text[at]) || digit(text[at]) || text[at] == '.')) { fail_at(start, "a number is digits, a point, an exponent and f or u after it"); return -1; }
    return t;
  }
  bool kind_here(Kind& k) {
    space();
    const std::size_t start = at;
    while (at < text.size() && (letter(text[at]) || digit(text[at]))) ++at;
    if (!num::kind_from_name(text.substr(start, at - start), k)) {
      fail_at(start, "a number type expected (bool, i8 ... i64, u8 ... u64, f32, f64)");
      return false;
    }
    return true;
  }
  int primary() {
    space();
    if (failed()) return -1;
    if (at >= text.size()) { fail("the formula ends too early"); return -1; }
    const std::size_t where = at;
    const char c = text[at];
    if (c == '(') {
      ++at;
      const int r = ternary();
      if (!failed() && !take(")")) fail("\")\" expected");
      return r;
    }
    if (digit(c) || (c == '.' && at + 1 < text.size() && digit(text[at + 1]))) return number(where, false);
    if (!letter(c)) {
      fail(std::string("unexpected \"") + c + "\"");
      return -1;
    }
    const std::size_t start = at;
    while (at < text.size() && (letter(text[at]) || digit(text[at]))) ++at;
    const std::string name(text.substr(start, at - start));
    if (name == "true" || name == "false") {
      const int t = node(kConst, where);
      term(t).kind = Kind::boolean;
      term(t).bits = name == "true";
      return t;
    }
    if (name == "num_cast" || name == "num_bit_cast") {
      // num_cast<To, From>(x): the conversion node of C++'s static_cast, From the type x has (num_bit_cast: std::bit_cast)
      Kind to{}, from{};
      if (!take("<")) { fail(name + "<To, From>(x)"); return -1; }
      if (!kind_here(to)) return -1;
      if (!take(",")) { fail(name + "<To, From>(x): \",\" expected"); return -1; }
      if (!kind_here(from)) return -1;
      if (!take(">") || !take("(")) { fail(name + "<To, From>(x)"); return -1; }
      const int x = ternary();
      if (failed()) return -1;
      if (!take(")")) { fail("\")\" expected"); return -1; }
      const int t = node(name == "num_cast" ? kCast : kBitCast, where, x);
      term(t).kind = to;
      term(t).from = from;
      return t;
    }
    space();
    if (at < text.size() && text[at] == '(') {
      ++at;
      const FormulaFunction* fn = nullptr;
      for (const FormulaFunction& x : kFunctions) if (name == x.name) fn = &x;
      if (!fn) { fail_at(start, "no function \"" + name + "\""); return -1; }
      int args[3] = {-1, -1, -1}, count = 0;
      if (!take(")")) {
        for (;;) {
          const int a = ternary();
          if (count < 3) args[count] = a;
          ++count;
          if (failed()) return -1;
          if (take(",")) continue;
          if (take(")")) break;
          fail("\",\" or \")\" expected");
          return -1;
        }
      }
      if (count != fn->args) { fail_at(start, name + " takes " + std::to_string(fn->args) + (fn->args == 1 ? " value" : " values")); return -1; }
      return node(fn->code, where, args[0], args[1], args[2]);
    }
    for (const FormulaFunction& x : kFunctions) if (name == x.name) { fail_at(start, name + " is a function: " + name + "(...)"); return -1; }
    std::size_t index = 0;
    while (index < f.inputs_.size() && f.inputs_[index] != name) ++index;
    if (index == f.inputs_.size()) {
      if (name == "exec" || name == "then" || name == "result" || name == "types" || name == "formula") { fail_at(start, "\"" + name + "\" cannot be an input's name"); return -1; }
      if (f.inputs_.size() >= Formula::kMaxInputs) { fail_at(start, "a formula has at most " + std::to_string(Formula::kMaxInputs) + " inputs"); return -1; }
      f.inputs_.push_back(name);
    }
    const int t = node(kInput, where);
    term(t).index = static_cast<u16>(index);
    return t;
  }

  // ---- the types: every value has its own, nothing converts but num_cast ----
  /// The whole number written out at t as the type k (it is one of it, or the formula is refused).
  bool settle(int t, Kind k) {
    Formula::Term& n = term(t);
    const std::string written = (n.minus ? "-" : "") + std::to_string(n.bits) + (n.unsigned_ ? "u" : "");
    if (!num::is_integer(k)) {
      fail_at(n.at, written + " is a whole number, and it meets " + a_kind(k) + (num::is_floating(k) ? ": " + written + (k == Kind::f32 ? ".0f" : ".0") : std::string(": num_cast")));
      return false;
    }
    if (n.unsigned_ && num::is_signed(k)) { fail_at(n.at, written + " is unsigned, and it meets " + a_kind(k) + ": " + written.substr(0, written.size() - 1)); return false; }
    const bool fits = num::visit(k, [&](auto x) {
      using T = typename decltype(x)::type;
      if (n.minus) return std::is_signed_v<T> && n.bits <= static_cast<u64>(std::numeric_limits<T>::max()) + 1;
      return n.bits <= static_cast<u64>(std::numeric_limits<T>::max());
    });
    if (!fits) { fail_at(n.at, written + " is not " + a_kind(k) + " (it meets one)"); return false; }
    n.bits = num::held_bits(k, n.minus ? 0 - n.bits : n.bits);
    n.kind = k;
    n.loose = false;
    return true;
  }
  /// A whole number written out that meets nothing it could take a type from: C++'s - an i32, or an i64 beyond it
  /// (7u a u32, or a u64).
  bool settle_alone(int t) {
    const Formula::Term& n = term(t);
    const u64 i32_end = n.minus ? u64{1} << 31 : (u64{1} << 31) - 1, u32_end = 0xffffffffu;
    if (n.unsigned_) return settle(t, n.bits <= u32_end ? Kind::u32 : Kind::u64);
    return settle(t, n.bits <= i32_end ? Kind::i32 : Kind::i64);
  }
  /// The operands of `op` are of one type: the type of those that have one, the numbers written out of it.
  bool same(int op, std::initializer_list<int> operands, Kind& k) {
    bool typed = false;
    for (int x : operands) {
      if (term(x).loose) continue;
      if (!typed) {
        k = term(x).kind;
        typed = true;
      } else if (term(x).kind != k) {
        for (int y : operands)
          if (term(y).code == kConst && num::is_floating(term(y).kind) && num::is_floating(term(y).kind == k ? term(x).kind : k)) {
            // (a floating number written out is of its own type: 0.5 an f64, 0.5f an f32)
            const Kind other = term(y).kind == k ? term(x).kind : k;
            fail_at(term(y).at, "this number is " + a_kind(term(y).kind) + ", and it meets " + a_kind(other) + (other == Kind::f32 ? ": with f after it" : ": without f after it"));
            return false;
          }
        fail_at(term(op).at, "the values meeting here are " + a_kind(k) + " and " + a_kind(term(x).kind) + ": no value converts by itself - num_cast<" + kind_text(k) + ", " + kind_text(term(x).kind) + ">(...)");
        return false;
      }
    }
    if (!typed) {
      if (term(op).code == kSelect || f.constant_) {
        for (int x : operands) if (!settle_alone(x)) return false;
        k = term(*operands.begin()).kind;
        for (int x : operands) if (term(x).kind != k) return same(op, operands, k);
        return true;
      }
      fail_at(term(op).at, "an operation of numbers written out only: write the number it gives");
      return false;
    }
    for (int x : operands)
      if (term(x).loose && !settle(x, k)) return false;
    return true;
  }
  bool arithmetic(int op, Kind k) {
    if (k == Kind::boolean) { fail_at(term(op).at, "a bool has no arithmetic: && || ! and ? : are its operations"); return false; }
    return true;
  }
  bool whole(int op, Kind k, const char* what) {
    if (!num::is_integer(k)) { fail_at(term(op).at, std::string(what) + " is an operation of whole numbers, this is " + a_kind(k) + (num::is_floating(k) && term(op).code == kRem ? ": fmod(a, b)" : "")); return false; }
    return true;
  }
  bool floating(int op, Kind k, const char* name) {
    if (!num::is_floating(k)) { fail_at(term(op).at, std::string(name) + " is a function of floating numbers (f32, f64), this is " + a_kind(k)); return false; }
    return true;
  }
  /// The type of the value at t (and of all under it); false when the formula does not suit its types.
  bool type(int t) {
    if (t < 0 || failed()) return false;
    Formula::Term& n = term(t);
    for (int child : {n.a, n.b, n.c})
      if (child >= 0 && !type(child)) return false;
    const int a = n.a, b = n.b, c = n.c;
    if (n.code != kConst && n.code != kInput && n.code != kSelect && !f.constant_) {
      // (what an operation gives of numbers written out only is a number written out itself)
      bool all = true;
      for (int child : {a, b, c}) all = all && (child < 0 || term(child).code == kConst);
      if (all) { fail_at(n.at, "an operation of numbers written out only: write the number it gives"); return false; }
    }
    Kind k = Kind::f64;
    switch (static_cast<Code>(n.code)) {
      case kConst: return true;
      case kInput:
        if (n.index >= f.types_.size()) return false;
        n.kind = f.types_[n.index];
        return true;
      case kAdd: case kSub: case kMul: case kDiv:
        if (!same(t, {a, b}, k) || !arithmetic(t, k)) return false;
        break;
      case kRem: case kBitAnd: case kBitOr: case kBitXor:
        if (!same(t, {a, b}, k) || !whole(t, k, n.code == kRem ? "%" : n.code == kBitAnd ? "&" : n.code == kBitOr ? "|" : "^")) return false;
        break;
      case kNeg: case kBitNot: case kNot:
        if (f.constant_ && term(a).loose && !settle_alone(a)) return false;
        if (!f.constant_ && (term(a).loose || term(a).code == kConst)) { fail_at(n.at, "an operation of a number written out: write the number it gives"); return false; }
        k = term(a).kind;
        if (n.code == kNeg && !arithmetic(t, k)) return false;
        if (n.code == kBitNot && !whole(t, k, "~")) return false;
        if (n.code == kNot && k != Kind::boolean) { fail_at(n.at, "! is the not of a bool, this is " + a_kind(k) + (num::is_integer(k) ? ": ~ of its bits, or == 0" : ": == 0")); return false; }
        break;
      case kShl: case kShr: {
        // (the value's type is the type of what it gives; the amount is a whole number of any type)
        if (term(a).loose && term(b).loose) { fail_at(n.at, "an operation of numbers written out only: write the number it gives"); return false; }
        if (term(a).loose && !settle_alone(a)) return false;
        if (term(b).loose && !settle_alone(b)) return false;
        k = term(a).kind;
        if (!whole(t, k, n.code == kShl ? "<<" : ">>")) return false;
        if (!num::is_integer(term(b).kind)) { fail_at(term(b).at, "the amount of a shift is a whole number, this is " + a_kind(term(b).kind)); return false; }
        break;
      }
      case kLt: case kLe: case kGt: case kGe: case kEq: case kNe:
        if (!same(t, {a, b}, k)) return false;
        if (n.code != kEq && n.code != kNe && !arithmetic(t, k)) return false;
        n.from = k;
        k = Kind::boolean;
        break;
      case kAnd: case kOr:
        if (!same(t, {a, b}, k)) return false;
        if (k != Kind::boolean) { fail_at(n.at, std::string(n.code == kAnd ? "&&" : "||") + " is an operation of bools, this is " + a_kind(k) + (num::is_integer(k) ? (n.code == kAnd ? ": & of its bits" : ": | of its bits") : "")); return false; }
        break;
      case kSelect:
        if (term(a).loose || term(a).kind != Kind::boolean) { fail_at(term(a).at, "what ? asks is a bool"); return false; }
        if (!same(t, {b, c}, k)) return false;
        break;
      case kCast: case kBitCast: {
        if (term(a).loose && !settle(a, n.from)) return false;
        if (term(a).kind != n.from) { fail_at(term(a).at, "this is " + a_kind(term(a).kind) + ", and " + (n.code == kCast ? "num_cast<" : "num_bit_cast<") + kind_text(n.kind) + ", " + kind_text(n.from) + "> converts " + a_kind(n.from)); return false; }
        if (n.code == kBitCast && (num::width(n.kind) != num::width(n.from) || n.kind == Kind::boolean || n.from == Kind::boolean)) {
          fail_at(n.at, "num_bit_cast takes the bits of a number as a type of the same size (f32 - u32, i32; f64 - u64, i64)");
          return false;
        }
        if (n.code == kCast && n.kind == n.from) { fail_at(n.at, "num_cast<" + kind_text(n.kind) + ", " + kind_text(n.from) + "> converts nothing"); return false; }
        return true;
      }
      default: {
        const FormulaFunction* fn = function_of(n.code);
        if (!fn) return false;
        if (fn->args == 1) {
          if (f.constant_ && term(a).loose && !settle_alone(a)) return false;
          if (!f.constant_ && (term(a).loose || term(a).code == kConst)) { fail_at(n.at, "a function of numbers written out only: write the number it gives"); return false; }
          k = term(a).kind;
        } else if (!(fn->args == 2 ? same(t, {a, b}, k) : same(t, {a, b, c}, k))) {
          return false;
        }
        if (fn->takes == Takes::arithmetic && !arithmetic(t, k)) return false;
        if (fn->takes != Takes::arithmetic && !floating(t, k, fn->name)) return false;
        if (fn->takes == Takes::floating_is) n.from = k, k = Kind::boolean;
        break;
      }
    }
    n.kind = k;
    return true;
  }

  /// The stack program of the tree under `t`; false when it needs a deeper stack than there is.
  bool program(int t, int& depth) {
    if (t < 0) return true;
    const Formula::Term& n = f.tree_[static_cast<std::size_t>(t)];
    int args = 0;
    for (int child : {n.a, n.b, n.c}) {
      if (child < 0) continue;
      if (!program(child, depth)) return false;
      ++args;
    }
    Formula::Op op;
    op.code = n.code;
    op.args = static_cast<u8>(args);
    op.bits = n.bits;
    op.index = n.code == kInput ? n.index : 0;
    if (n.code != kConst && n.code != kInput) {
      // (a comparison and isnan ... are of the type of their operands; the rest of their own)
      const bool of_operands = n.code == kLt || n.code == kLe || n.code == kGt || n.code == kGe || n.code == kEq || n.code == kNe ||
                               n.code == kIsnan || n.code == kIsinf || n.code == kIsfinite || n.code == kSignbit;
      const Kind on = n.code == kCast || n.code == kBitCast ? n.from : of_operands ? n.from : n.kind;
      op.fn = step_of(n.code, on, n.kind);
      if (!op.fn) return false;
    }
    f.code_.push_back(op);
    depth += 1 - args;
    return depth <= kMaxStack;
  }
};

// ---- the formula as text: the one way it is written, C++ ----
namespace {

/// How tightly an operation binds, as in C++ (the ternary loosest); kBound: a call, a number, a name.
constexpr int kBound = 12;
int binding(u8 code) {
  switch (static_cast<Code>(code)) {
    case kSelect: return 0;
    case kOr: return 1;
    case kAnd: return 2;
    case kBitOr: return 3;
    case kBitXor: return 4;
    case kBitAnd: return 5;
    case kEq: case kNe: return 6;
    case kLt: case kLe: case kGt: case kGe: return 7;
    case kShl: case kShr: return 8;
    case kAdd: case kSub: return 9;
    case kMul: case kDiv: case kRem: return 10;
    case kNeg: case kNot: case kBitNot: return 11;
    default: return kBound;
  }
}
const char* op_text(u8 code) {
  switch (static_cast<Code>(code)) {
    case kOr: return "||"; case kAnd: return "&&"; case kBitOr: return "|"; case kBitXor: return "^"; case kBitAnd: return "&";
    case kEq: return "=="; case kNe: return "!="; case kLt: return "<"; case kLe: return "<="; case kGt: return ">"; case kGe: return ">=";
    case kShl: return "<<"; case kShr: return ">>"; case kAdd: return "+"; case kSub: return "-"; case kMul: return "*"; case kDiv: return "/";
    case kRem: return "%"; case kNeg: return "-"; case kNot: return "!"; case kBitNot: return "~";
    default: return "";
  }
}
/// A floating number as C++ writes it: the shortest decimal that reads back as it (an f32's with f after it).
std::string real_text(u64 bits, bool single) {
  char b[48];
  const f64 v = single ? static_cast<f64>(num::of_bits<f32>(bits)) : num::of_bits<f64>(bits);
  for (int digits = 1; digits <= 17; ++digits) {
    std::snprintf(b, sizeof b, "%.*g", digits, v);
    if (single ? static_cast<f32>(scan::real(b, true).value) == static_cast<f32>(v) : scan::real(b, false).value == v) break;
  }
  std::string r = b;
  if (r.find_first_of(".e") == std::string::npos) r += ".0";
  if (std::signbit(v) && r[0] != '-') r = "-" + r;  // (-0.0)
  return single ? r + "f" : r;
}

}  // namespace

std::string Formula::print(int t, int outer) const {
  if (t < 0 || static_cast<std::size_t>(t) >= tree_.size()) return "0";
  const Term& n = tree_[static_cast<std::size_t>(t)];
  const int mine = binding(n.code);
  auto child_binding = [&](int x) { return x >= 0 ? binding(tree_[static_cast<std::size_t>(x)].code) : kBound; };
  auto negative = [&](int x) {
    if (x < 0) return false;
    const Term& c = tree_[static_cast<std::size_t>(x)];
    if (c.code != kConst) return false;
    if (num::is_floating(c.kind)) return std::signbit(c.kind == Kind::f32 ? num::of_bits<f32>(c.bits) : num::of_bits<f64>(c.bits));
    return num::is_signed(c.kind) && static_cast<i64>(c.bits) < 0;
  };
  std::string s;
  switch (static_cast<Code>(n.code)) {
    case kConst:
      if (n.kind == Kind::boolean) s = n.bits ? "true" : "false";
      else if (num::is_floating(n.kind)) s = real_text(n.bits, n.kind == Kind::f32);
      else {
        const bool minus = num::is_signed(n.kind) && static_cast<i64>(n.bits) < 0;
        s = (minus ? "-" : "") + std::to_string(minus ? 0 - n.bits : n.bits) + (n.unsigned_ ? "u" : "");
      }
      break;
    case kInput: s = n.index < inputs_.size() ? inputs_[n.index] : std::string("0"); break;
    case kSelect: s = print(n.a, 1) + " ? " + print(n.b, 1) + " : " + print(n.c, 0); break;
    case kNeg: case kNot: case kBitNot: {
      // (a sign in front of a sign or of a number below 0 is bracketed: "--x" is another operator in C++)
      const bool again = (n.a >= 0 && binding(tree_[static_cast<std::size_t>(n.a)].code) == mine) || negative(n.a);
      s = std::string(op_text(n.code)) + (again ? "(" + print(n.a, 0) + ")" : print(n.a, mine));
      break;
    }
    case kCast: case kBitCast:
      s = std::string(n.code == kCast ? "num_cast<" : "num_bit_cast<") + kind_text(n.kind) + ", " + kind_text(n.from) + ">(" + print(n.a, 0) + ")";
      break;
    default:
      if (const char* o = op_text(n.code); *o) {
        // left-associative: the right operand is bracketed at the same binding. Brackets also where C++ compilers ask
        // for them: another operation under & | ^ << >> or && in ||, a comparison under a comparison, ! before one.
        auto operand = [&](int x, bool right) {
          const int cb = child_binding(x);
          const bool asked = cb >= 1 && cb <= 10 && cb != mine &&
                             ((mine >= 3 && mine <= 5) || mine == 8 || (mine == 1 && cb == 2) || ((mine == 6 || mine == 7) && (cb == 6 || cb == 7)));
          const bool same_comparison = (mine == 6 || mine == 7) && cb == mine;
          const bool not_before = (mine == 6 || mine == 7) && !right && x >= 0 && tree_[static_cast<std::size_t>(x)].code == kNot;
          if (asked || same_comparison || not_before || cb < mine || (right && cb == mine)) return "(" + print(x, 0) + ")";
          return print(x, mine);
        };
        s = operand(n.a, false) + " " + o + " " + operand(n.b, true);
        break;
      }
      {
        const FormulaFunction* fn = function_of(n.code);
        s = std::string(fn ? fn->name : "?") + "(";
        bool first = true;
        for (int child : {n.a, n.b, n.c}) {
          if (child < 0) continue;
          s += (first ? "" : ", ") + print(child, 0);
          first = false;
        }
        s += ")";
      }
      break;
  }
  return mine < outer ? "(" + s + ")" : s;
}

std::string Formula::text() const { return ok() ? print(root_, 0) : std::string(); }
std::string Formula::types_text() const {
  std::string r;
  for (Kind k : types_) r += (r.empty() ? "" : " ") + kind_text(k);
  return r;
}

Formula::Formula(std::string_view text, std::string_view types) : Formula(text, types, false) {}
Formula Formula::constant(std::string_view text) { return Formula(text, "", true); }

Formula::Formula(std::string_view text, std::string_view types, bool constant) : constant_(constant) {
  // the types of the inputs: one number type each, apart by spaces
  for (std::size_t at = 0; at < types.size();) {
    while (at < types.size() && (types[at] == ' ' || types[at] == ',')) ++at;
    const std::size_t start = at;
    while (at < types.size() && types[at] != ' ' && types[at] != ',') ++at;
    if (start == at) break;
    Kind k{};
    if (!num::kind_from_name(types.substr(start, at - start), k)) {
      error_ = "\"" + std::string(types.substr(start, at - start)) + "\" is no number type (bool, i8 ... i64, u8 ... u64, f32, f64): the types of the formula's inputs";
      return;
    }
    types_.push_back(k);
  }
  FormulaParser p{*this, text};
  p.space();
  if (p.at >= text.size()) { error_ = "the formula is empty"; return; }
  root_ = p.ternary();
  p.space();
  if (error_.empty() && p.at < text.size()) p.fail(std::string("unexpected \"") + text[p.at] + "\"");
  if (error_.empty() && constant_ && !inputs_.empty()) error_ = "a constant formula reads no input";
  if (error_.empty() && inputs_.empty() && !constant_) error_ = "a formula reads at least one input (a number alone is the node Number)";
  if (error_.empty() && types_.size() != inputs_.size()) {
    std::string names;
    for (const std::string& in : inputs_) names += (names.empty() ? "" : ", ") + in;
    error_ = "the formula reads " + std::to_string(inputs_.size()) + " input(s) (" + names + "), and its types name " + std::to_string(types_.size()) + ": one type for each, in this order";
  }
  if (error_.empty()) {
    if (p.type(root_)) {
      if (tree_[static_cast<std::size_t>(root_)].loose) p.settle_alone(root_);
      result_ = tree_[static_cast<std::size_t>(root_)].kind;
    } else if (error_.empty()) {
      error_ = "the formula does not suit its types";
    }
  }
  int depth = 0;
  if (error_.empty() && !p.program(root_, depth)) error_ = "the formula is too deep";
  if (!error_.empty()) { code_.clear(); tree_.clear(); root_ = -1; result_ = Kind::f64; }
}

u64 Formula::evaluate(const u64* values, std::size_t count) const {
  if (code_.empty()) return 0;
  u64 st[kMaxStack + 4];
  int n = 0;
  for (const Op& op : code_) {
    // the operands are the top `args` values: a = the first, b = the second, c = the third
    const u64 a = op.args >= 1 ? st[n - op.args] : 0, b = op.args >= 2 ? st[n - op.args + 1] : 0, c = op.args >= 3 ? st[n - 1] : 0;
    n -= op.args;
    u64 r = 0;
    if (op.code == kConst) r = op.bits;
    else if (op.code == kInput) r = op.index < count ? values[op.index] : 0;
    else r = op.fn(a, b, c);
    st[n++] = r;
  }
  return n > 0 ? st[n - 1] : 0;
}

const Formula& Formula::cached(const std::string& text, const std::string& types) {
  static std::unordered_map<std::string, std::unique_ptr<Formula>> cache;
  const std::string key = types + "\n" + text;
  auto it = cache.find(key);
  if (it != cache.end()) return *it->second;
  return *(cache[key] = std::make_unique<Formula>(text, types));  // (kept: callers hold the reference while they evaluate)
}

}  // namespace kern::logic
