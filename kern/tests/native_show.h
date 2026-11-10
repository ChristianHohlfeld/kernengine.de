// How the native Blueprint test writes a value down: the interpreter's side (kern_tests
// --emit-native) and the compiled side (blueprint_native_test) use this one function, and the
// two texts must be the same, character by character.
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "kern/logic/logic.h"

inline std::string native_show(const kern::logic::Value& v) {
  using kern::logic::PinType;
  char b[160];
  auto number = [&](double x, int digits) -> std::string {
    if (std::isnan(x)) return "nan";  // (whatever its sign)
    std::snprintf(b, sizeof b, "%.*g", digits, x);
    return b;
  };
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer: std::snprintf(b, sizeof b, "%lld", static_cast<long long>(v.i)); return b;
    case PinType::number: return number(v.n, 17);
    case PinType::vector: return "(" + number(v.v.x, 9) + ", " + number(v.v.y, 9) + ", " + number(v.v.z, 9) + ")";
    case PinType::text: return "\"" + v.s + "\"";
    case PinType::actor: std::snprintf(b, sizeof b, "#%llu", static_cast<unsigned long long>(v.a)); return b;
    default: return "?";
  }
}

/// The host of the native Blueprint test: time stands still, nothing is random, errors are kept.
struct NativeTestHost : kern::logic::Host {
  std::vector<std::string> errors;
  kern::f64 time() const override { return 0; }
  void print(const std::string&, bool) override {}
  void error(const std::string& text) override { errors.push_back(text); }
  kern::u32 random() override { return 0; }
};
