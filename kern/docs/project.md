# Projekt, Level, Programm – wie ein Spiel auf Kern aufgebaut ist

Dieser Text beschreibt, wie man ein Spiel auf Kern anlegt, ohne ein bestehendes Spiel als Vorlage zu nehmen, und wie
die Engine es fährt (Stand 26.11.2026: alles hier ist gebaut und mit den genannten Tests belegt; was offen ist, steht
unter „Stand“ am Ende). Wo es weitergeht: die App-Laufzeit – Ton, Eingabe, Speichern, Widgets – in [app.md](app.md),
Akteure zur Laufzeit in [runtime_actors.md](runtime_actors.md), Blueprint und C++ in [blueprint_cpp.md](blueprint_cpp.md)
und [engine_object.md](engine_object.md), der Editor in [studio.md](studio.md) („Das Projekt“, „Play Project“).

## 1. Die Begriffe

| Begriff | Was es ist | Datei |
|---|---|---|
| **Projekt** | Das Spiel als Ganzes: seine Einstellungen, seine Levels, sein Start-Level und sein **Programm** (die Programm-Blueprints). Es gibt genau ein Projekt je Spiel. | `<name>.kproject` (`"format": "kern-project"`) |
| **Level** | Eine **Welt**: ihre Einstellungen (Schwerkraft, Schrittrate, Himmel …), ihre Akteure mit Komponenten und deren Blueprints, ihr eigener Level-Graph. Ein Projekt hat beliebig viele. | `<name>.klevel` (`"format": "kern-level"`) |
| **Programm-Blueprint** | Ein Blueprint, der im Projekt steht. Er gehört zu keiner Welt, sondern läuft in der **Programm-Laufzeit**, die einmal beim Start entsteht und bis zum Ende des Spiels bleibt. Zu ihm gehören die App, der Spielzustand, Menüs und alles, was über einen Levelwechsel hinaus besteht. | im Projekt |
| **Blueprint einer Welt** | Der Blueprint eines Akteurs eines Levels oder der Level-Graph. Er läuft in der Logik-Laufzeit **seiner** Welt (GameWorld) und endet mit ihr. | im Level |
| **Bibliothek** | Ein Blueprint mit `library = true`: nur Typen und Funktionen, keine Instanz. Er kann im Projekt oder in einem Level stehen. | beides |

Ordner (im Outliner) sind Ordnung im Editor und haben keine Bedeutung für die Engine. Was ein Blueprint ist, ergibt
sich daraus, **wo er steht**: im Projekt ist er Programm, im Level gehört er zur Welt.

## 2. Die Projekt-Datei

```json
{
  "format": "kern-project",
  "name": "Mein Spiel",
  "settings": {
    "renderer": { "shadows": true, "shadow": { … }, "bloom": { … }, "ao": { … }, "grade": true },
    "profiles": [ { "name": "mobile", "renderer": { "shadow": { … }, "ao": { … } }, "world": { "shadowDistance": 70 } } ],
    "heapReserveMb": 0,
    "loopBudget": 200000,
    "input": { "actions": [ { "name": "springen", "keys": ["space"], "pad": ["a"] } ], "axes": [ … ] }
  },
  "levels": ["levels/start.klevel", "levels/wald.klevel"],
  "startLevel": "levels/start.klevel",
  "blueprints": [
    { "name": "MeinSpiel", "logic": { … } },
    { "name": "Menue",     "logic": { … } },
    { "name": "Mathe",     "logic": { "library": true, … } }
  ]
}
```

- `settings` (`kern/app/project.h`, `ProjectSettings`) ist der **eine** Weg, den Renderer einzurichten (die App hat kein
  `configure`): `renderer`, die Geräteprofile `profiles` (je Name und Renderer – der des Projekts mit den Schlüsseln
  des Profils darüber; gewählt beim Start mit dem Startparameter `profile=<name>`, ein unbekannter Name ist ein Fehler
  im Log mit den vorhandenen Namen) und `heapReserveMb` (0–4095). Die Engine wendet sie vor dem Start des Renderers an;
  `kern::run` übergibt sie (4.). Das feste Telefon-Feld `"mobile"` früherer Dateien, ein Profil ohne Namen oder mit
  doppeltem Namen: Fehler mit Stelle. Ein Level hat keinen Abschnitt `"project"`. `loopBudget` ist die Schleifenwache
  der Programm-Logik (4.), `input` die Eingabe-Aktionen des Spiels (app.md 4.: ein Spiel fragt nach der Aktion, nie
  nach einer Taste).
- **Welt-Einstellungen je Gerät:** ein Profil kann `world` tragen – Schlüssel des Abschnitts `"world"` eines Levels
  (`WorldSettings`, etwa `shadowDistance`, `substeps`). Startet eine Welt des Projekts ein Level (`GameWorld::start`,
  `start_level`), gilt diese **Reihenfolge**: die Welt-Einstellungen des Levels, darüber die Schlüssel des gewählten
  Profils. Gewählt ist es bei einer Welt des Programms durch den Startparameter `profile=<name>` (`Engine::profile()`): eine Welt, die
  eine App in C++ aus ihrer Engine macht (`GameWorld(Engine&)`), und eine, die ein Programm-Blueprint macht (der Host des Programms
  gibt sie ihr beim Machen, `Program::set_profile` – `kern::run` setzt es vor `App::start`; Test `program_world_has_the_project`),
  sonst mit `GameWorld::set_profile` (Editor, Test); kein Profil: nur die des Levels. Ein Schlüssel, den `WorldSettings`
  nicht hat, oder ein Wert, den der Level-Leser nicht nimmt, ist beim Lesen des Projekts ein Fehler mit Stelle und den
  erlaubten Namen (`settings: profiles[0] "mobile".world: unknown key "shadowDistanz" - one of …`); ein gewähltes Profil,
  das das Projekt nicht hat, hält die Welt vom Start ab (mit den vorhandenen Namen). Im Editor stehen die Profile mit
  ihrem `world` in den Projekt-Einstellungen (Settings, dieselbe Prüfung). Geprüft: `world_takes_its_device_profile`.
- `levels` nennt die Levels des Projekts (Pfade relativ zur Projekt-Datei), `startLevel` das erste davon. Welches Level
  wann läuft, entscheidet das Programm (siehe 4.); `startLevel` ist nur die Vorgabe, die der Editor bei Play öffnet und
  die ein Programm ohne eigene Wahl nimmt.
- `blueprints` enthält die Programm-Blueprints und die Bibliotheken des Programms.

## 3. Laufzeiten

```
 Programm-Laufzeit (eine, aus dem Projekt)        Welt-Laufzeiten (je gestartete GameWorld, aus einem Level)
 ┌──────────────────────────────┐                 ┌──────────────────────────────────────┐
 │ MeinSpiel (implements App)   │── startet ────▶ │ Level-Graph                           │
 │ Menue                        │   GameWorld     │ Akteur Tür (Blueprint)                │
 │ Spielstand                   │   (Handle)      │ Akteur Gegner (Blueprint)             │
 └──────────────────────────────┘                 └──────────────────────────────────────┘
          ▲                                                    │
          └──── Welt → Programm nur über Objekte der Engine ───┘
               (fester Schritt, Schnittstellen wie App)
```

- Beim Start entsteht **genau eine Programm-Laufzeit** mit allen Programm-Blueprints des Projekts, in ihrer Reihenfolge.
  Sie hat einen eigenen Host: keine Welt, keine Simulationszeit, eigene Objekte der Engine. Begin Play läuft einmal.
- Eine **Welt** startet das Programm selbst, über die Knoten der Engine (`GameWorld` machen, ein Level des Projekts
  laden, starten). Der Handle der GameWorld ist ein Wert des Programms. Ein Levelwechsel bedeutet: Welt beenden, nächste
  Welt starten. Die Programm-Laufzeit bleibt dabei bestehen. Mehrere Welten zugleich sind möglich. **Eine Welt, die das
  Programm macht, hat sein Projekt von selbst** – kein Programm reicht es weiter: ein GameWorld, das ein Knoten im
  Host des Programms macht (über `EngineObjects::on_made`), und ein GameWorld, das eine App in C++ aus ihrer Engine
  macht (`GameWorld(Engine&)`: die Engine kennt das Projekt des Programms, das sie fährt – `Engine::project()`, gesetzt
  von `kern::run(App&, const Project&)` und `kern::run(Project)` vor `App::start`, zurückgenommen nach `App::stop`).
  Ihre Blueprints finden Bibliotheken erst im Level, dann im Projekt (5.). Eine Welt des Programms endet spätestens mit
  den Objekten des Programms. `GameWorld::set_project` bleibt nur für einen Host ohne Programm (der Editor, ein Test).
  Geprüft: `program_world_has_the_project`, `world_of_an_app_of_cpp_has_the_project`.
- **Ein Level starten und wechseln:** `GameWorld::start_level(path, mode)` (Knoten „Start Level“ an einer Welt des
  Programms) startet das Level des Projekts mit diesem Pfad (wie `levels` ihn nennt). Das Projekt hält die Texte seiner
  Levels (`Project::level_texts`, nicht Teil der Datei: `embedded_project()` füllt sie aus dem Bau, der Editor aus den
  Dateien daneben). Wechseln heißt: „Stop“, dann „Start Level“ des nächsten; was das Programm behält (Punkte, ein
  Zustand), sind seine eigenen Variablen und bleiben. Fehler mit Grund: die Welt hat kein Projekt (die Meldung sagt, wie eine
  Welt des Programms es hat), das Projekt hat kein Level dieses Pfads (seine Levels werden genannt) oder seinen Text
  nicht, der Text ist kein Level. Geprüft: `world_starts_a_level_of_its_project` (zwei Levels, Wechsel, Wert bleibt,
  auch über den Knoten einer Welt des Programms). Eine App in C++ macht es ebenso: `kern/games/murmel/src/murmel.cpp`
  (`Murmel::start`: `GameWorld(engine)`, `Murmel::lade`: je Level `start_level`; die Töne der Welt lädt
  `GameAudio::begin`), nativ belegt vom CI-Job `murmel` (Level 2 im erwarteten Frame, Punkte über beide Levels).
- Jede Welt hat ihre **eigene Logik-Laufzeit** für ihren Level-Graphen und die Blueprints ihrer Akteure, wie heute
  (`GameWorld::start_logic`). Ihre Ticks laufen in der Reihenfolge, die `Runtime::fire` festlegt: zuerst der
  Level-Graph, dann die Akteure in der Reihenfolge des Levels.

## 4. Der Einstieg

Ein Programm hat **genau einen**, ausdrücklichen Einstieg und keine schwachen Symbole:

```cpp
// ein Spiel, dessen App C++ ist
int kern_main() { static MeinSpiel app; return kern::run(app); }

// ein Spiel, dessen App C++ ist und das ein Projekt hat (Einstellungen, Programm-Blueprints neben der App)
int kern_main() { static MeinSpiel app; return kern::run(app, kern::game::embedded_project()); }

// ein Spiel aus seinem Projekt (die App ist ein Programm-Blueprint)
int kern_main() { return kern::run(kern::game::embedded_project()); }
```

- `kern_main()` (`kern/app/run.h`) definiert das Programm; die Plattformschicht (`kern/web/app.mjs`) ruft ihn einmal,
  vor `kern_app_start`. Ein Modul ohne ihn startet keine App, die Seite sagt es („module has no kern_main“). Die
  Engine fragt nach keiner Fabrik, und kein Symbol ist schwach.
- `kern::run(App&, settings)` übergibt der App-Laufzeit die App und die Einstellungen des Projekts (Vorgabe: die der
  Engine). Sie wendet die Einstellungen vor dem Renderer an, ruft extensions, startet den Renderer, danach start,
  update in jedem Frame und stop. Ein zweites `run` wird abgewiesen (1, mit Grund im Log); die erste App bleibt.
- `kern::run(Project)` (`kern/game/program.h`) startet die Programm-Laufzeit des Projekts. Die App ist das Objekt der
  Engine des **einen** Programm-Blueprints, der `App` implementiert (`kern::Implements<kern::App>`). Gibt es keinen oder
  zwei, bricht der Start ab und meldet den Grund mit Namen. Hält die App an (`kern_app_stop`), endet das Programm nach
  `App::stop`; startet sie wieder, startet es neu aus dem Projekt.
- `kern::run(App&, const Project&)` (`kern/game/program.h`): die App ist C++, das Programm des Projekts läuft neben ihr
  (`Program::start(project, app)`) – seine Blueprints (Menüs, Bibliotheken, die das C++ der App als ihre Klassen ruft)
  sind ab vor `App::start` bis nach `App::stop` gebunden. Ein Blueprint des Projekts, der ebenfalls `App` implementiert,
  weist den Start ab und wird genannt (ein Programm hat eine App); ein Projekt ohne Blueprints ist ein leeres Programm.
  Geprüft: `program_beside_an_app_of_cpp`.
- **Die Programm-Laufzeit bindet ihre Klassen.** Jeder Programm-Blueprint ist eine Instanz – eine Bibliothek auch, wie
  der Akteur einer Welt –, und jeder, der als seine Klasse läuft, ist beim Start gebunden (`NativeBlueprint::bind_class`):
  was das C++ des Programms ruft (`kern::logic::blueprint<T>()`) und was ein Export ruft, ohne dass das Programm selbst
  bindet. Die Schleifenwache der Programm-Logik ist die Einstellung `loopBudget` des Projekts (Vorgabe 200000,
  `logic::Runtime::budget`).
- **Exporte gehören zum Programm.** Ein Export (`KERN_EXPORT_AS`, `Function::exported`) steht nur in einem
  Programm-Blueprint. Die Programm-Laufzeit bindet beim Start jeden ihrer Blueprints, der als Klasse mit Exporten
  läuft (`NativeBlueprint::bind_exports`), und löst sie am Ende (danach meldet ein Export „no instance … is bound“);
  zwei Objekte derselben Klasse mit Exporten im Programm: der Start bricht ab und nennt den Blueprint. Ein Export in
  einem Blueprint einer Welt (Level-Graph oder Akteur) ist ein Fehler mit Ort und Grund: beim Lesen des Levels
  („actor "X": the function f is an export … - exports belong to the program“), in `game::validate` (Play startet
  nicht) und im Editor am offenen Blueprint. Geprüft: `program_binds_the_exports`, `studio_logic_implements`.
- Der Bau eines Spiels aus seinem Projekt bettet das Projekt samt seiner Levels ein (`kern/tools/embed_project.py`):
  `kern::game::embedded_project()` (`kern/game/embedded.h`) ist das gelesene Projekt. Die Daten sind Daten des
  Projekts und keine eigenen Symbole. Geprüft: `run_the_program_of_a_project`.

## 5. Bibliotheken finden

Ein Blueprint nennt seine Bibliotheken in `uses` (beim Namen).
- Ein **Programm-Blueprint** findet nur Bibliotheken des Projekts.
- Ein **Blueprint einer Welt** sucht zuerst in seinem Level, dann im Projekt.
- Steht derselbe Name im Level und im Projekt, ist das ein Fehler. Er wird beim Laden mit beiden Stellen gemeldet;
  stillschweigend gewinnt keiner.

## 6. Der feste Schritt und eigene Knoten

### 6.1 Der feste Schritt einer Welt (gebaut)

Was ein Programm in **jedem festen Schritt** einer Welt tun muss (zum Beispiel Kräfte, die Box3D nach jedem Schritt
löscht), gibt es der Welt ausdrücklich: ein Objekt, das `kern::game::FixedStep` ist (`kern/game/runtime.h`) – C++ des
Programms oder das Objekt der Engine eines Programm-Blueprints mit `kern::Implements<kern::game::FixedStep>` –, mit
`GameWorld::set_fixed_step(objekt)` (eins; `nullptr` nimmt es zurück, bevor das Objekt endet). Die Reihenfolge eines
festen Schritts ist festgelegt:

1. Eingaben, Antriebe, Gelenke, dann der Schritt der Physik (`substeps`),
2. die Ereignisse des Schritts (Kontakte, Trigger, Gesundheit),
3. **`FixedStep::fixed_step(dt)` des Programms**,
4. Event Tick der Blueprints der Welt (`Runtime::tick`: der Level-Graph, dann die Akteure in der Reihenfolge des Levels),
5. Respawns, Ziel und Zeit.

Was `fixed_step` ändert, sieht jeder Blueprint im Tick desselben Schritts; Kräfte wirken im nächsten Schritt der
Physik. Es hängt nicht davon ab, wo ein Knoten steht. Geprüft: `game_world_fixed_step`. (Ersetzt die frühere Anmeldung
`register_node_type`, ein globales Register, dessen Typen der Editor erst kannte, wenn das Programm lief, und dessen
Zeiger nach dem Ende der Programm-Laufzeit hingen – ersatzlos entfernt.)

### 6.2 Eigene Knoten eines Projekts

Einen eigenen Knotentyp als besondere Datenart gibt es nicht – ein Weg je Sache: **Die Funktion einer Bibliothek des
Projekts ist der eigene Knoten.** Ein Blueprint, der die Bibliothek benutzt (`uses`), hat ihre Funktionen als
Aufruf-Knoten in der Palette, mit ihren Ein- und Ausgängen; der Editor kennt sie, sobald das Projekt geladen ist.

```cpp
// im Projekt: die Bibliothek Mathe
struct Mathe : virtual kern::Library {
  struct clampedResult { Float value = 0.0; };
  KERN_PURE clampedResult clamped(Float value = 0.0) { return {math_max(value, 0.0)}; }
};
// in einem Level: ein Akteur, der sie benutzt – „Clamped“ ist sein Knoten
struct Tuer : virtual kern::Blueprint, virtual Mathe { … clamped(Hoehe) … };
```

Was in jedem Schritt laufen muss, ist kein Knoten, sondern der feste Schritt (6.1).

## 7. Ein Akteur kennt seine Welt: der typisierte Verweis `Actor`

**Gebaut** (26.11.2026). Ein Akteur im Blueprint ist ein Verweis eines eigenen Typs, nach demselben Muster wie
`Object<I>` (docs/engine_object.md): **seine Id und seine Welt**, nie eine Zahl.

| | |
|---|---|
| Pin-Typ | `actor` (Liste: `actor_list`) |
| C++-Text | `Actor`, `List<Actor>` – in der Engine `kern::logic::ActorHandle` |
| Wert | `logic::ActorHandle { ActorRef id; WorldId world; }` – die Id in ihrer Welt und die Nummer dieser Welt |
| kein Akteur | `Actor{}` (Id 0, Welt 0): die Vorgabe jeder Variablen dieses Typs, das einzige Literal |
| Welt | `logic::Host::world()`: die Welt der Objekte der Engine des Hosts (`EngineObjects::world`) – die Akteure einer `GameWorld` und ihre Objekte sind **eine** Welt (`GameWorld::world_id()`); ein Host ohne Objekte hat eine eigene Nummer (`logic::new_world`). Jeder Start einer Welt gibt eine neue Nummer: ein Akteur des Levels davor nennt im nächsten nichts. |

- **Die Engine gibt ihn so:** `GameWorld::spawn` (Vorlagen, docs/runtime_actors.md) gibt `logic::ActorHandle`; ein
  Pin `Actor` an einem Knoten der Engine (tools/engine_api.py: Tür `Conv<Actor>`, Klasse `Direct<Actor>`, Stellvertreter
  Form `a`) trägt ihn, wie er ist – welche Welt er haben muss, sagt die Funktion, die ihn nimmt.
- **Woher ein Akteur kommt:** nur aus seiner Welt – Self, Find Actor by Name, Find Actor with Tag, Spawn Actor,
  Get Player, ein Ereignis (Overlap, Hit …), For Each Actor with Tag, Line Trace. Jede dieser Stellen gibt den Akteur
  mit der Welt, in der der Knoten läuft (`Context::actor`, in der Klasse `kern_actor`).
- **Aus einer Zahl wird nie ein Akteur:** kein Draht Int → Actor, keine Umwandlung (`Value::converted` gibt keinen),
  kein Literal außer `Actor{}` – der Leser des C++-Texts weist `Actor{5}` mit Stelle und Grund ab („an actor is no
  number …“), `check` weist einen Wert eines Dokuments ab, der einen Akteur als Zahl hält („… holds an actor as a number“),
  ein JSON-Wert wird kein Akteur. Der Editor bietet am Akteur-Eingang keine Auswahl einer Id mehr: unverbunden heißt
  der Eingang, was der Knoten sagt (Self oder keiner); ein Akteur wird angeschlossen.
- **Der eine ausdrückliche Übergang zu Int:** der Knoten **Id of Actor** (`actor.id`, `actor_id(a)`) – zum Zeigen oder
  Zählen. Es gibt keinen Weg zurück.
- **Same Actor** (`actor.equal`, `actor_equal(a, b)`): dieselbe Welt und dieselbe Id; keiner und keiner sind derselbe.
  Ein Akteur einer anderen Welt ist nie derselbe, auch mit derselben Id. **Is Valid** (Gameplay, `actor.valid`) fragt
  wie bisher, ob der Akteur in dieser Welt lebt.
- **Ein Verweis aus fremder Welt trifft nie still einen Akteur.** Es gibt genau zwei Prüfungen, im Interpreter und in
  der Klasse gleich:
  - Die Knoten, die den **Blueprint** eines anderen Akteurs erreichen – Call on Actor, Get Variable on Actor, Has
    Blueprint, Call Event –, fragen `logic::Runtime::runtime_of`: ein Akteur der eigenen Welt läuft in dieser Laufzeit;
    einer einer anderen Welt in der Laufzeit, die der Host dort erreicht (`Host::world_runtime`, siehe „Vom Programm
    aus“), sonst erreicht der Knoten nichts (nicht den Akteur gleicher Id und nicht Self), gibt seine Vorgaben und sagt
    dem Host den Grund mit Blueprint und Knoten:
    `Poker: Call on Actor: actor 3 is of world 7, this Blueprint runs in world 6 - an actor names nothing in another world`.
  - Was den Akteur **in seiner Welt** benutzt – die Gameplay-Knoten (Get Location, Set Location, Damage, Spawn Actor mit
    Template …) und Engine Object of Blueprint (ein Objekt der Engine nennt nur in seiner Welt etwas, engine_object.md) –,
    fragt `logic::Runtime::actor_here`: nur ein Akteur der Welt, in der der Knoten läuft; sonst dieselbe Meldung.
- **Vom Programm aus** (gebaut 26.11.2026): Ein Programm-Blueprint erreicht die Blueprints der Akteure der Welten, die
  sein Programm fährt – Variable lesen (Get Variable on Actor), eine Funktion rufen (Call on Actor; gesetzt wird ein
  Wert eines Blueprints durch seine eigene Funktion), ein Ereignis schicken (Call Event), Has Blueprint. Der Host des
  Programms schlägt dazu die Welt des Verweises nach: das GameWorld unter seinen Objekten der Engine
  (`EngineObjects::all<GameWorld>()`), das als diese Welt läuft (`world_id()`), und dessen Logik-Laufzeit. Den Verweis
  gibt die Welt: `GameWorld::actor_named(name)` (Knoten der Engine „GameWorld actor named“; Actor{} mit Grund in
  `last_error`, wenn die Welt nicht läuft oder keinen Akteur des Namens hat) oder `GameWorld::spawn`.
  Ausdrücklich, mit Grund, wenn nichts erreicht wird: läuft die Welt nicht mehr („… - its world does not run any more
  (the GameWorld was stopped)“), ist sie neu gestartet (eine neue Welt: „… - no world of this program runs as world 7
  (it ended and started again as a new world, or the program did not make it)“). Eine Welt, die eine App in C++ macht
  (`GameWorld(Engine&)`), ist kein Objekt des Programm-Hosts – ihre Akteure erreicht die App in C++ selbst
  (`ActorBlueprint`). **In die andere Richtung**, von einer Welt zum Programm oder zu einer anderen Welt, gibt es
  **kein** stilles Durchfallen: der Host einer Welt erreicht nur ihre eigenen Akteure; der Weg führt über Objekte der
  Engine, die das Programm der Welt gibt (den festen Schritt, 6.1, oder eine Schnittstelle).
  Geprüft: `program_reaches_actors_of_its_worlds` (ein Programm, zwei Welten desselben Levels mit Akteuren gleicher Id:
  Aufruf, Ereignis, Variable, Has Blueprint treffen nur die Welt des Verweises – auch als Klasse; Welt gestoppt und neu
  gestartet: nichts erreicht, Grund gesagt; eine Welt erreicht weder das Programm noch die andere Welt).
- **Konverter:** `kern::logic::ActorHandle` im C++ eines Spiels ist `Actor`; `ActorHandle{}` ist `Actor{}`, `a == b`
  Same Actor, `a.id` Id of Actor; ein `ActorHandle` aus Zahlen wartet mit Grund. Der Testschreiber zieht Akteure als
  Werte der Sprache (keiner oder einer von wenigen Ids in einer von zwei Welten), nicht als Struct
  (tools/converter_cases: `actors`).
- **Tests:** `actors_know_their_world` (zwei Welten mit Akteuren gleicher Id: einer aus Welt B erreicht in Welt A
  nichts und sagt warum, der eigene schon; die Klasse fragt ebenso; kein Int wird ein Akteur), die Fälle der Stufen
  (blueprint_native: Akteure als Werte mit Welt), `tools/converter_cases/run.sh` (actors).

## 7a. Ein Programm aus C++: wohin der Konverter schreibt

`tools/cpp_to_blueprint.py --convert <ns> --level <level.klevel> --project <spiel.kproject> --folder <ordner>` schreibt jeden Blueprint
dorthin, wo er nach 1. hingehört – die Regel folgt daraus, was ein Blueprint ist und wer ihn benutzt, nie aus einem
Namen oder Ordner:

- **ins Projekt** (Programm): die App (ein Zustand, der `App` implementiert – `kern::run` startet das Programm mit
  ihrem Objekt), jeder Blueprint mit einem Export (Exporte gehören zum Programm, 4.), und jeder Blueprint, den ein
  Blueprint des Programms benutzt (`uses`, auch über andere hinweg – ein Programm-Blueprint findet Bibliotheken nur im
  Projekt, 5.). Eine Bibliothek, die Programm und Welt benutzen, steht also im Projekt; die Welt findet sie dort.
- **ins Level** (Welt): jeder andere.

Das Protokoll nennt je Blueprint Ort und Grund („where the Blueprints stand“, z. B. `CasesApp: the project - it
implements App …`, `door_cpp: the level - a world's …`). Ein Blueprint, der schon im Projekt steht und den der Lauf
nicht schreibt (eine Datei des Programms, die jetzt ein Blueprint ist: ihr C++ ist weg, das C++ des Programms ruft
ihn als seine Klasse), ist des Programms – was er benutzt, steht darum auch im Projekt. Ein Blueprint des eigenen
Ordners, der aus einem früheren Lauf im Level steht und nun des Programms ist, zieht ins Projekt; einer eines
anderen Ordners (eine eigene Quelle) wird nie verschoben: steht er im Level und benutzt ihn das Programm, bricht der
Lauf mit Name, Ordner und Grund ab, und beide Dateien bleiben, wie sie sind. Ein Lauf ohne `--project`, der
Programm-Blueprints hat, bricht mit ihren Namen und Gründen ab.
Projekt-Blueprints tragen wie Akteure einen Ordner (`"folder"`, Ordnung im Editor); der Konverter kennt daran seine
eigenen – `--folder` ist darum anzugeben (keine Vorgabe). Nur mit `--whole-program` (ein Lauf über das ganze
Programm, ausdrücklich, nie aus der Zahl der Dateien geschlossen) fällt aus seinem Ordner, was er nicht mehr schreibt.
Welche Blueprints das C++ des Programms direkt als Klassen ruft, sagt das Projekt (seine Blueprints, die der Lauf
nicht schreibt – nie ein Ordnername); `--classes <header.h>` nennt nur den Header, in den der Bau ihre Klassen
schreibt (keine Vorgabe; braucht `--project`). Ebenso `tools/blueprint_cpp.sh --project <p> <level> <out.cpp> <out.h>`:
der Header hält die Klassen der Blueprints des Projekts und der Bibliotheken, die sie benutzen. Das eine Objekt
der App findet er auch im Einstieg `int kern_main() { static MeinSpiel app; return kern::run(app); }`. Beide Dateien werden vom Leser der Engine gelesen (`tools/blueprint_cpp.sh --apply <datei> [--with <projekt>]`;
ein Level weist dabei einen Export ab, `game::world_export`) und nur geschrieben, wenn alle Blueprints gelesen sind.
Die Klassen schreibt `tools/blueprint_cpp.sh --project <spiel.kproject> <level> …` aus beiden. Geprüft:
`tools/converter_cases/run.sh` (Fall `program` + `door`: App, Export und deren Bibliothek im Projekt, die Tür im
Level, beide gelesen, ein Export im Level abgewiesen; `placement.py`: was ein Blueprint des Projekts benutzt, steht im
Projekt; eine eigene Quelle im Level, die das Programm benutzt, hält den Lauf an; `--classes` ohne Ordner).

## 8. Ein zweites Spiel anlegen – Schritt für Schritt

1. Im Editor „Neues Projekt“ wählen. Das legt `spiel.kproject` und ein leeres `levels/start.klevel` an.
2. Im Projekt einen Blueprint „Spiel“ anlegen, „Implements … App“ wählen und `start`, `update`, `stop` schreiben. In
   `start` ein GameWorld machen und mit „Start Level“ das Start-Level starten (3., „Ein Level starten und wechseln“).
3. Im Level die Akteure und ihre Blueprints anlegen.
4. Optional ein fester Schritt: im Projekt einen Blueprint „FesterSchritt“ mit „Implements … game_FixedStep“ und
   `fixed_step` schreiben; das Programm gibt ihn der Welt mit „Set Fixed Step“ (6.1).
5. Im Editor spielen: Play → **Play Project** (studio.md „Play Project“) spielt das Programm, wie das Spiel es spielt –
   die App im Viewport, ihre Levels, Widgets, Aktionen und Töne; Esc beendet. (Play allein spielt nur das offene Level.)
6. Bauen: Der Bau erzeugt den Einstieg `kern::run(kern::game::embedded_project())` (kern/game/embedded.h, siehe 4.).
7. Testen (9.).

## 9. Testen

- **Engine:** `kern/tests/run.sh [filter]` baut die Engine nativ (ASan/UBSan) und fährt `kern/tests/test_main.cpp`;
  die Tests dieses Texts heißen wie unter „Stand“ genannt. Danach vergleicht er jeden Blueprint-Fall als C++-Klasse
  mit dem Interpreter (blueprint_cpp.md „Der Beweis“).
- **Ein Spiel:** seine Tests registriert es mit `KERN_TEST(name) { KERN_CHECK(bedingung, "…", …); }`
  (`kern/test/test.h`); `kern/tools/test_main.cpp` fährt sie. Die Vergleichstests, die der Konverter schreibt
  (Quelle gegen Blueprint, `kern/test/converted.h`), stehen in derselben Liste (blueprint_cpp.md „Quelle gegen
  Blueprint“). Beispiel: `kern/games/murmel` (`run.sh`, CI-Job `murmel`).
- **Den Stand eines Programms lesen:** `kern::game::running_program()` (`kern/game/program.h`) ist das Programm, das
  `kern::run` startete, solange es läuft (bis die App stoppt, wieder ab ihrem nächsten Start; sonst null). Ein Test
  des Spiels liest dort die Variablen des App-Blueprints (`runtime()->instances()`, `Instance::get`), ruft seine
  Funktionen (`Instance::call`) und erreicht die Objekte der Engine, die es hält (`runtime()->host().engine_objects()`:
  seine Welten). Beispiel: `kern/tests/native_build/mini.cpp`.
- **Konverter:** `tools/converter_cases/run.sh` – kleine Fälle je Form von C++ (auch `program`, `door`, `actors`).
- **Editor und Browser:** `kern/tests/e2e/studio_e2e.mjs` (Studio, Projekt, Play Project), `blueprint_e2e.mjs`,
  `input_e2e.mjs`; ein Spiel im Browser: CI-Job `murmel-web`.

## Stand (26.11.2026, Zweig work-projekt)

- Gebaut: die Projekt-Datei (`kern/game/project.h`: `Project`, Leser/Schreiber mit Stelle und Grund,
  `program_blueprints`, `world_blueprints`, `library_conflicts`; ein Level hat keinen Abschnitt `"project"`), die
  Programm-Laufzeit (`kern/game/program.h`: `kern::game::Program`, bindet jede Klasse und die Exporte), das Einbetten
  (`kern/tools/embed_project.py`, `kern/game/embedded.h`), der eine Einstieg `kern_main()` mit `kern::run` (4.; keine
  schwachen Symbole), die Einstellungen als ein Weg (`profiles`, kein `App::configure`), der feste Schritt (6.1),
  Levels starten und wechseln (`GameWorld::start_level`), das Projekt einer Welt des Programms von selbst (3.:
  `EngineObjects::on_made`, `GameWorld(Engine&)`), der Konverter mit Projekt (7a.). Tests: `game_project_round_trip`,
  `game_project_errors`, `program_from_the_project`, `run_the_program_of_a_project`, `program_world_has_the_project`,
  `world_of_an_app_of_cpp_has_the_project`, `program_binds_the_exports`, `program_beside_an_app_of_cpp`,
  `world_starts_a_level_of_its_project`, `program_reaches_actors_of_its_worlds`, `game_world_fixed_step`,
  `tools/converter_cases/run.sh`. Akteure einer Welt vom Programm aus (7.).
- Gebaut im Editor: „Projekt“ (`kern/src/studio/project.cpp`, docs/studio.md „Das Projekt“): öffnen mit seinen
  Levels (das Start-Level ist das offene Level), speichern, Levels, Start-Level, Einstellungen, Programm-Blueprints im
  Blueprint-Editor; Play spielt das offene Level mit den Bibliotheken des Projekts. „Play Project“
  (`kern/src/studio/project_play.cpp`): das Programm mit seiner App im Viewport – `game::ProgramApp` auf einer
  `AppRuntime` im Host des Editors (`kern::AppHost`, app.md 1.). Tests `studio_project`, `studio_play_project`, e2e.
- Zweites Spiel: `kern/games/murmel` (App in C++, zwei Levels, Wechsel, Punkte über die Levels) – nativ und im
  Browser belegt (CI-Jobs `murmel`, `murmel-web`). Seine App ist C++ (`kern::run(app, projekt)`): „Play Project“
  spielt nur ein Programm, dessen App ein Programm-Blueprint ist – Murmel dort zu spielen braucht seine App als
  Blueprint (sie wartet noch am Testrahmen der Vergleichstests).
- Offen: Feld-Anfangswerte, die keine Konstante sind (`ShaderExtension* ext_[1] = {&look()}` in einem Zustand ohne
  eigenen Konstruktor), im Konverter und Vergleichstest.
- SpawnCity: `kern::run(sc::app(), embedded_project())`; seine Programm-Klassen stehen im Projekt.

