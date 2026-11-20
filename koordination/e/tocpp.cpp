#include <cstdio>
#include "logic_cases.h"
extern const char* deep_graph();
using namespace kern::logic;
static int trip(const Graph& g0, const char* name, bool show) {
  std::string text,err,again; Graph back;
  if(!to_cpp(g0,name,text,&err)){ std::printf("%s: to_cpp: %s\n",name,err.c_str()); return 1; }
  if(show) std::printf("%s",text.c_str());
  if(!from_cpp(text,back,nullptr,&err)){ std::printf("%s: from_cpp: %s\n",name,err.c_str()); return 1; }
  if(!to_cpp(back,name,again,&err) || again!=text){ std::printf("%s: not a fixpoint\n",name); return 1; }
  return 0;
}
int read_file(const char*);
int main(int argc,char**argv){ if(argc>2) return read_file(argv[2]); int bad=0; logic_cases::Case c{"d",deep_graph()}; bad+=trip(logic_cases::parse(c),"Rec",argc>1);
  for(const auto& k: logic_cases::cases()) bad+=trip(logic_cases::parse(k),"Case",false);
  std::printf("%zu cases + Rec, %d bad\n", logic_cases::cases().size(), bad); }
#include <fstream>
#include <sstream>
int read_file(const char* f){ std::ifstream i(f); std::stringstream s; s<<i.rdbuf(); Graph g; std::string err,name,text; bool ok=from_cpp(s.str(),g,&name,&err); std::printf("read=%d %s\n",ok,err.c_str()); if(ok){to_cpp(g,name,text,&err); std::printf("%s", text.c_str());} return !ok; }
