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
| Is Valid Object | `object_valid<kern::App>(x)` | Bool: ist es ein Objekt (nicht `{}`)? Die Schnittstelle ist eine Einstellung wie oben. |
| Handle of Object | `object_handle<kern::App>(x)` | Int: das Handle. Der ausdrückliche Übergang, solange die Funktionen der Engine ihre Objekte noch als `Int` nehmen (siehe Stand). |

## Für den Akteur-Verweis mit Welt (Zweig work-projekt)

Ein Akteur-Verweis, der seine Welt trägt, nimmt dieselbe Welt-Nummer: `{u32 world; ActorRef id;}`, wobei `world` die
Nummer der `EngineObjects` der Welt ist, in der der Akteur lebt (`GameWorld` hat genau eine). Damit sind „Objekt der
Engine aus Welt A“ und „Akteur aus Welt A“ dieselbe Frage: `world` vergleichen, dann das Handle / die Id in dieser Welt
nachschlagen. Der Wert hat dafür das Feld `Value::world` (für `object` gesetzt, sonst 0).

## Stand

- Gebaut: Pin-Typ, Wert, Prüfung der Drähte, JSON, C++-Text lesen und schreiben, Klasse (`kern::logic::Object<I>`,
  `kern/logic/logic.h`; `func_engine_object<I>` in `kern/logic/blueprint.h`), Interpreter, Editor (Pin-Farbe, Name, nur passende Drähte), `func.engine_object` mit Gründen, die drei Knoten.
  Tests: `engine_object_of_a_blueprint` (Knoten, vier Gründe, C++-Text), `object_refs_know_their_world` (Drähte, Welt, Variable), Tierfall `on_actor` (jede Stufe).
- Listen: `List<Object<I>>` (`object_list`, Typtext `object<App>_list`, Einträge `ObjectRef`); die Knoten der Listen nehmen
  Objekte jeder Schnittstelle als „Type“ (Make List, Get, Add, Sort – sortiert nach Welt, dann Handle). Geschrieben
  steht eine Liste nur ohne Objekte (`List<Object<kern::App>>{}`); ihre Objekte gibt die Engine.
- Offen (eigener Schritt, groß): die Tabelle der Engine (`tools/engine_api.py`) beschreibt ihre Objekte noch als `Int`
  (Form `h`/`i`). Wenn sie `Object<Klasse>` schreibt, entfällt `object_handle`, und der Konverter schreibt einen Zeiger
  der Engine als `Object<…>` statt `Int`. Bis dahin ist `object_handle` der eine, ausdrückliche Übergang.
