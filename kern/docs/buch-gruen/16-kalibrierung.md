# 16 Kalibrierung: aus dem Computer in die echte Welt

> **Kurz gesagt:** Ein Netz aus dem Lehrer soll auf der echten Micro Duck laufen. Dafür muss die
> Simulation den Roboter gut treffen, und das Netz darf sich auf nichts verlassen, was die
> Simulation nicht genau weiß. Kern vermisst dazu die Motoren, vergleicht den ganzen Roboter mit der
> Simulation und übt den Rest unter vielen leicht verschiedenen Bedingungen ein.

> **Begriffe vorab:**
> - **Servo** – ein kleiner Motor, der auf einen vorgegebenen Winkel fährt.
> - **Netz** – das gelernte Rechenwerk der Ente: Es bekommt Messwerte und antwortet mit Zielen für
>   die Servos.
> - **Lehrer** – Kerns Trainingssystem, in dem die Netze der Ente lernen ([Kapitel 14](14-lehrer.md)).
> - **Spielschicht** – Kerns eigenes Netz, das entscheidet, was die Ente mit Ball und Socke tut.
> - **Simulation** – die nachgebaute Ente im Computer, die nach den Regeln der Physik läuft.
> - **Episode** – ein Übungsdurchgang im Training, vom Start bis zum Sturz oder bis die Zeit um ist.
> - **Streuung** – wie weit ein Wert schwanken darf, etwa „plus/minus zehn Prozent“.
> - **deterministisch** – die Simulation rechnet bei gleichen Eingaben immer haargenau dasselbe.

Kern verfolgt drei Wege und einen Grundsatz:

1. **Gerätemodelle mit Profilen:** Ein Servo bekommt ein kleines Rechenmodell, beschrieben durch
   wenige Zahlen. Diese Zahlen passt man an Messungen an und prüft sie an anderen Messungen.
2. **Abgleich:** Eine Aufzeichnung des echten Roboters wird in der Simulation nachgespielt; der
   Unterschied ist der Abstand, der zur Wirklichkeit noch fehlt.
3. **Variation:** Was keine Messung genau festlegt, würfelt das Training in jeder Episode neu aus.
4. **Grundsatz:** Was aus Modell und Physik folgt, misst der Lehrer in der Simulation, statt es als
   feste Zahl hineinzuschreiben – etwa, wo die Socke liegen muss, damit die Ente sie greift (16.6).

An der Physik-Engine selbst (Box3D, [Kapitel 6](06-physik.md)) wird dabei nichts verändert. Man
könnte versucht sein, die Physik so zu verbiegen, bis die Ente passt – dann aber stimmte sie für
alles andere im Level nicht mehr. Darum bleibt die Physik für alle Dinge gleich, und was ein Motor
zusätzlich tut, kommt als eigener Baustein obendrauf, so wie ein Motor auf ein Gelenk geschraubt
wird.

> **Stell dir vor …** du übst am Keyboard und spielst morgen auf einem echten Flügel. Du kannst
> prüfen, wie schwer seine Tasten gehen (Gerätemodell), eine Aufnahme vom Flügel mit deinem Üben
> vergleichen (Abgleich) und auf vielen verschiedenen Instrumenten üben (Variation).

> **Achtung, Falle!** Dieses Kapitel beschreibt auch Messungen an echter Hardware. Servos unter Last
> können schnell und kräftig ausschlagen. Prüfstand fest einspannen, Arm und Gewicht sichern, Hände
> und Kabel aus dem Schwenkbereich, Strom begrenzen, Not-Aus griffbereit. Nie über die
> Betriebsspannung des Herstellers gehen, Lithium-Akkus nur mit geeignetem Ladegerät und unter
> Aufsicht laden. Neue Netze zuerst am gehaltenen oder aufgehängten Roboter testen. Alles auf eigenes
> Risiko (siehe [Impressum](00-impressum.md)).

## 16.1 Das Servomodell und sein Profil

Ein Servo wird im Computer nicht Zahnrad für Zahnrad nachgebaut. Kern beschreibt nur, **wie er sich
anfühlt**: wie fest er zieht, wie er bremst, wie träge und wie spät er reagiert. Diese wenigen Zahlen
zusammen heißen **Profil**. Ein anderer Servo ist einfach ein anderes Profil.

Am einfachsten stellst du dir den Servo als vier Teile vor, die zwischen dem Gelenk und seinem
Zielwinkel sitzen:

```
  Zielwinkel                                     Gelenk
      ●──── Feder ────────────────────────────────●
      │     zieht zum Ziel, aber nie stärker
      │     als der Motor kann (Kraftgrenze)
      │
      ●──── Stoßdämpfer ──────────────────────────●
            bremst, je schneller es sich dreht

  dazu: Reibung im Gelenk, ein schweres Schwungrad
        innen, und Befehle, die etwas spät ankommen
```

| Eigenschaft | Bild aus dem Alltag | Was sie bewirkt |
|---|---|---|
| Steifigkeit | eine Feder, die zum Ziel zieht | je weiter weg, desto kräftiger der Zug |
| Kraftgrenze | ein Mensch, der nur so fest ziehen kann | weit weg zieht er nicht noch fester |
| Dämpfung | der Stoßdämpfer am Fahrrad | schnelle Bewegungen werden gebremst |
| Reibung | ein schwergängiges Scharnier | kleine Kräfte bewegen erst einmal nichts |
| Armatur | ein schweres Schwungrad | schwer in Gang zu bringen und zu stoppen |
| Verzögerung | ein Befehl, der per Bote kommt | der Servo reagiert ein paar Hundertstel später |

Die **Armatur** braucht einen Satz mehr: Im Servo dreht sich ein kleiner Motor sehr schnell und
treibt über ein Getriebe das Gelenk. Dieser flinke Motor muss bei jeder Bewegung mit beschleunigt
werden, darum fühlt sich das Gelenk schwerer an, als seine Teile wiegen.

Dasselbe Modell rechnet im Studio, im Lehrer und auf dem Prüfstand, den du gleich in 16.2
kennenlernst; was an einer Stelle gemessen ist, gilt also überall. Ein grundsätzlich anderes Gerät
bekäme ein eigenes Modell.

Man fängt nicht bei null an: Kern bringt ein fertiges **Standardprofil** mit, die Werte, die Pollen
Robotics für den Servo der Micro Duck ermittelt hat. Eigene Messungen verfeinern es nur für deine
Servos.

Alle Zahlen im Profil stehen in echten Einheiten, so wie man sie am echten Servo misst. Dass die Ente
im Computer viermal so groß ist ([Kapitel 13](13-micro-duck.md)), rechnet Kern erst ganz zum Schluss
um, an der Grenze zur Physik. Darum gilt ein Profil, das du am echten Servo gemessen hast, ohne
Umrechnen auch in der Simulation.

Zu jeder Zahl gehört eine **Streuung**: eine ehrlich zugegebene Unsicherheit. Das Training zieht
daraus in jeder Episode für jeden Servo eigene Werte. Ein gut gemessener Servo bekommt eine kleine
Streuung, ein schlecht bekannter eine große. So hängt das Netz nicht an Zahlen, die niemand genau
kennt.

Die genauen Werte und das Dateiformat des Profils stehen im blauen Buch, Kapitel 16.

> **Probier's aus!** Öffne in Kern Studio auf https://kernengine.de/
> **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck), starte **Spielen**
> (Play), übernimm die Ente mit **F8** und drück die **1** (Sitzen). Schau
> auf die Beine: Sie fahren in die neue Haltung, statt hineinzuspringen. Daran arbeiten zwei zusammen:
> das Netz fürs Sitzen, das 50-mal pro Sekunde neue Ziele schickt, und das Servomodell, das jedes
> Gelenk mit begrenzter Kraft und gebremst zu seinem Ziel zieht. Welcher Anteil der Bewegung auf
> welches Konto geht, sieht man von außen nicht. Sicher ist nur: Kein Gelenk der Ente erreicht
> sein Ziel im selben Augenblick, jedes geht den Weg durch das Servomodell.

## 16.2 Der Prüfstand: einen Servo vermessen

Woher kommen die Zahlen eines Profils? Man misst sie. Dazu spannt man einen einzelnen Servo ein,
hängt einen Hebelarm mit Gewicht daran, schickt ihm Bewegungsbefehle und zeichnet auf, was er
wirklich tut. Dieser Teil richtet sich an Bastler mit echter Hardware; zum Verstehen reicht es, das
Prinzip zu kennen.

### So misst man

1. **Aufbau:** Servo fest einspannen, Achse waagerecht, ein Arm mit Gewicht daran, Gewicht und
   Hebellänge genau messen. In Ruhe hängt der Arm senkrecht nach unten wie ein Uhrpendel.
2. **Wie im Roboter betreiben:** dieselben Einstellungen und dieselbe Spannung wie in der Ente.
3. **Verschiedene Befehle senden** und mindestens 100-mal pro Sekunde den Winkel abfragen: kleine
   Sprünge (etwa 10 Grad) und große (etwa 50 Grad), ein immer schnelleres Hin und Her (ein
   **Chirp**, wie ein ansteigendes Vogelzwitschern) und ein gleichmäßiges Hin- und Herspringen
   zwischen zwei Winkeln.
4. **Zwei oder drei Lasten** ausprobieren. Mindestens eine Messung je Servo legt man beiseite; sie
   dient später nur zum Prüfen.
5. **Mehrere Servos** messen. Wie stark sie sich unterscheiden, wird zur Streuung im Profil.

### Die Messung im Computer nachspielen

Die Aufzeichnung (das **Log**) ist eine einfache Textdatei: zu jedem Zeitpunkt der gesendete und
der gemessene Winkel. Kern baut nun denselben Servo mit derselben Last in der Simulation nach und
schickt ihm dieselben Befehle.

Wichtig dabei: Der nachgebaute Prüfstand rechnet mit genau denselben Einstellungen wie die Ente im
Studio – gleicher Vergrößerungsmaßstab, gleich viele Physikschritte pro Sekunde, gleich weiche
Gelenke. Sonst würde ein Profil, das auf dem Prüfstand passt, in der Ente nicht mehr passen – so
wie eine Hose, die du mit dicken Winterstiefeln anprobiert hast, mit Sandalen anders sitzt.

Heraus kommen zwei Kurven:

```
  Winkel
    │          ──────────────       ──  gemessen
    │        ╱   • • • • • • •      •   simuliert
    │      ╱   •
    │    ╱   •
    │  ╱   •
    │╱ •
    └─────────────────────────────► Zeit
      Abstand = wie weit die beiden Kurven auseinanderliegen
```

Je besser das Profil, desto genauer decken sich die Kurven. Den Abstand fasst Kern in **eine Zahl**
zusammen: den durchschnittlichen Abstand zwischen beiden Kurven, wobei große Ausreißer stärker
zählen. Fachleute nennen das **RMS-Fehler**.

> **Probier's aus!** Wie streng Kern auf gleiche Einstellungen achtet, siehst du im Studio. Stell im
> Micro-Duck-Level unter **Welteinstellungen** (World Settings) die **Schrittrate** (Step rate), also
> die Zahl der Physikschritte pro Sekunde, auf 90 statt 100 und starte **Spielen** (Play). Kern
> meldet im **Ausgabeprotokoll** (Output Log): „set World Settings > Step Rate to 100“.
> Alles – Training, Vermessen, Spielen – rechnet mit 100 Schritten pro Sekunde; mit 90 wäre es eine
> andere Welt, und kein Profil und kein Netz wäre mehr darauf abgestimmt. Stell danach wieder 100
> ein.

### Anpassen und prüfen

Jetzt sucht der Computer die Profilwerte, bei denen die simulierte Kurve die gemessene am besten
trifft. Er dreht an sechs Stellschrauben: den fünf Eigenschaften des Servos und der Verzögerung.
Gesucht wird mit **CMA-ES**, dem Suchverfahren aus [Kapitel 14](14-lehrer.md): Es probiert eine
Gruppe von Kandidaten aus, rückt zu den besseren hin und probiert in deren Nähe weiter.

> **Stell dir vor …** zwölf Gitarren mit je sechs Wirbeln, jede etwas anders gestimmt. CMA-ES hört
> hin, welche am besten klingen, und stimmt die nächsten zwölf ähnlich wie die Gewinner.

Entscheidend ist danach der **Prüf-Fehler**: der Abstand auf der Messung, die man beiseitegelegt
hat. Nur er zeigt, ob das Profil den Servo wirklich erklärt oder die Kurven bloß auswendig kann –
wie eine Klassenarbeit mit neuen Aufgaben statt der geübten. Darum sollte die Prüfmessung ein anderes
Signal und eine andere Last haben.

> **Wusstest du?** Kern prüft das Verfahren mit einem absichtlich „verstellten“ Servo: Er zieht
> schwächer, reibt stärker und reagiert später als das Startprofil. Nach der Anpassung trifft die
> Simulation auch die Prüfmessung fast genau – der Rest liegt unter einem Fünftel Grad.

Ohne echten Servo kann man alle Schritte einmal mit Beispiel-Messungen aus der Simulation
ausprobieren. Die Befehle dafür stehen im blauen Buch, Kapitel 16 und 17.

## 16.3 Abgleich: der ganze Roboter

Der Prüfstand vermisst einen Servo. Der **Abgleich** prüft den ganzen Roboter: Man nimmt eine
Aufzeichnung der echten Ente – was sie in jedem Moment gemessen und welche Befehle ihr Netz gegeben
hat – und spielt dieselben Befehle in der Simulation nach. Dann vergleicht man, was die echte und
was die simulierte Ente dabei gemessen haben.

> **Stell dir vor …** zwei Musiker spielen nach denselben Noten, einer auf der Bühne, einer im
> Proberaum. Du legst die beiden Aufnahmen übereinander. Wo sie auseinanderlaufen, stimmt etwas am
> Instrument im Proberaum nicht.

Die simulierte Ente startet still in ihrer Grundhaltung, ohne Variation, mit den Servos aus dem
Profil, und bekommt 50-mal pro Sekunde die Befehle aus der Aufzeichnung. Verglichen wird, was ihre
Sensoren melden – wie sich der Körper dreht, wo „unten“ ist, wie die Gelenke stehen und wie schnell
sie sich bewegen –, für jedes Gelenk einzeln.

Bleibt ein Gelenk auffällig, fehlt dort etwas im Modell. Ein Kandidat ist **Getriebespiel**: ein
bisschen Wackeln im Getriebe, bevor die Zahnräder greifen. Kerns Servomodell kennt das noch nicht.

Das Schöne: Spielt man eine Aufzeichnung **aus der Simulation** mit denselben Servos wieder ab, ist
der Abstand **genau null**, bis in die letzte Stelle, weil die Simulation deterministisch ist. Mit
anderen Servos ergibt sich ein messbarer Abstand. Die Simulation würfelt beim Abgleich also nichts
aus; Zufall scheidet als Ursache aus. Darum ist jeder Unterschied zu einer echten Aufzeichnung ein
echter Fehler im Modell.

Wie der echte Roboter eine solche Aufzeichnung schreibt, beschreibt dieses Buch nicht; den Befehl
für den Abgleich nennt das blaue Buch, Kapitel 16.

> **Probier's aus!** Wie viel kleine Unterschiede ausmachen, siehst du im Studio. Öffne
> **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck) und klick die Ente
> an. Im **Details**-Panel steht unter
> **Spieler-Steuerung › Läufer** (Player Controller › Walker) das Feld
> **Aktionen zu Servo-Zielen** (Actions to servo goals). Lass es auf
> **Wie trainiert (Faktor 1, kein Filter)** (As trained (scale 1, no filter)), starte **Spielen**
> (Play), übernimm die Ente mit **F8** und kick mit der **3**. Den Ball musst du nicht treffen –
> schau nur, ob die Ente nach dem Kick stehen bleibt. Stoppe mit **Esc**, stell das Feld auf
> **Wie der Daemon des Roboters (Faktor 0,9, Tiefpass 0,5 / 0,7)**
> (As the robot's daemon (scale 0.9, low-pass 0.5 / 0.7)) und kick noch einmal. Diese Einstellung schwächt und glättet die Befehle so, wie der
> echte Roboter seine Befehle bekommt – und nun kippen die Kicks um. Das ist noch kein Abgleich,
> aber es zeigt, warum man ihn braucht: Dasselbe Netz, nur ein etwas anderer Weg zum Motor, und das
> Ergebnis ist ein anderes. Solche Unterschiede zwischen Simulation und Roboter soll der Abgleich
> aufspüren.

## 16.4 Training mit dem eigenen Profil

Hat man seine Servos vermessen, trainiert man neue Netze mit genau diesem Profil, samt seinen
Streuungen. Bei jeder Episode bekommen die Enten Servos, die innerhalb der Streuung leicht
voneinander abweichen. So lernt das Netz, mit jedem Exemplar zurechtzukommen. Nur bei der Prüfung
am Ende – auf einer ungestörten Ente ohne Variation – gilt das Profil exakt.

> **Für Neugierige:** Das eigene Profil wirkt auf das Training der Bewegungen, etwa des Sprungs. Die
> Spielschicht übt derzeit immer mit dem mitgelieferten Standardprofil. Der Befehl dafür steht im
> blauen Buch, Kapitel 16.

## 16.5 Variation: üben unter wechselnden Bedingungen

Was man nicht gemessen hat oder was von Ente zu Ente schwankt, lässt das Training absichtlich
schwanken. Jede Episode bekommt etwas andere Motoren, einen etwas anderen Boden, etwas
unzuverlässigere Sensoren. Ein Netz, das in all diesen Welten zurechtkommt, kommt auch mit der echten
zurecht – die ist dann nur eine weitere Variante. Fachleute nennen das **Domänen-Randomisierung**.

> **Stell dir vor …** du übst Tischtennis mal mit schwerem, mal mit leichtem Ball, auf rutschigem
> und auf stumpfem Boden. Am Turniertag ist dir egal, wie der Saal ist.

| Was unsicher ist | Wie das Training es abdeckt |
|---|---|
| jeder Servo ein Einzelstück | jeder Servo etwas anders, innerhalb seiner Streuung |
| Verzögerung auf dem Datenkabel | Befehle kommen mal früher, mal später an |
| Bau (Akku, Kabel …) | Rumpf mal etwas schwerer, Schwerpunkt etwas verschoben |
| Boden | mal glatter, mal griffiger |
| Lage- und Gelenksensoren | rauschen (zittern zufällig), hinken nach, sind leicht verstellt |
| Störungen | ab und zu ein zufälliger Schubs |
| Kamera (sucht Ball und Socke) | ungenaue Position, manchmal sieht sie gar nichts |
| Spielzeug | Bälle mal größer, mal kleiner, mal schwerer |

Ein Trick dabei: Der **Kritiker** – das zweite Netz, das beim Training nur bewertet, wie gut eine
Lage ist – sieht immer die saubere Wahrheit. Nur das handelnde Netz, das später auf den Roboter
kommt, muss mit verrauschten Sensoren klarkommen. Der Kritiker bleibt beim Training, er darf also
„schummeln“.

Die genauen Bereiche stehen im blauen Buch, Kapitel 14 und 16.

> **Für Neugierige:** Getriebespiel variiert Pollen Robotics in seinem eigenen Training. Darum kommt
> Pollens Lauf- und Stehnetz velstand mit wackligen Getrieben zurecht. Kerns eigene Netze lernen
> bisher ohne.

> **Probier's aus!** Starte **Spielen** (Play) im Micro-Duck-Level, ohne die Ente zu übernehmen. Du schaust zu und
> kannst Körper mit der linken Maustaste greifen. Greif den Ball, zieh ihn zur laufenden Ente und
> lass ihn los, sodass er sie anstößt. Schau, wie sie darauf reagiert. Zufällige Schubse wie dieser
> gehören zu dem, was im Training variiert wird – damit ein Stoß die Ente nicht überrascht.

## 16.6 In der Physik messen statt eintragen

Manche Zahlen ergeben sich aus der Physik. Würde man sie fest hineinschreiben, stimmten sie nach der
nächsten Änderung an Reibung, Modell oder Servo nicht mehr. Also misst Kern sie in der Simulation,
so wie man sie am echten Roboter messen würde.

> **Stell dir vor …** ein Kochrezept sagt „20 Minuten backen“. Mit einem neuen Ofen stimmt das nicht
> mehr. Besser ist „backen, bis der Zahnstocher trocken herauskommt“ – das gilt in jedem Ofen.

- **Die Bissposition:** Wo muss die Socke liegen, damit die Ente sie beim Aufheben greift und
  hält? Der Lehrer probiert vor dem ersten Training 99 Stellen aus. Er nimmt die Stelle, die
  zusammen mit ihren Nachbarstellen am zuverlässigsten klappt – also auch dann noch, wenn die Socke
  ein wenig daneben liegt.
- **„Hält etwas im Schnabel“:** Der Schnabel-Servo ist auf „zu“ gestellt und steht trotzdem ein
  Stück offen – dann steckt etwas dazwischen.
- **Stillstand:** Die Spielschicht bekommt laufend mitgeteilt, wie lange die Ente schon
  still steht – das hilft ihr bei der Entscheidung, was sie als Nächstes tut. Als „steht“ gilt, was
  die Beine tun: beide Füße am Boden. „Es gibt keinen Laufbefehl“ wäre die schlechtere Regel: Auf
  einen ganz kleinen Laufbefehl setzt die Ente manchmal gar nicht zum Schritt an. Sie steht dann,
  obwohl ein Befehl da ist – und wo diese Schwelle liegt, bestimmt die Physik.
- **Ausnahme, der Nachlauf nach „Stopp“:** Wie weit die Ente beim Anhalten noch geht, wurde einmal
  gemessen und dann fest eingetragen. Nach einer Änderung der Physik muss man neu messen.

Fest eingetragen ist dagegen die Reibung zwischen Materialien, etwa Stoff auf Holzboden. Diese Werte
stammen aus typischen Tabellen und warten auf eine Messung am echten Material.

## 16.7 Grenzen und nächste Schritte

Ehrlich gesagt: Bisher ist alles nur mit Messungen **aus der Simulation** geprüft; echte Messungen
fehlen noch. Sobald es sie gibt, funktionieren dieselben Werkzeuge mit ihnen. Außerdem:

- **Getriebespiel** fehlt. Zeigt der Abgleich, dass es gebraucht wird, kommt es ins Servomodell.
- **Ein bekannter Wackler:** Nach dem Kick mit rechts kippt die Ente in Kern manchmal um. Mögliche
  Gründe sind Wackeln im Getriebe, Füße, die im Computer etwas zu weich auf dem Boden aufsetzen,
  oder die Fußreibung – ein typischer Fall für den Abgleich mit einer echten Aufzeichnung.
- **Lagesensor und Kamera** haben noch keine eigenen, gemessenen Profile; ihr Rauschen ist bisher
  eine Annahme des Trainings.

Weitere kleine Vereinfachungen, die sich im Beispiel-Level nicht auswirken, beschreibt das blaue
Buch, Kapitel 16.

## Das nimmst du mit

- Drei Wege zum echten Roboter: Gerätemodelle vermessen, mit Aufzeichnungen abgleichen, unter
  Variation üben.
- Ein Servo ist ein Profil: Feder mit Kraftgrenze, Stoßdämpfer, Reibung, Schwungrad und Verzögerung,
  dazu ehrliche Streuungen.
- Der Prüfstand vermisst einen Servo; der Computer stimmt das Profil darauf ab. Zählen tut der
  Fehler auf einer Messung, die beim Anpassen nicht benutzt wurde.
- Prüfstand, Training und Spiel rechnen mit denselben Einstellungen; sonst gälte ein Profil nur an
  einer Stelle.
- Der Abgleich zeigt Gelenk für Gelenk, wo Simulation und Roboter auseinanderlaufen.
- Weil die Simulation immer exakt gleich rechnet, ist jeder Unterschied ein echter Modellfehler.
- Was sich messen lässt, misst Kern, statt es einzutragen.

## Knobelecke

1. Warum reicht es nicht, nur den Fehler auf den Messungen anzuschauen, an die das Profil angepasst
   wurde?
2. Ein Servo ist sehr gut vermessen. Sollte seine Streuung groß oder klein sein?
3. Eine Aufzeichnung aus der Simulation wird mit denselben Servos abgeglichen. Welcher Abstand kommt
   heraus, und warum?
4. Warum ist es riskant, die Bissposition als feste Zahl in den Code zu schreiben?

### Lösungen

1. Weil das Profil die Kurven sonst nur auswendig lernen könnte. Erst eine neue Messung zeigt, ob es
   den Servo wirklich erklärt – wie eine Klassenarbeit mit neuen Aufgaben.
2. Klein. Große Streuungen sind für Werte, die man nicht genau kennt.
3. Genau null, bis in die letzte Stelle, weil die Simulation bei gleichen Eingaben immer haargenau
   dasselbe rechnet.
4. Nach der nächsten Änderung an Reibung, Modell oder Servo stimmt sie nicht mehr. Gemessen passt
   sie immer zur aktuellen Physik.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
