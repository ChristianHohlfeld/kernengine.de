# Übergabe Paket p – 18.11.2026, 23:36
Maßgeblich: $S/stand/p.md. Zweig work-p, HEAD 85d60a1d gepusht (aede1c3e: format_arg(const char*), CI war rot) (work-zusammen eingemischt).
Commits seit Teil 1: 95ada08/2416329 E2.1 · e7538f1 E2.2 · 133bb74 Zeit-Test · c064f234 E2.1 Schritt 2 (+,−,×, logic::Sum) · 25a7318f Fix text_format
(Befund k, eigener Commit) · E3-A Aufrufweg (vor 8a028f9c) · E10 Fixed-Literal am Eingang + Leser-Fix Int-Formel mit indiziertem Argument (vor 6c7b9c5f)
· Switch on Text ohne Liste (vor a53ce271).
Bewiesen: E2.1 Schritt 1 (engine 221509-17831 rc=0); vergleich 230429-15316 auf 8a028f9c: 12/12 ok in beiden Durchgängen, keine Abweichung
(E2.1, E2.2, Schritt 2, E3-A); E3-A cost controls_cpp 44→10, combat_rules_cpp 69→11.
Bewiesen bis 85d60a1d: engine 002417-11420 rc=0, vergleich 230429-15316 + 010659-18840 sauber, CI native grün. BEREIT ZUM MERGEN gemeldet.
Offen (Block D, nächster Schritt):
1. Rest-Ursachen messen/straffen ohne Textänderung: Schleifenwächter (kern_pass: Zähler im Speicher je Durchlauf, schon schlank), whole_add (hat
   Schnellweg), Bytes als List<Int>, Listenprüfung (E2.3 neu messen). Bench steht unverändert: 25/29 über 1,10 (stand/p.md Tabelle).
2. Größte Reste: nostr_mulm 7,8× (Int64-Knoten, `i - 1.0` als Float-Formel an der Wurzel), settings_escape 7×, sha256 6,7×, qr 5× – nächster
   Schreibweise-Schritt wäre Wurzel +,−,× / Vergleich in Int-Formeln (braucht Bedingung „Ausgang nur in Int“ am Graphen) → Entwurf an main.
3. cost-Rest controls_cpp/combat_rules_cpp: Struktur-Argumente über logic::Value – eigener Entwurf.
Werkzeuge ($S/p): mk.sh, chk (CHK_FULL=1), chk2 (Lesefehler), chk3 (Fixpunkt, CHK3_OUT=datei), glue.cpp (Klasse+Glue eines Falls), runcase.cpp,
runfmt.cpp (ASan mit g++). Bench: `TMPDIR=$S/shared-tmp KERN_JOBS=2 BENCH_DIR=$S/p/b2 flock $S/heavy2.lock flock $S/heavy3.lock tools/blueprint_bench.sh $S/p/lvl/spawncity.klevel --check 1.10 15`.
