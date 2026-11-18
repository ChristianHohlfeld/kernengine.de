# Auftrag für Claude CLI auf mag: rückwirkender Differenztest (Paket „r“)

Du arbeitest direkt auf mag (56 Threads, viel RAM). SpawnCity hat Vorrang (Chris, 18.11.): nimm bis zu 24 Kerne (`KERN_JOBS=24`), rechne im
Arbeitsspeicher (`/dev/shm/spawncity-diff-$USER`, sonst `~/spawncity-diff`). Keine systemweiten Installationen, MAG-Stack nicht anfassen.
Daneben läuft `~/spawncity-auftrag/lauf2.sh` (die Warteschlange der anderen Agenten) – nicht anfassen, nicht beenden.
Falls du diesen Auftrag über Grok Bot schon begonnen hast (`~/spawncity-diff`): mach dort weiter, nach diesen Regeln.

Repo `ChristianHohlfeld/kernengine.de`, Branch `mag/differenztest` von `origin/work`. Nur auf diesen Branch pushen (nie main/work, kein Force-Push).
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
