# Belege der Absenkungen (Differenztests „zwei Bauten“)

Ein Differenztest in der Form „zwei Bauten“ (tools/source_diff.py --builds, tests/lowered/builds/<datei>) gilt für den Stand
direkt nach seiner Absenkung: er läuft einmal auf einem Hilfszweig `work-beleg-<commit>` (dieser Commit + Treiber), mit
`gh workflow run ci.yml --ref work-beleg-<commit> -f beleg=<datei>`. Grün = Beleg. Der Treiber bleibt nicht in der Suite.
Ohne Zeile hier geht keine Absenkung nach work (GEMEINSAM.md, Nachtrag 20:45).

| Absenkung (Commit, Datei, Funktionen) | Basis | Hilfszweig | CI-Lauf | Ergebnis |
|---|---|---|---|---|
| a5a157f2 + 8b27771d, game.cpp, Game::stop, Game::~Game | 8971e5c3 | work-test 1860c80d (Treiber builds/game) | mag-Lauf 1860c80d (lowered_game) | grün: ~Game 300/300 gleich; stop mit 5 bewusst beendeten Feldern (--ends, BLUEPRINTS.md „Bewusste Abweichungen“) |
