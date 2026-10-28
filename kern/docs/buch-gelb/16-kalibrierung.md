# 16 Kalibrierung: vom Bildschirm auf den Tisch

> **Dein Level:** Ein Netz, das im Computer läuft, soll auch auf der echten Micro Duck laufen. Dafür
> muss die Simulation den echten Roboter genau treffen. Hier lernst du, wie man einen Servo vermisst
> und die Simulation an die Wirklichkeit anpasst.

> **Wörter, die du brauchst:**
> - **Servo** – ein kleiner Motor mit Getriebe, der ein Gelenk auf einen bestimmten Winkel dreht
> - **Simulation** – der Roboter als Nachbau im Computer, berechnet nach den Regeln der Physik
> - **Netz** – das kleine Rechenprogramm, das der Ente sagt, wie sie ihre Gelenke bewegen soll
> - **Training** – viele Übungsrunden in der Simulation, in denen das Netz besser wird
> - **Log** – eine Textdatei, in der Zeile für Zeile Messwerte stehen
> - **Sensor** – ein Bauteil, das etwas misst, etwa einen Winkel oder eine Drehung
> - **deterministisch** – rechnet bei gleichen Eingaben immer exakt dasselbe aus

> **Achtung, Sicherheit – zuerst lesen!**
> Ab hier geht es um einen **echten Roboter**. Arbeite damit **nur zusammen mit einem Erwachsenen**.
>
> - **Hände weg von laufenden Servos.** Unter Last schlagen sie schnell und kräftig aus.
> - Prüfstand fest einspannen, Arm und Gewicht sichern, Hände und Kabel aus dem Schwenkbereich.
> - Strom begrenzen (zum Beispiel mit einem Labornetzteil, an dem man eine Stromgrenze einstellt)
>   und einen **Not-Aus** griffbereit halten: einen Schalter, der sofort allen Strom abschaltet.
> - Nie mehr Spannung als vom Servohersteller erlaubt.
> - Lithium-Akkus nur mit passendem Ladegerät und unter Aufsicht laden.
> - Neue Netze zuerst testen, während der Roboter gehalten wird oder hängt.
> - Alles auf eigenes Risiko ([Impressum](00-impressum.md)).

Kalibrieren heißt: die Simulation so lange nachstellen, bis sie sich verhält wie das echte Gerät. Das
Netz soll sich außerdem auf nichts verlassen, was die Simulation nicht genau weiß. Kern geht dafür
drei Wege, und ein vierter Grundsatz kommt dazu:

1. **Ein Modell für jedes Gerät:** Ein Servo wird durch ein paar Zahlen beschrieben. Diese Zahlen
   misst man am echten Servo und prüft sie danach mit einer zweiten, neuen Messung.
2. **Den ganzen Roboter vergleichen:** Der echte Roboter schreibt auf, was er tut. Die Simulation
   macht dasselbe nach, und man vergleicht.
3. **Variation:** Was man nicht genau messen kann, verändert das Training ständig ein bisschen.
4. **In der Physik messen statt eintragen.**

> **So kannst du es dir vorstellen:** Links steht der echte Roboter, rechts die Simulation. Dazwischen
> läuft immer derselbe Kreis: **messen → anpassen → prüfen**. Jede Runde rückt die beiden näher
> zusammen.

## 16.1 Das Servomodell

Damit die Simulation einen Servo nachmachen kann, beschreibt Kern ihn als Bild aus wenigen Teilen.
Jedes Teil hat eine Zahl, und die sechs Zahlen zusammen heißen **Servoprofil**.

> **So kannst du es dir vorstellen:** Zwischen dem Arm und seinem Ziel hängt eine **Feder**. Je weiter
> der Arm vom Ziel weg ist, desto stärker zieht sie ihn zurück. Ein **Motor** liefert diese Kraft,
> aber er hat eine **Kraftgrenze**: Mehr geht nicht, egal wie weit weg das Ziel ist. Ein
> **Stoßdämpfer** wie am Fahrrad bremst schnelle Bewegungen, damit der Arm nicht hin- und herschwingt.
> Dazu kommt etwas **Reibung**, die jede Bewegung ein wenig festhält.

Zwei Dinge fehlen noch im Bild:

- **Schwung des Motors:** Der kleine Motor im Servo dreht sich durch das Getriebe sehr schnell. Darum
  ist er schwer in Schwung zu bringen und schwer zu stoppen, als hinge am Arm zusätzliches Gewicht.
  Fachleute nennen das **Armatur**.
- **Verzögerung:** Ein Befehl braucht etwa 20 Tausendstelsekunden (20 ms), bis er beim Servo ankommt,
  wie eine Nachricht, die kurz unterwegs ist.

**Beispiel:** Steht der Arm doppelt so weit vom Ziel weg, zieht die Feder doppelt so stark. Bei den
Servos der Micro Duck erreicht sie die Kraftgrenze aber erst, wenn der Arm fast 100° danebenliegt.
Die Servos sind also eher **weich**, wie ein lockeres Gummiband ([Kapitel 13](13-micro-duck.md)).

Ein anderer Servo ist einfach ein anderes Profil, also andere Zahlen. Nur ein Gerät, das ganz anders
arbeitet (etwa ein Motor, der schiebt statt dreht), bekommt ein eigenes Modell.

### Wie sicher ist eine Zahl?

Zu jedem Profil gehört auch, wie **sicher** man sich bei jeder Zahl ist. Das heißt **Streuung**. Beim
Servo der Micro Duck streuen Schwung und Reibung um ±10 %, die Verzögerung liegt irgendwo zwischen 15
und 30 ms. Das Training würfelt daraus für jede Übungsrunde und jeden Servo eigene Werte.

**Das ist das Prinzip: Ein gut gemessener Servo bekommt kleine Streuungen, ein schlecht bekannter
große.** So verlässt sich das Netz nie auf eine Zahl, die in Wirklichkeit niemand genau kennt.

> **Für Profis:** Ein Profil ist eine kleine Textdatei. Kern prüft beim Laden jeden Wert, ob er eine
> Zahl im erlaubten Bereich ist. Die genauen Namen und Werte stehen im blauen Buch, Kapitel 16.

## 16.2 Der Prüfstand

Um die Zahlen eines Servos herauszufinden, baut man einen **Prüfstand**: Der Servo bewegt einen Arm
mit bekanntem Gewicht, und man schreibt mit, was passiert. Dann stellt man denselben Aufbau im Computer
nach und dreht an den Zahlen, bis beide gleich aussehen.

### So misst man

1. **Aufbau:** Servo fest einspannen, Achse waagerecht, ein Arm mit Gewicht. Die Masse auf 0,1 g genau
   wiegen und messen, wie weit der Schwerpunkt von der Achse weg ist (Arm auf einer Kante ausbalancieren).
   Bei waagerechter Achse zieht die Schwerkraft am Arm wie an einem Pendel.
2. **Wie im Roboter betreiben:** gleiche Einstellungen, gleiche Spannung.
3. **Bewegungen vorgeben** und mindestens 100-mal pro Sekunde messen, wo der Arm gerade steht.
4. **Zwei oder drei verschiedene Gewichte** nehmen, etwa 50 g und 120 g. Mindestens eine Messung je
   Servo legst du zur Seite: Das ist die **Prüfmessung**.
5. **Mehrere Servos** messen. Wie stark sie sich unterscheiden, wird zur Streuung im Profil.

Die Bewegungen sind drei Arten von Befehlen. So sehen sie über der Zeit aus:

```
Sprung:     ____
                |______         plötzlich ein neues Ziel

Chirp:      /\/\/\/\/\/\/\/     Hin und Her, immer schneller

Rechteck:   __    __    __
              |__|  |__|        immer hin und zurück
```

Der „Chirp“ (englisch für Zwitschern) beginnt langsam mit einer halben Schwingung pro Sekunde und
endet mit vier. Die Einheit dafür heißt **Hertz (Hz)**: Schwingungen pro Sekunde.

Alles landet in einem **Log**. Jede Zeile enthält die Zeit, den gesendeten Zielwinkel und den
gemessenen Winkel.

### Der Prüfstand im Computer

Kern baut denselben Aufbau in der Simulation nach: Servo, Arm, Gewicht, Schwerkraft. Dann schickt es
dieselben Befehle aus dem Log, jeweils um die Verzögerung später, und vergleicht Zeitpunkt für
Zeitpunkt den simulierten mit dem echten Winkel.

> **So kannst du es dir vorstellen:** Zeichne die echte und die simulierte Kurve übereinander auf
> Karopapier. An jedem Zeitpunkt misst du den Abstand zwischen den beiden Linien. Viele kleine
> Abstände heißt: Das Modell passt.

Alle Abstände fasst Kern zu einer Zahl zusammen, dem **RMS-Fehler**: jeden Abstand mit sich selbst
malnehmen (quadrieren), davon den Mittelwert bilden, daraus die **Wurzel** ziehen. Die Wurzel ist die
Umkehrung des Quadrierens: √25 = 5, weil 5 · 5 = 25. Für krumme Zahlen nimmst du die √-Taste am
Taschenrechner.

**Beispiel:** Abstände 1° und 7°.

- Quadrieren: 1 und 49
- Mittelwert: (1 + 49) / 2 = 25
- Wurzel: √25 = **5°**

Der normale Mittelwert wäre nur 4°. Durch das Quadrieren zählt der große Ausreißer stärker. Genau das
will man: Ein Modell, das manchmal weit danebenliegt, soll schlecht abschneiden.

### Anpassen und prüfen

Jetzt sucht der Computer die sechs Zahlen des Profils, mit denen der Fehler möglichst klein wird. Das macht ein Suchverfahren namens **CMA-ES** ([Anhang A](A-grundlagen-neuronale-netze.md)):
Es probiert in jeder Runde ein Dutzend Varianten aus und rückt dorthin, wo die besten lagen.

> **So kannst du es dir vorstellen:** Topfschlagen mit zwölf Freunden: Alle suchen gleichzeitig,
> und die nächste Runde startet dort, wo es am lautesten „heiß“ hieß.

Entscheidend ist dann der Fehler auf der **Prüfmessung**, die bei der Suche nicht benutzt wurde. Nur
er zeigt, ob das Modell den Servo wirklich erklärt.

> **Vorsicht, Falle:** Wer nur an den Messungen prüft, mit denen er angepasst hat, lernt wie für eine
> Klassenarbeit mit bekannten Aufgaben. Die Prüfmessung ist die neue Aufgabe – anderes Gewicht,
> andere Bewegung.

In Kerns Test weicht ein Servo absichtlich stark vom üblichen Profil ab. Nach der Anpassung an zwei
Messungen ist der Fehler auf der dritten, unbekannten auf weniger als ein Fünftel geschrumpft.

> **Für Profis:** Das Werkzeug dafür heißt Lehrer und läuft im Terminal. Ohne echten Servo kann es
> auch Beispiel-Logs aus der Simulation schreiben, mit denen man die ganze Kette einmal durchspielt.
> Die genauen Befehle stehen im blauen Buch, Kapitel 16.

## 16.3 Abgleich: der ganze Roboter

Nach dem einzelnen Servo kommt der ganze Roboter. Der echte Roboter schreibt 50-mal pro Sekunde auf,
was seine Sensoren melden und welche Befehle sein Netz gibt. Die simulierte Ente bekommt dann genau
dieselben Befehle, Takt für Takt, und Kern vergleicht, was ihre Sensoren melden: wie sie sich dreht,
wo für sie „unten“ ist, wie die Gelenke stehen und wie schnell sie sich bewegen. Für jedes Gelenk
gibt es einen eigenen RMS-Fehler.

> **So kannst du es dir vorstellen:** Eine Tänzerin nimmt ihre Choreografie auf. Eine zweite tanzt
> sie Schritt für Schritt nach. Wo die beiden auseinanderlaufen, stimmt bei der zweiten etwas nicht.

Kern prüft das Verfahren mit einem Log aus der Simulation selbst: Mit denselben Servos ist der Abstand **genau 0**, Bit für
Bit. Mit anderen Servos ist er messbar größer.

**Das ist das Prinzip: Weil Kern deterministisch ist ([Kapitel 14](14-lehrer.md)), ist jeder
Unterschied zu einem echten Log ein echter Fehler im Modell und kein Zufall.**

## 16.4 Training mit dem eigenen Profil

Hat man das Profil der eigenen Servos gemessen, trainiert man die Bewegungen damit neu. Die Enten im
Training bekommen dann genau diese Servos, mit ihren Streuungen. So passt das Netz zu genau deinem
Roboter.

> **Für Profis:** Das gilt für die Bewegungen wie den Sprung. Die Spielschicht (das Netz fürs Spielen
> mit Ball und Socke) nimmt derzeit immer
> das mitgelieferte Servoprofil. Der Befehl steht im blauen Buch, Kapitel 16.

## 16.5 Variation (Domain Randomization)

Was man nicht messen kann, würfelt das Training ([Kapitel 14](14-lehrer.md)). Jede Übungsrunde
bekommt eine etwas andere Ente in einer etwas anderen Welt. Für das Netz ist die echte Micro Duck dann
nur eine Variante mehr.

> **So kannst du es dir vorstellen:** Wer Fußball nur auf perfektem Rasen übt, stolpert auf dem
> Bolzplatz. Wer auf Rasen, Asphalt und Matsch geübt hat, kommt überall klar.

| Was unsicher ist | Wie das Training es abdeckt |
|---|---|
| Servos, jeder etwas anders | Reibung und Schwung ±10 % |
| Verzögerung im Kabel | 15 bis 30 ms |
| Bau (Akku, Kabel) | Rumpf etwas schwerer, Schwerpunkt verschoben |
| Boden | Sohlen mal glatter, mal griffiger |
| Lagesensor, Winkelmesser | Rauschen, Versatz, Verzögerung |
| Stöße | alle 3 bis 6 Sekunden ein Schubs |
| Kamera | ungenaue Position, mal gar kein Bild |
| Spielzeug | Ball verschieden groß und schwer |

Der **Lagesensor** (Fachwort: IMU) spürt, wo unten ist und wie sich die Ente dreht. Der
**Winkelmesser** (Encoder) meldet, wie ein Gelenk steht. **Rauschen** heißt: Die Werte zittern etwas
um den wahren Wert.

Der Trainer im Lernen, der **Kritiker**, sieht dabei die saubere Wahrheit. Nur das Netz, das später
auf den Roboter kommt, bekommt die verrauschten Sensorwerte. So lernt es mit dem, was es auch in
Wirklichkeit hat.

Eine Lücke gibt es: **Getriebespiel**, das kleine Wackeln im Getriebe, wenn die Zahnräder nicht ganz
fest ineinandergreifen. Es fehlt in Kerns Modell. Pollens Laufnetz velstand wurde mit Getriebespiel
trainiert und kommt damit klar; Kerns eigene Netze kennen es noch nicht. Auf dem echten Roboter ist
das ein Risiko.

> **Mission:** Starte im Micro-Duck-Level **Simulate** (Alt+S), zieh die Ente mit der Maus am Kopf
> zur Seite und lass los, bis sie umfällt. Schau zu, was dann passiert: Sie steht von selbst wieder
> auf. Das klappt, weil velstand im Training ständig geschubst und mit vielen Abweichungen geübt hat.

## 16.6 In der Physik messen statt eintragen

Manche Zahlen ergeben sich aus dem Modell und der Physik. Kern tippt sie nicht von Hand ein, sondern
**misst sie in der Simulation**, so wie man es am echten Roboter tun würde. Der Grund: Eine
eingetippte Zahl stimmt nicht mehr, sobald sich etwas ändert, etwa die Reibung oder der Schnabel.
Eine gemessene misst man einfach neu.

**Beispiel Bissposition:** Wo muss eine Socke vor der Ente liegen, damit der Schnabel sie sicher
greift? Kern legt sie an 99 Stellen hin, probiert jedes Mal das Aufheben aus und nimmt die Stelle,
an der es am zuverlässigsten klappt. Keine Bissposition steht als feste Zahl im Programm.

Genauso:

- **„Hält etwas“** liest Kern am Winkelmesser des Schnabel-Servos ab: Er soll zu sein, steht aber
  noch offen? Dann steckt etwas dazwischen.
- **„Steht still“** heißt: Beide Sohlen berühren den Boden. Das misst die Physik über die Kontakte.

Noch von Hand eingetragen sind zwei Dinge: wie weit die Ente nach „Stopp“ noch weiterläuft und wie
stark verschiedene Stoffe aneinander reiben. Beide sind Kandidaten für Messungen am echten Material.

> **Mission:** Starte **Simulate** und schalte unter Window › Show **Contact Points** ein. Du
> siehst, wo die Sohlen den Boden berühren – genau daran erkennt die **Spielschicht** (das Netz, das
> entscheidet, was die Ente mit Ball und Socke tut), ob die Ente steht.

## 16.7 Grenzen

**Ganz ehrlich: Es gibt noch keine echten Messdaten.** Alles in diesem Kapitel ist mit Logs aus der
Simulation geprüft. Sobald ein echter Prüfstand misst, funktioniert dasselbe Werkzeug mit echten Logs.

Offen ist noch:

- **Getriebespiel** fehlt im Modell (siehe 16.5).
- **Schwung des Motors** ist vereinfacht nachgebaut.
- **Ein Kick** der Ente kippt in Kern manchmal um. Ein Abgleich mit dem echten Roboter soll klären,
  warum.
- **Lagesensor, Reibung zwischen Stoffen und Kamera** sind noch nicht am echten Gerät gemessen.

> **Für Profis:** Kern rechnet intern mit einer viermal so großen Ente ([Kapitel 13](13-micro-duck.md)).
> Profile und Logs sind trotzdem in echten Einheiten; umgerechnet wird nur an der Grenze zur Physik.

> **Mission:** Läuft das Level, stopp es mit **Esc**. Wähl die **Micro Duck**, stell im
> Details-Panel **Actions to servo goals** auf `robot` und starte Play. So fährt Pollens Software den
> echten Roboter: Sie macht die Befehle des Netzes etwas kleiner und glättet sie. Übernimm die Ente
> mit **F8** und drück **3** für den Kick. Dann dasselbe mit `training`. Mit `robot` kippt der Kick
> um, mit `training` bleibt sie stehen. Schon so ein kleiner Unterschied ändert das Verhalten. Darum
> lohnt es sich, Simulation und Wirklichkeit genau abzugleichen.

## Level geschafft!

- Du arbeitest am echten Roboter nur mit einem Erwachsenen.
- Du kannst einen Servo als Feder, Motor mit Kraftgrenze, Stoßdämpfer und Reibung beschreiben.
- Du weißt, wozu Streuungen dienen: Unsichere Zahlen werden im Training gewürfelt.
- Du kennst den Prüfstand: einspannen, messen, anpassen, mit einer neuen Messung prüfen.
- Du kannst einen RMS-Fehler ausrechnen (Taschenrechner erlaubt).
- Du verstehst, warum Determinismus den Abgleich so stark macht.

## Rätsel-Ecke

1. Was tust du, bevor du einen echten Servo anschließt?
2. Rechne den RMS-Fehler für die Abstände 0°, 0°, 0° und 10°. Vergleiche mit dem normalen Mittelwert.
3. Warum muss die Prüfmessung ein anderes Gewicht haben als die Messungen zum Anpassen?
4. Ein Servo ist sehr genau vermessen, bei einem anderen ist man unsicher. Welcher bekommt im Profil
   die größere Streuung, und warum?
5. **Boss-Rätsel:** Der Abgleich zeigt bei einem Gelenk 10-mal mehr Fehler als bei allen anderen.
   Was schließt du daraus?

### Lösungen

1. Einen Erwachsenen dazuholen, den Prüfstand einspannen, Strom begrenzen, Not-Aus bereitlegen.
2. Quadrate 0, 0, 0, 100; Mittelwert 100 / 4 = 25; √25 = 5°. Der normale Mittelwert wäre nur 2,5°:
   Ein großer Ausreißer zählt beim RMS-Fehler stark.
3. Nur so zeigt sich, ob das Modell den Servo erklärt oder die Kurven nur auswendig kann.
4. Der unsichere. Dann würfelt das Training dort mehr, und das Netz verlässt sich nicht auf eine Zahl,
   die niemand genau kennt.
5. Im Modell dieses Gelenks fehlt etwas (zum Beispiel Getriebespiel). Zufall kann es nicht sein, weil
   die Simulation deterministisch ist: Sie rechnet immer gleich.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
