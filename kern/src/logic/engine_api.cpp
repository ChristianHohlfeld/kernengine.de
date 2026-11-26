// Kern Logic: the engine as nodes - the nodes of the table, and the stand-in of the tests
// (kern/logic/engine_api.h).
#include "kern/logic/engine_api.h"
#include "kern/logic/engine_host.h"

#include <algorithm>
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

const EngineInterface* engine_interface(std::string_view name) {
  for (const EngineInterface& i : engine_interfaces())
    if (name == i.name) return &i;
  return nullptr;
}

const EngineClass* engine_class(std::string_view name) {
  for (const EngineClass& c : engine_classes())
    if (name == c.name) return &c;
  return nullptr;
}

bool engine_class_is(std::string_view from, std::string_view to) {
  if (from == to) return engine_class(from) != nullptr;
  const EngineClass* const c = engine_class(from);
  if (!c) return false;
  for (const char* b : c->bases)
    if (to == b) return true;
  return false;
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
    d.world = true;  // (the engine: what a Callback's function does not ask)
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
      if (!f.field) c.host().engine(f.type, in, out);   // (a field of an object: nothing is asked - EngineFunction::field)
      // A place of the engine: the node binds it - its output is not what the function gave (a copy)
      // but what it was given, the place's key: the nodes that name it (setting "bound") read and set
      // what the engine holds there through the door, "<function>@get" / "<function>@set".
      if (f.place) {
        c.bind(first, std::move(in));
        return f.value ? kStop : 0;
      }
      for (std::size_t k = 0; k < f.outputs.size(); ++k) c.out(first + k, std::move(out[k]));
      return f.value ? kStop : 0;
    };
    lib.add(std::move(d));
  }
}

bool Host::engine(std::string_view, const std::vector<Value>&, std::vector<Value>&) { return false; }
ActorRef Host::find_actor(std::string_view) { return 0; }
EngineObjects* Host::engine_objects() { return nullptr; }
WorldId new_world() {
  static WorldId next = 0;
  return ++next;
}
WorldId Host::world() {
  if (EngineObjects* o = engine_objects()) return o->world();
  if (!own_world_) own_world_ = new_world();
  return own_world_;
}
void Host::engine_failed(std::string_view) {}

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
    case PinType::object: std::snprintf(b, sizeof b, "object %lld of world %u", static_cast<long long>(v.i), v.world); return b;
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

std::string engine_call_shown(std::string_view function, const std::vector<Value>& in) {
  std::string call(function);
  call += "(";
  for (std::size_t k = 0; k < in.size(); ++k) call += (k ? ", " : "") + engine_shown(in[k]);
  call += ")";
  return call;
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
/// 32-bit numbers hold them exactly; whole numbers are small enough for every whole type. A list has
/// no more than `most` entries; a list of a fixed length (`[16n]`) has exactly that many.
Value answer(std::string_view shape, std::size_t& at, u64& state, u64 most = ~u64{0}) {
  const char c = at < shape.size() ? shape[at++] : 'n';
  switch (c) {
    case 'b': return Value::boolean(next(state) % 2 != 0);
    case 'i': return Value::integer(static_cast<i64>(next(state) % 7));
    // (an object of the engine, of the stand-in's own world: `o` one or none, `h` one made - never none)
    case 'o': {
      const i64 h = static_cast<i64>(next(state) % 7);
      return Value::object(h ? ObjectRef{h, kStandInWorld} : ObjectRef{});
    }
    case 'h': return Value::object({1 + static_cast<i64>(next(state) % 7), kStandInWorld});
    case 'a': {  // (an actor, of the stand-in's own world, or none)
      const ActorRef id = static_cast<ActorRef>(next(state) % 7);
      return Value::actor(id ? ActorHandle{id, kStandInWorld} : ActorHandle{});
    }
    case 'v': {
      Vec3 v;
      v.x = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      v.y = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      v.z = static_cast<f32>(static_cast<i64>(next(state) % 2001) - 1000) / 8.0f;
      return Value::vector(v);
    }
    case 't': return Value::text(std::string(1, static_cast<char>('a' + next(state) % 5)));
    case 'c': {  // (a Callback the engine gives: unbound - its form is read over: c, what it gives, (what it is handed))
      while (at < shape.size() && shape[at] != ')') ++at;
      ++at;
      return Value::callback(nullptr);
    }
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
      // ([N item]: a list of a fixed length - `f32 m[16]` -, exactly N entries, as C++ holds them; [?item]: what may be
      // there or not - std::optional, std::unique_ptr of a value -, no entry or one; [item]: 0 to 2)
      u64 fixed = 0;
      bool sized = false;
      while (at < shape.size() && shape[at] >= '0' && shape[at] <= '9') { fixed = fixed * 10 + static_cast<u64>(shape[at++] - '0'); sized = true; }
      const bool maybe = !sized && at < shape.size() && shape[at] == '?';
      if (maybe) ++at;
      const std::size_t item = at;
      const std::size_t count = static_cast<std::size_t>(sized ? fixed : std::min<u64>(next(state) % (maybe ? 2 : 3), most));
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
      if (what == 'o' || what == 'h') { std::vector<ObjectRef> v; for (const Value& x : items) v.push_back(x.as_object()); return Value::of(std::move(v)); }
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

/// The Callbacks in a value of a form (EnginePin::shape): its own, or its fields' - with their forms.
void callbacks_in(std::string_view shape, std::size_t& at, const Value* v, std::vector<EngineStandIn::Kept>& out) {
  const char c = at < shape.size() ? shape[at++] : 'n';
  if (c == 'c') {
    const std::size_t from = at - 1;
    while (at < shape.size() && shape[at] != ')') ++at;
    ++at;
    if (v && v->type == PinType::callback && v->call) out.push_back({std::string(shape.substr(from, at - from)), *v});
  } else if (c == 'e') {
    while (at < shape.size() && shape[at] >= '0' && shape[at] <= '9') ++at;
  } else if (c == '{') {
    for (std::size_t k = 0; at < shape.size() && shape[at] != '}'; ++k) callbacks_in(shape, at, v ? v->field(k) : nullptr, out);
    ++at;
  } else if (c == '[') {
    while (at < shape.size() && shape[at] >= '0' && shape[at] <= '9') ++at;
    callbacks_in(shape, at, nullptr, out);  // (a list holds no Callback: its item's form is read over)
    ++at;
  }
}
/// Calls a Callback three times with inputs drawn from `state`; what it gives, written down.
std::string called(const EngineStandIn::Kept& k, u64& state) {
  const std::size_t open = k.shape.find('(');
  const std::string_view gives = std::string_view(k.shape).substr(1, open - 1), takes = std::string_view(k.shape).substr(open + 1, k.shape.size() - open - 2);
  std::string r = " ->";
  for (int time = 0; time < 3; ++time) {
    std::vector<Value> args;
    for (std::size_t at = 0; at < takes.size();) args.push_back(answer(takes, at, state));
    Value got;
    const bool bound = k.callback.call->call(args, got);
    r += " " + (bound && !gives.empty() ? engine_shown(got) : std::string(bound ? "()" : "unbound"));
  }
  return r;
}

}  // namespace

bool EngineStandIn::engine(std::string_view function, const std::vector<Value>& in, std::vector<Value>& out) {
  const std::size_t at = function.find('@');
  if (at != std::string_view::npos) {
    // (a place of the engine: read or set what the stand-in holds there)
    const EngineFunction* f = engine_function(function.substr(0, at));
    const std::string_view op = function.substr(at + 1);
    if (!f || !f->place || (op != "get" && op != "set") || (op == "set" && in.empty())) return false;
    const std::vector<Value> key_in(in.begin(), in.end() - (op == "set" ? 1 : 0));
    const std::string key = engine_call_shown(f->type, key_in);
    if (op == "set") {
      places[key] = in.back();
      return true;
    }
    auto it = places.find(key);
    if (it == places.end()) {
      u64 state = 1469598103934665603ull;
      for (const char ch : std::string(function) + key) state = (state ^ static_cast<unsigned char>(ch)) * 1099511628211ull;
      std::size_t from = 0;
      it = places.emplace(key, answer(f->outputs[0].shape, from, state)).first;
    }
    if (!out.empty()) out[0] = it->second;
    return true;
  }
  const EngineFunction* f = engine_function(function);
  if (!f) return false;
  std::string call = engine_call_shown(function, in);
  // the answer: from the call itself, and from how many calls came before it (the engine may say
  // something else when it is asked again)
  u64 state = 1469598103934665603ull + calls.size();
  for (const char ch : call) state = (state ^ static_cast<unsigned char>(ch)) * 1099511628211ull;
  // the Callbacks it is handed, and those an object it is called on was handed before: called, and what they give is
  // part of the call and of its answer
  std::vector<Kept> handed;
  for (std::size_t k = 0; k < f->inputs.size() && k < in.size(); ++k) {
    std::size_t at = 0;
    callbacks_in(f->inputs[k].shape, at, &in[k], handed);
  }
  const bool on_object = !in.empty() && in[0].type == PinType::object && !f->inputs.empty() && f->inputs[0].type == PinType::object;
  std::string heard;
  if (on_object)
    if (auto it = kept.find(in[0].i); it != kept.end())
      for (const auto& [which, k] : it->second) heard += called(k, state);
  for (const Kept& k : handed) heard += called(k, state);
  for (const char ch : heard) state = (state ^ static_cast<unsigned char>(ch)) * 1099511628211ull;
  call += heard;
  if (on_object) {
    const std::string_view type(f->type);
    if (type.size() > 8 && type.substr(type.size() - 8) == ".release") kept.erase(in[0].i);
    else
      for (std::size_t k = 0; k < handed.size(); ++k) kept[in[0].i][std::string(function) + "#" + std::to_string(k)] = handed[k];
  }
  for (std::size_t k = 0; k < f->outputs.size() && k < out.size(); ++k) {
    // (what the engine writes through a pointer: no more entries than its count says - the room there is)
    u64 most = ~u64{0};
    if (f->outputs[k].count[0])
      for (std::size_t j = 0; j < f->inputs.size() && j < in.size(); ++j)
        if (std::strcmp(f->inputs[j].id, f->outputs[k].count) == 0) most = in[j].i > 0 ? static_cast<u64>(in[j].i) * f->outputs[k].per : 0;
    std::size_t at = 0;
    out[k] = answer(f->outputs[k].shape, at, state, most);
  }
  calls.push_back(std::move(call));
  return true;
}

}  // namespace kern::logic
