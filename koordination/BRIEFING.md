# Hintergrund für Agenten, die kalt starten

**Zuerst: Lies $S/ANSATZ.md ganz und danach kern/games/spawncity/BLUEPRINTS.md ganz. Der Ansatz muss vollständig verstanden sein, bevor du etwas änderst.**

Repo: GitHub ChristianHohlfeld/kernengine.de (gh ist angemeldet). Antworte und berichte auf Deutsch.

**Das Projekt.** SpawnCity (kern/games/spawncity, ein Drohnen-Spiel, live auf kernengine.de) läuft auf
der eigenen Engine Kern (kern/). Ziel: Das ganze Spiel wird zu Blueprints (der visuellen Skriptsprache
von Kern), und zwar **generiert, nicht von Hand**: Blueprint == C++ auf Basis einer erlaubten
Teilmenge. Jeder Blueprint hat einen C++-Text (die „Notation", kern/docs/blueprint_cpp.md – lies die
Abschnitte, die dein Paket berühren) und läuft in drei Stufen: Interpreter, JIT und als übersetzte
C++-Klasse. Der Konverter `tools/cpp_to_blueprint.py` liest das C++ des Spiels (über clang-AST) und
schreibt daraus Blueprints, soweit das C++ in der erlaubten Teilmenge ist; alles andere „wartet" mit
einem Grund. Für jede geschriebene Funktion schreibt er einen Test (kern/games/spawncity/tests/converted/),
der sie **bit für bit** mit dem Quelltext vergleicht und prüft, dass die Klasse nie öfter Speicher
anfordert als der Quelltext.

**Stand:** 602 von 1.851 Funktionen sind geschrieben. Die Wartegründe der letzten vollen Konvertierung
stehen in `$S/p_fixed_convert.log` (Zeilen `    Klasse::funktion (Zeilen): <Grund>`, darüber eine Zeile
mit dem Einheiten-Namen; am Ende die Zusammenfassung „what the others wait for").

**Werkzeuge** (`S=/home/claude/kernengine.de/koordination`):
- `$S/why_struct.py`: zeigt jedes Feld einer Klasse ohne Blueprint-Typ mit Grund (Aufruf in FORK_RULES.md).
- Konverter-Probelauf auf wenigen Dateien (Aufruf in FORK_RULES.md).

**Zwei Antworten pro wartender Stelle** (kern/games/spawncity/BLUEPRINTS.md): eine allgemeine Fähigkeit
der Sprache/des Konverters – oder die Stelle des Spiels wird in erlaubtes C++ gleicher Kosten und
bit-genau gleichen Verhaltens umgeschrieben. Das C++ des Spiels darf umgeschrieben werden.

**Harte Regeln des Auftraggebers** (wörtlich): „Keine Kompromisse", „nicht hacky, sauber", „Keine
Sonderregelung", „kein caching", „immer zwischenschritte regelmäßig pushen", „Tests am Ende wenn
alles fertig ist" – also in dieser Phase **keine Testläufe**, nur die schnellen Kontrollen aus
FORK_RULES.md. Box3D nie anpassen. Die Physik bleibt, wie sie ist.

Lies danach `$S/FORK_RULES.md` – die Regeln dort gelten vollständig. Die Worktrees deiner Pakete
existieren schon (siehe dein Auftrag); lege keinen neuen an. Ein früherer Agent hat dort schon
angefangen (Commit „wip: Paket … – unterbrochen"): Sieh dir mit `git -C <worktree> show --stat HEAD`
und `git diff HEAD~1` an, was er getan hat, und mach dort weiter – prüfe seine Änderungen kritisch,
sie sind ungeprüft.
