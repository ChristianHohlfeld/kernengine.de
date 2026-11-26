# Projekt, Level, Programm – wie ein Spiel auf Kern aufgebaut ist

Dieser Text beschreibt, wie man ein Spiel auf Kern anlegt, ohne ein bestehendes Spiel als Vorlage zu nehmen. Er ist
der Entwurf (Stand 25.11.2026), nach dem die Engine gebaut wird. Wo die Engine heute noch anders arbeitet, steht das
unter „Stand“ am Ende.

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
    "profiles": [ { "name": "mobile", "renderer": { "shadow": { … }, "ao": { … } } } ],
    "heapReserveMb": 0
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
  doppeltem Namen: Fehler mit Stelle. Ein Level hat keinen Abschnitt `"project"`.
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
  Welt starten. Die Programm-Laufzeit bleibt dabei bestehen. Mehrere Welten zugleich sind möglich. Eine Welt, die das
  Programm macht (ein GameWorld, das ein Knoten im Host des Programms macht), bekommt dabei das Projekt
  (`GameWorld::set_project`, über `EngineObjects::on_made`): ihre Blueprints finden Bibliotheken erst im Level, dann
  im Projekt (5.). Sie endet spätestens mit den Objekten des Programms. Geprüft: `program_world_has_the_project`.
- Jede Welt hat ihre **eigene Logik-Laufzeit** für ihren Level-Graphen und die Blueprints ihrer Akteure, wie heute
  (`GameWorld::start_logic`). Ihre Ticks laufen in der Reihenfolge, die `Runtime::fire` festlegt: zuerst der
  Level-Graph, dann die Akteure in der Reihenfolge des Levels.

## 4. Der Einstieg

Ein Programm hat **genau einen**, ausdrücklichen Einstieg und keine schwachen Symbole:

```cpp
// ein Spiel, dessen App C++ ist
int kern_main() { static MeinSpiel app; return kern::run(app); }

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

## 7. Akteure einer Welt aus dem Programm erreichen: der Eingang „World“

Die Knoten, die einen anderen Blueprint ansprechen (Call on Actor, Get Variable on Actor, Has Blueprint), haben einen
Eingang **World** (Int, der Handle einer GameWorld):

- `0` (die Vorgabe) bedeutet die eigene Laufzeit, wie bisher.
- Ein Handle bedeutet die Logik-Laufzeit **dieser** Welt (`Host::runtime_of(handle)`). Ist der Handle keine Welt mit
  Logik, gibt der Knoten seine Vorgaben und meldet den Grund.
- Actor-Ids gelten nur in ihrer Welt. Eine Id aus Welt A in Welt B zu verwenden, ist ein Fehler des Programms.
- Einen Akteur in einer Welt findet man mit `GameWorld::find_actor(name)` (Knoten der Engine, mit dem Handle der Welt).
- In der Notation steht der Eingang nur, wenn er verbunden oder ungleich 0 ist:
  `func_call_on<"Oeffnen">(kern::in_world(welt, tuer), kern::out())`. Ohne ihn bleibt der Text wie bisher.
- In die andere Richtung, von der Welt zum Programm, gibt es **kein** stilles Durchfallen. Der Weg führt nur über Objekte
  der Engine, die das Programm der Welt gibt: den festen Schritt (6.1) oder eine Schnittstelle.

## 8. Ein zweites Spiel anlegen – Schritt für Schritt

1. Im Editor „Neues Projekt“ wählen. Das legt `spiel.kproject` und ein leeres `levels/start.klevel` an.
2. Im Projekt einen Blueprint „Spiel“ anlegen, „Implements … App“ wählen und `start`, `update`, `stop` schreiben. In
   `start` ein GameWorld machen und `startLevel` laden.
3. Im Level die Akteure und ihre Blueprints anlegen.
4. Optional ein fester Schritt: im Projekt einen Blueprint „FesterSchritt“ mit „Implements … game_FixedStep“ und
   `fixed_step` schreiben; das Programm gibt ihn der Welt mit „Set Fixed Step“ (6.1).
5. Play im Editor oder bauen: Der Bau erzeugt den Einstieg `kern::run(kern::game::embedded_project())` (kern/game/embedded.h, siehe 4.).

## Stand (25.11.2026, Zweig work-projekt)

- Gebaut (Teil 1): die Projekt-Datei (`kern/game/project.h`: `Project`, Leser/Schreiber mit Stelle und Grund,
  `program_blueprints`, `world_blueprints`, `library_conflicts`), die Programm-Laufzeit aus dem Projekt
  (`kern/game/program.h`: `kern::game::Program`), die Suche der Bibliotheken einer Welt (`GameWorld::set_project`),
  das Einbetten (`kern/tools/embed_project.py`, `kern/game/embedded.h`). Ein Level hat keinen Abschnitt `"project"`
  mehr (Fehler beim Lesen); `ProjectSettings::program`, `kern/game/level_app.h` und `embed_level.py --split-program`
  sind entfernt.
- Gebaut (Teil 1b): der Editor „Projekt“ (`kern/src/studio/project.cpp`, docs/studio.md „Das Projekt“): öffnen,
  speichern, Levels und Start-Level, Einstellungen, Programm-Blueprints im Blueprint-Editor; Studio-Test
  `studio_project`, e2e im Modul-Bau.
- Gebaut (Teil 3): der eine Einstieg `kern_main()` mit `kern::run(App&, settings)` / `kern::run(Project)`; die
  schwachen Symbole `kern_create_app`, `kern_project_app`, `kern_project_text` und `kern_game_extensions` sind
  ersatzlos entfernt (Test `run_the_program_of_a_project`). Einstellungen: ein Weg – `App::configure()` ist entfernt,
  Geräteprofile als Liste `profiles` statt `mobile` (Tests `game_project_round_trip`, `studio_project`). Welten, die
  das Programm macht, bekommen das Projekt (`program_world_has_the_project`). Exporte: die Programm-Laufzeit bindet
  und löst sie; ein Export in einer Welt wird abgewiesen (`program_binds_the_exports`).
  OFFEN (SpawnCity): `Game::start` bindet die Klassen des Ordners „Program“ noch an die Logik der Welt
  (`blueprints::bind(*world_->logic_runtime())`) – sie stehen bis zum Schaltschritt im Level und das Spiel hat eine
  App aus C++; das Binden fällt weg, sobald der Konverter sie ins Projekt schreibt (`spawncity.kproject`).
- Reihenfolge des Baus: (1) Projekt-Datei mit Programm-Blueprints, Editor „Projekt“, (2) fester Schritt der Welt (Knotentypen entfallen),
  (3) Einstieg `kern::run`, (4) Eingang „World“. Je Teil gibt es einen Engine-Test und einen Studio-Test.
