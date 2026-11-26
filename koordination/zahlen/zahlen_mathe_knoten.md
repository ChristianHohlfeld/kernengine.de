# Mathe-Funktionen ohne Knoten der Sprache – Vorlage für work-zahlen (Agent projekt-zahlen1, 26.11.)
Regel (main, 26.11.): Jede Mathe-Funktion, die die Quelle ruft, wird ein Knoten, der DIE Funktion der Engine ruft (kern::dm/musl,
f-Variante für f32). Auf work-projekt schreibt der Konverter für diese Funktionen heute eine Formel (tools/cpp_to_blueprint.py,
Fx-Formel ~Z. 10820-10845; im ersten Durchgang des Tests `converted::wide::*_bp`). Die Formel ist NICHT die Funktion der Engine.
Zählung: `grep std::<fn>(` in kern/games/spawncity/src (*.cpp, *.h); ohne `std::` und als f-Name (tanhf …) gibt es keine Stelle.
Die Firmware ruft keine dieser Funktionen. f32/f64: der Typ des Arguments ist statisch nicht ausgezählt. Was im Beispiel steht,
ist angegeben; genau liefert es das Konverter-Protokoll (Typ des ersten Arguments).

| Funktion | Stellen | Beispiel (Typ) | in musl_math (Engine) | Formel des Konverters heute |
|---|---|---|---|---|
| hypot | 239 | überall, meist f32 (Abstände) | ja: hypot, hypotf | `sqrt(a*a + b*b)` |
| ldexp | 4 | terrain.cpp:39/41, tiles.cpp:18/28 `std::ldexp(1.0, z)` (f64) | nein (exakt in IEEE: ldexp/scalbn wären aus musl zu nehmen oder exakt per Bits) | `a * pow(2.0, n)` |
| asinh | 2 | terrain.cpp:39, tiles.cpp:23 `std::asinh(std::tan(r))` (f64) | nein | `log(x + sqrt(x*x + 1))` |
| tanh | 2 | car.cpp:422 (f32: vx*2), drone.cpp:15 (f32) | nein | `div(exp(2x) - 1, exp(2x) + 1)` |
| sinh | 1 | tiles.cpp:30 `std::sinh(kPiD * …)` (f64) | nein | `(exp(x) - exp(-x)) * 0.5` |
| cosh | 0 | – | nein | `(exp(x) + exp(-x)) * 0.5` |
| log10 | 0 | – | ja: log10, log10f | `div(log(x), 2.302585092994046)` |
| log2 | 0 | – | nein | `div(log(x), 0.6931471805599453)` |
| cbrt | 0 | – | nein | `sign(x) * pow(abs(x), 0.3333333333333333)` |
| expm1 | 0 | – | ja: expm1, expm1f | (keine; nicht in MATH) |
Dazu Funktionen, für die es einen Knoten gibt, der aber in f64 rechnet: sin, cos, tan, asin, acos, atan, atan2, exp, log, pow (Knoten
rufen kern::dm in f64; mit f32 als Typ muss der f32-Knoten die f-Variante rufen). sqrt, floor, ceil, trunc, round, fmod, remainder,
copysign, fabs: exakt.
Bis die Knoten da sind: sinh/tanh/cosh/asinh (musl-Quelle fehlt in der Engine) sind auf work-projekt ein Wartegrund des Konverters
mit Ort („the engine has no tanh yet“); hypot, log10, ldexp, log2, cbrt bleiben vorerst Formeln (Abnahmeliste (b), B7).
