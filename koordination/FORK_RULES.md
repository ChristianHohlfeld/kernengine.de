# Regeln für alle Forks (Schreibphase, 2026-11-14)

Auftrag von Chris: „Nimm mehr Agenten, mach es heute fertig. Egal wie, ohne Kompromisse, keine
Shortcuts, keine Hacks." / „alles fertig schreiben, ohne jedes Mal zu testen … Tests am Ende, wenn
alles fertig ist" / „Fortschritt immer pushen regelmäßig".

S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad

## Wo du arbeitest
- Nur in deinem eigenen Worktree `/home/claude/w-<paket>`. Lege ihn als Erstes an:
  `git -C /home/claude/kw worktree add /home/claude/w-<paket> -b work-<paket> 6783e94`
  (das ist der Stand von origin/work), dann arbeite ausschließlich dort – jeder Befehl mit
  `cd /home/claude/w-<paket> && …` oder absoluten Pfaden dorthin.
- `/home/claude/kernengine.de` (Testbaum) und `/home/claude/kw` (Arbeitskopie des Hauptlaufs)
  änderst du nicht: keine Dateien dort schreiben, kein checkout/reset/merge dort.
- Kleine Commits, **nach jedem sinnvollen Teil pushen**: `git push -u origin work-<paket>`.
  Nie auf main oder work pushen, kein Force-Push. Jede Commit-Nachricht endet mit
  `Co-Authored-By: Claude Fable 5.2 <noreply@anthropic.com>` und
  `Claude-Session: https://claude.ai/code/session_01K2YidUPFTJShcS1fG6cZzJ`.
- Eigene Hilfsdateien nur unter `$S/<paket>/`.

## Schreibphase: keine Prüfläufe
- Verboten: `pipeline.sh`, `kern/tests/run.sh`, `run_tests.sh`, `inc.sh`, `engine_objs.sh`, volle
  Konvertierung, alles was länger als ein paar Minuten rechnet. Die Maschine hat 2 Kerne und 8 GB,
  und sieben Läufe teilen sie sich.
- Erlaubt, als schnelle Kontrolle:
  - eine einzelne Datei übersetzen, nur Syntax:
    `nice c++ -std=c++20 -fsyntax-only -fno-exceptions -fno-rtti -Wall -Wextra -Wno-unused-parameter -Ikern/include -Ikern/third_party/box3d/include -Ikern/games/spawncity/src <datei>`
  - `python3 $S/why_struct.py <dein worktree>/tools kern/games/spawncity/src/<datei>.cpp <Klasse>… -- -I../../include -I../../third_party/box3d/include -Isrc`
    (im Verzeichnis `kern/games/spawncity` starten): jedes Feld ohne Typ, mit Grund.
  - Konverter-Probelauf auf **höchstens 3** Quelltexten, `KERN_JOBS=1`, im Verzeichnis
    `kern/games/spawncity` deines Worktrees:
    ```
    D=$S/<paket>/dbg; rm -rf $D; mkdir -p $D/t/converted $S/shared-tmp
    git show HEAD:kern/games/spawncity/level/spawncity.klevel > $D/l.klevel; cp tests/*.h $D/t/
    FUNCTIONS=1 KERN_JOBS=1 TMPDIR=$S/shared-tmp nice python3 ../../../tools/cpp_to_blueprint.py --convert sc --level $D/l.klevel --test $D/t/converted --texts $D/texts -I../../include -I../../third_party/box3d/include -Isrc -- src/a.cpp > $D/out.log 2>&1
    ```
    Die Ausgabe nennt je Funktion „written" oder den Grund, warum sie wartet.
- **Jeder schwere Befehl** (Konverter, why_struct, Übersetzen von mehr als einer Datei) läuft
  unter der gemeinsamen Sperre: `flock $S/heavy.lock <befehl>`. Nie zwei davon gleichzeitig.

## Was sauber heißt
- Blueprint == C++ auf Basis der erlaubten Teilmenge. Für jede Stelle, die wartet, gibt es zwei
  Antworten (BLUEPRINTS.md): eine allgemeine Fähigkeit der Sprache/des Konverters – oder die Stelle
  wird in erlaubtes C++ **gleicher Kosten und bit-genau gleichen Verhaltens** umgeschrieben.
  Nichts von SpawnCity wandert in die Engine. Keine Sonderregeln für einzelne Funktionen, keine
  Namenslisten, kein Caching, keine Toleranzen.
- Umgeschriebenes C++ bleibt normales, lesbares C++: Index statt Zeiger in den eigenen Zustand,
  Referenz statt Zeiger auf genau ein Objekt, Wert statt Pimpl-Zeiger, Konstanten im Header.
  Kommentare im Stil der Datei (englisch, erklärend), keine Hinweise auf „Agent"/„Fork".
- Nicht anfassen: Box3D, die 22 als „Unchanged from the original SpawnCity" markierten Web-Dateien,
  Runden/XP/Lobby/Missionen (nicht wieder einbauen), Gewalt und Passanten bleiben wie sie sind,
  TON/MUSIK/RESET bleiben, wo sie sind. Kein Changelog-Eintrag, kein BLUEPRINTS.md – das macht der
  Hauptlauf beim Zusammenführen.
- Ändere nur, was zu deinem Paket gehört. Brauchst du etwas aus einem anderen Bereich (eine
  Fähigkeit des Konverters, der Engine, eine Änderung in einer Datei eines anderen Pakets), dann
  ändere es nicht selbst, sondern schreibe es unter „Bedarf" in deinen Bericht – außer dein Paket
  sagt ausdrücklich etwas anderes. Aufrufstellen deiner geänderten Signaturen passt du überall an.
- Spawne keine weiteren Agenten.

## Bericht (deine letzte Nachricht, knapp)
1. Zweig und letzter Commit.
2. Was geändert ist (Dateien, Klassen/Funktionen, die Regel dahinter).
3. Was `why_struct` / der Probelauf jetzt sagen (Zahlen).
4. Offen und warum; Bedarf an anderen Paketen.
5. Worauf der Schlusslauf achten muss (wo du dir am wenigsten sicher bist).
