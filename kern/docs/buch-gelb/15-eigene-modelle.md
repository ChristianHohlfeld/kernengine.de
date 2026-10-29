# 15 Eigene Modelle trainieren – dein Netz, deine Ente

> **Dein Level:** Jetzt trainierst du selbst. Du startest den Lehrer im Browser, liest seinen
> Fortschritt, wählst das beste Netz aus und lässt es im Studio springen. Und du lernst, worauf es
> ankommt, wenn man einer Ente eine ganz neue Bewegung beibringen will.

> **Wörter, die du brauchst:**
> - **Rückgabe** – alle Punkte, die eine Ente in einer Episode zusammen gesammelt hat
> - **Beobachtung** – alles, was das Netz in einem Takt zu sehen bekommt
> - **Aktion** – was das Netz antwortet, zum Beispiel 14 Servoziele
> - **Hyperparameter** – Einstellungen, die man vor dem Lernen festlegt, etwa die Zahl der Enten
> - **Lernrate** – wie groß jeder Lernschritt ist, also wie weit die Regler auf einmal gedreht werden
> - **Schnappschuss** – ein gespeicherter Zwischenstand des Netzes während des Trainings
> - **Kommandozeile** – ein Fenster, in das man Befehle tippt, statt Knöpfe zu klicken
> - **Totzone** – ein Bereich, in dem eine kleine Eingabe noch gar nichts bewirkt

Wie der Lehrer innen rechnet – Actor und Kritiker, PPO, Vorbild, Nachmachen, Variation – steht in
[Kapitel 14](14-lehrer.md). Hier probierst du es aus und lernst die Regeln, nach denen man ein
Training plant.

Es gibt zwei Wege. Der **Hauptweg** ist die **Lehrer-Seite im Browser**: Du brauchst nichts zu
installieren, und sie kann den Sprung und das Stehen trainieren. Der zweite Weg ist ein Programm für
die Kommandozeile. Das bekommt nur, wer Kern selbst aus dem Quellcode baut, und es kann ein paar Dinge
mehr. In diesem Kapitel steht es nur in den Kästen „Für Profis“.

> **Mission:** Öffne auf https://kernengine.de/ den Eintrag „Der Lehrer“, wähle „Sprung“ und drücke
> Start. Lass den Tab offen (ein Handy ans Ladekabel). Rechne mit 10 Minuten oder mehr, bis etwas
> passiert. Beobachte die Anzeige „echte Sprünge (sauberer Roboter)“: Ab wann springt dein Netz?

## 15.1 Die Lehrer-Seite

Die Lehrer-Seite ist eine komplette Trainingshalle im Browser. Du stellst oben ein, was trainiert wird,
drückst Start und siehst unten zu, wie das Netz besser wird. Sie rechnet mit demselben Trainingsplan
und derselben Physik wie das Programm auf einem großen Rechner.

> **So kannst du es dir vorstellen:** wie ein Backofen mit Sichtfenster. Du stellst Temperatur und
> Zeit ein, schiebst den Teig hinein und kannst durch die Scheibe sehen, wie er aufgeht.

Die Felder der Seite:

| Feld | Was es bedeutet |
|---|---|
| Bewegung | Sprung oder „Stehen“ als schneller Test |
| Worker | wie viele Helfer gleichzeitig rechnen |
| Enten je Worker | wie viele Enten jeder Helfer führt |
| Iterationen | wie viele Trainingsrunden |
| Seed | die Startzahl für den Zufall |
| Aktionsrauschen am Anfang | wie viel Zufall am Anfang (leer: wie im Plan) |
| Lernrate | wie große Schritte (leer: wie im Plan) |

Die Zahl der Worker schlägt die Seite selbst vor, passend zu deinem Gerät. Darunter liegen die Knöpfe
Start, Pause und „.onnx herunterladen“.

> **Für Profis:** Wer Kern auf dem eigenen Rechner baut ([Kapitel 2](02-bauen-und-testen.md), am besten
> zusammen mit einem Erwachsenen), bekommt mit einem Skript auch das Lehrer-Programm für die
> Kommandozeile. Es kann zusätzlich Spielstände fortsetzen, eigene Vorbilder suchen und die
> Spielschicht trainieren. Spannend: Das Skript darf das Programm genau für deinen Prozessor
> beschleunigen – das ändert **kein** Ergebnis, das Netz bleibt bis zur letzten Stelle dasselbe
> (Kapitel 14.7). Das Bauen und alle Befehle stehen im blauen Buch, Kapitel 15.

## 15.2 Was der Lehrer alles kann

Der Lehrer ist mehr als ein Trainingsknopf. Er ist ein ganzer Werkzeugkasten, damit man nicht nur
trainiert, sondern auch **misst** und **nachsieht**, was beim Training passiert.

> **So kannst du es dir vorstellen:** Ein Sportverein hat nicht nur einen Trainingsplatz, sondern auch
> eine Stoppuhr, eine Videokamera für die Zeitlupe und einen Arzt, der prüft, ob alles in Ordnung ist.

Das kann der Lehrer:

- **Trainieren:** Springen oder Stehen (im Browser), die Spielschicht (nur im Programm).
- **Messen:** Wie oft springt ein Netz echt, sauber und mit Variation?
- **Nachsehen:** eine Episode Takt für Takt anschauen, mit jedem Punkte-Posten einzeln.
- **Vorbild suchen:** einen neuen Sprung für das Nachmachen finden.
- **Prüfen:** den Fingerabdruck für den Determinismus bilden (Kapitel 14.7).
- **Servos nachmessen:** für die Kalibrierung ([Kapitel 16](16-kalibrierung.md)).

Im Browser gibt es davon das Trainieren und das Messen der Sprünge. Den Rest kann nur das Programm.
Die genauen Befehle stehen im blauen Buch, Kapitel 15.

## 15.3 Den Sprung trainieren

Ein Sprung-Training folgt immer demselben Plan, dem **Rezept**. Du musst nichts davon einstellen –
aber du solltest wissen, was gerade passiert, wenn du zusiehst.

> **So kannst du es dir vorstellen:** wie ein Trainingstag im Sportverein: aufwärmen mit dem, was du
> schon kannst, der Trainerin zuschauen und nachmachen, dann selbst üben, bis es sitzt.

Der Ablauf als Zeitstrahl:

1. **Rezept laden:** Start beim Lauf- und Stehnetz velstand, das schon stehen kann (Warmstart).
2. **Enten anlegen:** so viele Enten, wie Worker × Enten je Worker ergibt, jede mit ihrem eigenen
   Zufall.
3. **Nachmachen:** 8 Runden lang ahmt das Netz das Vorbild nach. Die Seite meldet „Nachahmen: Runde …
   von 8“ und prüft nach jeder Runde, ob das Netz schon springt.
4. **Kritiker zuerst:** Die ersten 30 Iterationen lernt nur der Kritiker.
5. **Üben:** Danach üben beide mit PPO, mit wenig Rauschen und voller Variation.
6. **Prüfen und speichern:** Alle 25 Iterationen springt das Netz 5-mal zur Probe, und die Seite
   speichert es.

Oft springt das Netz schon nach dem Nachmachen. Das Üben macht den Sprung danach **robust**, also
sicher auch mit einer etwas anderen Ente.

Zwei Dinge aus echten Läufen sind lehrreich:

- **Ohne Vorbild geht es nicht.** Reines Üben mit Punkten hat den Absprung nie gefunden (Kapitel 14.6).
- **Ein Training ist wie ein Spielstand.** Das Programm kann den ganzen Lernstand speichern und später
  genau dort weitermachen. Wegen des Determinismus kommt dabei dasselbe heraus wie ohne Pause.

Die Lehrer-Seite trainiert genau dasselbe Rezept wie das Programm. Gleicher Seed und gleich viele
Enten insgesamt ergeben dasselbe Netz – im Browser wie auf dem großen Rechner.

> **Für Profis:** Mit dem Programm entstand Kerns Sprungnetz: 32 Enten, Seed 1, 3000 Iterationen
> Üben, Schnappschüsse alle 250 Iterationen. Dort kann man auch ohne Vorbild trainieren, ein eigenes
> Vorbild suchen und einzelne Werte des Rezepts überschreiben. Befehle und Dateinamen: blaues Buch,
> Kapitel 15.

## 15.4 Fortschritt lesen

Während des Trainings zeigt die Seite nach jeder Iteration Zahlen und eine Kurve. Sie verraten, ob
das Netz wirklich etwas lernt. Die wichtigste Frage dabei: Steigen die Punkte, **und** kann die Ente
das, was sie soll?

> **So kannst du es dir vorstellen:** wie das Zeugnis und die Klassenarbeit. Die Punkte (Rückgabe)
> sind die Noten. Aber erst die echte Probe (echte Sprünge) zeigt, ob du es wirklich kannst – oder nur
> gelernt hast, wie man Punkte sammelt.

| Anzeige | Bedeutung | Worauf achten |
|---|---|---|
| mittlere Episoden-Rückgabe | Punkte pro Versuch | steigt über viele Iterationen |
| Episodenlänge (s) | wie lange ein Versuch dauert | Sprung: 2 s heißt ohne Sturz |
| Aktionsrauschen (std) | Wackelstärke | sinkt, wenn das Netz sicherer wird |
| echte Sprünge | Probe: 5 Sprünge | das eigentliche Ziel |
| Sprunghöhe, Flugzeit | wie gut gesprungen | sollen steigen |
| Schritte / s | Tempo deines Geräts | nur Tempo, kein Lernen |

Die Kurve zeigt Rückgabe und Episodenlänge über die Iterationen. Ein typischer guter Verlauf: Sie
zappelt von Runde zu Runde, steigt aber über Hunderte von Iterationen langsam an. Ist die Episodenlänge
kürzer als 2 Sekunden, enden Versuche zu früh – durch einen Sturz oder weil ein falsches Teil den
Boden berührt.

„std“ ist ein Fachkürzel für die Wackelstärke: wie stark der Zufall die Bewegungen typischerweise
verschiebt.

Wenn etwas schiefgeht, hilft das **Nachsehen**: Das Kommandozeilen-Programm zeigt eine Episode Takt
für Takt, mit jedem Punkte-Posten einzeln. So siehst du, ob das Netz springt – oder nur die Belohnung
austrickst. Die Lehrer-Seite im Browser kann das nicht. Dort schaust du dir dein Netz im Studio genau
an (15.8): Springt die Ente wirklich, oder macht sie etwas ganz anderes?

> **Für Profis:** Das Programm zeigt zwei Werte mehr. Der Fehler des Kritikers ist nach dem Nachmachen
> hoch (er kennt die neue Spielerin noch nicht) und soll dann fallen. Die Lernrate passt sich selbst
> an: Ändert sich die Policy pro Schritt zu stark, wird sie kleiner. Klebt sie dauerhaft am kleinsten
> erlaubten Wert, waren die Schritte also ständig **zu groß** – die Bremse ist voll angezogen. Alle
> Felder: blaues Buch, Kapitel 15.

## 15.5 Das beste Netz auswählen

Das letzte Netz ist nicht immer das beste. Ein Training kann nach einem guten Stand auch wieder
schlechter werden. Und die Probe während des Trainings sind nur 5 Sprünge – das kann Glück sein.

> **So kannst du es dir vorstellen:** wie beim Sportfest. Nicht der letzte Wurf zählt, sondern der
> beste. Und ein einzelner Glückstreffer macht noch keinen Sieger.

So wählt man richtig:

1. **Zwischenstände behalten:** Schnappschüsse speichern, statt nur den letzten Stand.
2. **Gründlich messen:** jeden mit viel mehr Episoden prüfen, sauber **und** mit Variation.
3. **Richtig sortieren:** zuerst echte Sprünge mit Variation, dann Stürze (am besten 0), dann
   Steigen, Flugzeit und Kippen.
4. **Im Studio ausprobieren** (15.8).

Für Kerns Sprungnetz ergab das 40 von 40 echte Sprünge sauber und 36 von 40 mit Variation, ohne Sturz.

Im Browser speichert die Seite alle 25 Iterationen den neuesten Stand und überschreibt den alten.
Siehst du einen guten Stand, sichere ihn mit „.onnx herunterladen“.

> **Für Profis:** Das Programm hebt Schnappschüsse automatisch auf und misst jeden mit 40 Episoden.
> Wie das geht, steht im blauen Buch, Kapitel 15.

## 15.6 Hyperparameter wählen

Hyperparameter sind die Einstellungen **vor** dem Lernen. Drei Regeln aus den echten Läufen reichen,
um sie zu verstehen.

**Regel 1: Die Lernrate ist die Schrittlänge.**

> **So kannst du es dir vorstellen:** Die Lernrate ist wie die Schrittlänge beim Laufen im Dunkeln.
> Zu kurz, und du kommst nie an. Zu lang, und du stolperst über die Bordsteinkante.

Nach dem Nachmachen ist die Ente schon gut. Darum lernt sie danach mit kleinen Schritten und wenig
Rauschen weiter. Große Schritte und viel Zufall würden ein gutes Netz leicht kaputtmachen. Und mehr
Rauschen hilft nicht beim Finden: Mit viel Zufall und ohne Startnetz lernte die Ente zu stehen, nicht
zu springen – denn Stehen bringt sicher Punkte, und der Zufall trifft den Absprung trotzdem nicht
(Kapitel 14.6).

**Startnetz und Netzgröße gehören zusammen.** Wer beim fertigen Stehnetz velstand startet, muss
dessen Größe behalten, also gleich viele Schichten und Regler. Ein Startnetz ist wie ein halb
fertiges Puzzle: Du kannst weiterlegen, aber nicht plötzlich einen größeren Rahmen nehmen. Ein Netz
anderer Größe muss bei null anfangen.

**Regel 2: Mehr Enten ändern das Netz, mehr Helfer nur das Tempo.** Ein Beispiel:

| Worker × Enten je Worker | Enten | Ergebnis |
|---|---|---|
| 2 × 16 | 32 | Netz A |
| 4 × 8 | 32 | wieder Netz A, nur schneller |
| 4 × 16 | 64 | Netz B, ein anderes |

Mehr Enten heißt mehr Erfahrung pro Iteration – also andere Daten und ein anderes Netz. Wenige Enten
bremsen das Lernen, weil jede Runde dann wenig Erfahrung bringt. Mehr Helfer verteilen dieselbe Arbeit
nur auf mehr Köche.

**Regel 3: Die Physik muss genau wie im Studio sein.** Man könnte die Physik gröber rechnen und wäre
schneller. Aber dann bewegt sich die Ente anders, und das Netz lernt eine andere Welt als die, in der
es später springen soll.

> **Für Profis:** Das Programm hat für jeden Hyperparameter einen eigenen Schalter: wie viele Takte
> jede Ente pro Iteration macht, wie oft der Auswerter die Daten durchgeht, wie groß die Netze sind,
> welches Servoprofil gilt. Die Liste mit allen Werten des Sprung-Rezepts steht im blauen Buch,
> Kapitel 15.

## 15.7 Die Spielschicht trainieren

Die Spielschicht – die Ente geht zu Ball oder Socke, kickt oder hebt auf – lernt man nur mit dem
Programm, nicht im Browser. Spannend ist sie trotzdem, denn sie zeigt den ganzen Weg in sechs Schritten.

> **So kannst du es dir vorstellen:** wie ein Kochrezept ausprobieren. Erst prüfst du, was deine
> Küche kann. Dann kochst du nach, schmeckst ab, schaust, wo es anders wurde, verbesserst – und
> servierst es am Ende den Gästen.

1. **Messen:** Wo greift der Schnabel eine Socke gut? Das misst der Lehrer bei jedem Lauf in der
   Physik, statt es hinzuschreiben. Wie weit die Ente nach „Stopp“ noch läuft, hat ein Messbefehl
   einmal ermittelt; diese Zahl steht noch fest im Programm.
2. **Nachmachen:** Das Netz ahmt das Vorbild der Spielschicht nach, Runde für Runde.
3. **Prüfen:** Kicks, Aufheben, Ausspucken und Stürze zählen – für das Netz **und** für das Vorbild.
4. **Ansehen:** eine Episode Entscheidung für Entscheidung, das Vorbild und das Netz nebeneinander.
   So sieht man, wo das Netz abweicht.
5. **Üben:** mit PPO, Kritiker zuerst, mit wenig Rauschen.
6. **Im Level prüfen:** das Beispiel-Level genau wie im Studio durchspielen lassen.

Ein wichtiger Gedanke steckt in Schritt 3: **Das Vorbild ist die Obergrenze.** Ein Netz, das nur
nachmacht, wird höchstens so gut wie sein Vorbild. Kann schon das Vorbild die Socke nicht festhalten,
liegt das Problem nicht beim Netz. Besser als das Vorbild kann es erst beim Üben werden, weil dort
nur zählt, was wirklich klappt.

> **Für Profis:** Alle Befehle für die sechs Schritte stehen im blauen Buch, Kapitel 15.

## 15.8 Im Studio benutzen

Ein Netz, das du im Browser trainierst, landet von selbst im Studio – auf demselben Gerät, im selben
Browser. Die Lehrer-Seite speichert es auf deinem Gerät, und Kern Studio lädt es beim Start. Offene
Studio-Tabs übernehmen jedes neue Netz sofort. Das gilt für den Sprung.

> **So kannst du es dir vorstellen:** wie ein Spielstand, den zwei Spiele auf deiner Konsole
> gemeinsam benutzen dürfen. Das eine speichert ihn, das andere lädt ihn.

Zum Prüfen: Micro-Duck-Level öffnen, **Spielen** (Play), Taste **7**. Die Spielschicht arbeitet von
allein, wenn die Ente autonom ist, die Kern-Extras an sind und Ball oder Socke in Sicht sind
([Kapitel 13](13-micro-duck.md)).

> **Mission:** Hast du im Browser einen Sprung trainiert? Öffne im selben Browser Kern Studio, lade
> **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck) – läuft gerade ein
> Spiel, erst mit Esc stoppen –, starte **Spielen**, drücke F8 und dann **7**. Jetzt springt **dein** Netz. Am Computer kannst du es nachprüfen: Drück F12, wähle den
> Reiter „Konsole“, dort steht „Kern: trained network jump.onnx loaded“. Auf dem Handy gibt es diese
> Konsole nicht – da reicht der Sprung als Beweis.

> **Mission:** Klick im Micro-Duck-Level die Ente an und schau unter
> **Details › Spieler-Steuerung › Läufer** (Details › Player Controller › Walker) nach
> **Autonom (Kamera, ToF)** (Autonomous (camera, ToF)) und
> **Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)** (Kern extras (ball play, sock
> fetching, beak in the pick)). Starte **Spielen** einmal mit und einmal ohne Kern-Extras (zum
> Umschalten erst mit Esc stoppen). Achte auf Ball und Socke: Mit Kern-Extras entscheidet die
> gelernte Spielschicht, was die Ente tut – sie geht auch zur Socke und versucht, sie aufzuheben.
> Ohne Kern-Extras übernimmt Pollens nachgebautes Ballspiel (BallPlay,
> [Kapitel 13](13-micro-duck.md)): Die Ente läuft nur zum Ball und kickt ihn; um die Socke kümmert
> sie sich nicht gezielt.

> **Für Profis:** Wer Kern selbst baut, kann ein Netz auch als Datei in Kerns Ordner für Netze legen.
> Kern prüft dabei, ob das Netz die richtige Form hat; ein Netz mit falscher Größe wird nicht
> benutzt. Ordner und Dateinamen: blaues Buch, Kapitel 15.

## 15.9 Eine neue Aufgabe hinzufügen

Eine neue Bewegung beizubringen ist mehr als „trainiere mal“. Man muss vorher genau sagen, was gelernt
werden soll und woran man den Erfolg erkennt. Dafür gibt es **fünf Schritte**; Schritt 4, das Vorbild,
braucht man nur für schwere Bewegungen.

> **So kannst du es dir vorstellen:** wie eine neue Sportart im Verein. Man braucht Spielregeln, einen
> Trainingsplan, einen Schiedsrichter, manchmal jemanden, der es vormacht – und am Ende einen Platz
> im Spielplan.

Nehmen wir als Beispiel eine **Verbeugung**:

1. **Die Aufgabe (die Spielregeln):** Wie startet eine Episode, wie lange dauert sie (bei der
   Verbeugung zum Beispiel 2 Sekunden), wofür gibt es Punkte? Etwa: Punkte, wenn der Kopf tief geht
   und beide Füße am Boden bleiben, Abzug fürs Zappeln, Ende bei einem Sturz. Dazu: Was darf der
   Kritiker extra sehen – etwa die wahre Höhe des Rumpfs?
2. **Das Rezept (der Trainingsplan):** Mit welchem Netz startet das Training, mit wie viel Rauschen
   und welcher Lernrate? Danach kennen Programm und Browser die neue Aufgabe; der Lehrer-Seite fehlt nur
   noch ein Eintrag im Menü. Braucht die Bewegung eine neue Eingabe, die das Startnetz nie gesehen hat
   (beim Sprung die Zeiger-Uhr), beginnen deren Regler bei 0 – wie in Kapitel 14.4: Erst handelt das
   neue Netz wie das alte, dann lernt es die neue Eingabe zu nutzen.
3. **Die Messung (der Schiedsrichter):** Ein Prüfer, der feststellt, ob eine Verbeugung **wirklich**
   gelungen ist – so wie es für den Sprung festgelegt ist, was ein echter Sprung ist.
4. **Ein Vorbild (nur bei Bedarf):** Findet das Üben die Bewegung nicht, beschreibt man sie mit
   wenigen Zahlen, lässt CMA-ES die besten suchen und baut ein Vorbild, das in **jeder** Lage eine
   Antwort hat. Nur dann kann das Netz es mit DAgger nachmachen.
5. **Im Studio einbauen (der Spielplan):** eine Taste, ein Knopf fürs Handy, ein Name für den
   Logik-Knoten **Robot Move** ([Kapitel 13](13-micro-duck.md)) und das Netz selbst. Die neue
   Bewegung läuft dann wie der Sprung: Drückst du die Taste, wechselt Kern hart auf ihr eigenes Netz,
   und nach ihrer Dauer (bei der Verbeugung 2 Sekunden) zurück zum Laufnetz. Wie ein Staffelstab:
   Immer hält genau ein Netz die Beine.

Der wichtigste Satz dieses Abschnitts steht in Schritt 3: **Miss, was die Bewegung ist – nicht, was
die Punkte zählen.** Die Punkte sind nur ein Lockmittel. Ein Netz findet oft einen Weg, Punkte zu
holen, ohne die Bewegung zu machen. Nur eine strenge Messung deckt das auf.

Eine Bewegung, die ganz andere Eingaben und Ausgaben braucht als die Bein-Netze, zeigt die
Spielschicht: Sie hat eine eigene Halle, eigene Punkte und eine eigene Messung. Das Lernen selbst –
PPO und Nachmachen – bleibt dasselbe.

> **Für Profis:** Wie Aufgabe, Rezept und Einbau im Programmcode aussehen, steht mit einem
> vollständigen Gerüst für die Verbeugung im blauen Buch, Kapitel 15.

## 15.10 Beobachtung, Aktion und Belohnung entwerfen

Das ist der wichtigste Abschnitt dieses Kapitels. Ein Netz lernt nur, was man ihm möglich macht: Was
es sieht (die **Beobachtung**), was es tun kann (die **Aktion**) und wofür es Punkte bekommt (die
**Belohnung**). Wer das gut entwirft, bekommt ein Netz, das auf dem echten Roboter funktioniert. Wer
es schlecht entwirft, bekommt ein Netz, das schummelt oder sich auf etwas verlässt, das es gar nicht
gibt.

> **So kannst du es dir vorstellen:** Du erfindest ein Spiel für deine kleine Schwester. Sagst du „Wer
> zuerst fertig ist, gewinnt“, wird sie schnell – aber vielleicht schlampig. Was belohnt wird, wird
> gemacht. Genau so ist es beim Netz.

Kerns Netze zeigen sieben Regeln.

### 1. Nur zeigen, was der Roboter wirklich hat

Der Actor bekommt nur, was die Sensoren des echten Roboters messen: Neigung, Drehung, Gelenkwinkel,
was die Kamera erkennt. Nie die wahre Position von Ball oder Socke. Die Spielschicht kennt den Ball nur
so, wie die Kamera ihn sieht – ungenau und manchmal gar nicht.

Warum so streng? Ein Netz verlässt sich auf alles, was es bekommt. Bekommt es im Training die wahre
Ballposition, nutzt es sie. Auf dem echten Roboter gibt es sie nicht – und das Netz ist blind. Die
wahren Werte bekommt darum nur der Kritiker, denn der kommt nie auf den Roboter.

### 2. Die Zeit sichtbar machen

Ein Netz hat **kein Gedächtnis**. Es sieht nur den jetzigen Takt. Woher soll es wissen, wann im Sprung
es ist oder wie lange es schon steht? Man muss es ihm zeigen.

Beim Sprung ist das die Uhr mit einem Zeiger (Kapitel 14.6): Das Netz bekommt die zwei Koordinaten
der Zeigerspitze, und der Zeiger läuft in 2 Sekunden einmal herum. Ein Beispiel: Nach 0,5 Sekunden hat
er eine Viertelrunde geschafft. Begann er rechts bei (1 | 0), steht er jetzt oben bei (0 | 1). Beide
Zahlen bleiben immer zwischen −1 und +1, wie lange die Episode auch läuft. Ein einfacher Zähler würde
dagegen immer größer.

Die Spielschicht bekommt stattdessen Zeiten zu sehen: wie lange sie schon steht, wie lange sie geht und
wie lange sie etwas trägt.

### 3. Messen statt befehlen

„Die Ente steht“ heißt in der Spielschicht: **Beide Sohlen sind länger als eine Viertelsekunde am
Boden** – also das, was die Beine wirklich tun. Nicht: „Der Laufbefehl ist null.“ Warum? Bei ganz
kleinen Laufbefehlen setzt der Gang manchmal gar nicht an. Dann **steht** die Ente, obwohl ein Befehl
anliegt. Wer dem Befehl glaubt statt der Messung, irrt sich.

Dieselbe Idee steckt im echten Sprung (Kapitel 14.6): Gezählt wird, was der Körper tut – Flug, Steigen,
Landen –, nicht was das Netz wollte.

### 4. Aktionen wie Bedienelemente

Die Aktionen der Spielschicht funktionieren wie ein Gamepad:

- **Laufbefehle mit Totzone,** wie der Stick: Unter 20 % Ausschlag steht die Ente. Bei 15 % passiert
  also nichts, bei 60 % geht sie los. So zittert sie nicht bei jeder winzigen Zahl.
- **Bewegungen als Knopf:** Kick und Aufheben sind Zahlen zwischen −1 und +1. Erst über 0,5 gilt der
  Knopf als gedrückt.
- **Grenzen für Kopf und Hals,** damit das Netz nichts Unmögliches verlangt.

Solche Aktionen sind für das Netz leicht zu treffen: Es muss nicht eine exakte Zahl erwischen, sondern
nur den richtigen Bereich.

### 5. Punkte-Posten mit Namen

Die Belohnung ist keine einzelne Zahl, sondern eine Summe aus Posten mit Namen: „aufrecht“, „landen“,
„zappeln“, „Sturz“ … Beim Nachsehen im Programm (15.4) kann man jeden Posten einzeln anschauen.
Steigen die Punkte, aber die Ente springt nicht, sieht man so sofort, **welcher** Posten die Punkte
bringt – und findet den Trick, mit dem das Netz schummelt.

Dazu kommen kleine Abzüge, die gute Manieren fördern: fürs Zappeln, fürs Wackeln, wenn die Ente sich
selbst berührt. Und manche Strafen wachsen per **Lehrplan**: Am Anfang darf die Ente noch zappeln und
fallen, ohne dass es viel kostet. Wenn sie die Bewegung allmählich kann, werden die Strafen größer.

### 6. Ein hartes Ende

Ein Sturz und jeder Bodenkontakt eines Teils, das keine Sohle ist, beenden die Episode **sofort**.
Ein paar Minuspunkte würden nicht reichen. Rechnen wir: Kostet Abstützen auf dem Schnabel 1 Punkt, aber
die Ente bleibt so noch 1 Sekunde stehen und holt 2 Punkte fürs Aufrechtbleiben, dann lohnt sich das
Abstützen. Ein sofortiges Ende nimmt ihr alle späteren Punkte – dann lohnt es sich nie.

### 7. Ein Vorbild ohne Geheimwissen

Soll das Netz ein Vorbild nachmachen, darf das Vorbild nur sehen, was auch das Netz sieht, und sich
nichts heimlich merken. Sonst kann das Netz es nicht lernen – wie bei einem Lehrer, der in die
Lösung schaut.

Ein Beispiel aus der Spielschicht: Das Vorbild will kicken, wenn die Ente lange genug steht. Würde es
das als einen einzelnen Knopfdruck in genau einem Takt melden, müsste das Netz diesen einen Takt
treffen. Stattdessen steigt der Wunsch als **Rampe**: Nach 1 Sekunde Stehen ist er −1, nach 1,25
Sekunden 0, nach 1,5 Sekunden +1. Die Stehzeit sieht das Netz (Regel 2). So kann es den Wunsch direkt
aus dem ablesen, was es sieht. Über 0,5 gilt der Knopf als gedrückt (Regel 4).

## 15.11 Fallstricke aus den echten Läufen

Beim Training von Kerns Netzen ging einiges schief. Aus jedem Fehler wurde eine Regel. Wer selbst
trainiert, stolpert sonst über dieselben Steine.

> **So kannst du es dir vorstellen:** wie ein Fehlerheft in Mathe. Wer aufschreibt, was schiefging,
> macht denselben Fehler nicht zweimal.

- **Die Belohnung wurde ausgetrickst.** Die Ente setzte sich auf den Schwanz, machte einen Hechtsprung
  oder stützte sich ab – alles gab Punkte, keins war ein Sprung. Geholfen haben nur strengere Regeln:
  hartes Ende bei Bodenkontakt, Abzug fürs Kippen, Punkte nur aufrecht.
- **Die Messung war zu locker.** Ein „Sprung“, bei dem die Ente sich nur aufrichtete, die Füße anzog
  und um 45° kippte, wurde erst mitgezählt. Seitdem zählt nur ein echter Sprung.
- **Seltene Bewegungen findet Üben allein nicht.** Was alle Servos gleichzeitig mit voller Kraft
  braucht, entsteht durch Zufall praktisch nie. Vorbild und Nachmachen lösten das.
- **Ein ahnungsloser Kritiker** macht eine gute Spielerin kaputt. Darum lernt nach dem Nachmachen
  zuerst nur der Kritiker.
- **Fehler in der Welt statt im Netz.** Die Socke kroch von selbst über den Boden, der Ball rollte
  ewig, und das Gähnen der Ente (eine ihrer Leerlauf-Bewegungen, [Kapitel 13](13-micro-duck.md))
  öffnete beim Aufheben den Schnabel. Lernt ein Netz an einer Stelle einfach nicht, kann ein solcher
  Fehler der Welt dahinterstecken – das Nachsehen Takt für Takt im Programm zeigt ihn.
- **Zahlen von Hand veralten.** Wo der Schnabel greift, stand früher als feste Zahl in der
  Beschreibung. Ändert sich die Physik, stimmt sie nicht mehr. Jetzt misst der Lehrer sie bei jedem
  Lauf neu.
- **Der Determinismus bricht leicht.** Eine einzige Rechnung, die anders rundet, und das Netz ist auf
  einem anderen Gerät ein anderes (Kapitel 14.7). Der Fingerabdruck zeigt es.
- **Eine andere Physik als im Studio** heißt: Das Netz lernt eine andere Welt.

> **Für Profis:** Ein Fallstrick betrifft den Umrechner (Kapitel 14.4). Bei wenigen Dutzend Enten
> ändert er sich von Iteration zu Iteration merklich. Darum rechnet der Auswerter die alte Policy vor
> jedem Lernschritt mit dem neuen Umrechner nach – sonst würde er alt und neu schief vergleichen.
> Mehr im blauen Buch, Kapitel 15.

## Level geschafft!

- Du trainierst einen Sprung auf der Lehrer-Seite und weißt, was dabei gerade passiert.
- Du liest Rückgabe, Episodenlänge, Rauschen und die Sprungprobe – und weißt, dass nur die Probe
  zeigt, ob die Ente es wirklich kann.
- Du wählst das beste Netz aus, sauber und mit Variation gemessen.
- Du weißt, was das Netz ändert (die Zahl der Enten) und was nur das Tempo (die Zahl der Helfer).
- Du bringst dein Netz ins Studio.
- Du kennst die sieben Regeln, nach denen man Beobachtung, Aktion und Belohnung entwirft.

## Rätsel-Ecke

1. Welche Taste startet im Studio den Sprung?
2. Du trainierst mit 2 Workern × 16 Enten, Seed 1. Deine Freundin nimmt 4 Worker × 8 Enten, auch Seed 1.
   Bekommt ihr dasselbe Netz? Und wenn sie 4 × 16 nimmt?
3. Warum lernt die Ente nach dem Nachmachen mit kleinen Schritten und wenig Rauschen weiter?
4. Du willst der Ente eine Verbeugung beibringen und gibst Punkte, je tiefer der Kopf ist. Was könnte
   die Ente stattdessen lernen, und welche Regel hilft?
5. **Boss-Rätsel:** Dein Netz bekommt immer mehr Punkte, aber bei „echte Sprünge“ steht dauernd 0 von
   5. Was ist wahrscheinlich passiert, und wie findest du es heraus?

### Lösungen

1. Die **7** (auf dem Handy der Knopf HOP).
2. Ja: 2 × 16 = 32 und 4 × 8 = 32 Enten, gleicher Seed – dasselbe Netz, ihres nur schneller. Mit
   4 × 16 = 64 Enten entstehen andere Erfahrungen und darum ein anderes Netz.
3. Nach dem Nachmachen kann sie schon springen. Große Schritte und viel Zufall könnten das gute Netz
   kaputtmachen – wie zu viel Salz in einer fertigen Suppe.
4. Sie könnte sich hinlegen oder mit dem Kopf auf den Boden stützen. Das gibt viele Punkte, ist aber
   keine Verbeugung. Hilfe: ein hartes Ende, wenn ein anderes Teil als die Sohlen den Boden berührt,
   und eine strenge Messung, was eine echte Verbeugung ist.
5. Das Netz trickst die Belohnung aus: Es hat einen Weg gefunden, Punkte zu holen, ohne zu springen.
   Finde heraus, **welcher** Punkte-Posten die Punkte bringt: Mit dem Kommandozeilen-Programm schaust
   du eine Episode Takt für Takt an; im Browser lädst du das Netz ins Studio und schaust genau zu,
   was die Ente stattdessen tut (sich abstützen, sich auf den Schwanz setzen …). Dann machst du die
   Regeln strenger.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
