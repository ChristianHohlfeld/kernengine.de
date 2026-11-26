#include "chain_b.h"
namespace cases {
long long Chain::step1(unsigned id) const { return static_cast<long long>(root(id).id) + base_; }
long long Chain::step3(unsigned id) const { return step2(id) * 2; }
}
