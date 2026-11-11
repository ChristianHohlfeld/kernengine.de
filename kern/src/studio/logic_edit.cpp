// Kern Studio: editing logic graphs (the level's and the actors'). Every edit is
// a level edit (one undo step); the page draws what logic_json() describes.
#include <algorithm>
#include <cctype>
#include <cmath>

#include "internal.h"
#include "kern/audio/synth.h"
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
    case K::tag: return "tag";
    case K::integer: return "integer";
    case K::function: return "function";
    case K::type: return "type";
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
std::string type_setting_for(const logic::Graph& g, const logic::Node& node, const std::string& pin, bool output, logic::PinType want) {
  const logic::NodeDef* def = logic::library().find(node.type);
  if (!def) return {};
  for (const logic::ParamDef& pd : def->params) {
    if (pd.kind != logic::ParamDef::Kind::type) continue;
    for (logic::PinType t : logic::type_choices(pd)) {
      logic::Node probe = node;
      probe.set_param(pd.key, logic::pin_type_name(t));
      std::vector<logic::PinDef> in, outs;
      logic::library().pins(probe, g, in, outs);
      for (const logic::PinDef& p : output ? outs : in)
        if (p.id == pin && p.type == want) return logic::pin_type_name(t);
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
  for (const logic::ParamDef& pd : def->params) if (pd.kind == logic::ParamDef::Kind::type) type = &pd;
  if (!type) return r;
  std::vector<logic::PinDef> in, out;
  logic::library().pins(node, g, in, out);
  std::vector<std::string> types_in(in.size()), types_out(out.size());
  std::vector<bool> typed_in(in.size(), false), typed_out(out.size(), false);
  for (logic::PinType choice : logic::type_choices(*type)) {
    logic::Node probe = node;
    probe.set_param(type->key, logic::pin_type_name(choice));
    std::vector<logic::PinDef> pin, pout;
    logic::library().pins(probe, g, pin, pout);
    if (pin.size() != in.size() || pout.size() != out.size()) return r;
    for (std::size_t i = 0; i < in.size(); ++i) {
      if (pin[i].type != in[i].type) typed_in[i] = true;
      types_in[i] += (types_in[i].empty() ? "" : " ") + std::string(logic::pin_type_name(pin[i].type));
    }
    for (std::size_t i = 0; i < out.size(); ++i) {
      if (pout[i].type != out[i].type) typed_out[i] = true;
      types_out[i] += (types_out[i].empty() ? "" : " ") + std::string(logic::pin_type_name(pout[i].type));
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
  for (std::size_t k = 0; k < in_a.size(); ++k) if (in_a[k].type != in_b[k].type) typed_in.push_back(in_a[k].id);
  for (std::size_t k = 0; k < out_a.size(); ++k) if (out_a[k].type != out_b[k].type) typed_out.push_back(out_a[k].id);
  for (const logic::Link& l : g.links) {
    if (l.to == id && l.in != replaced && std::find(typed_in.begin(), typed_in.end(), l.in) != typed_in.end()) return false;
    if (l.from == id && std::find(typed_out.begin(), typed_out.end(), l.out) != typed_out.end()) return false;
  }
  node->values.erase(std::remove_if(node->values.begin(), node->values.end(), [&](const auto& v) { return std::find(typed_in.begin(), typed_in.end(), v.first) != typed_in.end(); }),
                     node->values.end());
  node->set_param("type", type);
  return true;
}

/// Adds a link with the editor's rules: an exec output and a data input take one wire
/// (the new one replaces the old); exec inputs and data outputs fan in / out freely. A generic
/// node nothing is wired to yet takes the type of the wire (a Text List into Length makes it a
/// Length of a Text List).
bool connect(logic::Graph& g, u32 from, const std::string& out, u32 to, const std::string& in, std::string& error) {
  if (from == to) { error = "a node cannot connect to itself"; return false; }
  logic::PinDef po, pi;
  if (!pin_of(g, from, out, true, po) || !pin_of(g, to, in, false, pi)) { error = "no such pin"; return false; }
  if ((po.type == logic::PinType::exec) != (pi.type == logic::PinType::exec)) { error = "exec pins connect to exec pins only"; return false; }
  if (po.type != pi.type && po.type != logic::PinType::exec) {
    // (first the node the wire goes into, then the one it comes from)
    if (const logic::Node* n = g.find(to); n && retype(g, to, type_setting_for(g, *n, in, false, po.type), in)) pin_of(g, to, in, false, pi);
    else if (const logic::Node* m = g.find(from); m && !logic::convertible(po.type, pi.type) && retype(g, from, type_setting_for(g, *m, out, true, pi.type))) pin_of(g, from, out, true, po);
  }
  if (!logic::convertible(po.type, pi.type)) {
    error = std::string("a ") + logic::pin_type_name(po.type) + " cannot go into a " + logic::pin_type_name(pi.type) + " pin";
    return false;
  }
  auto& links = g.links;
  if (po.type == logic::PinType::exec) links.erase(std::remove_if(links.begin(), links.end(), [&](const logic::Link& l) { return l.from == from && l.out == out; }), links.end());
  else links.erase(std::remove_if(links.begin(), links.end(), [&](const logic::Link& l) { return l.to == to && l.in == in; }), links.end());
  links.push_back({from, out, to, in});
  return true;
}

/// The first pin of `node` that `type` can connect to (output side when `want_output`).
std::string compatible_pin(const logic::Graph& g, const logic::Node& node, logic::PinType type, bool want_output) {
  std::vector<logic::PinDef> in, out;
  logic::library().pins(node, g, in, out);
  for (const logic::PinDef& p : want_output ? out : in) {
    if ((p.type == logic::PinType::exec) != (type == logic::PinType::exec)) continue;
    if (want_output ? logic::convertible(p.type, type) : logic::convertible(type, p.type)) return p.id;
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

json::Value Studio::logic_json() const {
  json::Value o = json::Value::object();
  o.set("open", logic_open_);
  o.set("owner", logic_owner_);
  const game::Actor* owner = logic_owner_ ? level_.find(logic_owner_) : nullptr;
  o.set("ownerName", logic_owner_ ? (owner ? owner->name : std::string("?")) : std::string("Level"));
  o.set("view", logic_view_);
  o.set("paused", paused_);
  o.set("playing", play_ != PlayState::editing);
  o.set("breakpoints", static_cast<u32>(logic_breakpoints_.size()));
  // Owners to switch between: the level and every actor with a graph.
  json::Value owners = json::Value::array();
  {
    json::Value lv = json::Value::object();
    lv.set("id", 0);
    lv.set("name", "Level");
    lv.set("nodes", static_cast<u32>(level_.logic.nodes.size()));
    owners.push(std::move(lv));
  }
  for (const game::Actor& a : level_.actors)
    if (a.logic) {
      json::Value x = json::Value::object();
      x.set("id", a.id);
      x.set("name", a.name);
      x.set("nodes", static_cast<u32>(a.logic->nodes.size()));
      owners.push(std::move(x));
    }
  o.set("owners", std::move(owners));
  const logic::Graph* g = graph_of(level_, logic_owner_);
  o.set("exists", g != nullptr);
  json::Value events = json::Value::array();
  for (const std::string& e : event_names(level_)) events.push(e);
  o.set("events", std::move(events));
  if (!g) return o;
  const logic::Library& lib = logic::library();
  std::unordered_map<u32, std::vector<const logic::Issue*>> by_node;
  const std::vector<logic::Issue> issues = logic::check(*g, lib);
  for (const logic::Issue& i : issues) by_node[i.node].push_back(&i);
  json::Value nodes = json::Value::array();
  for (const logic::Node& n : g->nodes) {
    const logic::NodeDef* d = lib.find(n.type);
    json::Value x = json::Value::object();
    x.set("id", n.id);
    x.set("type", n.type);
    x.set("title", lib.title(n));
    x.set("x", static_cast<f64>(n.x));
    x.set("y", static_cast<f64>(n.y));
    if (!n.comment.empty()) x.set("comment", n.comment);
    if (const std::string sym = compact_symbol(n); !sym.empty()) x.set("compact", sym);
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
        // (a pin that takes the type of the first wire: the types it can be)
        for (const auto& [id, types] : generic) if (id == p.id) pj.set("generic", types);
        bool linked = false;
        for (const logic::Link& l : g->links)
          if ((output && l.from == n.id && l.out == p.id) || (!output && l.to == n.id && l.in == p.id)) linked = true;
        pj.set("linked", linked);
        if (!output && p.type != logic::PinType::exec) {
          const json::Value* v = n.value(p.id);
          pj.set("value", logic::Value::from_json(v ? *v : p.def, p.type).to_json());
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
        if (pd.kind == K::choice)
          for (std::size_t i = 0; i + 1 < pd.choices.size(); i += 2) choice(pd.choices[i], pd.choices[i + 1]);
        if (pd.kind == K::type)
          for (logic::PinType t : logic::type_choices(pd)) choice(logic::pin_type_name(t), logic::type_label(t));
        if (pd.kind == K::variable)
          for (const logic::Variable& v : g->variables) choice(v.name, v.name + " (" + logic::type_label(v.type) + ")");
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
          for (const std::string& e : event_names(level_)) choice(e, e);
        if (pd.kind == K::sound) {
          for (const audio::BuiltinSound& b : audio::builtin_sounds()) choice(std::string("builtin:") + b.name, b.label);
          for (const game::SoundAsset& sa : level_.sounds) choice(sa.id, sa.name);
        }
        pj.set("choices", std::move(choices));
        params.push(std::move(pj));
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
  json::Value links = json::Value::array();
  for (const logic::Link& l : g->links) {
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
    x.set("value", logic::Value::from_json(v.value, v.type).to_json());
    vars.push(std::move(x));
  }
  o.set("variables", std::move(vars));
  json::Value graph = logic::graph_to_json(*g);
  o.set("functions", graph["functions"].is_array() ? graph["functions"] : json::Value::array());
  o.set("machines", graph["machines"]);
  o.set("comments", graph["comments"].is_array() ? graph["comments"] : json::Value::array());
  json::Value iss = json::Value::array();
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
  if (!rt) return o;
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
    for (const logic::Variable& v : inst->graph().variables) vars.set(v.name, inst->get(v.name).to_json());
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
      logic_open_ = true;
      logic_view_ = "graph";
      const logic::Graph* g = owner ? (a && a->logic ? &*a->logic : nullptr) : &level_.logic;
      const logic::Node* n = g ? g->find(node) : nullptr;
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
  const logic::Graph* g = graph_of(level_, logic_owner_);
  logic::Graph empty;
  if (!g) g = &empty;
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
  logic::PinType from_type = logic::PinType::exec;
  if (c["from"].is_object()) {
    logic::PinDef p;
    from_output = c["from"]["output"].as_bool();
    if (pin_of(*g, static_cast<u32>(c["from"]["node"].as_number()), c["from"]["pin"].as_string(), from_output, p)) {
      from_pin = true;
      from_type = p.type;
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
  for (const auto& d : lib.all()) {
    if (variable_step(*d)) continue;                                                     // per variable below
    if (d->type == "flow.reroute" || d->type == "value.reroute") continue;               // from a wire, below
    if (d->type == "func.return" || d->type == "func.call" || d->type == "func.pure") continue;  // per function below
    Entry e{d->label, d->category, d->help, d->type, {}};
    if (from_pin && from_type != logic::PinType::exec) {
      // a generic node comes in the type that fits the wire (Length of a Text List, Select of a Vector)
      logic::Node probe;
      probe.type = d->type;
      std::vector<logic::PinDef> in, out;
      lib.pins(probe, *g, in, out);
      std::string fits;
      for (const logic::PinDef& p : from_output ? in : out)
        if (fits.empty() && p.type != logic::PinType::exec) fits = type_setting_for(*g, probe, p.id, !from_output, from_type);
      if (!fits.empty()) e.params.push_back({"type", fits});
    }
    if (d->type == "func.entry") { e.label = "New Function"; e.help = "Adds a function to this Blueprint: its entry and a Return node. Inputs and outputs are set in Details."; }
    if (d->type == "event.state_enter" || d->type == "event.state_exit" || d->type == "state.set" || d->type == "state.is" || d->type == "state.get") {
      if (g->machines.empty()) continue;
      e.params.push_back({"machine", g->machines.front().name});
    }
    consider(std::move(e));
  }
  if (from_pin) {
    if (from_type == logic::PinType::exec) consider({"Add Reroute Node", "Flow", "A knot the wire passes through", "flow.reroute", {}});
    else consider({"Add Reroute Node", "Flow", "A knot the wire passes through", "value.reroute", {{"type", logic::pin_type_name(from_type)}}});
  }
  for (const logic::Variable& v : g->variables) {
    consider({"Get " + v.name, "Variables", std::string("Reads ") + v.name, "var.get", {{"variable", v.name}}});
    consider({"Set " + v.name, "Variables", std::string("Writes ") + v.name, "var.set", {{"variable", v.name}}});
    if (v.type == logic::PinType::number || v.type == logic::PinType::integer)
      consider({"Add to " + v.name, "Variables", "Adds to " + v.name, "var.add", {{"variable", v.name}}});
    if (!logic::is_list(v.type)) continue;
    for (const auto& d : lib.all()) {
      if (!variable_step(*d) || d->type.rfind("list.", 0) != 0) continue;
      logic::Node named;
      named.type = d->type;
      named.set_param("variable", v.name);
      consider({lib.title(named), "Lists", d->help, d->type, {{"variable", v.name}}});
    }
  }
  for (const logic::Function& f : g->functions) {
    consider({f.name, "Functions", f.help.empty() ? "Calls the function " + f.name : f.help, f.pure ? "func.pure" : "func.call", {{"function", f.name}}});
    consider({"Return (" + f.name + ")", "Functions", "Ends " + f.name + " and gives its outputs to the caller", "func.return", {{"function", f.name}}});
  }
  for (const std::string& ev : event_names(level_)) {
    consider({"Call " + ev, "Events", "Raises the custom event " + ev, "flow.call", {{"name", ev}}});
    consider({"Event " + ev, "Events", "Runs when " + ev + " is called", "event.custom", {{"name", ev}}});
  }
  for (const logic::StateMachine& m : g->machines)
    for (const logic::State& s : m.states) {
      consider({"Go to " + s.name, "State Machines", m.name + ": switch to " + s.name, "state.set", {{"machine", m.name}, {"state", s.name}}});
      consider({"On " + s.name + " Entered", "State Machines", m.name + " entered " + s.name, "event.state_enter", {{"machine", m.name}, {"state", s.name}}});
    }
  // Categories in the order people reach for them.
  static const char* order[] = {"Events", "Flow", "Variables", "Lists", "Functions", "Game", "Actors", "Robots", "Physics", "Combat", "Effects", "Audio", "State Machines",
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

// ---------------------------------------------------------------------------
// commands

json::Value Studio::logic_command(const std::string& name, const json::Value& c) {
  auto num = [&](const char* k, f64 fallback = 0) { return c[k].is_number() ? c[k].as_number() : fallback; };
  auto str = [&](const char* k) -> std::string { return c[k].is_string() ? c[k].as_string() : std::string(); };
  if (name == "logicOpen") {
    const u32 owner = static_cast<u32>(num("owner", c["owner"].is_null() && !selection_.empty() ? primary() : 0));
    if (owner && !level_.find(owner)) return fail_result("no such actor");
    logic_owner_ = owner;
    logic_open_ = true;
    logic_view_ = "graph";
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
    const logic::Graph* g = graph_of(level_, logic_owner_);
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
    json::Value r = ok_result();
    r.set("text", json::stringify(logic::graph_to_json(part)));
    return r;
  }
  if (name == "logicCpp") {
    // The Blueprint in its second notation (docs/blueprint_cpp.md): the code alone - where the
    // nodes are stays with the graph.
    const logic::Graph* g = graph_of(level_, logic_owner_);
    const game::Actor* a = logic_owner_ ? level_.find(logic_owner_) : nullptr;
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
    const logic::Graph* g = graph_of(level_, logic_owner_);
    if (!g) return fail_result("this actor has no logic yet");
    std::string text, glue, why;
    if (!logic::native_ready(*g, &why)) return fail_result(why);  // (it does not compile: Compile says where)
    const game::Actor* owner = logic_owner_ ? level_.find(logic_owner_) : nullptr;
    const std::string cls = logic::cpp_class_name(owner ? owner->name : std::string("Level"), *g);
    if (!logic::to_cpp(*g, cls, text, &why) || !logic::native_glue(*g, cls, glue, &why)) return fail_result(why);
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
    const logic::Graph* g = graph_of(level_, logic_owner_);
    json::Value iss = json::Value::array();
    if (g)
      for (const logic::Issue& i : logic::check(*g)) {
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
  auto graph_edit = [&](const char* label, auto&& fn) {
    const bool done = edit(label, [&](game::Level& l) {
      logic::Graph* g = graph_of(l, owner, true);
      if (!g) { error = "the graph's actor is gone"; return false; }
      return fn(*g, l);
    });
    if (!done && !error.empty()) return fail_result(error);
    result.set("ok", done || error.empty());
    return result;
  };
  if (name == "logicCppApply") {
    // The Blueprint becomes what the C++ says: one undo step. The nodes the text still has keep
    // their ids and places; what cannot be read is refused with its line and the reason.
    const logic::Graph* g = graph_of(level_, owner);
    logic::Graph now;
    if (!logic::apply_cpp(str("text"), g ? *g : logic::Graph(), now, &error)) return fail_result(error);
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
      if ((type == "func.return" || type == "func.call" || type == "func.pure") && !g.function(n.param("function"))) {
        if (g.functions.empty()) { error = "this Blueprint has no function yet: add one in My Blueprint"; return false; }
        n.set_param("function", g.functions.front().name);
      }
      // New variables nodes on a graph without variables: offer one (Get / Set need a name).
      if ((type == "var.get" || type == "var.set" || type == "var.add") && n.param("variable").empty()) {
        if (g.variables.empty()) g.variables.push_back({"Count", logic::PinType::number, json::Value(0.0), {}});
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
          const std::string other = compatible_pin(g, g.nodes.back(), p.type, !output);
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
      return connect(g, static_cast<u32>(num("from")), str("out"), static_cast<u32>(num("to")), str("in"), error);
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
      n->set_value(p.id, logic::Value::from_json(c["value"], p.type).to_json());
      return true;
    });
  }
  if (name == "logicParam") {
    return graph_edit("Edit Node", [&](logic::Graph& g, game::Level&) {
      logic::Node* n = g.find(static_cast<u32>(num("node")));
      if (!n) { error = "no such node"; return false; }
      const json::Value& v = c["value"];
      n->set_param(str("key"), v.is_string() ? v.as_string() : v.is_number() ? std::to_string(static_cast<i64>(v.as_number())) : json::stringify(v));
      // A changed variable / count can remove pins: drop wires to pins that no longer exist.
      std::vector<logic::PinDef> in, out;
      logic::library().pins(*n, g, in, out);
      auto has = [](const std::vector<logic::PinDef>& v, const std::string& id) { for (const auto& p : v) if (p.id == id) return true; return false; };
      g.links.erase(std::remove_if(g.links.begin(), g.links.end(), [&](const logic::Link& l) {
                      return (l.from == n->id && !has(out, l.out)) || (l.to == n->id && !has(in, l.in));
                    }),
                    g.links.end());
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
  if (name == "logicFunction") {
    // A function of the graph: its name, whether it is offered as a pure node, its help and its
    // pins. The function's nodes (entry, Return, calls) follow every change.
    const std::string op = str("op");
    return graph_edit("Edit Function", [&](logic::Graph& g, game::Level&) {
      const std::string fname = str("name");
      logic::Function* f = g.function(fname);
      if (!f) { error = "no function " + fname; return false; }
      auto uses = [&](const logic::Node& n) {
        return (n.type == "func.entry" || n.type == "func.return" || n.type == "func.call" || n.type == "func.pure") && n.param("function") == fname;
      };
      // a pin's side on a node: the function's inputs leave the entry and enter the calls; the
      // outputs enter the Returns and leave the calls
      auto is_input_pin_of = [&](const logic::Node& n, bool input_side) { return input_side ? n.type != "func.entry" : n.type == "func.return"; };
      auto drop_links = [&](auto&& gone) {
        g.links.erase(std::remove_if(g.links.begin(), g.links.end(), gone), g.links.end());
      };
      if (op == "remove") {
        std::vector<u32> ids;
        for (const logic::Node& n : g.nodes) if (uses(n)) ids.push_back(n.id);
        auto has = [&](u32 id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };
        drop_links([&](const logic::Link& l) { return has(l.from) || has(l.to); });
        g.nodes.erase(std::remove_if(g.nodes.begin(), g.nodes.end(), [&](const logic::Node& n) { return has(n.id); }), g.nodes.end());
        g.functions.erase(std::remove_if(g.functions.begin(), g.functions.end(), [&](const logic::Function& x) { return x.name == fname; }), g.functions.end());
        return true;
      }
      if (op == "rename") {
        const std::string to = str("to");
        if (to.empty() || to.size() > 64 || g.function(to) || g.variable(to)) { error = "that name is empty or taken"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        for (logic::Node& n : g.nodes) if (uses(n)) n.set_param("function", to);
        f->name = to;
        return true;
      }
      if (op == "pure") {
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
      if (op == "help") { f->help = str("text"); return true; }
      const bool input_side = str("side") != "out";
      std::vector<logic::FunctionPin>& pins = input_side ? f->inputs : f->outputs;
      auto valid_name = [&](const std::string& n) {
        if (n.empty() || n.size() > 64 || n == "exec" || n == "then") return false;
        for (const logic::FunctionPin& p : pins) if (p.name == n) return false;
        // (an input is a parameter in the function's C++: it cannot hide a variable or a function)
        if (input_side && (g.variable(n) || g.function(n))) return false;
        return name_problem(g, n).empty();
      };
      if (op == "pinAdd") {
        if (pins.size() >= 16) { error = "a function has at most 16 inputs and 16 outputs"; return false; }
        std::string base = str("pin").empty() ? (input_side ? "value" : "result") : str("pin"), pname = base;
        for (int i = 2; !valid_name(pname) && i < 100; ++i) pname = base + std::to_string(i);
        logic::PinType t = logic::PinType::number;
        logic::pin_type_from_name(str("type").empty() ? "float" : str("type"), t);
        if (t == logic::PinType::exec) t = logic::PinType::number;
        pins.push_back({pname, t, logic::Value::zero(t).to_json()});
        result.set("pin", pname);
        return true;
      }
      const std::string pin = str("pin");
      auto it = std::find_if(pins.begin(), pins.end(), [&](const logic::FunctionPin& x) { return x.name == pin; });
      if (it == pins.end()) { error = "no pin " + pin; return false; }
      // the wires and the typed-in values of this pin on the function's nodes
      auto on_pin = [&](const logic::Link& l, bool& as_input) {
        const logic::Node* to = g.find(l.to);
        const logic::Node* from = g.find(l.from);
        if (to && uses(*to) && l.in == pin && is_input_pin_of(*to, input_side)) { as_input = true; return true; }
        if (from && uses(*from) && l.out == pin && !is_input_pin_of(*from, input_side)) { as_input = false; return true; }
        return false;
      };
      if (op == "pinRemove") {
        drop_links([&](const logic::Link& l) { bool in; return on_pin(l, in); });
        for (logic::Node& n : g.nodes)
          if (uses(n) && is_input_pin_of(n, input_side))
            n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == pin; }), n.values.end());
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
        for (logic::Node& n : g.nodes)
          if (uses(n) && is_input_pin_of(n, input_side))
            for (auto& v : n.values) if (v.first == pin) v.first = to;
        it->name = to;
        return true;
      }
      if (op == "pinType") {
        logic::PinType t;
        if (!logic::pin_type_from_name(str("type"), t) || t == logic::PinType::exec) { error = "unknown type"; return false; }
        it->value = logic::Value::from_json(it->value, it->type).converted(t).to_json();
        it->type = t;
        // values typed into the nodes were of the old type
        for (logic::Node& n : g.nodes)
          if (uses(n) && is_input_pin_of(n, input_side))
            n.values.erase(std::remove_if(n.values.begin(), n.values.end(), [&](const auto& v) { return v.first == pin; }), n.values.end());
        return true;
      }
      if (op == "pinValue") { it->value = logic::Value::from_json(c["value"], it->type).to_json(); return true; }
      if (op == "pinMove") {
        const std::ptrdiff_t at = it - pins.begin(), to = at + (num("by") < 0 ? -1 : 1);
        if (to < 0 || to >= static_cast<std::ptrdiff_t>(pins.size())) return false;
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
        for (int i = 2; g.variable(n) || g.function(n); ++i) n = base + std::to_string(i);
        logic::PinType t = logic::PinType::number;
        logic::pin_type_from_name(str("type").empty() ? "float" : str("type"), t);
        if (t == logic::PinType::exec) t = logic::PinType::number;
        g.variables.push_back({n, t, logic::Value::zero(t).to_json(), {}});
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
        if (to.empty() || to.size() > 64 || g.variable(to) || g.function(to)) { error = "that name is empty or taken"; return false; }
        if (const std::string why = name_problem(g, to); !why.empty()) { error = why; return false; }
        for (logic::Node& n : g.nodes)
          if (n.param("variable") == var) n.set_param("variable", to);
        for (logic::StateMachine& m : g.machines)
          for (logic::Transition& t : m.transitions)
            if (t.variable == var) t.variable = to;
        v->name = to;
        return true;
      }
      if (op == "type") {
        logic::PinType t;
        if (!logic::pin_type_from_name(str("type"), t) || t == logic::PinType::exec) { error = "unknown type"; return false; }
        v->value = logic::Value::from_json(v->value, v->type).converted(t).to_json();
        v->type = t;
        return true;
      }
      if (op == "value") {
        v->value = logic::Value::from_json(c["value"], v->type).to_json();
        return true;
      }
      error = "unknown variable operation";
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
      for (int i = 2; g.variable(vname) || g.function(vname); ++i) vname = base + std::to_string(i);
      logic::Variable v{vname, p.type, logic::Value::zero(p.type).to_json(), {}};
      if (!output)
        if (const json::Value* lit = n->value(p.id)) v.value = logic::Value::from_json(*lit, p.type).to_json();
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
      if (!exec) r.set_param("type", logic::pin_type_name(p.type));
      g.nodes.push_back(r);
      g.links.push_back({link.from, link.out, r.id, exec ? "exec" : "in"});
      g.links.push_back({r.id, exec ? "then" : "out", link.to, link.in});
      result.set("id", r.id);
      return true;
    });
  }
  if (name == "logicRemoveGraph") {
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
      text = R"({"variables":[{"name":"Spawned","type":"int","value":0}],"nodes":[{"id":1,"type":"event.every","x":0,"y":0,"params":{"seconds":"3"}},
        {"id":2,"type":"flow.branch","x":320,"y":0},{"id":3,"type":"math.compare","x":96,"y":176,"params":{"op":"<"},"values":{"b":10}},
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
        {"id":5,"type":"walker.tof","x":-352,"y":416},{"id":6,"type":"math.compare","x":-32,"y":448,"params":{"op":"<"},"values":{"b":1.6}},
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
