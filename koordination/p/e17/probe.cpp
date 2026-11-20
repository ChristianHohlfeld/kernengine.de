#include "test_util.h"
#undef SC_TEST
#define SC_TEST(name) [[maybe_unused]] static void name()
#include "test_common.cpp"
double probe(double a) { return converted_common::wrap_pi(a); }
