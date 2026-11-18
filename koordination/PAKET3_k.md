Paket „k“ (die Kette). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz, samt Nachtrag 15:40 (mag). Zweig work-k, Worktree /home/claude/w-k.

Befund (18.11., 16:07, mag mit 56 Threads): Im vollen Lauf wandelt tools/cpp_to_blueprint.py die Einheiten in der Reihenfolge ihrer Aufrufe um
(main: „the Blueprints, each after those it calls“ – ready/running/done, cut_calls, „call each other: X goes first“, „again“). Die rund 40
Game-Teile rufen einander und laufen darum fast nacheinander: 70 % der CPU sind frei, die Konvertierung dauert auf 56 Threads so lang wie auf 2.

Auftrag: Die Konvertierung nutzt alle Kerne – ohne dass sich am Ergebnis ein Byte ändert.
- Finde, was eine Einheit von den Einheiten, die sie ruft, wirklich braucht (tables/exports: welche Funktionen dort geschrieben sind, ihre
  Gestalt), und trenne das vom teuren Teil (Blueprint schreiben, Tests schreiben und übersetzen). Was nur vom Plan abhängt, läuft für alle
  Einheiten zugleich; was vom Ergebnis anderer abhängt, bekommt genau das – so früh wie möglich (z. B. erst die Frage „was wird geschrieben“
  als Fixpunkt über alle Einheiten, dann Schreiben und Tests aller Einheiten parallel; oder die Tests – das Übersetzen – vom Schreiben getrennt
  in einen eigenen parallelen Durchgang). Auch innerhalb einer Einheit: Teststücke werden parallel übersetzt, wo sie unabhängig sind.
- Kein Zwischenspeicher zwischen Läufen (BLUEPRINTS.md: „Kein Zwischenspeicher im Konverter“). Innerhalb eines Laufs nichts doppelt rechnen.
- Beweis: der volle Lauf vor und nach deiner Änderung schreibt dasselbe – level/spawncity.klevel und jede Datei in tests/converted byteweise
  gleich, convert.log gleich bis auf die Reihenfolge der Einheiten. Rechne beides auf mag: `$S/mag.sh voll origin/work` gibt es schon als
  Branch (frage mit `git ls-remote origin 'refs/heads/mag/ergebnis-work-voll-*'` nach dem neuesten, dessen mag-log/when „convert rc=0“ nennt)
  bzw. `$S/mag.sh voll origin/work-k` für deinen Stand; vergleiche die Bäume (git diff zwischen den ersten Commits der beiden Ergebnis-Branches,
  auf demselben Ausgangsstand). Nenne im Bericht die Dauer der Konvertierung vorher und nachher (mag-log/when, convert.log „real“).
- Ändere nichts an dem, WAS der Konverter schreibt (das gehört anderen Paketen, die gerade an Unit/TestWriter arbeiten): du änderst nur, WANN
  und WO gerechnet wird. Halte deine Änderungen im Ablauf (main, convert_unit, job_of) lokal, damit das Zusammenführen sauber bleibt.

## Nachtrag 16:40 (Koordinator) – erst eindeutig, dann schnell
Lies $S/uebergabe/k.md (Vorgänger: nur untersucht; Mess-Zweig work-k-zeit, Probe-Ausgabe $S/k/probe1.out bzw. $S/mag/work-k-zeit-probe-*).
Sein wichtigster Befund ändert den Auftrag: **das Ergebnis des Konverters hängt heute vom zeitlichen Ablauf ab** (eine laufende Einheit wird
nicht neu geprüft, wenn ihre Abhängigkeit inzwischen fertig wird; in f9 fällt Game/game_look_cpp von 7/13 auf 0/13, nachdem game_vr fertig ist;
der Wortlaut „is not written“/„which waits“ hängt davon ab, wer zuerst eine Tabelle hatte). Ein Lauf mit 2 Kernen und einer mit 56 können also
Verschiedenes schreiben. Das ist ein Fehler, kein Detail: der Konverter muss für denselben Quelltext immer dasselbe schreiben.
Neues Ziel, in dieser Reihenfolge:
1. **Eindeutig:** dasselbe Ergebnis bei jeder Zahl von Kernen und jedem Ablauf – Level, tests/converted und convert.log (bis auf die Reihenfolge
   der Einheiten im Protokoll) byteweise gleich bei KERN_JOBS=2 und KERN_JOBS=56. Runden mit eingefrorenem Stand, Ergebnisse am Rundenende
   sortiert angewendet (dein Vorgänger schlägt das vor) sind der naheliegende Weg; ein Fixpunkt: es wird so lange neu umgewandelt, bis sich
   nichts mehr ändert – und was am Ende geschrieben ist, ist das Größte, was geschrieben werden kann (keine Funktion wartet nur, weil ihr
   Partner später dran war). Kein Zwischenspeicher zwischen Läufen.
2. **Schnell:** innerhalb einer Runde alle Einheiten zugleich.
Beweis: zwei volle Läufe desselben Stands mit verschiedener Kernzahl (lauf2.sh auf mag nimmt für „voll“ 2/3 der Kerne; für den zweiten Lauf
setze KERN_JOBS im Auftrag nicht – bitte den Koordinator um eine Auftragsart mit fester Kernzahl, oder rechne den kleinen Lauf hier lokal unter
flock $S/heavy.lock mit KERN_JOBS=2 nach $S/final.sh) → byteweise gleich; und gegenüber dem Basislauf (mag/ergebnis-work-voll-153849-*,
sobald fertig) ist keine Funktion verloren (jede dort geschriebene ist geschrieben; mehr dürfen es sein – liste sie). Dauer vorher/nachher.

## Nachtrag 17:50 (neuer Koordinator) – Entscheidung E3
Die beiden Beweisläufe deines Vorgängers laufen seit 17:28:55 auf mag, beide auf e5f2bf1: `work-k-konvert-165842-30470` (8 Kerne) und
`work-k-konvert-165847-30555` (40 Kerne); dazu `work-k-probe-162316-2567`. Der Koordinator vergleicht die ersten Commits der beiden Ergebnis-Zweige selbst und gibt
dir das Ergebnis mit. Vor dem Mergen: (1) origin/work in work-k einmischen (Merge-Commit; work ist 9 Commits weiter), (2) auf dem eingemischten Stand der Beweis
neu: `konvert` mit 8 und mit 40 Kernen byteweise gleich (Level, tests/converted; convert.log bis auf Runden-/Zeitzeilen), (3) gegen den gültigen vollen Lauf auf
`work` (der Koordinator nennt dir den Ergebnis-Zweig) ist keine Funktion verloren – was mehr geschrieben ist, listest du; (4) jede Zeile
„was written and waits now“ ist ein Befund am Inhalt: mit Einheit, Funktion, Grund nach `$S/stand/k.md` unter „BEFUND FÜR <paket der Datei>“; (5) Dauer vorher/nachher.
Greift die Sicherung (ein Gesamtstand kehrt wieder), ist das ein Befund mit Namen, kein vorgesehener Ausgang. Monotonie wird nicht erzwungen.

## Nachtrag 22:25 – NEUER AUFTRAG für k: die Regeln des Konverters, die die Spiel-Suite rot machen
Der Ablauf (oben) ist erledigt und bewiesen und steht in work-zusammen. Der Zweig work-k / Worktree /home/claude/w-k dient jetzt diesem Auftrag:
**Jede Abweichung der Spiel-Suite, bei der Interpreter = Klasse ≠ Quelltext ist – also eine Regel des Konverters (oder eine Absenkung) falsch übersetzt –, finden,
allgemein beheben und Bit für Bit belegen.** Nicht dein Gebiet: der Test-Schreiber (TestWriter: t1, t2), Stellvertreter/Engine-Knoten (d), Leistung (p).
1. Zuerst: `git merge origin/work-zusammen` (8844531a oder neuer; work-k ist darin enthalten). Lies PAKET_COMMON3.md ganz (alle Nachträge, v. a. 21:10 und
   22:25), stand/KOORDINATOR.md (oberste Einträge), stand/t1.md „SUITE 203000“.
2. Arbeitsliste, in dieser Reihenfolge:
   - Gruppe 4: `flak_cell` 201/255 (combat_rules.cpp:239) – `snprintf("%lld", cy)` mit großem i64 (7626388851792824174): der Blueprint schreibt eine andere Zahl.
     Int ist exakt bis 2^53 – wie führt die Sprache ein i64 (Int64)? Die Regel für `%lld`/`%llu`/`%ld` muss den ganzen Bereich tragen.
   - Gruppe 5: `Car_new` 300/300 (car.h/car.cpp): in einer Liste von {Vec3, Vec3, Zahl} gibt der Quelltext 8, der Blueprint 0.
   - Gruppe 6: `CityStream_feed_terrain` 4/38, `CityStream_upload` 35/300 (t1s neue Anzeige nennt die erste abweichende Stelle – work-t1 e0e6c2ad).
   - Gruppe 7: `Drone_step` (drone.cpp): eine ganze Zahl im Zustand 20268 (Quelltext) gegen 85804 (Klasse).
   - Danach: was der Lauf „alle“ zeigt (`ergebnis-work-zusammen-voll-203000-750-alle-212005-5593`, sobald fertig: `./mag.sh holen <name>`; `alle-abweichungen.txt`,
     `einzeln.tar.xz`) – die Tests, die im ersten Lauf nie drankamen (u. a. alle `converted_Game__*`). t1 wertet ihn aus („SUITE 203000 – ALLE“ in stand/t1.md) und
     nennt je Gruppe das Gebiet; nimm, was „Regel des Konverters“ heißt. Ist eine Abweichung in Wahrheit ein Fehler des Tests: „BEFUND FÜR t1“ in stand/k.md und
     Nachricht an main; eines Stellvertreters/Engine-Knotens: „BEFUND FÜR d“.
3. Je Abweichung: Kleinstfall (eine kleine Funktion im erlaubten C++, die dieselbe Abweichung zeigt), die Regel in tools/cpp_to_blueprint.py finden, **allgemein**
   beheben (keine Sonderregel, keine Namensliste, keine Toleranz, kein Cache), Beleg: `$S/mag.sh vergleich origin/work-k <betroffene Quelltexte>` nennt in beiden
   Durchgängen keine Abweichung mehr für diese Funktion – und keine neue. Braucht es eine Absenkung der Quelle: nur mit grünem Differenztest (tests/lowered,
   tools/source_diff.py); die 22 unveränderten Web-Dateien, die Firmware und Box3D bleiben, wie sie sind.
4. tools/cpp_to_blueprint.py teilst du mit t1/t2 (TestWriter) – kleine Commits, oft `git merge origin/work-zusammen`; nach jedem belegten Fix: Commit, Push,
   stand/k.md nachführen („BEREIT ZUM MERGEN: <commit> – Beleg <auftrag>“), Nachricht an main. Der Koordinator mischt ein und rechnet `voll`.

