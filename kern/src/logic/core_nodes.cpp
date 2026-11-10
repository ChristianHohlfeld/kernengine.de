// Kern Logic: the built-in nodes (events, flow control, variables, state
// machines, functions, math, logic, vectors, text, debugging). Gameplay nodes live in
// the game module (src/game/logic_nodes.cpp).
#include <cmath>
#include <type_traits>
#include <utility>

#include "kern/core/dmath.h"
#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

namespace {

PinDef exec_in(std::string id = "exec", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef exec_out(std::string id = "then", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef data(std::string id, std::string label, PinType t, json::Value def = {}) { return {std::move(id), std::move(label), t, std::move(def)}; }

NodeDef make(std::string type, std::string label, std::string category, NodeKind kind, std::string help) {
  NodeDef d;
  d.type = std::move(type);
  d.label = std::move(label);
  d.category = std::move(category);
  d.kind = kind;
  d.help = std::move(help);
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

/// A value that is new on every read (Random): the node keeps its sequence.
NodeDef fresh(NodeDef d) {
  d.memory = true;
  return d;
}

f64 random01(Context& c) { return static_cast<f64>(c.host().random() >> 8) / static_cast<f64>(1u << 24); }

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
    NodeDef d = make("event.tick", "Event Tick", "Events", NodeKind::event, "Runs every simulation step. Delta is the step in seconds.");
    d.outputs = {exec_out(), data("delta", "Delta Seconds", PinType::number)};
    d.event = "logic.tick";
    d.color = 0xb4232f;
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("event.every", "Every N Seconds", "Events", NodeKind::event, "Runs repeatedly with a fixed interval (a looping timer).");
    d.params = {param("seconds", "Interval (s)", ParamDef::Kind::text, "1")};
    d.outputs = {exec_out(), data("count", "Count", PinType::integer)};
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
    d.outputs = {exec_out(), data("actor", "Actor", PinType::actor), data("value", "Value", PinType::number), data("text", "Text", PinType::text)};
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
    d.params = {param("type", "Type", ParamDef::Kind::choice, "float",
                      {"bool", "Boolean", "int", "Integer", "float", "Float", "vector", "Vector", "text", "Text", "actor", "Actor"})};
    d.keywords = "knot reroute wire";
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      PinType t = PinType::number;
      if (!pin_type_from_name(n.param("type", "float"), t) || t == PinType::exec) t = PinType::number;
      in = {data("in", "", t, Value::zero(t).to_json())};
      out = {data("out", "", t)};
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
    d.inputs = {exec_in(), data("first", "First Index", PinType::integer, 0), data("last", "Last Index", PinType::integer, 9)};
    d.outputs = {exec_out("body", "Loop Body"), data("index", "Index", PinType::integer), exec_out("completed", "Completed")};
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
      if (!flow::for_runs(m)) return c.exec("completed");
      c.out("index", Value::integer(flow::for_index(m)));
      c.loop_back(Context::kResume);
      return c.exec("body");
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.while", "While Loop", "Flow", NodeKind::flow, "Runs Loop Body while the condition is true (checked before every pass).");
    d.inputs = {exec_in(), data("condition", "Condition", PinType::boolean, false)};
    d.outputs = {exec_out("body", "Loop Body"), exec_out("completed", "Completed")};
    d.run = [](Context& c) {
      if (!c.in_bool("condition")) return c.exec("completed");
      c.loop_back(Context::kResume);
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
    d.inputs = {exec_in(), data("n", "N", PinType::integer, 3), exec_in("reset", "Reset")};
    d.outputs = {exec_out("exit", "Exit"), data("counter", "Counter", PinType::integer)};
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
    d.inputs = {exec_in(), data("duration", "Duration", PinType::number, 1)};
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
    d.inputs = {exec_in(), data("duration", "Duration", PinType::number, 1)};
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
    d.inputs = {exec_in("play", "Play"), exec_in("stop", "Stop"), data("duration", "Duration", PinType::number, 1)};
    d.params = {param("ease", "Easing", ParamDef::Kind::choice, "smooth", {"linear", "Linear", "smooth", "Smooth", "in", "Ease in", "out", "Ease out"})};
    d.outputs = {exec_out("update", "Update"), exec_out("finished", "Finished"), data("alpha", "Alpha", PinType::number)};
    d.keywords = "timeline lerp interpolate animate over time";
    d.run = [](Context& c) {
      f64* m = &c.slot(0);
      if (c.entry() == 1) {
        c.cancel_waits();
        flow::tween_stop(m);
        return kStop;
      }
      const f64 now = c.host().time();
      if (c.entry() == 0) {
        c.cancel_waits();
        flow::tween_play(m, now, c.in_number("duration"));
      }
      if (!flow::tween_runs(m)) return kStop;
      bool finished = false;
      c.out("alpha", Value::number(flow::tween_step(m, now, c.param("ease", "smooth"), finished)));
      if (finished) c.then(c.exec("finished"));  // (after Update)
      else c.wait(0, Context::kResume);          // again next step
      return c.exec("update");
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("flow.switch_int", "Switch on Int", "Flow", NodeKind::flow, "Continues at the output matching Selection, or Default.");
    d.inputs = {exec_in(), data("selection", "Selection", PinType::integer, 0)};
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
    d.inputs = {exec_in(), data("target", "Target", PinType::actor, 0), data("actor", "Actor", PinType::actor, 0), data("value", "Value", PinType::number, 0),
                data("text", "Text", PinType::text, "")};
    d.params = {param("name", "Event", ParamDef::Kind::event, "MyEvent")};
    d.outputs = {exec_out()};
    d.keywords = "broadcast dispatch message send trigger";
    d.title = [](const NodeDef&, const Node& n) { return "Call " + n.param("name", "MyEvent"); };
    d.run = [](Context& c) {
      std::vector<std::pair<std::string, Value>> args{{"actor", c.in("actor")}, {"value", c.in("value")}, {"text", c.in("text")}};
      c.runtime().call(c.param("name", "MyEvent"), c.in_actor("target"), std::move(args));
      return 0;
    };
    lib.add(std::move(d));
  }
}

void add_variables(Library& lib) {
  auto var_shape = [](bool input, bool output) {
    return [input, output](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const Variable* v = g.variable(n.param("variable"));
      const PinType t = v ? v->type : PinType::number;
      for (PinDef& p : in) if (p.id == "value" && input) { p.type = t; p.def = v ? v->value : json::Value(0.0); }
      for (PinDef& p : out) if (p.id == "value" && output) p.type = t;
    };
  };
  {
    NodeDef d = make("var.get", "Get Variable", "Variables", NodeKind::pure, "Reads a variable of this graph.");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable)};
    d.outputs = {data("value", "Value", PinType::number)};
    d.shape = var_shape(false, true);
    d.title = [](const NodeDef&, const Node& n) { return n.param("variable", "?"); };
    d.run = [](Context& c) {
      c.out(std::size_t{0}, c.instance().get(c.param("variable")));
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("var.set", "Set Variable", "Variables", NodeKind::action, "Writes a variable of this graph.");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable)};
    d.inputs = {exec_in(), data("value", "Value", PinType::number)};
    d.outputs = {exec_out(), data("value", "Value", PinType::number)};
    d.shape = var_shape(true, true);
    d.title = [](const NodeDef&, const Node& n) { return "Set " + n.param("variable", "?"); };
    d.run = [](Context& c) {
      const std::string name = c.param("variable");
      c.instance().set(name, c.in("value"));
      c.out("value", c.instance().get(name));
      return 0;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("var.add", "Add to Variable", "Variables", NodeKind::action, "Adds Amount to a number variable, Integer or Float (use -1 to count down).");
    d.params = {param("variable", "Variable", ParamDef::Kind::variable)};
    d.inputs = {exec_in(), data("amount", "Amount", PinType::number, 1.0)};
    d.outputs = {exec_out(), data("value", "Value", PinType::number)};
    d.keywords = "increment decrement counter plus";
    d.title = [](const NodeDef&, const Node& n) { return "Add to " + n.param("variable", "?"); };
    // The amount and the result have the variable's type: whole numbers add as whole numbers.
    d.shape = [](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const Variable* v = g.variable(n.param("variable"));
      if (!v || v->type != PinType::integer) return;
      for (PinDef& p : in) if (p.id == "amount") { p.type = PinType::integer; p.def = json::Value(1); }
      for (PinDef& p : out) if (p.id == "value") p.type = PinType::integer;
    };
    d.problem = [](const Node& n, const Graph& g) {
      const Variable* v = g.variable(n.param("variable"));
      return v && v->type != PinType::integer && v->type != PinType::number
                 ? "needs a number variable (Integer or Float): \"" + v->name + "\" is " + std::string(pin_type_name(v->type)) + " - use Set"
                 : std::string();
    };
    d.run = [](Context& c) {
      const std::string name = c.param("variable");
      const Value cur = c.instance().get(name);
      c.instance().set(name, Value::number(cur.as_number() + c.in_number("amount")));
      c.out("value", c.instance().get(name));
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

void add_math(Library& lib) {
  using K = PinType;
  using P = PureNodes;
  auto ab = [](K t, json::Value a = 0.0, json::Value b = 0.0) { return std::vector<PinDef>{data("a", "A", t, a), data("b", "B", t, b)}; };
  const PinDef number = data("result", "", K::number), integer = data("result", "", K::integer), boolean = data("result", "", K::boolean);
  lib.add(pure("math.add", "Add", "Math", ab(K::number), number, of(&P::math_add), "A + B", "+ plus sum"));
  lib.add(pure("math.sub", "Subtract", "Math", ab(K::number), number, of(&P::math_sub), "A - B", "- minus"));
  lib.add(pure("math.mul", "Multiply", "Math", ab(K::number, 1.0, 1.0), number, of(&P::math_mul), "A × B", "* times"));
  lib.add(pure("math.div", "Divide", "Math", ab(K::number, 1.0, 1.0), number, of(&P::math_div), "A ÷ B (0 when B is 0)", "/"));
  lib.add(pure("math.mod", "Modulo", "Math", ab(K::number, 0.0, 2.0), number, of(&P::math_mod), "Remainder of A ÷ B", "% remainder"));
  lib.add(pure("math.min", "Min", "Math", ab(K::number), number, of(&P::math_min)));
  lib.add(pure("math.max", "Max", "Math", ab(K::number), number, of(&P::math_max)));
  lib.add(pure("math.clamp", "Clamp", "Math", {data("value", "Value", K::number, 0.0), data("min", "Min", K::number, 0.0), data("max", "Max", K::number, 1.0)}, number, of(&P::math_clamp)));
  lib.add(pure("math.abs", "Absolute", "Math", {data("a", "A", K::number, 0.0)}, number, of(&P::math_abs)));
  lib.add(pure("math.negate", "Negate", "Math", {data("a", "A", K::number, 0.0)}, number, of(&P::math_negate)));
  lib.add(pure("math.lerp", "Lerp", "Math", {data("a", "A", K::number, 0.0), data("b", "B", K::number, 1.0), data("alpha", "Alpha", K::number, 0.5)}, number, of(&P::math_lerp),
               "A + (B - A) × Alpha", "interpolate mix blend"));
  lib.add(pure("math.sin", "Sin (degrees)", "Math", {data("a", "Degrees", K::number, 0.0)}, number, of(&P::math_sin)));
  lib.add(pure("math.cos", "Cos (degrees)", "Math", {data("a", "Degrees", K::number, 0.0)}, number, of(&P::math_cos)));
  lib.add(pure("math.atan2", "Atan2 (degrees)", "Math", {data("y", "Y", K::number, 0.0), data("x", "X", K::number, 1.0)}, number, of(&P::math_atan2), "Angle of (X, Y) in degrees", "angle"));
  lib.add(pure("math.sqrt", "Square Root", "Math", {data("a", "A", K::number, 0.0)}, number, of(&P::math_sqrt)));
  lib.add(pure("math.pow", "Power", "Math", {data("a", "Base", K::number, 2.0), data("b", "Exponent", K::number, 2.0)}, number, of(&P::math_pow)));
  lib.add(pure("math.round", "Round", "Math", {data("a", "A", K::number, 0.0)}, integer, of(&P::math_round)));
  lib.add(pure("math.floor", "Floor", "Math", {data("a", "A", K::number, 0.0)}, integer, of(&P::math_floor)));
  lib.add(fresh(pure("math.random", "Random Float in Range", "Math", {data("min", "Min", K::number, 0.0), data("max", "Max", K::number, 1.0)}, data("result", "", K::number),
               [](Context& c) { const f64 lo = c.in_number("min"); return Value::number(lo + (c.in_number("max") - lo) * random01(c)); }, "Deterministic per world", "rand")));
  lib.add(fresh(pure("math.random_int", "Random Integer in Range", "Math", {data("min", "Min", K::integer, 0), data("max", "Max", K::integer, 10)}, data("result", "", K::integer), [](Context& c) {
    const i64 lo = c.in_int("min"), hi = max_of(lo, c.in_int("max"));
    return Value::integer(lo + static_cast<i64>(c.host().random() % static_cast<u32>(min_of<i64>(hi - lo + 1, 0x7fffffff))));
  }, "Min to Max inclusive", "rand")));
  lib.add(fresh(pure("math.random_bool", "Random Bool", "Math", {data("chance", "Chance", K::number, 0.5)}, data("result", "", K::boolean),
               [](Context& c) { return Value::boolean(random01(c) < c.in_number("chance")); }, "True with the given chance (0..1)", "rand coin")));
  {
    NodeDef d = pure("math.compare", "Compare", "Logic", {data("a", "A", K::number, 0.0), data("b", "B", K::number, 0.0)}, boolean, [](Context& c) {
      const f64 a = c.in_number("a"), b = c.in_number("b");
      return Value::boolean(P::compare(c.param("op", "=="), a, b));
    }, "Compares two numbers", "equal less greater == != < > <= >=");
    d.params = {param("op", "Operator", ParamDef::Kind::choice, "==", {"==", "=", "!=", "≠", "<", "<", "<=", "≤", ">", ">", ">=", "≥"})};
    d.title = [](const NodeDef&, const Node& n) { return "A " + n.param("op", "==") + " B"; };
    lib.add(std::move(d));
  }
  lib.add(pure("logic.and", "AND", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_and), "", "&& both"));
  lib.add(pure("logic.or", "OR", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_or), "", "|| either"));
  lib.add(pure("logic.xor", "XOR", "Logic", ab(K::boolean, false, false), boolean, of(&P::logic_xor)));
  lib.add(pure("logic.not", "NOT", "Logic", {data("a", "A", K::boolean, false)}, boolean, of(&P::logic_not), "", "! invert"));
  {
    NodeDef d = pure("logic.select", "Select", "Logic", {data("pick", "Pick A", K::boolean, true), data("a", "A", K::number, 0.0), data("b", "B", K::number, 0.0)}, data("result", "", K::number),
                     [](Context& c) { return c.in_bool("pick") ? c.in("a") : c.in("b"); }, "A when Pick A is true, otherwise B", "ternary choose if");
    d.params = {param("type", "Type", ParamDef::Kind::choice, "float", {"float", "Number", "int", "Integer", "bool", "Boolean", "vector", "Vector", "text", "Text", "actor", "Actor"})};
    d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      PinType t = PinType::number;
      pin_type_from_name(n.param("type", "float"), t);
      if (t == PinType::exec) t = PinType::number;
      for (PinDef& p : in) if (p.id != "pick") { p.type = t; p.def = Value::zero(t).to_json(); }
      out[0].type = t;
    };
    lib.add(std::move(d));
  }
  // Literals.
  lib.add(pure("value.float", "Number", "Values", {data("value", "", K::number, 0.0)}, number, of(&P::value_float), "A constant number", "float literal constant"));
  lib.add(pure("value.int", "Integer", "Values", {data("value", "", K::integer, 0)}, integer, of(&P::value_int), "A constant integer", "int literal"));
  lib.add(pure("value.bool", "Boolean", "Values", {data("value", "", K::boolean, false)}, boolean, of(&P::value_bool), "true / false", "literal"));
  lib.add(pure("value.text", "Text", "Values", {data("value", "", K::text, "")}, data("result", "", K::text), of(&P::value_text), "A constant text", "string literal"));
  // Vectors.
  const json::Value zero3 = [] { json::Value a = json::Value::array(); a.push(0.0); a.push(0.0); a.push(0.0); return a; }();
  const PinDef vector = data("result", "", K::vector);
  lib.add(pure("vector.make", "Make Vector", "Vector", {data("x", "X", K::number, 0.0), data("y", "Y", K::number, 0.0), data("z", "Z", K::number, 0.0)}, vector, of(&P::vector_make)));
  {
    NodeDef d = make("vector.break", "Break Vector", "Vector", NodeKind::pure, "X, Y and Z of a vector.");
    d.inputs = {data("v", "Vector", K::vector, zero3)};
    d.outputs = {data("x", "X", K::number), data("y", "Y", K::number), data("z", "Z", K::number)};
    d.run = [](Context& c) {
      const P::Broken b = P::vector_break(c.in_vector("v"));
      c.out("x", Value::number(b.x));
      c.out("y", Value::number(b.y));
      c.out("z", Value::number(b.z));
      return kStop;
    };
    lib.add(std::move(d));
  }
  const std::vector<PinDef> vab{data("a", "A", K::vector, zero3), data("b", "B", K::vector, zero3)};
  lib.add(pure("vector.add", "Vector + Vector", "Vector", vab, vector, of(&P::vector_add), "", "add plus"));
  lib.add(pure("vector.sub", "Vector - Vector", "Vector", vab, vector, of(&P::vector_sub), "", "subtract minus"));
  lib.add(pure("vector.cross", "Cross Product", "Vector", vab, vector, of(&P::vector_cross)));
  lib.add(pure("vector.scale", "Vector × Float", "Vector", {data("a", "Vector", K::vector, zero3), data("b", "Scale", K::number, 1.0)}, vector, of(&P::vector_scale), "", "multiply scale"));
  lib.add(pure("vector.length", "Vector Length", "Vector", {data("a", "Vector", K::vector, zero3)}, number, of(&P::vector_length), "", "magnitude size"));
  lib.add(pure("vector.normalize", "Normalize", "Vector", {data("a", "Vector", K::vector, zero3)}, vector, of(&P::vector_normalize), "", "unit direction"));
  lib.add(pure("vector.distance", "Distance", "Vector", vab, number, of(&P::vector_distance)));
  lib.add(pure("vector.dot", "Dot Product", "Vector", vab, number, of(&P::vector_dot)));
  lib.add(pure("vector.lerp", "Lerp Vector", "Vector", {data("a", "A", K::vector, zero3), data("b", "B", K::vector, zero3), data("alpha", "Alpha", K::number, 0.5)}, vector, of(&P::vector_lerp), "",
               "interpolate"));
  // Text.
  lib.add(pure("text.append", "Append", "Text", {data("a", "A", K::text, ""), data("b", "B", K::text, ""), data("c", "C", K::text, "")}, data("result", "", K::text), of(&P::text_append),
               "Joins texts (numbers and vectors convert)", "concat string join format"));
  lib.add(pure("text.equal", "Text Equals", "Text", {data("a", "A", K::text, ""), data("b", "B", K::text, "")}, boolean, of(&P::text_equal)));
  lib.add(pure("text.contains", "Contains", "Text", {data("a", "Text", K::text, ""), data("b", "Search", K::text, "")}, boolean, of(&P::text_contains)));
  lib.add(pure("text.length", "Text Length", "Text", {data("a", "Text", K::text, "")}, integer, of(&P::text_length)));
  // Time.
  lib.add(pure("time.now", "Game Time", "Utilities", {}, data("result", "Seconds", K::number), [](Context& c) { return Value::number(c.host().time()); }, "Seconds since play started", "clock elapsed"));
  {
    NodeDef d = make("debug.print", "Print", "Utilities", NodeKind::action, "Writes text to the log (and on screen).");
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

// A formula as one node: every name in it is an input pin (logic.h, Formula).
void add_formula(Library& lib) {
  NodeDef d = make("math.formula", "Formula", "Math", NodeKind::pure,
                   "A formula as one node: every name in it becomes an input. Operators + - * / % ^, comparisons (1 when true), && || !, a ? b : c; "
                   "functions min max clamp abs sign floor ceil round trunc sqrt pow exp log sin cos tan asin acos atan atan2 (radians; rad(), deg()) lerp; pi.");
  d.params = {param("formula", "Formula", ParamDef::Kind::text, "a + b")};
  d.outputs = {data("result", "", PinType::number)};
  d.keywords = "math expression formula equation calculate compute";
  d.shape = [](const NodeDef&, const Node& n, const Graph&, std::vector<PinDef>& in, std::vector<PinDef>&) {
    const std::string text = n.param("formula", "a + b");
    for (const std::string& name : Formula::cached(text).inputs()) in.push_back(data(name, name, PinType::number, 0.0));
  };
  d.title = [](const NodeDef&, const Node& n) {
    const std::string f = n.param("formula", "a + b");
    return f.size() > 44 ? f.substr(0, 43) + "..." : f;
  };
  d.problem = [](const Node& n, const Graph&) {
    const std::string text = n.param("formula", "a + b");
    return Formula::cached(text).error();
  };
  d.run = [](Context& c) {
    const std::string text = c.param("formula", "a + b");
    const Formula& f = Formula::cached(text);
    f64 values[Formula::kMaxInputs] = {};
    const std::size_t count = min_of(f.inputs().size(), c.input_count());
    for (std::size_t i = 0; i < count; ++i) values[i] = c.in(i).as_number();
    c.out(std::size_t{0}, Value::number(f.evaluate(values, count)));
    return kStop;
  };
  lib.add(std::move(d));
}

// Functions: named, with typed inputs and outputs (Graph::functions). The four nodes take their
// data pins from the function they name; a pin's id is its name.
void add_functions(Library& lib) {
  // which == 0: the inputs as outputs (entry); 1: the outputs as inputs (return); 2: both (calls)
  auto shape = [](int which) {
    return [which](const NodeDef&, const Node& n, const Graph& g, std::vector<PinDef>& in, std::vector<PinDef>& out) {
      const Function* f = g.function(n.param("function"));
      if (!f) return;
      if (which == 0 || which == 2)
        for (const FunctionPin& p : f->inputs) (which == 0 ? out : in).push_back({p.name, p.name, p.type, p.value});
      if (which == 1 || which == 2)
        for (const FunctionPin& p : f->outputs) (which == 1 ? in : out).push_back({p.name, p.name, p.type, p.value});
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
    args.reserve(f->inputs.size());
    for (const FunctionPin& p : f->inputs) args.push_back(c.in(p.name));
    c.instance().call(f->name, args, results);
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
        for (const FunctionPin& p : f->outputs) values.emplace_back(p.name, c.in(p.name));
      c.instance().return_values(c.param("function"), values);
      return kStop;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = make("func.call", "Call Function", "Functions", NodeKind::action,
                     "Runs a function of this Blueprint to its end, then continues. Its results stay on the outputs until the next call.");
    d.params = {param("function", "Function", ParamDef::Kind::function)};
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
    add_math(lib);
    add_formula(lib);
    add_functions(lib);
  }
  return lib;
}

}  // namespace kern::logic
