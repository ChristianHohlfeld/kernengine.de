// How a program's own C++ calls a function of the Blueprint of one of the actors of a world: what Call
// on Actor is in a Blueprint (docs/blueprint_cpp.md, "Aufrufe aus C++"). The C++ names the actor and
// binds it - to the world the actor is in (a kern::game::GameWorld the program runs), or to the logic
// its own Blueprints run in -; a call gives its values in and takes the function's outputs back, typed:
//
//   kern::logic::ActorBlueprint rules{"Military"};
//   if (!rules.bind(world)) ...      // (the world has no actor of that name, or its Blueprint does not run)
//   f64 chance = 0;
//   rules.call("HitChance", std::tie(chance), distance, speed);
//
// is, written as a program's Blueprint that holds the actor (a variable `Rules` of the type Actor),
// Rules = engine_game_world_actor_named(world, "Military"); func_has_blueprint(Rules) and
// func_call_on<"HitChance">(Rules, kern::out(n7.chance), distance, speed); - bound to a logic (`bind(logic)`),
// func_has_blueprint(actor_find("Military")) and func_call_on<"HitChance">(actor_find("Military"), ...): the
// actor of that name where its Blueprint runs. The same call in every stage, through the same way of the
// runtime (Runtime::call_on, Runtime::get_on).
#pragma once

#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/core/fields.h"
#include "kern/logic/logic.h"

namespace kern::logic {

namespace detail {
template <class T> struct IsVector : std::false_type {};
template <class T> struct IsVector<std::vector<T>> : std::true_type {};
}  // namespace detail

/// A world whose actors a program's C++ reaches (ActorBlueprint::bind): one that names its actors as an Actor of it
/// (kern::game::GameWorld::actor_named - Actor{} with the reason in last_error where it has none of that name or does
/// not run), says whether it runs and as which world (every start is a new one) and gives the logic its Blueprints
/// run in. (A concept, not a class of kern::game: the logic does not know the game; a test's world is one as well.)
template <class W>
concept WorldOfActors = requires(W& w, const W& c, std::string_view name) {
  { c.actor_named(name) } -> std::same_as<ActorHandle>;
  { c.world_id() } -> std::convertible_to<WorldId>;
  { c.running() } -> std::convertible_to<bool>;
  { w.logic_runtime() } -> std::convertible_to<Runtime*>;
};

struct ActorBlueprint {
  explicit constexpr ActorBlueprint(const char* name) : actor(name) {}

  /// The actor whose Blueprint this is (its name).
  const char* const actor;

  /// Binds it to the actor of its name in `world` - a world the program runs (a kern::game::GameWorld), once it has
  /// started: the Actor of it is taken now (world.actor_named: its id and the world - kern/docs/project.md 7), and
  /// from now on every call and every variable it reads goes to that actor's running Blueprint in that world. What it
  /// gives back is Has Blueprint of that actor; false also where the world has no actor of the name or does not run
  /// (the reason in last_error) - then every call gives the zeros. When the world stops, or starts again (a new world,
  /// in which the Actor of the one before names nothing), a call reaches nothing: it gives the zeros, and the reason is
  /// reported (log, last_error) - as a program's Blueprint that holds the Actor is told. (The world is held by its
  /// address: it lives for as long as the program binds to it, as every object the C++ holds by reference.)
  template <WorldOfActors W> bool bind(W& world) {
    logic_ = nullptr;
    world_ = &world;
    reach_ = &reach<W>;
    handle_ = world.actor_named(actor);
    if (!handle_.valid()) return false;
    ActorRef id = 0;
    Runtime* there = target(id, "Has Blueprint");
    return there && there->has_blueprint(id);
  }
  /// The Actor it is bound to in a world (bind(world)): Actor{} where it is bound to a logic, to none, or the world had
  /// no actor of its name.
  ActorHandle handle() const { return handle_; }

  /// Binds it to the logic the level's Blueprints run in (the world's Runtime): from now on every call
  /// and every variable it reads goes there - to the actor of its name, found when it is called
  /// (Host::find_actor; a name the host does not know is no actor: the Level), and that actor's
  /// running Blueprint (Runtime::find). What it gives back is whether the actor has a Blueprint that
  /// runs there - Has Blueprint, func_has_blueprint(actor_find(actor)) in a Blueprint, where the logic
  /// is the Blueprint's own. Until it is bound, every call gives the zeros.
  /// (The logic it is bound to is the one of its actor's world: the logic of a world, or of the program's own
  /// Blueprints - Runtime::call_on of that logic, with no other world in between.)
  bool bind(Runtime& logic) {
    logic_ = &logic;
    world_ = nullptr;
    reach_ = nullptr;
    handle_ = {};
    return logic.has_blueprint(logic.host().find_actor(actor));
  }

  /// Runs `function` with `args` - each as a Blueprint takes it: a flag, a whole number, a decimal
  /// number, a vector, a text, an enum's value, a struct (a plain struct of the program whose fields
  /// are such values, in the order of the Blueprint's struct), a list of any of these - and puts its outputs, in their order, into `out`
  /// (std::tie(a, b)), each as the type of its place - converted as on a wire. Every output starts at
  /// the zero of its type: where the actor has no Blueprint that runs, or it has no such function, that
  /// is what the call gives - as Call on Actor gives the defaults its Blueprint declares for the
  /// function, and a Blueprint's declaration of a function of another actor has the zeros.
  /// A function that gives one number, or one flag: what it gives - 0, false where no Blueprint is bound or it
  /// has no such function. Call on Actor with one output, as a value: f64 speed = rules.number("Speed", x);
  template <class... A> f64 number(const char* function, const A&... args) const {
    f64 r = 0;
    call(function, std::tie(r), args...);
    return r;
  }
  template <class... A> bool flag(const char* function, const A&... args) const {
    bool r = false;
    call(function, std::tie(r), args...);
    return r;
  }
  /// The variable `name` of the actor's Blueprint as a T (as an output of call() is taken) - the zero of T
  /// where the actor has no Blueprint, it has no such variable or one whose type does not become a T
  /// (Runtime::get_on). Get Variable on Actor in a Blueprint.
  template <class T> T get(const char* name) const {
    T v{};
    Value held;
    ActorRef id = 0;
    Runtime* there = target(id, "Get Variable on Actor");
    if (there && there->get_on(id, name, pin_of<T>(), held)) taken(held.converted(pin_of<T>()), v);
    return v;
  }
  template <class... O, class... A> void call(const char* function, std::tuple<O&...> out, const A&... args) const {
    const std::vector<Value> in{argument(args)...};  // (first the values: one may be read from a place an output goes to)
    std::apply([](O&... o) { ((o = O{}), ...); }, out);
    std::vector<Value> results;
    ActorRef id = 0;
    Runtime* there = target(id, "Call on Actor");
    if (!there || !there->call_on(id, function, in, results, kCaller)) return;
    take(results, out, std::index_sequence_for<O...>{});
  }

 private:
  /// The logic it is bound to (null: none yet, or a world).
  Runtime* logic_ = nullptr;
  /// The world it is bound to (bind(world); null: none, or a logic), how its logic is reached (reach<W>), and the Actor
  /// of it there.
  void* world_ = nullptr;
  Runtime* (*reach_)(void* world, WorldId as, std::string& why) = nullptr;
  ActorHandle handle_{};

  /// The logic of `world` while it runs as the world `as`, else null and why.
  template <class W> static Runtime* reach(void* world, WorldId as, std::string& why) {
    W& w = *static_cast<W*>(world);
    if (!w.running()) {
      why = "its world does not run any more (the GameWorld was stopped)";
      return nullptr;
    }
    if (static_cast<WorldId>(w.world_id()) != as) {
      why = "its world ended and runs again as world " + std::to_string(static_cast<WorldId>(w.world_id())) + ", a new one (in which the Actor of the world before names nothing)";
      return nullptr;
    }
    Runtime* r = w.logic_runtime();
    if (!r) why = "its world runs no Blueprints";
    return r;
  }
  /// The logic a call of `node` goes to and the actor there (`id`; 0: the Level's): the logic it is bound to and the
  /// actor of its name there; of a world, its logic while the world runs as the one the Actor is of. Null where it goes
  /// nowhere - nothing bound, no actor of the name in the world, or a world that ended (reported: log and last_error).
  Runtime* target(ActorRef& id, const char* node) const {
    id = 0;
    if (logic_) {
      id = logic_->host().find_actor(actor);
      return logic_;
    }
    if (!reach_ || !handle_.valid()) return nullptr;
    std::string why;
    Runtime* there = reach_(world_, handle_.world, why);
    if (!there) {
      const std::string said = std::string(kCaller) + ": " + node + ": actor " + std::to_string(handle_.id) + " (" + actor + ") is of world " + std::to_string(handle_.world) + " - " + why +
                               " (kern/docs/project.md 7)";
      log(LogLevel::error, "%s", said.c_str());
      fail("%s", said.c_str());
      return nullptr;
    }
    id = handle_.id;
    return there;
  }
  /// Who calls, where a call is reported (Runtime::call_on): the program's C++.
  static constexpr const char* kCaller = "the program's C++";

  template <class T> static constexpr bool is_list = detail::IsVector<T>::value;
  /// A plain struct of the program that crosses as a Blueprint's struct (not a vector, not a list).
  template <class T> static constexpr bool is_struct = PlainStruct<T> && !std::is_same_v<T, Vec3> && !is_list<T>;
  /// A vector of the program's own type: one that becomes a Vec3 and is made of one (not a plain struct).
  template <class T> static constexpr bool is_vector = std::is_same_v<T, Vec3> || (!is_struct<T> && !is_list<T> && std::is_convertible_v<const T&, Vec3> && std::is_constructible_v<T, Vec3>);

  /// The pin type a value of the program's type T is in a Blueprint - what it is read as.
  template <class T> static constexpr PinType pin_of() {
    if constexpr (is_list<T>) return list_of(pin_of<typename T::value_type>());
    else if constexpr (std::is_same_v<T, bool>) return PinType::boolean;
    else if constexpr (std::is_enum_v<T>) return PinType::enumeration;
    else if constexpr (logic::num::Number<T>) return number_type<T>();   // (a number: its own type)
    else if constexpr (is_vector<T>) return PinType::vector;
    else if constexpr (std::is_same_v<T, std::string>) return PinType::text;
    else {
      static_assert(is_struct<T>, "a value is a flag, a whole number, a decimal number, a vector, a text, an enum's value, a plain struct of such values or a list of them");
      return PinType::structure;
    }
  }

  template <class T> static Value argument(const T& x) {
    static_assert(!std::is_same_v<T, f32>, "a 32-bit number goes in as the decimal it stands for, a 64-bit one (f64)");
    if constexpr (std::is_same_v<T, bool>) return Value::boolean(x);
    else if constexpr (std::is_enum_v<T>) return Value::enumeration(static_cast<i64>(x));
    else if constexpr (std::is_integral_v<T>) return Value::integer(static_cast<i64>(x));
    else if constexpr (std::is_same_v<T, f64>) return Value::number(x);
    else if constexpr (is_vector<T>) return Value::vector(static_cast<Vec3>(x));
    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) return Value::text(std::string(x));
    else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) return Value::text(x ? x : "");
    else if constexpr (is_list<T>) return list_argument(x);
    else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>) return Value::text(x);
    else {
      static_assert(is_struct<T>, "a value is a flag, a whole number, a decimal number (64 bit), a vector, a text, an enum's value, a plain struct of such values or a list of them");
      return std::apply([](const auto&... f) { return Value::structure({argument(f)...}); }, fields_of(x));
    }
  }
  template <class T> static Value list_argument(const std::vector<T>& items) {
    static_assert(!std::is_same_v<T, f32>, "a 32-bit number goes in as the decimal it stands for, a 64-bit one (f64)");
    if constexpr (std::is_same_v<T, bool>) return Value::of(std::vector<bool>(items.begin(), items.end()));
    else if constexpr (std::is_enum_v<T>) {
      std::vector<i64> r;
      r.reserve(items.size());
      for (T e : items) r.push_back(static_cast<i64>(e));
      return Value::enumerations(std::move(r));
    } else if constexpr (std::is_integral_v<T>) return Value::of(std::vector<i64>(items.begin(), items.end()));
    else if constexpr (std::is_same_v<T, f64> || std::is_same_v<T, Vec3> || std::is_same_v<T, std::string>) return Value::of(items);
    else if constexpr (is_vector<T>) return Value::of(std::vector<Vec3>(items.begin(), items.end()));
    else {
      std::vector<Value> r;
      r.reserve(items.size());
      for (const T& x : items) r.push_back(argument(x));
      return Value::structures(std::move(r));
    }
  }

  template <class T> static void taken(const Value& v, T& to) {
    if constexpr (std::is_same_v<T, bool>) to = v.b;
    else if constexpr (std::is_enum_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_integral_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_floating_point_v<T>) to = static_cast<T>(v.as_number());
    else if constexpr (is_vector<T>) to = T(v.as_vector());
    else if constexpr (std::is_same_v<T, std::string>) to = v.as_text();
    else if constexpr (is_list<T>) {
      using I = typename T::value_type;
      to.clear();
      if constexpr (std::is_same_v<I, bool> || std::is_same_v<I, f64> || std::is_same_v<I, Vec3> || std::is_same_v<I, std::string>) {
        const auto& got = v.items<I>();
        to.assign(got.begin(), got.end());
      } else if constexpr (std::is_enum_v<I> || std::is_integral_v<I>) {
        for (i64 x : v.items<i64>()) to.push_back(static_cast<I>(x));
      } else if constexpr (std::is_floating_point_v<I>) {
        for (f64 x : v.items<f64>()) to.push_back(static_cast<I>(x));
      } else if constexpr (is_vector<I>) {
        for (Vec3 x : v.items<Vec3>()) to.push_back(I(x));
      } else {
        to.resize(v.size());
        for (std::size_t k = 0; k < to.size(); ++k) taken(v.items<Value>()[k], to[k]);
      }
    } else {
      static_assert(is_struct<T>, "an output is a flag, a whole number, a decimal number, a vector, a text, an enum's value, a plain struct of such values or a list of them");
      // (a struct's value that holds no fields - the zero of the struct - has every field at its default: T{})
      to = T{};
      std::size_t k = 0;
      std::apply([&](auto&... f) { ((k < v.fields() ? taken(*v.field(k), f) : void(), ++k), ...); }, fields_of(to));
    }
  }
  template <class... O, std::size_t... I> static void take(const std::vector<Value>& results, std::tuple<O&...>& out, std::index_sequence<I...>) {
    ((I < results.size() ? taken(results[I].converted(pin_of<O>()), std::get<I>(out)) : void()), ...);
  }
};

}  // namespace kern::logic
