# SpawnCity als Editor-Projekt: der Weg

Ziel: SpawnCity ist ein Projekt des Kern-Editors – ein Level, Blueprints und Daten – und wird vom
Editor exportiert. Es enthält kein eigenes C++ mehr. Was heute in `src/` steht, ist dann entweder
ein System der Engine (für jedes Kern-Spiel nutzbar, im Editor als Komponente und als
Blueprint-Knoten), ein Blueprint (Spielregeln, Ablauf) oder ein Datensatz im Level.

Das beweist, was die Engine können muss, um ein Spiel dieser Größe ohne Code zu bauen – und es
hält SpawnCity von selbst im Gleichschritt mit der Engine, weil es nichts Eigenes mehr gibt, das
auseinanderlaufen könnte.

spawncity.de bleibt bei jedem Schritt das exakte Drop-in: Jede Stufe ist für sich lauffähig,
getestet und live.

## Wo SpawnCity heute steht

- Es läuft schon auf der Welt der Engine (`kern::game::GameWorld`): Spieler (Kerns Soldat),
  Drohne, Jet, Fallschirm und Autos sind Actors eines Levels, das `build_level()` im Code baut.
- Dieses Level hat schon einen Level-Blueprint; er ruft in jedem Physikschritt einen Knoten des
  Spiels (die Kraftmodelle der Fahrzeuge).
- Das Level beginnt als Datei (`level/spawncity.klevel`): die Welt-Einstellungen und die Regeln,
  die schon Blueprints sind. `SpawnCity.command("blueprints")` in der Browser-Konsole zeigt, welche laufen und wie
  (Interpreter, kompiliertes WebAssembly).
- Alles andere ist C++ des Spiels: zu Beginn 31.549 Zeilen in 75 Dateien (ohne Header), dazu die
  Seite (`web/`).

## Inventar: was wohin geht

| Teil | Dateien | Zeilen | Ziel |
|---|---|---:|---|
| Reine Regeln (Formeln und Tabellen des Originals) | `wanted`, `combat_rules`, `combat_detail`, `mp_rules`, `gaps_rules`, `guided`, `spawn_guard`, `jet_seats`, `lamps`, `world_traffic`, `controls`, `settings`, `equipment` | 2.643 | Blueprint-Funktionen und Datentabellen |
| Spielablauf (was wann passiert) | `game`, `game_town`, `game_events`, `game_military`, `game_gaps`, `game_weapons`, `game_foot_rounds`, `game_equipment`, `game_details`, `game_guided`, `game_barricades`, `game_seats`, `game_traffic`, `game_traffic_far`, `game_vehicle_damage`, `game_lamps`, `game_life`, `game_world`, `game_ui`, `game_controls` | 11.159 | Level- und Actor-Blueprints; was darin System ist, wandert mit dem System |
| Welt | `city_stream`, `tiles`, `mvt`, `terrain`, `interiors`, `destruction`, `world_marks`, `road_graph`, `city_jobs`, `facades`, `water_look` | 5.351 | Engine: Komponente „Kartenwelt“ (löst die vereinfachte „Procedural City“ ab) |
| Fahrzeuge | `car`, `drone`, `jet` | 1.138 | Engine: Player-Controller Vehicle / Drone / Jet mit diesen Modellen |
| Leben | `life`, `people`, `ragdoll` | 2.548 | Engine: Passanten, Tiere, Ragdoll als Komponenten |
| Mehrspieler | `net_session`, `game_net`, `game_worldsync`, `game_destruction_sync`, `game_projsync`, `game_toss`, `game_swarm` | 3.713 | Engine: Replikation der Komponenten (`net_game`) |
| Eingabe, Bild, Ton | `touch_play`, `look`, `game_look`, `perf`, `game_vr`, `sound_fx`, `combat_audio`, `game_audio_web` | 2.972 | Engine: Touch-Layout, Kamera/Look, VR, Sound |
| Modelle | `combat_art`, `life_art`, `people_art`, `vehicle_art`, `mp_art`, `world_art` | 1.234 | Engine: prozedurale Meshes im Katalog |
| Hilfen | `nostr`, `qr_code`, `gps_live` | 631 | Engine-Bibliothek |
| Einstieg | `main` | 160 | entfällt (Host des Editors) |

Die Seite (`web/`: Ansicht des Originals, Menüs, Ton des Originals) wird das Seitenmodul des
Projekts: Der Export des Editors nimmt es mit.

## Stufen

Jede Stufe endet live. „Fertig“ heißt: die Tests der Stufe sind grün, die drei Ausführungsarten
der Blueprints (Interpreter, JIT, exportiertes C++) liefern dasselbe, und der Eingabe- und der
Editor-Test der CI laufen durch.

### 0 – Fundament
- [x] **Blueprint-Funktionen:** Funktionen mit typisierten Eingängen und Rückgabewerten
  (Function Entry, Return, Call, pure Aufrufe), in allen drei Ausführungsarten, im Editor
  anlegbar; aus C++ aufrufbar (`logic::Instance::call`).
- [x] **Das Level als Datei:** `level/spawncity.klevel` ist das Projekt. `build_level()` beginnt
  mit dieser Datei (Welt-Einstellungen, Regel-Actors) und ergänzt, was noch im Code gebaut wird.
  Die Datei öffnet sich im Editor; die CI prüft das (`kern/tests/e2e/blueprint_e2e.mjs`).
- [x] **Erstes Regelwerk als Blueprint:** die Fahndung (`wanted`) – Actor „Wanted“ im Level mit
  19 Funktionen und den Variablen Heat / HiddenMs. Die Formeln sind aus dem C++ gelöscht;
  `wanted.h` ist nur noch die Art, wie der übrige Code den Blueprint aufruft. Die Unit-Tests
  prüfen den Blueprint gegen die Tabelle des Originals (`tests/test_rules.cpp`).

So entsteht ein Regelwerk: ein leerer Actor im Ordner *Rules* des Levels, sein Blueprint mit einer
Funktion je Regel (meist Entry → Formel → Return), der Zustand als Variablen. Der C++-Code, der
die Regel noch braucht, ruft die Funktion über ihren Namen (`Instance::call_number`); verschwindet
auch dieser Code in einen Blueprint, entfällt der Aufruf.

### 1 – Blueprint ⇄ C++, exakt und umkehrbar

Vor allen weiteren Regeln. Ein Blueprint und sein C++ sind dasselbe in zwei Schreibweisen
(`kern/docs/blueprint_cpp.md`): gespeichert wird der Blueprint, das C++ ist die zweite, ebenfalls
änderbare Ansicht – eine Quelle, kein Auseinanderlaufen.

- [x] **Die Schreibweise, hin:** Ein Blueprint ist eine C++-Klasse (`logic::to_cpp`). Variablen
  sind Member, Funktionen sind Funktionen, Events sind Methoden, Branch ist `if`, Schleifen sind
  Schleifen, eine Formel ist der Ausdruck selbst, jeder Knoten ist ein Aufruf seiner Funktion. Im
  Text steht kein Rückruf in eine Knoten-Registry.
- [x] **Der Rückweg:** Dieses C++ wird wieder zum selben Blueprint eingelesen (`logic::from_cpp`),
  auch von Hand geschriebenes (frühes `return`, `else if`, Anweisungen nach einem `if`). Was nicht
  zur Schreibweise gehört, wird mit Zeile, Spalte und Grund abgelehnt.
- [x] **Der Beweis für die Schreibweise:** Für jeden Knotentyp der Bibliothek und jeden
  Test-Graphen ergibt Blueprint → C++ → Blueprint denselben Blueprint, der Text ist ein Fixpunkt,
  und der zurückgelesene Blueprint läuft genau gleich (`kern/tests/run.sh logic_notation`; für
  „Wanted“: `rules_wanted_blueprint_is_its_cpp`).
- [x] **Die Regeln dazu:** Namen sind C++-Namen, ein Exec-Ausgang hat einen Draht, Return liegt
  auf einem Weg seiner Funktion, ein Zufallswert hat einen Leser; kein Weg führt ohne Warten in
  sich selbst zurück, keine Funktion ruft sich selbst auf, eine Funktion, die als Wert gelesen
  wird, rechnet nur, Add to Variable zählt im Typ der Variablen. `check()` meldet Verstöße, der
  Editor nimmt solche Namen nicht an.
- [x] **Die C++-Ansicht im Editor:** **C++** in der Toolbar des Blueprint-Editors zeigt den Text
  des Blueprints; ändern und **Apply** (ein Undo-Schritt) macht ihn zum Blueprint. Bestehende
  Knoten behalten Id, Platz und Kommentar, neue stehen neben ihrem Vorgänger; Fehler nennen Zeile
  und Grund (`logic::apply_cpp`, Befehle `logicCpp` / `logicCppApply`).
- [ ] Die Ansicht komfortabler: Syntaxfarben, Vervollständigung der Knotennamen, neue Knoten
  ohne Überlappung einsortieren.
- [x] **Native Ausführung, Teil 1 – Blueprints aus Funktionen:** Der Text kompiliert gegen
  `kern/logic/blueprint.h`, so wie er dasteht. Was ein Knoten rechnet, steht einmal, als
  typisierte Funktion in `kern/logic/nodes.h`; der Interpreter ruft dieselbe Funktion auf wie der
  Text. Es läuft: Variablen, Funktionen, Formeln, alle Wert-Knoten, Branch, Sequence, For, While,
  Do Once, Do N, Gate, Flip Flop, die Switches – also „Wanted“ ganz. `logic::native_ready` sagt
  für jeden Blueprint, ob sein Text so läuft, und sonst, welcher Knoten fehlt. Beweis:
  `blueprint_native_test` (in `kern/tests/run.sh`) kompiliert die Texte von fünf Blueprints und
  vergleicht über 2000 Aufrufe Zeichen für Zeichen mit dem Interpreter; `wanted_native_test` (in
  SpawnCitys Tests) dasselbe für „Wanted“.
- [x] **Das Spiel führt den Text aus:** Der Build schreibt die Blueprints des Levels als
  C++-Klassen in `src/level_blueprints.cpp` (`tools/blueprint_cpp.sh`: der Text jedes Blueprints,
  der so laufen kann, und der Kleber `logic::native_glue`). `logic::Runtime` führt einen Graphen,
  dessen Klasse im Programm steckt, als diese Klasse aus (Stufe „native C++“); die Engine
  erreicht Funktionen und Variablen beim Namen nur an dieser Grenze. Die Klasse läuft nur für
  genau den Graphen, aus dem sie geschrieben wurde (Hash) – ein geändertes Level fällt auf den
  Interpreter zurück, bis neu gebaut ist. „Wanted“ läuft so im ausgelieferten SpawnCity; die
  Regel-Tests laufen auf der Klasse und vergleichen sie mit dem Interpreter.
- [x] **Events als Methoden der Klasse:** Begin Play, Tick und Custom Event sind Handler
  `on<id>`, die die Laufzeit über den Kleber aufruft – wer welches Event bekommt, entscheidet
  `Runtime::fire` für alle Stufen gleich. Dazu Call Event, Print und Game Time. Ein Handler, der
  sein eigenes Event auslöst, hat je Lauf seine eigenen Werte (im Interpreter wie im Text). Sieben
  der Tier-Fälle laufen so als Klassen und liefern dasselbe wie Interpreter und JIT.
- [x] **Warten und State Machines in der Klasse:** Delay, Retriggerable Delay und Tween sind
  Funktionen von `blueprint.h`, was folgt, ein Lambda; die Wartezeiten führt die Laufzeit für alle
  Stufen an einer Stelle. Every N Seconds und die State Machines (Timer, Bedingungen, Events)
  bleiben ebenfalls bei der Laufzeit – ein Code für jede Stufe –, die Klasse hat die Handler und
  `state_set` / `state_get` / `state_is`.
- [x] **Der Schleifenwächter, für jede Stufe derselbe:** Gezählt werden Schleifendurchläufe je
  Schritt, an einer Stelle (`Runtime::pass`); über der Grenze enden die Schleifen der Kette, der
  Rest läuft zu Ende, dann ist der Blueprint gestoppt. Interpreter, JIT und Klasse hören beim
  selben Durchlauf auf.
- [x] **Random, und Eingänge genau einmal:** Jeder Random-Knoten hat seine eigene Folge (vom
  Seed des Blueprints und seiner Id; `logic::random`), also ist gleich, in welcher Reihenfolge C++
  Argumente auswertet. Jeder Eingang eines Knotens wird genau einmal gelesen (Select liest beide
  Seiten, Tween die Dauer auch bei Stop); der JIT überlässt Knoten, die Zufall lesen, ihrer
  Knotenfunktion. Damit läuft die ganze Kernbibliothek als Klasse; alle 15 Tier-Fälle stimmen in
  jeder Stufe überein.
- [ ] **Native Ausführung, Teil 2 – alles Übrige als Funktionen von `blueprint.h`:**
  - Die Gameplay-Knoten (jeder liest dabei jeden Eingang genau einmal).
  - Eine Funktion, die über ein Event wieder bei sich selbst ankommt: in allen Stufen gleich
    (heute überspringt der Interpreter den inneren Aufruf und meldet es); ebenso Funktionen, die
    mehr als 64 tief ineinander aufrufen (der Interpreter bricht dort ab).
  - Verschachtelung: Der JIT stoppt bei mehr als 64 ineinander liegenden Schleifen / Sequences,
    die anderen Stufen nicht.
  - Danach entfällt der bisherige Export (Schleife über Knotennummern mit `call_node` /
    `read_pin`, heute die Stufe „native C++ (node loop)“).
  - Ganze Zahlen: Der Interpreter hält Zahlen-Variablen als `double` (exakt bis 2^53), der Text
    als 64-Bit-Zahl; der JIT schneidet „keine Zahl“ und unendlich beim Umwandeln nicht auf 0.
    Beides wird gleichgezogen.
- [ ] **Nativer Export:** Der Editor exportiert das Spiel mit diesen Klassen, voll optimiert.
- [ ] Der JIT erzeugt Formeln und Funktionsaufrufe ebenfalls direkt (heute Rückruf).

### 2 – Die Sprache vervollständigen
- [x] Formel-Knoten (Formula): eine Formel als ein Knoten statt als Knotenkette.
- [ ] Listen (Arrays) und „For Each“.
- [ ] Strukturen (benannte Felder) und Datentabellen im Level (im Editor als Tabelle).
- [ ] Aufzählungen (Enum) als Pin-Typ mit Auswahl.
- [ ] Lokale Variablen in Funktionen; Funktionen, die sich selbst aufrufen dürfen.
- [ ] Blueprint-Bibliotheken: Funktionen eines Actors von anderen Blueprints aus aufrufen.

### 3 – Die reinen Regeln als Blueprints
In dieser Reihenfolge (klein nach groß; jede Datei verschwindet, ihre Tests bleiben):
`combat_detail`, `spawn_guard`, `guided`, `jet_seats`, `lamps`, `gaps_rules`, `controls`,
`settings` (Datentabelle), `world_traffic`, `equipment`, `combat_rules`, `mp_rules`.

### 4 – Systeme in die Engine
Je System: Code nach `kern/src/…`, als Komponente im Level und im Editor, Knoten für Blueprints,
Replikation; das vereinfachte Gegenstück der Engine entfällt.
1. Fahrzeuge (Auto mit Reifenmodell, Drohne mit Firmware, Jet)
2. Welt (Kartenwelt: Kacheln, Gelände, Innenräume, Zerstörung, Straßengraph)
3. Verkehr und Leben (Fahrer, Passanten, Tiere, Ragdoll)
4. Waffen, Geschosse, Ausrüstung
5. Mehrspieler
6. Eingabe, Kamera, VR, Ton, Modelle

### 5 – Der Spielablauf als Blueprints
`game_*.cpp` wird zu Blueprints der Actors (Spieler, Polizei, Militär, Abschleppdienst, Ereignisse …)
und des Levels.

### 6 – Export
- [ ] Der Editor exportiert das Projekt mit Seitenmodul; der Export ist das Drop-in für
  spawncity.de (CI vergleicht).
- [ ] `src/` ist leer und wird gelöscht.

## Regeln für jeden Schritt
- Es gibt zu jeder Zeit genau eine Umsetzung einer Regel oder eines Systems – nie C++ und
  Blueprint nebeneinander.
- Werte und Verhalten bleiben die des Originals; die bestehenden Tests wandern mit und prüfen
  danach den Blueprint bzw. das Engine-System.
- Box3D wird nicht getunt; Physik bleibt nach `kern/docs/physics.md`.
