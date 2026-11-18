# Auftrag für Claude CLI auf mag: rückwirkender Differenztest (Paket „r“)

Du arbeitest direkt auf mag (56 Threads, viel RAM). SpawnCity hat Vorrang (Chris, 18.11.): nimm bis zu 24 Kerne (`KERN_JOBS=24`), rechne im
Arbeitsspeicher (`/dev/shm/spawncity-diff-$USER`, sonst `~/spawncity-diff`). Keine systemweiten Installationen, MAG-Stack nicht anfassen.
Daneben läuft `~/spawncity-auftrag/lauf2.sh` (die Warteschlange der anderen Agenten) – nicht anfassen, nicht beenden.
Falls du diesen Auftrag über Grok Bot schon begonnen hast (`~/spawncity-diff`): mach dort weiter, nach diesen Regeln.

Repo `ChristianHohlfeld/kernengine.de`, Branch `mag/differenztest` von `origin/work-r` (dort steht schon das zusammengeführte Werkzeug: tools/source_diff.py und tests/source_diff.h aus work-f2 und work-f1).
Ein Vorgänger hat begonnen: lies `pakete/UEBERGABE_r.md` (auf diesem Branch mag/auftrag) zuerst – Umfang (76 Einheiten), erste Befunde je Datei, die sieben fehlenden allgemeinen Regeln des Werkzeugs und die offene Grundsatzfrage (alter Text gegen heutige fremde Header). Listen und Aufrufskript: `pakete/r/`. Nur auf diesen Branch pushen (nie main/work, kein Force-Push).
Commit-Nachrichten deutsch. `clang++` heißt hier `clang++-18` (`export CLANGXX=clang++-18`).

Lies zuerst, ganz: in `kern/games/spawncity/BLUEPRINTS.md` den Abschnitt „Der Weg ab hier“ mit „Kursprüfung 18.11.2026“ (bindend), den Kopf von
`tools/source_diff.py`, und als Beispiele `kern/games/spawncity/tests/lowered/test_settings_lowered.cpp`, `test_city_jobs_lowered.cpp`.

## Ziel
Für jede Datei unter `kern/games/spawncity/src/`, die sich zwischen `origin/main` und `origin/work` geändert hat
(`git diff --name-only origin/main origin/work -- kern/games/spawncity/src`), entsteht mit
`tools/source_diff.py --base origin/main --test tests/lowered …` ein Differenztest: alte gegen neue Fassung jeder geänderten Funktion, mit
demselben gewürfelten Zustand und denselben Argumenten, Bit für Bit verglichen (Rückgabe, jeder Parameter, das Objekt, der Zustand der Datei),
und gezählt, dass die neue Fassung nie öfter Speicher anfordert. `settings` und `city_jobs` sind gegen einen späteren Ausgangsstand bewiesen –
auch sie gegen `origin/main`.

## Regeln
- Keine Spiel-Quelle ändern (die gehört anderen Agenten, die gerade daran arbeiten).
- Kann das Werkzeug ein Paar nicht bilden, erweitere das Werkzeug allgemein: eine Regel, keine Namensliste. Auf `origin/work-f2` ist
  `tools/source_diff.py` schon erweitert – hol dir diese Fassung zuerst (nur diese Datei), damit nicht doppelt gebaut wird.
- Keine Toleranz, kein Ausnehmen von Funktionen. Zahlen der Sprache: 64 Bit, kein Float32.
- Findet ein Test einen echten Unterschied zwischen alt und neu: nichts reparieren. Schreib ihn sofort in `ABWEICHUNGEN.md` im Wurzelordner des
  Branches (Datei, Funktion, ein Beispielaufruf mit beiden Ergebnissen), push, mach weiter.
- Bewusste Änderungen des Verhaltens seit main (entfernte Caches – gleiche Ergebnisse, andere Kosten –, Fehlerbehebungen) erkennst du am Commit,
  der sie machte (`git log -L`, `git log -- datei`): liste sie in `ABWEICHUNGEN.md` mit Commit und Begründung; entscheide nicht selbst, ob sie
  in Ordnung sind.
- Tests bauen und laufen wie `kern/games/spawncity/tests/run_tests.sh lowered` (Filter auf die Tests in `tests/lowered`).
- Datei für Datei, kleine Commits, nach jeder Datei pushen – der Koordinator (Claude in der Cloud-Sitzung) liest den Branch mit.

## Bericht (als `BERICHT.md` im Wurzelordner des Branches, laufend aktualisiert)
Je Datei: Zahl der Paare, grün?; jede Abweichung; was am Werkzeug dazukam; was noch offen ist und warum.

## Aufräumen am Ende
`rm -rf /dev/shm/spawncity-diff-$USER ~/spawncity-diff`

## Nachtrag 16:40 (Koordinator) – die Grundsatzfrage ist entschieden: zwei Bauten statt zwei Namensräume
Die offene Frage aus `pakete/UEBERGABE_r.md` (alter Text gegen heutige fremde Header) und ein zweiter Befund (Paket g1: Methoden von `Game` liegen
in ~40 Dateien, die Klasse in game.h; gleichnamige anonyme Helfer verschiedener Dateien stoßen in einem Namensraum zusammen) haben dieselbe
saubere Lösung – baue sie als die allgemeine Form des Werkzeugs, du bist ab jetzt der eine Besitzer von `tools/source_diff.py` und
`kern/games/spawncity/tests/source_diff.h`:
- **Jede Seite ist das ganze Programm in ihrem Stand.** Die alte Seite wird aus dem ganzen Quellbaum des Ausgangs-Commits übersetzt
  (`git worktree`/`git archive` von `--base`), die neue aus dem heutigen Baum – zwei getrennte Bauten (zwei Programme oder zwei Bibliotheken),
  keine gemeinsamen Namensräume, keine gemischten Header.
- Der Treiber je Funktionspaar ist derselbe Text, einmal gegen jeden Bau übersetzt: er würfelt Zustand und Argumente aus demselben Keim
  (dieselben Ziehungen wie die konvertierten Tests, `source_diff.h`), ruft die Funktion und schreibt alles Verglichene als Bytes heraus
  (Rückgabe, jeder veränderbare Parameter, das Objekt – auch `Game` –, der Zustand der Datei, die Zahl der Speicheranforderungen, die Marke
  „C++ lässt offen“). Verglichen werden die beiden Byteströme, Aufruf für Aufruf. Wo sich die Gestalt eines Typs zwischen alt und neu geändert
  hat (ein Feld wurde zum Parameter, eine Dateivariable zum Feld einer Struktur), ordnet die Paarregel die Teile einander zu – wie bisher: eine
  Regel, keine Namensliste.
- Was die Funktion von der Engine ruft, geht auf beiden Seiten an dieselbe Engine im selben Stand (die Engine von heute; hat sich eine
  Engine-Funktion seit `--base` geändert, die der alte Text ruft, ist das eine Abweichung, die du in `ABWEICHUNGEN.md` nennst).
- Die bisherigen Tests in `tests/lowered` (settings, city_jobs und die von Paket f1: mvt, tiles, terrain, city_stream, interiors, facades,
  water_look auf `origin/work-f1`) müssen in der neuen Form dasselbe beweisen.
- Andere Agenten (g1, g2, f1, f2) schreiben weiter Quellen um; ihr Beweis läuft dann über dein Werkzeug mit `--base` = Commit vor ihrer
  Änderung. Halte den Aufruf einfach und dokumentiere ihn im Kopf von `tools/source_diff.py`; pushe das Werkzeug früh auf `mag/differenztest`,
  auch bevor alle 76 Einheiten durch sind – sag es in `BERICHT.md`, sobald die Game-Dateien damit gehen.
