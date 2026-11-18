# Übergabe Paket p (Leistung der exportierten Klasse) – 18.11.2026, ca. 17:20

S=/home/claude/kernengine.de/koordination
Zweig `work-p` (von origin/work 90b5ab8), Worktree `/home/claude/w-p`, letzter Commit `04f1a57` (gepusht).
Auftrag: $S/PAKET3_p.md, Regeln: $S/PAKET_COMMON3.md (samt Nachträgen 15:40, 16:20, 16:50, 17:10 „sparsam“).

## Entscheidungen mit dem Koordinator
Keine getroffen. Die Ursachenliste (unten) ist NOCH NICHT an main gemeldet. Laut Auftrag ist das vor dem Beheben Pflicht
(SendMessage an main). Ursache 1 braucht eine Entscheidung (sie ändert die Schreibweise, siehe unten).

## Fertig, aber unbewiesen
- `04f1a57`: `tools/blueprint_cpp.sh --classes <level> <out.h> <actor>...` schreibt nur die Klassen dieser Blueprints und
  der Bibliotheken, die sie benutzen (über `uses`, transitiv), ohne Kleber, `#pragma once`, am Ende `namespace blueprints
  { using <Klasse> = ::kern_blueprint_<hash>::<Klasse>; }`. Ein Blueprint ohne Klasse ist dort ein Fehler. Nur die Syntax ist
  geprüft (`c++ -fsyntax-only` sauber); gelaufen ist es noch nicht. Beim Übergeben lief im Hintergrund der erste Lauf:
  `KERN_JOBS=2 TMPDIR=$S/shared-tmp tools/blueprint_cpp.sh --classes kern/games/spawncity/level/spawncity.klevel $S/p/b/qr.h qr_code_cpp`
  (baut das Werkzeug neu, ein paar Minuten). Ergebnis (gelaufen, 17:12): $S/p/b/qr.h, 1335 Zeilen, Aliase shapes, qr_code, qr_code_cpp – dieselben Namensräume (Hashes) wie s5s Header; ob er allein übersetzt, ist noch nicht geprüft.
  Achtung Merge: work-s5 ändert dieselbe Datei (d5132bc, Header eines Ordners) – meine Änderung ist klein, beim Mergen von
  Hand zusammenführen. Mögliche Falle in meinem Code: `std::erase_if(held, …)` steht nach dem Bau von `all`, der über
  `held` sucht – das ist gewollt (Bibliotheken der gewünschten Actors sind in `only`), aber prüfen, dass `logic::link`
  alle Bibliotheken findet.

## Messung (Schritt 1) – bisher nur qr encode
Reproduziert mit s5s Bench ($S/s5/bench: bench_old = Spiel-C++ f32, bench_new = Klasse aus $S/s5/gen/level_blueprints.h,
Level von 955503b = Repo-Level vom 15.11.), diese Maschine, g++ 13 -O2:
- C++: 41–56 µs je encode (v5-URL), Klasse: 711–845 µs. Verhältnis 13–20×. Ausgabe gleich (Summe 1022).
- Kopie mit -g und n=200 in $S/p/qr (bn, cg.out = callgrind-Profil). Bau: im Worktree w-s5:
  `c++ -std=c++20 -O2 -g -fno-exceptions -fno-rtti -I kern/include -I $S/p/qr $S/p/qr/bench_new.cpp -o $S/p/qr/bn`
  (die Klasse braucht nur einen Stub `kern::logic::Runtime::pass`, steht in bench_new.cpp).
Der Release-Bau des Spiels ist `-O3 -flto=thin -DNDEBUG -ffp-contract=off -fno-exceptions -fno-rtti` (kern/build.sh Z. 56/127),
clang. Für die Messung so bauen (clang++-18 ist da).

### Plan für das Messwerkzeug (nicht gebaut)
- `tools/blueprint_bench.sh`: Klassen per `--classes` aus dem Level, dazu ein Treiber je Spiel
  (z. B. kern/games/spawncity/tests/bench/bench.cpp): je Fall ein Lambda Quelltext, ein Lambda Klasse, gleiche Eingaben
  (von Hand gebaut), Ergebnis-Gleichheit prüfen, K Runden × N Aufrufe abwechselnd, Minimum/Median je Aufruf, Verhältnis.
- Einheiten ohne Engine-Abhängigkeit (lassen sich mit dem Treiber allein übersetzen): qr_code, nostr (Int64-Bignum), mvt
  (Bytes/Text/Listen von Strukturen), wire (Bytes packen), person_hit (Vektor-Geometrie), perf (Strukturen, Floats),
  common.h (Quat/Vektor, Rng), city_jobs. Daraus 20–30 Funktionen wählen.
- Vergleichsbasis: das C++ des Spiels, wie gebaut (f32). Bei Float-Funktionen rechnet die Klasse 64 Bit (so gewollt);
  dort Ergebnisgleichheit nicht verlangen, aber die 64-Bit-Fassung („wide“, wie tests/converted.h sie hat: f32=double)
  als zweite Basis nennen. Achtung: die wide-Quelle in tests/converted benutzt `converted::at` (geprüfter Zugriff) – sie
  ist nicht die kostengleiche Basis.
- Level: das Repo-Level ist vom 15.11. (435ed96); Graphen ggf. älter als die Quelle → Ergebnisgleichheit im Treiber
  fängt Abweichungen. Frischer Graph nur über eine Probe (lokal schreibt sie $D/l.klevel; mag liefert kein Level).

## Ursachen (Schritt 2) – aus callgrind von qr encode ($S/p/qr/cg.out), Anteil an den Instruktionen
1. **Ganzzahlrechnung als Float-Formel (~65–70 %).** Der Konverter schreibt jede Integer-Rechnung als Formel-Knoten
   über Float: `z = kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::bxor(kern::math::wrap32(
   kern::math::shl(z, 1.0)), kern::math::mul32(kern::math::shr(z, 7.0), 285.0)); }, kern::to<Float>(z)));`
   Jeder Operand: i64→double, `kern::math::bits_of` (NaN-Prüfung, zwei Bereichsprüfungen, cvttsd2si), zurück nach double;
   am Ende `kern::to<Int>` = `logic::whole` (isfinite, 2 Vergleiche) + `logic::normal` (2 Vergleiche) (nodes.h ~Z. 90–125,
   logic.h ~Z. 120–145). `gf_mul` allein: 48 % (8 Durchläufe, je 3 solche Formeln; im C++ ~6 Int-Befehle).
   Auch Vergleiche: `math_compare<">=">(kern::to<Float>(i), 0.0)`, Schleifenbedingungen
   `kern::formula([](Float j, Float a){ return (j<0)!=(a<0) ? … }) > 0.5` (der Konverter bildet signed/unsigned-Vergleich nach).
   Behebung braucht den Graphen: der Generator (kern/src/logic/notation.cpp, Writer der Formel) müsste eine Formel, deren
   Eingänge Int sind und deren Ausdruck nur aus ganzzahligen Operationen besteht, ganzzahlig schreiben – bitgleich nur mit
   (a) Bereichsherleitung am Graphen (Schleifenindex aus flow_for(0,7), `band(x,255)`, `wrap32`, `mul32` haben feste
   Bereiche; Zuweisungen an eine lokale Variable vereinigen) oder (b) geprüftem Schnellweg: in i64 rechnen, solange jeder
   Zwischenwert |x| < 2^53 bleibt (dann rechnet double exakt dasselbe: +,−,× exakt; trunc(div) korrekt für |a| < 2^53;
   bits_of(double(Int)) == Int, weil `normal` jeden Int auf einen double-Wert legt – Randfall: −(2^63−1) wird als double
   −2^63, bits_of gibt dann i64 min), sonst die Float-Formel. Beides ändert, was der Text schreibt (Lambda-Typen) – also die
   Schreibweise/den Leser (from_cpp) und die Fixpunkt-Tests → **Entscheidung beim Koordinator nötig**: Schreibweise ändern
   (z. B. Formel-Lambda mit `auto`-Parametern und ein `kern::formula_int`, das erst exakt in i64 rechnet und bei Verlassen
   von ±2^53 auf double fällt) oder nur die Klasse anders erzeugen (dann ist die Klasse nicht mehr wörtlich to_cpp).
2. **`kern::to<Int,double>` nicht inline (~30 % mit Ursache 1 verschränkt).** g++ macht daraus einen `.isra`-Klon, also
   einen Aufruf je Formel. Behebung allgemein und ohne Schreibweise: `logic::detail::To<i64>::from`/`whole`/`normal`
   inline erzwingen (nodes.h Z. ~2515, `To<Whole> : To<i64>`) – bitgleich, reine Kosten.
3. **`math_compare` vergleicht den Operator zur Laufzeit als Text (~7,5 %).** blueprint.h Z. 692:
   `template <Setting Op> static Bool math_compare(Float a, Float b) { return compare(Op.view(), a, b); }` →
   `PureNodes::compare(std::string_view, …)` (nodes.h Z. 2289) wird nicht aufgelöst. Behebung: in math_compare mit
   `if constexpr` auf Op verzweigen (dieselben Ausdrücke, == und != mit 1e-9) – bitgleich, keine Schreibweise.
   Achtung: == / != sind „innerhalb 1e-9“, also kein exakter Ganzzahlvergleich – bei Ints gleichwertig, solange |a−b|
   ganzzahlig ist (immer bei Ints ≤ 2^53).
4. **Schleifenwächter je Durchlauf (wenige %).** `kern_pass()` (blueprint.h Z. 225) liest `kern_world.counters`, vergleicht
   und erhöht `count[3]` im Speicher; `ForLoop::next` geht über `logic::flow::for_step/for_runs/for_index` auf
   `n.memory` (Member der Klasse, kein Register). Bedeutung (Budget 200.000, gleicher Durchlauf in jeder Stufe) muss
   bleiben; billiger: Zähler lokal halten geht nicht ohne Bedeutungsänderung – höchstens die Speicherzugriffe straffen.
5. **Listenzugriff mit „beside“-Prüfung** (`List::operator[]`, nodes.h ~Z. 225): Vergleich je Zugriff, im Profil ~0,5 %.
   Bedeutung (Platz neben der Liste) ist festgelegt; wegfallen darf die Prüfung nur, wo der Index am Graphen beweisbar im
   Bereich ist (Schleifenindex 0..length−1 über dieselbe Liste, ohne Längenänderung im Rumpf).
6. `List<Bool>` als `Truth` und `list_add` je Bit in `encode_put` – klein.
Noch nicht untersucht (Auftrag nennt sie): Float-Funktionen (f32↔f64 an Feldern, Vector mit 32-Bit-Komponenten:
`single()`), Kopien von Strukturen/Listen, Text, Maps, Aufrufe über Bibliotheks-Basisklassen (`virtual` Basen – prüfen, ob
Aufrufe über virtuelle Basis Inlining verhindern), Int64.

## Nächste Schritte
1. $S/p/b/qr.h einmal mit s5s bench_new.cpp übersetzen (Include auf qr.h umstellen); Messwerkzeug + 20–30 Fälle bauen, Messung „vorher“ festhalten.
2. Profil von 2–3 Float-/Struktur-Funktionen dazu, Ursachenliste vervollständigen, **an main senden** (mit der Frage zu Ursache 1).
3. Ursachen 2 und 3 sind ohne Schreibweise behebbar (nodes.h/blueprint.h) – erst nach der Meldung.
4. Beweis: `$S/mag.sh engine origin/work-p` und `$S/mag.sh spiel origin/work-p`; Zeittest mit Schwelle (Schritt 5).

## mag-Aufträge
Keine gestellt.

## Fallen
- Commit-Ende laut Aufrufer: `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>` (PAKET_COMMON3.md nennt „Fable 5.2“ – der
  Aufrufer hat Opus 5.5 vorgegeben) plus die Claude-Session-Zeile.
- Sparsam arbeiten (Nachtrag 17:10): Stand nach $S/stand/p.md, Nachrichten an main nur für Entscheidung/Ursachenliste/Merge.
