// Kern Logic: the engine's functions as a Blueprint's class calls them itself.
//
// A node of the engine reaches the engine through one door, logic::Host::engine: its inputs as
// values (logic::Value), by the node's name (kern/logic/engine_host.h). The interpreter and the JIT
// call it so. A Blueprint's class - the exported game - calls the function of the engine itself,
// where its host gives it the engine's objects (Host::engine_objects, BlueprintWorld::engine):
//
//   Hit = engine_world_cast_ray(World, From, To, Filter, 0);
//     is   objects.get<physics::World>(World)->cast_ray(From, To, filter, {})
//
// - typed: every input is made into what the C++ function takes from the C++ type it has in the
//   class (a Float is the number, a Vector the Vec3, a struct of the Blueprint the engine's struct
//   field by field, a List<Vector> the std::vector it is), and what the function gives is made into
//   the class's type the same way. No logic::Value is made, nothing is looked up by a name;
// - a handle is looked up once, in the same EngineObjects the door uses (one object, one handle,
//   whoever asks);
// - what it does is what the door does with the same call, bit for bit - also where a call fails
//   (a handle of no object of the class: the host is told, Host::engine_failed, and the outputs are
//   their zeros). `Direct<Tag>` below says for each Blueprint type what `Conv<Tag>` of the door says
//   through a Value (engine_host.h); the tests compare the two for every node (blueprint_native.h,
//   engine_calls_* in tests/test_main.cpp).
//
// tools/engine_api.py writes both from the same table of the engine's functions: the calls behind
// the door (src/<component>/engine_calls.gen.cpp) and the class's functions with these calls
// (kern/logic/engine_nodes.gen.h).
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/logic/engine_host.h"
#include "kern/logic/nodes.h"

namespace kern::logic::engine_values {

/// A value of the Blueprint type `Tag` (the tags of engine_host.h) between the C++ type B a
/// Blueprint's class holds it as and the C++ type C the engine's function takes or gives:
/// `from(objects, b, c)` what the function is given, `make<B>(objects, c)` what it gave - and
/// `pass(objects, c)` what it gave where the Blueprint reads it nowhere: nothing is made of it, but an
/// object of the engine in it has its handle from then on, as it has behind the door.
template <class Tag> struct Direct;

/// What the engine gave for one field of a struct, as the type the field has (the struct is made of
/// its fields as they come - none is made first with its default and then set).
template <class Tag, class C> struct Made {
  EngineObjects& o;
  const C& c;
  /// (Not a list written out in braces: a List is made as the list it is.)
  template <class F>
    requires(!detail::is_initializer_list<F>)
  operator F() const {
    return Direct<Tag>::template make<F>(o, c);
  }
};
template <class Tag, class C> Made<Tag, C> made(EngineObjects& o, const C& c) { return {o, c}; }

template <> struct Direct<Bool> {
  template <class C> static void from(EngineObjects&, bool b, C& c) {
    static_assert(std::is_same_v<C, bool>, "a Bool pin of something else than a bool");
    c = b;
  }
  template <class B> static B make(EngineObjects&, bool c) { return B(c); }
  static void pass(EngineObjects&, bool) {}
};
/// (A whole number - or a pointer to an object of the engine: its handle; or an object of the engine a
/// value holds as a value: the one of the handle, copied there.)
template <> struct Direct<Int> {
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) {
    static_assert(std::is_same_v<B, Whole> || std::is_same_v<B, i64>, "an Int pin the class holds as something else than an Int");
    const i64 v = normal(static_cast<i64>(b));
    if constexpr (std::is_pointer_v<C>) {
      c = o.get<std::remove_pointer_t<C>>(v);
    } else if constexpr (std::is_class_v<C> && !is_count<C>) {
      if (const C* p = o.get<C>(v)) c = *p;
      else c = blank<C>();
    } else {
      static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number");
      if constexpr (is_count<C>) c = C(static_cast<u32>(v));
      else c = static_cast<C>(v);
    }
  }
  template <class B, class C> static B make(EngineObjects& o, const C& c) {
    static_assert(std::is_same_v<B, Whole> || std::is_same_v<B, i64>, "an Int pin the class holds as something else than an Int");
    if constexpr (std::is_pointer_v<C>) {
      return B(normal(c ? o.adopt(*c) : 0));
    } else if constexpr (std::is_class_v<C> && !is_count<C>) {
      return B(normal(o.own(std::make_unique<C>(c))));
    } else {
      static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number");
      if constexpr (is_count<C>) return B(normal(static_cast<i64>(static_cast<u32>(c))));
      else return B(normal(static_cast<i64>(c)));
    }
  }
  template <class C> static void pass(EngineObjects& o, const C& c) {
    if constexpr (std::is_pointer_v<C>) {
      if (c) o.adopt(*c);
    } else if constexpr (std::is_class_v<C> && !is_count<C>) {
      o.own(std::make_unique<C>(c));
    }
  }
};
template <> struct Direct<Float> {
  template <class C> static void from(EngineObjects&, f64 b, C& c) {
    static_assert(std::is_floating_point_v<C>, "a Float pin of something else than a number");
    c = static_cast<C>(b);
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_floating_point_v<C>, "a Float pin of something else than a number");
    static_assert(std::is_same_v<B, f64>, "a Float pin the class holds as something else than a Float");
    return static_cast<f64>(c);
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
template <> struct Direct<Vector> {
  template <class C> static void from(EngineObjects&, const Vec3& b, C& c) {
    static_assert(std::is_same_v<C, Vec3> || std::is_same_v<C, Vec2>, "a Vector pin of something else than a vector");
    if constexpr (std::is_same_v<C, Vec3>) c = b;
    else c = Vec2(b.x, b.y);
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<C, Vec3> || std::is_same_v<C, Vec2>, "a Vector pin of something else than a vector");
    static_assert(std::is_same_v<B, Vec3>, "a Vector pin the class holds as something else than a Vector");
    if constexpr (std::is_same_v<C, Vec3>) return c;
    else return Vec3(c.x, c.y, 0);
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
template <> struct Direct<Text> {
  template <class C> static void from(EngineObjects& o, const logic::Text& b, C& c) {
    static_assert(std::is_same_v<C, std::string> || std::is_same_v<C, const char*>, "a Text pin held as something else than a std::string");
    if constexpr (std::is_same_v<C, const char*>) c = o.text(b.str());   // (a field that points at a text: one that stays)
    else c = b.view();
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<B, logic::Text>, "a Text pin the class holds as something else than a Text");
    if constexpr (std::is_pointer_v<std::decay_t<C>>) return logic::Text(c ? std::string(c) : std::string());
    else return logic::Text(std::string(c));
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
template <> struct Direct<Int64> {
  template <class C> static void from(EngineObjects&, logic::Int64 b, C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    c = static_cast<C>(b.bits);
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    static_assert(std::is_same_v<B, logic::Int64>, "an Int64 pin the class holds as something else than an Int64");
    return logic::Int64(static_cast<u64>(c));
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
/// An enum: the Blueprint's enum has the engine's values in their order (E::values) - a value is its
/// place among them.
template <class E> struct Direct<Enum<E>> {
  static constexpr std::size_t count = sizeof(E::values) / sizeof(E::values[0]);
  template <class B, class C> static void from(EngineObjects&, const B& b, C& c) {
    static_assert(std::is_same_v<C, typename E::type>, "an enum pin of another enum");
    static_assert(std::is_enum_v<B>, "an enum pin the class holds as something else than an enum");
    const i64 k = static_cast<i64>(b);
    c = E::values[k >= 0 && static_cast<u64>(k) < count ? static_cast<std::size_t>(k) : 0];
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<C, typename E::type>, "an enum pin of another enum");
    static_assert(std::is_enum_v<B>, "an enum pin the class holds as something else than an enum");
    static_assert(detail::enum_count<B>() == count, "an enum of the Blueprint that has not the values of the engine's enum");
    std::size_t at = 0;
    for (std::size_t k = 0; k < count; ++k)
      if (E::values[k] == c) {
        at = k;
        break;
      }
    return static_cast<B>(static_cast<i64>(at));
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
/// A struct: S has `from(objects, b, engine's&)`, `make<B>(objects, engine's)` and `pass`, field by
/// field (written by tools/engine_api.py: the Blueprint's struct has the engine's fields in their order).
template <class S> struct Direct<Struct<S>> {
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) { S::from(o, b, c); }
  template <class B, class C> static B make(EngineObjects& o, const C& c) { return S::template make<B>(o, c); }
  template <class C> static void pass(EngineObjects& o, const C& c) { S::pass(o, c); }
};
template <class X, class Y> struct Direct<Pair<X, Y>> {
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) {
    const auto& [first, second] = b;
    Direct<X>::from(o, first, c.first);
    Direct<Y>::from(o, second, c.second);
  }
  template <class B, class C> static B make(EngineObjects& o, const C& c) {
    B b{made<X>(o, c.first), made<Y>(o, c.second)};
    [[maybe_unused]] const auto& [first, second] = b;   // (a struct of these two fields, and no more)
    return b;
  }
  template <class C> static void pass(EngineObjects& o, const C& c) {
    Direct<X>::pass(o, c.first);
    Direct<Y>::pass(o, c.second);
  }
};

namespace direct {
template <class B> inline constexpr bool is_fixed = false;
template <class T, std::size_t N> inline constexpr bool is_fixed<Fixed<T, N>> = true;
}  // namespace direct

template <class T> struct Direct<List<T>> {
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) {
    using L = ListOf<C>;
    static_assert(L::is, "a list pin of something else than a list");
    static_assert(!std::is_pointer_v<C>, "a list the engine is given as a pointer is held in a list of its own (engine_nodes.gen.h)");
    const std::size_t n = b.size();
    auto at = b.begin();
    if constexpr (requires { c.data; c.size; }) {   // (a view: of entries held for the call)
      auto& kept = o.template held<typename L::item>();
      kept.resize(n);
      for (std::size_t k = 0; k < n; ++k, ++at) Direct<T>::from(o, *at, kept[k]);
      c = C(kept.data(), kept.size());
    } else if constexpr (L::maybe) {
      if (n == 0) {
        c = C{};
      } else {
        auto x = blank<typename L::item>();
        Direct<T>::from(o, *at, x);
        if constexpr (std::is_same_v<C, std::optional<typename L::item>>) c = std::move(x);
        else c = std::make_unique<typename L::item>(std::move(x));
      }
    } else if constexpr (L::grows) {
      c.clear();
      c.resize(n);
      for (std::size_t k = 0; k < n; ++k, ++at) {
        auto x = blank<typename L::item>();
        Direct<T>::from(o, *at, x);
        c[k] = std::move(x);
      }
    } else {
      const std::size_t m = std::extent_v<C> ? std::extent_v<C> : std::size(c);
      for (std::size_t k = 0; k < m; ++k) {   // (a list of a fixed length: what the value does not have is zero)
        auto x = blank<typename L::item>();
        if (k < n) {
          Direct<T>::from(o, *at, x);
          ++at;
        }
        c[k] = std::move(x);
      }
    }
  }
  /// (A list of a fixed length keeps its length: what the engine gave beyond it is made and nobody reads it.)
  template <class B, class C> static B make(EngineObjects& o, const C& c) {
    using L = ListOf<C>;
    static_assert(L::is, "a list pin of something else than a list");
    B b{};
    std::size_t k = 0;
    const auto put = [&](const auto& x) {
      if constexpr (direct::is_fixed<B>) {
        if (k < b.size()) b.items[k] = Direct<T>::template make<typename B::Item>(o, x);
        else (void)Direct<T>::template make<typename B::Item>(o, x);
      } else {
        b.push_back(Direct<T>::template make<typename B::Item>(o, x));
      }
      ++k;
    };
    if constexpr (L::maybe) {
      if (c) put(*c);
    } else {
      for (const auto& x : c) put(x);
    }
    return b;
  }
  template <class C> static void pass(EngineObjects& o, const C& c) {
    using L = ListOf<C>;
    static_assert(L::is, "a list pin of something else than a list");
    if constexpr (L::maybe) {
      if (c) Direct<T>::pass(o, *c);
    } else {
      for (const auto& x : c) Direct<T>::pass(o, x);
    }
  }
};
/// A list inside a list: in the Blueprint a struct whose one field is the inner list.
template <class L> struct Direct<Row<L>> {
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) {
    const auto& [inner] = b;
    Direct<L>::from(o, inner, c);
  }
  template <class B, class C> static B make(EngineObjects& o, const C& c) {
    B b{made<L>(o, c)};
    [[maybe_unused]] const auto& [inner] = b;   // (a struct of this one field)
    return b;
  }
  template <class C> static void pass(EngineObjects& o, const C& c) { Direct<L>::pass(o, c); }
};

/// What the class holds as the very type the function takes - a Float where it takes an f64, a Vector
/// where it takes a Vec3, a List<Vector> where it takes a std::vector<Vec3>: handed over as it stands.
template <class Tag, class C, class B> inline constexpr bool passes = false;
template <> inline constexpr bool passes<Bool, bool, bool> = true;
template <> inline constexpr bool passes<Float, f64, f64> = true;
template <> inline constexpr bool passes<Vector, Vec3, Vec3> = true;
template <class T, class X, class B> inline constexpr bool passes<List<T>, std::vector<X>, B> = std::is_base_of_v<std::vector<X>, B> && passes<T, X, X>;

/// An input the function only reads, as the C++ type C it takes: the class's own value where it is one
/// (`passes`: a reference to it - nothing is copied), else a C made from it.
template <class Tag, class C, class B> decltype(auto) taken(EngineObjects& o, const B& b) {
  if constexpr (passes<Tag, C, B>) {
    return static_cast<const C&>(b);
  } else {
    C c = blank<C>();
    Direct<Tag>::from(o, b, c);
    return c;
  }
}

/// A Text as a function reads it through a `const char*`: its characters where they stand, if a 0 ends
/// them there (a text written out in the program, a text made while running) - else a copy that has one.
class Chars {
 public:
  explicit Chars(const logic::Text& text) : at_(text.ended()) {
    if (!at_) {
      own_ = text.str();
      at_ = own_.c_str();
    }
  }
  Chars(const Chars&) = delete;
  Chars& operator=(const Chars&) = delete;
  const char* c_str() const { return at_; }

 private:
  std::string own_;
  const char* at_;
};
/// A Text as a function reads it through a `const std::string&`: the std::string it holds (a text made
/// while running), else one made of its characters.
class String {
 public:
  explicit String(const logic::Text& text) : at_(text.made()) {
    if (!at_) {
      own_ = text.str();
      at_ = &own_;
    }
  }
  String(const String&) = delete;
  String& operator=(const String&) = delete;
  const std::string& get() const { return *at_; }

 private:
  std::string own_;
  const std::string* at_;
};

/// An output of a call that failed before it was given: the zero of its type (a struct: its defaults).
template <class B> void none(B& b) { b = B{}; }
/// An object of the engine a function gave, as the Int the class holds: its handle.
template <class B> B handle_as(i64 handle) { return B(normal(handle)); }

namespace direct {
template <class R> struct Hold {   // what a function gave by value: kept here
  std::optional<R> given;
  void set(R&& r) { given.emplace(std::move(r)); }
  const R* get() const { return given ? &*given : nullptr; }
};
template <class R> struct Hold<R&> {   // ... by reference: where the engine has it
  const R* given = nullptr;
  void set(R& r) { given = &r; }
  const R* get() const { return given; }
};
}  // namespace direct

/// What a function of the engine gave where a Blueprint's own type stands - one of its structs or
/// enums, or a list of them: read as that type where it is used. Where the class called through the
/// door, it holds the value the host gave instead. `R` is what the C++ function returns, `Tag` the
/// Blueprint type of the pin.
template <class Tag, class R> class Given {
 public:
  /// Through the door: the value the host gave.
  explicit Given(Value held) : held_(std::move(held)) {}
  /// Called on the engine itself, and the call failed: nothing was given (read, it is the zero of the type).
  explicit Given(EngineObjects& objects) : objects_(&objects) {}
  /// Called on the engine itself: what the function returned.
  Given(EngineObjects& objects, R&& r) : objects_(&objects) { hold_.set(std::forward<R>(r)); }
  /// (Not a list written out in braces: a List is assigned as the list it is.)
  template <class T>
    requires(!detail::is_initializer_list<T>)
  operator T() const {
    if (!objects_) return from_value<T>(held_);
    if (const auto* c = hold_.get()) return Direct<Tag>::template make<T>(*objects_, *c);
    return T{};
  }

 private:
  EngineObjects* objects_ = nullptr;
  direct::Hold<R> hold_;
  Value held_;
};

}  // namespace kern::logic::engine_values
