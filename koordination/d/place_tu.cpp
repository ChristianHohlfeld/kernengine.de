#include <cstdio>
#include <string>
#include "kern/kern.h"
#include "kern/logic/blueprint.h"
#include "kern/logic/logic.h"
#include "kern/logic/engine_host.h"
namespace kern::logic::detail {
template <class S, auto... M> struct HeldBy {
  static constexpr logic::PinType type = logic::PinType::structure;
  static bool is(const logic::Value& v) {
    if (v.type != type || v.fields() != sizeof...(M)) return false;
    std::size_t k = 0;
    return (logic::is_value<std::remove_cvref_t<decltype(std::declval<S&>().*M)>>(*v.field(k++)) && ...);
  }
  static S from(const logic::Value& v) {
    S s{};
    if (!is(v)) return s;
    std::size_t k = 0;
    ((s.*M = logic::from_value<std::remove_cvref_t<decltype(s.*M)>>(*v.field(k++))), ...);
    return s;
  }
  static logic::Value to(const S& s) { return logic::Value::structure({logic::to_value(s.*M)...}); }
};
}
#include "place_probe.h"
int main() {
  engine_place_probe::Places p;
  p.Bind(); p.Change(); p.Release(); p.Read();
  return 0;
}
