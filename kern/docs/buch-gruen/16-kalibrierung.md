# 16 Kalibrierung: aus dem Computer in die echte Welt

> **Kurz gesagt:** Ein Netz aus dem Lehrer soll auf der echten Micro Duck laufen. Dafür muss die
> Simulation den Roboter gut treffen, und das Netz darf sich auf nichts verlassen, was die
> Simulation nicht genau weiß. Kern vermisst dazu Servos, vergleicht den ganzen Roboter mit der
> Simulation und deckt den Rest mit Zufall ab.

Kern verfolgt drei Wege und einen Grundsatz:

1. **Gerätemodelle mit Profilen:** Ein Servo mit Regler, Reibung und Verzögerung ist ein kleines
   Modell mit Zahlen in echten Einheiten, angepasst an Messungen und an anderen Messungen geprüft.
2. **Abgleich:** Eine Aufzeichnung des echten Roboters wird in der Simulation nachgespielt; der
   Unterschied ist der verbleibende Abstand zur Wirklichkeit.
3. **Domänen-Randomisierung:** Was die Messung nicht festlegt, variiert das Training zufällig.
4. **Grundsatz:** Was aus Modell und Physik folgt, misst der Lehrer selbst. Box3D bleibt dabei
   unangetastet ([Kapitel 6](06-physik.md)).

> **Stell dir vor …** du übst am Keyboard und spielst morgen auf einem echten Flügel. Du kannst
> prüfen, wie schwer seine Tasten gehen (Gerätemodell), eine Aufnahme vom Flügel mit deinem Üben
> vergleichen (Abgleich) und auf vielen verschiedenen Instrumenten üben (Randomisierung).

> **Achtung, Falle!** Servos unter Last können schnell und kräftig ausschlagen. Prüfstand fest
> einspannen, Arm und Gewicht sichern, Hände und Kabel aus dem Schwenkbereich, Strom begrenzen,
> Not-Aus griffbereit. Nie über die Betriebsspannung des Herstellers gehen, Lithium-Akkus nur mit
> geeignetem Ladegerät und unter Aufsicht laden. Neue Netze zuerst am gehaltenen oder aufgehängten
> Roboter testen. Alles auf eigenes Risiko (siehe [Impressum](00-impressum.md)).

## 16.1 Das Servomodell und sein Profil

Das Servomodell kennst du aus [Kapitel 13](13-micro-duck.md): Zug zum Ziel bis zum Höchstmoment,
Bremsen, Reibung, Rotorträgheit (Armatur), Verzögerung – gerechnet von derselben Funktion im Studio,
im Lehrer und auf dem Prüfstand. Die Zahlen stehen in einem **Profil**, einer JSON-Datei (gekürzt):

```json
{
  "kind": "servo", "name": "xl330",
  "stiffness": 0.554, "damping": 0.053,
  "friction": 0.0048, "torque": 0.963,
  "armature": 0.0018, "delay": 0.02,
  "spreadFriction": 0.1, "spreadArmature": 0.1,
  "delayMin": 0.015, "delayMax": 0.03
}
```

Zeile für Zeile: Profil xl330; wie fest er zieht und bremst; Reibung und Kraft; wie träge und spät
(20 ms) er reagiert; Reibung und Armatur dürfen je Exemplar ±10 % abweichen; im Training streut die
Verzögerung zwischen 15 und 30 ms. Kern prüft jeden Wert auf einen sinnvollen Bereich. Ein anderer Servo ist ein anderes Profil;
ein grundsätzlich anderes Gerät, etwa ein Linearaktor, bekäme ein eigenes Modell.

Die **Streuungen** sind ehrlich zugegebene Unsicherheit: Das Training zieht daraus je Episode und
Servo eigene Werte. Ein gut gemessener Servo bekommt kleine Streuungen, ein schlecht bekannter große.
So hängt das Netz nicht an Zahlen, die niemand genau kennt.

## 16.2 Der Prüfstand: einen Servo vermessen

### So misst man

1. **Aufbau:** Servo einspannen, Achse waagerecht, Arm mit Gewicht; Masse (auf 0,1 g) und
   Schwerpunktabstand messen. Winkel 0 heißt: Arm hängt senkrecht nach unten.
2. **Wie im Roboter betreiben:** gleiche Firmware-Einstellungen, Spannung (7,4 V, sofern der
   Hersteller sie zulässt) und Bus-Takt.
3. **Signale**, mindestens 100-mal pro Sekunde abgefragt: kleine und große Sprünge (etwa ±0,2 und
   ±0,9 rad), ein Chirp (ein Hin und Her mit 0,5 rad, das von 0,5 auf 4 Hz schneller wird) und ein
   Rechteck (±0,3 rad).
4. **Zwei oder drei Lasten**, etwa 50 g an 5 cm und 120 g an 6 cm; mindestens ein Log je Servo nur
   zum Prüfen.
5. **Mehrere Servos** messen; ihre Unterschiede werden zur Streuung.

### Log und Prüfstand im Computer

Ein Prüfstand-Log ist eine Textdatei: oben Kommentarzeilen zur Last (`# mass`, `# arm` …), dann
Zeilen mit Zeit, gesendetem und gemessenem Winkel (`# horizontal 1`: Achse waagerecht, die
Schwerkraft zieht den Arm wie ein Pendel). Kern baut den Servo mit Last in Box3D nach, in derselben Umgebung wie die Ente (Maßstab 4, gleiche
Schrittrate, Gelenkweichheit und Armatur), spielt die gesendeten Ziele mit Verzögerung ab und
vergleicht simulierten und gemessenen Winkel. Der Fehler ist der quadratische Mittelwert, kurz **RMS**.

### Anpassen und prüfen

```
kern_lehrer --kalibrieren steps.csv,chirp.csv \
  --pruefen rechteck.csv --out xl330_unit3.json
```

Das sucht ein Profil, das die ersten beiden Logs am besten erklärt, und prüft es am dritten. Gesucht
wird mit **CMA-ES** ([Kapitel 14](14-lehrer.md)): 12 Kandidaten pro Generation, standardmäßig 80
Generationen. Die sechs Stellschrauben sind die fünf Servowerte (als Faktor zum Startprofil) und die
Verzögerung. Mit `--profil` beginnt die Suche bei einem eigenen Startprofil, `--iterations` ändert
die Zahl der Generationen, `--threads` verteilt die Kandidaten auf mehrere Rechenkerne.

> **Stell dir vor …** zwölf Gitarren mit je sechs Wirbeln, jede etwas anders gestimmt. CMA-ES hört
> hin, welche am besten klingen, und stimmt die nächsten zwölf ähnlich wie die Gewinner.

Entscheidend ist der **Prüf-Fehler** („held out“). Nur er zeigt, ob das Modell den Servo erklärt
oder die Kurven bloß auswendig kann – wie eine Klassenarbeit mit neuen Aufgaben. Darum gehören ins
Prüf-Log ein anderes Signal und eine andere Last.

> **Wusstest du?** Im Test weicht ein Servo absichtlich vom Profil ab (Steifigkeit −20 %, Reibung
> +50 %, 30 ms Verzögerung und mehr). Die Kalibrierung drückt den Prüf-Fehler auf unter ein Fünftel
> und unter 3 Milliradiant.

Ohne echten Servo schreibt `kern_lehrer --beispiele beispiele/` simulierte Beispiel-Logs (Sprünge,
Chirp, ein Roboter-Log) und das Profil, um die Kette einmal durchzuspielen.

> **Probier's aus!** Öffne in Kern Studio auf https://kernengine.de/ das Micro-Duck-Level und
> stelle bei der Ente **Actions to servo goals** auf `robot` (so fährt Pollens Software den echten
> Roboter). Starte Play, übernimm die Ente mit **F8** und kick den Ball mit der 3. Merkst du den
> Unterschied?

## 16.3 Abgleich: der ganze Roboter

Für den **Abgleich** braucht man ein Log des echten Roboters mit 50-mal pro Sekunde allen 61
Beobachtungen und 14 Aktionen, beginnend still in Grundhaltung. (Wie der Roboter es schreibt,
beschreibt dieses Buch nicht.)

```
kern_lehrer --abgleich roboter.csv --profil xl330_unit3.json
```

Die Simulation startet stehend auf dem sauberen Roboter, bekommt Takt für Takt dieselben Aktionen
wie im Training (ohne Abschwächung und Glättung) und vergleicht ihre Sensoren mit dem Log:
Drehung, Schwerkraft, Gelenkwinkel, -geschwindigkeiten – je Gruppe und für jedes Gelenk einzeln.
Bleibt ein Gelenk auffällig, fehlt dort etwas im Modell, etwa Getriebespiel.

Das Schöne: Ein Log aus der Simulation, mit denselben Servos abgespielt, ergibt Abstand **genau 0**,
Bit für Bit, denn die Simulation ist deterministisch; mit anderen Servos ergibt sich ein messbarer
Abstand. Darum ist jeder Unterschied zu einem echten
Log ein echter Modellfehler und kein Rauschen.

> **Probier's aus!** Stell im Micro-Duck-Level unter **World Settings** die Step rate auf 90 statt
> 100 und starte Play. Kern meldet, dass der Takt nicht passt:
> „set World Settings > Step Rate to 100“.

## 16.4 Training mit dem eigenen Profil

```
kern_lehrer --task jump --profil xl330_unit3.json --out jump.onnx
```

Jetzt trainieren die Enten mit den gemessenen Servos samt Streuungen; bei der Bewertung ohne
Variation gilt das Profil exakt. Das wirkt auf die Bewegungen; die Spielschicht nutzt derzeit immer
das XL330-Profil und variiert Reibung und Armatur in dessen Streuung.

## 16.5 Domänen-Randomisierung

Was nicht gemessen ist oder streut, würfelt das Training aus:

| Unsicherheit | Wie das Training sie abdeckt |
|---|---|
| Servo-Exemplare | eigene Werte je Servo (XL330: ±10 %) |
| Buslatenz | 15–30 ms |
| Bau (Akku, Kabel …) | Rumpfmasse ±5 %, Schwerpunkt verschoben |
| Boden | Sohlenreibung × 0,7–1,3 |
| Lagesensor, Gelenkgeber | Rauschen, 0–1 Takt Verzögerung, Versatz bis ±0,015 rad |
| Störungen | Stöße alle 3–6 s, bis ±0,3 m/s |
| Kamera-Detektor | Positionsrauschen, bis 20 % Aussetzer |
| Spielzeug | Ballradius 3,5–6 cm, 15–50 g |

Genaue Bereiche in [Kapitel 14](14-lehrer.md). Ein Trick: Der **Kritiker**, der beim Training
bewertet, sieht immer den sauberen Zustand; nur der **Actor**, der auf den Roboter kommt, muss mit
verrauschten Sensoren klarkommen. Getriebespiel variiert nur Pollens Training über eigene Varianten des Bauplans (velstand ist
dagegen robust); Kerns eigene Netze lernen ohne.

## 16.6 In der Physik messen statt eintragen

Zahlen, die aus Modell und Physik folgen, stimmen als feste Werte im Code nach der nächsten
Änderung nicht mehr. Also misst Kern sie in der Simulation, wie am echten Roboter:

- **Die Bissposition:** Wo muss die Socke liegen, damit das Aufheben sie greift und hält? Der
  Lehrer probiert vor dem ersten Training 99 Stellen und nimmt die zuverlässigste.
- **Der Nachlauf nach „Stopp“** (`--halt-messen`): Hier gilt die Regel noch nicht ganz – das
  Vorbild trägt den gemessenen Wert (0,7 cm aus 0,12 m/s) fest ein; nach einer Änderung der Physik
  misst man neu.
- **Der Griffsensor:** „hält etwas“ ist das, was der Mund-Servo meldet.
- **Stillstand:** gemessen über die Fußkontakte, nicht über „kein Laufbefehl“.

Eingetragen ist dagegen die Reibung der Materialpaare, begründet mit typischen Werten – sie wartet
auf eine Messung am echten Material.

## 16.7 Grenzen und nächste Schritte

- **Noch keine echten Messdaten** – alles ist mit Logs aus der Simulation geprüft.
- **Getriebespiel** fehlt; zeigt der Abgleich es, kommt es ins Servomodell.
- **Armatur** ist eine mitdrehende Scheibe; **Kick rechts** kippt manchmal.
- **Lagesensor** und **Kamera-Detektor** haben noch keine gemessenen Profile.
- **Reibungsmischung** und **Maßstab 4** sind harmlos: Boden und Sohle greifen im Beispiel-Level
  beide mit 1,0, und alles ist in echten Einheiten.

## Das nimmst du mit

- Drei Wege zum echten Roboter: Gerätemodelle, Abgleich, Randomisierung.
- Ein Servo ist ein Profil: wenige Zahlen in echten Einheiten plus Streuungen.
- CMA-ES passt das Profil an Prüfstand-Messungen an; zählen tut der Prüf-Fehler.
- Der Abgleich zeigt je Gelenk, wo Simulation und Roboter auseinanderlaufen.
- Determinismus macht jeden Unterschied zu einem echten Modellfehler.
- Was sich messen lässt, misst Kern, statt es einzutragen.

## Knobelecke

1. Warum reicht es nicht, nur den Fehler auf den Anpassungs-Logs anzuschauen?
2. Ein Servo ist sehr gut vermessen. Sollte seine Streuung groß oder klein sein?
3. Ein Log aus der Simulation wird mit denselben Servos abgeglichen. Welcher Abstand kommt heraus,
   und warum?
4. Warum ist es riskant, die Bissposition als feste Zahl in den Code zu schreiben?

### Lösungen

1. Weil das Profil die Kurven sonst nur auswendig lernen könnte. Erst neue Daten zeigen, ob es den
   Servo wirklich erklärt.
2. Klein. Große Streuungen sind für Werte, die man nicht genau kennt.
3. Genau 0, Bit für Bit, weil die Simulation deterministisch ist.
4. Nach der nächsten Änderung an Reibung, Modell oder Servo stimmt sie nicht mehr. Gemessen passt
   sie immer zur aktuellen Physik.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
