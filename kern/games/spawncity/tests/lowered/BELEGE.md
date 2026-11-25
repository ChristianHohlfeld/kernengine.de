# Belege der Absenkungen (Differenztests „zwei Bauten“)

Ein Differenztest in der Form „zwei Bauten“ (tools/source_diff.py --builds, tests/lowered/builds/<datei>) gilt für den Stand
direkt nach seiner Absenkung: er läuft einmal auf einem Hilfszweig `work-beleg-<commit>` (dieser Commit + Treiber), mit
`gh workflow run ci.yml --ref work-beleg-<commit> -f beleg=<datei>`. Grün = Beleg. Der Treiber bleibt nicht in der Suite.
Ohne Zeile hier geht keine Absenkung nach work (GEMEINSAM.md, Nachtrag 20:45). „Level aus“: das Level, mit dem beide Bauten
laufen (nicht Teil der Absenkung) – der Commit selbst, oder der volle Lauf, aus dem es im Hilfszweig steht.

| Absenkung (Commit, Datei, Funktionen) | Basis | Hilfszweig | Level aus | CI-Lauf | Ergebnis |
|---|---|---|---|---|---|
| a5a157f2 + 8b27771d, game.cpp, Game::stop, Game::~Game | 8971e5c3 | work-test 1860c80d (Treiber builds/game) | 1860c80d | mag-Lauf 1860c80d (lowered_game) | grün: ~Game 300/300 gleich; stop mit 5 bewusst beendeten Feldern (--ends, BLUEPRINTS.md „Bewusste Abweichungen“) |
| a74fa414, game_life.cpp, Game::life_update | 85a8ac88 | work-beleg-a74fa414 | a74fa414 | 45689737224 | grün: 300/300 Aufrufe verglichen |
| 31c74cde (work-einstieg2), game.cpp, register_node → Game::register_node | 082a9633 | – (die Form kennt das Paar „frei → Methode“ nicht; der Aufrufer Game::start ist unverändert) | – | Vergleichstest von Game::start im vollen Lauf + e2e (Entscheidung Koordinator) | offen bis Kandidat grün |
| 801fad42, game_town.cpp, Game::minimap_json | 765f6371 | work-beleg-801fad42 | 801fad42 | 45692907738 | **rot (Abweichung)**: 300 verglichen (35 offen), 1 von 300 weicht ab – Aufruf 308: das neue Programm überlebt einen Aufruf nicht, dem das alte einen Wert gibt (an main gemeldet) |
| 202482d9, city_stream.cpp, CityStream::update | 0aea3dec | work-beleg-202482d9 | 202482d9 | 45693026565 | **rot (Abweichung)**: 300 verglichen (75 offen), 115 von 300 weichen ab – sein Objekt: near_, ms_, worst_, worst_at_, vis_ms_, stats_ (z. B. Aufrufe 0, 3, 10; an main gemeldet) |
