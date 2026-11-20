// Paket p: eine Klasse (Text von to_cpp, CASE_H) ohne Laufzeit spielen: on1(), dann R und W ausgeben wie logic_cases::play.
#include <cstdio>
#include <string>
#include CASE_H
int main() {
  Case c;
  c.on1();
  std::string s = "R=[";
  for (std::size_t i = 0; i < c.R.size(); ++i) s += (i ? ", " : "") + std::to_string(static_cast<unsigned long long>(c.R[i].bits));
  s += "]\nW=[";
  for (std::size_t i = 0; i < c.W.size(); ++i) s += (i ? ", " : "") + std::to_string(static_cast<long long>(c.W[i].v));
  std::puts((s + "]").c_str());
}
