// Kern Logic: formulas (the Formula node). A recursive-descent parser writes a small stack
// program; evaluating it is a loop over that program on f64.
#include <cmath>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include "kern/core/dmath.h"
#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

namespace {

enum Code : u8 {
  kConst, kInput, kAdd, kSub, kMul, kDiv, kMod, kPow, kNeg, kNot, kLt, kLe, kGt, kGe, kEq, kNe, kAnd, kOr, kSelect,
  kMin, kMax, kClamp, kAbs, kSign, kFloor, kCeil, kRound, kTrunc, kSqrt, kExp, kLog,
  kSin, kCos, kTan, kAsin, kAcos, kAtan, kAtan2, kRad, kDeg, kLerp,
};

struct Fn { const char* name; Code code; u8 args; };
constexpr Fn kFunctions[] = {
    {"min", kMin, 2},     {"max", kMax, 2},     {"clamp", kClamp, 3}, {"abs", kAbs, 1},     {"sign", kSign, 1},   {"floor", kFloor, 1}, {"ceil", kCeil, 1},
    {"round", kRound, 1}, {"trunc", kTrunc, 1}, {"sqrt", kSqrt, 1},   {"pow", kPow, 2},     {"exp", kExp, 1},     {"log", kLog, 1},     {"sin", kSin, 1},
    {"cos", kCos, 1},     {"tan", kTan, 1},     {"asin", kAsin, 1},   {"acos", kAcos, 1},   {"atan", kAtan, 1},   {"atan2", kAtan2, 2}, {"rad", kRad, 1},
    {"deg", kDeg, 1},     {"lerp", kLerp, 3},   {"div", kDiv, 2},     {"mod", kMod, 2},
};
constexpr f64 kPi = math::pi;
constexpr int kMaxStack = 48;

bool letter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool digit(char c) { return c >= '0' && c <= '9'; }

}  // namespace

struct FormulaParser {
  Formula& f;
  std::string_view text;
  std::size_t at = 0;
  int nesting = 0;

  void fail(const std::string& why) {
    if (f.error_.empty()) f.error_ = why + " (at " + std::to_string(at + 1) + ")";
  }
  bool failed() const { return !f.error_.empty(); }
  void space() { while (at < text.size() && (text[at] == ' ' || text[at] == '\t' || text[at] == '\n' || text[at] == '\r')) ++at; }
  bool take(std::string_view token) {
    space();
    if (text.substr(at, token.size()) != token) return false;
    at += token.size();
    return true;
  }
  /// A one-character operator that is not the start of a longer one ("!" but not "!=").
  bool take_single(char c, std::string_view not_followed_by) {
    space();
    if (at >= text.size() || text[at] != c) return false;
    if (at + 1 < text.size() && not_followed_by.find(text[at + 1]) != std::string_view::npos) return false;
    ++at;
    return true;
  }
  /// A node of the tree: an operation on up to three earlier nodes (or a number / an input).
  int node(Code code, int a = -1, int b = -1, int c = -1, f64 number = 0, u16 index = 0) {
    Formula::Term t;
    t.code = code;
    t.a = a;
    t.b = b;
    t.c = c;
    t.number = number;
    t.index = index;
    f.tree_.push_back(t);
    return static_cast<int>(f.tree_.size()) - 1;
  }

  int ternary() {
    if (++nesting > 64) { fail("too many nested brackets"); return -1; }
    int r = logical_or();
    if (!failed() && take("?")) {
      const int yes = ternary();
      if (!take(":")) { fail("\":\" expected after \"?\""); --nesting; return -1; }
      const int no = ternary();
      r = node(kSelect, r, yes, no);
    }
    --nesting;
    return r;
  }
  int logical_or() {
    int r = logical_and();
    while (!failed() && take("||")) r = node(kOr, r, logical_and());
    return r;
  }
  int logical_and() {
    int r = compare();
    while (!failed() && take("&&")) r = node(kAnd, r, compare());
    return r;
  }
  int compare() {
    int r = sum();
    for (;;) {
      if (failed()) return r;
      Code c;
      if (take("<=")) c = kLe;
      else if (take(">=")) c = kGe;
      else if (take("==")) c = kEq;
      else if (take("!=")) c = kNe;
      else if (take_single('<', "")) c = kLt;
      else if (take_single('>', "")) c = kGt;
      else return r;
      r = node(c, r, sum());
    }
  }
  int sum() {
    int r = term();
    for (;;) {
      if (failed()) return r;
      if (take("+")) r = node(kAdd, r, term());
      else if (take("-")) r = node(kSub, r, term());
      else return r;
    }
  }
  int term() {
    int r = unary();
    for (;;) {
      if (failed()) return r;
      if (take("*")) r = node(kMul, r, unary());
      else if (take("/")) r = node(kDiv, r, unary());
      else if (take("%")) r = node(kMod, r, unary());
      else return r;
    }
  }
  int unary() {
    if (failed()) return -1;
    if (take("-")) return node(kNeg, unary());
    if (take_single('!', "=")) return node(kNot, unary());
    if (take("+")) return unary();
    return power();
  }
  int power() {
    const int r = primary();
    if (!failed() && take("^")) return node(kPow, r, unary());  // right-associative: 2 ^ 3 ^ 2 = 2 ^ 9
    return r;
  }
  int primary() {
    space();
    if (failed()) return -1;
    if (at >= text.size()) { fail("the formula ends too early"); return -1; }
    const char c = text[at];
    if (c == '(') {
      ++at;
      const int r = ternary();
      if (!failed() && !take(")")) fail("\")\" expected");
      return r;
    }
    if (digit(c) || (c == '.' && at + 1 < text.size() && digit(text[at + 1]))) {
      // digits [. digits] [e [+-] digits], read as the compiler reads a literal (correctly rounded)
      const std::size_t start = at;
      while (at < text.size() && digit(text[at])) ++at;
      if (at < text.size() && text[at] == '.') {
        ++at;
        while (at < text.size() && digit(text[at])) ++at;
      }
      if (at < text.size() && (text[at] == 'e' || text[at] == 'E')) {
        std::size_t k = at + 1;
        if (k < text.size() && (text[k] == '+' || text[k] == '-')) ++k;
        if (k < text.size() && digit(text[k])) {
          while (k < text.size() && digit(text[k])) ++k;
          at = k;
        }
      }
      const std::string number(text.substr(start, at - start));
      const f64 value = std::strtod(number.c_str(), nullptr);
      if (!std::isfinite(value)) { at = start; fail("the number is too large"); return -1; }
      return node(kConst, -1, -1, -1, value);
    }
    if (letter(c)) {
      const std::size_t start = at;
      while (at < text.size() && (letter(text[at]) || digit(text[at]))) ++at;
      const std::string name(text.substr(start, at - start));
      space();
      if (at < text.size() && text[at] == '(') {
        ++at;
        const Fn* fn = nullptr;
        for (const Fn& x : kFunctions) if (name == x.name) fn = &x;
        if (!fn) { at = start; fail("no function \"" + name + "\""); return -1; }
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
        if (count != fn->args) { fail(name + " takes " + std::to_string(fn->args) + (fn->args == 1 ? " value" : " values")); return -1; }
        return node(fn->code, args[0], args[1], args[2]);
      }
      if (name == "pi") return node(kConst, -1, -1, -1, kPi, 1);  // (index 1: written as pi)
      for (const Fn& x : kFunctions) if (name == x.name) { fail(name + " is a function: " + name + "(...)"); return -1; }
      std::size_t index = 0;
      while (index < f.inputs_.size() && f.inputs_[index] != name) ++index;
      if (index == f.inputs_.size()) {
        if (name == "exec" || name == "then" || name == "result") { at = start; fail("\"" + name + "\" cannot be an input's name"); return -1; }
        if (f.inputs_.size() >= Formula::kMaxInputs) { fail("a formula has at most " + std::to_string(Formula::kMaxInputs) + " inputs"); return -1; }
        f.inputs_.push_back(name);
      }
      return node(kInput, -1, -1, -1, 0, static_cast<u16>(index));
    }
    fail(std::string("unexpected \"") + c + "\"");
    return -1;
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
    op.number = n.number;
    op.index = n.code == kInput ? n.index : 0;
    f.code_.push_back(op);
    depth += 1 - args;
    return depth <= kMaxStack;
  }
};

// ---- the formula as text: Kern's own notation (the canonical one) and C++ --------------------
namespace {

/// How tightly an operation binds (the ternary loosest); kBound: a call, a number, a name.
constexpr int kBound = 9;
int binding(u8 code) {
  switch (static_cast<Code>(code)) {
    case kSelect: return 0;
    case kOr: return 1;
    case kAnd: return 2;
    case kLt: case kLe: case kGt: case kGe: case kEq: case kNe: return 3;
    case kAdd: case kSub: return 4;
    case kMul: case kDiv: case kMod: return 5;
    case kNeg: case kNot: return 6;
    case kPow: return 7;
    default: return kBound;
  }
}
const char* op_text(u8 code) {
  switch (static_cast<Code>(code)) {
    case kOr: return "||"; case kAnd: return "&&"; case kLt: return "<"; case kLe: return "<="; case kGt: return ">"; case kGe: return ">=";
    case kEq: return "=="; case kNe: return "!="; case kAdd: return "+"; case kSub: return "-"; case kMul: return "*"; case kDiv: return "/";
    case kMod: return "%"; case kPow: return "^";
    default: return "";
  }
}
/// The shortest text that reads back as the same number (17 significant digits always do).
std::string number_text(f64 v, bool as_float) {
  char b[40];
  if (v == std::floor(v) && std::fabs(v) < 1e15) {
    std::snprintf(b, sizeof b, "%.0f", v);  // (a whole number as it is said)
  } else {
    for (int digits = 1; digits <= 17; ++digits) {
      std::snprintf(b, sizeof b, "%.*g", digits, v);
      if (std::strtod(b, nullptr) == v) break;
    }
  }
  std::string r = b;
  if (as_float && r.find_first_of(".eEni") == std::string::npos) r += ".0";  // (a C++ double literal)
  return r;
}

}  // namespace

std::string Formula::print(int t, int outer, bool cpp) const {
  if (t < 0 || static_cast<std::size_t>(t) >= tree_.size()) return "0";
  const Term& n = tree_[static_cast<std::size_t>(t)];
  const int mine = binding(n.code);
  std::string s;
  switch (static_cast<Code>(n.code)) {
    case kConst: s = n.index == 1 ? std::string(cpp ? "kern::math::pi" : "pi") : number_text(n.number, cpp); break;
    case kInput: s = n.index < inputs_.size() ? inputs_[n.index] : std::string("0"); break;
    case kSelect: s = print(n.a, 1, cpp) + " ? " + print(n.b, 1, cpp) + " : " + print(n.c, 0, cpp); break;
    case kNeg: case kNot: {
      // (a sign in front of a sign is bracketed: "--x" would be another operator in C++)
      const bool sign_again = n.a >= 0 && tree_[static_cast<std::size_t>(n.a)].code == n.code;
      s = std::string(n.code == kNeg ? "-" : "!") + (sign_again ? "(" + print(n.a, 0, cpp) + ")" : print(n.a, mine, cpp));
      break;
    }
    case kPow:
      // C++ has no power operator (and its / and % are not the formula's): calls there
      if (cpp) s = "kern::math::pow(" + print(n.a, 0, true) + ", " + print(n.b, 0, true) + ")";
      else s = print(n.a, mine + 1, false) + " ^ " + print(n.b, 6, false);
      break;
    case kDiv: case kMod:
      if (cpp) { s = std::string(n.code == kDiv ? "kern::math::div(" : "kern::math::mod(") + print(n.a, 0, true) + ", " + print(n.b, 0, true) + ")"; break; }
      [[fallthrough]];
    case kOr: case kAnd: case kLt: case kLe: case kGt: case kGe: case kEq: case kNe: case kAdd: case kSub: case kMul:
      // left-associative: the right operand needs brackets at the same binding. A comparison of a
      // comparison is bracketed on both sides (C++ binds == looser than <; here they are equal).
      // In C++ an && inside an || is bracketed as well (compilers ask for it).
      {
        auto operand = [&](int child, int outer_binding) {
          const bool and_in_or = cpp && n.code == kOr && child >= 0 && tree_[static_cast<std::size_t>(child)].code == kAnd;
          return and_in_or ? "(" + print(child, 0, true) + ")" : print(child, outer_binding, cpp);
        };
        s = operand(n.a, mine == 3 ? mine + 1 : mine) + " " + op_text(n.code) + " " + operand(n.b, mine + 1);
      }
      break;
    default: {
      const char* name = "?";
      for (const Fn& x : kFunctions) if (x.code == n.code) name = x.name;
      s = std::string(cpp ? "kern::math::" : "") + name + "(";
      bool first = true;
      for (int child : {n.a, n.b, n.c}) {
        if (child < 0) continue;
        s += (first ? "" : ", ") + print(child, 0, cpp);
        first = false;
      }
      s += ")";
      break;
    }
  }
  const bool call_form = cpp && (n.code == kPow || n.code == kDiv || n.code == kMod);
  return mine < outer && !call_form ? "(" + s + ")" : s;
}

std::string Formula::text() const { return ok() ? print(root_, 0, false) : std::string(); }
std::string Formula::cpp() const { return ok() ? print(root_, 0, true) : std::string("0.0"); }

Formula::Formula(std::string_view text) {
  FormulaParser p{*this, text};
  p.space();
  if (p.at >= text.size()) { error_ = "the formula is empty"; return; }
  root_ = p.ternary();
  p.space();
  if (error_.empty() && p.at < text.size()) p.fail(std::string("unexpected \"") + text[p.at] + "\"");
  int depth = 0;
  if (error_.empty() && !p.program(root_, depth)) error_ = "the formula is too deep";
  if (!error_.empty()) { code_.clear(); tree_.clear(); root_ = -1; }
}

f64 Formula::evaluate(const f64* values, std::size_t count) const {
  if (code_.empty()) return 0;
  f64 st[kMaxStack + 4];
  int n = 0;
  for (const Op& op : code_) {
    // the operands are the top `args` values: a = the first, b = the second, c = the third
    const f64 a = op.args >= 1 ? st[n - op.args] : 0, b = op.args >= 2 ? st[n - op.args + 1] : 0, c = op.args >= 3 ? st[n - 1] : 0;
    n -= op.args;
    f64 r = 0;
    switch (static_cast<Code>(op.code)) {
      case kConst: r = op.number; break;
      case kInput: r = op.index < count ? values[op.index] : 0; break;
      case kAdd: r = a + b; break;
      case kSub: r = a - b; break;
      case kMul: r = a * b; break;
      case kDiv: r = math::div(a, b); break;
      case kMod: r = math::mod(a, b); break;
      case kPow: r = math::pow(a, b); break;
      case kNeg: r = -a; break;
      case kNot: r = a == 0 ? 1 : 0; break;
      case kLt: r = a < b ? 1 : 0; break;
      case kLe: r = a <= b ? 1 : 0; break;
      case kGt: r = a > b ? 1 : 0; break;
      case kGe: r = a >= b ? 1 : 0; break;
      case kEq: r = a == b ? 1 : 0; break;
      case kNe: r = a != b ? 1 : 0; break;
      case kAnd: r = a != 0 && b != 0 ? 1 : 0; break;
      case kOr: r = a != 0 || b != 0 ? 1 : 0; break;
      case kSelect: r = a != 0 ? b : c; break;
      // the functions: the ones a formula's C++ calls (kern::math, nodes.h)
      case kMin: r = math::min(a, b); break;
      case kMax: r = math::max(a, b); break;
      case kClamp: r = math::clamp(a, b, c); break;
      case kAbs: r = math::abs(a); break;
      case kSign: r = math::sign(a); break;
      case kFloor: r = math::floor(a); break;
      case kCeil: r = math::ceil(a); break;
      case kRound: r = math::round(a); break;
      case kTrunc: r = math::trunc(a); break;
      case kSqrt: r = math::sqrt(a); break;
      case kExp: r = math::exp(a); break;
      case kLog: r = math::log(a); break;
      case kSin: r = math::sin(a); break;
      case kCos: r = math::cos(a); break;
      case kTan: r = math::tan(a); break;
      case kAsin: r = math::asin(a); break;
      case kAcos: r = math::acos(a); break;
      case kAtan: r = math::atan(a); break;
      case kAtan2: r = math::atan2(a, b); break;
      case kRad: r = math::rad(a); break;
      case kDeg: r = math::deg(a); break;
      case kLerp: r = math::lerp(a, b, c); break;
    }
    st[n++] = r;
  }
  return n > 0 ? st[n - 1] : 0;
}

const Formula& Formula::cached(const std::string& text) {
  static std::unordered_map<std::string, std::unique_ptr<Formula>> cache;
  auto it = cache.find(text);
  if (it != cache.end()) return *it->second;
  return *(cache[text] = std::make_unique<Formula>(text));  // (kept: callers hold the reference while they evaluate)
}

}  // namespace kern::logic
