// Kern Logic: the engine as nodes - the nodes of the table, and the stand-in of the tests
// (kern/logic/engine_api.h).
#include "kern/logic/engine_api.h"

#include <cstdio>
#include <cstring>
#include <unordered_map>

namespace kern::logic {

const EngineFunction* engine_function(std::string_view type) {
  static const std::unordered_map<std::string_view, const EngineFunction*> by_type = [] {
    std::unordered_map<std::string_view, const EngineFunction*> m;
    for (const EngineFunction& f : engine_functions()) m.emplace(f.type, &f);
    return m;
  }();
  const auto it = by_type.find(type);
  return it == by_type.end() ? nullptr : it->second;
}

void add_engine_nodes(Library& lib) {
  for (const EngineFunction& f : engine_functions()) {
    NodeDef d;
    d.type = f.type;
    d.label = f.label;
    d.category = f.category;
    d.help = f.help;
    d.keywords = f.cpp;
    d.kind = f.value ? NodeKind::pure : NodeKind::action;
    d.native = true;
    if (!f.value) {
      d.inputs.push_back({"exec", "", PinType::exec, {}});
      d.outputs.push_back({"then", "", PinType::exec, {}});
    }
    for (const EnginePin& p : f.inputs) d.inputs.push_back({p.id, p.id, p.type, {}, false, p.of});
    for (const EnginePin& p : f.outputs) d.outputs.push_back({p.id, p.id, p.type, {}, false, p.of});
    d.run = [&f](Context& c) {
      const std::size_t first = f.value ? 0 : 1;
      std::vector<Value> in, out;
      for (std::size_t k = 0; k < f.inputs.size(); ++k) in.push_back(c.in(first + k));
      for (const EnginePin& p : f.outputs) out.push_back(Value::zero(p.type));
      c.host().engine(f.type, in, out);
      for (std::size_t k = 0; k < f.outputs.size(); ++k) c.out(first + k, std::move(out[k]));
      return f.value ? kStop : 0;
    };
    lib.add(std::move(d));
  }
}

bool Host::engine(std::string_view, const std::vector<Value>&, std::vector<Value>&) { return false; }

std::string engine_shown(const Value& v) {
  char b[128];
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer:
    case PinType::enumeration: std::snprintf(b, sizeof b, "%lld", static_cast<long long>(v.i)); return b;
    case PinType::number: std::snprintf(b, sizeof b, "%.17g", v.n); return b;
    case PinType::vector: std::snprintf(b, sizeof b, "(%.9g, %.9g, %.9g)", static_cast<f64>(v.v.x), static_cast<f64>(v.v.y), static_cast<f64>(v.v.z)); return b;
    case PinType::text: return "\"" + v.s + "\"";
    case PinType::actor: std::snprintf(b, sizeof b, "actor %u", v.a); return b;
    case PinType::int64: std::snprintf(b, sizeof b, "0x%llx", static_cast<unsigned long long>(static_cast<u64>(v.i))); return b;  // (its 64 bits)
    case PinType::map:
    case PinType::set: {  // (its entries in their order: {key: value, ...}, {key, ...} - a key's bits as the map holds them)
      std::string s = "{";
      const MapEntries& e = v.keyed();
      for (std::size_t k = 0; k < e.size(); ++k) {
        s += (k ? ", " : "") + (e.key_at(k).s.empty() ? std::to_string(e.key_at(k).i) : "\"" + e.key_at(k).s + "\"");
        if (v.type == PinType::map) s += ": " + engine_shown(e.value_at(k));
      }
      return s + "}";
    }
    case PinType::structure: {
      std::string s = "{";
      if (v.rows)
        for (std::size_t k = 0; k < v.rows->size(); ++k) s += (k ? ", " : "") + engine_shown((*v.rows)[k]);
      return s + "}";
    }
    default: {
      std::string s = "[";
      for (std::size_t k = 0; k < v.size(); ++k) s += (k ? ", " : "") + engine_shown(v.item(k));
      return s + "]";
    }
  }
}

namespace {

/// The next number of a sequence (splitmix64).
u64 next(u64& state) {
  u64 z = (state += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31);
}

/// A value of a form (EnginePin::shape), read from `at` on. Numbers are eighths, so that the engine's
/// 32-bit numbers hold them exactly; whole numbers are small enough for every whole type.
Value answer(std::string_view shape, std::size_t& at, u64& state) {
  const char c = at < shape.size() ? shape[at++] : 'n';
  switch (c) {
    case 'b': return Value::boolean(next(state) % 2 != 0);
    case 'i': return Value::integer(static_cast<i64>(next(state) % 7));
    case 'v': {
      Vec3 v;
      v.x = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      v.y = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      v.z = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      return Value::vector(v);
    }
    case 't': return Value::text(std::string(1, static_cast<char>('a' + next(state) % 5)));
    case 'l': return Value::int64(next(state));  // (an Int64: all of its 64 bits)
    case 'e': {
      i64 count = 0;
      while (at < shape.size() && shape[at] >= '0' && shape[at] <= '9') count = count * 10 + (shape[at++] - '0');
      return Value::enumeration(count > 0 ? static_cast<i64>(next(state) % static_cast<u64>(count)) : 0);
    }
    case '{': {
      std::vector<Value> fields;
      while (at < shape.size() && shape[at] != '}') fields.push_back(answer(shape, at, state));
      ++at;
      return Value::structure(std::move(fields));
    }
    case '[': {
      const std::size_t item = at;
      const std::size_t count = next(state) % 3;
      std::vector<Value> items;
      std::size_t end = at;
      for (std::size_t k = 0; k < count; ++k) {
        end = item;
        items.push_back(answer(shape, end, state));
      }
      if (count == 0) {   // (the item's form is read over)
        u64 unused = 0;
        end = item;
        answer(shape, end, unused);
      }
      at = end + 1;
      const char what = item < shape.size() ? shape[item] : 'n';
      if (what == '{') return Value::structures(std::move(items));
      if (what == 'e') {
        std::vector<i64> v;
        for (const Value& x : items) v.push_back(x.i);
        return Value::enumerations(std::move(v));
      }
      if (what == 'b') { std::vector<bool> v; for (const Value& x : items) v.push_back(x.b); return Value::of(std::move(v)); }
      if (what == 'i') { std::vector<i64> v; for (const Value& x : items) v.push_back(x.i); return Value::of(std::move(v)); }
      if (what == 'v') { std::vector<Vec3> v; for (const Value& x : items) v.push_back(x.v); return Value::of(std::move(v)); }
      if (what == 't') { std::vector<std::string> v; for (const Value& x : items) v.push_back(x.s); return Value::of(std::move(v)); }
      if (what == 'l') { std::vector<Int64> v; for (const Value& x : items) v.push_back(Int64(static_cast<u64>(x.i))); return Value::of(std::move(v)); }
      std::vector<f64> v;
      for (const Value& x : items) v.push_back(x.n);
      return Value::of(std::move(v));
    }
    default: return Value::number(static_cast<f64>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0);
  }
}

}  // namespace

bool EngineStandIn::engine(std::string_view function, const std::vector<Value>& in, std::vector<Value>& out) {
  const EngineFunction* f = engine_function(function);
  if (!f) return false;
  std::string call(function);
  call += "(";
  for (std::size_t k = 0; k < in.size(); ++k) call += (k ? ", " : "") + engine_shown(in[k]);
  call += ")";
  // the answer: from the call itself, and from how many calls came before it (the engine may say
  // something else when it is asked again)
  u64 state = 1469598103934665603ull + calls.size();
  for (const char ch : call) state = (state ^ static_cast<unsigned char>(ch)) * 1099511628211ull;
  for (std::size_t k = 0; k < f->outputs.size() && k < out.size(); ++k) {
    std::size_t at = 0;
    out[k] = answer(f->outputs[k].shape, at, state);
  }
  calls.push_back(std::move(call));
  return true;
}

}  // namespace kern::logic
