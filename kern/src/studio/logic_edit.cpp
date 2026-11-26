// Kern Studio: editing logic graphs (the level's and the actors'). Every edit is
// a level edit (one undo step); the page draws what logic_json() describes.
#include "kern/logic/engine_libraries.h"
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <cmath>

#include "internal.h"
#include "kern/audio/synth.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/nodes.h"
#include "kern/studio/studio.h"

namespace kern::studio {

namespace {

json::Value ok_result(bool v = true) {
  json::Value r = json::Value::object();
  r.set("ok", v);
  return r;
}
json::Value fail_result(const std::string& text) {
  json::Value r = json::Value::object();
  r.set("ok", false);
  r.set("error", text);
  return r;
}

/// Why `name` cannot name a variable, a function or a function's pin (empty: it can). A Blueprint
/// is its C++ as well (logic::to_cpp), and there these are names of the language.
/// Is `name` a local variable of one of the Blueprint's functions?
bool is_local(const logic::Graph& g, const std::string& name) {
  for (const logic::Function& f : g.functions) if (f.local(name)) return true;
  return false;
}
std::string name_problem(const logic::Graph& g, const std::string& name) {
  if (!logic::cpp_name_ok(name))
    return "\"" + name + "\" cannot be a name: letters, digits and _ (no spaces), not a word of C++ (int, class ...) or of Blueprints in C++ (Float, Text, loop ...)";
  for (const auto& d : logic::library().all()) {
    std::string fn = d->type;
    for (char& ch : fn) if (ch == '.') ch = '_';
    if (fn == name) return "\"" + name + "\" is the name of a node (" + d->label + ")";
  }
  for (const logic::Function& f : g.functions)
    if (f.name + "Result" == name) return "\"" + name + "\" is taken by the function " + f.name + " (the type of its results in C++)";
  if (g.enumeration(name)) return "\"" + name + "\" is an enum of this Blueprint";
  if (g.structure(name)) return "\"" + name + "\" is a struct of this Blueprint";
  return {};
}

/// Why `name` cannot name a type of the Blueprint - an enum or a struct - beyond what
/// name_problem says (empty: it can). In the Blueprint's C++ a type's name is known inside every
/// struct: no field there has it.
std::string type_name_problem(const logic::Graph& g, const std::string& name) {
  if (name.size() > 5 && name.substr(name.size() - 5) == "_list") return "a name that ends in _list reads as a list";
  for (const logic::StructType& s : g.structs)
    if (s.field(name) >= 0) return "\"" + name + "\" is a field of the struct " + s.name;
  return {};
}

std::string lower(std::string s) {
  for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

const char* kind_name(logic::NodeKind k) {
  switch (k) {
    case logic::NodeKind::event: return "event";
    case logic::NodeKind::action: return "action";
    case logic::NodeKind::pure: return "pure";
    case logic::NodeKind::flow: return "flow";
  }
  return "action";
}

const char* param_kind_name(logic::ParamDef::Kind k) {
  using K = logic::ParamDef::Kind;
  switch (k) {
    case K::text: return "text";
    case K::choice: return "choice";
    case K::variable: return "variable";
    case K::event: return "event";
    case K::machine: return "machine";
    case K::state: return "state";
    case K::key: return "key";
    case K::sound: return "sound";
    case K::actor_template: return "template";
    case K::tag: return "tag";
    case K::integer: return "integer";
    case K::function: return "function";
    case K::type: return "type";
    case K::field: return "field";
    case K::scope: return "scope";
    case K::path: return "path";
  }
  return "text";
}

logic::Graph* graph_of(game::Level& l, u32 owner, bool create) {
  if (!owner) return &l.logic;
  game::Actor* a = l.find(owner);
  if (!a) return nullptr;
  if (!a->logic) {
    if (!create) return nullptr;
    a->logic = logic::Graph{};
  }
  return &*a->logic;
}

const logic::Graph* graph_of(const game::Level& l, u32 owner) {
  if (!owner) return &l.logic;
  const game::Actor* a = l.find(owner);
  return a && a->logic ? &*a->logic : nullptr;
}

/// The other Blueprints of the level: (owner, its name, its graph) for each but `owner`'s.
std::vector<std::tuple<u32, std::string, const logic::Graph*>> other_blueprints(const game::Level& l, u32 owner) {
  std::vector<std::tuple<u32, std::string, const logic::Graph*>> r;
  if (owner != 0 && !l.logic.empty()) r.emplace_back(0u, "Level", &l.logic);
  for (const game::Actor& a : l.actors)
    if (a.id != owner && a.logic && !a.logic->empty()) r.emplace_back(a.id, a.name, &*a.logic);
  return r;
}
/// Every Blueprint of the level but the graph `g`: its owner's name and its graph, to change.
std::vector<std::pair<std::string, logic::Graph*>> other_graphs(game::Level& l, const logic::Graph* g) {
  std::vector<std::pair<std::string, logic::Graph*>> r;
  if (&l.logic != g && !l.logic.empty()) r.emplace_back("Level", &l.logic);
  for (game::Actor& a : l.actors)
    if (a.logic && &*a.logic != g && !a.logic->empty()) r.emplace_back(a.name, &*a.logic);
  return r;
}
// ---- types of the project: an enum or a struct that several Blueprints of the level have, the same in each ----
bool is_project_type(const logic::Graph& g, const std::string& name) {
  if (const logic::EnumType* e = g.enumeration(name)) return e->project;
  const logic::StructType* s = g.structure(name);
  return s && s->project;
}
/// The type `name` of `g` is one of the project - and so are the types its fields are of.
void make_project_type(logic::Graph& g, const std::string& name) {
  if (logic::EnumType* e = g.enumeration(name)) { e->project = true; return; }
  logic::StructType* s = g.structure(name);
  if (!s || s->project) return;
  s->project = true;
  std::vector<std::string> of;
  for (const logic::StructField& f : s->fields) if (!f.type.of.empty()) of.push_back(f.type.of);
  for (const std::string& n : of) make_project_type(g, n);
}
/// The Blueprint `to` gets the type `name` of `from` - a type of the project from now on - with
/// the types its fields are of. "" when it has it, else why not.
std::string adopt_project_type(logic::Graph& to, logic::Graph& from, const std::string& name) {
  make_project_type(from, name);
  auto differs = [&] { return "this Blueprint has a type " + name + " that is not the project's: rename one of them, or make them the same"; };
  if (const logic::EnumType* e = from.enumeration(name)) {
    if (const logic::EnumType* mine = to.enumeration(name)) return mine->project && logic::same_type(*mine, *e) ? std::string() : differs();
    if (to.structure(name) || to.variable(name) || to.function(name)) return "this Blueprint has something called " + name + " already";
    to.enums.push_back(*e);
    return {};
  }
  const logic::StructType* s = from.structure(name);
  if (!s) return "there is no type " + name;
  if (const logic::StructType* mine = to.structure(name)) return mine->project && logic::same_type(*mine, *s) ? std::string() : differs();
  if (to.enumeration(name) || to.variable(name) || to.function(name)) return "this Blueprint has something called " + name + " already";
  const logic::StructType copy = *s;  // (the fields' types first: a struct stands after the structs it holds)
  for (const logic::StructField& f : copy.fields)
    if (!f.type.of.empty())
      if (const std::string why = adopt_project_type(to, from, f.type.of); !why.empty()) return why;
  to.structs.push_back(copy);
  return {};
}
/// Where a type of the project that `g` has is not the same in another Blueprint of the level: "" when nowhere.
std::string project_type_conflict(const game::Level& l, u32 owner, const logic::Graph& g) {
  for (const auto& [id, name, other] : other_blueprints(l, owner)) {
    for (const logic::EnumType& e : g.enums)
      if (const logic::EnumType* x = e.project ? other->enumeration(e.name) : nullptr; x && x->project && !logic::same_type(e, *x))
        return e.name + " is an enum of the project, and the Blueprint of " + name + " has it with other values: a type of the project is the same in every Blueprint";
    for (const logic::StructType& t : g.structs)
      if (const logic::StructType* x = t.project ? other->structure(t.name) : nullptr; x && x->project && !logic::same_type(t, *x))
        return t.name + " is a struct of the project, and the Blueprint of " + name + " has it with other fields: a type of the project is the same in every Blueprint";
  }
  return {};
}
/// Does `other` have a function this declaration describes: of that name, with inputs and
/// outputs of the same types in the same order?
bool declares(const logic::Function& f, const logic::Function& other) {
  auto same = [](const std::vector<logic::FunctionPin>& a, const std::vector<logic::FunctionPin>& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k) if (a[k].type != b[k].type || a[k].of != b[k].of) return false;
    return true;
  };
  return !other.external && f.name == other.name && same(f.inputs, other.inputs) && same(f.outputs, other.outputs);
}

/// Custom event names used anywhere in the level (for Call Event and transitions).
std::vector<std::string> event_names(const game::Level& l) {
  std::vector<std::string> out;
  auto scan = [&](const logic::Graph& g) {
    for (const logic::Node& n : g.nodes)
      if (n.type == "event.custom" || n.type == "flow.call") {
        const std::string name = n.param("name", "MyEvent");
        if (!name.empty() && std::find(out.begin(), out.end(), name) == out.end()) out.push_back(name);
      }
    for (const logic::StateMachine& m : g.machines)
      for (const logic::Transition& t : m.transitions)
        if (t.trigger == logic::TriggerKind::event && !t.event.empty() && std::find(out.begin(), out.end(), t.event) == out.end()) out.push_back(t.event);
  };
  scan(l.logic);
  for (const game::Actor& a : l.actors) if (a.logic) scan(*a.logic);
  std::sort(out.begin(), out.end());
  return out;
}

bool pin_of(const logic::Graph& g, u32 node, const std::string& pin, bool output, logic::PinDef& out) {
  const logic::Node* n = g.find(node);
  if (!n) return false;
  std::vector<logic::PinDef> in, outs;
  logic::library().pins(*n, g, in, outs);
  for (const logic::PinDef& p : output ? outs : in)
    if (p.id == pin) { out = p; return true; }
  return false;
}

/// The value of the setting "Type" with which the pin `pin` of a generic node (Select, Reroute,
/// the list nodes) is of type `want` - "" when it has no such setting or none of its types fits.
std::string type_setting_for(const logic::Graph& g, const logic::Node& node, const std::string& pin, bool output, const logic::TypeRef& want) {
  const logic::NodeDef* def = logic::library().find(node.type);
  if (!def) return {};
  for (const logic::ParamDef& pd : def->params) {
    if (pd.kind != logic::ParamDef::Kind::type || pd.key != "type") continue;
    for (const logic::TypeRef& t : logic::type_choices(pd, g)) {
      logic::Node probe = node;
      probe.set_param(pd.key, logic::type_text(t));
      std::vector<logic::PinDef> in, outs;
      logic::library().pins(probe, g, in, outs);
      for (const logic::PinDef& p : output ? outs : in)
        if (p.id == pin && logic::TypeRef{p.type, p.of} == want) return logic::type_text(t);
    }
  }
  return {};
}

/// The types the pins of a generic node could have, were its setting "Type" another - for the
/// pins that setting decides, and only while nothing is wired to any of them (the node takes the
/// type of its first wire: connect; a wire into an input replaces the one that is there, so that
/// one does not count for its own pin). Pin id -> the types' names, blank between them.
std::vector<std::pair<std::string, std::string>> generic_pins(const logic::Graph& g, const logic::Node& node, bool output) {
  std::vector<std::pair<std::string, std::string>> r;
  const logic::NodeDef* def = logic::library().find(node.type);
  if (!def) return r;
  const logic::ParamDef* type = nullptr;
  for (const logic::ParamDef& pd : def->params) if (pd.kind == logic::ParamDef::Kind::type && pd.key == "type") type = &pd;
  if (!type) return r;
  std::vector<logic::PinDef> in, out;
  logic::library().pins(node, g, in, out);
  std::vector<std::string> types_in(in.size()), types_out(out.size());
  std::vector<bool> typed_in(in.size(), false), typed_out(out.size(), false);
  auto same = [](const logic::PinDef& a, const logic::PinDef& b) { return a.type == b.type && a.of == b.of; };
  for (const logic::TypeRef& choice : logic::type_choices(*type, g)) {
    logic::Node probe = node;
    probe.set_param(type->key, logic::type_text(choice));
    std::vector<logic::PinDef> pin, pout;
    logic::library().pins(probe, g, pin, pout);
    if (pin.size() != in.size() || pout.size() != out.size()) return r;
    for (std::size_t i = 0; i < in.size(); ++i) {
      if (!same(pin[i], in[i])) typed_in[i] = true;
      types_in[i] += (types_in[i].empty() ? "" : " ") + logic::type_text({pin[i].type, pin[i].of});
    }
    for (std::size_t i = 0; i < out.size(); ++i) {
      if (!same(pout[i], out[i])) typed_out[i] = true;
      types_out[i] += (types_out[i].empty() ? "" : " ") + logic::type_text({pout[i].type, pout[i].of});
    }
  }
  std::size_t wired = 0;
  std::vector<std::size_t> into(in.size(), 0);
  for (const logic::Link& l : g.links) {
    for (std::size_t i = 0; i < in.size(); ++i)
      if (typed_in[i] && l.to == node.id && l.in == in[i].id) { ++wired; ++into[i]; }
    for (std::size_t i = 0; i < out.size(); ++i) if (typed_out[i] && l.from == node.id && l.out == out[i].id) ++wired;
  }
  for (std::size_t i = 0; i < (output ? out.size() : in.size()); ++i) {
    if (!(output ? typed_out[i] : typed_in[i]) || wired - (output ? 0 : into[i]) != 0) continue;
    r.push_back({output ? out[i].id : in[i].id, output ? types_out[i] : types_in[i]});
  }
  return r;
}

/// A generic node whose typed pins are all free takes the type a new wire needs: its setting
/// "Type" becomes `type` (values typed into those pins were of the old type and go). `replaced`:
/// the input the new wire goes into - the wire that is there goes anyway.
bool retype(logic::Graph& g, u32 id, const std::string& type, const std::string& replaced = {}) {
  logic::Node* node = g.find(id);
  if (!node || type.empty()) return false;
  logic::Node probe = *node;
  probe.set_param("type", type);
  std::vector<logic::PinDef> in_a, out_a, in_b, out_b;
  logic::library().pins(*node, g, in_a, out_a);
  logic::library().pins(probe, g, in_b, out_b);
  if (in_a.size() != in_b.size() || out_a.size() != out_b.size()) return false;
  std::vector<std::string> typed_in, typed_out;
  for (std::size_t k = 0; k < in_a.size(); ++k) if (in_a[k].type != in_b[k].type || in_a[k].of != in_b[k].of) typed_in.push_back(in_a[k].id);
  for (std::size_t k = 0; k < out_a.size(); ++k) if (out_a[k].type != out_b[k].type || out_a[k].of != out_b[k].of) typed_out.push_back(out_a[k].id);
  for (const logic::Link& l : g.links) {
    if (l.to == id && l.in != replaced && std::find(typed_in.begin(), typed_in.end(), l.in) != typed_in.end()) return false;
    if (l.from == id && std::find(typed_out.begin(), typed_out.end(), l.out) != typed_out.end()) return false;
  }
  node->values.erase(std::remove_if(node->values.begin(), node->values.end(), [&](const auto& v) { return std::find(typed_in.begin(), typed_in.end(), v.first) != typed_in.end(); }),
                     node->values.end());
  node->set_param("type", type);
  return true;
}

/// A node of a map or a set (Length, Has Key, Key At, Find) that a map or set comes into takes
/// its keys and values: its settings Key and Value become the map's (values typed into its key
/// and value pins were of the old types and go). False: it is no such node, or it is of these.
/// (The settings Key and Value with which the pin `pin` of a node of this kind is a map or a set of
/// type `want` - false: it has no such pin and settings. A set's node has no Value: `value` stays empty.)
bool keyed_params(const logic::NodeDef& def, const std::string& pin, const logic::TypeRef& want, std::string& key, std::string& value) {
  if (!logic::is_keyed(want.type) || pin != (want.type == logic::PinType::map ? "map" : "set")) return false;
  bool has_key = false, has_value = false;
  for (const logic::ParamDef& pd : def.params) {
    has_key = has_key || (pd.kind == logic::ParamDef::Kind::type && pd.key == "key" && pd.keys_only);
    has_value = has_value || (pd.kind == logic::ParamDef::Kind::type && pd.key == "value");
  }
  if (!has_key || (want.type == logic::PinType::map) != has_value) return false;
  key = logic::type_text(logic::key_type(want));
  value = has_value ? logic::type_text(logic::value_type(want)) : std::string();
  return true;
}
bool keyed_settings(logic::Graph& g, u32 id, const std::string& pin, const logic::TypeRef& want) {
  logic::Node* node = g.find(id);
  const logic::NodeDef* def = node ? logic::library().find(node->type) : nullptr;
  std::string key, value;
  if (!def || !keyed_params(*def, pin, want, key, value)) return false;
  const bool has_value = !value.empty();
  if (node->param("key", "i64") == key && (!has_value || node->param("value", "f32") == value)) return false;
  node->set_param("key", key);
  if (has_value) node->set_param("value", value);
  node->values.erase(std::remove_if(node->values.begin(), node->values.end(), [](const auto& v) { return v.first == "key" || v.first == "value"; }), node->values.end());
  return true;
}

// ---------------------------------------------------------------------------
// numbers in the editor: the types offered, the literals typed in, the conversions put on a wire

/// "a u8", "an f32", "an i64", "a Vector", "an Actor": a name with its article, as it is said.
std::string an(const std::string& name) {
  const char c = name.empty() ? 'x' : name[0];
  const bool vowel = c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || (c == 'f' && name.size() > 1 && std::isdigit(static_cast<unsigned char>(name[1])));
  return (vowel ? "an " : "a ") + name;
}

/// The smallest and the largest value of a whole number type, written out ("-128", "127").
std::pair<std::string, std::string> integer_range(logic::PinType t) {
  const logic::num::Kind k = logic::kind_of(t);
  const int w = logic::num::width(k);
  if (!logic::num::is_signed(k)) return {"0", std::to_string(w == 64 ? ~u64{0} : (u64{1} << w) - 1)};
  const i64 hi = w == 64 ? std::numeric_limits<i64>::max() : static_cast<i64>((u64{1} << (w - 1)) - 1);
  return {std::to_string(-hi - 1), std::to_string(hi)};
}

/// How the editor names a type in its menus: the two defaults by what they are for - "Float (f32)", "Int (i64)" -,
/// every other number by its C++ name, the rest as logic::type_label names them.
std::string editor_type_label(const logic::TypeRef& t) {
  const logic::PinType e = logic::element_of(t.type);
  const char* list = logic::is_list(t.type) ? " List" : "";
  if (e == logic::kFloat && t.of.empty()) return std::string("Float") + list + " (f32)";
  if (e == logic::kInt && t.of.empty()) return std::string("Int") + list + " (i64)";
  return logic::type_label(t);
}

/// Why a value of the number type `from` can change on its way to `to` (static_cast, numbers.h) - "" when every
/// value of `from` is a value of `to`. Said on the Cast node the editor puts on a wire, so nothing is lost unseen.
std::string cast_loss(logic::PinType from, logic::PinType to) {
  using logic::num::Kind;
  const Kind f = logic::kind_of(from), t = logic::kind_of(to);
  if (f == t || f == Kind::boolean) return {};
  const std::string a = logic::pin_type_name(from), b = logic::pin_type_name(to);
  if (t == Kind::boolean) return "every " + a + " but 0 becomes true";
  if (logic::num::is_floating(f) && logic::num::is_floating(t))
    return logic::num::width(t) < logic::num::width(f) ? "an f64 becomes the nearest f32: fewer digits, beyond the ends of f32 ±inf" : std::string();
  if (logic::num::is_floating(f)) {
    const auto [lo, hi] = integer_range(to);
    return an(a) + " goes toward zero to " + an(b) + ": its fraction is lost, NaN gives 0, beyond " + lo + " ... " + hi + " the nearer end";
  }
  // (a whole number: how many bits its value has - its width, less the sign's)
  const int bits = logic::num::width(f) - (logic::num::is_signed(f) ? 1 : 0);
  if (logic::num::is_floating(t)) {
    const int digits = t == Kind::f32 ? 24 : 53;
    return bits > digits ? an(a) + " beyond 2^" + std::to_string(digits) + " becomes the nearest " + b : std::string();
  }
  const bool fs = logic::num::is_signed(f), ts = logic::num::is_signed(t);
  const int room = logic::num::width(t) - (ts ? 1 : 0);
  if ((fs && !ts) || room < bits) {
    const auto [lo, hi] = integer_range(to);
    if (fs && !ts && logic::num::width(t) >= logic::num::width(f)) return "a negative " + a + " goes round to a large " + b + " (-1 becomes " + hi + ")";
    return an(b) + " holds " + lo + " ... " + hi + ": beyond, the " + a + " keeps its low " + std::to_string(logic::num::width(t)) + " bits and goes round";
  }
  return {};
}

/// A value typed in for the type `t` as the document holds it - or why it is none. A number is read in its type,
/// exactly: a whole number must be one its type holds (300 is no u8, 1.5 no i32), a floating number is the nearest
/// value of its type (strtof for an f32 - never by way of a double; 0.5f says the same), inf, -inf and nan too.
/// Texts are how a value is typed in on the page (i64 and u64 beyond 2^53 are more than a number of JavaScript
/// holds); a list's items are each read so (the place of a bad one is said).
bool read_literal(const json::Value& in, const logic::TypeRef& t, const logic::Graph& g, json::Value& out, std::string& why) {
  if (logic::is_list(t.type) && (logic::is_number(logic::element_of(t.type)) || logic::element_of(t.type) == logic::PinType::vector) && in.is_array()) {
    json::Value items = json::Value::array();
    for (std::size_t k = 0; k < in.size(); ++k) {
      json::Value x;
      if (!read_literal(in[k], {logic::element_of(t.type), t.of}, g, x, why)) { why = "item " + std::to_string(k) + ": " + why; return false; }
      items.push(std::move(x));
    }
    out = logic::Value::from_json(items, t.type, g, t.of).to_json(g, t);
    return true;
  }
  if (t.type == logic::PinType::vector && in.is_array()) {
    // (a Vector: three f32, each read as one - X, Y, Z)
    json::Value parts = json::Value::array();
    for (std::size_t k = 0; k < 3; ++k) {
      json::Value x;
      if (!read_literal(k < in.size() ? in[k] : json::Value(0.0), {logic::PinType::f32, {}}, g, x, why)) { why = std::string(k == 0 ? "X" : k == 1 ? "Y" : "Z") + ": " + why; return false; }
      if (!x.is_number()) { why = std::string(k == 0 ? "X" : k == 1 ? "Y" : "Z") + ": a Vector's X, Y and Z are finite numbers here"; return false; }
      parts.push(x);
    }
    out = logic::Value::from_json(parts, t.type, g, t.of).to_json(g, t);
    return true;
  }
  if (!logic::is_number(t.type) || t.type == logic::PinType::boolean) {
    out = logic::Value::from_json(in, t.type, g, t.of).to_json(g, t);
    return true;
  }
  const std::string name = logic::pin_type_name(t.type);
  std::string text;
  if (in.is_string()) text = in.as_string();
  else if (in.is_number()) {
    const f64 d = in.as_number();
    if (logic::is_integer(t.type)) {
      if (d != std::floor(d)) { why = an(name) + " is a whole number"; return false; }
      if (std::fabs(d) > 9007199254740992.0) { why = "a whole number beyond 2^53 is written as its digits (a text), not as a number of JSON"; return false; }
      text = std::to_string(static_cast<i64>(d));
    } else {
      out = logic::Value::from_json(in, t.type, g, t.of).to_json(g, t);
      return true;
    }
  } else if (in.is_bool()) {
    why = an(name) + " is a number - true and false are a bool's (a Cast node makes one of the other)";
    return false;
  } else {
    why = an(name) + " is a number";
    return false;
  }
  while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) text.pop_back();
  std::size_t lead = 0;
  while (lead < text.size() && std::isspace(static_cast<unsigned char>(text[lead]))) ++lead;
  text = text.substr(lead);
  if (text.empty()) text = "0";
  if (logic::is_floating(t.type)) {
    std::string body = text;
    // (0.5f: an f32 literal as C++ writes it - the f after a digit or a point, not the f of "inf")
    if (t.type == logic::PinType::f32 && body.size() > 1 && (body.back() == 'f' || body.back() == 'F') && (std::isdigit(static_cast<unsigned char>(body[body.size() - 2])) || body[body.size() - 2] == '.') &&
        body.find_first_of("xX") == std::string::npos)
      body.pop_back();
    const logic::scan::Real r = logic::scan::real(body, t.type == logic::PinType::f32);
    if (r.end != body.size() || body.find('(') != std::string::npos) {
      why = "\"" + text + "\" is no " + name + ": a number such as 0.5, -3, 1e-3, inf or nan";
      return false;
    }
    out = logic::Value::from_json(json::Value(body), t.type, g, t.of).to_json(g, t);   // (the same reader: strtof / strtod)
    return true;
  }
  // a whole number: decimal digits, a minus in front for a signed type - and nothing its type cannot hold
  const auto [lo, hi] = integer_range(t.type);
  const std::string range = an(name) + " is a whole number from " + lo + " to " + hi;
  std::size_t at = text[0] == '-' || text[0] == '+' ? 1 : 0;
  if (at == text.size() || text.find_first_not_of("0123456789", at) != std::string::npos) { why = "\"" + text + "\" is no " + name + ": " + range; return false; }
  bool fits = false;
  logic::with_number(t.type, [&](auto tag) {
    using T = typename decltype(tag)::type;
    if constexpr (!std::is_same_v<T, bool> && !std::is_floating_point_v<T>) {
      T x{};
      const char* begin = text.data() + (text[0] == '+' ? 1 : 0);
      const auto [end, ec] = std::from_chars(begin, text.data() + text.size(), x);
      fits = ec == std::errc() && end == text.data() + text.size();
    }
    return 0;
  });
  if (!fits) { why = "\"" + text + "\" is no " + name + ": " + range; return false; }
  out = logic::Value::from_json(json::Value(text[0] == '+' ? text.substr(1) : text), t.type, g, t.of).to_json(g, t);
  return true;
}

/// A node the editor puts on a wire that carries one type into a pin of another: what converts it, in the open
/// (no wire converts - kern/docs/blueprint_cpp.md "Zahl-Typen"). Empty `type`: there is none, `why` says so.
struct Conversion {
  std::string type, in = "a";             // the node and its input the wire comes into
  std::vector<std::pair<std::string, std::string>> params;
  std::string why;
};
Conversion conversion_for(const logic::TypeRef& from, const logic::TypeRef& to) {
  using logic::PinType;
  Conversion c;
  const std::string a = logic::type_text(from), b = logic::type_text(to);
  if (logic::is_number(from.type) && logic::is_number(to.type)) {
    c.type = "num.cast";
    c.params = {{"from", logic::pin_type_name(from.type)}, {"to", logic::pin_type_name(to.type)}};
    return c;
  }
  if (logic::is_number(from.type) && to.type == PinType::text) {
    c.type = "num.text";
    c.params = {{"type", logic::pin_type_name(from.type)}};
    return c;
  }
  if (logic::is_list(from.type) && from.of.empty() && to.type == PinType::text && logic::element_of(from.type) != PinType::object) {
    c.type = "list.text";
    c.in = "list";
    return c;   // (its Type follows the list it gets: connect)
  }
  if (logic::is_number(from.type) && from.type != PinType::boolean && to.type == PinType::vector) {
    c.type = "vector.make";
    c.in = "x";
    return c;
  }
  if (from.type == PinType::vector && logic::is_number(to.type)) c.why = "Break Vector gives a Vector's X, Y and Z";
  else if (from.type == PinType::text && logic::is_number(to.type)) c.why = "a text is read as a number by Scan Text or Text to u64";
  else if (logic::is_list(from.type) && logic::is_list(to.type)) c.why = "a list is converted item by item: For Each and Cast";
  else c.why = "no node converts the one into the other";
  c.why = an(a) + " cannot go into " + an(b) + " pin: " + c.why;
  return c;
}

bool connect(logic::Graph& g, u32 from, const std::string& out, u32 to, const std::string& in, std::string& error, u32* inserted = nullptr);

/// The types a setting Type of the node offers: those the node takes - a node of numbers its numbers (Add: i8 ...
/// f64; Shift Left: the whole ones), the defaults Float (f32) and Int (i64) first, the other widths below them.
std::vector<logic::TypeRef> offered_types(const logic::NodeDef& def, const logic::ParamDef& pd, const logic::Node& n, const logic::Graph& g) {
  std::vector<logic::TypeRef> all = logic::type_choices(pd, g), r;
  if (!def.problem) return all;
  auto takes = [&](const std::string& type) {
    logic::Node probe = n;
    probe.id = 0;   // (no wire reaches it: what the node takes, not what is wired)
    probe.set_param(pd.key, type);
    return def.problem(probe, g).empty();
  };
  if (!takes(pd.def)) return all;   // (it says something else of itself: every type is offered)
  for (const logic::TypeRef& t : all)
    if (takes(logic::type_text(t))) r.push_back(t);
  return r.empty() ? all : r;
}

/// Puts the conversion of a wire from (from, out) into (to, in) between them, in the open: the node(s) halfway, the
/// wires through them. A number into a Vector: Make Vector with the number in X, Y and Z (a Cast to f32 first where
/// it is no f32). The id of the node the wire goes into last is `inserted`.
bool put_conversion(logic::Graph& g, u32 from, const std::string& out, const logic::TypeRef& have, u32 to, const std::string& in, const logic::TypeRef& want,
                    std::string& error, u32* inserted) {
  Conversion c = conversion_for(have, want);
  if (c.type.empty()) { error = c.why; return false; }
  const logic::Node* a = g.find(from);
  const logic::Node* b = g.find(to);
  if (!a || !b) { error = "no such node"; return false; }
  auto add = [&](const std::string& type, const std::vector<std::pair<std::string, std::string>>& params, f32 x, f32 y) {
    logic::Node n;
    n.id = g.next_id++;
    n.type = type;
    n.x = std::round(x / 8) * 8;
    n.y = std::round(y / 8) * 8;
    for (const auto& [k, v] : params) n.set_param(k, v);
    g.nodes.push_back(n);
    return n.id;
  };
  const f32 mx = (a->x + b->x) / 2, my = (a->y + b->y) / 2;
  if (c.type == "vector.make") {
    u32 source = from;
    std::string source_pin = out;
    if (have.type != logic::PinType::f32) {
      source = add("num.cast", {{"from", logic::pin_type_name(have.type)}, {"to", "f32"}}, mx - 200, my);
      source_pin = "result";
      if (!connect(g, from, out, source, "a", error)) return false;
    }
    const u32 make = add("vector.make", {}, mx, my);
    for (const char* p : {"x", "y", "z"})
      if (!connect(g, source, source_pin, make, p, error)) return false;
    if (!connect(g, make, "result", to, in, error)) return false;
    if (inserted) *inserted = make;
    return true;
  }
  const u32 id = add(c.type, c.params, mx, my);
  if (!connect(g, from, out, id, c.in, error)) return false;   // (a list into List to Text: its Type follows)
  if (!connect(g, id, "result", to, in, error)) return false;
  if (inserted) *inserted = id;
  return true;
}

/// Adds a link with the editor's rules: an exec output and a data input take one wire
/// (the new one replaces the old); exec inputs and data outputs fan in / out freely. A generic
/// node nothing is wired to yet takes the type of the wire (a Text List into Length makes it a
/// Length of a Text List; an f32 into a fresh Add makes it an Add of f32). A wire of another
/// type than its input gets the node that converts it, in the open (`inserted`: its id) - f32 into
/// i64 a Cast "f32 → i64", a number into a Text To Text -, or it is refused with the reason.
bool connect(logic::Graph& g, u32 from, const std::string& out, u32 to, const std::string& in, std::string& error, u32* inserted) {
  if (from == to) { error = "a node cannot connect to itself"; return false; }
  logic::PinDef po, pi;
  if (!pin_of(g, from, out, true, po) || !pin_of(g, to, in, false, pi)) { error = "no such pin"; return false; }
  if ((po.type == logic::PinType::exec) != (pi.type == logic::PinType::exec)) { error = "exec pins connect to exec pins only"; return false; }
  auto type_of = [](const logic::PinDef& p) { return logic::TypeRef{p.type, p.of}; };
  if (!(type_of(po) == type_of(pi)) && po.type != logic::PinType::exec) {
    // (first the node the wire goes into, then the one it comes from)
    if (keyed_settings(g, to, in, type_of(po))) pin_of(g, to, in, false, pi);  // (a map into a map's node: its keys and values)
    else if (const logic::Node* n = g.find(to); n && retype(g, to, type_setting_for(g, *n, in, false, type_of(po)), in)) pin_of(g, to, in, false, pi);
    else if (const logic::Node* m = g.find(from); m && !logic::fits(type_of(po), type_of(pi)) && retype(g, from, type_setting_for(g, *m, out, true, type_of(pi)))) pin_of(g, from, out, true, po);
  }
  if (!logic::fits(type_of(po), type_of(pi))) {
    // (another type: the node that converts it, on the wire - or the reason there is none)
    auto declared = [](logic::PinType t) { return logic::element_of(t) == logic::PinType::enumeration || logic::element_of(t) == logic::PinType::structure; };
    if (po.type != logic::PinType::exec && !declared(po.type) && !declared(pi.type))
      return put_conversion(g, from, out, type_of(po), to, in, type_of(pi), error, inserted);
    error = "a " + logic::type_text(type_of(po)) + " cannot go into a " + logic::type_text(type_of(pi)) + " pin";
    return false;
  }
  auto& links = g.links;
  if (po.type == logic::PinType::exec) links.erase(std::remove_if(links.begin(), links.end(), [&](const logic::Link& l) { return l.from == from && l.out == out; }), links.end());
  else links.erase(std::remove_if(links.begin(), links.end(), [&](const logic::Link& l) { return l.to == to && l.in == in; }), links.end());
  links.push_back({from, out, to, in});
  return true;
}

/// The first pin of `node` that `type` can connect to (output side when `want_output`).
std::string compatible_pin(const logic::Graph& g, const logic::Node& node, const logic::TypeRef& type, bool want_output) {
  std::vector<logic::PinDef> in, out;
  logic::library().pins(node, g, in, out);
  for (const logic::PinDef& p : want_output ? out : in) {
    if ((p.type == logic::PinType::exec) != (type.type == logic::PinType::exec)) continue;
    if (want_output ? logic::fits({p.type, p.of}, type) : logic::fits(type, {p.type, p.of})) return p.id;
  }
  return {};
}

/// Blueprint draws some nodes compact: a variable getter is a pill with the variable's name, an
/// operator a small box with its symbol. "" = a normal node with a title bar.
std::string compact_symbol(const logic::Node& n) {
  static const std::pair<const char*, const char*> kOps[] = {
      {"math.add", "+"}, {"math.sub", "\u2212"}, {"math.mul", "\u00d7"}, {"math.div", "\u00f7"}, {"math.mod", "%"}, {"math.negate", "\u2212x"},
      {"math.abs", "|x|"}, {"math.min", "MIN"}, {"math.max", "MAX"}, {"math.sqrt", "\u221a"}, {"math.pow", "x\u207f"}, {"logic.and", "AND"},
      {"logic.or", "OR"}, {"logic.xor", "XOR"}, {"logic.not", "NOT"}, {"vector.dot", "\u00b7"}, {"vector.scale", "\u00d7"}, {"text.append", "&"},
      {"text.equal", "=="}};
  for (const auto& [t, sym] : kOps)
    if (n.type == t) return sym;
  if (n.type == "math.compare") {
    const std::string op = n.param("op", "<");
    return op == "!=" ? "\u2260" : op == "<=" ? "\u2264" : op == ">=" ? "\u2265" : op;
  }
  if (n.type == "var.get") return "get";
  if (n.type == "flow.reroute" || n.type == "value.reroute") return "reroute";
  return {};
}

}  // namespace

// ---------------------------------------------------------------------------
// view


namespace {
/// Where the libraries a Blueprint of `level` uses are found (kern/docs/project.md 5.): in the level, by the name
/// of the actor that has them, then in the project (null: the level alone - the program's Blueprints, seen as a level).
logic::Blueprints blueprints_of(const game::Level& level, const game::Project* project) {
  return logic::with_engine(game::world_blueprints(level, project));  // (and the engine's own libraries: kern/logic/engine_libraries.h)
}
/// Does a Blueprint use the library `name` - itself, or through the libraries it uses?
bool uses_library(const game::Level& level, const game::Project* project, const logic::Graph& g, const std::string& name, int depth = 0) {
  if (depth > 32) return false;
  const logic::Blueprints all = blueprints_of(level, project);
  for (const std::string& used : g.uses) {
    if (used == name) return true;
    if (const logic::Graph* l = all(used); l && l != &g && uses_library(level, project, *l, name, depth + 1)) return true;
  }
  return false;
}
/// After an edit of the Blueprint `g` of the actor `owner`: it holds of its libraries what it
/// uses of them now (their types, how the functions look that it calls) - and the Blueprints of
/// the level that use it follow. False: one of them cannot (`error` says which, and why) - the
/// edit does not happen.
bool libraries_follow(game::Level& level, const game::Project* project, u32 owner, logic::Graph& g, std::string& error) {
  const logic::Blueprints all = blueprints_of(level, project);
  bool holds = !g.uses.empty();
  for (const logic::Function& f : g.functions) holds = holds || !f.from.empty();
  for (const logic::EnumType& e : g.enums) holds = holds || !e.from.empty();
  for (const logic::StructType& t : g.structs) holds = holds || !t.from.empty();
  if (holds && !logic::take_libraries(g, all, &error)) return false;
  const game::Actor* self = owner ? level.find(owner) : nullptr;
  if (!self) return true;
  for (game::Actor& a : level.actors) {
    if (a.id == owner || !a.logic || !uses_library(level, project, *a.logic, self->name)) continue;
    std::string why;
    if (!logic::take_libraries(*a.logic, all, &why)) {
      error = "the Blueprint of " + a.name + " uses this one: " + why;
      return false;
    }
  }
  return true;
}
/// What is wrong with a Blueprint. One that uses libraries is judged as the one graph it is
/// with them (logic::link): the rules that follow calls see what a library's function does.
std::vector<logic::Issue> linked_issues(const game::Level& level, const game::Project* project, const logic::Graph& g) {
  if (g.uses.empty()) return logic::check(g);
  logic::Graph whole;
  std::string why;
  if (!logic::link(g, blueprints_of(level, project), whole, &why)) {
    std::vector<logic::Issue> issues = logic::check(g);
    issues.push_back({0, true, why});
    return issues;
  }
  std::vector<logic::Issue> issues = logic::check(whole);
  // (what is wrong inside a library is said where the library is open: here only what is this Blueprint's)
  std::erase_if(issues, [&](const logic::Issue& i) { return i.node != 0 && !g.find(i.node); });
  return issues;
}
std::vector<logic::Issue> whole_issues(const game::Level& level, const game::Project* project, const logic::Graph& g) {
  std::vector<logic::Issue> issues = linked_issues(level, project, g);
  // (a Blueprint of a world - one with a project to find libraries in; the program's has none: an export is the
  // program's - the level is refused with one, Play too; here it is said where the Blueprint is open)
  if (project)
    if (const std::string e = game::world_export(g); !e.empty()) issues.push_back({0, true, e});
  return issues;
}
/// The Blueprint as an edit sees it: with every type of its libraries - those it does not use
/// yet among them (a Blueprint holds of its libraries what it uses; an edit may pick the rest).
logic::Graph with_library_types(const game::Level& level, const game::Project* project, const logic::Graph& g) {
  if (g.uses.empty()) return g;
  logic::Graph whole = g;
  if (!logic::take_libraries(whole, blueprints_of(level, project), nullptr, true)) return g;
  whole.functions = g.functions;
  return whole;
}
}  // namespace
json::Value Studio::logic_json() const {
  json::Value o = json::Value::object();
  o.set("open", logic_open_);
  o.set("owner", logic_owner_);
  o.set("program", logic_program_);  // a program Blueprint (of the project), not one of the level
  const game::Actor* owner = logic_owner_ ? logic_level().find(logic_owner_) : nullptr;
  o.set("ownerName", logic_owner_ ? (owner ? owner->name : std::string("?")) : std::string("Level"));
  o.set("view", logic_view_);
  o.set("paused", paused_);
  o.set("playing", play_ != PlayState::editing);
  o.set("breakpoints", static_cast<u32>(logic_breakpoints_.size()));
  // Owners to switch between: the level and every actor with a graph - or the program's Blueprints.
  json::Value owners = json::Value::array();
  if (!logic_program_) {
    json::Value lv = json::Value::object();
    lv.set("id", 0);
    lv.set("name", "Level");
    lv.set("nodes", static_cast<u32>(logic_level().logic.nodes.size()));
    owners.push(std::move(lv));
  }
  for (const game::Actor& a : logic_level().actors)
    if (a.logic) {
      json::Value x = json::Value::object();
      x.set("id", a.id);
      x.set("name", a.name);
      x.set("nodes", static_cast<u32>(a.logic->nodes.size()));
      owners.push(std::move(x));
    }
  o.set("owners", std::move(owners));
  const logic::Graph* g = graph_of(logic_level(), logic_owner_);
  o.set("exists", g != nullptr);
  json::Value events = json::Value::array();
  for (const std::string& e : event_names(logic_level())) events.push(e);
  o.set("events", std::move(events));
  if (!g) return o;
  const logic::Library& lib = logic::library();
  std::unordered_map<u32, std::vector<const logic::Issue*>> by_node;
  std::vector<logic::Issue> issues = whole_issues(logic_level(), logic_project(), *g);
  {
    // A function of another Blueprint: one of this level should have it as it is declared here.
    const auto others = other_blueprints(logic_level(), logic_owner_);
    for (const logic::Function& f : g->functions) {
      if (!f.external) continue;
      bool found = false, named = false;
      for (const auto& [id, name, other] : others)
        if (const logic::Function* of = other->function(f.name)) {
          named = named || !of->external;
          found = found || declares(f, *of);
        }
      if (!found)
        issues.push_back({0, false, "no other Blueprint of this level has " + std::string(named ? "the function \"" + f.name + "\" with these inputs and outputs" : "a function \"" + f.name + "\"") +
                                        ": a call of it gives the outputs' defaults"});
    }
  }
  for (const logic::Issue& i : issues) by_node[i.node].push_back(&i);
  json::Value nodes = json::Value::array();
  // (asked for every pin and every call below - once here, so a Blueprint of tens of thousands of nodes opens:
  // which pins have a wire, and the function each node stands in)
  auto pin_key = [](u32 node, const std::string& pin, bool output) { return std::to_string(node) + (output ? ">" : "<") + pin; };
  std::unordered_set<std::string> wired;
  wired.reserve(g->links.size() * 2);
  for (const logic::Link& l : g->links) {
    wired.insert(pin_key(l.from, l.out, true));
    wired.insert(pin_key(l.to, l.in, false));
  }
  const std::unordered_map<u32, std::string> body_names = g->bodies(lib);
  // Sheets: the view shows one function's body at a time, and what stands in no function - the events - on a sheet of
  // its own (""), whatever the size of the Blueprint: one way to see a Blueprint, as in any Blueprint editor. Which
  // sheet is open is kept for each owner (logic_sheets_): back at a Blueprint, its sheet is open again - never one
  // of the same name of another Blueprint. What the page needs of the nodes it does not get is counted here (counts)
  // or asked (logicFind, logicReveal).
  const auto open = logic_sheets_.find(logic_owner_);
  std::string sheet = open == logic_sheets_.end() ? std::string() : open->second;
  if (!sheet.empty() && !g->function(sheet)) sheet.clear();   // (renamed or removed: the events)
  const auto loose_of = logic_loose_.find(logic_owner_);
  auto shown = [&](u32 id) {
    const auto it = body_names.find(id);
    if (it != body_names.end()) return it->second == sheet;
    // (a node no function has by its wires: on the sheet it was put on in this session, else with the events)
    if (loose_of != logic_loose_.end())
      if (const auto put = loose_of->second.find(id); put != loose_of->second.end() && g->function(put->second)) return put->second == sheet;
    return sheet.empty();
  };
  // (lists of choices that are the same for every node of the Blueprint - its functions, variables, machines,
  // the level's events and sounds, the types a parameter takes: said once, in "choices"; a parameter names its list)
  json::Value shared = json::Value::object();
  std::unordered_map<const logic::ParamDef*, std::string> type_lists;
  std::unordered_map<std::string, u32> calls, variable_refs, type_refs;
  for (const logic::Node& n : g->nodes) {
    const logic::NodeDef* d = lib.find(n.type);
    if (n.type == "func.call" || n.type == "func.pure" || n.type == "func.call_on") ++calls[n.param("function")];
    if (d)
      for (const logic::ParamDef& pd : d->params) {
        if (pd.kind == logic::ParamDef::Kind::type) ++type_refs[n.param(pd.key, pd.def)];
        if (pd.key == "variable" && n.param("scope").empty()) ++variable_refs[n.param("variable")];
      }
  }
  for (const logic::Node& n : g->nodes) {
    if (!shown(n.id)) continue;
    const logic::NodeDef* d = lib.find(n.type);
    json::Value x = json::Value::object();
    x.set("id", n.id);
    x.set("type", n.type);
    x.set("title", lib.title(n));
    x.set("x", static_cast<f64>(n.x));
    x.set("y", static_cast<f64>(n.y));
    if (!n.comment.empty()) x.set("comment", n.comment);
    if (const std::string sym = compact_symbol(n); !sym.empty()) x.set("compact", sym);
    if (d) {
      // (the type of a node of numbers, said on the node - also on a compact "+": numberType; a Cast that can change
      // a value says how: loss)
      for (const logic::ParamDef& pd : d->params)
        if (pd.kind == logic::ParamDef::Kind::type && pd.key == "type") {
          logic::PinType t;
          if (logic::pin_type_from_name(n.param("type", pd.def), t) && logic::is_number(t) && !logic::is_list(t)) x.set("numberType", logic::pin_type_name(t));
        }
      if (n.type == "num.cast") {
        logic::PinType a, b;
        if (logic::pin_type_from_name(n.param("from", "f32"), a) && logic::pin_type_from_name(n.param("to", "f32"), b) && logic::is_number(a) && logic::is_number(b))
          if (const std::string loss = cast_loss(a, b); !loss.empty()) x.set("loss", loss);
      }
      if (n.type == "math.formula") {
        // (what the formula's error says and where in its text: the page marks the place in the text)
        const std::string text = n.param("formula", "a + b"), types = n.param("types", "f32 f32");
        const logic::Formula& f = logic::Formula::cached(text, types);
        if (!f.ok()) {
          std::string why = f.error();
          const std::size_t at = why.rfind(" (at ");
          if (at != std::string::npos && why.back() == ')') {
            x.set("formulaAt", std::atof(why.c_str() + at + 5));
            why = why.substr(0, at);
          }
          x.set("formulaError", why);
        }
      }
    }
    for (const auto& bp : logic_breakpoints_)
      if (bp.first == logic_owner_ && bp.second == n.id) x.set("breakpoint", true);
    if (paused_ && logic_break_hit_.first == logic_owner_ && logic_break_hit_.second == n.id) x.set("hit", true);
    if (d) {
      x.set("kind", kind_name(d->kind));
      x.set("category", d->category);
      x.set("help", d->help);
      if (d->color) x.set("color", d->color);
      if (d->latent) x.set("latent", true);
    } else {
      x.set("kind", "action");
    }
    std::vector<logic::PinDef> in, out;
    lib.pins(n, *g, in, out);
    auto pins = [&](const std::vector<logic::PinDef>& v, bool output) {
      json::Value arr = json::Value::array();
      const std::vector<std::pair<std::string, std::string>> generic = generic_pins(*g, n, output);
      for (const logic::PinDef& p : v) {
        if (p.hidden) continue;
        json::Value pj = json::Value::object();
        pj.set("id", p.id);
        pj.set("label", p.label);
        pj.set("type", logic::pin_type_name(p.type));
        if (!p.of.empty() && p.of != logic::kAnyWidth) pj.set("of", p.of);
        // (a pin that takes the type of the first wire: the types it can be)
        for (const auto& [id, types] : generic) if (id == p.id) pj.set("generic", types);
        if (p.of == logic::kAnyWidth) {  // (it reads a list of Ints of each width: List<Int>, List<U8> ...)
          std::string types;
          for (logic::PinType t : {logic::PinType::i64, logic::PinType::i32, logic::PinType::i16, logic::PinType::i8, logic::PinType::u64, logic::PinType::u32, logic::PinType::u16, logic::PinType::u8})
            types += (types.empty() ? "" : " ") + std::string(logic::pin_type_name(logic::list_of(t)));
          pj.set("generic", types);
        }
        pj.set("linked", wired.count(pin_key(n.id, p.id, output)) != 0);
        if (!output && p.type != logic::PinType::exec) {
          const json::Value* v = n.value(p.id);
          pj.set("value", logic::Value::from_json(v ? *v : p.def, p.type, *g, p.of).to_json(*g, p.of));
        }
        arr.push(std::move(pj));
      }
      return arr;
    };
    x.set("inputs", pins(in, false));
    x.set("outputs", pins(out, true));
    if (d && !d->params.empty()) {
      json::Value params = json::Value::array();
      for (const logic::ParamDef& pd : d->params) {
        // (the function a node's local variable belongs to: shown on the nodes that have one)
        if (pd.kind == logic::ParamDef::Kind::scope && n.param(pd.key).empty()) continue;
        json::Value pj = json::Value::object();
        pj.set("key", pd.key);
        pj.set("label", pd.label);
        pj.set("kind", param_kind_name(pd.kind));
        pj.set("value", n.param(pd.key, pd.def));
        json::Value choices = json::Value::array();
        auto choice = [&](const std::string& v, const std::string& label) {
          json::Value c = json::Value::array();
          c.push(v);
          c.push(label);
          choices.push(std::move(c));
        };
        using K = logic::ParamDef::Kind;
        // (a list every node of the Blueprint shares: built the first time a node asks, named ever after)
        std::string list;
        if (pd.kind == K::machine) list = "machine";
        if (pd.kind == K::function) list = "function";
        if (pd.kind == K::event) list = "event";
        if (pd.kind == K::sound) list = "sound";
        if (pd.kind == K::actor_template) list = "template";
        if (pd.kind == K::variable && !g->function(n.param("scope"))) list = "variable";
        if (pd.kind == K::type) list = type_lists.try_emplace(&pd, "type" + std::to_string(type_lists.size())).first->second;
        if (!list.empty() && shared.has(list)) {
          pj.set("list", list);
          params.push(std::move(pj));
          continue;
        }
        if (pd.kind == K::choice)
          for (std::size_t i = 0; i + 1 < pd.choices.size(); i += 2) choice(pd.choices[i], pd.choices[i + 1]);
        if (pd.kind == K::type)
          for (const logic::TypeRef& t : offered_types(*d, pd, n, *g)) choice(logic::type_text(t), editor_type_label(t));
        if (pd.kind == K::variable) {
          // (the variables of the Blueprint - or, for a node of a function's local variables, those)
          const logic::Function* of = g->function(n.param("scope"));
          auto offer = [&](const std::string& name, logic::PinType type, const std::string& type_of) {
            choice(name, name + " (" + logic::type_label(logic::TypeRef{type, type_of}) + ")");
          };
          if (of) for (const logic::FunctionPin& l : of->locals) offer(l.name, l.type, l.of);
          else for (const logic::Variable& v : g->variables) offer(v.name, v.type, v.of);
        }
        if (pd.kind == K::field) {
          // (the fields of the struct the node names: Find Row)
          if (const logic::StructType* s = g->structure(n.param("struct")))
            for (const logic::StructField& f : s->fields) choice(f.name, f.name + " (" + logic::type_label(f.type) + ")");
        }
        if (pd.kind == K::machine)
          for (const logic::StateMachine& m : g->machines) choice(m.name, m.name);
        if (pd.kind == K::function)
          for (const logic::Function& f : g->functions) choice(f.name, f.name);
        if (pd.kind == K::state) {
          if (n.type == "event.state_enter" || n.type == "event.state_exit") choice("any", "Any state");
          if (const logic::StateMachine* m = g->machine(n.param("machine", "State Machine")))
            for (const logic::State& s : m->states) choice(s.name, s.name);
        }
        if (pd.kind == K::event)
          for (const std::string& e : event_names(logic_level())) choice(e, e);
        if (pd.kind == K::actor_template)   // (the level's templates: what Spawn Actor can copy)
          for (const game::Actor& a : level_.actors) if (a.is_template) choice(a.name, a.name);
        if (pd.kind == K::sound) {
          for (const audio::BuiltinSound& b : audio::builtin_sounds()) choice(std::string("builtin:") + b.name, b.label);
          for (const game::SoundAsset& sa : level_.sounds) choice(sa.id, sa.name);
        }
        if (list.empty()) pj.set("choices", std::move(choices));
        else { shared.set(list, std::move(choices)); pj.set("list", list); }
        params.push(std::move(pj));
      }
      // A call of a function with reference inputs: for each, the place the call gives it - a
      // variable (of the Blueprint, or of the function the call stands in) and the path from it.
      if (const logic::Function* called = n.type == "func.call" || n.type == "func.pure" ? g->function(n.param("function")) : nullptr) {
        const auto in_body = body_names.find(n.id);
        const logic::Function* body = in_body == body_names.end() ? nullptr : g->function(in_body->second);
        for (const logic::FunctionPin& in : called->inputs) {
          if (!in.ref) continue;
          json::Value variable = json::Value::object(), path = json::Value::object(), choices = json::Value::array();
          auto offer = [&](const std::string& name, logic::PinType type, const std::string& type_of) {
            json::Value c = json::Value::array();
            c.push(name);
            c.push(name + " (" + logic::type_label(logic::TypeRef{type, type_of}) + ")");
            choices.push(std::move(c));
          };
          if (body) {
            for (const logic::FunctionPin& l : body->locals) offer(l.name, l.type, l.of);
            for (const logic::FunctionPin& i : body->inputs)
              if (in.constant || (i.ref && !i.constant)) offer(i.name, i.type, i.of);  // (what is only read here goes into a const reference)
          }
          for (const logic::Variable& v : g->variables) offer(v.name, v.type, v.of);
          variable.set("key", logic::ref_key(in.name, "variable"));
          variable.set("label", in.name);
          variable.set("kind", "variable");
          variable.set("value", n.param(logic::ref_key(in.name, "variable")));
          variable.set("choices", std::move(choices));
          params.push(std::move(variable));
          path.set("key", logic::ref_key(in.name, "path"));
          path.set("label", in.name + " path");
          path.set("kind", "path");
          path.set("value", n.param(logic::ref_key(in.name, "path")));
          path.set("choices", json::Value::array());
          params.push(std::move(path));
        }
      }
      x.set("params", std::move(params));
    }
    if (auto it = by_node.find(n.id); it != by_node.end()) {
      json::Value msgs = json::Value::array();
      bool error = false;
      for (const logic::Issue* i : it->second) { msgs.push(i->text); error |= i->error; }
      x.set("issues", std::move(msgs));
      x.set("error", error);
    }
    nodes.push(std::move(x));
  }
  o.set("nodes", std::move(nodes));
  o.set("choices", std::move(shared));
  {
    json::Value counts = json::Value::object();
    auto put = [&](const char* key, const std::unordered_map<std::string, u32>& of) {
      json::Value m = json::Value::object();
      for (const auto& [name, n] : of) m.set(name, n);
      counts.set(key, std::move(m));
    };
    put("calls", calls);
    put("variables", variable_refs);
    put("types", type_refs);
    counts.set("nodes", static_cast<u32>(g->nodes.size()));
    counts.set("links", static_cast<u32>(g->links.size()));
    o.set("counts", std::move(counts));
  }
  {
    // (the sheets: each function with a body - how many nodes, and the node it starts at)
    std::unordered_map<std::string, u32> size;
    u32 events = static_cast<u32>(g->nodes.size());
    for (const auto& [id, name] : body_names) { ++size[name]; --events; }
    json::Value list = json::Value::array();
    for (const logic::Node& n : g->nodes) {
      if (n.type != "func.entry") continue;
      json::Value x = json::Value::object();
      const std::string name = n.param("function");
      x.set("name", name);
      x.set("nodes", size[name]);
      x.set("entry", n.id);
      list.push(std::move(x));
    }
    o.set("sheets", std::move(list));
    o.set("sheet", sheet);
    o.set("sheetEvents", events);
  }
  json::Value links = json::Value::array();
  for (const logic::Link& l : g->links) {
    if (!(shown(l.from) && shown(l.to))) continue;
    json::Value x = json::Value::array();
    x.push(l.from);
    x.push(l.out);
    x.push(l.to);
    x.push(l.in);
    links.push(std::move(x));
  }
  o.set("links", std::move(links));
  json::Value vars = json::Value::array();
  for (const logic::Variable& v : g->variables) {
    json::Value x = json::Value::object();
    x.set("name", v.name);
    x.set("type", logic::pin_type_name(v.type));
    if (!v.of.empty()) x.set("of", v.of);
    if (v.count) x.set("count", static_cast<f64>(v.count));  // (a list of a fixed length)
    x.set("value", logic::Value::from_json(v.value, logic::declared(v), *g).to_json(*g, v.of));
    if (v.constant) x.set("const", true);  // (a constant: read, never changed)
    vars.push(std::move(x));
  }
  o.set("variables", std::move(vars));
  json::Value graph = logic::graph_to_json(*g);
  // (the types: with all its libraries have - any of them can be picked)
  const json::Value types = g->uses.empty() ? graph : logic::graph_to_json(with_library_types(logic_level(), logic_project(), *g));
  o.set("enums", types["enums"].is_array() ? types["enums"] : json::Value::array());
  o.set("structs", types["structs"].is_array() ? types["structs"] : json::Value::array());
  o.set("functions", graph["functions"].is_array() ? graph["functions"] : json::Value::array());
  {
    // Libraries: is this Blueprint one, which does it use, and which of the level's could it use.
    // (A function and a type it has of a library say so themselves: "from".)
    o.set("library", g->library);
    json::Value uses = json::Value::array(), libraries = json::Value::array();
    for (const std::string& used : g->uses) uses.push(used);
    const game::Actor* self = logic_owner_ ? logic_level().find(logic_owner_) : nullptr;
    for (const auto& [id, name, other] : other_blueprints(logic_level(), logic_owner_)) {
      if (!other->library || g->uses_library(name) || !id) continue;
      if (self && uses_library(logic_level(), logic_project(), *other, self->name)) continue;  // (a library does not use what uses it)
      libraries.push(name);
    }
    // a world's Blueprint finds libraries in the project too (kern/docs/project.md 5.)
    if (const game::Project* project = logic_project())
      for (const game::ProgramBlueprint& b : project->blueprints)
        if (b.logic.library && !g->uses_library(b.name)) libraries.push(b.name);
    // The engine's libraries (kern/logic/engine_libraries.h): every Blueprint can use them; none of them is an actor
    // of the level, so none can be changed here - the editor shows them as the engine's ("engine": read-only).
    json::Value engine = json::Value::array();  // (each of them: "libraries" are the level's)
    for (const std::string& name : logic::engine_library_names()) engine.push(name);
    o.set("uses", std::move(uses));
    o.set("libraries", std::move(libraries));
    o.set("engineLibraries", std::move(engine));
    // The interface of the engine it is the object of (Graph::implements), and those it could be (kern/docs/blueprint_cpp.md,
    // "Schnittstellen der Engine"): each with the functions the engine calls - a pure one it must have.
    o.set("implements", g->implements);
    json::Value interfaces = json::Value::array();
    for (const logic::EngineInterface& i : logic::engine_interfaces()) {
      if (*i.waits) continue;
      json::Value x = json::Value::object(), fns = json::Value::array();
      x.set("name", i.name);
      for (const logic::EngineVirtual& v : i.functions)
        if (!*v.waits) fns.push(std::string(v.name) + (v.pure ? "" : "?"));
      x.set("functions", std::move(fns));
      interfaces.push(std::move(x));
    }
    o.set("interfaces", std::move(interfaces));
  }
  {
    // (the functions of the level's other Blueprints this one does not have or declare yet)
    json::Value offered = json::Value::array();
    for (const auto& [id, name, other] : other_blueprints(logic_level(), logic_owner_))
      for (const logic::Function& f : other->functions) {
        if (f.external || g->function(f.name) || offered.size() >= 200) continue;
        json::Value x = json::Value::object();
        x.set("owner", id);
        x.set("ownerName", name);
        x.set("name", f.name);
        if (!f.help.empty()) x.set("help", f.help);
        offered.push(std::move(x));
      }
    o.set("otherFunctions", std::move(offered));
    // (the types of the level's other Blueprints this one does not have: it can have them too -
    // they are types of the project then)
    json::Value types = json::Value::array();
    auto offer = [&](u32 id, const std::string& owner, const std::string& type, const char* kind, bool project, const std::string& help) {
      if (g->enumeration(type) || g->structure(type) || types.size() >= 200) return;
      for (const json::Value& have : types.items()) if (have["name"].as_string() == type) return;
      json::Value x = json::Value::object();
      x.set("owner", id);
      x.set("ownerName", owner);
      x.set("name", type);
      x.set("kind", kind);
      x.set("project", project);
      if (!help.empty()) x.set("help", help);
      types.push(std::move(x));
    };
    for (const auto& [id, name, other] : other_blueprints(logic_level(), logic_owner_)) {
      for (const logic::EnumType& e : other->enums) offer(id, name, e.name, "enum", e.project, e.help);
      for (const logic::StructType& t : other->structs) offer(id, name, t.name, "struct", t.project, t.help);
    }
    o.set("otherTypes", std::move(types));
  }
  o.set("machines", graph["machines"]);
  {
    // (a comment box stands on the sheet of the nodes in it; one around nothing, on the events' sheet)
    json::Value boxes = json::Value::array();
    for (const logic::CommentBox& c : g->comments) {
      bool any = false, mine = false;
      for (const logic::Node& n : g->nodes) {
        if (n.x < c.x || n.y < c.y || n.x > c.x + c.w || n.y > c.y + c.h) continue;
        any = true;
        if (shown(n.id)) { mine = true; break; }
      }
      if (!(mine || (!any && sheet.empty()))) continue;
      const json::Value& all = graph["comments"];
      for (const json::Value& j : all.items()) if (static_cast<u32>(j["id"].as_number()) == c.id) boxes.push(j);
    }
    o.set("comments", std::move(boxes));
  }
  json::Value iss = json::Value::array();
  if (const std::string conflict = project_type_conflict(logic_level(), logic_owner_, *g); !conflict.empty()) {
    json::Value x = json::Value::object();
    x.set("node", 0);
    x.set("error", true);
    x.set("text", conflict);
    iss.push(std::move(x));
  }
  for (const logic::Issue& i : issues) {
    json::Value x = json::Value::object();
    x.set("node", i.node);
    x.set("error", i.error);
    x.set("text", i.text);
    iss.push(std::move(x));
  }
  o.set("issues", std::move(iss));
  return o;
}

json::Value Studio::logic_trace_json() const {
  json::Value o = json::Value::object();
  logic::Runtime* rt = world_->logic_runtime();
  if (!rt || logic_program_) return o;  // (a program Blueprint runs in no world)
  rt->tracing = true;
  const logic::Instance* inst = rt->find(static_cast<logic::ActorRef>(logic_owner_));
  json::Value ran = json::Value::array();
  const std::string name = logic_owner_ ? (level_.find(logic_owner_) ? level_.find(logic_owner_)->name : std::string()) : std::string("Level");
  for (const auto& t : rt->trace())
    if (t.instance == name) ran.push(t.node);
  rt->clear_trace();
  o.set("ran", std::move(ran));
  if (inst) {
    json::Value vars = json::Value::object();
    for (const logic::Variable& v : inst->graph().variables) vars.set(v.name, inst->get(v.name).to_json(inst->graph(), v.of));
    o.set("variables", std::move(vars));
    json::Value states = json::Value::object();
    for (const logic::StateMachine& m : inst->graph().machines) states.set(m.name, inst->state(m.name));
    o.set("states", std::move(states));
    o.set("disabled", inst->disabled());
    o.set("tier", inst->tier());
  }
  return o;
}

// ---------------------------------------------------------------------------
// debugging

void Studio::check_logic_breakpoints() {
  if (logic_breakpoints_.empty() || paused_ || play_ == PlayState::editing) return;
  logic::Runtime* rt = world_->logic_runtime();
  if (!rt) return;
  rt->tracing = true;  // the runtime records which nodes ran (also in compiled code)
  for (const auto& t : rt->trace())
    for (const auto& [owner, node] : logic_breakpoints_) {
      if (t.node != node) continue;
      const game::Actor* a = owner ? level_.find(owner) : nullptr;
      if (t.instance != (owner ? (a ? a->name : std::string()) : std::string("Level"))) continue;
      toggle_pause();
      logic_break_hit_ = {owner, node};
      logic_owner_ = owner;
      logic_program_ = false;
      logic_open_ = true;
      logic_view_ = "graph";
      const logic::Graph* g = owner ? (a && a->logic ? &*a->logic : nullptr) : &level_.logic;
      const logic::Node* n = g ? g->find(node) : nullptr;
      if (n) {   // (the sheet the node stands on)
        const auto bodies = g->bodies(logic::library());
        const auto at = bodies.find(node);
        logic_sheets_[owner] = at == bodies.end() ? std::string() : at->second;
      }
      report("Breakpoint: " + (n ? logic::library().title(*n) : std::string("node")) + " ran - the game is paused (Resume continues)", "warn");
      json::Value ui = json::Value::object();
      ui.set("action", "logicBreak");
      ui.set("node", node);
      push_ui(std::move(ui));
      touch();
      return;
    }
  if (!logic_open_) rt->clear_trace();  // the editor's live view clears it otherwise
}

// ---------------------------------------------------------------------------
// palette

json::Value Studio::logic_palette(const json::Value& c) const {
  const logic::Library& lib = logic::library();
  const logic::Graph* g = graph_of(logic_level(), logic_owner_);
  logic::Graph empty;
  if (!g) g = &empty;
  // (the nodes of its libraries' types are offered whether it uses the type yet or not)
  const logic::Graph seen = with_library_types(logic_level(), logic_project(), *g);
  g = &seen;
  const std::string query = lower(c["query"].is_string() ? c["query"].as_string() : std::string());
  std::vector<std::string> words;
  {
    std::string w;
    for (char ch : query + " ") {
      if (ch == ' ') { if (!w.empty()) words.push_back(w); w.clear(); }
      else w += ch;
    }
  }
  // Context: dragging from a pin keeps only nodes that can take the wire.
  bool from_pin = false, from_output = false;
  logic::TypeRef from_type{logic::PinType::exec, {}};
  if (c["from"].is_object()) {
    logic::PinDef p;
    from_output = c["from"]["output"].as_bool();
    if (pin_of(*g, static_cast<u32>(c["from"]["node"].as_number()), c["from"]["pin"].as_string(), from_output, p)) {
      from_pin = true;
      from_type = {p.type, p.of};
    }
  }
  struct Entry { std::string label, category, help, type; std::vector<std::pair<std::string, std::string>> params; int rank = 0; };
  std::vector<Entry> entries;
  auto consider = [&](Entry e) {
    // Fits the wire?
    if (from_pin) {
      logic::Node probe;
      probe.type = e.type;
      for (auto& [k, v] : e.params) probe.set_param(k, v);
      if (compatible_pin(*g, probe, from_type, !from_output).empty()) return;
    }
    const std::string hay = lower(e.label + " " + e.category + " " + e.help + " " + (lib.find(e.type) ? lib.find(e.type)->keywords : std::string()));
    for (const std::string& w : words) if (hay.find(w) == std::string::npos) return;
    const std::string l = lower(e.label);
    e.rank = query.empty() ? 0 : l.rfind(query, 0) == 0 ? 0 : l.find(query) != std::string::npos ? 1 : 2;
    entries.push_back(std::move(e));
  };
  /// The steps that change a list variable: offered per list variable.
  auto variable_step = [](const logic::NodeDef& d) {
    for (const logic::ParamDef& pd : d.params) if (pd.kind == logic::ParamDef::Kind::variable) return true;
    return false;
  };
  /// The nodes of an enum: offered per enum of the Blueprint.
  auto enum_node = [](const logic::NodeDef& d) {
    for (const logic::ParamDef& pd : d.params) if (pd.kind == logic::ParamDef::Kind::type && pd.enums_only) return true;
    return false;
  };
  /// The nodes of a struct (Make, Break): offered per struct of the Blueprint.
  auto struct_node = [](const logic::NodeDef& d) {
    for (const logic::ParamDef& pd : d.params) if (pd.kind == logic::ParamDef::Kind::type && pd.structs_only) return true;
    return false;
  };
  for (const auto& d : lib.all()) {
    if (variable_step(*d) || enum_node(*d) || struct_node(*d)) continue;                 // per variable, per enum, per struct below
    if (d->type == "flow.reroute" || d->type == "value.reroute") continue;               // from a wire, below
    if (d->type == "func.return" || d->type == "func.call" || d->type == "func.pure" || d->type == "func.call_on" || d->type == "func.callback") continue;  // per function below
    Entry e{d->label, d->category, d->help, d->type, {}};
    if (from_pin && from_type.type != logic::PinType::exec) {
      // a generic node comes in the type that fits the wire (Length of a Text List, Select of a Vector)
      logic::Node probe;
      probe.type = d->type;
      std::vector<logic::PinDef> in, out;
      lib.pins(probe, *g, in, out);
      std::string fits;
      for (const logic::PinDef& p : from_output ? in : out)
        if (fits.empty() && p.type != logic::PinType::exec) fits = type_setting_for(*g, probe, p.id, !from_output, from_type);
      if (!fits.empty()) e.params.push_back({"type", fits});
      if (fits.empty() && logic::is_keyed(from_type.type) && from_output) {
        // (a node of a map or a set, from a map or a set: with its keys and values)
        std::string key, value;
        for (const logic::PinDef& p : in)
          if (keyed_params(*d, p.id, from_type, key, value)) {
            e.params.push_back({"key", key});
            if (!value.empty()) e.params.push_back({"value", value});
            break;
          }
      }
    }
    if (d->type == "func.entry") { e.label = "New Function"; e.help = "Adds a function to this Blueprint: its entry and a Return node. Inputs and outputs are set in Details."; }
    if (d->type == "event.state_enter" || d->type == "event.state_exit" || d->type == "state.set" || d->type == "state.is" || d->type == "state.get") {
      if (g->machines.empty()) continue;
      e.params.push_back({"machine", g->machines.front().name});
    }
    consider(std::move(e));
  }
  if (from_pin) {
    if (from_type.type == logic::PinType::exec) consider({"Add Reroute Node", "Flow", "A knot the wire passes through", "flow.reroute", {}});
    else consider({"Add Reroute Node", "Flow", "A knot the wire passes through", "value.reroute", {{"type", logic::type_text(from_type)}}});
  }
  // The nodes of a variable: of the Blueprint's, and of each function's local variables (those
  // name their function, the setting "scope").
  using Params = std::vector<std::pair<std::string, std::string>>;
  // (`naming`: how its nodes name it - the variable, or a place of the engine: {"bound", id})
  auto variable_nodes = [&](const logic::Variable& v, const std::string& scope, const Params& naming, const std::string& kind) {
    auto with = [&](Params p) {
      if (!p.empty() && p.front().first == "variable") { p.erase(p.begin()); p.insert(p.begin(), naming.begin(), naming.end()); }
      if (!scope.empty()) p.push_back({"scope", scope});
      return p;
    };
    auto name = [&](logic::Node& n) {
      for (const auto& [k, x] : naming) n.set_param(k, x);
    };
    const std::string category = !kind.empty() ? kind : scope.empty() ? std::string("Variables") : "Local Variables of " + scope;
    const std::string what = scope.empty() ? v.name : "the local variable " + v.name + " of " + scope;
    consider({"Get " + v.name, category, "Reads " + what, "var.get", with({{"variable", v.name}})});
    if (v.constant) return;  // (a constant is read: nothing changes it)
    consider({"Set " + v.name, category, "Writes " + what, "var.set", with({{"variable", v.name}})});
    consider({"Take " + v.name, category, "Takes what " + what + " holds and leaves it empty", "var.take", with({{"variable", v.name}})});
    if (logic::is_number(v.type) && v.type != logic::PinType::boolean)
      consider({"Add to " + v.name, category, "Adds to " + what, "var.add", with({{"variable", v.name}})});
    // a struct variable: the Set of each of its fields - a Set at that place (Held.Damage)
    if (const logic::StructType* s = v.type == logic::PinType::structure ? g->structure(v.of) : nullptr)
      for (const logic::StructField& f : s->fields) {
        logic::Node named;
        named.type = "var.set";
        name(named);
        named.set_param("path", "." + f.name);
        consider({lib.title(named), scope.empty() && kind.empty() ? std::string("Structures") : category, "Changes the field " + f.name + " of " + what, named.type, with({{"variable", v.name}, {"path", "." + f.name}})});
      }
    if (logic::is_keyed(v.type)) {
      // a map variable: the Set of the value at a place (Cells[index]) -, and the steps of a map or a set that change it
      if (v.type == logic::PinType::map) {
        logic::Node named;
        named.type = "var.set";
        name(named);
        named.set_param("path", "[]");
        consider({lib.title(named), scope.empty() && kind.empty() ? std::string("Maps") : category, "Replaces the value of one entry of " + what + " by its place (beside the map: nothing)", named.type, with({{"variable", v.name}, {"path", "[]"}})});
      }
      const std::string prefix = v.type == logic::PinType::map ? "map." : "set.";
      for (const auto& d : lib.all()) {
        if (!variable_step(*d) || d->type.rfind(prefix, 0) != 0) continue;
        logic::Node named;
        named.type = d->type;
        name(named);
        consider({lib.title(named), scope.empty() && kind.empty() ? std::string(v.type == logic::PinType::map ? "Maps" : "Sets") : category, d->help, d->type, with({{"variable", v.name}})});
      }
      return;
    }
    if (!logic::is_list(v.type)) return;
    // a list variable: the Set of one of its entries - a Set at that place (Scores[index]) -, and the steps that change it
    {
      logic::Node named;
      named.type = "var.set";
      name(named);
      named.set_param("path", "[]");
      consider({lib.title(named), scope.empty() && kind.empty() ? std::string("Lists") : category, "Replaces one entry of " + what + " (beside the list: nothing)", named.type, with({{"variable", v.name}, {"path", "[]"}})});
    }
    // (a list of a fixed length: its entries are set - no step changes its length; Copy Bytes writes the
    // bytes of its numbers where they stand, and only a list of numbers has those)
    const logic::PinType item = logic::element_of(v.type);
    const bool numbers = logic::is_number(item) && item != logic::PinType::boolean;
    for (const auto& d : lib.all()) {
      if (!variable_step(*d) || d->type.rfind("list.", 0) != 0) continue;
      if (d->type == "list.copy_bytes" ? !numbers : v.count != 0) continue;
      if (d->type == "list.insert_text" && !(logic::is_list(v.type) && logic::is_integer(item))) continue;   // (the bytes of a text go into whole numbers)
      logic::Node named;
      named.type = d->type;
      name(named);
      consider({lib.title(named), scope.empty() && kind.empty() ? std::string("Lists") : category, d->help, d->type, with({{"variable", v.name}})});
    }
  };
  for (const logic::Variable& v : g->variables) variable_nodes(v, {}, {{"variable", v.name}}, {});
  for (const logic::Function& f : g->functions)
    for (const logic::FunctionPin& l : f.locals) variable_nodes(logic::Variable{l.name, l.type, l.value, {}, l.of, l.count}, f.name, {{"variable", l.name}}, {});
  // The nodes of a place of the engine (kern/docs/blueprint_cpp.md, "Plätze der Engine"): of each node of the graph
  // that binds one, as of a variable - its nodes name the node ("bound").
  for (const logic::Node& n : g->nodes) {
    const logic::EngineFunction* f = logic::engine_function(n.type);
    if (!f || !f->place) continue;
    logic::Node named;
    named.type = "var.get";
    named.set_param("bound", std::to_string(n.id));
    logic::Variable v;
    if (g->root_of(named, v) != logic::Graph::Root::bound) continue;
    v.name = lib.title(named);
    variable_nodes(v, {}, {{"bound", std::to_string(n.id)}}, "Places of the Engine");
  }
  for (const logic::EnumType& e : g->enums)
    for (const auto& d : lib.all()) {
      if (!enum_node(*d)) continue;
      logic::Node named;
      named.type = d->type;
      named.set_param("enum", e.name);
      consider({lib.title(named), d->category, d->help, d->type, {{"enum", e.name}}});
    }
  for (const logic::StructType& s : g->structs)
    for (const auto& d : lib.all()) {
      if (!struct_node(*d)) continue;
      logic::Node named;
      named.type = d->type;
      named.set_param("struct", s.name);
      // (Find Row: by the struct's first field, until another is chosen)
      bool by_field = false;
      for (const logic::ParamDef& pd : d->params) by_field = by_field || pd.kind == logic::ParamDef::Kind::field;
      if (by_field && !s.fields.empty()) {
        named.set_param("field", s.fields.front().name);
        consider({lib.title(named), d->category, d->help, d->type, {{"struct", s.name}, {"field", s.fields.front().name}}});
        continue;
      }
      consider({lib.title(named), d->category, d->help, d->type, {{"struct", s.name}}});
    }
  // (the functions of the libraries it uses: called directly, by the library's name)
  for (const std::string& used : g->uses) {
    const logic::Graph* l = blueprints_of(logic_level(), logic_project())(used);
    for (const logic::Function& f : l ? l->functions : std::vector<logic::Function>()) {
      if (f.external || !f.from.empty()) continue;
      const std::string full = logic::library_function(used, f.name);
      consider({full, "Library " + used, f.help.empty() ? "Calls the function " + f.name + " of the library " + used : f.help, f.pure ? "func.pure" : "func.call", {{"function", full}}});
      if (f.outputs.size() <= 1)  // (a function of a library as a value the engine calls: Make Callback)
        consider({"Callback " + full, "Library " + used, "A Callback of " + f.name + ": a value the engine calls (its first input, a const reference, bound to a place)", "func.callback", {{"function", full}}});
    }
  }
  for (const logic::Function& f : g->functions) {
    if (!f.from.empty()) continue;  // (a function of a library: offered above, with all the others the library has)
    consider({f.name + " on Actor", "Functions", "Calls the function " + f.name + " of another actor's Blueprint (or the Level's)" + (f.help.empty() ? std::string() : ": " + f.help), "func.call_on",
              {{"function", f.name}}});
    if (f.external) continue;  // (a function of another Blueprint has no body here)
    consider({f.name, "Functions", f.help.empty() ? "Calls the function " + f.name : f.help, f.pure ? "func.pure" : "func.call", {{"function", f.name}}});
    consider({"Return (" + f.name + ")", "Functions", "Ends " + f.name + " and gives its outputs to the caller", "func.return", {{"function", f.name}}});
    if (g->library && f.outputs.size() <= 1)
      consider({"Callback " + f.name, "Functions", "A Callback of " + f.name + ": a value the engine calls (its first input, a const reference, bound to a place)", "func.callback", {{"function", f.name}}});
  }
  for (const std::string& ev : event_names(logic_level())) {
    consider({"Call " + ev, "Events", "Raises the custom event " + ev, "flow.call", {{"name", ev}}});
    consider({"Event " + ev, "Events", "Runs when " + ev + " is called", "event.custom", {{"name", ev}}});
  }
  for (const logic::StateMachine& m : g->machines)
    for (const logic::State& s : m.states) {
      consider({"Go to " + s.name, "State Machines", m.name + ": switch to " + s.name, "state.set", {{"machine", m.name}, {"state", s.name}}});
      consider({"On " + s.name + " Entered", "State Machines", m.name + " entered " + s.name, "event.state_enter", {{"machine", m.name}, {"state", s.name}}});
    }
  // Categories in the order people reach for them.
  static const char* order[] = {"Events", "Flow", "Variables", "Lists", "Enums", "Structures", "Functions", "Game", "Actors", "Robots", "Physics", "Combat", "Effects", "Audio", "State Machines",
                                "Math", "Logic", "Vector", "Text", "Values", "Utilities"};
  auto cat_rank = [](const std::string& c) {
    for (int i = 0; i < static_cast<int>(std::size(order)); ++i) if (c == order[i]) return i;
    return static_cast<int>(std::size(order));
  };
  std::stable_sort(entries.begin(), entries.end(), [&](const Entry& a, const Entry& b) {
    if (a.rank != b.rank) return a.rank < b.rank;
    if (a.category != b.category) return cat_rank(a.category) < cat_rank(b.category);
    return a.label < b.label;
  });
  json::Value out = json::Value::array();
  std::size_t n = 0;
  for (const Entry& e : entries) {
    if (++n > 120) break;
    json::Value x = json::Value::object();
    x.set("label", e.label);
    x.set("category", e.category);
    x.set("help", e.help);
    x.set("type", e.type);
    if (!e.params.empty()) {
      json::Value p = json::Value::object();
      for (const auto& [k, v] : e.params) p.set(k, v);
      x.set("params", std::move(p));
    }
    out.push(std::move(x));
  }
  json::Value r = ok_result();
  r.set("items", std::move(out));
  return r;
}

/// A default kept through a change of type: converted where the types convert; a value of an
/// enum or a struct starts at the default of another one.
/// Does a place of this type name the enum or struct `name` - itself, its items, a map's keys or
/// values, a set's keys?
static bool names_type(const logic::TypeRef& t, const std::string& name) {
  for (const logic::TypeRef& named : logic::types_named(t))
    if (named.of == name) return true;
  return false;
}

static json::Value retyped_default(const logic::Graph& g, const json::Value& value, const logic::TypeRef& from, const logic::TypeRef& to) {
  const bool declared = logic::element_of(to.type) == logic::PinType::enumeration || logic::element_of(to.type) == logic::PinType::structure;
  const bool same = from.type == to.type && from.of == to.of;  // (the same type - a list of another length is one)
  if (declared && !same) return logic::zero_of(to, g).to_json(g, to);
  // (what the place held - a list of a fixed length with all its entries, also those its default
  // does not say - as the new type holds it: a list of a fixed length at its length)
  if (same) return logic::fitted(logic::Value::from_json(value, from, g), to, g).to_json(g, to);
  // (a list of another kind of entries: each entry as the new kind reads it - numbers stay numbers)
  const logic::Value held = logic::Value::from_json(value, from, g);
  const logic::Value as = logic::is_list(from.type) && logic::is_list(to.type) ? held : held.converted(to.type);
  return logic::Value::from_json(as.to_json(), to, g).to_json(g, to);
}

/// Everything in the Blueprint that holds a value: the defaults of variables, of functions' pins
/// and of structs' fields, and what is typed into the pins of nodes (the pins as they are now).
/// `fn(value, its type)` may change it.
static void each_held_value(logic::Graph& g, const std::function<void(json::Value&, const logic::TypeRef&)>& fn) {
  for (logic::Node& n : g.nodes) {
    std::vector<logic::PinDef> in, out;
    logic::library().pins(n, g, in, out);
    for (auto& [pin, value] : n.values)
      for (const logic::PinDef& pd : in)
        if (pd.id == pin && pd.type != logic::PinType::exec) fn(value, {pd.type, pd.of});
  }
  for (logic::Variable& v : g.variables) fn(v.value, logic::declared(v));
  for (logic::Function& f : g.functions)
    for (auto* pins : {&f.inputs, &f.outputs, &f.locals})
      for (logic::FunctionPin& fp : *pins) fn(fp.value, logic::declared(fp));
  for (logic::StructType& s : g.structs)
    for (logic::StructField& f : s.fields) fn(f.value, f.type);
}

/// Every value of the struct `name` inside the value `j` of type `t` - itself, the items of a
/// list, the fields of other structs - as the JSON object of its fields: `fn` may change it.
static void each_struct_in(const logic::Graph& g, json::Value& j, const logic::TypeRef& t, const std::string& name, const std::function<void(json::Value&)>& fn, std::size_t depth = 0) {
  const logic::StructType* s = logic::element_of(t.type) == logic::PinType::structure && depth <= g.structs.size() ? g.structure(t.of) : nullptr;
  if (!s) return;
  auto one = [&](json::Value& object) {
    if (!object.is_object()) return;
    for (const logic::StructField& f : s->fields)
      if (json::Value* held = object.find(f.name)) each_struct_in(g, *held, f.type, name, fn, depth + 1);
    if (s->name == name) fn(object);
  };
  if (t.type == logic::PinType::structure) one(j);
  else if (j.is_array())
    for (json::Value& item : j.items()) one(item);
}

/// After a struct's declaration changed: every value the Blueprint holds is what its type says
/// again - a struct with every field of the declaration (a new field at its default, a field that
/// is gone not there).
static void settle_struct_values(logic::Graph& g) {
  // (first the fields' defaults, in the order of the declarations: a struct's default holds those of the structs above it)
  for (logic::StructType& s : g.structs)
    for (logic::StructField& f : s.fields)
      if (logic::element_of(f.type.type) == logic::PinType::structure) f.value = logic::Value::from_json(f.value, f.type, g).to_json(g, f.type);
  each_held_value(g, [&](json::Value& value, const logic::TypeRef& t) {
    if (logic::element_of(t.type) == logic::PinType::structure) value = logic::Value::from_json(value, t, g).to_json(g, t);
  });
}

/// The wires of `g` that no longer fit after a change to a struct: into or out of a pin that is
/// gone, or between pins whose types do not go together any more - on the struct's own nodes
/// (Make, Break, Set Field), whose pins are the struct's fields.
static void drop_struct_wires(logic::Graph& g) {
  g.links.erase(std::remove_if(g.links.begin(), g.links.end(),
                               [&](const logic::Link& l) {
                                 const logic::Node* from = g.find(l.from);
                                 const logic::Node* to = g.find(l.to);
                                 if (!from || !to || (from->type.rfind("struct.", 0) != 0 && to->type.rfind("struct.", 0) != 0)) return false;
                                 logic::PinDef po, pi;
                                 if (!pin_of(g, l.from, l.out, true, po) || !pin_of(g, l.to, l.in, false, pi)) return true;
                                 return po.type != logic::PinType::exec && !logic::fits({po.type, po.of}, {pi.type, pi.of});
                               }),
                g.links.end());
}

/// The values of the enum `name` change their places: `place[k]` is where value k is afterwards
/// (-1: it is gone). A value is its place among the enum's values, so everything of the enum's
/// type follows: what is typed into pins, variables and functions' pins - also where a struct
/// holds it - (a value that is gone becomes the first one), and the wires of a Switch on the enum
/// (the wires of a value that is gone go with it). The enum's own list of values is the caller's
/// to change.
static void move_enum_values(logic::Graph& g, const std::string& name, const std::vector<i64>& place) {
  auto to = [&](i64 at) {
    const i64 now = at >= 0 && static_cast<std::size_t>(at) < place.size() ? place[static_cast<std::size_t>(at)] : 0;
    return now < 0 ? i64{0} : now;
  };
  std::function<void(json::Value&, const logic::TypeRef&, std::size_t)> moved = [&](json::Value& j, const logic::TypeRef& t, std::size_t depth) {
    if (logic::element_of(t.type) == logic::PinType::enumeration && t.of == name) {
      logic::Value v = logic::Value::from_json(j, t.type);
      if (t.type == logic::PinType::enumeration) v.i = to(v.i);
      else if (v.size() > 0)
        for (i64& x : v.edit<i64>()) x = to(x);
      j = v.to_json();
      return;
    }
    if (logic::is_keyed(t.type)) {
      // (a map or a set: its keys where they are of the enum - [[key, value], ...], [key, ...] -, a map's values as what they are)
      if (!j.is_array()) return;
      const logic::TypeRef key = logic::key_type(t), value = t.type == logic::PinType::map ? logic::value_type(t) : logic::TypeRef{};
      const bool keys = key.type == logic::PinType::enumeration && key.of == name;
      for (json::Value& entry : j.items()) {
        json::Value* k = t.type == logic::PinType::set ? &entry : entry.is_array() && entry.size() == 2 ? &entry.items()[0] : nullptr;
        if (keys && k && k->is_number()) *k = json::Value(static_cast<f64>(to(static_cast<i64>(k->as_number()))));
        if (t.type == logic::PinType::map && entry.is_array() && entry.size() == 2) moved(entry.items()[1], value, depth + 1);
      }
      return;
    }
    // (a struct: the fields that are of the enum, or hold it)
    const logic::StructType* s = logic::element_of(t.type) == logic::PinType::structure && depth <= g.structs.size() ? g.structure(t.of) : nullptr;
    if (!s) return;
    auto one = [&](json::Value& object) {
      if (!object.is_object()) return;
      for (const logic::StructField& f : s->fields)
        if (json::Value* held = object.find(f.name)) moved(*held, f.type, depth + 1);
    };
    if (t.type == logic::PinType::structure) one(j);
    else if (j.is_array())
      for (json::Value& item : j.items()) one(item);
  };
  each_held_value(g, [&](json::Value& value, const logic::TypeRef& t) { moved(value, t, 0); });
  // Switch on this enum: each wire stays with its value
  auto case_of = [&](const logic::Link& l) -> i64 {
    const logic::Node* from = g.find(l.from);
    if (!from || from->type != "flow.switch_enum" || from->param("enum") != name || l.out.rfind("case", 0) != 0) return -1;
    const i64 k = static_cast<i64>(std::strtoll(l.out.c_str() + 4, nullptr, 10));
    return k >= 0 && static_cast<std::size_t>(k) < place.size() ? k : -1;
  };
  g.links.erase(std::remove_if(g.links.begin(), g.links.end(), [&](const logic::Link& l) { const i64 k = case_of(l); return k >= 0 && place[static_cast<std::size_t>(k)] < 0; }), g.links.end());
  std::vector<std::pair<logic::Link*, std::string>> renamed;  // (first all of them read, then written: a wire's new name is another one's old)
  for (logic::Link& l : g.links)
    if (const i64 k = case_of(l); k >= 0) renamed.push_back({&l, "case" + std::to_string(place[static_cast<std::size_t>(k)])});
  for (auto& [link, out] : renamed) link->out = out;
}

// ---------------------------------------------------------------------------
// commands

json::Value Studio::logic_command(const std::string& name, const json::Value& c) {
  auto num = [&](const char* k, f64 fallback = 0) { return c[k].is_number() ? c[k].as_number() : fallback; };
  auto str = [&](const char* k) -> std::string { return c[k].is_string() ? c[k].as_string() : std::string(); };
  // A command that names a type may say a length with it ("count"): a list of a fixed length -
  // what a variable, a field of a struct, a pin or a local variable of a function is declared as.
  auto with_count = [&](logic::TypeRef& t, std::string& error) {
    const f64 n = num("count");
    t.count = 0;
    if (n == 0) return true;
    if (!logic::is_list(t.type) || n < 1 || n > logic::kLongestFixed || n != std::floor(n)) {
      error = "a fixed length is a list's: 1 to " + std::to_string(logic::kLongestFixed) + " entries";
      return false;
    }
    t.count = static_cast<u32>(n);
    return true;
  };
  if (name == "logicOpen") {
    // {program: "<name>"}: a program Blueprint of the project; else {owner}: the level's graph (0) or an actor's.
    if (c["program"].is_string()) {
      const std::string which = str("program");
      std::size_t at = project_.blueprints.size();
      for (std::size_t i = 0; i < project_.blueprints.size(); ++i)
        if (project_.blueprints[i].name == which) at = i;
      if (at == project_.blueprints.size()) return fail_result("the project has no Blueprint \"" + which + "\"");
      logic_program_ = true;
      logic_owner_ = static_cast<u32>(at + 1);
      logic_open_ = true;
      logic_view_ = "graph";
      logic_sheets_.erase(logic_owner_);
      touch();
      return ok_result();
    }
    const u32 owner = static_cast<u32>(num("owner", c["owner"].is_null() && !selection_.empty() ? primary() : 0));
    if (owner && !level_.find(owner)) return fail_result("no such actor");
    logic_program_ = false;
    logic_owner_ = owner;
    logic_open_ = true;
    logic_view_ = "graph";   // (its sheet is the one it had open last - logic_sheets_)
    touch();
    return ok_result();
  }
  if (name == "logicClose") {
    logic_open_ = false;
    touch();
    return ok_result();
  }
  if (name == "logicView") {
    logic_view_ = str("view").empty() ? "graph" : str("view");
    touch();
    return ok_result();
  }
  if (name == "logicSheet") {
    // The function whose body the graph view shows ("" = the events), for the Blueprint that is open (logic_json).
    logic_view_ = "graph";
    logic_sheets_[logic_owner_] = str("function");
    touch();
    return ok_result();
  }
  if (name == "logicReveal") {
    // Shows the sheet a node stands on (a result of Find, a compiler message): the page then moves to the node.
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    const u32 node = static_cast<u32>(num("node"));
    if (!g || !g->find(node)) return fail_result("no such node");
    const auto bodies = g->bodies(logic::library());
    const auto it = bodies.find(node);
    logic_view_ = "graph";
    logic_sheets_[logic_owner_] = it == bodies.end() ? std::string() : it->second;
    touch();
    return ok_result();
  }
  if (name == "logicFind") {
    // Find in the whole Blueprint, also on the sheets that are not shown: nodes by title, comment, category and
    // parameter values. At most 200 hits - "more" says there are others.
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    json::Value r = ok_result();
    json::Value hits = json::Value::array();
    std::string q = str("query");
    for (char& ch : q) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    bool more = false;
    if (g && !q.empty()) {
      const logic::Library& lib = logic::library();
      const auto bodies = g->bodies(lib);
      for (const logic::Node& n : g->nodes) {
        const logic::NodeDef* d = lib.find(n.type);
        const std::string title = lib.title(n);
        std::string hay = title + " " + n.comment + " " + (d ? d->category : std::string());
        for (const auto& [k, v] : n.params) hay += " " + v;
        std::vector<logic::PinDef> in, out;
        lib.pins(n, *g, in, out);
        for (const logic::PinDef& pin : in) hay += " " + pin.label;
        for (const logic::PinDef& pin : out) hay += " " + pin.label;
        for (char& ch : hay) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (hay.find(q) == std::string::npos) continue;
        if (hits.size() >= 200) { more = true; break; }
        json::Value h = json::Value::object();
        h.set("node", n.id);
        h.set("title", title);
        if (d) { h.set("category", d->category); h.set("kind", kind_name(d->kind)); }
        if (const auto it = bodies.find(n.id); it != bodies.end()) h.set("sheet", it->second);
        hits.push(std::move(h));
      }
    }
    r.set("hits", std::move(hits));
    r.set("more", more);
    return r;
  }
  if (name == "logicPalette") return logic_palette(c);
  if (name == "logicBreakpoint") {
    // F9 (Blueprint): a breakpoint on a node of the open graph; "on" omitted toggles.
    const std::pair<u32, u32> key{logic_owner_, static_cast<u32>(num("node"))};
    auto it = std::find(logic_breakpoints_.begin(), logic_breakpoints_.end(), key);
    const bool on = c["on"].is_bool() ? c["on"].as_bool() : it == logic_breakpoints_.end();
    if (on && it == logic_breakpoints_.end()) logic_breakpoints_.push_back(key);
    if (!on && it != logic_breakpoints_.end()) logic_breakpoints_.erase(it);
    if (c["clearAll"].as_bool()) logic_breakpoints_.clear();
    touch();
    json::Value r = ok_result();
    r.set("on", on);
    r.set("count", static_cast<u32>(logic_breakpoints_.size()));
    return r;
  }
  if (name == "logicTrace") {
    json::Value r = ok_result();
    r.set("trace", logic_trace_json());
    return r;
  }
  if (name == "logicCopy") {
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    if (!g) return fail_result("no graph");
    logic::Graph part;
    std::vector<u32> ids;
    for (const json::Value& v : c["ids"].items()) ids.push_back(static_cast<u32>(v.as_number()));
    for (const logic::Node& n : g->nodes) if (std::find(ids.begin(), ids.end(), n.id) != ids.end()) part.nodes.push_back(n);
    for (const logic::CommentBox& cb : g->comments) if (std::find(ids.begin(), ids.end(), cb.id) != ids.end()) part.comments.push_back(cb);
    for (const logic::Link& l : g->links)
      if (std::find(ids.begin(), ids.end(), l.from) != ids.end() && std::find(ids.begin(), ids.end(), l.to) != ids.end()) part.links.push_back(l);
    // The functions the copied nodes name travel with them (pasted into another Blueprint).
    for (const logic::Node& n : part.nodes)
      if (const logic::Function* f = g->function(n.param("function")); f && n.type.rfind("func.", 0) == 0 && !part.function(f->name)) part.functions.push_back(*f);
    // ... and so do the enums and the structs the nodes and those functions are of - a struct
    // with the structs and the enums its fields are of.
    std::vector<std::string> structs;
    auto take = [&](const std::string& of) {
      if (const logic::EnumType* e = g->enumeration(of); e && !part.enumeration(of)) part.enums.push_back(*e);
      const logic::StructType* s = g->structure(of);
      if (!s) return;
      std::vector<std::string> all = logic::structs_in(*s, *g);
      all.push_back(s->name);
      for (const std::string& name : all) {
        const logic::StructType* inner = g->structure(name);
        if (!inner || std::find(structs.begin(), structs.end(), name) != structs.end()) continue;
        structs.push_back(name);
        for (const logic::StructField& f : inner->fields)
          if (const logic::EnumType* e = g->enumeration(f.type.of); e && !part.enumeration(e->name)) part.enums.push_back(*e);
      }
    };
    for (const logic::Function& f : part.functions)
      for (const auto* pins : {&f.inputs, &f.outputs, &f.locals})
        for (const logic::FunctionPin& fp : *pins) take(fp.of);
    for (const logic::Node& n : part.nodes) {
      std::vector<logic::PinDef> in, out;
      logic::library().pins(n, *g, in, out);
      for (const auto* pins : {&in, &out})
        for (const logic::PinDef& pd : *pins) take(pd.of);
      if (n.type == "flow.switch_enum") take(n.param("enum"));
    }
    // (in the order they are declared: a struct after the structs its fields are of)
    for (const logic::StructType& s : g->structs)
      if (std::find(structs.begin(), structs.end(), s.name) != structs.end()) part.structs.push_back(s);
    json::Value r = ok_result();
    r.set("text", json::stringify(logic::graph_to_json(part)));
    return r;
  }
  if (name == "logicCpp") {
    // The Blueprint in its second notation (docs/blueprint_cpp.md): the code alone - where the
    // nodes are stays with the graph.
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    const game::Actor* a = logic_owner_ ? logic_level().find(logic_owner_) : nullptr;
    const logic::Graph none;
    const std::string owner_name = logic::cpp_class_name(a ? a->name : std::string("Level"), g ? *g : none);
    std::string text, why;
    if (!logic::to_cpp(g ? *g : none, owner_name, text, &why, logic::library(), false)) return fail_result(why);
    json::Value r = ok_result();
    r.set("text", text);
    r.set("name", owner_name);
    // Does a C++ compiler run this text as it stands? (If not: what is missing.)
    r.set("native", logic::native_ready(g ? *g : none, &why));
    if (!r["native"].as_bool()) r.set("nativeWhy", why);
    return r;
  }
  if (name == "logicExportCpp") {
    // The Blueprint as the source a game's build compiles: its own class - the Blueprint's text
    // as it is - and the glue the engine reaches it through (what tools/blueprint_cpp.sh writes
    // for a whole level). A game that links it runs the Blueprint as that compiled code.
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    if (!g) return fail_result("this actor has no logic yet");
    std::string text, glue, why;
    if (!logic::native_ready(*g, &why)) return fail_result(why);  // (it does not compile: Compile says where)
    const game::Actor* owner = logic_owner_ ? logic_level().find(logic_owner_) : nullptr;
    const std::string cls = logic::cpp_class_name(owner ? owner->name : std::string("Level"), *g);
    if (!logic::to_cpp(*g, cls, text, &why) || !logic::native_glue(*g, cls, glue, &why, logic::library(), {}, blueprints_of(logic_level(), logic_project()))) return fail_result(why);
    json::Value r = ok_result();
    r.set("text", text + "\n" + glue);
    std::string file;
    for (char ch : cls) file += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    r.set("file", "blueprint_" + file + ".cpp");
    r.set("native", true);
    return r;
  }
  if (name == "logicCompile") {
    json::Value r = ok_result();
    const logic::Graph* g = graph_of(logic_level(), logic_owner_);
    json::Value iss = json::Value::array();
    if (g)
      for (const logic::Issue& i : whole_issues(logic_level(), logic_project(), *g)) {
        json::Value x = json::Value::object();
        x.set("node", i.node);
        x.set("error", i.error);
        x.set("text", i.text);
        iss.push(std::move(x));
      }
    r.set("issues", std::move(iss));
    return r;
  }

  // ---- edits ----
  std::string error;
  json::Value result = ok_result();
  const u32 owner = logic_owner_;
  // Several commands with the same "transaction" are one undo step (picking wires up and dropping
  // them elsewhere, breaking all links of a node).
  if (c["transaction"].is_string() && name != "logicMove") {
    coalesce_ = "logic-tx:" + std::to_string(owner);
    transaction_ = str("transaction");
  }
  // A program Blueprint that the level's Blueprints use as a library: they follow it too (they find it in the project).
  auto program_library_follows = [&](u32 id, const logic::Graph& g, std::string& why) {
    const game::Actor* self = program_view_.find(id);
    if (!self || !g.library) return true;
    const logic::Blueprints all = [&](std::string_view n) -> const logic::Graph* {
      for (const game::Actor& a : level_.actors)
        if (a.logic && a.name == n) return &*a.logic;
      for (const game::Actor& a : program_view_.actors)
        if (a.logic && a.name == n) return &*a.logic;
      return nullptr;
    };
    for (game::Actor& a : level_.actors) {
      if (!a.logic || !uses_library(level_, &project_, *a.logic, self->name)) continue;
      if (!logic::take_libraries(*a.logic, all, &why)) {
        why = "the Blueprint of " + a.name + " (in the level) uses this one: " + why;
        return false;
      }
    }
    return true;
  };
  auto graph_edit = [&](const char* label, auto&& fn) {
    const bool done = edit_logic(label, [&](game::Level& l) {
      logic::Graph* g = graph_of(l, owner, true);
      if (!g) { error = "the graph's actor is gone"; return false; }
      // An edit may pick whatever the Blueprint's libraries have - any type, any function: while
      // it is made, the Blueprint holds all of it. After it, it holds what it uses - and the
      // Blueprints that use it follow.
      if (!g->uses.empty()) logic::take_libraries(*g, blueprints_of(l, logic_project()), nullptr, true);
      // (what the edit adds stands on the sheet that is open: a node that is no function's by its wires yet stays
      // there - logic_loose_ -, a new function opens on its own sheet, a renamed one stays open)
      std::unordered_set<u32> had;
      for (const logic::Node& n : g->nodes) had.insert(n.id);
      std::vector<std::string> named;
      for (const logic::Function& f : g->functions) named.push_back(f.name);
      if (!fn(*g, l) || !libraries_follow(l, logic_project(), owner, *g, error)) return false;
      if (logic_program_ && !program_library_follows(owner, *g, error)) return false;
      std::string& sheet = logic_sheets_[owner];
      std::vector<std::string> fresh;
      for (const logic::Function& f : g->functions)
        if (std::find(named.begin(), named.end(), f.name) == named.end()) fresh.push_back(f.name);
      if (fresh.size() == 1 && (named.size() < g->functions.size() || !g->function(sheet))) sheet = fresh[0];
      for (const logic::Node& n : g->nodes)
        if (!had.count(n.id) && !sheet.empty()) logic_loose_[owner][n.id] = sheet;
      return true;
    });
    if (!done && !error.empty()) return fail_result(error);
    result.set("ok", done || error.empty());
    return result;
  };
  // An edit of an enum or a struct. A type of the project is changed in every Blueprint of the
  // level that has it - one step, refused as a whole when one of them cannot follow.
  //   share   the type becomes one of the project (and the types its fields are of)
  //   local   it is this Blueprint's own again (while no other Blueprint has it)
  //   adopt   this Blueprint gets the type `name` of the Blueprint `from`
  auto type_edit = [&](const char* label, auto&& body) {
    const std::string op = str("op"), which = str("name");
    return graph_edit(label, [&](logic::Graph& g, game::Level& level) {
      if (op == "adopt") {
        logic::Graph* from = graph_of(level, static_cast<u32>(num("from")), false);
        if (!from || from == &g) { error = "that Blueprint has no type " + which; return false; }
        if (const std::string why = adopt_project_type(g, *from, which); !why.empty()) { error = why; return false; }
        result.set("name", which);
        return true;
      }
      if (op == "share" || op == "local") {
        if (!g.enumeration(which) && !g.structure(which)) { error = "no type " + which; return false; }
        if (const logic::EnumType* e = g.enumeration(which); e && !e->from.empty()) { error = which + " is a type of the library " + e->from + ": it is changed there"; return false; }
        if (const logic::StructType* t = g.structure(which); t && !t->from.empty()) { error = which + " is a type of the library " + t->from + ": it is changed there"; return false; }
        if (op == "share") {
          make_project_type(g, which);
          if (const std::string conflict = project_type_conflict(level, owner, g); !conflict.empty()) { error = conflict; return false; }
          return true;
        }
        for (const auto& [other_name, other] : other_graphs(level, &g))
          if (is_project_type(*other, which)) { error = "the Blueprint of " + other_name + " has " + which + " too: it stays a type of the project"; return false; }
        for (const logic::StructType& t : g.structs)
          if (t.project && t.name != which)
            for (const logic::StructField& f : t.fields)
              if (f.type.of == which) { error = which + " is a field's type in " + t.name + ", a struct of the project"; return false; }
        if (logic::EnumType* e = g.enumeration(which)) e->project = false;
        if (logic::StructType* t = g.structure(which)) t->project = false;
        return true;
      }
      const bool everywhere = op != "add" && op != "remove" && is_project_type(g, which);
      if (!body(g, level)) return false;
      if (!everywhere) return true;
      // (a field of a project struct is of a type of the project)
      std::string field_of = str("type");
      if (field_of.size() > 5 && field_of.compare(field_of.size() - 5, 5, "_list") == 0) field_of.resize(field_of.size() - 5);
      const bool brings = (op == "addField" || op == "fieldType") && (g.enumeration(field_of) || g.structure(field_of));
      if (brings) make_project_type(g, field_of);
      for (const auto& [other_name, other] : other_graphs(level, &g)) {
        if (!is_project_type(*other, which)) continue;
        if (brings)
          if (const std::string why = adopt_project_type(*other, g, field_of); !why.empty()) { error = "the Blueprint of " + other_name + ": " + why; return false; }
        if (!body(*other, level)) { error = "the Blueprint of " + other_name + ": " + (error.empty() ? "it cannot follow" : error); return false; }
      }
      if (const std::string conflict = project_type_conflict(level, owner, g); !conflict.empty()) { error = conflict; return false; }
      return true;
    });
  };
  if (name == "logicCppApply") {
    // The Blueprint becomes what the C++ says: one undo step. The nodes the text still has keep
    // their ids and places; what cannot be read is refused with its line and the reason.
    const logic::Graph* g = graph_of(logic_level(), owner);
    logic::Graph now;
    if (!logic::apply_cpp(str("text"), g ? *g : logic::Graph(), now, &error, logic::library(), blueprints_of(logic_level(), logic_project()))) return fail_result(error);
    if (const std::string conflict = project_type_conflict(logic_level(), owner, now); !conflict.empty()) return fail_result(conflict + " (change it in one of them: the others follow)");
    bool applied = false;
    graph_edit("Edit C++", [&](logic::Graph& graph, game::Level&) {
      graph = std::move(now);
      applied = true;
      return true;
    });
    if (!applied) return fail_result(error.empty() ? "Stop playing to change the Blueprint" : error);
    return result;
  }
  if (name == "logicAdd") {
    const std::string type = str("type");
    if (!logic::library().find(type)) return fail_result("unknown node type " + type);
    return graph_edit("Add Node", [&](logic::Graph& g, game::Level&) {
      logic::Node n;
      n.id = g.next_id++;
      n.type = type;
      n.x = static_cast<f32>(std::round(num("x") / 8) * 8);
      n.y = static_cast<f32>(std::round(num("y") / 8) * 8);
      for (const auto& [k, v] : c["params"].members()) n.set_param(k, v.is_string() ? v.as_string() : json::stringify(v));
      // Function nodes name their function. A new entry is a new function (with a Return next to
      // it); a call or a Return without a function takes the first one.
      if (type == "func.entry") {
        const std::string wanted = n.param("function");
        if (!wanted.empty() && g.function(wanted)) {
          for (const logic::Node& o : g.nodes)
            if (o.type == "func.entry" && o.param("function") == wanted) { error = "the function " + wanted + " has its entry node already"; return false; }
        } else {
          std::string base = wanted.empty() ? "NewFunction" : wanted, fname = base;
          if (const std::string why = name_problem(g, base); !why.empty()) { error = why; return false; }
          for (int i = 2; g.function(fname) || g.variable(fname); ++i) fname = base + std::to_string(i);
          logic::Function f;
          f.name = fname;
          g.functions.push_back(std::move(f));
          n.set_param("function", fname);
          logic::Node ret;
          ret.id = g.next_id++;
          ret.type = "func.return";
          ret.x = n.x + 320;
          ret.y = n.y;
          ret.set_param("function", fname);
          g.links.push_back({n.id, "then", ret.id, "exec"});
          g.nodes.push_back(std::move(ret));
          result.set("function", fname);
        }
      }
      // (a call of a function of a library it uses - Workshop::skid -: what the function looks like
      // comes with the edit, from the library)
      const bool of_library = (type == "func.call" || type == "func.pure") && n.param("function").find("::") != std::string::npos;
      if ((type == "func.return" || type == "func.call" || type == "func.pure" || type == "func.call_on") && !of_library && !g.function(n.param("function"))) {
        if (g.functions.empty()) { error = "this Blueprint has no function yet: add one in My Blueprint"; return false; }
        n.set_param("function", g.functions.front().name);
      }
      // New variables nodes on a graph without variables: offer one (Get / Set need a name).
      if ((type == "var.get" || type == "var.set" || type == "var.add" || type == "var.take") && n.param("variable").empty()) {
        if (g.variables.empty()) g.variables.push_back({"Count", logic::kFloat, json::Value(0.0), {}});
        n.set_param("variable", g.variables.front().name);
      }
      if ((type.rfind("event.state", 0) == 0 || type.rfind("state.", 0) == 0) && n.param("machine").empty()) {
        if (g.machines.empty()) {
          logic::StateMachine m;
          m.id = g.next_id++;
          m.states.push_back({g.next_id++, "Idle", 0, 0, true});
          m.states.push_back({g.next_id++, "Active", 260, 0, false});
          g.machines.push_back(std::move(m));
        }
        n.set_param("machine", g.machines.front().name);
      }
      g.nodes.push_back(n);
      result.set("id", n.id);
      // Dropped from a dragged wire: connect to the first fitting pin.
      if (c["from"].is_object()) {
        const u32 from_node = static_cast<u32>(c["from"]["node"].as_number());
        const std::string from_pin = c["from"]["pin"].as_string();
        const bool output = c["from"]["output"].as_bool();
        logic::PinDef p;
        if (pin_of(g, from_node, from_pin, output, p)) {
          std::string other = compatible_pin(g, g.nodes.back(), {p.type, p.of}, !output);
          // (none of its pins is of the wire's type: the first data pin - the node takes the wire's type where it can, else a
          // conversion goes between them, as on any wire)
          if (other.empty() && p.type != logic::PinType::exec) {
            std::vector<logic::PinDef> pin_in, pin_out;
            logic::library().pins(g.nodes.back(), g, pin_in, pin_out);
            for (const logic::PinDef& q : output ? pin_in : pin_out)
              if (q.type != logic::PinType::exec && !q.hidden && (output ? conversion_for({p.type, p.of}, {q.type, q.of}) : conversion_for({q.type, q.of}, {p.type, p.of})).why.empty()) { other = q.id; break; }
          }
          std::string ignore;
          if (!other.empty()) {
            if (output) connect(g, from_node, from_pin, n.id, other, ignore);
            else connect(g, n.id, other, from_node, from_pin, ignore);
          }
        }
      }
      return true;
    });
  }
  if (name == "logicPaste") {
    json::Value j;
    logic::Graph part;
    if (!json::parse(str("text"), j) || !logic::graph_from_json(j, part, &error)) return fail_result(error.empty() ? "not logic nodes" : error);
    logic::cpp_names(part);  // (copied before names had to be C++ names)
    return graph_edit("Paste Nodes", [&](logic::Graph& g, game::Level&) {
      if (part.nodes.empty() && part.comments.empty()) return false;
      f32 minx = 1e9f, miny = 1e9f;
      for (const logic::Node& n : part.nodes) { minx = min_of(minx, n.x); miny = min_of(miny, n.y); }
      for (const logic::CommentBox& cb : part.comments) { minx = min_of(minx, cb.x); miny = min_of(miny, cb.y); }
      // Functions the nodes name come along where this Blueprint has none of that name; a function
      // has one entry, so an entry of a function that is here already is not pasted again.
      // The enums the nodes are of come along where this Blueprint has none of that name. Where it
      // has one, the pasted nodes mean its values by their names (a name it does not have: its
      // first value) - a value is its place among the values, and the places here may be others.
      part.variables = g.variables;  // (a Set of a variable is of the type the variable has here)
      for (logic::EnumType& e : part.enums) {
        const logic::EnumType* here = g.enumeration(e.name);
        if (!here) {
          if (!g.variable(e.name) && !g.function(e.name) && !g.structure(e.name)) g.enums.push_back(e);
          continue;
        }
        if (here->values == e.values) continue;
        std::vector<i64> place(e.values.size());
        for (std::size_t k = 0; k < place.size(); ++k) {
          const auto at = std::find(here->values.begin(), here->values.end(), e.values[k]);
          place[k] = at == here->values.end() ? -1 : static_cast<i64>(at - here->values.begin());
        }
        move_enum_values(part, e.name, place);
        e.values = here->values;
      }
      // The structs the nodes are of come along where this Blueprint has no type of that name -
      // after the structs here: their fields are of structs that are here already, or come with
      // them. Where it has one, the pasted nodes mean its fields by their names (a value typed in
      // is held by the fields' names; a wire of a field that is not here goes).
      for (const logic::StructType& s : part.structs)
        if (!g.structure(s.name) && !g.enumeration(s.name) && !g.variable(s.name) && !g.function(s.name)) g.structs.push_back(s);
      for (const logic::Function& f : part.functions) if (!g.function(f.name)) g.functions.push_back(f);
      auto has_entry = [&](const std::string& fn) {
        for (const logic::Node& o : g.nodes) if (o.type == "func.entry" && o.param("function") == fn) return true;
        return false;
      };
      std::unordered_map<u32, u32> remap;
      json::Value ids = json::Value::array();
      for (logic::Node n : part.nodes) {
        if (n.type == "func.entry" && has_entry(n.param("function"))) continue;
        remap[n.id] = g.next_id;
        n.id = g.next_id++;
        n.x = n.x - minx + static_cast<f32>(num("x"));
        n.y = n.y - miny + static_cast<f32>(num("y"));
        ids.push(n.id);
        g.nodes.push_back(std::move(n));
      }
      for (const logic::Link& l : part.links)
        if (remap.count(l.from) && remap.count(l.to)) g.links.push_back({remap[l.from], l.out, remap[l.to], l.in});
      if (!part.structs.empty()) {
        settle_struct_values(g);
        drop_struct_wires(g);
      }
      for (logic::CommentBox cb : part.comments) {
        cb.id = g.next_id++;
        cb.x = cb.x - minx + static_cast<f32>(num("x"));
        cb.y = cb.y - miny + static_cast<f32>(num("y"));
        ids.push(cb.id);
        g.comments.push_back(std::move(cb));
      }
      result.set("ids", std::move(ids));
      return true;
    });
  }
  if (name == "logicMove") {
    coalesce_ = "logic-move:" + std::to_string(owner);
    for (const json::Value& m : c["moves"].items()) coalesce_ += "," + std::to_string(static_cast<u32>(m["id"].as_number()));
    transaction_ = str("transaction");
    return graph_edit("Move Nodes", [&](logic::Graph& g, game::Level&) {
      for (const json::Value& m : c["moves"].items()) {
        const u32 id = static_cast<u32>(m["id"].as_number());
        const f32 x = static_cast<f32>(std::round(m["x"].as_number() / 8) * 8), y = static_cast<f32>(std::round(m["y"].as_number() / 8) * 8);
        if (logic::Node* n = g.find(id)) { n->x = x; n->y = y; continue; }
        bool comment = false;
        for (logic::CommentBox& cb : g.comments)
          if (cb.id == id) {
            cb.x = x;
            cb.y = y;
            if (m["w"].is_number()) cb.w = static_cast<f32>(clamp(std::round(m["w"].as_number() / 8) * 8, 64.0, 1e5));
            if (m["h"].is_number()) cb.h = static_cast<f32>(clamp(std::round(m["h"].as_number() / 8) * 8, 48.0, 1e5));
            comment = true;
          }
        if (comment) continue;
        for (logic::StateMachine& sm : g.machines)
          for (logic::State& s : sm.states)
            if (s.id == id) { s.x = x; s.y = y; }
      }
      return true;
    });
  }
  if (name == "logicConnect") {
    return graph_edit("Connect", [&](logic::Graph& g, game::Level&) {
      u32 inserted = 0;
      if (!connect(g, static_cast<u32>(num("from")), str("out"), static_cast<u32>(num("to")), str("in"), error, &inserted)) return false;
      // (a wire of another type: the node that converts it stands on it now - said, with what it can lose)
      if (const logic::Node* n = inserted ? g.find(inserted) : nullptr) {
        result.set("inserted", inserted);
        result.set("title", logic::library().title(*n));
        if (n->type == "num.cast") {
          logic::PinType a, b;
          if (logic::pin_type_from_name(n->param("from"), a) && logic::pin_type_from_name(n->param("to"), b))
            if (const std::string loss = cast_loss(a, b); !loss.empty()) result.set("loss", loss);
        }
      }
      return true;
    });
  }
  if (name == "logicDisconnect") {
    return graph_edit("Break Link", [&](logic::Graph& g, game::Level&) {
      const u32 node = static_cast<u32>(num("node"));
      const std::string pin = str("pin");
      const bool output = c["output"].as_bool();
      const std::size_t before = g.links.size();
      g.links.erase(std::remove_if(g.links.begin(), g.links.end(), [&](const logic::Link& l) {
                      if (c["link"].is_array()) {
                        const json::Value& k = c["link"];
                        return l.from == static_cast<u32>(k[0].as_number()) && l.out == k[1].as_string() && l.to == static_cast<u32>(k[2].as_number()) && l.in == k[3].as_string();
                      }
                      return output ? (l.from == node && l.out == pin) : (l.to == node && l.in == pin);
                    }),
                    g.links.end());
      return g.links.size() != before;
    });
  }
  if (name == "logicDelete") {
    return graph_edit("Delete Nodes", [&](logic::Graph& g, game::Level&) {
      std::vector<u32> ids;
      for (const json::Value& v : c["ids"].items()) {
        // A function's entry stays as long as the function does (it goes with it in My Blueprint).
        const logic::Node* n = g.find(static_cast<u32>(v.as_number()));
        if (n && n->type == "func.entry" && g.function(n->param("function"))) { error = "a function's entry goes with the function: delete the function in My Blueprint"; continue; }
        ids.push_back(static_cast<u32>(v.as_number()));
      }
      auto gone = [&](u32 id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };
      const std::size_t before = g.nodes.size() + g.comments.size();
      g.comments.erase(std::remove_if(g.comments.begin(), g.comments.end(), [&](const logic::CommentBox& cb) { return gone(cb.id); }), g.comments.end());
      g.nodes.erase(std::remove_if(g.nodes.begin(), g.nodes.end(), [&](const logic::Node& n) { return gone(n.id); }), g.nodes.end());
      g.links.erase(std::remove_if(g.links.begin(), g.links.end(), [&](const logic::Link& l) { return gone(l.from) || gone(l.to); }), g.links.end());
      return g.nodes.size() + g.comments.size() != before;
    });
  }
  if (name == "logicValue") {
    coalesce_ = "logic-value:" + std::to_string(owner) + ":" + std::to_string(static_cast<u32>(num("node"))) + ":" + str("pin");
    return graph_edit("Edit Value", [&](logic::Graph& g, game::Level&) {
      logic::Node* n = g.find(static_cast<u32>(num("node")));
      logic::PinDef p;
      if (!n || !pin_of(g, n->id, str("pin"), false, p)) { error = "no such input"; return false; }
      json::Value v;
      if (!read_literal(c["value"], logic::TypeRef{p.type, p.of}, g, v, error)) { error = (p.label.empty() ? p.id : p.label) + ": " + error; return false; }
      n->set_value(p.id, std::move(v));
      return true;
    });
  }
  if (name == "logicParam") {
    return graph_edit("Edit Node", [&](logic::Graph& g, game::Level&) {
      logic::Node* n = g.find(static_cast<u32>(num("node")));
      if (!n) { error = "no such node"; return false; }
      const json::Value& v = c["value"];
      const std::string before_text = n->param("formula", "a + b"), before_types = n->param("types", "f32 f32");
      n->set_param(str("key"), v.is_string() ? v.as_string() : v.is_number() ? std::to_string(static_cast<i64>(v.as_number())) : json::stringify(v));
      // Another formula: its inputs keep their types by their names, a new one is a Float (f32) - its type is said at the
      // pin and changed in Details (the setting Types: one for each input, in the order they first appear)
      if (n->type == "math.formula" && str("key") == "formula") {
        const logic::Formula was(before_text, before_types), now(n->param("formula"), "");
        std::string types;
        for (const std::string& name : now.inputs()) {
          std::string t = "f32";
          for (std::size_t k = 0; k < was.inputs().size() && k < was.types().size(); ++k)
            if (was.inputs()[k] == name) t = std::string(logic::num::kKindNames[static_cast<u8>(was.types()[k])]);
          types += (types.empty() ? "" : " ") + t;
        }
        if (!now.inputs().empty()) n->set_param("types", types);
      }
      // Another variable: the node's path was a way through the old one - it starts at the new one again.
      if (str("key") == "variable" && !n->param("path").empty()) {
        logic::Variable held;
        if (!g.place_of(*n, held)) n->set_param("path", "");
      }
      // The place a call gives a reference input ("<input>.variable", "<input>.path"): it is
      // looked up in the function the call stands in, and another variable starts its path again.
      if (const std::string key = str("key"); (n->type == "func.call" || n->type == "func.pure") && key.find('.') != std::string::npos) {
        const std::string prefix = key.substr(0, key.find('.') + 1);
        n->set_param("scope", g.body_of(n->id, logic::library()));
        logic::Variable held;
        if (key == prefix + "variable" && !n->param(prefix + "path").empty() && !g.place_of(*n, held, nullptr, prefix)) n->set_param(prefix + "path", "");
      }
      // A changed variable / count can remove pins: drop wires to pins that no longer exist.
      std::vector<logic::PinDef> in, out;
      logic::library().pins(*n, g, in, out);
      auto has = [](const std::vector<logic::PinDef>& v, const std::string& id) { for (const auto& p : v) if (p.id == id) return true; return false; };
      g.links.erase(std::remove_if(g.links.begin(), g.links.end(), [&](const logic::Link& l) {
                      return (l.from == n->id && !has(out, l.out)) || (l.to == n->id && !has(in, l.in));
                    }),
                    g.links.end());
      // Another type of a node (its Type, a Cast's To and From, a formula's Types) never leaves a wire of another
      // type on it unseen: the change is refused, and the wire named.
      const logic::NodeDef* def = logic::library().find(n->type);
      bool typed = n->type == "math.formula" && str("key") == "types";
      if (def)
        for (const logic::ParamDef& pd : def->params) typed = typed || (pd.key == str("key") && pd.kind == logic::ParamDef::Kind::type);
      if (typed)
        for (const logic::Link& l : g.links) {
          if (l.from != n->id && l.to != n->id) continue;
          logic::PinDef po, pi;
          if (!pin_of(g, l.from, l.out, true, po) || !pin_of(g, l.to, l.in, false, pi) || po.type == logic::PinType::exec) continue;
          if (logic::fits({po.type, po.of}, {pi.type, pi.of})) continue;
          const bool mine = l.to == n->id;
          const std::string now = c["value"].is_string() ? c["value"].as_string() : json::stringify(c["value"]);
          error = mine ? "the wire into " + (pi.label.empty() ? pi.id : pi.label) + " brings " + an(logic::type_label({po.type, po.of})) + ": with " + str("key") + " " + now +
                             " the pin is " + an(logic::type_label({pi.type, pi.of}))
                       : "the wire from " + (po.label.empty() ? std::string("the result") : po.label) + " goes into " + an(logic::type_label({pi.type, pi.of})) + " pin: with " +
                             str("key") + " " + now + " it is " + an(logic::type_label({po.type, po.of}));
          error += " - break the wire first, or put a Cast on it";
          return false;
        }
      return true;
    });
  }
  if (name == "logicComment") {
    return graph_edit("Comment", [&](logic::Graph& g, game::Level&) {
      logic::Node* n = g.find(static_cast<u32>(num("node")));
      if (!n) return false;
      n->comment = str("text");
      return true;
    });
  }
  if (name == "logicImplements") {
    // The interface of the engine this Blueprint is the object of: `name` (empty: none). Its functions are the
    // Blueprint's of the names of the interface's (check() says what is missing or does not fit).
    const std::string which = str("name");
    return graph_edit("Implements", [&](logic::Graph& g, game::Level&) {
      if (g.implements == which) return false;
      if (!which.empty()) {
        const logic::EngineInterface* const i = logic::engine_interface(which);
        if (!i) { error = which + " is no interface of the engine"; return false; }
        if (*i->waits) { error = "no Blueprint implements " + which + " in this build: " + i->waits; return false; }
        if (g.library) { error = "a library is no object: it implements no interface of the engine"; return false; }
      }
      g.implements = which;
      return true;
    });
  }
  if (name == "logicLibrary") {
    // A library is a Blueprint that keeps nothing; other Blueprints use it - they have its types
    // and call its functions directly.
    //   library  this Blueprint is one (value: true / false)
    //   use      it uses the library `name` (the Blueprint of the actor of that name)
    //   unuse    it does not use it any more (while it calls none of its functions and has nothing of its types)
    const std::string op = str("op"), which = str("name");
    return graph_edit(op == "library" ? "Library" : "Use Library", [&](logic::Graph& g, game::Level& level) {
      if (op == "library") {
        const bool on = c["value"].as_bool();
        if (g.library == on) return false;
        if (on && !owner) { error = "the level's own Blueprint is no library: a library is used by the name of its actor"; return false; }
        g.library = on;
        return true;
      }
      if (op == "use") {
        const game::Actor* self = owner ? level.find(owner) : nullptr;
        if (g.uses_library(which)) return false;
        if (self && self->name == which) { error = "a Blueprint does not use itself"; return false; }
        g.uses.push_back(which);  // (that it is there, and a library, says take_libraries - after the edit)
        return true;
      }
      if (op == "unuse") {
        if (!g.uses_library(which)) return false;
        std::erase(g.uses, which);
        // (what the Blueprint still has of it would be of nothing: the edit is refused then)
        if (!logic::take_libraries(g, blueprints_of(level, logic_project()), &error)) return false;
        logic::Graph probe;
        std::string why;
        if (!logic::graph_from_json(logic::graph_to_json(g), probe, &why)) { error = "the Blueprint still uses a type of the library " + which + ": " + why; return false; }
        return true;
      }
      error = "unknown library edit " + op;
      return false;
    });
  }
  if (name == "logicFunction") {
    // A function of the graph: its name, whether it is offered as a pure node, its help and its
    // pins. The function's nodes (entry, Return, calls) follow every change.
    const std::string op = str("op");
    return graph_edit("Edit Function", [&](logic::Graph& g, game::Level& level) {
      const std::string fname = str("name");
      if (op == "declare") {
        // A function of another Blueprint of the level, declared here as it is there: this
        // Blueprint can then call it on an actor. Its pins' types are types this Blueprint has.
        logic::Graph* from = graph_of(level, static_cast<u32>(num("from")), false);
        const logic::Function* of = from && from != &g ? from->function(fname) : nullptr;
        if (!of || of->external) { error = "that Blueprint has no function " + fname; return false; }
        if (g.function(fname) || g.variable(fname) || g.enumeration(fname) || g.structure(fname) || is_local(g, fname)) { error = "this Blueprint has something called " + fname + " already"; return false; }
        if (const std::string why = name_problem(g, fname); !why.empty()) { error = why; return false; }
        logic::Function f;
        f.name = of->name;
        f.help = of->help;
        f.external = true;
        f.inputs = of->inputs;
        f.outputs = of->outputs;
        for (const auto* pins : {&f.inputs, &f.outputs})
          for (const logic::FunctionPin& p : *pins) {
            if (p.of.empty()) continue;
            // (its types come along: they are types of the project then, the same here and there)
            if (const std::string why = adopt_project_type(g, *from, p.of); !why.empty()) {
              error = "\"" + p.name + "\" of " + fname + " is of the type " + p.of + ": " + why;
              return false;
            }
          }
        for (const logic::FunctionPin& p : f.inputs)
          if (g.variable(p.name) || g.function(p.name)) { error = "the input \"" + p.name + "\" of " + fname + " has the name of a variable or a function of this Blueprint"; return false; }
        g.functions.push_back(std::move(f));
        result.set("function", fname);
        return true;
      }
      logic::Function* f = g.function(fname);
      if (!f) { error = "no function " + fname; return false; }
      if (!f->from.empty()) { error = fname + " is a function of the library " + f->from + ": it is changed there"; return false; }
      auto uses = [&](const logic::Node& n) {
        return (n.type == "func.entry" || n.type == "func.return" || n.type == "func.call" || n.type == "func.pure" || n.type == "func.call_on") && n.param("function") == fname;
      };
      // the nodes of its local variables (all of them, or of the one called `local`)
      auto of_local = [&](const logic::Node& n, const std::string& local = {}) {
        return n.param("scope") == fname && (local.empty() || n.param("variable") == local);
      };
      // a pin's side on a node: the function's inputs leave the entry and enter the calls; the
      // outputs enter the Returns and leave the calls
      auto is_input_pin_of = [&](const logic::Node& n, bool input_side) { return input_side ? n.type != "func.entry" : n.type == "func.return"; };
      auto drop_links = [&](auto&& gone) {
        g.links.erase(std::remove_if(g.links.begin(), g.links.end(), gone), g.links.end());
      };
      if (op == "remove") {
        std::vector<u32> ids;
        for (const logic::Node& n : g.nodes) if (uses(n) || of_local(n)) ids.push_back(n.id);
        auto has = [&](u32 id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };
        drop_links([&](const logic::Link& l) { return has(l.from) || has(l.to); });
        g.nodes.erase(std::remove_if(g.nodes.begin(), g.nodes.end(), [&](const logic::Node& n) { return has(n.id); }), g.nodes.end());
        g.functions.erase(std::remove_if(g.functions.begin(), g.functions.end(), [&](const logic::Function& x) { return x.name == fname; }), g.functions.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("to");
        if (to.empty() || to.size() > 64 || g.function(to) || g.variable(to)) { error = "that name is empty or taken"; return false; }
        if (is_local(g, to)) { error = "a function has a local variable of that name"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        for (logic::Node& n : g.nodes) {
          if (uses(n)) n.set_param("function", to);
          if (of_local(n)) n.set_param("scope", to);
        }
        f->name = to;
        return true;
      }
      if (op == "external") {
        // A function of another Blueprint: what it looks like stays, its body goes - the entry,
        // the Returns, the calls of it as one of this Blueprint's own, its local variables. And
        // back: a function of this Blueprint again, with an entry and a Return.
        const bool on = c["value"].as_bool();
        if (on == f->external) return true;
        if (on) {
          std::vector<u32> ids;
          for (const logic::Node& n : g.nodes) if ((uses(n) && n.type != "func.call_on") || of_local(n)) ids.push_back(n.id);
          auto has = [&](u32 id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };
          drop_links([&](const logic::Link& l) { return has(l.from) || has(l.to); });
          g.nodes.erase(std::remove_if(g.nodes.begin(), g.nodes.end(), [&](const logic::Node& n) { return has(n.id); }), g.nodes.end());
          f->locals.clear();
          f->pure = false;
          f->external = true;
          return true;
        }
        f->external = false;
        logic::Node entry, ret;
        entry.id = g.next_id++;
        entry.type = "func.entry";
        entry.x = static_cast<f32>(num("x"));
        entry.y = static_cast<f32>(num("y"));
        entry.set_param("function", fname);
        ret.id = g.next_id++;
        ret.type = "func.return";
        ret.x = entry.x + 320;
        ret.y = entry.y;
        ret.set_param("function", fname);
        g.links.push_back({entry.id, "then", ret.id, "exec"});
        result.set("id", entry.id);
        g.nodes.push_back(std::move(entry));
        g.nodes.push_back(std::move(ret));
        return true;
      }
      if (op == "pure") {
        if (f->external && c["value"].as_bool()) { error = "a function of another Blueprint is called as a step: what it does is not known here"; return false; }
        // The calls change their kind: a pure node has no exec pins.
        f->pure = c["value"].as_bool();
        for (logic::Node& n : g.nodes) {
          if (!uses(n) || (n.type != "func.call" && n.type != "func.pure")) continue;
          n.type = f->pure ? "func.pure" : "func.call";
          if (f->pure) {
            const u32 id = n.id;
            drop_links([&](const logic::Link& l) { return (l.to == id && l.in == "exec") || (l.from == id && l.out == "then"); });
          }
        }
        return true;
      }
      if (op == "overrides") {
        // (it is a function of the interface the Blueprint implements: the engine calls it - Function::overrides;
        // check() says where that does not fit)
        if (f->external) { error = "a function of another Blueprint is not this one's to override with"; return false; }
        f->overrides = c["value"].as_bool();
        return true;
      }
      if (op == "help") { f->help = str("text"); return true; }
      // (side "local": its local variables - edited like its pins; their nodes are the variable
      // nodes that name the function as their "scope")
      const bool locals = str("side") == "local";
      if (locals && f->external) { error = "a function of another Blueprint has no local variables here"; return false; }
      const bool input_side = !locals && str("side") != "out";
      std::vector<logic::FunctionPin>& pins = locals ? f->locals : input_side ? f->inputs : f->outputs;
      auto valid_name = [&](const std::string& n) {
        if (n.empty() || n.size() > 64 || n == "exec" || n == "then") return false;
        for (const logic::FunctionPin& p : pins) if (p.name == n) return false;
        // (an input is a parameter in the function's C++: it cannot hide a variable or a function;
        // a local variable is a local there: it hides nothing, and no pin hides it)
        if ((input_side || locals) && (g.variable(n) || g.function(n))) return false;
        if (locals && (f->input(n) || f->output(n) || g.enumeration(n) || g.structure(n))) return false;
        if (!locals && f->local(n)) return false;
        return name_problem(g, n).empty();
      };
      if (op == "pinAdd") {
        if (pins.size() >= (locals ? 64u : 16u)) { error = locals ? "a function has at most 64 local variables" : "a function has at most 16 inputs and 16 outputs"; return false; }
        std::string base = str("pin").empty() ? (locals ? "local" : input_side ? "value" : "result") : str("pin"), pname = base;
        for (int i = 2; !valid_name(pname) && i < 100; ++i) pname = base + std::to_string(i);
        logic::TypeRef t;
        if (!logic::type_from_text(str("type").empty() ? "f32" : str("type"), g, t)) { error = "unknown type"; return false; }
        if (!with_count(t, error)) return false;
        pins.push_back({pname, t.type, logic::zero_of(t, g).to_json(g, t), t.of, t.count});
        result.set("pin", pname);
        return true;
      }
      const std::string pin = str("pin");
      auto it = std::find_if(pins.begin(), pins.end(), [&](const logic::FunctionPin& x) { return x.name == pin; });
      if (it == pins.end()) { error = "no pin " + pin; return false; }
      // the wires and the typed-in values of this pin on the function's nodes
      auto on_pin = [&](const logic::Link& l, bool& as_input) {
        if (locals) return false;  // (a local variable is no pin)
        const logic::Node* to = g.find(l.to);
        const logic::Node* from = g.find(l.from);
        if (to && uses(*to) && l.in == pin && is_input_pin_of(*to, input_side)) { as_input = true; return true; }
        if (from && uses(*from) && l.out == pin && !is_input_pin_of(*from, input_side)) { as_input = false; return true; }
        return false;
      };
      if (op == "pinRemove") {
        drop_links([&](const logic::Link& l) { bool in; return on_pin(l, in); });
        for (logic::Node& n : g.nodes)
          if (!locals && uses(n) && is_input_pin_of(n, input_side))
            n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == pin; }), n.values.end());
        // (a reference input: the places the calls named for it go with it)
        if (input_side && it->ref) {
          drop_links([&](const logic::Link& l) { return g.find(l.to) && uses(*g.find(l.to)) && l.in.rfind(pin + ".", 0) == 0; });
          for (logic::Node& n : g.nodes)
            if (uses(n)) std::erase_if(n.params, [&](const auto& p) { return p.first.rfind(pin + ".", 0) == 0; });
        }
        pins.erase(it);
        return true;
      }
      if (op == "pinRename") {
        const std::string to = str("to");
        if (!valid_name(to)) { error = name_problem(g, to).empty() ? "that name is empty or taken" : name_problem(g, to); return false; }
        for (logic::Link& l : g.links) {
          bool as_input = false;
          if (!on_pin(l, as_input)) continue;
          (as_input ? l.in : l.out) = to;
        }
        for (logic::Node& n : g.nodes) {
          if (locals) { if (of_local(n, pin)) n.set_param("variable", to); continue; }
          if (uses(n) && is_input_pin_of(n, input_side))
            for (auto& v : n.values) if (v.first == pin) v.first = to;
          if (!input_side) continue;
          // an input named like a variable in the function (a reference, a place in an input by
          // value), the places the function's own calls name in it, and what calls name for it
          if (of_local(n, pin)) n.set_param("variable", to);
          if (n.param("scope") == fname)
            for (auto& p : n.params)
              if (p.first.size() > 9 && p.first.compare(p.first.size() - 9, 9, ".variable") == 0 && p.second == pin) p.second = to;
          if (!uses(n)) continue;
          for (auto& p : n.params) if (p.first.rfind(pin + ".", 0) == 0) p.first = to + p.first.substr(pin.size());
          for (auto& v : n.values) if (v.first.rfind(pin + ".", 0) == 0) v.first = to + v.first.substr(pin.size());
          for (logic::Link& l : g.links) if (l.to == n.id && l.in.rfind(pin + ".", 0) == 0) l.in = to + l.in.substr(pin.size());
        }
        it->name = to;
        return true;
      }
      if (op == "pinRef") {
        // How the function takes an input: its value ("") - or the place the call names, to change
        // it ("ref": Car& car) or to read it where it stands ("const": const Car& car).
        if (!input_side) { error = "a reference is an input: the place a call names"; return false; }
        const std::string how = str("ref");
        const bool was = it->ref;
        it->ref = how == "ref" || how == "const";
        it->constant = how == "const";
        if (it->ref == was) return true;
        logic::order_inputs(*f);  // (the inputs by reference come first: it goes behind the last of them, or before the first value)
        // (the pin is gone or back: so are its wires and typed-in values - and the places the calls named for it)
        drop_links([&](const logic::Link& l) { bool in; return on_pin(l, in) || (g.find(l.to) && uses(*g.find(l.to)) && l.in.rfind(pin + ".", 0) == 0); });
        for (logic::Node& n : g.nodes) {
          if (!uses(n)) continue;
          std::erase_if(n.values, [&](const auto& v) { return v.first == pin || v.first.rfind(pin + ".", 0) == 0; });
          std::erase_if(n.params, [&](const auto& p) { return p.first.rfind(pin + ".", 0) == 0; });
        }
        return true;
      }
      if (op == "pinType") {
        logic::TypeRef t;
        if (!logic::type_from_text(str("type"), g, t)) { error = "unknown type"; return false; }
        if (!with_count(t, error)) return false;
        it->value = retyped_default(g, it->value, logic::declared(*it), t);
        it->type = t.type;
        it->of = t.of;
        it->count = t.count;
        // values typed into the nodes were of the old type
        for (logic::Node& n : g.nodes)
          if (!locals && uses(n) && is_input_pin_of(n, input_side))
            n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == pin; }), n.values.end());
        return true;
      }
      if (op == "pinValue") {
        json::Value v;
        if (!read_literal(c["value"], logic::declared(*it), g, v, error)) { error = it->name + ": " + error; return false; }
        it->value = std::move(v);
        return true;
      }
      if (op == "pinMove") {
        const std::ptrdiff_t at = it - pins.begin(), to = at + (num("by") < 0 ? -1 : 1);
        if (to < 0 || to >= static_cast<std::ptrdiff_t>(pins.size())) return false;
        // (the inputs by reference come first: a value does not go before one, nor a reference behind a value)
        if (input_side && !locals && pins[static_cast<std::size_t>(at)].ref != pins[static_cast<std::size_t>(to)].ref) {
          error = "the inputs by reference come first (a value has a default, and in C++ every input after one with a default has one)";
          return false;
        }
        std::swap(pins[static_cast<std::size_t>(at)], pins[static_cast<std::size_t>(to)]);
        return true;
      }
      error = "unknown function edit " + op;
      return false;
    });
  }
  if (name == "logicVariable") {
    const std::string op = str("op");
    return graph_edit("Edit Variable", [&](logic::Graph& g, game::Level&) {
      const std::string var = str("name");
      if (op == "add") {
        std::string base = var.empty() ? "NewVar" : var, n = base;
        if (const std::string why = name_problem(g, base); !why.empty() && !var.empty()) { error = why; return false; }
        for (int i = 2; g.variable(n) || g.function(n) || g.enumeration(n) || g.structure(n) || is_local(g, n); ++i) n = base + std::to_string(i);
        logic::TypeRef t;
        if (!logic::type_from_text(str("type").empty() ? "f32" : str("type"), g, t)) { error = "unknown type"; return false; }
        if (!with_count(t, error)) return false;
        g.variables.push_back({n, t.type, logic::zero_of(t, g).to_json(g, t), {}, t.of, t.count});
        result.set("name", n);
        return true;
      }
      logic::Variable* v = g.variable(var);
      if (!v) { error = "no variable " + var; return false; }
      if (op == "remove") {
        g.variables.erase(std::remove_if(g.variables.begin(), g.variables.end(), [&](const logic::Variable& x) { return x.name == var; }), g.variables.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("to");
        if (to.empty() || to.size() > 64 || g.variable(to) || g.function(to) || g.enumeration(to) || g.structure(to)) { error = "that name is empty or taken"; return false; }
        if (is_local(g, to)) { error = "a function has a local variable of that name"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        for (logic::Node& n : g.nodes)
          if (n.param("variable") == var && n.param("scope").empty()) n.set_param("variable", to);
        for (logic::StateMachine& m : g.machines)
          for (logic::Transition& t : m.transitions)
            if (t.variable == var) t.variable = to;
        v->name = to;
        return true;
      }
      if (op == "type") {
        logic::TypeRef t;
        if (!logic::type_from_text(str("type"), g, t)) { error = "unknown type"; return false; }
        if (!with_count(t, error)) return false;
        v->value = retyped_default(g, v->value, logic::declared(*v), t);
        v->type = t.type;
        v->of = t.of;
        v->count = t.count;
        return true;
      }
      if (op == "value") {
        json::Value x;
        if (!read_literal(c["value"], logic::declared(*v), g, x, error)) { error = v->name + ": " + error; return false; }
        v->value = std::move(x);
        return true;
      }
      if (op == "constant") {
        // A constant is read like a variable and never changed (the nodes that would change it are
        // errors when the Blueprint compiles: they say where).
        const bool on = c["value"].as_bool();
        if (v->constant == on) return false;
        v->constant = on;
        return true;
      }
      error = "unknown variable operation";
      return false;
    });
  }
  if (name == "logicEnum") {
    // An enum of the Blueprint: its name, its values (their order is their meaning: a value is
    // its place among them - what is typed into variables and pins follows when values move).
    const std::string op = str("op");
    return type_edit("Edit Enum", [&](logic::Graph& g, game::Level&) {
      const std::string which = str("name");
      auto taken = [&](const std::string& n) { return g.variable(n) || g.function(n) || g.enumeration(n) || g.structure(n) || is_local(g, n); };
      if (op == "add") {
        std::string base = which.empty() ? "NewEnum" : which, n = base;
        if (const std::string why = name_problem(g, base); !why.empty() && !which.empty()) { error = why; return false; }
        if (const std::string why = type_name_problem(g, base); !why.empty()) { error = why; return false; }
        for (int i = 2; taken(n); ++i) n = base + std::to_string(i);
        g.enums.push_back({n, {"First", "Second"}, {}});
        result.set("name", n);
        return true;
      }
      logic::EnumType* e = g.enumeration(which);
      if (!e) { error = "no enum " + which; return false; }
      if (!e->from.empty()) { error = which + " is a type of the library " + e->from + ": it is changed there"; return false; }
      const std::string old = e->name;
      // The settings of nodes that name this enum (a type setting: "Mode", "Mode_list").
      auto each_setting = [&](const std::function<void(logic::Node&, const logic::ParamDef&, bool list)>& fn) {
        for (logic::Node& n : g.nodes) {
          const logic::NodeDef* d = logic::library().find(n.type);
          if (!d) continue;
          for (const logic::ParamDef& pd : d->params) {
            if (pd.kind != logic::ParamDef::Kind::type) continue;
            const std::string v = n.param(pd.key, pd.def);
            if (v == old) fn(n, pd, false);
            else if (v == old + "_list") fn(n, pd, true);
          }
        }
      };
      if (op == "remove") {
        std::string user;
        for (const logic::Variable& v : g.variables) if (user.empty() && names_type(logic::declared(v), old)) user = "the variable " + v.name;
        for (const logic::Function& f : g.functions)
          for (const auto* pins : {&f.inputs, &f.outputs, &f.locals})
            for (const logic::FunctionPin& fp : *pins) if (user.empty() && names_type(logic::declared(fp), old)) user = "\"" + fp.name + "\" of the function " + f.name;
        for (const logic::StructType& s : g.structs)
          for (const logic::StructField& f : s.fields) if (user.empty() && names_type(f.type, old)) user = "the field " + f.name + " of the struct " + s.name;
        each_setting([&](logic::Node& n, const logic::ParamDef&, bool) { if (user.empty()) user = logic::library().title(n); });
        if (!user.empty()) { error = old + " is in use: " + user; return false; }
        g.enums.erase(std::remove_if(g.enums.begin(), g.enums.end(), [&](const logic::EnumType& x) { return x.name == old; }), g.enums.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("to");
        if (to == old) return true;
        if (to.empty() || to.size() > 64 || taken(to)) { error = "that name is empty or taken"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        if (const std::string why = type_name_problem(g, to); !why.empty()) { error = why; return false; }
        each_setting([&](logic::Node& n, const logic::ParamDef& pd, bool list) { n.set_param(pd.key, list ? to + "_list" : to); });
        for (logic::Variable& v : g.variables) v.of = logic::type_renamed(logic::declared(v), old, to).of;  // (also where a map's keys or values are of it)
        for (logic::Function& f : g.functions)
          for (auto* pins : {&f.inputs, &f.outputs, &f.locals})
            for (logic::FunctionPin& fp : *pins) fp.of = logic::type_renamed(logic::declared(fp), old, to).of;
        for (logic::StructType& s : g.structs)
          for (logic::StructField& f : s.fields) f.type = logic::type_renamed(f.type, old, to);
        e->name = to;
        return true;
      }
      if (op == "help") { e->help = str("text"); return true; }
      auto value_name_ok = [&](const std::string& n) {
        if (!logic::cpp_name_ok(n)) { error = "\"" + n + "\" cannot be a value's name: letters, digits and _ (no spaces), not a word of C++"; return false; }
        if (std::find(e->values.begin(), e->values.end(), n) != e->values.end()) { error = old + " has a value " + n + " already"; return false; }
        return true;
      };
      if (op == "addValue") {
        std::string base = str("value").empty() ? "Value" : str("value"), n = base;
        if (!str("value").empty() && !value_name_ok(n)) return false;
        for (int i = 2; std::find(e->values.begin(), e->values.end(), n) != e->values.end(); ++i) n = base + std::to_string(i);
        if (e->values.size() >= 256) { error = "an enum has at most 256 values"; return false; }
        e->values.push_back(n);
        return true;
      }
      const std::size_t index = static_cast<std::size_t>(max_of(0.0, num("index")));
      if (index >= e->values.size()) { error = old + " has no value " + std::to_string(index); return false; }
      if (op == "renameValue") {
        const std::string to = str("to");
        if (to == e->values[index]) return true;
        if (!value_name_ok(to)) return false;
        e->values[index] = to;
        return true;
      }
      if (op == "removeValue" || op == "moveValue") {
        // where each value is afterwards (a removed one: nowhere)
        std::vector<i64> place(e->values.size());
        for (std::size_t k = 0; k < place.size(); ++k) place[k] = static_cast<i64>(k);
        std::size_t other = index;
        if (op == "removeValue") {
          if (e->values.size() <= 1) { error = "an enum has at least one value"; return false; }
          for (std::size_t k = 0; k < place.size(); ++k) place[k] = k == index ? -1 : static_cast<i64>(k > index ? k - 1 : k);
        } else {
          other = num("by") < 0 ? index - 1 : index + 1;
          if ((num("by") < 0 && index == 0) || other >= e->values.size()) return false;
          std::swap(place[index], place[other]);
        }
        move_enum_values(g, old, place);
        if (op == "removeValue") e->values.erase(e->values.begin() + static_cast<std::ptrdiff_t>(index));
        else std::swap(e->values[index], e->values[other]);
        return true;
      }
      error = "unknown enum operation";
      return false;
    });
  }
  if (name == "logicStruct") {
    // A struct of the Blueprint: its name and its fields - each with a name, a type and a default.
    // Everything of the struct's type follows a change: a value is held by its fields' names, so a
    // renamed field keeps its values and its wires on Make and Break, a field that goes takes
    // them along, and a field that moves changes nothing but its place.
    const std::string op = str("op");
    return type_edit("Edit Struct", [&](logic::Graph& g, game::Level&) {
      const std::string which = str("name");
      auto taken = [&](const std::string& n) { return g.variable(n) || g.function(n) || g.enumeration(n) || g.structure(n) || is_local(g, n); };
      if (op == "add") {
        std::string base = which.empty() ? "NewStruct" : which, n = base;
        if (const std::string why = name_problem(g, base); !why.empty() && !which.empty()) { error = why; return false; }
        if (const std::string why = type_name_problem(g, base); !why.empty()) { error = why; return false; }
        for (int i = 2; taken(n); ++i) n = base + std::to_string(i);
        g.structs.push_back({n, {{"Field", {logic::kFloat, {}}, json::Value(0.0)}}, {}});
        result.set("name", n);
        return true;
      }
      logic::StructType* s = g.structure(which);
      if (!s) { error = "no struct " + which; return false; }
      if (!s->from.empty()) { error = which + " is a type of the library " + s->from + ": it is changed there"; return false; }
      const std::string old = s->name;
      // The settings of nodes that name this struct (a type setting: "Weapon", "Weapon_list").
      auto each_setting = [&](const std::function<void(logic::Node&, const logic::ParamDef&, bool list)>& fn) {
        for (logic::Node& n : g.nodes) {
          const logic::NodeDef* d = logic::library().find(n.type);
          if (!d) continue;
          for (const logic::ParamDef& pd : d->params) {
            if (pd.kind != logic::ParamDef::Kind::type) continue;
            const std::string v = n.param(pd.key, pd.def);
            if (v == old) fn(n, pd, false);
            else if (v == old + "_list") fn(n, pd, true);
          }
        }
      };
      if (op == "remove") {
        std::string user;
        for (const logic::Variable& v : g.variables) if (user.empty() && names_type(logic::declared(v), old)) user = "the variable " + v.name;
        for (const logic::Function& f : g.functions)
          for (const auto* pins : {&f.inputs, &f.outputs, &f.locals})
            for (const logic::FunctionPin& fp : *pins) if (user.empty() && names_type(logic::declared(fp), old)) user = "\"" + fp.name + "\" of the function " + f.name;
        for (const logic::StructType& other : g.structs)
          for (const logic::StructField& f : other.fields) if (user.empty() && names_type(f.type, old)) user = "the field " + f.name + " of the struct " + other.name;
        each_setting([&](logic::Node& n, const logic::ParamDef&, bool) { if (user.empty()) user = logic::library().title(n); });
        if (!user.empty()) { error = old + " is in use: " + user; return false; }
        g.structs.erase(std::remove_if(g.structs.begin(), g.structs.end(), [&](const logic::StructType& x) { return x.name == old; }), g.structs.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("to");
        if (to == old) return true;
        if (to.empty() || to.size() > 64 || taken(to)) { error = "that name is empty or taken"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        if (const std::string why = type_name_problem(g, to); !why.empty()) { error = why; return false; }
        each_setting([&](logic::Node& n, const logic::ParamDef& pd, bool list) { n.set_param(pd.key, list ? to + "_list" : to); });
        for (logic::Variable& v : g.variables) v.of = logic::type_renamed(logic::declared(v), old, to).of;  // (also where a map's keys or values are of it)
        for (logic::Function& f : g.functions)
          for (auto* pins : {&f.inputs, &f.outputs, &f.locals})
            for (logic::FunctionPin& fp : *pins) fp.of = logic::type_renamed(logic::declared(fp), old, to).of;
        for (logic::StructType& other : g.structs)
          for (logic::StructField& f : other.fields) f.type = logic::type_renamed(f.type, old, to);
        s->name = to;
        return true;
      }
      if (op == "help") { s->help = str("text"); return true; }
      auto field_name_ok = [&](const std::string& n) {
        if (!logic::cpp_name_ok(n)) { error = "\"" + n + "\" cannot be a field's name: letters, digits and _ (no spaces), not a word of C++"; return false; }
        if (g.enumeration(n) || g.structure(n)) { error = "\"" + n + "\" is a type of this Blueprint"; return false; }
        if (s->field(n) >= 0) { error = old + " has a field " + n + " already"; return false; }
        return true;
      };
      // The type a field can have: a value type, an enum, another struct that does not hold this
      // one - or a list of one of these. (A struct holds structs declared above it: where the
      // wanted one stands below, this struct moves below it - with the structs that hold it.)
      auto field_type = [&](const std::string& text, logic::TypeRef& t) {
        if (!logic::type_from_text(text.empty() ? "f32" : text, g, t)) { error = "unknown type"; return false; }
        if (!with_count(t, error)) return false;
        for (const logic::TypeRef& named : logic::types_named(t)) {  // (a struct, a list of one, a map of one)
          if (logic::element_of(named.type) != logic::PinType::structure) continue;
          const logic::StructType* inner = g.structure(named.of);
          if (named.of == old) { error = "a struct cannot contain itself"; return false; }
          const std::vector<std::string> holds = inner ? logic::structs_in(*inner, g) : std::vector<std::string>();
          if (std::find(holds.begin(), holds.end(), old) != holds.end()) { error = named.of + " holds " + old + ": a struct cannot contain itself"; return false; }
        }
        return true;
      };
      /// Puts the structs in an order in which each stands after the structs its fields are of
      /// (as few of them move as it takes).
      auto order_structs = [&] {
        std::vector<logic::StructType> sorted;
        std::vector<logic::StructType> rest = std::move(g.structs);
        while (!rest.empty()) {
          bool placed = false;
          for (std::size_t k = 0; k < rest.size() && !placed; ++k) {
            bool ready = true;
            for (const logic::StructField& f : rest[k].fields)
              for (const logic::TypeRef& named : logic::types_named(f.type))
                if (logic::element_of(named.type) == logic::PinType::structure)
                  ready = ready && std::any_of(sorted.begin(), sorted.end(), [&](const logic::StructType& x) { return x.name == named.of; });
            if (!ready) continue;
            sorted.push_back(std::move(rest[k]));
            rest.erase(rest.begin() + static_cast<std::ptrdiff_t>(k));
            placed = true;
          }
          if (!placed) break;  // (never: field_type refuses a struct that would hold itself)
        }
        for (logic::StructType& x : rest) sorted.push_back(std::move(x));
        g.structs = std::move(sorted);
      };
      if (op == "addField") {
        if (s->fields.size() >= logic::StructType::kMaxFields) { error = "a struct has at most " + std::to_string(logic::StructType::kMaxFields) + " fields"; return false; }
        std::string base = str("field").empty() ? "Field" : str("field"), n = base;
        if (!str("field").empty() && !field_name_ok(n)) return false;
        for (int i = 2; s->field(n) >= 0 || g.enumeration(n) || g.structure(n); ++i) n = base + std::to_string(i);
        logic::TypeRef t;
        if (!field_type(str("type"), t)) return false;
        s->fields.push_back({n, t, logic::zero_of(t, g).to_json(g, t)});
        result.set("field", n);
        order_structs();
        settle_struct_values(g);
        return true;
      }
      const int index = s->field(str("field"));
      if (index < 0) { error = old + " has no field " + str("field"); return false; }
      const std::size_t at = static_cast<std::size_t>(index);
      const std::string field = s->fields[at].name;
      // the struct's own nodes: Make and Break have a pin for each field, Find Row names one
      auto is_node = [&](const logic::Node& n, const char* type) { return n.type == type && n.param("struct") == old; };
      auto finds_by_field = [&](const logic::Node& n) { return n.type == "list.find_row" && n.param("struct") == old && n.param("field") == field; };
      // ... and the nodes whose place leads through the field (Held.Damage, Cars[].Wheels[].Slip):
      // the steps of a node's path that are this field (none: the node has nothing to do with it)
      auto steps_through = [&](const logic::Node& n, std::vector<std::string>& steps) {
        std::vector<std::string> owners;
        std::vector<std::size_t> at_steps;
        if (n.param("path").empty() || !g.path_of(n, steps, owners)) return at_steps;
        for (std::size_t k = 0; k < steps.size(); ++k)
          if (steps[k] == field && owners[k] == old) at_steps.push_back(k);
        return at_steps;
      };
      // a Set of the field itself: its place ends there
      auto sets_field = [&](const logic::Node& n) {
        std::vector<std::string> steps;
        const std::vector<std::size_t> through = steps_through(n, steps);
        return n.type == "var.set" && !through.empty() && through.back() + 1 == steps.size();
      };
      if (op == "renameField") {
        const std::string to = str("to");
        if (to == field) return true;
        if (!field_name_ok(to)) return false;
        // every value of the struct holds the field under its name
        each_held_value(g, [&](json::Value& value, const logic::TypeRef& t) {
          each_struct_in(g, value, t, old, [&](json::Value& object) {
            if (const json::Value* held = object.find(field)) {
              const json::Value kept = *held;
              object.remove(field);
              object.set(to, kept);
            }
          });
        });
        for (logic::Node& n : g.nodes) {
          if (finds_by_field(n)) n.set_param("field", to);
          std::vector<std::string> steps;
          if (const std::vector<std::size_t> through = steps_through(n, steps); !through.empty()) {
            for (std::size_t k : through) steps[k] = to;
            n.set_param("path", logic::path_text(steps));
          }
          if (!is_node(n, "struct.make")) continue;
          for (auto& v : n.values) if (v.first == field) v.first = to;
        }
        for (logic::Link& l : g.links) {
          const logic::Node* to_node = g.find(l.to);
          const logic::Node* from_node = g.find(l.from);
          if (to_node && is_node(*to_node, "struct.make") && l.in == field) l.in = to;
          if (from_node && is_node(*from_node, "struct.break") && l.out == field) l.out = to;
        }
        s->fields[at].name = to;
        settle_struct_values(g);
        return true;
      }
      if (op == "removeField") {
        if (s->fields.size() <= 1) { error = "a struct has at least one field"; return false; }
        for (logic::Node& n : g.nodes)
          if (is_node(n, "struct.make")) n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == field; }), n.values.end());
        s->fields.erase(s->fields.begin() + static_cast<std::ptrdiff_t>(at));
        settle_struct_values(g);
        drop_struct_wires(g);
        return true;
      }
      if (op == "moveField") {
        const std::size_t other = num("by") < 0 ? at - 1 : at + 1;
        if ((num("by") < 0 && at == 0) || other >= s->fields.size()) return false;
        std::swap(s->fields[at], s->fields[other]);
        return true;
      }
      if (op == "fieldType") {
        logic::TypeRef t;
        if (!field_type(str("type"), t)) return false;
        const logic::TypeRef from = s->fields[at].type;
        if (from == t) return true;
        // what the field holds everywhere: kept where the types convert (as a variable's default is)
        each_held_value(g, [&](json::Value& value, const logic::TypeRef& held_type) {
          each_struct_in(g, value, held_type, old, [&](json::Value& object) {
            if (const json::Value* held = object.find(field)) object.set(field, retyped_default(g, *held, from, t));
          });
        });
        for (logic::Node& n : g.nodes) {
          if (is_node(n, "struct.make"))
            for (auto& v : n.values) if (v.first == field) v.second = retyped_default(g, v.second, from, t);
          if (sets_field(n) || finds_by_field(n)) n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == "value"; }), n.values.end());
        }
        s->fields[at].value = retyped_default(g, s->fields[at].value, from, t);
        s->fields[at].type = t;
        order_structs();
        settle_struct_values(g);
        drop_struct_wires(g);
        return true;
      }
      if (op == "fieldValue") {
        const logic::TypeRef t = s->fields[at].type;
        json::Value v;
        if (!read_literal(c["value"], t, g, v, error)) { error = s->fields[at].name + ": " + error; return false; }
        s->fields[at].value = std::move(v);
        settle_struct_values(g);
        return true;
      }
      error = "unknown struct operation";
      return false;
    });
  }
  if (name == "logicMachine") {
    const std::string op = str("op");
    return graph_edit("Edit State Machine", [&](logic::Graph& g, game::Level&) {
      if (op == "add") {
        logic::StateMachine m;
        m.id = g.next_id++;
        std::string base = str("name").empty() ? "State Machine" : str("name"), n = base;
        for (int i = 2; g.machine(n); ++i) n = base + " " + std::to_string(i);
        m.name = n;
        m.states.push_back({g.next_id++, "Idle", 0, 0, true});
        g.machines.push_back(std::move(m));
        result.set("id", g.machines.back().id);
        return true;
      }
      logic::StateMachine* m = g.machine(static_cast<u32>(num("machine")));
      if (!m) { error = "no such state machine"; return false; }
      if (op == "remove") {
        const u32 id = m->id;
        g.machines.erase(std::remove_if(g.machines.begin(), g.machines.end(), [&](const logic::StateMachine& x) { return x.id == id; }), g.machines.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("name");
        if (to.empty() || g.machine(to)) { error = "that name is empty or taken"; return false; }
        for (logic::Node& n : g.nodes)
          if (n.param("machine") == m->name) n.set_param("machine", to);
        m->name = to;
        return true;
      }
      if (op == "addState") {
        std::string base = str("name").empty() ? "State" : str("name"), n = base;
        for (int i = 2; m->find(n); ++i) n = base + " " + std::to_string(i);
        logic::State s{g.next_id++, n, static_cast<f32>(num("x")), static_cast<f32>(num("y")), m->states.empty()};
        m->states.push_back(s);
        result.set("id", s.id);
        if (c["from"].is_number()) {  // dragged out of a state: connect with a transition
          logic::Transition t;
          t.id = g.next_id++;
          t.from = static_cast<u32>(num("from"));
          t.to = s.id;
          t.trigger = logic::TriggerKind::timer;
          t.seconds = 1;
          m->transitions.push_back(t);
        }
        return true;
      }
      const u32 sid = static_cast<u32>(num("state"));
      if (op == "renameState") {
        const std::string to = str("name");
        logic::State* s = nullptr;
        for (logic::State& x : m->states) if (x.id == sid) s = &x;
        if (!s || to.empty() || m->find(to)) { error = "that name is empty or taken"; return false; }
        for (logic::Node& n : g.nodes)
          if (n.param("machine") == m->name && n.param("state") == s->name) n.set_param("state", to);
        s->name = to;
        return true;
      }
      if (op == "initial") {
        for (logic::State& x : m->states) x.initial = x.id == sid;
        return true;
      }
      if (op == "removeState") {
        m->states.erase(std::remove_if(m->states.begin(), m->states.end(), [&](const logic::State& x) { return x.id == sid; }), m->states.end());
        m->transitions.erase(std::remove_if(m->transitions.begin(), m->transitions.end(), [&](const logic::Transition& t) { return t.from == sid || t.to == sid; }),
                             m->transitions.end());
        if (!m->states.empty() && std::none_of(m->states.begin(), m->states.end(), [](const logic::State& x) { return x.initial; })) m->states.front().initial = true;
        return true;
      }
      if (op == "addTransition" || op == "editTransition") {
        logic::Transition* t = nullptr;
        if (op == "addTransition") {
          const u32 from = static_cast<u32>(num("from")), to = static_cast<u32>(num("to"));
          if (!m->find(to) || (from && !m->find(from)) || from == to) { error = "pick two different states"; return false; }
          logic::Transition nt;
          nt.id = g.next_id++;
          nt.from = from;
          nt.to = to;
          nt.trigger = logic::TriggerKind::event;
          nt.event = "Go";
          m->transitions.push_back(nt);
          t = &m->transitions.back();
          result.set("id", t->id);
        } else {
          for (logic::Transition& x : m->transitions) if (x.id == static_cast<u32>(num("transition"))) t = &x;
          if (!t) { error = "no such transition"; return false; }
        }
        const std::string trig = str("trigger");
        if (!trig.empty())
          t->trigger = trig == "timer" ? logic::TriggerKind::timer : trig == "condition" ? logic::TriggerKind::condition : trig == "auto" ? logic::TriggerKind::automatic : logic::TriggerKind::event;
        if (c["event"].is_string()) t->event = str("event");
        if (c["seconds"].is_number()) t->seconds = static_cast<f32>(clamp(num("seconds"), 0.0, 1e6));
        if (c["variable"].is_string()) t->variable = str("variable");
        if (c["label"].is_string()) t->label = str("label");
        if (t->trigger == logic::TriggerKind::condition && t->variable.empty()) {
          for (const logic::Variable& v : g.variables) if (v.type == logic::PinType::boolean) { t->variable = v.name; break; }
          if (t->variable.empty()) {
            std::string n = "Ready";
            for (int i = 2; g.variable(n); ++i) n = "Ready" + std::to_string(i);
            g.variables.push_back({n, logic::PinType::boolean, json::Value(false), {}});
            t->variable = n;
          }
        }
        return true;
      }
      if (op == "removeTransition") {
        const u32 tid = static_cast<u32>(num("transition"));
        m->transitions.erase(std::remove_if(m->transitions.begin(), m->transitions.end(), [&](const logic::Transition& t) { return t.id == tid; }), m->transitions.end());
        return true;
      }
      error = "unknown state machine operation";
      return false;
    });
  }
  if (name == "logicCommentBox") {
    const std::string op = str("op");
    return graph_edit(op == "add" ? "Add Comment" : op == "remove" ? "Delete Comment" : "Edit Comment", [&](logic::Graph& g, game::Level&) {
      if (op == "add") {
        logic::CommentBox cb;
        cb.id = g.next_id++;
        cb.x = static_cast<f32>(std::round(num("x") / 8) * 8);
        cb.y = static_cast<f32>(std::round(num("y") / 8) * 8);
        cb.w = static_cast<f32>(clamp(num("w", 400), 64.0, 1e5));
        cb.h = static_cast<f32>(clamp(num("h", 200), 48.0, 1e5));
        if (c["text"].is_string()) cb.text = str("text");
        cb.color = static_cast<u32>(clamp(num("color"), 0.0, 16777215.0));
        g.comments.push_back(cb);
        result.set("id", cb.id);
        return true;
      }
      const u32 id = static_cast<u32>(num("id"));
      auto it = std::find_if(g.comments.begin(), g.comments.end(), [&](const logic::CommentBox& x) { return x.id == id; });
      if (it == g.comments.end()) { error = "no such comment"; return false; }
      if (op == "remove") { g.comments.erase(it); return true; }
      if (c["text"].is_string()) it->text = str("text").substr(0, 2000);
      if (c["color"].is_number()) it->color = static_cast<u32>(clamp(num("color"), 0.0, 16777215.0));
      return true;
    });
  }
  if (name == "logicPromote") {
    // Promote to Variable (Blueprint): a new variable of the pin's type, wired to the pin through a
    // Get node (an input) or a Set node (an output).
    return graph_edit("Promote to Variable", [&](logic::Graph& g, game::Level&) {
      const u32 node = static_cast<u32>(num("node"));
      const bool output = c["output"].as_bool();
      logic::PinDef p;
      const logic::Node* n = g.find(node);
      if (!n || !pin_of(g, node, str("pin"), output, p) || p.type == logic::PinType::exec) { error = "only data pins can become variables"; return false; }
      std::string base;
      for (char ch : p.label.empty() ? p.id : p.label) if (std::isalnum(static_cast<unsigned char>(ch))) base += ch;
      // (a name C++ can write: "Text" or "Actor" alone would be the type's)
      if (base.empty() || std::isdigit(static_cast<unsigned char>(base[0]))) base = "NewVar" + base;
      if (!name_problem(g, base).empty()) base += "Value";
      std::string vname = base;
      for (int i = 2; g.variable(vname) || g.function(vname) || g.enumeration(vname) || g.structure(vname); ++i) vname = base + std::to_string(i);
      const logic::TypeRef held = logic::place_type(p);
      logic::Variable v{vname, p.type, logic::zero_of(held, g).to_json(g, held), {}, held.of};
      if (!output)
        if (const json::Value* lit = n->value(p.id)) v.value = logic::Value::from_json(*lit, held, g).to_json(g, held);
      g.variables.push_back(v);
      logic::Node vn;
      vn.id = g.next_id++;
      vn.type = output ? "var.set" : "var.get";
      vn.set_param("variable", vname);
      vn.x = n->x + (output ? 260.0f : -220.0f);
      vn.y = n->y + 40;
      g.nodes.push_back(vn);
      std::string ignore;
      if (output) connect(g, node, p.id, vn.id, "value", ignore);
      else connect(g, vn.id, "value", node, p.id, ignore);
      result.set("id", vn.id);
      result.set("variable", vname);
      return true;
    });
  }
  if (name == "logicInsertReroute") {
    // Double-click on a wire (Blueprint): a reroute knot splits it.
    return graph_edit("Add Reroute Node", [&](logic::Graph& g, game::Level&) {
      const json::Value& k = c["link"];
      if (!k.is_array() || k.size() != 4) { error = "no wire"; return false; }
      const logic::Link link{static_cast<u32>(k[0].as_number()), k[1].as_string(), static_cast<u32>(k[2].as_number()), k[3].as_string()};
      auto it = std::find(g.links.begin(), g.links.end(), link);
      if (it == g.links.end()) { error = "no such wire"; return false; }
      logic::PinDef p;
      if (!pin_of(g, link.from, link.out, true, p)) { error = "no such pin"; return false; }
      g.links.erase(it);
      logic::Node r;
      r.id = g.next_id++;
      r.x = static_cast<f32>(std::round(num("x") / 8) * 8);
      r.y = static_cast<f32>(std::round(num("y") / 8) * 8);
      const bool exec = p.type == logic::PinType::exec;
      r.type = exec ? "flow.reroute" : "value.reroute";
      if (!exec) r.set_param("type", logic::type_text({p.type, p.of}));
      g.nodes.push_back(r);
      g.links.push_back({link.from, link.out, r.id, exec ? "exec" : "in"});
      g.links.push_back({r.id, exec ? "then" : "out", link.to, link.in});
      result.set("id", r.id);
      return true;
    });
  }
  if (name == "logicRemoveGraph") {
    if (logic_program_) return fail_result("a program Blueprint is removed from the project (projectBlueprint remove)");
    if (!owner) return graph_edit("Clear Level Logic", [&](logic::Graph& g, game::Level&) { g = logic::Graph{}; return true; });
    const bool done = edit("Remove Logic", [&](game::Level& l) {
      game::Actor* a = l.find(owner);
      if (!a || !a->logic) return false;
      a->logic.reset();
      return true;
    });
    logic_owner_ = 0;
    return ok_result(done);
  }
  if (name == "logicTemplate") {
    // Ready-made graphs (the quickest way to see how it works).
    const std::string which = str("template");
    std::string text;
    if (which == "door")
      text = R"({"nodes":[{"id":1,"type":"game.interact","x":0,"y":0},{"id":8,"type":"flow.sequence","x":288,"y":0,"params":{"count":"2"}},
        {"id":2,"type":"flow.flip_flop","x":544,"y":0},
        {"id":3,"type":"flow.tween","x":800,"y":-128,"params":{"ease":"smooth"},"values":{"duration":0.8}},
        {"id":4,"type":"flow.tween","x":800,"y":112,"params":{"ease":"smooth"},"values":{"duration":0.8}},
        {"id":5,"type":"actor.move_by","x":1120,"y":-128,"values":{"offset":[0,0,0.05]}},{"id":6,"type":"actor.move_by","x":1120,"y":112,"values":{"offset":[0,0,-0.05]}},
        {"id":7,"type":"game.sound","x":544,"y":352,"params":{"sound":"builtin:confirm"}}],
        "links":[[1,"then",8,"exec"],[8,"then0",2,"exec"],[8,"then1",7,"exec"],[2,"a",3,"play"],[2,"b",4,"play"],[3,"update",5,"exec"],[4,"update",6,"exec"]]})";
    else if (which == "score_zone")
      text = R"({"nodes":[{"id":1,"type":"game.overlap_begin","x":0,"y":0},{"id":2,"type":"game.add_score","x":320,"y":0,"values":{"points":10}},
        {"id":3,"type":"game.message","x":640,"y":0,"values":{"text":"+10"}},{"id":4,"type":"game.sound","x":960,"y":0,"params":{"sound":"builtin:pickup"}}],
        "links":[[1,"then",2,"exec"],[2,"then",3,"exec"],[3,"then",4,"exec"]]})";
    else if (which == "spawner")
      text = R"({"variables":[{"name":"Spawned","type":"i64","value":0}],"nodes":[{"id":1,"type":"event.every","x":0,"y":0,"params":{"seconds":"3"}},
        {"id":2,"type":"flow.branch","x":320,"y":0},{"id":3,"type":"math.compare","x":96,"y":176,"params":{"op":"<","type":"i64"},"values":{"b":10}},
        {"id":4,"type":"var.get","x":-128,"y":192,"params":{"variable":"Spawned"}},{"id":5,"type":"actor.spawn","x":576,"y":0,"params":{"template":"Physics Cube"},"values":{"location":[0,0,6]}},
        {"id":6,"type":"var.add","x":960,"y":0,"params":{"variable":"Spawned"}}],
        "links":[[1,"then",2,"exec"],[4,"value",3,"a"],[3,"result",2,"condition"],[2,"true",5,"exec"],[5,"then",6,"exec"]]})";
    else if (which == "traffic_light")
      text = R"({"machines":[{"id":20,"name":"Light","states":[{"id":21,"name":"Green","x":0,"y":0,"initial":true},{"id":22,"name":"Yellow","x":280,"y":0},{"id":23,"name":"Red","x":140,"y":200}],
        "transitions":[{"id":30,"from":21,"to":22,"trigger":"timer","seconds":4},{"id":31,"from":22,"to":23,"trigger":"timer","seconds":1.5},{"id":32,"from":23,"to":21,"trigger":"timer","seconds":4}]}],
        "nodes":[{"id":1,"type":"event.state_enter","x":0,"y":0,"params":{"machine":"Light","state":"any"}},{"id":2,"type":"state.get","x":32,"y":192,"params":{"machine":"Light"}},
        {"id":3,"type":"game.message","x":352,"y":0}],"links":[[1,"then",3,"exec"],[2,"state",3,"text"]]})";
    else if (which == "robot_avoid")
      text = R"({"nodes":[{"id":1,"type":"event.begin","x":0,"y":0},{"id":2,"type":"walker.autonomous","x":320,"y":0,"values":{"enable":false}},
        {"id":3,"type":"event.every","x":0,"y":224,"params":{"seconds":"0.25"}},{"id":4,"type":"flow.branch","x":320,"y":224},
        {"id":5,"type":"walker.tof","x":-352,"y":416},{"id":6,"type":"math.compare","x":-32,"y":448,"params":{"op":"<","type":"f64"},"values":{"b":1.6}},
        {"id":7,"type":"logic.and","x":96,"y":384},
        {"id":8,"type":"walker.move","x":608,"y":160,"values":{"forward":0,"turn":45,"duration":0.6}},
        {"id":9,"type":"walker.move","x":608,"y":400,"values":{"forward":0.3}}],
        "comments":[{"id":20,"x":-384,"y":176,"w":1280,"h":480,"text":"Walk, and turn while the ToF sees something closer than 1.6 m"}],
        "links":[[1,"then",2,"exec"],[3,"then",4,"exec"],[5,"blocked",7,"a"],[5,"distance",6,"a"],[6,"result",7,"b"],[7,"result",4,"condition"],
        [4,"true",8,"exec"],[4,"false",9,"exec"]]})";
    else return fail_result("unknown template");
    json::Value j;
    logic::Graph part;
    if (!json::parse(text, j) || !logic::graph_from_json(j, part, &error)) return fail_result(error);
    logic::cpp_names(part);
    return graph_edit("Add Logic Template", [&](logic::Graph& g, game::Level&) {
      // Below what is there.
      f32 y0 = 0;
      for (const logic::Node& n : g.nodes) y0 = max_of(y0, n.y + 260);
      std::unordered_map<u32, u32> remap;
      for (const logic::Variable& v : part.variables) if (!g.variable(v.name)) g.variables.push_back(v);
      for (logic::StateMachine m : part.machines) {
        if (g.machine(m.name)) continue;
        std::unordered_map<u32, u32> sids;
        m.id = g.next_id++;
        for (logic::State& s : m.states) { sids[s.id] = g.next_id; s.id = g.next_id++; }
        for (logic::Transition& t : m.transitions) { t.id = g.next_id++; t.from = t.from ? sids[t.from] : 0; t.to = sids[t.to]; }
        g.machines.push_back(std::move(m));
      }
      for (logic::Node n : part.nodes) {
        remap[n.id] = g.next_id;
        n.id = g.next_id++;
        n.y += y0;
        g.nodes.push_back(std::move(n));
      }
      for (const logic::Link& l : part.links) g.links.push_back({remap[l.from], l.out, remap[l.to], l.in});
      for (logic::CommentBox cb : part.comments) {
        cb.id = g.next_id++;
        cb.y += y0;
        g.comments.push_back(std::move(cb));
      }
      return true;
    });
  }
  return fail_result("unknown logic command " + name);
}

}  // namespace kern::studio
