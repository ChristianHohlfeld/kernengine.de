# 15 Eigene Modelle trainieren – die Werkstatt

> **Kurz gesagt:** Hier trainierst du selbst. Auf der Lehrer-Seite im Browser bringst du der Ente
> den Sprung bei, liest den Fortschritt an Kurven ab, sicherst ein gutes Netz und siehst es im Studio
> springen. Dazu kommen die Regeln, nach denen man gute Übungen baut, und die Fallen, in die schon die
> echten Trainingsläufe getappt sind.

> **Begriffe vorab:**
> - **Lehrer-Seite** – die Webseite kernengine.de/lehrer/, auf der du im Browser trainierst.
> - **Kommandozeile** – ein Fenster, in das man Befehle tippt, statt zu klicken. Für dieses Kapitel
>   brauchst du sie nicht.
> - **Rückgabe** – alle Punkte einer Episode zusammengezählt.
> - **Rauschen** – der Zufall, der zu jedem Befehl des Netzes dazukommt: die Neugier der Ente.
> - **Lernrate** – wie weit der Trainer die Regler pro Schritt verstellt.
> - **Schnappschuss** – ein unterwegs gespeicherter Zwischenstand des Netzes.
> - **Messung** – eine strenge Prüfung, ob eine Bewegung wirklich gelungen ist, unabhängig von den
>   Punkten.

Wie der Lehrer innen arbeitet, steht in [Kapitel 14](14-lehrer.md). Die Begriffe von dort (Episode,
Übungsrunde, Kritiker, Variation, Vorbild, Nachahmen) brauchst du hier wieder.

## 15.1 Zwei Wege: Browser oder Kommandozeile

Es gibt zwei Wege zu trainieren, und beide ergeben bei gleichen Einstellungen **dasselbe Netz**:

- **Die Lehrer-Seite im Browser:** nichts installieren, alles mit Mausklicks. Das ist der Weg dieses
  Kapitels.
- **Das Werkzeug auf dem eigenen Rechner:** schneller und vielseitiger, aber man muss es selbst
  bauen und über die Kommandozeile bedienen.

**Was du ohne Kommandozeile überspringen kannst:** alle Kästen „Für Neugierige“ und Abschnitt 15.7
(Spielschicht). Nichts im übrigen Text setzt sie voraus. Die Prinzipien darin – woran man Fortschritt
erkennt, wie man das beste Netz findet, was gute Übungen ausmacht – gelten für beide Wege.

> **Für Neugierige:** Wie man das Werkzeug baut, welche Befehle es kennt und welche Einstellungen
> es gibt, steht im blauen Buch, Kapitel 15.1 und 15.2. Die Prozessor-Einstellungen beim Bauen
> machen es nur schneller; das Netz bleibt bis aufs Haar dasselbe wie im Browser.

## 15.2 Den Sprung im Browser trainieren

Ein Sprung-Lauf hat zwei Phasen, die du auf der Seite mitverfolgen kannst:

1. **Nachahmen:** 8 Runden lang lernt das Netz vom eingebauten Vorbild. Die Statuszeile zeigt
   „Nachahmen: Runde … von 8“. Danach kann das Netz meist schon springen, und die Seite speichert es
   zum ersten Mal für das Studio.
2. **Üben:** Jetzt lernt die Ente nach Punkten, mit Variation. Die ersten 30 Runden lernt nur der
   Kritiker (Kapitel 14.5), dann beide. Alle 25 Runden macht die Seite 5 Probesprünge und speichert
   das Netz.

**Wie lange dauert das?** Auf einem Testrechner mit 2 Rechenkernen kam im Browser der erste echte
Sprung nach knapp 9 Minuten, das Nachahmen war nach etwa 11 Minuten fertig. Das Üben dauert viel
länger, denn es braucht Tausende Runden. Wie schnell dein Gerät ist, zeigt die Seite unter
„Schritte / s“: so viele Entscheidungstakte schaffen alle Enten zusammen pro Sekunde.

> **Probier's aus!** Dein erster Sprung-Lauf:
> 1. Öffne **kernengine.de/lehrer/**. Warte, bis die Statuszeile „Bereit“ meldet.
> 2. Wähle bei **Bewegung** „Sprung“. Alle anderen Felder bleiben, wie sie sind.
> 3. Drück **Start** und lass den Tab offen. Ein Handy gehört ans Ladekabel.
> 4. Beobachte die Statuszeile: erst „Nachahmen: Runde 1 von 8“ und so weiter, dann „Lernt:
>    Iteration …“. Unter **echte Sprünge** siehst du, wie viele von 5 Probesprüngen gelingen.
> 5. Sobald unter den Knöpfen „Für Kern Studio gespeichert als jump.onnx“ steht, ist dein Netz
>    bereit für 15.6.
>
> Was du lernst: Erst das Vorbild macht den Sprung möglich, das Üben macht ihn danach sicher.

> **Wusstest du?** Gleiche Startzahl, gleich viele Enten und dasselbe Rezept ergeben im Browser
> dasselbe Netz wie auf dem Rechner. Die Zahl der Enten ist **Worker × Enten je Worker**; ein Worker
> ist ein Helfer im Hintergrund des Browsers, der seine Enten turnen lässt. Das eingebaute
> Sprung-Netz entstand mit 32 Enten, Startzahl 1 und 3000 Übungsrunden. Mit 4 Workern zu je 8 Enten
> (zusammen 32), Startzahl 1 (Feld „Seed“) und 3000 Iterationen stellst du also dieselben
> Einstellungen ein – wenn du die Zeit dafür hast.

## 15.3 Fortschritt lesen

Den Fortschritt liest man an **Kurven** ab, nicht an einer einzelnen Zahl. Von Runde zu Runde
schwanken die Werte, weil jede Runde andere Enten mit anderem Zufall turnen. Wichtig ist der Verlauf
über Hunderte Runden.

> **Stell dir vor …** die Fieberkurve im Krankenhaus: Eine einzelne Messung sagt wenig, der Verlauf
> über Tage sagt alles.

Die Lehrer-Seite zeichnet Rückgabe und Episodenlänge als Kurven und zeigt dazu Zahlen:

| Anzeige | Bedeutung | Worauf achten |
|---|---|---|
| Episoden-Rückgabe | Punkte einer Episode, im Mittel | soll über viele Runden steigen |
| Episodenlänge (s) | wie lange eine Episode dauert | Sprung: 2 s heißt ohne Sturz; kürzer heißt Sturz oder Abstützen |
| Aktionsrauschen | wie viel die Ente herumprobiert | sinkt, wenn sie sicherer wird (14.5) |
| echte Sprünge | von 5 Probesprüngen | **das eigentliche Ziel** |
| Schritte / s | Tempo des Geräts | sagt nichts über die Qualität |

Steigen die Punkte, aber die echten Sprünge nicht, ist das ein Warnzeichen: Vielleicht holt die
Ente ihre Punkte mit etwas, das gar kein Sprung ist (15.9).

> **Für Neugierige:** Das Werkzeug schreibt jede Runde als Textzeile und auf Wunsch in eine Tabelle,
> die man in eine Tabellenkalkulation laden kann. Mit der Kommandozeile kann man sich außerdem eine
> einzelne Episode Takt für Takt ansehen, mit jedem Punkte-Posten – wie eine Röntgenaufnahme, die
> zeigt, wofür die Ente ihre Punkte wirklich bekommt. Zeilenformat und Befehle: blaues Buch,
> Kapitel 15.4.

## 15.4 Das beste Netz auswählen

**Das zuletzt gespeicherte Netz ist nicht automatisch das beste.** Beim Üben kann ein Netz nach
einem guten Stand wieder schlechter werden. Und die Probe während des Laufs ist klein: Nur 5 Sprünge
auf dem sauberen Roboter – da kann ein Netz zufällig gut aussehen.

> **Stell dir vor …** du fotografierst eine Serie. Du nimmst nicht einfach das letzte Foto, sondern
> legst alle nebeneinander und suchst das beste aus.

So geht man vor:

1. Unterwegs **Schnappschüsse** aufheben.
2. Jeden mit vielen Sprüngen gründlich messen, auf dem sauberen Roboter **und** mit voller Variation.
3. Wählen nach: zuerst echte Sprünge mit Variation, dann keine Stürze, dann Höhe, Flugzeit und
   Schieflage.
4. Den Sieger im Studio prüfen (15.6).

Für das eingebaute Netz ergab diese Messung 40 von 40 echte Sprünge sauber, 36 von 40 mit
Variation und keinen einzigen Sturz.

Im Browser behält die Seite nur den **neuesten** Stand. Zeigt die Anzeige gerade 5 von 5 echte
Sprünge, kannst du diesen Stand mit **.onnx herunterladen** als Datei sichern, bevor das Üben
weitergeht.

> **Für Neugierige:** Das Werkzeug kann alle paar Hundert Runden einen Schnappschuss behalten und
> jeden mit 40 Episoden sauber und variiert messen. Befehle: blaues Buch, Kapitel 15.5.

## 15.5 Sanft weiterlernen

Nach dem Nachahmen kann das Netz schon springen. Jetzt gilt: **behutsam weiterlernen.** Kleine
Lernrate, wenig Rauschen und Kritiker zuerst – sonst geht das Gelernte verloren.

> **Stell dir vor …** du hast einem Hund gerade mühsam einen Trick beigebracht. Brüllst du jetzt
> neue Kommandos und wedelst wild mit Leckerlis, ist der Trick schnell wieder weg.

Darum stehen die Felder „Aktionsrauschen am Anfang“ und „Lernrate“ auf der Lehrer-Seite leer: Dann
gilt das erprobte Rezept. Was die echten Läufe außerdem gezeigt haben:

- **Mehr Herumprobieren hilft nicht.** Mit viel Rauschen und ohne Warmstart lernte die Ente nur zu
  stehen, nicht zu springen.
- **Wenige Enten bremsen.** Wenige Enten sammeln pro Runde wenig Erfahrung, die Lernrichtung wird
  unsicher.
- **Die Zahl der Enten bestimmt das Ergebnis, die Zahl der Rechenkerne nur das Tempo.** Auf der Seite
  heißt das: Die Gesamtzahl Worker × Enten je Worker entscheidet, welches Netz herauskommt. Mehr
  Worker bei gleicher Gesamtzahl ändern das Netz nicht, sie verteilen die Arbeit nur auf mehr
  Rechenkerne. Erhöhst du aber nur die Worker und lässt
  „Enten je Worker“ stehen, hast du mehr Enten – und ein anderes Netz.
- **Die Physik nicht vergröbern.** Mit gröber gerechneter Physik ginge das Training 1,6-mal
  schneller, aber das Netz lernte eine andere Welt als die des Studios.

> **Für Neugierige:** Das Werkzeug hat Schalter für alle Einstellungen des Rezepts und kann einen
> Lauf fortsetzen. Achtung: Beim Fortsetzen gibt man die Zahl der Runden als **Ziel** an. Wer nach
> 3000 Runden 1000 weitere will, schreibt 4000. Alles dazu: blaues Buch, Kapitel 15.3 und 15.6.

## 15.6 Im Studio benutzen

Ein Netz von der Lehrer-Seite landet von selbst im Studio, wenn beide im **selben Browser auf
demselben Gerät** laufen. Das Studio lädt es beim Start und übernimmt jedes neue sofort, auch in
schon offenen Tabs. Das gilt für den Sprung.

> **Probier's aus!** Deine Ente springt mit deinem Netz:
> 1. Starte auf der Lehrer-Seite einen Sprung-Lauf (15.2).
> 2. Öffne im selben Browser einen zweiten Tab mit https://kernengine.de/ und dort das Beispiel-Level
>    (File › Example Levels › Micro Duck).
> 3. Warte, bis die Lehrer-Seite „Für Kern Studio gespeichert als jump.onnx“ meldet. Das passiert
>    zum ersten Mal nach dem Nachahmen, dann alle 25 Runden.
> 4. Starte im Studio **Play**, übernimm die Ente mit **F8** und drück **7**.
>
> Was du siehst: Jetzt springt **dein** Netz, nicht mehr das mitgelieferte – es ersetzt es. Ein
> früher Stand springt oft noch unsicher. Drück nach einer Weile Übung noch einmal **7** und
> vergleiche.

> **Für Neugierige:** Man kann ein Netz auch als Datei in den Quellcode legen, und die
> Entwicklerkonsole des Browsers meldet, wenn ein Netz geladen wurde. Welche Form das Netz haben muss
> und wo die Dateien liegen: blaues Buch, Kapitel 15.8.

## 15.7 Die Spielschicht trainieren

Die Spielschicht lässt sich nur mit dem Werkzeug auf der Kommandozeile trainieren, nicht im Browser.
Wer das nicht vorhat, kann diesen Abschnitt überspringen. Der Ablauf zeigt aber zwei schöne
Prinzipien:

- **Erst messen, was das Vorbild braucht.** Wo der Schnabel eine Socke greift und wie weit die Ente
  nach „Stopp“ noch läuft, misst der Lehrer selbst in der aktuellen Physik (Kapitel 14.8). Die
  Nachlaufstrecke braucht das Vorbild, weil Pollens Gang nicht auf der Stelle stoppt, sondern noch
  ein Stück weitergeht.
- **Das Vorbild messen.** Bevor man das Netz prüft, prüft man das Vorbild. Das zeigt, was das Netz
  durch reines Nachahmen **höchstens** erreichen kann. Schafft schon das Vorbild etwas nicht, liegt es
  nicht am Netz.

Danach folgen Nachahmen, Prüfen, Ansehen, Üben und ein Probelauf im Beispiel-Level.

> **Achtung, Falle!** Beim Nachahmen wird die Ausgabedatei nach **jeder** Runde überschrieben. Wer
> eine frühere Runde behalten will, muss sie während des Laufs kopieren. Beim heutigen Spielnetz war
> Runde 8 von 12 die beste (Kapitel 14.8), nicht die letzte – genau die Lehre aus 15.4.

> **Für Neugierige:** Alle sechs Schritte mit Befehlen stehen im blauen Buch, Kapitel 15.7.

## 15.8 Eine neue Bewegung hinzufügen

Angenommen, die Ente soll sich **verbeugen**. Dafür braucht man fünf Zutaten:

1. **Eine Aufgabe:** Wie lange dauert die Verbeugung (etwa 2 Sekunden)? Wie startet die Ente? Wofür
   gibt es Punkte (aufrecht bleiben, ruhig bewegen und natürlich die Verbeugung selbst), was beendet
   die Episode (Sturz)? Und was darf der Kritiker zusätzlich wissen?
2. **Ein Rezept:** die Einstellungen fürs Training, etwa Warmstart von velstand, wenig Rauschen,
   kleine Lernrate. Damit kennen Werkzeug und Browser die neue Übung; auf der Lehrer-Seite fehlt dann
   nur noch ein Eintrag in der Auswahl.
3. **Eine Messung:** eine strenge Prüfung, ob wirklich eine Verbeugung stattfand (15.9).
4. **Die Verbindung zum Studio:** eine Taste, ein Knopf für Touch, der Name im Logik-Knoten
   **Robot Move** und die Netz-Datei. Im Studio wechselt die Ente dann für die Dauer der Bewegung auf
   das neue Netz und danach zurück.
5. **Ein Vorbild (oft nötig):** Bei schweren Bewegungen findet Üben allein den Weg nicht (Kapitel
   14.6). Dann braucht man einen einfachen Bewegungsplan, eine Note für die Suche und ein Vorbild,
   das in **jeder** Lage eine Antwort weiß – sonst funktioniert das Nachahmen mit Korrektur nicht.

Für jede dieser Zutaten muss man programmieren. Die Lernmaschine selbst bleibt dabei dieselbe: Üben
und Nachahmen kennen nur „eine Welt, in der eine Ente handelt und Punkte bekommt“. Auch die
Spielschicht mit ganz anderen Ein- und Ausgängen lernt mit genau dieser Maschine.

> **Für Neugierige:** Das blaue Buch zeigt das Gerüst einer Verbeugungs-Aufgabe in C++ und listet
> alle Stellen im Studio, die man ergänzen muss (Kapitel 15.9).

## 15.9 Messung ist nicht Belohnung

**Die Messung prüft, was eine Bewegung ist – nicht, wie viele Punkte sie gebracht hat.** Das ist
vielleicht die wichtigste Regel der ganzen Werkstatt.

Punkte sind ein Wegweiser zum Lernen. Netze finden aber jede Lücke darin. Würde man den Erfolg an
den Punkten messen, hielte man ein Netz für gut, das nur geschickt Punkte sammelt. Darum hat der
Sprung eine eigene, strenge Prüfung: alle Teile mindestens 60 Millisekunden in der Luft, der Rumpf
steigt, höchstens 30° Schieflage, aufrecht gelandet (Kapitel 14.6). Ob die Ente dabei viele oder
wenige Punkte bekam, ist der Prüfung egal.

> **Stell dir vor …** eine Fahrprüfung. Die Fahrstunden kannst du sammeln, wie du willst. In der
> Prüfung zählt nur, ob du wirklich sicher fährst.

## 15.10 Gute Übungen entwerfen

Wer eine neue Übung baut, entwirft drei Dinge: was das Netz **sieht** (die Beobachtung), was es
**tun** kann (die Aktionen) und wofür es **Punkte** gibt (die Belohnung). Kerns Netze leben dafür
sieben Regeln vor:

- **Nur, was der Roboter wirklich hat.** Das handelnde Netz sieht Sensorwerte, nie wahre Positionen.
  Ball und Socke kennt die Spielschicht nur aus der Kamera, verrauscht und mit Aussetzern. Wahres
  Wissen bekommt nur der Kritiker.
- **Zeit sichtbar machen.** Ein Netz hat kein eigenes Gedächtnis: Es sieht nur den jetzigen
  Augenblick und das, was man ihm mitgibt (etwa seinen eigenen letzten Befehl, 13.7). Darum muss man
  ihm sagen, wie weit eine Bewegung schon ist (die Uhr des Sprungs) oder wie lange es
  schon steht, geht oder trägt.
- **Messen statt befehlen.** „Steht still“ heißt: beide Sohlen am Boden, keine Bewegung läuft. Es
  zählt, was die Beine tun, nicht was befohlen wurde.
- **Aktionen wie Bedienelemente.** Laufbefehle wirken wie ein Steuerknüppel mit Ruhezone: Kleine
  Ausschläge bedeuten „stehen“. Eine Bewegung wie der Kick startet erst, wenn ihr Wert eine Schwelle
  überschreitet.
- **Punkte-Posten mit Namen.** Jeder Posten ist einzeln sichtbar, kleine Abzüge bremsen Zappeln und
  Wackeln, und Lehrpläne machen Strafen und Variation nach und nach strenger.
- **Hartes Episodenende.** Sturz und Bodenkontakt anderer Körperteile beenden die Episode; sonst wird
  Abstützen zur sicheren Wette.
- **Vorbild ohne Geheimwissen.** Das Vorbild sieht nur, was das Netz sieht. Das gilt auch für sein
  Timing: Das Vorbild sagt nicht von einem Augenblick auf den anderen „Jetzt kicken!“, sondern wird
  mit jedem Moment Stillstehen ungeduldiger, wie bei einem Countdown. So kann das Netz an der
  Stehzeit ablesen, wann es so weit ist. Ein plötzliches „Jetzt!“ ohne erkennbaren Grund könnte es
  nicht nachmachen.

> **Probier's aus!** Öffne auf https://kernengine.de/ das Micro-Duck-Level und starte **Play**. Es
> beginnt mit der Observer camera: Du schaust zu und kannst Dinge mit der Maus oder dem Finger
> anheben. Heb den Ball an eine andere Stelle und lass ihn los. Beobachte, wie die Spielschicht nur
> aus ihren Sensoren neu entscheidet: hinschauen, hingehen, stehen, kicken. Was du lernst: Die Ente
> „weiß“ nicht, wo der Ball ist – sie muss ihn sehen.

## 15.11 Fallstricke aus den echten Läufen

Damit du nicht hineintappst – das ist in den echten Läufen passiert:

- **Die Belohnung wird ausgenutzt.** Frühe Enten saßen auf dem Schwanz, machten Hechtsprünge oder
  stützten sich ab. Jedes Mal half nur eine strengere Regel: Bodenkontakt beendet die Episode,
  Schieflage kostet, Punkte gibt es nur aufrecht.
- **Die Messung war zu locker.** Die erste Prüfung ließ eine Ente durch, die sich nur aufrichtete und
  die Füße anzog, dabei 45° kippte. Seitdem zählt nur ein echter Sprung.
- **Üben allein findet seltene Bewegungen nicht.** Erst Vorbild und Nachahmen brachten den Sprung.
- **Ein guter Schüler, ein ahnungsloser Kritiker.** Ohne „Kritiker zuerst“ ging das Nachgeahmte
  kaputt (15.5).
- **Fehler in der Welt statt im Netz.** Beim Training der Spielschicht kamen drei ans Licht: Die
  Socke kroch von selbst über den Boden, der Ball rollte ewig, und das Gähnen im Leerlauf öffnete
  beim Aufheben den Schnabel. Lernt ein Netz an einer Stelle einfach nicht, lohnt ein Blick auf die
  Welt.
- **Zahlen von Hand veralten.** Wo der Schnabel greift, stand früher als feste Zahl in der
  Dokumentation. Jetzt misst der Lehrer es bei jedem Lauf neu.
- **Gleichheit aus Versehen brechen.** Schon eine einzige Rechenfunktion, die anders rundet, oder
  eine Summe in wechselnder Reihenfolge – und das Netz ist auf einem anderen Gerät ein anderes
  (Kapitel 14.7).
- **Andere Physik als das Studio.** Gröbere Physik, ein anderer Boden, andere Motoren: Dann lernt
  das Netz eine andere Welt als die, in der es später laufen soll.

## Das nimmst du mit

- Die Lehrer-Seite im Browser ist der einfachste Weg; das Werkzeug auf dem Rechner ist schneller,
  ergibt aber dasselbe Netz.
- Fortschritt liest man an Kurven, nicht an einer Zahl. Das eigentliche Ziel sind echte Sprünge.
- Das letzte Netz ist nicht immer das beste: Zwischenstände sichern und gründlich prüfen, sauber
  und mit Variation.
- Nach dem Nachahmen sanft weiterlernen: kleine Lernrate, wenig Rauschen, Kritiker zuerst.
- Die Zahl der Enten bestimmt das Ergebnis, die Zahl der Rechenkerne nur das Tempo.
- Eine neue Bewegung braucht Aufgabe, Rezept, Messung, Studio-Anschluss und oft ein Vorbild.
- Die Messung prüft, was eine Bewegung ist – nicht, wie viele Punkte sie brachte.

## Knobelecke

1. Du trainierst im Browser mit 4 Workern zu je 8 Enten. Eine Freundin nimmt auf ihrem Rechner
   8 Worker zu je 8 Enten, mit derselben Startzahl. Bekommt sie dasselbe Netz? Was müsste sie
   einstellen?
2. Warum prüft man die Kandidaten zum Schluss mit 40 Sprüngen, statt sich auf die 5 Probesprünge
   während des Laufs zu verlassen?
3. Warum soll man direkt nach dem Nachahmen nicht mit großer Lernrate und viel Rauschen üben?
4. Warum soll die Messung einer neuen Bewegung nicht einfach die Punkte zusammenzählen?
5. Ein Netz lernt eine Sache einfach nicht, so lange du auch übst. Wo könnte der Fehler außer im
   Netz noch stecken?

### Lösungen

1. Nein: Sie hat 64 Enten statt 32, also ein anderes Netz. Mit 8 Workern zu je 4 Enten (zusammen
   32) bekäme sie dasselbe Netz, nur schneller.
2. Fünf Sprünge sind ein sehr kleiner Test; ein Netz kann zufällig gut aussehen. Und erst mit
   Variation zeigt sich, ob es robust ist.
3. Das Netz kann den Sprung schon. Große Schritte und wildes Herumprobieren würden das Gelernte
   zerstören, bevor der Kritiker überhaupt gut schätzt.
4. Weil Netze Lücken in der Belohnung ausnutzen. Die Messung muss prüfen, ob die Bewegung wirklich
   passiert ist.
5. In der simulierten Welt: Bei der Spielschicht kroch etwa die Socke von selbst davon.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
