# Stand (Koordinator, 25.11.2026 20:45) – kurz; Archiv: UEBERGABE.md
Ziel „erste 100 %“: jede Funktion Blueprint + verglichen, Spiel läuft aus Blueprints (EIN Schaltschritt: Source → Program, src/*.cpp weg), Editor öffnet alles,
CI grün, main, Deploy, Changelog. Geschwindigkeit danach.

## Bewiesen / live
- main 9ac74916 = Abschluss 1 (1.763/1.847 Funktionen, 158 Blueprints), CI 45634699991 grün, deployt.
- work 05b10cd1: + Editor öffnet das ganze Projekt (175 Blueprints, Game 15,5 s; Blätter, CI 45662141502 grün), CI-Jobs probe (GitHub-Proben), fetch-depth 0.

## Läuft
- mag: Kandidat 2 (work-kandidat-1755, seit 18:22, alter engine-Stand → nur Zahlen), Kandidat 3b (work-kandidat-2045-voll-204140-3518: engine4, test, rest,
  game2, game-b, game-c, umschalten; CI 45687732838), tempo-Basislauf eb5789ac-konvert-200652. cap 126 (voll zählt 56). mag nur für Kandidaten.
- Skripte: $S/mag.sh, $S/ci_nach_konvert.sh <auftrag> (CI auf Ergebniszweig nach der Konvertierung). Regeln der Agenten: $S/GEMEINSAM.md (Nachträge bis 20:45).

## Agenten (Opus 5.5 medium; Aufträge $S/AUFTRAG_<name>.md, Stand $S/stand_<name>.md)
- test (work-test): Test-Schreiber – „cannot compare“ (explosion, player_position, combat_blast, rules, touch_down).
- belege (work-belege): Differenztests „zwei Bauten“ als Beleg je Commit → tests/lowered/BELEGE.md; GameWorld im Treiber; CI-Eingang beleg.
- game-b (controls, look, events), rest (game_ui, game_vr + Rest), einstieg2 (main.cpp/SpawnCityApp, Level-Seite, Game::start/update/stop),
  umschalten-b (Gesamtprobe Stufe 2: linken+starten; Kleber je Klasse), umschalten-c (Exporte; 96-Felder-Grenze; audio ohne Kopie),
  tests-a / tests-b (21 Spieltests auf Klassen; Projekt-Einstellungen work-tests-b-projekt), tempo (work-tempo2: Testrahmen belegt; Bäume je Lauf wartet auf Basislauf).
- beendet: game (work-game2), game-c, engine (work-engine4 fb4d73ec), umschalten (1d14d513), einstieg.

## Entscheidungen heute
- Schalten alles auf einmal (Nachtrag 19:00); umgeschaltet bleiben qr_code, person_hit, combat_audio; gps_live zurück in Source.
- Game::stop: bewusste Korrektur (5 Felder), Test hält genau das fest. Klang-Proben einmal nach f32 (Beleg offen). Kein Abschwächen von Spieltests;
  64/32-Abweichungen nach $S/werte_64.md, Entscheidung je Fall.
- Absenkung nach work nur mit Zeile in BELEGE.md. Offene Belege: game_town 8d59c94f, game.cpp 801846e2/f18a92df, game_life a74fa414, city_stream, city_jobs,
  game_controls 2d84671a, game_look, main.cpp (SpawnCityApp, Projekt-Einstellungen, Exporte).
- Befund: Konverter ist kein Fixpunkt über sein eigenes Level (Ids/Lagen von Game) – $S/befund_fixpunkt.md, nach dem Abschluss.

## Nächste Schritte
1. Kandidat 3b: Zahlen, Warteliste → $S/kandidat3_wartet.txt, an test/game-b/rest/einstieg2 verteilen. Grün + Belege → work.
2. Kandidat 4: + umschalten-b, umschalten-c, tests-a/b, tempo2, einstieg2, belege.
3. Gesamtprobe Stufe 2 ($S/gesamtprobe.md) → Restliste bis zum Schaltschritt. Dann schalten, voller Lauf, CI, main, Changelog.
