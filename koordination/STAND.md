# Stand (Koordinator, 25.11.2026 22:25) – kurz; Archiv: UEBERGABE.md
## Ziel (Chris, 25.11. ab 21:53 – gilt vor allem Früheren)
EINE GUTE ENGINE zuerst: solide, stabil, einfach zu verwenden und zu verstehen, vollständiger Funktionsumfang, für VIELE Spiele. Kern: C++-Teilsatz == Blueprint
(Bit für Bit, beide Richtungen), Physik, Editor. SpawnCity ist ein Produkt aus der Engine und ihr Prüfstein – KEINE 1:1-Kopie des alten Spiels, Fehler des
Originals werden behoben. Der Koordinator entscheidet frei und vollständig für die Qualität der Engine. Geschwindigkeit danach.
Regel je Fähigkeit: bräuchte ein zweites, ganz anderes Spiel das genauso? Ja → kern/ (allgemein, ohne Wort von SpawnCity, Engine-Test, Doku, im Editor sichtbar).
Nein → Spiel. Nichts in die Engine nur wegen einer Eigenheit des Spiels. Ein Weg je Sache, ausdrücklich statt still, Fehlermeldungen mit Stelle und Grund.
Regeln der Agenten: $S/GEMEINSAM.md (Nachträge bis 22:05). $S = /tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad

## Bewiesen / live
- main 9ac74916 = Abschluss 1 (1.763/1.847 Funktionen als Blueprints, live). work 05b10cd1+: Editor öffnet das ganze Projekt (CI 45662141502), CI-Probe-Job.
- Seit Abschluss 1 sonst NICHTS Neues bewiesen. Alles Weitere liegt auf Zweigen.

## Läuft
- mag: Kandidat 4b work-kandidat-2115-voll-211441-4258 (Zweigstände 20:45 + Leser-Fix, mit main.cpp; allein auf mag). Liefert Beweis + neues Level + Warteliste.
  Kandidat 2 scheiterte am Leser (Game mit Engine-Platz; Fix work-leser 61d13f81, nur per Engine-Test belegt). cap 126. $S/mag.sh, $S/ci_nach_konvert.sh.
- Agenten (Opus 5.5 medium; Auftrag $S/AUFTRAG_<name>.md, Stand $S/stand_<name>.md):
  einstieg4 (work-einstieg2, viel lokal ungepusht): NEUER SCHNITT – Projekt-Datei .kproject über den Levels (Einstellungen, Levels, Start-Level, Programm-Blueprints),
    Knotentyp = Blueprint des Projekts (Daten, Library je Projekt), ein Einstieg kern::run(App&)/kern::run(Project) ohne schwache Symbole, Eingang „world“ an
    den Akteur-Knoten. Zuerst Doku-Abschnitt, dann bauen; alter Weg (project.program, --split-program, schwache Symbole, register_node_type) wird zurückgenommen.
  enginelib (work-enginelib): Engine-Bibliotheken aus der Engine statt als Kopien im Level; „Engine gewinnt“ bei alten Levels → macht heutige Levels wieder gültig
    (heute bauen alle Spiel-Suiten rot: logic_Graph 11/12, game_Level 9/10).
  test2 (work-test2): Test-Schreiber – offen 'command'/kern::game, game()↔namespace game, „too many errors“, NodeKind; volle GitHub-Probe 45701448388 läuft.
  umschalten-b (work-umschalten-b): Gesamtprobe – das ganze Programm als Klassen baut, linkt, läuft 600 Ticks nativ (Gerüst); Web-Bau läuft. $S/gesamtprobe.md.
  pruefer: unabhängige Prüfung aller Entwurfsentscheidungen von heute → $S/entwurf_pruefung.md (abgleichen, VOR dem Mischen umsetzen).
- Fertig/pausiert: tests (work-tests: 19/21 Spieltest-Dateien auf Klassen; wartet auf neues Level), tempo (work-tempo2: Konverter 46 statt 65 min, byte-gleich
  belegt; Testrahmen belegt), umschalten-c (Exporte, work-umschalten-c 405fc269), belege (beendet; Differenztests sind nur noch Werkzeug), game*, rest, engine, leser.

## Plan für die Engine (aus $S/grenze.md – Bestandsaufnahme Engine/Spiel: 28+39+23 Posten)
1. Laufendes: Beweis (4b), Einstieg neu geschnitten, Test-Schreiber, Engine-Bibliotheken. 2. Offenes Komponenten-/Steuerungsmodell (registrierbar, C++ und Blueprint).
3. Spielspezifisches aus kern/ heraus (Soldat/Waffen/HUD, Micro Duck/Walker, Stadt-Komponente, SpawnCity-Konstanten) in Beispiel-Bibliotheken; Test: kein Spielname im Kern.
4. Grundfunktionen: Eingabe-Aktionen, UI-Widgets, Spielstand, Levelwechsel, Mesh-/Skelett-Import. 5. Allgemeines aus SpawnCity hochziehen (mvt/tiles, perf,
   road_graph, QR; dann car, ragdoll, terrain, net_session); doppelte Wege abbauen. 6. Einstieg für ein zweites Spiel: neutrale Doku, drei Vorlagen als Prüfsteine.
Unsaubere Entscheidungen von heute, noch zu ändern: Editor-Blätter mit Schwelle 2000 (einheitlich machen), Engine-Objekte als nackte Int (typisierte Handles),
kern_text_length („letzter Text“, globaler Zustand), Hilfen entfernen (all_program-Angleichung, CI-Eingang beleg, embed --without Source nach dem Schalten).
Offen: Zahlen nur 64 Bit vs. Geschwindigkeit (Performance-Schritt, mit Messung); Konverter kein Fixpunkt über sein Level ($S/befund_fixpunkt.md).

## Nächste Schritte
1. 4b auswerten → $S/k4.log, $S/kandidat4_wartet.txt → test2. Grün → nach work (Absenkungen brauchen keinen Differenz-Beleg mehr: Spieltests + e2e + Vergleich).
2. Kandidat 5: + tempo2, test2, tests, umschalten-b, umschalten-c, enginelib (sobald Suite wieder baut). 3. Prüfer-Liste umsetzen. 4. Engine-Plan Punkt 2 ff.

## Nachtrag 26.11. 01:25 — Regeln für „sauber" (verbindlich)

1. **Ein gemeinsamer Stand.** work-enginelib führt work-projekt + work-sauber zusammen; darauf kommen work-umschalten-c (schon in work-projekt), work-test2, work-tests, work-tempo2, work-umschalten-b. Gate für jeden Schritt: native, build, determinism, input, converter-cases und der Engine-Teil des e2e grün. Kein Zweig läuft länger allein weiter.
2. **Engine-Tests hängen an keinem Spiel.** e2e ist getrennt in Engine-Fälle (eigenes kleines Projekt) und Spiel-Fälle.
3. **Maßstab ist ein zweites Spiel** (Zweig work-zweitspiel, Befunde in zweitspiel_befunde.md). Öffentliche Schnittstellen (Akteure, Komponenten, Level zur Laufzeit) werden aus dem entworfen, was beide Spiele TUN — nicht aus dem, was SpawnCity gerade nennt. Die Entscheidung „game::Actor/Komponenten/Level als Structs öffentlich" ist zurückgenommen.
4. **Voller Lauf erst nach den schnellen Prüfungen:** converter-cases grün, `engine_api.py --check` gegen Level/Texte = 0, Konverter schreibt Programm-Blueprints ins .kproject. Kein Zwei-Stunden-Lauf, der an Bekanntem scheitert.
5. **Nach `work`** nur der gemeinsame Stand mit vollem Lauf + ganzer CI grün (auch Spiel-Jobs).
6. Bewiesen heißt: CI-Lauf mit Nummer. „Gepusht" ist nicht „bewiesen".

Stand der Teile: Exporte (CI grün), Einstieg kern_main + Projekt-Einstellungen + Exporte am Programm (CI 45722359527 grün), Object<I>/Implements als Typ/KERN_PLACE/Blätter (CI 45717868592 grün bis auf Spiel-Jobs), Engine-Bibliotheken aus der Engine (lokal grün, CI offen), Pure-Regel + converter-cases (Probe 45725044600 rc=0). Kandidat 4b gescheitert (Pure sc_gamepad) — behoben auf work-test2.

## Nachtrag 26.11. 05:20

**Gemeinsamer Stand = work-projekt.** Gate ganz grün: CI 45747202997 auf 6d81d0a1 (native 56363/0 + nativer Fall, build, determinism, input, converter-cases, e2e, murmel-web). Enthält: Projekt über den Levels (.kproject, Editor), Einstieg kern_main/kern::run, Engine-Bibliotheken aus der Engine, Object<I> als einziger Weg (ab 551c00d0), Exporte {ptr,len} am Programm, Konverter mit --project, Murmel (kern/games/murmel).

**Mischbereit mit eigener grüner CI (Agent projekt mischt ein):**
- work-vorlagen 894def26 (CI 45749486609): Vorlagen + GameWorld::spawn (spawn_copy weg), set_property mit Grund, set_material_color, SoundBank::request_world, SpawnCity-Fahrzeuge als Vorlagen (Spiel erst im vollen Lauf belegt).
- work-grundlagen 12b2821d (CI 45749980165): AppRuntime, Ton (engine.mixer), Eingabe-Aktionen, Speichern/Laden, Widgets, build_native.sh, unbekannte Schlüssel = Fehler.
- work-test2 68b3f294 (CI 45752401389): Testrahmen in kern/include/kern/test/, kein Spielname im Konverter, Murmel-Vergleichstests ganz grün.

**Kandidat 5 (voll, mag, Stand ac68a5ca): gescheitert**, convert rc=1 nach 110 min, 1595 geschrieben, 294 warten. Fatal: Span<const float> bekommt float[3] (MeshData_push_extra, terrain_cpp). 91 Kompilierfehler in Vergleichstests (Engine-Typen fehlen im Test). → Agent konverter4 (work-konverter). Protokoll k5.log im Scratchpad.

**Offen:** Akteur-Verweis {WorldId, ActorRef}; Play Project im Editor; 3f; Welt-Einstellungen je Geräteprofil (build_level), add_pawn über spawn; cost-Mehrverbrauch Murmel (punkte/lade/steuerung); SpawnCity auf Engine-Ton/-Eingabe/-Speichern umstellen; Engine-Plan Punkt 4 (Spielspezifisches aus kern/ heraus), 6, 7; Performance-Schritt.
Nach `work` kommt erst der gemeinsame Stand mit vollem Lauf + ganzer CI grün (auch Spiel-Jobs).

## Nachtrag 26.11. 10:05

**Gemeinsamer Stand work-projekt: Gate ganz grün CI 45767152613 auf cc4c2673** (native, build, determinism, input, converter-cases, e2e, murmel ganz, murmel-web, native-build). Dazugekommen seit 05:20: Vorlagen-Nacharbeit (Welt-Einstellungen je Geräteprofil, add_pawn über Vorlage, Vorlagen-Name in validate + Editor-Auswahl; kein Spiel hält mehr einen Level-Wert), Akteur-Verweis ActorHandle {id, world}, Programm erreicht Akteure seiner Welten, Play Project im Editor (AppHost, AppRuntime), Programm-Blueprints als Klassen (--classes), Konverter: nur lesende Sicht = Wert, ein clang-Lauf je Datei ohne Adressen (setarch weg), Doku-Einstieg kern/README.md.

**Volle Läufe:** Kandidat 6 (50076a01) von mir 08:47 bei Runde 23 abgebrochen — Fehleinschätzung (er hing nicht, lief nur parallel zu 7 langsam). Kandidat 7 (cc4c2673) rechnet seit 08:04, NICHT abbrechen.
Befund aus k6.log: Runden 9–21 konvertieren dieselben 30 Blueprints (Warten eine Stufe je Runde, Schwingen v3->v2); 317× Wurzelgrund „Game im Test unbekannt (kern::net)". → konverter5 auf work-konverter: 19f8ed89 (reaching_own), 498cabb4 (Aliase in Engine-Klassen), b713e33c (impliziter Konstruktor nur bei berechnetem Feld), a5e92dc9; CI 45783404532 und Proben laufen.

**ENTSCHEIDUNG DES BESITZERS OFFEN — Zahlen:** Murmels App als Programm-Blueprint: Level-Wechsel Frame 348 statt 349, geschafft 770 statt 772 (f32 gewonnen_seit_ in der Quelle gegen 64-Bit-Float der Sprache; mit f64 in der Quelle nativ ebenfalls 348/770 — befund_zahlen.md). Vorschlag Koordinator: eine Zahl-Art (64 Bit), `float` in Subset-Quellen = Konverter-Fehler mit Ort und Grund, Quelle umstellen, erwartete Werte einmal für beide Seiten. Erwartete Werte NICHT geändert (Rechteprüfung verweigert). Murmel-Teil liegt als stash „murmel-programm-wartet" (Worktree w-einstieg2) und Patch murmel_programm.patch im Scratchpad — nicht dauerhaft gesichert.

**Offen:** 3f Rest (Testrahmen, Zeile in main.cpp); Murmel im Editor ganz durchspielen (hängt an der Zahlen-Entscheidung); SpawnCity auf Engine-Ton/-Eingabe/-Speichern; Spielspezifisches aus kern/ (Soldat, Waffen, Micro Duck); cost-Mehrverbrauch Murmel; Performance-Schritt.

## Nachtrag 26.11. 11:20 — ZAHLEN: Vorrang eins

Besitzer 10:49–10:53: „Gerundet ist nicht, wie wir es haben wollten." / „Wir haben Fließkomma, Determinismus in der gesamten Engine." / „Inakzeptabel." Befund: Float der Sprache = f64 (schon auf main), Quelle = f32; die Vergleichstests übersetzen die Quelle „mit den Zahlen eines Blueprints" und verdecken das. Ganzzahlen mit Breite am Platz/Int64 sind laut erster Messung gleich; einfacher Int nur bis 2^53.
Zurückgenommen: Regel „kern::Float 64 Bit + Runden" (64919bc5); Murmel wieder f32, 349/772, App wieder C++.
Freigegeben: koordination/zahlen/zahlen_typen.md — Zahl-Typen der Sprache = die des Subsets (f32, f64, i8…u64), getypte Operationen, Umwandlungen als Knoten, UB der Quelle = Konverter-Fehler, Engine-Funktionen gerufen statt nachgerechnet, Vorgaben Float = f32 / Int = i64. Stufen 1–8.
Stufe 1 (ehrlicher Testrahmen, KERN_TEST_SOURCE_NUMBERS, 8e3dc4c6 angefangen) auf work-projekt; Stufen 2–8 auf Zweig work-zahlen. Ziel-Beleg: ehrlicher Durchgang für Murmel und SpawnCity ohne Abweichung grün, wird Gate; alter Durchgang entfällt.
Nichts gilt als „bitgenau" und nichts geht dafür nach work, bis das steht. Murmel-Programm-Blueprint: koordination/zahlen/murmel_programm.patch (gegen cc4c2673).

## Nachtrag 26.11. 14:05

**Zahlen (Vorrang eins).** Stufe 1 fertig auf work-projekt: ehrlicher Testdurchgang gegen die echte Quelle (KERN_TEST_SOURCE_NUMBERS=1, tools/source_numbers.py, compare.sh; „erwartet rot"-Fälle numbers_f32/_sum/_i64). Messung (koordination/zahlen/zahlen_schaden.md): alter Vergleich überall gleich; gegen die echte Quelle weicht ~die Hälfte der Funktionen mit f32-Rechnung ab (car 24/51, drone 53/114, game+main 56/287 …), Ganzzahl-/Bit-Funktionen nirgends. B1 gebaut und belegt: musl-Mathe der Engine unter den Standardnamen in jedem Bau (std_names.c, flags.sh), nativ + Wasm bitgleich; Regel: Subset-Quellen und Engine mit clang. B5: NaN/Inf/−0 bitgenau. B4 (JIT ≠ Interpreter an Rändern) belegt und behoben (JIT ruft die Knotenfunktion; math.mod 209 statt 9 ms → Performance-Schritt). Entschieden: Datenmodell des Subsets = LP64 fest (size_t ≡ u64 …); Dezimal-Brücke rules::decimal entfällt mit f32 (Stufe 8); fehlende Mathe-Knoten (hypot …) als Engine-Knoten auf work-zahlen. OFFEN BEIM BESITZER: musl-Quellen sinh/tanh (sinh.c, sinhf.c, tanh.c, tanhf.c, __expo2.c, __expo2f.c aus musl 1.2.5) — Hosts gesperrt.
Zweig work-zahlen (Stufen 2–6 in Arbeit): numbers.h, elf Pin-Typen, getypte Knotenfamilien, Klassen mit C++-Typen, Konverter schreibt C++-Typen, Engine-Funktionen gerufen statt Graph; Engine-Suite dort lokal fast grün.

**Caches entfernt** (Vorgabe „kein caching"; Anlass: ein Test lief mit altem logic_cases.h aus dem Cache grün): alle actions/cache, mtime-Wiederverwendung, feste /tmp-Ordner; KERN_RUN_DIR je Lauf.

**Volle Läufe.** Kandidat 7 (cc4c2673): rc=1, fatal engine_input_actions (Handliste der Bibliotheks-Header) → behoben (Header werden gefunden; frühes Scheitern). Kandidat 8 (fa64fb01): ERSTER durchgelaufener Konverter-Lauf — rc=0, Level+Projekt gelesen, 0× „goes round" (k7: 619), 103 statt 178 min, 1631/1839 geschrieben, 1588 verglichen (alter Durchgang), 208 warten; Suite rot: Klassen kompilieren nicht (ActorHandle statt Object in Held<Struct>, behoben in 2436faf4). Kandidat 9 (work-projekt ac491f28, mit dem Fix) rechnet seit 13:55.
Gate work-projekt zuletzt ganz grün: CI 45813948422 (3e540160).
Nichts geht als „bitgenau" nach work, bis der ehrliche Durchgang für Murmel und SpawnCity ohne Abweichung grün ist.

## Nachtrag 26.11. 16:00

**Entscheidung Besitzer 15:22 — Presets/Bausteine:** Soldat, Waffen, Micro Duck, Fahrzeuge … wandern NICHT ins Spiel, sondern werden mitgelieferte Bausteine der Engine (zweite Schicht neben dem Kern): Vorlagen + Blueprint-Bibliotheken + Assets, im Editor als Bibliothek zum Hineinziehen. Regeln: nur die öffentliche Schnittstelle; als Blueprints (lesbar, kopier- und änderbar); abwählbar je Projekt. „Kein Spielname im Kern" gilt für den Kern. Bau NACH den Zahl-Typen.

**Wartende Funktionen (k8: 208)** — koordination/wartende_funktionen.md: 137 an EINER Ursache (Test nimmt Stücke anderer Dateien ohne deren Header → Basisklasse fehlt), lokal behoben (Fall far); 26 Typnamen-Verwechslung (drei Structs „Blast", hält Game::explosion); 23 Engine-Objekt als Int (wohl behoben seit 2436faf4); 6 Exporte out_len; 5 Getter anderer Zustand (3f); 6 einzelne; 4 heute nicht schreibbar (Engine-Schnittstelle: WorldSettings, Text::view, embedded_level, Sicht auf Listenteil). Agent konverter7 auf work-konverter.

**Zahl-Typen (work-zahlen):** Formel getypt, NaN-Regel als Regel der Sprache (kanonisch, wo Bits sichtbar werden), lokal alles grün (kern_tests 62.470, blueprint_native 83 Blueprints, game_native). CI noch nicht ganz grün (zuletzt UBSan: i64-Überlauf bei Add to Variable in der Klasse). Entschieden: KEIN -fwrapv; die Rechnung kommt vom Typ (Plätze der Sprache in Klassen = Typ mit den Operationen aus numbers.h). size_t: LP64 bleibt; Konverter-Prüfung (Hinweis) für Stellen, an denen die Breite das Ergebnis ändern kann — Zählung steht aus. JIT auf work-zahlen 1,0× (Performance-Schritt).

Kandidat 9 (ac491f28) rechnet seit 13:55, noch ohne Ergebnis.

## Nachtrag 26.11. 18:00

Besitzer 17:57: „Du brauchst nichts von mir, do it." Damit entschieden (Koordinator):
- **Bausteine** (koordination/bausteine_entwurf.md): Name „Bausteine"; strikt Blueprint, keine C++-Anteile; Vorlage im Level als Verweis; Soldat zieht erst mit Replikation von Blueprint-Variablen um; eigene Pakete von Projekten später; Apache 2.0 + Lizenzangabe je Asset mit CI-Prüfung. Reihenfolge 0 Schicht+grund/spielelemente, 1 spielfigur+kamera (Murmel), 2 fahrzeug+sitze, 3 leben/waffen/hud/soldat, 4 ragdoll/sitzung, 5 Micro Duck. Bau nach den Zahl-Typen.
- **sinh/cosh/tanh/asinh:** musl-Quellen nicht holbar (Hosts gesperrt, Repo-Zugriff abgelehnt — nicht umgangen). Die Engine bekommt eigene Funktionen (neu geschrieben über exp/expm1/log/log1p/sqrt der Engine), eine Implementierung für Quelle/Blueprint/nativ/Wasm, mit Genauigkeits-Test gegen unabhängige Bezugswerte. Agent gate2 auf work-projekt. Daran hängen 31 wartende Funktionen.
- **Live (main):** bleibt, wie es ist, bis die Zahl-Typen fertig sind.

**MEILENSTEIN Zahl-Typen:** Engine-Suite auf work-zahlen ganz grün, CI 45858436873 (2e13c032): kern_tests 63.340/0, blueprint_native_test 84 Blueprints/46.217 Aufrufe, game_native_test, ASan+UBSan ohne Meldung. Offen dort: Konverter-Ausdrücke (Stufe 5), Engine-Konstanten/Mathe-Knoten, Murmel neu erzeugen, Editor, JIT getypt inline, SpawnCity.
Kandidat 9 (ac491f28): convert rc=0, 1609/1839 geschrieben, 230 warten (31 an sinh/tanh/asinh); Suite: noch 21 Compilerfehler statt 3262. Mathe-Test auf dem GCC-Bau rot → behoben (Compiler an Makros erkennen, pow(x,2) nicht als x*x; 4eb11583).

## Nachtrag 26.11. 18:25 (Kontrolle)
- Richtung seit 18:07 (Besitzer): SpawnCity massiv verkleinern, Engine 100 % heute, Spiel lauffähig. Kandidat 10 = verkleinertes Spiel (work-spiel), NICHT mehr work-projekt mit vollem Spiel.
- kleinspiel (a7c43ec7…, work-spiel): Weg B freigegeben (sc::SpawnCity : kern::App auf behaltenen Bibliotheken); Basis 0a8f32cb + 058aa524. Wartet auf Probe rc=0/wartend 0.
- gate2: work-projekt 0a8f32cb (eigene sinh/cosh/tanh/asinh), Lauf 45865235947 läuft. musl-Frage erledigt (Besitzer: „schreibst sie selber").
- zahlen7 (work-zahlen fcbcc0ef, CI 45863021396 läuft): Konverter-Ausdrücke, Murmel bitgenau.
- editor (work-zahlen-editor, noch nicht gepusht): Beleg-Zweig work-beleg-editor-<commit> erlaubt (nur cp spawncity.html ausgelassen, nie mischen).
- konverter7: work-konverter 058aa524, CI 45865013913 läuft.

## Nachtrag 26.11. 18:50 (Kontrolle)
- work-projekt 0a7bba61 Gate 45867270705: native, build, determinism, input, converter-cases, murmel, murmel-web, native-build GRÜN; e2e läuft; rot erwartet: native-spawncity-build, e2e-spawncity. Fall hyperbolic dort ohne f32-Funktion (kommt auf work-zahlen zurück, an zahlen7).
- work-spiel c12c930c: Probe 1 (64de83ea) 548/577, Bibliotheken 0 wartend. Probe 2 rc=1: Konverter-Fehler bind(Runtime&) + Engine-Lücke „Regel-Akteure vom Programm aus C++" → konverter7: ActorBlueprint::bind(ActorHandle)/Call on Actor; danach 3 App-Lücken (Export auf App-Zustand, Zustand mit Basis/extensions, WorldSettings). Fassaden-/Wasser-Shader vorerst heraus. Laufzeit-Abfrage step_hz heraus (Test hält 240).
- work-zahlen-editor 57a5765e: Punkte 1–5 gebaut; Beleg-Zweig darf SpawnCity-Teil von „Assemble the site" auslassen.
- work-konverter 02c1ca55 (CI wartet).

## Nachtrag 26.11. 19:17 (Kontrolle)
- work-projekt 0a7bba61: Gate 45867270705 GANZ GRÜN (rot nur erwartet: native-spawncity-build, e2e-spawncity). gate2 beendet.
- work-konverter 02c1ca55 (bind-Abbruch behoben): Gate 45870839112 ganz grün (gleiche erwartete Rote) → MISCHBEREIT. konverter7 beendet; Nachfolger konverter8 (a474e916…): bind an Akteur einer Welt (ActorHandle, Call on Actor), App-Lücken a–c, Fall pieces Link, K2 (Text::view, embedded_level, Listen-Sicht).
- kleinspiel beendet (03a6c1c4); Nachfolger kleinspiel2 (a4e55eb3…), work-spiel 336e4157, Läufe 45873837774/45873952731.
- zahlen7: work-zahlen a5730d18; Konverter-Ausdrücke getypt lokal; cmath ohne Umrechnung; 8 Funktionen fehlen der Engine → Auftrag mathe (a320b739…, work-projekt): log2, cbrt, ldexp, acosh, atanh, exp2, log1p, remainder.
- editor: work-zahlen-editor f712aa51, Beleg-Lauf 45870721699.

## Nachtrag 26.11. 19:50 (Kontrolle)
- work-spiel 336e4157 (mit 02c1ca55): CI 45873952731 build GRÜN; rot: native-spawncity-build + e2e-spawncity (alte engine_*-Kopien im Level; small_level.py-Fix lokal), input (Maus-Fang/Stick im Kleinspiel — wird untersucht). Probe 4 rc=1 aus gleicher Ursache.
- konverter8: bind(GameWorld&) entworfen und im Bau (Fall worldrules/worldmix), noch nicht gepusht. C++-Form: `g_rules.bind(w)`.
- work-zahlen af3fb833 (Glue-Fix Exporte); CI 45875999709 (47aa67f5) rot — Jobs offen. Editor c836e0f5 eingemischt, CI 45877234547 läuft; e2e-Beleg 45874580610 grün.
- mathe (acht Funktionen): läuft, noch kein Push.

## Nachtrag 26.11. 20:16 — FREIGABE des Besitzers
- Wörtlich: „Mach alles final und du hast von mir das Go für main merge fully". → Nach Abschluss darf der Koordinator nach main mischen (ohne Rückfrage). Bedingungen bleiben: ganzes Gate grün (auch SpawnCity-Jobs mit verkleinertem Spiel), Deploy nur bei grüner CI, kein force-push, Changelog-Eintrag (Berliner Zeit) für das, was live geht.
- Weg: work-konverter(bind an Welt) → work-projekt; work-zahlen(+Editor, Murmel bitgenau) → work-projekt; work-spiel einmischen, voller Lauf auf mag (Kandidat 10), Level neu → Gate ganz grün → Changelog → main.

## Nachtrag 26.11. 21:25 (Kontrolle)
- work-konverter c2a349c5: ActorBlueprint::bind(world) (CI 45889096694 läuft). work-spiel fee55914 nutzt es (Probe 45889361172 success).
- leser (work-konverter-leser, lokal a885d6c0): URSACHE n124 = logic::link (kern/src/logic/logic.cpp) schreibt „bound" beim Kopieren von Bibliotheksfunktionen nicht um; Fall placelib alt rot/neu grün.
- work-projekt f2249a51: acht Mathe-Funktionen (log2, log1p, exp2, cbrt, acosh, atanh, ldexp, remainder), Gate 45887919129 läuft (converter-cases, murmel, build, input, determinism grün; native/e2e laufen).
- work-zahlen 9c2d1474: push_extra(Span), CI 45888727173 läuft.
- work-spiel CI 45885597397 (b5e16e81): input ROT (offen), native-spawncity-build/e2e-spawncity rot (Level alt).

## Nachtrag 26.11. 22:15
- ~21:50: ALLE Opus-Agenten endeten mit „monthly spend limit" (HTTP 429; Reset 2.12. 13 Uhr). Nachfolger auf Fable 5.2 gestartet: leser2 (ac0331a0…, w-konv2, lokaler a885d6c0 logic::link-Fix), konverter9 (a693bd07…, w-konv: freie Funktionen mit Welt-Akteur, 6 Exporte), zahlen8 (a1430cbc…, w-zahlen: unfertiger Merge work-projekt, Mathe-Knoten, Murmel-Beleg CI 45892882964), kleinspiel4 (ac993caf…, w-spiel c23667f8).
- work-projekt 486a6900 = Merge work-konverter c2a349c5, Gate 45893050993 läuft. Zusammenführen macht vorerst der Koordinator selbst.
- Besitzer 22:07: „Finish fully now, we need to finalize and merge into Main now". Bedingung bleibt: ganzes Gate grün inkl. SpawnCity.

## Nachtrag 26.11. 23:23 (Kontrolle)
- work-projekt 486a6900 Gate ganz grün (45893050993). work-konverter-leser b0eafcb4 (logic::link-Fix + TEST(logic_library_place)): CI 45901105063 alles grün außer e2e (läuft) und den zwei erwarteten.
- mag kandidat-10-223348 (5486b432, ohne Fixes): convert rc=0 nach 6 min, suite rc=123 (auszuwerten: kleinspiel4), engine rc=0 23:16. Voller Lauf des Kleinspiels ≈ 42 min.
- konverter9 lokal: Kleinspiel nur noch 4 wartend (SpawnCity::start an spiel.cpp:74 logic->budget) → Engine: GameWorld::start_logic setzt loopBudget aus dem Projekt; Zeile entfällt im Spiel. Noch nicht gepusht.
- work-zahlen e5d4061b (Merge work-projekt 486a6900 fertig), CI 45902650621 läuft.
- work-spiel a71d4ec2 (mit Leser-Fix), Probe 45901148156 success.
