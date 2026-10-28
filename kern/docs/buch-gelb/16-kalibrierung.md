# 16 Kalibrierung: vom Bildschirm auf den Tisch

> **Dein Level:** Ein Netz, das im Computer läuft, soll auch auf der echten Micro Duck laufen. Dafür
> muss die Simulation den echten Roboter genau treffen. Hier lernst du, wie man einen Servo vermisst
> und die Simulation an die Wirklichkeit anpasst.

> **Achtung, Sicherheit – zuerst lesen!**
> Ab hier geht es um einen **echten Roboter**. Arbeite damit **nur zusammen mit einem Erwachsenen**.
>
> - **Hände weg von laufenden Servos.** Unter Last schlagen sie schnell und kräftig aus.
> - Prüfstand fest einspannen, Arm und Gewicht sichern, Hände und Kabel aus dem Schwenkbereich.
> - Strom begrenzen und einen Not-Aus griffbereit halten.
> - Nie mehr Spannung als vom Servohersteller erlaubt.
> - Lithium-Akkus nur mit passendem Ladegerät und unter Aufsicht laden.
> - Neue Netze zuerst testen, während der Roboter gehalten wird oder hängt.
> - Alles auf eigenes Risiko ([Impressum](00-impressum.md)).

Kern geht drei Wege, und ein vierter Grundsatz kommt dazu:

1. **Gerätemodelle mit Profilen**, angepasst an Messungen und an anderen Messungen geprüft.
2. **Abgleich des ganzen Roboters:** Ein Log des echten Roboters läuft in der Simulation.
3. **Domänen-Randomisierung:** Was man nicht messen kann, variiert das Training.
4. **In der Physik messen statt eintragen.**

Box3D bleibt unangetastet ([Kapitel 6](06-physik.md)). Das Werkzeug ist `kern_lehrer`.

## 16.1 Das Servomodell

Das Gesetz kennst du aus [Kapitel 13](13-micro-duck.md): Zug `K · (Ziel − Winkel)`, höchstens T,
dazu Dämpfung D, Reibung F, Armatur A und eine Verzögerung. Ein **Servoprofil** speichert diese
Zahlen als JSON-Datei, etwa `"stiffness": 0.554` für K. Kern prüft beim Lesen jeden Wert: Ist er
eine Zahl im erlaubten Bereich? Steifigkeit und Moment müssen größer als 0 sein. Fehlende Werte
bleiben 0.

Ein anderer Servo ist ein anderes Profil. Ein Gerät, das grundsätzlich anders arbeitet (etwa ein
Linearmotor), bekommt ein eigenes Modell. Dazu kommen **Streuungen**: Beim XL330 streuen Armatur
und Reibung um ±10 %. Das Training würfelt daraus je Servo und Episode eigene Werte, dazu je
Episode eine Busverzögerung zwischen 15 und 30 ms. So verlässt sich das Netz nicht auf eine Zahl, die niemand genau
kennt. Ein gut gemessener Servo bekommt kleine Streuungen, ein schlecht bekannter große.

## 16.2 Der Prüfstand

### So misst man

1. **Aufbau:** Servo fest eingespannt, Achse waagerecht, ein Arm mit Gewicht. Masse auf 0,1 g genau
   wiegen, Abstand des Schwerpunkts zur Achse messen (Arm auf einer Kante ausbalancieren).
2. **Wie im Roboter betreiben:** gleiche Einstellungen, gleiche Spannung (7,4 V, wenn der Hersteller
   sie erlaubt).
3. **Signale**, mindestens 100 Messungen pro Sekunde: Sprünge zwischen Winkeln, ein „Chirp“ (eine
   Schwingung, die immer schneller wird, 0,5 → 4 Hz) und ein Rechteck.
4. **Zwei oder drei Lasten**, etwa 50 g an 5 cm und 120 g an 6 cm. Mindestens ein Log je Servo
   bleibt als **Prüf-Log** zurück.
5. **Mehrere Servos** messen. Wie stark sie sich unterscheiden, wird zur Streuung im Profil.

Ein Log ist eine Textdatei:

```
# mass 0.05
# arm 0.05
# horizontal 1
time,target,angle
0,-0.5,0
0.005,-0.5,-0.0123
```

Oben die Last: Masse in kg, Abstand des Schwerpunkts zur Achse in m, optional `# inertia`.
`horizontal 1` heißt waagerechte Achse: Die Schwerkraft zieht am Arm wie an einem Pendel. Darunter je Zeile Zeit in s, gesendeter Zielwinkel und gemessener Winkel in rad.

### Der Prüfstand im Computer

Kern baut den Aufbau in Box3D nach, genau wie die Ente: Maßstab 4, dieselbe Schrittrate, dieselbe
Gelenkweichheit, Armatur als Scheibe. Schwerkraft gibt es nur bei waagerechter Achse. Dann spielt
Kern die Ziele aus dem Log ab, jeweils um die Verzögerung später, und vergleicht die Winkel.
Der Fehler ist der **RMS-Fehler**: alle Abweichungen quadrieren, den Mittelwert nehmen, die Wurzel
ziehen. Beispiel: Abweichungen 3 und 4 mrad ergeben √((9 + 16) / 2) ≈ 3,5 mrad.

### Anpassen und prüfen

```
kern_lehrer --kalibrieren steps.csv,chirp.csv
            --pruefen rechteck.csv --out xl330_unit3.json
```

(Im Terminal in einer Zeile.) Das Suchverfahren **CMA-ES** probiert je Runde 12 Kandidaten für
K, D, F, T, A und die Verzögerung und tastet sich in 80 Runden an die besten heran (änderbar mit
`--iterations`; Start ist `--profil`, sonst XL330). K bis A verändert es als **Faktor**:
Verdoppeln zählt so viel wie Halbieren, egal wie groß die Zahl ist. Die Verzögerung sucht es
zwischen 0 und 0,2 s.

> **So kannst du es dir vorstellen:** Topfschlagen mit zwölf Freunden: Alle suchen gleichzeitig,
> und die nächste Runde startet dort, wo es am lautesten „heiß“ hieß.

Entscheidend ist der Fehler auf dem **Prüf-Log**. Nur er zeigt, ob das Modell den Servo wirklich
erklärt.

> **Vorsicht, Falle:** Wer nur an den Anpass-Logs prüft, lernt wie für eine Klassenarbeit mit
> bekannten Aufgaben. Das Prüf-Log ist die neue Aufgabe – andere Last, anderes Signal.

Im Test weicht ein Servo stark vom Nennprofil ab (etwa K −20 %, 30 ms Verzögerung). Nach der
Anpassung an zwei Logs sinkt der Fehler auf dem dritten auf unter ein Fünftel des Startwerts und unter
3 mrad.

Ohne echten Servo hilft `kern_lehrer --beispiele DIR`: Es schreibt Beispiel-Logs (`servo_steps.csv`,
`servo_chirp.csv`, `robot.csv`) und das Profil `xl330.json`, in der Simulation erzeugt: 50 g an
5 cm mit Rechteck ±0,5 rad, 120 g an 6 cm mit Chirp, 250 Regeltakte des ganzen Roboters.

## 16.3 Abgleich: der ganze Roboter

Dafür braucht man ein Log des Roboters: eine Kopfzeile, dann 50 Zeilen pro Sekunde mit je 75 Zahlen
(61 Beobachtungen, 14 Aktionen), beginnend still in Grundstellung. Wie der Roboter es schreibt,
beschreibt dieses Buch nicht. `kern_lehrer --abgleich roboter.csv --profil …` gibt einer sauberen
Ente mit den Servos des Profils Takt für Takt die Aktionen aus dem Log (wie im Training: Scale 1,
kein Tiefpass). Dabei vergleicht es Drehung, Schwerkraft, Gelenkwinkel und -geschwindigkeiten mit
dem Log und meldet den RMS-Fehler je Gruppe und für jedes Gelenk einzeln.

Ein Log aus der Simulation selbst ergibt mit denselben Servos Abstand **0**, Bit für Bit, mit
anderen Servos einen messbaren Abstand. Weil Kern deterministisch ist, ist jeder Unterschied zu einem echten Log ein echter Modellfehler, kein Zufall.

## 16.4 Training mit dem eigenen Profil

`kern_lehrer --task jump --profil xl330_unit3.json` trainiert den Sprung mit den Servos des Profils und
seinen Streuungen. Die Spielschicht nimmt derzeit immer das XL330-Profil.

## 16.5 Domänen-Randomisierung

Was nicht gemessen ist, würfelt das Training ([Kapitel 14](14-lehrer.md)):

| Unsicherheit | Wie das Training sie abdeckt |
|---|---|
| Servo-Exemplare | Reibung, Armatur ±10 % |
| Buslatenz | 15–30 ms |
| Bau | Rumpfmasse ±5 %, Schwerpunkt verschoben |
| Boden | Sohlenreibung × 0,7–1,3 |
| IMU, Encoder | Rauschen, Versatz, Verzögerung |
| Störungen | Stöße alle 3–6 s |
| Kamera | Positionsrauschen, bis 20 % Aussetzer |
| Spielzeug | Ball 3,5–6 cm, 15–50 g |

Der Bewerter im Training (der **Kritiker**) sieht dabei den sauberen Zustand; nur das Netz, das auf
den Roboter kommt, bekommt verrauschte Sensoren. Getriebespiel fehlt im Modell; Pollens Training
variiert es, und velstand ist dagegen robust. Kerns eigene Netze lernen ohne.

> **Mission:** Starte im Micro-Duck-Level **Simulate** (Alt+S), zieh die Ente mit der Maus am Kopf
> zur Seite und lass los, bis sie umfällt. Pollen hat velstand mit vielen Abweichungen trainiert –
> gemessen steht sie in Kern aus Bauch- oder Rückenlage nach höchstens 12 s wieder.

## 16.6 In der Physik messen statt eintragen

Manche Zahlen misst Kern in der Simulation, statt sie in den Code zu schreiben: die **Bissposition**
(an 99 Stellen ausprobiert: wo greift der Schnabel eine Socke am zuverlässigsten?), den **Griff**
über den Mund-Encoder und den **Stillstand** über die Fußkontakte. Noch eingetragen sind der
Nachlauf nach „Stopp“ (0,7 cm aus 0,12 m/s, gemessen mit `--halt-messen`) und die Paar-Reibungen der
Oberflächen – Kandidaten für Messungen am echten Material.

> **Mission:** Starte **Simulate** und schalte unter Window › Show **Contact Points** ein. Du
> siehst, wo die Sohlen den Boden berühren – genau daran erkennt die Spielschicht, ob die Ente steht.

## 16.7 Grenzen

Noch gibt es **keine echten Messdaten**; alles ist mit Logs aus der Simulation geprüft. Offen sind
Getriebespiel, Armatur, rechter Kick, IMU-Profil, Reibungsmischung und Kamera-Detektor. Profile
und Logs sind in echten Einheiten; den Maßstab 4 rechnet Kern nur an der Grenze zu Box3D um.

> **Mission:** Läuft das Level, stopp es mit **Esc**. Wähl die **Micro Duck**, stell im
> Details-Panel **Actions to servo goals** auf `robot`, starte Play und schau ihr beim Laufen zu. Das
> ist die Einstellung, mit der Pollens Software den echten Roboter fährt.

## Level geschafft!

- Du arbeitest am echten Roboter nur mit einem Erwachsenen.
- Du weißt, was ein Servoprofil ist und wozu Streuungen dienen.
- Du kennst den Prüfstand: einspannen, messen, anpassen, prüfen.
- Du kannst einen RMS-Fehler ausrechnen.
- Du verstehst, warum Determinismus den Abgleich so stark macht.

## Rätsel-Ecke

1. Was tust du, bevor du einen echten Servo anschließt?
2. Rechne den RMS-Fehler für die Abweichungen 1, 1, 1 und 5 mrad.
3. Warum gehört ein Log mit anderer Last ins Prüf-Log?
4. **Boss-Rätsel:** Der Abgleich zeigt bei einem Gelenk 10-mal mehr Fehler als bei allen anderen.
   Was schließt du daraus?

### Lösungen

1. Einen Erwachsenen dazuholen, den Prüfstand einspannen, Strom begrenzen, Not-Aus bereitlegen.
2. √((1 + 1 + 1 + 25) / 4) = √7 ≈ 2,6 mrad. Ein großer Ausreißer zählt stark.
3. Nur so zeigt sich, ob das Modell den Servo erklärt oder die Kurven auswendig kann.
4. Dort fehlt etwas im Modell, zum Beispiel Getriebespiel. Zufall kann es nicht sein, weil die
   Simulation deterministisch ist.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
