// Kern Logic: formulas (the Formula node). A recursive-descent parser writes a small stack
// program; evaluating it is a loop over that program on f64.
#include <cmath>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include "kern/core/dmath.h"
#include "kern/logic/logic.h"

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
    {"deg", kDeg, 1},     {"lerp", kLerp, 3},
};
constexpr f64 kPi = 3.14159265358979323846;
constexpr int kMaxStack = 48;

bool letter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool digit(char c) { return c >= '0' && c <= '9'; }

}  // namespace

struct FormulaParser {
  Formula& f;
  std::string_view text;
  std::size_t at = 0;
  int depth = 0, deepest = 0, nesting = 0;

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
  /// A one-character operator that is not the start of a longer one ("<" but not "<=").
  bool take_single(char c, std::string_view not_followed_by) {
    space();
    if (at >= text.size() || text[at] != c) return false;
    if (at + 1 < text.size() && not_followed_by.find(text[at + 1]) != std::string_view::npos) return false;
    ++at;
    return true;
  }
  void emit(Code code, int pops, f64 number = 0, u16 index = 0) {
    Formula::Op op;
    op.code = code;
    op.args = static_cast<u8>(pops);
    op.number = number;
    op.index = index;
    f.code_.push_back(op);
    depth += 1 - pops;
    if (depth > deepest) deepest = depth;
    if (depth > kMaxStack) fail("the formula is too deep");
  }

  void ternary() {
    if (++nesting > 64) { fail("too many nested brackets"); return; }
    logical_or();
    if (!failed() && take("?")) {
      ternary();
      if (!take(":")) { fail("\":\" expected after \"?\""); --nesting; return; }
      ternary();
      emit(kSelect, 3);
    }
    --nesting;
  }
  void logical_or() {
    logical_and();
    while (!failed() && take("||")) { logical_and(); emit(kOr, 2); }
  }
  void logical_and() {
    compare();
    while (!failed() && take("&&")) { compare(); emit(kAnd, 2); }
  }
  void compare() {
    sum();
    for (;;) {
      if (failed()) return;
      Code c;
      if (take("<=")) c = kLe;
      else if (take(">=")) c = kGe;
      else if (take("==")) c = kEq;
      else if (take("!=")) c = kNe;
      else if (take_single('<', "")) c = kLt;
      else if (take_single('>', "")) c = kGt;
      else return;
      sum();
      emit(c, 2);
    }
  }
  void sum() {
    term();
    for (;;) {
      if (failed()) return;
      if (take("+")) { term(); emit(kAdd, 2); }
      else if (take("-")) { term(); emit(kSub, 2); }
      else return;
    }
  }
  void term() {
    unary();
    for (;;) {
      if (failed()) return;
      if (take("*")) { unary(); emit(kMul, 2); }
      else if (take("/")) { unary(); emit(kDiv, 2); }
      else if (take("%")) { unary(); emit(kMod, 2); }
      else return;
    }
  }
  void unary() {
    if (failed()) return;
    if (take("-")) { unary(); emit(kNeg, 1); return; }
    if (take_single('!', "=")) { unary(); emit(kNot, 1); return; }
    if (take("+")) { unary(); return; }
    power();
  }
  void power() {
    primary();
    if (!failed() && take("^")) { unary(); emit(kPow, 2); }  // right-associative: 2 ^ 3 ^ 2 = 2 ^ 9
  }
  void primary() {
    space();
    if (failed()) return;
    if (at >= text.size()) { fail("the formula ends too early"); return; }
    const char c = text[at];
    if (c == '(') {
      ++at;
      ternary();
      if (!failed() && !take(")")) fail("\")\" expected");
      return;
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
      emit(kConst, 0, std::strtod(number.c_str(), nullptr));
      return;
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
        if (!fn) { at = start; fail("no function \"" + name + "\""); return; }
        int count = 0;
        if (!take(")")) {
          for (;;) {
            ternary();
            ++count;
            if (failed()) return;
            if (take(",")) continue;
            if (take(")")) break;
            fail("\",\" or \")\" expected");
            return;
          }
        }
        if (count != fn->args) { fail(name + " takes " + std::to_string(fn->args) + (fn->args == 1 ? " value" : " values")); return; }
        emit(fn->code, fn->args);
        return;
      }
      if (name == "pi") { emit(kConst, 0, kPi); return; }
      for (const Fn& x : kFunctions) if (name == x.name) { fail(name + " is a function: " + name + "(...)"); return; }
      std::size_t index = 0;
      while (index < f.inputs_.size() && f.inputs_[index] != name) ++index;
      if (index == f.inputs_.size()) {
        if (name == "exec" || name == "then" || name == "result") { at = start; fail("\"" + name + "\" cannot be an input's name"); return; }
        if (f.inputs_.size() >= Formula::kMaxInputs) { fail("a formula has at most " + std::to_string(Formula::kMaxInputs) + " inputs"); return; }
        f.inputs_.push_back(name);
      }
      emit(kInput, 0, 0, static_cast<u16>(index));
      return;
    }
    fail(std::string("unexpected \"") + c + "\"");
  }
};

Formula::Formula(std::string_view text) {
  FormulaParser p{*this, text};
  p.space();
  if (p.at >= text.size()) { error_ = "the formula is empty"; return; }
  p.ternary();
  p.space();
  if (error_.empty() && p.at < text.size()) p.fail(std::string("unexpected \"") + text[p.at] + "\"");
  if (!error_.empty()) code_.clear();
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
      case kDiv: r = b != 0 ? a / b : 0; break;
      case kMod: r = b != 0 ? std::fmod(a, b) : 0; break;
      case kPow: r = dm::pow(a, b); break;
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
      case kMin: r = min_of(a, b); break;
      case kMax: r = max_of(a, b); break;
      case kClamp: r = clamp(a, min_of(b, c), max_of(b, c)); break;
      case kAbs: r = std::fabs(a); break;
      case kSign: r = a > 0 ? 1 : a < 0 ? -1 : 0; break;
      case kFloor: r = std::floor(a); break;
      case kCeil: r = std::ceil(a); break;
      case kRound: r = std::round(a); break;
      case kTrunc: r = std::trunc(a); break;
      case kSqrt: r = std::sqrt(max_of(0.0, a)); break;
      case kExp: r = dm::exp(a); break;
      case kLog: r = a > 0 ? dm::log(a) : 0; break;
      case kSin: r = dm::sin(a); break;
      case kCos: r = dm::cos(a); break;
      case kTan: r = dm::tan(a); break;
      case kAsin: r = dm::asin(clamp(a, -1.0, 1.0)); break;
      case kAcos: r = dm::acos(clamp(a, -1.0, 1.0)); break;
      case kAtan: r = dm::atan(a); break;
      case kAtan2: r = dm::atan2(a, b); break;
      case kRad: r = a * (kPi / 180); break;
      case kDeg: r = a * (180 / kPi); break;
      case kLerp: r = a + (b - a) * c; break;
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
