// Die Zahl der Sprache in einer Quelle des C++-Teilsatzes (kern/docs/blueprint_cpp.md, „Zahlen“).
//
// Ein Blueprint rechnet jede Kommazahl als Float: 64 Bit, in jeder Stufe gleich. Eine Quelle im Teilsatz hält und
// rechnet ihre Kommazahlen ebenso - als kern::Float -, damit sie nativ gebaut dasselbe ergibt wie ihr Blueprint.
// kern::f32 ist die Zahl der Engine (ihre Felder, Ein- und Ausgänge): gelesen wird sie genau zu einem Float, und ein
// Float, der in sie geht (ein Eingang oder ein Feld der Engine, eine Komponente eines kern::Vec3), wird zur nächsten
// f32 gerundet, wie C++ ein double zu einem float macht - nativ und im Blueprint an derselben Stelle.
#pragma once
#include "kern/core/base.h"

namespace kern {

/// Die Kommazahl der Sprache: 64 Bit (der Pin-Typ Float).
using Float = f64;

}  // namespace kern
