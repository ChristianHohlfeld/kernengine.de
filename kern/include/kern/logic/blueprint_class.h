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

/// Binding a class: the program says once which object its C++ - and the exports of the class - call. It belongs to
/// the program, not to a world: one bound object per class. Binding again while one is bound is refused with an
/// error (kern::fail), unless the program says it rebinds (rebind_blueprint) or has unbound (unbind_blueprint); an
/// object that ends unbinds itself (the glue of its class).
template <class T> void unbind_blueprint() { blueprint_object<T> = nullptr; }
/// Binds the class T to this one instance (it runs as T) - false, and an error, when it does not run as T.
template <class T> bool rebind_blueprint(Instance& instance, const char* name = "a Blueprint class") {
  blueprint_object<T> = instance.template native_object<T>();
  return blueprint_object<T> != nullptr || fail("%s: the instance %s does not run as this class", name, instance.name().c_str());
}
/// Binds the class T to the first instance of `logic` that runs as T (Runtime::native; found once, here) - false
/// when none does (the level has no such Blueprint, it has errors, or the binary's class was written from another
/// graph: nothing is bound then).
template <class T> bool rebind_blueprint(Runtime& logic, const char* name = "a Blueprint class") {
  blueprint_object<T> = nullptr;
  for (const auto& inst : logic.instances())
    if (T* object = inst->template native_object<T>()) {
      blueprint_object<T> = object;
      break;
    }
  return blueprint_object<T> != nullptr;
}
/// Binds the class T (rebind_blueprint) - refused, with an error, while another object of it is bound.
template <class T> bool bind_blueprint(Runtime& logic, const char* name = "a Blueprint class") {
  if (blueprint_object<T>) return fail("%s is bound already: the program unbinds it first (unbind_blueprint) or rebinds it (rebind_blueprint)", name);
  return rebind_blueprint<T>(logic, name);
}
template <class T> bool bind_blueprint(Instance& instance, const char* name = "a Blueprint class") {
  if (blueprint_object<T>) return fail("%s is bound already: the program unbinds it first (unbind_blueprint) or rebinds it (rebind_blueprint)", name);
  return rebind_blueprint<T>(instance, name);
}
/// Binds the class T to this object of it (the glue of every class: NativeBlueprint::bind_class - the program's runtime binds its Blueprints so) - refused,
/// with an error, while another object of it is bound; the same one again: true.
template <class T> bool bind_blueprint_object(T& object, const char* name = "a Blueprint class") {
  if (blueprint_object<T> && blueprint_object<T> != &object)
    return fail("%s is bound already: the program unbinds it first (unbind_blueprint) or rebinds it (rebind_blueprint)", name);
  blueprint_object<T> = &object;
  return true;
}
/// An export of the class called while none of it is bound: an error with the export's name (kern::fail) - the
/// export gives nothing then.
inline bool export_unbound(const char* exported) { return fail("the export %s: no instance of its Blueprint is bound (kern::logic::bind_blueprint)", exported); }

/// The bound object of the class T (bind_blueprint first): its functions are called on it directly.
template <class T> T& blueprint() { return *blueprint_object<T>; }


// ---- exports: a function of a Blueprint the program gives its host under a fixed C name ----
// (Function::exported, KERN_EXPORT_AS; the glue of the class writes the export - logic::native_glue)
/// A number as the export takes it in (i32, u32, f32, f64): the input is of that type itself (Function::wire).
template <class In, class C> In export_in(C x) {
  static_assert(std::is_same_v<In, C>, "an export's number is of its C type (i32, u32, f32, f64)");
  return x;
}
/// A text that comes in: its characters, copied (the host's memory is the host's).
inline Text export_text_in(const char* text, u32 length) { return Text(std::string_view(text ? text : "", text ? length : 0)); }
/// A text that comes in without its length: up to its 0.
inline Text export_text_in(const char* text) { return Text(std::string_view(text ? text : "")); }
/// Bytes that come in: a List<u8>, one entry each.
inline List<u8> export_bytes_in(const u8* bytes, u32 length) { return List<u8>(typename List<u8>::Items(bytes, bytes + length)); }
/// Floats that come in (f32, as the host gives them): a List<f32>, one entry each.
inline List<f32> export_floats_in(const f32* floats, u32 length) { return List<f32>(typename List<f32>::Items(floats, floats + length)); }
/// A number as the export gives it (i32, u32, f32, f64): the output is of that type itself.
template <class C, class Out> C export_out(const Out& x) {
  static_assert(std::is_same_v<C, Out>, "an export's number is of its C type (i32, u32, f32, f64)");
  return x;
}

// What an export gives its host by a pointer - a text, bytes, floats - is a piece {pointer, length}: the export
// returns the pointer and writes the length where the host says (`out_len`, may be null). The piece is the export's
// own - each export function keeps its own place (the template's `Export`: the function itself), until that export
// gives the next - so no export overwrites what another gave, and a call on another thread (the audio callback)
// touches only its own export's place. Once the place holds as many, giving again asks for no memory.
// A program's own C++ gives through these, the glue of a Blueprint's class through the *_out ones below.
template <auto Export> const char* export_text(std::string_view text, u32* out_len) {
  static std::string held;
  held.assign(text);
  if (out_len) *out_len = static_cast<u32>(held.size());
  return held.c_str();
}
template <auto Export> const char* export_text(const std::string& text, u32* out_len) { return export_text<Export>(std::string_view(text), out_len); }
/// Bytes an export gives (none: null, length 0).
template <auto Export> const u8* export_bytes(Span<const u8> bytes, u32* out_len) {
  static std::vector<u8> held;
  held.assign(bytes.begin(), bytes.end());
  if (out_len) *out_len = static_cast<u32>(held.size());
  return held.empty() ? nullptr : held.data();
}
template <auto Export> const u8* export_bytes(const std::vector<u8>& bytes, u32* out_len) { return export_bytes<Export>(Span<const u8>(bytes.data(), bytes.size()), out_len); }
/// Floats an export gives (none: null, length 0).
template <auto Export> const f32* export_floats(Span<const f32> floats, u32* out_len) {
  static std::vector<f32> held;
  held.assign(floats.begin(), floats.end());
  if (out_len) *out_len = static_cast<u32>(held.size());
  return held.empty() ? nullptr : held.data();
}
template <auto Export> const f32* export_floats(const std::vector<f32>& floats, u32* out_len) { return export_floats<Export>(Span<const f32>(floats.data(), floats.size()), out_len); }

/// What a Blueprint's export gives (the glue of its class): a Text, a List of Int (each entry as a u8, as C++
/// converts it) or a List of Float (each as an f32) - written straight into the export's own place.
template <auto Export> const char* export_text_out(const Text& text, u32* out_len) { return export_text<Export>(text.view(), out_len); }
template <auto Export, class L> const u8* export_bytes_out(const L& bytes, u32* out_len) {
  static std::vector<u8> held;
  held.resize(bytes.size());
  for (std::size_t i = 0; i < held.size(); ++i) held[i] = bytes[i];   // (a List<u8>)
  if (out_len) *out_len = static_cast<u32>(held.size());
  return held.empty() ? nullptr : held.data();
}
template <auto Export, class L> const f32* export_floats_out(const L& floats, u32* out_len) {
  static std::vector<f32> held;
  held.resize(floats.size());
  for (std::size_t i = 0; i < held.size(); ++i) held[i] = floats[i];   // (a List<f32>)
  if (out_len) *out_len = static_cast<u32>(held.size());
  return held.empty() ? nullptr : held.data();
}
}  // namespace kern::logic
