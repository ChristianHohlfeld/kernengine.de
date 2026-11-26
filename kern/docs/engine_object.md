# Objekte der Engine als Wert: `Object<I>`

Entwurf und Stand (25.11.2026, Zweig work-sauber). Ein Blueprint hält ein Objekt der Engine (eine App, eine
Shader-Erweiterung, einen Transport, eine Welt der Physik …) als **Verweis eines eigenen Typs**, nicht als Zahl.

## Warum

Bis heute war jedes Objekt der Engine im Blueprint ein `Int` (sein Handle in `logic::EngineObjects`). Daraus folgten
drei Fehler: Ein `Int`-Eingang, der eine Shader-Erweiterung erwartet, nimmt jede Zahl (auch `7`) und jedes andere
Objekt; der Editor kann zur Bearbeitungszeit nichts prüfen; und `0` hieß zugleich „kein Objekt“ und „etwas ist schief
gegangen“ – `func.engine_object` gab bei vier verschiedenen Ursachen still `0`.

## Der Typ

| | |
|---|---|
| Pin-Typ | `object` (Liste: `object_list`), mit `of` = die Klasse oder Schnittstelle der Engine (jeder Pin nennt seine; es gibt keinen „Objekt irgendeiner Klasse“) |
| Name von `of` | die eine Regel der Engine-Namen in Blueprints: der C++-Name unter `kern::`, `::` wird `_` – `kern::App` ist `App`, `kern::physics::World` ist `physics_World`, `kern::logic::NodeType` ist `logic_NodeType` (dieselbe Regel wie für Structs der Engine und `implements`) |
| C++-Text | `Object<kern::App>`, `List<Object<kern::ShaderExtension>>` – der echte C++-Typ der Engine als Argument, kein Text |
| Wert | `logic::ObjectRef { i64 handle; u32 world; }` – das Handle in den Objekten **einer** Welt und die Nummer dieser Welt |
| kein Objekt | `Object<I>{}` (Handle 0, Welt 0): die Vorgabe jeder Variablen dieses Typs – nie das Ergebnis eines Fehlers |

- **Art** steht im Typ (`of`), nicht im Wert: ein `Object<App>` und ein `Object<ShaderExtension>` sind verschiedene
  Typen. Ein Draht geht nur zwischen gleichen Typen. Aus einem `Int` wird nie ein Objekt (kein Autocast, kein Literal
  außer `{}`).
- **Besitzer/Welt** steht im Wert: `logic::EngineObjects::world()` ist eine Nummer, die jede Sammlung von Objekten der
  Engine bekommt, wenn sie entsteht (ab 1, im Prozess eindeutig; wer zwei Seiten gleich aufsetzt, gibt ihnen dieselbe:
  `EngineObjects(world)`). Ein Verweis gilt nur in seiner Welt: `EngineObjects::get<T>(ObjectRef)` gibt null und sagt
  den Grund (`EngineObjects::error`), wenn der Verweis aus einer anderen Welt kommt, kein Objekt ist oder keins der
  Klasse `T`. Ein Handle kann so nie still das Objekt mit derselben Nummer in einer anderen Welt treffen.
- **Ungültig ≠ 0-Zufall:** „kein Objekt“ gibt es nur ausdrücklich. Wo die Engine ein Objekt nicht geben kann, ist das ein
  Fehler mit Ort und Grund (an den Host, `Host::error`), und der Ausgang ist „kein Objekt“.

## Knoten

| Knoten | C++-Text | |
|---|---|---|
| Engine Object of Blueprint | `func_engine_object<kern::App>(target)` | Einstellung `interface` (Vorgabe `App`, am Knoten zu sehen): das Objekt der Engine, das der Blueprint auf Target als diese Schnittstelle ist. Gibt `Object<I>`. Fehler mit Grund statt 0: Target hat keinen laufenden Blueprint · der Blueprint implementiert keine / eine andere Schnittstelle · der Host hat keine Objekte der Engine · der Build hat die Schnittstelle nicht. `check` meldet eine fehlende oder unbekannte Schnittstelle zur Bearbeitungszeit. |
| Is Valid Object | `object_valid<kern::App>(x)` | Bool: ist es ein Objekt (nicht `{}`)? Die Einstellung `class` ist jede Klasse der Engine mit Objekten (nicht nur Schnittstellen): `object_valid<kern::audio::Mixer>`. |
| Same Object | `object_equal<kern::App>(a, b)` | Bool: dasselbe Objekt (Welt und Handle gleich; keins und keins sind dasselbe) – wie C++ zwei Zeiger vergleicht. |
| Handle of Object | `object_handle<kern::App>(x)` | Int: das Handle in seiner Welt – zum Zeigen oder Zählen. Der eine ausdrückliche Übergang zu Int: keine Funktion der Engine nimmt oder gibt ein Objekt als Int. |

## Der Akteur-Verweis mit Welt

Gebaut nach demselben Muster (docs/project.md 7.): `Actor` ist `{ActorRef id; WorldId world;}`, `world` die Nummer der
`EngineObjects` der Welt, in der der Akteur lebt (`GameWorld` hat genau eine; `Host::world()`). „Objekt der Engine aus
Welt A“ und „Akteur aus Welt A“ sind dieselbe Frage: `world` vergleichen, dann das Handle / die Id in dieser Welt
nachschlagen (`EngineObjects::get`, `Runtime::actor_here`). Der Wert nutzt dafür das Feld `Value::world`. Ein Unterschied:
den Blueprint eines Akteurs einer Welt, die sein Programm fährt, erreicht ein Programm-Blueprint (`Runtime::runtime_of`,
project.md 7. „Vom Programm aus“); ein Objekt der Engine nennt nur in seiner eigenen Welt etwas.

## Stand

- Gebaut: Pin-Typ, Wert, Prüfung der Drähte, JSON, C++-Text lesen und schreiben, Klasse (`kern::logic::Object<I>`,
  `kern/logic/logic.h`; `func_engine_object<I>` in `kern/logic/blueprint.h`), Interpreter, Editor (Pin-Farbe, Name, nur passende Drähte), `func.engine_object` mit Gründen, die drei Knoten.
  Tests: `engine_object_of_a_blueprint` (Knoten, vier Gründe, C++-Text), `object_refs_know_their_world` (Drähte, Welt, Variable), Tierfall `on_actor` (jede Stufe).
- Listen: `List<Object<I>>` (`object_list`, Typtext `object<App>_list`, Einträge `ObjectRef`); die Knoten der Listen nehmen
  Objekte jeder Schnittstelle als „Type“ (Make List, Get, Add, Sort – sortiert nach Welt, dann Handle). Geschrieben
  steht eine Liste nur ohne Objekte (`List<Object<kern::App>>{}`); ihre Objekte gibt die Engine.
- Die Tabelle der Engine (`tools/engine_api.py`, Stand work-object): jedes Objekt der Engine an einem Knoten ist ein
  `Object<Klasse>` (Pin `object`/`object_list`, Form `o` – eines oder keins – und `h` – was ein Maker gibt, nie keins).
  Die Tür (`engine_host.h`, `Conv<Handle>`) und die Klasse (`engine_direct.h`, `Direct<Handle>`) lesen den Verweis mit
  der Prüfung der Welt: ein Verweis einer anderen Welt, keiner Objekts oder einer anderen Klasse lässt den Aufruf
  scheitern, mit dem Eingang und dem Grund (`EngineObjects::error`, an den Host); ein `Int` an einem Objekt-Eingang ist ein
  Fehler, kein Handle. Steht ein Objekt in einem Wert (Feld eines Structs, Eintrag einer Liste), fragt der Aufruf vor der
  Engine (`object_in`). Der Stellvertreter gibt Objekte seiner eigenen Welt (`kStandInWorld`); Seiten eines Vergleichs
  haben eine Welt (`EngineObjects(world)`: `kNativeTestWorld`, `kTestWorld` der Vergleichstests).
- Der Konverter schreibt einen Zeiger, eine Referenz, ein `std::unique_ptr` der Engine als `Object<…>`: Nullzeiger
  `Object<…>{}`, `if (p)`/`p != nullptr` Is Valid Object, `a == b` Same Object, Container von Zeigern `List<Object<…>>`,
  `unique_ptr<…>::get()` das Objekt; ein Objekt wird nie eine Zahl (Fall tools/converter_cases/objects).
