# Die App-Laufzeit – was ein Spiel von der Engine bekommt

Entwurf und Stand (26.11.2026, Zweig work-grundlagen). Ein Spiel auf Kern schreibt seine App (`kern::App`, C++ oder
ein Programm-Blueprint, project.md 4.); die **App-Laufzeit** der Engine (`kern/app/runtime.h`, Plattform
`kern/web/app.mjs`) gibt ihr Bild, Eingabe, Ton, Speicher und Anzeige – ohne dass das Spiel dafür eine eigene Seite,
einen eigenen Audio-Host oder eigene Skripte schreibt. Je Sache ein Weg. Der Prüfstein ist das zweite Spiel Murmel
(kern/games/murmel): was es braucht, hat die Engine; was es um die Engine herum baute, ist entfernt.

## 1. Die App-Laufzeit als Objekt (`kern::AppRuntime`)

| | |
|---|---|
| C++ | `kern::AppRuntime rt(renderer)`: `give(app, settings)` (das ist `kern::run`), `start(w, h)`, `frame(now_ms, hidden)`, `frame_xr(…)`, `resize(w, h)`, `stop()`, `engine()`, `running()` |
| der Prozess | `kern::app_runtime()` ist die eine App-Laufzeit, die die C-Schnittstelle (`kern_app_*`) treibt; `kern::run(app)` gibt ihr die App |
| warum ein Objekt | damit ein Engine-Test eine App-Laufzeit für sich hat (eine App, die nicht startet; eine mit Ton …) – vorher gab es sie nur einmal je Prozess, und ein Test konnte nicht prüfen, was beim Start schiefgeht |
| bewusst nicht | zwei App-Laufzeiten auf einer Seite (die Plattform hat eine) |

**Start mit Grund (C2).** `start` ist false mit dem Grund in `last_error()` (die Seite zeigt ihn: „app start: …“):
- keine App: „no App: the program's kern_main() hands the engine its App with kern::run …“
- der Renderer startet nicht: „the renderer did not start: <Grund des Renderers>“
- `App::start` gibt false: „the App did not start: <Grund>“ – der Grund ist, was die App mit `kern::fail(…)` gesagt
  hat; sagt sie nichts: „its start() gave false and named no reason (an App says why with kern::fail)“. Ein Fehler von
  früher wird dabei nie als Grund gemeldet (der Start leert `last_error` vorher).
Geprüft: `app_runtime_start_says_why`.

## 2. Unbekannte Schlüssel sind Fehler (C3)

Ein Schlüssel, den der Leser eines Levels oder eines Projekts nicht kennt, ist ein **Fehler mit Ort und den erlaubten
Namen** – in jedem Objekt, das diese Leser lesen, nicht nur bei Komponenten:

```
actor "Sprungfeld": components.collider: unknown key - one of mesh, light, body, colliders, joints, …
world.gravty: unknown key - one of channels, gravity, substeps, …
settings.renderer.shadw: unknown key - one of shadows, shadow, bloom, ao, grade
```

- Ein Weg: `json::unknown_key(objekt, bekannte)` (kern/core/json.h) gibt die Meldung; der `Reader` des Levels merkt
  sich jeden Schlüssel, den er liest, und prüft am Ende (`Reader::done`). Was ein Leser nicht als Wert liest, aber kennt
  (`material`, `points`), nennt er ausdrücklich (`Reader::known`).
- Abgedeckt: das Level (oben), `world`, `camera`, jeder Akteur, `transform`, `components`, jede Komponente, `material`,
  jedes Element von `colliders`/`joints`, `textures`, `sounds`, `motions` (und ihre `keys`); das Projekt (oben),
  `settings`, `renderer`, `shadow`, `bloom`, `ao`, jedes Profil, jeder Eintrag von `blueprints`, und was diese Datei in
  späteren Abschnitten hinzufügt (`actions`, `ui`).
- Vorwärts: eine Datei einer neueren Engine sagt es über `version` („written by a newer Kern Studio“). Einen Schlüssel
  still zu übergehen, um alte Engines neue Dateien lesen zu lassen, gibt es nicht.
- Bewusst nicht hier: der Text eines Blueprints (`logic`) hat seinen eigenen Leser (kern/logic, `graph_from_json`) mit
  eigenen Meldungen.
Geprüft: `level_and_project_refuse_unknown_keys`.

## 3. Ton (A3)

Ein Spiel füllt nur seinen Mixer; hörbar macht ihn die App-Laufzeit – wie das Bild ohne Zutun des Spiels.

| | |
|---|---|
| C++ | `engine.mixer()` (`kern::audio::Mixer&`): der eine Mixer der App, beim Start der App bereit (`ready()`), mit der Rate des Geräts. `GameAudio`/`SoundBank` nehmen ihn: `GameAudio(engine.mixer(), bank)`. |
| Knoten | „Mixer“ an der Engine (`engine.engine.mixer`, aus der Tabelle der Engine): ein Programm-Blueprint holt ihn von seiner Engine und spielt darauf. |
| Plattform | `kern/web/audio.mjs` (`KernAudio`): ein AudioContext, ein AudioWorklet (ohne Worklet: ScriptProcessor), freigeschaltet bei der ersten Geste (Taste, Zeiger, Berührung); auf einer verborgenen Seite angehalten, danach weiter. `runKernApp` legt ihn an (Option `audio`, Vorgabe an) und zieht aus `kern_app_audio(frames)`. |
| C-Schnittstelle | `kern_app_audio_init(rate)` – vor `kern_app_start`, die Rate des Geräts (8000–192000); `kern_app_audio(frames, len*)` – die nächsten `frames` Frames, Stereo verschränkt (höchstens 8192 je Aufruf). Ohne `audio_init` (nativ, ohne Gerät) läuft der Mixer mit 48000 Hz, und wer will (ein Test), zieht mit `AppRuntime::audio(frames)`. |
| Fehler | eine Rate außerhalb 8000–192000 oder nach dem Start: `kern_app_audio_init` gibt 0, `last_error` sagt warum; zu viele Frames je Aufruf: 0 Frames und der Grund. Kein Gerät im Browser (kein AudioContext): die Seite meldet es einmal im Log, das Spiel läuft stumm weiter. |
| bewusst nicht | ein zweiter Mixer je App; Musik oder Ton in JavaScript (alles klingt aus dem Mixer); eine eigene Audio-Schleife je Spiel. |

Geprüft: `app_runtime_plays_its_mixer` (Engine-Test), Murmel (nativ: die Töne kommen aus `engine.mixer()`; e2e: die
Seite hat nach einer Taste einen laufenden AudioContext und zieht nicht-stille Proben).

## 4. Eingabe-Aktionen (B4)

Ein Spiel fragt nach dem, was der Spieler tut („jump“, „moveX“), nicht nach einer Taste. Welche Tasten, Knöpfe des
Gamepads, Maustasten und Finger das sind, steht als Daten im Projekt.

```json
"settings": { "input": {
  "actions": [ { "name": "jump", "keys": ["space"], "pad": ["a"], "mouse": [], "touch": "right" } ],
  "axes":    [ { "name": "moveX", "negative": ["left", "a"], "positive": ["right", "d"], "pad": "leftX", "deadzone": 0.2, "touch": "stickX" } ]
} }
```

| | |
|---|---|
| Daten | `kern/input/actions.h`: `InputActions` (`ActionBinding`, `AxisBinding`) in `ProjectSettings::input`, gelesen und geschrieben mit dem Projekt (`input_from_json`: jede Stelle mit Ort; unbekannte Schlüssel und Namen mit der Liste der erlaubten; ein Name doppelt – Aktionen und Achsen teilen ihre Namen – ist ein Fehler). |
| C++ | `engine.action("jump")` (gehalten), `engine.action_pressed("jump")` (in diesem Frame gedrückt), `engine.axis("moveX")` (−1 … 1: Tasten −1/+1, beide 0; der Stick hinter der Totzone neu auf 0 … 1 skaliert; Touch; der betragsgrößte Wert gewinnt). |
| Knoten | dieselben drei Funktionen der Engine (aus der Tabelle der Engine: „Action“, „Action Pressed“, „Axis“ mit dem Eingang Name). |
| Gamepad | `Input::gamepad()` (`Gamepad`: verbunden, `axes[4]` – links x/y, rechts x/y, oben/rechts +1 –, `lt`/`rt` 0 … 1, `buttons` als Bits der Standardbelegung). `kern/web/app.mjs` liest es jeden Frame (`readGamepad`, kern.mjs) und gibt es `kern_app_gamepad`. |
| Touch | ein Finger, der in der linken Hälfte beginnt, ist der Stick (`stickX`/`stickY`, oben und rechts +1; ein Radius – ein Achtel der kürzeren Seite des Bildes – ist 1); eine Aktion mit `touch` ist gehalten, solange ein Finger in ihrer Hälfte (`left`, `right`) oder irgendwo (`anywhere`) liegt, und gedrückt im Frame, in dem er aufsetzt (`Touch::began`). |
| Namen | `kern::Key` steht jetzt mit den Namen der Tasten, Knöpfe und Achsen in `kern/input/actions.h` (`key_name`, `parse_key`); `kern/app/app.h` nimmt sie von dort. |
| Fehler | ein Name, den das Projekt nicht hat: `action`/`axis` geben false/0, `last_error` nennt den Namen und die Namen, die es gibt, und das Log sagt es einmal je Name. |
| Editor | Projekt-Einstellungen, Abschnitt „Input“: jede Aktion und Achse mit ihren Bindungen, hinzufügen, ändern, entfernen (Befehl `projectSet` mit `settings.input`, dieselben Prüfungen wie beim Lesen). |
| bewusst nicht | Umbelegen durch den Spieler zur Laufzeit (die Daten sind das Projekt; ein späterer Schritt mit dem Speicher, 5.), gezeichnete Touch-Knöpfe (das ist UI, 6.), Kurven je Spielart. |

Geprüft: `input_actions_of_the_project` (Lesen/Schreiben, Fehler, Tasten, Pad, Touch, Maus). Murmel: `steuerung()` liest
nur noch `moveX`, `moveY`, `jump` aus seinem Projekt.
