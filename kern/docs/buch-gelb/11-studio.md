# 11 Kern Studio: Deine Werkstatt im Browser

> **Dein Level:** Jetzt wird gebaut! Kern Studio ist der Editor, in dem du Welten zusammensetzt,
> Physik ausprobierst, Spiele testest und als eigene Datei verschickst. Du lernst hier nicht jede
> Taste, sondern die Ideen, nach denen ein Editor funktioniert – am besten mit offenem Studio.

> **Wörter, die du brauchst:**
> - **Editor** – ein Programm zum Bauen und Bearbeiten, hier: von Spielwelten.
> - **Level** – eine ganze Spielwelt, gespeichert als eine Datei.
> - **Actor** – ein einzelnes Ding im Level: ein Würfel, eine Lampe, ein Auto, die Spielfigur.
> - **Komponente** – ein Baustein, den ein Actor trägt und der ihm eine Fähigkeit gibt.
> - **Viewport** – ein Fenster, durch das du mit einer Kamera in die 3D-Welt schaust.
> - **Starrkörper** (Rigid Body) – die Komponente, die ein Ding zum echten Physik-Körper macht: Es
>   fällt und lässt sich schubsen.
> - **HUD** – die Anzeige über dem Spielbild: Punkte, Zeit, Meldungen.
> - **WebAssembly** – eine schnelle Maschinensprache, die jeder Browser ausführen kann. Darin läuft
>   Kern.

Kern Studio ist wie die großen Profi-Editoren aufgebaut, zum Beispiel die Unreal Engine. Die
wichtigste Idee: **Was du im Editor siehst, ist schon das Spiel.** Der ganze Editor benutzt genau
dasselbe Programm wie dein fertiges, exportiertes Spiel. Was im Viewport fällt, rollt und leuchtet,
tut es später genauso (11.16). Wie daraus ein ganzes Spiel wird, zeigt
[Kapitel 12](12-eigene-spiele.md).

Dieses Kapitel geht in der Reihenfolge vor, in der man einen Editor lernt: erst verstehen, woraus eine
Welt besteht, dann schauen, auswählen und bewegen, dann Fehler rückgängig machen, testen, Regeln und
Logik hinzufügen und am Ende speichern und verschicken.

Studio zeigt seine Oberfläche auf Deutsch oder Englisch; ohne Vorgabe folgt es der Sprache des
Browsers. Dieses Buch nennt die deutschen Beschriftungen, in Klammern die englischen. Der Knopf
**DE | EN** unten rechts in der Statusleiste wechselt die Sprache; die Seite lädt neu, und dein Level
bleibt dabei erhalten.

> **Für Profis:** Die Sprache lässt sich auch in der Adresse festlegen: `?lang=de` oder `?lang=en`
> hinten anhängen. Studio merkt sich das auf diesem Gerät.

---

## 11.1 Studio starten

Studio läuft im Browser, auch am Handy und Tablet. Du öffnest es über https://kernengine.de/. Beim
allerersten Start ist **Orb Run** geladen: eine Zufallsstadt mit Verkehr, einem Hovercar, 24
Energie-Orbs und einem Tag, der in die Nacht übergeht. Danach öffnet Studio immer das Level, an dem du
zuletzt gearbeitet hast, und bietet **Neu anfangen** (Start fresh) für einen Neuanfang mit Orb Run an. Gab es
ungespeicherte Änderungen, fragt es, ob es sie wiederherstellen soll (11.13).

> **Mission:** Öffne Kern Studio, warte auf Orb Run und drück **Alt+P**. Fahr mit W A S D zu drei
> Orbs. **Esc** bringt dich zurück in den Editor. Du hast gerade im Editor gespielt – ohne etwas zu
> exportieren.

**Als App:** In Chrome und Edge zeigt die Titelleiste **⤓ Installieren** (⤓ Install), auf iPhone und iPad erklärt
derselbe Knopf, wie man Studio auf den Home-Bildschirm legt. Installiert startet Studio im Vollbild,
funktioniert auch offline und meldet sich, wenn eine neue Version da ist.

> **Für Profis:** Studio gibt es auch als eine einzige HTML-Datei, die ganz ohne Server läuft, und
> als Entwickler-Version zum Arbeiten am Studio selbst. Wie man beide baut, steht im blauen Buch,
> Kapitel 11.

---

## 11.2 Die Werkstatt im Überblick

Jeder große Editor teilt den Bildschirm in feste Bereiche, damit du immer weißt, wo was steht: in der
Mitte die Welt, am Rand die Werkzeuge und Listen. Wie eine Werkbank mit Werkzeugwand.

![Ausgewählter Actor mit Verschieben-Gizmo und seinen Komponenten in Details](../buch/bilder/studio-selected.png)

| Bereich | Wo | Wozu |
|---|---|---|
| Titelleiste | oben | Menüs, Levelname (Punkt = ungespeichert) |
| Toolbar | darunter | Knöpfe für Werkzeuge, Raster, Spielen, Logik, Export |
| Viewports | Mitte | 1, 2 oder 4 Blicke in die Welt |
| Outliner | rechts oben | Liste aller Actors |
| Details / **Welteinstellungen** (World Settings) | rechts unten | Eigenschaften der Auswahl bzw. der Welt |
| **Inhaltsablage** (Content Drawer) | unten, klappbar | Bausteine zum Platzieren, Bilder, Töne, Log |
| Statusleiste | ganz unten | Konsole, Meldungen, Zahlen, Version |

Die Trennlinien kannst du ziehen; Studio merkt sich die Größen.

**Die Menüs** sind so sortiert wie in vielen Programmen: **Datei** (File) zum Neu-Anlegen, Öffnen,
Speichern, Exportieren und mit den Beispiel-Levels; **Bearbeiten** (Edit) zum Rückgängigmachen,
Kopieren, Löschen und für die Einstellungen; **Fenster** (Window) für Ansichten und was eingeblendet
wird; **Spielen** (Play) zum Spielen, Simulieren und für Mehrspieler; dazu **Hilfe** (Help). Was
gerade nichts tun kann, ist grau. Neben vielen Einträgen steht das Tastenkürzel. Die wichtigsten
Tasten findest du in [Kapitel 17](17-referenz.md), alle Menüeinträge im blauen Buch, Kapitel 11.
**F1** zeigt die wichtigsten Tasten auch direkt in Studio.

**Datei › Neues Level...** (File › New Level...) bietet sechs Vorlagen (Doppelklick erstellt sofort):

| Vorlage | Was drin ist |
|---|---|
| Orb Run | Nachtstadt, Hovercar, 24 Orbs |
| Physics Lab | alle Arten von Gelenken: Scharnier, Schiene, Rad, Seilbrücke, Motor, Kistenpyramide |
| Shooting Range | Soldat, Zielscheiben, Deckung, mehrspielerfähig |
| Micro Duck | der Laufroboter von Pollen Robotics |
| **Basis** (Basic) | Boden, ein Würfel, Sonne und Himmel |
| **Leer** (Empty) | gar nichts |

Dieselben Levels findest du unter **Datei › Beispiel-Levels** (File › Example Levels). Für die
Missionen dieses Kapitels ist **Basis** dein Übungsplatz.

---

## 11.3 Ein Level ist ein Baukasten: Actors und Komponenten

Das wichtigste Prinzip des Editors: Eine Welt besteht aus **Actors**, und jeder Actor ist aus
**Komponenten** zusammengesteckt. Eine Komponente gibt ihm genau eine Fähigkeit: „sieht aus wie ein
Würfel“, „fällt herunter“, „leuchtet“, „gibt Punkte“, „macht Musik“. Was ein Ding **kann**, hängt nur
davon ab, welche Komponenten es trägt.

> **So kannst du es dir vorstellen:** ein Baukasten. Ein Actor ist eine Grundplatte. Steckst du Räder drauf,
> fährt sie, steckst du eine Lampe drauf, leuchtet sie. Dieselben Steine passen auf jede Platte.

Darum braucht Kern keine eigene Sorte „Kiste“, „Tür“ oder „Auto“. Eine Kiste ist ein Actor mit Form
und Physik-Körper, eine Tür hat dazu ein Gelenk, ein Orb hat eine leuchtende Kugelform und „gibt Punkte“.

| Komponente | Wozu? |
|---|---|
| **Statisches Mesh** (Static Mesh) | die Form und das Aussehen: Würfel, Kugel, Rampe … |
| **Punktlicht** (Point Light) | eine Lampe, die in alle Richtungen leuchtet |
| **Starrkörper** (Rigid Body) | macht ein Ding zum Physik-Körper: Es fällt und schubst |
| **Kollider** (Collider) | die unsichtbare Hülle, an der Dinge zusammenstoßen |
| **Gelenk** (Constraint) | ein Gelenk zu einem anderen Ding: Scharnier, Seil, Motor |
| **Spieler-Steuerung** (Player Controller) | macht das Ding zur Spielfigur, die du steuerst |
| **Waffe** (Weapon), **Gesundheit** (Health) | Waffe und Lebenspunkte für den Soldaten |
| **Pickup** | etwas zum Einsammeln, das Punkte gibt |
| **Trigger** | eine Zone, die etwas auslöst: Ziel, Gefahr, Sprungfeld, Teleporter … |
| **Rotator**, **Beweger** (Mover) | dreht sich von selbst oder fährt hin und her |
| **Prozedurale Stadt** (Procedural City) | baut eine ganze Zufallsstadt |
| **Audioquelle** (Audio Source) | spielt einen Ton im Raum ab |

Was die einzelnen Felder genau bewirken, erklären [Kapitel 6](06-physik.md) (Physik),
[Kapitel 7](07-audio.md) (Ton) und [Kapitel 8](08-spiel-laufzeit.md) (Spielregeln). Alle Feldnamen
stehen im blauen Buch, Kapitel 11.

**Was sonst noch zum Level gehört:** die **Welteinstellungen** (World Settings) mit den Regeln für die ganze Welt (11.9), Bilder,
Töne, Logik und die Kameraposition des Editors. Gemessen wird in Metern, Sekunden und Kilogramm, **Z
zeigt nach oben**. Eine Drehung steht als drei Winkel, wie bei einem Flugzeug: Rollen (zur Seite
kippen), Nicken (Nase hoch oder runter) und Gieren (nach links oder rechts drehen).

### Woher die Bausteine kommen

Fertige Actors liegen in der **Inhaltsablage** (Content Drawer, Strg+Leertaste), sortiert in
Gruppen wie in einem Werkzeugkasten mit Fächern: **Spieler** (Players) mit Spielfiguren wie
**Rollende Kugel** (Rolling Ball), **Spielablauf** (Gameplay) mit Spielelementen, etwa einem Orb,
**Formen** (Shapes), etwa einem Würfel, **Physik** (Physics) mit Physik-Dingen, etwa einer
Physikkiste, **Lichter** (Lights) mit Lampen, **Audio** mit Tönen und **Welt** (World) mit der
Zufallsstadt.

- **Klick** setzt den Baustein dorthin, wohin die Kamera blickt.
- **Ziehen** in den Viewport setzt ihn genau an die Stelle, wo du loslässt.
- Oder: **+ Hinzufügen** (+ Add) in der Toolbar oder Rechtsklick › **Actor platzieren**
  (Place Actor).

Ein neuer Actor ist sofort ausgewählt. Etwas aus **Spieler** wird automatisch deine Spielfigur.

Eigene Bilder und Töne holst du dir über die Tabs **Texturen** (Textures) und **Sounds** in der
Inhaltsablage (mehr dazu in 11.13). Der Tab **Ausgabeprotokoll** (Output Log) ist das Logbuch mit
allen Meldungen von Studio.

### Outliner und Details: Liste und Steckbrief

Der **Outliner** ist die Liste aller Actors deiner Welt, mit Suchfeld, Ordnern und einem Auge zum
Ausblenden. Ein Klick wählt aus, ein Doppelklick holt den Actor ins Bild, F2 benennt um. Die
Spielfigur trägt das Schild **SPIELER** (PLAYER).

**Details** ist der Steckbrief des ausgewählten Actors: oben Name und Lage (**Transformation**
(Transform) mit **Position** (Location), **Drehung** (Rotation) und **Skalierung** (Scale), jeweils
drei Zahlen X, Y, Z wie im Koordinatensystem), darunter je Komponente ein Abschnitt. Mit
**+ Hinzufügen** steckst du eine Komponente dazu, der Papierkorb nimmt sie weg. Zahlen kannst
du tippen (sogar Rechnungen wie `2*3`), mit gedrückter Maustaste auf dem Feldnamen nach links
oder rechts ziehen oder mit den Pfeiltasten ändern.

Studio prüft jeden Wert, bevor er ins Level kommt. Eine Größe von 0 oder ein Wert, der nicht passt,
landet nie im Level – so kann ein Tippfehler deine Welt nicht kaputt machen.

> **Mission:** Öffne **Datei › Beispiel-Levels › Basis** (File › Example Levels › Basic). Klick
> den Würfel „Cube“: Details zeigt ein **Statisches Mesh**, aber keinen Starrkörper. Platziere aus
> der Inhaltsablage (Gruppe **Physik**) einen **Physik-Würfel** (Physics Cube), indem du ihn neben den
> Würfel in den Viewport ziehst: Er hat zusätzlich einen **Starrkörper**. Setz bei beiden unter
> **Transformation** die **Position** Z auf 5 und drück **Alt+S**. Was fällt, was schwebt? **Esc**
> stoppt – und beide sind wieder oben. Warum, erfährst du in 11.8.

Der Würfel ohne Starrkörper ist für die Physik trotzdem da. Hat ein Ding eine Form mit Kollision,
aber keinen Starrkörper, gibt Studio ihm beim Testen einen unsichtbaren **festen** Körper: Er fällt nicht,
aber andere Dinge stoßen an ihm an, wie an einer Wand. Darum kann die Physikkiste auf dem Würfel
landen und deine Spielfigur gegen ihn laufen.

---

## 11.4 In die Welt schauen: Viewports und Kamera

Ein **Viewport** ist ein Fenster mit einer eigenen Kamera. Bis zu vier gleichzeitig zeigen dieselbe
Welt von verschiedenen Seiten. Ein Klick macht einen Viewport **aktiv**; dorthin gehen deine Tasten.

Es gibt zwei Arten von Blick:

- **Perspektive**: wie mit deinen Augen. Was weiter weg ist, wird kleiner.
- **Orthografisch** (im Menü **Oben** (Top), **Vorne** (Front), **Links** (Left) …): flach, ohne Perspektive, wie ein Bauplan. Gleich große
  Dinge sind überall gleich groß. Damit setzt man Dinge genau nebeneinander.

Unten links zeigt ein **Achsenkreuz** die Richtungen: X rot, Y grün, Z blau – und Z zeigt nach oben.

Jeder Viewport hat oben eine Leiste. Im **Ansichtsmodus** (View Mode) wählst du, wie die Welt gemalt
wird: **Beleuchtet** (Lit) mit Licht und Schatten, **Unbeleuchtet** (Unlit) nur mit Farben, ohne
Licht, **Drahtgitter** (Wireframe) nur als Drahtgitter aus Dreiecken oder
**Spieler-Kollision** (Player Collision): abgedunkelt, darüber die Hüllen, an denen Dinge anstoßen. Unter **Anzeigen** (Show) schaltest
du Hilfen ein und aus, etwa das Bodenraster, Symbole für Lampen oder die Physik-Hüllen. **G** blendet
alle Hilfen auf einmal aus und zeigt die Welt wie im Spiel.

### Fliegen wie in einem Spiel

Die Kamera steuerst du wie eine Spielfigur:

| Eingabe | Wirkung |
|---|---|
| rechte Maustaste halten + W A S D | fliegen, Maus schaut |
| … + Q / E | runter / hoch |
| … + Mausrad | schneller / langsamer (Stufe 1–8) |
| Alt + linke Maustaste | um die Auswahl kreisen |
| F | Auswahl ins Bild holen |
| Strg+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

**Mit Fingern:** ein Finger kreist, zwei Finger zoomen und verschieben, Tippen wählt aus. Was unter
deinen Fingern liegt, bleibt beim Zoomen unter ihnen.

> **Für Profis:** Wie schnell gezoomt wird, hängt davon ab, wie weit der Punkt unter den Fingern weg
> ist: Nahes zoomt in kleinen Schritten, Fernes in großen. Und für W A S D zählt die **Lage** der
> Taste, nicht was draufsteht – auf einer französischen Tastatur steuern die vier Tasten an derselben
> Stelle genauso. Kürzel wie Strg+Z gehen dagegen nach der Beschriftung.

> **Mission:** Flieg in Orb Run mit gehaltener rechter Maustaste über die Stadt und dreh dabei am
> Mausrad, bis du richtig schnell bist. Lass die Maustaste los und speichere mit **Strg+1** einen
> Blick auf ein Hochhaus. Flieg weg, lass los und drück **1** – du bist sofort zurück. Ohne Tastatur,
> oder falls dein Browser bei Strg+1 zu einem anderen Tab springt: **≡ › Lesezeichen** (≡ › Bookmarks)
> in der Leiste des Viewports, dort **Lesezeichen 1 setzen** (Set bookmark 1) und später
> **Zu Lesezeichen 1 springen** (Jump to bookmark 1).

Alle weiteren Kamera-Tasten stehen in [Kapitel 17](17-referenz.md).

---

## 11.5 Auswählen und Bewegen: Gizmo und Raster

Bevor du etwas veränderst, sagst du Studio, **was**: Du wählst aus. Ein Linksklick wählt, Umschalt
fügt hinzu, Strg schaltet einzeln an und aus, ein Klick ins Leere hebt alles auf. **Linkes Ziehen** zeichnet
immer ein Auswahlrechteck, auch wenn du auf einem Actor anfängst. Es bewegt nie etwas.

Bewegt wird **nur mit dem Gizmo**, dem bunten Anfasser am ausgewählten Actor. Diese Trennung ist
Absicht: Ein Klick zum Auswählen kann so nie aus Versehen etwas verschieben.

| Werkzeug (mit Taste) | Gizmo |
|---|---|
| **Auswählen (Q)** (Select (Q)) | keiner, nur auswählen |
| **Verschieben (W)** (Move (W)) | drei Pfeile, ziehen verschiebt |
| **Drehen (E)** (Rotate (E)) | drei Ringe, ziehen dreht |
| **Skalieren (R)** (Scale (R)) | drei Achsen, ziehen vergrößert |

Rot ist X, grün Y, blau Z; der Griff unter dem Zeiger leuchtet gelb. Beim Ziehen steht daneben, wie
weit du schiebst (in m), drehst (in °) oder vergrößerst. Hast du mehrere Actors gewählt, bewegen sich
alle; der Gizmo sitzt am zuletzt gewählten.

**Welt- oder eigene Achsen?** Ein Knopf in der Toolbar wechselt zwischen zwei Bildern. Zeigt er einen
Globus (Tooltip **Weltkoordinaten** (World coordinates)), richtet er den Gizmo nach der **Welt** aus:
Der rote Pfeil zeigt immer gleich, wie „Norden“ auf einer Karte. Zeigt er einen Würfel
(**Lokale Koordinaten** (Local coordinates)), richtet er ihn nach dem **Actor selbst** aus: Ist der gedreht, dreht der Gizmo mit, wie „vor mir“ für einen Menschen, der sich
umdreht.

**Alt + Ziehen** zieht eine **Kopie** heraus, das Original bleibt stehen, und ausgewählt ist danach
die Kopie. **Esc** während des Ziehens bricht ab. Kopierst du Actors mit Strg+C, landen sie als Text
in der Zwischenablage – du kannst sie darum mit Strg+V sogar in einem anderen Studio-Tab einfügen.

### Snapping: Karopapier für die Welt

> **So kannst du es dir vorstellen:** Snapping ist Karopapier im Matheheft. Wenn jede Ecke auf einem
> Kästchen liegt, passt alles genau aneinander.

Beim **Snapping** (Einrasten) springt ein Wert immer auf das **nächstgelegene Vielfache** einer
Schrittweite:

| Raster | Standard | Wirkung |
|---|---|---|
| **Am Raster ausrichten** (Grid snap) | an, 0,5 m | Verschieben rastet ein |
| **Drehung einrasten** (Rotation snap) | an, 15° | Drehen in 15°-Schritten |
| **Skalierung einrasten** (Scale snap) | aus, 0,25 | Vergrößern in Schritten |

**Beispiel:** Raster 0,5 m. Du ziehst einen Würfel auf x = 1,3 m. Die Vielfachen in der Nähe sind
1,0 und 1,5. 1,5 ist näher, also landet er bei **x = 1,5 m**.

In Weltachsen rastet der Actor auf diese festen Positionen ein. In seinen eigenen Achsen bewegt er
sich in Schritten von 0,5 m, ausgehend von da, wo er war. Neue Actors landen ebenfalls auf dem Raster.
Die Schrittweiten wählst du in der Toolbar.

> **Mission:** Wähl im Level Basis den Würfel „Cube“. Drück **E** und zieh an einem Ring: Er
> dreht in 15°-Schritten. **Strg+Z** nimmt das zurück. Drück **W**, halt **Alt** und zieh den roten Pfeil 1 m
> – zwei Würfel! Ausgewählt ist jetzt die Kopie. Zieh sie (ohne Alt) am blauen Pfeil 0,5 m hoch. Wiederhol das mit der jeweils
> neuen Kopie, bis eine Treppe aus fünf Stufen steht.

---

## 11.6 Undo: zurück zum Spielstand

Fehler gehören zum Bauen. Darum kann **Strg+Z** jeden Schritt zurücknehmen und **Strg+Y** ihn
wiederholen. Spannend ist, **wie** Studio das macht.

> **So kannst du es dir vorstellen:** Wie Spielstände. Statt mühsam rückwärts zu laufen, lädst du
> einfach den alten Stand – und der ist garantiert genau wie damals.

Nach jedem Schritt speichert Studio eine Kopie des **ganzen Levels**. Undo lädt einfach die vorige
Kopie. Studio muss also nie ausrechnen, wie man eine Änderung rückgängig macht – und darum kann dabei
auch nichts schiefgehen. Es hält **500 Schritte**.

- Ein ganzer Ziehvorgang ist **ein** Schritt, auch wenn sich der Wert dabei hunderte Male ändert.
- Eine Kopie mit Alt samt Verschieben ist ebenfalls **ein** Schritt.
- Eine Änderung, die nichts ändert, ist **kein** Schritt.
- **Bearbeiten › Verlauf...** (Edit › History...) zeigt alle Schritte als Liste. Ein Klick springt
  zu diesem Stand.

> **Krass, aber wahr:** Ziehst du drei Sekunden an einem Regler, ändert sich der Wert hunderte Male –
> trotzdem nimmt **ein** Strg+Z alles zurück. Jeder Ziehvorgang bekommt eine Kennung, und alle
> Änderungen mit derselben Kennung werden ein Schritt.

> **Mission:** Öffne nach deiner Treppe **Bearbeiten › Verlauf...**. Klick den Schritt, mit dem du die erste
> Kopie gezogen hast: Die Treppe schrumpft auf zwei Würfel. Klick dann den letzten Schritt – alle fünf
> Stufen sind wieder da. Speichere die Treppe jetzt mit **Strg+S**, zum Beispiel als
> `treppe.klevel` (mehr zum Speichern in 11.13). Du brauchst sie in 11.14 wieder.

---

## 11.7 Dinge verbinden: Eltern, Kinder und Gelenke

Manche Dinge gehören zusammen. Studio kennt zwei Arten, sie zu verbinden.

**Eltern und Kinder.** Hängst du einen Actor an einen anderen, wird er sein **Kind**. Bewegt sich der
**Elternteil**, bewegt sich das Kind mit – wie eine Tasse in deiner Hand: Gehst du, geht die Tasse
mit. Zum Anhängen wählst du den künftigen Elternteil **als Letztes** und nimmst Rechtsklick ›
**Anhängen an …** (Attach to …), oder du ziehst im Outliner einen
Actor auf einen anderen. **Lösen** (Detach) löst ihn wieder. **Strg+G** legt einen leeren Actor in die Mitte der Auswahl und hängt alles daran. Ordner im Outliner
sind dagegen nur zum Aufräumen da; sie bewegen nichts.

> **Für Profis:** Hat ein Kind keinen eigenen **Starrkörper** (Rigid Body), verschweißt die Physik es mit dem Körper
> seines Elternteils. Beide bewegen sich dann wie ein einziges Stück ([Kapitel 6](06-physik.md)).

**Gelenke.** Ein Gelenk verbindet zwei Dinge **beweglich**: ein Scharnier wie bei einer Tür, eine
Schiene, ein Kugelgelenk wie deine Schulter, eine feste Verbindung, ein Seil, ein Rad oder einen
Motor. So geht es am schnellsten:

1. Wähle den Actor, der **hängen** soll.
2. Wähle mit Umschalt oder Strg den dazu, **an dem** er hängt – als Letztes.
3. Rechtsklick › **Mit Gelenk verbinden** (Connect with Constraint) und dort eine Gelenkart.

Studio gibt dem hängenden Actor einen Starrkörper, falls er keinen hat. Ein Seil (**Abstand / Seil**
(Distance / rope)) hält genau den Abstand, den die beiden beim Start haben.

> **Mission:** Öffne das Level Basis neu, damit wieder nur ein Würfel dasteht:
> **Datei › Beispiel-Levels › Basis** (File › Example Levels › Basic). Zieh den Würfel mit dem
> blauen Pfeil 4 m nach oben. Platziere eine **Physik-Kugel** (Physics Sphere) und schieb sie 3 m zur
> Seite auf etwa 2 m Höhe. Klick erst die Kugel, dann mit Strg den Würfel, Rechtsklick auf den Würfel ›
> **Mit Gelenk verbinden › Abstand / Seil**, **Alt+S**: ein Pendel! Der Würfel schwebt fest: Ohne
> Starrkörper bekommt er einen unsichtbaren festen Körper (11.3), der nicht fällt und das Seil hält.

---

## 11.8 Testen: Spielen und Simulieren

Ein Editor muss zeigen, ob die Welt funktioniert. Studio kennt dafür drei Zustände:

- *Bauen*: Du baust. Nichts bewegt sich von selbst.
- *Simulieren*: Physik und Spielregeln laufen, du schaust mit der Editor-Kamera zu und greifst ein.
- *Spielen*: Du spielst mit deiner Spielfigur.

Das wichtigste Prinzip: **Testen verändert dein Level nicht.** Solange etwas läuft, ist das Level
gesperrt. Was in der Simulation passiert, verschwindet beim Stoppen, und alles steht wieder da, wo du es
hingebaut hast. Wie ein Probelauf mit Spielzeug, das danach von selbst zurück in die Ausgangslage
springt. Deshalb waren in der Mission aus 11.3 nach **Esc** beide Würfel wieder oben.

**Spielen** (Play) mit Alt+P startet das Spiel im aktiven Viewport: Die Kamera folgt deiner
Spielfigur, das HUD zeigt Punkte, Zeit und Tempo. Am Computer fängt Studio die Maus; **Esc** beendet,
**Strg+R** startet neu. Bei Sieg oder Niederlage erscheint „Gewonnen“ (Victory) oder „Spiel vorbei“
(Game over) mit **Nochmal spielen** (Play again).

Zum Spielen braucht das Level einen Actor mit **Spieler-Steuerung** (Player Controller). Den bekommst
du, wenn du etwas aus der Gruppe **Spieler** (Players) platzierst oder im Details-Panel die
Komponente hinzufügst. Haben mehrere Actors eine, wählst du mit Rechtsklick ›
**Zum Spieler machen (beim Spielen steuern)** (Make Player (control it in Play)), welchen du steuerst. Fehlt er, simuliert Studio nur und meldet „Noch nichts zu
steuern“ (Nothing to control yet). Außerdem prüft Studio das Level vor dem Start: Fehler (etwa ein
Gelenk zu einem gelöschten Actor) verhindern den Start, Warnungen stehen nur im
**Ausgabeprotokoll** (Output Log).

**F8** (aussteigen) lässt die Spielfigur los: Das Spiel läuft weiter, und du fliegst frei herum wie
beim Bauen. Noch einmal **F8** übernimmt die Figur wieder.

![Simulieren in Orb Run mit eingeblendeten Kollisionsformen (Anzeigen › Kollisionsformen)](../buch/bilder/studio-simulate.png)

**Simulieren** (Simulate) mit Alt+S lässt die Welt ohne Spieler laufen. Jetzt kannst du Dinge
**greifen**: Linke Maustaste auf einen Physik-Körper, ziehen bewegt ihn, nach oben ziehen hebt ihn,
Loslassen **wirft** ihn. Unter **Anzeigen** (Show) machst du sichtbar, was die Physik sieht:
**Kollisionsformen** (Collision Shapes), also die Hüllen, **Gelenke** (Constraints) und
**Kontaktpunkte** (Contact Points), also wo sich Dinge berühren. **Pause** hält an, die Punkt-Taste
geht dann einen Schritt weiter.

Manchmal gefällt dir, was die Simulation gemacht hat – eine Kiste, die genau richtig schief liegt.
Dann wählst du sie aus und drückst **K**, kurz für
**Simulationsänderungen übernehmen** (Keep Simulation Changes): Ihre Lage wandert ins Level, die
Simulation endet. Das ist ein ganz normaler Undo-Schritt.

> **Für Profis:** Der Griff ist kein Teleport. Ein Motor-Gelenk mit einer Feder zieht den gegriffenen
> Punkt zum Zeiger, „kritisch gedämpft“, also ohne Nachschwingen. Darum stößt der Körper an und kippt
> wie jeder andere.

> **Mission:** Öffne **Physics Lab** und drück **Alt+S**. Pack die oberste Kiste der Pyramide mit der
> linken Maustaste, zieh sie hoch und lass sie auf die Pyramide fallen. Schalte in der Leiste des
> Viewports **Anzeigen › Kontaktpunkte** (Show › Contact Points) ein. Wähle mit einem kurzen Klick
> eine schief liegende Kiste und drück **K**: Das Simulieren endet, und die Kiste bleibt im Level so
> schief liegen. **Strg+Z** stellt sie zurück.

Mit **Beobachterkamera** (Observer camera) in den **Welteinstellungen** (World Settings) startet
**Spielen** mit freier Kamera: Das Spiel läuft, du schaust zu und kannst Dinge greifen wie beim
Simulieren. **F8** übernimmt die Spielfigur. Das Level Micro Duck nutzt das.

---

## 11.9 Die Regeln der Welt: Welteinstellungen und Konsole

Manche Einstellungen gehören nicht zu einem Ding, sondern zur **ganzen Welt**. Sie stehen im Tab
**Welteinstellungen** (World Settings):

- **Spiel** (Game): Wie gewinnt man? Alles einsammeln, ein Ziel erreichen, eine Zeit überleben oder
  eine Punktzahl schaffen. Dazu Zeitlimit, Spielfigur und **Kill-Z** (Kill Z): die Höhe, unter der
  die Figur als heruntergefallen gilt und zurückgesetzt wird.
- **Himmel und Licht** (Sky and light): Tageszeit, wie schnell der Tag vergeht, Sonne, Nebel,
  Wolken, Boden.
- **Physik** (Physics): die Schwerkraft und wie genau die Physik rechnet ([Kapitel 6](06-physik.md)).
- **Audio**: Gesamtlautstärke, Musik, Hintergrundklang, Hall.

Die genauen Felder und Bereiche stehen im blauen Buch, Kapitel 11.

**Die Konsole.** Unten links in der Statusleiste liegt das Feld **Cmd** (auch auf Deutsch so
beschriftet). Dort tippst du Befehle,
statt zu klicken. Pfeil nach oben holt den letzten Befehl zurück.

| Befehl | Wirkung |
|---|---|
| `spawn` und ein Name | Baustein platzieren |
| `gravity x y z` | Schwerkraft ändern (leer: Erde) |
| `tod` und eine Zahl von 0 bis 1 | Tageszeit |
| `play`, `simulate`, `stop` | wie die Knöpfe |
| `help` | alle Befehle anzeigen |

Die Schwerkraft hat eine Stärke **und eine Richtung**. Darum braucht `gravity` drei Zahlen: wie
stark sie in X-, Y- und Z-Richtung zieht. Z zeigt nach oben, also heißt `gravity 0 0 -9.81`: mit
9,81 nach unten, wie auf der Erde. Ein Plus bei Z würde alles an die Decke fallen lassen.

Bei `tod` (Tageszeit, englisch „time of day“) ist der Tag ein Kreis von 0 bis 1: **0** ist
Mitternacht, **0,25** Sonnenaufgang, **0,5** Mittag, **0,75** Sonnenuntergang. Im Befehl schreibst du
das Komma als Punkt, also `tod 0.5`.

> **Mission:** Öffne **Datei › Beispiel-Levels › Basis** (File › Example Levels › Basic), klick
> ins Feld **Cmd** und tipp `spawn physics_cube`. Setz in Details unter **Transformation** (Transform)
> bei **Position** (Location) Z auf 10. Tipp
> `gravity 0 0 -1.6` (etwa Mondschwerkraft nach unten, auf der Erde sind es 9,81) und drück
> **Alt+S**: Wie langsam fällt die Kiste? **Esc** stoppt, dann `gravity` ohne Zahlen – zurück auf der Erde.

---

## 11.10 Logik ohne Code: der Logik-Editor

Damit in deiner Welt etwas **passiert** – eine Tür geht auf, eine Zone gibt Punkte –, braucht sie
Logik. Studio hat dafür einen eigenen Editor, in dem du **Knoten** mit **Drähten** verbindest, statt
Code zu schreiben.

> **So kannst du es dir vorstellen:** Wie Scratch, nur liegen die Blöcke nicht untereinander, sondern
> sind mit Drähten verbunden. Ein Ereignis-Knoten wie „Begin Play“ (das Spiel startet) ist der
> „Wenn grüne Flagge angeklickt“-Block. Der Draht sagt, was danach passiert.

Jeder Actor und das Level selbst können einen solchen **Logik-Graphen** tragen. Du öffnest ihn mit
**Logik** (Logic) in der Toolbar (für das Level), mit **Logik** in Details oder per Rechtsklick ›
**Logik bearbeiten** (Edit Logic) für einen Actor; hat er noch keinen Graphen, heißt der Eintrag
**Logik hinzufügen (Blueprint)** (Add Logic (Blueprint)).

**Wichtig:** Der Logik-Editor ist nicht übersetzt. Seine Knöpfe, Knoten und Vorlagen heißen auch in
der deutschen Oberfläche englisch – genau wie die Meldungen im **Ausgabeprotokoll** (Output Log) und
die Befehle der Konsole. Ab hier siehst du also Deutsch und Englisch gemischt. Ein leerer Graph bietet Start-Knoten an: **Begin Play**, **On Interact**
(die Figur benutzt den Actor), **On Begin Overlap** (etwas betritt ihn) und **Event Tick** (bei jedem
Bild). Unter **Templates…** liegen fertige Beispiele:

| Vorlage | Was sie tut |
|---|---|
| Door that opens on Interact | Taste F öffnet und schließt, mit Ton |
| Score zone (overlap +10) | Betreten gibt 10 Punkte |
| Spawner (a cube every 3 s) | alle 3 s eine Physikkiste, höchstens 10 |
| Traffic light (state machine) | Ampel Grün, Gelb, Rot |
| Robot: walk and turn away with its ToF | die Micro Duck dreht vor Hindernissen ab |

> **Mission:** Öffne das Level **Basis** (Basic), klick **Logik** in der Toolbar, dann
> **Begin Play**. Der Knoten hat rechts einen Ausgang namens „then“, einen kleinen Pfeil. Zieh von
> dort einen Draht ins Leere, tipp „show message“, drück Enter und trag „Hallo Welt!“ ein. Zurück mit
> **Viewport**, eine **Rollende Kugel** (Rolling Ball) platzieren, **Spielen** (Play): Dein Text
> steht im HUD.

Beim Start des Spiels übersetzt Kern jeden Graphen in schnelle Maschinensprache. **Compile** heißt
„übersetzen“: Der Knopf **Compile** (F7) prüft vorher, ob dein Graph Fehler hat. Die Fehler stehen
dann im Feld **Compiler Results** („Übersetzer-Ergebnisse“) unter dem Graphen.

> **Vorsicht, Falle:** Ein Graph mit Fehlern hält das Spiel **nicht** auf. Kern schaltet beim Start
> nur diesen Graphen ab, meldet „Logic error: …“ im **Ausgabeprotokoll** (Output Log), und der Rest
> läuft weiter. Passiert
> „nichts“, schau ins Log – oder drück vorher **F7**, dann stehen die Fehler in den Compiler Results.

Alle Knoten, Variablen und Ampel-Schaltungen erklärt [Kapitel 9](09-logik.md). Wie man einen Graphen
als C++ herunterlädt, zeigt [Kapitel 12](12-eigene-spiele.md).

---

## 11.11 Ein Studio für jedes Gerät

Studio und deine Spiele laufen am Computer, am Tablet und am Handy. Das Prinzip: **Die Engine merkt
selbst, womit du gerade spielst**, und schaltet auf das zuletzt benutzte Gerät um. Ein Finger schaltet
auf Touch, ein Tastendruck auf Tastatur, ein Knopf am Gamepad aufs Gamepad. Eine Maus am Tablet behält
die Touch-Knöpfe.

![Spielen am Telefon im Hochformat: ⟵ ENDE, ❚❚ PAUSE und die Touch-Sticks](../buch/bilder/phone-play-390.png)

**Am Touchscreen** füllt das Spiel den ganzen Bildschirm. Oben links stehen **⟵ ENDE** (⟵ EXIT) und
**❚❚ PAUSE**. Unten liegen **zwei Sticks**: links bewegen, rechts schauen. Dazu kommen nur die Knöpfe,
die deine Spielfigur wirklich braucht – eine Rollende Kugel etwa nur **SPRUNG** (JUMP) und **BOOST**,
ein Soldat dazu Knöpfe zum Zielen, Nachladen und Ducken. Hebt sich der letzte Finger, lässt die
Engine alles los, damit die Figur nicht weiterläuft. Welche Figur welche Knöpfe bekommt, steht im
blauen Buch, Kapitel 11.

**Am Gamepad** (Xbox-Belegung) bewegt der linke Stick, der rechte schaut, A springt. Die ganze
Belegung steht im blauen Buch, Kapitel 11.

**Auf schmalen Bildschirmen** wird Studio selbst kompakt: Der Viewport bekommt die ganze Fläche.
Outliner, Details und Welteinstellungen öffnen sich über den Panel-Knopf (Tooltip
**Outliner und Details** (Outliner and Details)) als Blatt, am Handy von unten. Die Knöpfe zum
Spielen und Simulieren liegen in einem **Dock** rechts in der Toolbar, das immer stehen bleibt,
während der Rest der Toolbar seitwärts scrollt.

![Studio in einem 820 px breiten Fenster](../buch/bilder/studio-narrow.png)

![Telefon im Hochformat: das Dock mit Spielen, Simulieren und dem Panel-Knopf](../buch/bilder/phone-dock-390.png)

---

## 11.12 Die Micro Duck im Studio

Die Micro Duck ist der zweibeinige Roboter von Pollen Robotics. Im Studio läuft sie mit denselben
gelernten Programmen wie der echte Roboter. Wie das funktioniert, erklärt
[Kapitel 13](13-micro-duck.md); hier geht es nur um die Bedienung.

**Das Beispiel-Level Micro Duck** ist ein Zimmer in ihrem Maßstab mit Möbeln, Ball und zwei Socken.
**Spielen** (Play) startet mit der **Beobachterkamera** (Observer camera): Du schaust zu, wie die Ente selbstständig herumläuft und mit dem
Ball spielt, und kannst Dinge mit Maus oder Finger anheben. **F8** übernimmt die Ente.

**Ohne Eingabe** erkundet sie den Raum mit ihrem Abstandssensor und ihrer Kamera, weicht aus und steht
nach einem Sturz selbst wieder auf. Hebst du sie hoch, hält sie still; setzt du sie ab, läuft sie
weiter – wie der echte Roboter.

**Selbst steuern:** W und S laufen, A und D drehen, die Leertaste quakt, **7** springt, die Tasten 1
bis 6 lösen weitere Bewegungen aus wie Sitzen oder Kicken. Die ganze Liste steht in
[Kapitel 17](17-referenz.md). Oben rechts zeigt Studio, was die Ente sieht: ihre Kopfkamera und das
Gitter ihres Abstandssensors ([Kapitel 13](13-micro-duck.md)).

Jede Bewegung der Ente ist ein eigenes gelerntes Programm, ein **Netz**. Das Laufen ist fest
eingebaut, die übrigen Netze holt Studio aus dem Internet. Fehlt eins, bleibt die Ente stehen, und
Studio sagt, warum. Netze, die du selbst im Kern Lehrer trainierst, lädt Studio sofort
([Kapitel 13](13-micro-duck.md) und [Kapitel 14](14-lehrer.md)).

> **Mission:** Öffne das Level **Micro Duck**, drück **Spielen** und schau eine Weile zu. Dann **F8** und
> Taste **7**: Sie springt. Lass dann alle Tasten los und warte ein paar Sekunden: Deine Eingabe hat
> nur kurz Vorrang, danach entscheidet die Ente wieder selbst. Was macht sie als Nächstes?

---

## 11.13 Speichern: Datei, Autosave und Einstellungen

Ein Level ist **eine einzige Datei** mit der Endung `.klevel`. Darin steckt alles: Welt, Actors,
Logik und sogar deine Bilder und Töne. Du kannst sie also verschicken, und beim Empfänger ist nichts
verloren. Die genaue Form der Datei steht in [Kapitel 17](17-referenz.md).

**Speichern** (Save) mit Strg+S speichert. In Chrome und Edge schreibt Studio nach dem ersten Dialog
direkt in dieselbe Datei, in anderen Browsern kommt ein Download. **Speichern unter...** (Save As...)
fragt immer nach einem Namen, **Level öffnen...** (Open Level...) mit Strg+O öffnet ein Level. Ziehst du eine Datei auf die Seite, wird ein Level geöffnet, ein
Bild zur Textur und ein Ton zum Sound.

**Autosave:** Solange etwas ungespeichert ist, legt Studio kurz nach jeder Änderung eine Kopie im
Browser ab. Beim nächsten Start fragt es: **Wiederherstellen** (Restore) holt die Kopie zurück, **Schließen** (Dismiss) löscht sie.
Sobald du richtig speicherst, ist die Kopie nicht mehr nötig und verschwindet.

> **Vorsicht, Falle:** Der Autosave lebt **nur in diesem Browser**. Neuer Rechner oder gelöschte
> Browserdaten – weg. Echte Sicherheit gibt nur **Strg+S**.

**Updates kosten nie Arbeit.** Gibt es eine neue Studio-Version, lädt Studio sie sofort, solange du
baust. Läuft gerade ein Spiel, erscheint **Neue Version verfügbar** (New version available), und du
wählst **Aktualisieren** (Update) oder **Später** (Later); spätestens wenn du stoppst, aktualisiert
Studio von selbst. Beim Update reist dein Level – auch mit ungespeicherten Änderungen – über
den Autosave in die neue Version – wie ein Umzug, bei dem die Kisten zuerst sicher im Auto stehen.

**Einstellungen:** Unter **Bearbeiten › Einstellungen...** (Edit › Preferences...) stellst du Kameratempo, Mausempfindlichkeit und die
Raster ein. Studio merkt sich dabei **nur, was du selbst geändert hast**. Alles andere folgt den
Standardwerten der jeweiligen Studio-Version. Wird ein Standard in einer neuen Version besser, bekommst
du die Verbesserung automatisch – außer du hast diesen Wert bewusst selbst eingestellt.
**Bearbeiten › Einstellungen zurücksetzen...** (Edit › Reset Settings to Defaults...) setzt alles
zurück; deine Levels bleiben dabei unberührt.

---

## 11.14 Spiel exportieren

Am Ende soll dein Level ein Spiel werden, das andere spielen können. Wähl
**Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...) oder den Paket-Knopf in der
Toolbar: **Spieltitel** (Game title) prüfen, **Verpacken** (Package) klicken. Studio lädt eine Datei
`<Titel>.html` herunter und bietet **Spielen** (Play it) an, das sie in einem neuen Tab startet.

Die Datei ist ein **komplettes Spiel**: Engine, Level und alles, was zum Spielen gehört, stecken
darin. Sie braucht weder Server noch Internet und startet sogar per Doppelklick sofort im Spiel, ohne
Editor. Gesteuert wird wie beim Spielen in Studio, am Handy mit Touch-Sticks. Online spielen geht auch
([Kapitel 10](10-netzwerk.md)).

Zwei Dinge zur Micro Duck: Die nachgeladenen Netze der Ente stecken **nicht** in der Datei – Laufen
geht, für alles andere müssen ihre Ordner neben der Datei liegen. Und das Aussehen der Ente stammt von
Pollen Robotics und darf nur **nicht kommerziell** verwendet werden, also nicht in einem Spiel, mit
dem du Geld verdienst ([Kapitel 12](12-eigene-spiele.md)).

> **Mission:** Öffne deine gespeicherte Treppe aus 11.6 mit **Strg+O** (zum Beispiel
> `treppe.klevel`). Gib ihr eine **Rollende Kugel** (Rolling Ball) als Spielfigur (einfach aus der
> Gruppe **Spieler** (Players) platzieren) und exportiere sie. Klick **Spielen** (Play it) – und
> schick die Datei einem Freund.

---

## 11.15 Mehrere Spieler

Im Menü **Spielen** (Play) startest du dein Level mit 2 bis 4 Spielern an einem Rechner
(**Anzahl der Spieler** (Number of Players)), machst die Leitung absichtlich schlechter
(**Netzwerk-Emulation** (Network Emulation)) oder spielst mit anderen Tabs und Rechnern
(**Online-Spiel hosten...** (Host Online Game...), **Online-Spiel beitreten...** (Join Online Game...)). Wie das geht und warum es trotz
schlechtem Internet funktioniert, zeigt [Kapitel 10](10-netzwerk.md) mit zwei Missionen.

---

## 11.16 Wie Studio gebaut ist

Zum Schluss ein Blick unter die Haube. Studio besteht aus zwei Teilen:

- der *Engine* in C++, als WebAssembly im Browser. Sie enthält den ganzen Editor: Level, Undo,
  Auswahl, Viewports, Menüs, Spielen und Mehrspieler.
- der **Webseite**. Sie zeichnet nur die Panels, reicht Maus und Tasten weiter und erledigt, was nur
  ein Browser kann: Dateien speichern, Maus fangen, Ton ausgeben, Päckchen verschicken.

> **So kannst du es dir vorstellen:** Studio ist die Spielkonsole, die Webseite der Controller. Der
> Controller drückt nur Knöpfe und zeigt an, was die Konsole sagt. Denken tut die Konsole.

**Warum so?** Erstens läuft im Editor genau der Code deines exportierten Spiels. Es gibt keine zweite
Version, die heimlich anders rechnen könnte – was im Editor klappt, klappt auch im Spiel. Zweitens
lässt sich der ganze Editor **ohne Browser testen**: Automatische Tests können ihn direkt bedienen und
prüfen, ob Navigation, Gizmos, Spielen und Speichern noch stimmen. So fällt ein Fehler auf, bevor ihn
jemand beim Bauen erlebt.

**Alles ist ein Befehl.** Was du in Studio tust – ein Menüeintrag, ein Knopf, eine Taste, eine
Zeile in der Konsole –, wird zu einem **Befehl** mit Namen und Werten, etwa „spawn, physics_cube“ oder
„setze die Position auf 0, 0, 2“. Die Engine führt ihn aus. Weil alles durch dieselbe Tür geht, können
auch die Tests Studio genauso bedienen wie du. Aus solchen Tests stammen die meisten Studio-Bilder
dieses Buchs.

> **Für Profis:** Die genauen Namen der C++-Klasse, der Funktionen zwischen Webseite und Engine und
> aller Befehle stehen im blauen Buch, Kapitel 11, und in [Kapitel 17](17-referenz.md).

---

## Level geschafft!

- Du weißt, dass ein Level aus Actors besteht und Komponenten bestimmen, was ein Actor kann.
- Du schaust mit Viewports in die Welt und kennst Perspektive und Bauplan-Ansicht.
- Du wählst aus, bewegst nur mit dem Gizmo und nutzt das Raster wie Karopapier.
- Du weißt, warum Undo wie ein Spielstand funktioniert.
- Du verbindest Dinge als Eltern und Kinder oder mit Gelenken.
- Du testest mit **Spielen** (Play) und **Simulieren** (Simulate), ohne dein Level zu verändern –
  außer mit K.
- Du baust Logik ohne Code, speicherst, exportierst und weißt, warum jede Aktion ein Befehl ist.

## Rätsel-Ecke

1. Ein Actor hat nur ein **Statisches Mesh** (Static Mesh). Was musst du hinzufügen, damit er beim
   Simulieren herunterfällt?
2. Du ziehst mit der linken Maustaste über einen Würfel. Bewegt er sich?
3. Du lässt beim Simulieren einen Turm einstürzen und drückst Esc. Was ist mit dem Turm im Level?
4. Dein Logik-Graph hat einen Fehler. Startet das Spiel? Wo steht die Meldung?
5. **Boss-Rätsel:** Raster 0,5 m, ein Würfel bei x = 0. Du hältst Alt und ziehst in Weltachsen am
   roten Pfeil 1,3 m in Pfeilrichtung. Wie viele Würfel gibt es, bei welchem x landet der bewegte, und
   wie oft musst du Strg+Z drücken, bis wieder nur einer da ist?

### Lösungen

1. Einen **Starrkörper** (Rigid Body). Erst er macht den Actor zum Physik-Körper (11.3).
2. **Nein** – linkes Ziehen zeichnet ein Auswahlrechteck. Bewegen tut nur der Gizmo.
3. Er steht **unverändert** da. Testen verändert das Level nicht; nur **K** übernimmt Lagen.
4. **Ja.** Nur dieser Graph wird abgeschaltet; „Logic error: …“ steht im
   **Ausgabeprotokoll** (Output Log).
5. **Zwei** Würfel, das Original bleibt bei 0. Die Kopie rastet aufs nächstgelegene Vielfache von
   0,5 m: 1,3 liegt näher an 1,5 als an 1,0, also **x = 1,5 m**. **Ein** Strg+Z genügt – Kopie und
   Bewegung sind ein Schritt.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
