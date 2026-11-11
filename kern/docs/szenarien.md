# Freie Trainingsszenarien – Kern Lehrer

## Warum dieser Ansatz?

Pollen Robotics trainiert Microduck mit mjlab/MuJoCo, rsl_rl-PPO, pro Task konfigurierten
Beobachtungen, Belohnungen und Domain Randomization. Kern nutzt seine bestehende C++-/Box3D-/PPO-
Pipeline weiter, lässt neue Waypoint-/Pose-Aufgaben aber als JSON statt als neue C++-Klasse zu.

## Benutzeroberfläche

1. In Kern Studio den Arbeitsbereich **Lehrer** öffnen (Werkzeugleiste, `docs/studio.md`) und als
   Aufgabe **Freies Trainingsszenario** wählen.
2. Szenarioname, Episodendauer, Ziele (x y Radius in Metern), Hindernisse (Box/Kugel) setzen.
3. Fortschritts-, Ziel-, Aufrecht- und Sturz-Belohnungen ändern.
4. Start führt das Szenario in parallelen Web Workern durch und trainiert mit PPO.
5. Alle 25 Iterationen wird über fünf saubere Episoden die Zielerreichung geprüft.
6. ONNX herunterladen; Szenario-JSON speichern/importieren; zusätzliche Parameter per JSON bearbeiten.
   Das Netz eines Szenarios ist keine Bewegung der Ente und wird deshalb nicht im Level abgelegt.

## Natives Training

    kern/tools/lehrer.sh
    kern/build/kern_lehrer --scenario parcous.json --iterations 3000 --out netz.onnx --log lauf.csv --checkpoint lauf.ckpt

Fortsetzen mit derselben Szenario-Datei und --resume lauf.ckpt.

## JSON-Schema, Beispiel

    {
      "name": "Zwei Ziele mit Hindernis",
      "duration": 8,
      "start": {"yaw": 0, "yawSpread": 10},
      "commands": {"forward": 0.15, "lateral": 0, "turn": 0},
      "waypoints": [{"x":0.4,"y":0,"radius":0.15}, {"x":0.85,"y":0.1,"radius":0.12}],
      "objects": [{"kind":"box","x":0.6,"y":0.25,"width":0.15,"depth":0.2,"height":0.2},
                  {"kind":"sphere","x":0.9,"y":-0.2,"radius":0.05,"dynamic":true,"mass":0.05}],
      "targetHeight": 0.115, "targetSpeed": 0, "push": 0.3, "endOnContact": false,
      "rewards": {"progress":12,"waypoint":8,"finish":20,"upright":2,
                  "fall":-12,"contact":-2,"actionRate":-0.1,"height":0,"speed":0,"pose":0}
    }

Optional: pose ist ein Array mit 14 Gelenk-Sollwinkeln in Radiant, gewichtet über rewards.pose.
Ein Szenario ohne Ziele funktioniert für Balance-/Posen-/Geschwindigkeitsbelohnungen.
Maximal 16 Wegpunkte, 32 Objekte, 16 KiB JSON, Dauer 0,5–60 Sekunden.
Zielpunkte werden nacheinander aktiviert; das letzte Ziel beendet die Episode erfolgreich.
Sturz beendet die Episode immer, endOnContact zusätzlich bei Rumpf-/Armkontakt.
ActionRate, Pose, Höhe und Geschwindigkeit sind zusätzliche Reward-Terme.
Boxen und Kugeln werden je Episode als echte Box3D-Collider erzeugt.

## Sim2real: sehr wichtig

Die 61 Beobachtungen/14 Aktionen und der 50-Hz-Takt bleiben unverändert.
Zielrichtung und Distanz werden in den Slots 55/56 als lokale Kommandos bereitgestellt,
während 48–50 die Pollen-Geschwindigkeitskommandos enthalten. Die Zielrichtung
stammt beim Training aus der Simulationsposition; der reale Roboter besitzt
keinen eingebauten Sensor für diese Daten. Eine reale Anwendung muss dieselben
Kommandos aus einer echten Positionsbestimmung liefern. Neue ONNX-Dateien
werden nicht automatisch im Studio als Fertigkeit registriert.
Vor Hardwareeinsatz sind die Servo-/Sensorprofile, Sturzschutz, Kollisionen
und die Observation-Belegung zu verifizieren.

## Noch nicht enthalten

Keine 3D-Vorschau im Lehrer, keine freien Kameradetektoren,
keine beliebigen Robotermodelle und kein komplett frei definierbares Greifszenario.
Dies ist der erste generische Simulator-Task mit echten Szenario-Geometrien,
komponierbaren Rewards und sequenziellen Zielen, nicht eine universelle
RL-Umgebung für beliebige Hardware.