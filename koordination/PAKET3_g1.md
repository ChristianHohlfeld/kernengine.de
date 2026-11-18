Paket „g1“. Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz und was es nennt. Sperre: heavy3.lock. Zweig work-g1, Worktree /home/claude/w-g1.

Deine Dateien des Spiels (nur diese änderst du): src/game.cpp, game_controls.cpp, game_look.cpp, game_ui.cpp, game_details.cpp, game_vr.cpp, game_guided.cpp, game_gaps.cpp, game_barricades.cpp, game_lamps.cpp (und game.h nur für Deklarationen dieser Dateien). Gründe, die im Quelltext liegen (Zeiger, Iteratoren, Konstruktoren, tuple/variant, break in der Mitte …): absenken mit Differenztest oder allgemeine Fähigkeit. Gründe „its test cannot compare it yet“ gehören den Paketen t1/t2 – nicht anfassen, im Bericht nennen, falls sie nach deren Arbeit an deiner Quelle liegen.

Deine Liste: grep -E "^Game/game_(cpp|controls_cpp|look_cpp|ui_cpp|details_cpp|vr_cpp|guided_cpp|gaps_cpp|barricades_cpp|lamps_cpp)|^Game/game\b|^game_cpp" $S/f9_roots.txt (Grundursachen) und $S/f9_waiting.txt (alles Wartende, auch Folgefehler).
Ziel: in deinen Dateien wartet nichts mehr – jede Funktion geschrieben UND im Test verglichen.
