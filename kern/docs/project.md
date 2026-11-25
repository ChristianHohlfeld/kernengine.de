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
    "mobile": { "shadow": { … }, "ao": { … } },
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

- `settings` enthält die heutigen Projekt-Einstellungen (`kern/app/project.h`, `ProjectSettings`). Die Engine wendet sie
  vor dem Start des Renderers an. Ein Level hat dann keinen Abschnitt `"project"` mehr.
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
               (Knotentyp des Projekts, Schnittstellen wie App)
```

- Beim Start entsteht **genau eine Programm-Laufzeit** mit allen Programm-Blueprints des Projekts, in ihrer Reihenfolge.
  Sie hat einen eigenen Host: keine Welt, keine Simulationszeit, eigene Objekte der Engine. Begin Play läuft einmal.
- Eine **Welt** startet das Programm selbst, über die Knoten der Engine (`GameWorld` machen, ein Level des Projekts
  laden, starten). Der Handle der GameWorld ist ein Wert des Programms. Ein Levelwechsel bedeutet: Welt beenden, nächste
  Welt starten. Die Programm-Laufzeit bleibt dabei bestehen. Mehrere Welten zugleich sind möglich.
- Jede Welt hat ihre **eigene Logik-Laufzeit** für ihren Level-Graphen und die Blueprints ihrer Akteure, wie heute
  (`GameWorld::start_logic`). Ihre Ticks laufen in der Reihenfolge, die `Runtime::fire` festlegt: zuerst der
  Level-Graph, dann die Akteure in der Reihenfolge des Levels.

## 4. Der Einstieg

Ein Programm hat **genau einen**, ausdrücklichen Einstieg und keine schwachen Symbole:

```cpp
// ein Spiel, dessen App C++ ist
int kern_main() { static MeinSpiel app; return kern::run(app); }

// ein Spiel aus seinem Projekt (die App ist ein Programm-Blueprint)
int kern_main() { return kern::run(kern::Project::embedded()); }
```

- `kern::run(App&)` übergibt der App-Laufzeit die App. Sie ruft configure/extensions vor dem Renderer, danach start,
  update in jedem Frame und stop.
- `kern::run(Project)` startet die Programm-Laufzeit des Projekts. Die App ist das Objekt der Engine des **einen**
  Programm-Blueprints, der `App` implementiert (`kern::Implements<"App">`). Gibt es keinen oder zwei, bricht der Start
  ab und meldet den Grund mit Namen.
- Der Bau eines Spiels aus seinem Projekt erzeugt den Einstieg und bettet das Projekt samt seiner Levels ein
  (`kern::Project::embedded()`). Die Daten sind Daten des Projekts und keine eigenen Symbole.

## 5. Bibliotheken finden

Ein Blueprint nennt seine Bibliotheken in `uses` (beim Namen).
- Ein **Programm-Blueprint** findet nur Bibliotheken des Projekts.
- Ein **Blueprint einer Welt** sucht zuerst in seinem Level, dann im Projekt.
- Steht derselbe Name im Level und im Projekt, ist das ein Fehler. Er wird beim Laden mit beiden Stellen gemeldet;
  stillschweigend gewinnt keiner.

## 6. Knotentypen des Projekts

Ein Projekt kann eigene Knotentypen haben, zum Beispiel einen festen Schritt der Physik. Ein Programm-Blueprint, der die
Schnittstelle **`logic_NodeType`** implementiert, **ist** ein Knotentyp des Projekts:

- Seine Beschreibung sind **Daten des Blueprints**, keine Funktionen:
  - Typ: `project.<Name des Blueprints>`
  - Label: der Anzeigename des Blueprints
  - Ereignis: eine Einstellung des Blueprints, z. B. `logic.tick`; leer heißt, der Knoten ist eine Aktion
  - Hilfe: die Beschreibung des Blueprints

  Damit kennt der Editor den Knoten, **sobald das Projekt geladen ist**. Er steht in der Palette, ohne dass etwas läuft.
- Die Schnittstelle selbst hat nur `run()` (und optional `help()`). Läuft ein Knoten dieses Typs (in einer Welt), ruft er
  `run` des Blueprints in der Programm-Laufzeit.
- Die Knotentypen gelten **je Projekt**: Das Projekt baut beim Laden seine Library aus dem Kern der Engine und seinen
  Knotentypen. Editor, Prüfung (`check`), Notation und alle Laufzeiten des Projekts nutzen diese Library. Eine globale
  Anmeldung zur Laufzeit gibt es nicht.
- Wo der Knoten steht, entscheidet das Level (oder das Programm, wenn es eine Welt baut). Steht er im Level-Graphen,
  läuft er in jedem Tick als Erstes.

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
  der Engine, die das Programm der Welt gibt: einen Knotentyp des Projekts oder eine Schnittstelle.

## 8. Ein zweites Spiel anlegen – Schritt für Schritt

1. Im Editor „Neues Projekt“ wählen. Das legt `spiel.kproject` und ein leeres `levels/start.klevel` an.
2. Im Projekt einen Blueprint „Spiel“ anlegen, „Implements … App“ wählen und `start`, `update`, `stop` schreiben. In
   `start` ein GameWorld machen und `startLevel` laden.
3. Im Level die Akteure und ihre Blueprints anlegen.
4. Optional einen Knotentyp: im Projekt einen Blueprint „FesterSchritt“ mit „Implements … logic_NodeType“, Ereignis
   `logic.tick`, und `run` schreiben. Danach den Knoten „FesterSchritt“ aus der Palette in den Level-Graphen ziehen.
5. Play im Editor oder bauen: Der Bau erzeugt den Einstieg `kern::run(kern::Project::embedded())`.

## Stand (25.11.2026, Zweig work-projekt)

- Gebaut (Teil 1): die Projekt-Datei (`kern/game/project.h`: `Project`, Leser/Schreiber mit Stelle und Grund,
  `program_blueprints`, `world_blueprints`, `library_conflicts`), die Programm-Laufzeit aus dem Projekt
  (`kern/game/program.h`: `kern::game::Program`), die Suche der Bibliotheken einer Welt (`GameWorld::set_project`),
  das Einbetten (`kern/tools/embed_project.py`, `kern/game/embedded.h`). Ein Level hat keinen Abschnitt `"project"`
  mehr (Fehler beim Lesen); `ProjectSettings::program`, `kern/game/level_app.h` und `embed_level.py --split-program`
  sind entfernt.
- Noch nach diesem Entwurf zu ersetzen:
  - `kern/logic/node_type.h` mit `register_node_type`, einer globalen Anmeldung zur Laufzeit (Teil 2)
  - schwache `kern_create_app`/`kern_project_app`/`kern_project_text` (Teil 3: `kern::run`)
- Reihenfolge des Baus: (1) Projekt-Datei mit Programm-Blueprints, Editor „Projekt“, (2) Knotentypen je Projekt,
  (3) Einstieg `kern::run`, (4) Eingang „World“. Je Teil gibt es einen Engine-Test und einen Studio-Test.
