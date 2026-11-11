// A Blueprint's C++ compiled by a C++ compiler computes what the interpreter computes
// (docs/blueprint_cpp.md). The cases: Blueprints written in the notation, and one made of every
// value node blueprint.h has; each with calls. kern_tests --emit-native <dir> writes each
// Blueprint's text exactly as logic::to_cpp gives it (and after it the glue the engine reaches
// the class through, logic::native_glue), a main() that makes the calls - on the class itself
// and, by name, through a logic::Instance that runs the graph as that class - and what the
// interpreter answers; run.sh compiles the texts into blueprint_native_test and compares.
#pragma once

#include <algorithm>
#include <cstdio>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "kern/game/logic_nodes.h"
#include "kern/logic/logic.h"
#include "logic_cases.h"
#include "native_show.h"

namespace blueprint_native {
using namespace kern;
using logic::PinType;
using logic::Value;

struct Call {
  std::string function;
  std::vector<Value> args;
};
struct Case {
  std::string name;
  logic::Graph graph;
  std::vector<Call> calls;
  bool scenario = false;  // no calls: the scenario of the tier cases (logic_cases::play) - events, ticks
};

// Branches, loops, the flow nodes that keep something, switches, steps several wires lead to.
inline const char* kFlow = R"kn(
struct Flow : kern::Blueprint {
  Float Heat = 0.0;
  Int Count = 0;
  Bool Open = false;
  Text Log = "";
  Vector Home = Vector{1.0, 2.0, 3.0};

  struct GradeResult { Int grade = 0; Text name = "none"; };
  /// Branches that return, an else-if chain, a path that ends without a Return.
  KERN_PURE GradeResult Grade(Float points = 0.0) {
    if (math_compare<">=">(points, 90.0)) {
      return {1, "top"};
    } else if (math_compare<">=">(points, 50.0)) {
      return {2, "pass"};
    } else if (math_compare<"<">(points, 0.0)) {
      return {};
    }
    return {3, text_append("fail ", kern::to<Text>(points), "")};
  }

  struct FirstAboveResult { Int sum = 0; Int last = -1; };
  /// A loop that returns from its middle, an index read after the loop.
  KERN_PURE FirstAboveResult FirstAbove(Int from = 0, Int to = 9, Float limit = 4.5) {
    for (auto it12 = flow_for(n12, from, to); it12.next();) {
      if (math_compare<">">(kern::to<Float>(n12.index), limit)) {
        return {n12.index, n12.index};
      }
    }
    return {-1, n12.index};
  }

  /// Nested loops, Add with whole numbers and with fractions, a While that counts down.
  void Fill(Int rows = 2, Int columns = 3) {
    Count = 0;
    for (auto it20 = flow_for(n20, 1, rows); it20.next();) {
      for (auto it21 = flow_for(n21, 1, columns); it21.next();) {
        Count += math_round(math_mul(kern::to<Float>(n20.index), kern::to<Float>(n21.index)));
        Heat += 0.25;
      }
    }
    while (loop(math_compare<">">(Heat, 1.0))) {
      Heat += -1.0;
      Log = text_append(Log, "-", "");
    }
  }

  /// Do Once, Do N, Gate and Flip Flop keep what they are between calls.
  void Step(Text what = "go") {
    switch (flow_switch_text<"go, reset, open, close, toggle, flip">(what)) {
      case 0: {
        {
          go30("exec");
        }
        {
          go31("exec");
        }
        {
          go32("enter");
        }
      } break;
      case 1: {
        {
          go30("reset");
        }
        {
          go31("reset");
        }
      } break;
      case 2: { go32("open"); } break;
      case 3: { go32("close"); } break;
      case 4: { go32("toggle"); } break;
      case 5: {
        switch (flow_flip_flop(n33)) {
          case 0: { Log = text_append(Log, "A", kern::to<Text>(n33.is_a)); } break;
          case 1: { Log = text_append(Log, "B", kern::to<Text>(n33.is_a)); } break;
        }
      } break;
      case 6: { Log = "?"; } break;
    }
  }

  /// Switch on Int with its default, an exec knot, paths that meet again.
  void Choose(Int which = 0) {
    flow_reroute();
    switch (flow_switch_int<"3">(which)) {
      case 0: { Log = "zero"; } break;
      case 1: { Log = "one"; } break;
      case 2: { Log = "two"; } break;
      case 3: { Log = "other"; } break;
    }
    Count += which;
  }

  void go30(Text entry) {
    if (flow_do_once(n30, entry, false)) {
      Log = text_append(Log, "once ", "");
    }
  }

  void go31(Text entry) {
    if (flow_do_n(n31, entry, 2)) {
      Log = text_append(Log, "n", kern::to<Text>(n31.counter));
    }
  }

  void go32(Text entry) {
    if (flow_gate(n32, entry, true)) {
      Open = logic_not(Open);
    }
  }

  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  kern::Node<30> n30;
  struct : kern::Node<31> { Int counter = 0; } n31;
  kern::Node<32> n32;
  struct : kern::Node<33> { Bool is_a = false; } n33;
};
)kn";

// Values with names, functions as values and as steps, results that are kept, an entry's values
// read from another body.
inline const char* kCalls = R"kn(
struct Calls : kern::Blueprint {
  Float Total = 0.0;
  Int Steps = 0;
  Text Trace = "";
  Bool Flag = false;

  struct ScaleResult { Float out = 0.0; Float twice = 0.0; };
  /// A value two inputs read (it has a name), a formula.
  KERN_PURE ScaleResult Scale(Float x = 1.0, Float by = 2.0) {
    const auto n40 = [&] { return math_mul(x, by); };
    return {n40(), kern::formula([](Float a, Float b) -> Float { return a + b; }, n40(), n40())};
  }

  struct AddUpResult { Float total = 0.0; Int steps = 0; };
  /// A function's value in a loop, an Add whose result is read, a Set whose result is read.
  AddUpResult AddUp(Float amount = 1.0, Int times = 1) {
    for (auto it42 = flow_for(n42, 1, times); it42.next();) {
      n43.value = (Total += Scale(amount, 1.5).out);
      n44.value = (Steps = math_floor(n43.value));
      Trace = text_append(Trace, kern::to<Text>(n44.value), ",");
    }
    return {Total, Steps};
  }

  /// A step two wires lead to reads what the function was given: the entry's values are kept.
  void Mark(Float amount = 1.0, Bool twice = false) {
    n50 = {amount, twice};
    {
      go52();
    }
    {
      if (twice) {
        go52();
      }
    }
  }

  struct BothResult { Float sum = 0.0; Text text = ""; };
  /// Functions called as steps: each call keeps its results.
  BothResult Both(Float a = 1.0, Float b = 2.0) {
    n61 = AddUp(a, 2);
    n62 = AddUp(b, 1);
    Flag = math_compare<">">(n61.total, n62.total);
    Mark(n62.total, Flag);
    return {math_add(n61.total, n62.total), text_append(kern::to<Text>(n61.steps), "/", kern::to<Text>(n62.steps))};
  }

  void go52() {
    n52.value = (Total += n50.amount);
    Trace = text_append(Trace, kern::to<Text>(n52.value), kern::to<Text>(n50.twice));
  }

  struct : kern::Node<42> { Int index = 0; } n42;
  struct { Float value = 0.0; } n43;
  struct { Int value = 0; } n44;
  struct { Float amount = 0.0; Bool twice = false; } n50;
  struct { Float value = 0.0; } n52;
  AddUpResult n61;
  AddUpResult n62;
};
)kn";

// Every wire between different types, Select and Reroute in every type, typed-in values.
inline const char* kCasts = R"kn(
struct Casts : kern::Blueprint {
  Bool B = false;
  Int I = 0;
  Float F = 0.0;
  Vector V = Vector{0.0, 0.0, 0.0};
  Text T = "";
  Actor A = Actor{0};

  /// Every wire between pins of different types.
  void FromFloat(Float x = 0.0) {
    B = kern::to<Bool>(x);
    I = kern::to<Int>(x);
    F = x;
    V = kern::to<Vector>(x);
    T = kern::to<Text>(x);
  }

  void FromInt(Int x = 0) {
    B = kern::to<Bool>(x);
    I = x;
    F = kern::to<Float>(x);
    V = kern::to<Vector>(x);
    T = kern::to<Text>(x);
  }

  void FromBool(Bool x = false) {
    B = x;
    I = kern::to<Int>(x);
    F = kern::to<Float>(x);
    T = kern::to<Text>(x);
  }

  void FromVector(Vector x = Vector{0.0, 0.0, 0.0}) {
    V = x;
    T = kern::to<Text>(x);
  }

  void FromActor(Actor x = Actor{0}) {
    A = x;
    B = kern::to<Bool>(x);
    T = kern::to<Text>(x);
  }

  struct PickResult { Float number = 0.0; Int whole = 0; Bool yes = false; Vector spot = Vector{0.0, 0.0, 0.0}; Text word = ""; Actor who = Actor{0}; };
  /// Select and Reroute in every type; the typed-in values of a node.
  KERN_PURE PickResult Pick(Bool first = true, Actor who = Actor{0}) {
    return {logic_select<"float">(first, 1.5, -2.5), logic_select<"int">(first, 7, -7), logic_select<"bool">(first, true, false),
            value_reroute<"vector">(logic_select<"vector">(first, Vector{1.0, 2.0, 3.0}, Vector{-1.0, 0.5, 0.25})), value_reroute<"text">(logic_select<"text">(first, "a \"word\"", "tab\tand\\slash")),
            value_reroute<"actor">(logic_select<"actor">(first, who, Actor{7}))};
  }
};
)kn";

// Lists of every type: the steps that change a list variable (at every index), the values that
// read a list, For Each (nested, over a list its body changes), lists a function takes and gives.
inline const char* kLists = R"kn(
struct Lists : kern::Blueprint {
  List<Float> Scores = List<Float>{1.0, 2.5};
  List<Int> Counts = List<Int>{};
  List<Text> Names = List<Text>{"a", "b", "a"};
  List<Bool> Flags = List<Bool>{};
  List<Vector> Points = List<Vector>{Vector{1.0, 2.0, 3.0}};
  List<Actor> Team = List<Actor>{};
  Float Total = 0.0;
  Text Shown = "";

  struct PushResult { Int index = 0; Int length = 0; Text shown = ""; };
  /// Add gives the index; a list prints its items.
  PushResult Push(Float value = 0.0) {
    list_add(n3, Scores, value);
    return {n3.index, list_length<"float">(Scores), kern::to<Text>(Scores)};
  }

  struct EditResult { Bool removed = false; Int length = 0; Text shown = ""; };
  /// The steps that change a list variable, at every index - also far beyond the list.
  EditResult Edit(Text what = "insert", Int index = 0, Float value = 0.0) {
    switch (flow_switch_text<"insert, set, remove_at, remove, clear">(what)) {
      case 0: { list_insert(Scores, index, value); } break;
      case 1: { list_set(Scores, index, value); } break;
      case 2: { list_remove_at(Scores, index); } break;
      case 3: { list_remove(n14, Scores, value); } break;
      case 4: { list_clear(Scores); } break;
    }
    return {n14.removed, list_length<"float">(Scores), kern::to<Text>(Scores)};
  }

  struct SumResult { Float total = 0.0; Int passes = 0; Int last = 0; };
  /// For Each, one inside the other; the index read after the loop.
  SumResult Sum() {
    Total = 0.0;
    for (auto it20 = flow_for_each<"float">(n20, Scores); it20.next();) {
      Total += n20.item;
      for (auto it22 = flow_for_each<"text">(n22, Names); it22.next();) {
        Shown = text_append(Shown, n22.item, kern::to<Text>(n22.index));
      }
    }
    return {Total, list_length<"text">(Names), n20.index};
  }

  struct AtResult { Float score = 0.0; Text name = ""; Int count = 0; Bool flag = false; Vector point = Vector{0.0, 0.0, 0.0}; Actor member = Actor{0}; Int where = 0; Bool has = false; };
  /// Get Item of every type (beyond the list: its zero), Find and Contains.
  KERN_PURE AtResult At(Int index = 0, Text name = "a") {
    return {list_get<"float">(Scores, index), list_get<"text">(Names, index), list_get<"int">(Counts, index), list_get<"bool">(Flags, index), list_get<"vector">(Points, index),
            list_get<"actor">(Team, index), list_find<"text">(Names, name), list_contains<"text">(Names, name)};
  }

  struct FillResult { List<Int> counts = List<Int>{}; List<Bool> flags = List<Bool>{}; List<Vector> points = List<Vector>{}; List<Actor> team = List<Actor>{}; };
  /// Lists of every type filled in a loop; a function gives lists.
  FillResult Fill(Int n = 3, Int big = 4611686018427387904, Actor who = Actor{0}) {
    list_clear(Counts);
    list_clear(Flags);
    list_clear(Points);
    list_clear(Team);
    for (auto it40 = flow_for(n40, 1, n); it40.next();) {
      list_add(Counts, math_round(math_mul(kern::to<Float>(n40.index), kern::to<Float>(big))));
      list_add(Flags, math_compare<"==">(math_mod(kern::to<Float>(n40.index), 2.0), 0.0));
      list_insert(Points, 0, vector_make(kern::to<Float>(n40.index), 0.5, -1.0));
      list_add(Team, who);
    }
    list_set(Counts, 0, -7);
    list_remove(Flags, false);
    return {Counts, Flags, Points, Team};
  }

  struct TakeResult { Int length = 0; List<Float> twice = List<Float>{}; List<Float> picked = List<Float>{}; };
  /// A function takes a list (with a default); Set of a list variable; Make List; Select between lists.
  TakeResult Take(List<Float> values = List<Float>{3.0, 4.0, 3.0}, Bool pick = true) {
    Scores = values;
    return {list_length<"float">(values), list_make<"float", "3">(list_get<"float">(values, 0), list_get<"float">(values, 0), 9.5),
            logic_select<"float_list">(pick, Scores, value_reroute<"float_list">(List<Float>{-1.0}))};
  }

  struct GrowResult { Int passes = 0; Int length = 0; };
  /// The loop goes through the list as it was when it started, whatever its body does to the variable.
  GrowResult Grow() {
    Total = 0.0;
    for (auto it60 = flow_for_each<"float">(n60, Scores); it60.next();) {
      list_add(Scores, math_add(n60.item, 100.0));
      list_remove_at(Scores, 0);
      Total += 1.0;
    }
    return {kern::to<Int>(Total), list_length<"float">(Scores)};
  }

  struct { Int index = 0; } n3;
  struct { Bool removed = false; } n14;
  struct : kern::Node<20> { Float item = 0.0; Int index = 0; } n20;
  struct : kern::Node<22> { Text item = ""; Int index = 0; } n22;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<60> { Float item = 0.0; Int index = 0; } n60;
};
)kn";

// A Sequence whose first part returns: the function has ended, the second part does not run.
inline const char* kFunctions = R"({"variables":[{"name":"Calls","type":"int","value":0}],
  "functions":[{"name":"Double","pure":true,"inputs":[{"name":"x","type":"float","value":0}],"outputs":[{"name":"result","type":"float","value":0}]},
    {"name":"Clamp01","inputs":[{"name":"v","type":"float","value":0.5}],"outputs":[{"name":"out","type":"float","value":-1},{"name":"low","type":"bool","value":false}]},
    {"name":"Label","inputs":[{"name":"who","type":"text","value":"nobody"}],"outputs":[{"name":"text","type":"text","value":""}]}],
  "nodes":[{"id":1,"type":"func.entry","params":{"function":"Double"}},{"id":2,"type":"math.mul","values":{"b":2}},{"id":3,"type":"func.return","params":{"function":"Double"}},
    {"id":10,"type":"func.entry","params":{"function":"Clamp01"}},{"id":11,"type":"var.add","params":{"variable":"Calls"}},{"id":12,"type":"flow.sequence","params":{"count":"2"}},
    {"id":13,"type":"flow.branch"},{"id":14,"type":"math.compare","params":{"op":"<"},"values":{"b":0}},
    {"id":15,"type":"func.return","params":{"function":"Clamp01"},"values":{"out":0,"low":true}},
    {"id":16,"type":"func.return","params":{"function":"Clamp01"}},{"id":17,"type":"math.min","values":{"b":1}},
    {"id":30,"type":"func.entry","params":{"function":"Label"}},{"id":31,"type":"text.append","values":{"a":"hello "}},{"id":32,"type":"func.return","params":{"function":"Label"}}],
  "links":[[1,"then",3,"exec"],[1,"x",2,"a"],[2,"result",3,"result"],
    [10,"then",11,"exec"],[11,"then",12,"exec"],[12,"then0",13,"exec"],[10,"v",14,"a"],[14,"result",13,"condition"],[13,"true",15,"exec"],
    [12,"then1",16,"exec"],[10,"v",17,"a"],[17,"result",16,"out"],
    [30,"then",32,"exec"],[30,"who",31,"b"],[31,"result",32,"text"]]})";

inline const char* kFormulas[] = {
    "a + b * c - a / b",
    "a % b + b % c",
    "a ^ b",
    "-a + !b",
    "(a < b) + (a <= b) * 2 + (a > b) * 4 + (a >= b) * 8 + (a == b) * 16 + (a != b) * 32",
    "a && b || !c",
    "a ? b : c",
    "min(a, b) + max(b, c) + clamp(a, b, c)",
    "abs(a) + sign(b)",
    "floor(a) + ceil(b) * 10 + round(c) * 100 + trunc(a) * 1000",
    "sqrt(a) + pow(b, 2) + exp(c / 1000) + log(b)",
    "sin(a) + cos(b) + tan(c)",
    "asin(a) + acos(b) + atan(c) + atan2(a, b)",
    "rad(a) + deg(b)",
    "lerp(a, b, c)",
    "pi * a",
    "1e3 + .5 * a - 2.5e-1",
    "a / 0 + b % 0",
    "max(9000, 16000 - level * 1300)",
    "pi",
};

/// The values a pin of a type is tried with.
inline std::vector<Value> samples(PinType t) {
  std::vector<Value> r;
  const f64 inf = std::numeric_limits<f64>::infinity();
  switch (t) {
    case PinType::boolean: r = {Value::boolean(false), Value::boolean(true)}; break;
    case PinType::integer:
      // (and whole numbers at their ends: 2^53, 2^62, Int's upper end)
      for (i64 v : {i64{0}, i64{1}, i64{-1}, i64{2}, i64{7}, i64{-13}, i64{100}, i64{1000000007}, i64{9007199254740992}, i64{-9007199254740992}, i64{4611686018427387904},
                    std::numeric_limits<i64>::max()})
        r.push_back(Value::integer(v));
      break;
    case PinType::number:
      for (f64 v : {0.0, 1.0, -1.0, 0.5, -2.5, 3.75, 90.0, 1e-9, 123456.789, -0.0, 2.0, 45.0, -180.0, 7.0, 0.3, 1e19, -1e300, inf, -inf, std::numeric_limits<f64>::quiet_NaN()})
        r.push_back(Value::number(v));
      break;
    case PinType::vector:
      for (Vec3 v : {Vec3{0, 0, 0}, Vec3{1, 0, 0}, Vec3{0, -2, 0}, Vec3{1.5f, 2.5f, -3.5f}, Vec3{1000, 0.001f, 7}}) r.push_back(Value::vector(v));
      break;
    case PinType::text:
      for (const char* v : {"", "a", "Kern", "hello world", "a \"b\" \\ c\n\ttab", "\xC3\xA4\xE2\x82\xAC"}) r.push_back(Value::text(v));
      break;
    case PinType::actor:
      for (logic::ActorRef v : {0u, 1u, 42u}) r.push_back(Value::actor(v));
      break;
    default: break;
  }
  if (logic::is_list(t)) {
    // lists of the item type's samples: none, one, all of them, some of them twice
    const std::vector<Value> items = samples(logic::element_of(t));
    auto list = [&](std::vector<std::size_t> picks) {
      json::Value a = json::Value::array();
      for (std::size_t k : picks) a.push(items[k % items.size()].to_json());
      return Value::from_json(a, t);
    };
    std::vector<std::size_t> all;
    for (std::size_t k = 0; k < items.size(); ++k) all.push_back(k);
    r = {list({}), list({1}), list(all), list({1, 0, 1, 2, 1}), list({2, 3, 4, 5, 3, 2})};
  }
  return r;
}

/// Adds the function P<k> to `g`: the value node `def` with `params` between a Function Entry
/// and a Return - its inputs are the function's, its outputs the function's results.
inline std::string add_probe(logic::Graph& g, const logic::NodeDef& def, const std::vector<std::pair<std::string, std::string>>& params, int k, const logic::Library& lib) {
  logic::Function f;
  f.name = "P" + std::to_string(k);
  f.pure = true;
  f.help = def.type;
  for (const auto& [key, value] : params) f.help += " " + value;
  logic::Node entry, node, give;
  entry.id = g.next_id++;
  node.id = g.next_id++;
  give.id = g.next_id++;
  entry.type = "func.entry";
  give.type = "func.return";
  node.type = def.type;
  entry.set_param("function", f.name);
  give.set_param("function", f.name);
  for (const auto& [key, value] : params) node.set_param(key, value);
  entry.y = node.y = give.y = 160.0f * static_cast<f32>(k);
  node.x = 280;
  node.y += 110;
  give.x = 560;
  std::vector<logic::PinDef> in, out;
  lib.pins(node, g, in, out);
  g.links.push_back({entry.id, "then", give.id, "exec"});
  for (const logic::PinDef& p : in) {
    if (p.type == PinType::exec) continue;
    const std::string name = "i" + std::to_string(f.inputs.size());
    f.inputs.push_back({name, p.type, p.def.is_null() ? Value::zero(p.type).to_json() : p.def});
    g.links.push_back({entry.id, name, node.id, p.id});
  }
  for (const logic::PinDef& p : out) {
    if (p.type == PinType::exec) continue;
    const std::string name = "o" + std::to_string(f.outputs.size());
    f.outputs.push_back({name, p.type, Value::zero(p.type).to_json()});
    g.links.push_back({node.id, p.id, give.id, name});
  }
  g.functions.push_back(f);
  g.nodes.push_back(std::move(entry));
  g.nodes.push_back(std::move(node));
  g.nodes.push_back(std::move(give));
  return f.name;
}

/// Every value node of the core library, in every setting, each as a function of its own - and
/// called with many values. (What the gameplay nodes give needs a game: they are in the
/// Blueprints of game_cases.h, played by game_native_test; this test links Kern Logic alone.)
inline Case nodes_case(const logic::Library& lib = logic::library()) {
  Case c;
  c.name = "Nodes";
  int k = 0;
  const std::vector<std::string>& of_the_game = game::logic_node_types();
  for (const auto& d : lib.all()) {
    if (d->kind != logic::NodeKind::pure || std::find(of_the_game.begin(), of_the_game.end(), d->type) != of_the_game.end()) continue;
    std::vector<std::vector<std::pair<std::string, std::string>>> variants{{}};
    if (d->type == "math.formula") {
      variants.clear();
      for (const char* formula : kFormulas) variants.push_back({{"formula", formula}});
    } else if (d->params.size() == 1 && d->params[0].kind == logic::ParamDef::Kind::choice) {
      variants.clear();
      for (std::size_t i = 0; i + 1 < d->params[0].choices.size(); i += 2) variants.push_back({{d->params[0].key, d->params[0].choices[i]}});
    } else if (d->type == "list.make") {
      // (a list of every item type, and lists of no, one and many items)
      variants.clear();
      for (std::size_t i = 0; i + 1 < d->params[0].choices.size(); i += 2) variants.push_back({{"type", d->params[0].choices[i]}, {"count", "3"}});
      for (const char* count : {"0", "1", "16"}) variants.push_back({{"type", "int"}, {"count", count}});
    }
    for (const auto& params : variants) {
      logic::Graph alone;
      add_probe(alone, *d, params, 0, lib);
      if (!logic::native_ready(alone, nullptr, lib)) continue;
      const std::string name = add_probe(c.graph, *d, params, k, lib);
      const logic::Function& f = *c.graph.function(name);
      std::vector<std::vector<Value>> pool;
      std::size_t most = 1;
      for (const logic::FunctionPin& p : f.inputs) {
        pool.push_back(samples(p.type));
        most = max_of(most, pool.back().size());
      }
      u32 seed = 2463534242u + static_cast<u32>(k) * 7919u;
      const std::size_t rounds = f.inputs.empty() ? 1 : most + 14;
      for (std::size_t r = 0; r < rounds; ++r) {
        Call call{name, {}};
        for (std::size_t i = 0; i < pool.size(); ++i) {
          std::size_t pick = r % pool[i].size();  // first every value on every input at once (equal inputs), then mixed
          if (r >= most) {
            seed ^= seed << 13;
            seed ^= seed >> 17;
            seed ^= seed << 5;
            pick = seed % pool[i].size();
          }
          call.args.push_back(pool[i][pick]);
        }
        c.calls.push_back(std::move(call));
      }
      ++k;
    }
  }
  return c;
}

inline std::vector<Value> numbers(std::initializer_list<f64> v) {
  std::vector<Value> r;
  for (f64 x : v) r.push_back(Value::number(x));
  return r;
}

inline std::vector<Case> cases() {
  std::vector<Case> all;
  auto read = [&](const char* name, const char* text) -> Case& {
    all.push_back({name, {}, {}});
    std::string error;
    if (!logic::from_cpp(text, all.back().graph, nullptr, &error)) std::printf("blueprint_native %s: %s\n", name, error.c_str());
    return all.back();
  };
  const Value yes = Value::boolean(true), no = Value::boolean(false);
  auto N = [](f64 v) { return Value::number(v); };
  auto I = [](i64 v) { return Value::integer(v); };
  auto T = [](const char* v) { return Value::text(v); };
  {
    Case& c = read("Flow", kFlow);
    for (f64 v : {95.0, 90.0, 89.999, 50.0, 12.5, -3.0, 0.0, std::numeric_limits<f64>::quiet_NaN()}) c.calls.push_back({"Grade", {N(v)}});
    c.calls.push_back({"FirstAbove", {I(0), I(9), N(4.5)}});
    c.calls.push_back({"FirstAbove", {I(3), I(3), N(0)}});
    c.calls.push_back({"FirstAbove", {I(5), I(2), N(1)}});  // no pass: the index of the run before stays
    c.calls.push_back({"FirstAbove", {I(0), I(5), N(100)}});
    c.calls.push_back({"FirstAbove", {I(-3), I(3), N(-1.5)}});
    c.calls.push_back({"Fill", {I(2), I(3)}});
    c.calls.push_back({"Fill", {I(0), I(3)}});
    c.calls.push_back({"Fill", {I(4), I(4)}});
    for (const char* what : {"go", "go", "go", "open", "go", "reset", "go", "flip", "flip", "toggle", "go", "close", "go", "x", "flip"}) c.calls.push_back({"Step", {T(what)}});
    for (i64 which : {-1, 0, 1, 2, 3, 100}) c.calls.push_back({"Choose", {I(which)}});
    c.calls.push_back({"Grade", {N(51)}});
  }
  {
    Case& c = read("Calls", kCalls);
    c.calls.push_back({"Scale", {N(1), N(2)}});
    c.calls.push_back({"Scale", {N(2.5), N(-1.5)}});
    c.calls.push_back({"AddUp", {N(1), I(1)}});
    c.calls.push_back({"AddUp", {N(2.5), I(3)}});
    c.calls.push_back({"AddUp", {N(0.1), I(0)}});
    c.calls.push_back({"AddUp", {N(-4), I(2)}});
    c.calls.push_back({"Mark", {N(1), no}});
    c.calls.push_back({"Mark", {N(2), yes}});
    c.calls.push_back({"Both", {N(1), N(2)}});
    c.calls.push_back({"Both", {N(5), N(0.5)}});
    c.calls.push_back({"Both", {N(-1), N(-1)}});
    c.calls.push_back({"Scale", {N(0), N(0)}});
  }
  {
    Case& c = read("Casts", kCasts);
    for (const Value& v : samples(PinType::number)) c.calls.push_back({"FromFloat", {v}});
    for (const Value& v : samples(PinType::integer)) c.calls.push_back({"FromInt", {v}});
    for (const Value& v : samples(PinType::boolean)) c.calls.push_back({"FromBool", {v}});
    for (const Value& v : samples(PinType::vector)) c.calls.push_back({"FromVector", {v}});
    for (const Value& v : samples(PinType::actor)) c.calls.push_back({"FromActor", {v}});
    c.calls.push_back({"Pick", {yes, Value::actor(3)}});
    c.calls.push_back({"Pick", {no, Value::actor(3)}});
  }
  {
    Case& c = read("Lists", kLists);
    const f64 nan = std::numeric_limits<f64>::quiet_NaN();
    const i64 most = std::numeric_limits<i64>::max();
    c.calls.push_back({"At", {I(0), T("a")}});
    for (f64 v : {7.5, 0.0, 2.5, nan, -1e300}) c.calls.push_back({"Push", {N(v)}});
    for (i64 at : {i64{-5}, i64{0}, i64{2}, i64{100}, most, -most}) c.calls.push_back({"Edit", {T("insert"), I(at), N(static_cast<f64>(at % 7) + 0.25)}});
    for (i64 at : {i64{-1}, i64{0}, i64{3}, i64{11}, i64{12}, most}) c.calls.push_back({"Edit", {T("set"), I(at), N(42)}});
    c.calls.push_back({"Sum", {}});
    for (f64 v : {42.0, 42.0, 2.5, nan, 1000.0}) c.calls.push_back({"Edit", {T("remove"), I(0), N(v)}});
    for (i64 at : {i64{-1}, i64{99}, i64{1}, i64{0}, i64{0}, most}) c.calls.push_back({"Edit", {T("remove_at"), I(at), N(0)}});
    c.calls.push_back({"Edit", {T("nothing"), I(0), N(0)}});
    c.calls.push_back({"Grow", {}});
    c.calls.push_back({"Sum", {}});
    for (i64 n : {i64{0}, i64{1}, i64{3}, i64{6}}) c.calls.push_back({"Fill", {I(n), I(n == 3 ? 4611686018427387904 : 1000000007), Value::actor(static_cast<logic::ActorRef>(n * 7))}});
    for (i64 at : {i64{-1}, i64{0}, i64{1}, i64{2}, i64{5}, i64{6}, most}) c.calls.push_back({"At", {I(at), T(at == 1 ? "b" : at == 2 ? "zz" : "a")}});
    for (const Value& v : samples(PinType::number_list)) {
      c.calls.push_back({"Take", {v, yes}});
      c.calls.push_back({"Take", {v, no}});
      c.calls.push_back({"Grow", {}});
    }
    c.calls.push_back({"Edit", {T("clear"), I(0), N(0)}});
    c.calls.push_back({"Edit", {T("insert"), I(5), N(1.5)}});
    c.calls.push_back({"Grow", {}});
    c.calls.push_back({"Sum", {}});
  }
  {
    all.push_back({"Functions", {}, {}});
    Case& c = all.back();
    json::Value j;
    if (!json::parse(kFunctions, j) || !logic::graph_from_json(j, c.graph)) std::printf("blueprint_native Functions: not read\n");
    c.calls.push_back({"Double", {N(21)}});
    for (f64 v : {0.25, 3.0, -2.0, 0.0, 1.0}) c.calls.push_back({"Clamp01", {N(v)}});
    c.calls.push_back({"Label", {T("Kern")}});
  }
  all.push_back(nodes_case());
  // The tier cases a compiler runs as they stand: their events are handlers of the class, the
  // runtime is the same for every tier.
  for (const logic_cases::Case& t : logic_cases::cases()) {
    Case c;
    c.name = std::string("Tier_") + t.name;
    c.graph = logic_cases::parse(t);
    c.scenario = true;
    if (logic::native_ready(c.graph)) all.push_back(std::move(c));
  }
  return all;
}

using Host = NativeTestHost;

/// One line per call: Function(values) -> results | variables.
inline std::string line(const std::string& function, const std::vector<std::string>& args, const std::vector<std::pair<std::string, std::string>>& results,
                        const std::vector<std::pair<std::string, std::string>>& variables) {
  std::string s = function + "(";
  for (std::size_t i = 0; i < args.size(); ++i) s += (i ? ", " : "") + args[i];
  s += ") ->";
  for (const auto& [name, value] : results) s += " " + name + "=" + value;
  s += " |";
  for (const auto& [name, value] : variables) s += " " + name + "=" + value;
  return s + "\n";
}

/// What the interpreter answers.
inline std::string play(const Case& c, std::vector<std::string>* errors = nullptr) {
  if (c.scenario) {
    logic_cases::Host host;
    logic::Runtime rt(host);
    rt.jit = false;
    rt.native = false;
    logic::Instance* inst = rt.add(c.graph, 0, c.name);
    return "== " + c.name + "\n" + logic_cases::play(rt, host, inst);
  }
  Host host;
  logic::Runtime rt(host);
  rt.jit = false;
  rt.native = false;  // (the interpreter, also in a binary that has the Blueprint as a class)
  logic::Instance* inst = rt.add(c.graph, 0, c.name);
  rt.begin();
  std::string text = "== " + c.name + "\n";
  for (const Call& call : c.calls) {
    const logic::Function* f = c.graph.function(call.function);
    std::vector<Value> out;
    if (!f || !inst->call(call.function, call.args, out)) host.errors.push_back(c.name + ": " + call.function + " could not be called");
    std::vector<std::string> args;
    std::vector<std::pair<std::string, std::string>> results, variables;
    for (const Value& a : call.args) args.push_back(native_show(a));
    for (std::size_t k = 0; f && k < f->outputs.size() && k < out.size(); ++k) results.push_back({f->outputs[k].name, native_show(out[k])});
    for (const logic::Variable& v : c.graph.variables) variables.push_back({v.name, native_show(inst->get(v.name))});
    text += line(call.function, args, results, variables);
  }
  if (errors) *errors = host.errors;
  return text;
}

inline std::string c_string(const std::string& s) {
  std::string r = "\"";
  char b[8];
  for (unsigned char ch : s) {
    if (ch == '"' || ch == '\\') { r += '\\'; r += static_cast<char>(ch); }
    else if (ch < 32 || ch > 126) { std::snprintf(b, sizeof b, "\\%03o", ch); r += b; }
    else r += static_cast<char>(ch);
  }
  return r + "\"";
}

inline const char* cpp_type(PinType t) {
  switch (t) {
    case PinType::boolean: return "bool";
    case PinType::integer: return "kern::i64";
    case PinType::number: return "kern::f64";
    case PinType::vector: return "kern::Vec3";
    case PinType::text: return "std::string";
    case PinType::actor: return "kern::logic::ActorHandle";
    case PinType::boolean_list: return "kern::List<bool>";
    case PinType::integer_list: return "kern::List<kern::logic::Whole>";
    case PinType::number_list: return "kern::List<kern::f64>";
    case PinType::vector_list: return "kern::List<kern::Vec3>";
    case PinType::text_list: return "kern::List<std::string>";
    case PinType::actor_list: return "kern::List<kern::logic::ActorHandle>";
    default: return "void";
  }
}

/// A value as C++ source, bit for bit.
inline std::string cpp_value(const Value& v) {
  char b[160];
  auto number = [&](f64 x, const char* suffix) -> std::string {
    if (std::isnan(x)) return std::string("std::numeric_limits<") + (*suffix ? "float" : "double") + ">::quiet_NaN()";
    if (std::isinf(x)) return std::string(x < 0 ? "-" : "") + "std::numeric_limits<" + (*suffix ? "float" : "double") + ">::infinity()";
    std::snprintf(b, sizeof b, "%a%s", x, suffix);
    return b;
  };
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer: std::snprintf(b, sizeof b, "%lldLL", static_cast<long long>(v.i)); return b;
    case PinType::number: return number(v.n, "");
    case PinType::vector: return "kern::Vec3{" + number(v.v.x, "f") + ", " + number(v.v.y, "f") + ", " + number(v.v.z, "f") + "}";
    case PinType::text: return "std::string(" + c_string(v.s) + ")";
    case PinType::actor: std::snprintf(b, sizeof b, "kern::logic::ActorHandle{%lluu}", static_cast<unsigned long long>(v.a)); return b;
    default: break;
  }
  if (!logic::is_list(v.type)) return "0";
  std::string r = std::string(cpp_type(v.type)) + "{";
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + cpp_value(v.item(k));
  return r + "}";
}

inline bool write_file(const std::string& path, const std::string& text) {
  FILE* f = std::fopen(path.c_str(), "wb");
  if (!f) return false;
  const bool ok = std::fwrite(text.data(), 1, text.size(), f) == text.size();
  return std::fclose(f) == 0 && ok;
}

/// Writes <dir>/bp_<Name>.h (the Blueprint's text, as it is, and its glue), blueprint_native_main.cpp
/// (the calls) and blueprint_native_expected.txt (the interpreter's answers).
inline bool emit(const std::string& dir, const std::vector<Case>& all, std::string* error) {
  std::string main = "// written by kern_tests --emit-native: the calls of the native Blueprint test\n"
                     "#include <cstdio>\n#include <fstream>\n#include <limits>\n#include <sstream>\n#include <string>\n\n#include \"logic_cases.h\"\n#include \"native_show.h\"\n";
  std::string runs, expected;
  for (const Case& c : all) {
    std::string text, glue, why;
    if (!logic::native_ready(c.graph, &why) || !logic::to_cpp(c.graph, c.name, text, &why) || !logic::native_glue(c.graph, c.name, glue, &why)) {
      if (error) *error = c.name + ": " + why;
      return false;
    }
    // the graph as the compiled test gets it (as a level file would hold it): the same Blueprint, the same hash
    const std::string stored = json::stringify(logic::graph_to_json(c.graph));
    logic::Graph again;
    json::Value parsed;
    if (!json::parse(stored, parsed) || !logic::graph_from_json(parsed, again) || logic::graph_hash(again) != logic::graph_hash(c.graph)) {
      if (error) *error = c.name + ": the graph is not the same after it was stored";
      return false;
    }
    if (stored.find(")KGRAPH\"") != std::string::npos) {
      if (error) *error = c.name + ": the graph contains the embedding's end mark";
      return false;
    }
    if (!write_file(dir + "/bp_" + c.name + ".h", text + "\n" + glue)) {
      if (error) *error = "cannot write into " + dir;
      return false;
    }
    std::vector<std::string> errors;
    expected += play(c, &errors);
    if (!errors.empty()) {
      if (error) *error = c.name + ": " + errors[0];
      return false;
    }
    main += "#include \"bp_" + c.name + ".h\"\n";
    std::string body = "static const char kGraph_" + c.name + "[] =";
    for (std::size_t at = 0; at < stored.size(); at += 8192) body += "\n    R\"KGRAPH(" + stored.substr(at, 8192) + ")KGRAPH\"";
    if (c.scenario) {
      // events and ticks need the runtime: the class behind a logic::Instance plays the scenario
      body += ";\n\nstatic void run_" + c.name + "(std::string& got, std::string& hosted) {\n"
              "  logic_cases::Host host;\n  kern::logic::Runtime rt(host);\n  rt.jit = false;\n  kern::logic::Graph graph;\n"
              "  std::string text = \"== " + c.name + "\\n\";\n"
              "  {\n    kern::json::Value stored;\n    if (!kern::json::parse(kGraph_" + c.name + ", stored) || !kern::logic::graph_from_json(stored, graph)) text += \"(the graph was not read)\\n\";\n  }\n"
              "  kern::logic::Instance* inst = rt.add(graph, 0, \"" + c.name + "\");\n"
              "  if (std::string(inst->tier()) != \"native C++\") text += std::string(\"(it runs in the \") + inst->tier() + \", not as its class)\\n\";\n"
              "  text += logic_cases::play(rt, host, inst);\n  got += text;\n  hosted += text;\n}\n\n";
      runs += body;
      continue;
    }
    body += ";\n\nstatic void run_" + c.name + "(std::string& got, std::string& hosted) {\n  " + c.name + " b;\n"
            "  NativeTestHost alone;  // (the class by itself: a host of its own - its Random nodes start from that host's number)\n  b.kern_world.host = &alone;\n"
            "  got += \"== " + c.name + "\\n\";\n  const auto variables = [&] {\n    return std::string(\" |\")";
    for (const logic::Variable& v : c.graph.variables) body += " + \" " + v.name + "=\" + show(b." + v.name + ")";
    body += ";\n  };\n"
            "  // the same Blueprint behind a logic::Instance: the engine's way to it, by name\n"
            "  NativeTestHost host;\n  kern::logic::Runtime rt(host);\n  rt.jit = false;\n  kern::logic::Graph graph;\n"
            "  {\n    kern::json::Value stored;\n    if (!kern::json::parse(kGraph_" + c.name + ", stored) || !kern::logic::graph_from_json(stored, graph)) hosted += \"(the graph was not read)\\n\";\n  }\n"
            "  kern::logic::Instance* inst = rt.add(graph, 0, \"" + c.name + "\");\n  rt.begin();\n  hosted += \"== " + c.name + "\\n\";\n"
            "  if (std::string(inst->tier()) != \"native C++\") hosted += std::string(\"(it runs in the \") + inst->tier() + \", not as its class)\\n\";\n"
            "  const auto hosted_variables = [&] {\n    return std::string(\" |\")";
    for (const logic::Variable& v : c.graph.variables) body += " + \" " + v.name + "=\" + native_show(inst->get(\"" + v.name + "\"))";
    body += ";\n  };\n";
    for (std::size_t at = 0; at < c.calls.size();) {
      std::size_t end = at;
      while (end < c.calls.size() && c.calls[end].function == c.calls[at].function) ++end;  // calls of one function in a row: one table
      const logic::Function& f = *c.graph.function(c.calls[at].function);
      body += "  {\n    const struct {";
      for (std::size_t i = 0; i < f.inputs.size(); ++i) body += std::string(" ") + cpp_type(f.inputs[i].type) + " a" + std::to_string(i) + ";";
      body += " int row; } rows[] = {\n";
      for (std::size_t r = at; r < end; ++r) {
        body += "        {";
        for (const Value& a : c.calls[r].args) body += cpp_value(a) + ", ";
        body += std::to_string(r) + "},\n";
      }
      body += "    };\n    for (const auto& r : rows) {\n      (void)r;\n      ";
      std::string call = "b." + f.name + "(";
      std::string shown = "std::string(\"" + f.name + "(\")";
      for (std::size_t i = 0; i < f.inputs.size(); ++i) {
        call += (i ? ", r.a" : "r.a") + std::to_string(i);
        shown += std::string(i ? " + \", \"" : "") + " + show(r.a" + std::to_string(i) + ")";
      }
      call += ")";
      body += (f.outputs.empty() ? "" : "const auto out = ") + call + ";\n      got += " + shown + " + \") ->\"";
      for (const logic::FunctionPin& o : f.outputs) body += " + \" " + o.name + "=\" + show(out." + o.name + ")";
      body += " + variables() + \"\\n\";\n";
      // ... and the same call by name
      body += "      std::vector<kern::logic::Value> results;\n      if (!inst->call(\"" + f.name + "\", {";
      for (std::size_t i = 0; i < f.inputs.size(); ++i) body += std::string(i ? ", " : "") + "kern::logic::to_value(r.a" + std::to_string(i) + ")";
      body += "}, results)) hosted += \"(no call) \";\n      hosted += " + shown + " + \") ->\"";
      for (std::size_t k = 0; k < f.outputs.size(); ++k)
        body += " + \" " + f.outputs[k].name + "=\" + native_show(results.size() > " + std::to_string(k) + " ? results[" + std::to_string(k) + "] : kern::logic::Value())";
      body += " + hosted_variables() + \"\\n\";\n    }\n  }\n";
      at = end;
    }
    runs += body + "}\n\n";
  }
  main += "\ntemplate <class T> static std::string show(const T& v) { return native_show(kern::logic::to_value(v)); }\n\n"
          "// The lines that are not the interpreter's (0: none).\n"
          "static int differs(const std::string& got, const std::string& expected, const char* what) {\n"
          "  if (got == expected) return 0;\n  std::size_t a = 0, b = 0, shown = 0;\n"
          "  while ((a < got.size() || b < expected.size()) && shown < 12) {\n"
          "    const std::size_t ae = got.find('\\n', a), be = expected.find('\\n', b);\n"
          "    const std::string x = a < got.size() ? got.substr(a, ae == std::string::npos ? ae : ae - a) : \"(nothing)\", y = b < expected.size() ? expected.substr(b, be == std::string::npos ? be : be - b) : \"(nothing)\";\n"
          "    if (x != y) { std::printf(\"FAIL %s: %s\\n     the interpreter: %s\\n\", what, x.c_str(), y.c_str()); ++shown; }\n"
          "    a = ae == std::string::npos ? got.size() : ae + 1;\n    b = be == std::string::npos ? expected.size() : be + 1;\n  }\n  return 1;\n}\n\n" + runs;
  main += "int main(int argc, char** argv) {\n  if (argc < 2) { std::fprintf(stderr, \"usage: blueprint_native_test <blueprint_native_expected.txt>\\n\"); return 2; }\n"
          "  std::ifstream in(argv[1]);\n  std::stringstream want;\n  want << in.rdbuf();\n  std::string got, hosted;\n";
  std::size_t count = 0, calls = 0;
  for (const Case& c : all) {
    main += "  run_" + c.name + "(got, hosted);\n";
    ++count;
    calls += c.calls.size();
  }
  main += "  const std::string expected = want.str();\n  int failures = 0;\n"
          "  failures += differs(got, expected, \"the compiled Blueprint\");\n"
          "  failures += differs(hosted, expected, \"its class behind logic::Instance\");\n";
  main += "  std::printf(\"%s blueprint_native_test (" + std::to_string(count) + " Blueprints as their C++, " + std::to_string(calls) +
          " calls on the class and by name through logic::Instance)\\n\", failures ? \"FAIL\" : \"ok  \");\n  return failures ? 1 : 0;\n}\n";
  if (!write_file(dir + "/blueprint_native_main.cpp", main) || !write_file(dir + "/blueprint_native_expected.txt", expected)) {
    if (error) *error = "cannot write into " + dir;
    return false;
  }
  return true;
}

}  // namespace blueprint_native
