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

