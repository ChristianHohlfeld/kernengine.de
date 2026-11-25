# STAND – zuerst und nur dies lesen (UEBERGABE.md ist Archiv, Verlauf bis 23.11.)

Fortgeschrieben vom Koordinator; höchstens 60 Zeilen. Antworte Chris deutsch, kurz, nur Ergebnisse. Seine E-Mail ist privat.

## Ziel und Regeln
Ziel, Regeln, Konzept: UEBERGABE.md Abschnitte 1–4 und im Repo kern/games/spawncity/BLUEPRINTS.md („Der Weg ab hier“,
„Stand 25.11.2026“, „Kursprüfung 18.11.2026“). Chris, 25.11.: fertig werden, Frist zwei Tage; die Geschwindigkeit der Klasse wird
am Ende optimiert und ist kein Tor fürs Umschalten; der Koordinator entscheidet selbst und darf selbst beheben.

## Stand 25.11.2026, 16:30
- **Abschluss 1 ist live:** `main` 9ac74916 (Squash) = `work` eb5789ac. 1.763 von 1.847 Funktionen als Blueprints, 1.718 Bit für
  Bit verglichen. Belege: mag `work-abschluss1-voll-130718-2` (alles rc=0), CI 45621562516 (alle Jobs, 24 Teil-Läufe grün).
- Heute behoben (alles in work): Speicherzähler zählte __cxa_atexit der C-Laufzeit mit (der eine rote lowered-Test); das gebaute
  Spiel trägt den Level-Ordner „Source“ nicht (embed_level.py --without; der Web-Bau scheiterte am 38-MB-Level); CI-Zeitlimits
  (Bau der Spiel-Tests 120 min, Teil-Lauf 75); e2e öffnet das Level, wie das Spiel es trägt (KERN_E2E_WHOLE_LEVEL=1: das ganze).
- **Offen bis „fertig“:** (1) 126 Grundursachen / 4.798 Zeilen warten (Liste: wartet_abschluss1.txt im Scratchpad des
  Koordinators, sonst aus convert.log des Laufs oben): 31 Funktionen am Test-Schreiber, 13 Zeiger-Fälle in Game, Referenzen,
  Engine-Objekte/Plätze, Rückrufe; (2) Differenztest für jede Umschreibung seit fb6cf4c; (3) Umschalten (Stufe 5), nie bewiesen;
  (4) der Editor öffnet das ganze Projekt (Stufe 6, e2e hing nach game_world); (5) 711 cost:-Zeilen und die Zeit der Klasse
  (qr encode ~17×) – am Ende; (6) Abschluss: Squash nach main, Changelog.

## Laufende Aufträge (Opus 5.5, mittlere Stufe; Briefe im Scratchpad: GEMEINSAM.md + AUFTRAG_<name>.md)
| Auftrag | Zweig | Besitz |
|---|---|---|
| test | work-test | Test-Schreiber, Kreis-Funktionen, Differenztest „zwei Bauten“ |
| game | work-game2 | alle src/game*.cpp/.h: wartende Funktionen von Game |
| engine | work-engine4 | engine_api.py, Engine-Knoten: work-d / work-d-index einmischen, Plätze, Rückrufe (E-c2) |
| umschalten | work-umschalten | Stufe 5: Mechanismus (Vorarbeit work-s5), Konverter liest Blueprint-Klassen, Dateien umschalten |
| tempo | work-tempo2 | Läufe auf mag schneller (Konverter-Prüfrunden, „Blueprint Game“ je Test); Beweis: byteweise gleich |
Jeder meldet „bereit zum Mischen“ mit Beleg; der Koordinator mischt sofort nach `work` (Beleg ansehen: when alle rc=0;
nur eigene Dateien; `python3 tools/engine_api.py` ohne Unterschied; Syntax). Volle Läufe und CI auf `work` an Meilensteinen.

## mag
Schleife `~/spawncity-auftrag/lauf2.sh` läuft (25.11. geprüft). Auftrag = Datei jobs/<name> auf Branch mag/auftrag (erste Zeile:
voll|konvert|probe|vergleich|spiel|engine|alle <ref> …), Ergebnis auf mag/ergebnis-<name> (mag-log/when mit rc). Voller Lauf
~100 min (Konvertierung ~55, Suite ~50); `spiel <ref> lowered` ~20 min. Fetch-Regel: `+refs/heads/mag/*:refs/remotes/origin/mag/*`.

## Fallen
- CI: `deploy` hängt nur an `build` und läuft nur auf main – vor jedem Push auf main alle Jobs auf dem Stand grün haben.
- Level und tests/converted im Repo müssen aus dem vollen Lauf GENAU dieses Stands stammen (Commit 1 des Ergebnis-Zweigs).
- Wochenkontingent: Reset mittwochs 13:00; am 20.11. und 22.11. war es vor dem Ende der Woche leer.
