# Akteure zur Laufzeit: Vorlagen und `spawn`

Entwurf und Stand (26.11.2026, Zweig work-vorlagen). Ein Spiel hält nie `game::Level`, `game::Actor` oder eine
Komponente als Wert – diese Typen sind die Innereien des Levels und bleiben in der Engine (nicht `KERN_BLUEPRINT_TYPE`,
blueprint_cpp.md „Die Bibliotheken der Engine“). Akteure, die ein Spiel zur Laufzeit braucht (Fahrzeuge, Münzen,
Geschosse, Gegner), entstehen aus **Vorlagen** des Levels.

## Die Vorlage

- Ein Akteur des Levels mit `"template": true` (C++ `Actor::is_template`, im Editor „Template“ in den Details, im
  Outliner mit der Marke „Template“) ist eine Vorlage. Seine Kinder gehören zu ihr; eine Vorlage steht oben im Level
  (keine Eltern).
- Eine Vorlage hat alles, was ein Akteur hat: Komponenten (Mesh, Körper, Kollider, Pickup …), Tags und ihren Blueprint.
- **Sie steht nicht in der Welt.** `GameWorld::start` setzt sie und ihre Kinder nicht in die Welt: kein Körper, kein
  Mesh, kein laufender Blueprint, `level()` und `find_actor` kennen sie nicht. Die Welt hält sie getrennt
  (`GameWorld::templates()`). Im Editor (nicht laufend) ist sie sichtbar und zu bearbeiten wie jeder Akteur.
- Prüfungen beim Start (`validate`, Fehler mit Akteur und Grund): zwei Vorlagen gleichen Namens; eine Vorlage mit
  Eltern; ein Gelenk zwischen einer Vorlage und etwas außerhalb von ihr; der Spieler des Levels (`world.player`) ist
  eine Vorlage oder ein Teil von ihr. Ein Spieler-Akteur in einer Vorlage ist nie „der Spieler“ (`default_player`).

## `spawn`

```cpp
ActorId GameWorld::spawn(std::string_view template_name, const Transform& at, ActorId id = 0);
```

- Eine Kopie der Vorlage mit ihren Kindern, Komponenten und ihrem Blueprint. Begin Play der Kopie läuft sofort, auch
  wenn ein Graph spawnt – im Budget des laufenden Schritts (`logic::Runtime::begin_added`). Die Wurzel bekommt
  `at` als ihre Transformation in der Welt (auch die Skalierung), die Kinder behalten ihre relative.
- **Ids:** die nächsten freien in der Reihenfolge Wurzel, Kinder (wie im Level) – deterministisch: dieselben Aufrufe
  geben dieselben Ids. `id` setzt die Id der Wurzel (ein Replikat spiegelt, was sein Server gesetzt hat); sonst 0.
- Die Kopie trägt den Namen der Vorlage. Ein Pickup in ihr zählt zu `Hud::pickups_total`.
- Repliziert: jeder Mitspieler bekommt den Namen der Vorlage, die Id und `at` (`GameWorld::SpawnChange`).
- **Fehler** (Rückgabe keiner, `ActorHandle{}`; `last_error()` sagt den Grund): die Welt läuft nicht; keine Vorlage dieses Namens – die
  Meldung nennt die Vorlagen des Levels („spawn: no template "Crate" - the level's templates: Coin, Car“, oder dass es
  keine hat); die Ids sind belegt.
- Entfernen: `GameWorld::destroy_actor(id)` (Knoten „Destroy Actor“) – der Akteur ist aus der Welt, sein Blueprint endet.
- Es gibt **einen** Weg: `spawn_copy` (Kopie irgendeines Akteurs der Welt) ist ersetzt, nicht daneben gestellt.

**Knoten:** „Spawn Actor“ (`actor.spawn`) mit der Einstellung *Template* (der Name der Vorlage, am Knoten zu sehen) und
den Eingängen Location, Rotation, Scale; Ausgang *Spawned* (der Akteur). C++-Text:
`actor_spawn<"Coin">(n3, Vector{0.0, 3.0, 0.5}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0})`. Unbekannte oder leere
Vorlage: Fehler an den Host beim Lauf, mit derselben Meldung (welche Vorlagen ein Level hat, weiß erst die laufende Welt; zur
Bearbeitungszeit ist jeder Knoten mit seinen Vorgaben gültig). Dazu der Knoten der Engine `GameWorld spawn` (aus dem Header).

**Rückgabe – der Akteur-Verweis:** `GameWorld::spawn` gibt `logic::ActorHandle` (die Id der Wurzel und die Welt,
`GameWorld::world_id()`; project.md 7.), der Knoten „Spawn Actor“ und der Knoten der Engine `GameWorld spawn` den Pin
`actor` – nie eine Zahl. Wer in C++ die Id braucht, liest `.id`.

## Eigenschaften setzen: typisiert, oder der Pfad mit Prüfung

- **Typisiert:** `GameWorld::set_material_color(id, color)` (Knoten „Set Color“): die Farbe des Mesh-Materials, 0..1 je
  Kanal (wie im Level). Fehler mit Grund: kein Akteur dieser Id, der Akteur hat kein Mesh.
- **Der Pfad** (`set_property(id, "components.light.color", value)`, Knoten „Set Property“) bleibt, weil er der Weg der
  Replikation ist (ein Server schickt jede Änderung als Pfad und Wert) und der allgemeine Knoten für jedes Feld. Er
  prüft und sagt den Grund, nie ein stilles Nichts: kein Akteur; ein leerer oder gesperrter Pfad (`id`, `parent`,
  `components.logic`); ein Pfad, den der Akteur nicht hat (die Meldung nennt die Schlüssel an der Stelle, an der er
  abbricht); ein Wert der falschen Art (eine Zahl für ein Feld mit Zahl – Text nur, wenn er ganz eine Zahl ist –, wahr/
  falsch für ein Feld mit Wahrheitswert; eine Zahl für einen Vektor setzt alle drei).

## Klänge einer Welt

`SoundBank::request_world(const GameWorld&)`: fordert jeden Klang an, den die Welt braucht – ihre Akteure **und ihre
Vorlagen** (was `spawn` macht, klingt sofort). So lädt ein Spiel die Klänge vor, ohne ein Level als Wert zu halten.
`GameAudio::begin` geht denselben Weg. `request_level(const Level&)` bleibt für den Editor (ein Level, das bearbeitet
wird, keine laufende Welt).

## Stand

- Gebaut: siehe Tests `world_templates_spawn`, `world_spawn_errors`, `world_set_property_says_why`, `sound_bank_request_world`
  (kern/tests/test_main.cpp); Murmel setzt seine Münzen aus der Vorlage „Muenze“ (Level-Graph, Begin Play); SpawnCity
  setzt seine Fahrzeuge (Vorlagen Drone, Jet, Car, Bus, Parachute) in `Game::start`.
- Offen: der typisierte Akteur-Verweis (Zweig work-projekt); Komponenten zur Laufzeit hinzufügen/entfernen (nicht
  nötig, solange Vorlagen sie tragen); `add_pawn` (Mitspieler) kopiert weiterhin den Spieler-Akteur der Welt – eine
  Vorlage „Spieler“ wäre der gleiche Weg, ist aber eine eigene Entscheidung (Besitz beim Start).
