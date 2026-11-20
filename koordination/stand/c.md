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

## Stand 15:46 – work-c = 92e43c44 (gepusht), nach Server-Neustart weiter
- 6a7c58bd: Regel Callback – Schleife erlaubt (memory && !loop, wie Bibliotheksregel; Test logic_callbacks „Loops“); Konverter: data() eines
  Engine-Objekts = sein Knoten (begin_roads: road_stream.data – Kosten: eine Kopie des Stroms je begin_roads, bis „Platz der Engine“ (d));
  HeightSampler{} = ungebunden; Fix f877b9e5 (drop außerhalb local_one → NameError). Kleinstfälle /tmp/c_gl/k, /tmp/c_gl/r.
- 92e43c44 Merge work-zusammen 4ac2e4f9.
- 150305 (8e576119): sampler geschrieben; Check-Fehler „needs the world (For Loop)“ → behoben in 6a7c58bd; begin_roads wartete an data().
  fail/go_offline/finish_features/update warten in der Probe an Funktionen anderer Dateien (training_level, road_is_water, request_around→unproject).
- Laufen: work-c-vergleich-154541-25809 (city_stream, terrain, tiles), CI 44547268243 (Engine-Suite).

## Stand 16:30 – work-c = 9c1907e8 (gepusht; mit work-zusammen fb51251f)
- 154541 (92e43c44; city_stream, terrain, tiles): convert rc=0, Terrain::sampler geschrieben und verglichen (ok), extract (tiles) geschrieben;
  Test city_stream übersetzte nicht: converted.h – Callback-Überladungen (take/arg …) erst nach den Vorlagen deklariert (ADL findet sie in std/kern
  nicht) → behoben 2a… (Vorwärtsdeklarationen); road_stream.data ohne Stellvertreter („no values of double *“) → behoben (const T* = gehaltene Zahlen).
  Lokal (city_stream+terrain, STAND_INS=1): kein „no stand-in“ mehr, Test übersetzt.
- Offen: Terrain_plan_dem 62/300, plan_windows 43/300 (beide Durchgänge, „no place named“) – im vollen Lauf 100601 ok. Ursache prüfen:
  Basislauf work-zusammen (terrain, tiles, city_stream) abgelegt.
- BEFUND s5 (ShaderExtension, App-Ereignisse) notiert – nach den city_stream-Funktionen.
- 16:43 Basis-Beleg work-zusammen-vergleich-162208-17909 (fb51251f; city_stream, terrain, tiles): ROT aus der Basis – CityStream_feed_terrain 9/32,
  merge_damage 2/300, Terrain add_water_polygon 17/84, add_waterway 2/22, build_collision_tile 20/20, build_render_tile 10/20, plan_dem 62/300,
  plan_images 39/300, plan_windows 43/300, set_water_cell 300/300 (real ähnlich). plan_dem/plan_windows in work-c 154541 mit denselben Zahlen → nicht von c.
  → BEFUND FÜR Koordinator/k (Basis seit 4ac2e4f9/fb51251f; im vollen Lauf 100601 waren plan_* ok).
- CI 44547268243 (92e43c44): native, build, determinism, input grün; e2e rot nur an „cannot convert int to int64 (voice)“ (Basis, siehe stand/e.md).

## BEREIT ZUM MERGEN: 9c1907e8 (work-c, enthält work-zusammen fb51251f) – Belege
- work-c-vergleich-164339-32470 (9c1907e8; city_stream, terrain, tiles): convert/bau rc=0; **CityStream::begin_roads, relay, finish_features,
  Terrain::sampler geschrieben UND verglichen, beide Durchgänge ohne Abweichung**. Rot nur, was die Basis genauso rot hat (Beleg
  work-zusammen-vergleich-162208-17909 auf fb51251f): CityStream_feed_terrain 9/32, Terrain_plan_dem 62/300 (real 67), plan_windows 43/300 (real 49)
  – dieselben Zahlen; die Basis hat dazu noch mehr Rotes (set_water_cell, build_collision_tile …), das in work-c nicht auftritt.
- Engine-Suite: CI 44547268243 (92e43c44, alle Sprach-/Test-Änderungen von c): native, build, determinism, input grün; e2e rot nur an
  „cannot convert int to int64 (voice)“ (Basis, stand/e.md); native-spawncity abgebrochen (Zeit). Danach nur Konverter/converted.h + Merge fb51251f.
- Das FAIL converted_terrain_cpp (real) aus 132846: Basis (Terrain_run_job/j.id, d a12da91a fehlte damals).
- Noch wartend (kein Funktionswert): CityStream::fail, go_offline, request_around (→ update): „an owned value that holds objects of the engine,
  replaced where it is no place“ (Map von unique_ptr<Tile> mit Engine-Objekten – Besitz/Destruktoren, nicht c). extract: Beleg läuft
  (tiles+mvt, layer() steht in mvt.cpp).

## Stand 18:50 – work-c = ce74f7d4 (gepusht)
- 0ba56431: Merge work-zusammen a424629f – Lambda mit auto-Eingängen nur noch in k's Fassung (c's entfernt; doppelt gebaut). Gemeldet.
- 174554 (9c1907e8; tiles, mvt): extract geschrieben, 8/8 ok in beiden Durchgängen.
- 180303 (0ba56431; city_stream, terrain): begin_roads/relay/sampler geschrieben; Stellvertreter-Durchgang: city_stream UB „offset 16 to null
  pointer“ (Quelle liest size() Einträge aus data() des Stellvertreters, der beide getrennt beantwortet) → ce74f7d4 converted::room.
  terrain (beide Durchgänge): UB „reference binding to null pointer CorridorSet“ nach Terrain_normal (auch schon 164339, nicht in der Basis 162208):
  der Test von Terrain_sampler ruft den Callback der Quelle auf dem gezogenen Terrain; gezogen werden auch leere unique_ptr in corridors_, die
  Terrain::corridor_set (`*corridors_[i]`) ohne Prüfung liest. **BEFUND FÜR t1:** ein leerer unique_ptr, den die Quelle dereferenziert, ist
  ein Ort, den C++ offen lässt (wie ein Eintrag außerhalb der Liste) – der Rahmen fängt ihn nicht (open_read) → Abbruch durch UBSan.
- Läuft: work-c-vergleich-184821-26350 (ce74f7d4; city_stream, terrain).
- 19:10 b835e56d: converted::room gibt alle gehaltenen Zahlen weiter (184821: begin_roads 161/162 – Quelle [x], Blueprint [x, y]). Läuft 190838.
- **Befund 184821 (ASan stack-use-after-scope):** der Stellvertreter behält die Callbacks eines Engine-Objekts (RoadBuilder 6) über den Testaufruf
  hinaus und ruft sie beim nächsten Aufruf auf dem Objekt; der Callback der KLASSE zeigt dann auf das CityStream-Objekt des beendeten Aufrufs
  (Callback {fn, place} ohne Lebenszeichen) → Terrain_deformed liest einen toten Stack. Im Interpreter wird der Callback mit dem Ende der Instanz
  ungebunden; in der Klasse nicht. = offener Punkt „behaltener Callback nach Ende der Klasse“ – nötig: NativeCallable hält ein Lebenszeichen des
  Objekts, dessen Platz gebunden ist (Klasse: Lebenszeichen je Blueprint-Objekt), sonst ungebunden wie im Interpreter.

## Stand 01:30 (20.11.) – work-c = 4bca8f52 (gepusht; enthält work-zusammen d5f527c5)
- 190838 (b835e56d): begin_roads 160/162 – der Blueprint-Seite rief der Stellvertreter zusätzlich Callbacks, die RoadBuilder 6 aus einem FRÜHEREN
  Aufruf behielt (kept über Aufrufe hinweg; Seiten verschieden; Klasse: Platz tot → ASan stack-use-after-scope). → a6127013: converted.h leert
  stand_in.kept je verglichenem Aufruf (Quelle und Blueprint), wie calls. Beleg: work-c-vergleich-001825-8576 (a6127013) – läuft seit 00:18,
  mag hängt offenbar (auch work-p-vergleich-001918, work-t1-probe-002234 seit 00:19 „läuft“) → Koordinator.
- 4bca8f52: Konverter `m[key] = std::move(v)` eines besessenen Werts mit Engine-Objekten (request_around, go_offline): Eintrag per operator[]
  (map_add), Altes beendet, ersetzt. Kleinstfall /tmp/c_gl/m richtig. Unbelegt auf dem Spiel (nächster vergleich city_stream, terrain).
- Offen: terrain UB (leerer unique_ptr in corridors_, BEFUND t1); register_node (game.cpp: eigener Knoten der Bibliothek zur Laufzeit, Lambda
  ruft game().fixed_step) und s5-BEFUND (ShaderExtension, App configure/start/update/stop) = „die Engine ruft ein Ereignis/eine Funktion eines
  Blueprints“ – Entwurf nötig (Ereignis des Game-Blueprints statt NodeDef zur Laufzeit), nicht begonnen.

## Stand 08:05 (20.11., Nachfolger) – work-c = 42e6e45e (gepusht; enthält work-zusammen bdb99729, engine_api.py neu erzeugt: unverändert)
- Rot im vollen Lauf 045421: terrain UBSan (leerer unique_ptr in corridors_) = der Callback der Quelle (Terrain::sampler) lief erst im Vergleich
  (Tally::take → same), außerhalb des Aussprungs g_bail → present() konnte nicht aufgeben. **42e6e45e (converted.h, c ändert die Stelle; t1 informiert):**
  defined() eines Callbacks ruft ihn an den Stellen von same_call mit eigenem Aussprung; offen gelesen / Zahlen verlassen → nicht verglichen, neu gezogen.
  Kleinstfall /tmp/c_gl/df richtig. city_stream: offset-16 (Stellvertreter) und heap-buffer-overflow in RoadBuilder::begin (Engine) = b835e56d/ce74f7d4
  (converted::room), unbelegt. Der ASan heap-use-after-free danach ist converted_Game__game_cpp (Car::detach), nicht c.
- 080407 verloren (mag-Neustart 08:06); neu abgelegt (siehe unten). work-c-vergleich-001825-8576 (a6127013) hängt seit 00:18 als „läuft“ –
  abbrechen wurde mir verwehrt (Koordinator bitte beenden).

## Aufgabe 2 – Entscheidung (c, 20.11. 08:30; nach den Zielen, ohne Freigabe – Koordinator: „entscheide selbst“)
Alle drei Stellen (s5-BEFUND 1 und 3, register_node) sind „die Engine ruft ein Objekt des Programms, das eine Schnittstelle der Engine erfüllt“ –
das ist NICHT der Callback (Teil 1: Funktion einer Bibliothek, Werte, ohne Welt), sondern eine eigene, allgemeine Fähigkeit:
**E-c2 „Blueprint als Umsetzung einer Schnittstelle der Engine“.** C++: `class X : public kern::<Schnittstelle>` mit `override`s
(kern::App, kern::ShaderExtension; jede Klasse der Engine mit virtuellen Funktionen ohne Daten). Blueprint: Einstellung `implements = <Schnittstelle>`,
die Overrides sind Funktionen des Blueprints mit den Namen/Signaturen der virtuellen Funktionen (check: jede reine virtuelle umgesetzt, Signatur gleich,
nichts sonst als override). Engine: je Schnittstelle ein erzeugter Adapter (engine_api.py, aus dem Header – keine Namensliste), der jede virtuelle
Funktion auf `Instance::call` (Interpreter/JIT) bzw. die Methode der Klasse (Export: die Klasse erbt direkt, volle Leistung) abbildet; Objekte der Engine
in der Signatur (Engine&, Frame, Program, UniformCache, Renderer) = Handles wie in der Tür. Angemeldet wird ein solcher Blueprint, wo C++ das Objekt
übergibt (App::extensions gibt die Liste der Objekte; Endzustand s5: das Level nennt den App-Blueprint).
**register_node** (game.cpp, NodeDef kind=event, event „logic.tick“, accepts immer, run ohne Fang ruft game().fixed_step, Knoten in build_level):
Entwurf des Vorgängers bestätigt – ein zur Laufzeit angemeldeter Knotentyp ist im Blueprint kein Wert, sondern ein **Ereignis des Blueprints, der den
Zustand hält** (Game): Event Tick → fixed_step(1/kStepHz). Die Anmeldung (NodeDef + library().add) und der gelegte Knoten sind zusammen dieses Ereignis;
der Konverter liest das allgemein: ein NodeDef des Programms mit festen Feldern, `kind = event`, festem `event`, `accepts` immer wahr, `run` ohne Fang,
der mit kStop endet → Ereignis dieses Typs im Graphen, in den das Programm den Knoten legt. Voraussetzung: der Graph ist ein Blueprint (build_level/Level
als Daten – s5 Endzustand). Reihenfolge der Tick-Handler (Runtime::fire, nach Instanz) muss gleich bleiben – Teil des Belegs.
**Bau:** nicht begonnen (Aufgabe 1 hatte Vorrang; Umfang: Sprache+Engine-Adapter+Konverter+Export+Tests+Editor, mehrere Stunden). Reihenfolge:
(1) E-c2 Sprache/Adapter mit Engine-Test (Fall ShaderExtension mit name()/vertex() als Text, App mit start/update/stop), nativer Fall;
(2) Konverter: Klasse, die von einer Schnittstelle der Engine erbt; (3) register_node, sobald build_level schreibbar ist (s5/g1).

## Stand 09:18 – work-c = 45c54236 (gepusht)
- work-c-vergleich-083848-27171 (42e6e45e): city_stream, city_stream_cpp ok in BEIDEN Durchgängen (b835e56d/a6127013/4bca8f52 damit belegt);
  terrain_cpp: kein UBSan mehr (42e6e45e wirkt), aber ASan heap-use-after-free: Callback der KLASSE aus Terrain_sampler zeigt auf den Referenz-Eingang
  des Klebers (lokal, mit dem Aufruf beim Namen beendet). → 45c54236 (notation.cpp native_glue): Referenz-Eingänge als Glieder des _Native-Objekts
  (wie die Variable des Aufrufs im Interpreter), busy_ für Aufruf mitten im eigenen.
- Laufen: work-c-vergleich-091714-16925 (45c54236; city_stream, terrain), CI 44701329842 (Engine-Suite für den Kleber).

## Stand 09:35 – work-c = 53c9ac69 (gepusht; enthält work-zusammen 8971e5c3)
- **work-c-vergleich-091714-16925 (45c54236; city_stream, terrain): 11 ok von 11 in BEIDEN Durchgängen (Stellvertreter und SC_ENGINE=real), 0 FAIL/ABBRUCH/LECK/ZEIT,
  keine Abweichung.** Belegt: b835e56d, a6127013, 4bca8f52, 42e6e45e, 45c54236 (Kleber: Referenz-Eingänge als Glieder → der Callback der Klasse aus Terrain_sampler lebt).
- 53c9ac69 Merge 8971e5c3: converted.h in EINER Fassung – t1's probe_source behalten, c's callback_defined (doppelt gebaut) entfernt.
- Laufen: work-c-vergleich-093342-22177 (53c9ac69; nach dem Merge), CI 44701329842 (45c54236): determinism, build, input grün; native/e2e laufen.

## BEREIT ZUM MERGEN: work-c 53c9ac69 – Beleg work-c-vergleich-093342-22177
- vergleich city_stream + terrain auf 53c9ac69 (enthält work-zusammen 8971e5c3): **11 ok von 11 in beiden Durchgängen** (Stellvertreter, SC_ENGINE=real),
  0 FAIL/ABBRUCH/LECK/ZEIT, keine Abweichung. Vorher (Lauf 045421) rot: converted_city_stream_cpp, converted_terrain_cpp.
- Commits seit work-zusammen: b835e56d, ce74f7d4 (converted::room), a6127013 (kept je Aufruf leeren), 4bca8f52 (m[key]=move), 45c54236 (Kleber: Referenz-
  Eingänge als Glieder des _Native-Objekts), Merges. (42e6e45e durch t1's probe_source ersetzt.)
- CI 44701329842 (45c54236): native, build, determinism, input grün; e2e rot nur an „cannot convert int to int64 (voice)“ (game_equipment_cpp,
  sound_fx_cpp) und Folge „Play runs them as machine code“ = Basis (stand/e.md); native-spawncity lief noch.
