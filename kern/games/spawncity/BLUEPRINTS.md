# SpawnCity als Editor-Projekt: der Weg

Ziel: SpawnCity ist ein Projekt des Kern-Editors – ein Level, Blueprints und Daten – und wird vom
Editor geladen und exportiert. Es enthält kein eigenes C++ mehr.

Der Weg dahin ist die Sprache selbst: Ein Blueprint und sein C++ sind dasselbe in zwei
Schreibweisen (`kern/docs/blueprint_cpp.md`); die Schreibweise ist ein Teil von C++. Was heute in
`src/` steht, ist gewöhnliches C++ und benutzt mehr von der Sprache als diesen Teil. Es wird in
die Schreibweise umgeschrieben – Datei für Datei, Funktion für Funktion, von einem Konverter
(`tools/cpp_to_blueprint.py`) – und ist danach ein Blueprint des Levels. Nichts von SpawnCity
wandert in die Engine. Die Engine und die Sprache bekommen nur, was allgemein ist: Fähigkeiten
der Sprache (Werte, Knoten), die dem Konverter noch fehlen, um eine Stelle zu schreiben.

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

| Teil | Dateien | Zeilen |
|---|---|---:|
| Reine Regeln (Formeln und Tabellen des Originals) | `wanted`, `combat_rules`, `combat_detail`, `mp_rules`, `gaps_rules`, `guided`, `spawn_guard`, `jet_seats`, `lamps`, `world_traffic`, `controls`, `settings`, `equipment` | 2.643 |
| Spielablauf (was wann passiert) | `game`, `game_town`, `game_events`, `game_military`, `game_gaps`, `game_weapons`, `game_foot_rounds`, `game_equipment`, `game_details`, `game_guided`, `game_barricades`, `game_seats`, `game_traffic`, `game_traffic_far`, `game_vehicle_damage`, `game_lamps`, `game_life`, `game_world`, `game_ui`, `game_controls` | 11.159 |
| Welt | `city_stream`, `tiles`, `mvt`, `terrain`, `interiors`, `destruction`, `world_marks`, `road_graph`, `city_jobs`, `facades`, `water_look` | 5.351 |
| Fahrzeuge | `car`, `drone`, `jet` | 1.138 |
| Leben | `life`, `people`, `ragdoll` | 2.548 |
| Mehrspieler | `net_session`, `game_net`, `game_worldsync`, `game_destruction_sync`, `game_projsync`, `game_toss`, `game_swarm` | 3.713 |
| Eingabe, Bild, Ton | `touch_play`, `look`, `game_look`, `perf`, `game_vr`, `sound_fx`, `combat_audio`, `game_audio_web` | 2.972 |
| Modelle | `combat_art`, `life_art`, `people_art`, `vehicle_art`, `mp_art`, `world_art` | 1.234 |
| Hilfen | `nostr`, `qr_code`, `gps_live` | 631 |
| Einstieg | `main` | 160 |

Alles davon wird Blueprint des Projekts; `main` entfällt (der Host des Editors lädt das Projekt).

Die Seite (`web/`: Ansicht des Originals, Menüs, Ton des Originals) wird das Seitenmodul des
Projekts: Der Export des Editors nimmt es mit.

## Stufen

Jede Stufe endet live. „Fertig“ heißt: die Tests der Stufe sind grün, die drei Ausführungsarten
der Blueprints (Interpreter, JIT, die eigene C++-Klasse) liefern dasselbe, und der Eingabe- und der
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
- [x] **Die Gameplay-Knoten als Funktionen:** Was ein Gameplay-Knoten tut, ist eine typisierte
  Funktion (`kern::game::nodes`, `kern/game/blueprint_nodes.h`) – Actors, Physik, Kampf, Effekte,
  die Events des Spiels, For Each Actor with Tag, die Roboter. Der Knoten der Bibliothek und die
  Klasse rufen dieselbe auf; jeder Eingang wird genau einmal gelesen. Wie ein Knoten aussieht
  (Name, Pins), steht getrennt davon (`logic_node_shapes.cpp`), deshalb kennt
  `tools/blueprint_cpp` die Knoten, ohne die Engine zu bauen. Damit ist **jeder Knoten der
  Bibliothek** eine Funktion: Jeder Blueprint, der fehlerfrei kompiliert, läuft als seine Klasse.
  Beweis: `game_native_test` (in `kern/tests/run.sh`) – zwei Levels, acht Blueprints mit jedem
  Gameplay-Knoten, vom Werkzeug des Spiel-Builds als Klassen geschrieben, im Interpreter und als
  Klassen gespielt und Zeile für Zeile verglichen.
- [x] **Der bisherige Export ist entfernt:** Die Schleife über Knotennummern (`generate_cpp`,
  die Stufe „native C++ (node loop)“) gibt es nicht mehr. Ein Blueprint hat genau ein C++ – seine
  Klasse; Download im Editor gibt sie, und ein Blueprint, der nicht kompiliert, hat keins.
- [x] **Funktionen, Events und Tiefe in jeder Stufe gleich:** Eine Funktion, die über ein Event
  wieder aufgerufen wird, während sie läuft, ist ein Aufruf wie jeder andere (der Interpreter
  übersprang den inneren Aufruf); jeder Aufruf hat seine Parameter, auch für den, der sie von
  außen liest. Wie tief Events ineinander liegen dürfen (64), zählt Funktionsaufrufe nicht mehr
  mit; Werte lesen einander ohne Grenze (der Interpreter hörte nach 48 auf). Tier-Fall
  `function_reentry`.
- [x] **Ganze Zahlen in jeder Stufe gleich:** `Int` ist eine ganze Zahl, wie ein Blueprint sie
  hält (`logic::normal`, in der Klasse der Typ `kern::logic::Whole`): bis 2^53 exakt, darüber
  die nächste Zahl eines `double`, nie über die Enden von 64 Bit. `Count += 1` addiert als Zahl
  und ist für jeden Wert definiert (vorher lief die Klasse dort in undefinierte
  `i64`-Arithmetik, der JIT ließ „keine Zahl“ und unendlich stehen, der Interpreter rundete nur
  in Variablen). Tier-Fall `whole_numbers`.
- [x] **Der JIT geht so tief wie die anderen Stufen:** Sein Stapel für offene Sequences und
  Schleifen ist so groß, wie der Graph ihn je brauchen kann (je laufender Kette lässt ein Knoten
  höchstens einmal etwas offen, und Ketten liegen höchstens 64 Events tief). Vorher stoppte er
  bei 512 offenen Schritten, wo Interpreter und Klasse weiterliefen. Ein Graph, der mehr als
  32 768 bräuchte, bleibt im Interpreter.
- [x] **Nativer Export:** `tools/export_game.sh level.klevel spiel.html` schreibt ein Level als
  eigenständiges Spiel mit diesen Klassen, voll optimiert (dieselbe Seite wie „Package Game“ im
  Editor, das Engine-Modul für das Level gebaut). Der Editor selbst läuft im Browser und kann
  kein C++ kompilieren – der Export ist deshalb ein Werkzeug des Repositorys. Browser-Test
  `export_e2e.mjs`: Die Blueprints des exportierten Spiels laufen als Klassen.
- [ ] Der JIT erzeugt Formeln und Funktionsaufrufe ebenfalls direkt (heute Rückruf).

### 2 – Die Sprache vervollständigen
- [x] Formel-Knoten (Formula): eine Formel als ein Knoten statt als Knotenkette.
- [x] **Listen (Arrays) und „For Each“:** Ein Pin oder eine Variable hält eine Liste von Werten
  eines Typs (`List<Float>`, `List<Text>`, `List<Actor>` …). Wert-Knoten lesen eine Liste (Make
  List, Length, Get Item, Contains, Find Item), Schritte ändern eine Listen-Variable (Add, Insert,
  Set Item, Remove Index, Remove Item, Clear), **For Each** geht durch eine Liste, wie sie beim
  Start der Schleife war; „All Actors with Tag“ gibt eine Actor-Liste. In C++:
  `list_add(Scores, x);`, `for (auto it9 = flow_for_each<Float>(n9, Scores); it9.next();)`.
  Eine Bedeutung je Knoten (`logic::list`), kein Index führt ins Leere, in Interpreter, JIT und
  Klasse dasselbe (Tier-Fall `lists`, Klassen-Fall `Lists`, jeder Knoten in jeder Einstellung).
  Im Editor: Listen-Pins, Listentypen für Variablen und Funktions-Pins, ein Eintrags-Editor;
  generische Knoten nehmen den Typ des ersten Drahts an.
- [x] Strukturen (benannte Felder) als Pin-Typ: `KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };`, Variablen, Listen und Funktions-Pins des Typs, Felder aus Wert-Typen, Aufzählungen, Strukturen und Listen, Make (`struct_make<Weapon>(...)`), Break (`struct_break<Weapon>(w).Damage`) und Set Field (`Held.Damage = x;`); im Editor mit Feld-Editor in Details.
- [x] Datentabellen: Eine Tabelle ist eine Variable, die eine Liste einer Struktur ist – eine Zeile je Eintrag, eine Spalte je Feld; der Editor zeigt und bearbeitet sie als Tabelle. **Find Row** holt die Zeile, deren Feld ein Wert ist (`list_find_row<&Item::Key>(Shop, key)` gibt `.found`, `.row`, `.index`). Kein eigener Begriff neben Listen und Strukturen; die Daten stehen im Blueprint des Levels und werden mit ihm kompiliert.
- [x] Aufzählungen (Enum) als Pin-Typ mit Auswahl: `KERN_ENUM(Mode, Closed, Open)`, Variablen, Listen und Funktions-Pins des Typs, „Switch on Enum“ (`case Mode::Open:`), Namen und Plätze der Werte als Knoten.
- [x] Lokale Variablen in Funktionen: je Funktion eigene Variablen, die bei jedem Aufruf mit ihrem Default beginnen – in C++ die lokalen Variablen am Anfang des Rumpfs (`Float Total = 0.0;`), gelesen und gesetzt mit denselben Knoten wie jede Variable. Nur auf einem Weg der eigenen Funktion (wie Return).
- **Entschieden: Eine Funktion ruft sich nicht selbst auf** – weder direkt noch über andere; Wiederholen ist Sache der Schleifen. Das bleibt eine Regel der Sprache, kein offener Punkt: Was ein Knoten sich merkt (Schleifenstand, Do Once, Wartezeit), gehört dem Knoten des Blueprints, nicht einem Aufruf – deshalb ist ein Blueprint in Graph, JIT und Klasse dasselbe und im Editor Schritt für Schritt zu verfolgen. Keiner der 17 Regel-Blueprints von SpawnCity hat es gebraucht (die Umkehrung einer Kurve und die Farbvergabe sind Schleifen).
- [x] Blueprint-Bibliotheken: Funktionen eines Actors von anderen Blueprints aus aufrufen. Ein Blueprint sagt, wie die Funktion aussieht (`KERN_EXTERN StarsResult Stars(Actor who = Actor{0});` – kein Rumpf), und ruft sie mit **Call on Actor** auf dem Blueprint eines Actors auf, ohne Target auf dem des Levels: `func_call_on<"Stars">(target, kern::out(n7.stars), who);`. Das Level-Blueprint ist damit die Bibliothek für alle. Hat das Ziel die Funktion nicht, gelten die erklärten Defaults; Aufrufe liegen höchstens 64 tief. In Interpreter, JIT und Klasse dasselbe (Tier-Fall `call_on`).
- [x] Typen des Projekts: Aufzählungen und Strukturen, die mehrere Blueprints eines Levels haben – in jedem gleich. Ein Blueprint holt sich den Typ eines anderen („+ An enum / A struct of another Blueprint…“; wer eine Funktion eines anderen erklärt, bekommt die Typen ihrer Pins von selbst), oder ein eigener Typ wird in Details zum Typ des Projekts. Eine Änderung daran geschieht in allen Blueprints, die ihn haben, als ein Schritt, und wird als Ganzes abgelehnt, wenn einer nicht folgen kann. Jeder Blueprint trägt die Erklärung selbst (`project`, im C++ `KERN_PROJECT_ENUM` / `KERN_PROJECT_STRUCT`): Er bleibt für sich lesbar und ausführbar, die drei Ausführungsarten sehen einen gewöhnlichen Typ. Editor-Test `studio_logic_project_types`, Browser-Test Abschnitt 1i.

### 3 – Die reinen Regeln als Blueprints
In dieser Reihenfolge (klein nach groß; jede Datei verschwindet, ihre Tests bleiben):
`combat_detail`, `spawn_guard`, `guided`, `jet_seats`, `lamps`, `gaps_rules`, `controls`,
`settings` (Datentabelle), `world_traffic`, `equipment`, `combat_rules`, `mp_rules`.

- [x] **`combat_detail`** – Actor „CombatDetail“ im Level: die Aufzählungen Critical und Round,
  die Tabellen Profiles und Rounds (Listen der Strukturen CriticalProfile und RoundSpec) und
  sieben Funktionen (IsCritical, DetonationAt, Accelerate, ClockUpdate, MomentumNs,
  BulletImpulse …). (Die Abstimmung über eine Spielregel stand zuerst auch hier; sie ist jetzt
  allein in „Multiplayer“.) `combat_detail.cpp` rechnet
  nichts mehr: Es ruft den Blueprint auf, wie `wanted.cpp`. Die Tests sind dieselben; dazu läuft
  der Blueprint als seine Klasse gegen den Interpreter (1007 Aufrufe, Wert für Wert gleich).
  **Gemessen** (Test `details_blueprint`, Build mit Speicherprüfung): ein Aufruf von ClockUpdate
  aus C++ kostet als Klasse rund 4 µs, im Interpreter rund 80 µs – das Spiel läuft mit der Klasse.
- [x] **`guided`** – Actor „Guided“: die Lenkrakete. Sechzehn Zahlen als Variablen (Speed, TurnG,
  Cooldown, PickPx, Range …), die Struktur Missile, die Aufzählung Result und die Funktionen
  UpdateDodge, PickScore, Launch, Track und Step (der Flug: von der Schiene fallen, steigen, flach
  fliegen, stürzen, zünden). `guided.cpp` ruft nur noch auf; die Flugtests sind dieselben und
  laufen gegen den Blueprint. Klasse gegen Interpreter: mit CombatDetail und Wanted 1306 Aufrufe.
- [x] **`spawn_guard`** – Actor „SpawnGuard“: die Strukturen Viewer und Footprint, die Abstände
  und Winkel als Variablen, die Funktionen ConeHalfAngle, SeenBy, CanSpawn, FootprintsOverlap. Ob
  ein Gebäude den Punkt verdeckt, fragt der Blueprint die Welt selbst: **Line Trace by Channels**
  gegen den Kanal „World“ – kein Rückruf ins C++ mehr. Dafür hat die Engine Box3Ds Abfragefilter
  bekommen (Kategorie und Maske, `physics::QueryFilter`) und das Level benannte Kollisionskanäle
  (SpawnCity: World, Vehicle, Drone, Query). Die Tests laufen in einer GameWorld mit einem
  Gebäude; ein „Fahrzeug“ im Weg verdeckt nichts.
- [x] **`jet_seats`** – Actor „JetSeats“: die Aufzählungen Seat und Result, die Struktur Row und
  die Funktionen Claim, Release, Drop, Valid, Get und SeatOf. Jede nimmt die Tabelle (eine Liste
  von Row) und gibt sie so zurück, wie sie danach ist – die Tabelle selbst hält weiter die
  Session (jede ihre eigene), entschieden wird im Blueprint.
- Der Aufruf aus C++ steht einmal in `blueprint_call.h` (`rules::Blueprint`).
- [x] **`lamps`** (die Regeln je Lampe) – Actor „Lamps“: sechs Zahlen als Variablen
  (HeadHitRadius, PoleRadius, ToppleSpeed, BlastMin, BlastDefault, BlastToppleShare) und die
  Funktionen Flat, Worse, ShotAt, BlastReach, BlastFate und CarFate – wann ein Schuss den Kopf
  trifft, was eine Explosion und ein Auto mit dem Mast machen, welcher Zustand der schlimmere ist.
  Das Feld selbst – tausende Lampen im Raster, 64-Bit-Zellschlüssel – bleibt in `lamps.cpp` und
  fragt je Lampe den Blueprint; es ist ein System und gehört zu Stufe 4 (ein Blueprint-Int ist nur
  bis 2^53 exakt).
- [x] **`gaps_rules`, Teil Sirenen** – Actor „Sirens“: Tonhöhen, Pegel und Hörweite als Variablen,
  die Aufzählung Organic und die Funktionen HornHz, WhoopHz, SirenGain, SirenCutoff, VoiceUpdate
  (Doppler aus der Abstandsänderung), PostArrivalGraceMs und OrganicRate. Offen aus `gaps_rules`:
  Rauchfeld, VisualPose und die Hashes – das sind Systeme (Stufe 4).
- [x] **`controls`** – Actor „Controls“: die Eingabe-Mathematik. Totzonen, Kurven und Zeiten als
  Variablen (die Blickkurve als drei Listen ihrer Stützpunkte, die Tastenhilfe als Tabelle Hints),
  die Strukturen Look und Cook, die Aufzählung DeviceEvent und vierzig Funktionen: WalkPadAxis,
  ShapeFpsStick, FpsDynamicCurve, LookAxisStep, FpsAimAssist, GamepadAxis, GameStickLeft / Right
  (mit der Umkehrung der Kurve des Flugreglers in 30 Halbierungen), RateStickStep, LookAdd /
  LookStep, ViewButtonStep, VoiceChordStep, ReleaseLatchStep, GrenadePull / Step / Release,
  DeviceInput, HintLine … `controls.cpp` hält nur noch, was ein Modus zwischen zwei Bildern
  behält, und ruft auf.
- [x] **`settings`** – Actor „Settings“: die Tabelle aller 60 Einstellungen als Datentabelle
  (Variable Options, eine Liste der Struktur Option: Key, Section, Label, Help, Min, Max, Step,
  Default, Toggle, Live – im Editor als Tabelle zu bearbeiten) und die Funktionen Snap (ein Wert,
  wie die Einstellung ihn nimmt), Word (true / on, false / off) und InSection. `settings.cpp` hat
  keine Tabelle mehr: Es liest sie beim Start aus dem Blueprint, hält die Werte des Spielers und
  schreibt das JSON für die Seite. Die Stick-Kurve `fineness_axis` gab es doppelt – sie ist
  PhoneAxis in „Controls“.
- [x] **`equipment`, Teil Jetpack** – Actor „Jetpack“: Schub, Treibstoff und Zeiten als
  Variablen, die Struktur Pack, die Funktionen Ignite und Update (Schubstoß, Halten, Steiggrenze,
  das Abfangen vor einer harten Landung, Nachfüllen). Offen aus `equipment`: Flugbuch und
  Musik-Synthesizer – Aufzeichnung und Klangerzeugung, also Systeme (Stufe 4).
- [x] **`world_traffic`, die Gesetze** – Actor „Traffic“: Ampelzeiten, Abstände und Wartezeiten
  als Variablen und elf Funktionen – JunctionLights, JunctionRadius, Light (welche Phase für
  welche Richtung), MustStop (Rot; Gelb, wenn es noch reicht), StopSpeed, YieldReach,
  YieldHalfWidth, Honk, SideReach, Bodies, GapSpeed. `world_traffic.cpp` behält die Kreuzungen
  der Straßen und die Schleifen über Autos und Leute (wer wo ist, wer eine Kreuzung hält) und
  fragt die Gesetze je Auto; je Paar Auto–Passant wird kein Blueprint gerufen (das wären
  tausende Aufrufe je Bild). Diese Schleifen sind ein System (Stufe 4).
- [x] **`combat_rules`** – fünf Actors:
  **„Military“** (die Uhr der Armee als Funktion Update über der Struktur Clock, die
  Trefferchancen von Hubschrauber, Soldat und Gangster, der Streufehler des Panzers, Flak,
  Jagdbomber-Timing, wer von den Passanten flieht, rennt oder zurückschlägt – und 47 Zahlen als
  Variablen), **„Zombies“** (die Runden der Zombienächte: Update, Spawned, NextRound, Hp, Speed),
  **„Sandbox“** (die Gegner als Datentabelle Enemies, ChooseEnemy, Budget, EnemyHitChance und der
  Chaos-Score mit Combo), **„Fun“** (Punkte auf Risiko: die Aufträge als Datentabelle Contracts,
  die Struktur Run mit 18 Feldern, Apply für jedes der zwölf Ereignisse, Multiplier) und
  **„Race“** (die Tore als Datentabelle Gates, GateAxes, Crossed, Update über Runden und
  Bestzeit). `combat_rules.cpp` rechnet nichts mehr außer den Hashes, die einer Zelle oder einer
  Person ihre eigene Zufallszahl geben. Die Zahlen, die das Spiel überall liest
  (`combat::kJetSpeed` …), sind keine C++-Konstanten mehr, sondern die Variablen der Blueprints,
  beim Binden gelesen. Eine doppelte Zahl (der Zombie-Treffer stand zweimal im C++) ist weg.
- [x] **`mp_rules`, die Regeln** – Actor „Multiplayer“: die Farben der Spieler (Palette,
  ColorSlots), Schaden je Waffe und Körperzone (Datentabelle Weapons), JudgeHit, Regen,
  RespawnOffset, SpawnRingOffset, die einstimmige Abstimmung (CreateBallot, CastVote), VoiceGain,
  die Pfeile und Plätze der Marker (BearingArrow, LocateMapPeer, SafeMobileMarkerPoint) und die
  Pings (PingFade, PingToggles, PingExpired). In `mp_rules.cpp` bleibt, was Daten bewegt: die
  Zeitleisten der Posen, der Sprach-Codec und seine Ströme, die Geometrie der Routen, die
  Punktetafel, die dichteste Gruppe für den Einstieg – Systeme (Stufe 4).
- **Damit sind die Regeln aller dreizehn Dateien Blueprints** (17 Actors im Ordner „Rules“ des
  Levels). Was in den Dateien noch steht, sind Brücken und Systeme.
- **Zahlen an der Grenze (32 Bit im Spiel, 64 Bit im Blueprint).** Ein Blueprint rechnet in
  64 Bit, wie das JavaScript des Originals, und nennt seine Schwellen dezimal (0,1 · 2,6 · 0,42).
  Das C++ des Spiels hält seinen Zustand in 32 Bit; dort ist „0,1“ die Zahl 0,100000001490116…
  – unverändert weitergereicht läge sie knapp jenseits einer Schwelle 0,1, auf der sie genau
  steht. Darum geht jede 32-Bit-Zahl als die Dezimalzahl hinüber, für die sie steht
  (`rules::number`, `rules::decimal` in `blueprint_call.h`: die kürzeste Dezimalzahl, die genau
  diese 32-Bit-Zahl ist). Damit antwortet jeder Vergleich einer Zahl des Spiels mit einer
  Schwelle genau wie vorher in 32 Bit – auf der Schwelle und auf beiden Seiten; der Weg ist
  eineindeutig (zurück in 32 Bit ist es dieselbe Zahl) und erhält die Ordnung. Geprüft im Test
  `controls_numbers_cross_exactly`: 29 Schwellen der Regeln mit je vier 32-Bit-Nachbarn zu beiden
  Seiten, dazu 200.000 Zahlen hin und zurück. Die drei Prüfungen, die zwischenzeitlich „knapp
  unter der Schwelle“ prüften, prüfen wieder genau auf ihr. Was eine Regel aus mehreren Zahlen
  *ausrechnet*, rechnet sie in 64 Bit – in Interpreter, JIT und Klasse und auf jeder Plattform
  Bit für Bit gleich (die Vergleichsläufe der Stufen prüfen das mit jedem Aufruf).
- **Entschieden: Die Blueprint-Sprache rechnet weiter in 64 Bit, nicht in 32.** Das Maß ist
  spawncity.de, nicht der C++-Port dazwischen. Das Original ist JavaScript und rechnet jede
  Regel in 64 Bit: In keiner seiner 251 Dateien unter `sim/` (Stand 9108537) steht ein
  `Math.fround`. Wo das alte C++ Zwischenergebnisse auf 32 Bit rundete, wich *es* vom Original
  ab; ein Blueprint rechnet dort jetzt, was das Original rechnet. Eine Sprache in 32 Bit wäre
  zudem nicht in sich stimmig: Ganze Zahlen wären nur bis 2^24 genau (heute 2^53), die Zeiten
  des Mehrspielers in Millisekunden passten nicht mehr hinein, und es bräuchte einen zweiten
  Zahlentyp in allen drei Ausführungsarten.
- **Zwei Spieler im Browser.** `tests/smoke.mjs` (`SC_SMOKE=mp`) spielt Host und Client in zwei
  Tabs. Dabei gefunden und behoben: Ein Client, der nicht binnen sechs Sekunden zum Host
  durchkam, blieb für immer draußen – er fragt jetzt jede Sekunde erneut (Test
  `mp_a_client_keeps_asking_until_the_host_answers`). Zwei veraltete Erwartungen des Tests sind
  berichtigt (die Antwort auf einen Treffer nennt die Zone; Friendly Fire ist an, nicht aus).
  Der Test selbst braucht eine Maschine, die zwei Tabs flüssig zeichnet: Mit Software-WebGL auf
  zwei Kernen verlieren sich die Spieler wieder (eine Session lebt in echter Zeit), deshalb läuft
  er nicht in der CI. Was eine Session tut – Treffer, Abstimmung, Kill, Effekte, Pings, Stimme,
  Übergabe des Hosts –, prüft `tests/test_mp.cpp` ohne Browser.
- **Der wackelnde Browser-Test** (ein Klick im Blueprint-Test lief selten in Playwrights 30 s):
  kein Fehler der Seite. Mit Software-WebGL liefert der Browser zeitweise sekundenlang kein Bild
  (gemessen im hängenden Zustand: der Thread der Seite zu 97 % untätig, zwei Bilder in zwölf
  Sekunden); ein Klick wartet auf zwei ruhige Bilder. Der Test gibt der Seite jetzt zwei Minuten.
- **Eine Abstimmung statt zwei.** Die einstimmige Abstimmung über eine Spielregel
  (rules_vote_engine.mjs) gab es im C++ zweimal – für die Session und für die Konsole – und
  dadurch zuerst auch in zwei Blueprints. Sie steht jetzt einmal, in „Multiplayer“: CreateBallot,
  CastVote (mit dem, was die einzelne Stimme bewirkt hat: auch „ignored“), Stale, BallotName.
  Auch ob die laufende Abstimmung der Session verfallen ist, fragt die Session dort (vorher ein
  eigener Vergleich im C++).
- **Gemessen** (wie das Spiel gebaut, -O2, `SC_TEST_RELEASE=1 run_tests.sh controls_call_cost`):
  Ein Aufruf einer Regel aus C++ kostet als Klasse rund 0,2 µs (eine Formel), 0,3 µs (mehrere
  Knoten, eine Schleife, eine zweite Funktion) bis 0,7 µs (eine Struktur hinein und heraus). Je
  Bild sind das für die Steuerung und den Verkehr zusammen weit unter einer Millisekunde.
- Geschrieben wird so ein Regel-Blueprint als sein C++-Text und mit
  `tools/blueprint_cpp.sh --apply level.klevel <Actor> text.cpp` in das Level gelegt (dieselbe
  Lesart wie „Apply“ in der C++-Ansicht des Editors); das Level bleibt die eine Quelle.

### 4 – Der Konverter: C++ zu Blueprints
`tools/cpp_to_blueprint.py` liest die Quellen mit dem Parser des Compilers (clangs Syntaxbaum)
und schreibt sie in der Schreibweise:

- Jede Quelldatei wird der Blueprint eines Actors im Ordner „Source“ des Levels (`terrain.cpp` →
  „Terrain“): ihre Funktionen sind seine Funktionen, ihre Konstanten seine Variablen, die
  Aufzählungen und Strukturen, mit denen sie rechnet, seine Typen. Hilfsfunktionen aus Headern
  (`inline`) stehen in jedem Blueprint, der sie benutzt – wie in C++ in jeder Übersetzungseinheit.
- Gerechnet wird mit der Arithmetik der Blueprint-Sprache: Zahlen in 64 Bit, ganze Zahlen exakt,
  eine Division durch null ergibt 0, Vektoren in 32 Bit. Das ist in Interpreter, JIT und Klasse
  und auf jeder Maschine gleich. Wo das C++ eine Zahl ausdrücklich auf 32 Bit bringt (das Kreuzprodukt
  zweier 2D-Vektoren, ein Vektor geteilt durch eine Zahl), tut es der Blueprint auch (`single` in der
  Formel). Vorzeichenlose 32-Bit-Arithmetik und Bit-Operationen haben eigene Formel-Funktionen.
- **Der Beweis je Funktion, Bit für Bit:** `--test` schreibt zu jeder Datei einen Test
  (`tests/converted/`). In ihm steht der Quelltext jeder umgeschriebenen Funktion noch einmal, wie er
  ist – mit allem, was er nennt (Klassen, Konstanten, Aufzählungen, Typnamen, Hilfsfunktionen), in der
  Reihenfolge der Übersetzungseinheit – und wird mit den Zahlen der Blueprint-Sprache übersetzt:
  `f32` ist `double`, Zahlen ohne das `f`, das sie auf 32 Bit bringt, die Funktionen der Zahlen
  (`sin`, `pow`, `sqrt` …) sind die der Formel (`kern::math`), und ein Vektor hält seine Komponenten
  in 32 Bit, gelesen sind sie Zahlen wie alle anderen (`converted::wide` in `tests/converted.h`); was
  Vektoren als Ganzes tun, tun die Funktionen der Engine, wie in den Knoten. Dieser Quelltext steht in
  einem eigenen Namensraum: Vom Programm selbst ist dort nichts erreichbar – was er nennt und der
  Test nicht hält, übersetzt nicht, statt still auf die 32-Bit-Funktion des Programms zu fallen.
  Jede Funktion wird 300-mal mit denselben Argumenten aufgerufen: dieser Quelltext, der Blueprint im
  Interpreter und der Blueprint als kompilierte Klasse. Alle drei Ergebnisse müssen **dieselben Bits**
  haben (auch 0.0 und −0.0 sind zwei Ergebnisse); es gibt keine Toleranz. Methoden werden auf einem
  gewürfelten Objekt ihrer Klasse aufgerufen; verglichen wird alles, was der Aufruf gibt und ändert.
- Was nicht verglichen werden kann, weil C++ dort nichts festlegt, zählt der Test und vergleicht es
  nicht: einen Aufruf, in dem der Quelltext durch null teilt oder die Zahlen verlässt (Wurzel aus
  Negativem), und einen, in dem er einen Eintrag außerhalb einer Liste liest oder schreibt, aus einer
  leeren Liste nimmt, ganze Zahlen durch null teilt, Bits um die Breite der Zahl schiebt, eine Zahl
  in einen ganzzahligen Typ bringt, der sie nicht hält, oder eine in einen Aufzählungswert, den es
  nicht gibt. Dafür führt der Test diese Stellen im Quelltext durch Funktionen (`converted::at`,
  `quot`, `shl`, `fits` …), die den Wert geben, wo es einen gibt, und den Aufruf sonst markieren.
  Für einen markierten Aufruf werden neue Argumente gewürfelt (bis zum Zwanzigfachen); je Funktion
  müssen mindestens 20 Aufrufe verglichen sein, sonst schlägt der Test fehl. Objekte einer Klasse
  mit Konstruktor baut der Test zur Hälfte mit dem Konstruktor (ihre Teile passen dann zusammen),
  zur Hälfte Feld für Feld.
- Die Schleifengrenze einer Laufzeit (`Runtime::budget`, 200.000 Durchläufe je Schritt) ist Sache
  des Programms: Der Test setzt sie hoch, weil Funktionen des Spiels lange schleifen (ein Klang,
  Abtastwert für Abtastwert gerechnet; eine Zahl hoch 256 Bit). Das Spiel muss das beim Umschalten
  (Stufe 5) ebenso tun. Eine Funktion, die lange rechnet, wird so oft verglichen, wie zwei Sekunden
  Platz lassen – mindestens 20-mal. Braucht die kompilierte Klasse für einen Aufruf länger als 5 ms,
  wird dieser Aufruf nur mit der Klasse verglichen: Der Interpreter bräuchte dafür Minuten (er löst
  jeden Schritt über Namen auf). Dauert ein einzelner Aufruf Sekunden (eine 256-Bit-Zahl hoch eine
  256-Bit-Zahl, Glied für Glied in 32 Bit), werden so viele verglichen, wie 20 Sekunden Platz
  lassen – mindestens fünf. Der Test nennt jede solche Funktion in seiner Ausgabe (`note:`).
- **64-Bit-Zahlen als Bitmuster.** Eine Zahl, die das Programm `u64`, `i64`, `uint64_t` oder
  `int64_t` schreibt, ist ein Bitmuster: ein Schlüssel aus zwei 32-Bit-Zahlen, ein Hash, der
  Zustand eines Zufallsgenerators. Eine ganze Zahl des Blueprints ist bis 2^53 exakt – das reicht
  dafür nicht. Im Blueprint ist so eine Zahl die Struktur `Whole64` aus ihren beiden 32-Bit-Hälften,
  und was C++ mit ihr rechnet (addieren, multiplizieren, schieben, teilen, vergleichen – alles um
  2^64 herum), rechnen die Funktionen aus `tools/blueprint_whole64.h`: C++ von der Art, die der
  Konverter umschreibt. Sie stehen in jedem Blueprint, der sie benutzt, und werden wie die
  Funktionen des Programms Bit für Bit mit ihrem Quelltext verglichen – über den ganzen Bereich
  von 64 Bit. Offen lässt C++ dort nur: vorzeichenbehaftetes Rechnen, das die 64 Bit verlässt, eine
  Division durch null, ein Schieben um 64 oder mehr (`converted::plus_bits`, `quot_bits`,
  `shl_bits` … markieren den Aufruf). Eine Zahl, die `size_t`, `long` … geschrieben ist, ist eine,
  mit der gezählt wird: die ganze Zahl des Blueprints (siehe unten).
- **Maps und Sets sind Listen.** Eine Map ist im Blueprint die Liste ihrer Zeilen – Strukturen aus
  Schlüssel (`first`) und Wert (`second`) –, ein Set die Liste seiner Schlüssel. Die Reihenfolge ist
  die, in der die Schlüssel hineinkamen (`std::map` und `std::set`: die der Schlüssel) – was
  `std::unordered_map` offen lässt, ist damit festgelegt, und im Test steht an ihrer Stelle eine Map
  mit genau dieser Reihenfolge (`converted::wide::Map`, `Set`). Nachschlagen ist der Knoten Find Row
  (`list_find_row<&Zeile::first>(liste, schlüssel)`); `m[k]` legt die Zeile an, wenn es sie nicht
  gibt, wie in C++ – auch wenn nur gelesen wird; `insert`, `emplace`, `try_emplace`,
  `insert_or_assign`, `erase`, `find`, `count`, `contains`, `at` sind Schritte und Werte über diese
  Liste. Ein Iterator (in eine Map, ein Set oder eine Liste) ist die Stelle, an der er steht – eine
  ganze Zahl; `end()` ist die Länge der Liste, `it->second` der Eintrag an der Stelle, `++it` die
  nächste. `auto [a, b] = wert;` und `for (auto& [k, v] : map)` sind Namen für die Teile des Werts:
  wo sie stehen, steht der Teil (`wert.first`).
- Ein Stück Quelltext, das mit den Zahlen der Sprache nicht übersetzt (es ruft etwas, das nicht
  umgeschrieben ist), nimmt der Konverter aus dem Test und nennt die Funktion als nicht verglichen.
- Eine Funktion wird geschrieben, wenn alles in ihr einen Platz in der Schreibweise hat – und
  alles, was sie aufruft. Die anderen bleiben C++, bis der Konverter oder die Sprache den Platz
  hat. Der Lauf nennt, worauf sie warten, nach Zeilen geordnet.

**Was der Beweis sagt und was nicht.** Er sagt: Der Blueprint rechnet genau das, was der Quelltext
sagt, in den Zahlen der Sprache – überall gleich. Er sagt nicht, dass das dieselben Bits sind wie im
heutigen Programm: Das rechnet dieselben Formeln mit `f32` in 32 Bit, die Sprache in 64 Bit (wie das
JavaScript-Original). Das ist die Festlegung der Sprache, kein Spielraum des Tests.

Stand (13.11.2026, 17:22): 355 von 1.428 Funktionen (2.700 von 28.544 Zeilen) aus 58 von 77 Dateien stehen
als Blueprints im Level (58 Actors im Ordner „Source“). Der Test vergleicht jede davon, die etwas
gibt, Bit für Bit mit ihrem Quelltext – 513 Vergleiche, weil Funktionen aus Headern (und die der
64-Bit-Zahlen) in jedem Blueprint stehen, der sie benutzt; keine ist ausgenommen. Das Spiel führt für diese Dateien weiter
ihr C++ aus (Stufe 5).

Worauf die anderen warten, nach Zeilen: der Zustand von `Game` (328 Funktionen, 13.818 Zeilen:
geteilte Zeiger, Typen der Engine mit eigenem Innenleben); Klassen mit einem Zeiger auf die Welt,
den Renderer, den Mixer oder die Physik der Engine (`Life`, `PeopleArt`, `Car`, `Marks`,
`Destruction`, `Drone`, `Interiors`, `Ragdolls`, `Terrain`, `CityStream` …: rund 7.000 Zeilen, dazu
rund 800 Zeilen mit Zeigern im Rumpf); `Session` (Netz der Engine: 664); die Aufrufe der Regel-Blueprints (482); Text formatieren
und zeichenweise lesen.

Aufruf (in `kern/games/spawncity`):

    ../../../tools/cpp_to_blueprint.py --convert sc --level level/spawncity.klevel --test tests/converted \
        -I../../include -I../../third_party/box3d/include -Isrc -- src/*.cpp

Was als Nächstes dazukommt, in der Reihenfolge der Zeilen, die darauf warten:
1. [x] **Objekte:** Die Daten einer Klasse sind eine Struktur, ihre Methoden Funktionen über diesen
   Wert (`Shake_add(self, amount)` gibt das geänderte `self` zurück). Parameter, durch die eine
   Funktion schreibt (`f32& out`), sind weitere Ergebnisse; gibt eine Funktion mehreres zurück, ist
   ihr Ergebnis eine Struktur (`Shake_sample_out`). Der Aufrufer speichert zurück, was sich
   geändert hat – auch in ein Feld, eine Komponente eines Vektors oder einen Eintrag einer Liste,
   Ebene für Ebene. Konstruktoren sind Funktionen, die den Wert bauen (`Projector_new`);
   Default-Argumente stehen beim Aufruf.
2. [x] **Lambdas:** eine lokale Funktion (`auto f = [&](…) { … };`) ist eine Funktion des
   Blueprints; was sie von ihrer Umgebung benutzt, sind ihre ersten Eingänge, was sie davon ändert,
   weitere Ergebnisse – bei jedem Aufruf zurückgespeichert. Per Wert gefangen: die Werte, wie sie
   bei der Deklaration sind. Sie wird mit der Funktion verglichen, in der sie steht.
   - [ ] Lambdas als Argumente (`std::sort`, `remove_if`, `find_if`) und `std::function`.
2a. [x] **Typen:** Namen, die das Programm Typen gibt (`using Bytes32 = …`); Aufzählungen mit
   eigenen Zahlen (die Zahlen stehen als Liste in einer Variablen, wo ein Wert als Zahl gebraucht
   wird); `std::pair` als Struktur; eine Liste in einer Liste als Struktur, die sie hält;
   `std::unique_ptr` und `std::optional` als Liste mit keinem oder einem Eintrag (`p->x`,
   `p.reset()`, `if (p)`, `std::make_unique`, `std::move`); `++i` in einem Ausdruck; eine
   Referenz als Name für eine Stelle (`T& x = a.b[i];` – wo `x` steht, steht `a.b[i]`); ein `if`,
   das eine Variable deklariert; `sizeof a / sizeof a[0]`.
3. [x] **Schleifen mit `break` / `continue`** (Flags, die den Rest des Durchlaufs überspringen),
   `do-while`, Schleifen, die die Einträge ihrer Liste ändern (der Eintrag ist eine lokale
   Variable und wird am Ende des Durchlaufs zurückgespeichert), `a = b = c`.
4. **Sprache:**
   - [x] Ganze Zahlen, wie eine Maschine sie hat: Die Formel hat `band`, `bor`, `bxor`, `shl`,
     `shr` (Bits) und `wrap32`, `wrap32s`, `mul32` (Rechnen, das bei 32 Bit umbricht) – in jeder
     Stufe dieselben Funktionen. Der Konverter schreibt vorzeichenlose 32-Bit-Arithmetik und
     Bit-Operationen damit (Hashes, Paketformate).
   - [x] Eine Struktur hat bis zu 96 Felder (vorher 32).
   - [x] Die Typen der Engine: Der Konverter liest die Header der Engine mit (zweiter Lauf des
     Compilers, dieselben Knoten-Namen) – was dort eine schlichte Struktur ist (Quat, Vec4, Mat4,
     Camera, Aabb …), ist eine Struktur des Blueprints, und was die Header an Funktionen dazu
     definieren (`Quat::rotate`, `Mat4::translation`), wird wie der Code des Spiels geschrieben.
   - [x] Ganze Zahlen in 64 Bit, mit denen gezählt wird (`size_t`, Längen, Indizes): Eine
     vorzeichenlose 64-Bit-Zahl ist im Blueprint die ganze Zahl, die ihre Bits mit Vorzeichen
     gelesen sind (2^64 − 1 ist −1). Was C++ um 2^64 herum addiert, subtrahiert und multipliziert,
     kommt so exakt gleich heraus, solange die Zahlen innerhalb von 2^53 um null liegen; Vergleiche
     und die Umwandlung in eine Zahl sagen es aus. Der Test markiert einen Aufruf, in dem der
     Quelltext diesen Bereich verlässt.
   - [x] 64-Bit-Zahlen als Bitmuster (Schlüssel aus zwei 32-Bit-Zahlen, FNV-Hash): die Struktur
     `Whole64` aus zwei 32-Bit-Hälften, gerechnet von den Funktionen aus
     `tools/blueprint_whole64.h` (siehe oben).
   - [x] Listen: einfügen und löschen an einer Stelle oder über einen Bereich (`erase`, `insert`),
     `resize`, `assign`, Listen, die ein Konstruktor baut (`std::vector<T> v(n, x)`, aus einem
     Bereich), `std::swap`, `std::reverse`.
   - [x] Maps und Sets als Listen ihrer Zeilen, Iteratoren als Stellen, Namen für die Teile eines
     Werts (`auto [a, b]`) – siehe oben.
   - [ ] Text formatieren und zerlegen; Listen sortieren und durchsuchen; `std::priority_queue`.
   - [ ] Zeiger: ein Zeiger auf ein Objekt des Spiels ist sein Platz in einer Liste oder ein Actor;
     ein Zeiger auf die Welt, den Renderer oder den Mixer der Engine ist die Engine selbst (6).
5. [ ] **Aufrufe zwischen Blueprints als Werte:** eine reine Funktion eines anderen Blueprints
   ohne Umweg über einen Schritt (heute: Call on Actor) – damit rufen umgeschriebene Funktionen die
   Regel-Blueprints der Stufe 3 direkt auf, wo das C++ heute über `blueprint_call.h` geht, und
   Funktionen anderer Dateien.
5a. [ ] **Der Zustand des Spiels:** `Game` und die Systeme, die es einmal gibt (Gelände,
   Stadt, Leben, Sitzung), sind Blueprints mit Variablen statt Strukturen; ihre Methoden ändern
   diese Variablen. Kleine Klassen, die es oft gibt, bleiben Strukturen in Listen.
6. [ ] **Die Engine als Knoten:** was die Quellen von der Engine aufrufen – Physik (Körper,
   Gelenke, Abfragen), Kamera, Renderer und Meshes, Netz, Ton, Zeit –, allgemein und für jedes
   Spiel.

   **Plan (13.11.2026).** Rund 21.000 der 28.544 Zeilen warten hierauf: `Game` und fast jede
   Klasse mit Zustand hält einen Zeiger auf die Welt der Physik, den Renderer oder den Mixer und
   ruft sie auf. Die Quellen rufen 154 Funktionen der Engine, die ihre Header nur deklarieren
   (`physics::World` 39, `GameWorld` 21, Netz 13, `Mixer` 10, `Renderer` 9, Meshes bauen 10 …).
   - **Ein Knoten je Funktion.** Was ein Header der Engine deklariert und nicht selbst definiert,
     ist ein Knoten: `World::cast_ray` ist `world.cast_ray`, in C++ `world_cast_ray(welt, von,
     nach)`. Eine `const`-Methode und eine freie Funktion ohne Seiteneffekt ist ein Wert, alles
     andere ein Schritt; was eine Funktion durch einen Parameter schreibt (`std::vector<u64>& out`),
     ist ein weiterer Ausgang. Was Rümpfe im Header hat, wird wie bisher umgeschrieben.
   - **Erzeugt aus den Headern** (`tools/engine_api.py`, mit dem Baum des Compilers wie der
     Konverter): die Tabelle der Funktionen mit ihren Pins (`src/game/engine_api.gen.cpp`), die
     Funktionen der Blueprint-Klasse (`include/kern/game/engine_nodes.gen.h`) und der Verteiler,
     der einen Aufruf mit seinen Werten an die Funktion der Engine gibt. Eine Funktion, deren Typen
     keinen Platz haben (ein Callback), bekommt keinen Knoten und wird genannt.
   - **Ein Objekt der Engine ist ein Handle:** Ein Zeiger oder eine Referenz auf eine Klasse der
     Engine mit eigenem Innenleben (`physics::World*`, `Renderer*`, `Mixer*`) ist im Blueprint eine
     ganze Zahl, die der Host vergibt; sie ist der erste Eingang der Knoten dieser Klasse. Schlichte
     Strukturen der Engine (`Pose`, `BodySettings`, `RayHit`) sind Strukturen, die der Blueprint
     deklariert – der Pin nennt den Namen.
   - **Alle Stufen gehen durch eine Tür:** `logic::Host::engine(function, in, out)`. Interpreter,
     JIT und Klasse rufen dieselbe Funktion des Hosts mit denselben Werten; das Spiel bindet dort
     die echte Engine, ein Test einen Stellvertreter.
   - **Der Beweis** bleibt Bit für Bit: Der Quelltext im Test läuft gegen Stellvertreter-Klassen
     der Engine (erzeugt), die jeden Aufruf mit seinen Argumenten aufzeichnen und die Antwort
     würfeln (aus der Form des Ausgangs, die die Tabelle nennt); der Blueprint läuft gegen einen
     Host, der dasselbe tut. Verglichen werden das Ergebnis und die Aufzeichnung: dieselben Aufrufe,
     in derselben Reihenfolge, mit denselben Bits. Reine Funktionen der Engine (Meshes bauen,
     Klänge rechnen) ruft der Test auf beiden Seiten echt auf.
   - Reihenfolge: (a) `Host::engine`, Tabelle, Knoten und Klassen-Funktionen in der Engine, mit
     Tests der Engine; (b) der Generator über alle Header; (c) der Konverter: Handles, Aufrufe als
     Knoten, Stellvertreter im Test; (d) der Verteiler auf die echte Engine – Voraussetzung für
     Stufe 5.
   - **Stand (13.11.2026, 12:45).**
     - [x] (a) + (b): Die Engine hat die Knoten. `tools/engine_api.py` liest 17 Header (Physik,
       Renderer, Meshes, Ton, Netz, Spielwelt, Jobs, App) und schreibt die Tabelle
       (`kern/src/logic/engine_api.gen.cpp`), die Funktionen der Blueprint-Klasse
       (`kern/include/kern/logic/engine_nodes.gen.h`) und den Fall der Engine-Tests
       (`kern/tests/engine_case.gen.h`): 540 Funktionen sind Knoten, 54 warten (Zeiger, durch die
       die Engine schreibt; Typen mit eigenem Innenleben als Wert). Ein Zeiger auf Daten, die nur
       gelesen werden (`const Vec3* rotation`, `const f32* samples` mit ihrer Anzahl daneben), ist
       die Liste dieser Daten – leer für `nullptr`. Die Tür ist `logic::Host::engine`; der
       Stellvertreter der Tests ist `logic::EngineStandIn` (antwortet aus der Form der Ausgänge,
       abhängig vom Aufruf und davon, wie viele vor ihm kamen, und hält jeden Aufruf fest). Die
       Engine prüft jeden Knoten ihrer Bibliothek in jeder Stufe: Der erzeugte Blueprint „Engine"
       ruft jede der 540 Funktionen einmal, im Interpreter und als kompilierte Klasse.
     - [x] (c) Der Konverter schreibt Engine-Aufrufe als diese Knoten (eingeschaltet seit 13.11.,
       15:55): 353 Funktionen, 2.682 Zeilen, 58 Dateien, 511 Vergleiche. Was er dabei
       tut: Ein Zeiger oder eine Referenz auf ein Objekt der Engine, dessen Daten keine Struktur
       sein können, ist ein `Int` (Handle; `nullptr` ist 0). Jeder Engine-Aufruf ist ein eigener
       Schritt vor der Anweisung, in der er steht – er läuft genau einmal, wo C++ ihn ruft; ein
       Wert geht in eine lokale Variable, ein Schritt mit Ausgängen in das Glied des Knotens
       (`struct { RayHit result = …; } n12;`). Steht er in einem Teil eines Ausdrucks, den C++
       nicht immer auswertet (`a && w->valid(b)`, `c ? x : y`), wird daraus ein `if`, das eine
       Variable setzt. Eine Funktion, die einen Schritt der Engine enthält, ist nicht „pure": Wer
       sie ruft, ruft sie als Schritt (`n12 = Car_reapply(…);`, gelesen wird `n12.Result`) und ist
       dann selbst so eine Funktion. Die Pins müssen die der Tabelle der Engine sein, sonst wartet
       die Funktion.
     - [x] (c) Der Test dazu ist geschrieben: Für jede Klasse der Engine, von der der Quelltext
       ein Objekt hält, hat der Test eine Stellvertreter-Klasse (`TestWriter.stand_ins`) mit genau
       den gerufenen Methoden – Signatur aus dem Header, in den Typen des Tests. Jede Methode gibt
       ihre Argumente als Werte an den Stellvertreter der Engine (`converted::engine_call`) und
       nimmt die Antwort zurück (`take` je Struktur, das Gegenstück zu `arg`). Objekte würfelt der
       Test aus einem kleinen Vorrat (`converted::engine_object`, Handle 1..6). Der Host der
       Blueprints im Test hat denselben Stellvertreter. `Tally::take` vergleicht zusätzlich die
       drei Aufzeichnungen (Quelltext, Interpreter, Klasse): dieselben Aufrufe in derselben
       Reihenfolge mit denselben Bits; die erste Abweichung nennt er.
     - [x] (c) Eingeschaltet. Dabei festgelegt:
       - Eine Funktion wird nur geschrieben, wenn ihr Test sie vergleicht. Was der Test noch nicht
         vergleichen kann, wartet mit Grund – und mit ihr, was sie ruft (die Datei wird ohne sie
         noch einmal umgeschrieben). Fällt die Definition einer Methode aus dem Test, fällt ihre
         Deklaration mit: Was sie ruft, übersetzt dann nicht, statt nicht zu linken.
       - Ein Quelltext, der außerhalb einer Liste liest, hat verlassen, was C++ festlegt; tut er
         das in einer Schleife weiter (bis zu einer Länge, die es nicht gibt), liefe er endlos.
         Nach zehntausend solchen Lesern gibt der Test den Aufruf auf und würfelt neu
         (`converted::open_read`).
       - Offen (wartet mit Grund): `pistol` (Feld-Zuweisung im Test), drei Funktionen mit
         `std::initializer_list<…>` als Argument, `Jet::step` und `PeopleArt::projectile` (ihr
         Quelltext mischt in `?:` eine Zahl mit einer Vektor-Komponente), Methoden schlichter
         Engine-Strukturen, die nur die Engine definiert (`MeshData::as_input`), Engine-Objekte,
         die eine Klasse als Wert hält.
     - [ ] (d) **Volle Geschwindigkeit im Export:** In der exportierten Klasse ruft die Funktion
       eines Engine-Knotens die Funktion der Engine direkt und typisiert
       (`world_of(welt)->cast_ray(a, b)`), ohne Werte zu verpacken; die Tür bleibt für Interpreter
       und Tests. Derselbe Generator schreibt das. Vor dem Umschalten der ersten Datei.
   - **Kein Zwischenspeicher im Konverter.** Jeder Lauf liest den Syntaxbaum frisch vom Compiler
     und übersetzt jeden Test neu – ein Baum zu einem anderen Stand der Header ist ein anderer Baum.

### 5 – Umschalten
Eine Datei schaltet um, sobald sie ganz durchläuft: Ihr C++ wird gelöscht, das Spiel läuft auf
ihrem Blueprint, und ihre bisherigen Tests prüfen den Blueprint. Bis dahin ist das C++ die Quelle,
aus der der Konverter den Blueprint schreibt, und der Vergleichstest hält beide gleich.

### 6 – Das Projekt laden
Das Projekt ist die Datei des Levels: `level/spawncity.klevel` mit den Actors, ihren Blueprints,
den Typen des Projekts und den Daten – dazu das Seitenmodul (`web/`: Ansicht, Menüs, Ton).

- [x] Der Editor öffnet die Datei wie jedes Level (Kern Studio: Open); jeder Blueprint ist im
  Blueprint-Editor als Graph und als C++ zu sehen und zu ändern.
- [x] `tools/blueprint_cpp.sh level.klevel out.cpp` schreibt die Blueprints als Klassen;
  `tools/export_game.sh level.klevel spiel.html` baut daraus das eigenständige Spiel.
- [ ] Das Projekt nennt sein Seitenmodul und die Einstellungen der Laufzeit (Schleifenwächter,
  Schrittweite), damit der Export des Editors ohne `build.sh` auskommt.
- [ ] Solange noch C++ in `src/` steht, baut `build.sh` beides zusammen: das Level mit seinen
  Blueprints als Klassen und das restliche C++.
- [ ] Der Export des Editors ist das Drop-in für spawncity.de (CI vergleicht); `src/` ist leer und
  wird gelöscht.

## Regeln für jeden Schritt
- Es gibt zu jeder Zeit genau eine Quelle einer Regel oder eines Systems. Für die Regeln der
  Stufe 3 ist das der Blueprint. Für eine Datei, die der Konverter noch nicht ganz schreibt, ist
  es ihr C++: Der Blueprint daneben ist daraus erzeugt (nie von Hand geändert) und durch den
  Vergleichstest an das C++ gebunden, bis die Datei umschaltet.
- Werte und Verhalten bleiben die des Originals; die bestehenden Tests wandern mit und prüfen
  danach den Blueprint.
- Box3D wird nicht getunt; Physik bleibt nach `kern/docs/physics.md`.
- Die Engine und die Blueprint-Sprache bleiben allgemein: Was hier dazukommt – Knoten, Typen,
  Fähigkeiten der Sprache –, ist für jedes Spiel gebaut, nicht für SpawnCity. Code von SpawnCity
  wandert nicht in die Engine. Was nur
  SpawnCity ist (Werte, Regeln, Inhalte), steht im Projekt: in seinem Level, seinen Blueprints
  und seinen Daten.
