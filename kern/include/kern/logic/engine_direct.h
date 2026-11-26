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

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <tuple>
#include <vector>

#include "kern/logic/engine_host.h"
#include "kern/logic/nodes.h"

namespace kern {
/// A Blueprint that is the object of an interface of the engine (Graph::implements; kern/docs/blueprint_cpp.md,
/// "Schnittstellen der Engine") names it as its last base - by the interface's C++ type, not by a text. Each function
/// the engine calls on it says so (KERN_OVERRIDE):
///   struct Water : kern::Blueprint, kern::Implements<kern::ShaderExtension> { ... KERN_OVERRIDE Text vertex() { ... } ... };
/// kern::logic::implementation(blueprint) makes the engine's object of a Blueprint's class (engine_nodes.gen.h).
template <class Interface> struct Implements {};
}  // namespace kern

namespace kern::logic::engine_direct {
/// The class a pointer to a member names (its own: `&B::f` of a function B has from a base names the base).
template <class M> struct MemberOf {};
template <class R, class C> struct MemberOf<R C::*> { using type = C; };
/// The adapter of an interface for a Blueprint's class (engine_nodes.gen.h writes one per interface: `template <class B>
/// using adapter = ...`).
template <class> inline constexpr bool kNoInterface = false;
template <class Interface> struct InterfaceOf {
  static_assert(kNoInterface<Interface>, "kern::Implements<I>: I is no interface of the engine (tools/engine_api.py writes one InterfaceOf per class of the engine with virtual functions that are nodes; kern/docs/blueprint_cpp.md, \"Schnittstellen der Engine\")");
};
template <class I> InterfaceOf<I> interface_of(const ::kern::Implements<I>&);
}  // namespace kern::logic::engine_direct

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
template <> struct Direct<Actor> {
  template <class C> static void from(EngineObjects&, ActorHandle b, C& c) {
    static_assert(std::is_same_v<C, ActorHandle>, "an Actor pin of something else than a kern::logic::ActorHandle");
    c = b;
  }
  template <class B> static B make(EngineObjects&, ActorHandle c) { return c; }
  static void pass(EngineObjects&, ActorHandle) {}
};
template <> struct Direct<Int> {
  template <class B, class C> static void from(EngineObjects&, const B& b, C& c) {
    static_assert(num::Integer<B>, "an Int pin the class holds as something else than a whole number (i64; an entry of a List<u8> ...: its own type)");
    static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number (an object of the engine is an Object<I>)");
    const i64 v = static_cast<i64>(b);
    if constexpr (is_count<C>) c = C(static_cast<u32>(v));
    else c = static_cast<C>(v);
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(num::Integer<B>, "an Int pin the class holds as something else than a whole number (i64; an entry of a List<u8> ...: its own type)");
    static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number (an object of the engine is an Object<I>)");
    if constexpr (is_count<C>) return static_cast<B>(static_cast<u32>(c));
    else return static_cast<B>(static_cast<i64>(c));
  }
  template <class C> static void pass(EngineObjects&, const C&) {}
};
/// An object of the engine (an Object<I> of the class: its ObjectRef) - a pointer to it, or one a value holds as a
/// value (the one it refers to, copied there). As Conv<Handle> of the door: one of another world, or of no object of
/// the class, is null, and the call fails (EngineObjects::error says why; the call asks before it calls the engine).
template <> struct Direct<Handle> {
  template <class B> static ObjectRef ref_of(const B& b) {
    if constexpr (std::is_same_v<B, ObjectRef>) return b;
    else return b.ref;
  }
  template <class B, class C> static void from(EngineObjects& o, const B& b, C& c) {
    if constexpr (std::is_pointer_v<C>) {
      c = object_in<std::remove_pointer_t<C>>(o, Value::object(ref_of(b)));
    } else {
      static_assert(std::is_class_v<C>, "an Object pin of something else than an object of the engine");
      if (const C* p = object_in<C>(o, Value::object(ref_of(b)))) c = *p;
      else c = blank<C>();
    }
  }
  template <class B, class C> static B make(EngineObjects& o, const C& c) {
    ObjectRef r;
    if constexpr (std::is_pointer_v<C>) r = o.ref(c ? o.adopt(*c) : 0);
    else r = o.ref(o.own(std::make_unique<C>(c)));
    if constexpr (std::is_same_v<B, ObjectRef>) return r;
    else return B(r);
  }
  template <class C> static void pass(EngineObjects& o, const C& c) {
    if constexpr (std::is_pointer_v<C>) {
      if (c) o.adopt(*c);
    } else {
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
  template <class C> static void from(EngineObjects&, u64 b, C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    c = static_cast<C>(b);
  }
  template <class B, class C> static B make(EngineObjects&, const C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    static_assert(std::is_same_v<B, u64>, "an Int64 pin the class holds as something else than a u64");
    return static_cast<u64>(c);
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

/// A Callback of the class as the engine takes it: the thunk and its place as the function pointer and its void* -
/// the same two words (kern::HeightSampler: fn, user) -, or a std::function that calls them (it holds the two words:
/// no memory is asked for). Unbound: the engine's nothing.
template <class R, class... A> struct Direct<Callback<R(A...)>> {
  template <class B, class C> static void from(EngineObjects&, const B& b, C& c) {
    if constexpr (callback_detail::is_function<C>) {
      if (b.fn) c = [b](auto... a) { return b(a...); };
      else c = nullptr;
    } else {
      auto& [fn, user] = c;
      static_assert(std::is_same_v<std::remove_cvref_t<decltype(fn)>, decltype(b.fn)>, "a Callback whose function the engine calls with other types than the class's");
      fn = b.fn;
      user = b.place;
    }
  }
  /// (a Callback the engine gives: unbound - the class calls none)
  template <class B, class C> static B make(EngineObjects&, const C&) { return B{}; }
  template <class C> static void pass(EngineObjects&, const C&) {}
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
/// An object of the engine a function gave, as the Object<I> the class holds: its handle in these objects.
template <class B> B object_as(EngineObjects& o, i64 handle) { return B(o.ref(handle)); }

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


// ---- places of the engine (kern/docs/blueprint_cpp.md, "Plätze der Engine") ----
// A function of the engine that gives a reference to something its object holds and only looks it up
// (`std::vector<ShotEvent>& shots() { return shots_; }`) is a place: its node binds it once - in the class
// `auto& r = world->shots();` - and the nodes that name a place work on it there. Place<P, B> is the node's
// output: the handle, and the address of what the engine holds (P::C) while the object of the handle is the
// one it was (EngineObjects::generation) - else it is looked up again, as the door looks it up every time; a
// handle of no object tells the host "<function>@get" as the door does, reads the Blueprint's defaults and
// writes into an entry of its own. Without the engine's objects (a test with a stand-in) each naming of the
// place reads it through the door ("<function>@get") into a copy of its own, and each change sets it back
// ("<function>@set") - what the interpreter does.
// What a place names is a Ref: the engine's C++ value there (`kern_at`), or an entry of its own of the
// Blueprint's type (`kern_own`: beside a list, where the engine holds nothing, the copy of the door). Reading
// it makes the Blueprint's value of the engine's (Direct<Tag>::make), a change puts it there (Direct::from):
// a 32-bit number of the engine is a Float only where it is read, as C++ reads the f32 field.

template <class T> struct ListTag : std::false_type {};
template <class T> struct ListTag<List<T>> : std::true_type { using item = T; };

/// A copy of a place read through the door: set back through it after every change.
struct PlaceDoor {
  virtual ~PlaceDoor() = default;
  virtual void wrote() = 0;
};

template <class Tag, class C, class B> struct RefCore {
  using kern_tag = Tag;
  C* kern_at = nullptr;
  B* kern_own = nullptr;
  EngineObjects* kern_o = nullptr;
  std::shared_ptr<PlaceDoor> kern_door;
  std::shared_ptr<void> kern_keep;  // (the entry of its own: as long as the place is named)

  B kern_read() const { return kern_own ? *kern_own : Direct<Tag>::template make<B>(*kern_o, *kern_at); }
  void kern_write(const B& b) const {
    if (kern_own) *kern_own = b;
    else Direct<Tag>::from(*kern_o, b, *kern_at);
    if (kern_door) kern_door->wrote();
  }
  /// The same place one step on: a field (its member of C and of B), or an entry of a list.
  template <class T, class CF, class BF, class CC = C, class BB = B> RefCore<T, CF, BF> kern_step(CF CC::*c, BF BB::*b) const {
    return {kern_own ? nullptr : &(kern_at->*c), kern_own ? &(kern_own->*b) : nullptr, kern_o, kern_door, kern_keep};
  }
  std::size_t kern_count() const { return kern_own ? kern_own->size() : kern_at->size(); }
};

template <class Tag, class C, class B> struct Ref;

/// A place's fields by their names (a struct of the engine: engine_nodes.gen.h writes S::Fields).
template <class D, class Tag, class C, class B> struct FieldsOf {
  explicit FieldsOf(D*) {}
};
template <class D, class S, class C, class B> struct FieldsOf<D, Struct<S>, C, B> : S::template Fields<D, C, B> {
  explicit FieldsOf(D* d) : S::template Fields<D, C, B>(d) {}
};

/// What a place is named for: read (the Blueprint's value), set, added to, its entries and its length -
/// each on what stands there now (D::ref()).
template <class D, class Tag, class C, class B> struct PlaceOps {
  using kern_place_value = B;   // (what stands there, read - kern::to reads it first)
  operator B() const { return self().ref().kern_read(); }
  /// A Set: what is set (the Blueprint's value, as the interpreter's Set gives it).
  template <class X> B operator=(const X& x) const {
    B b(x);
    self().ref().kern_write(b);
    return b;
  }
  template <class X> B operator+=(const X& x) const {
    const auto r = self().ref();
    B b = r.kern_read();
    b += x;
    r.kern_write(b);
    return b;
  }
  template <class X> B operator-=(const X& x) const {
    const auto r = self().ref();
    B b = r.kern_read();
    b -= x;
    r.kern_write(b);
    return b;
  }
  /// An entry (a list): beside the list an entry of its own with the defaults, of which nothing is kept.
  template <class I>
    requires ListTag<Tag>::value
  auto operator[](const I& index) const {
    using T = typename ListTag<Tag>::item;
    using CI = typename C::value_type;
    using BI = typename B::value_type;
    const RefCore<Tag, C, B> r = self().ref();
    const i64 i = static_cast<i64>(index);
    if (i >= 0 && static_cast<std::size_t>(i) < r.kern_count())
      return Ref<T, CI, BI>(RefCore<T, CI, BI>{r.kern_own ? nullptr : &(*r.kern_at)[static_cast<std::size_t>(i)], r.kern_own ? &(*r.kern_own)[static_cast<std::size_t>(i)] : nullptr, r.kern_o, r.kern_door, r.kern_keep});
    auto own = std::make_shared<BI>();
    BI* at = own.get();
    return Ref<T, CI, BI>(RefCore<T, CI, BI>{nullptr, at, r.kern_o, r.kern_door, std::move(own)});
  }

 private:
  const D& self() const { return static_cast<const D&>(*this); }
};

/// What a place names, as it is named once (an expression of the class).
template <class Tag, class C, class B> struct Ref : RefCore<Tag, C, B>, PlaceOps<Ref<Tag, C, B>, Tag, C, B>, FieldsOf<Ref<Tag, C, B>, Tag, C, B> {
  explicit Ref(RefCore<Tag, C, B> r) : RefCore<Tag, C, B>(std::move(r)), FieldsOf<Ref, Tag, C, B>(this) {}
  Ref(const Ref& r) : RefCore<Tag, C, B>(r), FieldsOf<Ref, Tag, C, B>(this) {}
  using PlaceOps<Ref, Tag, C, B>::operator=;
  B operator=(const Ref& r) const { return PlaceOps<Ref, Tag, C, B>::operator=(r.kern_read()); }
  RefCore<Tag, C, B> ref() const { return *this; }
};

/// A field of a place, by its name: the same place one step on, looked at when it is named.
template <class D, class Tag, class C, class B, class CF, class BF, CF C::*Cm, BF B::*Bm>
struct Field : PlaceOps<Field<D, Tag, C, B, CF, BF, Cm, Bm>, Tag, CF, BF>, FieldsOf<Field<D, Tag, C, B, CF, BF, Cm, Bm>, Tag, CF, BF> {
  explicit Field(D* d) : FieldsOf<Field, Tag, CF, BF>(this), kern_parent(d) {}
  /// (handed on by value - an argument of kern::formula: the same field of the same place, its fields its own)
  Field(const Field& f) : FieldsOf<Field, Tag, CF, BF>(this), kern_parent(f.kern_parent) {}
  using PlaceOps<Field, Tag, CF, BF>::operator=;
  BF operator=(const Field& f) const { return PlaceOps<Field, Tag, CF, BF>::operator=(BF(f)); }
  RefCore<Tag, CF, BF> ref() const { return kern_parent->ref().template kern_step<Tag>(Cm, Bm); }

 private:
  D* kern_parent;
};

/// The output of a node of the engine that binds a place of the engine (P: engine_nodes.gen.h - the object's
/// class, C, Tag, the input of the handle, In - the function's further inputs as C++ has them: the index of an
/// entry of an array the object holds, or none -, at(objects, object, in) - null where `in` names no entry: it
/// says why in objects.error - and the names "<function>@get", "<function>@set").
template <class P, class B> struct Place : PlaceOps<Place<P, B>, typename P::Tag, typename P::C, B>, FieldsOf<Place<P, B>, typename P::Tag, typename P::C, B> {
  using Tag = typename P::Tag;
  using C = typename P::C;
  Place() : FieldsOf<Place, Tag, C, B>(this) {}
  Place(const Place& p)
      : FieldsOf<Place, Tag, C, B>(this), kern_world(p.kern_world), kern_handle(p.kern_handle), kern_in(p.kern_in), kern_door_in(p.kern_door_in),
        kern_at(p.kern_at), kern_generation(p.kern_generation) {}
  Place& operator=(const Place& p) {
    kern_world = p.kern_world, kern_handle = p.kern_handle, kern_in = p.kern_in, kern_door_in = p.kern_door_in, kern_at = p.kern_at,
    kern_generation = p.kern_generation;
    return *this;
  }
  using PlaceOps<Place, Tag, C, B>::operator=;

  /// The node ran: the place of the object `handle` refers to (what the engine holds there: `at`, null where the
  /// class has not the engine's objects or the reference names no object).
  void kern_bind(const BlueprintWorld& world, ObjectRef handle, const typename P::In& in, C* at) {
    kern_world = &world;
    kern_handle = handle;
    kern_in = in;
    kern_at = at;
    kern_generation = at && world.engine ? world.engine->generation() : 0;
  }
  /// The same, by a class without the engine's objects: what the door is given for it (the handle and the
  /// further inputs, as the node was given them).
  void kern_bind(const BlueprintWorld& world, std::vector<Value> door_in) {
    kern_world = &world;
    kern_handle = door_in.empty() ? ObjectRef{} : door_in[0].as_object();
    kern_door_in = std::move(door_in);
    kern_at = nullptr;
    kern_generation = 0;
  }
  /// What stands there now.
  RefCore<Tag, C, B> ref() const {
    RefCore<Tag, C, B> r;
    Host* const host = kern_world ? kern_world->host : nullptr;
    if (EngineObjects* const o = kern_world ? kern_world->engine : nullptr) {
      r.kern_o = o;
      if (!kern_at || kern_generation != o->generation()) {
        // (the object of the handle may not be the one it was: looked up again, as the door does)
        o->begin_call();
        typename P::Object* const self = object_of<typename P::Object>(*o, kern_handle, P::pin);
        kern_at = self ? P::at(*o, *self, kern_in) : nullptr;
        kern_generation = o->generation();
        if (!kern_at) {
          if (host) host->engine_failed(P::get);
          return none(r);
        }
      }
      r.kern_at = kern_at;
      return r;
    }
    struct Door final : PlaceDoor {
      Host* host = nullptr;
      std::vector<Value> in;
      B b{};
      void wrote() override {
        std::vector<Value> none, given = in;
        given.push_back(to_value(b));
        host->engine(P::set, given, none);
      }
    };
    std::vector<Value> in = kern_door_in.empty() ? std::vector<Value>{Value::object(kern_handle)} : kern_door_in;
    std::vector<Value> out{Value::zero(pin_type_of<B>())};
    if (!host || !host->engine(P::get, in, out)) return none(r);
    auto door = std::make_shared<Door>();
    door->host = host;
    door->in = std::move(in);
    door->b = from_value<B>(out[0].converted(pin_type_of<B>()));
    r.kern_own = &door->b;
    r.kern_door = door;
    return r;
  }

 private:
  static RefCore<Tag, C, B> none(RefCore<Tag, C, B>& r) {
    auto own = std::make_shared<B>();
    r.kern_at = nullptr;
    r.kern_own = own.get();
    r.kern_keep = std::move(own);
    return r;
  }
  const BlueprintWorld* kern_world = nullptr;
  ObjectRef kern_handle;
  typename P::In kern_in{};
  std::vector<Value> kern_door_in;   // (only without the engine's objects - nothing is asked for it else)
  mutable C* kern_at = nullptr;
  mutable u64 kern_generation = 0;
};


/// Something that names a place of the engine (a Place, a Ref, a Field of one).
template <class X> concept NamesPlace = requires(const X& x) { x.ref().kern_count(); };

/// What an input of a node of the engine is given (engine_nodes.gen.h): where it names a place of the engine
/// (`kern::weld(c.mesh)` on an entry of `b.chunks()`), what stands there now, read - as the interpreter reads it
/// into the wire of the input; any other value as it is.
template <class X> decltype(auto) input_value(const X& x) {
  if constexpr (NamesPlace<X>) return x.ref().kern_read();
  else return (x);
}

/// The steps of a list at a place of the engine: on the engine's list itself (an item made into the engine's
/// type first, Direct::from), or on the entry of its own where the place has one - what logic::list does on a
/// Blueprint's list; through the door the list is set back after.
template <class P, class I> i64 list_add(const P& p, const I& item) {
  const auto r = p.ref();
  using R = decltype(r);
  using BI = typename std::remove_pointer_t<decltype(R::kern_own)>::value_type;
  i64 at;
  if (r.kern_own) {
    at = logic::list::add(*r.kern_own, BI(item));
  } else {
    using CI = typename std::remove_pointer_t<decltype(R::kern_at)>::value_type;
    auto c = blank<CI>();
    Direct<typename ListTag<typename R::kern_tag>::item>::from(*r.kern_o, BI(item), c);
    at = logic::list::add(*r.kern_at, std::move(c));
  }
  if (r.kern_door) r.kern_door->wrote();
  return at;
}
template <class P, class I> void list_insert(const P& p, i64 index, const I& item) {
  const auto r = p.ref();
  using R = decltype(r);
  using BI = typename std::remove_pointer_t<decltype(R::kern_own)>::value_type;
  if (r.kern_own) {
    logic::list::insert(*r.kern_own, index, BI(item));
  } else {
    using CI = typename std::remove_pointer_t<decltype(R::kern_at)>::value_type;
    auto c = blank<CI>();
    Direct<typename ListTag<typename R::kern_tag>::item>::from(*r.kern_o, BI(item), c);
    logic::list::insert(*r.kern_at, index, std::move(c));
  }
  if (r.kern_door) r.kern_door->wrote();
}
template <class P> void list_remove_at(const P& p, i64 index) {
  const auto r = p.ref();
  if (r.kern_own) logic::list::remove_at(*r.kern_own, index);
  else logic::list::remove_at(*r.kern_at, index);
  if (r.kern_door) r.kern_door->wrote();
}
/// (an item equals one of the engine's where the Blueprint's value of that one equals it - as the interpreter
/// compares them)
template <class P, class I> bool list_remove(const P& p, const I& item) {
  const auto r = p.ref();
  using R = decltype(r);
  using BI = typename std::remove_pointer_t<decltype(R::kern_own)>::value_type;
  bool removed;
  if (r.kern_own) {
    removed = logic::list::remove(*r.kern_own, BI(item));
  } else {
    const BI b(item);
    auto& items = *r.kern_at;
    std::size_t kept = 0;
    for (std::size_t k = 0; k < items.size(); ++k) {
      if (Direct<typename ListTag<typename R::kern_tag>::item>::template make<BI>(*r.kern_o, items[k]) == b) continue;
      if (kept != k) items[kept] = std::move(items[k]);
      ++kept;
    }
    removed = kept != items.size();
    items.resize(kept);
  }
  if (r.kern_door) r.kern_door->wrote();
  return removed;
}
template <class P> void list_clear(const P& p) {
  const auto r = p.ref();
  if (r.kern_own) r.kern_own->clear();
  else r.kern_at->clear();
  if (r.kern_door) r.kern_door->wrote();
}
template <class P, class I> void list_resize(const P& p, i64 length, const I& value) {
  const auto r = p.ref();
  using R = decltype(r);
  using BI = typename std::remove_pointer_t<decltype(R::kern_own)>::value_type;
  if (r.kern_own) {
    logic::list::resize(*r.kern_own, length, BI(value));
  } else {
    using CI = typename std::remove_pointer_t<decltype(R::kern_at)>::value_type;
    auto c = blank<CI>();
    Direct<typename ListTag<typename R::kern_tag>::item>::from(*r.kern_o, BI(value), c);
    logic::list::resize(*r.kern_at, length, c);
  }
  if (r.kern_door) r.kern_door->wrote();
}
template <class P, class BI> void list_append(const P& p, logic::Items<BI> items) {
  const auto r = p.ref();
  using R = decltype(r);
  if (r.kern_own) {
    logic::list::insert_items(*r.kern_own, static_cast<i64>(r.kern_own->size()), items.first, items.count);
  } else {
    using CI = typename std::remove_pointer_t<decltype(R::kern_at)>::value_type;
    // (at the end, as insert of a range puts them there - the library grows the list as it grows it for the
    // insert of the source and for a Blueprint's list, logic::list::insert_items -, then each item made into the
    // engine's where it stands)
    auto& list = *r.kern_at;
    const std::size_t at = list.size();
    list.insert(list.end(), items.count, blank<CI>());
    for (std::size_t k = 0; k < items.count; ++k) Direct<typename ListTag<typename R::kern_tag>::item>::from(*r.kern_o, items.first[k], list[at + k]);
  }
  if (r.kern_door) r.kern_door->wrote();
}
/// Take: what is there, and the Blueprint's zero of its type there.
template <class P> auto take(const P& p) {
  const auto r = p.ref();
  using B = std::remove_pointer_t<decltype(decltype(r)::kern_own)>;
  B was = r.kern_read();
  r.kern_write(B{});
  return was;
}

}  // namespace kern::logic::engine_values
