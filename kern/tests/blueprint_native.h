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
#include "kern/logic/nodes.h"
#include "engine_case.gen.h"
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
  /// Calls made after the others with values as they come from outside (Instance::call): of any
  /// type, also none the function's inputs have, and fewer than it has inputs. The class is given
  /// them as the engine gives them (logic::native_arg).
  std::vector<Call> outside = {};
  /// The libraries it uses, with those they use (each a case of its own before it): the
  /// Blueprints of its project.
  std::vector<std::pair<std::string, logic::Graph>> libraries = {};
  logic::Blueprints blueprints() const {
    auto held = std::make_shared<std::vector<std::pair<std::string, logic::Graph>>>(libraries);
    return [held](std::string_view name) -> const logic::Graph* {
      for (const auto& [n, g] : *held) if (n == name) return &g;
      return nullptr;
    };
  }
};

// Places: a variable, and from it fields and list entries - read, set, added to and changed as a
// list where they stand, at every depth, and beside every list on the way (there stands an entry
// with the defaults that keeps nothing).
inline const char* kPlaces = R"kn(
struct Places : kern::Blueprint {
  KERN_ENUM(Side, Front, Rear);
  KERN_STRUCT(Wheel) {
    Float Slip = 0.5;
    Side At = Side::Front;
    List<Float> Marks = List<Float>{1.0};
  };
  KERN_STRUCT(Car) {
    Text Name = "car";
    Float Speed = 3.0;
    Int Laps = 2;
    Wheel Spare = Wheel{0.25, Side::Rear, List<Float>{}};
    List<Wheel> Wheels = List<Wheel>{};
    List<Int> Gears = List<Int>{1, 2};
    List<Text> Tags = List<Text>{"new"};
    List<Bool> Lights = List<Bool>{true};
  };

  List<Car> Cars = List<Car>{};
  Car Mine = Car{"mine", 5.0, 1, Wheel{0.125, Side::Front, List<Float>{2.0, 4.0}}, List<Wheel>{Wheel{0.75, Side::Rear, List<Float>{}}}, List<Int>{4}, List<Text>{"a"}, List<Bool>{}};
  List<Float> Scores = List<Float>{1.0, 2.5};
  List<Int> Counts = List<Int>{7};

  struct GrowResult { Int cars = 0; Int wheels = 0; };
  /// A car more, with so many wheels: Add to a list that stands in an entry of a list.
  GrowResult Grow(Int wheels = 2, Float slip = 0.5) {
    list_add(n3, Cars, Mine);
    for (auto it4 = flow_for(n4, 1, wheels); it4.next();) {
      list_add(Cars[n3.index].Wheels, struct_make<Wheel>(math_mul(slip, kern::to<Float>(n4.index)), Side::Rear, list_make<Float, "1">(slip)));
    }
    return {list_length<Car>(Cars), list_length<Wheel>(Cars[n3.index].Wheels)};
  }

  struct ReadResult { Float slip = 0.0; Float speed = 0.0; Int laps = 0; Text name = ""; Float mark = 0.0; Int gear = 0; Text tag = ""; Bool light = false; Side at = Side::Front; Wheel spare = Wheel{0.5, Side::Front, List<Float>{1.0}}; Int wheels = 0; Float score = 0.0; Float mine = 0.0; };
  /// What stands at a place - a number, a text, an enum's value, a struct, an entry of a list of
  /// every kind -, also beside every list on the way to it.
  KERN_PURE ReadResult Read(Int car = 0, Int wheel = 0, Int at = 0) {
    return {Cars[car].Wheels[wheel].Slip, Cars[car].Speed, Cars[car].Laps, Cars[car].Name, Cars[car].Wheels[wheel].Marks[at], Cars[car].Gears[at], Cars[car].Tags[at], Cars[car].Lights[at],
            Cars[car].Wheels[wheel].At, Cars[car].Spare, list_length<Wheel>(Cars[car].Wheels), Scores[at], Mine.Spare.Marks[at]};
  }

  struct SetResult { Float slip = 0.0; Float speed = 0.0; Int laps = 0; Int gear = 0; Float score = 0.0; Text name = ""; };
  /// Set and Add at a place: a field, a field of a field, an entry, a field of an entry of a list
  /// in an entry - and what each step gives.
  SetResult Set(Int car = 0, Int wheel = 0, Float value = 1.0) {
    n20.value = (Cars[car].Wheels[wheel].Slip = value);
    Cars[car].Spare.Slip = math_add(Cars[car].Spare.Slip, value);
    n23.value = (Cars[car].Name = text_append(Cars[car].Name, "!", ""));
    n24.value = (Cars[car].Speed += value);
    n25.value = (Cars[car].Laps += 3);
    n26.value = (Cars[car].Gears[wheel] = kern::to<Int>(value));
    Cars[car].Gears[wheel] += 1;
    Cars[car].Wheels[wheel].Marks[0] += 0.5;
    Cars[car].Wheels[wheel].At = Side::Front;
    Cars[car].Tags[0] = "set";
    Cars[car].Lights[0] = false;
    Cars[car].Spare = Cars[car].Wheels[wheel];
    Mine.Spare.Marks[1] = value;
    Mine.Speed += 1.0;
    Mine.Laps += 1;
    n35.value = (Scores[wheel] += value);
    Counts[car] += 2;
    Scores[car] = value;
    return {n20.value, n24.value, n25.value, n26.value, n35.value, n23.value};
  }

  struct EditResult { Int index = 0; Bool removed = false; Int marks = 0; Int tags = 0; Text shown = ""; };
  /// The steps that change a list, on a list at a place - at every index, also beside the lists
  /// on the way.
  EditResult Edit(Text what = "add", Int car = 0, Int wheel = 0, Int at = 0, Float value = 0.0) {
    switch (flow_switch_text<"add, insert, remove_at, remove, clear, whole, tags">(what)) {
      case 0: { list_add(n41, Cars[car].Wheels[wheel].Marks, value); } break;
      case 1: { list_insert(Cars[car].Wheels[wheel].Marks, at, value); } break;
      case 2: { list_remove_at(Cars[car].Wheels[wheel].Marks, at); } break;
      case 3: { list_remove(n44, Cars[car].Wheels[wheel].Marks, value); } break;
      case 4: { list_clear(Cars[car].Wheels); } break;
      case 5: { Cars[car].Wheels[wheel] = Mine.Spare; } break;
      case 6: {
        list_add(Cars[car].Tags, kern::to<Text>(value));
        list_add(Mine.Tags, "m");
        list_insert(Mine.Spare.Marks, 0, value);
      } break;
    }
    return {n41.index, n44.removed, list_length<Float>(Cars[car].Wheels[wheel].Marks), list_length<Text>(Cars[car].Tags), kern::to<Text>(Cars[car].Wheels[wheel].Marks)};
  }

  struct LocalResult { Float slip = 0.0; Int wheels = 0; Text name = ""; };
  /// A local variable of a function is a place like any other.
  LocalResult Local(Float slip = 1.0, Int at = 0) {
    Car Work = Car{"work", 1.0, 0, Wheel{0.5, Side::Front, List<Float>{1.0}}, List<Wheel>{}, List<Int>{1, 2}, List<Text>{"new"}, List<Bool>{true}};
    list_add(Work.Wheels, struct_make<Wheel>(slip, Side::Rear, List<Float>{}));
    Work.Wheels[at].Slip += slip;
    Work.Spare.Marks[0] = slip;
    Work.Name = text_append(Work.Name, kern::to<Text>(Work.Wheels[at].Slip), "");
    return {Work.Wheels[at].Slip, list_length<Wheel>(Work.Wheels), Work.Name};
  }

  struct { Int index = 0; } n3;
  struct : kern::Node<4> { Int index = 0; } n4;
  struct { Float value = 0.0; } n20;
  struct { Text value = ""; } n23;
  struct { Float value = 0.0; } n24;
  struct { Int value = 0; } n25;
  struct { Int value = 0; } n26;
  struct { Float value = 0.0; } n35;
  struct { Int index = 0; } n41;
  struct { Bool removed = false; } n44;
};
)kn";

// Libraries: Blueprints that keep nothing - types, and functions - and a Blueprint that uses
// them: it has their types and calls their functions directly, with values and with references
// (an entry of its own list, also beside it). Each function is there once: in the class the
// library is a base, in the interpreter the Blueprint runs as one graph with its libraries.
inline const char* kParts = R"kn(
struct Parts : virtual kern::Library {
  KERN_ENUM(Side, Front, Rear);
  KERN_STRUCT(Wheel) {
    Float Slip = 0.5;
    Side At = Side::Front;
    List<Float> Marks = List<Float>{};
  };

  struct flippedResult { Side side = Side::Front; };
  KERN_PURE flippedResult flipped(Side side = Side::Front) {
    return {logic_select<Side>(enum_equal<Side>(side, Side::Front), Side::Rear, Side::Front)};
  }
};
)kn";
inline const char* kWorkshop = R"kn(
struct Workshop : virtual kern::Library, virtual Parts {
  /// Changes the wheel the call names.
  void skid(Wheel& wheel, Float by = 0.5) {
    wheel.Slip += by;
    list_add(wheel.Marks, wheel.Slip);
    wheel.At = Parts::flipped(wheel.At).side;
  }
  struct wearResult { Float slip = 0.0; Int marks = 0; };
  KERN_PURE wearResult wear(const Wheel& wheel) {
    return {wheel.Slip, list_length<Float>(wheel.Marks)};
  }
  struct twiceResult { Float slip = 0.0; Int passes = 0; };
  /// A function of the library that calls its own, and loops.
  twiceResult twice(Wheel& wheel, Int times = 2) {
    Int Passes = 0;
    for (auto it10 = flow_for(n10, 1, times); it10.next();) {
      skid(wheel, 0.25);
      Passes += 1;
    }
    return {wear(wheel).slip, Passes};
  }
  void fit(List<Wheel>& wheels, const Wheel& like, Int count = 1) {
    for (auto it20 = flow_for(n20, 1, count); it20.next();) {
      list_add(wheels, like);
    }
  }

  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<20> { Int index = 0; } n20;
};
)kn";
inline const char* kGarage = R"kn(
struct Garage : virtual kern::Blueprint, virtual Workshop, virtual Parts {
  KERN_STRUCT(Car) {
    Text Name = "car";
    List<Wheel> Wheels = List<Wheel>{};
  };

  List<Car> Cars = List<Car>{Car{"a", List<Wheel>{Wheel{0.25, Side::Rear, List<Float>{}}}}};
  Wheel Spare = Wheel{1.0, Side::Front, List<Float>{}};
  Float Seen = 0.0;

  struct RunResult { Float slip = 0.0; Int marks = 0; Side at = Side::Front; Int passes = 0; Int wheels = 0; Side flipped = Side::Front; };
  /// Calls of the libraries' functions: a place of its own as a reference - a variable, an entry
  /// of a list in an entry of a list, also beside the lists -, a value read of one.
  RunResult Run(Int car = 0, Int wheel = 0, Float by = 1.0) {
    Workshop::skid(Cars[car].Wheels[wheel], by);
    n5 = Workshop::twice(Cars[car].Wheels[wheel], 3);
    Seen = n5.slip;
    Workshop::skid(Spare, by);
    Workshop::fit(Cars[car].Wheels, Spare, 2);
    return {Workshop::wear(Cars[car].Wheels[wheel]).slip, Workshop::wear(Cars[car].Wheels[wheel]).marks, Cars[car].Wheels[wheel].At, n5.passes, list_length<Wheel>(Cars[car].Wheels),
            Parts::flipped(Spare.At).side};
  }

  Workshop::twiceResult n5;
};
)kn";

// References: an input of a function that is a place of the caller - changed where it stands
// (Car& car) or read there (const Car& car), nothing copied. A number, a Bool, a struct, a list;
// a field and a list entry of what the function was given, handed on to other functions; a local
// variable and an input by value as the place. An entry beside its list is one of its own: two
// of them are two, and naming another changes nothing of the one a function has.
inline const char* kReferences = R"kn(
struct References : kern::Blueprint {
  KERN_STRUCT(Wheel) {
    Float Slip = 0.5;
    List<Float> Marks = List<Float>{};
  };
  KERN_STRUCT(Car) {
    Text Name = "car";
    Float Speed = 3.0;
    Int Laps = 0;
    List<Wheel> Wheels = List<Wheel>{};
  };

  List<Car> Cars = List<Car>{Car{"a", 1.0, 0, List<Wheel>{Wheel{0.25, List<Float>{}}, Wheel{0.5, List<Float>{1.0}}}}, Car{"b", 2.0, 5, List<Wheel>{}}};
  Car Mine = Car{"mine", 5.0, 1, List<Wheel>{Wheel{0.75, List<Float>{}}}};
  Float Total = 0.0;
  Int Count = 0;
  List<Float> Scores = List<Float>{1.0, 2.0};
  List<Bool> Flags = List<Bool>{true, false};

  /// A reference to a number: changed where the caller has it.
  void Bump(Float& x, Float by = 1.0) {
    x += by;
  }
  void Tick(Int& n) {
    n += 1;
  }
  /// A reference to a struct: its fields are places.
  void Skid(Wheel& wheel, Float by = 0.5) {
    wheel.Slip += by;
    list_add(wheel.Marks, wheel.Slip);
    Bump(wheel.Slip, 0.125);
  }
  struct DriveResult { Float speed = 0.0; Int wheels = 0; };
  /// A reference handed on: a field, an entry of a list in what the function was given.
  DriveResult Drive(Car& car, Int wheel = 0, Float by = 1.0) {
    car.Speed += by;
    Tick(car.Laps);
    car.Name = text_append(car.Name, "+", "");
    Skid(car.Wheels[wheel], by);
    Bump(car.Speed, 0.5);
    return {car.Speed, list_length<Wheel>(car.Wheels)};
  }
  struct WearResult { Float slip = 0.0; Int marks = 0; Text name = ""; };
  /// A const reference: read where it stands - also by a function that is read as a value.
  KERN_PURE WearResult Wear(const Car& car, Int wheel = 0) {
    return {car.Wheels[wheel].Slip, list_length<Float>(car.Wheels[wheel].Marks), car.Name};
  }
  /// A list by reference: the steps that change a list work on it.
  void Fill(List<Float>& items, Int count = 2, Float value = 1.5) {
    for (auto it30 = flow_for(n30, 1, count); it30.next();) {
      list_add(items, math_mul(value, kern::to<Float>(n30.index)));
    }
    items[0] = -1.0;
  }

  /// A reference to a Bool: an entry of a list of them is a place like any other.
  void Toggle(Bool& flag) {
    flag = logic_not(flag);
  }
  /// Two places at once. Beside a list each is an entry of its own: what goes to one is not in
  /// the other.
  void Trade(Wheel& a, Wheel& b, Float by = 1.0) {
    a.Slip += by;
    b.Slip += math_mul(a.Slip, 2.0);
    list_add(a.Marks, b.Slip);
  }
  struct MixResult { Float sum = 0.0; Float slip = 0.0; };
  /// What a function was given stays what it is while other places are named - beside a list
  /// too, and in the same statement, in whatever order it is worked out.
  MixResult Mix(Wheel& wheel, Int car = 0, Int at = 0, Float by = 1.0) {
    wheel.Slip += by;
    return {math_add(Cars[car].Wheels[at].Slip, wheel.Slip), wheel.Slip};
  }
  struct LitResult { Int on = 0; Int found = 0; Bool first = false; Text shown = ""; };
  /// A list of Bools: the steps on it, a loop over it, its entries as values.
  LitResult Lit(Bool add = true, Int at = 0) {
    Int On = 0;
    list_add(Flags, add);
    Toggle(Flags[at]);
    Flags[0] = logic_and(Flags[at], add);
    for (auto it50 = flow_for_each<Bool>(n50, Flags); it50.next();) {
      On += kern::to<Int>(n50.item);
    }
    return {On, list_find<Bool>(Flags, add), Flags[0], kern::to<Text>(Flags[at])};
  }

  struct RunResult { Float speed = 0.0; Int wheels = 0; Float slip = 0.0; Int marks = 0; Text name = ""; Float total = 0.0; Int scores = 0; Float mine = 0.0; Int count = 0; Float sum = 0.0; Float mixed = 0.0; Bool flag = false; };
  /// Calls with places: an entry of a list (also beside it), a variable, a field, a number in a
  /// list; what a function read as a value reads of a place.
  RunResult Run(Int car = 0, Int wheel = 0, Float by = 1.0) {
    n40 = Drive(Cars[car], wheel, by);
    Drive(Mine, 0, by);
    Bump(Total, by);
    Tick(Count);
    Bump(Cars[car].Wheels[wheel].Slip, by);
    Bump(Scores[wheel], by);
    Fill(Scores, 2, by);
    Fill(Cars[car].Wheels[wheel].Marks, 1, by);
    Toggle(Flags[wheel]);
    Toggle(Flags[car]);
    Trade(Cars[car].Wheels[wheel], Cars[wheel].Wheels[car], by);
    n41 = Mix(Cars[car].Wheels[wheel], wheel, car, by);
    return {n40.speed, n40.wheels, Wear(Cars[car], wheel).slip, Wear(Cars[car], wheel).marks, Wear(Mine, 0).name, Total, list_length<Float>(Scores), Mine.Speed, Count, n41.sum, n41.slip, Flags[wheel]};
  }
  struct LocalResult { Float speed = 0.0; Float slip = 0.0; Text name = ""; Float first = 0.0; };
  /// A local variable as the place a call names; an input by value read at a place and handed on
  /// as a const reference.
  LocalResult Local(Car given = Car{"given", 1.0, 0, List<Wheel>{Wheel{0.5, List<Float>{}}}}, Float by = 1.0) {
    Car Work = Car{"work", 0.0, 0, List<Wheel>{}};
    Work = given;
    Drive(Work, 0, by);
    return {Work.Speed, Wear(given, 0).slip, Wear(Work, 0).name, given.Wheels[0].Slip};
  }

  struct : kern::Node<30> { Int index = 0; } n30;
  DriveResult n40;
  MixResult n41;
  struct : kern::Node<50> { Bool item = false; Int index = 0; } n50;
};
)kn";

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

// Local variables of functions: every call starts with their defaults; two functions have one of
// the same name; numbers, a text, a list, a struct; a pure function sets its own; a call that is
// interrupted by another call of the same function has its own back.
inline const char* kLocals = R"kn(
struct Locals : kern::Blueprint {
  KERN_STRUCT(Hit) { Text Who = ""; Float Damage = 1.0; };

  Int Calls = 0;
  Float Kept = 0.0;

  struct SumToResult { Float sum = 0.0; Int steps = 0; };
  /// Numbers: Add and a For loop; what a call left is gone with the next.
  SumToResult SumTo(Int n = 3) {
    Float Total = 0.5;
    Int Steps = 0;
    for (auto it10 = flow_for(n10, 1, n); it10.next();) {
      Total += kern::to<Float>(n10.index);
      Steps += 1;
    }
    Calls += Steps;
    return {Total, Steps};
  }

  struct JoinResult { Text text = ""; Int count = 0; Float damage = 0.0; };
  /// Another function's Total is a text; a list (Add to List), a struct (Set Field).
  JoinResult Join(Text a = "a", Text b = "b", Float d = 2.0) {
    Text Total = "<";
    List<Text> Seen = List<Text>{"x"};
    Hit Last = Hit{"none", 3.0};
    Total = text_append(Total, a, b);
    list_add(Seen, a);
    list_add(Seen, b);
    Last.Damage = math_add(struct_break<Hit>(Last).Damage, d);
    Last.Who = Total;
    return {text_append(Total, struct_break<Hit>(Last).Who, ">"), list_length<Text>(Seen), struct_break<Hit>(Last).Damage};
  }

  struct LimitResult { Float out = 0.0; };
  /// A function read as a value sets its own local variables.
  KERN_PURE LimitResult Limit(Float x = 0.0) {
    Float Out = -1.0;
    Out = x;
    if (math_compare<">">(x, 1.0)) {
      Out = 1.0;
    }
    return {Out};
  }

  struct OuterResult { Float mine = 0.0; Float limited = 0.0; };
  /// It raises an event whose handler calls it again: this call's Mine is its own afterwards.
  OuterResult Outer(Float x = 1.0, Bool again = false) {
    Float Mine = 0.25;
    Mine += math_mul(x, 10.0);
    if (again) {
      flow_call<"Again">(Actor{0}, Actor{0}, 5.0, "");
    }
    return {Mine, Limit(Mine).out};
  }

  KERN_EVENT(on40, event_custom<"Again">)
  void on40(Actor actor, Float value, Text text) {
    n41 = Outer(value, false);
    Kept = n41.mine;
  }

  struct : kern::Node<10> { Int index = 0; } n10;
  OuterResult n41;
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
    return {logic_select<Float>(first, 1.5, -2.5), logic_select<Int>(first, 7, -7), logic_select<Bool>(first, true, false),
            value_reroute<Vector>(logic_select<Vector>(first, Vector{1.0, 2.0, 3.0}, Vector{-1.0, 0.5, 0.25})), value_reroute<Text>(logic_select<Text>(first, "a \"word\"", "tab\tand\\slash")),
            value_reroute<Actor>(logic_select<Actor>(first, who, Actor{7}))};
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
    return {n3.index, list_length<Float>(Scores), kern::to<Text>(Scores)};
  }

  struct EditResult { Bool removed = false; Int length = 0; Text shown = ""; };
  /// The steps that change a list variable, at every index - also far beyond the list.
  EditResult Edit(Text what = "insert", Int index = 0, Float value = 0.0) {
    switch (flow_switch_text<"insert, set, remove_at, remove, clear">(what)) {
      case 0: { list_insert(Scores, index, value); } break;
      case 1: { Scores[index] = value; } break;
      case 2: { list_remove_at(Scores, index); } break;
      case 3: { list_remove(n14, Scores, value); } break;
      case 4: { list_clear(Scores); } break;
    }
    return {n14.removed, list_length<Float>(Scores), kern::to<Text>(Scores)};
  }

  struct SumResult { Float total = 0.0; Int passes = 0; Int last = 0; };
  /// For Each, one inside the other; the index read after the loop.
  SumResult Sum() {
    Total = 0.0;
    for (auto it20 = flow_for_each<Float>(n20, Scores); it20.next();) {
      Total += n20.item;
      for (auto it22 = flow_for_each<Text>(n22, Names); it22.next();) {
        Shown = text_append(Shown, n22.item, kern::to<Text>(n22.index));
      }
    }
    return {Total, list_length<Text>(Names), n20.index};
  }

  struct AtResult { Float score = 0.0; Text name = ""; Int count = 0; Bool flag = false; Vector point = Vector{0.0, 0.0, 0.0}; Actor member = Actor{0}; Int where = 0; Bool has = false; };
  /// Get Item of every type (beyond the list: its zero), Find and Contains.
  KERN_PURE AtResult At(Int index = 0, Text name = "a") {
    return {list_get<Float>(Scores, index), list_get<Text>(Names, index), list_get<Int>(Counts, index), list_get<Bool>(Flags, index), list_get<Vector>(Points, index),
            list_get<Actor>(Team, index), list_find<Text>(Names, name), list_contains<Text>(Names, name)};
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
    Counts[0] = -7;
    list_remove(Flags, false);
    return {Counts, Flags, Points, Team};
  }

  struct TakeResult { Int length = 0; List<Float> twice = List<Float>{}; List<Float> picked = List<Float>{}; };
  /// A function takes a list (with a default); Set of a list variable; Make List; Select between lists.
  TakeResult Take(List<Float> values = List<Float>{3.0, 4.0, 3.0}, Bool pick = true) {
    Scores = values;
    return {list_length<Float>(values), list_make<Float, "3">(list_get<Float>(values, 0), list_get<Float>(values, 0), 9.5),
            logic_select<List<Float>>(pick, Scores, value_reroute<List<Float>>(List<Float>{-1.0}))};
  }

  struct GrowResult { Int passes = 0; Int length = 0; };
  /// The loop goes through the list as it was when it started, whatever its body does to the variable.
  GrowResult Grow() {
    Total = 0.0;
    for (auto it60 = flow_for_each<Float>(n60, Scores); it60.next();) {
      list_add(Scores, math_add(n60.item, 100.0));
      list_remove_at(Scores, 0);
      Total += 1.0;
    }
    return {kern::to<Int>(Total), list_length<Float>(Scores)};
  }

  struct { Int index = 0; } n3;
  struct { Bool removed = false; } n14;
  struct : kern::Node<20> { Float item = 0.0; Int index = 0; } n20;
  struct : kern::Node<22> { Text item = ""; Int index = 0; } n22;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<60> { Float item = 0.0; Int index = 0; } n60;
};
)kn";

// Enums: a type of the Blueprint's own with named values - variables and lists of it, Switch on
// Enum, to and from an index and a name, an enum's value and a list of them through functions.
inline const char* kEnums = R"kn(
struct Enums : kern::Blueprint {
  /// What the door is doing.
  KERN_ENUM(Mode, Closed, Opening, Open, Closing);
  KERN_ENUM(One, Only);

  Mode State = Mode::Closed;
  Mode Wanted = Mode::Open;
  List<Mode> Seen = List<Mode>{Mode::Closed};
  One Single = One::Only;
  Text Log = "";
  Int Steps = 0;

  struct NextResult { Mode state = Mode::Closed; Text name = ""; Int index = 0; };
  /// Switch on an enum, one case for each value; an enum's value set, read, shown as text.
  NextResult Next() {
    switch (flow_switch_enum<Mode>(State)) {
      case Mode::Closed: {
        State = Mode::Opening;
      } break;
      case Mode::Opening: {
        State = Mode::Open;
      } break;
      case Mode::Open: {
        State = Mode::Closing;
      } break;
      case Mode::Closing: {
        State = Mode::Closed;
      } break;
    }
    list_add(Seen, State);
    Steps += 1;
    if (enum_equal<Mode>(State, Wanted)) {
      Log = text_append(Log, "wanted:", enum_name<Mode>(State));
    }
    return {State, enum_name<Mode>(State), enum_index<Mode>(State)};
  }

  struct FromResult { Mode byIndex = Mode::Closed; Bool valid = false; Mode byName = Mode::Closed; Bool found = false; Bool same = false; };
  /// Index to Enum and Text to Enum: what names no value is the first one, and says so.
  KERN_PURE FromResult From(Int index = 0, Text name = "Open") {
    return {enum_from_index<Mode>(index).value, enum_from_index<Mode>(index).valid, enum_from_name<Mode>(name).value, enum_from_name<Mode>(name).found,
            enum_equal<Mode>(enum_from_index<Mode>(index).value, enum_from_name<Mode>(name).value)};
  }

  struct TakeResult { Mode picked = Mode::Closing; List<Mode> all = List<Mode>{}; Int count = 0; Bool has = false; Mode third = Mode::Closed; };
  /// A function takes an enum's value and a list of them (with defaults) and gives them back;
  /// Select and the list nodes in the enum's type.
  TakeResult Take(Mode want = Mode::Open, List<Mode> some = List<Mode>{Mode::Closing, Mode::Open}, Bool pick = true) {
    Wanted = want;
    Seen = some;
    return {logic_select<Mode>(pick, want, list_get<Mode>(some, 0)), enum_values<Mode>(), list_length<Mode>(some), list_contains<Mode>(some, want), list_get<Mode>(list_make<Mode, "2">(want, Mode::Opening), 2)};
  }

  struct WalkResult { Text names = ""; Int last = 0; };
  /// For Each over all values of an enum; an enum with one value has one way on.
  WalkResult Walk() {
    Log = "";
    for (auto it40 = flow_for_each<Mode>(n40, enum_values<Mode>()); it40.next();) {
      Log = text_append(Log, enum_name<Mode>(n40.item), kern::to<Text>(enum_index<Mode>(n40.item)));
    }
    switch (flow_switch_enum<One>(Single)) {
      case One::Only: {
        Log = text_append(Log, "/", enum_name<One>(Single));
      } break;
    }
    return {Log, n40.index};
  }

  struct : kern::Node<40> { Mode item = Mode::Closed; Int index = 0; } n40;
};
)kn";

// Structs: a type of the Blueprint's own with named fields - an enum's, another struct's, lists
// of them. Make, Break and Set Field; variables and lists of a struct; functions that take and
// give them; the list nodes compare structs field by field.
inline const char* kStructs = R"kn(
struct Structs : kern::Blueprint {
  KERN_ENUM(Kind, Melee, Ranged, Magic);
  /// Where something is aimed.
  KERN_STRUCT(Aim) {
    Vector At = Vector{0.0, 0.0, 1.0};
    Int Tries = 3;
  };
  /// What a weapon does.
  KERN_STRUCT(Weapon) {
    Text Name = "Fist";
    Kind Type = Kind::Melee;
    Float Damage = 10.0;
    List<Float> Falloff = List<Float>{1.0, 0.5};
    Aim Sight = Aim{Vector{0.0, 0.0, 1.0}, 3};
    List<Aim> Marks = List<Aim>{};
    Bool Ready = true;
    Actor Owner = Actor{0};
    List<Kind> Also = List<Kind>{Kind::Magic};
  };
  KERN_STRUCT(Solo) {
    Float Only = 0.5;
  };

  Weapon Held = Weapon{"Knife", Kind::Melee, 25.0, List<Float>{}, Aim{Vector{1.0, 0.0, 0.0}, 1}, List<Aim>{}, true, Actor{0}, List<Kind>{}};
  List<Weapon> Rack = List<Weapon>{};
  Aim Last = Aim{Vector{0.0, 0.0, 1.0}, 3};
  Solo One = Solo{0.5};
  List<Aim> Seen = List<Aim>{Aim{Vector{0.0, 2.0, 0.0}, 7}};
  Int Count = 0;
  Text Log = "";

  struct ForgeResult { Weapon made = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; Aim sight = Aim{Vector{0.0, 0.0, 1.0}, 3}; Solo one = Solo{0.5}; };
  /// Make: a struct from a value for each field - a struct and lists inside it.
  KERN_PURE ForgeResult Forge(Text name = "Bow", Kind kind = Kind::Ranged, Float damage = 12.0, Vector at = Vector{0.0, 1.0, 0.0}, Actor who = Actor{0}) {
    return {struct_make<Weapon>(name, kind, damage, list_make<Float, "2">(damage, math_div(damage, 2.0)), struct_make<Aim>(at, kern::to<Int>(damage)), list_make<Aim, "2">(struct_make<Aim>(at, 1), Last), false, who,
                                list_make<Kind, "1">(kind)),
            struct_make<Aim>(vector_scale(at, damage), 2), struct_make<Solo>(damage)};
  }

  struct TakeResult { Text name = ""; Kind type = Kind::Melee; Int count = 0; Bool has = false; Int at = 0; Weapon first = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; Aim beyond = Aim{Vector{0.0, 0.0, 1.0}, 3}; Float only = 0.0; };
  /// A function takes a struct and a list of them (with defaults); Break; the list nodes of a
  /// struct's type - an item from beyond the list is the struct's default.
  TakeResult Take(Weapon w = Weapon{"Club", Kind::Melee, 7.5, List<Float>{}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{}}, List<Weapon> more = List<Weapon>{}, Solo one = Solo{2.5}) {
    Held = w;
    Rack = more;
    One = one;
    return {struct_break<Weapon>(w).Name, struct_break<Weapon>(w).Type, list_length<Weapon>(more), list_contains<Weapon>(more, w), list_find<Weapon>(more, w), list_get<Weapon>(more, 0),
            struct_break<Weapon>(list_get<Weapon>(more, 5)).Sight, struct_break<Solo>(One).Only};
  }

  struct LookupResult { Bool found = false; Text name = ""; Int at = 0; Float damage = 0.0; Bool byKind = false; Int kindAt = 0; Int strong = 0; };
  /// Find Row: a list of structs read as a table - the first row whose field is the value, by a
  /// text, by an enum's value, by a number; not found gives the struct's defaults and -1.
  KERN_PURE LookupResult Lookup(Text name = "Bow", Kind kind = Kind::Ranged, Float damage = 12.0) {
    return {list_find_row<&Weapon::Name>(Rack, name).found, struct_break<Weapon>(list_find_row<&Weapon::Name>(Rack, name).row).Name, list_find_row<&Weapon::Name>(Rack, name).index,
            struct_break<Weapon>(list_find_row<&Weapon::Name>(Rack, name).row).Damage, list_find_row<&Weapon::Type>(Rack, kind).found, list_find_row<&Weapon::Type>(Rack, kind).index,
            list_find_row<&Weapon::Damage>(Rack, damage).index};
  }

  struct TuneResult { Weapon held = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; Float was = 0.0; Int rack = 0; Int tries = 0; };
  /// Set Field: one field of a struct variable changes, the others stay - a number, a text, an
  /// enum's value, a struct, a list; what the step gives is read.
  TuneResult Tune(Float damage = 1.0, Text name = "x", Aim sight = Aim{Vector{0.0, 0.0, 1.0}, 3}, Kind kind = Kind::Magic) {
    n30.value = (Held.Damage = math_add(struct_break<Weapon>(Held).Damage, damage));
    Held.Name = name;
    Held.Sight = sight;
    Held.Type = kind;
    Held.Falloff = list_make<Float, "1">(n30.value);
    Held.Marks = Seen;
    Held.Also = list_make<Kind, "2">(kind, struct_break<Weapon>(Held).Type);
    One.Only = n30.value;
    n38.value = (Last.Tries = kern::to<Int>(damage));
    list_add(Rack, Held);
    return {Held, n30.value, list_length<Weapon>(Rack), n38.value};
  }

  struct EditResult { Int count = 0; Bool removed = false; Weapon first = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; Int index = 0; };
  /// The steps that change a list variable, on a list of structs; Remove Item takes out what
  /// equals the struct - every field of it.
  EditResult Edit(Text what = "insert", Int at = 0, Weapon w = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}) {
    switch (flow_switch_text<"insert, set, remove_at, remove, clear, add">(what)) {
      case 0: { list_insert(Rack, at, w); } break;
      case 1: { Rack[at] = w; } break;
      case 2: { list_remove_at(Rack, at); } break;
      case 3: { list_remove(n44, Rack, w); } break;
      case 4: { list_clear(Rack); } break;
      case 5: { list_add(n46, Rack, w); } break;
    }
    return {list_length<Weapon>(Rack), n44.removed, list_get<Weapon>(Rack, 0), n46.index};
  }

  struct WalkResult { Float total = 0.0; Text names = ""; Aim last = Aim{Vector{0.0, 0.0, 1.0}, 3}; Weapon picked = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; };
  /// For Each over a list of structs; Select and Reroute of a struct's type.
  WalkResult Walk(Bool pick = true) {
    Log = "";
    Count = 0;
    for (auto it60 = flow_for_each<Weapon>(n60, Rack); it60.next();) {
      Log = text_append(Log, struct_break<Weapon>(n60.item).Name, "/");
      Count += struct_break<Aim>(struct_break<Weapon>(n60.item).Sight).Tries;
      Last = struct_break<Weapon>(n60.item).Sight;
      for (auto it64 = flow_for_each<Aim>(n64, struct_break<Weapon>(n60.item).Marks); it64.next();) {
        Count += struct_break<Aim>(n64.item).Tries;
      }
    }
    return {kern::to<Float>(Count), Log, Last, logic_select<Weapon>(pick, Held, value_reroute<Weapon>(list_get<Weapon>(Rack, 1)))};
  }

  struct { Float value = 0.0; } n30;
  struct { Int value = 0; } n38;
  struct { Bool removed = false; } n44;
  struct { Int index = 0; } n46;
  struct : kern::Node<60> { Weapon item = Weapon{"Fist", Kind::Melee, 10.0, List<Float>{1.0, 0.5}, Aim{Vector{0.0, 0.0, 1.0}, 3}, List<Aim>{}, true, Actor{0}, List<Kind>{Kind::Magic}}; Int index = 0; } n60;
  struct : kern::Node<64> { Aim item = Aim{Vector{0.0, 0.0, 1.0}, 3}; Int index = 0; } n64;
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

/// The values a pin of a type is tried with (`count`: how many values its enum has, for an enum's).
inline std::vector<Value> samples(PinType t, std::size_t count = 0) {
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
    case PinType::enumeration:
      for (std::size_t k = 0; k < count; ++k) r.push_back(Value::enumeration(static_cast<i64>(k)));
      break;
    default: break;
  }
  if (logic::is_list(t)) {
    // lists of the item type's samples: none, one, all of them, some of them twice
    const std::vector<Value> items = samples(logic::element_of(t), count);
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

/// The values a pin of a type of the Blueprint `g` is tried with. A struct: its default, and for
/// each field two more with that field at other values.
inline std::vector<Value> samples(const logic::TypeRef& t, const logic::Graph& g) {
  if (logic::element_of(t.type) != PinType::structure) {
    const logic::EnumType* e = g.enumeration(t.of);
    return samples(t.type, e ? e->values.size() : 0);
  }
  std::vector<Value> items;
  if (const logic::StructType* s = g.structure(t.of)) {
    const Value plain = logic::default_of(*s, g);
    items.push_back(plain);
    for (std::size_t k = 0; k < s->fields.size(); ++k) {
      const std::vector<Value> of_field = samples(s->fields[k].type, g);
      for (std::size_t pick : {std::size_t{1}, of_field.size() - 1}) {
        Value changed = plain;
        if (!of_field.empty()) logic::structs::set_field(changed, k, of_field[pick % of_field.size()]);
        items.push_back(std::move(changed));
      }
    }
  }
  if (t.type == PinType::structure) return items;
  auto list = [&](std::vector<std::size_t> picks) {
    std::vector<Value> rows;
    for (std::size_t k : picks) rows.push_back(items[k % items.size()]);
    return Value::structures(std::move(rows));
  };
  std::vector<std::size_t> all;
  for (std::size_t k = 0; k < items.size(); ++k) all.push_back(k);
  return {list({}), list({1}), list(all), list({1, 0, 1, 2, 1}), list({2, 3, 4, 5, 3, 2})};
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
    f.inputs.push_back({name, p.type, p.def.is_null() ? logic::zero_of({p.type, p.of}, g).to_json(g, p.of) : p.def, p.of});
    g.links.push_back({entry.id, name, node.id, p.id});
  }
  for (const logic::PinDef& p : out) {
    if (p.type == PinType::exec) continue;
    const std::string name = "o" + std::to_string(f.outputs.size());
    f.outputs.push_back({name, p.type, logic::zero_of({p.type, p.of}, g).to_json(g, p.of), p.of});
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
  // (an enum and structs of its own: the generic nodes in their types, the enum nodes, Make and Break)
  {
    json::Value types;
    if (!json::parse(R"({"enums":[{"name":"Season","values":["Spring","Summer","Autumn","Winter"],"help":"The four seasons."}],
      "structs":[{"name":"Spot","fields":[{"name":"At","type":"vector","value":[0,0.5,-1]},{"name":"Label","type":"text","value":"here"}],"help":"A place."},
                 {"name":"Day","fields":[{"name":"When","type":"enum","of":"Season","value":1},{"name":"Warmth","type":"float","value":20.5},
                                         {"name":"Where","type":"struct","of":"Spot","value":{"Label":"home"}},{"name":"Hours","type":"int_list","value":[6,22]},
                                         {"name":"Stops","type":"struct_list","of":"Spot","value":[{"At":[1,2,3]}]},{"name":"Dry","type":"bool","value":true},
                                         {"name":"Guide","type":"actor","value":0},{"name":"Around","type":"enum_list","of":"Season","value":[0,3]}],"help":"A day of a season."},
                 {"name":"Lone","fields":[{"name":"Only","type":"int","value":4}]}],"nodes":[],"links":[]})",
                     types) ||
        !logic::graph_from_json(types, c.graph))
      std::printf("blueprint_native Nodes: its types were not read\n");
  }
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
    } else if (d->params.size() == 1 && d->params[0].kind == logic::ParamDef::Kind::type) {
      // (a generic node in every type it can have; an enum node for the enum, Make and Break for every struct)
      variants.clear();
      for (const logic::TypeRef& t : logic::type_choices(d->params[0], c.graph)) variants.push_back({{d->params[0].key, logic::type_text(t)}});
    } else if (d->type == "list.make") {
      // (a list of every item type, and lists of no, one and many items)
      variants.clear();
      for (const logic::TypeRef& t : logic::type_choices(d->params[0], c.graph)) variants.push_back({{"type", logic::type_text(t)}, {"count", "3"}});
      for (const char* count : {"0", "1", "16"}) variants.push_back({{"type", "int"}, {"count", count}});
    }
    for (const auto& params : variants) {
      logic::Graph alone;
      alone.enums = c.graph.enums;
      alone.structs = c.graph.structs;
      add_probe(alone, *d, params, 0, lib);
      if (!logic::native_ready(alone, nullptr, lib)) continue;
      const std::string name = add_probe(c.graph, *d, params, k, lib);
      const logic::Function& f = *c.graph.function(name);
      std::vector<std::vector<Value>> pool;
      std::size_t most = 1;
      for (const logic::FunctionPin& p : f.inputs) {
        pool.push_back(samples({p.type, p.of}, c.graph));
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
  // (a case that uses libraries finds them among the cases before it)
  const logic::Blueprints before = [&](std::string_view name) -> const logic::Graph* {
    for (std::size_t k = 0; k + 1 < all.size(); ++k) if (all[k].name == name) return &all[k].graph;
    return nullptr;
  };
  auto read = [&](const char* name, const char* text) -> Case& {
    all.push_back({name, {}, {}});
    std::string error;
    std::vector<std::string> used;
    if (!logic::from_cpp(text, all.back().graph, nullptr, &error, logic::library(), before) || !logic::libraries_of(all.back().graph, before, used, &error))
      std::printf("blueprint_native %s: %s\n", name, error.c_str());
    for (const std::string& library : used) all.back().libraries.push_back({library, *before(library)});
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
    Case& c = read("Locals", kLocals);
    for (i64 n : {3, 0, 5, 1}) c.calls.push_back({"SumTo", {I(n)}});
    c.calls.push_back({"Join", {T("a"), T("b"), N(2)}});
    c.calls.push_back({"Join", {T(""), T("zz"), N(-0.5)}});
    for (f64 x : {0.5, 1.0, 7.0, -2.0}) c.calls.push_back({"Limit", {N(x)}});
    c.calls.push_back({"Outer", {N(1), no}});
    c.calls.push_back({"Outer", {N(0.01), no}});  // (with the event: the tier case "locals" - a class called directly has no runtime to raise it)
    c.calls.push_back({"SumTo", {I(2)}});
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
    Case& c = read("Enums", kEnums);
    auto E = [](i64 place) { return Value::enumeration(place); };
    for (int k = 0; k < 6; ++k) c.calls.push_back({"Next", {}});
    for (i64 index : {i64{-1}, i64{0}, i64{2}, i64{3}, i64{4}, std::numeric_limits<i64>::max()})
      for (const char* name : {"Open", "Closing", "nope", ""}) c.calls.push_back({"From", {I(index), T(name)}});
    for (i64 want = 0; want < 4; ++want) {
      c.calls.push_back({"Take", {E(want), Value::enumerations({3, 2}), yes}});
      c.calls.push_back({"Take", {E(want), Value::enumerations({}), no}});
      c.calls.push_back({"Take", {E(want), Value::enumerations({0, 1, 2, 3, want}), no}});
      c.calls.push_back({"Next", {}});
    }
    c.calls.push_back({"Walk", {}});
    c.calls.push_back({"Next", {}});
  }
  {
    Case& c = read("Structs", kStructs);
    const logic::Graph& g = c.graph;
    auto E = [](i64 place) { return Value::enumeration(place); };
    const std::vector<Value> weapons = samples({PinType::structure, "Weapon"}, g), racks = samples({PinType::structure_list, "Weapon"}, g), aims = samples({PinType::structure, "Aim"}, g),
                             solos = samples({PinType::structure, "Solo"}, g);
    c.calls.push_back({"Walk", {yes}});
    for (f64 damage : {12.0, 0.0, -3.5, std::numeric_limits<f64>::infinity()})
      for (i64 kind = 0; kind < 3; ++kind) c.calls.push_back({"Forge", {T(kind ? "Bow" : ""), E(kind), N(damage), Value::vector({1, static_cast<f32>(kind), -2.5f}), Value::actor(static_cast<logic::ActorRef>(kind * 21))}});
    for (std::size_t k = 0; k < weapons.size(); ++k) {
      c.calls.push_back({"Take", {weapons[k], racks[k % racks.size()], solos[k % solos.size()]}});
      c.calls.push_back({"Edit", {T("add"), I(0), weapons[(k * 7 + 3) % weapons.size()]}});
      c.calls.push_back({"Tune", {N(static_cast<f64>(k) * 1.25), T(k % 2 ? "tuned" : ""), aims[k % aims.size()], E(static_cast<i64>(k % 3))}});
      c.calls.push_back({"Walk", {k % 2 ? yes : no}});
      c.calls.push_back({"Lookup", {T(k % 2 ? "tuned" : ""), E(static_cast<i64>(k % 3)), N(static_cast<f64>(k) * 1.25)}});
      c.calls.push_back({"Lookup", {T("nobody"), E(2), N(-1)}});
    }
    for (const Value& rack : racks) {
      c.calls.push_back({"Take", {weapons[2], rack, solos[0]}});
      for (const char* what : {"insert", "set", "remove", "remove_at", "add", "remove", "nothing"})
        for (i64 at : {i64{0}, i64{1}, i64{-1}, i64{99}}) c.calls.push_back({"Edit", {T(what), I(at), weapons[static_cast<std::size_t>(at + 1) % weapons.size()]}});
      c.calls.push_back({"Walk", {no}});
      c.calls.push_back({"Edit", {T("clear"), I(0), weapons[0]}});
      c.calls.push_back({"Walk", {yes}});
    }
    // From outside a struct's input takes a struct of its type: anything else is the struct's
    // default - no struct at all, one of another type, one with a field that is none of its
    // type's; a list, item by item.
    const Value other = aims[1], broken = Value::structure({T("x"), E(7), N(1), Value::of(std::vector<f64>{}), aims[0], Value::structures({}), yes, Value::actor(0), Value::enumerations({})});
    for (const Value& odd : {N(4), T("Weapon"), other, broken, Value::structure({}), Value::zero(PinType::structure), racks[2]})
      c.outside.push_back({"Take", {odd, Value::structures({weapons[1], odd, weapons[3]}), odd}});
    c.outside.push_back({"Take", {weapons[4], N(1), solos[1]}});
    c.outside.push_back({"Tune", {N(2), T("y"), weapons[1], E(1)}});
    c.outside.push_back({"Take", {}});
  }
  {
    Case& c = read("Places", kPlaces);
    const i64 most = std::numeric_limits<i64>::max();
    auto read_all = [&] {
      for (i64 car : {i64{0}, i64{1}, i64{2}, i64{-1}, i64{7}, most})
        for (i64 wheel : {i64{0}, i64{1}, i64{3}, i64{-1}})
          for (i64 at : {i64{0}, i64{1}, i64{9}}) c.calls.push_back({"Read", {I(car), I(wheel), I(at)}});
    };
    read_all();  // (nothing there yet: beside every list)
    c.calls.push_back({"Grow", {I(2), N(0.5)}});
    c.calls.push_back({"Grow", {I(0), N(1.0)}});
    c.calls.push_back({"Grow", {I(3), N(0.25)}});
    read_all();
    for (i64 car : {i64{0}, i64{2}, i64{1}, i64{-1}, i64{5}, most})
      for (i64 wheel : {i64{0}, i64{1}, i64{2}, i64{-3}, i64{40}}) c.calls.push_back({"Set", {I(car), I(wheel), N(static_cast<f64>(car % 5) + 0.75)}});
    read_all();
    for (const char* what : {"add", "insert", "remove", "remove_at", "whole", "tags", "add", "clear", "add", "nothing"})
      for (i64 car : {i64{0}, i64{2}, i64{9}, i64{-1}})
        for (i64 wheel : {i64{0}, i64{1}, i64{-1}})
          for (i64 at : {i64{0}, i64{5}, i64{-2}}) c.calls.push_back({"Edit", {T(what), I(car), I(wheel), I(at), N(static_cast<f64>(at) + 0.5)}});
    read_all();
    for (i64 at : {i64{0}, i64{1}, i64{-1}, most}) c.calls.push_back({"Local", {N(1.5), I(at)}});
    c.calls.push_back({"Grow", {I(1), N(2.0)}});
    read_all();
  }
  {
    read("Parts", kParts).calls = {{"flipped", {Value::enumeration(0)}}, {"flipped", {Value::enumeration(1)}}};
    Case& w = read("Workshop", kWorkshop);
    for (const Value& wheel : samples({PinType::structure, "Wheel"}, w.graph)) {
      w.calls.push_back({"skid", {wheel, N(0.75)}});
      w.calls.push_back({"wear", {wheel}});
      w.calls.push_back({"twice", {wheel, I(3)}});
    }
    w.calls.push_back({"fit", {Value::structures({}), samples({PinType::structure, "Wheel"}, w.graph).front(), I(2)}});
    Case& c = read("Garage", kGarage);
    const i64 most = std::numeric_limits<i64>::max();
    for (i64 car : {i64{0}, i64{1}, i64{-1}, most})
      for (i64 wheel : {i64{0}, i64{1}, i64{4}, i64{-1}}) c.calls.push_back({"Run", {I(car), I(wheel), N(static_cast<f64>(wheel % 3) + 0.5)}});
    c.calls.push_back({"Run", {I(0), I(0), N(-2.0)}});
  }
  {
    Case& c = read("References", kReferences);
    const logic::Graph& g = c.graph;
    const i64 most = std::numeric_limits<i64>::max();
    for (i64 car : {i64{0}, i64{1}, i64{2}, i64{-1}, most})
      for (i64 wheel : {i64{0}, i64{1}, i64{5}, i64{-1}}) c.calls.push_back({"Run", {I(car), I(wheel), N(static_cast<f64>(wheel % 3) + 0.5)}});
    for (const Value& car : samples({PinType::structure, "Car"}, g)) c.calls.push_back({"Local", {car, N(2.5)}});
    c.calls.push_back({"Run", {I(0), I(0), N(-1.0)}});
    for (i64 at : {i64{0}, i64{1}, i64{9}, i64{-1}}) {
      c.calls.push_back({"Lit", {Value::boolean(at != 1), I(at)}});
      c.calls.push_back({"Toggle", {Value::boolean(at == 0)}});
    }
    c.calls.push_back({"Trade", {samples({PinType::structure, "Wheel"}, g).front(), samples({PinType::structure, "Wheel"}, g).back(), N(0.5)}});
    // From outside a reference input takes a value, and what the function made of it stands
    // behind the outputs: Bump gives the number, Drive its outputs and then the car.
    c.calls.push_back({"Bump", {N(1.5), N(2.0)}});
    c.calls.push_back({"Tick", {I(41)}});
    for (const Value& car : samples({PinType::structure, "Car"}, g)) {
      c.calls.push_back({"Drive", {car, I(0), N(0.5)}});
      c.calls.push_back({"Wear", {car, I(0)}});
    }
    c.calls.push_back({"Fill", {Value::of(std::vector<f64>{4, 5}), I(3), N(0.5)}});
    c.outside.push_back({"Drive", {N(3), I(0)}});
    c.outside.push_back({"Bump", {}});
    c.outside.push_back({"Fill", {T("no list")}});
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
  {
    // The engine's functions (kern/logic/engine_api.h): a Blueprint that calls every one of them, against
    // the host's stand-in - what each gives is in a variable of the Blueprint.
    Case& c = read("Engine", engine_case::kEngine);
    for (const char* run : engine_case::kRuns) c.calls.push_back({run, {}});
  }
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
  rt.blueprints = c.blueprints();
  logic::Instance* inst = rt.add(c.graph, 0, c.name);
  rt.begin();
  std::string text = "== " + c.name + "\n";
  std::vector<Call> calls = c.calls;
  calls.insert(calls.end(), c.outside.begin(), c.outside.end());
  for (const Call& call : calls) {
    const logic::Function* f = c.graph.function(call.function);
    std::vector<Value> out;
    if (!f || !inst->call(call.function, call.args, out)) host.errors.push_back(c.name + ": " + call.function + " could not be called");
    std::vector<std::string> args;
    std::vector<std::pair<std::string, std::string>> results, variables;
    for (const Value& a : call.args) args.push_back(native_show(a));
    for (std::size_t k = 0; f && k < f->outputs.size() && k < out.size(); ++k) results.push_back({f->outputs[k].name, native_show(out[k])});
    // (a reference input, called by name: what the function made of it stands behind the outputs)
    std::size_t behind = f ? f->outputs.size() : 0;
    for (std::size_t k = 0; f && k < f->inputs.size(); ++k)
      if (f->inputs[k].ref && !f->inputs[k].constant) results.push_back({f->inputs[k].name, native_show(behind < out.size() ? out[behind] : Value())}), ++behind;
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

inline const char* builtin_cpp_type(PinType t) {
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
/// A pin's type in C++, outside the class `owner` (an enum and a struct are the class's own: Owner::Mode).
inline std::string cpp_type(PinType t, const std::string& of = {}, const std::string& owner = {}) {
  if (logic::element_of(t) != PinType::enumeration && logic::element_of(t) != PinType::structure) return builtin_cpp_type(t);
  return logic::is_list(t) ? "kern::List<" + owner + "::" + of + ">" : owner + "::" + of;
}

/// A value as C++ source, bit for bit. (An enum's value: by its place - static_cast<Owner::Mode>(2);
/// a struct's: every field - `g` has its declaration.)
inline std::string cpp_value(const Value& v, const std::string& of = {}, const std::string& owner = {}, const logic::Graph* g = nullptr) {
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
    case PinType::enumeration: return "static_cast<" + owner + "::" + of + ">(" + std::to_string(v.i) + ")";
    case PinType::structure: {
      const logic::StructType* s = g ? g->structure(of) : nullptr;
      std::string r = owner + "::" + of + "{";
      for (std::size_t k = 0; s && k < s->fields.size(); ++k) r += (k ? ", " : "") + cpp_value(logic::field_of(v, *s, k, *g), s->fields[k].type.of, owner, g);
      return r + "}";
    }
    default: break;
  }
  if (!logic::is_list(v.type)) return "0";
  std::string r = cpp_type(v.type, of, owner) + "{";
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + cpp_value(v.item(k), of, owner, g);
  return r + "}";
}

/// A value as the C++ that makes it as the engine holds it (kern::logic::Value), whatever it is.
inline std::string value_source(const Value& v) {
  const std::string L = "kern::logic::Value::";
  auto rows = [&](const char* make) {
    std::string r = L + make + "({";
    for (std::size_t k = 0, n = v.rows ? v.rows->size() : 0; k < n; ++k) r += (k ? ", " : "") + value_source((*v.rows)[k]);
    return r + "})";
  };
  switch (v.type) {
    case PinType::boolean: return L + "boolean(" + cpp_value(v) + ")";
    case PinType::integer: return L + "integer(" + cpp_value(v) + ")";
    case PinType::number: return L + "number(" + cpp_value(v) + ")";
    case PinType::vector: return L + "vector(" + cpp_value(v) + ")";
    case PinType::text: return L + "text(" + cpp_value(v) + ")";
    case PinType::actor: return L + "actor(" + std::to_string(v.a) + "u)";
    case PinType::enumeration: return L + "enumeration(" + std::to_string(v.i) + ")";
    case PinType::structure: return v.rows ? rows("structure") : L + "zero(kern::logic::PinType::structure)";
    case PinType::structure_list: return rows("structures");
    case PinType::enumeration_list: {
      std::string r = L + "enumerations({";
      for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + std::to_string(v.item(k).i);
      return r + "})";
    }
    default: return "kern::logic::to_value(" + cpp_value(v) + ")";  // (a list of a value type)
  }
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
    if (!logic::native_ready(c.graph, &why) || !logic::to_cpp(c.graph, c.name, text, &why) || !logic::native_glue(c.graph, c.name, glue, &why, logic::library(), {}, c.blueprints())) {
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
    // (the libraries it uses, as the runtime finds them: the Blueprints of its project)
    std::string used;
    for (std::size_t k = 0; k < c.libraries.size(); ++k)
      used += "  kern::logic::Graph library" + std::to_string(k) + ";\n  {\n    kern::json::Value stored;\n    if (!kern::json::parse(kGraph_" + c.libraries[k].first +
              ", stored) || !kern::logic::graph_from_json(stored, library" + std::to_string(k) + ")) hosted += \"(a library was not read)\\n\";\n  }\n";
    if (!c.libraries.empty()) {
      used += "  rt.blueprints = [&](std::string_view name) -> const kern::logic::Graph* {\n";
      for (std::size_t k = 0; k < c.libraries.size(); ++k) used += "    if (name == \"" + c.libraries[k].first + "\") return &library" + std::to_string(k) + ";\n";
      used += "    return nullptr;\n  };\n";
    }
    body += ";\n\nstatic void run_" + c.name + "(std::string& got, std::string& hosted) {\n  " + c.name + " b;\n"
            "  NativeTestHost alone;  // (the class by itself: a host of its own - its Random nodes start from that host's number)\n  b.kern_world.host = &alone;\n"
            "  got += \"== " + c.name + "\\n\";\n  const auto variables = [&] {\n    return std::string(\" |\")";
    for (const logic::Variable& v : c.graph.variables) body += " + \" " + v.name + "=\" + show(b." + v.name + ")";
    body += ";\n  };\n"
            "  // the same Blueprint behind a logic::Instance: the engine's way to it, by name\n"
            "  NativeTestHost host;\n  kern::logic::Runtime rt(host);\n  rt.jit = false;\n  kern::logic::Graph graph;\n"
            "  {\n    kern::json::Value stored;\n    if (!kern::json::parse(kGraph_" + c.name + ", stored) || !kern::logic::graph_from_json(stored, graph)) hosted += \"(the graph was not read)\\n\";\n  }\n" +
            used +
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
      for (std::size_t i = 0; i < f.inputs.size(); ++i) body += " " + cpp_type(f.inputs[i].type, f.inputs[i].of, c.name) + " a" + std::to_string(i) + ";";
      body += " int row; } rows[] = {\n";
      for (std::size_t r = at; r < end; ++r) {
        body += "        {";
        for (std::size_t i = 0; i < c.calls[r].args.size(); ++i) body += cpp_value(c.calls[r].args[i], i < f.inputs.size() ? f.inputs[i].of : std::string(), c.name, &c.graph) + ", ";
        body += std::to_string(r) + "},\n";
      }
      body += "    };\n    for (const auto& r : rows) {\n      (void)r;\n      ";
      std::string call = "b." + f.name + "(";
      std::string shown = "std::string(\"" + f.name + "(\")";
      // (a reference input is a place: the class is given a variable that starts as the value,
      // and what the function made of it is shown behind the outputs - as a call by name gives it)
      std::string places, changed, changed_by_name;
      std::size_t behind = f.outputs.size();
      for (std::size_t i = 0; i < f.inputs.size(); ++i) {
        const std::string k = std::to_string(i);
        if (f.inputs[i].ref) {
          places += "kern::logic::NativeParam<&" + c.name + "::" + f.name + ", " + k + "> p" + k + " = r.a" + k + ";\n      ";
          if (!f.inputs[i].constant) {
            changed += " + \" " + f.inputs[i].name + "=\" + show(p" + k + ")";
            changed_by_name += " + \" " + f.inputs[i].name + "=\" + native_show(results.size() > " + std::to_string(behind) + " ? results[" + std::to_string(behind) + "] : kern::logic::Value())";
            ++behind;
          }
        }
        call += std::string(i ? ", " : "") + (f.inputs[i].ref ? "p" : "r.a") + k;
        shown += std::string(i ? " + \", \"" : "") + " + show(r.a" + k + ")";
      }
      call += ")";
      body += places + (f.outputs.empty() ? "" : "const auto out = ") + call + ";\n      got += " + shown + " + \") ->\"";
      for (const logic::FunctionPin& o : f.outputs) body += " + \" " + o.name + "=\" + show(out." + o.name + ")";
      body += changed + " + variables() + \"\\n\";\n";
      // ... and the same call by name
      body += "      std::vector<kern::logic::Value> results;\n      if (!inst->call(\"" + f.name + "\", {";
      for (std::size_t i = 0; i < f.inputs.size(); ++i) body += std::string(i ? ", " : "") + "kern::logic::to_value(r.a" + std::to_string(i) + ")";
      body += "}, results)) hosted += \"(no call) \";\n      hosted += " + shown + " + \") ->\"";
      for (std::size_t k = 0; k < f.outputs.size(); ++k)
        body += " + \" " + f.outputs[k].name + "=\" + native_show(results.size() > " + std::to_string(k) + " ? results[" + std::to_string(k) + "] : kern::logic::Value())";
      body += changed_by_name + " + hosted_variables() + \"\\n\";\n    }\n  }\n";
      at = end;
    }
    // The calls with values from outside: the class takes them as the engine gives them - each
    // input through logic::native_arg, a missing one its default.
    for (const Call& call : c.outside) {
      const logic::Function& f = *c.graph.function(call.function);
      std::string given, direct = "b." + f.name + "(", shown = "std::string(\"" + f.name + "(\")";
      for (std::size_t i = 0; i < call.args.size(); ++i) {
        given += (i ? ", " : "") + value_source(call.args[i]);
        shown += std::string(i ? " + \", \"" : "") + " + native_show(given[" + std::to_string(i) + "])";
      }
      std::string places, changed, changed_by_name;
      std::size_t behind = f.outputs.size();
      for (std::size_t i = 0; i < f.inputs.size(); ++i) {
        const logic::FunctionPin& in = f.inputs[i];
        const std::string k = std::to_string(i);
        const std::string taken = "kern::logic::native_arg<" + cpp_type(in.type, in.of, c.name) + ">(given, " + k + ", " + cpp_value(Value::from_json(in.value, in.type, c.graph, in.of), in.of, c.name, &c.graph) + ")";
        if (in.ref) {
          places += "    kern::logic::NativeParam<&" + c.name + "::" + f.name + ", " + k + "> p" + k + " = " + taken + ";\n";
          if (!in.constant) {
            changed += " + \" " + in.name + "=\" + show(p" + k + ")";
            changed_by_name += " + \" " + in.name + "=\" + native_show(results.size() > " + std::to_string(behind) + " ? results[" + std::to_string(behind) + "] : kern::logic::Value())";
            ++behind;
          }
        }
        direct += std::string(i ? ", " : "") + (in.ref ? "p" + k : taken);
      }
      body += "  {\n    const std::vector<kern::logic::Value> given = {" + given + "};\n" + places + "    " + (f.outputs.empty() ? "" : "const auto out = ") + direct + ");\n    got += " + shown + " + \") ->\"";
      for (const logic::FunctionPin& o : f.outputs) body += " + \" " + o.name + "=\" + show(out." + o.name + ")";
      body += changed + " + variables() + \"\\n\";\n    std::vector<kern::logic::Value> results;\n    if (!inst->call(\"" + f.name + "\", given, results)) hosted += \"(no call) \";\n    hosted += " + shown + " + \") ->\"";
      for (std::size_t k = 0; k < f.outputs.size(); ++k)
        body += " + \" " + f.outputs[k].name + "=\" + native_show(results.size() > " + std::to_string(k) + " ? results[" + std::to_string(k) + "] : kern::logic::Value())";
      body += changed_by_name + " + hosted_variables() + \"\\n\";\n  }\n";
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
    calls += c.calls.size() + c.outside.size();
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
