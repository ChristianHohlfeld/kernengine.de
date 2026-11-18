# Gemeinsam für die Pakete der Runde „Alles“ (18.11.2026)

Chris (18.11., 13:42): „All. WE need all.“ – vollständige Konvertierung und Prüfung, kein „wartet“.
S=/home/claude/kernengine.de/koordination

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
- Worktree: siehe „Nachtrag 17:45 – NEUE SITZUNG“, Punkt 1 (der Zweig work-<paket> besteht; früher: `worktree add … -b work-<paket> origin/work`).
  Nur auf deinen Zweig pushen (nie main/work, kein Force-Push). Kleine Commits, jeden fertigen Teil pushen.
  Commit-Nachrichten deutsch, am Ende:
    (siehe „Nachtrag 17:45“, Punkt 6)
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

## Nachtrag 17:45 – NEUE SITZUNG (gilt vor allem Früheren, wo es sich widerspricht)
Die Sitzung, in der die Nachträge oben geschrieben wurden, ist beendet; ihr Arbeitsordner, ihre Worktrees und ihre laufenden Agenten sind weg.
Du bist der **Nachfolger** auf deinem Paket in einer neuen Sitzung. Was bleibt: GitHub (dein Zweig `work-<paket>`), dieser Ordner `$S`
(= /home/claude/kernengine.de/koordination, der Ordner `koordination/` des Zweigs `koordination`) und die Übergabe deines Vorgängers `$S/uebergabe/<paket>.md`.
1. **Worktree neu anlegen** (der Zweig besteht – NICHT von origin/work neu abzweigen):
   `git -C /home/claude/kw fetch -q origin; git -C /home/claude/kw worktree add /home/claude/w-<paket> -B work-<paket> origin/work-<paket>`
   Arbeite nur dort. /home/claude/kw (Zweig work, Koordinator) und /home/claude/kernengine.de änderst du nicht. Der Klon ist vollständig (kein flacher Klon).
2. **Lies zuerst, ganz:** `$S/ANSATZ.md`, diese Datei, `$S/PAKET3_<paket>.md` (samt Nachträgen), `$S/uebergabe/<paket>.md`,
   `git -C /home/claude/w-<paket> log --stat origin/work..HEAD` (jede Nachricht ganz), in BLUEPRINTS.md „Der Weg ab hier“ samt „Kursprüfung 18.11.2026“.
   Was der Vorgänger gelöst hat, löst du nicht noch einmal; was er „unbewiesen“ nennt, prüfst du, bevor du darauf baust.
3. **Pfade des Vorgängers:** `$S/mag/<name>/` (alte mag-Ergebnisse) und seine Hilfsordner `$S/<paket>/…` gibt es nur noch, soweit sie in `$S` liegen
   (g1, g2, k, p, r, s5, pilot). Jedes frühere mag-Ergebnis holst du neu: `$S/mag.sh holen <name>` (der Name steht in der Übergabe; `$S/mag.sh stand` listet alle).
   `$S/final/f9/convert.log` ist wieder da (aus f9_convert.log.xz).
4. **mag (Proben und Suiten rechnet mag, nicht diese Maschine – hier 2 Kerne für alle):**
   - ablegen und warten in einem Zug nur, wenn es < 9 Minuten dauert (ein Vordergrund-Befehl endet nach 10 Minuten). Sonst:
     `MAG_NOWAIT=1 $S/mag.sh probe origin/work-<paket> src/a.cpp …` → gibt den Namen; dann `$S/mag.sh warten <name>` (wartet bis 9 Minuten; „läuft noch“ → noch einmal),
     oder zwischendurch weiterarbeiten und später `$S/mag.sh holen <name>`. Kein eigenes Schleifen-Abfragen mit git.
   - Arten: `probe <ref> <quellen…>` (Minuten; mit allen game*.cpp oder allen Quelltexten bis ~1 h), `spiel <ref> [filter]` (~20 min), `engine <ref> [filter]` (~35 min),
     `voll <ref>` (Konvertierung des ganzen Spiels + Spiel-Suite + Engine-Suite, ~1,5–2,5 h): **einmal als Abschlussbeweis deines Zweigs**, wenn deine Proben
     sauber sind und origin/work eingemischt ist – nicht zum Probieren. `konvert <ref> <kerne>` nur Paket k.
   - Ob eine rote Stelle in einem vollen Lauf von dir kommt, zeigt der Vergleich mit dem Basislauf auf `work`: `$S/mag.sh holen work-voll-162644-5528`
     (sobald fertig; convert.log.xz, suite.log.xz, engine.log.xz). Was dort genauso rot ist, ist nicht deins – nenne es trotzdem.
   - mag rechnet den Stand des Zweigs **zum Zeitpunkt des Starts** des Auftrags, nicht des Ablegens: erst pushen, dann ablegen; `mag-log/when` nennt den Commit.
   - Bündeln: eine Probe je Hypothese, mehrere Korrekturen in einer Probe. Lokal nur Syntax je Datei (`-fsyntax-only`) und Kleinstfälle; lokale Konverter-Proben
     (≤3 Quelltexte, KERN_JOBS=1, `flock $S/heavy2.lock` bzw. `heavy3.lock`) nur, wenn mag nichts annimmt.
5. **Git-Zugang der Sitzung:** höchstens 2 Übertragungen zugleich für alle Agenten zusammen. Scheitert fetch/push mit HTTP 429 („too many concurrent transfers“),
   mit „cannot lock ref“ oder „index.lock“: 5–10 s warten, wiederholen (bis zu sechsmal) – nie umgehen, nie aufgeben. Pushen: `git push -u origin work-<paket>`
   (nie main, nie work, kein Force-Push). Vor einem Merge von origin/work in deinen Zweig: `git fetch origin`; Merge-Commit, kein Rebase.
6. **Commit-Ende:** die Attributionszeilen, die deine eigene Sitzung dir vorgibt; gibt sie dir keine vor:
   `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>` und `Claude-Session: https://claude.ai/code/session_012NJs7i46AnXdFpunu1X3k9`.
7. **Stand und Meldungen:** Schreibe deinen Stand laufend nach `$S/stand/<paket>.md` (kurz: fertig+bewiesen mit Beweis / in Arbeit / wartet worauf) und halte
   `$S/uebergabe/<paket>.md` so aktuell, dass ein Nachfolger jederzeit übernehmen kann (Erkenntnisse über Ursachen ausschreiben). Der Koordinator liest beides.
   Du hast keinen Rückkanal während der Arbeit: **Brauchst du eine Entscheidung**, die über dein Paket hinausgeht, oder scheiterst du zweimal am selben Grund –
   bring den Stand in einen übersetzbaren Zustand, pushe, schreibe die Frage mit allem, was der Koordinator zum Entscheiden braucht, nach `$S/stand/<paket>.md`
   unter „ENTSCHEIDUNG NÖTIG“ und beende deinen Lauf mit genau dieser Frage als Bericht. Der Koordinator setzt dich mit der Antwort fort (dein Kontext bleibt).
   Arbeite aber vorher alles ab, was die Frage nicht berührt. **„BEREIT ZUM MERGEN“** (mit Beweis: mag-Auftrag/CI, Zahlen, die Commits) schreibst du als
   eigene Überschrift nach `$S/stand/<paket>.md`, sobald ein Teil bewiesen ist – und arbeitest weiter, wenn dein Auftrag mehr hat (der Koordinator liest die
   Stand-Dateien regelmäßig und mergt von dort); dein Lauf endet erst, wenn alles getan ist, du eine Entscheidung brauchst oder nur noch auf andere wartest.
   Befunde für andere Pakete: in `$S/stand/<paket>.md` unter „BEFUND FÜR <paket>“ und im Bericht.
   Dein letzter Text ist dein Bericht an den Koordinator; er beginnt mit einer dieser Zeilen: `BEREIT ZUM MERGEN: …`, `ENTSCHEIDUNG NÖTIG: …`, `WARTET AUF: …`
   oder `STAND: …`; dann Zweig + letzter Commit, die Beweise (Namen der mag-Aufträge, Zahlen), was offen ist und warum.
8. **Spawne keine weiteren Agenten.** Antworte und berichte auf Deutsch, knapp. „Sparsam arbeiten“ (Nachtrag 17:10) gilt weiter; an der Strenge wird nichts gespart.
9. **Diese Maschine: 2 Kerne, 8 GB, für alle Agenten zusammen.** Jeder Befehl, der einen Compiler startet (auch `-fsyntax-only`, `tools/engine_api.py`,
   `tools/blueprint_cpp.sh`, `tools/source_diff.py`, jede lokale Konverter-Probe), läuft unter einer der zwei Sperren: `flock $S/heavy2.lock <befehl>` oder
   `flock $S/heavy3.lock <befehl>` (nimm die freie: erst `flock -n -E 99 $S/heavy2.lock …`, bei 99 die andere ohne `-n`). Nie mehr als ein solcher Befehl von dir
   zugleich; nichts Schweres im Hintergrund liegen lassen. Zeitmessungen (Paket p): beide Sperren nehmen (`flock $S/heavy2.lock flock $S/heavy3.lock …`), kurz halten.
10. **Bekannt bei Sitzungsbeginn (18.11., 17:30):**
   - Seit dem 14.11. gibt es keinen gültigen vollen Lauf; Level und tests/converted auf `work` sind von f8 und passen nicht zu den Quellen – darum ist die
     Spiel-Suite auf `work` rot (bricht bei converted_car ab; mag `work-d-spiel-154354-17035`). Der volle Lauf auf `work` (90b5ab8) ist auf mag eingereiht
     (`work-voll-162644-5528`); der Koordinator bringt sein Ergebnis nach `work` und sagt Bescheid. `work-voll-153849-12713` ist ein abgebrochener Lauf (nie verwenden).
   - **Die Engine-Suite ist auf dem Stand von `work` (90b5ab8 = 30f63e4) auf mag ROT** (dreimal gleich): `blueprint_native_test`, Fall „Engine on the engine itself“,
     R129 (jobs_take_queued) – Klasse `[1,0,0,0,2,0,…]` (16 Einträge), durch die Tür `[0,0,0,0]`. In CI galt R129 als „nicht zweimal gleich durch die Tür“ und wurde
     nicht verglichen; auf mag (56 Threads) ist die Tür wiederholbar und die Klasse weicht ab. Paket d behebt das als Erstes. Bis dahin: eine Engine-Suite deines
     Zweigs, die NUR daran scheitert, ist für dein Paket kein Fehler – nenne es im Bericht so; jede andere Abweichung ist deine.
   - „Geschrieben und verglichen“ in einer Probe heißt nur: der Test übersetzt. Bit für Bit geprüft wird in der Spiel-Suite (`spiel`).

## Nachtrag 18:10 (Koordinator) – mag rechnet anders, und das Kontingent ist sehr knapp
1. **Proben haben seit 18:04 das Level** (lauf2.sh d6a7ddc: `--level` mit einer Kopie). Vorher wartete in JEDER mag-Probe jede Funktion, die den Blueprint eines
   Regel-Actors ruft (Controls, Traffic, JetSeats, Multiplayer, Lamps …), mit „… the Blueprint of the actor X, which the level does not have“ – ein Artefakt der Probe
   (Befund f2). Alle Proben, die vor 18:04 gestartet sind (`mag-log/when` nennt den Start), haben es: diese Zeilen sind keine Gründe, und was dahinter läge, zeigen sie
   nicht. Eine Probe liest ihre Blueprints jetzt am Ende auch ins Level ein – scheitert das, nennt `when` `convert rc≠0` und convert.log den Grund: das ist ein Befund.
2. **Ein voller Lauf (`voll`) liefert früher:** die Engine-Suite rechnet von Anfang an neben der Konvertierung; Level und Tests werden gleich nach der Konvertierung
   gepusht (Commit „… Level und Tests der Blueprints“, Protokoll in `mag-zwischen/`: `$S/mag.sh holen <name>` zeigt den Zwischenstand und legt ihn nach
   `$S/mag/<name>/zwischen/`). `mag-log/` kommt wie bisher am Ende. Gilt für Läufe, die nach 18:04 starten.
3. **Warten kostet:** auf mag nur in Stücken von höchstens 4 Minuten warten (`$S/mag.sh warten <name> 4`, wiederholen) und in der Zeit nichts Großes nachlesen.
   Keine Ausgabe über ~40 Zeilen. Keine Arbeit auf Verdacht: keine Änderung ohne Beleg aus einer Probe oder einem Kleinstfall.
4. mag ist voll (bis zu 17 Aufträge zugleich, darunter vier `konvert` und der volle Lauf): eine Probe über alle game*.cpp braucht zurzeit 45 Minuten und mehr.
   Lege nichts doppelt ab; `$S/mag.sh stand` zeigt, was läuft.

## Nachtrag 21:10 (Koordinator) – der zusammengeführte Stand ist die Basis; so wird ab jetzt bewiesen und gemergt
1. **Basis ist `origin/work-zusammen` (8874e79 oder neuer)**, nicht mehr dein alter Zweigstand: dort sind work (mit dem Fix von d), k, t1, t2, f2, g1, g2 und von f1
   der Konverter zusammengeführt. Drei doppelt gebaute Fähigkeiten stehen dort in EINER Fassung – baue nur auf diesen: `scope_for` (nicht `looked_from`),
   `case_block`/`leaves_case` für break mitten im switch-Fall (nicht `switch_break`), `stood` für Namen von Teilen eines Werts (nicht `where_of`). Dazu neu im
   Konverter: `settled_extern` (jede KERN_EXTERN-Erklärung einmal für das ganze Programm). **Als Erstes: `git fetch origin && git merge origin/work-zusammen`** in
   deinen Zweig (Merge-Commit), pushen. Die Absenkungen der Game-Dateien von g1/g2 und alles von f1 außer dem Konverter sind dort bewusst NICHT enthalten.
2. **Es gibt wieder einen gültigen vollen Lauf:** `work-zusammen-voll-203000-750` auf 8874e79 – `convert rc=0`, 1.632 von 1.844 Funktionen geschrieben (18.813 von
   30.978 Zeilen), 1.589 im Test; 118 Funktionen „its test cannot compare it yet“, 9 Rückfälle „was written and waits now“. Das ist deine Arbeitsliste:
   `$S/mag.sh holen work-zusammen-voll-203000-750` → `$S/mag/work-zusammen-voll-203000-750/` (nach Ende: convert.log.xz, suite.log.xz, engine.log.xz; schon vorher
   `zwischen/convert.log.xz` und `zwischen/texts/` = die Texte aller 156 Blueprints). Nur mit xzgrep lesen. `f9_*` ist damit überholt.
3. **Der Takt:** du behebst und belegst mit Proben (mit Level, Minuten; über alle Quelltexte ~25 min: aus kern/games/spawncity
   `$(ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp\|main.cpp")`), schreibst „BEREIT ZUM MERGEN“ mit Commits und Auftragsnamen nach
   `$S/stand/<paket>.md` und arbeitest weiter. **Den vollen Lauf macht der Koordinator**: er mischt die bereiten Zweige in `work-zusammen`, lässt `voll` rechnen
   (Konvertierung 23 min, Suiten ~20 min) und bringt den Stand nach `work`, wenn er grün ist. Du legst selbst KEINEN `voll` ab. mag ist zurzeit fast leer.
4. **Lesefehler lokal prüfen statt mit einem Lauf:** `python3 $S/lesen.py <worktree> <texts-ordner>` liest die Texte (einer Probe: `mag/<name>/texts`, eines vollen Laufs:
   `mag/<name>/zwischen/texts`) wie der Konverter ins Level, 1 Minute, unter einer Sperre (`flock $S/heavy3.lock …`, `TMPDIR=$S/shared-tmp`).
5. Rückkanal: du kannst dem Koordinator mit SendMessage an `main` schreiben – nur für eine Entscheidung, die dich wirklich aufhält. Sonst Stand-Datei.
6. Alles aus „Nachtrag 17:45“ und „Nachtrag 18:10“ gilt weiter (Worktree besteht schon; sparsam: Warten in Stücken von 4 Minuten, keine großen Ausgaben, keine Arbeit
   auf Verdacht; Sperren für lokale Compiler; nie main/work pushen; keine weiteren Agenten).

## Nachtrag 22:25 – „vergleich“: Bit für Bit in Minuten; mag ist überbucht
1. **Eine Probe beweist nichts über Werte** („geschrieben und verglichen“ heißt dort nur: der Test übersetzt). Die erste ausgeführte Spiel-Suite hat das gezeigt
   (stand/KOORDINATOR.md 21:25). Darum gibt es jetzt **`$S/mag.sh vergleich origin/work-<paket> src/a.cpp [src/b.cpp …]`**: der Konverter über diese Quelltexte
   (wie `probe`, auf dem Level des letzten gültigen vollen Laufs), danach werden ihre Tests gebaut und laufen **jeder für sich, gegen den Stellvertreter und gegen
   die Engine selbst** (`SC_ENGINE=real`); ein Abbruch oder Hänger (über 20 min) hält die anderen nicht auf. Ergebnis nach `./mag.sh holen <name>` in
   `$S/mag/<name>/`: `when` (convert rc, bau rc, alle rc + Summenzeile), `alle.txt` / `alle-real.txt` (je Test ok / FAIL / ABBRUCH / ZEIT), `alle-abweichungen.txt`,
   `alle-real-abweichungen.txt` (jede „calls differ“-Zeile), `alle-cost.txt`, `einzeln.tar.xz` (jede Ausgabe ganz), `convert.log.xz`, `texts/`.
   **Ein Fix an einer Abweichung ist erst belegt, wenn `vergleich` über die betroffenen Quelltexte in beiden Durchgängen keine Abweichung mehr nennt.**
   Der Beweis für `work` bleibt der volle Lauf des Koordinators.
2. **mag ist überbucht** (22:15: zehn Aufträge, ~100 Threads auf 56 – alles rechnet doppelt so lang). Darum ab jetzt: **keine Probe über alle Quelltexte**
   (das zeigt der volle Lauf des Koordinators); proben und vergleichen nur über die Quelltexte, die deine Änderung betrifft (bis ~6 Dateien). Höchstens **ein**
   eigener Auftrag zugleich je Agent; eine überholte Probe nicht durch eine zweite daneben ersetzen, sondern die laufende abwarten oder gebündelt neu ablegen.
3. Läufe von Agenten können abbrechen (Server-Neustart, Ende des eigenen Kontingents): **nach jedem abgeschlossenen Schritt committen und pushen** und die
   eigene Stand-Datei nachführen; die Übergabe-Datei (`uebergabe/<paket>.md`) früh schreiben und fortschreiben, nicht erst am Ende.

