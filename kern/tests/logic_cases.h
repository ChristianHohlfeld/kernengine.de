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
      // handlers run before the 65th level is refused, in every tier).
      {"function_reentry",
       R"({"variables":[{"name":"Log","type":"float","value":0},{"name":"Calls","type":"int","value":0},{"name":"DeepCount","type":"int","value":0}],
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
          {"id":44,"type":"func.call","params":{"function":"Outer"}},
          {"id":50,"type":"func.entry","params":{"function":"Outer"}},{"id":51,"type":"func.call","params":{"function":"Inner"}},
          {"id":60,"type":"func.entry","params":{"function":"Inner"}},{"id":61,"type":"flow.branch"},{"id":62,"type":"math.compare","params":{"op":">"},"values":{"b":0}},
          {"id":63,"type":"flow.call","params":{"name":"Deep"}},{"id":64,"type":"math.sub","values":{"b":1}}],
        "links":[[20,"then",21,"exec"],[21,"then",22,"exec"],[22,"then0",23,"exec"],[20,"x",24,"a"],[24,"result",23,"condition"],[23,"true",25,"exec"],[20,"x",27,"a"],[27,"result",25,"value"],
          [25,"then",26,"exec"],[22,"then1",26,"exec"],[29,"value",28,"acc"],[20,"x",28,"x"],[28,"result",26,"value"],
          [30,"then",31,"exec"],[30,"value",31,"x"],[32,"then",33,"exec"],[32,"value",33,"x"],
          [40,"then",41,"exec"],[42,"then",43,"exec"],[43,"then",44,"exec"],[42,"value",44,"v"],
          [50,"then",51,"exec"],[50,"v",51,"v"],[60,"then",61,"exec"],[60,"v",62,"a"],[62,"result",61,"condition"],[61,"true",63,"exec"],[60,"v",64,"a"],[64,"result",63,"value"]]})"},
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
      {"infinite_loop",
       R"({"variables":[{"name":"N","type":"int","value":0}],
        "nodes":[{"id":1,"type":"event.begin"},{"id":2,"type":"flow.while","values":{"condition":true}},{"id":3,"type":"var.add","params":{"variable":"N"}}],
        "links":[[1,"then",2,"exec"],[2,"body",3,"exec"]]})"},
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
