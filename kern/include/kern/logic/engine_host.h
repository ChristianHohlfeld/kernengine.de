// Kern Logic: the engine's nodes on the engine itself.
//
// A node of the engine (kern/logic/engine_api.h) reaches the engine through logic::Host::engine. This
// is what a host gives it there: the real function, called with the node's inputs and giving back
// what it gives - for the interpreter and the JIT, and for a Blueprint's class whose host keeps the
// engine's objects to itself. A class whose host gives it these objects (Host::engine_objects) calls
// the same function on them itself, typed (kern/logic/engine_direct.h): the same objects behind the
// same handles, the same result.
//
//   - `EngineObjects` are the objects of the engine a Blueprint holds handles of (a handle is a whole
//     number, 0 is none): those it made (<class>.make - owned here until <class>.release, or until
//     the objects end), those a function gave it by value (owned here the same way), and those of
//     the host's own a function gave it by reference or pointer, or the host put in (`adopt`: the
//     same object always has the same handle; their life is the host's). Handles are counted from 1
//     in the order the objects came and are never given twice.
//   - `call_engine(objects, function, in, out)` calls the function of a node: every input made into
//     what the C++ function takes, every output from what it gives or wrote. Each component of the
//     engine brings the calls of its own headers (written by tools/engine_api.py from the headers:
//     src/<component>/engine_calls.gen.cpp) - a build has the functions of the components it links.
//   - What a function writes through a pointer with a count beside it (EnginePin::count) gets the
//     room the count says, EnginePin::per numbers for each it counts - and no more: the call checks
//     that the engine wrote nothing past it (else it fails: the table does not say what the engine
//     does).
//
// A call fails (false, with `EngineObjects::error` saying why) where its inputs name no object of
// the class the function takes, or where the engine wrote past the room the table gives it.
#pragma once

#include <array>
#include <bit>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "kern/logic/engine_api.h"
#include "kern/logic/logic.h"
#include "kern/math/math.h"

namespace kern::logic {

class EngineObjects {
 public:
  /// The class of an object (no RTTI: the address of a variable of each class).
  using Kind = const void*;
  template <class T> static Kind kind() {
    static const char tag = 0;
    return &tag;
  }
  /// The same object as each of the classes it derives from that are objects of the engine too
  /// (a HostTransport is a Transport): its handle names it as either.
  using Kinds = std::vector<std::pair<Kind, void*>>;
  template <class T, class... Bases> static Kinds kinds_of(T* at) {
    return Kinds{{kind<T>(), static_cast<void*>(at)}, {kind<Bases>(), static_cast<void*>(static_cast<Bases*>(at))}...};
  }

  EngineObjects() = default;
  EngineObjects(const EngineObjects&) = delete;
  EngineObjects& operator=(const EngineObjects&) = delete;
  /// What it owns ends in the reverse order it came.
  ~EngineObjects() { clear(); }

  /// The handle of an object the host keeps itself: the same object (of the same class), the same handle.
  template <class T, class... Bases> i64 adopt(const T& object) {
    return put(kinds_of<T, Bases...>(const_cast<T*>(&object)), nullptr);
  }
  /// A handle of an object this keeps from now on (made by a node, or given by value).
  template <class T, class... Bases> i64 own(std::unique_ptr<T> object) {
    if (!object) return 0;
    T* at = object.release();
    return put(kinds_of<T, Bases...>(at), [](void* p) { delete static_cast<T*>(p); });
  }
  /// The object of a handle as a T (null: 0, an unknown handle, or one of another class - see `missing`).
  template <class T> T* get(i64 handle) {
    if (void* p = find(handle, kind<std::remove_const_t<T>>())) return static_cast<T*>(p);
    if (handle != 0 && missing_)
      if (void* p = missing_(*this, handle, kind<std::remove_const_t<T>>())) {
        if (!calling_on_) calling_on_ = handle;   // (made for this call: the object the call is on, as one found)
        return static_cast<T*>(p);
      }
    return nullptr;
  }
  /// Ends an object of a handle this owns (its destructor): true if it did. A handle the host keeps
  /// itself is not this one's to end. (Where a handle it has no object of names one made for it -
  /// see `missing` - that is the object ended: made as there, and ended.)
  template <class T> bool release(i64 handle) {
    if (end(handle, kind<T>())) return true;
    if (handle == 0 || !missing_ || find(handle, kind<T>())) return false;
    return missing_(*this, handle, kind<T>()) && end(handle, kind<T>());
  }
  /// Ends everything this owns and forgets every handle (the next one is 1 again).
  void clear();
  /// How many objects a handle names now.
  std::size_t size() const;

  /// What a host does with a handle it has no object of (null: nothing - the call fails). A test
  /// makes one there, the same way on every side it compares (`put_at`; engine_default_maker makes
  /// one of a class the way C++ makes it without arguments).
  using Missing = void* (*)(EngineObjects&, i64 handle, Kind want);
  void on_missing(Missing m) { missing_ = m; }
  /// An object made for a handle the host had none of (see on_missing): this ends it. (Where the handle names
  /// an object of another class, both stay - each of them ends with these objects.)
  template <class T, class... Bases> T* put_at(i64 handle, std::unique_ptr<T> object) {
    T* at = object.release();
    place(handle, kinds_of<T, Bases...>(at), [](void* p) { delete static_cast<T*>(p); });
    return at;
  }
  /// ... made together with the objects of the engine its constructor takes (MadeWith - they have no
  /// handle): this ends them all, the object first.
  template <class T, class... Bases, class H> T* put_made(i64 handle, std::unique_ptr<H> made) {
    T* at = &*made->object;
    H* owner = made.release();
    place(handle, kinds_of<T, Bases...>(at), [](void* p) { delete static_cast<H*>(p); }, owner);
    return at;
  }

  /// A text that stays as long as these objects - where the engine holds a text as `const char*` (a field of
  /// a struct it is given): the same text, the same characters.
  const char* text(const std::string& s) { return texts_.insert(s).first->c_str(); }

  /// Room for what a call is given that the engine sees through a view (a Span field of a struct): kept
  /// until the next call (call_engine lets it go before each).
  template <class X> std::vector<X>& held() {
    auto p = std::make_shared<std::vector<X>>();
    std::vector<X>& r = *p;
    held_.push_back(std::move(p));
    return r;
  }
  void let_go() { held_.clear(); }

  /// A Callback a call hands the engine (Conv of a Callback): what the C++ value the engine gets points at - these
  /// objects and the Callable it calls. It stays for the call; given to a method of an object of the engine (the
  /// call's first input is its handle), with that object - the last one each such call handed it -, until the
  /// object ends (call_engine: kept_by).
  struct Handed {
    EngineObjects* objects = nullptr;
    std::shared_ptr<const Callable> call;
  };
  const Handed* hand(std::shared_ptr<const Callable> call) {
    calling_.push_back(std::make_shared<Handed>(Handed{this, std::move(call)}));
    return calling_.back().get();
  }
  /// The Callbacks the call that just ran handed the engine stay with the object `handle` (the n-th of `function`
  /// in place of the one before it).
  void kept_by(i64 handle, std::string_view function) {
    if (calling_.empty()) return;
    if (!entries_.count(handle)) return;  // (no object: they stay for the call only)
    auto& mine = kept_[handle];
    for (std::size_t k = 0; k < calling_.size(); ++k) mine[std::string(function) + "#" + std::to_string(k)] = calling_[k];
    calling_.clear();
  }
  /// A call begins: nothing has failed yet, and what the call before was given to look at is let go.
  void begin_call() {
    error.clear();
    calling_.clear();
    calling_on_ = 0;
    let_go();
  }

  /// Why the last call failed.
  std::string error;
  /// How often a handle has stopped naming the object it named (release, clear, an object put at a handle):
  /// what a Blueprint's class holds of an object - a place of the engine (engine_values::Place) - is still
  /// that object while this is the same, and is looked up again by its handle when it is not.
  u64 generation() const { return generation_; }

 private:
  struct Entry {
    void* at = nullptr;
    void (*end)(void*) = nullptr;  // (null: the host's own)
    void* owner = nullptr;          // what `end` ends, if not the object itself: what holds it (MadeWith)
    Kinds kinds;  // its class first, then those it derives from
  };
  i64 put(Kinds kinds, void (*end)(void*));
  void place(i64 handle, Kinds kinds, void (*end)(void*), void* owner = nullptr);
  void forget_lent(i64 lender);
  void* find(i64 handle, Kind k) const;
  bool end(i64 handle, Kind k);

  // (a handle names one object - of a class and those it derives from; only where a test makes one for a handle
  // it had none of as that class (on_missing) does the same number name an object of another class beside it,
  // as the test's stand-in keeps its objects by class and number)
  std::unordered_map<i64, std::vector<Entry>> entries_;
  std::vector<std::pair<i64, void*>> order_;  // (the objects in the order they came: handle and where)
  std::unordered_map<const void*, std::vector<std::pair<Kind, i64>>> by_object_;
  // What an object lent: a call on an object this owns that hands out one of the host's own - a reference or a
  // pointer the object gave: its world, its level - names something that is there while that object is. When
  // the object ends, the handles of what it lent are forgotten with it (end): a handle of what is gone names
  // nothing - and not the place where it was. (By the handle of the object that lent: handle and where.
  // `calling_on_`: the first object the call being made asked for - the one it is a call on.)
  std::unordered_map<i64, std::vector<std::pair<i64, void*>>> lent_;
  mutable i64 calling_on_ = 0;
  i64 next_ = 1;
  u64 generation_ = 0;
  Missing missing_ = nullptr;
  std::unordered_set<std::string> texts_;
  std::vector<std::shared_ptr<void>> held_;
  std::vector<std::shared_ptr<Handed>> calling_;  // (the Callbacks of the call being made)
  std::unordered_map<i64, std::unordered_map<std::string, std::shared_ptr<Handed>>> kept_;  // (with their object)
};

/// The call of one node of the engine (written by tools/engine_api.py).
using EngineCallFn = bool (*)(EngineObjects&, const std::vector<Value>& in, std::vector<Value>& out);
struct EngineCall {
  const char* type;  // the node: "engine.mixer.render"
  EngineCallFn call;
};
/// The calls of a component's headers: a variable of this in its engine_calls.gen.cpp puts them in.
struct EngineCalls {
  EngineCalls(const EngineCall* calls, std::size_t count);
  const EngineCall* calls;
  std::size_t count;
  const EngineCalls* next;
};
/// How a component makes an object of one of its classes as C++ makes it without arguments, at a
/// handle (written by tools/engine_api.py for each class whose make node needs no input).
struct EngineMaker {
  EngineObjects::Kind kind;
  void* (*make)(EngineObjects&, i64 handle);
};
struct EngineMakers {
  EngineMakers(const EngineMaker* makers, std::size_t count);
  const EngineMaker* makers;
  std::size_t count;
  const EngineMakers* next;
};
/// An object of the class `want` made as C++ makes it without arguments, at `handle` - what a test's
/// host does with a handle it has no object of (EngineObjects::on_missing): the same object on every
/// side it compares. Null: this build makes none of that class so.
void* engine_make_default(EngineObjects& objects, i64 handle, EngineObjects::Kind want);
/// The call of a node in this build, or null (its component is not linked, or no such node).
EngineCallFn engine_call(std::string_view function);
/// Calls the engine's function of a node: `in` the node's inputs, `out` its outputs (given as their
/// zeros). False: no such function in this build, or the call failed (objects.error says why).
bool call_engine(EngineObjects& objects, std::string_view function, const std::vector<Value>& in, std::vector<Value>& out);

// ---- values: a pin's value as the C++ function takes it, and back ----
// The tool writes, per pin, the Blueprint type it is (a tag below) and lets C++ name the C++ type: a
// u64 that is an Int64 and a size_t that is an Int are the same type in C++, the pin says which.
namespace engine_values {

struct Bool {};
struct Int {};
struct Float {};
struct Vector {};
struct Text {};
struct Int64 {};
/// An enum: E has `using type = <the enum>` and `values[]`, its values in the order of the Blueprint's enum.
template <class E> struct Enum {};
/// A struct: S has `to(objects, value)` and `from(objects, Value, value&)`, field by field.
template <class S> struct Struct {};
template <class T> struct List {};
/// A list inside a list: in the Blueprint a struct whose one field is the inner list.
template <class L> struct Row {};
/// std::pair: a struct of `first` and `second`.
template <class A, class B> struct Pair {};
/// An object of the engine: its handle.
struct Handle {};
/// A Callback (Callback<Float(Float, Float)>: the tags of its result and its inputs) - in C++ a struct of a pointer
/// to a function with a void* in front and that void* (kern::HeightSampler), or a std::function.
template <class S> struct Callback {};

template <class Tag> struct Conv;

/// A value of a C++ type before the pin's value is put in it: as C++ makes it without arguments - or, of
/// plain data that has only constructors with arguments (Rng(u32 seed)), all of its bytes zero, as the
/// Blueprint's struct is before its fields are set (they are all set from the pin).
template <class T> T blank() {
  if constexpr (std::is_default_constructible_v<T>) {
    return T{};
  } else {
    static_assert(std::is_trivially_copyable_v<T>, "a value the engine takes that is made only by a constructor with arguments, and is no plain data");
    struct Bytes {
      unsigned char b[sizeof(T)] = {};
    };
    return std::bit_cast<T>(Bytes{});
  }
}

/// An object of the engine made for a handle a host has none of (EngineObjects::on_missing), whose
/// constructor takes other objects of the engine (`Client(Transport&, ...)`): those are made with it, the
/// way each is made without a handle, and kept here - they end after it, in the reverse order they came.
template <class T> struct MadeWith {
  std::vector<std::pair<void*, void (*)(void*)>> kept;
  std::optional<T> object;
  MadeWith() = default;
  MadeWith(const MadeWith&) = delete;
  MadeWith& operator=(const MadeWith&) = delete;
  ~MadeWith() {
    object.reset();
    for (auto it = kept.rbegin(); it != kept.rend(); ++it) it->second(it->first);
  }
  template <class D> D& keep(D* d) {
    kept.emplace_back(d, [](void* p) { delete static_cast<D*>(p); });
    return *d;
  }
};

template <class C> inline constexpr bool is_count = false;
template <u32 Per> inline constexpr bool is_count<Count<Per>> = true;

template <> struct Conv<Bool> {
  template <class C> static void from(EngineObjects&, const Value& v, C& c) {
    static_assert(std::is_same_v<C, bool>, "a Bool pin of something else than a bool");
    c = v.as_bool();
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<C, bool>, "a Bool pin of something else than a bool");
    return Value::boolean(c);
  }
};
template <> struct Conv<Handle>;
/// (A whole number - or, in a list, a pointer to an object of the engine: its handle.)
template <> struct Conv<Int> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c);
  template <class C> static Value to(EngineObjects& o, const C& c);
};
template <> struct Conv<Float> {
  template <class C> static void from(EngineObjects&, const Value& v, C& c) {
    static_assert(std::is_floating_point_v<C>, "a Float pin of something else than a number");
    c = static_cast<C>(v.as_number());
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    static_assert(std::is_floating_point_v<C>, "a Float pin of something else than a number");
    return Value::number(static_cast<f64>(c));
  }
};
template <> struct Conv<Vector> {
  template <class C> static void from(EngineObjects&, const Value& v, C& c) {
    static_assert(std::is_same_v<C, Vec3> || std::is_same_v<C, Vec2>, "a Vector pin of something else than a vector");
    const Vec3 x = v.as_vector();
    if constexpr (std::is_same_v<C, Vec3>) c = x;
    else c = Vec2(x.x, x.y);
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<C, Vec3> || std::is_same_v<C, Vec2>, "a Vector pin of something else than a vector");
    if constexpr (std::is_same_v<C, Vec3>) return Value::vector(c);
    else return Value::vector(Vec3(c.x, c.y, 0));
  }
};
template <> struct Conv<Text> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    static_assert(std::is_same_v<C, std::string> || std::is_same_v<C, const char*>, "a Text pin held as something else than a std::string");
    if constexpr (std::is_same_v<C, const char*>) c = o.text(v.as_text());   // (a field that points at a text: one that stays)
    else c = v.as_text();
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    if constexpr (std::is_pointer_v<C>) return Value::text(c ? std::string(c) : std::string());
    else return Value::text(std::string(c));
  }
};
template <> struct Conv<Int64> {
  template <class C> static void from(EngineObjects&, const Value& v, C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    c = static_cast<C>(static_cast<u64>(v.i));
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    static_assert(std::is_integral_v<C> && sizeof(C) == 8, "an Int64 pin of something else than 64 bits");
    return Value::int64(static_cast<u64>(c));
  }
};
template <class E> struct Conv<Enum<E>> {
  static constexpr std::size_t count = sizeof(E::values) / sizeof(E::values[0]);
  template <class C> static void from(EngineObjects&, const Value& v, C& c) {
    static_assert(std::is_same_v<C, typename E::type>, "an enum pin of another enum");
    const i64 k = v.as_int();
    c = E::values[k >= 0 && static_cast<u64>(k) < count ? static_cast<std::size_t>(k) : 0];
  }
  template <class C> static Value to(EngineObjects&, const C& c) {
    static_assert(std::is_same_v<C, typename E::type>, "an enum pin of another enum");
    for (std::size_t k = 0; k < count; ++k)
      if (E::values[k] == c) return Value::enumeration(static_cast<i64>(k));
    return Value::enumeration(0);
  }
};
template <class S> struct Conv<Struct<S>> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) { S::from(o, v, c); }
  template <class C> static Value to(EngineObjects& o, const C& c) { return S::to(o, c); }
};
template <class A, class B> struct Conv<Pair<A, B>> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    if (const Value* f = v.field(0)) Conv<A>::from(o, *f, c.first);
    if (const Value* f = v.field(1)) Conv<B>::from(o, *f, c.second);
  }
  template <class C> static Value to(EngineObjects& o, const C& c) { return Value::structure({Conv<A>::to(o, c.first), Conv<B>::to(o, c.second)}); }
};
template <> struct Conv<Handle> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    static_assert(std::is_pointer_v<C>, "a handle in a list or a struct is a pointer to the object");
    c = o.get<std::remove_pointer_t<C>>(v.as_int());
  }
  template <class C> static Value to(EngineObjects& o, const C& c) {
    static_assert(std::is_pointer_v<C>, "a handle in a list or a struct is a pointer to the object");
    return Value::integer(c ? o.adopt(*c) : 0);
  }
};

template <class C> void Conv<Int>::from(EngineObjects& o, const Value& v, C& c) {
  if constexpr (std::is_pointer_v<C>) {
    Conv<Handle>::from(o, v, c);
  } else if constexpr (std::is_class_v<C> && !is_count<C>) {
    // (an object of the engine a value holds as a value - a TextureAsset in a Level's list: the one of the
    // handle, copied there; none - the handle 0 - is one as C++ makes it)
    if (const C* p = o.get<C>(v.as_int())) c = *p;
    else c = blank<C>();
  } else {
    static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number");
    if constexpr (is_count<C>) c = C(static_cast<u32>(v.as_int()));
    else c = static_cast<C>(v.as_int());
  }
}
template <class C> Value Conv<Int>::to(EngineObjects& o, const C& c) {
  if constexpr (std::is_pointer_v<C>) {
    return Conv<Handle>::to(o, c);
  } else if constexpr (std::is_class_v<C> && !is_count<C>) {
    return Value::integer(o.own(std::make_unique<C>(c)));   // (... given out: a copy of its own, a handle of its own)
  } else {
    static_assert((std::is_integral_v<C> && !std::is_same_v<C, bool>) || std::is_enum_v<C> || is_count<C>, "an Int pin of something else than a whole number");
    if constexpr (is_count<C>) return Value::integer(static_cast<i64>(static_cast<u32>(c)));
    else return Value::integer(static_cast<i64>(c));
  }
}

/// How a list of the Blueprint type T holds its items (Value::of / items).
template <class T> struct Held { using type = Value; };   // (structs, rows, pairs: a Value each)
template <> struct Held<Bool> { using type = bool; };
template <> struct Held<Int> { using type = i64; };
template <> struct Held<Handle> { using type = i64; };
template <> struct Held<Float> { using type = f64; };
template <> struct Held<Vector> { using type = Vec3; };
template <> struct Held<Text> { using type = std::string; };
template <> struct Held<Int64> { using type = logic::Int64; };
template <class E> struct Held<Enum<E>> { using type = i64; };

/// One item of a list of T as the list holds it, and back.
template <class T, class C> typename Held<T>::type held_of(EngineObjects& o, const C& c) {
  const Value v = Conv<T>::to(o, c);
  using H = typename Held<T>::type;
  if constexpr (std::is_same_v<H, Value>) return v;
  else if constexpr (std::is_same_v<H, bool>) return v.b;
  else if constexpr (std::is_same_v<H, i64>) return v.i;
  else if constexpr (std::is_same_v<H, f64>) return v.n;
  else if constexpr (std::is_same_v<H, Vec3>) return v.v;
  else if constexpr (std::is_same_v<H, std::string>) return v.s;
  else return logic::Int64(static_cast<u64>(v.i));
}
template <class T, class C, class H> void from_held(EngineObjects& o, const H& h, C& c) {
  if constexpr (std::is_same_v<H, Value>) Conv<T>::from(o, h, c);
  else if constexpr (std::is_same_v<H, bool>) Conv<T>::from(o, Value::boolean(h), c);
  else if constexpr (std::is_same_v<H, i64>) Conv<T>::from(o, Value::integer(h), c);
  else if constexpr (std::is_same_v<H, f64>) Conv<T>::from(o, Value::number(h), c);
  else if constexpr (std::is_same_v<H, Vec3>) Conv<T>::from(o, Value::vector(h), c);
  else if constexpr (std::is_same_v<H, std::string>) Conv<T>::from(o, Value::text(h), c);
  else Conv<T>::from(o, Value::int64(h.bits), c);
}

/// Entry k of a list the value holds, as the type it holds (a std::vector<bool> gives a proxy for an entry -
/// libc++ one that is not a bool - so its entry is read as the bool it is).
template <class H> decltype(auto) held_at(const std::vector<H>& h, std::size_t k) {
  if constexpr (std::is_same_v<H, bool>) return static_cast<bool>(h[k]);
  else return h[k];
}

template <class C> struct ListOf { static constexpr bool is = false; };
template <class X, class A> struct ListOf<std::vector<X, A>> { static constexpr bool is = true, grows = true, maybe = false; using item = X; };
template <class X, std::size_t N> struct ListOf<std::array<X, N>> { static constexpr bool is = true, grows = false, maybe = false; using item = X; };
template <class X, std::size_t N> struct ListOf<X[N]> { static constexpr bool is = true, grows = false, maybe = false; using item = X; };
template <class X> struct ListOf<Span<X>> { static constexpr bool is = true, grows = false, maybe = false; using item = std::remove_const_t<X>; };
template <class X> struct ListOf<std::optional<X>> { static constexpr bool is = true, grows = true, maybe = true; using item = X; };
template <class X, class D> struct ListOf<std::unique_ptr<X, D>> { static constexpr bool is = true, grows = true, maybe = true; using item = X; };
/// (What a function gives through a pointer to data that may be null - `const ClipInfo* clip_info(id)`: a list
/// of no entry or the one it points at.)
template <class X> struct ListOf<X*> { static constexpr bool is = true, grows = false, maybe = true, pointed = true; using item = std::remove_const_t<X>; };

template <class T> struct Conv<List<T>> {
  using H = typename Held<T>::type;
  static const std::vector<H>& items(const Value& v) {
    if constexpr (std::is_same_v<H, i64>) {
      static const std::vector<i64> none;
      return v.holds<i64>() ? v.items<i64>() : none;
    } else {
      return v.items<H>();
    }
  }
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    using L = ListOf<C>;
    static_assert(L::is, "a list pin of something else than a list");
    static_assert(!std::is_pointer_v<C>, "a list the engine is given as a pointer is held in a list of its own (engine_calls.gen.cpp)");
    const std::vector<H>& h = items(v);
    if constexpr (requires { c.data; c.size; }) {   // (a view: of entries held for the call)
      auto& kept = o.held<typename L::item>();
      kept.resize(h.size());
      for (std::size_t k = 0; k < h.size(); ++k) from_held<T>(o, held_at(h, k), kept[k]);
      c = C(kept.data(), kept.size());
    } else if constexpr (L::maybe) {
      if (h.empty()) {
        c = C{};
      } else {
        auto x = blank<typename L::item>();
        from_held<T>(o, held_at(h, 0), x);
        if constexpr (std::is_same_v<C, std::optional<typename L::item>>) c = std::move(x);
        else c = std::make_unique<typename L::item>(std::move(x));
      }
    } else if constexpr (L::grows) {
      c.clear();
      c.resize(h.size());
      for (std::size_t k = 0; k < h.size(); ++k) {
        auto x = blank<typename L::item>();
        from_held<T>(o, held_at(h, k), x);
        c[k] = std::move(x);
      }
    } else {
      const std::size_t n = std::extent_v<C> ? std::extent_v<C> : std::size(c);
      for (std::size_t k = 0; k < n; ++k) {   // (a list of a fixed length: what the value does not have is zero)
        auto x = blank<typename L::item>();
        if (k < h.size()) from_held<T>(o, held_at(h, k), x);
        c[k] = std::move(x);
      }
    }
  }
  template <class C> static Value to(EngineObjects& o, const C& c) {
    using L = ListOf<C>;
    static_assert(L::is, "a list pin of something else than a list");
    std::vector<H> h;
    if constexpr (L::maybe) {
      if (c) h.push_back(held_of<T>(o, *c));
    } else {
      for (const auto& x : c) h.push_back(held_of<T>(o, x));
    }
    if constexpr (std::is_same_v<T, Handle> || std::is_same_v<T, Int>) {
      return Value::of(std::move(h));
    } else if constexpr (std::is_same_v<H, i64>) {
      return Value::enumerations(std::move(h));
    } else {
      return Value::of(std::move(h));
    }
  }
};
template <class L> struct Conv<Row<L>> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    if (const Value* f = v.field(0)) Conv<L>::from(o, *f, c);
    else Conv<L>::from(o, Value{}, c);
  }
  template <class C> static Value to(EngineObjects& o, const C& c) { return Value::structure({Conv<L>::to(o, c)}); }
};

/// The input `k` of a call as a C++ value of type C.
template <class Tag, class C> void take(EngineObjects& o, const std::vector<Value>& in, std::size_t k, C& c) {
  if (k < in.size()) Conv<Tag>::from(o, in[k], c);
}
/// The output `k` of a call from a C++ value.
template <class Tag, class C> void give(EngineObjects& o, std::vector<Value>& out, std::size_t k, const C& c) {
  if (k < out.size()) out[k] = Conv<Tag>::to(o, c);
}

/// The object of a handle as a T, or null - then the call fails, saying which input it was.
template <class T> T* object_of(EngineObjects& o, i64 handle, const char* pin) {
  T* p = o.get<T>(handle);
  if (!p) o.error = std::string("no object of the engine of its class at the input ") + pin;
  return p;
}
/// The same where the function takes a pointer: null for the handle 0; a handle of no such object
/// fails the call (o.error says which input).
template <class T> T* maybe_of(EngineObjects& o, i64 handle, const char* pin) {
  return handle == 0 ? nullptr : object_of<T>(o, handle, pin);
}
/// The object of the handle at input `k` as a T, or null - then the call fails, saying which input.
template <class T> T* object(EngineObjects& o, const std::vector<Value>& in, std::size_t k, const char* pin) {
  return object_of<T>(o, k < in.size() ? in[k].as_int() : 0, pin);
}
/// The object of the handle at input `k` as a T where the function takes a pointer: null for the handle
/// 0; a handle of no such object fails the call (o.error says which input).
template <class T> T* maybe(EngineObjects& o, const std::vector<Value>& in, std::size_t k, const char* pin) {
  return maybe_of<T>(o, k < in.size() ? in[k].as_int() : 0, pin);
}
/// The room a count says (none for less than nothing).
template <class C> std::size_t room(const C& c) {
  if constexpr (is_count<C>) return static_cast<u32>(c);
  else if constexpr (std::is_signed_v<C>) return c > 0 ? static_cast<std::size_t>(c) : 0;
  else return static_cast<std::size_t>(c);
}
/// How many numbers a count says are read through the pointer before it: the count - Count<Per>: Per numbers
/// each (`triangulate(const f64* xy, Count<2> vertex_count, ...)` reads 2 * vertex_count numbers).
template <class C> std::size_t numbers(const C& c) {
  if constexpr (is_count<C>) return static_cast<std::size_t>(static_cast<u32>(c)) * C::per;
  else return room(c);
}
/// What a function gives that is an object of the engine: its handle - the host's own (a reference, a
/// pointer: adopted), or one given by value (owned from now on).
template <class T, class... Bases, class R> i64 handle_of(EngineObjects& o, R&& r) {
  using D = std::remove_cvref_t<R>;
  if constexpr (std::is_pointer_v<D>) return r ? o.adopt<T, Bases...>(*r) : 0;
  else if constexpr (std::is_lvalue_reference_v<R>) return o.adopt<T, Bases...>(r);
  else if constexpr (requires { r.release(); r.get(); }) return o.own<T, Bases...>(std::unique_ptr<T>(std::move(r)));
  else return o.own<T, Bases...>(std::make_unique<T>(std::move(r)));
}

/// Numbers the engine writes through a pointer with a count beside it: the room the count says and a
/// fence behind it, which the call checks after the engine wrote.
template <class X> struct Written {
  static constexpr std::size_t kFence = 16;
  std::vector<X> room;
  std::size_t n = 0;
  explicit Written(std::size_t count) : room(count + kFence), n(count) {
    for (std::size_t k = n; k < room.size(); ++k) room[k] = fence(k);
  }
  static X fence(std::size_t k) {
    if constexpr (std::is_floating_point_v<X>) return static_cast<X>(-1234567.0 - static_cast<f64>(k));
    else return static_cast<X>(0x5a5a5a5a5a5a5a5aull ^ k);
  }
  X* data() { return room.data(); }
  /// Did the engine keep to the room? (Else: what it wrote past it is the table's error.)
  bool kept(EngineObjects& o, const char* pin) {
    for (std::size_t k = n; k < room.size(); ++k)
      if (!(room[k] == fence(k))) {
        o.error = std::string("the engine wrote more numbers through ") + pin + " than its count says (EnginePin::per)";
        return false;
      }
    room.resize(n);
    return true;
  }
};

namespace callback_detail {
template <class C> struct Function;  // (std::function<R(A...)>: its result and its inputs)
template <class CR, class... CA> struct Function<std::function<CR(CA...)>> {
  template <class RT, class... AT> static std::function<CR(CA...)> made(EngineObjects& o, std::shared_ptr<const Callable> call) {
    return [&o, call = std::move(call)](CA... a) -> CR {
      const std::array<Value, sizeof...(CA)> args{Conv<AT>::to(o, a)...};
      Value r;
      call->call(args, r);
      if constexpr (!std::is_void_v<CR>) {
        CR c = blank<CR>();
        Conv<RT>::from(o, r, c);
        return c;
      }
    };
  }
};
template <class F> struct Thunk;  // (the function the engine calls through the pointer: the Handed is its void*)
template <class CR, class... CA> struct Thunk<CR (*)(void*, CA...)> {
  template <class RT, class... AT> static CR call(void* u, CA... a) {
    const EngineObjects::Handed& h = *static_cast<const EngineObjects::Handed*>(u);
    const std::array<Value, sizeof...(CA)> args{Conv<AT>::to(*h.objects, a)...};
    Value r;
    h.call->call(args, r);
    if constexpr (!std::is_void_v<CR>) {
      CR c = blank<CR>();
      Conv<RT>::from(*h.objects, r, c);
      return c;
    }
  }
};
template <class C> inline constexpr bool is_function = false;
template <class S> inline constexpr bool is_function<std::function<S>> = true;
}  // namespace callback_detail
/// A Callback the engine takes: it calls what the value calls (the interpreter, the JIT, a class through the door). An
/// unbound one is the engine's own nothing (a null pointer, an empty std::function).
template <class R, class... A> struct Conv<Callback<R(A...)>> {
  template <class C> static void from(EngineObjects& o, const Value& v, C& c) {
    if constexpr (callback_detail::is_function<C>) {
      if (v.call) c = callback_detail::Function<C>::template made<R, A...>(o, v.call);
      else c = nullptr;
    } else {
      auto& [fn, user] = c;  // (the pointer to the function first, then what it is handed)
      static_assert(std::is_pointer_v<std::remove_cvref_t<decltype(fn)>> && std::is_same_v<std::remove_cvref_t<decltype(user)>, void*>, "a Callback pin of something else than a function pointer with its void*");
      if (!v.call) {
        fn = nullptr;
        user = nullptr;
        return;
      }
      fn = &callback_detail::Thunk<std::remove_cvref_t<decltype(fn)>>::template call<R, A...>;
      user = const_cast<EngineObjects::Handed*>(o.hand(v.call));
    }
  }
  /// (a Callback the engine gives: none a Blueprint calls - unbound)
  template <class C> static Value to(EngineObjects&, const C&) { return Value::callback(nullptr); }
};

}  // namespace engine_values
}  // namespace kern::logic
