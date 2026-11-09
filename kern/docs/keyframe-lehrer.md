# Keyframe-Lehrer: eine neue Micro-Duck-Bewegung lernen

Der Lehrer kann Bewegungen **ohne LLM und ohne neuen C++-Task pro Bewegung** aus einer echten 3D-Posenfolge lernen.

## Bedienung (Browser)

1. \`/lehrer/\` öffnen, **Neue Bewegung (Keyframes)** auswählen.
2. Die eingebettete **Kern-Studio-GameWorld** stellt die Micro Duck mit **Box3D-Schwerkraft 0** dar.
   Der echte \`physics::MouseGrab\` (kinematischer Maus-Body + Motor-Joint, Feder) zieht Gliedmaßen.
   Für Posen ist der Rumpf physikalisch fixiert; seine 14 Servo-Motoren sind **ausgeschaltet**.
   Box3Ds Gelenkgrenzen bleiben erhalten. Die Ente ist kein bloß gerendertes Skelett.
3. Bei \`0 s\` die Ausgangspose speichern. Gelenke bewegen und weitere Keyframes speichern,
   etwa Kopf nach unten bei \`0,3 s\`, nach oben bei \`0,6 s\`; am Ende bei \`Dauer\` die Endpose.
   Ein Klick auf einen gespeicherten Keyframe spielt dessen Zielpose durch die echten Servos
   kurz vor, danach kehrt die Ente in den schwerelosen Posiermodus zurück.
4. **Start**: Der Lehrer übernimmt maximal 32 Keyframes, jeweils 14 **relative Servo-Winkel
   in Radiant** und ihre absolute Zeit in Sekunden. Die Werte kommen aus den Box3D-Encodern
   (nicht aus einem animierten 3D-Modell). Die Konfiguration wird für native und WASM-Tasks
   identisch als \`motion:{...}\` übergeben.
5. **Nachahmen**: Unter normaler Gravitation fährt der Lehrer die interpolierten Servo-Targets
   in einer eigenen GameWorld und sammelt 50-Hz-Beobachtungen mit Sollaktionen. Behaviour Cloning
   trainiert den Actor auf diese Daten, ausgehend von Pollens \`velstand\`-Netz.
   **Üben**: PPO belohnt die gemessene Gelenkpose, die Referenzaktion und das Gleichgewicht.
   Glieder-Massen, Servo-Verzögerungen und Sensorrauschen werden im Training berücksichtigt.
6. **Übernehmen/Testen**: Alle 25 PPO-Iterationen und zum Schluss wird ein echtes 61→14-ONNX
   exportiert und in IndexedDB gespeichert. \`kern_studio_load_policy\` lädt die Bytes in
   die offene Simulation. Zusätzlich registriert Kern eine neue, benannte, zeitbegrenzte
   Fähigkeit zur Laufzeit (Phase cos/sin in Beobachtung 55/56).
   Die Schaltfläche **Netz an der Ente testen** lädt die neue Version ausdrücklich und startet
   sie bei normaler Schwerkraft. In normalen Studio-Tabs erscheinen die gelernten Fähigkeiten
   als eigene Schaltflächen, ohne jede Änderung am statischen Pollen-Manifest.

## Daten und Grenzen

\`\`\`json
{
  "name": "motion_kopfnicken",
  "duration": 1.2,
  "frames": [
    {"time": 0, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
    {"time": 0.6, "joints": [0,0,0,0,0,0,0.35,0,0,0,0,0,0,0]},
    {"time": 1.2, "joints": [0,0,0,0,0,0,0,0,0,0,0,0,0,0]}
  ]
}
\`\`\`

- Die Eingabe wird geprüft: Dauer 0,3–15 s, zwei bis 32 streng aufsteigende Keyframes,
  Beginn bei 0, Ende bei Dauer, exakt 14 endliche Winkel innerhalb ±2,5 rad.
- Ein Pose-Screenshot oder eine beliebige Animation allein trainiert kein Netz; erforderlich
  sind Referenzaktionen mit korrekter Servo-Kinematik und ein eindeutiges Phasenkommando.
- Die Imitation ist **zeit-/phasenabhängig**. Sie ist keine ereignisgesteuerte Aufgabe.
  Sockengreifen erfordert zusätzlich Wahrnehmung, Greif-/Kontaktmessung und die bestehende
  \`play.onnx\`-Task, nicht einfach ein Keyframe-Netz.
- Browser-Modelle werden lokal pro Origin gespeichert; der GitHub-Stand und der reale
  Roboter ändern sich dadurch nicht. **Keine automatische Übertragung an reale Hardware.**
  Für echten Betrieb braucht das Gerät eine passende Laufzeit, Servo-Profil, Phasensteuerung
  und einen Sicherheits-/Grenztest. Im Studio ist der Live-Test unmittelbar möglich.
- Nur ein offener Tab innerhalb derselben Origin kann die trainierten Netze automatisch
  übernehmen. Für andere Browser-Geräte ist ONNX-Export vorgesehen.
- Die Qualität wird über die mittlere physikalisch gemessene Winkelabweichung (Grad) und
  Stürze in sauberen Simulationsläufen überprüft. ONNX-Export garantiert keinen Erfolg.
- Die ursprünglichen Sprung-/Stehen- und freien Wegpunkt-Szenarien bleiben verfügbar.

## Code

- \`physics::MouseGrab\`, \`GameWorld::set_walker_pose_edit\`
- \`Studio::run\`: \`motionBegin\`, \`motionPose\`, \`motionEdit\`, \`motionPreview\`, \`motionRegister\`, \`motionPlay\`, \`motionRun\`
- \`kern/src/lehrer/motion.cpp\`: validierte Referenz, Phasenkommando und Pose-Reward
- \`kern/src/lehrer/recipe.cpp\`: Behavior-Cloning-Warmstart; danach normales PPO
- \`kern/web/lehrer/\`: Keyframe-UI, PPO-Worker und ONNX-Persistenz
- \`kern/web/studio/\`: eingebettete GameWorld, Live-Übernahme, Skill-Buttons
- \`kern/tests/test_main.cpp\` und \`kern/tests/lehrer_wasm_test.mjs\`: Regression
