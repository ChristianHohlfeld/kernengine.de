# Stand c (Rückrufe der Engine in das Programm) – 19.11.2026

Zweig work-c (Worktree /home/claude/w-c), Basis origin/work-zusammen 6998a56e. Entwurf E2 (uebergabe/c.md) gilt; hier die
Entscheidungen beim Bau (nach den Zielen, ohne Freigabe – Chris).

## Entwurf beim Bau (Entscheidungen)
- **Typ** `PinType::callback` (hinter `set`: Listen/Werte-Zählung unberührt), `TypeRef::of` = Signatur `float(float,float)`,
  ohne Ergebnis `(float)`. Eingänge/Ergebnis: Bool, Int, Float, Vector, Int64 (Werte, die C++ als Wert übergibt) – Teil 1.
  Kein Callback in Listen, Maps, Sets; kein Vergleich, keine Umwandlung, kein Aufruf durch den Blueprint.
- **Wert** `Value::call` = `shared_ptr<const Callable>` (leer = ungebunden: ein Aufruf gibt die Null des Ergebnisses).
  Interpreter/JIT: {Lebenszeichen der Instanz, Funktion, Spot des Platzes}; Klasse durch die Tür: {fn, place, Wert-Thunk}.
- **Knoten** `func.callback` „Make Callback“ (Wert-Knoten): Einstellung `function`; ist ihr erster Eingang eine const-Referenz,
  ist das der gebundene Platz (Einstellungen wie die Referenz-Eingänge von Call). Text `callback<&Lib::fn>(platz)` / `callback<&Lib::fn>()`.
- **Regel (check):** die Funktion ist eine **Funktion einer Bibliothek** (die Klasse ruft sie auf dem Objekt der Bibliothek, das
  nichts hält: `kern::logic::the_library<L>`), hat höchstens einen Ausgang, braucht nichts von der Welt – transitiv keine
  Knoten der Engine/des Spiels, kein Print, Game Time, Random, Call on Actor/Event (NodeDef::world).
- **Klasse:** `Callback<R(A…)>` = `{R(*fn)(void*, A…); void* place;}`, Thunk ruft `the_library<L>.fn(*place, a…)`.
  Zum HeightSampler Feld für Feld, zu std::function ein Lambda mit fn+place (kleiner Puffer, keine Anforderung).
- **Tür:** der C++-Wert der Engine ruft über ein Trampolin den Callable. Lebensdauer: ein Callback-Eingang einer Methode
  eines Objekts der Engine wird mit dem Objekt gehalten (je Objekt und Pin der letzte), einer freien Funktion nur während
  des Aufrufs.

## Stand 13:30 (work-c, letzter Commit siehe git log)
- Gebaut: Sprache, Knoten, Text, Klasse, Tür, direkte Form, Generator (HeightSampler/std::function → Callback; RoadOptions
  Struct der Engine; 814 Knoten), Stellvertreter ruft Callbacks (dreimal, gewürfelt, behalten je Objekt), nativer Fall
  Ground/Site, Test logic_callbacks, Doku, Editor, Konverter-Regel „Lambda an einem Callback-Platz“.
- **Lokal belegt** (Kleinstfall, logic+core gebaut): logic_callbacks 0 Fehler; Ground/Site Interpreter = Klasse (übersetzter
  Text), Zeichen für Zeichen (3.5/true, 3.75; nach Again 7.25/7.75).
- Läuft: CI 44523773375 auf edefcb08 (Engine-Suite inkl. JIT/WebAssembly, native, determinism);
  mag `work-c-vergleich-132846-9096` (city_stream, terrain – Rückschritte durch HeightSampler/RoadOptions als Werte?).
- Offen (uebergabe/c.md): Terrain::sampler (Felder fn/user mit void*), Test-Schreiber der Quelle für Callback-Werte,
  Regel „kein Listeneintrag auf dem Weg des Platzes“, Teil 2 Jobs.
- **Berührt Paket d:** tools/engine_api.py nur `tag()` (3 Zeilen); erzeugte Dateien (engine_api.gen.cpp, engine_nodes.gen.h,
  engine_case.gen.h, src/geo/engine_calls.gen.cpp) neu – beim Mischen mit d neu erzeugen statt Text mischen.
