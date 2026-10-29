# 11 Kern Studio

Kern Studio ist der Editor der Kern-Engine. Er arbeitet wie die Unreal Engine (Kern ist kein Produkt von Epic Games): Ein Level besteht aus
Actors, jeder Actor trägt Komponenten, man bearbeitet ihn in Viewports mit Unreal-Navigation und
Transform-Gizmos, in Outliner, Details und **Welteinstellungen** (World Settings). **Spielen** (Play) und
**Simulieren** (Simulate) laufen direkt im Viewport, und ein fertiges Level wird mit einem Klick zu einer
einzigen HTML-Datei, die überall läuft.

Das Besondere an Studio steckt unter der Oberfläche: Die gesamte Editorlogik ist C++ und läuft im selben
WebAssembly-Modul wie Renderer, Box3D-Physik und Spiel-Laufzeit. Was man im Viewport sieht, rendert und
simuliert genau der Code, der später das exportierte Spiel ausführt. Die Browserseite zeichnet nur Panels
und reicht Eingaben weiter (Abschnitt 11.19).

Dieses Kapitel ist das Benutzerhandbuch. Wie man daraus ein eigenes Spiel macht, zeigt
[Kapitel 12](12-eigene-spiele.md); die Logik-Graphen beschreibt [Kapitel 9](09-logik.md), die Physik
[Kapitel 6](06-physik.md), den Ton [Kapitel 7](07-audio.md), die Spiel-Laufzeit hinter **Spielen**
[Kapitel 8](08-spiel-laufzeit.md) und den Roboter Micro Duck [Kapitel 13](13-micro-duck.md).

![Kern Studio mit dem Level Orb Run: Menüleiste, Toolbar, Viewport, Outliner und Details](bilder/studio.png)

Pfade wie `kern/src/studio/ui.cpp` beziehen sich in diesem Kapitel auf das Wurzelverzeichnis des
Repositorys, in dem die Engine im Ordner `kern/` liegt.

---

## 11.1 Studio starten

Es gibt drei Wege, Studio zu öffnen:

| Weg | Was man tut | Wofür |
|---|---|---|
| Live | auf https://kernengine.de/ den Pfad `/editor/` öffnen | sofort loslegen, auch auf Telefon und Tablet |
| Eine Datei | `tools/build_studio.sh` bauen, dann `generated/studio.html` öffnen | offline, ohne Server; die Datei enthält alles |
| Entwicklung | Repository statisch ausliefern, `kern/web/studio/index.html` öffnen | am Studio selbst arbeiten (lädt `generated/kern_studio.wasm`) |

Die Startseite des Projekts (https://kernengine.de/) verlinkt Studio, den Lehrer, die
beiden Beispielspiele und zwei Levels zum direkten Spielen (`kern/web/studio/play.html?level=orbRun` bzw.
`?level=physicsLab`). Ihre Fußzeile nennt den Rechteinhaber („© 2026 Christian Heinrich Hohlfeld“) und verlinkt die Lizenz
(`LICENSING.txt`, Apache 2.0), die Bestandteile Dritter (`LICENSES.txt`), den Quellcode, die Studio-Doku,
die Engine-Doku, das Impressum und die Datenschutzerklärung.

**Was beim Start passiert:**

1. Ein Splash-Bildschirm „Engine startet“ („Starting the engine“) erscheint, während das
   WebAssembly-Modul lädt.
2. Beim allerersten Start ist Orb Run geladen: eine prozedurale Stadt mit Verkehr, einem
   Box3D-Hovercar, 24 Energie-Orbs und einem Tag, der in die Nacht übergeht. Das **Ausgabeprotokoll**
   (Output Log) meldet „Kern Studio ready - Orb Run loaded" (das Protokoll selbst bleibt englisch).
3. Bei jedem weiteren Start öffnet Studio das Level, an dem man zuletzt gearbeitet hat, in seinem
   gespeicherten bzw. geöffneten Stand. Eine Meldung „„…“ wieder geöffnet, wo du zuletzt warst“
   („Reopened "…", where you worked last time“) bietet **Neu anfangen** (Start fresh) an, um
   stattdessen mit Orb Run neu zu beginnen.
4. Hatte die letzte Sitzung ungespeicherte Änderungen, fragt Studio
   **Restore "…" from your last session?** (die Frage ist nicht übersetzt) mit **Wiederherstellen**
   (Restore) und **Schließen** (Dismiss) (Abschnitt 11.15).

Mit **Spielen** (Play), Alt+P, fährt man in Orb Run sofort los.

### Als App installieren

Unter `/editor/` ist Studio eine installierbare Web-App:

- Chrome und Edge (Android, Windows, macOS, Linux) zeigen in der Titelleiste von Studio einen Knopf
  **⤓ Installieren** (⤓ Install). Auf iPhone und iPad erklärt derselbe Knopf die zwei Schritte:
  **Teilen** (Share), dann **Zum Home-Bildschirm** (Add to Home Screen).
- Installiert startet Studio im Vollbild; Notch und Statusleiste bleiben frei.
- Ein Service Worker hält die zuletzt geladene Version für den Offline-Betrieb vor, holt die Seite aber
  immer zuerst aus dem Netz.
- Jeder Deploy schreibt seine Build-Kennung in die Seite und in `build.json`. Studio fragt beim Start,
  bei jeder Rückkehr zur App, wenn das Netz zurückkommt und alle 60 s nach. Ist ein neuer Build da und
  kann nichts verloren gehen (Editiermodus, nichts ungespeichert), lädt Studio sofort neu. Sonst
  erscheint **Neue Version verfügbar** (New version available) mit **Aktualisieren** (Update) und
  **Später** (Later); beim Aktualisieren wandert das Level über den Autosave in die neue Version.
- Unten rechts in der Statusleiste steht die laufende Version, mit Hinweis, wenn online eine neuere
  bereitliegt.

### Sprache

Studio zeigt seine Oberfläche auf Deutsch oder Englisch. Ohne Vorgabe folgt es der Sprache des
Browsers. Der Knopf **DE | EN** unten rechts in der Statusleiste wechselt sie; die Seite lädt dann neu,
das Level bleibt über den Autosave erhalten. `?lang=de` bzw. `?lang=en` in der Adresse legt die Sprache
ebenso fest; Studio merkt sich die Wahl auf dem Gerät. Übersetzt wird nur die Anzeige: Produkt- und
Levelnamen bleiben englisch, Eingaben, Actor-Namen und das Ausgabeprotokoll bleiben unverändert
(`kern/web/studio/game.mjs`). Exportierte Spiele folgen ebenso der Sprache des Browsers bzw. `?lang=`.

Dieses Buch nennt die Beschriftungen, wie Studio sie auf Deutsch zeigt, in Klammern die englischen, etwa
**Datei** (File). Wer Studio auf Englisch nutzt (Knopf **DE | EN** unten rechts in der Statusleiste oder
`?lang=en`), findet sie dort. Was Studio nicht übersetzt, steht hier englisch und ohne Klammer.

---

## 11.2 Die Oberfläche im Überblick

| Bereich | Lage | Inhalt |
|---|---|---|
| Titelleiste | ganz oben | Kern-Logo, Menüleiste (**Datei** (File), **Bearbeiten** (Edit), **Fenster** (Window), **Spielen** (Play), **Hilfe** (Help)), Dokument-Tab mit Levelname und Punkt für ungespeicherte Änderungen, ggf. **⤓ Installieren** (⤓ Install), Bilder pro Sekunde |
| Toolbar | darunter | Speichern, Rückgängig/Wiederholen, **Hinzufügen** (Add), Werkzeuge, Koordinatensystem, Raster, Kamerageschwindigkeit, Spiel-Knöpfe, Viewport-Aufteilung, **Logik** (Logic), Spiel verpacken, Knopf für Outliner und Details |
| Viewports | Mitte | 1, 2 oder 4 Ansichten mit eigener Leiste |
| Outliner | rechts oben | alle Actors als Baum mit Ordnern, Suche, Sichtbarkeit |
| **Details** / **Welteinstellungen** (World Settings) | rechts unten | Eigenschaften der Auswahl bzw. der Welt, als zwei Tabs |
| **Inhaltsablage** (Content Drawer) | unten, ein- und ausklappbar | Tabs **Actors platzieren** (Place Actors), **Texturen** (Textures), **Sounds**, **Ausgabeprotokoll** (Output Log) |
| Statusleiste | ganz unten | **Inhaltsablage**, **Ausgabeprotokoll**, Konsole, Statusmeldung, Physik-Statistik, Ton, Sprache, Pegel, Auswahl, Version |

Die Trennlinie zwischen Viewport und Seitenleiste lässt sich ziehen (mindestens 240 px, höchstens 60 %
der Fensterbreite), ebenso die zwischen Outliner und Details. Studio merkt sich beide Größen und ob die
Inhaltsablage offen war.

### 11.2.1 Menüleiste

Ein Klick öffnet ein Menü; ist eines offen, wechselt das Überfahren eines anderen Menütitels direkt dorthin.
Mit der Maus klappen Untermenüs beim Überfahren **neben** dem Menü auf, nie darüber. Passt daneben
kein Platz oder wird getippt (Touch, Stift), ersetzt das Untermenü das Menü an Ort und Stelle, mit einer
Zurück-Zeile oben. So liegt nie ein Menü unter dem Finger, der es geöffnet hat. Auf Touch-Geräten sind
Menüzeilen 40 px hoch. Esc schließt jedes Menü.

**Datei**

| Eintrag | Kürzel | Wirkung |
|---|---|---|
| **Neues Level...** (New Level...) | Strg+N | Dialog mit sechs Vorlagen (siehe unten) |
| **Level öffnen...** (Open Level...) | Strg+O | `.klevel`-Datei öffnen |
| **Speichern** (Save) | Strg+S | speichern (beim ersten Mal mit Dateidialog) |
| **Speichern unter...** (Save As...) | Strg+Umschalt+S | unter neuem Namen speichern |
| **Textur importieren...** (Import Texture...) | | Bild (PNG, JPEG, WebP) als Textur ins Level holen |
| **Spiel verpacken (HTML)...** (Package Game (HTML)...) | | Spiel als eine HTML-Datei exportieren (Abschnitt 11.16) |
| **Beispiel-Levels** (Example Levels) ▸ | | Orb Run, Physics Lab, Shooting Range, Micro Duck, **Grundformen** (Basic), **Leer** (Empty) |

Die Vorlagen im Dialog **Neues Level** (New Level); ein Doppelklick auf eine Karte erstellt sofort, sonst
der Knopf **Erstellen** (Create):

| Vorlage | Beschreibung im Dialog |
|---|---|
| Orb Run | „Prozedurale Nachtstadt, Box3D-Hovercar, 24 Orbs zum Sammeln“ |
| Physics Lab | „Jedes Box3D-Gelenk: Scharnier, Schiene, Rad, Seilbrücke, Motor, Stapel“ |
| Shooting Range | Ego-Shooter-Soldat (Desktop, Gamepad, Touch), Ziele, Deckung, mehrspielerfähig (im Dialog englisch) |
| Micro Duck | Pollen Robotics' Laufroboter auf seinem trainierten Netz, läuft selbstständig mit ToF und Kamera (im Dialog englisch) |
| **Grundformen** | „Boden, Sonne und Himmel – ein sauberer Anfang“; dazu ein Würfel, der Actor „Cube“ |
| **Leer** | „Gar nichts“ |

Hat das aktuelle Level ungespeicherte Änderungen, weist der Dialog darauf hin, dass sie im Autosave
bleiben, bis man das neue Level bearbeitet.

**Bearbeiten**

| Eintrag | Kürzel | Wirkung |
|---|---|---|
| **Rückgängig:** *Schritt* (Undo *Schritt*) | Strg+Z | letzten Schritt zurücknehmen (der Name des Schritts steht im Eintrag) |
| **Wiederholen:** *Schritt* (Redo *Schritt*) | Strg+Y | wiederholen |
| **Verlauf...** (History...) | | alle Schritte als Liste; ein Klick springt zu diesem Stand |
| **Ausschneiden** (Cut) / **Kopieren** (Copy) / **Einfügen** (Paste) | Strg+X / C / V | über die Zwischenablage |
| **Duplizieren** (Duplicate) | Strg+D | Auswahl verdoppeln |
| **Löschen** (Delete) | Entf | Auswahl löschen |
| **Alles auswählen** (Select All) | Strg+A | alles auswählen |
| **Auswahl aufheben** (Deselect) | Esc | Auswahl aufheben |
| **Gruppieren** (Group) | Strg+G | Auswahl unter einen neuen Actor „Group" hängen |
| **Auf den Boden setzen** (Snap to Floor) | Ende | Auswahl auf den Boden setzen |
| **Einstellungen...** (Preferences...) | | Kamera, Maus, Raster (Tabelle in Abschnitt 11.15) |
| **Einstellungen zurücksetzen...** (Reset Settings to Defaults...) | | alle Editor-Einstellungen zurücksetzen |

**Fenster**

| Eintrag | Kürzel | Wirkung |
|---|---|---|
| **Viewport-Aufteilung** (Viewport Layout) ▸ | | 1, 2 oder 4 Viewports |
| **Viewport-Typ** (Viewport Type) ▸ | Alt+G / J / K / H | **Perspektive** (Perspective) (Alt+G), **Oben** (Top) (Alt+J), **Unten** (Bottom), **Links** (Left), **Rechts** (Right) (Alt+K), **Vorne** (Front) (Alt+H), **Hinten** (Back) (für den aktiven Viewport) |
| **Ansichtsmodus** (View Mode) ▸ | Alt+4, Alt+3, Alt+2, Alt+5 | **Beleuchtet** (Lit), **Unbeleuchtet** (Unlit), **Drahtgitter** (Wireframe), **Kollision** (Collision) |
| **Anzeigen** (Show) ▸ | | **Raster** (Grid), **Symbole** (Icons), **Auswahlumriss** (Selection Outline), **Kollisionsformen** (Collision Shapes), **Gelenke** (Constraints), **Kontaktpunkte** (Contact Points), **Begrenzungen** (Bounds), **Nebel** (Fog), **Himmel** (Sky), **Statistik** (Stats), **Roboterkamera** (Robot Camera) |
| **Spielansicht** (Game View) | G | alle Editor-Helfer im aktiven Viewport aus |
| **Auswahl fokussieren** (Focus Selection) | F | Auswahl ins Bild holen |
| **Inhaltsablage** | Strg+Leertaste | Inhaltsablage ein/aus |
| **Ausgabeprotokoll** | | Inhaltsablage mit dem Protokoll öffnen |
| **Vollbild** (Fullscreen) | F11 | Browser-Vollbild |

**Spielen**

| Eintrag | Kürzel | Wirkung |
|---|---|---|
| **Spielen** | Alt+P | spielen (Abschnitt 11.11) |
| **Simulieren** (Simulate) | Alt+S | Physik laufen lassen, Kamera frei |
| **Pause** / **Fortsetzen** (Resume) | Pause | anhalten / fortsetzen |
| **Ein Bild weiter** (Advance One Frame) | . | einen Schritt weiter (nur pausiert) |
| **Stopp** (Stop) | Esc | zurück zum Bearbeiten |
| **Aus dem Spieler aussteigen** (Eject from Player) / **Spieler übernehmen** (Possess Player) | F8 | Spieler verlassen / übernehmen |
| **Spiel neu starten** (Restart Game) | Strg+R | Spiel neu starten |
| **Simulationsänderungen übernehmen** (Keep Simulation Changes) | K | simulierte Posen der Auswahl ins Level übernehmen |
| **Anzahl der Spieler** (Number of Players) ▸ | | **1 Spieler** (1 Player) bis **4 Spieler (Listen-Server)** (4 Players (listen server)) (Abschnitt 11.17) |
| **Netzwerk-Emulation** (Network Emulation) ▸ | | **Perfekt** (Perfect), **LAN (5 ms)**, **Mittel (50 ms, 1 % Verlust)** (Average (50 ms, 1% loss)), **Schlecht (150 ms, 5 % Verlust)** (Bad (150 ms, 5% loss)), **Sehr schlecht (300 ms, 20 % Verlust)** (Terrible (300 ms, 20% loss)) |
| **Online-Spiel hosten...** (Host Online Game...) | | Spiel über das Netz anbieten |
| **Online-Spiel beitreten...** (Join Online Game...) | | einem angebotenen Spiel beitreten |
| **Online-Spiel verlassen** (Leave Online Game) | | Online-Spiel verlassen |

**Hilfe**

| Eintrag | Kürzel | Wirkung |
|---|---|---|
| **Steuerung...** (Controls...) | F1 | Tabelle der Bedienung |
| **Über Kern Studio** (About Kern Studio) | | kurze Beschreibung |

Einträge, die im aktuellen Zustand nichts tun (etwa **Löschen** ohne Auswahl oder **Neues Level...**
beim Spielen), sind ausgegraut.

### 11.2.2 Toolbar

Von links nach rechts:

| Element | Kürzel | Wirkung |
|---|---|---|
| Diskette: **Speichern** | Strg+S | speichern |
| **Rückgängig** (Undo) / **Wiederholen** (Redo) | Strg+Z / Strg+Y | der Tooltip nennt den Schritt |
| **Hinzufügen** ▾ | | Menü mit allen Katalog-Gruppen (Abschnitt 11.8); beim Spielen gesperrt |
| **Auswählen (Q)** (Select (Q)) / **Verschieben (W)** (Move (W)) / **Drehen (E)** (Rotate (E)) / **Skalieren (R)** (Scale (R)) | Q / W / E / R | Werkzeug wählen |
| Globus bzw. Würfel: **Welt- / lokale Koordinaten** (World / local coordinates) | Strg+` | Gizmo in Welt- oder lokalen Achsen |
| Raster-Knopf + Auswahl: **Am Raster ausrichten** (Grid snap) | [ / ] | Raster an/aus, Rastergröße 1 cm bis 100 m |
| Winkel-Knopf + Auswahl: **Drehung einrasten** (Rotation snap) | | Drehschritte an/aus, 1°, 5°, 10°, 15°, 30°, 45°, 90° |
| Skalierungs-Knopf + Auswahl: **Skalierung einrasten** (Scale snap) | | Skalierungsschritte an/aus, 0,0625 bis 1 |
| Kamera + Auswahl: **Kamerageschwindigkeit** (Camera speed) | Mausrad beim Fliegen | Kamerageschwindigkeit 1 bis 8 |
| **Im Viewport spielen** (Play in the viewport) | Alt+P | spielen; beim Simulieren übernimmt er den Spieler |
| **Physik simulieren** (Simulate physics) | Alt+S | simulieren |
| **Pause** | Pause | anhalten / fortsetzen |
| **Ein Bild weiter** | . | einen Schritt weiter (nur pausiert) |
| **Stopp** | Esc | anhalten und zurück zum Bearbeiten |
| **Aussteigen / übernehmen** (Eject / possess) | F8 | Spieler verlassen / übernehmen |
| **Viewport-Aufteilung** | | 1, 2 oder 4 Viewports |
| **Logik** | | Logik-Graph des Levels öffnen (Abschnitt 11.10) |
| Paket: **Spiel als eine HTML-Datei verpacken** (Package the game as one HTML file) | | Dialog **Spiel verpacken** (Package Game) |
| **Outliner und Details** (Outliner and Details) | | nur auf schmalen Bildschirmen: Outliner, Details, Welt als Blatt (Abschnitt 11.18) |

Die Knöpfe der Toolbar zeigen nur Symbole; die Namen oben sind ihre Tooltips.

### 11.2.3 Viewports

Jeder Viewport hat oben eine eigene Leiste:

| Element | Inhalt |
|---|---|
| ≡ **Viewport-Optionen** (Viewport options) | **Spielansicht** (G), **Sichtfeld** (Field of View) mit 50°, 70°, 90°, 110°, **Auswahl fokussieren** (F), **Lesezeichen** (Bookmarks) mit **Zu Lesezeichen *n* springen** (Jump to bookmark *n*), Taste 1–9, und **Lesezeichen *n* setzen** (Set bookmark *n*), Strg+1–9, **Aufteilung** (Layout) |
| Ansichtstyp | **Perspektive** oder orthografisch: **Oben**, **Unten**, **Links**, **Rechts**, **Vorne**, **Hinten** |
| Ansichtsmodus | **Beleuchtet**, **Unbeleuchtet**, **Drahtgitter**, **Spieler-Kollision** (Player Collision); das Menü **Fenster › Ansichtsmodus** nennt denselben Modus **Kollision** |
| **Anzeigen** | **Raster**, **Symbole (lights, empties)** (Icons (lights, empties)), **Selection outline**, **Collision shapes**, **Gelenke**, **Contact points**, **Begrenzungen**, **Nebel**, **Himmel**, **Statistik**; die Liste bleibt beim Umschalten offen. Die klein geschriebenen englischen Einträge übersetzt Studio hier nicht |
| Maximieren: **Maximieren / wiederherstellen (4er-Aufteilung)** (Maximise / restore (quad layout)) | wechselt zwischen vier Viewports und diesem allein |

Der Klick auf ein Element der Leiste macht den Viewport zum aktiven. Wechselt man von der Perspektive in
eine orthografische Ansicht, schaltet Studio von **Beleuchtet** auf **Drahtgitter**, zurück in die
Perspektive wieder auf **Beleuchtet**. **Gelenke** ist standardmäßig aus: Sonst zeichnete jede Kette, Tür
und jedes Seil seine Debug-Linien.

Im Viewport selbst zeigt Studio:

- **Achsenkreuz** unten links (X rot, Y grün, Z blau) in der aktiven Perspektive. Z zeigt nach oben.
- **Infozeile**: in der Perspektive „Tempo *n*“ („Speed *n*“) und die Kameraposition, in
  orthografischen Ansichten die sichtbare Breite in Metern oder Kilometern. Mit **Anzeigen › Statistik**
  zusätzlich Bilder pro Sekunde und während der Simulation Bodies, Kontakte und Schrittzahl.
- **Hinweiszeile** je nach Zustand, etwa beim Fliegen
  „Flying · WASD move · Q/E down/up · wheel speed … · Shift faster · Z/C zoom" (nicht übersetzt).
- **Gizmo-Anzeige** beim Ziehen: Verschiebung (Δ in m), Winkel (°) oder Skalierungsfaktoren.

![Vier Viewports: Perspektive und drei orthografische Ansichten](bilder/studio-quad.png)

### 11.2.4 Outliner

Der Outliner listet alle Actors des Levels in den Spalten Sichtbarkeit (Auge), **Name** (Label) und
**Typ** (Type). Der Typ folgt den Komponenten: Prozedurale Stadt (Procedural City), Spieler (Player),
Trigger, Pickup, Audioquelle (Audio Source), Verbundenes Mesh (Constrained Mesh), Physik-Mesh (Physics
Mesh), Statisches Mesh (Static Mesh), Punktlicht (Point Light), Physikkörper (Physics Body) oder Leer
(Empty).

| Aktion | Wirkung |
|---|---|
| Klick | auswählen |
| Umschalt+Klick | alles zwischen dem zuletzt gewählten und diesem Actor hinzufügen |
| Strg+Klick | einzeln hinzufügen oder entfernen |
| Doppelklick | Actor im Viewport fokussieren; bei einem Ordner: umbenennen |
| F2 | umbenennen (direkt in der Zeile, Enter bestätigt, Esc bricht ab) |
| Auge | ein-/ausblenden, bei Ordnern alle Actors darin |
| Pfeil | Kinder bzw. Ordnerinhalt auf- und zuklappen |
| Ziehen auf einen Actor | als Kind anhängen (Weltlage bleibt) |
| Ziehen auf einen Ordner | in den Ordner verschieben |
| Ziehen ins Leere | vom Eltern-Actor lösen und aus dem Ordner nehmen |
| Ordner-Knopf im Panelkopf: **Neuer Ordner für die Auswahl** (New folder for the selection) | neuen Ordner „New Folder" für die Auswahl anlegen |
| Rechtsklick | Kontextmenü (wie im Viewport); bei Ordnern zusätzlich **Ordner umbenennen** (Rename Folder) und **Ordner entfernen (Actors behalten)** (Remove Folder (keep actors)) |
| Suchfeld „Actors suchen“ (Search actors) | filtert nach Name, Typ und Tags |

Der Actor, den man beim Spielen steuert, trägt das Abzeichen **SPIELER** (PLAYER); gesperrte Actors ein
Schloss. Die Fußzeile zählt Actors und Auswahl. Ordner gelten nur für Actors der obersten Ebene.

### 11.2.5 Details

Details zeigt die Auswahl: oben das Symbol, der Name (bei Mehrfachauswahl die Anzahl) und die Knöpfe
**Logik** (Graph dieses Actors) und **Hinzufügen** (Komponente hinzufügen). Darunter ein Suchfeld und
die Abschnitte **Actor**, **Transformation** (Transform) und eine Sektion je Komponente. Jede Sektion
klappt per Klick auf ihren Kopf zu; Komponenten haben dort einen Papierkorb zum Entfernen.

Bei Mehrfachauswahl erscheinen nur Komponenten, die alle ausgewählten Actors haben. Unterschiedliche
Werte zeigen „Mehrere Werte“ (Multiple values), Zahlenfelder das nicht übersetzte „Multiple"; eine
Eingabe setzt sie für alle.

Bearbeiten:

| Feldart | Bedienung |
|---|---|
| Zahl | tippen und Enter; Rechenausdrücke wie `2*3` oder `10/4+1` sind erlaubt, Komma gilt als Punkt |
| Zahl ziehen | den Feldnamen (oder ein nicht fokussiertes Zahlenfeld) seitwärts ziehen; Umschalt ×10, Strg ×0,1 |
| Zahl mit Pfeiltasten | ↑/↓ im Feld; Umschalt ×10, Strg ×0,1 |
| Regler | Felder mit Bereich 0 bis 1 haben zusätzlich einen Schieberegler |
| Vektor | drei Felder X, Y, Z; bei **Skalierung** (Scale) ein Schloss **Proportionen sperren** (Lock proportions) für proportionales Skalieren |
| Farbe | Farbwähler oder Hex-Wert (`#rrggbb`); der Wähler bleibt offen, solange man darin zieht, jede Farbe wirkt sofort |
| Auswahl | Liste (Form, Typ, Modus, Sound, Textur, Ziel-Actor …) |
| Tags | durch Komma getrennt |
| Zurücksetzen | Pfeil **Auf Standard zurücksetzen** (Reset to default) rechts neben dem Feld, sichtbar wenn der Wert vom Standard abweicht |
| Rechtsklick auf ein Feld | **Auf Standard zurücksetzen** (Reset to Default), **Wert kopieren** (Copy Value), **Wert einfügen** (Paste Value); in Textfeldern zusätzlich **Ausschneiden**, **Kopieren**, **Einfügen**, **Alles auswählen** |

**Warum ein Zieh-Vorgang nur ein Schritt für Rückgängig ist:** Studio gibt jedem zusammenhängenden
Vorgang (Ziehen am Feldnamen, Schieberegler, Farbwähler) ein Transaktionskennzeichen. Die Engine fasst alle
Änderungen derselben Eigenschaft mit demselben Kennzeichen zu einem Schritt zusammen. Getippte Werte und
Klicks sind eigene Schritte.

Jede Änderung geht über die JSON-Form des Actors, wie sie auch beim Laden einer Datei gelesen wird. Werte
werden deshalb genauso geprüft und begrenzt, als kämen sie aus einer `.klevel`-Datei.

![Ausgewählter Actor mit dem Gizmo zum Verschieben und seinen Komponenten in Details](bilder/studio-selected.png)

### 11.2.6 Statusleiste und Konsole

| Element | Wirkung |
|---|---|
| **Inhaltsablage** | Inhaltsablage ein/aus (Strg+Leertaste) |
| **Ausgabeprotokoll** | Inhaltsablage mit dem Protokoll |
| **Cmd** | Konsole; ↑/↓ blättern in früheren Eingaben, Esc verlässt das Feld |
| Statusmeldung | letzte Meldung des Editors |
| Statistik | beim Simulieren und Spielen: Bodies, wache Bodies, Gelenke (davon gebrochen), Kontakte; online zusätzlich Rolle, Spieler, Ping, Verlust, Datenrate |
| Lautsprecher: **Ton an / aus** (Sound on / off) | Ton an/aus |
| Sprachknopf **DE \| EN** | Sprache der Oberfläche (Abschnitt 11.1) |
| Pegel: **Ausgangspegel** (Output level) | Ausgangspegel links/rechts |
| Auswahl | „*n* von *m* ausgewählt“ („*n* of *m* selected“) oder Anzahl der Actors |
| Version | laufender Build |

Konsolenbefehle (`help` listet sie):

| Befehl | Wirkung |
|---|---|
| `play`, `simulate`, `stop` | wie die Knöpfe |
| `spawn <item>` | Katalog-Element platzieren, z. B. `spawn physics_cube` (IDs in Abschnitt 11.8) |
| `select <name>` | alle Actors auswählen, deren Name den Text enthält |
| `tod <0..1>` | Tageszeit setzen |
| `gravity <x y z>` | Schwerkraft setzen (ohne Werte: 0 0 −9,80665) |
| `camspeed <1-8>` | Kamerageschwindigkeit |
| `stat` | Bilder pro Sekunde und Zählerstände ins Log |
| `clear` | Log leeren |

### 11.2.7 Kontextmenü

Rechtsklick ohne Ziehen in einem Viewport wählt den Actor unter dem Cursor und öffnet das Kontextmenü.
Auf Touch-Geräten öffnet langes Halten (mindestens 0,5 s ohne Bewegung) dasselbe Menü. Studio lässt das
Kontextmenü des Browsers nirgends zu: Überall, wo kein eigenes Menü passt, erscheint ein Studio-Menü.

| Situation | Einträge |
|---|---|
| Bearbeiten, Auswahl vorhanden | **Fokussieren** (Focus), **Umbenennen** (Rename), **Ausschneiden**, **Kopieren**, **Einfügen**, **Duplizieren**, **Löschen**, **Logik bearbeiten** (Edit Logic) bzw. **Logik hinzufügen (Blueprint)** (Add Logic (Blueprint)) (ein Actor), **Zum Spieler machen (beim Spielen steuern)** (Make Player (control it in Play)) (Actor mit Komponente **Spieler-Steuerung** (Player Controller)), **Auf den Boden setzen**, **Gruppieren**, **Anhängen an …** (Attach to …) und **Mit Gelenk verbinden** (Connect with Constraint) ▸ (ab zwei Actors), **Lösen** (Detach) (wenn angehängt), **Ausblenden** (Hide), **Sperren** (Lock) bzw. **Entsperren** (Unlock), **Kinder auswählen** (Select Children), **Actor platzieren** (Place Actor) ▸ |
| Bearbeiten, nichts ausgewählt | **Einfügen**, **Alles auswählen**, **Alle einblenden** (Show All), **Actor platzieren** ▸ |
| Simulieren/Spielen, Auswahl vorhanden | **Fokussieren**, **Simulationsänderungen übernehmen** |

**Actor platzieren** ▸ setzt **Würfel** (Cube), **Kugel** (Sphere), **Physik-Würfel** (Physics Cube),
**Punktlicht** (Point Light), **Orb** oder **Leerer Actor** (Empty Actor) an die Stelle, an der man
geklickt hat.

---

## 11.3 Navigation im Viewport

Die Steuerung folgt der Unreal Engine. Alle Kamerabewegungen gelten für den Viewport unter dem Zeiger;
ein Klick macht ihn zum aktiven.

### 11.3.1 Maus und Tastatur

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste halten + W A S D | Fliegen (Fly Mode wie in Unreal): vor, links, zurück, rechts |
| … + Q / E | runter / hoch (auch Bild↓ / Bild↑) |
| … + Pfeiltasten | wie W A S D |
| … + Umschalt | 2,5-mal schneller |
| … + Mausrad | Kamerageschwindigkeit 1–8 ändern |
| … + Z / C | heranzoomen / wegzoomen (Sichtfeld) |
| … + Maus bewegen | umsehen |
| Rechte + linke Maustaste | hoch/runter und seitwärts verschieben |
| Mittlere Maustaste ziehen | schwenken |
| Alt + linke Maustaste | um die Auswahl kreisen (bzw. um den Punkt unter dem Cursor) |
| Alt + rechte Maustaste | Dolly: näher/weiter zum Drehpunkt |
| Alt + mittlere Maustaste | schwenken |
| Mausrad | vor/zurück; in orthografischen Ansichten zum Cursor zoomen |
| Rechte oder mittlere Maustaste in orthografischen Ansichten | verschieben |
| F | Auswahl einrahmen (ohne Auswahl: das ganze Level) |
| Doppelklick in den Viewport | Auswahl fokussieren |
| Strg+1 … Strg+9 | Kamera-Lesezeichen setzen |
| 1 … 9 | Lesezeichen anspringen |

Ob Alt+links um die Auswahl kreist, regelt die Einstellung **Um die Auswahl kreisen (Alt+LMT)** (Orbit
around selection (Alt+LMB)), standardmäßig an.
Ohne Auswahl kreist die Kamera um den Punkt unter dem Cursor, sonst um einen Punkt am Boden vor ihr.

Während man fliegt, gehören W, E, R, Q den Bewegungstasten; Werkzeuge schalten dann nicht um. Bewegungen
lesen die physische Tastenposition (WASD bleibt auf AZERTY und QWERTZ an derselben Stelle), Kürzel wie
Strg+Z dagegen die beschriftete Taste.

Die Kamerageschwindigkeit in Metern pro Sekunde:

| Stufe | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| m/s | 1 | 2,5 | 5 | 10 | 20 | 40 | 80 | 160 |

Stufe 4 ist der Standard.

### 11.3.2 Finger

Im Editor und beim Simulieren steuert man die Kamera wie in einem 3D-Betrachter am Telefon:

| Geste | Wirkung |
|---|---|
| Ein Finger ziehen | um den Punkt unter dem Finger kreisen (mit **Um die Auswahl kreisen (Alt+LMT)** um die Auswahl); in orthografischen Ansichten verschieben |
| Zwei Finger | zum Punkt zwischen ihnen zoomen (auseinander: näher) und mit ihnen verschieben |
| Tippen | auswählen |
| Zweimal tippen | Auswahl einrahmen (wie F) |
| Lange halten | Kontextmenü |
| Finger auf einem Gizmo-Griff | verschieben, drehen, skalieren wie mit der Maus |
| Finger in einem anderen Viewport | macht ihn zum aktiven |

Beim Zoomen mit zwei Fingern bleibt, was unter den Fingern liegt, unter ihnen. Der Maßstab ist der
Abstand zu diesem Punkt; deshalb fühlt sich die Geste an einer Socke so an wie an einem Häuserblock.
Nach einem Pinch kreist der nächste Ein-Finger-Zug um den Punkt, zu dem man gezoomt hat, solange nichts
anderes die Kamera bewegt oder die Auswahl ändert. Die Seite selbst zoomt beim Pinch nie mit.

---

## 11.4 Tastenkürzel

Die vollständige Belegung, wie sie `Studio::key` in `kern/src/studio/ui.cpp` umsetzt. **F1** zeigt die
wichtigsten Kürzel auch in Studio.

### 11.4.1 Bearbeiten

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Werkzeug **Auswählen (Q)** (Select (Q)) / **Verschieben (W)** (Move (W)) / **Drehen (E)** (Rotate (E)) / **Skalieren (R)** (Scale (R)) |
| Leertaste | Verschieben → Drehen → Skalieren → Verschieben |
| Strg+` | Welt- / lokale Achsen |
| [ / ] | Raster kleiner / größer (0,01; 0,05; 0,1; 0,25; 0,5; 1; 2; 5; 10; 50; 100 m) |
| Entf, Rücktaste | Auswahl löschen |
| F | Auswahl fokussieren |
| Ende | Auswahl auf den Boden setzen |
| H | Auswahl ausblenden |
| Umschalt+H | alle anderen ausblenden |
| Strg+H | alles einblenden |
| G | **Spielansicht** (Game View) im aktiven Viewport |
| Esc | Gizmo-Zug abbrechen, sonst Auswahl aufheben |
| F2 | primären Actor umbenennen |
| F11 | Vollbild |
| F1 | Dialog aus **Hilfe › Steuerung...** (Help › Controls...) |
| Strg+Z / Strg+Umschalt+Z / Strg+Y | Rückgängig / Wiederholen / Wiederholen |
| Strg+C / X / V / D | Kopieren / Ausschneiden / Einfügen / Duplizieren |
| Strg+A | alles auswählen |
| Strg+G | gruppieren |
| Strg+S / Strg+Umschalt+S | Speichern / Speichern unter |
| Strg+O | Level öffnen |
| Strg+N | **Neues Level...** (New Level...) |
| Strg+Leertaste | **Inhaltsablage** (Content Drawer) |
| Strg+1 … 9 / 1 … 9 | Kamera-Lesezeichen setzen / anspringen |
| Alt+G / J / H / K | **Perspektive** (Perspective) / **Oben** (Top) / **Vorne** (Front) / **Rechts** (Right) |
| Alt+2 / 3 / 4 / 5 | **Drahtgitter** (Wireframe) / **Unbeleuchtet** (Unlit) / **Beleuchtet** (Lit) / **Kollision** (Collision) |
| Alt+P / Alt+S | **Spielen** (Play) / **Simulieren** (Simulate) |
| Alt + Gizmo ziehen | Kopie ziehen |

Kopierte Actors landen als Text (`{"format": "kern-actors", "actors": […]}`) in der Zwischenablage des
Systems. Ein Strg+V in einem anderen Tab oder einer anderen Sitzung fügt sie dort ein.

### 11.4.2 Spielen und Simulieren

| Taste | Wirkung |
|---|---|
| Esc | **Stopp** (Stop) (bricht zuerst einen laufenden Gizmo-Zug ab) |
| F8 | **Aus dem Spieler aussteigen** (Eject from Player) bzw. **Spieler übernehmen** (Possess Player) |
| Umschalt+F1 | Maus freigeben, ohne anzuhalten |
| Pause oder Alt+Umschalt+P | **Pause** / **Fortsetzen** (Resume) |
| Alt+P | Spielen beenden |
| Alt+S | auf Simulieren umschalten |
| Strg+R | Spiel neu starten (nicht beim Soldaten: dort ist Strg Ducken, R Nachladen) |
| . | einen Schritt weiter, wenn pausiert (beim Simulieren oder nach dem Aussteigen) |
| K | **Simulationsänderungen übernehmen** (Keep Simulation Changes), beim Simulieren |
| F | Auswahl fokussieren (beim Simulieren oder nach dem Aussteigen) |
| G | **Spielansicht** (beim Simulieren oder nach dem Aussteigen) |

Wenn der Browser die Maus gefangen hat (Pointer-Lock), schluckt er das erste Esc, um sie freizugeben.
Studio wertet diesen Verlust des Pointer-Locks als Esc und stoppt sofort, sodass ein Druck genügt. Ein
Fensterwechsel gibt die Maus nur frei.

Die Spieltasten selbst:

| Eingabe | Wirkung |
|---|---|
| W A S D, Pfeiltasten | bewegen, lenken, Gas |
| Maus (nach Klick in den Viewport) | umsehen |
| Leertaste | springen; beim Flieger hoch (auch E) |
| Q, Umschalt links, Strg links | beim Flieger runter |
| Umschalt | Boost bzw. Sprint |
| Linke Maustaste | feuern (Soldat); der Klick, der die Maus fängt, feuert nicht |
| Rechte Maustaste | zielen (Soldat) |
| R | nachladen |
| C oder Strg links | ducken |
| F | benutzen (Interact) |
| Mausrad | Spielkamera zoomen |
| 1–7, V | Bewegungen der Micro Duck (Abschnitt 11.13) |

Gamepads und Touch-Steuerung beschreibt Abschnitt 11.12.

### 11.4.3 Logik-Editor

Der Logik-Editor ist nicht übersetzt; Knöpfe und Knoten heißen dort auch auf Deutsch wie auf Englisch.

| Taste | Wirkung |
|---|---|
| F7 | Compile |
| Strg+F | Find (Suchen) |
| F9 | Breakpoint setzen/entfernen |
| Q | Verbindungen geraderichten |
| Umschalt+W / A / S / D | ausrichten |
| B / S / D / N / O / G / L + Klick | Branch / Sequence / Delay / Do N / Do Once / Gate / For Loop einfügen |
| C | Kommentar um die Auswahl |
| Strg+C / X / V / D | Kopieren / Ausschneiden / Einfügen / Duplizieren |
| Entf | löschen |
| F / Pos1 | Auswahl / alles einrahmen |
| Strg+Z | Rückgängig |
| Esc | zurück zum Viewport |

Solange der Logik-Editor offen ist, gehören Buchstaben und Entf dem Graphen; nur Strg+Z, Strg+Y, Strg+S
und F1 erreichen das Level.

---

## 11.5 Auswählen, Gizmos und Snapping

### Auswählen

| Eingabe | Wirkung |
|---|---|
| Linksklick | auswählen; Umschalt ergänzt, Strg schaltet um |
| Klick ins Leere | Auswahl aufheben (ohne Umschalt/Strg) |
| Linke Maustaste ziehen | Rahmenauswahl; Umschalt ergänzt, Strg schaltet um |
| Outliner | siehe Abschnitt 11.2.4 |

Linkes Ziehen bewegt nie die Kamera und nie einen Actor, auch nicht, wenn man auf einem Actor anfängt;
es zeichnet immer ein Auswahlrechteck (der Actor, auf dem man begann, gehört dann dazu). Actors bewegt
wie in Unreal nur der Gizmo. Die Rahmenauswahl nimmt jeden sichtbaren, nicht gesperrten Actor, dessen
Ursprung im Rechteck liegt; die prozedurale Stadt bleibt außen vor.

Lichter, Empties und andere Actors ohne Mesh wählt man über ihr Symbol im Viewport (**Fenster ›
Anzeigen › Symbole** (Window › Show › Icons)). Gesperrte Actors (Kontextmenü **Sperren** (Lock)) lassen
sich im Viewport nicht wählen: Der Klick geht durch sie hindurch zu dem, was dahinter liegt.

Die **primäre** Auswahl ist der zuletzt gewählte Actor. An ihm sitzt der Gizmo, und Details zeigt seinen
Namen.

### Gizmos

| Werkzeug | Griffe |
|---|---|
| **Verschieben (W)** (Move (W)) | drei Achsenpfeile, drei Flächen-Quadrate für je zwei Achsen, Mittelpunkt für die Bildschirmebene |
| **Drehen (E)** (Rotate (E)) | drei Ringe |
| **Skalieren (R)** (Scale (R)) | drei Achsen und ein Mittelgriff für gleichmäßiges Skalieren |
| **Auswählen (Q)** (Select (Q)) | kein Gizmo |

Achsen sind rot (X), grün (Y) und blau (Z); der Griff unter dem Zeiger leuchtet gelb. Mit dem Globus- bzw.
Würfel-Knopf **Welt- / lokale Koordinaten** (World / local coordinates), Strg+`, richtet sich der Gizmo
nach der Welt oder nach dem primären Actor. Bewegt werden
alle ausgewählten Actors, deren Vorfahren nicht selbst ausgewählt sind; Kinder folgen ihren Eltern.

**Alt + Gizmo ziehen** zieht eine Kopie und lässt das Original stehen. Kopie und Bewegung sind ein
einziger Schritt für Rückgängig. **Esc** während des Ziehens bricht ab und stellt den Ausgangszustand her.

### Snapping

| Raster | Standard | Wirkung |
|---|---|---|
| **Am Raster ausrichten** (Grid snap) | an, 0,5 m | Verschieben rastet auf Vielfache der Rastergröße ein (in Weltachsen auf absolute Positionen, in lokalen Achsen auf Schritte) |
| **Drehung einrasten** (Rotation snap) | an, 15° | Drehen dreht in Schritten |
| **Skalierung einrasten** (Scale snap) | aus, 0,25 | Skalieren ändert Faktoren in Schritten |

Das Raster gilt auch beim Platzieren neuer Actors: Ihre X- und Y-Position wird auf das Raster gelegt.
Eine Auswahl in der Toolbar schaltet das jeweilige Raster gleich mit ein. Der perspektivische Viewport
zeichnet ein Bodenraster, orthografische Ansichten ihr eigenes.

---

## 11.6 Levels, Actors und Komponenten

Ein Level besteht aus den **Welteinstellungen** (World Settings), den Actors, Texturen, Sounds, dem
Logik-Graphen des Levels und der Editor-Kamera. Einheiten sind Meter, Sekunden und Kilogramm, Z zeigt nach oben. Rotationen stehen als
Euler-Winkel in Grad (x Roll, y Pitch, z Yaw), Farben als sRGB.

Ein Actor hat einen Namen, einen Eltern-Actor, einen Outliner-Ordner, Tags, die Schalter für Ausblenden
und Sperren, eine Transformation und Komponenten.

### 11.6.1 Actors anlegen

1. Die **Inhaltsablage** (Content Drawer) öffnen (Strg+Leertaste) und ein Element anklicken: Es erscheint
   dort, wohin die aktive Kamera blickt (auf dem getroffenen Objekt, sonst auf dem Boden vor der Kamera,
   sonst 10 m voraus).
2. Oder ein Element in einen Viewport ziehen: Es landet genau dort, wo man loslässt.
3. Oder **Hinzufügen** (Add) in der Toolbar, das Kontextmenü **Actor platzieren** (Place Actor) ▸ oder
   die Konsole (`spawn <id>`).

Neue Actors bekommen eindeutige Namen („Cube", „Cube 2", …) und sind danach ausgewählt. Wer einen Actor
aus der Gruppe **Spieler** (Players) platziert, macht ihn zum Spieler; das Ausgabeprotokoll meldet
„… is the player now - press Play (Alt+P) to control it".

### 11.6.2 Hierarchie und Ordner

- Anhängen: zwei oder mehr Actors wählen, den künftigen Eltern-Actor zuletzt, dann im Kontextmenü
  **Anhängen an …** (Attach to …); oder im Outliner auf einen Actor ziehen. Die Weltlage bleibt
  erhalten.
- Lösen: **Lösen** (Detach) im Kontextmenü oder im Outliner ins Leere ziehen.
- Gruppieren: Strg+G legt einen leeren Actor „Group" in die Mitte der Auswahl und hängt sie daran.
- Ordner: Feld **Outliner-Ordner** (Outliner folder) in Details, Ordner-Knopf im Outliner oder Ziehen auf
  einen Ordner.
- Kinder wählen: **Kinder auswählen** (Select Children) im Kontextmenü.

Kind-Actors ohne eigenen Body werden beim Simulieren als Compound in den Body des nächsten
Vorfahren geschweißt. Meshes mit Kollision, Pickups und Trigger ohne Body bekommen einen impliziten
statischen Body, bei Animation durch Rotator oder Beweger (Mover) einen kinematischen
([Kapitel 6](06-physik.md)).

### 11.6.3 Komponenten

**Hinzufügen** in Details bietet nur an, was die Auswahl noch nicht hat; Kollider und Gelenke sind Listen
und können mehrfach vorkommen. Die prozedurale Stadt gibt es nur für einen einzelnen Actor ohne Eltern.

Die Tabelle nennt die Komponenten und Felder, wie Details sie zeigt; kursiv stehen die Zwischenüberschriften
einer Sektion, in eckigen Klammern die Auswahl-Einträge. Englisch gebliebene Namen übersetzt Studio nicht.

| Komponente | Wichtigste Felder |
|---|---|
| **Statisches Mesh** (Static Mesh) | **Form** (Shape) [**Box**, **Kugel** (Sphere), **Zylinder** (Cylinder), **Kegel** (Cone), **Kapsel** (Capsule), **Ebene** (Plane), **Torus**, **Rampe** (Ramp), **Mesh**], **Größe** (Size) bzw. **Radius**/**Höhe** (Height)/**Segmente** (Segments), nur was die Form nutzt; *Physik* (Physics): **Kollision erzeugen** (Generate collision), **Reibung** (Friction), **Rückprall (Elastizität)** (Restitution (bounce)), **Rollwiderstand** (Rolling resistance); *Material*: **Grundfarbe** (Base color), **Rauheit** (Roughness), **Metallisch** (Metallic), **Leuchtfarbe** (Emissive color), **Leuchtstärke** (Emissive strength), **Deckkraft** (Opacity), **Unbeleuchtet** (Unlit), **Wirft Schatten** (Cast shadow), **Beidseitig** (Two sided), **Textur** (Texture), **Texturkachelung** (Texture tiling) |
| **Punktlicht** (Point Light) | **Lichtfarbe** (Light color), **Intensität** (Intensity), **Reichweite** (Attenuation radius) |
| **Starrkörper (Box3D)** (Rigid Body (Box3D)) | **Mobilität** (Mobility) [**Statisch** (Static), **Kinematisch** (Kinematic), **Dynamisch (Physik simulieren)** (Dynamic (simulate physics))], **Masse (0 = aus Dichte)** (Mass (0 = from density)), **Lineare Dämpfung** (Linear damping), **Winkeldämpfung** (Angular damping), **Schwerkraftfaktor** (Gravity scale), **Kontinuierlich (Geschoss)** (Continuous (bullet)), **Wach starten** (Start awake), **Ruhen erlauben** (Allow sleep); *Gelenke* (Constraints): **Position sperren** (Lock position), **Drehung sperren** (Lock rotation), je X/Y/Z; *Anfangszustand* (Initial state): **Anfangsgeschwindigkeit** (Initial velocity), **Anfangsdrehung** (Initial spin); *Sound*: **Aufprallgeräusch** (Impact sound), **Aufprallschwelle** (Impact threshold) |
| **Kollider** (Collider), mehrere | **Form** [**An Mesh anpassen** (Fit to mesh), **Box**, **Kugel**, **Kapsel**, **Konvexe Hülle** (Convex hull)], **Versatz** (Offset), **Drehung** (Rotation), **Boxgröße** (Box size), **Radius**, **Kapsellänge** (Capsule length); *Physikmaterial* (Physical material): **Dichte** (Density), **Reibung**, **Rückprall (Elastizität)**, **Rollwiderstand**; *Kollision* (Collision): **Sensor (nur Überlappung)** (Sensor (overlap only)), **Kanal-Bits** (Channel bits), **Kollidiert mit Bits** (Collides with bits), **Gruppenindex** (Group index); *Sound*: **Aufprallgeräusch** [**Automatisch** (Automatic) = der des Bodys] |
| **Gelenk** (Constraint), mehrere | **Typ** (Type) [**Scharnier (Drehgelenk)** (Hinge (revolute)), **Schiene (Schubgelenk)** (Slider (prismatic)), **Kugelgelenk** (Ball (spherical)), **Fest (verschweißt)** (Fixed (weld)), **Abstand / Seil** (Distance / rope), **Rad (Federung)** (Wheel (suspension)), **Motor**, **Parallel**, **Keine Kollision (Filter)** (No collision (filter))], **Verbundener Actor** (Connected actor) [**Welt (fester Punkt)** (World (fixed point)) oder ein Actor], **Anker (lokal)** (Anchor (local)), **Achse (lokal)** (Axis (local)), **Verbundene kollidieren** (Collide connected); *Grenzen* (Limits): **Begrenzung** (Limit), **Unten** (Lower), **Oben** (Upper); *Motor*: **Motor**, **Geschwindigkeit** (Speed), **Max. Kraft / Drehmoment** (Max force / torque); *Feder* (Spring): **Feder**, **Frequenz** (Frequency), **Dämpfungsgrad** (Damping ratio); **Ruhelänge (0 = wie beim Start)** (Rest length (0 = at start)); *Bruch* (Breaking): **Bruchkraft (0 = nie)** (Break force (0 = never)), **Bruchmoment (0 = nie)** (Break torque (0 = never)), **Bruchgeräusch** (Break sound). Eine Info-Zeile nennt die Einheiten (Grad oder Meter) |
| **Spieler-Steuerung** (Player Controller) | **Bewegung** (Movement) [**Hovercar** (Hover car), **Rollende Kugel** (Rolling ball), **Figur** (Character), **Flieger** (Flyer), **Fahrzeug** (Vehicle), **Soldat (Ego-Shooter)** (Soldier (first person shooter)), **Läufer (Roboter auf seinem trainierten Netz)** (Walker (robot on its trained network))], **Höchstgeschwindigkeit** (Max speed), **Beschleunigung** (Acceleration), **Drehrate** (Turn rate), **Sprunggeschwindigkeit (0 = keiner)** (Jump speed (0 = none)), **Boost**, **Boost-Geschwindigkeit** (Boost speed), **Schwebehöhe** (Hover height); *Kamera* (Camera): **Kamera** [**Verfolger** (Chase), **Ego-Perspektive** (First person), **Von oben** (Top down)], **Abstand** (Distance), **Höhe**, **Sichtfeld** (Field of view); *Soldat* (Soldier): **Ducken-Geschwindigkeit** (Crouch speed), **Augenhöhe** (Eye height), **Blickempfindlichkeit** (Look sensitivity), **Kopfwippen** (Head bob); *Läufer* (Walker): **Autonom (Kamera, ToF)** (Autonomous (camera, ToF)), **Laufnetz** (Walking network), **Aktionen zu Servo-Zielen** (Actions to servo goals), **Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)** (Kern extras (ball play, sock fetching, beak in the pick)); *Sound*: **Motorschleife** (Engine loop), **Jump**, **Boost** |
| **Waffe** (Weapon) | **Typ** [**Gewehr** (Rifle), **Pistole** (Pistol), **Schrotflinte** (Shotgun), **Scharfschützengewehr** (Sniper rifle)], **Schaden** (Damage), **Kopftreffer-Faktor** (Headshot multiplier), **Feuerrate** (Fire rate), **Automatisch** (Automatic), **Schrot pro Schuss** (Pellets per shot), **Reichweite** (Range), **Impuls auf Körper** (Impulse on bodies); *Munition* (Ammo): **Magazin** (Magazine), **Reserve**, **Nachladezeit** (Reload time); *Fahrverhalten* (Handling): **Streuung aus der Hüfte** (Hip spread), **Streuung beim Zielen** (Aimed spread), **Rückstoß** (Recoil), **Sichtfeld beim Zielen** (Aim field of view); *Sound*: **Fire**, **Reload** |
| **Gesundheit** (Health) | **Gesundheit**, **Panzerung** (Armor), **Regeneriert nach (0 = nie)** (Regenerate after (0 = never)), **Regeneration**, **Wiedereinstieg** (Respawn), **Wiedereinstiegs-Verzögerung** (Respawn delay), **Punkte pro Abschuss** (Points for a kill); *Sound*: **Hit**, **Down**; *Explosion*: **Explosionsradius (0 = keiner)** (Explosion radius (0 = none)), **Explosionsschaden** (Explosion damage) |
| **Pickup** | **Punkte** (Points), **Funkenfarbe** (Burst color), **Wiedereinstieg nach (0 = nie)** (Respawn after (0 = never)), **Sound**, **Heilt** (Heals), **Munition** |
| **Trigger** | **Aktion** (Action) [**Ziel (Sieg)** (Goal (win)), **Gefahr (Niederlage)** (Hazard (lose)), **Checkpoint (hier wieder einsteigen)** (Checkpoint (respawn here)), **Nachricht** (Message), **Sprungfeld (Start)** (Jump pad (launch)), **Teleport**, **Schadenszone** (Damage zone), **Nur Logik-Ereignis** (Logic event only)], **Nachricht**, **Nur einmal** (Only once), **Stärke (Start m/s, Schaden)** (Strength (launch m/s, damage)), **Teleport-Ziel** (Teleport target), **Sound** [**Automatisch** = passend zur Aktion] |
| **Rotator** | **Drehrate** (Rotation rate), °/s je Achse |
| **Beweger** (Mover) | **Versatz**, **Periode** (Period), **Phase**, **Bewegungsart** (Motion) [**Sinus** (Sine), **Hin und her** (Ping-pong), **Schleife** (Loop)] |
| **Prozedurale Stadt** (Procedural City) | **Seed**, **Blöcke pro Seite** (Blocks per side), **Blockraster** (Block pitch), **Straßenbreite** (Street width), **Höchstes Gebäude** (Tallest building), **Bäume** (Trees), **Verkehr (Autos)** (Traffic cars), **Erleuchtete Fenster bei Nacht** (Lit windows at night) |
| **Audioquelle** (Audio Source) | **Sound**, **Lautstärke** (Volume), **Tonhöhe** (Pitch), **Schleife**, **Beim Start abspielen** (Play on start), **Startverzögerung** (Start delay), **Bus**; *Abschwächung* (Attenuation): **Räumlich (3D)** (Spatial (3D)), **Inner radius**, **Abfallentfernung** (Falloff distance), **Abfall** (Roll-off), **Doppler**, **Tonhöhe nach Tempo** (Pitch by speed) |

Was die Felder physikalisch bedeuten und welche Werte sich bewähren, steht in [Kapitel 6](06-physik.md)
(Body, Kollider, Gelenk), [Kapitel 7](07-audio.md) (Audioquelle, Sounds) und
[Kapitel 8](08-spiel-laufzeit.md) (Spieler-Steuerung, Pickup, Trigger, Waffe, Gesundheit).

### 11.6.4 Gelenke mit „Mit Gelenk verbinden“

Schneller als ein Gelenk von Hand:

1. Den Actor wählen, der am Gelenk hängen soll (Body B).
2. Mit Umschalt oder Strg den Actor dazu wählen, an dem er hängt (Body A), **als Letztes**.
3. Rechtsklick › **Mit Gelenk verbinden** (Connect with Constraint) › **Scharnier** (Hinge), **Schiene**
   (Slider), **Kugel** (Ball), **Fest (verschweißt)**, **Abstand / Seil**, **Rad** (Wheel) oder **Motor**.

Studio setzt den Anker auf halbem Weg zwischen den beiden Actors (außer bei **Abstand / Seil** und
**Rad**) und gibt dem gehängten Actor einen **Starrkörper (Box3D)**, falls er keinen hat. Mehrere
gehängte Actors bekommen je ein Gelenk zum zuletzt gewählten.

### 11.6.5 Der Spieler

Beim Spielen steuert man einen Actor mit **Spieler-Steuerung**. Festlegen lässt er sich auf drei Arten:
einen Actor aus der Gruppe **Spieler** platzieren, Rechtsklick › **Zum Spieler machen (beim Spielen steuern)** (Make
Player (control it in Play)) oder **Welteinstellungen › Spieler (beim Spielen gesteuert)** (World Settings
› Player (controlled in Play)). Hat das Level keinen Actor mit Spieler-Steuerung, startet **Spielen**
(Play) stattdessen eine Simulation und meldet „Noch nichts zu steuern …“ („Nothing to control yet …“).

### 11.6.6 Fehler und Warnungen

Vor dem Spielen und Simulieren prüft Studio das Level (`game::validate`). Fehler wie verwaiste
Referenzen oder ungültige Werte verhindern den Start: „Behebe die Fehler oben, um zu spielen“ („Fix the
errors above to play“). Warnungen erscheinen nur im **Ausgabeprotokoll** (Output Log).

---

## 11.7 Rückgängig und Verlauf

Jeder Schritt speichert das vollständige Dokument nach der Änderung. **Rückgängig** (Undo) und
**Wiederholen** (Redo) sind dadurch exakt:
Es gibt keine Umkehroperationen, die auseinanderlaufen könnten, und das Ergebnis hängt nur von der Folge
der Änderungen ab, nie vom Timing. Studio hält bis zu 500 Schritte.

- **Bearbeiten › Verlauf...** (Edit › History...) zeigt alle Schritte im Dialog **Rückgängig-Verlauf**
  (Undo History); ein Klick springt zu diesem Stand.
- Aufeinanderfolgende Änderungen derselben Eigenschaft innerhalb eines Ziehvorgangs sind ein Schritt
  (Abschnitt 11.2.5).
- Eine Änderung, die nichts ändert, erzeugt keinen Schritt.
- Beim Spielen und Simulieren ist das Level gesperrt: „Stop playing to edit the level" (nicht übersetzt).

---

## 11.8 Inhaltsablage und eingebaute Inhalte

Die **Inhaltsablage** (Content Drawer) öffnet man mit Strg+Leertaste oder dem gleichnamigen Knopf in
der Statusleiste. Sie hat vier Tabs und ein Suchfeld und öffnet sich unter den Viewports.

### 11.8.1 Actors platzieren

Im Tab **Actors platzieren** (Place Actors) platziert ein Klick, Ziehen in einen Viewport platziert an
der Ablagestelle. Der Tooltip jeder Kachel beschreibt das Element. In der Konsole und in Befehlen heißen
die Elemente mit ihrer ID.

| Gruppe | Element | ID | Beschreibung |
|---|---|---|---|
| **Spieler** (Players) | **Soldat** (Soldier) | `soldier` | Ego-Shooter: Gewehr, Gesundheit, Wiedereinstieg; Maus, Gamepad, Touch; online-fähig |
| **Spieler** | **Figur** (Character) | `character` | Läufer aus der dritten Person: geht, sprintet, springt, steigt Stufen |
| **Spieler** | **Hovercar** (Hover Car) | `hover_car` | schnelles Box3D-Hovercar mit Boost |
| **Spieler** | **Rollende Kugel** (Rolling Ball) | `rolling_ball` | Physikkugel zum Rollen und Springen |
| **Spieler** | **Drohne** (Drone) | `drone` | Flieger: Leertaste/E hoch, Q/Strg runter |
| **Spieler** | **Micro Duck** | `micro_duck` | Pollen Robotics' Zweibeiner auf seinen trainierten Netzen |
| **Spieler** | **Micro Duck (Rollers)** | `micro_duck_rollers` | die Micro Duck auf Rollschuhen: `roller.onnx` fährt, 2 duckt sich |
| **Spielablauf** (Gameplay) | **Orb** | `orb` | leuchtendes Pickup, 1 Punkt |
| **Spielablauf** | **Stern** (Star) | `star` | drehendes Pickup, 10 Punkte |
| **Spielablauf** | **Medipack** (Health Pack) | `health_pack` | heilt 50; bleibt liegen bei voller Gesundheit; kommt nach 15 s wieder |
| **Spielablauf** | **Munitionskiste** (Ammo Box) | `ammo_box` | 60 Schuss für Soldaten; kommt nach 20 s wieder |
| **Spielablauf** | **Ziel** (Goal) | `goal` | Trigger, der das Spiel gewinnt |
| **Spielablauf** | **Gefahr** (Hazard) | `hazard` | Trigger, der das Spiel verliert |
| **Spielablauf** | **Schadenszone** (Damage Zone) | `damage_zone` | verletzt, wer hineinläuft (25 je Eintritt) |
| **Spielablauf** | **Checkpoint** | `checkpoint` | Wiedereinstiegspunkt |
| **Spielablauf** | **Sprungfeld** (Jump Pad) | `jump_pad` | schleudert Spieler hoch und nach vorn |
| **Spielablauf** | **Teleporter** | `teleporter` | zwei verbundene Felder: auf A treten, bei B ankommen |
| **Spielablauf** | **Nachrichten-Trigger** (Message Trigger) | `message` | zeigt bei Berührung einen Text |
| **Spielablauf** | **Bewegliche Plattform** (Moving Platform) | `moving_platform` | kinematische Plattform mit Beweger |
| **Spielablauf** | **Drehbalken** (Spinner) | `spinner` | drehender kinematischer Balken |
| **Spielablauf** | **Zielpuppe** (Target Dummy) | `target_dummy` | beschießbares Ziel mit Gesundheit und Kopf; kommt wieder |
| **Spielablauf** | **Explosives Fass** (Explosive Barrel) | `explosive_barrel` | explodiert bei Beschuss: stößt und verletzt, was nah ist |
| **Formen** (Shapes) | **Würfel** (Cube) | `cube` | 1-m-Würfel mit Kollision |
| **Formen** | **Kugel** (Sphere) | `sphere` | 1-m-Kugel |
| **Formen** | **Zylinder** (Cylinder) | `cylinder` | stehender Zylinder |
| **Formen** | **Kegel** (Cone) | `cone` | stehender Kegel |
| **Formen** | **Kapsel** (Capsule) | `capsule` | Kapsel entlang Z |
| **Formen** | **Ebene** (Plane) | `plane` | 10 × 10 m Boden |
| **Formen** | **Torus** | `torus` | Ring |
| **Formen** | **Rampe** (Ramp) | `ramp` | Keil zum Hinauffahren oder -rollen |
| **Formen** | **Leerer Actor** (Empty Actor) | `empty` | nur Transformation: Eltern oder Drehpunkt |
| **Physik** (Physics) | **Physik-Würfel** (Physics Cube) | `physics_cube` | dynamische Box3D-Kiste |
| **Physik** | **Physik-Kugel** (Physics Sphere) | `physics_sphere` | dynamische Kugel |
| **Physik** | **Physik-Fass** (Physics Barrel) | `physics_barrel` | dynamischer Zylinder (konvexe Hülle) |
| **Physik** | **Kistenstapel** (Crate Stack) | `crate_stack` | Pyramide aus 10 Kisten |
| **Physik** | **Pendel** (Pendulum) | `pendulum` | Kugel an einem Abstandsgelenk |
| **Physik** | **Scharniertür** (Hinged Door) | `hinged_door` | Tür an einem gefederten Scharnier mit Grenzen |
| **Physik** | **Kette** (Chain) | `chain` | 8 Glieder an Kugelgelenken |
| **Physik** | **Wippe** (See-Saw) | `see_saw` | Brett auf einem Scharnier |
| **Physik** | **Socke** (Sock) | `micro_duck_sock` | zerknüllte Socke im Maßstab der Micro Duck (weiche Klumpen an Federn); die Ente hebt sie mit 2 oder von selbst auf |
| **Lichter** (Lights) | **Punktlicht** (Point Light) | `point_light` | farbiges Licht mit Radius |
| **Audio** | **Audioquelle** (Audio Source) | `audio_source` | 3D-Klangquelle (Maschinenbrummen) |
| **Audio** | **Wind** (Ambient Wind) | `ambient_wind` | Wind in Schleife, überall hörbar |
| **Audio** | **Stadtatmosphäre** (City Ambience) | `city_ambience` | Stadtgeräusch in Schleife mit Abfall |
| **Audio** | **Radio** | `radio` | spielt den ruhigen Musik-Loop in 3D |
| **Welt** (World) | **Prozedurale Stadt** (Procedural City) | `city` | Blöcke, Straßen, Bäume und Verkehr |

Beim Einfügen einer Micro Duck stellt Studio die Physik auf 100 Hz mit 16 Teilschritten
(**Welteinstellungen › Schrittrate** (World Settings › Step rate) und **Teilschritte** (Sub-steps)) und
meldet das im Ausgabeprotokoll: Servogetriebene Roboter sind lange Gelenkketten, die ohne genug
Teilschritte gedehnt würden, und ihr Netz braucht seinen festen Takt.

### 11.8.2 Texturen

Der Tab **Texturen** (Textures) zeigt alle Texturen des Levels mit Vorschau und Größe.

- **Bild importieren** (Import image), alternativ **Datei › Textur importieren...** (File › Import
  Texture...) oder ein Bild auf die Seite ziehen, holt PNG, JPEG oder WebP ins Level. Bilder über
  2048 px Kantenlänge verkleinert Studio. Ist beim Import etwas ausgewählt, bekommt jedes ausgewählte Mesh die Textur, und seine Farbe wird weiß.
- **Doppelklick** auf eine Textur setzt sie auf die Auswahl.
- Der Papierkorb entfernt sie; Meshes, die sie nutzten, verlieren sie.

Texturen liegen als Data-URL im Level; die `.klevel`-Datei ist damit in sich vollständig.

### 11.8.3 Sounds

Der Tab **Sounds** listet die importierten Sounds und die eingebauten, nach Kategorien.

- Ein Klick spielt einen Sound an (der Knopf oben mit dem nicht übersetzten „Playing preview" stoppt
  ihn).
- Ein Doppelklick setzt ihn als Sound einer **Audioquelle** (Audio Source) auf die Auswahl (die
  Komponente kommt dazu, falls sie fehlt).
- Ziehen in einen Viewport platziert eine Audioquelle mit diesem Sound.
- **Audio importieren (WAV, MP3, OGG, M4A, FLAC)** (Import audio (WAV, MP3, OGG, M4A, FLAC)), oder
  Dateien auf die Seite ziehen, holt diese Formate und alles, was der Browser dekodieren kann, bis 24 MB
  je Datei. Rückgängig nimmt einen Import zurück.

Jedes Sound-Feld in Details hat einen Knopf **Vorhören** (Preview). Ton beginnt erst nach dem ersten
Klick oder Tastendruck (Browser-Regel); ohne Web Audio arbeitet Studio stumm. Die Sound-Engine beschreibt
[Kapitel 7](07-audio.md).

### 11.8.4 Ausgabeprotokoll

Der Tab **Ausgabeprotokoll** (Output Log) zeigt alle Meldungen mit Uhrzeit, Warnungen und Fehler farbig,
und zwar die letzten 200 Einträge; die Meldungen selbst sind nicht übersetzt. Beim Spielen und Simulieren
erscheinen hier auch Spielereignisse; `clear` in der Konsole leert das Protokoll.

---

## 11.9 Welteinstellungen

Der Tab **Welteinstellungen** (World Settings) neben **Details** (auf schmalen Bildschirmen der Tab
**Welt** (World)) gilt für das ganze Level.

| Feld | Bedeutung |
|---|---|
| **Level-Name** (Level name) | Name des Levels (auch Dateiname beim Speichern) |
| *Spiel* (Game) | |
| **Spieltitel** (Game title) | Titel im HUD und im exportierten Spiel |
| **Spieler (beim Spielen gesteuert)** (Player (controlled in Play)) | welcher Actor gesteuert wird |
| **Beobachterkamera (Spielen: fliegen, zuschauen, Körper ziehen)** (Observer camera (Play: fly, watch, drag bodies)) | das Spielen startet mit freier Kamera (Abschnitt 11.11.4) |
| **Siegbedingung** (Win condition) | **Keine (Sandkasten)** (None (sandbox)), **Alle Pickups sammeln** (Collect all pickups), **Ein Ziel erreichen** (Reach a goal trigger), **Das Zeitlimit überleben** (Survive the time limit), **Eine Punktzahl erreichen** (Reach a target score) |
| **Zeitlimit (0 = keins)** (Time limit (0 = none)) | Sekunden |
| **Zielpunktzahl** (Target score) | Punkte für **Eine Punktzahl erreichen** |
| **Kill-Z (darunter Wiedereinstieg)** (Kill Z (respawn below)) | unter dieser Höhe wird der Spieler zurückgesetzt |
| *Himmel und Licht* (Sky and light) | |
| **Tageszeit** (Time of day) | 0 bis knapp 1 |
| **Tageslänge (0 = angehalten)** (Day length (0 = frozen)) | Sekunden für einen ganzen Tag |
| **Sonnenrichtung** (Sun azimuth) | Grad |
| **Nebeldichte** (Fog density) | Dichte des Nebels |
| **Belichtung** (Exposure) | Belichtung |
| **Umgebungsverdeckung (0 = aus)** (Ambient occlusion radius (0 = off)) | Radius in Metern |
| **Schattenreichweite (kleiner: schärfer)** (Shadow distance (smaller: sharper)) | Meter der Sicht, die die Schattenkarte der Sonne abdeckt (2–2000, Standard 80; das Micro-Duck-Level 10) |
| **Wolken** (Clouds) | Wolken an/aus |
| **Endloser Boden** (Endless ground) | unendlicher Boden |
| **Bodenfarbe** (Ground color) | Farbe des Bodens |
| *Physik (Box3D)* (Physics (Box3D)) | |
| **Schwerkraft** (Gravity) | m/s² als Vektor |
| **Teilschritte** (Sub-steps) | 1 bis 16 |
| **Bodenreibung** (Ground friction) | Reibung des Bodens |
| **Bodenelastizität** (Ground bounciness) | Restitution des Bodens |
| **Kontaktsteifigkeit** (Contact stiffness) | Hz |
| **Kontaktdämpfung** (Contact damping ratio) | Dämpfung der Kontakte |
| **Kontakt-Ausstoßgeschwindigkeit** (Contact push-out speed) | m/s |
| **Schrittrate** (Step rate) | 15 bis 480 Hz |
| *Audio* | |
| **Gesamtlautstärke** (Master volume) | Gesamtlautstärke |
| **Musik** (Music), **Musiklautstärke** (Music volume) | Musikstück und Lautstärke |
| **Atmosphäre** (Ambience), **Atmosphärenlautstärke** (Ambience volume) | Hintergrundklang und Lautstärke |
| **Hall** (Reverb) | Hallanteil |
| **Doppler** | Stärke des Doppler-Effekts |

Die Physikwerte erklärt [Kapitel 6](06-physik.md), Himmel und Licht [Kapitel 4](04-grafik.md).

---

## 11.10 Der Logik-Editor

Jeder Actor und das Level selbst können einen Logik-Graphen tragen, nach dem Vorbild der Blueprints der
Unreal Engine. Geöffnet wird er über **Logik** (Logic) in der Toolbar (Graph des Levels), über **Logik**
in Details oder über **Logik bearbeiten** (Edit Logic) / **Logik hinzufügen (Blueprint)** (Add Logic
(Blueprint)) im Kontextmenü (Graph des Actors). Der Editor deckt dann die Viewports ab. Er selbst ist
nicht übersetzt; seine Beschriftungen stehen hier so, wie er sie zeigt.

| Bereich | Inhalt |
|---|---|
| Toolbar | **Viewport** (zurück, Esc), **Compile** (F7, mit Statusanzeige OK / Warnung / Fehler), Save (speichert das Level), **Find** (Strg+F), Auswahl des Blueprints (Level oder Actor), **Templates**, **C++** (Export), Rechteckauswahl (Touch), Hilfe |
| My Blueprint | Graphen (Event Graph, State Machines), Variablen mit Typfarbe und Live-Wert beim Spielen, Events |
| Graph | Tabs, Zoomstufen wie in Unreal, Knoten und Drähte |
| Details | ausgewählter Knoten, Variable, Kommentar, State oder Transition |
| Compiler Results / Find Results | Probleme und Suchtreffer; ein Klick springt zum Knoten |

Ein leerer Graph bietet Startknöpfe an: **Begin Play**, **On Interact**, **On Begin Overlap**,
**Event Tick** und **Templates…**. Die Vorlagen fügen fertige Graphen ein:

| Vorlage | Was sie tut |
|---|---|
| Door that opens on Interact | Interact schaltet zwischen Öffnen und Schließen, mit Ton |
| Score zone (overlap +10) | Betreten gibt 10 Punkte, zeigt „+10" und spielt einen Ton |
| Spawner (a cube every 3 s) | alle 3 s ein **Physik-Würfel** (Physics Cube), höchstens 10 |
| Traffic light (state machine) | Ampel als State Machine (Grün, Gelb, Rot) |
| Robot: walk and turn away with its ToF | die Micro Duck läuft und dreht ab, wenn ihr ToF näher als 1,6 m etwas sieht |

Ein Beispiel, wie es der Browser-Test bedient: Basic-Level, **Logik**, **Begin Play**, vom Ausgang
„then" einen Draht ins Leere ziehen, „show message" tippen, Enter, Text eintragen, **Viewport**, eine
**Rollende Kugel** (Rolling Ball) platzieren, **Spielen** (Play): Das HUD zeigt den Text.

Beim Start von **Spielen** oder **Simulieren** (Simulate) kompiliert Kern jeden Graphen zu
WebAssembly-Maschinencode; ist ein Graph als C++ ins Spiel eingebaut, läuft dieser Code statt des JIT. Ein Graph mit Fehlern wird beim Start
abgeschaltet und als „Logic error: …“ gemeldet; der Rest des Levels läuft weiter. **C++** lädt den Graphen als lesbaren C++-Quelltext herunter, der in
ein Spiel gebaut als nativer Code läuft (Abschnitt 12.4.3 in [Kapitel 12](12-eigene-spiele.md)).
Mit F9 gesetzte Breakpoints halten das Spiel an, sobald der Knoten läuft. Alle Knoten, Variablen, State
Machines, die Bedienung im Detail und die drei Ausführungsstufen beschreibt [Kapitel 9](09-logik.md).

---

## 11.11 Spielen und Simulieren

Studio kennt drei Zustände: Bearbeiten, Simulieren und Spielen. In beiden laufenden Zuständen ist das
Level gesperrt; Änderungen in der Simulation verschwinden beim **Stopp** (Stop), außer man übernimmt sie
mit **Simulationsänderungen übernehmen** (Keep Simulation Changes).

### 11.11.1 Spielen

**Spielen** (Play), mit Alt+P oder dem Toolbar-Knopf **Im Viewport spielen** (Play in the viewport),
startet das Spiel im aktiven Viewport: Die Spiel-Laufzeit baut die
Box3D-Welt, der Spieler bekommt die Steuerung, die Kamera folgt ihm, und das HUD zeigt Titel, eingesammelte
Pickups, Punkte, verbleibende oder vergangene Zeit, Bestzeit, Tempo in km/h mit Boost-Balken (beim Soldaten
Fadenkreuz, Gesundheit, Munition). Leiste und Infozeile des Viewports verschwinden.

1. Auf dem Desktop fängt Studio die Maus (Pointer-Lock). Ist sie frei, steht im Viewport der nicht
   übersetzte Hinweis „Click the viewport to control the game · Esc stops · F8 ejects"; ein Klick fängt
   sie wieder.
2. Bei Sieg oder Niederlage erscheint ein Banner „Gewonnen“ („Victory“) bzw. „Spiel vorbei“ („Game
   over“) mit **Nochmal spielen** (Play again).
3. Strg+R oder **Spielen › Spiel neu starten** (Play › Restart Game) startet neu.
4. Esc oder **Stopp** beendet das Spiel.

![Spielen im Viewport: das Hovercar in Orb Run mit HUD](bilder/studio-play.png)

### 11.11.2 Aussteigen und Übernehmen

F8 (**Aus dem Spieler aussteigen**, Eject from Player) verlässt den Spieler: Das Spiel läuft weiter, die
Kamera ist frei, und man kann wie beim Bearbeiten fliegen, auswählen und Körper greifen. Studio wechselt
dabei in den Zustand Simulieren. Noch einmal F8 (**Spieler übernehmen**, Possess Player) oder der Knopf
**Im Viewport spielen** übernimmt den Spieler wieder.

### 11.11.3 Simulieren

**Simulieren** (Simulate), Alt+S, lässt die Physik und die Spielregeln laufen, ohne einen Spieler zu
steuern. Die Kamera bleibt die des Editors.

- **Körper greifen:** Linke Maustaste auf einem dynamischen Körper (oder ein Finger) greift ihn mit dem
  Mouse Joint. Ziehen bewegt ihn, den Zeiger nach oben ziehen hebt ihn hoch, das Mausrad bzw. ein zweiter
  Finger (auseinander/zusammen) holt ihn näher oder schiebt ihn weg, Loslassen wirft ihn mit seinem
  Schwung. Strg ist dafür nicht nötig. Ein kurzer Klick wählt den Körper aus. Der Hinweis im Viewport
  lautet „Simulating · drag grabs bodies · K keeps poses · Esc stops“ (nicht übersetzt), der Dialog aus
  **Hilfe › Steuerung...** (Help › Controls...), F1, nennt „Ziehen (Simulieren) – Physikkörper greifen
  und werfen“ („Drag (Simulate) – Grab and throw physics bodies“).
- **Eine Micro Duck am Rumpf** hält der Griff zusätzlich in ihrer Ausrichtung, wie eine Hand den
  Roboter hält, statt sie am Punkt pendeln zu lassen (`begin_grab` mit `hold_rotation`). So
  hochgehoben, pausiert sie wie der echte Roboter ([Kapitel 13](13-micro-duck.md)).
- **Warum der Körper physikalisch bleibt:** Ein Motor-Gelenk mit Feder (7,5 Hz, kritisch gedämpft) zieht
  den gegriffenen Punkt zum Zeiger. Der Körper stößt an, kippt und schwingt wie jeder andere.
- **K** übernimmt die simulierten Posen der ausgewählten Actors ins Level (ein Schritt für Rückgängig) und beendet
  die Simulation.
- **Pause**, dann **.** geht Schritt für Schritt.

**Fenster › Anzeigen › Kollisionsformen** (Window › Show › Collision Shapes) zeigt die Box3D-Formen,
**Gelenke** (Constraints) die Gelenke, **Kontaktpunkte** (Contact Points) die Kontakte. Die Statusleiste
zählt Bodies, wache Bodies, Gelenke, gebrochene Gelenke und Kontakte.

![Simulieren mit eingeblendeten Kollisionsformen](bilder/studio-simulate.png)

![Physics Lab: alle Gelenktypen in der Simulation](bilder/studio-physics-lab.png)

### 11.11.4 Beobachterkamera

Mit **Welteinstellungen › Beobachterkamera (Spielen: fliegen, zuschauen, Körper ziehen)** (World
Settings › Observer camera (Play: fly, watch, drag bodies)) startet das Spielen nicht hinter dem Spieler,
sondern mit freier Kamera: Das Spiel läuft, man schaut zu und greift ein. Das Level Micro Duck nutzt das.

| Gerät | Bedienung |
|---|---|
| Desktop | W A S D fliegen, Q / E runter / hoch, Umschalt schneller, ohne gehaltene Maustaste; Ziehen ins Leere schaut sich um; rechte Maustaste und Mausrad wie im Editor |
| Telefon | ein Finger ins Leere schaut sich um; zwei Finger zoomen und schwenken |
| Beide | einen Körper ziehen, heben und werfen wie beim Simulieren |
| F8 | den Spieler übernehmen (etwa die Ente per Gamepad), noch einmal F8 schaut wieder zu |

---

## 11.12 Mobil, Touch und Gamepad

### 11.12.1 Welches Gerät gerade steuert

Die Engine erkennt Touch, Maus mit Tastatur und Gamepad selbst und schaltet auf das zuletzt benutzte um
(`InputArbiter` in `kern/src/game/touch.cpp`, nach den Regeln von SpawnCity):

- Ein Finger schaltet auf Touch.
- Eine Maus schaltet nur auf Geräten mit feinem Zeiger auf Desktop; eine Maus am Tablet behält das
  Touch-Layout.
- Jeder echte Tastendruck schaltet auf Desktop, auch am Telefon.
- Ein Gamepad zählt, sobald ein Knopf gedrückt oder ein Stick über 0,22 ausgelenkt wird.
- Pointer-Lock gibt es nur mit Maus.

### 11.12.2 Spielen auf Touch-Geräten

Das Spielen auf einem Touch-Gerät füllt den ganzen Bildschirm und schaltet auf einen Viewport. Oben
links stehen **⟵ ENDE** (⟵ EXIT) zum Beenden und **❚❚ PAUSE** bzw. **▶ WEITER** (▶ RESUME).

Die Engine legt die Bedienelemente selbst an (`TouchControls`), die Seite zeichnet sie nur:

- **Zwei Sticks** von 168 px in den unteren Ecken, bündig mit den sicheren Rändern; kleiner (mindestens
  72 px) nur, wenn zwei nicht passen. Links bewegen (Beschriftung **LAUFEN** (MOVE), bei Fahrzeugen
  **LENKEN / GAS** (STEER / GAS), beim Flieger **FLIEGEN** (FLY)), rechts schauen (**SCHAUEN** (LOOK))
  mit Drehen am Rand: Zieht man den rechten Stick über seinen Ring hinaus, dreht die Kamera weiter.
- **Sprint:** den linken Stick ganz nach vorn drücken (Figur und Soldat); der Ring leuchtet.
- **Bildschirm ziehen:** schaut sich um. Beim Soldaten mit Waffe feuert ein Finger auf dem Bildschirm und
  zielt weiter, solange er liegt.
- **Knöpfe** links neben dem rechten Stick und darüber, je nach **Bewegung** (Movement) der
  **Spieler-Steuerung** (Player Controller):

| Bewegung | Knöpfe |
|---|---|
| **Soldat (Ego-Shooter)** (Soldier (first person shooter)) | **SPRUNG** (JUMP), **LADEN** (RELOAD) (mit Waffe), **ZIELEN** (AIM) (mit Waffe, Umschalter), **DUCKEN** (CROUCH) (Umschalter), **NUTZEN** (USE) |
| **Figur** (Character) | **SPRUNG** (wenn die Sprunggeschwindigkeit > 0), **NUTZEN** |
| **Rollende Kugel** (Rolling ball) | **SPRUNG** (wenn die Sprunggeschwindigkeit > 0), **BOOST** (wenn Boost an) |
| **Flieger** (Flyer) | **HOCH** (UP), **RUNTER** (DOWN), **BOOST** (wenn Boost an) |
| **Hovercar** (Hover car), **Fahrzeug** (Vehicle) | **BOOST** (wenn Boost an), **HÜPFEN** (HOP) (wenn die Sprunggeschwindigkeit > 0), **NUTZEN** |
| **Läufer (Roboter auf seinem trainierten Netz)** (Walker (robot on its trained network)), die Micro Duck | **QUAK** (QUACK), **GREIFEN** (PICK), **SITZ** (SIT), **KICK L**, **KICK R**, **ROLLEN** (ROLL), **SCHNABEL** (BEAK), **HUIII** (WHEEE), **HÜPFEN** |

Hebt sich der letzte Finger vom Bildschirm oder geht die Seite in den Hintergrund, lässt die Engine
alles los; nichts bleibt gedrückt hängen.

![Spielen am Telefon im Hochformat: ENDE, PAUSE und die Touch-Sticks](bilder/phone-play-390.png)

### 11.12.3 Gamepad

Gamepads mit Xbox-Belegung (Standard-Mapping des Browsers) funktionieren beim Spielen im Editor und im
exportierten Spiel, mit Dead-Zones und Kurven wie in SpawnCity:

| Eingabe | Wirkung |
|---|---|
| Linker Stick | bewegen |
| Rechter Stick | schauen |
| RT | feuern |
| LT | zielen |
| A | springen |
| B | ducken |
| X | nachladen |
| Y | benutzen |
| L3 oder LB | Sprint/Boost |
| RB / LB | beim Flieger hoch / runter |
| Start | Pause |

---

## 11.13 Die Micro Duck steuern

Die Micro Duck ist Pollen Robotics' zweibeiniger Roboter, der auf seinen echten, trainierten Netzen läuft.
Wie Modell, Servos und Netze funktionieren, beschreibt [Kapitel 13](13-micro-duck.md); hier geht es um die
Bedienung im Studio.

![Das Beispiel-Level Micro Duck: ein Zimmer im Maßstab der Ente (Ente in der eigenen Gestaltung dieses Buchs)](bilder/duck-level.png)

**Zum Ausprobieren:** **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck)
öffnet ein Zimmer in ihrem Maßstab mit Möbeln, einem Ball und zwei Socken. Die Ente läuft autonom. Schon
ohne Zusätze spielt sie Ball wie in Pollens ursprünglicher Runtime (BallPlay: hinlaufen, ausrichten, mit
links kicken); mit eingeschaltetem **Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)** (Kern
extras (ball play, sock fetching, beak in the pick)), im Beispiel-Level an, spielt stattdessen Kerns
Spielschicht mit Ball und Socken. Der Level-Blueprint lässt sie zwitschern (Knoten **Chirp**), sobald ihre
Kopfkamera den Ball sieht. Das Spielen startet mit der Beobachterkamera (Abschnitt 11.11.4): Man schaut
zu und kann den Ball mit Maus oder Finger anheben; F8 übernimmt die Ente.

**Ohne Eingabe** läuft die Ente selbstständig (in **Details** die Sektion **Spieler-Steuerung** (Player
Controller), darunter *Läufer* (Walker) das Feld **Autonom (Kamera, ToF)** (Autonomous (camera, ToF))):
Sie erkundet den Raum mit Kopf-ToF und Kamera, weicht Hindernissen aus und steht nach einem Sturz selbst
wieder auf. Hebt man sie am Rumpf hoch, pausiert sie (Robot State „Held“) und läuft abgesetzt weiter.

**Tastatur** (beim Spielen, den Spieler übernommen):

| Taste | Bewegung |
|---|---|
| W / S | vorwärts / rückwärts laufen |
| A / D | drehen |
| Leertaste | quaken |
| 1 | hinsetzen / aufstehen |
| 2 | aufheben (Pick) |
| 3 | mit links kicken |
| 4 | mit rechts kicken |
| 5 | rollen (gehalten: rollt weiter) |
| 6 | Schnabel öffnen / schließen |
| **7** | **springen** – Kerns eigene Bewegung aus `jump.onnx` |
| V | „Wheee"-Fahrt, solange gehalten |

**Touch:** die Knöpfe aus Abschnitt 11.12.2. **HÜPFEN** (HOP) ist der Sprung (Taste 7),
**QUAK** (QUACK) quakt, **HUIII** (WHEEE) startet die Fahrt, die übrigen entsprechen den Tasten 1–6.

**Gamepad:** RT quakt, LT startet die Wheee-Fahrt (beide öffnen dabei den Schnabel), B setzt sich hin bzw.
steht auf, Y hebt auf, X rollt.

Logik steuert die Ente mit den Knoten der Kategorie **Robots** (Walk, Look, Robot ToF, Robot Camera Sees,
Robot Move mit der Bewegung „jump" und weitere, [Kapitel 9](09-logik.md)).

**Kopfkamera und ToF:** Während des Spiels zeigt Studio oben rechts im aktiven Viewport, was die Ente
sieht (**Fenster › Anzeigen › Roboterkamera** (Window › Show › Robot Camera), standardmäßig an; die
Beschriftungen der Anzeige sind nicht übersetzt):

- **HEAD CAMERA** – das Bild ihrer Kopfkamera, dazu ihr Zustand bzw. „Fallen".
- **TOF 8×8** – das 8×8-Gitter ihres Abstandssensors: rot ein Hindernis (heller, je näher), grün Boden,
  gelb zu nah, dunkel nichts; dazu der kleinste Abstand in echten Metern oder „clear".

![Die Micro Duck beim Spielen mit Kopfkamera und ToF-Gitter (Ente in der eigenen Gestaltung dieses Buchs)](bilder/duck-play.png)

---

## 11.14 Policies laden (jump.onnx, play.onnx)

Die Bewegungen der Ente sind neuronale Netze im ONNX-Format. Das Laufnetz `velstand.onnx` ist fest im
Modul eingebaut; alle anderen lädt die Seite nach, sobald ein Level eine Ente enthält:

| Netz | Herkunft | Ordner auf dem Server | Wofür |
|---|---|---|---|
| Pollens Satz (`roller.onnx`, `roulade.onnx`, `ball_kick_left.onnx`, `ball_kick_right.onnx`, `alpha_*.onnx`, `roller_crouch.onnx` …, laut Manifest) | Pollen Robotics, `kern/third_party/microduck/` | `microduck/` | Sitzen, Aufheben, Kicken, Rollen, Rollschuhe, ältere Gangart |
| `jump.onnx` | Kern Lehrer, `kern/policies/` | `policies/` | der Sprung (Taste 7, Touch-Knopf **HÜPFEN** (HOP), Robot Move „jump") |
| `play.onnx` | Kern Lehrer, `kern/policies/` | `policies/` | die Spielschicht: Ball kicken, Socke aufheben, wegtragen, ausspucken (nur mit dem Feld **Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)** (Kern extras (ball play, sock fetching, beak in the pick))) |

So läuft das Laden:

1. Die Engine meldet über `kern_studio_pending_policies()`, welche Dateien das Level braucht und noch
   fehlen.
2. Die Seite holt jede Datei: Kerns eigene Netze aus `policies/`, Pollens aus `microduck/`, jeweils neben
   der Seite. Die Entwicklungsseite liest die Kopien im Repository (`kern/third_party/microduck/`,
   `kern/policies/`), ebenso die Single-File-Version in `generated/`.
3. `kern_studio_load_policy(file, bytes)` übergibt die Bytes; die Engine prüft, ob es ein Netz ist, das sie
   ausführen kann, und registriert es unter seinem Dateinamen.
4. Scheitert eine Datei, versucht die Seite es nach 5 s, 10 s, 20 s … erneut, höchstens jede Minute.
   Bis dahin steht die Bewegung nicht zur Verfügung; die Ente bleibt stehen und sagt, warum.

**Selbst trainierte Netze:** Was der Kern Lehrer im Browser trainiert (Seite `/lehrer/`,
[Kapitel 14](14-lehrer.md)), speichert er im selben Browser (IndexedDB `kern-lehrer`, Speicher
`policies`, Schlüssel = Dateiname wie `jump.onnx`). Studio lädt diese Netze beim Start und erneut, sobald
der Lehrer ein neueres meldet (BroadcastChannel `kern-lehrer`). Ein auf demselben Gerät trainiertes Netz
ersetzt damit das mitgelieferte, ohne dass man eine Datei kopiert.

Beim Veröffentlichen kopiert die CI die Netze nach `editor/microduck/` und `editor/policies/`. Wer ein
exportiertes Spiel mit Ente selbst hostet, legt diese beiden Ordner neben die HTML-Datei
(Abschnitt 11.16). Zu `microduck/` gehören die Lizenz (`kern/third_party/microduck/LICENSE`)
und die NOTICE; ein Spiel mit dem Aussehen der Ente ist nur nicht kommerziell nutzbar
(Abschnitt 12.6 in [Kapitel 12](12-eigene-spiele.md)).

---

## 11.15 Speichern, Laden, Autosave und Einstellungen

### 11.15.1 Das Format `.klevel`

Ein Level ist eine JSON-Datei mit `"format": "kern-level"` und `"version": 1`. Sie enthält `name`,
`world`, die Editor-Kamera (`camera`), `nextId`, `actors`, `textures`, `sounds` und gegebenenfalls `logic`.
Texturen und importierte Sounds stehen als Data-URL darin; eine Datei ist damit vollständig. Eine Datei
einer neueren Studio-Version lehnt Studio mit „level was written by a newer Kern Studio" ab. Das Format
im Einzelnen beschreibt [Kapitel 8](08-spiel-laufzeit.md).

### 11.15.2 Speichern und Öffnen

| Aktion | Was passiert |
|---|---|
| **Datei › Speichern** (File › Save), Strg+S | mit File System Access (Chrome, Edge) beim ersten Mal ein Speicherdialog, danach direkt in dieselbe Datei; sonst ein Download `<Levelname>.klevel` |
| **Datei › Speichern unter...** (File › Save As...), Strg+Umschalt+S | immer mit Dialog bzw. Download |
| **Datei › Level öffnen...** (File › Open Level...), Strg+O | Dateidialog für `.klevel` und `.json` |
| Datei auf die Seite ziehen | `.klevel`/`.json` öffnet, Bilder werden Texturen, Audiodateien Sounds |

Der Dateiname ist der Levelname, in dem alles außer Buchstaben, Ziffern, `-` und `_` zu `_` wird.

**Android** hat kein `showOpenFilePicker`; Studio öffnet dort den normalen Dateidialog. Der kennt die Endung
`.klevel` nicht und würde mit einem Filter alle gespeicherten Levels ausgrauen. Auf Android filtert Studio
deshalb nicht; ist die gewählte Datei kein Level, erscheint „… is not a Kern level" (nicht übersetzt),
und der Editor läuft weiter.

### 11.15.3 Autosave und Wiederherstellen

Studio schützt ungespeicherte Arbeit, ohne bei jedem Neuladen zu fragen:

- Die Engine schreibt spätestens 2 s nach jeder Änderung eine Kopie, aber nur, solange das Dokument
  ungespeicherte Änderungen hat. Die Seite hält höchstens eine Kopie (`localStorage`,
  `kern-studio.autosave.v1`).
- Nach dem Speichern, oder wenn man alle Änderungen bis zum gespeicherten Stand rückgängig macht, löscht die Seite die
  Kopie wieder.
- Beim Verlassen der Seite, beim Wechsel in den Hintergrund und vor einem Update schreibt sie eine fällige
  Kopie sofort. Hat das Dokument ungespeicherte Änderungen, fragt der Browser beim Verlassen nach.
- Beim Spielen und Simulieren wird nichts geschrieben; eine fällige Kopie folgt nach dem Stoppen.
- Beim nächsten Start bietet Studio **Wiederherstellen** (Restore) oder **Schließen** (Dismiss) an.
  **Wiederherstellen** lädt die Kopie, das Level bleibt aber ungespeichert (Punkt am Tab), denn sein
  Inhalt steht in keiner Datei. Bis zum Speichern wird es bei jedem Start erneut angeboten. **Schließen** löscht die
  Kopie.
- Öffnet man ein anderes Level, bleibt die Kopie des vorigen erhalten, bis das neue eigene Änderungen
  bekommt.

Getrennt davon merkt sich Studio das zuletzt bearbeitete Level in seinem sauberen Stand
(`kern-studio.last.v1`) und öffnet es beim nächsten Start wieder.

### 11.15.4 Einstellungen

Studio merkt sich im Browser (`kern-studio.settings.v1`) **nur die Einstellungen, die man selbst geändert
hat**: den Dialog **Einstellungen** (Preferences), Viewport-Aufteilung, Raster, Welt-/lokale Achsen, Ton
an/aus und pro Viewport Ansichtstyp, Ansichtsmodus, Sichtfeld und die Einträge unter **Anzeigen** (Show).
Was man nie angefasst hat, folgt den Standardwerten der jeweiligen Version, auch wenn diese sich ändern.
Panelgrößen und Inhaltsablage speichert die Seite eigens (`kern-studio.layout.v1`). Dass Spielen mit
mehreren Spielern von selbst auf vier Viewports schaltet, gilt nicht als eigene Wahl.

**Bearbeiten › Einstellungen...** (Edit › Preferences...):

| Feld | Bereich | Standard |
|---|---|---|
| **Kamerageschwindigkeit (1-8)** (Camera speed (1-8)) | 1–8 | 4 |
| **Mausempfindlichkeit** (Mouse sensitivity) | 0,1–5 | 1 |
| **Maus-Y umkehren** (Invert mouse Y) | an/aus | aus |
| **Um die Auswahl kreisen (Alt+LMT)** (Orbit around selection (Alt+LMB)) | an/aus | an |
| **Am Raster einrasten** (Grid snapping), **Rastergröße** (Grid size) | 0,001–1000 m | an, 0,5 m |
| **Drehung einrasten** (Rotation snapping), **Drehschritt** (Rotation step) | 0,1–180° | an, 15° |
| **Skalierung einrasten** (Scale snapping), **Skalierungsschritt** (Scale step) | 0,001–100 | aus, 0,25 |

**Bearbeiten › Einstellungen zurücksetzen...** (Edit › Reset Settings to Defaults...) fragt in einem
Studio-Dialog nach und setzt alle Einstellungen, Panelgrößen und die Inhaltsablage zurück. Levels bleiben
unberührt.

---

## 11.16 Spiel exportieren

**Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...) oder der Paket-Knopf in der Toolbar
öffnet den Dialog **Spiel verpacken** (Package Game).

1. **Spieltitel** (Game title) eintragen (vorbelegt mit **Welteinstellungen › Spieltitel** (World
   Settings › Game title) bzw. dem Levelnamen).
2. **Verpacken** (Package) klicken.
3. Studio lädt `<Titel>.html` herunter und meldet „Packaged … (*n* MB)" (nicht übersetzt) mit
   **Spielen** (Play it), das die Datei in einem neuen Tab startet.

Die Datei ist ein vollständiges Spiel: Sie enthält das Engine-Modul als Base64, `kern.mjs` und `game.mjs`
als ein Inline-Modul und das Level als JSON. Sie braucht keinen Server und kein Netz und läuft auch von
`file://`. Beim Start lädt sie das Modul im Spieler-Modus (`kern_studio_init(…, player_only = 1)`): kein
Editor, nur das Level, sofort im Spiel. Das Modul enthält Pollens 3D-Modelle der Micro Duck
(CC BY-NC-SA 4.0, nicht kommerziell), außer es wurde kommerziell gebaut (`KERN_COMMERCIAL=1
tools/build_studio.sh`); siehe Abschnitt 12.6 in [Kapitel 12](12-eigene-spiele.md).

![Das exportierte Orb Run als eigenständige HTML-Datei](bilder/game.png)

**Bedienung im exportierten Spiel:**

| Eingabe | Wirkung |
|---|---|
| W A S D / Pfeile, Leertaste, Umschalt, Maus | wie beim Spielen im Editor (Abschnitt 11.4.2) |
| Klick | fängt die Maus für den Maus-Look |
| Esc | gibt die Maus frei und pausiert („Pausiert“, „Klicken zum Fortfahren“; englisch „Paused“, „Click to continue“) |
| P | Pause |
| R oder Enter nach Sieg/Niederlage, **Nochmal spielen** (Play again) | neu starten |
| Strg+R | neu starten |
| F8 | mit Beobachterkamera (Abschnitt 11.11.4): Spieler übernehmen und zurück |
| Gamepad | wie in Abschnitt 11.12.3 |
| Touch | Sticks und Knöpfe wie beim Spielen im Editor, oben links ein ↻ zum Neustart |

Unten steht ein Hinweis mit der Tastenbelegung, auf Touch-Geräten ausgeblendet.

![Das exportierte Spiel am Telefon mit Touch-Sticks](bilder/game-phone.png)

**Online spielen mit der exportierten Datei:** Die Datei versteht drei Parameter in der Adresse:
`?host=<raum>` bietet ein Spiel an, `?join=<raum>` tritt bei, `&relay=ws://…` nutzt ein Relay für andere
Rechner (Abschnitt 11.17). Scheitert der Verbindungsaufbau, spielt man allein weiter.

**Was nicht in der Datei steckt:** Die nachladbaren Netze der Micro Duck (Abschnitt 11.14).
Das Laufen funktioniert ohne sie, weil `velstand.onnx` eingebaut ist. Für Sitzen, Kicken, Springen und die
Spielschicht gehören die Ordner `microduck/` und `policies/` neben die HTML-Datei. Zu `microduck/` gehören
die Lizenz (`kern/third_party/microduck/LICENSE`) und die NOTICE; ein Spiel mit dem Aussehen der Ente ist
nur nicht kommerziell nutzbar (Abschnitt 12.6 in [Kapitel 12](12-eigene-spiele.md)).

---

## 11.17 Multiplayer

### 11.17.1 Mehrere Spieler im Editor

**Spielen › Anzahl der Spieler** (Play › Number of Players), wie Unreals „Number of Players", startet das
Spielen mit 2, 3 oder 4 Spielern: Die Welt des Levels ist ein Listen-Server, jeder weitere Spieler ein
Netzwerk-Client mit eigener Welt und eigenem Viewport, verbunden über ein Netz im selben Prozess.

1. **Spielen › Anzahl der Spieler › 2 Spieler (Listen-Server)** (Play › Number of Players › 2 Players
   (listen server)).
2. **Spielen**. Studio schaltet auf vier Viewports, falls nur einer offen war: Viewport 1 zeigt den
   Server, Viewport 2 den ersten Client usw. Jeder zeigt sein eigenes HUD.
3. Die Eingabe bekommt der Spieler des aktiven Viewports; die anderen stehen still. Ein Klick in einen
   anderen Viewport wechselt den Spieler.
4. **Spielen › Netzwerk-Emulation** (Play › Network Emulation) belastet die Verbindung zu den lokalen
   Clients mit Latenz, Jitter und Paketverlust, von **Perfekt** (Perfect) bis **Sehr schlecht (300 ms,
   20 % Verlust)** (Terrible (300 ms, 20% loss)).
5. Die Statusleiste zeigt Rolle, Spielerzahl, Ping, Verlust und Datenrate.

![Zwei Spieler im Editor: Listen-Server und lokaler Client in eigenen Viewports](bilder/studio-multiplayer.png)

### 11.17.2 Online zwischen Browsern

| Schritt | Host | Gast |
|---|---|---|
| 1 | dasselbe Level öffnen | dasselbe Level öffnen |
| 2 | **Spielen › Online-Spiel hosten...** (Play › Host Online Game...) | **Spielen › Online-Spiel beitreten...** (Play › Join Online Game...) |
| 3 | **Raum** (Room) wählen (Standard „kern"), **Relay (optional)** leer lassen oder eintragen | denselben Raum, ggf. dasselbe Relay |
| 4 | **Hosten** (Host) | **Beitreten** (Join) |

- **Ohne Relay** verbinden sich Tabs desselben Browsers auf demselben Rechner über einen
  BroadcastChannel; ein Server ist nicht nötig. Gibt es im Raum keinen Host, meldet **Beitreten** das
  nach 4 s.
- **Mit Relay** spielen Rechner im Netz miteinander: `node tools/net_relay.mjs [port]` starten (Standardport
  8790, braucht das npm-Paket `ws`) und seine Adresse eintragen, etwa `ws://host:8790`.
- **Spielen › Online-Spiel verlassen** (Play › Leave Online Game) verlässt das Spiel. Der Host bemerkt,
  wenn ein Gast geht.
- Ein Host im Hintergrund-Tab läuft über einen Worker-Takt weiter, damit seine Spieler nicht in einen
  Timeout laufen.

Das Relay ist absichtlich dumm: Es leitet nur Datagramme weiter. Bestätigungen, Wiederholungen, Kanäle,
Verbindungsaufbau und Timeouts erledigt Kerns eigener Netzwerk-Stack in C++ ([Kapitel 10](10-netzwerk.md)).
Logik läuft nur auf der Autorität; geänderte Eigenschaften und erzeugte Actors werden repliziert, auch an
Spieler, die später dazukommen.

---

## 11.18 Schmale Bildschirme und Telefone

Bis 900 px Fensterbreite oder auf Touch-Geräten mit höchstens 600 px Höhe schaltet Studio auf ein
kompaktes Layout:

- **Der Viewport bekommt die ganze Fläche.** Outliner, Details und Welteinstellungen öffnen sich über den
  Knopf **Outliner und Details** (Outliner and Details) als ein Blatt mit großen Tabs (**Outliner**,
  **Details**, **Welt** (World)): im Querformat rechts neben dem Viewport, im Hochformat von unten (64 %
  der Höhe). Ohne Auswahl öffnet es mit dem Outliner.
- **Spielen und Simulieren sind immer mit einem Tipp erreichbar.** Die Knöpfe **Im Viewport spielen**
  (Play in the viewport) und **Physik simulieren** (Simulate physics) liegen zusammen mit dem Knopf
  **Outliner und Details** in einem Dock am rechten Rand der Toolbar. Der Rest der Toolbar scrollt
  darunter seitwärts durch, das Dock bleibt stehen, auch bei offenem Blatt, offener Inhaltsablage oder
  offenem Logik-Editor. Beim Simulieren und Spielen stehen dort **Pause**, **Stopp** (Stop) und
  **Aussteigen / übernehmen** (Eject / possess) bzw. **Im Viewport spielen** zum Übernehmen; Knöpfe, die
  gerade nichts tun, sind ausgeblendet, die übrigen haben Touch-Größe (mindestens 38 px).
- Bis 900 px Breite blendet die Toolbar Raster-, Winkel- und Skalierungsauswahl sowie den Achsen-Knopf aus,
  die Statusleiste Konsole und Physik-Statistik.
- Die **Inhaltsablage** (Content Drawer) belegt höchstens knapp die halbe Höhe.
- Menüs öffnen Untermenüs an Ort und Stelle mit Zurück-Zeile (Abschnitt 11.2.1).
- Das Spielen füllt den ganzen Bildschirm mit **⟵ ENDE** (⟵ EXIT) und **❚❚ PAUSE** (Abschnitt 11.12.2).
- Die Seite scrollt nie seitwärts.

![Studio in einem 820 px breiten Fenster](bilder/studio-narrow.png)

![Telefon im Hochformat: das Dock mit den Knöpfen zum Spielen und Simulieren und dem Knopf Outliner und Details](bilder/phone-dock-390.png)

---

## 11.19 Architektur

### 11.19.1 Die Editorlogik ist C++

Studio ist die Klasse `kern::studio::Studio` (`kern/include/kern/studio/studio.h`). Sie besitzt das
Dokument, die Historie, die Auswahl, vier Viewports mit Kameras und Navigation, die Gizmos, Overlays,
das Details-Schema, den Katalog, Menüs, Dialoge, Einstellungen, Spielen/Simulieren und das
Netzwerkspiel. Die Seite enthält keine Editorlogik.

| Datei | Inhalt |
|---|---|
| `kern/src/studio/studio.cpp` | Dokument, Historie, Auswahl, Actor-Operationen, Spielen, Netzwerk |
| `kern/src/studio/viewport.cpp` | Kameras, Navigation, Finger, Picking, Rendern, Spieleingabe |
| `kern/src/studio/gizmo.cpp` | Verschieben, Drehen, Skalieren, Einrasten, Alt-Duplikat |
| `kern/src/studio/overlays.cpp` | Raster, Auswahl-Umriss, Symbole, Debug-Linien |
| `kern/src/studio/details.cpp` | Property-Schema, Details, Welteinstellungen |
| `kern/src/studio/catalog.cpp` | Katalog für Actors platzieren |
| `kern/src/studio/ui.cpp` | Tastatur, Menüs, Dialoge, Befehle, Panel-JSON |
| `kern/src/studio/logic_edit.cpp` | Logik-Editor |
| `kern/src/studio/settings.cpp` | Einstellungen |
| `kern/src/studio/api.cpp` | `kern_studio_*` C-ABI |

**Warum so?** Erstens läuft im Editor exakt der Code, der auch das exportierte Spiel ausführt; es gibt
keine zweite Implementierung, die abweichen könnte. Zweitens lässt sich der ganze Editor ohne Browser
testen: Die Unit-Tests (`kern/tests/run.sh studio`) bedienen Navigation, Gizmos, Katalog, Befehle,
Outliner, Spielen, Speichern, Autosave und Einstellungen direkt in C++.

### 11.19.2 Die C-ABI `kern_studio_*`

Die Seite spricht Studio über flache C-Funktionen an, die das WebAssembly-Modul exportiert. Zeichenketten
gehen als Zeiger und Länge in den linearen Speicher; JSON-Antworten kommen als Zeiger auf einen Puffer
zurück, der bis zum nächsten Aufruf gilt (`kern_studio_result_length()` nennt die Länge).

| Funktion | Zweck |
|---|---|
| `kern_studio_init(w, h, player_only)` | Renderer und Studio; `player_only` für exportierte Spiele |
| `kern_studio_frame(now_ms)` | Kameras, Simulation, Overlays, Rendern aller sichtbaren Viewports |
| `kern_studio_resize(w, h)` | Größe des Drawing Buffers |
| `kern_studio_set_viewport(i, x, y, w, h, css_w, css_h, dpr, visible)` | Rechteck eines Viewports (Ursprung unten links) |
| `kern_studio_pointer(i, type, x, y, button, buttons, mods, wheel)` | Zeiger: 0 down, 1 move, 2 up, 3 cancel, 4 wheel, 5 leave |
| `kern_studio_touch(type, id, x, y, time)` | Finger; 1, wenn die Touch-Steuerung ihn genommen hat |
| `kern_studio_key(key, code, down, mods, repeat)` | Tastatur; Antwort `{"handled": bool}` |
| `kern_studio_look(dx, dy)` | Maus-Look bei Pointer-Lock |
| `kern_studio_blur()` | Fokus verloren: alle Tasten los |
| `kern_studio_command(json)` | Befehl, Antwort `{"ok": …}` |
| `kern_studio_state()` / `kern_studio_status()` | Panel-Zustand bzw. Werte pro Frame |
| `kern_studio_revision()` | ändert sich, wenn `state()` neu gelesen werden muss |
| `kern_studio_take_ui()` | Aufträge an die Seite (Pointer-Lock, Zwischenablage, Menüs, Dialoge, Dateien …) |
| `kern_studio_pending_textures()`, `…_texture_source(id)`, `…_texture_pixels(…)` | Texturen dekodiert die Seite |
| `kern_studio_pending_sounds()`, `…_sound_source(id)`, `…_sound_pcm(…)`, `…_sound_failed(…)` | importierte Sounds dekodiert die Seite |
| `kern_studio_audio_init(rate)`, `…_audio_render(frames)`, `…_audio_prepare(ms)` | Mixer starten, Stereo-PCM ziehen, eingebaute Sounds vorbereiten |
| `kern_studio_pending_policies()`, `kern_studio_load_policy(file, bytes)` | Netze der Micro Duck |
| `kern_studio_net_push(from, bytes)`, `…_net_take()`, `…_net_out()`, `…_net_out_to()` | Datagramme für den Netzwerk-Stack |
| `kern_studio_take_autosave(now)`, `…_take_autosave_discard()`, `kern_studio_dirty()` | Autosave |
| `kern_studio_ao_set(…)` | Ambient Occlusion |
| `kern_studio_shadow_filter(taps)` | Filter des Sonnenschattens: 1, 4 (Telefone) oder 9 (Desktop) PCF-Taps |
| `kern_studio_context_lost()`, `…_context_restored()` | WebGL-Kontextverlust |

Alles, was man in Studio tut, ist ein Befehl als JSON-Objekt `{"cmd": …}`, auch jeder Menüeintrag: Ein
Menüeintrag im JSON von `state()` trägt den Befehl, den er ausführt. Die wichtigsten Gruppen:

| Gruppe | Befehle |
|---|---|
| Auswahl | `select`, `selectRange`, `selectAll`, `deselect`, `selectChildren`, `selectFolder` |
| Actors | `spawn`, `delete`, `duplicate`, `copy`, `cut`, `paste`, `pasteText`, `rename`, `attach`, `moveTo`, `setFolder`, `renameFolder`, `group`, `snapToFloor`, `hide`, `show`, `showAll`, `lock`, `focus`, `connect`, `setPlayer` |
| Eigenschaften | `set {path, value}`, `setWorld`, `addComponent`, `removeComponent`, `importTexture`, `removeTexture` |
| Historie | `undo`, `redo`, `historyJump` |
| Play | `play {mode}`, `stop`, `pause`, `stepFrame`, `eject`, `restart`, `keep`, `gameInput`, `gamepad`, `device`, `touchRelease`, `pointerCaptured` |
| Netz | `netPlayers`, `netSimulate`, `netAddress`, `netHost`, `netJoin`, `netStop` |
| Ansichten | `tool`, `space`, `snap`, `cameraSpeed`, `layout`, `maximize`, `activeViewport`, `viewType`, `viewMode`, `showFlag`, `gameView`, `viewCamera`, `fov`, `bookmark` |
| Panels | `filter`, `collapse`, `section`, `menu`, `dialog`, `submitDialog`, `catalog`, `clearLog`, `ui` |
| Dokumente | `new {template}`, `open`, `restore`, `save`, `saved`, `packageText`, `validate`, `console` |
| Ton | `previewSound`, `stopSound`, `importSound`, `removeSound`, `renameSound`, `mute` |
| Einstellungen | `settings`, `applySettings`, `takeSettings`, `resetSettings` |
| Hilfen | `project {id oder point}` liefert die Bildschirmposition |
| Logik | alle Befehle, die mit `logic` beginnen ([Kapitel 9](09-logik.md)) |

Eigenschaftspfade folgen der JSON-Form des Actors, zum Beispiel `transform.position`,
`components.mesh.material.color` oder `components.joints.0.lower`.

### 11.19.3 `studio.mjs` als dünne Schicht

`kern/web/studio/studio.mjs` lädt das Modul, legt die DOM-Panels an und führt eine einfache Schleife aus:

1. Pro Bildschirm-Frame Viewport-Rechtecke melden, Gamepad abfragen, `kern_studio_frame` aufrufen,
   Ton nachfüllen, Netzwerkpakete senden.
2. `take_ui()` abholen und ausführen: Pointer-Lock, Zwischenablage, Kontextmenü, Dialog, Speichern,
   Öffnen, Paketieren, Netz öffnen …
3. Hat sich `revision()` geändert, `state()` lesen und die Panels neu zeichnen; jedes Panel nur, wenn sich
   sein JSON geändert hat.
4. `status()` lesen und HUD, Statistik, Gizmo-Anzeige, Rahmenauswahl, Robot-Kamera und Touch-Steuerung
   aktualisieren.
5. Alle 200 ms Texturen, Sounds und Netze nachladen, alle 500 ms Autosave und Einstellungen prüfen.

Die Seite erledigt nur, was ein Browser kann und C++ nicht: Dateidialoge, Downloads, Zwischenablage,
Pointer-Lock, Bild- und Audio-Dekodierung, `localStorage`, Web-Audio-Ausgabe, Datagramme über
BroadcastChannel oder WebSocket und das Schreiben exportierter Spiele. `kern/web/studio/game.mjs` enthält
die gemeinsamen Teile (JS-Hülle `StudioEngine` um die C-ABI, Tonausgabe `KernAudio`, Netz `KernNet`,
Gamepad, Touch-Overlay, HUD) und ist zugleich der Spiel-Host exportierter Spiele.

Die Seite stellt außerdem `window.KernStudio` bereit: `command(c)`, `state`, `status`, `key(…)`,
`pointer(…)`, `viewportRect(i)` und weitere. Darüber bedient der Browser-Test
`kern/tests/e2e/studio_e2e.mjs` Studio mit Playwright: Menüs, Fliegen, Werkzeuge, Outliner, Details mit
Rückgängig, Inhaltsablage, Kontextmenü, Spielen, Simulieren, Viewport-Aufteilungen, Vorlagen, Logik, Micro Duck, Multiplayer,
Export, Wiederherstellen, Einstellungen, Telefon-Layouts und Android-Dateidialog. Die Bilder dieses Kapitels
stammen zum großen Teil aus diesem Test; in den Abbildungen zeigt die Ente die eigene Gestaltung des Buchs.

Wie man die Bausteine selbst nutzt, zeigt [Kapitel 12](12-eigene-spiele.md); alle Befehle und Funktionen
sammelt [Kapitel 17](17-referenz.md).

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
