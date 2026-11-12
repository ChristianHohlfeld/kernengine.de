// Native unit tests of SpawnCity's simulation models (no GL, deterministic):
// flight controller + drone plant, the jet's aerodynamics and fly-by-wire, the car's
// suspension and tyres, the map decoder and the game rules.
//
//   kern/games/spawncity/tests/run_tests.sh
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "blueprint_native.h"
#include "rules.h"
#include "test_util.h"

namespace sct {
std::vector<Case>& cases() { static std::vector<Case> c; return c; }
int failures = 0, checks = 0;
}  // namespace sct

// The calls of the native test of the rules' Blueprint: every function with many values, then a
// chase - crimes, out of sight, the escape.
static blueprint_native::Case wanted_case() {
  using kern::logic::PinType;
  using kern::logic::Value;
  blueprint_native::Case c;
  c.name = "Wanted";
  std::string error;
  if (!sc::rules::actor_graph("Wanted", c.graph, &error)) std::printf("the level has no Wanted Blueprint: %s\n", error.c_str());
  const double numbers[] = {0, 3.9, 4, 8.5, 9, 15, 22, 30, 35, 60, -2, 0.016, 6, 38, 1e6};
  for (const kern::logic::Function& f : c.graph.functions) {
    if (!f.pure) continue;
    for (int r = 0; r < 30; ++r) {
      blueprint_native::Call call{f.name, {}};
      for (std::size_t k = 0; k < f.inputs.size(); ++k) {
        const int at = r * static_cast<int>(2 * k + 1) + static_cast<int>(k) * 4;
        if (f.inputs[k].type == PinType::integer) call.args.push_back(Value::integer(at % 9 - 1));
        else if (f.inputs[k].type == PinType::boolean) call.args.push_back(Value::boolean(at % 2 == 0));
        else call.args.push_back(Value::number(numbers[static_cast<std::size_t>(at) % (sizeof numbers / sizeof numbers[0])]));
      }
      c.calls.push_back(std::move(call));
      if (f.inputs.empty()) break;
    }
  }
  auto report = [&](int crime, bool witnessed) { c.calls.push_back({"Report", {Value::integer(crime), Value::boolean(witnessed)}}); };
  auto update = [&](double dt, bool seen) { c.calls.push_back({"Update", {Value::number(dt), Value::boolean(seen)}}); };
  auto stars = [&] { c.calls.push_back({"Stars", {}}); };
  for (int crime = -1; crime <= 10; ++crime) {
    report(crime, crime % 3 != 0);
    stars();
  }
  update(0.5, true);
  for (int i = 0; i < 12; ++i) {
    update(1.0, false);
    stars();
  }
  for (int i = 0; i < 11; ++i) report(6, true);
  stars();
  for (int i = 0; i < 5; ++i) update(2.0, i == 2);
  stars();
  c.calls.push_back({"Clear", {}});
  stars();
  update(1.0 / 60.0, false);
  return c;
}

// The same for the Blueprint of the actor "CombatDetail": every function with many values of its
// inputs' types - enums, structs and lists among them.
/// Every function of a rule actor's Blueprint with many values of its inputs' types - enums,
/// structs and lists among them.
static blueprint_native::Case rule_case(const char* actor) {
  using kern::logic::Value;
  blueprint_native::Case c;
  c.name = actor;
  std::string error;
  if (!sc::rules::actor_graph(actor, c.graph, &error)) std::printf("the level has no %s Blueprint: %s\n", actor, error.c_str());
  for (const kern::logic::Function& f : c.graph.functions) {
    std::vector<std::vector<Value>> samples;
    for (const kern::logic::FunctionPin& p : f.inputs) samples.push_back(blueprint_native::samples(kern::logic::TypeRef{p.type, p.of}, c.graph));
    for (std::size_t r = 0; r < 40; ++r) {
      blueprint_native::Call call{f.name, {}};
      for (std::size_t k = 0; k < samples.size(); ++k)
        if (!samples[k].empty()) call.args.push_back(samples[k][(r * (2 * k + 1) + k * 3) % samples[k].size()]);
      c.calls.push_back(std::move(call));
      if (f.inputs.empty()) break;
    }
  }
  return c;
}

static blueprint_native::Case combat_detail_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("CombatDetail");
  // a ballot from its start to unanimous, and the clock of a drone that turns critical and blows
  auto ints = [](std::initializer_list<kern::i64> v) { return Value::of(std::vector<kern::i64>(v)); };
  c.calls.push_back({"CreateBallot", {Value::text("b"), Value::text("friendlyFire"), Value::boolean(true), Value::integer(2), ints({3, 1, 2, 2}), Value::number(0)}});
  c.calls.push_back({"CreateBallot", {Value::text("b"), Value::text("friendlyFire"), Value::boolean(true), Value::integer(9), ints({1, 2}), Value::number(0)}});
  for (double hp : {100.0, 28.0, 20.0, 20.0})
    c.calls.push_back({"ClockUpdate", {Value::enumeration(3), Value::number(hp), Value::number(100), Value::number(1.5), Value::number(hp < 25 ? 3.05 : -1), Value::number(hp < 25 ? 28 : 100)}});
  return c;
}

// The Blueprint of the actor "Guided": its functions with many values, and a missile from the
// rail to its target - every step the class gives is the interpreter's.
static blueprint_native::Case guided_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("Guided");
  auto missile = [](kern::Vec3 p, kern::Vec3 v, kern::Vec3 goal, double born, double cruise, bool motor) {
    return Value::structure({Value::vector(p), Value::vector(v), Value::vector(goal), Value::number(born), Value::number(cruise), Value::boolean(motor), Value::boolean(true), Value::number(0.2),
                             Value::boolean(false), Value::vector({goal.x, goal.y, 0}), Value::boolean(true)});
  };
  c.calls.push_back({"Launch", {Value::vector({0, 0, 20}), Value::vector({300, 0, 1}), Value::number(0), Value::boolean(false)}});
  for (double now : {0.1, 0.5, 3.0, 30.0})
    for (float x : {0.0f, 120.0f, 260.0f, 299.0f})
      c.calls.push_back({"Step", {missile({x, 1, 25}, {30, 0, -2}, {300, 0, 1}, 0, 29, now > 0.4), Value::number(now), Value::number(1.0 / 60), Value::number(x > 250 ? 26 : 0)}});
  for (float at : {0.0f, 0.4f})
    c.calls.push_back({"Track", {missile({0, 0, 20}, {30, 0, 0}, {100, 0, 1}, 0, 29, true), Value::vector({100 + at, at, 1}), Value::number(0.05)}});
  return c;
}

int main(int argc, char** argv) {
  // spawncity_tests --emit-native <dir>: the Wanted Blueprint as its C++ text, calls to it and
  // the interpreter's answers (run_tests.sh compiles them into wanted_native_test).
  if (argc > 2 && std::strcmp(argv[1], "--emit-native") == 0) {
    std::string error;
    if (!blueprint_native::emit(argv[2], {wanted_case(), combat_detail_case(), guided_case(), rule_case("SpawnGuard")}, &error)) {
      std::printf("FAIL --emit-native: %s\n", error.c_str());
      return 1;
    }
    return 0;
  }
  const char* only = argc > 1 ? argv[1] : nullptr;
  int ran = 0;
  for (const sct::Case& c : sct::cases()) {
    if (only && !std::strstr(c.name, only)) continue;
    const int before = sct::failures;
    c.fn();
    ++ran;
    std::printf("%s %s\n", sct::failures == before ? "ok  " : "FAIL", c.name);
  }
  std::printf("%d tests, %d checks, %d failures\n", ran, sct::checks, sct::failures);
  return sct::failures ? 1 : 0;
}
