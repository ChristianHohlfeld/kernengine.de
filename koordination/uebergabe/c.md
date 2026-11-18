# Paket c – ENTWURF „die Engine ruft eine Funktion eines Blueprints“ (18.11.2026, nicht gebaut)

Stand: nur Entwurf. Worktree /home/claude/w-c, Zweig work-c = origin/work 90b5ab8, keine Commits.
Gebaut wird erst nach Bestätigung durch main.

## 1. Die betroffenen Stellen

Engine (Header, aus denen tools/engine_api.py die Knoten macht):
- `kern/geo/mesh_data.h` `HeightSampler { f64 (*fn)(void*, f64, f64); void* user; }`: heute ein Handle (make/release-Knoten).
  Er geht in `RoadBuilder::begin(…, HeightSampler)` (die Engine **behält** ihn im Objekt und ruft ihn später in `step()`)
  und in `build_terrain_tile(…, HeightSampler, …)` (Aufruf nur **während** des Knotens).
- `kern/geo/roads.h` `RoadOptions::deformed` (`std::function<bool(f64,f64,f64)>`): macht RoadOptions ganz zum Handle
  (deshalb „a member of Int is set“ in begin_roads). RoadBuilder behält es und ruft es in `step()`.
- `kern/core/jobs.h` `jobs::Fn = void(*)(Span<const u8>, std::vector<u8>&)`, `jobs::define(kind, fn)`: einer der 20 von
  `engine_api.py --list` („a pointer“). Die übrigen 19 sind **keine** Rückrufe (variadisch: fail/log; Zeiger, durch die
  geschrieben wird; void*-Puffer in net) – nicht Paket c.
- Engine-intern, kein Rückruf des Programms: `Runtime::instances` (Feld `blueprints` als std::function, Einheit
  engine_logic_logic) – Runtime ist ein Objekt der Engine, gehört zu d („Plätze der Engine“). `kern/src/api/geo_api.cpp`
  `host_heights()` ist Engine-intern (kein Knoten).

Spiel:
- `terrain.cpp` `Terrain::sampler` (7) – „HeightSampler given back by value“.
- `city_stream.cpp` `CityStream::begin_roads` (17) – Lambda `[this]` in `ro.deformed`, `terrain_.sampler()`;
  daran hängen `finish_features` (61), `relay` (13), `work` (110, wartet außerdem: ruft sich selbst), `update` (85,
  wartet außerdem an request_around). `fail`/`go_offline` warten an `remove` (list erase) – nicht an Rückrufen.
- `city_jobs.cpp` `KERN_JOB(kTile, run_tile)`, `KERN_JOB(kBuildings, run_buildings)`; `terrain.cpp`
  `KERN_JOB(Terrain::kDemJob, dem_grid_job)` – Anmeldung der Job-Funktionen (statisch). Die Funktionen selbst warten an
  anderem (GeoProjector::init/fail, Referenz).
- `game.cpp` `register_node` (18) – das Programm meldet einen eigenen Knoten an (`NodeDef::accepts/run` als Lambdas, die
  `game().fixed_step` rufen). **Kein Rückrufwert**: Im Blueprint ist das ein Handler des Events, auf das der Knoten hört
  (`d.event = "logic.tick"` → `KERN_EVENT(onN, event_tick)` im Blueprint Game, der `fixed_step(1/kStepHz)` ruft). Ein
  Blueprint fügt der Bibliothek keine Knoten hinzu. Antwort (b) in game.cpp (Absenkung, nicht meine Datei) oder eine
  Regel des Konverters „NodeDef-Event, dessen run nur eine Funktion des Programms ruft, accepts immer true = Handler“ –
  **Entscheidung von main nötig**.
- Keine Rückrufe (zur Abgrenzung): `rules.cpp actor_graph`, `Game::build_level` (Graph/Level als lokale Objekte der
  Engine), `Game::vr_comfort` (TextureDesc/MeshLook/MeshData), alle lokalen Lambdas `[&]` (Paket „Lambdas“).

## 2. Der Wert

**`Callback<R(A…)>`** – „eine Funktion eines Blueprints“ als Wert fester Gestalt: *welche* Funktion (zur Übersetzungszeit
fest) + höchstens **ein gebundener Platz** (der `void* user` / das `[this]`). Nur die Engine ruft ihn. In der Sprache gibt
es keinen Aufruf eines Callback, keinen Vergleich, keine Umwandlung (auch nicht zu Text), keinen Schlüssel einer Map,
keine Liste davon. Erlaubt als Typ eines Pins, einer Variablen, eines Felds einer Struktur (RoadOptions), eines Ein-/
Ausgangs einer Funktion, einer lokalen Variablen.

**Was er nennen darf (Regel, check):** eine Funktion, die **nichts vom Blueprint braucht** – rechnet nur (Regeln von pure),
liest keine Variable des Blueprints, ruft keinen Knoten der Engine/des Spiels, kein Print/Game Time/Random, ruft nur
ebensolche Funktionen. Ihre erste Eingabe darf eine **const-Referenz** sein: das ist der gebundene Platz (Terrain bei
`sampler`, CityStream bei `deformed`); die übrigen Eingänge/Ausgänge sind genau die Signatur. Eine Funktion mit
Ausgang-Referenz (`std::vector<u8>& output` bei jobs::Fn) nimmt sie als Referenz-Eingang.
Der gebundene Platz **bleibt, wo er ist**: auf seinem Weg kein Eintrag einer Liste/Map (Fixed ausgenommen) – auch über die
Referenz-Eingänge aller Aufrufer (dieselbe transitive Prüfung wie „gehaltener Eintrag“).

Begründung der Einschränkung: Damit ist der Wiedereintritt kein Thema (die Funktion ändert nichts, startet keinen
Handler, braucht die Welt nicht), die Reihenfolge beliebig (Engine darf ihn rufen, wann und wie oft sie will) und in der
Klasse kostet er genau, was das C++ kostet (s. u.). Rückrufe, die etwas ändern, warten mit Grund – im Spiel gibt es keinen.

## 3. Schreibweise, Knoten, Dokument

- Typ: `Callback<Float(Float, Float)>`, `Callback<Bool(Float, Float, Float)>`; Dokument `"type": "callback", "of":
  "float(float,float)"`; Editor „Callback of Float(Float, Float)“, ein Pin-Typ mit eigener Farbe.
- Knoten **Make Callback** (`func.callback`, Wert-Knoten, Palette „Functions“): Einstellung `function` (auch
  `Lib::fn`), und – wenn die Funktion einen Platz bindet – `bind.variable`/`bind.scope`/`bind.path` wie die
  Referenz-Eingänge von Call (ohne Index-Eingänge: kein Listeneintrag). Text:
  `callback<&Terrain_lib::height_at>(self)` bzw. ohne Platz `callback<&city_jobs::run_tile>()`.
- Die gemeinte Funktion steht in der Klasse als `KERN_STATIC` (= `static`, C++ prüft selbst, dass sie kein `this`
  braucht); `check()` meldet eine Funktion, die ein Callback nennt und etwas vom Blueprint braucht.
- Der Leser lehnt jeden Aufruf eines Callback ab (`a callback is called by the engine only`).

## 4. Jede Stufe

- **Klasse:** `kern::logic::Callback<R(A…)>` = `{R (*fn)(void*, A…); void* place;}` – fn ist ein Thunk
  `[](void* p, A… a) { return Cls::height_at(*static_cast<const Terrain*>(p), a…); }`. Zum `HeightSampler` wird er
  Feld für Feld (gleiche Gestalt: zwei Wörter, ein indirekter Aufruf – **dieselben Kosten wie heute fn + void***), zu
  `std::function` wie im Quelltext (aus einem Lambda mit einem Zeiger: keine Anforderung, small buffer), zu `jobs::Fn`
  ein Thunk ohne Platz. `engine_values::Direct<Callback<…>>` ↔ C++-Typ, geschrieben von engine_api.py.
- **Interpreter/JIT:** der Wert ist `{Funktion, Spot des Platzes (Instanz + Variable + Weg)}`. Die Tür
  (`engine_calls.gen.cpp`) baut den C++-Typ mit einem Trampolin, dessen `user` auf einen Eintrag der Laufzeit zeigt
  (`Runtime` hält ihn, solange die Instanz lebt); das Trampolin ruft `Runtime::call_static(fn, spot, args)` –
  synchron, auf einem eigenen Rahmen (Parameter/lokale Variablen wie bei `Runtime::fire` gesichert), liest den Platz,
  wie er gerade ist. Der Wiedereintritt während eines laufenden Engine-Knotens ist damit nur ein Lesen.
- **Bitgleich:** dieselbe Funktion, dieselben Eingänge, derselbe Platz – gelesen nie geändert; in jeder Stufe dasselbe
  Ergebnis, egal wann und wie oft die Engine ruft.
- **Stellvertreter (Test):** `EngineStandIn` ruft jeden Callback, den ein Knoten bekommt, einmal mit den Nullen seiner
  Eingänge (so wird der Weg in jeder Stufe verglichen); mit `SC_ENGINE=real` ruft die echte Engine.

## 5. Lebensdauer

Ein Callback mit Platz gilt, solange die Instanz lebt. Er darf nur gehen (a) in einen Engine-Aufruf, der ihn nur
während des Knotens ruft, oder (b) in ein Objekt der Engine, dessen Handle **dieselbe Instanz** hält – das gibt End Play
schon heute frei (umgekehrte Reihenfolge), bevor die Instanz geht. Danach kann die Engine ihn nicht mehr rufen; in
jeder Stufe gleich. Was die Tabelle sagt: ein Callback-Eingang einer Methode eines Objekts der Engine = „behalten“ (b),
einer freien Funktion = „nur während des Aufrufs“ (a), außer die Funktion behält ihn global (jobs::define) – dann ist
ein Platz verboten (nur Funktionen ohne Platz). Ein Callback ohne Platz gilt immer (die Funktion braucht nichts).

## 6. Threads und Jobs

- Synchron im Knoten auf dem Frame-Thread: HeightSampler, deformed – erlaubt (s. o.).
- Auf anderen Threads/Web-Workern: nur `jobs::Fn` – Funktion ohne Platz, braucht nichts vom Blueprint → threadfest.
  Klasse: echte Threads/Worker. Interpreter/JIT: der Worker hat das Level nicht → die Tür meldet die Art „nur im
  Frame“ an, `jobs::pump` rechnet sie auf dem Frame-Thread (jobs.h: „the result is the same“) – bitgleich, weil
  Bytes hinein, Bytes heraus, ohne Zustand.
- **Offen für main:** Die Anmeldung geschieht im Quelltext statisch (`KERN_JOB`), damit die zweite Instanz des Moduls
  (Worker) die Funktion kennt, ohne das Spiel zu starten. Im Blueprint wäre `engine_jobs_define(kind, callback)` ein
  Schritt (z. B. Begin Play) – im Worker läuft der nie. Vorschlag: Jobs als **zweiter Teil**; dafür eine Erklärung einer
  Bibliothek `KERN_JOB(kind, &Lib::fn);` (wie `KERN_CONST` eine Erklärung, kein Knoten; der Kleber meldet sie beim
  statischen Start an, die Runtime beim Laden). Bitte bestätigen oder Teil 2 zurückstellen.

## 7. Konverter (Paket c, eigene Funktionen)

- engine_api.py (eigene Funktion, klein): C++-Typ → Callback: (i) Struktur aus genau einem Funktionszeiger mit `void*`
  vorn und einem `void*` (HeightSampler), (ii) `std::function<R(A…)>`, (iii) Funktionszeiger ohne `void*` (jobs::Fn).
  Damit wird HeightSampler kein Handle mehr (make/release-Knoten entfallen), RoadOptions eine Struktur der Engine mit
  einem Callback-Feld. **Berührt Paket d** (engine_api.py, Einordnung Handle/Struktur).
- cpp_to_blueprint.py: ein Lambda/Funktionszeiger, der an einen Callback-Platz geht (Feld, Argument, `s.fn = …`), wird
  eine statische Funktion des Blueprints (das Lambda gehoben; `[this]` oder `user = this` → const-Referenz des Platzes);
  `s.user = const_cast<T*>(this); s.fn = …; return s;` → `return {callback<…>(self)};`.
- Keine Absenkung im Spiel nötig für sampler/begin_roads. register_node: s. Abschnitt 1 (Entscheidung).

## 8. Beweis (nach dem Bau)
Tier-Fall `callbacks` (logic_cases.h: ein Engine-Knoten, der einen Callback behält und später ruft; einer, der ihn
synchron ruft; Platz in Variable und über Referenz-Eingang), Fall mit jedem Knoten (test_main), Palette, Doku-Abschnitt
„Rückrufe der Engine“ in blueprint_cpp.md, native Fälle „Engine“; im Spiel Terrain::sampler, begin_roads (und was daran
hängt) geschrieben und verglichen, auch SC_ENGINE=real.
