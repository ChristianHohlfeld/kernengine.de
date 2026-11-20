#include <chrono>
#include <cstdio>
#include "logic_cases.h"
using namespace kern; using namespace kern::logic;
static const char* g = R"({"variables":[{"name":"Total","type":"float","value":0}],
 "functions":[{"name":"Add","inputs":[{"name":"x","type":"float","value":0}],"outputs":[{"name":"r","type":"float","value":0}]}],
 "nodes":[{"id":10,"type":"func.entry","params":{"function":"Add"}},{"id":11,"type":"var.add","params":{"variable":"Total"}},{"id":12,"type":"func.return","params":{"function":"Add"}},{"id":13,"type":"math.mul","values":{"b":2}},
  {"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"flow.for","values":{"first":1}},{"id":3,"type":"func.call","params":{"function":"Add"}},{"id":4,"type":"var.add","params":{"variable":"Total"}}],
 "links":[[10,"then",11,"exec"],[10,"x",11,"amount"],[11,"then",12,"exec"],[10,"x",13,"a"],[13,"result",12,"r"],
  [1,"then",2,"exec"],[1,"value",2,"last"],[2,"body",3,"exec"],[2,"index",3,"x"],[3,"then",4,"exec"],[3,"r",4,"amount"]]})";
int main(){
  logic_cases::Host host; Runtime rt(host); rt.budget = 2000000000u;
  logic_cases::Case c{"b",g}; Instance* inst = rt.add(logic_cases::parse(c),0,"m"); rt.begin();
  double best=1e9;
  for(int r=0;r<(getenv("ONE")?1:7);r++){ auto t0=std::chrono::steady_clock::now(); rt.call("Go",0,{{"value",Value::number(getenv("ONE")?20000:200000)}}); double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count(); if(ms<best)best=ms; }
  std::printf("Total=%.0f best %.2f ms / 200000 calls = %.1f ns/call  log=%s\n", inst->get("Total").as_number(), best, best*1e6/200000, host.log.substr(0,80).c_str());
}
