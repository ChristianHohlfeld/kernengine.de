#include <cstdio>
#include <cstdint>
#include "logic_cases.h"
using namespace kern; using namespace kern::logic;
static uintptr_t hi=0, lo=~uintptr_t(0); static int prints=0;
struct H : logic_cases::Host { void print(const std::string& t, bool s) override { volatile char x; uintptr_t a=(uintptr_t)&x; if(a>hi)hi=a; if(a<lo)lo=a; ++prints; } };
const char* deep_graph(); static const char* deep = R"({"variables":[{"name":"R","type":"int","value":0}],
 "functions":[{"name":"Deep","inputs":[{"name":"n","type":"int","value":0}],"outputs":[]},
              {"name":"Fib","inputs":[{"name":"n","type":"int","value":0}],"outputs":[{"name":"r","type":"int","value":0}]}],
 "nodes":[{"id":10,"type":"func.entry","params":{"function":"Deep"}},{"id":11,"type":"flow.branch"},{"id":12,"type":"math.compare","params":{"op":">"},"values":{"b":0}},
  {"id":13,"type":"debug.print","values":{"text":"x","screen":false}},{"id":14,"type":"func.call","params":{"function":"Deep"}},{"id":15,"type":"math.sub","values":{"b":1}},
  {"id":1,"type":"event.custom","params":{"name":"Go"}},{"id":2,"type":"func.call","params":{"function":"Deep"}},
  {"id":20,"type":"func.entry","params":{"function":"Fib"}},{"id":21,"type":"flow.branch"},{"id":22,"type":"math.compare","params":{"op":"<"},"values":{"b":2}},
  {"id":23,"type":"func.return","params":{"function":"Fib"}},{"id":24,"type":"func.call","params":{"function":"Fib"}},{"id":25,"type":"math.sub","values":{"b":1}},
  {"id":26,"type":"func.call","params":{"function":"Fib"}},{"id":27,"type":"math.sub","values":{"b":2}},{"id":28,"type":"func.return","params":{"function":"Fib"}},{"id":29,"type":"math.add"},
  {"id":3,"type":"event.custom","params":{"name":"F"}},{"id":4,"type":"func.call","params":{"function":"Fib"}},{"id":5,"type":"var.set","params":{"variable":"R"}}],
 "links":[[10,"then",11,"exec"],[10,"n",12,"a"],[12,"result",11,"condition"],[11,"true",13,"exec"],[13,"then",14,"exec"],[10,"n",15,"a"],[15,"result",14,"n"],
  [1,"then",2,"exec"],[1,"value",2,"n"],
  [20,"then",21,"exec"],[20,"n",22,"a"],[22,"result",21,"condition"],[21,"true",23,"exec"],[20,"n",23,"r"],[21,"false",24,"exec"],[20,"n",25,"a"],[25,"result",24,"n"],
  [24,"then",26,"exec"],[20,"n",27,"a"],[27,"result",26,"n"],[26,"then",28,"exec"],[24,"r",29,"a"],[26,"r",29,"b"],[29,"result",28,"r"],
  [3,"then",4,"exec"],[3,"value",4,"n"],[4,"then",5,"exec"],[4,"r",5,"value"]]})";
int main(int argc,char**argv){
  int N = argc>1?atoi(argv[1]):1000;
  H host; Runtime rt(host); rt.budget = 100000000;
  logic_cases::Case c{"deep",deep};
  Instance* inst = rt.add(logic_cases::parse(c),0,"m");
  rt.begin();
  rt.call("Go",0,{{"value",Value::number(N)}});
  std::printf("disabled=%d prints=%d bytes/level=%.1f\n", inst->disabled(), prints, prints>1? double(hi-lo)/(prints-1):0.0);
  for (int k : {5,10,15}) { rt.call("F",0,{{"value",Value::number(k)}}); std::printf("Fib(%d) interpreter=%lld\n", k, (long long)inst->get("R").as_int()); }
  std::printf("log: %s\n", host.log.substr(0,300).c_str());
}
const char* deep_graph(){return deep;}
