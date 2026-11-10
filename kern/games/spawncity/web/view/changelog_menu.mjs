// Unchanged from the original SpawnCity (spawncity.de, sim/changelog_menu.mjs, commit 9108537), run on Kern's page
// against the original's DOM (web/view.mjs); the game follows its settings (view_menu.mjs).
// Title screen: NEU / CHANGELOG under WORLD. The most important points of every update, newest
// first. New updates go on top of CHANGELOG (keep each point to one line a player understands).
// Each entry: date + time (Berlin) of the update; only what is NEW in that update — never repeat a
// point an earlier entry already announced.
// Mouse, touch, keyboard (Esc closes) and the pad (pad_actions.mjs: up/down scroll, B closes).
export const CHANGELOG_MENU_VERSION="title-changelog-v1";
export const CHANGELOG=Object.freeze([
  {date:"2026-11-10",time:"22:15",title:"Nativer Export: ein Level als Spiel mit kompilierten Blueprints",items:[
    "Kern-Engine: Ein Level lässt sich jetzt mit einem Befehl als eigenständiges Spiel exportieren, dessen Blueprints fest als C++ einkompiliert sind (tools/export_game.sh). Das Spiel ist dieselbe einzelne HTML-Datei, die „Package Game“ im Editor schreibt – nur muss beim Start nichts mehr übersetzt werden, und der Compiler optimiert Blueprints und Engine zusammen.",
    "Der Editor im Browser kann kein C++ kompilieren; sein „Package Game“ bleibt wie es ist (die Blueprints übersetzt dort der Browser beim Start). Ein automatischer Browser-Test spielt ein nativ exportiertes Level und prüft, dass seine Blueprints wirklich als kompilierte Klassen laufen.",
  ]},
  {date:"2026-11-10",time:"22:09",title:"Ganze Zahlen: überall dieselbe Zahl",items:[
    "Kern-Blueprints: Eine ganze Zahl (Integer) ist jetzt in jeder Form dieselbe – als Graph, im Browser beschleunigt und als kompiliertes C++. Bis 2^53 ist sie exakt; größer geht es bis zu den Enden von 64 Bit, nie darüber hinaus. Eine Variable, die sich immer wieder verdoppelt, bleibt am oberen Ende stehen (das kompilierte C++ lief dort bisher ins Undefinierte), und aus „keine Zahl“ oder unendlich wird beim Runden überall 0.",
    "Beschleunigte Blueprints gehen jetzt so tief wie die anderen Formen: Ihr Speicher für offene Sequences und Schleifen ist so groß, wie der Blueprint ihn je brauchen kann (bisher war bei 512 Schluss).",
    "Damit sind die bekannten Unterschiede zwischen Blueprint und C++ ausgeräumt; 17 Prüf-Blueprints liefern in allen drei Formen dasselbe.",
  ]},
  {date:"2026-11-10",time:"21:43",title:"Funktionen und Events: in jeder Form gleich tief",items:[
    "Kern-Blueprints: Löst eine Funktion ein Event aus und dessen Handler ruft dieselbe Funktion noch einmal auf, ist das jetzt ein Aufruf wie jeder andere – bisher wurde der innere Aufruf als Graph übersprungen, als kompiliertes C++ aber ausgeführt. Jeder Aufruf hat seine eigenen Werte; ist der Handler fertig, rechnet der unterbrochene Aufruf mit seinen weiter.",
    "Wie tief Events ineinander liegen dürfen (64), zählt Funktionsaufrufe nicht mehr mit, und lange Rechenketten werden vollständig ausgewertet (bisher endete der Graph nach 48 Werten hintereinander). Blueprint und C++ hören damit an derselben Stelle auf.",
  ]},
  {date:"2026-11-10",time:"21:11",title:"Ein C++ je Blueprint: der alte Export ist weg",items:[
    "Unter der Haube (für alle Kern-Spiele): Den früheren C++-Export von Blueprints – eine Schleife über Knotennummern, die zur Laufzeit in die Knoten-Bibliothek zurückrief – gibt es nicht mehr. Jeder Blueprint hat jetzt genau ein C++: seine eigene Klasse, in der jeder Knoten ein direkter Funktionsaufruf ist.",
    "Im Kern-Editor liefert Download in der C++-Ansicht immer diese Klasse. Kompiliert ein Blueprint nicht, sagt der Editor, warum – statt etwas anderes zu exportieren.",
  ]},
  {date:"2026-11-10",time:"20:55",title:"Jeder Blueprint läuft als kompiliertes C++ – auch die Spielknoten",items:[
    "Unter der Haube (für alle Kern-Spiele): Auch die Spielknoten sind jetzt C++-Funktionen – Actors bewegen und erzeugen, Physik, Schaden und Heilung, Sounds und Effekte, die Events des Spiels (Überlappung, Treffer, Eingabe, Tod und Respawn …), die Schleife über alle Actors mit einem Tag und die Roboter-Knoten. Damit gibt es keinen Knoten mehr, den ein C++-Compiler nicht ausführt: Jeder Blueprint, der fehlerfrei kompiliert, läuft im gebauten Spiel als seine eigene C++-Klasse.",
    "Blueprint und C++ rufen für jeden Knoten dieselbe Funktion auf. Geprüft wird das mit zwei Test-Levels, deren Blueprints jeden Spielknoten benutzen: einmal als Graph gespielt, einmal als kompilierte Klassen – jede Variable, jeder Actor und jede Meldung müssen übereinstimmen.",
    "Im Kern-Editor steht in der C++-Ansicht jetzt bei jedem Blueprint „runs as compiled C++“, und Download liefert seine Klasse.",
  ]},
  {date:"2026-11-10",time:"20:02",title:"Zufall in Blueprints: jeder Knoten mit eigener Folge",items:[
    "Kern-Blueprints: Jeder Random-Knoten zieht jetzt aus seiner eigenen Zufallsfolge. Was er liefert, hängt nicht mehr davon ab, welcher andere Random-Knoten vor ihm dran war – und ist als Blueprint und als kompiliertes C++ dasselbe. Die Zahlen selbst sind andere als bisher, weiterhin je Welt wiederholbar.",
    "Jeder Eingang eines Knotens wird genau einmal ausgewertet, auch wenn der Knoten ihn dann nicht braucht (Select liest beide Seiten).",
    "Damit läuft die gesamte Grundausstattung der Blueprints als kompiliertes C++. Es fehlen noch die Spielknoten.",
  ]},
  {date:"2026-11-10",time:"18:53",title:"Endlosschleifen: derselbe Schutz für Blueprint und C++",items:[
    "Kern-Blueprints: Der Schutz vor Endlosschleifen zählt jetzt Schleifendurchläufe (200 000 je Schritt) statt Knoten. Läuft eine Schleife darüber, enden die Schleifen dieser Kette, der Rest läuft noch zu Ende, dann ist der Blueprint gestoppt und es wird gemeldet. Bisher brach die Kette mittendrin ab.",
    "So verhält sich ein Blueprint in jeder Form gleich – als Graph, im Browser beschleunigt und als kompiliertes C++ hören alle beim selben Durchlauf auf. Damit laufen alle Prüf-Blueprints der Engine auch als C++-Klassen mit demselben Ergebnis.",
    "Nebenbei rechnen beschleunigte Blueprints schneller, weil nicht mehr jeder Knoten mitgezählt wird.",
  ]},
  {date:"2026-11-10",time:"18:31",title:"Tween: Finished kommt nie schon mit Play",items:[
    "Kern-Blueprints: Ein Tween meldet Finished jetzt immer erst mit seinem letzten Update in einem späteren Schritt – auch bei Dauer 0 (bisher im selben Augenblick wie Play). Ein Tween, der sich aus Finished selbst wieder startet, wiederholt sich damit sauber, als Blueprint wie als kompiliertes C++.",
    "Wer denselben Tween aus dessen eigenem Update neu startet, bekommt im Kern-Editor jetzt einen Hinweis am Knoten: Das wäre eine Endlosschleife.",
  ]},
  {date:"2026-11-10",time:"18:12",title:"Warten und State Machines laufen als kompiliertes C++",items:[
    "Unter der Haube (für alle Kern-Spiele): Auch Blueprints, die warten (Delay, Tween, Timer) oder State Machines benutzen, laufen jetzt als kompilierter C++-Code. Wartezeiten und Zustandswechsel führt dabei für Blueprint und C++ dieselbe Stelle der Engine – beide tun zur selben Zeit dasselbe.",
    "Damit läuft die ganze Grundausstattung der Blueprints als C++, mit Ausnahme von Zufallszahlen. Als Nächstes kommen der Schutz vor Endlosschleifen und die Spielknoten (Actors bewegen, Sounds, Eingabe).",
  ]},
  {date:"2026-11-10",time:"17:51",title:"Blueprint-Events laufen als kompiliertes C++",items:[
    "Unter der Haube (für alle Kern-Spiele): Auch Blueprints mit Events laufen jetzt als kompilierter C++-Code – Begin Play, Tick und eigene Events sind Methoden der Klasse, dazu Call Event, Print und Game Time. Wer welches Event bekommt, entscheidet für Blueprint und C++ dieselbe Stelle der Engine.",
    "Ein Event, das sich selbst wieder auslöst, hat jetzt in jedem Lauf seine eigenen Werte – wie die Parameter eines Funktionsaufrufs. Bisher sah der äußere Lauf danach die Werte des inneren.",
    "Warten (Delay), State Machines und die Spielknoten folgen als Nächstes.",
  ]},
  {date:"2026-11-10",time:"17:25",title:"Kern-Editor: die C++-Ansicht sagt, was schon kompiliert läuft",items:[
    "Im Kern-Editor zeigt die C++-Ansicht eines Blueprints jetzt, ob sein Code schon direkt als C++ läuft – oder welcher Knoten dafür noch fehlt. Download liefert genau die Datei, die ein Spiel dafür mitkompiliert.",
    "Die Klasse eines Blueprints heißt wie sein Actor, als gültiger C++-Name (aus „Door 1“ wird Door1).",
  ]},
  {date:"2026-11-10",time:"17:15",title:"Die Fahndungsregeln laufen im Spiel als kompiliertes C++",items:[
    "Die Fahndung von SpawnCity (Sterne, Polizei, Entkommen, EMP) läuft jetzt im Spiel selbst als kompilierter C++-Code ihres Blueprints – nicht mehr über den Blueprint-Interpreter. Die Werte sind dieselben; das wird bei jedem Update mit über 500 Aufrufen gegen den Blueprint geprüft.",
    "Unter der Haube (für alle Kern-Spiele): Beim Bauen eines Spiels werden die Blueprints seines Levels als C++-Klassen mitkompiliert. Das Level bleibt die eine Quelle – wird ein Blueprint geändert, läuft er bis zum nächsten Bau wieder als Blueprint, nie als veralteter Code.",
  ]},
  {date:"2026-11-10",time:"16:46",title:"Blueprint-Code läuft direkt als C++",items:[
    "Unter der Haube (für alle Kern-Spiele): Den C++-Text eines Blueprints führt jetzt ein C++-Compiler aus, so wie er dasteht – jeder Knoten ist eine Funktion, nichts wird beim Laufen in einer Knotenliste nachgeschlagen. Das gilt zuerst für Blueprints aus Funktionen, Formeln und Variablen; Events, Warten und die Spielknoten folgen.",
    "Was ein Knoten rechnet, steht dafür nur noch an einer Stelle: Graph und C++ benutzen dieselbe Funktion. Bei jedem Update laufen über 2000 Aufrufe einmal als kompiliertes C++ und einmal als Blueprint und werden Zeichen für Zeichen verglichen – auch mit Extremwerten wie unendlich.",
    "Die Fahndungsregeln von SpawnCity bestehen diese Prüfung vollständig. Im Spiel selbst laufen sie weiter als Blueprint, bis auch Events und Warten als C++ laufen – dort ändert sich nichts.",
    "Neue Regeln im Kern-Editor, damit Graph und C++ immer dasselbe bedeuten: Eine Funktion ruft sich nicht selbst auf, eine Funktion, die als Wert gelesen wird, rechnet nur, und Wiederholen ist Sache der Schleifen-Knoten (ein Draht zurück braucht ein Warten). Verstöße zeigt der Editor am Knoten.",
    "Add to Variable zählt ganze Zahlen jetzt ganzzahlig.",
  ]},
  {date:"2026-11-10",time:"15:48",title:"Blueprints sind jetzt auch C++ – in beide Richtungen",items:[
    "Unter der Haube (für alle Kern-Spiele): Jeder Blueprint hat jetzt genau einen C++-Text, und jeder solche Text ist genau ein Blueprint. Variablen sind Member, Funktionen sind Funktionen, eine Verzweigung ist ein if, eine Formel ist der Ausdruck selbst. Hin und zurück geht nichts verloren – das wird bei jedem Update für jeden Knotentyp der Engine geprüft.",
    "Im Kern-Editor zeigt der Knopf C++ im Blueprint-Editor diesen Text. Er lässt sich dort ändern und anwenden: Der Blueprint ist dann, was im C++ steht, und alle Knoten bleiben, wo sie waren. Was sich nicht lesen lässt, wird mit Zeile und Grund abgelehnt.",
    "Die Fahndungsregeln von SpawnCity lassen sich so als C++ lesen und ändern – es bleibt ein und dieselbe Quelle. Am Spiel ändert sich nichts.",
    "Dafür heißen Variablen, Funktionen und Pins in Blueprints jetzt wie in C++ (ohne Leerzeichen); ältere Levels werden beim Öffnen von selbst umbenannt.",
  ]},
  {date:"2026-11-10",time:"12:45",title:"SpawnCity wird ein Editor-Projekt: die ersten Regeln sind Blueprints",items:[
    "Die Fahndung (Sterne, Polizei-Anzahl, Ankunft, Trefferchance, EMP, Entkommen) steht nicht mehr im Programmcode, sondern als Blueprint im Level von SpawnCity – 19 Funktionen, die sich im Kern-Editor öffnen und ändern lassen. Am Spiel ändert sich nichts: Alle Werte sind die des Originals und werden bei jedem Update gegen dessen Tabelle geprüft.",
    "Das Level von SpawnCity ist jetzt eine Datei, die der Kern-Editor lesen kann. Schritt für Schritt wandern Regeln und Systeme dorthin, bis SpawnCity ganz ohne eigenen Programmcode auskommt.",
    "Neu in der Engine (für alle Kern-Spiele): Blueprint-Funktionen mit Ein- und Ausgängen und ein Formel-Knoten, der eine ganze Formel wie max(9000, 16000 - level * 1300) als einen Knoten rechnet.",
  ]},
  {date:"2026-11-10",time:"10:12",title:"Controller, Finger und Gerät: überall gleich gelesen",items:[
    "Controller: Die große Karte hört jetzt auf denselben Controller wie das Spiel und die Menüs. War neben dem Xbox-Controller ein zweites Gerät angeschlossen (Lenkrad, Funkempfänger), reagierte die Karte bisher unter Umständen gar nicht.",
    "Unter der Haube: Auch Controller, Geräteerkennung (Maus, Finger, Controller) und das Loslassen aller Finger liest jetzt die eine Eingabeschicht der Kern Engine – für SpawnCity, die Beispielspiele, den Editor und jedes exportierte Spiel gleich.",
    "Jede dieser Seiten wird ab jetzt bei jedem Update automatisch mit echter Maus und Tastatur gegen dieselben Regeln geprüft.",
  ]},
  {date:"2026-11-10",time:"09:13",title:"Eine Eingabeschicht für alle Kern-Spiele",items:[
    "Unter der Haube: Tastatur, Maus, Zeigersperre und Fokus liest jetzt eine einzige Schicht der Kern Engine – für SpawnCity genauso wie für jedes andere Kern-Spiel. An der Bedienung ändert sich nichts; Eingabefehler können so nicht mehr nur in einem der Spiele stecken.",
  ]},
  {date:"2026-11-10",time:"03:59",title:"Verkehr fährt über Kartengrenzen weiter",items:[
    "Autos und Busse fahren an den Grenzen der Kartenkacheln einfach weiter, statt dort zu wenden – der Verkehr fließt durchgehend durch die Stadt.",
    "Straßen liegen an diesen Grenzen nicht mehr doppelt übereinander: keine Geister-Kreuzungen und keine doppelten Fahrspuren mehr.",
  ]},
  {date:"2026-11-10",time:"03:36",title:"Verkehr ohne Auffahrunfälle",items:[
    "Autos und Busse springen nicht mehr aus Staus in die Luft: Fahrer hielten Abstand, indem sie mit Vollgas zurücksetzten – dabei schoben sie sich aufeinander und wurden hochgeschleudert. Jetzt bremsen sie wie im Original hinter dem Vordermann bis zum Stand.",
    "Fahrer folgen ihrer Spur genauer (Zielpunkt ein Stück voraus auf der Fahrbahn) und bremsen vor Kurven – weniger Ausflüge auf den Gehweg.",
    "Verkehr in der Ferne hält sich an Ampeln, Fußgänger und Abstände und fährt nicht mehr durch wartende Autos; er wird erst dort wieder zum echten Fahrzeug, wo Platz und fester Boden ist.",
    "Autos stehen am Hang so, wie die Straße liegt, und setzen ohne Ruck auf ihren Rädern auf.",
    "Der Abschleppdienst kommt nur noch für Autos, die wirklich festsitzen – nicht für die Schlange vor einer roten Ampel.",
  ]},
  {date:"2026-11-10",time:"02:22",title:"Zielen und Schießen gleichzeitig",items:[
    "Maus: Mit gehaltener rechter Taste anvisieren und dabei mit der linken schießen funktioniert jetzt zuverlässig – in beliebiger Reihenfolge der Tasten.",
    "Maus: Das Anvisieren bleibt stabil, solange du die rechte Taste hältst.",
  ]},
  {date:"2026-11-10",time:"01:31",title:"Flüssiger bei viel Verkehr",items:[
    "Deutlich flüssiger in vollen Straßen: Die Fahrzeug-Physik kostet nur noch einen Bruchteil der Rechenzeit.",
  ]},
  {date:"2026-11-10",time:"01:15",title:"Multiplayer: eine Welt für alle",items:[
    "Multiplayer: Passanten, Jogger, Autos, Kühe, Hunde, Katzen und Tauben sind bei allen exakt dieselben – der Host bestimmt die Straße, Mitspieler zeigen genau seine Welt und erfinden nichts mehr selbst dazu.",
    "Multiplayer: Was der eine sieht, fehlt beim anderen nicht mehr – auch volle Straßen und große Schwärme kommen vollständig bei jedem Mitspieler an.",
    "Multiplayer: Die Autos des Hosts fahren bei Mitspielern flüssig auf seiner Spur.",
    "Multiplayer: Ampeln schalten bei allen gleichzeitig; erlegte Tauben eines Schwarms sind bei allen dieselben.",
  ]},
  {date:"2026-11-10",time:"00:30",title:"Die Weltkarte zeigt wieder die ganze Welt",items:[
    "Große Karte: Sie zeigt wieder die ganze Welt – von den Kontinenten bis zur Bordsteinkante zoombar, mit Straßen, Wasser, Parks, Häusern und Ortsnamen.",
    "Große Karte: Wegpunkte lassen sich überall auf der Welt setzen.",
  ]},
  {date:"2026-11-10",time:"00:21",title:"Die Welt außerhalb deiner Stadt stimmt wieder",items:[
    "Überall auf der Welt steht die Stadt wieder richtig auf dem Gelände – keine versunkenen oder schwebenden Viertel mehr, wenn du deine Startstadt verlässt.",
    "Straßen liegen wie im Original eben im Gelände, Häuser stehen passend daran.",
    "Beim schnellen Fahren stehen die Häuser schon dort, wo du hinkommst: Die Stadt baut sich entlang deines Wegs voraus auf.",
    "Bäume und Laternen stehen neben der Straße – nicht mehr auf der Fahrbahn, in Häusern oder in der Luft; Ampeln stehen an der Ecke der Kreuzung.",
    "Fehlt ein Kartenstück wegen schlechter Verbindung, wird es von selbst erneut geladen.",
    "Autos und Busse fahren auf echten Rädern mit Federung – die alte Federung konnte Busse in die Luft schleudern.",
  ]},
  {date:"2026-11-07",time:"20:35",title:"Vollständiger Rollback auf den Stand vor Freunde/öffentlich und Spielrunden",items:[
    "SpawnCity läuft wieder wie vor dem MULTI-Update mit MIT FREUNDEN / ÖFFENTLICH und der öffentlichen Tagesstadt.",
    "Die fünf Spielrunden mit Abstimmung, das Runden-Testkommando, XP/Level und die Bild-/Clip-Teilen-Funktionen sind zurückgenommen.",
    "Der Spielstand entspricht wieder der Version vor diesen Updates; das übrige Spiel bleibt unverändert.",
  ]},
  {date:"2026-11-07",time:"18:52",title:"Beifahrer sitzen, Hunde überall, Würfe ohne Haken",items:[
    "Multiplayer: Beifahrer sitzen jetzt sichtbar im Auto (Kopf und Arme im Fenster, ihre Waffe in der Hand) und schauen auf Sitzhöhe heraus – getroffen werden sie dort, wo sie sitzen.",
    "Multiplayer: Hunde und Katzen gibt es auch rund um weit entfernte Mitspieler – jede Nachbarschaft mit denselben Runden für alle.",
    "Multiplayer: Geworfene Passanten, Tiere und Tauben folgen bei allen exakt der Bahn des Werfers – auch bei schwankender Verbindung ohne Haken.",
    "Multiplayer: Auch weit entfernte Mitspieler (über 4 km vom Host) haben beim Host das echte Gelände unter ihren Autos.",
  ]},
  {date:"2026-11-07",time:"17:56",title:"Echtes Gelände bei Mitspielern",items:[
    "Multiplayer: Unter weiter entfernten Mitspielern liegt auch beim Host das echte Gelände – Autos fahren dort über dieselben Hügel wie bei ihnen.",
    "Neuigkeiten: jedes Update steht wieder mit Datum, Uhrzeit und eigenem Titel im NEU / CHANGELOG-Menü.",
  ]},
  {date:"2026-11-07",time:"17:46",title:"Eigener Jet-Platz",items:[
    "Multiplayer: Jeder Spieler hat seinen Jet auf einem eigenen Platz – nie mehr zwei Jets übereinander; am bemannten Jet eines Mitspielers steigst du als Mitflieger ein, statt in deinen eigenen.",
  ]},
  {date:"2026-11-07",time:"17:28",title:"Alle Geschosse sichtbar, kein Ruckeln",items:[
    "Multiplayer: Jedes Geschoss fliegt bei allen sichtbar auf der Bahn des Schützen – Handgranaten, Granatwerfer, Raketen, Lenkraketen samt Kurven, Drohnenraketen, Jet-Raketen und Bomben – mit Leuchten, Licht und Rauch.",
    "Multiplayer: Kein Ruckeln mehr – Mitspieler laufen mit einem Puffer, der sich der Verbindung anpasst; Autos, Passanten, Jogger, Kühe und Tauben werden zwischen den Paketen in ihrem eigenen Tempo weiterbewegt statt stoßweise nachgezogen.",
  ]},
  {date:"2026-11-07",time:"17:11",title:"Beifahrer schießen",items:[
    "Multiplayer: Als Beifahrer im Auto kannst du schießen – du stehst mit deiner Waffe im Wagen (Kopf und Arme über dem Dach), deine Kugeln und Granaten verlassen das Auto, statt darin zu explodieren, und die anderen sehen dich; der Knopf heißt MITFAHREN bzw. AUSSTEIGEN.",
    "Multiplayer: Wer im Jet mitfliegt, hat die Stadt um sich – sie lädt dort, wo der Jet ist, nicht mehr dort, wo er eingestiegen ist (vorher: nur Ampeln und Wiese).",
  ]},
  {date:"2026-11-07",time:"16:55",title:"Tiere und Würfe für alle gleich",items:[
    "Multiplayer: Tiere sind für alle dieselben – Kühe, Hunde, Katzen und jeder einzelne Tauben-Schwarm kommen vom Host (Hunde und Katzen laufen ihre Runde nach derselben Uhr, also exakt gleich), auch rund um weit entfernte Mitspieler.",
    "Multiplayer: Was einer packt und wirft – Passanten, Hunde, Katzen, Kühe, Tauben –, fliegt auf allen Geräten mit Physik dieselbe Bahn: der Werfer besitzt den Körper, bis er still liegt, die anderen steuern ihre Kopie auf seine Spur.",
  ]},
  {date:"2026-11-07",time:"16:31",title:"Eine zerstörte Stadt für alle",items:[
    "Multiplayer: Eine zerstörte Stadt für alle – Krater (Boden, Kollision und Straßen darauf) und Gebäudeschäden sind der Zustand des Hosts und auf jedem Gerät gleich, auch in gerade nicht geladenen Vierteln und für Spieler, die später dazukommen; eine Prüfsumme gleicht Abweichungen automatisch ab, RESET setzt die Stadt überall zurück.",
  ]},
  {date:"2026-11-07",time:"16:20",title:"Neue Mitspieler-Anzeige",items:[
    "Multiplayer: Neue Mitspieler-Anzeige – im Blick ein Schild über dem Kopf (Name, Entfernung, ▲/▼ wie viel höher oder tiefer), außer Sicht ein großes Abzeichen am Rand mit Pfeil zu ihm, Entfernung und LINKS / RECHTS / HINTER DIR / ÜBER DIR / UNTER DIR – für jeden Mitspieler.",
  ]},
  {date:"2026-11-07",time:"16:12",title:"Sprachchat zeigt An/Aus",items:[
    "Sprachchat: Mikrofon- und Lautsprecher-Knopf zeigen klar, ob sie an sind (grün leuchtend) oder aus (dunkel, durchgestrichen); die Mikrofon-Freigabe wird direkt beim Tippen gefragt – in VR schon beim Betreten der Brille, im VR-Menü gibt es MIKROFON AN/AUS; wird es verweigert, sagt das Spiel es.",
  ]},
  {date:"2026-11-07",time:"16:05",title:"Die Einstellungen des Hosts gelten",items:[
    "Multiplayer: Die WELT-Einstellungen des Hosts gelten für alle (Leute, Verkehr, Tiere, Zombies, Polizei, Militär, Flak, Jetpack, zerstörbares Gelände, Lenkraketen, Immer Tag) – Mitspieler sehen sie gesperrt, danach gelten wieder ihre eigenen.",
  ]},
  {date:"2026-11-07",time:"15:57",title:"Mitfahren und GPS-Start",items:[
    "Multiplayer: Mitfahren und Mitfliegen funktioniert sichtbar – neben dem stehenden Auto oder gelandeten Jet eines Mitspielers erscheint MITFAHREN/MITFLIEGEN (auf Touch der Einsteigen-Knopf), drinnen AUSSTEIGEN.",
    "Multiplayer: Startet der Host per GPS oder an einem gewählten Ort, ziehen alle Mitspieler mit in seine Stadt – vorher blieb eine früh eröffnete Sitzung am Standardort hängen.",
  ]},
  {date:"2026-11-07",time:"15:42",title:"Granaten treffen Mitspieler",items:[
    "Multiplayer: Granaten, Raketen und Lenkraketen fliegen nicht mehr durch Mitspieler – sie explodieren an ihrem Körper oder Fahrzeug.",
  ]},
  {date:"2026-11-07",time:"15:36",title:"Drohnen-Höhe wie im Original",items:[
    "Drohne: Höhenhaltung wie im Original – der Höhenmesser sieht nur Boden und Gebäude, Autos, Passanten und Tiere darunter lassen sie nicht mehr springen; auf dem Handy reagiert das Höhenfeld wieder genau.",
    "Radio verstummt, sobald Musik oder Ton ausgeschaltet ist.",
  ]},
  {date:"2026-11-07",time:"15:27",title:"Sicherer Start, Boxen ohne Schussgeräusch",items:[
    "Start, RESET, Respawn und Beitritt: du wirst mit Sicherheitsabstand über dem Boden abgesetzt und gehalten, bis der Boden unter dir fertig geladen ist – kein Durchrutschen mehr beim Start.",
    "Multiplayer: Boxen und Messer eines Mitspielers erzeugen keine Schuss-Töne und Schuss-Spuren mehr; man hört den Schlag, wenn er trifft. Man sieht seine Schläge (der Arm schnellt vor), bei VR-Spielern die echten Handbewegungen samt Waffe in der Hand. Boxen am Desktop/Handy trifft jetzt auch Mitspieler.",
  ]},
  {date:"2026-11-07",time:"15:14",title:"Kompakte Handy-Oberfläche",items:[
    "Handy: kompakte Oberfläche – mehr Spiel, weniger Knöpfe im Bild: durchsichtige Sticks (nur ein dünner Ring, greifen weiter auf voller Fläche), kleinere, halbtransparente Aktionsknöpfe, schmale obere Leiste mit ⚙ statt SETTINGS, kleinere Minimap, Sprach-Knöpfe nur im MULTI.",
  ]},
  {date:"2026-11-07",time:"15:10",title:"Echte Physik bei fernen Mitspielern",items:[
    "Multiplayer: auch rund um weit entfernte Mitspieler fahren die Autos des Hosts mit echter Physik – der Host lädt dort Boden- und Gebäudekollision mit.",
  ]},
  {date:"2026-11-07",time:"13:08",title:"Der Host bestimmt die Stadt",items:[
    "Multiplayer: der Host bestimmt die ganze Stadt – er lädt sie auch rund um jeden Mitspieler und führt dort Verkehr und Passanten; ein Mitspieler hat nur noch so lange eine eigene Stadt, bis die des Hosts bei ihm ankommt.",
    "Jet am Handy: KANONE sitzt jetzt über dem rechten Stick statt auf der Knopfleiste; Drohne: 📻 links neben der Minimap.",
    "VR: das Menü wählt nie aus Versehen die erste Zeile; Stick-Bewegungen im Menü drehen dich nicht und wechseln keine Waffe; gehaltene Tiere bleiben in der Hand; beim Laufen mit den Armen kein Wusch-Ton bei jedem Schwung.",
    "Luftschlag und Explosionen treffen den Piloten unter seiner Drohne zuverlässig; Aussteigen unter einer Brücke landet nicht mehr oben auf ihr.",
  ]},
  {date:"2026-11-07",time:"12:36",title:"Leise Atmosphäre, grüne Werfer-Granate",items:[
    "Leise Atmosphäre am Boden: ein fernes Stadtrauschen, Vogelgezwitscher am Tag, ab und zu ein Hund, nachts Grillen und manchmal ein Käuzchen, oben Wind (Lautstärke: Ton → Umgebung).",
    "Die Werfer-Granate leuchtet wieder grün wie im Original und taucht die Straße im Flug in ihr Licht.",
    "VR: Tauben fangen – ganz langsam anschleichen und vorsichtig zugreifen (klappt nicht immer, sonst fliegt der Schwarm auf); loslassen und sie fliegt davon.",
  ]},
  {date:"2026-11-07",time:"12:29",title:"Jogger für alle gleich, Beifahrer",items:[
    "Multiplayer: auch die Jogger-Gruppe ist bei allen dieselbe; die Weltdaten sind jetzt sehr sparsam (rund 1 KB/s) – Mitspieler ruckeln und hängen nicht mehr, auch über die Relay-Verbindung.",
    "Multiplayer: als Beifahrer zu einem Mitspieler ins Auto steigen (E / X / Einsteigen neben seinem stehenden Auto) – auch in seinen Jet; nochmal drücken steigt aus.",
  ]},
  {date:"2026-11-07",time:"12:15",title:"Straßen im Krater, Lichtsäulen",items:[
    "Straßen liegen nach Explosionen sauber im Krater statt darüber zu schweben (feines Netz über verformtem Boden, auch bei mehreren Kratern kurz hintereinander).",
    "Multiplayer: über jedem Mitspieler, der weiter als 40 m weg ist, steht eine Lichtsäule in seiner Farbe; die Pfeile am Bildrand zeigen auch bei Mitspielern hinter dir in die richtige Richtung.",
  ]},
  {date:"2026-11-07",time:"12:06",title:"Eine gemeinsame Welt",items:[
    "Multiplayer: eine gemeinsame Welt – alle sehen dieselben Autos an derselben Stelle (gleiche Farbe, gleiche Fahrt) und dieselben Passanten; der Host führt Verkehr und Passanten, rund um jeden Spieler. Das Auto, das ein Mitspieler fährt, gehört ihm.",
  ]},
  {date:"2026-11-07",time:"11:47",title:"Mitspieler mit echter Waffe",items:[
    "Multiplayer: Mitspieler halten ihre echte Waffe (Pistole, MP, Sniper, Werfer, Raketenwerfer, Messer) und zielen sichtbar dorthin, wohin sie schauen – hoch, runter, links, rechts.",
    "Respawn und RESET stellen dich auf den Boden, wie er jetzt ist – auch in einem Krater, nie darunter.",
    "VR: Zielfernrohr ganz einfach – linken Abzug halten (oder das Okular ans Auge), das Bild kommt vors Auge.",
  ]},
  {date:"2026-11-07",time:"11:38",title:"Großes VR- und Touch-Update",items:[
    "Handy: der rechte Stick steht jetzt in jedem Modus etwas vom Rand weg – sichtbar genau dort, wo er auch greift (vorher sah man ihn am Rand, gegriffen wurde weiter links).",
    "Minimap in jeder Welt; ⛶ öffnet die große Karte.",
    "Radio auch im Jet und in der Drohne (📻 in der Leiste, in VR im Menü).",
    "Luftschlag mit den Original-Tönen beim Zielen, Markieren und Bestätigen; wer im Bombenteppich steht, stirbt – auch der Pilot unter seiner Drohne.",
    "Eigene Drohnen-Schüsse treffen den eigenen Piloten am Boden wieder.",
    "Granaten fliegen nicht mehr durch Passanten, sie explodieren an ihnen.",
    "Ruß und Löcher an Fassaden verschwinden mit den weggesprengten Stockwerken.",
    "Umgefallene Passanten sehen als Ragdoll aus wie vorher: Haare, Gesicht, runder Körper, Schuhe.",
    "VR: Boxen und Messer allein durch die Bewegung – ein Schlag trifft, wenn die Faust ankommt, stößt mit ihrer Wucht und rüttelt am Controller; Ohrfeigen schleudern.",
    "VR: Passanten, Hunde, Katzen und Kühe packen und richtig werfen (der ganze Körper fliegt mit dem Schwung).",
    "VR: Laufen mit den Armen – je kräftiger, desto schneller (bis Sprint), und dabei zielen und schießen.",
    "VR: Waffenwechsel am rechten Stick (hoch/runter), weich oder in 30°-Schritten drehen.",
    "VR: Menü mit Einstellungsseite (Laufen, Drehen, Horizont, Tunnel, Immer Tag, Ton, Musik, Radio), Einsteigen/Aussteigen, Kamera innen/außen (auch: linken Stick drücken); Y halten = RESET von überall.",
  ]},
  {date:"2026-11-07",time:"10:41",title:"Handy-Knöpfe sicher, VR-Fahrzeuge",items:[
    "Handy: DRONE, ARM und die anderen Knöpfe schalten bei jedem Tipp sicher – auch nach längerem Drücken (vorher schalteten sie manchmal hin und sofort zurück, die Drohne blieb unscharf und die Sticks taten nichts).",
    "VR: Drohne wie eine echte Fernsteuerung (Mode 2) – links Gas und Drehen, rechts Neigen, rechter Abzug feuert, A startet und landet.",
    "VR: Auto und Jet immer im Cockpit; du schaust mit dem Kopf, der rechte Stick dreht die Sicht nicht mehr.",
    "VR: Komfort-Tunnel – der Rand des Blicks dunkelt ab, wenn das Spiel dich dreht oder beschleunigt; Horizont halten oder frei im Cockpit mitkippen (Einstellung und Brillen-Menü).",
    "VR: auf der Uhr am Handgelenk stehen in Drohne, Auto und Jet die Tasten.",
  ]},
  {date:"2026-11-06",time:"23:48",title:"VR: Kimme und Korn",items:[
    "VR: Kimme und Korn liegen genau auf der Schusslinie; B wechselt die Waffe, EMP im Menü (im Auto dort das Radio).",
  ]},
  {date:"2026-11-06",time:"23:41",title:"VR-Multiplayer: packen und schubsen",items:[
    "VR-Multiplayer: Mitspieler am Kragen packen, ziehen und schubsen, Schläge und Messer treffen sie – repliziert auf ihrer Seite.",
  ]},
  {date:"2026-11-06",time:"23:35",title:"VR: Greifen",items:[
    "VR: Greifen mit Griff-Taste (oder Abzug der freien Hand) – Kisten, Tiere, Passanten (fallen als Ragdoll in deine Hand), Autos nur ein wenig schieben; loslassen wirft mit deinem Schwung.",
  ]},
  {date:"2026-11-06",time:"22:49",title:"Himmel ohne Kanten",items:[
    "Der Himmel ohne Kachel-Kanten in den Wolken.",
  ]},
  {date:"2026-11-06",time:"22:43",title:"Zombies stöhnen, neue Polizeidrohnen",items:[
    "Getroffene Vögel krächzen jedes Mal anders.",
    "Zombies stöhnen gruselig (jeder anders) und kreischen bei Treffern.",
    "Polizeidrohnen weiß mit schwarzem Bauch, blauem Band und rot-blauem Blitzlicht; Martinshorn und Drohnen-Ton deutlich hörbar, klar verschieden.",
  ]},
  {date:"2026-11-06",time:"22:40",title:"VR: Waffe in echter Größe, Uhr-HUD",items:[
    "VR: nur die Waffe in der Hand, keine Arme; die Pistole in echter Größe und hinter Wänden verdeckt wie ein echtes Objekt.",
    "VR: Boxen und Messer per Schlag mit dem Controller (nicht per Abzug), beim Messer die linke Hand als Faust; Messer tötet mit einem Stich.",
    "VR: Uhr-HUD am linken Handgelenk statt im Blickfeld; Y öffnet dort das Menü – MULTI, Drohne/zu Fuß, Luftschlag wohin du schaust, Radio, Laufen, Reset, VR beenden.",
    "VR: Raketen folgen deinem Blick, Granatwerfer und Raketen fliegen aus der Waffe in der Hand; beim Tod dreht sich die Welt nicht mehr.",
    "Neu unter WELT: Immer Tag. Sommer-Sonne mit kürzeren Nächten, hellerer Mond und hellere Straßenlaternen.",
  ]},
  {date:"2026-11-06",time:"22:24",title:"Rechter Stick weiter vom Rand",items:[
    "Rechter Stick deutlich weiter vom Rand weg (Platz zum schnellen Drehen nach rechts), schnelle Wischer drehen weiter (Beschleunigung wie CoD Mobile).",
    "MULTI ruckelt nicht mehr: die Nostr-Signatur (über 100 ms je Paket) lief jede Sekunde mehrfach – jetzt aus; Daten ohne Direktverbindung gebündelt.",
  ]},
  {date:"2026-11-06",time:"22:14",title:"Handy ohne Flackern",items:[
    "Handy: kein Flackern und Ruckeln mehr durch ständiges Vollbild-Umschalten bei jedem Tipp; der rechte Stick ist wieder da; Schießen per Touch wieder wie vorher (Tipp auf den Bildschirm, kein FIRE-Knopf); der VR-Knopf erscheint nur noch im Titel.",
  ]},
  {date:"2026-11-06",time:"21:48",title:"Glänzendes Licht",items:[
    "Licht glänzt jetzt auf Lack, Glas und Straße; auf starken Rechnern bis zu 10 Lichter (Laternen, Scheinwerfer ringsum), auf dem Handy wie im Original.",
    "VR, zwei Steuerungen (Einstellungen → VR, in der Brille: beide Sticks drücken): Sticks – links gehen, rechts in 30°-Schritten drehen; oder Laufen mit den Armen – links joggen = gehen, beide = rennen, ohne Stick.",
  ]},
  {date:"2026-11-06",time:"21:22",title:"VR ohne schwarzes Bild",items:[
    "VR: kein schwarzes Bild mehr (die Seite verkleinerte das Bild der Brille), ein Schattenbild pro Frame statt pro Auge, Foveated Rendering und eine leichtere Auflösung gegen Ruckler, der Himmel sitzt richtig zu jedem Auge.",
    "VR: dein Körper dreht sich mit, wenn du dich im Raum drehst; die Waffen liegen in der rechten Hand und schießen aus ihr, die Fäuste in beiden.",
  ]},
  {date:"2026-11-06",time:"19:39",title:"Neuer Raketenwerfer",items:[
    "Neuer Raketenwerfer: kompaktes olivgrünes Rohr mit Optik und Lasermodul, tiefer und weiter rechts gehalten – er verdeckt die Sicht nicht mehr.",
  ]},
  {date:"2026-11-06",time:"19:25",title:"Polizei wie im Original",items:[
    "Polizei wie im Original: schwarze Streifenwagen mit weißen Türen und rot-blau blitzendem Balken, Polizisten in schwarzer Weste.",
  ]},
  {date:"2026-11-06",time:"19:23",title:"Meta Quest: IN VR SPIELEN",items:[
    "Meta Quest: großer Knopf IN VR SPIELEN, keine Maus-Sperre mehr, die zurück ins Menü warf.",
    "Kein Durchfallen mehr bei weiten Flügen: der Boden unter dir wird immer sofort gebaut, die Höhendaten werden vorausschauend in Flugrichtung geladen.",
    "EJECT löst den Schleudersitz per Tipp aus; RESET setzt aus dem Cockpit zurück an den Start – ohne Schleudersitz.",
    "Raketen und Granaten treffen Busse an ihrer ganzen Länge.",
  ]},
  {date:"2026-11-06",time:"18:41",title:"App immer aktuell, schönere Menschen",items:[
    "Installierte App (PWA) startet immer mit der neuesten Version.",
    "Nur Querformat: Handys sperren auf quer (Vollbild), sonst zeigt SpawnCity „Bitte quer halten“ – Drehen macht nichts mehr kaputt.",
    "HOME SCREEN ist immer da, solange SpawnCity nicht installiert ist.",
    "Treffer sitzen genau auf der Figur: Kopf, Oberkörper mit Armen, Beine – auch bei Mitspielern, Drohnenpiloten und am Fallschirm.",
    "Schönere Menschen: runde Glieder, Schuhe, Hände, Gesicht und Haare; die Spielfigur mit Gesicht.",
  ]},
  {date:"2026-11-06",time:"18:17",title:"Jet für Dogfights, Lenkrakete, Luftschlag-Pfeile",items:[
    "Multiplayer: Mitspieler und ihre Zombies, Soldaten und Panzer stehen auf deinem Boden – kein Schweben, kein Waten mehr.",
    "Jet arcade-steuerbar für Dogfights: der Knüppel dreht direkt, die Flugbahn folgt der Nase, im Schwebeflug dreht er auf der Stelle.",
    "Jet-Crash in Wand oder Boden: er explodiert. Harte Landungen beschädigen ihn.",
    "Lenkrakete: erster Tipp markiert das Ziel (roter Ring), zweiter Tipp feuert.",
    "Luftschlag: blaue Pfeile auf dem Boden zeigen, wo die Bomben entlanglaufen – wie im Original.",
    "Vögel und Tauben kreischen beim Treffer, stürzen ab und schlagen hörbar auf.",
    "Die Titelmusik läuft nie mehr doppelt.",
    "MULTI-Knopf zeigt klar SOLO ▸ MULTI, die Suche, das Warten und die Spielerzahl.",
    "Auto: der linke Stick bewegt sich sichtbar mit.",
  ]},
  {date:"2026-11-06",time:"17:54",title:"Ton-Knöpfe und Ladebildschirm",items:[
    "Ton- und Musikknöpfe zeigen klar AN oder AUS; kurzer Ladebildschirm, bis die Welt steht.",
  ]},
  {date:"2026-11-06",time:"17:33",title:"Kein Flackern, klares Zielfernrohr",items:[
    "Kein Flackern mehr an Fassaden; das Zielfernrohr zeigt alles, auch weit weg.",
    "Messer mit Ausfallschritt, Treffer-Ruck und Blut; Fäuste mit linker und rechter Maustaste.",
    "Ohne Netz startet das Trainingsgelände mit Straßen, Teich, Brücke und Häusern.",
  ]},
  {date:"2026-11-06",time:"16:40",title:"Update-Knopf, Scheinwerfer, Bäume",items:[
    "UPDATE AVAILABLE: ist eine neue Version online, erscheint ein Knopf zum Aktualisieren.",
    "VR: auf der Meta Quest mit dem VR-Knopf in die Brille, die Controller steuern alles.",
    "Wracks sind klar verkohlt, brennen aus und werden abgeräumt; Granaten zerstören auch den Jet.",
    "Nachts Scheinwerfer an allen Autos; Blaulicht färbt die Straße.",
    "Bäume stehen auf dem Boden und kippen bei Explosionen um; Tauben lassen sich abschießen.",
    "Menschen schreien bei Treffern, Raketen zischen im Flug.",
    "Schüsse kommen beim Seitwärtslaufen genau aus der Waffe; der Jet steht nicht mehr auf der Straße.",
  ]},
  {date:"2026-11-06",time:"16:00",title:"SpawnCity läuft jetzt auf Kern",items:[
    "Schneller und flüssiger: die ganze Welt läuft auf der neuen Kern-Engine.",
    "Luftschlag landet genau dort, wo du tippst.",
    "Messer, Fäuste und Fingerschüsse reagieren auf jeden kurzen Tipp; Fäuste: linke Hälfte linke Hand, rechte Hälfte rechte Hand.",
    "Hochkant gehaltene Handys zeigen das Spiel richtig im Querformat.",
    "Jet: Steuer-Sticks und KANONE wieder da, kein Flugzeug mehr in der Innenkamera, aufgeräumte Knöpfe.",
    "MULTI zeigt, was passiert: Suche, Lobby offen, Anzahl der Spieler.",
    "Kopftreffer bei Mitspielern zählen zuverlässig; am Fallschirm hängt die echte Spielfigur.",
    "Granaten blitzen beim Aufprall, Brandflecken laufen weich in den Boden aus, schärferes Bild.",
    "Neu unter WELT: Lenkraketen abschaltbar. Standard jetzt: Friendly Fire an, zerstörbares Gelände und Flak aus.",
    "EJECT sitzt jetzt links oben und verdeckt die Minimap nicht mehr.",
    "Bei Kopftreffern gibt es weder HEADSHOT-Schrift noch Ansager – nur ein kurzes neutrales Knacken.",
    "Vollbild startet wieder wie vor der App-Umstellung; die Steuerung bleibt in normaler Ansicht und Vollbild nutzbar.",
    "Alle bisherigen Änderungen und Updates erscheinen wieder in der richtigen Reihenfolge im NEU / CHANGELOG-Menü.",
    "Die aktuelle Spielversion wird jetzt einheitlich unter spawncity.de veröffentlicht – einschließlich des verbesserten Touchpad-Mouselooks.",
    "Notebook-Touchpads drehen die Kamera in der Ego-Perspektive wieder gleichmäßig, auch bei schnellen Fingerbewegungen.",
    "HEADSHOT! Kopftreffer mit Sniper und Glock töten sofort, mit der MP dreifacher Schaden – mit Einblendung und Ansager.",
    "Sniper-Zielfernrohr hängt nicht mehr: Drohne, Auto, Jet, Drehen des Handys oder Tod beenden das Zielen sofort.",
    "Beim Zielen durchs Fernrohr tauchen keine Zombies oder Leute mehr plötzlich neben dir auf.",
    "Ein Sniper-Schuss sprengt kein Auto mehr: Karosserie braucht drei Treffer, der Motorraum zwei – erst raucht es, dann knallt es.",
    "Polizei, Militär und Zombies sind weniger tödlich: weniger Schaden, kurze Reaktionszeit, keine Salven mehr.",
    "Jogger joggen jetzt wirklich – mit Laufstil, Sportklamotten und ohne durch Häuser zu laufen.",
    "Polizei, Polizeidrohnen und Militär kommen wieder: du entkommst erst, wenn sie da waren – nicht mehr, bevor das erste Auto eintrifft. Das Militär rückt bei fünf Sternen deutlich früher aus.",
    "Ein Haus wegzuschießen reißt keinen ganzen Block mehr mit: brennende Autos, Trümmer und Polizeiwagen beschädigen keine Häuser mehr, nur noch echte Waffen.",
    "Busse kannst du jetzt genauso fahren wie Autos – mit Außen- und Innenansicht.",
    "Mehr Autos auf den Straßen: Kombis, SUVs, Transporter und ein flacher Elektro-Roadster.",
    "Raketenwerfer: nie mehr blockiert – eine neue Rakete übernimmt den Laserpunkt, die alte fliegt ungelenkt weiter. Der Punkt ist jetzt wirklich rot. Lenkraketen ziehen runde Rauchwolken statt Vierecken.",
    "Die Glock ist feiner gebaut (Leuchtvisier, Auswurffenster, Abzug), der Schlitten fährt bei jedem Schuss sichtbar zurück, und der Schuss knallt satter.",
    "Das Messer hinterlässt Spuren: Wunden und Blut bei Treffern, Schnittkerben an Wänden und Autos.",
    "Die Arme in der Ich-Ansicht verdecken weniger: kürzer, und die zweite Hand kommt von unten statt quer durchs Bild.",
    "Passanten, die die Arme hochreißen, schreien dabei.",
    "Spieler halten etwa 18 % mehr aus.",
    "Zombies brechen nicht mehr sofort durch: eine Barrikade hält, egal wie viele drauf einschlagen, und durch Bretter wird niemand gekratzt. Nachts sind die Eingänge nur noch verbarrikadiert, ohne Türen.",
    "Routenpfeile liegen jetzt als Pfeile auf der Straße statt als flache Symbole im Bild.",
    "Häuser sind überall fest: Autos fahren nicht mehr durch Gebäude (auch nicht in der Innenansicht), Passanten und Tiere laufen nicht mehr hindurch – egal wie weit du dich von deiner Drohne entfernst.",
    "Die Physik repariert sich selbst: ein einzelner kaputter Körper kann die Kollisionen nicht mehr lahmlegen, und sollte die Physik doch einmal abstürzen, wird sie sofort neu aufgebaut.",
    "Tauben! Kleine Trupps laufen in der Stadt herum, nicken, picken und gurren – kommst du ihnen zu nahe, fährt ein Auto vorbei oder knallt es, flattern alle zusammen auf und landen woanders.",
    "Vögel fliegen jetzt mit dem Kopf voran statt seitwärts.",
    "Raketenwerfer: nur noch ein großer roter Laserpunkt, kein Strahl mehr. Auf dem Handy lenkst du die fliegende Rakete mit dem Finger: Tipp auf ein neues Ziel und zieh, die Rakete folgt dem Punkt.",
    "Im Jet gibt es nur noch einen EJECT-Knopf (oben rechts) – der Flugmodus-Knopf unten ist wieder frei erreichbar.",
    "Die Ton- und Musik-Knöpfe oben wackeln nicht mehr.",
    "Sounds sind zurück: Motor, Umgebung und Explosionen setzen nach dem Zurückkehren in die App wieder ein (spätestens beim ersten Tippen).",
    "Straßen und Gehwege verschwinden nicht mehr kurz, wenn irgendwo etwas explodiert.",
    "Neue Hände in der Ego-Sicht und eine schnellere Messer-Animation.",
    "Sniper: Treffer und Einschlag kommen erst an, wenn die Kugel da ist.",
    "Granatwerfer-Explosionen sind wieder deutlich zu hören.",
    "Auf Dachaufbauten (Treppenhäuser) kann man jetzt stehen.",
    "Gebäude sind beim Start nicht mehr schon eingestürzt.",
    "SpawnCity hat jetzt einen eigenen Mehrspieler-Raum.",
    "Bitte die Seite einmal neu laden. Wer noch eine ältere Version offen hat, sieht die anderen erst nach dem Neuladen.",
    "Ab und zu steht irgendwo in der Stadt eine Vierlings-Flak hinter Sandsäcken. Fliegst du mit Drohne oder Jet in ihre Nähe (gut 1 km, Sichtlinie), feuert sie wie verrückt: vier Rohre, Leuchtspur bei jedem Schuss, Sprengwolken um dich herum, harter Sound. Nahe Detonationen beschädigen dich.",
    "Ausschalten: eine Explosion daneben oder ein paar Scharfschützen-Treffer. Alle Spieler finden die Flak an denselben Orten. Im Menü unter WORLD → FLAK abschaltbar.",
  ]},
  {date:"2026-10-15",time:"23:58",title:"spawncity.de ist die Adresse",items:[
    "Das Spiel läuft jetzt unter spawncity.de. Die Startseite geht direkt ins Spiel.",
    "Die Home-Bildschirm-App bleibt an der Adresse, mit der du sie angelegt hast. Einmal unter spawncity.de öffnen und neu auf den Home-Bildschirm legen."]},
  {date:"2026-10-15",time:"23:54",title:"Ping-Markierung, Luftschlag in Spielerfarbe, Voice-Reichweite",items:[
    "PING: zeig deinen Mitspielern eine Richtung oder einen Treffpunkt – PC: Z oder mittlere Maustaste, Xbox: Steuerkreuz rechts, Handy: Bildschirm 0,6 s gedrückt halten. Nochmal auf deinen Ping = weg; nach 90 s verschwindet er von selbst.",
    "Der Luftschlag-Strahl vom Himmel hat jetzt die Farbe des Spielers, der ihn gerufen hat – so ist klar, woher ein roter (oder blauer, grüner …) Strahl kommt.",
    "Vor dem Luftschlag zeigt eine pulsierende blaue Pfeilspur am Boden, in welche Richtung die Bomben fallen.",
    "Einstellungen: VOICE ÜBERALL HÖRBAR – aus (Standard) = nur Mitspieler in der Nähe hören dich, an = wie Funk für alle.",
  ]},
  {date:"2026-10-15",time:"23:49",title:"Trümmer sind fest",items:[
    "Ein weggesprengtes Haus hinterlässt jetzt einen niedrigen Trümmersockel, der genauso fest ist, wie er aussieht: Du, Passanten, Autos und Tiere laufen nicht mehr hindurch.",
    "Auf den Trümmersockel kannst du hochspringen und darauf herumlaufen.",
    "Kühe laufen nicht mehr durch Häuser – auch nicht, wenn sie in Panik davonrennen."]},
  {date:"2026-10-15",time:"23:43",title:"Neuer Name: SpawnCity",items:[
    "Das Spiel heißt jetzt SpawnCity – du spawnst genau dort, wo du gerade stehst, in deiner echten Stadt. Neue Adresse: spawncity.de.",
    "Neuer Titel auf dem Startbildschirm (SPAWN über CITY) und ein neues App-Symbol für den Home-Bildschirm.",
    "Deine Einstellungen, der gewählte Startort und die schon geladenen Karten bleiben beim Umzug auf den neuen Namen erhalten."]},
  {date:"2026-10-15",time:"22:02",title:"Lasergelenkter Raketenwerfer, Autos ohne Ruckeln",items:[
    "Raketenwerfer im Half-Life-2-Stil: ein roter Laserpunkt zeigt, wohin du zielst, und die Rakete fliegt dem Punkt hinterher – du lenkst sie mit dem Zielen. Immer nur eine Rakete gleichzeitig; wechselst du die Waffe, fliegt sie ungelenkt weiter.",
    "Der Raketenwerfer hat ein neues Aussehen (dunkles Metall, Laser-Zielgerät mit rotem Licht), die Rakete fliegt gerade ausgerichtet und zieht eine Rauchspur hinter sich her.",
    "Zerstörte Häuser: an der Stelle eines weggeschossenen Hauses bleibt keine unsichtbare Kollision mehr stehen – kein Hochschleudern mehr beim Springen oder mit dem Jetpack.",
    "Autos ruckeln nicht mehr beim Fahren (Außen- und Innenansicht): Auto, Räder und Kamera werden in jedem Bild an derselben Stelle gezeichnet, auch die Polizeiautos.",
    "Fahrer sitzen tiefer – ihre Köpfe ragen nicht mehr durchs Autodach.",
    "Wird die App geschlossen oder in den Hintergrund gelegt, verstummen Musik und alle Geräusche; beim Zurückkehren geht es genau dort weiter."]},
  {date:"2026-10-15",time:"18:50",title:"Fahrer in den Autos, Autoklau mit Folgen",items:[
    "In jedem fahrenden Auto sitzt jetzt ein Fahrer – durch die getönten Scheiben zu sehen.",
    "Klaust du ein Auto, fliegt der Fahrer raus und steht neben dem Wagen: je nach Typ rennt er weg, schimpft erst und haut dann ab oder geht auf dich los. Leute in der Nähe drehen sich um und schimpfen, ein Zeuge meldet es (ein kleiner Stern).",
    "Ein geräumtes Auto bleibt leer stehen und fährt erst weiter, wenn es keiner mehr sieht. Steigt ein wütender Fahrer aus, steht er sofort sichtbar da.",
    "Selten zieht ein Hitzkopf selbst einen Fahrer aus einem wartenden Auto und rast damit davon – das Opfer flieht, die Umstehenden schreien.",
  ]},
  {date:"2026-10-15",time:"18:45",title:"Echte Panik",items:[
    "Fliehende Passanten laufen jetzt jeder anders schnell. Bei echter Angst (Schüsse nah, Explosion) sprinten sie ein paar Sekunden mit 6–8,5 m/s, viele reißen dabei die Hände über den Kopf. Danach bleiben sie eine Weile schreckhaft und gehen eilig weiter.",
  ]},
  {date:"2026-10-15",time:"18:43",title:"Handy-Knöpfe an den Rand, Autos ohne Ruckeln",items:[
    "Handy: alle Knöpfe sitzen jetzt am Rand, in Reichweite des Daumens, der sie braucht – Luftschlag und Granate über dem linken Stick, Waffe/Springen bzw. ARM/STOP links neben dem rechten Stick, Einsteigen und EMP über dem rechten Stick. Die Bildmitte bleibt frei. Gleich in allen Modi und im Hoch- wie Querformat.",
    "Im Auto: Handbremse, Kamera und Radio rechts über dem Daumen, Aussteigen unten rechts. Mikrofon und Stummschalten wandern in die obere Leiste; GPS, Musik und Ton liegen auch im Hochformat oben rechts.",
    "Autos ruckeln nicht mehr: die Physik läuft mit festen 60 Schritten pro Sekunde, gezeichnet wird jetzt sauber dazwischen – auch auf 90/120-Hz-Displays und bei schwankender Bildrate.",
    "Kein „Dach weg“ mehr: steht hinter dem Auto eine Wand, rutscht die Kamera nicht mehr ins Auto, sondern hebt sich darüber.",
    "Innenansicht: nur noch ein kleines Lenkrad unten im Bild – keine Armaturen, Säulen oder Motorhaube, freie Sicht auf die Straße.",
  ]},
  {date:"2026-10-15",time:"18:15",title:"Echter Standort, Offline-App, ruhigere Stadt, Lenkrakete",items:[
    "Jeder Start beginnt an deinem echten Standort: frischer, genauer GPS-Fix (nie ein alter); kommt der genaue Fix spät, springt die Welt nach. Zurück aus dem Menü: bist du über 400 m weiter, kommt die Welt zu dir. Ein im Menü gesuchter Ort gilt nur noch bis zum Schließen der App.",
    "Installierte App (Startbildschirm): startet auch offline, holt aber online immer sofort die neueste Version. Kartendaten von Orten, an denen du warst, bleiben gespeichert.",
    "Lenkrakete: jeder Druck feuert – mit Ziel in der Nähe des Fadenkreuzes fliegt sie es an, sonst auf den anvisierten Punkt. 5 s Nachladen, sichtbar auf dem Waffenknopf und unter dem Fadenkreuz. Dafür etwas schwächer.",
    "Passanten sind deutlich friedlicher (weniger Hitzköpfe, kaum Waffen, langsamer am Abzug). Unsichtbare Schützen gibt es nicht mehr: nur wer sichtbar ist, kann dich angreifen.",
    "Polizei entspannter: mehr Sterne nötig, weniger Streifen, kommen später, schießen seltener und ungenauer, du entkommst schneller.",
    "Kühe grasen gemütlich auf ihrem Fleck statt über die Weide zu rasen.",
  ]},
  {date:"2026-10-15",time:"00:29",title:"Jet-Sticks zeigen dahin, wo du ziehst",items:[
    "Handy im Hochformat: die beiden Sticks im Jet (und im Auto) lasen bisher die Bildschirm-Achsen statt der gedrehten Spielansicht – deshalb ging der linke Stick „irgendwohin“. Jetzt gilt: wohin du ziehst, dahin geht’s. Der Ausschlag zählt ab da, wo der Daumen aufsetzt (kein Ruck beim Antippen).",
    "Belegung wie bei Flugsimulatoren (Mode 2): links ↑↓ Schub/Steigen, ←→ Seitenruder; rechts Nicken und Rollen.",
  ]},
  {date:"2026-10-15",time:"00:26",title:"Deine Ideen fürs nächste Update",items:[
    "Nach etwa 15 Minuten Spielzeit mit einer Version fragt das Hauptmenü einmal kurz nach deiner Idee für das nächste Update – eine Zeile, anonym, ohne Konto. Alle Vorschläge werden gesammelt und zusammengefasst; „Später“ fragt nach weiteren 10 Minuten nochmal, „Nein danke“ nie mehr für diese Version.",
  ]},
  {date:"2026-10-15",time:"00:23",title:"Spielen, wo du willst",items:[
    "Im Hauptmenü unter START: 📍 Startort. Standard ist dein GPS-Standort – oder du suchst eine Stadt, Straße oder Adresse (mit Vorschlägen beim Tippen) und startest dort. Die letzten Orte bleiben gespeichert; ein neuer Ort während des Spiels gilt beim nächsten START.",
  ]},
  {date:"2026-10-14",time:"23:48",title:"Multiplayer: Blickrichtung, echte Drohnen, kein Rauswurf",items:[
    "Mitspieler schauen jetzt in die Richtung, in die sie wirklich schauen (vorher war die Drehung seitenverkehrt) – auch im Auto per Live-GPS.",
    "Fremde Drohnen sehen aus wie deine eigene: Arme, Motoren, drehende Propeller, Kufen – in der Spielerfarbe an Haube, Motorglocken und LED-Ring.",
    "Mitspieler und ihre Drohnen verschwinden nicht mehr, wenn ihr Handy kurz pausiert oder die Verbindung hakt – sie bleiben sichtbar stehen.",
    "Updates werfen dich nicht mehr aus einer laufenden Runde: ist ein Spiel gestartet, erscheint nur „UPDATE“ (tippen oder U). „Später“ lässt 10 Minuten Ruhe.",
    "Live-GPS auf Android: die Standortabfrage kommt sofort beim Tippen (vorher blieb es ohne Nachfrage bei „Suche GPS“ hängen); drinnen wird nach kurzer Zeit der ungefähre Standort genommen; blockiert ist klar angezeigt.",
  ]},
  {date:"2026-10-14",time:"23:41",title:"Maus-Empfindlichkeit, ADAC zerstörbar",items:[
    "PC: in den Einstellungen gibt es jetzt MAUS-EMPFINDLICHKEIT (20–300 %) und MAUS Y INVERTIEREN – gilt zu Fuß, in der Drohne, im Auto und im Jet.",
    "Der gelbe Abschleppwagen kann jetzt zerstört werden: Explosionen und genug Treffer lassen ihn ausbrennen, sein Auftrag fällt weg.",
  ]},
  {date:"2026-10-14",time:"23:37",title:"Keine Geister-Räder mehr, festere Häuser, Kisten anschießen",items:[
    "Autos aus großer Entfernung (Jet, Fallschirm, Dach) erscheinen nicht mehr als vier lose Räder: die Räder verschwinden jetzt zusammen mit der Karosserie.",
    "Häuser-Kollision: L-förmige Häuser und Höfe bestehen aus wenigen festen Blöcken statt vieler Dreiecke – dadurch sind fast doppelt so viele Häuser rundum fest, und ein Haus ist immer ganz fest, nie nur zur Hälfte.",
    "Kisten, Fässer, Pylonen und Tonnen reagieren jetzt auf jeden Treffer, auch auf die MP – mit dem echten Impuls der Kugel (Scharfschützengewehr wirft sie um, Pistole schubst Pylonen). Im Multiplayer sehen alle den Flug.",
  ]},
  {date:"2026-10-14",time:"23:25",title:"Granatwerfer und Raketenwerfer wieder getrennt",items:[
    "Zu Fuß gibt es wieder beide: der Granatwerfer (5) schießt springende 40-mm-Granaten mit Zeitzünder, der neue Raketenwerfer (6) eine schnelle, flache Rakete mit Feuerschweif, die beim Aufprall explodiert. Fäuste jetzt 7, Messer 8.",
    "Raketenwerfer als eigenes Schulterrohr mit sichtbarem Gefechtskopf; Xbox und Mausrad schalten durch alle Waffen.",
    "PC: die Tastenhilfe liegt unter den Mikrofon-Knöpfen und blendet sich nach 12 s aus – beim Waffen- oder Moduswechsel kommt sie zurück.",
  ]},
  {date:"2026-10-14",time:"23:15",title:"Radio mit SWR, NDR & Co., Effekte-Schalter, Steuerung",items:[
    "Autoradio: die öffentlich-rechtlichen Sender deines Bundeslands (z. B. SWR3, SWR1, NDR 2, Bayern 3, WDR 2, hr3) plus Deutschlandfunk, danach die Lokalsender. Läuft beim Einsteigen, unabhängig vom Musik-Schalter.",
    "Radio aus: in der Senderliste gibt es den Platz „RADIO AUS“ (📻 tippen bzw. N/B), 📻 halten schaltet direkt aus. Der Motor ist leiser und tritt unter dem Radio zurück.",
    "„Effekte aus“ schaltet jetzt wirklich alles stumm – auch Düsenjet, Sirenen, Tiere und Motoren.",
    "Handy: Schießen funktioniert bis direkt an die Sticks heran – die Stickfläche ist genau der gezeichnete Kreis.",
    "Kein Airstrike mehr, solange man tot ist. Respawn am PC jetzt auch mit der Leertaste.",
    "Nach dem Schleudersitz verschwinden die Jet-Knöpfe (Flug/Rakete/Bombe).",
    "Weit entfernte Personen sind Silhouetten statt Klötze und erst ab 130 m vereinfacht.",
    "Klick neben das Hauptmenü löst keinen Schuss mehr aus. „1 Runde überlebt“ statt „1 Runden“.",
    "PC: die Tastenhilfe sitzt oben, unten bleibt Platz für ARM/STOP und Fahrzeug-Tasten.",
  ]},
  {date:"2026-10-13",title:"Passanten sprinten jetzt panisch",items:["Bei Schüssen und Explosionen fliehen Passanten jetzt im echten Sprint mit längeren Schritten, stärkerem Beinabstoß und vorgebeugtem Oberkörper."]},
  {date:"2026-10-13",title:"ARCADE JET, Voice Toggle und übersichtlicheres Gameplay",items:[
    "VOICE: Mikrofon ist jetzt ein echter EIN/AUS-Schalter statt Push-to-Talk. Tippen bzw. H oder Xbox R3 schaltet dauerhaft ein und wieder aus; ohne Freigabe bleibt es aus. Mitspielerpositionen werden für die 70-m-Reichweite korrekt übertragen.",
    "Die mobilen Voice-Tasten sitzen tiefer an der Daumen-Zone und meiden weiterhin Joysticks, Fahrzeugaktionen und das Sichtfeld.",
    "Jet-Steuerung reagiert schneller, mit deutlicherem Nick- und Rollverhalten; die Box3D-Kollisionen, Aerodynamik und alle Personen bleiben erhalten.",
    "Jet-Raketen: erst RAKETE wählen (1), dann auf Ziel tippen bzw. auslösen (2). Danach 2,4 Sekunden Nachladezeit mit Anzeige; kein versehentlicher Geschützschuss während des Abschusses.",
    "Mobile EJECT ist weg von den Daumentasten und erfordert 650 ms bewusstes Halten. JET-EINSTEIGEN ist leicht nach unten versetzt.",
    "Die Taste FPS / DRONE ist kürzer und steht weiter links. Die große Karte kann direkt über das ⛶-Symbol aus allen Spielmodi geöffnet werden.",
    "Der Standard-8-Bit-Song läuft ausschließlich im Titelmenü. Im Spiel bleiben Radio und Effekte. Regelmäßige Düsenjäger sind 15 % leiser.",
    "Dynamische Physikschritte holen bei schwankenden Mobile-FPS Rückstand auf; die Jet-HUD-Synchronisation wurde entlastet, ohne Weltumfang zu streichen.",
  ]},
  {date:"2026-10-12",title:"Voice-Chat für Maus, Tastatur und Xbox-Controller",items:[
    "PC: Mikrofon mit H gedrückt halten, Stummschaltung mit K umschalten. Grössere Voice-Felder zeigen die Belegung direkt im HUD und sind mit der Maus bedienbar.",
    "Xbox: rechten Stick (R3) gedrückt halten zum Sprechen, R3 halten und Y drücken zum Stummschalten. Y wechselt dabei keine Waffe.",
    "Die Voice-Steuerung nutzt in SOLO und MULTI dieselbe Logik. Beim Loslassen, im Menü und beim Controller-Verlust endet die Mikrofonaufnahme zuverlässig.",
  ]},
  {date:"2026-10-12",title:"Voice-Buttons weichen Fahrzeug-Aktionen aus",items:[
    "Erscheint beim Gehen spontan EINSTEIGEN für ein Auto oder einen Jet, bleiben Mikrofon und Stumm-Taste sichtbar. Sie suchen einen freien Platz am Rand statt zu verschwinden.",
    "Auch SCORE/FLOW, Waffen und Daumensteuerung werden bei der Positionierung berücksichtigt; die Mitte bleibt zum Zielen frei.",
  ]},
  {date:"2026-10-12",title:"Mobile: Voice bleibt frei von Diagnose-Einblendungen",items:[
    "Das kleine Build-/Online-Label überdeckt beim Spielen keine Mikrofon- und Stumm-Taste mehr. Die Versionsprüfung läuft im Hintergrund weiter.",
  ]},
  {date:"2026-10-12",title:"Voice-Chat: gleiche Bedienung in SOLO und MULTI",items:[
    "Mikrofon (Push-to-Talk) und Stummschaltung bleiben auch allein im Spiel sichtbar und nutzbar, unabhängig vom Multiplayer-Modus.",
    "SOLO startet keine öffentliche Mitspielersuche. Erst MULTI verbindet öffentlich; Voice überträgt nur an tatsächlich verbundene Mitspieler in Reichweite.",
    "Das Mikrofon öffnet sich nur bei gedrücktem PTT und wird beim Loslassen wieder vollständig freigegeben. Ein RESET ändert die gewählte Stummschaltung nicht.",
    "Die mobilen Voice-Tasten meiden Bewegungs-, Waffen-, Granaten- und Menütasten, damit die Sicht zum Spielen frei bleibt.",
  ]},
  {date:"2026-10-12",title:"Mobile: kompakte Hochformat-Tasten sauber getrennt",items:[
    "Auch bei 360 Pixel breiten Handys mit gedrehter Spielansicht berühren sich FLOW und GRANATE nicht mehr; die Tasten und die freie Sicht bleiben an ihrem Platz.",
  ]},
  {date:"2026-10-12",title:"Mobile: mehr Sicht, weniger HUD-Überlagerungen",items:[
    "Schmale Querformate und Safari-Browserleisten: STRIKE und GRANATE verdecken keine Punkte- oder FLOW-Anzeige mehr. Auf besonders flachen Displays tritt die optionale Punkteanzeige zurück.",
    "Im Multiplayer verschwindet die doppelte Solo-Punkteanzeige; Mitspieler-Namensschilder bleiben über den ARM-, STOP- und Daumentasten.",
    "Die mobile Sichtprüfung kontrolliert zusätzlich Hoch- und Querformat, kurze Bildschirme, Drohne, Fußmodus und die freie Bildschirmmitte.",
  ]},
  {date:"2026-10-12",title:"PC: Waffenwechsel beim Start zuverlässig sichtbar",items:[
    "Auf PCs ohne Touchscreen startet die passende Maus-/Tastatur-Steuerung auch dann korrekt, wenn der Browser den Mauszeiger nicht als primäres Eingabegerät meldet.",
    "Die Waffenwahl wird dadurch nicht mehr versehentlich wie auf einem Handy ausgeblendet; der automatische Desktop-Test prüft jetzt auch den tatsächlichen Eingabemodus.",
  ]},
  {date:"2026-10-12",title:"Gebäudezerstörung und Kollisionen repariert",items:[
    "Komplett zerstörte Häuser blockieren nicht mehr unsichtbar: Ihre alten Physik-Körper werden vollständig entfernt.",
    "Beim Nachladen von Häusern bleiben Zerstörung und beschädigte Dächer erhalten – auch wenn sich die Kartenkacheln ändern.",
    "Wände und Dächer werden in der Physik nach Beschädigung korrekt neu aufgebaut. RESET stellt unbeschädigte Gebäude wieder her.",
    "Stabilere Box3D-Kollisionen durch lokale Gebäude-Geometrie und präzise Aktualisierung beim Fahren und Fliegen.",
  ]},
  {date:"2026-10-12",title:"Handy: kleine Fahrzeugtasten am Daumen",items:["Auto und Jet erscheinen als zwei getrennte runde Symbole über dem rechten Stick – auch wenn beide in Reichweite sind. Die Bildschirmmitte bleibt frei, keine Bedienelemente überlappen."]},
  {date:"2026-10-12",title:"Desktop: erster Rechtsklick aktiviert Zielen und Schießen",items:["Wer zuerst RMB zum Zielen gedrückt hält, aktiviert damit die Maussteuerung und kann sofort mit LMB schießen. Schießen und Zielen bleiben unabhängig; auch der erste Schuss geht nicht verloren."]},
  {date:"2026-10-12",title:"Jet-Einstieg per F und freiere Mobile-Sicht",items:["F steigt zu Fuß in einen erreichbaren Jet ein. Die Zombie-Reparatur blockiert F nur, wenn sie wirklich möglich ist.","Jet- und Auto-Einstieg liegen auf dem Handy an den Rändern statt vor dem Fadenkreuz; Sprachknöpfe weichen ihnen aus."]},
  {date:"2026-10-12",title:"Mobile: Raketen-Abschuss und Waffenwahl stabil",items:["GUN, ROCKETS und GUIDED haben jetzt einen eindeutigen Schussweg. Ein Raketenabschuss nutzt zuverlässig die feste Bildschirmrichtung und funktioniert erst nach Waffenwahl und Freigabe."]},
  {date:"2026-10-12",title:"Handy: HUD bleibt außerhalb der Daumen und der Bildmitte",items:["Voice-Knöpfe erscheinen nur im verbundenen Multiplayer und finden am linken oder rechten Bildschirmrand einen freien Platz knapp über den Sticks. Alle Button-Ziele bleiben getrennt; der Moduswechsel hat jetzt Abstand zum linken Stick."]},
  {date:"2026-10-12",title:"Desktop: Schießen während Zielen repariert",items:["Rechte Maustaste zum Zielen gedrückt halten und gleichzeitig mit links schießen funktioniert wieder – zu Fuß und in der Drohne. Auch Fäuste behalten getrennte linke und rechte Schläge."]},
  {date:"2026-10-11",title:"HUD aufgeräumt: Regeln ins Menü, Voice oben",items:["Abstimmungen erscheinen nur noch im WORLD-Menü bzw. der Pause-Menü-Sektion; sie verdecken nicht mehr das Spiel. PTT und Stummschaltung sind runde Symbole oben, außerhalb der Bewegungssticks."]},
  {date:"2026-10-11",title:"Multiplayer: Sprachchat in der Nähe",items:["Halte 🎙 PTT oder V zum Sprechen gedrückt; Mikrofon erst nach Freigabe. Nahe Mitspieler hören dich leiser mit zunehmender Entfernung; MUTE schaltet ab."]},
  {date:"2026-10-11",title:"Multiplayer: RULES-Abstimmung",items:["Änderungen an Friendly Fire brauchen jetzt die Zustimmung aller verbundenen Spieler. Bei NEIN, Zeitablauf oder Spielerwechsel bleibt die bisherige Regel bestehen."]},
  {date:"2026-10-11",title:"Multiplayer: Friendly Fire",items:["Im WORLD-Menü kann der erste Spieler Friendly Fire einschalten oder ausschalten. Ohne Freigabe verletzen Waffen keine Mitspieler."]},
  {date:"2026-10-11",title:"GUIDED: fairer und ausweichbar",items:["Lenkraketen sind langsamer, schwächer und seltener. Lock-on erhält eine sichtbare Warnung; sprintende Fußgänger können die Zielverfolgung abschütteln."]},
  {date:"2026-10-11",title:"NUKE vollständig entfernt",items:["Atomwaffen sind aus Drohne, Raketenwerfer, Menüs und WORLD-Regeln entfernt. Normale Explosionen und Gebäudeschaden bleiben erhalten."]},
  {date:"2026-10-11",title:"Minimap: Double-Tap auch zu Fuß",items:["Die Minimap lässt sich per Doppeltipp jetzt auch zu Fuß und in der First-Person-Ansicht vollständig vergrößern; Auto, Jet und Drohne bleiben unverändert."]},
  {date:"2026-10-11",title:"Multiplayer: Host-Verbindung stabilisiert",items:["Beim Beitreten wird die Host-Reihenfolge nach Aufbau der Verbindung bestätigt; alle Spieler erkennen denselben Host."]},
  {date:"2026-10-11",title:"Multiplayer: Erster Spieler leitet die Runde",items:["Der zuerst beigetretene Spieler behält die Spielleitung – unabhängig von seiner Netzwerk-ID. Beim Verlassen übernimmt der nächste Spieler."]},
  {date:"2026-10-11",title:"Multiplayer: Spawn bei der Spielergruppe",items:["Neue Mitspieler erscheinen bei der groessten aktiven Spielergruppe statt an einer weit entfernten Startposition."]},
  {date:"2026-10-11",title:"Multiplayer: alle Spieler suchen dieselbe Lobby",items:[
    "Die automatische Spielersuche verwendet nun denselben öffentlichen Raum statt unterschiedliche GPS- oder Zeit-Räume.",
  ]},
  {date:"2026-10-11",title:"Live-Prüfung: Gelände, Update und Handy-Steuerung",items:[
    "Die Live-Prüfung testet echte Geländehöhen und prüft den Update-Hinweis nach einer neuen Versionsabfrage zuverlässig.",
    "Handy- und Desktop-Tests prüfen die sichtbaren Waffen und den gedrehten MODE-Knopf in der tatsächlichen Bediengröße.",
  ]},
  {date:"2026-10-11",title:"Terrain: Live-Prüfung der echten Karte",items:["Gelände und Moduswechsel werden zusätzlich direkt auf der veröffentlichten echten Karte geprüft, einschließlich Update-Hinweis für laufende Spiele."]},
  {date:"2026-10-11",title:"Terrain: stabile Höhen beim Nachladen",items:[
    "Unvollständige Höhendaten verbiegen den Boden nicht mehr; Straßen und Bodenkollision folgen gemeinsam der aktuellen Höhe.",
    "Beim Wechsel der Stadt werden verspätete Höhen verworfen. Auch weit entfernte Mitspieler behalten dieselbe Höhenreferenz.",
  ]},
  {date:"2026-10-11",title:"Terrain: Boden erscheint direkt beim Moduswechsel",items:[
    "Der echte Boden wird sofort geladen; Satellitenfarben folgen später, ohne Bodenlöcher oder weiße Flächen beim Umschalten.",
  ]},
  {date:"2026-10-11",title:"Multiplayer: flüssigere Auto- und Jet-Marker",items:[
    "Die Symbole anderer Autos und Jets bewegen sich auf der Karte ruhiger und springen bei einem Fahrzeugwechsel nicht quer über die Straße.",
  ]},
  {date:"2026-10-11",title:"Multiplayer: Mitspieler auch außerhalb der Karte finden",items:[
    "Pfeile am Kartenrand zeigen jetzt in die tatsächliche Richtung des Mitspielers – nicht immer nach Nordost.",
  ]},
  {date:"2026-10-11",title:"AR: Wegpfeile bleiben beim Umschauen auf der Strecke",items:[
    "Die 3D-Wegpfeile folgen jetzt jeder Kamerabewegung, während die Straßenroute sparsam berechnet wird.",
  ]},
  {date:"2026-10-11",title:"AR: sichere Routensuche und Abbiegehinweise",items:[
    "Beim Wechsel des Ziels oder Fahrmodus überschreibt keine verspätete Routenantwort die neue Strecke.",
    "Die Navigation kündigt Links- und Rechtskurven an, ohne jeden kleinen Straßenknick als Abzweigung zu behandeln.",
  ]},
  {date:"2026-10-11",title:"AR: Wegpfeile entlang der echten Straßen",items:[
    "Beim Gehen und Fahren zeigen mehrere Pfeile entlang der berechneten Strecke den Weg.",
    "Drohne und Jet bleiben bei der direkten Zielführung; die Mobile- und Desktop-Tests wurden an den neuen Bedienungsaufbau angepasst.",
  ]},
  {date:"2026-10-11",title:"Multiplayer: zwei Spieler in einem Jet",items:[
    "Ein Spieler fliegt als Pilot, ein zweiter kann als Passagier mitfliegen; die Sitzplätze werden innerhalb der Mehrspieler-Runde abgestimmt.",
    "Alle Mitspieler erscheinen auf der großen Karte; die Passagierfigur wird auch im Flug am Jet angezeigt.",
  ]},
  {date:"2026-10-11",title:"Multiplayer: Jet bleibt stehen, Mitspieler auf der Minimap",items:[
    "Geparkte Jets springen nicht mehr ständig zur Seite, wenn man einsteigen will.",
    "Alle verbundenen Spieler werden auf der Minimap mit Position und Blickrichtung angezeigt.",
  ]},
  {date:"2026-10-10",title:"Flüssiger im Jet und bei hohem Tempo",items:[
    "Häuser-Kollisionen werden beim schnellen Fliegen und Fahren nicht mehr komplett neu gebaut: unveränderte Häuser behalten ihre Physik, nur neue kommen dazu (vorher alle paar hundert Millisekunden alles, in drei Physik-Welten).",
    "Grundrisse werden nur einmal trianguliert; Bäume werden während des Jetflugs nicht ständig neu verteilt.",
    "Live-Anzeigen (Jet-HUD, Navigation) lösen keine Neu-Scans der Oberfläche mehr aus – vorher liefen mehrere DOM-Beobachter pro Bild.",
    "EXIT und MULTI überlappen am PC nicht mehr.",
  ]},
  {date:"2026-10-10",title:"Vollbild-Karte mit Zielsuche, einheitliche Bedienung, Jet-HUD",items:[
    "Vollbild-Karte wie in GTA: Minimap doppelt antippen, M oder VIEW gedrückt halten (Xbox). Ziehen, zoomen, tippen = Wegpunkt; ✕ / M / Esc / Ⓑ schließt.",
    "Ziel suchen: Adresse eintippen (z. B. „Dorfstraße 8, Kirchberg an der Jagst“) – Route über Straßen (zu Fuß über Fußwege), dezenter Richtungspfeil am Horizont und eine Zeile mit Entfernung. Ohne Ziel wird nichts angezeigt.",
    "Überall gleich: EXIT oben links (mit Nachfrage), MULTI direkt daneben. Der Modus-Knopf sitzt unten links am Daumen und zeigt, wohin er wechselt: TO DRONE, GO ON FOOT, EJECT.",
    "Jet: grünes, dezentes HUD – Stadtname mit Linie zum Stadtzentrum, Geschwindigkeit links, Höhe rechts, Flugbahnmarke. Auf Touch schießt die Kanone dorthin, wo du tippst.",
    "Jets stehen nicht mehr in Häusern: der Abstellplatz braucht freien Platz für Flügel und Heck und rückt nach, wenn Häuser nachladen.",
    "Zielfernrohr: feineres Absehen mit Vergrößerungsanzeige; Einschläge, Splitter und Blut bleiben auch auf große Entfernung sichtbar.",
    "Bäume stehen nicht mehr in Häusern oder auf der Fahrbahn.",
    "Schnellerer Start: die Umgebungsbeleuchtung braucht den schweren Himmels-Shader nicht mehr.",
  ]},
  {date:"2026-10-10",title:"Polizei rückt aus, echte Sirenen",items:[
    "Randale zu Fuß ruft die Polizei: Schüsse in der Nähe von Leuten, Faustschläge und Messerstiche bringen Sterne – Streifenwagen und Polizeidrohnen kommen.",
    "Streifenwagen lassen sich zerschießen: sie qualmen, brennen und explodieren.",
    "Echte Sirenen: Streifenwagen mit deutschem Martinshorn (Tatütata), Polizeidrohnen mit leisem elektronischem Heulton – räumlich, mit Doppler, dezent im Mix.",
  ]},
  {date:"2026-10-10",title:"Steuerung wie CoD, schlauere Fahrer und Passanten",items:[
    "Zielen wie in Call of Duty: Mobile-Daumen 1:1 ohne Totzone und schneller, PC-Maus roh ohne Beschleunigung, Xbox mit „Dynamic“-Kurve.",
    "Ein Glock-Schuss sprengt kein Auto mehr; keine Kettenexplosionen die Straße runter.",
    "Zombies und Passanten in deiner Nähe sind keine grauen Blöcke mehr.",
    "Fahrer halten Abstand, bremsen vor Kurven, hupen seltener. Gerammt oder beschossen: Panik (Rückwärtsgang, Vollgas weg), Angriff (versuchen dich zu überfahren) oder Wutbürger steigt aus – manchmal mit Messer oder Pistole.",
    "Passanten hören jeden Schuss in der Nähe, sprinten erst los, laufen nicht mehr durch Wände, prügeln sich manchmal untereinander und flüchten vor tief fliegenden Drohnen und Jets.",
    "Fäuste treffen zuverlässig (auch Hunde, Katzen, Kühe), Tiere fliegen nicht mehr übers Eis und stehen wieder auf; die schwarze Katze lebt nach ihrem Angriff weiter.",
    "Neue, rundere Fäuste.",
  ]},
  {date:"2026-10-10",title:"Nukes per Schalter, LIVE GPS, Kühe, Boxen",items:[
    "Hauptmenü → WORLD: DRONE NUKE und LAUNCHER NUKE (Standard aus). Im Multiplayer gelten die Schalter des Hosts.",
    "Raketenwerfer feuert ohne Nuke normale Raketen – mit LAUNCHER NUKE die Atombombe.",
    "Changelog im Hauptmenü.",
    "Drohnen-LENKRAKETE: Ziel markieren (Mensch, Tier, Vogel, Auto, Drohne), nochmal drücken – Marschflugkörper mit Triebwerkssound, Reiseflug und Sturzflug.",
    "Autoradio wie in GTA – mit den echten Lokalsendern der Region (am Bodensee die Bodensee-Sender). 📻 / N / B / Steuerkreuz.",
    "Die Laufgruppe: ab und zu joggt eine Gruppe im Gleichschritt mit eigenem Schlachtruf durch die Straßen – mit Streak fürs Überfahren.",
    "Messer als eigene Waffe (Taste 7, im Waffenwechsel nach den Fäusten).",
    "Gangster: wenige Passanten tragen ein Messer, ganz selten eine Pistole – und kommen auf dich zu, statt wegzurennen.",
    "RESET räumt alles auf: Laternen, Kühe, Autos, Passanten, Kisten, Granaten in der Luft.",
    "Explosionsspuren dezenter; Ruß und Löcher verschwinden mit der eingestürzten Wand.",
    "Raketen im Flug und Leuchtspur werfen echtes Licht; Asphalt hat Flicken und Körnung statt einer Einheitsfarbe.",
    "Auto-Innenansicht: die Frontscheibe ist das Bild – schmale Säulen, flaches Armaturenbrett, Motorhaube in Wagenfarbe, Lenkrad unten.",
    "LIVE GPS (Taste L): die Welt folgt deiner echten Position – Joggen, Rad, Auto. Im Multiplayer sehen die anderen dich laufen, Rad fahren (mit Fahrrad) oder im Auto.",
    "Kühe auf den echten Weiden: physisch, treffbar, mit Muhen und Schmerzlaut.",
    "Passanten haben Temperament: meist ängstlich, manche rennen weg, wenige schlagen zurück; sie schreien, fluchen und beruhigen sich wieder.",
    "Panik nur in der Nähe von Schüssen – nicht mehr die ganze Stadt.",
    "Boxen: zwei unabhängige Fäuste, Kombos, Wusch- und Treffergeräusche; Tiere reagieren auf Schläge.",
    "Handgranaten (T / LB): Stift ziehen, kochen, werfen.",
    "Eine Granate anschießen zündet sie sofort; Werfergranaten explodieren an Gebäuden, Menschen, Laternen und Tieren, auf dem Boden springen sie.",
    "Echtes Licht von Laternen, Scheinwerfern, Mündungsfeuer, Explosionen und Jetpack.",
    "Jetpack mit Zündung, Schubdröhnen und 3D-Sound anderer Spieler.",
    "Die schwarze Katze faucht und kreischt beim Angriff.",
    "Mobile: kleinere Sticks in den Ecken, nur ein Waffenknopf, JUMP links davon, Scope darüber; Touch-Schüsse gehen exakt auf den Finger.",
  ]},
  {date:"2026-10-09",title:"Fäuste, Autos, Multiplayer-Körper",items:[
    "Fäuste für dich – und für Passanten, die genug haben.",
    "Jedes Auto, in das du steigst, fährt; Cockpit-Ansicht (C).",
    "Drohne feuert Raketensalven; Werferraketen explodieren beim Aufprall.",
    "Tag und Nacht sind im Multiplayer für alle gleich; Esc fragt „zurück ins Hauptmenü?“.",
    "Multiplayer: Trefferzonen, Ragdolls mit Todeskamera, Jetpack repliziert, Mitspieler überfahren, Gravity Gun auf alles.",
    "Klassische Glock, saubere Luftschlag-Zielwahl, Reset-Abstimmung per Desktop und Pad.",
  ]},
  {date:"2026-10-08",title:"Laternen, Zombie-Nächte, Polizei, Performance",items:[
    "Straßenlaternen an jeder echten Straße mit Lichtkegeln bei Nacht.",
    "Zombie-Nächte, Schwingtüren mit Barrikaden.",
    "Reset: allein = ganzes Level zurück; mit anderen = Abstimmung (ein Nein bricht ab).",
    "GTA-Polizei am Boden: Streifenwagen jagen dich, Beamte steigen aus und schießen.",
    "Kampfjets mit Luftschlag; ferne Passanten als Impostor.",
    "Autos mit echter Federung; geparkte Autos bleiben stehen und sind repliziert.",
    "Glasfenster von innen, Einschussrisse in Scheiben, stabile Schatten.",
    "3D-Kreuzblick-Modus in den Einstellungen.",
    "Performance: Boden in 64 Chunks, Straßennetz viel leichter, nur sichtbare Menschen werden gezeichnet.",
  ]},
  {date:"2026-10-06",title:"OPPENHEIMER",items:[
    "Neuer Name OPPENHEIMER; als App auf den Home-Bildschirm installierbar, immer der neueste Stand.",
    "Klassische Pilzwolke; jede Explosion kann Gebäude zerstören.",
    "Drohne regeneriert nach Zerstörung; Panik in der Fallout-Zone.",
    "Nahtloser Übergang Menü → Spiel, Menü-Flyover, Sound-Schalter.",
    "Performance: eine 3D-Ansicht statt zwei, weniger Speichermüll in Verkehr und Physik.",
  ]},
  {date:"2026-10-02",title:"Nuke und Mobile-Multiplayer",items:[
    "Drohnen-Nuke mit Druckwelle, Kamerabeben und Pilzwolke.",
    "Drohnen-Waffenwahl auf dem Desktop; STOP- und EMP-Knopf, Reset auf Mobile.",
    "Mobile-Multiplayer findet Mitspieler zuverlässig.",
  ]},
  {date:"2026-08-24",title:"Zu Fuß in der Ego-Perspektive",items:[
    "Aussteigen und zu Fuß: Dual-Stick-Steuerung, Pistole und MP, Touch-Feuer.",
    "Xbox-Controller: klassischer Flug- und Zielmodus.",
    "Belebte Stadt: Passanten, Verkehr und Tiere.",
    "Audio-Einstellungen; realistische FPV-Kamera.",
  ]},
  {date:"2026-08-17",title:"Die echte Welt",items:[
    "Echte Luftbilder und Gebäude mit Kollision auf der WORLD-Karte.",
    "Immer Querformat, Minimap von oben, Xbox-Steuerung.",
  ]},
  {date:"2026-08-12",title:"Multiplayer VS",items:[
    "Mitspieler in der Nähe und im WLAN finden sich automatisch; verschlüsselte Verbindung.",
    "Kampf-HUD, Explosionen und Respawn.",
  ]},
  {date:"2026-08-10",title:"Hardware-treuer Drohnensimulator",items:[
    "Flugregler wie auf dem echten ESP32 (1 kHz), GAME-Modus bis 90 km/h.",
    "Stick-Invertierungen, Schüsse exakt auf den Touchpunkt, Einschusslöcher.",
  ]},
]);
const CSS=`
#gameMenuChangelog{margin-top:4px;min-width:min(64vw,300px);padding:9px 24px;font:800 clamp(12px,1.6vw,14px) "Inter",system-ui,sans-serif;letter-spacing:.24em;color:#d8dde2;background:#ffffff0c;border:1px solid #ffffff30;border-radius:10px;cursor:pointer}
#gameMenuChangelog:hover,#gameMenuChangelog:focus-visible{background:#ffffff20;outline:none}
#changelogPanel{position:absolute;inset:0;z-index:5;display:flex;align-items:center;justify-content:center;background:#0b0d10e8;backdrop-filter:blur(4px)}
#changelogPanel[hidden]{display:none}
#changelogPanel .cl-card{width:min(94vw,560px);max-height:calc(100dvh - 32px);display:flex;flex-direction:column;padding:16px 16px 12px;border:1px solid #ffffff2e;border-radius:14px;background:#12161ce6;text-align:left}
#changelogPanel h2{margin:0 0 10px;font:900 italic 22px/1 "Inter",system-ui,sans-serif;letter-spacing:.06em;color:#ffb02e}
#changelogPanel .cl-list{overflow:auto;flex:1;padding-right:4px;overscroll-behavior:contain}
#changelogPanel section{margin:0 0 12px}
#changelogPanel h3{margin:0 0 5px;font:850 13px/1.2 system-ui,sans-serif;letter-spacing:.06em;color:#eef0f2}
#changelogPanel h3 span{color:#ff9a3a;margin-right:8px;font-variant-numeric:tabular-nums}
#changelogPanel section:first-child h3::after{content:"NEU";margin-left:8px;padding:1px 6px;border-radius:5px;background:#ff8a1f;color:#16120c;font-size:10px;letter-spacing:.14em;vertical-align:1px}
#changelogPanel ul{margin:0;padding-left:18px}
#changelogPanel li{margin:2px 0;font:500 12.5px/1.38 system-ui,sans-serif;color:#c3ccd4}
#changelogPanel .cl-done{display:block;width:100%;margin-top:10px;padding:12px;border:0;border-radius:10px;background:linear-gradient(180deg,#ffb02e,#ff7a1a);color:#16120c;font:850 15px/1 system-ui,sans-serif;letter-spacing:.24em;cursor:pointer}
#changelogPanel .cl-done:focus-visible,body.pad-input #changelogPanel :focus{outline:3px solid #ffd76a;outline-offset:2px}`;
const esc=t=>String(t).replace(/[&<>"]/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;"}[c]));
const fmt=d=>{const[y,m,dd]=d.split("-");return`${dd}.${m}.${y}`;};
let panel=null;
function open(){if(!panel)return;panel.hidden=false;panel.querySelector(".cl-list").scrollTop=0;panel.querySelector(".cl-done")?.focus({preventScroll:true});}
function close(){if(!panel)return;panel.hidden=true;document.getElementById("gameMenuChangelog")?.focus({preventScroll:true});}
function scroll(dy){panel?.querySelector(".cl-list")?.scrollBy({top:dy,behavior:"smooth"});}
function mount(){
  const menu=document.getElementById("gameMenu"),anchor=document.getElementById("gameMenuWorld")||document.getElementById("gameMenuStart");if(!menu||!anchor)return false;
  if(document.getElementById("gameMenuChangelog"))return true;
  const st=document.createElement("style");st.dataset.changelogMenu=CHANGELOG_MENU_VERSION;st.textContent=CSS;document.head.appendChild(st);
  const btn=document.createElement("button");btn.id="gameMenuChangelog";btn.type="button";btn.textContent="CHANGELOG";anchor.after(btn);
  panel=document.createElement("div");panel.id="changelogPanel";panel.hidden=true;panel.setAttribute("role","dialog");panel.setAttribute("aria-label","Changelog");
  panel.innerHTML=`<div class="cl-card"><h2>CHANGELOG</h2><div class="cl-list" tabindex="-1">${CHANGELOG.map(u=>`<section><h3><span>${fmt(u.date)}${u.time?` · ${esc(u.time)}`:""}</span>${esc(u.title)}</h3><ul>${u.items.map(i=>`<li>${esc(i)}</li>`).join("")}</ul></section>`).join("")}</div><button type="button" class="cl-done">OK</button></div>`;
  menu.appendChild(panel);
  btn.addEventListener("click",e=>{e.preventDefault();open();});
  panel.addEventListener("click",e=>{if(e.target.closest?.(".cl-done")||e.target===panel)close();});
  panel.addEventListener("keydown",e=>{if(e.key==="Escape"){e.preventDefault();e.stopPropagation();close();}else if(e.key==="ArrowDown"||e.key==="ArrowUp"){e.preventDefault();scroll(e.key==="ArrowDown"?120:-120);}});
  globalThis.__arondightChangelogMenu={open,close,scroll,get isOpen(){return!panel.hidden;},entries:CHANGELOG};
  return true;
}
function wait(){if(!mount())setTimeout(wait,200);}
if(typeof document!=="undefined"){if(document.readyState==="loading")document.addEventListener("DOMContentLoaded",wait,{once:true});else wait();}
