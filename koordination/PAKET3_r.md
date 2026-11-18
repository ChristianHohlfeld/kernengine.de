Paket „r“ (rückwirkender Differenztest). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz, samt Nachtrag 15:40 (mag); dann den Kopf von
tools/source_diff.py und kern/games/spawncity/tests/lowered/. Zweig work-r, Worktree /home/claude/w-r.

Ziel: Für jede Datei unter kern/games/spawncity/src/, die sich zwischen origin/main und origin/work geändert hat
(git diff --name-only origin/main origin/work -- kern/games/spawncity/src), entsteht mit tools/source_diff.py --base origin/main --test tests/lowered …
ein Differenztest, der alte gegen neue Fassung jeder geänderten Funktion Bit für Bit vergleicht und zählt, dass die neue nie öfter Speicher
anfordert. settings und city_jobs sind gegen einen späteren Ausgangsstand bewiesen – auch sie gegen origin/main.
Regeln: Keine Spiel-Quelle ändern. Kann das Werkzeug ein Paar nicht bilden, erweitere das Werkzeug allgemein (eine Regel, keine Namensliste) –
Paket f2 hat tools/source_diff.py auf seinem Zweig ebenfalls erweitert (origin/work-f2): hol dir das zuerst (merge origin/work-f2 nur für diese
Datei bzw. cherry-pick), damit ihr nicht doppelt baut. Keine Toleranz, kein Ausnehmen. Findet ein Test einen echten Unterschied zwischen alt und
neu: nichts reparieren, sondern sofort melden (SendMessage an main: Datei, Funktion, ein Beispielaufruf mit beiden Ergebnissen) und weitermachen.
Bewusste Änderungen des Verhaltens seit main (es gab welche, z. B. entfernte Caches in terrain – gleiche Ergebnisse, andere Kosten – oder
Fehlerbehebungen) erkennst du am Commit, der sie machte (git log -L / git log -- datei): liste sie mit Commit und Begründung, entscheide nicht selbst,
ob sie in Ordnung sind. Tests bauen und laufen: `$S/mag.sh spiel origin/work-r lowered`. Arbeite Datei für Datei, kleine Commits, pushen.
Bericht: je Datei Zahl der Paare, grün?; jede Abweichung; was am Werkzeug dazukam.
