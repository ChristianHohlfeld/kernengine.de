# Übergabe Paket k („die Kette“), 18.11.2026, etwa 17:20

## Stand
- Zweig `work-k`, letzter Commit **e5f2bf1** (gepusht). Er liegt auf origin/work 955503b; origin/work ist inzwischen 9 Commits weiter. Vor dem Mergen muss rebased bzw. gemergt werden. Die Änderung ist lokal auf main/job_of begrenzt und sollte deshalb sauber gehen.
- Commits:
  - 0181688: Ablauf in Runden.
  - e5f2bf1: Meldung „<Einheit>: f() was written and waits now (<Grund>)“.
- Der Stand übersetzt (py_compile grün). Geändert ist nur tools/cpp_to_blueprint.py:
  - main: der Abschnitt „the Blueprints, each after those it calls“
  - job_of: need_of ist herausgelöst
  - neue Hilfsfunktion convert_timed nach convert_unit
- Unit und TestWriter sind unverändert.
- Zweig `work-k-zeit` (bc0ddda, vom Vorgänger) enthält nur Zeitmessung. Nicht mergen.

## Der Entwurf (was main jetzt tut)
- **convert_unit ist eine reine Funktion seiner Eingabe** (job_of). Die Eingabe besteht aus:
  - den Dateien der Einheit,
  - wanted[u] (nur bei Engine-Headern),
  - tables[d] und exports[d] für alle d in need(u). need(u) ist die transitive Hülle von deps[u]; im Zyklus gehört u selbst dazu.
- **Fassung einer Einheit:** `gave[u]` ist die Liste ihrer verschiedenen Ergebnisse (table, exports), verglichen mit ==. `version[u]` ist der Index des aktuellen Ergebnisses darin.
- **Signatur:** `asked_of(u)` = (Dateien, wanted, ((d, version[d]) für d in sorted(need))). Die Signatur legt die Eingabe exakt fest, ganz ohne Hash.
- **Neu umwandeln:** nur wenn die Signatur sich gegenüber `given[u]` (Signatur der letzten Umwandlung) geändert hat. Gleiche Eingabe heißt gleiches Ergebnis, nichts wird doppelt gerechnet. Es gibt keinen Zwischenspeicher zwischen Läufen.
- **Runde:**
  - `new` = die Einheiten mit geänderter Signatur.
  - `now` = die Einheiten aus `new`, bei denen kein d in need(u) ∩ new außerhalb des eigenen Zyklus liegt (`u in needs[d]` heißt: gleicher Zyklus).
  - Alle jobs von `now` werden aus demselben Stand gebaut und zugleich abgegeben, die längsten zuerst: nach der Dauer vom letzten Mal (`took`), beim ersten Mal nach der Dateigröße. Die Reihenfolge der Abgabe ändert am Ergebnis nichts.
  - Erst wenn alle fertig sind, werden die Ergebnisse in Namensreihenfolge eingesetzt.
  - Danach werden die „missing“ ausgewertet: deps[u] bekommt d dazu; ist d ein Engine-Header und fehlt key in tables[d], kommt key nach wanted[d]. Ist d noch unbekannt, wird es angelegt.
- **Was wegfällt:** cut_calls, „goes first“, „is converted again“, reaches.
- **Sicherung:** Kehrt ein Gesamtstand (asks, given) wieder, endet der Lauf mit SystemExit und den Namen.
- **Protokoll:**
  - je Runde eine Zeile „round N: K Blueprints“,
  - am Ende je Einheit einmal das Endergebnis, sortiert nach Namen, mit r["errors"], der Zeile „n / m functions“ und mit FUNCTIONS je Funktion „written“ oder dem Grund,
  - die Summenzeilen unverändert.

## Warum der Lauf endet und warum das Ergebnis der größte Stand ist (so dem Koordinator gesagt)
- deps und wanted wachsen nur.
- In den Runden wachsen die Einträge der Partner: aus „fehlt“ wird „wartet“, aus „wartet“ wird „geschrieben“, solange die Umwandlung monoton ist. Dann bilden die Runden (Jacobi von unten) eine aufsteigende Kette über endlich viele Funktionen. Sie endet im kleinsten Fixpunkt.
- Jeder Ablauf „eine nach der anderen“, wie im alten Lauf, bleibt unter diesem Fixpunkt. Beweis per Induktion: aus x ≤ lfp folgt F(x) ≤ F(lfp) = lfp. Was der Basislauf schreibt, schreibt also auch der neue Lauf. Keine Funktion wartet nur, weil ihr Partner später dran war. Echte Rekursion über Teile hinweg bleibt wartend, wie eine Funktion, die sich selbst ruft.
- Monotonie wird **nicht erzwungen**. Erzwingen hieße, ein altes Ergebnis festzuhalten, und das wäre ein Eingriff in das, was geschrieben wird.
- Ein Bruch der Monotonie wird gemeldet: „<Einheit>: f() was written and waits now (…)“. Das ist ein Befund am Inhalt für die Pakete an Unit/TestWriter. Ein bekannter Kandidat aus f9: Game/game_look_cpp fällt von 7/13 auf 0/13, sobald game_vr fertig ist, weil der Test dann als Ganzes nicht mehr übersetzt.
- Schwingt etwas, greift die Sicherung mit Namen. Das ist dann ebenfalls ein Befund, kein vorgesehener Ausgang.

## Entscheidungen und Vorgaben des Koordinators (sinngemäß)
- Nachtrag 16:40, in dieser Reihenfolge:
  1. eindeutig: gleiches Ergebnis bei jeder Kernzahl
  2. schnell
- Beweis:
  - zwei volle Läufe mit verschiedener Kernzahl müssen Level, tests/converted und convert.log byteweise gleich liefern,
  - gegenüber dem Basislauf ist keine Funktion verloren; was mehr geschrieben ist, wird aufgelistet,
  - die Dauer vorher und nachher wird genannt.
- Neue Auftragsart vom Koordinator: `$S/mag.sh konvert origin/work-k <kerne>`. Sie rechnet nur die Konvertierung des ganzen Spiels mit genau dieser Kernzahl. Der erste Commit von mag/ergebnis-<name> enthält Level und tests/converted, dazu mag-log/convert.log. Verglichen werden die Bäume der beiden ersten Commits. Die Auftragsart greift, sobald auf mag die neue Schleife läuft (nach dem vollen Basislauf).
- Zum Protokoll: je Funktion „written“ oder ihr Grund und die Summenzeilen müssen bleiben, weil andere Skripte sie lesen. Das ist erfüllt.
- Zum Enden: Die Sicherung darf nur Sicherung sein. Das Enden muss begründet werden (siehe oben), ein Schwingen ist als Befund mit Namen zu melden.
- Nachtrag 17:10: sparsam arbeiten. Der Stand steht in $S/stand/k.md. Nachrichten an main nur für eine Entscheidung, für „bereit zum Mergen“ mit Beweis oder für einen Befund, der andere betrifft.

## Bewiesen
- Lokal, klein: src/rules.cpp, spawn_guard.cpp, wanted.cpp mit KERN_JOBS=1, alter gegen neuen Konverter.
  - Ergebnis: Level, tests/converted, texts und das Log (ohne die Ablaufzeilen, sortiert) sind identisch („GLEICH“).
  - Ablauf: 2 Runden zu je 4 Einheiten.
  - Ablage: $S/k/lokal/ (c_old.log, c_new.log, t_*, texts_*, l_*.klevel).
- Hinweis zum Test-Verzeichnis: Es muss neben converted.h liegen (z. B. tests/converted_k im Spielordner, danach entfernen). Sonst steht „converted.h: No such file“ in allen Gründen.

## Unbewiesen / wartet (mag-Aufträge, nichts mehr abfragt)
- `work-k-konvert-165842-30470`: konvert origin/work-k 8 (abgelegt auf 0181688 oder e5f2bf1, je nachdem, wann mag ihn holt).
- `work-k-konvert-165847-30555`: konvert origin/work-k 40.
- `work-k-probe-162316-2567`: Probe über game.cpp, game_lamps, game_look.
- Ergebnisse kommen als Branches `mag/ergebnis-<name>`. Abfrage: `git ls-remote origin 'refs/heads/mag/ergebnis-work-k*'`. mag hing bis 17:10 an anderen Aufträgen und am vollen Basislauf `mag/ergebnis-work-voll-153849-12713` (auf 955503b, Start 15:39; um 17:00 noch ohne „convert rc=0“). Dieser Basislauf ist der Vergleich für „nichts verloren“ und liefert die Vorher-Dauer (mag-log/when, „real“ in convert.log).
- Wurden die Aufträge auf e5f2bf1 gerechnet, gelten sie für den heutigen Stand. Wurden sie auf 0181688 gerechnet, liegt der einzige Unterschied in der Meldezeile; Level und Tests sind davon nicht berührt.
- Nächste Schritte:
  1. Die ersten Commits der beiden konvert-Branches vergleichen: `git diff --stat A B`, der Baum muss gleich sein. Dazu convert.log beider Läufe ohne „round“-Zeilen und ohne „real/user/sys“ vergleichen.
  2. Gegen den Basislauf: je Einheit den letzten Block nehmen und prüfen, dass jede dort „written“-Funktion auch neu „written“ ist. Mehr Geschriebenes auflisten.
  3. Die Meldungen „was written and waits now“ heraussuchen und als Befund an main geben.
  4. Die Dauer nennen.
  5. „bereit zum Mergen“ an main.
  - Für den Abschlussbericht: Zweig + Commit, was geändert ist, Beweis, Dauer vorher/nachher, zusätzlich geschriebene Funktionen.

## Fallen
- Der alte Basislauf hängt selbst vom zeitlichen Ablauf ab und druckt Einheiten mehrfach. Vergleichbar ist nur der letzte Block je Einheit plus die Schlusszeilen. Im Wortlaut „is not written“ gegenüber „which waits“ kann er vom neuen Lauf abweichen; das ist kein Fehler des neuen Ablaufs.
- Wird im SCC erstmals umgewandelt, sehen die Partner noch keine Tabellen. Jede Einheit im Zyklus wird deshalb mindestens zweimal umgewandelt: Ihre eigene Tabelle ist Teil ihrer Eingabe (need enthält u), so wollte es job_of schon vorher.
- Wandzeit ≈ Summe über die Runden der jeweils langsamsten Einheit. Dauert ein Game-Teil allein lange (Baum per clang bei jeder Umwandlung neu, bis zu 400 g++-Runden im Test), bleibt das die Grenze. Mögliche spätere Verbesserung: den Baum je Einheit über die Runden halten (ein Prozess je Einheit). Das ist innerhalb eines Laufs erlaubt, vorher aber den Speicher auf mag prüfen.
- PYTHONHASHSEED ist je Lauf zufällig. Die Worker werden geforkt und teilen ihn. Hängt Unit/TestWriter irgendwo an der Reihenfolge eines Mengen-Durchlaufs, unterscheiden sich zwei Läufe unabhängig vom Ablauf. Weicht der 8/40-Vergleich ab, zuerst dort suchen; das wäre dann ein Befund für die anderen Pakete.
- Nicht in den Ablauf gehören (Reihenfolge-Lauf der Teststücke): „Teststücke einzeln parallel übersetzen“ geht nicht. Ein Test ist eine Übersetzungseinheit, und die g++-Runden in TestWriter.write hängen voneinander ab.

# NEUER AUFTRAG (ab 22:25): Regeln des Konverters, die die Spiel-Suite rot machen
Worktree /home/claude/w-k, Zweig work-k (auf origin/work-zusammen 8844531a + eigene Commits). Arbeitsliste: PAKET3_k.md Nachtrag 22:25.

## Regeln des Konverters (Befunde und Fixe)
- **Gruppe 4 flak_cell (combat_rules.cpp:239):** `snprintf("%lld", static_cast<long long>(cy))` eines i64: ging über
  `int64_to_int` in Format Text (Int, genau bis 2^53). Fix 2a6ef5b3: `bits64(x)` (u64/i64 unter Casts, die 64 Bit behalten) +
  `format_values`: an %s gleicher Breite/`-` den Text der Bits (int64_text_signed d/i, int64_text u, int64_text_hex x);
  andere Flags/Präzision/X/o/c → Wait. std::to_string ebenso. Gilt in `formatted` (snprintf) und `logged` (fprintf).
- **Gruppe 7 Drone_step:** Die Abweichung steckt in `fw_.last_navigation_sequence_` (u16): Quelltext 20268, Klasse 85804 (= +65536).
  Ursache: `nav_seq_++` (u16, drone.cpp:322) wurde `wrap32(nav_seq_ + 1)` – ++ rechnete im Typ von x statt in int mit
  Rückwandlung. Ebenso nahm `x op= y` das Ergebnis ohne Rückwandlung in den Typ von x, und der schnelle Weg `x += 1` galt
  auch für u32/u16/u8. Fix 2a6ef5b3: `held_as(a, synth)` (Rechenart wie C++, IntegralCast zurück, wo narrow etwas abschneidet);
  `x += …` direkt nur für UNCUT = int, long, long long, unsigned long(long).
- **Gruppe 5 Car_new:** keine Regel des Konverters – `struct Applied { Vec3 p, f; i32 on; }; Applied applied_[6];` ohne
  Initialisierer: `Car(s)` lässt `on` unbestimmt (Quelltext 8 = Speicherrest, Blueprint 0). Der Test vergleicht einen Wert,
  den C++ offen lässt → BEFUND FÜR t1 (stand/k.md).
- Beleg-Auftrag für 4+7: work-k-vergleich-222911-25281 (combat_rules.cpp, drone.cpp auf 2a6ef5b3).
- Kleinstfall-Ordner: /tmp/k_mini (src/mini.cpp; Aufruf: cpp_to_blueprint.py --convert sc --texts … -I kern/include -Isrc -- src/mini.cpp, unter flock).
- 12e3da24: der Text eines Int64 im Format steht in einer eigenen Variable (`digits = int64_text_signed(cx);`) – die Klasse
  las sonst einen vergangenen Temporär-Text (ASan in 222911-25281).
- **Gruppe 6 CityStream_upload:** `t.cell_ids[key].push_back(id)` – variable_of (Kopie ändern, zurückschreiben): store für
  `m[key] = v` hebt `put = t_2` in self.pre → stand VOR `t_2 = m[key]`. Fix 20cb5259: Hebungen des Zurückschreibens kommen in die
  Zeilen danach. feed_terrain ist im Lauf 222045 nicht geschrieben.
- E3-C (p): keine Konverterregel – Wertliterale schreiben feste Felder als List (Sprache); die Klasse fordert dafür Speicher an → BEFUND d/p.
- Beleg-Auftrag für 12e3da24 + 20cb5259 (+2a6ef5b3): work-k-vergleich-224717-8097 (combat_rules, drone, city_stream).
- Danach: Liste aus „alle“ (ergebnis-work-zusammen-voll-203000-750-alle-212005-5593), Regeln des Konverters.
- **Belegt (work-k-vergleich-224717-8097):** G4, G6, G7 – BEREIT ZUM MERGEN 20cb5259 gemeldet.
- **buildings_near (u64-Schlüssel):** `[k, t]` über kern::Map<u64,…>: `.first` hatte tp Int (Compiler: `const unsigned long`).
  Fix fa3dc97d: held_type gibt für `.first` eines keyed_entry den Schlüsseltyp (rows()[2]). Beleg: work-k-vergleich-230203-14801
  (city_stream, destruction auf ad36eee9 = mit work-zusammen c6e4c1cf, t1s Testrahmen mit Ortsangaben).
- **Destruction_collapse/fracture:** Argument-Reihenfolge (GCC rechts→links) – ENTSCHEIDUNG NÖTIG in stand/k.md; nicht umgebaut.
- **12e3da24** zurücknehmen, sobald ps Fix (format_arg für std::string in kern/include/kern/logic/blueprint.h) in work-zusammen ist.
- Hilfsskript für alte Ausgaben ohne Ortsangabe: /tmp/k_diff.py <zeile.txt> (erste abweichende Zahl, Quelle gegen Blueprint).
- **E9 (Koordinator):** Reihenfolge, die C++ offen lässt = die von clang (von links). Absenkung destruction.cpp 90ecf01f + Differenztest;
  grün nur mit clang (wartet auf t1s run_tests-Umstellung). **Später (nach grüner Suite):** allgemeine Absicherung im Konverter – ein Aufruf
  mit mehr als einem wirkenden Argument wartet („C++ leaves open in which order the arguments …“); braucht Wirkungsanalyse (Lambda mit
  Referenz-Fang wie `tint` zählt).
- **E13:** ganze Spiel-Suite mit clang (t1). 90ecf01f dann zurücknehmen (eigener Commit), nachdem vergleich destruction.cpp unter clang
  sauber ist. Destruction_blast 1/161 ist dieselbe Ursache (pieces_[1].size). Kleinstfälle in $S/k/.
- Offen: 12e3da24 zurücknehmen, sobald ps text_format-Fix in work-zusammen ist; Liste des vollen Laufs work-zusammen-voll-232247-24031
  für meine Dateien (city_stream, destruction, combat_*, drone, car, Firmware-Einheiten; Game-Dateien hat Paket g).
- c32f607b: Diagnose im Ablauf für pendelnde Blueprints (goes round: …). Lauf 232247-24031 endete mit Pendeln von 30 Game-Einheiten.
- Stand 23:58: work-k = c32f607b (enthält work-zusammen 5a445af8). Gemergt in work-zusammen: 20cb5259 (G4/G6/G7), fa3dc97d.
  Nicht melden: 90ecf01f (E13). Offen: siehe stand/k.md „WARTET AUF“.

# AUFTRAG 04:40 (Koordinator, KOORDINATOR.md „04:40“ B0–B3)
- work-k: Merge work-zusammen 12438d1c (Konflikt im Ablauf: Fassung von work-zusammen genommen, sie enthält c32f607b), Merge work-t1 c5b8ab99,
  dc7a12cc Rücknahme 90ecf01f (E13), **8b8c0e10 B0**: dateieigene Konstanten gleichen Namens in mehreren Quelldateien der Teile eines Blueprints
  → `<datei>_<name>` (survey_one "constants", main nach part_numbers: World.constant_names, Unit.constant). Kleinstfall /tmp/k_mini2 (Aufruf:
  `cd /tmp/k_mini2; FUNCTIONS=1 KERN_JOBS=1 flock $S/heavy3.lock python3 /home/claude/w-k/tools/cpp_to_blueprint.py --convert sc --texts t -I/home/claude/w-k/kern/include -Isrc -- src/w.cpp src/w_a.cpp src/w_b.cpp`; Quelle in namespace sc!).
- Beleg-Auftrag: work-k-konvert-044336-12397 (Erwartung convert rc=0, „cannot compare it yet“ ~11). Danach BEREIT ZUM MERGEN + Nachricht an main.
- Danach B1 (Mat4_perspective .m[15]), B2 (SoundFx `.l.voice` u64), B3 (Game-Teile; Ergebnis work-zusammen-suite-alle-043356-8771).
- 05:40 B0 BELEGT (work-k-konvert-044336-12397 rc=0, 1695/1652, 20 wartend) → BEREIT ZUM MERGEN 8b8c0e10 gemeldet.
- cdcd2f51 B1: Test-Schreiber – memset auf f32-Liste → converted::layout::set_bytes<true> (converted.h). Ursache: `sizeof` wird im Test die Größe im
  Programm (64), die Test-floats sind 64-bittig → memset nullte nur die Hälfte (m[15] blieb 1). Kleinstfall /tmp/k_sb/t2.cpp.
- 5d804704 B2: qtype + Tree.wide_names (WIDE_NAMES): Alias, der einen 64-Bit-Typ nennt (VoiceId = u64), wird kern::u64 statt `unsigned long`
  (vorher Feld Int, Array Int64). engine_api.py neu laufen lassen (6 s, unter flock): 8 voice-Eingänge der Mixer-Knoten Int64. Kleinstfall /tmp/k_v.
  Risiko: das Level des letzten vollen Laufs hat Mixer-Knoten mit Int-Eingang – vergleich zeigt, ob der Leser das annimmt; Engine-Suite nötig.
- Beleg B1+B2: work-k-vergleich-054057-841 (sound_fx.cpp, game_life.cpp, game.cpp; erwartet: Mat4_perspective/projection_matrix, SoundFx_*, life_hud ohne Abweichung).
- B3 aus work-zusammen-suite-alle-043356-8771 (entpackt in $S/mag/<name>/e/): mp_hud_json 7/20 (`"connected"` Quelle false / Blueprint true),
  draw_events, gaps_guided, rocket_smoke_emit, spawn_squad/tank (rng_.s), equipment/look: ASan SEGV in libc. number_at → BEFUND t1 (UB).
- 09dd6818 B3: steps_in → assigned(steps=True): eine Änderung ohne Variable der Funktion (Feld von this, *unique_ptr) zählt als Schritt; damit
  bekommt `P& r = g.a[g.cur++ % n]` / `const Lane& L = lanes[rng_.next() % n]` seinen Index einmal (`r_at`). Kleinstfall /tmp/k_r.
  Beleg geplant: vergleich game_foot_rounds, game_military, game_gaps, game_events, game.cpp (nach 054057-841).
- 06:00 B1+B2 BELEGT (work-k-vergleich-054057-841, 46/46 beide) → BEREIT ZUM MERGEN 5d804704 gemeldet. Engine-Suite für die Tabelle offen (voller Lauf).
- work-k-vergleich-060032-8835 (09dd6818): 52/52 ok, aber die B3-Funktionen waren NICHT geschrieben (unit_hash in gaps_rules.cpp, lane_signature in
  road_graph.cpp, put in combat_art.cpp fehlten) – kein Beleg. Neu: work-k-vergleich-062739-13858 (foot_rounds, military, gaps, gaps_rules, road_graph, game.cpp).
- Befunde t1 (equipment ui_radio SEGV, number_at UB) und offene Punkte (mp_hud_json connected, draw_events) in stand/k.md.
- 07:25 B3 (09dd6818) BELEGT für rocket_smoke_emit/gaps_guided (work-k-vergleich-062739-13858; real-FAIL nur LECK von d) → BEREIT ZUM MERGEN 09dd6818.
  Nächster Schritt für den Nachfolger: nach dem vollen Lauf des Koordinators spawn_squad/spawn_tank, life_hud, draw_events, mp_hud_json prüfen;
  Engine-Suite mit der neuen Engine-Tabelle (5d804704) prüfen.

# NACHFOLGER 3 (ab 07:30, 19.11.) – Aufgaben mp_hud_json, draw_events, Session_update/life_hud, Terrain_run_job, Warteliste
- work-k: Merge work-zusammen 821208ac; **ed63dca3** Reihenfolge der Argumente (operand/calm/unmoved, in invoke, engine(), format_values): braucht ein
  Argument Schritte (hoist), werden Argumente links davon, die die Schritte sehen/ändern können oder selbst rufen, vorher in `arg`-Variablen gehalten.
  Ursache mp_hud_json: `esc(net_.status())` → `a_2 = Session_status(net_)` vor hosting()/connected(). Kleinstfall /tmp/k_ord/src/o.cpp (pair(peek(c), bump(c))).
  **3878258a**: die Menge der Hilfsvariablen heißt `self.temps` (`self.made` überdeckte die Methode made → TypeError, Lauf 075447 convert rc=1).
  **4e72f8b4** (Test-Schreiber, wie B1): `memcpy(bytes, &rec, sizeof rec)` einer Struktur mit f32 → converted::layout::to_bytes (Bytes des Programms).
  Ursache Terrain_run_job (jobs.submit, DemWindow-Bytes ab 16). Kleinstfall /tmp/k_mc/t.cpp.
- draw_events: kein Konverterfehler – Test gibt combat_kGates, nicht combat_rules_g_gates → BEFUND FÜR t1 (stand/k.md).
- Session_update, life_hud: in 043356 ohne Abweichung (nur cost).
- Beleg-Auftrag: work-k-vergleich-081204-23050 (game_net, net_session, combat_rules, game_events, terrain, game.cpp auf 3878258a).
- Aufgabe 5: „may run handlers“ ist eine Prüfung der Sprache (notation.cpp places_ok) → ENTSCHEIDUNG NÖTIG in stand/k.md (Vorschlag A: Laufzeitprüfung).
  Der Konverter gibt bei const-Referenz eine Kopie (place_arg raising) – bei Car (Destruktor mit Rumpf) falsch; vermutlich Ursache der ~Car-Wartenden
  draw_vehicles/draw_remote_vehicles/swarm_draw_vehicles (unbelegt). „a member of Int is set“ (vr_comfort, look_draw, begin_roads): Engine-Struktur
  (TextureDesc mit `const u8* rgba`/MeshLook?) wird als Int geführt – nicht untersucht, eher Paket d.
- 10:20: 3878258a gemergt (work-zusammen f0450061; work-k hat es eingemischt). Belege 081204 (58/58), 091538 (54/54), 095224 (60/60) – kein Rückschritt;
  mp_hud_json in keiner Probe geschrieben (Kette mp_local_ready → mp_art → mp_rules → player_position …) → voller Lauf 100601.
- Session_update (080335): BEFUND FÜR t1 – arg(E) im Testrahmen gibt die C++-Zahl eines Enums statt seines Index (ClientState −3…3).
- E18 (Koordinator 09:55: Weg A) – ENTSCHEIDUNG NÖTIG zur Form in der Klasse (V1 Prüfung im Text um den Aufruf / V2 in jeder Listenoperation), siehe stand/k.md.
  Erkenntnisse: Interpreter Instance::call(places) logic.cpp ~2590 (Spot = Schlüssel + Pfad, je Zugriff neu aufgelöst); JIT ruft func.call über die
  C++-Funktion des Knotens (keine eigene Arbeit); Klasse = Text der Notation (call_args notation.cpp ~1425), Listenoperationen über logic::list::* (nodes.h).

# NACHFOLGER 4 (19.11., ab 11:55) – Destruktoren, Wartelisten-Regeln, ctl_strike_confirm
- Commits (work-k): cd69e381 Destruktor mit Rumpf (ends_itself) = Funktion `<Klasse>_end` (Tree.body, survey, convert_unit, wanted_name;
  Emit.run wie Methode; keine out_fields → „it gives nothing“, verglichen im Test der Funktion, die den Wert beendet); Emit.own_end ruft ihn in
  ended_parts/state_ended vor den Feldern. moved_from: Destruktor ohne Kopie/Verschiebung → std::move kopiert (alles bleibt).
  39ddc452 Const.inlined ohne constexpr (nur Konstanten gelesen) → `const Vec3 kWall = rgb(…)` (Interiors::build).
  8c91947d Unit.lambda_vars: benanntes Lambda als Prädikat in einem anderen Lambda (Marks::update/prune_walls); Gefangenes wird weitergegeben.
  fb37e05e Function.asks_engine (Tabelle "engine"): ein Aufruf, der die Engine fragt, ist unter cond_depth ein Hoists → chosen/either
  (ctl_strike_confirm: logic_select rief body() dreimal). bb6da523 `(x = v)` als Wert: Schritt davor, Wert = x. aa24c1f0 goto vorwärts:
  labels_of/block_steps, Schalter to_<marke>, goto_live je Block. 928b7571 TestWriter.gather/want_record: Destruktor einer gehaltenen Klasse
  wird mitgehalten (sonst fehlt ~Ragdolls in der Testkopie → Game::stop 8/20). 1e523e83 Merge work-zusammen (13:00).
- Kleinstfälle: /tmp/k_dt (Destruktor), /tmp/k_rgb, /tmp/k_rm, /tmp/k_sel, /tmp/k_as, /tmp/k_go. Lokal mit Test: das Test-Verzeichnis muss im Spielordner
  liegen (tests/converted_kX); TEST_KEEP=1 legt einen Ordner „1“ im Spielordner an – danach löschen.
- Belegt: 123309-19335 (53/53): Interiors::build, Game::stop, ~Ragdolls. Läuft: 131110-451 (controls, vr, weapons, world_marks, city_stream, game.cpp).
- Befunde: audio_init → d (Stellvertreter: sound_bank.make gibt 0); draw_combat → t1 (+0.5, wie draw_events).
- Offen, untersucht, nicht gebaut:
  * Decoder::step (mvt.cpp): `Layer& layer = tile_.layers.back();` – named places kennen .back()/.front() nicht. Ersatz durch den Aufruf bei jeder
    Verwendung stimmt nur, solange die Liste ihre Länge behält; dazwischen ändert `++feature_`/read_feature Felder von this → Index beim Binden
    halten (wie picks), dafür einen gehobenen Index-Knoten bauen.
  * handle_shots (world_->shots()), run_buildings (BuildingBuilder::chunks()): Platz der Engine → d.
  * hud_json: `for (char& c : s) … c = ' '` braucht einen Knoten „Code setzen“ in Text (alle Stufen) – Sprache.
  * GeoProjector::init: kern::fail ist variadisch → Engine-Knoten fail(Text) + format_values; Engine/Sprache (d).
  * „a pointer“: gaps_draw `for (Part* p : {&g.disc, &g.ember})` (Schleife über Plätze ausrollen), gaps_jet_audio (Zeiger in Liste, find-or-append),
    look_view_models/look_command (kern::Mesh* der Engine → Handle, d), Writer::end_layer (reinterpret_cast<const u8*>(&v.n) → Bytes eines f64).
  * List.g (worldsync_update): `Game* g` in einer lokalen Struktur = Zeiger auf das eine Objekt des Zustands; Methoden der Struktur greifen auf den
    Zustand zu. Feat.pts (minimap_json): Zeiger auf fremde Liste – ohne Kopie nicht abbildbar.
  * gaps_sirens: tuple mit Zeiger; swarm_update: find_if auf r.units mit r = swarms_[key] (spot mit pre); extract: Lambda mit auto.
