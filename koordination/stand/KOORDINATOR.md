# Koordinator – Mitteilungen an alle (neueste oben)
- 23:17 **Der volle Lauf rechnet: `work-zusammen-voll-230430-15353` auf aec5ded8, Start 23:13:36** (mit t1, k 20cb5259, d Array-Fix, p Teil 1). Das Budget
  der Warteschlange steht jetzt in der Datei `cap` auf mag/auftrag (84: voller Lauf + eine Engine-Suite + ein Vergleich) – Vergleiche warten, bis sie passen.
  · **Entscheidung E10 (p):** der Wert an einem Eingang vom Typ „Liste fester Länge“ wird als `Fixed<T, N>{…}` geschrieben (wie bei Feldern; Bedeutung und Graph
    gleich, der Text der Klasse ändert sich) – JA: Länge aus dem Typ des Eingangs, der Leser lehnt eine falsche Länge ab, Fixpunkt, Engine-Fall, Doku.
  · p: Fix `text_format` 25a7318f gebaut, lokal mit ASan belegt (Vergleich wartet); Aufrufweg an einen Actor gebaut, unbewiesen.
- 23:13 **work-zusammen = HEAD nach Merge work-d 50832229** (Array-Parameter fester Länge: `inertia[6]` liest 6 Zahlen; Beleg `work-d-vergleich-225255-10477`,
  converted_rigid_spec in beiden Durchgängen ok). Der volle Lauf `work-zusammen-voll-230430-15353` wartete noch und nimmt diesen Stand (mag löst den Stand beim Start auf).
  d: E7 umgesetzt (cf8786c), Schritt 7 gebaut (e02ea24, unbewiesen), CI build auf c0d6d24 grün, Engine-Suite wartet auf mag.
- 23:05 **work-zusammen = 946303c2** (+ t1 a48ae637, + k 20cb5259). **Voller Lauf abgelegt: `work-zusammen-voll-230430-15353`** (startet, sobald die laufenden
  Aufträge bis auf einen kleinen fertig sind; bis dahin nimmt die Warteschlange nichts Neues an). Alle: vor dem nächsten Vergleich `git merge origin/work-zusammen`.
  · **Entscheidung E9 (Reihenfolge, die C++ offen lässt):** destruction.cpp:191/194 – zwei Argumente eines Aufrufs ziehen aus demselben rng; GCC (der Test) rechnet
    von rechts, clang (der Compiler des Spiels, emscripten) und das Original in JS von links. **Es gilt die Bedeutung des Compilers, der das Spiel baut.** Die Quelle
    wird gesenkt (Werte in dieser Reihenfolge benannt; k), der Differenztest ist Pflicht und grün – **gebaut mit clang**; t1 stellt run_tests.sh so um, dass
    tests/lowered allgemein mit clang gebaut wird (keine Ausnahmeliste). Eine Wirkungsanalyse im Konverter (solche Aufrufe warten lassen) nicht jetzt.
  · **Entscheidung E8 (t1):** der Aufruf des Quelltexts läuft im Test auf genulltem Speicher – ein Mitglied, das C++ unbestimmt lässt (Car: `applied_[i].on`),
    ist kein Vergleichswert; genullt ist eine der Belegungen, die C++ erlaubt, und die, die der Blueprint hat. Versteckt nichts, was die Quelle festlegt.
  · **Sperre für `work`:** 12e3da24 (k) ist ein Ausweichen um den use-after-free von `text_format` in der Klasse (p behebt ihn) – erst zurücknehmen, dann nach work.
  · Offen in Arbeit: create_body/inertia[6] (d, Fix 5083222, Vergleich wartet), u64-Schlüssel einer Map (k, fa3dc97d, Vergleich läuft), Car (t1, Vergleich läuft).
- 22:55 **Erste Ergebnisse von `vergleich` – die Auftragsart läuft (16 min für zwei Quelltexte unter Last):**
  · `work-zusammen-engine-213241-11183` **rc=0** (Engine-Suite auf 8844531a mit ds Count<Per>-Fix).
  · `work-zusammen-vergleich-222045-20677` (destruction, city_stream auf 8844531a): Destruction_tear und CityStream_building_shape weichen nicht mehr ab (earcut-Fix
    wirkt); es bleiben Destruction_collapse 7/25, Destruction_fracture 11/20 bzw. 12/28, CityStream_upload 29/243 → k.
    **BEFUND FÜR t1:** die Tests von destruction und city_stream enden in beiden Durchgängen mit LeakSanitizer (19 bzw. 84 MB, Vektoren der Quelltext-Kopie nie
    zerstört) → rc=1, die Suite wäre rot.
  · **Verlorenes Ausgabe-Ende:** LeakSanitizer beendet den Prozess (_exit), bevor stdout geleert ist – in eine Datei ging der letzte Puffer verloren (FAIL-Zeile,
    Summenzeile, letzte Abweichungen). **lauf2.sh febe6fc8:** Einzelläufe schreiben über ein Terminal (script), neue Zeilen LECK / ABBRUCH / ZEIT in alle.txt.
    **Ergebnisse von `vergleich`/`alle`, die vor 22:48 gestartet sind, können am Ende unvollständig sein.** Der alte Lauf „alle“ (212005) ist abgebrochen, neu:
    `ergebnis-work-zusammen-voll-203000-750-alle-224816-8576`.
  · k: G7 (Drone_step) in beiden Durchgängen sauber; G6 CityStream_upload behoben (20cb5259: `m[key].push_back` schrieb den alten Inhalt zurück); Beleg gebündelt
    `work-k-vergleich-224717-8097`. **BEFUND FÜR p (von k):** use-after-free in `text_format` der Klasse (liest einen vorläufigen Text) – k umgeht es vorläufig im
    Konverter (12e3da24), der Fix gehört in die Klasse. **BEFUND FÜR d (von k):** `inertia[6]` bei create_body/set_mass_data wird als 1 Zahl aufgezeichnet
    (create_body 300/300). **E3-C** ist keine Konverter-Regel: Wertliterale eines Fixed schreiben List, die Klasse fordert Speicher an → p. Alle gemeldet.
- 22:45 Zweiter Neustart des Servers hinter der Sitzung (~22:43): t1, t2, k, d, p abgebrochen und aus ihrem Verlauf fortgesetzt. mag und CI liefen weiter.
- 22:57 **Entscheidung E7 (d):** `list_append` auf einem Platz der Engine wächst wie `insert` der Bibliothek (`list.insert(end, n, blank<C>())`, dann
  `Direct::from` je Eintrag) – dieselbe Regel wie für Blueprint-Listen (insert_items) und den Quelltext; keine eigene Reserve-Regel am Platz. Bedingungen: Werte in
  allen drei Stufen gleich (Engine-Fall), keine Sonderbehandlung, andere Listenschritte am Platz ebenso angleichen. d (Nachfolger 3): Klasse, Palette, Doku gebaut
  (c0d6d24, 42c86fe, 62d0978), Engine-Suite `work-d-engine-223942-31317` wartet, CI 44403776223 läuft.
- 22:35 **mag-Warteschlange neu (lauf2.sh 43cc7e0b):** angenommen wird nur, was ins Budget an Kernen passt (70 von 56 Threads: ein voller Lauf + ein Vergleich);
  volle Läufe zuerst, dann vergleich/alle, dann der Rest. **`./mag.sh abbrechen <name>`** beendet den EIGENEN Auftrag (er schreibt sein Protokoll und räumt auf).
  Ursache der Überbuchung war die Schleife selbst: nach jedem Ersetzen zählte sie die laufenden Aufträge nicht mehr.
  · **work-zusammen = e9eec025**: + p Teil 1 (bis 7aacebfb; Beleg `work-p-engine-212302-7164` rc=0). p-Nachfolger gestartet (E2.1/E2.2 beweisen, dann +,−,× und Aufrufweg).
  · k (22:27 gestartet): G4 (`%lld` eines i64) und G7 (Drone_step: `u16 nav_seq_++` ohne Rückwandlung in den Typ) in 2a6ef5b3, Beleg läuft (`work-k-vergleich-222911-25281`).
    **BEFUND FÜR t1 (von k):** Car_new 300/300 – `Applied applied_[6]` ohne Initialisierer, der Quelltext lässt `.on` unbestimmt (gemeldet).
    **BEFUND FÜR k (von p, E3-C):** Firmware-Einheit: `fc_Filters` mit `List<…>` wo die Quelle feste Arrays hat (`Fixed<T, N>` gibt es) – Typung der Felder (gemeldet).
  · Abgebrochen: t1s zwei überholte Proben über alle Quelltexte; im Lauf „alle“ (212005) die Testläufe über 20 Minuten. Quote 22:31: **42 %**.
- 22:25 **Neu: `./mag.sh vergleich origin/work-<paket> src/a.cpp …` – Probe UND Vergleich Bit für Bit in Minuten** (PAKET_COMMON3.md, Nachtrag 22:25 – bitte
  lesen): eine Probe belegt keine Werte; ein Fix an einer Abweichung ist erst belegt, wenn `vergleich` in beiden Durchgängen (Stellvertreter, Engine selbst) keine
  Abweichung mehr nennt. **mag ist überbucht: keine Probe über alle Quelltexte mehr, höchstens ein eigener Auftrag zugleich je Agent.** Nach jedem Schritt
  committen/pushen (Läufe brechen ab). Chris, 22:16: „Just decide and do it“ → gestartet: **k** (neuer Auftrag: Regeln des Konverters, die die Suite rot machen –
  Gruppen 4–7 und was „alle“ zeigt; PAKET3_k.md Nachtrag 22:25) und **d, Nachfolger 3** (Platz der Engine beweisen und zu Ende bringen; PAKET3_d.md Nachtrag 22:25).
- 21:50 **Entscheidungen E4–E6 für p** (PAKET3_p.md, Nachtrag 21:50): Formel mit ganzzahligen Ein-/Ausgängen JA (Eigenschaft steht bei der Formel-Funktion,
  Beweis in allen drei Stufen an den Rändern, Fixpunkt); Weitergabe beim Anhängen JA; `at_known` zurückgestellt; p darf den Aufrufweg an einen Actor
  (func_call_on, Runtime::call_on, Instance::call) ändern – Text und Bedeutung unverändert, nach E2.1/E2.2; Zeit-Test ohne Ausnahmeliste, erst als Bericht.
  **BEFUND FÜR t1** (von p, E3-B): `cost:`-Zeilen mit 1–2 Anforderungen (combat_art put/beam, city_jobs put_*/read_*, destruction, city_stream) – entstehen sie im
  Test beim Umwandeln der Argumente (Mat4/Struktur aus Value)? CombatArt_put fügt wie die Quelle ein.
  · Stand 21:46: `work-zusammen-probe-213256-11283` convert rc=0 (city_stream, destruction auf 8844531a). t1: Fehler 1+2 gebaut (13464b5f), Anzeige der ersten
    abweichenden Stelle (e0e6c2ad), Proben laufen. t2: sscanf über converted::scan_c (06989d83). „alle“ rechnet noch.
- 21:35 **work-zusammen = 8844531a** (8874e79 + ds Fix für Zeiger-Eingänge, work-d 690ea4f): die Zahl neben einem Zeiger-Eingang sagt in ihrem Typ, wie viele
  Zahlen jedes Gezählte ist (`kern::Count<Per>`, wie bei Ausgängen); earcut.triangulate/deviation nehmen `Count<2> vertex_count`; Tür und Klasse prüfen mit
  `ev::numbers()`; der Stellvertreter des Quelltexts zeichnet Per · Zahl Werte auf (`count_per`). Erzeuger ohne Unterschied (818 Knoten), keine Teile von
  „Platz der Engine“ dabei. Beleg bisher: Probe `work-d-probe-212615-8375`. Laufen: `work-zusammen-engine-213241-11183`, `work-zusammen-probe-213256-11283`
  (city_stream, destruction). **Alle: vor der nächsten Probe origin/work-zusammen holen.**
  · **d ist beendet** (Lauf zu Ende); „Platz der Engine“: Schritt 3 fertig, 4/5 gebaut, nichts davon bewiesen – Stand in `uebergabe/d.md` Abschnitt B. Nachfolger noch nicht gestartet.
  · ~21:30 Neustart des Servers hinter der Sitzung: t1, t2, p wurden dabei abgebrochen und sind fortgesetzt (aus ihrem Verlauf).
- 21:25 **Die erste ausgeführte Spiel-Suite ist ROT und unvollständig** (`work-zusammen-voll-203000-750`: convert rc=0, **suite rc=1**, engine rc=0).
  · Gelaufen sind nur 21 von 269 Tests (17 ok, 4 FAIL), dann brach ein ASan-Fehler in `converted_engine_geo_mesh_data` (MeshData_push_extra) das ganze
    Testprogramm ab; der Durchgang `SC_ENGINE=real` lief nicht. Auswertung von t1: `stand/t1.md`, Abschnitt „SUITE 203000“ (7 Gruppen) – **das ist die Arbeitsliste.**
  · **Zuordnung:** 1 (push_extra, Bereich hinter einem gezogenen Zeiger) und 2 (city_stream ×15: `self` als Wert, Zerstörung im Aufzeichnungsfenster) → **t1**;
    3 (earcut.triangulate: Länge eines Zeiger-Eingangs im Stellvertreter; danach Destruction_fracture/update/blast neu beurteilen) → **d**;
    4 (flak_cell: `snprintf("%lld")` mit großem i64), 5 (Car_new 300/300), 6 (feed_terrain, upload), 7 (Drone_step) → Regeln des Konverters, ordnet der
    Koordinator zu, sobald die ganze Liste da ist (f1/f2 laufen nicht).
  · **Neue Auftragsart auf mag: `./mag.sh alle <stand mit Level und Tests>`** – baut einmal und lässt jeden Vergleichstest in seinem eigenen Lauf rechnen, einmal
    gegen den Stellvertreter, einmal gegen die Engine selbst; ein Abbruch hält die anderen nicht auf. Ergebnis: `mag-log/alle.txt`, `alle-real.txt`,
    `alle-abweichungen.txt`, `alle-real-abweichungen.txt`, `*-cost.txt`, `einzeln.tar.xz`. Läuft seit 21:20 auf dem gültigen Lauf:
    `ergebnis-work-zusammen-voll-203000-750-alle-212005-5593` → `$S/mag/<name>/` nach `./mag.sh holen <name>`. Der Umbau des Testrahmens (t1: jeder Test im
    eigenen Kindprozess, real-Durchgang immer) bleibt gewollt – er gilt dann überall, auch in der CI.
  · `work-zusammen` geht NICHT nach `work`, solange die Suite rot ist. `work` bleibt 42831863.
- 20:35 **Stand:**
  · `work-zusammen-voll-195003-886` (c432166): wieder `convert rc=1`, derselbe Lesefehler („pad“ von Controls_DeviceInput). Ursache genauer: die Erklärung steht
    in der **Bibliothek** `controls_cpp`; Game benutzt sie und hat die Variable `pad` – gebunden steht die Erklärung im Graphen von Game. Der erste Fix (nur für
    Teile eines Blueprints) griff daneben. **Zweiter Fix 8874e79** (work-zusammen): die Erklärungen werden am Ende des Laufs einmal für das ganze Programm festgelegt.
  · **Lokal nachgewiesen, dass danach nichts weiter im Weg steht:** `$S/lesen.py <worktree> <texts>` liest die Texte eines vollen Laufs (`mag-zwischen/texts`) wie
    der Konverter ins Level (44 s). Mit der berichtigten Erklärung (`pad_2`) nimmt das Level **alle 156 Texte** des Laufs an. Lesefehler lassen sich so in unter
    einer Minute prüfen statt in einem Lauf auf mag.
  · Neuer voller Lauf: **`work-zusammen-voll-203000-750`** (auf 8874e79, 20:30). Basislauf `work-voll-162644-5528` ist fertig: convert rc=1 (19:01), suite rc=1,
    engine rc=1 – wie vorhergesagt ohne gültiges Level.
- 19:50 **Stand:**
  · Voller Lauf der Zusammenführung `work-zusammen-voll-185405-1226` (auf 9931aea): **Konvertierung in 27 min** (15 Runden, 37 Kerne) – geschrieben
    **1.660 / 1.878 Funktionen (88,4 %), 18.926 / 31.103 Zeilen (60,8 %)** (f9: 1.577 / 1.876, 16.136 / 31.096); 101 Funktionen „its test cannot compare it yet“,
    9 Rückfälle „was written and waits now“. **Engine-Suite grün (rc=0)** auf dem zusammengeführten Baum. **Aber `convert rc=1`:** der Leser weist das Level ab –
    `Game.cpp: error: the input "pad" of Controls_DeviceInput has the name of a variable or a function` (controls.cpp ist ein Teil von Game und kennt Games Variable
    `pad` nicht). Spiel-Suite darum wertlos (altes Level, rc=1).
  · **Behoben im Konverter (work-zusammen c432166, allgemein):** die Erklärung einer Funktion eines Actor-Blueprints (KERN_EXTERN) wird für einen Blueprint aus
    mehreren Teilen beim Zusammensetzen festgelegt – kein Eingang heißt wie etwas des ganzen Blueprints, in jedem Teil gleich (`settled_extern`).
  · Neu abgelegt: Probe `work-zusammen-probe-194959-848` (controls + game + game_controls, mit Level), voller Lauf **`work-zusammen-voll-195003-886`**.
    lauf2.sh 9d621c4: ein voller Lauf legt jetzt auch die Texte der Blueprints nach `mag-zwischen/texts`.
  · t1-/t2-Proben über alle Quelltexte sind fertig (`work-t1-probe-174058-4177`, `work-t2-probe-163129-9216`, beide rc=0, beide vor 18:04 gestartet: ohne Level).
- 18:55 **Stand:**
  · **Lesefehler bestätigt auf `work`** (Probe `work-probe-182522-630` auf 90b5ab8: net_session_cpp 52/67, „the number 78 names two nodes“, nicht gelesen) – der
    Basislauf `work-voll-162644-5528` endet also ohne gültiges Level. Auf work-f2 liest es (`work-f2-probe-182528-670`: net_session_cpp 67/67, rc=0).
  · **work-d ist in `work` gemergt** (4283186, Baum = 5367e14): Engine-Suite auf mag zweimal grün (`work-d-engine-174905-11207`, `-174911-11261`), CI 44351625418
    grün (native, build, determinism, input, e2e), Generator ohne Abweichung (818 Knoten). F1 ist damit behoben.
  · **k: 8 gegen 40 Kerne byteweise gleich** (tests/converted, 145 Dateien) auf e5f2bf1 und auf 3dd3338; alle vier Läufe `convert rc=1` nur wegen des Lesefehlers
    (das Level blieb, wie es war – die Gleichheit des Levels sagt darum nichts).
  · **Probe-Zusammenführung `work-zusammen` (9931aea)** = work + k + t1 + t2 + f2 + g2 + g1 + von f1 nur der Konverter; die fünf Absenkungen der Game-Dateien
    (g1 7d1d656; g2 9ce8133, 1380806, ef21598, ffa277f) sind darin zurückgenommen, die Absenkungen/Tests/Werkzeug/Engine-Header von f1 nicht dabei. Doppelt Gebautes
    auf je eine Fassung: `scope_for` (f2) statt `looked_from` (g1); `case_block` (g2, Merker je Fall) statt Merker um den ganzen switch (g1); `stood` (t1) statt
    `where_of` (f1). Voller Lauf darauf: **`work-zusammen-voll-185405-1226`** (neue Fassung: Level und Tests gleich nach der Konvertierung).
    Ist er gültig und grün, ist er der Beweis für genau diesen Baum → nach `work`.
  · mag: `work-s5-engine-163459-12172` rc=0 (auf e749453 – R129 hing also wirklich vom Ablauf ab); `work-f1-spiel-174450-8421` rc=123 (Differenztests von f1
    übersetzen noch nicht); g1-/g2-Proben über alle game*.cpp fertig (rc=0, vor 18:04: ohne Level).
- 18:27 **BEFUND (aus den zwei 40-Kern-Läufen von k, e5f2bf1 und 3dd3338, beide `convert rc=1`):** das Einlesen ins Level scheitert –
  `net_session_cpp.cpp: line 611, column 20: the number 78 names two nodes` → „the Blueprints were not read: the level is as it was“. Beide Läufe gleich
  (14 Runden, 41–45 min bei 40 Kernen). Steckt das auch in `work` (90b5ab8), endet der Basislauf `work-voll-162644-5528` ebenso ohne gültiges Level.
  Zur Klärung abgelegt (Probe mit Level und Texten): `work-probe-182522-630` auf origin/work, `work-f2-probe-182528-670` auf origin/work-f2 (je net_session + jet_seats + mp_rules).
  Dazu 8 Zeilen „was written and waits now“: Game/game_look_cpp ×7 (look_begin, look_blast, look_camera, look_apply_tier – „invalid use of incomplete type Renderer“ –,
  look_lap, look_people_budget, view_zoom), Game/game_lamps_cpp gaps_lamps_draw („‘Game’ has not been declared“) – BEFUND FÜR t2.
- 18:24 CI 44351625418 auf work-d 5367e14 (Fix F1 + Zwischenstand): build, determinism, native, input, e2e grün (native-spawncity lief noch); mag
  `work-d-engine-174905-11207` lief noch. Probe mit Level funktioniert: `work-probe-180405-238` rc=0.
- 18:10: Proben auf mag haben seit **18:04** das Level (kein „which the level does not have“-Artefakt mehr); volle Läufe liefern Level und Tests gleich nach der
  Konvertierung (`mag-zwischen/`). Einzelheiten: PAKET_COMMON3.md, „Nachtrag 18:10“.
- 17:50: alle Agenten gestoppt (Unterbrechung der Sitzung); Neustart nur auf Chris' Wort.
- Nichts geht auf `work`, bevor das Ergebnis von `work-voll-162644-5528` (läuft seit 17:38:59 auf 90b5ab8) übernommen ist.
