# Übergabe: SpawnCity auf Kern – vollständige Konvertierung in Blueprints

Stand: Mittwoch, 18.11.2026, 17:25 Uhr (Europe/Berlin). Geschrieben vom Koordinator der Sitzung
`session_01K2YidUPFTJShcS1fG6cZzJ` für den Koordinator der nächsten Sitzung. Diese Datei und der Ordner,
in dem sie liegt (Branch `koordination` im Repo), sind **alles**, was die neue Sitzung von der alten hat –
der Arbeitsordner der alten Sitzung (Scratchpad, lokale Worktrees, laufende Agenten) ist weg.

**Lies diese Datei ganz. Dann die Dateien, die Abschnitt 13 nennt. Erst dann handle.**
**Abschnitt 16 (am Ende) schreibt den Stand fort – was dort steht, gilt vor den Abschnitten 5–10 und 15, wo es abweicht.**
Antworte Chris auf Deutsch. Keine Aufwandsschätzungen in Entwickler-Tagen. Seine E-Mail-Adresse ist privat.

---

## 1. Das Ziel (unverändert, nicht verhandelbar)

Repo `ChristianHohlfeld/kernengine.de`. SpawnCity auf der Engine „Kern“ (`kern/games/spawncity`) ist der
vollständige, exakte Drop-in-Ersatz für spawncity.de (das JavaScript-Original).

**Das ganze Spiel wird Blueprints**, erzeugt von `tools/cpp_to_blueprint.py` aus C++, das dem erlaubten
Teilsatz genügt. Blueprint und C++-Teilsatz sind dasselbe in zwei Schreibweisen, in beide Richtungen, in
Echtzeit im Editor (Kern Studio), nativ exportierbar als C++ mit voller Leistung. Am Ende ist das Projekt
ein Level + Blueprints + Daten; `kern/games/spawncity/src/` ist leer und wird gelöscht.

Chris, wörtlich (bindend):
- „die idee war c++ subset interchangeable realtime als blueprint und native alles exportierbar in c++,
  full performance. dazu wollten wir die subset constraints der c++ codes mit script umwandelbar machen
  damit wir generisch blueprints aus constraint conformem cpp haben.“
- „blueprint == c++ , auf constraintbasis für beide: nur die erlaubten constraints aus c++ sind gestattet“
- „warum, wir wollten doch generieren, nicht händisch alles umschreiben oder?!?“ – Blueprints werden
  **generiert**, nie von Hand gebaut oder geändert.
- „kein caching.“ · „Konsequent nicht hacky.“ · „No excuses, keine Sonderregelung“ · „All the way.“
- „Sei streng im Ansatz, sauber und der Umsetzung, schnell im finalisieren.“
- „immer zwischenschritte regelmäßig pushen. wichtig.“
- „Melden bringt mir nichts. Exaktheit und voll durchlaufen schon.“
- „Konsistenz bei rechnen ist wichtiger als perfekte Abbildungen aller Operationen.“
- Vergleich **Bit für Bit, ohne Toleranz**. Kein Schummeln in Tests. Kein Bridge von „Funktionsaufrufen“
  aus einem Blueprint in C++ des Programms.
- 15.11.: „Vollständige Konvertierung und Prüfung. Vollständiger Umfang. Nicht ausweichen. Gesamtziel voll
  erreichen.“ – „Wenn du dafür Blueprint, Engine oder Subset erweitern musst, tu es.“ – „Ziel ist immer
  noch die vollständige Echtzeit-Interoperabilität zwischen Blueprint und C++ Subset.“
- 18.11.: „All. WE need all.“ · „Do 100“ · „vollständig, ungekürzt und ohne Shortcuts …, sauber und final“
  · „Wir dürfen doch keinen Fehler machen. 100% clean. Nichts skippen.“

**Fertig heißt** (vorher wird nicht „fertig“ gemeldet):
1. Jede Funktion des Programms ist als Blueprint geschrieben (zuletzt gezählt: 1.876 Funktionen, 31.096 Zeilen).
2. Jede ist im Test Bit für Bit mit ihrem Quelltext verglichen – Interpreter und Klasse, gegen den
   Stellvertreter der Engine **und** gegen die echte Engine (`SC_ENGINE=real`). Nichts „wartet“.
3. Jede Umschreibung der Spiel-Quelle seit `main` hat einen grünen Differenztest (alt gegen neu, Bit für
   Bit, nie öfter Speicher).
4. Die exportierte Klasse jedes Blueprints ist so schnell wie das C++, aus dem er kommt (Zeit ist Teil des
   Beweises).
5. Das Spiel läuft aus den Blueprints (Stufe 5: jede Datei umgeschaltet, ihr C++ gelöscht), `src/` leer.
6. Alle CI-Jobs grün (native, native-spawncity, build, e2e, input, determinism) – auf `work`, dann auf `main`.
7. Squash nach `main`, CI grün, Deploy, Changelog-Eintrag, `BLUEPRINTS.md` aktuell.

---

## 2. Feste Regeln

- **Box3D nie tunen.** Physik nach Erin Cattos Doku (`kern/docs/physics.md`).
- **Nicht ändern:** die Firmware (`kern/games/spawncity/firmware` – mit dem echten Fluggerät geteilt; der
  Konverter nimmt sie, wie sie ist); die 22 Web-Dateien mit „Unchanged from the original SpawnCity“
  (Ausnahme: `web/view/changelog_menu.mjs` bekommt Changelog-Einträge); „Gewalt und Passanten“ bleiben, wie
  sie sind; TON/MUSIK/RESET-Knöpfe bleiben; „ente muss neutral bleiben“. Runden/XP/Teilen/öffentliche
  Freundes-Lobby/Missionen sind zurückgenommen – **nicht wieder einbauen**.
- **Nichts von SpawnCity wandert in die Engine.** Engine, Sprache, Werkzeuge bekommen nur, was allgemein ist
  und jedem Spiel dient.
- **Kein Zwischenspeicher** im Konverter zwischen Läufen (und keine Caches im Spiel als Ausweg).
- Blockierte Hosts/Rechte nicht umgehen. GitHub nur über `gh` (GraphQL ist gesperrt: REST über `gh api`).
- **Git:** Agenten pushen nur auf eigene Zweige (`work-<paket>`). Nur der Koordinator pusht auf `work`.
  Niemand pusht auf `main`, außer beim Abschluss (Abschnitt 12). Kein Force-Push, nie.
  Commit-Nachrichten deutsch; am Ende die Attributionszeilen, die die jeweilige Sitzung vorgibt.
- **Deploy nur bei grüner CI.** Achtung: in `.github/workflows/ci.yml` hängt `deploy` nur an `build` und
  läuft nur auf `main` – also vor jedem Push auf `main` selbst sicherstellen, dass native,
  native-spawncity, e2e, input und determinism auf genau diesem Stand grün sind.
- **Changelog** je Level-/Engine-Update, das live geht: in
  `kern/games/spawncity/web/view/changelog_menu.mjs` direkt nach `export const CHANGELOG=Object.freeze([`
  einfügen, Format `  {date:"2026-11-18",time:"HH:MM",title:"…",items:[\n    "…",\n  ]},` – Datum und
  Berliner Zeit aus dem Zeit-Werkzeug der Sitzung, nicht geraten.
- **mag** (Chris' Server, 2× Xeon E5-2680 v4 = 56 Threads, 387 GB RAM): SpawnCity hat Vorrang, RAM ist egal
  (Chris, 18.11.). Keine systemweiten Installationen, MAG-Stack nicht anfassen, nur eigene Zweige pushen.

---

## 3. Das Konzept (der Plan steht in `kern/games/spawncity/BLUEPRINTS.md` – ganz lesen)

- Die Sprache der Blueprints ist der erlaubte Teil von C++. Jeder Baustein hat einen Knoten, bedeutet in
  jeder Stufe dasselbe (Interpreter, JIT, die eigene C++-Klasse = nativer Export) und kostet in der Klasse,
  was das C++ kostet, das er darstellt.
- Für jede Stelle des Spiels, die der Konverter nicht schreiben kann, gibt es **genau zwei Antworten**:
  (a) die Sprache bekommt den Baustein – allgemein, in jeder Stufe, mit Knoten, Schreibweise, Regeln
  (`check`), Doku, Engine-Tests, nativem Fall, Editor; oder (b) die Stelle wird in erlaubtes C++ **gleicher
  Kosten und bitgenau gleichen Verhaltens** umgeschrieben („Absenkung“) – **mit Differenztest als Beweis**.
  Eine Form, die Gestalt oder Kosten ändert, ist keine von beiden.
- **Die Abbildung:** Klasse mit Feldern → Struktur; Methode → Funktion mit Referenz auf ihr Objekt;
  **`Game` (ein Objekt, ~40 Dateien `game_*.cpp`) → EIN Blueprint**: Felder = Variablen, Methoden =
  Funktionen (Teile `Game/<datei>`); Quelldatei/Header → je ein Blueprint, ohne Zustand eine Bibliothek;
  Zeiger als Parameter → Referenz; Zeiger auf Engine-Objekt → Handle; `std::vector` → Liste; Map/Set;
  `u64`/`i64` als Bitmuster → `Int64`; Funktion der Engine → Knoten der Engine (815+ Knoten, erzeugt von
  `tools/engine_api.py` – nach Änderungen an Engine-Headern neu laufen lassen).
- **Zahlen: die Sprache rechnet in 64 Bit. Kein Float32.** Das Maß ist spawncity.de (JavaScript, 64 Bit),
  nicht der C++-Port (f32). Der Vergleichstest bindet den Blueprint an den Quelltext *in den Zahlen der
  Sprache* (der Test übersetzt den Quelltext mit `f32 = double`); an der Grenze zu noch nicht
  umgeschaltetem C++ gehen einzelne Zahlen hinüber (`rules::decimal` hinein, `static_cast<f32>` heraus).
  Beim Umschalten rechnet eine Datei, was das Original rechnet. (Am 15.11. wurde ein Float32-Typ erwogen –
  **verworfen**, steht so in der Kursprüfung.)
- **Stufen:** 0–3 erledigt; 4 = der Konverter schreibt alles (läuft); 5 = Umschalten (Mechanismus gebaut,
  erste Datei `qr_code` auf Zweig, unbewiesen); 6 = das Projekt lädt im Editor, Export ist das Drop-in.
- **Es gibt zu jeder Zeit genau eine Quelle:** bis eine Datei umschaltet, ihr C++ (der Blueprint daneben ist
  erzeugt und durch den Vergleichstest gebunden); danach der Blueprint im Level.

### Kursprüfung 18.11.2026 (steht in BLUEPRINTS.md, bindend)
1. Zahlen bleiben 64 Bit. 2. Eine Umschreibung gilt nur mit Differenztest (`tools/source_diff.py`,
`tests/lowered/`), auch rückwirkend für alles seit `main`. 3. **Eine Datei, ein Auftrag** – Pakete sind nach
Dateien des Spiels geschnitten; der Konverter bekommt nur Allgemeines. (Anlass: in der Runde vom 15.11.
haben mehrere Agenten dieselben Stellen doppelt und verschieden umgeschrieben, und ein „Richtungswechsel“
wurde verkündet, der keiner war – der Plan erlaubte (a)/(b) seit dem 13.11.)

---

## 4. Entscheidungen dieser Sitzung (18.11.) – alle bindend, bis Chris anders entscheidet

1. **Kein Float32** (s. o.).
2. **Differenztest in der Form „zwei Bauten“:** jede Seite ist das ganze Programm in ihrem Stand – alter
   Baum aus `--base`, neuer Baum von heute, derselbe Treiber gegen beide übersetzt, verglichen werden
   Byteströme (Rückgabe, Parameter, Objekt – auch `Game` –, Zustand der Datei, Zahl der
   Speicheranforderungen). Grund: `Game` liegt in game.h und ~40 Dateien, gleichnamige anonyme Helfer
   stoßen zusammen, alter Text übersetzt nicht gegen neue Header. Besitzer des Werkzeugs: die Claude CLI
   auf mag (Zweig `mag/differenztest`). Bis es steht, werden Absenkungen in Game-Dateien nur vorbereitet
   (eigene Commits „Absenkung <datei>: …“, `--base` notiert), **nicht gemergt**.
3. **Stufe 5, die Form des Umschaltens:** Das Spiel nimmt die Typen der Klasse – **ein Typ je Struktur**,
   keine Umwandlung von Strukturen/Listen je Aufruf; erzeugter Header mit den Klassen unter festen Namen
   (Namensraum `blueprints`, aus demselben Generator wie `level_blueprints.cpp`), ein einmal gebundenes
   Objekt je Klasse (`bind_blueprint<T>`), direkter typisierter Aufruf, kein Aufruf beim Namen.
   Umgeschaltete Blueprints liegen im Level-Ordner „Program“; der Konverter verwirft/schreibt nur „Source“.
   Das Spiel setzt `Runtime::budget` wie der Test (`rules::kLoopBudget`). Der Konverter erzeugt den Header
   selbst, wenn das Level einen Ordner „Program“ hat (keine neue Pflicht-Option in Skripten).
4. **Leistungsregel:** Eine umgeschaltete Datei wird erst gemergt, wenn ihre Klasse so schnell ist wie ihr
   C++. (Befund: `qr_code` als Klasse rund 17× langsamer – Paket p.) Zeit wird Teil des Beweises (Test).
5. **Platz der Engine:** Eine Funktion der Engine, die eine nicht-const Referenz gibt (z. B.
   `GameWorld::shots()`), ist ein Platz, keine Kopie. Der Platz ist der **Ausgang des Engine-Knotens**
   („bound“, einmal gebunden wie `auto& r = f();`); die bestehenden Platz-/Listen-Knoten arbeiten darauf.
   Auflagen: nur für Funktionen, die der Generator am Baum als Nachschlagen ohne Wirkung erkennt
   (`return <Feld>;`); gebunden nur innerhalb des Funktionslaufs (nicht über Warten/Events; `check()` macht
   das zum Fehler, wo es feststeht); endet das Objekt hinter dem Handle, gilt „ungebunden“ (kein toter
   Zeiger); der Stellvertreter im Test hält die Liste als Zustand; der Editor bietet die Knoten an.
   Das Spiel senkt diese Stellen **nicht** ab.
6. **Rückrufe der Engine in das Programm** (HeightSampler, `std::function`-Felder): allgemeine Fähigkeit
   „die Engine ruft eine Funktion eines Blueprints“ – erst Entwurf, Bau nach Bestätigung (Paket c).
   `begin_roads, relay, update, finish_features, fail, go_offline` (city_stream) und `Terrain::sampler`
   bleiben bis dahin unverändert.
7. **Konverter-Ablauf:** erst eindeutig (dasselbe Ergebnis bei jeder Kernzahl), dann parallel (Paket k).
   Befund: heute hängt das Ergebnis vom zeitlichen Ablauf ab (in f9 fiel `Game/game_look_cpp` von 7/13 auf
   0/13). Ein Rückfall „war geschrieben, wartet jetzt“ ist ein Fehler am Inhalt und wird gemeldet.
8. **`Lists Equal` für Listen von Strukturen** allgemein (Paket f2), soweit C++ dort `==` festlegt.
9. **settings** in der Form des Piloten (`SettingsTable` als Zustand, den das Spiel hält) – die bewiesene;
   `toss_` bleibt ein Wert; `update_scores` nimmt die Beschriftung als Sicht (`std::string_view`).
10. **Kontingent:** Agenten laufen als **Opus 5.5, mittlere Stufe**; der Koordinator überwacht. Agenten
    schreiben ihren Stand in Dateien und melden nur Entscheidungsbedarf, „bereit zum Mergen“ mit Beweis,
    Befunde für andere. Die Strenge der Beweise wird nicht gesenkt – sie rechnet mag.

---

## 5. Stand der Zahlen

| | 14.11. (f8, im Repo) | 18.11. (f9, abgebrochen) |
|---|---|---|
| Funktionen geschrieben | 1.368 / 1.817 (75 %) | 1.577 / 1.876 (84 %) |
| Zeilen geschrieben | 11.996 / 30.741 (39 %) | 16.136 / 31.096 (52 %) |
| Bit für Bit verglichen | 1.324 | noch nicht gemessen |

- f9 lief lokal auf `5dcd9eb`, schrieb alle Funktionslisten, brach aber beim Einlesen ins Level ab. Drei
  Fehler dahinter sind behoben und auf `work`: `955503b` (ganze Zahl im Vergleich zweier Textplätze),
  `52a9275` (Eintrag einer Liste in einer Liste, dem eine Liste zugewiesen wird), `3b355c4` (Leser: lokale
  Map/Set). **Seit dem 14.11. gibt es keinen vollständigen, gültigen Lauf.** Level und `tests/converted` im
  Repo sind von f8 und passen nicht zu den Quellen auf `work` – darum ist native-spawncity rot.
- f9: 358 Funktionen warten, 250 Grundursachen; ~100 davon im Test-Schreiber („its test cannot compare it
  yet“). Listen: `f9_roots.txt`, `f9_waiting.txt`, `f9_convert.log.xz` (in diesem Ordner).
- Gesamtfortschritt, geschätzt (keine Messung): ~65 %.

---

## 6. Zweige (alles gepusht; nichts liegt nur lokal)

- `main` @ `fb6cf4c` (14.11., 10:32) – live. Seitdem nichts.
- `work` @ `90b5ab8` – der Integrationszweig. Enthält: alle fünf Zweige vom 15.11. (gamestate, text,
  places2, values, pilot), `work-engine3` (Verteiler auf die echte Engine, `SC_ENGINE=real`), `work-d`
  (direkte Engine-Aufrufe in der Klasse, `engine_direct.h`), die Kursprüfung in BLUEPRINTS.md, die drei
  Korrekturen oben, „Insert Text Bytes“ nur für Listen ganzer Zahlen.
- CI auf `work`: Lauf `44335902766` auf `3b355c4`: determinism, native, build, input, e2e **grün**,
  native-spawncity **rot** (erwartet, s. o.). Lauf `44341582015` auf `90b5ab8` lief bei Übergabe noch –
  prüfen: `gh run view 44341582015 --json jobs --jq '.jobs[]|"\(.name)=\(.conclusion)"'`.
  CI starten: `gh workflow run ci.yml --ref <zweig>` (ein Dispatch wird von späteren Pushes nicht
  abgebrochen; die PR-Läufe von PR #4 `work→main`, Entwurf, schon).
- Paket-Zweige, **noch nicht in `work`** (Stand bei Übergabe; der genaue letzte Commit steht je Paket in
  `uebergabe/<paket>.md` bzw. `git log origin/work..origin/work-<paket>`):
  `work-g1`, `work-g2`, `work-f1`, `work-f2`, `work-t1`, `work-t2`, `work-k`, `work-p`, `work-s5`,
  `work-d` (Folgeauftrag „Platz der Engine“, falls schon etwas gepusht ist), `work-c` (falls angelegt),
  `work-r` (Werkzeug-Stand für den Differenztest; weitergeführt auf `mag/differenztest`),
  `work-k-zeit` (nur Zeitmessung, **nicht mergen**).
- mag: `mag/auftrag` (Skript + Warteschlange), `mag/ergebnis-<name>` (ein Zweig je Auftrag),
  `mag/differenztest` (CLI auf mag), `mag/schlusslauf-1` (**wertlos**, Lauf ohne Compiler – nicht
  verwenden), `mag/ergebnis` (alter Prüfer vom 15.11.).
- **Falle:** der Klon holt per Vorgabe nur bestimmte Zweige. Einmal ausführen:
  `git config --add remote.origin.fetch '+refs/heads/mag/*:refs/remotes/origin/mag/*'` und
  `git config --add remote.origin.fetch '+refs/heads/work-*:refs/remotes/origin/work-*'` und
  `git config --add remote.origin.fetch '+refs/heads/koordination:refs/remotes/origin/koordination'`
  (oder `git ls-remote origin 'refs/heads/mag/*'` benutzen). Ohne das sieht man die mag-Zweige nicht – so
  ist am 18.11. eine Stunde lang ein blinder Wächter gelaufen.

---

## 7. mag: die Rechenmaschine (läuft unabhängig von jeder Sitzung)

- Auf mag läuft `~/spawncity-auftrag/lauf2.sh` (Branch `mag/auftrag`, Datei `lauf2.sh` – Kopf lesen). Die
  Schleife holt alle 20 s den Branch; jede Datei `jobs/<name>` ist ein Auftrag (erste Zeile):
  - `voll <ref>` – Konvertierung des ganzen Spiels + Spiel-Suite + Engine-Suite; Ergebnis-Zweig: Commit 1 =
    Level und `tests/converted`, Commit 2 = Protokolle (`mag-log/`).
  - `konvert <ref> <kerne>` – nur die Konvertierung, mit genau so vielen Kernen (für den Beweis von Paket k).
  - `probe <ref> <quelle…>` – Konverter über diese Quelltexte (Pfade relativ zu `kern/games/spawncity`,
    bei Game-Teilen `src/game.cpp` dazu; mit `TEST_ERRORS=1`, `converted.h` neben dem Testordner).
  - `engine <ref> [filter]` – Engine-Suite (`kern/tests/run.sh`, samt nativer Stufe).
  - `spiel <ref> [filter]` – Spiel-Suite (`run_tests.sh`, auch `tests/lowered`), ohne neue Konvertierung.
  Ergebnis: Zweig `mag/ergebnis-<name>`, schon beim Start mit `mag-log/when` (so sieht man, dass gerechnet
  wird); am Ende `when` mit `… rc=N`, `*.tail.txt`, `*.log.xz`, bei Proben `texts/`.
- Stand der Schleife: rechnet im Arbeitsspeicher (`/dev/shm`), hält einen Spiegel des Repos, bis zu 14
  Aufträge zugleich, ersetzt sich selbst bei einer neuen Fassung, ohne etwas anzuhalten; `clang++` heißt
  dort `clang++-18` (das Skript sucht ihn). Die Fassung mit 14 Plätzen/Spiegel (`fe843e4`) greift, sobald
  die Aufträge fertig sind, die bei ihrem Push liefen – prüfen, ob sie greift (Starts mehrerer Aufträge im
  Sekundenabstand; `when` nennt die Werkzeuge). Greift sie nicht oder steht die Schleife: Chris bitten, in
  der Shell von mag auszuführen:
  `pkill -f 'spawncity-auftrag/lauf' ; cd ~/spawncity-auftrag && git pull -q && nohup bash ~/spawncity-auftrag/lauf2.sh > ~/spawncity-auftrag4.log 2>&1 &`
- **Einen Auftrag ablegen:** `mag.sh` in diesem Ordner (Pfad `S` oben in der Datei an das neue Scratchpad
  anpassen; es braucht einen Worktree des Branches `mag/auftrag` unter `$S/magjob` und den Klon unter
  `/home/claude/kw` – siehe Abschnitt 11). Der Stand muss gepusht sein – mag rechnet, was auf GitHub steht.
- Bei Übergabe eingereiht bzw. laufend: `work-voll-162644-5528` = **voller Lauf auf `origin/work`** (zum
  Zeitpunkt seines Starts; enthält die drei Korrekturen, wenn er nach 16:48 anlief) – **dieses Ergebnis
  zuerst ansehen** (Abschnitt 10, Schritt 1). Dazu viele Proben der Pakete; `work-voll-153849-12713` ist
  ein abgebrochener Lauf (nicht verwenden).
- Dauer: eine Probe über drei Dateien ~4 Minuten; die Konvertierung des ganzen Spiels ~1 Stunde (die
  Game-Teile laufen nacheinander – das behebt Paket k); Spiel-Suite ~5 Minuten, Engine-Suite ~27 Minuten.
- **Claude CLI auf mag** (Opus 5.5, automatischer Modus, Ordner `~/spawncity-diff`): hat den Auftrag
  `pakete/CLI_AUF_MAG.md` auf `mag/auftrag` (rückwirkender Differenztest + Werkzeug in der Form „zwei
  Bauten“); schreibt `BERICHT.md` und `ABWEICHUNGEN.md` auf `mag/differenztest`. Sie ist von hier nicht
  direkt erreichbar: Nachrichten gehen über Chris (eine Zeile, die er dort eintippt) oder über die Datei im
  Branch. Ihr Login läuft am 20.11. ab (`/login`). Sie zählt vermutlich auf dasselbe Kontingent.

Lokal (Cloud-Sitzung): 2 Kerne, 8 GB – nur Syntax je Datei, kleine Proben (≤ 3 Quelltexte, `KERN_JOBS=1`),
keine Suiten. `final.sh` (in diesem Ordner) ist der lokale volle Lauf als Notweg (~2 Stunden).

---

## 8. Die Pakete (eine Datei, ein Auftrag)

Aufträge: `PAKET_COMMON3.md` (gemeinsam, mit allen Nachträgen – **jeder Agent liest es ganz**) und
`PAKET3_<paket>.md`. Übergaben der Agenten: `uebergabe/<paket>.md` – **vor jedem Neustart eines Pakets
lesen**; kurze Stände: `stand/<paket>.md`. In den Aufträgen steht der alte Scratchpad-Pfad als `S` – beim
Neustart auf den neuen Ordner umschreiben (Abschnitt 11).

| Paket | Besitz | Stand bei Übergabe (Näheres: `uebergabe/<paket>.md`) |
|---|---|---|
| **g1** | `game.cpp, game_controls, game_look, game_ui, game_details, game_vr, game_guided, game_gaps, game_barricades, game_lamps` | allgemeine Konverter-Regeln gepusht; Absenkungen nur vorbereiten (Entscheidung 2); echte Gründe zeigt nur eine Probe über **alle** `src/game*.cpp` |
| **g2** | `game_life, game_military, game_town, game_townsync, game_events, game_foot_rounds, game_weapons, game_world, game_worldsync, game_swarm, game_net, game_traffic, game_traffic_far, game_vehicle_damage, game_destruction_sync, game_projsync, game_toss` | Konverter-Regeln + drei Absenkungen (notiert in `g2/absenkungen.txt`); die Engine-Listen (`shots()` …) warten auf Paket d |
| **f1** | `city_stream, city_jobs, tiles, mvt, interiors, facades, terrain, water_look`, Engine-Header `engine_geo_*`/`engine_logic_logic` (nur Allgemeines) | Absenkungen des Vorgängers; deren Differenztests übersetzten nicht, neu erzeugt – **grün noch nicht bewiesen**; Rückrufe warten auf Paket c |
| **f2** | `life, people, touch_play, world_traffic, world_marks, net_session, nostr, car, lamps, destruction, rules.cpp`, kleine Header | `==` zweier Listen (Sprache), Namen in Methoden wie C++, `update_pigeons` abgesenkt mit grünem Differenztest; lokal: car 15/15, nostr 41/41, net_session 33/33; offen: `Lists Equal` für Strukturen |
| **t1** | Test-Schreiber: „no values of the variables of Game“, „written by a macro … at nowhere“, „C++ leaves open …“ | drei Gründe an der Wurzel behoben, doppelte Engine-Stellvertreter in Teil-Läufen behoben; Nachweis im Vergleich braucht den vollen Lauf |
| **t2** | Test-Schreiber: „its source does not compile with a Blueprint's numbers yet“ (alle Arten), „a method of a class that is not written“ | braucht eine Probe über alle Quelltexte (auf mag eingereiht) |
| **d** | `tools/engine_api.py`, Engine-Knoten | Teil 1 fertig, bewiesen, **in `work`**. Folgeauftrag „Platz der Engine“: Entwurf bestätigt (Entscheidung 5), Bau begonnen |
| **k** | Ablauf des Konverters (`main`, `convert_unit`, `job_of`) | Entwurf gepusht (Runden mit eingefrorenem Stand, Fixpunkt); Beweis offen: `konvert` mit 8 und 40 Kernen byteweise gleich, keine Funktion verloren |
| **p** | Leistung der exportierten Klasse (`tools/blueprint_cpp.cpp`, `notation.cpp`, `nodes.h` …) | gerade begonnen; soll zuerst die Ursachenliste liefern |
| **s5** | Stufe 5, Mechanismus des Umschaltens | Mechanismus gebaut, `qr_code` umgeschaltet auf `work-s5` – **unbewiesen** (Suiten auf mag) und 17× langsamer; **angehalten** bis p die Ursachen kennt; offen: Konverter liest Typen/Aufrufe einer Blueprint-Klasse aus dem Level |
| **c** | Rückrufe der Engine | nur Entwurf angefordert; **angehalten** |
| **r** | rückwirkender Differenztest, `tools/source_diff.py` | bei der CLI auf mag (`mag/differenztest`); 117 Dateien / 76 Einheiten; bei Übergabe kein einziger Test grün |

Konflikte, die beim Zusammenführen kommen: „break mitten im switch-Fall“ steht doppelt (g2 `54f8e9d`, g1
`81d7455`) – **eine** Fassung behalten, die mit dem saubereren Beweis; `tools/source_diff.py` und
`tests/source_diff.h`: die Fassung von `mag/differenztest` gewinnt (sonst work-r/work-f2 vor work-f1).

---

## 9. Offene Befunde und Risiken (ehrlich)

1. **Leistung der Klasse (17×)** – das größte offene Risiko. Bisher zählten die Tests nur
   Speicheranforderungen, nicht Zeit. Ohne Behebung darf nicht umgeschaltet werden. Umfang unbekannt, bis
   die Ursachenliste da ist.
2. **Kein gültiger voller Lauf seit dem 14.11.** – jede Zahl seit f9 ist aus Proben. Erst der nächste volle
   Lauf zeigt, was die fünf Zweige vom 15.11. und die Korrekturen wirklich bringen, ob V's Korrektur
   (Zustand von Game im Test; leerer `unique_ptr`/`optional` gibt den Aufruf auf) hält, und ob der
   Vergleich gegen die echte Engine (`SC_ENGINE=real`, in CI nie gelaufen) Abweichungen findet.
   Bekannt: ein Aufruf, der auf beiden Seiten fehlschlägt, gibt beiderseits Nullen und gilt als gleich –
   prüfen, ob das zu viel durchwinkt.
3. **Der Konverter ist ablaufabhängig** (Paket k) – bis das behoben ist, können zwei Läufe desselben Stands
   Verschiedenes schreiben. Zahlen aus Läufen mit verschiedener Kernzahl nicht blind vergleichen.
4. **Rückwirkender Differenztest**: groß (76 Einheiten), Werkzeug wird gerade umgebaut. Bewusste
   Verhaltensänderungen seit `main` (entfernte Caches in terrain u. a.) listet die CLI – Chris entscheidet
   nicht-triviale Fälle.
5. **64 Bit beim Umschalten:** bestehende Tests einer umgeschalteten Datei können andere Werte sehen als im
   f32-Port. Nie einen Test weich machen – Stelle mit beiden Werten dem Koordinator, bei echter
   Entscheidung Chris. e2e/Determinismus müssen grün bleiben; das Maß ist spawncity.de.
6. Weiter offen aus dem 15.11.: Destruktoren mit Rumpf (`~Ragdolls() { clear(); }`), JSON als Typ der
   Sprache (`actor_graph`, `build_level`), `clear()` einer `std::vector` von Endendem (Reihenfolge lässt
   C++ offen), `std::initializer_list` als Argument, `app`-Knoten nicht in den Spieltests gebunden,
   `Runtime::instances` (std::function-Feld), Dateien mit nur deklarierten Engine-Typen
   (`kern::Renderer`). Jeder davon braucht Antwort (a) oder (b) – „wartet“ ist kein Endzustand.
7. **Kosten (Speicher):** Die Klasse darf nie öfter Speicher anfordern als der Quelltext; der Test zählt, und
   `cost:`-Zeilen in der Ausgabe der Spiel-Suite sind Fehler, sobald die Liste leer sein soll. Der Prüfer auf mag
   fand am 15.11. auf `266887f` noch 101 `cost:`-Zeilen (Gruppen A–F: Zweig `mag/ergebnis`, Ordner `266887f…`,
   `kosten_ursachen.md`). Seitdem nicht neu gezählt – im nächsten vollen Lauf `grep -c 'cost:' suite.log` ansehen;
   jede Zeile ist ein offener Punkt für „fertig“ (Abschnitt 1, Punkt 4 gilt für Speicher **und** Zeit).
8. **Kontingent:** am 18.11. um 17:08 waren 31 % des Wochenlimits verbraucht (Reset war 13:00). Am 15.11.
   hat das Limit alle Agenten drei Tage gestoppt. Chris nach einer Stunde nach der Prozentzahl fragen und
   die Zahl der Agenten danach richten (bei ~8 %/h auf vier zurück: t1, t2, k, p).

---

## 10. Reihenfolge bis 100 % (so weitermachen)

**Schritt 1 – Bestandsaufnahme (vor allem anderen):**
`git fetch`; CI-Lauf `44341582015` ansehen; den vollen Lauf auf mag ansehen
(`git ls-remote origin 'refs/heads/mag/ergebnis-work-voll-*'`, `git show <zweig>:mag-log/when`). Ist
`convert rc=0` und nennt `convert.log` Funktionen: Summenzeilen lesen (neue Zahlen!), `suite`/`engine` rc
und Tails lesen. Ist die Konvertierung gültig: Commit 1 des Ergebnis-Zweigs (Level + `tests/converted`) per
`git cherry-pick` nach `work` – **nur wenn der Lauf auf dem Stand lief, der heute `work` ist** (sonst neuen
`voll origin/work` einreihen) –, pushen, CI starten. Ziel: native-spawncity grün oder eine Liste echter
Abweichungen (jede ist ein Fehler, der behoben wird – nie ausnehmen).

**Schritt 2 – Agenten neu starten** (Opus 5.5, mittlere Stufe; nach Kontingent, in dieser Rangfolge):
t1, t2, k, p (kritischer Pfad) → g1, g2, f1, f2 → d (Platz der Engine) → c (nach bestätigtem Entwurf) → s5
(wenn p die Ursachen kennt). Jeder als „Nachfolger“ auf seinem Zweig: liest `PAKET_COMMON3.md`,
`PAKET3_<paket>.md`, `uebergabe/<paket>.md`, `git log origin/work..origin/work-<paket>`.

**Schritt 3 – Zusammenführen, Runde für Runde.** Ein Zweig kommt nach `work`, wenn: (1) der Agent „bereit“
meldet **mit Beweis** (mag-Lauf oder CI, Zahlen); (2) er nur seine Dateien berührt hat
(`git diff --stat origin/work...origin/work-<paket>`); (3) jede Quelländerung ihren **grünen** Differenztest
hat; (4) neue Knoten in Engine-Test, nativem Fall, „Fall mit jedem Knoten“, Palette und Doku stehen;
(5) `python3 tools/engine_api.py` keine Abweichung zu den eingecheckten `.gen`-Dateien ergibt;
(6) alle Spiel-Quellen übersetzen (Syntax). Danach: `voll origin/work` auf mag, Ergebnis nach `work`, CI.
Wiederholen, bis `convert.log` keine wartende Funktion mehr nennt und jede verglichen ist.

**Schritt 4 – Leistung:** Ursachenliste von p prüfen, Behebung freigeben, Zeit-Test in die Suiten.

**Schritt 5 – Sprach-/Engine-Erweiterungen fertig:** Platz der Engine (d), Rückrufe (c), dazu die Punkte
aus 9.6. Jede nach bestätigtem Entwurf.

**Schritt 6 – Rückwirkender Differenztest vollständig grün** (CLI auf mag); Abweichungen beheben oder Chris
vorlegen. Die vorbereiteten Absenkungen der Game-Dateien damit beweisen und mergen.

**Schritt 7 – Umschalten (Stufe 5):** Datei für Datei, Blätter zuerst; je Datei: C++ gelöscht, ihre Tests
prüfen den Blueprint, Klasse so schnell wie das C++, Suiten grün. Am Ende `Game`. Dann Stufe 6 (offene
Kästchen in BLUEPRINTS.md), `src/` leer.

**Schritt 8 – Abschluss:** Abschnitt 12.

Nach jedem Schritt: `BLUEPRINTS.md` (Stand-Absatz) nachführen, pushen.

---

## 11. Die neue Sitzung aufsetzen (konkret)

1. Das Repo `ChristianHohlfeld/kernengine.de` muss der Sitzung angehängt sein (Schreibzugriff). Klon nach
   `/home/claude/kw` (so nennen ihn die Aufträge), Zweig `work`. Fetch-Regeln aus Abschnitt 6 setzen.
2. `git fetch origin koordination` und den Ordner `koordination/` in das Scratchpad der neuen Sitzung
   kopieren – das ist das neue `S`. In `PAKET_COMMON3.md`, `PAKET3_*.md`, `mag.sh`, `final.sh` den alten
   Pfad von `S` (seit der Sitzung vom Abend des 18.11.: `/home/claude/kernengine.de/koordination`, davor
   `/tmp/claude-0/-home-claude/87277680-…/scratchpad`) durch das neue `S` ersetzen (`grep -rl` + `sed`).
   Ordner `uebergabe/`, `stand/`, `mag/` dort belassen bzw. anlegen. **Seit dem Abend des 18.11. ist `S` der
   Ordner `koordination/` des ausgecheckten Zweigs `koordination` selbst** (Abschnitt 16) – eine neue Sitzung, die
   das Repo wieder nach `/home/claude/kernengine.de` klont und dort `koordination` auscheckt, muss nichts ersetzen.
3. Worktree für die Warteschlange: `git -C /home/claude/kw worktree add $S/magjob origin/mag/auftrag` und
   dort `git checkout -B mag/auftrag origin/mag/auftrag` (mag.sh pusht von dort). Test:
   `$S/mag.sh probe origin/work src/settings.cpp src/game.cpp` (ein paar Minuten).
4. Agenten-Worktrees gibt es nicht mehr: ein Nachfolger legt `/home/claude/w-<paket>` neu an –
   `git worktree add /home/claude/w-<paket> -B work-<paket> origin/work-<paket>` (der Zweig besteht schon;
   **nicht** von `origin/work` neu abzweigen). Das im Startauftrag jedes Agenten ausdrücklich sagen
   (der „Nachtrag 16:20 – Nachfolger“ in PAKET_COMMON3.md geht von einem bestehenden Worktree aus).
5. Werkzeuge lokal: `clang++` 18, `g++` 13, `python3`, `gh`. Keine Paketinstallation nötig.
6. Diese Datei nach Schritt 1 aus Abschnitt 10 mit dem neuen Stand fortschreiben (Branch `koordination`),
   damit eine weitere Übergabe jederzeit möglich ist.

---

## 12. Abschluss (erst wenn Abschnitt 1 „Fertig heißt“ 1–6 erfüllt ist)

1. Auf `work`: alle CI-Jobs grün auf genau dem Stand (Dispatch, Ergebnis abwarten).
2. `BLUEPRINTS.md` aktuell; Changelog-Eintrag (Format Abschnitt 2, Berliner Zeit aus dem Zeit-Werkzeug).
3. Squash von `work` nach `main` (ein Commit, kein Force-Push), pushen; CI auf `main` abwarten – `deploy`
   läuft nur bei grünem `build`; die anderen Jobs trotzdem prüfen. Live prüfen.
4. `work` danach per Merge-Commit mit `main` verbinden (kein Force-Push), weiterarbeiten wie gehabt.

---

## 13. Was in diesem Ordner liegt (und in welcher Reihenfolge lesen)

1. `UEBERGABE.md` – diese Datei.
2. Im Repo, Zweig `work`: `kern/games/spawncity/BLUEPRINTS.md` (**ganz**; der Plan, die Abbildung, die
   Kursprüfung), `kern/docs/blueprint_cpp.md` (die Sprache), Kopf von `tools/cpp_to_blueprint.py`,
   `tools/source_diff.py`, `tools/engine_api.py`.
3. `ANSATZ.md` (der Ansatz in Chris' Worten, 14.11. – gilt weiter; die dort genannte „Schreibphase ohne
   Prüfläufe“ ist überholt: geprüft wird jetzt laufend auf mag), `FORK_RULES.md`, `BRIEFING.md` (ältere
   Regeln für Agenten; wo sie `PAKET_COMMON3.md` widersprechen, gilt `PAKET_COMMON3.md`).
4. `PAKET_COMMON3.md` + `PAKET3_*.md` – die Aufträge. `UEBERGABE_AGENT.txt` – der Wortlaut, mit dem die
   Agenten zur Übergabe aufgefordert wurden.
5. `uebergabe/*.md` – die Übergaben der Agenten (je Paket; die wichtigste Quelle für den Neustart).
   `stand/*.md` – kurze Stände.
6. `f9_roots.txt`, `f9_waiting.txt`, `f9_convert.log.xz` – der letzte Lauf mit vollständigen Listen.
7. `mag.sh`, `final.sh`, `pilot/` (Messprotokolle des Piloten vom 15.11.), `g1/`, `g2/` (Notizen zu
   Absenkungen), `s5/` (Benchmark für Paket p, falls geliefert), `k/`, `r/`.

---

## 14. Was Chris in den neuen Chat schreibt (Vorschlag)

> Du übernimmst als Koordinator das Projekt SpawnCity auf Kern. Repo ChristianHohlfeld/kernengine.de
> (anhängen, Schreibzugriff). Hol den Branch `koordination`, lies `koordination/UEBERGABE.md` ganz und alles,
> was Abschnitt 13 nennt, dann mach genau bei Abschnitt 10, Schritt 1 weiter. Agenten als Opus 5.5 auf
> mittlerer Stufe, du überwachst. Vollständig, sauber, nichts überspringen. Antworte auf Deutsch.

---

## 15. Letzte Meldungen der Agenten bei der Übergabe (18.11., 17:15) – ergänzt Abschnitt 8

Letzte Commits (alles gepusht): `work-g1 7d1d656`, `work-g2 ffa277f`, `work-f1 3903052`, `work-f2 687b163`,
`work-t1 c7c49ed`, `work-t2 a546621`, `work-k e5f2bf1`, `work-p 04f1a57`, `work-s5 e749453`,
`work-d edae85f` (ein Commit über `work`: „Zwischenstand: Plätze der Engine – der Generator erkennt sie, die Tür
kann sie lesen und setzen“, unbewiesen; Stand, Schrittliste und bestätigter Entwurf in `uebergabe/d.md` – vom Agenten
zur Übergabe vollständig fortgeschrieben: Schritt 1 und 2 von 7 gepusht – der Generator erkennt 6 Plätze, darunter
`game_world.shots`, `events`, `sound_cues`, `hud_mut`; die Tür kann sie lesen und setzen, nur Syntax geprüft; es fehlen
Interpreter, Klasse, Schreibweise mit check() und Palette, Tests und Doku, Stellvertreter im Test, Konverter), `work-c`: kein Commit (nur Entwurf, `uebergabe/c.md`),
`work-r fde4b9f`.

**Wichtig – fast nichts davon ist durch einen Lauf bewiesen.** mag hat von 15:39 bis 16:52 keinen Auftrag
gerechnet (alte Schleife, blockiert durch einen vollen Lauf); die Agenten haben lokal mit Teil-Proben
gearbeitet. „Geschrieben und verglichen“ in einer Probe heißt nur: der Test übersetzt und der Konverter meldet
die Funktion nicht als unvergleichbar. **Bit für Bit geprüft wird erst in der Spiel-Suite.** Bewiesen im
strengen Sinn sind nur: Paket d Teil 1 (CI), der Differenztest `life` (f2: 1 Paar, 300 Aufrufe, Basis
`955503b`; alle 7 Tests in `tests/lowered` auf work-f2 grün, 128 Checks) und die 32 Paare des Piloten.

### Entscheidungen, die der neue Koordinator als Erstes treffen muss

**(E1) Paket p – die Hauptursache der 17× (65–70 % der Zeit): Rechnen mit ganzen Zahlen läuft in der Klasse
über double** (jeder Operand umgewandelt, `bits_of`, `whole`, `normal`; `kern::to<Int>(double)` wird nicht
eingebettet; `gf_mul` allein 48 %). Weitere Ursachen: `math_compare` vergleicht den Operator zur Laufzeit als
Text (~7,5 % – mit `if constexpr` bitgleich behebbar), Schleifenwächter und Schleifenzustand im Speicher (ein
paar Prozent), Indexprüfung (~0,5 %). Nur qr encode ist profiliert (Float, Strukturen, Text, Maps, Int64 noch
nicht). Der Agent nennt zwei bitgleiche Wege: (i) der Generator leitet am Graphen den Wertebereich her;
(ii) ein geprüfter Weg über i64, der auf double zurückfällt, sobald ein Zwischenwert ±2^53 verlässt.
*Empfehlung des scheidenden Koordinators (nicht mehr geprüft – erst lesen, dann entscheiden):* (ii) zuerst,
**innerhalb der Umsetzung der Knoten** (`nodes.h`/Generator), ohne die Bedeutung zu ändern: die Sprache legt
fest „ganze Zahlen exakt bis 2^53, sonst wie double“; ein i64-Pfad mit Bereichsprüfung von Operanden und
Ergebnis rechnet in diesem Bereich **dasselbe Bit für Bit** und fällt sonst auf die heutige Formel zurück –
das ist eine schnellere Ausführung derselben Bedeutung, kein neuer Zahlentyp (kein Verstoß gegen „64 Bit,
kein Float32“). Prüfen, ob das wirklich Schreibweise/Leser berührt (der Agent meint ja; wenn die Bedeutung
gleich bleibt, sollte der Text gleich bleiben können). (i) danach, wo die Prüfung selbst noch kostet.
Beweis: Engine-Suite samt nativer Stufe bitgleich, Messung je Funktion, dann der Zeit-Test.
Das Messwerkzeug über 20–30 Funktionen ist noch nicht gebaut; `tools/blueprint_cpp.sh --classes` auf work-p
ist ungetestet und berührt dieselbe Datei wie work-s5 (von Hand zusammenführen).

**(E2) Paket c – Entwurf „Rückruf der Engine“** (`uebergabe/c.md`, ganz lesen): `Callback<R(A…)>` = Funktion +
höchstens ein gebundener Platz (const-Referenz); nur die Engine ruft ihn; er darf nur eine Funktion nennen, die
nichts vom Blueprint braucht (`static` in der Klasse → Wiedereintritt ist nur Lesen, Jobs sind threadfest);
Knoten „Make Callback“, Text `callback<&Lib::fn>(platz)`; Klasse: Thunk + `void*` (Kosten wie heute);
Interpreter/JIT: Trampolin auf `Runtime::call_static`; Lebensdauer an das Engine-Objekt/den Aufruf gebunden.
Der Entwurf ist **noch nicht bestätigt**. Drei Fragen des Agenten: (1) `register_node` in game.cpp (das
Programm meldet einen Knoten auf „logic.tick“ an, der `fixed_step` ruft) – Absenkung in game.cpp (Paket g1)
oder Regel des Konverters? *Empfehlung:* im Blueprint ist das ein Event-Handler des Blueprints Game
(`event_tick`); wählen, was ohne Sonderregel auskommt – eine allgemeine Konverter-Regel nur, wenn „ein Programm
meldet einen Tick-Handler an“ eine Form ist, die jedes Spiel so schreibt; sonst Absenkung mit Differenztest.
(2) Jobs (`KERN_JOB`, drei Anmeldungen: `run_tile`, `run_buildings`, `dem_grid_job`): statische Erklärung
`KERN_JOB(kind, &Lib::fn);` in einer Bibliothek – *Empfehlung:* bestätigen, mit dem Nachweis, dass Klasse
(Threads) und Interpreter (`pump` auf dem Frame-Thread) dasselbe Ergebnis geben (Determinismus-Job!).
(3) Berührung mit Paket d in `engine_api.py` (HeightSampler/RoadOptions wären keine Handles mehr) –
nacheinander bauen: erst d fertig, dann c.

**(E3) Paket k** – vor dem Mergen: Zweig auf `origin/work` nachziehen (9 Commits zurück); Beweis abwarten:
die Aufträge `work-k-konvert-165842-30470` (8 Kerne) und `work-k-konvert-165847-30555` (40 Kerne) sind auf mag
eingereiht → erste Commits der beiden Ergebnis-Zweige byteweise vergleichen; gegen einen gültigen Basislauf:
keine Funktion verloren. Jede Zeile „was written and waits now“ ist ein Befund am Inhalt → an das Paket der
Datei. Der Lauf endet nur sicher, solange die Umwandlung monoton ist (nicht erzwungen, s. `uebergabe/k.md`).

### Befunde aus den Übergaben, die über ein Paket hinausgehen
- **t1:** „verglichen“ in Proben ≠ ausgeführt (s. o.). Fünf Proben wurden wegen des Sitzungswechsels abgebrochen
  (lamps/townsync/world, swarm/military, gaps/events, weapons/foot_rounds, city_stream+terrain).
- **t2:** `struct AimTarget {…} g_aim;` stand im Test doppelt, g++ machte `g_aim` zu `int` (Ursache der
  f9-Meldung bei `Game::foot_input`, `ctl_assist`; gleiches Muster `g_view`) – behoben in `a546621`, im
  Konverter noch nicht durch einen Lauf bestätigt. `json::Value::is_object/items` warten jetzt ehrlich
  (Engine-Objekt) – hängt an „JSON als Typ der Sprache“ (9.6). Seine Probe über alle 77 Quelltexte
  (`work-t2-probe-163129-9216`) läuft auf dem Stand **vor** seinen Änderungen; danach eine zweite auf `a546621`.
- **g1:** Absenkung `game_gaps.cpp` (`gaps_sirens`, `gaps_jet_audio`; `--base b13f80a`, in `g1/absenkungen.txt`)
  – nur Syntax geprüft. Bedarf an der Engine: `Mixer::set_bus`, `TextureDesc::rgba`; offene Fragen zu
  `register_node` und `build_level` (s. E2 und 9.6). Mehrere „Wurzeln“ in f9 erben nur den Grund einer
  gerufenen Funktion.
- **g2:** vier Absenkungen, je ein Commit, nur Syntax geprüft (`g2/absenkungen.txt`): `worldsync_update`,
  `minimap_json` (ruft je Merkmal einmal mehr `city_.features(tile)` – **beim Beweis auf die Zeit achten**;
  wenn das die Kosten ändert, ist es keine zulässige Absenkung), `traffic_make_near`, `mp_` als Wert mit Merker
  `mp_made_` statt `unique_ptr`. Ursache nicht gefunden für „a pointer“ (`traffic_rules, step_world,
  world_command, bigmap_json`) und „list: erase()“ (`update_world, rebase_origin`) – zeigt nur die Probe über
  alle Game-Dateien (`work-g2-probe-164700-20497`, Ergebnis-Zweig selbst abholen). Wartet auf d (`shots()`)
  und f2 (`ring_put` in world_marks hält `town_mark`, `world_blast` auf).
- **f1:** `0b58333` bewiesen per lokaler Probe (mvt 30/30); `3903052` Zwischenstand: sieben Konverter-Regeln
  (fünf per Probe bestätigt, zwei ungeprüft), terrain.cpp `const auto&`. Die Differenztests des Vorgängers
  übersetzten **nie**; neu erzeugt; nur mvt, tiles, city_jobs übersetzen nachweislich; **keiner ist gelaufen**.
  facades/water_look brauchen keinen Test (nur ein `#include` kam dazu). Nächster Schritt:
  `mag.sh spiel origin/work-f1 lowered`, dann Probe über alle acht Quelltexte.
- **f2:** `687b163` ist Zwischenstand (Lists Equal für Strukturen: Prüfung im Konverter `Emit.memberwise`,
  Engine-Test, Doku) – Syntax von `test_main.cpp` ungeprüft, Probe von terrain.cpp fehlt.
- **s5:** nach dem Einmischen von `origin/work` (mit Paket d) müssen Spiel- und Engine-Suite auf `e749453`
  laufen; die eingereihten Aufträge laufen noch auf `59716b8`. Der Konverter-Lauf mit den neuen Teilen
  (Ordner „Program“, Header vor dem Lesen) fehlt. Die Löschung von qr_code steht im Engine-Commit `a12bc2c`.
- **Auf `work` fehlt nichts mehr für den Leser:** `3b355c4` (lokale Map/Set) ist drin – die Notiz in t1s
  Übergabe dazu ist überholt.

### mag bei Übergabe
17:16: zehn Ergebnis-Zweige; sechs Aufträge liefen (darunter Suiten, ~30 Minuten), ~35 warteten; die neue
Fassung der Schleife (`fe843e4`: 14 zugleich, Spiegel) greift danach. Viele wartende Proben sind von Agenten,
die es nicht mehr gibt – ihre Ergebnisse sind trotzdem brauchbar (Zweig `mag/ergebnis-<name>`, Name beginnt mit
dem Paket-Zweig). CI-Lauf `44341582015` (work `90b5ab8`): determinism und native grün, Rest lief noch.

---

## 16. Fortschreibung – Sitzung `session_012NJs7i46AnXdFpunu1X3k9` (Koordinator ab 18.11.2026, 17:22)

### 16.1 Wie diese Sitzung aufgesetzt ist (ersetzt Abschnitt 11, soweit es abweicht)
- Klon: `/home/claude/kernengine.de` (vollständig, nicht flach; Fetch-Regeln für `mag/*`, `work-*`, `work`, `koordination` gesetzt), ausgecheckt ist der Zweig
  **`koordination`** – der Ordner `koordination/` darin **ist `S`** (`/home/claude/kernengine.de/koordination`). Was Agenten nach `$S/stand`, `$S/uebergabe`
  schreiben, ist damit sofort versionierbar: `git -C /home/claude/kernengine.de add koordination && commit && push origin koordination`.
  Nicht versioniert (`.git/info/exclude`): `koordination/mag/` (geholte mag-Protokolle), `magjob/`, `final/`, `shared-tmp/`, `*.lock`.
- `/home/claude/kw`: Worktree auf `work` (nur der Koordinator). `$S/magjob`: Worktree auf `mag/auftrag` (mag.sh pusht von dort).
  Agenten: `/home/claude/w-<paket>` auf `work-<paket>` (PAKET_COMMON3.md, „Nachtrag 17:45 – NEUE SITZUNG“ – dort steht alles, was ein Agent dieser Sitzung wissen muss).
- `mag.sh` ist neu gefasst: holt das Ergebnis unter seinem eigenen Namen (die alte Fassung las `FETCH_HEAD` – bei mehreren Aufrufen zugleich konnte ein Agent das
  Ergebnis eines anderen bekommen), legt mit Wiederholung ab, kennt `MAG_NOWAIT=1`, `holen <name>`, `warten <name> [min]`, `stand`.
  Ein Vordergrund-Befehl der Sitzung endet nach 10 Minuten – darum `warten` in Stücken.
- Der Git-Zugang der Sitzung lässt nur 2 Übertragungen zugleich zu (HTTP 429 → warten, wiederholen).
- Agenten haben in dieser Sitzung **keinen Rückkanal** während der Arbeit: sie schreiben nach `$S/stand/<paket>.md` und enden mit Frage/„BEREIT ZUM MERGEN“/Bericht;
  der Koordinator setzt sie mit der Antwort fort.

### 16.2 Schritt 1 – Bestandsaufnahme (Stand 18.11., 17:35)
- **CI-Lauf 44341582015** (`work` 90b5ab8), abgeschlossen: determinism, build, native, input, e2e **grün**; native-spawncity **rot** (erwartet: Level/Tests von f8);
  deploy übersprungen.
- **mag:** die Schleife mit 14 Plätzen und Spiegel greift seit **17:25:38** (zehn Starts in sieben Sekunden). Manche Aufträge zeigen ihren Ergebnis-Zweig erst am Ende
  (der Push beim Start geht bei vielen gleichzeitigen Starts teils verloren) – „kein Zweig“ heißt nicht „läuft nicht“.
- **Der volle Lauf `work-voll-162644-5528` ist um 17:38:59 gestartet, auf 90b5ab8 (= `work`), 37 Kerne** (er stand alphabetisch am Ende der Schlange; neben ihm
  laufen u. a. vier `konvert`-Läufe von k, die Suiten von s5, die t2-Probe über alle Quelltexte). Erwartet: Konvertierung ~1 h+, dann Spiel-Suite, dann
  Engine-Suite (die wird wegen F1 rot sein). **Bis sein Ergebnis übernommen ist, wird nichts auf `work` gepusht** – sonst liefe er nicht mehr „auf dem Stand, der
  heute `work` ist“ (Abschnitt 10, Schritt 1). Übernahme: `git ls-remote origin 'refs/heads/mag/ergebnis-work-voll-162644-5528'`; `when` muss `convert rc=0`
  nennen und convert.log Funktionen; dann den Commit „… – Level und Tests der Blueprints“ per cherry-pick nach `work`, pushen, `gh workflow run ci.yml --ref work`.
- Aus der Schlange genommen (Commit 3595a93 auf `mag/auftrag`): fünf Proben von work-t1 (vor dem Leser-Fix, laut t1 überholt) und die Zeitmessung work-k-zeit.
- **Befund F1 – die Engine-Suite ist auf dem Stand von `work` auf mag ROT** (drei Läufe auf 30f63e4, Baum identisch mit 90b5ab8: `work-d-engine-154346-16923`,
  `-161602-25272`, `-163138-9400`, je `engine rc=1`): 54.323 Checks der Unit-Tests ohne Fehler, dann `blueprint_native_test`: „Engine on the engine itself, the
  class calling the engine itself: after Free1: R129=[1, 0, 0, 0, 2, 0, …] (16 Einträge) – through the door: R129=[0, 0, 0, 0]“. In CI galt R129
  (jobs_take_queued) als nicht wiederholbar und wurde nicht verglichen (darum dort grün). **Paket d, Teil 1, ist damit nicht bewiesen** – Auftrag an d: PAKET3_d.md,
  Nachtrag 17:50 (zuerst das). Zum Vergleich: `work-f1-engine-153945-13867` auf 0b58333 (vor Paket d) ist grün (54.370 Checks, native Stufe ok, 32m47).
- **Spiel-Suite auf dem Stand von `work`** (`work-d-spiel-154354-17035`, rc=1, 21 min): bricht wie erwartet im Vergleich `converted_car` ab (`Car_new` – Level von f8
  gegen heutige Quelle); bis dahin 7 `cost:`-Zeilen (Firmware `*_reset`/`*_new`, `CarSpec_car`). Sagt über den heutigen Konverter nichts.
- `work-f1-spiel-153945-13864` (rc=123, auf 0b58333, also die alten, nie übersetzten Differenztests): übersetzt nicht (`source_diff.h`: „a value the test cannot
  compare/carry/draw“, city_stream/interiors: `Tile`, `LifeArt`) – wie in f1s Übergabe beschrieben; gilt nicht für 3903052.

### 16.3 Entscheidungen des neuen Koordinators (18.11., 17:50) – bindend, bis Chris anders entscheidet
- **E1 (Paket p):** PAKET3_p.md, Nachtrag 17:50. Kern: Die Klasse bleibt Wort für Wort `to_cpp` (ein C++ je Blueprint, kein zweiter Erzeuger). Die Bedeutung
  bleibt (64 Bit, `Int` wie `normal`/`whole`). Ohne Rückfrage: `math_compare` zur Übersetzungszeit, `kern::to<Int>` eingebettet, der geprüfte ganzzahlige Weg
  **in der Umsetzung der Formel-Funktionen** (Text unverändert, Ränder im Engine-Test bewiesen), Straffungen ohne Bedeutungsänderung. Jede Änderung der
  **Schreibweise** (z. B. Formel mit ganzzahligen Ein-/Ausgängen, am Graphen bewiesener Index) nur nach Entwurf – das wäre ein Baustein der Sprache (Antwort a).
  Zuerst das Messwerkzeug und die vollständige Ursachenliste (bisher nur qr encode).
- **E2 (Paket c):** PAKET3_c.md, Nachtrag 17:50. Entwurf `Callback<R(A…)>` **bestätigt** mit zwei Auflagen (der Stellvertreter vergleicht, was der Rückruf gibt –
  mit gewürfelten Eingängen, auch für behaltene Rückrufe). `register_node`: kein Rückruf, **keine** NodeDef-Regel im Konverter – eigener Entwurf „Events des
  Programms“ zusammen mit `build_level` (Stufe 6). Jobs (`KERN_JOB(kind, &Lib::fn);`) als Teil 2 bestätigt, mit Determinismus-Nachweis. Gebaut wird **nach** Paket d.
- **E3 (Paket k):** PAKET3_k.md, Nachtrag 17:50. Der Koordinator vergleicht die zwei laufenden `konvert`-Ergebnisse selbst; vor dem Mergen origin/work einmischen
  und den Beweis auf dem eingemischten Stand wiederholen, gegen den gültigen vollen Lauf „nichts verloren“.
- **Start der Agenten (Schritt 2):** siehe 16.4 – wer gestartet ist und wer bewusst noch nicht.

### 16.4 Schritt 2 – Agenten (gestartet 18.11., 17:40; alle Opus 5.5, mittlere Stufe; Worktrees `/home/claude/w-<paket>`)
| Paket | erster Auftrag in dieser Sitzung | endet mit |
|---|---|---|
| **d** | ZUERST Befund F1 (R129, Engine-Suite rot) – Fix in eigenen Commits, Beweis: Engine-Suite zweimal grün + CI; dann Platz der Engine ab Schritt 3/7; Antwort zu `Mixer::bus`, `TextureDesc::rgba` | „BEREIT ZUM MERGEN (Engine-Suite)“ in stand/d.md, arbeitet weiter |
| **p** | Messwerkzeug, vollständige Ursachenliste, Behebung nach E1 Punkt 3 (Text unverändert), Entwurf für alles, was die Schreibweise ändert; Zeit-Test | Tabelle vorher/nachher; ggf. „ENTSCHEIDUNG NÖTIG“ (Schreibweise) |
| **t1** | origin/work einmischen, abgebrochene Nachweise auf mag (gebündelt), Rest der drei Gründe, Prüfung „beidseitig gescheiterte Aufrufe gelten als gleich“ (9.2), voller Lauf als Abschlussbeweis | „BEREIT ZUM MERGEN“ mit Zahlen gegen den Basislauf |
| **t2** | Probe über alle Quelltexte (läuft seit 17:33 auf a546621) auswerten, drei Hypothesen, set_craters-`operator==` (von f2), frame_uniforms/Renderer (von f1), voller Lauf als Abschlussbeweis | wie t1 |
| **k** | origin/work einmischen (getan: 3dd3338), 8 gegen 40 Kerne byteweise – auf e5f2bf1 (läuft) und auf dem eingemischten Stand (läuft seit 17:41), „nichts verloren“ gegen den Basislauf | „BEREIT ZUM MERGEN“ mit vier Auftragsnamen und Dauer |
| **g1** | Probe über alle game*.cpp auswerten, eigene Gründe als Konverterfähigkeit, Absenkungen nur vorbereiten; Beschreibung für den Entwurf „Events des Programms/build_level“ (Stufe 6) | Tabelle je Funktion; „BEREIT ZUM MERGEN (Konverter-Teil)“ |
| **g2** | Probe über alle game*.cpp auswerten, ungeklärte Gründe („a pointer“, „list: erase()“), minimap_json-Absenkung auf gleiche Kosten prüfen, traffic_make_near-Annahme belegen | wie g1 |
| **f1** | origin/work einmischen, Differenztests grün (`spiel … lowered`), Probe über alle acht Quelltexte, ungeprüfte Regeln aus 3903052 | „BEREIT ZUM MERGEN“ mit Paaren/Aufrufen |
| **f2** | ZUERST test_main.cpp:4796 (687b163 übersetzt nicht), Lists Equal für Strukturen in jeder Stufe bewiesen, Proben auswerten, Differenztests grün | wie f1 |
Bewusst **nicht** gestartet: **c** (Entwurf bestätigt, baut nach d – engine_api.py), **s5** (wartet auf p), **r** (bei der CLI auf mag; `mag/differenztest`
zuletzt 3c536af, 17:00, noch kein BERICHT.md). Die agentIds dieser Sitzung stehen in `/home/claude/agenten.txt` (nicht im Repo).
Kontingent: 17:08 → 31 %, **17:46 → 32 %** (Chris). Das Einlesen des Koordinators und der Start der neun Agenten haben also etwa einen Punkt gekostet; die
Rate mit neun laufenden Agenten zeigt erst die nächste Zahl (erbeten für ~18:45). Regel bleibt: bei ~8 %/h zurück auf d, p, t1, t2, k.

### 16.5 Was der Koordinator als Nächstes tut (Reihenfolge)
1. Ergebnis von `work-voll-162644-5528` abwarten → Summenzeilen, `suite`/`engine` rc, `cost:`-Zeilen → Commit 1 nach `work`, pushen, CI (Schritt 1 fertig); die neuen
   Zahlen in Abschnitt 5 und in BLUEPRINTS.md (Stand-Absatz) nachführen.
2. Stand-Dateien lesen (`$S/stand/*.md`: „BEREIT ZUM MERGEN“, „ENTSCHEIDUNG NÖTIG“, „BEFUND FÜR …“), Befunde weitergeben, Entscheidungen treffen.
3. Mergen nach Abschnitt 10, Schritt 3 (sechs Bedingungen) – zuerst der Fix von d (F1), dann k, dann t1/t2, dann g/f; danach `voll origin/work`.
4. Entwurf „Events des Programms / build_level“ (Stufe 6) anfordern, sobald g1 die Beschreibung geliefert hat; c starten, sobald d fertig ist; s5, sobald p die
   Ursachen behoben hat.

### 16.6 Stopp der Agenten (18.11., ~17:50)
Alle neun Agenten wurden etwa zehn Minuten nach dem Start von der Nutzerseite gestoppt (zeitgleich mit einer unterbrochenen Antwort des Koordinators um
17:49/17:50) – **kein Bericht kommt mehr von ihnen; neu gestartet wird nur auf Chris' Wort.** Entscheidung Chris 17:56 zur Frage, die Läufe vom Mergen zu
entkoppeln: **„wir lassen wie gehabt“** – die Regeln aus Abschnitt 2/10 gelten unverändert (Beweis vor jedem Merge, Absenkung nur mit grünem Differenztest).
Was die Agenten bis dahin gepusht haben (alles auf GitHub, Arbeitsbäume sauber – außer p):
- **d** `5367e14`: Fix für F1 als eigener Commit **`6f62201`** (Eltern: `work` 90b5ab8, enthält nur den Fix). Ursache laut stand/d.md: die Jobs des Falls liefen
  auf Worker-Threads, das Ergebnis von take_queued hing vom Ablauf ab; `take_queued`/`done` nativ ohne Sperre. Lösung: `jobs::set_threads(false)` (zwei neue
  Knoten), Sperren, jedes Spiel per fork vom selben Zustand, Uhren aus der Tabelle (`KERN_CLOCK`), jede andere Abweichung ist ein FAIL. **Beweis läuft noch:**
  mag `work-d-engine-174905-11207`, `work-d-engine-174911-11261`, CI `44351625418` – vor dem Übernehmen ansehen. Antworten zu `Mixer::bus`/`TextureDesc::rgba`:
  der Entwurf trägt beide nicht (stand/d.md, „C“).
- **t1** `77d96e1`: origin/work eingemischt; **Lücke gefunden und geschlossen (unbewiesen):** im Durchgang gegen die echte Engine (`SC_ENGINE=real`) zählte ein
  Aufruf, den die Tür abweist (Handle 0 = „kein Objekt“), als verglichen und gleich (stand/t1.md, „E“). Probe über alle Quelltexte: `work-t1-probe-174058-4177`.
- **t2** `965b490`: origin/work eingemischt; Probe `work-t2-probe-163129-9216` lief noch.
- **k** `3dd3338`: origin/work eingemischt; vier `konvert`-Läufe auf mag (e5f2bf1: 8/40 Kerne; 3dd3338: `work-k-konvert-174058-4191`, `-174111-4316`).
- **g1** `8522b2f`, **g2** `2362cf1`: origin/work eingemischt; die Proben über alle game*.cpp liefen noch (auf dem Stand davor).
- **f1** `172d40b`: origin/work und `mag/differenztest` eingemischt, Differenztests neu erzeugt (mvt 2, tiles 3, terrain 3, city_stream 3, interiors 1,
  city_jobs 23+4); mag: `work-f1-spiel-174450-8421` (lowered), `work-f1-probe-174453-8472`.
- **f2** `139fbcf`: test_main.cpp übersetzt wieder, Stufen-Fall `lists_equal_structs`; mag `work-f2-engine-174611-9407`. **Befund für den Koordinator:** die
  Auftragsart `probe` ruft den Konverter ohne `--level` – jede Funktion, die den Blueprint eines Regel-Actors ruft, wartet dort mit „the Blueprint of the actor X,
  which the level does not have“ (Artefakt der Probe). Vorschlag: `--level level/spawncity.klevel` im Zweig `probe` von lauf2.sh. Noch nicht umgesetzt.
- **p** `04f1a57` unverändert; **ungepushte Arbeit** (logic.h, notation.cpp, tools/blueprint_cpp.cpp, tests/bench/) liegt im Worktree `/home/claude/w-p` und als
  Sicherung in `koordination/p/unterbrochen_1750/` (Patch + bench) – ungeprüft, mitten in der Änderung.
mag rechnet unabhängig weiter (der volle Lauf `work-voll-162644-5528` seit 17:38:59).

### 16.7 Kontingent und der schnellste Weg (18.11., 18:10)
- **Kontingent:** 17:46 → 32 %, **17:56 → 36 %** – vier Punkte in zehn Minuten (Kaltstart von neun Agenten, die sich zugleich einlesen, plus mehrere kurze Antworten
  des Koordinators, der den ganzen Projektstand im Kontext hält – jede Antwort des Koordinators kostet etwa so viel wie der Kaltstart eines Agenten). Das Kontingent,
  nicht die Uhr, ist der Engpass: 64 % müssen bis zum Reset am 25.11., 13:00, reichen.
- Chris, 17:59: „Finde die schnellste Lösung alles zu finalisieren“. Ein gestoppter Agent lässt sich nicht fortsetzen (SendMessage wird abgewiesen); ein Neustart
  braucht Chris' ausdrückliches Wort. Empfehlung des Koordinators: nur der kritische Pfad – **d, p, t1, t2, k** –, g1/g2/f1/f2 erst, wenn t1/t2/d gemergt sind
  (viele ihrer Funktionen warten auf diese drei); c nach d, s5 nach p. Wenige, seltene Kontrollen des Koordinators (jede kostet).
- **Ohne Kontingent getan (mag):** lauf2.sh `d6a7ddc` – Probe mit Level (Befund f2), voller Lauf mit Engine-Suite parallel und frühem Push von Level und Tests
  (spart je vollem Lauf rund 55 Minuten bis zur Übernahme). Die neue Schleife hat sich um 18:04 selbst ersetzt und startet Aufträge (geprüft: `work-probe-180405-238`,
  `work-f2-engine-174611-9407` um 18:04:22). mag.sh zeigt den Zwischenstand eines vollen Laufs.
- **Reihenfolge der Merges, sobald das Ergebnis des vollen Laufs übernommen ist:** Fix von d (`6f62201`, wenn seine zwei Engine-Suiten und CI 44351625418 grün sind) →
  k (Konvertierung parallel: verkürzt jeden weiteren Lauf) → t1/t2 → dann `voll origin/work`.

### 16.8 Stand 18:55 – der Basislauf auf `work` ist verloren; der Weg über eine Probe-Zusammenführung
- **`work` (90b5ab8) lässt sich nicht ins Level lesen:** `net_session_cpp.cpp: line 611, column 20: the number 78 names two nodes` (Probe `work-probe-182522-630`;
  ebenso alle vier `konvert`-Läufe von k). Der Basislauf `work-voll-162644-5528` (seit 17:38:59) endet darum mit `convert rc=1` und dem alten Level – **es gibt weiter
  keinen gültigen vollen Lauf seit dem 14.11.** Sein convert.log bleibt als Liste „geschrieben/wartet“ für 90b5ab8 brauchbar. Auf work-f2 liest net_session (67/67).
  Hinter einem Lesefehler können weitere liegen (work-f1 hat zwei weitere Leser-Korrekturen: `entries_ended`, `assigned`).
- **Gemergt nach `work`: work-d** (Merge 4283186, Baum 5367e14) – F1 behoben, bewiesen (Engine-Suite auf mag zweimal grün, CI 44351625418, Generator ohne
  Abweichung). `work` = 4283186.
- **k bewiesen eindeutig:** 8 gegen 40 Kerne byteweise gleich (tests/converted) auf zwei Ständen; 41–45 min bei 40 Kernen unter voller Last (14 Runden).
  Acht Rückfälle „was written and waits now“ (game_look ×7, game_lamps ×1 – Test übersetzt nicht mehr: Renderer unvollständig, `Game` nicht deklariert → t2).
- **Probe-Zusammenführung `work-zusammen` (9931aea)** – Inhalt und die drei aufgelösten Doppelbauten: `stand/KOORDINATOR.md`, Eintrag 18:55. Der volle Lauf
  `work-zusammen-voll-185405-1226` ist der Beweis für genau diesen Baum: Level und Tests erscheinen gleich nach der Konvertierung (`$S/mag.sh holen <name>` zeigt den
  Zwischenstand), dann Spiel-Suite, Engine-Suite parallel. **Ist `convert rc=0`:** das ist der erste gültige Stand seit dem 14.11. – Zahlen lesen, Suite abwarten;
  sind die Suiten grün (oder nur an Stellen rot, die es auf `work` auch sind und die benannt werden), `work-zusammen` samt dem Commit „Level und Tests“ nach `work`
  mergen, CI. **Ist `convert rc≠0`:** der nächste Lesefehler steht in `mag-zwischen/convert.tail.txt` – beheben (Besitzer der Datei) und neu.
- Nicht in der Zusammenführung und weiter offen: die Absenkungen der Game-Dateien von g1/g2 (warten auf den Differenztest „zwei Bauten“), alles von f1 außer dem
  Konverter (seine Differenztests übersetzen nicht), p (ungepusht, gesichert), s5.
- Agenten: weiterhin keine (Neustart nur auf Chris' ausdrückliches Wort; seit 17:59 keine Nachricht von ihm).

### 16.9 Stand 19:50 – erste Zahlen des zusammengeführten Stands, zweiter Lesefehler behoben
- `work-zusammen-voll-185405-1226` (9931aea): Konvertierung **27 min** (mit dem Ablauf von k, 37 Kerne, 15 Runden; vorher rund 60 min).
  **Geschrieben 1.660 / 1.878 Funktionen (88,4 %), 18.926 / 31.103 Zeilen (60,8 %)** – aus den Zeilen je Einheit summiert (die Summenzeile fehlt, weil das Einlesen
  scheiterte). 101 Funktionen warten im Test („its test cannot compare it yet“), 9 Rückfälle. Engine-Suite **rc=0**. `convert rc=1`: Leser – „the input "pad" of
  Controls_DeviceInput has the name of a variable or a function“ (Game.cpp).
- Fix `c432166` auf work-zusammen (Konverter, `settled_extern` beim Zusammensetzen der Teile). Neuer voller Lauf `work-zusammen-voll-195003-886` (19:50).
  **Vorgehen bleibt:** `convert rc=0` → Spiel-Suite lesen → grün (oder nur benannt rot wie auf `work`) → work-zusammen samt „Level und Tests“ nach `work`, CI.
  Weiterer Lesefehler → Texte liegen jetzt in `mag-zwischen/texts` (lauf2.sh 9d621c4).

### 16.10 Stand 20:35 – Lesefehler verstanden, lokales Werkzeug zum Einlesen
- Der Lesefehler „pad“ kam aus einer **Bibliothek** (controls_cpp erklärt Controls_DeviceInput; Game benutzt sie und hat die Variable `pad`). Fix **8874e79**
  (work-zusammen, Konverter `main`: jede KERN_EXTERN-Erklärung einmal für das ganze Programm, `settled_extern`). c432166 (nur Teile) bleibt als Teil davon.
- **`koordination/lesen.py`**: liest die Texte eines vollen Laufs (`mag-zwischen/texts`, seit lauf2.sh 9d621c4) lokal ins Level, in der Reihenfolge der Bibliotheken –
  44 s statt eines Laufs. Mit der berichtigten Erklärung nimmt das Level alle 156 Texte des Laufs 195003 an: hinter diesem Lesefehler liegt auf diesem Stand kein weiterer.
- Läuft: `work-zusammen-voll-203000-750` (8874e79). Erwartet: `convert rc=0` gegen 20:55, Spiel-Suite gegen 21:15 – die erste Bit-für-Bit-Suite seit dem 14.11.
- Basislauf `work-voll-162644-5528` (90b5ab8): convert rc=1, suite rc=1, engine rc=1 (R129, vor dem Fix von d) – nur noch als Liste für 90b5ab8 brauchbar.

### 16.11 Stand 21:10 – gültiger voller Lauf, Agenten wieder gestartet
- **`work-zusammen-voll-203000-750` auf 8874e79: `convert rc=0` (20:53) – der erste gültige Lauf seit dem 14.11.** Konvertierung 22m43.
  „1632 of 1844 functions of the program are written as Blueprints (18813 of 30978 lines), and 28 of the engine's headers that they call: 156 Blueprints – 70 of
  source files, 1 of states, 85 of headers and of shapes“; „1589 functions of the Blueprints are compared with their source, bit for bit, by the tests in
  tests/converted … 5 give nothing“. 118 Funktionen „its test cannot compare it yet“, 9 Rückfälle. Zum Vergleich f8 (14.11.): 1.368 / 1.817, 11.996 / 30.741
  Zeilen, 1.324 verglichen. Die Spiel-Suite dieses Laufs (der erste ausgeführte Bit-für-Bit-Vergleich) rechnete um 21:10 noch.
- Chris, 21:05: **„Start“** (für d, p, t1, t2, k). Gestartet 21:08 als neue Agenten (Opus 5.5, mittlere Stufe; die gestoppten lassen sich nicht fortsetzen):
  **t2** (Arbeitsliste: die 118 + 9), **t1** (zuerst: Auswertung der Spiel-Suite als Arbeitsliste für alle; dann seine Gründe und die Lücke gegen die echte
  Engine), **p** (Vorgängerarbeit im Worktree sichern, Messwerkzeug, Ursachen, Behebung nach E1), **d** (Platz der Engine, Schritte 3–7). **k nicht gestartet:**
  sein Beweis (8 gegen 40 Kerne byteweise gleich, zwei Stände) ist erbracht, sein Ablauf steht in work-zusammen und hat die Konvertierung von ~60 auf 23 min gebracht.
- **Neuer Takt (PAKET_COMMON3.md, Nachtrag 21:10):** Basis für alle ist `origin/work-zusammen`; Agenten beheben und belegen mit Proben und melden „BEREIT ZUM MERGEN“
  in ihrer Stand-Datei; den vollen Lauf rechnet der Koordinator auf dem zusammengeführten Stand und bringt ihn nach `work`, wenn er grün ist.
- Noch nicht wieder gestartet (brauchen Chris' Wort): g1, g2, f1, f2; angehalten: c (nach d), s5 (nach p).

### 16.12 Stand 21:25 – die erste ausgeführte Spiel-Suite ist rot und brach ab; Auftragsart „alle“
- **`work-zusammen-voll-203000-750`: `suite rc=1` (21:15), `engine rc=0` (21:05).** Die Spiel-Suite lief nicht durch: nach 21 von 269 Tests (17 ok, 4 FAIL)
  brach AddressSanitizer das Testprogramm ab (heap-buffer-overflow in der Quelltext-Kopie `MeshData::push_extra`, Test `converted_engine_geo_mesh_data`: der Test
  gibt einen Zeiger in eine gezogene Liste, der Quelltext liest `extra_size` Einträge darüber hinaus – in C++ undefiniert, gehört in den Test-Schreiber, t1).
  246 Tests und der Durchgang `SC_ENGINE=real` liefen nicht. 121 `cost:`-Zeilen (93 „… instead of 0“ – Speicher, den die Klasse braucht und der Quelltext nicht:
  Arbeit für p/Konverter), 23 `note:`-Zeilen. **Damit ist `work-zusammen` nicht nach `work` zu bringen; `work` bleibt 42831863.**
- **Die 7 Gruppen der Auswertung (t1, `stand/t1.md` „SUITE 203000“) mit Zuordnung:** (1) push_extra-Abbruch → t1; (2) 15 Funktionen von city_stream – der Test
  nimmt `self` als Wert, die Zerstörung (`road_builder.release`) fällt ins Aufzeichnungsfenster → t1; (3) `earcut.triangulate(xy, vertex_count, …)` – der
  Stellvertreter nimmt vertex_count statt 2·vertex_count Zahlen (CityStream_building_shape, Destruction_tear/collapse; danach fracture/update/blast neu
  beurteilen) → d; (4) flak_cell 201/255: `snprintf("%lld")` mit großem i64 → Regel des Konverters; (5) Car_new 300/300 → f2-Gebiet; (6) CityStream_feed_terrain
  4/38, upload 35/300 → f1-Gebiet; (7) Drone_step (nur Klasse verglichen) → Regel des Konverters. 4–7 haben keinen laufenden Agenten (f1/f2 nicht gestartet).
- **Neue Auftragsart `alle` auf mag (lauf2.sh e01e7245, mag.sh):** `./mag.sh alle origin/mag/ergebnis-<voller lauf>` baut das Testprogramm einmal und lässt jeden
  Test `converted_<einheit>` in seinem eigenen Lauf rechnen – gegen den Stellvertreter und gegen die Engine selbst; ein Abbruch oder Fehlschlag hält die anderen
  nicht auf. So zeigt EIN Lauf die ganze Liste. Ergebnis in `mag-log/`: `alle.txt`, `alle-real.txt` (je Test ok/FAIL/ABBRUCH und eine Summenzeile),
  `alle-abweichungen.txt`, `alle-real-abweichungen.txt`, `alle-cost.txt`, `einzeln.tar.xz` (jede Ausgabe). `mag.sh` erkennt „fertig“ jetzt daran, dass `when`
  diesem Auftrag gehört und ein Ergebnis nennt (ein Stand, der selbst ein Ergebnis-Zweig ist, bringt dessen mag-log mit).
  Abgelegt 21:20: `ergebnis-work-zusammen-voll-203000-750-alle-212005-5593`. **Nicht erfasst von `alle`:** die eigenen Tests des Spiels und `tests/lowered`
  (kein `converted_` im Namen) – die zeigt erst wieder ein Lauf der ganzen Suite, sobald sie nicht mehr abbricht (Umbau des Testrahmens durch t1: jeder Test
  im eigenen Kindprozess, real-Durchgang immer).
- **Nächster Schritt:** Ergebnis von `alle` holen, die ganze Abweichungsliste je Gebiet in `stand/KOORDINATOR.md` schreiben und zuordnen; bereite Zweige (t1, t2, d)
  in `work-zusammen` einmischen, `voll` darauf; erst ein gültiger Lauf mit grüner Spiel-Suite (beide Durchgänge) und grüner Engine-Suite geht nach `work`.

### 16.13 Stand 21:35 – earcut-Fix in work-zusammen, d beendet, Neustart des Servers
- **`work-zusammen` = 8844531a** = 8874e79 + work-d 690ea4f (nur dieser Commit, per cherry-pick; `python3 tools/engine_api.py` danach ohne Unterschied, 818 Knoten,
  keine Teile von „Platz der Engine“): Gruppe 3 der Suite – `earcut.triangulate(xy, Count<2> vertex_count, …)`, `ev::numbers()` in Tür und Klasse, der
  Stellvertreter zeichnet Per · Zahl Werte auf. Bit für Bit noch nicht geprüft (erst die nächste Spiel-Suite). Laufen auf mag:
  `work-zusammen-engine-213241-11183` (Engine-Suite, weil earcut.h/Tür geändert), `work-zusammen-probe-213256-11283` (city_stream, destruction).
- **d ist beendet** (sein Lauf war zu Ende, nicht gestoppt). Zweig work-d = 2250736: Schritt 3 (Interpreter, 6310a9e) fertig, nur Syntax geprüft; Schritt 4
  (Klasse, 64cb433) gebaut, nie instanziiert; Schritt 5 (Schreibweise, check() 2250736) teilweise, Palette fehlt; Schritt 6/7 offen; keine Engine-Suite, kein
  CI-Lauf. Offene Frage von d: `list_append` auf einem Platz reserviert genau Länge + n – ein C++-insert wächst je nach Bibliothek anders (prüfen oder entscheiden).
  Nachfolger liest `uebergabe/d.md`, Abschnitt B „Stand der 7 Schritte“. **Noch nicht gestartet** (Quote; „Start“ von Chris deckt d).
- **~21:30 Neustart des Servers hinter der Sitzung:** t1, t2, p wurden abgebrochen (nicht von Chris gestoppt) und per SendMessage aus ihrem Verlauf fortgesetzt
  (das geht in diesem Fall – anders als bei den um 17:50 vom Nutzer gestoppten). Aufträge auf mag liefen weiter. Nächste Kontrolle 22:02 (Auswertung von „alle“).

### 16.14 Stand 22:30 – „Just decide and do it“: Auftragsart „vergleich“, k und d-Nachfolger gestartet, Entscheidungen E4–E6
- **Chris, 22:16: „Just decide and do it“** (auf die Fragen nach Quote / k / Nachfolger d). Der Koordinator entscheidet also selbst über Starts und Entwürfe;
  Maß bleibt die Quote (Stand unbekannt, zuletzt 36 % um 17:56) – nicht mehr als fünf Agenten zugleich, g1/g2/f1/f2 nicht einzeln (ihr Rest liegt bei k), c nach d, s5 nach p.
- **Der eigentliche Engpass war der Beweis-Takt:** eine Probe belegt keine Werte (nur dass der Test übersetzt) – Bit für Bit zeigte bisher erst ein voller Lauf (~45–60 min,
  nur der Koordinator). **Neu: `./mag.sh vergleich <ref> <quellen…>`** (lauf2.sh 310cfd47): Konverter über diese Quelltexte auf dem Level des letzten gültigen vollen
  Laufs (Datei `grundlage` im Zweig mag/auftrag nennt dessen Ergebnis-Zweig – **nach jedem gültigen vollen Lauf nachführen**), dann werden nur ihre Tests gebaut und
  laufen jeder für sich, gegen den Stellvertreter und gegen die Engine selbst, je höchstens 20 min. Ergebnis wie bei „alle“. Regel für alle Agenten
  (PAKET_COMMON3.md, Nachtrag 22:25): ein Fix an einer Abweichung ist erst belegt, wenn `vergleich` in beiden Durchgängen keine Abweichung mehr nennt.
  Erster Auftrag: `work-zusammen-vergleich-222045-20677` (destruction, city_stream auf 8844531a – prüft ds earcut-Fix Bit für Bit; noch ungeprüft, ob die Auftragsart
  selbst sauber durchläuft: beim ersten Ergebnis ansehen).
- **„alle“ erkennt Abbrüche jetzt an der fehlenden Summenzeile** („N tests, M checks“; ASan endet mit 1 wie ein Fehlschlag) und begrenzt jeden Lauf auf 20 min (Hänger →
  „ZEIT“). Der Lauf `…-alle-212005-5593` (alte Fassung, ohne Zeitgrenze) war um 22:17 nach 57 min noch nicht fertig – mag war überbucht (zehn Aufträge, ~100 Threads
  auf 56); hängt er, neu ablegen (neue Fassung). **mag-Regel:** keine Probe über alle Quelltexte mehr, höchstens ein eigener Auftrag je Agent.
- **Gestartet 22:27 (Opus 5.5, mittlere Stufe):** **k** – neuer Auftrag „Regeln des Konverters, die die Spiel-Suite rot machen“ (Gruppen 4–7, danach die Liste aus
  „alle“; PAKET3_k.md Nachtrag 22:25); **d, Nachfolger 3** – Platz der Engine beweisen (instanziieren, Engine-Tests, mag, CI) und Schritte 5–7 (PAKET3_d.md Nachtrag 22:25).
  Laufen weiter: t1, t2, p. IDs in /home/claude/agenten.txt.
- **Entscheidungen E4–E6 für p (21:50, PAKET3_p.md):** ganzzahlige Formel `[](Int s) -> Int` JA (Eigenschaft steht bei der Formel-Funktion; Beweis drei Stufen an den
  Rändern, Fixpunkt); Weitergabe beim Anhängen JA; `at_known` zurückgestellt; Aufrufweg an einen Actor darf p ändern (Text/Bedeutung unverändert); Zeit-Test ohne
  Ausnahmeliste, bis dahin Bericht. Gemessen von p: Ganzzahl-Fälle 10–18× langsamer als das C++, Ursache Float-Grenze der Formel (Projektion 25,1 → 2,8 ns, C++ 1,85 ns).

### 16.15 Stand 22:35 – mag-Warteschlange mit Budget und Abbruch, p Teil 1 in work-zusammen, Quote 42 %
- **Quote 22:31: 42 %** (17:56: 36 %). Fünf Agenten laufen: t1, t2, k, d (Nachfolger 3), p (Nachfolger, 22:33). Bei diesem Verbrauch bleiben mit fünf Agenten grob elf Stunden.
- **mag war überbucht, weil die Schleife nach jedem Ersetzen (neue lauf2.sh) ihre laufenden Aufträge nicht mehr zählte** – nach drei Fassungen an einem Abend ~120 Threads
  auf 56. **lauf2.sh 43cc7e0b:** jeder angenommene Auftrag steht in `.lauf/<name>` (Bedarf, Nummer seiner Schale; übersteht das Ersetzen), Aufträge älterer Fassungen
  zählen über ihre Prozesse; Budget `CAP = N·5/4` (70): voll 56, alle 37, vergleich 14, sonst 9; volle Läufe zuerst, dann vergleich/alle, dann der Rest; der erste,
  der nicht passt, hält die nach ihm auf. **Abbruch:** Datei `abbruch/<name>` auf mag/auftrag (`./mag.sh abbrechen <name> [tests]`): jeder Prozess im Ordner des
  Auftrags endet (seine Schale, git, xz, tar nicht – er schreibt sein Protokoll, pusht, räumt auf); „tests“ beendet nur Testläufe über 20 Minuten. Die Funktionen
  stehen in lauf2.sh zwischen `# >>> warteschlange` und `# <<<` und sind vor dem Push lokal mit Stellvertreter-Aufträgen durchgespielt (/tmp/wstest.sh-Muster:
  Funktionen herausschneiden, `run_job` ersetzen) – **eine fehlerhafte lauf2.sh beendet die Schleife auf mag, und nur Chris kann sie dort neu starten: vor jedem
  Push `bash -n` und die Probe.**
- **work-zusammen = e9eec025** = 8844531a + Merge work-p 7aacebfb (p Teil 1: Messwerkzeug, Straffung ohne Textänderung, Engine-Fall formula_bits_edges; Beleg
  `work-p-engine-212302-7164` rc=0, vom Koordinator nachgesehen). p (Vorgänger) endete 22:25 – gebaut, unbewiesen auf work-p: E2.1 (qr 14× → 5×, rng 13× → 1,7×),
  E2.2 (settings_escape 9,7× → 7,7×), Zeit-Test `--check 1.10` auf Bestzeiten (25 von 29 Fällen darüber: nur Bericht). Nachfolger: beweisen, dann +,−,×, Aufrufweg, Rest.
- **k:** G4 und G7 behoben (2a6ef5b3, Beleg läuft); Car_new ist ein Befund für t1 (unbestimmtes Mitglied im Quelltext); E3-C (Firmware: Felder fester Länge als
  `List`) ist ein Befund für k.
- **Offen bei Redaktionsschluss:** Ergebnis von „alle“ (Hänger beendet, sollte folgen), `work-zusammen-engine-213241-11183`, `work-zusammen-vergleich-222045-20677`
  (erster Lauf der Auftragsart – ansehen, ob sie sauber durchläuft), `work-k-vergleich-222911-25281`. Nächster voller Lauf, sobald t1 (Fehler 1+2, Testrahmen) und k
  (G4, G7) belegt und eingemischt sind; danach `grundlage` im Zweig mag/auftrag auf den neuen gültigen Lauf setzen.

### 16.16 Stand 23:05 – t1 und k eingemischt, voller Lauf abgelegt, Entscheidungen E7–E9
- **work-zusammen = 946303c2** = e9eec025 + Merge work-t1 a48ae637 (c6e4c1cf) + Merge work-k 20cb5259. Belege, vom Koordinator nachgesehen:
  `work-t1-vergleich-223114-27906` (city_stream, city_jobs: beide Durchgänge 11 von 12 ok, kein Abbruch, kein Leck), `work-k-vergleich-224717-8097`
  (flak_cell, Drone_step, CityStream_upload ohne Abweichung), `work-zusammen-engine-213241-11183` rc=0 (auf 8844531a), `work-p-engine-212302-7164` rc=0.
  **Voller Lauf: `work-zusammen-voll-230430-15353`** (abgelegt 23:04; er wartet, bis die laufenden Aufträge ins Budget passen).
- **Was der zusammengeführte Stand jetzt kann:** Testrahmen mit einem Kindprozess je Test und zeilenweisem stdout (kein Absturz hält die Suite an, der Durchgang
  gegen die Engine selbst läuft immer), Ortsangabe der ersten abweichenden Stelle, keine Lecks nach longjmp, Bereich hinter gezogenen Zeigern, Listen ohne
  Reserve-Platz, Quelltext auf genulltem Speicher; Count<Per> für Zeiger-Eingänge; i64/u64 in Formaten mit 64 Bit, ++/op= mit Rückschnitt auf den Typ,
  Zurückschreiben einer geänderten Kopie; p Teil 1.
- **Bekannt rot im abgelegten Lauf (Fix gebaut, Beleg läuft):** create_body (d: `inertia[6]` als 1 Zahl aufgezeichnet, Fix 5083222 auf work-d), CityStream_buildings_near
  (k: u64-Schlüssel einer Map als Int, fa3dc97d), Car_new (t1: genullter Speicher ist drin – Beleg `work-t1-vergleich-225216-10251`), Destruction_collapse/fracture (E9).
- **E7 (d):** `list_append` auf einem Platz der Engine wächst wie `insert` der Bibliothek – dieselbe Regel wie für Blueprint-Listen und den Quelltext.
- **E8 (t1):** der Quelltext läuft im Test auf genulltem Speicher; ein von C++ unbestimmt gelassenes Mitglied ist kein Vergleichswert.
- **E9 (k, t1) – berührt die Regel „jede Absenkung hat einen grünen Differenztest“, darum für Chris sichtbar:** Wo C++ eine Reihenfolge offen lässt und das Ergebnis
  davon abhängt (destruction.cpp:191/194: zwei Argumente eines Aufrufs ziehen aus demselben rng), gilt die Bedeutung des Compilers, der das Spiel baut (clang über
  emscripten: von links – wie das Original in JS). Die Quelle wird gesenkt (Werte in dieser Reihenfolge benannt), der Differenztest bleibt Pflicht und muss grün
  sein – gebaut mit clang; t1 stellt run_tests.sh allgemein um (tests/lowered baut mit clang, keine Ausnahme je Test). Unter GCC kann dieser Differenztest nicht
  grün sein: dort hat die heutige Quelle ein anderes Ergebnis als das Spiel. Lehnt Chris das ab, bleibt nur: die zwei Funktionen warten („C++ leaves open“).
- **Sperre für `work`:** 12e3da24 (k) ist ein Ausweichen im Konverter um einen use-after-free von `text_format` in der Klasse (Befund für p, vorgezogen). Erst wenn
  ps Fix in work-zusammen ist und k 12e3da24 zurückgenommen hat, darf ein grüner voller Lauf nach `work`.
- **Einzelläufe auf mag schreiben seit lauf2.sh febe6fc8 über ein Terminal** (script): LeakSanitizer beendet den Prozess mit _exit, bevor stdout geleert ist – vorher
  fehlte dann das Ende der Ausgabe. Kategorien in alle.txt: ok / FAIL / ABBRUCH / LECK / ZEIT. Der Lauf „alle“ auf dem alten Stand ist abgebrochen (zweimal):
  der volle Lauf auf dem neuen Stand zeigt mehr.
- Zweiter Neustart des Servers hinter der Sitzung um 22:43 (wie 21:30, jeweils zusammen mit einer Unterbrechung der Antwort): alle fünf Agenten per SendMessage an ihre
  ID fortgesetzt. Ein beim Neustart laufender Auftrag kann seine Startmeldung auf mag verlieren (`work-k-vergleich-224717` erschien als „wartet“ und lieferte doch).

### 16.17 Stand 23:35 – t2 eingemischt, voller Lauf neu, Paket g, Entscheidungen E10–E13
- **work-zusammen = 5a445af8** = 946303c2 + Merge work-d 50832229 (Array-Parameter fester Länge, aec5ded8) + Merge work-t2 19b96a95 (b41427f5) + Merge work-k
  fa3dc97d. **Voller Lauf: `work-zusammen-voll-232247-24031`, Start 23:23:02** (der von 23:13 auf aec5ded8 lief 8 min und wurde abgebrochen: ohne t2 hätte er die
  Game-Dateien nicht gezeigt). Budget der Warteschlange jetzt über die Datei `cap` im Zweig mag/auftrag (84).
- **t2 ist beendet** (Lauf zu Ende): rund 100 der 118 wartenden Funktionen haben ihren Test (Namensraum je Quelldatei, enum ohne Namen, Konstanten-Anweisung nur
  einmal, take(Span), scan_c). Sein Vergleich über sieben Game-Quelltexte (`work-t2-vergleich-223555-30482`): 50 ok / 7 FAIL / 2 ABBRUCH – echte Abweichungen, die
  vorher kein Test erreichte. Reste (lamps Field::Spot ×3, Graph, Beleg scan_c) → t1.
- **Agenten (fünf):** t1 (Testrahmen/Test-Schreiber; jetzt: Suite auf clang), k (Regeln, Nicht-Game-Dateien), **g** (neu 23:27: Regeln, Game-Dateien – PAKET3_g.md,
  Zweig work-g1), d (Nachfolger 3: Stellvertreter-Befunde, dann Platz der Engine), p (Nachfolger: text_format-Fix, Leistung).
- **E10 (p):** Wert an einem Eingang „Liste fester Länge“ als `Fixed<T, N>{…}` (wie bei Feldern). **E11 (t2):** `-Wno-missing-field-initializers` im Test ist keine
  Toleranz. **E12 (t2):** ein `logic::Graph` in einem Ergebnis wird über seine Serialisierung verglichen; bis dahin warten vehicle_actor und audio_init ehrlich.
- **E13 – ändert den Beweis und ist darum für Chris sichtbar:** Die ganze Spiel-Suite baut mit **clang**, dem Compiler des Spiels (bisher GCC). Anlass: zwei Stellen, an
  denen C++ die Reihenfolge von Argumenten offen lässt (destruction.cpp:191/194, game.h:170) – GCC rechnet von rechts, clang von links wie das Original in JS und
  wie der Blueprint. Verglichen wird damit gegen die Bedeutung, die das Spiel wirklich hat; keine Absenkung je Stelle, kein Differenztest, der unter GCC nicht grün
  sein kann. Der Absenkungs-Teil von E9 ist damit überholt (k nimmt 90ecf01f zurück). Offen: ob clang-18 mit ASan/UBSan auf mag linkt (t1 klärt es als Erstes;
  auf mag wird nichts installiert) – wenn nicht, bleibt GCC und E9 gilt wieder (Absenkung + Differenztest unter clang als eigenes Programm).
- **Nächste Schritte:** Zwischenstand des vollen Laufs (~23:50: convert rc, Zahlen; `grundlage` nachführen), Spiel-Suite (~00:20) → Liste (t1); Belegtes einmischen
  (g c4d4544b, p 25a7318f text_format → danach k 12e3da24 zurück, d Stellvertreter-Block, t1 clang); nächster voller Lauf mit clang.

### 16.18 Stand 00:55 (19.11.) – die Konvertierung des ganzen Spiels pendelt; clang ist drin; Agentenwechsel
- **Kein gültiger voller Lauf seit 20:53.** `work-zusammen-voll-232247-24031` (5a445af8): `convert rc=1` nach 27 min – ab Runde 9 geben 30 Game-Einheiten Runde für
  Runde, was sie vorher gaben, bis zur Sicherung in Runde 16. Ursache (g, aus zwei Diagnose-Läufen `work-zusammen-konvert-235338-1934`, `work-g1-konvert-002324-11083`):
  wartende Funktionen, deren GRUND Runde um Runde wechselt – `Game::explosion` wartet auf `look_blast()` „is not written“ und umgekehrt, dazu 15 Aufrufer in 11
  Teilen (combat_hud mit seinem Test-Fehler fiel schon in Runde 10, vor dem Kreis). Eine kleine Probe (game_events + combat_rules + game) konvergiert – es zeigt sich
  erst im ganzen Programm. **Abhilfe von g (480af66b, verallgemeinert 082acc7d):** was im Kreis wechselt (geschrieben/wartet oder der Grund) und dort wartet, wartet
  von da an, Grund = der erste im Kreis + „(it went round with <Blueprint>)“; deterministisch, Läufe ohne Kreis unberührt. Dritter Versuch läuft:
  `work-g1-konvert-005330-16535` auf 2a5d6f77 (enthält work-zusammen 9f10aa88), Ergebnis ~01:20. **Ist er gültig: g's Commits einmischen, `voll` ablegen.**
  Offen danach bei g: die Wurzel (warum zwei Funktionen, die einander rufen, nicht zusammen geschrieben werden; Test-Stücke anderer Dateien in `<datei>_local`).
- **work-zusammen = 9f10aa88** = 5a445af8 + g c4d4544b (Schleifengrenze, die die Engine fragt) + k c32f607b (Diagnose im Ablauf) + Merge work-t1 87e3d741
  (**E13: die Spiel-Suite baut mit clang**, CI auf clang, SC_ASAN, Index-Prüfung; 310c6e10) + Merge work-t1 694d04c1 (der Konverter prüft das Übersetzen eines
  Tests mit dem Compiler und den Schaltern der Suite, ohne -Werror; Rückfall aus 13464b5f behoben). Belege: `work-t1-vergleich-000108-5748` (clang 18.1.3 linkt
  auf mag mit ASan/UBSan; 43 von 44 ok in beiden Durchgängen; destruction ohne Absenkung gleich), `work-t1-vergleich-002703-13260` (keine wartende Funktion in
  controls, combat_rules, net_session, city_jobs), `work-g1-vergleich-232903-26675`.
- **E13 ist damit vollzogen:** verglichen wird gegen die Bedeutung, die der Compiler des Spiels der Quelle gibt. Erledigt sind dadurch alle Abweichungen, die nur an
  der Reihenfolge der Argumente hingen (Destruction_collapse/fracture/blast, eject, state_of). ks Absenkung von destruction.cpp (90ecf01f, nur auf work-k) ist nie
  gemergt worden und entfällt. **E14:** `set_pcm` zählt Samples (`Count<1> samples`), die Engine rechnet frames = samples / channels.
- **Agenten (vier laufen):** t1 (Testrahmen/Test-Schreiber; Reste von t2: lamps Field::Spot, scan_c-Beleg, Graph E12), g (Game-Dateien, jetzt: Pendeln),
  d Nachfolger 4 (seit 00:02: Count im Typ statt Namensregel – `work-d-count`; `T m[N]` in Antwort/Form; set_pcm – Belege laufen), p Nachfolger (text_format-Fix
  25a7318f gebaut, Vergleich `work-p-vergleich-230429-15316` 12 von 12 ok in beiden Durchgängen, ein Engine-Lauf rc=1 – Ursache bei p in Arbeit; E3-A gebaut).
  **Beendet:** t2 (23:20), k (23:55; Nachfolger, sobald ein gültiger Lauf die Liste seiner Dateien gibt), d Nachfolger 3 (23:58).
- **Platz der Engine (work-d ddb54b72):** Schritte 3–6 gebaut, Engine-Suite grün (`work-d-engine-223942-31317`), CI e2e rot allein wegen des alten Levels, Schritt 7
  gebaut und unbelegt. **Wird erst nach dem ersten grünen Stand in `work` gemergt.**
- **Sperren für `work` unverändert:** 12e3da24 (Ausweichen von k um den use-after-free in text_format) zurücknehmen, sobald ps Fix gemergt ist – k ist beendet, das
  macht der Koordinator mit `git revert 12e3da24` in /home/claude/w-zus.
- **mag:** Auftragsarten `konvert <ref> <kerne>` (nur Konvertierung, ~27 min bei 37 Kernen – der schnelle Test, ob das ganze Programm konvergiert), `vergleich`,
  `alle`, `voll`; Budget in der Datei `cap` (84); `abbrechen`. `grundlage` für `vergleich` ist weiter der Lauf 203000 (der letzte gültige).

### 16.19 Stand 02:00 (19.11.) – der Ablauf endet wieder; Lesefehler behoben; neuer voller Lauf; nur d4 läuft
- **`work-zusammen-voll-012215-4354` (2c7667bc):** die Sicherung von g trägt am ganzen Programm – Fixpunkt nach Runde 18 (Konvertierung 32 min), 16 Funktionen
  „waits for good … (it went round with …)“. Dann `convert rc=1` am Leser: „Game.cpp: line 8106, column 18: kern::to<Type>(value) or kern::formula(...) expected, not
  kern::math“ (`std::atoi`, game_toss.cpp:245, als `kern::math::wrap32s(int64_low(...))` ohne Formel gesetzt). Fix g 96d52bef, lokal mit `lesen.py` an den Texten des
  Laufs belegt (zwei Stellen berichtigt: alle 156 Texte gelesen). **work-zusammen = 84696123; `work-zusammen-voll-015912-10300` läuft seit 01:59** (Konvertierung
  bis ~02:32, Suiten danach). Zählung im Protokoll von 012215: 103 Zeilen „cannot compare it yet“ (203000: 118), 16 „waits for good“.
- **Seit 9f10aa88 in work-zusammen:** t1 8e2474a9 (2dacbbb5), p 85d60a1d (f67af15b), Rücknahme 12e3da24 (e3901213), g 7bd9ac5d (2c7667bc), g 96d52bef (84696123).
  Die Sperre „12e3da24“ für `work` ist damit weg; ihr Beleg (flak_cell / combat_rules_cpp unter ASan ohne das Ausweichen) kommt mit diesem Lauf.
- **Beendet (Läufe zu Ende, alles Gebaute gemergt):** t1 (01:05), p-Nachfolger (01:15), g (01:58). **Es läuft nur d Nachfolger 4** (Count im Typ, `T m[N]`, set_pcm, Maker
  für Client/Server). **Nachfolger starten, sobald die Suite eines gültigen Laufs die Arbeitsliste gibt:** t1 (Auswertung, look_apply_tier, Graph E12), k (Regeln
  Nicht-Game-Dateien), g (Regeln Game-Dateien: combat_hud/g_fun, kReliable, kOlive – Stücke einer Datei über einen dritten Teil; Game-Abweichungen der Suite),
  p (Leistung: zwei Entwürfe angekündigt) – p erst nach dem ersten grünen Stand. Übergaben: uebergabe/{t1,k,g,p,d}.md.
- **Neue Entwurfspunkte der Stufe 6 (neben Platz der Engine, Callbacks/Events, build_level):** **Rekursion** – die Sprache verbietet sie (Leser, notation.cpp:686ff.;
  Wiedereintritt nur über Events, höchstens 64 tief); im Spiel 16 Funktionen in Rekursion über Teile (Kern: Game::explosion ↔ blast_cars/wreck_car/combat_blast) +
  6 Aufrufer. Vorschlag g: Rekursion mit derselben Grenze 64 zulassen (Engine-Fall in allen Stufen). **Graph als Wert** (E12, 2 Funktionen).
- **Vor dem Abschluss zu lösen (CI):** der Job native-spawncity läuft auf `work` wie auf den Zweigen in seine 45-Minuten-Grenze (rot).
- **Leistung (p, Kopf 85d60a1d):** Zeit-Test `--check 1.10`: 25 von 29 Fällen darüber; größte Reste mulm 7,8×, settings_escape 7×, sha256 6,7×, from_hex 5,1×, qr 5×.
  Nächste Entwürfe laut uebergabe/p.md: +,−,× und Vergleich an der Wurzel einer Int-Formel; Struktur-Argumente bei Call on Actor ohne logic::Value.

### 16.20 Stand 02:45 (19.11.) – GÜLTIGER LAUF: `work-zusammen-voll-020121-10802` (4748a503), convert rc=0
- **Konvertierung 02:01:37 → 02:32:34 (18 Runden), `convert rc=0`.** „1637 of 1844 functions of the program are written as Blueprints (19218 of 30978 lines), and 28 of
  the engine's headers that they call: 156 Blueprints – 70 of source files, 1 of states, 85 of headers and of shapes“; „1594 functions of the Blueprints are
  compared with their source, bit for bit …; 5 give nothing“. Zum Vergleich 203000 (20:53): 1632 / 18813 / 1589. **`grundlage` im Zweig mag/auftrag zeigt jetzt auf
  diesen Lauf.** Die Suiten rechneten bei Redaktionsschluss (Spiel-Suite mit clang, beide Durchgänge im Testrahmen von t1; Engine-Suite).
- **work-zusammen = 4748a503** = 84696123 + work-d-count (E14 set_pcm, Count im Typ), work-d-array (`T m[N]`), work-d-maker (Objekte der Engine für sechs Klassen).
- **Die nächste große Hebelstelle (rund 116 wartende Funktionen, fast alle Game):** der Test übersetzt im ganzen Programm nicht, wenige Familien – `kReliable`/
  `kUnreliable` aus net_session_cpp_local (~60), `World`/`arg` (10), `draw_CarSpec` fehlt (16), `g_fun`/`multiplier` (6), „';' after top level declarator“ (6),
  `kOlive` (5), `kScorches` (2), Graph (E12). Im Kleinen (t2: sieben Dateien; g: drei) tritt es nicht auf. → Nachfolger t1 („t1b“, gestartet 02:47).
  Dazu 16 Funktionen „waits for good“ (Rekursion über Teile; Entwurfspunkt Stufe 6) und die 207 nicht geschriebenen (1844 − 1637: brauchen Platz der Engine,
  Callbacks/Events, build_level – Stufe 6).
- **Agenten:** d4 beim dritten Neustart des Servers (~02:28) abgebrochen und NICHT fortgesetzt (seine Fixe sind gemergt; Engine-Suite des Makers
  `work-d-maker-engine-015441-9451` abholen). Laufend: t1b. Nach der Auswertung der Suite: Nachfolger für die Regeln (k: Nicht-Game, g: Game).
- **Nächster Schritt:** Suite-Ergebnis (suite rc, engine rc in `mag/<name>/when`; suite.log) auswerten – beide Durchgänge; ist alles grün (unwahrscheinlich beim
  ersten Mal): `work-zusammen` + Commit „Level und Tests“ des Laufs nach `work` (Abschnitt 10), CI anstoßen. Sonst Liste nach Gebieten, Agenten, nächster Lauf.

### 16.21 Stand 04:45 (19.11.) – erste vollständige Testliste; zwei Fehler des Rahmens bzw. der Engine vom Koordinator behoben; drei Nachfolger gestartet
- **Suiten des gültigen Laufs 020121 (4748a503):** `engine rc=0` (02:37). `suite rc=1` (03:19) – in beiden Durchgängen nach ~26 Tests abgeschnitten: **Zählfehler im
  Testrahmen** (das Kind erbte die Summen der Tests vor ihm: Verdopplung je Test, Überlauf in int, UBSan beendete den Durchgang; ein Fehlschlag ließ alle folgenden
  Tests als FAIL erscheinen). **Fix Koordinator: work-zusammen d15559ba** (test_main.cpp: das Kind beginnt mit eigenen Zählern; Kleinstfall alt/neu).
- **Erste vollständige Liste: `work-zusammen-suite-alle-032626-8142`** (Zweig `work-zusammen-suite` = Ergebnis-Zweig des Laufs + Fix; jeder der 150 Tests einzeln,
  41 min): **Stellvertreter 113 ok / 35 FAIL / 4 ZEIT; Engine selbst 108 ok / 39 FAIL / 9 LECK / 3 ZEIT.** Arbeitsliste nach Gebieten: stand/KOORDINATOR.md, Eintrag 04:40
  (A–E); genaue Auswertung: stand/t1.md „SUITE 020121“.
- **33 der FAIL (alle Game-Teile) und die ZEIT-Fälle hatten EINE Ursache: Game lief im Interpreter statt als Klasse.** tools/blueprint_cpp: „the Blueprint holds the
  function person_hit::hit as its library had it before“. Mit einer Diagnose-Ausgabe lokal gefunden: `logic::cpp_names` (Level-Lader und Werkzeug) benannte den
  Eingang `pad` der GEHALTENEN Bibliotheks-Funktion in `pad2` um, weil Game eine Variable `pad` hat – `link()` hielt den Blueprint dann für veraltet.
  **Fix Koordinator: work-zusammen 12438d1c** (notation.cpp: gehaltene Funktionen behalten ihre Eingangsnamen, wie der Leser es schon hält; Engine-Fall in
  kern/tests/test_main.cpp). Lokal belegt: 173 von 173 Klassen (vorher 172, Game im Interpreter), Klassentext übersetzt mit clang. **Laufen auf mag:**
  `work-zusammen-suite-alle-043356-8771` (dieselben Tests mit dem Fix) und `work-zusammen-engine-043403-8808` (Engine-Suite auf 12438d1c).
  (Nebenbefund: tools/blueprint_cpp gibt mit g++ und clang++ gebaut byteweise denselben Text – der Schlüssel eines Graphen hängt nicht am Compiler.)
- **Übrige echte Abweichungen sind wenige:** Mat4 `.m[15]` (Mat4_perspective 300/300, projection_matrix 62/300), SoundFx `.l.voice` (u64 über Float64), sechs
  Game-Funktionen (draw_events, gaps_guided, number_at, rocket_smoke_emit, spawn_squad, spawn_tank), Session_update 1/56, game_details `.Result`; Abbrüche mvt/life
  (Test); gegen die Engine 9 LECK, ASan in kern::weld, 14 Funktionen „0 compared“. Dazu 271 cost-Zeilen (Leistung/Speicher, Paket p später).
- **t1b (02:41–04:30) beendet:** die Familien der ~116 wartenden Funktionen sind gebaut (work-t1 c5b8ab99; im Lauf `work-t1-konvert-034529-8942` 11 statt 87
  Wartende) – **noch nicht gemergt**, weil sie eine Regel des Konverters sichtbar machen: „Game: two parts have a constant kQuiet that is not the same“
  (game_projsync.cpp:25 = 0.6, game_toss.cpp:34 = 0.5) → convert rc=1. **Reihenfolge: k behebt kQuiet → `konvert` rc=0 → work-t1 + work-k einmischen → `voll`.**
- **Gestartet 04:42 (Opus 5.5, mittlere Stufe):** **k** (Nachfolger; Regeln für alle Dateien: B0 kQuiet zuerst, dann die Abweichungen B1–B3; Zweig work-k),
  **t1** (Nachfolger 4: get_u8 ×9, mvt, life, request_level, look_apply_tier; Auswertung des Laufs 043356; Zweig work-t1), **d** (Nachfolger 5: Lecks, weld,
  „0 compared“ gegen die Engine; Zweig work-d). IDs: /home/claude/agenten.txt. p (Leistung) erst nach dem ersten grünen Stand.
- Fünf Neustarts des Servers hinter der Sitzung bisher (21:30, 22:43, 02:28, 03:10, 03:52) – laufende Agenten werden dabei abgebrochen und per SendMessage an ihre
  ID fortgesetzt (zuletzt geschah das teils automatisch); mag ist davon unberührt.

### 16.22 Stand 06:00 (19.11.) – 138 von 150 Tests ok, 1695 von 1844 Funktionen geschrieben; voller Lauf 055004 rechnet
- **Engine-Fix cpp_names (12438d1c) bewiesen:** `work-zusammen-engine-043403-8808` rc=0; Kontrolllauf `work-zusammen-suite-alle-043356-8771` (Level und Tests des
  Laufs 020121 + beide Fixe des Koordinators): **Stellvertreter 138 ok / 12 FAIL / 1 ZEIT; Engine selbst 127 ok / 23 FAIL / 11 LECK / 1 ZEIT** (vorher 113/35/4 und
  108/39/9 LECK/3). Auswertung: stand/t1.md „SUITE 020121b“.
- **work-zusammen = fe83683e** = 12438d1c + Merge work-k 8b8c0e10 (kQuiet-Regel: dateieigene Konstanten gleichen Namens verschiedener Teile heißen `<datei>_<name>`;
  enthält t1b c5b8ab99 – Familien kReliable, CarSpec/World, g_fun; 247dab47) + Merge work-t1 c13e917c (mvt, life, game_look/equipment, LightPool-Stellvertreter,
  get_u8 – letzteres nur im ganzen Programm belegbar). Beleg der Konvertierung: `work-k-konvert-044336-12397` convert rc=0 – **1695 von 1844 Funktionen geschrieben
  (21450 von 30978 Zeilen), 1652 verglichen, 20 „cannot compare it yet“, 18 „waits for good“** (020121: 1637 / 19218 / 1594 / ~116 / 16).
- **Voller Lauf: `work-zusammen-voll-055004-6649` auf fe83683e, Start 05:50:35** (Konvertierung bis ~06:25, Suiten danach – die erste ganze Spiel-Suite am Stück mit
  dem berichtigten Testrahmen, mit eigenen Tests, tests/lowered und native). Nach convert rc=0: `grundlage` im Zweig mag/auftrag auf diesen Lauf.
  Er wird nicht grün sein – bekannt offen:
  · **k (läuft, work-k):** Mat4 `.m[15]` (Ursache laut k im Test: memset auf f32-Liste; cdcd2f51 unbelegt), SoundFx `.l.voice` (5d804704: Alias einer 64-Bit-Zahl
    bleibt Int64, ändert engine_api – unbelegt, Beleg `work-k-vergleich-054057-841`), draw_events, rocket_smoke_emit, gaps_guided, spawn_squad/spawn_tank (`rng_.s`),
    mp_hud_json; number_at 8/300 (from > s.size(): was C++ offen lässt → Test).
  · **d (läuft, work-d / kurze Zweige work-d-…):** 11 LECK aus engine_make_default (im eigenen Lauf `work-d-belege2-vergleich-052701-29611` schon 0 LECK, noch nicht
    gemeldet), ASan in kern::weld (city_stream gegen die Engine), 14 × „only 0 calls could be compared“, Maker für LightPool (look_apply_tier), key.
  · **t1 (beendet 05:52; Nachfolger nach dem Lauf):** get_u8 im vollen Lauf prüfen; Graph als Wert/request_level (E12); ein Test, der gegen die Engine einmal FAIL
    und einmal ok zeigt (game_details_cpp); ZEIT converted_Game__game (>20 min in beiden Durchgängen – hängt er oder rechnet er nur lang?).
  · 16–18 Funktionen in Rekursion über Teile (Entwurfspunkt Stufe 6), ~149 nicht geschriebene (Platz der Engine, Callbacks, Events – Stufe 6), 271 cost-Zeilen und
    der Zeit-Test (Paket p) – nach dem ersten grünen Stand.
- **Was für den ersten grünen Stand in `work` fehlt:** die Punkte von k und d oben belegt und gemergt, dann ein voller Lauf mit `convert rc=0`, `suite rc=0`
  (beide Durchgänge), `engine rc=0` → work-zusammen + „Level und Tests“ nach `work`, CI (der Job native-spawncity läuft bisher in seine 45-Minuten-Grenze).

