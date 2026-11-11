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
    case PinType::enumeration: std::snprintf(b, sizeof b, "~%lld", static_cast<long long>(v.i)); return b;  // (its place among its enum's values)
    case PinType::structure: {  // its fields, each as above
      std::string r = "{";
      for (std::size_t k = 0, n = v.fields(); k < n; ++k) r += (k ? ", " : "") + native_show(*v.field(k));
      return r + "}";
    }
    default: break;
  }
  if (!kern::logic::is_list(v.type)) return "?";
  std::string r = "[";  // a list: its items, each as above
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + native_show(v.item(k));
  return r + "]";
}

/// The host of the native Blueprint test: time stands still, errors are kept; its random numbers
/// are one fixed sequence (a Blueprint draws its seed from it).
struct NativeTestHost : kern::logic::Host {
  std::vector<std::string> errors;
  kern::f64 time() const override { return 0; }
  void print(const std::string&, bool) override {}
  void error(const std::string& text) override { errors.push_back(text); }
  kern::u32 state = 2463534242u;
  kern::u32 random() override {
    state = state * 1664525u + 1013904223u;
    return state;
  }
};
