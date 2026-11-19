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

## Stand 14:35 – work-c = a137ba34 (gepusht)
- **Engine-Suite grün (CI 44526990074 auf a137ba34):** native: 54577 Prüfungen, 0 Fehler (logic_callbacks,
  logic_notation_every_node_type, tier cases …); `blueprint_native_test` ok (64 Blueprints als ihr C++, 14111 Aufrufe – darunter
  Ground/Site mit Callbacks und der Fall Engine mit RoadOptions als Struct, Callback-Pins); build ok (logic_wasm_test: alle
  Fälle Interpreter = WebAssembly); determinism ok. native-spawncity/e2e/input liefen noch.
- Lokal (Kleinstfall): logic_callbacks 0 Fehler; Ground/Site Interpreter = Klasse (übersetzter Text) Zeichen für Zeichen.
- JIT: func.callback ist kein Inline-Knoten – der JIT ruft seine C++-Funktion (call_node), wie der Interpreter. Ein eigener
  WebAssembly-Fall fehlt (logic_cases.h sind Einzel-Graphen, ein Callback braucht eine Bibliothek).
- Konverter: CityStream::begin_roads (Lambda [this] → Funktion der Bibliothek, callback<…>(self)) und Terrain::sampler (Felder
  fn/user) werden geschrieben, soweit lokal ohne Level sichtbar; **Beleg ausstehend:** mag `work-c-vergleich-132846-9096`
  (city_stream, terrain; abgelegt 13:28, mag voll – rechnet den Stand beim Start).
- Kein „BEREIT ZUM MERGEN“, bis der vergleich in beiden Durchgängen ohne Abweichung ist. Erwartete Lücke dort: der
  Test-Schreiber der Quelle kennt Callback-Werte noch nicht (HeightSampler/RoadOptions sind jetzt Werte: arg/same/draw für
  fn+void* und std::function fehlen in converted.h/stand_ins) – dann warten sampler/begin_roads „am Test“.
- **Berührt Paket d (Besitzer engine_api.py):** dort nur `Calls.tag` (+3 Zeilen); Api/Unit-Erweiterung in cpp_to_blueprint.py
  (callback_record/sig/type, btype, zero, Api.pin/shape); erzeugte Dateien neu (engine_api.gen.cpp, engine_nodes.gen.h,
  engine_case.gen.h, src/geo/engine_calls.gen.cpp) – beim Mischen neu erzeugen. EngineStandIn (engine_api.cpp) ruft jetzt
  Callbacks; EngineObjects hält sie (engine_host.h).
- Regel berührt: keine. (Neu in der Sprache: Typ callback, Knoten func.callback, NodeDef::world – allgemein, für jedes Spiel.)

## Stand 15:10 (Nachfolger) – work-c = f877b9e5 (gepusht)
- f0619a81 Merge work-t1-cb 5311eab5 (Test-Schreiber: Callback-Werte) + work-zusammen 0f7f5a46; erzeugte Dateien neu (engine_api.py, 815 Knoten).
- 8e576119 Konverter: Tabelle der Engine liest Callback-Pins (of = "float(float,float)" – vorher fiel der Pin heraus → begin_roads „its pins differ“);
  Callback als Kopie gemacht (`return s;` → sampler „made here“).
- f877b9e5 Konverter: Lambda mit auto-Eingängen (extract, tiles.cpp) – je Spezialisierung eine Funktion; Lambda-Eingang (ohne Fang) = die Funktion seines Typs.
  Kleinstfall /tmp/c_gl/k.
- FAIL converted_terrain_cpp (real) in 132846: aus der Basis (Terrain_run_job, j.id – d a12da91a fehlte in a137ba34, jetzt drin).
- Läuft: work-c-vergleich-150305-5063 (8e576119; city_stream, terrain).
