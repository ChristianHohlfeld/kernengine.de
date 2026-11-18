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

#include "kern/logic/logic.h"

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

}  // namespace kern::logic
