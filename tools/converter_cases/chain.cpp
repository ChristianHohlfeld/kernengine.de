#include "chain.h"
namespace cases {
Chain& chain() { static Chain c; return c; }
kern::logic::ActorHandle Chain::root(unsigned id) const { return kern::logic::ActorHandle{id, 1}; }
long long Chain::step2(unsigned id) const { return step1(id) + base_; }
long long Chain::step4(unsigned id) const { return step3(id) + 1; }
}
