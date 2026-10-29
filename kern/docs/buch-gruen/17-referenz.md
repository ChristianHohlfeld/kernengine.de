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
nichts merken: **F1** zeigt die wichtigsten Kürzel jederzeit im Studio selbst an. Die Tasten heißen
hier wie auf deutschen Tastaturen: **Strg**, **Umschalt**, **Entf**, **Leertaste** – so zeigt sie
auch Studio auf Deutsch an. Nur die Taste **Ende** steht in Studios Menüs als „End“. Auf dem Mac
versteht Studio die Cmd-Taste wie Strg. Alle Einzelheiten stehen in
[Kapitel 11](11-studio.md).

### Kamera im Viewport

Der **Viewport** ist das große Fenster in der Mitte, durch das du in die 3D-Welt schaust. Die
Kamera darin steuerst du so:

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste halten + W A S D | fliegen wie in einem Spiel (Fly Mode) |
| … + Q / E | runter / hoch |
| … + Umschalt | schneller fliegen |
| … + Mausrad | Fluggeschwindigkeit einstellen |
| Alt + linke Maustaste | um das Ausgewählte herumkreisen wie um eine Skulptur |
| Mittlere Maustaste ziehen | Bild seitlich verschieben |
| F | das Ausgewählte ins Bild holen |
| Doppelklick auf ein Ding | beim Bearbeiten: es auswählen und ins Bild holen |
| Strg+1 … 9 / 1 … 9 | Kameraposition merken / zu ihr zurückspringen |

Die gemerkten Kamerapositionen heißen **Lesezeichen** (Bookmarks): wie Lesezeichen in einem Buch,
nur für Blickwinkel. Merken und Zurückspringen klappt im Bearbeiten-Modus; beim Spielen steuern die
Ziffern zum Beispiel die Ente. Ein Doppelklick ins Leere holt nichts ins Bild: Der erste Klick ins Leere hebt
die Auswahl auf. Am Telefon kreist ein Finger, zwei Finger zoomen und verschieben, zweimal Tippen
holt die Auswahl ins Bild, langes Halten öffnet das Kontextmenü.

### Bearbeiten

Hier drückst du die Tasten **ohne** gehaltene rechte Maustaste. Mit gehaltener rechter Maustaste
fliegt die Kamera, und Q und E heißen dann „runter“ und „hoch“ (siehe oben).

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Werkzeug: auswählen / verschieben / drehen / skalieren (Größe ändern) |
| Leertaste | verschieben → drehen → skalieren reihum |
| [ / ] | Einrastraster feiner / gröber (das Gitter, an dem Dinge beim Verschieben einrasten) |
| Entf | Auswahl löschen |
| Ende | Auswahl auf den Boden setzen |
| H / Strg+H | ausblenden / alles wieder einblenden |
| G | Spielansicht (Game View): Editor-Helfer aus, Bild wie im fertigen Spiel |
| F2 | umbenennen |
| Strg+Z / Strg+Y | rückgängig / wiederholen |
| Strg+C / X / V / D | kopieren / ausschneiden / einfügen / duplizieren |
| Strg+G | gruppieren |
| Strg+S / Strg+O / Strg+N | speichern / öffnen / neues Level |
| Strg+Leertaste | Inhaltsablage (Content Drawer) auf- und zuklappen |
| Alt+P / Alt+S | Spielen (Play) / Simulieren (Simulate) |

Die **Inhaltsablage** (Content Drawer) ist die Schublade unten im Studio mit Bauteilen, Bildern und
Klängen; das **Ausgabeprotokoll** (Output Log) ist die Liste der Meldungen des Studios. Beide
erreichst du auch mit der Maus, über einen Knopf in der Statusleiste ganz unten. Das hilft, wenn dein
System Strg+Leertaste schon für etwas anderes benutzt, etwa zum Umschalten der Tastatursprache.

> **Wusstest du?** Kopierte Dinge landen als Text in der Zwischenablage deines Computers. Ein Strg+V
> in einem anderen Browser-Tab mit Studio fügt sie dort ein – so ziehst du Dinge von einem Level ins
> andere.

### Spielen und Simulieren

Zwei Arten, ein Level laufen zu lassen: Bei **Spielen** (Play) spielst du selbst, die Kamera folgt
deiner Spielfigur. Manche Level, etwa die Micro Duck, starten das Spielen aber als Zuschauer mit
freier Kamera, eingestellt in den **Welteinstellungen** (World Settings) mit dem Feld
**Beobachterkamera (Spielen: fliegen, zuschauen, Körper ziehen)**
(Observer camera (Play: fly, watch, drag bodies)); mit **F8** übernimmst du dann die Figur. Bei
**Simulieren** (Simulate) laufen Physik und Spielregeln, aber niemand spielt; du behältst die freie Kamera des Editors und
kannst Körper mit der Maus greifen, ziehen und werfen. In beiden Fällen ist das Level gesperrt, und
beim Stoppen ist alles wieder wie vorher – außer du übernimmst die neuen Lagen mit **K**.

| Taste | Wirkung |
|---|---|
| Esc | stoppen |
| F8 | Spielfigur loslassen / wieder übernehmen (Menü: Aus dem Spieler aussteigen / Spieler übernehmen) |
| Pause oder Alt+Umschalt+P | anhalten / weiter |
| . (Punkt) | einen Schritt weiter, wenn angehalten |
| Strg+R | Spiel neu starten |
| K | beim Simulieren: die Lagen der ausgewählten Dinge ins Level übernehmen |

Im Spiel selbst gilt das Übliche: W A S D bewegen, Maus umsehen, Leertaste springen, Umschalt sprinten,
F benutzen. Die Taste F hat also zwei Aufgaben: Steuerst du eine Spielfigur, benutzt sie etwas;
sonst holt sie die Auswahl ins Bild. Die Tasten des Logik-Editors stehen in [Kapitel 9](09-logik.md).

> **Probier's aus!** Öffne auf https://kernengine.de/
> **Datei › Beispiel-Levels › Physics Lab** (File › Example Levels › Physics Lab) und drück **Alt+S**
> für **Simulieren**. Drück dann **Pause** und tippe ein paarmal auf den
> **Punkt**. Du siehst die Physik Schritt für Schritt – wie ein Daumenkino, das du selbst
> umblätterst. Drück noch einmal **Pause**, damit die Zeit weiterläuft. Greif dann mit der linken
> Maustaste einen Körper und wirf ihn; mit **Esc** ist alles wieder an seinem Platz. (Viele Laptops
> haben keine Pause-Taste; dann nimm Alt+Umschalt+P.)

## 17.2 Tasten der Micro Duck

| Bewegung | Tastatur | Gamepad | Touch |
|---|---|---|---|
| Laufen / drehen | W S / A D | linker Stick | LAUFEN (MOVE) |
| Sitzen / Aufstehen | 1 | B | SITZ (SIT) |
| Aufheben (trägt sie Rollschuhe: Ducken) | 2 | Y | GREIFEN (PICK) |
| Kick links / rechts | 3 / 4 | – | KICK L / KICK R |
| Rolle (halten: weiter) | 5 | X | ROLLEN (ROLL) |
| Schnabel auf / zu | 6 | – | SCHNABEL (BEAK) |
| Sprung | 7 | – | HÜPFEN (HOP) |
| Quaken / „Wheee“ | Leertaste / V | rechter / linker Trigger | QUAK / HUIII (QUACK / WHEEE) |

Die Ente gibt es auch als Variante mit Rollschuhen, „Micro Duck (Rollers)“; sie fährt dann, statt
zu gehen ([Kapitel 13](13-micro-duck.md)).

Mit **F8** übernimmst du die Ente; dann hören W A S D und die Zahlentasten auf dich. Jede Eingabe
zählt als „Hand am Steuer“ für 2 Sekunden. So lange halten Pollens Entscheidungslogik, die
[Kapitel 13](13-micro-duck.md) das **Gehirn** der Ente nennt, und die Spielschicht (Kerns Netz, das
selbst entscheidet, ob sie Ball oder Socke holt) still. Drückst du länger nichts, entscheidet die
Ente wieder selbst, sofern im Details-Panel **Autonom (Kamera, ToF)** (Autonomous (camera, ToF)) an
ist.

> **Probier's aus!** Öffne
> **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck), drück **Spielen**
> (Play), übernimm die Ente
> mit **F8** und probier die Tasten von 2 bis 7 der Reihe nach. Warte nach jeder Taste, bis die
> Bewegung fertig ist: Solange eine läuft, startet keine neue. Drück dann die **1**, damit sie sich
> setzt, und gleich danach die **3**. Nichts passiert: Aus dem Sitzen startet keine Bewegung. Warte,
> bis sie ganz sitzt, und drück wieder die **1** – erst wenn sie aufgestanden ist, kickt sie.

## 17.3 Dateien

Kern speichert seine Sachen in wenigen Dateiarten. Die meisten sind einfache **Textdateien**: Man
kann sie mit jedem Texteditor öffnen. **JSON** ist ein Textformat aus Namen und Werten („name: Orb
Run“), **CSV** eine Tabelle als Text: eine Tabellenzeile pro Textzeile, die Spalten durch Kommas
getrennt – CSV-Dateien öffnet auch jede Tabellenkalkulation.

| Datei | Was drin ist | mehr in |
|---|---|---|
| Level (`.klevel`) | eine ganze Spielwelt, als JSON | [Kapitel 8](08-spiel-laufzeit.md), [11](11-studio.md) |
| Netz (`.onnx`) | ein trainiertes Netz | [Kapitel 13](13-micro-duck.md), [14](14-lehrer.md) |
| Servoprofil (JSON) | die Zahlen eines Servotyps | [Kapitel 16](16-kalibrierung.md) |
| Messungen (CSV) | Aufzeichnungen vom Prüfstand oder Roboter | [Kapitel 16](16-kalibrierung.md) |

**Levels.** Ein Level enthält alles, was in der Welt ist: die Dinge mit ihren Bausteinen, die
Welteinstellungen, die Kameraposition des Editors, eingebettete Bilder und Klänge und die Logik. Eine fremde
Datei oder eine aus einem neueren Studio öffnet Kern lieber gar nicht, statt sie halb falsch zu
lesen.

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
- **Die Schnittstelle zwischen Engine und Browser:** Die **Engine** ist das Herz von Kern, das
  Programm, das rechnet, zeichnet und die Physik laufen lässt. Sie ist in einer Programmiersprache
  für schnelle Programme geschrieben (C++) und läuft im Browser in einem schnellen Programmformat
  (WebAssembly); die Webseite drumherum ist in der Sprache der Webseiten geschrieben (JavaScript).
  Die beiden reden nur über eine genau festgelegte Liste von Funktionen miteinander, wie über die
  Durchreiche einer Kantine. Geht dabei etwas schief, bekommt die Webseite eine Meldung zurück,
  statt dass das Programm abstürzt. Und die Engine fasst selbst keine Dateien auf deinem Gerät an:
  Was sie braucht, reicht ihr die Webseite durch.

> **Wusstest du?** Die Webseite des Studios enthält keine Editorlogik. Menüs, Outliner, Details,
> Gizmos, Rückgängig – all das steckt in der Engine. Die Seite fragt nur über die Durchreiche, was
> sie anzeigen soll, und zeichnet es. Der Vorteil: Im Editor läuft genau derselbe Code wie im
> fertigen Spiel, und der ganze Editor lässt sich ohne Browser testen. So finden automatische
> Prüfungen Fehler, ohne dass jemand klicken muss.

## 17.5 Glossar

Die Fachwörter des grünen Buchs, kurz in Alltagssprache. In Klammern steht, wo sie ausführlich
vorkommen. Fehlt dir ein Wort, hilft das Glossar des blauen Buchs weiter.

> **Achtung, Falle!** Das Wort **Actor** hat in diesem Buch zwei Bedeutungen. Im **Studio** ist ein
> Actor ein Ding im Level: eine Kiste, eine Lampe, die Ente. Beim **Lernen** (Kapitel 14, Anhang A)
> ist der Actor das Netz, das handelt – im Gegensatz zum Kritiker, der nur bewertet. Welche Bedeutung
> gemeint ist, verrät immer das Kapitel.

### Studio und Spiele

| Begriff | Bedeutung |
|---|---|
| **Actor (im Studio)** | ein Ding im Level mit Position und Bausteinen ([Kap. 8](08-spiel-laufzeit.md)) |
| **Ausgabeprotokoll** (Output Log) | die Liste der Meldungen des Studios, mit Uhrzeit |
| **Aussteigen / übernehmen (F8)** (Eject / possess (F8)) | im Spiel die Spielfigur loslassen / wieder übernehmen |
| **Autosave** | Studio sichert deine Arbeit regelmäßig von selbst |
| **Beispiel-Levels** (Example Levels) | fertige Beispielwelten im Menü **Datei** (File), etwa Orb Run oder Micro Duck |
| **Blueprint / Logik-Graph** | ein Programm aus Kästchen und Verbindungslinien, ohne Tippen ([Kap. 9](09-logik.md)) |
| **Details** | das Panel mit allen Einstellungen des ausgewählten Dings |
| **GameWorld** | der Teil von Kern, der ein Level zum Leben erweckt und laufen lässt ([Kap. 8](08-spiel-laufzeit.md)) |
| **Gizmo** | Pfeile und Ringe am ausgewählten Ding zum Verschieben, Drehen, Skalieren |
| **HUD** | Anzeigen über dem Spielbild: Punkte, Zeit, Tempo |
| **Inhaltsablage** (Content Drawer) | die Schublade unten im Studio: Bauteile, Bilder, Klänge, Ausgabeprotokoll |
| **Kern Studio, Studio** | der Editor im Browser, in dem du Level baust und spielst ([Kap. 11](11-studio.md)) |
| **Knoten / Pin** | Kästchen im Logik-Graph / ihre Anschlüsse für Verbindungslinien |
| **Kollider** (Collider) | die unsichtbare Form, mit der ein Ding in der Physik anstößt |
| **Komponente** | ein Baustein eines Actors, etwa Licht, Klang oder Physik |
| **Lesezeichen** (Bookmarks) | eine gemerkte Kameraposition (Strg+1 … 9) |
| **Level** | eine Spielwelt, gespeichert als `.klevel` |
| **Outliner** | die Liste aller Dinge im Level, als Baum geordnet |
| **Pickup** | etwas zum Einsammeln, etwa ein leuchtender Orb |
| **Spiel verpacken** (Package Game) | ein Level als eigenständige Webseite zum Weitergeben speichern |
| **Spielansicht** (Game View) | Ansicht ohne Editor-Helfer, so wie im fertigen Spiel |
| **Spielen / Simulieren** (Play / Simulate) | selbst spielen / Physik laufen lassen und zuschauen (17.1) |
| **Trigger** | eine unsichtbare Zone, die etwas auslöst, wenn jemand hineingerät |
| **Viewport** | das große Fenster, durch das du in die 3D-Welt schaust |
| **Welteinstellungen** (World Settings) | Einstellungen der ganzen Welt, etwa Schwerkraft und Schrittrate |

### Technik unter der Haube

| Begriff | Bedeutung |
|---|---|
| **Bit** | die kleinste Einheit im Computer: ein Kästchen, in dem 0 oder 1 steht |
| **Bloom** | das Glühen um sehr helle Stellen, wie Gegenlicht ([Kap. 4](04-grafik.md)) |
| **Box3D** | die Physik-Engine, die Kern unverändert mitbringt ([Kap. 6](06-physik.md)) |
| **C++** | die Programmiersprache, in der Kern geschrieben ist |
| **Compiler, kompilieren** | Programmtext in Befehle übersetzen, die der Rechner direkt ausführt |
| **Determinismus** | gleiche Eingaben ergeben bis in die letzte Stelle dasselbe Ergebnis, auf jedem Gerät |
| **Engine** | das Herz von Kern: das Programm, das rechnet, zeichnet und die Physik laufen lässt |
| **Fingerabdruck** | eine Prüfzahl pro Takt; stimmt sie überall, rechnet Kern überall gleich |
| **Frame** | ein einzelnes Bild auf dem Bildschirm; viele pro Sekunde ergeben Bewegung |
| **Gelenk** (Constraint) | eine Verbindung zwischen zwei Körpern, etwa ein Scharnier |
| **GTAO** | ein Verfahren, das Ecken und Ritzen natürlich abdunkelt ([Kap. 4](04-grafik.md)) |
| **Hash** | eine kurze Zahl aus beliebig vielen Daten; gleiche Daten, gleiche Zahl ([Kap. 3](03-grundlagen.md)) |
| **Instanz** | eine Kopie derselben Form an anderer Stelle, wie hundert Bäume aus einer Vorlage |
| **Interpolation** | Zwischenwerte mischen, etwa zwischen zwei Lagebildern ([Kap. 4](04-grafik.md), [10](10-netzwerk.md)) |
| **Interpreter** | führt einen Logik-Graphen Kästchen für Kästchen aus, wie ein Dolmetscher |
| **JavaScript** | die Programmiersprache der Webseiten |
| **JIT** | übersetzt einen Logik-Graphen beim Start in Befehle, die der Rechner direkt ausführt |
| **Kontakt** | eine Stelle, an der sich zwei Körper berühren |
| **Matrix** | eine Tabelle aus 4 mal 4 Zahlen, die Strecken, Drehen und Verschieben bündelt ([Kap. 3](03-grundlagen.md)) |
| **Mesh** | eine 3D-Form, gebaut aus vielen kleinen Dreiecken |
| **Partikel** | viele winzige Teilchen für Funken, Staub oder Rauch |
| **Physikschritt** | ein Bild im Daumenkino der Physik; die Welt rückt ein Stückchen weiter |
| **Rechenkern** | eines der Rechenwerke im Prozessor; mehrere können gleichzeitig rechnen |
| **Schrittrate** | wie viele Physikschritte pro Sekunde gerechnet werden; für die Ente 100 |
| **Server / Client** | beim Mehrspieler: der Rechner, der bestimmt / die Rechner, die mitspielen |
| **Shader** | ein kleines Programm der Grafikkarte, das die Farbe jedes Bildpunkts ausrechnet |
| **Simulation** | die nachgebaute Welt im Computer, die nach den Regeln der Physik weiterläuft |
| **Snapshot** | ein „Lagebild“ des Servers beim Mehrspieler: wo gerade alles steht ([Kap. 10](10-netzwerk.md)) |
| **Span** | ein „Zettel“, der auf Daten zeigt (wo sie anfangen, wie viele), ohne sie zu besitzen ([Kap. 3](03-grundlagen.md)) |
| **Starrer Körper** | ein Körper, der sich nie verbiegt; so rechnet Box3D alle Körper |
| **Sub-Steps** | Unterschritte innerhalb eines Physikschritts, für mehr Genauigkeit |
| **Textur** | ein Bild, das auf eine Oberfläche geklebt wird |
| **Thread, Worker** | ein Arbeitsstrang; mehrere rechnen gleichzeitig auf mehreren Rechenkernen |
| **Vektor** | eine Wegbeschreibung aus Zahlen, etwa „3 m nach rechts, 1 m nach vorn“ ([Kap. 3](03-grundlagen.md)) |
| **WebAssembly** | ein Programmformat, das Browser fast so schnell ausführen wie echte Programme |
| **Welding** | „Verschweißen“: doppelte Ecken eines Meshs zu einer verschmelzen ([Kap. 5](05-szene-und-geometrie.md)) |

### Roboter und Messen

| Begriff | Bedeutung |
|---|---|
| **Abgleich** | eine Aufzeichnung des echten Roboters in der Simulation nachspielen und vergleichen ([Kap. 16](16-kalibrierung.md)) |
| **Armatur** | Zusatzträgheit, weil der flinke Motor im Servo jede Bewegung mitmachen muss |
| **Chirp** | ein Messsignal: ein Hin und Her, das immer schneller wird, wie ansteigendes Vogelzwitschern |
| **CSV** | eine Tabelle als Textdatei, Spalten durch Kommas getrennt |
| **Drehmoment** | Drehkraft: wie fest ein Motor an einem Gelenk drehen kann |
| **Gehirn (der Ente)** | in Kapitel 13 Pollens Entscheidungslogik: was die Ente tut, wenn niemand steuert |
| **Gelenksensor (Encoder)** | misst im Servo, welchen Winkel das Gelenk gerade hat |
| **Gerätemodell** | ein kleines Rechenmodell eines echten Geräts, etwa eines Servos |
| **Getriebespiel** | ein bisschen Wackeln im Getriebe, bevor die Zahnräder greifen |
| **Grundhaltung** | die Ausgangsstellung der Gelenke, wenn die Ente ruhig steht |
| **Hz (Hertz)** | „mal pro Sekunde“: 50 Hz heißt 50-mal pro Sekunde |
| **JSON** | ein Textformat aus Namen und Werten |
| **Kalibrierung** | die Zahlen eines Modells an Messungen anpassen |
| **Kamera-Detektor** | meldet, wo die Kamera der Ente Ball oder Socke sieht; Kern ahmt ihn nach |
| **Lagesensor (IMU)** | spürt, wo unten ist und wie schnell sich der Körper dreht |
| **Läufer** (Walker) | die Steuerart im Studio für Laufroboter wie die Ente, die ein Netz bewegt |
| **Log** | eine Aufzeichnung, Zeile für Zeile: was gesendet und was gemessen wurde |
| **Micro Duck** | der zweibeinige Laufroboter von Pollen Robotics ([Kap. 13](13-micro-duck.md)) |
| **ms (Millisekunde)** | eine tausendstel Sekunde |
| **Pollen Robotics** | die Firma, die die echte Micro Duck und ihre Netze gebaut hat; Kern ist nicht mit ihr verbunden |
| **Prüfstand** | ein einzelner Servo mit Arm und Gewicht, festgespannt zum Vermessen |
| **Prüf-Fehler** | der Fehler auf einer Messung, die beim Anpassen nicht benutzt wurde |
| **rad (Radiant)** | ein Winkelmaß der Technik: 1 rad sind etwa 57 Grad, ein ganzer Kreis etwa 6,28 rad |
| **Regeltakt, Takt** | der Herzschlag der Steuerung: 50-mal pro Sekunde bekommt jeder Servo ein neues Ziel |
| **RMS-Fehler** | der durchschnittliche Abstand zweier Kurven, große Ausreißer zählen stärker |
| **Servo** | ein Motor, der auf einen vorgegebenen Winkel fährt |
| **Servoprofil** | die wenigen Zahlen, die einen Servotyp beschreiben ([Kap. 16](16-kalibrierung.md)) |
| **Sim-to-Real-Lücke** | der Unterschied zwischen Simulation und echtem Roboter |
| **Streuung** | wie weit ein Wert schwanken darf oder von Exemplar zu Exemplar schwankt |
| **ToF** | ein Abstandssensor, der misst, wie lange Licht hin und zurück braucht |

Ein Bild zu **rad**: Leg eine Schnur, so lang wie der Radius eines Kreises, auf seinen Rand. Der
Winkel, den sie vom Mittelpunkt aus überspannt, ist 1 rad. Rund sechseinviertel solcher Schnüre
reichen einmal um den Kreis.

### Netze und Lernen

| Begriff | Bedeutung |
|---|---|
| **Actor (beim Lernen)** | das Netz, das handelt und später auf den Roboter kommt ([Anhang A](A-grundlagen-neuronale-netze.md)) |
| **Adam** | ein kluges Verfahren, die Regler in gleichmäßigen, ruhigen Schritten zu verstellen |
| **Agent** | der Handelnde beim Lernen durch Belohnung, hier die Ente mit ihrem Netz |
| **Aktion** | was das Netz in einem Takt tut: 14 Zielwinkel für die Servos |
| **Aktivierung (ELU)** | das „Verbiegen“ im Neuron: positive Zahlen bleiben, negative werden gestaucht |
| **asymmetrischer Kritiker** | der Kritiker sieht beim Üben mehr als der Actor, etwa die genaue Höhe |
| **Backpropagation** | den Fehler vom Ausgang rückwärts auf alle Regler verteilen |
| **Beobachtung** | was die Ente in einem Takt misst: 61 Zahlen über Lage, Gelenke, Befehle |
| **Belohnung** | Punkte pro Takt, die sagen, wie gut die Ente gerade war |
| **Bias** | die Grundeinstellung eines Neurons, die immer dazugezählt wird |
| **CMA-ES** | Suchverfahren: eine Wolke von Kandidaten probieren, zu den besseren hinrücken |
| **DAgger** | Nachahmen, bei dem das Netz selbst fährt und das Vorbild jede Lage kommentiert |
| **e-Funktion** | Schrumpfen um denselben Anteil pro Schritt; ELU staucht damit negative Zahlen |
| **Entropie** | ein Maß für die Neugier: wie breit die Ente um ihre beste Idee herum ausprobiert |
| **Epoche** | einmal alle gesammelten Übungsdaten durchgehen |
| **Episode** | ein Übungsdurchgang, vom Start bis zum Sturz oder bis die Zeit um ist |
| **GAE** | schätzt den Vorteil eines Schritts: eigene Überraschung plus ein Teil seiner Folgen |
| **Gewicht** | ein Regler im Netz: wie stark ein Eingang zählt |
| **Gradient** | die Steigung der Verlust-Landschaft: in welche Richtung es bergauf geht |
| **Gradientenabstieg** | lernen, indem man in der Verlust-Landschaft Schritt für Schritt bergab geht |
| **Iteration** | eine Trainingsrunde: alle Enten üben ein Stück, dann wird das Netz einmal verbessert |
| **Kettenregel** | die Rechenregel, mit der Backpropagation den Fehler Schicht für Schicht zurückreicht |
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
| **Normalverteilung (Glockenkurve)** | kleine Abweichungen sind häufig, große selten |
| **ONNX** | ein Austauschformat für Netze, das viele Programme lesen können |
| **Policy** | die Entscheidungsregel der Ente, also ihr trainiertes Netz |
| **PPO** | das Lernverfahren des Lehrers: aus Belohnung lernen, in kleinen, vorsichtigen Schritten |
| **Rauschen, σ (Sigma)** | zufällige Abweichung; σ sagt, wie breit sie ist |
| **Return** | die Gesamtpunktzahl ab jetzt, spätere Punkte etwas weniger wert |
| **Rezept** | alle Trainingseinstellungen einer Aufgabe zusammen |
| **Sammler / Lerner** | die Teile des Trainings, die Erfahrung sammeln / daraus lernen |
| **Schicht** | viele Neuronen nebeneinander, die dieselben Eingänge sehen |
| **Seed** | die Startzahl für den Zufall: gleicher Seed, gleicher Zufall |
| **Spielschicht** | Kerns Netz, das entscheidet, was die Ente mit Ball und Socke tut |
| **Überanpassung** | Auswendiglernen statt Verstehen: gut bei Übungen, schlecht bei Neuem |
| **überwachtes Lernen** | Lernen mit Lösungsheft: zu jeder Aufgabe gibt es die richtige Antwort |
| **Variation** | jede Episode würfelt etwas andere Bedingungen aus; Fachwort Domänen-Randomisierung |
| **velstand** | Pollens Netz zum Laufen und Stehen, in Kern fest eingebaut |
| **Verlust** | eine Zahl, die sagt, wie falsch das Netz gerade liegt; kleiner ist besser |
| **Verteilungsverschiebung** | ein nachahmendes Netz gerät beim Selbermachen in Lagen, die es nie gesehen hat |
| **Vorbild** | eine Vorführung, die ein Netz zuerst nachahmt |
| **Vorteil** | wie viel besser eine Aktion war als das, was die Ente hier sonst tut |
| **Warmstart** | ein neues Training beginnt mit einem schon trainierten Netz statt mit Zufall |
| **Wert** | wie viele Punkte die Ente von einer Lage aus noch erwarten kann |

Die genauen Definitionen stehen im Glossar des blauen Buchs, Kapitel 17.

## Das nimmst du mit

- **F1** im Studio zeigt die wichtigsten Kürzel; Alt+P spielt, Alt+S simuliert, Esc stoppt.
- **Spielen** (Play) heißt selbst spielen, **Simulieren** (Simulate) heißt zuschauen und Körper
  greifen.
- Die Ente steuerst du mit W A S D und den Tasten 1 bis 7.
- Levels sind Textdateien, Netze `.onnx` im selben Format wie bei Pollen Robotics.
- Der ganze Editor steckt in der Engine; die Webseite zeichnet nur, was die Engine ihr sagt.
- „Actor“ heißt im Studio ein Ding im Level, beim Lernen das handelnde Netz.
- Für jede Funktion, jede Option und jedes Dateiformat schlägst du im blauen Buch nach.

## Knobelecke

1. Du hast eine Kiste ausgewählt, aber sie liegt außerhalb des Bildes. Wie holst du sie ins Bild?
2. Du liest „Der Actor lernt aus den Vorteilen, die der Kritiker schätzt.“ Ist hier eine Kiste im
   Level gemeint?
3. Du drückst bei der Ente die 3, aber nichts passiert. Woran könnte es liegen?
4. Was ist der Unterschied zwischen **Spielen** (Play) und **Simulieren** (Simulate)?

### Lösungen

1. **F** holt die Auswahl ins Bild, ein Doppelklick auf ihren Namen im Outliner auch. (Ein
   Doppelklick ins Leere hilft nicht, der hebt die Auswahl auf. Und hast du die Kiste mit **H**
   ausgeblendet, holt **Strg+H** sie zurück.)
2. Nein. Neben dem Kritiker ist der Actor das handelnde Netz beim Lernen, kein Ding im Level.
3. Vielleicht sitzt die Ente gerade: Aus dem Sitzen startet keine Bewegung. Erst mit der 1
   aufstehen lassen. Oder es läuft noch eine andere Bewegung; dann wartest du, bis sie fertig ist.
   (Und F8 nicht vergessen, damit du die Ente überhaupt steuerst.)
4. Beim Spielen spielst du selbst, die Kamera folgt der Spielfigur (in manchen Levels schaust du
   erst zu und übernimmst mit F8). Beim Simulieren laufen Physik und Regeln ohne Spieler; du behältst die
   freie Kamera und kannst Körper greifen und mit K ihre neuen Lagen übernehmen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
