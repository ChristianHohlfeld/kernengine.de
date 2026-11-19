// How a program's own C++ calls a Blueprint of the level directly: as its class, typed, with the
// types the Blueprint declares (kern/docs/blueprint_cpp.md, "Aus C++ direkt").
//
// tools/blueprint_cpp writes the Blueprints the program calls this way into a header (their classes,
// under their names in the namespace `blueprints`, with the libraries they use) and the rest of the
// level into the source beside it. The program binds each class once, when the level's logic has
// started - to the very object the runtime runs that Blueprint as -, and from then on calls its
// functions as the functions of any C++ class:
//
//   kern::logic::bind_blueprint<blueprints::qr_code_cpp>(logic);      // once (blueprints::bind: all of them)
//   blueprints::qr_code::qr_Code code;
//   if (kern::logic::blueprint<blueprints::qr_code_cpp>().encode(text, code).Result) ...
//
// Nothing is looked up by name, nothing is packed into values: a reference binds to the Blueprint's
// own type, a value goes in as the Blueprint takes it. The object is the runtime's, so the class runs in
// the world the runtime gives it - its loop guard (Runtime::budget) is the runtime's, as in every tier.
#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include "kern/api/c_api.h"
#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

/// The object of the Blueprint class T that the program's C++ calls (null until it is bound).
template <class T> inline T* blueprint_object = nullptr;

/// Binds the class T to `logic`: the instance there that runs as T (Runtime::native; found once, here)
/// is from now on what blueprint<T>() gives - until it is bound again (to another runtime: a runtime
/// that ends takes its objects with it). False: no instance there runs as T (the level has no such
/// Blueprint, it has errors, or the binary's class was written from another graph).
template <class T> bool bind_blueprint(Runtime& logic) {
  blueprint_object<T> = nullptr;
  for (const auto& inst : logic.instances())
    if (T* object = inst->template native_object<T>()) {
      blueprint_object<T> = object;
      break;
    }
  return blueprint_object<T> != nullptr;
}

/// The bound object of the class T (bind_blueprint first): its functions are called on it directly.
template <class T> T& blueprint() { return *blueprint_object<T>; }


// ---- exports: a function of a Blueprint the program gives its host under a fixed C name ----
// (Function::exported, KERN_EXPORT_AS; the glue of the class writes the export - logic::native_glue)
/// A number as the export takes it in (i32, u32, f32, f64) - into the language's Int, Float or Bool as C++
/// converts it.
template <class In, class C> In export_in(C x) { return In(x); }
template <> inline bool export_in<bool, i32>(i32 x) { return x != 0; }
template <> inline bool export_in<bool, u32>(u32 x) { return x != 0; }
template <> inline bool export_in<bool, f32>(f32 x) { return x != 0; }
template <> inline bool export_in<bool, f64>(f64 x) { return x != 0; }
/// A text that comes in: its characters, copied (the host's memory is the host's).
inline Text export_text_in(const char* text, u32 length) { return Text(std::string_view(text ? text : "", text ? length : 0)); }
/// Bytes that come in: a List of Int, one entry each.
inline List<Whole> export_bytes_in(const u8* bytes, u32 length) {
  List<Whole> out;
  out.reserve(length);
  for (u32 i = 0; i < length; ++i) out.push_back(Whole(static_cast<i64>(bytes[i])));
  return out;
}
/// A number as the export gives it (i32, u32, f32, f64): as C++ converts it.
template <class C, class Out> C export_out(const Out& x) {
  if constexpr (std::is_same_v<Out, Whole>) return static_cast<C>(x.v);
  else return static_cast<C>(x);
}
/// A text the export gives: its characters, kept until an export gives the next text (the host reads them by
/// the pointer, as far as its length says).
inline const char* export_text_out(const Text& text) {
  static std::string held;
  held.assign(text.view());
  return held.c_str();
}
}  // namespace kern::logic
