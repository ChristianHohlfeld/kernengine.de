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
#include <vector>
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
/// A text that comes in without its length: up to its 0.
inline Text export_text_in(const char* text) { return Text(std::string_view(text ? text : "")); }
/// Bytes that come in: a List of Int, one entry each.
inline List<Whole> export_bytes_in(const u8* bytes, u32 length) {
  List<Whole> out;
  out.reserve(length);
  for (u32 i = 0; i < length; ++i) out.push_back(Whole(static_cast<i64>(bytes[i])));
  return out;
}
/// Floats that come in (f32, as the host gives them): a List of Float, one entry each, as C++ converts an f32.
inline List<f64> export_floats_in(const f32* floats, u32 length) {
  List<f64> out;
  out.reserve(length);
  for (u32 i = 0; i < length; ++i) out.push_back(static_cast<f64>(floats[i]));
  return out;
}
/// A number as the export gives it (i32, u32, f32, f64): as C++ converts it.
template <class C, class Out> C export_out(const Out& x) {
  if constexpr (std::is_same_v<Out, Whole>) return static_cast<C>(x.v);
  else return static_cast<C>(x);
}

// What an export gives its host by a pointer - a text, bytes, floats - is kept here until an export gives the next
// of its kind (one place for each kind, in the whole program: a program's own C++ gives through the same functions
// as the glue of a class). The host reads it by the pointer at once - a text as far as kern_text_length says
// (kern/src/api/c_api.cpp), bytes and floats as many as it knows.
inline std::string& export_kept_text() {
  static std::string held;
  return held;
}
inline std::vector<u8>& export_kept_bytes() {
  static std::vector<u8> held;
  return held;
}
inline std::vector<f32>& export_kept_floats() {
  static std::vector<f32> held;
  return held;
}
/// A text an export gives: its characters, kept (export_kept_text).
inline const char* export_text(std::string_view text) {
  std::string& held = export_kept_text();
  held.assign(text);
  return held.c_str();
}
inline const char* export_text(const std::string& text) { return export_text(std::string_view(text)); }
/// Bytes an export gives: kept (export_kept_bytes); none (null) when there are none.
inline const u8* export_bytes(Span<const u8> bytes) {
  std::vector<u8>& held = export_kept_bytes();
  held.assign(bytes.begin(), bytes.end());
  return held.empty() ? nullptr : held.data();
}
inline const u8* export_bytes(const std::vector<u8>& bytes) { return export_bytes(Span<const u8>(bytes.data(), bytes.size())); }
/// Floats an export gives: kept (export_kept_floats); none (null) when there are none.
inline const f32* export_floats(Span<const f32> floats) {
  std::vector<f32>& held = export_kept_floats();
  held.assign(floats.begin(), floats.end());
  return held.empty() ? nullptr : held.data();
}
inline const f32* export_floats(const std::vector<f32>& floats) { return export_floats(Span<const f32>(floats.data(), floats.size())); }
/// The length of the text an export gave last.
inline u32 export_text_length() { return static_cast<u32>(export_kept_text().size()); }

/// What a Blueprint's export gives (the glue of its class): a Text, a List of Int (each entry as a u8, as C++
/// converts it) or a List of Float (each as an f32), given as a program's own C++ gives it (above).
inline const char* export_text_out(const Text& text) { return export_text(text.view()); }
// (written straight into the place that is kept: once it holds as many, an export asks for no memory - an audio
// callback gives its samples every few milliseconds)
template <class L> const u8* export_bytes_out(const L& bytes) {
  std::vector<u8>& held = export_kept_bytes();
  held.resize(bytes.size());
  for (std::size_t i = 0; i < held.size(); ++i) held[i] = static_cast<u8>(Whole(bytes[i]).v);
  return held.empty() ? nullptr : held.data();
}
template <class L> const f32* export_floats_out(const L& floats) {
  std::vector<f32>& held = export_kept_floats();
  held.resize(floats.size());
  for (std::size_t i = 0; i < held.size(); ++i) held[i] = static_cast<f32>(static_cast<f64>(floats[i]));
  return held.empty() ? nullptr : held.data();
}
}  // namespace kern::logic
