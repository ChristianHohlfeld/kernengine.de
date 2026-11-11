# Keyframe-Lehrer: eine neue Micro-Duck-Bewegung lernen

Der Lehrer kann Bewegungen **ohne LLM und ohne neuen C++-Task pro Bewegung** aus einer echten
3D-Posenfolge lernen. Er ist ein Arbeitsbereich von Kern Studio (`docs/studio.md`, „Der Lehrer“):
Die Ente steht im Viewport des Studios, das Panel des Lehrers liegt daneben, und was die Ente lernt,
gehört zum Level.

## Bedienung

1. Im Studio **Lehrer** in der Werkzeugleiste anklicken (oder Fenster > Lehrer, oder die Adresse
   `…/editor/?train=1`), als Aufgabe **Bewegung aus Keyframes** wählen.
2. Die Micro Duck steht auf einer eigenen Bühne in der GameWorld des Studios, mit
   **Box3D-Schwerkraft 0**. Der echte `physics::MouseGrab` (kinematischer Maus-Body + Motor-Joint,
   Feder) zieht Gliedmaßen, mit der Maus oder dem Finger. Für Posen ist der Rumpf physikalisch
   fixiert; die 14 Servo-Motoren sind **ausgeschaltet**, bis ein Glied losgelassen wird: Dann halten
   sie die Pose. Box3Ds Gelenkgrenzen bleiben erhalten. Die Ente ist kein bloß gerendertes Skelett.
   Das Level selbst steht dabei still; seine Actors werden nicht verändert.
3. Der erste Keyframe ist die Grundpose bei `0 s`. Gelenke bewegen und in der Leiste auf der Bühne
   **+ Keyframe** zu einem Zeitpunkt drücken, etwa Kopf nach unten bei `0,3 s`, nach oben bei
   `0,6 s`; am Ende bei der Länge der Bewegung die Endpose. **Anzeigen** an einem Keyframe fährt die
   Ente durch die echten Servos in dessen Pose. Nach jedem Keyframe geht die Zeit im gleichen Schritt weiter (nach `0,3` und `0,6` steht sie bei
   `0,9`), und **Gleichmäßig verteilen** setzt alle Keyframes in gleichen Abständen über die Länge.
   Zeitpunkte lassen sich in der Liste ändern, Keyframes
   löschen. Jede Änderung ist ein Schritt im Verlauf des Levels (Rückgängig / Wiederholen).
4. **Start**: Der Lehrer übernimmt maximal 32 Keyframes, jeweils 14 **relative Servo-Winkel
   in Radiant** und ihre absolute Zeit in Sekunden. Die Werte kommen aus den Box3D-Encodern
   (nicht aus einem animierten 3D-Modell). Die Konfiguration wird für native und WASM-Tasks
   identisch als `motion:{...}` übergeben.
5. **Nachahmen**: Unter normaler Gravitation fährt der Lehrer die interpolierten Servo-Targets
   in einer eigenen GameWorld und sammelt 50-Hz-Beobachtungen mit Sollaktionen. Behaviour Cloning
   trainiert den Actor auf diese Daten, ausgehend von Pollens `velstand`-Netz.
   **Üben**: PPO belohnt die gemessene Gelenkpose, die Referenzaktion und das Gleichgewicht.
   Glieder-Massen, Servo-Verzögerungen und Sensorrauschen werden im Training berücksichtigt.
   Das Training läuft in Web Workern der Studio-Seite (je Worker ein `kern_lehrer.wasm`) und geht
   weiter, während im Studio bearbeitet oder gespielt wird.
6. **Übernehmen/Testen**: Nach dem Nachahmen, alle 25 PPO-Iterationen und zum Schluss wird ein
   echtes 61→14-ONNX exportiert und **im Level gespeichert** (`Level::motions`, siehe unten); ein
   Trainingslauf ist ein Schritt im Verlauf. **Netz an der Ente testen** stellt die Ente bei
   normaler Schwerkraft auf den Boden, auf dem sie trainiert, und startet die Bewegung. **Im
   Level** simuliert das offene Level und lässt dessen Micro Duck die Bewegung spielen. In
   Blueprints spielt der Knoten **Robot Move** (`walker.skill`) sie unter ihrem Namen.

## Daten und Grenzen

Die Aufgabe, die der Lehrer bekommt (`motion:` + JSON):

```json
{
  "name": "kopfnicken",
  "duration": 1.2,
  "frames": [
    {"time": 0, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
    {"time": 0.6, "joints": [0,0,0,0,0,0,0.35,0,0,0,0,0,0,0]},
    {"time": 1.2, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]}
  ]
}
```

Die Bewegung im Level (`.klevel`, Feld `motions`; fehlt, wenn dem Level nichts beigebracht wurde):

```json
"motions": [
  {"name": "kopfnicken", "duration": 1.2,
   "keys": [{"time": 0, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
            {"time": 0.6, "joints": [0,0,0,0,0,0,0.35,0,0,0,0,0,0,0]},
            {"time": 1.2, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]}],
   "onnx": "<die ONNX-Datei, Base64>"}
]
```

- `name`: a–z, 0–9, `_` und `-`, höchstens 38 Zeichen; so heißt die Bewegung in Blueprints.
  `duration`: Sekunden in Roboterzeit. `keys`: die Keyframes in Zeitreihenfolge (leer bei einem
  Netz, das nicht aus Keyframes kommt). `onnx`: das trainierte Netz; fehlt, solange die Bewegung
  noch nicht beigebracht wurde.
- Beim Start einer Welt (`GameWorld::start`: Spielen, Simulieren, ein verpacktes Spiel, der native
  Export) werden die Bewegungen des Levels zu Bewegungen seiner Micro Ducks
  (`GameWorld::learn_motions`); `walker_skill(ente, name)` startet sie. Ein Netz, das der Roboter
  nicht ausführen kann (nicht 61→14), wird als Warnung gemeldet und ausgelassen. Eine Bewegung des
  Levels mit dem Namen einer eigenen Bewegung des Roboters ersetzt diese in diesem Level: Trainiert
  der Lehrer den **Sprung**, ist das Ergebnis der eigene `jump` des Levels.
- Die Eingabe des Lehrers wird geprüft: Dauer 0,3–15 s, zwei bis 32 streng aufsteigende
  Keyframes, Beginn bei 0, Ende bei Dauer, exakt 14 endliche Winkel innerhalb ±2,5 rad.
- Ein Pose-Screenshot oder eine beliebige Animation allein trainiert kein Netz; erforderlich
  sind Referenzaktionen mit korrekter Servo-Kinematik und ein eindeutiges Phasenkommando.
- Die Imitation ist **zeit-/phasenabhängig**. Sie ist keine ereignisgesteuerte Aufgabe.
  Sockengreifen erfordert zusätzlich Wahrnehmung, Greif-/Kontaktmessung und die bestehende
  `play.onnx`-Task, nicht einfach ein Keyframe-Netz.
- Das Level ist die Quelle: Speichern, Öffnen, „Spiel verpacken“ und der native Export
  (`tools/export_game.sh`) tragen die Bewegungen mit. Ein Netz ist rund 0,8 MB groß (als Base64
  rund 1,1 MB); die Kopie für „Wiederherstellen“ im Browser-Speicher (einige MB) fasst deshalb
  nur Levels mit wenigen Bewegungen – solche Levels als Datei speichern.
- **Keine automatische Übertragung an reale Hardware.** Für echten Betrieb braucht das Gerät eine
  passende Laufzeit, Servo-Profil, Phasensteuerung und einen Sicherheits-/Grenztest. Das Netz
  einer Bewegung lässt sich aus der Liste „Bewegungen dieses Levels“ als `.onnx` herunterladen.
- Die Qualität wird über die mittlere physikalisch gemessene Winkelabweichung (Grad) und
  Stürze in sauberen Simulationsläufen überprüft. ONNX-Export garantiert keinen Erfolg.
- Sprung, Stehen und die freien Wegpunkt-Szenarien (`docs/szenarien.md`) trainiert derselbe
  Arbeitsbereich. Das Netz eines Szenarios oder des Schnelltests „Stehen“ ist keine Bewegung der
  Ente; es wird als `.onnx` heruntergeladen.

## Von der früheren Lehrer-Seite

Bis zu diesem Stand war der Lehrer eine eigene Seite (`/lehrer/`), die ihre Netze im Browser des
Geräts ablegte (IndexedDB `kern-lehrer`, die Namen in `localStorage` unter
`kern-lehrer.motion-skills.v1`). Die Adressen `/lehrer/` und `/train/` leiten jetzt in den
Arbeitsbereich. Findet das Studio solche Netze auf dem Gerät, bietet es einmal an, sie in das
offene Level zu importieren; im Panel des Lehrers bleibt das Angebot für Levels, die sie noch nicht
haben. Die Bewegungen behalten ihre Namen (`motion_…`), damit Blueprints sie weiter finden. Der
Browser-Speicher wird danach nicht mehr gelesen: Was die Enten können, steht im Level.

## Code

- `physics::MouseGrab`, `GameWorld::set_walker_pose_edit`, `GameWorld::set_walker_pose`
- `kern/src/studio/trainer.cpp`: die Bühne (Posieren, Testen), Keyframes, die Bewegungen des
  Levels; Befehle `trainer`, `trainerCapture`, `trainerKey`, `trainerShow`, `trainerMotion`,
  `trainerTask`, `trainerLearned`, `trainerTest`, `trainerPlay`, `trainerNet`, `trainerImport`, …
- `kern/web/studio/trainer.mjs`: das Panel, der Trainingslauf in Web Workern
- `kern/include/kern/game/level.h` (`MotionAsset`), `kern/src/game/level.cpp`: die Bewegungen in
  der Level-Datei; `kern/src/game/walker.cpp` (`GameWorld::learn_motions`): die Bewegungen einer Welt
- `kern/src/lehrer/motion.cpp`: validierte Referenz, Phasenkommando und Pose-Reward
- `kern/src/lehrer/recipe.cpp`: Behavior-Cloning-Warmstart; danach normales PPO
- `kern/tests/test_main.cpp` (`studio_lehrer_*`, `game_level_keeps_learned_motions`,
  `micro_duck_plays_the_motions_its_level_taught`), `kern/tests/lehrer_wasm_test.mjs`,
  `kern/tests/e2e/studio_e2e.mjs`: Regression
