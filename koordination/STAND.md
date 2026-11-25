# Stand (Koordinator, 25.11.2026 22:25) – kurz; Archiv: UEBERGABE.md
## Ziel (Chris, 25.11. ab 21:53 – gilt vor allem Früheren)
EINE GUTE ENGINE zuerst: solide, stabil, einfach zu verwenden und zu verstehen, vollständiger Funktionsumfang, für VIELE Spiele. Kern: C++-Teilsatz == Blueprint
(Bit für Bit, beide Richtungen), Physik, Editor. SpawnCity ist ein Produkt aus der Engine und ihr Prüfstein – KEINE 1:1-Kopie des alten Spiels, Fehler des
Originals werden behoben. Der Koordinator entscheidet frei und vollständig für die Qualität der Engine. Geschwindigkeit danach.
Regel je Fähigkeit: bräuchte ein zweites, ganz anderes Spiel das genauso? Ja → kern/ (allgemein, ohne Wort von SpawnCity, Engine-Test, Doku, im Editor sichtbar).
Nein → Spiel. Nichts in die Engine nur wegen einer Eigenheit des Spiels. Ein Weg je Sache, ausdrücklich statt still, Fehlermeldungen mit Stelle und Grund.
Regeln der Agenten: $S/GEMEINSAM.md (Nachträge bis 22:05). $S = /tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad

## Bewiesen / live
- main 9ac74916 = Abschluss 1 (1.763/1.847 Funktionen als Blueprints, live). work 05b10cd1+: Editor öffnet das ganze Projekt (CI 45662141502), CI-Probe-Job.
- Seit Abschluss 1 sonst NICHTS Neues bewiesen. Alles Weitere liegt auf Zweigen.

## Läuft
- mag: Kandidat 4b work-kandidat-2115-voll-211441-4258 (Zweigstände 20:45 + Leser-Fix, mit main.cpp; allein auf mag). Liefert Beweis + neues Level + Warteliste.
  Kandidat 2 scheiterte am Leser (Game mit Engine-Platz; Fix work-leser 61d13f81, nur per Engine-Test belegt). cap 126. $S/mag.sh, $S/ci_nach_konvert.sh.
- Agenten (Opus 5.5 medium; Auftrag $S/AUFTRAG_<name>.md, Stand $S/stand_<name>.md):
  einstieg4 (work-einstieg2, viel lokal ungepusht): NEUER SCHNITT – Projekt-Datei .kproject über den Levels (Einstellungen, Levels, Start-Level, Programm-Blueprints),
    Knotentyp = Blueprint des Projekts (Daten, Library je Projekt), ein Einstieg kern::run(App&)/kern::run(Project) ohne schwache Symbole, Eingang „world“ an
    den Akteur-Knoten. Zuerst Doku-Abschnitt, dann bauen; alter Weg (project.program, --split-program, schwache Symbole, register_node_type) wird zurückgenommen.
  enginelib (work-enginelib): Engine-Bibliotheken aus der Engine statt als Kopien im Level; „Engine gewinnt“ bei alten Levels → macht heutige Levels wieder gültig
    (heute bauen alle Spiel-Suiten rot: logic_Graph 11/12, game_Level 9/10).
  test2 (work-test2): Test-Schreiber – offen 'command'/kern::game, game()↔namespace game, „too many errors“, NodeKind; volle GitHub-Probe 45701448388 läuft.
  umschalten-b (work-umschalten-b): Gesamtprobe – das ganze Programm als Klassen baut, linkt, läuft 600 Ticks nativ (Gerüst); Web-Bau läuft. $S/gesamtprobe.md.
  pruefer: unabhängige Prüfung aller Entwurfsentscheidungen von heute → $S/entwurf_pruefung.md (abgleichen, VOR dem Mischen umsetzen).
- Fertig/pausiert: tests (work-tests: 19/21 Spieltest-Dateien auf Klassen; wartet auf neues Level), tempo (work-tempo2: Konverter 46 statt 65 min, byte-gleich
  belegt; Testrahmen belegt), umschalten-c (Exporte, work-umschalten-c 405fc269), belege (beendet; Differenztests sind nur noch Werkzeug), game*, rest, engine, leser.

## Plan für die Engine (aus $S/grenze.md – Bestandsaufnahme Engine/Spiel: 28+39+23 Posten)
1. Laufendes: Beweis (4b), Einstieg neu geschnitten, Test-Schreiber, Engine-Bibliotheken. 2. Offenes Komponenten-/Steuerungsmodell (registrierbar, C++ und Blueprint).
3. Spielspezifisches aus kern/ heraus (Soldat/Waffen/HUD, Micro Duck/Walker, Stadt-Komponente, SpawnCity-Konstanten) in Beispiel-Bibliotheken; Test: kein Spielname im Kern.
4. Grundfunktionen: Eingabe-Aktionen, UI-Widgets, Spielstand, Levelwechsel, Mesh-/Skelett-Import. 5. Allgemeines aus SpawnCity hochziehen (mvt/tiles, perf,
   road_graph, QR; dann car, ragdoll, terrain, net_session); doppelte Wege abbauen. 6. Einstieg für ein zweites Spiel: neutrale Doku, drei Vorlagen als Prüfsteine.
Unsaubere Entscheidungen von heute, noch zu ändern: Editor-Blätter mit Schwelle 2000 (einheitlich machen), Engine-Objekte als nackte Int (typisierte Handles),
kern_text_length („letzter Text“, globaler Zustand), Hilfen entfernen (all_program-Angleichung, CI-Eingang beleg, embed --without Source nach dem Schalten).
Offen: Zahlen nur 64 Bit vs. Geschwindigkeit (Performance-Schritt, mit Messung); Konverter kein Fixpunkt über sein Level ($S/befund_fixpunkt.md).

## Nächste Schritte
1. 4b auswerten → $S/k4.log, $S/kandidat4_wartet.txt → test2. Grün → nach work (Absenkungen brauchen keinen Differenz-Beleg mehr: Spieltests + e2e + Vergleich).
2. Kandidat 5: + tempo2, test2, tests, umschalten-b, umschalten-c, enginelib (sobald Suite wieder baut). 3. Prüfer-Liste umsetzen. 4. Engine-Plan Punkt 2 ff.
