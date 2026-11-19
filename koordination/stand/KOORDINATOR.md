# Koordinator – Mitteilungen an alle (neueste oben)
- 07:35 **Entscheidung E16 (Rekursion, Paket e):** die Sprache lässt zu, dass eine Funktion sich selbst ruft, direkt oder über andere – als Schritt (func.call);
  ein Kreis über einen pure gelesenen Wert bleibt ein Fehler. Grenze 65.536 geschachtelte Funktionsaufrufe in Interpreter und JIT (festgelegter Fehler, der
  Blueprint stoppt; Events/call_on bleiben bei 64) – über allem, was das C++ des Spiels erreicht (nativ ~40.000 Aufrufe, WASM ~2.600 Ebenen). Interpreter und JIT
  werden über ein Sprungbrett vom Maschinenstapel unabhängig (heute 1.504 B je Aufruf: in WASM nur ~350 Ebenen). Die Klasse: gewöhnlicher Methodenaufruf, ohne
  Zähler. Endlose Rekursion = „C++ leaves open“ (im Test aufgegeben und neu gezogen). **Neue Form der Klasse, allgemein:** der Wert eines Schritts, der nur auf
  dem Weg seiner Funktion gelesen wird, ist lokal in der Methode, sonst Mitglied – das ändert die Texte aller Blueprints; e baut das ZUERST und einzeln
  (Beleg Engine-Suite + vergleich + Zeit-Test), p baut darauf auf. Befund e: echte Kreise im Spiel sind nur {explosion, combat_blast} und
  {command, ui_command, ctl_*, ui_reset, vr_* – 21 Funktionen}; heute hat in keiner Stufe ein Aufruf eigene Werte (Fib(10): Interpreter −80, Klasse 5, C++ 55 bei
  abgeschalteter Leser-Regel) – die Regel nur zu streichen reicht nicht.
  · k Nachfolger 3 gestartet 07:25 (mp_hud_json, draw_events, Session_update, life_hud; dann die Regel-Gründe aus wartet_055004.txt).
- 07:20 **work-zusammen = 821208ac** (+ d work-d-belege2: leck, weld, app, key, release – Engine-Suite 062159 rc=0; + Tabellen-Fix 1007cb7e now_ms).
  Voller Lauf **`work-zusammen-voll-070328-32111`** rechnet darauf (Konvertierung bis ~07:45). Engine-Suiten grün: fe83683e (im Lauf 055004), f03f0f53 (060116).
  · **Die Spiel-Suite läuft ihre Tests nacheinander (ein Kern): 41 von 150 Tests in ~40 Minuten** – Suite von 055004 abgebrochen (40 ok, 1 FAIL
    converted_engine_math_math, vor ks Fix). t1 baut: Kinder nebeneinander (KERN_JOBS), Zeit je Test, CPU-Summe. Bis dahin gibt es Listen nur aus Einzel-Läufen
    (`alle`). **Alle: `mag.sh vergleich` braucht Pfade `src/<datei>.cpp`** (relativ zu kern/games/spawncity) – ohne `src/` schreibt die Probe keinen Test.
  · Neu gestartet: **e** (Rekursion – Entwurf zuerst; PAKET3_e.md) und **p** Nachfolger 2 (Zeit-Test, Entwürfe an den Koordinator). d5 beendet, alles gemergt.
- 06:48 **Entscheidung E15 (ersetzt E12; t2, Graph):** ein Typ aus kern::logic, den die Regel des Konverters selbst (Unit.engine_object) als einfache Daten
  einstuft (Graph, Node, Link, StateMachine, Function …), ist im Test eine Struktur wie jede Struktur der Engine – mitübersetzt, Feld für Feld gezogen und
  verglichen; seine Objekt-Teile (Variable, FunctionPin, json::Value) sind Stellvertreter mit Handle (wie TextureAsset in Level). Die Maschinerie der Logik
  (Runtime, ActorBlueprint, Instance) bleibt ausgenommen. Grund (Befund t2): über die Serialisierung lässt sich nicht Bit für Bit vergleichen – die Teile mit
  json::Value sind in der Sprache Handles, die der Stellvertreter nicht auflöst; Feld für Feld ist strenger. Bedingungen: keine Liste von Typnamen; Quelltext,
  der einen Graph an die echte Logik gibt, wartet ehrlich; kein Feld ausgelassen. Betrifft vehicle_actor, Game::audio_init, ui_audio_init (request_level/Level).
- 06:37 **work-zusammen = 0f05da13** (+ k 09dd6818, B3: Name für einen Eintrag mit Schritt im Index; Beleg `work-k-vergleich-060032-8835` 52 von 52 ok in beiden
  Durchgängen, kein Rückschritt – die vier Funktionen der Abweichung schreibt der Teil-Lauf nicht, ihren Vergleich zeigt der volle Lauf).
  · **BEFUND Koordinator (Konverter, seit 18.11. abends unbemerkt):** 33 Funktionen (1682 Zeilen: CityStream::update/work/…, Terrain::update/run_job/…,
    Decoder::step, Game::update/fixed_step/drive_traffic/worldsync_update/…) warten an „the engine's now_ms() (… its pins differ)“. Ursache: `engine_table()` las
    Tabelleneinträge mit Schaltern am Ende (`, true` place; `, false, true` clock – 8 Einträge) ohne ihre Ausgänge. Fix **work-tabelle 1007cb7e** (Schalter
    mitgelesen; ein Knoten mit place wartet weiter, mit dem wahren Grund, bis „Platz der Engine“ eingemischt ist). Beleg läuft:
    `work-tabelle-vergleich-063526-15825` (city_stream, terrain, mvt, game_look).
  · Was die 137 noch nicht geschriebenen Funktionen aufhält (Lauf 055004), je Grund und je Funktion: **`$S/wartet_055004.txt`**.
  · **Neu gestartet 06:36:** t1 Nachfolger 6 (A ZEIT converted_Game__game; B ein Test gibt bei jedem Lauf dasselbe Urteil + genauer Filter; C number_at/Zeiger
    hinter das Ende eines Texts; D Auswertung „SUITE 055004“) und t2 (E12: Graph als Wert, SoundBank::request_level – die letzten 3 „cannot compare it yet“).
  · d: Beleg-Zweig work-d-belege2 (leck, weld, app, key, release) mischt sich ohne Konflikt in 0f05da13 (lokal geprüft, Generator reproduziert); eingemischt
    wird, sobald `work-d-belege2-engine-062159-12230` grün ist.
  · Quote (Chris): 53 % um 06:34 (42 % um 22:31). Höchstens 4–5 Agenten.
- 06:30 **GÜLTIG: `work-zusammen-voll-055004-6649` auf fe83683e – convert rc=0 (06:19).** „1707 of 1844 functions of the program are written as Blueprints (22249 of
  30978 lines)“, „1664 functions … compared with their source“; **nur noch 3 Zeilen „cannot compare it yet“** (2 × get_u8, Graph) – 020121: 1637 / 1594 / ~116.
  `grundlage` für `vergleich` ist jetzt dieser Lauf. Die Suiten rechnen (Ergebnis ~07:10).
  · **work-zusammen = f03f0f53** (+ k 5d804704: 64-Bit-Aliase bleiben Int64 – voice-Eingänge der Mixer-Knoten; memset im Test → Mat4 .m[15]; Beleg
    `work-k-vergleich-054057-841` 46 von 46 ok in beiden Durchgängen). Engine-Suite dazu: `work-zusammen-engine-060116-9012` (läuft).
  · k arbeitet an B3 (09dd6818: ein Name für einen Eintrag mit Schritt im Index wurde je Nutzung neu ausgewertet – rocket_smoke_emit, spawn_squad, spawn_tank,
    gaps_guided; Beleg `work-k-vergleich-060032-8835` läuft). d: Lecks/weld/Maker (Belege laufen bzw. warten). t1 beendet (Nachfolger nach der Suite).
- 05:45 **Fortschritt, alles belegt:**
  · **Engine-Fix cpp_names (12438d1c): `work-zusammen-engine-043403-8808` rc=0**, und im Kontrolllauf `work-zusammen-suite-alle-043356-8771` kommt „Game runs as its
    C++ class“ nicht mehr vor: **Stellvertreter 138 ok / 12 FAIL / 1 ZEIT (vorher 113/35/4); Engine selbst 127 ok / 23 FAIL / 11 LECK / 1 ZEIT.** Auswertung t1:
    stand/t1.md „SUITE 020121b“.
  · **k B0 (kQuiet) gemergt – work-zusammen enthält jetzt work-k 8b8c0e10 samt t1b (c5b8ab99).** Beleg `work-k-konvert-044336-12397`: convert rc=0,
    **1695 von 1844 geschrieben (21450 Zeilen), 1652 verglichen, 20 „cannot compare it yet“** (020121: 1637 / 19218 / 1594 / ~116), 18 „waits for good“.
  · Rest gegen den Stellvertreter: k – Mat4 .m[15] (Ursache laut k im Test: memset auf f32-Liste, sizeof 64 gegen 128 Byte breite Test-Floats; cdcd2f51 unbelegt),
    SoundFx .l.voice (5d804704 unbelegt, braucht Engine-Suite), draw_events, rocket_smoke_emit, gaps_guided, spawn_squad/spawn_tank (rng_.s), mp_hud_json; number_at
    8/300 ist laut k ein Fall für t1 (from > s.size(): C++ lässt es offen). t1 – life (belegt), mvt, game_look, game_equipment, get_u8 (gebaut, Beleg
    `work-t1-vergleich-051419-24098` läuft). ZEIT converted_Game__game (beide Durchgänge) – offen. d – 11 LECK, weld-ASan, 14 × „0 compared“
    (`work-d-belege2-vergleich-050059-20415`: Testprogramm baute nicht – d).
  · **Nächster voller Lauf:** sobald t1s Beleg da ist (work-t1 c13e917c einmischen), sonst spätestens 06:00 mit dem, was belegt ist.
- 04:40 **DIE GANZE LISTE (erste vollständige Auswertung): `work-zusammen-suite-alle-032626-8142`** – Level und Tests des gültigen Laufs 020121 (4748a503), jeder
  der 150 Tests einzeln. **Stellvertreter: 113 ok, 35 FAIL, 4 ZEIT. Engine selbst: 108 ok, 39 FAIL, 9 LECK, 3 ZEIT.** Auswertung von t1b: stand/t1.md „SUITE 020121“.
  **A. Eine Ursache für 33 Game-Tests (+ die 4 ZEIT) – BEHOBEN vom Koordinator (work-zusammen 12438d1c, Engine):** „Game runs as its C++ class (it runs:
     interpreter)“. tools/blueprint_cpp schrieb Game nicht als Klasse: „the Blueprint holds the function person_hit::hit as its library had it before“. Das Level war
     stimmig – `cpp_names` (Lader/Werkzeug) benannte den Eingang `pad` der GEHALTENEN Bibliotheks-Funktion in `pad2` um, weil Game eine Variable `pad` hat. Jetzt
     behalten gehaltene Funktionen ihre Eingangsnamen (wie der Leser es schon hält). Lokal: 173 von 173 Klassen, Klassentext übersetzt mit clang. Neuer Lauf auf
     demselben Level mit dem Fix: **`work-zusammen-suite-alle-043356-8771`**; Engine-Suite: `work-zusammen-engine-043403-8808`.
  **B. Regeln des Konverters (Nachfolger k, Zweig work-k):**
     0. SPERRT DEN NÄCHSTEN LAUF: „Game: two parts have a constant kQuiet that is not the same“ (game_projsync.cpp:25 kQuiet = 0.6, game_toss.cpp:34 kQuiet = 0.5) –
        zwei dateieigene Konstanten gleichen Namens in zwei Teilen eines Blueprints; mit den Test-Fixen von t1b (work-t1 c5b8ab99, noch nicht gemergt) werden ihre
        Nutzer erst geschrieben, dann bricht die Konvertierung ab (`work-t1-konvert-034529-8942`, convert rc=1). Allgemeine Regel nötig (Ablauf ~Z. 13850).
     1. Mat4_perspective 300/300, projection_matrix 62/300: „first difference at .m[15]: the source 1, the Blueprint 0“ (converted_engine_math_math).
     2. SoundFx_drive 40/300, SoundFx_update 169/300: `.l.voice` – ein u64 ging über Float64 (wie CityStream_buildings_near).
     3. Game: draw_events 8/18, gaps_guided 4/7, number_at 8/300, rocket_smoke_emit 2/20, spawn_squad 10/20, spawn_tank 5/20; game_details_cpp `.Result`;
        Session_update 1/56; gegen die Engine life_hud 1/33.
  **C. Test-Schreiber (Nachfolger t1, Zweig work-t1 = c5b8ab99):** 9 × „dispatch: no member named 'get_u8' in wire::Reader“ (Reader::get_u8 inline in wire.h fehlt
     im Klassentext des Tests); converted_mvt_cpp bricht ab (der Test zieht Reader{p, end, ok} unabhängig von der Span – die Quelle liest über den Puffer);
     converted_life_cpp (signal 6: std::string aus nullptr); SoundBank::request_level; look_apply_tier (LightPool-Temporär ohne make); Graph (E12).
  **D. Engine-Objekte / Durchgang gegen die Engine (Nachfolger d, Zweig work-d):** 9 LECK (Objekte aus engine_make_default: interiors, terrain, net_session,
     world_marks, game_gaps, game_town); converted_city_stream_cpp bricht gegen die Engine mit ASan in kern::weld ab; 14 Funktionen „only 0 calls could be compared“
     (aim_ray, car_input, ctl_screen_ray, gaps_init, key, my_state, parachute_input, ui_draw_gps, ui_draw_helpers, update_camera, vr_room, vr_room_forward,
     vr_scope_update, vr_update_view – in converted_Game__game_cpp / game_controls).
  **E. cost-Zeilen:** 271 (Stellvertreter) – Leistung/Speicher, Paket p nach dem ersten grünen Stand.
  · t1b ist beendet (Familien 1–7 der wartenden Funktionen gebaut: im abgebrochenen Lauf 11 statt 87 Wartende; mergen erst mit der kQuiet-Regel).
- 03:30 **Lauf 020121: `engine rc=0` (02:37), `suite rc=1` (03:19) – und die Spiel-Suite ist in beiden Durchgängen nach 25 bzw. 27 Tests abgeschnitten.**
  Ursache im Testrahmen (Kindprozess je Test, test_main.cpp): das Kind erbte die Summen der Tests vor ihm und meldete sie mit seinen eigenen zurück – die Summe
  verdoppelte sich je Test (Überlauf in int nach ~26 Tests, UBSan beendete den Durchgang, test_main.cpp:232), und ein Fehlschlag ließ jeden Test danach als FAIL
  erscheinen. In Einzelläufen (`vergleich`) fiel das nie auf. **Behoben vom Koordinator: work-zusammen d15559ba** (das Kind beginnt mit eigenen Zählern; Kleinstfall
  alt/neu). **Die ganze Liste rechnet jetzt `work-zusammen-suite-alle-032626-8142`** (Zweig work-zusammen-suite = Level und Tests des gültigen Laufs + der Fix; jeder
  converted_-Test einzeln, beide Durchgänge).
  · Aus dem abgeschnittenen Protokoll sicher: Durchgang 1 – 24 Tests ok (Firmware-Einheiten, car, city_jobs, city_stream, combat_*, common, controls, destruction,
    drone, engine_geo_*), **converted_engine_math_math FAIL**: Mat4_perspective 300/300 und projection_matrix 62/300, „first difference at .m[15]: the source 1, the
    Blueprint 0“. Durchgang 2 (gegen die Engine): die ersten 9 ok, dann **converted_city_stream_cpp bricht mit ASan ab** (danach alles Folgefehler der Zählung).
    121 cost-Zeilen von 21:15 sind auf wenige gefallen (Car_new 61/300 „1 instead of 0“, combat_audio render …) – genaue Zahl aus „alle“.
  · Vierter Neustart des Servers hinter der Sitzung (03:10): t1b automatisch fortgesetzt.
- 02:45 **GÜLTIG: `work-zusammen-voll-020121-10802` auf 4748a503 – `convert rc=0` (02:32, 18 Runden, 31 min).** Der erste gültige Lauf seit 20:53.
  „1637 of 1844 functions of the program are written as Blueprints (19218 of 30978 lines) … 156 Blueprints“; „1594 functions … compared with their source“.
  (203000: 1632 / 18813 Zeilen / 1589.) Die Suiten rechnen (Spiel-Suite mit clang, beide Durchgänge; Engine-Suite). `grundlage` für `vergleich` ist jetzt dieser Lauf.
  · **Warum es trotz t2 kaum mehr sind:** im ganzen Programm warten rund 116 Funktionen an ihrem Test, fast alle aus wenigen Familien (Protokoll:
    `xz -dc $S/mag/work-zusammen-voll-020121-10802/zwischen/convert.log.xz | grep 'cannot compare it yet'`):
    (1) ~60 × `kReliable`/`kUnreliable` – „use of undeclared identifier 'kReliable'; did you mean 'net_session_cpp_local::kReliable'?“ in den Tests der Game-Teile, die
        Session-Methoden mitnehmen (town_to_host 12, send_fx 9+11+2+2, town_to_all 9, seat_request 7, gaps_lamp_break 4, send_proj 3, send_swarm_hit 3 …);
    (2) 10 × „type name 'World' … no matching function for call to 'arg'“ (Game::fixed_step u. a.); (3) 16 × `draw_CarSpec` fehlt („did you mean 'draw_MassSpec'?“:
        Car, spawn_police_unit, steer_torque_at, suspension_tuning, ai_drive …); (4) 6 × `g_fun`/`multiplier` (combat_hud …); (5) 6 × „expected ';' after top level
        declarator“; (6) 5 × `kOlive` (CombatArt); (7) 2 × `kScorches`; dazu Graph (E12). Alles Test-Schreiber im ganzen Programm → Nachfolger t1 (gestartet).
    Dazu 16 „waits for good“ (Rekursion über Teile, Entwurfspunkt Stufe 6).
  · Dritter Neustart des Servers hinter der Sitzung (~02:28): d4 wurde abgebrochen – nicht fortgesetzt (seine drei Fixe sind gemergt; der Rest von d kommt nach dem
    ersten grünen Stand).
- 02:10 **work-zusammen = 4748a503** – dazu die drei belegten Fixe von d4 (stand/d.md 01:35/01:54): work-d-count (E14 set_pcm zählt Samples; die Zahl neben
  einem Zeiger sagt ihre Länge im Typ – keine Namensregel mehr), work-d-array (ein Feld `T m[N]` hat in Form und Antwort N Werte), work-d-maker (Objekte der Engine
  auch für Klassen, deren Konstruktor Werte und machbare Objekte nimmt: Engine, GameAudio, GameWorld, SoundBank, net::Client, net::Server). Belege:
  `work-d-count-vergleich-000943-8780`, `work-d-belege-vergleich-002858-13659` (Stellvertreter 47 von 47 ok), `work-d-belege-engine-005831-17971` rc=0,
  `work-d-maker-vergleich-013431-7648` (gegen die Engine: matrix_of, eject, state_of, Session_connected/join/stop werden verglichen, keine Abweichung). Die
  Engine-Suite für den Maker rechnet der volle Lauf mit. **Voller Lauf neu: siehe `./mag.sh stand` (work-zusammen-voll-02…)** – der von 01:59 lief 4 min und ist
  abgebrochen (ohne die Fixe von d).
  · Offen von d4 an t1: note Session_join/stop „engine.client.release(n) (the handle names no Client that was made)“; alle-real.txt nennt
    converted_net_session_cpp / game_gaps FAIL, die Einzelausgabe sagt „0 failures“ – ungeklärt (Auswertung der Einzelläufe in lauf2.sh prüfen: Koordinator).
- 02:00 **Voller Lauf 012215: der Ablauf endet jetzt (Fixpunkt nach Runde 18, 16 Funktionen „waits for good … went round“), aber `convert rc=1` am Leser:**
  „Game.cpp: line 8106, column 18: kern::to<Type>(value) or kern::formula(...) expected, not kern::math“ – `std::atoi` (game_toss.cpp:245) wurde zu
  `id = kern::math::wrap32s(int64_low(...))` ohne Formel. g: Fix 96d52bef (der Schnitt auf 32 Bit ist ein eigener Schritt), lokal mit lesen.py belegt (alle 156 Texte
  gelesen). **work-zusammen = 84696123, neuer voller Lauf: `work-zusammen-voll-015912-10300`** (01:59). **g ist beendet** (Lauf zu Ende). Es läuft nur noch d4.
  · Wurzel des Kreises (g): Rekursion über Teile – Game::explosion → blast_cars/wreck_car → explosion, explosion → combat_blast → explosion. Die Sprache verbietet
    Rekursion (der Leser lehnt sie ab, notation.cpp:686ff.; erlaubt ist Wiedereintritt über ein Event, höchstens 64 tief). 16 Funktionen festgehalten, 6 Aufrufer
    warten mit; 0 warten wegen direkter Selbstrufe. **Entwurfspunkt Stufe 6 „Rekursion“** (Vorschlag g: mit derselben Grenze 64 zulassen) – nicht entschieden.
  · Offen bei den Game-Dateien (Nachfolger g): combat_hud („g_fun is not a member of …combat_rules_cpp_local“) und dieselbe Art bei kReliable/kUnreliable
    (send_fx, town_to_all, town_to_host) und kOlive – Stücke einer Datei, die ein Test über einen dritten Teil bekommt; nur im ganzen Programm.
- 01:25 **work-zusammen = 2c7667bc** und darauf der **volle Lauf `work-zusammen-voll-012215-4354`** (abgelegt 01:22). Seit 9f10aa88 dazu: t1 8e2474a9 (Methoden
  außerhalb der Klasse, note nennt den Grund der Engine; 2dacbbb5), **p 85d60a1d** (ganzzahlige Formeln mit +,−,×, Weitergabe beim Anhängen, Aufrufweg an einen
  Actor ohne vector, Fixed an Eingängen, Switch on Text, Zeit-Test, Fix text_format; f67af15b – Belege `work-p-engine-002417-11420` rc=0,
  `work-p-vergleich-010659-18840` 14 von 14 ok in beiden Durchgängen), **Rücknahme von 12e3da24** (ks Ausweichen; e3901213), **g 7bd9ac5d** (der Ablauf endet auch,
  wenn die Runden kreisen: was dort wechselt und wartet, wartet von da an – „(it went round with …)“; am ganzen Programm noch unbelegt, dieser Lauf ist der Beleg).
  · **t1 und p sind beendet** (Läufe zu Ende, alles Gebaute gemergt). Es laufen g (Wurzel des Kreises: explosion/wreck_car ↔ look_blast) und d Nachfolger 4.
  · Offen aus ps Bericht: Zeit-Test 25 von 29 Fällen über 1,10 (mulm 7,8×, settings_escape 7×, sha256 6,7×, from_hex 5,1×, qr 5×) – zwei weitere Entwürfe zur
    Schreibweise angekündigt (+,−,× und Vergleich an der Wurzel einer Int-Formel; Struktur-Argumente bei Call on Actor ohne logic::Value). CI-Job native-spawncity
    läuft auf work wie auf work-p in seine 45-Minuten-Grenze (rot) – vor dem Abschluss zu lösen.
- 00:18 **work-zusammen enthält E13: die Spiel-Suite baut mit clang** (Merge work-t1 87e3d741; Beleg `work-t1-vergleich-000108-5748`: clang 18.1.3 linkt auf mag mit
  ASan/UBSan, beide Durchgänge 43 von 44 ok, converted_destruction(_cpp) ok OHNE Absenkung). **Alle: `git merge origin/work-zusammen` – eure Vergleiche bauen dann
  mit clang.** Stellen, an denen nur die Reihenfolge der Argumente abwich (Destruction_collapse/fracture/blast; eject, state_of), sind damit erledigt; ks Absenkung
  90ecf01f ist nie gemergt worden und entfällt. p: Vergleich `work-p-vergleich-230429-15316` (qr_code, settings, nostr, controls) in beiden Durchgängen 12 von 12 ok.
- 00:05 **d (Nachfolger 3) ist beendet.** Platz der Engine: Schritte 3–6 gebaut, Engine-Suite grün (`work-d-engine-223942-31317` rc=0), CI e2e rot allein wegen des
  alten Levels; Schritt 7 gebaut, unbelegt. **Entscheidung:** der Platz wird erst nach dem ersten grünen Stand in `work` gemergt. **Entscheidung E14 (set_pcm):**
  die Zahl neben dem Zeiger zählt die Samples (`Count<1> samples`), die Engine rechnet frames = samples / channels. Nachfolger 4 gestartet: zuerst die drei
  Stellvertreter-Befunde (Count im Typ statt Namensregel; `T m[N]` in Antwort/Form; set_pcm) – PAKET3_d.md, Nachtrag 00:05.
- 00:00 **work-zusammen = e4aa2e8d** (+ g c4d4544b: eine Schleife, deren Grenze die Engine fragt, ist keine Zählschleife – Beleg `work-g1-vergleich-232903-26675`;
  + k c32f607b: der Ablauf nennt, was im Kreis geht). **k ist beendet** (Lauf zu Ende; offen bei k: 12e3da24 zurück nach ps Fix, 90ecf01f zurück nach clang,
  Liste seiner Dateien – Nachfolger, sobald ein gültiger Lauf eine Liste gibt). **g übernimmt das Pendeln ganz** (Ursache combat_hud/g_fun + der Ablauf darf nicht pendeln).
  Der volle Lauf 232247 ist abgebrochen (seine Suite auf ungültigem Level hätte niemand genutzt); statt dessen rechnet **nur die Konvertierung mit Diagnose**:
  `konvert origin/work-zusammen 37` – sie zeigt je Einheit, was wechselt („goes round: …“).
- 23:55 **Der volle Lauf `work-zusammen-voll-232247-24031` (5a445af8): `convert rc=1` (23:50, 27 min) – der Ablauf pendelt.** Ab Runde 9 geben 30 Game-Einheiten Runde
  für Runde, was sie vorher gaben („what one writes takes from the other what it writes“), bis zur Sicherung in Runde 16. Einzige Meldung davor (Runde 10):
  `Game::combat_hud()` „was written and waits now“ – sein Test übersetzt nicht: „‘combat::Fun’ has no member named ‘multiplier’ – ‘g_fun’ is not a member of
  ‘…::combat::combat_rules_cpp_local’“ (die Familie, die t2 mit `file_home` behoben hatte; t2s Sieben-Dateien-Vergleich lief sauber – also die Zusammenführung oder
  erst das ganze Programm). **k klärt beides:** die Ursache (gezielte Probe, Eingrenzen zwischen den Merge-Ständen) und dass der Ablauf nicht pendeln kann
  (was in einer Runde wartet, wartet weiter). Kein gültiges Level aus diesem Lauf; seine Suiten rechnen zu Ende (erster Blick auf die Liste, Engine-Suite) – kein Beweis.
  `grundlage` auf mag bleibt der Lauf 203000.
- 23:35 **Entscheidung E13 – die ganze Spiel-Suite baut mit clang, dem Compiler des Spiels (ersetzt den Absenkungs-Teil von E9).** g fand die zweite Stelle
  derselben Art (game.h:170 `read_body(world_->physics(), body(a))`: GCC ruft actor_body zuerst, clang und der Blueprint physics – eject 65/65, state_of 91/91).
  Die Bedeutung der Quelle ist die, die der Compiler des Spiels ihr gibt (emscripten ist clang) – also vergleicht der Test gegen genau den, statt je Stelle zu
  senken. t1 stellt run_tests.sh um (cxx = clang nach der Suche des Konverters; fehlt clang, schlägt der Lauf fehl; Compiler im Cache-Schlüssel; CI). Erste
  Klärung: linkt clang-18 mit ASan/UBSan auf mag. Die Absenkung von destruction.cpp (k, 90ecf01f) wird nicht gemergt und zurückgenommen, sobald ein Vergleich
  unter clang collapse/fracture ohne sie gleich zeigt. Beleg des Grundsatzes: Kleinstfall von k (clang alt = neu, g++ alt ≠ neu).
  · g (23:27 gestartet): c4d4544b – eine for-Grenze, die die Engine fragt (`c < server_->max_clients()`), wird je Runde gefragt (Beleg läuft).
    **BEFUNDE FÜR t1 (von g):** Session-Kopie ruft am Ende client.release (300/300 bei Session_*); Temporär `kern::LightPool(n, 96)` ohne light_pool.make im
    Stellvertreter (look_apply_tier); Abbruch game_look: gezogener Index außerhalb eines Arrays fester Länge (`kTier[t]`).
    **BEFUNDE FÜR d (von g, t2):** matrix_of – Antwort des Stellvertreters für `f32 m[16]` hat 0–2 statt 16 Einträge; `push_incoming(from, const u8* data, int bytes)` –
    die Zahl neben dem Zeiger wird am Namen erkannt (Namensliste, greift bei `bytes` nicht): die Zahl sagt es künftig in ihrem Typ (Count<Per>); set_pcm (t2).
- 23:25 **work-zusammen = 5a445af8** (+ t2 19b96a95: rund 100 der 118 wartenden Funktionen bekommen ihren Test; + k fa3dc97d: u64-Schlüssel einer Map).
  **Der volle Lauf ist neu abgelegt: `work-zusammen-voll-232247-24031`** (der von 23:13 lief 8 min und ist abgebrochen – ohne t2 hätte er die Game-Dateien nicht gezeigt).
  · t2 ist beendet (Lauf zu Ende). Sein Vergleich `work-t2-vergleich-223555-30482` über sieben Game-Quelltexte: convert rc=0, bau rc=0, 50 ok / 7 FAIL / 2 ABBRUCH –
    Abweichungen der Blueprints, die vorher kein Test erreichte. **Neues Paket g** (PAKET3_g.md): Regeln des Konverters für die Game-Dateien; k behält die anderen.
  · **Entscheidung E11 (t2):** `-Wno-missing-field-initializers` beim Übersetzen der Tests ist keine Toleranz – ein ausgelassenes Feld einer Aggregat-Initialisierung
    ist in C++ festgelegt (Wert-Initialisierung), in Quelle und Test dasselbe; die Warnung sagt nichts über Werte.
  · **Entscheidung E12 (t2, Graph):** ein `logic::Graph` in einem Ergebnis (vehicle_actor → Actor mit optional<Graph>; audio_init/Level) wird im Test über seine
    Serialisierung verglichen (der Text ist der Wert). Bis das gebaut ist, warten die zwei Funktionen ehrlich mit ihrem Grund; Reihenfolge: nach der grünen Suite (t1).
  · Reste von t2 → t1: lamps `Field::Spot` (3 Funktionen, „cannot convert vector<Spot>“), Beleg für sscanf/scan_c auf mag (game_toss), Graph (E12).
  · **BEFUND FÜR d (von t2):** gaps_guided_sounds – `set_pcm` bekommt im Blueprint die ganze Liste statt einer Zahl.
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
