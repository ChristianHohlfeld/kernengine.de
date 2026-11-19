# Paket c – Übergabe (19.11.2026, Bau begonnen)

Zweig **work-c** (Worktree /home/claude/w-c), Basis origin/work-zusammen 6998a56e. Entwurf E2 (bestätigt) gilt; die
Entscheidungen beim Bau stehen in stand/c.md. Der Entwurf vom 18.11. (Stellen, Begründung) steht unten in Kurzform.

## Gebaut (Commits auf work-c, alle gepusht)
1. f54c7738 – Sprache: `PinType::callback` (hinter set), `TypeRef::of` = Signatur `float(float,float)` / `(float)`;
   `Value::call` (`Callable`, leer = ungebunden), `call_back`, `callback_of/result/inputs`; Knoten `func.callback`
   (Make Callback, core_nodes.cpp, Ende von add_functions); `Instance::callable` → `InstanceCallable` (Instanz nach Nummer,
   Funktion, Spot; Eingänge in die Typen der Funktion gewandelt); check (logic.cpp, Block „Make Callback“ vor find_pin):
   Funktion einer Bibliothek, ≤1 Ausgang, Werte Bool/Int/Float/Vector/Int64, transitiv `NodeDef::world` frei (neues Flag:
   Engine-Knoten, Print, Game Time, Random, Call Event/on Actor/Get on/Has Blueprint; dazu latent, raises, memory, Events,
   alles außerhalb Library::core). Text (notation.cpp): Typ `Callback<Float(Float, Float)>`, Wert `Callback<…>{}`,
   Ausdruck `callback<&Lib::fn>(platz)` (Schreiber in pure(), Leser vor `g.function(t.s)` in value()). Klasse: nodes.h
   `logic::Callback<R(A…)>` {fn, place}, `NativeCallable`, Held/to_value; blueprint.h `the_library<L>`, `callbacks::bound/free`,
   `BlueprintCore::callback<F>(place)`. Tür (engine_host.h): Tag `ev::Callback<…>`, `Conv` (Trampolin `Thunk` / std::function),
   `EngineObjects::hand/kept_by` (call_engine behält die Callbacks einer Methode eines Objekts mit dem Objekt, je Funktion#n).
   Direkt (engine_direct.h): fn/place Feld für Feld (static_assert gleicher Typen) bzw. std::function.
2. Test `logic_callbacks` (test_main.cpp, vor logic_libraries).
3. Generator: cpp_to_blueprint.py `callback_record/callback_sig/callback_type` (Unit), `callback_parts/callback_of`
   (frei), btype/zero/Api.pin/Api.shape; engine_api.py Tag. Neu erzeugt: HeightSampler kein Handle, RoadOptions Struct
   (Feld deformed), 814 Knoten. EngineStandIn ruft Callbacks (dreimal, gewürfelt, in Aufzeichnung und Antwort; behalten
   je Objekt, bei jedem späteren Aufruf auf dem Objekt wieder gerufen, `.release` lässt los).
4. 8645f642 – nativer Fall Ground/Site (blueprint_native.h; native_show ruft Callbacks mit 1.5, -2.25, 4), Doku-Abschnitt
   „Rückrufe der Engine (Callback)“, Editor (Palette logic_edit.cpp, Farbe/Typ web/studio/logic.mjs), every_node lässt
   func.callback aus.

## Unbewiesen / offen (in dieser Reihenfolge)
- **Engine-Suite**: `work-c-engine-125735-29671` rechnet f54c7738+Test (nur Schritt 1/2); danach eine Engine-Suite auf dem
  neuesten Stand (nativer Fall, every_node, Generator) – Ergebnisse in stand/c.md.
- Bekannte Lücke der Regel: „kein Listeneintrag auf dem Weg des gebundenen Platzes (auch über Referenz-Eingänge der Aufrufer)“
  ist NICHT gebaut (Site bindet absichtlich Fields[1] – in C++ ein Zeiger auf den Eintrag, im Interpreter der Spot mit Index:
  gleich, solange die Liste nicht wächst). Entscheiden: verbieten (check) oder so lassen und dokumentieren.
- Weltregel für Funktionen einer anderen Bibliothek: nur wo ihr Rumpf im Graphen ist (gelinkt); ungelinkt prüft sie niemand.
- Risiko Stellvertreter: ein behaltener NativeCallable (Klasse durch die Tür) zeigt nach dem Ende der Klasse ins Leere, wenn
  das Objekt der Engine nicht freigegeben wurde – NativeCallable sollte das Lebenszeichen der Instanz halten.
- **Test-Schreiber (Quelle im Test)**: HeightSampler/RoadOptions sind jetzt Werte. Die Stubs der Quelle brauchen den echten
  Typ (HeightSampler {fn,user}, RoadOptions mit std::function) und `arg()` → `Value::callback(Callable um den C++-Zeiger /
  die std::function)`; `same()` eines zurückgegebenen Callbacks (Terrain::sampler) = beide rufen und vergleichen. Stelle:
  stand_ins() ~Z. 11580 cpp_to_blueprint.py, converted.h (arg/same).
- **Konverter (das Spiel)**: (a) `ro.deformed = [this](f64 x, f64 y, f64 r) { … }` → Lambda heben zu einer Funktion der
  Bibliothek der Datei mit `const CityStream& self` vorn, `ro.deformed = callback<&lib::fn>(self)`; Captures nur `this`.
  (b) Terrain::sampler: `s.user = const_cast<T*>(this); s.fn = [](void* u, …) { static_cast<const T*>(u)->… }` →
  `return {callback<&lib::fn>(self)}` (Feld für Feld zu einem Wert des Callback-Structs). Beides allgemein: „ein Lambda an
  einem Platz vom Typ Callback“. Danach vergleich über city_stream, terrain (beide Durchgänge).
- Teil 2 Jobs (`KERN_JOB(kind, &Lib::fn);`, jobs::Fn ohne void*) – nicht begonnen.
- engine_api.py: nur `tag()` (3 Zeilen) berührt; Paket d arbeitet dort – beim Einmischen auf Konflikte in den erzeugten
  Dateien achten (neu erzeugen statt mischen).

## Entwurf 18.11. (Kurzform)
Stellen: HeightSampler (RoadBuilder::begin behält, build_terrain_tile ruft während), RoadOptions::deformed (std::function),
jobs::Fn (Teil 2). Spiel: Terrain::sampler, CityStream::begin_roads (daran finish_features, relay, work, update);
fail/go_offline warten an anderem (remove/erase). register_node (game.cpp) ist kein Rückruf (Stufe 6, eigener Entwurf).
