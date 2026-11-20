// Paket p: Klasse des Falls format_text_of_value ohne Laufzeit spielen (ASan), T ausgeben.
#include <cstdio>
#include CASE_H
int main() { Case c; c.on1(); std::puts(std::string(c.T.view()).c_str()); }
