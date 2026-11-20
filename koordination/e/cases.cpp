#include <cstdio>
#include "logic_cases.h"
using namespace kern::logic;
int main(){ for(const auto& c: logic_cases::cases()){ logic_cases::Host host; Runtime rt(host); rt.tracing = true; Instance* inst=rt.add(logic_cases::parse(c),0,"m"); std::string r=logic_cases::play(rt,host,inst); std::printf("== %s\n%s", c.name, r.c_str()); } }
