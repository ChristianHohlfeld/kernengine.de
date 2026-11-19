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

## Belege
- Lokal (logic+core gebaut, /tmp/cbk – Kleinstfall): logic_callbacks 0 Fehler; Ground/Site Interpreter = Klasse Zeichen für Zeichen.
- CI 44526990074 (a137ba34): native grün (54577 Prüfungen, blueprint_native 64 Blueprints/14111 Aufrufe inkl. Ground/Site und
  Engine mit RoadOptions-Struct), build grün (logic_wasm_test), determinism grün. native-spawncity/e2e/input liefen noch.
- Ausstehend: mag `work-c-vergleich-132846-9096` (city_stream, terrain) – `./mag.sh holen work-c-vergleich-132846-9096`.
  Der mag-Auftrag `work-c-engine-125735-29671` ist abgebrochen (die CI hat die Engine-Suite gerechnet).
- Lokaler Kleinstfall-Bau: /tmp/cbk (build.sh, re.sh: logic+core + Test-Hauptprogramm; nat.cpp/cls.cpp: Ground/Site als
  Interpreter und als übersetzte Klasse) – nicht im Repo.

## Unbewiesen / offen (in dieser Reihenfolge)
- **Engine-Suite**: `work-c-engine-125735-29671` rechnet f54c7738+Test (nur Schritt 1/2); danach eine Engine-Suite auf dem
  neuesten Stand (nativer Fall, every_node, Generator) – Ergebnisse in stand/c.md.
- Regel „kein Listeneintrag (Liste ohne feste Länge) und kein Map-Eintrag auf dem Weg des gebundenen Platzes“ gebaut (87163668),
  nur am Weg des Knotens selbst – über die Referenz-Eingänge der Aufrufer hinweg nicht (wie C++: der Aufrufer nennt den Platz).
- Weltregel für Funktionen einer anderen Bibliothek: nur wo ihr Rumpf im Graphen ist (gelinkt); ungelinkt prüft sie niemand.
- Klasse ohne RTTI: NativeCallable wird an `Callable::native_kind` erkannt.
- Risiko Stellvertreter: ein behaltener NativeCallable (Klasse durch die Tür) zeigt nach dem Ende der Klasse ins Leere, wenn
  das Objekt der Engine nicht freigegeben wurde – NativeCallable sollte das Lebenszeichen der Instanz halten.
- **Test-Schreiber (Quelle im Test)**: HeightSampler/RoadOptions sind jetzt Werte. Die Stubs der Quelle brauchen den echten
  Typ (HeightSampler {fn,user}, RoadOptions mit std::function) und `arg()` → `Value::callback(Callable um den C++-Zeiger /
  die std::function)`; `same()` eines zurückgegebenen Callbacks (Terrain::sampler) = beide rufen und vergleichen. Stelle:
  stand_ins() ~Z. 11580 cpp_to_blueprint.py, converted.h (arg/same).
- **Konverter (das Spiel)**: (a) GEBAUT (Emit.callback_lambda, Aufruf in Emit.ex bei want = Callback<…>): `ro.deformed = [this](f64 x, f64 y, f64 r) { … }` → Lambda heben zu einer Funktion der
  Bibliothek der Datei mit `const CityStream& self` vorn, `ro.deformed = callback<&lib::fn>(self)`; Captures nur `this`.
  (b) GEBAUT (Emit.callback_field_set, aus Emit.effect): Terrain::sampler – `s.user = this` merkt den Platz, `s.fn = Lambda
  (void* u, …)` hebt das Lambda (der Rumpf als Kopie, `static_cast<const T*>(u)->` → `u.`), `s = callback<&lib::fn>(self)`.
  Ein Callback ohne Argumente gemacht = ungebunden (Emit, „made here“). Lokale Probe (nur 2 Dateien, Level von
  work-zusammen): sampler wartet nur an Terrain::height, begin_roads an road_palette (beide andere Datei) – Beleg über das
  Level: mag `work-c-vergleich-132846-9096` (abgelegt 13:28, rechnet den Stand beim Start).
- Teil 2 Jobs (`KERN_JOB(kind, &Lib::fn);`, jobs::Fn ohne void*) – nicht begonnen.
- engine_api.py: nur `tag()` (3 Zeilen) berührt; Paket d arbeitet dort – beim Einmischen auf Konflikte in den erzeugten
  Dateien achten (neu erzeugen statt mischen).

## Entwurf 18.11. (Kurzform)
Stellen: HeightSampler (RoadBuilder::begin behält, build_terrain_tile ruft während), RoadOptions::deformed (std::function),
jobs::Fn (Teil 2). Spiel: Terrain::sampler, CityStream::begin_roads (daran finish_features, relay, work, update);
fail/go_offline warten an anderem (remove/erase). register_node (game.cpp) ist kein Rückruf (Stufe 6, eigener Entwurf).

## Nachfolger 19.11. (ab 15:00) – Stand 18:25, work-c = 0ba56431
- Gebaut: Tabelle der Engine liest Callback-Pins (begin_roads „pins differ“); Callback als Kopie / `HeightSampler{}` (ungebunden); data() eines
  Engine-Objekts = sein Knoten (road_stream.data; eine Kopie des Stroms je begin_roads – entfällt mit „Platz der Engine“, d); Regel Callback:
  Schleife erlaubt (memory && !loop, wie Bibliotheksregel; Test logic_callbacks „Loops“); Test-Schreiber (converted.h): Callback-Überladungen vor
  den Vorlagen deklariert (ADL), `const T*`-Ergebnis eines Stellvertreters (gehaltene Zahlen); Spell `const f64*`.
- Lambda mit auto-Eingängen: k hatte dieselbe Fähigkeit gebaut → beim Merge a424629f nur k's Fassung behalten (c's local_template entfernt).
- Belege: work-c-vergleich-164339-32470 (9c1907e8): begin_roads, relay, finish_features, Terrain::sampler geschrieben+verglichen, ohne Abweichung;
  Rot nur aus der Basis (work-zusammen-vergleich-162208-17909: feed_terrain, plan_dem, plan_windows – gleiche Zahlen). 174554 (tiles+mvt): extract
  geschrieben, 8/8 ok in beiden Durchgängen. CI 44547268243: native/build/determinism/input grün, e2e nur „int64 (voice)“ (Basis).
  Ausstehend: work-c-vergleich-180303-21611 (0ba56431; city_stream, terrain) – Beleg nach dem Merge.
- Wartet, kein Funktionswert: CityStream::fail, go_offline, request_around (→ update): „an owned value that holds objects of the engine, replaced
  where it is no place“.
- Offen (später, soweit das Spiel es braucht): NativeCallable hält das Lebenszeichen der Instanz nicht; JIT-Fall mit Bibliothek; Jobs (KERN_JOB);
  BEFUND s5: ShaderExtension (Fassaden/Wasser), App-Ereignisse – Engine ruft Funktionen/Ereignisse eines Blueprints (eigener Entwurf nötig).
- Kleinstfälle: /tmp/c_gl/k (generisches Lambda), /tmp/c_gl/r (RoadStream/RoadBuilder), /tmp/c_gl/cs (lokal city_stream+terrain, STAND_INS=1).
