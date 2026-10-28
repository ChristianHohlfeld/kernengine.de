# 17 Zum Nachschlagen

> **Kurz gesagt:** Dieses Kapitel liest man nicht am Stück, man schlägt darin nach. Hier findest du
> die Tasten im Studio und bei der Micro Duck, die wichtigsten Dateiarten und ein großes Glossar mit
> allen Begriffen, über die du in diesem Buch stolpern könntest. Was nur Programmierer brauchen,
> steht im blauen Buch.

Dieses Buch gibt es in drei Fassungen mit demselben Inhalt: das **grüne** (dieses hier, einfach
erklärt), das **blaue** (vollständig und technisch, für Programmierer, mit jeder Funktion und jeder
Option) und das **gelbe** (für Jugendliche). Wenn hier steht „im blauen Buch, Kapitel 17“, ist das
gleichnamige Kapitel der blauen Fassung gemeint.

> **Stell dir vor …** dieses Kapitel ist der Zettel, der innen an der Werkstatttür hängt: Welcher
> Schalter macht was, und was heißt eigentlich „Drehmoment“? Die Bedienungsanleitung jeder Maschine
> steht im Regal (im blauen Buch). Für den Alltag reicht meistens der Zettel.

## 17.1 Tasten im Studio

Kern Studio bedienst du ähnlich wie die großen Spiele-Baukästen, die Profis benutzen. Du musst dir
nichts merken: **F1** zeigt die wichtigsten Kürzel jederzeit im Studio selbst an. Auf dem Mac
versteht Studio die Cmd-Taste wie Ctrl. Alle Einzelheiten stehen in [Kapitel 11](11-studio.md).

### Kamera im Viewport

Der **Viewport** ist das große Fenster in der Mitte, durch das du in die 3D-Welt schaust. Die
Kamera darin steuerst du so:

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste halten + W A S D | fliegen wie in einem Spiel (Fly Mode) |
| … + Q / E | runter / hoch |
| … + Shift | 2,5-mal schneller |
| … + Mausrad | Fluggeschwindigkeit einstellen (Stufe 1–8) |
| Alt + linke Maustaste | um das Ausgewählte herumkreisen wie um eine Skulptur |
| Mittlere Maustaste ziehen | Bild seitlich verschieben |
| F oder Doppelklick in den Viewport | das Ausgewählte ins Bild holen |
| Ctrl+1 … 9 / 1 … 9 | Kameraposition merken / zu ihr zurückspringen |

Die gemerkten Kamerapositionen heißen **Lesezeichen**: wie Lesezeichen in einem Buch, nur für
Blickwinkel. Am Telefon kreist ein Finger, zwei Finger zoomen und verschieben, zweimal Tippen holt
die Auswahl ins Bild, langes Halten öffnet das Kontextmenü.

### Bearbeiten

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Werkzeug: auswählen / verschieben / drehen / skalieren |
| Leertaste | verschieben → drehen → skalieren reihum |
| [ / ] | Einrastraster feiner / gröber |
| Entf | Auswahl löschen |
| End | Auswahl auf den Boden setzen |
| H / Ctrl+H | ausblenden / alles wieder einblenden |
| G | Game View: Editor-Helfer aus, Bild wie im fertigen Spiel |
| F2 | umbenennen |
| Ctrl+Z / Ctrl+Y | rückgängig / wiederholen |
| Ctrl+C / X / V / D | kopieren / ausschneiden / einfügen / duplizieren |
| Ctrl+G | gruppieren |
| Ctrl+S / Ctrl+O / Ctrl+N | speichern / öffnen / neues Level |
| Ctrl+Space | Content Drawer auf- und zuklappen |
| Alt+P / Alt+S | Play / Simulate |

Den **Content Drawer** und das **Output Log** erreichst du auch mit der Maus: Beide haben einen Knopf
in der Statusleiste ganz unten. Das hilft, wenn dein System Ctrl+Space schon für etwas anderes
benutzt, etwa zum Umschalten der Tastatursprache.

> **Wusstest du?** Kopierte Dinge landen als Text in der Zwischenablage deines Computers. Ein Ctrl+V
> in einem anderen Browser-Tab mit Studio fügt sie dort ein – so ziehst du Dinge von einem Level ins
> andere.

### Play und Simulate

Zwei Arten, ein Level laufen zu lassen: Bei **Play** spielst du selbst, die Kamera folgt deiner
Spielfigur. Bei **Simulate** laufen Physik und Spielregeln, aber niemand spielt; du behältst die
freie Kamera des Editors und kannst Körper mit der Maus greifen, ziehen und werfen. In beiden Fällen
ist das Level gesperrt, und beim Stoppen ist alles wieder wie vorher – außer du übernimmst die neuen
Lagen mit **K**.

| Taste | Wirkung |
|---|---|
| Esc | stoppen |
| F8 | Spielfigur loslassen (Eject) / wieder übernehmen (Possess) |
| Pause oder Alt+Shift+P | anhalten / weiter |
| . (Punkt) | einen Schritt weiter, wenn angehalten |
| Ctrl+R | Spiel neu starten |
| K | in Simulate: die Lagen der ausgewählten Dinge ins Level übernehmen |

Im Spiel selbst gilt das Übliche: W A S D bewegen, Maus umsehen, Leertaste springen, Shift sprinten,
F benutzen. Die Tasten des Logik-Editors stehen in [Kapitel 9](09-logik.md).

> **Probier's aus!** Öffne auf https://kernengine.de/ **File › Example Levels › Physics Lab** und
> drück **Alt+S** für Simulate. Drück dann **Pause** (oder Alt+Shift+P) und tippe ein paarmal auf den
> **Punkt**. Du siehst die Physik Schritt für Schritt – wie ein Daumenkino, das du selbst
> umblätterst. Greif zum Schluss mit der linken Maustaste einen Körper und wirf ihn; mit **Esc**
> ist alles wieder an seinem Platz.

## 17.2 Tasten der Micro Duck

| Bewegung | Tastatur | Gamepad | Touch |
|---|---|---|---|
| Laufen / drehen | W S / A D | linker Stick | MOVE |
| Sitzen / Aufstehen | 1 | B | SIT |
| Aufheben (auf Rollschuhen: Ducken) | 2 | Y | PICK |
| Kick links / rechts | 3 / 4 | – | KICK L / R |
| Rolle (halten: weiter) | 5 | X | ROLL |
| Schnabel auf / zu | 6 | – | BEAK |
| Sprung | 7 | – | HOP |
| Quaken / „Wheee“ | Leertaste / V | rechter / linker Trigger | QUACK / WHEEE |

Jede dieser Tasten gibt dir für 2 Sekunden das Steuer. So lange machen das „Gehirn“ der Ente und ihre
Spielschicht (das Netz, das selbst entscheidet, ob sie Ball oder Socke holt) Pause
([Kapitel 13](13-micro-duck.md)).

> **Probier's aus!** Öffne **File › Example Levels › Micro Duck**, drück Play, übernimm die Ente
> mit **F8** und probier alle Tasten von 1 bis 7 der Reihe nach. Drück dann die **1**, damit sie
> sich setzt, und gleich danach die **3**. Nichts passiert: Aus dem Sitzen startet keine Bewegung.
> Erst wenn sie mit der 1 wieder aufgestanden ist, kickt sie.

## 17.3 Dateien

Kern speichert seine Sachen in wenigen Dateiarten. Die meisten sind einfache **Textdateien**: Man
kann sie mit jedem Texteditor öffnen. **JSON** ist ein Textformat aus Namen und Werten („name:
Orb Run“), **CSV** eine Tabelle als Text, eine Zeile pro Zeile, die Spalten durch Kommas getrennt –
CSV-Dateien öffnet auch jede Tabellenkalkulation.

| Datei | Was drin ist | mehr in |
|---|---|---|
| Level (`.klevel`) | eine ganze Spielwelt, als JSON | [Kapitel 8](08-spiel-laufzeit.md), [11](11-studio.md) |
| Netz (`.onnx`) | ein trainiertes Netz | [Kapitel 13](13-micro-duck.md), [14](14-lehrer.md) |
| Servoprofil (JSON) | die Zahlen eines Servotyps | [Kapitel 16](16-kalibrierung.md) |
| Messungen (CSV) | Aufzeichnungen vom Prüfstand oder Roboter | [Kapitel 16](16-kalibrierung.md) |

**Levels.** Ein Level enthält alles, was in der Welt ist: die Dinge mit ihren Bausteinen, die World
Settings, die Kameraposition des Editors, eingebettete Bilder und Klänge und die Logik. Gerechnet
wird in Metern, Sekunden und Kilogramm, Drehungen stehen in Grad. Eine fremde Datei oder eine aus
einem neueren Studio öffnet Kern lieber gar nicht, statt sie halb falsch zu lesen.

**Netze.** Kern liest und schreibt Netze in genau der Form, in der Pollen Robotics sie für die Micro
Duck verwendet. Darum laufen Pollens Netze in Kern und Kerns Netze auf dem Roboter. Ein Netz, das du
im Browser mit dem Lehrer trainierst, bleibt auf deinem Gerät gespeichert und ersetzt dort das
mitgelieferte.

Die genauen Formate stehen im blauen Buch, Kapitel 17.

## 17.4 Was nur im blauen Buch steht

Zwei Teile der Referenz sind reine Programmierer-Werkzeuge und stehen deshalb nur im blauen Buch,
Kapitel 17:

- **Der Lehrer als Kommandozeilenprogramm:** Neben der Lehrer-Seite im Browser gibt es den Lehrer
  auch als Programm, das man mit getippten Befehlen startet – zum Trainieren, Bewerten, Kalibrieren
  und Messen. Alle Befehle mit ihren Einstellungen und Beispielen stehen dort.
- **Die Schnittstelle zwischen Engine und Browser:** Kern ist in C++ geschrieben und läuft im Browser
  als WebAssembly; die Webseite drumherum ist JavaScript. Die beiden reden nur über eine genau
  festgelegte Liste von Funktionen miteinander, wie über die Durchreiche einer Kantine.

> **Wusstest du?** Die Webseite des Studios enthält keine Editorlogik. Menüs, Outliner, Details,
> Gizmos, Rückgängig – all das steckt in der Engine. Die Seite fragt nur über die Durchreiche, was
> sie anzeigen soll, und zeichnet es. Der Vorteil: Im Editor läuft genau derselbe Code wie im
> fertigen Spiel, und der ganze Editor lässt sich ohne Browser testen.

## 17.5 Glossar

Alle Fachwörter des grünen Buchs, kurz in Alltagssprache. In Klammern steht, wo sie ausführlich
vorkommen.

> **Achtung, Falle!** Das Wort **Actor** hat in diesem Buch zwei Bedeutungen. Im **Studio** ist ein
> Actor ein Ding im Level: eine Kiste, eine Lampe, die Ente. Beim **Lernen** (Kapitel 14, Anhang A)
> ist der Actor das Netz, das handelt – im Gegensatz zum Kritiker, der nur bewertet. Welche Bedeutung
> gemeint ist, verrät immer das Kapitel.

### Studio und Spiele

| Begriff | Bedeutung |
|---|---|
| **Actor (im Studio)** | ein Ding im Level mit Position und Bausteinen ([Kap. 8](08-spiel-laufzeit.md)) |
| **Autosave** | Studio sichert deine Arbeit regelmäßig von selbst |
| **Blueprint / Logik-Graph** | ein Programm aus Kästchen und Verbindungslinien, ohne Tippen ([Kap. 9](09-logik.md)) |
| **Collider** | die unsichtbare Form, mit der ein Ding in der Physik anstößt |
| **Content Drawer** | die Schublade unten im Studio: Bauteile, Bilder, Klänge, Output Log |
| **Details** | das Panel mit allen Einstellungen des ausgewählten Dings |
| **Eject / Possess** | im Spiel die Spielfigur loslassen / wieder übernehmen (F8) |
| **Example Levels** | fertige Beispielwelten im Menü File, etwa Orb Run oder Micro Duck |
| **Game View** | Ansicht ohne Editor-Helfer, so wie im fertigen Spiel |
| **Gizmo** | Pfeile und Ringe am ausgewählten Ding zum Verschieben, Drehen, Skalieren |
| **HUD** | Anzeigen über dem Spielbild: Punkte, Zeit, Tempo |
| **Knoten / Pin** | Kästchen im Logik-Graph / ihre Anschlüsse für Verbindungslinien |
| **Komponente** | ein Baustein eines Actors, etwa Licht, Klang oder Physik |
| **Lesezeichen** | eine gemerkte Kameraposition (Ctrl+1 … 9) |
| **Level** | eine Spielwelt, gespeichert als `.klevel` |
| **Outliner** | die Liste aller Dinge im Level, als Baum geordnet |
| **Output Log** | die Liste der Meldungen des Studios, mit Uhrzeit |
| **Package Game** | ein Level als eigenständige Webseite zum Weitergeben speichern |
| **Pickup** | etwas zum Einsammeln, etwa ein leuchtender Orb |
| **Play / Simulate** | selbst spielen / Physik laufen lassen und zuschauen (17.1) |
| **Trigger** | eine unsichtbare Zone, die etwas auslöst, wenn jemand hineingerät |
| **Viewport** | das große Fenster, durch das du in die 3D-Welt schaust |
| **World Settings** | Einstellungen der ganzen Welt, etwa Schwerkraft und Schrittrate |

### Technik unter der Haube

| Begriff | Bedeutung |
|---|---|
| **Box3D** | die Physik-Engine, die Kern unverändert mitbringt ([Kap. 6](06-physik.md)) |
| **C++** | die Programmiersprache, in der Kern geschrieben ist |
| **Compiler, kompilieren** | Programmtext in Befehle übersetzen, die der Rechner direkt ausführt |
| **Determinismus** | gleiche Eingaben ergeben Bit für Bit dasselbe Ergebnis, auf jedem Gerät |
| **Fingerabdruck** | eine Prüfzahl pro Takt; stimmt sie überall, rechnet Kern überall gleich |
| **Frame** | ein einzelnes Bild auf dem Bildschirm; viele pro Sekunde ergeben Bewegung |
| **Gelenk (Constraint)** | eine Verbindung zwischen zwei Körpern, etwa ein Scharnier |
| **GTAO** | ein Verfahren, das Ecken und Ritzen natürlich abdunkelt ([Kap. 4](04-grafik.md)) |
| **Instanz** | eine Kopie derselben Form an anderer Stelle, wie hundert Bäume aus einer Vorlage |
| **Interpreter** | führt einen Logik-Graphen Kästchen für Kästchen aus, wie ein Dolmetscher |
| **JavaScript** | die Programmiersprache der Webseiten |
| **JIT** | übersetzt einen Logik-Graphen beim Start in schnellen Maschinencode |
| **Kontakt** | eine Stelle, an der sich zwei Körper berühren |
| **Mesh** | eine 3D-Form, gebaut aus vielen kleinen Dreiecken |
| **Partikel** | viele winzige Teilchen für Funken, Staub oder Rauch |
| **Physikschritt** | ein Bild im Daumenkino der Physik; die Welt rückt ein Stückchen weiter |
| **Schrittrate** | wie viele Physikschritte pro Sekunde gerechnet werden; für die Ente 100 |
| **Server / Client** | beim Mehrspieler: der Rechner, der bestimmt / die Rechner, die mitspielen |
| **Shader** | ein kleines Programm der Grafikkarte, das die Farbe jedes Bildpunkts ausrechnet |
| **Starrer Körper** | ein Körper, der sich nie verbiegt; so rechnet Box3D alle Körper |
| **Sub-Steps** | Unterschritte innerhalb eines Physikschritts, für mehr Genauigkeit |
| **Textur** | ein Bild, das auf eine Oberfläche geklebt wird |
| **Thread, Worker** | ein Arbeitsstrang; mehrere rechnen gleichzeitig auf mehreren Prozessorkernen |
| **WebAssembly** | ein Programmformat, das Browser fast so schnell ausführen wie echte Programme |

### Roboter und Messen

| Begriff | Bedeutung |
|---|---|
| **Abgleich** | eine Aufzeichnung des echten Roboters in der Simulation nachspielen und vergleichen ([Kap. 16](16-kalibrierung.md)) |
| **Armatur** | Zusatzträgheit, weil der flinke Motor im Servo jede Bewegung mitmachen muss |
| **CSV** | eine Tabelle als Textdatei, Spalten durch Kommas getrennt |
| **Drehmoment** | Drehkraft: wie fest ein Motor an einem Gelenk drehen kann |
| **Gelenksensor (Encoder)** | misst im Servo, welchen Winkel das Gelenk gerade hat |
| **Gerätemodell** | ein kleines Rechenmodell eines echten Geräts, etwa eines Servos |
| **Getriebespiel** | ein bisschen Wackeln im Getriebe, bevor die Zahnräder greifen |
| **Grundhaltung** | die Ausgangsstellung der Gelenke, wenn die Ente ruhig steht |
| **Hz (Hertz)** | „mal pro Sekunde“: 50 Hz heißt 50-mal pro Sekunde |
| **JSON** | ein Textformat aus Namen und Werten |
| **Kalibrierung** | die Zahlen eines Modells an Messungen anpassen |
| **Kamera-Detektor** | meldet, wo die Kamera der Ente Ball oder Socke sieht; Kern ahmt ihn nach |
| **Lagesensor (IMU)** | spürt, wo unten ist und wie schnell sich der Körper dreht |
| **Log** | eine Aufzeichnung, Zeile für Zeile: was gesendet und was gemessen wurde |
| **Micro Duck** | der zweibeinige Laufroboter von Pollen Robotics ([Kap. 13](13-micro-duck.md)) |
| **ms (Millisekunde)** | eine tausendstel Sekunde |
| **Prüfstand** | ein einzelner Servo mit Arm und Gewicht, festgespannt zum Vermessen |
| **Prüf-Fehler** | der Fehler auf einer Messung, die beim Anpassen nicht benutzt wurde |
| **rad (Radiant)** | ein Winkelmaß der Technik: 1 rad sind etwa 57 Grad, ein ganzer Kreis etwa 6,28 rad |
| **Milliradiant** | ein Tausendstel rad, etwa 0,06 Grad |
| **Regeltakt, Takt** | der Herzschlag der Steuerung: 50-mal pro Sekunde bekommt jeder Servo ein neues Ziel |
| **RMS-Fehler** | der durchschnittliche Abstand zweier Kurven, große Ausreißer zählen stärker |
| **Servo** | ein Motor, der auf einen vorgegebenen Winkel fährt |
| **Servoprofil** | die wenigen Zahlen, die einen Servotyp beschreiben ([Kap. 16](16-kalibrierung.md)) |
| **Sim-to-Real-Lücke** | der Unterschied zwischen Simulation und echtem Roboter |
| **Streuung** | wie weit ein Wert schwanken darf oder von Exemplar zu Exemplar schwankt |
| **ToF** | ein Abstandssensor, der misst, wie lange Licht hin und zurück braucht |
| **Walker** | die Steuerart im Studio für Laufroboter wie die Ente, die ein Netz bewegt |

Ein Bild zu **rad**: Leg eine Schnur, so lang wie der Radius eines Kreises, auf seinen Rand. Der
Winkel, den sie vom Mittelpunkt aus überspannt, ist 1 rad. Rund sechseinviertel solcher Schnüre
reichen einmal um den Kreis.

### Netze und Lernen

| Begriff | Bedeutung |
|---|---|
| **Actor (beim Lernen)** | das Netz, das handelt und später auf den Roboter kommt ([Anhang A](A-grundlagen-neuronale-netze.md)) |
| **Adam** | ein kluges Verfahren, die Regler in gleichmäßigen, ruhigen Schritten zu verstellen |
| **Aktion** | was das Netz in einem Takt tut: 14 Zielwinkel für die Servos |
| **Aktivierung (ELU)** | das „Verbiegen“ im Neuron: positive Zahlen bleiben, negative werden gestaucht |
| **asymmetrischer Kritiker** | der Kritiker sieht beim Üben mehr als der Actor, etwa die genaue Höhe |
| **Backpropagation** | den Fehler vom Ausgang rückwärts auf alle Regler verteilen |
| **Beobachtung** | was die Ente in einem Takt misst: 61 Zahlen über Lage, Gelenke, Befehle |
| **Belohnung** | Punkte pro Takt, die sagen, wie gut die Ente gerade war |
| **Bias** | die Grundeinstellung eines Neurons, die immer dazugezählt wird |
| **CMA-ES** | Suchverfahren: eine Wolke von Kandidaten probieren, zu den besseren hinrücken |
| **DAgger** | Nachahmen, bei dem das Netz selbst fährt und das Vorbild jede Lage kommentiert |
| **Entropie** | ein Maß für die Neugier: wie breit die Ente um ihre beste Idee herum ausprobiert |
| **Epoche** | einmal alle gesammelten Übungsdaten durchgehen |
| **Episode** | ein Übungsdurchgang, vom Start bis zum Sturz oder bis die Zeit um ist |
| **Gewicht** | ein Regler im Netz: wie stark ein Eingang zählt |
| **Gradient** | die Steigung der Verlust-Landschaft: in welche Richtung es bergauf geht |
| **Iteration** | eine Trainingsrunde: alle Enten üben ein Stück, dann wird das Netz einmal verbessert |
| **Kritiker** | das zweite Netz beim Üben; es schätzt, wie gut eine Lage ist, und handelt nie |
| **Kritiker zuerst** | nach dem Nachahmen lernt eine Weile nur der Kritiker, bis seine Urteile taugen |
| **Lehrer** | Kerns Trainingssystem für die Netze der Micro Duck ([Kap. 14](14-lehrer.md)) |
| **Lehrplan** | Schwierigkeiten kommen erst nach und nach dazu, wie beim Klavierüben |
| **Lernrate** | die Schrittlänge beim Verstellen der Regler |
| **Minibatch** | eine kleine, zufällige Handvoll Übungsdaten für einen Lernschritt |
| **Nachahmen (Verhaltensklonen)** | das Netz lernt, zu tun, was ein Vorbild vormacht |
| **Netz (neuronales Netz)** | ein Rechenwerk mit vielen Reglern: Zahlen rein, Zahlen raus |
| **Neuron** | der kleinste Baustein: malnehmen, zusammenzählen, verbiegen |
| **Normalisierer** | rechnet jeden Messwert auf ein gemeinsames Maß um: „wie weit über dem Üblichen?“ |
| **ONNX** | ein Austauschformat für Netze, das viele Programme lesen können |
| **Phasensignal** | eine Art Uhr, die dem Sprung-Netz sagt, wo im Sprung es gerade ist |
| **Policy** | die Entscheidungsregel der Ente, also ihr trainiertes Netz |
| **PPO** | das Lernverfahren des Lehrers: aus Belohnung lernen, in kleinen, vorsichtigen Schritten |
| **Rauschen, σ (Sigma)** | zufällige Abweichung; σ sagt, wie breit sie ist |
| **Return** | die Gesamtpunktzahl ab jetzt, spätere Punkte etwas weniger wert |
| **Rezept** | alle Trainingseinstellungen einer Aufgabe zusammen |
| **Rollout** | gesammelte Erfahrung: was beobachtet, getan und belohnt wurde |
| **Sammler / Lerner** | die Teile des Trainings, die Erfahrung sammeln / daraus lernen |
| **Schicht** | viele Neuronen nebeneinander, die dieselben Eingänge sehen |
| **Seed** | die Startzahl für den Zufall: gleicher Seed, gleicher Zufall |
| **Spielschicht** | Kerns Netz, das entscheidet, was die Ente mit Ball und Socke tut |
| **Überanpassung** | Auswendiglernen statt Verstehen: gut bei Übungen, schlecht bei Neuem |
| **Variation** | jede Episode würfelt etwas andere Bedingungen aus; Fachwort Domänen-Randomisierung |
| **velstand** | Pollens Netz zum Laufen und Stehen, in Kern fest eingebaut |
| **Verlust** | eine Zahl, die sagt, wie falsch das Netz gerade liegt; kleiner ist besser |
| **Vorbild** | eine Vorführung, die ein Netz zuerst nachahmt |
| **Vorteil** | wie viel besser eine Aktion war als das, was die Ente hier sonst tut |
| **Warmstart** | ein neues Training beginnt mit einem schon trainierten Netz statt mit Zufall |
| **Wert** | wie viele Punkte die Ente von einer Lage aus noch erwarten kann |

Die genauen Definitionen stehen im Glossar des blauen Buchs, Kapitel 17.

## Das nimmst du mit

- **F1** im Studio zeigt die wichtigsten Kürzel; Alt+P spielt, Alt+S simuliert, Esc stoppt.
- Play heißt selbst spielen, Simulate heißt zuschauen und Körper greifen.
- Die Ente steuerst du mit W A S D und den Tasten 1 bis 7.
- Levels sind Textdateien, Netze `.onnx` im selben Format wie bei Pollen Robotics.
- Der ganze Editor steckt in der Engine; die Webseite zeichnet nur, was die Engine ihr sagt.
- „Actor“ heißt im Studio ein Ding im Level, beim Lernen das handelnde Netz.
- Für jede Funktion, jede Option und jedes Dateiformat schlägst du im blauen Buch nach.

## Knobelecke

1. Du hast eine Kiste im Viewport ausgewählt, sie ist aber nicht zu sehen. Was hilft?
2. Du liest „Der Actor lernt aus den Vorteilen, die der Kritiker schätzt.“ Ist hier eine Kiste im
   Level gemeint?
3. Du drückst bei der Ente die 3, aber nichts passiert. Woran könnte es liegen?
4. Was ist der Unterschied zwischen Play und Simulate?

### Lösungen

1. **F** holt die Auswahl ins Bild, ein Doppelklick in den Viewport auch.
2. Nein. Neben dem Kritiker ist der Actor das handelnde Netz beim Lernen, kein Ding im Level.
3. Vielleicht sitzt die Ente gerade: Aus dem Sitzen startet keine Bewegung. Erst mit der 1
   aufstehen lassen. (Und F8 nicht vergessen, damit du die Ente überhaupt steuerst.)
4. Bei Play spielst du selbst, die Kamera folgt der Spielfigur. Bei Simulate laufen Physik und Regeln
   ohne Spieler; du behältst die freie Kamera und kannst Körper greifen und mit K ihre neuen Lagen
   übernehmen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
