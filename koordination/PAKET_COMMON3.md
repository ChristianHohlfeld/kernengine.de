# Gemeinsam für die Pakete der Runde „Alles“ (18.11.2026)

Chris (18.11., 13:42): „All. WE need all.“ – vollständige Konvertierung und Prüfung, kein „wartet“.
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad

Lies zuerst, ganz: $S/ANSATZ.md, dann in kern/games/spawncity/BLUEPRINTS.md den Abschnitt „Der Weg ab hier“
mit **„Kursprüfung 18.11.2026“** (bindend), $S/FORK_RULES.md. Dann dein Paket.

## Was diese Runde von den früheren unterscheidet (bindend)
1. **Eine Datei, ein Auftrag.** Du änderst am Spiel NUR die Dateien deines Pakets (und Aufrufstellen, die
   sich zwingend mitändern – so wenig wie möglich). Fällt dir etwas in einer fremden Datei auf: in den Bericht.
2. **Umschreiben in den Teilsatz („Absenkung“) nur mit Beweis.** Für jede Datei, deren Quelle du änderst:
   `tools/source_diff.py --base <Commit vor deiner Änderung> --test tests/lowered …` (Kopf der Datei lesen;
   Beispiele: tests/lowered/test_settings_lowered.cpp, test_city_jobs_lowered.cpp), der Differenztest muss
   grün sein (Bit für Bit, nie öfter Speicher). Gleiches Verhalten, gleiche Kosten, gleiche Gestalt –
   kein Cache, keine Kopie je Bild, keine Sonderregel. Kann das Werkzeug ein Paar nicht bilden, erweitere
   das Werkzeug allgemein (eine Regel, keine Namensliste).
3. **Konverter/Sprache/Engine nur, wo es eine allgemeine Fähigkeit ist**, die jeder Teilsatz-Code braucht
   (dann: Knoten in jeder Stufe bitgleich – Interpreter, JIT, Klasse –, Engine-Test, nativer Fall in
   kern/tests/logic_cases.h, Doku in kern/docs/blueprint_cpp.md; nach neuen Knoten prüfen: der Fall mit
   jedem Knoten in kern/tests/test_main.cpp und die Palette des Editors, kern/src/studio/logic_edit.cpp).
4. **Zahlen der Sprache: 64 Bit. Kein Float32.** Nicht verhandelbar (Kursprüfung, Punkt 1).
5. Firmware (kern/games/spawncity/firmware), die 22 „Unchanged from the original“-Webdateien, Box3D,
   Gewalt/Passanten: nicht ändern.

## Arbeitsweise
- Worktree: `git -C /home/claude/kw fetch -q origin && git -C /home/claude/kw worktree add /home/claude/w-<paket> -b work-<paket> origin/work`.
  Nur auf deinen Zweig pushen (nie main/work, kein Force-Push). Kleine Commits, jeden fertigen Teil pushen.
  Commit-Nachrichten deutsch, am Ende:
    Co-Authored-By: Claude Fable 5.2 <noreply@anthropic.com>
    Claude-Session: https://claude.ai/code/session_01K2YidUPFTJShcS1fG6cZzJ
- Stand der Liste: $S/final/f9/convert.log (Schlusslauf f9 auf work 5dcd9eb, 18.11.): je Einheit jede Funktion
  mit „written“ oder ihrem Grund. $S/f9_roots.txt: die Grundursachen. Dein Paket nennt deine Funktionen.
  Folgefehler („it calls X, which waits“, „… is not written“) verschwinden mit ihrer Ursache – prüfe am Ende
  mit einem Probelauf, dass in deinen Dateien NICHTS mehr wartet, auch nicht im Test
  („its test cannot compare it yet“ zählt als wartend).
- Die Maschine hat 2 Kerne und 8 GB, mehrere Pakete laufen parallel: Probeläufe des Konverters mit ≤3
  Quelltexten (bei Game-Teilen src/game.cpp dazu), KERN_JOBS=1, unter `flock $S/heavy2.lock` (gerade
  Paketbuchstaben B, D) bzw. `flock $S/heavy3.lock` (A, C, E). Keine vollen Suiten, keine vollen Läufe.
  Syntax je Datei immer. Der Aufruf des Konverters steht in $S/final.sh.
- Bericht am Ende (kurz, deutsch): Zweig + letzter Commit; je Datei geschrieben/verglichen vorher → nachher;
  welche Absenkungen (mit Differenztest: Zahl der Paare, grün?); was an Konverter/Engine allgemein dazukam;
  was noch wartet und warum es nicht ging (nur mit zwingender Begründung).

## Nachtrag 15:40 – Probeläufe auf mag (56 Kerne) statt hier
`$S/mag.sh probe origin/work-<paket> src/a.cpp src/b.cpp [src/game.cpp]` rechnet den Konverter über diese Quelltexte auf mag
(etwa 4 Minuten statt 20; bis zu 6 Aufträge zugleich) und gibt das Ende des Protokolls aus; alles liegt danach in $S/mag/<name>/
(convert.log.xz, texts/). Vorher deinen Stand pushen – mag rechnet, was auf GitHub steht. Ebenso
`$S/mag.sh engine origin/work-<paket> [filter]` (Engine-Suite) und `$S/mag.sh spiel origin/work-<paket> [filter]` (Spiel-Suite,
auch die Differenztests tests/lowered). Die Sperren heavy2/heavy3 braucht es dafür nicht. Hier lokal nur noch Syntax je Datei.
Mit Timeout aufrufen (Probe bis 15 Minuten, Suiten bis 60) oder im Hintergrund.

## Nachtrag 16:20 – Nachfolger
Wenn dein Zweig work-<paket> und dein Worktree /home/claude/w-<paket> schon bestehen, bist du der Nachfolger eines Agenten, der an diesem
Paket gearbeitet hat: lege nichts neu an. Lies zuerst `git -C /home/claude/w-<paket> log --stat origin/work..HEAD` (jede Commit-Nachricht ganz –
sie sagt, was gelöst ist und wie) und, falls vorhanden, $S/uebergabe/<paket>.md. Arbeite dort weiter. Was der Vorgänger gelöst hat, löst du
nicht noch einmal; was er halb hat, prüfst du, bevor du darauf baust. Ergebnisse früherer mag-Aufträge liegen in $S/mag/ (Name beginnt mit dem Zweig).
Der Koordinator überwacht dich: melde dich mit SendMessage an main, sobald (a) ein Teil fertig und bewiesen ist, (b) du eine Entscheidung
brauchst, die über dein Paket hinausgeht, (c) du zweimal hintereinander am selben Grund scheiterst – nicht weiterprobieren, melden.

## Nachtrag 16:50 – solange mag nichts liefert
mag steckt bis zum Ende des vollen Laufs von 15:39 in der alten Schleife (danach, oder sobald Chris die neue startet, laufen alle wartenden
Aufträge von selbst, bis zu 6 zugleich). Bis dahin darfst du lokal proben: 1–3 Quelltexte, KERN_JOBS=1, unter flock heavy2.lock oder heavy3.lock
(nimm die, die frei ist: `flock -n`), TEST_KEEP/TEST_ERRORS nach Bedarf. Keine Suiten, keine vollen Läufe lokal. Frag dafür nicht nach.
Auf work sind seit 16:48 zwei Korrekturen, die jeder Lauf braucht (hol sie dir: git merge origin/work): 52a9275 (Konverter: Eintrag einer
Liste in einer Liste, dem eine ganze Liste zugewiesen wird) und 3b355c4 (Leser: lokale Map/Set) – ohne sie weist der Leser das Level ab.

## Nachtrag 17:10 – sparsam arbeiten (das Kontingent ist knapp), ohne an der Strenge zu sparen
- Ausgaben klein halten: Protokolle nie ganz lesen – `grep`/`tail`/`sed -n` auf das, was du suchst; große Dateien (cpp_to_blueprint.py,
  notation.cpp, test_main.cpp, convert.log) nur in Ausschnitten (Grep, Read mit offset/limit).
- Nicht warten und nachfragen: ein Auftrag an mag = ein Aufruf mit langem Timeout (oder im Hintergrund), kein Schleifen-Abfragen.
- Erst denken, dann rechnen lassen: eine Probe je Hypothese, mehrere Korrekturen in einer Probe bündeln.
- Fortschritt in eine Datei, nicht an den Koordinator: schreibe deinen Stand laufend nach $S/stand/<paket>.md (kurz: fertig+bewiesen / in
  Arbeit / wartet worauf). Eine Nachricht an main nur noch für: (a) eine Entscheidung, die du brauchst, (b) „bereit zum Mergen“ mit Beweis,
  (c) einen Befund, der andere Pakete betrifft. Kein Zwischenbericht sonst.
- An der Strenge wird nichts gespart: jeder Beweis bleibt (bitgleich, Differenztest, Suiten) – er wird von mag gerechnet, nicht von dir begründet.
