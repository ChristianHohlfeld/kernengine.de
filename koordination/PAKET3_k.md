Paket „k“ (die Kette). Lies zuerst $S/PAKET_COMMON3.md (S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad) ganz, samt Nachtrag 15:40 (mag). Zweig work-k, Worktree /home/claude/w-k.

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
