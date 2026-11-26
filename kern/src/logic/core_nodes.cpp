// Kern Logic: the built-in nodes (events, flow control, variables, state
// machines, functions, math, logic, vectors, text, debugging). Gameplay nodes live in
// the game module (src/game/logic_nodes.cpp).
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

#include "kern/core/dmath.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

namespace {

PinDef exec_in(std::string id = "exec", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef exec_out(std::string id = "then", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef data(std::string id, std::string label, PinType t, json::Value def = {}) { return {std::move(id), std::move(label), t, std::move(def)}; }
/// An input that reads a list of whole numbers of any type as it is (kAnyWidth: List<i64>, List<u8> ...).
PinDef any_ints(std::string id, std::string label) {
  PinDef p = data(std::move(id), std::move(label), PinType::i64_list, json::Value::array());
  p.of = std::string(kAnyWidth);
  return p;
}
/// A list of bytes a node gives: a List<u8> (what C++ holds bytes in - std::vector<u8>, std::array<u8, N>).
PinDef bytes_out(std::string id, std::string label) { return data(std::move(id), std::move(label), PinType::u8_list); }

NodeDef make(std::string type, std::string label, std::string category, NodeKind kind, std::string help, std::string keywords = {}) {
  NodeDef d;
  d.type = std::move(type);
  d.label = std::move(label);
  d.category = std::move(category);
  d.kind = kind;
  d.help = std::move(help);
  d.keywords = std::move(keywords);
  return d;
}

/// Pure node computing one output from its inputs.
NodeDef pure(std::string type, std::string label, std::string category, std::vector<PinDef> in, PinDef out, std::function<Value(Context&)> fn, std::string help = {},
             std::string keywords = {}) {
  NodeDef d = make(std::move(type), std::move(label), std::move(category), NodeKind::pure, std::move(help));
  d.inputs = std::move(in);
  d.outputs = {std::move(out)};
  d.keywords = std::move(keywords);
  d.run = [fn = std::move(fn)](Context& c) {
    c.out(std::size_t{0}, fn(c));
    return kStop;
  };
  return d;
}

/// A function of nodes.h as a value node: its arguments are the node's inputs in their order, its
/// result the output. What the node computes is that function - a Blueprint's C++ calls the same one.
template <class R, class... A, std::size_t... I> Value call_typed(R (*f)(A...), const Value* v, std::index_sequence<I...>) {
  return to_value(f(from_value<std::decay_t<A>>(v[I])...));
}
template <class R, class... A> std::function<Value(Context&)> of(R (*f)(A...)) {
  return [f](Context& c) {
    Value v[sizeof...(A)];
    for (std::size_t i = 0; i < sizeof...(A); ++i) v[i] = c.in(i);  // (the inputs one after the other, in their order)
    return call_typed(f, v, std::index_sequence_for<A...>{});
  };
}

ParamDef param(std::string key, std::string label, ParamDef::Kind kind, std::string def = {}, std::vector<std::string> choices = {}) {
  ParamDef p;
  p.key = std::move(key);
  p.label = std::move(label);
  p.kind = kind;
  p.def = std::move(def);
  p.choices = std::move(choices);
  return p;
}
/// The setting of a node that names a variable: the function whose local variable it is (empty:
/// a variable of the Blueprint).
/// A node that reads or changes the world it runs in (NodeDef::world).
NodeDef worldly(NodeDef d) {
  d.world = true;
  return d;
}
ParamDef local_of() { return param("scope", "Local of", ParamDef::Kind::scope); }
/// The setting "path" of a node that names a place: from its variable through fields and list
/// entries (ParamDef::Kind::path).
ParamDef path_of() { return param("path", "Path", ParamDef::Kind::path); }
/// The place a node names, as its title has it: Cars[].Speed.
// (a place of the engine: the node that bound it - n12[].to)
std::string place_title(const Node& n) { return (n.param("bound").empty() ? n.param("variable", "?") : "n" + n.param("bound")) + n.param("path"); }

/// The setting "Type" of a generic node: the type of its pins (`with_lists`: a list type too).
ParamDef type_param(bool with_lists) {
  ParamDef p = param("type", "Type", ParamDef::Kind::type, "f32");
  p.with_lists = with_lists;
  return p;
}
/// The setting "Enum" of an enum node: which enum of the Blueprint.
ParamDef enum_param() {
  ParamDef p = param("enum", "Enum", ParamDef::Kind::type);
  p.enums_only = true;
  return p;
}
/// The setting "Struct" of a struct node: which struct of the Blueprint.
ParamDef struct_param() {
  ParamDef p = param("struct", "Struct", ParamDef::Kind::type);
  p.structs_only = true;
  return p;
}
/// The type a node's setting "Type" names in its Blueprint - Float when it names none (the
/// check says so). `lists`: a list type may be meant.
TypeRef setting_type(const Node& n, const Graph& g, bool lists) {
  TypeRef t;
  if (!type_from_text(n.param("type", "f32"), g, t) || (!lists && is_list(t.type)) || is_keyed(t.type)) t = {kFloat, {}};
  return t;
}
/// A data pin of a type a Blueprint names (an enum's with the enum, a struct's with the struct).
PinDef typed_pin(std::string id, std::string label, const TypeRef& t, json::Value def = {}) {
  PinDef p = data(std::move(id), std::move(label), t.type, std::move(def));
  p.of = t.of;
  p.count = t.count;
  return p;
}
/// What a pin of type `t` holds when nothing is typed in (a struct: its fields' defaults).
json::Value zero_json(const TypeRef& t, const Graph& g) { return zero_of(t, g).to_json(g, t); }
void retype(PinDef& p, const TypeRef& t, const Graph& g) {
  p.type = t.type;
  p.of = t.of;
  p.def = zero_json(t, g);
}

/// A value that is new on every read (Random): the node keeps its sequence.
NodeDef fresh(NodeDef d) {
  d.memory = true;
  return d;
}

int clamp_count(const Node& n, std::string_view key, int def, int lo, int hi) { return setting_count(n.param(key), def, lo, hi); }

void add_events(Library& lib) {
  {
    NodeDef d = make("event.begin", "Begin Play", "Events", NodeKind::event, "Runs once when play starts (or when the actor is spawned).");
    d.outputs = {exec_out()};
    d.event = "logic.begin";
    d.color = 0xb4232f;
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.end", "End Play", "Events", NodeKind::event, "Runs once when play stops (or when the actor is taken out of the world) - after it began.");
    d.outputs = {exec_out()};
    d.event = "logic.end";
    d.color = 0xb4232f;
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.tick", "Event Tick", "Events", NodeKind::event, "Runs every simulation step. Delta is the step in seconds.");
    d.outputs = {exec_out(), data("delta", "Delta Seconds", PinType::f64)};
    d.event = "logic.tick";
    d.color = 0xb4232f;
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.every", "Every N Seconds", "Events", NodeKind::event, "Runs repeatedly with a fixed interval (a looping timer).");
    d.params = {param("seconds", "Interval (s)", ParamDef::Kind::text, "1")};
    d.outputs = {exec_out(), data("count", "Count", PinType::i64)};
    d.color = 0xb4232f;
    d.title = [](const NodeDef&, const Node& n) { return "Every " + n.param("seconds", "1") + " s"; };
    // Delivered with the tick: counts its own time.
    d.event = "logic.tick";
    d.accepts = [](const Node&, const Event&) { return true; };
    d.run = [](Context& c) {
      NodeState& s = c.state();
      const f64 period = max_of(0.01, std::atof(c.param("seconds", "1").c_str()));
      if (s.time == 0) s.time = c.host().time() + period;
      if (c.host().time() + 1e-9 < s.time) return kStop;
      s.time += period;
      if (s.time < c.host().time()) s.time = c.host().time() + period;
      c.out("count", Value::integer(++s.counter));
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.custom", "Custom Event", "Events", NodeKind::event,
                     "An event you name and raise yourself with Call Event (on this graph, another actor's or all). Also switches state machine transitions with that name.");
    d.params = {param("name", "Name", ParamDef::Kind::event, "MyEvent")};
    d.outputs = {exec_out(), data("actor", "Actor", PinType::actor), data("value", "Value", PinType::f64), data("text", "Text", PinType::text)};
    d.color = 0xb4232f;
    d.title = [](const NodeDef&, const Node& n) { return "Event " + n.param("name", "MyEvent"); };
    d.event = "logic.custom";
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.state_enter", "On State Entered", "State Machines", NodeKind::event, "Runs when a state machine enters the state.");
    d.params = {param("machine", "Machine", ParamDef::Kind::machine, "State Machine"), param("state", "State", ParamDef::Kind::state, "any")};
    d.outputs = {exec_out(), data("previous", "Previous State", PinType::text)};
    d.color = 0x0e7490;
    d.title = [](const NodeDef&, const Node& n) { return "Entered " + n.param("state", "any"); };
    d.accepts = [](const Node& n, const Event& e) {
      const std::string m = n.param("machine", "State Machine"), s = n.param("state", "any");
      return e.key.rfind(m + "/", 0) == 0 && (s.empty() || s == "any" || e.key == m + "/" + s);
    };
    d.event = "logic.state_enter";
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.state_exit", "On State Left", "State Machines", NodeKind::event, "Runs when a state machine leaves the state.");
    d.params = {param("machine", "Machine", ParamDef::Kind::machine, "State Machine"), param("state", "State", ParamDef::Kind::state, "any")};
    d.outputs = {exec_out()};
    d.color = 0x0e7490;
    d.title = [](const NodeDef&, const Node& n) { return "Left " + n.param("state", "any"); };
    d.accepts = [](const Node& n, const Event& e) {
      const std::string m = n.param("machine", "State Machine"), s = n.param("state", "any");
      return e.key.rfind(m + "/", 0) == 0 && (s.empty() || s == "any" || e.key == m + "/" + s);
    };
    d.event = "logic.state_exit";
    lib.add(std::move(d));
  }
}

void add_flow(Library& lib) {
  {
    // Reroute nodes (Blueprint knots): a wire bends through them; they change nothing.
    NodeDef d = make("flow.reroute", "Reroute (exec)", "Flow", NodeKind::action, "A knot an exec wire passes through, to route wires tidily.");
    d.inputs = {exec_in()};
    d.outputs = {exec_out()};
    d.keywords = "knot reroute wire";
    d.run = [](Context&) { return 0; };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("value.reroute", "Reroute", "Flow", NodeKind::pure, "A knot a data wire passes through, to route wires tidily.");
    d.params = {type_param(true)};
    d.keywords = "knot reroute wire";
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const TypeRef t = setting_type(n, g, true);
      in = {typed_pin("in", "", t, zero_json(t, g))};
      out = {typed_pin("out", "", t)};
    };
    d.run = [](Context& c) {
      c.out(std::size_t{0}, c.in(std::size_t{0}));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.branch", "Branch", "Flow", NodeKind::flow, "If the condition is true, continue at True, otherwise at False.");
    d.inputs = {exec_in(), data("condition", "Condition", PinType::boolean, true)};
    d.outputs = {exec_out("true", "True"), exec_out("false", "False")};
    d.keywords = "if else condition";
    d.run = [](Context& c) { return c.in_bool("condition") ? 0 : 1; };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.sequence", "Sequence", "Flow", NodeKind::flow, "Runs its outputs one after the other.");
    d.inputs = {exec_in()};
    d.params = {param("count", "Outputs", ParamDef::Kind::integer, "2")};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>&, std::vector<PinDef>& out) {
      out.clear();
      const int k = clamp_count(n, "count", 2, 1, 16);
      for (int i = 0; i < k; ++i) out.push_back(exec_out("then" + std::to_string(i), "Then " + std::to_string(i)));
    };
    d.run = [](Context& c) {
      const int k = clamp_count(c.node(), "count", 2, 1, 16);
      for (int i = 1; i < k; ++i) c.then(i);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.for", "For Loop", "Flow", NodeKind::flow, "Runs Loop Body for every index from First to Last (inclusive), then Completed.");
    d.inputs = {exec_in(), data("first", "First Index", PinType::i64, 0), data("last", "Last Index", PinType::i64, 9)};
    d.outputs = {exec_out("body", "Loop Body"), data("index", "Index", PinType::i64), exec_out("completed", "Completed")};
    d.keywords = "repeat for each count";
    // State in slots (index, last): the same in the interpreter and in generated code.
    d.loop = true;
    d.memory = true;
    d.run = [](Context& c) {
      f64* m = &c.slot(0);
      if (c.entry() != Context::kResume) {
        const i64 first = c.in_int("first");
        flow::for_start(m, first, c.in_int("last"));
      } else {
        flow::for_step(m);
      }
      if (!flow::for_runs(m) || !c.loop_back(Context::kResume)) return c.exec("completed");
      c.out("index", Value::integer(flow::for_index(m)));
      return c.exec("body");
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.while", "While Loop", "Flow", NodeKind::flow, "Runs Loop Body while the condition is true (checked before every pass).");
    d.inputs = {exec_in(), data("condition", "Condition", PinType::boolean, false)};
    d.outputs = {exec_out("body", "Loop Body"), exec_out("completed", "Completed")};
    d.run = [](Context& c) {
      if (!c.in_bool("condition") || !c.loop_back(Context::kResume)) return c.exec("completed");
      return c.exec("body");
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.do_once", "Do Once", "Flow", NodeKind::flow, "Lets the first call through; later calls are ignored until Reset.");
    d.inputs = {exec_in(), exec_in("reset", "Reset"), data("closed", "Start Closed", PinType::boolean, false)};
    d.outputs = {exec_out("completed", "Completed")};
    d.memory = true;
    d.first_entry_only = true;
    d.run = [](Context& c) { return flow::do_once(&c.slot(0), c.entry(), c.in_bool("closed")) ? 0 : kStop; };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.do_n", "Do N", "Flow", NodeKind::flow, "Lets N calls through, then blocks until Reset.");
    d.inputs = {exec_in(), data("n", "N", PinType::i64, 3), exec_in("reset", "Reset")};
    d.outputs = {exec_out("exit", "Exit"), data("counter", "Counter", PinType::i64)};
    d.memory = true;
    d.first_entry_only = true;
    d.run = [](Context& c) {
      i64 counter = 0;
      if (!flow::do_n(&c.slot(0), c.entry(), c.in_int("n"), counter)) return kStop;
      c.out("counter", Value::integer(counter));
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.gate", "Gate", "Flow", NodeKind::flow, "Enter passes through while the gate is open. Open, Close and Toggle change it.");
    d.inputs = {exec_in("enter", "Enter"), exec_in("open", "Open"), exec_in("close", "Close"), exec_in("toggle", "Toggle"), data("closed", "Start Closed", PinType::boolean, false)};
    d.outputs = {exec_out("exit", "Exit")};
    d.memory = true;
    d.first_entry_only = true;
    d.run = [](Context& c) { return flow::gate(&c.slot(0), c.entry(), c.in_bool("closed")) ? 0 : kStop; };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.flip_flop", "Flip Flop", "Flow", NodeKind::flow, "Alternates between A and B on every call.");
    d.inputs = {exec_in()};
    d.outputs = {exec_out("a", "A"), exec_out("b", "B"), data("is_a", "Is A", PinType::boolean)};
    d.memory = true;
    d.run = [](Context& c) {
      bool is_a = false;
      const int out = flow::flip_flop(&c.slot(0), is_a);
      c.out("is_a", Value::boolean(is_a));
      return out;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.delay", "Delay", "Flow", NodeKind::flow, "Waits, then continues. Calls while waiting are ignored.");
    d.latent = true;
    d.memory = true;
    d.inputs = {exec_in(), data("duration", "Duration", PinType::f64, 1)};
    d.outputs = {exec_out("completed", "Completed")};
    d.keywords = "wait timer sleep";
    d.run = [](Context& c) {
      f64* m = &c.slot(0);
      if (c.entry() == Context::kResume) {
        flow::delay_done(m);
        return 0;
      }
      const f64 duration = c.in_number("duration");
      return flow::delay_start(m) ? c.wait(duration) : kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.retrigger_delay", "Retriggerable Delay", "Flow", NodeKind::flow, "Waits; every new call restarts the wait.");
    d.latent = true;
    d.memory = true;
    d.inputs = {exec_in(), data("duration", "Duration", PinType::f64, 1)};
    d.outputs = {exec_out("completed", "Completed")};
    d.run = [](Context& c) {
      if (c.entry() == Context::kResume) return 0;
      c.cancel_waits();
      return c.wait(c.in_number("duration"));
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.tween", "Tween", "Flow", NodeKind::flow,
                     "Runs Update every step for Duration seconds with Alpha going 0 to 1 (eased), then Finished. Use it to move, fade or scale over time (a timeline).");
    d.latent = true;
    d.memory = true;
    d.first_entry_only = true;  // (Stop ends the path)
    d.now_outputs = 1;          // (Update runs with Play; Finished never does)
    d.inputs = {exec_in("play", "Play"), exec_in("stop", "Stop"), data("duration", "Duration", PinType::f64, 1)};
    d.params = {param("ease", "Easing", ParamDef::Kind::choice, "smooth", {"linear", "Linear", "smooth", "Smooth", "in", "Ease in", "out", "Ease out"})};
    d.outputs = {exec_out("update", "Update"), exec_out("finished", "Finished"), data("alpha", "Alpha", PinType::f64)};
    d.keywords = "timeline lerp interpolate animate over time";
    d.run = [](Context& c) {
      f64* m = &c.slot(0);
      const f64 duration = c.entry() == Context::kResume ? 0.0 : c.in_number("duration");  // (read with every call, Stop too)
      if (c.entry() == 1) {
        c.cancel_waits();
        flow::tween_stop(m);
        return kStop;
      }
      const f64 now = c.host().time();
      if (c.entry() == 0) {
        c.cancel_waits();
        flow::tween_play(m, now, duration);
      }
      if (!flow::tween_runs(m)) return kStop;
      bool finished = false;
      c.out("alpha", Value::number(flow::tween_step(m, now, c.param("ease", "smooth"), c.entry() == 0, finished)));
      if (finished) c.then(c.exec("finished"));  // (after Update)
      else c.wait(0, Context::kResume);          // again next step
      return c.exec("update");
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.switch_int", "Switch on Int", "Flow", NodeKind::flow, "Continues at the output matching Selection, or Default.");
    d.inputs = {exec_in(), data("selection", "Selection", PinType::i64, 0)};
    d.params = {param("count", "Cases", ParamDef::Kind::integer, "3")};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>&, std::vector<PinDef>& out) {
      out.clear();
      const int k = clamp_count(n, "count", 3, 1, 32);
      for (int i = 0; i < k; ++i) out.push_back(exec_out("case" + std::to_string(i), std::to_string(i)));
      out.push_back(exec_out("default", "Default"));
    };
    d.run = [](Context& c) { return flow::switch_int(clamp_count(c.node(), "count", 3, 1, 32), c.in_int("selection")); };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.switch_text", "Switch on Text", "Flow", NodeKind::flow, "Continues at the output whose text equals Selection (cases separated by commas), or Default.");
    d.inputs = {exec_in(), data("selection", "Selection", PinType::text, "")};
    d.params = {param("cases", "Cases", ParamDef::Kind::text, "A, B")};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>&, std::vector<PinDef>& out) {
      out.clear();
      int i = 0;
      for (const std::string& s : setting_cases(n.param("cases", "A, B"))) out.push_back(exec_out("case" + std::to_string(i++), s));
      out.push_back(exec_out("default", "Default"));
    };
    d.run = [](Context& c) { return flow::switch_text(c.param("cases", "A, B"), c.in_text("selection")); };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.call", "Call Event", "Flow", NodeKind::action,
                     "Raises a Custom Event: on Target's graph, or on every graph when Target is empty. Also switches state machine transitions with that name.");
    d.world = true;
    d.inputs = {exec_in(), data("target", "Target", PinType::actor, 0), data("actor", "Actor", PinType::actor, 0), data("value", "Value", PinType::f64, 0),
                data("text", "Text", PinType::text, "")};
    d.params = {param("name", "Event", ParamDef::Kind::event, "MyEvent")};
    d.outputs = {exec_out()};
    d.keywords = "broadcast dispatch message send trigger";
    d.title = [](const NodeDef&, const Node& n) { return "Call " + n.param("name", "MyEvent"); };
    d.run = [](Context& c) {
      std::vector<std::pair<std::string, Value>> args{{"actor", c.in("actor")}, {"value", c.in("value")}, {"text", c.in("text")}};
      c.runtime().call(c.instance(), c.param("name", "MyEvent"), c.in_actor("target"), std::move(args));
      return 0;
    };
    d.raises = true;
    lib.add(std::move(d));
  }
}

void add_variables(Library& lib) {
  auto var_shape = [](bool input, bool output) {
    return [input, output](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      const PinType t = v ? v->type : PinType::f64;
      for (PinDef& p : in) if (p.id == "value" && input) { p.type = t; p.of = v ? v->of : std::string(); p.def = v ? v->value : json::Value(0.0); }
      for (PinDef& p : out) if (p.id == "value" && output) { p.type = t; p.of = v ? v->of : std::string(); }
    };
  };
  {
    NodeDef d = make("var.get", "Get Variable", "Variables", NodeKind::pure, "Reads a variable of this graph.");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable), local_of(), path_of()};
    d.outputs = {data("value", "Value", PinType::f64)};
    d.shape = var_shape(false, true);
    d.title = [](const NodeDef&, const Node& n) { return place_title(n); };
    d.run = [](Context& c) {
      c.out(std::size_t{0}, c.instance().place(c.node(), c.path()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("var.set", "Set Variable", "Variables", NodeKind::action,
                     "Writes a variable of this graph - or, with a path, a place in it: one field of a struct, one entry of a list (beside the list: nothing). "
                     "Value: what is there now.",
                     "assign change write field member entry item element");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in(), data("value", "Value", PinType::f64)};
    d.outputs = {exec_out(), data("value", "Value", PinType::f64)};
    d.shape = var_shape(true, true);
    d.title = [](const NodeDef&, const Node& n) { return "Set " + place_title(n); };
    d.run = [](Context& c) {
      const Value value = c.in("value");
      const std::vector<i64> at = c.path();
      const Graph& g = c.instance().graph();
      Variable named;
      const TypeRef type = g.place_of(c.node(), named) ? declared(named) : TypeRef{value.type, {}};  // (a list of a fixed length keeps its length)
      Value now;
      c.instance().change(c.node(), at, [&](Value& v) {
        v = from_outside(value, type, g);
        now = v;
      });
      c.out("value", now);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("var.take", "Take Variable", "Variables", NodeKind::action,
                     "Takes what a variable holds - or a place in it, one field of a struct, one entry of a list - and leaves the zero of its type there: a list without items, "
                     "an empty text, 0, a struct with its defaults. Value: what was there (a list, a text is handed on, not copied).",
                     "move hand over empty clear exchange take out");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in()};
    d.outputs = {exec_out(), data("value", "Value", PinType::f64)};
    d.shape = var_shape(false, true);
    d.title = [](const NodeDef&, const Node& n) { return "Take " + place_title(n); };
    d.run = [](Context& c) {
      const std::vector<i64> at = c.path();
      const Graph& g = c.instance().graph();
      Variable named;
      Value taken = g.place_of(c.node(), named) ? zero_of(declared(named), g) : Value();
      c.instance().change(c.node(), at, [&](Value& v) { std::swap(taken, v); });
      c.out("value", taken);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("var.add", "Add to Variable", "Variables", NodeKind::action,
                     "Adds Amount to a number variable - x += amount of C++ in the variable's type (numbers.h: a whole number goes round at its width).");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in(), data("amount", "Amount", kFloat, 1.0)};
    d.outputs = {exec_out(), data("value", "Value", kFloat)};
    d.keywords = "increment decrement counter plus";
    d.title = [](const NodeDef&, const Node& n) { return "Add to " + place_title(n); };
    // The amount and the result have the variable's type (a place of it: the place's).
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      if (!v || !is_number(v->type) || v->type == PinType::boolean) return;
      for (PinDef& p : in) if (p.id == "amount") { p.type = v->type; p.def = json::Value(1); }
      for (PinDef& p : out) if (p.id == "value") p.type = v->type;
    };
    d.problem = [](const Node& n, const Graph& g) {
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      return v && (!is_number(v->type) || v->type == PinType::boolean)
                 ? "needs a number variable: \"" + v->name + "\" is " + std::string(pin_type_name(v->type)) + " - use Set"
                 : std::string();
    };
    d.run = [](Context& c) {
      const Value amount = c.in("amount");
      const std::vector<i64> at = c.path();
      Value now;
      c.instance().change(c.node(), at, [&](Value& v) {
        // (the number where it is, in its type: num::add - as C++'s += on it)
        if (is_number(v.type) && v.type != PinType::boolean && amount.type == v.type)
          v = with_number(v.type, [&](auto t) { using T = typename decltype(t)::type; if constexpr (num::Arithmetic<T>) return Value::of_number(num::add(v.as<T>(), amount.as<T>())); else return v; });
        now = v;
      });
      c.out("value", now);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("state.set", "Set State", "State Machines", NodeKind::action, "Switches a state machine to a state directly.");
    d.params = {param("machine", "Machine", ParamDef::Kind::machine, "State Machine"), param("state", "State", ParamDef::Kind::state, "")};
    d.inputs = {exec_in()};
    d.outputs = {exec_out()};
    d.title = [](const NodeDef&, const Node& n) { return "Go to " + n.param("state", "?"); };
    d.run = [](Context& c) {
      c.instance().set_state(c.param("machine", "State Machine"), c.param("state"));
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("state.get", "Current State", "State Machines", NodeKind::pure, "The name of the state a machine is in.");
    d.params = {param("machine", "Machine", ParamDef::Kind::machine, "State Machine")};
    d.outputs = {data("state", "State", PinType::text)};
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::text(c.instance().state(c.param("machine", "State Machine"))));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("state.is", "Is In State", "State Machines", NodeKind::pure, "True while the machine is in the state.");
    d.params = {param("machine", "Machine", ParamDef::Kind::machine, "State Machine"), param("state", "State", ParamDef::Kind::state, "")};
    d.outputs = {data("result", "Result", PinType::boolean)};
    d.title = [](const NodeDef&, const Node& n) { return "Is " + n.param("state", "?"); };
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::boolean(c.instance().state(c.param("machine", "State Machine")) == c.param("state")));
      return kStop;
    };
    lib.add(std::move(d));
  }
}

// ---------------------------------------------------------------------------
// lists
// ---------------------------------------------------------------------------
// A list holds values of one type. The value nodes read a list (a wire, a variable, Make List);
// the steps change a list variable; For Each goes through a list as it was when the loop started.
// What they do is in kern/logic/nodes.h (logic::list): a Blueprint's C++ calls the same functions.

/// The "Type" of a generic list node: the type of the list's items.
TypeRef item_type(const Node& n, const Graph& g) { return setting_type(n, g, false); }
/// Calls f(std::type_identity<T>{}) with the C++ type of a list's items.
template <class F> void with_items(PinType element, F&& f) {
  if (is_number(element)) return with_number(element, f);
  switch (element) {
    case PinType::vector: f(std::type_identity<Vec3>{}); break;
    case PinType::text: f(std::type_identity<std::string>{}); break;
    case PinType::actor: f(std::type_identity<ActorHandle>{}); break;
    case PinType::object: f(std::type_identity<ObjectRef>{}); break;
    case PinType::enumeration: f(std::type_identity<i64>{}); break;  // (an enum's value is its place among them)
    case PinType::structure: f(std::type_identity<Value>{}); break;  // (a struct is the value that holds its fields)
    default: break;
  }
}

void add_lists(Library& lib) {
  // ---- values: generic over the type of the items (the setting Type) ----
  auto typed = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
    const TypeRef e = item_type(n, g);
    for (std::vector<PinDef>* pins : {&in, &out})
      for (PinDef& p : *pins) {
        if (p.id == "list" || p.id == "other") { retype(p, {list_of(e.type), e.of}, g); p.def = json::Value::array(); }
        if (p.id == "item") retype(p, e, g);
      }
  };
  {
    NodeDef d = make("list.make", "Make List", "Lists", NodeKind::pure, "A list of the values given, in their order.", "array create");
    d.params = {type_param(false), param("count", "Items", ParamDef::Kind::integer, "2")};
    d.outputs = {data("list", "List", PinType::f64_list)};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const TypeRef e = item_type(n, g);
      in.clear();
      const int k = clamp_count(n, "count", 2, 0, 16);
      for (int i = 0; i < k; ++i) in.push_back(typed_pin("item" + std::to_string(i), std::to_string(i), e, zero_json(e, g)));
      retype(out[0], {list_of(e.type), e.of}, g);
    };
    d.run = [](Context& c) {
      const std::size_t count = c.input_count();
      const TypeRef e = item_type(c.node(), c.instance().graph());
      with_items(element_of(c.output(0).type), [&](auto type) {
        using T = typename decltype(type)::type;
        std::vector<T> items;
        for (std::size_t k = 0; k < count; ++k) items.push_back(from_value<T>(c.in(k)));
        c.out(std::size_t{0}, Value::of(std::move(items)));
      });
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("list.length", "Length", "Lists", NodeKind::pure, "How many items the list has.", "count size number of items");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list)};
    d.outputs = {data("length", "Length", PinType::i64)};
    d.shape = typed;
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::integer(static_cast<i64>(c.in(std::size_t{0}).size())));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // (no wire makes a text of a list: this node does - each item as To Text writes it, between [ and ])
    NodeDef d = make("list.text", "List to Text", "Text", NodeKind::pure, "The items of the list as a text: [1, 2.5, 3] - each as its To Text writes it.",
                     "string print show convert");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list)};
    d.outputs = {data("result", "", PinType::text)};
    d.shape = typed;
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::text(c.in(std::size_t{0}).as_text()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("list.get", "Get Item", "Lists", NodeKind::pure, "The item at the index (the first is 0). Beyond the list: the zero of its type.", "element at index array");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list), data("index", "Index", PinType::i64, 0)};
    d.outputs = {data("item", "Item", PinType::f64)};
    d.shape = typed;
    d.run = [](Context& c) {
      const Value items = c.in(std::size_t{0});
      const i64 index = c.in_int("index");
      with_items(element_of(c.input(0).type), [&](auto type) {
        using T = typename decltype(type)::type;
        c.out(std::size_t{0}, to_value(list::get(items.template items<T>(), index)));
      });
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("list.contains", "Contains", "Lists", NodeKind::pure, "Is there an item that equals the value?", "has includes in list");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list), data("item", "Item", PinType::f64, 0.0)};
    d.outputs = {data("result", "Result", PinType::boolean)};
    d.shape = typed;
    d.run = [](Context& c) {
      const Value items = c.in(std::size_t{0}), item = c.in(std::size_t{1});
      with_items(element_of(c.input(0).type), [&](auto type) {
        using T = typename decltype(type)::type;
        c.out(std::size_t{0}, Value::boolean(list::contains(items.template items<T>(), from_value<T>(item))));
      });
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("list.find", "Find Item", "Lists", NodeKind::pure, "The index of the first item that equals the value, or -1.", "index of search");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list), data("item", "Item", PinType::f64, 0.0)};
    d.outputs = {data("index", "Index", PinType::i64)};
    d.shape = typed;
    d.run = [](Context& c) {
      const Value items = c.in(std::size_t{0}), item = c.in(std::size_t{1});
      with_items(element_of(c.input(0).type), [&](auto type) {
        using T = typename decltype(type)::type;
        c.out(std::size_t{0}, Value::integer(list::find(items.template items<T>(), from_value<T>(item))));
      });
      return kStop;
    };
    lib.add(std::move(d));
  }

  {
    // Lists Equal: what C++ writes as `a == b` of two std::vector of one type (logic::list::equal)
    NodeDef d = make("list.equal", "Lists Equal", "Lists", NodeKind::pure,
                     "Do the two lists hold the same items in the same order - as many, and each equal to the one at its place?", "same compare identical == != array");
    d.params = {type_param(false)};
    d.inputs = {data("list", "List", PinType::f64_list), data("other", "Other", PinType::f64_list)};
    d.outputs = {data("result", "Result", PinType::boolean)};
    d.shape = typed;
    d.run = [](Context& c) {
      const Value items = c.in(std::size_t{0}), other = c.in(std::size_t{1});
      with_items(element_of(c.input(0).type), [&](auto type) {
        using T = typename decltype(type)::type;
        c.out(std::size_t{0}, Value::boolean(list::equal(items.template items<T>(), other.template items<T>())));
      });
      return kStop;
    };
    lib.add(std::move(d));
  }
  // ---- steps: they change a list variable (the setting Variable) ----
  auto of_variable = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
    Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
    const TypeRef e = v && is_list(v->type) ? TypeRef{element_of(v->type), v->of} : TypeRef{};
    for (PinDef& p : in) {
      if (p.id == "item" || p.id == "value") retype(p, e, g);
      if (p.id == "items") retype(p, TypeRef{list_of(e.type), e.of}, g);  // (a list of its kind)
    }
    (void)out;
  };
  auto needs_list = [](const Node& n, const Graph& g) {
    Variable named;
    const Variable* v = g.place_of(n, named) ? &named : nullptr;
    if (v && !is_list(v->type)) return "needs a list variable: \"" + v->name + "\" is " + std::string(pin_type_name(v->type));
    if (v && v->count)
      return "\"" + v->name + "\" is a list of a fixed length (" + std::to_string(v->count) + "): its entries are set (" + v->name + "[index] = value) - nothing is added to it or taken out";
    return std::string();
  };
  // A step on the list variable of the node: `change(items, c)` with the vector of its items.
  // The inputs have been read by then (each exactly once, whatever the variable is).
  auto step = [&](const char* type, const char* label, const char* help, const char* keywords, std::vector<PinDef> in, std::vector<PinDef> out) {
    NodeDef d = make(type, label, "Lists", NodeKind::action, help, keywords);
    d.params = {param("variable", "List", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in()};
    for (PinDef& p : in) d.inputs.push_back(std::move(p));
    d.outputs = {exec_out()};
    for (PinDef& p : out) d.outputs.push_back(std::move(p));
    d.shape = of_variable;
    d.problem = needs_list;
    return d;
  };
  // The list at the node's place, to change it: `change(list)` with the value that holds it. The
  // node's own inputs have been read by then; the indices of its path are read here.
  auto on_list = [](Context& c, const std::function<void(Value&)>& change) {
    const std::vector<i64> at = c.path();
    // (a list of a fixed length keeps its length: the check refuses these steps there - a graph
    // that runs all the same changes nothing)
    Variable named;
    if (c.instance().graph().place_of(c.node(), named) && named.count) return;
    c.instance().change(c.node(), at, [&](Value& v) {
      if (is_list(v.type)) change(v);
    });
  };
  {
    NodeDef d = step("list.add", "Add to List", "Adds the value at the end of a list variable. Index: where it is now.", "append push array", {data("item", "Item", PinType::f64, 0.0)},
                     {data("index", "Index", PinType::i64)});
    d.title = [](const NodeDef&, const Node& n) { return "Add to " + place_title(n); };
    d.run = [on_list](Context& c) {
      const Value item = c.in("item");
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          c.out("index", Value::integer(list::add(v.template edit<T>(), from_value<T>(item))));
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.insert", "Insert into List", "Puts the value in front of the item at the index (0: at the start; past the end: at the end).", "add at array",
                     {data("index", "Index", PinType::i64, 0), data("item", "Item", PinType::f64, 0.0)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Insert into " + place_title(n); };
    d.run = [on_list](Context& c) {
      const i64 index = c.in_int("index");
      const Value item = c.in("item");
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          list::insert(v.template edit<T>(), index, from_value<T>(item));
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.remove_at", "Remove Index", "Takes the item at the index out of a list variable. Beyond the list: nothing.", "delete erase array", {data("index", "Index", PinType::i64, 0)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Remove Index from " + place_title(n); };
    d.run = [on_list](Context& c) {
      const i64 index = c.in_int("index");
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          list::remove_at(v.template edit<T>(), index);
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.remove", "Remove Item", "Takes every item that equals the value out of a list variable. Removed: was there one?", "delete erase array",
                     {data("item", "Item", PinType::f64, 0.0)}, {data("removed", "Removed", PinType::boolean)});
    d.title = [](const NodeDef&, const Node& n) { return "Remove from " + place_title(n); };
    d.run = [on_list](Context& c) {
      const Value item = c.in("item");
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          c.out("removed", Value::boolean(list::remove(v.template edit<T>(), from_value<T>(item))));
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.resize", "Set Length",
                     "Makes a list variable that long in one go: cut at its end, or filled up with the value. Less than 0: empty; it does not grow beyond 16,777,216 items.",
                     "resize size count fill length array", {data("length", "Length", PinType::i64, 0), data("value", "Value", PinType::f64, 0.0)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Set Length of " + place_title(n); };
    d.run = [on_list](Context& c) {
      const i64 length = c.in_int("length");
      const Value value = c.in("value");
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          list::resize(v.template edit<T>(), length, from_value<T>(value));
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    // Sort List: in the order a pure function of the Blueprint says (two items -> does the first go
    // before the second?), or by the items' own order; stable (nodes.h, list::sort)
    NodeDef d = step("list.sort", "Sort List",
                     "Puts the items of a list variable in order: by Order, a pure function that takes two items and gives whether the first goes before the second - "
                     "or, without one, by their own order (numbers, whole numbers, booleans, texts, enum values, Int64 by their bits as a u64). Items that are equal in that order keep theirs.",
                     "sort order arrange rank array", {}, {});
    d.params.push_back(param("function", "Order", ParamDef::Kind::function, ""));
    d.params.back().optional = true;
    d.title = [](const NodeDef&, const Node& n) { return "Sort " + place_title(n); };
    d.problem = [needs_list = d.problem](const Node& n, const Graph& g) -> std::string {
      std::string why = needs_list ? needs_list(n, g) : std::string();
      if (!why.empty()) return why;
      Variable named;
      if (!g.place_of(n, named) || !is_list(named.type)) return {};
      const PinType e = element_of(named.type);
      const std::string fn = n.param("function", "");
      if (fn.empty()) {
        if (e == PinType::vector || e == PinType::actor || e == PinType::structure) return "a list of " + std::string(pin_type_name(e)) + " has no order of its own: Order names a function that says it";
        return {};
      }
      const Function* f = g.function(fn);
      if (!f) return "Order: there is no function \"" + fn + "\"";
      if (!f->pure) return "Order: " + fn + " is to be pure (it is asked again and again while the list is put in order)";
      // (by value or as a const reference: it reads them where they stand, it changes nothing)
      auto item = [&](const FunctionPin& in) { return in.type == e && in.of == named.of && (!in.ref || in.constant); };
      const bool takes = f->inputs.size() == 2 && item(f->inputs[0]) && item(f->inputs[1]);
      if (!takes || f->outputs.size() != 1 || f->outputs[0].type != PinType::boolean)
        return "Order: " + fn + " takes two items of the list (by value or as const references) and gives one Bool";
      return {};
    };
    d.run = [](Context& c) {
      const std::vector<i64> at = c.path();  // (its indices, read once)
      Value list;
      c.instance().change(c.node(), at, [&](Value& v) { list = v; });  // (taken out: the order is worked out apart from the place)
      Variable named;
      if (!is_list(list.type) || (c.instance().graph().place_of(c.node(), named) && named.count)) return 0;
      const std::string fn = c.param("function", "");
      auto before = [&](const Value& a, const Value& b) -> bool {
        if (!fn.empty()) {
          std::vector<Value> r;
          c.instance().call_inside(fn, {a, b}, r);
          return !r.empty() && r[0].as_bool();
        }
        if (is_number(a.type)) return with_number(a.type, [&](auto t) { using T = typename decltype(t)::type; return a.as<T>() < b.as<T>(); });  // (as C++'s <)
        switch (a.type) {
          case PinType::enumeration: return a.i < b.i;
          case PinType::text: return a.s < b.s;
          case PinType::object: return a.as_object() < b.as_object();  // (by world, then handle - logic::ObjectRef's <)
          default: return false;
        }
      };
      std::vector<std::size_t> order(list.size());
      for (std::size_t k = 0; k < order.size(); ++k) order[k] = k;
      std::stable_sort(order.begin(), order.end(), [&](std::size_t i, std::size_t j) { return before(list.item(i), list.item(j)); });
      Value sorted = list;
      with_items(element_of(list.type), [&](auto type) {
        using T = typename decltype(type)::type;
        const std::vector<T>& from = list.items<T>();
        std::vector<T>& to = sorted.template edit<T>();
        for (std::size_t k = 0; k < order.size(); ++k) to[k] = from[order[k]];
      });
      c.instance().change(c.node(), at, [&](Value& v) { v = std::move(sorted); });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.append", "Append List", "Adds the items of another list at the end of a list variable, in their order and in one go.", "add all extend concat join array",
                     {data("items", "Items", PinType::f64_list)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Append to " + place_title(n); };
    d.run = [on_list](Context& c) {
      const Value items = c.in("items");
      on_list(c, [&](Value& v) {
        if (items.type != v.type) return;
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          // (the node's own value of the items: a list into itself reads them as they were)
          const std::vector<T>& more = items.template items<T>();
          list::append(v.template edit<T>(), more);
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.insert_list", "Insert List",
                     "Puts the items of another list in front of the item at the index, in their order and in one go (0: at the start; past the end: at the end).", "add all at array",
                     {data("index", "Index", PinType::i64, 0), data("items", "Items", PinType::f64_list)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Insert List into " + place_title(n); };
    d.run = [on_list](Context& c) {
      const i64 index = c.in_int("index");
      const Value items = c.in("items");
      on_list(c, [&](Value& v) {
        if (items.type != v.type) return;
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          const std::vector<T>& more = items.template items<T>();
          list::insert_list(v.template edit<T>(), index, more);
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    // Insert Text Bytes: the bytes of a text (0 .. 255) into a list of whole numbers (logic::texts::insert_bytes)
    NodeDef d = step("list.insert_text", "Insert Text Bytes",
                     "Puts the bytes of a text - each a whole number 0 to 255 - in front of the item at the index of a list of whole numbers, in their order and in one go "
                     "(0: at the start; past the end: at the end): what inserting a text's range into a std::vector<u8> does.",
                     "bytes text string utf8 append insert array", {data("index", "Index", PinType::i64, 0), data("text", "Text", PinType::text, "")}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Insert Text Bytes into " + place_title(n); };
    d.problem = [needs_list](const Node& n, const Graph& g) {
      const std::string why = needs_list(n, g);
      if (!why.empty()) return why;
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      if (v && !(is_list(v->type) && is_integer(element_of(v->type)))) return "needs a list of whole numbers: \"" + v->name + "\" is " + std::string(pin_type_name(v->type));
      return std::string();
    };
    d.run = [on_list](Context& c) {
      const i64 index = c.in_int("index");
      const std::string text = c.in_text("text");
      on_list(c, [&](Value& v) {
        // (each byte as C++ stores a char in an entry of the list's type: an i8 holds 128 .. 255 as -128 .. -1)
        if (!is_list(v.type) || !is_integer(element_of(v.type))) return;
        with_number(element_of(v.type), [&](auto t) {
          using T = typename decltype(t)::type;
          if constexpr (num::Integer<T>) texts::insert_bytes(v.template edit<T>(), index, text);
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.reserve", "Reserve",
                     "Makes room for that many items in a list variable: nothing one can read changes - the list then grows that far without asking for memory again.",
                     "capacity room memory array", {data("count", "Count", PinType::i64, 0)}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Reserve in " + place_title(n); };
    d.run = [](Context& c) {
      // (a list of the interpreter has no room to make: the step reads its input and goes on)
      (void)c.in_int("count");
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    // Copy Bytes: what std::memcpy between lists of numbers does - the bytes of numbers, in one go
    // (logic::bytes::copy, nodes.h: the same function for the interpreter and the class).
    auto code_param = [](const char* key, const char* label) {
      return param(key, label, ParamDef::Kind::choice, "u8",
                   {"u8", "u8", "i8", "i8", "u16", "u16", "i16", "i16", "u32", "u32", "i32", "i32", "u64", "u64", "i64", "i64", "f32", "f32", "f64", "f64"});
    };
    auto list_of_code = [](bytes::Code c) { return list_of(bytes::type_of(c)); };
    NodeDef d = make("list.copy_bytes", "Copy Bytes", "Lists", NodeKind::action,
                     "Writes Count bytes of the numbers of Items, from its item From on, into the list variable from its item At on - each item of a list the "
                     "number its setting says (the list's own type: u8 i8 u16 i16 u32 i32 u64 i64 f32 f64), little-endian, as C++ lays them out: what "
                     "std::memcpy between lists of numbers does, in one go. A byte Items does not have is 0; one beyond the list is not written (it keeps "
                     "its length); an item some of whose bytes are written keeps the others. Items are read as they were (a list into itself, as memmove).",
                     "memcpy memmove bytes binary serialize wire buffer float array copy");
    d.params = {param("variable", "List", ParamDef::Kind::variable), local_of(), path_of(), code_param("code", "Number"), code_param("from_code", "Number of Items")};
    d.inputs = {exec_in(), data("at", "At", PinType::i64, 0), any_ints("items", "Items"), data("from", "From", PinType::i64, 0),
                data("count", "Count", PinType::i64, 0)};
    d.outputs = {exec_out()};
    d.title = [](const NodeDef&, const Node& n) { return "Copy Bytes into " + place_title(n); };
    d.shape = [list_of_code](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>&) {
      const bytes::Code of = bytes::code_of(n.param("from_code", "u8"));
      for (PinDef& p : in)
        if (p.id == "items" && of != bytes::Code::none && of != bytes::Code::boolean) retype(p, TypeRef{list_of_code(of), {}}, g);
    };
    d.problem = [](const Node& n, const Graph& g) -> std::string {
      const bytes::Code into = bytes::code_of(n.param("code", "u8")), of = bytes::code_of(n.param("from_code", "u8"));
      if (into == bytes::Code::none || into == bytes::Code::boolean || of == bytes::Code::none || of == bytes::Code::boolean)
        return "a number is one of u8 i8 u16 i16 u32 i32 u64 i64 f32 f64";
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      if (v && !is_list(v->type)) return "needs a list variable: \"" + v->name + "\" is " + std::string(pin_type_name(v->type));
      if (v && !bytes::suits_list(into, element_of(v->type)))
        return "\"" + v->name + "\" is a list of " + std::string(pin_type_name(element_of(v->type))) + ": its numbers are not " + n.param("code", "u8") +
               " (the number of a list is its own type)";
      return {};
    };
    d.run = [](Context& c) {
      const i64 at = c.in_int("at");
      const Value items = c.in("items");
      const i64 from = c.in_int("from"), count = c.in_int("count");
      const bytes::Code into = bytes::code_of(c.param("code", "u8")), of = bytes::code_of(c.param("from_code", "u8"));
      const std::vector<i64> path = c.path();  // (its indices, read after its inputs - as every step on a list)
      c.instance().change(c.node(), path, [&](Value& v) {
        if (!is_list(v.type) || !is_list(items.type) || !bytes::suits_list(into, element_of(v.type)) || !bytes::suits_list(of, element_of(items.type))) return;
        auto numbers = [](PinType element, auto&& f) {
          if (is_number(element) && element != PinType::boolean)
            with_number(element, [&](auto t) { if constexpr (!std::is_same_v<typename decltype(t)::type, bool>) f(t); });
        };
        numbers(element_of(v.type), [&](auto to_type) {
          using D = typename decltype(to_type)::type;
          numbers(element_of(items.type), [&](auto from_type) {
            using S = typename decltype(from_type)::type;
            const std::vector<S>& source = items.template items<S>();
            std::vector<D>& target = v.template edit<D>();
            bytes::copy(target.data(), target.size(), into, at, source.data(), source.size(), of, from, count);
          });
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = step("list.clear", "Clear List", "Takes every item out of a list variable.", "empty reset array", {}, {});
    d.title = [](const NodeDef&, const Node& n) { return "Clear " + place_title(n); };
    d.run = [on_list](Context& c) {
      on_list(c, [&](Value& v) {
        with_items(element_of(v.type), [&](auto type) {
          using T = typename decltype(type)::type;
          list::clear(v.template edit<T>());
        });
      });
      return 0;
    };
    lib.add(std::move(d));
  }

  // ---- For Each ----
  {
    NodeDef d = make("flow.for_each", "For Each", "Flow", NodeKind::flow, "Runs Loop Body for every item of the list - as the list is when the loop starts - then Completed.",
                     "loop list array every all items");
    d.params = {type_param(false)};
    d.inputs = {exec_in(), data("list", "List", PinType::f64_list)};
    d.outputs = {exec_out("body", "Loop Body"), data("item", "Item", PinType::f64), data("index", "Index", PinType::i64), exec_out("completed", "Completed")};
    d.shape = typed;
    d.loop = true;
    d.memory = true;
    d.run = [](Context& c) {
      // Where the loop is, is the node's: the list as the loop started, and the pass.
      NodeState& s = c.state();
      if (c.entry() != Context::kResume) {
        s.value = c.in("list");
        s.index = 0;
      } else {
        ++s.index;
      }
      if (s.index < 0 || static_cast<u64>(s.index) >= s.value.size() || !c.loop_back(Context::kResume)) return c.exec("completed");
      c.out("item", s.value.item(static_cast<std::size_t>(s.index)));
      c.out("index", Value::integer(s.index));
      return c.exec("body");
    };
    lib.add(std::move(d));
  }
}

// ---------------------------------------------------------------------------
// maps and sets
// ---------------------------------------------------------------------------
// A map holds values of one type, each under a key of its own, in the order the keys came; a set
// holds keys in that order (kern/core/map.h). The value nodes read one; the steps change one at
// a place (a variable, a field, an entry of a list ...). A value of a map is a place by its
// entry's place in that order (Cells[i]). What they do is in kern/logic/nodes.h (logic::map,
// logic::set): a Blueprint's C++ calls the same functions on the same container.

namespace {
ParamDef key_param() {
  ParamDef p = param("key", "Key", ParamDef::Kind::type, "i64");
  p.keys_only = true;
  return p;
}
ParamDef value_param() {
  ParamDef p = param("value", "Value", ParamDef::Kind::type, "f32");
  p.with_lists = true;
  return p;
}
/// The keys' type a node's setting "Key" names (an Int when it names none a key can be).
TypeRef key_setting(const Node& n, const Graph& g) {
  TypeRef t;
  if (!type_from_text(n.param("key", "i64"), g, t) || !key_type_ok(t.type)) t = {PinType::i64, {}};
  return t;
}
/// The values' type the setting "Value" names (a Float when it names none a value can be).
TypeRef value_setting(const Node& n, const Graph& g) {
  TypeRef t;
  if (!type_from_text(n.param("value", "f32"), g, t) || is_keyed(t.type)) t = {kFloat, {}};
  return t;
}
/// The map or set at a node's place: its type (none: the place is no map or set of `kind`).
bool keyed_place(const Node& n, const Graph& g, PinType kind, TypeRef& out) {
  Variable named;
  if (!g.place_of(n, named) || named.type != kind) return false;
  out = {named.type, named.of};
  return true;
}
}  // namespace

void add_maps(Library& lib) {
  // ---- values: generic over the keys' and the values' types (the settings Key, Value) ----
  auto typed = [](bool of_map) {
    return [of_map](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const TypeRef key = key_setting(n, g), value = value_setting(n, g);
      const TypeRef whole = of_map ? TypeRef{PinType::map, keyed_of(key, &value)} : TypeRef{PinType::set, keyed_of(key, nullptr)};
      for (std::vector<PinDef>* pins : {&in, &out})
        for (PinDef& p : *pins) {
          if (p.id == "map" || p.id == "set") { retype(p, whole, g); p.def = json::Value::array(); }
          if (p.id == "key") retype(p, key, g);
          if (p.id == "value") retype(p, value, g);
        }
    };
  };
  auto value_node = [&](const char* type, const char* label, const char* help, const char* keywords, bool of_map, std::vector<PinDef> in, std::vector<PinDef> out) {
    NodeDef d = make(type, label, of_map ? "Maps" : "Sets", NodeKind::pure, help, keywords);
    d.params = of_map ? std::vector<ParamDef>{key_param(), value_param()} : std::vector<ParamDef>{key_param()};
    d.inputs = {data(of_map ? "map" : "set", of_map ? "Map" : "Set", of_map ? PinType::map : PinType::set)};
    for (PinDef& p : in) d.inputs.push_back(std::move(p));
    d.outputs = std::move(out);
    d.shape = typed(of_map);
    return d;
  };
  for (bool of_map : {true, false}) {
    const std::string kind = of_map ? "map" : "set";
    {
      NodeDef d = value_node(of_map ? "map.length" : "set.length", "Length", of_map ? "How many entries the map has." : "How many keys the set has.", "count size number of entries", of_map, {},
                             {data("length", "Length", PinType::i64)});
      d.run = [](Context& c) {
        c.out(std::size_t{0}, Value::integer(static_cast<i64>(c.in(std::size_t{0}).size())));
        return kStop;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = value_node(of_map ? "map.contains" : "set.contains", "Has Key", of_map ? "Is there an entry under the key?" : "Is the key in the set?", "contains in has find", of_map,
                             {data("key", "Key", PinType::i64, 0)}, {data("result", "Result", PinType::boolean)});
      d.run = [](Context& c) {
        const Value m = c.in(std::size_t{0});
        c.out(std::size_t{0}, Value::boolean(map::contains(m.keyed(), key_of(c.in("key")))));
        return kStop;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = value_node(of_map ? "map.key_at" : "set.key_at", "Key At",
                             of_map ? "The key of the entry at the index, in the order the keys came (the first is 0). Beyond the map: the zero of the keys' type."
                                    : "The key at the index, in the order the keys came (the first is 0). Beyond the set: the zero of the keys' type.",
                             "key by index entry at", of_map, {data("index", "Index", PinType::i64, 0)}, {data("key", "Key", PinType::i64)});
      d.run = [](Context& c) {
        const Value m = c.in(std::size_t{0});
        const i64 at = c.in_int("index");
        const PinType key = c.output(0).type;
        c.out(std::size_t{0}, at >= 0 && static_cast<u64>(at) < m.size() ? key_value(m.keyed().key_at(static_cast<std::size_t>(at)), key) : key_value(MapKey{}, key));
        return kStop;
      };
      lib.add(std::move(d));
    }
    if (of_map) {
      NodeDef d = value_node("map.find", "Find", "The value under the key. Found: is there an entry? (If not: the zero of the values' type, index -1.) Index: the entry's place.", "get lookup value at key",
                             true, {data("key", "Key", PinType::i64, 0)},
                             {data("found", "Found", PinType::boolean), data("value", "Value", PinType::f64), data("index", "Index", PinType::i64)});
      d.run = [](Context& c) {
        const Value m = c.in(std::size_t{0});
        const i64 at = map::index_of(m.keyed(), key_of(c.in("key")));
        c.out("found", Value::boolean(at >= 0));
        c.out("value", at >= 0 ? m.keyed().value_at(static_cast<std::size_t>(at)) : zero_of({c.output(1).type, c.output(1).of}, c.instance().graph()));
        c.out("index", Value::integer(at));
        return kStop;
      };
      lib.add(std::move(d));
    } else {
      NodeDef d = value_node("set.find", "Find Key", "The place of the key in the order the keys came, or -1.", "index of search", false, {data("key", "Key", PinType::i64, 0)},
                             {data("index", "Index", PinType::i64)});
      d.run = [](Context& c) {
        const Value m = c.in(std::size_t{0});
        c.out(std::size_t{0}, Value::integer(set::index_of(m.keyed(), key_of(c.in("key")))));
        return kStop;
      };
      lib.add(std::move(d));
    }
  }

  // ---- steps: they change a map or a set at the node's place (the settings Variable, Path) ----
  auto of_place = [](PinType kind) {
    return [kind](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>&) {
      TypeRef t;
      if (!keyed_place(n, g, kind, t)) return;
      for (PinDef& p : in) {
        if (p.id == "key") retype(p, key_type(t), g);
        if (p.id == "value") retype(p, value_type(t), g);
      }
    };
  };
  auto needs = [](PinType kind) {
    return [kind](const Node& n, const Graph& g) {
      Variable named;
      if (g.place_of(n, named) && named.type != kind)
        return "needs a " + std::string(kind == PinType::map ? "map" : "set") + ": \"" + named.name + "\" is " + type_text({named.type, named.of, named.count});
      return std::string();
    };
  };
  auto step = [&](PinType kind, const char* type, const char* label, const char* help, const char* keywords, std::vector<PinDef> in, std::vector<PinDef> out) {
    NodeDef d = make(type, label, kind == PinType::map ? "Maps" : "Sets", NodeKind::action, help, keywords);
    d.params = {param("variable", kind == PinType::map ? "Map" : "Set", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in()};
    for (PinDef& p : in) d.inputs.push_back(std::move(p));
    d.outputs = {exec_out()};
    for (PinDef& p : out) d.outputs.push_back(std::move(p));
    d.shape = of_place(kind);
    d.problem = needs(kind);
    return d;
  };
  // The map or set at the node's place, to change it: `change(entries)`. The node's own inputs
  // have been read by then; the indices of its path are read here.
  auto on_entries = [](Context& c, PinType kind, const std::function<void(MapEntries&)>& change) {
    const std::vector<i64> at = c.path();
    c.instance().change(c.node(), at, [&](Value& v) {
      if (v.type == kind) change(v.own_entries());
    });
  };
  {
    NodeDef d = step(PinType::map, "map.set", "Set at Key", "The key gets the value: its entry - or a new one at the end, when there is none. Added: was it new? Index: the entry's place.",
                     "put insert assign store", {data("key", "Key", PinType::i64, 0), data("value", "Value", PinType::f64, 0.0)},
                     {data("added", "Added", PinType::boolean), data("index", "Index", PinType::i64)});
    d.title = [](const NodeDef&, const Node& n) { return "Set in " + place_title(n); };
    d.run = [on_entries](Context& c) {
      const MapKey key = key_of(c.in("key"));
      // (the value as the map holds it: a U8 its low bits, as C++ stores it there)
      TypeRef t;
      const Value value = keyed_place(c.node(), c.instance().graph(), PinType::map, t) ? fitted(c.in("value"), value_type(t), c.instance().graph()) : c.in("value");
      on_entries(c, PinType::map, [&](MapEntries& e) {
        const auto [added, at] = map::set(e, key, value);
        c.out("added", Value::boolean(added));
        c.out("index", Value::integer(at));
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  for (bool of_map : {true, false}) {
    const PinType kind = of_map ? PinType::map : PinType::set;
    {
      NodeDef d = step(kind, of_map ? "map.add" : "set.add", "Add Key",
                       of_map ? "A new entry at the end with the zero of the values' type - unless the key has one. Added: was it new? Index: the entry's place."
                              : "Puts the key in at the end, unless it is there. Added: was it new? Index: its place.",
                       "insert put emplace", {data("key", "Key", PinType::i64, 0)}, {data("added", "Added", PinType::boolean), data("index", "Index", PinType::i64)});
      d.title = [](const NodeDef&, const Node& n) { return "Add to " + place_title(n); };
      d.run = [on_entries, of_map, kind](Context& c) {
        const MapKey key = key_of(c.in("key"));
        on_entries(c, kind, [&](MapEntries& e) {
          bool added;
          i64 at;
          if (of_map) {
            const auto [fresh, place] = map::add(e, key);
            added = fresh, at = place;
            // (the zero of the values' type, as the map is declared - a struct at its defaults)
            if (fresh) {
              TypeRef t;
              if (keyed_place(c.node(), c.instance().graph(), PinType::map, t)) e.value_at(static_cast<std::size_t>(at)) = zero_of(value_type(t), c.instance().graph());
            }
          } else {
            const auto [fresh, place] = map::add(e, key);
            added = fresh, at = place;
          }
          c.out("added", Value::boolean(added));
          c.out("index", Value::integer(at));
        });
        return 0;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = step(kind, of_map ? "map.remove" : "set.remove", "Remove Key", of_map ? "Takes the key's entry out (the others keep their order). Removed: was there one?" : "Takes the key out (the others keep their order). Removed: was it there?",
                       "delete erase", {data("key", "Key", PinType::i64, 0)}, {data("removed", "Removed", PinType::boolean)});
      d.title = [](const NodeDef&, const Node& n) { return "Remove from " + place_title(n); };
      d.run = [on_entries, kind](Context& c) {
        const MapKey key = key_of(c.in("key"));
        on_entries(c, kind, [&](MapEntries& e) { c.out("removed", Value::boolean(map::remove(e, key))); });
        return 0;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = step(kind, of_map ? "map.remove_at" : "set.remove_at", "Remove Index", "Takes the entry at the index out (the others keep their order). Beyond it: nothing.", "delete erase",
                       {data("index", "Index", PinType::i64, 0)}, {});
      d.title = [](const NodeDef&, const Node& n) { return "Remove from " + place_title(n); };
      d.run = [on_entries, kind](Context& c) {
        const i64 at = c.in_int("index");
        on_entries(c, kind, [&](MapEntries& e) { map::remove_at(e, at); });
        return 0;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = step(kind, of_map ? "map.clear" : "set.clear", of_map ? "Clear Map" : "Clear Set", "Takes every entry out.", "empty reset", {}, {});
      d.title = [](const NodeDef&, const Node& n) { return "Clear " + place_title(n); };
      d.run = [on_entries, kind](Context& c) {
        on_entries(c, kind, [&](MapEntries& e) { map::clear(e); });
        return 0;
      };
      lib.add(std::move(d));
    }
    {
      NodeDef d = step(kind, of_map ? "map.reserve" : "set.reserve", "Reserve", "Makes room for that many entries in one go (what is there stays as it is).", "capacity room",
                       {data("count", "Count", PinType::i64, 0)}, {});
      d.title = [](const NodeDef&, const Node& n) { return "Reserve " + place_title(n); };
      d.run = [on_entries, kind](Context& c) {
        const i64 count = c.in_int("count");
        on_entries(c, kind, [&](MapEntries& e) { map::reserve(e, count); });
        return 0;
      };
      lib.add(std::move(d));
    }
  }
}

// ---------------------------------------------------------------------------
// enums
// ---------------------------------------------------------------------------
// A Blueprint declares enums (Graph::enums); these nodes work on the values of one of them - the
// setting "Enum" says which. What they do is logic::enums (nodes.h).

void add_enums(Library& lib) {
  auto of = [](const Node& n, const Graph& g) { return g.enumeration(n.param("enum")); };
  // Every pin called value, a, b is of the node's enum; "values" is the list of them.
  auto typed = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
    const TypeRef e{PinType::enumeration, n.param("enum")};
    for (std::vector<PinDef>* pins : {&in, &out})
      for (PinDef& p : *pins) {
        if (p.id == "value" || p.id == "a" || p.id == "b") retype(p, e, g);
        if (p.id == "values") retype(p, {PinType::enumeration_list, e.of}, g);
      }
  };
  auto titled = [](const char* before, const char* after) {
    return [before, after](const NodeDef&, const Node& n) { return std::string(before) + n.param("enum", "?") + after; };
  };
  auto node = [&](const char* type, const char* label, NodeKind kind, const char* help, const char* keywords) {
    NodeDef d = make(type, label, "Enums", kind, help, keywords);
    d.params = {enum_param()};
    d.shape = typed;
    return d;
  };
  {
    NodeDef d = node("enum.equal", "Equal (Enum)", NodeKind::pure, "Are the two the same value of the enum?", "== same is compare");
    d.inputs = {data("a", "A", PinType::enumeration), data("b", "B", PinType::enumeration)};
    d.outputs = {data("result", "Result", PinType::boolean)};
    d.title = titled("Equal (", ")");
    d.run = [](Context& c) {
      const i64 a = c.in_int("a"), b = c.in_int("b");
      c.out(std::size_t{0}, Value::boolean(a == b));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("enum.name", "Enum to Text", NodeKind::pure, "The name of the value.", "string print show name");
    d.inputs = {data("value", "Value", PinType::enumeration)};
    d.outputs = {data("name", "Name", PinType::text)};
    d.title = titled("", " to Text");
    d.run = [of](Context& c) {
      const i64 value = c.in_int("value");
      const EnumType* e = of(c.node(), c.instance().graph());
      c.out(std::size_t{0}, Value::text(e ? enums::name(e->values, value) : std::string()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("enum.from_name", "Text to Enum", NodeKind::pure, "The value with this name. Found: is there one? (If not: the first value.)", "string parse name");
    d.inputs = {data("name", "Name", PinType::text, "")};
    d.outputs = {data("value", "Value", PinType::enumeration), data("found", "Found", PinType::boolean)};
    d.title = titled("Text to ", "");
    d.run = [of](Context& c) {
      const std::string name = c.in_text("name");
      const EnumType* e = of(c.node(), c.instance().graph());
      const i64 at = e ? enums::find(e->values, name) : -1;
      c.out("value", Value::enumeration(at < 0 ? 0 : at));
      c.out("found", Value::boolean(at >= 0));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("enum.index", "Enum to Index", NodeKind::pure, "The place of the value among the enum's values (the first is 0).", "int number ordinal");
    d.inputs = {data("value", "Value", PinType::enumeration)};
    d.outputs = {data("index", "Index", PinType::i64)};
    d.title = titled("", " to Index");
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::integer(c.in_int("value")));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("enum.from_index", "Index to Enum", NodeKind::pure, "The value at this place. Valid: is there one? (If not: the first value.)", "int number ordinal byte");
    d.inputs = {data("index", "Index", PinType::i64, 0)};
    d.outputs = {data("value", "Value", PinType::enumeration), data("valid", "Valid", PinType::boolean)};
    d.title = titled("Index to ", "");
    d.run = [of](Context& c) {
      const i64 index = c.in_int("index");
      const EnumType* e = of(c.node(), c.instance().graph());
      const std::size_t count = e ? e->values.size() : 0;
      c.out("value", Value::enumeration(enums::valid(count, index)));
      c.out("valid", Value::boolean(enums::is_value(count, index)));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("enum.values", "All Values", NodeKind::pure, "Every value of the enum, in their order - a list For Each goes through.", "list every all each");
    d.outputs = {data("values", "Values", PinType::enumeration_list)};
    d.title = titled("All Values of ", "");
    d.run = [of](Context& c) {
      const EnumType* e = of(c.node(), c.instance().graph());
      std::vector<i64> all;
      for (std::size_t k = 0; e && k < e->values.size(); ++k) all.push_back(static_cast<i64>(k));
      c.out(std::size_t{0}, Value::enumerations(std::move(all)));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = node("flow.switch_enum", "Switch on Enum", NodeKind::flow, "Goes on at the output of the value: one for each value of the enum.", "case select match branch");
    d.category = "Flow";
    d.inputs = {exec_in(), data("value", "Value", PinType::enumeration)};
    d.title = titled("Switch on ", "");
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const EnumType* e = g.enumeration(n.param("enum"));
      for (PinDef& p : in)
        if (p.id == "value") retype(p, {PinType::enumeration, n.param("enum")}, g);
      out.clear();
      for (std::size_t k = 0; e && k < e->values.size(); ++k) out.push_back(exec_out("case" + std::to_string(k), e->values[k]));
    };
    d.run = [](Context& c) {
      // (a value of the enum is one of its values: there is an output for each)
      const i64 value = c.in_int("value");
      int outs = 0;
      for (std::size_t k = 0; k < c.node_outputs(); ++k) outs += c.output(k).type == PinType::exec ? 1 : 0;
      return value >= 0 && value < outs ? static_cast<int>(value) : kStop;
    };
    lib.add(std::move(d));
  }
}

// ---------------------------------------------------------------------------
// structs
// ---------------------------------------------------------------------------
// A Blueprint declares structs (Graph::structs). Make gives a struct's value from a value for
// each field, Break gives the fields of one, Set Field changes one field of a struct variable.
// What they do is logic::structs (nodes.h); the generic nodes (Select, Reroute, the list nodes,
// For Each) take a struct's type as they take any other.

void add_structs(Library& lib) {
  auto of = [](const Node& n, const Graph& g) { return g.structure(n.param("struct")); };
  {
    NodeDef d = make("struct.make", "Make Struct", "Structures", NodeKind::pure, "A value of the struct, from a value for each of its fields.", "create new construct record fields");
    d.params = {struct_param()};
    d.outputs = {data("value", "Value", PinType::structure)};
    d.title = [](const NodeDef&, const Node& n) { return "Make " + n.param("struct", "?"); };
    d.shape = [of](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      in.clear();
      if (const StructType* s = of(n, g))
        for (const StructField& f : s->fields) in.push_back(typed_pin(f.name, f.name, f.type, f.value));
      retype(out[0], {PinType::structure, n.param("struct")}, g);
    };
    d.run = [](Context& c) {
      const Graph& g = c.instance().graph();
      const StructType* s = g.structure(c.param("struct"));
      std::vector<Value> fields;
      // (the fields one after the other, in their order - a list of a fixed length at its length)
      for (std::size_t k = 0; k < c.input_count(); ++k) fields.push_back(s && k < s->fields.size() && s->fields[k].type.count ? fitted(c.in(k), s->fields[k].type, g) : c.in(k));
      c.out(std::size_t{0}, structs::make(std::move(fields)));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("struct.break", "Break Struct", "Structures", NodeKind::pure, "The fields of a struct's value, each on a pin of its own.", "split fields members get read");
    d.params = {struct_param()};
    d.inputs = {data("value", "Value", PinType::structure)};
    d.title = [](const NodeDef&, const Node& n) { return "Break " + n.param("struct", "?"); };
    d.shape = [of](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      retype(in[0], {PinType::structure, n.param("struct")}, g);
      out.clear();
      if (const StructType* s = of(n, g))
        for (const StructField& f : s->fields) out.push_back(typed_pin(f.name, f.name, f.type));
    };
    d.run = [of](Context& c) {
      const Value value = c.in(std::size_t{0});
      const Graph& g = c.instance().graph();
      if (const StructType* s = of(c.node(), g))
        for (std::size_t k = 0; k < s->fields.size(); ++k) c.out(k, field_of(value, *s, k, g));
      return kStop;
    };
    lib.add(std::move(d));
  }
  // The bytes of a struct of numbers, in the layout C++ gives it (logic::bytes, nodes.h): what
  // std::bit_cast and std::memcpy between a struct and bytes see.
  auto layout_problem = [of](const Node& n, const Graph& g) -> std::string {
    const bytes::Layout l = bytes::layout(n.param("layout", ""));
    if (l.error) return std::string("the layout: ") + l.error;
    const StructType* st = of(n, g);
    if (!st) return "which struct?";
    if (st->fields.size() != l.count)
      return "the layout has " + std::to_string(l.count) + " field(s), " + st->name + " has " + std::to_string(st->fields.size());
    for (std::size_t k = 0; k < l.count; ++k)
      if (st->fields[k].type.count || !bytes::suits(l.fields[k].code, st->fields[k].type.type))
        return "the layout's field " + std::to_string(k + 1) + " does not suit " + st->name + "." + st->fields[k].name + " (a field of the layout is the number type of its field itself)";
    return {};
  };
  auto layout_param = [] { return param("layout", "Layout", ParamDef::Kind::text, "4: i32@0"); };
  {
    NodeDef d = make("struct.bytes", "Bytes of Struct", "Structures", NodeKind::pure,
                     "The bytes of a struct of numbers in the layout C++ gives it: \"20: u32@0 u16@4 ...\" - the size, then for each field the number it is "
                     "(bool u8 i8 u16 i16 u32 i32 u64 i64 f32 f64) and where it begins. Little-endian; a byte no field covers is 0.",
                     "bit cast memcpy binary bytes serialize wire packet");
    d.params = {struct_param(), layout_param()};
    d.inputs = {data("value", "Value", PinType::structure)};
    d.outputs = {bytes_out("bytes", "Bytes")};
    d.title = [](const NodeDef&, const Node& n) { return "Bytes of " + n.param("struct", "?"); };
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>&) { retype(in[0], {PinType::structure, n.param("struct")}, g); };
    d.problem = layout_problem;
    d.run = [of](Context& c) {
      const Graph& g = c.instance().graph();
      const StructType* st = of(c.node(), g);
      const bytes::Layout l = bytes::layout(c.param("layout", ""));
      std::vector<u8> out(l.error ? 0 : l.size, 0);
      if (st && !l.error) {
        const Value value = c.in(std::size_t{0});
        for (std::size_t k = 0; k < l.count && k < st->fields.size(); ++k) {
          const Value f = field_of(value, *st, k, g);
          const bytes::Field& at = l.fields[k];
          const PinType t = st->fields[k].type.type;
          // (the field's own bits: it is of the layout's number - a NaN the one NaN, numbers.h seen)
          const u64 v = is_number(t) ? with_number(t, [&](auto x) { return num::seen_bits(num::of_bits<typename decltype(x)::type>(f.converted(t).bits())); }) : 0;
          bytes::put(out.data(), at, v);
        }
      }
      c.out(std::size_t{0}, Value::of(std::move(out)));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("struct.of_bytes", "Struct of Bytes", "Structures", NodeKind::pure,
                     "The struct of numbers whose bytes begin at At in the list, in the layout C++ gives it (as Bytes of Struct). A byte is a number's last 8 bits; "
                     "where the list has none, it is 0.",
                     "bit cast memcpy binary bytes deserialize parse wire packet");
    d.params = {struct_param(), layout_param()};
    d.inputs = {any_ints("bytes", "Bytes"), data("at", "At", PinType::i64, 0)};
    d.outputs = {data("value", "Value", PinType::structure)};
    d.title = [](const NodeDef&, const Node& n) { return n.param("struct", "?") + " of Bytes"; };
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>&, std::vector<PinDef>& out) { retype(out[0], {PinType::structure, n.param("struct")}, g); };
    d.problem = layout_problem;
    d.run = [of](Context& c) {
      const Graph& g = c.instance().graph();
      const StructType* st = of(c.node(), g);
      const bytes::Layout l = bytes::layout(c.param("layout", ""));
      const Value list = c.in(std::size_t{0});
      const i64 from = c.in_int("at");
      // (the bytes: the last 8 bits of each number of the list, whatever whole type it is of)
      std::vector<i64> held;
      if (is_list(list.type) && is_integer(element_of(list.type)))
        with_number(element_of(list.type), [&](auto t) {
          using T = typename decltype(t)::type;
          for (const auto& x : list.items<T>()) held.push_back(static_cast<i64>(x));
        });
      auto byte = [&](u32 i) { return bytes::byte_at(held.data(), held.size(), from, i); };
      std::vector<Value> fields;
      if (st) {
        for (std::size_t k = 0; k < st->fields.size(); ++k) {
          if (l.error || k >= l.count) {
            fields.push_back(field_of(Value(), *st, k, g));
            continue;
          }
          const bytes::Field& at = l.fields[k];
          const u64 v = bytes::take(at, byte);
          const PinType t = st->fields[k].type.type;
          fields.push_back(is_number(t) ? Value::of_bits(t, v) : field_of(Value(), *st, k, g));
        }
      }
      c.out(std::size_t{0}, structs::make(std::move(fields)));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // A list of structs read as a table: the row whose field is the value looked for.
    NodeDef d = make("list.find_row", "Find Row", "Lists", NodeKind::pure,
                     "The first item of a list of structs whose field is this value - a table's row by its key. Found: is there one? (If not: the struct's defaults, index -1.)",
                     "table lookup data key search where by");
    d.params = {struct_param(), param("field", "Field", ParamDef::Kind::field)};
    d.inputs = {data("list", "List", PinType::structure_list), data("value", "Value", PinType::f64)};
    d.outputs = {data("found", "Found", PinType::boolean), data("row", "Row", PinType::structure), data("index", "Index", PinType::i64)};
    d.title = [](const NodeDef&, const Node& n) { return "Find " + n.param("struct", "?") + " by " + n.param("field", "?"); };
    d.shape = [of](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const std::string name = n.param("struct");
      retype(in[0], {PinType::structure_list, name}, g);
      retype(out[1], {PinType::structure, name}, g);
      const StructType* s = of(n, g);
      const int k = s ? s->field(n.param("field")) : -1;
      if (k < 0) return;
      const StructField& f = s->fields[static_cast<std::size_t>(k)];
      retype(in[1], f.type, g);
      in[1].def = f.value;
    };
    d.problem = [of](const Node& n, const Graph& g) {
      const StructType* s = of(n, g);
      if (!s || s->field(n.param("field")) >= 0) return std::string();  // (the check says that there is no such struct)
      std::string all;
      for (const StructField& f : s->fields) all += (all.empty() ? "" : ", ") + f.name;
      return s->name + " has no field \"" + n.param("field") + "\" (" + all + ")";
    };
    d.run = [of](Context& c) {
      const Value list = c.in("list"), value = c.in("value");
      const Graph& g = c.instance().graph();
      const StructType* s = of(c.node(), g);
      const int k = s ? s->field(c.param("field")) : -1;
      i64 at = -1;
      for (std::size_t i = 0; s && k >= 0 && at < 0 && i < list.size(); ++i)
        if (field_of(list.item(i), *s, static_cast<std::size_t>(k), g) == value) at = static_cast<i64>(i);
      c.out("found", Value::boolean(at >= 0));
      c.out("row", at >= 0 ? list.item(static_cast<std::size_t>(at)) : s ? default_of(*s, g) : Value::zero(PinType::structure));
      c.out("index", Value::integer(at));
      return kStop;
    };
    lib.add(std::move(d));
  }
}

// ---------------------------------------------------------------------------
// numbers: each operation of every number type (PureNodes, kern/logic/numbers.h)

/// Which number types a node of numbers takes (its setting "Type").
enum class Family : u8 { arithmetic, integer, floating, number };
template <Family F, class T> constexpr bool family_has() {
  if constexpr (F == Family::arithmetic) return num::Arithmetic<T>;
  else if constexpr (F == Family::integer) return num::Integer<T>;
  else if constexpr (F == Family::floating) return num::Floating<T>;
  else return num::Number<T>;
}
bool in_family(PinType t, Family f) {
  return f == Family::number ? is_number(t) : f == Family::integer ? is_integer(t) : f == Family::floating ? is_floating(t) : is_number(t) && t != PinType::boolean;
}
const char* family_text(Family f) {
  return f == Family::integer ? "a whole number type (i8 ... u64)" : f == Family::floating ? "f32 or f64" : f == Family::number ? "a number type (bool, i8 ... u64, f32, f64)"
                                                                                                              : "a number type (i8 ... u64, f32, f64)";
}
/// The number type a node's setting `key` names (f32 - i64 for whole numbers - when it names none: the check says so).
PinType number_setting(const Node& n, std::string_view key, Family f) {
  PinType t;
  if (pin_type_from_name(n.param(key), t) && in_family(t, f)) return t;
  return f == Family::integer ? kInt : kFloat;
}
ParamDef number_param(std::string key, std::string label, Family f) {
  ParamDef p = param(std::move(key), std::move(label), ParamDef::Kind::type, f == Family::integer ? "i64" : "f32");
  return p;
}
/// An input of a node of numbers: of the node's type (`fixed` exec), or of a type of its own.
struct NumIn {
  const char* id;
  const char* label;
  PinType fixed = PinType::exec;
  f64 def = 0;
};
/// What a node of numbers gives: a value of its type, a bool, an i32 (a count of bits), a text.
enum class Gives : u8 { same, boolean, count, text };
/// A node of numbers: its setting "Type" is the type of its pins; `body(std::type_identity<T>, inputs)` computes it
/// with the function of PureNodes - for T of its family only.
template <Family F, class Body>
NodeDef number_node(std::string type, std::string label, std::vector<NumIn> ins, Gives gives, Body body, std::string help = {}, std::string keywords = {}) {
  NodeDef d = make(std::move(type), std::move(label), "Math", NodeKind::pure, std::move(help), std::move(keywords));
  const PinType t0 = F == Family::integer ? kInt : kFloat;
  for (const NumIn& x : ins) d.inputs.push_back(data(x.id, x.label, x.fixed == PinType::exec ? t0 : x.fixed, x.def));
  const PinType out = gives == Gives::same ? t0 : gives == Gives::boolean ? PinType::boolean : gives == Gives::count ? PinType::i32 : PinType::text;
  d.outputs = {data("result", "", out)};
  d.params = {number_param("type", "Type", F)};
  d.shape = [ins, gives](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& outs) {
    const PinType t = number_setting(n, "type", F);
    for (PinDef& p : in)
      for (const NumIn& x : ins)
        if (p.id == x.id && x.fixed == PinType::exec) retype(p, {t, {}}, g);
    if (gives == Gives::same) retype(outs[0], {t, {}}, g);
  };
  d.problem = [](const Node& n, const Graph&) -> std::string {
    PinType t;
    if (!pin_type_from_name(n.param("type", F == Family::integer ? "i64" : "f32"), t) || !in_family(t, F)) return std::string("its Type is ") + family_text(F);
    return {};
  };
  const std::size_t count = ins.size();
  d.run = [body, count](Context& c) {
    const PinType t = number_setting(c.node(), "type", F);
    Value v[4];
    for (std::size_t k = 0; k < count && k < 4; ++k) v[k] = c.in(k);
    c.out(std::size_t{0}, with_number(t, [&](auto tag) -> Value {
      using T = typename decltype(tag)::type;
      if constexpr (family_has<F, T>()) return to_value(body(tag, v));
      else return Value();
    }));
    return kStop;
  };
  return d;
}
#define KERN_NUM1(fn) [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::fn<T>(v[0].as<T>()); }
#define KERN_NUM2(fn) [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::fn<T>(v[0].as<T>(), v[1].as<T>()); }
#define KERN_NUM3(fn) [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::fn<T>(v[0].as<T>(), v[1].as<T>(), v[2].as<T>()); }
#define KERN_NUMBY(fn) [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::fn<T>(v[0].as<T>(), v[1].as<i64>()); }

void add_math(Library& lib) {
  using K = PinType;
  using P = PureNodes;
  using F = Family;
  auto ab = [](K t, json::Value a = 0.0, json::Value b = 0.0) { return std::vector<PinDef>{data("a", "A", t, a), data("b", "B", t, b)}; };
  const PinDef boolean = data("result", "", K::boolean), integer = data("result", "", K::i64);
  const std::vector<NumIn> a{{"a", "A"}}, ab2{{"a", "A"}, {"b", "B"}};
  // ---- arithmetic: in the type, as C++ (numbers.h: a whole number goes round at its width; / and % by 0 give 0) ----
  lib.add(number_node<F::arithmetic>("math.add", "Add", ab2, Gives::same, KERN_NUM2(math_add), "A + B, in the type", "+ plus sum"));
  lib.add(number_node<F::arithmetic>("math.sub", "Subtract", ab2, Gives::same, KERN_NUM2(math_sub), "A - B, in the type", "- minus"));
  lib.add(number_node<F::arithmetic>("math.mul", "Multiply", {{"a", "A", K::exec, 1.0}, {"b", "B", K::exec, 1.0}}, Gives::same, KERN_NUM2(math_mul), "A × B, in the type", "* times"));
  lib.add(number_node<F::arithmetic>("math.div", "Divide", {{"a", "A", K::exec, 1.0}, {"b", "B", K::exec, 1.0}}, Gives::same, KERN_NUM2(math_div),
                                     "A ÷ B: a whole number toward zero (by 0: 0), a floating one as IEEE 754 (by 0: inf or NaN)", "/"));
  lib.add(number_node<F::arithmetic>("math.rem", "Remainder", {{"a", "A", K::exec, 0.0}, {"b", "B", K::exec, 2.0}}, Gives::same, KERN_NUM2(math_rem),
                                     "A % B: of whole numbers with the sign of A (by 0: 0), of floating ones std::fmod", "% modulo fmod"));
  lib.add(number_node<F::arithmetic>("math.min", "Min", ab2, Gives::same, KERN_NUM2(math_min), "std::min: (B < A) ? B : A"));
  lib.add(number_node<F::arithmetic>("math.max", "Max", ab2, Gives::same, KERN_NUM2(math_max), "std::max: (A < B) ? B : A"));
  lib.add(number_node<F::arithmetic>("math.clamp", "Clamp", {{"value", "Value"}, {"min", "Min"}, {"max", "Max", K::exec, 1.0}}, Gives::same, KERN_NUM3(math_clamp),
                                     "Value between Min and Max (in either order)"));
  lib.add(number_node<F::arithmetic>("math.abs", "Absolute", a, Gives::same, KERN_NUM1(math_abs), "|A| (of the smallest signed whole number: itself)"));
  lib.add(number_node<F::arithmetic>("math.negate", "Negate", a, Gives::same, KERN_NUM1(math_negate), "-A", "minus"));
  lib.add(number_node<F::floating>("math.lerp", "Lerp", {{"a", "A"}, {"b", "B", K::exec, 1.0}, {"alpha", "Alpha", K::exec, 0.5}}, Gives::same, KERN_NUM3(math_lerp),
                                   "A + (B - A) × Alpha, each step in the type", "interpolate mix blend"));
  // ---- comparisons ----
  {
    NodeDef d = number_node<F::arithmetic>("math.compare", "Compare", ab2, Gives::boolean, [](auto, const Value*) { return false; }, "Compares two numbers of a type, as C++ does",
                                           "equal less greater == != < > <= >=");
    d.category = "Logic";
    d.params.insert(d.params.begin(), param("op", "Operator", ParamDef::Kind::choice, "==", {"==", "==", "!=", "!=", "<", "<", "<=", "<=", ">", ">", ">=", ">="}));
    d.title = [](const NodeDef&, const Node& n) { return "A " + n.param("op", "==") + " B"; };
    d.run = [](Context& c) {
      const PinType t = number_setting(c.node(), "type", F::arithmetic);
      const Value x = c.in(std::size_t{0}), y = c.in(std::size_t{1});
      const std::string op = c.param("op", "==");
      c.out(std::size_t{0}, Value::boolean(with_number(t, [&](auto tag) {
        using T = typename decltype(tag)::type;
        if constexpr (num::Arithmetic<T>) return P::compare<T>(op, x.as<T>(), y.as<T>());
        else return false;
      })));
      return kStop;
    };
    lib.add(std::move(d));
  }
  // ---- bits of whole numbers (a shift by By mod the width) ----
  lib.add(number_node<F::integer>("math.and", "Bitwise And", ab2, Gives::same, KERN_NUM2(math_and), "A & B", "& bitwise mask"));
  lib.add(number_node<F::integer>("math.or", "Bitwise Or", ab2, Gives::same, KERN_NUM2(math_or), "A | B", "| bitwise"));
  lib.add(number_node<F::integer>("math.xor", "Bitwise Xor", ab2, Gives::same, KERN_NUM2(math_xor), "A ^ B", "^ bitwise"));
  lib.add(number_node<F::integer>("math.not", "Bitwise Not", a, Gives::same, KERN_NUM1(math_not), "~A: every bit the other way", "~ invert"));
  const std::vector<NumIn> by{{"a", "A"}, {"by", "By", K::i64, 1.0}};
  lib.add(number_node<F::integer>("math.shift_left", "Shift Left", by, Gives::same, KERN_NUMBY(math_shift_left), "A << By (By mod the width)", "<< shl"));
  lib.add(number_node<F::integer>("math.shift_right", "Shift Right", by, Gives::same, KERN_NUMBY(math_shift_right),
                                  "A >> By (By mod the width): a signed A brings its sign in, an unsigned one zeros", ">> shr"));
  lib.add(number_node<F::integer>("math.rotate_left", "Rotate Left", by, Gives::same, KERN_NUMBY(math_rotate_left), "The bits turned By to the left (std::rotl)", "rotl"));
  lib.add(number_node<F::integer>("math.rotate_right", "Rotate Right", by, Gives::same, KERN_NUMBY(math_rotate_right), "The bits turned By to the right (std::rotr)", "rotr"));
  lib.add(number_node<F::integer>("math.count_ones", "Count Ones", a, Gives::count, KERN_NUM1(math_count_ones), "How many bits are 1 (std::popcount)", "popcount"));
  lib.add(number_node<F::integer>("math.leading_zeros", "Leading Zeros", a, Gives::count, KERN_NUM1(math_leading_zeros), "std::countl_zero", "clz"));
  lib.add(number_node<F::integer>("math.trailing_zeros", "Trailing Zeros", a, Gives::count, KERN_NUM1(math_trailing_zeros), "std::countr_zero", "ctz"));
  // ---- the functions of <cmath> on floating numbers (radians, as C++; the engine's own sin, exp ...: kern::dm) ----
  struct One { const char* type; const char* label; const char* help; };
#define KERN_FLOAT1(name, label, help) lib.add(number_node<F::floating>("math." #name, label, a, Gives::same, KERN_NUM1(math_##name), help))
  KERN_FLOAT1(sqrt, "Square Root", "std::sqrt (of a negative number: NaN)");
  KERN_FLOAT1(floor, "Floor", "std::floor");
  KERN_FLOAT1(ceil, "Ceil", "std::ceil");
  KERN_FLOAT1(trunc, "Truncate", "std::trunc: toward zero");
  KERN_FLOAT1(round, "Round", "std::round: half away from zero");
  KERN_FLOAT1(sin, "Sin", "std::sin (radians)");
  KERN_FLOAT1(cos, "Cos", "std::cos (radians)");
  KERN_FLOAT1(tan, "Tan", "std::tan (radians)");
  KERN_FLOAT1(asin, "Asin", "std::asin (radians)");
  KERN_FLOAT1(acos, "Acos", "std::acos (radians)");
  KERN_FLOAT1(atan, "Atan", "std::atan (radians)");
  KERN_FLOAT1(exp, "Exp", "std::exp");
  KERN_FLOAT1(expm1, "Exp Minus 1", "std::expm1");
  KERN_FLOAT1(sinh, "Sinh", "std::sinh (the engine's own: kern/src/math/hyperbolic.c)");
  KERN_FLOAT1(cosh, "Cosh", "std::cosh (the engine's own)");
  KERN_FLOAT1(tanh, "Tanh", "std::tanh (the engine's own)");
  KERN_FLOAT1(asinh, "Asinh", "std::asinh (the engine's own)");
  KERN_FLOAT1(log, "Log", "std::log (of 0: -inf, of a negative number: NaN)");
  KERN_FLOAT1(log10, "Log 10", "std::log10");
#undef KERN_FLOAT1
  lib.add(number_node<F::floating>("math.atan2", "Atan2", {{"y", "Y"}, {"x", "X", K::exec, 1.0}}, Gives::same, KERN_NUM2(math_atan2), "std::atan2(Y, X): the angle of (X, Y) in radians", "angle"));
  lib.add(number_node<F::floating>("math.pow", "Power", {{"a", "Base", K::exec, 2.0}, {"b", "Exponent", K::exec, 2.0}}, Gives::same, KERN_NUM2(math_pow), "std::pow"));
  lib.add(number_node<F::floating>("math.hypot", "Hypot", ab2, Gives::same, KERN_NUM2(math_hypot), "std::hypot"));
  lib.add(number_node<F::floating>("math.copysign", "Copy Sign", {{"a", "Magnitude"}, {"b", "Sign", K::exec, 1.0}}, Gives::same, KERN_NUM2(math_copysign),
                                   "The size of Magnitude with the sign of Sign - also the sign of -0 (std::copysign)", "copysign negative zero minus"));
  lib.add(number_node<F::floating>("math.fmin", "Fmin", ab2, Gives::same, KERN_NUM2(math_fmin), "std::fmin: a NaN on one side gives the other"));
  lib.add(number_node<F::floating>("math.fmax", "Fmax", ab2, Gives::same, KERN_NUM2(math_fmax), "std::fmax: a NaN on one side gives the other"));
  lib.add(number_node<F::floating>("math.isnan", "Is NaN", a, Gives::boolean, KERN_NUM1(math_isnan), "std::isnan"));
  lib.add(number_node<F::floating>("math.isinf", "Is Inf", a, Gives::boolean, KERN_NUM1(math_isinf), "std::isinf"));
  lib.add(number_node<F::floating>("math.isfinite", "Is Finite", a, Gives::boolean, KERN_NUM1(math_isfinite), "std::isfinite"));
  lib.add(number_node<F::floating>("math.signbit", "Sign Bit", a, Gives::boolean, KERN_NUM1(math_signbit), "std::signbit (of -0: true)"));
  // ---- numbers as texts ----
  {
    NodeDef d = number_node<F::number>("num.text", "To Text", a, Gives::text, [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::num_text<T>(v[0].as<T>()); },
                                       "The number as a text: a whole number in decimal, a floating one the shortest decimal that gives it again (std::to_chars), inf, -inf, nan",
                                       "string convert print");
    d.category = "Text";
    lib.add(std::move(d));
  }
  {
    NodeDef d = number_node<F::integer>("num.text_hex", "To Hex Text", a, Gives::text, [](auto t, const Value* v) { using T = typename decltype(t)::type; return P::num_text_hex<T>(v[0].as<T>()); },
                                        "The bits of the number in hexadecimal (lower case, no 0x)", "string hex");
    d.category = "Text";
    lib.add(std::move(d));
  }
  lib.add(pure("num.text_to_u64", "Text to u64", "Text", {data("a", "Text", K::text, "")}, data("result", "", K::u64), of(&P::text_to_u64),
               "A whole number written out (decimal, 0x hexadecimal, - in front: two's complement) - 0 if it is none", "parse string"));
  // ---- conversions: one node for each that C++ knows (static_cast<To>(x); numbers.h fixes what C++ leaves open) ----
  {
    NodeDef d = make("num.cast", "Cast", "Math", NodeKind::pure,
                     "Value as the type To, as static_cast<To> of C++ converts it: a whole number to a narrower one keeps its low bits, a floating number to a "
                     "whole one goes toward zero (NaN: 0, beyond the ends: the nearer end), to a floating one the nearest, to bool != 0.",
                     "convert static_cast IntegralCast FloatingCast IntegralToFloating FloatingToIntegral Promotion");
    d.params = {number_param("to", "To", F::number), number_param("from", "From", F::number)};
    d.inputs = {data("a", "Value", kFloat, 0.0)};
    d.outputs = {data("result", "", kFloat)};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      retype(in[0], {number_setting(n, "from", F::number), {}}, g);
      retype(out[0], {number_setting(n, "to", F::number), {}}, g);
    };
    d.title = [](const NodeDef&, const Node& n) {
      const PinType from = number_setting(n, "from", F::number), to = number_setting(n, "to", F::number);
      return std::string(pin_type_name(from)) + " → " + pin_type_name(to) + " (" + std::string(num::cast_name(num::cast_kind(kind_of(from), kind_of(to)))) + ")";
    };
    d.problem = [](const Node& n, const Graph&) -> std::string {
      PinType t;
      for (const char* k : {"to", "from"})
        if (!pin_type_from_name(n.param(k, "f32"), t) || !is_number(t)) return std::string("its ") + (k[0] == 't' ? "To" : "From") + " is a number type (bool, i8 ... u64, f32, f64)";
      return {};
    };
    d.run = [](Context& c) {
      const Value x = c.in(std::size_t{0});
      const PinType to = number_setting(c.node(), "to", F::number);
      c.out(std::size_t{0}, with_number(to, [&](auto t) { return Value::of_number(x.template cast_to<typename decltype(t)::type>()); }));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("num.bit_cast", "Bit Cast", "Math", NodeKind::pure,
                     "The bits of Value as the type To of the same size (std::bit_cast: f32 and u32 or i32, f64 and u64 or i64).", "bit cast reinterpret float bits");
    d.params = {number_param("to", "To", F::number), number_param("from", "From", F::number)};
    d.inputs = {data("a", "Value", kFloat, 0.0)};
    d.outputs = {data("result", "", PinType::u32)};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      retype(in[0], {number_setting(n, "from", F::number), {}}, g);
      retype(out[0], {number_setting(n, "to", F::number), {}}, g);
    };
    d.problem = [](const Node& n, const Graph&) -> std::string {
      const PinType from = number_setting(n, "from", F::number), to = number_setting(n, "to", F::number);
      if (from == PinType::boolean || to == PinType::boolean || num::width(kind_of(from)) != num::width(kind_of(to))) return "From and To are numbers of the same size (not bool)";
      return {};
    };
    d.run = [](Context& c) {
      const Value x = c.in(std::size_t{0});
      const PinType to = number_setting(c.node(), "to", F::number);
      // (the same bits, the other type: std::bit_cast - of a NaN the one NaN's, numbers.h seen and reinterpret)
      const u64 bits = with_number(x.type, [&](auto t) { return num::seen_bits(num::of_bits<typename decltype(t)::type>(x.bits())); });
      c.out(std::size_t{0}, Value::of_bits(to, bits));
      return kStop;
    };
    lib.add(std::move(d));
  }
  // Random: every node has a sequence of its own (logic::random), from the Blueprint's seed.
  lib.add(worldly(fresh(pure("math.random", "Random Float in Range", "Math", {data("min", "Min", K::f64, 0.0), data("max", "Max", K::f64, 1.0)}, data("result", "", K::f64), [](Context& c) {
    const f64 lo = c.in_number("min"), hi = c.in_number("max");
    return Value::number(random::number(&c.slot(0), c.instance().seed(), c.node().id, lo, hi));
  }, "Deterministic per world", "rand"))));
  lib.add(worldly(fresh(pure("math.random_int", "Random Integer in Range", "Math", {data("min", "Min", K::i64, 0), data("max", "Max", K::i64, 10)}, data("result", "", K::i64), [](Context& c) {
    const i64 lo = c.in_int("min"), hi = c.in_int("max");
    return Value::integer(random::whole_number(&c.slot(0), c.instance().seed(), c.node().id, lo, hi));
  }, "Min to Max inclusive", "rand"))));
  lib.add(worldly(fresh(pure("math.random_bool", "Random Bool", "Math", {data("chance", "Chance", K::f64, 0.5)}, data("result", "", K::boolean), [](Context& c) {
    const f64 chance = c.in_number("chance");
    return Value::boolean(random::chance(&c.slot(0), c.instance().seed(), c.node().id, chance));
  }, "True with the given chance (0..1)", "rand coin"))));
  lib.add(pure("logic.and", "AND", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_and), "", "&& both"));
  lib.add(pure("logic.or", "OR", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_or), "", "|| either"));
  lib.add(pure("logic.xor", "XOR", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_xor)));
  lib.add(pure("logic.not", "NOT", "Logic", {data("a", "A", K::boolean, false)}, boolean, of(&P::logic_not), "", "! invert"));
  {
    NodeDef d = pure("logic.select", "Select", "Logic", {data("pick", "Pick A", K::boolean, true), data("a", "A", kFloat, 0.0), data("b", "B", kFloat, 0.0)}, data("result", "", kFloat),
                     [](Context& c) {
                       // (both are read, whichever is picked: every input of a node is worked out once, as the arguments of a call)
                       const bool pick = c.in_bool("pick");
                       const Value a = c.in("a"), b = c.in("b");
                       return pick ? a : b;
                     },
                     "A when Pick A is true, otherwise B", "ternary choose if");
    d.params = {type_param(true)};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const TypeRef t = setting_type(n, g, true);
      for (PinDef& p : in) if (p.id != "pick") retype(p, t, g);
      retype(out[0], t, g);
    };
    lib.add(std::move(d));
  }
  // Literals.
  {
    NodeDef d = number_node<F::number>("value.number", "Number", {{"value", ""}}, Gives::same, KERN_NUM1(value_number), "A number written out, of its type", "float int literal constant");
    d.category = "Values";
    lib.add(std::move(d));
  }
  lib.add(pure("value.bool", "Boolean", "Values", {data("value", "", K::boolean, false)}, boolean, of(&P::value_bool), "true / false", "literal"));
  lib.add(pure("value.text", "Text", "Values", {data("value", "", K::text, "")}, data("result", "", K::text), of(&P::value_text), "A constant text", "string literal"));
  // Vectors (of f32: kern::Vec3).
  const json::Value zero3 = [] { json::Value a = json::Value::array(); a.push(0.0); a.push(0.0); a.push(0.0); return a; }();
  const PinDef vector = data("result", "", K::vector), single = data("result", "", K::f32);
  lib.add(pure("vector.make", "Make Vector", "Vector", {data("x", "X", K::f32, 0.0), data("y", "Y", K::f32, 0.0), data("z", "Z", K::f32, 0.0)}, vector, of(&P::vector_make)));
  {
    NodeDef d = make("vector.break", "Break Vector", "Vector", NodeKind::pure, "X, Y and Z of a vector.");
    d.inputs = {data("v", "Vector", K::vector, zero3)};
    d.outputs = {data("x", "X", K::f32), data("y", "Y", K::f32), data("z", "Z", K::f32)};
    d.run = [](Context& c) {
      const P::Broken b = P::vector_break(c.in_vector("v"));
      c.out("x", Value::of_number(b.x));
      c.out("y", Value::of_number(b.y));
      c.out("z", Value::of_number(b.z));
      return kStop;
    };
    lib.add(std::move(d));
  }
  const std::vector<PinDef> vab{data("a", "A", K::vector, zero3), data("b", "B", K::vector, zero3)};
  lib.add(pure("vector.add", "Vector + Vector", "Vector", vab, vector, of(&P::vector_add), "", "add plus"));
  lib.add(pure("vector.sub", "Vector - Vector", "Vector", vab, vector, of(&P::vector_sub), "", "subtract minus"));
  lib.add(pure("vector.cross", "Cross Product", "Vector", vab, vector, of(&P::vector_cross)));
  lib.add(pure("vector.scale", "Vector × Float", "Vector", {data("a", "Vector", K::vector, zero3), data("b", "Scale", K::f32, 1.0)}, vector, of(&P::vector_scale), "", "multiply scale"));
  lib.add(pure("vector.length", "Vector Length", "Vector", {data("a", "Vector", K::vector, zero3)}, single, of(&P::vector_length), "", "magnitude size"));
  lib.add(pure("vector.normalize", "Normalize", "Vector", {data("a", "Vector", K::vector, zero3)}, vector, of(&P::vector_normalize), "", "unit direction"));
  lib.add(pure("vector.distance", "Distance", "Vector", vab, single, of(&P::vector_distance)));
  lib.add(pure("vector.dot", "Dot Product", "Vector", vab, single, of(&P::vector_dot)));
  lib.add(pure("vector.lerp", "Lerp Vector", "Vector", {data("a", "A", K::vector, zero3), data("b", "B", K::vector, zero3), data("alpha", "Alpha", K::f32, 0.5)}, vector, of(&P::vector_lerp), "",
               "interpolate"));
  // Text.
  {
    NodeDef d = pure("text.append", "Append", "Text", {data("a", "A", K::text, ""), data("b", "B", K::text, ""), data("c", "C", K::text, "")}, data("result", "", K::text),
                     of(&P::text_append), "Joins texts (numbers and vectors convert)", "concat string join format");
    d.takes_first = true;
    lib.add(d);
  }
  lib.add(pure("text.equal", "Text Equals", "Text", {data("a", "A", K::text, ""), data("b", "B", K::text, "")}, boolean, of(&P::text_equal)));
  lib.add(pure("text.contains", "Contains", "Text", {data("a", "Text", K::text, ""), data("b", "Search", K::text, "")}, boolean, of(&P::text_contains)));
  lib.add(pure("text.length", "Text Length", "Text", {data("a", "Text", K::text, "")}, integer, of(&P::text_length)));
  lib.add(pure("text.code_at", "Code At", "Text", {data("a", "Text", K::text, ""), data("index", "Index", K::i64, 0)}, integer, of(&P::text_code_at),
               "The byte at Index as a number 0 to 255 (beyond the text: 0)", "char character byte ascii index"));
  lib.add(pure("text.of_code", "Text of Code", "Text", {data("code", "Code", K::i64, 65)}, data("result", "", K::text), of(&P::text_of_code),
               "A text of one byte: Code cut to 8 bits", "char character byte ascii"));
  {
    // Set Code: one byte of a text variable set where it stands - what `s[i] = c` does to a std::string (Text::set_at)
    NodeDef d = make("text.set_code", "Set Code", "Text", NodeKind::action,
                     "Sets the byte at Index of a text variable - or of a text at a place in one - to Code (cut to 8 bits), where it stands: the text keeps its "
                     "length, the other bytes stay. An index outside the text changes nothing.",
                     "char character byte ascii index replace write string");
    d.params = {param("variable", "Text", ParamDef::Kind::variable), local_of(), path_of()};
    d.inputs = {exec_in(), data("index", "Index", PinType::i64, 0), data("code", "Code", PinType::i64, 32)};
    d.outputs = {exec_out()};
    d.title = [](const NodeDef&, const Node& n) { return "Set Code in " + place_title(n); };
    d.problem = [](const Node& n, const Graph& g) -> std::string {
      Variable named;
      const Variable* v = g.place_of(n, named) ? &named : nullptr;
      if (v && v->type != PinType::text) return "needs a text variable: \"" + v->name + "\" is " + std::string(pin_type_name(v->type));
      return {};
    };
    d.run = [](Context& c) {
      const i64 index = c.in_int("index"), code = c.in_int("code");
      const std::vector<i64> at = c.path();  // (its indices, read after its inputs - as every step on a place)
      c.instance().change(c.node(), at, [&](Value& v) {
        if (v.type == PinType::text && index >= 0 && static_cast<u64>(index) < v.s.size()) v.s[static_cast<std::size_t>(index)] = static_cast<char>(static_cast<u8>(code));
      });
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    // Format Text: a format as printf has it, and a value for each of its conversions (nodes.h, format)
    NodeDef d = make("text.format", "Format Text", "Text", NodeKind::pure,
                     "A text from a format as C's printf has it: %d %u %x %o %c take an Int, %f %e %g a Float, %s a Text; flags - + space 0 #, width and "
                     "precision. Character for character what snprintf gives. Most: at most that many bytes of it (less than 0: all).");
    d.params = {param("format", "Format", ParamDef::Kind::text, "%d")};
    d.inputs = {data("most", "Most", K::i64, -1)};
    d.outputs = {data("result", "", K::text)};
    d.keywords = "format printf sprintf snprintf number to text string";
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>&) {
      std::vector<format::Arg::Kind> kinds;
      format::kinds(n.param("format", "%d"), &kinds, nullptr);
      for (std::size_t k = 0; k < kinds.size(); ++k) {
        const PinType t = kinds[k] == format::Arg::Kind::number ? K::f64 : kinds[k] == format::Arg::Kind::text ? K::text : K::i64;
        in.push_back(data("v" + std::to_string(k), std::to_string(k), t, t == K::text ? json::Value("") : json::Value(0.0)));
      }
    };
    d.title = [](const NodeDef&, const Node& n) {
      const std::string f = n.param("format", "%d");
      return "Format \"" + (f.size() > 30 ? f.substr(0, 29) + "..." : f) + "\"";
    };
    d.problem = [](const Node& n, const Graph&) {
      std::string why;
      return format::kinds(n.param("format", "%d"), nullptr, &why) ? std::string() : "the format: " + why;
    };
    d.run = [](Context& c) {
      const std::string fmt = c.param("format", "%d");
      std::vector<format::Arg::Kind> kinds;
      format::kinds(fmt, &kinds, nullptr);
      std::vector<Value> held;  // (the texts the values are read from stay here while the text is made)
      held.reserve(kinds.size());
      std::vector<format::Arg> args;
      for (std::size_t k = 0; k < kinds.size() && k + 1 < c.input_count(); ++k) {
        held.push_back(kinds[k] == format::Arg::Kind::text ? Value::text(c.in(k + 1).as_text()) : c.in(k + 1));
        const Value& v = held.back();
        args.push_back(kinds[k] == format::Arg::Kind::number ? format::arg(v.as_number()) : kinds[k] == format::Arg::Kind::text ? format::arg(std::string_view(v.s)) : format::arg(v.as_int()));
      }
      c.out(std::size_t{0}, Value::text(format::text_of(fmt, args.data(), args.size(), c.in(std::size_t{0}).as_int())));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // What std::string and std::string_view do (texts, nodes.h): a place in a text is an Int; where C++
    // gives npos - no place - the Int is -1, and a place handed in reads -1 as npos ("to the end").
    const PinDef text = data("result", "", K::text);
    struct Find { const char* type; const char* label; i64 (*f)(std::string, std::string, i64); i64 from; const char* help; const char* keywords; };
    for (const Find& x : {Find{"text.find", "Find Text", &P::text_find, 0, "Where Search first stands in Text, from From on (std::string::find) - -1: nowhere",
                               "find index search position string indexof"},
                          Find{"text.rfind", "Find Last Text", &P::text_rfind, -1, "Where Search last begins in Text, at From or before (-1: anywhere - std::string::rfind) - -1: nowhere",
                               "rfind last index search position string lastindexof"},
                          Find{"text.find_first_of", "Find First Of", &P::text_find_first_of, 0, "The first place from From on with one of the characters of Set (std::string::find_first_of) - -1: none",
                               "find first of any character set string"},
                          Find{"text.find_last_of", "Find Last Of", &P::text_find_last_of, -1, "The last place at From or before with one of the characters of Set (std::string::find_last_of) - -1: none",
                               "find last of any character set string"},
                          Find{"text.find_first_not_of", "Find First Not Of", &P::text_find_first_not_of, 0,
                               "The first place from From on with a character that is not in Set (std::string::find_first_not_of) - -1: none", "find first not of skip trim character set string"},
                          Find{"text.find_last_not_of", "Find Last Not Of", &P::text_find_last_not_of, -1,
                               "The last place at From or before with a character that is not in Set (std::string::find_last_not_of) - -1: none", "find last not of trim character set string"}}) {
      const bool set = std::string(x.type).find("_of") != std::string::npos;
      lib.add(pure(x.type, x.label, "Text", {data("a", "Text", K::text, ""), data("b", set ? "Set" : "Search", K::text, ""), data("from", "From", K::i64, static_cast<f64>(x.from))}, integer,
                   of(x.f), x.help, x.keywords));
    }
    lib.add(pure("text.sub", "Sub Text", "Text", {data("a", "Text", K::text, ""), data("at", "At", K::i64, 0), data("count", "Count", K::i64, -1)}, text, of(&P::text_sub),
                 "Count characters of Text from At on (-1 or beyond the end: to the end - std::string::substr); At beyond the end: no text", "substr substring slice part mid left right"));
    lib.add(pure("text.starts_with", "Starts With", "Text", {data("a", "Text", K::text, ""), data("b", "Start", K::text, "")}, boolean, of(&P::text_starts_with),
                 "Does Text begin with Start (std::string::starts_with)", "prefix begins string"));
    lib.add(pure("text.ends_with", "Ends With", "Text", {data("a", "Text", K::text, ""), data("b", "End", K::text, "")}, boolean, of(&P::text_ends_with),
                 "Does Text end with End (std::string::ends_with)", "suffix string"));
    lib.add(pure("text.less", "Text Less", "Text", {data("a", "A", K::text, ""), data("b", "B", K::text, "")}, boolean, of(&P::text_less),
                 "Does A come before B - byte by byte, as unsigned characters, a shorter text before a longer one it begins (std::string's <)", "< compare order sort string"));
    lib.add(pure("text.insert", "Insert Text", "Text", {data("a", "Text", K::text, ""), data("at", "At", K::i64, 0), data("b", "Insert", K::text, "")}, text, of(&P::text_insert),
                 "Text with Insert put in at At (std::string::insert); At beyond the end: Text as it is", "insert put string"));
    lib.add(pure("text.erase", "Erase Text", "Text", {data("a", "Text", K::text, ""), data("at", "At", K::i64, 0), data("count", "Count", K::i64, -1)}, text, of(&P::text_erase),
                 "Text without Count characters from At on (-1: to the end - std::string::erase); At beyond the end: Text as it is", "erase remove delete cut string"));
    lib.add(pure("text.replace", "Replace Text", "Text",
                 {data("a", "Text", K::text, ""), data("at", "At", K::i64, 0), data("count", "Count", K::i64, 0), data("b", "With", K::text, "")}, text, of(&P::text_replace),
                 "Text with Count characters from At on replaced by With (std::string::replace); At beyond the end: Text as it is", "replace string"));
    lib.add(pure("text.repeat", "Repeat Text", "Text", {data("a", "Text", K::text, "*"), data("count", "Count", K::i64, 3)}, text, of(&P::text_repeat),
                 "Text so many times, one after the other (std::string(count, c) of a character); Count 0 or less: no text", "repeat times fill padding string"));
    lib.add(pure("text.bytes", "Bytes of Text", "Text", {data("a", "Text", K::text, "")}, bytes_out("result", ""), of(&P::text_bytes),
                 "The bytes of the text, each a whole number 0 to 255 (a std::vector<u8> of its range)", "bytes chars utf8 encode array"));
    lib.add(pure("text.reserve", "Reserve Text", "Text", {data("a", "Text", K::text, ""), data("count", "Count", K::i64, 0)}, text, of(&P::text_reserve),
                 "The text as it is - in a Blueprint's class with room for Count characters, so that what is appended to it fits (std::string::reserve)",
                 "reserve capacity room string"));
    lib.add(pure("text.of_bytes", "Text of Bytes", "Text", {any_ints("bytes", "Bytes"), data("at", "At", K::i64, 0), data("count", "Count", K::i64, 0)}, text,
                 [](Context& c) {
                   const Value bytes = c.in(std::size_t{0});
                   const i64 at = c.in(std::size_t{1}).as_int(), count = c.in(std::size_t{2}).as_int();
                   std::string r;
                   if (is_list(bytes.type) && is_integer(element_of(bytes.type)))
                     with_number(element_of(bytes.type), [&](auto t) {
                       using T = typename decltype(t)::type;
                       if constexpr (num::Integer<T>) r = P::text_of_bytes<T>(bytes.items<T>(), at, count);
                     });
                   return Value::text(std::move(r));
                 }, "Count bytes of the list from At on as a text, each cut to 8 bits (beyond the list: up to its end)",
                 "bytes chars string decode utf8 buffer"));
    // Numbers read from a text at a place, as C reads them (scan, nodes.h): Value, and End - the place just
    // after what was read (nothing read: At itself, as C's end pointer)
    // (strtod: an f64, strtof: an f32, strtoll: an i64, strtoull: a u64)
    auto parse = [&](const char* type, const char* label, PinType t, bool base, auto f, const char* help, const char* keywords) {
      NodeDef d = make(type, label, "Text", NodeKind::pure, help);
      d.inputs = {data("a", "Text", K::text, ""), data("at", "At", K::i64, 0)};
      if (base) d.inputs.push_back(data("base", "Base", K::i64, 10));
      d.outputs = {data("value", "Value", t), data("end", "End", K::i64)};
      d.keywords = keywords;
      d.run = [f](Context& c) {
        const auto r = f(c);
        c.out("value", Value::of_number(r.value));
        c.out("end", Value::integer(r.end));
        return kStop;
      };
      lib.add(std::move(d));
    };
    parse("text.parse_float", "Parse Float", K::f64, false, [](Context& c) { return P::text_parse_float(c.in(std::size_t{0}).as_text(), c.in(std::size_t{1}).as_int()); },
          "The number at At of Text, as C's strtod reads it (atof)", "strtod atof parse number float double read");
    parse("text.parse_float32", "Parse Float 32", K::f32, false, [](Context& c) { return P::text_parse_float32(c.in(std::size_t{0}).as_text(), c.in(std::size_t{1}).as_int()); },
          "The number at At of Text as an f32, as C's strtof reads it", "strtof parse number float single read");
    parse("text.parse_int", "Parse Int", K::i64, true, [](Context& c) { return P::text_parse_int(c.in(std::size_t{0}).as_text(), c.in(std::size_t{1}).as_int(), c.in(std::size_t{2}).as_int()); },
          "The whole number at At of Text, as C's strtoll reads it in Base (0: by its prefix - 0x, 0b, 0) (atoi, atol, strtol)", "strtol strtoll atoi atol parse integer read");
    parse("text.parse_uint", "Parse Int Unsigned", K::u64, true, [](Context& c) { return P::text_parse_uint(c.in(std::size_t{0}).as_text(), c.in(std::size_t{1}).as_int(), c.in(std::size_t{2}).as_int()); },
          "The whole number at At of Text, as C's strtoull reads it in Base (0: by its prefix - 0x, 0b, 0) (strtoul)", "strtoul strtoull parse unsigned integer read");
  }
  {
    // Scan Text: what sscanf reads with a format (scan, nodes.h) - Count, what sscanf gives, and a value
    // for each conversion; the inputs Before hold what was there before, which stays where the scan
    // does not reach (as a variable sscanf did not write keeps its value)
    NodeDef d = make("text.scan", "Scan Text", "Text", NodeKind::action,
                     "Reads Text from At on as C's sscanf reads it with a format: %d %i %u %o %x %b take whole numbers (l ll: 64 bits), %f %e %g %a a float (l: a double), %s and "
                     "%[set] a word, %n how many characters were read; a width, * (read, not given). Count: how many values were read (-1: the text ended before the first).");
    d.params = {param("format", "Format", ParamDef::Kind::text, "%d")};
    d.inputs = {exec_in(), data("a", "Text", K::text, ""), data("at", "At", K::i64, 0)};
    d.outputs = {exec_out(), data("count", "Count", K::i64)};
    d.keywords = "scan sscanf scanf parse read numbers words format";
    auto pin_of = [](scan::Kind k) { return k == scan::Kind::whole ? K::i64 : k == scan::Kind::whole64 ? K::u64 : k == scan::Kind::number ? K::f64 : K::text; };
    d.shape = [pin_of](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      scan::Plan plan;
      if (!scan::plan(n.param("format", "%d"), &plan, nullptr)) return;
      for (std::size_t k = 0; k < plan.count; ++k) {
        const PinType t = pin_of(plan.kinds[k]);
        in.push_back(data("p" + std::to_string(k), "Before " + std::to_string(k), t, t == K::text ? json::Value("") : t == K::u64 ? json::Value("0") : json::Value(0.0)));
        out.push_back(data("v" + std::to_string(k), std::to_string(k), t));
      }
    };
    d.title = [](const NodeDef&, const Node& n) {
      const std::string f = n.param("format", "%d");
      return "Scan \"" + (f.size() > 30 ? f.substr(0, 29) + "..." : f) + "\"";
    };
    d.problem = [](const Node& n, const Graph&) {
      const char* why = "";
      return scan::plan(n.param("format", "%d"), nullptr, &why) ? std::string() : std::string("the format: ") + why;
    };
    d.run = [](Context& c) {
      const std::string fmt = c.param("format", "%d");
      scan::Plan plan;
      if (!scan::plan(fmt, &plan, nullptr)) plan = {};
      scan::Slot slots[scan::kMostSlots];
      for (std::size_t k = 0; k < plan.count; ++k) {
        scan::Slot& s = slots[k];
        s.kind = plan.kinds[k];
        const Value v = c.in("p" + std::to_string(k));
        if (s.kind == scan::Kind::whole) s.whole = v.as_int();
        else if (s.kind == scan::Kind::whole64) s.bits = v.type == PinType::u64 ? v.bits() : 0;
        else if (s.kind == scan::Kind::number) s.number = v.as_number();
        else s.text = v.as_text();
      }
      const std::string a = c.in_text("a");
      const i64 count = scan::run(scan::c_string(a, c.in_int("at")), fmt, slots, plan.count);
      c.out("count", Value::integer(count));
      for (std::size_t k = 0; k < plan.count; ++k) {
        const scan::Slot& s = slots[k];
        const std::string pin = "v" + std::to_string(k);
        c.out(pin, s.kind == scan::Kind::whole ? Value::integer(s.whole) : s.kind == scan::Kind::whole64 ? Value::of_number(s.bits) : s.kind == scan::Kind::number ? Value::number(s.number)
                                                                                                                                 : Value::text(s.text));
      }
      return 0;
    };
    lib.add(std::move(d));
  }
  // Time.
  lib.add(worldly(pure("time.now", "Game Time", "Utilities", {}, data("result", "Seconds", K::f64), [](Context& c) { return Value::number(c.host().time()); }, "Seconds since play started", "clock elapsed")));
  {
    NodeDef d = make("debug.print", "Print", "Utilities", NodeKind::action, "Writes text to the log (and on screen).");
    d.world = true;
    d.inputs = {exec_in(), data("text", "Text", K::text, "Hello"), data("screen", "On Screen", K::boolean, true)};
    d.outputs = {exec_out()};
    d.keywords = "log debug message string";
    d.run = [](Context& c) {
      c.host().print(c.in_text("text"), c.in_bool("screen"));
      return 0;
    };
    lib.add(std::move(d));
  }
}

// A formula as one node: every name in it is an input pin, of the type its setting "Types" names (logic.h, Formula).
void add_formula(Library& lib) {
  NodeDef d = make("math.formula", "Formula", "Math", NodeKind::pure,
                   "A formula as one node, in C++: every name in it becomes an input, of the type Types names for it (in the order the names first "
                   "appear). Each operation is the node's of its type - nothing converts but num_cast<To, From>(x). Numbers: 0.5f an f32, 0.5 an "
                   "f64, a whole number the type of what it meets. Operators + - * / % << >> & | ^ ~, comparisons (a bool), && || !, c ? a : b; "
                   "functions min max abs clamp lerp sqrt floor ceil trunc round sin cos tan asin acos atan atan2 exp expm1 log log10 pow hypot sinh cosh tanh asinh "
                   "copysign fmin fmax fmod isnan isinf isfinite signbit.");
  d.params = {param("formula", "Formula", ParamDef::Kind::text, "a + b"), param("types", "Types", ParamDef::Kind::text, "f32 f32")};
  d.outputs = {data("result", "", PinType::f32)};
  d.keywords = "math expression formula equation calculate compute";
  d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>& out) {
    const std::string text = n.param("formula", "a + b"), types = n.param("types", "f32 f32");
    const Formula& f = Formula::cached(text, types);
    for (std::size_t i = 0; i < f.inputs().size(); ++i) {
      const PinType t = i < f.types().size() ? pin_type_of(f.types()[i]) : PinType::f32;
      in.push_back(data(f.inputs()[i], f.inputs()[i], t, Value::zero(t).to_json()));
    }
    out[0].type = pin_type_of(f.result());
  };
  d.title = [](const NodeDef&, const Node& n) {
    const std::string f = n.param("formula", "a + b");
    return f.size() > 44 ? f.substr(0, 43) + "..." : f;
  };
  d.problem = [](const Node& n, const Graph&) {
    const std::string text = n.param("formula", "a + b"), types = n.param("types", "f32 f32");
    return Formula::cached(text, types).error();
  };
  d.run = [](Context& c) {
    const std::string text = c.param("formula", "a + b"), types = c.param("types", "f32 f32");
    const Formula& f = Formula::cached(text, types);
    u64 bits[Formula::kMaxInputs] = {};
    const std::size_t count = min_of(f.inputs().size(), c.input_count());
    for (std::size_t i = 0; i < count; ++i) bits[i] = c.in(i).bits();
    c.out(std::size_t{0}, Value::of_bits(pin_type_of(f.result()), f.evaluate(bits, count)));
    return kStop;
  };
  lib.add(std::move(d));
}

// Functions: named, with typed inputs and outputs (Graph::functions). The four nodes take their
// data pins from the function they name; a pin's id is its name.
/// The setting of a node that names an interface of the engine (Engine Object of Blueprint, Is Valid Object, Handle of
/// Object): one of the engine's interfaces, written as its C++ type - func_engine_object<kern::App>, object_valid<kern::App>.
/// Its default is said on the node (App); which one it is, the node shows.
ParamDef interface_param() {
  std::vector<std::string> interfaces;
  for (const EngineInterface& i : engine_interfaces()) {
    interfaces.push_back(i.name);
    interfaces.push_back(i.name);
  }
  ParamDef p = param("interface", "Interface", ParamDef::Kind::choice, "App", std::move(interfaces));
  p.engine_class = true;
  return p;
}
std::string interface_problem(const Node& n, const Graph&) {
  const std::string i = n.param("interface", "App");
  if (i.empty()) return "names no interface of the engine (its setting Interface: App, ShaderExtension ...)";
  if (!engine_interface(i)) return "the engine has no interface " + i;
  return {};
}

/// The setting of a node that works on an Object of any class of the engine (Is Valid Object, Handle of Object, Same
/// Object): the class, written as its C++ type - object_valid<kern::audio::Mixer>. Its default is said on the node (App).
ParamDef class_param() {
  std::vector<std::string> classes;
  for (const EngineClass& c : engine_classes()) {
    classes.push_back(c.name);
    classes.push_back(c.name);
  }
  ParamDef p = param("class", "Class", ParamDef::Kind::choice, "App", std::move(classes));
  p.engine_class = true;
  return p;
}
std::string class_problem(const Node& n, const Graph&) {
  const std::string c = n.param("class", "App");
  if (c.empty()) return "names no class of the engine (its setting Class: App, audio_Mixer ...)";
  if (!engine_class(c)) return "the engine has no class " + c;
  return {};
}

void add_functions(Library& lib) {
  // which == 0: the inputs as outputs (entry); 1: the outputs as inputs (return); 2: both (calls)
  auto shape = [](int which) {
    return [which](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const Function* f = g.function(n.param("function"));
      if (!f) return;
      // (a reference input has no pin: in the function it is named like a variable, and a call
      // names the place it is - with an index input for every list entry of that place's path)
      if (which == 0 || which == 2)
        for (const FunctionPin& p : f->inputs) {
          if (!p.ref) {
            (which == 0 ? out : in).push_back({p.name, p.name, p.type, p.value, false, p.of, p.count});
            continue;
          }
          if (which != 2) continue;
          const std::size_t entries = path_indices(n.param(ref_key(p.name, "path")));
          for (std::size_t k = 0; k < entries; ++k)
            in.push_back({ref_key(p.name, path_pin(k)), p.name + (entries > 1 ? " index " + std::to_string(k + 1) : std::string(" index")), PinType::i64, json::Value(0), false, {}});
        }
      if (which == 1 || which == 2)
        for (const FunctionPin& p : f->outputs) (which == 1 ? in : out).push_back({p.name, p.name, p.type, p.value, false, p.of, p.count});
    };
  };
  auto named = [](const char* prefix) {
    return [prefix](const NodeDef&, const Node& n) { return std::string(prefix) + n.param("function", "?"); };
  };
  // Runs the function with the node's inputs and puts its results on the node's outputs.
  auto call = [](Context& c) {
    const Function* f = c.instance().graph().function(c.param("function"));
    if (!f) return;
    std::vector<Value> args, results;
    std::vector<Instance::Spot> places(f->inputs.size());
    args.reserve(f->inputs.size());
    for (std::size_t k = 0; k < f->inputs.size(); ++k) {
      const FunctionPin& p = f->inputs[k];
      if (p.ref) places[k] = c.spot(ref_key(p.name, ""));  // (the place the call names for it, as it is now)
      args.push_back(p.ref ? Value() : c.in(p.name));
    }
    c.instance().call_inside(f->name, args, results, places);
    for (std::size_t k = 0; k < f->outputs.size() && k < results.size(); ++k) c.out(f->outputs[k].name, results[k]);
  };
  {
    NodeDef d = make("func.entry", "Function Entry", "Functions", NodeKind::event,
                     "Where a function starts: its inputs come out here. Every function has exactly one. Functions are made in My Blueprint.");
    d.params = {param("function", "Function", ParamDef::Kind::function)};
    d.outputs = {exec_out()};
    d.shape = shape(0);
    d.title = named("Function ");
    d.color = 0x6d3fc4;
    d.event = "logic.function";  // (never raised: a function starts when it is called)
    d.keywords = "function begin start input parameter argument";
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("func.return", "Return", "Functions", NodeKind::action,
                     "Ends the function and gives its outputs to the caller. Loops and sequences of the function stop here too. Outputs no Return set keep their defaults.");
    d.params = {param("function", "Function", ParamDef::Kind::function)};
    d.inputs = {exec_in()};
    d.shape = shape(1);
    d.color = 0x6d3fc4;
    d.keywords = "return result output end exit function";
    d.run = [](Context& c) {
      const Function* f = c.instance().graph().function(c.param("function"));
      std::vector<std::pair<std::string, Value>> values;
      if (f)
        for (const FunctionPin& p : f->outputs) values.emplace_back(p.name, p.count ? fitted(c.in(p.name), declared(p), c.instance().graph()) : c.in(p.name));
      c.instance().return_values(c.param("function"), values);
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("func.call", "Call Function", "Functions", NodeKind::action,
                     "Runs a function of this Blueprint to its end, then continues. Its results stay on the outputs until the next call.");
    d.params = {param("function", "Function", ParamDef::Kind::function), local_of()};  // ("scope": the function the call stands in - where the places it names are looked up)
    d.inputs = {exec_in()};
    d.outputs = {exec_out()};
    d.shape = shape(2);
    d.title = named("");
    d.color = 0x2f6fd0;
    d.keywords = "call run invoke function";
    d.run = [call](Context& c) {
      call(c);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("func.call_on", "Call on Actor", "Functions", NodeKind::action,
                     "Runs a function of another actor's Blueprint to its end, then continues: Target's, or the Level's when Target is empty. This Blueprint says what the "
                     "function looks like (My Blueprint: a function \"of another Blueprint\", or one of its own with the same name and pins). Where the target has no such "
                     "function, the outputs are their defaults.");
    d.world = true;
    d.params = {param("function", "Function", ParamDef::Kind::function)};
    d.inputs = {exec_in(), data("this", "Target", PinType::actor, 0)};  // ("this": a word of C++ - no pin of a function has that name)
    d.outputs = {exec_out()};
    d.shape = shape(2);
    d.title = [](const NodeDef&, const Node& n) { return n.param("function", "?") + " on Actor"; };
    d.raises = true;
    d.color = 0x2f6fd0;
    d.keywords = "call run invoke function other blueprint actor library remote interface message";
    d.run = [](Context& c) {
      const Function* f = c.instance().graph().function(c.param("function"));
      if (!f) return 0;
      const ActorHandle target = c.in_actor("this");
      std::vector<Value> args, results;
      args.reserve(f->inputs.size());
      for (const FunctionPin& p : f->inputs) args.push_back(c.in(p.name));
      c.runtime().call_on(c.instance(), f->name, target, args, results);
      for (std::size_t k = 0; k < f->outputs.size() && k < results.size(); ++k) c.out(f->outputs[k].name, results[k]);
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    // Get Variable on Actor: what a variable of another actor's Blueprint holds now (its type is the setting Type)
    NodeDef d = make("func.get_on", "Get Variable on Actor", "Functions", NodeKind::pure,
                     "What the variable of the name holds in the Blueprint of Target (the Level's when Target is empty), as a value of the type the setting says. Where Target has no such "
                     "variable of that type: the zero of the type.");
    d.world = true;
    d.params = {param("variable", "Variable", ParamDef::Kind::text, ""), type_param(true)};
    d.inputs = {data("this", "Target", PinType::actor, 0)};
    d.outputs = {data("value", "Value", PinType::f64)};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>&, std::vector<PinDef>& out) { retype(out[0], setting_type(n, g, true), g); };
    d.title = [](const NodeDef&, const Node& n) { return n.param("variable", "?") + " of Actor"; };
    d.keywords = "get read variable other blueprint actor remote";
    d.run = [](Context& c) {
      const TypeRef t{c.output(0).type, c.output(0).of};
      c.out(std::size_t{0}, c.runtime().get_on(c.instance(), c.param("variable"), c.in_actor("this"), t));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // Has Blueprint: whether Call on Actor and Get Variable on Actor reach a Blueprint on Target
    NodeDef d = make("func.has_blueprint", "Has Blueprint", "Functions", NodeKind::pure,
                     "True when Target (the Level when Target is empty) has a Blueprint that runs: one without errors that has not been stopped. Call on Actor and Get "
                     "Variable on Actor reach it; where it has none, they give their defaults.");
    d.world = true;
    d.inputs = {data("target", "Target", PinType::actor, 0)};
    d.outputs = {data("result", "Has Blueprint", PinType::boolean)};
    d.keywords = "has blueprint logic running valid other actor remote bound";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::boolean(c.runtime().has_blueprint(c.instance(), c.in_actor("target"))));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // Engine Object of Blueprint: the object of the engine another Blueprint is (Graph::implements), as the interface
    // its setting names - an Object<I> (kern/docs/engine_object.md)
    NodeDef d = make("func.engine_object", "Engine Object of Blueprint", "Functions", NodeKind::pure,
                     "The object of the engine the Blueprint on Target (the Level when Target is empty) is, as the interface of the "
                     "engine named in Interface (App, Shader Extension ...): what a function of the engine takes where it is handed "
                     "such an object (an App's Extensions). The same object every time. Where there is none - Target has no Blueprint "
                     "that runs, it implements no interface or another one, the host has no objects of the engine, this build lacks "
                     "the interface - the log says which, and it gives no object.");
    d.world = true;
    d.params = {interface_param()};
    d.inputs = {data("target", "Target", PinType::actor, 0)};
    d.outputs = {data("result", "Engine Object", PinType::object)};
    d.keywords = "engine object interface implements implementation handle extension app other actor";
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>&, std::vector<PinDef>& out) { out[0].of = n.param("interface", "App"); };   // (unset: its default, as the node shows it)
    d.problem = interface_problem;
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::object(c.runtime().engine_object(c.instance(), c.in_actor("target"), c.param("interface"))));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("object.valid", "Is Valid Object", "Engine", NodeKind::pure,
                     "Does it refer to an object of the engine (and not to none)? Whether that object is still there, the engine "
                     "says where it is used.");
    d.params = {class_param()};
    d.inputs = {data("object", "Object", PinType::object)};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>&) { in[0].of = n.param("class", "App"); };   // (unset: its default, as the node shows it)
    d.problem = class_problem;
    d.outputs = {data("result", "Is Valid", PinType::boolean)};
    d.keywords = "object valid none engine handle null pointer";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::boolean(c.in("object").as_object().valid()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // Same Object: do two Objects refer to the same object of the engine - the same world and the same handle (none and
    // none are the same; C++ compares two pointers so)
    NodeDef d = make("object.equal", "Same Object", "Engine", NodeKind::pure,
                     "Do A and B refer to the same object of the engine (or both to none)? An object of another world is never the same.");
    d.params = {class_param()};
    d.inputs = {data("a", "A", PinType::object), data("b", "B", PinType::object)};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>&) { in[0].of = in[1].of = n.param("class", "App"); };
    d.problem = class_problem;
    d.outputs = {data("result", "Same", PinType::boolean)};
    d.keywords = "object same equal identical == engine handle pointer";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::boolean(c.in("a").as_object() == c.in("b").as_object()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("object.handle", "Handle of Object", "Engine", NodeKind::pure,
                     "The handle of an object of the engine in its world, as an Int (0: it refers to none) - to show it or to count "
                     "with it; no function of the engine takes one (they take Objects, kern/docs/engine_object.md). There is no way "
                     "back: no Int becomes an Object.");
    d.params = {class_param()};
    d.inputs = {data("object", "Object", PinType::object)};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>&) { in[0].of = n.param("class", "App"); };   // (unset: its default, as the node shows it)
    d.problem = class_problem;
    d.outputs = {data("result", "Handle", PinType::i64)};
    d.keywords = "object handle int engine";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::integer(c.in("object").as_object().handle));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // Same Actor: do two Actors refer to the same actor - the same world and the same id (none and none are the same)
    NodeDef d = make("actor.equal", "Same Actor", "Actors", NodeKind::pure,
                     "Do A and B refer to the same actor (or both to none)? An actor of another world is never the same, even with the same id.");
    d.inputs = {data("a", "A", PinType::actor), data("b", "B", PinType::actor)};
    d.outputs = {data("result", "Same", PinType::boolean)};
    d.keywords = "actor same equal identical == compare";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::boolean(c.in("a").as_actor() == c.in("b").as_actor()));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("actor.id", "Id of Actor", "Actors", NodeKind::pure,
                     "The id of an actor in its world, as an Int (0: it refers to none) - to show it or to count with it. There is "
                     "no way back: no Int becomes an Actor (an actor comes from its world: Self, Find Actor, Spawn Actor, an event).");
    d.inputs = {data("actor", "Actor", PinType::actor)};
    d.outputs = {data("result", "Id", PinType::i64)};
    d.keywords = "actor id number int handle";
    d.run = [](Context& c) {
      c.out(std::size_t{0}, Value::integer(c.in("actor").as_actor().id));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("func.pure", "Function Value", "Functions", NodeKind::pure,
                     "A function as a value: it runs whenever one of its outputs is read. For functions that only compute (no effects on the world).");
    d.params = {param("function", "Function", ParamDef::Kind::function)};
    d.shape = shape(2);
    d.title = named("");
    d.keywords = "call pure function value compute";
    d.run = [call](Context& c) {
      call(c);
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    // Make Callback: a function of a library as a value that only the engine calls (a height lookup, a test a
    // builder asks) - with the place its first input names, where that is a const reference
    NodeDef d = make("func.callback", "Make Callback", "Functions", NodeKind::pure,
                     "A function of a library as a value the engine calls - while one of its nodes runs, or later, while the object of the engine it was "
                     "given to lives. Its first input, where that is a const reference, is bound to the place this node names (read when the engine calls). "
                     "The function needs nothing of the world: it only computes. A Blueprint does not call a Callback itself.");
    d.params = {param("function", "Function", ParamDef::Kind::function), local_of()};
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const Function* f = g.function(n.param("function"));
      out.push_back({"callback", "Callback", PinType::callback, {}, false, f ? callback_of(*f) : std::string("()")});
      if (!f || f->inputs.empty() || !f->inputs[0].ref) return;
      const FunctionPin& p = f->inputs[0];
      const std::size_t entries = path_indices(n.param(ref_key(p.name, "path")));
      for (std::size_t k = 0; k < entries; ++k)
        in.push_back({ref_key(p.name, path_pin(k)), p.name + (entries > 1 ? " index " + std::to_string(k + 1) : std::string(" index")), PinType::i64, json::Value(0), false, {}});
    };
    d.title = [](const NodeDef&, const Node& n) { return "Callback " + n.param("function", "?"); };
    d.color = 0x6d3fc4;
    d.keywords = "callback function value engine calls sampler handler";
    d.run = [](Context& c) {
      const Function* f = c.instance().graph().function(c.param("function"));
      if (!f) return kStop;
      Instance::Spot place;
      const bool bound = !f->inputs.empty() && f->inputs[0].ref;
      if (bound) place = c.spot(ref_key(f->inputs[0].name, ""));  // (the place, as the node names it now)
      c.out(std::size_t{0}, Value::callback(c.instance().callable(f->name, bound, std::move(place))));
      return kStop;
    };
    lib.add(std::move(d));
  }
}

}  // namespace

Library& Library::core() {
  static Library lib;
  static bool ready = false;
  if (!ready) {
    ready = true;
    add_events(lib);
    add_flow(lib);
    add_variables(lib);
    add_lists(lib);
    add_maps(lib);
    add_enums(lib);
    add_structs(lib);
    add_math(lib);
    add_formula(lib);
    add_functions(lib);
    // Every node of the core library is a function of blueprint.h (BlueprintCore) or a form of the
    // notation; the native Blueprint test compiles each of them.
    add_engine_nodes(lib);   // (the engine's functions: kern/logic/engine_api.h)
    for (const auto& d : lib.defs_) d->native = true;
  }
  return lib;
}

}  // namespace kern::logic
