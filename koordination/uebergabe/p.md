# Übergabe Paket p – 18.11.2026, 23:36
Maßgeblich: $S/stand/p.md. Zweig work-p, HEAD a53ce271 gepusht (work-zusammen eingemischt).
Commits seit Teil 1: 95ada08/2416329 E2.1 · e7538f1 E2.2 · 133bb74 Zeit-Test · c064f234 E2.1 Schritt 2 (+,−,×, logic::Sum) · 25a7318f Fix text_format
(Befund k, eigener Commit) · E3-A Aufrufweg (vor 8a028f9c) · E10 Fixed-Literal am Eingang + Leser-Fix Int-Formel mit indiziertem Argument (vor 6c7b9c5f)
· Switch on Text ohne Liste (vor a53ce271).
Bewiesen: E2.1 Schritt 1 (engine 221509-17831 rc=0); vergleich 230429-15316 auf 8a028f9c: 12/12 ok in beiden Durchgängen, keine Abweichung
(E2.1, E2.2, Schritt 2, E3-A); E3-A cost controls_cpp 44→10, combat_rules_cpp 69→11.
Läuft: `work-p-engine-232958-27563` (engine auf 6c7b9c5f) – mit `$S/mag.sh warten <name> 4` / `holen`; CI run 44407872390 (auf 63467c0b).
Offen danach:
1. engine grün → „BEREIT ZUM MERGEN“ (25a7318f text_format separat, Rest bis 6c7b9c5f) in stand/p.md + SendMessage main.
2. EIN vergleich über src/drone.cpp (Firmware Arondight45: cost Runtime_reset/new muss weg sein – E10) + src/controls.cpp, src/combat_rules.cpp; dann
   CI auf dem Kopf (`gh workflow run ci.yml --ref work-p`).
3. Bench neu messen (Tabelle stand/p.md); D-Rest: Schleifenwächter (kern_pass schon schlank – Zähler im Speicher je Durchlauf), whole_add (schon
   ein Schnellweg), Bytes als List<Int>, Listenprüfung (E2.3 neu messen). Restliche cost-Zeilen in controls_cpp/combat_rules_cpp: Struktur-Argumente
   über logic::Value (to_value/from_outside) – eigener Entwurf nötig.
Werkzeuge ($S/p): mk.sh, chk (CHK_FULL=1), chk2 (Lesefehler), chk3 (Fixpunkt, CHK3_OUT=datei), glue.cpp (Klasse+Glue eines Falls), runcase.cpp,
runfmt.cpp (ASan mit g++). Bench: `TMPDIR=$S/shared-tmp KERN_JOBS=2 BENCH_DIR=$S/p/b2 flock $S/heavy2.lock flock $S/heavy3.lock tools/blueprint_bench.sh $S/p/lvl/spawncity.klevel --check 1.10 15`.
