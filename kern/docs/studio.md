# Kern Studio

Kern Studio ist der Editor der Kern-Engine. Er arbeitet wie eine Game Engine im Stil der Unreal Engine: Levels aus Actors mit Komponenten, Viewports mit Unreal-Navigation, Transform-Gizmos, Outliner und Details-Panel. Play und Simulate laufen direkt im Viewport. Rendern, Box3D-Physik, Steuerung und Spielregeln laufen im selben C++-Code, mit dem auch exportierte Spiele laufen.

- Live: https://kernengine.de/editor/
- Bauen: `tools/build_studio.sh` → `generated/studio.html` (eine Datei, läuft offline)
- Entwickeln: Repository statisch ausliefern, `kern/web/studio/index.html` öffnen (lädt `generated/kern_studio.wasm`)

**Sprache:** Auf Systemen und Browsern mit Deutsch als erster Sprache zeigt Studio seine Oberfläche auf Deutsch, auch die Spiele (HUD, Touch-Knöpfe). Das ist rein optisch: Befehle, Konsole, Level-Dateien, Logik-Graphen und alles, was man selbst eingibt, bleiben unverändert. Der Knopf **DE / EN** unten rechts in der Statusleiste schaltet um und merkt sich die Wahl; `?lang=en` oder `?lang=de` in der Adresse tut dasselbe (`game.mjs`, Abschnitt „German interface“).

Beim Start ist **Orb Run** geladen. Das ist eine prozedurale Stadt mit Verkehr, einem Box3D-Hovercar, 24 Energie-Orbs und einem Tag, der in die Nacht übergeht. Mit **Play** (Alt+P) fährt man sofort los.

## Aufbau

```
kern/include/kern/game/   Level-Modell, Geometrie, Laufzeit, Demo-Levels
kern/src/game/            level.cpp (Modell + .klevel), geometry.cpp (Formen, Stadt),
                          runtime.cpp (GameWorld: Darstellung, Box3D, Controller, Regeln), demos.cpp
kern/include/kern/studio/ studio.h - der Editor (ohne Browser testbar)
kern/src/studio/          studio.cpp (Dokument, Historie, Auswahl, Actor-Operationen, Play)
                          viewport.cpp (Kameras, Navigation, Picking, Rendern)
                          gizmo.cpp (Move/Rotate/Scale), overlays.cpp (Grid, Outline, Icons, Debug-Linien)
                          details.cpp (Property-Schema, Details, World Settings)
                          catalog.cpp (Place Actors), ui.cpp (Tastatur, Menüs, Dialoge, Befehle)
                          api.cpp (kern_studio_* C-ABI)
kern/web/studio/          index.html, studio.css, studio.mjs (Editor-Seite), game.mjs (Spiel-Host + JS-Wrapper)
```

Die Seite enthält keine Editorlogik. Sie zeichnet die Panels aus dem JSON von `state()` und `status()`, leitet Eingaben weiter und erledigt, was nur ein Browser kann: Dateidialoge, Downloads, Zwischenablage, Pointer-Lock, Bilddekodierung, Autosave in `localStorage` und das Schreiben exportierter Spiele.

## Oberfläche

| Bereich | Inhalt |
|---|---|
| Menüleiste | File, Edit, Window, Play, Help |
| Toolbar | Speichern, Undo/Redo, **+ Add** (Place Actors), Werkzeuge Select/Move/Rotate/Scale, Welt-/lokale Achsen, Grid-, Rotations- und Skalierungsraster, Kamerageschwindigkeit, Play/Simulate/Pause/Frame/Stop/Eject, Layout, Package Game |
| Viewports | 1, 2 oder 4 (Perspektive, Top, Front, Side). Pro Viewport: Optionen, Ansichtstyp, View Mode (Lit, Unlit, Wireframe, Player Collision), Show-Flags, Maximieren |
| Outliner | Ordner, Hierarchie, Suche, Sichtbarkeit (Auge), Sperren, Drag-and-drop zum Anhängen, in Ordner verschieben oder Lösen; Doppelklick fokussiert, F2 benennt um |
| Details | Name, alle Komponenten der Auswahl (Mehrfachauswahl zeigt „Multiple“), **+ Add** für Komponenten, Suche, Zurücksetzen. Zahlenfelder lassen sich ziehen (Label oder Feld seitwärts ziehen, Shift ×10, Ctrl ×0,1) und rechnen (`2*3`). Der Farbwähler bleibt offen, solange man darin klickt und zieht: jede Farbe wirkt sofort, die ganze Sitzung ist ein Undo-Schritt |
| World Settings | Spieltitel, Siegbedingung, Zeitlimit, Kill-Z, Tageszeit, Tageslänge, Sonne, Nebel, Belichtung, Wolken, Boden (mit Reibung und Restitution), Schwerkraft, Substeps, Schrittrate, Kontakt-Steifigkeit, -Dämpfung und -Ausstoßgeschwindigkeit |
| Content Drawer (Ctrl+Space) | Place Actors (Klick oder Ziehen in einen Viewport), Texturen, Output Log |
| Statusleiste | Konsole (`help`), Statusmeldung, Physik-Statistik, Auswahl |

Beispiel-Levels (File › Example Levels und File › New Level): Orb Run, Physics Lab, Shooting Range,
Micro Duck, Basic, Empty.

**Menüs:** Mit der Maus klappen Untermenüs beim Überfahren neben dem Menü auf, nie darüber. Passt
daneben kein Platz, oder wird getippt (Touch, Stift), öffnet ein Tipp das Untermenü an Ort und
Stelle, mit einer Zurück-Zeile oben. So liegt nie ein Menü unter dem Finger, der es geöffnet hat.
Lange Menüs scrollen, und auf Touch sind die Zeilen 40 px hoch.

**Handy und kleine Fenster** (bis 900 px Breite, oder Touch mit bis 600 px Höhe):
- Der Viewport bekommt die ganze Fläche.
- Outliner, Details und World Settings öffnen sich über den Panels-Knopf rechts in der Toolbar als
  ein Blatt mit großen Tabs. Im Querformat erscheint es neben dem Viewport, im Hochformat von unten.
- Der Content Drawer belegt höchstens die halbe Höhe.
- **Play und Simulate sind immer mit einem Tipp erreichbar**, nie nur im Menü: Sie liegen zusammen mit
  dem Panels-Knopf in einem Dock am rechten Rand der Toolbar. Der Rest der Toolbar scrollt darunter
  durch, das Dock bleibt stehen – auch bei offenem Outliner/Details/World-Blatt, Content Drawer oder
  Logic-Editor. Während Simulate/Play stehen dort Pause, Stop und Eject (bzw. Play zum Übernehmen des
  Spielers); Knöpfe, die im aktuellen Zustand nichts tun, sind ausgeblendet, die übrigen haben
  Touch-Größe (mind. 38 px).
- Play auf Touch-Geräten füllt den ganzen Bildschirm; oben links stehen **EXIT** (Stop) und
  **PAUSE**/**RESUME**.

## Steuerung

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste + WASD / QE | Fly Mode; Mausrad währenddessen ändert die Geschwindigkeit (1-8), Shift schneller, Z/C zoomen |
| Linksklick | auswählen (Shift ergänzt, Ctrl schaltet um); Klick ins Leere hebt die Auswahl auf |
| Mit links ziehen (auch über einem Actor) | Rahmenauswahl (Shift ergänzt, Ctrl schaltet um); verschoben wird mit dem Gizmo |
| Rechte + linke Maustaste | im Fly Mode hoch/runter und seitwärts |
| Mittlere Maustaste ziehen | schwenken |
| Alt + links / rechts / Mitte | um die Auswahl kreisen / Dolly / schwenken |
| Mausrad | vor/zurück (orthografisch: Zoom zum Cursor) |
| F / End | Auswahl fokussieren / auf den Boden setzen |
| Q W E R / Leertaste | Select / Move / Rotate / Scale / durchschalten |
| Alt + Gizmo ziehen | beim Bewegen duplizieren |
| Aus „Place Actors“ ziehen | neuer Actor landet genau dort, wo losgelassen wird |
| Ctrl+Z / Ctrl+Y | Rückgängig / Wiederholen (Edit → History zeigt alle Schritte) |
| Ctrl+C / V / X / D, Entf | Kopieren, Einfügen, Ausschneiden, Duplizieren, Löschen |
| H / Shift+H / Ctrl+H | Auswahl verstecken / den Rest verstecken / alles zeigen |
| Ctrl+1..9 / 1..9 | Kamera-Lesezeichen setzen / anspringen |
| Alt+G / J / H / K | Perspektive / Top / Front / Side |
| Alt+2 / 3 / 4 / 5 | Wireframe / Unlit / Lit / Collision |
| G | Game View (Editor-Helfer aus) |
| [ / ] | Grid-Raster kleiner / größer |
| Alt+P / Alt+S | Play / Simulate |
| Esc / F8 / Shift+F1 | Stopp / Eject bzw. Possess / Maus freigeben |
| Ziehen auf einem Körper (Simulate, Eject, Beobachten) | ihn mit dem Mouse Joint greifen, ziehen, hochheben und werfen; Mausrad: näher / weiter; ein Klick wählt ihn aus |
| K (Simulate) | simulierte Posen der Auswahl übernehmen |
| Rechtsklick | Kontextmenü: Fokus, Umbenennen, Anhängen, mit Gelenk verbinden, Sperren, Actor platzieren |

## Levels, Actors und Komponenten

Ein Level (`.klevel`, JSON, `"format": "kern-level"`, Version 1) besteht aus World Settings, Actors, Texturen, Sounds, der Editor-Kamera und dem, was der Lehrer seinen Micro Ducks beigebracht hat (`motions`, siehe „Der Lehrer“). Einheiten sind Meter, Sekunden und Kilogramm, Z zeigt nach oben. Rotationen stehen als Euler-Winkel in Grad (x Roll, y Pitch, z Yaw), Farben als sRGB.

Ein Actor hat Name, Eltern-Actor, Outliner-Ordner, Hidden/Locked, Tags, Transform und diese Komponenten:

| Komponente | Bedeutung |
|---|---|
| Static Mesh | Box, Sphere, Cylinder, Cone, Capsule, Plane, Torus, Ramp mit PBR-Material (Farbe, Roughness, Metallic, Emissive, Opacity, Unlit, Schatten, zweiseitig, Textur mit Tiling); „Generate collision“ erzeugt einen passenden Collider |
| Point Light | Farbe, Intensität, Radius |
| Rigid Body (Box3D) | Static, Kinematic, Dynamic; Masse (0 = aus der Dichte), Dämpfung, Gravity Scale, Bullet, Sleep, Achsensperren, Startgeschwindigkeit; Aufprall-Sound mit Schwelle. Importierte Roboter tragen zusätzlich Schwerpunkt und Trägheitstensor (`centerOfMass`, `inertia` im Leveldokument) |
| Collider (mehrere) | Fit to mesh, Box, Sphere, Capsule, Convex Hull; Offset, Rotation, Dichte, Reibung, Restitution, Rollwiderstand, Sensor, Kanäle (Kategorie/Maske/Gruppe), Impact Sound (*auto* = der des Bodys; so klingt eine Sohle anders als ein Gehäuse) |
| Constraint (mehrere) | Hinge, Slider, Ball, Fixed, Distance/Rope, Wheel, Motor, Parallel, No Collision; Anker und Achse lokal, Limits, Motor, Feder, Ruhelänge, Bruchkraft und -moment. Hinge: Referenzwinkel der Baupose (`reference`); Servo mit Ziel, Steifigkeit, Dämpfung, Reibung und Haltemoment (`servo*`, siehe [`physics.md`](physics.md) Abschnitt 5) |
| Player Controller | Hover Car, Rolling Ball, Character, Flyer, Vehicle, Soldier, Walker; Geschwindigkeit, Beschleunigung, Wenderate, Sprung, Boost, Schwebehöhe, Kamera (Chase, First Person, Top Down); Motor-Loop, Sprung- und Boost-Sound |
| Pickup | Punkte, Partikelfarbe, Respawn, Sound |
| Trigger | Goal, Hazard, Checkpoint, Message; Sound (Automatic = passend zur Aktion) |
| Audio Source | Sound, Lautstärke, Pitch, Loop, Autoplay, Startverzögerung, Bus; 3D: innerer Radius, Falloff, Roll-off (Inverse, Linear, Exponential), Doppler, Pitch nach Geschwindigkeit |
| Rotator / Mover | Drehung bzw. Bewegung (Sinus, Ping-Pong, Schleife) |
| Procedural City | Seed, Blöcke, Raster, Straßenbreite, Höhe, Bäume, Verkehr, beleuchtete Fenster in der Nacht |

Constraints haben zusätzlich einen Bruch-Sound. Die World Settings enthalten unter „Audio“ Master-Lautstärke, Musik, Ambience (je mit Lautstärke), Hall und Doppler.

Kind-Actors ohne eigenen Body werden beim Simulieren als Compound in den Body des nächsten Vorfahren geschweißt. Meshes mit Kollision, Pickups und Trigger ohne Body bekommen einen impliziten statischen Body, bei Animation durch Rotator oder Mover einen kinematischen. Gelenke verbinden den Actor (Body B) mit dem Ziel-Actor (Body A, 0 = Welt). Ausnahme sind Wheel-Gelenke: Dort ist das Chassis Body A.

Fehler wie verwaiste Referenzen oder ungültige Werte verhindern Play; Warnungen erscheinen im Output Log.

## Place Actors

Shapes: Cube, Sphere, Cylinder, Cone, Capsule, Plane, Torus, Ramp, Empty Actor · Lights: Point Light · Physics: Physics Cube, Physics Sphere, Physics Barrel, Crate Stack, Pendulum, Hinged Door, Chain, See-Saw, Sock · Players: Soldier, Character, Hover Car, Rolling Ball, Drone, Micro Duck, Micro Duck (Rollers) · Gameplay: Orb, Star, Health Pack, Ammo Box, Goal, Hazard, Damage Zone, Checkpoint, Jump Pad, Teleporter, Message Trigger, Moving Platform, Spinner, Target Dummy, Explosive Barrel · Audio: Audio Source, Ambient Wind, City Ambience, Radio · World: Procedural City (Liste: `src/studio/catalog.cpp`).

## Sound

Die Sound-Engine ist C++ im Engine-Modul (`kern/src/audio`): ein Software-Mixer mit 64 Stimmen (Hermite-Resampling, Prioritäten, Voice Stealing, virtuelle Stimmen außerhalb der Hörweite), 3D-Klang am Listener der Kamera (Equal-Power-Panning, Roll-off, Luftdämpfung, Tiefpass hinter dem Hörer, Doppler), Bussen (Music, Effects, Ambience, Interface, Voice → Master) mit klickfreien Lautstärkerampen, Tiefpass und Hall-Send, einem Freeverb-Hall, Peak-Limiter und Soft-Clip. Die Ausgabe überschreitet nie ±1.

Eingebaut sind 38 synthetisierte Sounds (`builtin_sounds()`) und die Laute der Ente (Pickup, Jump, Impact, Kunststoff-Tippen, kleine Schritte, Explosion, Motor, Hover, Wind, Stadt, Servo, Quack, UI-Klicks und zwei Musik-Loops). Sie werden deterministisch und in Zeitscheiben gerendert, ein Level mit Musik ruckelt beim Laden nicht. Eigene Dateien (WAV, MP3, OGG, M4A, FLAC – was der Browser dekodieren kann) importiert man im Content Drawer unter **Sounds**, per Drag & Drop auf die Seite oder über das Plus am Sound-Feld; sie liegen als Data-URL im Level und werden per Windowed-Sinc auf die Ausgaberate gebracht.

Bedienung: Ein Klick auf einen Sound im Tab Sounds spielt ihn an, ein Doppelklick setzt ihn auf die Auswahl, Ziehen in den Viewport platziert eine Audio Source. Jedes Sound-Feld hat eine Vorhör-Taste. Ausgewählte Audio Sources zeigen innen den vollen und außen den Falloff-Radius. Die Statusleiste hat Stummschalter und Pegelanzeige.

Während Play und Simulate spielt das Spiel Musik, Ambience und Audio Sources, den Motor-Loop des Spielers (Pitch und Lautstärke folgen Tempo und Gas), Schritte, Landungen, Sprung, Boost, Pickups, Trigger, Sieg/Niederlage, Aufpralle von Bodies und brechende Gelenke. Die Ereignisse entstehen in den festen Physikschritten und sind deterministisch. Pause pausiert alle Stimmen.

Im Browser überträgt `kern/web/studio/game.mjs` (`KernAudio`) das PCM an die Lautsprecher: per AudioWorklet (Blob- oder Data-Modul), sonst per ScriptProcessor. Die erste Berührung, Taste oder der erste Klick startet den Ton (Browser-Regel, iOS eingeschlossen; `navigator.audioSession` steht auf „playback“). Exportierte Spiele haben denselben Ton.

Vorlagen unter File → New Level: Orb Run, Physics Lab (alle Gelenktypen: Stapel, Pendel, Tür, Seilbrücke mit Bruchstelle, Drehteller mit Motor, Lift, Rover auf Wheel-Gelenken), Basic und Empty.

## Spiele exportieren

**Nativer Export:** `tools/export_game.sh level.klevel spiel.html` schreibt dieselbe Seite wie Package Game, aber mit einem Engine-Modul, das für dieses Level gebaut ist – jeder Blueprint als seine C++-Klasse einkompiliert, voll optimiert (Stufe „native C++“ statt JIT; siehe [blueprint_cpp.md](blueprint_cpp.md)). Braucht wasi-sdk, einen C++-Compiler und Node.

File → Package Game schreibt eine eigenständige HTML-Datei. Sie enthält das Engine-Modul (base64), `kern.mjs`, `game.mjs` und das Level. Das Spiel läuft im Vollbild mit HUD, Maus-Look per Pointer-Lock und Touch-Steuerung (Joystick, Sprung, Boost). Es braucht keinen Server und kein Netz.

## Der Lehrer

Der Lehrer bringt der Micro Duck neue Bewegungen bei (`docs/lehrer.md`, `docs/keyframe-lehrer.md`,
`docs/szenarien.md`). Er ist ein Arbeitsbereich des Studios, keine eigene Seite: Knopf **Lehrer** in
der Werkzeugleiste, Window → Lehrer oder die Adresse `…/editor/?train=1` (die früheren Adressen
`/lehrer/` und `/train/` leiten dorthin). Esc, derselbe Knopf oder das Kreuz im Panel führen zurück.

- **Bühne:** Der Arbeitsbereich simuliert ein eigenes kleines Level in der GameWorld des Studios
  (`src/studio/trainer.cpp`); das offene Level steht still, Viewport, Werkzeug und Auswahl kommen beim
  Schließen so zurück, wie sie waren. Posieren: Ente schwerelos, Rumpf fest, Glieder mit Box3Ds
  Maus-Gelenk ziehen (Maus oder Finger), sie halten, wo man loslässt. Das Gelenk des geführten
  Glieds bewegt sich dabei gegen Gelenkreibung (ein Gelenkmotor mit Tempo 0 und begrenztem Drehmoment: ein
  Zehntel dessen, womit die Hand das Gelenk höchstens drehen kann): es bleibt stehen, wo die Hand stehen bleibt. Das gilt nur beim Posieren – Training, Testen und Spiel haben das
  unveränderte Servo-Gesetz. Testen: Ente bei normaler Schwerkraft auf ihrem Trainingsboden, sie
  spielt eine gelernte Bewegung.
- **Kamera der Bühne:** die des Studios mit denselben Bedienungen und Tempo-Stufen, im Maßstab der
  Ente: ein Vierzigstel der Wege im Level (Stufe 4: ein Rad-Klick 25 cm statt 10 m).
- **Panel** (`web/studio/trainer.mjs`, anstelle von Outliner und Details; auf dem Telefon das Blatt
  über der Bühne): Aufgabe (Bewegung aus Keyframes, Sprung, Stehen, freies Szenario), Keyframes,
  Start / Pause / Stopp, Experteneinstellungen, Fortschritt mit Kurve, „Bewegungen dieses Levels“.
- **Training:** in Web Workern der Seite, je Worker ein `kern_lehrer.wasm` (Sammler und ein Lerner).
  Es läuft weiter, während man bearbeitet, spielt oder den Arbeitsbereich verlässt; der Knopf in der
  Werkzeugleiste zeigt den Fortschritt.
- **Ergebnis im Level:** Eine Bewegung (Name, Länge, Keyframes, trainiertes Netz) steht im Level unter
  `motions` und ist Teil seines Verlaufs (Undo / Redo, auch im Arbeitsbereich). Speichern, Öffnen,
  Package Game und der native Export tragen sie mit; beim Start einer Welt werden sie zu Bewegungen
  der Enten des Levels, und der Blueprint-Knoten Robot Move spielt sie unter ihrem Namen.

| Befehl | Wirkung |
|---|---|
| `trainer {open?}` | Arbeitsbereich öffnen / schließen |
| `trainerPose`, `trainerCapture {time}`, `trainerShow {index}` | gemessene Gelenkwinkel; Pose als Keyframe; Servos fahren in einen Keyframe |
| `trainerMotion {name?, duration?}`, `trainerKey {index, time \| remove}`, `trainerClear` | die Bewegung, die gerade gelehrt wird |
| `trainerSpread` | alle Keyframes in gleichen Abständen über die Länge der Bewegung |
| `trainerNew`, `trainerSelect {name}`, `trainerRemove {name}` | neue Bewegung; eine des Levels bearbeiten; entfernen |
| `trainerTask` | die Bewegung als Aufgabe des Lehrers (`motion:{…}`) |
| `trainerLearned {name, onnx, run}` | ein trainiertes Netz (Base64) ins Level; ein Lauf (`run`) ist ein Verlaufsschritt |
| `trainerTest {move}`, `trainerStage` | Bewegung auf der Bühne testen; zurück zum Posieren |
| `trainerPlay {move}` | das Level simulieren, seine Micro Duck spielt die Bewegung |
| `trainerNet {name}`, `trainerImport {motions}` | Netz einer Bewegung (Base64); Netze von woanders als Bewegungen ins Level |

`state().trainer` (nur im Arbeitsbereich): `stage` (`pose` / `test`), `motion` (`name`, `duration`,
`keys`, `saved`, `trained`), `testing`. `state().motions`: die Bewegungen des Levels (`name`,
`duration`, Zahl der `keys`, `bytes` des Netzes).

## Protokoll

Die Seite spricht den Editor über die C-ABI in `kern/src/studio/api.cpp` an:

| Funktion | Zweck |
|---|---|
| `kern_studio_init(w, h, player_only)` | Renderer (Schatten, Bloom, NightWindows) und Studio; `player_only` für exportierte Spiele |
| `kern_studio_frame(now_ms)` | Kameras, Simulation, Overlays, Rendern aller sichtbaren Viewports |
| `kern_studio_set_viewport(i, x, y, w, h, css_w, css_h, dpr, visible)` | Viewport-Rechteck im Drawing Buffer (Ursprung unten links) |
| `kern_studio_pointer(i, type, x, y, button, buttons, mods, wheel)` | Zeiger (0 down, 1 move, 2 up, 3 cancel, 4 wheel, 5 leave) |
| `kern_studio_key(key, code, down, mods, repeat)` | Tastatur, Antwort `{"handled": bool}` |
| `kern_studio_look(dx, dy)` | Maus-Look bei Pointer-Lock |
| `kern_studio_command(json)` | Befehl, Antwort `{"ok": …}` |
| `kern_studio_state()` / `kern_studio_status()` | Panel-Zustand bzw. Werte pro Frame (HUD, Physik, Kamera, Marquee, Gizmo) |
| `kern_studio_revision()` | ändert sich, wenn `state()` neu gelesen werden muss |
| `kern_studio_take_ui()` | Aufträge an die Seite: `capturePointer`, `releasePointer`, `clipboard`, `readClipboard`, `popup`, `dialog`, `save`, `saveAs`, `open`, `importTexture`, `packageGame`, `toggleDrawer`, `toggleLog`, `fullscreen`, `rename` |
| `kern_studio_pending_textures()`, `…_texture_source(id)`, `…_texture_pixels(id, w, h, rgba)` | Texturen dekodiert die Seite |
| `kern_studio_take_autosave(now)`, `kern_studio_take_autosave_discard()`, `kern_studio_dirty()` | Autosave (siehe [Wiederherstellen](#wiederherstellen-restore-from-last-session)) |
| `kern_studio_audio_init(rate)`, `kern_studio_audio_render(frames)` | Mixer starten, Stereo-PCM ziehen |
| `kern_studio_pending_sounds()`, `…_sound_source(id)`, `…_sound_pcm(id, pcm, frames, channels, rate)`, `…_sound_failed(id, reason)` | importierte Sounds dekodiert die Seite |

Befehle (`{"cmd": …}`):

- Auswahl: `select {ids, add, toggle, focus}`, `selectRange {id}`, `selectAll`, `deselect`, `selectChildren`, `selectFolder {folder}`
- Actors: `spawn {item, at | viewport+x+y, sound}`, `delete`, `duplicate`, `copy`, `cut`, `paste`, `pasteText {text}`, `rename {id, name}`, `attach {ids, parent}`, `moveTo {ids, parent | folder}`, `setFolder {folder}`, `renameFolder {from, to}`, `group`, `snapToFloor`, `hide`/`show {ids | folder}`, `showAll`, `lock {value}`, `focus {ids}`, `connect {kind}`
- Properties: `set {path, value}` (z. B. `transform.position`, `components.mesh.material.color`, `components.joints.0.lower`), `setWorld {path, value}`, `addComponent {type}`, `removeComponent {type, index}`, `importTexture {name, src, width, height}`, `removeTexture {id}`
- Sound: `previewSound {sound, volume, pitch}`, `stopSound`, `importSound {name, src, duration}`, `removeSound {id}`, `renameSound {id, name}`, `mute {muted}`
- Hilfen: `project {id | point, viewport}` liefert die Bildschirmposition
- Historie: `undo`, `redo`, `historyJump {index}` (aufeinanderfolgende Änderungen derselben Eigenschaft werden zu einem Schritt zusammengefasst)
- Play: `play {mode: play|simulate}`, `stop`, `pause`, `stepFrame`, `eject`, `restart`, `keep`, `gameInput {moveX, moveY, up, jump, boost}`, `pointerCaptured {value}`
- Werkzeuge und Ansichten: `tool`, `space`, `snap`, `cameraSpeed`, `layout`, `maximize`, `activeViewport`, `viewType`, `viewMode`, `showFlag`, `gameView`, `fov`, `bookmark {slot, set}`
- Panels: `filter`, `collapse {key}`, `section {id, closed}`, `menu {name}`, `dialog {name}`, `submitDialog {name, values}`, `catalog`, `clearLog`, `ui {action}`
- Dokumente: `new {template}`, `open {text, name}`, `restore {text, name}`, `save`, `saved`, `packageText`, `validate`, `console {text}`

## Einstellungen

Studio merkt sich die Einstellungen des Nutzers im Browser (`localStorage`, `kern-studio.settings.v1`,
Format `{version: 1, values: {…}}`):

- **Was dazugehört:** Preferences (Kamerageschwindigkeit, Mausempfindlichkeit, Invert Y, Orbit um die
  Auswahl), Viewport-Layout, Grid-/Rotations-/Skalierungsraster, Welt-/lokale Achsen, Ton an/aus und pro
  Viewport Ansichtstyp, View Mode, Sichtfeld und alle Show-Flags. Die Seite speichert außerdem
  Panel-Größen und den Content Drawer (`kern-studio.layout.v1`).
- **Nur eigene Entscheidungen:** Die Engine (`kern/src/studio/settings.cpp`) liefert mit
  `take_settings()` nur die Einstellungen, die der Nutzer selbst geändert hat (per Menü, Toolbar,
  Tastenkürzel, Dialog) oder die aus dem Speicher geladen wurden. Was nie angefasst wurde, wird nicht
  gespeichert und folgt den Standardwerten des jeweiligen Builds. Ändert sich ein Standardwert (z. B.
  Show › Constraints ist jetzt **aus**), gilt er für alle, die diese Einstellung nie geändert haben.
- **Wann gespeichert wird:** alle 0,5 s, wenn sich etwas geändert hat, und bei `pagehide` bzw. beim
  Wechsel in den Hintergrund. Unbekannte Schlüssel und ungültige Werte im Speicher werden beim Laden
  übergangen. Dass Multiplayer-Play von selbst auf vier Viewports schaltet, gilt nicht als Wahl.
- **Auf Standard zurücksetzen:** Edit › **Reset Settings to Defaults…** fragt im Studio-eigenen Dialog
  nach (kein Browser-Dialog) und setzt alle Einstellungen, Panel-Größen und den Content Drawer zurück;
  der gespeicherte Stand wird gelöscht. Levels bleiben unberührt.
- Befehle: `settings` (aktuelle Werte und Standardwerte), `applySettings {values}`, `takeSettings`,
  `resetSettings`.

## Wiederherstellen („Restore from last session“)

Beim Start bietet Studio **Restore "…" from your last session? · Restore / Dismiss** nur an, wenn die
letzte Sitzung ungespeicherte Arbeit zurückgelassen hat. Ein normales Neuladen eines Editors ohne
ungespeicherte Änderungen zeigt nichts an.

- **Wann gespeichert wird:** Die Seite hält höchstens eine Kopie in `localStorage`
  (`kern-studio.autosave.v1`). Die Engine entscheidet: `take_autosave(now)` liefert den Level-Text nur,
  wenn eine Änderung fällig ist (`autosave_seconds`, 2 s) und das Dokument ungespeicherte Änderungen hat
  (`dirty()`).
- **Wann sie gelöscht wird:** Gehört die gespeicherte Kopie zum aktuellen Dokument und ist nichts mehr
  ungespeichert, meldet `take_autosave_discard()` das genau einmal, und die Seite löscht die Kopie. Das
  passiert nach dem Speichern und wenn die Änderungen bis zum gespeicherten Stand rückgängig gemacht
  wurden. **Dismiss** löscht die Kopie ebenfalls.
- **Anderes Level öffnen oder neu anlegen:** Die Kopie der ungespeicherten Änderungen des vorigen Levels
  bleibt erhalten, bis das neue Level eigene Änderungen bekommt. Sie wird dann ersetzt.
- **Restore** lädt die Kopie mit dem Befehl `restore`. Das Level bleibt ungespeichert (Punkt am Tab,
  Rückfrage beim Verlassen), denn sein Inhalt steht in keiner Datei. Bis zum Speichern wird es bei
  jedem Start wieder angeboten.
- **Seite verlassen:** Bei `beforeunload`, `pagehide` und beim Wechsel in den Hintergrund
  (`visibilitychange`) schreibt die Seite eine fällige Kopie sofort bzw. löscht eine veraltete. So kommt
  auch auf Mobilgeräten und beim Update-Neuladen (siehe [Als App installieren](#als-app-installieren-android-ios-desktop))
  nichts abhanden.
- **Absturz oder nicht sauber beendet:** Die Kopie wird spätestens 2 s nach jeder Änderung geschrieben.
  Stürzt der Tab ab oder wird er beendet, ohne dass gespeichert wurde, liegt sie beim nächsten Start
  noch da und wird angeboten. War beim Absturz nichts ungespeichert, gibt es auch nichts anzubieten.
- **Nicht gespeichert, solange Play läuft:** Während Play/Simulate wird nichts geschrieben. Eine fällige
  Kopie folgt nach Stop.

## Viewport mit den Fingern

Im Editor und in Simulate steuert man die Kamera wie in einem 3D-Betrachter am Handy
(`Studio::free_touch_pointer`):

- **Ein Finger** dreht die Kamera um den Punkt unter dem Finger (oder um die Auswahl, wenn
  „Orbit around selection" an ist). In Draufsicht und Seitenansichten verschiebt er die Ansicht.
- **Zwei Finger** zoomen zum Punkt zwischen ihnen (auseinander näher, zusammen weiter weg) und
  verschieben die Ansicht mit sich: Was unter den Fingern liegt, bleibt unter ihnen. Der Maßstab ist
  der Abstand zu diesem Punkt, deshalb fühlt sich das an einer Socke wie an einem Häuserblock gleich an.
- **Tippen** wählt aus, **zweimal tippen** holt die Auswahl ins Bild (wie F), **lange halten** öffnet
  das Kontextmenü.
- Ein Finger auf einem **Gizmo**-Griff verschiebt, dreht oder skaliert wie mit der Maus.
- Ein Finger in einem anderen Viewport macht diesen zum aktiven; die Seite zoomt beim Pinch nie mit.
- Test: `studio_touch_navigates_the_editor_viewport`.

## Beobachten und Anfassen (Observer-Kamera)

Mit **World Settings → Observer camera** startet Play nicht hinter dem Spieler, sondern mit einer
freien Kamera: Das Spiel läuft, man fliegt herum und schaut zu. Das Beispiel-Level Micro Duck nutzt
das.

- **Desktop:**
  - WASD fliegt, Q / E fliegt hoch / runter, Shift fliegt schneller (ohne gehaltene rechte Maustaste).
  - Ziehen ins Leere schaut sich um; rechte Maustaste und Mausrad wie im Editor.
- **Handy:**
  - Ein Finger ins Leere schaut sich um.
  - Zwei Finger zoomen und schwenken wie im Editor (siehe [Viewport mit den Fingern](#viewport-mit-den-fingern)).
- **Anfassen:** Jeden Körper (dynamischer Box3D-Body) greift man mit dem Mouse Joint, mit der Maus
  wie mit dem Finger, auch in Simulate und nach Eject (Ctrl ist nicht mehr nötig).
  - Ein Motor-Joint mit Feder (7,5 Hz, kritisch gedämpft) zieht den gegriffenen Punkt zum Zeiger.
    Der Körper bleibt ganz in der Physik: Er stößt an, kippt, schwingt und fliegt beim Loslassen
    mit seinem Schwung weiter.
  - Den Zeiger nach oben ziehen hebt ihn hoch.
  - Mausrad oder ein zweiter Finger (auseinander / zusammen) holt ihn näher oder schiebt ihn weg.
  - Ein kurzer Klick oder Tipp wählt den Körper aus.
- **Spieler übernehmen:** F8 übernimmt den Spieler (z. B. die Ente per Gamepad), F8 noch einmal
  beobachtet wieder. Im exportierten Spiel gibt es beim Beobachten keinen Pointer-Lock.
- **Tests:** `studio_observer_camera_grabs_bodies` (Fliegen, Maus und Finger heben die Kiste, Mausrad,
  Pinch, F8), `studio_simulate_drags_bodies_without_ctrl`, E2E am Handy (der Ball wird mit dem Finger
  angehoben)

## Tests

- `kern/tests/run.sh studio`: Start mit Orb Run, Fly- und Orbit-Navigation, jedes Katalog-Element platzieren und simulieren, Gizmo-Drag mit Undo/Redo und Alt-Duplikat, Tastenkürzel, Befehle, Menüs, Dialoge, Outliner-Hierarchie und Ordner, Play mit F8 und Esc, Keep Simulation Changes, Speichern/Laden, Texturen, Autosave (nur bei ungespeicherten Änderungen, Löschen nach Speichern und Undo, `restore`), Einstellungen (Standardwerte, nur eigene Entscheidungen, Laden, Zurücksetzen), Packaging, Player-Modus
- `kern/tests/e2e/studio_e2e.mjs`: Browser-Test (Playwright) der Single-File-Version: Menüs, Fly Mode mit rechter Maustaste und WASD, Werkzeuge, Outliner, Details-Bearbeitung mit Undo, Content Drawer (Klick und Drag-and-drop), Kontextmenü, Play mit HUD und Fahren, Eject, Simulate, Quad-Layout, neues Level aus Vorlage, Hilfe, exportiertes Spiel, Wiederherstellen (Neuladen ohne Änderung: kein Angebot; mit ungespeicherter Änderung: Angebot, Restore, Dismiss), Einstellungen (Constraints standardmäßig aus, Speichern und Neuladen, Reset im Studio-Dialog), Handy hoch und quer (Play/Simulate/Pause/Stop in jedem Zustand mit einem Tipp erreichbar), Level öffnen auf Android über den Dateidialog, Farbwähler bleibt beim Ziehen offen, Micro Duck am Handy beobachten und den Ball mit dem Finger anheben, Modul-Version und schmales Fenster

## Logic: Blueprints, die C++ sind

Jeder Actor und das Level können einen Logic-Graphen tragen: Events, Flow, Variablen,
Funktionen, State Machines, Mathe (auch als Formel) und rund 70 Gameplay-Knoten. Geöffnet wird er über **Logic** in der Toolbar
oder in Details.

Der Blueprint-Editor folgt Unreals Blueprint-Editor (`web/studio/logic.mjs`) und deckt das ganze Fenster ab:

| Bereich | Inhalt |
|---|---|
| Toolbar | Viewport (zurück), **Compile** (F7, mit Status ✓ / ⚠ / ✖), Save, Find (Ctrl+F), welcher Blueprint (Level oder Actor), Vorlagen, **C++** (der Blueprint als C++: ansehen, ändern, anwenden) |
| My Blueprint | Graphen (Event Graph, State Machines, +), Variablen (Typfarbe, Live-Wert beim Spielen, Get/Set, ziehen: Ctrl = Get, Alt = Set), Funktionen (Signatur, Call / Return, ziehen: ein Aufruf), Events (Custom Events: Event / Call), Enums (die Aufzählungen des Blueprints; + legt eine an), Structures (die Strukturen des Blueprints; + legt eine an, die Knöpfe Make und Break setzen ihre Knoten in den Graphen) |
| Graph | Tabs, Breadcrumb, Zoom-Stufen wie Unreal („Zoom 1:1“, „Zoom −2“), Wasserzeichen EVENT GRAPH |
| Details | ausgewählter Knoten (Hilfe, Einstellungen, alle Eingänge, Kommentar), Variable (Name, Typ – auch Listen, Aufzählungen und Strukturen –, Default, Referenzen), Funktion (Name, Pure, Beschreibung, Ein- und Ausgänge mit Typ und Default), Aufzählung (Name, Beschreibung, Werte), Struktur (Name, Beschreibung, Felder mit Typ und Default), Kommentar (Text, Farbe), State, Transition |
| Compiler Results / Find Results | Probleme und Suchtreffer, ein Klick springt zum Knoten |

Im Graphen:
- **Knoten:**
  - Titelleisten nach Art: Event rot, Funktion blau, pure grün, Flow grau.
  - Eingaben ohne Draht direkt am Pin.
  - Variablen-Getter und Operatoren sind kompakte Knoten: Name bzw. Symbol (+, −, ×, ÷, <, AND …).
- **Drähte:**
  - Unreal-Splines, farbig nach Typ, Exec weiß und dicker.
  - Drähte ausgewählter Knoten werden hervorgehoben.
  - Beim Spielen laufen Impulse über die Exec-Drähte, die gerade feuerten.
- **Verdrahten:**
  - Einen Pin ziehen, passende Pins leuchten auf.
  - Ins Leere fallen lassen: das kontextsensitive Aktionsmenü mit „Add Reroute Node“.
  - Ctrl+Ziehen nimmt die Drähte eines Pins auf und hängt sie um.
  - Alt+Klick trennt.
  - Doppelklick auf einen Draht setzt einen Reroute-Knoten.
- **Rechtsklick (Touch: langer Druck):**
  - Leere Fläche: „All Actions for this Blueprint“ mit aufklappbaren Kategorien und Suche.
  - Knoten: Ausschneiden/Kopieren/Duplizieren, Links trennen, Kommentar, Ausrichten/Verteilen, Breakpoint.
  - Pin: Links trennen, „Promote to Variable“, Default zurücksetzen.
- **Kommentare (C):**
  - Ein Rahmen um die Auswahl; verschieben verschiebt den Inhalt mit.
  - Ecke ziehen ändert die Größe, Doppelklick auf den Titel benennt um, die Farbe kommt aus Details.
- **Tastatur:**

  | Taste | Wirkung |
  |---|---|
  | Q | Verbindungen geraderichten |
  | Shift+W/A/S/D | Ausrichten |
  | B / S / D / N / O / G / L + Klick | Branch / Sequence / Delay / Do N / Do Once / Gate / For Loop |
  | Ctrl+C/X/V/D/W | Kopieren / Ausschneiden / Einfügen / Duplizieren |
  | F | Auswahl einrahmen |
  | Home | alles einrahmen |
  | Ctrl+Z | Rückgängig (jede Änderung ist ein Schritt) |
- **Debugging:**
  - F9 setzt einen Breakpoint (roter Punkt).
  - Läuft der Knoten beim Spielen, hält das Spiel nach diesem Simulationsschritt an.
  - Der Editor springt zum Knoten; Resume, Step und Clear Breakpoints stehen oben im Graphen.
  - Variablen zeigen ihre Live-Werte.
- **Handy:**
  - Ein Finger verschiebt, zwei Finger zoomen, ein langer Druck ist der Rechtsklick.
  - Ein Tipp auf einen Pin zeigt die passenden Knoten.
  - Der +-Knopf öffnet das Aktionsmenü als Sheet.
  - My Blueprint und Details sind Schubladen, ein Kasten-Knopf schaltet die Rechteckauswahl ein.
- **Vorlagen:** Tür, Punktezone, Spawner, Ampel.

### Blueprints als C++

Jeder Blueprint hat genau einen C++-Text und jeder solche Text ist genau ein Blueprint
(`logic::to_cpp`, `logic::from_cpp`; die Schreibweise, ihre Regeln und der Beweis stehen in
[blueprint_cpp.md](blueprint_cpp.md)). **C++** in der Toolbar öffnet diese zweite Ansicht: Der
Text folgt dem Graphen, und ein geänderter Text wird mit **Apply** (Ctrl+Enter) zum Blueprint –
ein Undo-Schritt; Knoten, die es noch gibt, bleiben, wo sie sind. Deshalb sind die Namen von Variablen, Funktionen und
Funktions-Pins C++-Namen: Buchstaben, Ziffern und `_`, keine Leerzeichen, kein Wort von C++ und
keins der Schreibweise (`Float`, `Text`, `loop` …). Der Editor nimmt andere Namen nicht an und
sagt, warum; `check()` meldet einen Graphen, den C++ nicht sagen kann, als Fehler – auch einen
Exec-Ausgang mit zwei Drähten, ein Return außerhalb des Wegs seiner Funktion, einen Zufallswert,
den zwei Eingänge lesen, oder einen Weg, der ohne Warten in sich selbst zurückführt (dafür gibt
es die Schleifen-Knoten).

Jeden Blueprint führt ein C++-Compiler als genau diesen Text aus (`kern/logic/blueprint.h`) –
auch seine Gameplay-Knoten: Actors, Physik, Kampf, Effekte, Roboter. Graph und Text rufen
dieselben Funktionen auf (`kern/logic/nodes.h`, `kern/game/blueprint_nodes.h`). Die Ansicht zeigt
das neben dem Namen an („runs as compiled C++“). **Download**
gibt die Quelle, die ein Spiel-Build dafür kompiliert – die Klasse des Blueprints samt dem
Kleber, über den die Engine sie beim Namen erreicht; `tools/blueprint_cpp.sh level.klevel out.cpp`
schreibt dasselbe für alle Blueprints eines Levels, und ein Spiel, das `out.cpp` einbindet, führt
sie als kompilierten Code aus (Stufe „native C++“).

### Funktionen

Eine Funktion hat einen Namen, typisierte Eingänge und typisierte Ausgänge. Angelegt wird sie mit
**+** bei *Functions* in My Blueprint (oder „New Function“ im Aktionsmenü); Details zeigt dann Name,
*Pure*, Beschreibung und die Pins.

- Der Rumpf beginnt am Knoten **Function Entry** (die Eingänge kommen dort heraus; jede Funktion
  hat genau einen, er verschwindet nur mit der Funktion) und endet an einem **Return**-Knoten, der
  die Ausgänge an den Aufrufer gibt. Return beendet die Funktion sofort, auch mitten in einer
  Schleife oder Sequence. Ausgänge, die kein Return gesetzt hat, behalten ihren Default.
- Aufgerufen wird sie mit einem Knoten, der ihren Namen trägt: mit Exec-Pins (**Call**), oder – ist
  sie *Pure* – als Wert-Knoten ohne Exec-Pins, der rechnet, sobald ein Ausgang gelesen wird. Eine
  Pure-Funktion rechnet nur: Sie verzweigt, läuft in Schleifen und liest Werte, aber sie setzt
  keine Variable, wartet nicht und löst nichts aus – und was sie als Schritt aufruft, ebenfalls
  nicht. Sonst ist es ein Compile-Fehler am betroffenen Knoten (Abhilfe: *Pure* abwählen und die
  Funktion als Schritt aufrufen).
- Ein Aufruf läuft sofort bis zum Ende. Nach einem Delay ist der Aufrufer schon weiter; ein Return
  dahinter erreicht ihn nicht mehr. Eine Funktion ruft sich nicht selbst auf, auch nicht über
  andere Funktionen: Das ist ein Compile-Fehler am aufrufenden Knoten (Wiederholen ist Sache der
  Schleifen-Knoten). Nach einem Delay ist sie zu Ende – sich dann aufzurufen, ist ein Timer.
- **Konstanten:** In den Details einer Variablen macht *Constant* sie zu einer Konstanten: Sie
  wird gelesen wie eine Variable und nie geändert – das Knotenmenü bietet nur noch *Get* an, und
  ein Knoten, der sie ändern würde, ist ein Compile-Fehler. Im C++-Text: `KERN_CONST Float Gravity
  = 9.81;`. Eine Bibliothek hat nur Konstanten.
- **Bibliotheken:** Im Abschnitt *Libraries* von *My Blueprint* steht, ob dieser Blueprint eine
  Bibliothek ist (*This Blueprint is a library*: er behält nichts – Typen und Funktionen, keine
  Variablen, keine Events) und welche Bibliotheken des Levels er benutzt (*+ Use a library…*; eine
  Bibliothek heißt wie der Actor, der sie hat). Wer eine Bibliothek benutzt, hat ihre Typen (in
  den Listen mit einem **L**) und findet ihre Funktionen im Knotenmenü unter *Library <Name>*
  (`Workshop::skid`); ein Aufruf ist ein Aufruf wie der einer eigenen Funktion, auch mit
  Referenzen. Was der Bibliothek gehört, wird dort geändert – und wer sie benutzt, folgt in
  demselben Schritt; eine Änderung, der ein Benutzer nicht folgen kann (eine Funktion entfernen,
  die er aufruft), wird mit dem Grund abgelehnt. Im C++-Text ist die Bibliothek eine Basis:
  `struct Garage : virtual kern::Blueprint, virtual Workshop`.
- **Referenz-Eingänge:** In den Details der Funktion steht bei jedem Eingang, wie die Funktion ihn
  nimmt – *value* (sein Wert beim Aufruf), *&* (der Platz, den der Aufruf nennt: die Funktion
  ändert ihn dort) oder *const &* (sie liest ihn dort). Eine Referenz hat keinen Pin: Am Call-Knoten
  stehen stattdessen die Variable und der *Path* zum Platz (`Cars` und `[].Wheels[]`, je `[]` ein
  Index-Pin), in der Funktion legen *Get* und *Set* in der Zeile des Eingangs die Knoten in den
  Graphen. Die Referenzen stehen vor den Werten (wie in C++ die Parameter ohne Default vor denen
  mit): Ein Eingang, der Referenz wird, rückt dorthin, und ↑ schiebt keinen Wert vor eine Referenz.
  Nichts wird kopiert – in keiner Stufe. Solange ein Aufruf einen Eintrag einer Liste
  hält, ändert nichts in ihm die Länge dieser Liste: sonst ist es ein Compile-Fehler am Call-Knoten.
- **Lokale Variablen** gehören einer Funktion allein: Details der Funktion, Abschnitt *Local
  Variables* – **+ Local Variable**, dann Name, Typ und Default wie bei einem Pin; *Get* und *Set*
  in der Zeile legen den Knoten in den Graphen, das Aktionsmenü bietet sie unter „Local Variables
  of <Funktion>“ an (auch Add und die Listen-Schritte). Jeder Aufruf beginnt mit den
  Defaults, und mit dem Aufruf sind sie vorbei: Eine lokale Variable wird nur auf einem Weg ihrer
  eigenen Funktion gelesen und gesetzt – nicht nach einem Delay, nicht in einem Event, nicht in
  einer anderen Funktion (sonst Compile-Fehler am Knoten, wie bei Return). Zwei Funktionen dürfen
  je eine lokale Variable desselben Namens haben; sie heißt aber nicht wie eine Variable, eine
  Funktion oder ein Typ des Blueprints und nicht wie ein Pin ihrer Funktion. Von außen
  (`Instance::get`) ist sie nicht zu lesen. Auch eine Pure-Funktion darf ihre lokalen Variablen
  setzen – das gehört zum Rechnen.
- Aus C++ ruft `logic::Instance::call("Name", args, results)` eine Funktion; `call_number` ist die
  Kurzform für Zahlen. So rufen Engine-Systeme Regeln auf, die im Blueprint stehen.
- Kopierte Knoten nehmen die Funktionen mit, die sie nennen.

### Funktionen anderer Blueprints (Call on Actor)

Ein Blueprint kann eine Funktion aufrufen, die ein anderer Blueprint hat – der eines Actors oder
der des Levels. Dazu sagt er, **wie die Funktion aussieht** (Name, Eingänge, Ausgänge), und ruft
sie **auf einem Actor** auf.

- **Erklären:** In My Blueprint steht unter *Functions* die Auswahl „+ A function of another
  Blueprint…“ mit den Funktionen der anderen Blueprints des Levels; gewählt, steht die Funktion
  hier so, wie sie dort ist (↗ vor dem Namen). Von Hand geht es auch: eine Funktion anlegen und in
  Details **Of another Blueprint** ankreuzen. So eine Funktion hat hier keinen Rumpf – kein
  Function Entry, kein Return, keine lokalen Variablen, nicht *Pure*.
- **Aufrufen:** der Knoten **<Funktion> on Actor** (Details: *Call on Actor*, oder das
  Aktionsmenü). Er hat den Eingang **Target**: der Actor, auf dessen Blueprint die Funktion läuft.
  Ohne Target läuft sie auf dem **Blueprint des Levels** – was dort als Funktion steht, ist damit
  die Bibliothek für alle Blueprints des Levels. Der Aufruf ist ein Schritt (was der andere
  Blueprint tut, ist hier nicht bekannt) und läuft sofort bis zum Ende.
- Auch eine **eigene** Funktion lässt sich so auf einem anderen Actor aufrufen, der sie ebenfalls
  hat („Hit on Actor“ zwischen zwei Gegnern).
- Hat das Ziel keinen Blueprint oder keine Funktion dieses Namens, geben die Ausgänge die hier
  erklärten Defaults. Werte gehen der Reihe nach und werden umgewandelt wie an einem Draht; eine
  Aufzählung geht als ihr Platz, eine Struktur Feld für Feld. Der Editor weist darauf hin, wenn
  kein Blueprint des Levels die Funktion so hat, wie sie erklärt ist (kein Fehler).
- Aufrufe über Blueprints hinweg dürfen im Kreis gehen (A ruft B, B ruft A, eine Funktion sich
  selbst auf ihrem eigenen Actor): Jeder Aufruf hat seine eigenen Parameter und lokalen Variablen.
  Tiefer als 64 ineinander geht es nicht – der Aufruf entfällt dann und wird gemeldet.

### Typen des Projekts (Aufzählungen und Strukturen, die mehrere Blueprints haben)

Eine Aufzählung oder Struktur gehört zuerst dem Blueprint, der sie erklärt. Brauchen mehrere
Blueprints eines Levels denselben Typ – einer gibt eine „Door“ an eine Funktion des anderen –, wird
er ein **Typ des Projekts**: Jeder Blueprint, der ihn hat, hat ihn gleich.

- **Holen:** In My Blueprint steht unter *Enums* und *Structures* die Auswahl „+ An enum / A struct
  of another Blueprint…“ mit den Typen der anderen Blueprints des Levels. Gewählt, hat dieser
  Blueprint den Typ auch – samt der Typen, aus denen seine Felder sind –, und er ist in beiden ein
  Typ des Projekts (Kennzeichen **P**). Wer mit „+ A function of another Blueprint…“ eine Funktion
  erklärt, bekommt die Typen ihrer Ein- und Ausgänge von selbst dazu.
- **Selbst erklären:** in Details eines Typs **Of the project** ankreuzen. Abwählen geht, solange
  kein anderer Blueprint den Typ hat.
- **Ändern:** Eine Änderung an einem Typ des Projekts – ein Wert mehr, ein Feld mehr, ein neuer
  Name, eine andere Reihenfolge – geschieht in **allen** Blueprints des Levels, die ihn haben, als
  ein Schritt (ein Undo nimmt sie überall zurück). Kann einer nicht folgen (der neue Name ist dort
  vergeben), wird die Änderung als Ganzes abgelehnt und sagt, welcher Blueprint es ist. Ein Feld
  einer Projekt-Struktur ist selbst ein Typ des Projekts: Der Feldtyp geht mit.
- **Löschen** nimmt den Typ nur aus diesem Blueprint; die anderen behalten ihn.
- Als C++ heißt so ein Typ `KERN_PROJECT_ENUM(...)` bzw. `KERN_PROJECT_STRUCT(...) { ... };`. Ein
  Text, der einen Typ des Projekts nur hier anders erklärt, wird beim Apply abgelehnt; stimmen die
  Blueprints eines Levels einmal nicht überein (eine von Hand geänderte Datei), zeigt der Editor
  das als Fehler des Blueprints.

### Kollisionskanäle (Box3Ds Filter)

Was womit kollidiert und was ein Strahl trifft, folgt genau Box3D (Erin Catto, *Filtering* und
*Query Filtering*):

- Ein Level nennt seine **Kollisionskanäle** (Welt-Einstellungen, *Collision channels*): Namen
  durch Kommas getrennt, höchstens 32 – der erste ist Bit 1, der zweite Bit 2, der dritte Bit 4.
  Box3D: „The category bits should represent your application object types.“ Neu ist ein Level
  mit dem einen Kanal `World`.
- Ein **Collider** hat *Channels* (wozu er gehört – normalerweise ein Kanal) und *Collides with*
  (welche Kanäle er annimmt), im Editor je ein Kästchen pro Kanal. Zwei Collider kollidieren, wenn
  jeder den Kanal des anderen annimmt: `(catA & maskB) != 0 && (catB & maskA) != 0`. Der
  *Group index* geht vor: gleiche positive Gruppe kollidiert immer, gleiche negative nie.
- Eine **Abfrage** – ein Strahl, ein Cast – hat denselben Filter: die Kanäle, die sie trifft, und
  ihre eigenen. Sie trifft einen Collider, wenn
  `(shape.category & query.mask) != 0 && (shape.mask & query.category) != 0`; so kann ein Collider
  bestimmte Strahlen auslassen (ein Fenster die Kamera). In C++ ist das `physics::QueryFilter`
  an `cast_ray`, `cast_sphere`, `cast_mover` und `GameWorld::line_trace`.
- Im Blueprint: **Line Trace by Channels** (wie Line Trace, dazu *Channels* und *Own Channels*)
  und **Channels**, das die Bits zu Namen gibt (`"World, Vehicle"`, `"All"`). Ein Name, den das
  Level nicht hat, ist kein Bit. In C++:
  `physics_trace_channels(n4, start, end, Actor{0}, physics_channels("World"), 4294967295);`

### Listen und For Each

Eine Variable, ein Pin einer Funktion oder ein Draht kann eine **Liste** von Werten eines Typs
tragen: Boolean List, Integer List, Float List, Vector List, Text List, Actor List (Unreals
Arrays). Ein Listen-Pin ist ein kleines Raster in der Farbe seiner Einträge.

- **Anlegen:** In Details einer Variablen (oder eines Funktions-Pins) als *Type* eine Liste
  wählen. Darunter stehen die Einträge des Defaults, einer je Zeile mit dem Feld seines Typs;
  **+ Item** fügt einen an, **×** nimmt einen heraus. Eine Liste, die direkt in einen Knoten
  getippt ist, bearbeitet man genauso in dessen Details; am Pin steht, wie viele Einträge sie hat.
- **Lesen** (Kategorie *Lists*, Wert-Knoten): **Make List**, **Length**, **Get Item**,
  **Contains**, **Find Item**. Der erste Eintrag hat den Index 0. Get Item jenseits der Liste
  gibt den Nullwert des Typs, Find Item gibt −1, wenn nichts passt.
- **Ändern** (Schritte, je Listen-Variable angeboten: „Add to Scores“): **Add to List** (gibt den
  Index), **Insert into List**, **Remove Index**, **Remove Item** (alle gleichen
  Einträge; sagt, ob es einen gab), **Clear List**. Sie ändern die Liste, die sie nennen – eine
  Variable oder, mit der Einstellung *Path*, eine Liste an einem Platz darin
  (`Cars[].Wheels`). Eine ganze Liste setzt *Set* wie bei jeder Variablen, einen Eintrag *Set*
  mit dem Path `[]` („Set Scores[]“, der Index ist ein Pin).
- **For Each** (Flow) läuft für jeden Eintrag einmal durch *Loop Body* – mit *Item* und *Index* –
  und danach durch *Completed*. Es geht durch die Liste, wie sie beim Start der Schleife war: Der
  Rumpf darf die Variable ändern, ohne dass die Schleife springt oder nie endet.
- **Der Typ kommt vom Draht.** Die Knoten, die mit jeder Liste arbeiten (auch Select und Reroute),
  tragen den Typ als Einstellung *Type*. Zieht man ein Kabel von einem Listen-Pin ins Leere,
  bietet das Menü sie schon im passenden Typ an; ein neuer Knoten nimmt den Typ des ersten Drahts
  an. Hängen schon Drähte an seinen typisierten Pins, bleibt er, was er ist.
- Eine Liste passt nur in einen Eingang ihres Typs – und in jeden Text-Eingang: Print zeigt
  `[1, 2.5, 7]`. (Ausnahme: eine Liste einer Aufzählung, siehe unten.)
- **All Actors with Tag** (Actors) gibt alle lebenden Actors mit einem Tag als Actor List.
- Als C++: `List<Float> Scores = List<Float>{1.0, 2.5};`, `list_add(Scores, x);`,
  `for (auto it9 = flow_for_each<Float>(n9, Scores); it9.next();) { ... }` – siehe
  [blueprint_cpp.md](blueprint_cpp.md), Abschnitt „Listen“.

### Aufzählungen

Ein Blueprint kann **Aufzählungen** (Enums) deklarieren: einen eigenen Typ mit benannten Werten,
etwa `Mode` mit den Werten `Closed`, `Opening`, `Open`. Von diesem Typ kann eine Variable sein,
ebenso eine Liste (`Mode List`), die Ein- und Ausgänge einer Funktion und Pins. Ein Wert einer
Aufzählung ist genau einer ihrer Werte. Er ist weder Zahl noch Text: Er wandelt sich in nichts um,
und ein Draht verbindet nur Pins derselben Aufzählung (auch eine Liste davon passt in keinen
Text-Eingang). Wo eine Zahl oder ein Text gebraucht wird, sagt ein Knoten, was gemeint ist.

- **Anlegen:** **+** bei *Enums* in My Blueprint legt eine an; sie beginnt als `NewEnum` mit den
  Werten `First` und `Second`. Ist sie ausgewählt, zeigt Details ihren Namen, eine Beschreibung und
  ihre Werte: Jeden Wert kann man umbenennen, nach oben oder unten schieben oder entfernen;
  **+ Value** fügt einen an.
- **Als Typ wählen:** Die Auswahlliste *Type* einer Variablen und der Ein- und Ausgänge einer
  Funktion führt die Aufzählungen des Blueprints und eine Liste von jeder (`Mode`, `Mode List`).
- **Einen Wert wählen:** Ein Pin vom Typ einer Aufzählung, an dem kein Draht hängt, zeigt eine
  Auswahlliste mit ihren Werten. Den Default einer Variablen dieses Typs wählt man genauso; eine
  Liste einer Aufzählung hat eine Auswahlliste je Eintrag.
- **Ihre Knoten** bietet das Aktionsmenü (Rechtsklick im Graphen, oder ein Draht, von einem Pin
  gezogen) je Aufzählung an, bei ihrem Namen:

  | Knoten | Pins und Ergebnis |
  |---|---|
  | **Switch on Mode** (Kategorie Flow) | ein Exec-Eingang, der Wert und ein Exec-Ausgang je Wert, beschriftet mit dem Namen des Werts |
  | **Equal (Mode)** | A, B → Result (Boolean) |
  | **Mode to Text** | der Name des Werts |
  | **Text to Mode** | der Wert mit diesem Namen, dazu **Found** – heißt kein Wert so: der erste Wert, und Found ist falsch |
  | **Mode to Index** | die Stelle des Werts unter den Werten der Aufzählung; die erste ist 0 |
  | **Index to Mode** | der Wert an dieser Stelle, dazu **Valid** – gibt es die Stelle nicht: der erste Wert, und Valid ist falsch |
  | **All Values of Mode** | eine Liste aller Werte in ihrer Reihenfolge, für ein For Each |
- **Die Knoten für jeden Typ** arbeiten auch mit Aufzählungen: Get und Set einer Variablen, Select,
  Reroute, die Listen-Knoten (Make List, Length, Get Item, Contains, Find Item, Add to … und die
  übrigen Schritte auf einer Listen-Variablen) und For Each. Ein neuer solcher Knoten nimmt den
  Typ der Aufzählung von seinem ersten Draht an.
- **Eine Aufzählung ändern, die benutzt wird:** Die Aufzählung oder einen Wert umzubenennen,
  ändert nichts außer dem Namen. Einen Wert nach oben oder unten zu schieben, lässt alles bei
  seinem Wert: was in Pins und Variablen eingetragen ist, und die Drähte eines Switch. Wird ein
  Wert entfernt, ist, was dieser Wert war, danach der erste Wert, und der Draht an seinem
  Switch-Ausgang fällt weg. Eine Aufzählung, von deren Typ noch etwas ist – eine Variable, ein Pin
  einer Funktion, ein Knoten –, lässt sich nicht entfernen; der Editor sagt, was sie benutzt.
- **Namen:** Der Name einer Aufzählung und die Namen ihrer Werte sind C++-Namen: Buchstaben,
  Ziffern und `_`, keine Ziffer am Anfang, kein Wort von C++ und keins der Schreibweise
  (`Float` …). Der Name einer Aufzählung endet nicht auf `_list`, keine zwei Werte einer
  Aufzählung heißen gleich, und eine Aufzählung hat mindestens einen Wert (höchstens 256).
- Als C++: `KERN_ENUM(Mode, Closed, Opening, Open);`, `Mode State = Mode::Closed;`,
  `switch (flow_switch_enum<Mode>(State)) { case Mode::Closed: { ... } break; ... }` – siehe
  [blueprint_cpp.md](blueprint_cpp.md), Abschnitt „Aufzählungen“.

### Strukturen

Ein Blueprint kann **Strukturen** (Structs) deklarieren: einen eigenen Typ mit benannten Feldern,
etwa `Weapon` mit den Feldern `Name`, `Type`, `Damage`. Jedes Feld hat einen Typ und einen
Default. Von diesem Typ kann eine Variable sein, ebenso eine Liste (`Weapon List`), die Ein- und
Ausgänge einer Funktion und Pins. Ein Wert einer Struktur hält einen Wert für jedes ihrer Felder.
Er ist weder Zahl noch Text: Er wandelt sich in nichts um, und ein Draht verbindet nur Pins
derselben Struktur. Die Farbe der Strukturen ist ein Blau (`#3f6fd6`).

- **Anlegen:** **+** bei *Structures* in My Blueprint legt eine an; sie beginnt als `NewStruct`
  mit dem Feld `Field` (Float). Ist sie ausgewählt, zeigt Details ihren Namen, eine Beschreibung
  und ihre Felder: Jedes Feld hat einen Namen, einen Typ und einen Default; man kann es
  umbenennen, nach oben oder unten schieben oder entfernen. **+ Field** fügt eins an.
- **Der Typ eines Felds** ist ein Wert-Typ, eine Aufzählung, eine andere Struktur oder eine Liste
  von einem davon. Keine Struktur enthält sich selbst, auch nicht über andere: Die Auswahlliste
  führt nur die Strukturen, die diese nicht enthalten. Eine Struktur hat mindestens ein Feld und
  höchstens 96.
- **Als Typ wählen:** Die Auswahlliste *Type* einer Variablen und der Ein- und Ausgänge einer
  Funktion führt die Strukturen des Blueprints und eine Liste von jeder (`Weapon`, `Weapon List`).
- **Einen Wert eintragen:** Der Default einer Variablen vom Typ einer Struktur hat in Details eine
  Zeile je Feld, mit dem Editor des Feld-Typs (Zahl, Haken, Text, Auswahlliste einer Aufzählung,
  die Felder einer Struktur darin, die Einträge einer Liste). Eine Liste von Strukturen hat diese
  Zeilen je Eintrag. Ein Pin vom Typ einer Struktur, an dem kein Draht hängt, beginnt beim
  Default der Struktur – jedes Feld mit seinem Default – und zeigt am Knoten in Kurzform, was er
  hält; eingetragen wird sein Wert in Details, Feld für Feld.
- **Ihre Knoten** (Kategorie Structures) bietet das Aktionsmenü je Struktur an, bei ihrem Namen;
  Make und Break setzen auch die Knöpfe an der Zeile der Struktur in My Blueprint:

  | Knoten | Pins und Ergebnis |
  |---|---|
  | **Make Weapon** | ein Eingang je Feld (ohne Draht: der Default des Felds) → der Wert der Struktur |
  | **Break Weapon** | der Wert → ein Ausgang je Feld |
  | **Set Held.Damage** (Set an einem Platz; je Struktur-Variable und Feld angeboten) | ein Schritt mit Exec-Pins: ändert das eine Feld der Variablen und lässt die anderen, wie sie sind; gibt das Feld, wie es jetzt ist. Variable und *Path* (der Weg von ihr zum Platz: `.Damage`, tiefer `.Sight.Tries`, durch eine Liste `.Marks[].Tries` mit einem Index-Pin je `[]`) stehen auf dem Knoten und in Details unter Settings. Get, Add und die Listen-Schritte nennen einen Platz genauso |
- **Die Knoten für jeden Typ** arbeiten auch mit Strukturen: Get und Set einer Variablen, Select,
  Reroute, die Listen-Knoten und For Each. Contains, Find Item und Remove Item vergleichen Feld
  für Feld: Zwei Werte sind gleich, wenn alle ihre Felder gleich sind. Ein Eintrag jenseits einer
  Liste ist der Default der Struktur.
- **Eine Struktur ändern, die benutzt wird:** Die Struktur oder ein Feld umzubenennen, ändert
  nichts außer dem Namen – die Werte und die Drähte an Make und Break bleiben. Ein Feld nach oben
  oder unten zu schieben, ändert nur seinen Platz. Ein neues Feld hat überall seinen Default. Wird
  ein Feld entfernt, fällt sein Wert überall weg, ebenso die Drähte an seinen Pins; ein Knoten,
  dessen Path durch es führt, meldet danach einen Fehler. Bekommt ein Feld einen anderen Typ, bleibt sein Wert,
  wo die Typen sich umwandeln, und Drähte, die nicht mehr passen, fallen weg. Nimmt ein Feld den
  Typ einer Struktur an, die in My Blueprint unter dieser steht, rückt diese Struktur unter sie.
  Eine Struktur, von deren Typ noch etwas ist – eine Variable, ein Pin einer Funktion, ein Feld,
  ein Knoten –, lässt sich nicht entfernen; der Editor sagt, was sie benutzt.
- **Namen:** Der Name einer Struktur und die Namen ihrer Felder sind C++-Namen: Buchstaben,
  Ziffern und `_`, keine Ziffer am Anfang, kein Wort von C++ und keins der Schreibweise
  (`Float` …). Der Name einer Struktur endet nicht auf `_list`; sie heißt nicht wie eine Variable,
  Funktion oder Aufzählung. Keine zwei Felder einer Struktur heißen gleich, und kein Feld heißt
  wie eine Aufzählung oder Struktur des Blueprints.
- Als C++: `KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };`,
  `Weapon Held = Weapon{"Knife", 25.0};`, `struct_make<Weapon>("Bow", 12.0)`,
  `struct_break<Weapon>(Held).Damage`, `Held.Damage = 30.0;` – siehe
  [blueprint_cpp.md](blueprint_cpp.md), Abschnitt „Strukturen“.

### Datentabellen

Eine Tabelle ist kein eigener Begriff: Sie ist eine Variable, die eine **Liste einer Struktur** ist.
Jeder Eintrag ist eine Zeile, jedes Feld eine Spalte.

- **Als Tabelle bearbeiten:** Die Details einer solchen Variablen zeigen ihren Standardwert als
  Tabelle – eine Zeile je Eintrag, eine Spalte je Feld, **+ Row** hängt eine Zeile an, × nimmt
  eine heraus. Ein Feld, das selbst eine Struktur oder Liste ist, hat keine Spalte; es steht unter
  seiner Zeile.
- **Find Row** (Lists; im Menü je Struktur als „Find Item by Key“) holt die erste Zeile, deren
  Feld der gesuchte Wert ist – der Schlüssel kann jedes Feld sein, das man in der Einstellung
  **Field** wählt. Ausgänge: **Found**, **Row** (ohne Treffer die Standardwerte der Struktur),
  **Index** (ohne Treffer −1).
- Die Daten stehen im Blueprint (für Daten des ganzen Spiels: im Blueprint des Levels) und werden
  mit ihm gespeichert, als C++ geschrieben und kompiliert. Weil es eine Variable ist, lässt sich
  die Tabelle im Spiel auch ändern (Add, Set an einem Eintrag oder Feld …).

### Formel-Knoten

**Formula** (Kategorie Math) ist eine Formel als ein Knoten: Jeder Name darin wird ein Eingang,
`max(9000, 16000 - level * 1300)` hat also den Eingang `level`. Die Formel steht in Details unter
Settings und ist der Titel des Knotens; eine Formel, die sich nicht lesen lässt, ist ein
Compile-Fehler mit Stelle und Grund.

| | |
|---|---|
| Operatoren | `+ - * / %`, `^` (Potenz), Vorzeichen `-`, `!` |
| Vergleiche | `< <= > >= == !=` ergeben 1 oder 0; `&&`, `||`; `a ? b : c` |
| Funktionen | `min max clamp(x, lo, hi) abs sign floor ceil round trunc sqrt pow(x, y) exp log lerp(a, b, t)` |
| Winkel | `sin cos tan asin acos atan atan2(y, x)` in Radiant; `rad(grad)`, `deg(radiant)`; `pi` |
| Ganze Zahlen, wie eine Maschine sie hat | Bits: `band(a, b)`, `bor(a, b)`, `bxor(a, b)`, `shl(a, n)`, `shr(a, n)`; 32 Bit: `wrap32(x)` (was eine vorzeichenlose 32-Bit-Zahl davon hält: 0 … 4294967295), `wrap32s(x)` (mit Vorzeichen), `mul32(a, b)` (das Produkt, wie 32 Bit es halten) – für Hashes und Paketformate; `single(x)`: die Zahl, wie eine 32-Bit-Kommazahl sie hält (was die Komponente eines Vektors ist) |

Gerechnet wird wie in den Mathe-Knoten (f64, dieselben Funktionen auf jeder Plattform); eine
Division durch 0 ergibt 0.

Ausgeführt wird in drei Stufen, die dasselbe Ergebnis liefern (`kern/tests/logic_cases.h`):
1. **Interpreter**, zum Bearbeiten.
2. **JIT**: Beim Start von Play/Simulate kompiliert Kern jeden Graphen zu WebAssembly-Maschinencode
   (`src/logic/codegen.cpp`). Das ist etwa 55-mal schneller als der Interpreter. Die Statuszeile zeigt
   „compiled“.
3. **Als C++-Klasse**: Jeder Blueprint ist auch ein C++-Text (siehe „Blueprints als C++“).
   **Download** in der C++-Ansicht gibt die Quelle dafür; in ein Spiel eingebunden
   (`kern/build.sh --studio out.wasm blueprint_<name>.cpp`, oder für ein ganzes Level
   `tools/blueprint_cpp.sh`), läuft der Blueprint als diese kompilierte Klasse – gefunden über den
   Hash des Graphen, also nie als veralteter Code.

Logic läuft nur auf der Autorität (Standalone oder Server). Geänderte Eigenschaften und erzeugte
Actors werden repliziert, auch an Spieler, die später dazukommen.

## Spieler, Soldat, Touch und Gamepad

- **Spieler setzen:** Ein Element aus der Gruppe **Players** in die Welt ziehen; es wird sofort der
  Spieler (Badge PLAYER im Outliner). Alternativ Rechtsklick → *Make Player* oder World Settings →
  *Player*.
- **Soldat:** Ego-Shooter nach SpawnCity.
  - Bewegung: Gehen, Sprinten, Ducken, Springen.
  - Waffen: Maschinenpistole, Pistole, Sniper oder 40-mm-Werfer mit Magazinen, Nachladen, Zielen über Kimme
    und Korn sowie Kopftreffern.
  - Spiel: Respawn und Lag-Kompensation im Online-Spiel.
  - Optik: Andere Spieler sehen die animierte Gelenkfigur mit Waffe, dazu Mündungsfeuer, Tracer,
    Einschusslöcher und Splitter.
- **Micro Duck:** Pollen Robotics' zweibeiniger Roboter mit seinem echten, trainierten Netz
  (velstand v8).
  - Steuerung: W/S laufen, A/D drehen, Leertaste quakt.
  - Ohne Eingabe läuft sie autonom: Sie erkundet den Raum mit Kopf-ToF und Kamera, weicht
    Hindernissen aus und steht nach einem Sturz selbst wieder auf (Details › Walker › Autonomous).
  - Oben rechts im Viewport zeigt Studio ihre Kopfkamera und das ToF-Gitter (Show › Robot Camera).
  - Logic steuert sie mit den Knoten der Kategorie Robots (Walk, Look, Robot ToF, Robot Camera Sees …).
  - Beim Einfügen stellt Studio die Physik auf 100 Hz mit 16 Sub-Steps.
  - Beispiel-Level **Micro Duck**: ein Zimmer in ihrem Maßstab (2,5 × 2,5 m echt) mit Möbeln und
    einem Ball. Die Ente läuft autonom. Der Level-Blueprint lässt sie quaken, sobald ihre
    Kopfkamera den Ball sieht.
  - Details: [`walker.md`](walker.md).
- **Geräteerkennung:** Die Engine erkennt Touch, Maus+Tastatur und Gamepad selbst und schaltet auf
  das zuletzt benutzte um (`src/game/touch.cpp`).
  - Auf Touch erscheinen 168-px-Sticks in den Ecken: links bewegen, rechts schauen mit Edge-Turn.
    Ein Tippen auf den Bildschirm feuert, die Buttons passen sich dem Controller an.
  - Play läuft im Vollbild mit EXIT; Pointer-Lock gibt es nur mit Maus.
  - Gamepad: Xbox-Belegung mit Dead-Zones und Kurven wie in SpawnCity.
- **Viewport:** Actors bewegt nur der Gizmo (W / E / R) wie in Unreal. Linkes Ziehen zeichnet ein
  Auswahlrechteck.

Physik-Regeln für eigene Presets: [`physics.md`](physics.md).

## Als App installieren (Android, iOS, Desktop)

Kern Studio unter `/editor/` ist eine installierbare Web-App:

- **Installieren:**
  - Chrome und Edge (Android, Windows, macOS, Linux) zeigen in der Titelleiste **Install**.
  - Auf dem iPhone und iPad erklärt der Button die zwei Schritte: Teilen, dann *Zum Home-Bildschirm*.
  - Die App startet danach im Vollbild (`display: fullscreen`). Notches und Statusleiste bleiben frei
    (`viewport-fit=cover` mit Safe-Area-Abständen).
- **Offline:** Der Service Worker (`site/editor/sw.js`) holt die Seite immer zuerst aus dem Netz. Nur
  wenn das Netz fehlt, startet die zuletzt geladene Version aus dem Cache. Icons und Manifest kommen
  aus dem Cache und werden im Hintergrund aufgefrischt.
- **Nie eine alte Version:**
  - Jeder Deploy schreibt den Build-SHA als ersten Kommentar in die Seite und als `build.json` daneben.
  - Die Seite fragt `build.json` beim Start, bei jeder Rückkehr zur App, wenn das Netz zurückkommt,
    und alle 60 s ab. Diese Abfrage geht an keinem Cache vorbei.
  - Ist ein neuer Build da, lädt Studio sofort neu, solange nichts verloren gehen kann: im Editier-Modus
    ohne ungespeicherte Änderungen.
  - Sonst erscheint **New version available · Update / Later**. Beim Update wird das Level per Autosave
    mitgenommen und beim nächsten Start angeboten (siehe [Wiederherstellen](#wiederherstellen-restore-from-last-session)).
  - Unten rechts steht die laufende Version, gelb, wenn online eine neuere bereitliegt.
- **Level öffnen auf Android:** Android hat kein `showOpenFilePicker`, Studio öffnet den normalen
  Dateiauswahl-Dialog. Der kennt die Endung `.klevel` nicht und würde mit einem Filter alle
  gespeicherten Levels ausgrauen (heraus käme man nur mit Zurück). Auf Android filtert Studio deshalb
  nicht; ist die gewählte Datei kein Level, erscheint ein Hinweis, und der Editor läuft weiter.
- **Icons:** 192, 512 und maskable 512 im Manifest, dazu ein Apple Touch Icon (180) und ein Favicon
  (`site/editor/icons`).
