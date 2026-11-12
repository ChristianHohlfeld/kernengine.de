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
- [ ] Funktionen, die sich selbst aufrufen dürfen (heute eine Regel: Wiederholen ist Sache der Schleifen).
- [x] Blueprint-Bibliotheken: Funktionen eines Actors von anderen Blueprints aus aufrufen. Ein Blueprint sagt, wie die Funktion aussieht (`KERN_EXTERN StarsResult Stars(Actor who = Actor{0});` – kein Rumpf), und ruft sie mit **Call on Actor** auf dem Blueprint eines Actors auf, ohne Target auf dem des Levels: `func_call_on<"Stars">(target, kern::out(n7.stars), who);`. Das Level-Blueprint ist damit die Bibliothek für alle. Hat das Ziel die Funktion nicht, gelten die erklärten Defaults; Aufrufe liegen höchstens 64 tief. In Interpreter, JIT und Klasse dasselbe (Tier-Fall `call_on`).
- [ ] Typen (Aufzählungen, Strukturen), die mehrere Blueprints eines Projekts teilen – heute erklärt jeder Blueprint seine eigenen, und ein Aufruf über Blueprints hinweg gibt sie nach Platz bzw. Feld für Feld weiter.

### 3 – Die reinen Regeln als Blueprints
In dieser Reihenfolge (klein nach groß; jede Datei verschwindet, ihre Tests bleiben):
`combat_detail`, `spawn_guard`, `guided`, `jet_seats`, `lamps`, `gaps_rules`, `controls`,
`settings` (Datentabelle), `world_traffic`, `equipment`, `combat_rules`, `mp_rules`.

- [x] **`combat_detail`** – Actor „CombatDetail“ im Level: die Aufzählungen Critical, Round und
  Ballot, die Tabellen Profiles und Rounds (Listen der Strukturen CriticalProfile und RoundSpec),
  die Struktur Vote und zwölf Funktionen (IsCritical, DetonationAt, Accelerate, ClockUpdate,
  MomentumNs, BulletImpulse, CreateBallot, CastVote, BallotName …). `combat_detail.cpp` rechnet
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
  behält, und ruft auf. Der Blueprint rechnet wie das JavaScript des Originals in 64 Bit; drei
  Prüfungen, die genau auf dem Rand einer Totzone standen (0,1 als 32-Bit-Zahl liegt knapp
  darüber), prüfen jetzt knapp darunter.
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
- **Gemessen** (wie das Spiel gebaut, -O2, `SC_TEST_RELEASE=1 run_tests.sh controls_call_cost`):
  Ein Aufruf einer Regel aus C++ kostet als Klasse rund 0,2 µs (eine Formel), 0,3 µs (mehrere
  Knoten, eine Schleife, eine zweite Funktion) bis 0,7 µs (eine Struktur hinein und heraus). Je
  Bild sind das für die Steuerung und den Verkehr zusammen weit unter einer Millisekunde.
- Geschrieben wird so ein Regel-Blueprint als sein C++-Text und mit
  `tools/blueprint_cpp.sh --apply level.klevel <Actor> text.cpp` in das Level gelegt (dieselbe
  Lesart wie „Apply“ in der C++-Ansicht des Editors); das Level bleibt die eine Quelle.

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
- Die Engine und die Blueprint-Sprache bleiben allgemein: Was hier dazukommt – Knoten, Typen,
  Engine-Systeme, Replikation –, ist für jedes Spiel gebaut, nicht für SpawnCity. Was nur
  SpawnCity ist (Werte, Regeln, Inhalte), steht im Projekt: in seinem Level, seinen Blueprints
  und seinen Daten. Ein Engine-System bekommt nie einen SpawnCity-Sonderfall.
