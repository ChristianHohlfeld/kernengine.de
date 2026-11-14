// Kern Logic: values, graph data, the node library, checking and the interpreter.
#include "kern/logic/logic.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>

namespace kern::logic {

// ---------------------------------------------------------------------------
// pin types and values

namespace {
std::string number_text(f64 n) {
  if (std::isfinite(n) && std::fabs(n) < 1e15 && n == std::floor(n)) return std::to_string(static_cast<i64>(n));
  char b[48];
  std::snprintf(b, sizeof b, "%.4g", n);
  return b;
}
}  // namespace

const char* pin_type_name(PinType t) { return static_cast<u32>(t) < static_cast<u32>(PinType::count) ? kPinTypeNames[static_cast<u32>(t)].data() : "?"; }

namespace {
/// Is a type one the Blueprint declares (an enum's or a struct's, or a list of them)?
bool declared_type(PinType t) { return element_of(t) == PinType::enumeration || element_of(t) == PinType::structure; }
}  // namespace

std::vector<TypeRef> type_choices(const ParamDef& pd, const Graph& g) {
  std::vector<TypeRef> r;
  if (pd.keys_only) {
    for (PinType t : {PinType::integer, PinType::text, PinType::boolean}) r.push_back({t, {}});
    for (const EnumType& e : g.enums) r.push_back({PinType::enumeration, e.name});
    return r;
  }
  if (!pd.enums_only && !pd.structs_only)
    for (PinType t : {PinType::number, PinType::integer, PinType::boolean, PinType::vector, PinType::text, PinType::actor}) r.push_back({t, {}});
  if (!pd.structs_only)
    for (const EnumType& e : g.enums) r.push_back({PinType::enumeration, e.name});
  if (!pd.enums_only)
    for (const StructType& s : g.structs) r.push_back({PinType::structure, s.name});
  if (pd.with_lists)
    for (std::size_t k = 0, n = r.size(); k < n; ++k) r.push_back({list_of(r[k].type), r[k].of});
  return r;
}
std::string type_text(const TypeRef& t) {
  if (is_keyed(t.type)) return std::string(pin_type_name(t.type)) + "<" + t.of + ">";  // ("map<int>float>", "set<text>")
  // (a list of a fixed length says its length: "float_list[16]" - in messages; a setting names no such type)
  const std::string length = t.count ? "[" + std::to_string(t.count) + "]" : std::string();
  if (!declared_type(t.type)) return pin_type_name(t.type) + length;
  return (is_list(t.type) ? t.of + "_list" : t.of) + length;
}
bool type_from_text(std::string_view text, const Graph& g, TypeRef& out) {
  for (PinType keyed : {PinType::map, PinType::set}) {
    const std::string_view head = pin_type_name(keyed);
    if (text.size() > head.size() + 2 && text.substr(0, head.size()) == head && text[head.size()] == '<' && text.back() == '>') {
      TypeRef t{keyed, std::string(text.substr(head.size() + 1, text.size() - head.size() - 2))};
      const TypeRef key = key_type(t);
      if (!key_type_ok(key.type) || (key.type == PinType::enumeration && !g.enumeration(key.of))) return false;
      if (keyed == PinType::map) {
        const TypeRef value = value_type(t);
        TypeRef named;
        if (value.type == PinType::exec || is_keyed(value.type) || !type_from_text(type_text(value), g, named)) return false;
      } else if (t.of.find('>') != std::string::npos) {
        return false;
      }
      out = t;
      return true;
    }
  }
  PinType builtin;
  if (pin_type_from_name(text, builtin) && builtin != PinType::exec && !declared_type(builtin)) {
    // (the types' own names only: a setting holds the one name of its type)
    if (text != pin_type_name(builtin)) return false;
    out = {builtin, {}};
    return true;
  }
  const bool listed = text.size() > 5 && text.substr(text.size() - 5) == "_list";
  const std::string_view item = listed ? text.substr(0, text.size() - 5) : text;
  if (g.enumeration(text) || g.structure(text)) {
    out = {g.enumeration(text) ? PinType::enumeration : PinType::structure, std::string(text)};
    return true;
  }
  if (listed && (g.enumeration(item) || g.structure(item))) {
    out = {g.enumeration(item) ? PinType::enumeration_list : PinType::structure_list, std::string(item)};
    return true;
  }
  return false;
}
std::string type_label(const TypeRef& t) {
  if (t.type == PinType::map) return "Map of " + type_label(key_type(t)) + " to " + type_label(value_type(t));
  if (t.type == PinType::set) return "Set of " + type_label(key_type(t));
  static const char* const kLabels[] = {"Exec", "Boolean", "Integer", "Float", "Vector", "Text", "Actor"};
  const PinType e = element_of(t.type);
  const std::string name = declared_type(e) ? (!t.of.empty() ? t.of : e == PinType::enumeration ? std::string("Enum") : std::string("Struct")) : static_cast<u32>(e) < 7 ? kLabels[static_cast<u32>(e)] : "?";
  return is_list(t.type) ? name + " List" : name;
}

bool fits(const TypeRef& from, const TypeRef& to) {
  if (is_keyed(from.type) || is_keyed(to.type)) return from.type == to.type && from.of == to.of;  // (the same keys, the same values)
  if (!convertible(from.type, to.type)) return false;
  return !declared_type(from.type) || !declared_type(to.type) || from.of == to.of;
}

bool convertible(PinType from, PinType to) {
  if (from == to) return true;
  if (from == PinType::exec || to == PinType::exec) return false;
  // (a value of an enum is one of its names, a place among them: it is no number and no text
  // by itself - Enum to Text, Enum to Index say which is meant. A struct is its fields: Break
  // gives each of them)
  if (declared_type(from) || declared_type(to)) return false;
  if (is_keyed(from) || is_keyed(to)) return false;  // (a map is a map of its keys and values, a set a set of its keys)
  if (to == PinType::text) return true;  // everything prints
  if (is_list(from) || is_list(to)) return false;  // (a list is a list of its type and nothing else)
  const bool from_num = from == PinType::integer || from == PinType::number || from == PinType::boolean;
  const bool to_num = to == PinType::integer || to == PinType::number || to == PinType::boolean;
  if (from_num && to_num) return true;
  if (to == PinType::boolean && from == PinType::actor) return true;  // is valid
  if (to == PinType::vector && (from == PinType::number || from == PinType::integer)) return true;  // splat
  return false;
}

namespace {
/// One type in what TypeRef::of says of a map or a set: "int", "enum:Mode", "struct_list:Weapon".
TypeRef named_type(std::string_view text) {
  const std::size_t colon = text.find(':');
  TypeRef t;
  if (!pin_type_from_name(text.substr(0, colon), t.type)) t.type = PinType::exec;  // (no such type: the check says so)
  if (colon != std::string_view::npos) t.of = std::string(text.substr(colon + 1));
  return t;
}
std::string type_named(const TypeRef& t) { return std::string(pin_type_name(t.type)) + (t.of.empty() ? "" : ":" + t.of); }
}  // namespace

TypeRef key_type(const TypeRef& keyed) {
  const std::size_t at = keyed.of.find('>');
  return named_type(std::string_view(keyed.of).substr(0, at));
}
TypeRef value_type(const TypeRef& map) {
  const std::size_t at = map.of.find('>');
  return at == std::string::npos ? TypeRef{PinType::exec, {}} : named_type(std::string_view(map.of).substr(at + 1));
}
std::string keyed_of(const TypeRef& key, const TypeRef* value) { return type_named(key) + (value ? ">" + type_named(*value) : std::string()); }

MapKey key_of(const Value& v) {
  MapKey k;
  if (v.type == PinType::text) k.s = v.s;
  else k.i = v.type == PinType::boolean ? (v.b ? 1 : 0) : v.as_int();
  return k;
}
Value key_value(const MapKey& k, PinType key) {
  switch (key) {
    case PinType::boolean: return Value::boolean(k.i != 0);
    case PinType::enumeration: return Value::enumeration(k.i);
    case PinType::text: return Value::text(k.s);
    default: return Value::integer(k.i);
  }
}

const MapEntries& Value::keyed() const {
  static const MapEntries none;
  return entries && is_keyed(type) ? *entries : none;
}
MapEntries& Value::own_entries() {
  if (!entries) entries = std::make_shared<MapEntries>();
  else if (entries.use_count() > 1) entries = std::make_shared<MapEntries>(*entries);  // (someone else reads these)
  return const_cast<MapEntries&>(*entries);  // (made as MapEntries, here)
}

Value Value::zero(PinType t) {
  Value v;
  v.type = t;
  return v;
}

bool Value::as_bool() const {
  switch (type) {
    case PinType::boolean: return b;
    case PinType::integer: return i != 0;
    case PinType::number: return n != 0;
    case PinType::vector: return length_sq(v) > 0;
    case PinType::text: return !s.empty() && s != "false" && s != "0";
    case PinType::actor: return a != 0;
    case PinType::enumeration: return i != 0;
    default: return false;
  }
}

i64 Value::as_int() const {
  switch (type) {
    case PinType::boolean: return b ? 1 : 0;
    case PinType::integer: return i;
    case PinType::enumeration: return i;
    case PinType::number: return whole(n);
    case PinType::text: return std::strtoll(s.c_str(), nullptr, 10);
    case PinType::actor: return a;
    default: return 0;
  }
}

f64 Value::as_number() const {
  switch (type) {
    case PinType::boolean: return b ? 1 : 0;
    case PinType::integer: return static_cast<f64>(i);
    case PinType::enumeration: return static_cast<f64>(i);
    case PinType::number: return n;
    case PinType::vector: return length(v);
    case PinType::text: return std::strtod(s.c_str(), nullptr);
    case PinType::actor: return a;
    default: return 0;
  }
}

Vec3 Value::as_vector() const {
  if (type == PinType::vector) return v;
  const f32 x = static_cast<f32>(as_number());
  return {x, x, x};
}

std::string Value::as_text() const {
  switch (type) {
    case PinType::boolean: return b ? "true" : "false";
    case PinType::integer: return std::to_string(i);
    case PinType::number: return number_text(n);
    case PinType::vector: return "(" + number_text(v.x) + ", " + number_text(v.y) + ", " + number_text(v.z) + ")";
    case PinType::text: return s;
    case PinType::actor: return a ? "actor " + std::to_string(a) : "none";
    case PinType::enumeration: return std::to_string(i);  // (its place; its name is the enum's to say - Enum to Text)
    case PinType::structure: {
      // (what it holds, in the order of its fields; their names are the struct's to say - Break)
      std::string r = "{";
      for (std::size_t k = 0, n = fields(); k < n; ++k) r += (k ? ", " : "") + (*rows)[k].as_text();
      return r + "}";
    }
    default: break;
  }
  if (type == PinType::map) {  // a map prints its entries: {1: 2.5, 3: 4}
    std::string r = "{";
    for (std::size_t k = 0, n = size(); k < n; ++k) {
      const MapKey& key = entries->key_at(k);
      r += (k ? ", " : "") + (key.s.empty() && !key.i ? std::string("0") : key.s.empty() ? std::to_string(key.i) : key.s) + ": " + entries->value_at(k).as_text();
    }
    return r + "}";
  }
  if (type == PinType::set) {  // a set its keys: {1, 3}
    std::string r = "{";
    for (std::size_t k = 0, n = size(); k < n; ++k) r += (k ? ", " : "") + (entries->key_at(k).s.empty() ? std::to_string(entries->key_at(k).i) : entries->key_at(k).s);
    return r + "}";
  }
  if (!is_list(type)) return "";
  // a list prints its items: [1, 2.5, 3]
  std::string r = "[";
  for (std::size_t k = 0, n = size(); k < n; ++k) r += (k ? ", " : "") + item(k).as_text();
  return r + "]";
}

std::size_t Value::size() const {
  if (is_keyed(type)) return entries ? entries->size() : 0;
  if (type == PinType::structure_list) return rows ? rows->size() : 0;
  if (!list) return 0;
  switch (type) {
    case PinType::boolean_list: return std::get<std::vector<bool>>(*list).size();
    case PinType::integer_list: return std::get<std::vector<i64>>(*list).size();
    case PinType::number_list: return std::get<std::vector<f64>>(*list).size();
    case PinType::vector_list: return std::get<std::vector<Vec3>>(*list).size();
    case PinType::text_list: return std::get<std::vector<std::string>>(*list).size();
    case PinType::actor_list: return std::get<std::vector<ActorHandle>>(*list).size();
    case PinType::enumeration_list: return std::get<std::vector<i64>>(*list).size();
    default: return 0;
  }
}

Value Value::item(std::size_t index) const {
  if (type == PinType::map) return index < size() ? entries->value_at(index) : Value{};  // (a map's value at its place: what its type is says the map's - item_of)
  if (type == PinType::set) return Value{};
  if (index >= size()) return zero(element_of(type));
  if (type == PinType::structure_list) return (*rows)[index];
  switch (type) {
    case PinType::boolean_list: return boolean(std::get<std::vector<bool>>(*list)[index]);
    case PinType::integer_list: return integer(std::get<std::vector<i64>>(*list)[index]);
    case PinType::number_list: return number(std::get<std::vector<f64>>(*list)[index]);
    case PinType::vector_list: return vector(std::get<std::vector<Vec3>>(*list)[index]);
    case PinType::text_list: return text(std::get<std::vector<std::string>>(*list)[index]);
    case PinType::actor_list: return actor(std::get<std::vector<ActorHandle>>(*list)[index].id);
    case PinType::enumeration_list: return enumeration(std::get<std::vector<i64>>(*list)[index]);
    default: return zero(element_of(type));
  }
}

ActorRef Value::as_actor() const {
  if (type == PinType::actor) return a;
  if (type == PinType::integer) return static_cast<ActorRef>(max_of<i64>(0, i));
  if (type == PinType::number) return static_cast<ActorRef>(max_of(0.0, n));
  return 0;
}

Value Value::converted(PinType to) const {
  if (to == type) return *this;
  switch (to) {
    case PinType::boolean: return boolean(as_bool());
    case PinType::integer: return integer(as_int());
    case PinType::number: return number(as_number());
    case PinType::vector: return vector(as_vector());
    case PinType::text: return text(as_text());
    case PinType::actor: return actor(as_actor());
    case PinType::enumeration: return enumeration(as_int());
    case PinType::structure: return zero(to);  // (nothing but a struct is one: its default - zero_of)
    case PinType::enumeration_list:
      if (type == PinType::integer_list) {  // (what a node computed on the places of an enum's values: those values)
        Value r = *this;
        r.type = to;
        return r;
      }
      return zero(to);
    default: return is_list(to) || is_keyed(to) ? zero(to) : *this;  // (nothing but the list itself is a list of that type)
  }
}

json::Value Value::to_json() const {
  switch (type) {
    case PinType::boolean: return json::Value(b);
    case PinType::integer: return json::Value(static_cast<f64>(i));
    case PinType::number: return json::Value(std::isfinite(n) ? n : 0.0);
    case PinType::vector: {
      json::Value arr = json::Value::array();
      arr.push(static_cast<f64>(v.x));
      arr.push(static_cast<f64>(v.y));
      arr.push(static_cast<f64>(v.z));
      return arr;
    }
    case PinType::text: return json::Value(s);
    case PinType::actor: return json::Value(static_cast<f64>(a));
    case PinType::enumeration: return json::Value(static_cast<f64>(i));
    default: break;
  }
  // (a struct's fields have their names in its declaration: to_json(g, of) writes them)
  if (type == PinType::structure) return json::Value::object();
  if (is_keyed(type)) {  // (its keys as numbers and texts - with its type: to_json(g, declared))
    json::Value arr = json::Value::array();
    for (std::size_t k = 0, n = size(); k < n; ++k) {
      const MapKey& key = entries->key_at(k);
      const json::Value kj = key.s.empty() ? json::Value(static_cast<f64>(key.i)) : json::Value(key.s);
      if (type == PinType::set) {
        arr.push(kj);
        continue;
      }
      json::Value pair = json::Value::array();
      pair.push(kj);
      pair.push(entries->value_at(k).to_json());
      arr.push(pair);
    }
    return arr;
  }
  if (!is_list(type)) return json::Value();
  json::Value arr = json::Value::array();  // a list: its items
  for (std::size_t k = 0, n = size(); k < n; ++k) arr.push(item(k).to_json());
  return arr;
}

namespace {
/// How deep structs can be inside structs in `g`: a struct holds structs declared above it, so
/// no deeper than there are structs. (A graph somebody built wrong - a struct that contains
/// itself - ends there too; the check says what is wrong with it.)
int struct_depth(const Graph& g) { return static_cast<int>(g.structs.size()); }

Value read_value(const json::Value& j, const TypeRef& t, const Graph& g, int depth);
Value default_value(const StructType& s, const Graph& g, int depth) {
  std::vector<Value> fields;
  fields.reserve(s.fields.size());
  for (const StructField& f : s.fields) fields.push_back(read_value(f.value, f.type, g, depth - 1));
  return Value::structure(std::move(fields));
}
Value read_struct(const json::Value& j, const StructType& s, const Graph& g, int depth) {
  std::vector<Value> fields;
  fields.reserve(s.fields.size());
  for (const StructField& f : s.fields) {
    const json::Value* held = j.is_object() ? j.find(f.name) : nullptr;
    fields.push_back(read_value(held ? *held : f.value, f.type, g, depth - 1));
  }
  return Value::structure(std::move(fields));
}
/// A list at the length it is declared with (TypeRef::count): cut at its end, or filled up with
/// the zero of its entries.
void at_length(Value& v, const TypeRef& t, const Graph& g) {
  if (!t.count || !is_list(t.type) || v.type != t.type || v.size() == t.count) return;
  switch (element_of(t.type)) {
    case PinType::boolean: v.edit<bool>().resize(t.count); break;
    case PinType::integer:
    case PinType::enumeration: v.edit<i64>().resize(t.count); break;
    case PinType::number: v.edit<f64>().resize(t.count); break;
    case PinType::vector: v.edit<Vec3>().resize(t.count); break;
    case PinType::text: v.edit<std::string>().resize(t.count); break;
    case PinType::actor: v.edit<ActorHandle>().resize(t.count); break;
    case PinType::structure: v.edit<Value>().resize(t.count, zero_of(TypeRef{PinType::structure, t.of}, g)); break;
    default: break;
  }
}
Value read_value(const json::Value& j, const TypeRef& t, const Graph& g, int depth) {
  if (is_keyed(t.type)) {
    // a map: [[key, value], ...]; a set: [key, ...] - in the order of the entries (a key said again: the first)
    Value v = Value::zero(t.type);
    const TypeRef key = key_type(t), value = t.type == PinType::map ? value_type(t) : TypeRef{};
    for (const json::Value& x : j.items()) {
      const bool pair = t.type == PinType::map && x.is_array() && x.size() == 2;
      if (t.type == PinType::map && !pair) continue;
      const MapKey k = key_of(Value::from_json(pair ? x[0] : x, key.type));
      MapEntries& e = v.own_entries();
      if (e.contains(k)) continue;
      e.try_emplace(k, pair ? read_value(x[1], value, g, depth) : Value{});
    }
    return v;
  }
  const StructType* s = element_of(t.type) == PinType::structure && depth > 0 ? g.structure(t.of) : nullptr;
  if (!s) {
    Value v = Value::from_json(j, t.type);
    at_length(v, t, g);
    return v;
  }
  if (t.type == PinType::structure) return read_struct(j, *s, g, depth);
  std::vector<Value> items;
  for (const json::Value& x : j.items()) items.push_back(read_struct(x, *s, g, depth));
  Value v = Value::structures(std::move(items));
  at_length(v, t, g);
  return v;
}
/// Bit for bit the same value of type `t`? (-0.0 is not 0.0; a field a struct's value does not
/// hold is at its declared default.)
bool identical(const Value& a, const Value& b, const TypeRef& t, const Graph& g, int depth) {
  if (a.type != b.type) return false;
  auto bits = [](f64 x, f64 y) { return std::memcmp(&x, &y, sizeof x) == 0; };
  auto bits32 = [](f32 x, f32 y) { return std::memcmp(&x, &y, sizeof x) == 0; };
  switch (a.type) {
    case PinType::boolean: return a.b == b.b;
    case PinType::integer:
    case PinType::enumeration: return a.i == b.i;
    case PinType::number: return bits(a.n, b.n);
    case PinType::vector: return bits32(a.v.x, b.v.x) && bits32(a.v.y, b.v.y) && bits32(a.v.z, b.v.z);
    case PinType::text: return a.s == b.s;
    case PinType::actor: return a.a == b.a;
    default: break;
  }
  if (a.type == PinType::structure) {
    const StructType* s = depth > 0 ? g.structure(t.of) : nullptr;
    if (!s) return a == b;
    for (std::size_t k = 0; k < s->fields.size(); ++k) {
      const Value* x = a.field(k);
      const Value* y = b.field(k);
      if (!x && !y) continue;  // (both at the field's default)
      const StructField& f = s->fields[k];
      if (!identical(x ? *x : read_value(f.value, f.type, g, depth - 1), y ? *y : read_value(f.value, f.type, g, depth - 1), f.type, g, depth - 1)) return false;
    }
    return true;
  }
  if (is_keyed(a.type)) {
    if (a.size() != b.size()) return false;
    const TypeRef value = a.type == PinType::map ? value_type(t) : TypeRef{};
    for (std::size_t k = 0, n = a.size(); k < n; ++k) {
      if (!(a.keyed().key_at(k) == b.keyed().key_at(k))) return false;
      if (a.type == PinType::map && !identical(a.keyed().value_at(k), b.keyed().value_at(k), value, g, depth)) return false;
    }
    return true;
  }
  if (!is_list(a.type) || a.size() != b.size()) return !is_list(a.type);
  const TypeRef item{element_of(t.type), t.of};
  for (std::size_t k = 0, n = a.size(); k < n; ++k)
    if (!identical(a.item(k), b.item(k), item, g, depth)) return false;
  return true;
}
std::size_t said_of(const Value& v, const TypeRef& t, const Graph& g, int depth) {
  std::size_t n = v.size();
  if (!t.count || !is_list(t.type) || v.type != t.type) return n;
  if (n > t.count) n = t.count;
  auto zero = [](f64 x) { return x == 0 && !std::signbit(x); };
  switch (element_of(t.type)) {
    case PinType::boolean: for (const auto& x = v.items<bool>(); n && !x[n - 1];) --n; break;
    case PinType::integer:
    case PinType::enumeration: for (const auto& x = v.items<i64>(); n && x[n - 1] == 0;) --n; break;
    case PinType::number: for (const auto& x = v.items<f64>(); n && zero(x[n - 1]);) --n; break;
    case PinType::vector: for (const auto& x = v.items<Vec3>(); n && zero(x[n - 1].x) && zero(x[n - 1].y) && zero(x[n - 1].z);) --n; break;
    case PinType::text: for (const auto& x = v.items<std::string>(); n && x[n - 1].empty();) --n; break;
    case PinType::actor: for (const auto& x = v.items<ActorHandle>(); n && x[n - 1] == ActorHandle{};) --n; break;
    case PinType::structure: {
      // (the zero of a struct is the struct at its declared defaults: at_length fills up with it)
      const TypeRef item{PinType::structure, t.of};
      const Value none = zero_of(item, g);
      for (const auto& x = v.items<Value>(); n && identical(x[n - 1], none, item, g, depth);) --n;
      break;
    }
    default: break;
  }
  return n;
}
/// A value as JSON: a struct is an object of its fields by their names. `said`: as a default says
/// it - a list of a fixed length (the place itself: `t`; a field of a struct) up to its last
/// entry that is not zero; else in full, every entry.
json::Value written(const Value& v, const TypeRef& t, const Graph& g, int depth, bool said) {
  if (is_keyed(v.type)) {
    json::Value arr = json::Value::array();
    const TypeRef key = key_type(t), value = v.type == PinType::map ? value_type(t) : TypeRef{};
    for (std::size_t k = 0, n = v.size(); k < n; ++k) {
      const json::Value kj = key_value(v.keyed().key_at(k), key.type).to_json();
      if (v.type == PinType::set) {
        arr.push(kj);
        continue;
      }
      json::Value pair = json::Value::array();
      pair.push(kj);
      pair.push(written(v.keyed().value_at(k), value, g, depth, said));
      arr.push(pair);
    }
    return arr;
  }
  const StructType* s = element_of(v.type) == PinType::structure && depth > 0 ? g.structure(t.of) : nullptr;
  const std::size_t entries = !is_list(v.type) ? 0 : said ? said_of(v, t, g, depth) : v.size();
  if (!s) {
    if (!is_list(v.type) || entries == v.size()) return v.to_json();
    json::Value arr = json::Value::array();
    for (std::size_t k = 0; k < entries; ++k) arr.push(v.item(k).to_json());
    return arr;
  }
  auto one = [&](const Value& x) {
    json::Value o = json::Value::object();
    for (std::size_t k = 0; k < s->fields.size(); ++k) {
      const StructField& f = s->fields[k];
      const Value* held = x.field(k);
      o.set(f.name, written(held ? *held : read_value(f.value, f.type, g, depth - 1), f.type, g, depth - 1, said));
    }
    return o;
  };
  if (v.type == PinType::structure) return one(v);
  json::Value arr = json::Value::array();
  const auto& rows = v.items<Value>();
  for (std::size_t k = 0; k < entries; ++k) arr.push(one(rows[k]));
  return arr;
}
bool value_of(const Value& v, const TypeRef& t, const Graph& g, int depth) {
  if (v.type != t.type) return false;
  if (is_keyed(t.type)) {
    const TypeRef key = key_type(t), value = t.type == PinType::map ? value_type(t) : TypeRef{};
    for (std::size_t k = 0, n = v.size(); k < n; ++k) {
      if (!value_of(key_value(v.keyed().key_at(k), key.type), key, g, depth)) return false;
      if (t.type == PinType::map && !value_of(v.keyed().value_at(k), value, g, depth)) return false;
    }
    return true;
  }
  if (element_of(t.type) == PinType::enumeration) {
    const EnumType* e = g.enumeration(t.of);
    const i64 count = e ? static_cast<i64>(e->values.size()) : 0;
    if (t.type == PinType::enumeration) return v.i >= 0 && v.i < count;
    for (i64 x : v.items<i64>())
      if (x < 0 || x >= count) return false;
    return true;
  }
  if (element_of(t.type) != PinType::structure) return true;
  const StructType* s = depth > 0 ? g.structure(t.of) : nullptr;
  if (!s) return false;
  auto one = [&](const Value& x) {
    if (x.type != PinType::structure || x.fields() != s->fields.size()) return false;
    for (std::size_t k = 0; k < s->fields.size(); ++k)
      if (!value_of(*x.field(k), s->fields[k].type, g, depth - 1)) return false;
    return true;
  };
  if (t.type == PinType::structure) return one(v);
  for (const Value& x : v.items<Value>())
    if (!one(x)) return false;
  return true;
}
}  // namespace

json::Value Value::to_json(const Graph& g, std::string_view of) const { return written(*this, TypeRef{type, std::string(of)}, g, struct_depth(g), false); }
json::Value Value::to_json(const Graph& g, const TypeRef& declared) const { return written(*this, declared, g, struct_depth(g), true); }
Value Value::from_json(const json::Value& j, PinType t, const Graph& g, std::string_view of) { return read_value(j, {t, std::string(of)}, g, struct_depth(g)); }

Value zero_of(const TypeRef& t, const Graph& g) {
  const StructType* s = t.type == PinType::structure ? g.structure(t.of) : nullptr;
  if (s) return default_of(*s, g);
  Value v = Value::zero(t.type);
  at_length(v, t, g);  // (a list of a fixed length: that many zeros)
  return v;
}
Value Value::from_json(const json::Value& j, const TypeRef& declared, const Graph& g) { return read_value(j, declared, g, struct_depth(g)); }
bool has_fixed(const TypeRef& t, const Graph& g) {
  if (t.count) return true;
  if (element_of(t.type) != PinType::structure) return false;
  // (a struct holds structs declared above it: no deeper than there are structs)
  std::vector<const StructType*> todo;
  if (const StructType* s = g.structure(t.of)) todo.push_back(s);
  for (std::size_t seen = 0; seen < todo.size() && seen <= g.structs.size(); ++seen)
    for (const StructField& f : todo[seen]->fields) {
      if (f.type.count) return true;
      if (element_of(f.type.type) != PinType::structure) continue;
      const StructType* inner = g.structure(f.type.of);
      if (inner && std::find(todo.begin(), todo.end(), inner) == todo.end()) todo.push_back(inner);
    }
  return false;
}
namespace {
void fit(Value& v, const TypeRef& t, const Graph& g, int depth) {
  if (v.type != t.type) return;
  at_length(v, t, g);
  if (element_of(t.type) != PinType::structure || depth <= 0) return;
  const StructType* s = g.structure(t.of);
  if (!s || !has_fixed(TypeRef{PinType::structure, t.of}, g)) return;
  auto one = [&](Value& x) {
    if (x.type != PinType::structure) return;
    hold_fields(x, *s, g);
    std::vector<Value> fields;
    fields.reserve(s->fields.size());
    for (std::size_t k = 0; k < s->fields.size(); ++k) {
      Value f = *x.field(k);
      fit(f, s->fields[k].type, g, depth - 1);
      fields.push_back(std::move(f));
    }
    x = Value::structure(std::move(fields));
  };
  if (t.type == PinType::structure) one(v);
  else
    for (Value& x : v.edit<Value>()) one(x);
}
}  // namespace
std::size_t said(const Value& list, const TypeRef& declared, const Graph& g) { return said_of(list, declared, g, struct_depth(g)); }
Value fitted(Value v, const TypeRef& t, const Graph& g) {
  if (has_fixed(t, g)) fit(v, t, g, struct_depth(g));
  return v;
}
Value default_of(const StructType& s, const Graph& g) { return default_value(s, g, struct_depth(g)); }
Value field_of(const Value& v, const StructType& s, std::size_t index, const Graph& g) {
  if (const Value* held = v.field(index)) return *held;
  if (index >= s.fields.size()) return Value();
  return read_value(s.fields[index].value, s.fields[index].type, g, struct_depth(g) - 1);
}
bool is_value_of(const Value& v, const TypeRef& t, const Graph& g) { return value_of(v, t, g, struct_depth(g)); }
void hold_fields(Value& v, const StructType& s, const Graph& g) {
  if (v.type == PinType::structure && v.fields() == s.fields.size()) return;
  std::vector<Value> fields;
  fields.reserve(s.fields.size());
  for (std::size_t k = 0; k < s.fields.size(); ++k) fields.push_back(field_of(v, s, k, g));
  v = Value::structure(std::move(fields));
}

namespace {
Value from_outside_typed(const Value& v, const TypeRef& t, const Graph& g);
}
Value from_outside(const Value& v, const TypeRef& t, const Graph& g) { return fitted(from_outside_typed(v, t, g), t, g); }
namespace {
Value from_outside_typed(const Value& v, const TypeRef& t, const Graph& g) {
  Value r = v.converted(t.type);
  if (element_of(t.type) == PinType::enumeration) {
    // one of the enum's values: what is none of them is the first (inside a Blueprint every enum
    // value is one - the check and the enum nodes see to that)
    const EnumType* e = g.enumeration(t.of);
    const i64 count = e ? static_cast<i64>(e->values.size()) : 0;
    if (t.type == PinType::enumeration) {
      if (r.i < 0 || r.i >= count) r.i = 0;
      return r;
    }
    bool all = true;
    for (i64 x : r.items<i64>()) all = all && x >= 0 && x < count;
    if (!all)
      for (i64& x : r.edit<i64>())
        if (x < 0 || x >= count) x = 0;
    return r;
  }
  if (element_of(t.type) != PinType::structure) return r;
  // a struct of that type, with a value for every field: anything else is the struct's default
  // (in a list: item by item)
  const TypeRef one{PinType::structure, t.of};
  if (t.type == PinType::structure) return is_value_of(r, one, g) ? r : zero_of(one, g);
  bool all = true;
  for (const Value& x : r.items<Value>()) all = all && is_value_of(x, one, g);
  if (!all)
    for (Value& x : r.edit<Value>())
      if (!is_value_of(x, one, g)) x = zero_of(one, g);
  return r;
}
}  // namespace

std::vector<std::string> structs_in(const StructType& s, const Graph& g) {
  std::vector<std::string> names;
  std::vector<const StructType*> todo{&s};
  while (!todo.empty()) {
    const StructType* at = todo.back();
    todo.pop_back();
    for (const StructField& f : at->fields) {
      if (element_of(f.type.type) != PinType::structure || f.type.of == s.name) continue;
      if (std::find(names.begin(), names.end(), f.type.of) != names.end()) continue;
      names.push_back(f.type.of);
      if (const StructType* inner = g.structure(f.type.of)) todo.push_back(inner);
    }
  }
  return names;
}

Value Value::from_json(const json::Value& j, PinType t) {
  Value v = zero(t);
  switch (t) {
    case PinType::boolean: v.b = j.is_bool() ? j.as_bool() : j.truthy(); break;
    case PinType::integer: v.i = normal(j.is_number() ? whole(std::round(j.as_number())) : j.is_string() ? std::strtoll(j.as_string().c_str(), nullptr, 10) : 0); break;
    case PinType::number: v.n = j.is_number() ? j.as_number() : j.is_string() ? std::strtod(j.as_string().c_str(), nullptr) : 0; break;
    case PinType::vector:
      if (j.is_array() && j.size() >= 3) v.v = {static_cast<f32>(j[0].as_number()), static_cast<f32>(j[1].as_number()), static_cast<f32>(j[2].as_number())};
      else if (j.is_number()) v.v = Vec3{1, 1, 1} * static_cast<f32>(j.as_number());
      break;
    case PinType::text: v.s = j.is_string() ? j.as_string() : j.is_null() ? std::string() : json::stringify(j); break;
    case PinType::actor: v.a = j.is_number() ? static_cast<ActorRef>(max_of(0.0, j.as_number())) : 0; break;
    case PinType::enumeration: v.i = j.is_number() ? max_of(i64{0}, whole(j.as_number())) : 0; break;
    default: break;
  }
  // (a list of structs: from_json(j, t, g, of) reads them - their fields are the declaration's)
  if (is_list(t) && t != PinType::structure_list && j.is_array() && j.size() > 0) {
    // a list: an array of its items (anything else: the empty list)
    const PinType e = element_of(t);
    auto held = std::make_shared<ListItems>();
    for (std::size_t k = 0; k < j.size(); ++k) {
      const Value x = from_json(j[k], e);
      switch (e) {
        case PinType::boolean: std::get<std::vector<bool>>(*held).push_back(x.b); break;
        case PinType::integer: std::get<std::vector<i64>>(*held).push_back(x.i); break;
        case PinType::number: std::get<std::vector<f64>>(*held).push_back(x.n); break;
        case PinType::vector: std::get<std::vector<Vec3>>(*held).push_back(x.v); break;
        case PinType::text: std::get<std::vector<std::string>>(*held).push_back(x.s); break;
        case PinType::actor: std::get<std::vector<ActorHandle>>(*held).push_back({x.a}); break;
        case PinType::enumeration: std::get<std::vector<i64>>(*held).push_back(x.i); break;
        default: break;
      }
    }
    v.list = std::move(held);
  }
  return v;
}

bool Value::operator==(const Value& o) const {
  if (type != o.type) {
    // (an enum's value is no number and no text: it is equal to an enum's value only - and a
    // struct to a struct)
    if (type == PinType::enumeration || o.type == PinType::enumeration || type == PinType::structure || o.type == PinType::structure) return false;
    if ((type == PinType::text) != (o.type == PinType::text)) return as_text() == o.as_text();
    if (is_list(type) || is_list(o.type) || is_keyed(type) || is_keyed(o.type)) return false;
    return as_number() == o.as_number();
  }
  if (is_keyed(type)) return size() == o.size() && (size() == 0 || entries == o.entries || *entries == *o.entries);  // (the same entries in the same order)
  switch (type) {
    case PinType::boolean: return b == o.b;
    case PinType::integer: return i == o.i;
    case PinType::number: return n == o.n;
    case PinType::vector: return v.x == o.v.x && v.y == o.v.y && v.z == o.v.z;
    case PinType::text: return s == o.s;
    case PinType::actor: return a == o.a;
    case PinType::enumeration: return i == o.i;
    default: break;
  }
  if (element_of(type) == PinType::structure) {
    // structs: every field equal; lists of them: the same structs in the same order
    const std::size_t count = rows ? rows->size() : 0;
    if (count != (o.rows ? o.rows->size() : 0)) return false;
    return count == 0 || rows == o.rows || *rows == *o.rows;
  }
  if (!is_list(type)) return true;
  if (size() != o.size()) return false;  // lists: the same items in the same order
  return size() == 0 || *list == *o.list;
}

// ---------------------------------------------------------------------------
// graph data

const json::Value* Node::value(std::string_view pin) const {
  for (const auto& [k, v] : values) if (k == pin) return &v;
  return nullptr;
}

std::string Node::param(std::string_view key, std::string_view fallback) const {
  for (const auto& [k, v] : params) if (k == key) return v;
  return std::string(fallback);
}

void Node::set_value(std::string_view pin, json::Value v) {
  for (auto& [k, x] : values) if (k == pin) { x = std::move(v); return; }
  values.emplace_back(std::string(pin), std::move(v));
}

void Node::set_param(std::string_view key, std::string v) {
  for (auto& [k, x] : params) if (k == key) { x = std::move(v); return; }
  params.emplace_back(std::string(key), std::move(v));
}

const char* trigger_kind_name(TriggerKind k) {
  switch (k) {
    case TriggerKind::event: return "event";
    case TriggerKind::timer: return "timer";
    case TriggerKind::condition: return "condition";
    case TriggerKind::automatic: return "auto";
  }
  return "event";
}

const State* StateMachine::find(u32 state) const {
  for (const State& s : states) if (s.id == state) return &s;
  return nullptr;
}
const State* StateMachine::find(std::string_view name) const {
  for (const State& s : states) if (s.name == name) return &s;
  return nullptr;
}

Node* Graph::find(u32 id) {
  for (Node& n : nodes) if (n.id == id) return &n;
  return nullptr;
}
const Node* Graph::find(u32 id) const {
  for (const Node& n : nodes) if (n.id == id) return &n;
  return nullptr;
}
const Variable* Graph::variable(std::string_view name) const {
  for (const Variable& v : variables) if (v.name == name) return &v;
  return nullptr;
}
Variable* Graph::variable(std::string_view name) {
  for (Variable& v : variables) if (v.name == name) return &v;
  return nullptr;
}
bool Graph::variable_of(const Node& n, Variable& out) const { return root_of(n, out) != Root::none; }
Graph::Root Graph::root_of(const Node& n, Variable& out, std::string_view prefix) const {
  const std::string name = n.param(std::string(prefix) + "variable"), scope = n.param("scope");
  const Function* f = scope.empty() ? nullptr : function(scope);
  // (in a function: its local variables and its inputs come before the Blueprint's variables -
  // a variable node of a local variable says so with "scope", a call's place may be any of them)
  if (const FunctionPin* l = f ? f->local(name) : nullptr) {
    out = Variable{l->name, l->type, l->value, {}, l->of, l->count};
    return Root::local;
  }
  if (const FunctionPin* in = f ? f->input(name) : nullptr) {
    out = Variable{in->name, in->type, in->value, {}, in->of, in->count};
    return !in->ref ? Root::input : in->constant ? Root::constant_reference : Root::reference;
  }
  // (the variable nodes name a variable of the Blueprint without "scope"; a call that stands in a
  // function names one where the function has no local variable or input of that name)
  if (!scope.empty() && prefix.empty()) return Root::none;
  const Variable* v = variable(name);
  if (v) out = *v;
  return !v ? Root::none : v->constant ? Root::constant : Root::variable;
}
bool path_steps(std::string_view path, std::vector<std::string>& steps) {
  steps.clear();
  std::size_t k = 0;
  while (k < path.size()) {
    if (path[k] == '[') {
      if (k + 1 >= path.size() || path[k + 1] != ']') return false;
      steps.emplace_back();
      k += 2;
    } else if (path[k] == '.') {
      std::size_t end = k + 1;
      while (end < path.size() && path[end] != '.' && path[end] != '[') ++end;
      if (end == k + 1) return false;
      steps.emplace_back(path.substr(k + 1, end - k - 1));
      k = end;
    } else {
      return false;
    }
  }
  return true;
}
std::string path_text(const std::vector<std::string>& steps) {
  std::string r;
  for (const std::string& step : steps) r += step.empty() ? std::string("[]") : "." + step;
  return r;
}
bool Graph::path_of(const Node& n, std::vector<std::string>& steps, std::vector<std::string>& owners, std::string_view prefix) const {
  owners.clear();
  Variable var;
  if (!path_steps(n.param(std::string(prefix) + "path"), steps) || root_of(n, var, prefix) == Root::none) return false;
  TypeRef at{var.type, var.of};
  bool fits = true;
  for (const std::string& step : steps) {
    if (step.empty()) {
      fits = fits && (is_list(at.type) || at.type == PinType::map);
      if (fits) at = at.type == PinType::map ? value_type(at) : TypeRef{element_of(at.type), at.of};
      owners.emplace_back();
      continue;
    }
    const StructType* s = fits && at.type == PinType::structure ? structure(at.of) : nullptr;
    const int k = s ? s->field(step) : -1;
    owners.push_back(k >= 0 ? s->name : std::string());
    fits = k >= 0;
    if (fits) at = s->fields[static_cast<std::size_t>(k)].type;
  }
  return true;
}
std::size_t path_indices(std::string_view path) {
  std::size_t n = 0;
  for (char c : path) n += c == '[';
  return n;
}
std::string path_pin(std::size_t k) { return "path" + std::to_string(k); }
std::string ref_key(std::string_view input, std::string_view what) { return std::string(input) + "." + std::string(what); }
const FunctionPin* input_before_reference(const Function& f) {
  const FunctionPin* early = nullptr;
  for (const FunctionPin& in : f.inputs) {
    if (in.ref && early) return early;
    if (!in.ref && !early) early = &in;
  }
  return nullptr;
}
std::string input_order_problem(const Function& f, const FunctionPin& early) {
  return "function \"" + f.name + "\": the input \"" + early.name +
         "\" is a value with a default and stands before a reference - the inputs by reference come first (in C++ every input after one with a default has one)";
}
void order_inputs(Function& f) {
  std::stable_partition(f.inputs.begin(), f.inputs.end(), [](const FunctionPin& in) { return in.ref; });
}
bool is_path_pin(std::string_view id) {
  if (id.size() < 5 || id.substr(0, 4) != "path") return false;
  for (char c : id.substr(4))
    if (c < '0' || c > '9') return false;
  return true;
}
bool Graph::place_of(const Node& n, Variable& out, std::string* why, std::string_view prefix) const {
  if (root_of(n, out, prefix) == Root::none) {
    if (why) *why = "no variable \"" + n.param(std::string(prefix) + "variable") + "\"";
    return false;
  }
  const std::string path = n.param(std::string(prefix) + "path");
  if (path.empty()) return true;
  std::vector<std::string> steps;
  if (!path_steps(path, steps)) {
    if (why) *why = "\"" + path + "\" is no path: fields and list entries lead from a variable to a place (.Field, [])";
    return false;
  }
  TypeRef at{out.type, out.of, out.count};
  std::string name = out.name;
  const json::Value* declared = nullptr;  // (the default the last step's field declares)
  for (const std::string& step : steps) {
    if (step.empty()) {
      if (!is_list(at.type) && at.type != PinType::map) {
        if (why) *why = name + " is " + type_text(at) + ": it has no entries";
        return false;
      }
      at = at.type == PinType::map ? value_type(at) : TypeRef{element_of(at.type), at.of};  // (of a map: the value at its place)
      at.count = 0;
      name += "[]";
      declared = nullptr;
      continue;
    }
    const StructType* s = at.type == PinType::structure ? structure(at.of) : nullptr;
    if (!s) {
      if (why) *why = name + " is " + type_text(at) + ": it has no fields";
      return false;
    }
    const int k = s->field(step);
    if (k < 0) {
      std::string all;
      for (const StructField& f : s->fields) all += (all.empty() ? "" : ", ") + f.name;
      if (why) *why = s->name + " has no field \"" + step + "\" (" + all + ")";
      return false;
    }
    at = s->fields[static_cast<std::size_t>(k)].type;
    declared = &s->fields[static_cast<std::size_t>(k)].value;
    name += "." + step;
  }
  // what stands there when nothing else is said: a field's declared default, an entry's zero
  out.value = declared ? *declared : zero_of(at, *this).to_json(*this, at);
  out.name = name;
  out.type = at.type;
  out.of = at.of;
  out.count = at.count;
  return true;
}
namespace {
/// The nodes of a function's body: the steps its Function Entry leads to along exec wires, and
/// the values they read.
std::vector<u32> body_nodes(const Graph& g, u32 entry, const Library& lib) {
  std::vector<u32> todo{entry}, seen{entry};
  auto take = [&](u32 id) {
    if (std::find(seen.begin(), seen.end(), id) != seen.end()) return;
    seen.push_back(id);
    todo.push_back(id);
  };
  while (!todo.empty()) {
    const u32 at = todo.back();
    todo.pop_back();
    const Node* n = g.find(at);
    if (!n) continue;
    std::vector<PinDef> in, out;
    lib.pins(*n, g, in, out);
    for (const Link& l : g.links) {
      if (l.from == at)
        for (const PinDef& p : out)
          if (p.id == l.out && p.type == PinType::exec) take(l.to);
      if (l.to == at)
        for (const PinDef& p : in)
          if (p.id == l.in && p.type != PinType::exec) take(l.from);
    }
  }
  return seen;
}
}  // namespace
std::string Graph::body_of(u32 node, const Library& lib) const {
  std::string body;
  for (const Node& entry : nodes) {
    if (entry.type != "func.entry") continue;
    const std::vector<u32> seen = body_nodes(*this, entry.id, lib);
    if (std::find(seen.begin(), seen.end(), node) == seen.end()) continue;
    if (!body.empty() && body != entry.param("function")) return {};
    body = entry.param("function");
  }
  return body;
}
StateMachine* Graph::machine(u32 id) {
  for (StateMachine& m : machines) if (m.id == id) return &m;
  return nullptr;
}
const StateMachine* Graph::machine(std::string_view name) const {
  for (const StateMachine& m : machines) if (m.name == name) return &m;
  return nullptr;
}
bool same_type(const EnumType& a, const EnumType& b) { return a.name == b.name && a.values == b.values; }
bool same_type(const StructType& a, const StructType& b) {
  if (a.name != b.name || a.fields.size() != b.fields.size()) return false;
  for (std::size_t k = 0; k < a.fields.size(); ++k) {
    const StructField &x = a.fields[k], &y = b.fields[k];
    if (x.name != y.name || x.type != y.type || json::stringify(x.value) != json::stringify(y.value)) return false;
  }
  return true;
}

int StructType::field(std::string_view n) const {
  for (std::size_t k = 0; k < fields.size(); ++k) if (fields[k].name == n) return static_cast<int>(k);
  return -1;
}
const StructType* Graph::structure(std::string_view name) const {
  for (const StructType& s : structs) if (s.name == name) return &s;
  return nullptr;
}
StructType* Graph::structure(std::string_view name) {
  for (StructType& s : structs) if (s.name == name) return &s;
  return nullptr;
}
const EnumType* Graph::enumeration(std::string_view name) const {
  for (const EnumType& e : enums) if (e.name == name) return &e;
  return nullptr;
}
EnumType* Graph::enumeration(std::string_view name) {
  for (EnumType& e : enums) if (e.name == name) return &e;
  return nullptr;
}
const Function* Graph::function(std::string_view name) const {
  for (const Function& f : functions) if (f.name == name) return &f;
  return nullptr;
}
bool Graph::uses_library(std::string_view name) const { return std::find(uses.begin(), uses.end(), name) != uses.end(); }

// ---------------------------------------------------------------------------
// libraries: Blueprints that keep nothing, and the Blueprints that use them

std::string library_function(std::string_view library, std::string_view function) { return std::string(library) + "::" + std::string(function); }

namespace {
/// "Random::range" -> Random, range. False: a name of the Blueprint's own.
bool split_library_function(std::string_view name, std::string& library, std::string& function) {
  const std::size_t at = name.find("::");
  if (at == std::string_view::npos) return false;
  library = std::string(name.substr(0, at));
  function = std::string(name.substr(at + 2));
  return true;
}
bool calls_function(const Node& n) { return n.type == "func.call" || n.type == "func.pure"; }
bool same_pins(const std::vector<FunctionPin>& a, const std::vector<FunctionPin>& b) {
  if (a.size() != b.size()) return false;
  for (std::size_t k = 0; k < a.size(); ++k)
    if (a[k].name != b[k].name || a[k].type != b[k].type || a[k].of != b[k].of || a[k].ref != b[k].ref || a[k].constant != b[k].constant ||
        json::stringify(a[k].value) != json::stringify(b[k].value))
      return false;
  return true;
}
/// A library's function as a Blueprint that uses the library holds it: what it looks like.
Function declared(const std::string& library, const Function& f) {
  Function d;
  d.name = library_function(library, f.name);
  d.from = library;
  d.inputs = f.inputs;
  d.outputs = f.outputs;
  d.pure = f.pure;
  d.help = f.help;
  return d;
}
bool say(std::string* error, const std::string& why) {
  if (error) *error = why;
  return false;
}
}  // namespace

bool libraries_of(const Graph& g, const Blueprints& all, std::vector<std::string>& out, std::string* error) {
  out.clear();
  std::vector<std::string> open;  // (the libraries being followed: one of them again is a library that uses itself)
  std::function<bool(const std::vector<std::string>&)> follow = [&](const std::vector<std::string>& names) {
    for (const std::string& name : names) {
      if (std::find(out.begin(), out.end(), name) != out.end()) continue;
      if (std::find(open.begin(), open.end(), name) != open.end()) return say(error, "the library " + name + " uses itself (through the libraries it uses)");
      const Graph* l = all ? all(name) : nullptr;
      if (!l) return say(error, "the library \"" + name + "\" is not there: no Blueprint of that name");
      if (!l->library) return say(error, "\"" + name + "\" is no library: only a Blueprint that keeps nothing is used by others (My Blueprint: Library)");
      open.push_back(name);
      if (!follow(l->uses)) return false;
      open.pop_back();
      out.push_back(name);
    }
    return true;
  };
  return follow(g.uses);
}

namespace {
/// The types a Blueprint has of the libraries `libraries` (each after those it uses), in front of
/// its own: those it names - in its variables, in what its functions take, give and keep, in the
/// settings of its nodes -, and those the fields of these are of (`whole`: all of them). What is
/// declared first comes first: a library after the libraries it uses, a struct after the structs
/// its fields are of. No name is two types: of those a Blueprint holds, each is declared once.
bool take_types(Graph& g, const std::vector<std::string>& libraries, const Blueprints& all, std::string* error, bool whole, const Library& lib) {
  struct Declared { std::string library; const EnumType* e = nullptr; const StructType* t = nullptr; };
  std::vector<Declared> declared;  // (in the order they are declared in)
  for (const std::string& name : libraries) {
    const Graph& l = *all(name);
    for (const EnumType& e : l.enums) if (e.from.empty()) declared.push_back({name, &e, nullptr});
    for (const StructType& t : l.structs) if (t.from.empty()) declared.push_back({name, nullptr, &t});
  }
  auto name_of = [](const Declared& d) -> const std::string& { return d.e ? d.e->name : d.t->name; };
  // The names it names.
  std::set<std::string> named;
  if (!whole) {
    std::vector<std::string> todo;
    auto name = [&](const std::string& of) { if (!of.empty() && named.insert(of).second) todo.push_back(of); };
    for (const Variable& v : g.variables) name(v.of);
    for (const Function& f : g.functions)
      for (const auto* pins : {&f.inputs, &f.outputs, &f.locals})
        for (const FunctionPin& p : *pins) name(p.of);
    for (const Node& n : g.nodes) {
      const NodeDef* d = lib.find(n.type);
      if (!d) continue;
      for (const ParamDef& pd : d->params) {
        if (pd.kind != ParamDef::Kind::type) continue;
        std::string v = n.param(pd.key, pd.def);
        if (v.size() > 5 && v.compare(v.size() - 5, 5, "_list") == 0) v.resize(v.size() - 5);
        name(v);
      }
    }
    for (const StructType& t : g.structs)
      if (t.from.empty())
        for (const StructField& f : t.fields) name(f.type.of);
    while (!todo.empty()) {  // (and the types the fields of those are of)
      const std::string of = std::move(todo.back());
      todo.pop_back();
      for (const Declared& d : declared)
        if (d.t && d.t->name == of)
          for (const StructField& f : d.t->fields) name(f.type.of);
    }
  }
  std::vector<EnumType> enums;
  std::vector<StructType> structs;
  std::map<std::string, std::string> from;  // a type it holds -> the library that declares it
  for (const Declared& d : declared) {
    const std::string& type = name_of(d);
    if (!whole && !named.count(type)) continue;
    if (auto [at, fresh] = from.emplace(type, d.library); !fresh) {
      // (two libraries may each have a type of that name while the Blueprint names none of them;
      // where everything is held to pick from, the name is the first's - what is picked is judged after)
      if (whole) continue;
      return say(error, "the libraries " + at->second + " and " + d.library + " both declare a type \"" + type + "\"");
    }
    if (d.e) {
      enums.push_back(*d.e);
      enums.back().from = d.library;
    } else {
      structs.push_back(*d.t);
      structs.back().from = d.library;
    }
  }
  for (const EnumType& e : g.enums) {
    if (!e.from.empty()) continue;
    if (auto at = from.find(e.name); at != from.end()) return say(error, "the type \"" + e.name + "\" is declared here and in the library " + at->second);
    enums.push_back(e);
  }
  for (const StructType& t : g.structs) {
    if (!t.from.empty()) continue;
    if (auto at = from.find(t.name); at != from.end()) return say(error, "the type \"" + t.name + "\" is declared here and in the library " + at->second);
    structs.push_back(t);
  }
  g.enums = std::move(enums);
  g.structs = std::move(structs);
  return true;
}
}  // namespace

bool take_libraries(Graph& g, const Blueprints& all, std::string* error, bool whole, const Library* lib_) {
  std::vector<std::string> libraries;
  if (!libraries_of(g, all, libraries, error)) return false;
  // How the functions look that it calls of them.
  std::vector<Function> functions;
  auto take = [&](const std::string& library, const std::string& function) {
    const std::string name = library_function(library, function);
    for (const Function& f : functions) if (f.name == name) return true;
    if (!g.uses_library(library)) return say(error, name + " is called, but the Blueprint does not use the library " + library);
    const Graph* l = all(library);
    const Function* f = l ? l->function(function) : nullptr;
    if (!f || !f->from.empty() || f->external) return say(error, "the library " + library + " has no function \"" + function + "\"");
    functions.push_back(declared(library, *f));
    return true;
  };
  if (whole) {
    for (const std::string& name : g.uses)
      for (const Function& f : all(name)->functions)
        if (f.from.empty() && !f.external && !take(name, f.name)) return false;
  } else {
    std::string library, function;
    for (const Node& n : g.nodes)
      if (calls_function(n) && split_library_function(n.param("function"), library, function) && !take(library, function)) return false;
  }
  std::sort(functions.begin(), functions.end(), [](const Function& a, const Function& b) { return a.name < b.name; });
  for (Function& f : g.functions)
    if (f.from.empty()) functions.push_back(std::move(f));
  g.functions = std::move(functions);
  // Their types: those it names now - what a function it calls takes and gives among them.
  return take_types(g, libraries, all, error, whole, lib_ ? *lib_ : library());
}

bool link(const Graph& g, const Blueprints& all, Graph& out, std::string* error, const Library* lib_) {
  out = g;
  if (g.uses.empty()) return true;
  const Library& lib = lib_ ? *lib_ : library();
  // What it holds of its libraries is what they have: the rules were judged, and its C++ was
  // written, against that.
  {
    Graph fresh = g;
    if (!take_libraries(fresh, all, error, false, &lib)) return false;
    auto stale = [&](const std::string& what) { return say(error, "the Blueprint holds " + what + " as its library had it before: its libraries have changed since (compile it again)"); };
    for (const Function& f : fresh.functions) {
      if (f.from.empty()) continue;
      const Function* held = g.function(f.name);
      if (!held || held->from != f.from || held->pure != f.pure || !same_pins(held->inputs, f.inputs) || !same_pins(held->outputs, f.outputs)) return stale("the function " + f.name);
    }
    std::size_t from = 0;
    for (const EnumType& e : fresh.enums) {
      if (e.from.empty()) continue;
      ++from;
      const EnumType* held = g.enumeration(e.name);
      if (!held || held->from != e.from || !same_type(*held, e)) return stale("the type " + e.name + " of " + e.from);
    }
    for (const StructType& t : fresh.structs) {
      if (t.from.empty()) continue;
      ++from;
      const StructType* held = g.structure(t.name);
      if (!held || held->from != t.from || !same_type(*held, t)) return stale("the type " + t.name + " of " + t.from);
    }
    std::size_t held = 0;
    for (const EnumType& e : g.enums) held += e.from.empty() ? 0 : 1;
    for (const StructType& t : g.structs) held += t.from.empty() ? 0 : 1;
    if (held != from) return stale("a type");
  }
  // The functions it calls of its libraries, and those they call: with their bodies, under the
  // names the calls use. A library's nodes get ids above the Blueprint's own.
  u32 next = out.next_id;
  for (const Node& n : out.nodes) next = max_of(next, n.id + 1);
  std::vector<std::string> todo, done;
  for (const Node& n : g.nodes)
    if (calls_function(n) && n.param("function").find("::") != std::string::npos) todo.push_back(n.param("function"));
  std::map<std::string, std::map<u32, u32>> copied;  // per library: its node -> the node here
  while (!todo.empty()) {
    const std::string name = todo.back();
    todo.pop_back();
    if (std::find(done.begin(), done.end(), name) != done.end()) continue;
    done.push_back(name);
    std::string library, function;
    split_library_function(name, library, function);
    const Graph* l = all ? all(library) : nullptr;
    const Function* f = l ? l->function(function) : nullptr;
    if (!f || !f->from.empty() || f->external) return say(error, "the library " + library + " has no function \"" + function + "\"");
    Function linked = *f;
    linked.name = name;
    linked.from = library;  // (it is the library's - here with its body)
    if (Function* held = out.function(name)) *held = std::move(linked);
    else out.functions.push_back(std::move(linked));
    const Node* entry = nullptr;
    for (const Node& n : l->nodes)
      if (n.type == "func.entry" && n.param("function") == function) entry = &n;
    if (!entry) return say(error, "the function " + name + " has no Function Entry node");
    std::map<u32, u32>& ids = copied[library];
    for (u32 id : body_nodes(*l, entry->id, lib)) {
      if (ids.count(id)) continue;
      const Node* from = l->find(id);
      if (!from) continue;
      Node n = *from;
      n.id = ids[id] = next++;
      // The names it has of its own library are the names they have here: a function of the
      // library (its entry, its returns, a call of it), the function a local variable is of -
      // and a constant of the library (where the node names no local variable or input of its
      // function by that name).
      const Function* body = n.param("scope").empty() ? nullptr : l->function(n.param("scope"));
      for (auto& [key, value] : n.params) {
        if (value.empty() || value.find("::") != std::string::npos) continue;
        if (key == "scope") {
          value = library_function(library, value);
        } else if (key == "function") {
          const Function* named = l->function(value);
          if (named && !named->external) value = library_function(library, value);
        } else if (key == "variable" || (key.size() > 9 && key.compare(key.size() - 9, 9, ".variable") == 0)) {
          const Variable* constant = l->variable(value);
          if (!constant || (body && (body->local(value) || body->input(value)))) continue;
          const std::string name = library_function(library, value);
          if (!out.variable(name)) {
            Variable here = *constant;
            here.name = name;
            here.from = library;
            out.variables.push_back(std::move(here));
          }
          value = name;
        }
      }
      if (calls_function(n)) todo.push_back(n.param("function"));
      if (n.type == "func.call_on") {
        // (a function of another actor's Blueprint the library calls: what it looks like comes along)
        const Function* named = l->function(n.param("function"));
        const Function* held = named ? out.function(named->name) : nullptr;
        if (named && held && (!held->external || !same_pins(held->inputs, named->inputs) || !same_pins(held->outputs, named->outputs)))
          return say(error, "the library " + library + " calls " + named->name + " on an actor, and the Blueprint has another function of that name");
        if (named && !held) out.functions.push_back(*named);
      }
      out.nodes.push_back(std::move(n));
    }
  }
  for (const auto& [library, ids] : copied) {
    const Graph& l = *all(library);
    for (const Link& link : l.links) {
      auto from = ids.find(link.from), to = ids.find(link.to);
      if (from == ids.end() || to == ids.end()) continue;
      Link here = link;
      here.from = from->second;
      here.to = to->second;
      out.links.push_back(std::move(here));
    }
  }
  out.next_id = next;
  // The types the bodies name that came along are the graph's too: as their libraries declare them.
  std::vector<std::string> libraries;
  if (!libraries_of(g, all, libraries, error) || !take_types(out, libraries, all, error, false, lib)) return false;
  out.uses.clear();  // (one graph now: nothing is used from outside it)
  return true;
}
Function* Graph::function(std::string_view name) {
  for (Function& f : functions) if (f.name == name) return &f;
  return nullptr;
}
const FunctionPin* Function::input(std::string_view n) const {
  for (const FunctionPin& p : inputs) if (p.name == n) return &p;
  return nullptr;
}
const FunctionPin* Function::output(std::string_view n) const {
  for (const FunctionPin& p : outputs) if (p.name == n) return &p;
  return nullptr;
}
const FunctionPin* Function::local(std::string_view n) const {
  for (const FunctionPin& p : locals) if (p.name == n) return &p;
  return nullptr;
}
FunctionPin* Function::local(std::string_view n) {
  for (FunctionPin& p : locals) if (p.name == n) return &p;
  return nullptr;
}
bool Graph::operator==(const Graph& o) const { return graph_to_json(*this) == graph_to_json(o); }

json::Value graph_to_json(const Graph& g) {
  json::Value root = json::Value::object();
  root.set("kind", "kern-logic");
  root.set("version", 1);
  root.set("nextId", g.next_id);
  json::Value vars = json::Value::array();
  for (const Variable& v : g.variables) {
    json::Value o = json::Value::object();
    o.set("name", v.name);
    o.set("type", pin_type_name(v.type));
    if (!v.of.empty()) o.set("of", v.of);
    if (v.count) o.set("count", static_cast<f64>(v.count));  // (a list of a fixed length)
    o.set("value", v.value);
    if (!v.help.empty()) o.set("help", v.help);
    if (v.constant) o.set("const", true);
    if (!v.from.empty()) o.set("from", v.from);
    vars.push(std::move(o));
  }
  root.set("variables", std::move(vars));
  if (g.library) root.set("library", true);  // (only then, like the types: other graphs keep their content hash)
  if (!g.uses.empty()) {
    json::Value uses = json::Value::array();
    for (const std::string& name : g.uses) uses.push(name);
    root.set("uses", std::move(uses));
  }
  if (!g.enums.empty()) {  // (only then: graphs without types of their own keep their content hash)
    json::Value enums = json::Value::array();
    for (const EnumType& e : g.enums) {
      json::Value o = json::Value::object();
      o.set("name", e.name);
      json::Value values = json::Value::array();
      for (const std::string& v : e.values) values.push(v);
      o.set("values", std::move(values));
      if (!e.help.empty()) o.set("help", e.help);
      if (e.project) o.set("project", true);
      if (!e.from.empty()) o.set("from", e.from);
      enums.push(std::move(o));
    }
    root.set("enums", std::move(enums));
  }
  if (!g.structs.empty()) {
    json::Value structs = json::Value::array();
    for (const StructType& s : g.structs) {
      json::Value o = json::Value::object();
      o.set("name", s.name);
      json::Value fields = json::Value::array();
      for (const StructField& f : s.fields) {
        json::Value x = json::Value::object();
        x.set("name", f.name);
        x.set("type", pin_type_name(f.type.type));
        if (!f.type.of.empty()) x.set("of", f.type.of);
        if (f.type.count) x.set("count", static_cast<f64>(f.type.count));
        x.set("value", f.value);
        fields.push(std::move(x));
      }
      o.set("fields", std::move(fields));
      if (!s.help.empty()) o.set("help", s.help);
      if (s.project) o.set("project", true);
      if (!s.from.empty()) o.set("from", s.from);
      structs.push(std::move(o));
    }
    root.set("structs", std::move(structs));
  }
  json::Value nodes = json::Value::array();
  for (const Node& n : g.nodes) {
    json::Value o = json::Value::object();
    o.set("id", n.id);
    o.set("type", n.type);
    o.set("x", static_cast<f64>(std::round(n.x)));
    o.set("y", static_cast<f64>(std::round(n.y)));
    if (!n.params.empty()) {
      json::Value p = json::Value::object();
      for (const auto& [k, v] : n.params) p.set(k, v);
      o.set("params", std::move(p));
    }
    if (!n.values.empty()) {
      json::Value p = json::Value::object();
      for (const auto& [k, v] : n.values) p.set(k, v);
      o.set("values", std::move(p));
    }
    if (!n.comment.empty()) o.set("comment", n.comment);
    nodes.push(std::move(o));
  }
  root.set("nodes", std::move(nodes));
  json::Value links = json::Value::array();
  for (const Link& l : g.links) {
    json::Value o = json::Value::array();
    o.push(l.from);
    o.push(l.out);
    o.push(l.to);
    o.push(l.in);
    links.push(std::move(o));
  }
  root.set("links", std::move(links));
  json::Value machines = json::Value::array();
  for (const StateMachine& m : g.machines) {
    json::Value o = json::Value::object();
    o.set("id", m.id);
    o.set("name", m.name);
    json::Value states = json::Value::array();
    for (const State& s : m.states) {
      json::Value x = json::Value::object();
      x.set("id", s.id);
      x.set("name", s.name);
      x.set("x", static_cast<f64>(std::round(s.x)));
      x.set("y", static_cast<f64>(std::round(s.y)));
      if (s.initial) x.set("initial", true);
      states.push(std::move(x));
    }
    o.set("states", std::move(states));
    json::Value transitions = json::Value::array();
    for (const Transition& t : m.transitions) {
      json::Value x = json::Value::object();
      x.set("id", t.id);
      x.set("from", t.from);
      x.set("to", t.to);
      x.set("trigger", trigger_kind_name(t.trigger));
      if (t.trigger == TriggerKind::event) x.set("event", t.event);
      if (t.trigger == TriggerKind::timer) x.set("seconds", static_cast<f64>(t.seconds));
      if (t.trigger == TriggerKind::condition) x.set("variable", t.variable);
      if (!t.label.empty()) x.set("label", t.label);
      transitions.push(std::move(x));
    }
    o.set("transitions", std::move(transitions));
    machines.push(std::move(o));
  }
  root.set("machines", std::move(machines));
  if (!g.functions.empty()) {  // (only then: graphs without functions keep their content hash)
    json::Value functions = json::Value::array();
    auto pins = [](const std::vector<FunctionPin>& list) {
      json::Value a = json::Value::array();
      for (const FunctionPin& p : list) {
        json::Value x = json::Value::object();
        x.set("name", p.name);
        x.set("type", pin_type_name(p.type));
        if (!p.of.empty()) x.set("of", p.of);
        if (p.count) x.set("count", static_cast<f64>(p.count));
        if (p.ref) x.set("ref", p.constant ? json::Value("const") : json::Value(true));  // (a reference input: Car& car, const Car& car)
        x.set("value", p.value);
        a.push(std::move(x));
      }
      return a;
    };
    for (const Function& f : g.functions) {
      json::Value o = json::Value::object();
      o.set("name", f.name);
      if (f.pure) o.set("pure", true);
      if (f.external) o.set("external", true);
      if (!f.from.empty()) o.set("from", f.from);
      if (!f.help.empty()) o.set("help", f.help);
      o.set("inputs", pins(f.inputs));
      o.set("outputs", pins(f.outputs));
      if (!f.locals.empty()) o.set("locals", pins(f.locals));
      functions.push(std::move(o));
    }
    root.set("functions", std::move(functions));
  }
  if (!g.comments.empty()) {
    json::Value comments = json::Value::array();
    for (const CommentBox& c : g.comments) {
      json::Value x = json::Value::object();
      x.set("id", c.id);
      x.set("x", static_cast<f64>(std::round(c.x)));
      x.set("y", static_cast<f64>(std::round(c.y)));
      x.set("w", static_cast<f64>(std::round(c.w)));
      x.set("h", static_cast<f64>(std::round(c.h)));
      x.set("text", c.text);
      if (c.color) x.set("color", c.color);
      comments.push(std::move(x));
    }
    root.set("comments", std::move(comments));
  }
  return root;
}

bool graph_from_json(const json::Value& j, Graph& out, std::string* error) {
  auto bad = [&](const std::string& why) {
    if (error) *error = "logic graph: " + why;
    return false;
  };
  if (!j.is_object()) return bad("not an object");
  Graph g;
  u32 max_id = 0;
  auto id_of = [&](const json::Value& v) -> u32 {
    const f64 n = v.as_number(0);
    if (!(n >= 1 && n < 1e9)) return 0;
    const u32 id = static_cast<u32>(n);
    max_id = max_of(max_id, id);
    return id;
  };
  g.library = j["library"].as_bool();
  for (const json::Value& v : j["uses"].items()) {
    const std::string name = v.as_string();
    if (name.empty() || name.size() > 64) return bad("a library that is used needs a name (1-64 characters)");
    if (g.uses_library(name)) return bad("the library \"" + name + "\" is used twice");
    g.uses.push_back(name);
  }
  for (const json::Value& v : j["enums"].items()) {
    EnumType e;
    e.name = v["name"].as_string();
    if (e.name.empty() || e.name.size() > 64) return bad("an enum needs a name (1-64 characters)");
    if (g.enumeration(e.name)) return bad("enum \"" + e.name + "\" is declared twice");
    for (const json::Value& x : v["values"].items()) e.values.push_back(x.as_string());
    e.help = v["help"].as_string();
    e.project = v["project"].as_bool();
    e.from = v["from"].as_string();
    g.enums.push_back(std::move(e));
  }
  // A type in the file: its name, and for an enum's or a struct's values which one ("of").
  auto type_of = [&](const json::Value& v, PinType& type, std::string& of) {
    if (!pin_type_from_name(v["type"].as_string(), type) || type == PinType::exec) return false;
    if (is_keyed(type)) {  // (a map's or a set's: what its keys and values are - types the Blueprint has above here)
      of = v["of"].as_string();
      TypeRef t;
      return type_from_text(type_text({type, of}), g, t);
    }
    if (!declared_type(type)) return true;
    of = v["of"].as_string();
    return element_of(type) == PinType::enumeration ? g.enumeration(of) != nullptr : g.structure(of) != nullptr;
  };
  // (a list of a fixed length: "count" - what is no such count is none; the check says what is wrong with one)
  auto count_of = [](const json::Value& v) {
    const f64 n = v["count"].as_number(0);
    return n >= 1 && n <= 4294967295.0 && n == std::floor(n) ? static_cast<u32>(n) : u32{0};
  };
  // (a struct after the structs its fields are of: no struct contains itself)
  for (const json::Value& v : j["structs"].items()) {
    StructType s;
    s.name = v["name"].as_string();
    if (s.name.empty() || s.name.size() > 64) return bad("a struct needs a name (1-64 characters)");
    if (g.structure(s.name) || g.enumeration(s.name)) return bad("the type \"" + s.name + "\" is declared twice");
    for (const json::Value& x : v["fields"].items()) {
      StructField f;
      f.name = x["name"].as_string();
      if (f.name.empty() || f.name.size() > 64) return bad("struct \"" + s.name + "\": a field needs a name (1-64 characters)");
      if (s.field(f.name) >= 0) return bad("struct \"" + s.name + "\": the field \"" + f.name + "\" exists twice");
      if (!type_of(x, f.type.type, f.type.of)) return bad("struct \"" + s.name + "\": the field \"" + f.name + "\" has an unknown type (a struct holds structs declared above it)");
      f.type.count = count_of(x);
      f.value = Value::from_json(x["value"], f.type, g).to_json(g, f.type);
      s.fields.push_back(std::move(f));
    }
    s.help = v["help"].as_string();
    s.project = v["project"].as_bool();
    s.from = v["from"].as_string();
    g.structs.push_back(std::move(s));
  }
  for (const json::Value& v : j["variables"].items()) {
    Variable var;
    var.name = v["name"].as_string();
    if (var.name.empty() || var.name.size() > 64) return bad("a variable needs a name (1-64 characters)");
    if (g.variable(var.name)) return bad("variable \"" + var.name + "\" is declared twice");
    if (!type_of(v, var.type, var.of)) return bad("variable \"" + var.name + "\" has an unknown type");
    var.count = count_of(v);
    var.value = Value::from_json(v["value"], declared(var), g).to_json(g, declared(var));
    var.help = v["help"].as_string();
    var.constant = v["const"].as_bool();
    var.from = v["from"].as_string();
    g.variables.push_back(std::move(var));
  }
  for (const json::Value& v : j["nodes"].items()) {
    Node n;
    n.id = id_of(v["id"]);
    if (!n.id) return bad("a node has no valid id");
    if (g.find(n.id)) return bad("node id " + std::to_string(n.id) + " is used twice");
    n.type = v["type"].as_string();
    if (n.type.empty()) return bad("node " + std::to_string(n.id) + " has no type");
    n.x = static_cast<f32>(v["x"].as_number());
    n.y = static_cast<f32>(v["y"].as_number());
    if (!std::isfinite(n.x) || !std::isfinite(n.y) || std::fabs(n.x) > 1e6 || std::fabs(n.y) > 1e6) n.x = n.y = 0;
    for (const auto& [k, p] : v["params"].members()) n.params.emplace_back(k, p.is_string() ? p.as_string() : json::stringify(p));
    for (const auto& [k, p] : v["values"].members()) n.values.emplace_back(k, p);
    n.comment = v["comment"].as_string();
    g.nodes.push_back(std::move(n));
  }
  for (const json::Value& v : j["links"].items()) {
    Link l;
    if (v.is_array() && v.size() == 4) {
      l.from = static_cast<u32>(v[0].as_number());
      l.out = v[1].as_string();
      l.to = static_cast<u32>(v[2].as_number());
      l.in = v[3].as_string();
    } else {
      l.from = static_cast<u32>(v["from"].as_number());
      l.out = v["out"].as_string();
      l.to = static_cast<u32>(v["to"].as_number());
      l.in = v["in"].as_string();
    }
    if (!g.find(l.from) || !g.find(l.to)) return bad("a link refers to a missing node");
    if (std::find(g.links.begin(), g.links.end(), l) == g.links.end()) g.links.push_back(std::move(l));
  }
  // Documents from before a Set could name a place: Set Field and Set List Item were nodes of
  // their own. Each is the Set at that place - Held.Damage = value;   Scores[index] = value;
  for (Node& n : g.nodes) {
    if (n.type == "struct.set_field") {
      n.type = "var.set";
      n.set_param("path", "." + n.param("field"));
      std::erase_if(n.params, [](const auto& p) { return p.first == "field"; });
    } else if (n.type == "list.set") {
      n.type = "var.set";
      n.set_param("path", "[]");
      const std::string index = path_pin(0);
      for (auto& [pin, value] : n.values) pin = pin == "index" ? index : pin == "item" ? std::string("value") : pin;
      for (Link& l : g.links)
        if (l.to == n.id) l.in = l.in == "index" ? index : l.in == "item" ? std::string("value") : l.in;
    }
  }
  for (const json::Value& v : j["machines"].items()) {
    StateMachine m;
    m.id = id_of(v["id"]);
    if (!m.id) return bad("a state machine has no valid id");
    m.name = v["name"].as_string();
    if (m.name.empty()) m.name = "State Machine";
    for (const json::Value& s : v["states"].items()) {
      State st;
      st.id = id_of(s["id"]);
      st.name = s["name"].as_string();
      if (!st.id || st.name.empty()) return bad("a state of \"" + m.name + "\" needs an id and a name");
      if (m.find(st.name)) return bad("state \"" + st.name + "\" exists twice in \"" + m.name + "\"");
      st.x = static_cast<f32>(s["x"].as_number());
      st.y = static_cast<f32>(s["y"].as_number());
      st.initial = s["initial"].as_bool();
      m.states.push_back(std::move(st));
    }
    for (const json::Value& t : v["transitions"].items()) {
      Transition tr;
      tr.id = id_of(t["id"]);
      tr.from = static_cast<u32>(t["from"].as_number());
      tr.to = static_cast<u32>(t["to"].as_number());
      const std::string kind = t["trigger"].is_string() ? t["trigger"].as_string() : std::string("event");
      tr.trigger = kind == "timer" ? TriggerKind::timer : kind == "condition" ? TriggerKind::condition : kind == "auto" ? TriggerKind::automatic : TriggerKind::event;
      tr.event = t["event"].as_string();
      tr.seconds = static_cast<f32>(clamp(t["seconds"].as_number(1), 0.0, 1e6));
      tr.variable = t["variable"].as_string();
      tr.label = t["label"].as_string();
      if (!tr.id || !m.find(tr.to) || (tr.from && !m.find(tr.from))) return bad("a transition of \"" + m.name + "\" connects missing states");
      m.transitions.push_back(std::move(tr));
    }
    g.machines.push_back(std::move(m));
  }
  for (const json::Value& v : j["functions"].items()) {
    Function f;
    f.name = v["name"].as_string();
    f.from = v["from"].as_string();
    // (a function of a library is named with it: "Random::range")
    if (f.name.empty() || f.name.size() > 64 + (f.from.empty() ? 0 : f.from.size() + 2)) return bad("a function needs a name (1-64 characters)");
    if (g.function(f.name)) return bad("function \"" + f.name + "\" is declared twice");
    f.pure = v["pure"].as_bool();
    f.external = v["external"].as_bool();
    f.help = v["help"].as_string();
    auto pins = [&](const json::Value& list, std::vector<FunctionPin>& out, const char* what) {
      for (const json::Value& x : list.items()) {
        FunctionPin p;
        p.name = x["name"].as_string();
        if (p.name.empty() || p.name.size() > 64 || p.name == "exec" || p.name == "then") return bad(std::string("function \"") + f.name + "\": " + what + " needs a name (not exec / then)");
        for (const FunctionPin& q : out) if (q.name == p.name) return bad("function \"" + f.name + "\": " + what + " \"" + p.name + "\" exists twice");
        if (!type_of(x, p.type, p.of)) return bad("function \"" + f.name + "\": \"" + p.name + "\" has an unknown type");
        p.count = count_of(x);
        p.value = Value::from_json(x["value"], declared(p), g).to_json(g, declared(p));
        p.constant = x["ref"].is_string() && x["ref"].as_string() == "const";
        p.ref = p.constant || x["ref"].as_bool();
        out.push_back(std::move(p));
      }
      return true;
    };
    if (!pins(v["inputs"], f.inputs, "an input") || !pins(v["outputs"], f.outputs, "an output") || !pins(v["locals"], f.locals, "a local variable")) return false;
    g.functions.push_back(std::move(f));
  }
  for (const json::Value& v : j["comments"].items()) {
    CommentBox c;
    c.id = id_of(v["id"]);
    if (!c.id) return bad("a comment box has no valid id");
    auto coord = [](const json::Value& x, f32 fallback, f32 lo, f32 hi) {
      const f64 n = x.as_number(fallback);
      return std::isfinite(n) ? static_cast<f32>(clamp(n, static_cast<f64>(lo), static_cast<f64>(hi))) : fallback;
    };
    c.x = coord(v["x"], 0, -1e6f, 1e6f);
    c.y = coord(v["y"], 0, -1e6f, 1e6f);
    c.w = coord(v["w"], 400, 40, 1e5f);
    c.h = coord(v["h"], 200, 40, 1e5f);
    c.text = v["text"].is_string() ? v["text"].as_string() : std::string("Comment");
    c.color = static_cast<u32>(clamp(v["color"].as_number(0), 0.0, 16777215.0));
    g.comments.push_back(std::move(c));
  }
  g.next_id = max_of(max_id + 1, static_cast<u32>(j["nextId"].as_number(0)));
  out = std::move(g);
  return true;
}

// ---------------------------------------------------------------------------
// library

void Library::add(NodeDef def) {
  auto it = index_.find(def.type);
  if (it != index_.end()) { *defs_[it->second] = std::move(def); return; }
  index_[def.type] = defs_.size();
  defs_.push_back(std::make_unique<NodeDef>(std::move(def)));
}

const NodeDef* Library::find(std::string_view type) const {
  auto it = index_.find(std::string(type));
  return it == index_.end() ? nullptr : defs_[it->second].get();
}
NodeDef* Library::edit(std::string_view type) {
  auto it = index_.find(std::string(type));
  return it == index_.end() ? nullptr : defs_[it->second].get();
}

void Library::pins(const Node& node, const Graph& graph, std::vector<PinDef>& in, std::vector<PinDef>& out) const {
  in.clear();
  out.clear();
  const NodeDef* d = find(node.type);
  if (!d) return;
  in = d->inputs;
  out = d->outputs;
  if (d->shape) d->shape(*d, node, graph, in, out);
  // (a node that names a place: an index input for every list entry its path goes through)
  for (const ParamDef& pd : d->params) {
    if (pd.kind != ParamDef::Kind::path) continue;
    const std::size_t entries = path_indices(node.param(pd.key));
    for (std::size_t k = 0; k < entries; ++k) {
      PinDef pin;
      pin.id = path_pin(k);
      pin.label = entries > 1 ? "Index " + std::to_string(k + 1) : std::string("Index");
      pin.type = PinType::integer;
      pin.def = json::Value(0);
      in.push_back(std::move(pin));
    }
  }
}

std::string Library::title(const Node& node) const {
  const NodeDef* d = find(node.type);
  if (!d) return node.type;
  return d->title ? d->title(*d, node) : d->label;
}

Library& library() { return Library::core(); }

// ---------------------------------------------------------------------------
// checking

std::vector<Issue> check(const Graph& g, const Library& lib) {
  std::vector<Issue> out;
  auto err = [&](u32 node, std::string text) { out.push_back({node, true, std::move(text)}); };
  auto warn = [&](u32 node, std::string text) { out.push_back({node, false, std::move(text)}); };
  std::unordered_map<u32, std::pair<std::vector<PinDef>, std::vector<PinDef>>> pins;
  // A value that is held somewhere (a default, a typed-in value): where it is an enum's - itself,
  // in a list, in a struct's field - it is one of the enum's values.
  std::function<void(u32, const std::string&, const Value&, const TypeRef&, std::size_t)> held_ok = [&](u32 node, const std::string& what, const Value& v, const TypeRef& t, std::size_t depth) {
    if (is_keyed(t.type)) {  // (its keys and values, each as what it is)
      const TypeRef key = key_type(t), value = t.type == PinType::map ? value_type(t) : TypeRef{};
      for (std::size_t k = 0, n = v.size(); k < n; ++k) {
        held_ok(node, what + ", key " + std::to_string(k), key_value(v.keyed().key_at(k), key.type), key, depth);
        if (t.type == PinType::map) held_ok(node, what + ", value " + std::to_string(k), v.keyed().value_at(k), value, depth);
      }
      return;
    }
    if (element_of(t.type) == PinType::enumeration) {
      const EnumType* e = g.enumeration(t.of);
      if (!e) return;
      for (std::size_t k = 0, n = t.type == PinType::enumeration ? 1 : v.size(); k < n; ++k) {
        const i64 index = t.type == PinType::enumeration ? v.i : v.item(k).i;
        if (index >= static_cast<i64>(e->values.size())) err(node, what + ": " + std::to_string(index) + " is no value of " + e->name + " (it has " + std::to_string(e->values.size()) + ")");
      }
      return;
    }
    const StructType* s = element_of(t.type) == PinType::structure && depth < g.structs.size() ? g.structure(t.of) : nullptr;
    if (!s) return;
    for (std::size_t item = 0, items = t.type == PinType::structure ? 1 : v.size(); item < items; ++item) {
      const Value one = t.type == PinType::structure ? v : v.item(item);
      for (std::size_t k = 0; k < s->fields.size(); ++k)
        if (const Value* held = one.field(k)) held_ok(node, what + ", field " + s->fields[k].name, *held, s->fields[k].type, depth + 1);
    }
  };
  for (const Node& n : g.nodes) {
    const NodeDef* d = lib.find(n.type);
    if (!d) { err(n.id, "unknown node type \"" + n.type + "\""); continue; }
    auto& p = pins[n.id];
    lib.pins(n, g, p.first, p.second);
    for (const ParamDef& pd : d->params) {
      const std::string v = n.param(pd.key, pd.def);
      if (pd.kind == ParamDef::Kind::variable) {
        const std::string scope = n.param("scope");
        Variable named;
        if (!scope.empty() && !g.function(scope)) err(n.id, lib.title(n) + ": no function \"" + scope + "\" (whose local variable \"" + v + "\" would be)");
        else if (!g.variable_of(n, named)) err(n.id, lib.title(n) + (scope.empty() ? ": no variable \"" + v + "\"" : ": the function " + scope + " has no local variable \"" + v + "\""));
      }
      if (pd.kind == ParamDef::Kind::path && !v.empty()) {
        Variable named;
        std::string why;
        if (g.variable_of(n, named) && !g.place_of(n, named, &why)) err(n.id, lib.title(n) + ": " + why);
      }
      if (pd.kind == ParamDef::Kind::machine && !g.machine(v)) err(n.id, lib.title(n) + ": no state machine \"" + v + "\"");
      if (pd.kind == ParamDef::Kind::state) {
        const StateMachine* m = g.machine(n.param("machine"));
        if (m && !v.empty() && v != "any" && !m->find(v)) err(n.id, lib.title(n) + ": \"" + m->name + "\" has no state \"" + v + "\"");
      }
      if (pd.kind == ParamDef::Kind::event && v.empty()) err(n.id, lib.title(n) + ": the event needs a name");
      if (pd.kind == ParamDef::Kind::function && !g.function(v) && !(pd.optional && v.empty())) err(n.id, d->label + ": no function \"" + v + "\"");
      if (pd.kind == ParamDef::Kind::type) {
        // (the setting holds a type's own name: what it means does not depend on a reader's goodwill)
        bool known = false;
        for (const TypeRef& t : type_choices(pd, g)) known = known || v == type_text(t);
        if (!known && pd.enums_only) err(n.id, lib.title(n) + ": \"" + v + "\" is no enum of this Blueprint");
        else if (!known && pd.structs_only) err(n.id, lib.title(n) + ": \"" + v + "\" is no struct of this Blueprint");
        else if (!known)
          err(n.id, lib.title(n) + ": \"" + v + "\" is no type it can have (" + pd.label + ": float, int, bool, vector, text, actor, an enum or a struct of this Blueprint" +
                        (pd.with_lists ? ", or a list of one - float_list ...)" : ")"));
      }
    }
    // a value of an enum typed into a pin is one of the enum's values - also where a struct holds it
    for (const PinDef& in : p.first) {
      const json::Value* typed = n.value(in.id);
      if (!typed) continue;
      if (in.type == PinType::enumeration) {
        const EnumType* e = g.enumeration(in.of);
        if (e && typed->is_number() && typed->as_number() >= static_cast<f64>(e->values.size()))
          err(n.id, lib.title(n) + ": \"" + in.id + "\" is value " + std::to_string(whole(typed->as_number())) + " of " + e->name + ", which has " + std::to_string(e->values.size()));
      } else if (element_of(in.type) == PinType::structure && g.structure(in.of)) {
        held_ok(n.id, lib.title(n) + ": \"" + in.id + "\"", Value::from_json(*typed, in.type, g, in.of), {in.type, in.of}, 0);
      }
    }
    if (d->problem)
      if (const std::string why = d->problem(n, g); !why.empty()) err(n.id, d->label + ": " + why);
  }
  // The enums: each has values, and no two of them the same name.
  for (const EnumType& e : g.enums) {
    if (e.values.empty()) err(0, "enum \"" + e.name + "\" has no values");
    for (std::size_t a = 0; a < e.values.size(); ++a) {
      if (e.values[a].empty()) err(0, "enum \"" + e.name + "\": value " + std::to_string(a) + " has no name");
      for (std::size_t b = 0; b < a; ++b)
        if (e.values[a] == e.values[b]) err(0, "enum \"" + e.name + "\" has two values called \"" + e.values[a] + "\"");
    }
  }
  // The structs: each has fields (96 at most), no two of them the same name; a field is of a
  // type there is - an enum, or a struct declared above its own (no struct contains itself).
  for (std::size_t at = 0; at < g.structs.size(); ++at) {
    const StructType& s = g.structs[at];
    if (s.fields.empty()) err(0, "struct \"" + s.name + "\" has no fields");
    if (s.fields.size() > StructType::kMaxFields) err(0, "struct \"" + s.name + "\" has " + std::to_string(s.fields.size()) + " fields (a struct has " + std::to_string(StructType::kMaxFields) + " at most)");
    bool sound = true;
    for (std::size_t a = 0; a < s.fields.size(); ++a) {
      const StructField& f = s.fields[a];
      const std::string what = "struct \"" + s.name + "\": the field \"" + f.name + "\"";
      if (f.name.empty()) err(0, "struct \"" + s.name + "\": field " + std::to_string(a) + " has no name");
      for (std::size_t b = 0; b < a; ++b)
        if (f.name == s.fields[b].name) err(0, "struct \"" + s.name + "\" has two fields called \"" + f.name + "\"");
      if (f.type.type == PinType::exec || f.type.type >= PinType::count) { err(0, what + " has no type"); sound = false; }
      TypeRef named = f.type;  // (what names a type of the Blueprint: of a map its values, of a set its keys)
      if (is_keyed(f.type.type)) {
        TypeRef sure;
        if (!type_from_text(type_text(f.type), g, sure)) { err(0, what + " is " + type_text(f.type) + ", which this Blueprint cannot have"); sound = false; continue; }
        named = f.type.type == PinType::map ? value_type(f.type) : key_type(f.type);
      }
      if (element_of(named.type) == PinType::enumeration && !g.enumeration(named.of)) { err(0, what + " is of the enum \"" + named.of + "\", which this Blueprint does not have"); sound = false; }
      if (element_of(named.type) != PinType::structure) continue;
      std::size_t inner = 0;
      while (inner < g.structs.size() && g.structs[inner].name != named.of) ++inner;
      if (inner == at) err(0, "struct \"" + s.name + "\" contains itself (the field \"" + f.name + "\")");
      else if (inner == g.structs.size()) err(0, what + " is of the struct \"" + named.of + "\", which this Blueprint does not have");
      else if (inner > at) err(0, what + " is of the struct \"" + named.of + "\", which is declared below it (a struct holds structs declared above it)");
      sound = sound && inner < at;
    }
    if (!sound) continue;
    for (const StructField& f : s.fields) held_ok(0, "struct \"" + s.name + "\": the field \"" + f.name + "\"", Value::from_json(f.value, f.type.type, g, f.type.of), f.type, 0);
  }
  // What is of an enum's or a struct's type names one the Blueprint has, and its default holds
  // values of the enums only.
  auto typed_ok = [&](const std::string& what, PinType type, const std::string& of, const json::Value& value) {
    if (is_keyed(type)) {
      TypeRef sure;
      if (!type_from_text(type_text({type, of}), g, sure)) { err(0, what + " is " + type_text({type, of}) + ", which this Blueprint cannot have"); return; }
      held_ok(0, what, Value::from_json(value, type, g, of), {type, of}, 0);
      return;
    }
    if (element_of(type) == PinType::structure) {
      if (!g.structure(of)) { err(0, what + " is of the struct \"" + of + "\", which this Blueprint does not have"); return; }
      held_ok(0, what, Value::from_json(value, type, g, of), {type, of}, 0);
      return;
    }
    if (element_of(type) != PinType::enumeration) return;
    if (!g.enumeration(of)) { err(0, what + " is of the enum \"" + of + "\", which this Blueprint does not have"); return; }
    held_ok(0, what, Value::from_json(value, type), {type, of}, 0);
  };
  for (const Variable& v : g.variables) typed_ok("variable \"" + v.name + "\"", v.type, v.of, v.value);
  for (const Function& f : g.functions)
    for (const auto* pins_of : {&f.inputs, &f.outputs, &f.locals})
      for (const FunctionPin& fp : *pins_of) typed_ok("function \"" + f.name + "\": \"" + fp.name + "\"", fp.type, fp.of, fp.value);
  // A fixed length is a list's: 1 to kLongestFixed entries, and its default has that many.
  auto length_ok = [&](const std::string& what, const TypeRef& t, const json::Value& value) {
    if (!t.count) return;
    if (!is_list(t.type)) { err(0, what + " has a fixed length (" + std::to_string(t.count) + "), but is " + type_text({t.type, t.of}) + ": a length is a list's"); return; }
    if (t.count > kLongestFixed) { err(0, what + " is a list of " + std::to_string(t.count) + " entries: a list of a fixed length has " + std::to_string(kLongestFixed) + " at most"); return; }
    if (value.is_array() && value.size() > t.count)
      err(0, what + " is a list of " + std::to_string(t.count) + " entries, its default has " + std::to_string(value.size()));
  };
  for (const StructType& st : g.structs)
    for (const StructField& f : st.fields) length_ok("struct \"" + st.name + "\": the field \"" + f.name + "\"", f.type, f.value);
  for (const Variable& v : g.variables) length_ok("variable \"" + v.name + "\"", declared(v), v.value);
  for (const Function& f : g.functions)
    for (const auto* pins_of : {&f.inputs, &f.outputs, &f.locals})
      for (const FunctionPin& fp : *pins_of) length_ok("function \"" + f.name + "\": \"" + fp.name + "\"", declared(fp), fp.value);
  // A function's body starts at its one entry node.
  for (const Function& f : g.functions) {
    int entries = 0, returns = 0;
    for (const Node& n : g.nodes) {
      if (n.param("function") != f.name) continue;
      entries += n.type == "func.entry" ? 1 : 0;
      returns += n.type == "func.return" ? 1 : 0;
    }
    if (f.external) {
      // A function of another Blueprint has no body here: it is called on an actor.
      for (const Node& n : g.nodes)
        if (n.param("function") == f.name && (n.type == "func.entry" || n.type == "func.return" || n.type == "func.call" || n.type == "func.pure"))
          err(n.id, "\"" + f.name + "\" is a function of another Blueprint: it has no body here and is called with Call on Actor");
      if (f.pure) err(0, "function \"" + f.name + "\" is a function of another Blueprint: what it does is not known here, so it is called as a step (not Pure)");
      if (!f.locals.empty()) err(0, "function \"" + f.name + "\" is a function of another Blueprint: it has no local variables here");
      continue;
    }
    if (!f.from.empty()) {
      // A function of a library: called directly. Where the Blueprint uses the library it holds
      // what the function looks like - the library has its body; in the one graph a Blueprint is
      // with its libraries (logic::link) the body is there.
      for (const Node& n : g.nodes)
        if (n.type == "func.call_on" && n.param("function") == f.name) err(n.id, f.name + " is a function of the library " + f.from + ": it is called directly (Call Function), not on an actor");
      if (entries == 0) {
        if (!g.uses_library(f.from)) err(0, f.name + " is a function of the library " + f.from + ", which the Blueprint does not use");
        if (returns != 0) err(0, f.name + " is a function of the library " + f.from + ": its body is there, not here");
        continue;
      }
    }
    if (entries == 0) err(0, "function \"" + f.name + "\" has no Function Entry node");
    if (entries > 1) err(0, "function \"" + f.name + "\" has " + std::to_string(entries) + " Function Entry nodes (one is its start)");
    if (entries == 1 && returns == 0 && !f.outputs.empty()) warn(0, "function \"" + f.name + "\" has no Return node: it gives its outputs' defaults");
  }
  // A library keeps nothing: what it has are types and functions, and no node of it keeps
  // something between two calls. So a Blueprint that uses it has nothing of it but its functions.
  if (g.library) {
    for (const Variable& v : g.variables)
      if (!v.constant) {
        err(0, "a library keeps nothing: it has no variables (\"" + v.name + "\") but constants - what a function works on, its call gives it (a reference input)");
        break;
      }
    if (!g.machines.empty()) err(0, "a library keeps nothing: it has no state machines");
    for (const Node& n : g.nodes) {
      const NodeDef* d = lib.find(n.type);
      if (!d) continue;
      if (d->kind == NodeKind::event && n.type != "func.entry") err(n.id, "a library keeps nothing and waits for nothing: it has no events (" + lib.title(n) + ")");
      else if (d->latent) err(n.id, "a library keeps nothing: " + lib.title(n) + " waits");
      else if (d->memory && !d->loop) err(n.id, "a library keeps nothing: " + lib.title(n) + " keeps something between two calls");
    }
  }
  for (std::size_t k = 0; k < g.uses.size(); ++k)
    for (std::size_t o = 0; o < k; ++o)
      if (g.uses[o] == g.uses[k]) err(0, "the library \"" + g.uses[k] + "\" is used twice");
  // Reference inputs (FunctionPin::ref): a call names a place for each, and the function works
  // on it where it stands.
  for (const Function& f : g.functions)
    if (const FunctionPin* early = input_before_reference(f)) err(0, input_order_problem(f, *early));
  for (const Function& f : g.functions)
    for (const FunctionPin& in : f.inputs) {
      if (!in.ref) continue;
      const std::string shown = "function \"" + f.name + "\": the input \"" + in.name + "\"";
      if (f.external) err(0, shown + " cannot be a reference - a call on another actor's Blueprint gives values, no places");
      else if (f.pure && !in.constant) err(0, shown + " is a reference the function could change, but a function that is read as a value changes nothing: a const reference reads a place where it stands");
    }
  for (const Node& n : g.nodes) {
    const NodeDef* d = lib.find(n.type);
    if (!d) continue;
    bool names = false;
    for (const ParamDef& pd : d->params) names = names || pd.kind == ParamDef::Kind::variable;
    Variable v;
    if (names) {
      // (an input of the function, named like a variable: by value and by const reference it is read)
      const Graph::Root root = g.root_of(n, v);
      const bool reads = n.type == "var.get";
      if (!reads && (root == Graph::Root::input || root == Graph::Root::constant_reference))
        err(n.id, lib.title(n) + ": " + v.name + " is " + (root == Graph::Root::input ? "an input by value" : "a const reference") + " of " + n.param("scope") +
                      " - it is read, not changed (an input that is changed where the caller has it is a reference: " + type_text({v.type, v.of}) + "& " + v.name + ")");
      if (!reads && root == Graph::Root::constant) err(n.id, lib.title(n) + ": " + v.name + " is a constant - it is read, not changed");
      if (reads && root == Graph::Root::input && n.param("path").empty())
        err(n.id, lib.title(n) + ": an input by value is read from the Function Entry; a Get names a place in it (" + v.name + ".Field, " + v.name + "[index])");
    }
    if (n.type != "func.call" && n.type != "func.pure" && n.type != "func.call_on") continue;
    const Function* f = g.function(n.param("function"));
    if (!f) continue;
    for (const FunctionPin& in : f->inputs) {
      if (!in.ref) continue;
      const std::string prefix = ref_key(in.name, ""), what = lib.title(n) + ": the reference input \"" + in.name + "\"";
      std::string why;
      if (n.type == "func.call_on") {
        err(n.id, what + " needs a place, and a call on another actor's Blueprint gives values");
      } else if (n.param(prefix + "variable").empty()) {
        err(n.id, what + " needs a place: a variable, or a field or a list entry in one (" + n.param("function") + "(Variable.Field, ...))");
      } else if (!n.param("scope").empty() && !g.function(n.param("scope"))) {
        err(n.id, what + ": no function \"" + n.param("scope") + "\" (in which the call would stand)");
      } else if (!g.place_of(n, v, &why, prefix)) {
        err(n.id, what + ": " + why);
      } else if (v.type != in.type || v.of != in.of || v.count != in.count) {
        err(n.id, what + " is " + type_text(declared(in)) + ", but " + v.name + " is " + type_text(declared(v)) + " - a reference is the place itself, nothing converts");
      } else if (Variable root; !in.constant && !Graph::writable(g.root_of(n, root, prefix))) {
        err(n.id, what + " is changed by the function, but " + root.name + " is only read here (a constant, an input by value or a const reference)");
      }
    }
  }
  auto find_pin = [&](u32 node, const std::string& id, bool input) -> const PinDef* {
    auto it = pins.find(node);
    if (it == pins.end()) return nullptr;
    for (const PinDef& p : input ? it->second.first : it->second.second) if (p.id == id) return &p;
    return nullptr;
  };
  std::unordered_map<std::string, int> data_in_links;
  for (const Link& l : g.links) {
    const PinDef* o = find_pin(l.from, l.out, false);
    const PinDef* i = find_pin(l.to, l.in, true);
    if (!o || !i) {
      if (pins.count(l.from) && pins.count(l.to)) err(l.to, "a wire connects a pin that does not exist (" + l.out + " -> " + l.in + ")");
      continue;
    }
    if ((o->type == PinType::exec) != (i->type == PinType::exec)) { err(l.to, "an exec wire is connected to a data pin"); continue; }
    if (!fits({o->type, o->of}, {i->type, i->of}))
      err(l.to, "cannot convert " + type_text({o->type, o->of}) + " to " + type_text({i->type, i->of}) + " (" + i->label + ")");
    if (i->type != PinType::exec && ++data_in_links[std::to_string(l.to) + ":" + l.in] > 1) err(l.to, "input \"" + i->label + "\" has more than one wire");
  }
  // Pure nodes must not depend on themselves.
  std::unordered_map<u32, int> mark;
  std::function<bool(u32)> cyclic = [&](u32 id) -> bool {
    int& m = mark[id];
    if (m == 1) return true;
    if (m == 2) return false;
    m = 1;
    const Node* n = g.find(id);
    const NodeDef* d = n ? lib.find(n->type) : nullptr;
    if (d && d->kind == NodeKind::pure)
      for (const Link& l : g.links)
        if (l.to == id) {
          const Node* src = g.find(l.from);
          const NodeDef* sd = src ? lib.find(src->type) : nullptr;
          if (sd && sd->kind == NodeKind::pure && cyclic(l.from)) return true;
        }
    m = 2;
    return false;
  };
  for (const Node& n : g.nodes) {
    const NodeDef* d = lib.find(n.type);
    if (d && d->kind == NodeKind::pure && cyclic(n.id)) { err(n.id, lib.title(n) + " depends on itself"); mark.clear(); }
  }
  // Events with nothing attached, actions nobody reaches.
  for (const Node& n : g.nodes) {
    const NodeDef* d = lib.find(n.type);
    if (!d) continue;
    bool exec_in = false, exec_linked = false;
    for (const PinDef& p : pins[n.id].first)
      if (p.type == PinType::exec) {
        exec_in = true;
        for (const Link& l : g.links) if (l.to == n.id && l.in == p.id) exec_linked = true;
      }
    if (exec_in && !exec_linked && d->kind != NodeKind::pure) warn(n.id, lib.title(n) + " never runs (nothing is wired to its exec input)");
  }
  for (const StateMachine& m : g.machines) {
    int initial = 0;
    for (const State& s : m.states) initial += s.initial ? 1 : 0;
    if (!m.states.empty() && initial != 1) err(0, "state machine \"" + m.name + "\" needs exactly one initial state");
    for (const Transition& t : m.transitions) {
      if (t.trigger == TriggerKind::event && t.event.empty()) err(0, "a transition of \"" + m.name + "\" needs an event name");
      if (t.trigger == TriggerKind::condition) {
        const Variable* v = g.variable(t.variable);
        if (!v) err(0, "a transition of \"" + m.name + "\" waits for the missing variable \"" + t.variable + "\"");
      }
    }
  }
  // A Blueprint is its C++ as well: what that cannot say (names C++ cannot write, a Return off its
  // function's path, a random value read twice ...) is an error of the graph.
  bool errors = false;
  for (const Issue& i : out) errors = errors || i.error;
  if (!errors) {
    u32 node = 0;
    if (const std::string why = cpp_problem(g, &node, lib); !why.empty()) err(node, why);
  }
  return out;
}

// ---------------------------------------------------------------------------
// instances

namespace {
std::vector<Instance*>& instance_registry() {
  static std::vector<Instance*> r{nullptr};  // ids start at 1
  return r;
}
bool numeric(PinType t) { return t == PinType::boolean || t == PinType::integer || t == PinType::number; }


f64 to_type(f64 v, PinType t) {
  if (t == PinType::boolean) return v != 0 ? 1.0 : 0.0;
  if (t == PinType::integer) return static_cast<f64>(whole(v));  // (a whole number as Int holds it: logic::normal)
  return v;
}
Value from_slot(f64 v, PinType t) {
  if (t == PinType::boolean) return Value::boolean(v != 0);
  if (t == PinType::integer) return Value::integer(whole(v));
  return Value::number(v);
}
}  // namespace

std::unordered_map<u64, NativeBlueprintFactory>& blueprint_registry() {
  static std::unordered_map<u64, NativeBlueprintFactory> r;
  return r;
}
void register_native_blueprint(u64 hash, NativeBlueprintFactory make) { blueprint_registry()[hash] = make; }

u64 graph_hash(const Graph& g) {
  // FNV-1a of the graph without positions and comments (moving a node keeps the code valid).
  Graph copy = g;
  for (Node& n : copy.nodes) { n.x = n.y = 0; n.comment.clear(); }
  copy.comments.clear();
  for (StateMachine& m : copy.machines) for (State& st : m.states) st.x = st.y = 0;
  const std::string text = json::stringify(graph_to_json(copy));
  u64 h = 1469598103934665603ull;
  for (unsigned char ch : text) { h ^= ch; h *= 1099511628211ull; }
  return h;
}

Instance::~Instance() {
  auto& reg = instance_registry();
  if (id_ > 0 && static_cast<std::size_t>(id_) < reg.size()) reg[static_cast<std::size_t>(id_)] = nullptr;
  if (jit_fn_ >= 0) rt_.host().jit_release(jit_fn_);
}

Instance* Runtime::instance_by_id(i32 id) {
  auto& reg = instance_registry();
  return id > 0 && static_cast<std::size_t>(id) < reg.size() ? reg[static_cast<std::size_t>(id)] : nullptr;
}

void Runtime::jit_ready(i32 id, i32 function) {
  Instance* inst = instance_by_id(id);
  if (!inst) return;
  inst->jit_pending_ = false;
  if (function >= 0) inst->jit_fn_ = function;
}

i32* Instance::counters() { return rt_.counters_; }

void Instance::trace_node(int node) {
  if (node < 0 || static_cast<std::size_t>(node) >= nodes_.size() || !rt_.tracing || rt_.trace_.size() >= 4096) return;
  rt_.trace_.push_back({name_, nodes_[static_cast<std::size_t>(node)].node->id, rt_.host().time()});
}

int Instance::var_slot(std::string_view name) const {
  auto it = var_slots_.find(std::string(name));
  return it == var_slots_.end() ? -1 : it->second;
}
namespace {
/// Where an instance holds the variable a node names.
std::string key_of(const Node& n) {
  const std::string scope = n.param("scope");
  return scope.empty() ? n.param("variable") : scope + "." + n.param("variable");
}
}  // namespace
int Instance::var_slot(const Node& n) const { return var_slot(key_of(n)); }

int Instance::out_slot(int node, int out) const {
  const CNode& c = nodes_[static_cast<std::size_t>(node)];
  if (!c.def || !inline_node(node)) return -1;
  // Only inline exec nodes keep outputs in slots (events and pure nodes do not).
  const std::string& t = c.def->type;
  if (t != "flow.for" && t != "flow.do_n" && t != "flow.flip_flop" && t != "var.set" && t != "var.add") return -1;
  int ordinal = 0;
  for (int i = 0; i < static_cast<int>(c.out.size()); ++i) {
    if (c.out[static_cast<std::size_t>(i)].type == PinType::exec) continue;
    if (i == out) return ordinal < 4 ? node_slot(node) + 4 + ordinal : -1;
    ++ordinal;
  }
  return -1;
}

std::pair<int, int> Instance::exec_target(int node, int ordinal) const {
  const CNode& c = nodes_[static_cast<std::size_t>(node)];
  if (ordinal < 0 || static_cast<std::size_t>(ordinal) >= c.exec_out.size()) return {-1, 0};
  return {c.exec_out[static_cast<std::size_t>(ordinal)], c.exec_out_entry[static_cast<std::size_t>(ordinal)]};
}

u32 Instance::seed() {
  if (!seeded_) {
    seeded_ = true;
    seed_ = rt_.host().random();
  }
  return seed_;
}

bool Instance::inline_pure(int node) const {
  const CNode& c = nodes_[static_cast<std::size_t>(node)];
  if (!c.def || c.def->kind != NodeKind::pure || c.fresh) return false;
  static const char* kInline[] = {"math.add", "math.sub", "math.mul", "math.div", "math.mod", "math.min", "math.max", "math.clamp", "math.abs", "math.negate",
                                  "math.lerp", "math.sqrt", "math.floor", "math.round", "math.compare", "logic.and", "logic.or", "logic.xor", "logic.not",
                                  "value.float", "value.int", "value.bool"};
  for (const char* t : kInline) if (c.def->type == t) return true;
  if (c.def->type == "logic.select") return numeric(c.out[0].type);
  if (c.def->type == "var.get") return var_slot(*c.node) >= 0 && c.node->param("path").empty();
  return false;
}

bool Instance::inline_node(int node) const {
  const CNode& c = nodes_[static_cast<std::size_t>(node)];
  if (!c.def) return false;
  if (c.def->kind == NodeKind::pure) return inline_pure(node);
  if (c.def->kind == NodeKind::event) return !c.def->run;  // plain events just continue
  if (c.fed_fresh) return false;  // (its node function reads every input exactly once)
  const std::string& t = c.def->type;
  if (t == "flow.branch" || t == "flow.sequence" || t == "flow.for" || t == "flow.while" || t == "flow.do_once" || t == "flow.do_n" || t == "flow.gate" ||
      t == "flow.flip_flop")
    return true;
  if (t == "var.set" || t == "var.add") return var_slot(*c.node) >= 0 && c.node->param("path").empty();
  return false;
}

int Instance::call_node(int node, int entry) {
  if (node < 0 || static_cast<std::size_t>(node) >= nodes_.size() || disabled_) return kStop;
  CNode& c = nodes_[static_cast<std::size_t>(node)];
  if (!c.def) return kStop;
  ++rt_.executed_;
  Context ctx(rt_, *this, node, entry);
  ctx.compiled_ = true;
  const int r = c.def->run ? c.def->run(ctx) : 0;
  if (rt_.tracing && rt_.trace_.size() < 4096) rt_.trace_.push_back({name_, c.node->id, rt_.host().time()});
  // Continuations (Sequence-like C++ nodes, loops) go onto the generated code's stack, last first.
  for (auto it = ctx.continuations_.rbegin(); it != ctx.continuations_.rend(); ++it) {
    if (stack_[0] >= stack_capacity_) break;
    stack_[static_cast<std::size_t>(1 + 2 * stack_[0])] = it->first;
    stack_[static_cast<std::size_t>(2 + 2 * stack_[0])] = it->second;
    ++stack_[0];
  }
  return r;
}

f64 Instance::read_pin(int node, int pin) {
  if (node < 0 || static_cast<std::size_t>(node) >= nodes_.size()) return 0;
  Context ctx(rt_, *this, node, 0);
  ctx.compiled_ = true;
  return ctx.in(static_cast<std::size_t>(pin)).as_number();
}

void Instance::enter_compiled() {}

Instance::Instance(Runtime& rt, const Graph& graph, ActorRef self, std::string name) : rt_(rt), graph_(graph), self_(self), name_(std::move(name)) {
  const Library& lib = rt.lib();
  auto& reg = instance_registry();
  id_ = static_cast<i32>(reg.size());
  reg.push_back(this);
  // Slots: numeric variables first, then 8 per node. Sizes never change afterwards.
  int numeric_vars = 0;
  for (const Variable& v : graph_.variables) numeric_vars += numeric(v.type) ? 1 : 0;
  for (const Function& f : graph_.functions)
    for (const FunctionPin& l : f.locals) numeric_vars += numeric(l.type) ? 1 : 0;
  node_slot_base_ = numeric_vars;
  slots_.assign(static_cast<std::size_t>(numeric_vars) + graph_.nodes.size() * CodeLayout::kNodeSlots, 0.0);
  stack_.assign(1 + 2 * CodeLayout::kStack, 0);
  {
    int k = 0;
    for (const Variable& v : graph_.variables) {
      const Value init = Value::from_json(v.value, declared(v), graph_);
      var_types_[v.name] = declared(v);
      if (numeric(v.type)) {
        var_slots_[v.name] = k;
        slots_[static_cast<std::size_t>(k++)] = to_type(init.as_number(), v.type);
      } else {
        vars_[v.name] = init;
      }
    }
    // A function's local variables are held like the others, under "<function>.<name>" (no
    // variable has such a name): numbers in slots, where generated code reads them.
    locals_.resize(graph_.functions.size());
    for (std::size_t f = 0; f < graph_.functions.size(); ++f)
      for (const FunctionPin& l : graph_.functions[f].locals) {
        const std::string key = graph_.functions[f].name + "." + l.name;
        const Value init = Value::from_json(l.value, declared(l), graph_);
        var_types_[key] = declared(l);
        if (numeric(l.type)) {
          var_slots_[key] = k;
          slots_[static_cast<std::size_t>(k++)] = to_type(init.as_number(), l.type);
        } else {
          vars_[key] = init;
        }
        locals_[f].push_back({key, init});
      }
    // A reference input has a variable of its own too, under the same kind of key: a call from
    // outside (Instance::call) gives a value, not a place - the input is then this variable.
    refs_.resize(graph_.functions.size());
    for (const Function& f : graph_.functions)
      for (const FunctionPin& in : f.inputs) {
        if (!in.ref) continue;
        const std::string key = f.name + "." + in.name;
        var_types_[key] = {in.type, in.of};
        vars_[key] = zero_of({in.type, in.of}, graph_);
      }
  }
  nodes_.resize(graph_.nodes.size());
  for (std::size_t i = 0; i < graph_.nodes.size(); ++i) {
    CNode& c = nodes_[i];
    c.node = &graph_.nodes[i];
    c.def = lib.find(c.node->type);
    index_[c.node->id] = static_cast<int>(i);
    if (!c.def) continue;
    lib.pins(*c.node, graph_, c.in, c.out);
    c.outputs.resize(c.out.size());
    for (std::size_t k = 0; k < c.out.size(); ++k) c.outputs[k] = zero_of({c.out[k].type, c.out[k].of}, graph_);
    c.data_in.resize(c.in.size());
    c.literal.resize(c.in.size());
    c.exec_in_index.assign(c.in.size(), -1);
    int ordinal = 0;
    for (std::size_t k = 0; k < c.in.size(); ++k) {
      const PinDef& p = c.in[k];
      if (p.type == PinType::exec) { c.exec_in_index[k] = ordinal++; continue; }
      const json::Value* lit = c.node->value(p.id);
      c.literal[k] = Value::from_json(lit ? *lit : p.def, p.type, graph_, p.of);
    }
  }
  // Wires.
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    CNode& c = nodes_[i];
    if (!c.def) continue;
    int exec_outs = 0;
    for (const PinDef& p : c.out) exec_outs += p.type == PinType::exec ? 1 : 0;
    c.exec_out.assign(static_cast<std::size_t>(exec_outs), -1);
    c.exec_out_entry.assign(static_cast<std::size_t>(exec_outs), 0);
  }
  for (const Link& l : graph_.links) {
    auto fi = index_.find(l.from), ti = index_.find(l.to);
    if (fi == index_.end() || ti == index_.end()) continue;
    CNode& from = nodes_[static_cast<std::size_t>(fi->second)];
    CNode& to = nodes_[static_cast<std::size_t>(ti->second)];
    if (!from.def || !to.def) continue;
    int out_index = -1, exec_ordinal = -1, ord = 0;
    for (std::size_t k = 0; k < from.out.size(); ++k) {
      if (from.out[k].id == l.out) { out_index = static_cast<int>(k); if (from.out[k].type == PinType::exec) exec_ordinal = ord; break; }
      if (from.out[k].type == PinType::exec) ++ord;
    }
    int in_index = -1;
    for (std::size_t k = 0; k < to.in.size(); ++k) if (to.in[k].id == l.in) { in_index = static_cast<int>(k); break; }
    if (out_index < 0 || in_index < 0) continue;
    if (exec_ordinal >= 0) {
      if (to.in[static_cast<std::size_t>(in_index)].type != PinType::exec) continue;
      from.exec_out[static_cast<std::size_t>(exec_ordinal)] = ti->second;
      from.exec_out_entry[static_cast<std::size_t>(exec_ordinal)] = to.exec_in_index[static_cast<std::size_t>(in_index)];
    } else if (to.in[static_cast<std::size_t>(in_index)].type != PinType::exec &&
               fits({from.out[static_cast<std::size_t>(out_index)].type, from.out[static_cast<std::size_t>(out_index)].of},
                    {to.in[static_cast<std::size_t>(in_index)].type, to.in[static_cast<std::size_t>(in_index)].of})) {
      to.data_in[static_cast<std::size_t>(in_index)] = {fi->second, out_index};
    }
  }
  for (const StateMachine& m : graph_.machines) machines_.push_back({&m, 0, 0});
  // Exec targets as a flat table generated code reads after calling a C++ node.
  exec_table_offset_.assign(nodes_.size(), 0);
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    exec_table_offset_[i] = static_cast<int>(exec_table_.size());
    for (std::size_t k = 0; k < nodes_[i].exec_out.size(); ++k) {
      exec_table_.push_back(nodes_[i].exec_out[k]);
      exec_table_.push_back(nodes_[i].exec_out_entry[k]);
    }
  }
  exec_table_.push_back(-1);  // never empty (a stable address)
  for (std::size_t i = 0; i < nodes_.size(); ++i)
    if (nodes_[i].def && nodes_[i].def->kind == NodeKind::event) events_.push_back(static_cast<int>(i));
  std::sort(events_.begin(), events_.end(), [&](int a, int b) { return nodes_[static_cast<std::size_t>(a)].node->id < nodes_[static_cast<std::size_t>(b)].node->id; });
  // Functions: where each one's body starts.
  // Values that are new on every read (Random) and what reads them: generated code leaves these
  // nodes to their node functions, which read every input exactly once.
  {
    std::vector<u8> known(nodes_.size(), 0);
    std::function<bool(int)> is_fresh = [&](int i) -> bool {
      CNode& c = nodes_[static_cast<std::size_t>(i)];
      if (known[static_cast<std::size_t>(i)]) return c.fresh;
      known[static_cast<std::size_t>(i)] = 1;  // (a cycle of values is an error of the graph: it ends here)
      if (!c.def || c.def->kind != NodeKind::pure) return false;
      bool fresh = c.def->memory;
      for (const CNode::Source& s : c.data_in)
        if (s.node >= 0 && is_fresh(s.node)) fresh = true;
      return c.fresh = fresh;
    };
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
      is_fresh(static_cast<int>(i));
      for (const CNode::Source& s : nodes_[i].data_in)
        if (s.node >= 0 && is_fresh(s.node)) nodes_[i].fed_fresh = true;
    }
  }
  // The generated code's continuation stack, as large as this graph can ever need: a node that
  // leaves something to go on with (a Sequence its further outputs, a loop its next pass) does so
  // at most once in each chain that is running, and chains nest 64 events deep at most (with the
  // outermost and a call from outside: 66).
  {
    i64 open = 0;
    for (const CNode& c : nodes_)
      if (c.def && (c.def->kind == NodeKind::flow || c.def->loop)) open += static_cast<i64>(max_of<std::size_t>(c.exec_out.size(), 1));
    const i64 needed = 66 * open;
    jit_fits_ = needed <= CodeLayout::kStackMost;
    stack_capacity_ = static_cast<int>(clamp<i64>(needed, CodeLayout::kStack, CodeLayout::kStackMost));
    stack_.assign(static_cast<std::size_t>(1 + 2 * stack_capacity_), 0);
  }
  fn_entry_.assign(graph_.functions.size(), -1);
  for (std::size_t f = 0; f < graph_.functions.size(); ++f) {
    fn_index_[graph_.functions[f].name] = static_cast<int>(f);
    for (std::size_t i = 0; i < nodes_.size(); ++i)
      if (nodes_[i].def && nodes_[i].def->type == "func.entry" && nodes_[i].node->param("function") == graph_.functions[f].name) { fn_entry_[f] = static_cast<int>(i); break; }
  }
}

bool Instance::call(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results) { return call_function(function, args, results, false); }
bool Instance::call_inside(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results) { return call_function(function, args, results, true); }

bool Instance::call_inside(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results, const std::vector<Spot>& places) {
  return call_function(function, args, results, true, &places);
}
bool Instance::call_function(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results, bool inside, const std::vector<Spot>* places) {
  results.clear();
  if (blueprint_) {
    if (disabled_) return false;
    if (rt_.depth_ == 0) rt_.step_starts();  // (a call from outside any chain is a step of its own for the loop guard)
    rt_.counters_[1] = static_cast<i32>(min_of<u64>(rt_.budget, 0x7fffffff));
    ++rt_.depth_;
    ++active_;
    const beside::Visit visit;  // (the entries beside the lists that the class names are this visit's)
    const bool known = blueprint_->call(function, args, results);
    rt_.chain_ends(*this);
    --rt_.depth_;
    return known;
  }
  auto it = fn_index_.find(std::string(function));
  if (it == fn_index_.end()) return false;
  const int f = it->second;
  const Function& fn = graph_.functions[static_cast<std::size_t>(f)];
  results.reserve(fn.outputs.size());
  for (const FunctionPin& p : fn.outputs) results.push_back(Value::from_json(p.value, declared(p), graph_));
  const int entry = fn_entry_[static_cast<std::size_t>(f)];
  if (entry < 0 || disabled_) return false;
  // (A function does not call itself - check() refuses that. An event it raises may reach a
  // handler that calls it again while it runs: a call like any other, as in the Blueprint's C++.
  // Its parameters are the inner call's then; Runtime::fire gives the outer call its own back.)
  CNode& e = nodes_[static_cast<std::size_t>(entry)];
  // The inputs by value are the entry node's outputs. A reference input is the place the call
  // names (`places`) - or, called from outside, a variable of its own that starts as the argument.
  std::vector<Spot> bound(fn.inputs.size());
  for (std::size_t k = 0; k < fn.inputs.size(); ++k) {
    const FunctionPin& in = fn.inputs[k];
    if (in.ref) {
      if (places && k < places->size()) {
        bound[k] = (*places)[k];
      } else {
        bound[k].key = fn.name + "." + in.name;
        vars_[bound[k].key] = k < args.size() ? from_outside(args[k], declared(in), graph_) : zero_of(declared(in), graph_);
      }
      continue;
    }
    for (std::size_t o = 0; o < e.out.size(); ++o) {
      if (e.out[o].type == PinType::exec || e.out[o].id != in.name) continue;
      // (an input by value is what it is declared as: a list of a fixed length at its length)
      e.outputs[o] = k < args.size() ? from_outside(args[k], {e.out[o].type, e.out[o].of, in.count}, graph_) : Value::from_json(in.value, declared(in), graph_).converted(e.out[o].type);
    }
  }
  std::vector<Spot> outer = std::move(refs_[static_cast<std::size_t>(f)]);
  refs_[static_cast<std::size_t>(f)] = std::move(bound);
  // A call from outside any chain (the engine's C++ asking a rule) is a step of its own for the
  // loop guard: the budget counts what one step runs, and a paused world has no ticks to start
  // a new count while its systems keep asking.
  if (rt_.depth_ == 0) rt_.step_starts();
  for (const Local& l : locals_[static_cast<std::size_t>(f)]) hold(l.key, l.init);  // (every call starts with its locals' defaults)
  Frame frame;
  frame.function = f;
  frame.depth = rt_.depth_ + (inside ? 0 : 1);  // (the depth its own chain runs at)
  frame.stack_base = stack_[0];
  frame.results = std::move(results);
  frames_.push_back(std::move(frame));
  rt_.run_from(*this, entry, 0, inside ? Runtime::Chain::call_inside : Runtime::Chain::call);
  results = std::move(frames_.back().results);
  frames_.pop_back();
  // (from outside: what the function made of its reference inputs stands behind its outputs)
  if (!places)
    for (const FunctionPin& in : fn.inputs)
      if (in.ref && !in.constant) results.push_back(held(fn.name + "." + in.name));
  refs_[static_cast<std::size_t>(f)] = std::move(outer);
  return true;
}

f64 Instance::call_number(std::string_view function, std::initializer_list<f64> args, f64 fallback) {
  std::vector<Value> in, out;
  in.reserve(args.size());
  for (f64 a : args) in.push_back(Value::number(a));
  return call(function, in, out) && !out.empty() ? out[0].as_number() : fallback;
}

bool Instance::return_values(std::string_view function, const std::vector<std::pair<std::string, Value>>& values) {
  // Only inside the function's own chain: a Return reached from an event the function raised,
  // or after a latent node, ends that chain and nothing else.
  if (frames_.empty() || frames_.back().depth != rt_.depth_) return false;
  Frame& fr = frames_.back();
  const Function& fn = graph_.functions[static_cast<std::size_t>(fr.function)];
  if (fn.name != function) {
    rt_.host().error(name_ + ": a Return of \"" + std::string(function) + "\" was reached inside \"" + fn.name + "\" - its values are ignored");
  } else {
    for (std::size_t o = 0; o < fn.outputs.size() && o < fr.results.size(); ++o)
      for (const auto& [name, v] : values)
        if (name == fn.outputs[o].name) { fr.results[o] = v.converted(fn.outputs[o].type); break; }
  }
  fr.returned = true;
  stack_[0] = fr.stack_base;  // generated code: nothing of the call is left to continue
  return true;
}

Value Instance::get(std::string_view variable) const {
  if (blueprint_) {
    Value v;
    return blueprint_->get(variable, v) ? v : Value();
  }
  // (a function's local variables are its own: nobody outside reads or sets them)
  return variable.find('.') == std::string_view::npos ? held(std::string(variable)) : Value();
}
Value Instance::held(const std::string& key) const {
  if (const int k = var_slot(key); k >= 0) {
    auto t = var_types_.find(key);
    return from_slot(slots_[static_cast<std::size_t>(k)], t != var_types_.end() ? t->second.type : PinType::number);
  }
  auto it = vars_.find(key);
  return it == vars_.end() ? Value() : it->second;
}
bool Instance::hold(const std::string& key, const Value& v) {
  auto t = var_types_.find(key);
  if (t == var_types_.end()) return false;
  if (const int k = var_slot(key); k >= 0) {
    slots_[static_cast<std::size_t>(k)] = to_type(v.as_number(), t->second.type);
    return true;
  }
  auto it = vars_.find(key);
  if (it == vars_.end()) return false;
  it->second = from_outside(v, {it->second.type, t->second.of, t->second.count}, graph_);
  return true;
}
Value Instance::get(const Node& n) const { return blueprint_ ? Value() : held(key_of(n)); }
bool Instance::set(const Node& n, const Value& v) { return !blueprint_ && hold(key_of(n), v); }
Value* Instance::variable(const Node& n) {
  if (blueprint_) return nullptr;
  auto it = vars_.find(key_of(n));
  return it == vars_.end() ? nullptr : &it->second;
}
namespace {
/// The entry `at` of a list becomes `x` (a list of structs holds them as values: Value::edit).
void set_item(Value& list, std::size_t at, const Value& x) {
  switch (list.type) {
    case PinType::boolean_list: list.edit<bool>()[at] = x.as_bool(); break;
    case PinType::integer_list: list.edit<i64>()[at] = x.as_int(); break;
    case PinType::number_list: list.edit<f64>()[at] = x.as_number(); break;
    case PinType::vector_list: list.edit<Vec3>()[at] = x.as_vector(); break;
    case PinType::text_list: list.edit<std::string>()[at] = x.as_text(); break;
    case PinType::actor_list: list.edit<ActorHandle>()[at] = ActorHandle{x.as_actor()}; break;
    case PinType::enumeration_list: list.edit<i64>()[at] = x.i; break;
    default: break;
  }
}
/// What an entry of a list - or the value of a map's entry at its place - is of.
TypeRef entry_type(const TypeRef& t) { return t.type == PinType::map ? value_type(t) : TypeRef{element_of(t.type), t.of}; }
/// Has a value of this type entries by their place, a path's `[]`? (A list; a map: its values in
/// the order of their keys.)
bool indexed(PinType t) { return is_list(t) || t == PinType::map; }
/// What `steps` (from `k` on) lead to in `v`, a value of type `t`. Beside a list: an entry with
/// the defaults.
Value value_at(const Value& v, const TypeRef& t, const std::vector<Instance::Spot::Step>& steps, std::size_t k, const Graph& g) {
  if (k == steps.size()) return v;
  if (steps[k].field.empty()) {
    const i64 at = steps[k].index;
    const TypeRef item = entry_type(t);
    const bool there = indexed(t.type) && at >= 0 && static_cast<std::size_t>(at) < v.size();
    return value_at(there ? v.item(static_cast<std::size_t>(at)) : zero_of(item, g), item, steps, k + 1, g);
  }
  const StructType* s = t.type == PinType::structure ? g.structure(t.of) : nullptr;
  const int f = s ? s->field(steps[k].field) : -1;
  if (f < 0) return Value();
  return value_at(field_of(v, *s, static_cast<std::size_t>(f), g), s->fields[static_cast<std::size_t>(f)].type, steps, k + 1, g);
}
/// The same to change it: `change` gets the value there, in its place - every value on the way
/// is the variable's own (Value::edit, own_rows). Beside a list `change` gets an entry with the
/// defaults, and nothing of it is kept.
void change_at(Value& v, const TypeRef& t, const std::vector<Instance::Spot::Step>& steps, std::size_t k, const Graph& g, const std::function<void(Value&)>& change) {
  if (k == steps.size()) {
    change(v);
    return;
  }
  if (steps[k].field.empty()) {
    const i64 at = steps[k].index;
    const TypeRef item = entry_type(t);
    const bool there = indexed(t.type) && at >= 0 && static_cast<std::size_t>(at) < v.size();
    if (!there) {
      Value beside = zero_of(item, g);
      change_at(beside, item, steps, k + 1, g, change);
      return;
    }
    if (t.type == PinType::structure_list) {
      change_at(v.edit<Value>()[static_cast<std::size_t>(at)], item, steps, k + 1, g, change);
      return;
    }
    if (t.type == PinType::map) {
      change_at(v.own_entries().value_at(static_cast<std::size_t>(at)), item, steps, k + 1, g, change);
      return;
    }
    Value entry = v.item(static_cast<std::size_t>(at));
    change_at(entry, item, steps, k + 1, g, change);
    set_item(v, static_cast<std::size_t>(at), entry);
    return;
  }
  const StructType* s = t.type == PinType::structure ? g.structure(t.of) : nullptr;
  const int f = s ? s->field(steps[k].field) : -1;
  if (f < 0) return;
  hold_fields(v, *s, g);
  change_at(v.own_rows()[static_cast<std::size_t>(f)], s->fields[static_cast<std::size_t>(f)].type, steps, k + 1, g, change);
}
}  // namespace
bool Instance::spot(const Node& n, std::string_view prefix, const std::vector<i64>& indices, Spot& out) const {
  out = Spot();
  const std::string name = n.param(std::string(prefix) + "variable"), scope = n.param("scope");
  auto fn = scope.empty() ? fn_index_.end() : fn_index_.find(scope);
  const Function* f = fn == fn_index_.end() ? nullptr : &graph_.functions[static_cast<std::size_t>(fn->second)];
  if (f && f->local(name)) {
    out.key = scope + "." + name;
  } else if (const FunctionPin* in = f ? f->input(name) : nullptr) {
    const std::size_t k = static_cast<std::size_t>(in - f->inputs.data());
    if (in->ref) {
      // (a reference input: the place the running call was given)
      const std::vector<Spot>& bound = refs_[static_cast<std::size_t>(fn->second)];
      if (k >= bound.size()) return false;
      out = bound[k];
    } else {
      // (an input by value: what the entry node holds of the running call)
      out.entry = fn_entry_[static_cast<std::size_t>(fn->second)];
      if (out.entry < 0) return false;
      const CNode& e = nodes_[static_cast<std::size_t>(out.entry)];
      for (std::size_t o = 0; o < e.out.size(); ++o)
        if (e.out[o].type != PinType::exec && e.out[o].id == name) out.output = static_cast<int>(o);
      if (out.output < 0) return false;
    }
  } else if (scope.empty() || !prefix.empty()) {
    out.key = name;
  } else {
    return false;
  }
  std::vector<std::string> steps;
  if (!path_steps(n.param(std::string(prefix) + "path"), steps)) return false;
  std::size_t entry = 0;
  const std::size_t first = out.steps.size();  // (a reference input's own steps have been walked when it was bound)
  for (std::string& step : steps) {
    Spot::Step st;
    if (step.empty()) st.index = entry < indices.size() ? indices[entry] : -1, ++entry;
    else st.field = std::move(step);
    out.steps.push_back(std::move(st));
  }
  // Naming the place finds it. Where a list entry lies beside its list, the place goes on in an
  // entry of its own with the zero of the items' type, as in a Blueprint's class (kern::List).
  TypeRef t;
  Value at;
  if (out.beside) {
    t = out.beside->type;
    at = out.beside->value;
  } else if (out.entry >= 0) {
    const CNode& e = nodes_[static_cast<std::size_t>(out.entry)];
    t = {e.out[static_cast<std::size_t>(out.output)].type, e.out[static_cast<std::size_t>(out.output)].of};
    at = e.outputs[static_cast<std::size_t>(out.output)];
  } else {
    auto type = var_types_.find(out.key);
    if (type == var_types_.end()) return false;
    t = type->second;
    at = held(out.key);
  }
  for (std::size_t k = 0; k < first && k < out.steps.size(); ++k) {
    // (the steps it came with: the same way as when it was bound)
    const Spot::Step& st = out.steps[k];
    if (st.field.empty()) {
      const TypeRef item = entry_type(t);
      at = st.index >= 0 && static_cast<std::size_t>(st.index) < at.size() ? at.item(static_cast<std::size_t>(st.index)) : zero_of(item, graph_);
      t = item;
    } else {
      const StructType* s = t.type == PinType::structure ? graph_.structure(t.of) : nullptr;
      const int f = s ? s->field(st.field) : -1;
      if (f < 0) return false;
      at = field_of(at, *s, static_cast<std::size_t>(f), graph_);
      t = s->fields[static_cast<std::size_t>(f)].type;
    }
  }
  for (std::size_t k = first; k < out.steps.size(); ++k) {
    const Spot::Step st = out.steps[k];
    if (!st.field.empty()) {
      const StructType* s = t.type == PinType::structure ? graph_.structure(t.of) : nullptr;
      const int f = s ? s->field(st.field) : -1;
      if (f < 0) return false;
      at = field_of(at, *s, static_cast<std::size_t>(f), graph_);
      t = s->fields[static_cast<std::size_t>(f)].type;
      continue;
    }
    if (!indexed(t.type)) return false;
    const TypeRef item = entry_type(t);
    if (st.index >= 0 && static_cast<std::size_t>(st.index) < at.size()) {
      at = at.item(static_cast<std::size_t>(st.index));
      t = item;
      continue;
    }
    // beside the list: from here on the place is in an entry of its own
    at = zero_of(item, graph_);
    out.beside = std::make_shared<Spot::Beside>(Spot::Beside{item, at});
    out.key.clear();
    out.entry = out.output = -1;
    out.steps.erase(out.steps.begin(), out.steps.begin() + static_cast<std::ptrdiff_t>(k) + 1);
    k = static_cast<std::size_t>(-1);  // (the steps that are left start at that entry)
    t = item;
  }
  return true;
}
Value Instance::read(const Spot& s) const {
  if (s.beside) return value_at(s.beside->value, s.beside->type, s.steps, 0, graph_);
  if (s.entry >= 0) {
    const CNode& e = nodes_[static_cast<std::size_t>(s.entry)];
    const std::size_t o = static_cast<std::size_t>(s.output);
    return value_at(e.outputs[o], {e.out[o].type, e.out[o].of}, s.steps, 0, graph_);
  }
  auto t = var_types_.find(s.key);
  if (t == var_types_.end()) return Value();
  return s.steps.empty() ? held(s.key) : value_at(held(s.key), t->second, s.steps, 0, graph_);
}
bool Instance::write(const Spot& s, const std::function<void(Value&)>& change) {
  if (s.beside) {
    change_at(s.beside->value, s.beside->type, s.steps, 0, graph_, change);
    return true;
  }
  auto t = var_types_.find(s.key);
  if (s.entry >= 0 || t == var_types_.end()) return false;
  if (var_slot(s.key) >= 0) {
    // (a number of the Blueprint lives in a slot: there is no way into it)
    if (!s.steps.empty()) return false;
    Value v = held(s.key);
    change(v);
    return hold(s.key, v);
  }
  auto it = vars_.find(s.key);
  if (it == vars_.end()) return false;
  change_at(it->second, t->second, s.steps, 0, graph_, change);
  return true;
}
Value Instance::place(const Node& n, const std::vector<i64>& indices) const {
  Spot s;
  return !blueprint_ && spot(n, {}, indices, s) ? read(s) : Value();
}
bool Instance::change(const Node& n, const std::vector<i64>& indices, const std::function<void(Value&)>& change) {
  Spot s;
  return !blueprint_ && spot(n, {}, indices, s) && write(s, change);
}
std::vector<Value> Instance::locals() const {
  std::vector<Value> r;
  for (const auto& of : locals_)
    for (const Local& l : of) r.push_back(held(l.key));
  return r;
}
Instance::Held Instance::held_calls() const {
  Held h;
  for (int entry : fn_entry_) h.parameters.push_back(entry >= 0 ? nodes_[static_cast<std::size_t>(entry)].outputs : std::vector<Value>());
  h.locals = locals();
  h.refs = refs_;
  return h;
}
void Instance::set_held_calls(Held held) {
  set_locals(held.locals);
  refs_ = std::move(held.refs);
  for (std::size_t f = 0; f < held.parameters.size() && f < fn_entry_.size(); ++f)
    if (fn_entry_[f] >= 0) nodes_[static_cast<std::size_t>(fn_entry_[f])].outputs = std::move(held.parameters[f]);
}
void Instance::set_locals(const std::vector<Value>& values) {
  std::size_t k = 0;
  for (const auto& of : locals_)
    for (const Local& l : of)
      if (k < values.size()) hold(l.key, values[k++]);
}

Value* Instance::variable(std::string_view name) {
  if (blueprint_) return nullptr;
  auto it = name.find('.') == std::string_view::npos ? vars_.find(std::string(name)) : vars_.end();
  return it == vars_.end() ? nullptr : &it->second;
}
bool Instance::set(std::string_view variable, const Value& v) {
  if (const Variable* named = graph_.variable(variable); named && named->constant) return false;  // (a constant is what it is)
  if (blueprint_) return blueprint_->set(variable, v);
  return variable.find('.') == std::string_view::npos && hold(std::string(variable), v);
}

std::string Instance::state(std::string_view machine) const {
  for (const Machine& m : machines_)
    if (m.def->name == machine) {
      const State* s = m.def->find(m.state);
      return s ? s->name : std::string();
    }
  return {};
}

bool Instance::set_state(std::string_view machine, std::string_view state) {
  for (std::size_t i = 0; i < machines_.size(); ++i)
    if (machines_[i].def->name == machine) {
      const State* s = machines_[i].def->find(state);
      if (!s) return false;
      rt_.enter_state(*this, i, s->id);
      return true;
    }
  return false;
}

// ---------------------------------------------------------------------------
// runtime

const Value* Event::arg(std::string_view name) const {
  for (const auto& [k, v] : args) if (k == name) return &v;
  return nullptr;
}

Runtime::Runtime(Host& host, const Library& lib) : host_(host), lib_(lib) {}
Runtime::~Runtime() = default;

Instance* Runtime::add(const Graph& own, ActorRef self, std::string name) {
  // A Blueprint that uses libraries runs as one graph with them: their functions are functions
  // of that graph, and a call of one is a call like any other - in every tier.
  Graph linked;
  std::string unlinked;
  const bool whole = own.uses.empty() || link(own, blueprints, linked, &unlinked, &lib_);
  const Graph& graph = !own.uses.empty() && whole ? linked : own;
  auto inst = std::make_unique<Instance>(*this, graph, self, std::move(name));
  if (!whole) {
    host_.error(inst->name() + ": " + unlinked);
    inst->disabled_ = true;
  }
  for (const Issue& i : check(graph, lib_))
    if (i.error) {
      host_.error(inst->name() + ": " + i.text);
      inst->disabled_ = true;
    }
  Instance* added = inst.get();
  instances_.push_back(std::move(inst));
  if (!added->disabled_) {
    // Code compiled into the binary wins - the Blueprint's own C++ class; else the host's JIT.
    auto own = blueprint_registry().find(graph_hash(graph));
    if (native && own != blueprint_registry().end()) {
      added->blueprint_ = own->second();
      added->blueprint_->attach({&host_, this, added, self, counters_});
    } else if (jit && added->jit_fits_) {
      added->jit_pending_ = host_.jit_compile(compile_wasm(*added), added->id_);
    }
  }
  return added;
}

void Runtime::remove(ActorRef self) {
  for (std::size_t i = 0; i < instances_.size();) {
    if (instances_[i]->self() == self) {
      Instance* gone = instances_[i].get();
      pending_.erase(std::remove_if(pending_.begin(), pending_.end(), [&](const Pending& p) { return p.inst == gone; }), pending_.end());
      instances_.erase(instances_.begin() + static_cast<std::ptrdiff_t>(i));
    } else ++i;
  }
}

void Runtime::clear() {
  instances_.clear();
  pending_.clear();
  trace_.clear();
}

void Runtime::call_on(Instance& caller, std::string_view function, ActorRef target, const std::vector<Value>& args, std::vector<Value>& results) {
  results.clear();
  const Graph& g = caller.graph();
  const Function* declared = g.function(function);
  if (declared)
    for (const FunctionPin& p : declared->outputs) results.push_back(Value::from_json(p.value, logic::declared(p), g));
  Instance* to = find(target);
  if (!to || to->disabled_) return;
  if (depth_ >= 64) {
    host_.error(caller.name() + ": calls on other Blueprints nest more than 64 deep - the call of " + std::string(function) + " is left out");
    return;
  }
  // (a Blueprint in the middle of a call of that function: the interpreter and the JIT hold a
  // call's values in the instance - the call that is interrupted has its own back afterwards,
  // as the Blueprint's class has them in its methods' own parameters and locals)
  const bool at_work = !to->blueprint_ && to->active_ > 0;
  Instance::Held held;
  if (at_work) held = to->held_calls();
  std::vector<Value> got;
  const bool known = to->call(function, args, got);
  if (at_work) to->set_held_calls(std::move(held));
  if (!known || !declared) return;
  for (std::size_t k = 0; k < results.size() && k < got.size(); ++k)
    results[k] = from_outside(got[k], logic::declared(declared->outputs[k]), g);
}

Instance* Runtime::find(ActorRef self) {
  for (auto& i : instances_) if (i->self() == self) return i.get();
  return nullptr;
}

void Runtime::step_starts() {
  tick_start_ = executed_;
  counters_[1] = static_cast<i32>(min_of<u64>(budget, 0x7fffffff));
  counters_[3] = 0;
}

bool Runtime::pass(Instance& inst) {
  counters_[1] = static_cast<i32>(min_of<u64>(budget, 0x7fffffff));  // (the limit as it is now: compiled code reads it there)
  if (!inst.stopping_ && counters_[3] < counters_[1]) {
    ++counters_[3];
    return true;
  }
  if (!inst.stopping_) {
    inst.stopping_ = true;
    host_.error(inst.name() + ": a loop made more than " + std::to_string(counters_[1]) + " passes in one step - infinite loop? The Blueprint is stopped.");
  }
  return false;
}

void Runtime::chain_ends(Instance& inst) {
  if (--inst.active_ == 0 && inst.stopping_) inst.disabled_ = true;
}

void Runtime::begin() {
  step_starts();
  for (std::size_t k = 0; k < instances_.size(); ++k) {
    Instance& inst = *instances_[k];
    if (inst.begun_ || inst.disabled_) continue;
    inst.begun_ = true;
    for (std::size_t m = 0; m < inst.machines_.size(); ++m)
      for (const State& s : inst.machines_[m].def->states)
        if (s.initial) { enter_state(inst, m, s.id); break; }
    Event e;
    e.type = "logic.begin";
    e.target = inst.self();
    for (std::size_t at = 0; at < inst.events_.size() && !inst.disabled_; ++at) {
      const int i = inst.events_[at];
      if (inst.nodes_[static_cast<std::size_t>(i)].def->type == "event.begin") run_from(inst, i, 0);
    }
  }
}

void Runtime::tick(f32 dt) {
  step_starts();
  const f64 now = host_.time();
  // Latent actions that are due (in order of their time).
  std::stable_sort(pending_.begin(), pending_.end(), [](const Pending& a, const Pending& b) { return a.at < b.at; });
  std::vector<Pending> due;
  for (std::size_t i = 0; i < pending_.size();) {
    if (pending_[i].at <= now + 1e-9) { due.push_back(pending_[i]); pending_.erase(pending_.begin() + static_cast<std::ptrdiff_t>(i)); }
    else ++i;
  }
  for (const Pending& p : due)
    if (!p.inst->disabled_) run_from(*p.inst, p.node, p.entry);
  // Tick events.
  Event tick;
  tick.type = "logic.tick";
  tick.args.push_back({"delta", Value::number(dt)});
  fire(tick);
  // State machine timers and conditions.
  for (std::size_t k = 0; k < instances_.size(); ++k) check_transitions(*instances_[k], nullptr);
}

void Runtime::fire(const Event& e) {
  // Instances may be added while handlers run (spawned actors): iterate by index.
  for (std::size_t k = 0; k < instances_.size(); ++k) {
    Instance& inst = *instances_[k];
    if (inst.disabled_ || (e.target && inst.self() != e.target && !(e.level && inst.self() == 0))) continue;
    // (the handlers of one event run in the order of their node ids, whatever order the graph is stored in)
    for (std::size_t at = 0; at < inst.events_.size(); ++at) {
      const std::size_t i = static_cast<std::size_t>(inst.events_[at]);
      Instance::CNode& c = inst.nodes_[i];
      if (c.def->event != e.type) continue;
      bool ok;
      if (c.def->accepts) ok = c.def->accepts(*c.node, e);
      else {
        const std::string want = c.def->params.empty() ? std::string() : c.node->param(c.def->params[0].key, c.def->params[0].def);
        ok = e.key.empty() || want.empty() || want == "any" || want == e.key;
      }
      if (!ok) continue;
      // The event's values are the handler's, as the parameters of a method are a call's: when the
      // event reaches this handler again while it runs (it raised its own event), the inner run
      // has its values - and the outer one has its own again when that is over.
      std::vector<Value> outer;
      const bool nested = c.running > 0;
      if (nested) outer = c.outputs;
      // The same for the functions of a Blueprint that is at work (one of them raised this event
      // and is in the middle of its call): what they hold of their parameters is the running
      // calls' again when this handler is over - whatever it called. (A Blueprint's class does
      // that in its glue.)
      std::vector<std::vector<Value>> parameters;
      const bool at_work = !inst.blueprint_ && inst.active_ > 0;
      Instance::Held held;
      if (at_work) held = inst.held_calls();
      for (std::size_t o = 0; o < c.out.size(); ++o)
        if (const Value* v = e.arg(c.out[o].id)) c.outputs[o] = v->converted(c.out[o].type);
      ++c.running;
      run_from(inst, static_cast<int>(i), 0);
      --c.running;
      if (nested) c.outputs = std::move(outer);
      if (at_work) inst.set_held_calls(std::move(held));
      if (inst.disabled_) break;
    }
  }
}

void Runtime::call(std::string_view name, ActorRef target, std::vector<std::pair<std::string, Value>> args) {
  Event e;
  e.type = "logic.custom";
  e.key = std::string(name);
  e.target = target;
  e.args = std::move(args);
  fire(e);
  const std::string n(name);
  for (std::size_t k = 0; k < instances_.size(); ++k)
    if (!target || instances_[k]->self() == target) check_transitions(*instances_[k], &n);
}

void Runtime::enter_state(Instance& inst, std::size_t machine, u32 state) {
  Instance::Machine& m = inst.machines_[machine];
  const State* old = m.def->find(m.state);
  const State* next = m.def->find(state);
  if (!next) return;
  if (old) {
    Event e;
    e.type = "logic.state_exit";
    e.target = inst.self();
    e.key = m.def->name + "/" + old->name;
    fire(e);
  }
  m.state = state;
  m.entered = host_.time();
  Event e;
  e.type = "logic.state_enter";
  e.target = inst.self();
  e.key = m.def->name + "/" + next->name;
  e.args.push_back({"previous", Value::text(old ? old->name : std::string())});
  fire(e);
}

void Runtime::check_transitions(Instance& inst, const std::string* event) {
  if (inst.disabled_ || !inst.begun_) return;
  const f64 now = host_.time();
  for (std::size_t mi = 0; mi < inst.machines_.size(); ++mi) {
    // One transition per machine per call (automatic chains advance one state per step).
    for (const Transition& t : inst.machines_[mi].def->transitions) {
      const Instance::Machine& m = inst.machines_[mi];
      if (t.from && t.from != m.state) continue;
      if (t.to == m.state && !t.from) continue;
      bool go = false;
      switch (t.trigger) {
        case TriggerKind::event: go = event && *event == t.event; break;
        case TriggerKind::timer: go = !event && now - m.entered >= t.seconds; break;
        case TriggerKind::condition: go = !event && inst.get(t.variable).as_bool(); break;
        case TriggerKind::automatic: go = !event; break;
      }
      if (go) { enter_state(inst, mi, t.to); break; }
    }
  }
}

bool Runtime::run_compiled(Instance& inst, int node, int entry) {
  counters_[1] = static_cast<i32>(min_of<u64>(budget, 0x7fffffff));
  counters_[2] = tracing ? 1 : 0;
  // (a loop of this chain that is refused a pass sets [4]; a chain that runs inside this one has its own)
  const i32 outer_refused = counters_[4];
  counters_[4] = 0;
  int r;
#if defined(__wasm__)
  // The JIT put the module's run function into the indirect function table: a direct call.
  using RunFn = int (*)(int, int);
  r = reinterpret_cast<RunFn>(static_cast<uintptr_t>(inst.jit_fn_))(node, entry);
#else
  r = 0;
  (void)node;
  (void)entry;
#endif
  executed_ += static_cast<u64>(counters_[0] > 0 ? counters_[0] : 0);
  counters_[0] = 0;
  if (counters_[4] && !inst.stopping_) {
    inst.stopping_ = true;
    host_.error(inst.name() + ": a loop made more than " + std::to_string(counters_[1]) + " passes in one step - infinite loop? The Blueprint is stopped.");
  }
  counters_[4] = outer_refused;
  if (r == 2) {
    host_.error(inst.name() + ": the graph nests deeper than " + std::to_string(inst.stack_capacity_) + " loops / sequences - stopped.");
    inst.disabled_ = true;
  }
  return true;
}

void Runtime::wait(Instance& inst, u32 node, f64 seconds, int entry) {
  const auto it = inst.index_.find(node);
  if (it == inst.index_.end()) return;
  Pending p;
  p.inst = &inst;
  p.node = it->second;
  p.at = host_.time() + max_of(0.0, seconds);
  p.entry = entry;
  p.serial = ++serial_;
  pending_.push_back(p);
}

void Runtime::cancel_waits(Instance& inst, u32 node) {
  const auto it = inst.index_.find(node);
  if (it == inst.index_.end()) return;
  const int index = it->second;
  pending_.erase(std::remove_if(pending_.begin(), pending_.end(), [&](const Pending& p) { return p.inst == &inst && p.node == index; }), pending_.end());
}

void Runtime::run_from(Instance& inst, int start, int entry, Chain chain) {
  if (inst.blueprint_) {
    // The Blueprint's own class: the chain is a method of it.
    if (++depth_ > 64) {
      --depth_;
      host_.error(inst.name() + ": events nest more than 64 deep - the chain is stopped");
      return;
    }
    // An event that decides itself whether it is due (Every N Seconds counts its time): that is
    // the event's side, the same code for every tier - then the handler.
    const Instance::CNode& c = inst.nodes_[static_cast<std::size_t>(start)];
    bool due = true;
    if (entry != Context::kResume && c.def && c.def->kind == NodeKind::event && c.def->run) {
      Context ctx(*this, inst, start, entry);
      due = c.def->run(ctx) == 0;
    }
    counters_[1] = static_cast<i32>(min_of<u64>(budget, 0x7fffffff));  // (the class reads the limit there)
    ++inst.active_;
    const beside::Visit visit;
    if (due) inst.blueprint_->run(start, entry);
    chain_ends(inst);
    --depth_;
    return;
  }
  // How deep chains nest: handlers of events that handlers raise (at most 64), and a function
  // the engine calls from outside. A function called by a node of the graph belongs to the chain
  // it is called in - in the Blueprint's C++ it is a call like any other, and functions cannot
  // call themselves.
  const bool counts = chain != Chain::call_inside;
  if (counts && ++depth_ > 64 && chain == Chain::event) {
    --depth_;
    host_.error(inst.name() + ": events nest more than 64 deep - the chain is stopped");
    return;
  }
  if (inst.compiled()) {
    ++inst.active_;
    run_compiled(inst, start, entry);
    chain_ends(inst);
    if (counts) --depth_;
    return;
  }
  // A Return node ended the function call this chain is (Instance::return_values).
  auto returned = [&] { return !inst.frames_.empty() && inst.frames_.back().returned && inst.frames_.back().depth == depth_; };
  std::vector<std::pair<int, int>> stack{{start, entry}};
  ++inst.active_;
  while (!stack.empty() && !inst.disabled_ && !returned()) {
    auto [n, e] = stack.back();
    stack.pop_back();
    while (n >= 0 && !inst.disabled_) {
      ++executed_;
      Instance::CNode& c = inst.nodes_[static_cast<std::size_t>(n)];
      if (!c.def) break;
      Context ctx(*this, inst, n, e);
      const int r = c.def->run ? c.def->run(ctx) : 0;
      if (tracing && trace_.size() < 4096) trace_.push_back({inst.name(), c.node->id, host_.time()});
      if (returned()) break;
      for (auto it = ctx.continuations_.rbegin(); it != ctx.continuations_.rend(); ++it) stack.push_back(*it);
      if (r < 0 || static_cast<std::size_t>(r) >= c.exec_out.size()) break;
      e = c.exec_out_entry[static_cast<std::size_t>(r)];
      n = c.exec_out[static_cast<std::size_t>(r)];
    }
  }
  chain_ends(inst);
  if (counts) --depth_;
}

// ---------------------------------------------------------------------------
// context

Context::Context(Runtime& rt, Instance& inst, int node, int entry) : rt_(rt), inst_(inst), node_(node), entry_(entry) {}

std::string Context::param(std::string_view key, std::string_view fallback) const {
  const Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  for (const auto& [k, v] : c.node->params) if (k == key) return v;
  if (c.def)
    for (const ParamDef& p : c.def->params) if (p.key == key) return p.def;
  return std::string(fallback);
}

Value Context::pull(int node, int pin) {
  Instance::CNode& src = inst_.nodes_[static_cast<std::size_t>(node)];
  // Outputs of inline nodes live in slots (generated code writes them there).
  if (const int s = inst_.out_slot(node, pin); s >= 0 && src.def->kind != NodeKind::pure)
    return from_slot(inst_.slots_[static_cast<std::size_t>(s)], src.out[static_cast<std::size_t>(pin)].type).converted(src.out[static_cast<std::size_t>(pin)].type);
  if (src.def && src.def->kind == NodeKind::pure && src.def->run) {
    // (as deep as the values read each other: a value that reads itself is an error of the
    // graph - check() - and an expression in C++ has no limit either)
    ++rt_.executed_;
    Context c(rt_, inst_, node, 0);
    src.def->run(c);
  }
  return src.outputs[static_cast<std::size_t>(pin)];
}

Value Context::in(std::size_t index) {
  Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  if (index >= c.in.size()) return Value();
  const auto& src = c.data_in[index];
  if (src.node < 0) return c.literal[index];
  return pull(src.node, src.pin).converted(c.in[index].type);
}

std::vector<i64> Context::path() {
  std::vector<i64> r;
  const std::size_t count = inst_.nodes_[static_cast<std::size_t>(node_)].in.size();
  for (std::size_t k = 0; k < count; ++k)
    if (is_path_pin(inst_.nodes_[static_cast<std::size_t>(node_)].in[k].id)) r.push_back(in(k).as_int());
  return r;
}

Instance::Spot Context::spot(std::string_view prefix) {
  std::vector<i64> indices;
  const std::size_t count = inst_.nodes_[static_cast<std::size_t>(node_)].in.size();
  for (std::size_t k = 0;; ++k) {
    const std::string pin = std::string(prefix) + path_pin(k);
    bool found = false;
    for (std::size_t i = 0; i < count && !found; ++i)
      if (inst_.nodes_[static_cast<std::size_t>(node_)].in[i].id == pin) {
        indices.push_back(in(i).as_int());
        found = true;
      }
    if (!found) break;
  }
  Instance::Spot s;
  inst_.spot(node(), prefix, indices, s);
  return s;
}

Value Context::in(std::string_view pin) {
  const Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  for (std::size_t i = 0; i < c.in.size(); ++i) if (c.in[i].id == pin) return in(i);
  return Value();
}

void Context::out(std::size_t index, Value v) {
  Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  if (index >= c.outputs.size()) return;
  c.outputs[index] = v.converted(c.out[index].type);
  // (a struct that holds no fields is its type's default: an item from beyond a list's end)
  if (c.out[index].type == PinType::structure && !c.outputs[index].rows) c.outputs[index] = zero_of({c.out[index].type, c.out[index].of}, inst_.graph_);
  const int s = inst_.out_slot(node_, static_cast<int>(index));
  if (s >= 0) inst_.slots_[static_cast<std::size_t>(s)] = c.outputs[index].as_number();
}

void Context::out(std::string_view pin, Value v) {
  const Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  for (std::size_t i = 0; i < c.out.size(); ++i) if (c.out[i].id == pin) { out(i, std::move(v)); return; }
}

int Context::exec(std::string_view pin) const {
  const Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  int ord = 0;
  for (const PinDef& p : c.out) {
    if (p.type != PinType::exec) continue;
    if (p.id == pin) return ord;
    ++ord;
  }
  return kStop;
}

int Context::wait(f64 seconds, int entry) {
  Runtime::Pending p;
  p.inst = &inst_;
  p.node = node_;
  p.at = rt_.host_.time() + max_of(0.0, seconds);
  p.entry = entry;
  p.serial = ++rt_.serial_;
  rt_.pending_.push_back(p);
  return kSuspend;
}

void Context::cancel_waits() {
  auto& v = rt_.pending_;
  v.erase(std::remove_if(v.begin(), v.end(), [&](const Runtime::Pending& p) { return p.inst == &inst_ && p.node == node_; }), v.end());
}


void Context::then(int exec_output) {
  const Instance::CNode& c = inst_.nodes_[static_cast<std::size_t>(node_)];
  if (exec_output < 0 || static_cast<std::size_t>(exec_output) >= c.exec_out.size()) return;
  const int target = c.exec_out[static_cast<std::size_t>(exec_output)];
  if (target >= 0) continuations_.push_back({target, c.exec_out_entry[static_cast<std::size_t>(exec_output)]});
}

bool Context::loop_back(int entry) {
  if (!rt_.pass(inst_)) return false;
  continuations_.push_back({node_, entry});
  return true;
}

}  // namespace kern::logic
