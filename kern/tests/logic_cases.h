// Logic graphs whose results must be identical in every tier: the interpreter,
// the JIT (WebAssembly, tests/logic_wasm_test.*) and the Blueprint's own C++ class
// (run.sh compiles their texts into blueprint_native_test: tests/blueprint_native.h).
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "kern/core/json.h"
#include "kern/logic/logic.h"

namespace logic_cases {

struct Case {
  const char* name;
  const char* graph;
};

inline const std::vector<Case>& cases() {
  static const std::vector<Case> list = {
      {"math_loops",
       R"({"variables":[{"name":"Sum","type":"float","value":0},{"name":"Odd","type":"int","value":0},{"name":"Flag","type":"bool","value":false},{"name":"Avg","type":"float","value":0}],
        "nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"flow.for","values":{"first":1,"last":20}},
          {"id":3,"type":"var.add","params":{"variable":"Sum"}},{"id":4,"type":"math.mul","values":{"b":1.5}},
          {"id":5,"type":"flow.branch"},{"id":6,"type":"math.compare","params":{"op":"=="},"values":{"b":1}},{"id":7,"type":"math.mod","values":{"b":2}},
          {"id":8,"type":"var.add","params":{"variable":"Odd"}},
          {"id":9,"type":"var.set","params":{"variable":"Flag"}},{"id":10,"type":"logic.and"},{"id":11,"type":"math.compare","params":{"op":">"},"values":{"b":100}},
          {"id":12,"type":"var.get","params":{"variable":"Sum"}},{"id":13,"type":"logic.not"},{"id":14,"type":"math.compare","params":{"op":"<"},"values":{"b":5}},
          {"id":15,"type":"var.set","params":{"variable":"Avg"}},{"id":16,"type":"math.div","values":{"b":20}},{"id":17,"type":"math.clamp","values":{"min":0,"max":14}},
          {"id":18,"type":"math.round"},{"id":19,"type":"math.lerp","values":{"alpha":0.25}},{"id":20,"type":"math.sqrt"}],
        "links":[[1,"then",2,"exec"],[2,"body",3,"exec"],[2,"index",4,"a"],[4,"result",3,"amount"],
          [3,"then",5,"exec"],[2,"index",7,"a"],[7,"result",6,"a"],[6,"result",5,"condition"],[5,"true",8,"exec"],
          [2,"completed",9,"exec"],[12,"value",11,"a"],[11,"result",10,"a"],[14,"result",13,"a"],[13,"result",10,"b"],[12,"value",14,"a"],[10,"result",9,"value"],
          [9,"then",15,"exec"],[12,"value",16,"a"],[16,"result",17,"value"],[17,"result",18,"a"],[18,"result",19,"b"],[20,"result",19,"a"],[12,"value",20,"a"],[19,"result",15,"value"]]})"},
      {"gates_and_sequences",
       R"({"variables":[{"name":"Log","type":"float","value":0},{"name":"Once","type":"int","value":0},{"name":"N","type":"int","value":0},{"name":"Gate","type":"int","value":0},{"name":"Flips","type":"int","value":0}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"flow.sequence","params":{"count":"5"}},
          {"id":3,"type":"var.set","params":{"variable":"Log"}},{"id":4,"type":"math.add"},{"id":5,"type":"math.mul","values":{"b":10}},{"id":6,"type":"var.get","params":{"variable":"Log"}},
          {"id":7,"type":"flow.do_once"},{"id":8,"type":"var.add","params":{"variable":"Once"}},
          {"id":9,"type":"flow.do_n","values":{"n":2}},{"id":10,"type":"var.add","params":{"variable":"N"}},
          {"id":11,"type":"flow.gate","values":{"closed":true}},{"id":12,"type":"var.add","params":{"variable":"Gate"}},
          {"id":13,"type":"flow.flip_flop"},{"id":14,"type":"var.add","params":{"variable":"Flips"},"values":{"amount":1}},{"id":15,"type":"var.add","params":{"variable":"Flips"},"values":{"amount":100}},
          {"id":16,"type":"event.custom","params":{"name":"Open"}},{"id":17,"type":"value.float","values":{"value":1}}],
        "links":[[1,"then",2,"exec"],[2,"then0",3,"exec"],[6,"value",5,"a"],[5,"result",4,"a"],[17,"result",4,"b"],[4,"result",3,"value"],
          [2,"then1",7,"exec"],[7,"completed",8,"exec"],[2,"then2",9,"exec"],[9,"exit",10,"exec"],[2,"then3",11,"enter"],[11,"exit",12,"exec"],
          [2,"then4",13,"exec"],[13,"a",14,"exec"],[13,"b",15,"exec"],[16,"then",11,"open"]]})"},
      {"while_nested",
       R"({"variables":[{"name":"I","type":"int","value":0},{"name":"Total","type":"float","value":0}],
        "nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"flow.while"},{"id":3,"type":"math.compare","params":{"op":"<"},"values":{"b":7}},{"id":4,"type":"var.get","params":{"variable":"I"}},
          {"id":5,"type":"flow.sequence","params":{"count":"2"}},{"id":6,"type":"var.add","params":{"variable":"I"}},{"id":7,"type":"flow.for","values":{"first":0,"last":3}},
          {"id":8,"type":"var.add","params":{"variable":"Total"}},{"id":9,"type":"math.mul"},{"id":10,"type":"var.get","params":{"variable":"I"}}],
        "links":[[1,"then",2,"exec"],[4,"value",3,"a"],[3,"result",2,"condition"],[2,"body",5,"exec"],[5,"then0",6,"exec"],[5,"then1",7,"exec"],
          [7,"body",8,"exec"],[7,"index",9,"a"],[10,"value",9,"b"],[9,"result",8,"amount"]]})"},
      {"mixed_cpp_nodes",
       R"({"variables":[{"name":"Label","type":"text","value":""},{"name":"Count","type":"int","value":0},{"name":"Done","type":"bool","value":false}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.add","params":{"variable":"Count"}},{"id":3,"type":"var.set","params":{"variable":"Label"}},
          {"id":4,"type":"text.append","values":{"a":"n="}},{"id":5,"type":"debug.print"},{"id":6,"type":"flow.branch"},{"id":7,"type":"math.compare","params":{"op":">="},"values":{"b":3}},
          {"id":8,"type":"flow.delay","values":{"duration":0.5}},{"id":9,"type":"var.set","params":{"variable":"Done"},"values":{"value":true}},{"id":10,"type":"math.add"}],
        "links":[[1,"then",2,"exec"],[2,"then",3,"exec"],[2,"value",4,"b"],[4,"result",3,"value"],[3,"then",5,"exec"],[3,"value",5,"text"],
          [5,"then",6,"exec"],[2,"value",7,"a"],[7,"result",6,"condition"],[6,"true",8,"exec"],[8,"completed",9,"exec"],[1,"value",10,"a"],[2,"value",10,"b"]]})"},
      {"state_machine",
       R"({"variables":[{"name":"Entered","type":"int","value":0},{"name":"Ready","type":"bool","value":false}],
        "machines":[{"id":30,"name":"M","states":[{"id":31,"name":"A","initial":true},{"id":32,"name":"B"},{"id":33,"name":"C"}],
          "transitions":[{"id":40,"from":31,"to":32,"trigger":"event","event":"Go"},{"id":41,"from":32,"to":33,"trigger":"timer","seconds":0.25},{"id":42,"from":33,"to":31,"trigger":"condition","variable":"Ready"}]}],
        "nodes":[{"id":1,"type":"event.state_enter","params":{"machine":"M","state":"any"}},{"id":2,"type":"var.add","params":{"variable":"Entered"}},
          {"id":3,"type":"event.state_enter","params":{"machine":"M","state":"C"}},{"id":4,"type":"var.set","params":{"variable":"Ready"},"values":{"value":true}}],
        "links":[[1,"then",2,"exec"],[3,"then",4,"exec"]]})"},
      {"reroutes",
       R"({"variables":[{"name":"Sum","type":"float","value":0},{"name":"Hits","type":"int","value":0}],
        "comments":[{"id":50,"x":-20,"y":-40,"w":600,"h":300,"text":"Knots change nothing"}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"flow.reroute"},{"id":3,"type":"flow.reroute"},
          {"id":4,"type":"var.add","params":{"variable":"Hits"}},{"id":5,"type":"value.reroute","params":{"type":"float"}},
          {"id":6,"type":"var.add","params":{"variable":"Sum"}},{"id":7,"type":"value.reroute","params":{"type":"float"}}],
        "links":[[1,"then",2,"exec"],[2,"then",3,"exec"],[3,"then",4,"exec"],[4,"then",6,"exec"],[1,"value",5,"in"],[5,"out",7,"in"],[7,"out",6,"amount"]]})"},
      {"functions",
       R"({"variables":[{"name":"Total","type":"float","value":0},{"name":"A","type":"float","value":0},{"name":"Early","type":"int","value":0},{"name":"Acc","type":"float","value":0},{"name":"AccSum","type":"float","value":0}],
        "functions":[{"name":"Double","pure":true,"inputs":[{"name":"x","type":"float","value":0}],"outputs":[{"name":"result","type":"float","value":0}]},
          {"name":"SumTo","inputs":[{"name":"n","type":"int","value":0}],"outputs":[{"name":"sum","type":"float","value":0},{"name":"early","type":"bool","value":false}]}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.add","params":{"variable":"Total"}},{"id":3,"type":"func.pure","params":{"function":"Double"}},
          {"id":4,"type":"func.call","params":{"function":"SumTo"}},{"id":5,"type":"var.set","params":{"variable":"A"}},{"id":6,"type":"flow.branch"},
          {"id":7,"type":"var.add","params":{"variable":"Early"},"values":{"amount":1}},{"id":8,"type":"var.add","params":{"variable":"AccSum"}},
          {"id":20,"type":"func.entry","params":{"function":"Double"}},{"id":21,"type":"math.mul","values":{"b":2}},{"id":22,"type":"func.return","params":{"function":"Double"}},
          {"id":30,"type":"func.entry","params":{"function":"SumTo"}},{"id":31,"type":"var.set","params":{"variable":"Acc"},"values":{"value":0}},{"id":32,"type":"flow.for","values":{"first":1}},
          {"id":33,"type":"var.add","params":{"variable":"Acc"}},{"id":34,"type":"flow.branch"},{"id":35,"type":"math.compare","params":{"op":"=="},"values":{"b":4}},
          {"id":36,"type":"func.return","params":{"function":"SumTo"},"values":{"early":true}},{"id":37,"type":"var.get","params":{"variable":"Acc"}},
          {"id":38,"type":"func.return","params":{"function":"SumTo"},"values":{"early":false}}],
        "links":[[1,"then",2,"exec"],[1,"value",3,"x"],[3,"result",2,"amount"],[2,"then",4,"exec"],[1,"value",4,"n"],[4,"then",5,"exec"],[4,"sum",5,"value"],
          [5,"then",8,"exec"],[37,"value",8,"amount"],[8,"then",6,"exec"],[4,"early",6,"condition"],[6,"true",7,"exec"],
          [20,"then",22,"exec"],[20,"x",21,"a"],[21,"result",22,"result"],
          [30,"then",31,"exec"],[31,"then",32,"exec"],[30,"n",32,"last"],[32,"body",33,"exec"],[32,"index",33,"amount"],[33,"then",34,"exec"],[32,"index",35,"a"],[35,"result",34,"condition"],
          [34,"true",36,"exec"],[37,"value",36,"sum"],[32,"completed",38,"exec"],[37,"value",38,"sum"]]})"},
      {"formulas",
       R"kg({"variables":[{"name":"F","type":"float","value":0},{"name":"G","type":"float","value":0},{"name":"H","type":"int","value":0}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.set","params":{"variable":"F"}},
          {"id":3,"type":"math.formula","params":{"formula":"max(2, value * 1.5) + (value > 3 ? 10 : 0)"}},
          {"id":4,"type":"var.add","params":{"variable":"G"}},{"id":5,"type":"math.formula","params":{"formula":"clamp(x, lo, 4) * 0.7 + sin(rad(90)) - 2 ^ 3 / 4"},"values":{"lo":1}},
          {"id":6,"type":"var.set","params":{"variable":"H"}},{"id":7,"type":"math.formula","params":{"formula":"ceil(x * 0.7) + (x % 2 == 0 && x != 0) * 100"}}],
        "links":[[1,"then",2,"exec"],[1,"value",3,"value"],[3,"result",2,"value"],[2,"then",4,"exec"],[1,"value",5,"x"],[5,"result",4,"amount"],
          [4,"then",6,"exec"],[1,"value",7,"x"],[7,"result",6,"value"]]})kg"},
      // The sign of -0 (Copy Sign, the formula's copysign): -Go's value is -0 for 0, and Copy Sign
      // gives the size of 3 with that sign - -3 in every tier, where Sign of -0 says 0.
      {"signed_zero",
       R"kg({"variables":[{"name":"F","type":"float","value":0},{"name":"G","type":"float","value":0},{"name":"Signs","type":"float_list","value":[]}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.add","params":{"variable":"G"}},{"id":3,"type":"math.negate"},
          {"id":4,"type":"math.copysign","values":{"a":3}},{"id":5,"type":"var.set","params":{"variable":"F"}},
          {"id":6,"type":"math.formula","params":{"formula":"copysign(1, z) * 100 + copysign(value, 1 - value)"}},{"id":7,"type":"list.add","params":{"variable":"Signs"}}],
        "links":[[1,"then",2,"exec"],[1,"value",3,"a"],[3,"result",4,"b"],[4,"result",2,"amount"],[2,"then",5,"exec"],[3,"result",6,"z"],[1,"value",6,"value"],
          [6,"result",5,"value"],[5,"then",7,"exec"],[6,"result",7,"item"]]})kg"},
      // The bytes of a struct (Bytes of Struct, Struct of Bytes): Go's value goes into a struct of
      // numbers, its bytes into B, and back - from the first byte and from the second (the last byte
      // missing: 0). Flags is cut to 16 bits, Speed to a float's, Vx is signed.
      {"bytes",
       R"kg({"structs":[{"name":"Frame","fields":[{"name":"Magic","type":"int","value":1447121230},{"name":"Seq","type":"int","value":0},{"name":"Vx","type":"int","value":-3},)kg"
       R"kg({"name":"Flags","type":"int","value":70000},{"name":"On","type":"bool","value":true},{"name":"Speed","type":"float","value":0}]}],)kg"
       R"kg("variables":[{"name":"B","type":"int_list","value":[]},{"name":"Back","type":"struct","of":"Frame","value":{}},{"name":"Shifted","type":"struct","of":"Frame","value":{}}],)kg"
       R"kg("nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.set","params":{"variable":"B"}},)kg"
       R"kg({"id":3,"type":"struct.bytes","params":{"struct":"Frame","layout":"16: u32@0 u16@4 i16@6 u16@8 bool@10 f32@12"}},{"id":4,"type":"struct.make","params":{"struct":"Frame"}},)kg"
       R"kg({"id":5,"type":"math.formula","params":{"formula":"value * 0.1"}},{"id":6,"type":"math.round"},{"id":7,"type":"var.set","params":{"variable":"Back"}},)kg"
       R"kg({"id":8,"type":"struct.of_bytes","params":{"struct":"Frame","layout":"16: u32@0 u16@4 i16@6 u16@8 bool@10 f32@12"}},{"id":9,"type":"var.get","params":{"variable":"B"}},)kg"
       R"kg({"id":10,"type":"var.set","params":{"variable":"Shifted"}},)kg"
       R"kg({"id":11,"type":"struct.of_bytes","params":{"struct":"Frame","layout":"16: u32@0 u16@4 i16@6 u16@8 bool@10 f32@12"},"values":{"at":1}}],)kg"
       R"kg("links":[[1,"then",2,"exec"],[4,"value",3,"value"],[3,"bytes",2,"value"],[1,"value",5,"value"],[5,"result",4,"Speed"],[1,"value",6,"a"],[6,"result",4,"Seq"],)kg"
       R"kg([2,"then",7,"exec"],[9,"value",8,"bytes"],[8,"value",7,"value"],[7,"then",10,"exec"],[9,"value",11,"bytes"],[11,"value",10,"value"]]})kg"},
      {"events_and_time",
       R"({"variables":[{"name":"Ticks","type":"int","value":0},{"name":"Elapsed","type":"float","value":0},{"name":"Heard","type":"float","value":0},{"name":"Last","type":"text","value":""}],
        "nodes":[{"id":1,"type":"event.tick"},{"id":2,"type":"var.add","params":{"variable":"Ticks"}},{"id":3,"type":"var.add","params":{"variable":"Elapsed"}},
          {"id":11,"type":"flow.branch"},{"id":12,"type":"math.compare","params":{"op":"=="},"values":{"b":30}},{"id":13,"type":"debug.print","values":{"screen":false}},
          {"id":14,"type":"time.now"},{"id":16,"type":"text.append","values":{"a":"tick 30 at ","c":" s"}},
          {"id":4,"type":"event.custom","params":{"name":"Go"}},{"id":5,"type":"flow.call","params":{"name":"Echo"},"values":{"text":"from go"}},{"id":17,"type":"math.mul","values":{"b":2}},
          {"id":6,"type":"event.custom","params":{"name":"Echo"}},{"id":7,"type":"var.add","params":{"variable":"Heard"}},{"id":8,"type":"var.set","params":{"variable":"Last"}},
          {"id":9,"type":"debug.print"},{"id":10,"type":"text.append","values":{"a":"echo "}}],
        "links":[[1,"then",2,"exec"],[2,"then",3,"exec"],[1,"delta",3,"amount"],[3,"then",11,"exec"],[2,"value",12,"a"],[12,"result",11,"condition"],[11,"true",13,"exec"],
          [14,"result",16,"b"],[16,"result",13,"text"],
          [4,"then",5,"exec"],[4,"value",17,"a"],[17,"result",5,"value"],
          [6,"then",7,"exec"],[6,"value",7,"amount"],[7,"then",8,"exec"],[6,"text",8,"value"],[8,"then",9,"exec"],[6,"text",10,"b"],[6,"value",10,"c"],[10,"result",9,"text"]]})"},
      // A handler that raises its own event: every run has its own values, like the parameters of
      // a method - also after the inner run is over (Sum), and for who reads them from outside (Seen).
      {"event_reentry",
       R"({"variables":[{"name":"Sum","type":"float","value":0},{"name":"Runs","type":"int","value":0},{"name":"Seen","type":"float","value":-1}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.add","params":{"variable":"Runs"}},{"id":3,"type":"flow.branch"},
          {"id":4,"type":"math.compare","params":{"op":">"},"values":{"b":0}},{"id":5,"type":"flow.call","params":{"name":"Go"}},{"id":6,"type":"math.sub","values":{"b":1}},
          {"id":7,"type":"var.add","params":{"variable":"Sum"}},{"id":8,"type":"flow.sequence","params":{"count":"2"}},
          {"id":9,"type":"event.custom","params":{"name":"Open"}},{"id":10,"type":"var.set","params":{"variable":"Seen"}}],
        "links":[[1,"then",2,"exec"],[2,"then",8,"exec"],[8,"then0",3,"exec"],[1,"value",4,"a"],[4,"result",3,"condition"],[3,"true",5,"exec"],[1,"value",6,"a"],[6,"result",5,"value"],
          [8,"then1",7,"exec"],[1,"value",7,"amount"],[9,"then",10,"exec"],[1,"value",10,"value"]]})"},
      // A function that is called again while it runs - by an event it raised (Step): a call like
      // any other. Each call has its parameters, also for the step two paths lead to (it reads
      // them from outside the body's own path) and after the inner call is over. Functions called
      // by nodes do not count for how deep events nest (Outer, Inner between the Deep events: 63
      // handlers run before the 65th level is refused, in every tier). Every Deep handler leaves
      // ten steps of a Sequence open while the next level runs - more than 600 at the deepest: the
      // JIT's stack is as large as the graph can need (Tail).
      {"function_reentry",
       R"({"variables":[{"name":"Log","type":"float","value":0},{"name":"Calls","type":"int","value":0},{"name":"DeepCount","type":"int","value":0},{"name":"Tail","type":"int","value":0}],
        "functions":[{"name":"Step","inputs":[{"name":"x","type":"float","value":0}],"outputs":[]},{"name":"Outer","inputs":[{"name":"v","type":"float","value":0}],"outputs":[]},
          {"name":"Inner","inputs":[{"name":"v","type":"float","value":0}],"outputs":[]}],
        "nodes":[{"id":20,"type":"func.entry","params":{"function":"Step"}},{"id":21,"type":"var.add","params":{"variable":"Calls"},"values":{"amount":1}},
          {"id":22,"type":"flow.sequence","params":{"count":"2"}},{"id":23,"type":"flow.branch"},{"id":24,"type":"math.compare","params":{"op":">"},"values":{"b":0}},
          {"id":25,"type":"flow.call","params":{"name":"Again"}},{"id":27,"type":"math.sub","values":{"b":1}},{"id":26,"type":"var.set","params":{"variable":"Log"}},
          {"id":28,"type":"math.formula","params":{"formula":"(acc * 7 + x) % 1000003"}},{"id":29,"type":"var.get","params":{"variable":"Log"}},
          {"id":30,"type":"event.custom","params":{"name":"Again"}},{"id":31,"type":"func.call","params":{"function":"Step"}},
          {"id":32,"type":"event.custom","params":{"name":"Go"}},{"id":33,"type":"func.call","params":{"function":"Step"}},
          {"id":40,"type":"event.custom","params":{"name":"Open"}},{"id":41,"type":"flow.call","params":{"name":"Deep"},"values":{"value":70}},
          {"id":42,"type":"event.custom","params":{"name":"Deep"}},{"id":43,"type":"var.add","params":{"variable":"DeepCount"},"values":{"amount":1}},
          {"id":44,"type":"func.call","params":{"function":"Outer"}},{"id":45,"type":"flow.sequence","params":{"count":"11"}},
          {"id":46,"type":"var.add","params":{"variable":"Tail"},"values":{"amount":1}},
          {"id":50,"type":"func.entry","params":{"function":"Outer"}},{"id":51,"type":"func.call","params":{"function":"Inner"}},
          {"id":60,"type":"func.entry","params":{"function":"Inner"}},{"id":61,"type":"flow.branch"},{"id":62,"type":"math.compare","params":{"op":">"},"values":{"b":0}},
          {"id":63,"type":"flow.call","params":{"name":"Deep"}},{"id":64,"type":"math.sub","values":{"b":1}}],
        "links":[[20,"then",21,"exec"],[21,"then",22,"exec"],[22,"then0",23,"exec"],[20,"x",24,"a"],[24,"result",23,"condition"],[23,"true",25,"exec"],[20,"x",27,"a"],[27,"result",25,"value"],
          [25,"then",26,"exec"],[22,"then1",26,"exec"],[29,"value",28,"acc"],[20,"x",28,"x"],[28,"result",26,"value"],
          [30,"then",31,"exec"],[30,"value",31,"x"],[32,"then",33,"exec"],[32,"value",33,"x"],
          [40,"then",41,"exec"],[42,"then",43,"exec"],[43,"then",45,"exec"],[45,"then0",44,"exec"],[42,"value",44,"v"],
          [45,"then1",46,"exec"],[45,"then2",46,"exec"],[45,"then3",46,"exec"],[45,"then4",46,"exec"],[45,"then5",46,"exec"],[45,"then6",46,"exec"],[45,"then7",46,"exec"],
          [45,"then8",46,"exec"],[45,"then9",46,"exec"],[45,"then10",46,"exec"],
          [50,"then",51,"exec"],[50,"v",51,"v"],[60,"then",61,"exec"],[60,"v",62,"a"],[62,"result",61,"condition"],[61,"true",63,"exec"],[60,"v",64,"a"],[64,"result",63,"value"]]})"},
      // Whole numbers at their ends: a variable doubled 70 times stays at Int's end, one past 2^53
      // adds as a number does, Float to Int of what does not fit, Floor and Round of endless and
      // of what is no number - every tier holds a whole number the same way (logic::normal). Int's
      // lower end is -(2^63 - 1): what is typed in as -2^63 (Least) is that, and C++ can write it.
      {"whole_numbers",
       R"({"variables":[{"name":"Big","type":"int","value":1},{"name":"Edge","type":"int","value":9007199254740992},{"name":"Least","type":"int","value":-9223372036854775808},{"name":"Top","type":"int","value":0},
          {"name":"Low","type":"int","value":0},{"name":"FromFloor","type":"float","value":-1},{"name":"FromRound","type":"float","value":-1},
          {"name":"TopAsFloat","type":"float","value":0},{"name":"Shown","type":"text","value":""}],"nodes":[{"id":1,"type":"event.begin"},
          {"id":2,"type":"flow.for","values":{"first":0,"last":69}},{"id":20,"type":"math.mul","values":{"a":1e+300,"b":1e+300}},
          {"id":21,"type":"flow.sequence","params":{"count":"4"}},{"id":22,"type":"var.add","params":{"variable":"Big"}},{"id":23,"type":"var.get","params":{"variable":"Big"}},
          {"id":24,"type":"var.set","params":{"variable":"Shown"}},{"id":25,"type":"var.get","params":{"variable":"Big"}},
          {"id":26,"type":"var.add","params":{"variable":"Edge"},"values":{"amount":1}},{"id":27,"type":"var.add","params":{"variable":"Edge"},"values":{"amount":1}},
          {"id":28,"type":"var.add","params":{"variable":"Edge"},"values":{"amount":1}},{"id":29,"type":"var.set","params":{"variable":"Top"}},
          {"id":30,"type":"value.float","values":{"value":1e+300}},{"id":31,"type":"var.add","params":{"variable":"Top"},"values":{"amount":5}},
          {"id":32,"type":"var.set","params":{"variable":"TopAsFloat"}},{"id":33,"type":"var.get","params":{"variable":"Top"}},{"id":34,"type":"var.set","params":{"variable":"Low"}},
          {"id":35,"type":"value.float","values":{"value":-1e+300}},{"id":36,"type":"var.add","params":{"variable":"Low"},"values":{"amount":-1}},
          {"id":37,"type":"var.set","params":{"variable":"FromFloor"}},{"id":38,"type":"math.floor"},{"id":39,"type":"var.set","params":{"variable":"FromRound"}},
          {"id":40,"type":"math.round"},{"id":41,"type":"math.sub"}],"links":[[1,"then",21,"exec"],[2,"body",22,"exec"],[2,"completed",24,"exec"],[20,"result",38,"a"],
          [20,"result",41,"a"],[20,"result",41,"b"],[21,"then0",2,"exec"],[21,"then1",26,"exec"],[21,"then2",29,"exec"],[21,"then3",37,"exec"],[23,"value",22,"amount"],
          [25,"value",24,"value"],[26,"then",27,"exec"],[27,"then",28,"exec"],[29,"then",31,"exec"],[30,"result",29,"value"],[31,"then",32,"exec"],[32,"then",34,"exec"],
          [33,"value",32,"value"],[34,"then",36,"exec"],[35,"result",34,"value"],[37,"then",39,"exec"],[38,"result",37,"value"],[40,"result",39,"value"],[41,"result",40,"a"]]})"},
      // Every N Seconds, a Retriggerable Delay, three Tweens (one runs to its end, one is stopped,
      // one plays itself again from Finished).
      {"waits",
       R"({"variables":[{"name":"Count","type":"int","value":0},{"name":"Fired","type":"int","value":0},{"name":"Alpha","type":"float","value":0},{"name":"Steps","type":"int","value":0},
          {"name":"Done","type":"bool","value":false},{"name":"Steps2","type":"int","value":0},{"name":"Late","type":"bool","value":false},{"name":"Rounds","type":"int","value":0}],
        "nodes":[{"id":1,"type":"event.every","params":{"seconds":"0.25"}},{"id":2,"type":"var.set","params":{"variable":"Count"}},
          {"id":3,"type":"event.custom","params":{"name":"Go"}},{"id":4,"type":"flow.retrigger_delay","values":{"duration":0.3}},{"id":5,"type":"var.add","params":{"variable":"Fired"}},
          {"id":6,"type":"event.begin"},{"id":14,"type":"flow.sequence","params":{"count":"3"}},{"id":7,"type":"flow.tween","params":{"ease":"smooth"},"values":{"duration":0.5}},
          {"id":8,"type":"var.set","params":{"variable":"Alpha"}},{"id":9,"type":"var.add","params":{"variable":"Steps"}},
          {"id":10,"type":"var.set","params":{"variable":"Done"},"values":{"value":true}},{"id":11,"type":"debug.print","values":{"text":"tween done"}},
          {"id":12,"type":"event.custom","params":{"name":"Open"}},{"id":13,"type":"flow.tween","params":{"ease":"in"},"values":{"duration":1}},
          {"id":15,"type":"var.add","params":{"variable":"Steps2"}},{"id":16,"type":"var.set","params":{"variable":"Late"},"values":{"value":true}},
          {"id":17,"type":"flow.tween","params":{"ease":"linear"},"values":{"duration":0.4}},{"id":18,"type":"var.add","params":{"variable":"Rounds"}}],
        "links":[[1,"then",2,"exec"],[1,"count",2,"value"],[3,"then",4,"exec"],[4,"completed",5,"exec"],
          [6,"then",14,"exec"],[14,"then0",7,"play"],[14,"then1",13,"play"],[14,"then2",17,"play"],[7,"update",8,"exec"],[7,"alpha",8,"value"],[8,"then",9,"exec"],[7,"finished",10,"exec"],[10,"then",11,"exec"],
          [13,"update",15,"exec"],[13,"finished",16,"exec"],[12,"then",13,"stop"],[17,"finished",18,"exec"],[18,"then",17,"play"]]})"},
      // A machine with an event and a timer, set directly too; the state events, Current State, Is In State.
      {"states",
       R"({"variables":[{"name":"Left","type":"int","value":0},{"name":"OpenTicks","type":"int","value":0}],
        "machines":[{"id":30,"name":"Door","states":[{"id":31,"name":"Closed","initial":true},{"id":32,"name":"Open"}],
          "transitions":[{"id":40,"from":31,"to":32,"trigger":"event","event":"Go"},{"id":41,"from":32,"to":31,"trigger":"timer","seconds":0.5}]}],
        "nodes":[{"id":1,"type":"event.state_exit","params":{"machine":"Door","state":"any"}},{"id":2,"type":"var.add","params":{"variable":"Left"}},
          {"id":3,"type":"event.state_enter","params":{"machine":"Door","state":"Open"}},{"id":4,"type":"debug.print"},{"id":5,"type":"text.append","values":{"a":"now "}},
          {"id":6,"type":"state.get","params":{"machine":"Door"}},
          {"id":7,"type":"event.tick"},{"id":8,"type":"flow.branch"},{"id":9,"type":"state.is","params":{"machine":"Door","state":"Open"}},{"id":10,"type":"var.add","params":{"variable":"OpenTicks"}},
          {"id":11,"type":"event.custom","params":{"name":"Open"}},{"id":12,"type":"state.set","params":{"machine":"Door","state":"Open"}}],
        "links":[[1,"then",2,"exec"],[3,"then",4,"exec"],[6,"state",5,"b"],[3,"previous",5,"c"],[5,"result",4,"text"],
          [7,"then",8,"exec"],[9,"result",8,"condition"],[8,"true",10,"exec"],[11,"then",12,"exec"]]})"},
      // Random: every node has a sequence of its own; Select reads both sides; a Random value passed on.
      {"random",
       R"({"variables":[{"name":"A","type":"float","value":0},{"name":"B","type":"int","value":0},{"name":"Heads","type":"int","value":0},{"name":"Picked","type":"float","value":0},
          {"name":"Sum","type":"float","value":0}],
        "nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"var.set","params":{"variable":"A"}},{"id":3,"type":"math.random","values":{"min":-1,"max":1}},
          {"id":4,"type":"var.set","params":{"variable":"B"}},{"id":5,"type":"math.random_int","values":{"min":3,"max":9}},
          {"id":6,"type":"flow.branch"},{"id":7,"type":"math.random_bool","values":{"chance":0.5}},{"id":8,"type":"var.add","params":{"variable":"Heads"}},
          {"id":9,"type":"var.set","params":{"variable":"Picked"}},{"id":10,"type":"logic.select"},{"id":11,"type":"math.random","values":{"min":10,"max":20}},
          {"id":12,"type":"math.random","values":{"min":100,"max":200}},{"id":13,"type":"math.compare","params":{"op":">"},"values":{"b":3}},
          {"id":14,"type":"var.add","params":{"variable":"Sum"}},{"id":15,"type":"math.add"},{"id":16,"type":"math.random"},{"id":17,"type":"math.mul","values":{"b":2}},
          {"id":18,"type":"math.random_int","values":{"min":1,"max":6}},{"id":19,"type":"flow.sequence","params":{"count":"2"}}],
        "links":[[1,"then",2,"exec"],[3,"result",2,"value"],[2,"then",4,"exec"],[5,"result",4,"value"],[4,"then",19,"exec"],[19,"then0",6,"exec"],[7,"result",6,"condition"],[6,"true",8,"exec"],
          [19,"then1",9,"exec"],[1,"value",13,"a"],[13,"result",10,"pick"],[11,"result",10,"a"],[12,"result",10,"b"],[10,"result",9,"value"],
          [9,"then",14,"exec"],[16,"result",17,"a"],[17,"result",15,"a"],[18,"result",15,"b"],[15,"result",14,"amount"]]})"},
      // Lists: the steps on list variables, the values that read lists, For Each - over a list its
      // body changes, and one whose body raises the event that starts the same loop again - a list
      // through Select, lists as text.
      {"lists",
       R"({"variables":[{"name":"Values","type":"float_list","value":[]},{"name":"Words","type":"text_list","value":["x"]},{"name":"Seen","type":"int_list","value":[]},
          {"name":"Trail","type":"vector_list","value":[[1,2,3]]},{"name":"Sum","type":"float","value":0},{"name":"Passes","type":"int","value":0},
          {"name":"Found","type":"int","value":0},{"name":"Has","type":"bool","value":false},{"name":"Shown","type":"text","value":""},
          {"name":"Depth","type":"int","value":0}],"nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"flow.for","values":{"first":1,"last":5}},
          {"id":5,"type":"list.add","params":{"variable":"Words"},"values":{"item":"last"}},{"id":10,"type":"event.custom","params":{"name":"Go"}},
          {"id":12,"type":"list.remove","params":{"variable":"Values"},"values":{"item":3}},{"id":14,"type":"flow.for_each","params":{"type":"float"}},
          {"id":20,"type":"event.custom","params":{"name":"Open"}},{"id":22,"type":"flow.for_each","params":{"type":"text"}},
          {"id":30,"type":"event.every","params":{"seconds":"0.5"}},{"id":31,"type":"list.add","params":{"variable":"Values"}},{"id":32,"type":"math.mul","values":{"b":1.5}},
          {"id":33,"type":"list.insert","params":{"variable":"Words"},"values":{"index":0,"item":"first"}},{"id":34,"type":"var.set","params":{"variable":"Found"}},
          {"id":35,"type":"list.add","params":{"variable":"Trail"}},{"id":36,"type":"vector.scale","values":{"b":2}},
          {"id":37,"type":"list.get","params":{"type":"vector"},"values":{"index":0}},{"id":38,"type":"var.get","params":{"variable":"Trail"}},
          {"id":39,"type":"list.add","params":{"variable":"Values"}},{"id":40,"type":"var.set","params":{"variable":"Has"}},
          {"id":41,"type":"var.set","params":{"variable":"Sum"},"values":{"value":0}},{"id":42,"type":"var.get","params":{"variable":"Values"}},
          {"id":43,"type":"var.add","params":{"variable":"Sum"}},{"id":44,"type":"var.add","params":{"variable":"Passes"},"values":{"amount":1}},{"id":45,"type":"flow.branch"},
          {"id":46,"type":"math.compare","params":{"op":">"},"values":{"b":6}},{"id":47,"type":"list.set","params":{"variable":"Values"},"values":{"item":0.5}},
          {"id":48,"type":"var.set","params":{"variable":"Shown"}},{"id":49,"type":"text.append","values":{"b":" "}},{"id":50,"type":"var.get","params":{"variable":"Values"}},
          {"id":51,"type":"list.find","params":{"type":"float"}},{"id":52,"type":"var.get","params":{"variable":"Values"}},
          {"id":53,"type":"list.add","params":{"variable":"Seen"}},{"id":54,"type":"list.length","params":{"type":"float"}},
          {"id":55,"type":"var.get","params":{"variable":"Values"}},{"id":56,"type":"debug.print","values":{"screen":false}},
          {"id":57,"type":"logic.select","params":{"type":"int_list"}},{"id":58,"type":"list.contains","params":{"type":"int"},"values":{"item":7}},
          {"id":59,"type":"var.get","params":{"variable":"Seen"}},{"id":60,"type":"var.get","params":{"variable":"Seen"}},
          {"id":61,"type":"list.make","params":{"type":"int","count":"2"}},{"id":62,"type":"var.get","params":{"variable":"Found"}},
          {"id":63,"type":"var.add","params":{"variable":"Depth"},"values":{"amount":1}},{"id":64,"type":"var.get","params":{"variable":"Words"}},
          {"id":65,"type":"var.set","params":{"variable":"Shown"}},{"id":66,"type":"text.append","values":{"b":"|"}},{"id":67,"type":"var.get","params":{"variable":"Shown"}},
          {"id":68,"type":"flow.branch"},{"id":69,"type":"math.compare","params":{"op":"<"},"values":{"b":2}},{"id":70,"type":"var.get","params":{"variable":"Depth"}},
          {"id":71,"type":"flow.call","params":{"name":"Open"},"values":{"target":0,"actor":0,"value":0,"text":""}},{"id":72,"type":"list.clear","params":{"variable":"Words"}},
          {"id":73,"type":"list.remove_at","params":{"variable":"Values"},"values":{"index":0}},{"id":74,"type":"list.add","params":{"variable":"Seen"}},
          {"id":75,"type":"list.insert","params":{"variable":"Trail"},"values":{"index":1}},
          {"id":76,"type":"vector.make","values":{"y":0,"z":-0.5}}],"links":[[1,"then",2,"exec"],[2,"body",31,"exec"],[2,"completed",33,"exec"],[2,"index",32,"a"],
          [5,"index",34,"value"],[5,"then",34,"exec"],[10,"then",39,"exec"],[10,"value",39,"item"],[10,"value",51,"item"],[12,"removed",40,"value"],[12,"then",40,"exec"],
          [14,"body",43,"exec"],[14,"completed",48,"exec"],[14,"index",47,"index"],[14,"index",61,"item0"],[14,"item",43,"amount"],[14,"item",46,"a"],[20,"then",63,"exec"],
          [22,"body",65,"exec"],[22,"completed",72,"exec"],[22,"item",66,"c"],[30,"count",74,"item"],[30,"count",76,"x"],[30,"then",73,"exec"],[32,"result",31,"item"],
          [33,"then",5,"exec"],[34,"then",35,"exec"],[36,"result",35,"item"],[37,"item",36,"a"],[38,"value",37,"list"],[39,"then",12,"exec"],[40,"then",41,"exec"],
          [41,"then",14,"exec"],[42,"value",14,"list"],[43,"then",44,"exec"],[44,"then",45,"exec"],[45,"true",47,"exec"],[46,"result",45,"condition"],[48,"then",53,"exec"],
          [49,"result",48,"value"],[50,"value",49,"a"],[51,"index",49,"c"],[52,"value",51,"list"],[53,"then",56,"exec"],[54,"length",53,"item"],[55,"value",54,"list"],
          [57,"result",56,"text"],[58,"result",57,"pick"],[59,"value",58,"list"],[60,"value",57,"a"],[61,"list",57,"b"],[62,"value",61,"item1"],[63,"then",22,"exec"],
          [64,"value",22,"list"],[65,"then",68,"exec"],[66,"result",65,"value"],[67,"value",66,"a"],[68,"true",71,"exec"],[69,"result",68,"condition"],[70,"value",69,"a"],
          [73,"then",74,"exec"],[74,"then",75,"exec"],[76,"result",75,"item"]]})"},
      // Enums: a variable and a list of an enum's type, Switch on Enum, the conversions to and from
      // an index and a name (what names no value is the first), all values in a For Each, Select.
      {"enums",
       R"({"enums":[{"name":"Light","values":["Red","Amber","Green"]}],"variables":[{"name":"Now","type":"enum","of":"Light","value":0},
          {"name":"History","type":"enum_list","of":"Light","value":[]},{"name":"Reds","type":"int","value":0},{"name":"Changes","type":"int","value":0},
          {"name":"Shown","type":"text","value":""},{"name":"Valid","type":"bool","value":false}],"nodes":[{"id":1,"type":"event.begin"},
          {"id":2,"type":"flow.for_each","params":{"type":"Light"}},{"id":10,"type":"event.custom","params":{"name":"Go"}},
          {"id":20,"type":"event.custom","params":{"name":"Open"}},{"id":30,"type":"event.every","params":{"seconds":"0.5"}},
          {"id":31,"type":"enum.values","params":{"enum":"Light"}},{"id":32,"type":"var.set","params":{"variable":"Shown"}},{"id":33,"type":"text.append","values":{"c":","}},
          {"id":34,"type":"var.get","params":{"variable":"Shown"}},{"id":35,"type":"enum.name","params":{"enum":"Light"}},
          {"id":36,"type":"var.set","params":{"variable":"Valid"}},{"id":37,"type":"enum.from_index","params":{"enum":"Light"}},
          {"id":38,"type":"var.set","params":{"variable":"Now"}},{"id":39,"type":"enum.from_index","params":{"enum":"Light"}},
          {"id":40,"type":"list.add","params":{"variable":"History"}},{"id":41,"type":"var.get","params":{"variable":"Now"}},
          {"id":42,"type":"flow.switch_enum","params":{"enum":"Light"}},{"id":43,"type":"var.get","params":{"variable":"Now"}},
          {"id":44,"type":"var.add","params":{"variable":"Reds"},"values":{"amount":1}},{"id":45,"type":"var.set","params":{"variable":"Now"},"values":{"value":2}},
          {"id":46,"type":"var.set","params":{"variable":"Now"}},{"id":47,"type":"logic.select","params":{"type":"Light"},"values":{"a":0,"b":1}},
          {"id":48,"type":"list.contains","params":{"type":"Light"},"values":{"item":1}},{"id":49,"type":"var.get","params":{"variable":"History"}},
          {"id":50,"type":"var.add","params":{"variable":"Changes"},"values":{"amount":1}},{"id":51,"type":"var.set","params":{"variable":"Shown"}},
          {"id":52,"type":"text.append","values":{"b":"/"}},{"id":53,"type":"enum.name","params":{"enum":"Light"}},{"id":54,"type":"var.get","params":{"variable":"Now"}},
          {"id":55,"type":"list.length","params":{"type":"Light"}},{"id":56,"type":"var.get","params":{"variable":"History"}},
          {"id":57,"type":"debug.print","values":{"screen":false}},{"id":58,"type":"text.append"},{"id":59,"type":"enum.name","params":{"enum":"Light"}},
          {"id":60,"type":"list.get","params":{"type":"Light"},"values":{"index":0}},{"id":61,"type":"var.get","params":{"variable":"History"}},
          {"id":62,"type":"enum.index","params":{"enum":"Light"}},{"id":63,"type":"var.get","params":{"variable":"Now"}},
          {"id":64,"type":"enum.equal","params":{"enum":"Light"},"values":{"b":0}},{"id":65,"type":"var.get","params":{"variable":"Now"}},
          {"id":66,"type":"var.set","params":{"variable":"Now"}},{"id":67,"type":"enum.from_name","params":{"enum":"Light"},"values":{"name":"Amber"}},
          {"id":68,"type":"list.clear","params":{"variable":"History"}},{"id":69,"type":"var.set","params":{"variable":"Now"}},
          {"id":70,"type":"enum.from_index","params":{"enum":"Light"}},{"id":71,"type":"math.round"},{"id":72,"type":"math.mod","values":{"b":3}},
          {"id":73,"type":"list.insert","params":{"variable":"History"},"values":{"index":0}},
          {"id":74,"type":"var.get","params":{"variable":"Now"}}],"links":[[1,"then",2,"exec"],[2,"body",32,"exec"],[2,"item",35,"value"],[10,"then",36,"exec"],
          [10,"value",37,"index"],[10,"value",39,"index"],[20,"then",66,"exec"],[30,"count",72,"a"],[30,"then",69,"exec"],[31,"values",2,"list"],[33,"result",32,"value"],
          [34,"value",33,"a"],[35,"name",33,"b"],[36,"then",38,"exec"],[37,"valid",36,"value"],[38,"then",40,"exec"],[39,"value",38,"value"],[40,"then",42,"exec"],
          [41,"value",40,"item"],[42,"case0",44,"exec"],[42,"case1",45,"exec"],[42,"case2",46,"exec"],[43,"value",42,"value"],[44,"then",50,"exec"],[45,"then",50,"exec"],
          [46,"then",50,"exec"],[47,"result",46,"value"],[48,"result",47,"pick"],[49,"value",48,"list"],[50,"then",51,"exec"],[51,"then",57,"exec"],[52,"result",51,"value"],
          [53,"name",52,"a"],[54,"value",53,"value"],[55,"length",52,"c"],[56,"value",55,"list"],[58,"result",57,"text"],[59,"name",58,"a"],[60,"item",59,"value"],
          [61,"value",60,"list"],[62,"index",58,"b"],[63,"value",62,"value"],[64,"result",58,"c"],[65,"value",64,"a"],[66,"then",68,"exec"],[67,"value",66,"value"],
          [69,"then",73,"exec"],[70,"value",69,"value"],[71,"result",70,"index"],[72,"result",71,"a"],[74,"value",73,"item"]]})"},
      // Structs: variables and a list of a struct's type - a struct with an enum's value, another
      // struct and lists inside it. Make, Break and Set Field; the list nodes compare structs field
      // by field; an item from beyond a list is the struct's default; Select and For Each.
      {"structs",
       R"({"enums":[{"name":"Phase","values":["Idle","Run","Done"]}],"structs":[
          {"name":"Step","fields":[{"name":"Label","type":"text","value":"s"},{"name":"Cost","type":"float","value":1.5}]},
          {"name":"Job","fields":[{"name":"Name","type":"text","value":"job"},{"name":"State","type":"enum","of":"Phase","value":0},{"name":"Left","type":"int","value":3},{"name":"Now","type":"struct","of":"Step","value":{"Label":"first","Cost":1.5}},{"name":"Costs","type":"float_list","value":[1]},{"name":"Plan","type":"struct_list","of":"Step","value":[]}]}],"variables":[
          {"name":"Work","type":"struct","of":"Job","value":{"Name":"A","State":0,"Left":3,"Now":{"Label":"first","Cost":1.5},"Costs":[1],"Plan":[]}},
          {"name":"Queue","type":"struct_list","of":"Job","value":[]},{"name":"Spent","type":"float","value":0},{"name":"Shown","type":"text","value":""},
          {"name":"Hits","type":"int","value":0},{"name":"Same","type":"bool","value":false},{"name":"Kept","type":"struct","of":"Step","value":{"Label":"s","Cost":1.5}}],"nodes":[
          {"id":1,"type":"event.begin"},{"id":10,"type":"event.custom","params":{"name":"Go"}},{"id":20,"type":"event.custom","params":{"name":"Open"}},
          {"id":21,"type":"flow.for_each","params":{"type":"Job"}},{"id":30,"type":"event.every","params":{"seconds":"0.5"}},
          {"id":31,"type":"struct.set_field","params":{"variable":"Work","field":"Now"}},
          {"id":32,"type":"struct.make","params":{"struct":"Step"},"values":{"Label":"begin","Cost":2.5}},{"id":33,"type":"list.add","params":{"variable":"Queue"}},
          {"id":34,"type":"var.get","params":{"variable":"Work"}},{"id":35,"type":"struct.set_field","params":{"variable":"Work","field":"Left"}},{"id":36,"type":"math.round"},
          {"id":37,"type":"math.sub","values":{"b":1}},{"id":38,"type":"struct.break","params":{"struct":"Job"}},{"id":39,"type":"var.get","params":{"variable":"Work"}},
          {"id":40,"type":"struct.set_field","params":{"variable":"Work","field":"State"}},{"id":41,"type":"enum.from_index","params":{"enum":"Phase"}},
          {"id":42,"type":"math.round"},{"id":43,"type":"math.mod","values":{"b":3}},{"id":44,"type":"struct.set_field","params":{"variable":"Work","field":"Costs"}},
          {"id":45,"type":"list.make","params":{"type":"float","count":"2"}},{"id":46,"type":"math.mul","values":{"b":2}},{"id":47,"type":"var.add","params":{"variable":"Spent"}},
          {"id":48,"type":"struct.break","params":{"struct":"Step"}},{"id":49,"type":"struct.break","params":{"struct":"Job"}},
          {"id":50,"type":"var.get","params":{"variable":"Work"}},{"id":51,"type":"var.set","params":{"variable":"Same"}},{"id":52,"type":"list.contains","params":{"type":"Job"}},
          {"id":53,"type":"var.get","params":{"variable":"Queue"}},{"id":54,"type":"var.get","params":{"variable":"Work"}},
          {"id":55,"type":"list.add","params":{"variable":"Queue"}},{"id":56,"type":"var.get","params":{"variable":"Work"}},
          {"id":57,"type":"var.set","params":{"variable":"Shown"}},{"id":58,"type":"text.append"},{"id":59,"type":"var.get","params":{"variable":"Shown"}},
          {"id":60,"type":"enum.name","params":{"enum":"Phase"}},{"id":61,"type":"struct.break","params":{"struct":"Job"}},
          {"id":62,"type":"list.get","params":{"type":"Job"},"values":{"index":1}},{"id":63,"type":"var.get","params":{"variable":"Queue"}},
          {"id":64,"type":"list.find","params":{"type":"Job"}},{"id":65,"type":"var.get","params":{"variable":"Queue"}},{"id":66,"type":"var.get","params":{"variable":"Work"}},
          {"id":67,"type":"var.get","params":{"variable":"Queue"}},{"id":68,"type":"var.add","params":{"variable":"Hits"}},
          {"id":69,"type":"struct.break","params":{"struct":"Job"}},{"id":70,"type":"list.remove","params":{"variable":"Queue"}},
          {"id":71,"type":"list.get","params":{"type":"Job"},"values":{"index":0}},{"id":72,"type":"var.get","params":{"variable":"Queue"}},
          {"id":73,"type":"var.set","params":{"variable":"Work"}},{"id":74,"type":"logic.select","params":{"type":"Job"}},{"id":75,"type":"var.get","params":{"variable":"Same"}},
          {"id":76,"type":"list.get","params":{"type":"Job"},"values":{"index":99}},{"id":77,"type":"var.get","params":{"variable":"Queue"}},
          {"id":78,"type":"var.get","params":{"variable":"Work"}},{"id":79,"type":"var.set","params":{"variable":"Kept"}},{"id":80,"type":"struct.break","params":{"struct":"Job"}},
          {"id":81,"type":"var.get","params":{"variable":"Work"}},{"id":82,"type":"struct.set_field","params":{"variable":"Work","field":"Plan"}},
          {"id":83,"type":"list.make","params":{"type":"Step","count":"2"}},{"id":84,"type":"struct.break","params":{"struct":"Job"}},
          {"id":85,"type":"var.get","params":{"variable":"Work"}},{"id":86,"type":"struct.make","params":{"struct":"Step"}},
          {"id":87,"type":"text.append","values":{"a":"t","c":""}},{"id":88,"type":"struct.set_field","params":{"variable":"Work","field":"Now"}},
          {"id":89,"type":"list.get","params":{"type":"Step"},"values":{"index":1}},{"id":90,"type":"struct.break","params":{"struct":"Job"}},
          {"id":91,"type":"var.get","params":{"variable":"Work"}},{"id":92,"type":"struct.set_field","params":{"variable":"Work","field":"Name"}},
          {"id":93,"type":"struct.break","params":{"struct":"Step"}},{"id":94,"type":"var.get","params":{"variable":"Kept"}}],"links":[[1,"then",31,"exec"],[10,"then",35,"exec"],
          [10,"value",43,"a"],[10,"value",45,"item0"],[10,"value",46,"a"],[20,"then",21,"exec"],[21,"body",68,"exec"],[21,"completed",70,"exec"],[21,"item",69,"value"],
          [30,"count",86,"Cost"],[30,"count",87,"b"],[30,"then",82,"exec"],[31,"then",33,"exec"],[32,"value",31,"value"],[34,"value",33,"item"],[35,"then",40,"exec"],
          [36,"result",35,"value"],[37,"result",36,"a"],[38,"Left",37,"a"],[39,"value",38,"value"],[40,"then",44,"exec"],[41,"value",40,"value"],[42,"result",41,"index"],
          [43,"result",42,"a"],[44,"then",47,"exec"],[45,"list",44,"value"],[46,"result",45,"item1"],[47,"then",51,"exec"],[48,"Cost",47,"amount"],[49,"Now",48,"value"],
          [50,"value",49,"value"],[51,"then",55,"exec"],[52,"result",51,"value"],[53,"value",52,"list"],[54,"value",52,"item"],[55,"then",57,"exec"],[56,"value",55,"item"],
          [58,"result",57,"value"],[59,"value",58,"a"],[60,"name",58,"b"],[61,"State",60,"value"],[62,"item",61,"value"],[63,"value",62,"list"],[64,"index",58,"c"],
          [65,"value",64,"list"],[66,"value",64,"item"],[67,"value",21,"list"],[69,"Left",68,"amount"],[70,"then",73,"exec"],[71,"item",70,"item"],[72,"value",71,"list"],
          [73,"then",79,"exec"],[74,"result",73,"value"],[75,"value",74,"pick"],[76,"item",74,"a"],[77,"value",76,"list"],[78,"value",74,"b"],[80,"Now",79,"value"],
          [81,"value",80,"value"],[82,"then",88,"exec"],[83,"list",82,"value"],[84,"Now",83,"item0"],[85,"value",84,"value"],[86,"value",83,"item1"],[87,"result",86,"Label"],
          [88,"then",92,"exec"],[89,"item",88,"value"],[90,"Plan",89,"list"],[91,"value",90,"value"],[93,"Label",92,"value"],[94,"value",93,"value"]]})"},
      {"infinite_loop",
       R"({"variables":[{"name":"N","type":"int","value":0}],
        "nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"flow.while","values":{"condition":true}},{"id":3,"type":"var.add","params":{"variable":"N"}}],
        "links":[[1,"then",2,"exec"],[2,"body",3,"exec"]]})"},
      // Local variables of functions: every call starts with their defaults (numbers in slots, a
      // text); two functions have a Total; a pure function sets its own; Outer raises an event
      // whose handler calls Outer again - the interrupted call has its own Own afterwards.
      {"locals",
       R"({"nextId":94,"variables":[{"name":"Sum","type":"float","value":0},{"name":"Steps","type":"int","value":0},{"name":"Kept","type":"float","value":0},{"name":"Got","type":"float","value":0},
        {"name":"Shown","type":"text","value":""}],"nodes":[{"id":10,"type":"flow.for","values":{"first":1}},{"id":40,"type":"event.custom","params":{"name":"Again"}},
        {"id":41,"type":"func.call","params":{"function":"Outer"},"values":{"again":false}},{"id":50,"type":"event.custom","params":{"name":"Go"}},
        {"id":51,"type":"func.call","params":{"function":"SumTo"}},{"id":54,"type":"func.call","params":{"function":"Outer"},"values":{"again":true}},
        {"id":56,"type":"func.call","params":{"function":"Join"}},{"id":57,"type":"func.entry","params":{"function":"SumTo"}},
        {"id":58,"type":"var.add","params":{"variable":"Total","scope":"SumTo"}},{"id":59,"type":"var.add","params":{"variable":"Count","scope":"SumTo"},"values":{"amount":1}},
        {"id":60,"type":"func.return","params":{"function":"SumTo"}},{"id":61,"type":"var.get","params":{"variable":"Total","scope":"SumTo"}},
        {"id":62,"type":"var.get","params":{"variable":"Count","scope":"SumTo"}},{"id":63,"type":"func.entry","params":{"function":"Limit"}},
        {"id":64,"type":"var.set","params":{"variable":"Out","scope":"Limit"}},{"id":65,"type":"flow.branch"},{"id":66,"type":"math.compare","params":{"op":">"},"values":{"b":1}},
        {"id":67,"type":"var.set","params":{"variable":"Out","scope":"Limit"},"values":{"value":1}},{"id":68,"type":"func.return","params":{"function":"Limit"}},
        {"id":69,"type":"var.get","params":{"variable":"Out","scope":"Limit"}},{"id":70,"type":"func.entry","params":{"function":"Outer"}},
        {"id":71,"type":"var.add","params":{"variable":"Own","scope":"Outer"}},{"id":72,"type":"math.mul","values":{"b":10}},{"id":73,"type":"flow.branch"},
        {"id":74,"type":"flow.call","params":{"name":"Again"},"values":{"target":0,"actor":0,"value":5,"text":""}},{"id":75,"type":"func.return","params":{"function":"Outer"}},
        {"id":76,"type":"math.add"},{"id":77,"type":"var.get","params":{"variable":"Own","scope":"Outer"}},{"id":78,"type":"func.pure","params":{"function":"Limit"}},
        {"id":79,"type":"var.get","params":{"variable":"Own","scope":"Outer"}},{"id":80,"type":"func.entry","params":{"function":"Join"}},
        {"id":81,"type":"var.set","params":{"variable":"Total","scope":"Join"}},{"id":82,"type":"text.append","values":{"c":">"}},
        {"id":83,"type":"var.get","params":{"variable":"Total","scope":"Join"}},{"id":84,"type":"func.return","params":{"function":"Join"}},
        {"id":85,"type":"var.get","params":{"variable":"Total","scope":"Join"}},{"id":86,"type":"var.set","params":{"variable":"Kept"}},{"id":87,"type":"var.add","params":{"variable":"Sum"}},
        {"id":88,"type":"var.add","params":{"variable":"Steps"}},{"id":89,"type":"var.set","params":{"variable":"Got"}},{"id":90,"type":"func.pure","params":{"function":"Limit"}},
        {"id":91,"type":"var.set","params":{"variable":"Shown"}},{"id":92,"type":"text.append","values":{"c":""}},
        {"id":93,"type":"var.get","params":{"variable":"Shown"}}],"links":[[10,"body",58,"exec"],[10,"completed",60,"exec"],[10,"index",58,"amount"],[40,"then",41,"exec"],[40,"value",41,"x"],[41,"mine",86,"value"],[41,"then",86,"exec"],[50,"then",51,"exec"],[50,"value",51,"n"],[50,"value",54,"x"],[50,"value",90,"x"],[51,"steps",88,"amount"],[51,"sum",87,"amount"],[51,"then",87,"exec"],[54,"mine",89,"value"],[54,"then",89,"exec"],[56,"text",92,"b"],[56,"then",91,"exec"],[57,"n",10,"last"],[57,"then",10,"exec"],[58,"then",59,"exec"],[61,"value",60,"sum"],[62,"value",60,"steps"],[63,"then",64,"exec"],[63,"x",64,"value"],[63,"x",66,"a"],[64,"then",65,"exec"],[65,"false",68,"exec"],[65,"true",67,"exec"],[66,"result",65,"condition"],[67,"then",68,"exec"],[69,"value",68,"out"],[70,"again",73,"condition"],[70,"then",71,"exec"],[70,"x",72,"a"],[71,"then",73,"exec"],[72,"result",71,"amount"],[73,"false",75,"exec"],[73,"true",74,"exec"],[74,"then",75,"exec"],[76,"result",75,"mine"],[77,"value",76,"a"],[78,"out",76,"b"],[79,"value",78,"x"],[80,"a",82,"b"],[80,"then",81,"exec"],[81,"then",84,"exec"],[82,"result",81,"value"],[83,"value",82,"a"],[85,"value",84,"text"],[87,"then",88,"exec"],[88,"then",54,"exec"],[89,"then",56,"exec"],[90,"out",56,"a"],[92,"result",91,"value"],[93,"value",92,"a"]],"functions":[{"name":"SumTo","inputs":[{"name":"n","type":"int","value":3}],"outputs":[{"name":"sum","type":"float","value":0},
        {"name":"steps","type":"int","value":0}],"locals":[{"name":"Total","type":"float","value":0.5},{"name":"Count","type":"int","value":0}]},
        {"name":"Limit","pure":true,"inputs":[{"name":"x","type":"float","value":0}],"outputs":[{"name":"out","type":"float","value":0}],"locals":[{"name":"Out","type":"float","value":-1}]},
        {"name":"Outer","inputs":[{"name":"x","type":"float","value":1},
        {"name":"again","type":"bool","value":false}],"outputs":[{"name":"mine","type":"float","value":0}],"locals":[{"name":"Own","type":"float","value":0.25}]},
        {"name":"Join","inputs":[{"name":"a","type":"text","value":"a"}],"outputs":[{"name":"text","type":"text","value":""}],"locals":[{"name":"Total","type":"text","value":"<"}]}]})"},
      // Lists of a fixed length (Fixed<Float, 3>): a variable, a field of a struct - also of structs
      // in a fixed list -, an input, an output and a local variable of a function, a reference to
      // one. Entries are set and read, also beside the list; a list set there as a whole is cut or
      // filled up (a longer list, a shorter one, Make List, Make Struct, a Take leaves the zeros);
      // read as a whole it is a list.
      {"fixed_lists",
       R"({"structs":[{"name":"Mat","fields":[{"name":"m","type":"float_list","count":4,"value":[1,0,0,1]},{"name":"Name","type":"text","value":"m"}]},
          {"name":"Rig","fields":[{"name":"Bones","type":"struct_list","of":"Mat","count":2,"value":[{"m":[1,0,0,1],"Name":"a"},{"m":[2,0,0,2],"Name":"b"}]},{"name":"Free","type":"float_list","value":[]}]}],"variables":[
          {"name":"Axis","type":"float_list","count":3,"value":[1,2,3]},{"name":"Tags","type":"text_list","count":2,"value":["x","y"]},
          {"name":"Bits","type":"bool_list","count":3,"value":[true,false,true]},{"name":"Now","type":"struct","of":"Mat","value":{"m":[1,0,0,1],"Name":"now"}},
          {"name":"Body","type":"struct","of":"Rig","value":{"Bones":[{"m":[1,0,0,1],"Name":"a"},{"m":[2,0,0,2],"Name":"b"}],"Free":[]}},
          {"name":"Any","type":"float_list","value":[9]},{"name":"Shown","type":"text","value":""},{"name":"Sum","type":"float","value":0}],"nodes":[
          {"id":3,"type":"flow.for","values":{"first":0,"last":2}},{"id":10,"type":"event.custom","params":{"name":"Go"}},
          {"id":12,"type":"func.call","params":{"function":"Scaled"}},{"id":30,"type":"event.custom","params":{"name":"Open"}},
          {"id":31,"type":"var.take","params":{"variable":"Axis"}},{"id":34,"type":"flow.for_each","params":{"type":"text"}},
          {"id":35,"type":"func.entry","params":{"function":"Mul"}},{"id":36,"type":"var.set","params":{"variable":"R","scope":"Mul","path":".m[]"},"values":{"path0":0}},
          {"id":37,"type":"math.add"},{"id":38,"type":"math.mul"},{"id":39,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":0}},
          {"id":40,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":0}},{"id":41,"type":"math.mul"},
          {"id":42,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":1}},
          {"id":43,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":2}},
          {"id":44,"type":"var.set","params":{"variable":"R","scope":"Mul","path":".m[]"},"values":{"path0":1}},{"id":45,"type":"math.add"},{"id":46,"type":"math.mul"},
          {"id":47,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":0}},
          {"id":48,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":1}},{"id":49,"type":"math.mul"},
          {"id":50,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":1}},
          {"id":51,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":3}},
          {"id":52,"type":"var.set","params":{"variable":"R","scope":"Mul","path":".m[]"},"values":{"path0":2}},{"id":53,"type":"math.add"},{"id":54,"type":"math.mul"},
          {"id":55,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":2}},
          {"id":56,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":0}},{"id":57,"type":"math.mul"},
          {"id":58,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":3}},
          {"id":59,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":2}},
          {"id":60,"type":"var.set","params":{"variable":"R","scope":"Mul","path":".m[]"},"values":{"path0":3}},{"id":61,"type":"math.add"},{"id":62,"type":"math.mul"},
          {"id":63,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":2}},
          {"id":64,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":1}},{"id":65,"type":"math.mul"},
          {"id":66,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".m[]"},"values":{"path0":3}},
          {"id":67,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".m[]"},"values":{"path0":3}},
          {"id":68,"type":"var.set","params":{"variable":"R","scope":"Mul","path":".Name"}},{"id":69,"type":"text.append","values":{"b":"*"}},
          {"id":70,"type":"var.get","params":{"variable":"a","scope":"Mul","path":".Name"}},{"id":71,"type":"var.get","params":{"variable":"b","scope":"Mul","path":".Name"}},
          {"id":72,"type":"func.return","params":{"function":"Mul"}},{"id":73,"type":"var.get","params":{"variable":"R","scope":"Mul"}},
          {"id":74,"type":"func.entry","params":{"function":"Scaled"}},{"id":75,"type":"var.set","params":{"variable":"Made","scope":"Scaled","path":"[]"}},
          {"id":76,"type":"math.mul"},{"id":77,"type":"list.get","params":{"type":"float"}},{"id":78,"type":"func.return","params":{"function":"Scaled"}},
          {"id":79,"type":"var.get","params":{"variable":"Made","scope":"Scaled"}},{"id":80,"type":"func.entry","params":{"function":"Bump"}},
          {"id":81,"type":"var.add","params":{"variable":"axis","scope":"Bump","path":"[]"},"values":{"path0":0}},
          {"id":82,"type":"var.set","params":{"variable":"axis","scope":"Bump","path":"[]"},"values":{"path0":5,"value":7}},
          {"id":83,"type":"var.set","params":{"variable":"Axis","path":"[]"}},{"id":84,"type":"var.get","params":{"variable":"Axis"}},
          {"id":85,"type":"var.set","params":{"variable":"Axis"}},{"id":86,"type":"var.set","params":{"variable":"Any"}},{"id":87,"type":"var.get","params":{"variable":"Axis"}},
          {"id":88,"type":"list.add","params":{"variable":"Any"}},{"id":89,"type":"var.set","params":{"variable":"Axis"}},{"id":90,"type":"var.get","params":{"variable":"Any"}},
          {"id":91,"type":"var.set","params":{"variable":"Tags"}},{"id":92,"type":"list.make","params":{"type":"text","count":"1"}},
          {"id":93,"type":"func.call","params":{"function":"Bump","axis.variable":"Axis"},"values":{"by":0.5}},{"id":94,"type":"var.set","params":{"variable":"Now","path":".m"}},
          {"id":95,"type":"list.make","params":{"type":"float","count":"4"},"values":{"item1":1,"item2":2,"item3":3}},
          {"id":96,"type":"var.set","params":{"variable":"Body","path":".Bones[].m[]"},"values":{"path0":1,"path1":3}},
          {"id":97,"type":"var.set","params":{"variable":"Body","path":".Bones[].Name"},"values":{"value":"hit"}},{"id":98,"type":"var.set","params":{"variable":"Now"}},
          {"id":99,"type":"func.pure","params":{"function":"Mul","a.variable":"Now","b.variable":"Body","b.path":".Bones[]"},"values":{"b.path0":0}},
          {"id":100,"type":"var.set","params":{"variable":"Bits","path":"[]"},"values":{"path0":1}},{"id":101,"type":"math.compare","params":{"op":">"},"values":{"b":3}},
          {"id":102,"type":"var.add","params":{"variable":"Sum"}},{"id":103,"type":"math.add"},{"id":104,"type":"math.add"},
          {"id":105,"type":"list.length","params":{"type":"float"}},{"id":106,"type":"var.get","params":{"variable":"Axis"}},
          {"id":107,"type":"list.get","params":{"type":"float"},"values":{"index":3}},{"id":108,"type":"var.get","params":{"variable":"Now","path":".m"}},
          {"id":109,"type":"list.find","params":{"type":"float"}},{"id":110,"type":"var.get","params":{"variable":"Axis"}},
          {"id":111,"type":"var.set","params":{"variable":"Shown"}},{"id":112,"type":"text.append"},{"id":113,"type":"var.get","params":{"variable":"Shown"}},
          {"id":114,"type":"var.get","params":{"variable":"Axis"}},{"id":115,"type":"list.contains","params":{"type":"text"},"values":{"item":""}},
          {"id":116,"type":"var.get","params":{"variable":"Tags"}},{"id":117,"type":"var.set","params":{"variable":"Any"}},{"id":118,"type":"var.set","params":{"variable":"Body"}},
          {"id":119,"type":"struct.make","params":{"struct":"Rig"}},{"id":120,"type":"list.make","params":{"type":"Mat","count":"1"}},
          {"id":121,"type":"var.get","params":{"variable":"Now"}},{"id":122,"type":"var.get","params":{"variable":"Any"}},{"id":123,"type":"var.get","params":{"variable":"Tags"}},
          {"id":124,"type":"var.set","params":{"variable":"Shown"}},{"id":125,"type":"text.append","values":{"b":"|"}},{"id":126,"type":"var.get","params":{"variable":"Shown"}},
          {"id":127,"type":"var.set","params":{"variable":"Now","path":".m"}},{"id":128,"type":"var.get","params":{"variable":"Any"}},
          {"id":129,"type":"var.set","params":{"variable":"Shown"}},{"id":130,"type":"text.append"},{"id":131,"type":"var.get","params":{"variable":"Shown"}},
          {"id":132,"type":"var.get","params":{"variable":"Bits"}},{"id":133,"type":"var.get","params":{"variable":"Body","path":".Bones[].m"},"values":{"path0":1}}],"links":[
          [3,"body",75,"exec"],[3,"completed",78,"exec"],[3,"index",75,"path0"],[3,"index",77,"index"],[10,"then",83,"exec"],[10,"value",12,"by"],[10,"value",83,"path0"],
          [10,"value",83,"value"],[10,"value",88,"item"],[10,"value",92,"item0"],[10,"value",95,"item0"],[10,"value",96,"value"],[10,"value",97,"path0"],[10,"value",101,"a"],
          [10,"value",109,"item"],[12,"then",85,"exec"],[12,"v",85,"value"],[30,"then",31,"exec"],[31,"then",117,"exec"],[31,"value",117,"value"],[34,"body",124,"exec"],
          [34,"completed",127,"exec"],[34,"item",125,"c"],[35,"then",36,"exec"],[36,"then",44,"exec"],[37,"result",36,"value"],[38,"result",37,"a"],[39,"value",38,"a"],
          [40,"value",38,"b"],[41,"result",37,"b"],[42,"value",41,"a"],[43,"value",41,"b"],[44,"then",52,"exec"],[45,"result",44,"value"],[46,"result",45,"a"],[47,"value",46,"a"],
          [48,"value",46,"b"],[49,"result",45,"b"],[50,"value",49,"a"],[51,"value",49,"b"],[52,"then",60,"exec"],[53,"result",52,"value"],[54,"result",53,"a"],[55,"value",54,"a"],
          [56,"value",54,"b"],[57,"result",53,"b"],[58,"value",57,"a"],[59,"value",57,"b"],[60,"then",68,"exec"],[61,"result",60,"value"],[62,"result",61,"a"],[63,"value",62,"a"],
          [64,"value",62,"b"],[65,"result",61,"b"],[66,"value",65,"a"],[67,"value",65,"b"],[68,"then",72,"exec"],[69,"result",68,"value"],[70,"value",69,"a"],[71,"value",69,"c"],
          [73,"value",72,"out"],[74,"by",76,"b"],[74,"then",3,"exec"],[74,"v",77,"list"],[76,"result",75,"value"],[77,"item",76,"a"],[79,"value",78,"v"],[80,"by",81,"amount"],
          [80,"then",81,"exec"],[81,"then",82,"exec"],[83,"then",12,"exec"],[84,"value",12,"v"],[85,"then",86,"exec"],[86,"then",88,"exec"],[87,"value",86,"value"],
          [88,"then",89,"exec"],[89,"then",91,"exec"],[90,"value",89,"value"],[91,"then",93,"exec"],[92,"list",91,"value"],[93,"then",94,"exec"],[94,"then",96,"exec"],
          [95,"list",94,"value"],[96,"then",97,"exec"],[97,"then",98,"exec"],[98,"then",100,"exec"],[99,"out",98,"value"],[100,"then",102,"exec"],[101,"result",100,"value"],
          [102,"then",111,"exec"],[103,"result",102,"amount"],[104,"result",103,"a"],[105,"length",104,"a"],[106,"value",105,"list"],[107,"item",104,"b"],[108,"value",107,"list"],
          [109,"index",103,"b"],[110,"value",109,"list"],[112,"result",111,"value"],[113,"value",112,"a"],[114,"value",112,"b"],[115,"result",112,"c"],[116,"value",115,"list"],
          [117,"then",118,"exec"],[118,"then",34,"exec"],[119,"value",118,"value"],[120,"list",119,"Bones"],[121,"value",120,"item0"],[122,"value",119,"Free"],
          [123,"value",34,"list"],[125,"result",124,"value"],[126,"value",125,"a"],[127,"then",129,"exec"],[128,"value",127,"value"],[130,"result",129,"value"],
          [131,"value",130,"a"],[132,"value",130,"b"],[133,"value",130,"c"]],"functions":[
          {"name":"Mul","pure":true,"inputs":[{"name":"a","type":"struct","of":"Mat","ref":"const","value":{"m":[1,0,0,1],"Name":"m"}},{"name":"b","type":"struct","of":"Mat","ref":"const","value":{"m":[1,0,0,1],"Name":"m"}}],"outputs":[{"name":"out","type":"struct","of":"Mat","value":{"m":[1,0,0,1],"Name":"m"}}],"locals":[{"name":"R","type":"struct","of":"Mat","value":{"m":[1,0,0,1],"Name":"m"}}]},
          {"name":"Scaled","inputs":[{"name":"v","type":"float_list","count":3,"value":[1,1,1]},{"name":"by","type":"float","value":2}],"outputs":[{"name":"v","type":"float_list","count":3,"value":[]}],"locals":[{"name":"Made","type":"float_list","count":3,"value":[]}]},
          {"name":"Bump","inputs":[{"name":"axis","type":"float_list","count":3,"ref":true,"value":[]},{"name":"by","type":"float","value":1}],"outputs":[]}]})"},
      // Sort List: a list of structs in the order a pure function says (more kills first, then fewer
      // deaths; equal ones as they stood), numbers and texts by their own order.
      {"list_sort",
       R"({"structs":[{"name":"Score","fields":[{"name":"Name","type":"text","value":""},{"name":"Kills","type":"int","value":0},{"name":"Deaths","type":"int","value":0}]}],"variables":[
          {"name":"Scores","type":"struct_list","of":"Score","value":[]},{"name":"Times","type":"float_list","value":[3.5,1,2,1]},
          {"name":"Names","type":"text_list","value":["c","a","b"]},{"name":"Shown","type":"text","value":""}],"nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},
          {"id":2,"type":"func.entry","params":{"function":"Before"}},{"id":3,"type":"func.return","params":{"function":"Before"}},{"id":4,"type":"logic.or"},
          {"id":5,"type":"math.compare","params":{"op":">"}},{"id":6,"type":"var.get","params":{"variable":"a","scope":"Before","path":".Kills"}},
          {"id":7,"type":"var.get","params":{"variable":"b","scope":"Before","path":".Kills"}},{"id":8,"type":"logic.and"},{"id":9,"type":"math.compare","params":{"op":"=="}},
          {"id":10,"type":"var.get","params":{"variable":"a","scope":"Before","path":".Kills"}},
          {"id":11,"type":"var.get","params":{"variable":"b","scope":"Before","path":".Kills"}},{"id":12,"type":"math.compare","params":{"op":"<"}},
          {"id":13,"type":"var.get","params":{"variable":"a","scope":"Before","path":".Deaths"}},
          {"id":14,"type":"var.get","params":{"variable":"b","scope":"Before","path":".Deaths"}},{"id":15,"type":"list.add","params":{"variable":"Scores"}},
          {"id":16,"type":"struct.make","params":{"struct":"Score"},"values":{"Name":"x","Kills":2,"Deaths":3}},{"id":17,"type":"list.add","params":{"variable":"Scores"}},
          {"id":18,"type":"struct.make","params":{"struct":"Score"},"values":{"Name":"y","Kills":5,"Deaths":1}},{"id":19,"type":"list.add","params":{"variable":"Scores"}},
          {"id":20,"type":"struct.make","params":{"struct":"Score"},"values":{"Name":"z","Kills":2,"Deaths":1}},{"id":21,"type":"list.add","params":{"variable":"Scores"}},
          {"id":22,"type":"struct.make","params":{"struct":"Score"},"values":{"Name":"w","Kills":2,"Deaths":3}},
          {"id":23,"type":"list.sort","params":{"variable":"Scores","function":"Before"}},{"id":24,"type":"list.sort","params":{"variable":"Times","function":""}},
          {"id":25,"type":"list.sort","params":{"variable":"Names","function":""}},{"id":26,"type":"var.set","params":{"variable":"Shown"}},{"id":27,"type":"text.append"},
          {"id":28,"type":"var.get","params":{"variable":"Scores","path":"[].Name"},"values":{"path0":0}},
          {"id":29,"type":"var.get","params":{"variable":"Scores","path":"[].Name"},"values":{"path0":3}},{"id":30,"type":"var.get","params":{"variable":"Times"}}],"links":[
          [1,"then",15,"exec"],[2,"then",3,"exec"],[4,"result",3,"before"],[5,"result",4,"a"],[6,"value",5,"a"],[7,"value",5,"b"],[8,"result",4,"b"],[9,"result",8,"a"],
          [10,"value",9,"a"],[11,"value",9,"b"],[12,"result",8,"b"],[13,"value",12,"a"],[14,"value",12,"b"],[15,"then",17,"exec"],[16,"value",15,"item"],[17,"then",19,"exec"],
          [18,"value",17,"item"],[19,"then",21,"exec"],[20,"value",19,"item"],[21,"then",23,"exec"],[22,"value",21,"item"],[23,"then",24,"exec"],[24,"then",25,"exec"],
          [25,"then",26,"exec"],[27,"result",26,"value"],[28,"value",27,"a"],[29,"value",27,"b"],[30,"value",27,"c"]],"functions":[
          {"name":"Before","pure":true,"inputs":[{"name":"a","type":"struct","of":"Score","value":{"Name":"","Kills":0,"Deaths":0}},{"name":"b","type":"struct","of":"Score","value":{"Name":"","Kills":0,"Deaths":0}}],"outputs":[{"name":"before","type":"bool","value":false}]}]})"},
      // Format Text: a format as printf has it with Int, Float and Text values - widths, precision,
      // %x %c %%, the Most input cutting what it gives (snprintf's size).
      {"text_format",
       R"({"variables":[{"name":"Shown","type":"text","value":""},{"name":"Time","type":"float","value":0}],"nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},
          {"id":2,"type":"func.entry","params":{"function":"Clock"}},{"id":3,"type":"func.return","params":{"function":"Clock"}},
          {"id":4,"type":"text.format","params":{"format":"%02d:%06.3f"},"values":{"most":-1}},{"id":5,"type":"var.set","params":{"variable":"Shown"}},
          {"id":6,"type":"text.append"},{"id":7,"type":"func.pure","params":{"function":"Clock"}},
          {"id":8,"type":"text.format","params":{"format":"|%s|%5.1f|%x|%c|100%%"},"values":{"most":-1,"v3":65}},
          {"id":9,"type":"text.format","params":{"format":"%s"},"values":{"most":3,"v0":"abcdef"}}],"links":[[1,"text",8,"v0"],[1,"then",5,"exec"],[1,"value",7,"m"],
          [1,"value",7,"s"],[1,"value",8,"v1"],[1,"value",8,"v2"],[2,"m",4,"v0"],[2,"s",4,"v1"],[2,"then",3,"exec"],[4,"result",3,"out"],[6,"result",5,"value"],[7,"out",6,"a"],
          [8,"result",6,"b"],[9,"result",6,"c"]],"functions":[
          {"name":"Clock","pure":true,"inputs":[{"name":"m","type":"int","value":0},{"name":"s","type":"float","value":0}],"outputs":[{"name":"out","type":"text","value":""}]}]})"},
      // Lists in one go: Set Length cuts and fills (a list of numbers, of booleans, of structs;
      // less than 0), Append and Insert List put a whole list in - at the start, in the middle,
      // past the end, a list into itself -, Reserve changes nothing one can read.
      {"lists_at_once",
       R"({"structs":[{"name":"Dot","fields":[{"name":"Name","type":"text","value":"dot"},{"name":"At","type":"float","value":1.5}]}],"variables":[
          {"name":"Values","type":"float_list","value":[1,2,3]},{"name":"Flags","type":"bool_list","value":[true]},{"name":"Dots","type":"struct_list","of":"Dot","value":[]},
          {"name":"Words","type":"text_list","value":["a"]},{"name":"More","type":"float_list","value":[7,8]},{"name":"Shown","type":"text","value":""},
          {"name":"Steps","type":"int","value":0}],"nodes":[{"id":1,"type":"event.begin"},{"id":10,"type":"event.custom","params":{"name":"Go"}},
          {"id":20,"type":"event.custom","params":{"name":"Open"}},{"id":21,"type":"list.reserve","params":{"variable":"Values"},"values":{"count":64}},
          {"id":22,"type":"list.resize","params":{"variable":"Values"},"values":{"length":5,"value":0.5}},
          {"id":23,"type":"list.resize","params":{"variable":"Flags"},"values":{"length":3,"value":true}},
          {"id":24,"type":"list.resize","params":{"variable":"Dots"},"values":{"length":2}},
          {"id":25,"type":"struct.make","params":{"struct":"Dot"},"values":{"Name":"made","At":2.5}},{"id":26,"type":"list.append","params":{"variable":"Words"}},
          {"id":27,"type":"list.make","params":{"type":"text","count":"2"},"values":{"item0":"b","item1":"c"}},{"id":28,"type":"list.resize","params":{"variable":"Values"}},
          {"id":29,"type":"list.append","params":{"variable":"Values"}},{"id":30,"type":"var.get","params":{"variable":"More"}},
          {"id":31,"type":"list.insert_list","params":{"variable":"Values"},"values":{"index":1}},
          {"id":32,"type":"list.make","params":{"type":"float","count":"2"},"values":{"item1":9}},{"id":33,"type":"list.append","params":{"variable":"Values"}},
          {"id":34,"type":"var.get","params":{"variable":"Values"}},{"id":35,"type":"list.insert_list","params":{"variable":"More"}},{"id":36,"type":"math.sub","values":{"b":3}},
          {"id":37,"type":"var.get","params":{"variable":"More"}},{"id":38,"type":"list.resize","params":{"variable":"Dots"}},{"id":39,"type":"math.sub","values":{"a":4}},
          {"id":40,"type":"list.get","params":{"type":"Dot"},"values":{"index":0}},{"id":41,"type":"var.get","params":{"variable":"Dots"}},
          {"id":42,"type":"list.reserve","params":{"variable":"Words"}},{"id":43,"type":"math.mul","values":{"b":-1}},{"id":44,"type":"var.set","params":{"variable":"Shown"}},
          {"id":45,"type":"text.append"},{"id":46,"type":"var.get","params":{"variable":"Shown"}},{"id":47,"type":"list.length","params":{"type":"float"}},
          {"id":48,"type":"var.get","params":{"variable":"Values"}},{"id":49,"type":"var.get","params":{"variable":"More"}},
          {"id":50,"type":"var.add","params":{"variable":"Steps"}},{"id":51,"type":"list.length","params":{"type":"Dot"}},{"id":52,"type":"var.get","params":{"variable":"Dots"}},
          {"id":53,"type":"list.resize","params":{"variable":"Values"},"values":{"length":3,"value":0}},
          {"id":54,"type":"list.resize","params":{"variable":"More"},"values":{"length":2,"value":0}},
          {"id":55,"type":"list.resize","params":{"variable":"Words"},"values":{"length":-5,"value":"x"}},{"id":56,"type":"list.append","params":{"variable":"Words"}},
          {"id":57,"type":"var.get","params":{"variable":"Words"}},{"id":58,"type":"list.resize","params":{"variable":"Flags"},"values":{"length":5,"value":false}},
          {"id":59,"type":"list.insert_list","params":{"variable":"Flags"},"values":{"index":99}},
          {"id":60,"type":"list.make","params":{"type":"bool","count":"1"},"values":{"item0":true}}],"links":[[1,"then",21,"exec"],[10,"then",28,"exec"],[10,"value",28,"length"],
          [10,"value",28,"value"],[10,"value",32,"item0"],[10,"value",36,"a"],[10,"value",39,"b"],[10,"value",43,"a"],[20,"then",53,"exec"],[21,"then",22,"exec"],
          [22,"then",23,"exec"],[23,"then",24,"exec"],[24,"then",26,"exec"],[25,"value",24,"value"],[27,"list",26,"items"],[28,"then",29,"exec"],[29,"then",31,"exec"],
          [30,"value",29,"items"],[31,"then",33,"exec"],[32,"list",31,"items"],[33,"then",35,"exec"],[34,"value",33,"items"],[35,"then",38,"exec"],[36,"result",35,"index"],
          [37,"value",35,"items"],[38,"then",42,"exec"],[39,"result",38,"length"],[40,"item",38,"value"],[41,"value",40,"list"],[42,"then",44,"exec"],[43,"result",42,"count"],
          [44,"then",50,"exec"],[45,"result",44,"value"],[46,"value",45,"a"],[47,"length",45,"b"],[48,"value",47,"list"],[49,"value",45,"c"],[51,"length",50,"amount"],
          [52,"value",51,"list"],[53,"then",54,"exec"],[54,"then",55,"exec"],[55,"then",56,"exec"],[56,"then",58,"exec"],[57,"value",56,"items"],[58,"then",59,"exec"],
          [60,"list",59,"items"]]})"},
      // Handing over: what nothing needs again is not copied. A Return hands over a local variable
      // and an input by value it names once (std::move in the Blueprint's C++), a step what the
      // step right before it gave; a Take gives what stands at a place - a variable, a field, a
      // list entry, one beside its list - and leaves the zero of its type there. Where a value is
      // read again (twice in a Return, after a Branch, by two inputs) it is copied as before.
      {"handing_over",
       R"({"structs":[
          {"name":"Path","fields":[{"name":"Name","type":"text","value":"path"},{"name":"Points","type":"float_list","value":[1,2]},{"name":"Length","type":"float","value":3}]}],"variables":[
          {"name":"Log","type":"float_list","value":[1,2]},{"name":"Taken","type":"float_list","value":[]},{"name":"Words","type":"text_list","value":["a","b"]},
          {"name":"Word","type":"text","value":"w"},{"name":"Shown","type":"text","value":""},
          {"name":"Way","type":"struct","of":"Path","value":{"Name":"way","Points":[4,5],"Length":9}},
          {"name":"Ways","type":"struct_list","of":"Path","value":[{"Name":"one","Points":[1],"Length":1},{"Name":"two","Points":[2,2],"Length":2}]},
          {"name":"Old","type":"struct","of":"Path","value":{"Name":"path","Points":[1,2],"Length":3}},{"name":"N","type":"float","value":7},
          {"name":"Was","type":"float","value":0},{"name":"Count","type":"int","value":0}],"nodes":[{"id":2,"type":"flow.for","values":{"first":1}},
          {"id":20,"type":"var.take","params":{"variable":"Log"}},{"id":24,"type":"var.take","params":{"variable":"Held","scope":"LocalTake"}},
          {"id":40,"type":"event.custom","params":{"name":"Go"}},{"id":41,"type":"func.call","params":{"function":"Make"}},
          {"id":43,"type":"func.call","params":{"function":"Make"},"values":{"n":2}},{"id":44,"type":"func.call","params":{"function":"Pass"}},
          {"id":46,"type":"func.call","params":{"function":"Both"}},{"id":48,"type":"func.call","params":{"function":"Make"},"values":{"n":1}},
          {"id":51,"type":"func.call","params":{"function":"Twice"}},{"id":53,"type":"func.call","params":{"function":"LocalTake"}},
          {"id":60,"type":"event.custom","params":{"name":"Open"}},{"id":61,"type":"func.call","params":{"function":"TakeLog"}},
          {"id":63,"type":"var.take","params":{"variable":"Word"}},{"id":65,"type":"var.take","params":{"variable":"Way","path":".Points"}},
          {"id":67,"type":"var.take","params":{"variable":"Ways","path":"[]"},"values":{"path0":1}},
          {"id":69,"type":"var.take","params":{"variable":"Words","path":"[]"},"values":{"path0":0}},
          {"id":71,"type":"var.take","params":{"variable":"Ways","path":"[].Name"},"values":{"path0":5}},{"id":73,"type":"var.take","params":{"variable":"N"}},
          {"id":74,"type":"func.entry","params":{"function":"Make"}},{"id":75,"type":"list.add","params":{"variable":"Made","scope":"Make"}},
          {"id":76,"type":"math.mul","values":{"b":1.5}},{"id":77,"type":"func.return","params":{"function":"Make"}},
          {"id":78,"type":"var.get","params":{"variable":"Made","scope":"Make"}},{"id":79,"type":"func.entry","params":{"function":"Both"}},
          {"id":80,"type":"var.set","params":{"variable":"Kept","scope":"Both"}},{"id":81,"type":"list.add","params":{"variable":"Kept","scope":"Both"},"values":{"item":0.5}},
          {"id":82,"type":"func.return","params":{"function":"Both"}},{"id":83,"type":"var.get","params":{"variable":"Kept","scope":"Both"}},
          {"id":84,"type":"list.length","params":{"type":"float"}},{"id":85,"type":"var.get","params":{"variable":"Kept","scope":"Both"}},
          {"id":86,"type":"func.entry","params":{"function":"Pass"}},{"id":87,"type":"func.return","params":{"function":"Pass"}},
          {"id":88,"type":"func.entry","params":{"function":"Twice"}},{"id":89,"type":"func.return","params":{"function":"Twice"}},
          {"id":90,"type":"text.append","values":{"b":"!","c":""}},{"id":91,"type":"func.entry","params":{"function":"TakeLog"}},
          {"id":92,"type":"func.return","params":{"function":"TakeLog"}},{"id":93,"type":"func.entry","params":{"function":"LocalTake"}},
          {"id":94,"type":"var.set","params":{"variable":"Held","scope":"LocalTake"}},{"id":95,"type":"text.append","values":{"c":""}},
          {"id":96,"type":"var.get","params":{"variable":"Held","scope":"LocalTake"}},{"id":97,"type":"func.return","params":{"function":"LocalTake"}},
          {"id":98,"type":"var.get","params":{"variable":"Held","scope":"LocalTake"}},{"id":99,"type":"var.set","params":{"variable":"Taken"}},
          {"id":100,"type":"text.append","values":{"a":"v","c":""}},{"id":101,"type":"var.set","params":{"variable":"Shown"}},{"id":102,"type":"text.append"},
          {"id":103,"type":"var.get","params":{"variable":"Shown"}},{"id":104,"type":"var.get","params":{"variable":"Taken"}},
          {"id":105,"type":"var.add","params":{"variable":"Count"}},{"id":106,"type":"flow.branch"},{"id":107,"type":"math.compare","params":{"op":">"},"values":{"b":2}},
          {"id":108,"type":"var.set","params":{"variable":"Log"}},{"id":109,"type":"list.add","params":{"variable":"Log"}},
          {"id":110,"type":"list.length","params":{"type":"float"}},{"id":111,"type":"var.get","params":{"variable":"Word"}},
          {"id":112,"type":"var.set","params":{"variable":"Shown"}},{"id":113,"type":"text.append"},{"id":114,"type":"var.get","params":{"variable":"Shown"}},
          {"id":115,"type":"var.set","params":{"variable":"Shown"}},{"id":116,"type":"text.append"},{"id":117,"type":"var.get","params":{"variable":"Shown"}},
          {"id":118,"type":"var.set","params":{"variable":"Taken"}},{"id":119,"type":"var.set","params":{"variable":"Shown"}},{"id":120,"type":"text.append","values":{"b":"<"}},
          {"id":121,"type":"var.get","params":{"variable":"Shown"}},{"id":122,"type":"list.add","params":{"variable":"Taken"}},
          {"id":123,"type":"list.length","params":{"type":"float"}},{"id":124,"type":"var.set","params":{"variable":"Old"}},
          {"id":125,"type":"var.set","params":{"variable":"Word"}},{"id":126,"type":"var.set","params":{"variable":"Shown"}},{"id":127,"type":"text.append","values":{"b":">"}},
          {"id":128,"type":"var.get","params":{"variable":"Shown"}},{"id":129,"type":"var.set","params":{"variable":"Was"}},
          {"id":130,"type":"var.take","params":{"variable":"Count"}}],"links":[[2,"body",75,"exec"],[2,"completed",77,"exec"],[2,"index",76,"a"],[20,"then",92,"exec"],
          [20,"value",92,"list"],[24,"then",97,"exec"],[24,"value",97,"text"],[40,"then",41,"exec"],[40,"value",41,"n"],[40,"value",100,"b"],[40,"value",107,"a"],
          [41,"list",99,"value"],[41,"then",99,"exec"],[43,"list",44,"given"],[43,"then",44,"exec"],[44,"list",102,"b"],[44,"name",102,"c"],[44,"then",101,"exec"],
          [46,"count",105,"amount"],[46,"then",105,"exec"],[48,"list",108,"value"],[48,"list",110,"list"],[48,"then",106,"exec"],[51,"a",113,"b"],[51,"b",53,"given"],
          [51,"b",113,"c"],[51,"then",112,"exec"],[53,"left",116,"c"],[53,"text",116,"b"],[53,"then",115,"exec"],[60,"then",61,"exec"],[61,"list",118,"value"],
          [61,"then",118,"exec"],[63,"then",119,"exec"],[63,"value",120,"c"],[65,"then",122,"exec"],[65,"value",123,"list"],[67,"then",124,"exec"],[67,"value",124,"value"],
          [69,"then",125,"exec"],[69,"value",125,"value"],[71,"then",126,"exec"],[71,"value",127,"c"],[73,"then",129,"exec"],[73,"value",129,"value"],[74,"n",2,"last"],
          [74,"then",2,"exec"],[76,"result",75,"item"],[78,"value",77,"list"],[79,"given",80,"value"],[79,"then",80,"exec"],[80,"then",81,"exec"],[81,"then",82,"exec"],
          [83,"value",82,"list"],[84,"length",82,"count"],[85,"value",84,"list"],[86,"given",87,"list"],[86,"name",87,"name"],[86,"then",87,"exec"],[88,"name",89,"a"],
          [88,"name",90,"a"],[88,"then",89,"exec"],[90,"result",89,"b"],[91,"then",20,"exec"],[93,"given",95,"b"],[93,"then",94,"exec"],[94,"then",24,"exec"],
          [95,"result",94,"value"],[96,"value",95,"a"],[98,"value",97,"left"],[99,"then",43,"exec"],[100,"result",44,"name"],[101,"then",46,"exec"],[102,"result",101,"value"],
          [103,"value",102,"a"],[104,"value",46,"given"],[105,"then",48,"exec"],[106,"false",109,"exec"],[106,"true",108,"exec"],[107,"result",106,"condition"],
          [108,"then",109,"exec"],[109,"then",51,"exec"],[110,"length",109,"item"],[111,"value",51,"name"],[112,"then",53,"exec"],[113,"result",112,"value"],[114,"value",113,"a"],
          [116,"result",115,"value"],[117,"value",116,"a"],[118,"then",63,"exec"],[119,"then",65,"exec"],[120,"result",119,"value"],[121,"value",120,"a"],[122,"then",67,"exec"],
          [123,"length",122,"item"],[124,"then",69,"exec"],[125,"then",71,"exec"],[126,"then",73,"exec"],[127,"result",126,"value"],[128,"value",127,"a"],[129,"then",130,"exec"]],"functions":[
          {"name":"Make","inputs":[{"name":"n","type":"int","value":3}],"outputs":[{"name":"list","type":"float_list","value":[]}],"locals":[{"name":"Made","type":"float_list","value":[]}]},
          {"name":"Both","inputs":[{"name":"given","type":"float_list","value":[]}],"outputs":[{"name":"list","type":"float_list","value":[]},{"name":"count","type":"int","value":0}],"locals":[{"name":"Kept","type":"float_list","value":[]}]},
          {"name":"Pass","inputs":[{"name":"given","type":"float_list","value":[]},{"name":"name","type":"text","value":"n"}],"outputs":[{"name":"list","type":"float_list","value":[]},{"name":"name","type":"text","value":""}]},
          {"name":"Twice","inputs":[{"name":"name","type":"text","value":"n"}],"outputs":[{"name":"a","type":"text","value":""},{"name":"b","type":"text","value":""}]},
          {"name":"TakeLog","inputs":[],"outputs":[{"name":"list","type":"float_list","value":[]}]},
          {"name":"LocalTake","inputs":[{"name":"given","type":"text","value":"g"}],"outputs":[{"name":"text","type":"text","value":""},{"name":"left","type":"text","value":""}],"locals":[{"name":"Held","type":"text","value":"h"}]}]})"},
      // Call on Actor: a function called on a Blueprint (here its own: the Level's) - one that
      // calls itself that way, every call with its own parameters and local variable; a function
      // of another Blueprint nobody has (the declared defaults); calls that never end (64 deep).
      {"call_on",
       R"({"nextId":75,"variables":[{"name":"Sum","type":"float","value":0},{"name":"Miss","type":"float","value":0},
        {"name":"Deep","type":"int","value":0}],"nodes":[{"id":20,"type":"func.call_on","params":{"function":"Down"},"values":{"this":0}},{"id":50,"type":"event.custom","params":{"name":"Go"}},
        {"id":51,"type":"func.call","params":{"function":"Down"}},{"id":53,"type":"func.call_on","params":{"function":"Missing"},"values":{"this":0}},
        {"id":60,"type":"event.custom","params":{"name":"Open"}},{"id":61,"type":"func.entry","params":{"function":"Down"}},{"id":62,"type":"var.set","params":{"variable":"Mine","scope":"Down"}},
        {"id":63,"type":"flow.branch"},{"id":64,"type":"math.compare","params":{"op":">"},"values":{"b":0}},{"id":65,"type":"math.sub","values":{"b":1}},
        {"id":66,"type":"func.return","params":{"function":"Down"}},{"id":67,"type":"math.add"},{"id":68,"type":"var.get","params":{"variable":"Mine","scope":"Down"}},
        {"id":69,"type":"func.entry","params":{"function":"Forever"}},{"id":70,"type":"var.add","params":{"variable":"Deep"},"values":{"amount":1}},
        {"id":71,"type":"func.call_on","params":{"function":"Forever"},"values":{"this":0}},{"id":72,"type":"var.add","params":{"variable":"Sum"}},
        {"id":73,"type":"var.add","params":{"variable":"Miss"}},
        {"id":74,"type":"func.call","params":{"function":"Forever"}}],"links":[[20,"sum",67,"b"],[20,"then",66,"exec"],[50,"then",51,"exec"],[50,"value",51,"n"],[50,"value",53,"x"],[51,"sum",72,"amount"],[51,"then",72,"exec"],[53,"then",73,"exec"],[53,"value",73,"amount"],[60,"then",74,"exec"],[61,"n",62,"value"],[61,"n",64,"a"],[61,"n",65,"a"],[61,"then",62,"exec"],[62,"then",63,"exec"],[63,"false",66,"exec"],[63,"true",20,"exec"],[64,"result",63,"condition"],[65,"result",20,"n"],[67,"result",66,"sum"],[68,"value",67,"a"],[69,"then",70,"exec"],[70,"then",71,"exec"],[72,"then",53,"exec"]],"functions":[{"name":"Missing","external":true,"inputs":[{"name":"x","type":"float","value":1}],"outputs":[{"name":"value","type":"float","value":2.5}]},
        {"name":"Down","inputs":[{"name":"n","type":"int","value":0}],"outputs":[{"name":"sum","type":"float","value":0}],"locals":[{"name":"Mine","type":"int","value":0}]},
        {"name":"Forever","inputs":[],"outputs":[]}]})"},
      // Int64: 64 bits as u64 and i64 of C++ - every Int64 node, on values beyond 2^53 (a Float
      // times 10^17, halves of 32 bits), dividing by 0 and the smallest i64 by -1, shifts by more
      // than 63 (the amount's last 6 bits), Floats beyond the ends and no number, texts that are
      // no number; literals on pins and as variables' defaults.
      {"int64",
       R"({"variables":[{"name":"R","type":"int64_list","value":[]},{"name":"Bs","type":"bool_list","value":[]},{"name":"Is","type":"int_list","value":[])"
       R"(},{"name":"Fs","type":"float_list","value":[]},{"name":"Ts","type":"text_list","value":[]},{"name":"Min","type":"int64","value":"9223372036854775808")"
       R"(},{"name":"MinusOne","type":"int64","value":"18446744073709551615"},{"name":"Zero","type":"int64","value":"0"}],"nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"})"
       R"(},{"id":2,"type":"math.mul","values":{"b":1e+17}},{"id":3,"type":"int64.of_float"},{"id":4,"type":"math.round"},{"id":5,"type":"math.mul","values":{"b":1000})"
       R"(},{"id":6,"type":"int64.of_halves","values":{"low":4294967295}},{"id":7,"type":"var.get","params":{"variable":"Min"}},{"id":8,"type":"var.get","params":{"variable":"MinusOne"})"
       R"(},{"id":9,"type":"var.get","params":{"variable":"Zero"}},{"id":10,"type":"int64.add"},{"id":11,"type":"list.add","params":{"variable":"R"}},{"id":12,"type":"int64.add")"
       R"(},{"id":13,"type":"list.add","params":{"variable":"R"}},{"id":14,"type":"int64.add"},{"id":15,"type":"list.add","params":{"variable":"R"}},{"id":16,"type":"int64.add")"
       R"(},{"id":17,"type":"list.add","params":{"variable":"R"}},{"id":18,"type":"int64.sub"},{"id":19,"type":"list.add","params":{"variable":"R"}},{"id":20,"type":"int64.sub")"
       R"(},{"id":21,"type":"list.add","params":{"variable":"R"}},{"id":22,"type":"int64.sub"},{"id":23,"type":"list.add","params":{"variable":"R"}},{"id":24,"type":"int64.sub")"
       R"(},{"id":25,"type":"list.add","params":{"variable":"R"}},{"id":26,"type":"int64.mul"},{"id":27,"type":"list.add","params":{"variable":"R"}},{"id":28,"type":"int64.mul")"
       R"(},{"id":29,"type":"list.add","params":{"variable":"R"}},{"id":30,"type":"int64.mul"},{"id":31,"type":"list.add","params":{"variable":"R"}},{"id":32,"type":"int64.mul")"
       R"(},{"id":33,"type":"list.add","params":{"variable":"R"}},{"id":34,"type":"int64.div"},{"id":35,"type":"list.add","params":{"variable":"R"}},{"id":36,"type":"int64.div")"
       R"(},{"id":37,"type":"list.add","params":{"variable":"R"}},{"id":38,"type":"int64.div"},{"id":39,"type":"list.add","params":{"variable":"R"}},{"id":40,"type":"int64.div")"
       R"(},{"id":41,"type":"list.add","params":{"variable":"R"}},{"id":42,"type":"int64.div_unsigned"},{"id":43,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":44,"type":"int64.div_unsigned"},{"id":45,"type":"list.add","params":{"variable":"R"}},{"id":46,"type":"int64.div_unsigned")"
       R"(},{"id":47,"type":"list.add","params":{"variable":"R"}},{"id":48,"type":"int64.div_unsigned"},{"id":49,"type":"list.add","params":{"variable":"R"}},{"id":50,"type":"int64.rem")"
       R"(},{"id":51,"type":"list.add","params":{"variable":"R"}},{"id":52,"type":"int64.rem"},{"id":53,"type":"list.add","params":{"variable":"R"}},{"id":54,"type":"int64.rem")"
       R"(},{"id":55,"type":"list.add","params":{"variable":"R"}},{"id":56,"type":"int64.rem"},{"id":57,"type":"list.add","params":{"variable":"R"}},{"id":58,"type":"int64.rem_unsigned")"
       R"(},{"id":59,"type":"list.add","params":{"variable":"R"}},{"id":60,"type":"int64.rem_unsigned"},{"id":61,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":62,"type":"int64.rem_unsigned"},{"id":63,"type":"list.add","params":{"variable":"R"}},{"id":64,"type":"int64.rem_unsigned")"
       R"(},{"id":65,"type":"list.add","params":{"variable":"R"}},{"id":66,"type":"int64.and"},{"id":67,"type":"list.add","params":{"variable":"R"}},{"id":68,"type":"int64.and")"
       R"(},{"id":69,"type":"list.add","params":{"variable":"R"}},{"id":70,"type":"int64.and"},{"id":71,"type":"list.add","params":{"variable":"R"}},{"id":72,"type":"int64.and")"
       R"(},{"id":73,"type":"list.add","params":{"variable":"R"}},{"id":74,"type":"int64.or"},{"id":75,"type":"list.add","params":{"variable":"R"}},{"id":76,"type":"int64.or")"
       R"(},{"id":77,"type":"list.add","params":{"variable":"R"}},{"id":78,"type":"int64.or"},{"id":79,"type":"list.add","params":{"variable":"R"}},{"id":80,"type":"int64.or")"
       R"(},{"id":81,"type":"list.add","params":{"variable":"R"}},{"id":82,"type":"int64.xor"},{"id":83,"type":"list.add","params":{"variable":"R"}},{"id":84,"type":"int64.xor")"
       R"(},{"id":85,"type":"list.add","params":{"variable":"R"}},{"id":86,"type":"int64.xor"},{"id":87,"type":"list.add","params":{"variable":"R"}},{"id":88,"type":"int64.xor")"
       R"(},{"id":89,"type":"list.add","params":{"variable":"R"}},{"id":90,"type":"math.add","values":{"b":61}},{"id":91,"type":"int64.shift_left")"
       R"(},{"id":92,"type":"list.add","params":{"variable":"R"}},{"id":93,"type":"int64.shift_left"},{"id":94,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":95,"type":"int64.shift_right"},{"id":96,"type":"list.add","params":{"variable":"R"}},{"id":97,"type":"int64.shift_right")"
       R"(},{"id":98,"type":"list.add","params":{"variable":"R"}},{"id":99,"type":"int64.shift_right_unsigned"},{"id":100,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":101,"type":"int64.shift_right_unsigned"},{"id":102,"type":"list.add","params":{"variable":"R"}},{"id":103,"type":"int64.rotate_left")"
       R"(},{"id":104,"type":"list.add","params":{"variable":"R"}},{"id":105,"type":"int64.rotate_left"},{"id":106,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":107,"type":"int64.rotate_right"},{"id":108,"type":"list.add","params":{"variable":"R"}},{"id":109,"type":"int64.rotate_right")"
       R"(},{"id":110,"type":"list.add","params":{"variable":"R"}},{"id":111,"type":"int64.not"},{"id":112,"type":"list.add","params":{"variable":"R"}},{"id":113,"type":"int64.equal")"
       R"(},{"id":114,"type":"list.add","params":{"variable":"Bs"}},{"id":115,"type":"int64.equal"},{"id":116,"type":"list.add","params":{"variable":"Bs"}},{"id":117,"type":"int64.less")"
       R"(},{"id":118,"type":"list.add","params":{"variable":"Bs"}},{"id":119,"type":"int64.less_unsigned"},{"id":120,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":121,"type":"int64.less"},{"id":122,"type":"list.add","params":{"variable":"Bs"}},{"id":123,"type":"int64.less_unsigned")"
       R"(},{"id":124,"type":"list.add","params":{"variable":"Bs"}},{"id":125,"type":"math.sub","values":{"b":3}},{"id":126,"type":"int64.of_int")"
       R"(},{"id":127,"type":"list.add","params":{"variable":"R"}},{"id":128,"type":"int64.to_int"},{"id":129,"type":"list.add","params":{"variable":"Is"}},{"id":130,"type":"int64.to_int")"
       R"(},{"id":131,"type":"list.add","params":{"variable":"Is"}},{"id":132,"type":"int64.to_int_unsigned"},{"id":133,"type":"list.add","params":{"variable":"Is"})"
       R"(},{"id":134,"type":"int64.to_int_unsigned"},{"id":135,"type":"list.add","params":{"variable":"Is"}},{"id":136,"type":"int64.high")"
       R"(},{"id":137,"type":"list.add","params":{"variable":"Is"}},{"id":138,"type":"int64.low"},{"id":139,"type":"list.add","params":{"variable":"Is"}},{"id":140,"type":"int64.high")"
       R"(},{"id":141,"type":"list.add","params":{"variable":"Is"}},{"id":142,"type":"int64.count_ones"},{"id":143,"type":"list.add","params":{"variable":"Is"})"
       R"(},{"id":144,"type":"int64.count_ones"},{"id":145,"type":"list.add","params":{"variable":"Is"}},{"id":146,"type":"int64.leading_zeros")"
       R"(},{"id":147,"type":"list.add","params":{"variable":"Is"}},{"id":148,"type":"int64.leading_zeros"},{"id":149,"type":"list.add","params":{"variable":"Is"})"
       R"(},{"id":150,"type":"int64.trailing_zeros"},{"id":151,"type":"list.add","params":{"variable":"Is"}},{"id":152,"type":"int64.trailing_zeros")"
       R"(},{"id":153,"type":"list.add","params":{"variable":"Is"}},{"id":154,"type":"int64.to_float"},{"id":155,"type":"list.add","params":{"variable":"Fs"})"
       R"(},{"id":156,"type":"int64.to_float_unsigned"},{"id":157,"type":"list.add","params":{"variable":"Fs"}},{"id":158,"type":"int64.to_float")"
       R"(},{"id":159,"type":"list.add","params":{"variable":"Fs"}},{"id":160,"type":"int64.float_of_bits"},{"id":161,"type":"list.add","params":{"variable":"Fs"})"
       R"(},{"id":162,"type":"int64.float_of_bits"},{"id":163,"type":"list.add","params":{"variable":"Fs"}},{"id":164,"type":"int64.of_float","values":{"a":-1e+30})"
       R"(},{"id":165,"type":"list.add","params":{"variable":"R"}},{"id":166,"type":"int64.of_float","values":{"a":1e+30}},{"id":167,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":168,"type":"int64.of_float_unsigned","values":{"a":-5.5}},{"id":169,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":170,"type":"int64.of_float_unsigned","values":{"a":1e+30}},{"id":171,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":172,"type":"int64.of_float_unsigned","values":{"a":3.75}},{"id":173,"type":"list.add","params":{"variable":"R"}},{"id":174,"type":"int64.bits_of_float","values":{"a":0.1})"
       R"(},{"id":175,"type":"list.add","params":{"variable":"R"}},{"id":176,"type":"math.sqrt","values":{"a":-1}},{"id":177,"type":"int64.of_float")"
       R"(},{"id":178,"type":"list.add","params":{"variable":"R"}},{"id":179,"type":"int64.text"},{"id":180,"type":"list.add","params":{"variable":"Ts"}},{"id":181,"type":"int64.text")"
       R"(},{"id":182,"type":"list.add","params":{"variable":"Ts"}},{"id":183,"type":"int64.text_signed"},{"id":184,"type":"list.add","params":{"variable":"Ts"})"
       R"(},{"id":185,"type":"int64.text_signed"},{"id":186,"type":"list.add","params":{"variable":"Ts"}},{"id":187,"type":"int64.text_hex")"
       R"(},{"id":188,"type":"list.add","params":{"variable":"Ts"}},{"id":189,"type":"int64.text_hex"},{"id":190,"type":"list.add","params":{"variable":"Ts"})"
       R"(},{"id":191,"type":"int64.text_signed"},{"id":192,"type":"int64.of_text"},{"id":193,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":194,"type":"int64.of_text","values":{"a":"0x1F"}},{"id":195,"type":"list.add","params":{"variable":"R"}},{"id":196,"type":"int64.of_text","values":{"a":"-5"})"
       R"(},{"id":197,"type":"list.add","params":{"variable":"R"}},{"id":198,"type":"int64.of_text","values":{"a":"18446744073709551616"})"
       R"(},{"id":199,"type":"list.add","params":{"variable":"R"}},{"id":200,"type":"int64.of_text","values":{"a":"zz"}},{"id":201,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":202,"type":"int64.of_text","values":{"a":""}},{"id":203,"type":"list.add","params":{"variable":"R"}},{"id":204,"type":"int64.of_text","values":{"a":"-0x8000000000000000"})"
       R"(},{"id":205,"type":"list.add","params":{"variable":"R"}},{"id":206,"type":"int64.add","values":{"a":"18446744073709551615","b":"2"})"
       R"(},{"id":207,"type":"list.add","params":{"variable":"R"})"
       R"(},{"id":208,"type":"var.set","params":{"variable":"Zero"}}],"links":[[1,"value",2,"a"],[2,"result",3,"a"],[1,"value",4,"a"],[4,"result",5,"a"],[5,"result",6,"high"],[3,"result",10,"a"],[6,"result",10,"b"],[1,"then",11,"exec"],[10,"result",11,"item"],[3,"result",12,"a"],[9,"value",12,"b"],[11,"then",13,"exec"],[12,"result",13,"item"],[7,"value",14,"a"],[8,"value",14,"b"],[13,"then",15,"exec"],[14,"result",15,"item"],[6,"result",16,"a"],[8,"value",16,"b"],[15,"then",17,"exec"],[16,"result",17,"item"],[3,"result",18,"a"],[6,"result",18,"b"],[17,"then",19,"exec"],[18,"result",19,"item"],[3,"result",20,"a"],[9,"value",20,"b"],[19,"then",21,"exec"],[20,"result",21,"item"],[7,"value",22,"a"],[8,"value",22,"b"],[21,"then",23,"exec"],[22,"result",23,"item"],[6,"result",24,"a"],[8,"value",24,"b"],[23,"then",25,"exec"],[24,"result",25,"item"],[3,"result",26,"a"],[6,"result",26,"b"],[25,"then",27,"exec"],[26,"result",27,"item"],[3,"result",28,"a"],[9,"value",28,"b"],[27,"then",29,"exec"],[28,"result",29,"item"],[7,"value",30,"a"],[8,"value",30,"b"],[29,"then",31,"exec"],[30,"result",31,"item"],[6,"result",32,"a"],[8,"value",32,"b"],[31,"then",33,"exec"],[32,"result",33,"item"],[3,"result",34,"a"],[6,"result",34,"b"],[33,"then",35,"exec"],[34,"result",35,"item"],[3,"result",36,"a"],[9,"value",36,"b"],[35,"then",37,"exec"],[36,"result",37,"item"],[7,"value",38,"a"],[8,"value",38,"b"],[37,"then",39,"exec"],[38,"result",39,"item"],[6,"result",40,"a"],[8,"value",40,"b"],[39,"then",41,"exec"],[40,"result",41,"item"],[3,"result",42,"a"],[6,"result",42,"b"],[41,"then",43,"exec"],[42,"result",43,"item"],[3,"result",44,"a"],[9,"value",44,"b"],[43,"then",45,"exec"],[44,"result",45,"item"],[7,"value",46,"a"],[8,"value",46,"b"],[45,"then",47,"exec"],[46,"result",47,"item"],[6,"result",48,"a"],[8,"value",48,"b"],[47,"then",49,"exec"],[48,"result",49,"item"],[3,"result",50,"a"],[6,"result",50,"b"],[49,"then",51,"exec"],[50,"result",51,"item"],[3,"result",52,"a"],[9,"value",52,"b"],[51,"then",53,"exec"],[52,"result",53,"item"],[7,"value",54,"a"],[8,"value",54,"b"],[53,"then",55,"exec"],[54,"result",55,"item"],[6,"result",56,"a"],[8,"value",56,"b"],[55,"then",57,"exec"],[56,"result",57,"item"],[3,"result",58,"a"],[6,"result",58,"b"],[57,"then",59,"exec"],[58,"result",59,"item"],[3,"result",60,"a"],[9,"value",60,"b"],[59,"then",61,"exec"],[60,"result",61,"item"],[7,"value",62,"a"],[8,"value",62,"b"],[61,"then",63,"exec"],[62,"result",63,"item"],[6,"result",64,"a"],[8,"value",64,"b"],[63,"then",65,"exec"],[64,"result",65,"item"],[3,"result",66,"a"],[6,"result",66,"b"],[65,"then",67,"exec"],[66,"result",67,"item"],[3,"result",68,"a"],[9,"value",68,"b"],[67,"then",69,"exec"],[68,"result",69,"item"],[7,"value",70,"a"],[8,"value",70,"b"],[69,"then",71,"exec"],[70,"result",71,"item"],[6,"result",72,"a"],[8,"value",72,"b"],[71,"then",73,"exec"],[72,"result",73,"item"],[3,"result",74,"a"],[6,"result",74,"b"],[73,"then",75,"exec"],[74,"result",75,"item"],[3,"result",76,"a"],[9,"value",76,"b"],[75,"then",77,"exec"],[76,"result",77,"item"],[7,"value",78,"a"],[8,"value",78,"b"],[77,"then",79,"exec"],[78,"result",79,"item"],[6,"result",80,"a"],[8,"value",80,"b"],[79,"then",81,"exec"],[80,"result",81,"item"],[3,"result",82,"a"],[6,"result",82,"b"],[81,"then",83,"exec"],[82,"result",83,"item"],[3,"result",84,"a"],[9,"value",84,"b"],[83,"then",85,"exec"],[84,"result",85,"item"],[7,"value",86,"a"],[8,"value",86,"b"],[85,"then",87,"exec"],[86,"result",87,"item"],[6,"result",88,"a"],[8,"value",88,"b"],[87,"then",89,"exec"],[88,"result",89,"item"],[4,"result",90,"a"],[6,"result",91,"a"],[90,"result",91,"by"],[89,"then",92,"exec"],[91,"result",92,"item"],[7,"value",93,"a"],[90,"result",93,"by"],[92,"then",94,"exec"],[93,"result",94,"item"],[6,"result",95,"a"],[90,"result",95,"by"],[94,"then",96,"exec"],[95,"result",96,"item"],[7,"value",97,"a"],[90,"result",97,"by"],[96,"then",98,"exec"],[97,"result",98,"item"],[6,"result",99,"a"],[90,"result",99,"by"],[98,"then",100,"exec"],[99,"result",100,"item"],[7,"value",101,"a"],[90,"result",101,"by"],[100,"then",102,"exec"],[101,"result",102,"item"],[6,"result",103,"a"],[90,"result",103,"by"],[102,"then",104,"exec"],[103,"result",104,"item"],[7,"value",105,"a"],[90,"result",105,"by"],[104,"then",106,"exec"],[105,"result",106,"item"],[6,"result",107,"a"],[90,"result",107,"by"],[106,"then",108,"exec"],[107,"result",108,"item"],[7,"value",109,"a"],[90,"result",109,"by"],[108,"then",110,"exec"],[109,"result",110,"item"],[3,"result",111,"a"],[110,"then",112,"exec"],[111,"result",112,"item"],[3,"result",113,"a"],[6,"result",113,"b"],[112,"then",114,"exec"],[113,"result",114,"item"],[9,"value",115,"a"],[9,"value",115,"b"],[114,"then",116,"exec"],[115,"result",116,"item"],[3,"result",117,"a"],[8,"value",117,"b"],[116,"then",118,"exec"],[117,"result",118,"item"],[3,"result",119,"a"],[8,"value",119,"b"],[118,"then",120,"exec"],[119,"result",120,"item"],[7,"value",121,"a"],[9,"value",121,"b"],[120,"then",122,"exec"],[121,"result",122,"item"],[7,"value",123,"a"],[9,"value",123,"b"],[122,"then",124,"exec"],[123,"result",124,"item"],[4,"result",125,"a"],[125,"result",126,"a"],[124,"then",127,"exec"],[126,"result",127,"item"],[3,"result",128,"a"],[127,"then",129,"exec"],[128,"result",129,"item"],[8,"value",130,"a"],[129,"then",131,"exec"],[130,"result",131,"item"],[8,"value",132,"a"],[131,"then",133,"exec"],[132,"result",133,"item"],[3,"result",134,"a"],[133,"then",135,"exec"],[134,"result",135,"item"],[3,"result",136,"a"],[135,"then",137,"exec"],[136,"result",137,"item"],[3,"result",138,"a"],[137,"then",139,"exec"],[138,"result",139,"item"],[8,"value",140,"a"],[139,"then",141,"exec"],[140,"result",141,"item"],[3,"result",142,"a"],[141,"then",143,"exec"],[142,"result",143,"item"],[8,"value",144,"a"],[143,"then",145,"exec"],[144,"result",145,"item"],[9,"value",146,"a"],[145,"then",147,"exec"],[146,"result",147,"item"],[6,"result",148,"a"],[147,"then",149,"exec"],[148,"result",149,"item"],[9,"value",150,"a"],[149,"then",151,"exec"],[150,"result",151,"item"],[7,"value",152,"a"],[151,"then",153,"exec"],[152,"result",153,"item"],[8,"value",154,"a"],[153,"then",155,"exec"],[154,"result",155,"item"],[8,"value",156,"a"],[155,"then",157,"exec"],[156,"result",157,"item"],[3,"result",158,"a"],[157,"then",159,"exec"],[158,"result",159,"item"],[6,"result",160,"a"],[159,"then",161,"exec"],[160,"result",161,"item"],[8,"value",162,"a"],[161,"then",163,"exec"],[162,"result",163,"item"],[163,"then",165,"exec"],[164,"result",165,"item"],[165,"then",167,"exec"],[166,"result",167,"item"],[167,"then",169,"exec"],[168,"result",169,"item"],[169,"then",171,"exec"],[170,"result",171,"item"],[171,"then",173,"exec"],[172,"result",173,"item"],[173,"then",175,"exec"],[174,"result",175,"item"],[176,"result",177,"a"],[175,"then",178,"exec"],[177,"result",178,"item"],[3,"result",179,"a"],[178,"then",180,"exec"],[179,"result",180,"item"],[8,"value",181,"a"],[180,"then",182,"exec"],[181,"result",182,"item"],[8,"value",183,"a"],[182,"then",184,"exec"],[183,"result",184,"item"],[3,"result",185,"a"],[184,"then",186,"exec"],[185,"result",186,"item"],[7,"value",187,"a"],[186,"then",188,"exec"],[187,"result",188,"item"],[6,"result",189,"a"],[188,"then",190,"exec"],[189,"result",190,"item"],[3,"result",191,"a"],[191,"result",192,"a"],[190,"then",193,"exec"],[192,"result",193,"item"],[193,"then",195,"exec"],[194,"result",195,"item"],[195,"then",197,"exec"],[196,"result",197,"item"],[197,"then",199,"exec"],[198,"result",199,"item"],[199,"then",201,"exec"],[200,"result",201,"item"],[201,"then",203,"exec"],[202,"result",203,"item"],[203,"then",205,"exec"],[204,"result",205,"item"],[205,"then",207,"exec"],[206,"result",207,"item"],[207,"then",208,"exec"],[206,"result",208,"value"]]})"},
      // Maps and sets: in the order their keys came. Int, Text, enum, Int64 and Bool keys; values that
      // are Floats, Texts, lists and structs; every node of both; a value of a map as a place
      // (Cells[i].Count += 1, a list in it, one beside the map); taking out from the middle of the
      // order (Open) and putting a key in again (it goes to the end); dividing by 0 into a value.
      {"maps",
       R"({"enums":[{"name":"Kind","values":["A","B","C"]}],"structs":[{"name":"Cell","fields":[{"name":"Count","type":"int","value":0)"
       R"(},{"name":"Tags","type":"text_list","value":[]}]}],"variables":[{"name":"Scores","type":"map","of":"int>float","value":[[3,1.5],[1,2]])"
       R"(},{"name":"Cells","type":"map","of":"text>struct:Cell","value":[["x",{"Count":1,"Tags":["p"]}]]},{"name":"Lists","type":"map","of":"enum:Kind>float_list","value":[])"
       R"(},{"name":"Big","type":"map","of":"int64>text","value":[["18446744073709551615","max"]]},{"name":"Seen","type":"set","of":"int64","value":["5"])"
       R"(},{"name":"Words","type":"set","of":"text","value":["a","b"]},{"name":"Flags","type":"set","of":"bool","value":[]},{"name":"Out","type":"float_list","value":[])"
       R"(},{"name":"Ins","type":"int_list","value":[]},{"name":"Bs","type":"bool_list","value":[])"
       R"(},{"name":"Ks","type":"text_list","value":[]}],"nodes":[{"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"math.round")"
       R"(},{"id":3,"type":"math.mul","values":{"b":1.5}},{"id":4,"type":"map.set","params":{"variable":"Scores"}},{"id":5,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":6,"type":"list.add","params":{"variable":"Ins"}},{"id":7,"type":"math.div","values":{"b":0}},{"id":8,"type":"map.set","params":{"variable":"Scores"},"values":{"key":99})"
       R"(},{"id":9,"type":"map.add","params":{"variable":"Cells"}},{"id":10,"type":"list.add","params":{"variable":"Ins"}},{"id":11,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":12,"type":"var.add","params":{"variable":"Cells","path":"[].Count"},"values":{"amount":1})"
       R"(},{"id":13,"type":"list.add","params":{"variable":"Cells","path":"[].Tags"},"values":{"item":"t"}},{"id":14,"type":"math.mod","values":{"b":3}},{"id":15,"type":"math.round")"
       R"(},{"id":16,"type":"enum.from_index","params":{"enum":"Kind"}},{"id":17,"type":"list.make","params":{"type":"float","count":"2"})"
       R"(},{"id":18,"type":"map.set","params":{"variable":"Lists"}},{"id":19,"type":"math.add","values":{"b":1000000}},{"id":20,"type":"int64.of_halves","values":{"low":7})"
       R"(},{"id":21,"type":"map.set","params":{"variable":"Big"},"values":{"value":"v"}},{"id":22,"type":"math.mul","values":{"b":1e+17}},{"id":23,"type":"int64.of_float")"
       R"(},{"id":24,"type":"set.add","params":{"variable":"Seen"}},{"id":25,"type":"list.add","params":{"variable":"Bs"}},{"id":26,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":27,"type":"set.add","params":{"variable":"Words"}},{"id":28,"type":"math.compare","params":{"op":">"},"values":{"b":2})"
       R"(},{"id":29,"type":"set.add","params":{"variable":"Flags"}},{"id":30,"type":"map.reserve","params":{"variable":"Scores"},"values":{"count":8})"
       R"(},{"id":31,"type":"set.reserve","params":{"variable":"Words"},"values":{"count":4}},{"id":32,"type":"map.length","params":{"key":"int","value":"float"})"
       R"(},{"id":33,"type":"var.get","params":{"variable":"Scores"}},{"id":34,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":35,"type":"map.contains","params":{"key":"int","value":"float"},"values":{"key":4}},{"id":36,"type":"var.get","params":{"variable":"Scores"})"
       R"(},{"id":37,"type":"list.add","params":{"variable":"Bs"}},{"id":38,"type":"map.find","params":{"key":"int","value":"float"}},{"id":39,"type":"var.get","params":{"variable":"Scores"})"
       R"(},{"id":40,"type":"list.add","params":{"variable":"Bs"}},{"id":41,"type":"list.add","params":{"variable":"Out"}},{"id":42,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":43,"type":"map.find","params":{"key":"int","value":"float"},"values":{"key":1234}},{"id":44,"type":"var.get","params":{"variable":"Scores"})"
       R"(},{"id":45,"type":"list.add","params":{"variable":"Bs"}},{"id":46,"type":"list.add","params":{"variable":"Out"}},{"id":47,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":48,"type":"map.key_at","params":{"key":"text","value":"Cell"},"values":{"index":0}},{"id":49,"type":"var.get","params":{"variable":"Cells"})"
       R"(},{"id":50,"type":"list.add","params":{"variable":"Ks"}},{"id":51,"type":"map.key_at","params":{"key":"text","value":"Cell"},"values":{"index":99})"
       R"(},{"id":52,"type":"var.get","params":{"variable":"Cells"}},{"id":53,"type":"list.add","params":{"variable":"Ks"})"
       R"(},{"id":54,"type":"map.find","params":{"key":"text","value":"Cell"},"values":{"key":"2"}},{"id":55,"type":"var.get","params":{"variable":"Cells"})"
       R"(},{"id":56,"type":"list.add","params":{"variable":"Bs"}},{"id":57,"type":"struct.break","params":{"struct":"Cell"}},{"id":58,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":59,"type":"list.length","params":{"type":"text"}},{"id":60,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":61,"type":"map.find","params":{"key":"Kind","value":"float_list"}},{"id":62,"type":"var.get","params":{"variable":"Lists"})"
       R"(},{"id":63,"type":"list.length","params":{"type":"float"}},{"id":64,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":65,"type":"map.key_at","params":{"key":"int64","value":"text"},"values":{"index":1}},{"id":66,"type":"var.get","params":{"variable":"Big"}},{"id":67,"type":"int64.text")"
       R"(},{"id":68,"type":"list.add","params":{"variable":"Ks"}},{"id":69,"type":"set.length","params":{"key":"int64"}},{"id":70,"type":"var.get","params":{"variable":"Seen"})"
       R"(},{"id":71,"type":"list.add","params":{"variable":"Ins"}},{"id":72,"type":"set.contains","params":{"key":"text"},"values":{"key":"a"})"
       R"(},{"id":73,"type":"var.get","params":{"variable":"Words"}},{"id":74,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":75,"type":"set.contains","params":{"key":"bool"},"values":{"key":true}},{"id":76,"type":"var.get","params":{"variable":"Flags"})"
       R"(},{"id":77,"type":"list.add","params":{"variable":"Bs"}},{"id":78,"type":"set.key_at","params":{"key":"int64"},"values":{"index":0})"
       R"(},{"id":79,"type":"var.get","params":{"variable":"Seen"}},{"id":80,"type":"int64.text"},{"id":81,"type":"list.add","params":{"variable":"Ks"})"
       R"(},{"id":82,"type":"set.find","params":{"key":"text"},"values":{"key":"4"}},{"id":83,"type":"var.get","params":{"variable":"Words"})"
       R"(},{"id":84,"type":"list.add","params":{"variable":"Ins"}},{"id":85,"type":"var.get","params":{"variable":"Cells","path":"[].Tags"},"values":{"path0":0})"
       R"(},{"id":86,"type":"list.length","params":{"type":"text"}},{"id":87,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":88,"type":"var.get","params":{"variable":"Cells","path":"[].Count"},"values":{"path0":50}},{"id":89,"type":"list.add","params":{"variable":"Ins"})"
       R"(},{"id":90,"type":"event.custom","params":{"name":"Open"}},{"id":91,"type":"map.remove","params":{"variable":"Scores"},"values":{"key":2})"
       R"(},{"id":92,"type":"list.add","params":{"variable":"Bs"}},{"id":93,"type":"map.remove","params":{"variable":"Scores"},"values":{"key":77})"
       R"(},{"id":94,"type":"list.add","params":{"variable":"Bs"}},{"id":95,"type":"map.remove_at","params":{"variable":"Cells"},"values":{"index":1})"
       R"(},{"id":96,"type":"map.remove_at","params":{"variable":"Cells"},"values":{"index":-1})"
       R"(},{"id":97,"type":"map.remove","params":{"variable":"Big"},"values":{"key":"18446744073709551615"}},{"id":98,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":99,"type":"set.remove","params":{"variable":"Words"},"values":{"key":"a"}},{"id":100,"type":"list.add","params":{"variable":"Bs"})"
       R"(},{"id":101,"type":"set.remove_at","params":{"variable":"Seen"},"values":{"index":0}},{"id":102,"type":"set.remove_at","params":{"variable":"Seen"},"values":{"index":40})"
       R"(},{"id":103,"type":"map.clear","params":{"variable":"Lists"}},{"id":104,"type":"set.clear","params":{"variable":"Flags"})"
       R"(},{"id":105,"type":"map.set","params":{"variable":"Cells"},"values":{"key":"new"})"
       R"(},{"id":106,"type":"struct.make","params":{"struct":"Cell"},"values":{"Count":7}}],"links":[[1,"value",2,"a"],[1,"value",3,"a"],[1,"then",4,"exec"],[2,"result",4,"key"],[3,"result",4,"value"],[4,"then",5,"exec"],[4,"added",5,"item"],[5,"then",6,"exec"],[4,"index",6,"item"],[1,"value",7,"a"],[6,"then",8,"exec"],[7,"result",8,"value"],[8,"then",9,"exec"],[2,"result",9,"key"],[9,"then",10,"exec"],[9,"index",10,"item"],[10,"then",11,"exec"],[9,"added",11,"item"],[11,"then",12,"exec"],[9,"index",12,"path0"],[12,"then",13,"exec"],[9,"index",13,"path0"],[2,"result",14,"a"],[14,"result",15,"a"],[15,"result",16,"index"],[1,"value",17,"item0"],[3,"result",17,"item1"],[13,"then",18,"exec"],[16,"value",18,"key"],[17,"list",18,"value"],[2,"result",19,"a"],[19,"result",20,"high"],[18,"then",21,"exec"],[20,"result",21,"key"],[1,"value",22,"a"],[22,"result",23,"a"],[21,"then",24,"exec"],[23,"result",24,"key"],[24,"then",25,"exec"],[24,"added",25,"item"],[25,"then",26,"exec"],[24,"index",26,"item"],[26,"then",27,"exec"],[2,"result",27,"key"],[1,"value",28,"a"],[27,"then",29,"exec"],[28,"result",29,"key"],[29,"then",30,"exec"],[30,"then",31,"exec"],[33,"value",32,"map"],[31,"then",34,"exec"],[32,"length",34,"item"],[36,"value",35,"map"],[34,"then",37,"exec"],[35,"result",37,"item"],[39,"value",38,"map"],[2,"result",38,"key"],[37,"then",40,"exec"],[38,"found",40,"item"],[40,"then",41,"exec"],[38,"value",41,"item"],[41,"then",42,"exec"],[38,"index",42,"item"],[44,"value",43,"map"],[42,"then",45,"exec"],[43,"found",45,"item"],[45,"then",46,"exec"],[43,"value",46,"item"],[46,"then",47,"exec"],[43,"index",47,"item"],[49,"value",48,"map"],[47,"then",50,"exec"],[48,"key",50,"item"],[52,"value",51,"map"],[50,"then",53,"exec"],[51,"key",53,"item"],[55,"value",54,"map"],[53,"then",56,"exec"],[54,"found",56,"item"],[54,"value",57,"value"],[56,"then",58,"exec"],[57,"Count",58,"item"],[57,"Tags",59,"list"],[58,"then",60,"exec"],[59,"length",60,"item"],[62,"value",61,"map"],[16,"value",61,"key"],[61,"value",63,"list"],[60,"then",64,"exec"],[63,"length",64,"item"],[66,"value",65,"map"],[65,"key",67,"a"],[64,"then",68,"exec"],[67,"result",68,"item"],[70,"value",69,"set"],[68,"then",71,"exec"],[69,"length",71,"item"],[73,"value",72,"set"],[71,"then",74,"exec"],[72,"result",74,"item"],[76,"value",75,"set"],[74,"then",77,"exec"],[75,"result",77,"item"],[79,"value",78,"set"],[78,"key",80,"a"],[77,"then",81,"exec"],[80,"result",81,"item"],[83,"value",82,"set"],[81,"then",84,"exec"],[82,"index",84,"item"],[85,"value",86,"list"],[84,"then",87,"exec"],[86,"length",87,"item"],[87,"then",89,"exec"],[88,"value",89,"item"],[90,"then",91,"exec"],[91,"then",92,"exec"],[91,"removed",92,"item"],[92,"then",93,"exec"],[93,"then",94,"exec"],[93,"removed",94,"item"],[94,"then",95,"exec"],[95,"then",96,"exec"],[96,"then",97,"exec"],[97,"then",98,"exec"],[97,"removed",98,"item"],[98,"then",99,"exec"],[99,"then",100,"exec"],[99,"removed",100,"item"],[100,"then",101,"exec"],[101,"then",102,"exec"],[102,"then",103,"exec"],[103,"then",104,"exec"],[104,"then",105,"exec"],[106,"value",105,"value"]]})"},
  };
  return list;
}

/// Host with a clock and a log; records prints.
struct Host : kern::logic::Host {
  kern::f64 t = 0;
  kern::u32 seed = 1;
  std::string log;
  kern::f64 time() const override { return t; }
  void print(const std::string& text, bool) override { log += "print " + text + "\n"; }
  void error(const std::string& text) override { log += "error " + text.substr(0, 40) + "\n"; }
  kern::u32 random() override { seed = seed * 1664525u + 1013904223u; return seed; }
};

inline kern::logic::Graph parse(const Case& c) {
  kern::json::Value j;
  kern::logic::Graph g;
  if (kern::json::parse(c.graph, j)) kern::logic::graph_from_json(j, g);
  return g;
}

/// The scenario every tier runs: Begin Play, 3 x Go, Open, 2 x Go, ticks for 2 s, Go.
/// Returns the variables and the log as text.
inline std::string play(kern::logic::Runtime& rt, Host& host, kern::logic::Instance* inst) {
  using kern::logic::Value;
  rt.budget = 20000;
  rt.begin();
  for (int k = 0; k < 3; ++k) rt.call("Go", 0, {{"value", Value::number(k * 2)}});
  rt.call("Open");
  for (int k = 0; k < 2; ++k) rt.call("Go", 0, {{"value", Value::number(5)}});
  for (int i = 0; i < 120; ++i) { host.t += 1.0 / 60; rt.tick(1.0f / 60); }
  rt.call("Go", 0, {{"value", Value::number(1)}});
  for (int i = 0; i < 60; ++i) { host.t += 1.0 / 60; rt.tick(1.0f / 60); }
  std::string out;
  for (const auto& v : inst->graph().variables) out += v.name + "=" + inst->get(v.name).as_text() + "\n";
  for (const auto& m : inst->graph().machines) out += m.name + ":" + inst->state(m.name) + "\n";
  out += inst->disabled() ? "disabled\n" : "running\n";
  return out + host.log;
}

}  // namespace logic_cases
